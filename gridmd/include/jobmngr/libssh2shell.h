/****************************************************************************
 * \file plinkshell.h
 * Definition of class gmShellLibssh2
****************************************************************************/

#ifndef INCL_LIBSSH2SHELL_H
#define INCL_LIBSSH2SHELL_H

#include "jobmngr/gmshell.h"
#include <boost/shared_ptr.hpp>

#ifndef USING_LIBSSH2
#error "Macro USING_LIBSSH2 should be defined, or don't include this header."
#endif

class gmShellLibssh2 : public gmShell
{
protected:
  //int plink_att_num;  /// Number of attaemnts to connect remote system
  //int plink_retry_delay;  /// Delay between retries
  //gmdString userhost, plink_args;
  //gmdString host, login, pw, key_path;

  //gmdString plink_path, pscp_path;
  //gmdString plink_pre, pscp_pre;
  bool auth_changed;

  class Impl;
  boost::shared_ptr<Impl> pImpl;
  Impl& impl();


  virtual int execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  //int pscp_execute(const gmdString& src, const gmdString& dst, gmdString& err);
  int stage_out(pCSTR locpath, pCSTR rempath, unsigned flags,
                bool locpath_is_file);


public:
  /// userhost should have the form "login@hostname"
  /// Typical values of "plink_args" are (see PLINK manual for details):
  ///   -load <session_name>
  ///   -i <ssh_key_path>
  ///   -pw <password>
  /// arguments "-ssh -batch" will be added automatically
  gmShellLibssh2(pCSTR login="", pCSTR host="");

  virtual pCSTR GetClassName() {
    return "gmShellLibssh2";
  }

  virtual int MkDir(pCSTR rempath){
    return Execute(gmdString("mkdir -p ") + rempath);
  }
  virtual int Remove(pCSTR rempath){
    return Execute(gmdString("rm -Rf ") + rempath);
  }
  virtual int Remove(const gmdArrayString& remfiles) {
    return Remove( ArrayToString(remfiles, " ") );
  }
//  virtual int StageIn(pCSTR locpath, pCSTR rempath, int type = 0);
//  virtual int StageIn(const gmdArrayString& locfiles, pCSTR remdir, int type = 0);
//  virtual int StageOut(pCSTR locpath, pCSTR rempath, int type = 0);
//  virtual int StageOut(pCSTR locdir, const gmdArrayString& remfiles, int type = 0);

  virtual int StageIn(pCSTR locfile, pCSTR rempath, unsigned flags = 0) ;
  virtual int StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags = 0) ;
  virtual int StageOut(pCSTR locpath, pCSTR remfile, unsigned flags = 0) ;
  virtual int StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags = 0) ;


//  int static dos2unix(pCSTR src, pCSTR dst);
//  int static unix2dos(pCSTR src, pCSTR dst);
};

#endif
