#ifndef TASK_H
#define TASK_H

#include <wx/thread.h>
#include "threadpool/gmthreaddefs.h"

class gmThread;

class gmTask
{
protected:

    gmTask();
    virtual ~gmTask() {;}
    virtual gmTASK_TYPE Type() const = 0;
    virtual int Run() = 0;

private:

    gmTASK_STATUS Status();
    int Result();
    void Kill();

    bool ResetThread(gmThread *thread = NULL);
    void SetStatus(gmTASK_STATUS status);

    gmThread* mThread;
    wxMutex mStatusMutex;
    wxMutex mResultMutex;
    wxCondition mExited;
    gmTASK_STATUS mStatus;
    int mTaskResult;

    friend class gmThread;
    friend class gmThreadPool;
};



#endif // TASK_H
