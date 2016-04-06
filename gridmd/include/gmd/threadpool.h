#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <list>
#include <map>
#include <logexc.h>

class gmTask;
class gmThread;

class gmdThreadPool
{
public:

    gmdThreadPool() {;}
    ~gmdThreadPool() {;}

    gmTaskID CreateGMMainTask(int (*gridmd_main)(int, char*[]), int argc, char* argv[])
    {
        return LOGERR(GMPOOLTASK_INVALID_ID, "gmdThreadPool::CreateGMMainTask(int (*gridmd_main)(int, char*[]),"
                                             "int argc, char* argv[]) is not implemented", false);
    }
    gmTaskID CreateScriptTask(const char* command)
    {
        return LOGERR(GMPOOLTASK_INVALID_ID, "gmdThreadPool::CreateScriptTask(const char* command) "
                                             "is not implemented", false);
    }

    int TaskResult(gmTaskID taskID)
    {
        return LOGERR(GMPOOLTASK_INVALID_RESULT, "gmdThreadPool::TaskResult(gmTaskID taskID) "
                                                 "is not implemented", false);
    }

    gmTASK_STATUS TaskStatus(gmTaskID taskID)
    {
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
        return static_cast<gmTASK_TYPE>(LOGERR(gmTASK_INVALID_TYPE, "gmdThreadPool::TaskType(gmTaskID taskID) "
                                           "is not implemented", false));
    }

    void RemoveTask(gmTaskID taskID) {
        LOGMSG(vblOERR, "gmdThreadPool::RemoveTask(gmTaskID taskID) "
                                    "is not implemented", false);

    }

    bool IsValidIndex(gmTaskID taskID) {
         LOGMSG(vblOERR, "gmdThreadPool::IsValidIndex(gmTaskID taskID) "
                        "is not implemented", false);
    }
};

#endif // THREADPOOL_H
