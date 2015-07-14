/****************************************************************************
 * \file jobmngr.cpp
 * Implementation of gmJob class and gmJobManger service functions
****************************************************************************/

#include <map>

#include <gmd/filefn.h>
#include <gmd/filename.h>
#include <gmd/utils.h>
#include <gmd/tokenzr.h>

#include "jobmngr/jobmngr.h"

using namespace std;


// Internal static variables
pCSTR gmJobManager::script_file = "run";
pCSTR gmJobManager::user_cmd_file = "cmd";
pCSTR gmJobManager::status_file = "stat";
pCSTR gmJobManager::log_file = "log";
pCSTR gmJobManager::stdout_file = "out";
pCSTR gmJobManager::stderr_file = "err";
pCSTR gmJobManager::workdir_file = "wd";
pCSTR gmJobManager::info_file = "info";


/****************************************************************************
 * Implementation of gmJobManager class
****************************************************************************/


void gmJobManager::init() {
  InitParam("save_job_info", "false");
  InitParam("jobs_limit", "0");
  InitParam("wait_timeout", "0");
  InitParam("wait_delay", "3000");
  InitParam("init_job_cmd", "");
  InitParam("end_job_cmd", "");
  InitParam("pre_subm_cmd", "");
  InitParam("mpi_enabled", UNINITIALIZED_PARAM);
};


void gmJobManager::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "save_job_info")
    save_job_info = ParseBoolParam(par_name, par_value);
  else if(par_name == "jobs_limit")
    jobs_limit = ParseIntParam(par_name, par_value);
  else if(par_name == "wait_timeout")
    wait_timeout = ParseIntParam(par_name, par_value, 0);
  else if(par_name == "wait_delay")
    wait_delay = ParseIntParam(par_name, par_value, 0);
  else if(par_name == "init_job_cmd") init_job_cmd = par_value;
  else if(par_name == "end_job_cmd") end_job_cmd = par_value;
  else if(par_name == "pre_subm_cmd") {
    if( par_value.Contains("\n") || par_value.Contains("\r") )
      LOGJOBERR("'pre_subm_cmd' should not contain CR/LF symbols!");
    pre_subm_cmd = par_value;
  }
  if(par_name == "mpi_enabled") {
    if(par_value == UNINITIALIZED_PARAM) {
      mpi_enabled = mpi_support();
      param[par_name] = (mpi_enabled ? "true" : "false");
    }
    else {
      bool value = ParseBoolParam(par_name, par_value);
      if(value && !mpi_support())
        LOGJOBERR(fmt("Job manager %s does not support MPI jobs!", GetClassName()));
      else
        mpi_enabled = value;
    }
  }
  else
    gmJobManagerBase::set_param(par_name, par_value);
}


void gmJobManager::Open(gmShell& shell_) {
  shell = &shell_;
  if( shell->check_command_set(required_cmd_set()) )
    LOGJOBERR("Job manager and shell do not conform!");
  
  rem_tmp_dir = shell->GetParam("rem_tmp_dir");
};


