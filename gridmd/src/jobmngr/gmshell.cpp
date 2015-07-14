/****************************************************************************
 * \file gmshell.cpp
 * Implementation of the class gmShell
****************************************************************************/

#include <stdio.h>
#ifdef _WIN32
#include <userenv.h>
#endif

#include <gmd/filename.h>
#include <gmd/dir.h>

#include "jobmngr/gmshell.h"


gmShell::gmShell() {
  etimer.Pause(); ttimer.Pause();

  clear_err();

  InitParam("rem_tmp_dir", ".gmJobManager");
  InitParam("home_dir", "");
  InitParam("home_dir_win", "");
  InitParam("rem_perm_dir", "");
  InitParam("local_remove", "false");
  InitParam("dump_commands", "false");
}


void gmShell::set_param(const gmdString& par_name, const gmdString& par_value){
  if(par_name == "home_dir") home_dir = par_value;
  else if(par_name == "home_dir_win") home_dir_win = par_value;
  else if(par_name == "rem_perm_dir") rem_perm_dir = par_value;
  else if(par_name == "local_remove")
    EnableRemoveLocal = ParseBoolParam(par_name, par_value);
  else if(par_name == "dump_commands")
    dump_commands = ParseBoolParam(par_name, par_value);
  
  gmJobManagerBase::set_param(par_name, par_value);
}


int gmShell::remove_local(pCSTR locpath, unsigned flags){
  int res;
  gmdArrayString out, err;

  if( gmdFileExists(locpath) )
    res = !gmdRemoveFile(locpath);
  else {  // Remove the directore tree
#ifdef _WIN32
    gmdString lpath(locpath);
    lpath.Replace("/", "\\");
    gmdString cmd = (flags & RECURSIVE) && gmdDirExists(lpath) ?
      "rd /S /Q \"" : "del /Q \"";
    res = command_local(cmd + lpath + "\"", out, err);
#else
    // Bash is needed to process file masks
    gmdString cmd = (flags & RECURSIVE) ? "rm -fr " : "rm -f ";
    res = command_local(cmd + locpath, out, err);
#endif
  }
  if(res && !(flags & MAYNOTEXIST)) {
    LOGJOBWARN(fmt("Cannot delete the local path %s",locpath));
    res = set_err(DELETE_ERROR, ArrayToString(err));
  }

  return res;
}


int gmShell::RemoveLocal(pCSTR locpath, unsigned flags){
  if(EnableRemoveLocal)
    return remove_local(locpath, flags);

  LOGJOBWARN("Removal of local files and directories is disabled");
  return set_err(LOCAL_REMOVE_DISABLED);
}


void gmShell::execute_begin(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  out.Clear(); err.Clear();
  clear_err();

  if(dump_commands)
    LOGJOBMSG4( (pCSTR)("------ execute (cmd)------\n" + cmd) );

  etimer.Resume();
}


int gmShell::execute_end(int res, gmdArrayString& out, gmdArrayString& err) {
  etimer.Pause();

  if(res) set_err(NONZERO_EXIT_CODE, ArrayToString(err));

  if(dump_commands) {
    LOGJOBMSG4( fmt("------ execute res=%i" , res ) );
    if(!out.IsEmpty())
      LOGJOBMSG4( (pCSTR) ("------ execute (cout)------\n" + ArrayToString(out) ) );
    if(!err.IsEmpty())
      LOGJOBMSG4( (pCSTR) ("------ execute (cerr)------\n" + ArrayToString(err) ) );
  }

  return res;
}


int gmShell::execute_error(int code) {
  etimer.Pause();
  if(code) LOGJOBWARN((pCSTR)get_err_msg().c_str());
  return -1;
}


int gmShell::StageIn_begin(pCSTR locpath, pCSTR rempath, unsigned flags) {
  clear_err();

  // Create output dir
  if(flags & CREATEPATH) {
    gmdString newdir(rempath);
    bool rpath_is_dir = newdir.EndsWith("/") || newdir.EndsWith("\\");
    if(rpath_is_dir) newdir.Truncate(newdir.Length() - 1);

    if( !(!locpath || flags & RECURSIVE || rpath_is_dir || has_wildcards(locpath)) ) {
      gmdFileName filename(newdir);
      newdir = filename.GetPath();
    }
    if( !newdir.IsEmpty() ) MkDir(newdir);
  }

  if(!error_code) ttimer.Resume();

  return error_code;
}


int gmShell::StageOut_begin(pCSTR locpath, pCSTR rempath, unsigned flags) {
  clear_err();
  gmdString lpath(locpath);

  // Create output dir
  if(flags & CREATEPATH) {
    gmdString newdir;
    bool lpath_is_dir = lpath.EndsWith("/") || lpath.EndsWith("\\");
    if(lpath_is_dir) lpath.Truncate(lpath.Length() - 1);

    if( !rempath || flags & RECURSIVE || lpath_is_dir || has_wildcards(rempath) )
      newdir = lpath; // create a directory rpath
    else { // create lpath without ending file name
      gmdFileName filename(lpath);
      newdir = filename.GetPath();
    }
    if( !newdir.IsEmpty() && !gmdDirExists(newdir) ) 
      MkDirLocal(newdir);
  }
  else if(!rempath && !gmdDirExists(lpath) )
    set_err(COPY_ERROR, lpath + ": output directory does not exist");

  if(!error_code) ttimer.Resume();

  return error_code;
}


