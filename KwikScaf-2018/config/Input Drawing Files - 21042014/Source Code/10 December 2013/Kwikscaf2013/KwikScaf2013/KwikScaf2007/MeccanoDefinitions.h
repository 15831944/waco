//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined MECCANODEFINITIONS
#define MECCANODEFINITIONS

//********************************************************************************
//********************************************************************************
//									DEFINITIONS HEADER
//********************************************************************************
//********************************************************************************

//********************************************************************************
//* CREATED BY DAVID RAINEY 14/7/99
//*
//*	Last Changed By : $Author: Jsb $
//*	Last Revision	: $Date: 15/11/00 11:45a $
//*	Version			: $Revision: 134 $
//*
//********************************************************************************

//********************************************************************************
//*	This file contains the common definitions of CONSTANTS and new variable types 
//* used by all classes.
//*
//********************************************************************************

//Standard includes
#include "stdafx.h"
#include "GraphicTypes.h"
#include <afxtempl.h>
#include <math.h>
#include "VersionDefinitions.h"

#define DELETE_PTR(x) if (x) { delete x; x = NULL; }

#include "ComponentDefinitions.h"
#include "MaterialTypeDefinitions.h"
#include "BayDefinitions.h"
#include "Serialization.h"

//#define	USE_LINE_CLASS

/////////////////////////////////////////////////////////////////////
//Rounding Error compensation
const double ROUND_ERROR = 0.50;
const double ROUND_ERROR_SMALL = 0.0005;
const double ROUND_ERROR_ANGLE = 0.01;
const double ROUND_ERROR_ANGLE_SMALL = 0.0001;

#include "StagesPlanksEtcDefinitions.h"

/////////////////////////////////////////////////////////////////////
//Project Details
#define	PROJECT_NAME	KwikScaf
#define	MOST_LIKELY_PATH	_T("C:\\Program Files\\Acad2000\\KwikScaf\\")

/////////////////////////////////////////////////////////////////////
//Standard Star separation
//const double STAR_SEPARATION_KWIKSTAGE			= 500.00;
const double STAR_SEPARATION_KWIKSTAGE			= 495.33;
const double STAR_SEPARATION_MILLS				= 500.00;
const double STAR_SEPARATION_APPROX_KWIKSTAGE	= 500.00;
const double STAR_SEPARATION_APPROX_MILLS		= 500.00;
const double STANDARD_POLE_CORRECTION	= 500.00;	//eg. a 3m standard only has 2.5m of stars
extern double STAR_SEPARATION;
extern double STAR_SEPARATION_APPROX;

inline double ConvertRLtoStarRL( double dRL, double dStarSeparation )
{
	return (dRL/STAR_SEPARATION_APPROX)*dStarSeparation;
}

inline double ConvertStarRLtoRL( double dStarRL, double dStarSeparation )
{
	return (dStarRL/dStarSeparation)*STAR_SEPARATION_APPROX;
}

enum SystemEnum
{
	S_MILLS,
	S_KWIKSTAGE,
	S_OTHER,
	S_INVALID = -1
};
const CString	SYSTEM_TEXT_MILLS		= _T("Mills");
const CString	SYSTEM_TEXT_KWIKSTAGE	= _T("KwikStage");
const CString	SYSTEM_TEXT_OTHER		= _T("Other");

/////////////////////////////////////////////////////////////////////
//Standard top and bottom extra lengths
const double EXTRA_LENGTH_KS_TOP_OF_STANDARD	= 310.00;
const double EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD = 185.00;
const double EXTRA_LENGTH_MS_TOP_OF_STANDARD	= 265.00;
const double EXTRA_LENGTH_MS_BOTTOM_OF_STANDARD = 235.00;
extern double EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
extern double EXTRA_LENGTH_AT_TOP_OF_STANDARD;

/////////////////////////////////////////////////////////////////////
//Jack Length
extern double JACK_LENGTH;
extern double JACK_LENGTH_APPROX;

/////////////////////////////////////////////////////////////////////
//RL Adjustment
extern double RL_ADJUST;
extern double RL_ADJUST_APPROX;

/////////////////////////////////////////////////////////////////////
//Mesh GuardLength Limit
//#define	SMALLEST_MESH_IS_1200

/////////////////////////////////////////////////////////////////////
//Standards stuff
enum StandardFitEnum
{
	STD_FIT_COURSE,
	STD_FIT_FINE,
	STD_FIT_SAME,
};

/////////////////////////////////////////////////////////////////////
//Autobuild Run fit
enum RunFitEnum
{
	RF_TIGHT,	//Tight fit
	RF_LOOSE	//Loose fit
};


/////////////////////////////////////////////////////////////////////
//Bay Joins
enum JoinTypeEnum
{
	JOIN_TYPE_FORWARD,
	JOIN_TYPE_BACKWARD,
	JOIN_TYPE_INNER,
	JOIN_TYPE_OUTER
};

const JoinTypeEnum DEFAULT_JOIN_TYPE = JOIN_TYPE_FORWARD;

/////////////////////////////////////////////////////////////////////
//Handrail style
enum HandrailStyleEnum
{
	HRS_NONE,
	HRS_RAIL_ONLY,
	HRS_HALF_MESH,
	HRS_FULL_MESH,
	HRS_FULL_MESH_TO_GROUND,
	HRS_INVALID	= -1
};


/////////////////////////////////////////////////////////////////////
//Matrix Stuff
/////////////////////////////////////////////////////////////////////

enum MatrixStandardEnum
{
	MATRIX_STANDARDS_INNER,	//the inner standard (southern)
	MATRIX_STANDARDS_OUTER,	//the outer standard (northern)
	MATRIX_STANDARDS_SIZE
};

const int MATRIX_NO_MATRIX_OBJECT	= -1;
const int MATRIX_EMPTY_MATRIX		= -2;
const int MATRIX_NO_MATCH			= -3;

/////////////////////////////////////////////////////////////////////
//IDs
//Most valid ID's are simply a +ve int value, this is why we don't use an enum.
//	However, We need some reserved values to indicate erros, etc.
const int ID_INVALID			= -1;	//The ID number is INvalid
const int ID_VALID				= -2;	//The ID number IS valid
const int ID_ALL_IDS			= -3;	//Used for selecting all IDs
const int ID_NO_IDS				= -4;	//Error code for no elements in list
const int ID_NONE_MATCHING		= -5;	//There were no IDs matching the criteria



