/*****************************************************************************
 *
 *   Copyright (c), Kinetic Technologies Ltd. 2003        All Rights Reserved.
 *
 *   Author	: Denis Shirabaikin, KINTECH, Moscow, Russia
 *
 *   Project	: KHIMERA
 *
 *   $Revision: 1.1 $
 *   $Date: 2013/05/24 17:53:14 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/src/gmd/xml/xml_file.cpp,v 1.1 2013/05/24 17:53:14 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/src/gmd/xml/xml_file.cpp,v $
$Revision: 1.1 $
$Author: valuev $
$Date: 2013/05/24 17:53:14 $
*/
/******************************************************************************
 * $Log: xml_file.cpp,v $
 * Revision 1.1  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.25  2009/10/09 15:28:47  wasp
 * In function addElement node name check was added.
 *
 * Revision 1.24  2009/05/22 09:32:18  wasp
 * Parsing of nullable double attribute was implemented.
 *
 * Revision 1.23  2008/06/07 14:04:12  chorkov
 * added specials. exception for the error XML.
 *
 * Revision 1.22  2008/06/03 12:45:13  wasp
 * Function getRootNode() was added.
 *
 * Revision 1.21  2008/04/30 14:13:16  wasp
 * Functions hasAttribute were added. These functions will replace corresponding isAttribute functions.
 *
 * Revision 1.20  2007/12/03 16:13:18  chorkov
 * *** empty log message ***
 *
 * Revision 1.19  2007/11/02 11:37:06  wasp
 * Bug in converBool(string) was fixed. Function createDoc was modified.
 *
 * Revision 1.18  2007/10/31 15:06:11  wasp
 * Functions for XML file creation and manipulation were implemented.
 *
 * Revision 1.17  2007/10/30 14:48:13  wasp
 * Dumping of xml tree was implemented. Removing node was added.
 *
 * Revision 1.16  2007/10/26 09:48:42  wasp
 * Constructor for file contents attaching was fixed.
 *
 * Revision 1.15  2007/04/24 10:24:44  wasp
 * Adopted to the new libxml.
 *
 * Revision 1.14  2007/01/29 13:44:37  chorkov
 * *** empty log message ***
 *
 * Revision 1.13  2006/10/30 16:41:53  wasp
 * In filenames: std::string -> QString. Second step.
 *
 * Revision 1.12  2006/10/20 12:49:21  wasp
 * Parsing from memory was added.
 *
 * Revision 1.11  2006/10/18 13:52:24  wasp
 * Attaching memory content was implemented.
 *
 * Revision 1.10  2006/09/20 15:32:09  wasp
 * Parsing uint values was implemented.
 *
 * Revision 1.9  2005/05/03 07:41:05  wasp
 * The parseBoolAttribute function was added.
 *
 * Revision 1.8  2005/01/11 10:28:20  wasp
 * This revision corresponds to the New Year delivery.
 *
 * Revision 1.7  2004/10/08 14:13:48  wasp
 * New functions for parsing data from xmlNodePtr were added.
 * Typed XML file class was removed.
 *
 * Revision 1.6  2004/07/05 13:39:47  wasp
 * Headers order was changed to fix the problem with MSVC.
 *
 * Revision 1.5  2004/06/29 15:07:42  wasp
 * delete was replaced by xmlFree.
 *
 * Revision 1.4  2004/06/16 14:00:32  chorkov
 * Add isAttribute method.
 * Add method clear to errase.
 *
 * Revision 1.3  2004/04/19 10:08:29  wasp
 * Additional functions for processing XmlNodePtr were implemented.
 *
 * Revision 1.2  2004/03/09 11:45:52  wasp
 * This revision corresponds to 8th March day.
 *
 * Revision 1.1.1.1  2003/11/03 14:05:25  wasp
 * Initial revision
 *
 *
 *
 * 
*******************************************************************************/

#ifndef __MAKEDEPEND__
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xinclude.h>

#include <iostream>
#include <sstream>
//#include <strstream>
#include <list>
#include <cstring>
#endif

#include "xml_file.h"

using namespace std;


//------------------------------------------Converters----------------------------------------

string XMLFile::convertFloat(double val)
{
	string					res;
	ostringstream			to;

	to.precision(16);
	to.setf(ios::scientific, ios::floatfield);
	to << val;
	if(!to) throw xml_error("Bad value for conversion!", this);
	res = to.str();

#ifdef DEBUG_XML
	cerr << "XMLFile::convertFloat: value is " << res << "\n";
#endif
	return res;
}

string XMLFile::convertInt(int val)
{
	string					res;
	ostringstream			to;

	to << val;
	if(!to) throw xml_error("Bad value for conversion!", this);
	res = to.str();

#ifdef DEBUG_XML
	cerr << "XMLFile::convertFloat: value is " << res << "\n";
#endif
	return res;
}