int gmShell::StageIn_end(pCSTR locpath, pCSTR rempath, unsigned flags) {
  if(error_code) {  // error
    if(error_code == NO_INPUT_FILE && (flags & MAYNOTEXIST))  // ignore it
      clear_err();
    else { // report as warning
      if(!locpath) locpath = "<FILE LIST>";
      LOGJOBWARN(fmt("%s => %s upload error: %s",
        locpath, rempath, (pCSTR)get_err_msg().c_str()));
    }
  }
  else if((flags & MOVE) && locpath)  // no error and locpath is not a list
    RemoveLocal(locpath, flags);

  ttimer.Pause();

  return error_code;
}


int gmShell::StageOut_end(pCSTR locpath, pCSTR rempath, unsigned flags) {
  if(error_code) {  // error
    if(error_code == NO_INPUT_FILE && (flags & MAYNOTEXIST))  // ignore it
      clear_err();
    else { // report as warning
      if(!rempath) rempath = "<FILE LIST>";
      LOGJOBWARN(fmt("%s <= %s download error: %s",
        locpath, rempath, (pCSTR)get_err_msg().c_str()));
    }
  }
  else if((flags & MOVE) && rempath)  // no error and rempath is not a list
    Remove(rempath);

  ttimer.Pause();

  return error_code;
}


int gmShell::StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags){
  int res = 0;

  // Create output dir if needed
  if(flags & CREATEPATH) res = MkDir(remdir);

   // Copy files
  if(!res)
    for(unsigned i=0; i<locfiles.GetCount(); i++) {
      int res1 = StageIn(locfiles[i], remdir, flags & ~CREATEPATH);
      res = res || res1;
    }

  return res;
}


int gmShell::StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags){
  int res = 0;

  // Create output dir if needed
  if( !gmdDirExists(locdir) ) {
    if(flags & CREATEPATH)
      res = MkDirLocal(locdir);
    else {
      res = set_err(COPY_ERROR, fmt("Output directory %s does not exist\n", locdir));
      LOGJOBWARN((pCSTR)get_err_msg().c_str());
    }
  }

  if(!res) {
    gmdArrayString rmlist;
    for(unsigned i=0; i<remfiles.GetCount(); i++) {
      int res1 = StageOut(locdir, remfiles[i], flags & ~(CREATEPATH | MOVE));
      if((flags & MOVE) && !res1) rmlist.Add(remfiles[i]);
      res = res || res1;
    }

    if( !rmlist.IsEmpty() ) {
      int res1 = Remove(rmlist);  // Remove source files
      if( !(flags & MAYNOTEXIST) ) res = res || res1;
    }
  }

  return res;
}


int gmShell::StageOutText(gmdString& str, pCSTR rempath) {
  gmdArrayString out, err;
  int res = Execute(gmdString("cat ") + rempath, out, err);
  if(!res) str = ArrayToString(out);
  return res;
}


int gmShell::MkDir(pCSTR rempath){
  int res = Execute(gmdString("mkdir -p ") + rempath);
  if(res) change_err_code(DIR_CREATE_ERROR);
  return res;
}


int gmShell::Remove(pCSTR rempath){
  int res = Execute(gmdString("rm -Rf ") + rempath);
  if(res) change_err_code(DELETE_ERROR);
  return res;
}


int gmShell::MkDirLocal(pCSTR locpath){
  int res = 0;

  if( !gmdDirExists(locpath) ) {  // Create the directore tree
    gmdArrayString out, err;
#ifdef _WIN32
    gmdString lpath(locpath);
    lpath.Replace("/", "\\");
    res = command_local( gmdString("md \"") + lpath + "\"", out, err );
#else
    res = command_local( gmdString("mkdir -p ") + locpath, out, err );
#endif
    if(res) {
      LOGJOBWARN(fmt("Cannot create the local directory %s",locpath));
      res = set_err(DIR_CREATE_ERROR, ArrayToString(err));
    }
  }

  return res;
}


gmdString gmShell::rem_path_subst(const gmdString& path){
  gmdString res(path);

  // Substitute home directory
  res.Replace(HOME_DIR_NAME, get_home_dir());

  // Substitute permanent storage direcory
  if( !rem_perm_dir.IsEmpty() ) {
    res.Replace(PERM_STORAGE_NAME "/", rem_perm_dir);  // replace all occurrences with ending '/'
    gmdString res1;
    if( res.EndsWith(PERM_STORAGE_NAME, &res1) ) // replace at the string end
      res = res1 + rem_perm_dir;
  }

  return res;
}