// Prepare job, copy input files and submit it provided prepare_only=false
int gmJobManager::Submit(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only){
  gmdString id, jobdir, workdir, remote_cmd, rem_copy;
  gmdArrayString out, err;

  if( !(required_cmd_set() == gmShell::CMD_SET_BASH
    || required_cmd_set() == gmShell::CMD_SET_MINGW) )
    LOGJOBERR("Remote server does not support Bash commands");

  // Check parameters
  if(!user_id) user_id = "";
  assert_no_specials(user_id, "user_id");

  if(job.mpi && !mpi_enabled) {
    LOGJOBWARN("MPI jobs are disabled!");
    return 1;
  }

  if(job.mpi && job.nproc == 0) {
    LOGJOBWARN("MPI job is submitted with nproc = 0");
    return 1;
  }

  // Create and go to a common dir for job info
  remote_cmd =
    "PDIR=" + rem_tmp_dir + "; mkdir -p $PDIR\n";

  // Create a unique job directory with random or user-defined job id
  remote_cmd += gmdString::Format(
    mktemp ?
      "JDIR=`mktemp -d $PDIR/job-%s-XXXXXXXXXX`\n" :
      "JDIR=$PDIR/job-%s; mkdir $JDIR\n",
    user_id);
  remote_cmd += "echo $JDIR\n";

  // Create or find work directory
  if( job.forerunner.IsEmpty() ) 
    remote_cmd +=
      "WDIR=$JDIR/work; mkdir $WDIR; echo $WDIR";   // create default work dir
  else
    remote_cmd +=
      "FJDIR=$PDIR/job-" + job.forerunner + "\n"   // refer to a forerunner job
      "if [ -f $FJDIR/wd ]; then WDIR=`cat $FJDIR/wd`\n"  // check if there is further reference
      "else WDIR=$FJDIR/work; fi\n"
      "if [ $WDIR == $JDIR/work ]; then echo 'Cyclic job reference!' 1>&2\n"  // check cyclic refernces
      "elif [ ! -d $WDIR ]; then echo 'Forerunner job is not found!' 1>&2\n"  // check if the work dir exists
      "else echo $WDIR; echo $WDIR >$JDIR/wd; fi";

  if( shell->Execute(remote_cmd, out, err) || !err.IsEmpty() || out.GetCount() != 2 ) {
    LOGJOBWARN(fmt("Unable to create job service or work directory: %s",
      (pCSTR)shell->ArrayToString(err).c_str() ));
    return JOB_FAILED;
  }
  remote_cmd.Clear();
  
  // Extract job id
  if(out.size()>0)
    jobdir  = out[0];
  id      = jobdir.AfterLast('/').Mid(4);
  if(out.size()>1)
    workdir = out[1];
  if(jobdir.IsEmpty() || id.IsEmpty() || workdir.IsEmpty())
    LOGJOBERR("Unvalid job id, job dir or work dir is returned from the remote system!");

  // Ensure that the job id is unique 
  //FOR_EACH_LOOP(JobList, jobs, j)
  //  if(id == (*j)->id) LOGJOBERR("Job ID duplication is found");

  //if( job.haveLocals() && shell->MkDir(jobdir) )
  //  LOGJOBERR("Unable to create job temporary directory");
  //else remote_cmd += "mkdir -p " + jobdir + '\n';

  // Update job object and save job info
  SetJobData(job, id, jobdir, workdir);
  jobs.push_back(&job);

  // Stage in input files
  if( StageAllInput(job, rem_copy) ) {
    LOGJOBWARN("Unable to upload input files");
    return JOB_FAILED;
  }
  if( !rem_copy.IsEmpty() ) remote_cmd += "if " + rem_copy + "; then\n";

  // Go to the job directory
  remote_cmd += "cd " + jobdir + "\n";

  // Save Job info
  if(save_job_info)
    remote_cmd += "echo '"
      "Client user name:   " + gmdGetUserName() + "\n"
      "Client host name:   " + gmdGetHostName() + "\n"
      "Client OS:          " + gmdGetOsDescription() + "\n"
      "Shell class name:   " + shell->GetClassName() + "\n"
      "Manager class name: " + GetClassName() + "\n"
      "Job forerunner:     " + job.forerunner + "\n"
      + gmdString::Format(
        "Job run info:       mpi=%d, nproc=%d, ppn=%d, walltime=%d",
        job.mpi, job.nproc, job.ppn, job.walltime) + "\n"
      "Job extra info:     " + job.info
      + "' >" + info_file + "\n";

  gmdString user_cmd;
  remote_cmd += get_user_cmd(job, user_cmd, job.mpi || make_cmd_script);

  // Prepare the main start script
  remote_cmd += gmdString() + 
    "echo '" + get_script_body(job, user_cmd) + "' >" + script_file + "\n"
    "chmod u+x " + script_file + "\n";

  // Save submission command for a delayed run
  job.subm_cmd = pre_subm_cmd.IsEmpty() ?
    get_subm_cmd(job) :
    //pre_subm_cmd + ">/dev/null; " + get_subm_cmd(job);
    "if { " + pre_subm_cmd + "; } >/dev/null; then " + get_subm_cmd(job) + "; "
    "else echo PRESUBM_CMD_ERROR; fi";

  remote_cmd +=
    "echo '" + job.subm_cmd + "\nJOB_PREPARED' >" + status_file + "\n";

  if(log_level & LOG_TIMING)
    remote_cmd += gmdString("echo PREPARED `date +'%s %N'` > ") + log_file + "\n";

  if(!prepare_only) // Execute submission command
    remote_cmd += ConvertSpecials(job.subm_cmd);

  if( !rem_copy.IsEmpty() ) remote_cmd +=
    "\nelse echo '\nJOB_FAILED' >" + jobdir + "/" + status_file + "\nfi";

  // DEBUG!!!
  // puts((pCSTR)remote_cmd.c_str());

  if(prepare_only)  // Copy files without submitting the job
    return ( !shell->Execute(remote_cmd, out, err) && err.IsEmpty() ) ?
      JOB_PREPARED : JOB_FAILED;

  return RemoteSubmit(job, remote_cmd);
}