string XMLFile::convertUInt(unsigned int val)
{
	string					res;
	ostringstream			to;

	to << val;
	if(!to) throw xml_error("Bad value for conversion!", this);
	res = to.str();

#ifdef DEBUG_XML
	cerr << "XMLFile::convertFloat: value is " << res << "\n";
#endif
	return res;
}

string XMLFile::convertBool(bool val)
{
	string					res;
	ostringstream			to;

	boolalpha(to);
	to << val;
	if(!to) throw xml_error("Bad value for conversion!", this);
	res = to.str();

#ifdef DEBUG_XML
	cerr << "XMLFile::convertFloat: value is " << res << "\n";
#endif
    return res;
}


double XMLFile::convertFloat(string val)
{
	double					res;
	istringstream			from(val);
	from >> res;
	if(!from) throw xml_error("Bad value for conversion!", this);

#ifdef DEBUG_XML
	cerr << "XMLFile::convertFloat: value is " << res << "\n";
#endif
	return res;
}

int XMLFile::convertInt(string val)
{
	int					res;
	istringstream		from(val);
	from >> res;
	if(!from) throw xml_error("Bad value for conversion!", this);

#ifdef DEBUG_XML
	cerr << "XMLFile::convertInt: value is " << res << "\n";
#endif
	return res;
}

unsigned int XMLFile::convertUInt(string val)
{
	unsigned int		res;
	istringstream		from(val);
	from >> res;
	if(!from) throw xml_error("Bad value for conversion!", this);

#ifdef DEBUG_XML
	cerr << "XMLFile::convertUInt: value is " << res << "\n";
#endif
	return res;
}

bool XMLFile::convertBool(string val)
{
	bool					res;
	string				inp;
	istringstream		from(val);
	
	//Trim whitespaces!
	from >> inp;
	if(!from) throw xml_error("Bad value for conversion!", this);

	
	if(inp == "1" || inp == "true") {
		res = true;
	} else {
		if(inp == "0" || inp == "false") {
			res = false;
		} else {
			throw xml_error("Bad value for conversion!", this);
		}
	};

#ifdef DEBUG_XML
	cerr << "XMLFile::convertBool: value is " << res << "\n";
#endif
	return res;
}

double XMLFile::convertFloat(const char* val)
{
	return convertFloat(string(val));
}

int XMLFile::convertInt(const char* val)
{
	return convertInt(string(val));
}

unsigned int XMLFile::convertUInt(const char* val)
{
	return convertUInt(string(val));
}

bool XMLFile::convertBool(const char* val)
{
    return convertBool(string(val));
}

string XMLFile::convertStr(string val)
{
    return val;
}

//-------------------URL Access----------------------------------------------------------

const string &XMLFile::getFileName() const
{
    return fname;
}

void XMLFile::setFileName(const string &fname_)
{
    fname=fname_;
    if(doc)
    {
        if(doc->URL) xmlFree((void*) doc->URL);
        doc->URL=xmlStrdup((const xmlChar*) fname_.c_str() );
    }
}



//-------------------Creating/Modifying----------------------------------------------------------
	/*!Creates empty document. Current document will be deleted.*/
xmlNodePtr XMLFile::createDoc(string rootName)
{
	xmlNodePtr			rootNode = 0;


	xmlLineNumbersDefault(1);
	if(doc) {
		xmlFreeDoc(doc);
		doc = 0;
	};
	XMLFile::fname = "";

	doc = xmlNewDoc((xmlChar*) "1.0");
	if(!doc) throw xml_error("Can't create XML document!", this);

	rootNode = xmlNewNode(NULL, (xmlChar*) rootName.c_str());
	if(!rootNode) throw xml_error("Can't create root node!", this);
	
	xmlDocSetRootElement(doc, rootNode);

	return rootNode;
}

xmlNodePtr XMLFile::addElement(xmlNodePtr parent, string name, string content)
{
	if(name.empty()) throw xml_error("Name of created node is empty!", this);
	return xmlNewTextChild(parent, NULL, (xmlChar*) name.c_str(),
		content != "" ? (xmlChar*) content.c_str() : NULL);
}

xmlNodePtr XMLFile::addElement(string parentPath, string name, string content)
{
	if(isNode(parentPath))
		return addElement(getNodeByPath(parentPath), name, content);
	else
		return NULL;
}

xmlNodePtr XMLFile::addText(xmlNodePtr parent, string content)
{
    return xmlAddChild( parent, xmlNewText( (xmlChar*) content.c_str() ));
}

xmlNodePtr XMLFile::addText(string parentPath, string content)
{
    if(isNode(parentPath))
        return addText(getNodeByPath(parentPath), content);
    else
        return NULL;
}


void XMLFile::setContent(xmlNodePtr node, const char* content)
{
	xmlNodeSetContent(node, (xmlChar*) content);
}

