// BayStageLevel.cpp: implementation of the BayStageLevel class.
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
#include "BayStageLevel.h"
#include "Bay.h"
#include "Controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const int	INVALID_LEVEL = -1;
extern Controller *gpController;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BayStageLevel::BayStageLevel()
{
	CString sTemp;
	SetBay( NULL );
	SetLevel( INVALID_LEVEL );
	sTemp.Empty();
	SetStage( sTemp );
	SetCommitted( false );
	SetVisible( true );
}

BayStageLevel::~BayStageLevel()
{
}

Bay * BayStageLevel::GetBay()
{
	return m_pBay;
}

void BayStageLevel::SetBay(Bay *pBay)
{
	m_pBay = pBay;
}

int BayStageLevel::GetLevel()
{
	return m_iLevel;
}

void BayStageLevel::SetLevel(int iLevel)
{
	m_iLevel = iLevel;
}

CString BayStageLevel::GetStage()
{
	return m_sStage;
}

void BayStageLevel::SetStage(LPCTSTR strStage)
{
	m_sStage = strStage;
}

BayStageLevelArray::BayStageLevelArray()
{
	SetController(NULL);
	RemoveAll();
}

BayStageLevelArray::~BayStageLevelArray()
{
	BayStageLevel	*pElement;

	while( GetSize()>0 )
	{
		pElement = NULL;
		pElement = GetAt( 0 );
		if( pElement!=NULL )
		{
			delete pElement;
		}
		else
		{
			assert( false );
		}
		RemoveAt(0);
	}
}

void BayStageLevelArray::Serialize(CArchive &ar )
{
	VersionNumber	uiVersion;
	int				i, iSize, iLevel, iBayNumber;
	Bay				*pBay;
	bool			bCommitted, bVisible;
	BOOL			BTemp;
	CString			sMsg, sTemp;
	CString			sStageName;
	BayStageLevel	*pElement;

	assert( m_pController!=NULL );

	if( ar.IsStoring() )
	{
		uiVersion = BAY_STAGE_LEVEL_VERSION_LATEST;
		ar << uiVersion;

		//BAY_STAGE_LEVEL_VERSION_1_0_2
		iSize = 0;
		if( !m_pController->SaveSelectedOnly() )
			iSize = GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pElement = GetAt( i );
			pElement->Serialize(ar);
		}
	}
	else
	{
		bCommitted = false;
		bVisible = true;
		ar >> uiVersion;
		switch( uiVersion )
		{
		case( BAY_STAGE_LEVEL_VERSION_1_0_2 ):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				pElement = new BayStageLevel();
				pElement->Serialize(ar);
				if( pElement->GetBay()==NULL && pElement->GetLevel()==INVALID_LEVEL )
					delete pElement;
				else
					Add( pElement );
			}
			break;	//cannot do the usual fallthrough here!

		case( BAY_STAGE_LEVEL_VERSION_1_0_1 ):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> iBayNumber;
				pBay = m_pController->GetBayFromBayNumber( iBayNumber );
				if( pBay!=NULL )
				{
					pElement = new BayStageLevel();
					pElement->SetBay( pBay );
					ar >> iLevel;
					pElement->SetLevel( iLevel );
					ar >> sStageName;
					pElement->SetStage( sStageName );
					LOAD_bool_IN_AR(bCommitted);
					pElement->SetCommitted( bCommitted );
//					LOAD_bool_IN_AR(bVisible);
					pElement->SetCommitted( bVisible );
					Add( pElement );
				}
				else
				{
					//chuck these away
					ar >> iLevel;
					ar >> sStageName;
					LOAD_bool_IN_AR(bCommitted);
				}
			}
			break;	//cannot do the usual fallthrough here!

		case( BAY_STAGE_LEVEL_VERSION_1_0_0 ):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> iBayNumber;
				pBay = m_pController->GetBayFromBayNumber( iBayNumber );
				if( pBay!=NULL )
				{
					pElement = new BayStageLevel();
					pElement->SetBay( pBay );
					ar >> iLevel;
					pElement->SetLevel( iLevel );
					ar >> sStageName;
					pElement->SetStage( sStageName );
					pElement->SetCommitted( bCommitted );
				}
				else
				{
					//chuck these away
					ar >> iLevel;
					ar >> sStageName;
				}
			}
			break;
		default:
			assert( false );
			if( uiVersion>BAY_STAGE_LEVEL_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: BayStageLevelArray.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), BAY_STAGE_LEVEL_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

