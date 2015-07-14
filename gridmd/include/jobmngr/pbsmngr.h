/****************************************************************************
 * \file pbsmngr.h
 * Header file for PBS Job Manager
****************************************************************************/

#ifndef __PBSMNGR_H
#define __PBSMNGR_H

#include "jobmngr/jobmngr.h"


/// Implementation of SLURM Job Manager
class gmPBSManager : public  gmJobManager
{
protected:
  gmdString cmd_prefix, mpi_run_cmd, mpi_args;

  virtual gmdString get_subm_cmd(gmJob& job);
  virtual gmdString get_script_body(gmJob& job, gmdString& user_cmd);
  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd);
  virtual gmdString get_status_cmd();
  virtual int interpret_status(const gmdString& str);
  virtual gmdString get_stop_cmd(gmJob& job);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  
  void init();

public:
  gmPBSManager() { init(); }
  gmPBSManager(gmShell& shell_) { init(); Open(shell_); }
  ~gmPBSManager() { Close(); }

  virtual bool mpi_support() { return true; }
  virtual pCSTR GetClassName() {
    return "gmPBSManager";
  }
};

#endif
