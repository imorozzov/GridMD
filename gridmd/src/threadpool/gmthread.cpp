#include "threadpool/gmthreadpool.h"
#include "threadpool/gmthread.h"
#include "threadpool/gmtask.h"
#include <algorithm>


gmThread::gmThread(gmThreadPool *pool):
    wxThread(wxTHREAD_JOINABLE),
    mCurrentTask(NULL),
    mPool(pool)
{;}

gmThread::~gmThread()
{
    if (mPool->mIsRunning)
    {
        {
            wxMutexLocker lock(mPool->mThreadsMutex);
            mPool->mThreads.erase(std::find(mPool->mThreads.begin(), mPool->mThreads.end(), this));
            mPool->mThreadsNotifier.Signal();
        }
    }
    Wait();
}

void gmThread::Start()
{
    Create();
    Run();
}

wxThread::ExitCode gmThread::Entry()
{
    while(true)
    {
        mCurrentTask = NULL;
        {
            wxMutexLocker lock(mPool->mQueueMutex);
            while(mPool->mIsRunning && mPool->mTasksList.empty())
                mPool->mQueueNotifier.Wait();

            if (!mPool->mIsRunning)
                break;

            mCurrentTask = mPool->mTasksList.front();
            mPool->mTasksList.pop_front();
        }
        StartTask();
    }

    return static_cast<ExitCode>(0);
}

void gmThread::StartTask()
{
    if (mCurrentTask) {

        wxMutexLocker lock(mCurrentTask->mResultMutex);
        mCurrentTask->ResetThread(this);
        mCurrentTask->SetStatus(gmTASK_PROCESSED);
        mCurrentTask->mTaskResult = mCurrentTask->Run();
        mCurrentTask->SetStatus(gmTASK_FINISHED);
        mCurrentTask->ResetThread();
        mCurrentTask->mExited.Signal();
        mCurrentTask = NULL;
    }
}
