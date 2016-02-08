/*e***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2007-2010        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD
 *
 *   $Revision: 1.17 $
 *   $Date: 2016/02/03 16:41:18 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmnode.h,v 1.17 2016/02/03 16:41:18 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmnode.h,v $
$Revision: 1.17 $
$Author: valuev $
$Date: 2016/02/03 16:41:18 $
*/
/*e****************************************************************************
 * $Log: gmnode.h,v $
 * Revision 1.17  2016/02/03 16:41:18  valuev
 * fixes
 *
 * Revision 1.16  2015/11/05 11:22:28  valuev
 * fixed trajectory saving (regular sequence)
 *
 * Revision 1.15  2015/03/27 18:23:15  valuev
 * edges
 *
 * Revision 1.14  2015/03/20 15:46:36  valuev
 * node name changes
 *
 * Revision 1.13  2015/03/17 13:51:36  valuev
 * some fixes for scheduling
 *
 * Revision 1.12  2015/03/16 16:10:20  valuev
 * working on sweep farm
 *
 * Revision 1.11  2015/01/30 09:08:04  valuev
 * working on sweeps
 *
 * Revision 1.10  2014/11/12 17:26:10  valuev
 * added sweep farm project
 *
 * Revision 1.9  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.8  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.7  2013/03/07 17:08:59  valuev
 * kintech xml
 *
 * Revision 1.6  2012/11/22 17:01:06  valuev
 * sync with kintech svn
 *
 * Revision 1.5  2012/11/21 17:06:15  valuev
 * sync with kintech svn
 *
 * Revision 1.4  2012/11/06 11:35:41  valuev
 * added resource per node specification
 *
 * Revision 1.3  2012/10/29 11:10:14  valuev
 * mdified queueing (not working)
 *
 * Revision 1.2  2012/10/23 17:04:33  valuev
 * adding node actions
 *
 * Revision 1.1  2012/10/23 10:44:56  valuev
 * node action
 *
 *
*******************************************************************************/

# ifndef GMNODE_H
# define GMNODE_H

/**\en @file gmnode.h
       @brief Classes for workflow graph nodes
  
*/

/**\ru @file gmnode.h
       @brief Классы для работы с узлами графа сценария

*/

#include <string>
#include "gmdefs.h"
#include "refobj.h"  // mngptr
//#include "gridmd.h"
#include "jobmngr/jobmngr.h" // gmJobTiming type


///\en Error categories appearing by graph execution
enum gmERROR_CODES{
  gmERR_NONE = 0,
  gmERR_GRAPH, ///<\en Invalid graph (redundant, missing nodes, etc.)
  gmERR_RESOURCE,  ///<\en Misconfigured resource
  gmERR_CONNECTION, ///<\en Error while communicating data (incl. network errors)
  gmERR_FILE, ///<\en Missing files
  gmERR_EXECUTION, ///<\en Error status received when executing application or command
  gmERR_QUEUE, ///<\en Error status received by resource scheduler
};


///\en Graph element, base class for nodes and edges.
class sysGraphElm{
  std::string label;
public:
  sysGraphElm(const std::string &slabel=""):label(slabel){}
  std::string GetLabel() const { return label;}
	///\en Sets new label.
	///    \return old label
  std::string SetLabel(const std::string &newlabel){ 
		std::string oldlabel = label;
		label = newlabel;
		return oldlabel;
	}
  

  /*int type; //e< element type:logical or processing
  int state; //e< execution state: pending,scheduled,failed,executing,done
  gmdString id; //e< unique identifier in the graph

  int SetStatus(int sstat){
    state=sstat;
  }*/
};



class sysDataPort{
public:
  virtual bool IsConnectableTo(const sysDataPort &other) const{
    (void)other;
    return false;
  }
  virtual bool IsConnectableFrom(const sysDataPort &other) const{
    (void)other;
    return false;
  }
  virtual int DumpToFile(FILE *f){
    (void)f;
    return -1;
  }
  virtual int GetFromFile(FILE *F){
    (void)F;
    return -1;
  }
  virtual int CopyFrom(const sysDataPort &other){
    (void)other;
    return -1;
  }
  virtual int CopyTo(sysDataPort &other) const {
    (void)other;
    return other.CopyFrom(*this);
  }
};

template<class data_tt>
class gmDataPort: public sysDataPort{
  mngptr<data_tt> mptr;
public:
  gmDataPort(data_tt *dataptr=NULL){
    if(!dataptr)mptr.reset(new data_tt,1);
    else mptr.reset(dataptr,0);
  }
  virtual bool IsConnectableTo(const sysDataPort &other) const{
    return false;
  }
  virtual bool IsConnectableFrom(const sysDataPort &other) const{
    return false;
  }
  virtual int DumpToFile(FILE *f){
    return -1;
  }
  virtual int GetFromFile(FILE *F){
    return -1;
  }
  virtual int CopyFrom(const sysDataPort &other){
    return -1;
  }
  virtual int CopyTo(sysDataPort &other) const {
    return other.CopyFrom(*this);
  }
};

