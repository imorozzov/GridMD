/****************************************************************************
 * \file jobmngr.h
 * Definition of classes gmJob and gmJobManager (abstract)
****************************************************************************/

#ifndef __JOBMNGR_H
#define __JOBMNGR_H

#include <string>
#include <list>
#include <string.h>
#include <gmd/string.h>

#include "jobmngr/gmshell.h"


// ============================================================================
// Type definitions
// ============================================================================

class gmJob;

/// List of all possible job states
enum gmJobState {
  JOB_INIT = 0, JOB_PREPARED, JOB_SUBMITTED, JOB_QUEUED, JOB_RUNNING,
  JOB_SUSPENDED, JOB_EXITING, JOB_COMPLETED, JOB_HAVERESULT, JOB_FAILED };

/// Job list
typedef std::list<gmJob*> JobList;


// ============================================================================
// Structure to store information about input or output file
// ============================================================================

class gmFileTrans
{
public:
  gmdString lpath, rpath;
  unsigned flags;

  gmFileTrans() : flags(0) {}
  gmFileTrans(pCSTR lpath_, pCSTR rpath_, unsigned flags_) {
    lpath = lpath_; rpath = rpath_; flags = flags_;
  }
};

typedef std::list<gmFileTrans> gmFileList;


// ============================================================================
// Job execution timing
// ============================================================================
struct gmJobTiming {
  long submitting, queued, running;
	gmJobTiming():submitting(-1), queued(-1), running(-1){}
};


// ============================================================================
// Abstract class to provide general job manager functionality
// ============================================================================

class gmJobManager : public gmJobManagerBase
{
  friend class gmJob;

public:
  enum {LOG_NONE = 0, LOG_TIMING = 0x1};

protected:
  // Static constants
  static pCSTR script_file, user_cmd_file, status_file, log_file, workdir_file,
               stdout_file, stderr_file, info_file;

  // Internal variables
  gmShell *shell;
  gmdString rem_tmp_dir;
  JobList jobs;
  bool fBatchOperation;
  bool make_cmd_script;
  bool mpi_enabled;
  int jobs_subm, jobs_wait;
  int wait_time;
  int log_level;

  // User parameters
  bool save_job_info;    // If true the file info is created in the job working directory
  int jobs_limit;        // Maximum number of submitted jobs in this manager, 0 for unlimited
  int wait_timeout;      // Maximum wait time in Wait functions (in ms), 0 for unlimited
  int wait_delay;        // Delay between status quires in Wait functions (in ms)
  gmdString init_job_cmd; // Command to be executed before starting each job
                         // (runs at the first node in MPI jobs)
  gmdString end_job_cmd;  // Command to be executed afted completition of each job
  gmdString pre_subm_cmd; // Command to be executed just before submission (output is lost)


  // Single job manipulation
  virtual int Submit(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only = false);
  virtual int RemoteSubmit(gmJob& job, const gmdString& cmd);
  virtual int Prepare(gmJob& job, pCSTR user_id, bool mktemp);
  virtual int Resubmit(gmJob& job);
  virtual int Status(gmJob& job);
  virtual int Stop(gmJob& job);
  virtual int Attach(gmJob& job, pCSTR id);
  virtual void Detach(gmJob& job){
    jobs.remove(&job);
  }
  virtual int Wait(gmJob& job);
  virtual int FetchResult(gmJob& job, bool fWait);
  virtual void Clear(gmJob& job);
  virtual int GetTiming(gmJob& job, gmJobTiming& jobtim);

  // Retrive std file names
  gmdString GetStdOut(gmJob& job);
  gmdString GetStdErr(gmJob& job);

  // Staging input and output data
  virtual int StageAllInput(gmJob& job, gmdString& remcmd);
  virtual int StageAllOutput(gmJob& job, gmdString& remcmd);
  virtual int StageOutputFile(gmJob& job, gmFileTrans& file, gmdString& remcmd);

  void drain_prepared(gmJob* job_exclude = NULL);
  gmJob* CreateManagedJob(pCSTR id);

  // Static functions for job object private members setup
  static void SetJobData(gmJob& job, pCSTR id, pCSTR dir = NULL,
    pCSTR wdir = NULL, pCSTR subm_cmd = NULL, pCSTR subm_id = NULL);
  static void SetJobState(gmJob& job, int state);

  // Service static functions
  static gmdString ConvertSpecials(gmdString& src);

