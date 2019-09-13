// BayStageLevel.h: interface for the BayStageLevel class.
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

#if !defined(AFX_BAYSTAGELEVEL_H__3A381C01_A4E8_11D3_B807_00804890115F__INCLUDED_)
#define AFX_BAYSTAGELEVEL_H__3A381C01_A4E8_11D3_B807_00804890115F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class Controller;
class Bay;


class BayStageLevel  
{
public:
	void Serialize( CArchive &ar );
	void SetVisible( bool bVisible );
	bool IsVisible();
	void SetCommitted( bool bCommitted=true );
	bool IsCommitted();
	void SetStage( LPCTSTR strStage );
	CString GetStage( );
	void SetLevel( int iLevel );
	int GetLevel();
	void SetBay( Bay *pBay );
	Bay * GetBay();
	BayStageLevel();
	virtual ~BayStageLevel();

protected:
	bool m_bVisible;
	Bay		*m_pBay;
	int		m_iLevel;
	CString m_sStage;
	bool	m_bCommitted;
};

#include <afxtempl.h>

class BayStageLevelArray : public CArray<BayStageLevel*, BayStageLevel*>
{
public:
	bool IsStageLevelVisible( int iLevel, CString sStage );
	void ClearVisible();
	void SetVisible( int iBN, int iLvl, LPCTSTR strStage, bool bVisible/*=true*/ );
	bool IsVisible(int iBayNumber, int iLevel, LPCTSTR strStage );
	int GetColour( CString sStage, int iLevel );
	void BayDeleted( Bay *pBay );
	void ClearCommitted();
	bool IsCommitted( int iBayNumber, int iLevel, LPCTSTR strStage );
	void SetController( Controller *pController );
	void SetAtGrowStage( int iBayNumber, int iLevel, LPCTSTR strStage );
	CString GetStage( int iBayNumber, int iLevel );
	void Serialize(CArchive &ar);
	void SetCommitted( int iBayNumber, int iLevel, LPCTSTR strStage, bool bCommitted=true );
	BayStageLevelArray();
	~BayStageLevelArray();
protected:
	Controller *m_pController;
};


#endif // !defined(AFX_BAYSTAGELEVEL_H__3A381C01_A4E8_11D3_B807_00804890115F__INCLUDED_)
