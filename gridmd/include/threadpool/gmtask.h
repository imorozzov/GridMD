#ifndef TASK_H
#define TASK_H

#include <wx/thread.h>

#define INVALID_TASK_RESULT -1

class gmThread;
class gmTask
{
public:

    enum gmTASK_STATUS {
        gmTASK_IDLE = 0,
        gmTASK_POOLED,
        gmTASK_PROCESSED,
        gmTASK_FAILED,
        gmTASK_FINISHED
    };

    enum gmTASK_TYPE {
        gmTASK_GRIDMD_MAIN = 0,
        gmTASK_COMMAND,
        gmTASK_NODEACTION,
    };

    gmTask();
    virtual ~gmTask() {;}
    int Result();
    gmTASK_STATUS Status();
    wxString strStatus();
    virtual gmTASK_TYPE Type() const = 0;
    void Kill();

protected:

    virtual int Run() = 0;

private:

    bool ResetThread(gmThread *thread = NULL);
    void SetStatus(gmTASK_STATUS status);

    gmThread* mThread;
    wxMutex mStatusMutex;
    wxMutex mResultMutex;
    wxCondition mExited;
    gmTASK_STATUS mStatus;
    int mTaskResult;

    friend class gmThread;
    friend class gmThreadPool;
};



#endif // TASK_H
