//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              ComponentDefinitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 5/10/00 8:14a $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/COMPONENTDEFINITIONS.H $
//	Version			: $Revision: 22 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	__COMPONENT_DEFINITIONS_H__
#define	__COMPONENT_DEFINITIONS_H__

#include "stdafx.h"
/////////////////////////////////////////////////////////////////////
//Component stuff
/////////////////////////////////////////////////////////////////////

enum ComponentTypeEnum
{
	CT_DECKING_PLANK,
	CT_LAPBOARD,
	CT_STAGE_BOARD,
	CT_LEDGER,
	CT_TRANSOM,
	CT_MESH_GUARD,
	CT_RAIL,
	CT_MID_RAIL,
	CT_TOE_BOARD,
	CT_CHAIN_LINK,
	CT_SHADE_CLOTH,
	CT_TIE_TUBE,
	CT_TIE_CLAMP_COLUMN,
	CT_TIE_CLAMP_MASONARY,
	CT_TIE_CLAMP_YOKE,
	CT_TIE_CLAMP_90DEGREE,
	CT_BRACING,
	CT_STAIR,
	CT_LADDER,
	CT_STANDARD,
	CT_STANDARD_OPENEND,
	CT_JACK,
	CT_SOLEBOARD,
	CT_HOPUP_BRACKET,
	CT_CORNER_STAGE_BOARD,
	CT_TEXT,
	CT_TIE_BAR,
	CT_STANDARD_CONNECTOR,
	CT_LADDER_PUTLOG,
	CT_STAIR_RAIL,
	CT_STAIR_RAIL_STOPEND,
	CT_TOE_BOARD_CLIP,
	CT_BOM_EXTRA,
	CT_PUTLOG_CLIP,
	CT_VISUAL_COMPONENT,
	CT_UNDEFINED	= -1
};

/////////////////////////////////////////////////////////////////////
//Component Points
enum ComponentPointsEnum
{
	CP_FIRST_POINT,		//first point
	CP_SECOND_POINT,	//second point
	CP_NUMBER_OF_POINTS	//array size
};

/////////////////////////////////////////////////////////////////////
//The Component type unknow string
const CString UNKNOWN_COMP_TYPE = _T("Unknown Component Type");

/////////////////////////////////////////////////////////////////////
//Components with Set Lengths
const double COMPONENT_LENGTH_0500 =  500.00;
const double COMPONENT_LENGTH_0600 =  600.00;
const double COMPONENT_LENGTH_0700 =  700.00;
const double COMPONENT_LENGTH_0800 =  800.00;
const double COMPONENT_LENGTH_1000 = 1000.00;
const double COMPONENT_LENGTH_1200 = 1200.00;
const double COMPONENT_LENGTH_1270 = 1270.00;
const double COMPONENT_LENGTH_1300 = 1300.00;
const double COMPONENT_LENGTH_1500 = 1500.00;
const double COMPONENT_LENGTH_1600 = 1600.00;
const double COMPONENT_LENGTH_1700 = 1700.00;
const double COMPONENT_LENGTH_1800 = 1800.00;
const double COMPONENT_LENGTH_1830 = 1830.00;
const double COMPONENT_LENGTH_1900 = 1900.00;
const double COMPONENT_LENGTH_2000 = 2000.00;
const double COMPONENT_LENGTH_2100 = 2100.00;
const double COMPONENT_LENGTH_2400 = 2400.00;
const double COMPONENT_LENGTH_2439 = 2439.00;
const double COMPONENT_LENGTH_2500 = 2500.00;
const double COMPONENT_LENGTH_2700 = 2700.00;
const double COMPONENT_LENGTH_3000 = 3000.00;
const double COMPONENT_LENGTH_3200 = 3200.00;
const double COMPONENT_LENGTH_3600 = 3600.00;
const double COMPONENT_LENGTH_4000 = 4000.00;
const double COMPONENT_LENGTH_4200 = 4200.00;
const double COMPONENT_LENGTH_6000 = 6000.00;

