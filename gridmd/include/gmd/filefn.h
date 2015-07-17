// File manipulations (system dependent)

#ifndef GRIMD_FILEFN_H
#define GRIMD_FILEFN_H

#include <gmd/string.h>


bool gmdFileExists(const char *file);
bool gmdDirExists(const char *dir);

bool gmdMkdir(const gmdString& dir);
bool gmdRmdir(const gmdString& dir);

gmdString gmdGetCwd();

bool gmdRemoveFile(const gmdString& file);
bool gmdCopyFile(const gmdString& file1, const gmdString& file2,
                 bool overwrite = true);

#endif
