// MillsType.h: interface for the MillsType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MILLSTYPE_H__61B59581_2BCC_11D4_B80A_00804890115F__INCLUDED_)
#define AFX_MILLSTYPE_H__61B59581_2BCC_11D4_B80A_00804890115F__INCLUDED_

#include "doubleArray.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class MillsType  
{
public:
	MillsType();
	virtual ~MillsType();

	bool IsMillsTypeSet();

	int		GetType();
	int		AddType( int iType );
	void	SetType( int iType );
	void	ClearType();

	bool	HasGap( SideOfBayEnum eSide );
	bool	HasHalfSize( SideOfBayEnum eSide );
	bool	SetGap( SideOfBayEnum eSide );
	intArray GetHalfs();
	intArray GetGaps();


	bool	HasStandard( SideOfBayEnum eSide );
	void	RemoveStandard( SideOfBayEnum eSide );
	intArray GetMissingStandards();
	intArray GetExistingStandards();

protected:
	int m_iType;
};

#endif // !defined(AFX_MILLSTYPE_H__61B59581_2BCC_11D4_B80A_00804890115F__INCLUDED_)
