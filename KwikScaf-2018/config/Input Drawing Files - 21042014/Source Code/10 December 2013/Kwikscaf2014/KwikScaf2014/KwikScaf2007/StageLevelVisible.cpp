// StageLevelVisible.cpp: implementation of the StageLevelVisible class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "Controller.h"
#include "meccanodefinitions.h"
#include "StageLevelVisible.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern Controller *gpController;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StageLevelVisible::StageLevelVisible()
{
	DeleteAll();
}

StageLevelVisible::~StageLevelVisible()
{
	DeleteAll();
}

//////////////////////////////////////////////////////////////////////
// StageLevelVisibleElement Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StageLevelVisibleElement::StageLevelVisibleElement()
{
	m_sStageName.Empty();
	m_iaHiddenLevels.RemoveAll();
	m_iaHiddenLevels.SetUniqueElementsOnly(true);
}

StageLevelVisibleElement::~StageLevelVisibleElement()
{
	m_iaHiddenLevels.RemoveAll();
}

void StageLevelVisibleElement::SetStageName(CString sStageName)
{
	m_sStageName = sStageName;
}

CString StageLevelVisibleElement::GetStageName()
{
	return m_sStageName;
}

bool StageLevelVisibleElement::IsLevelHidden(int iLevel)
{
	return m_iaHiddenLevels.IsElementOf(iLevel);
}

void StageLevelVisibleElement::SetLevelHidden(int iLevel, bool bHidden)
{
	int i;
	if( bHidden )
	{
		//Add this level to the list of hidden levels,
		//	we have already set unique only!
		m_iaHiddenLevels.Add(iLevel);

		//This list needs to be sorted
		m_iaHiddenLevels.SelectionSort();
	}
	else
	{
		//We don't want to hide this level
		int	iHiddenLevel;
		for( i=0; i<m_iaHiddenLevels.GetSize(); i++ )
		{
			iHiddenLevel = m_iaHiddenLevels.GetAt(i);
			if( iHiddenLevel==iLevel )
			{
				//This is currently hidden, so remove it from the list of hidden levels
				m_iaHiddenLevels.RemoveAt(i);
				return;
			}
			if( iHiddenLevel>iLevel )
			{
				//This is a sorted list and we didn't find it
				return;
			}
		}
		//if we get here than the level is not hidden!
	}
}

CString StageLevelVisible::GetStage(int iStageIndex)
{
	CString						sStageName;
	StageLevelVisibleElement	*pElement;

	sStageName = STAGE_DEFAULT_VALUE;
	if( iStageIndex>=0 && iStageIndex<GetNumberOfStages() )
	{
		pElement = GetAt(iStageIndex);
		sStageName = pElement->GetStageName();
	}
	
	return sStageName;
}

int StageLevelVisible::AddStage(CString sStage)
{
	int							i;
	StageLevelVisibleElement	*pElement;
	for( i=0; i<GetNumberOfStages(); i++ )
	{
		pElement = GetAt(i);
		if( pElement->GetStageName()==sStage )
		{
			//already exists
			return i;
		}
	}

	//If we get to here, then the stage does not yet exist
	pElement = new StageLevelVisibleElement();
	pElement->SetStageName( sStage );
	return Add( pElement );
}

int StageLevelVisible::GetNumberOfStages()
{
	return GetSize();
}

bool StageLevelVisible::IsVisible(CString sStageName, int iLevel)
{
	StageLevelVisibleElement	*pElement;

	pElement = GetElement( sStageName );
	if( pElement!=NULL )
		return !pElement->IsLevelHidden(iLevel);
	return true;
}

void StageLevelVisible::SetHidden(CString sStageName, int iLevel, bool bHidden)
{
	StageLevelVisibleElement	*pElement;

	pElement = GetElement( sStageName );
	if( pElement!=NULL )
		pElement->SetLevelHidden( iLevel, bHidden );
}

void StageLevelVisible::SetVisible(CString sStageName, int iLevel, bool bVisible)
{
	StageLevelVisibleElement	*pElement;

	pElement = GetElement( sStageName );

	if( pElement!=NULL )
		pElement->SetLevelHidden( iLevel, !bVisible );
}

