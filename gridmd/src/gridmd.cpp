/*s***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2005-2010        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD, ivutils
 *
 *   This source code is Free Software and distributed under the terms of wxWidgets license (www.wxwidgets.org) 
 *
 *   $Revision: 1.50 $
 *   $Date: 2016/02/08 08:16:59 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/src/gridmd.cpp,v 1.50 2016/02/08 08:16:59 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/src/gridmd.cpp,v $
$Revision: 1.50 $
$Author: valuev $
$Date: 2016/02/08 08:16:59 $
*/
/*s****************************************************************************
 * $Log: gridmd.cpp,v $
 * Revision 1.50  2016/02/08 08:16:59  valuev
 * working examples
 *
 * Revision 1.49  2016/02/05 19:04:45  valuev
 * implicit workflow fixes
 *
 * Revision 1.48  2016/02/04 16:41:04  valuev
 * serial final job execution
 *
 * Revision 1.47  2016/02/03 16:41:18  valuev
 * fixes
 *
 * Revision 1.46  2016/02/02 12:15:35  valuev
 * fixing mutex
 *
 * Revision 1.45  2015/06/17 19:37:59  valuev
 * sweep farm restructured
 *
 * Revision 1.44  2015/05/12 13:53:45  valuev
 * new version with all functions in read node
 *
 * Revision 1.43  2015/03/30 11:50:07  valuev
 * workinng version of sweep farm
 *
 * Revision 1.42  2015/03/27 18:23:15  valuev
 * edges
 *
 * Revision 1.41  2015/03/20 15:46:36  valuev
 * node name changes
 *
 * Revision 1.40  2015/03/20 07:51:30  valuev
 * reading data
 *
 * Revision 1.39  2015/03/18 13:03:05  valuev
 * added data write
 *
 * Revision 1.38  2015/03/17 13:51:36  valuev
 * some fixes for scheduling
 *
 * Revision 1.37  2015/02/02 11:49:52  valuev
 * iccs2015
 *
 * Revision 1.36  2015/01/30 09:08:04  valuev
 * working on sweeps
 *
 * Revision 1.35  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.34  2013/07/04 14:06:24  valuev
 * extended file management
 *
 * Revision 1.33  2013/05/28 12:20:06  valuev
 * wxWidgets compatibility fixes
 *
 * Revision 1.32  2013/05/27 17:00:35  valuev
 * added conversion to const char * for wxWidgets UNICODE
 *
 * Revision 1.31  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.30  2013/03/07 17:08:59  valuev
 * kintech xml
 *
 * Revision 1.29  2013/02/26 16:44:19  valuev
 * changed job manager parameters interface
 *
 * Revision 1.28  2012/12/28 17:14:30  valuev
 * added node state callbacks, fixed restart
 *
 * Revision 1.27  2012/12/24 17:01:58  valuev
 * fixed local link check
 *
 * Revision 1.26  2012/12/21 16:23:05  valuev
 * unified node states
 *
 * Revision 1.25  2012/12/14 17:34:47  valuev
 * fixed restart
 *
 * Revision 1.24  2012/12/12 17:28:00  valuev
 * added virtual nodes and dummy jobs
 *
 * Revision 1.23  2012/12/11 17:24:36  valuev
 * updated scheduling, node state changes on the fly
 *
 * Revision 1.22  2012/12/04 17:10:31  valuev
 * added windows job manager, implemented fetching workdir files, fixed file renaming after transfer
 *
 * Revision 1.21  2012/11/29 18:14:48  valuev
 * sync with svn
 *
 * Revision 1.20  2012/11/22 17:01:06  valuev
 * sync with kintech svn
 *
 * Revision 1.19  2012/11/21 17:06:15  valuev
 * sync with kintech svn
 *
 * Revision 1.18  2012/11/08 14:32:50  valuev
 * various extensions
 *
 * Revision 1.17  2012/11/07 22:13:54  valuev
 * grouped nodes
 *
 * Revision 1.16  2012/11/06 18:05:43  valuev
 * various small fixes, sync with svn
 *
 * Revision 1.15  2012/11/06 11:35:41  valuev
 * added resource per node specification
 *
 * Revision 1.14  2012/10/30 11:39:58  valuev
 * modified queueing (working)
 *
 * Revision 1.13  2012/10/25 14:15:04  valuev
 * updated explicit workflow
 *
 * Revision 1.12  2012/10/24 17:05:00  valuev
 * working explicit workflow and example
 *
 * Revision 1.11  2012/10/23 17:04:33  valuev
 * adding node actions
 *
 * Revision 1.10  2012/10/23 10:44:56  valuev
 * node action
 *
 * Revision 1.9  2012/09/07 14:26:06  valuev
 * removed using namespace from h files
 *
 * Revision 1.8  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 * Revision 1.7  2010/10/14 09:52:49  valuev
 * added link data analysis and node restart on failed links
 *
 * Revision 1.6  2010/10/12 17:45:47  valuev
 * added documentation, some cleanup
 *
 * Revision 1.5  2010/10/07 16:41:03  morozov
 * Fixed Unix compatibility issues for release 2.1
 *
 * Revision 1.4  2010/10/01 07:01:04  valuev
 * added xml resource saving
 *
 * Revision 1.3  2010/09/15 18:12:25  valuev
 * moved misplaced counter
 *
 * Revision 1.2  2010/09/15 17:54:15  valuev
 * added fork skeleton
 *
 * Revision 1.1  2010/08/04 16:07:28  valuev
 * added forgotten file
 *
 *
*******************************************************************************/

#include <gmd/filefn.h>
#include <gmdefs.h>

# include "gridmd.h"
# include "workflow.h"
# include "gmsched.h"

# include "jobmngr/jobmngr.h"
# include "jobmngr/pbsmngr.h"
# include "jobmngr/bshmngr.h"
# include "jobmngr/slurmmngr.h"

# include "jobmngr/gmshell.h"

#ifdef _WIN32
# include "jobmngr/plinkshell.h"
# include "jobmngr/winshmngr.h"
# include "jobmngr/winapimngr.h"
# include "jobmngr/mingwshell.h"
# include "jobmngr/winshell.h"
#else
# include "jobmngr/unixshell.h"
# include "jobmngr/unixsshell.h"
#endif
#ifdef USING_LIBSSH
# include "jobmngr/libsshell.h"
#endif
#ifdef USING_LIBSSH2
#include "jobmngr/libssh2shell.h"
#endif


#include <boost/foreach.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/static_assert.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/lexicographical_compare.hpp>
#include <functional>
#include <algorithm>


using namespace std;
using namespace boost;

#ifdef _WIN32
#define WIN32_ONLY( ARG )  , ARG
#define LINUX_ONLY( ARG )
#else
#define WIN32_ONLY( ARG )
#define LINUX_ONLY( ARG )  , ARG
#endif



const char* gmResTypeName[]= { "pbs", "slurm", "bsh" WIN32_ONLY("wsh") WIN32_ONLY("win") };
const char* gmShellTypeName[]= { "" LINUX_ONLY("unix") LINUX_ONLY("ssh")
                                 WIN32_ONLY("plink") WIN32_ONLY("mingw") WIN32_ONLY("cmd")
                               #ifdef USING_LIBSSH
                                 , "libssh"
                               #endif
                               #ifdef USING_LIBSSH2
                                 , "libssh2"
                               #endif
                               };

BOOST_STATIC_ASSERT( sizeof(gmResTypeName)/sizeof(gmResTypeName[0])     == gmRES_TYPES_SIZE   );
BOOST_STATIC_ASSERT( sizeof(gmShellTypeName)/sizeof(gmShellTypeName[0]) == gmSHELL_TYPES_SIZE );



string gmGetStateName(gmNODE_STATES state){
  switch(state){
    case gmNS_NO_NODE:
      return "NO_NODE";
    case gmNS_UNPROC:
      return "UNPROC";
    case gmNS_PROC:
      return "PROC";
    case gmNS_BLOCK:
      return "BLOCK";  
    case gmNS_WAIT:
      return "WAIT";
    case gmNS_FAIL:
      return "FAIL";
    case gmNS_EXE:
      return "EXE";
    case gmNS_TARG:
      return "TARG";
    case gmNS_TEMP:
      return "TMP";
    case gmNS_SCHED:
      return "SCHED";
  }
  return "OTHER";
}


gmNODE_STATES gmGetStateId(const string &state){
  if(state=="UNPROC")
    return gmNS_UNPROC;
  else if(state == "PROC")
    return gmNS_PROC;
  else if(state == "BLOCK")
    return gmNS_BLOCK;
  else if(state == "WAIT")
    return gmNS_WAIT;
  else if(state == "FAIL")
    return gmNS_FAIL;
  else if(state == "EXE")
    return gmNS_EXE;
  else if(state == "TARG")
    return gmNS_TARG;
  else if(state == "TMP")
    return gmNS_TEMP;
  else if(state == "SCHED")
    return gmNS_SCHED;
  return gmNS_NO_NODE;
}

#if !USING_GMTHREADS
gmManager gmExperiment;
#else
gmManager gmExpObj;
gmManRedirector gmExpRedirector(&gmExpObj);

#endif



bool gmShellDescr::operator==(const gmShellDescr &other) const {
  return 
    shell_type == other.shell_type &&
    host       == other.host       &&
    login      == other.login      &&
    get_const_map_el(param, "home_dir"   ) == get_const_map_el(other.param, "home_dir"   ) &&
    get_const_map_el(param, "rem_tmp_dir") == get_const_map_el(other.param, "rem_tmp_dir");
}

