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
//                              ColourCode Class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 17/11/00 4:42p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/COLOURCODE.H $
//	Version			: $Revision: 20 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	__COLOUR_CODE_H__
#define	__COLOUR_CODE_H__

#include "MeccanoDefinitions.h"

/////////////////////////////////////////////////////////////////////
//Colour
/////////////////////////////////////////////////////////////////////

//Defined colours
enum ColourEnum
{
	COLOUR_ERASE,
	COLOUR_RED,
	COLOUR_YELLOW,
	COLOUR_GREEN,
	COLOUR_CYAN,
	COLOUR_BLUE,
	COLOUR_MAGENTA,
	COLOUR_WHITE,
	COLOUR_DARK_GREY,
	COLOUR_GREY
};

//RGB colours for CKwikscafButton class
const int COLOUR_RGB_RED		= 0x0000ff;
const int COLOUR_RGB_GREEN		= 0x00ff00;
const int COLOUR_RGB_BLUE		= 0xff0000;
const int COLOUR_RGB_BLACK		= 0x000000;
const int COLOUR_RGB_WHITE		= COLOUR_RGB_RED+COLOUR_RGB_GREEN+COLOUR_RGB_BLUE;
const int COLOUR_RGB_YELLOW		= COLOUR_RGB_RED+COLOUR_RGB_GREEN;
const int COLOUR_RGB_CYAN		= COLOUR_RGB_GREEN+COLOUR_RGB_BLUE;
const int COLOUR_RGB_MAGENTA	= COLOUR_RGB_RED+COLOUR_RGB_BLUE;
const int COLOUR_RGB_GREY_LIGHT	= 0xc4c4c4;
const int COLOUR_RGB_GREY_DARK	= 0x606060;

const int COLOUR_UNDEFINED				= -10;
const int COLOUR_MULTISELECT			= -20;

//colour codes for items
extern int COLOUR_CROSSHAIR;
extern int COLOUR_CROSSHAIR_DEBUG;
extern int COLOUR_CROSSHAIR_ARC;
extern int COLOUR_CROSSHAIR_3D_VIEW;
extern int COLOUR_CROSSHAIR_MATRIX;
extern int COLOUR_MATRIX_RL_TEXT;
extern int COLOUR_MATRIX_RL_LINES_MINOR;
extern int COLOUR_MATRIX_RL_LINES_MAJOR;
extern int COLOUR_MATRIX_RL_LINES_SPAN;
extern int COLOUR_MATRIX_LEVELS_TEXT;
extern int COLOUR_MATRIX_LEVELS_LINES;


extern int COLOUR_0700;
extern int COLOUR_1200;
extern int COLOUR_1800;
extern int COLOUR_2400;

extern int COLOUR_DECKING_PLANK;
extern int COLOUR_STAGE_BOARD;
extern int COLOUR_LAPBOARD;
extern int COLOUR_LEDGER;
extern int COLOUR_TRANSOM;
extern int COLOUR_MESH_GUARD;
extern int COLOUR_RAIL;
extern int COLOUR_MID_RAIL;
extern int COLOUR_TOE_BOARD;
extern int COLOUR_CHAIN_LINK;
extern int COLOUR_SHADE_CLOTH;
extern int COLOUR_CHAIN_AND_SHADE;
extern int COLOUR_TIE_TUBE;
extern int COLOUR_TIE_CLAMP_COLUMN;
extern int COLOUR_TIE_CLAMP_MASONARY;
extern int COLOUR_TIE_CLAMP_YOKE;
extern int COLOUR_TIE_CLAMP_90DEGREE;
extern int COLOUR_BRACING;
extern int COLOUR_STAIR;
extern int COLOUR_LADDER;
extern int COLOUR_STANDARD;
extern int COLOUR_JACK;
extern int COLOUR_SOLEBOARD;
extern int COLOUR_HOPUP_BRACKET;
extern int COLOUR_CORNER_STAGE_BOARD;
extern int COLOUR_TEXT;
extern int COLOUR_TIE_BAR;
extern int COLOUR_STANDARD_CONNECTOR;
extern int COLOUR_LADDER_PUTLOG;
extern int COLOUR_STAIR_RAIL;
extern int COLOUR_STAIR_RAIL_STOPEND;
extern int COLOUR_TOE_BOARD_CLIP;
extern int COLOUR_BOM_EXTRA;
extern int COLOUR_PUTLOG_CLIP;
extern int COLOUR_VISUAL_COMPONENT;

