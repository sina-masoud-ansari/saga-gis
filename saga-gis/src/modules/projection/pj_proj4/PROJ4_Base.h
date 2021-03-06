/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     PROJ4_Base.h                      //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__PROJ4_Base_H
#define HEADER_INCLUDED__PROJ4_Base_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <projects.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	PROJ4_INTERFACE_SIMPLE	= 0,
	PROJ4_INTERFACE_DIALOG
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_proj4_EXPORT CPROJ4_Base : public CSG_Module
{
public:
	CPROJ4_Base(int Interface, bool bInputList);

	virtual bool		do_Sync_Projections			(void)	{	return( false  );	}


protected:

	bool				m_bInputList;


	virtual int			On_Parameter_Changed		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute					(void);
	virtual bool		On_Execute_Conversion		(void)	= 0;

	bool				Set_Inverse					(bool bOn = true);

	CSG_String			Get_Proj_Name				(bool bDestination = true);

	bool				Get_Converted				(double &x, double &y);
	bool				Get_Converted				(TSG_Point &Point);


private:

	bool				m_bInverse;

	int					m_Interface;

	PJ					*m_pPrjSrc, *m_pPrjDst;


	bool				_Get_Projections			(CSG_String &sPrjSrc, CSG_String &sPrjDst);
	bool				_Get_Projection				(CSG_String &sPrj, CSG_Parameters &P);

	bool				_Init_Projection			(CSG_Parameters &P);
	bool				_Init_Projection			(const CSG_String &sID, const CSG_String &sName, const CSG_String &sArgs);


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__PROJ4_Base_H
