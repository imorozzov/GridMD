#ifndef THREAD_H
#define THREAD_H

#include <wx/thread.h>

class gmTask;
class gmThreadPool;

class gmThread : public wxThread
{
public:
    gmThread(gmThreadPool *pool);
    virtual ~gmThread();
    void Start();

protected:
    virtual ExitCode Entry();

private:
    void StartTask();

    gmTask* mCurrentTask;
    gmThreadPool* mPool;
};

#endif // THREAD_H
