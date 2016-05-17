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
 *   $Revision: 1.77 $
 *   $Date: 2016/02/08 08:16:59 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gridmd.h,v 1.77 2016/02/08 08:16:59 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gridmd.h,v $
$Revision: 1.77 $
$Author: valuev $
$Date: 2016/02/08 08:16:59 $
*/
/*s****************************************************************************
 * $Log: gridmd.h,v $
 * Revision 1.77  2016/02/08 08:16:59  valuev
 * working examples
 *
 * Revision 1.76  2016/02/05 19:04:45  valuev
 * implicit workflow fixes
 *
 * Revision 1.75  2016/02/04 16:41:04  valuev
 * serial final job execution
 *
 * Revision 1.74  2016/02/03 16:41:18  valuev
 * fixes
 *
 * Revision 1.73  2016/02/02 12:15:34  valuev
 * fixing mutex
 *
 * Revision 1.72  2015/07/17 18:23:05  valuev
 * working with vs8
 *
 * Revision 1.71  2015/06/17 19:37:59  valuev
 * sweep farm restructured
 *
 * Revision 1.70  2015/05/12 13:53:45  valuev
 * new version with all functions in read node
 *
 * Revision 1.69  2015/04/10 13:18:48  valuev
 * updated state write
 *
 * Revision 1.68  2015/03/27 18:23:15  valuev
 * edges
 *
 * Revision 1.67  2015/03/20 15:46:36  valuev
 * node name changes
 *
 * Revision 1.66  2015/03/17 13:51:36  valuev
 * some fixes for scheduling
 *
 * Revision 1.65  2015/03/16 16:10:20  valuev
 * working on sweep farm
 *
 * Revision 1.64  2015/02/02 11:49:52  valuev
 * iccs2015
 *
 * Revision 1.63  2015/01/30 09:08:04  valuev
 * working on sweeps
 *
 * Revision 1.62  2014/10/28 10:50:47  valuev
 * adapting tcpengine for AWPMD
 *
 * Revision 1.61  2014/09/26 16:57:56  valuev
 * one more sync
 *
 * Revision 1.60  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.59  2013/07/04 14:06:24  valuev
 * extended file management
 *
 * Revision 1.58  2013/06/18 13:55:42  valuev
 * changed temporary names algorithm
 *
 * Revision 1.57  2013/05/31 17:46:11  valuev
 * added wildcard links
 *
 * Revision 1.56  2013/05/27 17:00:35  valuev
 * added conversion to const char * for wxWidgets UNICODE
 *
 * Revision 1.55  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.54  2013/03/07 17:08:59  valuev
 * kintech xml
 *
 * Revision 1.53  2013/02/26 16:44:18  valuev
 * changed job manager parameters interface
 *
 * Revision 1.52  2012/12/28 17:14:30  valuev
 * added node state callbacks, fixed restart
 *
 * Revision 1.51  2012/12/24 17:01:58  valuev
 * fixed local link check
 *
 * Revision 1.50  2012/12/21 16:23:05  valuev
 * unified node states
 *
 * Revision 1.49  2012/12/12 17:28:00  valuev
 * added virtual nodes and dummy jobs
 *
 * Revision 1.48  2012/12/11 17:24:36  valuev
 * updated scheduling, node state changes on the fly
 *
 * Revision 1.47  2012/12/04 17:10:31  valuev
 * added windows job manager, implemented fetching workdir files, fixed file renaming after transfer
 *
 * Revision 1.46  2012/11/30 18:37:14  valuev
 * modified file handling
 *
 * Revision 1.45  2012/11/29 18:14:48  valuev
 * sync with svn
 *
 * Revision 1.44  2012/11/22 17:01:06  valuev
 * sync with kintech svn
 *
 * Revision 1.43  2012/11/21 17:06:15  valuev
 * sync with kintech svn
 *
 * Revision 1.42  2012/11/08 14:32:50  valuev
 * various extensions
 *
 * Revision 1.41  2012/11/07 22:13:54  valuev
 * grouped nodes
 *
 * Revision 1.40  2012/11/06 18:05:43  valuev
 * various small fixes, sync with svn
 *
 * Revision 1.39  2012/11/06 11:35:41  valuev
 * added resource per node specification
 *
 * Revision 1.38  2012/10/29 11:10:14  valuev
 * mdified queueing (not working)
 *
 * Revision 1.37  2012/10/25 14:15:04  valuev
 * updated explicit workflow
 *
 * Revision 1.36  2012/10/24 17:05:00  valuev
 * working explicit workflow and example
 *
 * Revision 1.35  2012/10/23 17:04:33  valuev
 * adding node actions
 *
 * Revision 1.34  2012/10/23 10:44:56  valuev
 * node action
 *
 * Revision 1.33  2012/10/04 16:37:45  valuev
 * sync with KINTECH svn
 *
 * Revision 1.32  2012/09/07 14:26:06  valuev
 * removed using namespace from h files
 *
 * Revision 1.31  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 * Revision 1.30  2012/01/17 06:00:07  valuev
 * neb workflow
 *
 * Revision 1.29  2011/01/25 10:07:35  valuev
 * some new fixes
 *
 * Revision 1.28  2010/11/20 18:08:22  valuev
 * added GridMD implementation to tcpengine
 *
 * Revision 1.27  2010/11/17 02:17:16  valuev
 * added file links
 *
 * Revision 1.26  2010/10/14 16:20:29  valuev
 * added link file cleanup
 *
 * Revision 1.25  2010/10/14 15:02:29  morozov
 * Pre-release 2.1 (windows version). Corrected documentation and examples.
 *
 * Revision 1.24  2010/10/14 09:52:49  valuev
 * added link data analysis and node restart on failed links
 *
 * Revision 1.23  2010/10/12 17:45:47  valuev
 * added documentation, some cleanup
 *
 * Revision 1.22  2010/10/12 10:14:34  valuev
 * fixed thread analysis deadlock
 *
 * Revision 1.21  2010/10/07 16:03:44  valuev
 * fixed restart mechanism
 *
 * Revision 1.20  2010/10/07 15:43:13  morozov
 * Template instances node_input and node_output are moved out of gmMamager scope to fulfil GCC requirements
 *
 * Revision 1.19  2010/10/07 15:35:15  morozov
 * Fixed __int32 compatibility problem
 *
 * Revision 1.18  2010/10/07 11:20:31  valuev
 * preliminary program restart
 *
 * Revision 1.17  2010/10/06 14:01:50  valuev
 * Compiled with icc
 *
 * Revision 1.16  2010/10/06 13:51:58  valuev
 * added xml graph, fixed memory leaks in JobManager
 *
 * Revision 1.15  2010/10/01 09:13:51  valuev
 * added resource loading from xml
 *
 * Revision 1.14  2010/10/01 07:01:04  valuev
 * added xml resource saving
 *
 * Revision 1.13  2010/09/17 19:36:18  valuev
 * changed fork function names, added regular expressions
 *
 * Revision 1.12  2010/09/17 12:24:43  valuev
 * updated fork
 *
 * Revision 1.11  2010/09/15 17:54:15  valuev
 * added fork skeleton
 *
 * Revision 1.10  2010/08/10 17:15:16  morozov
 * Release 2.0
 *
 * Revision 1.8  2010/08/07 09:29:36  valuev
 * modified/added documentation
 *
 * Revision 1.7  2010/08/07 08:50:23  valuev
 * added resource types
 *
 * Revision 1.6  2010/08/05 14:43:00  morozov
 * simple_workflow example for the release 2.0
 *
 * Revision 1.5  2010/08/05 07:04:57  valuev
 * fixed remote job submission
 *
 * Revision 1.4  2010/07/31 16:12:04  valuev
 * modified workflow interface
 *
 * Revision 1.3  2010/07/28 17:54:11  morozov
 * Fixed doxygen documantation. Added gmShellUnix communication protocol.
 *
 * Revision 1.2  2010/07/28 14:37:00  valuev
 * Added documentation to workflow
 *
 * Revision 1.1  2010/07/22 17:16:38  valuev
 * Separated workflow interface, added skeletons
 *
 *
*******************************************************************************/


# ifndef GRIDMD_H
# define GRIDMD_H

/** @file gridmd.h
    @brief Classes for GridMD workflow interface

**/




# include "gmdinc.h"
//# include <gmd/cmdline.h>

# include "refobj.h"
# include "logexc.h"
# include "seqpack.h"

# ifndef WRITE_DEBUG_GRAPH
# define WRITE_DEBUG_GRAPH 0
# endif


#ifdef _WIN32
//# include <stdlib.h>
typedef __int32 gm_int32;
#else
#include <stdint.h>
typedef int32_t gm_int32;
#endif

#include "jobmngr/jobmngr.h"
#include "jobmngr/gmshell.h"

#include "gmnode.h"  

#include "gmdefs.h"
#include <gmd/redirector.h>  // for redirecting gmExperiment to multiple threads

///\en Category structure for indicating the hard link to \ref gmManager::mark_node() function
struct gmHardLink{
  typedef void value_type;
  static const int type=gmLINK_HARD;
};

///\en Returns string representation of node state
std::string gmGetStateName(gmNODE_STATES state);


///\en Category structure for indicating the data link to \ref gmManager::mark_node() function.
///    Constructed with a given data type and optional port numbers. 
template<class data_t>
struct gmDataLink: public std::pair<int,int>{
  typedef data_t value_type;
  gmDataLink(int srcport=-1, int destport=-1):std::pair<int,int>(srcport,destport){}
  static const int type=gmLINK_DATA;
};


