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
 *   $Revision: 1.41 $
 *   $Date: 2016/02/08 08:16:59 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/src/gmsched.cpp,v 1.41 2016/02/08 08:16:59 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/src/gmsched.cpp,v $
$Revision: 1.41 $
$Author: valuev $
$Date: 2016/02/08 08:16:59 $
*/
/*s****************************************************************************
 * $Log: gmsched.cpp,v $
 * Revision 1.41  2016/02/08 08:16:59  valuev
 * working examples
 *
 * Revision 1.40  2016/02/05 19:04:45  valuev
 * implicit workflow fixes
 *
 * Revision 1.39  2016/02/04 16:41:04  valuev
 * serial final job execution
 *
 * Revision 1.38  2016/02/02 12:15:35  valuev
 * fixing mutex
 *
 * Revision 1.37  2015/06/17 19:37:59  valuev
 * sweep farm restructured
 *
 * Revision 1.36  2015/05/12 13:53:45  valuev
 * new version with all functions in read node
 *
 * Revision 1.35  2015/03/30 17:08:53  valuev
 * vdnodes
 *
 * Revision 1.34  2015/03/30 11:50:07  valuev
 * workinng version of sweep farm
 *
 * Revision 1.33  2015/03/27 18:23:15  valuev
 * edges
 *
 * Revision 1.32  2015/03/16 16:10:20  valuev
 * working on sweep farm
 *
 * Revision 1.31  2014/09/26 16:57:56  valuev
 * one more sync
 *
 * Revision 1.30  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.29  2013/07/04 14:06:24  valuev
 * extended file management
 *
 * Revision 1.28  2013/06/18 13:55:42  valuev
 * changed temporary names algorithm
 *
 * Revision 1.27  2013/06/13 16:58:36  valuev
 * added path checks
 *
 * Revision 1.26  2013/05/31 17:46:11  valuev
 * added wildcard links
 *
 * Revision 1.25  2013/05/28 12:20:06  valuev
 * wxWidgets compatibility fixes
 *
 * Revision 1.24  2013/05/27 17:00:35  valuev
 * added conversion to const char * for wxWidgets UNICODE
 *
 * Revision 1.23  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.22  2013/03/07 17:08:59  valuev
 * kintech xml
 *
 * Revision 1.21  2012/12/24 17:01:58  valuev
 * fixed local link check
 *
 * Revision 1.20  2012/12/21 16:23:05  valuev
 * unified node states
 *
 * Revision 1.19  2012/12/12 17:28:00  valuev
 * added virtual nodes and dummy jobs
 *
 * Revision 1.18  2012/12/11 17:24:36  valuev
 * updated scheduling, node state changes on the fly
 *
 * Revision 1.17  2012/12/04 17:10:31  valuev
 * added windows job manager, implemented fetching workdir files, fixed file renaming after transfer
 *
 * Revision 1.16  2012/11/30 18:37:14  valuev
 * modified file handling
 *
 * Revision 1.15  2012/11/29 18:14:48  valuev
 * sync with svn
 *
 * Revision 1.14  2012/11/22 17:01:06  valuev
 * sync with kintech svn
 *
 * Revision 1.13  2012/11/21 17:06:15  valuev
 * sync with kintech svn
 *
 * Revision 1.12  2012/11/08 14:32:50  valuev
 * various extensions
 *
 * Revision 1.11  2012/11/07 22:13:54  valuev
 * grouped nodes
 *
 * Revision 1.10  2012/11/06 18:05:43  valuev
 * various small fixes, sync with svn
 *
 * Revision 1.9  2012/11/06 11:35:41  valuev
 * added resource per node specification
 *
 * Revision 1.8  2012/11/02 14:19:54  valuev
 * sync
 *
 * Revision 1.7  2012/10/31 17:30:44  valuev
 * extended local execution, added return code analysis
 *
 * Revision 1.6  2012/10/30 11:39:58  valuev
 * modified queueing (working)
 *
 * Revision 1.5  2012/10/29 11:10:14  valuev
 * mdified queueing (not working)
 *
 * Revision 1.4  2012/10/24 17:05:00  valuev
 * working explicit workflow and example
 *
 * Revision 1.3  2012/10/24 08:23:39  valuev
 * tested explicit workflow
 *
 * Revision 1.2  2012/10/23 10:44:56  valuev
 * node action
 *
 * Revision 1.1  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 *
*******************************************************************************/

#include <gmd/filefn.h>

# include "gmsched.h"

//int gmCheckDirTree(const gmdString &filename, bool makeall);

bool check_or_create_dir(const gmdString &file){
  gmdString dir;
  gmdFileName::SplitPath(file,&dir,NULL,NULL);
  if(dir == "")
    return true;
  if(dir.Find('*') != gmdNOT_FOUND || dir.Find('?') != gmdNOT_FOUND) // has wildcards, no check
    return true;
#ifdef _WIN32
  int res = gmShellWin().MkDirLocal(dir.c_str());
#else
  int res = gmShellUnix().MkDirLocal(dir.c_str());
# endif 
  return (res==0);
}

int gmScheduler::add_resource(const gmResourceDescr &rdescr, const string &name, bool reload){
  // checking name
  string rname;
  if(name=="")
    rname=rdescr.session.host;
  else
    rname=name;
  if(rname=="local")
    return LOGERR(-1,fmt("gmScheduler.add_resource: the name '%s' is reserved, can't add a resource with this name",(const char *)name.c_str()),0);
  if(names.find(rname)!=names.end()){
    if(!reload)
      return LOGERR(-1,fmt("gmScheduler.add_resource: resource with the name '%s' already exists in the list!",(const char *)rname.c_str()),0);
  }
  else
    reload=false;

  gmShell *shell = NULL;
  for(size_t i=0; i < resources.size(); i++)
    if( resources[i]->descr.session == rdescr.session ) {
      shell = resources[i]->psh;
      break;
    }

  if(!shell) {  // Shell is not found in the list
    shell = rdescr.session.CreateShell();
    if(!shell)
      return LOGERR(-1,fmt("gmScheduler.add_resource: can't create session shell for resource '%s'!",(const char *)rname.c_str()),0);
    shells.push_back(shell);
  }

  gmJobManager *jm = rdescr.CreateJobManager(shell);   
  if(!jm)
    return LOGERR(-1,fmt("gmScheduler.add_resource: can't create job manager for resource '%s'!",(const char *)rname.c_str()),0);

  // binding  
  gmResource *res;
  res=new gmResource;
  res->descr=rdescr;
  res->psh = shell;
  res->pjm.reset(jm,1);
  res->name=rname;

  if(reload){
    // deleting this one
    resources.erase(resources.begin()+names[rname]);
    // inserting
    resources.insert(resources.begin()+names[rname],res);
  }
  else{
    names[res->name]=(int)resources.size();
    resources.push_back(res);
  }
  // TODO:
  // now test the resource and deploy the program
  // ....
  return (int)resources.size()-1;
}

# ifndef NO_XML

int gmScheduler::Load(XMLFile& xmldoc, xmlNodePtr node, bool clear, bool reload){
  if(clear)
    resources.clear();
  int nloaded=0;
  std::vector<xmlNodePtr> nodes;
  if(node)
    nodes = xmldoc.getAllChildren(node);

  FOR_EACH_LOOP(std::vector<xmlNodePtr>, nodes, child){
    xmlNodePtr node = *child;

    if(gmdString((const char*)node->name) == "resource"){
      gmResourceDescr rdescr;
      gmdString name;
      if(rdescr.Load(xmldoc, node, name)>0){
        if(add_resource(rdescr,(const char *)name.c_str(),reload)<0)
          LOGMSG(vblWARN,fmt("gmScheduler.Load: failed to add resource named '%s', resource skipped",(const char *)name.c_str()),0);
        else
          nloaded++;
      }
      else
        LOGMSG(vblWARN,fmt("gmScheduler.Load: failed to load resource, skipped"),0);
    }
  }
  return nloaded;
}