/////////////////////////////////////////////////////////////////////
//Bracing
enum BracingReturnEnums
{
	BRACING_OK,						//fine
	BRACING_ERROR_ARRANGMENT,		//the arrangment is erroneous
	BRACING_ERROR_STANDARDS,		//the standards can't support the arrangement
	BRACING_ERROR_ENUM,				//an invalid enum was used
	BRACING_ERROR_OBSTRUCTION,		//There is something on that side obstructin the bracing
	BRACING_ERROR_OTHER
};

const int DEFAULT_BRACING_SPACING = 4;

/////////////////////////////////////////////////////////////////////
//Lift Stuff
/////////////////////////////////////////////////////////////////////

enum LiftRiseEnum
{
	LIFT_RISE_0000MM,
	LIFT_RISE_0500MM,
	LIFT_RISE_1000MM,
	LIFT_RISE_1500MM,
	LIFT_RISE_2000MM,
	LIFT_RISE_2500MM,
	LIFT_RISE_3000MM,
	LIFT_RISE_ALL_HEIGHTS		= -1,	//applies to all heights
	LIFT_RISE_INVALID			= -3,	//the Lift rise value is invalid
	LIFT_RISE_MULTI_SELECTION	= -4	//they have chosen a multiple selection the
										//	values are not consistant
};


enum StairPositionRiseEnum
{
	SPR_INNER,
	SPR_OUTER
};

/////////////////////////////////////////////////////////////////////////////////
//Conversion from an actual height to a Rise enum
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetRiseEnumFromRise
//Takes a Height, which is relative to the bottom of the lift
//	and converts it to a rise enum value
inline LiftRiseEnum	GetRiseEnumFromRise( double dHeight )
{
	LiftRiseEnum eRise;
	if		( dHeight<250.00 )	eRise=LIFT_RISE_0000MM;
	else if	( dHeight<750.00 )	eRise=LIFT_RISE_0500MM;
	else if	( dHeight<1250.00 )	eRise=LIFT_RISE_1000MM;
	else if	( dHeight<1750.00 )	eRise=LIFT_RISE_1500MM;
	else if	( dHeight<2250.00 )	eRise=LIFT_RISE_2000MM;
	else if	( dHeight<2750.00 )	eRise=LIFT_RISE_2500MM;
	else if	( dHeight<3250.00 )	eRise=LIFT_RISE_3000MM;
	else						eRise=LIFT_RISE_INVALID;

	return eRise;
}

/////////////////////////////////////////////////////////////////////////////////
//GetRiseFromRiseEnum
//Takes a Height, which is relative to the bottom of the lift
//	and converts it to a rise enum value
inline double	GetRiseFromRiseEnum( LiftRiseEnum eRise, double dStarSeparation )
{
	double dRise;
	dRise = 0.00;
	switch( eRise )
	{
		default:	
		case( LIFT_RISE_INVALID ):							//fallthrough
		case( LIFT_RISE_MULTI_SELECTION ):					//fallthrough
			assert( FALSE );	//What are you passing me crap for?
			dRise -= dStarSeparation;
			break;
		case( LIFT_RISE_3000MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_2500MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_2000MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_1500MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_1000MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_0500MM ): dRise+= dStarSeparation;	//fallthrough
		case( LIFT_RISE_0000MM ):
			break;
	}

	return dRise;
}


enum	LiftTypeEnum
{
	LIFT_TYPE_LIFT,
	LIFT_TYPE_LAPBOARD,
	LIFT_TYPE_STAIRS,
	LIFT_TYPE_LADDER,
	LIFT_TYPE_BUTTRESS,
	LIFT_TYPE_UNDEFINED = -1
};

inline LiftTypeEnum ConvertBayTypeToLiftType( BayTypeEnum BayType )
{
	switch( BayType )
	{
	case( BAY_TYPE_BAY ):		return LIFT_TYPE_LIFT;
	case( BAY_TYPE_LAPBOARD ):	return LIFT_TYPE_LAPBOARD;
	case( BAY_TYPE_STAIRS ):	return LIFT_TYPE_STAIRS;
	case( BAY_TYPE_LADDER ):	return LIFT_TYPE_LADDER;
	case( BAY_TYPE_BUTTRESS ):	return LIFT_TYPE_BUTTRESS;
	default:
	case( BAY_TYPE_UNDEFINED ):
		break;
	}
	return LIFT_TYPE_UNDEFINED;
}

/////////////////////////////////////////////////////////////////////////////////
//ConvertboolToBOOL
inline BOOL	ConvertboolToBOOL( bool bValue )
{
	return bValue? TRUE: FALSE;
}

/////////////////////////////////////////////////////////////////////////////////
//ConvertBOOLTobool
inline bool	ConvertBOOLTobool( BOOL bValue )
{
	return bValue? true: false;
}


const int DELETE_KEY = 46;
const int BACKSPACE_KEY = 8;

enum DimensionSelectionEnum
{
	DS_LENGTH,
	DS_WIDTH,
	DS_HEIGHT
};

enum VisibilityEnum
{
	VIS_VISIBLE,
	VIS_NOT_VISIBLE,
	VIS_MULTI_SELECTION	= -1,
	VIS_ERASE			= -2,	
	VIS_INVALID			= -3,
	VIS_USE_DEFAULT		= -4
};

/////////////////////////////////////////////////////////////////////
//Stage & Level
/////////////////////////////////////////////////////////////////////
#define STAGE_MULTIPLE_SELECTION	_T("<Multiple Selection>")
#define STAGE_DEFAULT_VALUE			_T("<None>")
#define LEVEL_MULTIPLE_SELECTION	_T("<Multiple Selection>")
#define LEVEL_DEFAULT_STRING		_T("<None>")
const int LEVEL_DEFAULT_VALUE		= -1;

/////////////////////////////////////////////////////////////////////
//Other stuff
/////////////////////////////////////////////////////////////////////
#define BRACES_EVERY_MAX_NUMBER_OF_BAYS 6