bool gmShellDescr::operator < (const gmShellDescr &other) const {

    if( shell_type != other.shell_type ) return shell_type < other.shell_type;
    if( host       != other.host       ) return host       < other.host      ;
    if( login      != other.login      ) return login      < other.login     ;

    if(get_const_map_el(param, "home_dir"   ) != get_const_map_el(other.param, "home_dir"   ) )
        return get_const_map_el(param, "home_dir"   ) < get_const_map_el(other.param, "home_dir"   );

    if(get_const_map_el(param, "rem_tmp_dir") != get_const_map_el(other.param, "rem_tmp_dir") )
        return get_const_map_el(param, "rem_tmp_dir") < get_const_map_el(other.param, "rem_tmp_dir");

    return false;
}


gmShell *gmShellDescr::CreateShell() const {
  // Create shell of a given type
  gmShell *shell = NULL;
#ifdef _WIN32
  if(shell_type==gmSHELL_PLINK)
    shell= new gmShellPlink((const char *)login.c_str(), (const char *)host.c_str());
  else if(shell_type==gmSHELL_MINGW)
    shell= new gmShellMinGW;
  else if(shell_type==gmSHELL_WIN)
    shell= new gmShellWin;
#else
  if(shell_type==gmSHELL_UNIX)
    shell= new gmShellUnix;
  else if(shell_type==gmSSHELL_UNIX)
    shell= new gmSShellUnix((const char *)login.c_str(), (const char *)host.c_str());
#endif
#ifdef USING_LIBSSH
  else if(shell_type==gmSHELL_LIBSSH)
    shell= new gmShellLibssh((const char *)login.c_str(), (const char *)host.c_str());
#endif
#ifdef USING_LIBSSH2
  else if(shell_type==gmSSHELL_LIBSSH2)
    shell= new gmShellLibssh2((const char *)login.c_str(), (const char *)host.c_str());
#endif


  else  // add other shell types here
    LOGERR(-2,fmt("gmShellDescr.CreateShell: unsupported shell type %d",shell_type),0);

  // Set custom shell parameters
  if(shell)
    for(gmExtraParHash::const_iterator it = param.begin(); it != param.end(); ++it)
      shell->SetParam(it->first, it->second);

  return shell;
}

void gmResourceDescr::init(const int res_type_, const int shell_type_, int active_) {
  active = active_;

  session.shell_type = shell_type_;
  session.host.Clear();
  session.login.Clear();
  session.param.clear();

  res_type = res_type_;
  prefix.Clear();
  postfix.Clear();
  progdir.Clear();
  progname.Clear();
  param.clear();

	timing = false;
}

# ifndef NO_XML

int gmResourceDescr::Save(XMLFile& xmldoc, xmlNodePtr parent, const gmdString &name){
  xmlNodePtr resource;

  // Node <resource>
  resource = xmldoc.addElement(parent, "resource");
  xmldoc.addAttribute(resource, "name", name=="" ? WXSTRING_ADAPTER(session.login+"@"+session.host) : WXSTRING_ADAPTER(name));
  xmldoc.addAttribute(resource, "active", fmt("%d",active));
  
  // Node <session>
  xmlNodePtr node = xmldoc.addElement(resource, "session");
  xmldoc.addAttribute(node, "type", gmShellTypeName[session.shell_type]);
  if(!session.host.IsEmpty()) xmldoc.addAttribute(node, "host", WXSTRING_ADAPTER(session.host));
  if(!session.login.IsEmpty()) xmldoc.addAttribute(node, "login", WXSTRING_ADAPTER(session.login));
  
  FOR_EACH_LOOP(gmExtraParHash, session.param, it)
    xmldoc.addElement(node, (const char *)it->first.c_str(), (const char *)it->second.c_str());

  // Node <job_manager>
  node = xmldoc.addElement(resource, "job_manager");
  xmldoc.addAttribute(node, "type", gmResTypeName[res_type]);
	xmldoc.addAttribute(node, "timing", timing? "yes": "no" );

  FOR_EACH_LOOP(gmExtraParHash, param, it)
    xmldoc.addElement(node, (const char *)it->first.c_str(), (const char *)it->second.c_str());

  // Node <application>
  node = xmldoc.addElement(resource, "application");
  if(!progname.IsEmpty()) xmldoc.addAttribute(node, "name", WXSTRING_ADAPTER(progname));

  if(!postfix.IsEmpty()) xmldoc.addElement(node, "postfix", (std::string)postfix);
  if(!prefix.IsEmpty()) xmldoc.addElement(node, "prefix", (std::string)prefix);
  if(!progdir.IsEmpty()) xmldoc.addElement(node, "progdir", (std::string)progdir);

  return 1;
}


int gmResourceDescr::Load(XMLFile& xmldoc, xmlNodePtr parent, gmdString &name){

  gmdString str;
  std::vector<xmlNodePtr> pars;

  init();

  name = xmldoc.parseOptionalAttribute(parent, "name", "");
  active = xmldoc.parseOptionalAttribute(parent, "active", 1);

  // <session> element
  xmlNodePtr node = xmldoc.getChildByName(parent, "session", false);
  if(!node)
    return LOGERR(-5,fmt("gmResourceDescr.Load: no <session> definition found for resource named '%s', resource skipped",(const char *)name.c_str()),0);

	// timing
  str = xmldoc.parseOptionalAttribute(node, "timing", "no");
  if(str == "yes")
		timing = true;
	else
		timing = false;

  // Get shell type
  int ival=gmSHELL_TYPES_SIZE;
  str = xmldoc.parseOptionalAttribute(node, "type", "");
  if(str == "")
    return LOGERR(-3,fmt("gmResourceDescr.Load: can't read session type from resource named '%s'",(const char *)name.c_str()),0);



  for(int i=0;i<gmSHELL_TYPES_SIZE;i++){
    if(str==gmShellTypeName[i]){
      ival=i;break;
    }
  }
  if(ival>=gmSHELL_TYPES_SIZE)
    return LOGERR(-4,fmt("gmResourceDescr.Load: unknown shell type '%s' for resource named '%s'",(const char *)str.c_str(),(const char *)name.c_str()),0);
  session.shell_type=ival;

  // Get shell parameters
  session.login = xmldoc.parseOptionalAttribute(node, "login", "");
  session.host = xmldoc.parseOptionalAttribute(node, "host", "");
  
  pars = xmldoc.getAllChildren(node);
  FOR_EACH_LOOP(std::vector<xmlNodePtr>, pars, par)
    if((*par)->type == XML_ELEMENT_NODE)
      session.param[xmldoc.getNodeName(*par)] = xmldoc.parseNode(*par);

  // <job_manager> element
  node = xmldoc.getChildByName(parent, "job_manager", false);
  if(!node)
    return LOGERR(-5,fmt("gmResourceDescr.Load: no <job_manager> definition found for resource named '%s', resource skipped",(const char *)name.c_str()),0);

  // Get job manager type
  ival=gmRES_TYPES_SIZE;
  str = xmldoc.parseOptionalAttribute(node, "type", "");
  if(str == "")
    return LOGERR(-1,fmt("gmResourceDescr.Load: can't read resource type from resource named '%s'",(const char *)name.c_str()),0);
  for(int i=0;i<gmRES_TYPES_SIZE;i++){
    if(str==gmResTypeName[i]){
      ival=i;break;
    }
  }
  if(ival>=gmRES_TYPES_SIZE)
    return LOGERR(-2,fmt("gmResourceDescr.Load: unknown resource type '%s' for resource named '%s'",(const char *)str.c_str(),(const char *)name.c_str()),0);
  res_type=ival;

  // Get job manager parameters
  pars = xmldoc.getAllChildren(node);
  FOR_EACH_LOOP(std::vector<xmlNodePtr>, pars, par)
    if((*par)->type == XML_ELEMENT_NODE)
      param[xmldoc.getNodeName(*par)] = xmldoc.parseNode(*par);

  // <application> element
  node = xmldoc.getChildByName(parent, "application", false);
  if(node) {
    // Get application parameters
    progname = xmldoc.parseOptionalAttribute(node, "name", "");

    xmlNodePtr par = xmldoc.getChildByName(node, "progdir", false);
    if(par) progdir = xmldoc.parseNode(par);
    
    par = xmldoc.getChildByName(node, "prefix", false);
    if(par) prefix = xmldoc.parseNode(par);
    
    par = xmldoc.getChildByName(node, "postfix", false);
    if(par) postfix = xmldoc.parseNode(par);
  }

  return 1;
}

# endif // NO_XML

gmJobManager * gmResourceDescr::CreateJobManager(gmShell *shell) const {
  // Create shell
  if(!shell)
    shell= session.CreateShell();
  if(!shell)
    return NULL;

  // Create job manager of a given type
  gmJobManager *jm=NULL;

  switch((gmRESOURCE_TYPES) res_type)
  {
    case gmRES_PBS   : jm= new gmPBSManager    (*shell); break;
    case gmRES_SLURM : jm= new gmSLURMManager  (*shell); break;
    case gmRES_BSH   : jm= new gmBShManager    (*shell); break;
#ifdef _WIN32
    case gmRES_WSH   : jm= new gmWinShManager  (*shell); break;
    case gmRES_WINAPI: jm= new gmWinAPIManager (*shell); break;
#endif
    default:
        LOGERR(-3,fmt("gmResourceDescr.CreateJobManager: unsupported resource type %d",res_type),0);
        return NULL;
  }

  assert(jm);

  // Set custom job manager parameters
  for(gmExtraParHash::const_iterator it = param.begin(); it != param.end(); ++it)
    jm->SetParam(it->first, it->second);

	if(timing)
		jm->set_log_level(gmJobManager::LOG_TIMING);

  return jm;
}

