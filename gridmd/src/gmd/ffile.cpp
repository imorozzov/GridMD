// Simple buffered file I/O

#include <gmd/ffile.h>

#include "logexc.h"


bool gmdFFile::open_file(const char* filename, const char* mode, bool break_on_err) {
  m_fp = fopen(filename, mode);
  if ( !m_fp ) {
    if(break_on_err) LOGERR0(fmt("can't open file '%s'", filename));
    return false;
  }
  m_name = filename;
  return true;
}


bool gmdFFile::Open(const char* filename, const char* mode){
  if( IsOpened() ) LOGERR0("FFile: file must be closed before reopening");
  return open_file(filename, mode, false);
}


bool gmdFFile::Close(){
  if ( IsOpened() ) {
    if ( fclose(m_fp) != 0 ) {
        LOGERR0(fmt("FFile: can't close file '%s'", m_name.c_str()));
        return false;
    }
    m_fp = NULL;
  }
  return true;
}


size_t gmdFFile::Read(void *pBuf, size_t nCount){
  if( !IsOpened() ) LOGERR0("can't read from a closed file");

  size_t nRead = fread(pBuf, 1, nCount, m_fp);
  if ( (nRead < nCount) && Error() )
    LOGERR0(fmt("Read error on file '%s'", m_name.c_str()));

  return nRead;
}


size_t gmdFFile::Write(const void *pBuf, size_t nCount){
  if( !IsOpened() ) LOGERR0("can't write to a closed file");

  size_t nWritten = fwrite(pBuf, 1, nCount, m_fp);
  if ( nWritten < nCount )
    LOGERR0(fmt("Write error on file '%s'", m_name.c_str()));

  return nWritten;
}

bool gmdFFile::Write(const gmdString& s){
    const char* buf = s.c_str();
    size_t size = s.Len();
    return Write(buf, size) == size;
}



bool gmdFFile::ReadAll(gmdString *str){
  if( !IsOpened() ) LOGERR0("can't read from a closed file");

  str->Clear();
  const int BUFSIZE = 1024;
  char buf[BUFSIZE];

  while( !feof(m_fp) ) {
    size_t nRead = fread(buf, sizeof(char), BUFSIZE-1, m_fp);
    if( ferror(m_fp) ) {
      LOGERR0(fmt("Error when reading file %s", m_name.c_str()));
      return false;
    }
    buf[nRead] = 0;
    (*str) += buf;
  }

  return true;
}
