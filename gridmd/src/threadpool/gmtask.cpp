#include "threadpool/gmtask.h"
#include "threadpool/gmthread.h"
#include <wx/string.h>

gmTask::gmTask() :
    mThread(NULL),
    mExited(mResultMutex),
    mStatus(gmTASK_IDLE),
    mTaskResult(INVALID_TASK_RESULT)
{
}

int gmTask::Result()
{
    int ret = INVALID_TASK_RESULT; //TODO: throw custom exception

    wxMutexLocker lock(mResultMutex);
    while(Status() == gmTASK_POOLED)
        mExited.Wait();

    if (Status() == gmTASK_FINISHED)
        ret = mTaskResult;

    return ret;
}

gmTask::gmTASK_STATUS gmTask::Status()
{
    wxMutexLocker lock(mStatusMutex);
    return mStatus;
}

wxString gmTask::strStatus()
{
    wxString result;
    switch(Status()) {
    case gmTASK_IDLE:
        result = wxT("Idle");
        break;
    case gmTASK_POOLED:
        result = wxT("Pooled");
        break;
    case gmTASK_PROCESSED:
        result = wxT("Processed");
        break;
    case gmTASK_FAILED:
        result = wxT("Failed");
        break;
    case gmTASK_FINISHED:
        result = wxT("Finished");
        break;
    }
    return result;
}


void gmTask::Kill()
{
    if (Status() == gmTASK_PROCESSED) {
        mThread->Kill();
        delete mThread;
        ResetThread();
        SetStatus(gmTask::gmTASK_FAILED);
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

void gmTask::SetStatus(gmTask::gmTASK_STATUS status)
{
    wxMutexLocker lock(mStatusMutex);
    mStatus = status;
}


