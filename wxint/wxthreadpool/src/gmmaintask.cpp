#include <wxthreadpool/include/gmmaintask.h>
#include <gridmd.h>


gmMainTask::gmMainTask(int (*gridmd_main)(int, char *[]), int argc, char *argv[]):
    mFuncPtr(gridmd_main),
    mArgc(argc)   
{
   // copying arguments
   mArgv = new const char * [argc];
   mStrings = new std::string[argc]; 
   for(int i=0;i<argc;i++){
     mStrings[i] = argv ? std::string(argv[i]) : "" ;
     mArgv[i] = mStrings[i].c_str(); 
   }
    
}

gmMainTask::~gmMainTask()
{
   delete [] mStrings;
   delete [] mArgv;
}



gmTASK_TYPE gmMainTask::Type() const
{
    return gmTASK_GRIDMD_MAIN;
}

int gmMainTask::Run()
{
    int result = 0;
    try
    {
       result = mFuncPtr(mArgc, (char **)mArgv);// gridmd_main allows to modify argv? TODO: make argv const char **!!!
    }
    catch(const task_finished& ){
      ;
    }
    return result;
}
