// Sustem-level operations

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <string>
#endif

#include "logexc.h"
#include <gmd/tokenzr.h>
#include <gmd/utils.h>
#include "gmd_internal.h"


#ifndef _WIN32
  typedef int HANDLE;
  typedef unsigned long DWORD;
  typedef bool BOOL;
#endif


// Definitions of the internal functions
long wait_for_child(HANDLE h);
int split_cmd_into_args(const gmdString& command, gmdChar** argv, const int max_args);
void string_to_line_array(const gmdString& str, gmdArrayString& arr);


//----------------------------------------------------------------------------
// gmdPipe class for sdtout, stderr redirection
//----------------------------------------------------------------------------
class gmdPipe {
public:
  HANDLE rd, wr;

  BOOL Open() {
#ifdef _WIN32
    SECURITY_ATTRIBUTES security;
    security.nLength              = sizeof(security);
    security.lpSecurityDescriptor = NULL;
    security.bInheritHandle       = TRUE; // to pass it to the child
    return CreatePipe(&rd, &wr, &security, 0);
#else
    return pipe(&rd) != -1;
#endif
  }

  static void CloseHandle(HANDLE h) {
#ifdef _WIN32
  ::CloseHandle(h);
#else
  close(h);
#endif
  }
};


class gmdPipeReader {
protected:
  HANDLE h;
  enum{ BUFSIZE = 4096 };
  gmdChar buf[BUFSIZE + 1];

public:
  gmdString data;

  gmdPipeReader(HANDLE handle) : h(handle) {}

  int UpdateBuf();
  gmdString& Read();
  static void ReadAll(
    HANDLE h_out, HANDLE h_err, gmdArrayString* output, gmdArrayString* error );
};


// Update input buffer
int gmdPipeReader::UpdateBuf() {
#ifdef _WIN32
  DWORD dwRead;
  //if( can_read() && !ReadFile(h, buf, BUFSIZE, &dwRead, NULL) )
  if( !ReadFile(h, buf, BUFSIZE, &dwRead, NULL) ) {
    if( GetLastError() == ERROR_BROKEN_PIPE ) return -1;
    else LOGERR0("gmdExecute: failed reading from pipe!");
  }
  return dwRead;
#else
  // When a background process started from a command line like
  // "nohup script.sh >out 2>err &\n echo $!"
  // the read() function can stall untill the background process is completed.
  // To avoid this the can_read() function is added with a 1 ms delay which 
  // ensures that we don't miss any output.
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1000;  // timeout is one millisecond

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(h, &readfds);

  if( select(h + 1, &readfds, NULL, NULL, &tv) != 1 ) return 0;

  return read(h, buf, BUFSIZE);
#endif
}


gmdString& gmdPipeReader::Read() {
  int nread;
  do {
    nread = UpdateBuf();
    if(nread > 0) {
      buf[nread] = 0;
      data += buf;
    }
#ifdef _WIN32
  } while(nread >= 0);
#else
  } while(nread > 0);
#endif

  return data;
}


void gmdPipeReader::ReadAll(
    HANDLE h_out, HANDLE h_err, gmdArrayString* output, gmdArrayString* error )
{
  gmdPipeReader out(h_out), err(h_err);

  if(output) string_to_line_array(out.Read(), *output); else out.Read();
  if(error ) string_to_line_array(err.Read(), *error ); else err.Read();

  gmdPipe::CloseHandle(h_out);
  gmdPipe::CloseHandle(h_err);
}


