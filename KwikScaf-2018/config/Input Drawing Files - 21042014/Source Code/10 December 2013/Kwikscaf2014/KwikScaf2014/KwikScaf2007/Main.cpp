//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.

#include "stdafx.h"

#include "Component.h"
#include "MeccanoDefinitions.h"
#include "Main.h"
#include "Controller.h"
#include "AutoBuildTools.h"
#include <geassign.h>
#include "EntityReactor.h"
#include "Entity.h"
#include "MdiAware.h"
#include "ScaffoldArchive.h"
#include "Security.h"
#include "HelpAboutDlg.h"


#ifdef	USE_LINE_CLASS
#include "Line.h"
#endif	//#ifdef	USE_LINE_CLASS

// ****************************************************************
//  GLOBAL VARIABLE!!!  *******************************************
//  gpAsdkAppDocGlobals *******************************************
// ****************************************************************
AsdkAppDocGlobals			*gpAsdkAppDocGlobals; 
AsdkMeccanoEditorReactor	*gpMeccanoEditorReactor;
bool						 gbOpenFileDialogActive;
Controller					*gpController;
ScaffoldArchive				*gpScaffoldArchive;
Security					*gpSecurity;
bool						bCreatingLapboards;
bool						bCreatingTieTube;
HelpAboutDlg				*g_pSplashScreen;
CTime						m_StartTime;
HINSTANCE					_hdllInstance;
double						STAR_SEPARATION;
double						JACK_LENGTH;
double						RL_ADJUST;
double						STAR_SEPARATION_APPROX;
double						JACK_LENGTH_APPROX;
double						RL_ADJUST_APPROX;
double						EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
double						EXTRA_LENGTH_AT_TOP_OF_STANDARD;

#define	ALLOW_INDIVIDUAL_COMPONENTS

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								MAIN.CPP
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:32p $
//	Version			: $Revision: 116 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file is used as the initial entry point for the Dll
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(KWIKSCAF);