#define TIES_EVERY_MAX_NUMBER_OF_BAYS 6

enum TieTypesEnum
{
	TIE_TYPE_MASONARY,
	TIE_TYPE_COLUMN,
	TIE_TYPE_YOKE,
	TIE_TYPE_BUTTRESS_12,
	TIE_TYPE_BUTTRESS_18,
	TIE_TYPE_BUTTRESS_24,
	TIE_TYPES_NUMBER_OF,
	TIE_TYPES_INVALID = -1
};

inline CString GetTieTypeDescriptionStr( TieTypesEnum eTieType )
{
	switch( eTieType )
	{
		case( TIE_TYPE_MASONARY )		: return _T("Masonary");			break;
		case( TIE_TYPE_COLUMN )			: return _T("Column");				break;
		case( TIE_TYPE_YOKE )			: return _T("Yoke");				break;
		case( TIE_TYPE_BUTTRESS_12 )	: return _T("1.2m Buttress Bay");	break;
		case( TIE_TYPE_BUTTRESS_18 )	: return _T("1.8m Buttress Bay");	break;
		case( TIE_TYPE_BUTTRESS_24 )	: return _T("2.4m Buttress Bay");	break;
		default							: return _T("Unknown");				break;
	};
}

const TieTypesEnum		DEFAULT_TIE_TYPE				= TIE_TYPE_MASONARY;
const int				DEFAULT_TIE_SPACING				= 2;
const double			DEFAULT_TIE_VERTICAL_SPACING	= 4000.00;
const double			DEFAULT_TIE_INTO_WALL_COLUMN	= 400.00;
const double			DEFAULT_TIE_INTO_WALL_MASONARY	= 0.00;
const double			DEFAULT_TIE_INTO_WALL_YOKE		= 400.00;


enum RoundingTypeEnum
{
	RT_ROUND_DOWN,
	RT_ROUND_UP,
	RT_ROUND_OFF,
	RT_INVALID
};

//typedef AcDbEntity			Entity;

/////////////////////////////////////////////////////////////////////
//Units conversion function
//to use these simply multiply the value by this number
const double CONVERT_M_TO_MM = 1000.00;
const double CONVERT_MM_TO_M = 0.001;

/////////////////////////////////////////////////////////////////////
//Length of the RL markers in the matrix
const double	MATRIX_LINE_LENGTH_MINOR = 100.00;
const double	MATRIX_LINE_LENGTH_MAJOR = 300.00;
const int		NUMBER_OF_MATRIX_ELEMENTS_PER_SCALE = 4;
const double	MATRIX_SEPARATION_ALONG_X	= COMPONENT_LENGTH_1800*2.00;

/////////////////////////////////////////////////////////////////////
//Layers
#define	LAYER_NAME_3D			_T("KwikScaf 3D")
#define LAYER_NAME_SCHEMATIC	_T("KwikScaf Schematic")
#define LAYER_NAME_MATRIX		_T("KwikScaf Matrix")
#define LAYER_NAME_SECTIONS		_T("KwikScaf Sections")

/////////////////////////////////////////////////////////////////////
//Prefixes
const CString GROUP_PREFIX_SCHEMATIC = _T("Group_");
const CString GROUP_PREFIX_MATRIX = _T("Matrix_Group_");
const CString GROUP_PREFIX_CUT_THROUGH = _T("Kut_Group_");
const CString COMP_PREFIX = _T("Comp_");


#define REACTOR_DICTIONARY_NAME			_T("ASDK_DICT")
#define SUMMARY_INFO_DICTIONARY_NAME	_T("DWGPROPS")

//Plotting styles
enum PlotSyleEnum
{
	PS_LINE,
	PS_DOTS_CLOSE,
	PS_DOTS_DISTANT,
	PS_DASHED
};


//Text styles
#define	TEXT_STYLE_NORMAL				_T("KwikScaf ISO")
const CString	TEXT_STYLE_GENERAL		= TEXT_STYLE_NORMAL;
const CString	TEXT_STYLE_COMPONENT	= TEXT_STYLE_NORMAL;
const CString	TEXT_STYLE_MATRIX		= TEXT_STYLE_NORMAL;
const CString	TEXT_STYLE_SCHEMATIC	= TEXT_STYLE_NORMAL;
const CString	TEXT_STYLE_LAPBOARD		= TEXT_STYLE_NORMAL;
const CString	TEXT_STYLE_STANDARD		= _T("Standard");

//Plot styles
#define	PLOT_STYLE_NORMAL			_T("Standard")
#define	PLOT_STYLE_LINE				_T("KwikScaf_Line")
#define	PLOT_STYLE_DASHED			_T("KwikScaf_DASHED")
#define	PLOT_STYLE_DOTS_CLOSE		_T("KwikScaf_DOTC")
#define	PLOT_STYLE_DOTS_DISTANT		_T("KwikScaf_DOTD")


inline bool GetPlotStyleName( PlotSyleEnum ePS, TCHAR *strPS )
{
	switch( ePS )
	{
	case( PS_LINE ):
		_tcscpy( strPS, PLOT_STYLE_LINE );
		break;

	case( PS_DASHED ):
		_tcscpy( strPS, PLOT_STYLE_DASHED );
		break;

	case( PS_DOTS_CLOSE ):
		_tcscpy( strPS, PLOT_STYLE_DOTS_CLOSE );
		break;

	case( PS_DOTS_DISTANT ):
		_tcscpy( strPS, PLOT_STYLE_DOTS_DISTANT );
		break;

	default:
		_tcscpy( strPS, PLOT_STYLE_NORMAL );
		return false;
	}

	return true;
}

