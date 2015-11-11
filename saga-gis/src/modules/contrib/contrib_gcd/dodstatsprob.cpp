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
//                       contrib_gcd                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     dodstatsprob.cpp                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                        Author                         //
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
//    e-mail:     author@email.net                       //
//                                                       //
//    contact:    Author                                 //
//                Sesame Street. 7                       //
//                12345 Metropolis                       //
//                Nirvana                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dodstatsprob.h"



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cdodstatsprob::Cdodstatsprob(void)
{
	// Module info
	Set_Name		(_TL("Probabilistic Threshold Statistics"));
	Set_Author		(SG_T("Sina Masoud-Ansari"));
	Set_Description	(_TW("Display DoD statistics for a probabilistic thresholded raster."));

	// GCD setup
	GCDBinDir = SG_File_Make_Path(CSG_String("bin"), CSG_String("GCD"));
	GCDBinDir = SG_File_Get_Path_Absolute(GCDBinDir);
	GCD = SG_File_Make_Path(GCDBinDir, CSG_String("gcd"), CSG_String("exe"));
	GCD_CMD = CSG_String("dodstatsprob");

	// Logging
	LogOutput = SG_File_Make_Path(GCDBinDir, CSG_String("out"), CSG_String("txt"));
	LogError = SG_File_Make_Path(GCDBinDir, CSG_String("error"), CSG_String("txt"));;

	// Grids
	Parameters.Add_Grid(NULL, "DOD_INPUT"	, _TL("DoD"), _TL("Raster to be used as DoD"), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "DOD_THRESHOLD"	, _TL("Threshold"), _TL("Thresholded raster"), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "DOD_PROPERROR"	, _TL("Propagated Error"), _TL("Propagated error raster"), PARAMETER_INPUT);

	DoD_InputPath = SG_File_Make_Path(GCDBinDir, CSG_String("dodinput"), CSG_String("tif"));
	DoD_ThresholdPath = SG_File_Make_Path(GCDBinDir, CSG_String("dodthresh"), CSG_String("tif"));
	DoD_PropErrorPath = SG_File_Make_Path(GCDBinDir, CSG_String("dodproperror"), CSG_String("tif"));

	// Tables
	Parameters.Add_Table(NULL, "STATS_OUTPUT", _TL("DoD Output Statistics"), _TL("DoD Statistics"), PARAMETER_OUTPUT); 

	//GDAL
	GDALDriver = CSG_String("GTiff");
	GDALOptions = CSG_String(""); 
	Get_Projection(Projection);

}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cdodstatsprob::On_Execute(void)
{

	if (!GetParameterValues())
	{
		return false;
	}

	// convert grids to tiffs for command input
	CSG_Grid* InputGrids [3] = {DoD_Input, DoD_Threshold, DoD_PropError};

	CSG_Strings InputGridPaths = CSG_Strings();
	InputGridPaths.Add(DoD_InputPath);
	InputGridPaths.Add(DoD_ThresholdPath);
	InputGridPaths.Add(DoD_PropErrorPath);

	if (!SaveGridsAsTIFF(InputGrids, InputGridPaths))
	{
		return false;
	}

	CSG_String CMD = CSG_String::Format(SG_T("%s %s %s %s %s >%s 2>%s"), GCD.c_str(), GCD_CMD.c_str(), DoD_InputPath.c_str(), DoD_ThresholdPath.c_str(), DoD_PropErrorPath.c_str(), LogOutput.c_str(), LogError.c_str());
	Message_Add(CSG_String("Executing: ") + CMD);			
	if (system(CMD.b_str()) != 0)
	{
		Message_Dlg(CSG_String::Format(SG_T("Error while executing %s, see Execution Log for details"), GCD_CMD.c_str()));
		DisplayLogs();
		return false;
	}	

	CreateStatsTable();

	DisplayFile(LogOutput);
	return true;
}