//Adjustments to Position Components in 3D exactly
//192(152(bottom of tranny star)+40(star height))-185(EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD) = 7;
const double LOW_STAR_KWIKS_Z_ADJUST					= 7.00;
const double TRANSOM_KWIKS_WANTFA_THICKNESS				= 45.00;
const double TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS		= 5.00;
const double LOW_STAR_MILLS_Z_ADJUST					= 0.00;
const double TRANSOM_MILLS_WANTFA_THICKNESS				= 0.00;
const double TRANSOM_MILLS_ANGLE_SECTION_THICKNESS		= 5.00;
//192(152(bottom of tranny star)+40(star height))-185(EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD)+25(dist between stars) = 32
const double HIGH_STAR_KWIKS_Z_ADJUST					= 32.00;
const double HIGH_STAR_MILLS_Z_ADJUST					= 0.00;

const double HOPUP_BRACKET_KWIKS_Z_ADJUST				= 51.00;
const double HOPUP_BRACKET_MILLS_Z_ADJUST				= 51.00;
const double HOPUP_BRACKET_KWIKS_TIE_BAR_HOLE_CNTR		= 32.00;
const double TIE_BAR_KWIKS_CNTR_PEG_TO_EDGE				= 19.00;
const double TIE_BAR_KWIKS_OUTER_EDGE_TO_INNER_EDGE		= 5.00;
const double BRACING_MILLS_HOLE_TO_EDGE					= 34.00;
const double BRACING_KWIKS_HOLE_TO_EDGE					= 34.00;
const double STANDARD_KWIKS_SPRIGET_LENGTH				= 100.00;
const double STANDARD_MILLS_SPRIGET_LENGTH				= 0.00;	



/////////////////////////////////////////////////////////////////////
//Handrail types
/////////////////////////////////////////////////////////////////////

enum HandrailTypeEnum
{
//	CT_HANDRAIL_TOEBOARD,
	CT_HANDRAIL_MIDRAIL_TB,
	CT_HANDRAIL_HALFMESH,
	CT_HANDRAIL_FULLMESH
};

inline CString GetHandrailTypeDescriptionStr( HandrailTypeEnum eHandrailType )
{
	switch( eHandrailType )
	{
//		case( CT_HANDRAIL_TOEBOARD )	: return "Handrail & Toeboard";
		case( CT_HANDRAIL_MIDRAIL_TB )	: return _T("Handrail, Midrail & Toeboard");
		case( CT_HANDRAIL_HALFMESH )	: return _T("Handrail & HalfMesh");
		case( CT_HANDRAIL_FULLMESH )	: return _T("Handrail & FullMesh");
		default:
			;//assert( false );
			return _T("Unknown");
	}
	;//assert( false );
	return _T("");
}


/////////////////////////////////////////////////////////////////////
// GetComponentDescriptionStr						INLINE FUNCTION
//
// This function gets the descriptive string for the component type enum arguement.
// The return value is a CString.

inline CString GetComponentDescStr( ComponentTypeEnum eComponentType )
{
	switch( eComponentType )
	{
		case( CT_DECKING_PLANK )		: return _T("Decking Plank");	
		case( CT_LAPBOARD )				: return _T("Lapboard");
		case( CT_STAGE_BOARD )			: return _T("Stage Board");
		case( CT_LEDGER )				: return _T("Ledger");
		case( CT_TRANSOM )				: return _T("Transom");
		case( CT_MESH_GUARD )			: return _T("Mesh Guard");
		case( CT_RAIL )					: return _T("Rail");
		case( CT_MID_RAIL )				: return _T("Mid-Rail");
		case( CT_TOE_BOARD )			: return _T("Toeboard");
		case( CT_CHAIN_LINK )			: return _T("Chain Link");
		case( CT_SHADE_CLOTH )			: return _T("Shade Cloth");
		case( CT_TIE_TUBE )				: return _T("Tie Tube");
		case( CT_TIE_CLAMP_COLUMN )		: return _T("Column Tie");
		case( CT_TIE_CLAMP_MASONARY )	: return _T("Masonary Tie");
		case( CT_TIE_CLAMP_YOKE )		: return _T("Yoke Tie");
		case( CT_TIE_CLAMP_90DEGREE )	: return _T("Tie Clamp 90Deg");
		case( CT_BRACING )				: return _T("Bracing");
		case( CT_STAIR )				: return _T("Stair");
		case( CT_LADDER )				: return _T("Ladder");
		case( CT_STANDARD )				: return _T("Standard");
		case( CT_STANDARD_OPENEND )		: return _T("Standard Open End");
		case( CT_JACK )					: return _T("Jack");
		case( CT_SOLEBOARD )			: return _T("Soleboard");
		case( CT_HOPUP_BRACKET )		: return _T("Hopup Bracket");
		case( CT_CORNER_STAGE_BOARD )	: return _T("Cnr Stage Board");
		case( CT_TEXT )					: return _T("Text");
		case( CT_TIE_BAR )				: return _T("Tie Bar");
		case( CT_STANDARD_CONNECTOR )	: return _T("Standard Connector");
		case( CT_LADDER_PUTLOG )		: return _T("Ladder Tower Putlog");
		case( CT_STAIR_RAIL )			: return _T("Stair Rail");
		case( CT_STAIR_RAIL_STOPEND )	: return _T("Stair Rail Stopend");
		case( CT_TOE_BOARD_CLIP )		: return _T("Toe Board Clip");
		case( CT_PUTLOG_CLIP )			: return _T("Putlog Clip");
		case( CT_BOM_EXTRA )			: return _T("BOM Extra");
		case( CT_VISUAL_COMPONENT )		: return _T("Visual Component");
		case( CT_UNDEFINED )			: return _T("Undefined");
		default:
			;//assert( false );
			return _T("Unknown");
	};
	;//assert( false );
	return _T("");
}

