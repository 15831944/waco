// EntityReactor.h: interface for the EntityReactor class.
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

#if !defined(AFX_ENTITYREACTOR_H__F4312293_535E_11D3_9E97_0008C7999B1D__INCLUDED_)
#define AFX_ENTITYREACTOR_H__F4312293_535E_11D3_9E97_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              EntityReactor Class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 25/09/00 4:04p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/ENTITYREACTOR.H $
//	Version			: $Revision: 17 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#include <dbmain.h>

//forward declaration
class Component;
class Controller;
class Bay;
class Matrix;


class EntityReactor : public AcDbObject  
{
public:
	ACRX_DECLARE_MEMBERS(EntityReactor);

	bool IsGroup();
	void SetIsGroup( bool bIsGroup );
	bool IsLevelLine();
	void SetLevelLine( int iLevelLine );

	//////////////////////////////////////////////////////////////////////////
	//Construction/Destruction
	EntityReactor();
	virtual ~EntityReactor();

	//////////////////////////////////////////////////////////////////////////
	//overrides, to trap messages
	virtual void cancelled       (const AcDbObject* dbObj);
	virtual void copied          (const AcDbObject* dbObj, const AcDbObject* newObj);
	virtual void erased          (const AcDbObject* dbObj, Adesk::Boolean pErasing = true);
	virtual void goodbye         (const AcDbObject* dbObj);
	virtual void openedForModify (const AcDbObject* dbObj);
	virtual void modified        (const AcDbObject* dbObj);
	virtual void subObjModified  (const AcDbObject* dbObj, const AcDbObject* subObj);
	virtual void modifyUndone    (const AcDbObject* dbObj);
	virtual void modifiedXData   (const AcDbObject* dbObj);
	virtual void unappended      (const AcDbObject* dbObj);
	virtual void reappended      (const AcDbObject* dbObj);
	virtual void objectClosed    (const AcDbObjectId objId);
	virtual void modifiedGraphics(const AcDbEntity* dbEnt);


	////////////////////////////////////////////////////////
	//We have a close so it is only logical we have an open
	Acad::ErrorStatus open( bool bForceOpen=false );


	////////////////////////////////////////////////////////
	//Pointers to other classes
	Component	*GetComponentPointer() const;
	Controller	*GetControllerPointer() const;
	Bay			*GetBayPointer( ) const;
	Matrix		*GetMatrixPointer() const;
	void SetComponentPointer( Component *pComponent );
	void SetControllerPointer( Controller *pController );
	void SetBayPointer( Bay *pBay );
	void SetMatrixPointer( Matrix *pMatrix );


protected:
	int m_iKwikscafReactorCheck;
	bool		m_bIsGroup;
	int			m_iLevelLine;
	Matrix		*m_pMatrix;
	Bay			*m_pBay;
	Controller	*m_pController;
	Component	*m_pComponent;
private:
	Component * GetComponent(AcDbEntity *pEnt);
	bool IsKwikscaffReactor(LPCTSTR strReactorName) const;
};

#endif // !defined(AFX_ENTITYREACTOR_H__F4312293_535E_11D3_9E97_0008C7999B1D__INCLUDED_)




/*******************************************************************************
*	History Records
********************************************************************************
* $History: EntityReactor.h $
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 9/09/99    Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Matrix now has crosshair which operates correctly
 * 2) Still only have one (1) MatrixElement
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 8/19/99    Time: 10:37a
 * Updated in $/Meccano/Stage 1/Code
*
*******************************************************************************/
