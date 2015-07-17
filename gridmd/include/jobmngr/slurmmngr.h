/****************************************************************************
 * \file slurmmngr.h
 * Header file for SLURM Job Manager
****************************************************************************/

#ifndef __SLURMMNGR_H
#define __SLURMMNGR_H

#include "jobmngr/jobmngr.h"


/// Implementation of SLURM Job Manager
class gmSLURMManager : public  gmJobManager
{
protected:
  gmdString cmd_prefix;

  virtual gmdString get_subm_cmd(gmJob& job);
  virtual gmdString get_script_body(gmJob& job, gmdString& user_cmd);
  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd);
  virtual gmdString get_status_cmd();
  virtual int interpret_status(const gmdString& str);
  virtual gmdString get_stop_cmd(gmJob& job);
  gmdString interpret_sbatch(const gmdString& str);
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  
  void init();

public:
  gmSLURMManager() { init(); }
  gmSLURMManager(gmShell& shell_) { init(); Open(shell_); }
  ~gmSLURMManager() { Close(); }

  virtual bool mpi_support() { return true; }
  virtual pCSTR GetClassName() {
    return "gmSLURMManager";
  }
};

#endif
