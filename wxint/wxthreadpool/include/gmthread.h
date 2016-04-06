#ifndef THREAD_H
#define THREAD_H

#include <wx/thread.h>

class gmTask;
class gmThreadPool;

class gmThread : public wxThread
{ 
protected:

    gmThread(gmThreadPool *pool);
    virtual ~gmThread();
    virtual ExitCode Entry();

    void Start();
    void StartTask();

private:

    gmTask* mCurrentTask;
    gmThreadPool* mPool;

    friend class gmThreadPool;
    friend class gmTask;
};

#endif // THREAD_H