gmdString gmShell::get_full_path_win(const gmdString& rel_path) {
  gmdString full_path;
  full_path = (rel_path.StartsWith("\\") || rel_path.Contains(":")) ? 
    rel_path :
    get_home_dir_win() + "\\" + rel_path;
  full_path.Replace("/", "\\");
  return full_path;
}


gmdString gmShell::GetTempFile() {
#ifdef _WIN32
  gmdString res;
  char *tmpfile = _tempnam(NULL, "gmShell");
  if(tmpfile) {
    res = tmpfile;
    free(tmpfile);
  }
  else LOGJOBERR("Failed to create a temporary file");
  return res;
#else
  LOGERR0("GetTempFile: not implemented");
  return "";
#endif
}


gmdString gmShell::GetTempDir() {
#ifdef _WIN32
  gmdString tempdir = GetTempFile();
  if( !gmdMkdir(tempdir) )
    LOGJOBERR(fmt("Cannot create temporary directory %s", (pCSTR)tempdir.c_str()));
  return tempdir;
#else
  LOGERR0("GetTempFile: not implemented");
  return "";
#endif
}


int gmShell::check_command_set(int cmd_set){
  return cmd_set != CMD_SET_BASH;
}


int gmShell::dos2unix(pCSTR src, pCSTR dst){
  FILE *fi = fopen(src, "rb");
  if(!fi) return NO_INPUT_FILE;

  FILE *fo = fopen(dst, "wb");
  if(!fo) {
    fclose(fi);
    return COPY_ERROR;
  }

  int res = OK;
  char c;
  while( (c = fgetc(fi)) != EOF )
    if(c != '\r' && fputc(c, fo) == EOF) { res = COPY_ERROR; break; }  // put any char but '\r'

  fclose(fo);
  fclose(fi);
  return res;
}


int gmShell::unix2dos(pCSTR src, pCSTR dst){
  FILE *fi = fopen(src, "rb");
  if(!fi) return NO_INPUT_FILE;

  FILE *fo = fopen(dst, "wb");
  if(!fo) {
    fclose(fi);
    return COPY_ERROR;
  }

  int res = 0;
  char c;
  while( (c = fgetc(fi)) != EOF ) {
    if(c == '\r') continue;  // skip all '\r' in the input file
    if(c == '\n' && fputc('\r', fo) == EOF) { res = 3; break; }  // put '\r' before each '\n'
    if(fputc(c, fo) == EOF) { res = COPY_ERROR; break; }  // put all other chars
  }

  fclose(fo);
  fclose(fi);
  return res;
}


int gmShell::conv_files(const gmdArrayString& srcfiles, const gmdString& dstpath,
                        int (&funct)(pCSTR,pCSTR)) {
  int res = 0;

  bool dst_is_dir = gmdDirExists(dstpath);

  for(unsigned i=0; i<srcfiles.GetCount(); i++) {
    const gmdString& srcfile = srcfiles[i];

    gmdString dstfile = dstpath;
    if(dst_is_dir) {
      gmdFileName filename(srcfile);
      dstfile += '\\' + filename.GetFullName();
    }

    int res1 = funct(srcfile, dstfile);
    if(res1 > res) {
      res = res1;
      set_err( res1, fmt("%s => %s: No such file or conversion failed\n",
                         (pCSTR)srcfile.c_str(), (pCSTR)dstfile.c_str()) );
    }
  }

  return res;
}


int gmShell::files_by_mask(pCSTR mask, gmdArrayString& filelist, unsigned flags) {
  filelist.Clear();
  gmdFileName fn(mask);

  return gmdDir::GetAllFiles(
    fn.GetPath(), &filelist, fn.GetFullName(), 
    gmdDIR_FILES | gmdDIR_HIDDEN | (flags & RECURSIVE ? gmdDIR_DIRS : 0)
  );
}


gmdString gmShell::get_home_dir() {
  if( !home_dir.IsEmpty() ) return home_dir;

  gmdArrayString out, err;
  if( Execute("echo $HOME", out, err) || out.IsEmpty() || !err.IsEmpty() ) {
    LOGJOBERR("Unable to get home directory");
    return "";
  }

  return (home_dir = out[0]);
}


gmdString gmShell::get_home_dir_win() {
#ifdef _WIN32
  if( home_dir_win.IsEmpty() ) {
    char szHomeDirBuf[MAX_PATH];
    DWORD BufSize = MAX_PATH;
    HANDLE hToken = 0;

    OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken );
    if( !hToken || !GetUserProfileDirectoryA(hToken, szHomeDirBuf, &BufSize) )
      LOGJOBERR("Unable to get Windows user home directory");
    CloseHandle( hToken );

    home_dir_win = (pCSTR)szHomeDirBuf;
  }
#endif
  return home_dir_win;
}