  static void assert_no_specials(pCSTR str, pCSTR par_name) {
    if( strcspn(str, "\'\" /\\\n\r\t") < strlen(str) )
      LOGJOBERR( fmt("Parameter %s contain invalid characters!", par_name) );
  }

  // Functions to be overriden
  // -------------------------
  virtual gmdString get_user_cmd(gmJob& job, gmdString& user_cmd, bool make_script);
  virtual gmdString get_subm_cmd(gmJob& job)                         { (void)&job; return ""; }
  virtual gmdString get_script_body(gmJob& job, gmdString& user_cmd) { (void)&job; (void) user_cmd; return ""; }
  virtual int exec_subm_cmd(gmJob& job, const gmdString& cmd)        { (void)&job; (void) cmd; return 1; }
  virtual gmdString get_status_cmd()                                 { return ""; }
  virtual int interpret_status(const gmdString& str)                 { (void) str; return JOB_FAILED; }
  virtual gmdString get_stop_cmd(gmJob& job)                         { (void)&job; return ""; }
  virtual void set_param(const gmdString& par_name, const gmdString& par_value);
  // -------------------------

  void init();

public:
  // Creation-destruction
  gmJobManager()  :
    fBatchOperation(false), make_cmd_script(false), jobs_subm(0), jobs_wait(0),
    wait_time(0), log_level(0)
  {}

  virtual ~gmJobManager() { Close(); }

  virtual void Open(gmShell& shell_);
  virtual void Close(){
    DetachAll();
  }

  // Creation of managed job
  gmJob* CreateJob() {
    return CreateManagedJob(NULL);
  }
  gmJob* Restore(pCSTR id){
    return CreateManagedJob(id);
  }

  // Collective operations
  virtual int WaitAll(pCSTR id_prefix = NULL);
  virtual int FetchAll(bool fWait = true, pCSTR id_prefix = NULL);
  virtual void StopAll(pCSTR id_prefix = NULL);
  virtual void ClearAll(pCSTR id_prefix = NULL);
  virtual void DetachAll(pCSTR id_prefix = NULL);
  //virtual JobList RestoreAll(pCSTR id_prefix = NULL) = 0;  // temporary not supported

  // Information
  JobList GetJobList(pCSTR id_prefix = NULL) const;
  gmJob* JobByID(pCSTR id) const;
  gmJob* operator[](pCSTR id){ return JobByID(id); }
  long WaitTime() const { return wait_time; }
  gmShell* GetShell() const { return shell; }
  virtual bool mpi_support() { return false; }
  virtual int required_cmd_set() { return gmShell::CMD_SET_BASH; }

  // Logging
  void set_log_level(int log_level_) { log_level = log_level_; }
};


// ============================================================================
// Main class to store job-related information
// and to provide user interace for job management functions
// ============================================================================

class gmJob
{
  friend class gmJobManager;

public:
  // Job types
  enum {NORMAL = 0, DUMMY = 0x01};

  // File transfer options 
  enum {
    MOVE = gmShell::MOVE, TEXT = gmShell::TEXT, RECURSIVE = gmShell::RECURSIVE,
    CREATEPATH = gmShell::CREATEPATH, MAYNOTEXIST = gmShell::MAYNOTEXIST,
    REMOTE = 0x100
  };

  gmFileList infiles, outfiles;  // lists of input and output files
  gmdString stdoutf, stderrf;  // path to stdout and strerr files related to the job workdir
  gmdString command;   // command to be executed when the job is running
  gmdString forerunner;  // the forerunner's job id in the case of hard link
  gmdString info;      // Extra job info that can be saved into 'info' file in the working directory
  unsigned nproc;     // total number of processes
  unsigned ppn;       // number of processes per node
  unsigned walltime;  // maximum execution wall time in seconds
  bool mpi;           // indicates that this is an MPI job
  unsigned type;      // define specific job types

protected:
  int state;          // current job state
  gmJobManager* mng;  // pointer to the JobManager the job is linked with
  gmdString id;        // job id
  gmdString dir;       // job directory for service files (path related to $HOME)
  gmdString wdir;      // work directory where the command is executed (path related to $HOME)
  gmdString subm_cmd;  // submission command
  gmdString subm_id;   // internal id assigned by a submission system 
  bool managed;       // determines if the job is deleted (true) or reinitialized (false) on Clear()

  static const char* state_names[10];

  inline void Init(){
    if(!managed) mng = NULL;
    state = JOB_INIT;
    dir = wdir = subm_cmd = subm_id = "";
  }

