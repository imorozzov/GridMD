project(pbs_job_manager)
if(WIN32)
  add_executable(pbs_plink1 pbs_plink1.cpp)
  target_link_libraries(pbs_plink1 ${LINKED_LIBS})

  add_executable(pbs_plink2 pbs_plink2.cpp)
  target_link_libraries(pbs_plink2 ${LINKED_LIBS})
endif(WIN32)

if(UNIX)
  add_executable(pbs_unixshell1 pbs_unixshell1.cpp)
  target_link_libraries(pbs_unixshell1 ${LINKED_LIBS})
endif(UNIX)