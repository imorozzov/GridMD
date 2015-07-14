/****************************************************************************
 * \file unixshell.cpp
 * Implementation of the class gmShellUnix
****************************************************************************/

#include <stdio.h>
#include <gmd/utils.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>
#include "jobmngr/unixshell.h"

int gmShellUnix::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  // Execute Shell command
  int res;

  execute_begin(cmd, out, err);

  gmdString cmd_scr(cmd);
  // Screen all '\' symbols as gmdExecute replaces all '\<char>' by '<char>'
  // when passing the parameters to execvp()
  cmd_scr.Replace("\\", "\\\\");
  cmd_scr.Replace("\"", "\\\"");  // screed double quotes

  // Bash is needed to process file masks
  res = command_local("cd; " + cmd_scr, out, err);
  
  if(res == -1)  // Fatal error
    return execute_error( set_err(EXECUTE_ERROR, "Error executing shell") );

  return execute_end(res, out, err);
}


int gmShellUnix::cp_execute(const gmdString& cmd) {
  gmdArrayString out, err;
  
  int res = command_local(cmd, out, err);
  
  if(res) {
    gmdString errmsg = ArrayToString(err);
    res = errmsg.EndsWith("No such file or directory") &&
          errmsg.StartsWith("cp: cannot stat") ?
      NO_INPUT_FILE : COPY_ERROR;
    set_err(res, errmsg);
  }

  return res;
}


int gmShellUnix::StageIn(pCSTR locpath, pCSTR rempath, unsigned flags){
  gmdString rpath(get_full_path(rempath));

  if( StageIn_begin(locpath, rpath, flags) )
    return StageIn_end(locpath, rpath, flags);

  gmdString cmd = (flags & RECURSIVE) ? "cp -fr " : "cp -f ";
  cp_execute(cmd + locpath + " " + rpath);
  
  return StageIn_end(locpath, rpath, flags);
}


int gmShellUnix::StageOut(pCSTR locpath, pCSTR rempath, unsigned flags){
  gmdString rpath(get_full_path(rempath));

  if( StageOut_begin(locpath, rpath, flags) )
    return StageOut_end(locpath, rpath, flags);

  gmdString cmd = (flags & RECURSIVE) ? "cp -fr " : "cp -f ";
  cp_execute(cmd + rpath + " " + locpath);
  
  return  StageOut_end(locpath, rpath, flags);
}
