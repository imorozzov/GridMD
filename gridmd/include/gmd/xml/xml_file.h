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
*   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmd/xml/xml_file.h,v 1.1 2013/05/24 17:53:14 valuev Exp $
*
*****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmd/xml/xml_file.h,v $
$Revision: 1.1 $
$Author: valuev $
$Date: 2013/05/24 17:53:14 $
*/
/******************************************************************************
* $Log: xml_file.h,v $
* Revision 1.1  2013/05/24 17:53:14  valuev
* sync with kintech svn
*
* Revision 1.19  2009/05/22 09:32:18  wasp
* Parsing of nullable double attribute was implemented.
*
* Revision 1.18  2008/06/07 14:04:13  chorkov
* added specials. exception for the error XML.
*
* Revision 1.17  2008/06/03 12:45:13  wasp
* Function getRootNode() was added.
*
* Revision 1.16  2008/04/30 14:13:16  wasp
* Functions hasAttribute were added. These functions will replace corresponding isAttribute functions.
*
* Revision 1.15  2007/12/03 16:13:18  chorkov
* *** empty log message ***
*
* Revision 1.14  2007/11/02 11:37:06  wasp
* Bug in converBool(std::string) was fixed. Function createDoc was modified.
*
* Revision 1.13  2007/10/31 15:06:11  wasp
* Functions for XML file creation and manipulation were implemented.
*
* Revision 1.12  2007/10/30 14:48:13  wasp
* Dumping of xml tree was implemented. Removing node was added.
*
* Revision 1.11  2006/10/20 12:49:21  wasp
* Parsing from memory was added.
*
* Revision 1.10  2006/10/18 13:52:24  wasp
* Attaching memory content was implemented.
*
* Revision 1.9  2006/09/20 15:32:09  wasp
* Parsing uint values was implemented.
*
* Revision 1.8  2005/05/26 10:04:30  tatiana
* Function getFileName has been implemented in class XMLFile
*
* Revision 1.7  2005/05/03 07:41:05  wasp
* The parseBoolAttribute function was added.
*
* Revision 1.6  2005/01/11 10:28:20  wasp
* This revision corresponds to the New Year delivery.
*
* Revision 1.5  2004/10/08 14:13:48  wasp
* New functions for parsing data from xmlNodePtr were added.
* Typed XML file class was removed.
*
* Revision 1.4  2004/07/05 13:39:48  wasp
* Headers order was changed to fix the problem with MSVC.
*
* Revision 1.3  2004/06/16 14:00:32  chorkov
* Add isAttribute method.
* Add method clear to errase.
*
* Revision 1.2  2004/04/19 10:08:29  wasp
* Additional functions for processing XmlNodePtr were implemented.
*
* Revision 1.1.1.1  2003/11/03 14:05:25  wasp
* Initial revision
*
*
*
*
*******************************************************************************/

#ifndef _HDR_XML_FILE_
#define _HDR_XML_FILE_


#define LIBXML_DEBUG_ENABLED

#ifndef __MAKEDEPEND__
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <list>
#endif



class XMLFile {
private:
	std::string					fname; //< I know about 'doc->URL'. 'fname' is used for debug needs.
protected:
	xmlDocPtr					doc;

	virtual double				convertFloat(std::string val);
	virtual int					convertInt(std::string val);
	virtual unsigned int		convertUInt(std::string val);
	virtual bool				convertBool(std::string val);

	virtual double				convertFloat(const char* val);
	virtual int					convertInt(const char* val);
	virtual unsigned int		convertUInt(const char* val);
	virtual bool				convertBool(const char* val);

    virtual std::string         convertStr(std::string val); // Do nothing. Need for syntax equvialtet to other data types.

	virtual std::string		convertFloat(double val);
	virtual std::string		convertInt(int val);
	virtual std::string		convertUInt(unsigned int val);
	virtual std::string		convertBool(bool val);

public:

	const std::string&		getFileName () const;        ///< return UTF8 string
	void							setFileName(const std::string& fname);  ///< fname is UTF8 string.

public:
	//-------------------Creating/Modifying----------------------------------------------------------
	/*!Creates empty document. Current document will be deleted.*/
	virtual xmlNodePtr				createDoc(std::string rootName);

	virtual xmlNodePtr				addElement(xmlNodePtr parent, std::string name, std::string content = "");
	virtual xmlNodePtr				addElement(std::string parentPath, std::string name, std::string content = "");

