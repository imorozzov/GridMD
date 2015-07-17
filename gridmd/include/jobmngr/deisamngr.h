/****************************************************************************
 * \file deisamngr.h
 * Header file for Deisa Job Manager
****************************************************************************/

#ifndef __DEISAMNGR_H
#define __DEISAMNGR_H

#include "jobmngr/jobmngr.h"


/* Works only with wxUSE_GUI=1 and wxUSE_TIMER=1
#define wxUSE_TIMER 1
#include <wx/timer.h>

/// Timer for waking up the deshl-server
class IdleTimer : public wxTimer {
public:
  IdleTimer() : wxTimer();
  void Notify();
} idle_timer;
*/


/// Implementation of Deisa Job Manager
class DeisaManager : public  gmJobManager
{
protected:
  const char *DESHL_idle_cmd;

  wxString site, sitep, deshl_client, deshl_server, appname,
           rem_tmp_var, rem_perm_var, tar_path;
  int jobcounter;
  long server_pid;
  wxChar pathsep;
  wxString deshl_cmd;
  wxArrayString cmdout;  // not safe in multi-threaded program!

  virtual int Submit(gmJob&);
  virtual int Status(gmJob&);
  virtual int Wait(gmJob&);
  virtual void Stop(gmJob&);
  virtual int FetchResult(gmJob&);
  virtual void Clear(gmJob&);

  enum {DESHL_OUTPUT = 1, DESHL_SKIP_ERR = 2};
  int DESHL_cmd(wxString cmd, int flags = 0);
  int DESHL_check_cmd();

public:
  DeisaManager() {
    server_pid = 0;
  }

  DeisaManager(pCSTR service, pCSTR username = NULL){
    server_pid = 0;
    Open(service, username);
  }

  ~DeisaManager(){
    Close();
  }

  virtual void Open(pCSTR service, pCSTR username);
  virtual void Close();

  virtual void MkDir(pCSTR remdir);
  virtual void StageIn(pCSTR locfile, pCSTR remfile, int type = fCopy);
  virtual void StageOut(pCSTR locfile, pCSTR remfile, int type = fCopy);
  virtual void Remove(pCSTR remfile);
};


/// Abstract class to store extra job information
class gmDeisaJobInfo : public gmJobExtraInfo
{
public:
  wxArrayString untar_list;
  wxString locdir, stoutfile;
  bool locdir_exists;

  gmDeisaJobInfo() {};
//  virtual ~gmDeisaJobInfo() {}
};

#endif
