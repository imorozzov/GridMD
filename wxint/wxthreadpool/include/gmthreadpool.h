#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <wx/thread.h>
#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <list>
#include <map>
#include <typeinfo>

#include <wxthreadpool/include/gmredirector.h>

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
    gmTASK_STATUS TaskStatus(gmTaskID taskID) const;


    ///\en Returns string representation of \ref gmThreadPool::TaskStatus().
    ///    \return String representation of tasks status
    gmdString StrTaskStatus(gmTaskID taskID) const;

    ///\en Gets task type.
    ///    Possible return values are:\n
    ///    \a gmTASK_GRIDMD_MAIN -- task is \ref gridmd_main(int argc, char **argv) function execution.\n
    ///    \a gmTASK_COMMAND -- task is script execution.\n
    ///    \a gmTASK_INVALID_TYPE -- is returned if \a taskID index is not valid (see \ref gmThreadPool::IsValidIndex()).
    ///    \param taskID
    ///    \return Task type
    gmTASK_TYPE TaskType(gmTaskID taskID) const;

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
    bool IsValidIndex(gmTaskID taskID) const;


    ///\en Registers a new \ref gmRedirectorPrototyped object with \a prototype.
    ///    Use this method to register prototype if you want to fetch a copy of it
    ///    later with \ref gmThreadPool::GetRedirector(). Note, that in current implementation
    ///    you can register only one instance of certain type (i.e. you can't
    ///    store two or more prototypes of one type).
    ///    \param prototype
    template <typename T>
    void RegisterPrototypedRedirector(const T& prototype) {
        wxMutexLocker lock(mRedirectorsMutex);
        std::map<const std::type_info*, gmRedirectorBase*>::iterator mapIter = mRedirectorsMap.find(&typeid(T));
        if (mapIter != mRedirectorsMap.end())
            delete mapIter->second;
        mRedirectorsMap[&typeid(T)] = new gmRedirectorPrototyped<T>(prototype);
    }

    ///\en Gets a pointer to the base class of \ref gmRedirector to work
    ///    with an arbitary specialized redirectors.
    ///    \ref gmRedirectorBase is used to abstract from the specialization of
    ///    \ref gmRedirector bazed classes. To fetch unique object for a \ref gmTask
    ///    execution you should do something like
    ///
    ///\code
    ///    gmdRedirectorBase* baseRedirector = gmdThreadPool::GetRedirector(typeid(ObjectType));
    ///    if (baseRedirector) {
    ///        gmdRedirector<ObjectType>* redirector = static_cast<gmdRedirector<ObjectType>* > (baseRedirector);
    ///        ObjectType* object = redirector->GetObject();
    ///        if (object) {
    ///            /*...*/
    ///       }
    ///    }
    ///\endcode
    ///
    ///    within the function of \ref gmTask execution (for example \ref gridmd_main(int argc, char **argv)
    ///    of \ref gmMainTask), or from the main thread. You should previously register prototype of \a ObjectType
    ///    with \ref gmThreadPool::RegisterPrototypedRedirector(). See \ref redirector_example.cpp to get more
    ///    info how to work with that mechanism.
    ///
    ///    \param typeInfo std::type_info instance of queried \ref gmRedirector specialization
    ///    type fetched by \a typeid(ObjectType).
    ///    \return \ref gmRedirectorBase instance or NULL if there's no such specialization for
    ///    queried type.
    static gmRedirectorBase* GetRedirector(const std::type_info &typeInfo);

protected:
    virtual ExitCode Entry();

private:

    gmTaskID SubmitTask(gmTask* task);

    size_t mThreadsCount;
    bool mIsRunning;
    mutable wxMutex mQueueMutex;
    wxCondition mQueueNotifier;
    wxMutex mThreadsMutex;
    wxCondition mThreadsNotifier;

    std::list<gmThread*> mThreads;
    std::list<gmTask*> mTasksQueue;
    std::map<gmTaskID, gmTask*> mTasksMap;

    struct TypeInfoComparator {
        bool operator()(const std::type_info* left, const std::type_info* right) const {
            return left->before(*right);
        }
    };

    static wxMutex mRedirectorsMutex;
    static std::map<const std::type_info*, gmRedirectorBase*, TypeInfoComparator> mRedirectorsMap;

    friend class gmThread;
};






#endif // THREADPOOL_H
