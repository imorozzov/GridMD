// Redefinitions for wxWidgets library classes

#ifndef GRIMD_STRING_H
#define GRIMD_STRING_H

#include <wx/string.h>

#define gmdChar  wxChar
#define gmdString  wxString
#define gmdNOT_FOUND wxNOT_FOUND

#define WXSTRING_ADAPTER(string)  static_cast<const char*>(string)

#endif
