// LapboardBay.h: interface for the LapboardBay class.
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

#if !defined(AFX_LAPBOARDBAY_H__500BA2D3_7528_11D3_9ECE_0008C7999B1D__INCLUDED_)
#define AFX_LAPBOARDBAY_H__500BA2D3_7528_11D3_9ECE_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Bay.h"
#include "PreviewTemplate.h"	// Added by ClassView

class LapboardBay : public Bay  
{
public:
	bool IsBaySelected();
	void SetSchematicTransform(Matrix3D Transform);
	
	Matrix3D GetSchematicTransform();
	void UpdateSchematicView();
	void PositionLapboardStandards();
	void SetBayWidth( double dWidth );
	void SetBayLength( double dLength );
	int CreateLapboardLift( double dRL );
	int AddLift( Lift *pLift, double dRL );
	void SetBayPointers( Bay *pBayWest, Bay *pBayEast, SideOfBayEnum eSideWest, SideOfBayEnum eSideEast, bool bShowWarning=true );
	void CreateSchematic();
	void MoveSchematic( Matrix3D Transform, bool bStore );
	virtual void CreateSchematicPoints();
	LapBoardTemplate * GetTemplate();

	LapboardBay();
	virtual ~LapboardBay();

	//Pointers
	Run			*GetRunPointer();
	Controller	*GetContoller();
	
	Bay			*GetBackward() const;
	Bay			*GetForward() const;
	Bay			*GetOuter() const;
	Bay			*GetInner() const;
	LapboardBay	*GetCornerEast() const;
	LapboardBay	*GetCornerWest() const;
	Bay			*GetBayEast() const;
	Bay			*GetBayWest() const;

	void SetBackward( Bay *pBay );
	void SetForward( Bay *pBay );
	void SetOuter( Bay *pBay );
	void SetInner( Bay *pBay );
	void SetCornerEast( LapboardBay *pLapBay );
	void SetCornerWest( LapboardBay *pLapBay );
	void SetBayWest( Bay *pBay );
	void SetBayEast( Bay *pBay );

	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	///////////////////////////////////////////////////////////////////////////////
	void Serialize(CArchive &ar);

protected:
	Bay					*m_BayWest;
	Bay					*m_BayEast;
	Controller			*m_pController;
	LapBoardTemplate	m_LBTplt;
};

#include <afxtempl.h>
class LapboardList : public CArray<LapboardBay*,LapboardBay*>
{
public:

	LapboardList();
	virtual ~LapboardList();

	//New
	LapboardBay * CreateNewLapboard();

	//retrieves the number of lapboards owned by the controller
	int GetNumberOfLapboards() const;

	//controls the initial user interface such that the appropriate function below
	//	is called.
	int	AddNewLapboard( LapboardBay *pLapboard );

	//Retrieve a lapboard pointer
	LapboardBay *GetLapboard( int iLapboardID );

	//Deleting
	void DeleteAllLapboards();
	bool DeleteLapboard( int iLapboardID, int iCount=1 );

	//ID's
	void ReIDAllLapboards( int iStartID=0 );


protected:

};

#endif // !defined(AFX_LAPBOARDBAY_H__500BA2D3_7528_11D3_9ECE_0008C7999B1D__INCLUDED_)