enum
{
	BP_SW_STD,	//SouthWest Standard
	BP_SE_STD,	//SouthEast Standard
	BP_NW_STD,	//NorthWest Standard
	BP_NE_STD,	//NorthEast Standard
	BP_SW_HND,	//SouthWest Handrail
	BP_SE_HND,	//SouthEast Handrail
	BP_NW_HND,	//NorthWest Handrail
	BP_NE_HND,	//NorthEast Handrail
	BP_NNE_STG,	//North North East Stage board 
	BP_ENE_STG,	//East  North East Stage board 
	BP_ESE_STG,	//East  South East Stage board 
	BP_SSE_STG,	//South South East Stage board 
	BP_SSW_STG,	//South South West Stage board 
	BP_WSW_STG,	//West  South West Stage board 
	BP_WNW_STG,	//West  North West Stage board 
	BP_NNW_STG,	//North North West Stage board 
	BP_NE_STG,	//East  North East Stage board corner
	BP_SE_STG,	//South South East Stage board corner
	BP_SW_STG,	//West  South West Stage board corner
	BP_NW_STG,	//North North West Stage board corner
	BP_NNE_WAL_TIE,	//NNE Wall Tie
	BP_NNE_BEG_TIE,	//NNE Begin Tie
	BP_NNE_END_TIE,	//NNE End Tie
	BP_ENE_WAL_TIE,	//ENE Wall Tie
	BP_ENE_BEG_TIE,	//ENE Begin Tie
	BP_ENE_END_TIE,	//ENE End Tie
	BP_ESE_WAL_TIE,	//ESE Wall Tie
	BP_ESE_BEG_TIE,	//ESE Begin Tie
	BP_ESE_END_TIE,	//ESE End Tie
	BP_SSE_WAL_TIE,	//SSE Wall Tie
	BP_SSE_BEG_TIE,	//SSE Begin Tie
	BP_SSE_END_TIE,	//SSE End Tie
	BP_SSW_WAL_TIE,	//SSW Wall Tie
	BP_SSW_BEG_TIE,	//SSW Begin Tie
	BP_SSW_END_TIE,	//SSW End Tie
	BP_WSW_WAL_TIE,	//WSW Wall Tie
	BP_WSW_BEG_TIE,	//WSW Begin Tie
	BP_WSW_END_TIE,	//WSW End Tie
	BP_WNW_WAL_TIE,	//WNW Wall Tie
	BP_WNW_BEG_TIE,	//WNW Begin Tie
	BP_WNW_END_TIE,	//WNW End Tie
	BP_NNW_WAL_TIE,	//NNW Wall Tie
	BP_NNW_BEG_TIE,	//NNW Begin Tie
	BP_NNW_END_TIE,	//NNW End Tie
	BP_NNE_BRC,	//North North East Brace
	BP_ENE_BRC,	//East  North East Brace
	BP_ESE_BRC,	//East  South East Brace
	BP_SSE_BRC,	//South South East Brace
	BP_SSW_BRC,	//South South West Brace
	BP_WSW_BRC,	//West  South West Brace
	BP_WNW_BRC,	//West  North West Brace
	BP_NNW_BRC,	//North North West Brace
	BP_END_ARW,	//Tail point of arrow
	BP_PNT_ARW,	//Pointy end of arrow
	BP_NTH_ARW,	//Northern point of arrow
	BP_STH_ARW,	//Southern point of arrow
	BP_E_STAIR,	//Eastern Stair point
	BP_W_STAIR,	//Western Stair point
	BP_1ASTAIR,	//Stair 1A point
	BP_2ASTAIR,	//Stair 2A point
	BP_3ASTAIR,	//Stair 3A point
	BP_4ASTAIR,	//Stair 4A point
	BP_5ASTAIR,	//Stair 5A point
	BP_6ASTAIR,	//Stair 6A point
	BP_1BSTAIR,	//Stair 1B point
	BP_2BSTAIR,	//Stair 2B point
	BP_3BSTAIR,	//Stair 3B point
	BP_4BSTAIR,	//Stair 4B point
	BP_5BSTAIR,	//Stair 5B point
	BP_6BSTAIR,	//Stair 6B point
	BP_N_LADDR,	//Northern Ladder point
	BP_C_LADDR,	//Central Ladder point
	BP_E_LADDR,	//Eastern Ladder point
	BP_1ALADDR,	//Ladder 1A point
	BP_2ALADDR,	//Ladder 2A point
	BP_3ALADDR,	//Ladder 3A point
	BP_4ALADDR,	//Ladder 4A point
	BP_5ALADDR,	//Ladder 5A point
	BP_1BLADDR,	//Ladder 1B point
	BP_2BLADDR,	//Ladder 2B point
	BP_3BLADDR,	//Ladder 3B point
	BP_4BLADDR,	//Ladder 4B point
	BP_5BLADDR,	//Ladder 5B point
	BP_N_MILLS,	//Northern Mills midpoint
	BP_E_MILLS,	//Northern Mills midpoint
	BP_S_MILLS,	//Southern Mills midpoint
	BP_W_MILLS,	//Northern Mills midpoint
	BP_N_M_STG,	//Northern Mills midpoint for Stageboard
	BP_E_M_STG,	//Northern Mills midpoint for Stageboard
	BP_S_M_STG,	//Southern Mills midpoint for Stageboard
	BP_W_M_STG,	//Northern Mills midpoint for Stageboard
	BP_BUT12_NW,	//North-western point for northern 1.2m Buttress Bay
	BP_BUT12_NE,	//North-eastern point for northern 1.2m Buttress Bay
	BP_BUT18_NW,	//North-western point for northern 1.8m Buttress Bay
	BP_BUT18_NE,	//North-eastern point for northern 1.8m Buttress Bay
	BP_BUT24_NW,	//North-western point for northern 2.4m Buttress Bay
	BP_BUT24_NE,	//North-eastern point for northern 2.4m Buttress Bay
	BP_CHAIN_NNE,	//NNE ChainMesh/Shade
	BP_CHAIN_ENE,	//ENE ChainMesh/Shade
	BP_CHAIN_ESE,	//ESE ChainMesh/Shade
	BP_CHAIN_SSE,	//SSE ChainMesh/Shade
	BP_CHAIN_SSW,	//SSW ChainMesh/Shade
	BP_CHAIN_WSW,	//WSW ChainMesh/Shade
	BP_CHAIN_WNW,	//WNW ChainMesh/Shade
	BP_CHAIN_NNW,	//NNW ChainMesh/Shade
	BP_SIZE		//Size of array
};

