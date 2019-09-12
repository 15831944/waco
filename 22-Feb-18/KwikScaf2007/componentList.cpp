// componentList.cpp: implementation of the ComponentList class.
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
#include "componentList.h"
#include "Component.h"

#include "Bay.h"
#include "Controller.h"

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//					COMPONENT LIST ELEMENT / CLASS MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	Constructor
//
//	default constructor
//
ComponentListElement::ComponentListElement() 
{
	m_ePosition			= SIDE_INVALID;
	m_iComponentRise	= LIFT_RISE_INVALID;
	m_pComponent		= NULL;
}


/////////////////////////////////////////////////////////////////////////////////
// Constructor 2
//
//
ComponentListElement::ComponentListElement( Component *pComponent ) 
{
	m_pComponent = pComponent;
}


/////////////////////////////////////////////////////////////////////////////////
// Constructor 3
//
//
ComponentListElement::ComponentListElement( SideOfBayEnum ePosition, int iComponentRise, Component *pComponent )
{
	m_ePosition = ePosition;
	m_iComponentRise = iComponentRise;
	m_pComponent = pComponent;
}


/////////////////////////////////////////////////////////////////////////////////
//	Destructor
//
//
ComponentListElement::~ComponentListElement()
{
	if (m_pComponent)
		delete m_pComponent;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//					COMPONENT LIST CLASS MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	Constructor
//
ComponentList::ComponentList()
{
	DeleteAll();
}


/////////////////////////////////////////////////////////////////////////////////
//	Destructor
//
//
ComponentList::~ComponentList()
{
	DeleteAll();
}

/////////////////////////////////////////////////////////////////////////////////
//	List addition functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	AddComponent
//
//
int ComponentList::AddComponent( SideOfBayEnum ePosition, int iComponentRise, Component *pComponent )
{
	int iID;
	iID = Add(new ComponentListElement( ePosition, iComponentRise, pComponent));
	pComponent->SetID( iID );

	CString sText;
	sText.Format( _T("Component%i-%s"), pComponent->GetID(),
				GetComponentDescStr(pComponent->GetType()) );
	if( !gED.AddEntityInfo( pComponent, sText ) )
	{
		//dump the content
		CString sTemp;
    sTemp.Format( _T("\nDump of component list(%i):"), GetSize() );
		sText+=sTemp;
		for( int i=0; i<GetSize(); i++ )
		{
			sTemp.Format( _T("%lX,"), GetAt(i)->m_pComponent );
			sText+= sTemp;
		}
		acutPrintf( sText );
	}
	return iID;
}


/////////////////////////////////////////////////////////////////////////////////
//	List deletion functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	DeleteAll
//
//	CArray wrapper function.
//  Deletes all the elements in the array.
//
void ComponentList::DeleteAll()
{
	ComponentListElement *pComponentListElement;

	while(GetSize()>0)
	{
		pComponentListElement = NULL;
		pComponentListElement = GetAt(0);
		;//assert( pComponentListElement!=NULL );

		gED.DeleteEntityInfo( pComponentListElement->m_pComponent );
		delete pComponentListElement;

		RemoveAt(0);
	}
}


/////////////////////////////////////////////////////////////////////////////////
//	RemoveAt
//
//	CArray wrapper function.
//	Removes one or more elements starting at a specified index in an array. 
//	In the process, it shifts down all the elements above the removed element(s). 
//	It decrements the upper bound of the array but does not free memory. 
//
void ComponentList::RemoveAt(int nIndex, int nCount)
{
	;//assert( nIndex>=0 );
	;//assert( nCount>0 );
	;//assert( nIndex+nCount<=GetSize() );
	CArray<ComponentListElement*, ComponentListElement*>::RemoveAt(nIndex, nCount);
}


////////////////////////////////////////////////////////////////////////////////
//	DeleteComponent
//
//
bool ComponentList::DeleteComponent(int iCompId)
{
	int						iID;
	bool					bComponentRemoved;
	Component				*pComponent;
	ComponentListElement	*pEle;

	bComponentRemoved = false;
	pEle=NULL;
	if( iCompId>=0 && iCompId < GetSize() )
	{
		pEle=NULL;
		pEle = GetAt(iCompId);

		if( pEle!=NULL )
		{
			gED.DeleteEntityInfo( pEle->m_pComponent );
			delete pEle;
			pEle = NULL;
		}
		else
		{
			;//assert( false );
		}
		RemoveAt(iCompId);

		//renumber components
		iID = iCompId;
		while( iID<GetSize() )
		{
			pComponent	= GetComponent(iID);
			pComponent->SetID(iID);
			iID++;
		}
		bComponentRemoved = true;
	}

	return bComponentRemoved;
}


/////////////////////////////////////////////////////////////////////////////////
// List member attribute access functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	GetComponent
//
//
Component * ComponentList::GetComponent(int iComponentID) const
{
	;//assert( iComponentID>=0 && iComponentID<GetSize() );
	return GetAt(iComponentID)->m_pComponent;
}


/////////////////////////////////////////////////////////////////////////////////
//	GetRise
//
//
int ComponentList::GetRise(int iComponentID) const
{
	return GetAt(iComponentID)->m_iComponentRise;
}


/////////////////////////////////////////////////////////////////////////////////
//	GetPosition
//
//
SideOfBayEnum ComponentList::GetPosition(int iCompId) const
{
	ComponentListElement *pEle;
	;//assert( iCompId>=0 );
	;//assert( iCompId<GetSize() );
	pEle = NULL;
	pEle = GetAt(iCompId);
	if( (long)pEle<(long)0x00010000 )
	{
		//dump the content
		CString sTemp, sText;
		sText.Empty();
    sTemp.Format( _T("\nDump of component list(%i):"), GetSize() );
		sText+=sTemp;
		for( int i=0; i<GetSize(); i++ )
		{
			sTemp.Format( _T("%lX,"), (long)GetAt(i) );
			sText+= sTemp;
		}
		acutPrintf( sText );
	}
	return pEle->m_ePosition;
}


////////////////////////////////////////////////////////////////////////////////
//	GetType
//
//
ComponentTypeEnum ComponentList::GetType(int iComponentID) const
{
	return GetAt(iComponentID)->m_pComponent->GetType();
}


/////////////////////////////////////////////////////////////////////////////////
// List query functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	GetSize
//
//
int ComponentList::GetSize() const
{
	return CArray<ComponentListElement*, ComponentListElement*>::GetSize();
}


/////////////////////////////////////////////////////////////////////////////////
//	GetComponent
//
//
Component *ComponentList::GetComponent( SideOfBayEnum ePosition, int iRise, ComponentTypeEnum eCompType) const
{
	bool		bIgnorRise;

	bIgnorRise = false;
	switch( eCompType )
	{
	////////////////////////////////////////////////////
	//Bay components
	case( CT_BRACING ):		//fallthrough
		;//assert( iRise>=0 );
		break;
	case( CT_JACK ):		//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_SOLEBOARD ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_CHAIN_LINK ):	//fallthrough
		bIgnorRise = true;
		;//assert( iRise==0 );
		break;
	case( CT_SHADE_CLOTH ):
		bIgnorRise = true;
		;//assert( iRise==0 );
		break;
	case( CT_TIE_TUBE ):			//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_TIE_CLAMP_YOKE ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_TIE_CLAMP_90DEGREE ):
		;//assert( iRise==0 );
		break;
	case( CT_STANDARD ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_STANDARD_OPENEND ):	//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_STAIR ):		//fallthrough
		;//assert( iRise==0 );
		break;
	case( CT_LADDER ):
		;//assert( iRise==0 );
		break;

	////////////////////////////////////////////////////
	//Lift components
	case( CT_MESH_GUARD ):
		;//assert( iRise==LIFT_RISE_1000MM || iRise==LIFT_RISE_2000MM );
		break;
	case( CT_RAIL ):
		;//assert( iRise==LIFT_RISE_1000MM );
		break;
	case( CT_MID_RAIL ):
		;//assert( iRise==LIFT_RISE_0500MM );
		break;
	case( CT_TOE_BOARD ):
	case( CT_HOPUP_BRACKET ):
	case( CT_CORNER_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_LAPBOARD ):
		;//assert( iRise==LIFT_RISE_0000MM );
		break;
	case( CT_DECKING_PLANK ):
		;//assert( iRise>=PLANK01 && iRise<=PLANK10 );
		break;
	case( CT_STAGE_BOARD ):
		;//assert( iRise>=PLANK01 && iRise<=PLANK03 );
		break;
	default:
		;//assert( false );	//not part of the lift
	}

	int iSize, iID;
	Component	*pComponent;
	pComponent = NULL;

	iSize = GetNumberOfComponents();
	for( iID = 0; iID<iSize; iID++ )
	{
		if ((GetPosition(iID) == ePosition) &&
			(GetType(iID) == eCompType) )
		{	
			if( bIgnorRise )
			{
				pComponent = GetComponent(iID);
				break;
			}
			else
			{
				if( GetRise(iID)==iRise )
				{
					pComponent = GetComponent(iID);
					break;
				}
			}
		}
	}
	
	return pComponent;
}


/////////////////////////////////////////////////////////////////////////////////
//	GetNumberOfPosTypes
//
// 
int ComponentList::GetNumberOfPosTypes( SideOfBayEnum ePosition, ComponentTypeEnum eCompType) const
{
	int count;

	count = 0;

	for( int iComponentID = 0; iComponentID < GetNumberOfComponents();iComponentID++ )
	{
		if ((GetPosition(iComponentID) == ePosition) &&
			(GetType(iComponentID) == eCompType))
				++count;
	}
	
	return count;
}


/////////////////////////////////////////////////////////////////////////////////
// Set list member attribute functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	SetRise
//
//
void ComponentList::SetRise( int iComponentID, int iComponentRise )
{
	if (iComponentID < GetNumberOfComponents())
		GetAt(iComponentID)->m_iComponentRise = iComponentRise;
}


/////////////////////////////////////////////////////////////////////////////////
//	SetPosition
//
//
void ComponentList::SetPosition( int iComponentID, SideOfBayEnum eNewPosition)
{
	if (iComponentID < GetNumberOfComponents())
		GetAt(iComponentID)->m_ePosition = eNewPosition;
}


/*****************************************************************
 *	History Records
 *****************************************************************
 * $History: componentList.cpp $ 
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:41a
 * Updated in $/Meccano/Stage 1/Code
 * have removed all the _T('enum')s from the component details list
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * fixing up the chain link delete problem
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Dar          Date: 9/22/99    Time: 10:40a
 * Updated in $/Meccano/Stage 1/Code
 * Bom extra alpha save load works
 * 
 * *****************  Version 25  *****************
 * User: Dar          Date: 9/21/99    Time: 11:36a
 * Updated in $/Meccano/Stage 1/Code
 * added array clean up in the desctructor as it wasn't clearing the array
 * objects
 * 
 * *****************  Version 24  *****************
 * User: Dar          Date: 9/08/99    Time: 11:45a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 9/01/99    Time: 9:22a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on the cleanup operation
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Dar          Date: 8/19/99    Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/12/99    Time: 2:50p
 * Updated in $/Meccano/Stage 1/Code
 * Updating for andrews build
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 2/08/99    Time: 3:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 29/07/99   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
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
 * *****************  Version 3  *****************
 * User: Dar          Date: 20/07/99   Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * compilible group of classes componentList, componentListElement, Lift,
 * Component
 * 
 *****************************************************************/


/////////////////////////////////////////////////////////////////////////////////
//operator =
//
ComponentList & ComponentList::operator =(const ComponentList &Original)
{
	int					i;
	double				dLength, dWeight;
	ComponentTypeEnum	eComponentType;
	MaterialTypeEnum	eMaterialType;
	Component			*pComponent, *pComp;

	DeleteAll();
	for( i=0; i<Original.GetNumberOfComponents(); i++ )
	{
		pComp = Original.GetComponent( i );
		pComp->GetComponentDetails( dLength, dWeight, eComponentType, eMaterialType );
		pComponent = new Component( dLength, eComponentType, eMaterialType, pComp->GetSystem() );
		*pComponent = *pComp;
		AddComponent( Original.GetPosition(i), Original.GetRise(i), pComponent );
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////
//GetNumberOfComponents
//
int ComponentList::GetNumberOfComponents() const 
{
	return GetSize();
}

/////////////////////////////////////////////////////////////////////////////////
//GetComponentID
//
int ComponentList::GetComponentID(Component *pComponent)
{
	int iReturnedComponentID;
	iReturnedComponentID = -1;					

	for( int iComponentID = 0; iComponentID < GetNumberOfComponents(); iComponentID++ )
	{
		if (pComponent == GetComponent(iComponentID))
			iReturnedComponentID = iComponentID;
	}
	
	return iReturnedComponentID;
}
