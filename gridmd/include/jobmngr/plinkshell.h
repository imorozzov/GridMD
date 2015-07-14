/****************************************************************************
 * \file plinkshell.h
 * Definition of class gmShellPlink
****************************************************************************/

#ifndef __PLINKSHELL_H
#define __PLINKSHELL_H

#include "jobmngr/gmshell.h"


class gmShellPlink : public gmShell
{
protected:
  int plink_att_num;  /// Number of attaemnts to connect remote system
  int plink_retry_delay;  /// Delay between retries
  gmdString userhost, plink_args;
  gmdString plink_path, pscp_path;
  gmdString plink_pre, pscp_pre;
  bool auth_defined;

  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  int pscp_execute(const gmdString& args);

public:
  /// userhost should have the form "login@hostname"
  /// Typical values of "plink_args" are (see PLINK manual for details):
  ///   -load <session_name>
  ///   -i <ssh_key_path>
  ///   -pw <password>
  /// arguments "-ssh -batch" will be added automatically
  gmShellPlink(pCSTR login="", pCSTR host="", pCSTR plink_args="");

  virtual pCSTR GetClassName() {
    return "gmShellPlink";
  }

  virtual int StageIn(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
  virtual int StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR rempath, unsigned flags = 0);
};

#endif
