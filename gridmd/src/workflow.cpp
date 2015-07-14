# include "workflow.h"

# include "boost/graph/filtered_graph.hpp"
# include "boost/graph/reverse_graph.hpp"

# include <gmd/regex.h>

using namespace std;
using namespace boost;

std::string gmGraph::vertex_writer::gmGetStateCollor(gmNODE_STATES state){
  switch(state){
    case gmNS_NO_NODE:
      return "orangered";
    case gmNS_UNPROC:
      return "lemonchiffon";
    case gmNS_PROC:
      return "green";
    case gmNS_BLOCK:
      return "lightgrey";
    case gmNS_WAIT:
      return "cyan";
    case gmNS_FAIL:
      return "red";
    case gmNS_EXE:
      return "yellow";
    case gmNS_TARG:
      return "darkseagreen";
    case gmNS_TEMP:
      return "yellow";
    case gmNS_SCHED:    
      return "blue";
  }
  return "lightyellow";
}



template <class Tag, class convert_t=gmGraph::graph_t>
struct set_flag{
  typedef Tag event_filter;
  enum gmNODE_STATES flag;
  gmGraph *parent;
  set_flag(enum gmNODE_STATES flag_=gmNS_UNPROC, gmGraph *parent_=NULL):flag(flag_), parent(parent_){}
  template <class X, class Graph> 
  void operator()(X v,Graph &g){
    convert_t *gp=(convert_t *)&g; // hack to break const constraint on Graph
    if((*gp)[v].flag!=gmNS_FAIL && (*gp)[v].flag!=gmNS_EXE && (*gp)[v].flag!=gmNS_PROC && (*gp)[v].flag!=gmNS_WAIT){ // fail, exe, proc, wait status remains
      if(!parent) // set flag directly
        (*gp)[v].flag=flag; // change the property
      else // set flag via node_state
        parent->set_node_state(v, flag /*, true*/);
    }
    //put(&gmGraph::vertex_prop_t::flag, g, v,flag);
  }
};

template <class Tag, class convert_t=gmGraph::graph_t>
struct set_unproc{
  typedef Tag event_filter;
  gmGraph *parent;
  set_unproc(gmGraph *parent_=NULL):parent(parent_){}
  template <class X, class Graph> 
  void operator()(X v,Graph &g){
    convert_t *gp=(convert_t *)&g; // hack to break const constraint on Graph
    if(!(*gp)[v].force){
      if(!parent) // set flag directly
        (*gp)[v].flag=gmNS_UNPROC; // change the property
      else // set flag via node_state
        parent->set_node_state(v, gmNS_UNPROC, true);      
    }
  }
};

template <class Tag, class convert_t=gmGraph::graph_t>
struct set_thread{
  typedef Tag event_filter;
  int flag;
  vector<gmGraph::wthread_t> *threads;
  set_thread(int flag_=0, vector<gmGraph::wthread_t> *threads_=NULL):flag(flag_),threads(threads_){}
  template <class X, class Graph> 
  void operator()(X v,Graph &g){
    convert_t *gp=(convert_t *)&g; // hack to break const constraint on Graph
    (*gp)[v].threads.push_back(flag); // change the proprty
    (*threads)[flag].nodes.insert((*threads)[flag].nodes.begin(),v); // adding nodes in specific order
  }
};


struct link_type {
  gmGraph *parent;
  int type;
  link_type(int type_=~0, gmGraph *parent_=NULL):type(type_),parent(parent_){ }
  template <typename Edge>
  bool operator()(const Edge& e) const {
    int i=parent->edgeid[e]; 
    if(i<0 || (parent->links[i]->GetType()&type))     
      return true;
    return false;
  } 
};

template<class T=int, class comp_t=equal_to<T> >
struct node_type {
  typedef T gmGraph::vertex_prop_t::* prop_t;
  gmGraph *parent;
  T type;
  prop_t prop;
  comp_t comp; //binary_function<int,int,bool>

  node_type(prop_t prop_=&gmGraph::vertex_prop_t::flag,T type_=0, gmGraph *parent_=NULL):type(type_),parent(parent_),prop(prop_){ }
  
  template <typename Vertex>
  bool operator()(const Vertex& v) const {
    return comp(parent->g[v].*prop,type);
  } 
};

