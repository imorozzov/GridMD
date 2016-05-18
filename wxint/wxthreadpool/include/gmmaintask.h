#ifndef GMMAINTASK_H
#define GMMAINTASK_H

#include <wxthreadpool/include/gmtask.h>

class gmMainTask : public gmTask
{
public:
    gmMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[]);

    ~gmMainTask();

    virtual gmTASK_TYPE Type() const;

protected:
    virtual int Run();

    int (*mFuncPtr)(int, char*[]);
    int mArgc;
    const char** mArgv;
    std::string *mStrings; ///\en< array for storing copied arguments
};

#endif // GMMAINTASK_H