///\en Category structure for indicating the file link to \ref gmManager::mark_node() function.
///    Constructed with a given file name. 
struct gmFileLink{
  string filename;
  string filename_dest;
  int type;
  gmFileLink(const string &filename_, const string &filename_dest_="", bool optional = false):
             filename(filename_){
    type=gmLINK_DATA|gmLINK_FILE;
    if(optional)
      type |= gmLINK_OPTDATA;
    if(filename_dest_=="")
      filename_dest = filename;
    else
      filename_dest = filename_dest_;

  }
  //static const int type=gmLINK_DATA|gmLINK_FILE;
};


///\en Category structure for indicating a link by a set of files referenced by wildcards to \ref gmManager::mark_node() function.
///    Constructed with a given pattern for source that may include wildcards and
///    desination which must be a directory name. 
struct gmWildcardLink: public gmFileLink{
  gmWildcardLink(const string &source, const string &destination="./"):gmFileLink(source,destination){
    gmFileLink::type = gmLINK_WCARD|gmLINK_DATA|gmLINK_FILE|gmLINK_OPTDATA;
  }
  
  //static const int type=gmLINK_WCARD|gmLINK_DATA|gmLINK_FILE;
};

///\en Category structure for indicating the file link to \ref gmManager::mark_node() function.
///    Constructed with a given link name. For this link the existance of files is not
///    checked. This link is used to manage files out of GridMD scope.
struct gmStatusLink: public gmFileLink{
  gmStatusLink(const string &linkname_=""):
             gmFileLink(linkname_){
    gmFileLink::type = gmLINK_STATUS|gmLINK_DATA|gmLINK_FILE;           
  }
  //static const int type=gmLINK_STATUS|gmLINK_DATA|gmLINK_FILE;
};

template<class data_t>
struct gmlink_traits{
  typedef data_t value_type;
  static const bool has_value=true;
};

template<>
struct gmlink_traits<void>{
  typedef int value_type;
  static const bool has_value=false;
};


///\en Resource broker types
enum gmRESOURCE_TYPES{
  gmRES_PBS,///<\en Cluster with installed PBS-compatible queueing system
  gmRES_SLURM,///<\en Cluster with installed SLURM-compatible queueing system
  gmRES_BSH, ///< create as 'nohup' under bash envirnment, on machine specified by shell.
#ifdef _WIN32
  gmRES_WSH,///< Win32 only. \sa gmWinShManager
  gmRES_WINAPI, ///< Win32 only. \sa gmWinAPIManager
#endif
  gmRES_TYPES_SIZE ///< \en The number of defined resource types
};

#ifdef _WIN32
  const int gmRES_DEFAULT_TYPE = gmRES_WINAPI;
#else
  const int gmRES_DEFAULT_TYPE = gmRES_BSH;
#endif


//# define MAX_RES_TYPES 3

///\en Type names corresponding to resource types \ref gmRESOURCE_TYPES.
extern const char* gmResTypeName[];



///\en Shell types. The shell is used to access the resource broker remotely.
///    Not all combinations of (resource,shell) are valid within \ref gmResourceDescr
///    (see gmJobManager documentation).
///    gmManager::add_resource() returns <0 for invalid combinations. 
enum gmSHELL_TYPES{
  gmSHELL_NONE,
#ifndef _WIN32
  gmSHELL_UNIX, ///<\en Plain unix command line (no ssh) to access the resource at local machine
  gmSSHELL_UNIX, ///<\en Unix ssh to access the remote resource
#else
  gmSHELL_PLINK, ///<\en Windows plink (ssh client), requires plink.exe and pscp.exe available from PuTTy.
  gmSHELL_MINGW, ///< Win32 only. \sa gmShellMinGW
  gmSHELL_WIN,
#endif
#ifdef USING_LIBSSH
  gmSHELL_LIBSSH,
#endif
#ifdef USING_LIBSSH2
  gmSSHELL_LIBSSH2,
#endif
  gmSHELL_TYPES_SIZE ///< \en The number of defined shell types
};

#ifdef _WIN32
  const int gmSHELL_DEFAULT_TYPE = gmSHELL_WIN;
#else
  const int gmSHELL_DEFAULT_TYPE = gmSHELL_UNIX;
#endif


///\en Type names corresponding to shell types \ref gmSHELL_TYPES.
extern const char* gmShellTypeName[];

///\en User function to work with gridmd. This function must be explicitly
///    defined by the user and explicitly called from the user code.
///    The command line prameters \a argc and \a argv are usually redirected from
///    main() and used for calling \ref gmManager::init(). See 
///    description of \ref gmManager::init() for the list of GridMD-specific command line parameters.
extern int gridmd_main(int argc, char **argv);

///\en Maximum allowed number of recursions in calls to gridmd_main
# define MAX_RECURSION 10



struct gmShellDescr{
  int shell_type; // <\en Shell type: one of \ref gmSHELL_TYPES, default is \ref gmSHELL_UNIX
  gmdString login;      ///<\en Login name
  gmdString host;       ///<\en Remote host name or IP address
  //gmdString ssh_path;   ///<\en Path to the SSH client
  //gmdString scp_path;   ///<\en Path to the SCP tool
  //gmdString client_cfg; ///<\en SSH client command line options  
  //gmdString rem_tmp_dir;///<\en directory on remote machine, to be used as temporary directory. empty - mean use default value, depend on resource type.

  gmExtraParHash param;  ///<\en Set of extra parameters

  gmShellDescr(const int shell_type_ = gmSHELL_DEFAULT_TYPE){
    shell_type = shell_type_;
  }

  ///\en Compare two descriptors: if equal, resources are considered belonging to the same host
  bool operator==(const gmShellDescr &other) const ;
  bool operator< (const gmShellDescr &other) const ;
  gmShell *CreateShell() const;
};


///\en Resourse description for gmManager.
/// For remote cross-platform execution the same application must be compiled for each of the
/// available resources; for submission of remote jobs when the managing GridMD
/// application runs on Windows system an SSH-client for Windows (typically Putty)
/// is required to connect to the remote server.
struct gmResourceDescr{

  /*int shell_type; ///<\en Shell type: one of \ref gmSHELL_TYPES, default is \ref gmSHELL_UNIX
  gmdString login;      ///<\en Login name
  gmdString host;       ///<\en Remote host name or IP address
  gmdString ssh_path;   ///<\en Path to the SSH client
  gmdString scp_path;   ///<\en Path to the SCP tool
  gmdString client_cfg; ///<\en SSH client command line options*/

  gmShellDescr session;
  //gmdString use_session;

  int res_type; ///<\en Resource type: one of \ref gmRESOURCE_TYPES, default is \ref gmRES_PBS

	bool timing;

  // Parameters from <job_manager> XML section
  //int max_queued; ///<\en Maximal number of simultaneously queued jobs for this resource, default is -1 (no limit)
  //int mpi_support;      ///<\en >0 if the manager accepts MPI jobs
  //gmdString init_job_cmd; ///<\en  Command to be executed before mpiexec command for MPI jobs
  //gmdString end_job_cmd;  ///<\en  Command to be executed after mpiexec command for MPI jobs
  gmExtraParHash param;  ///<\en Set of extra parameters for a particular manager

  // Parameters from <application> XML section
  gmdString prefix;     ///<\en Shell commands that should be executed before starting each job on the remote resource
  gmdString postfix;    ///<\en Shell commands that should be executed after finishing each job on the remote resource
  gmdString progdir;    ///<\en Directory where the GridMD application executable is located
  gmdString progname;   ///<\en Name of GridMD application executable

  // User defined parameters (not saved in XML file)
  //int wait_timeout;      ///<\en  Maximul wait time in Wait functions (in ms)
  //int wait_delay;        ///<\en  Delay between status quires in Wait functions (in ms)
  //int save_job_info;    ///<\en If >0 the file info is created in the job working directory

  int active;            ///<\en Flag that activates this resource for automatic scheduling (1 by default)

  ///\en Constructor
  gmResourceDescr(const int res_type_ = gmRES_DEFAULT_TYPE,
    const int shell_type_ = gmSHELL_DEFAULT_TYPE, int active_ = 1) {
     init(res_type_, shell_type_, active_);
  }

  ///\en Initialization
  void init(const int res_type_ = gmRES_DEFAULT_TYPE,
    const int shell_type_ = gmSHELL_DEFAULT_TYPE, int active_ = 1);

# ifndef NO_XML
  ///\en Saves description as a child of the given XmlNode.
  ///    \param [out] parent pointer to the node for which a child will be created
  ///    \param [out] name is the name of the resource 
  ///                 in the XML document, default is login\@host
  ///    \return >0 on success, <0 otherwise
  int Save(XMLFile& xmldoc, xmlNodePtr parent, const gmdString &name="");

  ///\en Loads resource description from 
  ///     the given XmlNode.
  ///    \param [in] parent pointer to the parent node
  ///    \param [in] name is the name of the resource 
  ///    \return >0 on success, <0 otherwise
  int Load(XMLFile& xmldoc, xmlNodePtr parent, gmdString &name);
# endif
  ///\en Creates an instance of job manager corresponding to this descriptor
  ///    \return a pointer to created object (must be deleted with delete) , NULL on error
  gmJobManager *CreateJobManager(gmShell *shell = NULL) const;

