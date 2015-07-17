// File manipulations (system dependent)

#include <boost/filesystem.hpp>
#include "logexc.h"

#include <gmd/filefn.h>

using namespace boost::filesystem;

bool gmdRemovePath(const char* path);


bool gmdFileExists(const char *file) {
  return is_regular_file(file);
}


bool gmdDirExists(const char *dir){
  return is_directory(dir);
}


bool gmdMkdir(const gmdString& dir){
  boost::system::error_code ec;
  create_directory(dir.c_str(), ec);
  //if( ec ) LOGERR0(fmt("Directory %s cannot be created: %s",
  //  dir.c_str(), ec.message().c_str()));
  return !ec.value();
}


gmdString gmdGetCwd(){
  boost::system::error_code ec;
  path cwd = current_path(ec);
  if( ec ) LOGERR0( fmt("gmdGetCwd failed: %s", ec.message().c_str()));
  return cwd.string(
            #ifdef _MSC_VER
                  // see https://svn.boost.org/trac/boost/ticket/6320
                  // conversion 'narrow string' <-> 'path' may don't work at stage of
                  // initializetion of global variables.
                  std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(std::locale(""))
            #endif
              );
}


bool gmdRemoveFile(const gmdString& file){
  if( !gmdFileExists(file) )
    LOGERR0(fmt("gmdRemoveFile: file %s doesn't exist", file.c_str()));
  return gmdRemovePath(file);
}


bool gmdRmdir(const gmdString& dir){
  if( !gmdDirExists(dir) )
    LOGERR0(fmt("gmdRemoveFile: directory %s doesn't exist", dir.c_str()));
  return gmdRemovePath(dir);
}


bool gmdCopyFile(const gmdString& file1, const gmdString& file2, bool overwrite) {
  boost::system::error_code ec;
  copy_file(file1.c_str(), file2.c_str(),
    (overwrite ? copy_option::overwrite_if_exists : copy_option::none), ec);
  //if( ec ) LOGERR0( fmt("File copy failed '%s' to '%s': %s",
  //  file1.c_str(), file2.c_str(), ec.message().c_str() ) );
  return !ec.value();
}


//----------------------------------------------------------------------------
// Internal functions
//----------------------------------------------------------------------------
bool gmdRemovePath(const char* path){
  boost::system::error_code ec;
  remove(path, ec);
  //if( ec ) LOGERR0(fmt("File/directory %s cannot be removed: %s",
  //  path, ec.message().c_str()));
  return !ec.value();
}
