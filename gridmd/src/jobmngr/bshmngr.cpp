/****************************************************************************
 * \file bshmngr.cpp
 * Implementation of bash shell Job Manager
****************************************************************************/

#include "jobmngr/bshmngr.h"


// Prepare submission command
// "\n" should replaced by "\\n"
// "'" should replaced by "\\q"
gmdString gmBShManager::get_subm_cmd(gmJob& job){
  (void) job;
  return gmdString() +
    "nohup ./" + script_file + " >" + stdout_file + " 2>" + stderr_file + " &\\n"
    "echo $!";
}


// Return main script body
gmdString gmBShManager::get_script_body(gmJob& job, gmdString& user_cmd){
  return
    "#!/bin/bash\n"
    "SFILE=" + shell->get_full_path(job._dir()) + "/" + status_file + "\n"
    "echo $$ >$SFILE\n"
    "echo JOB_SUBMITTED >>$SFILE\n"
    "cd " + shell->get_full_path(job._wdir()) + "\n"
    + (init_job_cmd.IsEmpty() ? "" : init_job_cmd + "\n")
    //+ "if ( " + (job.mpi ?
    //             gmdString::Format("%s -np %d ", param["mpi_run_cmd"].c_str(), job.nproc) : "")
    //   + user_cmd + " )\n"
    + "if ( " + user_cmd + " )\n"
      "then STAT=JOB_COMPLETED; else STAT=JOB_FAILED; fi\n"
    + (end_job_cmd.IsEmpty() ? "" : end_job_cmd + "\n")
    + "echo $STAT >>$SFILE";
}


// Execute submission command and save internal ID or return to JOB_PREPARED
int gmBShManager::exec_subm_cmd(gmJob& job, const gmdString& cmd){
  gmdArrayString out, err;
  if( shell->Execute(cmd, out, err) || !err.IsEmpty() || out.IsEmpty() )
    return JOB_FAILED;

  SetJobData(job, 0, 0, 0, 0, out[0]);
  return JOB_SUBMITTED;
}


gmdString gmBShManager::get_status_cmd(){
  return
    "ps h p `head -n 1 " + gmdString(status_file) + "` 2>/dev/null";
}


int gmBShManager::interpret_status(const gmdString& str){
  return (!str.IsEmpty() && !str.StartsWith("JOB")) ?
    JOB_RUNNING : JOB_INIT;
}


gmdString gmBShManager::get_stop_cmd(gmJob& job){
  return "kill -9 " + job._subm_id();
}
