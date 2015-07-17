/****************************************************************************
 * \file mingwshell.cpp
 * Implementation of the class gmShellMinGW
****************************************************************************/

#include <stdio.h>
#include <gmd/utils.h>
#include <gmd/filename.h>
#include "jobmngr/mingwshell.h"


gmShellMinGW::gmShellMinGW(pCSTR bash_path, pCSTR home_path) {
  // Global parameters which could be read from XML
  InitParam("rem_tmp_dir", "gmJobManager");  // will be created in user's home dir
  InitParam("rem_perm_dir", "");
  
  InitParam("bash_path", bash_path);
  InitParam("home_dir_win", home_path); // home path in Windows notation
  gmdString homedir(home_path);  // home path in Unix notation
  homedir.Replace("\\", "/");
  homedir.Replace(":", "");
  InitParam("home_dir", "/" + homedir);
}


void gmShellMinGW::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "bash_path")
    bash_cmd = par_value + " --login -c ";  // command line template

  gmShell::set_param(par_name, par_value);
}


// Execute Shell command
int gmShellMinGW::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  execute_begin(cmd, out, err);

  gmdString cmd_scr(cmd);
  cmd_scr.Replace("\"", "\\\"");  // screed double quotes

  int res = gmdExecute(bash_cmd + '\"' + cmd_scr + '\"', out, err);
  if(res == -1) LOGJOBERR("Error executing shell!"); // Fatal error

  execute_end(res, out, err);

  return res;
}


// Execute Shell command in background
int gmShellMinGW::ExecuteAsync(const gmdString& cmd){
  if(dump_commands)
    LOGJOBMSG4( (pCSTR)("------ _execute_async (cmd)------\n" + cmd) );

  gmdString cmd_scr(cmd);
  cmd_scr.Replace("\"", "\\\"");  // screed double quotes

  int pid = gmdExecute(bash_cmd + '\"' + cmd_scr + '\"');
  if(pid <= 0) LOGJOBERR("Error executing shell!"); // Fatal error

  return pid;
}


int gmShellMinGW::check_command_set(int cmd_set){
  return cmd_set != CMD_SET_MINGW;
}
