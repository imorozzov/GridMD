/****************************************************************************
 * \file unixsshell.h
 * Definition of class gmSShellUnix
****************************************************************************/

#ifndef __UNIXSSHELL_H
#define __UNIXSSHELL_H

#include "jobmngr/gmshell.h"


class gmSShellUnix : public gmShell
{
protected:
  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);

  int scp_execute(const gmdString& cmd, bool upload);

  gmdString userhost, ssh_args;
  gmdString ssh_path, scp_path;
  gmdString ssh_pre, scp_pre;
  bool auth_defined;

public:
  gmSShellUnix(pCSTR login="", pCSTR host="", pCSTR ssh_args="");

  virtual pCSTR GetClassName() {
    return "gmSShellUnix";
  }

  virtual int StageIn(pCSTR locfile, pCSTR rempath, unsigned flags = 0);
  virtual int StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR remfile, unsigned flags = 0);
  virtual int StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags = 0);
};

#endif
