/****************************************************************************
 * \file libsshell.cpp
 * Implementation of the class gmShellLibssh
****************************************************************************/

#include <cstring>
#include <stack>
#include <gmd/utils.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>
#include <gmd/ffile.h>
#include <gmd/dir.h>
#include <logexc.h>

#include <libssh/libssh.h>
#include <libssh/callbacks.h>

#include "jobmngr/libsshell.h"


class scp_read_thread {
  ssh_scp m_scp;
  void* m_buffer;
  size_t m_size;
  size_t* m_nread;

public:
  scp_read_thread(ssh_scp scp, void* buffer, size_t size, size_t* nread)
    : m_scp(scp), m_buffer(buffer), m_size(size), m_nread(nread)
  {}
  
  void operator()() {
    *m_nread = ssh_scp_read(m_scp, m_buffer, m_size);
  }
};


// Default Unix permissions for files/dir created on the remote host
enum { DEFAULT_FILE_PERM = 0644, DEFAULT_DIR_PERM = 0755};


// ============================================================================
// SSH_Data structure
// ============================================================================
class SSH_Data {
public:
  ssh_session session;
  ssh_scp scp;
  ssh_channel channel;
  ssh_callbacks_struct cb;

  static int auth_callback(const char *prompt, char *buf, size_t len,
                           int echo, int verify, void *userdata)
  {
    return SSH_AUTH_DENIED;
  }
                       //(ssh_session session, int priority, const char *message, void *userdata)
  static void log_callback(ssh_session session, int priority, const char *msg, void *userdata)
  {
      (void)userdata;
      LOGJOBMSG4( fmt_s( "[libssh][%i][%p] %s", priority, session, msg ? msg : "" ).c_str() );
  }

  SSH_Data() : session(NULL), scp(NULL), channel(NULL) {
    std::memset(&cb, 0, sizeof(cb));
    ssh_callbacks_init(&cb);
    cb.auth_function = & auth_callback;
    cb.log_function  = & log_callback ;
    cb.userdata      = this;
    ;
  }
};


// ============================================================================
// DirTraverser class
// ============================================================================
class DirTraverser : public gmdDirTraverser
{
protected:
  gmShellLibssh& shell;
  gmdString curdir, relpath;
  unsigned flags;

  int update_curdir(const gmdString& path);

public:
  DirTraverser(gmShellLibssh& shell_) : shell(shell_) {}

  void Init(const gmdString& dir, unsigned flags_) {
    curdir = dir;
    flags = flags_;
  }

  virtual gmdDirTraverseResult OnFile(const gmdString& path);
  virtual gmdDirTraverseResult OnDir(const gmdString& path);
};


int DirTraverser::update_curdir(const gmdString& path) {
  // Find the maximal common part of the path
  while( !path.StartsWith(curdir + DIR_SEPARATOR, &relpath) ) {
    // Leaving the previous directory
    if(ssh_scp_leave_directory(shell.ssh->scp) != SSH_OK)
      return shell.set_ssh_err(gmShell::COPY_ERROR);
    gmdFileName fn(curdir);
    curdir = fn.GetPath();
  }
  return 0;
}


gmdDirTraverseResult DirTraverser::OnFile(const gmdString& path){
  int res = update_curdir(path);
  if(!res) res = shell.send_file(path, flags);
  return res ? gmdDIR_STOP : gmdDIR_CONTINUE;
}


gmdDirTraverseResult DirTraverser::OnDir(const gmdString& path) {
  int res = update_curdir(path);

  // Entering a new directory
  if(!res) {
    res = (ssh_scp_push_directory(shell.ssh->scp, relpath, DEFAULT_DIR_PERM) != SSH_OK);
    if(res) shell.set_ssh_err(gmShell::COPY_ERROR);
  }
  if(!res) curdir = path;

  return res ? gmdDIR_STOP : gmdDIR_CONTINUE;
}


// ============================================================================
// gmShellLibssh class
// ============================================================================