  bool operator==(const gmResourceDescr &other) const ;
  bool operator< (const gmResourceDescr &other) const ;

};


struct task_finished{};

///\en Template function that performs datatype packing into a file. 
///    The function is used when transferring link data from/to a node. The
///    existing implementation (memory dump) is valid for all simple plain data types.
///    Has to be specialized for user types requiring any special procedures.
template <class T> 
bool dump_to_file(FILE *f, const T *ptr){
  return fwrite(ptr,sizeof(T),1,f)>0 ? true : false; 
}

///\en Template function that performs datatype reading from a file. 
///    The function is used when transferring link data from/to a node. The
///    existing implementation (copy form file to memory) is valid for all simple plain data types.
///    Has to be specialized for user types requiring any special procedures.
template <class T> 
bool restore_from_file(FILE *f, T *ptr){
  return fread(ptr,sizeof(T),1,f)>0 ? true : false; 
}


class gmLinkData{
public:
  bool readonly;
  virtual bool Dump(FILE *f)    { (void)f; return false; }
  virtual bool Restore(FILE *f) { (void)f; return false; }
  gmLinkData(bool r_only=false):readonly(r_only){}
};

template <class T>
class gmData: public gmLinkData{
public:
  typedef T value_type;
  T v;
  virtual bool Dump(FILE *f){
    gm_int32 size=(gm_int32)sizeof(T);
    fwrite(&size,sizeof(size),1,f);
    return dump_to_file(f,&v);
  }
  virtual bool Restore(FILE *f){
    gm_int32 size;
    
    if(fread(&size,sizeof(size),1,f)!=1){
      LOGERR(-1,fmt("gmData<%s>.Restore: can't read size info from file",typeid(T).name()),0);
      return false;
    }
    if(size!=(gm_int32)sizeof(T)){
      LOGERR(-1,fmt("gmData<%s>.Restore: invalid data size in file (%d instead of %d)",typeid(T).name(), size, sizeof(T)),0);
      return false;
    }
    return restore_from_file(f,&v);
  }
  gmData(bool r_only=false):gmLinkData(r_only){}
};


///\en Treatement of nodes with duplicated names for \ref gmSelector
enum gmNAME_SORT{
  gmSEL_ALL=-1, ///<\en  Select all nodes including thouse with duplicated names
  gmSEL_NEAREST=-2,///<\en Between the nodes with duplicated names select the nearest in id to the current node
};


///\en  Class representing a collection of nodes/ports selected using differemt categories:\n
///     - indicating integer ID of a node\n
///     - indicating regular expression matching the symbolic name of a node\n
///     - indicating a node category by using node selector flags \ref gmNODES.\n
///     NB: port selection is not implemented yet. The ports 
///     for \ref gmManager::mark_node() function are selected explicitly 
///     by indicating portid (the same for all selectted nodes) or automatically (when portid is -1).
class gmSelector{
  friend class gmManager;
  friend class gmGraph;
  template <class A, class B, class C> friend  class gmFork; // needs str and id, TODO: rewrite with interface
  ///\en String representation of a selector (regular expression to select by name).
  ///    When the string is void, uses integer representation.
  string str;
  ///\en Integer representation
  gmNodeID id;
  ///\en Flag indicating how to handle duplicated node names
  int dup_sel;
  ///\en Vector representation: lists selected nodes
  std::vector<gmNodeID> nodevec;
  ///\en Node state representation: lists matching node states
  std::vector<gmNODE_STATES> nodestates;
public:

  ///\en Construct from a string, \a dup_sel indicates how to treat duplicated names (see \ref gmNAME_SORT).
  ///    The string is a standard regular expression 
  ///    to math the node names. For details on regular expressions see wxWidgets manual.
  ///    For example, for selecting nodes with names starting from 'A' and 'B' 
  ///    the expression "(A.*|B.*)" may be used. If there are nodes with the same names
  ///    and the flag \ref gmSEL_NEAREST is given, then only the node closest to the curren node
  ///    in id will be selected among duplicates. Otherwise all duplicates are listed.
  gmSelector(const string &str_, int dup_sel_=gmSEL_NEAREST){
    init(str_,dup_sel_);
  }
  ///\en Construct from a char *, same as above
  gmSelector(const char *str_,int dup_sel_=gmSEL_NEAREST){
    init(string(str_),dup_sel_);
  }
  ///\en Construct from integer mode id flag, string is empty then
  gmSelector(gmNodeID id_):id(id_),dup_sel(gmSEL_ALL){}

  ///\en Construct from matching node states
  gmSelector(const vector<gmNODE_STATES> &states):id(gmNODE_AUTO),dup_sel(gmSEL_ALL),nodestates(states){}

  ///\en Construct from a single node state
  gmSelector(gmNODE_STATES state):id(gmNODE_AUTO),dup_sel(gmSEL_ALL){
    nodestates.push_back(state);
  }
  
  ///\en Construct from vector of nodes, string is empty then
  gmSelector(const vector<gmNodeID> &nodes):id(gmNODE_AUTO),dup_sel(gmSEL_ALL),nodevec(nodes){}


  ///\en (Re-)initialize from a string
  void init(const string &str_,int dup_sel_=gmSEL_NEAREST){
    id=gmNODE_AUTO;
    str=str_;
    dup_sel=dup_sel_;
    sscanf((const char *)str.c_str(),"%d",&id);
  }

  std::string to_string() const {
    if(id==gmNODE_AUTO){
      if(str!="")
        return str;
      if(nodevec.size()){
        string ret="[";
        size_t i=0;
        for(;i<nodevec.size()-1;i++)
          ret+=fmt_s("%d",nodevec[i]);
        ret+=fmt_s("%d]",nodevec[i]);
        return ret;
      }        
    }
    return fmt_s("%d",id);
  }
};

///\en mutex is used to lock access to file-stored information, like 'restor file' and '*.dot graph'.
///  Mutex should be recursive.
class gmMutex{
public:
    virtual void lock()  {};
    virtual void unlock() {};

    static gmMutex voidMutex;
protected:
    virtual ~gmMutex() {}
};

///\en Call-back events, sent by gmManager. Set of events may be extended.
class gmManagerCallback{
public:
  ///\en Called when the graph state has changed.
  virtual void StatusUpdated() {}

  ///\en Called when node state has changed
  virtual void NodeStateChanged(gmNodeID id, gmNODE_STATES current_state, gmNODE_STATES previous_state) {
    (void) id;
    (void) current_state;
    (void) previous_state;
    //printf("node %d: %s->%s\n",id,gmGetStateName(previous_state).c_str(),gmGetStateName(current_state).c_str());
  }

  static gmManagerCallback voidCallBack;
protected:
  virtual ~gmManagerCallback() {}
};


class gmGraph;
class gmScheduler;




typedef char **charDoublePtr;

///\en  Main interface class to access workflow functions. The default global instance
///     of gmManager is called \ref gmExperiment. Many of the 
///     useful member calls
///     to gmExperiment object may be performed the same way in the global scope
///     (namespace gridmd), for example
///     gmExperiment().mark_node("A") is equivalent to gridmd::mark_node("A") and to gridmd::node("A") 
class gmManager{
  friend class gmScheduler;
	friend class gmNodeAction;
protected:
  ///<\en name of the current experiment
  string name;
  int mode;
  int exetype;
  
  gmNodeID idcount;
  int have_start_node; ///<\en id of the first node in distributed section ('start' or other was added), for compatibility with older versions
  
  int dprev; ///<\en distance to previous node (default 1)
  gmNodeID end_node;
  gmNodeID max_node; ///<\en maximal allowed nodeid (for working mode)
  int _argc;
  char **_argv;
  gmGraph *graph;
  int_pack wnodes;
  int distr_node; ///<\en start of the distributed node generation
  int write_files;
  int del_files;
  int last_loaded_event; ///<\en id of the last event loaded from saved event stack when restarting
  string restart_file;

  typedef refmap<string, gmLinkData> data_map_t;
  data_map_t data_rep;

  ///\en Scheduler for managing resources
  gmScheduler *sched;

  ///\en Mutex to be used to lock access to file-soterd information, like *.xml state of claculation.
  gmMutex *mutex;

  gmManagerCallback* call_back;

  ///\en Creates job \a threads from a graph (makes one graph iteration).
  ///    \a graph_error is filled with nonzero error code in case of graph-related errors.
  ///    \return true if there are more iterations needed to process/check remaining nodes.
  bool create_jobs(int &graph_error);

  ///\en Puts the jobs from \a threads into execution queue.
  ///    \return true on success
  bool queue_jobs();

  ///\en Returns true if job list is empty or if some of the jobs are finished.
  ///    Removes the finished jobs from \a threads and marks finished nodes as processed (PROC).
  bool check_jobs(size_t *nfailed=NULL);


  int link_cleanup();

  ///\en Level of recursion in gridmd_main() calls
  static int rec_level;
  ///\en Indicates that graph error (deadlock or such) happened during the execution
  int graph_error;
  
  ///\en Number of failed threads in the last call to \ref execute_iteration()
  size_t nfailed;
  ///\en Number of finished threads in the last call to \ref execute_iteration()
  size_t nfinished;
  ///\en Number of started threads in the last call to \ref execute_iteration()
  size_t nstarted;

