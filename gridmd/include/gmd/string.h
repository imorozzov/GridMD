// Basic string manipulations (based on std::string)

#ifndef GRIMD_STRING_H
#define GRIMD_STRING_H

#include <string>

typedef char gmdChar;
typedef std::basic_string<gmdChar> gmdStringBase;

#define gmdSTRING_MAXLEN gmdStringBase::npos
enum { gmdNOT_FOUND = -1 };


class gmdString : public gmdStringBase {
public:
  size_t Len() const { return length(); }
  size_t Length() const { return Len(); }

  operator const gmdChar*() const { return c_str(); }

  bool IsEmpty() const { return empty(); }

  void Clear() { clear(); }

  bool Contains(const gmdString& str) const {
    return find(str) != npos;
  }

  gmdString Left(size_t count) const {
    return substr(0, count);
  }

  gmdString Right(size_t count) const {
    return count > Len() ? *this : (gmdString)substr(Len() - count, count);
  }

  gmdString& Truncate(size_t len) {
    if(len < Len()) resize(len);
    return *this;
  }

  gmdString& RemoveLast() {
    if( !IsEmpty() ) resize(Len()-1);
    return *this;
  }

  gmdString Mid(size_t first, size_t count = gmdSTRING_MAXLEN) const {
    return first >= Len() ? "" : substr(first, count);
  }

  int Find(const gmdChar* sz) const { 
    size_t res = find(sz);
    return (res == npos) ? gmdNOT_FOUND : (int)res;
  }
  
  int Find(const gmdChar ch, bool fromEnd = false) const {
    size_t res = fromEnd ? rfind(ch) : find(ch);
    return (res == npos) ? gmdNOT_FOUND : (int)res;
  }

  bool EndsWith(const gmdChar *suffix, gmdString *rest = NULL) const;
  bool StartsWith(const gmdChar *suffix, gmdString *rest = NULL) const;

  gmdString BeforeFirst(gmdChar ch) const {
    size_t pos = find(ch);
    return pos == npos ? *this : (gmdString)substr(0, pos);
  }

  gmdString BeforeLast(gmdChar ch) const {
    size_t pos = rfind(ch);
    return pos == npos ? "" : substr(0, pos);
  }

  gmdString AfterFirst(gmdChar ch) const {
    size_t pos = find(ch);
    return pos == npos ? "" : substr(pos + 1);
  }

  gmdString AfterLast(gmdChar ch) const {
    size_t pos = rfind(ch);
    return pos == npos ? *this : (gmdString)substr(pos + 1);
  }

  size_t Replace(const gmdChar* szOld, const gmdChar* szNew,
                 bool replaceAll = true);

  gmdString& Trim(bool fromRight = true);

  gmdString Lower() const;
  gmdString Upper() const;

  bool ToLong(long *val, int base = 10) const;
  bool ToDouble(double *val) const;

  gmdChar& operator[](int n) { return at(n); }

  gmdChar Last() const { return at(Len()-1); }

  friend gmdString operator+(const gmdString& string1, const gmdString& string2);
  friend gmdString operator+(const gmdString& string, const gmdChar *psz);
  friend gmdString operator+(const gmdChar *psz, const gmdString& string);
  friend gmdString operator+(const gmdString& string, const gmdChar psz);
  friend gmdString operator+(const gmdChar psz, const gmdString& string);

  static gmdString Format(const gmdChar *format, ...);

  gmdString() {}
  gmdString(const gmdChar* psz) : gmdStringBase(psz) {}
  gmdString(const gmdStringBase& str) : gmdStringBase(str) {}
};


inline gmdString operator+(const gmdString& string1, const gmdString& string2) {
  return (gmdStringBase&)string1 + (gmdStringBase&)string2;
}

inline gmdString operator+(const gmdString& string, const gmdChar *psz) {
  return (gmdStringBase&)string + psz;
}

inline gmdString operator+(const gmdChar *psz, const gmdString& string) {
  return psz + (gmdStringBase&)string;
}

inline gmdString operator+(const gmdString& string, const gmdChar psz) {
  return (gmdStringBase&)string + psz;
}

inline gmdString operator+(const gmdChar psz, const gmdString& string) {
  return psz + (gmdStringBase&)string;
}

#endif
