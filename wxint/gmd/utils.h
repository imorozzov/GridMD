// Redefinitions for wxWidgets library classes

#ifndef GRIMD_UTILS_H
#define GRIMD_UTILS_H

#include <wx/utils.h>

#define gmdExecute  wxExecute
#define gmdKill  wxKill
#define gmdGetUserName  wxGetUserName
#define gmdGetHostName  wxGetHostName
#define gmdGetOsDescription wxGetOsDescription
#define gmdMilliSleep  wxMilliSleep
#define gmdSleep  wxSleep

#define gmdEXEC_SYNC  wxEXEC_SYNC
#define gmdSIGKILL  wxSIGKILL
#define gmdKILL_CHILDREN  wxKILL_CHILDREN

#endif
