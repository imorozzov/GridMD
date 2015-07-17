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
 *   $Revision: 1.1 $
 *   $Date: 2012/08/29 16:50:28 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmsweep.h,v 1.1 2012/08/29 16:50:28 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmsweep.h,v $
$Revision: 1.1 $
$Author: valuev $
$Date: 2012/08/29 16:50:28 $
*/
/*s****************************************************************************
 * $Log: gmsweep.h,v $
 * Revision 1.1  2012/08/29 16:50:28  valuev
 * restructured gridmd for VS10
 *

*******************************************************************************/


# ifndef GMFORK_H
# define GMFORK_H

/** @file gmfork.h
    \en @brief Classes implementing GridMD Fork skeleton

**/

# include "gridmd.h"

///\en Counters of created objects for all fork templates.
///    Each array index is responsible for corresponding recursion level of gmManager;
extern int fork_objcount[MAX_RECURSION]; 

///\en Skeleton class for concurrent branching.
///    Fork skeleton is used to split execution into several
///    branches that may be run concurrently. The branches may have
///    the same or different code. The skeleton has 4 building elements:\n
///    \a begin_node -- the node where the branches deviate from each other, 
///                     only one begin node is possible for each fork;\n
///    \a split_node -- beginning of the branch, each branch (tooth) of the fork 
///                     has one split node;\n
///    \a merge_node -- end of the branch, each branch (tooth) of the fork 
///                     has one merge node. It may coincide with the split node;\n
///    \a end_node   -- a node where the branches join, there may be 0, 1 or  several 
///                     end nodes of the fork.\n
///    Template parameters are data types corresponding to the links between
///    begin node and split nodes (\a split_tt) and merge nodes and end node (\a merge_tt).
///    Also, if there are no added nodes between the split and merge nodes of the branch,
///    and the split and merge nodes are different, a datalink of type \a branch_tt 
///    is automatically created. 
///    If any of the data types is \a void, then corresponding hard link is used.
///    Node construction for the fork is performed by calling markup functions, for example
///    \ref gmFork::begin() creates the begin node. All markup functions appear in
///    two versions: the one creating a new node, and the other using the current
///    node as building element of the fork. The second version markup functions
///    end with \a _here postfix, for example \a fork.begin_here() will assign the current node
///    as the begin node of \a fork.
///    Usually the fork is constructed in the following sequence: \n
///    gmFork::begin() -- adds begin node;\n
///    gmFork::split() -- adds a starting node of the new branch, 
///               links it with the begin node using \a split_tt datalink;\n
///    gmManager::mark_node() -- adds a node to this branch;\n
///    gmManager::mark_node() -- adds a node to this branch;\n
///    ...\n
///    gmFork::merge() -- finishes this branch by adding a node which will be linked by \a merge_tt
///               datalink with end node(s);\n
///    gmFork::split() -- adds another branch; \n
///    ...\n
///    gmFork::end() -- adds an end node, links it with all merge nodes by \a merge_t datalink. 
///    Deviations from this sequence are also possible (fork with no end nodes, fork with multiple end nodes,
///    parallel forks sharing some building elements, nested forks, etc.)
///    The link data for all created nodes may be acessed in the usual way by \ref gridmd::node_input<>()
///    and \ref gridmd::node_output<>() functions. There is also a more type safe way of
///    acessing this data by using the branch data access gmFork functions
///    (\ref gmFork::vsplit_out(), \ref gmFork::vsplit_in(), ... etc.) 
template <class split_tt=void, class branch_tt=split_tt, class  merge_tt=branch_tt>
class gmFork{
public:
  typedef typename gmlink_traits<split_tt>::value_type split_t;
  typedef typename gmlink_traits<merge_tt>::value_type merge_t;
  typedef typename gmlink_traits<branch_tt>::value_type branch_t;
protected:
  gmManager *sys;
  string name;
  struct branchid_t{
    int beg; // start node of the branch (set by split function) 
    int end;  // end node of the branch (set by merge function)
    int merged; // number of bottom nodes this node merged to
    branchid_t(int beg_,int end_=-1):beg(beg_),end(end_),merged(0){}
  };
  vector<branchid_t> branches;
  int top_node; 
  vector<int> bottom_node; // a set of bottom nodes
  split_t tmp_split;
  merge_t tmp_merge;
  branch_t tmp_branch;
  int my_reclevel; // recursion level of creation
public:
  ///\en Constructor of the fork.
  ///    \param name_ the name of the fork. By default constructs the name
  ///    'forkN', where N is the count of currently existing forks. 
  ///    The fork name is also used for naming the parant elements (like
  ///    fork0.begin for begin node, etc.) if their names are not explicitly specified;
  ///    \param manager is a pointer to managing object, defaults to the common \ref gmExperiment.
  gmFork(const string &name_="",gmManager *manager=&gmExperiment):sys(manager){
    // checking initialization of objcount
    if(fork_objcount[0]<0)
      for(int i=0;i<MAX_RECURSION;i++)
        fork_objcount[i]=0; // initializing
    my_reclevel=sys->get_recursion();
    fork_objcount[my_reclevel]++; 
    set_name(name_);
    top_node=-1;
  }