/*
template <class Tag, class convert_t=gmGraph::graph_t>
struct set_target{
  typedef Tag event_filter;
  gmGraph *parent;
  gmGraph::graph_t::vertex_descriptor vaux;
  set_target(gmGraph::graph_t::vertex_descriptor vaux_):vaux(vaux_){}
  template <class X, class Graph> 
  void operator()(X v,Graph &g){
    comp_graph_t::out_edge_iterator ei, ei_end;
    boost::tie(ei,ei_end)= out_edges(v,gcomp); // obtaining incoming links for the node
    if(ei!=ei_end){ // has outgoing data links
      convert_t *gp=(convert_t *)&g; // hack to break const constraint on Graph
      (*gp)[v].flag=TARG; // mark node as target node
      add_edge(vaux,v,-1,g);
    }
  }
};*/

/*struct thread_descr_t{
  gmGraph::graph_t::vertex_descriptor target;

  
};*/



string gmGraph::format_groups() const {
  string s;
  int i, n=gnames.size();
  for(i=0;i<n;i++){
    s+=fmt("subgraph \"cluster%d\"{ label=\"",i);
    s+=gnames[i]+"\"; ";
    group_map_t::const_iterator it=gmap.begin();
    for(;it!=gmap.end();++it){
      if(it->second==i)
        s+=fmt("%d; ",it->first);
    }
    s+="}\n";
  }
  return s; 
}


gmGraph::gmGraph():prev(-1),nodes(1),links(1),ds(rank,parent), callback(NULL), new_threads(0) {
  root = -1; //root=add_node(new gmNode("start"));
  edgeid = get(edge_index, g);
  wr_flags=gmGV_ALL;
  //thread_start=0;
}

int gmGraph::add_group(const string &name){
  if(name=="")return -1;
  gnames.push_back(name);
  return (int)gnames.size()-1;
}


int gmGraph::find_group(const string &name) const {
  if(name=="")return -1;
  int i=0;
  for(;i<(int)gnames.size();i++){
    if(gnames[i]==name)
      return i;
  }
  return -2;
}

gmNodeID gmGraph::add_node(sysGraphNode *node, int groupind){
  nodes.push_back(node);
  prev=(gmNodeID)nodes.size()-1;
  graph_t::vertex_descriptor vd=add_vertex(vertex_prop_t(prev,gmNS_NO_NODE),g);
  vdnodes.push_back(vd);
  if(groupind>=0 && groupind<(int)gnames.size()){ // adding to the group
    gmap[prev]=groupind;
  }
  // managing hard-linked sets
  ds.make_set((int)nodes.size()-1);
  // changing node state NO_NODE->UNPROC with possible callback action
  set_node_state(vd,gmNS_UNPROC,true);
  return prev;
}

int gmGraph::write(const char *filename, int flags){
  return write(g,filename,flags);
  /*ofstream file;
  file.open(filename);
  wr_flags=flags;
  write_graphviz(file, g, vertex_writer(this),edge_writer(this));
  file.close();
  return 1;*/
}

/// -1 means first available source port
///    destination port is always new
int gmGraph::link(gmNodeID node1, gmNodeID node2,int linktype,const string &name,int srcport, const string &dest_name_){
  string link_name=name, dest_name= dest_name_=="" ? name : dest_name_;
  int destport=-1;
  if((linktype&gmLINK_FILE)==gmLINK_FILE){
    srcport=-1;
  }
  else if(linktype&gmLINK_DATA){
    int nextsrc=nodes[node1]->GetOutCount();
    destport=nodes[node2]->GetInpCount();
    if(srcport<0)
      srcport=nextsrc;
    else if(srcport>nextsrc) // source port out of range
      return LOGERR(-1,fmt("gmGraph.link: linking nodes %d and %d: source port %d does not exist",node1,node2,srcport),0); 
    if(srcport==nextsrc)
      nodes[node1]->outports++; // TODO: replace with proper port addition 
    nodes[node2]->inports++;
    if(name==""){ //automatic name selection
      link_name=fmt("%d_%d.out",node1,srcport);
      dest_name=fmt("%d_%d.in",node2,destport);
    }
  }
  else{
    srcport=-1; // port is not defined for hard links
   
    if(linktype==gmLINK_HARD){
      if(nodes[node1]->GetFinal()==1) // this is intermidiate execute node
        linktype=gmLINK_LOCAL; // linking to main graph with local link (no link in fact)
    }
  }
  
  links.push_back(new sysGraphLink(linktype,link_name,srcport,destport,dest_name));
  int ind=(int)links.size()-1;
  
  add_edge(vdnodes[node1],vdnodes[node2],ind,g);

  // managing hard-linked sets
  if(linktype==gmLINK_HARD)
    ds.union_set(node1,node2);
  
  return ind;
}



