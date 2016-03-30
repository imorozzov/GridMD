#include "threadpool/gmmaintask.h"
#include "gridmd.h"


gmMainTask::gmMainTask(int (*gridmd_main)(int, char *[]), int argc, char *argv[]):
    mFuncPtr(gridmd_main),
    mArgc(argc),
    mArgv(argv)
{;}

gmTASK_TYPE gmMainTask::Type() const
{
    return gmTASK_GRIDMD_MAIN;
}

int gmMainTask::Run()
{
    try
    {
       mFuncPtr(mArgc, mArgv);
    }
    catch(const task_finished& ){;}
    return 0;
}
