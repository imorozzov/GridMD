/*e***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2007-2010        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD
 *
 *   $Revision: 1.55 $
 *   $Date: 2016/02/03 16:41:18 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/workflow.h,v 1.55 2016/02/03 16:41:18 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/workflow.h,v $
$Revision: 1.55 $
$Author: valuev $
$Date: 2016/02/03 16:41:18 $
*/
/*e****************************************************************************
 * $Log: workflow.h,v $
 * Revision 1.55  2016/02/03 16:41:18  valuev
 * fixes
 *
 * Revision 1.54  2015/06/17 19:37:59  valuev
 * sweep farm restructured
 *
 * Revision 1.53  2015/05/12 13:53:45  valuev
 * new version with all functions in read node
 *
 * Revision 1.52  2015/03/27 18:23:15  valuev
 * edges
 *
 * Revision 1.51  2015/03/27 05:36:43  valuev
 * getting edges
 *
 * Revision 1.50  2015/03/20 15:46:36  valuev
 * node name changes
 *
 * Revision 1.49  2015/01/30 09:08:04  valuev
 * working on sweeps
 *
 * Revision 1.48  2014/09/26 16:54:19  valuev
 * syncronizing with kintech svn
 *
 * Revision 1.47  2013/07/04 14:06:24  valuev
 * extended file management
 *
 * Revision 1.46  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.45  2012/12/28 17:14:30  valuev
 * added node state callbacks, fixed restart
 *
 * Revision 1.44  2012/12/24 17:01:58  valuev
 * fixed local link check
 *
 * Revision 1.43  2012/12/21 16:23:05  valuev
 * unified node states
 *
 * Revision 1.42  2012/12/11 17:24:36  valuev
 * updated scheduling, node state changes on the fly
 *
 * Revision 1.41  2012/11/29 18:14:48  valuev
 * sync with svn
 *
 * Revision 1.40  2012/11/21 17:06:15  valuev
 * sync with kintech svn
 *
 * Revision 1.39  2012/11/08 14:32:50  valuev
 * various extensions
 *
 * Revision 1.38  2012/11/07 18:08:08  valuev
 * sync with svn
 *
 * Revision 1.37  2012/10/30 11:39:58  valuev
 * modified queueing (working)
 *
 * Revision 1.36  2012/10/25 14:15:04  valuev
 * updated explicit workflow
 *
 * Revision 1.35  2012/10/23 17:04:33  valuev
 * adding node actions
 *
 * Revision 1.34  2012/10/23 10:44:56  valuev
 * node action
 *
 * Revision 1.33  2012/10/19 09:18:24  valuev
 * started modifying nodesffffffffffffffffffff
 *
 * Revision 1.32  2012/09/07 14:26:06  valuev
 * removed using namespace from h files
 *
 * Revision 1.31  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 * Revision 1.30  2012/08/29 16:50:28  valuev
 * restructured gridmd for VS10
 *
 * Revision 1.29  2011/07/21 14:33:16  valuev
 * made compatible with wxWidgets 2.9 (unicode)
 *
 * Revision 1.28  2011/01/25 10:07:35  valuev
 * some new fixes
 *
 * Revision 1.27  2010/11/20 18:08:22  valuev
 * added GridMD implementation to tcpengine
 *
 * Revision 1.26  2010/11/17 02:17:16  valuev
 * added file links
 *
 * Revision 1.25  2010/10/21 16:02:01  morozov
 * Release 2.1
 *
 * Revision 1.24  2010/10/14 16:20:29  valuev
 * added link file cleanup
 *
 * Revision 1.23  2010/10/14 15:02:29  morozov
 * Pre-release 2.1 (windows version). Corrected documentation and examples.
 *
 * Revision 1.22  2010/10/14 09:52:49  valuev
 * added link data analysis and node restart on failed links
 *
 * Revision 1.20  2010/10/07 16:03:44  valuev
 * fixed restart mechanism
 *
 * Revision 1.19  2010/10/07 11:20:31  valuev
 * preliminary program restart
 *
 * Revision 1.18  2010/10/06 13:51:58  valuev
 * added xml graph, fixed memory leaks in JobManager
 *
 * Revision 1.17  2010/10/01 09:13:51  valuev
 * added resource loading from xml
 *
 * Revision 1.16  2010/10/01 07:01:04  valuev
 * added xml resource saving
 *
 * Revision 1.15  2010/09/17 19:36:18  valuev
 * changed fork function names, added regular expressions
 *
 * Revision 1.14  2010/08/10 17:15:16  morozov
 * Release 2.0
 *
 * Revision 1.13  2010/08/07 06:16:58  valuev
 * fixed job states at queueing
 *
 * Revision 1.12  2010/08/05 07:04:57  valuev
 * fixed remote job submission
 *
 * Revision 1.11  2010/07/31 16:12:04  valuev
 * modified workflow interface
 *
 * Revision 1.10  2010/07/28 14:37:00  valuev
 * Added documentation to workflow
 *
 * Revision 1.9  2010/07/22 17:14:15  valuev
 * fixed interface with jobmanager
 *
 * Revision 1.8  2010/07/21 22:20:39  valuev
 * fixed argc, argv
 *
 * Revision 1.7  2010/07/21 21:17:07  valuev
 * fixed bottom_node
 *
 * Revision 1.6  2010/07/21 19:14:09  valuev
 * modified link names
 *
 * Revision 1.5  2010/07/21 18:36:50  valuev
 * added file transfer
 *
 * Revision 1.4  2010/07/21 14:54:14  valuev
 * standalone worker calls exit instead of throwing task_finished()
 *
 * Revision 1.3  2010/07/21 14:03:45  valuev
 * workflow for jobmanager
 *
 * Revision 1.2  2010/06/12 20:01:45  valuev
 * compiled with icc 10
 *
 * Revision 1.1  2010/06/12 18:03:54  valuev
 * added workflow files
 *
*******************************************************************************/