struct gmVoid{};

template<>
class gmDataPort<gmVoid>: public sysDataPort{
public:
    gmDataPort(gmVoid *dataptr=NULL){
        (void)dataptr;
  }
  virtual bool IsConnectableTo(const sysDataPort &other) const{
    (void)other;
    return true;
  }
  virtual bool IsConnectableFrom(const sysDataPort &other) const{
    (void)other;
    return true;
  }
  virtual int DumpToFile(FILE *f){
    (void)f;
    return 0;
  }
  virtual int GetFromFile(FILE *F){
    (void)F;
    return 0;
  }
  virtual int CopyFrom(const sysDataPort &other){
    (void)other;
    return 1;
  }
  virtual int CopyTo(sysDataPort &other) const {
    (void)other;
    return 1;
  }
};


class sysGraphNode;

///\en Properties of the node affected by the user configurable settings. These properties may either be explicitly
///    assigned using public members or are assigned in controllable way via gmManager node manipulation commands (in case of private members). 
struct gmNodeProp {
friend class gmManager;
  std::string blockname;
  bool final;
  std::string application_call;
  int workdir_id;
public: 
  bool local;         ///<\en if true, the node is executed locally (using resources on the client only)
  std::string resource;    ///<\en the name of resource for execution of the node, assigned automatically if empty
  std::string info;   ///<\en  Extra job info that can be saved into 'info' file in the working directory
  unsigned nproc;     ///<\en total number of processes, 0 if unset
  unsigned ppn;       ///<\en number of processes per node, 0 if unset
  unsigned walltime;  ///<\en maximum execution wall time in seconds, 0 if unset
  bool mpi;           ///<\en indicates that this is an MPI job
  bool force_restart; ///<\en if true, the node will always be restarted when the graph is recovered
	std::vector<std::string> string_data; ///<\en arbitrary character data stored in xml restart file with the node
	gmJobTiming timing; ///<\en filled with time measurements for remote jobs
  
  
  gmNodeProp(const std::string &blockname_=""):
    blockname(blockname_),
    final(false),
    workdir_id(-1),
    local(false),
    resource(""),
    nproc(0),
    ppn(0),
    walltime(0),
    mpi(false),
    force_restart(false)
    {}
};


///\en Class that specifies actions while node is being executed (callback functions,
///    satus updates, etc.). The node and the execution graph is accesible via calling 
///    \ref gmNodeAction::GetParent
class gmNodeAction {
friend class gmManager;
friend class gmScheduler;
  ///\en parent is set by gmManager
  sysGraphNode *m_parent; 
    ///\en manager is set by gmManager
  gmManager *m_manager; 

protected:
  void SetParent(sysGraphNode *parent){
    m_parent=parent;
  }
  void SetManager(gmManager *manager){
    m_manager=manager;
  }

	int OnExecuteWrapped();

public:
  gmNodeAction():m_parent(NULL){}

  sysGraphNode *GetParent(){
    return m_parent;
  }
  gmManager *GetManager(){
    return m_manager;
  }
  //virtual int OnSchedule() { return 0; }
  virtual int OnExecute() { return 0; }
  virtual gmNodeAction* Clone() const { return new gmNodeAction(); }
};



//e graph node
class sysGraphNode: public sysGraphElm{
  friend class gmManager;
  friend class gmGraph;
  friend class gmScheduler;
  friend class vertex_writer;
  //friend class gmGraph::vertex_writer;
protected:
  gmNodeID id;

  int inports; /// temporary
  int outports;  
  double ts0; ///<\en scheduled (model) begin execution time
  double ts1; ///<\en scheduled (model) end time
  double t0; ///<\en real begin execution time
  double t1; ///<\en real execution time
  double progress; ///<\en execution progress in %, <0 means not started

  gmERROR_CODES error_code;
  std::string error_message;
	bool read; ///<\en set 'true'if the node was read from restart file
  
  gmNodeProp userprop;
  mngptr<gmNodeAction> useraction; 
  bool implicit_action;  ///<\en true if the node may have associated implicit code block
public:

  sysGraphNode(const std::string &slabel="", int final_=0):sysGraphElm(slabel),inports(0),outports(0), error_code(gmERR_NONE),read(false),implicit_action(false){
    id = gmNODE_NONE;
    ts0=0;
    ts1=-1;
    t0=-1;
    t1=-1;
    progress=-1.;
    userprop.final=final_ ? true: false;
  }

