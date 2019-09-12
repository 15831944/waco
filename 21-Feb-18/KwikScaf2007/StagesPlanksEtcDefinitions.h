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
//                              StagesPlanksEtcDefinitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 27/06/00 4:52p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/STAGESPLANKSETCDEFINITIONS.H $
//	Version			: $Revision: 8 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	__STAGES_PLANKS_ETC_DEFINITIONS_H__
#define	__STAGES_PLANKS_ETC_DEFINITIONS_H__

//#define	ALLOW_1X1_STAGE_BRD


/////////////////////////////////////////////////////////////////////
//Corner Stage Board Size
//The reason this value is a double and not a enum is because this
//	value will be passed along as the length of the stageoboard
enum CornerStageBoardRiseEnum
{
	CNR_STAGE_BOARD_RISE_1X1,
	CNR_STAGE_BOARD_RISE_2X2,
	CNR_STAGE_BOARD_RISE_3X3,
	CNR_STAGE_BOARD_RISE_1X2L,		//left 1x2
	CNR_STAGE_BOARD_RISE_1X3L,		//left 1x3
	CNR_STAGE_BOARD_RISE_2X3L,		//left 2x3
	CNR_STAGE_BOARD_RISE_2X1L,		//left 2x1
	CNR_STAGE_BOARD_RISE_3X1L,		//left 3x1
	CNR_STAGE_BOARD_RISE_3X2L,		//left 3x2
};
const int CNR_STAGE_BOARD_RISE_1X2R	= CNR_STAGE_BOARD_RISE_2X1L;	//right 1x2
const int CNR_STAGE_BOARD_RISE_1X3R	= CNR_STAGE_BOARD_RISE_3X1L;	//right 1x3
const int CNR_STAGE_BOARD_RISE_2X3R	= CNR_STAGE_BOARD_RISE_3X2L;	//right 2x3
const int CNR_STAGE_BOARD_RISE_2X1R	= CNR_STAGE_BOARD_RISE_1X2L;	//right 1x2
const int CNR_STAGE_BOARD_RISE_3X1R	= CNR_STAGE_BOARD_RISE_1X3L;	//right 1x3
const int CNR_STAGE_BOARD_RISE_3X2R	= CNR_STAGE_BOARD_RISE_2X3L;	//right 2x3


/////////////////////////////////////////////////////////////////////
//	PLANK Positions
//This value determines the position of the plank as viewed from above
//	PLANK01 would be the southern (or inner) most plank.  This value is
//	passed around as a Rise value, hence the use of the Rise in the
//	enum name.  We can use the rise because the rise has no meaning
//	to planks, since they are always at the level of the decking
enum PlankPosRiseEnum
{
	PLANK01,	//southern most (or, the inner most, for stageboards)
	PLANK02,
	PLANK03,	//Size of 700 bay (maximum value for stageboards)
	PLANK04,
	PLANK05,	//Size for 1200 bay
	PLANK06,
	PLANK07,	//Size for 1800 bay
	PLANK08,
	PLANK09,
	PLANK10		//Size for 2400 bay
};

/////////////////////////////////////////////////////////////////////
//Number of Planks required
//These are some defines for the number of planks required for each bay and
//	and for stage board size
const int	NUM_PLANKS_FOR_STAGE_BRD_1_WIDE	= 1;
const int	NUM_PLANKS_FOR_STAGE_BRD_2_WIDE	= 2;
const int	NUM_PLANKS_FOR_STAGE_BRD_3_WIDE	= 3;
const int	NUM_PLANKS_FOR_BAY_SIZE_0700	= 3;
const int	NUM_PLANKS_FOR_BAY_SIZE_1200	= 5;
const int	NUM_PLANKS_FOR_BAY_SIZE_1800	= 7;
const int	NUM_PLANKS_FOR_BAY_SIZE_2400	= 10;