# endif


// returns true if the name is directory
bool is_dirname(const gmdString &filename){
  gmdString dir, name, ext;
  // filter filename (or target wildcard) from destination name  
  gmdFileName::SplitPath(filename,&dir,&name,&ext);
  if(name =="" && ext == "")
    return true; // destination is directory
  else
    return false;
}

// gets unique temporary names for a link, modifies filename and destname 
void get_tmp_names(gmdString &filename, gmdString &destname, bool remote_to_local, const gmdString &prefix, int source_nodeid,int target_nodeid, int linkid, bool check_dir = true){
  int flags = 0;
  bool has_wildcards = false;
  if(filename.Find('*') != gmdNOT_FOUND || filename.Find('?') != gmdNOT_FOUND) // has wildcards
    has_wildcards = true;

  gmdString result = prefix+gmdString::Format("/link%d_n%d-n%d/",linkid, source_nodeid, target_nodeid);
  if(check_dir && !check_or_create_dir(result)){
    LOGMSG(vblWARN,fmt("get_tmp_names: failed to create local directory '%s'!",(const char *)result.c_str()),0);
  }

  gmdString destname0=destname, dir, name, ext;
  if(has_wildcards) // ensure that destination name is a directory ending with separator
    destname0+="/";
     
  // filter filename (or target wildcard) from destination name  
  gmdFileName::SplitPath(destname0,&dir,&name,&ext);
  gmdString mydest = result+ name;
  if(ext!="")
    mydest+=gmdString(".")+ext;

  //if(name =="" && ext == "")
    //mydest+="/"; // destination is directory
  
  if(remote_to_local){
    if(has_wildcards)
      destname = result; // redirect destination to tmp directory, source mask unchanged
    else
      destname = mydest;
  }
  else{
    if(has_wildcards){
      filename = result + "*"; // source mask: get all from tmp directory
      destname = dir + "/"; // take dir only from  destination
    }
    else{
      filename = mydest; // destname unchanged
    }
  }

  
  /*gmdString name_only, ext, fileres;
  gmdFileName::SplitPath(filename,(gmdString *)NULL,(gmdString *)NULL,&name_only,&ext,gmdPATH_NATIVE); 
  fileres=name_only+ext+gmdString::Format("_node%d", source_nodeid);
  gmdFileName::SplitPath(destname,(gmdString *)NULL,(gmdString *)NULL,&name_only,&ext,gmdPATH_NATIVE); 
  return fileres+"-"+name_only+ext+gmdString::Format("_node%d_link%d", target_nodeid, linkid);*/
} 

// filename2 is alternative terget location
bool gmScheduler::get_idlink_filenames(gmGraph *graph, gmGraph::graph_t::edge_descriptor edge, gmdString &filename, gmdString &destname, gmdString &filename2, bool thread_local_){
  gmNodeID source_nodeid = graph->g[source(edge,graph->g)].nodeid;
  gmNodeID target_nodeid = graph->g[target(edge,graph->g)].nodeid;
  filename=
    graph->links[graph->edgeid[edge]]->GetName();
  destname=
    graph->links[graph->edgeid[edge]]->GetDestName();
 
  bool src_local = graph->get_node(source_nodeid)->GetLocal()!=0;
  bool trg_local = graph->get_node(target_nodeid)->GetLocal()!=0;

  filename2 = filename;
  if(!src_local && !trg_local && !thread_local_){ // both nonlocal, use tmp file
    get_tmp_names(filename,destname,false,workdir+"/"+sys->name.c_str()+".tmp",source_nodeid,target_nodeid,graph->edgeid[edge], false);
    //filename=get_tmp_name(filename,source_nodeid,destname,target_nodeid,graph->edgeid[edge]);
  }
  else{
    get_tmp_names(filename2,destname,false,workdir+"/"+sys->name.c_str()+".tmp",source_nodeid,target_nodeid,graph->edgeid[edge], false);
  }
  return trg_local;
}

bool gmScheduler::get_odlink_filenames(gmGraph *graph,gmGraph::graph_t::edge_descriptor edge, gmdString &filename, gmdString &destname, bool thread_local_){
  //gmdString rr = gmdFileName::CreateTempFileName("./gmname-");
  //gmdString rr2 = _tempnam("./","gmname-");
  //gmdString rr2 = tmpnam("gmname-");

  gmNodeID source_nodeid = graph->g[source(edge,graph->g)].nodeid;
  gmNodeID target_nodeid = graph->g[target(edge,graph->g)].nodeid;
  filename=
    graph->links[graph->edgeid[edge]]->GetName();
  destname=
    graph->links[graph->edgeid[edge]]->GetDestName();
 
  bool src_local = graph->get_node(source_nodeid)->GetLocal()!=0 ;
  bool trg_local = graph->get_node(target_nodeid)->GetLocal()!=0 ;

  if(!src_local && !trg_local && !thread_local_){ // both nonlocal, use tmp file
    get_tmp_names(filename,destname,true,workdir+"/"+sys->name.c_str()+".tmp",source_nodeid,target_nodeid,graph->edgeid[edge]);
    //destname=get_tmp_name(filename,source_nodeid,destname,target_nodeid,graph->edgeid[edge]);
  }
  return trg_local;
}


gmERROR_CODES classify_jm_error(gmJobManager *pjm, gmERROR_CODES guess = gmERR_RESOURCE){
  if(!pjm)
    return gmERR_RESOURCE;
  gmShell* shell = pjm->GetShell();
  if(!shell)
    return gmERR_RESOURCE;
  gmdString err =shell->get_err_msg();
  if(err.Contains("incomplete file transfer")) // connection
    return gmERR_CONNECTION;
  if(err.Contains("LibSSH error")) // connection
    return gmERR_CONNECTION;
  else // unknown
    return guess;
}





