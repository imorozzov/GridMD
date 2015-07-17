// String tokenizer

#ifndef GRIMD_TOKENZR_H
#define GRIMD_TOKENZR_H

#include <boost/tokenizer.hpp>
#include <string>

#include <gmd/string.h>


class gmdStringTokenizer {
protected:
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  tokenizer m_tok;
  tokenizer::iterator m_iter;
  std::string m_str;

public:
  void SetString(const gmdString& to_tokenize,
                 const gmdString& delims = " \t\r\n");

  bool HasMoreTokens() const {
    return m_iter != m_tok.end();
  }

  gmdString GetNextToken() {
    return HasMoreTokens() ? *(m_iter++) : "";
  }

  gmdStringTokenizer() : m_tok(std::string()) {}

  gmdStringTokenizer(const gmdString& to_tokenize,
                     const gmdString& delims = " \t\r\n") : m_tok(std::string())
  {
    SetString(to_tokenize, delims);
  }
};

#endif
