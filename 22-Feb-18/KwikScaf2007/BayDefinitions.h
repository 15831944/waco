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
//                              BayDefinitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 14/12/00 9:52a $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/BAYDEFINITIONS.H $
//	Version			: $Revision: 28 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	__BAY_DEFINITIONS_H__
#define	__BAY_DEFINITIONS_H__


#include "ComponentDefinitions.h"

enum	BayTypeEnum
{
	BAY_TYPE_BAY,
	BAY_TYPE_LAPBOARD,
	BAY_TYPE_STAIRS,
	BAY_TYPE_LADDER,
	BAY_TYPE_BUTTRESS,
	BAY_TYPE_UNDEFINED = -1
};

typedef unsigned int MillsType;
const MillsType	MILLS_TYPE_NONE					= 0x0000;
const MillsType	MILLS_TYPE_MISSING_STND_TEST	= 0x000F;
const MillsType	MILLS_TYPE_MISSING_STND_NE		= 0x0001;
const MillsType	MILLS_TYPE_MISSING_STND_SE		= 0x0002;
const MillsType	MILLS_TYPE_MISSING_STND_SW		= 0x0004;
const MillsType	MILLS_TYPE_MISSING_STND_NW		= 0x0008;
const MillsType	MILLS_TYPE_CONNECT_TEST			= 0xFFF0;
const MillsType	MILLS_TYPE_CONNECT_N			= 0x0010;
const MillsType	MILLS_TYPE_CONNECT_E			= 0x0020;
const MillsType	MILLS_TYPE_CONNECT_S			= 0x0040;
const MillsType	MILLS_TYPE_CONNECT_W			= 0x0080;
const MillsType	MILLS_TYPE_CONNECT_NNE			= 0x0100;
const MillsType	MILLS_TYPE_CONNECT_ENE			= 0x0200;
const MillsType	MILLS_TYPE_CONNECT_ESE			= 0x0400;
const MillsType	MILLS_TYPE_CONNECT_SSE			= 0x0800;
const MillsType	MILLS_TYPE_CONNECT_SSW			= 0x1000;
const MillsType	MILLS_TYPE_CONNECT_WSW			= 0x2000;
const MillsType	MILLS_TYPE_CONNECT_WNW			= 0x4000;
const MillsType	MILLS_TYPE_CONNECT_NNW			= 0x8000;

/////////////////////////////////////////////////////////////////////
//Standard Lengths
//
//returns 0 if no more types
enum StandardLengthsEnum
{
	STANDARD_LENGTH_0500,
	STANDARD_LENGTH_0800,
	STANDARD_LENGTH_1000,
	STANDARD_LENGTH_1500,
	STANDARD_LENGTH_2000,
	STANDARD_LENGTH_2500,
	STANDARD_LENGTH_3000
};

