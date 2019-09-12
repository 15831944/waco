// LiftList.cpp: implementation of the LiftList class.
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
#include "LiftList.h"
#include "LiftDetailsDialog.h"
#include <dbdict.h>
#include "Component.h"
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//					LIFT LIST ELEMENT CLASS MEMBER FUNCTIONS
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
LiftListElement::LiftListElement()
{
	m_pLift = NULL;
	m_dRL	= 0;
}


/////////////////////////////////////////////////////////////////////////////////
//	Constructor 2
//
//
LiftListElement::LiftListElement(Lift *pLift, double dRL)
{
	m_pLift = pLift;
	m_dRL	= dRL;
}


/////////////////////////////////////////////////////////////////////////////////
//	Destructor
//
//
LiftListElement::~LiftListElement()
{
	if (m_pLift)
		delete m_pLift;
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//					LIFT LIST CLASS MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	Constructor
//
//
LiftList::LiftList()
{
	m_pLiftDetailsDialog = NULL;
}


/////////////////////////////////////////////////////////////////////////////////
//	Destructor
//
//
LiftList::~LiftList()
{
	DELETE_PTR(m_pLiftDetailsDialog);
}



/////////////////////////////////////////////////////////////////////////////////
//	List addition functions																																																																																																																																																																																																																																																																																																																																																																																																																											
/////////////////////////////////////////////////////////////////////////////////

#include "Bay.h"
#include "Controller.h"

/////////////////////////////////////////////////////////////////////////////////
//	AddLift
//
//	Adds an element into the array.
//
int LiftList::AddLift(Lift *pLift, double dRL)
{
	int iID;
	iID = Add(new LiftListElement(pLift, dRL));
	pLift->SetLiftID( iID );
	return iID;
}


/////////////////////////////////////////////////////////////////////////////////
//	InsertLift
//
//	Inserts an element into the array.
//
bool LiftList::InsertLift(Lift *pLift, double dRL, int iLiftID)
{
	InsertAt(iLiftID, new LiftListElement(pLift, dRL));
	while( iLiftID<GetSize() )
	{
		pLift = GetLift( iLiftID );
		pLift->SetLiftID( iLiftID );
		iLiftID++;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//	GetRL
//  Returns the RL of the lift referred to by the LiftID
double LiftList::GetRL(int iLiftID) const
{
	if( iLiftID<0 || iLiftID>=GetSize() )
		return -1.00;
	return GetAt(iLiftID)->m_dRL;
}

/////////////////////////////////////////////////////////////////////////////////
//SetRL
//Sets the RL of the lift referred to by the LiftID
void LiftList::SetRL( int iLiftID, double dRL )
{
	if( GetSize()<=0 )
	{
		return;
	}

#ifdef _DEBUG
	int		iTemp;
	double	dTemp, dRemainder;
	;//assert( GetLift(0)!=NULL && GetLift(0)->GetBayPointer()!=NULL );
	dTemp = dRL/GetLift(0)->GetBayPointer()->GetStarSeparation();
	iTemp = (int)(dTemp+(dTemp>0.00? 0.50: -0.50));
	dRemainder = dTemp-(double)iTemp;
	;//assert( dRemainder<ROUND_ERROR_SMALL && dRemainder>(-1.00*ROUND_ERROR_SMALL) );
#endif	//#ifdef _DEBUG
	GetAt(iLiftID)->m_dRL = dRL;
}

/////////////////////////////////////////////////////////////////////////////////
//	GetLift
//
//  Returns a pointer to the lift referred to by the LiftID
//
Lift *LiftList::GetLift(int iLiftID) const
{
	if( iLiftID<0 || iLiftID>=GetSize() )
		return NULL;
	return GetAt(iLiftID)->m_pLift;
}


/////////////////////////////////////////////////////////////////////////////////
//	GetID
//
//  Retrives the liftID of a Lift based on the pointer to that lift in memory.
//  If no matching pointers are found the function returns the DEFINE value
//	ID_NONE_MATCHING.
//
int LiftList::GetID(Lift *pLift) const
{
	int iRetLiftID;

	iRetLiftID = ID_NONE_MATCHING;

	for( int iLiftID = 0; iLiftID < GetSize();iLiftID++ )
	{
		if (pLift == GetAt(iLiftID)->m_pLift)
			iRetLiftID = iLiftID;
	}

	return iRetLiftID;
}


/////////////////////////////////////////////////////////////////////////////////
//	GetSize
//
//	CArray wrapper function.
//	Returns number of elements in the array - 1 (zero based).
//
int LiftList::GetSize() const
{
	return CArray<LiftListElement*, LiftListElement*>::GetSize();
}


/////////////////////////////////////////////////////////////////////////////////
//	DeleteAll
//
//	CArray wrapper function.
//  Deletes all the elements in the array.
//
void LiftList::DeleteAll()
{
	LiftListElement *pLiftListElement;

	while( GetSize()>0 )
	{
		pLiftListElement=NULL;
		pLiftListElement = GetAt(0);
		if( pLiftListElement!=NULL )
		{
			delete pLiftListElement;
		}
		else
		{
			;//assert( false );
		}

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
void LiftList::RemoveAt(int nIndex, int nCount)
{
	CArray<LiftListElement*, LiftListElement*>::RemoveAt(nIndex, nCount);
}


/// **** start dictionary test code ********
/// ****************************************
/// ****************************************
/// ****************************************
// Opens the dictionary associated with the key REACTOR_DICTIONARY_NAME
// and iterates through all its entries, printing out the
// integer data value in each entry.
//
void iterateDictionary()
{
    AcDbDictionary		*pNamedobj;
	Acad::ErrorStatus	es;

    es = acdbHostApplicationServices()->workingDatabase()
			->getNamedObjectsDictionary(pNamedobj, AcDb::kForRead);
	;//assert( es==Acad::eOk );

	//Get a pointer to the REACTOR_DICTIONARY_NAME dictionary.
	AcDbDictionary *pDict;
	es = pNamedobj->getAt( REACTOR_DICTIONARY_NAME, (AcDbObject*&)pDict, AcDb::kForRead);
	;//assert( es==Acad::eOk );

	es = pNamedobj->close();
	;//assert( es==Acad::eOk );

    // Get an iterator for the REACTOR_DICTIONARY_NAME dictionary.
    AcDbDictionaryIterator *pDictIter = pDict->newIterator();
    Component *pComponent;

//    Adesk::Int16 val;
	double val;

    for (; !pDictIter->done(); pDictIter->next())
	{

        // Get the current record, open it for read, and
        // print its data.
        //
        pDictIter->getObject((AcDbObject*&)pComponent,
            AcDb::kForRead);
        val = pComponent->GetLengthCommon();
        es = pComponent->close();
		;//assert( es==Acad::eOk );

    acutPrintf(_T("\nLength is:  %.2f"), val);
    }
    delete pDictIter;

    es = pDict->close();
	;//assert( es==Acad::eOk );
}

// This function creates two objects of class AsdkMyClass. 
// It fills them in with the integers 1 and 2, and then adds 
// them to the dictionary associated with the key REACTOR_DICTIONARY_NAME.  

// If this dictionary doesn't exist, it is created and added
// to the named object dictionary.
//
void createDictionary()
{
    AcDbDictionary		*pNamedobj;
	Acad::ErrorStatus	es;

    acdbHostApplicationServices()->workingDatabase()->
        getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

    // Check to see if the dictionary we want to create is
    // already present. If not, create it and add
    // it to the named object dictionary.
    //
    AcDbDictionary *pDict;
    if (pNamedobj->getAt(REACTOR_DICTIONARY_NAME, (AcDbObject*&) pDict,
        AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pDict = new AcDbDictionary;
        AcDbObjectId DictId;

        es=pNamedobj->setAt( REACTOR_DICTIONARY_NAME, pDict, DictId);
		;//assert( es==Acad::eOk );
    }
    es = pNamedobj->close();
	;//assert( es==Acad::eOk );

    if (pDict)
	{
        // Create new objects to add to the new dictionary,
        // add them, then close them.
		Component *pComponent1;
		Component *pComponent2;
		pComponent1 = new Component( 10.1, CT_TRANSOM, MT_STEEL, S_KWIKSTAGE );
		pComponent2 = new Component( 20.2, CT_LEDGER, MT_STEEL, S_KWIKSTAGE );

        AcDbObjectId rId1, rId2; // ? ids

        es = pDict->setAt(_T("COMP1"), pComponent1, rId1);
		;//assert( es==Acad::eOk );
        es = pDict->setAt(_T("COMP2"), pComponent2, rId2);
		;//assert( es==Acad::eOk );

        es = pComponent1->close();
        pComponent2->close();

        es = pDict->close();
		;//assert( es==Acad::eOk );
    }
}

/// ****************************************
/// ****************************************
/// ****************************************
/// **** end dictionary test code ********

/////////////////////////////////////////////////////////////////////////////////
//	Test
//
//	This a test function 
//
void LiftList::Test()
{
//	Tests Dictionary

	createDictionary();

//	iterateDictionary();
}


LiftList & LiftList::operator =(const LiftList &Original)
{
	int				i;
	double			dRL;
	Lift			*pLift;

	DeleteAll();
	for( i=0; i<Original.GetSize(); i++ )
	{
		pLift	= new Lift;
		*pLift	= *Original.GetLift(i);
		dRL		= Original.GetRL(i);
		AddLift( pLift, dRL );
	}
	return *this;
}

int LiftList::DisplayLiftDetailsDialog(CWnd *pParent)
{
	//int iButtonPressed;
	INT_PTR iButtonPressed;
/*	if (m_pLiftDetailsDialog)                       // modeless
	{
		m_pLiftDetailsDialog->ShowWindow(SW_SHOW);
		m_pLiftDetailsDialog->SetActiveWindow();
	}
	else
	{
		m_pLiftDetailsDialog = new LiftDetailsDialog(pParent, this);
		m_pLiftDetailsDialog->Create();
		m_pLiftDetailsDialog->ShowWindow(SW_SHOW);
	}
*/
	m_pLiftDetailsDialog = new LiftDetailsDialog(pParent, this); // modal
	iButtonPressed = m_pLiftDetailsDialog->DoModal();
	DELETE_PTR(m_pLiftDetailsDialog);

	return iButtonPressed;
}


void LiftList::DestroyLiftDetailsDialog()
{
	DELETE_PTR(m_pLiftDetailsDialog);
}

void LiftList::DeleteLiftDetailsDialog()
{
	DELETE_PTR(m_pLiftDetailsDialog);
}

void LiftList::SetRLsForAllLifts(double dRLBottomLift)
{
	int				iLiftID;
	Lift			*pLift;
	double			dRL, dCurrentRL, dRise;
	Vector3D		Vector;
	Matrix3D		Transform;
	LiftRiseEnum	eRise;

	dRL = dRLBottomLift;
	for( iLiftID=0; iLiftID<GetSize(); iLiftID++ )
	{
		pLift = GetLift( iLiftID );

		//////////////////////////////////////////////
		//how far do we have to move this lift?
		dCurrentRL = GetRL(iLiftID);
		if( dRL!=dCurrentRL )
		{
			Vector.set( 0.00, 0.00, dRL-dCurrentRL );
			Transform.setToTranslation( Vector );
			pLift->Move( Transform, true );
		}

		//////////////////////////////////////////////
		//Set the new RL!
		SetRL( iLiftID, dRL );
		
		//////////////////////////////////////////////
		//find the RL for the next lift
		eRise = pLift->GetRise();
		dRise = GetRiseFromRiseEnum( eRise, pLift->GetStarSeparation() );
		dRL+= dRise;
	}
}

LiftListElement * LiftList::GetAt(int iLiftID) const
{
	return CArray<LiftListElement*, LiftListElement*>::GetAt( iLiftID );
}

void LiftList::SetAt(int iLiftID, LiftListElement *pElement)
{
	CArray<LiftListElement*, LiftListElement*>::SetAt( iLiftID, pElement );
}
