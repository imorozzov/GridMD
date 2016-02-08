/*****************************************************************************
*
*   Copyright (c), Kinetic Technologies Ltd. 2003        All Rights Reserved.
*
*   Author	: Denis Shirabaikin, KINTECH, Moscow, Russia
*
*   Project	: KHIMERA
*
*   $Revision: 1.2 $
*   $Date: 2016/02/05 19:04:45 $
*   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmd/xml/xml_file.h,v 1.2 2016/02/05 19:04:45 valuev Exp $
*
*****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmd/xml/xml_file.h,v $
$Revision: 1.2 $
$Author: valuev $
$Date: 2016/02/05 19:04:45 $
*/
/******************************************************************************
* $Log: xml_file.h,v $
* Revision 1.2  2016/02/05 19:04:45  valuev
* implicit workflow fixes
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

#if __cplusplus > 199711L // C++11
# else
#ifndef nullptr
#define nullptr 0
#endif
#endif



class XMLFile {
private:
	std::string					fname; //< I know about 'doc->URL'. 'fname' is used for debug needs.
protected:
    xmlDocPtr					doc;

    virtual double				convertFloat(std::string val) const;
    virtual int					convertInt(std::string val) const;
    virtual unsigned int		convertUInt(std::string val) const;
    virtual bool				convertBool(std::string val) const;

    virtual double				convertFloat(const char* val) const;
    virtual int					convertInt  (const char* val) const;
    virtual unsigned int		convertUInt (const char* val) const;
    virtual bool				convertBool (const char* val) const;

    virtual std::string         convertStr(std::string val) const; // Do nothing. Need for syntax equvialtet to other data types.


    virtual std::string		convertFloat(double         val) const;
    virtual std::string		convertInt  (int            val) const;
    virtual std::string		convertUInt (unsigned int   val) const;
    virtual std::string		convertBool (bool           val) const;

    virtual std::vector<std::string> convertArray(std::string val)  const;
    virtual std::string convertArray(const std::vector<std::string>& val) const;

protected:

    std::string convertStr(double         val) const { return convertFloat(val); }
    std::string convertStr(int            val) const { return convertInt  (val); }
    std::string convertStr(unsigned int   val) const { return convertUInt (val); }
    std::string convertStr(bool           val) const { return convertBool (val); }
    std::string convertStr(const std::vector<std::string>& val) const { return convertArray(val); }

    template<typename T>
    std::string convertStr(const std::vector<T>& val) const
    {
        // Not optimanl, but need call 'virtual convertArray()' to access array splitter, specified at child classs.
        std::vector<std::string> ret;
        ret.reserve(val.size());
        for(size_t i=0; i<val.size(); ++i)
            ret.push_back(convertStr(val[i]));
        return convertArray(ret);
    }


public:

	const std::string&		getFileName () const;        ///< return UTF8 string
	void							setFileName(const std::string& fname);  ///< fname is UTF8 string.

public:
	//-------------------Creating/Modifying----------------------------------------------------------
	/*!Creates empty document. Current document will be deleted.*/
	virtual xmlNodePtr				createDoc(std::string rootName);

	virtual xmlNodePtr				addElement(xmlNodePtr parent, std::string name, std::string content = "");
	virtual xmlNodePtr				addElement(std::string parentPath, std::string name, std::string content = "");

	virtual xmlNodePtr				addOptionalElement(xmlNodePtr parent, std::string name, std::string content);
	virtual xmlNodePtr				addOptionalElement(std::string parentPath, std::string name, std::string content);

	virtual xmlNodePtr				addText(xmlNodePtr parent, std::string content = "\n");
	virtual xmlNodePtr				addText(std::string parentPath, std::string content = "\n");

    virtual void						setContent(xmlNodePtr node, const char* content);
    virtual void						setContent(xmlNodePtr node, std::string content);
    virtual void						setContent(xmlNodePtr node, double      content);
    virtual void						setContent(xmlNodePtr node, int         content);
    virtual void						setContent(xmlNodePtr node, bool        content);
    virtual void						setContent(xmlNodePtr node, std::vector<bool       > content) { return setContent(node, convertStr(content)); }
    virtual void						setContent(xmlNodePtr node, std::vector<int        > content) { return setContent(node, convertStr(content)); }
    virtual void						setContent(xmlNodePtr node, std::vector<double     > content) { return setContent(node, convertStr(content)); }
    virtual void						setContent(xmlNodePtr node, std::vector<std::string> content) { return setContent(node, convertStr(content)); }

    virtual void						setContent(std::string path, const char* content);
    virtual void						setContent(std::string path, std::string content);
    virtual void						setContent(std::string path, double      content);
    virtual void						setContent(std::string path, int         content);
    virtual void						setContent(std::string path, bool        content);
    virtual void						setContent(std::string path, std::vector<bool       > content) { return setContent(path, convertStr(content)); }
    virtual void						setContent(std::string path, std::vector<int        > content) { return setContent(path, convertStr(content)); }
    virtual void						setContent(std::string path, std::vector<double     > content) { return setContent(path, convertStr(content)); }
    virtual void						setContent(std::string path, std::vector<std::string> content) { return setContent(path, convertStr(content)); }


    virtual void						addAttribute(xmlNodePtr node, std::string name, const char* content);
    virtual void						addAttribute(xmlNodePtr node, std::string name, std::string content);
    virtual void						addAttribute(xmlNodePtr node, std::string name, double      content);
    virtual void						addAttribute(xmlNodePtr node, std::string name, int         content);
    virtual void						addAttribute(xmlNodePtr node, std::string name, bool        content);
    virtual void						addAttribute(xmlNodePtr node, std::string name, std::vector<bool       > content) { return addAttribute(node, name, convertStr(content)); }
    virtual void						addAttribute(xmlNodePtr node, std::string name, std::vector<int        > content) { return addAttribute(node, name, convertStr(content)); }
    virtual void						addAttribute(xmlNodePtr node, std::string name, std::vector<double     > content) { return addAttribute(node, name, convertStr(content)); }
    virtual void						addAttribute(xmlNodePtr node, std::string name, std::vector<std::string> content) { return addAttribute(node, name, convertStr(content)); }

    virtual void						addAttribute(std::string path, std::string name, const char* content);
    virtual void						addAttribute(std::string path, std::string name, std::string content);
    virtual void						addAttribute(std::string path, std::string name, double      content);
    virtual void						addAttribute(std::string path, std::string name, int         content);
    virtual void						addAttribute(std::string path, std::string name, bool        content);
    virtual void						addAttribute(std::string path, std::string name, std::vector<bool       > content) { return addAttribute(path, name, convertStr(content)); }
    virtual void						addAttribute(std::string path, std::string name, std::vector<int        > content) { return addAttribute(path, name, convertStr(content)); }
    virtual void						addAttribute(std::string path, std::string name, std::vector<double     > content) { return addAttribute(path, name, convertStr(content)); }
    virtual void						addAttribute(std::string path, std::string name, std::vector<std::string> content) { return addAttribute(path, name, convertStr(content)); }


    virtual void						setAttribute(xmlNodePtr node, std::string name, const char* content);
    virtual void						setAttribute(xmlNodePtr node, std::string name, std::string content);
    virtual void						setAttribute(xmlNodePtr node, std::string name, double      content);
    virtual void						setAttribute(xmlNodePtr node, std::string name, int         content);
    virtual void						setAttribute(xmlNodePtr node, std::string name, bool        content);
    virtual void						setAttribute(xmlNodePtr node, std::string name, std::vector<bool       > content) { return setAttribute(node, name, convertStr(content)); }
    virtual void						setAttribute(xmlNodePtr node, std::string name, std::vector<int        > content) { return setAttribute(node, name, convertStr(content)); }
    virtual void						setAttribute(xmlNodePtr node, std::string name, std::vector<double     > content) { return setAttribute(node, name, convertStr(content)); }
    virtual void						setAttribute(xmlNodePtr node, std::string name, std::vector<std::string> content) { return setAttribute(node, name, convertStr(content)); }

	virtual void						addOptionalAttribute(xmlNodePtr node, std::string name, const char* content);
	virtual void						addOptionalAttribute(xmlNodePtr node, std::string name, std::string content);

	/*!Dumps document into string.*/
    virtual void						dumpToMemory(std::string& s) const;
	virtual void						dumpNodeToMemory(std::string& s, xmlNodePtr node) const;
	virtual void						dumpNodeToMemory(std::string& s, std::string path) const;

    virtual void                        dumpToFile( const std::string& utf8_file_name ) const;
    virtual void                        dumpToFile( const std::string& utf8_file_name , int format) const;

	/*!Removes node from tree and free it's memory.*/
	virtual void						removeNode(xmlNodePtr node);
	virtual void						removeNode(std::string path);

	virtual void						removeAttribute(xmlNodePtr node, std::string name);
	virtual void						removeAttribute(std::string path, std::string name);


	//-------------------Reading-----------------------------------------------------------------

	virtual xmlNodePtr						getRootNode(void) const;

	virtual xmlNodePtr						getNodeByPath(const char* path, bool throwOnNotFound=true) const;
	virtual xmlNodePtr						getNodeByPath(std::string path, bool throwOnNotFound=true) const;
	virtual std::vector<xmlNodePtr>		getNodesByPath(const char* path) const;
	virtual std::vector<xmlNodePtr>		getNodesByPath(std::string path) const;

	virtual xmlNodePtr						getNodeByRelativePath(const char* path, xmlNodePtr current_node, bool throwOnNotFound=true) const;
	virtual xmlNodePtr						getNodeByRelativePath(std::string path, xmlNodePtr current_node, bool throwOnNotFound=true) const;
	virtual std::vector<xmlNodePtr>		getNodesByRelativePath(const char* path, xmlNodePtr current_node) const;
	virtual std::vector<xmlNodePtr>		getNodesByRelativePath(std::string path, xmlNodePtr current_node) const;

    virtual std::string						getNodePath(xmlNodePtr node) const;

    virtual xmlNodePtr						getChildByName(xmlNodePtr parent, const char* name, bool throwOnNotFound=false) const;
    virtual xmlNodePtr						getChildByName(xmlNodePtr parent, std::string name, bool throwOnNotFound=false) const;
	virtual std::vector<xmlNodePtr>		getChildsByName(xmlNodePtr parent, const char* name) const;
	virtual std::vector<xmlNodePtr>		getChildsByName(xmlNodePtr parent, std::string name) const;
	virtual std::vector<xmlNodePtr>		getAllChildren(xmlNodePtr parent) const;
			  
	virtual bool								isNode(xmlNodePtr parent, std::string name) const;
	virtual bool								isNode(xmlNodePtr parent, const char* name) const;

	virtual bool								isNode(std::string path) const;
	virtual bool								isNode(const char* path) const;

	virtual bool								isAttribute(std::string path) const;
	virtual bool								isAttribute(const char* path) const;

	virtual bool								hasAttribute(xmlNodePtr node, std::string name) const;
	virtual bool								hasAttribute(xmlNodePtr node, const char* name) const;
    virtual bool    							hasAttribute(xmlNodePtr node, std::string name, bool inheridFromParent) const;

	virtual std::string						parseNode(xmlNodePtr node) const;
	virtual std::string						getNodeName(xmlNodePtr node) const;

	virtual std::string						parseNodeTree(xmlNodePtr node) const;

	virtual std::string						parseAttribute(xmlNodePtr node, std::string name) const;
	virtual std::string						parseAttribute(xmlNodePtr node, const char* name) const;

	virtual std::string						parseAttribute(std::string root, std::string name) const;
	virtual std::string						parseAttribute(const char* root, const char* name) const;

	virtual int									parseIntAttribute(std::string root, std::string name) const;
	virtual int									parseIntAttribute(const char* root, const char* name) const;
	virtual int									parseIntAttribute(xmlNodePtr node, std::string name) const;

	virtual double								parseFloatAttribute(std::string root, std::string name) const;
	virtual double								parseFloatAttribute(const char* root, const char* name) const;
	virtual double								parseFloatAttribute(xmlNodePtr node, std::string name) const;

	virtual bool								parseBoolAttribute(std::string root, std::string name) const;
	virtual bool								parseBoolAttribute(const char* root, const char* name) const;
	virtual bool								parseBoolAttribute(xmlNodePtr node, std::string name) const;


	virtual bool    							parseOptionalAttribute(xmlNodePtr node, std::string name, bool        defaultValue, bool inheridFromParent = false) const;
	virtual int     							parseOptionalAttribute(xmlNodePtr node, std::string name, int         defaultValue, bool inheridFromParent = false) const;
	virtual double  							parseOptionalAttribute(xmlNodePtr node, std::string name, double      defaultValue, bool inheridFromParent = false) const;
	virtual std::string  					parseOptionalAttribute(xmlNodePtr node, std::string name, std::string defaultValue, bool inheridFromParent = false) const;
	virtual std::string  					parseOptionalAttribute(xmlNodePtr node, std::string name, const char* defaultValue, bool inheridFromParent = false) const;

    virtual std::vector<bool       >		parseOptionalAttribute(xmlNodePtr node, std::string name, std::vector<bool       > defaultValue, bool inheridFromParent = false) const;
    virtual std::vector<int        >		parseOptionalAttribute(xmlNodePtr node, std::string name, std::vector<int        > defaultValue, bool inheridFromParent = false) const;
    virtual std::vector<double     >		parseOptionalAttribute(xmlNodePtr node, std::string name, std::vector<double     > defaultValue, bool inheridFromParent = false) const;
    virtual std::vector<std::string>		parseOptionalAttribute(xmlNodePtr node, std::string name, std::vector<std::string> defaultValue, bool inheridFromParent = false) const;


    virtual bool         					parseOptional(std::string xpath, bool        defaultValue) const;
    virtual int          					parseOptional(std::string xpath, int         defaultValue) const;
    virtual double       					parseOptional(std::string xpath, double      defaultValue) const;
    virtual std::string  					parseOptional(std::string xpath, std::string defaultValue) const;
    virtual std::string  					parseOptional(std::string xpath, const char* defaultValue) const;

    virtual std::vector<bool       >		parseOptional(std::string xpath, std::vector<bool       > defaultValue) const;
    virtual std::vector<int        >		parseOptional(std::string xpath, std::vector<int        > defaultValue) const;
    virtual std::vector<double     >		parseOptional(std::string xpath, std::vector<double     > defaultValue) const;
    virtual std::vector<std::string>		parseOptional(std::string xpath, std::vector<std::string> defaultValue) const;

    virtual bool         					parseOptional(xmlNodePtr parent, std::string xpath, bool        defaultValue) const;
    virtual int          					parseOptional(xmlNodePtr parent, std::string xpath, int         defaultValue) const;
    virtual double       					parseOptional(xmlNodePtr parent, std::string xpath, double      defaultValue) const;
    virtual std::string  					parseOptional(xmlNodePtr parent, std::string xpath, std::string defaultValue) const;
    virtual std::string  					parseOptional(xmlNodePtr parent, std::string xpath, const char* defaultValue) const;

    virtual std::vector<bool       >		parseOptional(xmlNodePtr parent, std::string xpath, std::vector<bool       > defaultValue) const;
    virtual std::vector<int        >		parseOptional(xmlNodePtr parent, std::string xpath, std::vector<int        > defaultValue) const;
    virtual std::vector<double     >		parseOptional(xmlNodePtr parent, std::string xpath, std::vector<double     > defaultValue) const;
    virtual std::vector<std::string>		parseOptional(xmlNodePtr parent, std::string xpath, std::vector<std::string> defaultValue) const;

    //-------------------Construct& parse-----------------------------------------------------------------

	virtual void								attach(std::string fname); ///< fname is UTF8 string.
	virtual void								attach(char* fname);  ///< fname is UTF8 string.
	virtual void								attachMemory(const char *buffer, int size);
	virtual void								attachMemory(std::string buffer);
	virtual void								detach(void);

