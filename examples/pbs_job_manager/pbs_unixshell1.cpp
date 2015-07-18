/*****************************************************************************
  GridMD class library
  Example for PBS Job Manager and gmShellUnix communication protocol

  Copyright (c) 2009 Igor Morozov, Ilya Valuev

  For the terms and conditions for copying, distribution and modification
  see file license.txt
******************************************************************************/

#include <stdio.h>
#include <gmd/app.h>
#include <gmd/utils.h>
#include "jobmngr/pbsmngr.h"
#include "jobmngr/unixshell.h"

int main(int argc, char *argv[])
{
  // Initialization of gmdWindows library
  if( !gmdInitialize() ) {
    puts("Failed to initialize the gmdWidgets library.");
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

    // Initialization of the shell
    gmShellUnix shell;

    // Creation of the persistent remote directory
    // (needed for the first run only)
    puts("Copying permanent files to the remote system ...");
    shell.MkDir("tmp/remote_dir");

    // Copying of the persistent files
    shell.StageIn("../input/permfile.txt", "tmp/remote_dir");
    // For text files the full remote file name should be specified ("tmp/remote_dir/prog.sh")!
    shell.StageIn("text:../input/prog.sh", "tmp/remote_dir/prog.sh");

    // Attribute for execution of the script prog.sh
    shell.Execute("chmod u+x tmp/remote_dir/prog.sh");

    // The following command enables output of all messages (vblALL)
    // and sets program interruption only on errors (vblERR)
    message_logger::global().set_levels(vblALL,vblERR);

    // Initialization of the job manager
    gmPBSManager mngr(shell);  // use the second argument to select the PBS queue
    mngr.SetParam("/usr/local/bin", "GMD"); // "GMD" is the prefix for the PBS jobs

    // Initialization of the job
    gmJob* job = mngr.CreateJob();
    job->command = "$HOME/tmp/remote_dir/prog.sh 1 2"; // execution command path and arguments
    // Input files and directories
    job->AddInFile("../input/infile1.txt", "", gmJob::TEXT);
    job->AddInFile("../input/infile2.txt", "", gmJob::TEXT);
    job->AddInFile("../input/indir", "");
    job->AddInFile("tmp/remote_dir/permfile.txt", "", gmJob::REMOTE); // этот файл будет взят из каталога на удаленной системе

    printf("State before submission: %s\n", gmJob::StateName(job->GetState()));

    // Job submission
    res = job->Submit("test");
    gmdString id = job->GetID();
    printf("State after submission: %s, id = %s\n", gmJob::StateName(res), id.c_str());

    gmdSleep(2);  // Usually after 2 seconds the jobs is started

    // Copying of an intermediate result (a file from the working directory)
    res = job->StageOut("../example1-out-inter", "outfile.txt", gmJob::TEXT | gmJob::CREATEPATH);
    printf("File uploaded with the exit code: %d\n", res);

    // Detaching from the job manager. After Detach the remote job is still
    // running while the job object is deleted.
    job->Detach();

    // Restoring the job object (attaching to the working remote job)
    // using the saved id
    job = mngr.Restore(id);
    res = job->LastState();  // LastState returns here the status obtained by Restore
                             // without extra remote system calls
    printf("State after Attach: %s\n", gmJob::StateName(res));

    if(res && res != JOB_FAILED) {
      // Output files and directories
      job->AddOutFile("../example1-out-final", "outfile.txt", gmJob::TEXT | gmJob::CREATEPATH);
      job->AddOutFile("../example1-out-final", "outdir");
      job->AddOutFile("tmp/remote_dir", "permfile.txt", gmJob::REMOTE); // этот файл будет скопирован в каталог на удаленной системе
      // Files to store stdout and stderr
      job->AddOutFile("../example1-out-final/out", "STDOUT");
      job->AddOutFile("../example1-out-final/err", "STDERR");

      // Waiting for the job completion and fetching the results
      //mngr.wait_timeout = 15000;  // uncommend this to set a timeout
      res = job->FetchResult();  // use the secong arg to prevent waiting
      printf("State after FetchResult: %s\n", gmJob::StateName(res));
    }

    // Removing the temporary directory on the remote source and the job object
    job->Clear();
  }
  catch(gmJobException &e){ (void)e; }

  gmdUninitialize();

  return 0;
}
