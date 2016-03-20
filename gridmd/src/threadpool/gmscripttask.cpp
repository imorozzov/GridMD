#include "threadpool/gmscripttask.h"
#include <stdlib.h>

gmScriptTask::gmScriptTask(const char *command):
    mCommand(command)
{}

gmTask::gmTASK_TYPE gmScriptTask::Type() const
{
    return gmTASK_COMMAND;
}

int gmScriptTask::Run()
{
    return system(mCommand);
}