  ///\en Maximum number of restarts after job fails allowed for a node
  int max_node_restarts;
  ///\en Graph write frequency: 0= never, 1=once, 2=after each change
  int graph_update;
  ///\en Flag to indicate that we want only to construct a graph
  int construct_only;
  ///\en Flags to indicate what information is needed in the graph picture
  int wr_flags;
  ///\en Flag to indicate that girdmd_main is a standalone worker
  int start_as_worker;
  ///\en Flag to indicate that we have to repeat all local nodes when restarting (-R command line option).
  int full_restart;
  int start_distr;

  int tmp_int;
  map<string, int> blocks;

  ///\en If true, always try to fetch output files connected to jobs, even on job failure
  bool fetch_on_fail;

  ///\en Indicates that we are executing in local mode inside execute(). Used to check when temporarily switching to construction mode.
  bool entered_exec;

	int unset_mode(int mode_bits){
    int ret = mode;
		mode&=~(mode_bits);
		return ret;
	}
	int set_mode(int mode_bits){
		int ret = mode;
		mode|=(mode_bits);
		return ret;
	}
	int copy_mode(int mode_bits){
		int ret = mode;
		mode=mode_bits;
		return ret;
	}


	bool check_restart_file(size_t *nerrors = NULL){
		if(nerrors)
		  *nerrors = 0;
		bool continue_exec=true;
		if(restart_file!=""){ // loading restart information
			int d_node;
			int res=load_state((const char *)restart_file.c_str(),d_node);
			if(res==-100 || res==0) // file does not exist (all unfinished) or no pending threads found
				restart_file="";
			else if(res<0){
				LOGERR(0,fmt("gmManager.check_restart_file: failed to restart from file '%s'",(const char *)restart_file.c_str()),0);  
				graph_error = 1;
				continue_exec = false ;
				if(nerrors)
				  *nerrors = 1; 
			}
			else if(d_node>=idcount) // skip this subgraph
				continue_exec = false; 	         
		}
		return continue_exec;
	}

  ///\en Redirection function. Should return a pointer to unique independent copy of the object for each thread.
  gmManager *thread_redirect(const gmManager *from_where){
    return NULL;
  }

public:

  ///\en Constructor. The optional \a mode parameter specifies the mode in
  ///    which the gmManager is created (one of \ref gmMODES). Normally the default (gmMODE_LOCAL)
  ///    should be used. The operation modes are then selectd automatically. Most of gmManager
  ///    functions work dependent on the execution mode. They also should be called from inside 
  ///    of \ref gridmd_main(...) function. 
  gmManager(int mode=gmMODE_LOCAL,
            gmMutex *mutex=&gmMutex::voidMutex,
            gmManagerCallback *call_back =&gmManagerCallback::voidCallBack );


  ///\en Copy constructor: intended for working mode multi-thread, creates clean instance
  gmManager(const gmManager &other):mutex(&gmMutex::voidMutex), call_back(&gmManagerCallback::voidCallBack), mode(gmMODE_LOCAL){
    _init_params();
  }

  ///\en Destructor.
  ~gmManager();

  ///\en Numerical experiment initialization.
  ///    Local mode: initializes the experiment passing the command line parameters.
  ///    For proper worker mode operation parameters \a argc and \a argv 
  ///    must be redirected from main() command line arguments. 
  ///    GridMD will interpret the following comand line parameters:\n
  ///    \a -w\<node_list\>, where \a \<node_list\> is a sequence of numbers delimited by ':' --
  ///    a switch indicating worker mode with corresponding nodes execution request;\n
  ///    \a -r [xmlfile] -- restart flag (manager mode only), xmlfile will be read as saved experiment configuration, 'expname.xml' 
  ///    is assumed when the file is not specified, where expname is the experiment name supplied to \ref gmManager::init().
  ///    \param name_ name for the current experiment which will be used to construct
  ///                 job names, and file names for saving configurations/checkpoints. Default is 'gmexp'.   
  ///    Other modes: does nothing
  ///    \return Local mode: 1 on success, <0 otherwise
  ///    \return Other modes: 0
  int init(int &argc, charDoublePtr &argv, const string &name_="gmexp");

  ///\en Numerical experiment initialization (local manager mode only).
  ///    Local mode: initializes the experiment passing the parameters. 
  ///    \param name_ name for the current experiment which will be used to construct
  ///                 job names, and file names for saving configurations/checkpoints.
  ///    \param restart -- restart flag, xmlfile will be read as saved experiment configuration
  ///    \param recalc_locals -- if restart flag is true, forces recalculation of all local nodes for saved configuration, otherwise no effect
  ///    \param restart_file -- if restart flag is true, name of xml file containig saved configuration, name.xml by default
  ///    Other modes: does nothing
  ///    \return Local mode: 1 on success, <0 otherwise
  ///    \return Other modes: 0
  int init(const string &name_, bool restart=false, bool recalc_locals=false, const string &restart_file_="");
  

  ///\en Constructor immediately initializing with \ref init()
  gmManager(const string &name_, bool restart=false, bool recalc_locals=false, const string &restart_file_=""):mutex(&gmMutex::voidMutex), call_back(&gmManagerCallback::voidCallBack), mode(gmMODE_LOCAL){
    _init_params();
    init(name_,restart,recalc_locals,restart_file);
  }
  
  ///\en Adds the compute resource into the remote resource description list
  ///    Default resource \a name is the same as hostname in the description.
  ///    Operation modes: local, construction (for explicit execution)\n
  ///    \return the number of currently existing resources (resource_id-1) on success, 0 if the mode is invalid, <0 otherwise
  int add_resource(const gmResourceDescr &rdescr, const string &name="");

  ///\en Adds compute resources found in the given XML file to the list of resources.
  ///    Searches for \<scheduler\> entries at top level of the file and loads
  ///    resource descriptions from each of the \<resource\> child found.
  ///    Operation modes: local\n
  ///    \return the number of successfully added resources.
  int load_resources(const char *filename);
  
  ///\en Saves current list of compute resources into XML file.
  ///    Operation modes: local\n
  ///    \return >0 for success, <0 otherwise, 0 for wrong mode.
  int save_resources(const char *filename);

  ///\en Writes the graph to corresponding file(s) (graphwiz, xml, etc).
  ///    Operation modes: local, construction
  ///    \return >0 for success, <0 otherwise
  int write_graph();

  ///\en Bit flags for temporary files handling modes (see \ref gmTMP_FILES).
  ///    gmFILES_LOCAL: \n 
  ///    Tells to create all link files even in the local mode.
  ///    Note that restart (-R and -r command line options) in local mode 
  ///    will not produce correct results if this flag is not set.\n
  ///    gmFILES_CLEANUP: \n 
  ///    Tells to cleanup all link files that are not in use (after connected nodes are processed).
  ///    Note that full restart (-R command line option) may not be possible if this flag is on.\n
  ///    Operation modes: local\n
  ///    \return the previous setting
  int set_link_files(int file_flags=0){
    swap(file_flags,write_files);
    return file_flags;
  }


  

  ///\en  Sets graphviz output flags (combination of  \ref gmGV_FIELDS).\n
  ///    Operation modes: local, construct\n
  ///    \return the previous setting
  int set_graphviz_flags(int flags=gmGV_ALL){
    swap(flags,wr_flags);
    return flags;
  }
  ///\en Switches automatic graphviz (dot) output modes:
  ///    0 = off, 1 = auto update in execute, 2 = auto update by each graph change  \n
  ///    Operation modes: local, construct\n
  ///    \return the previous setting
  int set_graphviz_output(int mode = 2){
    swap(graph_update,mode);
    return mode;
  }


  ///\en Returns the graph object (for use in skeleton extensions only).
  gmGraph *get_graph(){
    return graph;
  }

  ///\en Set execution mode (one of \ref gmEXE_MODES).
  ///    Operation modes: local\n
  ///    Do not mix with operation mode! This parameters determines
  ///    the way the experiment will be executed:\n
  ///    gmEXE_LOCAL: locally, recursively  using the same application\n
  ///    gmEXE_REMOTE: on remote resource(s) which have to be configured by \ref add_resource()\n
  ///    gmEXE_MIXED: mixing remote and local execution based on performance measurements (not implemented yet)\n
  ///    gmEXE_CONSTRUCT_ONLY: locally, skipping the node execution (for testing the graph integrity)\n
  ///    gmEXE_SERIAL: executes nodes locally just as they appear in the graph (in construction time), no recursive calls performed. This
  ///    mode is not guaranteed to work properly because some links may be
  ///    created after the node is added to the graph. In this case broken link warning is given at node execution.
  ///    \return the previous setting of 0 for wrong mode
  int set_execution(int flags=gmEXE_LOCAL){
    if(mode==gmMODE_LOCAL){
      if(flags&gmEXE_CONSTRUCT_ONLY){ // checking the CONSTRUCT_ONLY setting
        flags=gmEXE_SERIAL; // on, switching to serial mode
        construct_only=1;
      }
      else
        construct_only=0;
      swap(exetype,flags);
      //if(exetype==gmEXE_SERIAL){ // fully serial mode
      //  mode=gmMODE_CONSTRUCT|gmMODE_WORKER;
      //}
      //else
        //mode=gmMODE_CONSTRUCT;
      return flags;
    }
    else 
      return 0;
  }

  
  ///\en Returns current operation mode (see \ref gmMODES).
  int get_mode() const {
    return mode;
  }

  ///\en Returns current gridmd_main recurison level between 0 and \ref MAX_RECURSION.
  ///    This is to take into account when handling  global variables. 
  int get_recursion() const {
    return rec_level;
  }

  
  