extern int COLOUR_COMMITTED_FULLY;
extern int COLOUR_COMMITTED_PARTIALLY;

inline int GetBayColour( double dBayLength )
{
	int iColour;

	if( dBayLength>=COMPONENT_LENGTH_2400 )
		iColour = COLOUR_2400;
	else if( dBayLength>=COMPONENT_LENGTH_1800 )
		iColour = COLOUR_1800;
	else if( dBayLength>=COMPONENT_LENGTH_1200 )
		iColour = COLOUR_1200;
	else if( dBayLength>=COMPONENT_LENGTH_0700 )
		iColour = COLOUR_0700;
	else
		iColour = COLOUR_BLUE;

	return iColour;
}


inline int GetComponentColour( ComponentTypeEnum eType )
{
	switch( eType )
	{
	case( CT_DECKING_PLANK ):		return COLOUR_DECKING_PLANK;
	case( CT_STAGE_BOARD ):			return COLOUR_STAGE_BOARD;
	case( CT_LAPBOARD ):			return COLOUR_LAPBOARD;
	case( CT_LEDGER ):				return COLOUR_LEDGER;
	case( CT_TRANSOM ):				return COLOUR_TRANSOM;
	case( CT_MESH_GUARD	):			return COLOUR_MESH_GUARD;
	case( CT_RAIL ):				return COLOUR_RAIL;
	case( CT_MID_RAIL ):			return COLOUR_MID_RAIL;
	case( CT_TOE_BOARD ):			return COLOUR_TOE_BOARD;
	case( CT_CHAIN_LINK ):			return COLOUR_CHAIN_LINK;
	case( CT_SHADE_CLOTH ):			return COLOUR_SHADE_CLOTH;
	case( CT_TIE_TUBE ):			return COLOUR_TIE_TUBE;
	case( CT_TIE_CLAMP_COLUMN ):	return COLOUR_TIE_CLAMP_COLUMN;
	case( CT_TIE_CLAMP_MASONARY ):	return COLOUR_TIE_CLAMP_MASONARY;
	case( CT_TIE_CLAMP_YOKE ):		return COLOUR_TIE_CLAMP_YOKE;
	case( CT_TIE_CLAMP_90DEGREE ):	return COLOUR_TIE_CLAMP_90DEGREE;
	case( CT_BRACING ):				return COLOUR_BRACING;
	case( CT_STAIR ):				return COLOUR_STAIR;
	case( CT_LADDER ):				return COLOUR_LADDER;
	case( CT_STANDARD ):			return COLOUR_STANDARD;
	case( CT_STANDARD_OPENEND ):	return COLOUR_STANDARD;//return COLOUR_STANDARD_OPENEND
	case( CT_JACK ):				return COLOUR_JACK;
	case( CT_SOLEBOARD ):			return COLOUR_SOLEBOARD;
	case( CT_HOPUP_BRACKET ):		return COLOUR_HOPUP_BRACKET;
	case( CT_CORNER_STAGE_BOARD ):	return COLOUR_CORNER_STAGE_BOARD;
	case( CT_TEXT ):				return COLOUR_TEXT;
	case( CT_TIE_BAR ):				return COLOUR_TIE_BAR;
	case( CT_STANDARD_CONNECTOR ):	return COLOUR_STANDARD_CONNECTOR;
	case( CT_LADDER_PUTLOG ):		return COLOUR_LADDER_PUTLOG;
	case( CT_STAIR_RAIL ):			return COLOUR_STAIR_RAIL;
	case( CT_STAIR_RAIL_STOPEND ):	return COLOUR_STAIR_RAIL_STOPEND;
	case( CT_TOE_BOARD_CLIP ):		return COLOUR_TOE_BOARD_CLIP;
	case( CT_BOM_EXTRA ):			return COLOUR_BOM_EXTRA;
	case( CT_PUTLOG_CLIP ):			return COLOUR_PUTLOG_CLIP;
	case( CT_VISUAL_COMPONENT ):	return COLOUR_VISUAL_COMPONENT;
	default:
	case( CT_UNDEFINED ):
		assert( FALSE );	//invalid component type
	}
	return 0;
}