# ifndef WORKFLOW_H
# define WORKFLOW_H

/**\en @file workflow.h
       @brief Workflow definition classes
  
*/

/**\ru @file workflow.h
       @brief Классы для работы с распределенными сценариями 
+

*/

#include <vector>
#include <string>

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/pending/disjoint_sets.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/algorithm/string/replace.hpp>
//#include <boost/vector_property_map.hpp>
#include <stdexcept>
#include <string>
#include <fstream>


#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>

//using namespace boost;



#include <stdio.h>
#include <gmd/app.h>
#include <gmd/utils.h>

#include "refobj.h"
#include "gridmd.h"
#include <gmd/threadpool.h>
//#include "gmnode.h"


class invalid_mark: public logic_error{
public:
  invalid_mark(const string& message):logic_error(message){}
};


class gmGraph {
  friend struct link_type;
  friend class gmManager;
  friend class gmScheduler;
  template<class, class> friend struct set_flag;
  template<class, class> friend struct node_type;

  typedef boost::property<boost::edge_index_t, int> int_prop;

  ///\en number of newly formed threads formed by process_step, updated on each process_step call 
  size_t new_threads;
  
public:
  ///\en Node states as returned by \ref get_node_state_by_id()
  /*enum gmNODE_STATES {
    NO_NODE=-1, ///<\en node does not exist
    UNPROC=0,   ///<\en node is unprocessed
    PROC=1,     ///<\en node is processed (finished)
    BLOCK=2,    ///<\en execution of a node is blocked by missing link data
    WAIT=3,     ///<\en node is delayed
    TARG=4,     ///<\en node is a thread target (internal markup) 
    TEMP=5,     ///<\en node is temporary, created automatically (internal markup)
    FAIL=6,     ///<\en node execution has failed
    EXE=7,      ///<\en node is executing (node job submitted)   
  };*/

