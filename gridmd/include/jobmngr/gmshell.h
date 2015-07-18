/****************************************************************************
 * \file gmshell.h
 * Definition of class gmShell (abstract) and its successors
****************************************************************************/

#ifndef __GMSHELL_H
#define __GMSHELL_H

#include <string>
#include <gmd/string.h>
#include <gmd/arrstr.h>
#include <gmd/stopwatch.h>
#include <gmd/filefn.h>
#include <gmd/utils.h>

#include "jobmngrbase.h"


// $PERM_STORAGE variable
#define HOME_DIR_NAME     "~"
#define PERM_STORAGE_NAME "$PERM_STORAGE"

#ifdef _WIN32
#define DIR_SEPARATOR "\\"
#else
#define DIR_SEPARATOR "/"
#endif

// ============================================================================
// Abstract class to provide general job shell functionality
// ============================================================================
class gmShell : public gmJobManagerBase
{
public:
  // Target interpreter command set
  enum {CMD_SET_BASH, CMD_SET_DOS, CMD_SET_MINGW};

  // File transfer options
  // WARNING: CREATEPATH isn't documented and isn't supported by all shells
  enum {MOVE = 0x1, TEXT = 0x2, RECURSIVE = 0x4, CREATEPATH = 0x8, MAYNOTEXIST = 0x10};

  // Error codes
  enum {
    OK = 0, INVALID_REQUEST, LOCAL_FS_ERROR, LOCAL_REMOVE_DISABLED,
    NO_INPUT_FILE, CONNECTION_ERROR, DIR_CREATE_ERROR,
    COPY_ERROR, DELETE_ERROR, EXECUTE_ERROR, NONZERO_EXIT_CODE
  };

protected:
  gmdStopWatch etimer, ttimer;
  bool EnableRemoveLocal;  /// if false (by default), an attempt to remove local files/dirs generates an error
  gmdString home_dir, home_dir_win, rem_perm_dir;
  bool dump_commands;  /// dump executed commands for debug

  int error_code;
  gmdString error_msg;

  void clear_err() {
    error_code = 0;
    error_msg.Clear();
  }

  int set_err(int code, pCSTR msg = "") {
    error_msg = msg;
    return error_code = code;
  }

  void change_err_code(int code) {
    error_code = code;
  }

  int remove_local(pCSTR file, unsigned flags = RECURSIVE);

  virtual void set_param(const gmdString& par_name, const gmdString& par_value);

  virtual int execute(const gmdString& cmd,
                      gmdArrayString& out, gmdArrayString& err) = 0;
  void execute_begin(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err);
  int execute_end(int res, gmdArrayString& out, gmdArrayString& err);
  int execute_error(int code = -1);

  // involve command interpreter to execute a command localy
  int command_local(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err) {
#ifdef _WIN32
    gmdString exec_cmd = "cmd.exe /C \"" + cmd + "\"";
#else
    gmdString exec_cmd = "bash -c \"" + cmd + "\"";
#endif
    return gmdExecute(exec_cmd, out, err);
  }

  int StageIn_begin(pCSTR locpath, pCSTR rempath, unsigned flags);
  int StageOut_begin(pCSTR locpath, pCSTR rempath, unsigned flags);
  int StageIn_end(pCSTR locpath, pCSTR rempath, unsigned flags);
  int StageOut_end(pCSTR locpath, pCSTR rempath, unsigned flags);

  gmdString static GetTempFile();
  gmdString static GetTempDir();

  int static dos2unix(pCSTR src, pCSTR dst);
  int static unix2dos(pCSTR src, pCSTR dst);
  int conv_files(const gmdArrayString& srcfiles, const gmdString& dstpath,
                 int (&funct)(pCSTR,pCSTR));

  virtual gmdString get_home_dir();
  virtual gmdString get_home_dir_win();

  static int files_by_mask(pCSTR mask, gmdArrayString& filelist, unsigned flags = 0);

  static void assert_no_recursive(unsigned flags) {
    if(flags & RECURSIVE)
      LOGJOBERR("StageIn/StageOut: RECURSIVE mode is not supported");
  }

  static bool path_exists(pCSTR path, unsigned flags) {
    return gmdFileExists(path) || ((flags & RECURSIVE) && gmdDirExists(path));
  }

public:
  gmShell();
  virtual ~gmShell() {}

  int get_err_code() { return error_code; }
  const gmdString& get_err_msg() { return error_msg; }

  virtual void Open() {}
  virtual void Close() {}
  virtual int Execute(const gmdString& cmd){
    gmdArrayString out, err;
    return execute(cmd, out, err);
  }
  virtual int Execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err) {
    return execute(cmd, out, err);
  }
  virtual int ExecuteAsync(const gmdString& cmd) {
    (void) cmd; return not_implemented();
  }
  virtual int MkDir(pCSTR rempath);
  virtual int Remove(pCSTR rempath);

  virtual int Remove(const gmdArrayString& remfiles) {
    return Remove( ArrayToString(remfiles, " ") );
  }

  virtual int StageIn(pCSTR locpath, pCSTR rempath, unsigned flags = 0) = 0;
  virtual int StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags = 0);
  virtual int StageOut(pCSTR locpath, pCSTR rempath, unsigned flags = 0) = 0;
  virtual int StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags = 0);
  virtual int StageOutText(gmdString& str, pCSTR rempath);

  virtual int MkDirLocal(pCSTR locpath);
  virtual int RemoveLocal(pCSTR locpath, unsigned flags = RECURSIVE);

  inline long ExecTime() { return etimer.Time(); }
  inline long TransferTime() { return ttimer.Time(); }

  // Service functions (for internal use mainly)
	// Substitute special directories
  gmdString rem_path_subst(const gmdString& path);

  gmdString get_full_path(const gmdString& rel_path) {
    return (rel_path.StartsWith("/") ? "" : get_home_dir() + "/") + rel_path;
  }
  gmdString get_full_path_win(const gmdString& rel_path);

  virtual int check_command_set(int cmd_set);
};

#endif