size_t gmScheduler::queue_jobs(gmGraph *graph, int exetype){
  
  size_t nrunning=0;
  vector<gmGraph::wthread_t> &threads=graph->threads;
  for(size_t i=0/*graph->thread_start*/;i<threads.size();i++){
    if(threads[i].state==1 || threads[i].state>=3) // finished or failed
      continue;
    string nodelist="-w"; // forming nodelist
    vector< vector<int> > jobset; // subjobs of current thread
    int mynode=-1; // tells whether the last subset is internal (>=0) or external (-1)
    int has_local=0;
    threads[i].exetype=exetype; 
    string thread_resource;
    string thread_info;   //  Extra job info that can be saved into 'info' file in the working directory
    unsigned thread_nproc=0;     // total number of processes
    unsigned thread_ppn=0;       // number of processes per node
    unsigned thread_walltime=0;  // maximum execution wall time in seconds, 0 if unset
    bool thread_mpi=false;       // indicates that this is an MPI job

    int thread_wd_id=-1, thread_rnode=0;
    for(size_t j=0;j<threads[i].nodes.size();j++){
      

      // check whether there is a finish node
      if(graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->GetFinal()==1 && !(exetype&gmEXE_EXPLICIT_ONLY)){
        //has_local=1;
        //LOGMSG(vblMESS3,nodelist.c_str(),0);
        LOGMSG(vblMESS3,fmt("'Final' node %d found, passing to serial mode",graph->g[threads[i].nodes[j]].nodeid),0);
        //threads[i].state=0;
        threads[i].exetype=gmEXE_SERIAL; // will be thrown after checkup
        //throw task_finished(); // this must be processed in local mode
      }
      else {// check whether there is a local node
        bool local=graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.local;
        if(graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->GetFinal()==1) // final is always local
          local=true;
        if(local && !has_local){
          has_local=1;
          //LOGMSG(vblMESS3,nodelist.c_str(),0);
          LOGMSG(vblMESS3,fmt("'Local' node %d found, passing to local mode",graph->g[threads[i].nodes[j]].nodeid),0);
          threads[i].exetype=gmEXE_LOCAL; 
        }
      }
      nodelist+=fmt("%d",graph->g[threads[i].nodes[j]].nodeid);
      if(j!=threads[i].nodes.size()-1)
        nodelist+=":";

      // checking if we have special resource assignments
      string set_resource = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.resource;
      if(set_resource !=""){
        if(thread_resource!="" && set_resource!=thread_resource)
          LOGMSG(vblWARN,
             fmt("gmManager.queue_jobs: resource '%s' set for node %d is different"
             " from '%s' assigned for a forerunner node in the thread, keeping first"
             " assignment",(const char *)thread_resource.c_str(),graph->g[threads[i].nodes[j]].nodeid,(const char *)set_resource.c_str()),0);
        else
          thread_resource=set_resource;
        
      }
      // checking if we have workdir group assignment
      int set_wd_id = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.workdir_id;
      if(set_wd_id !=-1){
        if(thread_wd_id!=-1 && set_wd_id!=thread_wd_id)
          LOGMSG(vblWARN,
             fmt("gmManager.queue_jobs: workdir group %d set for node %d is different"
             " from %d assigned for the forerunner node %d in the thread, keeping first"
             " assignment",thread_wd_id,graph->g[threads[i].nodes[j]].nodeid,set_wd_id,graph->g[threads[i].nodes[thread_rnode]].nodeid),0);
        else{
          thread_rnode=j; // node that determines the group
          thread_wd_id=set_wd_id;
        }
      }
      unsigned set_nproc = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.nproc;
      if(set_nproc !=0){
        if(thread_nproc!=0 && set_nproc!=thread_nproc)
          LOGMSG(vblWARN,
             fmt("gmManager.queue_jobs: nproc %d set for node %d is different"
             " from %d assigned for the forerunner node %d in the thread, keeping first"
             " assignment",thread_nproc,graph->g[threads[i].nodes[j]].nodeid,set_nproc,graph->g[threads[i].nodes[thread_rnode]].nodeid),0);
        else
          thread_nproc=set_nproc;        
      }
      unsigned set_ppn = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.ppn;
      if(set_ppn !=0){
        if(thread_ppn!=0 && set_ppn!=thread_ppn)
          LOGMSG(vblWARN,
             fmt("gmManager.queue_jobs: ppn %d set for node %d is different"
             " from %d assigned for the forerunner node %d in the thread, keeping first"
             " assignment",thread_ppn,graph->g[threads[i].nodes[j]].nodeid,set_ppn,graph->g[threads[i].nodes[thread_rnode]].nodeid),0);
        else
          thread_ppn=set_ppn;        
      }
      // walltime is added
      thread_walltime += graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.walltime; 
      // mpi is true for the whole thread if at least one node is mpi
      bool set_mpi = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.mpi;
      if(set_mpi)
        thread_mpi=set_mpi;
      // info is accumulated
      string set_info = graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.info;
      if(set_info!=""){
        if(thread_info!="")
          thread_info+="\n";
        thread_info+=set_info;
      }
      
      // analysing node action
      string command=graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.application_call;
      
      if(command!=""){ // external application
        jobset.push_back( vector<int>(1,j));
        mynode=-1;
      }
      else{ // this application
        if(exetype&gmEXE_EXPLICIT_ONLY){
          if(!graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->useraction.ptr()) // no action defined
            continue; //no need to put a job: node does nothing 
        }

        if(mynode>=0)
          jobset[mynode].push_back(j);
        else
          jobset.push_back( vector<int>(1,j));
        mynode=(int)jobset.size()-1;
      }
    } // end thread nodes loop
    thread_rnode=graph->g[threads[i].nodes[thread_rnode]].nodeid; // recalculating to node id
    if(threads[i].state>=0 && threads[i].state!=2){
      LOGMSG(vblMESS3,fmt("%s (already queued)",(const char *)nodelist.c_str()),0);
      nrunning++;
      continue;
    }
    // select resource
    if((exetype&gmEXE_REMOTE) && !resources.size()){
      LOGMSG(vblWARN,"gmManager.queue_jobs: no external resources defined, switching to local execution!",0);
      exetype &= gmEXE_REMOTE;
    }
    threads[i].state=0; // execution
    threads[i].resource_id=-1;
    LOGMSG(vblMESS3,(const char *)nodelist.c_str(),0);

    if(threads[i].exetype&gmEXE_REMOTE){ // remote (mixed) execution
      threads[i].exetype=gmEXE_REMOTE; 
      // selecting remote resource
      if(thread_resource!=""){ // predefined
        if(names.find(thread_resource)!=names.end())
          threads[i].resource_id=names[thread_resource];
        else
          LOGMSG(vblWARN,fmt("gmManager.queue_jobs: resource '%s' was not added/configured",(const char *)thread_resource.c_str()),0);
      }
      else{  // automatic: select between active resources
        size_t nres=resources.size();
        for(size_t j=0;j<nres;j++){
          if(resources[(j+res_cnt)%nres]->descr.active){ // round-robin
            threads[i].resource_id=(j+res_cnt)%nres;
            res_cnt++;
            break;
          }
        }
      }
      if(threads[i].resource_id==-1){ // no resources found
        if(!(exetype&gmEXE_LOCAL)){
          const char* msg = fmt("gmManager.queue_jobs: no valid remote resources found for the job");
          LOGMSG(vblWARN,msg,0);
          threads[i].state=2; // fail the job 
          threads[i].restarts=-1; // indicates failed thread
          threads[i].error_code = gmERR_RESOURCE;
          threads[i].error_message = msg;
          nrunning++;
          continue;
        }
        else // use local mode
          threads[i].exetype=gmEXE_LOCAL; 
      }
    }

    // here check that all input links exist
    for(size_t j=0;j<threads[i].idlinks.size();j++){
      bool failed_link=false;
      int linktype = graph->links[graph->edgeid[threads[i].idlinks[j]]]->GetType();
      gmdString filename, destname, filename2;
      bool thread_local = (threads[i].exetype==gmEXE_LOCAL || threads[i].exetype==gmEXE_SERIAL); //threads[i].recursive
      if(thread_local && (linktype&gmLINK_DATA) && !(sys->write_files&gmFILES_LOCAL)) // in this case files are not used for data links
        continue; // don't check

      bool trg_local = get_idlink_filenames(graph,threads[i].idlinks[j],filename,destname,filename2,thread_local);
      if(trg_local)  // should have arrived to local filename, otherwise should be accessible by (modified) filename
        filename=destname; 
      
      // checking that filename exists
      
      if((sys->write_files&gmFILES_LOCAL) || exetype&gmEXE_REMOTE || (linktype&gmLINK_FILE)){ // local imitation of file transfer (data link) or file link
        if(!(linktype&gmLINK_OPTDATA)){ // don't check wildcards and optional files
          // checking that the file exists
          if(!gmdFileExists(filename)){
            // checking alternative location
            if(!gmdFileExists(filename2))
              failed_link=true;
            else{
              LOGMSG(vblWARN,fmt("gmManager.queue_jobs: file '%s' is found in alternative location '%s', node locality changed?\n",(const char *)filename.c_str(), (const char *)filename2.c_str()),0);
              if(thread_local) // correcting the local placement
                filename = destname; 
              // need to copy
              LOGMSG(vblMESS3,fmt("Copying file '%s'->'%s'",(const char *)filename2.c_str(),(const char *)filename.c_str()),0);
              int flags = gmJob::CREATEPATH;
              /*if(is_dirname(destname))
                flags|=gmJob::RECURSIVE;
              if(linktype&gmLINK_OPTDATA)
                flags|=gmJob::MAYNOTEXIST;  */
              int res = local_shell->StageIn((const char *)filename2.c_str(),(const char *)filename.c_str(),flags);
              // copy and check the result
              if(res /*!gmdCopyFile(filename,destname)*/ ){
                LOGMSG(vblMESS4,"FAILED",0);
                if(threads[i].state!=2){
                  LOGMSG(vblWARN,fmt("gmManager.queue_jobs: unable to copy file '%s' to destination '%s'",(const char *)filename2.c_str(),(const char *)filename.c_str()),0);
                  failed_link=true; 
                }
              }
              else
                LOGMSG(vblMESS4,"OK",0);          
            }
          }
        }    
      }
      else{ // checking that data exists
         gmLinkData *d=sys->data_rep[(const char *)filename.c_str()];
         if(!d)
           failed_link=true;
      } 
      if(failed_link){ 
        // since there is an output check, this is possible only when the data was produced and then removed
        const char *msg = fmt("gmManager.prepare_links: there is no data for link '%s' produced by processed node %d, restarting source node",(const char *)filename.c_str(),source(threads[i].idlinks[j],graph->g));
        LOGMSG(vblWARN,msg,0);
        threads[i].state=5; // 5; // thread cancelled
        //threads[i].restarts=-2; // try to restart
        threads[i].error_code = gmERR_FILE;
        threads[i].error_message = msg;
        // mark source node as unprocessed
        graph->set_node_state(source(threads[i].idlinks[j],graph->g),gmNS_UNPROC);
        //graph->g[source(threads[i].idlinks[j],graph->g)].flag=gmGraph::UNPROC;
      }
    } 

   
    // check that we have where to copy output to
    for(size_t j=0;j<threads[i].odlinks.size();j++){
      gmdString filename, destname;
      bool thread_local = (threads[i].exetype==gmEXE_LOCAL || threads[i].exetype==gmEXE_SERIAL); //threads[i].recursive
      bool trg_local = get_odlink_filenames(graph,threads[i].odlinks[j],filename,destname , thread_local);       
      // check directory tree 
      if((trg_local||thread_local) && !check_or_create_dir(destname)){
        const char *msg = fmt("gmManager.prepare_links: can't create path for link '%s' that has to be produced by node %d, marking the node as failed",(const char *)destname.c_str(),source(threads[i].odlinks[j],graph->g));
        LOGMSG(vblWARN,msg,0);
        threads[i].state=5; // thread cancelled
        threads[i].restarts=-1; // don't try to restart
        threads[i].error_code = gmERR_FILE;
        threads[i].error_message = msg;
        // mark source node as failed
        graph->set_node_state(source(threads[i].odlinks[j],graph->g),gmNS_FAIL);
        graph->set_node_error(source(threads[i].odlinks[j],graph->g), threads[i].error_code, threads[i].error_message);
      }
    }
     
    if(threads[i].state==5){ // thread cancelled
      threads[i].state=2;
      
      nrunning++;
      continue;
    }
     
    if(threads[i].exetype&gmEXE_REMOTE){ // resource_id must be valid here
      gmResource *res=resources[threads[i].resource_id];
       
      if(thread_wd_id>=0){  // thread in the group
        bool failed=false;
        if(wdgroup[thread_wd_id].jobid!=""){ // run group, compare shells
          if(names.find(wdgroup[thread_wd_id].resource)!=names.end()){
            int wd_resource_id=names[wdgroup[thread_wd_id].resource];
            if(!(res->descr.session==resources[wd_resource_id]->descr.session)){ // not equal
              if(thread_resource!=""){ // selection was not automatic
                const char *msg = fmt("gmManager.queue_jobs:  resources '%s' (node %d) and '%s' (node %d) are "
                  "operated by different sessions and can't belong to same group %d"
                  "marking thread as failed",(const char *)wdgroup[thread_wd_id].resource.c_str(),wdgroup[thread_wd_id].head_nodeid,(const char *)thread_resource.c_str(),thread_rnode);
                LOGERR(-1,msg,0);
                failed=true;
                threads[i].error_code = gmERR_RESOURCE;
                threads[i].error_message = msg;
              }
              else { // automatic, reassigning
                // TODO: make proper selection
                threads[i].resource_id=wd_resource_id;
                res=resources[threads[i].resource_id];
              }
            }
          }
          //else{} // can't find group resource, proceed?
        }
        if(failed){
          threads[i].state=2; // 2 means try to restart
          nrunning++;
          continue;
        }
      }

      gmJob *job=res->pjm->CreateJob();
      // filling the attributes
      job->info= thread_info;      // Extra job info that can be saved into 'info' file in the working directory
      if(thread_nproc)
        job->nproc=thread_nproc;     // total number of processes
      if(thread_ppn)
        job->ppn=thread_ppn;       // number of processes per node
      if(thread_walltime)
        job->walltime=thread_walltime;  // maximum execution wall time in seconds
      job->mpi=thread_mpi;           // if true, indicates that this is an MPI job


      //list data files
      for(size_t j=0;j<threads[i].idlinks.size();j++){
        int linktype = graph->links[graph->edgeid[threads[i].idlinks[j]]]->GetType();
        gmdString filename, destname, filename2;
        get_idlink_filenames(graph,threads[i].idlinks[j],filename,destname,filename2);
        if(!gmdFileExists(filename))// using alternative location (was checked before)
          filename = filename2;
        int flags = gmJob::CREATEPATH;
        if(is_dirname(destname))
          flags|=gmJob::RECURSIVE;
        if(linktype&gmLINK_OPTDATA)
          flags|=gmJob::MAYNOTEXIST; 
        job->AddInFile((const char *)filename.c_str(),(const char *)destname.c_str(),flags);
      }
      for(size_t j=0;j<threads[i].odlinks.size();j++){
        int linktype = graph->links[graph->edgeid[threads[i].odlinks[j]]]->GetType();
        gmdString filename, destname;
        get_odlink_filenames(graph,threads[i].odlinks[j],filename,destname); 
        int flags = gmJob::CREATEPATH;
        if(is_dirname(destname))
          flags|=gmJob::RECURSIVE;
        if(linktype&gmLINK_OPTDATA)
          flags|=gmJob::MAYNOTEXIST;
        job->AddOutFile(destname,filename,flags);        
      }
      
      gmdString progname=res->descr.progname;
      if(progname=="" && sys->_argv && sys->_argv[0]){ // getting current application name
        gmdFileName fn(sys->_argv[0]);
        progname=fn.GetFullPath();
      }
      
      // Forming the command (one job)
      // TODO: make a chain of connected jobs instead
      job->command="";
      if(res->descr.prefix!="")
        job->command += res->descr.prefix+"\n"; 
      for(size_t j=0;j<jobset.size();j++){
        string sub_nodelist="-w"; // forming nodelist
        // first node command: if void then internal
        string command=graph->nodes[graph->g[threads[i].nodes[jobset[j][0]]].nodeid]->userprop.application_call;
        if(command==""){ // using internal nodes
          
          for(size_t k=0;k<jobset[j].size();k++){
            sub_nodelist+=fmt("%d",graph->g[threads[i].nodes[jobset[j][k]]].nodeid);
            if(k!=jobset[j].size()-1)
              sub_nodelist+=":";
          }

          if(progname==""){
            LOGMSG(vblWARN,fmt("gmManager.queue_jobs: GridMD program name is empty for remote internal nodes (%s), execution skipped",(const char *)sub_nodelist.c_str()),0);
            continue;
          }

          job->command += ""
            +res->descr.progdir+progname+" "
            +(const char *)sub_nodelist.c_str();
          // adding original program arguments
          for(int k=1;k<sys->_argc;k++){
            job->command+=" ";
            job->command+=sys->_argv[k];
          }
          job->command+="\n";
        }
        else{
          //command = res->descr.locate_application(command); // find resource specific command
          job->command += command+"\n";
        }
      }
      if(res->descr.postfix!="")
        job->command += res->descr.postfix + "\n"; 

      if(thread_wd_id>=0){  // thread in the group
        if(wdgroup[thread_wd_id].jobid!="") // run group
          job->forerunner=wdgroup[thread_wd_id].jobid; // linking
      }

      if(!jobset.size()){ // checking if this is a virtual node, setting dummy flag
        LOGMSG(vblMESS3,fmt("Thread contains virtual nodes only"),0);
        job->type=gmJob::DUMMY;
      }

      int state = job->Submit(*(res->pjm), (const char *)sys->name.c_str());
      LOGMSG(vblMESS3,fmt("Job submitted to '%s', id = %s, state = %s",(const char *)res->name/*descr.session.host*/.c_str(), (const char *)job->GetID().c_str(), gmJob::StateName(state)),0);
      if(state==JOB_FAILED){
        const char * msg = fmt("gmManager.queue_jobs:  job submission to '%s' failed, marking thread as failed",(const char *)res->name/*descr.session.host*/.c_str());
        LOGERR(-1,msg,0);
        threads[i].state=2;
        
        threads[i].error_code = classify_jm_error(res->pjm.ptr(),gmERR_EXECUTION); // TODO: classify errors properly
        threads[i].error_message = msg;
        nrunning++;

        // if Submit fails we must delete job manually !!!
        delete job;
      }
      else{
        threads[i].jobid=(const char *)job->GetID().c_str();
        for(size_t j=0;j<threads[i].nodes.size();j++) // marking nodes as executing
          graph->set_node_state(threads[i].nodes[j],gmNS_EXE);

        if(thread_wd_id>=0){  // thread in the group
          threads[i].persistent= true; // no deletion
          if(wdgroup[thread_wd_id].jobid==""){ // unrun group, saving data
            wdgroup[thread_wd_id].jobid=job->GetID();
            wdgroup[thread_wd_id].resource=res->name;
            wdgroup[thread_wd_id].head_nodeid=thread_rnode;
          }
        }
        nrunning++;
      }
      //job.Detach();
      
    } // remote
    if(threads[i].exetype&gmEXE_LOCAL && threads[i].resource_id==-1){ // local and not assigned yet
      threads[i].exetype=gmEXE_LOCAL; 
      LOGMSG(vblMESS3,"locally",0);
    }
    else if(threads[i].exetype==gmEXE_SERIAL){
      LOGMSG(vblMESS3,"locally and serially",0);
      threads[i].state=0; // mark this thread as executing
    }

    if(/*threads[i].exetype==gmEXE_SERIAL ||*/ threads[i].exetype==gmEXE_LOCAL){ // performing associated actions, for serial threads they are executed in process_curnode()
      //threads[i].state=1; // mark this thread as finished 
      threads[i].state=0; // mark this thread as executing
      
      if(exetype&gmEXE_EXPLICIT_ONLY){
        //HERE execute local nodes and actions
        for(size_t j=0;j<jobset.size() /*threads[i].nodes.size()*/;j++){
          int res=0;
          for(size_t k=0;k<jobset[j].size();k++){
            int headid=jobset[j][k];
            // local node action
            gmNodeAction *action=graph->nodes[graph->g[threads[i].nodes[headid]].nodeid]->useraction.ptr();
            if(action){
              res=action->OnExecuteWrapped();
              if(res<0){ // failed
                const char *msg = fmt("gmManager.queue_jobs: local function of node %d retuned fail status %d, marking thread as failed",
                              graph->g[threads[i].nodes[headid]].nodeid,res);
                LOGERR(-1,msg,0);
                threads[i].state = 2; // try to restart
                threads[i].error_code = gmERR_EXECUTION;
                threads[i].error_message = msg;
                break;
              }
						  else if(res==gmREDO) // redo requested
							  threads[i].needs_redo = 1;
						  else
                threads[i].needs_redo = 0;
            }

            // it can also be application call:
            string command=graph->nodes[graph->g[threads[i].nodes[headid]].nodeid]->userprop.application_call;
            if(command!=""){
              res = system((const char *)command.c_str());  // passes environment
              //res = (int)gmdExecute((const char *)command.c_str(),gmdEXEC_SYNC);
              if(res!=0){ // failed
                const char *msg = fmt("gmManager.queue_jobs: local application call of node %d exited with status %d, marking thread as failed",
                              graph->g[threads[i].nodes[headid]].nodeid,res);
                LOGERR(-1,msg,0);
                threads[i].state=2;
                threads[i].error_code = gmERR_EXECUTION;
                threads[i].error_message = msg;
                break;
              }
            }
          } // k
        }
      }
      else{ // recursion mechanism to execute associated code blocks, local actions are also executed in recursive calls by process_curnode()
        
        //threads[i].state=1; // mark this thread as finished 
        threads[i].state=0; // mark this thread as executing
        threads[i].recursive = true; // mark this thread as executed recursively
        // inserting working node list into argumants
        char **myargv= new char *[sys->_argc+1];
        myargv[0]= sys->_argv ? sys->_argv[0] : (char*) ""; // if _argv is NULL, then _argc must be 1 (see init)
        myargv[1]=(char *)nodelist.c_str();
        for(int k=2;k<sys->_argc+1;k++)
          myargv[k]=sys->_argv[k-1];

        /*
        if((sys->write_files&gmFILES_LOCAL) || exetype&gmEXE_REMOTE){ // local imitation of file transfer or mixed mode
          //list data files
          for(size_t j=0;j<threads[i].idlinks.size();j++){
            gmdString filename=graph->links[graph->edgeid[threads[i].idlinks[j]]]->GetName();
            gmdString destname=graph->links[graph->edgeid[threads[i].idlinks[j]]]->GetDestName();
            LOGMSG(vblMESS3,fmt("Copying file '%s'->'%s'",(const char *)filename.c_str(),(const char *)destname.c_str()),0);
            gmdCopyFile(filename,destname);
          }   
        }*/

        if(exetype&gmEXE_THREADS){  // using thread pool
          LOGMSG(vblMESS3,"using threads",0);
          if(!threadpool){ // inbitializing threadpool
            threadpool = new gmdThreadPool;
 # if USING_GMTHREADS
            // global gmExpRedirector will take care that each tread works with its own copy of global gmManager
            threadpool->RegisterRedirector(&gmExpRedirector);
# endif
          }
          threads[i].taskid = threadpool->CreateGMMainTask(gridmd_main,sys->_argc+1,myargv);
          //threadpool->TaskResult(threads[i].taskid);
        }
        else{ // direct recursive call
          LOGMSG(vblMESS3,"recursively",0);
          // saving counters
          int idcount_tmp = sys->idcount;
          int have_start_node_tmp = sys->have_start_node; 

          sys->rec_level++;
          try{
            // argc, argv define the subgraph
            gridmd_main(sys->_argc+1,myargv); // executes nodes locally
          }
          catch(task_finished){} // terminated because maximal node is reached
          /*if((sys->write_files&gmFILES_LOCAL)){ // moving local links to their destinations
            for(size_t j=0;j<threads[i].odlinks.size();j++){
              gmdString filename, destname;
              bool trg_local = get_odlink_filenames(graph,threads[i].odlinks[j],filename,destname , false);  // simulating nonlocal thread
              LOGMSG(vblMESS3,fmt("Moving file '%s'->'%s'",(const char *)filename.c_str(),(const char *)destname.c_str()),0);
              if(gmdCopyFile(filename,destname)){
                gmdRemoveFile(filename);
              }
            }
          }*/
          sys->rec_level--;
        
          // restoring
          sys->idcount = idcount_tmp;
          sys->have_start_node = have_start_node_tmp; 
        }

        delete [] myargv;
      }
      nrunning++;
    } //local
    else if(threads[i].exetype==gmEXE_SERIAL){
      nrunning++;
    }
  }
  return nrunning;
}