bool gmResourceDescr::operator==(const gmResourceDescr &other) const
{
    return      session     == other.session
            &&  res_type    == other.res_type
            &&  prefix      == other.prefix
            &&  postfix     == other.postfix
            &&  progdir     == other.progdir
            &&  progname    == other.progname
            &&  active      == other.active
            &&  boost::range::equal(param, other.param)
            //&&  param.       == other.param
            ;

}

bool gmResourceDescr::operator< (const gmResourceDescr &other) const
{
    if( ! ( session   == other.session  ) ) return session   < other.session    ;
    if( ! ( res_type  == other.res_type ) ) return res_type  < other.res_type   ;
    if( ! ( prefix    == other.prefix   ) ) return prefix    < other.prefix     ;
    if( ! ( postfix   == other.postfix  ) ) return postfix   < other.postfix    ;
    if( ! ( progdir   == other.progdir  ) ) return progdir   < other.progdir    ;
    if( ! ( progname  == other.progname ) ) return progname  < other.progname   ;
    if( ! ( active    == other.active   ) ) return active    < other.active     ;

    if( ! boost::range::equal(param, other.param) )
        return boost::range::lexicographical_compare(param, other.param);

    return false;
}



class gmVoidMutex : public gmMutex {
public:
    virtual void lock() {}
    virtual void unlock() {}
} voidMutexItem;
//gmMutex& gmMutex::voidMutex=voidMutexItem;  /// gmMutex& gmMutex::voidMutex=voidMutexItem

gmMutex gmMutex::voidMutex;
gmManagerCallback gmManagerCallback::voidCallBack;


class gmScopeLock
{
    gmMutex* const mutex;
public:
    gmScopeLock(gmMutex* mutex) : mutex(mutex) {
        mutex->lock();
    }
    ~gmScopeLock() {
        mutex->unlock();
    }
};

int gmManager::rec_level = 0;

int gmManager::init(const string &name_, bool restart, bool recalc_locals, const string &restart_file_){
  gmScopeLock scopeLock(mutex);
  if(mode==gmMODE_LOCAL && rec_level==0){
    _argc=1;
    _argv=NULL;
    name=name_;
    if(restart){
      full_restart = recalc_locals ? 1 : 0;
      if(restart_file_=="")
        restart_file=name+".xml";
      else
        restart_file=restart_file_;
    }
    else
      restart_file="";
    return 1;
  }
  return 0;
}

int gmManager::init(int &argc, charDoublePtr &argv, const string &name_){
  gmManager *substitute = thread_redirect(this);
  if(substitute)
    return substitute->init(argc,argv,name_);

  gmScopeLock scopeLock(mutex);
  if(mode==gmMODE_LOCAL && rec_level==0){
    _argc=argc;
    _argv=argv;
  }
  
  // clearing idcount
  //idcount=0;
  // parsing command line
  for(int i=1;i<argc;i++){
    if(argv[i][0]=='-'){
      if(argv[i][1]=='w'){ // worker mode
        mode=gmMODE_WORKER;
        if(rec_level==0)
          start_as_worker=1;
        wnodes.clear();
        add_ranges(wnodes,argv[i]+2,0,1000000,true,":-");
        if(!wnodes.size())
          return LOGERR(-1,fmt("gmManager.init: invalid list of nodes for worker mode ('%s')!",argv[i]+2),0); 
        // calculating max_node
        max_node=-1;
        for(int_pack::iterator it=wnodes.begin();it!=wnodes.end();++it){
          if(max_node<*it)
            max_node=*it;
        }
        idcount=0; // clearing idcount
        // clearing this entry
        for(int j=i;j<argc-1;j++)
          argv[j]=argv[j+1];
        argc--;
        break;
      }
      else if((argv[i][1]=='r' || argv[i][1]=='R')  && mode==gmMODE_LOCAL){ // restart
        if(argv[i][1]=='R')
          full_restart=1;
        restart_file=argv[i]+2;
        if(restart_file==""){
          if(name_!="")
            restart_file=name_+".xml";
          else if(name!="")
            restart_file=name+".xml";
        }
        // clearing this entry
        for(int j=i;j<argc-1;j++)
          argv[j]=argv[j+1];
        argc--;
        i--;
      }
    }
  }
  if(mode&gmMODE_CONSTRUCT){ // still in construction mode
    idcount=0;
  }
  if(mode==gmMODE_LOCAL){
    if(name_!="")
      name=name_;
  }
  return 1;
}


gmManager::gmManager(int smode, gmMutex *mutex, gmManagerCallback *call_back)
:mode(smode),mutex(mutex), call_back(call_back){
  _init_params();
}

void gmManager::_init_params(){
  idcount=0;
  dprev=1;
  max_node=-1;
  data_rep=1;
  last_loaded_event= -1;
  distr_node=-1;
  set_execution(gmEXE_LOCAL);
  write_files=0;
  del_files=0;
  graph= new gmGraph;
  graph->set_callback(call_back);
  graph_error=0;
  max_node_restarts=2;
  graph_update=2;
  construct_only=0;
  start_as_worker=0;
  set_graphviz_flags(gmGV_ALL);
  name="gmexp";
  sched = new gmScheduler(this);
  full_restart=0;
  start_distr=0;
  rec_level=0;
  tmpstate=gmNS_NO_NODE;
  _argv = NULL;
  fetch_on_fail = false;
  nfailed = 0;
  nstarted = 0;
  nfinished = 0;
  entered_exec = false;

  have_start_node = -1;
}



gmManager::~gmManager(){
  if((write_files&gmFILES_CLEANUP)){
    link_cleanup();
    //gmdRmdir(sched->workdir+"/"+name.c_str()+".tmp");  // dangerous
  }
  delete graph;
  delete sched;
}


int gmManager::write_graph(){
  gmScopeLock scopeLock(mutex);
  // writing graphvis file
  return graph->write((const char *)string(name+".dot").c_str(),wr_flags);
}

void gmManager::updated_status()
{
  gmScopeLock scopeLock(mutex);
  if(graph_update>=1) // writing the graphwiz file
    write_graph();
  call_back->StatusUpdated();
  // writing XML file
  gmdString xmlname=name+".xml";
  if(restart_file!=xmlname) // protect restart file from being overwritten
    save_state((const char *)xmlname.c_str());
}

int gmManager::mark_distributed(){

  if(have_start_node<0){
    if(have_start_node == -1){ // the very first distributed section
      if(mode&gmMODE_WORKER){
        have_start_node = idcount;
        return process_cur_node(); // in case we have something associated with 'start' node;
      }
      else{ // actually adding nodes only when constructing
        gmNode *start = new gmNode("start");
        graph->root=graph->add_node(start);
        
        if(!(exetype&gmEXE_EXPLICIT_ONLY))
          start->implicit_action = true; // may have implicit actions
        else{ // there may be implicit calls
          if(rec_level>0 && have_start_node>=0){ // this means that the graph is not inherited by recursion 
                                               // (now rec_level is static!)
            LOGMSG(vblWARN,"gmManager::mark_distributed: recursively entering graph construction without initialized gmManager instance!\n"
                           "                             Note: gmManager instance should be global to use it in implicit calls!\n",0);
          }
        }  
        have_start_node = idcount; //=0
      }
    }
    else{
      have_start_node = 0; // temporary
      int res = mark_node("start",gmNODE_PREV,gmStatusLink());
      have_start_node = idcount;
      if(mode&gmMODE_WORKER)
        return res;
    }
  }
  if(mode&gmMODE_WORKER)
    return 0;
  if((write_files&gmFILES_CLEANUP))
    link_cleanup();
  // check mode
  if(exetype==gmEXE_SERIAL){ // all tasks performed at once
    if(!construct_only)
      mode=gmMODE_CONSTRUCT|gmMODE_WORKER;
    else
      mode=gmMODE_CONSTRUCT;
    updated_status();
    start_distr=1;
    return 1;
  }
  
  if(mode&gmMODE_CONSTRUCT){
    start_distr=1;
    updated_status();
    return 0;
  }
  // we are in the local mode
  if(graph_error)
    LOGMSG(vblWARN,fmt("gmManager.mark_distributed: clearing the error status '%d' of the previous subgraph execution!", graph_error),0);
  graph_error=0;
  
  // go to construction mode
  mode|=gmMODE_CONSTRUCT;
  
  if(exetype&gmEXE_EXPLICIT_ONLY)  // executes nodes only as external routines/ callbacks
    return 0;

  // setting maximal construction node
  distr_node=idcount;
  int have_start_node_tmp = have_start_node;

  // HERE RECURSION STARTS
  rec_level++;
  try{
    gridmd_main(_argc,_argv);
  }
  catch(task_finished){}
  rec_level--;

 
 
  // switching to final worker mode to finish local tasks
  // (swith back in execute())
  mode&=~gmMODE_CONSTRUCT;
  mode|=gmMODE_WORKER;
  mode|=gmMODE_FINAL;
  
  if(graph_error){ 
     LOGMSG(vblWARN,fmt("gmManager.mark_distributed: execution of the subgraph failed with the status '%d', skipping the failed subgraph", graph_error),0);
     //max_node=-1; // don't break from worker mode
     //wnodes.clear(); // wnodes is empty in this case
     // here change the status of the remaining threads
  }

  // restoring
  idcount = distr_node;
  have_start_node = have_start_node_tmp;

  return 1; // go on in worker mode
}


