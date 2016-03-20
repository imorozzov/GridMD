#ifndef GMMAINTASK_H
#define GMMAINTASK_H

#include "threadpool/gmtask.h"

class gmMainTask : public gmTask
{
public:
    gmMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[]);

    virtual gmTASK_TYPE Type() const;

protected:
    virtual int Run();

    int (*mFuncPtr)(int, char*[]);
    int mArgc;
    char** mArgv;
};

#endif // GMMAINTASK_H