void XMLFile::setContent(xmlNodePtr node, string content)
{
	setContent(node, content.c_str());
}

void XMLFile::setContent(xmlNodePtr node, double content)
{
	setContent(node, convertFloat(content));
}

void XMLFile::setContent(xmlNodePtr node, int content)
{
	setContent(node, convertInt(content));
}

void XMLFile::setContent(xmlNodePtr node, bool content)
{
	setContent(node, convertBool(content));
}

void XMLFile::setContent(string path, const char* content)
{
	if(isNode(path))
		setContent(getNodeByPath(path), content);
}

void XMLFile::setContent(string path, string content)
{
	setContent(path, content.c_str());
}

void XMLFile::setContent(string path, double content)
{
	setContent(path, convertFloat(content));
}

void XMLFile::setContent(string path, int content)
{
	setContent(path, convertInt(content));
}

void XMLFile::setContent(string path, bool content)
{
	setContent(path, convertBool(content));
}

void XMLFile::addOptionalAttribute(xmlNodePtr node, string name, const char* content)
{
	if(strlen(content) > 0)
    xmlNewProp(node, (xmlChar*) name.c_str(), (xmlChar*) content);
}

void XMLFile::addOptionalAttribute(xmlNodePtr node, string name, string content)
{
	addOptionalAttribute(node, name, content.c_str());
}

void XMLFile::addAttribute(xmlNodePtr node, string name, const char* content)
{
    xmlNewProp(node, (xmlChar*) name.c_str(), (xmlChar*) content);
}

void XMLFile::addAttribute(xmlNodePtr node, string name, string content)
{
	addAttribute(node, name, content.c_str());
}

void XMLFile::addAttribute(xmlNodePtr node, string name, double content)
{
	addAttribute(node, name, convertFloat(content));
}

void XMLFile::addAttribute(xmlNodePtr node, string name, int content)
{
	addAttribute(node, name, convertInt(content));
}

void XMLFile::addAttribute(xmlNodePtr node, string name, bool content)
{
	addAttribute(node, name, convertBool(content));
}

void XMLFile::addAttribute(string path, string name, const char* content)
{
	if(isNode(path))
		addAttribute(getNodeByPath(path), name, content);
}

void XMLFile::addAttribute(string path, string name, string content)
{
	addAttribute(path, name, content.c_str());
}

void XMLFile::addAttribute(string path, string name, double content)
{
	addAttribute(path, name, convertFloat(content));
}

void XMLFile::addAttribute(string path, string name, int content)
{
	addAttribute(path, name, convertInt(content));
}

void XMLFile::addAttribute(string path, string name, bool content)
{
	addAttribute(path, name, convertBool(content));
}

void XMLFile::setAttribute(xmlNodePtr node, string name, const char* content)
{
	xmlSetProp(node, (xmlChar*) name.c_str(), (xmlChar*) content);
}

void XMLFile::setAttribute(xmlNodePtr node, string name, string content)
{
	setAttribute(node, name, content.c_str());
}

void XMLFile::setAttribute(xmlNodePtr node, string name, double content)
{
	setAttribute(node, name, convertFloat(content));
}

void XMLFile::setAttribute(xmlNodePtr node, string name, int content)
{
	setAttribute(node, name, convertInt(content));
}

void XMLFile::setAttribute(xmlNodePtr node, string name, bool content)
{
	setAttribute(node, name, convertBool(content));
}



//------------------------------------------Node utilities----------------------------------------
xmlNodePtr XMLFile::getRootNode(void)
{
	return xmlDocGetRootElement(doc);
}

xmlNodePtr XMLFile::getNodeByPath(const char* path, bool throwOnNotFound)
{
    return getNodeByPath(string(path), throwOnNotFound);
}

xmlNodePtr XMLFile::getNodeByPath(string path, bool throwOnNotFound)
{
    return getNodeByRelativePath(path, xmlDocGetRootElement(doc), throwOnNotFound);
}

xmlNodePtr XMLFile::getNodeByRelativePath(const char*  path, xmlNodePtr current_node, bool throwOnNotFound)
{
    return getNodeByRelativePath(std::string(path), current_node, throwOnNotFound);
}

