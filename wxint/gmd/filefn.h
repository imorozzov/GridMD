// Redefinitions for wxWidgets library classes

#ifndef GRIMD_FILEFN_H
#define GRIMD_FILEFN_H

#include <wx/filefn.h>

#define gmdFileExists  wxFileExists
#define gmdDirExists  wxDirExists
#define gmdMkdir  wxMkdir
#define gmdRmdir  wxRmdir
#define gmdGetCwd  wxGetCwd
#define gmdCopyFile  wxCopyFile
#define gmdRemoveFile  wxRemoveFile

#endif
