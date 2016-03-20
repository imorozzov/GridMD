#ifndef SCRIPTTASK_H
#define SCRIPTTASK_H

#include "gmtask.h"

class gmScriptTask : public gmTask
{
public:
    gmScriptTask(const char* command);
    virtual gmTASK_TYPE Type() const;

protected:
    virtual int Run();

private:
    const char* mCommand;
};

#endif // SCRIPTTASK_H