//----------------------------------------------------------------------------
// Execute a command
//----------------------------------------------------------------------------
long gmdExecute(const gmdString& command,
                gmdArrayString* output, gmdArrayString* error, int sync){
  long res;

  BOOL redirect = (output || error || ! sync);

  gmdPipe StdIn, StdOut, StdErr;
  if(redirect && ( !StdIn.Open() || !StdOut.Open() || !StdErr.Open() ) )
    LOGERR0("gmdExecute: error creating pipes");
  
  HANDLE StdOut_rd_dup = 0, StdErr_rd_dup = 0; //, StdIn_wr_dup;

#ifdef _WIN32
  // Fill in CreateProcess structure
  STARTUPINFOA si;
  gmdZeroMemory(si);
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  
  // Create handle duplicates and close unused pipes
  if(output || error) {
    gmdPipe::CloseHandle(StdIn.wr);

    if( !DuplicateHandle(GetCurrentProcess(), StdOut.rd,
        GetCurrentProcess(), &StdOut_rd_dup , 0,
        FALSE,
        DUPLICATE_SAME_ACCESS) )
      LOGERR0("DuplicateHandle failed");
    gmdPipe::CloseHandle(StdOut.rd);

    if( !DuplicateHandle(GetCurrentProcess(), StdErr.rd,
        GetCurrentProcess(), &StdErr_rd_dup , 0,
        FALSE,
        DUPLICATE_SAME_ACCESS) )
      LOGERR0("DuplicateHandle failed");
    gmdPipe::CloseHandle(StdErr.rd);

    // Assign pipes to child process I/O streams
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = StdIn.rd;
    si.hStdOutput = StdOut.wr;
    si.hStdError = StdErr.wr;
    //si.wShowWindow = SW_HIDE;
  }

  DWORD creationFlags =  0;
  //creationFlags |= NORMAL_PRIORITY_CLASS;
#ifdef ALLOW_DETACHED_PROCESS
  if(!sync) creationFlags |= DETACHED_PROCESS;
  if(!sync) creationFlags |= CREATE_NO_WINDOW;
#endif


  // Create process
  PROCESS_INFORMATION pi;
  gmdZeroMemory(pi);
  int pid = CreateProcessA(
    NULL,               // application name (use only cmd line)
    (char*)command.c_str(),    // full command line
    NULL,               // security attributes: defaults for both
    NULL,               //   the process and its main thread
    redirect,           // inherit handles if we use pipes
    creationFlags,      // process creation flags
    NULL,               // environment (use the same)
    NULL,               // current directory (use the same)
    &si,                // startup info (unused here)
    &pi                 // process info
    ) != 0 ? pi.dwProcessId : -1;

  HANDLE hProcess = pi.hProcess;
#else
  // Prepare the list of arguments
  const int MAX_ARGS = 128;
  gmdChar *argv[MAX_ARGS];
  int argc = split_cmd_into_args(command, argv, MAX_ARGS);
  
  // Create process
  pid_t pid = fork();
  
  if(pid < 0) {
    LOGERR0("gmdExecute: fork failed");
    return -1;
  }

  if( pid == 0 ) {  // we're in child
    if( redirect && (
          dup2(StdIn.rd, STDIN_FILENO) == -1 ||
          dup2(StdOut.wr, STDOUT_FILENO) == -1 ||
          dup2(StdErr.wr, STDERR_FILENO) == -1 )
      ) puts("gmdExecute: Failed to redirect child process input/output!");
    else {
      gmdPipe::CloseHandle(StdIn.wr);
      gmdPipe::CloseHandle(StdOut.rd);
      gmdPipe::CloseHandle(StdErr.rd);

      execvp(*argv, argv);
      puts("gmdExecute: execvp failed!");
    }
    exit(-1);
  }
  
  // we're in parent
  for(argc=0; argv[argc]; argc++) delete[] argv[argc];

  gmdPipe::CloseHandle(StdIn.wr);
  HANDLE hProcess = pid;

  StdOut_rd_dup = StdOut.rd;
  StdErr_rd_dup = StdErr.rd;
#endif
  
  // Close child pipe ends
  if( output || error ) {
    gmdPipe::CloseHandle(StdIn.rd);
    gmdPipe::CloseHandle(StdOut.wr);
    gmdPipe::CloseHandle(StdErr.wr);
    if(pid < 0) {
      gmdPipe::CloseHandle(StdOut_rd_dup);
      gmdPipe::CloseHandle(StdErr_rd_dup);
    }
  }

  if(pid < 0) return -1;  // exit on error
  res = pid;

#ifdef _WIN32
  if(output || error)
    gmdPipeReader::ReadAll(StdOut_rd_dup, StdErr_rd_dup, output, error);
  
  if(sync) {  // Wait for the child process termination (if needed)
    WaitForSingleObject(hProcess, INFINITE);
    if( !GetExitCodeProcess(hProcess, (LPDWORD)&res) )
      LOGERR0("GetExitCodeProcess failed");
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
#else
  // Wait for the child process termination (if needed)
  if(sync) {
    int statval;
    if( waitpid(hProcess, &statval, 0) != -1 && WIFEXITED(statval) )
      res = WEXITSTATUS(statval);
    else
      LOGERR0("Error getting child exit code");

    if(redirect)
      gmdPipeReader::ReadAll(StdOut_rd_dup, StdErr_rd_dup, output, error);
  }
#endif

  return res;
}


//----------------------------------------------------------------------------
// Kill processes
//----------------------------------------------------------------------------
#ifdef _WIN32
int gmdKillAllChildren(long pid) {
  // Take a snapshot of all processes in the system.
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE) return -1;

  PROCESSENTRY32 pe;
  gmdZeroMemory(pe);
  pe.dwSize = sizeof(PROCESSENTRY32);

  // Walk the snapshot of the processes, and for each process,
  // kill it if its parent is pid.
  if (!Process32First(hProcessSnap, &pe)) return -1;
  do {
    if (pe.th32ParentProcessID == (DWORD) pid) {
      if (gmdKill(pe.th32ProcessID, gmdSIGKILL)) return -1;  // recursion!
    }
  } while (Process32Next (hProcessSnap, &pe));

  CloseHandle(hProcessSnap);
  return 0;
}
#endif