inline CString GetComponentDescStrAlign( ComponentTypeEnum eComponentType )
{
	switch( eComponentType )
	{
		case( CT_DECKING_PLANK )		: return _T("Decking Plank   ");
		case( CT_LAPBOARD )				: return _T("Lapboard        ");
		case( CT_STAGE_BOARD )			: return _T("Stage Board     ");
		case( CT_LEDGER )				: return _T("Ledger          ");
		case( CT_TRANSOM )				: return _T("Transom         ");
		case( CT_MESH_GUARD )			: return _T("Mesh Guard      ");
		case( CT_RAIL )					: return _T("Rail            ");
		case( CT_MID_RAIL )				: return _T("Mid-Rail        ");
		case( CT_TOE_BOARD )			: return _T("Toeboard        ");
		case( CT_CHAIN_LINK )			: return _T("Chain Link      ");
		case( CT_SHADE_CLOTH )			: return _T("Shade Cloth     ");
		case( CT_TIE_TUBE )				: return _T("Tie Tube        ");
		case( CT_TIE_CLAMP_COLUMN )		: return _T("Column Tie      ");
		case( CT_TIE_CLAMP_MASONARY )	: return _T("Masonary Tie    ");
		case( CT_TIE_CLAMP_YOKE )		: return _T("Yoke Tie        ");
		case( CT_TIE_CLAMP_90DEGREE )	: return _T("Tie Clamp 90Deg ");
		case( CT_BRACING )				: return _T("Bracing         ");
		case( CT_STAIR )				: return _T("Stair           ");
		case( CT_LADDER )				: return _T("Ladder          ");
		case( CT_STANDARD )				: return _T("Standard        ");
		case( CT_STANDARD_OPENEND )		: return _T("Standard Open   ");
		case( CT_JACK )					: return _T("Jack            ");
		case( CT_SOLEBOARD )			: return _T("Soleboard       ");
		case( CT_HOPUP_BRACKET )		: return _T("Hopup Bracket   ");
		case( CT_CORNER_STAGE_BOARD )	: return _T("Cnr Stage Board ");
		case( CT_TEXT )					: return _T("Text            ");
		case( CT_VISUAL_COMPONENT )		: return _T("Visual Component");
		case( CT_UNDEFINED )			: return _T("Undefined       ");
		case( CT_BOM_EXTRA )			: return _T("BOM Extra       ");
		default:
			;//assert( false );
			return _T("Unknown         ");
	};
	;//assert( false );
	return _T("");
}


#endif	//#ifndef	__COMPONENT_DEFINITIONS_H__

/*******************************************************************************
*	History Records
********************************************************************************
* $History: ComponentDefinitions.h $
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 5/10/00    Time: 8:14a
 * Updated in $/Meccano/Stage 1/Code
 * just finished building version 1.5.1.3 (R1.1c)
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
*
*******************************************************************************/
