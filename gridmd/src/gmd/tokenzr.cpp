// String tokenizer

#include <ctype.h>

//#include "logexc.h"
#include <gmd/tokenzr.h>


void gmdStringTokenizer::SetString(
  const gmdString& to_tokenize, const gmdString& delims )
{
  m_str = to_tokenize;

  // Check if the string contatins only whitespace characters
  char* p;
  for(p = (char*)delims.c_str(); *p; p++)
    if( !isspace(*p) ) break;

  // If there are only whitespaces drop empty tokens, keep them otherwize
  boost::empty_token_policy emode =
    (*p) ? boost::keep_empty_tokens : boost::drop_empty_tokens;

  if(emode == boost::keep_empty_tokens) {
    // drop last delimiter characters in str
    int last_not_delim = m_str.find_last_not_of(delims.c_str());
    if(last_not_delim != std::string::npos) m_str.resize(last_not_delim + 1);
  }

  m_tok.assign(m_str, boost::char_separator<char>(delims.c_str(), "", emode));
  m_iter = m_tok.begin();
}
