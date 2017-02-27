project(pbs_job_manager)
if(WIN32)
  add_executable(pbs_plink1 pbs_plink1.cpp)
  target_link_libraries(pbs_plink1 gridmd)

  add_executable(pbs_plink2 pbs_plink2.cpp)
  target_link_libraries(pbs_plink2 gridmd)
  
  add_executable(pbs_shellSsh pbs_shellSsh.cpp)
  target_link_libraries(pbs_shellSsh gridmd)
endif(WIN32)

if(UNIX)
  add_executable(pbs_unixshell1 pbs_unixshell1.cpp)
  target_link_libraries(pbs_unixshell1 gridmd)
endif(UNIX)