xmlNodePtr XMLFile::getNodeByRelativePath(string path, xmlNodePtr current_node, bool throwOnNotFound)
{
    xmlNodePtr					res;
    xmlXPathObjectPtr			xpath;
    xmlXPathContextPtr		ctxt;

#ifdef DEBUG_XML
    cerr << "XMLFile::getNodeByPath: looking for " << path.c_str() << " ...\n";
#endif

    if(!doc) throw xml_error("Document not found!",this, current_node, path);

    ctxt = xmlXPathNewContext(doc);
    ctxt->node = current_node;
    xpath = xmlXPathEvalExpression((const xmlChar *)path.c_str(), ctxt);
    if(!xpath) {
        xmlXPathFreeContext(ctxt);
        throw xml_error("Bad path! (Cann't evaluate XPath. Wrong syntax.)",this, current_node, path);
    };
    if(!xpath->nodesetval) {
        xmlXPathFreeObject(xpath);
        xmlXPathFreeContext(ctxt);
        if(throwOnNotFound)
            throw xml_error("Bad path! (Nothing to return.)",this, current_node, path);
        return NULL;
    };

    if(!xpath->nodesetval->nodeTab) {
        xmlXPathFreeObject(xpath);
        xmlXPathFreeContext(ctxt);
        if(throwOnNotFound)
            throw xml_error("Bad path! (Nothing to return.)",this, current_node, path);
        return NULL;
    };

    res = xpath->nodesetval->nodeTab[0];
    xmlXPathFreeObject(xpath);
    xmlXPathFreeContext(ctxt);
    if( !res && throwOnNotFound ) throw xml_error("Bad path! (Nothing to return.)",this, current_node, path);

    return res;
};

vector<xmlNodePtr> XMLFile::getNodesByPath(const char* path)
{
	return getNodesByPath(string(path));
}

vector<xmlNodePtr> XMLFile::getNodesByPath(string path)
{
	return getNodesByRelativePath(path, xmlDocGetRootElement(doc));
}

vector<xmlNodePtr> XMLFile::getNodesByRelativePath(const char*  path, xmlNodePtr current_node)
{
	return getNodesByRelativePath(std::string(path), current_node);
}

vector<xmlNodePtr> XMLFile::getNodesByRelativePath(string path, xmlNodePtr current_node)
{
	vector<xmlNodePtr>		res;
	xmlXPathObjectPtr			xpath;
	xmlXPathContextPtr		ctxt;

#ifdef DEBUG_XML
	cerr << "XMLFile::getNodeByPath: looking for " << path.c_str() << " ...\n";
#endif

	if(!doc) throw xml_error("Document not found!",this, current_node, path);

	ctxt = xmlXPathNewContext(doc);
	ctxt->node = current_node;
	xpath = xmlXPathEvalExpression((const xmlChar *)path.c_str(), ctxt);
	if(!xpath) {
		xmlXPathFreeContext(ctxt);
		//return res;
		throw xml_error("Bad path! (Cann't evaluate XPath. Wrong syntax.)",this, current_node, path);
	};
	if(!xpath->nodesetval) {
		xmlXPathFreeObject(xpath);
		xmlXPathFreeContext(ctxt);
		return res;
		//throw invalid_argument("Bad path!");
	};
	
	res.resize(xpath->nodesetval->nodeNr);
	for(int i = 0; i < xpath->nodesetval->nodeNr; i++)
		res[i] = xpath->nodesetval->nodeTab[i];
	xmlXPathFreeObject(xpath);
	xmlXPathFreeContext(ctxt);
	//if(res.size() == 0) throw invalid_argument("Bad path!");

	return res;
}

string XMLFile::getNodePath(xmlNodePtr node)
{
	xmlChar*			retStr = 0;
	string			res;
	//xmlNodePtr		cur;

	if(!node) return res;
	retStr = xmlGetNodePath(node);
	if(!retStr) throw xml_error("Bad node!",this, node);
	res = string((char*)retStr);
	xmlFree(retStr);
#ifdef DEBUG_XML
	cerr << "XMLFile::getNodePath: path is \"" << res.c_str() << "\"\n";
#endif
	return res;
}

xmlNodePtr XMLFile::getChildByName(xmlNodePtr parent, string name,  bool throwOnNotFound)
{
	xmlNodePtr				cur;
	
	cur = parent->xmlChildrenNode;
	while(cur != NULL) {
		if(getNodeName(cur) == name) return cur;
		cur = cur->next;
	};
	
    if(throwOnNotFound)
        throw xml_error( "Child node not found." , this, parent, name );

	return NULL;
}

xmlNodePtr XMLFile::getChildByName(xmlNodePtr parent, const char* name, bool throwOnNotFound)
{
    return getChildByName(parent, string(name), throwOnNotFound);
}

vector<xmlNodePtr> XMLFile::getChildsByName(xmlNodePtr parent, string name)
{
	xmlNodePtr					cur;
	vector<xmlNodePtr>		ret;
	
	ret.clear();
	cur = parent->xmlChildrenNode;
	while(cur != NULL) {
		if(getNodeName(cur) == name) ret.push_back(cur);
		cur = cur->next;
	};
	
	return ret;
}

vector<xmlNodePtr> XMLFile::getChildsByName(xmlNodePtr parent, const char* name)
{
	return getChildsByName(parent, string(name));
}