bool gmGraph::process_start(){ 
  graph_t::vertex_iterator vi, vi_end;
  boost::tie(vi,vi_end)=vertices(g);
  if(vi==vi_end)
    return false;
  g[*vi].thread_start=g[*vi].threads.size();//g[*vi].threads.clear();
  //g[*vi++].flag = gmNS_PROC;  // TODO process node as needed (calling local functions) 
  set_node_state(*vi,gmNS_PROC /*, true*/);
  ++vi;
  for(;vi!=vi_end;++vi){
    g[*vi].thread_start=g[*vi].threads.size(); //g[*vi].threads.clear();
    if(g[*vi].flag!=gmNS_FAIL && g[*vi].flag!=gmNS_PROC && g[*vi].flag!=gmNS_UNPROC && !g[*vi].force){ // if not processed or forced state
      set_node_state(*vi,gmNS_SCHED);
      //g[*vi].flag = gmNS_UNPROC;
    }
  }
  //write("graph.dot");
  return true;
}

//\en Returns \a true if the node has incoming data links to unprocessed nodes 
bool gmGraph::is_blocked_directly(graph_t::vertex_descriptor v){
  graph_t::in_edge_iterator ei, ei_end;
  boost::tie(ei,ei_end)=in_edges(v,g); // obtaining incoming links for the node
  for(;ei!=ei_end;++ei){
    int i=edgeid[*ei]; 
    if(i>=0 && links[i]->GetType()&gmLINK_DATA){ // this is the data link
      if(g[source(*ei,g)].flag!=gmNS_PROC) // the source is not processed
        return true; // this node is blocked
    }
  }
  return false; // this node is not blocked
}

//\en Returns \a true if the node has outgoing data links to blocked/waiting/failed nodes 
//    or if it is a terminal node
bool gmGraph::is_blocking(graph_t::vertex_descriptor v){
  graph_t::out_edge_iterator ei, ei_end;
  boost::tie(ei,ei_end)=out_edges(v,g); // obtaining outcoming links for the node
  if(ei==ei_end)
    return true;
  for(;ei!=ei_end;++ei){
    int i=edgeid[*ei]; 
    if(i>=0 && links[i]->GetType()&gmLINK_DATA){ // this is the data link
//      if(g[target(*ei,g)].flag==gmNS_BLOCK || g[target(*ei,g)].flag==gmNS_WAIT || g[target(*ei,g)].flag==gmNS_FAIL) // the source is not processed
        return true; // this node is blocking some other node
    }
  }
  return false; // this node is not blocking any node
}



int gmGraph::unprocess_recursively_by_id(int nid){
  if(!(nid>=0 && (size_t)nid<nodes.size())) // node index out of array
    return -1;
  graph_t::vertex_descriptor vi=vdnodes[nid];
  //typedef filtered_graph<graph_t, link_type> filterl_t;
  //filterl_t fg1(g, link_type(gmLINK_ALL,this));
  // mark all visited nodes as unprocessed, starting from vi
  breadth_first_search(g,vertex(vi,g), visitor(make_bfs_visitor(set_unproc<on_finish_vertex, graph_t>(this))) );
  //breadth_first_search(fg1,vertex(vi,g), visitor(make_bfs_visitor(set_unproc<on_finish_vertex, filterl_t>())) );
  return 1; 
}