#if __cplusplus > 199711L // C++11
    XMLFile clone() const;
# endif

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

#if __cplusplus > 199711L  // C++11
    XMLFile( XMLFile&& );
    XMLFile& operator = ( XMLFile&& );
# endif

	virtual ~XMLFile(void);

private:
    // Copy is time-expansive and not oibvious (share or clone DOM tree?),
    // thus copy construction (copy assignment) are not supported.
    // Function 'clone' to be used.
    XMLFile( const XMLFile& );
    XMLFile& operator = ( const XMLFile& );
};

class xml_error : public std::invalid_argument
{
public:
    typedef std::vector<std::string> collection;
    typedef std::vector<xmlNodePtr>  ptr_collection;

    xml_error( const std::string& brife_wath_,
               const XMLFile*xmlFile_=nullptr,
               xmlNodePtr node_=nullptr,
               const std::string& xpath_="",
               const collection& ditails=collection(),
               const ptr_collection& nodes=ptr_collection(),
               const std::string& file_path=""
               );
    const XMLFile*xmlFile;
    xmlNodePtr node;
    std::string xpath;
    std::vector<std::string> ditails;
    ~xml_error() throw() {}


    static std::string make_what(xmlNodePtr node);
    static std::string make_what(const std::string& brife_wath_,
                                 const XMLFile*xmlFile_=nullptr,
                                 xmlNodePtr node_=nullptr,
                                 const std::string& xpath_="",
                                 const collection& ditails=collection(),
                                 const ptr_collection& nodes=ptr_collection(),
                                 const std::string& file_path=""
                                 );

};

#endif
