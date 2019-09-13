// componentList.h: interface for the ComponentList class.
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

#if !defined(AFX_COMPONENTLIST_H__4DF36A76_3E47_11D3_8A5C_00508B043A6C__INCLUDED_)
#define AFX_COMPONENTLIST_H__4DF36A76_3E47_11D3_8A5C_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
////////////////////// INCLUDES ///////////////////////////////
#include "MeccanoDefinitions.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//			COMPONENT LIST ELEMENT CLASS / COMPONENT LIST CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 19/05/00 12:03p $
//	Version			: $Revision: 19 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////

class Component;

///////////////////////////////////////////////////////////////////////////////
//					COMPONENT LIST ELEMENT CLASS 
class ComponentListElement  
{
public:
	////////////////////
	////////////////////
	////Functionality///
	////////////////////
	////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	//construction/destruction
	/////////////////////////////////////////////////////////////////////////////////

	ComponentListElement();
	ComponentListElement( Component *pComponent );
	ComponentListElement( SideOfBayEnum ePosition, int iComponentRise, Component *pComponent );
	virtual ~ComponentListElement();

	////////////////////
	////////////////////
	/////Attributes/////
	////////////////////
	////////////////////
	SideOfBayEnum	m_ePosition;
	int				m_iComponentRise;
	Component		*m_pComponent;
};


///////////////////////////////////////////////////////////////////////////////
//					COMPONENT LIST CLASS
class ComponentList : protected CArray<ComponentListElement*, ComponentListElement*>
{
public:
	int GetComponentID(Component *pComponent);
	int GetNumberOfComponents() const;
	ComponentList &operator =(const ComponentList &Original);
	////////////////////
	////////////////////
	////Functionality///
	////////////////////
	////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	//construction/destruction
	/////////////////////////////////////////////////////////////////////////////////
	ComponentList();
	virtual ~ComponentList();

	/////////////////////////////////////////////////////////////////////////////////
	//list addition functions
	/////////////////////////////////////////////////////////////////////////////////
	int	AddComponent(  SideOfBayEnum ePosition, int iComponentRise, Component *pComponent );

	/////////////////////////////////////////////////////////////////////////////////
	//list deletion functions
	/////////////////////////////////////////////////////////////////////////////////
	void DeleteAll();
	bool DeleteComponent(int iComponentID);

	/////////////////////////////////////////////////////////////////////////////////
	//list member attribute access functions
	/////////////////////////////////////////////////////////////////////////////////
	Component				*GetComponent( int iComponentID ) const;
	int						GetRise( int iComponentID ) const;
	SideOfBayEnum			GetPosition( int iComponentID ) const;
	ComponentTypeEnum		GetType( int iComponentID ) const;

	/////////////////////////////////////////////////////////////////////////////////
	//list query functions
	/////////////////////////////////////////////////////////////////////////////////
	int			GetSize() const;
	Component	*GetComponent( SideOfBayEnum ePosition, int iRise, ComponentTypeEnum eCompType ) const;
	int			GetNumberOfPosTypes( SideOfBayEnum ePosition, ComponentTypeEnum eCompType) const;

	/////////////////////////////////////////////////////////////////////////////////
	//set list member attribute functions
	/////////////////////////////////////////////////////////////////////////////////
	void SetPosition(int iComponentID, SideOfBayEnum eNewPosition);
	void SetRise( int iComponentID, int iComponentRise );

private:
	void RemoveAt(int nIndex, int nCount =1);

};

#endif // !defined(AFX_COMPONENTLIST_H__4DF36A76_3E47_11D3_8A5C_00508B043A6C__INCLUDED_)

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: componentList.h $
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:41a
 * Updated in $/Meccano/Stage 1/Code
 * have removed all the 'enum's from the component details list
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * fixing up the chain link delete problem
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Dar          Date: 8/19/99    Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 8  *****************
 * User: Dar          Date: 28/07/99   Time: 12:40p
 * Updated in $/Meccano/Stage 1/Code
 * Converted _T("Get()") functions to _T("Get() const") functions
 * 
 * *****************  Version 7  *****************
 * User: Dar          Date: 27/07/99   Time: 11:28a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Dar          Date: 27/07/99   Time: 9:43a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Dar          Date: 21/07/99   Time: 5:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Dar          Date: 21/07/99   Time: 8:51a
 * Updated in $/Meccano/Stage 1/Code
 * 
 *
 *******************************************************************************/