inline void ResetColours()
{
	COLOUR_TIE_CLAMP_MASONARY	= 131;
	COLOUR_TIE_CLAMP_YOKE		= 132;
	COLOUR_TIE_CLAMP_90DEGREE	= 140;
	COLOUR_BRACING				= 150;
	COLOUR_STAIR				= 160;
	COLOUR_LADDER				= 170;
	COLOUR_STANDARD				= 180;
	COLOUR_JACK					= 190;
	COLOUR_SOLEBOARD			= 200;
	COLOUR_HOPUP_BRACKET		= 210;
	COLOUR_CORNER_STAGE_BOARD	= 220;
	COLOUR_TEXT					= 230;
	COLOUR_TIE_BAR				= 240;
	COLOUR_STANDARD_CONNECTOR	= 250;
	COLOUR_LADDER_PUTLOG		= 10;
	COLOUR_STAIR_RAIL			= 20;
	COLOUR_STAIR_RAIL_STOPEND	= 30;
	COLOUR_TOE_BOARD_CLIP		= 40;
	COLOUR_BOM_EXTRA			= 50;
	COLOUR_PUTLOG_CLIP			= 60;
	COLOUR_VISUAL_COMPONENT		= 70;


	COLOUR_CROSSHAIR				= COLOUR_YELLOW;
	COLOUR_CROSSHAIR_DEBUG			= COLOUR_RED;
	COLOUR_CROSSHAIR_ARC			= COLOUR_YELLOW;
	COLOUR_CROSSHAIR_3D_VIEW		= COLOUR_YELLOW;
	COLOUR_CROSSHAIR_MATRIX			= COLOUR_GREY;
	COLOUR_MATRIX_RL_TEXT			= COLOUR_YELLOW;
	COLOUR_MATRIX_RL_LINES_MINOR	= COLOUR_RED;
	COLOUR_MATRIX_RL_LINES_MAJOR	= COLOUR_BLUE;
	COLOUR_MATRIX_RL_LINES_SPAN		= COLOUR_YELLOW;
	COLOUR_MATRIX_LEVELS_TEXT		= COLOUR_GREEN;
	COLOUR_MATRIX_LEVELS_LINES		= COLOUR_DARK_GREY;

	COLOUR_0700		= COLOUR_CYAN;		//0.7m long = Cyan,
	COLOUR_1200		= COLOUR_YELLOW;	//1.2m long = Yellow,
	COLOUR_1800		= COLOUR_GREEN;		//1.8m long = Green,
	COLOUR_2400		= COLOUR_RED;		//2.4m long = Red.

	COLOUR_COMMITTED_FULLY		= COLOUR_WHITE;
	COLOUR_COMMITTED_PARTIALLY	= COLOUR_DARK_GREY;
}

inline void StoreColoursInRegistry()
{
	SetIntInRegistry( _T("ColourTieClampMasonary"),		COLOUR_TIE_CLAMP_MASONARY );
	SetIntInRegistry( _T("ColourTieClampYoke"),			COLOUR_TIE_CLAMP_YOKE );
	SetIntInRegistry( _T("ColourTieClamp90degree"),		COLOUR_TIE_CLAMP_90DEGREE );
	SetIntInRegistry( _T("ColourBracing"),				COLOUR_BRACING );
	SetIntInRegistry( _T("ColourStair"),				COLOUR_STAIR );
	SetIntInRegistry( _T("ColourLadder"),				COLOUR_LADDER );
	SetIntInRegistry( _T("ColourStandard"),				COLOUR_STANDARD );
	SetIntInRegistry( _T("ColourJack"),					COLOUR_JACK );
	SetIntInRegistry( _T("ColourSoleboard"),			COLOUR_SOLEBOARD );
	SetIntInRegistry( _T("ColourHopupBracket"),			COLOUR_HOPUP_BRACKET );
	SetIntInRegistry( _T("ColourCornerStageBoard"),		COLOUR_CORNER_STAGE_BOARD );
	SetIntInRegistry( _T("ColourText"),					COLOUR_TEXT );
	SetIntInRegistry( _T("ColourTieBar"),				COLOUR_TIE_BAR );
	SetIntInRegistry( _T("ColourStandardConnector"),	COLOUR_STANDARD_CONNECTOR );
	SetIntInRegistry( _T("ColourLadderPutlog"),			COLOUR_LADDER_PUTLOG );
	SetIntInRegistry( _T("ColourStairRail"),			COLOUR_STAIR_RAIL );
	SetIntInRegistry( _T("ColourStairRailStopend"),		COLOUR_STAIR_RAIL_STOPEND );
	SetIntInRegistry( _T("ColourToeBoardClip"),			COLOUR_TOE_BOARD_CLIP );
	SetIntInRegistry( _T("ColourBomExtra"),				COLOUR_BOM_EXTRA );
	SetIntInRegistry( _T("ColourPutlogClip"),			COLOUR_PUTLOG_CLIP );
	SetIntInRegistry( _T("ColourVisualComponent"),		COLOUR_VISUAL_COMPONENT );

	SetIntInRegistry( _T("ColourCrosshair"),			COLOUR_CROSSHAIR );
	SetIntInRegistry( _T("ColourCrosshairDebug"),		COLOUR_CROSSHAIR_DEBUG );
	SetIntInRegistry( _T("ColourCrosshairArc"),			COLOUR_CROSSHAIR_ARC );
	SetIntInRegistry( _T("ColourCrosshair3dView"),		COLOUR_CROSSHAIR_3D_VIEW );
	SetIntInRegistry( _T("ColourCrosshairMatrix"),		COLOUR_CROSSHAIR_MATRIX );
	SetIntInRegistry( _T("ColourMatrixRlText"),			COLOUR_MATRIX_RL_TEXT );
	SetIntInRegistry( _T("ColourMatrixRlLinesMinor"),	COLOUR_MATRIX_RL_LINES_MINOR );
	SetIntInRegistry( _T("ColourMatrixRlLinesMajor"),	COLOUR_MATRIX_RL_LINES_MAJOR );
	SetIntInRegistry( _T("ColourMatrixRlLinesSpan"),	COLOUR_MATRIX_RL_LINES_SPAN );
	SetIntInRegistry( _T("ColourMatrixLevelsText"),		COLOUR_MATRIX_LEVELS_TEXT );
	SetIntInRegistry( _T("ColourMatrixLevelsLines"),	COLOUR_MATRIX_LEVELS_LINES );

	SetIntInRegistry( _T("Colour0700"),	COLOUR_0700 );
	SetIntInRegistry( _T("Colour1200"),	COLOUR_1200 );
	SetIntInRegistry( _T("Colour1800"),	COLOUR_1800 );
	SetIntInRegistry( _T("Colour2400"),	COLOUR_2400 );

	SetIntInRegistry( _T("ColourCommittedFully"),		COLOUR_COMMITTED_FULLY );
	SetIntInRegistry( _T("ColourCommittedPartially"),	COLOUR_COMMITTED_PARTIALLY );
}