gmShellLibssh::gmShellLibssh(pCSTR login, pCSTR host)
  : ssh(new SSH_Data()), connected(false)
{
  // Global parameters which could be read from XML
  InitParam("login", login);
  InitParam("host", host);
  InitParam("port", "22");
  InitParam("knownhosts", "");  // Path to 'knownhosts' file
  InitParam("host_accept", "known");  // KNOWN, NEW, CHANGED, ALL
  InitParam("password", "");  // Password for password based authorization
  InitParam("privkey", "");  // Path to the private key
  InitParam("privkey_pass", "");  // Encrypted private key password
  InitParam("log_verbosity", "0");  // Log verbosity, 0 - disabled
  InitParam("ssh_timeout_ms", "");  // Timeout for ssh_connect() in miliseconds
  InitParam("conn_att_num", "2");  // Number of connection attempts
  InitParam("conn_retry_delay", "3000");  // Delay between attempts in miliseconds
  InitParam("read_att_num", "3");  // number of read attempts
}


gmShellLibssh::~gmShellLibssh() {
  Close();
  delete ssh;
}


void gmShellLibssh::Close() {
  if(ssh->channel) {
    ssh_channel_free(ssh->channel);
    ssh->channel = NULL;
  }
  
  if(ssh->scp) {
    ssh_scp_free(ssh->scp);
    ssh->scp = NULL;
  }

  if(ssh->session) {
    if(ssh_is_connected(ssh->session)) ssh_disconnect(ssh->session);
    ssh_free(ssh->session);
    ssh->session = NULL;
  }

  connected = false;
}


void gmShellLibssh::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "port")
    port = ParseIntParam(par_name, par_value, 1, 65535);
  else if(par_name == "log_verbosity")
    log_verbosity = ParseIntParam(par_name, par_value, 0, 4);
  else if(par_name == "ssh_timeout_ms")
    ssh_timeout_ms = par_value.IsEmpty() ? -1 : ParseIntParam(par_name, par_value, 0);
  else if(par_name == "conn_att_num")
    conn_att_num = ParseIntParam(par_name, par_value, 1);
  else if(par_name == "conn_retry_delay")
    conn_retry_delay =  ParseIntParam(par_name, par_value, 0);
  else if(par_name == "execute_timeout")
    execute_timeout =  ParseIntParam(par_name, par_value, 0);
  else if(par_name == "copy_timeout")
    copy_timeout =  ParseIntParam(par_name, par_value, 0);
  else if(par_name == "channel_read_inteval")
    channel_read_inteval =  ParseIntParam(par_name, par_value, 0);
  else if(par_name == "read_att_num")
    read_att_num =  ParseIntParam(par_name, par_value, 1);
  else if(par_name == "host_accept") {
    gmdString val = par_value.Upper();
    if(val == "KNOWN") host_accept = HOST_ACC_KNOWN;
    else if(val == "NEW") host_accept = HOST_ACC_NEW;
    else if(val == "CHANGED") host_accept = HOST_ACC_CHANGED;
    else if(val == "ALL") host_accept = HOST_ACC_ALL;
    else LOGJOBERR("Invalid 'host_accept' parameter value");
  }

  gmShell::set_param(par_name, par_value);
}


int gmShellLibssh::check_connection(bool on_error) {
  if(on_error || !connected || !ssh_is_connected(ssh->session))
    Close();  // transmission failed or the connection is broken
  
  if(connected) return 0;

  // Reconnect
  int rc;
  for(int i=0; i<conn_att_num; i++) {
    if(i > 0) {
      LOGJOBMSG( fmt("SSH connection failed: %s\nRetrying %d more times",
        ssh_get_error(ssh->session), conn_att_num - i) );
      if(conn_retry_delay) gmdMilliSleep(conn_retry_delay);
      Close();  // prevents 'ssh_socket_connect called unconnected'
    }
    if(!ssh->session) Open();
    rc = connect();
    if(rc <= 0) break;  // fatal error, error_code is set already
  }
  
  connected = (rc == 0);
  
  if(rc > 0) set_ssh_err(CONNECTION_ERROR);  // conn_att_num exceeded

  return rc;
}


