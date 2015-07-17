/****************************************************************************
 * \file slurmmngr.cpp
 * Implementation of SLURM Job Manager
****************************************************************************/

#include <gmd/filename.h>
#include <gmd/utils.h>
#include <gmd/regex.h>
#include <iostream>

#include "jobmngr/slurmmngr.h"

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif


// Initialize job manager, set default parameter values
void gmSLURMManager::init() {
  gmJobManager::init();
  InitParam("slurm_cmd_serial", "srun");
  InitParam("slurm_cmd_mpi", "srun");
  InitParam("sbatch_args", "");
  InitParam("job_name_prefix", "");
  InitParam("slurm_path", "");

  make_cmd_script = true;
}


void gmSLURMManager::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "slurm_path") {
    cmd_prefix = par_value;
    if(!cmd_prefix.IsEmpty() && !cmd_prefix.EndsWith("/")) cmd_prefix += "/";
  }
  else
    gmJobManager::set_param(par_name, par_value);
}


// Prepare submission command
// "\n" should replaced by "\\n"
// "'" should replaced by "\\q"
gmdString gmSLURMManager::get_subm_cmd(gmJob& job){
  return gmdString() +
    "if JOBID=`"
      + cmd_prefix + "sbatch -Q "
      "-J " + param["job_name_prefix"] + job._id() + " "
      + param["sbatch_args"] + " "
      + (job.nproc ? gmdString::Format("-n%d ", job.nproc) : "")
      + (job.nproc && job.ppn ? gmdString::Format("--ntasks-per-node=%d ", job.ppn) : "")
      + (job.walltime ? gmdString::Format("-t%d:%d ", job.walltime/60, job.walltime%60) : "")
      + "-o " + stdout_file + " -e " + stderr_file + " "
      + "./"+script_file
    + "`; then "
      "echo $JOBID | sed -r \\qs/Submitted batch job ([0-9]+)/\\1/\\q >" + status_file
      + "; echo JOB_SUBMITTED >>" + status_file
      + "; head -n 1 " + status_file
    + "; fi";
}


// Prepare start script
gmdString gmSLURMManager::get_script_body(gmJob& job, gmdString& user_cmd){
  return
    "#!/bin/bash\n"
    "SFILE=" + shell->get_full_path(job._dir()) + "/" + status_file + "\n"
    "cd " + shell->get_full_path(job._wdir()) + "\n"
    + (init_job_cmd.IsEmpty() ? "" : init_job_cmd + "\n")
    + "if ( " + 
      param[job.mpi ? "slurm_cmd_mpi" : "slurm_cmd_serial"]
      + " " + user_cmd + " )\n"
    "then STAT=JOB_COMPLETED; else STAT=JOB_FAILED; fi\n"
    + (end_job_cmd.IsEmpty() ? "" : end_job_cmd + "\n")
    + "echo $STAT >>$SFILE";
}


// Execute submission command and save internal ID or return to JOB_PREPARED
int gmSLURMManager::exec_subm_cmd(gmJob& job, const gmdString& cmd){
  gmdArrayString out, err;

  if( !shell->Execute(cmd, out, err) && err.IsEmpty() && !out.IsEmpty() ) {
    gmdString id = out[0].AfterLast(' ');
    if( id[0] >= '0' && id[0] <= '9' ){ // no errors & valid job id is returned
      SetJobData(job, 0, 0, 0, 0, id);
      return JOB_SUBMITTED;
    }
  }

  return JOB_FAILED;
}


gmdString gmSLURMManager::get_status_cmd(){
  return
    cmd_prefix + "squeue -h -o %%t -j `head -n 1 " + status_file + "` 2>/dev/null";
}


// Analyze output of the script
int gmSLURMManager::interpret_status(const gmdString& str){
  // TODO: remove debug print
    LOGMSG( vblMESS3, (pCSTR)("------ gmSLURMManager::interpret_status str='"+str+"'\n") , 1  );
  //LOGJOBMSG( (pCSTR)("-- gmSLURMManager::interpret_status : '"+str+"'" ).c_str() );

  int state;
  if( str == "PD" || str == "CF" ) state = JOB_QUEUED;
  else if( str == "R" ) state = JOB_RUNNING;
  else if( str == "S" ) state = JOB_SUSPENDED;
  else if( str == "CG" ) state = JOB_EXITING;
  else state = JOB_INIT;

  return state;
}


gmdString gmSLURMManager::get_stop_cmd(gmJob& job){
  return cmd_prefix + "scancel " + job._subm_id();
}