vector<xmlNodePtr> XMLFile::getAllChildren(xmlNodePtr parent)
{
	xmlNodePtr					cur;
	vector<xmlNodePtr>		ret;
	
	ret.clear();
	cur = parent->xmlChildrenNode;
	while(cur != NULL) {
		ret.push_back(cur);
		cur = cur->next;
	};
	
	return ret;
}

bool XMLFile::isNode(xmlNodePtr parent, string name)
{
	return (getChildByName(parent, name) != NULL);
}

bool XMLFile::isNode(xmlNodePtr parent, const char* name)
{
	return isNode(parent, string(name));
}

bool XMLFile::isNode(string path)
{
//	xmlNodePtr					res;
	xmlXPathObjectPtr			xpath;
	xmlXPathContextPtr		ctxt;

#ifdef DEBUG_XML
	cerr << "XMLFile::isNode: checking node " << path.c_str() << " ...\n";
#endif

	if(!doc) 
		throw xml_error("Document not found!",this, NULL, path);
		//throw invalid_argument("Document not found!");

	ctxt = xmlXPathNewContext(doc);
	ctxt->node = xmlDocGetRootElement(doc);
	xpath = xmlXPathEvalExpression((const xmlChar *)path.c_str(), ctxt);
	if(!xpath) {
		xmlXPathFreeContext(ctxt);
		return false;
	};
	if(!xpath->nodesetval) {
		xmlXPathFreeObject(xpath);
		xmlXPathFreeContext(ctxt);
		return false;
	};
	//if(!xpath->nodesetval->nodeTab) {
	if(!xpath->nodesetval->nodeNr) {
		xmlXPathFreeObject(xpath);
		xmlXPathFreeContext(ctxt);
		return false;
	};
	xmlXPathFreeObject(xpath);
	xmlXPathFreeContext(ctxt);
	return true;
};

bool  XMLFile::isNode(const char* path)
{
	return isNode(string(path));
};

bool  XMLFile::isAttribute(string path)
{
	xmlXPathObjectPtr			xpath;
	xmlXPathContextPtr		ctxt;
	bool							ret;

#ifdef DEBUG_XML
	cerr << "XMLFile::isAttribute: checking attribute " << path.c_str() << " ...\n";
#endif

	if(!doc) throw invalid_argument("Document not found!");

	ctxt = xmlXPathNewContext(doc);
	ctxt->node = xmlDocGetRootElement(doc);
	xpath = xmlXPathEvalExpression((const xmlChar *)path.c_str(), ctxt);
	if(!xpath) {
		xmlXPathFreeContext(ctxt);
		return false;
	};
	
	ret = (xpath->nodesetval) && (xpath->nodesetval->nodeTab)
				&& (xpath->nodesetval->nodeTab[0]->type==XML_ATTRIBUTE_NODE);

	xmlXPathFreeObject(xpath);
	xmlXPathFreeContext(ctxt);
	return ret;
}

bool  XMLFile::isAttribute(const char* path)
{
	return isAttribute(string(path));
}

bool  XMLFile::hasAttribute(xmlNodePtr node, string name)
{
#ifdef DEBUG_XML
	cerr << "XMLFile::hasAttribute: checking attribute " << name << " ...\n";
#endif
	if(!node) 
		throw xml_error("Parent node not set!",this, node, name);
		//throw invalid_argument("Parent node not set!"); 

	xmlChar*res= xmlGetProp(node,(const xmlChar*) name.c_str());
	if(res) {
		xmlFree(res);
		return true;
	} else return false;
}

bool  XMLFile::hasAttribute(xmlNodePtr node, const char* name)
{
	return hasAttribute(node, string(name));
}

bool XMLFile::parseBoolAttribute(string root, string name)
{
	return parseBoolAttribute(getNodeByPath(root), string(name));
}

bool XMLFile::parseBoolAttribute(const char* root, const char* name)
{
	return parseBoolAttribute(string(root), string(name));
}

bool XMLFile::parseBoolAttribute(xmlNodePtr node, string name)
{
	xmlChar*			retStr;
	bool				res;

	retStr = xmlGetProp(node, (const xmlChar*)name.c_str());
	if(!retStr) 
		throw xml_error("Bad node!",this, node, name);
		
	res = convertBool((const char*)retStr);
	xmlFree(retStr);

#ifdef DEBUG_XML
	cerr << "XMLFile::parseBoolAttribute: value is \"" << res << "\"\n";
#endif
	return res;
}

int XMLFile::parseIntAttribute(string root, string name)
{
	return parseIntAttribute(getNodeByPath(root), string(name));
}

int XMLFile::parseIntAttribute(const char* root, const char* name)
{
	return parseIntAttribute(string(root), string(name));
}

