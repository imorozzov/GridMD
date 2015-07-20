if(NOT USE_WXWIDGETS)
  file(GLOB GRIDMD_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/gmd/*.cpp")
  file(GLOB GRIDMD_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/gmd/*.h")
  set(GRIDMD_HEADERS ${GRIDMD_HEADERS} "src/gmd/gmd_internal.h")
endif(NOT USE_WXWIDGETS)

if(WITH_XML)
  file(GLOB GRIDMD_XML_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/gmd/xml/*.cpp")
  file(GLOB GRIDMD_XML_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/gmd/xml/*.h")
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} ${GRIDMD_XML_SOURCES})
  set(GRIDMD_HEADERS ${GRIDMD_HEADERS} ${GRIDMD_XML_HEADERS})
endif(WITH_XML)

file(GLOB GRIDMD_CORE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB GRIDMD_CORE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.h")

#Jobmngr sources
message("Not including DeisaManager and SHManager since they are not used")
set(GRIDMD_SOURCES ${GRIDMD_CORE_SOURCES} ${GRIDMD_SOURCES} "src/jobmngr/jobmngr.cpp" "src/jobmngr/jobmngrbase.cpp" "src/jobmngr/gmshell.cpp" "src/jobmngr/pbsmngr.cpp" "src/jobmngr/bshmngr.cpp" "src/jobmngr/slurmmngr.cpp")
set(GRIDMD_HEADERS ${GRIDMD_CORE_HEADERS} ${GRIDMD_HEADERS} "include/jobmngr/jobmngr.h" "include/jobmngr/jobmngrbase.h" "include/jobmngr/gmshell.h" "include/jobmngr/pbsmngr.h" "include/jobmngr/bshmngr.h" "include/jobmngr/slurmmngr.h")
if(WIN32)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/winshell.cpp" "src/jobmngr/mingwshell.cpp" "src/jobmngr/winshmngr.cpp" "src/jobmngr/winapimngr.cpp" "src/jobmngr/plinkshell.cpp")
  set(GRIDMD_HEADERS ${GRIDMD_HEADERS} "include/jobmngr/winshell.h" "include/jobmngr/mingwshell.h" "include/jobmngr/winshmngr.h" "include/jobmngr/winapimngr.h" "include/jobmngr/plinkshell.h")
endif(WIN32)

if (UNIX)
  set (GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/unixshell.cpp" "src/jobmngr/unixsshell.cpp")
  set (GRIDMD_HEADERS ${GRIDMD_HEADERS} "include/jobmngr/unixshell.h" "include/jobmngr/unixsshell.h")
endif(UNIX)

if (USE_LIBSSH)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/libsshell.cpp")
  set(GRIDMD_HEADERS ${GRIDMD_HEADERS} "include/jobmngr/libsshell.h")
endif(USE_LIBSSH)

if (USE_LIBSSH2)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/libssh2shell.cpp")
  set(GRIDMD_HEADERS ${GRIDMD_HEADERS} "include/jobmngr/libssh2shell.h")
endif(USE_LIBSSH2)