  ///\en Here begins the distributed portion of \ref gridmd_main(...) that ends with \ref end_distributed().
  ///    There can be several distributed portions, but nesting of them is not allowed.
  ///    This function adds the 'start' node to the graph, so processing of the
  ///    start node (nodeid=0) should be performed if this function returns 1.\n
  ///    Operation modes: local, construction, worker\n
  ///    The course of execution after calling begin_distributed() depends on the 
  ///    execution mode (see \ref set_execution()). In the serial and construction_only 
  ///    execution mode the application proceeds normally. In the remote and local execution modes
  ///    the main function \ref gridmd_main() is called (recursed) from this function and proceeds in
  ///    construction or local mode.
  ///    \return 1  in worker mode if 'start' node is passed
  ///    \return 0  in all other cases 
  int begin_distributed(){
    return mark_distributed();
  }

  ///\en Here ends the distributed part of the application that begins with \ref begin_distributed().
  ///    This function adds the 'finish' node to the graph and connects all nodes without outputs
  ///    constructed so far to the 'finish' node with hard links. The processing of the 'finish'
  ///    node should be performed if this function returns 1. Note that this node is always local, so it 
  ///    (and all nodes connected with it by incoming hard links) is never transferred to a remote host for execution.
  ///    Moreover, it is guranteed that the 'finish' node is performed by the same \ref gridmd_main() instance
  ///    that called begin_distributed() for the first time (the first \ref gridmd_main() instance in fact). So local 
  ///    variables that existed before \ref begin_distributed() call have valid values and can be used when processing the 'finish' node
  ///    and its hard-linked parents. Note that this is not true for other nodes including the 'start' node.\n
  ///    Operation modes: local, construction, worker\n
  ///    \return 1  in worker mode if 'finish' node is passed
  ///    \return 0  in all other cases 
  template<class link_type>
  int end_distributed(const string &name="finish", gmSelector input=gmNODE_NOOUTPUTS, const link_type &t=gmHardLink(), int srcport=-1){
    if(in_construction()){
      if(start_distr!=1)
        begin_distributed();
      start_distr=0;
    }
    //node_property(gmNODE_AUTO)->SetLocal();
    int res = mark_node(name,input,t,srcport);
    //have_start_node = -1 - idcount; //  prerare for next distributed section
    if(mode&gmMODE_WORKER){
      have_start_node = -1 - idcount; //  prerare for next distributed section
      if(distr_node<=idcount){
        if(mode&gmMODE_FINAL){
          mode = gmMODE_LOCAL; // returning to local mode
          have_start_node = -1 - idcount; //  prerare for next distributed section
        }
        return res; // return the result of mark_node
      }
      else return 0; // skip previous section
    }
    if(in_construction()){
      //have_start_node = -1 - idcount; //  prerare for next distributed section
      gmNodeProp *prop=&(_node_ptr(get_curnode())->userprop); // node should always be OK
      prop->local=true; // end node must be local
      prop->final=true; 
      return execute()>=0 ? 1 : 0;
    }
    else{
      have_start_node = -1 - idcount; //  prerare for next distributed section
      return 0;
    }
  }

  ///\en Same as above for hard links.
  int end_distributed(const string &name="finish",gmSelector input=gmNODE_PREV){
    return end_distributed(name,input,gmHardLink());
  }

  ///\en Same as \ref begin_distributed()
  int mark_distributed();

  ///\en Executes workflow graph from its current state. 
  ///    \return 0         on success if further execution is not needed (all done), 
  ///            1         on success if there are unprocessed nodes,
  ///            2         on success if there are executing nodes 
  ///            -nfail    on failure.
  ///    In the last case nfail is the number of failed graph threads.
  ///    \param wait if true, waits till all nodes are processed or failed, 
  ///    otherwise returns after scheduling jobs (performs one execute iteration only)
  ///    \param cleanup: 0 means no cleanup of jobs, -1= automatic, 1= yes
  int execute(bool wait=true, int cleanup=-1, int max_schedules = -1);

  

  ///\en Performs one sheduling iteration of workflow graph from its current state. 
  ///    \return 0         on success if further execution is not needed (all done), 
  ///            1         on success if there are unprocessed nodes,
  ///            2         on success if there are executing nodes 
  ///            -nfail    on failure.
  ///    In the last case nfail is the number of failed graph threads.
  int execute_iteration(){
    return execute(false,0,1);
  }

	int execute_iterations(int num){
    return execute(false,0,num);
  }

  ///\en Stops jobs for all executing nodes. The stopped incomplete nodes are assigned \ref gmNS_FAIL state.
  ///    \return number of stopped jobs.
  ///    \param fetch_results if true, try to fetch the results for nodes being stopped.
  ///    \param cleanup for stopped jobs: 0 means no cleanup of jobs, -1= automatic, 1= yes
  ///    Operation modes: construction, worker\n
  int stop_all(bool fetch_results=false, int cleanup=-1){
    return stop_node(gmSelector(gmNODE_ALL/*gmNS_EXE*/),fetch_results, cleanup);
  }

  ///\en Stops jobs for nodes matching a given selector in case they are executing. The stopped incomplete nodes are assigned \ref gmNS_FAIL state.
  ///    \return number of stopped jobs.
  ///    \param fetch_results if true, try to fetch the results for nodes being stopped.
  ///    \param cleanup for stopped jobs: 0 means no cleanup of jobs, -1= automatic, 1= yes
  ///    Operation modes: construction, worker\n
  int stop_node(gmSelector nodes, bool fetch_results=false, int cleanup=-1);

  ///\en Adds a node to the graph (construction mode), triggers processing of this node (worker mode).
  ///    Should be called inside distributed \ref gridmd_main() section (between \ref begin_distributed() and \ref end_distributed()).
  ///    The parameters are arbitrary node \a name and the selector (\ref gmNODES) telling how to conect this 
  ///    node with the previously constructed ones. The connection type is specified by a template parameter \a link_type,
  ///    which can be either\n
  ///    \ref gmHardLink to indicate a hard link,  or\n
  ///    \ref gmDataLink\<data_t\> to indicate a data link envolving data of type data_t.\n
  ///    For the data link a source port of a link may be indicated by a parameter \a srcport. The corresponding destination port
  ///    (input port) is added automatically on the node being constructed.
  ///    Selector \a input may have the following values:\n
  ///    >=0 connect with the node having nodeID \a input,\n
  ///    \ref gmNODE_NONE no links needed \n
  ///    \ref gmNODE_ALL connect with all availbale nodes \n
  ///    \ref gmNODE_PREV connect with the previously constructed node \n
  ///    \ref gmNODE_NOINPUTS connect with nodes having no inputs so far \n
  ///    \ref gmNODE_NOOUTPUTS connect with nodes having no outputs so far \n
  ///    \ref gmNODE_AUTO same as gmNODE_PREV
  ///    Operation modes: construction, worker
  ///    \return 1 if this node has to be processed (worker mode only), 0 otherwise
  template<class link_type>
  int mark_node(const string &name, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
    int res=_mark_node(name);
		if(input.id==gmNODE_NONE ||
      (idcount<=0 && (input.id==gmNODE_ALL || input.id==gmNODE_PREV || input.id==gmNODE_AUTO)))
			return res;
    //if(input.id!=gmNODE_NONE)
    link(t,input,idcount,srcport);
    dprev=1; // reverting to original dprev
    return res;
  }

  ///\en Same as above for hard links.
  int mark_node(const string &name, gmSelector input=gmNODE_PREV){
    return mark_node(name,input,gmHardLink());
  }


   ///\en Adds a local node to the graph (construction mode), triggers processing of this node (worker mode).
  ///    Should be called inside distributed \ref gridmd_main() section (between \ref begin_distributed() and \ref end_distributed()).
  ///    The parameters are arbitrary node \a name and the selector (\ref gmNODES) telling how to conect this 
  ///    node with the previously constructed ones. The connection type is specified by a template parameter \a link_type,
  ///    which can be either\n
  ///    \ref gmHardLink to indicate a hard link,  or\n
  ///    \ref gmDataLink\<data_t\> to indicate a data link envolving data of type data_t.\n
  ///    For the data link a source port of a link may be indicated by a parameter \a srcport. The corresponding destination port
  ///    (input port) is added automatically on the node being constructed.
  ///    Selector \a input may have the following values:\n
  ///    >=0 connect with the node having nodeID \a input,\n
  ///    \ref gmNODE_NONE no links needed \n
  ///    \ref gmNODE_ALL connect with all availbale nodes \n
  ///    \ref gmNODE_PREV connect with the previously constructed node \n
  ///    \ref gmNODE_NOINPUTS connect with nodes having no inputs so far \n
  ///    \ref gmNODE_NOOUTPUTS connect with nodes having no outputs so far \n
  ///    \ref gmNODE_AUTO same as gmNODE_PREV
  ///    Operation modes: construction, worker
  ///    \return 1 if this node has to be processed (worker mode only), 0 otherwise
  template<class link_type>
  int local_node(const string &name, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
    int res = mark_node(name,input,t,srcport);
    _node_ptr(get_curnode())->userprop.local = true;
    return res;
  }

  ///\en Same as above for hard links.
  int local_node(const string &name, gmSelector input=gmNODE_PREV){
    return local_node(name,input,gmHardLink());
  }

  ///\en Synonym of \ref mark_node.
  template<class link_type>
  int node(const string &name, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
    return mark_node(name,input,t,srcport);
  }

