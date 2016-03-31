project(gridmd_examples)
option(BUILD_SIMPLEWORKFLOW "Build Simple Workflow example" ON)
option(BUILD_PBSJOBMANAGER "Build PBS Job manager example" ON)
option(BUILD_THREADPOOL_EXAMPLE "Build Threadpool example" ON)

include_directories(${GRIDMD_INCLUDE_DIR})
include_directories(${IVUTILS_INCLUDE_DIR})

set(LINKED_LIBS gridmd ivutils)
if(BUILD_SIMPLEWORKFLOW)
  add_subdirectory(simple_workflow)
endif(BUILD_SIMPLEWORKFLOW)

if(BUILD_PBSJOBMANAGER)
  add_subdirectory(pbs_job_manager)
endif(BUILD_PBSJOBMANAGER)

if(BUILD_THREADPOOL_EXAMPLE)
    add_subdirectory(threadpool)
endif(BUILD_THREADPOOL_EXAMPLE)
