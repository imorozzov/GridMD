#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <wx/thread.h>
#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <list>
#include <map>

/** \file gmthreadpool.h
    \en \brief Class that implements GridMD Thread Pool.

**/

class gmTask;
class gmThread;


///\en GridMD Thread Pool implementation using wxWidgets.
class gmThreadPool : public wxThread
{
public:

    ///\en Constructor of the Thread Pool.
    ///    Initializes \a threads number of threads and puts them in waiting mode for
    ///    new submitted tasks.
    ///    \param threads number of threads that will work in pool.
    gmThreadPool(size_t threads = wxThread::GetCPUCount() - 1);

    ///\en Destructor of the Thread Pool.
    ///    Kills all working and waiting threads and destroys all queued and
    ///    processed tasks. Destructor blocks caller thread until all the threads
    ///    are destroyed.
    ~gmThreadPool();


    ///\en Creates new task of \ref gridmd_main(int argc, char **argv) function execution.
    ///    Puts created task in a pool queue. Set task status in \a gmTASK_POOLED.
    ///    \return Task id
    gmTaskID CreateGMMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[]);

    ///\en Creates new task of script execution and puts it in a pool queue.
    ///    Set task status in \a gmTASK_POOLED.
    ///    \return Task id
    gmTaskID CreateScriptTask(const char* command);

    ///\en Returns task exit code.
    ///    Waits for task with \a taskID index to complete --
    ///    blocks the caller thread until queried task is performed.
    ///    After destroys task object and returns task exit code.
    ///    If \a taskID index is not valid (see \ref gmThreadPool::IsValidIndex())
    ///    returns GMPOOLTASK_INVALID_RESULT (see definition in \ref gmthreaddefs.h).
    ///    \param taskID
    ///    \return Task exit code
    int TaskResult(gmTaskID taskID);

    ///\en Gets task current status.
    ///    Possible return values are:\n
    ///    \a gmTASK_POOLED -- task has been queued for processing.\n
    ///    \a gmTASK_PROCESSED -- task is being processed by any of the worker threads.\n
    ///    \a gmTASK_FINISHES -- task succesfully finished. call gmThreadPool::TaskResult() to
    ///                          get its exit code and destroy it.\n
    ///    \a gmTASK_INVALID_STATUS -- is returned if \a taskID index is not valid (see \ref gmThreadPool::IsValidIndex()).
    ///    \param taskID
    ///    \return Task status
    gmTASK_STATUS TaskStatus(gmTaskID taskID);


    ///\en Returns string representation of \ref gmThreadPool::TaskStatus().
    ///    \return String representation of tasks status
    gmdString StrTaskStatus(gmTaskID taskID);

    ///\en Gets task type.
    ///    Possible return values are:\n
    ///    \a gmTASK_GRIDMD_MAIN -- task is \ref gridmd_main(int argc, char **argv) function execution.\n
    ///    \a gmTASK_COMMAND -- task is script execution.\n
    ///    \a gmTASK_INVALID_TYPE -- is returned if \a taskID index is not valid (see \ref gmThreadPool::IsValidIndex()).
    ///    \param taskID
    ///    \return Task type
    gmTASK_TYPE TaskType(gmTaskID taskID);

    ///\en Removes task with \a taskID index from pool queue and destroys it.
    ///    After call to this function \ref gmThreadPool::IsValidIndex()
    ///    for queried \a taskID returns \a false. If task is being processed, it
    ///    will be killed immediately. If \a taskID index is not valid
    ///    (see \ref gmThreadPool::IsValidIndex()), does nothing.
    ///    \param taskID
    void RemoveTask(gmTaskID taskID);


    ///\en Checks, if task with \a taskID exists in thread pool.
    ///    Returns \a true if task is queued for execution,
    ///    or being processed, or finished. Note that \ref gmThreadPool::TaskResult()
    ///    and \ref gmThreadPool::RemoveTask() makes \a taskID index invalid.
    ///    \param taskID
    ///    \return Is task with \a taskID valid
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