// Run submission command for the remote batch system
int gmJobManager::RemoteSubmit(gmJob& job, const gmdString& cmd){
  int res;
  if(job.type & gmJob::DUMMY) {
    res = shell->Execute(
      "cd " + job.dir + "\n"
      "echo 'DUMMY_JOB\nJOB_COMPLETED' >" + status_file
    );
    res = (res == 0) ? JOB_COMPLETED : JOB_FAILED;
  }
  else
    res = exec_subm_cmd(job, cmd);

  return res;
}


gmdString gmJobManager::get_user_cmd(gmJob& job, gmdString& user_cmd, bool make_script) {
  gmdString cmd = job.command;
  cmd.Replace("'", "'\"'\"'");  // screen single quotes

  if(!make_script) {
    user_cmd = cmd;
    return "";
  }

  // Save user commands to user_cmd_file
  user_cmd = shell->get_full_path(job._dir()) + "/" + user_cmd_file + "\n";
  return gmdString()
    + "echo '"
      "#!/bin/sh\n"
       + cmd + "' >" + user_cmd_file + "\n"
    "chmod u+x " + user_cmd_file + "\n";
}


gmdString gmJobManager::ConvertSpecials(gmdString& src) {
  gmdString res(src);
  res.Replace("\\n","\n");
  res.Replace("\\q","'");
  return res;
}


int gmJobManager::Prepare(gmJob& job, pCSTR user_id, bool mktemp){
  return Submit(job, user_id, mktemp, true);
};


int gmJobManager::Resubmit(gmJob& job){
    LOGJOBMSG( fmt("Resubmitting job %s", (const char*) job.id) );
  return RemoteSubmit(job,
    "cd " + job.dir + "; " + ConvertSpecials(job.subm_cmd)
  );
}


int gmJobManager::Status(gmJob& job){
  gmdArrayString out, err;
  int res;

  res = shell->Execute(
      "cd " + job.dir + "\n"
      "JSTATE=`tail -n 1 " + status_file + "`\n"
      "if [ _$JSTATE = _JOB_SUBMITTED -o _$JSTATE = _JOB_COMPLETED ]; then\n"
      "  "+ get_status_cmd() + "\n"
      "  JSTATE=`tail -n 1 " + status_file + "`\n"  // May be: execution process take pleca at first quer of status file, but been completed at one line above...
      "fi\n"
      "echo $JSTATE",
    out, err);

  if(res || !err.IsEmpty() || out.IsEmpty() || out.Last() == "JOB_FAILED")
    res = JOB_FAILED;
  else {
    res = interpret_status(out[0]);
    if(res == JOB_INIT || res == JOB_COMPLETED)  // no such job in queue
      res = ( out.Last() == "JOB_COMPLETED" ) ? JOB_COMPLETED
        //: ( out.Last() == "JOB_SUBMITTED" ) ? JOB_SUBMITTED
          : JOB_FAILED;
  }

  return res;
}


int gmJobManager::Stop(gmJob& job){
  gmdArrayString out, err;

  shell->Execute( gmdString::Format(
    "cd %s\n"
    "SFILE=%s\n"
    "STAT=`tail -n 1 $SFILE`\n"
    "if [ _$STAT = _JOB_SUBMITTED ]\n"
    "then %s\nSTAT=JOB_FAILED; echo $STAT >>$SFILE; fi\n"
    "echo $STAT",
    (pCSTR)job.dir.c_str(), status_file, (pCSTR)get_stop_cmd(job).c_str() ),
  out, err);

  return (!out.IsEmpty() && out[0] == "JOB_COMPLETED") ? JOB_COMPLETED : JOB_FAILED;
}


