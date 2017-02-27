/*****************************************************************************
GridMD class library
Example for PBS Job Manager and gmShellLibssh communication protocol:
working with multiple jobs

Copyright (c) 2017 Igor Morozov, Ilya Valuev, Evgenia Vdovina 

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
	if (!gmdInitialize()) {
		puts("Failed to initialize the GridMD library.");
		return -1;
	}

	message_logger::global().set_throw(true);  

	try {
		int res;
		message_logger::global().set_levels(vblALL, vblALLBAD);
		gmShellLibssh shell("vdovina", "nwo5.ihed.ras.ru");
		shell.SetParam("privkey", "C:\\Users\\evgenia\\Documents\\Diploma\\keys\\open_ssh");
		shell.SetParam("host_accept", "all");

		message_logger::global().set_levels(vblALL, vblERR);
		gmBShManager mngr(shell);
		
		int power = 1; // степень, в которую нужно возвести экспоненту
		int n = 20; // сколько членов ряда нужно считать

		for (int i = 0; i < n; i++) {
			gmJob* job = mngr.CreateJob();
			job->command = gmdString::Format("power=%d^%d; fact=$(echo \"define f(x) {if (x>1){return x*f(x-1)};return 1} f(%d)\" | bc); echo \"$power / $fact\" | bc -l > res.txt ", 
				power, i, i); // вычисляем каждый член ряда по отдельности
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
		jobCalc->command = gmdString::Format("cd ../.. ;  head -n 1 job-*/work/res.txt | awk '{s+=$1} END {printf \"%%.20f\", s}'"); // суммируем полученные результаты
		
		res = jobCalc->Submit();
		res = jobCalc->FetchResult();
		mngr.ClearAll();
		puts("All jobs are cleared.");

	}
	catch (gmJobException &e) { (void)e; }

	gmdUninitialize();

	return 0;
}
