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
    if (IsAlive()) {
        mPool->mQueueNotifier.Broadcast();
        Wait();
    }
    if (mPool->mIsRunning)
    {
        {
            wxMutexLocker lock(mPool->mThreadsMutex);
            mPool->mThreads.erase(std::find(mPool->mThreads.begin(), mPool->mThreads.end(), this));
            mPool->mThreadsNotifier.Signal();
        }
        {
            wxMutexLocker lock(mPool->mQueueMutex);
            std::list<gmTask*>::iterator itCurTask = std::find(mPool->mTasks.begin(), mPool->mTasks.end(), mCurrentTask);
            if (itCurTask != mPool->mTasks.end())
                mPool->mTasks.erase(itCurTask);
        }
    }
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
            while(mPool->mIsRunning && mPool->mTasks.empty())
                mPool->mQueueNotifier.Wait();

            if (!mPool->mIsRunning)
                break;

            mCurrentTask = mPool->mTasks.front();
            mPool->mTasks.pop_front();
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
        mCurrentTask->SetStatus(gmTask::gmTASK_PROCESSED);
        mCurrentTask->mTaskResult = mCurrentTask->Run();
        mCurrentTask->SetStatus(gmTask::gmTASK_FINISHED);
        mCurrentTask->ResetThread();
        mCurrentTask->mExited.Signal();
        mCurrentTask = NULL;
    }
}

