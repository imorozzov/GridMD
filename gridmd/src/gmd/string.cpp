// Basic string manipulations (based on std::string)

#include <string.h>
#include <stdlib.h>
#include <algorithm>

#include <gmd/string.h>
#include "logexc.h"


bool gmdString::EndsWith(const gmdChar *suffix, gmdString *rest) const {
  gmdString s(suffix);
  size_t len = s.Len();
  bool res = (Right(len) == s);
  if(res && rest) *rest = substr(0, Len()-len);
  return res;
}


bool gmdString::StartsWith(const gmdChar *suffix, gmdString *rest) const {
  gmdString s(suffix);
  size_t len = s.Len();
  bool res = (Left(len) == s);
  if(res && rest) *rest = substr(len);
  return res;
}


size_t gmdString::Replace(const gmdChar* szOld, const gmdChar* szNew,
                          bool replaceAll)
{
  gmdString res;
  size_t pos = 0, nrep = 0;
  size_t oldlen = strlen(szOld);

  if( !(szOld && *szOld && szNew) )
    LOGERR0("gmdString::Replace(): invalid parameter");

  while(pos < Len()) {
    int pos1 = find(szOld, pos);
    if(pos1 == npos) break;

    nrep++;
    res += substr(pos, pos1 - pos);
    res += szNew;
    pos = pos1 + oldlen;

    if(!replaceAll && nrep == 1) break;
  }

  if(pos < Len()) res += substr(pos);

  *this = res;
  return nrep;
}


gmdString& gmdString::Trim(bool fromRight) {
  if( fromRight ) {
    size_t pos = find_last_not_of(" \t\r\n");
    resize( pos == npos ? 0 : pos + 1 );
  }
  else {
    size_t pos = find_first_not_of(" \t\r\n");
    erase(0, pos);
  }
  return *this;
}


gmdString gmdString::Lower() const {
  gmdString res(*this);
  std::transform(begin(), end(), res.begin(), tolower);
  return res;
}


gmdString gmdString::Upper() const {
  gmdString res(*this);
  std::transform(begin(), end(), res.begin(), toupper);
  return res;
}


bool gmdString::ToLong(long *val, int base) const {
  gmdChar *end;
  *val = strtol(c_str(), &end, base);
  return !*end && (end != c_str());
}


bool gmdString::ToDouble(double *val) const {
  gmdChar *end;
  *val = strtod(c_str(), &end);
  return !*end && (end != c_str());
}


gmdString gmdString::Format(const gmdChar *pszFormat, ...) {
  va_list argptr;
  va_start(argptr, pszFormat);
  std::string ret=fmt_va(pszFormat, argptr);
  va_end(argptr);
  return ret;
}


//------------------------
/*
#include <gmd/ffile.h>

void testtt() {
  gmdString s, s1;
  gmdFFile file;
  file.Open(s + "test");
  file.Open("test" + s);
  file.Open(s + s1);
}
*/
