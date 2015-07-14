******************************************************************************
*  GENERAL NOTES
******************************************************************************

This directory contains the examples for the PBS Job Manager (see the Job
Manager manual in gridmd/doc/JobManager.pdf). All examples require wxWidgets
library (http://www.wxwidgets.org/) version 2.8 or later.

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
that has to be changed is the value of WXCONFIG variable in Makefile.arch.
It is possible to specify multiple architectures in Makefile.arch. The particular
architecture can be selected using the '_ARCH = ' make command line option or
changing the DEFAULT_ARCH variable in the 'Makefile' (line 18).


******************************************************************************
*  EXAMPLE pbs_plink1
******************************************************************************

Handling a single job using the PBS Job Manager over plink ssh protocol. The
example should be compiled and run under MS Windows having PuTTY tools plink
and pscp pre-installed. The example shows how to create, submit, monitor,
detach and attach a job to the remotely installed PBS. It is also shown how the
user can manage files using gmPlinkShell protocol class.


******************************************************************************
*  EXAMPLE pbs_plink2
******************************************************************************

The same as pbs_plink1 but shows handling multiple jobs. Usage of the
gmPBSManager::RestoreAll function is illustrated.


******************************************************************************
*  EXAMPLE pbs_unixshell1.cpp
******************************************************************************

The same as pbs_plink1 but using the Unix shell to access the PBS server.
