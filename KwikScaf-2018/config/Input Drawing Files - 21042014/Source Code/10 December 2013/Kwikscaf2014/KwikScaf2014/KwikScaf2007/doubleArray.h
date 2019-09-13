// doubleArray.h: interface for the doubleArray class.
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

#if !defined(AFX_DOUBLEARRAY_H__D7C17562_A7DC_11D3_B807_00804890115F__INCLUDED_)
#define AFX_DOUBLEARRAY_H__D7C17562_A7DC_11D3_B807_00804890115F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class doubleArray : public CArray<double, double&>  
{
public:
	bool RemoveElement( double dValue );
	doubleArray();
	doubleArray( const doubleArray &Original );
	virtual ~doubleArray();

	void ReverseOrder();
	double GetTotal();
	double GetSum();
	void RemoveDoubleups();
	void SetUniqueElementsOnly( bool bUnique=true );

	int Add( double dValue );
	bool IsElementOf(double dValue);
	int CountInstances( double dValue );
	void SelectionSort( bool bIncremental=true );
	doubleArray& operator=( const doubleArray &Original );
	bool operator==(const doubleArray &Original);
	bool operator!=(const doubleArray &Original);
protected:
	bool m_bUnique;
	bool m_bOrderImportant;
};


class intArray : public CArray<int, int&>  
{
public:
	bool RemoveElement( int iValue );
	void RemoveDoubleups();
	void	SelectionSort(bool bIncremental=true);
	int		GetTotal();
	int		CountInstances(int iValue);
	bool	IsElementOf( int iValue );
	void	SetUniqueElementsOnly(bool bUnique);
	int		Add(int iValue);
	intArray( const intArray &Original );
	intArray();
	virtual ~intArray();

	intArray& operator=( const intArray &Original );
	bool operator==( const intArray &Original );
	bool operator!=( const intArray &Original );
protected:
	bool m_bUnique;
	bool m_bOrderImportant;
};


#endif // !defined(AFX_DOUBLEARRAY_H__D7C17562_A7DC_11D3_B807_00804890115F__INCLUDED_)
