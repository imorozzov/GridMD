/****************************************************************************
 * \file jobmngrbase.cpp
 * Abstract base class for gmShell and gmJobManager
 * Implements error and parameter handling
****************************************************************************/

#include "jobmngr/jobmngr.h"

using namespace std;


pCSTR gmJobManagerBase::UNINITIALIZED_PARAM = "_UNINITIALIZED_";


void gmJobManagerBase::SetParam(pCSTR par_name, pCSTR par_value){
  if( param.find(par_name) == param.end() ) LOGJOBERR(
    fmt("Invalid parameter name '%s' for class %s", par_name, GetClassName()));

  InitParam(par_name, par_value ? par_value : "");
}


void gmJobManagerBase::SetParam(pCSTR par_name, int par_value){
  SetParam(par_name, (pCSTR)gmdString::Format("%d", par_value).c_str());
}


void gmJobManagerBase::SetParam(pCSTR par_name, double par_value){
  SetParam(par_name, (pCSTR)gmdString::Format("%g", par_value).c_str());
}


gmdString gmJobManagerBase::GetParam(pCSTR par_name){
  if( param.find(par_name) == param.end() ) LOGJOBERR(
    fmt("Invalid parameter name '%s' for class %s", par_name, GetClassName()));

  return param[par_name];
}


bool gmJobManagerBase::ParseBoolParam(const gmdString& par_name, const gmdString& par_value){
  if(par_value.Lower() == "true" || par_value == "1") return true;
  else if(par_value.Lower() != "false" && par_value != "0") LOGJOBERR( 
    fmt("Bool parameter '%s' has invalid value of '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()));
  return false;
}


int gmJobManagerBase::ParseIntParam(const gmdString& par_name, const gmdString& par_value){
  long i;
  if( par_value.ToLong(&i) ) return (int)i;
  LOGJOBERR( 
    fmt("Integer parameter '%s' has invalid value of '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()));
  return 0;
}


int gmJobManagerBase::ParseIntParam(const gmdString& par_name, const gmdString& par_value,
                                    int min_val)
{
  long i;
  if( par_value.ToLong(&i) && i >= min_val) return (int)i;
  LOGJOBERR( 
    fmt("Integer parameter '%s' has invalid value of '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()));
  return 0;
}


int gmJobManagerBase::ParseIntParam(const gmdString& par_name, const gmdString& par_value,
                                    int min_val, int max_val)
{
  long i;
  if( par_value.ToLong(&i) && i >= min_val && i <= max_val) return (int)i;
  LOGJOBERR( 
    fmt("Integer parameter '%s' has invalid value of '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()));
  return 0;
}


double gmJobManagerBase::ParseDoubleParam(const gmdString& par_name, const gmdString& par_value){
  double val;
  if( par_value.ToDouble(&val) ) return val;
  LOGJOBERR( 
    fmt("Double parameter '%s' has invalid value of '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()));
  return 0;
}


void gmJobManagerBase::InvalidParValue(const gmdString& par_name, const gmdString& par_value) {
  LOGJOBERR( fmt("Invalid parameter value '%s' = '%s'", (pCSTR)par_name.c_str(), (pCSTR)par_value.c_str()) );
}

void gmJobManagerBase::string_to_line_array(const gmdString& str, gmdArrayString& arr) {
  arr.Clear();
  std::string::size_type i = 0, len = str.Len();
  
  while(i < len) {
    int j = str.find_first_of("\r\n", i);
    if(j == std::string::npos) j = len;
    arr.Add( str.Mid(i, j - i) );
    i = j + 1;
    if(i < len && str[j] == '\r' && str[i] == '\n') i++;
  }
}


gmdString gmJobManagerBase::get_filename_ending(const gmdString& path) {
  int i;
  for(i = path.Len()-1; i>=0; i--)
    if(path[i] == '\\' || path[i] == '/') break;
  return path.Mid(i+1);
}


gmdString gmJobManagerBase::ArrayToString(const gmdArrayString& arr, pCSTR delim){
  gmdString str;
  for(unsigned i=0; i<arr.GetCount(); i++) str += (i>0 ? delim : "") + arr[i];
  return str;
}
