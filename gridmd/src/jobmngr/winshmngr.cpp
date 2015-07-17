/****************************************************************************
 * \file winshmngr.cpp
 * Implementation of the Windows MinGW Job Manager
****************************************************************************/

#include <stdio.h>

#include <gmd/tokenzr.h>
#include <gmd/utils.h>
#include <gmd/filefn.h>

#include "jobmngr/winshmngr.h"
#include <windows.h>  //moved after wxWidgets includes to prevent conflicts with UNICODE version


// Initialize job manager, set default parameter values
void gmWinShManager::init() {
  gmJobManager::init();
  InitParam("script_type", "windows");
  InitParam("stat_read_delay", "200");
  InitParam("stat_read_ntries", "20");

  make_cmd_script = true;
}


void gmWinShManager::set_param(const gmdString& par_name, const gmdString& par_value){
  bool err = false;
  if(par_name == "script_type") {
    if(par_value == "windows") script_type = SCRIPT_TYPE_WIN;
    else if(par_value == "bash") script_type = SCRIPT_TYPE_BASH;
    else err = true;
  }
  else if(par_name == "stat_read_delay")
    stat_read_delay = ParseIntParam(par_name, par_value, 0);
  else if(par_name == "stat_read_ntries")
    stat_read_ntries = ParseIntParam(par_name, par_value, 1);
  else
    gmJobManager::set_param(par_name, par_value);

  if(err) InvalidParValue(par_name, par_value);
}


gmdString gmWinShManager::get_user_cmd(gmJob& job, gmdString& user_cmd, bool make_script) {
  gmdString remote_cmd;

  if(script_type == SCRIPT_TYPE_WIN) {
    gmdString cmd = job.command;
    cmd.Replace("'", "'\"'\"'");  // screen single quotes

    gmdString jobdir = shell->get_full_path_win(job._dir());
    jobdir.Replace("\\", "\\\\\\\\");
    jobdir.Replace(" ", "\\\\ ");
    user_cmd = "cmd //c " + jobdir + "\\\\\\\\user_cmd.bat\n";
    remote_cmd =
      + "echo '"
        "@echo off\n"
         + cmd + "\n' >user_cmd.bat\n";
  } else
    remote_cmd = gmJobManager::get_user_cmd(job, user_cmd, make_script);

  return remote_cmd;
}


// Prepare submission command
// "\n" should replaced by "\\n"
// "'" should replaced by "\\q"
gmdString gmWinShManager::get_subm_cmd(gmJob& job){
  (void) job;
  return gmdString() +
    "echo $$ >" + status_file + "; "
    "./" + script_file + " >" + stdout_file + " 2>" + stderr_file;
}


// Return main script body
gmdString gmWinShManager::get_script_body(gmJob& job, gmdString& user_cmd){
  return
    "#!/bin/bash\n"
    "SFILE=" + shell->get_full_path(job._dir()) + "/" + status_file + "\n"
    "echo JOB_SUBMITTED >>$SFILE\n"
    "cd " + shell->get_full_path(job._wdir()) + "\n"
    + (init_job_cmd.IsEmpty() ? "" : init_job_cmd + "\n")
    + "if ( " + user_cmd + " )\n"
      "then STAT=JOB_COMPLETED; else STAT=JOB_FAILED; fi\n"
    + (end_job_cmd.IsEmpty() ? "" : end_job_cmd + "\n")
    + "echo $STAT >>$SFILE";
}


// Execute submission command and save internal ID or return to JOB_PREPARED
int gmWinShManager::exec_subm_cmd(gmJob& job, const gmdString& cmd){
  // Execute script asynchronously
  if(shell->ExecuteAsync(cmd) <= 0)
    LOGJOBERR(fmt("ExecuteAsync failed for job id = %s", job._id()));

  // Wait until the process is started and read its pid from 'stat' file
  // gmdFFile and gmdTextFile cannot be used as the file change during read
  // lead to assertion break
  gmdString filename(shell->get_full_path_win(job._dir()) + "/" + status_file);
  FILE* sfile;
  gmdString pid, state;
  int i;
  const int max_line_len = 64;
  char str[max_line_len];
  for(i = 0; i < stat_read_ntries; i++) {
    if( gmdFileExists(filename) && (sfile = fopen(filename,"rt")) ) {
      pid = fgets(str, max_line_len, sfile) ? str : "";
      state = fgets(str, max_line_len, sfile) ? str : "";
      fclose(sfile);
      //printf("--- Read from stat file at i = %d:\n%s%s", i, (pCSTR)pid.c_str(), (pCSTR)state.c_str());
      pid.Trim();
      if(state == "JOB_SUBMITTED\n" && pid[0] >= '0' && pid[0] <= '9') break;
    }
    gmdMilliSleep(stat_read_delay);
  }
  //printf("--- Job started i = %d, pid = %s\n", i, (pCSTR)pid.c_str());

  if(i >= stat_read_ntries)
    LOGJOBERR(fmt("Job start timeout exceeded for job id = %s", job._id()));

  // Save job PID read from the 'stat' file
  SetJobData(job, 0, 0, 0, 0, pid);
  return JOB_SUBMITTED;
}


gmdString gmWinShManager::get_status_cmd(){
  return 
    "ps -W | sed -r 's/^.{26}(.{11}).*/\\1/'"
    "| grep `head -n 1 " + gmdString(status_file) + "`";
}


int gmWinShManager::interpret_status(const gmdString& str){
  int res;

  if( str.IsEmpty() || str == "JOB_FAILED" ) res = JOB_FAILED;
  else if( str == "JOB_COMPLETED" ) res = JOB_COMPLETED;
  else res = JOB_RUNNING;

  return res;
}


gmdString gmWinShManager::get_stop_cmd(gmJob& job){
  gmdArrayString out, err;
  int res = shell->Execute(  // select all processes with the same PGID
    "ps -W | grep ' " + job._subm_id() + " ' | sed -r 's/^.{26}(.{11}).*/\\1/'",
    out, err);
  if(res) LOGJOBERR("Process query failed");
  
  for(unsigned i=0; i<out.Count(); i++) {
    long pid;
    if(out[i].Trim().Trim(false).ToLong(&pid) && pid) gmdKill(pid, gmdSIGKILL);
    //printf("Killing process %d\n", pid);
  }

  return "";
}