int gmJobManager::Attach(gmJob& job, pCSTR id){
  gmdArrayString out, err;
  gmdString jobdir = rem_tmp_dir + "/job-" + id;

  // Run the script the looks for the job with the given id
  int res = shell->Execute(
      "JDIR=" + jobdir + "\n"
      "SFILE=" + status_file + "\n"          // status_file
      "if [ -f $JDIR/$SFILE ]; then\n"  // if job status file exists
      "cd $JDIR\n"
      "WDFILE=" + workdir_file + "\n"         // workdir_file
      "if [ -f $WDFILE ]; then cat $WDFILE; else echo $JDIR/work; fi\n"  // print work dir
      "head -n 1 $SFILE\n"  // print submission command or submission job id
      "JSTATE=`tail -n 1 $SFILE`\n"
      "if [ _$JSTATE = _JOB_SUBMITTED -o _$JSTATE = _JOB_COMPLETED ]; then\n"
      + get_status_cmd() + "\n"
      "fi\n"
      "echo $JSTATE\n"
      "else echo No job; fi",
    out, err);

  // Interpret script output
  if(!err.IsEmpty() || out.IsEmpty()) {
    LOGJOBWARN("gmJobManager: Attach() failed");
    return JOB_FAILED;
  }

  if( out[0] == "No job" ) return JOB_INIT;   // Valid job is not found
  
  int nlines = out.GetCount();
  if(nlines < 3) return JOB_FAILED;

  gmdString wdir   = out[0];

  // Get job id from work dir and initialize job.forerunner
  gmdString fjob = wdir.Left(wdir.Length() - 5).AfterLast('/').Mid(4);
  job.forerunner = (fjob != id) ? fjob : "";

  pCSTR subm_cmd = "";
  pCSTR subm_id = out[1];

  gmdString status = out.Last();
  if( status == "JOB_PREPARED" ) {  // Job is prepared but not submitted
    res = JOB_PREPARED;
    subm_cmd = out[1];
    subm_id = "";
  }
  else if( status == "JOB_FAILED" )
    res = JOB_FAILED;
  else {
    res = interpret_status(out[2]);
    if(res == JOB_INIT || res == JOB_COMPLETED)  // no such job in queue
      res = ( status == "JOB_COMPLETED" ) ? JOB_COMPLETED : JOB_FAILED;
  }

  // Save job information
  SetJobData(job, id, jobdir, wdir, subm_cmd, subm_id);
  jobs.push_back(&job);

  return res;
}


int gmJobManager::Wait(gmJob& job) {
  int state;
  gmdStopWatch timer;

  while( (state = job.GetState()) < JOB_COMPLETED &&
         (wait_timeout == 0 || timer.Time() < wait_timeout) ) {
    gmdMilliSleep(wait_delay);  // wait 3 seconds before the next try
  }

  wait_time = timer.Time();
  return state;
}


int gmJobManager::FetchResult(gmJob& job, bool fWait){
  gmdString cmd;
  gmdArrayString out, err;
  
  if(fWait) job.Wait(); else job.GetState();

  int res1 = StageAllOutput(job, cmd);
  int res2 = cmd.IsEmpty() ? 0 : shell->Execute(cmd, out, err) || !err.IsEmpty();

  return res1 || res2;
}


void gmJobManager::Clear(gmJob& job){
  if( !job.dir.IsEmpty() ) shell->Remove( job.dir );
  Detach(job);
}


int gmJobManager::GetTiming(gmJob& job, gmJobTiming& jobtim) {
  jobtim.submitting = jobtim.queued = jobtim.running = -1;

  if( !(log_level & LOG_TIMING) ) return -1;

  gmdString str;
  if( shell->StageOutText(str, shell->get_full_path(job._dir()) + "/" + log_file) )
    return -2;

  int64 prepared = -1, submitted = -1, started = -1;

  gmdStringTokenizer tkz(str, " \n");
  while(tkz.HasMoreTokens()) {
    long sec, nsec;
    gmdString action = tkz.GetNextToken();
    if(!tkz.GetNextToken().ToLong(&sec) || !tkz.GetNextToken().ToLong(&nsec))
      return -3;
    int64 time_ms = nsec/1000000 + (int64)sec*1000;
    if(action == "PREPARED") prepared = time_ms;
    else if(action == "SUBMITTED" && prepared > 0) {
      submitted = time_ms;
      jobtim.submitting = (long)(time_ms - prepared);
    }
    else if(action == "STARTED" && submitted > 0) {
      started = time_ms;
      jobtim.queued = (long)(time_ms - submitted);
    }
    else if(action == "ENDED" && started > 0)
      jobtim.running = (long)(time_ms - started);
    else return -4;
  }

  return 0;
}


gmdString gmJobManager::GetStdOut(gmJob& job){
  return job.GetState() >= JOB_COMPLETED ? stdout_file : "";
}


gmdString gmJobManager::GetStdErr(gmJob& job){
  return job.GetState() >= JOB_COMPLETED ? stderr_file : "";
}


