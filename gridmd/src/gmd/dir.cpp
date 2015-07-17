// Enumerating of the files in a directory 

#include <boost/filesystem.hpp>

#include <gmd/dir.h>

#include "logexc.h"

#ifdef _WIN32
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif

using namespace boost::filesystem;


class gmdDirTraverserSimple : public gmdDirTraverser {
private:
  gmdArrayString* m_files;

public:
  gmdDirTraverserSimple(gmdArrayString* files) : m_files(files) {}

  virtual gmdDirTraverseResult OnFile(const gmdString& filename) {
    m_files->Add(filename);
    return gmdDIR_CONTINUE;
  }

  virtual gmdDirTraverseResult OnDir(const gmdString& dirname) {
    (void)dirname;
    return gmdDIR_CONTINUE;
  }
};


size_t gmdDir::GetAllFiles(const gmdString& dirname, gmdArrayString *files,
                           const gmdString& filespec, int flags)
{
  gmdDir dir(dirname);
  gmdDirTraverserSimple traverser(files);
  return dir.Traverse(traverser, filespec, flags);
}


// Wildcard matching algorithm is taken from 
// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html
bool gmdDir::wild_match(const char* pat, const char* str) {
   while (*str) {
      switch (*pat) {
         case '?':
            if (*str == '.') return false;
            break;
         case '*':
            do { ++pat; } while (*pat == '*'); /* enddo */
            if (!*pat) return true;
            while (*str) if (wild_match(pat, str++)) return true;
            return false;
         default:
            if(*str != *pat) return false;
            break;
      } /* endswitch */
      ++pat, ++str;
   } /* endwhile */
   while (*pat == '*') ++pat;
   return !*pat;
}


template<class DirIterator>
size_t gmdDir::list(gmdDirTraverser& sink, const gmdString& filespec, int flags) {
#ifdef _WIN32
  gmdString fspec = filespec.Upper();
#else
  gmdString fspec(filespec);
#endif

  boost::system::error_code ec;
  DirIterator end, i(path(m_dir.c_str()), ec);
  if( ec ) LOGERR0( fmt("gmdDir::Traverse failed: %s", ec.message().c_str()));

  size_t fcount = 0;
  for(; i != end; i++) {
    if((flags & gmdDIR_FILES) && is_regular_file( i->status() ) ) {
      gmdString fn = i->path().filename().string();
#ifdef _WIN32
      fn = fn.Upper();
      if( i->path().extension().string().empty() ) fn += ".";
#endif
      if( !fspec.IsEmpty() && !wild_match(fspec, fn) ) continue;
      fcount++;
      if( sink.OnFile( i->path().string() ) == gmdDIR_STOP ) break;
    }
    else if(flags & gmdDIR_DIRS) {
      if( sink.OnDir( i->path().string() ) == gmdDIR_STOP ) break;
    }
  }

  return fcount;
}


size_t gmdDir::Traverse(gmdDirTraverser& sink, const gmdString& filespec, int flags)
{
  if( !IsOpened() ) return (size_t) -1;

  return (flags & gmdDIR_DIRS) ?
    list<recursive_directory_iterator>(sink, filespec, flags) :
    list<directory_iterator>(sink, filespec, flags);
}
