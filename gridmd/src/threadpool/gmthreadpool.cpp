#include "threadpool/gmthreadpool.h"
#include "threadpool/gmthread.h"
#include "threadpool/gmtask.h"

gmThreadPool::gmThreadPool(size_t threads):
    wxThread(wxTHREAD_JOINABLE),
    mThreadsCount(threads),
    mIsRunning(true),
    mQueueNotifier(mQueueMutex),
    mThreadsNotifier(mThreadsMutex)
{
    for(size_t i = 0; i < mThreadsCount; ++i) {
        gmThread* thread = new gmThread(this);
        mThreads.push_back(thread);
        thread->Start();
    }
    Create();
    Run();
}

gmThreadPool::~gmThreadPool()
{
    mIsRunning = false;
    mThreadsNotifier.Signal();
    Wait();
    std::list<gmThread*>::iterator iter = mThreads.begin();
    for(; iter != mThreads.end(); ++iter) {
        mQueueNotifier.Broadcast();
        delete (*iter);
    }
}


void gmThreadPool::SubmitTask(gmTask *task)
{
    {
        wxMutexLocker lock(mQueueMutex);
        mTasks.push_back(task);
        task->SetStatus(gmTask::gmTASK_POOLED);
    }
    mQueueNotifier.Signal();
}


wxThread::ExitCode gmThreadPool::Entry()
{
    while(1)
    {
        wxMutexLocker lock(mThreadsMutex);
        while(mIsRunning && mThreads.size() == mThreadsCount)
            mThreadsNotifier.Wait();

        if (!mIsRunning)
            break;

        for (size_t i = 0; i < mThreadsCount - mThreads.size(); ++i) {
            gmThread* thread = new gmThread(this);
            mThreads.push_back(thread);
            thread->Start();
        }
    }
    return static_cast<ExitCode>(0);
}