/// Stage all local input files for the given job
/// and create a copying command for the remote system (remcmd)
/// Returns 0 on success.
/// Assumes that the current dir is $HOME
int gmJobManager::StageAllInput(gmJob& job, gmdString& remcmd){
  int res = 0;
  gmdString createdirs, rpath, lpath;
  typedef map<gmdString, gmdArrayString> CopyFileList;
  typedef map<unsigned, CopyFileList> CopyFileListOpt;
  CopyFileListOpt copylist;

  remcmd.Clear();
  FOR_EACH_LOOP(gmFileList, job.infiles, file) {
    // Append the path to working dir if needed
    rpath = file->rpath;
    gmdString& lpath = file->lpath;
    unsigned flags = file->flags;
    if( !rpath.StartsWith("/") ) rpath = job.wdir + '/' + rpath;

    gmdString newdir;
    if( flags & gmJob::CREATEPATH ) {
      bool rpath_is_dir = rpath.EndsWith("/");
      if(rpath_is_dir) rpath.Truncate(rpath.Length() - 1);

      if( flags & gmJob::RECURSIVE || rpath_is_dir || has_wildcards(lpath) )
        newdir = rpath;
      else {
        gmdFileName filename(rpath);
        newdir = filename.GetPath();
      }
    }

    // Handle local and remote input files separately
    if(flags & gmJob::REMOTE ) {
      if( !remcmd.IsEmpty() ) remcmd += '\n';
      if( !newdir.IsEmpty() )
        remcmd += "mkdir -p " + shell->rem_path_subst(newdir) + '\n';
      if( flags & gmJob::MOVE )
        remcmd += "mv ";
      else
        remcmd += (flags & gmJob::RECURSIVE) ? "cp -rp " : "cp -p ";
      remcmd += shell->rem_path_subst(lpath) + ' ' + rpath;
    }
    else {
      if( !newdir.IsEmpty() ) createdirs += ' ' + newdir;
      // Group files by output directory
      unsigned cpflags = flags & ~(gmJob::CREATEPATH | gmJob::REMOTE);
      copylist[cpflags][rpath].Add(lpath);
    }
  }
  
  if( !createdirs.IsEmpty() )  // Create directories for local files
    res = shell->MkDir(createdirs);

  // Loop over all copying options (lfile->first)
  FOR_EACH_LOOP(CopyFileListOpt, copylist, optlist){
    unsigned flags = optlist->first;
    // Loop over local files (lfile->second) for the same target dir (lfile->first)
    FOR_EACH_LOOP(CopyFileList, optlist->second, lfile) {
      if(res) break;
      res = shell->StageIn(lfile->second, lfile->first, flags);
    }
  }

  return res;
}


/// Stage all local output files for the given job
/// and create a copying command for the remote system (remcmd)
/// Returns 0 on success.
/// Assumes that the corrent dir is $HOME
int gmJobManager::StageAllOutput(gmJob& job, gmdString& remcmd){
  int res = 0;

  remcmd.Clear();
  FOR_EACH_LOOP(gmFileList, job.outfiles, file) {
    int res1 = StageOutputFile(job, *file, remcmd);
    res = res || res1;
  }

  return res;
}


/// Stage a local output file for the given job
/// ore create a copying command for the remote system (remcmd)
/// Returns 0 on success
/// Assumes that the current dir is $HOME
int gmJobManager::StageOutputFile(gmJob& job, gmFileTrans& file, gmdString& remcmd){
  int res = 0;
  gmdString rpath(file.rpath);
  gmdString& lpath = file.lpath;
  unsigned flags = file.flags;

  if( rpath.IsEmpty() ) return 1;

  if(rpath == "STDOUT")
    rpath = job.dir + '/' + GetStdOut(job), flags |= gmJob::TEXT;
  else if(rpath == "STDERR")
    rpath = job.dir + '/' + GetStdErr(job), flags |= gmJob::TEXT;
  else if( !rpath.StartsWith("/") )
    rpath = job.wdir + '/' + rpath;

  gmdString newdir;
  if( flags & gmJob::CREATEPATH ) {
    bool lpath_is_dir = lpath.EndsWith("/") || lpath.EndsWith("\\");
    if(lpath_is_dir) lpath.Truncate(lpath.Length() - 1);

    if( flags & gmJob::RECURSIVE || lpath_is_dir || has_wildcards(rpath) )
      newdir = lpath;
    else {
      gmdFileName filename(lpath);
      newdir = filename.GetPath();
    }
  }

  if(flags & gmJob::REMOTE) {
    gmdString lpath(shell->rem_path_subst(lpath));
    if( !remcmd.IsEmpty() ) remcmd += '\n';
    if( !newdir.IsEmpty() )
      remcmd += "mkdir -p " + shell->rem_path_subst(newdir) + '\n';
    if( flags & gmJob::MOVE )
      remcmd += "mv ";
    else
      remcmd += (flags & gmJob::RECURSIVE) ? "cp -rp " : "cp -p ";
    remcmd += rpath + ' ' + lpath;
  }
  else {
    if( !newdir.IsEmpty() && !gmdDirExists(newdir) ) {
      res = shell->MkDirLocal(newdir);
      if(res) {
        LOGJOBWARN(fmt("Cannot create output directory %s", (pCSTR)lpath.c_str()));
        return res;
      }
    }

    res = shell->StageOut(lpath, rpath, flags);
  }

  return res;
}