int gmdKill(long pid, gmdSignal sig, int *krc, int flags) {
#ifdef _WIN32
  if(sig == gmdSIGNONE) return 0;
  if(sig != gmdSIGKILL) LOGERR0("gmdKill: only SIGKILL signal can be sent");

  if (flags & gmdKILL_CHILDREN) gmdKillAllChildren(pid);

  // get the process handle to operate on
  HANDLE hProcess = ::OpenProcess(SYNCHRONIZE |
                                  PROCESS_TERMINATE |
                                  PROCESS_QUERY_INFORMATION,
                                  FALSE, // not inheritable
                                  (DWORD)pid);
  if(!hProcess) return -1;
  if( !TerminateProcess(hProcess, (UINT)-1) )
    LOGERR0( fmt("Failed to kill process %d", pid) );
  
  int res = WaitForSingleObject(hProcess, 1000);
  if(res != WAIT_OBJECT_0) 
    LOGERR0( fmt("Process %d termination timeout", pid) );

  return 0;
#else
  return kill((pid_t) (flags & gmdKILL_CHILDREN) ? -pid : pid, (int)sig);
#endif
}

//----------------------------------------------------------------------------
// Sleep
//----------------------------------------------------------------------------
void gmdMilliSleep(unsigned long milliseconds){
#ifdef _WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds*1000);
#endif
}


//----------------------------------------------------------------------------
// System information
//----------------------------------------------------------------------------
gmdString gmdGetUserName() {
#ifdef _WIN32
  const int nSize = 1024;
  char buf[nSize];
  DWORD size = nSize;
  if( GetUserNameA(buf, &size) == 0 ) LOGERR0("gmdGetUserName failed");
  return buf;
#else
  struct passwd *who;
  if ((who = getpwuid(getuid ())) == NULL) LOGERR0("gmdGetUserName failed");
  return who->pw_name;
#endif
}


gmdString gmdGetHostName() {
  const int nSize = 1024;
  char buf[nSize];
#ifdef _WIN32
  DWORD size = nSize;
  if( GetComputerNameA(buf, &size) == 0 )
#else
  if( gethostname(buf, nSize) == -1 )
#endif
    LOGERR0("gmdGetHostName failed");
  return buf;
}


