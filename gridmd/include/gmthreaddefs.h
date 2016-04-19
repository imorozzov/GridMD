#ifndef GMTHREADDEFS_H
#define GMTHREADDEFS_H

/** \file gmthreaddefs.h
    \en \brief Defines for GridMD Thread Pool.

**/

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
#include <wx/hashmap.h>
#include <wx/thread.h>
typedef wxThreadIdType gmThreadId;
#define GMPOOLTASK_INVALID_ID wxPointerHash()(NULL)
#else
#include <stddef.h>
typedef size_t gmTaskID; /// NOTE: replace with actual used type in non-wxWidgets task IDs implementation
typedef size_t gmThreadId; /// NOTE: replace with actual used type in non-wxWidgets thread IDs implementation
#include <limits>
#define GMPOOLTASK_INVALID_ID std::numeric_limits<size_t>::min()
#endif

#endif // GMTHREADDEFS_H