int XMLFile::parseIntAttribute(xmlNodePtr node, string name)
{
	xmlChar*			retStr;
	int				res;

	retStr = xmlGetProp(node, (const xmlChar*)name.c_str());
	if(!retStr) 
		throw xml_error("Bad node!",this, node, name);
		
	res = convertInt((const char*)retStr);
	xmlFree(retStr);

#ifdef DEBUG_XML
	cerr << "XMLFile::parseIntAttribute: value is \"" << res << "\"\n";
#endif
	return res;
}

double XMLFile::parseFloatAttribute(string root, string name)
{
	return parseFloatAttribute(getNodeByPath(root), string(name));
};

double XMLFile::parseFloatAttribute(const char* root, const char* name)
{
	return parseFloatAttribute(string(root), string(name));
};

double XMLFile::parseFloatAttribute(xmlNodePtr node, string name)
{
	xmlChar*			retStr;
	double			res;

	retStr = xmlGetProp(node, (const xmlChar*)name.c_str());
	if(!retStr) 
		throw xml_error("Bad node!",this, node, name);
		
	res = convertFloat((const char*)retStr);
	xmlFree(retStr);

#ifdef DEBUG_XML
	cerr << "XMLFile::parseFloatAttribute: value is \"" << res << "\"\n";
#endif
	return res;
}

string XMLFile::parseAttribute(string root, string name)
{
	xmlNodePtr		node;
	xmlChar*			retStr;
	string			res;

	node = getNodeByPath(root);
	retStr = xmlGetProp(node, (const xmlChar*)name.c_str());
	if(!retStr) 
		throw xml_error("Bad node!",this, node, name);
	res = string((const char*)retStr);
	xmlFree(retStr);

#ifdef DEBUG_XML
	cerr << "XMLFile::parseAttribute: string is \"" << res.c_str() << "\"\n";
#endif
	return res;
};

string XMLFile::parseAttribute(const char* root, const char* name)
{
	return parseAttribute(string(root), string(name));
};

string XMLFile::parseAttribute(xmlNodePtr node, string name)
{
	xmlChar*			retStr;
	string			res;

	retStr = xmlGetProp(node, (const xmlChar*)name.c_str());
	if(!retStr) throw xml_error("Bad node!",this, node, name);
	res = string((const char*)retStr);
	xmlFree(retStr);

#ifdef DEBUG_XML
	cerr << "XMLFile::parseAttribute: string is \"" << res.c_str() << "\"\n";
#endif
	return res;
}

string XMLFile::parseAttribute(xmlNodePtr node, const char* name)
{
	return parseAttribute(node, string(name));
}

namespace {
    template <typename T>
    T parseOptionalAttributeImpl(
            XMLFile&   parser,
            xmlNodePtr node,
            string     name,
            T          defaultValue,
            bool       inheridFromParent,
            T          (XMLFile::*getter)(xmlNodePtr node, string name))
    {
        if(parser.hasAttribute(node, name))
            return (parser.*getter)(node, name);
        else if(inheridFromParent && node->parent)
            return parser.parseOptionalAttribute(node->parent, name, defaultValue, inheridFromParent);
        else
            return defaultValue;
    }


}

bool   XMLFile::parseOptionalAttribute(xmlNodePtr node, string name, bool     defaultValue, bool inheridFromParent)
{
    return parseOptionalAttributeImpl( *this, node, name, defaultValue, inheridFromParent, & XMLFile::parseBoolAttribute );
}

int    XMLFile::parseOptionalAttribute(xmlNodePtr node, string name, int      defaultValue, bool inheridFromParent)
{
    return parseOptionalAttributeImpl( *this, node, name, defaultValue, inheridFromParent, & XMLFile::parseIntAttribute );
}

double XMLFile::parseOptionalAttribute(xmlNodePtr node, string name, double   defaultValue, bool inheridFromParent)
{
    return parseOptionalAttributeImpl( *this, node, name, defaultValue, inheridFromParent, & XMLFile::parseFloatAttribute );
}

string XMLFile::parseOptionalAttribute(xmlNodePtr node, string name, string   defaultValue, bool inheridFromParent)
{
    return parseOptionalAttributeImpl( *this, node, name, defaultValue, inheridFromParent, & XMLFile::parseAttribute );
}
string XMLFile::parseOptionalAttribute(xmlNodePtr node, string name, const char* defaultValue, bool inheridFromParent)
{
    return parseOptionalAttribute(node, name, std::string(defaultValue), inheridFromParent);
}

namespace {
    template <typename T>
    T parseOptionalImpl(
            XMLFile&        parser      ,
            const string&   xpath       ,
            T               defaultValue,
            T  (XMLFile::*  convert     )(std::string content)  )
    {
        if(parser.isNode(xpath))
            return (parser.*convert)( parser.parseNode(  parser.getNodeByPath(xpath) ) );
        else
            return defaultValue;
    }