gmdString gmdGetOsDescription() {
#ifdef _WIN32
  gmdString str;
  OSVERSIONINFOA info;
  gmdZeroMemory(info);
  info.dwOSVersionInfoSize = sizeof(info);

  if ( ::GetVersionExA(&info) ) {
    switch ( info.dwPlatformId ) {
      case VER_PLATFORM_WIN32s:
          str = "Win32s on Windows 3.1";
          break;

      case VER_PLATFORM_WIN32_WINDOWS:
        switch (info.dwMinorVersion) {
          case 0:
            if ( info.szCSDVersion[1] == 'B' ||
                 info.szCSDVersion[1] == 'C' ) str = "Windows 95 OSR2";
            else str = "Windows 95";
            break;
          case 10:
            if ( info.szCSDVersion[1] == 'B' ||
                 info.szCSDVersion[1] == 'C' ) str = "Windows 98 SE";
            else str = "Windows 98";
            break;
          case 90:
            str = "Windows ME";
            break;
          default:
            str = gmdString::Format("Windows 9x (%d.%d)",
              info.dwMajorVersion, info.dwMinorVersion);
            break;
        }
        if ( !gmdIsEmpty(info.szCSDVersion) )
          str += gmdString(" (") + info.szCSDVersion + ")";
        break;

      case VER_PLATFORM_WIN32_NT:
        switch ( info.dwMajorVersion ) {
          case 5:
            switch ( info.dwMinorVersion ) {
              case 0:
                str = gmdString::Format("Windows 2000 (build %lu",
                           info.dwBuildNumber);
                break;

              case 1:
                str = gmdString::Format("Windows XP (build %lu",
                           info.dwBuildNumber);
                break;

              case 2:
                str = gmdString::Format("Windows Server 2003 (build %lu",
                           info.dwBuildNumber);
                break;
            }
            break;

          case 6:
            if ( info.dwMinorVersion == 0 )
              str = gmdString::Format("Windows Vista (build %lu",
                         info.dwBuildNumber);
            break;
        }

        if ( str.empty() )
          str = gmdString::Format("Windows NT %lu.%lu (build %lu",
                 info.dwMajorVersion,
                 info.dwMinorVersion,
                 info.dwBuildNumber);

        if ( !gmdIsEmpty(info.szCSDVersion) )
          str += gmdString(", ") + info.szCSDVersion;
        str += ")";
        break;
    }
  } else
    LOGERR0("gmdGetOsDescription failed");

  return str;
#else
  gmdArrayString out, err;
  if( gmdExecute("uname -s -r -m", out, err) )
    LOGERR0("gmdGetOsDescription failed");
  return out[0];
#endif
}


//----------------------------------------------------------------------------
// Internal functions
//----------------------------------------------------------------------------

// Split the command line into arguments
// Returs the number of arguments
#ifndef _WIN32
int split_cmd_into_args(const gmdString& command, gmdChar** argv, const int max_args) {
  int argc = 0;
  gmdString argument;
  const gmdChar *cptr = command.c_str();
  bool escaped = false;

  do {
    argument.Clear();
    gmdChar quotechar = 0;

    while ( isspace(*cptr) ) cptr++; // eat leading whitespace

    if ( *cptr == '"' || *cptr == '\'' )  quotechar = *cptr++;

    do {
      if ( *cptr == '\\' && !escaped ) {
        escaped = true;
        cptr++;
        continue;
      }

      // all other characters:
      argument += *cptr++;
      escaped = false;
      
// Disable escaping all characters except quotechar
//      escaped = ( *cptr == '\\' && !escaped );
//      if( escaped && cptr[1] == quotechar ) {
//        cptr++;
//        continue;
//      }
//
//      // all other characters:
//      argument += *cptr++;

      // have we reached the end of the argument?
      if ( (*cptr == quotechar && !escaped)
           || (quotechar == 0 && isspace(*cptr))
           || *cptr == 0 )
      {
        if(argc >= max_args)
          LOGERR0("too many arguments in gmdExecute");

        argv[argc] = new gmdChar[argument.length() + 1];
        strcpy(argv[argc], argument.c_str());
        argc++;

        // if not at end of buffer, swallow last character:
        if(*cptr) cptr++;

        break; // done with this one, start over
      }
    } while(*cptr);
  } while(*cptr);
  argv[argc] = NULL;
  
  return argc;
}
#endif


void string_to_line_array(const gmdString& str, gmdArrayString& arr){
  arr.Clear();
  std::string::size_type i = 0, len = str.Len();
  
  while(i < len) {
    int j = str.find_first_of("\r\n", i);
    if(j == std::string::npos) j = len;
    arr.Add( str.Mid(i, j - i) );
    i = j + 1;
    if(i < len && str[j] == '\r' && str[i] == '\n') i++;
  }
}
