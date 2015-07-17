/****************************************************************************
 * \file shmngr.h
 * Header file for SH Job Manager
****************************************************************************/

#ifndef __SHMNGR_H
#define __SHMNGR_H

#include "jobmngr/jobmngr.h"
#include "named_formater.h"
#include <boost/function.hpp>


/// Class to store extra job information
class gmSHJobInfo : public gmJobInfo
{
public:
  wxString sh_data;     // sh_data saves JOBSPEC for submitted jobs and empty string for prepared jobs
  wxString sh_command;  // start command for prepared jobs

  gmSHJobInfo() {}
  gmSHJobInfo(wxString& data) : sh_data(data) {}
};


/// Implementation of SH Job Manager
class gmSHManager : public  gmJobManager
{
protected:
  //static pCSTR script_file, status_file, stdout_file, stderr_file, folder_prefix, cut_id_command, grep_squeue;
  wxString sh_dir, sh_args, name_prefix;

  virtual int Submit(gmJob& job, pCSTR user_id, bool mktemp);
  virtual int Prepare(gmJob& job, pCSTR user_id, bool mktemp);
  virtual int Resubmit(gmJob& job);
  virtual int Status(gmJob& job);
  virtual void Stop(gmJob& job);
  virtual int Attach(gmJob& job, pCSTR id);
  virtual wxString GetStdOut(gmJob& job);
  virtual wxString GetStdErr(gmJob& job);

  /// @{  make named argumens on all contex-depended template substitution
  //struct t_named_arguments;
  typedef named_formater::args t_named_arguments;
  t_named_arguments mk_named_arguments(gmJob& job);
  t_named_arguments mk_named_arguments();
  /// @}

  // Additional functions not present in the base class
  static inline gmSHJobInfo* JobInfo(gmJob& job) {
    return (gmSHJobInfo*)job._info();
  }

  int _start(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only);
  int execute_start(gmJob& job, const wxString& cmd);
  int interpret_status(const wxString& str);
  struct squeue_result_item
  {
      wxString id;
      wxString sh_data;
      wxString status_file_content;
      int      state;
      squeue_result_item() : state(-1) {}
  };
  std::vector<squeue_result_item> interpret_status_list(wxString out);
  wxString interpret_start(const wxString& str);

public:
  gmSHManager() {}
  gmSHManager(gmShell& shell_, pCSTR service=NULL) {
    Open(shell_, service);
  }
  ~gmSHManager() {
    Close();
  }

  /// Sets extra parameters for the start execution
  void Configure(pCSTR sh_dir_, pCSTR job_name_prefix);

  // Redefinitions
  virtual void Open(gmShell& shell_, pCSTR service=NULL);
  virtual int WaitAll(pCSTR id_prefix = NULL);
  virtual int FetchAll(bool fWait = true, pCSTR id_prefix = NULL);
  virtual void StopAll(pCSTR id_prefix = NULL);
  virtual void ClearAll(pCSTR id_prefix = NULL);
  //virtual void DetachAll(pCSTR id_prefix = NULL);  // use base class function
  virtual JobList RestoreAll(pCSTR id_prefix = NULL);
};

#endif