  struct vertex_prop_t {
    int nodeid;
    enum gmNODE_STATES flag;
    bool force;
    int max_restarts;
    size_t thread_start;
    std::vector<int> threads;
    vertex_prop_t(int nodeid_=-1,enum gmNODE_STATES flag_=gmNS_UNPROC):nodeid(nodeid_),flag(flag_),thread_start(0),max_restarts(-1),force(false){}
  };
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, vertex_prop_t, int_prop> graph_t;
  ///\en vectors for indexing hard-linked components
  boost::vector_property_map<int> rank, parent;
  ///\en disjoint set object for finding hard-linked components
  boost::disjoint_sets<boost::vector_property_map<int> ,boost::vector_property_map<int> >  ds;

  ///\en job thread descriptor
  struct wthread_t{
    int state;
    string jobid;
    gmTaskID taskid; ///<\en for threadpool tasks 
    string stderr_;
    string stdout_;
    int resource_id;
    int restarts;
		int needs_redo; // redo is normal behaviour unlike restart!
    int exetype;
    bool persistent;
    bool stopped;
    std::vector<graph_t::vertex_descriptor> nodes;  ///<\en list of all nodes
    std::vector<graph_t::edge_descriptor> idlinks; ///<\en list of all incoming data links
    std::vector<graph_t::edge_descriptor> odlinks; ///<\en list of all outgoing data links
    bool recursive; ///<\en true if thread is executed by the same process using recursion
    gmERROR_CODES error_code;
    std::string error_message;
    bool read; ///<\en indicates that the tread is read from file
    wthread_t():state(-1),exetype(gmEXE_LOCAL),resource_id(-1),restarts(0),persistent(false),
			stopped(false),recursive(false),error_code(gmERR_NONE),needs_redo(0), read(false), taskid(GMPOOLTASK_INVALID_ID) {}
  };
protected:
 
  graph_t g;
  boost::property_map<graph_t, boost::edge_index_t>::type edgeid;
  typedef map<gmNodeID,int> group_map_t;
  group_map_t gmap;
  std::vector<string> gnames;

  refvector<sysGraphNode> nodes;
  ///\en vertex descriptors corresponding to nodes
  std::vector<graph_t::vertex_descriptor> vdnodes; 
  refvector<sysGraphLink> links;
  gmNodeID root;
  gmNodeID prev;

  ///\en Node state changes are pusged_back to this vector in order of appearence.
  vector< std::pair<gmNodeID, gmNODE_STATES> > event_stack;
 
  ///\en starting index of unfinished threads
  //size_t thread_start;
  std::vector<wthread_t> threads;
  int wr_flags;

   ///\en This is called to inform the manager of node state changes in course of graph algorithm
  gmManagerCallback *callback;
  
  gmNodeProp &get_nodeprop(sysGraphNode *node){
    return node->userprop;
  }

  class edge_writer{
    gmGraph *parent;
  public:
    edge_writer(gmGraph *par):parent(par){}
    template< class descr_t> 
    void operator()(std::ostream& out, const descr_t& v) const {
      int i=parent->edgeid[v]; 
      int flags=parent->wr_flags;
      if(i>=0){
        if(parent->links[i]->GetType()&gmLINK_DATA) {    
          out << "\t[style=\"dashed\"";
          string label, dest;
          if(flags&gmGV_LINKNAME)
            label=parent->links[i]->GetName();
            //if(label.size()>10) label="..."+label.substr( label.size()-7 );
          if(flags&gmGV_DESTNAME){
            dest=parent->links[i]->GetDestName();
            //if(dest.size()>10) dest="..."+dest.substr( dest.size()-7 );
            if(dest!=label){
              label+=" -> ";
              label+=dest;
            }
          }
					if(flags&gmGV_DLINKPROP){
						if(parent->links[i]->GetType()&gmLINK_OPTDATA)
						{
							 label+=" (opt.)";
						}
						if(parent->links[i]->GetType()&gmLINK_STATUS)
						{
							 label+=" (status)";
						}
					}

          label=boost::algorithm::replace_all_copy( label, "\\", "/" );
          if(label!=""){
            if(flags&gmGV_LINKLABLES)
                out <<" label=\" "<<label<<"\"";
            else
                out <<" tooltip=\""<<label<<"\"";
          }
          out<<"]";
        }
        else if(parent->links[i]->GetType()&gmLINK_PROC)
          out << "[arrowhead=\"none\" style=\"setlinewidth(3)\"]";
        else if(parent->links[i]->GetType()&gmLINK_LOCAL)     
          out << "[arrowhead=\"odot\"]";
        // all others (HARD) are drawn with solid lines and arrows
      }
      else
        out << "[label=\"tmp\"]";
    }
  };
  