// Find and start waiting jobs except the given in argument
// Doesn't work if called from ClearAll or DetachAll
// Returns the number of resubmitted jobs
void gmJobManager::drain_prepared(gmJob* job_exclude){
  if(fBatchOperation) return;
  
  FOR_EACH_LOOP(JobList, jobs, j) {
    if(jobs_wait == 0 || (jobs_limit && jobs_subm >= jobs_limit)) break;
    if(*j != job_exclude && (*j)->_state() == JOB_PREPARED) {
      int res = Resubmit(**j);
      if(res == JOB_PREPARED) break;  // don't try to submit more jobs if the queue is full
      else SetJobState(**j, res);  // invokes state_change
    }
  }
}


void gmJobManager::SetJobData(gmJob& job, pCSTR id, pCSTR dir,
    pCSTR wdir, pCSTR subm_cmd, pCSTR subm_id){
  if(id) job.id = id;
  if(dir) job.dir = dir;
  if(wdir) job.wdir = wdir;
  if(subm_cmd) job.subm_cmd = subm_cmd;
  if(subm_id) job.subm_id = subm_id;
}


void gmJobManager::SetJobState(gmJob& job, int state) {
  if(state >= 0) {
    job.state_change(job.state, state);
    job.state = state;
  }
}


gmJob* gmJobManager::CreateManagedJob(pCSTR id){
  gmJob* pjob = new gmJob();
  pjob->managed = true;
  pjob->mng = this;
  if(id && pjob->Attach(*this, id) == JOB_INIT) { delete pjob; pjob = NULL; }
  return pjob;
}


// ============================================================================
// Batch functions for a job list
// ============================================================================

int gmJobManager::WaitAll(pCSTR id_prefix){
  JobList pjobs, rjobs;
  gmdString list, status;
  gmdArrayString out, err;
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
    list.Clear();
    rjobs.clear();
    for(JobList::iterator it=pjobs.begin(); it!=pjobs.end();){
      gmJob* j = *(it++);
      state = j->_state();
      if(state == JOB_FAILED) res = JOB_FAILED;
      if(state >= JOB_COMPLETED) pjobs.remove(j);  // remove all completed jobs
      else if(state >=JOB_SUBMITTED){
        rjobs.push_back(j);  // create list of 
        list += ' ' + j->GetID();
      }
    }

    if( pjobs.empty() ) break;  // normal loop exit

    // DEBUG!!!
    //printf("Jobs submitted: %d, Jobs prepared: %d\nWaiting for jobs:", jobs_subm, jobs_wait);
    //  FOR_EACH_LOOP(JobList, jobs, j) printf(" %s (state=%d)", (*j)->GetID().c_str(), (*j)->_state());
    //puts("");

    if(not_first_iter) gmdMilliSleep(wait_delay);  // wait 3 seconds before the next try
    else not_first_iter = true;

    // Run the script to get status of the running jobs
    if( !list.IsEmpty() ) {  // monitor SUBMITTED jobs
      shell->Execute(
          "cd " + rem_tmp_dir + "\n"
          "SFILE=" + status_file + "\n"
          "for JOB in" + list + " ; do\n"
          "if [ -f job-$JOB/$SFILE ]; then\n"
          "cd job-$JOB\n"
          "JSTATE=`tail -n 1 $SFILE`\n"
          "if [ _$JSTATE = _JOB_SUBMITTED -o _$JSTATE = _JOB_COMPLETED ]; then\n"
          + get_status_cmd() + "\n"
          "fi\n"
          "echo $JSTATE\n"
          "echo ---job---\n"
          "cd ..\n"
          "fi; done",
        out, err);

      //gmdString str = shell->ArrayToString(out); //DEBUG

      if( out.IsEmpty() ) {
        LOGJOBWARN("WaitAll is unable to inquire job states");
        return JOB_FAILED;
      }

      int i = 0, k = 0, out_lines = out.GetCount();
      FOR_EACH_LOOP(JobList, rjobs, j) {
        while(k < out_lines && out[k] != "---job---") k++;
        if(k == 0 || k == out_lines)
          LOGJOBERR("WaitAll error: invalid script output");
        if( out[k-1] == "JOB_FAILED" ) state = JOB_FAILED;
        else {
          state = interpret_status( out[i] );
          if(state == JOB_INIT || state == JOB_COMPLETED)  // no such job in queue
            state = ( out[k-1] == "JOB_COMPLETED" ) ? JOB_COMPLETED : JOB_FAILED;
          }
        // Change state of the completed or failed jobs (invokes state_change and runs waiting jobs)
        SetJobState(**j, state);
        i = ++k;
      }
    }
    else drain_prepared();  // try to resubmit PREPARED jobs
  }

  wait_time = timer.Time();
  return res;
}