  ~gmFork(){
    fork_objcount[my_reclevel]--; 
  }

  ///\en Returns current number of branches in the fork (construction and worker mode).
  size_t branchcount() const {
    return branches.size();
  }

  /// \en Sets/changes the name of the fork. By default or when the name is empty
  ///      constructs the name
  ///     'forkN', where N is the count of currently existing forks. 
  void set_name(const string &name_=""){
    name=name_;
    if(name=="") // automatic name assignement
      name=fmt("fork%d",fork_objcount[my_reclevel]-1);
  }

  
  
  /// \en Adds the top join node of the fork (begin node).
  ///     The parameters and the return value 
  ///     are the same as for \ref gmManager::mark_node(),
  ///     except for the name rule: if the \a name_ is empty, the node is
  ///     named \<fork_name\>.begin. As usual, the return value 
  ///     (nonzero for processing request in the worker mode, 0 otherwise) may be analyzed
  ///     to trigger begin node exection. If template parameter \a split_tt is not \a void,
  ///     the created node will have an output dataport for each branch 
  ///     (indexed form 0, the number of branches 
  ///     is returned by \ref gmFork::branchcount()). The dataports are accesible
  ///     in the usual way by gmManager::node_output<>(portid) function. 
  ///     Alternatively, the link data may be output by using \ref gmFork::vbegin_out() function,
  ///     allowing more accurate worker mode type control.
  ///     All existing fork branches and their data is deleted by calling this function.
  ///     The fork may be reinitialized for reuse by this function.
  template<class link_type>
  int begin(const string &name_, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1){
    string splname=name_;
    if(splname=="") // automatic name assignement
      splname=fmt("%sbegin",name=="" ? "" : (name+".").c_str()); // name from the fork name and branch id 
    int res=sys->mark_node(splname,input,t,srcport);
    begin_here();
    return res;
  }

  /// \en Hard link version of gmFork::begin<>().
  ///     Constructs incoming hard links from the nodes specified by \a input.
  int begin(const string &name_="", gmSelector input=gmNODE_PREV){
    return begin<gmHardLink>(name_,input);
  }

  /// \en Begins the fork with the current node whatever it is. 
  ///     The output dataports (see gmFork::begin<>()) are added to the current node. 
  void begin_here(){
    top_node=sys->get_curnode();
    bottom_node.clear(); 
    branches.clear();
  }
  
  ///\en  Starts a new branch by creating a split node and linking it to the fork begin node
  ///     with \a split_tt data link (or hard link when \a split_tt is \a void).
  ///     If \a split_tt is not \a void, one input 
  ///     data port of \a split_tt data is accessible by
  ///     gmManager::node_input<>(0) function and contains the split data placed in the
  ///     Nth output of the begin node, where N is the curren branch Id starting from zero.
  ///     Alternatively, the link data may be obtained by using \ref gmFork::vsplit_in() function,
  ///     allowing more accurate worker mode type control.
  ///     If the name is empty the node is named '\<fork_name\>.split(N)', where N is the branch number.
  ///     The return value 
  ///     (nonzero for processing request in the worker mode, 0 otherwise) may be analyzed
  ///     to trigger begin node exection.
  ///     If split() is called before gmFork::begin(), gmFork::begin_here() 
  ///     is called for the current node.
  int split(const string &name_=""){
    if(top_node<0)
      begin_here();
    string splname=name_;
    int bcount=(int)branches.size();
    if(splname=="") // automatic name assignement
      splname=fmt("%ssplit(%d)",name=="" ? "" : (name+".").c_str(),bcount); // name from the fork name and branch id 
    int res=sys->mark_node(splname,gmNODE_NONE);
    split_here();
    return res;   
  }

