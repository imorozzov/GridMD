/****************************************************************************
 * \file plinkshell.cpp
 * Implementation of the class gmShellPlink
****************************************************************************/

#include <gmd/utils.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>

#include "jobmngr/plinkshell.h"


gmShellPlink::gmShellPlink(pCSTR login, pCSTR host, pCSTR plink_args) :
  auth_defined(false)
{
  // Global parameters which could be read from XML
  InitParam("login", login);
  InitParam("host", host);
  InitParam("plink_args", plink_args);
  InitParam("plink_path", UNINITIALIZED_PARAM);
  InitParam("pscp_path", UNINITIALIZED_PARAM);
  InitParam("plink_att_num", "1");
  InitParam("plink_retry_delay", "3000");
}


void gmShellPlink::set_param(const gmdString& par_name, const gmdString& par_value){
  bool plink_cmd_changed = false, pscp_cmd_changed = false, auth_changed = false;

  if(par_name == "login" || par_name == "host") auth_changed = true;
  else if(par_name == "plink_att_num")
    plink_att_num = ParseIntParam(par_name, par_value, 1);
  else if(par_name == "plink_retry_delay")
    plink_retry_delay =  ParseIntParam(par_name, par_value, 0);
  else if(par_name == "plink_args") {
    plink_args = par_value;
    auth_changed = true;
  }
  else if(par_name == "plink_path") {
    plink_path = (par_value == UNINITIALIZED_PARAM) ? "" : par_value;
    if( plink_path.IsEmpty() ) {
      plink_path = "C:\\Program Files\\PuTTY\\plink.exe";
      if( !gmdFileExists(plink_path) ) {
        plink_path = "C:\\Program Files (x86)\\PuTTY\\plink.exe";
        if( !gmdFileExists(plink_path) && par_value != UNINITIALIZED_PARAM ) {
          LOGJOBERR("plink.exe is not found, use 'plink_path' to define the path!");
          plink_path = "plink.exe";
        }
      }
    }
    plink_cmd_changed = true;
  }
  else if(par_name == "pscp_path") {
    pscp_path = (par_value == UNINITIALIZED_PARAM) ? "" : par_value;
    if( pscp_path.IsEmpty() ) {
      pscp_path = "C:\\Program Files (x86)\\PuTTY\\pscp.exe";
      if( !gmdFileExists(pscp_path) ) {
        pscp_path = "C:\\Program Files\\PuTTY\\pscp.exe";
        if( !gmdFileExists(pscp_path) && par_value != UNINITIALIZED_PARAM ) {
          LOGJOBERR("pscp.exe is not found, use 'pscp_path' to define the path!");
          plink_path = "pscp.exe";
        }
      }
    }
    pscp_cmd_changed = true;
  }

  // Check authorization parameters
  if(auth_changed)
    auth_defined = !param["host"].IsEmpty() && !param["login"].IsEmpty() &&
      ( plink_args.Contains("-load") ||
        plink_args.Contains("-pw") ||
        plink_args.Contains("-i") );
  
  if(auth_defined) {
    // Create command line templates
    userhost = param["login"] + "@" + param["host"];
    if(plink_cmd_changed || auth_changed)
      plink_pre = "\"" + gmdString(plink_path) + "\" -batch -ssh " + plink_args + " "
        + (plink_args.Contains("-load") ? "" : userhost) + " \"";
    if(pscp_cmd_changed || auth_changed)
      pscp_pre  = "\"" + gmdString(pscp_path) + "\"  -batch -q " + plink_args + " ";
  }

  gmShell::set_param(par_name, par_value);
}


