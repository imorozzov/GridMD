/****************************************************************************
 * \file pbsmngr.cpp
 * Implementation of PBS Job Manager
****************************************************************************/

#include "jobmngr/pbsmngr.h"


// Initialize job manager, set default parameter values
void gmPBSManager::init() {
  gmJobManager::init();
  InitParam("mpi_run_cmd", "mpiexec");
  InitParam("mpi_args", "");
  InitParam("qsub_args", "");
  InitParam("job_name_prefix", "");
  InitParam("pbs_path", "");
}


void gmPBSManager::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "mpi_run_cmd")
    mpi_run_cmd = par_value + " ";
  else if(par_name == "mpi_args")
    mpi_args = par_value + " ";
  else if(par_name == "pbs_path") {
    cmd_prefix = par_value;
    if(!cmd_prefix.IsEmpty() && !cmd_prefix.EndsWith("/")) cmd_prefix += "/";
  }
  else
    gmJobManager::set_param(par_name, par_value);
}


// Prepare submission command
// "\n" should replaced by "\\n"
// "'" should replaced by "\\q"
gmdString gmPBSManager::get_subm_cmd(gmJob& job){

  // Use (param["job_name_prefix"] + id) as the job name
  gmdString subm_cmd =
    "if JOBID=`"
      + cmd_prefix + "qsub "
      "-N " + param["job_name_prefix"] + job._id() + " "
      + param["qsub_args"];

  int nodes = job.nproc, ppn = job.ppn, nextra = 0;
  if(job.nproc && ppn) {
    nodes = job.nproc / ppn;
    if(nodes) nextra = job.nproc % ppn;
    else nodes = 1, ppn = job.nproc;
  }

  // Process additional job parameters
  if(nodes || job.walltime) {
    subm_cmd += " -l ";
    if(nodes) {
      subm_cmd += gmdString::Format("nodes=%d", nodes);
      if(ppn) subm_cmd += gmdString::Format(":ppn=%d", ppn);
      if(nextra) subm_cmd += gmdString::Format("+1:ppn=%d", nextra);
      if(job.walltime) subm_cmd += ",";
    }
    if(job.walltime) subm_cmd += gmdString::Format("walltime=%d", job.walltime);
  }
  
  // Redirect output and save qsub result
  subm_cmd += gmdString() +
      " -o " + stdout_file + " -e " + stderr_file + " "
      + script_file
    + "`; then "
      "echo $JOBID; echo $JOBID >" + status_file + "; echo JOB_SUBMITTED >>" + status_file;
  if(log_level & LOG_TIMING)
    subm_cmd += gmdString("; echo SUBMITTED `date +\"%s %N\"` >>") + log_file;
  subm_cmd += "; fi";

  return subm_cmd;
}


// Return main script body
gmdString gmPBSManager::get_script_body(gmJob& job, gmdString& user_cmd){
  return
    "#!/bin/bash\n"
    + (log_level & LOG_TIMING ?
      gmdString("echo STARTED `date +\"%s %N\"` >>")
      + shell->get_full_path(job._dir()) + "/" + log_file + "\n" : "")
    + "SFILE=" + shell->get_full_path(job._dir()) + "/" + status_file + "\n"
    "cd " + shell->get_full_path(job._wdir()) + "\n"
    + (init_job_cmd.IsEmpty() ? "" : init_job_cmd + "\n")
    + "if ( " + (job.mpi ? mpi_run_cmd + mpi_args : "") + user_cmd + " )\n"
      "then STAT=JOB_COMPLETED; else STAT=JOB_FAILED; fi\n"
    + (end_job_cmd.IsEmpty() ? "" : end_job_cmd + "\n")
    + "echo $STAT >>$SFILE\n"
    + (log_level & LOG_TIMING ?
      gmdString("echo ENDED `date +\"%s %N\"` >>")
      + shell->get_full_path(job._dir()) + "/" + log_file + "\n" : "");
}


// Execute submission command and save internal ID or return to JOB_PREPARED
int gmPBSManager::exec_subm_cmd(gmJob& job, const gmdString& cmd){
  gmdArrayString out, err;

  if( !shell->Execute(cmd, out, err) && err.IsEmpty() && out.GetCount() == 1 &&
      out[0][0] >= '0' && out[0][0] <= '9' ){ // no errors & valid job id is returned
    SetJobData(job, 0, 0, 0, 0, out[0]);
    return JOB_SUBMITTED;
  }

  // Check if the queue is overloaded
  return
    (!err.IsEmpty() && err[0] == "qsub: Maximum number of jobs already in queue") ?
      JOB_PREPARED : JOB_FAILED;
}


// Get status line
gmdString gmPBSManager::get_status_cmd(){
  return
    cmd_prefix + "qstat -f `head -n 1 " + status_file + "`  2>/dev/null | grep 'job_state ='";
}


// Analyze status line
int gmPBSManager::interpret_status(const gmdString& str){
  int i = str.Find("job_state =");
  if(i != gmdNOT_FOUND)
    switch( (char)str[i+12] ) {
      case 'W': return JOB_SUBMITTED; // job is waiting for its execution time
      case 'H':                       // Job is held
      case 'Q': return JOB_QUEUED;    // job is queued, eligible to run or routed
      case 'R':                       // job is running
      case 'T': return JOB_RUNNING;   // job is being moved to new location
      case 'S': return JOB_SUSPENDED; // job is suspend (Unicos only)
      case 'E': return JOB_EXITING;   // job is exiting after having run
      case 'C': return JOB_COMPLETED; // job is marked as completed
    }
  return JOB_INIT;  // no such job in queue
}


gmdString gmPBSManager::get_stop_cmd(gmJob& job){
  return cmd_prefix + "qdel " + job._subm_id();
}