  class vertex_writer{
    gmGraph *parent;
    bool write_groups;
    static std::string gmGetStateCollor(gmNODE_STATES state);

  public:
    vertex_writer(gmGraph *par):parent(par),write_groups(true){}
    template< class descr_t> 
    void operator()(std::ostream& out, const descr_t& v) const {
      int i=parent->g[v].nodeid;
      int flags=parent->wr_flags;
      //const char *st[]={"UNPROC","PROC","BLOCK","WAIT","TARG","TEMP","FAIL", "EXE"};
      //const char *state=st[parent->g[v].flag];
      string state=gmGetStateName(parent->g[v].flag);
      string collor=gmGetStateCollor(parent->g[v].flag);
      string nums=fmt("%d",i);
      if(parent->g[v].threads.size() && flags&gmGV_THREAD){
        nums+=" (";
        for(size_t j=0;j<parent->g[v].threads.size();j++){
          nums+=fmt("%d",parent->g[v].threads[j]);
          if(j!=parent->g[v].threads.size()-1)
            nums+=",";
        }
        nums+=")";
      }
      // if(flags&gmGV_CLUSTER){
      //   nums+=fmt("[%d]",parent->ds.find_set(i));
      // }

      if(i>=0){ // normal node
        string shape="Mrecord";
        string label;
        string splitter= (flags&gmGV_NODELABELS) ? "|" : "\t";
        if(flags&gmGV_NO_EMPTY_NODES)
        {
            if(parent->nodes[i]->isVirtual())
                shape="point";
        }
        if(flags&gmGV_NODENAME){
          label=parent->nodes[i]->GetLabel();
          if(parent->nodes[i]->GetFinal() || parent->nodes[i]->GetLocal())
            if(shape!="point")  // Mrecord -> record, making it square
              shape = "record";
          //if(parent->nodes[i]->GetLocal())
            //label+="(L)";
        }
        if(flags&gmGV_NODENUM){
          if(label!="")
            label+=splitter;
          label+=nums;
        }
        if(flags&gmGV_NODESTATE){
          if(label!="")
            label+=splitter;
          label+=state;
        }
        if(flags&gmGV_COMMAND_TEXT)
        {
          if(!parent->get_nodeprop(parent->nodes[i]).application_call.empty())
            {
                if(!label.empty())
                    label+=(flags&gmGV_NODELABELS) ? "|" : "&#x0D;&#x0A;";
                gmdString tmp=parent->get_nodeprop(parent->nodes[i]).application_call;
                tmp.Replace( "&", "&amp;" );
                tmp.Replace( "\"", "''" );
                tmp.Replace( "\\", "\\\\" );
                tmp.Replace( ">", "&gt;" );
                tmp.Replace( "<", "&lt;" );
                //tmp.Replace( "\n", "&br;" );
                tmp.Replace( "\x0D", "&#13;" );
                tmp.Replace( "\x0A", "&#10;" );

                label+= tmp;
            }
        }
        out << "[shape=\""<< shape << "\"";

        if(flags&gmGV_NODECOLORS)
            out<<" style=filled " <<" color=" << collor ;

        if(flags&gmGV_NODELABELS)
            out<<" label=\"{" << label <<"}\"]";
        else
            out<<" tooltip=\"" << label <<"\" label=\""<< i <<"\" ]";
      }
      else // auxiliary node
        out << "[shape=\"Mrecord\" label=\"{AUX}\"]";

      if(i==parent->nodes.size()-1 && write_groups){ // the last node
        string s=parent->format_groups();
        if(s!=""){
          out << ";\n" << s;
        }
      }
    }
  };

  string format_groups() const;

