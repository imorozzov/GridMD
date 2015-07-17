// Regular expressions

#include <gmd/regex.h>

bool gmdRegEx::Compile(const gmdString& pattern) {
  e.assign(pattern.c_str(),
    boost::regex_constants::extended | boost::regex_constants::no_except);
  valid = !e.empty();
  return valid;
}


bool gmdRegEx::Matches(const gmdString& text) const {
  return valid ? boost::regex_match(text.c_str(), e) : false;
}
