/****************************************************************************
 * \file unixsshell.cpp
 * Implementation of the class gmSShellUnix
****************************************************************************/

#include <stdio.h>
#include <gmd/utils.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>
#include "jobmngr/unixsshell.h"


gmSShellUnix::gmSShellUnix(pCSTR login, pCSTR host, pCSTR ssh_args) :
  auth_defined(false)
{
  InitParam("login", login);
  InitParam("host", host);
  InitParam("ssh_args", ssh_args);
  InitParam("ssh_path", "ssh");
  InitParam("scp_path", "scp");
}


void gmSShellUnix::set_param(const gmdString& par_name, const gmdString& par_value){
  bool err = false;
  bool ssh_cmd_changed = false, scp_cmd_changed = false, auth_changed = false;

  if(par_name == "login" || par_name == "host") auth_changed = true;
  else if(par_name == "ssh_args") {
    ssh_args = par_value;
    auth_changed = true;
  }
  else if(par_name == "ssh_path") {
    ssh_path = par_value;
    ssh_cmd_changed = true;
    err = ssh_path.IsEmpty();
  }
  else if(par_name == "scp_path") {
    scp_path = par_value;
    scp_cmd_changed = true;
    err = scp_path.IsEmpty();
  }
  else
    gmShell::set_param(par_name, par_value);

  if(err) {
    InvalidParValue(par_name, par_value);
    return;
  }
  
  // Check authorization parameters
  if(auth_changed) auth_defined = !param["host"].IsEmpty();
  
  if(auth_defined) {
    // Create command line templates
    userhost = param["login"] + "@" + param["host"];
    if(ssh_cmd_changed || auth_changed)
      ssh_pre = gmdString(ssh_path) + ' '
              + ssh_args + " -o BatchMode=yes " + userhost + " \"";
    if(scp_cmd_changed || auth_changed)
      scp_pre  = gmdString(scp_path) + ' ' + ssh_args + " -B ";
  }
}


int gmSShellUnix::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  int res;

  execute_begin(cmd, out, err);

  if(!auth_defined) {
    LOGJOBERR("Authentification method is not defined, "
      "check 'login', 'host' and 'ssh_args' parameters!");
    return -1;
  }

  gmdString cmd_scr(cmd);
  // Screen all '\' symbols as gmdExecute replaces all '\<char>' by '<char>'
  // when passing the parameters to execvp()
  cmd_scr.Replace("\\", "\\\\");
  cmd_scr.Replace("\"", "\\\"");  // screed double quotes
  if( cmd_scr.IsEmpty() ) cmd_scr = "#";  // prevent invoking interactive mode

  res = gmdExecute(ssh_pre + cmd_scr + "\"", out, err);
  if(res == -1)
    return execute_error( set_err(EXECUTE_ERROR, "Error executing scp") );

  // I don't know why, but ssh sometimes returns an empty line in stderr.
  // Trying to remove it.
  if(!err.IsEmpty() && err[0].IsEmpty())
    err.RemoveAt(0);

  return execute_end(res, out, err);
}


int gmSShellUnix::scp_execute(const gmdString& cmd, bool upload){
  int res;
  gmdArrayString out, err;

  if(!auth_defined) {
    LOGJOBERR("Authentification method is not defined, "
      "check 'login', 'host' and 'ssh_args' parameters!");
    return -1;
  }

  if(dump_commands)
    LOGJOBMSG4( (pCSTR) ("------ pscp_execute ------\n"
                                 " cmd=" + scp_pre + cmd +"\n"
                        ));

  // Execute Shell command
  ttimer.Resume();
  res = gmdExecute(scp_pre + cmd, out, err);
  ttimer.Pause();
  if(res == -1) 
    return( set_err(EXECUTE_ERROR, "Error executing scp!") );

  if(dump_commands && res != 0)
    LOGJOBMSG4( (pCSTR) ("------ pscp_execute (error)------\n"
                         " cout=" + ArrayToString(out)+"\n"
                         " cerr=" + ArrayToString(err)+"\n"
              ));

  if(res) {
    gmdString errmsg = ArrayToString(err), str;
    res =
      ( errmsg.EndsWith(": No such file or directory") &&
        (  ( errmsg.StartsWith("scp: ") && !upload )   // missing remote path
         ||( !errmsg.StartsWith("scp: ") && upload ) ) // missing local path
      ) ?
    NO_INPUT_FILE :  COPY_ERROR;
    set_err(res, errmsg);
  }

  return res;
}


int gmSShellUnix::StageIn(pCSTR locpath, pCSTR rempath, unsigned flags){
  if( StageIn_begin(locpath, rempath, flags) )
    return StageIn_end(locpath, rempath, flags);
  
  gmdString opts;
  if(flags & RECURSIVE) opts = "-r ";
  scp_execute(opts + locpath + ' ' + userhost + ':' + rempath, true);
  
  return StageIn_end(locpath, rempath, flags);
}


int gmSShellUnix::StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags){
  gmdArrayString srclist;

  if( StageIn_begin(NULL, remdir, flags) )
    return StageIn_end(NULL, remdir, flags);

  for(unsigned i=0; i<locfiles.GetCount(); i++) {
    const gmdString& lfile = locfiles[i];
    if( has_wildcards(lfile) ) {
      gmdArrayString filelist;
      unsigned nfiles = files_by_mask(lfile, filelist, flags);
      if(nfiles)
        for(unsigned k=0; k<nfiles; k++) srclist.Add(filelist[k]);
      else
        set_err(NO_INPUT_FILE, fmt("%s: no such files", (pCSTR)lfile.c_str()));
    }
    else if( path_exists(lfile, flags) )
      srclist.Add(lfile);
    else
      set_err(NO_INPUT_FILE, fmt("%s: no sich file or directory", (pCSTR)lfile.c_str()));
  }
  
  if( !srclist.IsEmpty() ) {
    gmdString src;
    for(unsigned i=0; i<srclist.GetCount(); i++) src += srclist[i] + ' ';
    gmdString opts;
    if(flags & RECURSIVE) opts = "-r ";
    scp_execute(opts + src + ' ' + userhost + ':' + remdir, true);

    if(!error_code && (flags & MOVE))  // custom move (not using StageIn_end)
      for(unsigned i=0; i<srclist.GetCount(); i++) RemoveLocal(srclist[i], flags);
  }

  return StageIn_end(NULL, remdir, flags);
}


int gmSShellUnix::StageOut(pCSTR locpath, pCSTR rempath, unsigned flags){
  if( StageOut_begin(locpath, rempath, flags) )
    return StageOut_end(locpath, rempath, flags);

  gmdString opts;
  if(flags & RECURSIVE) opts = "-r ";
  scp_execute(opts + userhost + ':' + rempath + ' ' + locpath, false);
  
  return StageOut_end(locpath, rempath, flags);
}


int gmSShellUnix::StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags){
  gmdString srclist, rmlist;

  if( StageOut_begin(locdir, NULL, flags) )
    return StageOut_end(locdir, NULL, flags);

  for(unsigned i=0; i<remfiles.GetCount(); i++) {
    srclist += userhost + ':' + remfiles[i] + ' ';
    if(flags & MOVE) rmlist += remfiles[i] + ' ';
  }
  
  gmdString opts;
  if(flags & RECURSIVE) opts = "-r ";
  int res = scp_execute(opts + srclist + locdir, false);

  if(!res && (flags & MOVE)) res = Remove(rmlist);

  return StageOut_end(locdir, NULL, flags);
}
