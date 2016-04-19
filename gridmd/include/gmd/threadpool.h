#ifndef GRIDMD_THREADPOOL_H
#define GRIDMD_THREADPOOL_H


#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <gmd/redirector.h>
#include <list>
#include <map>
#include <typeinfo>
#include <logexc.h>


class gmdThreadPool
{
public:

    gmdThreadPool() {;}

    ~gmdThreadPool() {
        std::map<const std::type_info*, gmdRedirectorBase*>::iterator mapIter = mRedirectorsMap.begin();
        for(;mapIter != mRedirectorsMap.end();++mapIter)
            delete mapIter->second;
    }

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
       LOGMSG(vblOERR, "gmdThreadPool::TaskType(gmTaskID taskID) is not implemented", false);
       return gmTASK_INVALID_TYPE;
    }

    void RemoveTask(gmTaskID taskID) {
        LOGMSG(vblOERR, "gmdThreadPool::RemoveTask(gmTaskID taskID) "
                                    "is not implemented", false);

    }

    bool IsValidIndex(gmTaskID taskID) {
         LOGMSG(vblOERR, "gmdThreadPool::IsValidIndex(gmTaskID taskID) "
                         "is not implemented", false);
    }


    template <typename T>
    void RegisterPrototypedRedirector(const T& prototype) {

        ///TODO: here must be mutex for mRedirectorsMap
        std::map<const std::type_info*, gmdRedirectorBase*>::iterator mapIter = mRedirectorsMap.find(&typeid(T));
        if (mapIter != mRedirectorsMap.end())
            delete mapIter->second;
        mRedirectorsMap[&typeid(T)] = new gmdRedirectorPrototyped<T>(prototype);
    }

    static gmdRedirectorBase* GetRedirector(const std::type_info &typeInfo) {

        ///TODO: here must be mutex for mRedirectorsMap
        std::map<const std::type_info*, gmdRedirectorBase*>::const_iterator mapIter = mRedirectorsMap.find(&typeInfo);
        if (mapIter != mRedirectorsMap.end())
            return mapIter->second;
        else
            return NULL;
    }

private:
    struct TypeInfoComparator {
        bool operator()(const std::type_info* left, const std::type_info* right) const {
            return left->before(*right);
        }
    };

    static std::map<const std::type_info*, gmdRedirectorBase*, TypeInfoComparator> mRedirectorsMap;
};

#endif // GRIDMD_THREADPOOL_H
