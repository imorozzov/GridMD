/*s***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2005-2010        All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD, ivutils
 *
 *   This source code is Free Software and distributed under the terms of wxWidgets license (www.wxwidgets.org) 
 *
 *   $Revision: 1.4 $
 *   $Date: 2010/10/07 16:41:03 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/src/gmfork.cpp,v 1.4 2010/10/07 16:41:03 morozov Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/src/gmfork.cpp,v $
$Revision: 1.4 $
$Author: morozov $
$Date: 2010/10/07 16:41:03 $
*/
/*s****************************************************************************
 * $Log: gmfork.cpp,v $
 * Revision 1.4  2010/10/07 16:41:03  morozov
 * Fixed Unix compatibility issues for release 2.1
 *
 * Revision 1.3  2010/10/01 07:01:04  valuev
 * added xml resource saving
 *
 * Revision 1.2  2010/09/15 18:12:25  valuev
 * moved misplaced counter
 *
 * Revision 1.1  2010/09/15 17:55:21  valuev
 * added fork skeleton
 *
 * Revision 1.1  2010/08/04 16:07:28  valuev
 * added forgotten file
 *
 *
*******************************************************************************/

# include "gmfork.h"


int fork_objcount[MAX_RECURSION]={-1};
