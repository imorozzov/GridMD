#file(GLOB_RECURSE GRIDMD_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.h")
file(GLOB_RECURSE GRIDMD_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/gmd/*.cpp")
file(GLOB GRIDMD_CORE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
#Jobmngr sources
message("Not including DeisaManager and SHManager since they are not used")
set(GRIDMD_SOURCES ${GRIDMD_CORE_SOURCES} ${GRIDMD_SOURCES} "src/jobmngr/jobmngr.cpp" "src/jobmngr/jobmngrbase.cpp" "src/jobmngr/gmshell.cpp" "src/jobmngr/pbsmngr.cpp" "src/jobmngr/bshmngr.cpp" "src/jobmngr/slurmmngr.cpp")
if(WIN32)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/winshell.cpp" "src/jobmngr/mingwshell.cpp" "src/jobmngr/winshmngr.cpp" "src/jobmngr/winapimngr.cpp" "src/jobmngr/plinkshell.cpp")
endif(WIN32)

if (UNIX)
  set (GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/unixshell.cpp" "src/jobmngr/unixsshell.cpp")
endif(UNIX)

if (USE_LIBSSH)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/libsshell.h")
endif(USE_LIBSSH)

if (USE_LIBSSH2)
  set(GRIDMD_SOURCES ${GRIDMD_SOURCES} "src/jobmngr/libssh2shell.h")
endif(USE_LIBSSH2)
