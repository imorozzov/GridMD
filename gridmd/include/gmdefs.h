#ifndef GMDEFS_H
#define GMDEFS_H

/**  @file gmdefs.h Forward definitions for type(s) and functions declared at gridmd.h
*/

typedef int gmNodeID;
typedef int gmEdgeID;

///\en Node selector flags for use in linking functions together with node ids (see also \ref gmSelector).
enum gmNODES{
  gmNODE_NONE=-1,///<\en No nodes selected
  gmNODE_ALL=-2,///<\en All available nodes
  gmNODE_PREV=-3, ///<\en Previous node
  gmNODE_NOINPUTS=-4, ///<\en All nodes currently with no input links
  gmNODE_NOOUTPUTS=-5, ///<\en All nodes currently with no output links
  gmNODE_AUTO=-6, ///<\en Automatic selection performed by implementation
  gmNODE_NEXT=-7, ///<\en Next node: works for \ref set_node_property and \ref set_node_action only
  gmNODE_CUR=-8, ///<\en Current node (just created or the one being processed)
  gmNODE_LOCAL=-9, ///<\en For linking with local filesystem or data
};

///\en Special edge ids, to be returned from edge location functions 
enum gmEDGES{
  gmEDGE_NONE=-1,///<\en No edges found
};

///\en Execution modes (bit flags). Specified in \ref gmManager::set_execution() to select which type of execution is allowed for processing.
enum gmEXE_MODES{
  gmEXE_SERIAL=0x0, ///<\en The code is executed serially, no parallel processing is employed.
  gmEXE_LOCAL=0x1, ///<\en Nodes are allowed to execute as local tasks regardless of their status
  gmEXE_REMOTE=0x2, ///<\en Remote nodes are executed remotely (by creating jobs via \ref gmJobManager)
  gmEXE_CONSTRUCT_ONLY=0x4, ///<\en Nodes are never executed, only the execution graph is constructed
  gmEXE_EXPLICIT_ONLY=0x8, ///<\en No implicit nodal code is executed, no recursion
  gmEXE_THREADS=0x10, ///<\en Local nodes may execute in parallel by different threads within the same process
  gmEXE_MIXED=0x13, ///<\en Nodes are allowed to execute in all available ways (locally, remotely and using threads)
  //gmEXE_LOCAL_UNSPLIT=0x4  ///<\en local, without subtasks: main() is called in worker mode
};

///\en Operation modes of \ref gmManager (bit flags). This modes are in most cases assigned in low level processing functions and are for reference only.
enum gmMODES{
  gmMODE_LOCAL=0, ///<\en Default setting, the modes are automatically selected in course of operation
  gmMODE_MANAGER=0x1,///<\en Same as gmMODE_CONSTRUCT
  gmMODE_CONSTRUCT=0x1, ///<\en Construction mode in which the graph is assembled
  gmMODE_WORKER=0x2, ///<\en Worker mode for execution of the worker nodes
  gmMODE_FINAL=0x4, ///<\en used in combination with worker mode to process final node branch serially
};


///\en Types of links used to connect GridMD nodes.
enum gmLINKS{
  gmLINK_HARD=0x1,///<\en Hard (logical) link
  gmLINK_DATA=0x2,///<\en Data link
  gmLINK_PROC=0x4,///<\en Process link (subtype of hard link)
  gmLINK_LOCAL=0x8, ///<\en Hard link to intermediate 'finish' node (does not require simultaneous node processing)
  gmLINK_FILE=0x10, ///<\en File link (to be used as subtype of data link)
  gmLINK_STATUS=0x20, ///<\en Status link (to be used as subtype of file link)
  gmLINK_WCARD=0x40, ///<\en Wildcard link (to be used as subtype of file link)
  gmLINK_OPTDATA=0x80, ///<\en Link data is optional
  gmLINK_ALL=0xffff ///<\en All link types
};


///\en Fields to output in the graph file for Graphviz visualization
enum gmGV_FIELDS{
  gmGV_NODENAME=0x1,///<\en node name
  gmGV_LINKNAME=0x2,///<\en link (file) name
  gmGV_DESTNAME=0x4,///<\en link destination name
  gmGV_NODENUM=0x8,///<\en node number
  gmGV_NODESTATE=0x10, ///<\en node state
  gmGV_THREAD=0x20, ///<\en execution thread
  gmGV_CLUSTER=0x40,///<\en index of hard-linked cluster
  gmGV_LINKLABLES=0x80, ///<\en annotate link information as edge labels. (tooltip is used on opposite case.)
  gmGV_NODELABELS=0x100,
  gmGV_NODECOLORS=0x200,
  gmGV_NO_EMPTY_NODES=0x400, //<\en plot empty nodes as point.
  gmGV_COMMAND_TEXT=0x800,
	gmGV_DLINKPROP=0x01000, //<\en annotate detailed detailed link properties (status, optional, etc.)
  gmGV_ALL=0xfffffff ///<\en all
};


///\en Bit flags to indicate temporary files settings in \ref gmManager::set_link_files().
enum gmTMP_FILES{
  gmFILES_LOCAL=0x1, ///<\en Create link files even for local mode
  gmFILES_CLEANUP=0x2, ///<\en Cleanup all temporary link files after all connected nodes are processed
};


///\en Node execution states. These states are in most cases assigned automatically 
///    (except for \ref gmNS_PROC, \ref gmNS_UNPROC, \ref gmNS_WAIT).
enum gmNODE_STATES{
  gmNS_NO_NODE=-1, ///<\en node does not exist or state is not defined
  gmNS_UNPROC=0,   ///<\en node is unprocessed
  gmNS_PROC=1,     ///<\en node is processed (finished)
  gmNS_BLOCK=2,    ///<\en execution of a node is blocked by missing link data
  gmNS_WAIT=3,     ///<\en execution of a node is manually delayed
  gmNS_FAIL=6,     ///<\en node execution has failed
  gmNS_EXE=7,      ///<\en node is executing (node job submitted)
  gmNS_TARG=8,     ///<\en node is a thread target (internal markup)
  gmNS_TEMP=9,     ///<\en node is temporary, created automatically (internal markup)
  gmNS_SCHED=10,   ///<\en node is considered for scheduling (internal markup)
	gmNS_VOID=11,    ///<\en void (deleted) node, not taken into account by graph operations
};


///\en Result type of callback functions (see \ref gmRESULTS).
typedef int gmRESULT;

///\en Result codes for callback functions.
enum gmRESULTS {
	gmFAIL = -1,
	gmSKIP = 0,
  gmDONE = 1,
	gmREDO = 2,
};

class gmManager;
class gmLinkData;
template <class T>
class gmData;
class gmSelector;
class gmMutex;
class gmManagerCallback;

#endif // GMDEFS_H
