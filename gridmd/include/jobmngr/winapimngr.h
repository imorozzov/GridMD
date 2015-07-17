/****************************************************************************
 * \file winapimngr.h
 * Header file for the Windows API Job Manager
****************************************************************************/

#ifndef __WINAPIMNGR_H
#define __WINAPIMNGR_H

#include "jobmngr/jobmngr.h"

class gmWinAPIManager : public gmJobManager
{
protected:
  gmdString mpi_run_cmd, mpi_args;
  int kill_delay; // in miliseconds
  int kill_ntries;

  virtual int Submit(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only = false);
  virtual int RemoteSubmit(gmJob& job, const gmdString& cmd);
  virtual int Status(gmJob& job);
  virtual int Stop(gmJob& job);
  virtual int Attach(gmJob& job, pCSTR id);

  virtual int StageAllInput(gmJob& job, gmdString& remcmd);
  virtual int StageOutputFile(gmJob& job, gmFileTrans& file, gmdString& remcmd);

  virtual int WaitAll(pCSTR id_prefix = NULL);
  virtual void StopAll(pCSTR id_prefix = NULL);

  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);

  // Service functions
  static gmdString ReadFile(pCSTR filename);
  static bool ProcessRunning(int pid);
  void SafeProcessKill(int pid);

  static int job_pid(gmJob& job) {
    long pid = 0;
    job._subm_id().ToLong(&pid);
    return pid;
  }
  
  void init();

public:
  gmWinAPIManager() { init(); }
  gmWinAPIManager(gmShell& shell_) { init(); Open(shell_); }
  ~gmWinAPIManager() { Close(); }

  virtual int required_cmd_set() { return gmShell::CMD_SET_DOS; }
  virtual bool mpi_support() { return true; }

  virtual pCSTR GetClassName() {
    return "gmWinAPIManager";
  }
};


#endif