//void gmScheduler::check_thread(vector<gmGraph::wthread_t> &threads, size_t i, size_t& nfinished, size_t &nfailed){
//}


// returns the number of newly finished jobs
size_t gmScheduler::check_jobs(gmGraph *graph, size_t *pfailed, int thread_id, bool stop, bool force_fetch){
  vector<gmGraph::wthread_t> &threads=graph->threads;
  if(pfailed)
    *pfailed=0;
  if(!(threads.size()/*-graph->thread_start*/))
    return 0;
  /*if(exetype!=gmEXE_LOCAL && !resources.size()){
    LOGMSG(vblWARN,"gmManager.check_jobs: no external resources defined, switching tom local execution!",0);
    exetype=gmEXE_LOCAL;
  }*/
  size_t nfinished=0;
  size_t nfailed=0;
  bool found_finish = false; // indicates that local finish node is among the excuting nodes
  for(size_t i=0/*graph->thread_start*/;i<threads.size();i++){
    if(thread_id>=0 && (int)i!=thread_id) // select given thtread id only
      continue;

    if(threads[i].state==1 || threads[i].state>=3) // finished or failed
      continue;
    //threads[i].read = false; // we are going to modify the thread
    std::string cur_jobid = threads[i].jobid;
    if(threads[i].state==0 && threads[i].exetype!=gmEXE_LOCAL && threads[i].exetype!=gmEXE_SERIAL){
      gmJobManager *jm=resources[threads[i].resource_id]->pjm.ptr();
      //JobList joblist = jm->RestoreAll((const char *)threads[i].jobid.c_str());
      gmJob *pjob=jm->JobByID((const char *)threads[i].jobid.c_str());
      int state;
      if(!pjob){ // trying to restore
        // TODO: restore all jobs from this task here
        pjob=jm->Restore((const char *)threads[i].jobid.c_str());
        if(!pjob){ // mark job as failed
          const char *msg = fmt("gmManager.check_jobs: unable to restore the job with id = %s, marking as failed!", (const char *)threads[i].jobid.c_str());
          LOGMSG(vblWARN,msg,0);
          threads[i].state=2;
          threads[i].error_code = classify_jm_error(jm,gmERR_RESOURCE);
          threads[i].error_message = msg;
          continue;
        }
        //list data files
        for(size_t j=0;j<threads[i].odlinks.size();j++){ // restoring the output files
          int linktype = graph->links[graph->edgeid[threads[i].odlinks[j]]]->GetType();
          gmdString filename, destname;
          get_odlink_filenames(graph,threads[i].odlinks[j],filename,destname);
          int flags = gmJob::CREATEPATH;
          if(is_dirname(destname))
            flags|=gmJob::RECURSIVE;
          if(linktype&gmLINK_OPTDATA)
            flags|=gmJob::MAYNOTEXIST;
          pjob->AddOutFile(destname,filename,flags);
          //gmdString filename=graph->links[graph->edgeid[threads[i].odlinks[j]]]->GetName();
          //pjob->AddOutFile((const char *)filename.c_str(),(const char *)filename.c_str());
        }
        state=pjob->LastState();
      }
      
      
      if(stop){
        state = pjob->Stop();
        LOGMSG(vblMESS3,fmt("Job %s: state = %s (after stopping the job)", (const char *)pjob->GetID().c_str(), gmJob::StateName(state)),0);  
        threads[i].stopped=true;
      }
      else{
        state=pjob->GetState();
        LOGMSG(vblMESS3,fmt("Job %s: state = %s", (const char *)pjob->GetID().c_str(), gmJob::StateName(state)),0);  
      }

      bool clear_job = false;

      if(state==JOB_COMPLETED){ // fetching results (change for remote files here)
        state=pjob->FetchResult();
        if(state==JOB_FAILED){
          const char *msg = fmt("gmManager.check_jobs: unable to fetch results for the job with id = %s, marking as failed!", (const char *)pjob->GetID().c_str());
          LOGMSG(vblWARN,msg,0);
          threads[i].state=2;
          threads[i].error_code = classify_jm_error(jm,gmERR_EXECUTION); // gmERR_FILE ?
          threads[i].error_message = msg;
        }
        else{ 
          threads[i].state=1;
          
          if(threads[i].persistent) // adding to persistent jobs for future cleanup
            resources[threads[i].resource_id]->persistent_jobs.push_back(threads[i].jobid);
					else
						clear_job = true; // marking for clearing
          
        }
      }
      else if(state==JOB_FAILED){
        if(!stop){
          const char * msg = fmt("gmManager.check_jobs: the job with id = %s is reported as failed by JobMnager!", (const char *)pjob->GetID().c_str());
          LOGMSG(vblWARN,msg,0);
          threads[i].error_code = classify_jm_error(jm,gmERR_EXECUTION);
          threads[i].error_message = msg;
        }
        threads[i].state=2;
      }
      else if(force_fetch) // just fetching
        pjob->FetchResult();

			gmJobTiming timing;
			if(pjob->GetTiming(timing)>=0){
				for(size_t j=0;j<threads[i].nodes.size();j++) // setting node timings 
					graph->nodes[graph->g[threads[i].nodes[j]].nodeid]->userprop.timing = timing;
			}

			
      if(state==JOB_FAILED){ // staging out stderr and stdout 
        pjob->FetchResult(); // Force download output(s). Log file(s) may be useful to detecting error reason.

        pjob->ClearOutFiles();
				gmdString stdo("remote.stdout."), stde("remote.stderr.");
				stdo+=threads[i].jobid;
        stde+=threads[i].jobid;
        pjob->AddOutFile(stdo.c_str(),"STDOUT");
        pjob->AddOutFile(stde.c_str(),"STDERR");
        pjob->FetchResult();
        char buffer[1001];
        FILE *f=fopen(stdo.c_str(),"rt");
        if(f){ // copying max 1000 characters of stdout
          size_t n=fread(buffer,sizeof(char),1000,f);
          buffer[n]=0;
          threads[i].stdout_=buffer;
          fclose(f);
          if(n!=0)
            LOGMSG(vblWARN,fmt("job stdout: %s\n" ,threads[i].stdout_.c_str()),0);
          if(sys->write_files&gmFILES_CLEANUP)
            gmdRemoveFile(stdo.c_str());
        }
        f=fopen(stde.c_str(),"rt");
        if(f){ // copying max 1000 characters of stderr
          size_t n=fread(buffer,sizeof(char),1000,f);
          buffer[n]=0;
          threads[i].stderr_=buffer;
          fclose(f);
          if(n!=0)
            LOGMSG(vblWARN,fmt("job stderr: %s\n" ,threads[i].stderr_.c_str()),0);
          if(sys->write_files&gmFILES_CLEANUP)
            gmdRemoveFile(stde.c_str());
        }
        // here we might clear the failed job
        //pjob->Clear(); 
        //OR adding to persistent jobs for future cleanup
        
        if((threads[i].error_code == gmERR_EXECUTION || stop) && !threads[i].persistent){
          clear_job = true; // marking for clearing
        }
        else{
          threads[i].persistent=true; // set this flag to make cleanup by restart if necessary
          resources[threads[i].resource_id]->persistent_jobs.push_back(threads[i].jobid);
        }
        //if(stop)
          //threads[i].jobid=""; // clearing jobid to not attempt to restart by reading restart file
      }

      if(clear_job){
        threads[i].jobid=""; // indicates that the job is cleared
        pjob->Clear(); // only if there are no remote files
      }


    }
    else if(threads[i].state==0 && (threads[i].exetype==gmEXE_LOCAL || threads[i].exetype==gmEXE_SERIAL)){
      if(threads[i].exetype==gmEXE_LOCAL){
        if(threads[i].taskid==GMPOOLTASK_INVALID_ID) // this job was not run as thread from thread pool
          threads[i].state=1; // in this case local threads arrive here being actually finished 
        else{ 
           gmTASK_STATUS status = threadpool->TaskStatus(threads[i].taskid);
           if(status != gmTASK_FINISHED)
             continue; // wait till the thread finishes
           threads[i].state=1; // mark as finished
           int exit_code = threadpool->TaskResult(threads[i].taskid);
           if(exit_code!=0){
             //threads[i].state=2; // failing the thread  (don't)
             const char *msg = fmt("gmManager.check_jobs: thread job (thread %d) is reported as failed by thread pool (exit code %d)",i,exit_code);
             LOGMSG(vblWARN,msg,0);  
           }
        }    
      }
      // check the files
      for(size_t j=0;j<threads[i].odlinks.size();j++){ // checking the output files
        int linktype = graph->links[graph->edgeid[threads[i].odlinks[j]]]->GetType();
        if((linktype&gmLINK_DATA) && !(sys->write_files&gmFILES_LOCAL)) // in this case files are not used for data links
          continue; 
        
        gmdString filename, destname;
        bool thread_local = (threads[i].exetype==gmEXE_LOCAL || threads[i].exetype==gmEXE_SERIAL); //threads[i].recursive
        bool trg_local=get_odlink_filenames(graph,threads[i].odlinks[j],filename,destname,thread_local);
        bool failed_link=false;
        if(!(linktype&gmLINK_OPTDATA)){// don't check wildcards and optional files
          if(!gmdFileExists(filename)){
            failed_link=true;
            LOGMSG(vblWARN,fmt("gmManager.check_jobs: missing output file '%s' of thread %d",(const char *)filename.c_str(),i),0);
          }
        }
        if( (trg_local||thread_local) && !failed_link && filename!=destname){ // need to copy
          LOGMSG(vblMESS3,fmt("Copying file '%s'->'%s'",(const char *)filename.c_str(),(const char *)destname.c_str()),0);
          int flags = gmJob::CREATEPATH;
          if(is_dirname(destname))
            flags|=gmJob::RECURSIVE;
          if(linktype&gmLINK_OPTDATA)
            flags|=gmJob::MAYNOTEXIST;  /*,gmShell::RECURSIVE*/
          int res = local_shell->StageIn((const char *)filename.c_str(),(const char *)destname.c_str(),flags);
          // copy and check the result
          if(res /*!gmdCopyFile(filename,destname)*/ ){
            LOGMSG(vblMESS4,"FAILED",0);
            if(threads[i].state!=2){
              LOGMSG(vblWARN,fmt("gmManager.check_jobs: unable to copy file '%s' to destination '%s'",(const char *)filename.c_str(),(const char *)destname.c_str()),0);
              failed_link=true; 
            }
          }
          else
            LOGMSG(vblMESS4,"OK",0);          
        }
        if(failed_link && threads[i].state!=2){
          threads[i].state=2; // failing the thread
          const char *msg = fmt("gmManager.check_jobs: the local job (thread %d) is failed by missing output link",i);
          LOGMSG(vblWARN,msg,0); 
          threads[i].error_code = gmERR_FILE;
          threads[i].error_message = msg;
          
        }
      }
    }
    if(threads[i].exetype==gmEXE_SERIAL){ // finish node found
      if(threads[i].state==0 || threads[i].state==2){ // any unfinished serial thread (even failed) must exit with throw
        if(!found_finish){ 
          sys->wnodes.clear();
          sys->wnodes.start_record();
          sys->max_node=-1;
          found_finish = true; // mark for throw task_finished()
        }
      }
      if(threads[i].state==0){ // executing: collect the node list
        for(size_t j=0;j<threads[i].nodes.size();j++){
          int inode=sys->graph->g[threads[i].nodes[j]].nodeid;
          sys->wnodes.next_record(inode);
          if(sys->max_node<inode)
            sys->max_node=inode;
          // marking nodes as processed
          sys->graph->set_node_state(threads[i].nodes[j],gmNS_PROC);
          //sys->graph->set_node_state(threads[i].nodes[j],gmNS_UNPROC);
          //sys->graph->g[threads[i].nodes[j]].flag=gmGraph::PROC;
        }
        sys->wnodes.end_record();
        //threads[i].state=0; // executing, to be analyzed by check_jobs
        //throw task_finished(); // this must be processed in local mode
        threads[i].state=1;  // mark as finished
      }
    }
    
    if(threads[i].state>0){ // finished or failed
      nfinished++;
      int was_restart=0; 
      if(threads[i].state==2){ // candidate to fail: checking wether we can restart this thread
        if(threads[i].restarts>=0 && !stop && !threads[i].stopped){ // stop leads to thread failure
          int restart_lim=0;
          for(size_t j=0;j<threads[i].nodes.size();j++){ // marking nodes as processed
            int max_restarts=graph->g[threads[i].nodes[j]].max_restarts>=0 ? graph->g[threads[i].nodes[j]].max_restarts : sys->max_node_restarts;
            if(restart_lim<max_restarts)
              restart_lim=max_restarts;
          }

          threads[i].restarts++;
          if(threads[i].restarts>restart_lim){
            threads[i].state=3; // limit reached, no restart possible
            LOGMSG(vblWARN,fmt("gmManager.check_jobs: maximum number of restarts for a job reached, cancelling the thread."),0);
          }
          else 
            was_restart=1;
        }
        else
          threads[i].state=3; // job is unconditionally failed
        if(threads[i].state==3 && threads[i].exetype==gmEXE_SERIAL) // job is unconditionally failed and we proceed serially
          sys->graph_error = 1; // set graph error to indicate that something failed in serial mode 
      }
      // setting node states if it was not a file thread
      //if(!threads[i].read){
        for(size_t j=0;j<threads[i].nodes.size();j++){ // marking nodes as processed
				  if(threads[i].state==1){ // finished correctly
            if(threads[i].needs_redo) // some of the nodes requested rerun
						  graph->set_node_state(threads[i].nodes[j],gmNS_UNPROC, true);
					  else
					    graph->set_node_state(threads[i].nodes[j],gmNS_PROC);
				  }
          else if(threads[i].state==3){// state==3, absolutely failed
            if(threads[i].restarts==-2) // cancelled thread
              graph->set_node_state(threads[i].nodes[j],gmNS_UNPROC);
            else {
              graph->set_node_state(threads[i].nodes[j],gmNS_FAIL);
              graph->set_node_error(threads[i].nodes[j],threads[i].error_code,threads[i].error_message); // translating error to node
              nfailed++;
            }
          }
        }
      //}

      if(was_restart){
        LOGMSG(vblWARN,fmt("gmManager.check_jobs: attempting to rerun the job with id = %s after a failure!", (const char *)cur_jobid.c_str()),0);
        threads[i].jobid="";  // must indicate a duplicate thread here
        if(threads[i].exetype==gmEXE_SERIAL) // if the thread is restarting, cancel going to serial mode
          found_finish = false; // cancel throwing task_finished(), resubmit this thread
      }
      else{
        // removing the finished thread from nodal list
        for(size_t j=0;j<threads[i].nodes.size();j++){
          vector<int> &nodthreads=graph->g[threads[i].nodes[j]].threads;
          size_t &nodstart=graph->g[threads[i].nodes[j]].thread_start;
          for(size_t k=nodstart;k<nodthreads.size();k++){
            if(nodthreads[k]==i){
              swap(nodthreads[nodstart],nodthreads[k]);
              nodstart++;
              break;
            }
          }
        }
      }
    }
  }
  // marking nodes of restarting threads as EXE (regardless of what was marked by intersecting threads)
  for(size_t i=0/*graph->thread_start*/;i<threads.size();i++){
    if(threads[i].state==2){ // restarting
       for(size_t j=0;j<threads[i].nodes.size();j++) // marking nodes as executing
         graph->set_node_state(threads[i].nodes[j],gmNS_EXE);
    }
  }
  // analyse the cleanup list
  //...
  
  if(found_finish)
    throw task_finished(); // this must be processed in serial worker mode


  if(pfailed)
    *pfailed=nfailed;
  return nfinished;
}

