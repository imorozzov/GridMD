/****************************************************************************
 * \file winshmngr.h
 * Header file for the Windows MinGW Job Manager
****************************************************************************/

#ifndef __WINSHMNGR_H
#define __WINSHMNGR_H

#include "jobmngr/jobmngr.h"

class gmWinShManager : public gmJobManager
{
protected:
  int stat_read_ntries, stat_read_delay; // in miliseconds
  enum {SCRIPT_TYPE_WIN, SCRIPT_TYPE_BASH} script_type;

  virtual gmdString get_user_cmd(gmJob& job, gmdString& user_cmd, bool make_script);
  virtual gmdString get_subm_cmd(gmJob& job);
  virtual gmdString get_script_body(gmJob& job, gmdString& user_cmd);
  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd);
  virtual gmdString get_status_cmd();
  virtual int interpret_status(const gmdString& str);
  virtual gmdString get_stop_cmd(gmJob& job);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  
  void init();

public:
  gmWinShManager() { init(); }
  gmWinShManager(gmShell& shell_) { init(); Open(shell_); }
  ~gmWinShManager() { Close(); }

  virtual int required_cmd_set() { return gmShell::CMD_SET_MINGW; }

  virtual pCSTR GetClassName() {
    return "gmWinShManager";
  }
};


#endif
