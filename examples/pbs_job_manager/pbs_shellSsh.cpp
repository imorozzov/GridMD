/*****************************************************************************
GridMD class library
Example for PBS Job Manager and gmShellLibssh communication protocol:
working with multiple jobs

Copyright (c) 2017 Igor Morozov, Ilya Valuev, Vdovina Evgenia

For the terms and conditions for copying, distribution and modification
see file license.txt
******************************************************************************/

#include <stdio.h>
#include <gmd/app.h>
#include <gmd/utils.h>
#include "jobmngr/bshmngr.h"
#include "jobmngr/pbsmngr.h"
#include "jobmngr/plinkshell.h"
#include "jobmngr/libsshell.h"
#include <list>
#include <algorithm>

int main(int argc, char *argv[])
{
	// Initialization of wxWindows library
	if (!gmdInitialize()) {
		puts("Failed to initialize the GridMD library.");
		return -1;
	}

	// message_logger is an auxiliary object defined in file logexc.h
	// it is used for the error handling
	message_logger::global().set_throw(true);  // run-time errors will generate exceptions

	try {
		int res;
		message_logger::global().set_levels(vblALL, vblALLBAD);
		/*gmShellPlink shell("vdovina", "nwo2.ihed.ras.ru", "-load vdovina");
		shell.SetParam("plink_path", "C:\\Users\\User\\Desktop\\putty\\plink.exe");
		shell.SetParam("pscp_path", "C:\\Users\\User\\Desktop\\putty\\pscp.exe");
		*/
		gmShellLibssh shell("vdovina", "nwo5.ihed.ras.ru");
		shell.SetParam("privkey", "C:\\Users\\evgenia\\Documents\\Diploma\\keys\\open_ssh");
		shell.SetParam("host_accept", "all");

		//shell.SetParam("dump_commands", true);
		// shell.SetParam("plink_att_num", "2");
		// shell.SetParam("plink_retry_delay", "3000");
		//shell.SetParam("home_dir", "/home/evdovina");
		//shell.SetParam("home_dir_win", "C:\\Diploma\\tmp2\\");

		message_logger::global().set_levels(vblALL, vblERR);
		gmBShManager mngr(shell);  // use the second argument to select the PBS queue
								   // Initialization of the job
		
		int power = 1;
		int n = 20;
		for (int i = 0; i < n; i++) {
			gmJob* job = mngr.CreateJob();
			job->command = gmdString::Format("chmod u+x test.sh; ./test.sh %d %d > res.txt", i, power);
			job->AddInFile("test.sh", "", gmJob::TEXT);
			job->AddOutFile(gmdString::Format("out\\test_out%d.txt", i), "STDOUT");
			job->AddOutFile(gmdString::Format("out\\test_err%d.txt", i), "STDERR");
			res = job->Submit(mngr, gmdString::Format("%d", i), false);
			printf("Job with id %s is submitted\n", job->GetID());
		}
		res = mngr.WaitAll();
		printf("All elements are calculated, adding proccess is running \n");
		gmJob* jobCalc = mngr.CreateJob();
		puts(jobCalc->GetWorkDir());
		jobCalc->AddOutFile(gmdString::Format("out\\test_out.txt"), "STDOUT");
		jobCalc->AddOutFile(gmdString::Format("out\\test_err.txt"), "STDERR"); 
		jobCalc->command = gmdString::Format("cd ../.. ;  head -n 1 job-*/work/res.txt | awk '{s+=$1} END {printf \"%%.20f\", s}'");
		res = jobCalc->Submit();

		res = jobCalc->FetchResult();
		mngr.ClearAll();
		puts("All jobs are cleared.");

	}
	catch (gmJobException &e) { (void)e; }

	gmdUninitialize();

	return 0;
}
