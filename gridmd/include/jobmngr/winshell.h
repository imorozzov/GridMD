/****************************************************************************
 * \file winshell.h
 * Definition of class gmShellWin
****************************************************************************/

#ifndef __WINSHELL_H
#define __WINSHELL_H

#include "jobmngr/gmshell.h"


class gmShellWin : public gmShell
{
protected:
  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);

  gmdString ReadFile(pCSTR filename);

  virtual gmdString get_home_dir() {
    return get_home_dir_win();
  }

  int xcopy(gmdString& src, gmdString& dst, unsigned flags);

public:
  gmShellWin();

  virtual pCSTR GetClassName() {
    return "gmShellWin";
  }

  int ExecuteAsync(const gmdString& cmd);
  virtual int MkDir(pCSTR rempath) {
    return MkDirLocal( get_full_path_win(rempath) );
  }
  virtual int Remove(pCSTR rempath){
    return remove_local( get_full_path_win(rempath) );
  }
  virtual int Remove(const gmdArrayString& remfiles);
  virtual int StageIn(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
  virtual int StageOutText(gmdString& str, pCSTR rempath);
  virtual int check_command_set(int cmd_set);
};

#endif
