// Simple buffered file I/O

#ifndef GRIMD_FFILE_H
#define GRIMD_FFILE_H

#include <stdio.h>

#include <gmd/string.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 gmdFileOffset;
#else
typedef long long gmdFileOffset;
#endif

#if !defined(_WIN32)
  #define gmdFseek fseeko
  #define gmdFtell ftello
  typedef off_t gmdFseekType;
#elif defined __VISUALC__ && __VISUALC__ >= 1400
  #define gmdFseek _fseeki64
  #define gmdFtell _ftelli64
  typedef __int64 gmdFseekType;
#else
  #define gmdFseek fseek
  #define gmdFtell ftell
  typedef long gmdFseekType;
#endif


class gmdFFile {
protected:
  FILE* m_fp;
  gmdString m_name;

  bool open_file(const char* filename, const char* mode, bool break_on_err);

public:
  bool Open(const char* filename, const char* mode = "r");
  bool Close();

  gmdFileOffset Tell() const {
    return (gmdFileOffset)gmdFtell(m_fp);
  }

  bool Seek(gmdFileOffset ofs) {
    if((gmdFseekType)ofs != ofs) return false;
    return !fseek(m_fp, (gmdFseekType)ofs, SEEK_SET);
  }

  // Library implementations are allowed to not meaningfully support SEEK_END
  bool SeekEnd(gmdFileOffset ofs = 0) {
    if((gmdFseekType)ofs != ofs) return false;
    return !fseek(m_fp, (gmdFseekType)ofs, SEEK_END);
  }

  size_t Read(void *pBuf, size_t nCount);
  size_t Write(const void *pBuf, size_t nCount);
  bool Write(const gmdString& s);
  bool ReadAll(gmdString *str);

  bool IsOpened() const { return m_fp != NULL; }
  bool Eof() const { return feof(m_fp) != 0; }
  bool Error() const { return ferror(m_fp) != 0; }

  gmdFFile() : m_fp(NULL) {}
  gmdFFile(const char* filename, const char* mode = "r") : m_fp(NULL) {
    open_file(filename, mode, true);
  }

  ~gmdFFile() { Close(); }
};

#endif