CString BayStageLevelArray::GetStage(int iBayNumber, int iLevel)
{
	int				i, iSize;
	Bay				*pBay;
	bool			bFound;
	CString			sStageName, sMsg;
	BayStageLevel	*pElement;

	iSize = GetSize();
	sStageName.Empty();
	bFound = false;
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		assert( pElement!=NULL );

		pBay = pElement->GetBay();

		assert( pBay!=NULL );

/*
		#ifdef _DEBUG
		acutPrintf( "\nlooking for Bay(%i) Level(%i), Have(%i, %i, %s )",
			iBayNumber, iLevel, pBay->GetBayNumber(),
			pElement->GetLevel(), pElement->GetStage() );
		#endif //#ifdef _DEBUG
*/
		if( iBayNumber==pBay->GetBayNumber() &&
			iLevel==pElement->GetLevel() )
		{
			sStageName = pElement->GetStage( );
/*
			#ifdef _DEBUG
			acutPrintf( " - Found it" );
			#endif //#ifdef _DEBUG
*/
			bFound = true;
			break;
		}
	}
/*
#ifdef _DEBUG
	if( !bFound )
	{
		if( iSize>0 )
			acutPrintf( "\nError: Stage Name Not Found or Unassigned for Bay(%i) Level(%i)", iBayNumber, iLevel );

		for( i=0; i<iSize; i++ )
		{
			pElement = GetAt( i );

			pBay = pElement->GetBay();

			assert( pBay!=NULL );

			acutPrintf( "\nArray contents[%i] - %i, %i, %s", i, 
				pBay->GetBayNumber(),
				pElement->GetLevel(), pElement->GetStage() );
		}
	}
	#endif //#ifdef _DEBUG
*/
	return sStageName;
}

void BayStageLevelArray::SetAtGrowStage(int iBayNumber, int iLevel, LPCTSTR strStage)
{
	int				i, iSize;
	Bay				*pBay;
	bool			bFound;
	CString			sStageName;
	BayStageLevel	*pElement;

	assert( m_pController!=NULL );
	iSize = GetSize();
	bFound = false;
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		if( iBayNumber==pElement->GetBay()->GetBayNumber() &&
			iLevel==pElement->GetLevel() )
		{
			pElement->SetStage( strStage );
			return;
		}
	}

	//not found so add a new one!
	pElement = new BayStageLevel();
	pBay = m_pController->GetBayFromBayNumber( iBayNumber );
	if( pBay!=NULL )
	{
		pElement->SetBay( pBay );
		pElement->SetLevel( iLevel );
		pElement->SetStage( strStage );
		Add( pElement );
	}

}

void BayStageLevelArray::SetController(Controller *pController)
{
	m_pController = pController;
}

bool BayStageLevel::IsCommitted()
{
	return m_bCommitted;
}

void BayStageLevel::SetCommitted(bool bCommitted)
{
	m_bCommitted = bCommitted;
}

void BayStageLevelArray::SetCommitted( int iBN, int iLvl, LPCTSTR strStage, bool bComit/*=true*/ )
{
	int				iIndex, iCount;
	Bay				*pB;
	bool			bExists;
	CString			sStageName;
	BayStageLevel	*pEle;

	iCount = GetSize();
	sStageName = strStage;
	bExists = false;
	for( iIndex=0; iIndex<iCount; iIndex++ )
	{
		pEle = GetAt( iIndex );
		pB = pEle->GetBay();
		assert( pB!=NULL );

		if( iBN==pB->GetBayNumber() &&
			iLvl==pEle->GetLevel() &&
			sStageName==pEle->GetStage() )
		{
			bExists = true;
			break;
		}
	}
	if( bExists )
	{
		pEle->SetCommitted( bComit );
	}
	else
	{
		if( iCount>0 )
		{
			//This component must have been stolen from a neighboring bay to support
			//	the structure of a stage that is going out in an earlier stage
		}
	}
}

bool BayStageLevelArray::IsCommitted(int iBayNumber, int iLevel, LPCTSTR strStage )
{
	int				i, iSize;
	Bay				*pBay;
	CString			sStage;
	BayStageLevel	*pElement;

	iSize = GetSize();
	sStage = strStage;
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		pBay = pElement->GetBay();
		assert( pBay!=NULL );
		if( iBayNumber==pBay->GetBayNumber() &&
			iLevel==pElement->GetLevel() &&
			sStage==pElement->GetStage() )
		{
			return pElement->IsCommitted();
		}
	}
	return false;
}

void BayStageLevelArray::ClearCommitted()
{
	int				i, iSize;
	BayStageLevel	*pElement;

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		pElement->SetCommitted( false );
	}
}

void BayStageLevelArray::BayDeleted(Bay *pBay)
{
	int				i, iSize;
	BayStageLevel	*pElement;

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		if( pElement->GetBay()==pBay )
		{
			RemoveAt(i);
			i--;
			iSize = GetSize();
		}
	}
}

