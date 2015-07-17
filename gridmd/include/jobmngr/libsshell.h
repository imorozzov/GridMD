/****************************************************************************
 * \file libsshell.h
 * Definition of class gmShellLibssh
****************************************************************************/

#ifndef __LIBSSHELL_H
#define __LIBSSHELL_H

#include "jobmngr/gmshell.h"

class SSH_Data;
class DirTraverser;

class gmShellLibssh : public gmShell
{
friend class DirTraverser;

protected:
  enum { HOST_ACC_KNOWN, HOST_ACC_NEW, HOST_ACC_CHANGED, HOST_ACC_ALL }
    host_accept;

  int log_verbosity;
  int port;
  int ssh_timeout_ms;
  int conn_att_num;  /// Number of attempts to connect remote system
  int conn_retry_delay;  /// Delay between retries
  int execute_timeout, copy_timeout, channel_read_inteval, read_att_num;
  bool connected;
  SSH_Data* ssh;
  
  int check_connection(bool on_error = false);
  int connect();
  int open_scp_session(int mode, pCSTR rempath);
  int read_from_channel(gmdString& dst, int is_stderr);
  int send_file(pCSTR path, unsigned flags);
  int receive_file(pCSTR path, unsigned flags, bool locpath_is_dir);
  int set_ssh_err(int code);

  static int auth_callback(const char *prompt, char *buf, size_t len, int echo,
    int verify, void *userdata);

  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);

public:
  gmShellLibssh(pCSTR login="", pCSTR host="");
  ~gmShellLibssh();

  virtual pCSTR GetClassName() {
    return "gmShellLibssh";
  }

  virtual void Open();
  virtual void Close();

  virtual int StageIn(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
};

#endif
