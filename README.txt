******************************************************************************
*  OVERVIEW
******************************************************************************

GridMD is the C++ class library intended to help developers to quickly
build  molecular dynamics or Monte-Carlo simulation programs and to run them
in the distributed environment.

GridMD library is free software. The full licence is available in the file
/gridmd/doc/gridmd_license.txt

Home project web page: http://gridmd.sourceforge.net/


******************************************************************************
*  FILE STRUCTURE
******************************************************************************

/-gridmd                  Main directory for the library source filed
  |-doc                   Documentation and licences
    |-gridmd_license.txt  GridMD library license
    |-wx_license.txt      Supplementary wxWindows library licence
    |-jobmngr_en.pdf      Manual for the Job Manager component
	  |-html                Documentation for the entire GridMD library
  |-include               Main GridMD include files
  |-src                   Main GridMD source files

|-ivutils
  |-include               Additional utility include files
  |-src                   Additional utility source files

|-examples                Sample GridMD applications
  |-pbs_job_manager       Examples for the PBS Job Manager
  |-simple_workflow       Examples for the workflow scenarios

|-wxint                   Headers (interface) to use wxWidgets library

******************************************************************************
*  INSTALLATION
******************************************************************************

No install scripts are prepared yet. You can just unpack the archive and add
the source files to your projects. The examples of GridMD usage are given in
the 'examples' subdirectory.