  ///\en Synonym of \ref mark_node.
  int node(const string &name, gmSelector input=gmNODE_PREV){
    return mark_node(name,input,gmHardLink());
  }

  ///\en Selects nodes/ports matching given selector into a std::pair vector {nodeid,portid}. Parameter \a input is not used currently.
  int select_nodes(std::vector< std::pair<int,int> > &nodes, gmSelector selector, bool input=true, int def_port=-1, int closest_to=-1) const;

	///\en Finds one node mathching the selector
	///    \return ID of a node or \ref gmNODE_NONE if not found
	gmNodeID find_node(const gmSelector &selector){
    std::vector< std::pair<int,int> > nodes;
		if(select_nodes(nodes,selector)>0)
			return nodes[0].first;
		else
			return gmNODE_NONE;
	}

	gmNode *get_node_ptr(gmNodeID nodeid) const {
	  return _node_ptr(nodeid);	
	}

  ///\en Selects edges matching all pairs of source and destination node selectors.
	///    \return the number of found edges.
  int select_edges(std::vector<gmEdgeID> &edges, gmSelector source, gmSelector dest, int srcport = -1, int destport =-1) const ;

	gmNodeID get_source(gmEdgeID edgeid) const;


	///\en Finds one edge by source and destination node ids.
	///    \return ID of an edge or \ref gmEDGE_NONE if not found
	gmEdgeID find_edge(gmNodeID source, gmNodeID dest) const;

  gmEdge *get_edge_ptr(gmEdgeID edgeid) const ;


  ///\en Links the nodes indicated by selectors \a from and \a to with data or hard link. 
  ///    Each of the selectors may have the following values (\ref gmNODES):\n
  ///    >=0 connect with the node having nodeID \a input,\n
  ///    \ref gmNODE_NONE no links needed \n
  ///    \ref gmNODE_ALL connect with all availbale nodes \n
  ///    \ref gmNODE_PREV connect with the previously constructed node \n
  ///    \ref gmNODE_NOOUTPUTS connect with nodes having no outputs so far \n
  ///    The link type is deduced from the template argument type, use link<gmHardLink> or link<void>  for hard links,
  ///    and link<data_t> for data links envolving transfer of data_t.
  ///    For the data link a source port of a link may be indicated by a parameter \a srcport. The corresponding destination port
  ///    (input port) is added automatically on the destination nodes.
  ///    Operation modes: construction\n
  ///    \return 1 on success, <0 (and logs error) otherwise
  template <class data_t>
  int link(gmSelector from, gmSelector to, int srcport=-1){
    return link(gmDataLink<data_t>(), from , to ,srcport);
  }

  template <class link_t>
  int link(const link_t & lnk, gmSelector from, gmSelector to, int srcport=-1){
    if(mode&gmMODE_WORKER) // no links in worker mode
      return 0;
    if(mode&gmMODE_CONSTRUCT){  // construction mode, begining the construction
      if(!check_distrnode()) // checking that we are above distrnode
        return 0; // we are on inactive branch of the graph
    }
    //if(!(( mode==gmMODE_CONSTRUCT || mode==gmMODE_LOCAL ) && // construction mode, begining the construction
    //  check_distrnode())){ // checking that we are above distrnode
    //  return 0;
    //}
    // add type to dictionary here
    // ...
    std::vector< std::pair<int,int> > inputs, outputs;
    if(select_nodes(inputs,from, true, srcport, idcount)<=0)
      return LOGERR(-1,fmt("gmManager:link no nodes found that match input_node specification '%s', link skipped",(const char *)from.to_string().c_str()),0);
    if(select_nodes(outputs,to, false, -1, idcount)<=0)
      return LOGERR(-1,fmt("gmManager:link no nodes found that match output_node specification '%s', link skipped",(const char *)to.to_string().c_str()),0);
    int hits = 0;
    for(size_t i=0;i<inputs.size();i++)
      for(size_t j=0;j<outputs.size();j++)
        if(link_single(lnk, inputs[i].first,outputs[j].first,inputs[i].second)>=0)
          hits++;
    if(hits && graph_update>=2) // writing the graphwiz file
      write_graph();
    return 1;
  }

  
  ///\en Returns 1 only in worker mode and only when current node needs to be processed.
  ///    Otherwise returns 0. This function may be used alternatively to checking the return code
  ///    of mark_node function when there are additional links to be added
  ///    to the node before processing.
  int process_cur_node();
 

  ///\en Get output associated with given data port (default is 0) for the current node.
  ///    The data type must mutch the one indicated by link creation, 
  ///    otherwise the warning is given (at worker mode runtime) and temporary reference 
  ///    (not transferred anywhere by GridMD) is returned.
  ///    Operation modes: worker\n
  ///    The transfer of data corresponding to the output ports is performed by GridMD. 
  ///    This function returns a reference to the variable which can be used as output in course of current node processing.
  ///    When node processing is finished, the corresponding data file is written and transfered to the next destination node.
  ///    \return a reference to data_t for valid data types (excluding void).
  template<class data_t>
  typename gmlink_traits<data_t>::value_type &node_output(int portid=0){
    string port_name=fmt("%d_%d.out",idcount,portid);
    data_t *d=get_data<data_t>(port_name,0);
    if(!d){ // wrong type, creating temporary
      LOGMSG(vblWARN,fmt("gmManager.node_output: output with different type exists for '%s', temporary created (no data transfer possible)",(const char *)port_name.c_str()),0);
      port_name+=".";
      port_name+=typeid(data_t).name();
      d=get_data<data_t>(port_name,0);
    }
    return *d;
  }

  ///\en Get input associated with given data port (default is 0) for the current node.
  ///    The data type must mutch the one indicated by link creation, 
  ///    otherwise a warning is given (at worker mode runtime) and temporary reference 
  ///    (whith garbage data) is returned.
  ///    Operation modes: worker\n
  ///    Transfer of data corresponding to the input ports is performed by GridMD. This function performs data reading from
  ///    a file transferred to a destination node and returns the reference to a variable which can be used 
  ///    as input in course of current node processing.
  ///    \return a const reference to data_t for valid data types (excluding void).
  template<class data_t>
  const typename gmlink_traits<data_t>::value_type &node_input(int portid=0){
    string port_name=fmt("%d_%d.in",idcount,portid);
    data_t *d;
    int res= -1;
    if(exetype!=gmEXE_SERIAL && ((write_files&gmFILES_LOCAL) || start_as_worker)) // working with files
      res= _get_data<data_t>(&d,port_name,1);
    else{ // working with memory data
      int source, iport;
      std::pair<int,int> p=get_inlink(idcount,portid);
      source=p.first, iport=p.second;
      if(source==-1)
        LOGMSG(vblWARN,fmt("gmManager.node_input: undefined input link for node %d, port %d, using uninitialized data!",idcount,portid),0);
      else 
        port_name=fmt("%d_%d.out",source,iport);
      int res= _get_data<data_t>(&d,port_name,0);
    }
    if(res==-2){ // wrong type, creating temporary
      LOGMSG(vblWARN,fmt("gmManager.node_input: input with different type exists for '%s', temporary created (using uninitialized data)",(const char *)port_name.c_str()),0);
      port_name+=".";
      port_name+=typeid(data_t).name();
      res=_get_data<data_t>(&d,port_name,0);
    }
    return *d;
  }

  template<class data_t>
  data_t *get_data(const string &data_name, int update=1){
    data_t *v;
    _get_data(&v,data_name,update);
    return v;
  }
   
  gmNodeID advance_nodeid();

  

  ///\en Get current node ID.
  gmNodeID get_curnode() const {
    return idcount;
  }
  
  /// check the distr_node before advancing nodeid
  /// also checks the error status of the last graph, otherwise returns 0
  int check_distrnode() const {
    if(distr_node<=idcount && !graph_error)
      return 1; // OK
    else
      return 0;
  }

  /// checks that the node with given ID belongs to the subgraph being executed
  /// throws exception when the subgraph is finished
  int check_subgraph(gmNodeID node){
    //if(mode==gmMODE_LOCAL)
      //return 0; // change to finding final nodes
    if(node==-1 || (max_node>=0 && node>max_node)){ // max_node=-1 is reserved for serial execution
      if(rec_level==0 || (write_files&gmFILES_LOCAL)){
        dump_data(); // dumping all repository variables into files
        data_rep.clear(); // clearing to avoid double counting in the local mode
      }
      if(exetype&gmEXE_LOCAL){
        if(rec_level>0)
          throw task_finished();  // termination of the worker started recursively
        else
          exit(0); // termination of  the standalone worker 
      }
      else
        return 0; // must not arrive here  
    }
    if(exetype!=gmEXE_SERIAL){ // if nodelist is actual
      // checking nodelist
      bool found =false;
      for(int_pack::iterator it=wnodes.begin();it!=wnodes.end();++it){
        if(*it==(int)node){
          found=true;
          break;
        }
      }
      if(!found)
        return 0;
    }
    return 1;
  }
  
  ///\en Returns TRUE if the manager is in construction mode and 
  ///    in the current distributed section of the code, FALSE otherwise. 
  ///    Used to program workflow skeletons
  ///    for detecting the construction mode.
  bool in_construction() const{
    return (get_mode()&gmMODE_CONSTRUCT) && // construction mode
      check_distrnode(); // checking that we are above distrnode (the end of the finished distributed section)
  }