  ///\en  Starts a branch without creating a new node. Uses
  ///     the current node as a split node and links it to the fork begin node
  ///     with \a split_tt data link (or hard link when \a split_tt is \a void).
  ///     Same data port as for gmFork::split() is available.
  void split_here(){
    if(top_node<0)
      begin_here();
    // constructing links
    int bcount=(int)branches.size();
    int splnode=sys->get_curnode();
    branches.push_back(branchid_t(splnode,-1)); // add new branch
    if(top_node!=splnode)
      sys->link<split_tt>(top_node,splnode,bcount); // create a link to the current node, port bcount
  }

  /// \en Finishes current branch by creating a merge node.
  ///     First 4 parameters and the return value 
  ///     are the same as for \ref gmManager::mark_node(),
  ///     except for the name rule: if the \a name_ is empty, the node is
  ///     named '\<fork_name\>.merge(N)',where N is the branch number.
  ///     As usual, the return value 
  ///     (nonzero for processing request in the worker mode, 0 otherwise) may be analyzed
  ///     to trigger merge node exection.
  ///     If input is gmNODE_AUTO and there is a split node for the current branch,
  ///     then a datalink of type branch_tt is created 
  ///     between the split node and the merge node of the branch, ignoring the supplied link type.
  ///     If end nodes already exist for this fork, the merge node is 
  ///     immediately linked to all of them
  ///     with \a merge_tt data link (or hard link when \a split_tt is \a void).
  ///     If template parameter \a merge_tt is not \a void,
  ///     the created node will have output dataports.
  ///     The number of these dataports is controlled by \a same_ports parameter.
  ///     If it is \a true, then all end nodes will be linked from the 
  ///     same (0th) dataport of the merge node.
  ///     If it is \a false, then each of the end nodes will be linked from a
  ///     separate dataport of the merge node. In this case the merge node will have
  ///     an output dataport for each end node (indexed form 0).
  ///     The dataports are accesible
  ///     in the usual way by gmManager::node_output<>(portid) function. 
  ///     If  this function is not called for the branch, then the merge node
  ///     coincides with the split node.
  template<class link_type>
  int merge(const string &name_, gmSelector input=gmNODE_PREV, const link_type &t=gmHardLink(), int srcport=-1,bool same_ports=false){
    string splname=name_;
    int bcount=(int)branches.size();
    
    if(splname==""){ // automatic name assignement
      int bnum=bcount-1;
      if(!bcount || branches[bcount-1].end>=0) // the branch bcount already begun, but there were no split node
        bnum=bcount;
      splname=fmt("%smerge(%d)",name=="" ? "" : (name+".").c_str(),bnum); // name from the fork name and branch id 
    }
    int res;
    if(input.str=="" && input.id==gmNODE_AUTO && bcount && branches[bcount-1].beg>=0) // auto and split node exists
      res=sys->mark_node(splname,branches[bcount-1].beg,gmDataLink<branch_tt>(),-1);
    else
      res=sys->mark_node(splname,input,t,srcport);
    merge_here(same_ports);
    return res;   
  }
  /// \en Automatic/Hard link version of gmFork::merge<>().
  ///     If input is gmNODE_AUTO, then a datalink of type branch_tt is created 
  ///     between the split node and the merge node of the branch. Otherwise
  ///     hard link(s) to the specified input node(s) are created.
  int merge(const string &name="", gmSelector input=gmNODE_AUTO,bool same_ports=false){
    return merge<gmHardLink>(name,input,gmHardLink(),-1,same_ports);
  }


  
  /// \en Finishes current branch without creating a new node. Uses
  ///     the current node as a merge node, creating output links.
  ///     No input links are created.
  ///     Same data ports as for gmFork::merge() are available. 
  void merge_here(bool same_ports=false){
    // constructing links
    int mergenode=sys->get_curnode();
    int bcount=(int)branches.size();
    if(!bcount || branches[bcount-1].end>=0){//  there are no unfinished branch from previous split
      split_here(); // create it
      bcount++;
    }
    branches[bcount-1].end=mergenode; // finish this branch
    if(branches[bcount-1].merged==0){ // the last branch is not merged yet
      for(size_t i=0;i<bottom_node.size();i++){
        if(bottom_node[i]!=mergenode){
          sys->link<merge_tt>(mergenode,bottom_node[i],same_ports ? 0 : -1); // create a link to the current node, adding new port
          branches[bcount-1].merged++;
        }
      }
    }
  }