gmNodeID gmManager::advance_nodeid(){
  if(have_start_node==-1)
    return 0; 
  //if(graph->root<0)
    //return 0;
  //gmNodeID tmp=idcount;
  idcount++;
  return idcount;
}


int gmManager::execute(bool wait, int cleanup, int max_schedules){
  if(mode&gmMODE_CONSTRUCT){
    if(check_distrnode()){ // checking that we are above distrnode and no error happened before
      //end_node=graph->add_node(new sysGraphNode("finish",1));
      //link<void>(gmNODE_NOOUTPUTS,end_node);
      end_node=get_curnode(); //!!
      idcount=end_node;
      updated_status();
    }
    else{
      //advance_nodeid();
      return 0; // we are in different branch
    }
    if(construct_only){ // end of construction, no execution needed
      mode=gmMODE_LOCAL; // returning to local mode
      return 0;
    }
  }
  //else end_node=advance_nodeid();
  else end_node=get_curnode(); //!!

  if(mode&gmMODE_WORKER){
    if(max_node<0){ // returning from the skipped subgraph or from serial pass
      mode=gmMODE_LOCAL; 
      if(exetype==gmEXE_SERIAL)
        return 1; // serial pass
      else
        return 0; // skipped subgraph
    }

    // make_subgraph_from_all_pending
    int res=check_subgraph(end_node);
    if(res){ // end_node reached, switching to local mode
      check_jobs(); // marking just finished local nodes as processed, clering threads
      mode=gmMODE_LOCAL;
      if(start_as_worker) 
        exit(0); // should never arrive here since the end node is local
      updated_status();
      if(write_files&gmFILES_CLEANUP)
        link_cleanup();
    }
    return res;
  }
   
  
  //mode&=~gmMODE_CONSTRUCT;
	unset_mode(gmMODE_CONSTRUCT);
  entered_exec = true;
 
  // applying graph algorithm
  if(!graph->pending_threads_count()) // had no threads before 
    graph->process_start();
  
  nfailed=0;
  nstarted=0;
  nfinished=0;

  bool continue_exec = check_restart_file(&nfailed);
  updated_status();

  bool need_more = false;
  bool wait_break= false;

  if(continue_exec){
    
    if(graph->pending_threads_count()) // had threads before 
      check_jobs(&nfailed);

		int iter = 0;
    while( !wait_break && (need_more = create_jobs(graph_error)) ){
      updated_status();
			
     /*
     9.  Execute the worker tasks of step 7, collect all
        files corresponding to outgoing data links of the subgraphs' nodes
        (or put these tasks in the submission script). Wait until at least one of
        the subgraph tasks finishes.*/
      if(!queue_jobs())
        continue; // no actively running jobs remain
      while(!check_jobs(&nfailed)){ // check the status of running jobs until at least one is finished 
        updated_status();
        if(!wait){
          wait_break=true;
          break;
        }
        // do something useful
        //sleep(200);
      }
			iter++;
			if(max_schedules>0 && iter>=max_schedules) 
				break;
    }
    updated_status();
  }
  //mode&=~gmMODE_WORKER; // needed for local mode
	//mode|=gmMODE_CONSTRUCT;
	unset_mode(gmMODE_WORKER); // needed for local mode
	set_mode(gmMODE_CONSTRUCT);
  entered_exec = false;

  if(continue_exec){ // cleanups
    if(cleanup==-1){ // auto
      if(wait)
        cleanup=1;
      else
        cleanup=0;
    }
    if(cleanup==1){
      if(clean_jobs())
        updated_status();
    }
  }

  // this point will  be reached in:
  // 1. case of graph deadlock since queue_jobs
  // for 'finish' node passes local job to the local gridmd_main
  // and this instance of gridmd_main throws task_finished from queue_jobs
  // (graph_error=1)
  // 2. all graph nodes are processed (nothing to do, graph_error=0) 
  if(!(exetype&gmEXE_EXPLICIT_ONLY))  // no need to throw if we are in explicit mode
    throw task_finished(); 
     
  if(nfailed)
    return -((int)nfailed);
  return need_more ? (job_state_changed() ? 1: 2 ) : 0;
}


bool gmManager::queue_jobs(){
  LOGMSG(vblMESS3,"Queuing jobs:",0);
  size_t nrunning=sched->queue_jobs(graph,exetype);
  updated_status();

  /*for(size_t i=0;i<threads.size();i++){
    printf("thread%d: \nnodes:",i);
    for(size_t j=0;j<threads[i].nodes.size();j++)
      printf("%d ",graph->g[threads[i].nodes[j]].nodeid);
    printf("\ninput:");
    for(size_t j=0;j<threads[i].idlinks.size();j++)
      printf("%d ",threads[i].idlinks[j]);
    printf("\noutput:");
    for(size_t j=0;j<threads[i].odlinks.size();j++)
      printf("%d ",threads[i].odlinks[j]);
    printf("\n");
  }*/
  return nrunning >0 ? true : false;
}


bool gmManager::check_jobs(size_t *nfailed){
  size_t nfailed_now;
  size_t nfinished_now =sched->check_jobs(graph,&nfailed_now,-1, false, fetch_on_fail);
  updated_status();
  if(nfinished_now && (write_files&gmFILES_CLEANUP))
    link_cleanup();
  nfinished+= nfinished_now;
  nfailed+= nfailed_now;
  return nfinished_now ? true : false;
}


bool gmManager::create_jobs(int &graph_error){
  bool need_more = graph->process_step(graph_error);
  size_t new_threads = graph->get_new_threads_count();
  //if(new_treads)
    //updated_status();
  nstarted+= new_threads; 
  return need_more;
}

int gmManager::_mark_node(const string &name){
  int tmode = get_mode();
  if(mode==gmMODE_LOCAL && rec_level==0){ 
    if(entered_exec)
      mode|=gmMODE_CONSTRUCT; // temporarily switching to  construction mode
    else {
      if(have_start_node<0){ // add node first, then mark_distributed
        mode|=gmMODE_CONSTRUCT;
      }
      else 
        mark_distributed();// automatically swithching to distributed regime or going to the next subgraph
    }
  }
  if(mode&gmMODE_CONSTRUCT && // construction mode, begining the construction
    check_distrnode()){ // checking that we are above distrnode
    
    gmNodeID prev_node=graph->get_prev_node(); // get the default previous node
    
    gmNode *node= new sysGraphNode((const char *)name.c_str());//new gmSimpleNode<>(name.c_str(),0);
    // setting the properties
    node->userprop=tmpprop;
    if(tmpact.ptr())
      node->useraction.reset(tmpact->Clone(),1);
    string blockname=tmpprop.blockname;
    tmpprop=gmNodeProp(blockname); // clearing the tmpprop, keeping blockname
    tmpact.reset(NULL); // clearing action
    int groupid=-1;
    if(blockname!="")
      groupid=blocks[blockname];
    gmNodeID this_node=graph->add_node(node,groupid);
		node->id = this_node;
    idcount=this_node;
    if(!(exetype&gmEXE_EXPLICIT_ONLY)) // this node may have implicit code blocks
      node->implicit_action = true;
    
    //callback->NodeStateChanged(this_node,gmNS_NO_NODE,gmNS_UNPROC);
    if(tmpstate!=gmNS_NO_NODE)
      set_node_state(tmpstate,this_node); // also writes graph
    else{
      if(graph_update>=2) // writing the graphwiz file
        write_graph();
    }
    if(have_start_node<0){
      have_start_node = idcount;
      mark_distributed();
    }
    
  }
  else{
    if(have_start_node<0)
      have_start_node = idcount;
    advance_nodeid(); 
  }

  if(mode&gmMODE_CONSTRUCT && entered_exec) // returning to local mode if in exec
    copy_mode(tmode); //mode&=~gmMODE_CONSTRUCT;
  
  return process_cur_node();
  
}
 
struct name_cmp{
  refvector<sysGraphNode> &nodes;
  name_cmp(refvector<sysGraphNode> &nodes_):nodes(nodes_){}
  bool operator()(const pair<int,int> &i1, const pair<int,int> &i2) const {
    return nodes[i1.first]->GetLabel()<nodes[i2.first]->GetLabel();
  }
};

///\en Selects nodes into a vector
int gmManager::select_nodes(vector< pair<int,int> > &nodes, gmSelector selector, bool input, int def_port, int closest_to) const {
  // analyzing preset values
  if(selector.id==gmNODE_PREV || (selector.id==gmNODE_AUTO && selector.str=="" 
                                  && !selector.nodevec.size() && 
                                  !selector.nodestates.size())){ // full auto = previous node
    selector.id=idcount-dprev;
    if(selector.id<0)
      return -1;
    selector.str="";
  }
  else if(selector.id==gmNODE_CUR){
    selector.id=idcount;
    if(selector.id<0)
      return -1;
    selector.str="";
  }

  int res=graph->select_nodes(nodes,selector,input,def_port);
  if(res<0)
    return res;
  if(selector.nodevec.size() || selector.nodestates.size()) // no need to filter further ??
    return res; 
  if(closest_to>0 && selector.dup_sel==gmSEL_NEAREST && selector.id==gmNODE_AUTO){
    // sorting by equal names
    sort(nodes.begin(),nodes.end(),name_cmp(graph->nodes));
    // determining the closest node
    for(size_t i=1;i<nodes.size();i++){
      if(graph->nodes[nodes[i-1].first]->GetLabel()==graph->nodes[nodes[i].first]->GetLabel()){
        if(abs(nodes[i].first-closest_to)>abs(nodes[i-1].first-closest_to)) // keep the closest
          nodes.erase(nodes.begin()+i);
        else
          nodes.erase(nodes.begin()+i-1);
        i--;
        res--;
      }
    }
  }
  return res;
}


