// Regular expressions
// Use the EXTENDED regular expression type (Perl, awk and egrep)

#ifndef GRIMD_REGEX_H
#define GRIMD_REGEX_H

#include <boost/regex.hpp>

#include <gmd/string.h>

class gmdRegEx {
protected:
  boost::regex e;
  bool valid;

public:
  bool Compile(const gmdString& pattern);
  bool Matches(const gmdString& text) const;
  bool IsValid() const { return valid; }

  gmdRegEx() : valid(false) {};
  gmdRegEx(const gmdString& pattern) { Compile(pattern); }
};

#endif