bool Cdodstatsprob::CreateStatsTable()
{
	// set up table
	DoDStatsTable->Destroy();
	DoDStatsTable->Set_Name("DoD Stats");

	// set table fields
	DoDStatsTable->Add_Field(SG_T("DoD Raster"), SG_DATATYPE_String);
	DoDStatsTable->Add_Field(SG_T("Area Erosion"), SG_DATATYPE_Double);
	DoDStatsTable->Add_Field(SG_T("Area Deposition"), SG_DATATYPE_Double);
	DoDStatsTable->Add_Field(SG_T("Vol Erosion"), SG_DATATYPE_Double);
	DoDStatsTable->Add_Field(SG_T("Vol Deposition"), SG_DATATYPE_Double);

	// read table data
	CSG_File File;
	if (File.Open(LogOutput, SG_FILE_R, false))
	{

		if (File.Length() == 0)
		{
			Message_Add(CSG_String("Output file " + LogOutput + CSG_String(" is empty!")));
			return false;
		}
		else
		{
			CSG_String Line;
			CSG_Table_Record* Record;

			// skip first two lines
			File.Read_Line(Line); File.Read_Line(Line);

			// RAW
			// -------------------------------------------------------

			Record = DoDStatsTable->Add_Record();
			Record->Set_Value(0, "Raw");

			// Area Erosion
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(1, Line.asDouble());

			// Area Deposition
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(2, Line.asDouble());

			// Vol Erosion
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(3, Line.asDouble());

			// Vol Deposition
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(4, Line.asDouble());

			// skip next 3 lines
			File.Read_Line(Line); File.Read_Line(Line);  File.Read_Line(Line);
			// -------------------------------------------------------

			// THRESHOLDED
			// -------------------------------------------------------

			Record = DoDStatsTable->Add_Record();
			Record->Set_Value(0, "Thresholded");

			// Area Erosion
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(1, Line.asDouble());

			// Area Deposition
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(2, Line.asDouble());

			// Vol Erosion
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(3, Line.asDouble());

			// Vol Deposition
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(4, Line.asDouble());		

			// skip 2 lines
			File.Read_Line(Line); File.Read_Line(Line);  File.Read_Line(Line);
			// -------------------------------------------------------

			// ERROR
			// -------------------------------------------------------
			Record = DoDStatsTable->Add_Record();
			Record->Set_Value(0, "Error");

			// Vol Erosion
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(3, Line.asDouble());

			// Vol Deposition
			File.Read_Line(Line); Line = Line.AfterFirst(':'); Line.Trim();
			Record->Set_Value(4, Line.asDouble());					
			// -------------------------------------------------------

		}
		File.Close();
	}
	else
	{
		Message_Add(CSG_String("Unable to open " + LogOutput + CSG_String(" for reading")));
		return false;
	}

	return true;
}


bool Cdodstatsprob::SaveGridsAsTIFF(CSG_Grid** grids, CSG_Strings paths)
{
	TSG_Data_Type Type;
	CSG_String FilePath;
	CSG_Grid* Grid;

	// SAVE TIFFS
	for (int i = 0; i < paths.Get_Count(); i++)
	{
		 FilePath = paths[i];
		 Grid = grids[i];
		 Type = Grid->Get_Type();

		if( !GDALDataSet.Open_Write(FilePath, GDALDriver, GDALOptions, Type, 1, *Get_System(), Projection) )
		{
			Error_Set(CSG_String::Format(SG_T("%s: '%s' "), _TL("Failed to open file for writing: "), FilePath.c_str()));
			return( false );
		}
		GDALDataSet.Write(0, Grid);
		if( !GDALDataSet.Close() )
		{
			Error_Set(CSG_String::Format(SG_T("%s: '%s' "), _TL("Failed to close file after writing: "), FilePath.c_str()));
			return( false );
		}
	}

	return true;
}

bool Cdodstatsprob::GetParameterValues()
{

	DoD_Input = Parameters("DOD_INPUT")->asGrid();
	DoD_Threshold = Parameters("DOD_THRESHOLD")->asGrid();
	DoD_PropError = Parameters("DOD_PROPERROR")->asGrid();
	DoDStatsTable = Parameters("STATS_OUTPUT")->asTable();

	return true;
}

void Cdodstatsprob::DisplayLogs()
{
	DisplayFile(LogOutput);
	DisplayFile(LogError);
}

void Cdodstatsprob::DisplayFile(CSG_String path)
{

	if (SG_File_Exists(path))
	{
		CSG_File File;
		if (File.Open(path, SG_FILE_R, false))
		{
			CSG_String Line;
			while (! File.is_EOF() && File.Read_Line(Line))
			{
				Message_Add(Line);
			}
			File.Close();
		} 
		else 
		{
			Message_Add(CSG_String("Unable to open " + path + CSG_String(" for reading")));
		}
	}
	else
	{
		Message_Add(CSG_String("File '" + path + CSG_String("' does not exist")));
	}

}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
