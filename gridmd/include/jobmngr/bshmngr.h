/****************************************************************************
 * \file bshmngr.h
 * Header file for bash shell Job Manager
****************************************************************************/

#ifndef __BSHMNGR_H
#define __BSHMNGR_H

#include "jobmngr/jobmngr.h"


class gmBShManager : public  gmJobManager
{
protected:
  virtual gmdString get_subm_cmd(gmJob& job);
  virtual gmdString get_script_body(gmJob& job, gmdString& user_cmd);
  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd);
  virtual gmdString get_status_cmd();
  virtual int interpret_status(const gmdString& str);
  virtual gmdString get_stop_cmd(gmJob& job);
  //virtual void set_param(const gmdString& par_name, const gmdString& par_value);

public:
  gmBShManager() { init(); }
  gmBShManager(gmShell& shell_) { init(); Open(shell_); }
  ~gmBShManager() { Close(); }

  virtual pCSTR GetClassName() {
    return "gmBShManager";
  }
};


#endif