void StageLevelVisible::DeleteAll()
{
	while( GetNumberOfStages()>0 )
	{
		RemoveStage(0);
	}
	RemoveAll();
}

void StageLevelVisible::RemoveStage(int iIndex)
{
	StageLevelVisibleElement	*pElement;

	assert( iIndex>=0 && iIndex<GetNumberOfStages() );

	pElement = GetAt(iIndex);
	delete pElement;
	RemoveAt(iIndex);
}

StageLevelVisibleElement * StageLevelVisible::GetElement(CString sStageName)
{
	int							i;
	StageLevelVisibleElement	*pElement;
	for( i=0; i<GetNumberOfStages(); i++ )
	{
		pElement = GetAt(i);
		if( pElement->GetStageName()==sStageName )
		{
			//already exists
			return pElement;
		}
	}
	return NULL;
}

void StageLevelVisibleElement::Serialize(CArchive &ar)
{
	CString			sMsg, sTemp;
	VersionNumber	uiVersion;

	if (ar.IsStoring())    // Store Object?
	{
		uiVersion = SL_VIS_ELEMENT_VERSION_LATEST;
		ar << uiVersion;

		///////////////////////////////////////////////////////////
		//SL_VIS_ELEMENT_VERSION_1_0_0
		ar << m_sStageName;
		m_iaHiddenLevels.Serialize(ar);
	}
	else
	{
		if( !gpController->IsInsertingBlock() )
		{
			m_sStageName = STAGE_DEFAULT_VALUE;
			m_iaHiddenLevels.RemoveAll();
		}

		ar >> uiVersion;
		switch (uiVersion)
		{
		case(SL_VIS_ELEMENT_VERSION_1_0_0):
			ar >> m_sStageName;
			m_iaHiddenLevels.Serialize(ar);
			break;

		default:
			assert( false );
			if( uiVersion>SL_VIS_ELEMENT_VERSION_LATEST )
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
      sMsg+= _T("Class: StageLevelVisibleElement.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), SL_VIS_ELEMENT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

void StageLevelVisible::Serialize(CArchive &ar)
{
	int							i, j, iSize;
	CString						sMsg, sTemp;
	VersionNumber				uiVersion;
	StageLevelVisibleElement	*pElement, *pEle;

	if (ar.IsStoring())    // Store Object?
	{
		uiVersion = SL_VIS_VERSION_LATEST;
		ar << uiVersion;

		///////////////////////////////////////////////////////////
		//SL_VIS_VERSION_1_0_0
		iSize = GetNumberOfStages();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pElement = GetAt(i);
			pElement->Serialize(ar);
		}
	}
	else
	{
		if( !gpController->IsInsertingBlock() )
		{
			DeleteAll();
		}

		ar >> uiVersion;
		switch (uiVersion)
		{
		case(SL_VIS_VERSION_1_0_0):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				pElement = new StageLevelVisibleElement();
				pElement->Serialize(ar);
				for( j=0; j<GetNumberOfStages(); j++ )
				{
					pEle = GetAt(j);
					if( pElement->GetStageName()==pEle->GetStageName() )
					{
						//already exists, don't add it again
						break;
					}
				}
				Add( pElement );
			}
			break;

		default:
			assert( false );
			if( uiVersion>SL_VIS_VERSION_LATEST )
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
      sMsg+= _T("Class: StageLevelVisible.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), SL_VIS_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

StageLevelVisibleElement & StageLevelVisibleElement::operator=(const StageLevelVisibleElement &Original)
{
	m_iaHiddenLevels = Original.m_iaHiddenLevels;
	m_sStageName = Original.m_sStageName;
	return *this;
}

StageLevelVisible & StageLevelVisible::operator=(const StageLevelVisible &Original)
{
	int							i;
	StageLevelVisibleElement	*pEle, *pOrig;
	DeleteAll();

	for( i=0; i<Original.GetSize(); i++ )
	{
		pOrig = Original.GetAt(i);
		pEle = new StageLevelVisibleElement();
		*pEle = *pOrig;
		Add(pEle);
	}
	assert( GetSize()==Original.GetSize() );

	return *this;
}