  std::string GetName() const { return GetLabel();}

  gmNodeID GetID() const {
    return id;
  }

  std::vector<std::string> &GetStringData() {
    return userprop.string_data;
  }

  virtual int GetInpCount() const{
    return inports;
  }
  ///\en Tells whether the node is final in the graph.
  virtual int GetFinal() const {
    return userprop.final ? 1 : 0;
  }
  ///\en Tells whether the node is local.
  virtual int GetLocal() const {
    return userprop.local ? 1 : 0;
  }
  ///\en Tells whether the node is a call to external program.
  virtual int GetExternal() const {
    return 0;
  }
  virtual int GetOutCount() const{
    return outports;
  }
  virtual sysDataPort *Input(int portn) const{
    (void)portn;
    return NULL;
  }
  virtual sysDataPort *Output(int portn) const{
    (void)portn;
    return NULL;
  }
  virtual int isVirtual() const{
      return useraction.ptr() == NULL
          && userprop.application_call.empty()
              && !implicit_action;
  }

  ///\en Gets error code and optionally (if message !=0) error message 
  virtual gmERROR_CODES GetErrorStatus(std::string *message = NULL) const {
    if(message)
      *message = error_message;
    return error_code;
  }

};





/*
/// GridMD graph node
class gmGraphNode: public sysGraphNode{
protected:
  refvector<sysDataPort> inputs;
  refvector<sysDataPort> outputs;
public:
  gmGraphNode(const std::string &slabel=""):sysGraphNode(slabel),inputs(1),outputs(1){};

  virtual int GetInpCount() const{
    return (int)inputs.size();
  }
  virtual int GetOutCount() const{
    return (int)outputs.size();
  }
  virtual sysDataPort *Input(int portn) const{
    return inputs[portn];
  }
  virtual sysDataPort *Output(int portn) const{
    return outputs[portn];
  }
};


template<class input_tt=gmVoid, class output_tt=input_tt>
class gmSimpleNode: public sysGraphNode{
public:
  typedef gmDataPort<gmVoid> port0_t;
  typedef gmDataPort<input_tt> port1_t;
  typedef gmDataPort<output_tt> port2_t;
protected:
  port0_t port0;
  port1_t port1;
  port2_t port2;
  int inpc, outc;
public:
  /// setting zero pointers means that the ports will not be visible from outside
  /// the entries data will be created and  the ports may be accessible by internal functions
  gmSimpleNode(const std::string &slabel="",int local_=0,input_tt *pport1=NULL,output_tt *pport2=NULL):port1(pport1),port2(pport2),sysGraphNode(slabel,local_){
    if(!pport1)inpc=1;
    else inpc=2;
    if(!pport2)outc=1;
    else outc=2;
  }

  virtual int GetInpCount() const{
    return inpc;
  }
  virtual int GetOutCount() const{
    return outc;
  }
  virtual sysDataPort *Input(int portn) const{
    if(portn==0 || inpc==1)return (sysDataPort *)&port0;
    else return (sysDataPort *)&port1;
  }
  virtual sysDataPort *Output(int portn) const{
    if(portn==0 || outc==1)return (sysDataPort *)&port0;
    else return (sysDataPort *)&port2;
  }

};


template<class input_tt, class output_tt>
class gmDataProcNode: public gmSimpleNode<input_tt,output_tt>{
public:
  gmDataProcNode(const std::string &slabel="",int local_=0,input_tt *pport1=NULL,output_tt *pport2=NULL):gmSimpleNode<input_tt,output_tt>(slabel,local_,pport1,pport2){}
};
*/


class sysGraphLink:public sysGraphElm{
  friend class gmManager;
  int type;
  int srcport, destport;
  std::string name;
  std::string dest_name;
public:
  int state; // 0=ok,1=cleaned up
  sysGraphLink(int stype=0, const std::string &sname="", int srcport_=0, int destport_=0, const std::string &dest_name_=""):
    type(stype),srcport(srcport_),destport(destport_),name(sname),dest_name(dest_name_),state(0){
    if(dest_name=="")
      dest_name=name;
  }
  virtual int GetType() const {
    return type;
  }
  virtual const std::string &GetName() const {
    return name;
  }

  virtual const std::string &GetDestName() const {
    return dest_name;
  }
  virtual int GetSrcPort() const {
    return srcport;
  }
  virtual int GetDestPort() const {
    return destport;
  }
};

typedef sysGraphNode gmNode;
typedef sysGraphLink gmEdge;

//template <class T=int> struct node_type; 

# endif