	virtual xmlNodePtr				addText(xmlNodePtr parent, std::string content = "\n");
	virtual xmlNodePtr				addText(std::string parentPath, std::string content = "\n");

	virtual void						setContent(xmlNodePtr node, const char* content);
	virtual void						setContent(xmlNodePtr node, std::string content);
	virtual void						setContent(xmlNodePtr node, double content);
	virtual void						setContent(xmlNodePtr node, int content);
	virtual void						setContent(xmlNodePtr node, bool content);

	virtual void						setContent(std::string path, const char* content);
	virtual void						setContent(std::string path, std::string content);
	virtual void						setContent(std::string path, double content);
	virtual void						setContent(std::string path, int content);
	virtual void						setContent(std::string path, bool content);

	virtual void						addAttribute(xmlNodePtr node, std::string name, const char* content);
	virtual void						addAttribute(xmlNodePtr node, std::string name, std::string content);
	virtual void						addAttribute(xmlNodePtr node, std::string name, double content);
	virtual void						addAttribute(xmlNodePtr node, std::string name, int content);
	virtual void						addAttribute(xmlNodePtr node, std::string name, bool content);

	virtual void						addAttribute(std::string path, std::string name, const char* content);
	virtual void						addAttribute(std::string path, std::string name, std::string content);
	virtual void						addAttribute(std::string path, std::string name, double content);
	virtual void						addAttribute(std::string path, std::string name, int content);
	virtual void						addAttribute(std::string path, std::string name, bool content);

	virtual void						setAttribute(xmlNodePtr node, std::string name, const char* content);
	virtual void						setAttribute(xmlNodePtr node, std::string name, std::string content);
	virtual void						setAttribute(xmlNodePtr node, std::string name, double content);
	virtual void						setAttribute(xmlNodePtr node, std::string name, int content);
	virtual void						setAttribute(xmlNodePtr node, std::string name, bool content);

	virtual void						addOptionalAttribute(xmlNodePtr node, std::string name, const char* content);
	virtual void						addOptionalAttribute(xmlNodePtr node, std::string name, std::string content);

	/*!Dumps document into string.*/
    virtual void						dumpToMemory(std::string& s);
	virtual void						dumpNodeToMemory(std::string& s, xmlNodePtr node);
	virtual void						dumpNodeToMemory(std::string& s, std::string     path);

    virtual void                        dumpToFile( const std::string& utf8_file_name );

	/*!Removes node from tree and free it's memory.*/
	virtual void						removeNode(xmlNodePtr node);
	virtual void						removeNode(std::string path);

	virtual void						removeAttribute(xmlNodePtr node, std::string name);
	virtual void						removeAttribute(std::string path, std::string name);


	//-------------------Reading-----------------------------------------------------------------

	virtual xmlNodePtr						getRootNode(void);

	virtual xmlNodePtr						getNodeByPath(const char* path, bool throwOnNotFound=true);
	virtual xmlNodePtr						getNodeByPath(std::string path, bool throwOnNotFound=true);
	virtual std::vector<xmlNodePtr>		getNodesByPath(const char* path);
	virtual std::vector<xmlNodePtr>		getNodesByPath(std::string path);

	virtual xmlNodePtr						getNodeByRelativePath(const char* path, xmlNodePtr current_node, bool throwOnNotFound=true);
	virtual xmlNodePtr						getNodeByRelativePath(std::string path, xmlNodePtr current_node, bool throwOnNotFound=true);
	virtual std::vector<xmlNodePtr>		getNodesByRelativePath(const char* path, xmlNodePtr current_node);
	virtual std::vector<xmlNodePtr>		getNodesByRelativePath(std::string path, xmlNodePtr current_node);

	virtual std::string						getNodePath(xmlNodePtr node);

    virtual xmlNodePtr						getChildByName(xmlNodePtr parent, const char* name, bool throwOnNotFound=false);
    virtual xmlNodePtr						getChildByName(xmlNodePtr parent, std::string name, bool throwOnNotFound=false);
	virtual std::vector<xmlNodePtr>		getChildsByName(xmlNodePtr parent, const char* name);
	virtual std::vector<xmlNodePtr>		getChildsByName(xmlNodePtr parent, std::string name);
	virtual std::vector<xmlNodePtr>		getAllChildren(xmlNodePtr parent);
			  
	virtual bool								isNode(xmlNodePtr parent, std::string name);
	virtual bool								isNode(xmlNodePtr parent, const char* name);

