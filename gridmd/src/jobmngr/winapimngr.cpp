/****************************************************************************
 * \file winapimngr.cpp
 * Implementation of the Windows API Job Manager
****************************************************************************/

#include <stdio.h>
#include <gmd/tokenzr.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>
#include <gmd/ffile.h>
//#include <gmd/log.h>
#include <gmd/utils.h>
#include <windows.h>

#include "jobmngr/winapimngr.h"


// Initialize job manager, set default parameter values
void gmWinAPIManager::init() {
  gmJobManager::init();
  InitParam("mpi_run_cmd", "mpiexec.exe");
  InitParam("mpi_args", "");
  InitParam("wait_delay", "500");
  InitParam("kill_delay", "100");
  InitParam("kill_ntries", "20");
}


void gmWinAPIManager::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "mpi_run_cmd")
    mpi_run_cmd = par_value + " ";
  else if(par_name == "mpi_args")
    mpi_args = par_value + " ";
  else if(par_name == "kill_delay")
    kill_delay = ParseIntParam(par_name, par_value, 0);
  else if(par_name == "kill_ntries")
    kill_ntries = ParseIntParam(par_name, par_value, 1);
  else
    gmJobManager::set_param(par_name, par_value);
}


int gmWinAPIManager::Submit(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only){
  gmdString id, jobdir, workdir;
  gmdString& tmpdir = rem_tmp_dir;
  int res;

  // Check parameters
  assert_no_specials(user_id, "user_id");

  gmdString mpi_cmd;
  if(job.mpi) {
    if(!mpi_enabled) {
      LOGJOBWARN("MPI jobs are disabled!");
      return 1;
    }
    if(job.nproc == 0) {
      LOGJOBWARN("MPI job is submitted with nproc = 0");
      return 1;
    }
    mpi_cmd = mpi_run_cmd + mpi_args + gmdString::Format("-n %d ", job.nproc);
    if(job.ppn) mpi_cmd += gmdString::Format("-cores %d ", job.ppn);
  }

  // Create common job dir if needed
  if( !gmdDirExists(tmpdir) && shell->MkDir(tmpdir) )
    LOGJOBERR(fmt("Unable to create directory %s", (pCSTR)tmpdir.c_str()));

  // Create a unique job directory with random or user-defined job id
  jobdir = tmpdir + "\\job-" + user_id;
  gmdString tmpfile;
  if(mktemp) {
    tmpfile = gmdFileName::CreateTempFileName(tmpdir + "\\");
    if( jobdir.IsEmpty() ) 
      LOGJOBERR(fmt("Unable to create temporary file in %s", (pCSTR)tmpdir.c_str()));
    gmdFileName fn(tmpfile);
    jobdir += "-" + fn.GetName();
  }

  if( shell->MkDir(jobdir) ) {
    LOGJOBERR(fmt("Unable to temporary directory %s", (pCSTR)jobdir.c_str()));
    return 1;
  }
  if( !tmpfile.IsEmpty() ) gmdRemoveFile(tmpfile);

  // Extract job id
  id = jobdir.AfterLast('\\').Mid(4);

  // Create or assign work directory
  res = 0;
  if( job.forerunner.IsEmpty() ){
    workdir = jobdir + "\\work";
    res |= shell->MkDir(workdir);
  } else {
    gmdString ljobdir = tmpdir + "\\job-" + job.forerunner;
    if( !gmdDirExists(ljobdir) )
      LOGJOBERR(fmt("Job link failed: job %s doesn't exists!", (pCSTR)job.forerunner.c_str()));
    
    // Get linked job workdir
    workdir = gmdFileExists(ljobdir + "\\" + workdir_file) ?
      ReadFile(ljobdir + "\\" + workdir_file) :
      ljobdir + "\\work";

    // Create wd file
    gmdFFile file(jobdir + "\\" + workdir_file, "wt");
    res |= !file.Write(workdir);
  }
  if(res) LOGJOBERR(fmt("Unable to create work fir for job %s!", (pCSTR)id.c_str()));

  // Update job object and save job info
  SetJobData(job, id, jobdir, workdir);
  jobs.push_back(&job);

  // Stage in input files
  if( StageAllInput(job, gmdString()) )
    LOGJOBERR("Unable to copy local input files");

  // Save Job info
  if(save_job_info){
    gmdFFile file(jobdir + "\\" + info_file, "wt");
    file.Write(
      "Client user name:   " + gmdGetUserName() + "\n"
      "Client host name:   " + gmdGetHostName() + "\n"
      "Client OS:          " + gmdGetOsDescription() + "\n"
      "Shell class name:   " + shell->GetClassName() + "\n"
      "Manager class name: " + GetClassName() + "\n"
      "Job forerunner:     " + job.forerunner + "\n"
      + gmdString::Format(
        "Job run info:       mpi=%d, nproc=%d, ppn=%d, walltime=%d",
        job.mpi, job.nproc, job.ppn, job.walltime) + "\n"
      "Job extra info:     " + job.info + "\n"
    );
  }

  // Prepare the main start script
  gmdString sfile = jobdir + "\\" + status_file;
  gmdString rfile = jobdir + "\\" + script_file + ".cmd";

  gmdFFile file(rfile, "wt");
  file.Write(
    "@echo off\n"
    + (workdir.Contains(":") ? workdir.BeforeFirst(':') + ":\n" : "")  // change disk
    + "cd \"" + workdir + "\"\n"
    + (init_job_cmd.IsEmpty() ? "" : init_job_cmd + "\n")
    + mpi_cmd + job.command + "\n"
    "if errorlevel 1 goto JOB_FAILED\n"
    "echo JOB_COMPLETED >\"" + sfile + "\"\n"
    "goto ENDING\n"
    ":JOB_FAILED\n"
    "echo JOB_FAILED >\"" + sfile + "\"\n"
    ":ENDING\n"
    + (end_job_cmd.IsEmpty() ? "" : end_job_cmd + "\n")
  );
  file.Close();

  if(prepare_only) {
    gmdFFile file(sfile, "wt");
    file.Write("JOB_PREPARED");
  }

  return prepare_only ? JOB_PREPARED : RemoteSubmit(job, "");
}


