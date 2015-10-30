# GridMD
## Overview


GridMD is the C++ class library intended to help developers to quickly
build  molecular dynamics or Monte-Carlo simulation programs and to run them
in the distributed environment.

GridMD library is free software. The full licence is available in the file
/gridmd/doc/gridmd_license.txt

Home project web page: http://gridmd.sourceforge.net/

## File structure
```
/-gridmd                  Main directory for the library source files
  |-doc                   Documentation and licences
    |-gridmd_license.txt  GridMD library license
    |-wx_license.txt      Supplementary wxWindows library licence
    |-jobmngr_en.pdf      Manual for the Job Manager component
	  |-html                Documentation for the entire GridMD library
  |-include               Main GridMD include files
  |-src                   Main GridMD source files
|-cmake                   CMake related files
  |-Modules               CMake modules
|-ivutils
  |-include               Additional utility include files
  |-src                   Additional utility source files

|-examples                Sample GridMD applications
  |-pbs_job_manager       Examples for the PBS Job Manager
  |-simple_workflow       Examples for the workflow scenarios
|-wxint                   Headers (interface) to use wxWidgets library
```

## How to build
### Dependencies
* [Boost](http://www.boost.org/) headers
* **Either**
   * [Boost](http://www.boost.org/) libraries ```filesystem```, ```regex```, ```system```, ```timer``` (default) **OR**
    * [WxWidgets](https://www.wxwidgets.org/) ```base``` library*
* [libssh](https://www.libssh.org/) (default) or [libssh2](http://www.libssh2.org/)
* [libxml2](http://www.xmlsoft.org) with [iconv](https://www.gnu.org/software/libiconv/) (optional, enabled by default)
