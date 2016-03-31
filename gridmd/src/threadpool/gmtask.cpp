#include "threadpool/gmtask.h"
#include "threadpool/gmthread.h"

gmTask::gmTask() :
    mThread(NULL),
    mExited(mResultMutex),
    mStatus(gmTASK_POOLED),
    mTaskResult(GMPOOLTASK_INVALID_RESULT)
{
}

int gmTask::Result()
{
    int ret = GMPOOLTASK_INVALID_RESULT; //TODO: throw custom exception

    wxMutexLocker lock(mResultMutex);
    while(Status() == gmTASK_POOLED)
        mExited.Wait();

    if (Status() == gmTASK_FINISHED)
        ret = mTaskResult;

    return ret;
}

gmTASK_STATUS gmTask::Status()
{
    wxMutexLocker lock(mStatusMutex);
    return mStatus;
}




void gmTask::Kill()
{
    if (Status() == gmTASK_PROCESSED) {
        mThread->Kill();
        delete mThread;
        ResetThread();
        SetStatus(gmTASK_CANCELED);
    }
}

bool gmTask::ResetThread(gmThread *thread)
{
    if (!mThread) {
        mThread = thread;
        return true;
    }
    return false;
}

void gmTask::SetStatus(gmTASK_STATUS status)
{
    wxMutexLocker lock(mStatusMutex);
    mStatus = status;
}