//////////////////////////////////////////////////////////////////////////////////////
//initApp
//This function is used to register commands with autocad
void initApp()
{
	STAR_SEPARATION			= STAR_SEPARATION_KWIKSTAGE;
	JACK_LENGTH				= STAR_SEPARATION;
	RL_ADJUST				= STAR_SEPARATION;
	STAR_SEPARATION_APPROX	= STAR_SEPARATION_APPROX_KWIKSTAGE;
	JACK_LENGTH_APPROX		= STAR_SEPARATION_APPROX;
	RL_ADJUST_APPROX		= STAR_SEPARATION_APPROX;
	EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD	= EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;
	EXTRA_LENGTH_AT_TOP_OF_STANDARD		= EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;


	bCreatingLapboards = false;
	bCreatingTieTube = false;

	// register a command with the AutoCAD command mechanism
	acedRegCmds->addCommand("KWIKSCAF", "Autobuild",				"AutoBuild",				ACRX_CMD_MODAL,								DoAutoBuild					);
	acedRegCmds->addCommand("KWIKSCAF", "Overrun",					"Overrun",					ACRX_CMD_TRANSPARENT,						Overrun						);

	//stage & level
	acedRegCmds->addCommand("KWIKSCAF", "Stages",					"Stages",					ACRX_CMD_TRANSPARENT,						SetStageForSelectedBays		);
	acedRegCmds->addCommand("KWIKSCAF", "AddLevel",					"AddLevel",					ACRX_CMD_TRANSPARENT,						AddLevel					);
	acedRegCmds->addCommand("KWIKSCAF", "RegenLevels",				"RegenLevels",				ACRX_CMD_TRANSPARENT,						RegenMatrixLevels			);
	acedRegCmds->addCommand("KWIKSCAF", "EnterLevels",				"EnterLevels",				ACRX_CMD_TRANSPARENT,						EnterLevels					);

	//Level
	acedRegCmds->addCommand("KWIKSCAF", "ToggleLvlLength",			"ToggleLvlLength",			ACRX_CMD_TRANSPARENT,						ToggleLvlLength				);
	acedRegCmds->addCommand("KWIKSCAF", "ToggleFirstScale",			"ToggleFirstScale",			ACRX_CMD_TRANSPARENT,						ToggleFirstScale			);
	acedRegCmds->addCommand("KWIKSCAF", "ToggleScaleLines",			"ToggleScaleLines",			ACRX_CMD_TRANSPARENT,						ToggleScaleLines			);
	acedRegCmds->addCommand("KWIKSCAF", "ColourByStageLevel",		"ColourByStageLevel",		ACRX_CMD_TRANSPARENT,						ColourByStageLevel			);

	//BOM	
	acedRegCmds->addCommand("KWIKSCAF", "BOM",						"BOM",						ACRX_CMD_TRANSPARENT,						BOM							);
	acedRegCmds->addCommand("KWIKSCAF", "BOMS",						"BOMS",						ACRX_CMD_TRANSPARENT,						BOMSummary					);
	acedRegCmds->addCommand("KWIKSCAF", "BOMExtra",					"BOMExtra",					ACRX_CMD_TRANSPARENT,						BOMExtraDisplay				);

	//Views
	acedRegCmds->addCommand("KWIKSCAF", "RedrawSchematic",			"RedrawSchematic",			ACRX_CMD_TRANSPARENT,						Redraw						);
	acedRegCmds->addCommand("KWIKSCAF", "UseActualComps",			"UseActualComps",			ACRX_CMD_TRANSPARENT,						UseActual3DComps			);
	acedRegCmds->addCommand("KWIKSCAF", "ClearActualComps",			"ClearActualComps",			ACRX_CMD_TRANSPARENT,						ClearActual3DComps			);
	acedRegCmds->addCommand("KWIKSCAF", "Create3DView",				"Create3DView",				ACRX_CMD_TRANSPARENT,						Create3DView				);
	acedRegCmds->addCommand("KWIKSCAF", "Delete3DView",				"Delete3DView",				ACRX_CMD_TRANSPARENT,						Delete3DView				);
	acedRegCmds->addCommand("KWIKSCAF", "CreateMatrix",				"CreateMatrix",				ACRX_CMD_TRANSPARENT,						CreateMatrix				);
	acedRegCmds->addCommand("KWIKSCAF", "DeleteMatrix",				"DeleteMatrix",				ACRX_CMD_TRANSPARENT,						DeleteMatrix				);
	acedRegCmds->addCommand("KWIKSCAF", "Kut",						"Kut",						ACRX_CMD_TRANSPARENT,						CreateCutThrough			);
	acedRegCmds->addCommand("KWIKSCAF", "ResetKutSuggest",			"ResetKutSuggest",			ACRX_CMD_TRANSPARENT,						ResetCutThroughSuggest		);

	//Component Details/Stock Details
	acedRegCmds->addCommand("KWIKSCAF", "CCDFN",					"CCDFN",					ACRX_CMD_TRANSPARENT,						ClearCompDetialsFilename	);
	acedRegCmds->addCommand("KWIKSCAF", "CSDFN",					"CSDFN",					ACRX_CMD_TRANSPARENT,						ClearStockDetailsFilename	);
	acedRegCmds->addCommand("KWIKSCAF", "CDDFN",					"CDDFN",					ACRX_CMD_TRANSPARENT,						ClearDrawingDetailsFilename	);

	//Edit functions
	acedRegCmds->addCommand("KWIKSCAF", "EditBay",					"EditBay",					ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	EditBay						);
	acedRegCmds->addCommand("KWIKSCAF", "EditRun",					"EditRun",					ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	EditRun						);
	acedRegCmds->addCommand("KWIKSCAF", "ResizeBay",				"ResizeBay",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	ResizeBay					);
	acedRegCmds->addCommand("KWIKSCAF", "InsertBay",				"InsertBay",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	InsertBay					);
	acedRegCmds->addCommand("KWIKSCAF", "StairBay",					"StairBay",					ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	StairBay					);
	acedRegCmds->addCommand("KWIKSCAF", "LadderBay",				"LadderBay",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	LadderBay					);
	acedRegCmds->addCommand("KWIKSCAF", "ButtressBay",				"ButtressBay",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	ButtressBay					);
	acedRegCmds->addCommand("KWIKSCAF", "ChangeRLs",				"ChangeRLs",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	ChangeRLs					);
#ifdef	ALLOW_INDIVIDUAL_COMPONENTS
	acedRegCmds->addCommand("KWIKSCAF", "EditComponents",			"EditComponents",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	EditComponent				);
	//See bug #1647 - CR not working
	//acedRegCmds->addCommand("KWIKSCAF", "RotateComponents",			"RotateComponents",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RotateComponent				);
	acedRegCmds->addCommand("KWIKSCAF", "CopyComponents",			"CopyComponents",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	CopyComponent				);
	//acedRegCmds->addCommand("KWIKSCAF", "MoveComponents",			"MoveComponents",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	MoveComponent				);
#endif	//#ifdef	ALLOW_INDIVIDUAL_COMPONENTS

	//adding / removing components
	acedRegCmds->addCommand("KWIKSCAF", "AddBracing",				"AddBracing",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddBracing					);
	acedRegCmds->addCommand("KWIKSCAF", "AddChainLink",				"AddChainLink",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddChainLink				);
	acedRegCmds->addCommand("KWIKSCAF", "AddHandrail",				"AddHandrail",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddHandrail					);
	acedRegCmds->addCommand("KWIKSCAF", "AddHopups",				"AddHopups",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddHopups					);
	acedRegCmds->addCommand("KWIKSCAF", "AddLapboard",				"AddLapboard",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddLapboard					);
	acedRegCmds->addCommand("KWIKSCAF", "AddShadeCloth",			"AddShadeCloth",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddShadeCloth				);
	acedRegCmds->addCommand("KWIKSCAF", "AddTie",					"AddTie",					ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddTie						);
	acedRegCmds->addCommand("KWIKSCAF", "AddTube",					"AddTube",					ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	AddTieTube					);
	acedRegCmds->addCommand("KWIKSCAF", "AddCornerStageBoard",		"AddCornerStageBoard",		ACRX_CMD_TRANSPARENT,						AddCornerStageBoard			);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveBracing",			"RemoveBracing",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveBracing				);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveChainLink",			"RemoveChainLink",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveChainLink				);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveHandrail",			"RemoveHandrail",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveHandrail				);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveHopups",				"RemoveHopups",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveHopups				);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveShadeCloth",			"RemoveShadeCloth",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveShadeCloth			);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveTies",				"RemoveTies",				ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveTies					);
	acedRegCmds->addCommand("KWIKSCAF", "RemoveCornerStageBoard",	"RemoveCornerStageBoard",	ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	RemoveCornerStageBoard		);
