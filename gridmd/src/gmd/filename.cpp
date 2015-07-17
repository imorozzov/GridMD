// File name decomposition

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif

#include <boost/filesystem.hpp>
#include "logexc.h"

#include <gmd/filename.h>


void gmdFileName::SplitPath(
  const gmdString& fullpath, gmdString* volume, gmdString* path,
  gmdString* name, gmdString* ext, gmdPathFormat format)
{
  boost::filesystem::path p(fullpath.c_str());
  if(volume) {
    *volume = p.root_name().string();
    if( volume->EndsWith(":") ) volume->Truncate(volume->Len()-1);
  }
  if(path) {
    gmdString vol = p.root_name().string();
    gmdString parent = p.parent_path().string();
    parent.StartsWith(vol, path);
  }
  if(name) *name = p.stem().string();
  if(ext) *ext = gmdString(p.extension().string()).Mid(1);
}


void gmdFileName::SplitPath(
  const gmdString& fullpath, gmdString* path,
  gmdString* name, gmdString* ext, gmdPathFormat format)
{
  boost::filesystem::path p(fullpath.c_str());
  if(path) *path = p.parent_path().string();
  if(name) *name = p.stem().string();
  if(ext) *ext = gmdString(p.extension().string()).Mid(1);
}


gmdString gmdFileName::GetTempDir() {
  gmdString p = boost::filesystem::temp_directory_path().string(
#ifdef _MSC_VER
      // see https://svn.boost.org/trac/boost/ticket/6320
      // conversion 'narrow string' <-> 'path' may don't work at stage of
      // initializetion of global variables.
      std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(std::locale(""))
#endif
      );
  //gmdString p = boost::filesystem::temp_directory_path().string();
  if(p.EndsWith("/") || p.EndsWith("\\")) p.Truncate(p.Len()-1);
  return p;
}


gmdString gmdFileName::CreateTempFileName(const gmdString& prefix){
  gmdString dir, name;
  SplitPath(prefix, &dir, &name, NULL);
  if (dir.empty()) dir = GetTempDir();

#ifdef _WIN32
  char buf[MAX_PATH+1];
  if( !GetTempFileNameA(dir, name, 0, buf) ) LOGERR0("GetTempFileName failed");
  return buf;
#else
  gmdString path(dir);
  if( !dir.EndsWith("/") && name.IsEmpty() ) path += "/";
  path += name + "XXXXXX";
  char *buf = new char[path.Len()+1];
  strcpy(buf, path.c_str());
  if( !mkstemp(buf) ) LOGERR0("GetTempFileName failed");
  path = buf;
  delete[] buf;
  return path;
#endif
}
