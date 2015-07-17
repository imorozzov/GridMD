/*s***************************************************************************
 *
 *   Copyright (c), Ilya Valuev 2005-2015   All Rights Reserved.
 *
 *   Author	: Ilya Valuev, MIPT, Moscow, Russia
 *
 *   Project	: GridMD, ivutils
 *
 *   This source code is Free Software and distributed under the terms of wxWidgets license (www.wxwidgets.org) 
 *
 *   $Revision: 1.1 $
 *   $Date: 2015/03/18 13:03:05 $
 *   @(#) $Header: /home/plasmacvs/source_tree/gridmd/src/gmnode.cpp,v 1.1 2015/03/18 13:03:05 valuev Exp $
 *
 *****************************************************************************/
/*
$Source: /home/plasmacvs/source_tree/gridmd/src/gmnode.cpp,v $
$Revision: 1.1 $
$Author: valuev $
$Date: 2015/03/18 13:03:05 $
*/
/*s****************************************************************************
 * $Log: gmnode.cpp,v $
 * Revision 1.1  2015/03/18 13:03:05  valuev
 * added data write
 *
 *
*******************************************************************************/

#include "gmnode.h"
#include "gridmd.h"

int gmNodeAction::OnExecuteWrapped(){
	int tmode =0;
	// switch to construction mode
	if(m_manager)
  	tmode = m_manager->set_mode(gmMODE_CONSTRUCT);
	
	int res = OnExecute();
	
	if(m_manager){
		// switch back
  	m_manager->copy_mode(tmode);
		// checking if we need to upload info for newly added nodes
		m_manager->check_restart_file();
	}
	return res;
}