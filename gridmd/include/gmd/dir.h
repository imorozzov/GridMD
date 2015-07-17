// Enumerating of the files in a directory 

#ifndef GRIMD_DIR_H
#define GRIMD_DIR_H

#include <gmd/string.h>
#include <gmd/arrstr.h>

enum {
  gmdDIR_FILES     = 0x0001,       // include files
  gmdDIR_DIRS      = 0x0002,       // include directories
  gmdDIR_HIDDEN    = 0x0004,       // include hidden files
  gmdDIR_DOTDOT    = 0x0008,       // include '.' and '..'

  // by default, enumerate everything except '.' and '..'
  gmdDIR_DEFAULT   = gmdDIR_FILES | gmdDIR_DIRS | gmdDIR_HIDDEN
};


enum gmdDirTraverseResult {
    gmdDIR_IGNORE = -1,      // ignore this directory but continue with others
    gmdDIR_STOP,             // stop traversing
    gmdDIR_CONTINUE          // continue into this directory
};


class gmdDirTraverser {
public:
  virtual ~gmdDirTraverser() { }
  virtual gmdDirTraverseResult OnFile(const gmdString& filename) = 0;
  virtual gmdDirTraverseResult OnDir(const gmdString& dirname) = 0;
};


class gmdDir {
private:
  gmdString m_dir;
  bool fOpened;

  static bool wild_match(const char* pat, const char* str);

  template<class DirIterator>
  size_t list(gmdDirTraverser& sink, const gmdString& filespec, int flags);

public:
  gmdDir() : fOpened(false) {}
  gmdDir(const gmdString& dir) : fOpened(true), m_dir(dir) {}

  bool Open(const gmdString& dir) {
    m_dir = dir;
    fOpened = true;
    return true;
  }

  bool IsOpened() const {
    return fOpened;
  }

  static size_t GetAllFiles(const gmdString& dirname, gmdArrayString *files,
                            const gmdString& filespec = "",
                            int flags = gmdDIR_DEFAULT);

  size_t Traverse(gmdDirTraverser& sink, const gmdString& filespec = "",
                  int flags = gmdDIR_DEFAULT);
};

#endif