// Strart the connection, authorize host and user
// Returns: =0 if OK, >0 if unexpected (network?) error, <0 auth error (fatal)
// For fatal errors calls set_err()
int gmShellLibssh::connect() {
  // Start connection
  if(ssh_connect(ssh->session) != SSH_OK) return 1;

  // Check the server host key
  // In future one can add here a callback to check
  // if the host key should be accepted
  int state = ssh_is_server_known(ssh->session);
  gmdString msg;
  switch (state) {
    case SSH_SERVER_FOUND_OTHER:
      if(host_accept < HOST_ACC_ALL)
        msg = "SSH: Server host key interfere with another host";
      break;

    case SSH_SERVER_KNOWN_CHANGED:
      if(host_accept < HOST_ACC_CHANGED)
        msg = "SSH: Server host key has been changed";
      break;

    case SSH_SERVER_FILE_NOT_FOUND:
    case SSH_SERVER_NOT_KNOWN:
      if(host_accept < HOST_ACC_NEW) {
        msg = "SSH: Host key is not found";
        break;
      }

    if (ssh_write_knownhost(ssh->session) < 0)
      msg = "SSH: Error writing to 'knownhost' file " + param["knownhosts"];
    break;
      
    case SSH_SERVER_ERROR:  // may be non-fatal network error
      return 2;
  }

  if( !msg.IsEmpty() ) {  // fatal auth error
    set_err(CONNECTION_ERROR, msg);
    return -2;
  }

  // User authorization
  int rc = SSH_AUTH_DENIED;
  if( !param["password"].IsEmpty() )
    rc = ssh_userauth_password(ssh->session, NULL, param["password"]);
  
  if(rc == SSH_AUTH_DENIED || rc == SSH_AUTH_PARTIAL) {
    pCSTR privkey = (pCSTR)param["privkey"].c_str();
    pCSTR keypass = (pCSTR)param["privkey_pass"].c_str();
    if(!keypass[0]) keypass = NULL;

    if(privkey[0])
      rc = ssh_userauth_privatekey_file(ssh->session, NULL, privkey, keypass);
    else
      rc = ssh_userauth_autopubkey(ssh->session, keypass);
  }
  
  if(rc == SSH_AUTH_ERROR) return 3;  // may be non-fatal network error

  if(rc != SSH_AUTH_SUCCESS) // fatal auth error
    set_err(CONNECTION_ERROR, "SSH: All authorization methods failed");

  return rc == SSH_AUTH_SUCCESS ? 0 : -3;
}


// Open SCP session
int gmShellLibssh::open_scp_session(int mode, pCSTR rempath) {
  int rc;

  for(int i=0; i<2; i++) {
    if( (rc = check_connection(i > 0)) ) return rc;

    // Allocate SCP session handle
    if(ssh->scp) ssh_scp_free(ssh->scp);
    ssh->scp = ssh_scp_new(ssh->session, mode, rempath);
    if(!ssh->scp) LOGJOBERR("Unable to allocate scp handle");

    if( (rc = ssh_scp_init(ssh->scp)) == SSH_OK ) break;
  }

  if(rc) set_ssh_err(COPY_ERROR);
  return rc;
}


// Read all output data from SSH channel after command execution
int gmShellLibssh::read_from_channel(gmdString& dst, int is_stderr) {
  const int bufsize = 32768;
  char buf[bufsize];
  int nread = 0;

  while(ssh_channel_is_open(ssh->channel) && !ssh_channel_is_eof(ssh->channel)) {
    nread = ssh_channel_read(ssh->channel, buf, bufsize-1, is_stderr);
    if(nread < 0) return 1;
    else if(nread == 0) break;

    buf[nread] = 0;
    dst += buf;
  }
    
  return 0;
}