int gmScheduler::fetch_workdir_files(int groupid, const std::vector< pair<std::string, std::string> > &files){ 
  // checking id
  if((int)wdgroup.size()<=groupid)
    return LOGERR(-2,fmt("gmManager.fetch_workdir_files: workdir group %d does not exist",groupid),0); // no such group
  // checking that this group has started
  if(wdgroup[groupid].jobid=="") // not yet run
    return -1;
  // checking that the resource is defined
  gmResource *res=NULL;
  if(names.find(wdgroup[groupid].resource)!=names.end())
    res=resources[names[wdgroup[groupid].resource]];
  else
    return LOGERR(-3,fmt("gmManager.fetch_workdir_files: resource '%s' is not defined for workdir group %d",(const char *)wdgroup[groupid].resource.c_str(), groupid),0); // no such group
  
  // restoring job to transfer files
  gmJob *job=res->pjm->JobByID((const char *)wdgroup[groupid].jobid.c_str());
  if(!job){ // trying to restore
    job = res->pjm->Restore((const char *)wdgroup[groupid].jobid.c_str());
    if(!job)
      return LOGERR(-4,fmt("gmManager.fetch_workdir_files: can't restore job '%s' from resource '%s'",(const char *)wdgroup[groupid].jobid.c_str(),(const char *)wdgroup[groupid].resource.c_str(), groupid),0); // no such group
  }

  return stage_out_files(job,files);
}