int gmManager::graph_link(int type,gmNodeID from, gmNodeID to, int srcport, string *name, string *dest_name){
  int linkid=graph->link(from,to,type,name && *name!="" ? *name : "",srcport, dest_name && *dest_name!="" ? *dest_name : "");
	// set the target state to unprocessed
	graph->set_node_state_by_id(to,gmNS_UNPROC,true);
  if(name  && linkid>=0)
    *name=graph->links[linkid]->GetName();
  return linkid;
}


int gmManager::add_resource(const gmResourceDescr &rdescr, const string &name){
  if(!(exetype&gmEXE_EXPLICIT_ONLY)){ // need mode check?
    // only for local mode
    if(mode!=gmMODE_LOCAL)
      return 0;
  }
  return sched->add_resource(rdescr,name);
}


int gmManager::load_resources(const char *filename){
  if(!(exetype&gmEXE_EXPLICIT_ONLY)){ // need mode check?
    // only for local mode
    if(mode!=gmMODE_LOCAL)
      return 0;
  }
  int nloaded=0;
# ifndef NO_XML
  try{
    XMLFile xmldoc(filename);
    xmlNodePtr root = xmldoc.getRootNode();
    if(!root)
      return LOGERR(0,fmt("gmManager.load_resources: failed to open XML file '%s'",filename),0);
    std::vector<xmlNodePtr> nodes = xmldoc.getAllChildren(root);
    FOR_EACH_LOOP(std::vector<xmlNodePtr>, nodes, node)
      if( xmldoc.getNodeName(*node) == "scheduler" )
        nloaded += sched->Load(xmldoc, *node);
  }
  catch(xml_error){
    return LOGERR(0,fmt("gmManager::load_resources: can't load resources from '%s'\n",filename), 0); 
  }
# else
  LOGMSG(vblWARN,"gmManager::load_resources: NO_XML compiler option used -- can't load/save resource descriptions.\n",0); 
# endif

  return nloaded;
}

int gmManager::save_resources(const char *filename){
  if(!(exetype&gmEXE_EXPLICIT_ONLY)){ // need mode check?
    // only for local mode
    if(mode!=gmMODE_LOCAL)
      return 0;
  } 
  
# ifndef NO_XML  
  XMLFile xmldoc;
  xmlNodePtr root = xmldoc.createDoc("flowgraph");

  // writing resources
  xmlNodePtr reslist = xmldoc.addElement(root, "scheduler");;
  sched->Save(xmldoc, reslist);

  xmldoc.dumpToFile(filename,1);
  
# else
  LOGMSG(vblWARN,"gmManager::save_resources: NO_XML compiler option used -- can't load/save resource descriptions.\n",0); 
# endif
  return 1;
}


int gmManager::save_state(const char *filename){
  
  gmScopeLock scopeLock(mutex);

# ifndef NO_XML
  XMLFile xmldoc;
  xmlNodePtr root = xmldoc.createDoc("flowgraph");

  // writing resources
  xmlNodePtr reslist = xmldoc.addElement(root, "scheduler");
  sched->Save(xmldoc, reslist);

  // writing nodes and links
  gmGraph::graph_t::vertex_iterator vi_start, vi_end, vi;
  boost::tie(vi_start,vi_end)=vertices(graph->g); // getting node iterator
  for(vi=vi_start;vi!=vi_end;++vi){
    int flag=graph->g[*vi].flag;
    if(flag==gmNS_TEMP) // skip temporary nodes
      continue;
    int id=graph->g[*vi].nodeid;
    sysGraphNode &node=*(graph->nodes[id]);
    
    // <node> element
    xmlNodePtr xmlnode = xmldoc.addElement(root, "node");
    xmldoc.addAttribute(xmlnode,"id",fmt("n%d",id));
    if(node.GetFinal())
      xmldoc.addAttribute(xmlnode,"final","yes");
    xmldoc.addAttribute(xmlnode, "ts0",fmt("%g",node.ts0));
    if(node.ts1>0)
      xmldoc.addAttribute(xmlnode,"ts1",fmt("%g",node.ts1));
    if(node.t0>0)
      xmldoc.addAttribute(xmlnode,"t0",fmt("%g",node.t0));
    if(node.t1>0)
      xmldoc.addAttribute(xmlnode,"t1",fmt("%g",node.t1));

    // saving thread list
    vector<int> &nodthreads=graph->g[*vi].threads;
    for(int i=0;i<(int)nodthreads.size();i++){
      xmlNodePtr node1 = xmldoc.addElement(xmlnode, "thread");
      xmldoc.addAttribute(node1,"id",fmt("%d",nodthreads[i]));
    }

    // <progress> subelement
    string progr;
    if(node.progress>=0) // progress is set
      progr=fmt("%f",node.progress);
    else{ // not set, simulating
      if(flag==gmNS_PROC)
        progr="100.";
      else if(flag==gmNS_EXE)
        progr="50.";
      else
        progr="0.";
    }
    xmldoc.addElement(xmlnode, "progress", progr);

    if(node.GetLabel()!="")
      xmldoc.addElement(xmlnode, "label", node.GetLabel());
		
		// data
		if(node.userprop.string_data.size()){
			xmlNodePtr datanode =  xmldoc.addElement(xmlnode, "data");
			for(size_t i=0;i<node.userprop.string_data.size();i++){
        xmlNodePtr node1 = xmldoc.addElement(datanode, "item",node.userprop.string_data[i]);
      }
		}
  }

  // links
  gmGraph::graph_t::edge_iterator ei_start, ei_end, ei;
  boost::tie(ei_start,ei_end)=edges(graph->g); // getting edge iterator
  for(ei=ei_start;ei!=ei_end;++ei){
    int id=graph->edgeid[*ei];
    sysGraphLink &link=*(graph->links[id]);
    
    xmlNodePtr xmlnode = xmldoc.addElement(root, "link");
    
    int type=link.GetType();
    xmldoc.addAttribute(xmlnode,"id",fmt("l%d",id));
    xmldoc.addAttribute(xmlnode,"node0",fmt("n%d",graph->g[source(*ei,graph->g)].nodeid));
    if(type!=gmLINK_HARD)
      xmldoc.addAttribute(xmlnode,"port0",fmt("%d",link.srcport));
    xmldoc.addAttribute(xmlnode,"node1",fmt("n%d",graph->g[target(*ei,graph->g)].nodeid));
    if(type!=gmLINK_HARD)
      xmldoc.addAttribute(xmlnode,"port1",fmt("%d",link.destport));
    string stype;
    
    if(type==gmLINK_HARD)
      stype="hard";
    else if(type==gmLINK_DATA)
      stype="data";
    else if(type==gmLINK_PROC)
      stype="production";
    else if(type==(gmLINK_PROC|gmLINK_DATA))
      stype="data-production";
    else if(type==gmLINK_LOCAL)
      stype="local";
    else
      stype=fmt("%d",type);
    xmldoc.addAttribute(xmlnode,"type",stype);

    if(type&gmLINK_DATA)
      xmldoc.addElement(xmlnode, "label", link.GetName()+"/"+link.GetDestName());
  }

  // threads
  xmlNodePtr xthreads = xmldoc.addElement(root, "threads");

  xmlNodePtr xdistr = xmldoc.addElement(xthreads, "distrnode");
  xmldoc.addAttribute(xdistr,"id",fmt("%d",distr_node));

  vector<gmGraph::wthread_t> &threads=graph->threads;
  for(int i=0; i<(int)threads.size(); i++){
    //if(threads[i].state==1 && !threads[i].persistent)

    xmlNodePtr xthread = xmldoc.addElement(xthreads, "thread");
    xmldoc.addAttribute(xthread,"id",fmt("%d",i));
    int rid=threads[i].resource_id;
		//if(rid<0) // don't record local threads
			//continue; 
    xmldoc.addAttribute(xthread,"resource",rid>=0 ? sched->resources[rid]->name : "local");
    if(rid>=0)
      xmldoc.addAttribute(xthread,"jobid",threads[i].jobid);
    bool persistent=threads[i].persistent;
    if(persistent)
      xmldoc.addAttribute(xthread,"persistent","yes");
    if(threads[i].stopped)
      xmldoc.addAttribute(xthread,"stopped","yes");

    string sstate;
    if(threads[i].state==-1)
      sstate="preparing";
    else if(threads[i].state==0)
      sstate="executing";
    else if(threads[i].state==1)
      sstate="finished";
    else if(threads[i].state==2)
      sstate="restarting";
    else if(threads[i].state==3)
      sstate="failed";
    else if(threads[i].state==4)
      sstate="pending";
    else sstate=fmt("%d",threads[i].state);
    xmldoc.addAttribute(xthread,"state",sstate);

    if(threads[i].stdout_!="")
      xmldoc.addElement(xthread, "stdout", threads[i].stdout_);
    if(threads[i].stderr_!="")
      xmldoc.addElement(xthread, "stderr", threads[i].stderr_);
    
    // output link ids
    for(int j=0; j<(int)threads[i].odlinks.size(); j++){
      xmlNodePtr node1 = xmldoc.addElement(xthread, "odlink");
      xmldoc.addAttribute(node1,"id",fmt("l%d",graph->edgeid[threads[i].odlinks[j]]));
    }
    // input link ids
    for(int j=0; j<(int)threads[i].idlinks.size(); j++){
      xmlNodePtr node1 = xmldoc.addElement(xthread, "idlink");
      xmldoc.addAttribute(node1,"id",fmt("l%d",graph->edgeid[threads[i].idlinks[j]]));
    }
  }

  // groups
  xmlNodePtr xgroups = xmldoc.addElement(root, "groups");
  for(int i=0; i<(int)sched->wdgroup.size(); i++){
    xmlNodePtr xgroup = xmldoc.addElement(xgroups, "group");
    xmldoc.addAttribute(xgroup,"jobid",sched->wdgroup[i].jobid);
    xmldoc.addAttribute(xgroup,"resource",sched->wdgroup[i].resource);
    xmldoc.addAttribute(xgroup,"head_node",fmt("%d",sched->wdgroup[i].head_nodeid));    
  }

  // events
  xmlNodePtr xevents = xmldoc.addElement(root, "events");
  const vector< std::pair<gmNodeID, gmNODE_STATES> >& stack = get_event_stack();
  for(size_t i=0; i<stack.size(); i++){
    xmlNodePtr xevent = xmldoc.addElement(xevents, "event");
    xmldoc.addAttribute(xevent,"nodeid",fmt("%d",stack[i].first));
    xmldoc.addAttribute(xevent,"state",gmGetStateName(stack[i].second));
  }

  //xmldoc.Save(filename);
  // workaroud for a wxFlush bug which results in hang up under Unix (wxWidgets-2.8.4).
  std::string buf;
  xmldoc.dumpToMemory(buf);

  FILE* outfile = fopen(filename, "wt");
  if(!outfile) return LOGERR(-100,fmt("gmManager.save_state: failed to open file '%s' for output",filename),0);
  fprintf(outfile, "%s", (const char *)buf.c_str());
  fclose(outfile);
# else
  LOGMSG(vblWARN,"gmManager::save_state: NO_XML compiler option used -- can't load/save sates.\n",0); 
# endif

  return 1;
}