inline void LoadColoursFromRegistry()
{
	COLOUR_TIE_CLAMP_MASONARY	= GetIntInRegistry( _T("ColourTieClampMasonary"),	COLOUR_TIE_CLAMP_MASONARY );
	COLOUR_TIE_CLAMP_YOKE		= GetIntInRegistry( _T("ColourTieClampYoke"),		COLOUR_TIE_CLAMP_YOKE );
	COLOUR_TIE_CLAMP_90DEGREE	= GetIntInRegistry( _T("ColourTieClamp90degree"),	COLOUR_TIE_CLAMP_90DEGREE );
	COLOUR_BRACING				= GetIntInRegistry( _T("ColourBracing"),			COLOUR_BRACING );
	COLOUR_STAIR				= GetIntInRegistry( _T("ColourStair"),				COLOUR_STAIR );
	COLOUR_LADDER				= GetIntInRegistry( _T("ColourLadder"),				COLOUR_LADDER );
	COLOUR_STANDARD				= GetIntInRegistry( _T("ColourStandard"),			COLOUR_STANDARD );
	COLOUR_JACK					= GetIntInRegistry( _T("ColourJack"),				COLOUR_JACK );
	COLOUR_SOLEBOARD			= GetIntInRegistry( _T("ColourSoleboard"),			COLOUR_SOLEBOARD );
	COLOUR_HOPUP_BRACKET		= GetIntInRegistry( _T("ColourHopupBracket"),		COLOUR_HOPUP_BRACKET );
	COLOUR_CORNER_STAGE_BOARD	= GetIntInRegistry( _T("ColourCornerStageBoard"),	COLOUR_CORNER_STAGE_BOARD );
	COLOUR_TEXT					= GetIntInRegistry( _T("ColourText"),				COLOUR_TEXT );
	COLOUR_TIE_BAR				= GetIntInRegistry( _T("ColourTieBar"),				COLOUR_TIE_BAR );
	COLOUR_STANDARD_CONNECTOR	= GetIntInRegistry( _T("ColourStandardConnector"),	COLOUR_STANDARD_CONNECTOR );
	COLOUR_LADDER_PUTLOG		= GetIntInRegistry( _T("ColourLadderPutlog"),		COLOUR_LADDER_PUTLOG );
	COLOUR_STAIR_RAIL			= GetIntInRegistry( _T("ColourStairRail"),			COLOUR_STAIR_RAIL );
	COLOUR_STAIR_RAIL_STOPEND	= GetIntInRegistry( _T("ColourStairRailStopend"),	COLOUR_STAIR_RAIL_STOPEND );
	COLOUR_TOE_BOARD_CLIP		= GetIntInRegistry( _T("ColourToeBoardClip"),		COLOUR_TOE_BOARD_CLIP );
	COLOUR_BOM_EXTRA			= GetIntInRegistry( _T("ColourBomExtra"),			COLOUR_BOM_EXTRA );
	COLOUR_PUTLOG_CLIP			= GetIntInRegistry( _T("ColourPutlogClip"),			COLOUR_PUTLOG_CLIP );
	COLOUR_VISUAL_COMPONENT		= GetIntInRegistry( _T("ColourVisualComponent"),	COLOUR_VISUAL_COMPONENT );

	COLOUR_CROSSHAIR				= GetIntInRegistry( _T("ColourCrosshair"),			COLOUR_CROSSHAIR );
	COLOUR_CROSSHAIR_DEBUG			= GetIntInRegistry( _T("ColourCrosshairDebug"),		COLOUR_CROSSHAIR_DEBUG );
	COLOUR_CROSSHAIR_ARC			= GetIntInRegistry( _T("ColourCrosshairArc"),		COLOUR_CROSSHAIR_ARC );
	COLOUR_CROSSHAIR_3D_VIEW		= GetIntInRegistry( _T("ColourCrosshair3dView"),	COLOUR_CROSSHAIR_3D_VIEW );
	COLOUR_CROSSHAIR_MATRIX			= GetIntInRegistry( _T("ColourCrosshairMatrix"),	COLOUR_CROSSHAIR_MATRIX );
	COLOUR_MATRIX_RL_TEXT			= GetIntInRegistry( _T("ColourMatrixRlText"),		COLOUR_MATRIX_RL_TEXT );
	COLOUR_MATRIX_RL_LINES_MINOR	= GetIntInRegistry( _T("ColourMatrixRlLinesMinor"),	COLOUR_MATRIX_RL_LINES_MINOR );
	COLOUR_MATRIX_RL_LINES_MAJOR	= GetIntInRegistry( _T("ColourMatrixRlLinesMajor"),	COLOUR_MATRIX_RL_LINES_MAJOR );
	COLOUR_MATRIX_RL_LINES_SPAN		= GetIntInRegistry( _T("ColourMatrixRlLinesSpan"),	COLOUR_MATRIX_RL_LINES_SPAN );
	COLOUR_MATRIX_LEVELS_TEXT		= GetIntInRegistry( _T("ColourMatrixLevelsText"),	COLOUR_MATRIX_LEVELS_TEXT );
	COLOUR_MATRIX_LEVELS_LINES		= GetIntInRegistry( _T("ColourMatrixLevelsLines"),	COLOUR_MATRIX_LEVELS_LINES );

	COLOUR_0700	= GetIntInRegistry( _T("Colour0700"),	COLOUR_0700 );
	COLOUR_1200	= GetIntInRegistry( _T("Colour1200"),	COLOUR_1200 );
	COLOUR_1800	= GetIntInRegistry( _T("Colour1800"),	COLOUR_1800 );
	COLOUR_2400	= GetIntInRegistry( _T("Colour2400"),	COLOUR_2400 );

	COLOUR_COMMITTED_FULLY		= GetIntInRegistry( _T("ColourCommittedFully"),		COLOUR_COMMITTED_FULLY );
	COLOUR_COMMITTED_PARTIALLY	= GetIntInRegistry( _T("ColourCommittedPartially"),	COLOUR_COMMITTED_PARTIALLY );
}

#endif	//#ifndef	__COLOUR_CODE_H__

/*******************************************************************************
*	History Records
********************************************************************************
* $History: ColourCode.h $
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 17/11/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * HCT Colour seems to be fully working now
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 5/10/00    Time: 8:13a
 * Updated in $/Meccano/Stage 1/Code
 * just finished building version 1.5.1.3 (R1.1c)
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 30/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Customize Colour complete and hidden - 
 * Shade and Chain mesh prices fixed
 * Bill Cross stage 1 fully complete
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 30/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:53a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * RLScale, and Levels are nearly working
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 29/09/99   Time: 12:33p
 * Updated in $/Meccano/Stage 1/Code
 * Simple Curve avoidance functionality now operational
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 9/09/99    Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Matrix now has crosshair which operates correctly
 * 2) Still only have one (1) MatrixElement
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
*
*******************************************************************************/