    template <typename T>
    T parseOptionalImpl(
            XMLFile&        parser      ,
            xmlNodePtr      parent      ,
            const string&   xpath       ,
            T               defaultValue,
            T  (XMLFile::*  convert     )(std::string content)  )
    {

        xmlNodePtr node=parser.getNodeByRelativePath(xpath, parent, false );
        if(node)
            return (parser.*convert)( parser.parseNode(  node ) );
        else
            return defaultValue;
    }
}

bool XMLFile::parseOptional(string xpath, bool defaultValue)
{
    return parseOptionalImpl( *this, xpath, defaultValue , & XMLFile::convertBool );
}

int XMLFile::parseOptional(string xpath, int defaultValue)
{
    return parseOptionalImpl( *this, xpath, defaultValue , & XMLFile::convertInt );
}

double XMLFile::parseOptional(string xpath, double defaultValue)
{
    return parseOptionalImpl( *this, xpath, defaultValue , & XMLFile::convertFloat );
}

string XMLFile::parseOptional(string xpath, string defaultValue)
{
    return parseOptionalImpl( *this, xpath, defaultValue , & XMLFile::convertStr );
}

string XMLFile::parseOptional(string xpath, const char *defaultValue)
{
    return parseOptionalImpl( *this, xpath, (std::string)defaultValue , & XMLFile::convertStr );
}

bool XMLFile::parseOptional(xmlNodePtr parent, string xpath, bool defaultValue)
{
    return parseOptionalImpl(*this, parent, xpath, defaultValue, & XMLFile::convertBool );
}

int XMLFile::parseOptional(xmlNodePtr parent, string xpath, int defaultValue)
{
    return parseOptionalImpl(*this, parent, xpath, defaultValue, & XMLFile::convertInt );
}

double XMLFile::parseOptional(xmlNodePtr parent, string xpath, double defaultValue)
{
    return parseOptionalImpl( *this, parent, xpath, defaultValue , & XMLFile::convertFloat );
}

string XMLFile::parseOptional(xmlNodePtr parent, string xpath, string defaultValue)
{
    return parseOptionalImpl( *this, parent, xpath, defaultValue , & XMLFile::convertStr );
}

string XMLFile::parseOptional(xmlNodePtr parent, string xpath, const char *defaultValue)
{
    return parseOptionalImpl( *this, parent, xpath, (std::string)defaultValue , & XMLFile::convertStr );
}


string XMLFile::parseNode(xmlNodePtr node)
{
	xmlChar*			retStr;
	string			res;

	retStr = xmlNodeGetContent(node);
	if(!retStr) throw xml_error("Bad node!",this, node);
	res = string((char*)retStr);
	xmlFree(retStr);
#ifdef DEBUG_XML
	cerr << "XMLFile::parseNode: string is \"" << res.c_str() << "\"\n";
#endif
	return res;
}

string XMLFile::getNodeName(xmlNodePtr node)
{
	xmlChar*			retStr;
	string			res;

	if(!node) return res;
	retStr = (xmlChar*)node->name;
	if(!retStr) throw xml_error("Bad node!",this, node);
	res = string((char*)retStr);
#ifdef DEBUG_XML
	cerr << "XMLFile::getNodeName: name is \"" << res.c_str() << "\"\n";
#endif
	return res;
}

string XMLFile::parseNodeTree(xmlNodePtr node)
{
	string					res;
	xmlBufferPtr			buf;
	int						ret;
	
	buf = xmlBufferCreate();
	ret = xmlNodeDump(buf, node->doc, node, 0, 0);
	if(ret == -1) throw xml_error("Bad node!",this, node);
	res = string((char*)xmlBufferContent(buf));
	xmlBufferFree(buf);
#ifdef DEBUG_XML
	cerr << "XMLFile::parseNodeTree: string is \"" << res.c_str() << "\"\n";
#endif
	return res;
}

void XMLFile::removeNode(xmlNodePtr node)
{
	xmlUnlinkNode(node);
	xmlFreeNode(node);
}

void XMLFile::removeNode(string path)
{
	if(isNode(path))
		removeNode(getNodeByPath(path));
}

void XMLFile::removeAttribute(xmlNodePtr node, string name)
{
	xmlAttrPtr appr=xmlHasProp( node, (const xmlChar*) name.c_str() );
	if(appr)
		xmlRemoveProp(appr);
}

void XMLFile::removeAttribute(string     path, string name)
{
	removeAttribute( getNodeByPath(path), name );
}


void XMLFile::attachMemory(const char *buffer, int size)
{
	xmlLineNumbersDefault(1);
	if(doc) {
		xmlFreeDoc(doc);
		doc = 0;
	};
	XMLFile::fname = "";
	doc = xmlParseMemory(buffer, size);
	if(!doc) throw xml_error("Can't parse XML file!");
}

void XMLFile::attachMemory(string buffer)
{
	attachMemory(buffer.c_str(), buffer.size());
}