int gmManager::load_state(const char *filename, int &d_node){
  int res=0;
# ifndef NO_XML
  std::vector<xmlNodePtr> children;

  xmlNodePtr root = NULL;
  XMLFile xmldoc;
  try{
    xmldoc.attach(filename);
    root = xmldoc.getRootNode();
  }
  catch(xml_error){}
  if(!root)
    return LOGERR(-100,fmt("gmManager.load_state: failed to open XML file '%s'",filename),0);
  
  try {  // catching xml_error
    vector<gmGraph::wthread_t> &threads=graph->threads;
    //threads.clear(); !!!!!
    //graph->thread_start=0;
    // reading resources and threads
    d_node=0;

    // <scheduler> element
    xmlNodePtr node = xmldoc.getChildByName(root, "scheduler", true);
    sched->Load(xmldoc,node,false,true);

    // <threads> element
    node = xmldoc.getChildByName(root, "threads", true);
    if( !full_restart ) {
      xmlNodePtr node1 = xmldoc.getChildByName(node, "distrnode", true);
      d_node = xmldoc.parseIntAttribute(node1, "id");
    }

    children = xmldoc.getChildsByName(node, "thread");

    FOR_EACH_LOOP(std::vector<xmlNodePtr>, children, node1) {
      int tid;
      tid = xmldoc.parseIntAttribute(*node1, "id");
      if(tid<0)
        return LOGERR(-2,fmt("gmManager.load_state: invalid id value (%d) of a thread from file '%s'",tid,filename),0);
      if((int)threads.size()<=tid)
        threads.resize(tid+1);
      
      gmdString resource=xmldoc.parseOptionalAttribute(*node1,"resource","local");
      if(resource=="local"){
        threads[tid].resource_id=-1;
        threads[tid].exetype=gmEXE_LOCAL;
      }
      else{
        threads[tid].resource_id=sched->find_resource((const char *)resource.c_str());
        if(threads[tid].resource_id<0)
          return LOGERR(-3,fmt("gmManager.load_state: can't find resource named '%s' of a thread %d from file '%s'",(const char *)resource.c_str(),tid,filename),0);
        threads[tid].exetype=gmEXE_REMOTE;
      }
      gmdString state=xmldoc.parseOptionalAttribute(*node1,"state","preparing");
      if(state=="preparing")
        threads[tid].state=-1;
      else if(state=="executing")
        threads[tid].state=0;
      else if(state=="finished")
        threads[tid].state=1;
      else if(state=="restarting")
        threads[tid].state=2;
      else if(state=="failed")
        threads[tid].state=3;
      else if(state=="pending")
        threads[tid].state=4;
      else if(!sscanf((const char *)state.c_str(),"%d",&threads[tid].state))
        return LOGERR(-4,fmt("gmManager.load_state: invalid state '%s' of thread %d from file '%s'",(const char *)state.c_str(),tid,filename),0);
      
      if(threads[tid].state==-1 && threads[tid].exetype!=gmEXE_LOCAL) // restart all incomplete remote threads
        threads[tid].state=2;


      gmdString pers=xmldoc.parseOptionalAttribute(*node1,"persistent","no");
      if(pers=="yes")
        threads[tid].persistent=true;

      gmdString stopped=xmldoc.parseOptionalAttribute(*node1,"stopped","no");
      if(stopped=="yes")
        threads[tid].stopped=true;

      threads[tid].jobid=xmldoc.parseOptionalAttribute(*node1,"jobid","");
      
      if(threads[tid].state==3 && threads[tid].jobid!="" && !threads[tid].stopped) // try to recover failed threads with valid jobids
        threads[tid].state=0;  // executing

      // thread links
      threads[tid].odlinks.clear();
      threads[tid].idlinks.clear();
      std::vector<xmlNodePtr> children1 = xmldoc.getAllChildren(*node1);
      FOR_EACH_LOOP(std::vector<xmlNodePtr>, children1, node2) {
        int lid;
        if(xmldoc.getNodeName(*node2)=="odlink" || xmldoc.getNodeName(*node2)=="idlink"){
          //xmldoc.parseFormAttribute(*node2,"id","l%d",&lid);
          if( sscanf( xmldoc.parseAttribute(*node2,"id").c_str(), "l%d", &lid ) != 1 )
              return LOGERR(-6,fmt("gmManager.load_state: at %s in thread %d in file '%s', fail to parse id.", (const char *)xmldoc.getNodeName(*node2).c_str(),tid,filename),0);;
          // checking value
          if(lid<0)
            return LOGERR(-6,fmt("gmManager.load_state: id %d of %s in thread %d in file '%s' is out of range (graph changed?)",lid,(const char *)xmldoc.getNodeName(*node2).c_str(),tid,filename),0);
          else if(lid<(int)graph->links.size()){ // this link is already here
            // find the edge with this edge id
            gmGraph::graph_t::edge_descriptor ed;
            if(!graph->find_edge(lid,ed))
              return LOGERR(-12,fmt("gmManager.load_state: id %d of %s in thread %d in file '%s' is invalid (graph changed?)",lid,(const char *)xmldoc.getNodeName(*node2).c_str(),tid,filename),0);
            else{
              if(xmldoc.getNodeName(*node2) == "odlink")
                threads[tid].odlinks.push_back(ed);
              else
                threads[tid].idlinks.push_back(ed);
            }
          }
          else{ // this thread is marked as pending
            threads[tid].state=4;
          }
        }
      }
      if(threads[tid].state==4)
        res++; // have pending threads (still something to read from this file)

      threads[tid].read=true; // mark as read from file
    } // thread

    // <groups> element
    node = xmldoc.getChildByName(root, "groups", true);

    children = xmldoc.getAllChildren(node);
    size_t gid=0;
    FOR_EACH_LOOP(std::vector<xmlNodePtr>, children, node1) {
      if(xmldoc.getNodeName(*node1)=="group" && gid<sched->wdgroup.size()){
        sched->wdgroup[gid].head_nodeid = xmldoc.parseIntAttribute(*node1,"head_node");
        sched->wdgroup[gid].resource=xmldoc.parseOptionalAttribute(*node1,"resource","");
        sched->wdgroup[gid].jobid=xmldoc.parseOptionalAttribute(*node1,"jobid","");
        gid++;
      } // group
    }

    // <events> element
    node = xmldoc.getChildByName(root, "events", true);
    int event_count = 0;
    children = xmldoc.getAllChildren(node);
    FOR_EACH_LOOP(std::vector<xmlNodePtr>, children, node1) {
      if(xmldoc.getNodeName(*node1)=="event"){
        if(event_count>last_loaded_event){  // load the events not loaded before
          int nodeid = xmldoc.parseIntAttribute(*node1,"nodeid");
          enum gmNODE_STATES state = gmGetStateId(xmldoc.parseAttribute(*node1,"state"));
          if(nodeid <(int)graph->nodes.size())
            graph->set_node_state_by_id(nodeid,state,true);
          else{  // first occurence of non-existing node, stop here
            last_loaded_event = event_count;
            break;
          }
        }
        //graph->event_stack.push_back( make_pair( nodeid, state));
        event_count++;
      } // event
    }

    // reading nodes 
    children = xmldoc.getChildsByName(root, "node");
    FOR_EACH_LOOP(std::vector<xmlNodePtr>, children, node) {
      int id;
      //xmldoc.parseFormAttribute(*node,"id","n%d",&id);
      if( sscanf( (const char *)xmldoc.parseAttribute(*node,"id").c_str(), "n%d", &id ) != 1 )
          return LOGERR(-10,fmt("gmManager.load_state: at file '%s', at node '%s', attribute 'id' fail to parse", filename,  xmldoc.getNodePath(*node).c_str() ),0);
      // finding a node with given id
      gmGraph::graph_t::vertex_iterator vi_start, vi_end, vi;
      boost::tie(vi_start,vi_end)=vertices(graph->g); // getting node iterator
      for(vi=vi_start;vi!=vi_end;++vi){
        if(graph->g[*vi].nodeid==id)
          break;
      }
      if(vi==vi_end)
        continue; // this is OK because the graph can grow
        //return LOGERR(-8,fmt("gmManager.load_state: can't find a node with id=%d specified in file '%s' (graph has changed?)",id,filename),0);
			int nid=graph->g[*vi].nodeid;
      sysGraphNode &gnode=*(graph->nodes[nid]);
			if(gnode.read) // already loaded
				continue;


      // setting label
      xmlNodePtr lnode =  xmldoc.getChildByName(*node, "label", false);
      if(lnode){
        std::string label = xmldoc.parseNode(lnode);
        gnode.SetLabel(label);
      }

      vector<int> &nodthreads=graph->g[*vi].threads;
      nodthreads.clear();
      graph->g[*vi].thread_start=0;
      
      std::vector<xmlNodePtr> children1 = xmldoc.getChildsByName(*node, "thread");
      FOR_EACH_LOOP(std::vector<xmlNodePtr>, children1, node1) {
        int tid;
        tid = xmldoc.parseIntAttribute(*node1,"id");
        if(tid<0 || tid>=(int)threads.size())
          return LOGERR(-10,fmt("gmManager.load_state: id %d of a thread from file '%s', node %d is out of range",tid,filename,id),0);
        
				//if(threads[tid].rid<0) // don't process old local threads
					//continue;
				
				nodthreads.push_back(tid);
        // assigning the node state
        /* this is now done via event stack
        if(threads[tid].state==0) //executing
          graph->set_node_state(*vi,gmNS_EXE,true);
        else if(threads[tid].state==1) //finished
          graph->set_node_state(*vi,gmNS_PROC,true);
        else if(threads[tid].state>1) //(temporarily) failed
          graph->set_node_state(*vi,gmNS_FAIL,true);
          */
        // adding this node to thread
        threads[tid].nodes.push_back(*vi);
        
        //if the node is local above distr_node, mark its threads as restarting (require to restart local threads)
        if(nid>distr_node  && gnode.GetFinal() && (full_restart || threads[tid].state==0) )
          threads[tid].state=2;
        // marking finished threads
        if(threads[tid].state==1 || threads[tid].state==3)
          graph->g[*vi].thread_start++;
      } // thread
	   
      // data
      children1 = xmldoc.getChildsByName(*node, "data");
      FOR_EACH_LOOP(std::vector<xmlNodePtr>, children1, node1) {
        std::vector<xmlNodePtr> children2 = xmldoc.getChildsByName(*node1, "item");
        FOR_EACH_LOOP(std::vector<xmlNodePtr>, children2, node2) {
          gnode.userprop.string_data.push_back(xmldoc.parseNode(*node2));
        }
      }
			gnode.read = true;
    } // node
  }
  catch(xml_error err) {
    return LOGERR(-7,fmt(
      "gmManager.load_state: error reading XML from '%s' near %s",
      filename, err.xpath.c_str()),0
    );
  }
# else
  LOGMSG(vblWARN,"gmManager::load_state: NO_XML compiler option used -- can't load/save sates.\n",0); 
# endif

  return res;
}


