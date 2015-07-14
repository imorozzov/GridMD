/*****************************************************************************
  GridMD class library
  Example for PBS Job Manager and gmShellPlink communication protocol:
  working with multiple jobs

  Copyright (c) 2009 Igor Morozov, Ilya Valuev

  For the terms and conditions for copying, distribution and modification
  see file license.txt
******************************************************************************/

#include <stdio.h>
#include <wx/app.h>
#include <wx/utils.h>
#include "jobmngr/pbsmngr.h"
#include "jobmngr/plinkshell.h"

int main(int argc, char *argv[])
{
  // Initialization of wxWindows library
  if( !wxInitialize() ) {
    puts("Failed to initialize the wxWidgets library.");
    return -1;
  }

  // message_logger is an auxiliary object defined in file logexc.h
  // it is used for the error handling
  message_logger::global().set_throw(true);  // run-time errors will generate exceptions

  try{
    int res;

    // Error handling class message_logger contains definitions
    // of errors, warnings and messages. Errors (vblOERR level) occur in
    // the case of incorrect input parameters of library functions,
    // incorrect function call sequence, impossibility to access the
    // remote system. Warnings (vblWARN level) occur due to the failure of
    // file copying, remote command execution, job duplication.
    //
    // The following command enables output of all messages (vblALL)
    // and sets program interruption on errors and warnings (vblALLBAD)
    message_logger::global().set_levels(vblALL,vblALLBAD);

    // Definition of plink authorization parameters (MUST BE CHANGED!)
    gmShellPlink shell("USER", "HOSTNAME", "-load \"PUTTY_SESSION_NAME\"");
    shell.SetParam("plink_att_num", "2");
    shell.SetParam("plink_retry_delay", "3000");

    // Creation of local output directory
    shell.MkDirLocal("../example2-out");

    // The following command enables output of all messages (vblALL)
    // and sets program interruption only on errors (vblERR)
    message_logger::global().set_levels(vblALL,vblERR);

    // Initialization of the job manager
    gmPBSManager mngr(shell);  // use the second argument to select the PBS queue
    mngr.SetParam("/usr/local/bin", "GMD"); // "GMD" - префикс для имени задач в PBS

    // Initialization of the job which will be used as a template
    gmJob job_template;
    
    // Submission of multiple jobs
    const int njobs = 2;  // number of jobs to submit
    printf("Submitting %d jobs ...\n", njobs);
    for(int i=0; i<njobs; i++){
      // Initialization of the current job
      gmJob job = job_template;
      job.command = wxString::Format(
        "echo Test Job %d runs on `hostname` >result%d.txt; sleep 10", i, i);

      // Submission of the job with the id like "batch-<JOB NUMBER>"
      res = job.Submit(mngr, wxString::Format("batch-%d", i), false);
      wxString id = job.GetID();
      printf("Job %s is submitted, state = %s\n", id.c_str(), gmJob::StateName(res));

      // The job will be detached automatically on the local gmJob object deletion
    }

    // Restoting of all jobs with the given user id
    JobList joblist = mngr.RestoreAll("batch");

    // Fetching the results without waiting for the job completion
    int i = 0;
    for each(gmJob* pjob in joblist) {
    // If your compiler does not support 'for each' operator
    // replace this line by the following two lines:
    //   FOR_EACH_LOOP(JobList, joblist, ppjob) {
    //   gmJob *pjob = *ppjob;   
      printf("Restored job %s, state = %s\n",
             pjob->GetID().c_str(), gmJob::StateName(pjob->LastState()));
      pjob->AddOutFile( "../example2-out",
                        wxString::Format("result%d.txt", i++), gmJob::TEXT );
    }
    puts("Waiting for completion and fetching results...");
    res = mngr.FetchAll(true);  // change to 'false' to wait for the job completion
    printf("Exit code of FetachAll: %s\n", gmJob::StateName(res));

    // Stopping all jobs and deleting their remote directories
    // The user has to delete all joblist elements created by RestoreAll
    mngr.ClearAll();
    wxSleep(1);
    puts("All jobs are cleared.");
  }
  catch(gmJobException &e){ (void)e; }

  wxUninitialize();

  return 0;
}