int gmScheduler::stage_out_files(gmJob *job, const std::vector< pair<std::string, std::string> > &files){
    //staging out files
  int nfiles=0;
  for(size_t j=0;j<files.size();j++){
    std::string dest=files[j].second=="" ? files[j].first : files[j].second;
    LOGMSG(vblMESS4,fmt("Staging out file '%s' --> '%s'",(const char *)files[j].first.c_str(),(const char *)dest.c_str()),0);
    if(!job->StageOut((const char *)dest.c_str(),(const char *)files[j].first.c_str())){
      LOGMSG(vblMESS4,"OK",0);
      nfiles++;
    }
    else
      LOGMSG(vblMESS4,"FAILED",0);
  }
  return nfiles;
}


int gmScheduler::fetch_node_files(gmGraph *graph, int nid, const std::vector< pair<std::string, std::string> > &files){
  gmGraph::graph_t::vertex_descriptor vd=graph->vdnodes[nid];
  vector<int> &nodthreads=graph->g[vd].threads;
  int hits =0;
  map< pair<string, int>, bool>  found_jobs;
  for(size_t i=0;i<nodthreads.size();i++){
    int threadid = nodthreads[i];
    std::string cur_jobid = graph->threads[threadid].jobid;
    if(cur_jobid=="")
      continue;
    int res_id = graph->threads[threadid].resource_id;
    if(res_id<0 || res_id>= (int)resources.size())
      continue;

    if(found_jobs.find(make_pair(cur_jobid,res_id))== found_jobs.end()) // not found
      found_jobs[make_pair(cur_jobid,res_id)] = true;
    else
      continue; // this job already processed

    gmJobManager *jm=resources[res_id]->pjm.ptr();
    gmJob *pjob=jm->JobByID((const char *)cur_jobid.c_str());
    if(!pjob) // trying to restore
      pjob=jm->Restore((const char *)cur_jobid.c_str());
    if(!pjob)
      continue;
     hits+=stage_out_files(pjob,files);
  }
  return hits; 
}