void XMLFile::attach(string fname)
{
	xmlLineNumbersDefault(1);
	if(doc) {
		xmlFreeDoc(doc);
		doc = 0;
	};
	XMLFile::fname = fname;
	doc = xmlParseFile(fname.c_str());
	if(!doc) throw xml_error("Wrong XML file name!");
}

void XMLFile::attach(char* fname)
{
    attach(string(fname));
}

void XMLFile::detach(void)
{
	if(doc) xmlFreeDoc(doc);
    fname.erase();
    doc = 0;
}



void XMLFile::dumpToMemory(string& s)
{
	xmlChar		*xmlBuffer = 0;
	int			size;

	if(!doc) throw xml_error("Empty document!");
	
	//xmlDocDumpMemory(doc, &xmlBuffer, &size);

	xmlKeepBlanksDefault(0);
	xmlDocDumpFormatMemory(doc, &xmlBuffer, &size, 1);

	if(xmlBuffer) {
		s = string((char*)(xmlBuffer));
		xmlFree(xmlBuffer);
	} else {
		s = "";
	};
}

void XMLFile::dumpNodeToMemory(string& s, xmlNodePtr node)
{
	if(!doc)  throw xml_error("Empty document!");
	if(!node) throw xml_error("Empty node!");

	xmlBufferPtr buffer =xmlBufferCreate();

	xmlKeepBlanksDefault(0);
	xmlNodeDump(buffer, doc, node, 0, 1);

	const char* content=(const char*) xmlBufferContent(buffer);
	std::string( content, content+ xmlBufferLength(buffer) ).swap(s);

	xmlBufferFree(buffer);
}

void XMLFile::dumpNodeToMemory(string& s, string     path)
{
	if(isNode(path))
        dumpNodeToMemory( s, getNodeByPath(path));
}

void XMLFile::dumpToFile(const string &utf8_file_name)
{
    int ret=xmlSaveFile( utf8_file_name.c_str(), doc );
    if(ret<0)
        throw xml_error("Fail to write xml file.", this);
}

int XMLFile::processXinclude(int iterations_limit)
{
    if(!doc)
        return 0;

    int summ=0;
    for( int i=0; i!=iterations_limit; ++i )
    {
        const int ret=xmlXIncludeProcess(doc);
        if(ret==0)
            break; // no more to substitute...;
        if(ret==-1)
            throw xml_error( "Error at XInclude substitution" , this);

        summ +=ret;
    }
    return summ; // no more to substitute...;
}

int XMLFile::processXinclude(xmlNodePtr node, int iterations_limit)
{
    if(!node)
        return 0;

    int summ=0;
    for( int i=0; i!=iterations_limit; ++i )
    {
        const int ret=xmlXIncludeProcessTree(node);
        if(ret==0)
            break; // no more to substitute...;
        if(ret==-1)
            throw xml_error( "Error at XInclude substitution" , this);

        summ +=ret;
    }
    return summ; // no more to substitute...;
}


XMLFile::XMLFile(void)
{
   fname.erase();
	doc = 0;
}

XMLFile::XMLFile(string fname)
{
	doc = 0;
	attach(fname);
}

XMLFile::XMLFile(char* fname)
{
	doc = 0;
	attach(string(fname));
}

XMLFile::XMLFile(const char *buffer, int size)
{
	doc = 0;
    attachMemory(buffer, size);
}

XMLFile::XMLFile(xmlDocPtr doc)
    : doc(doc)
{}

XMLFile::~XMLFile(void)
{
	if(doc) xmlFreeDoc(doc);
}


static std::string xml_error_what(const std::string& brife_wath_,  XMLFile*xmlFile_, xmlNodePtr node_, std::string xpath_)
{
	
	std::string ret=brife_wath_;

	try
	{
		if(xmlFile_!=NULL)
		{
			try
			{
				ret+= "\n" "At document: '" + xmlFile_->getFileName()+"'";
			}
			catch(...)
			{
				ret+= "\n" "At incorrect document.";
			}

			try
			{
				if(node_!=NULL)
					ret+="\n" "At node: '"+ xmlFile_->getNodePath(node_)+"'";
			}
			catch(...)
			{
				ret+= "\n" "At incorrect node.";
			}

			if( ! xpath_.empty() )
				ret+= "\n" "With XPath='"+xpath_+"'.";
		}
	}
	catch(...)
	{} // I Am shure! all error ignored.

	return ret;
}

xml_error::xml_error(const std::string& brife_wath_,  XMLFile*xmlFile_, xmlNodePtr node_, const std::string& xpath_)
	: invalid_argument( xml_error_what( brife_wath_,  xmlFile_, node_, xpath_ ) ),
	  xmlFile(xmlFile_), node(node_), xpath(xpath_ )
{}