#ifdef	ALLOW_INDIVIDUAL_COMPONENTS
	acedRegCmds->addCommand("KWIKSCAF", "InsertComponent",			"InsertComponent",			ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	InsertComponent				);
#endif	//#ifdef	ALLOW_INDIVIDUAL_COMPONENTS

	//Mills/Kwikstage
	acedRegCmds->addCommand("KWIKSCAF", "UseMills",					"UseMills",					ACRX_CMD_TRANSPARENT,						UseMills					);
	acedRegCmds->addCommand("KWIKSCAF", "UseKwikStage",				"UseKwikStage",				ACRX_CMD_TRANSPARENT,						UseKwikstage					);
	acedRegCmds->addCommand("KWIKSCAF", "ChangeSystem",				"ChangeSystem",				ACRX_CMD_TRANSPARENT,						ChangeSystem				);

	//Mills System
	acedRegCmds->addCommand("KWIKSCAF", "MillsSystemCnr",			"MillsSystemCnr",			ACRX_CMD_TRANSPARENT,						MillsSystemCorner			);
	acedRegCmds->addCommand("KWIKSCAF", "ConvertToMillsCorner",		"ConvertToMillsCorner",		ACRX_CMD_TRANSPARENT,						ConvertToMillsCorner		);
	acedRegCmds->addCommand("KWIKSCAF", "UnconvertMillsCorner ",	"UnconvertMillsCorner",		ACRX_CMD_TRANSPARENT,						UnconvertMillsCorner		);

	//Sydney Corners
	acedRegCmds->addCommand("KWIKSCAF", "SydneyCnr",				"SydneyCnr",				ACRX_CMD_TRANSPARENT,						SydneyCorner				);
	acedRegCmds->addCommand("KWIKSCAF", "SydneyCnrSeparation",		"SydneyCnrSeparation",		ACRX_CMD_TRANSPARENT,						SydneyCornerSeparation		);

	//Hide Matrix
	acedRegCmds->addCommand("KWIKSCAF", "ShowEntireMatrix",			"ShowEntireMatrix",			ACRX_CMD_TRANSPARENT,						ShowEntireMatrix			);
	acedRegCmds->addCommand("KWIKSCAF", "HideMatrixSections",		"HideMatrixSections",		ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	HideMatrixSections			);

	//Misc
	acedRegCmds->addCommand("KWIKSCAF", "JobDetail",				"JobDetail",				ACRX_CMD_TRANSPARENT,						JobDetail					);
	acedRegCmds->addCommand("KWIKSCAF", "DoCleanUp",				"DoCleanUp",				ACRX_CMD_TRANSPARENT,						DoCleanUp					);
	acedRegCmds->addCommand("KWIKSCAF", "SetArcLapSpan",			"SetArcLapSpan",			ACRX_CMD_TRANSPARENT,						SetArcLapSpan				);
	acedRegCmds->addCommand("KWIKSCAF", "ClearCommitted",			"ClearCommitted",			ACRX_CMD_TRANSPARENT,						ClearCommitted				);
	acedRegCmds->addCommand("KWIKSCAF", "EditStandards",			"EditStandards",			ACRX_CMD_TRANSPARENT,						EditStandards				);
	acedRegCmds->addCommand("KWIKSCAF", "HCT",						"HCT",						ACRX_CMD_TRANSPARENT,						HideComponentTypes			);
	acedRegCmds->addCommand("KWIKSCAF", "ChangeColours",			"ChangeColours",			ACRX_CMD_TRANSPARENT,						HideComponentTypes			);
	acedRegCmds->addCommand("KWIKSCAF", "UseToeboardOnLapboard",	"UseToeboardOnLapboard",	ACRX_CMD_TRANSPARENT,						UseToeboardOnLapboard		);
	acedRegCmds->addCommand("KWIKSCAF", "UsePutLogClipsInLadder",	"UsePutLogClipsInLadder",	ACRX_CMD_TRANSPARENT,						UsePutLogClipsInLadder		);
	acedRegCmds->addCommand("KWIKSCAF", "WOLH",						"WOLH",						ACRX_CMD_TRANSPARENT,						WallOffsetFromLowestHopup	);
	acedRegCmds->addCommand("KWIKSCAF", "AboutKwikscaf",			"AboutKwikscaf",			ACRX_CMD_TRANSPARENT,						AboutKwikscaf				);
	acedRegCmds->addCommand("KWIKSCAF", "Use1500Soleboards",		"Use1500Soleboards",		ACRX_CMD_TRANSPARENT,						Use1500Soleboards			);
	acedRegCmds->addCommand("KWIKSCAF", "SMSLOS",					"SMSLOS",					ACRX_CMD_TRANSPARENT,						ShowMatrixSectLabelsOnSch	);
	acedRegCmds->addCommand("KWIKSCAF", "ShowStandardLengths",		"ShowStandardLengths",		ACRX_CMD_TRANSPARENT,						ShowStandardLengths			);
	acedRegCmds->addCommand("KWIKSCAF", "ShowZLine",				"ShowZLine",				ACRX_CMD_TRANSPARENT,						DisplayZigZagLine			);
	acedRegCmds->addCommand("KWIKSCAF", "AllComponentsVisible",		"AllComponentsVisible",		ACRX_CMD_TRANSPARENT,						AllComponentsVisible		);
	acedRegCmds->addCommand("KWIKSCAF", "BaysInBOMS",				"BaysInBOMS",				ACRX_CMD_TRANSPARENT,						ShowBaysInBOMS				);
	acedRegCmds->addCommand("KWIKSCAF", "BaySizesInBOMS",			"BaySizesInBOMS",			ACRX_CMD_TRANSPARENT,						ShowBaySizesInBOMS			);
	acedRegCmds->addCommand("KWIKSCAF", "EditStock",				"EditStock",				ACRX_CMD_TRANSPARENT,						EditStock					);
	acedRegCmds->addCommand("KWIKSCAF", "CalcPerRoll",				"CalcPerRoll",				ACRX_CMD_TRANSPARENT,						CalcPerRoll					);

	//Hidden/Secret functions
	acedRegCmds->addCommand("KWIKSCAF", "TC",						"TC",						ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	TraceContents				);
	acedRegCmds->addCommand("KWIKSCAF", "TVC",						"TVC",						ACRX_CMD_TRANSPARENT|ACRX_CMD_USEPICKSET,	TraceVisualComponents		);
	acedRegCmds->addCommand("KWIKSCAF", "DS",						"DS",						ACRX_CMD_TRANSPARENT,						DeleteSchematic				);
	acedRegCmds->addCommand("KWIKSCAF", "SubT4L",					"SubT4L",					ACRX_CMD_TRANSPARENT,						SwapLedgerAndTransoms		);
	acedRegCmds->addCommand("KWIKSCAF", "DisplaySalePrice",			"DisplaySalePrice",			ACRX_CMD_TRANSPARENT,						DisplaySalePrice			);
	acedRegCmds->addCommand("KWIKSCAF", "ShowSavedHistory",			"ShowSavedHistory",			ACRX_CMD_TRANSPARENT,						ShowSavedHistory			);
	acedRegCmds->addCommand("KWIKSCAF", "ClearSavedHistory",		"ClearSavedHistory",		ACRX_CMD_TRANSPARENT,						ClearSavedHistory			);
	acedRegCmds->addCommand("KWIKSCAF", "CnvrtSystemOnLoad",		"CnvrtSystemOnLoad",		ACRX_CMD_TRANSPARENT,						CnvrtSystemOnLoad			);
	acedRegCmds->addCommand("KWIKSCAF", "ShowStockList",			"ShowStockList",			ACRX_CMD_TRANSPARENT,						ShowStockList				);
	acedRegCmds->addCommand("KWIKSCAF", "StoreStockList",			"StoreStockList",			ACRX_CMD_TRANSPARENT,						StoreStockList				);
	acedRegCmds->addCommand("KWIKSCAF", "KwikRLs",					"KwikRLs",					ACRX_CMD_TRANSPARENT,						SetRLs						);
	acedRegCmds->addCommand("KWIKSCAF", "DrawKwikRLs",				"DrawKwikRLs",				ACRX_CMD_TRANSPARENT,						DrawKwikRLs					);
	acedRegCmds->addCommand("KWIKSCAF", "FindRLs",					"FindRLs",					ACRX_CMD_TRANSPARENT,						FindRLs						);
	acedRegCmds->addCommand("KWIKSCAF", "KwikRegen",				"KwikRegen",				ACRX_CMD_TRANSPARENT,						KwikRegen					);

	AcMemFile		::rxInit();
    EntityReactor	::rxInit();
    Entity			::rxInit();
	Component		::rxInit();
	AsdkDbReactor	::rxInit();