bool gmGraph::process_step(int &graph_error){
  (void) graph_error;
   /*2. Specify all nodes having incoming data links with
        unprocessed sources as `blocked'. Exit with deadlock indication if
        the number of blocked nodes is nonzero and did not change 
        since the previous iteration.*/
  graph_t::vertex_descriptor vt=add_vertex(vertex_prop_t(-1,gmNS_TEMP),g); // adding temporary vertex
  graph_t::vertex_iterator vi_start, vi_end, vi;
  boost::tie(vi_start,vi_end)=vertices(g); // getting node iterator
  for(vi=vi_start;vi!=vi_end;++vi){
    g[*vi].thread_start=g[*vi].threads.size();//g[*vi].threads.clear(); // empty threads
    if(g[*vi].flag==gmNS_BLOCK) // remove blocks, analyze again
      set_node_state(*vi,gmNS_SCHED);
    if(g[*vi].flag==gmNS_EXE || g[*vi].flag==gmNS_FAIL || g[*vi].flag==gmNS_WAIT || is_blocked_directly(*vi)) // connect the failed/blocked node with temporary node
      add_edge(vt,*vi,-1,g);
  }
# if WRITE_DEBUG_GRAPH
  write("graph.dot");
# endif
  
  /*3.  Specify all nodes connected with blocked and failed nodes by recursively following outgoing (logical)
        links as blocked.  */
  // filtered graph having only logical links
  typedef filtered_graph<graph_t, link_type> filterl_t;
  filterl_t fg1(g, link_type(gmLINK_HARD|gmLINK_PROC,this));

# if WRITE_DEBUG_GRAPH
  write(fg1,"graph.dot");
# endif


  // mark all visited nodes as blocked, starting from temporary. The failed, executing, delayed nodes themselves remain unchanged
  breadth_first_search(fg1,vertex(vt,g), visitor(make_bfs_visitor(set_flag<on_finish_vertex, filterl_t>(gmNS_BLOCK, this))) );
# if WRITE_DEBUG_GRAPH
  write("graph.dot");
# endif

  // removing temporary edges
  clear_vertex(vt,g);
  remove_vertex(vt,g);

# if WRITE_DEBUG_GRAPH
  write("graph.dot");
# endif

  /*4.  Find the maximal set of unprocessed non-blocked nodes connected
        with processed nodes by recursively following all outgoing links.
    5.  Find all nodes
        from the set of step 3 that have outgoing data links to blocked
        nodes or coincide with the `finish' root node.*/
  // target nodes that block execution
  vector<graph_t::vertex_descriptor> targets;
  boost::tie(vi_start,vi_end)=vertices(g);
  for(vi=vi_start;vi!=vi_end;++vi){
    if(g[*vi].flag!=gmNS_UNPROC && g[*vi].flag!=gmNS_SCHED) // check that this is unprocessed node
      continue;
    // searching for outgoing data links to blocked nodes or 
    // checking that this node is terminal (no outgoing links)
    if(is_blocking(*vi)){// adding to targets
      targets.push_back(*vi); // here compare with existing targets
      set_node_state(*vi,gmNS_TARG); // marking as target
    }
  }
  // return if no new targets apeeared
 # if WRITE_DEBUG_GRAPH
  write("graph.dot");
# endif


  
  //typedef filtered_graph<graph_t, link_type, node_type<int, equal_to<int> > > filterln_t;
  //filterln_t fg2(g,link_type(gmLINK_HARD|gmLINK_PROC,this), 
  //  node_type<int, equal_to<int> >(&vertex_prop_t::thread,-1,this));


  new_threads=targets.size(); // newly formed threads
  size_t onthreads=threads.size();
  threads.resize(onthreads+new_threads); // adding new threads
  //if(!(threads.size()/*-thread_start*/))
    //return false; // finished
  if(!new_threads){
    // checking that there are unfinished threads
    size_t i=0;
    for(;i<threads.size();i++){
      if(threads[i].state!=1 && threads[i].state<3 ) // not finished and not failed
        break;
    }
    if(i==threads.size())
      return false; // all finished or failed 
    return true; // no new threads, wait till some of the threads finish
  }
  /*
    6.  For each node found in step 4 find the
        maximal subgraph connected with it by incoming logical links */
  typedef filtered_graph<graph_t, link_type> filterln_t;
  filterln_t fg2(g,link_type(gmLINK_HARD|gmLINK_PROC,this)); 

# if WRITE_DEBUG_GRAPH
  write(fg2,"graph.dot");
# endif

  // reverse graph
  typedef reverse_graph<filterln_t, filterln_t&> rgraph_t;
  rgraph_t rfg2(fg2);

# if WRITE_DEBUG_GRAPH
  write_no_edgetypes(rfg2,"graph.dot");
# endif

  for(size_t i=0;i<new_threads;i++){
    breadth_first_search(rfg2,vertex(targets[i],g), visitor(make_bfs_visitor(set_thread<on_finish_vertex, rgraph_t>((int)(onthreads+i),&threads))) );
  }

# if WRITE_DEBUG_GRAPH
  write("graph.dot");
# endif

  // now we have threads assigned to each node
  // here join-split threads
  
  
  
  /*
    7.  For each subgraph of step 5 list
        all input files corresponding to incoming data links entering the
        subgraph
    8.  Form worker tasks from subgraphs of step 5 and
        corresponding files of step 6.*/
 
  
  // form worker jobs
  filterln_t::vertex_iterator wvi_start, wvi_end, wvi;
  boost::tie(wvi_start,wvi_end)=vertices(fg2); // getting node iterator
  for(wvi=wvi_start;wvi!=wvi_end;++wvi){
    graph_t::vertex_descriptor v=*wvi;
    if(!(g[v].threads.size()-g[v].thread_start))// the node does not belong to any thread
      continue;
    //for(size_t j=0;j<g[v].threads.size();j++) // adding node to each thread
    //  threads[g[v].threads[j]].nodes.push_back(g[v].nodeid);
    graph_t::in_edge_iterator ei, ei_end;
    boost::tie(ei,ei_end)=in_edges(v,g); // obtaining incoming links for the node
    for(;ei!=ei_end;++ei){
      int i=edgeid[*ei]; 
      if(i>=0 && links[i]->GetType()&gmLINK_DATA && !(links[i]->GetType()&gmLINK_STATUS)){ // this is the data link, but not status link
        for(size_t j=g[v].thread_start;j<g[v].threads.size();j++) // adding input to each thread
          threads[g[v].threads[j]].idlinks.push_back(*ei);
      }
    }
    graph_t::out_edge_iterator eo, eo_end;
    boost::tie(eo,eo_end)=out_edges(v,g); // obtaining incoming links for the node
    for(;eo!=eo_end;++eo){
      int i=edgeid[*eo]; 
      if(i>=0 && links[i]->GetType()&gmLINK_DATA && !(links[i]->GetType()&gmLINK_STATUS)){ // this is the data link
        for(size_t j=g[v].thread_start;j<g[v].threads.size();j++) // adding output to each thread
          threads[g[v].threads[j]].odlinks.push_back(*eo);
      }
    }
  }
  return true;
}


