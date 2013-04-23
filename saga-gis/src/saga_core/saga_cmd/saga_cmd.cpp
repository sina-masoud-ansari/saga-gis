/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     SAGA_CMD.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/app.h>
#include <wx/utils.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute			(int argc, char *argv[]);

bool		Run_Script		(const CSG_String &Script);

bool		Load_Libraries	(void);

bool		Check_First		(const CSG_String &Argument);
bool		Check_Flags		(const CSG_String &Argument);

void		Print_Libraries	(void);
void		Print_Modules	(CSG_Module_Library *pLibrary);
void		Print_Execution	(CSG_Module_Library *pLibrary, CSG_Module *pModule);

void		Print_Logo		(void);
void		Print_Get_Help	(void);
void		Print_Help		(void);
void		Print_Version	(void);

void		Create_Example	(void);
void		Create_Docs		(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		main	(int argc, char *argv[])
{
//---------------------------------------------------------
#if !defined(_DEBUG) && defined(_SAGA_VC)
#define _MODULE_EXCEPTION
_try 
{
#endif
//---------------------------------------------------------

#if wxCHECK_VERSION(2, 8, 11)
	if( !wxInitialize(argc, argv) )
#else
	if( !wxInitialize() )
#endif
	{
		CMD_Print_Error(SG_T("initialisation failed"));

		return( 1 );
	}

	setlocale(LC_NUMERIC, "C");
	
	SG_Set_UI_Callback(CMD_Get_Callback());

	//-----------------------------------------------------
	if( !Load_Libraries() )
	{
		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	if( argc <= 1 )
	{
		Print_Libraries();

		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	if( Check_First(argv[1]) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	while( argc > 1 && Check_Flags(argv[1]) )
	{
		argc--;	argv++;
	}

	if( argc <= 1 )
	{
		CMD_Print_Error(_TL("no arguments for saga call"));

		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	bool	bResult;

	Print_Logo();

	if( argc == 2 && SG_File_Exists(CSG_String(argv[1])) )
	{
		bResult	= Run_Script(argv[1]);
	}
	else
	{
		bResult	= Execute(argc, argv);
	}

	CMD_Print("");

//---------------------------------------------------------
#ifdef _DEBUG
	CMD_Set_Interactive(true);
	CMD_Get_Pause();
#endif

#ifdef _MODULE_EXCEPTION
}
_except(1)
{
	Print_Error(_TL("access violation"));
}
#endif
//---------------------------------------------------------

	wxUninitialize();

	return( bResult ? 0 : 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute(int argc, char *argv[])
{
	CSG_Module_Library	*pLibrary;
	CSG_Module			*pModule;

	if( argc == 1 || (pLibrary = SG_Get_Module_Library_Manager().Get_Library(CSG_String(argv[1]), true)) == NULL )
	{
		Print_Libraries();

		return( false );
	}

	if( argc == 2
	||  (  (pModule = pLibrary->Get_Module(CSG_String(argv[2])        )) == NULL
	    && (pModule = pLibrary->Get_Module(CSG_String(argv[2]).asInt())) == NULL) )
	{
		Print_Modules(pLibrary);

		return( false );
	}

	if( argc == 3 && CMD_Get_XML() )
	{	// Just output module synopsis as XML-tagged text, then return.
		CMD_Print(stderr, pModule->Get_Summary(true, "", "", true));

		return true;
	}

	if( pModule->is_Interactive() )
	{
		CMD_Print_Error(_TL("cannot execute interactive module"), pModule->Get_Name());

		return( false );
	}

	//-----------------------------------------------------
	Print_Execution(pLibrary, pModule);

	CCMD_Module	CMD_Module(pModule);

	return( CMD_Module.Execute(argc - 2, argv + 2) );
}

//---------------------------------------------------------
bool		Execute(CSG_String Command)
{
	Command.Trim();

	if( Command.is_Empty() )
	{
		return( true );
	}

	if( !Command.Left(3).CmpNoCase("REM") || Command[0] == '#' )
	{
		return( true );
	}

	if( !Command.Left(4).CmpNoCase("ECHO") )
	{
		CMD_Print(Command.AfterFirst(' '));

		return( true );
	}

	//-----------------------------------------------------
	int		argc	= 1;
	char	**argv	= NULL;

	while( Command.Length() > 0 )
	{
		CSG_String	s	= Command[0] == '\"' ? Command.AfterFirst('\"').BeforeFirst('\"') : Command.BeforeFirst(' ');

		argv		= (char **)SG_Realloc(argv, (argc + 1) * sizeof(char *));
		argv[argc]	= (char  *)SG_Calloc(1 + s.Length(), sizeof(char));

		memcpy(argv[argc++], s.b_str(), s.Length() * sizeof(char));

		Command	= Command.AfterFirst(' ');	Command.Trim();
	}

	//-----------------------------------------------------
	bool	bResult	= Execute(argc, argv);

	for(int i=1; i<argc; i++)
	{
		SG_FREE_SAFE(argv[i]);
	}

	SG_FREE_SAFE(argv);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Set_Environment(CSG_String &Command)
{
	wxString	Value, Key	= Command.AfterFirst('%').BeforeFirst('%').c_str();

	if( Key.IsEmpty() )
	{
		return( true );
	}

	if( !wxGetEnv(Key, &Value) || Command.Replace(&Key.Prepend("%").Append("%"), &Value) < 1 )
	{
		return( false );
	}

	return( Set_Environment(Command) );
}

//---------------------------------------------------------
bool		Run_Script(const CSG_String &Script)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(CSG_String::Format(SG_T("%s: %s"), _TL("Running Script"), Script.c_str()));
	}

	CSG_File	Stream;

	if( !Stream.Open(Script, SG_FILE_R, false) )
	{
		CMD_Print_Error(_TL("could not open file"), Script);

		return( false );
	}

	CSG_String	Command;

	while( Stream.Read_Line(Command) )
	{
		Set_Environment(Command);

		if( !Execute(Command) )
		{
			CMD_Print_Error(_TL("invalid command"), Command);

			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Load_Libraries(const CSG_String &Directory)
{
	bool	bShow	= CMD_Get_Show_Messages();

	CMD_Set_Show_Messages(false);
	int	n	= SG_Get_Module_Library_Manager().Add_Directory(Directory, false);
	CMD_Set_Show_Messages(bShow);

	return( n > 0 );
}

//---------------------------------------------------------
bool		Load_Libraries(void)
{
	wxString	Path, CMD_Path	= SG_File_Get_Path(SG_UI_Get_Application_Path()).c_str();

    #if defined(_SAGA_LINUX)
		Load_Libraries(wxT(MODULE_LIBRARY_PATH));
	#else
		if( wxGetEnv(wxT("PATH"), &Path) && Path.Length() > 0 )
		{
			Path	+= wxT(";");
		}

		Path	+= SG_File_Make_Path(CMD_Path, SG_T("dll"    )).c_str(); Path += wxT(";");
		Path	+= SG_File_Make_Path(CMD_Path, SG_T("modules")).c_str();

		wxSetEnv("PATH"            , Path);
		wxSetEnv("GDAL_DRIVER_PATH", SG_File_Make_Path(CMD_Path, SG_T("dll")).c_str());

		Load_Libraries(SG_File_Make_Path(CMD_Path, SG_T("modules")));
    #endif

	if( wxGetEnv(SG_T("SAGA_MLB"), &Path) && wxDirExists(Path) )
	{
		Load_Libraries(&Path);
	}

	if( SG_Get_Module_Library_Manager().Get_Count() <= 0 )
	{
		CMD_Print_Error(SG_T("could not load any module library"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Check_First		(const CSG_String &Argument)
{
	if( !Argument.CmpNoCase(SG_T("-h")) || !Argument.CmpNoCase(SG_T("--help")) )
	{
		Print_Help();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-v")) || !Argument.CmpNoCase(SG_T("--version")) )
	{
		Print_Version();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-b")) || !Argument.CmpNoCase(SG_T("--batch")) )
	{
		Create_Example();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-d")) || !Argument.CmpNoCase(SG_T("--docs")) )
	{
		Create_Docs();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		Check_Flags		(const CSG_String &Argument)
{
	CSG_String	s(Argument.BeforeFirst(SG_T('=')));

	if( !s.CmpNoCase(SG_T("-f")) || !s.CmpNoCase(SG_T("--flags")) )
	{
		s	= CSG_String(Argument).AfterFirst(SG_T('='));

		CMD_Set_Show_Progress(s.Find('q') < 0 && s.Find('s') < 0);	// q, s: no progress report
		CMD_Set_Show_Messages(s.Find('r') < 0 && s.Find('s') < 0);	// r, s: no messages report
		CMD_Set_Interactive  (s.Find('i') >= 0                  );	// i: allow user interaction
		CMD_Set_XML          (s.Find('x') >= 0                  );	// x: message output as xml

		if( s.Find('l') >= 0 )	// l: load translation dictionary
		{
			SG_Get_Translator() .Create(SG_File_Make_Path(SG_UI_Get_Application_Path(), SG_T("saga"), SG_T("lng")), false);
		}

		if( s.Find('p') >= 0 )	// p: load projections dictionary
		{
			SG_Get_Projections().Create(SG_File_Make_Path(SG_UI_Get_Application_Path(), SG_T("saga_prj"), SG_T("srs")));
		}

		return( true );
	}

	else if( !s.CmpNoCase(SG_T("-c")) || !s.CmpNoCase(SG_T("--cores")) )
	{
		#ifdef _OPENMP
		int	nCores	= 1;

		if( !CSG_String(Argument).AfterFirst(SG_T('=')).asInt(nCores) || nCores > SG_Get_Max_Num_Procs_Omp() )
		{
			nCores	= SG_Get_Max_Num_Procs_Omp();
		}

		SG_Set_Max_Num_Threads_Omp(nCores);
		#endif // _OPENMP

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Print_Libraries	(void)
{
	CMD_Print_Error(_TL("library"));

	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			CMD_Print(SG_Get_Module_Library_Manager().Get_Summary(SG_SUMMARY_FMT_XML_NO_INTERACTIVE));
		}
		else
		{
			CMD_Print(SG_Get_Module_Library_Manager().Get_Summary(SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE));
		}
	}
}

//---------------------------------------------------------
void		Print_Modules	(CSG_Module_Library *pLibrary)
{
	CMD_Print_Error(_TL("module"));

	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_XML_NO_INTERACTIVE));
		}
		else
		{
			CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE));

			Print_Get_Help();
		}
	}
}

//---------------------------------------------------------
void		Print_Execution	(CSG_Module_Library *pLibrary, CSG_Module *pModule)
{
	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			SG_PRINTF(SG_T("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n"));
			SG_PRINTF(SG_T("\n<%s>"), SG_XML_LIBRARY);
			SG_PRINTF(SG_T("\n\t<%s>%s</%s>"), SG_XML_LIBRARY_PATH, pLibrary->Get_File_Name().c_str(), SG_XML_LIBRARY_PATH);
			SG_PRINTF(SG_T("\n\t<%s>%s</%s>"), SG_XML_LIBRARY_NAME, pLibrary->Get_Name()     .c_str(), SG_XML_LIBRARY_NAME);
			SG_PRINTF(SG_T("\n</%s>"), SG_XML_LIBRARY);
		}
		else
		{
			SG_PRINTF(SG_T("\n_____________________________________________"));
			SG_PRINTF(SG_T("\n%s:\t%s"), _TL("library path"), pLibrary->Get_File_Name().c_str());
			SG_PRINTF(SG_T("\n%s:\t%s"), _TL("library name"), pLibrary->Get_Name     ().c_str());
			SG_PRINTF(SG_T("\n%s:\t%s"), _TL("module name "), pModule ->Get_Name     ().c_str());
			SG_PRINTF(SG_T("\n%s:\t%s"), _TL("author      "), pModule ->Get_Author   ().c_str());
			SG_PRINTF(SG_T("\n_____________________________________________\n"));
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Print_Logo		(void)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(
			"\n_____________________________________________"
			"\n  #####   ##   #####    ##"
			"\n ###     ###  ##       ###"
			"\n  ###   # ## ##  #### # ##"
			"\n   ### ##### ##    # #####"
			"\n##### #   ##  ##### #   ##"
			"\n_____________________________________________"
			"\n"
		);
	}
}

//---------------------------------------------------------
void		Print_Version	(void)
{
	CMD_Print(SG_T("SAGA Version: ") SAGA_VERSION SG_T("\n"));
}

//---------------------------------------------------------
void		Print_Get_Help	(void)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(_TL("type -h or --help for further information"));
		CMD_Print("");
	} 
}

//---------------------------------------------------------
void		Print_Help		(void)
{
	Print_Logo();

	Print_Version();

	CMD_Print(
		"under GNU General Public License (GPL)\n"
		"\n"
		"Usage:\n"
		"\n"
		"saga_cmd [-h, --help]\n"
		"saga_cmd [-v, --version]\n"
		"saga_cmd [-b, --batch]\n"
		"saga_cmd [-d, --docs]\n"
#ifdef _OPENMP
		"saga_cmd [-c, --cores][= # of CPU cores] <LIBRARY> <MODULE> <module specific options...>\n"
#endif
		"saga_cmd [-f, --flags][=qrsilpx] <LIBRARY> <MODULE> <module specific options...>\n"
		"saga_cmd [-f, --flags][=qrsilpx] <SCRIPT>\n"
		"\n"
		"[-h], [--help]: help on usage\n"
		"[-v], [--version]: print version information\n"
		"[-b], [--batch]: create a batch file example\n"
		"[-d], [--docs]: create module documentation files in current working directory\n"
#ifdef _OPENMP
		"[-c], [--cores]: number of physical processors to use for computation\n"
#endif
		"[-f], [--flags]: various flags for general usage\n"
		"  q: no progress report\n"
		"  r: no messages report\n"
		"  s: silent mode (no progress and no messages report)\n"
		"  i: allow user interaction\n"
		"  l: load translation dictionary\n"
		"  p: load projections dictionary\n"
		"  x: use XML markups for synopses and messages\n"
		"<LIBRARY>\t"	": name of the library\n"
		"<MODULE>\t"	": either name or index of the module\n"
		"<SCRIPT>\t"	": saga cmd script file with one or more module calls\n"
		"\n"
		"example:\n"
		"  saga_cmd -f=sp ta_morphometry \n"
		"           \"Local Morphometry\"\n"
		"           -ELEVATION c:\\dem.sgrd\n"
		"           -SLOPE     d:\\slope.sgrd\n"
		"           -ASPECT    d:\\aspect.sgrd\n"
		"           -METHOD    1\n"
		"\n"
		"_____________________________________________\n"
		"Module libraries are expected to be in the SAGA installation\n"
		"directory or its \'modules\' subdirectory. If this is not found\n"
		"the current working directory will be searched for instead.\n"
		"Alternatively you can add the environment variable \'SAGA_MLB\'\n"
		"and let it point to the desired directory.\n"
		"\n"
		"SAGA CMD is particularly useful for the automated\n"
		"execution of a series of analysis steps, because it\n"
		"allows you to execute modules using batch files.\n"
		"Calling saga_cmd with the option \'-b\' or \'--batch\'\n"
		"creates a batch file example. You probably have to edit\n"
		"the path definitions to make the batch file run on your\n"
		"computer.\n"
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Example	(void)
{
	CSG_String	CWD(&wxGetCwd());
	CSG_File	Stream;

	CMD_Print(_TL("creating batch file example"));

	if( !Stream.Open(SG_File_Make_Path(CWD, SG_T("saga_cmd_example"), SG_T("bat")), SG_FILE_W, false) )
	{
		CMD_Print(_TL("failed"));

		return;
	}

	Stream.Printf(CSG_String(
		"@ECHO OFF\n"
		"\n"
		"SET FLAGS=-f=q\n"
		"REM SET SAGA=.\n"
		"REM SET SAGA_MLB=%%SAGA%%\\modules\n"
		"REM PATH=PATH;%%SAGA%%;%%SAGA_MLB%%\n"
		"\n"
		"IF EXIST dem.sgrd GOTO :GO\n"
		"IF EXIST srtm.tif GOTO :SRTM\n"
		"ECHO _____________________________________________\n"
		"ECHO create a Gaussian landscape\n"
		"saga_cmd %%FLAGS%% recreations_fractals 5 -GRID=dem.sgrd -NX=400 -NY=400 -H=0.75\n"
		"GOTO :GO\n"
		"\n"
		":SRTM\n"
		"ECHO _____________________________________________\n"
		"ECHO import and project srtm (geotiff)\n"
		"saga_cmd %%FLAGS%% io_gdal              0 -FILES=srtm.tif -GRIDS=srtm -TRANSFORM\n"
		"saga_cmd %%FLAGS%% pj_proj4             7 -SOURCE=srtm.sgrd -TARGET_TYPE=0 -GET_USER_GRID=dem.sgrd -GET_USER_SIZE=1000.0 -SOURCE_PROJ=\"+proj=longlat +datum=WGS84\" -TARGET_PROJ=\"+proj=utm +zone=32 +datum=WGS84\""
		"\n"
		":GO\n"
		"ECHO _____________________________________________\n"
		"ECHO create contour lines from DEM\n"
		"saga_cmd %%FLAGS%% shapes_grid          5 -INPUT=dem.sgrd -CONTOUR=contour.shp -ZSTEP=25.0\n"
		"\n"
		"ECHO _____________________________________________\n"
		"ECHO do some terrain analysis\n"
		"saga_cmd %%FLAGS%% ta_preprocessor      2 -DEM=dem.sgrd -DEM_PREPROC=dem.sgrd\n"
		"saga_cmd %%FLAGS%% ta_lighting          0 -ELEVATION=dem.sgrd -SHADE=shade.sgrd -METHOD=0 -AZIMUTH=-45 -DECLINATION=45\n"
		"saga_cmd %%FLAGS%% ta_morphometry       0 -ELEVATION=dem.sgrd -SLOPE=slope.sgrd -ASPECT=aspect.sgrd -HCURV=hcurv.sgrd -VCURV=vcurv.sgrd\n"
		"saga_cmd %%FLAGS%% ta_hydrology         0 -ELEVATION=dem.sgrd -CAREA=carea.sgrd\n"
		"\n"
		"ECHO _____________________________________________\n"
		"ECHO run saga cmd script\n"
		"SET INPUT=dem.sgrd;slope.sgrd;hcurv.sgrd;vcurv.sgrd\n"
		"saga_cmd %%FLAGS%% saga_cmd_example.txt\n"
		"\n"
		"PAUSE\n"
	));

	if( !Stream.Open(SG_File_Make_Path(CWD, SG_T("saga_cmd_example"), SG_T("txt")), SG_FILE_W, false) )
	{
		CMD_Print(_TL("failed"));

		return;
	}

	Stream.Printf(CSG_String(
		"REM \'REM\' or \'#\' can be used for comments, \'ECHO\' for message output.\n"
		"REM environment variables can be accessed using the ms-dos/window style\n"
		"\n"
		"ECHO _____________________________________________\n"
		"ECHO cluster analysis and vectorisation\n"
		"imagery_classification 1 -GRIDS=%%INPUT%% -CLUSTER=cluster.sgrd -NORMALISE -NCLUSTER=5\n"
		"grid_filter            6 -INPUT=cluster.sgrd -RADIUS=3\n"
		"shapes_grid            6 -GRID=cluster.sgrd -POLYGONS=cluster.shp -CLASS_ALL=1\n"
		"shapes_tools           3 -SHAPES=cluster.shp -FIELD=ID -EXPRESSION=\"a = 1\"\n"
		"shapes_tools           6 -INPUT=cluster.shp -OUTPUT=cluster_class1.shp\n"
		"ECHO ready\n"
	));

	CMD_Print(_TL("okay"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Docs		(void)
{
	Print_Logo();

	if( Load_Libraries() )
	{
		CMD_Print(_TL("creating module documentation files"));

		CMD_Set_Show_Messages(false);

		SG_Get_Module_Library_Manager().Get_Summary(wxGetCwd().wx_str());

		CMD_Print(_TL("okay"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
