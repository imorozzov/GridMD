#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <wx/thread.h>
#include <gmdefs.h>
#include <list>
#include <map>

class gmTask;
class gmThread;

class gmThreadPool : public wxThread
{
public:

    gmThreadPool(size_t threads = wxThread::GetCPUCount() - 1);
    ~gmThreadPool();

    gmTaskID CreateGMMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[]);
    gmTaskID CreateScriptTask(const char* command);

    int TaskResult(gmTaskID taskID);
    gmTASK_STATUS TaskStatus(gmTaskID taskID);
    wxString StrTaskStatus(gmTaskID taskID);
    gmTASK_TYPE TaskType(gmTaskID taskID);
    void RemoveTask(gmTaskID taskID);
    bool IsValidIndex(gmTaskID taskID);

protected:
    virtual ExitCode Entry();

private:

    gmTaskID SubmitTask(gmTask* task);

    size_t mThreadsCount;
    bool mIsRunning;
    wxMutex mQueueMutex;
    wxCondition mQueueNotifier;
    wxMutex mThreadsMutex;
    wxCondition mThreadsNotifier;

    std::list<gmThread*> mThreads;
    std::list<gmTask*> mTasksList;
    std::map<gmTaskID, gmTask*> mTasksMap;

    friend class gmThread;
};

#endif // THREADPOOL_H