int gmGraph::select_nodes_regex(vector< pair<int,int> > &snodes, const std::string &expr, bool input, int def_port) const{
  (void) input;
  int nhits=0;
  graph_t::vertex_iterator vi, vi_end;
  boost::tie(vi,vi_end)=vertices(g);
  if(vi==vi_end)
    return nhits;
  std::string myexpr("^");
  myexpr+=expr;
  myexpr+="$";
  //wxString str(myexpr.c_str(), myexpr.length());
  gmdRegEx rexpr(myexpr);
  if(!rexpr.IsValid())
    return LOGERR(-1,fmt("gmGraph.select_nodes_regex: syntax error in node selector expression '%s'",(const char *)expr.c_str()),0);
  
  for(;vi!=vi_end;++vi){
    int i=g[*vi].nodeid;;
    string label=nodes[i]->GetLabel();
    if(rexpr.Matches(label)){
      snodes.push_back(make_pair(i,def_port));
      nhits++;
    }
    /*
    
    if(selector==gmNODE_ALL){
      sel=true;
    }
    else if(selector==gmNODE_NOINPUTS){
      graph_t::in_edge_iterator ei, ei_end;
      boost::tie(ei,ei_end)=in_edges(*vi,g); // obtaining incoming links for the node
      if(ei==ei_end) // node has no inputs
        sel=true; 
      
    }
    else if(selector==gmNODE_NOOUTPUTS && g[*vi].nodeid!=(int)(nodes.size()-1)){
      graph_t::out_edge_iterator eo, eo_end;
      boost::tie(eo,eo_end)=out_edges(*vi,g); // obtaining incoming links for the node
      if(eo==eo_end) // node has no outputs
        sel=true;
    }

    if(sel){
      snodes.push_back(make_pair(g[*vi].nodeid,def_port));
      nhits++;
    }*/
  }
  return nhits;
}


