/*e***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2005        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD
 *
 *   $Revision: 1.1 $
 *   $Date: 2013/05/24 17:53:14 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/include/gmdinc.h,v 1.1 2013/05/24 17:53:14 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/include/gmdinc.h,v $
$Revision: 1.1 $
$Author: valuev $
$Date: 2013/05/24 17:53:14 $
*/
/*e****************************************************************************
 * $Log: gmdinc.h,v $
 * Revision 1.1  2013/05/24 17:53:14  valuev
 * sync with kintech svn
 *
 * Revision 1.14  2012/11/22 17:01:06  valuev
 * sync with kintech svn
 *
 * Revision 1.13  2012/09/07 14:26:06  valuev
 * removed using namespace from h files
 *
 * Revision 1.12  2012/09/04 13:14:05  valuev
 * wx compatibility
 *
 * Revision 1.11  2012/08/31 15:15:44  valuev
 * restructured gridmd workflow
 *
 * Revision 1.10  2011/07/22 14:24:55  valuev
 * forces with norm matrix
 *
 * Revision 1.9  2011/07/21 14:33:16  valuev
 * made compatible with wxWidgets 2.9 (unicode)
 *
 * Revision 1.8  2010/10/07 11:20:31  valuev
 * preliminary program restart
 *
 * Revision 1.7  2010/10/01 07:01:04  valuev
 * added xml resource saving
 *
 * Revision 1.6  2010/06/12 17:57:31  valuev
 * some workflow coding
 *
 * Revision 1.5  2009/06/10 20:53:28  valuev
 * updated splindes, trajReader
 *
 * Revision 1.4  2008/02/21 14:02:41  valuev
 * Added parametric methods
 *
 * Revision 1.3  2007/12/13 18:58:38  valuev
 * Added interface to Dakota
 *
 * Revision 1.2  2007/11/20 22:15:48  valuev
 * Added parametric project
 *
 * Revision 1.1  2005/12/02 18:50:56  valuev
 * added  HEAD project tree
 *
 * Revision 1.1  2005/11/30 23:37:39  valuev
 * put gridmd on cvs at biolab1.mipt.ru
 *
 *
*******************************************************************************/
# ifndef __WXINC_H
# define __WXINC_H

/*e @file gmdinc.h
    @brief Necessary includes/definitions for wxWidgets Base library    
**/

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <stdio.h>

//e default is STL_HASH
# ifndef STL_HASH
# define STL_HASH 1
# include <map>
# define gm_DECLARE_STRING_HASH_MAP(type,hash_type) typedef std::map<gmdString,type> hash_type;
# endif

//e These macros are used to put wxWidgets declarations into separate namespace.
//e Trick for VC class browser only.



#include <gmd/string.h>
#include <gmd/app.h>

#include <gmd/filename.h>
#include <gmd/tokenzr.h>
//#include <gmd/utils.h>
//#include <gmd/process.h>
//# include <gmd/cmdline.h>

#ifndef NO_XML
#include <gmd/xml/xml_file.h>
#endif

// without this pragma, the stupid compiler precompiles #defines below so that
// changing them doesn't "take place" later!
#ifdef __VISUALC__
    #pragma hdrstop
#endif


# endif