#ifdef	USE_LINE_CLASS
	Line			::rxInit();
#endif	//#ifdef	USE_LINE_CLASS

    acrxBuildClassHierarchy();
}

//////////////////////////////////////////////////////////////////////////////////////
// BOMSummary
//
void BOMSummary()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->DoBOMSummary();
	}
}

//////////////////////////////////////////////////////
//TextController
//Tests the controller is valid and available
bool TestController()
{
	CString	sMsg;

	if( gpController==NULL )
		return false;

	if( !gpController->IsComponentDetailsLoaded() )
	{
		sMsg = "Cannot proceed without valid ComponentDetials file.\n\nPlease restart Autocad to reload file.";
		MessageBox( NULL, sMsg, "STOP! Error", MB_OK );
		return false;
	}

	if( !gpController->IsStockFileLoaded() )
	{
		sMsg = "Cannot proceed without valid Stock file.\n\nPlease restart Autocad to reload file.";
		MessageBox( NULL, sMsg, "STOP! Error", MB_OK );
		return false;
	}

  /* 
	if( !gpSecurity->QuerySecurity() )
		return false;
  */

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//DoAutoBuild
//initiates the Autobuild command
void DoAutoBuild( )
{
	if(TestController())
	{
		TrapMouse();

		giAutobuildNumber++;
		assert( gpController!=NULL );
		gpController->DoAutoBuild();

		UnTrapMouse();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DoAutoBuild
//initiates the Autobuild command
void AboutKwikscaf( )
{
	HelpAboutDlg Dialog;
	if( TestController() )
	{
#ifndef	_DEBUG
		Dialog.SetHardwareLockID( gpSecurity->GetSerialNumber() );
#endif	//#ifndef	_DEBUG
		Dialog.DoModal();
	}
	else
	{
		Dialog.SetShowHardwareKeyMsg(true);
		Dialog.DoModal();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ResetCutThroughSuggest
//Initiates the Autobuild command
void ResetCutThroughSuggest( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ResetLabelCount();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//Use1500Soleboards
//Changes the ability to use 1.5m soleboards
void Use1500Soleboards( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->Use1500Soleboards();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//SwapLedgerAndTransoms
//Allows the user to set if the 1200 and 700 transom and ledgers are substitued
void SwapLedgerAndTransoms( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ChangeUse0700TransomForLedger();
		gpController->ChangeUse1200TransomForLedger();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowMatrixSectLabelsOnSch
//Changes the ability to create labels for matrix sections on schematic
void ShowMatrixSectLabelsOnSch( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ChangeShowMatrixLabelsOnSchematic();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowStandardLengths
//Changes the ability to display the length of each standard 1/2 way up its length
void ShowStandardLengths( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ChangeShowStandardLengths();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DisplayZigZagLine
//Changes the ability to display the Z line
void DisplayZigZagLine( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->DisplayZigZagLine();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//AllComponentsVisible
//Changes the Visible flag on every visible component
void AllComponentsVisible( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->SetAllComponentsVisible();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowBaysInBOMS
//Changes the ability to display the Bay numbers in the BOMS
void ShowBaysInBOMS( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ShowBaysInBOMS();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowBaySizesInBOMS
//Changes the ability to display the quantity versus Bay dimensionin the BOMS
void ShowBaySizesInBOMS( )
{
	if( TestController() )
	{
		assert( gpController!=NULL );
		gpController->ShowBaySizesInBOMS();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//TraceContents
//Traces the contents of a bay!
void TraceContents( )
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->TraceContents();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//TraceVisualComponents
//lists all the visual components
void TraceVisualComponents( )
{
	if (TestController())
	{
		assert( gpController!=NULL );
		int iCount, i, j;
		Component *pComponent;
		acutPrintf( "\nThere are %i visual components:\n", gpController->GetVisualComponents()->GetNumberOfComponents() );
		for( i=CT_DECKING_PLANK; i<=CT_VISUAL_COMPONENT; i++ )
		{
			iCount = 0;
			for( j=0; j<gpController->GetVisualComponents()->GetNumberOfComponents(); j++ )
			{
				pComponent=gpController->GetVisualComponents()->GetComponent(j);
				if( pComponent->GetType()==(ComponentTypeEnum)i )
				{
					iCount++;
				}
			}
			if( iCount>0 )
			{
				acutPrintf( "%i x %s\n", iCount, GetComponentDescStr( (ComponentTypeEnum)i ) );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DeleteSchematic
//Traces the contents of a bay!
void DeleteSchematic( )
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ChangeDeleteSchematic();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//ResizeBay
//Resizes a bay
void ResizeBay( )
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ResizeBay();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//Destroy the controller
void DestroyController()
{
	// this is done now in the mdi aware class AsdkPerDocData
}

//////////////////////////////////////////////////////////////////////////////////////
// SetStageForSelectedBays
// selects and assigns stages to bays and levels
void SetStageForSelectedBays()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->SetStageForSelectedBays();
}

//////////////////////////////////////////////////////////////////////////////////////
//BOM
//creates the BOM
void BOM()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->BOM();
}

//////////////////////////////////////////////////////////////////////////////////////
//BOMExtraDisplay
//creates the BOMExtraDisplay
void BOMExtraDisplay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DisplayBOMExtra();
}

//////////////////////////////////////////////////////////////////////////////////////
//JobDetail
//creates the JobDetail
void JobDetail()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DisplayJobDetail();
}

//////////////////////////////////////////////////////////////////////////////////////
//Create3DView
//creates the 3D View
void Create3DView()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->Create3DView();
}

//////////////////////////////////////////////////////////////////////////////////////
//Delete3DView
//Deletes the 3D View
void Delete3DView()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->Delete3DView();
}

//////////////////////////////////////////////////////////////////////////////////////
//CreateMatrix
//creates the 3D View
void CreateMatrix()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->CreateMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////
//DeleteMatrix
//Deletes the 3D View
void DeleteMatrix()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////
//CreateCutThrough
//creates the 3D View for a birdcage
void CreateCutThrough()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->CreateCutThrough();
}


//////////////////////////////////////////////////////////////////////////////////////
//ClearComponentsDetialsFilename
//Resets the ComponentDetails.csv filename in the registry
void ClearCompDetialsFilename()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->GetCompDetails()->SetFilenameInRegistry( "" );
}

//////////////////////////////////////////////////////////////////////////////////////
//ClearStockDetailsFilename
//Resets the Stock.csv filename in the registry
void ClearStockDetailsFilename()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->GetStockList()->ClearFilenameInRegistry(true);
}

//////////////////////////////////////////////////////////////////////////////////////
//ClearDrawingDetailsFilename	
//Resets the DrawingDetails.csv filename in the registry
void ClearDrawingDetailsFilename()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->GetStockList()->ClearFilenameInRegistry(false);
}

//////////////////////////////////////////////////////////////////////////////////////
//Overrun
//Deletes the 3D View
void Overrun()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->SetOverrun( !gpController->GetOverrun() );
}

//////////////////////////////////////////////////////////////////////////////////////
//Editbay
//Edits a bay or a group of bays
void EditBay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->EditBay();
}

//////////////////////////////////////////////////////////////////////////////////////
//EditRuns
//Edits a Run
void EditRun()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->EditRun();
}

//////////////////////////////////////////////////////////////////////////////////////
//Overrun
//Deletes the 3D View
void InsertBay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->InsertBay();
}

//////////////////////////////////////////////////////////////////////////////////////
//UseActual3DComps
//Use the dwg files to draw all 3d components
void UseActual3DComps()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->UseActualComponents();
}

//////////////////////////////////////////////////////////////////////////////////////
//ClearActual3DComps
//Clears all the entity value for actual components
void ClearActual3DComps()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->ClearActualComponents();
}



//////////////////////////////////////////////////////////////////////////////////////
//DoCleanUp
//Use the dwg files to draw all 3d components
void DoCleanUp()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->CleanUp( );
}

//////////////////////////////////////////////////////////////////////////////////////
//StairBay
//Use the dwg files to draw all 3d components
void StairBay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->StairBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//LadderBay
//Use the dwg files to draw all 3d components
void LadderBay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->LadderBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//ButtressBay
//Use the dwg files to draw all 3d components
void ButtressBay()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->ButtressBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddLevel
//Adds a level seperator on the first available star above or at RL0
void AddLevel()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->AddLevel( );
}


//////////////////////////////////////////////////////////////////////////////////////
//RegenMatrixLevels
//Adds a level seperator on the first available star above or at RL0
void RegenMatrixLevels()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->RegenMatrixLevels( );
}