	virtual bool								isNode(std::string path);
	virtual bool								isNode(const char* path);

	virtual bool								isAttribute(std::string path);
	virtual bool								isAttribute(const char* path);

	virtual bool								hasAttribute(xmlNodePtr node, std::string name);
	virtual bool								hasAttribute(xmlNodePtr node, const char* name);

	virtual std::string						parseNode(xmlNodePtr node);
	virtual std::string						getNodeName(xmlNodePtr node);

	virtual std::string						parseNodeTree(xmlNodePtr node);

	virtual std::string						parseAttribute(xmlNodePtr node, std::string name);
	virtual std::string						parseAttribute(xmlNodePtr node, const char* name);

	virtual std::string						parseAttribute(std::string root, std::string name);
	virtual std::string						parseAttribute(const char* root, const char* name);

	virtual int									parseIntAttribute(std::string root, std::string name);
	virtual int									parseIntAttribute(const char* root, const char* name);
	virtual int									parseIntAttribute(xmlNodePtr node, std::string name);

	virtual double								parseFloatAttribute(std::string root, std::string name);
	virtual double								parseFloatAttribute(const char* root, const char* name);
	virtual double								parseFloatAttribute(xmlNodePtr node, std::string name);

	virtual bool								parseBoolAttribute(std::string root, std::string name);
	virtual bool								parseBoolAttribute(const char* root, const char* name);
	virtual bool								parseBoolAttribute(xmlNodePtr node, std::string name);

	virtual bool    							parseOptionalAttribute(xmlNodePtr node, std::string name, bool        defaultValue, bool inheridFromParent = false);
	virtual int     							parseOptionalAttribute(xmlNodePtr node, std::string name, int         defaultValue, bool inheridFromParent = false);
	virtual double  							parseOptionalAttribute(xmlNodePtr node, std::string name, double      defaultValue, bool inheridFromParent = false);
	virtual std::string  					parseOptionalAttribute(xmlNodePtr node, std::string name, std::string defaultValue, bool inheridFromParent = false);
	virtual std::string  					parseOptionalAttribute(xmlNodePtr node, std::string name, const char* defaultValue, bool inheridFromParent = false);


	virtual bool    							parseOptional(std::string xpath, bool        defaultValue);
	virtual int     							parseOptional(std::string xpath, int         defaultValue);
	virtual double  							parseOptional(std::string xpath, double      defaultValue);
	virtual std::string  					parseOptional(std::string xpath, std::string      defaultValue);
	virtual std::string  					parseOptional(std::string xpath, const char* defaultValue);

    virtual bool    						parseOptional(xmlNodePtr parent, std::string xpath, bool        defaultValue);
    virtual int     						parseOptional(xmlNodePtr parent, std::string xpath, int         defaultValue);
    virtual double  						parseOptional(xmlNodePtr parent, std::string xpath, double      defaultValue);
    virtual std::string  					parseOptional(xmlNodePtr parent, std::string xpath, std::string defaultValue);
    virtual std::string  					parseOptional(xmlNodePtr parent, std::string xpath, const char* defaultValue);

    //-------------------Construct& parse-----------------------------------------------------------------

	virtual void								attach(std::string fname); ///< fname is UTF8 string.
	virtual void								attach(char* fname);  ///< fname is UTF8 string.
	virtual void								attachMemory(const char *buffer, int size);
	virtual void								attachMemory(std::string buffer);
	virtual void								detach(void);

	/**
	iteration - is max number of iterations, to be process to resolve
	xinclude, pointed on xinclude.
	set '-1' to infinite iterations;
	Return count of substituted nodes.
	**/
	virtual int			processXinclude(int iterations_limit=1);
	virtual int			processXinclude(xmlNodePtr node, int iterations_limit=1);

	XMLFile(void);
	XMLFile(std::string fname); ///< fname is UTF8 string.
	XMLFile(char* fname); ///< fname is UTF8 string.
	XMLFile(const char *buffer, int size);
	XMLFile(xmlDocPtr doc);

	virtual ~XMLFile(void);

};

class xml_error : public std::invalid_argument
{
public:
	xml_error( const std::string& brife_wath_,  XMLFile*xmlFile_=NULL, xmlNodePtr node_=NULL, const std::string& xpath_="");
	XMLFile*xmlFile;
	xmlNodePtr node;
	std::string xpath;

	~xml_error() throw() {}
};


#endif
