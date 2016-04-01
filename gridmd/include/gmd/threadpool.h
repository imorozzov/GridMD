#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <gmdefs.h>
#include <gmd/string.h>
#include <list>
#include <map>

class gmTask;
class gmThread;

class gmdThreadPool
{
public:

    gmdThreadPool() {;}
    ~gmdThreadPool() {;}

    gmTaskID CreateGMMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[])
    {
        std::cout << "gmdThreadPool::CreateGMMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[]) is not implemented" << std::endl;
        return 0;
    }
    gmTaskID CreateScriptTask(const char* command)
    {
        std::cout << "gmdThreadPool::CreateScriptTask(const char* command) is not implemented" << std::endl;
        return 0;
    }

    int TaskResult(gmTaskID taskID)
    {
        std::cout << "gmdThreadPool::TaskResult(gmTaskID taskID) is not implemented" << std::endl;
        return GMPOOLTASK_INVALID_RESULT;
    }

    gmTASK_STATUS TaskStatus(gmTaskID taskID)
    {
        std::cout << "gmdThreadPool::TaskStatus(gmTaskID taskID) is not implemented" << std::endl;
        return gmTASK_INVALID_STATUS;
    }

    gmdString StrTaskStatus(gmTaskID taskID)
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

    gmTASK_TYPE TaskType(gmTaskID taskID) {
        std::cout << "gmdThreadPool::TaskType(gmTaskID taskID) is not implemented" << std::endl;
        return gmTASK_INVALID_TYPE;
    }

    void RemoveTask(gmTaskID taskID) {
        std::cout << "gmdThreadPool::RemoveTask(gmTaskID taskID) is not implemented" << std::endl;
    }

    bool IsValidIndex(gmTaskID taskID) {
         std::cout << "gmdThreadPool::IsValidIndex(gmTaskID taskID) is not implemented" << std::endl;
         return false;
    }
};

#endif // THREADPOOL_H
