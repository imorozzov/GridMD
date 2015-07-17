******************************************************************************
*  GENERAL NOTES
******************************************************************************

This directory contains the examples for the GridMD workflow subsystem (see the
manual in gridmd/doc/html). All examples require wxWidgets library
(http://www.wxwidgets.org/) version 2.8 or later with xml component
and Boost C++ library (http://www.boost.org/).

The subdirectory 'vs8' contains the MS Visual Studio 2005 project (.vcproj) and
solution (.sln) files. All projects have four configurations: Debug, Debug DLL,
Release and Release DLL. The configurations Debug and Release assume using of
the static wxWidgets library whereas the others ones are based on the DLL
version. Before compilation the paths to wxWidgets include and library
directories should be specified in the 'VC++ Directories' section of the Visual
Studio options. Moreover the correct library name should be set in the
Linker/Input section of the Project properties.

The Unix make file is located in the 'unix' subdirectory. All system-specific
information must be specified in the 'Makefile.arch' file. At least one thing
that has to be changed is the value of WXCONFIG variable in Makefile.arch. The
path to the root Boost directory can be specified here as well. It is possible
to specify multiple architectures in Makefile.arch. The particular architecture
can be selected using the '_ARCH = ' make command line option or changing the
DEFAULT_ARCH variable in the 'Makefile' (line 18).

All examples might be compiled just running
  make
To compile a single example run for example
  make simple_workflow.exe

All examples can be executed in different modes as specified in the argument of
gmExperiment.set_execution() function. For remote execution one needs to
compile the application on the remote system and to edit resources.xml file
providing the information about remote connection.

Workflow information is saved to gmexp.dot file. It can be visualized by
Graphviz program (http://www.graphviz.org). The visualized graphs for all
examples are included in graphs/ subdirectory.


******************************************************************************
*  GRIDMD PROGRAM EXECUTION AND RESTART
******************************************************************************

The GridMD application saves its current state at every important stage of its
execution. The state is saved in XML file which is named
<exeperiment_name>.xml. The application may be interrupted at any moment of its
execution, for example while it is waiting for remote job completion. After
that it may be restarted, retrieving all job information and workflow progress
from the saved XML state file. To restart the GridMD application the following
command line options are available:

  -r[config.xml] continue execution from the state saved in 'config.xml'.
                 If 'config.xml' is not specified, the default
                 <exeperiment_name>.xml is used for restarting. Note that it is
                 then overwritten by the new state information when the execution
                 continues.

  -R[config.xml] restart execution from the beginning, using available calculated
                 data link information and started jobs when possible. In this case
                 all local nodes (i.e. the nodes hard-linked to 'finish' nodes)
                 are re-executed. If 'config.xml' is not specified, the default
                 <exeperiment_name>.xml is used for restarting. Note that it is
                 then overwritten by the new state information when the execution
                 continues.


******************************************************************************
*  EXAMPLE simple_workflow
******************************************************************************

This example shows usage of the functions for creation and linking of the parts
(nodes) of a distributed workflow. The nodes perform simple arithmetic
calculations.


******************************************************************************
*  EXAMPLE simple_workflow_fork
******************************************************************************

This example is analogous to "simple_workflow" but uses gmFork template instead
of the explicit node creation/link functions.


******************************************************************************
*  EXAMPLE fork_skeleton
******************************************************************************

This example performs calculation of first N terms of the series:
                   x^2   x^3         x^N
  exp(x) = 1 + x + --- + --- + ... + --- + ...
                    2     6           N!
Each term is calculated as a separate job. The example shows how to use gmFork
template and split-merge skeleton.


******************************************************************************
*  EXAMPLE fork_skeleton_extra
******************************************************************************

This example is analogous to "fork_skeleton" but calculation of the series is
split into two loops. First N terms and subsequent N_extra terms of the series
are calculated using two gmFork template and split-merge skeletons. Example
shows how to merge the results of multiple distributed loops.