// Send file 'path' using scp session
int gmShellLibssh::send_file(pCSTR path, unsigned flags) {
  const int bufsize = 32768;
  char buffer[bufsize];
  gmdFFile fi;

#ifdef _WIN32
  const int textmode = flags & TEXT;
#else
  const int textmode = 0;
#endif
  
  if( !fi.Open(path, textmode ? "rt" : "rb") )
    return set_err(NO_INPUT_FILE, gmdString(path) + ": no such file");

  // Get file size
  gmdFileOffset filesize64 = 0;
  if(textmode || !fi.SeekEnd())
    // Determine the file size by reading all data with LF conversion
    while( !fi.Eof() ) filesize64 += fi.Read(buffer, bufsize);
  else
    // Get the binary files size using fseek
    filesize64 = fi.Tell();

  size_t filesize = (size_t)filesize64;
  if(filesize != filesize64) {
    fi.Close();
    return set_err(COPY_ERROR,
      gmdString(path) + ": LibSSH doesn't support large files (>4Gb)");
  }

  // Extract file name and initiate transmission
  gmdFileName fn(path);
  if( ssh_scp_push_file(ssh->scp, fn.GetFullName(), filesize,
      DEFAULT_FILE_PERM) != SSH_OK )
    return set_ssh_err(COPY_ERROR);

  // Send data
  fi.Seek(0);
  size_t nwrite = 0;
  for(size_t i=0; i<filesize; i+=nwrite) {
    nwrite = fi.Read(buffer, bufsize);
    if(nwrite <= 0)
      return set_err(COPY_ERROR, gmdString(path) + ": error reading input file");

    if( ssh_scp_write(ssh->scp, buffer, nwrite) != SSH_OK )
      return set_ssh_err(COPY_ERROR);
  }

  return 0;
}


// Reseive file using scp session and save it to 'path'
int gmShellLibssh::receive_file(pCSTR path, unsigned flags, bool locpath_is_dir) {
  const int bufsize = 32768;
  char buffer[bufsize];
  gmdFFile fo;

  // Get file name from the remote side
  pCSTR filename = ssh_scp_request_get_filename(ssh->scp);
  if(!filename)
    return set_err(NO_INPUT_FILE, fmt("%s: no such remote file", path));

  gmdString lpath(path);
  if(locpath_is_dir) lpath += gmdString(DIR_SEPARATOR) + filename;

  size_t filesize = ssh_scp_request_get_size(ssh->scp);

  // Always open files in "wb" mode as "wt" leads to invalid conversion
  // \0xD \0xA => \0xD \0xD \0xA
  if( !fo.Open(lpath, "wb") ) {
    ssh_scp_deny_request(ssh->scp, "");
    return set_err(LOCAL_FS_ERROR, lpath + ": unable to open local file for writing");
  }

  if(ssh_scp_accept_request(ssh->scp) != SSH_OK) {
    fo.Close();
    return set_ssh_err(COPY_ERROR);
  }

  size_t nread, ntot = 0;

  do {
    int rc = ssh_scp_read(ssh->scp, buffer, bufsize);
    
    if(rc < 0) {
      set_ssh_err(COPY_ERROR);
      break;
    }
    nread = rc;

#ifdef _WIN32
    if(flags & TEXT) { // Convert \n => \r\n
      size_t i, pos = 0;
      for(i=0; i<nread; i++) {
        if(buffer[i] == '\r') { // skip all '\r' in the input file
          if(i > pos) fo.Write(buffer + pos, i - pos);
          pos = i + 1;
        }
        else if(buffer[i] == '\n') { // put '\r' before each '\n'
          if(i > pos) fo.Write(buffer + pos, i - pos);
          fo.Write("\r", 1);  
          pos = i;
        }
      }
      if(i > pos) fo.Write(buffer + pos, i - pos);
    }
    else // write buffer without conversion
#endif
      if(nread) fo.Write(buffer, nread);
    
    ntot += nread;

    if(log_verbosity >= 1)
      LOGJOBMSG(fmt("Read %u bytes from %s, total: %u/%u\n",
                    nread, filename, ntot, filesize));

  } while(nread && ntot < filesize);
  
  fo.Close();

  if(!error_code && ntot != filesize)
    set_err(COPY_ERROR, gmdString(path) + ": incomplete file transfer");

  return error_code;
}


int gmShellLibssh::set_ssh_err(int code){
  set_err(code, gmdString("SSH: LibSSH error: ") + ssh_get_error(ssh->session));
  return code;
}