//////////////////////////////////////////////////////////////////////////////////
//HandRailOptionEnum
enum HandRailOptionEnum
{
	HRO_HRO,
	HRO_MESH,
	HRO_FULLMESH,
	HRO_NONE,
	HRO_INVALID =-1
};


//////////////////////////////////////////////////////////////////////////////////
//Lapboard Point arrangement
//
// 11_ _ _ _1_ _ _ _ _4_ _ _ _12
//         /|         |\
//       /  |_________|  \
//     /    2         3    \
//  0/                       \5
// 10\                       /
//     \    8_________7    /
//       \  |         |  /
// 13 _ _ _\|_ _ _ _ _|/_ _ _ 14
//          9         6
enum
{
	POINT_LAPBOARD_00,	//
	POINT_LAPBOARD_01,	//
	POINT_LAPBOARD_02,	//
	POINT_LAPBOARD_03,	//
	POINT_LAPBOARD_04,	//
	POINT_LAPBOARD_05,	//
	POINT_LAPBOARD_06,	//
	POINT_LAPBOARD_07,	//
	POINT_LAPBOARD_08,	//
	POINT_LAPBOARD_09,	//
	POINT_LAPBOARD_10,	//
	POINT_LAPBOARD_11,	//
	POINT_LAPBOARD_12,	//
	POINT_LAPBOARD_13,	//
	POINT_LAPBOARD_14
};
	
enum DirtyFlagEnum
{
	DF_CLEAN,
	DF_DELETE,
	DF_DIRTY
};


const double pi = 3.14159265358979;
const	double 	CONVERT_TO_DEG			= 180.00/pi;


//////////////////////////////////////////////////////////////////////////////
//Calculations/Conversion
//////////////////////////////////////////////////////////////////////////////
enum AxisEnum
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

const	Vector3D	VECTOR_ROTATE_AROUND_XAXIS( 1.00, 0.00, 0.00 );
const	Vector3D	VECTOR_ROTATE_AROUND_YAXIS( 0.00, 1.00, 0.00 );
const	Vector3D	VECTOR_ROTATE_AROUND_ZAXIS( 0.00, 0.00, 1.00 );