int gmJobManager::FetchAll(bool fWait, pCSTR id_prefix) {
  if(fWait) WaitAll(id_prefix);

  int state = JOB_HAVERESULT;
  FOR_EACH_LOOP(JobList, jobs, j)
    if( (!id_prefix || (*j)->GetID().StartsWith(id_prefix))
        && (*j)->FetchResult(fWait) == JOB_FAILED ) state = JOB_FAILED;

  return state;
}


void gmJobManager::StopAll(pCSTR id_prefix){
  gmdString remote_cmd;

  fBatchOperation = true;

  // Create list of all running jobs
  FOR_EACH_LOOP(JobList, jobs, j)
    if( !id_prefix || (*j)->GetID().StartsWith(id_prefix) ) {
      if((*j)->state >= JOB_SUBMITTED && (*j)->state < JOB_COMPLETED) {
        remote_cmd += gmdString::Format(
          "SFILE=%s/%s\n"
          "if [ _`tail -n 1 $SFILE` = _JOB_SUBMITTED ]; then %s\n"
          "echo JOB_FAILED >>$SFILE; fi\n",
          (pCSTR)(*j)->dir.c_str(), status_file, (pCSTR)get_stop_cmd(**j).c_str()
        );
        SetJobState(**j, JOB_FAILED);
      }
    }

  if( !remote_cmd.IsEmpty() ) shell->Execute(remote_cmd);

  fBatchOperation = false;
  drain_prepared();
}


void gmJobManager::ClearAll(pCSTR id_prefix) {
  StopAll(id_prefix);

  fBatchOperation = true;
  for(JobList::iterator it=jobs.begin(); it!=jobs.end();){
    gmJob* j = *(it++);
    if( !id_prefix || j->GetID().StartsWith(id_prefix) ) j->Clear();
  }
  fBatchOperation = false;
  drain_prepared();
}


void gmJobManager::DetachAll(pCSTR id_prefix) {
  fBatchOperation = true;
  for(JobList::iterator it=jobs.begin(); it!=jobs.end();){
    gmJob* j = *(it++);
    if( !id_prefix || j->GetID().StartsWith(id_prefix) ) j->Detach();
  }
  fBatchOperation = false;
  drain_prepared();
}


/* Temporary not supported
JobList gmJobManager::RestoreAll(pCSTR id_prefix){
  JobList newjobs;
  gmdString out, err, id, subm_data, status;
  if(!id_prefix) id_prefix = "";

  // Run the script the looks for all jobs with the given id_prefix
  shell->Execute( gmdString::Format(
      "cd %s\n"
      "for JOB in job-%s*; do\n"
      "STATF=$JOB/%s\n"
      "if [ -f $STATF ]; then\n"
      "echo Job found: $JOB\n"
      "cat $STATF\n"
      "if [ _`tail -n 1 $STATF` = _JOB_SUBMITTED ]; then\n"
      "%sqstat -f `head -n 1 $STATF` | grep 'job_state ='\n"
      "fi;fi;done",
      (pCSTR)rem_tmp_dir.c_str(), id_prefix, status_file, (pCSTR)pbs_dir.c_str()),
    out, err);

  // Loop over all found jobs
  int i, res;
  while( (i = out.Find("Job found: job-")) != gmdNOT_FOUND ){
    // Interpret script output
    out = out.Mid(i+18);
    id = out.BeforeFirst('\n');
    if(JobByID(id)){
      LOGJOBWARN(fmt("RestoreAll: job %s ignored as it already exists in Job Manager", (pCSTR)id.c_str()));
      continue;
    }

    out = out.AfterFirst('\n');
    if( out.StartsWith("Job found:") ) continue;  // Empty status file
    subm_data = out.BeforeFirst('\n');  // Can be PBS ID or qsub command

    out = out.AfterFirst('\n');
    status = out.BeforeFirst('\n');
    out = out.AfterFirst('\n');

    char *subm_cmd = NULL, *subm_id = &subm_data[0];

    if( status == "JOB_SUBMITTED" )  // Get the status of submited job
      res = interpret_qstat( out.BeforeFirst('\n') );
    else if( status == "JOB_PREPARED" ) {  // Job is prepared but not submitted
      res = JOB_PREPARED;
      subm_cmd = &subm_data[0]; subm_id = NULL;
    }
    else if( status == "JOB_FAILED" )
      res = JOB_FAILED;
    else continue;  // wrong job status file

     // Create a new job object and add it to the job lists
    gmJob* job = CreateJob();
    SetJobData(*job, id, rem_tmp_dir + "/job-" + id, subm_cmd, subm_id, res);
    newjobs.push_back(job);
    jobs.push_back(job);
  }

  drain_prepared();

  return newjobs;
}
*/