int gmGraph::select_nodes(vector< pair<int,int> > &snodes, gmSelector sel, bool input, int def_port) const {
  gmNodeID selector=sel.id;
  if(selector==gmNODE_AUTO &&sel.str!="") // set by regex
    return select_nodes_regex(snodes,sel.str,input,def_port);
  if(selector==gmNODE_AUTO && sel.nodevec.size()){ // set by vector
    // checking nodes 
    for(size_t i=0;i<sel.nodevec.size(); i++){
      gmNodeID nid=sel.nodevec[i];
      if(nid>=0 && (size_t)nid<nodes.size()) // node OK
        snodes.push_back(make_pair(nid,def_port));
    }
    return (int)snodes.size();
  }
  if(selector>=0){ // just single node
    if((size_t)selector>=nodes.size())
      return -1; // out of range
    snodes.push_back(make_pair(selector,def_port));
    return 1;
  }
  else if(selector==gmNODE_AUTO && sel.nodestates.size()){ // set by node state
    for(size_t i=0;i<vdnodes.size();i++){
      graph_t::vertex_descriptor vi=vdnodes[i];
      for(size_t j=0;j<sel.nodestates.size();j++){
        if(g[vi].flag==sel.nodestates[j]) // matching
          snodes.push_back(make_pair(i,def_port));
      }
    }
    return (int)snodes.size();
  }
  else if(selector==gmNODE_NONE)
    return 0;
  int nhits=0; // graph-based selection
  graph_t::vertex_iterator vi, vi_end;
  boost::tie(vi,vi_end)=vertices(g);
  if(vi==vi_end)
    return nhits;
  
  for(;vi!=vi_end;++vi){
    bool sel=false;
    if(selector==gmNODE_ALL){
      sel=true;
    }
    else if(selector==gmNODE_NOINPUTS){
      graph_t::in_edge_iterator ei, ei_end;
      boost::tie(ei,ei_end)=in_edges(*vi,g); // obtaining incoming links for the node
      if(ei==ei_end) // node has no inputs
        sel=true; 
      
    }
    else if(selector==gmNODE_NOOUTPUTS && g[*vi].nodeid!=(int)(nodes.size()-1)){
      graph_t::out_edge_iterator eo, eo_end;
      boost::tie(eo,eo_end)=out_edges(*vi,g); // obtaining incoming links for the node
      if(eo==eo_end) // node has no outputs
        sel=true;
    }

    if(sel){
      snodes.push_back(make_pair(g[*vi].nodeid,def_port));
      nhits++;
    }
  }
  return nhits;
}



//\en Finds incoming data link to a node/port from another node
//    \returns (nodeid,portid) of the source or (-1,-1) if not found
pair<int,int> gmGraph::get_inlink(int nodeid, int portid) const{
  graph_t::vertex_descriptor vi=vdnodes[nodeid];

  graph_t::in_edge_iterator ei, ei_end;
  boost::tie(ei,ei_end)=in_edges(vi,g); // obtaining incoming links for the node
  for(;ei!=ei_end;++ei){
     int i=edgeid[*ei]; 
     if(i>=0 && links[i]->GetType()&gmLINK_DATA){ // this is the data link
       if(links[i]->GetDestPort()==portid){
         graph_t::vertex_descriptor vd=source(*ei,g);
         return make_pair(g[vd].nodeid,links[i]->GetSrcPort());
       }
     }
  }
  return make_pair(-1,-1);
}


//\en Find ids of all data links connecting source and destination nodes/ports, adds them to \a linkids.
//    \returns the number of found links
int gmGraph::find_links(int src_nodeid, int src_portid, int dst_nodeid, int dst_portid, std::vector<int> &linkids) const{
	if(!(src_nodeid>=0 && (size_t)src_nodeid<nodes.size())) // node index out of array
    return 0;
	if(!(dst_nodeid>=0 && (size_t)dst_nodeid<nodes.size())) // node index out of array
    return 0;
  graph_t::vertex_descriptor svi=vdnodes[src_nodeid];
  graph_t::vertex_descriptor dvi=vdnodes[dst_nodeid];

  int found = 0;
  graph_t::in_edge_iterator ei, ei_end;
  boost::tie(ei,ei_end)=in_edges(dvi,g); // obtaining incoming links for the node
  for(;ei!=ei_end;++ei){
    graph_t::vertex_descriptor vd=source(*ei,g);
    if(vd!=svi)
      continue;

     int i=edgeid[*ei]; 
     if(i>=0){
       if((src_portid<0 || links[i]->GetDestPort()==src_portid) && (dst_portid<0 || links[i]->GetSrcPort()==dst_portid)){
         linkids.push_back(i);
         ++found;
       }
     }
  }
  return found;
}

//\en Returns the edge descriptor of a link with given \a edge_id into \a ed.
//    \retval \a true on success, \a false otherwise.
bool gmGraph::find_edge(int edge_id, graph_t::edge_descriptor &ed) const {
  graph_t::edge_iterator ei, e;
  boost::tie(ei,e)=edges(g);
  for(;ei!=e;++ei){
    if(edgeid[*ei]==edge_id)
      break;
  }
  if(ei==e)
    return false;
  else
    ed=*ei;
  return true;
}



size_t gmGraph::pending_threads_count() const {
  if(!(threads.size()))
    return 0;
  size_t npend=0;
  for(size_t i=0;i<threads.size();i++){
    if(threads[i].state==1 || threads[i].state>=3) // finished or failed
      continue;
    npend++;
  }
  return npend;
}