// Run submission command for the remote batch system
int gmWinAPIManager::RemoteSubmit(gmJob& job, const gmdString& cmd){
  int res;
  if(job.type & gmJob::DUMMY) {
    gmdFFile file(job._dir() + "\\" + status_file, "wt");
    if( file.IsOpened() ) {
      file.Write("JOB_COMPLETED");
      file.Close();
      res = JOB_COMPLETED;
    } else
      res = JOB_FAILED;
  } else
    res = exec_subm_cmd(job, cmd);

  return res;
}


// Execute submission command and save internal ID or return to JOB_PREPARED
int gmWinAPIManager::exec_subm_cmd(gmJob& job, const gmdString& cmd){
  // Change status in file
  gmdFFile file(job._dir() + "\\" + status_file, "wt");
  file.Write("JOB_SUBMITTED");
  file.Close();

  // Execute script asynchronously
  int pid = shell->ExecuteAsync(
    "\"" + job._dir() + "\\" + script_file + ".cmd\""
    " >\"" + job._dir() + "\\" + stdout_file + "\""
    " 2>\"" + job._dir() + "\\" + stderr_file + "\""
    " <Nul"
  );

  if(pid > 0){
    // Save pid to 'pid' file
    gmdString subm_id = gmdString::Format("%d", pid);
    gmdFFile file(job._dir() + "\\pid", "wt");
    file.Write(subm_id);
    SetJobData(job, 0, 0, 0, 0, subm_id);
  }
  else LOGJOBERR(fmt("ExecuteAsync failed for job id = %s", job._id()));

  return JOB_SUBMITTED;
}


// Get status and pid
int gmWinAPIManager::Status(gmJob& job) {
  gmdString sfile = job._dir() + "\\" + status_file;
  if( !gmdFileExists(sfile) ) return JOB_FAILED;

  gmdString stat = ReadFile(sfile);
  //printf("--- stat file = %s\n", stat.c_str());

  int res;
  if(stat == "JOB_PREPARED") res = JOB_PREPARED;
  else if(stat == "JOB_COMPLETED") res = JOB_COMPLETED;
  else if(stat == "JOB_SUBMITTED") {
    int pid = job_pid(job);
    res = (pid && ProcessRunning(pid)) ? JOB_RUNNING : JOB_FAILED;
  }
  else res = JOB_FAILED;

  return res;
}


int gmWinAPIManager::Stop(gmJob& job){
  int state = Status(job);

  if(state == JOB_RUNNING) SafeProcessKill( job_pid(job) );

  return (state == JOB_COMPLETED) ? JOB_COMPLETED : JOB_FAILED;
}


int gmWinAPIManager::Attach(gmJob& job, pCSTR id){
  gmdString jobdir = rem_tmp_dir + "\\job-" + id;
  gmdString sfile = jobdir + "\\" + status_file;

  if( !gmdFileExists(sfile) ) return JOB_INIT;
  
  gmdString workdir;
  if( gmdFileExists(jobdir + "\\" + workdir_file) ) {
    workdir = ReadFile(jobdir + "\\" + workdir_file);
    job.forerunner = workdir.Mid(rem_tmp_dir.Length() + 5).BeforeFirst('\\');
  }
  else {
    workdir = jobdir + "\\work";
    job.forerunner = "";
  }

  gmdString subm_id = gmdFileExists(jobdir + "\\pid") ?
    ReadFile(jobdir + "\\pid") : "";

  SetJobData(job, id, jobdir, workdir, "", subm_id);
  jobs.push_back(&job);

  return Status(job);
}