int gmShellPlink::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err){
  // Execute Shell command
  int i, res;
  gmdString errmsg;

  execute_begin(cmd, out, err);

  if(!auth_defined)
    return execute_error(
      set_err(CONNECTION_ERROR,
        "PLINK: Authentification method is not defined, "
        "check 'login', 'host' and 'plink_args' parameters!" )
      );

  // Screed double quotes
  gmdString cmd_scr(cmd);
  cmd_scr.Replace("\"", "\\\"");
  // Avoid empty command that switches plink into the interactive mode
  if( cmd_scr.IsEmpty() ) cmd_scr = "#";

  for(i=0; i<plink_att_num; i++){
    if(i) {  // Retrying on a network error
      LOGJOBMSG( fmt("PLINK connection failed: %s\nRetrying %d more times",
                     (const gmdChar*)errmsg, plink_att_num - i) );
      gmdMilliSleep(plink_retry_delay);  // delay before a retry
    }

    if(dump_commands)
      LOGJOBMSG4( (pCSTR)("------ execute (plink)------\n" + plink_pre + cmd_scr + '\"') );

    res = gmdExecute(plink_pre + cmd_scr + '\"', out, err);
    if(res == -1) {
      execute_end(res, out, err);
      return execute_error( set_err(EXECUTE_ERROR, "Error executing PLINK") );
    }
    else if(res == 0)
      break;

    errmsg = ArrayToString(err);
    if( !errmsg.StartsWith("Unable to open connection") ) break;
  }
  
  if(i >= plink_att_num) return set_err(CONNECTION_ERROR, errmsg);

  return execute_end(res, out, err);
}


int gmShellPlink::pscp_execute(const gmdString& args){
  int i, res;
  gmdString errmsg;

  if(!auth_defined)
    LOGJOBERR("Authentification method are not defined, "
      "check 'login', 'host' and 'plink_args' parameters!");

  if(dump_commands)
    LOGJOBMSG4( (pCSTR) ("------ pscp_execute ------\n"
                         " cmd=" + pscp_pre + args + "\n"
              ));

  for(i=0; i<plink_att_num; i++){

    if(i) {  // Retrying on a network error
      LOGJOBMSG( fmt("PLINK connection failed: %s\nRetrying %d more times",
                     (const gmdChar*)errmsg, plink_att_num - i) );
      gmdMilliSleep(plink_retry_delay);  // delay before a retry
    }

    gmdArrayString out, err;
    //ttimer.Resume();
    res = gmdExecute(pscp_pre + args, out, err);
    //ttimer.Pause();
    if(res == -1) 
      return( set_err(EXECUTE_ERROR, "Error executing PSCP!") );

    if(dump_commands && res != 0)
      LOGJOBMSG4( (pCSTR) ("------ pscp_execute (error)------\n"
                           " cout=" + ArrayToString(out)+"\n"
                           " cerr=" + ArrayToString(err)+"\n"
                ));


    if(res == 0) break;

    errmsg = ArrayToString(err);
    if( !errmsg.StartsWith("Fatal: Network error:") ) break;
  }
  
  if(res) { // Replace 'res' by an internal error code
    if(i >= plink_att_num) res = CONNECTION_ERROR;
    else if( ( errmsg.Contains("no such file or directory") ||
               errmsg.Contains("matched no files") ) &&
             !errmsg.Contains("unable to open")
           ) res = NO_INPUT_FILE;
    else res = COPY_ERROR;
    set_err(res, errmsg);
  }

  return res;
}


int gmShellPlink::StageIn(pCSTR locpath, pCSTR rempath, unsigned flags){
  gmdString tempdir, src;

  if( StageIn_begin(locpath, rempath, flags) )
    return StageIn_end(locpath, rempath, flags);

  if( !path_exists(locpath, flags) )
    set_err(NO_INPUT_FILE, fmt("%s: no such file or directory", locpath));
  else if(flags & TEXT){
    // Convert text file
    assert_no_recursive(flags);

    tempdir = GetTempDir(); // we cannot check if 'rempath' is dir or file name 
    gmdArrayString srclist;
    srclist.Add(locpath);
    conv_files(srclist, tempdir, dos2unix);

    gmdFileName filename(locpath);
    src = tempdir + "\\" + filename.GetFullName();
  }
  else
    src = locpath;

  // Copy file
  if(!error_code) {
    gmdString args = (flags & RECURSIVE) ? "-r \"" : "\"";
    args += src + "\" \"" + userhost + ':' + rem_path_subst(rempath) + "\"";
    pscp_execute(args);
  }

  if( !tempdir.IsEmpty() ) remove_local(tempdir);

  return StageIn_end(locpath, rempath, flags);
}