int gmManager::link_cleanup(){
  typedef component_index<int> components_t;

  components_t components(graph->parent.storage_begin(), graph->parent.storage_end());
  vector<int> comp_state(components.size(),1);
  vector<int> node_comp(num_vertices(graph->g));

  // Iterate through the component indices
  BOOST_FOREACH(int current_index, components) {
    // Iterate through the child vertex indices for [current_index]
    BOOST_FOREACH(int child_index, components[current_index]) {
      node_comp[child_index]=current_index; //cross-indexing
      if(graph->g[child_index].flag!=gmNS_PROC)
        comp_state[current_index]=0;
      
    }
    if(comp_state[current_index]==0)
      continue;
    // fully processed cluster: may delete incoming data files
    BOOST_FOREACH(int child_index, components[current_index]) {
      gmGraph::graph_t::in_edge_iterator ei, ei_end;
      boost::tie(ei,ei_end)=in_edges(child_index,graph->g); // obtaining incoming links for the node
      for(;ei!=ei_end;++ei){
        int i=graph->edgeid[*ei]; 
        if(i>=0 && graph->links[i]->GetType()&gmLINK_DATA && !(graph->links[i]->state&0x1)){ // this is the data link
          gmdString destname, filename, filename2;
          sched->get_idlink_filenames(graph,*ei,filename,destname,filename2,false);
          gmdString files[] = {destname, filename, filename2};
          for(int i=0;i<3;i++){
            //gmdString destname=graph->links[i]->GetDestName();
            if(gmdFileExists(files[i])){
              LOGMSG(vblMESS3,fmt("Removing the file '%s'",(const char *)files[i].c_str()),0);
              gmdRemoveFile(files[i]);
              graph->links[i]->state|=0x1;
            }
          }
        }
      }
    }
  }
  // now finding the processed clusters having output links only to processed clusters 
  BOOST_FOREACH(int current_index, components) {
    if(comp_state[current_index]==0) // not processed
      continue;
    // fully processed cluster: check the clusters it connects to
    BOOST_FOREACH(int child_index, components[current_index]) {
      gmGraph::graph_t::out_edge_iterator ei, ei_end;
      boost::tie(ei,ei_end)=out_edges(child_index,graph->g); // obtaining outgoing links for the node
      for(;ei!=ei_end;++ei){
        int i=graph->edgeid[*ei]; 
        if(i>=0 && graph->links[i]->GetType()&gmLINK_DATA && !(graph->links[i]->state&0x2)){ // this is the data link
          int s=target(*ei,graph->g);
          if(comp_state[node_comp[s]]==1){ // the target cluster is also processed, may delete
            gmdString filename=graph->links[i]->GetName();
            if(gmdFileExists(filename)){
              LOGMSG(vblMESS3,fmt("Removing the file '%s'",(const char *)filename.c_str()),0);
              gmdRemoveFile(filename);
              graph->links[i]->state|=0x2;
            }
          }
        }
      }
    }
  }
  return 1;
}


pair<int,int> gmManager::get_inlink(int nodeid, int portid) const {
  return graph->get_inlink(nodeid,portid);
}


gmNode *gmManager::_node_ptr(int nodeid) const{
  if(nodeid<0){ // applying selectors
    gmNodeID cid = get_curnode();
    if(nodeid == gmNODE_AUTO || nodeid == gmNODE_CUR)
      nodeid = cid;
    else if(nodeid == gmNODE_PREV)
      nodeid = cid -1;
  }
  if(nodeid<0 || nodeid>=(int)graph->nodes.size())
    return NULL;
  return 
    graph->nodes[nodeid];
}


gmEdge *gmManager::get_edge_ptr(int edgeid) const{
  if(edgeid<0 || edgeid>=(int)graph->links.size())
    return NULL;
  return 
    graph->links[edgeid];
}


//\en Finds one edge by source and destination node ids.
//    \return ID of an edge or \ref gmEDGE_NONE if not found
gmEdgeID gmManager::find_edge(gmNodeID source, gmNodeID dest) const{
	std::vector<int> linkids;
	graph->find_links(source,-1,dest,-1,linkids);
	if(linkids.size())
		return linkids[0];
	else
		return gmEDGE_NONE;
}


//\en Selects edges matching all pairs of source and destination node selectors.
int gmManager::select_edges(std::vector<gmEdgeID> &edges, gmSelector source, gmSelector dest, int srcport , int destport) const {
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,source,false,srcport, idcount)<0)
    return 0;
	std::vector< std::pair<int,int> > outputs;
  if(select_nodes(outputs,dest,false,destport, idcount)<0)
    return 0;
	int hits = 0;
	for(size_t i=0;i<inputs.size();i++){
		for(size_t j=0;j<outputs.size();j++){
		  hits+=graph->find_links(inputs[i].first,srcport,outputs[j].first,destport,edges);
		}
	}
  return hits;
}

gmNodeID gmManager::get_source(gmEdgeID edgeid) const{
	gmGraph::graph_t::edge_descriptor ed;
  if(!graph->find_edge(edgeid,ed))
		return gmNODE_NONE;
  return graph->g[source(ed,graph->g)].nodeid;
}


int gmManager::block(const string &blockname, bool cond){
  (void) cond;
  tmpprop.blockname=blockname;
  if(blockname!=""){
    if(blocks.find(blockname)==blocks.end())
      blocks[blockname]=graph->add_group(blockname);
  }  
  return 1;
}




