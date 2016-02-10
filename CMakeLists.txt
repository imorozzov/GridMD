cmake_minimum_required(VERSION 3.0.0)
project(gridmd)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")


option(USE_LIBSSH "Use libssh" ON)
option(USE_LIBSSH2 "Use libssh2" OFF)
option(WITH_XML "Enable XML support via libxml2" ON)
option(USE_WXWIDGETS "Use WxWidgets library" OFF)
option(BUILD_SHARED "Build GridMD as shared library" ON)
option(BUILD_EXAMPLES "Build all examples" ON)


if(WIN32)
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
	add_definitions("-D_SCL_SECURE_NO_WARNINGS")
endif(WIN32)

if (BUILD_SHARED)
  set(BUILDCONF SHARED)
else(BUILD_SHARED)
  set(BUILDCONF STATIC)
endif(BUILD_SHARED)
if (USE_WXWIDGETS) #find boost headers
  find_package(Boost REQUIRED)
  find_package(wxWidgets REQUIRED base)
  include_directories(${wxWidgets_INCLUDE_DIRS})
  include(${wxWidgets_USE_FILE})
  set(LINKED_LIBS ${LINKED_LIBS} ${wxWidgets_LIBRARIES})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/wxint)
else(USE_WXWIDGETS) #find boost compiled
  find_package(Boost REQUIRED filesystem timer system regex)
  set(LINKED_LIBS ${LINKED_LIBS} ${Boost_LIBRARIES})
endif (USE_WXWIDGETS)



if (USE_LIBSSH)
  find_package(LibSSH)
  if(LIBSSH_FOUND)
    add_definitions("-DUSING_LIBSSH")
    set(LINKED_LIBS ${LINKED_LIBS} ${LIBSSH_LIBRARIES})
    include_directories(${LIBSSH_INCLUDE_DIRS})
    message("LibSSH include dir: ${LIBSSH_INCLUDE_DIRS}")
  else(LIBSSH_LIBRARIES)
    set(USE_LIBSSH OFF)
    message("LibSSH not found!")
  endif(LIBSSH_FOUND)
endif(USE_LIBSSH)


if (USE_LIBSSH2)
  find_package(LibSSH2)
  if(LIBSSH2_FOUND)
    add_definitions("-DUSING_LIBSSH2")
    set(LINKED_LIBS ${LINKED_LIBS} ${LIBSSH2_LIBRARY})
    include_directories(LIBSSH2_INCLUDE_DIR)
  else(LIBSSH2_FOUND)
    set(USE_LIBSSH2 OFF)
  endif(LIBSSH2_FOUND)
endif(USE_LIBSSH2)

if (WITH_XML)
  #LibXML also needs iconv
  find_package(Iconv)
  if (ICONV_FOUND)
    include_directories(${ICONV_INCLUDE_DIR})
    set(LINKED_LIBS ${LINKED_LIBS} ${ICONV_LIBRARIES})
  else(ICONV_FOUND)
    set(WITH_XML OFF)
  endif(ICONV_FOUND)
  find_package(LibXml2 REQUIRED)
  if(LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    set(LINKED_LIBS ${LINKED_LIBS} ${LIBXML2_LIBRARIES})
  else(LIBXML2_FOUND)
    set(WITH_XML OFF)
  endif(LIBXML2_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gridmd/include/gmd/xml)
else(WITH_XML)
  add_definitions("-DNO_XML")
endif(WITH_XML)

#if(NOT WITH_XML)
#  add_definitions("-DNO_XML")
#endif(NOT WITH_XML)

include_directories(${Boost_INCLUDE_DIR})

#find_package(LibXml2 REQUIRED)

if(WIN32)
set(LINKED_LIBS ${LINKED_LIBS} userenv)
endif(WIN32)


#build ivutils library
add_subdirectory(ivutils)
set(LINKED_LIBS ${LINKED_LIBS} ivutils)

#build gridmd library
add_subdirectory(gridmd)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "includedir='${dir}'")
endforeach()