// Allocate SSH session object and set options
void gmShellLibssh::Open() {
  ssh->session = ssh_new();
  if(!ssh->session) LOGJOBERR("Unable to allocate SSH session handle");

  if( param["host"].IsEmpty() )
    LOGJOBERR("'host' parameter is not specified");

  ssh_options_set(ssh->session, SSH_OPTIONS_HOST, (pCSTR)param["host"].c_str());
  ssh_options_set(ssh->session, SSH_OPTIONS_USER, (pCSTR)param["login"].c_str());
  ssh_options_set(ssh->session, SSH_OPTIONS_LOG_VERBOSITY, &log_verbosity);
  ssh_options_set(ssh->session, SSH_OPTIONS_PORT, &port);
  if( !param["knownhosts"].IsEmpty() )
    ssh_options_set(ssh->session, SSH_OPTIONS_KNOWNHOSTS, (pCSTR)param["knownhosts"].c_str());
  if(ssh_timeout_ms >= 0) {
    long timeout_sec = ssh_timeout_ms / 1000;
    long timeout_usec = ssh_timeout_ms - timeout_sec*1000;
    timeout_usec *= 1000;
    ssh_options_set(ssh->session, SSH_OPTIONS_TIMEOUT, &timeout_sec);
    ssh_options_set(ssh->session, SSH_OPTIONS_TIMEOUT_USEC, &timeout_usec);
  }

  ssh_set_callbacks(ssh->session, &ssh->cb);
}


int gmShellLibssh::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  int res = -1, rc;

  execute_begin(cmd, out, err);

  for(int iatt = 0; iatt < read_att_num; iatt++) {
    if(iatt && log_verbosity >= 1)
      LOGJOBMSG(fmt("StageOut error: %s\nRetrying, attempt %d/%d\n",
                    error_msg.c_str(), iatt+1, read_att_num));
    clear_err();

    for(int i=0; i<2; i++) {
      if( check_connection(i > 0) ) return execute_error();

      // Allocate channel handle
      if(ssh->channel) ssh_channel_free(ssh->channel);
      ssh->channel = ssh_channel_new(ssh->session);
      if(!ssh->channel) LOGJOBERR("Unable to allocate SSH channel handle");

      if( (rc = ssh_channel_open_session(ssh->channel)) == SSH_OK ) break;
    }

    if(rc != SSH_OK) return execute_error( set_ssh_err(CONNECTION_ERROR) );

    do { // This is not actually a loop, just for being able to break on errors
      // Execute command
      if( (rc = ssh_channel_request_exec(ssh->channel, cmd)) != SSH_OK ) break;

      // Read stdout and stderr
      gmdString errstr, outstr;
      if( (rc = read_from_channel(outstr, 0)) ||
          (rc = read_from_channel(errstr, 1)) ) break;
      string_to_line_array(outstr, out);
      string_to_line_array(errstr, err);

      if( (rc = ssh_channel_send_eof(ssh->channel)) != SSH_OK ) break;

      // Get exit code
      res = ssh_channel_get_exit_status(ssh->channel);
    } while(false);

    if(rc != SSH_OK) set_ssh_err(EXECUTE_ERROR);

    ssh_channel_close(ssh->channel);

    if(rc == SSH_OK) break;
  }

  return rc == SSH_OK ? execute_end(res, out, err) : execute_error(rc);
}