  template< typename graph_t >
  int write(const graph_t &graph, const char *filename, int flags=gmGV_ALL){
    ofstream file;
    file.open(filename);
    wr_flags=flags;
    write_graphviz(file, graph, vertex_writer(this),edge_writer(this));
    file.close();
    return 1;
  }
  template< typename graph_t >
  int write_no_edgetypes(const graph_t &graph, const char *filename, int flags=gmGV_ALL){
    ofstream file;
    file.open(filename);
    wr_flags=flags;
    write_graphviz(file, graph, vertex_writer(this));
    file.close();
    return 1;
  }

public:
  gmGraph();
  ///\en Adds a display group with given name. Used for graphwiz display.
  int add_group(const string &name="");
  ///\en Finds a display group with given name. 
  ///\en \return group ID >=0 or <0 on failure
  int find_group(const string &name) const ;

  ///\en Adds a node to the graph. Optional display group with given gropup id is used for graphwiz display.
  ///    If >=0, groupid should reference the existig display group (see \ref add_group()). 
  gmNodeID add_node(sysGraphNode *node, int groupid=-1);
  
  /// -1 means first available source port
  ///    destination port is always new
  int link(gmNodeID node1, gmNodeID node2,int linktype=gmLINK_HARD,const string &name="",int srcport=-1,const string &dest_name="");


  gmNodeID get_prev_node(){
    return prev;
  }

  ///\en Returns node pointer by id
  sysGraphNode *get_node(gmNodeID node){
    if(node<0 || node >= (gmNodeID)nodes.size())
      return NULL;
    else 
      return nodes[node];
  }

  int write(const char *filename, int flags=gmGV_ALL);


/*  1.  Specify `start' root node as `processed' and all other nodes as
        unprocessed.
    2.  Specify all nodes having incoming data links with
        unprocessed sources as `blocked'. Exit with deadlock indication if
        the number of blocked nodes is nonzero and did not change 
        since the previous iteration.
    3.  Specify all nodes connected with blocked nodes by recursively following outgoing logical
        links as blocked.
    4.  Find the maximal set of unprocessed non-blocked nodes connected
        with processed nodes by recursively following all outgoing links.
    5.  Find all nodes
        from the set of step 3 that have outgoing data links to blocked
        nodes or coincide with the `finish' root node.
    6.  For each node found in step 4 find the
        maximal subgraph connected with it by incoming logical links
    7.  For each subgraph of step 5 list
        all input files corresponding to incoming data links entering the
        subgraph
    8.  Form worker tasks from subgraphs of step 5 and
        corresponding files of step 6.
    9.  Execute the worker tasks of step 7, collect all
        files corresponding to outgoing data links of the subgraphs' nodes
        (or put these tasks in the submission script). Wait until at least one of
        the subgraph tasks finishes.
    10. Mark all nodes in all finished
        subgraphs of step 7 as 'processed'.
    11. Finish if there are no
        unprocessed nodes, otherwise go to step 2.
*/

  ///1.  Specify `start' root node as `processed' and all other nodes as unprocessed,
  ///    clear the thread (job) id
  bool process_start();

  ///\en Returns \a true if the node has incoming data links to unprocessed nodes 
  bool is_blocked_directly(graph_t::vertex_descriptor v);

  ///\en Returns \a true if the node has outgoing data links to blocked nodes 
  ///    or if it is a terminal node
  bool is_blocking(graph_t::vertex_descriptor v);


  ///\en \a graph_error indicates possible graph deadlocks (not used yet)
  bool process_step(int &graph_error);

  ///\en Selects nodes into a std::vector
  int select_nodes(std::vector< std::pair<int,int> > &snodes, gmSelector sel, bool input=true, int def_port=-1) const;

  ///\en Selects nodes into a std::vector with string regular expression
  int select_nodes_regex(std::vector< std::pair<int,int> > &snodes, const string &expr, bool input=true, int def_port=-1) const;