inline double CalculateAngle( Point3D &ptPoint, AxisEnum eAxis=X_AXIS )
{
	double dAngle;

	switch( eAxis )
	{
	default:		//Fallthrough
	case( X_AXIS ):	//angle around z axis from x axis
		if( ptPoint.x<=ROUND_ERROR_ANGLE && ptPoint.x>=-1.00*ROUND_ERROR_ANGLE )
		{
			if( ptPoint.y<=ROUND_ERROR_ANGLE && ptPoint.y>=-1.00*ROUND_ERROR_ANGLE )
			{
				//same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( ptPoint.y>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( ptPoint.x!=0.00 );
			dAngle = atan2( ptPoint.y, ptPoint.x );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	case( Y_AXIS ):	//angle around x axis from y axis
		if( ptPoint.y<=ROUND_ERROR_ANGLE && ptPoint.y>=-1.00*ROUND_ERROR_ANGLE )
		{
			if( ptPoint.z<=ROUND_ERROR_ANGLE && ptPoint.z>=-1.00*ROUND_ERROR_ANGLE )
			{
				//same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( ptPoint.z>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( ptPoint.y!=0.00 );
			dAngle = atan2( ptPoint.z, ptPoint.y );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	case( Z_AXIS ):	//angle around y axis from z axis
		if( ptPoint.z<=ROUND_ERROR_ANGLE && ptPoint.z>=-1.00*ROUND_ERROR_ANGLE )
		{
			if( ptPoint.x<=ROUND_ERROR_ANGLE && ptPoint.x>=-1.00*ROUND_ERROR_ANGLE )
			{
				//same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( ptPoint.x>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( ptPoint.z!=0.00 );
			dAngle = atan2( ptPoint.x, ptPoint.z );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	}

	return dAngle;
}

inline double CalculateAngle( Vector3D &Vector, AxisEnum eAxis=X_AXIS )
{
	double dAngle;

	switch( eAxis )
	{
	default:		//Fallthrough
	case( X_AXIS ):	//angle around z axis from x axis
		if( Vector.x<=ROUND_ERROR_ANGLE && Vector.x>=-1.00*ROUND_ERROR_ANGLE )
		{
			//X is zero
			if( Vector.y<=ROUND_ERROR_ANGLE && Vector.y>=-1.00*ROUND_ERROR_ANGLE )
			{
				//Y is zero - same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( Vector.y>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( Vector.x!=0.00 );
			dAngle = atan2( Vector.y, Vector.x );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	case( Y_AXIS ):	//angle around x axis from y axis
		if( Vector.y<=ROUND_ERROR_ANGLE && Vector.y>=-1.00*ROUND_ERROR_ANGLE )
		{
			if( Vector.z<=ROUND_ERROR_ANGLE && Vector.z>=-1.00*ROUND_ERROR_ANGLE )
			{
				//same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( Vector.z>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( Vector.y!=0.00 );
			dAngle = atan2( Vector.z, Vector.y );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	case( Z_AXIS ):	//angle around y axis from z axis
		if( Vector.z<=ROUND_ERROR_ANGLE && Vector.z>=-1.00*ROUND_ERROR_ANGLE )
		{
			if( Vector.x<=ROUND_ERROR_ANGLE && Vector.x>=-1.00*ROUND_ERROR_ANGLE )
			{
				//same point
				dAngle = 0.00;
			}
			else
			{
				dAngle = pi/2.00;
				dAngle*= ( Vector.x>=0.00 )? 1.00: -1.00;
			}
		}
		else
		{
			assert( Vector.z!=0.00 );
			dAngle = atan2( Vector.x, Vector.z );
			if( dAngle<=(pi/-2.00) )
				dAngle+= (2.00*pi);
		}
		break;
	}

	return dAngle;
}

inline Matrix3D CalculateRotationMatrix( Vector3D &From, Vector3D &To )
{
	double		dAngleX, dAngleY, dAngleZ;
	Vector3D	Vect;
	Matrix3D	Temp, Result;

	dAngleX	= CalculateAngle( To,	X_AXIS );
	dAngleX-= CalculateAngle( From,	X_AXIS );
	dAngleY	= CalculateAngle( To,	Y_AXIS );
	dAngleY-= CalculateAngle( From,	Y_AXIS );
	dAngleZ	= CalculateAngle( To,	Z_AXIS );
	dAngleZ-= CalculateAngle( From,	Z_AXIS );

	Result.setToRotation( dAngleX, VECTOR_ROTATE_AROUND_ZAXIS );
	Result.rotation( dAngleY, VECTOR_ROTATE_AROUND_XAXIS );
	Result.rotation( dAngleZ, VECTOR_ROTATE_AROUND_YAXIS );

	return Result;
}

inline Vector3D FromOriginToPointAsVector( Point3D pt )
{
	Vector3D Vector;
	Vector = pt - Point3D::kOrigin;
//	Vector.set( pt.x, pt.y, pt.z );
	return Vector;
}

#define	COMMA	_T(",")
const	CString	PROJECT_SECTION_NAME						= _T("Meccano");
const	CString	ERROR_MSG_NOT_OPEN_FILE_WRITE				= _T("I couldn't open the file to write to it:\n\n");
const	CString	ERROR_MSG_NOT_OPEN_FILE_READ				= _T("I couldn't open the file in read mode:\n\n");
const	CString	ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE			= _T("File Open Error");
const	CString	ERROR_MSG_INVALID_RL						= _T("Invalid Relative Level");

//Registry entries
const	CString	DEFAULT_RUN_FILENAME_SECTION				= _T("DefaultRun");
const	CString	SYDNEY_CORNER_FLAG							= _T("SydneyCorner");
const	CString	MILLS_SYSTEM_CORNER_FLAG					= _T("MillsSystemCorner");
const	CString	USE_LONGER_BRACING_FLAG						= _T("UseLongerBracing");
const	CString	WALL_OFFSET_FROM_BOTTOM_HOPUP				= _T("OffsetFromBottomHopup");
const	CString	USE_TOE_BOARD_ON_LAPBOARD					= _T("UseToeboardOnLapboard");
const	CString	SYDNEY_CORNER_SEPARATION					= _T("SydneyCornerSeparation");
const	CString	ACTUAL_COMPONENTS							= _T("ActualComponents");
const	CString	TITLE_LOAD_DEFAULT_RUN_FILE					= _T("Load Default Run");
const	CString	TITLE_SAVE_DEFAULT_RUN_FILE					= _T("Save Default Run");
const	CString	USE_MIDRAIL_WITH_CHAIN_MESH					= _T("Midrail with Chain");
const	CString	USE_LEDGER_EVERY_METRE						= _T("Ledger every metre");
const	CString	DELETE_SCHEMATIC_FLAG						= _T("Delete Schematic");

// Branch name file
const	CString	PROJECT_BRANCH_NAME_FILENAME_SECTION		= _T("Branch Name Filename");
const	CString	EXTENTION_BRANCH_NAME_FILE					= _T(".kbn");
const	CString	FILTER_BRANCH_NAME_FILE						= _T("KwikScaf Branch Name File(*.kbn)|*.kbn|All Files(*.*)|*.*||");
const	CString	TITLE_BRANCH_NAME_FILE						= _T("KwikScaf branch name file");

// component details csv defines
const	CString	PROJECT_COMPONENT_DETAILS_FILENAME_SECTION	= _T("ComponentDetailsFilename");
const	CString	PROJECT_COMPONENT_DETAILS_ENT_PATH_SECTION	= _T("ComponentDetailsEntityPath");
const	CString	TITLE_LOAD_COMPONENT_DETAILS_FILE			= _T("Component Details File");
const	CString	TITLE_LOAD_3D_COMPONENT_ENTITY_FILE			= _T("3D Component Entity File");
const	CString	EXTENTION_LOAD_COMPONENT_DETAILS_FILE		= _T(".csv");
const	CString	FILTER_LOAD_COMPONENT_DETAILS_FILE			= _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||");
const	CString	EXTENTION_LOAD_3D_ENTITY_FILE				= _T(".dwg");
const	CString	FILTER_LOAD_3D_ENTITY_FILE					= _T("AutoCAD Drawing File(*.dwg)|*.dwg|All Files(*.*)|*.*||");
const	CString	EXTENTION_DEFAULT_RUN_FILE					= _T(".kwr");
const	CString	FILTER_DEFAULT_RUN_FILE						= _T("KwikScaf Run File(*.kwr)|*.kwr|All Files(*.*)|*.*||");

// stock list csv defines
const	CString	PROJECT_STOCK_LIST_FILENAME_SECTION			= _T("StockListFilename");
const	CString	PROJECT_STOCK_LIST_ENT_PATH_SECTION			= _T("StockListEntityPath");
const	CString	TITLE_LOAD_STOCK_LIST_FILE					= _T("Stock List File");
const	CString	EXTENTION_LOAD_STOCK_LIST_FILE				= _T(".csv");
const	CString	FILTER_LOAD_STOCK_LIST_FILE					= _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||");

// drawing csv defines
const	CString	PROJECT_DRAWING_DETAILS_FILENAME_SECTION			= _T("DrawingDetailsFilename");
const	CString	PROJECT_DRAWING_DETAILS_ENT_PATH_SECTION			= _T("DrawingDetailsEntityPath");
const	CString	TITLE_LOAD_DRAWING_DETAILS_FILE						= _T("Drawing Details File");
const	CString	EXTENTION_LOAD_DRAWING_DETAILS_FILE					= _T(".csv");
const	CString	FILTER_LOAD_DRAWING_DETAILS_FILE					= _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||");

//BOM Summary output file
const	CString	EXTENTION_BOM_FILE							= _T(".csv");
const	CString	FILTER_BOM_FILE								= _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||");
const	CString	TITLE_SAVE_BOM_SUMMARY_FILE					= _T("BOM Summary File");
const	CString	EXTENTION_SAVE_BOM_SUMMARY_FILE				= _T(".csv");
const	CString	FILTER_SAVE_BOM_SUMMARY_FILE				= _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||");

//Actual Component Filenames
const	CString	TITLE_ACTUAL_DRAWING_FILE					= _T("AutoCAD drawing file");
const	CString	EXTENSION_ACTUAL_DRAWING_FILE				= _T(".dwg");
const	CString	FILTER_ACTUAL_DRAWING_FILE					= _T("Autocad Drawing File(*.dwg)|*.dwg|All Files(*.*)|*.*||");

//AVI defines
const	CString	AVI_FILENAME_SECTION						= _T("AviFilePath");

//Stock Component file
const	CString	KWIKSCAF_STOCK_COMPONENT_EXTENSION			= _T(".ksf");
const	CString	TITLE_KWIKSCAF_STOCK_SAVE_FILE				= _T("Export Stock File");
const	CString	TITLE_KWIKSCAF_STOCK_LOAD_FILE				= _T("Import Stock File");
const	CString	FILTER_KWIKSCAF_STOCK_FILE					= _T("KwikScaf Stock File(*.ksf)|*.ksf|All Files(*.*)|*.*||");

enum UseActualComponentsEnum
{
	UAC_NEITHER	= 0x00,
	UAC_MATRIX	= 0x01,
	UAC_3D_VIEW = 0x02,
	UAC_BOTH	= UAC_MATRIX|UAC_3D_VIEW
};

//Registry defaults
const	int		SYDNEY_CORNER_FLAG_DEFAULT					= FALSE;
const	int		USE_LONGER_BRACING_FLAG_DEFAULT				= TRUE;
const	int		MILLS_SYSTEM_CORNER_FLAG_DEFAULT			= FALSE;
const	int		WALL_OFFSET_FROM_BOTTOM_HOPUP_FLAG_DEFAULT	= TRUE;
const	int		USE_TOEBOARD_ON_LAPBOARDS_FLAG_DEFAULT		= TRUE;
const	int		SYDNEY_CORNER_SEPARATION_DEFAULT			= 100;
const	int		USE_MIDRAIL_WITH_CHAIN_MESH_DEFAULT			= FALSE;
const	int		USE_LEDGER_EVERY_METRE_DEFAULT				= FALSE;
const	int		ACTUAL_COMPONENTS_DEFAULT					= (int)UAC_NEITHER;
const	int		WINDOW_POSTION_X_DEFAULT					= -1;
const	int		WINDOW_POSTION_Y_DEFAULT					= -1;
const	int		DELETE_SCHEMATIC_FLAG_DEFAULT				= TRUE;

// Changed LPCTSTR to CString, ~SJ, 03.09.2007
inline CString GetStringInRegistry( CString sSectionName )
{
	//Get the filename from the registry
	CWinApp	*pApp;
	pApp = AfxGetApp();
	return pApp->GetProfileString(PROJECT_SECTION_NAME, sSectionName);
}

inline void SetStringInRegistry( CString sSectionName, LPCTSTR strFileName )
{
	//Store the filename in the registry
	CWinApp	*pApp;
	pApp = AfxGetApp();
	pApp->WriteProfileString(PROJECT_SECTION_NAME, sSectionName, strFileName);
}

inline int GetIntInRegistry( CString sSectionName, int iDefault=0 )
{
	//Get the filename from the registry
	CWinApp	*pApp;
	pApp = AfxGetApp();
	return pApp->GetProfileInt(PROJECT_SECTION_NAME, sSectionName, iDefault);
}

inline void SetIntInRegistry( CString sSectionName, int iValue )
{
	//Store the filename in the registry
	CWinApp	*pApp;
	pApp = AfxGetApp();
	pApp->WriteProfileInt(PROJECT_SECTION_NAME, sSectionName, iValue );
}

enum CommittedProportionEnum
{
	COMMIT_NONE,
	COMMIT_PARTIAL,
	COMMIT_FULL,
	COMMIT_INVALID
};

inline void ConvertNumberToString(int iNumber, CString &sText )
{
	const int	iLettersInAlphabet = 26;
	CString		sTemp;

	sText.Empty();
	do
	{
		sTemp.Format( _T("%c"), (_T('A')+(iNumber%iLettersInAlphabet)) );
		sText = sTemp + sText;

		iNumber = int( iNumber/iLettersInAlphabet );
	}
	while( iNumber>0 );
}

//Large numbers
const double	LARGE_NUMBER = 100000000.00;
const double	LARGE_NEGATIVE_NUMBER = -100000000.00;

//XData labels
const	CString XDATA_LABEL_COMPONENT	= _T("KwikScaf_Component");
enum XDATA_STRUCT_COMPONENT
{
	XDSC_APPNAME,			//XData name
	XDSC_COMPONENT_POINTER,	//Pointer to the component
	XDSC_POSITION,			//3D Position
	XDSC_DISPLACEMENT,		//Vector Direction of 1st arm
	XDSC_DIRECTION,			//Vector Direction of 2nd arm
	XDSC_DISTANCE,			//Distance moved
	XDSC_SCALE				//Scale factor
};
const	CString XDATA_LABEL_BAY			= _T("KwikScaf_Bay");
enum XDATA_STRUCT_BAY
{
	XDSB_APPNAME,		//XData name,
	XDSB_BAY_POINTER,	//Bay Pointer
	XDSB_DESCRIPTION,	//Description
	XDSB_POSITION,		//3D Position
	XDSB_DISPLACEMENT,	//Vector Direction of 1st arm
	XDSB_DIRECTION,		//Vector Direction of 2nd arm
	XDSB_DISTANCE,		//Distance moved
	XDSB_SCALE			//Scale factor
};
const	CString XDATA_LABEL_KUT_COMPONENT	= _T("KwikScaf_Kut_Component");
enum XDATA_STRUCT_KUT_COMPONENT
{
	XDKC_APPNAME,			//XData name
	XDKC_COMPONENT_POINTER,	//Pointer to the component
	XDKC_POSITION,			//3D Position
	XDKC_DISPLACEMENT,		//Vector Direction of 1st arm
	XDKC_DIRECTION,			//Vector Direction of 2nd arm
	XDKC_DISTANCE,			//Distance moved
	XDKC_SCALE				//Scale factor
};
const	CString XDATA_LABEL_PRESERVE	= _T("KwikScaf_Preserve_Flag");
enum XDATA_STRUCT_PRESERVE_FLAG
{
	XDPF_APPNAME,			//XData name
	XDPF_PRESERVE_FLAG		//Preserve Flag
};

//Extension Definitions
#include "ColourCode.h"

#endif MECCANODEFINITIONS


/*****************************************************************
 *	History Records
 *****************************************************************
 * $History: MeccanoDefinitions.h $
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 15/11/00   Time: 11:45a
 * Updated in $/Meccano/Stage 1/Code
 * Storage untested and integration of WFH
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:43p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 29/09/00   Time: 9:10a
 * Updated in $/Meccano/Stage 1/Code
 * Just about to change the mills & kwikscaf system from bool to enum
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 20/09/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Acad rotation is now working with individual components just need to
 * store b4 release to WA
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 19/09/00   Time: 5:08p
 * Updated in $/Meccano/Stage 1/Code
 * rotate seems to be working correctly now
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 11/08/00   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed crash where we couldn't _topen any previous drawings thanks to
 * AGT's change of fonts
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 9/08/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 25/07/00   Time: 5:07p
 * Updated in $/Meccano/Stage 1/Code
 * Column Ties are not oriented correctly
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 24/07/00   Time: 1:38p
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to B4 3D position exact started
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 18/07/00   Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 21/02/00   Time: 11:41a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 14/01/00   Time: 2:37p
 * Updated in $/Meccano/Stage 1/Code
 * Fixing the Dialog boxes at the moment
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 13/01/00   Time: 8:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 12/01/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 18/11/99   Time: 11:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:54a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 68  *****************
 * User: Dar          Date: 1/11/99    Time: 2:38p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 26/10/99   Time: 7:53a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 64  *****************
 * User: Dar          Date: 18/10/99   Time: 5:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 24/09/99   Time: 3:45p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 23/09/99   Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * All 3D Component Entity drawing now use the same path, and it will
 * prompt them for that path if it can't find it
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Dar          Date: 9/14/99    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 13/09/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 52  *****************
 * User: Dar          Date: 9/07/99    Time: 3:38p
 * Updated in $/Meccano/Stage 1/Code
 * MACROS BELOW For serialization conversion of bool type
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 7/09/99    Time: 3:30p
 * Updated in $/Meccano/Stage 1/Code
 * 1) plot line styles
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Dar          Date: 9/01/99    Time: 5:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 45  *****************
 * User: Dar          Date: 8/27/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 8/27/99    Time: 11:14a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:36a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/20/99    Time: 11:42a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/13/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Dar          Date: 8/12/99    Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * fixed double type for bracing every and ties every bay to ints
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/12/99    Time: 2:50p
 * Updated in $/Meccano/Stage 1/Code
 * Updating for andrews build
 * 
 * *****************  Version 34  *****************
 * User: Dar          Date: 8/12/99    Time: 11:34a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Dar          Date: 8/12/99    Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * added DELETE_PTR macro for safe deleting
 * 
 * *****************  Version 32  *****************
 * User: Dar          Date: 8/12/99    Time: 9:45a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/10/99    Time: 12:19p
 * Updated in $/Meccano/Stage 1/Code
 * Added the Entity class
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/05/99    Time: 3:49p
 * Updated in $/Meccano/Stage 1/Code
 * 1. Bays colour coded
 * 2. Tight fit
 * 3. Run length < mouse
 * 4. Birdcage Width < mouse width
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/05/99    Time: 2:07a
 * Updated in $/Meccano/Stage 1/Code
 * Birdcaging preview now works
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 27/07/99   Time: 12:02
 * Updated in $/Meccano/Stage 1/Code
 * Added MatrixFindResultsEnum
 * Added TieTypesEnum
 * Added JoinTypeEnum
 * 
 * *****************  Version 19  *****************
 * User: Dar          Date: 23/07/99   Time: 10:57a
 * Updated in $/Meccano/Stage 1/Code
 * added getComponentDescriptionString inline function utility for the BOM
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 23/07/99   Time: 10:27
 * Updated in $/Meccano/Stage 1/Code
 * removed left and right corner brackets
 * added other to material types
 * added jack length
 * added corner stage boards defines
 * changed deck planks
 * removed transom & ledger positions
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 21/07/99   Time: 14:10
 * Updated in $/Meccano/Stage 1/Code
 * Added ComponentPointsEnum, Stages&Levels
 * Changed SideOfBayEnum, CornerOfBayEnum
 * Removed ComponentPositionsEnum
 * 
 * *****************  Version 16  *****************
 * User: Dar          Date: 20/07/99   Time: 3:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Dar          Date: 20/07/99   Time: 1:14p
 * Updated in $/Meccano/Stage 1/Code
 * fixed two small syntax errors
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 20/07/99   Time: 13:04
 * Updated in $/Meccano/Stage 1/Code
 * Added some unit conversion, matrix stuff, and some extra defined
 * lengths
 * 
 * *****************  Version 13  *****************
 * User: Dar          Date: 16/07/99   Time: 2:19p
 * Updated in $/Meccano/Stage 1/Code
 * chnaged lift height to lift rise
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 16/07/99   Time: 14:17
 * Updated in $/Meccano/Stage 1/Code
 * Added the component length and star separation
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 16/07/99   Time: 11:30
 * Updated in $/Meccano/Stage 1/Code
 * Added a double array class
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 16/07/99   Time: 11:25
 * Updated in $/Meccano/Stage 1/Code
 * changed the LiftHeightEnum, added some LIFT_ID's, relocated the
 * COMPONENT POSITIONS
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 15/07/99   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 15/07/99   Time: 14:01
 * Updated in $/Meccano/Stage 1/Code
 * Added enums for sides
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 15/07/99   Time: 11:49
 * Updated in $/Meccano/Stage 1/Code
 * Fixed up some errors in the compiling
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 15/07/99   Time: 11:43
 * Updated in $/Meccano/Stage 1/Code
 * MFC should now be working via stdafx.h
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 15/07/99   Time: 9:51
 * Updated in $/Meccano/Stage 1/Code
 * I have added the Point3D, Vector3D & Matrix3D
 * 
 *****************************************************************/

 