///\en Group nodes together: the grouped nodes will share working directory
///\en \return group ID>=0 on success, <0 on failure
int gmManager::make_node_group(gmSelector nodes){
  if(!in_construction())
    return -1;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-2,fmt("gmManager.make_node_group: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.str.c_str()),0);
  int hits=0, gid=-1;
  for(size_t i=0;i<inputs.size();i++){
    gmNode *nodep=_node_ptr(inputs[i].first);
    if(nodep){ 
      //if(nodep->userprop.workdir_id>=0 && nodep->userprop.workdir_id!=sched->get_group_count())// already set differently
      //  LOGERR(-2,fmt("gmManager.make_node_group: node %d already belongs to group %d, setting skipped",inputs[i].first,nodep->userprop.workdir_id),0);
      //else{
        nodep->userprop.workdir_id= sched->get_group_count();
        if(!hits) // adidng display group for the first time
          gid=graph->add_group(fmt("workdir group %d",nodep->userprop.workdir_id)); // group for displaying
        graph->gmap[inputs[i].first] = gid; // linking
        hits++;
      //}
    }
  }
  if(hits){ // really added something
    int res=(int)sched->add_group();
    if(graph_update>=2) // writing the graphwiz file
      write_graph();
    return res;
  }

  return -3; // added nothing
}



///\en Assign nodes to the existing group: the grouped nodes will share working directory
///\en \return group ID>=0 on success, <0 on failure
int gmManager::assign_to_group(int groupid, gmSelector nodes){
  if(!in_construction())
    return -1;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-2,fmt("gmManager.assign_to_group: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.str.c_str()),0);
  if(groupid<0 || groupid>=(int)sched->get_group_count())
    return LOGERR(-3,fmt("gmManager.assign_to_group: the group with id %d does not exist",groupid),0);

  int hits = 0;
  for(size_t i=0;i<inputs.size();i++){
    gmNode *nodep=_node_ptr(inputs[i].first);
    if(nodep){ 
      nodep->userprop.workdir_id= groupid;
      // linking to graoh display group 
      int gid=graph->find_group(fmt("workdir group %d",nodep->userprop.workdir_id)); // group for displaying
      if(gid>=0)
        graph->gmap[inputs[i].first] = gid; // linking
      hits++;
    }
  }
  if(hits){ // really added something
    if(graph_update>=2) // writing the graphwiz file
      write_graph();
    return groupid;
  }

  return -4; // added nothing
}

///\en Set an action object associated with the node. OnExecute() will be called for that object when node executes.  
///    \return Number of affected nodes.
int gmManager::set_node_action(const gmNodeAction &action, gmSelector nodes){
  if(!in_construction())
    return 0;
  if(nodes.id==gmNODE_NEXT){ // reserving for next nodes
    tmpact.reset(action.Clone(),1);
    tmpprop.application_call=""; // action is defined by a class
    return 1;
  }
  if(nodes.id==gmNODE_AUTO) // auto means current
    nodes.id = gmNODE_CUR;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-1,fmt("gmManager.set_node_action: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.to_string().c_str()),0);
  for(size_t i=0;i<inputs.size();i++){
    gmNode *nodep=_node_ptr(inputs[i].first);
    if(nodep){
      gmNodeAction *action_copy = action.Clone();
      nodep->useraction.reset(action_copy,1);
      action_copy->SetManager(this);
      action_copy->SetParent(nodep);
    }
  }
  return (int)inputs.size();
}


///\en Sets a new name to selected node(s). 
///    \return Number of affected nodes.
int gmManager::set_node_name(const std::string &newname, gmSelector nodes){
  if(!in_construction())
    return 0;
  if(nodes.id==gmNODE_NEXT) // can't do for next node
    return 0;
  if(nodes.id==gmNODE_AUTO && nodes.str=="") // auto means current
    nodes.id = gmNODE_CUR;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-1,fmt("gmManager.set_node_name: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.to_string().c_str()),0);
  for(size_t i=0;i<inputs.size();i++){
    gmNode *nodep=_node_ptr(inputs[i].first);
    if(nodep)
			nodep->SetLabel(newname);
  }
  return (int)inputs.size();
}


///\en Set an action command associated with the node. The command will be executed (locally or remotely) when node executes.
int gmManager::set_node_action(const std::string &command, gmSelector nodes){
  if(!in_construction())
    return 0;
  if(nodes.id==gmNODE_NEXT){ // reserving for next nodes
    tmpprop.application_call=command; // action is command
    tmpact.reset(NULL); 
    return 1;
  }
  if(nodes.id==gmNODE_AUTO) // auto means current
    nodes.id = gmNODE_CUR;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-1,fmt("gmManager.set_node_action: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.to_string().c_str()),0);
  for(size_t i=0;i<inputs.size();i++){
    gmNode *nodep=_node_ptr(inputs[i].first);
    if(nodep)
      nodep->userprop.application_call=command;
  }
  return (int)inputs.size();
}


enum gmNODE_STATES gmManager::get_node_state_by_id(gmNodeID nid) const {
  return graph->get_node_state_by_id(nid);
}

size_t gmManager::clean_jobs(){
  if(!in_construction())
    return 0;
  if(!sched)
    return 0;
  return sched->clean_jobs();
}

int gmManager::fetch_workdir_files(int groupid, const std::vector< pair<std::string, std::string> > &files){
  // make shure we are not in worker mode
  if(get_mode()&gmMODE_WORKER)
    return -7;
  return sched->fetch_workdir_files(groupid,files);
}

int gmManager::fetch_node_files(gmSelector nodes, const std::vector< pair<std::string, std::string> > &files){
 if(!in_construction())
    return 0;
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-2,fmt("gmManager.fetch_node_files: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.to_string().c_str()),0);
  // fetching
  int res, nfiles = 0;
  for(size_t i=0;i<inputs.size();i++){
    res = sched->fetch_node_files(graph,inputs[i].first,files);   
    if(res>0)
      nfiles++;
  }
  return nfiles>0 ? nfiles : res;
}

int gmManager::set_node_state(enum gmNODE_STATES state, gmSelector nodes){
  if(!in_construction())
    return 0;
  if(nodes.id==gmNODE_NEXT){ // reserving for next nodes
    tmpstate=state;
    return 1;
  }
  // checking that the state is valid
  bool allowed=
    state==gmNS_NO_NODE ||
    state==gmNS_UNPROC ||
    state==gmNS_PROC ||
    state==gmNS_FAIL ||
    state==gmNS_WAIT ;
    
  if(!allowed)
    return -1; // all other states can not be set manually

  
  std::vector< std::pair<int,int> > inputs;
  if(select_nodes(inputs,nodes,false, -1, idcount)<0)
    return LOGERR(-1,fmt("gmManager.set_node_state: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.to_string().c_str()),0);
  // changing state
  int hits  = 0;
  for(size_t i=0;i<inputs.size();i++){
    gmNODE_STATES gns=graph->get_node_state_by_id(inputs[i].first);
    // some nodes have internal states and cannot be assigned a new state
    if(gns==gmNS_EXE   ||
      gns==gmNS_TARG || gns==gmNS_TEMP){
      LOGMSG(vblWARN,fmt("gmManager.set_node_state: trying to assign a new state for node%d being processed, skipped",inputs[i].first),0); 
      continue;
    }
    gmNODE_STATES prev = graph->set_node_state_by_id(inputs[i].first,state,true);
    if(prev!=gmNS_NO_NODE && prev!=state)
      hits++;
    if(state==gmNS_UNPROC)
      graph->unprocess_recursively_by_id(inputs[i].first);  // marks nodes as unprocessed by recursively following outgoing links     
  }
  

  if(hits && graph_update>=2) // writing the graphwiz file
    write_graph();
  

  return hits; //(int)inputs.size();
}


int gmManager::stop_node(gmSelector nodes, bool fetch_results, int cleanup){
  //if(!in_construction())
    //return 0;
  if(!sched)
    return 0;
  std::vector< std::pair<int,int> > nodeids;
  if(select_nodes(nodeids,nodes)<=0)
    return 0;
  int hits = 0;

  for(size_t i=0;i<nodeids.size(); i++){
    gmNODE_STATES st=get_node_state_by_id(nodeids[i].first);
    if(st!=gmNS_EXE)
      continue;
    hits+=sched->stop_node_threads(graph,nodeids[i].first,fetch_results);
    // add cleanup code here
  }
  if(cleanup) // TODO: make proper cleanup for selected nodes only
    clean_jobs();
  if(hits){
    updated_status();
  }
  return hits;
}

int gmManager::process_cur_node(){
  if(mode&gmMODE_WORKER){ // worker mode
    int must_execute = check_subgraph(idcount);
    if(must_execute && idcount >=0 && idcount<(int)graph->nodes.size()){ //local actions
        // local node action
      gmNodeAction *action=graph->nodes[idcount]->useraction.ptr();
      if(action){
        int res=action->OnExecute();
        if(res<0){ // failed
          LOGERR(-1,fmt("gmManager.process_cur_node: local function of node %d retuned fail status %d",
                        idcount,res),0);
          // TODO : set error flag here
        }
      }

      // it can also be application call:
      string command=graph->nodes[idcount]->userprop.application_call;
      if(command!=""){
        int res = ::system((const char *)command.c_str());  // passes environment
        //res = (int)gmdExecute((const char *)command.c_str(),gmdEXEC_SYNC);
        if(res!=0){ // failed
          LOGERR(-1,fmt("gmManager.process_cur_node: local application call of node %d exited with status %d",
                        idcount,res),0);
          // TODO : set error flag here
        }
      }
    }
    return must_execute;
  }
  return 0;
}


 bool gmManager::job_state_changed() const{
   return nfailed > 0 ? true : ( nfinished ? true : (nstarted > 0 ? true : false ) );
 }


const vector< std::pair<gmNodeID, gmNODE_STATES> >& gmManager::get_event_stack() const {
  return graph->event_stack;
}