inline int GetNumberOfPlanksForWidth( double dWidth )
{
	int	iNumberOfPlanks;

	if( dWidth<=COMPONENT_LENGTH_0700+ROUND_ERROR )
		iNumberOfPlanks = NUM_PLANKS_FOR_BAY_SIZE_0700;
	else if( dWidth<=COMPONENT_LENGTH_1200+ROUND_ERROR )
		iNumberOfPlanks = NUM_PLANKS_FOR_BAY_SIZE_1200;
	else if( dWidth<= COMPONENT_LENGTH_1800+ROUND_ERROR )
		iNumberOfPlanks = NUM_PLANKS_FOR_BAY_SIZE_1800;
	else
		iNumberOfPlanks = NUM_PLANKS_FOR_BAY_SIZE_2400;

	return iNumberOfPlanks;
}

inline double GetWidthForNumberOfPlanks( int iNumberOfPlanks )
{
	double dWidth;

	if( iNumberOfPlanks<= NUM_PLANKS_FOR_BAY_SIZE_0700 )
		dWidth=COMPONENT_LENGTH_0700;
	else if( iNumberOfPlanks<= NUM_PLANKS_FOR_BAY_SIZE_1200 )
		dWidth=COMPONENT_LENGTH_1200;
	else if( iNumberOfPlanks<= NUM_PLANKS_FOR_BAY_SIZE_1800 )
		dWidth=COMPONENT_LENGTH_1800;
	else
		dWidth=COMPONENT_LENGTH_2400;

	return dWidth;
}


/////////////////////////////////////////////////////////////////////
//Hop up bracket positions
//This is used to for hop up boards which only occur at corners and 
//	also reqruire a further direction since there are 2 possible
//	positions per corner
//
//     NNW   NNE
//     _|     |_
//  WNW         ENE
//
//  WSW_       _ESE
//      |     |
//     SSW   SSE
//
enum BracketCornerOfBayEnum
{
	CNR_NORTH_NORTH_EAST,
	CNR_EAST_NORTH_EAST,
	CNR_EAST_SOUTH_EAST,
	CNR_SOUTH_SOUTH_EAST,
	CNR_SOUTH_SOUTH_WEST,
	CNR_WEST_SOUTH_WEST,
	CNR_WEST_NORTH_WEST,
	CNR_NORTH_NORTH_WEST
};

inline SideOfBayEnum BracketCornerAsSideOfBay( BracketCornerOfBayEnum eBracketCorner )
{
	switch( eBracketCorner )
	{
		default:
		case( CNR_NORTH_NORTH_EAST ):	return NORTH_NORTH_EAST;
		case( CNR_EAST_NORTH_EAST ):	return EAST_NORTH_EAST;
		case( CNR_EAST_SOUTH_EAST ):	return EAST_SOUTH_EAST;
		case( CNR_SOUTH_SOUTH_EAST ):	return SOUTH_SOUTH_EAST;
		case( CNR_SOUTH_SOUTH_WEST ):	return SOUTH_SOUTH_WEST;
		case( CNR_WEST_SOUTH_WEST ):	return WEST_SOUTH_WEST;
		case( CNR_WEST_NORTH_WEST ):	return WEST_NORTH_WEST;
		case( CNR_NORTH_NORTH_WEST ):	return NORTH_NORTH_WEST;
	}
	return NORTH_NORTH_EAST;
}

inline BracketCornerOfBayEnum SideOfBayAsBracketCorner( SideOfBayEnum eSide )
{
	switch( eSide )
	{
		default:
		case( NORTH_NORTH_EAST ):	return CNR_NORTH_NORTH_EAST;
		case( EAST_NORTH_EAST ):	return CNR_EAST_NORTH_EAST;
		case( EAST_SOUTH_EAST ):	return CNR_EAST_SOUTH_EAST;
		case( SOUTH_SOUTH_EAST ):	return CNR_SOUTH_SOUTH_EAST;
		case( SOUTH_SOUTH_WEST ):	return CNR_SOUTH_SOUTH_WEST;
		case( WEST_SOUTH_WEST ):	return CNR_WEST_SOUTH_WEST;
		case( WEST_NORTH_WEST ):	return CNR_WEST_NORTH_WEST;
		case( NORTH_NORTH_WEST ):	return CNR_NORTH_NORTH_WEST;
	}
	return CNR_NORTH_NORTH_EAST;
}

