/****************************************************************************
 * \file jobmngrbase.h
 * Abstract base class for gmShell and gmJobManager
 * Implements error and parameter handling
****************************************************************************/

#ifndef __JOBMANAGERBASE_H
#define __JOBMANAGERBASE_H

#include <map>
#include <gmd/string.h>

#include "logexc.h"
#include "jobmngr/gmshell.h"


// ============================================================================
// Type definitions
// ============================================================================

typedef char* pSTR;
typedef const char* pCSTR;

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 int64;
#else
typedef long long int64;
#endif

typedef std::map<gmdString,gmdString> gmExtraParHash;

inline gmdString get_const_map_el(const gmExtraParHash& strmap, const gmdString& key) {
  gmExtraParHash::const_iterator it = strmap.find(key);
  return it == strmap.end() ? "" : it->second;
}


// ============================================================================
// Useful macros
// ============================================================================

#define FOR_EACH_LOOP(type, arr, var)\
  for(type::iterator var = (arr).begin(); var != (arr).end(); var++)

#define FOR_EACH_LOOP_C(type, arr, var)\
  for(type::const_iterator var = (arr).begin(); var != (arr).end(); var++)


// ============================================================================
// Class to store information about gmJob and gmJobManager exceptions
// ============================================================================

class gmJobException: public std::logic_error{
public:
  gmJobException(const char *str) : std::logic_error(str) {}
};

/// Properties of gmJobException exceptions
template <>
struct log_exception_traits<gmJobException>{
  /// exeption level according to the vbLEVELS: 
    static int level(const gmJobException &signal){ (void)signal; return vblOERR; }
  /// the string name of exception category
  static std::string name(const gmJobException &signal){ (void)signal; return "Job Manager error";}
  /// adds words to what()
  static gmJobException add_words(const gmJobException &orig, const char *words){
    (void)orig;
    return gmJobException(words);
  }
};

/// Macro for error reporting
#define LOGJOBERR(text) message(gmJobException(""), 0, "JOBMNGR: %s at %s:%d", (text), __FILE__, __LINE__)
//#define LOGJOBWARN(text) message(vblWARN, 0, " %s", (text))
//#define LOGJOBMSG(text) message(vblPROGR, 0, " %s", (text))
//#define LOGJOBMSG4(text) message(vblMESS4, 0, " %s", (text))

#define LOGJOBWARN(text) message_str(vblWARN , 0, (text))
#define LOGJOBMSG(text)  message_str(vblPROGR, 0, (text))
#define LOGJOBMSG4(text) message_str(vblMESS4, 0, (text))


// ============================================================================
// Abstract base class for gmShell and gmJobManager
// ============================================================================

class gmJobManagerBase
{
protected:
  gmExtraParHash param;

  // This funtion should be overriden in the descendant classes
  // and is responsible for changing of the internal class variables
  virtual void set_param(const gmdString& par_name, const gmdString& par_value) { (void) par_name; (void) par_value; }

  // Parameter conversion and validation
  static pCSTR UNINITIALIZED_PARAM;  // parameter value to force parameter initialization
  static bool ParseBoolParam(const gmdString& par_name, const gmdString& par_value);
  static int ParseIntParam(const gmdString& par_name, const gmdString& par_value);
  static int ParseIntParam(const gmdString& par_name, const gmdString& par_value,
                           int min_val);
  static int ParseIntParam(const gmdString& par_name, const gmdString& par_value,
                           int min_val, int max_val);
  static double ParseDoubleParam(const gmdString& par_name, const gmdString& par_value);
  void InvalidParValue(const gmdString& par_name, const gmdString& par_value);

  // Create and initialize a new parameter
  void InitParam(pCSTR par_name, pCSTR par_value){
    param[par_name] = par_value;
    set_param(par_name, par_value);
  }

  // Auxilary fumctions
  static void string_to_line_array(const gmdString& str, gmdArrayString& arr);

  static gmdString get_filename_ending(const gmdString& path);

  static bool has_wildcards(const gmdString& path) {
    return path.Find('*') != gmdNOT_FOUND || path.Find('?') != gmdNOT_FOUND;
  }

  static int not_implemented() {
    LOGJOBERR("The function is not implemented!");
    return -1;
  }

public:
  // Returns the name of the current class
  virtual pCSTR GetClassName() = 0;

  // Set the specified parameter value
  void SetParam(pCSTR par_name, pCSTR par_value);
  void SetParam(pCSTR par_name, int par_value);
  void SetParam(pCSTR par_name, double par_value);

  // Get the specified parameter value
  gmdString GetParam(pCSTR par_name);
  
  bool GetBoolParam(pCSTR par_name) {
    return ParseBoolParam(par_name, GetParam(par_name));
  }
  
  int GetIntParam(pCSTR par_name) {
    return ParseIntParam(par_name, GetParam(par_name));
  }
  
  double GetDoubleParam(pCSTR par_name) {
    return ParseDoubleParam(par_name, GetParam(par_name));
  }

  /// Convert gmdArrayString object into a single gmdString object where the strings
  /// are delimited by the 'delim'
  static gmdString ArrayToString(const gmdArrayString& arr, pCSTR delim="\n");
};

#endif