inline double StandardLengths(int index)
{
	switch( (StandardLengthsEnum)index )
	{
		case( STANDARD_LENGTH_0500 ):	return COMPONENT_LENGTH_0500;
		case( STANDARD_LENGTH_0800 ):	return COMPONENT_LENGTH_0800;
		case( STANDARD_LENGTH_1000 ):	return COMPONENT_LENGTH_1000;
		case( STANDARD_LENGTH_1500 ):	return COMPONENT_LENGTH_1500;
		case( STANDARD_LENGTH_2000 ):	return COMPONENT_LENGTH_2000;
		case( STANDARD_LENGTH_2500 ):	return COMPONENT_LENGTH_2500;
		case( STANDARD_LENGTH_3000 ):	return COMPONENT_LENGTH_3000;
		default	 :	return 0;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
//Bay Lengths
//
//returns 0 if no more types
inline double BayLengths(int index)
{
	switch( index )
	{
		case( 0 ):	return COMPONENT_LENGTH_0700;
		case( 1 ):	return COMPONENT_LENGTH_1200;
		case( 2 ):	return COMPONENT_LENGTH_1800;
		case( 3 ):	return COMPONENT_LENGTH_2400;
		default	 :	return 0;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
//Bay Widths
//
//returns 0 if no more types
inline double BayWidths(int index)
{
	switch( index )
	{
		case( 0 ):	return COMPONENT_LENGTH_0700;
		case( 1 ):	return COMPONENT_LENGTH_1200;
		case( 2 ):	return COMPONENT_LENGTH_1800;
		case( 3 ):	return COMPONENT_LENGTH_2400;
		default	 :	return 0;
	}
	return 0;
}

//The actual kwikstage bay lengths converstions are:
//2.4m = 2439mm
//1.8m = 1829mm
//1.2m = 1270mm
//0.7m = 0762mm
const double	BAY_LENGTH_ACTUAL_2400 = 2439.00;
const double	BAY_LENGTH_ACTUAL_1800 = 1829.00;
const double	BAY_LENGTH_ACTUAL_1200 = 1270.00;
const double	BAY_LENGTH_ACTUAL_0700 = 762.00;

/////////////////////////////////////////////////////////////////////
//Conversion to lengths
//
//returns 0 if no more types
inline double GetCommonBayLength(double dActualLength)
{
	double dLength;
	;//assert( dActualLength>COMPONENT_LENGTH_0600 );
	;//assert( dActualLength<COMPONENT_LENGTH_2500 );

	if( dActualLength<BAY_LENGTH_ACTUAL_0700+0.50 )
	{
		dLength = COMPONENT_LENGTH_0700;
	}
	else if( dActualLength<BAY_LENGTH_ACTUAL_1200+0.50 )
	{
		dLength = COMPONENT_LENGTH_1200;
	}
	else if( dActualLength<BAY_LENGTH_ACTUAL_1800+0.50 )
	{
		dLength = COMPONENT_LENGTH_1800;
	}
	else if( dActualLength<BAY_LENGTH_ACTUAL_2400+0.50 )
	{
		dLength = COMPONENT_LENGTH_2400;
	}
	else
	{
		dLength = COMPONENT_LENGTH_2400;
	}

	return dLength;
}


/////////////////////////////////////////////////////////////////////
//Bay Stuff
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Sides of a bay
//This is used by items most items that don't specifically exist in
//	corners only.  N,E,S,W are first since these are the most common
enum SideOfBayEnum
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NORTH_EAST,
	SOUTH_EAST,
	NORTH_WEST,
	SOUTH_WEST,
	NORTH_NORTH_EAST,
	EAST_NORTH_EAST,
	EAST_SOUTH_EAST,
	SOUTH_SOUTH_EAST,
	SOUTH_SOUTH_WEST,
	WEST_SOUTH_WEST,
	WEST_NORTH_WEST,
	NORTH_NORTH_WEST,
	ALL_SIDES			= -1,	//N, E, S, W, etc
	ALL_VISIBLE_SIDES	= -2,	//Need to look at pForward, pBackward, etc.
	SIDE_INVALID		= -3
};

const SideOfBayEnum N	= NORTH; 
const SideOfBayEnum E	= EAST;
const SideOfBayEnum S	= SOUTH;
const SideOfBayEnum W	= WEST;
const SideOfBayEnum NE	= NORTH_EAST;
const SideOfBayEnum SE	= SOUTH_EAST;
const SideOfBayEnum NW	= NORTH_WEST;
const SideOfBayEnum SW	= SOUTH_WEST;
const SideOfBayEnum NNE	= NORTH_NORTH_EAST;
const SideOfBayEnum ENE	= EAST_NORTH_EAST;
const SideOfBayEnum ESE	= EAST_SOUTH_EAST;
const SideOfBayEnum SSE	= SOUTH_SOUTH_EAST;
const SideOfBayEnum SSW	= SOUTH_SOUTH_WEST;
const SideOfBayEnum WSW	= WEST_SOUTH_WEST;
const SideOfBayEnum WNW	= WEST_NORTH_WEST;
const SideOfBayEnum NNW	= NORTH_NORTH_WEST;


/////////////////////////////////////////////////////////////////////
// GetSideOfBayDescStr						INLINE FUNCTION
//
// This function gets the descriptive string for the side of bay enum arguement.
// The return value is a CString.

inline CString GetSideOfBayDescStr( SideOfBayEnum eSide )
{
	switch( eSide )
	{
		case( NORTH )				: return _T("North");
		case( EAST )				: return _T("East");
		case( SOUTH )				: return _T("South");
		case( WEST )				: return _T("West");
		case( NORTH_EAST )			: return _T("North East");
		case( SOUTH_EAST )			: return _T("South East");
		case( NORTH_WEST )			: return _T("North West");
		case( SOUTH_WEST )			: return _T("South West");
		case( NORTH_NORTH_EAST )	: return _T("North North East");
		case( EAST_NORTH_EAST )		: return _T("East North East");
		case( EAST_SOUTH_EAST )		: return _T("East South East");
		case( SOUTH_SOUTH_EAST )	: return _T("South South East");
		case( SOUTH_SOUTH_WEST )	: return _T("South South West");
		case( WEST_SOUTH_WEST )		: return _T("West South West");
		case( WEST_NORTH_WEST )		: return _T("West North West");
		case( NORTH_NORTH_WEST )	: return _T("North North West");
		default:
			;//assert( false );
			return _T("Unknown Side");
	};
	;//assert( false );
	return _T("");
}


/////////////////////////////////////////////////////////////////////
// GetSideOfBayDescStrAlign						INLINE FUNCTION
//
// This function gets the descriptive string for the Side of Bay enum arguement.
//	However, all the text enteries are of the same length to align easily
// The return value is a CString.

inline CString GetSideOfBayDescStrAlign( SideOfBayEnum eSide )
{
	switch( eSide )
	{
		case( NORTH )				: return _T("North           ");
		case( EAST )				: return _T("East            ");
		case( SOUTH )				: return _T("South           ");
		case( WEST )				: return _T("West            ");
		case( NORTH_EAST )			: return _T("North East      ");
		case( SOUTH_EAST )			: return _T("South East      ");
		case( NORTH_WEST )			: return _T("North West      ");
		case( SOUTH_WEST )			: return _T("South West      ");
		case( NORTH_NORTH_EAST )	: return _T("North North East");
		case( EAST_NORTH_EAST )		: return _T("East North East ");
		case( EAST_SOUTH_EAST )		: return _T("East South East ");
		case( SOUTH_SOUTH_EAST )	: return _T("South South East");
		case( SOUTH_SOUTH_WEST )	: return _T("South South West");
		case( WEST_SOUTH_WEST )		: return _T("West South West ");
		case( WEST_NORTH_WEST )		: return _T("West North West ");
		case( NORTH_NORTH_WEST )	: return _T("North North West");
		default:
			;//assert( false );
			return _T("Unknown Side    ");
	};
	;//assert( false );
	return _T("");
}

inline SideOfBayEnum GetOppositeSide( SideOfBayEnum eSide )
{
	SideOfBayEnum eOpposite;

	eOpposite = SIDE_INVALID;
	switch( eSide )
	{
	case( NORTH ):		eOpposite = SOUTH;	break;
	case( EAST ):		eOpposite = WEST;	break;
	case( SOUTH ):		eOpposite = NORTH;	break;
	case( WEST ):		eOpposite = EAST;	break;
	case( NORTH_EAST ):		eOpposite = SOUTH_WEST;	break;
	case( SOUTH_EAST ):		eOpposite = NORTH_WEST;	break;
	case( NORTH_WEST ):		eOpposite = SOUTH_EAST;	break;
	case( SOUTH_WEST ):		eOpposite = NORTH_EAST;	break;
	case( NORTH_NORTH_EAST ):		eOpposite = SOUTH_SOUTH_WEST;	break;
	case( EAST_NORTH_EAST ):		eOpposite = WEST_SOUTH_WEST;	break;
	case( EAST_SOUTH_EAST ):		eOpposite = WEST_NORTH_WEST;	break;
	case( SOUTH_SOUTH_EAST ):		eOpposite = NORTH_NORTH_WEST;	break;
	case( SOUTH_SOUTH_WEST ):		eOpposite = NORTH_NORTH_EAST;	break;
	case( WEST_SOUTH_WEST ):		eOpposite = EAST_NORTH_EAST;	break;
	case( WEST_NORTH_WEST ):		eOpposite = EAST_SOUTH_EAST;	break;
	case( NORTH_NORTH_WEST ):		eOpposite = SOUTH_SOUTH_EAST;	break;
	default:
		;//assert( false );
	}
	return eOpposite;
}


/////////////////////////////////////////////////////////////////////
//Corners of the Bay
//This is used to for items which only occur at corners such as
//	Standards and corner stages
enum CornerOfBayEnum
{
	CNR_NORTH_EAST,
	CNR_SOUTH_EAST,
	CNR_NORTH_WEST,
	CNR_SOUTH_WEST,
	CNR_ALL_SIDES	= -1,	//
	CNR_INVALID		= -3	//not valid
};

inline SideOfBayEnum CornerAsSideOfBay( CornerOfBayEnum eCorner )
{
	switch( eCorner )
	{
		default:	//fallthrough
		case( CNR_SOUTH_EAST ):	return SOUTH_EAST;
		case( CNR_NORTH_EAST ):	return NORTH_EAST;
		case( CNR_NORTH_WEST ):	return NORTH_WEST;
		case( CNR_SOUTH_WEST ):	return SOUTH_WEST;
	}
	return SOUTH_EAST;
}

inline CornerOfBayEnum SideOfBayAsCorner( SideOfBayEnum eSide )
{
	switch( eSide )
	{
		default:	//fallthrough
		case( SOUTH_EAST ):	return CNR_SOUTH_EAST;
		case( NORTH_EAST ):	return CNR_NORTH_EAST;
		case( NORTH_WEST ):	return CNR_NORTH_WEST;
		case( SOUTH_WEST ):	return CNR_SOUTH_WEST;
	}
	return CNR_SOUTH_EAST;
}

enum TextPositionEnum
{
	TP_BAY_NUMBER,
	TP_BAY_LENGTH,
	TP_MATRIX_FORWARD,
	TP_MATRIX_BACKWARD,
	TP_TIE_TUBE_LENGTH
};

enum BayDirectionEnum
{
	DIRECTION_FORWARD,
	DIRECTION_BACKWARD,
	DIRECTION_OUTER,
	DIRECTION_INNER,
	DIRECTION_CORNER_FORWARD,
	DIRECTION_CORNER_BACKWARD
};

enum	RoofProtectionEnum
{
	RP_NONE,
	RP_1500MM,
	RP_2000MM,
	RP_INVALID = -1
};

const	double	GRAVITY_TO_STANDARD		= 300.00;

#endif	//#ifndef	__BAY_DEFINITIONS_H__

/*******************************************************************************
*	History Records
********************************************************************************
* $History: BayDefinitions.h $
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 14/12/00   Time: 9:52a
 * Updated in $/Meccano/Stage 1/Code
 * 500 and 800mm standards seem to be working
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 6/07/00    Time: 12:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 26/10/99   Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
*
*******************************************************************************/
