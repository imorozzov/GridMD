/****************************************************************************
 * \file mingwshell.h
 * Definition of class gmShellMinGW
****************************************************************************/

#ifndef __MINGWSHELL_H
#define __MINGWSHELL_H

#include "jobmngr/winshell.h"


class gmShellMinGW : public gmShellWin
{
protected:
  gmdString bash_cmd;

  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);

public:
  gmShellMinGW(pCSTR bash_path = "bash.exe", pCSTR home_path="");

  virtual pCSTR GetClassName() {
    return "gmShellMinGW";
  }

  int ExecuteAsync(const gmdString& cmd);
  virtual int MkDir(pCSTR rempath){
    return Execute(gmdString("mkdir -p ") + rempath);
  }
  virtual int Remove(pCSTR rempath){
    return Execute(gmdString("rm -Rf ") + rempath);
  }
  virtual int Remove(const gmdArrayString& remfiles) {
    return Remove( ArrayToString(remfiles, " ") );
  }

  virtual int check_command_set(int cmd_set);
};

#endif
