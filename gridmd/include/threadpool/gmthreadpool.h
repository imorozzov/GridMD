#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <wx/thread.h>
#include <list>

class gmTask;
class gmThread;

class gmThreadPool : public wxThread
{
public:
    gmThreadPool(size_t threads = wxThread::GetCPUCount() - 1);
    ~gmThreadPool();
    void SubmitTask(gmTask* task);

protected:
    virtual ExitCode Entry();

private:

    size_t mThreadsCount;
    bool mIsRunning;
    wxMutex mQueueMutex;
    wxCondition mQueueNotifier;
    wxMutex mThreadsMutex;
    wxCondition mThreadsNotifier;

    std::list<gmThread*> mThreads;
    std::list<gmTask*> mTasks;

    friend class gmThread;
};

#endif // THREADPOOL_H