/// Stage all local input files for the given job
/// and create a copying command for the remote system (remcmd)
/// Returns 0 on success.
/// Assumes that the corrent dir is $HOME
int gmWinAPIManager::StageAllInput(gmJob& job, gmdString& remcmd){
  int res = 0;

  FOR_EACH_LOOP(gmFileList, job.infiles, file) {
    // Append the path to working dir if needed
    gmdString rpath = file->rpath;
    if( !(rpath.StartsWith("\\") || rpath.Contains(":")) )
      rpath = job._wdir() + '\\' + rpath;
      
    res |= shell->StageIn(file->lpath, rpath,
      file->flags & ~gmJob::REMOTE );
  }
  
  return res;
}


/// Stage a local output file for the given job
/// ore create a copying command for the remote system (remcmd)
/// Returns 0 on success and -1 if the remote command is created
/// Assumes that the current dir is $HOME
int gmWinAPIManager::StageOutputFile(gmJob& job, gmFileTrans& file, gmdString& remcmd){
  int res = 0;
  gmdString rpath(file.rpath);

  if( rpath.IsEmpty() ) return 1;

  if(rpath == "STDOUT")
    rpath = job._dir() + '\\' + GetStdOut(job), file.flags |= gmJob::TEXT;
  else if(rpath == "STDERR")
    rpath = job._dir() + '\\' + GetStdErr(job), file.flags |= gmJob::TEXT;
  else if( !(rpath.StartsWith("\\") || rpath.Contains(":")) )
    rpath = job._wdir() + '\\' + rpath;

  if(!res) res = shell->StageOut(file.lpath, rpath,
      file.flags & ~gmJob::REMOTE );

  return res;
}

// ============================================================================
// Batch functions for a job list
// ============================================================================

int gmWinAPIManager::WaitAll(pCSTR id_prefix){
  JobList pjobs, rjobs;
  gmdStopWatch timer;
  int res = JOB_COMPLETED, state;
  int not_first_iter = 0;
  wait_time = 0;

  // Create list of the selected jobs
  FOR_EACH_LOOP(JobList, jobs, j)
    if( !id_prefix || (*j)->GetID().StartsWith(id_prefix) ) pjobs.push_back(*j);

  if( pjobs.empty() ) return JOB_INIT;

  while( wait_timeout == 0 || timer.Time() < wait_timeout ){
    // Process the current list
    rjobs.clear();
    for(JobList::iterator it=pjobs.begin(); it!=pjobs.end();){
      gmJob* j = *(it++);
      state = j->_state();
      if(state == JOB_FAILED) res = JOB_FAILED;
      if(state >= JOB_COMPLETED) pjobs.remove(j);  // remove all completed jobs
      else if(state >=JOB_SUBMITTED) rjobs.push_back(j);
    }

    if( pjobs.empty() ) break;  // normal loop exit

    // DEBUG!!!
    //printf("--- Jobs submitted: %d, Jobs prepared: %d\nWaiting for jobs:", jobs_subm, jobs_wait);
    //  FOR_EACH_LOOP(JobList, jobs, j) printf(" %s (state=%d)", (*j)->GetID().c_str(), (*j)->_state());
    //puts("");

    if(not_first_iter) gmdMilliSleep(wait_delay);  // wait 3 seconds before the next try
    else not_first_iter = true;

    // Get status of the running jobs
    FOR_EACH_LOOP(JobList, rjobs, j)
      state = (*j)->GetState();
    drain_prepared();  // try to resubmit PREPARED jobs
  }

  wait_time = timer.Time();
  return res;
}


void gmWinAPIManager::StopAll(pCSTR id_prefix){
  FOR_EACH_LOOP(JobList, jobs, j)
    if( !id_prefix || (*j)->GetID().StartsWith(id_prefix) ) (*j)->Stop();
}


// ============================================================================
// Service functions
// ============================================================================

// Returns the whole file as a single string
gmdString gmWinAPIManager::ReadFile(pCSTR filename){
  gmdString str;
  gmdFFile file(filename, "rt");
  file.ReadAll(&str);
  return str.Trim();
}


// Check if the process exists using Win32 API
bool gmWinAPIManager::ProcessRunning(int pid){
  bool res = false;

  HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
  if(h) {
    DWORD exitcode;
    res = GetExitCodeProcess(h, &exitcode) ? (exitcode == STILL_ACTIVE) : 0;
    CloseHandle(h);
  }
  return res;
}

void gmWinAPIManager::SafeProcessKill(int pid){
  gmdKill(pid, gmdSIGKILL, 0, gmdKILL_CHILDREN);

  int i;
  for(i = 0; i < kill_ntries && ProcessRunning(pid); i++)
    gmdMilliSleep(kill_delay);
  if(i >= kill_ntries) LOGJOBERR(fmt("Failed to kill process %d!", pid));
}