int gmScheduler::stop_node_threads(gmGraph *graph, int nid, bool fetch_results){
  gmGraph::graph_t::vertex_descriptor vd=graph->vdnodes[nid];
  vector<int> &nodthreads=graph->g[vd].threads;
  int hits =0;
  for(size_t i=0;i<nodthreads.size();i++){
    size_t nstop=0;
    check_jobs(graph,&nstop,nodthreads[i],true,fetch_results);
    hits+=(int)nstop;
  }
  return hits; 
}


size_t gmResource::clean_jobs(){
  if(!pjm.ptr())
    return 0;
  size_t ncleaned=0;
  for(size_t i=0;i<persistent_jobs.size(); i++){
    gmJob *pjob=pjm->JobByID((const char *)persistent_jobs[i].c_str());
    if(!pjob){ // trying to restore
      pjob=pjm->Restore((const char *)persistent_jobs[i].c_str());
      if(!pjob){ // no job found
        LOGMSG(vblWARN,fmt("gmResource.clean_jobs: unable to restore the job with id = %s, skipping!", (const char *)persistent_jobs[i].c_str()),0);
        continue;
      }
    }
    int state=pjob->GetState();
    LOGMSG(vblMESS3,fmt("Clearing job %s: state = %s", (const char *)pjob->GetID().c_str(), gmJob::StateName(state)),0);     
    pjob->Clear(); // clean all jobs
    ncleaned++;
  }
  persistent_jobs.clear(); // clearing vector
  return ncleaned;
}

/*

int gmCheckDirTree(const gmdString &filename, bool makeall){
  gmdFileName rname(filename);
  
  gmdArrayString rdirs=rname.GetDirs();
  
  gmdArrayString accdir;
  int n=rdirs.Count(),i;
  gmdString str;
  if(rname.HasVolume())
    str=rname.GetVolume()+rname.GetVolumeSeparator()+"/";
  else{
    if(rname.IsAbsolute())
      str="/";
  }    
  for(i=0;i<n;i++){
    str+=rdirs[i]+'/';
    accdir.Add(str);
  }

  for(i=n-1;i>=0;i--){
    if(gmdDirExists(accdir[i]))break;
  }
  if(i!=n-1){
    if(makeall){
      for(i++;i<n;i++){ // creating directories
        if(!gmdMkdir(accdir[i]))return -i;
      }
    }
    else{
      i++;
      return -i;
    }
  }
  return 1;
}*/


