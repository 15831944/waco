// doubleArray.cpp: implementation of the doubleArray class.
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
#include "doubleArray.h"
#include "MeccanoDefinitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

doubleArray::doubleArray()
{
	SetUniqueElementsOnly(false);
	m_bOrderImportant = false;
}

doubleArray::doubleArray(const doubleArray &Original)
{
	SetUniqueElementsOnly(false);
	*this = Original;
	m_bOrderImportant = false;
}

doubleArray::~doubleArray()
{
	RemoveAll();
}

doubleArray& doubleArray::operator=( const doubleArray &Original )
{
	double	dValue;
	int i;

	RemoveAll();
	for( i=0; i<Original.GetSize(); i++ )
	{
		dValue = Original.GetAt(i);
		Add( dValue );
	}
	return *this;
}

bool doubleArray::operator==( const doubleArray &Original )
{
	int		i;
	double	dValue;

	if( GetSize()!=Original.GetSize() )
		return false;

	if( m_bOrderImportant )
	{
		for( i=0; i<GetSize(); i++ )
		{
			if( GetAt(i)-ROUND_ERROR<Original.GetAt(i) || 
				GetAt(i)+ROUND_ERROR>Original.GetAt(i) )
			{
				return false;
			}
		}
	}
	else
	{
		int			j;
		bool		bFound;
		doubleArray	daCopy;
		daCopy = Original;
		for( i=0; i<GetSize(); i++ )
		{
			bFound = false;
			dValue = GetAt(i);
			for( j=0; j<daCopy.GetSize(); j++ )
			{
				if( dValue-ROUND_ERROR<daCopy.GetAt(j) &&
					dValue+ROUND_ERROR>daCopy.GetAt(j) )
				{
					//Found this one, remove it from the list so it doesn't get Found again
					daCopy.RemoveAt(j);
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				return false;
			}
		}
	}
	return true;
}

bool doubleArray::operator!=( const doubleArray &Original )
{
	return !operator==( Original );
}

intArray::intArray()
{
	SetUniqueElementsOnly(false);
	m_bOrderImportant = false;
}

intArray::intArray(const intArray &Original)
{
	SetUniqueElementsOnly(false);
	*this = Original;
	m_bOrderImportant = false;
}

intArray::~intArray()
{
	RemoveAll();
}

intArray& intArray::operator=( const intArray &Original )
{
	int	i, iValue;

	RemoveAll();
	for( i=0; i<Original.GetSize(); i++ )
	{
		iValue = Original.GetAt(i);
		Add( iValue );
	}
	return *this;
}

bool intArray::operator==( const intArray &Original )
{
	int	iValue;
	int i;

	if( GetSize()!=Original.GetSize() )
		return false;

	if( m_bOrderImportant )
	{
		for( i=0; i<GetSize(); i++ )
		{
			if( GetAt(i)!=Original.GetAt(i) )
			{
				return false;
			}
		}
	}
	else
	{
		int		j;
		bool	bFound;
		intArray	iaCopy;
		iaCopy = Original;
		for( i=0; i<GetSize(); i++ )
		{
			bFound = false;
			iValue = GetAt(i);
			for( j=0; j<iaCopy.GetSize(); j++ )
			{
				if( iValue==iaCopy.GetAt(j) )
				{
					//Found this one, remove it from the list so it doesn't get Found again
					iaCopy.RemoveAt(j);
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				return false;
			}
		}
	}
	return true;
}

bool intArray::operator!=( const intArray &Original )
{
	return !operator==( Original );
}


void doubleArray::SelectionSort(bool bIncremental/*=true*/)
{
	int		i, j;
	double	dValue, dTemp;

	for( i=0; i<GetSize()-1; i++ )
	{
		dValue = GetAt(i);
		for( j=i+1; j<GetSize(); j++ )
		{
			if( ( bIncremental && dValue>GetAt(j) ) ||
				(!bIncremental && dValue<GetAt(j) ) )
			{
				dTemp = GetAt(j);
				SetAt( i, dTemp );
				SetAt( j, dValue );
				//we have changed the value at index i, so get it again!
				dValue = dTemp;
			}
		}
	}
}

double doubleArray::GetTotal()
{
	double dTotal;
	dTotal = 0.00;
	for( int i=0; i<GetSize(); i++ )
	{
		dTotal+= GetAt(i);
	}
	return dTotal;
}

int doubleArray::CountInstances(double dValue)
{
	int		iCount;
	double	dElement;

	iCount=0;
	for( int i=0; i<GetSize(); i++ )
	{
		dElement = GetAt(i);
		if( dElement>dValue-ROUND_ERROR && 
			dElement<dValue+ROUND_ERROR )
		{
			iCount++;
		}
	}
	return iCount;
}

bool doubleArray::IsElementOf( double dValue )
{
	return (CountInstances(dValue)>0);
}

void doubleArray::SetUniqueElementsOnly(bool bUnique)
{
	m_bUnique = bUnique;
}

int doubleArray::Add(double dValue)
{
	if( m_bUnique && IsElementOf(dValue) )
	{
		return -1;
	}
	return CArray<double, double&>::Add( dValue );
}



int intArray::GetTotal()
{
	int iTotal;
	iTotal = 0;
	for( int i=0; i<GetSize(); i++ )
	{
		iTotal+= GetAt(i);
	}
	return iTotal;
}

int intArray::CountInstances(int iValue)
{
	int	iCount, iElement;

	iCount=0;
	for( int i=0; i<GetSize(); i++ )
	{
		iElement = GetAt(i);
		if( iElement==iValue )
			iCount++;
	}
	return iCount;
}

bool intArray::IsElementOf( int iValue )
{
	return (CountInstances(iValue)>0);
}

void intArray::SetUniqueElementsOnly(bool bUnique)
{
	m_bUnique = bUnique;
}

int intArray::Add(int iValue)
{
	if( m_bUnique && IsElementOf(iValue) )
	{
		return -1;
	}
	return CArray<int, int&>::Add( iValue );
}

void intArray::SelectionSort(bool bIncremental/*=true*/)
{
	int		i, j, iValue, iTemp;

	for( i=0; i<GetSize()-1; i++ )
	{
		iValue = GetAt(i);
		for( j=i+1; j<GetSize(); j++ )
		{
			if( ( bIncremental && iValue>GetAt(j) ) ||
				(!bIncremental && iValue<GetAt(j) ) )
			{
				iTemp = GetAt(j);
				SetAt( i, iTemp );
				SetAt( j, iValue );
				//we have changed the value at index i, so get it again!
				iValue = iTemp;
			}
		}
	}
}

void doubleArray::RemoveDoubleups()
{
	int		i, j;
	double	dValue, dTemp;

	for( i=0; i<GetSize()-1; i++ )
	{
		dValue = GetAt(i);
		for( j=i+1; j<GetSize(); j++ )
		{
			dTemp = GetAt(j);
			if( dValue>=dTemp-ROUND_ERROR &&
				dValue<=dTemp-ROUND_ERROR )
			{
				RemoveAt(j);
			}
		}
	}
}

void intArray::RemoveDoubleups()
{
	int		i, j, iValue, iTemp;

	for( i=0; i<GetSize()-1; i++ )
	{
		iValue = GetAt(i);
		for( j=i+1; j<GetSize(); j++ )
		{
			iTemp = GetAt(j);
			if( iValue==iTemp )
			{
				RemoveAt(j);
			}
		}
	}
}

double doubleArray::GetSum()
{
	return GetTotal();
}

void doubleArray::ReverseOrder()
{
	int		iIndex, iSwap;
	double	dValue, dSwap;

	for( iIndex=0; iIndex<int(GetSize()/2); iIndex++ )
	{
		iSwap = GetSize()-1-iIndex;
		dValue	= GetAt(iIndex);
		dSwap	= GetAt(iSwap);
		SetAt( iIndex, dSwap );
		SetAt( iSwap, dValue );
	}
}

bool doubleArray::RemoveElement(double dValue)
{
	bool	bFound;
	double	dElement;

	bFound = true;
	for( int i=0; i<GetSize(); i++ )
	{
		dElement = GetAt(i);
		if( dElement>dValue-ROUND_ERROR && 
			dElement<dValue+ROUND_ERROR )
		{
			RemoveAt(i);
			bFound = true;
		}
	}
	return bFound;
}

bool intArray::RemoveElement(int iValue)
{
	int		iElement;
	bool	bFound;

	bFound = false;
	for( int i=0; i<GetSize(); i++ )
	{
		iElement = GetAt(i);
		if( iElement==iValue )
		{
			RemoveAt(i);
			bFound = true;
		}
	}
	return bFound;
}
