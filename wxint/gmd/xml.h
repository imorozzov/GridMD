// Redefinitions for wxWidgets library classes

#ifndef GRIMD_XML_H
#define GRIMD_XML_H

#include <wx/xml/xml.h>

#define gmdXML_ELEMENT_NODE wxXML_ELEMENT_NODE
#define gmdXML_TEXT_NODE    wxXML_TEXT_NODE

#define gmdXmlNode          wxXmlNode
#define gmdXmlDocument      wxXmlDocument


// reads the property as a double
inline bool GetPropVal(gmdXmlNode *node,const gmdString &name,double *val){
# if wxMAJOR_VERSION<=2 && wxMINOR_VERSION<=8   //  old version
  gmdString sval;
  if(!node->GetPropVal(name,&sval))
    return false;
# else
  gmdString sval=node->GetAttribute(name, "");
  if(sval.IsEmpty())
    return false;
# endif
  return 
    sval.ToDouble(val);
}

// reads the property from a given format
inline bool GetPropVal(gmdXmlNode *node,const gmdString &name,const char *format, void *input){
# if wxMAJOR_VERSION<=2 && wxMINOR_VERSION<=8   //  old version
  gmdString sval;
  if(!node->GetPropVal(name,&sval))
    return false;
# else
  gmdString sval=node->GetAttribute(name, "");
  if(sval.IsEmpty())
    return false;
# endif
  return 
    (sscanf((const char *)sval.c_str(),format,input)==1);
}

// reads the property as a integer or returns a default value
inline int GetPropValInt(gmdXmlNode *node, const gmdString &name, int default_val) {
  long res;
  return node->GetPropVal(name, "").ToLong(&res) ? res : default_val;
}

// reads the property as a double or returns a default valu
inline int GetPropValDbl(gmdXmlNode *node, const gmdString &name, double default_val) {
  double res;
  return node->GetPropVal(name, "").ToDouble(&res) ? res : default_val;
}

#endif