int gmShellPlink::StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags){
  gmdString tempdir, src, err;
  gmdArrayString srclist;

  if( StageIn_begin(NULL, remdir, flags) )
    return StageIn_end(NULL, remdir, flags);

  for(unsigned i=0; i<locfiles.GetCount(); i++) {
    gmdString lfile = locfiles[i];
    if( lfile.IsEmpty() ) continue;
    lfile.Replace("/", "\\");

    if( has_wildcards(lfile) ) {
      // List all files explicitly if lfile contains wildcards
      gmdArrayString filelist;
      unsigned nfiles = files_by_mask(lfile, filelist, flags);
      if(nfiles)
        for(unsigned k=0; k<nfiles; k++) srclist.Add(filelist[k]);
      else
        set_err(NO_INPUT_FILE, fmt("%s: no such files", (pCSTR)lfile.c_str()));
    }
    else if( path_exists(lfile, flags) )
      srclist.Add(lfile);
    else
      set_err(NO_INPUT_FILE, fmt("%s: no sich file or directory", (pCSTR)lfile.c_str()));
  }

  if( !srclist.IsEmpty() ) {
    if(flags & TEXT) {
      // Convert text files and store them in the temporary dir
      assert_no_recursive(flags);  // combination RECURSIVE & TEXT is not supported!
      tempdir = GetTempDir();
      //dos2unix(srclist, tempdir);
      conv_files(srclist, tempdir, dos2unix);
      src = tempdir + "\\*.*";
    } else {
      // prepare the space separated file list for PLINK
      for(unsigned i=0; i<srclist.GetCount(); i++)
        src += (i>0 ? "\" \"" : "") + srclist[i];
    }

    // Copy files
    if(!error_code) {
      gmdString args = (flags & RECURSIVE) ? "-r \"" : "\"";
      args += src + "\" \"" + userhost + ':' + rem_path_subst(remdir) + "\"";
      pscp_execute(args);
    }

    if( !tempdir.IsEmpty() ) remove_local(tempdir);

    if(!error_code && (flags & MOVE))  // custom move (not using StageIn_end)
      for(unsigned i=0; i<srclist.GetCount(); i++) RemoveLocal(srclist[i], flags);
  }

  return StageIn_end(NULL, remdir, flags);
}


int gmShellPlink::StageOut(pCSTR locpath, pCSTR rempath, unsigned flags) {
  gmdString dst, tempdir, err;

  if( StageOut_begin(locpath, rempath, flags) )
    return StageOut_end(locpath, rempath, flags);

  bool locpath_is_file = !gmdDirExists(locpath);

  if(locpath_is_file) assert_no_recursive(flags);

  if(flags & TEXT) {
    assert_no_recursive(flags);
    tempdir = GetTempDir();
    dst = tempdir;
    if(locpath_is_file) {
      gmdFileName filename(locpath);
      dst += "\\" + filename.GetFullName();
    }
  }
  else
    dst = locpath;

  gmdString rpath( rem_path_subst(rempath) );
  gmdString args = (flags & RECURSIVE) ? "-r " : "";
  args += "\"" + userhost + ':' + rpath + "\" \"" + dst + "\"";
  pscp_execute(args);

  if(flags & TEXT) {  // Convert text files
    if(!error_code) {
      gmdArrayString filelist;
      files_by_mask(tempdir + "\\*.*", filelist);

      if(locpath_is_file && filelist.GetCount() != 1)
        set_err(COPY_ERROR,
          fmt("remote path %s is not a regular file", (pCSTR)rpath.c_str()));
      else
        conv_files(filelist, locpath, unix2dos);
    }

    remove_local(tempdir);  // Remove temporary file
  }

  return StageOut_end(locpath, rpath, flags);
}