  void state_change(int oldstate, int newstate);

public:
  gmJob(pCSTR cmd = NULL, int nproc_ = 0, int ppn_ = 0) {
    mpi = false;
    managed = false;
    nproc = ppn = walltime = 0;
    type = NORMAL;
    if(cmd) command = cmd;
    if(nproc_ > 0) {
      mpi = true;
      nproc = nproc_;
      if(ppn_ > 0) ppn = ppn_;
    }
    Init();
  }

  ~gmJob() {
    if(mng) {
      mng->Detach(*this);
      state_change(state, JOB_INIT);
    }
  }

  inline void AccertState(int minstate) {
    if(state < minstate)
      LOGJOBERR( fmt("Incorrect job state %s when required %s",
        gmJob::StateName(state), gmJob::StateName(minstate)) );
  }

  int GetState() {
    if(mng && state >= JOB_SUBMITTED && state < JOB_COMPLETED){
      int newstate = mng->Status(*this);
      state_change(state, newstate);
      state = newstate;
    }
    return state;
  }

  pCSTR GetStateStr() {
    return gmJob::StateName(GetState());
  }

  void AddInFile(pCSTR src, pCSTR workpath = "", unsigned flags = 0){
    infiles.push_back( gmFileTrans(src, workpath, flags) );
  }

  void AddOutFile(pCSTR dst, pCSTR workpath, unsigned flags = 0){
    if(workpath[0] == 0) LOGJOBERR("Empty output file name in gmJob::AddOutFile");
    outfiles.push_back( gmFileTrans(dst, workpath, flags) );
  }

  void ClearInFiles(){
    infiles.clear();
  }

  void ClearOutFiles(){
    outfiles.clear();
  }

  int LastState(){ return state; }

  static inline pCSTR StateName(int state){
      return (state >=(int)0 && state < int( sizeof(state_names)/sizeof(char*) ) ) ?
      state_names[state] : "INVALID";
  }

  gmdString GetID(){
    return state >= JOB_PREPARED ? id : "";
  }

  gmdString GetWorkDir(){
    return state >= JOB_PREPARED ? wdir : "";
  }

  int Submit(gmJobManager& mngr, pCSTR user_id = NULL, bool mktemp = true);
  int Submit(pCSTR user_id = NULL, bool mktemp = true){
    if(!managed) LOGJOBERR("Non-managed job is submitted without mngr parameter");
    return Submit(*mng, user_id, mktemp);
  }

  int Attach(gmJobManager& mngr, pCSTR id_);

  int Wait(){
    AccertState(JOB_SUBMITTED);
    if(state >= JOB_COMPLETED) return state;
    int newstate = mng->Wait(*this);
    state_change(state, newstate);
    return state = newstate;
  }

  int FetchResult(bool fWait = true){
    AccertState(JOB_PREPARED);
    int res = mng->FetchResult(*this, fWait);
    if( state == JOB_COMPLETED )
      state = res ? JOB_FAILED : JOB_HAVERESULT;
    return state;
  }

  int StageOut(pCSTR dst, pCSTR workpath, unsigned flags = 0);

  int Stop(){
    AccertState(JOB_PREPARED);
    int newstate = state;
    if(state < JOB_SUBMITTED) newstate = JOB_FAILED;
    else if(state < JOB_COMPLETED) newstate = mng->Stop(*this);
    state_change(state, newstate);
    return state = newstate;
  }

  void Detach() {
    if(managed) delete this;  // mng->Detach is called from destructor for managed jobs;
    else{
      if(mng) mng->Detach(*this);
      state_change(state, JOB_INIT);
      Init();
    }
  }

  void Clear() {
    if(state >= JOB_PREPARED) Stop();  // state_change is called from Stop
    if(mng) {
      mng->Clear(*this);
      mng = NULL;
    }
    if(managed) delete this; else Init();
  }

  int GetTiming(gmJobTiming& jobtim) {
    AccertState(JOB_PREPARED);
    return mng ? mng->GetTiming(*this, jobtim) : -1;
  }

  // Access to private members
  inline int _state() { return state; }
  inline gmdString _id() { return id; }
  inline gmdString _dir() { return dir; }
  inline gmdString _wdir() { return wdir; }
  inline gmdString _subm_cmd() { return subm_cmd; }
  inline gmdString _subm_id() { return subm_id; }
  //bool haveLocals();
};

#endif
