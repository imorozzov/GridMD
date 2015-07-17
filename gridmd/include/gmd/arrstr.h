// String arrays (based on std::vector<gmdString>)

#ifndef GRIMD_ARRSTR_H
#define GRIMD_ARRSTR_H

#include <vector>

#include <gmd/string.h>


class gmdArrayString : public std::vector<gmdString> {
public:
  size_t GetCount() const { return size(); }
  size_t Count() const { return GetCount(); }
  bool IsEmpty() const { return empty(); }
  gmdString Last() const { return at(size() - 1); }
  
  void RemoveAt(size_t nIndex, size_t count = 1) {
    erase(begin() + nIndex, begin() + nIndex + count);
  }

  size_t Add(const gmdString& str) {
    push_back(str);
    return size() - 1;
  }

  void Clear() { clear(); }
};

#endif
