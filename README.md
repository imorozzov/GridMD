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

### CMake Options

Option | Type| Meaning | Default
-----|------|----|---|
```USE_LIBSSH```|bool|Use libssh|:heavy_check_mark:|
```USE_LIBSSH2```|bool|Use libssh2|:heavy_multiplication_x:|
```WITH_XML```|bool|Enable XML support via libxml2|:heavy_check_mark:|
```USE_WXWIDGETS```|bool|Enable WxWidgets implementation. If not set, fall back to compiled Boost|:heavy_multiplication_x:|
```BUILD_SHARED```|bool|Build as shared library|:heavy_check_mark:|
```BUILD_EXAMPLES```|bool|Build examples from ```examples``` directory|:heavy_check_mark:|
```BOOST_ROOT```|directory| Boost root|Empty|
```BOOST_LIBRARYDIR```|directory|Directory with built Boost libraries (if wxWidgets is disabled)|Empty|
```wxWidgets_LIB_DIR```|directory|Directory with built WxWidgets base library|Empty|
```wxWidgets_ROOT_DIR```|directory|WxWidgets root|Empty|
```SSH_LIBRARY```|file|Path to libssh library|Empty|
```LIBSSH_INCLUDE_DIR```|directory|Path to libssh includes|Empty|
```LIBSSH2_LIBRARY```|file|Path to libssh2 library|Empty|
```LIBSSH2_INCLUDE_DIR```|directory|Path to libssh2 includes|Empty|
```LIBXML2_INCLUDE_DIR```|directory|Path to libxml ibcludes|Empty|
```LIBXML2_LIBRARIES```|file|Path to libxml2 library (file)|Empty|
```ICONV_INCLUDE_DIR```|directory|Path to iconv includes|Empty|
```ICONV_LIBRARIES```|file|Path to iconv library|Empty|

If you are on UNIX, CMake should find the libraries in system paths. Otherwise you might set them appropriately.

#### Example Windows configuration (Microsoft Visual Studio 2015)
![cmake-gui](https://www.dropbox.com/s/p7em1f6p3tuwb7r/cmake-gui.png?raw=1)