JobList gmJobManager::GetJobList(pCSTR id_prefix) const {
  if( !id_prefix ) return jobs;
  
  JobList seljobs;
  FOR_EACH_LOOP_C(JobList, jobs, j)
    if( (*j)->GetID().StartsWith(id_prefix) ) seljobs.push_back(*j);
  return seljobs;
}

gmJob* gmJobManager::JobByID(pCSTR id) const {
  FOR_EACH_LOOP_C(JobList, jobs, j)
    if( (*j)->GetID() == id ) return *j;
  return NULL;
}


/****************************************************************************
 * Implementation of gmJob class
****************************************************************************/

const char* gmJob::state_names[] = {
  "INIT", "PREPARED", "SUBMITTED", "QUEUED", "RUNNING",
  "SUSPENDED", "EXITING", "COMPLETED", "HAVERESULT", "FAILED"};

int gmJob::Submit(gmJobManager& mngr, pCSTR user_id, bool mktemp){
  if(state != JOB_INIT)
    LOGJOBERR("gmJob object is not in JOB_INIT state on Submit");
  if( !mktemp && (!user_id || !user_id[0]) )
    LOGJOBERR("Job id should be specified when mktemp = false on Submit");
  if( !mktemp && mngr[user_id] )
    LOGJOBERR("Job with the same id has been already submitted");

  if(managed && mng != &mngr)
    LOGJOBERR("Managed job should be submitted to the same job manager" );
  mng = &mngr;

  state = !mng->jobs_limit || mng->jobs_subm < mng->jobs_limit ?
    mng->Submit(*this, user_id, mktemp) : 
    mng->Prepare(*this, user_id, mktemp);

  state_change(JOB_INIT, state);
  return state;
}


int gmJob::Attach(gmJobManager& mngr, pCSTR id_) {
  if( !(id_ && id_[0]) ) LOGJOBERR("Job id is not specified!");
  if(state != JOB_INIT)
    LOGJOBERR("gmJob object is not in JOB_INIT state on Attach");
  if( mngr[id_] ){
    LOGJOBWARN(fmt("Attach: job with the id='%s' already exists in Job Manager", id_));
    return JOB_INIT;
  }

  state = mngr.Attach(*this, id_);
  if( state != JOB_INIT ){
    mng = &mngr;
    if(state == JOB_PREPARED && (!mng->jobs_limit || mng->jobs_subm < mng->jobs_limit))
      state = mng->Resubmit(*this);
    state_change(JOB_INIT, state);
  }

  return state;
}


int gmJob::StageOut(pCSTR dst, pCSTR workpath, unsigned flags){
  gmdString cmd;
  gmdArrayString out, err;
  if( !(dst && dst[0] && workpath && workpath[0]) ) LOGJOBERR("Empty file names!" );
  AccertState(JOB_PREPARED);

  gmFileTrans file(dst, workpath, flags);
  int res1 = mng->StageOutputFile(*this, file, cmd);
  int res2 = 0;
  if(!cmd.IsEmpty()) res2 = mng->shell->Execute(cmd, out, err) || !err.IsEmpty();
  return res1 || res2;
}


void gmJob::state_change(int oldstate, int newstate){
  if(oldstate == JOB_INIT) { // Job is started or attached
    if(newstate == JOB_PREPARED) mng->jobs_wait++;
    else if(newstate >= JOB_SUBMITTED && newstate < JOB_COMPLETED) mng->jobs_subm++;
  }
  else if(oldstate == JOB_PREPARED && newstate != JOB_PREPARED){ // Job is submitted or cleared
    mng->jobs_wait--;
    if(newstate >= JOB_SUBMITTED && newstate < JOB_COMPLETED) mng->jobs_subm++;
  }
  else if(oldstate >= JOB_SUBMITTED && oldstate < JOB_COMPLETED &&
          (newstate == JOB_INIT || newstate >=JOB_COMPLETED)){ // Job is completed or failed/cleared
    mng->jobs_subm--;

    mng->drain_prepared(this); // job_exclude = this avoids endless recursion
  }
}

/*
bool gmJob::haveLocals() {
  bool res = false;
  for(unsigned i=0; i<infiles.GetCount(); i++)
    if( !infiles[i].StartsWith("remote:") ) {
      res = true; break;
    }
  return res;
}
*/