  ///\en Sets a property associated with the node(s). 
  ///    \return Number of affected nodes.
  ///    Operation modes: construction\n
  template<class value_t>
  int set_node_property(value_t (gmNodeProp::*m), const value_t &value, gmSelector nodes=gmNODE_AUTO){
    if(!in_construction())
      return 0;
    if(nodes.id==gmNODE_NEXT){ // reserving for next nodes
      tmpprop.*m=value;
      return 1;
    }
    if(nodes.id==gmNODE_AUTO) // auto means current
      nodes.id = gmNODE_CUR;
    std::vector< std::pair<int,int> > inputs;
    if(select_nodes(inputs,nodes,false, -1, idcount)<0)
      return LOGERR(-1,fmt("gmManager.set_node_property: no nodes found that match input_node specifier '%s', function skipped",(const char *)nodes.str.c_str()),0);
    for(size_t i=0;i<inputs.size();i++){
      // TODO: here decide whether this property can be changed or not
      gmNode *nodep=_node_ptr(inputs[i].first);
      if(nodep)
        nodep->userprop.*m=value;
    }
    return (int)inputs.size();
  }

  ///\en Returns a pointer to string data property of the node, or NULL if \a id is invalid
  vector<std::string> *node_data_by_id(gmNodeID id=gmNODE_AUTO){ 
    if(id == gmNODE_NEXT)
      return &tmpprop.string_data;
    if(id==gmNODE_AUTO) // auto means current
      id = gmNODE_CUR;
    gmNode *ptr = _node_ptr(id);
    if(!ptr)
      return NULL;
    return &ptr->GetStringData();
  }

  ///    Operation modes: construction\n
  template<class value_t>
  value_t get_node_property_by_id(value_t (gmNodeProp::*m), gmNodeID nid) const {
      if(nid == gmNODE_NEXT)
        return tmpprop.*m;
      if(nid==gmNODE_AUTO) // auto means current
        nid = gmNODE_CUR;
      gmNode *nodep=_node_ptr(nid);
      if(!nodep)
      {
          LOGERR(-1,fmt("gmManager.get_node_property_by_id: no nodes found that match '%i', function skipped", nid), 0);
          return value_t();
      }
      return nodep->userprop.*m;
  }


  ///\en Gets error status (one of \ref gmERROR_CODES) currently associated with the node
  ///    and optionally the error message
  ///    Operation modes: construction\n
  gmERROR_CODES get_node_error_by_id(gmNodeID nid, std::string *message = NULL) const {
    if(nid == gmNODE_NEXT)
      return gmERR_NONE;
    gmNode *nodep=_node_ptr(nid);
    if(!nodep){
      LOGERR(-1,fmt("gmManager.get_node_property_by_id: no nodes found that match '%d', function skipped", nid), 0);
      if(message)
        *message =  "node not found";
      return gmERR_GRAPH;
    }
    return nodep->GetErrorStatus(message);
  }

  ///\en Sets resource associated with the node(s). Resource with this name must have been added to 
  ///    gmManager (see \ref add_resource) or loaded from XML at the time of execution. If assigned, the execution will be
  ///    scheduled exactly to this resource, no automatic resource allocation happens. For hard-linked nodes 
  ///    with different resources a warning is given and the resource set for the first node in the thread is used. 
  ///    \return Number of affected nodes.
  ///    Operation modes: construction\n
	int set_node_resource(const std::string &resource_name, gmSelector nodes=gmNODE_AUTO){
    return set_node_property(&gmNodeProp::resource,resource_name, nodes);
  }

	///\en Sets a new name to selected node(s). 
  ///    \return Number of affected nodes.
  int set_node_name(const std::string &newname, gmSelector nodes=gmNODE_AUTO);

  ///\en Define a logical node block (currently affects graph output only).
  ///    [Operation modes: construction\n]
  int block(const string &blockname, bool cond=true);

  ///\en Group nodes together: the grouped nodes will share working directory under the same user@host. 
  ///    If the nodes belong to other groups, they will be reassigned.
  ///    Resources for the grouped nodes are checked at time of execution 
  ///    to be operated by the same session (shell).
  ///    \return group ID>=0 on success, <0 on failure
  ///    Operation modes: construction\n
  int make_node_group(gmSelector nodes);


  ///\en Assign nodes to the existing group: the grouped nodes will share working directory under the same user@host. 
  ///    If the nodes belong to other groups, they will be reassigned.
  ///    Resources for the grouped nodes are checked at time of execution 
  ///    to be operated by the same session (shell).
  ///    \return group ID>=0 on success, <0 on failure
  ///    Operation modes: construction\n
  int assign_to_group(int groupid, gmSelector nodes);

  ///\en Set an action object associated with the node(s). OnExecute() will be called for that object when node executes.  
  ///    \return Number of affected nodes.
  ///    Operation modes: construction\n
  int set_node_action(const gmNodeAction &action, gmSelector nodes=gmNODE_AUTO);

  ///\en Set an action command associated with the node(s). The command will be executed (locally or remotely) when node executes.
  ///    \return Number of affected nodes.
  ///    Operation modes: construction\n
  int set_node_action(const std::string &command, gmSelector nodes=gmNODE_AUTO);


  ///\en Manually assign a state to the node(s). 
  ///    The following states can be manually assigned only:
  ///      \ref gmNS_NO_NODE  don't preassign any state for the node (the state assigned automatically, default behaviour)
  ///      \ref gmNS_UNPROC   assign this to recalculate the node, \n
  ///                         all dependent nodes are also automatically marked as unprocessed\n
  ///      \ref gmNS_PROC     mark the node as processed, all data associated with outgoing links must exist, 
  ///                         for outgoing file links of remote nodes the file links should have 
  ///                         names \<dest_filename\>.node\<remote_nodeid\>\n
  ///     \ref gmNS_WAIT      mark the node as delayed, no attempt to execute the node will be performed\n
  ///     \ref gmNS_FAIL      mark the node as failed  
  /// \return Number of affected nodes, -1 if the selected state cannot be set manually
  ///    Operation modes: construction\n
  int set_node_state(enum gmNODE_STATES state, gmSelector nodes=gmNODE_AUTO);

  ///\en \return current node state, one of \ref gmNODE_STATES
  enum gmNODE_STATES get_node_state_by_id(gmNodeID nid) const;
  
  ///\en Set a node which will be autmatically linked by creation of a new node
  gmNodeID set_autolink_node(gmNodeID node=-1){
    if(node<0)
      dprev=1;
    else
      dprev=get_curnode()-node+1;
    if(dprev<1)
      dprev=1;
    return get_curnode()-dprev+1;
  }

  ///\en Cleans persistent nodal jobs from all resources
  ///    \return number of successfully cleaned jobs
  ///    Operation modes: construction\n
  size_t clean_jobs();
  
  ///<\en Fetch files from the working directory of the specified workdir group.
  ///     The vector \a files contains filename pairs {source_name, dest_name}, where
  ///     \a source_name is the name on the (remote) resource, 
  ///     \a dest_name is the name on the local machine where the file should be placed.
  ///     If \a dest_name is "" the same name as source in the local working directory is assumed.
  ///     This function executes successfuly if the  working directory is valid, i.e.:\n
  ///     - when at least one node of the group was submitted to execution AND\n
  ///     - before clean_jobs() command was performed.
  ///     \return the number of fetched files on success or, 
  ///             -1 if workdir is not available
  ///             <-1 on other error;
  ///    Operation modes: construction\n
  int fetch_workdir_files(int groupid, const std::vector< std::pair<std::string, std::string> > &files);

  ///\en Same as \ref fetch_workdir_files but for single file.
  ///    Operation modes: construction\n
  int fetch_workdir_file(int groupid, const std::string &source, const std::string &dest=""){
    std::vector< std::pair<std::string, std::string > > files(1, std::make_pair( source, dest ) );
    return fetch_workdir_files(groupid,files);
  }

  ///<\en Fetch files from the nodes specified by a selector.
  ///     The vector \a files contains filename pairs {source_name, dest_name}, where
  ///     \a source_name is the name on the (remote) resource, 
  ///     \a dest_name is the name on the local machine where the file should be placed.
  ///     If \a dest_name is "" the same name as source in the local working directory is assumed.
  ///     This function executes successfuly for a node if the node is a part of a thread (job) with valid job directory, i.e.:\n
  ///     - the node is remote (job-managed) and has been submitted to execution AND\n
  ///     - no clean_jobs() command was performed after finishing this node.
  ///     \return the number of fetched files on success or, 
  ///             -1 if job workdir was not available for (all) listed node(s)
  ///             <-1 on other error;
  ///    Operation modes: construction\n
  int fetch_node_files(gmSelector nodes, const std::vector< std::pair<std::string, std::string> > &files);

   ///\en Same as \ref fetch_node_files but for single file.
  ///    Operation modes: construction\n
  int fetch_node_file(gmSelector nodes, const std::string &source, const std::string &dest=""){
    std::vector< std::pair<std::string, std::string > > files(1, std::make_pair( source, dest ) );
    return fetch_node_files(nodes,files);
  }

  ///\en If set to \a true, an attempt is always made to fetch link files from a remote node
  ///    after node job is run, even if the node job has failed.
  ///    \return previous setting
  bool set_fetch_on_fail(bool flag){
    swap(fetch_on_fail, flag);
    return flag;
  }


