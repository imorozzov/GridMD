/****************************************************************************
 * \file unixshell.h
 * Definition of class gmShellUnix
****************************************************************************/

#ifndef __UNIXSHELL_H
#define __UNIXSHELL_H

#include "jobmngr/gmshell.h"


class gmShellUnix : public gmShell
{
protected:
  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);

  int cp_execute(const gmdString& cmd);
  
public:
  gmShellUnix() {}

  virtual pCSTR GetClassName() {
    return "gmShellUnix";
  }
  virtual int StageIn(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
};

#endif