  ///\en Find ids of all data links connecting source and destination nodes/ports, adds them to \a linkids.
  ///    \returns the number of found links
  int find_links(int src_nodeid, int src_portid, int dst_nodeid, int dst_portid, std::vector<int> &linkids) const;

  ///\en Finds incoming data link to a node/port from another node
  ///    \returns (nodeid,portid) of the source or (-1,-1) if not found
  std::pair<int,int> get_inlink(int nodeid, int portid) const;

  ///\en Returns the edge descriptor of a link with given \a edge_id into \a ed.
  ///    \retval \a true on success, \a false otherwise.
  bool find_edge(int edge_id, graph_t::edge_descriptor &ed) const ;

  ///\en Returns the number of pending (NOT finished or failed) threads in the graph
  size_t pending_threads_count() const ;

  enum gmNODE_STATES get_node_state_by_id(gmNodeID nid) const {
    if(!(nid>=0 && (size_t)nid<nodes.size())) // node index out of array
      return gmNS_NO_NODE;
    graph_t::vertex_descriptor vi=vdnodes[nid];
    return g[vi].flag;
  }


  enum gmNODE_STATES set_node_state_by_id(gmNodeID nid, enum gmNODE_STATES state, bool force=false){
    if(!(nid>=0 && (size_t)nid<nodes.size())) // node index out of array
      return gmNS_NO_NODE;
    graph_t::vertex_descriptor vi=vdnodes[nid];
    return set_node_state(vi,state,force);
  }

  int set_node_error(graph_t::vertex_descriptor vi, gmERROR_CODES err_code, const std::string &err_message = ""){
    int nid = g[vi].nodeid;
    if(!(nid>=0 && (size_t)nid<nodes.size())) // node index out of array
      return -1;
    nodes[nid]->error_code = err_code;
    nodes[nid]->error_message = err_message;
    return 1;
  }

  enum gmNODE_STATES set_node_state(graph_t::vertex_descriptor vi, enum gmNODE_STATES state, bool force=false){
    //if(g[vi].nodeid == 2)
      //g[vi].nodeid = 2;
    enum gmNODE_STATES prev=g[vi].flag;
    if(!force){  // check previous state
      if(prev==gmNS_PROC) // PROC never changes
        return gmNS_NO_NODE;
      else if(prev==gmNS_WAIT) // WAIT never changes
        return gmNS_NO_NODE;
    }
    else{
      if(state==gmNS_NO_NODE){ // remove forcing
        g[vi].force=false;
        state = prev;
      }
      else if(state==gmNS_UNPROC) // remove forcing
        g[vi].force=false;
      else 
        g[vi].force=true; // indicates that flag will not change
    }
    g[vi].flag = state;
   
    if(g[vi].nodeid == -1) // don't report temporary nodes
      return prev;
    
    if(state==prev) // no actual change, don't report
      return prev;
    if(state == gmNS_UNPROC && (prev == gmNS_SCHED || prev == gmNS_TARG))// same as above, because SHED and TARG are eqivalent to UNPROC
      return prev;

    if(prev == gmNS_FAIL) // clear error status if changing form FAIL
      set_node_error(vi,gmERR_NONE);
    

    if(state == gmNS_SCHED ) // don't report transition to SCHED
      return prev;
    if(state == gmNS_TARG ) // don't report transition to TARG
      return prev;
    if(prev == gmNS_SCHED ){
      if(state==gmNS_BLOCK) // don't report SCHED->BLOCK (considered as same state)
        return prev;
      prev = gmNS_BLOCK;  // substitute previous state
    }

    if(callback)
      callback->NodeStateChanged(g[vi].nodeid,state,prev);
    // filling event hash: only changes are recorded
    event_stack.push_back( make_pair(g[vi].nodeid,state) );
    return prev;
  }

  int unprocess_recursively_by_id(int nid);

  
      

  ///\en Sets callback object for node state change. func->NodeStateChanged() is called for each state change caused by graph processing
  gmManagerCallback *set_callback(gmManagerCallback *func){
    std::swap(func, callback);
    return func; 
  }


  size_t get_new_threads_count() const {
    return new_threads;
  }
};



# endif