//////////////////////////////////////////////////////////////////////////////////////
//EnterLevels
//Allows the user to type in levels directly at the command line EnterLevels
void EnterLevels()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->EnterLevels( );
}



//////////////////////////////////////////////////////////////////////////////////////
//UseMills
//Adds a level seperator on the first available star above or at RL0
void UseMills()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->SetSystem( S_MILLS );
}


//////////////////////////////////////////////////////////////////////////////////////
//SetArcLapSpan
//Adds a level seperator on the first available star above or at RL0
void SetArcLapSpan()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->SetArcLapSpan();
}

//////////////////////////////////////////////////////////////////////////////////////
//UseKwikstage
//Adds a level seperator on the first available star above or at RL0
void UseKwikstage()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->SetSystem( S_KWIKSTAGE );
}

//////////////////////////////////////////////////////////////////////////////////////
//ChangeSystem
//Adds a level seperator on the first available star above or at RL0
void ChangeSystem()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->ChangeSystem();
}

//////////////////////////////////////////////////////////////////////////////////////
//AddTie
//Use the dwg files to draw all 3d components
void AddTie()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddTieToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddBracing
//Use the dwg files to draw all 3d components
void AddBracing()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddBracingToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveBracing
//Use the dwg files to draw all 3d components
void RemoveBracing()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteBracingFromBays( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveTies
//Use the dwg files to draw all 3d components
void RemoveTies()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteTiesFromBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveChainLink
//Use the dwg files to draw all 3d components
void RemoveChainLink()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteChainLinkFromBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveShadeCloth
//Use the dwg files to draw all 3d components
void RemoveShadeCloth()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteShadeClothFromBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddHandrails
void AddHandrail()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddHandrailsToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddChainLink
void AddChainLink()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddChainLinkToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddHandrails
void AddShadeCloth()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddShadeClothToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveHandrail
void RemoveHandrail()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteHandrails( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddHopups
void AddHopups()
{
	assert( gpController!=NULL );
	if( TestController() )
		gpController->AddHopupsToBay( );
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveTies
//Use the dwg files to draw all 3d components
void RemoveHopups()
{
	assert( gpController!=NULL );
	if (TestController())
		gpController->DeleteHopups( );
}

//////////////////////////////////////////////////////////////////////////////////////
//AddLapboard
//Use the dwg files to draw all 3d components
void AddLapboard()
{
	if (TestController())
	{
		bCreatingLapboards = true;
		TrapMouse();
		assert( gpController!=NULL );
		gpController->GetABTools()->SetAccessEnabled( true );
		gpController->GetABTools()->CreateLapboard( );
		gpController->GetABTools()->SetAccessEnabled( false );
		UnTrapMouse();
		bCreatingLapboards = false;
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//AddTieTube
//Use the dwg files to draw all 3d components
void AddTieTube()
{
	if (TestController())
	{
		bCreatingTieTube = true;
		TrapMouse();
		assert( gpController!=NULL );
		gpController->GetABTools()->SetAccessEnabled( true );
		gpController->GetABTools()->CreateTieTube( );
		gpController->GetABTools()->SetAccessEnabled( false );
		UnTrapMouse();
		bCreatingTieTube = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//AddCornerStageBoard
//
void AddCornerStageBoard()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->AddCornerStageBoard();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//RemoveCornerStageBoard
//
void RemoveCornerStageBoard()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->RemoveCornerStageBoard();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//Redraw
//Use the dwg files to draw all 3d components
void Redraw()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->RedrawSchematic(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ChangeRLs
//Use the dwg files to draw all 3d components
void ChangeRLs()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ChangeRLs();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//EditComponent
//Edits individual components
void EditComponent()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ComponentsEdit();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//EditStock
//Add/Edit or remove KwikScaf Stock Compoenents
void EditStock()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->EditStock();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//MoveComponent
//Moves individual components
void MoveComponent()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ComponentMove();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//RotateComponent
//Rotates individual components
void RotateComponent()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ComponentRotate();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//CopyComponent
//Copys individual components
void CopyComponent()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ComponentCopy();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//InsertComponent
//Inserts an individual components
void InsertComponent()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ComponentInsert();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ToggleLvlLength
//Use the long or the short level line
void ToggleLvlLength()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ToggleLvlLength();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ToggleFirstScale
//Show the first scale or not
void ToggleFirstScale()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ToggleFirstScale();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ToggleScaleLines
//Show the 2.0m scale line or not
void ToggleScaleLines()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ToggleScaleLines();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ClearCommitted
//Clear the committed flag from all components
void ClearCommitted()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->ClearCommitted();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//MillsSystemCorner
//Clear the committed flag from all components
void MillsSystemCorner()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->MillsSystemCorner();
	}
	acedRetInt( gpController->IsMillsSystemCorner()? 1: 0 );
}


//////////////////////////////////////////////////////////////////////////////////////
//ConvertToMillsCorner
//Converts a non-stair/ladder bay into mills corner bay
void ConvertToMillsCorner()
{
	if (TestController())
	{
		gpController->ConvertToMillsCorner();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//UnconvertMillsCorner
//converts a bay from a mills system corner bay to a normal bay
void UnconvertMillsCorner()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->UnconvertMillsCorner();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//SydneyCorner
//Clear the committed flag from all components
void SydneyCorner()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SydneyCorner();
	}
	acedRetInt( gpController->IsSydneyCorner()? 1: 0 );
}

//////////////////////////////////////////////////////////////////////////////////////
//SydneyCornerSeparation
//Clear the committed flag from all components
void SydneyCornerSeparation()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SydneyCornerSeparation();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//EditStandards
//Clear the committed flag from all components
void EditStandards()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->EditStandards();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowEntireMatrix
//Clear the committed flag from all components
void ShowEntireMatrix()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->ShowAllMatrixElements();
		gpController->CreateMatrix(true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//HideMatrixSections
//Clear the committed flag from all components
void HideMatrixSections()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->ExcludeMatrixElements();
		gpController->CreateMatrix(true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//HideMatrixSections
//Clear the committed flag from all components
void HideComponentTypes()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SelectHiddenComponentTypes();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ColourByStageLevel
//Clear the committed flag from all components
void ColourByStageLevel()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->ColourByStageLevel();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DisplaySalePrice
//
void DisplaySalePrice()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->DisplaySalePrice();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DisplaySalePrice
//
void ShowSavedHistory()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SavedHistoryShow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DisplaySalePrice
//
void ClearSavedHistory()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SavedHistoryClear();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DisplaySalePrice
//
void CnvrtSystemOnLoad()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->SetConvertToSystemOnNextOpen(true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//ShowStockList
//
void ShowStockList()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->GetStockList()->TraceContents(true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//StoreStockList
//
void StoreStockList()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->GetStockList()->TraceContents(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//SetRLs
//
void SetRLs()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->SetRLs();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//DrawKwikRLs
//
void DrawKwikRLs()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->DrawKwikRLs();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//FindRLs
//
void FindRLs()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->ShowRLforPoint();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//KwikRegen
//
void KwikRegen()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		assert( gpController->GetStockList()!=NULL );
		gpController->UpdateAllVeiws();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//UseToeboardOnLapboard
//
void UseToeboardOnLapboard()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->UseToeboardOnLapboard();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//UsePutLogClipsInLadder
//
void UsePutLogClipsInLadder()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->UsePutLogClipsInLadder();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//WallOffsetFromLowestHopup
//
void WallOffsetFromLowestHopup()
{
	assert( gpController!=NULL );
	if (TestController())
	{
		gpController->WallOffsetFromLowestHopup();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//unloadApp
//used to deregister the command groups from autocad
void unloadApp()
{
	acedRegCmds->removeGroup("KWIKSCAF");
	DestroyController(); // this is done now in the mdi aware class AsdkPerDocData

    // Remove the AsdkObjectToNotify class from the ACRX
    // runtime class hierarchy. If this is done while the
    // database is still active, it should cause all objects
    // of class AsdkObjectToNotify to be turned into proxies.
    // 
	deleteAcRxClass(AcMemFile::desc());
	deleteAcRxClass(Component::desc());
    deleteAcRxClass(Entity::desc());
    deleteAcRxClass(EntityReactor::desc());
    deleteAcRxClass(AsdkDbReactor::desc());
#ifdef	USE_LINE_CLASS
    deleteAcRxClass(Line::desc());
#endif	//#ifdef	USE_LINE_CLASS
}


/////////////////////////////////////////////////////////////////////////////
// Entry points
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//acrxEntryPoint
//this function is the first point of call from autocad when this dll
// is initially loaded/unloaded
extern "C" AcRx::AppRetCode 
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	switch (msg) 
	{
		case AcRx::kInitAppMsg:
			gbOpenFileDialogActive = false;
			gpSecurity = NULL;
			gpController = NULL;
			gpScaffoldArchive = NULL;
			acrxDynamicLinker->unlockApplication(pkt);
			acrxRegisterAppMDIAware(pkt);
	        gpAsdkAppDocGlobals = new AsdkAppDocGlobals(curDoc()); 
		    gpAsdkAppDocGlobals->setGlobals(curDoc()); 
			gpMeccanoEditorReactor = new AsdkMeccanoEditorReactor();
			acedEditor->addReactor( gpMeccanoEditorReactor );
			initApp();  
			gpSecurity = new Security(acedGetAcadFrame());

			TrapMouse();
			m_StartTime = m_StartTime.GetCurrentTime();
			g_pSplashScreen = new HelpAboutDlg();
			g_pSplashScreen->SetModal(true);
#ifndef	_DEBUG
			g_pSplashScreen->SetHardwareLockID( gpSecurity->GetSerialNumber() );
#endif	//#ifndef	_DEBUG
			g_pSplashScreen->Create(IDD_HELP_ABOUT);
			g_pSplashScreen->ShowWindow( SW_SHOW );
			g_pSplashScreen->BeginModalState();
			g_pSplashScreen->SetFocus();


			break;
		case AcRx::kUnloadAppMsg:
			acedEditor->removeReactor( gpMeccanoEditorReactor );
			DELETE_PTR(gpMeccanoEditorReactor);
	        if (gpAsdkAppDocGlobals != NULL) 
			{ 
	            gpAsdkAppDocGlobals->unload(); 
				DELETE_PTR(gpAsdkAppDocGlobals); 
			} 

			unloadApp();
			DELETE_PTR(gpSecurity);
			break;
		default:
			break;
	}
	return AcRx::kRetOK;
}

//////////////////////////////////////////////////////////////////////////////////////
//DllMain
//This is the very first function called when the dll is initialised
//  but then again I guess you already knew that!
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved

	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_hdllInstance = hInstance;
        KWIKSCAF.AttachInstance(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
        KWIKSCAF.DetachInstance();  
	}
	
	return 1;   // ok
}

//////////////////////////////////////////////////////////////////////////////////
void TrapMouse()
{
	assert( gpController!=NULL );
	gpController->GetABTools()->SetMouseTrack(false);

    if (gbMouseDone == true) // already has the hook?
      return;
  
    if (acedRegisterFilterWinMsg(filterMouse) == false)
        acutPrintf("Can't register Windows Msg hook - VH - mouse\n"); 
    else
        gbMouseDone = true;
}

//////////////////////////////////////////////////////////////////////////////////
void UnTrapMouse()
{
    if (gbMouseDone == true)
    {
        acedRemoveFilterWinMsg(filterMouse); 
        gbMouseDone = false;
    }
}

BOOL filterMouse(MSG *pMsg)
{
	if( g_pSplashScreen!=NULL )
	{
		bool bDelete = false;
		if( pMsg->message==WM_MOUSEMOVE )
		{
			bDelete = true;
		}
		else if( pMsg->message==WM_TIMER )
		{
			int			iSeconds;
			CTime		CurrentTime;
			CTimeSpan	LengthOfTime;
			CurrentTime = CurrentTime.GetCurrentTime();

			LengthOfTime = CurrentTime-m_StartTime;
			iSeconds = LengthOfTime.GetSeconds();
			if( iSeconds>3 )
			{
				bDelete = true;
			}
		}

		if(bDelete)
		{
			g_pSplashScreen->ShowWindow( SW_HIDE );
			g_pSplashScreen->EndDialog(1);
			delete g_pSplashScreen;
			g_pSplashScreen = NULL;
			if( giAutobuildNumber<=0 )
				UnTrapMouse();
			assert( gpController!=NULL );
			gpController->RefreshScreen();
		}
	}

	assert( gpController!=NULL );
	if( bCreatingLapboards )
		return gpController->GetABTools()->filterMouseForLapboard( pMsg );

	if( bCreatingTieTube )
		return gpController->GetABTools()->filterMouseForTieTube( pMsg );

	return gpController->GetABTools()->filterMouse( pMsg );
}



//////////////////////////////////////////////////////////////////////////////////////
//CalcPerRoll
//Add/Edit or remove KwikScaf Stock Compoenents
void CalcPerRoll()
{
	if (TestController())
	{
		assert( gpController!=NULL );
		gpController->CalcPerRoll();
	}
}