  ///\en Set maximum number of restart efforts after node failure.
  ///    \return previous setting
  int set_max_node_restarts(int number){
    std::swap(number,max_node_restarts);
    return number;
  }

  ///\en Get current event stack.
  ///    Node state changes are pusged_back to this vector in order of appearence.
  const vector< std::pair<gmNodeID, gmNODE_STATES> >& get_event_stack() const;
 
  ///\en \return true if there was a job state change (jobs finished, started or failed) 
  ///            during the last execute_iteration(), false otherwise (no jobs or waiting for existing jobs to finish).
  bool job_state_changed() const;

protected:
  gmNodeProp tmpprop;
  mngptr<gmNodeAction> tmpact;
  enum gmNODE_STATES tmpstate;
  ///\en Dumps all repository data
  void dump_data(){
    for(data_map_t::iterator it=data_rep.begin();it!=data_rep.end();++it){
      if(it->second->readonly) // no need to dump this file, readonly
         continue;
      LOGMSG(vblMESS3,fmt("Dumping the link file '%s'",(const char *)it->first.c_str()),0);
      FILE *f=fopen((const char *)it->first.c_str(),"wb");
      if(!f)
        LOGMSG(vblWARN,fmt("gmManager.dump_data: can't open the file '%s' for writing, link data skipped!",(const char *)it->first.c_str()),0);
      else if(!it->second->Dump(f))
        LOGMSG(vblWARN,fmt("gmManager.dump_data: dumping data to the file '%s' failed, link data skipped!",(const char *)it->first.c_str()),0);
      fclose(f);
    }
  }
  int _mark_node(const string &name); 
  

  template <class link_t>
  int link_single(const link_t& lnk, gmNodeID from, gmNodeID to, int srcport=-1){
    (void)lnk;
    string link_name;
    int linkid=graph_link(lnk.type,from,to,srcport,&link_name);
    //if(linkid>=0){
    //  gmData<data_t> *ptr=new gmData<data_t>();
    //  data_rep[link_name]=ptr;
    //}
    return linkid;
  }

  /// 1= OK
  /// -1 = no file
  /// -2 = wrong type
  template<class data_t>
  int _get_data(data_t ** data,const string &data_name, int update=1){
    gmLinkData *d=data_rep[data_name];
    gmData<data_t> *ptr;
    int res=1;
    if(!d){
      ptr = new gmData<data_t>(update? true: false);
      data_rep[data_name]=ptr;
      if(update && ((write_files&gmFILES_LOCAL) || rec_level==0)){
        // checking if there is a file which can be used to restore the value
        LOGMSG(vblMESS3,fmt("Reading the link file '%s'",(const char *)data_name.c_str()),0);
        FILE *f=fopen((const char *)data_name.c_str(),"rb");
        if(f){
          if(!ptr->Restore(f)){
            LOGERR(-2,fmt("Can't load data from file '%s'",(const char *)data_name.c_str()),0);
            res=-2;
          }
          else
            LOGMSG(vblMESS3,fmt("File OK"),0);
          fclose(f);
        }
        else{
          LOGMSG(vblMESS3,fmt("No file"),0);
          res=-1;
        }
      }
    }
    else
      ptr=dynamic_cast<gmData<data_t> *>(d);
    if(!ptr){
      *data=NULL;
      res= -2;
    }
    else 
      *data=&ptr->v;
    return res; 
  }
  
  std::pair<int,int> get_inlink(int nodeid, int portid) const;

  int graph_link(int type,gmNodeID from, gmNodeID to, int srcport=-1, string *name=NULL, string *name_dest=NULL);

  int save_state(const char *xmlfile);
  int load_state(const char *xmlfile, int &d_node);

  gmNode *_node_ptr(int nodeid) const;

  void updated_status();

  void _init_params();

};

template<>
inline int &gmManager::node_output<void>(int portid){
  string port_name=fmt("%d_%d.out",idcount,portid);
  LOGMSG(vblWARN,fmt("gmManager.node_output: output for hard link requested for '%s', temporary created (no data transfer possible)",(const char *)port_name.c_str()),0);
  return tmp_int;
}

template<>
inline const int &gmManager::node_input<void>(int portid){
  string port_name=fmt("%d_%d.out",idcount,portid);
  LOGMSG(vblWARN,fmt("gmManager.node_input: input for hard link requested for '%s', temporary created (no data transfer possible)",(const char *)port_name.c_str()),0);
  return tmp_int;
}

template <>
inline int gmManager::link_single< gmDataLink<void> >(const gmDataLink<void> &lnk, gmNodeID from, gmNodeID to, int srcport){
    (void)srcport; (void)lnk;
  return graph_link(gmLINK_HARD,from,to);
}

template <>
inline int gmManager::link_single<gmFileLink>(const gmFileLink &lnk, gmNodeID from, gmNodeID to, int srcport){
  string link_name=lnk.filename;
  string link_dest=lnk.filename_dest;
  int linkid=graph_link(lnk.type,from,to,srcport,&link_name, &link_dest);
  return linkid;
}


// same as file link
template <>
inline int gmManager::link_single<gmWildcardLink>(const gmWildcardLink &lnk, gmNodeID from, gmNodeID to, int srcport){
  string link_name=lnk.filename;
  string link_dest=lnk.filename_dest;
  int linkid=graph_link(lnk.type,from,to,srcport,&link_name, &link_dest);
  return linkid;
}


template <>
inline int gmManager::link_single<gmStatusLink>(const gmStatusLink &lnk, gmNodeID from, gmNodeID to, int srcport){
  string link_name=lnk.filename;
  int linkid=graph_link(lnk.type,from,to,srcport,&link_name);
  return linkid;
}

template <>
inline int gmManager::link<gmHardLink>(gmSelector from, gmSelector to, int srcport){
  (void) srcport;
  return link<void>(from,to);
}


#if !USING_GMTHREADS
///\en Main experiment object.
extern gmManager gmExperiment;
#else
extern gmManager gmExpObj;
extern gmRedirectorPrototyped<gmManager> gmExpRedirector;

# define gmExperiment (*(gmExpRedirector.GetObject()))

#endif

///\en Namespace for global GridMD functions. These functions duplicate (for convenience) the corresponding
///    \ref gmManager class member functions and are applied to the global \ref gmExperiment object.
namespace gridmd{

///\en \ref gmManager::begin_distributed() called for the global gmExperiment
inline int begin_distributed(){
  return gmExperiment.begin_distributed();
}
///\en \ref gmManager::end_distributed() called for the global gmExperiment
template<class link_type>
int end_distributed(const string &name="finish", gmSelector input=gmNODE_NOOUTPUTS, const link_type &t=gmHardLink(), int srcport=-1){
  return gmExperiment.end_distributed(name,input,t,srcport);
}
///\en Same as above for hard links.
inline int end_distributed(const string &name="finish",gmSelector input=gmNODE_NOOUTPUTS){
  return gmExperiment.end_distributed(name,input,gmHardLink());
}
template<class value_t>
int set_node_property(value_t (gmNodeProp::*m), value_t value, gmSelector nodes=gmNODE_AUTO){
  return gmExperiment.set_node_property(m,value,nodes);
}

inline int set_node_action(const gmNodeAction &action, gmSelector nodes=gmNODE_AUTO){
  return gmExperiment.set_node_action(action,nodes);
}

///\en See \ref gmManager::mark_node()
template<class link_type>
int mark_node(const string &name, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
  return gmExperiment.mark_node(name,input,t,srcport);
}
///\en See \ref gmManager::mark_node()
inline int mark_node(const string &name, gmSelector input=gmNODE_PREV){
  return gmExperiment.mark_node(name,input);
}

#define EXEC 1

///\en See \ref gmManager::mark_node()
template<class link_type>
int node(const string &name, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
  return gmExperiment.mark_node(name,input,t,srcport);
}
///\en See \ref gmManager::mark_node()
inline int node(const string &name, gmSelector input=gmNODE_PREV){
  return gmExperiment.mark_node(name,input);
}



///\en See \ref gmManager::get_curnode()
inline int get_curnode(){
  return gmExperiment.get_curnode();
}

///\en See \ref gmManager::link()
template <class data_t>
inline int link(gmSelector from, gmSelector to, int srcport=-1){
  return gmExperiment.link<data_t>(from,to,srcport);
}

template <>
inline int link<gmHardLink>(gmSelector from, gmSelector to, int srcport){
  (void)srcport;
  return gmExperiment.link<void>(from,to);
}

template <class link_t>
int link(const link_t & lnk, gmSelector from, gmSelector to, int srcport=-1){
  return gmExperiment.link(lnk,from,to,srcport);
}

///\en See \ref gmManager::node_output()
template<class data_t>
data_t &node_output(int portid=0){
  return gmExperiment.node_output<data_t>(portid);
}

///\en See \ref gmManager::node_input()
template<class data_t>
const data_t &node_input(int portid=0){
  return gmExperiment.node_input<data_t>(portid); 
}

///\en See \ref gmManager::process_cur_node()
inline int process_cur_node(){
  return gmExperiment.process_cur_node(); 
}

///\en See \ref gmManager::init()
inline int gridmd_init(int argc,char **argv){
  return gmExperiment.init(argc,argv); 
}

inline int block(const string &blockname, bool cond=true){
  return gmExperiment.block(blockname,cond);
}


inline gmNodeID set_autolink_node(gmNodeID node=-1){
  return gmExperiment.set_autolink_node(node);
}

}

# endif
