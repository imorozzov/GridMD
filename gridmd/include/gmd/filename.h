// File name decomposition

#ifndef GRIMD_FILENAME_H
#define GRIMD_FILENAME_H

#include <gmd/string.h>

#include <boost/filesystem.hpp>

enum gmdPathFormat {
  gmdPATH_NATIVE = 0,
  gmdPATH_UNIX = 1,
  gmdPATH_WIN = 5
};


class gmdFileName {
protected:
  boost::filesystem::path m_path;

public:
  gmdString GetFullPath() const {
#ifdef _WIN32
    gmdString p = m_path.generic_string();
    p.Replace("/","\\");
    return p;
#else
    return m_path.generic_string();
#endif
  }

  gmdString GetPath() const {
    return m_path.parent_path().string();
  }

  gmdString GetFullName() const {
    return m_path.filename().string();
  }

  gmdString GetName() const {
    return m_path.stem().string();
  }

  static void SplitPath(
    const gmdString& fullpath, gmdString* volume, gmdString* path,
    gmdString* name, gmdString* ext, gmdPathFormat format = gmdPATH_NATIVE);

  static void SplitPath(
    const gmdString& fullpath, gmdString* path,
    gmdString* name, gmdString* ext, gmdPathFormat format = gmdPATH_NATIVE);

  static gmdString GetTempDir();
  static gmdString CreateTempFileName(const gmdString& prefix);

  gmdFileName() {}
  gmdFileName(const char* fullpath, gmdPathFormat format = gmdPATH_NATIVE)
      : m_path(fullpath) { (void) format; }
};

#endif