int BayStageLevelArray::GetColour(CString sStage, int iLevel)
{
	int				i, iColour, iStages, iLevels;

	iColour = COLOUR_RED;
	if( m_pController!=NULL )
	{
		iStages = m_pController->GetNumberOfStages();
		iLevels = m_pController->GetLevelList()->GetSize()+1;

		if( iStages==0 )
		{
			return iLevel-1+COLOUR_RED;	//There are no stages, colour only by level
		}
		else
		{
			for( i=0; i<iStages; i++ )
			{
				if( sStage==m_pController->GetStage(i) )
				{
					//found it
					return iColour+iLevel;
				}
				else
				{
					//not in this group
					iColour+= iLevels;
				}
			}
			//assert( false );
			return iColour;
		}
	}
	else
	{
		return iLevel-1+COLOUR_RED;	//There are no stages, colour only by level
	}
}

bool BayStageLevel::IsVisible()
{
	return m_bVisible;
}

void BayStageLevel::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

bool BayStageLevelArray::IsVisible(int iBayNumber, int iLevel, LPCTSTR strStage)
{
	int				i, iSize;
	Bay				*pBay;
	CString			sStage;
	BayStageLevel	*pElement;

	iSize = GetSize();
	sStage = strStage;
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		pBay = pElement->GetBay();
		assert( pBay!=NULL );
		if( iBayNumber==pBay->GetBayNumber() &&
			iLevel==pElement->GetLevel() &&
			sStage==pElement->GetStage() )
		{
			return pElement->IsVisible();
		}
	}
	return true;
}

void BayStageLevelArray::SetVisible(int iBN, int iLvl, LPCTSTR strStage, bool bVisible)
{
	int				iIndex, iCount;
	Bay				*pB;
	bool			bExists;
	CString			sStageName;
	BayStageLevel	*pEle;

	iCount = GetSize();
	sStageName = strStage;
	bExists = false;
	for( iIndex=0; iIndex<iCount; iIndex++ )
	{
		pEle = GetAt( iIndex );
		pB = pEle->GetBay();
		assert( pB!=NULL );

		if( iBN==pB->GetBayNumber() &&
			iLvl==pEle->GetLevel() &&
			sStageName==pEle->GetStage() )
		{
			bExists = true;
			break;
		}
	}
	if( bExists )
	{
		pEle->SetVisible( bVisible );
	}
	else
	{
		if( iCount>0 )
		{
			//This component must have been stolen from a neighboring bay to support
			//	the structure of a stage that is going out in an earlier stage
		}
	}
}

void BayStageLevelArray::ClearVisible()
{
	int				i, iSize;
	BayStageLevel	*pElement;

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt( i );
		pElement->SetVisible( true );
	}
}

void BayStageLevel::Serialize(CArchive &ar)
{
	int				iBayNumber, iLevel;
	Bay				*pBay;
	bool			bCommitted, bVisible;
	BOOL			BTemp;
	CString			sStageName, sMsg, sTemp;
	VersionNumber	uiVersion;

	if( ar.IsStoring() )
	{
		uiVersion = BAY_STAGE_LEVEL_ELEMENT_VERSION_LATEST;
		ar << uiVersion;

		//BAY_STAGE_LEVEL_ELEMENT_VERSION_1_0_0
		ar << GetBay()->GetBayNumber();
		ar << GetLevel();
		ar << GetStage();
		bCommitted = IsCommitted();
		STORE_bool_IN_AR(bCommitted);
		bVisible = IsVisible();
		STORE_bool_IN_AR(bVisible);
	}
	else
	{
		bCommitted = false;
		bVisible = true;
		ar >> uiVersion;
		switch( uiVersion )
		{
		case( BAY_STAGE_LEVEL_ELEMENT_VERSION_1_0_0 ):
			ar >> iBayNumber;
			pBay = gpController->GetBayFromBayNumber( iBayNumber );
			if( pBay!=NULL )
			{
				SetBay( pBay );
				ar >> iLevel;
				SetLevel( iLevel );
				ar >> sStageName;
				SetStage( sStageName );
				LOAD_bool_IN_AR(bCommitted);
				SetCommitted( bCommitted );
				LOAD_bool_IN_AR(bVisible);
				SetVisible( bVisible );
			}
			else
			{
				//chuck these away
				SetBay( NULL );
				SetLevel( INVALID_LEVEL );
				ar >> iLevel;
				ar >> sStageName;
				LOAD_bool_IN_AR(bCommitted);
				LOAD_bool_IN_AR(bVisible);
			}
			break;	//cannot do the usual fallthrough here!

		default:
			assert( false );
			if( uiVersion>BAY_STAGE_LEVEL_ELEMENT_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: BayStageLevel.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), BAY_STAGE_LEVEL_ELEMENT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

bool BayStageLevelArray::IsStageLevelVisible(int iLevel, CString sStage)
{
	int				i;
	BayStageLevel	*pElement;

	for( i=0; i<GetSize(); i++ )
	{
		pElement = GetAt( i );
		if( pElement->GetLevel()==iLevel &&
			pElement->GetStage()==sStage )
		{
			return pElement->IsVisible();
		}
	}
	return true;
}
