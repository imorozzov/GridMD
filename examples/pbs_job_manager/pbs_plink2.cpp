/*****************************************************************************
  GridMD class library
  Example for PBS Job Manager and gmShellPlink communication protocol:
  working with multiple jobs

  Copyright (c) 2009 Igor Morozov, Ilya Valuev

  For the terms and conditions for copying, distribution and modification
  see file license.txt
******************************************************************************/

#include <stdio.h>
#include <gmd/app.h>
#include <gmd/utils.h>
#include "jobmngr/pbsmngr.h"
#include "jobmngr/plinkshell.h"

int main(int argc, char *argv[])
{
  // Initialization of wxWindows library
  if( !gmdInitialize() ) {
    puts("Failed to initialize the GridMD library.");
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
    mngr.SetParam("job_name_prefix", "GMD"); // "GMD" - префикс для имени задач в PBS

    // Initialization of the job which will be used as a template
    gmJob job_template;
    
    // Submission of multiple jobs
    const int njobs = 2;  // number of jobs to submit
    printf("Submitting %d jobs ...\n", njobs);
    for(int i=0; i<njobs; i++){
      // Initialization of the current job
      gmJob job = job_template;
      job.command = gmdString::Format(
        "echo Test Job %d runs on `hostname` >result%d.txt; sleep 10", i, i);

      // Submission of the job with the id like "batch-<JOB NUMBER>"
      res = job.Submit(mngr, gmdString::Format("batch-%d", i), false);
      gmdString id = job.GetID();
      printf("Job %s is submitted, state = %s\n", id.c_str(), gmJob::StateName(res));

      // The job will be detached automatically on the local gmJob object deletion
    }

    // Restoting jobs using their ids 
    // and fetching temporary results without waiting for jobs completion
    for(int i=0; i<njobs; i++) {
      gmdString jobname = gmdString::Format("batch-%d", i);
      printf("Restoring job %s... ", jobname);
      gmJob* pjob = mngr.Restore(jobname);
      if(pjob) {
        printf("SUCCESS, job state = %s\n", gmJob::StateName(pjob->LastState()));
        pjob->AddOutFile( "../example2-out",
                          gmdString::Format("result%d.txt", i), gmJob::TEXT );
      }
      else
        puts("FAILED!");
    }

    // Fetching final results
    puts("Waiting for completion and fetching results...");
    res = mngr.FetchAll(true);  // change to 'false' to wait for the job completion
    printf("Exit code of FetachAll: %s\n", gmJob::StateName(res));

    // Stopping all jobs and deleting their remote directories
    // The user has to delete all joblist elements created by RestoreAll
    mngr.ClearAll();
    gmdSleep(1);
    puts("All jobs are cleared.");
  }
  catch(gmJobException &e){ (void)e; }

  gmdUninitialize();

  return 0;
}