int gmShellLibssh::StageIn(pCSTR locpath, pCSTR rempath, unsigned flags){
  int mode = SSH_SCP_WRITE | (flags & RECURSIVE ? SSH_SCP_RECURSIVE : 0);

  if( StageIn_begin(locpath, rempath, flags) || open_scp_session(mode, rempath) )
    return StageIn_end(locpath, rempath, flags);

  // Adjust local path
  gmdString lpath(locpath);
#ifdef _WIN32
  lpath.Replace("/", "\\");
#endif
  if(lpath.EndsWith(DIR_SEPARATOR)) lpath.Truncate(lpath.Len()-1);

  // Send files/dirs
  int nfiles = 0;
  if( gmdFileExists(lpath) ) {
    send_file(lpath, flags);  // Send a single file
    nfiles++;
  }
  else {
    gmdDir lister;
    DirTraverser traverser(*this);
    gmdFileName fn(lpath);
    int lflags = gmdDIR_FILES | gmdDIR_HIDDEN | (flags & RECURSIVE ? gmdDIR_DIRS : 0);

    // Send a directory or a set of files defined using wildcards
    if( gmdDirExists(lpath) && (flags & RECURSIVE)) {
      // Send a whole directory
      if( ssh_scp_push_directory(ssh->scp, fn.GetFullName(), DEFAULT_DIR_PERM)
          != SSH_OK )
        set_ssh_err(COPY_ERROR);
      else {
        lister.Open(lpath);
        traverser.Init(lpath, flags);
        nfiles = lister.Traverse(traverser, "", lflags);
      }
    }
    else if( has_wildcards(lpath) ) {
      // Send files defined using wildcards
      lister.Open(fn.GetPath());
      traverser.Init(fn.GetPath(), flags);
      nfiles = lister.Traverse(traverser, fn.GetFullName(), lflags);
    }
  }
  
  ssh_scp_close(ssh->scp);

  if(!error_code && nfiles == 0)
    set_err(NO_INPUT_FILE, lpath + ": no input files or recurse is disabled");

  return StageIn_end(lpath, rempath, flags);
}


int gmShellLibssh::StageOut(pCSTR locpath, pCSTR rempath, unsigned flags){
  int mode = SSH_SCP_READ | (flags & RECURSIVE ? SSH_SCP_RECURSIVE : 0);

  if( StageOut_begin(locpath, rempath, flags) )
    return StageOut_end(locpath, rempath, flags);
  
  // Adjust local path
  gmdString lpath(locpath);
#ifdef _WIN32
  lpath.Replace("/", "\\");
#endif
  if(lpath.EndsWith(DIR_SEPARATOR)) lpath.Truncate(lpath.Len()-1);

  bool locpath_is_dir = gmdDirExists(lpath);

  for(int iatt = 0; iatt < read_att_num; iatt++) {
    if(iatt && log_verbosity >=1)
      LOGJOBMSG(fmt("StageOut error: %s\nRetrying, attempt %d/%d\n",
                    error_msg.c_str(), iatt+1, read_att_num));
    clear_err();

    if( open_scp_session(mode, rempath) )
      return set_ssh_err(CONNECTION_ERROR);

    pCSTR filename;
    gmdString warn_msg;
    std::stack<gmdString> dirstack;
    int rc;
    bool fatal_error = false;

    while( !error_code &&
           (rc = ssh_scp_pull_request(ssh->scp)) != SSH_SCP_REQUEST_EOF )
    {
      switch(rc) {
        case SSH_SCP_REQUEST_NEWFILE:
          receive_file(lpath, flags, locpath_is_dir);
          break;

        case SSH_SCP_REQUEST_NEWDIR:
          if(!locpath_is_dir) {
            set_err(INVALID_REQUEST, lpath + ": source path is not a directory");
            break;
          }

          if( !(filename = ssh_scp_request_get_filename(ssh->scp)) ) {
            set_ssh_err(COPY_ERROR);
            break;
          }

          dirstack.push(lpath);
          lpath += gmdString(DIR_SEPARATOR) + filename;

          if( !gmdMkdir(lpath) ) {
            set_err(LOCAL_FS_ERROR, lpath + ": unable to create local directory");
            break;
          }

          ssh_scp_accept_request(ssh->scp);
          break;

        case SSH_SCP_REQUEST_ENDDIR:
          if( dirstack.empty() )  // logical error in libssh or in this code
            LOGJOBERR("gmShellLibssh::StageOut: error in the directory structure");
          lpath = dirstack.top();
          dirstack.pop();
          break;

       case SSH_SCP_REQUEST_WARNING:
         warn_msg = ssh_scp_request_get_warning(ssh->scp);
         if( warn_msg.EndsWith("No such file or directory") )
           set_err(NO_INPUT_FILE, gmdString(rempath) + ": No file or directory");
         break;

       case SSH_ERROR:
         set_ssh_err(COPY_ERROR);
      }
    }

    ssh_scp_close(ssh->scp);

    if(error_code != COPY_ERROR) break;
  }

  return StageOut_end(lpath, rempath, flags);
}
