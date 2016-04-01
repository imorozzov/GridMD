#ifndef GMTHREADDEFS_H
#define GMTHREADDEFS_H

enum gmTASK_TYPE {
    gmTASK_INVALID_TYPE = -1,
    gmTASK_GRIDMD_MAIN,
    gmTASK_COMMAND
};

enum gmTASK_STATUS {
    gmTASK_INVALID_STATUS = -1,
    gmTASK_POOLED,
    gmTASK_PROCESSED,
    gmTASK_CANCELED,
    gmTASK_FINISHED
};

#define GMPOOLTASK_INVALID_RESULT -1

#ifdef USING_WXWIDGETS
#include <wx/defs.h>
typedef wxUIntPtr gmTaskID;
#else
typedef size_t gmTaskID;
#endif

#endif // GMTHREADDEFS_H
