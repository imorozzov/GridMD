/*s***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2005-2012        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD, ivutils
 *
 *   This source code is Free Software and distributed under the terms of wxWidgets license (www.wxwidgets.org) 
 *
 *   $Revision: 1.15 $
 *   $Date: 2014/09/26 16:57:56 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmsched.h,v 1.15 2014/09/26 16:57:56 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmsched.h,v $
$Revision: 1.15 $
$Author: valuev $
$Date: 2014/09/26 16:57:56 $
*/
/*s****************************************************************************
 * $Log: gmsched.h,v $
 * Revision 1.15  2014/09/26 16:57:56  valuev
 * one more sync
 *
 * Revision 1.14  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.13  2013/06/18 13:55:42  valuev
 * changed temporary names algorithm
 *
 * Revision 1.12  2013/06/13 16:58:36  valuev
 * added path checks
 *
 * Revision 1.11  2013/05/31 17:46:11  valuev
 * added wildcard links
 *
 * Revision 1.10  2013/05/27 17:00:35  valuev
 * added conversion to const char * for wxWidgets UNICODE
 *
 * Revision 1.9  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.8  2013/03/07 17:08:59  valuev
 * kintech xml
 *
 * Revision 1.7  2012/12/24 17:01:58  valuev
 * fixed local link check
 *
 * Revision 1.6  2012/12/04 17:10:31  valuev
 * added windows job manager, implemented fetching workdir files, fixed file renaming after transfer
 *
 * Revision 1.5  2012/11/08 14:32:50  valuev
 * various extensions
 *
 * Revision 1.4  2012/11/07 22:13:54  valuev
 * grouped nodes
 *
 * Revision 1.3  2012/11/06 11:35:41  valuev
 * added resource per node specification
 *
 * Revision 1.2  2012/10/30 11:39:58  valuev
 * modified queueing (working)
 *
 * Revision 1.1  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 *
*******************************************************************************/


# ifndef GMSCHED_H
# define GMSCHED_H

/** @file gmsched.h
    \en @brief Classes for job scheduling within GridMD

**/

# include "workflow.h"
#ifdef _WIN32
# include "jobmngr/winshell.h"
# else
# include "jobmngr/unixshell.h"
# endif

///\en Resource class 
class gmResource{
  friend class gmScheduler;
  friend class gmManager;
  mngptr<gmJobManager> pjm;
  gmShell* psh;
  string name;
  vector<std::string> persistent_jobs;
public:
  gmResourceDescr descr;
  ///\en cleans persistent jobs of the resource
  ///    \return number of jobs cleaned successfully
  size_t clean_jobs();
};





///\en Scheduler managing different resources
class gmScheduler{
  friend class gmManager;
  struct wd_group_t{
    std::string jobid;
    std::string resource;
    gmNodeID head_nodeid;
    wd_group_t():head_nodeid(-1){}
  };
protected:
  ///\en Resources for external execution
  refvector<gmResource> resources;
  refvector<gmShell> shells;
  map<string, int> names;
  ///\en Resource counter to loop over resources
  size_t res_cnt;
  gmManager *sys;
  vector<wd_group_t> wdgroup;
  ///\en shell for local operations (file copy, etc.)
  gmShell* local_shell;
  gmdString workdir;
  ///\en thread pool to create tasks for local threads (initialized by first use)
  gmdThreadPool *threadpool;

public:
  gmScheduler(gmManager *sys_):sys(sys_),resources(1),shells(1){
    res_cnt=0;
    
#ifdef _WIN32
    local_shell = new gmShellWin();
#else
    local_shell =  new gmShellUnix();
# endif 
    workdir = gmdGetCwd();
    local_shell->SetParam("home_dir",workdir);
    local_shell->SetParam("home_dir_win",workdir);
    shells.push_back(local_shell);
    threadpool = NULL;
  }
  ~gmScheduler(){
    if(threadpool) delete threadpool;
  }
 
  size_t add_group(){
    wdgroup.push_back(wd_group_t());
    return wdgroup.size()-1;
  }

  size_t get_group_count() const {
    return wdgroup.size();
  }

  int find_resource(const string &name){
    if(names.find(name)!=names.end())
      return names[name];
    return -1;
  }

  ///\en  Adds the resource to the resource description list. Default name is the same as hostname;
  ///     If \a reload is \a true, then changes the existing resource for the new one in case of duplicated names.
  ///     \return the current number of resources (on success)
  ///     \return <0 on error: -1 means that the naem exists
  int add_resource(const gmResourceDescr &rdescr, const string &name="", bool reload=false);

# ifndef NO_XML
  ///\en Saves the scheduler appending the resources as chidren to the given node
  int Save(XMLFile& xmldoc, xmlNodePtr node){
    for(size_t i=0;i<resources.size();i++){
      resources[i]->descr.Save(xmldoc, node, (const char *)resources[i]->name.c_str());
    }
    return 1;
  }

  ///\en Loads resources from the current node. Clears existing ones if \a clear flag is set.
  ///    If \a reload is \a true, then changes the existing resource for the new one in case of duplicated names.
  ///    \return the number of loaded resources
  int Load(XMLFile& xmldoc, xmlNodePtr node, bool clear=false, bool reload=false);

# endif

  size_t queue_jobs(gmGraph *graph, int exetype);

  size_t check_jobs(gmGraph *graph, size_t *nfailed = NULL, int thread_id =-1, bool stop = false, bool force_fetch = false);



  size_t clean_jobs(){
    size_t ncleaned=0;
    for(size_t i=0;i<resources.size();i++){
      LOGMSG(vblMESS3,fmt("Cleaning jobs from resource '%s'", (const char *)resources[i]->name.c_str()),0);     
      ncleaned+=resources[i]->clean_jobs();
    }
    return ncleaned;
  }
 
  int fetch_workdir_files(int groupid, const std::vector< pair<std::string, std::string> > &files);
  int fetch_node_files(gmGraph *graph, int nid, const std::vector< pair<std::string, std::string> > &files);

  int stage_out_files(gmJob *job, const std::vector< pair<std::string, std::string> > &files);

  bool get_idlink_filenames(gmGraph *graph, gmGraph::graph_t::edge_descriptor edge, gmdString &filename, gmdString &destname, gmdString &filename2, bool thread_local_=false);
  bool get_odlink_filenames(gmGraph *graph, gmGraph::graph_t::edge_descriptor edge, gmdString &filename, gmdString &destname, bool thread_local_=false);

  int stop_node_threads(gmGraph *graph, int nid, bool fetch_results);

};



# endif