inline int GetCornerFillerFromDimensions( int iLeft, int iRight )
{
	int iSize;
	//always consider this as a left had component
	switch( iLeft )
	{
	case( NUM_PLANKS_FOR_STAGE_BRD_3_WIDE ):
		switch( iRight )
		{
		case( NUM_PLANKS_FOR_STAGE_BRD_3_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_3X3;
			break;
		default:									//fallthrough
		case( NUM_PLANKS_FOR_STAGE_BRD_2_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_2X3L;
			break;
		case( NUM_PLANKS_FOR_STAGE_BRD_1_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_1X3L;
			break;
		}
		break;
	default:										//fallthrough				
	case( NUM_PLANKS_FOR_STAGE_BRD_2_WIDE ):
		switch( iRight )
		{
		case( NUM_PLANKS_FOR_STAGE_BRD_3_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_3X2L;
			break;
		default:									//fallthrough
		case( NUM_PLANKS_FOR_STAGE_BRD_2_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_2X2;
			break;
		case( NUM_PLANKS_FOR_STAGE_BRD_1_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_1X2L;
			break;
		}
		break;
	case( NUM_PLANKS_FOR_STAGE_BRD_1_WIDE ):
		switch( iRight )
		{
		case( NUM_PLANKS_FOR_STAGE_BRD_3_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_3X1L;
			break;
		default:									//fallthrough
		case( NUM_PLANKS_FOR_STAGE_BRD_2_WIDE ):
			iSize = CNR_STAGE_BOARD_RISE_2X1L;
			break;
		case( NUM_PLANKS_FOR_STAGE_BRD_1_WIDE ):
#ifdef	ALLOW_1X1_STAGE_BRD
			iSize = CNR_STAGE_BOARD_RISE_1X1;
			break;
#else	//#ifdef	ALLOW_1X1_STAGE_BRD
			return false;
#endif	//#ifdef	ALLOW_1X1_STAGE_BRD
		}
		break;
	}
	return iSize;
}

inline bool GetDimensionsForCornerFiller( int iSize, int &iLeft, int &iRight )
{
	bool bReturn = true;
	if( iSize==CNR_STAGE_BOARD_RISE_2X2 )
	{
		iLeft	= 2;
		iRight	= 2;
	}
#ifdef	ALLOW_1X1_STAGE_BRD
	else if( iSize== CNR_STAGE_BOARD_RISE_1X1)
	{
		iLeft	= 1;
		iRight	= 1;
	}
#endif	//#ifdef	ALLOW_1X1_STAGE_BRD
	else if( iSize==CNR_STAGE_BOARD_RISE_3X3 )
	{
		iLeft	= 3;
		iRight	= 3;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_1X2L )
	{
		iLeft	= 1;
		iRight	= 2;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_1X3L )
	{
		iLeft	= 1;
		iRight	= 3;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_2X3L )
	{
		iLeft	= 2;
		iRight	= 3;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_2X1L )
	{
		iLeft	= 2;
		iRight	= 1;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_3X1L )
	{
		iLeft	= 3;
		iRight	= 1;
	}
	else if( iSize==CNR_STAGE_BOARD_RISE_3X2L )
	{
		iLeft	= 3;
		iRight	= 2;
	}
	else
	{
		;//assert( false );
		bReturn = false;
	}
	return bReturn;
}


#endif	//#ifndef	__STAGES_PLANKS_ETC_DEFINITIONS_H__

/*******************************************************************************
*	History Records
********************************************************************************
* $History: StagesPlanksEtcDefinitions.h $
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
*
*******************************************************************************/
