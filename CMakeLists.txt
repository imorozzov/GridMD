cmake_minimum_required(VERSION 3.0.0)
project(gridmd)

#TODO: implement linking to libssh
option(USE_LIBSSH "Use libssh" OFF)
option(USE_LIBSSH2 "Use libssh2" OFF)
option(USE_WXWIDGETS "Use WxWidgets library" OFF)
option(BUILD_SHARED "Build GridMD as shared library" ON)
option(BUILD_EXAMPLES "Build all examples" ON)

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

include_directories(${BOOST_INCLUDE_DIRS})
#TODO make usage of libxml2 to be optional
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
set(LINKED_LIBS ${LINKED_LIBS} ${LIBXML2_LIBRARIES})

#build ivutils library
add_subdirectory(ivutils)
set(LINKED_LIBS ${LINKED_LIBS} ivutils)
#build gridmd library
add_subdirectory(gridmd)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()