  /// \en Adds a final node where all branches join.
  ///     If the \a name_ is empty, the node is
  ///     named '\<fork_name\>.end'. The return value 
  ///     (nonzero for processing request in the worker mode, 0 otherwise) may be analyzed
  ///     to trigger merge node exection.
  ///     All existing branches (merge nodes or split nodes, if merge nodes are undefined)  
  ///     are linked to the end node
  ///     with \a merge_tt data link (or hard link when \a split_tt is \a void).
  ///     If template parameter \a merge_tt is not \a void,
  ///     the merge node of each branch will have output dataports.
  ///     The number of these dataports is controlled by \a same_ports parameter.
  ///     If it is \a true, then the end node will be linked from the 
  ///     same (0th) dataport of the merge node.
  ///     If it is \a false, then the end node will be linked from a
  ///     separate dataport of each of the merge nodes. In this case each merge node will have
  ///     an output dataport to this end node (its id corresponding to the order of end nodes creation).
  ///     The dataports are accesible
  ///     in the usual way by \ref gmManager::node_output<>(portid) function. 
  ///     The end node has input data ports for each of the branches
  ///     with ids matching the branch number starting form 0.
  ///     If this function is never called for the fork, the fork remains open from the bottom and
  ///     its ends may be explicitly linked with other nodes.
  int end(const string &name_="", bool same_ports=false){
    string splname=name_;
    if(splname=="") // automatic name assignement
      splname=fmt("%send",name=="" ? "" : (name+".").c_str()); // name from the fork name
    int res=sys->mark_node(splname,gmNODE_NONE);
    end_here(same_ports);
    return res;   
  }

  ///\en Uses current node as an end node of the fork. The parameter \a same_ports
  ///    controls the number of dataports created at merge nodes, see
  ///    \ref gmFork::end().
  void end_here(bool same_ports=false){
    // constructing links
    int this_node=sys->get_curnode();
    bottom_node.push_back(this_node); //adding this node to bottom set
    size_t bcount=branches.size();
    for(size_t i=0;i<bcount;i++){ // creating a link for each merge node from a branch to this bottom node
      if(branches[i].end<0) // no merge node, using split node
        branches[i].end=branches[i].beg;
      if(this_node!=branches[i].end){
        sys->link(gmDataLink<merge_tt>(),branches[i].end,this_node,same_ports ? 0 : -1); // create a link to the current node adding new port
        branches[i].merged++;
      }
    }
  }

  ///\en Returns a reference to split_tt to put the value of split data for branch \a branchid. May be called
  ///    at begin node only in worker mode. If called at wrong node or split_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference for which no data transfer is possible is returned.
  split_t &vbegin_out(int branchid) const{
    if(gmlink_traits<split_t>::has_value){
      size_t bcount=branches.size();
      // checking that we are at begin node
      if(top_node==sys->get_curnode())
        return sys->node_output<split_tt>(branchid);
      else
        LOGMSG(vblWARN,fmt("gmFork<>.vsplit_out: invalid call outside end node of '%s', using uninitialized data!",name.c_str()),0);
    }
    else
      LOGMSG(vblWARN,fmt("gmFork<>.vsplit_out: invalid call for hard link in '%s', using uninitialized data!",name.c_str()),0);
    return tmp_split;
    
  }


