// ComponentDetailsElement.h: interface for the ComponentDetailsElement class.
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

#if !defined(AFX_COMPONENTDETAILSELEMENT_H__6772921B_5427_11D3_9E98_0008C7999B1D__INCLUDED_)
#define AFX_COMPONENTDETAILSELEMENT_H__6772921B_5427_11D3_9E98_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              ComponentDetails Class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 31/10/00 4:42p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/COMPONENTDETAILS.H $
//	Version			: $Revision: 28 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#include "MeccanoDefinitions.h"

const	double	CDT_INVALID		= -1.00;
const	double	CDT_NOT_FOUND	= -2.00;
const	double	CDT_UNDEFINED	= -3.00;

//forward declarations
class Entity;
class ComponentDetailsArray;
class StockListElement;
class Controller;

class ComponentDetailsElement  
{
public:
	StockListElement *GetStockDetails( SystemEnum eSystem );

	/////////////////////////////////////////////////////////
	//Costruction/Destruction
	ComponentDetailsElement( ComponentDetailsArray *pArrayPointer );
	virtual ~ComponentDetailsElement();

	ComponentTypeEnum	m_eCompType;
	double				m_dCommonLength;
	MaterialTypeEnum	m_eMaterialType;

	//Part Number
	CString				m_sMillsPartNumber;
	CString				m_sKwikstagePartNumber;
	CString				m_sOtherPartNumber;

	//pointer to stock list element
	StockListElement	*m_pStockDetailsMills;
	StockListElement	*m_pStockDetailsKwikScaf;
	StockListElement	*m_pStockDetailsOther;
};

#include <afxtempl.h>
#include "doubleArray.h"

class ComponentDetailsArray: public CArray<ComponentDetailsElement*, ComponentDetailsElement*>
{
public:
	StockListElement * GetStockDetails( ComponentTypeEnum eType, double dCommonLength, MaterialTypeEnum eMaterial, SystemEnum eSystem );

	double GetBayLengthActual(	double dCommonLength, SystemEnum eSystem );
	double GetBayWidthActual(	double dCommonLength, SystemEnum eSystem );
	double GetBayLengthCommon(	double dActualLength, SystemEnum eSystem );
	double GetBayWidthCommon(	double dActualLength, SystemEnum eSystem );
	// Changed LPCTSTR to CString, ~BSK, 21.02.2018
	void	SetFilenameInRegistry(CString strFileName);
	double GetAvailableLength(ComponentTypeEnum eType, MaterialTypeEnum eMaterail, double dLength, RoundingTypeEnum eRound, bool bNotFoundReturnNearest=false );
	doubleArray GetCommonLengthsForComponent( ComponentTypeEnum eCompType, MaterialTypeEnum eMaterial );

	void	SetEntPathInRegistry(LPCTSTR strFileName);
	LPCTSTR	GetEntPathInRegistry();
	
	ComponentDetailsArray();
	virtual ~ComponentDetailsArray();
	void	RemoveAll();

	SystemEnum	GetSystem() const;

	bool	Load(Controller *pController);

	double	GetCommonLength(	SystemEnum eSystem, ComponentTypeEnum eCompType, double dActualLength, MaterialTypeEnum eMaterial, bool bShowWarning=true );
	double	GetActualLength(	SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning=true );
	double	GetActualWidth(		SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning=true );
	double	GetActualHeight(	SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning=true );
	double	GetActualWeight(	SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning=true );
	LPCTSTR	GetComponentPartNumberStr(	SystemEnum eSystem, ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial  );
	LPCTSTR	GetComponentDescriptionStr(	SystemEnum eSystem, ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial  );

	ComponentDetailsElement *GetMatchingComponent( CString sPartNumber );
	ComponentDetailsElement *GetMatchingComponent(ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning );

protected:

private:
	bool	GetFile( CStdioFile &File );
	CString	GetFilenameInRegistry();
	double	GetNextDoubleAndMoveOn( CString &sLine );

/*
	void switchToModelSpace();
	void switchToPaperSpace();
*/
};


#endif // !defined(AFX_COMPONENTDETAILSELEMENT_H__6772921B_5427_11D3_9E98_0008C7999B1D__INCLUDED_)



/*******************************************************************************
*	History Records
********************************************************************************
* $History: ComponentDetails.h $
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 5/10/00    Time: 8:14a
 * Updated in $/Meccano/Stage 1/Code
 * just finished building version 1.5.1.3 (R1.1c)
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 21/08/00   Time: 3:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 6/10/99    Time: 11:50a
 * Updated in $/Meccano/Stage 1/Code
 * Have updated Reader for ComponentDetails.csv since addition of Mills
 * and  Kwikscaff Part numbers
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 5/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * Added the PartNumber to the ComponentDetails
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 23/09/99   Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * All 3D Component Entity drawing now use the same path, and it will
 * prompt them for that path if it can't find it
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:08a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
*
*******************************************************************************/
