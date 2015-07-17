/****************************************************************************
 * \file winshell.cpp
 * Implementation of the class gmShellWin
****************************************************************************/

#include <stdio.h>
#include <gmd/utils.h>
#include <gmd/filefn.h>
#include <gmd/filename.h>
#include <gmd/ffile.h>
#include "jobmngr/winshell.h"


gmShellWin::gmShellWin() {
  // Global parameters which could be read from XML
  InitParam("rem_tmp_dir", gmdFileName::GetTempDir() + "\\gmJobManager");
  InitParam("rem_perm_dir", "");
}


// Execute Shell command
int gmShellWin::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  execute_begin(cmd, out, err);

  long res = gmdExecute(cmd, out, err);
  if(res == -1) LOGJOBERR("Error executing shell!"); // Fatal error
  
  return execute_end(res, out, err);
}


int gmShellWin::ExecuteAsync(const gmdString& cmd){
  if(dump_commands)
    LOGJOBMSG4( (pCSTR)("------ _execute_async (cmd)------\n" + cmd) );

  int pid = gmdExecute(cmd);
  if(pid <= 0) LOGJOBERR("Error executing shell!"); // Fatal error

  return pid;
}


int gmShellWin::Remove(const gmdArrayString& remfiles) {
  int res = 0;
  for(unsigned i=0; i<remfiles.Count(); i++) {
    int res1 = Remove( remfiles[i] );
    res = res || res1;
  }
  return res;
}


int gmShellWin::StageIn(pCSTR locpath, pCSTR rempath, unsigned flags){
  gmdString lpath(locpath), rpath(get_full_path_win(rempath));

  if( StageIn_begin(locpath, rpath, flags) )
    return StageIn_end(locpath, rpath, flags);

  if( gmdFileExists(lpath) ) {
    // Copy a single file using gmdCopyFile
    if( gmdDirExists(rpath) ) {
      if( !rpath.EndsWith("\\") ) rpath += "\\";
      rpath += get_filename_ending(lpath);
    }
    if( !gmdCopyFile(lpath, rpath) ) set_err(COPY_ERROR, "File copy error");
  }
  else {
    // Copy multiple files or directories using XCOPY (may be slower?)
    lpath.Replace("/", "\\");
    xcopy(lpath, rpath, flags);
  }

  return StageIn_end(lpath, rpath, flags);
}


int gmShellWin::StageOut(pCSTR locpath, pCSTR rempath, unsigned flags){
  gmdString lpath(locpath), rpath(get_full_path_win(rempath));

  if( StageOut_begin(locpath, rpath, flags) )
    return StageOut_end(locpath, rpath, flags);

  if( gmdFileExists(rpath) ) {
    // Copy a single file using gmdCopyFile
    if( gmdDirExists(lpath) ) {
      if( !lpath.EndsWith("\\" ) ) lpath += "\\";
      lpath += get_filename_ending(rpath);
    }
    if( !gmdCopyFile(rpath, lpath) ) set_err(COPY_ERROR, "File copy error");
  }
  else {
    // Copy multiple files or directories using XCOPY (may be slower?)
    lpath.Replace("/", "\\");
    xcopy(rpath, lpath, flags);
  }

  return StageOut_end(lpath, rpath, flags);
}


int gmShellWin::xcopy(gmdString& src, gmdString& dst, unsigned flags) {
  if( gmdDirExists(src) ) {
    if( !dst.EndsWith("\\") ) dst += "\\";
    dst += get_filename_ending(src);
    if( !gmdDirExists(dst) ) gmdMkdir(dst);
  }
  gmdString cmd = "xcopy /Y /Q "; //DEBUG
  if(flags & RECURSIVE) cmd += "/E ";

  gmdArrayString out, err;
  int res = command_local(cmd + "\"" + src + "\" \"" + dst + "\"", out, err);
  if( !err.IsEmpty() ) {
    // According to manual
    // http://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/xcopy.mspx?mfr=true
    // XCOPY should return 1 if there are not source files.
    // In fact it returns 4 if there are no wildcards or 0 if there are wildcards
    // but no files are copied
    res = (res == 1) ||
          (res == 0 && has_wildcards(src)) ||
          (res == 4 && !has_wildcards(src) && !path_exists(src, flags)) ?
      NO_INPUT_FILE : COPY_ERROR;
    set_err(res, ArrayToString(err));
  }
  
  return res;
}


int gmShellWin::StageOutText(gmdString& str, pCSTR rempath) {
  gmdFFile file;
  int res = !file.Open(get_full_path_win(rempath), "rt");
  if(!res) file.ReadAll(&str);
  return res;
}


int gmShellWin::check_command_set(int cmd_set){
  return cmd_set != CMD_SET_DOS;
}
