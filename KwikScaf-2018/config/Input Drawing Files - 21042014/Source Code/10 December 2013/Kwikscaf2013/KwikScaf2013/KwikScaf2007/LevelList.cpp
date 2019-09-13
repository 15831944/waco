// LevelList.cpp: implementation of the LevelList class.
//
//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "LevelList.h"
#include "MeccanoDefinitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LevelList::LevelList()
{
	doubleArray::SetUniqueElementsOnly(true);
}

LevelList::~LevelList()
{

}

void LevelList::AddLevel(double dRL, bool bEnsureSeparation/*=true*/)
{
	int		i;
	if( bEnsureSeparation )
	{
		for( i=0; i<GetSize(); i++ )
		{
			if( dRL<GetAt(i) && dRL>GetAt(i)-STAR_SEPARATION )
			{
				//This is within STAR_SEPARATION of another level,
				//	move it down one star & try again
				dRL = GetAt(i)-STAR_SEPARATION;
				i=-1;
			}
			else if( dRL<GetAt(i)+STAR_SEPARATION && dRL>=GetAt(i) )
			{
				//this is within STAR_SEPARATION of another level,
				//	move it up one star & try again
				dRL = GetAt(i)+STAR_SEPARATION;
				i=-1;
			}
			else if( dRL<GetAt(i)+STAR_SEPARATION )
			{
				//missed it or it doesn't exist
				break;
			}
		}

		if( i>=GetSize() )
			doubleArray::Add( dRL );
		else
			doubleArray::InsertAt( i, dRL );
	}
	else
	{
		doubleArray::Add( dRL );
	}
}

bool LevelList::RemoveLevel(double dRL)
{
	int i;
	for( i=0; i<GetSize(); i++ )
	{
		if( dRL<GetAt(i)+ROUND_ERROR && dRL>GetAt(i)-ROUND_ERROR )
			return RemoveLevel(i);
	}
	return false;
}

bool LevelList::RemoveLevel(int iLevelID, int nCount/*=1*/)
{
	if( iLevelID<0 || iLevelID+nCount>GetSize() )
		return false;

	doubleArray::RemoveAt( iLevelID, nCount );
	return true;
}

double LevelList::GetLevel(int iPos)
{
	assert( iPos>=0 );
	assert( iPos<GetSize() );
	return GetAt(iPos);
}

int LevelList::GetSize()
{
	return doubleArray::GetSize();
}

void LevelList::RemoveAll()
{
	doubleArray::RemoveAll();
}

//NOTE: for the purposes of this function, iLevel must be 0 based
//	not 1 based!
void LevelList::GetRLLimitsForLevel(int iLevel, double &dRLLower, double &dRLUpper)
{
	int iSize;

	//This is the pattern we are trying to form:
	//	Where:
	//	-1x10^6 == LARGE_NEGATIVE_NUMBER
	//	 1x10^6 == LARGE_NUMBER
	//	L[n]	== GetAt(n)
	//	and each cell contains dRLLower above dRLUpper!
	//			|					iLevel                      	
	//			|		0|		1|		2|		3|		4|		5|  ->
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		 0  |-1x10^6 |		 |		 |		 |		 |		 |
	//		    | 1x10^6 |		 |		 |		 |		 |		 |
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		 1  |-1x10^6 | L[0]	 |		 |		 |		 |		 |
	//		    | L[0]	 | 1x10^6|		 |		 |		 |		 |
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		 2  |-1x10^6 | L[0]	 | L[1]	 |		 |		 |		 |
	//		    | L[0]	 | L[1]	 | 1x10^6|		 |		 |		 |
	//iSize-----+--------+-------+-------+-------+-------+-------+----
	//		 3  |-1x10^6 | L[0]	 | L[1]	 | L[2]	 |		 |		 |
	//		    | L[0]	 | L[1]	 | L[2]	 | 1x10^6|		 |		 |
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		 4  |-1x10^6 | L[0]	 | L[1]	 | L[2]	 | L[3]	 |		 |
	//		    | L[0]	 | L[1]	 | L[2]	 | L[3]	 | 1x10^6|		 |
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		 5  |-1x10^6 | L[0]	 | L[1]	 | L[2]	 | L[3]	 | L[4]	 |
	//		    | L[0]	 | L[1]  | L[2]	 | L[3]	 | L[4]	 | 1x10^6|
	//     -----+--------+-------+-------+-------+-------+-------+----
	//		    |		 |		 |		 |		 |		 |		 |

	dRLUpper= LARGE_NUMBER;
	dRLLower= LARGE_NEGATIVE_NUMBER;
	iSize = GetSize();

	assert(iLevel>=0);
	assert(iLevel<=iSize);
	if( iSize>0 )
	{
		if( iLevel==iSize )
		{
			//don't change dRLUpper
			dRLLower = GetLevel(iLevel-1);
		}
		else if( iLevel==0 )
		{
			//don't change dRLLower
			dRLUpper = GetLevel(iLevel);
		}
		else
		{
			dRLLower = GetLevel(iLevel-1);
			dRLUpper = GetLevel(iLevel);
		}
	}
	assert( dRLUpper>dRLLower );
}

int LevelList::GetLevelForRL(double dRL)
{
	int			iLvl;
	double		dLevel;

	for( iLvl=0; iLvl<GetSize(); iLvl++ )
	{
		dLevel = GetLevel(iLvl);
		if( dRL<dLevel )
		{
			break;
		}
	}
	iLvl++;
	return iLvl;
}

double LevelList::GetMaximumSpacing()
{
	double dMaxSpacing, dMinSpacing;
	if( !GetMaxMinSpacing( dMaxSpacing, dMinSpacing ) )
		return -1.00;
	return dMaxSpacing;
}

double LevelList::GetMinimumSpacing()
{
	double dMaxSpacing, dMinSpacing;
	if( !GetMaxMinSpacing( dMaxSpacing, dMinSpacing ) )
		return -1.00;
	return dMinSpacing;
}

bool LevelList::GetMaxMinSpacing(double &dMaxSpacing, double &dMinSpacing)
{
	int			jLvl;
	bool		bFound;
	double		dLevel, dSpacing;

	dMinSpacing = LARGE_NUMBER;
	dMaxSpacing = LARGE_NEGATIVE_NUMBER;

	bFound = false;
	for( int iLvl=0; iLvl<GetSize()-1; iLvl++ )
	{
		dLevel = GetLevel(iLvl);
		for( jLvl=iLvl+1; jLvl<GetSize(); jLvl++ )
		{
			if( iLvl!=jLvl )
			{
				bFound = true;
				dSpacing = fabs(GetLevel(jLvl)-dLevel);
				dMaxSpacing = max( dSpacing, dMaxSpacing );
				dMinSpacing = min( dSpacing, dMinSpacing );
			}
		}
	}
	return bFound;
}

void LevelList::SelectionSort(bool bIncremental/*=true*/)
{
	doubleArray::SelectionSort(bIncremental);
}
