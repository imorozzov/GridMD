#include <wxthreadpool/include/gmthreadpool.h>
#include <wxthreadpool/include/gmthread.h>
#include <wxthreadpool/include/gmmaintask.h>
#include <wxthreadpool/include/gmscripttask.h>
#include <wxthreadpool/include/gmredirector.h>

#include <wx/hashmap.h>
#include <algorithm>
#include <utility>


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

    std::list<gmThread*>::iterator threadsIter = mThreads.begin();
    for(; threadsIter != mThreads.end(); ++threadsIter) {
        (*threadsIter)->Kill();
        delete (*threadsIter);
    }

    std::map<gmTaskID, gmTask*>::iterator tasksIter = mTasksMap.begin();
    for(; tasksIter != mTasksMap.end(); ++tasksIter) {
        delete tasksIter->second;
    }
}

gmTaskID gmThreadPool::CreateGMMainTask(int (*gridmd_main)(int, char *[]), int argc, char *argv[])
{
    return SubmitTask(new gmMainTask(gridmd_main, argc, argv));
}

gmTaskID gmThreadPool::CreateScriptTask(const char *command)
{
    return SubmitTask(new gmScriptTask(command));
}

int gmThreadPool::TaskResult(gmTaskID taskID)
{
    int result = GMPOOLTASK_INVALID_RESULT;

    if(IsValidIndex(taskID)) {
        gmTask* task = mTasksMap.at(taskID);
        result = task->Result();
        mTasksMap.erase(taskID);
        delete task;
    }

    return result;
}

gmTASK_STATUS gmThreadPool::TaskStatus(gmTaskID taskID) const
{
    if(IsValidIndex(taskID))
        return mTasksMap.at(taskID)->Status();

    return gmTASK_INVALID_STATUS;
}

gmdString gmThreadPool::StrTaskStatus(gmTaskID taskID) const
{
    gmdString result;
    switch(TaskStatus(taskID)) {
    case gmTASK_INVALID_STATUS:
        result = "Invalid Status";
        break;
    case gmTASK_POOLED:
        result = "Pooled";
        break;
    case gmTASK_PROCESSED:
        result = "Processed";
        break;
    case gmTASK_CANCELED:
        result = "Canceled";
        break;
    case gmTASK_FINISHED:
        result = "Finished";
        break;
    }
    return result;
}

gmTASK_TYPE gmThreadPool::TaskType(gmTaskID taskID) const
{
    if(IsValidIndex(taskID))
        return mTasksMap.at(taskID)->Type();


    return gmTASK_INVALID_TYPE;
}

void gmThreadPool::RemoveTask(gmTaskID taskID)
{

    if(!IsValidIndex(taskID))
        return;

    gmTask* task = NULL;
    {
        wxMutexLocker lock(mQueueMutex);
        std::map<gmTaskID, gmTask*>::iterator mapIter = mTasksMap.find(taskID);
        task = mapIter->second;

        if(task->Status() == gmTASK_POOLED) {
            std::list<gmTask*>::iterator itCurTask = std::find(mTasksQueue.begin(), mTasksQueue.end(), task);
            if (itCurTask != mTasksQueue.end())
                mTasksQueue.erase(itCurTask);
        }
        else
            if (task->Status() == gmTASK_PROCESSED) {
                gmThreadId idToDelete = task->mThread->GetId();
                task->Kill();
                {
                    wxMutexLocker lock(mRedirectorsMutex);
                    std::list<gmRedirectorBase*>::iterator redIter = mRedirectors.begin();
                    for(;redIter != mRedirectors.end(); ++redIter)
                        (*redIter)->RemoveObject(idToDelete);
                }
            }
    }

    mTasksMap.erase(taskID);
    delete task;
}

bool gmThreadPool::IsValidIndex(gmTaskID taskID) const
{
    wxMutexLocker lock(mQueueMutex);
    return (mTasksMap.count(taskID)!=0);
}


gmTaskID gmThreadPool::SubmitTask(gmTask *task)
{
    {
        wxMutexLocker lock(mQueueMutex);
        mTasksQueue.push_back(task);
    }
    mQueueNotifier.Signal();
    mTasksMap[wxPointerHash()(task)] = task;
    return wxPointerHash()(task);
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