  ///\en Returns a reference to split_tt to get the value of split data for current branch. May be called
  ///    at split node only in worker mode. If called at wrong node or split_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference with garbage data is returned.
  const split_t &vsplit_in() const{
    size_t bcount=branches.size();
    // checking that we are at split node
    if(bcount && branches[bcount-1].beg==sys->get_curnode())
      return sys->node_input<split_tt>();
    else{
      LOGMSG(vblWARN,fmt("gmFork<>.vsplit(in): invalid call outside split node of '%s', using uninitialized data!",name.c_str()),0);
      return tmp_split;
    }
  }


  ///\en Returns a reference to branch_tt to put the value of split data for current branch. May be called
  ///    at split node only in worker mode. If called at wrong node or branch_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference for which no data transfer is possible is returned.
  branch_t &vsplit_out(){
    size_t bcount=branches.size();
    // checking that we are at split node
    if(bcount && branches[bcount-1].beg==sys->get_curnode())
      return sys->node_output<branch_tt>();
    LOGMSG(vblWARN,fmt("gmFork<>.vbranch_out: invalid call outside split node of '%s', using uninitialized data!",name.c_str()),0);
    return tmp_branch;
  }

  ///\en Returns a reference to branch_tt to get the value of branch data for current branch. May be called
  ///    at merge node (or at split node when there is no merge node) only in worker mode. If called at wrong node 
  ///    or branch_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference with garbage data is returned.
  const branch_t &vmerge_in() const {
    size_t bcount=branches.size();
    int this_node=sys->get_curnode();
    // checking that we are at merge node or at split node if there is no merge node
    if(bcount && (branches[bcount-1].end==this_node || (branches[bcount-1].beg==this_node && branches[bcount-1].end<0) ) )
      return sys->node_input<branch_tt>();
    LOGMSG(vblWARN,fmt("gmFork<>.vbranch_in: invalid call outside merge node of '%s', using uninitialized data!",name.c_str()),0);
    return tmp_branch;
  }
  
  ///\en Returns a reference to merge_tt to put the value of merge data for current branch. May be called
  ///    at merge node (or at split node when there is no merge node) only in worker mode.
  ///    If called at wrong node or merge_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference for which no data transfer is possible is returned.
  ///    \param portid is a port number to put the data, usually corresponding to the end node this 
  ///                  data is supplied to. See description of 
  ///                  \a same_ports parameter of the \ref gmFork::merge function for detais. 
  merge_t &vmerge_out(int portid=0){
    size_t bcount=branches.size();
    int this_node=sys->get_curnode();
    // checking that we are at merge node or at split node if there is no merge node
    if(bcount && (branches[bcount-1].end==this_node || (branches[bcount-1].beg==this_node && branches[bcount-1].end<0) ) )
      return sys->node_output<merge_tt>(portid);
    else{
      LOGMSG(vblWARN,fmt("gmFork<>.vmerge(out): invalid call outside merge node of '%s', using uninitialized data!",name.c_str()),0);
      return tmp_merge;
    }
  }
  
  ///\en Returns a reference to merge_tt to get the value of merge data for branch \a branchid. May be called
  ///    at end nodes only in worker mode. If called at wrong node or merge_tt is void, a warning is given
  ///    at worker mode runtime and a temporary reference with garbage data is returned.
  const merge_t &vend_in(int branchid) const {
    int this_node=sys->get_curnode();
    for(size_t i=0;i<bottom_node.size();i++){
       if(bottom_node[i]==this_node)
         return sys->node_input<merge_tt>(branchid);
    }
    LOGMSG(vblWARN,fmt("gmFork<>.vmerge(in): invalid call outside end node of '%s', using uninitialized data!",name.c_str()),0);
    return tmp_merge;
  }

  ///\en Returns a node ID of the split node in the branch \a branchid.
  int get_splitnode(int branchid) const {
    if(branchid<0 || branchid>=(int)branches.size()){
      LOGMSG(vblWARN,fmt("gmFork<>.get_splitnode: branch id %d for '%s' is outside the valid range [0,%d]!",branchid,name.c_str(),branches.size()-1),0);
      return gmNODE_NONE;
    }
    return branches[branchid].beg;
  }

  
};


# endif
