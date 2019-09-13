// FreeTieTube.cpp: implementation of the FreeTieTube class.
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
#include "FreeTieTube.h"
#include "VersionDefinitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BayTieTubeTemplate::BayTieTubeTemplate()
{
}

BayTieTubeTemplate::~BayTieTubeTemplate()
{

}

BayTieTubeTemplates::BayTieTubeTemplates()
{
	RemoveAll();
}

BayTieTubeTemplates::~BayTieTubeTemplates()
{
	DeleteAll();
}

void BayTieTubeTemplates::Serialize(CArchive &ar)
{
	int					i, iSize, iStd;
	CString				sMsg, sTemp;
	BayTieTubeTemplate	*pTTT;

//	CArray<BayTieTubeTemplate*, BayTieTubeTemplate*>::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << BAY_TIE_TUBE_VERSION_LATEST;

		iSize = GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pTTT = GetAt(i);
			ar << pTTT->m_dAngle;
			ar << pTTT->m_dFullLength;
			iStd = (int)pTTT->m_eStandard;
			ar << iStd;
		}
	}
	else
	{
		//set default
		DeleteAll();

		VersionNumber uiVersion;
		ar >> uiVersion;

		switch( uiVersion )
		{
		case( BAY_TIE_TUBE_VERSION_1_0_0 ):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				pTTT = new BayTieTubeTemplate();
				ar >> pTTT->m_dAngle;
				ar >> pTTT->m_dFullLength;
				ar >> iStd;
				pTTT->m_eStandard = (CornerOfBayEnum)iStd;
				Add( pTTT );
			}
			break;
		default:
			assert( false );
			if( uiVersion>BAY_TIE_TUBE_VERSION_LATEST )
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
      sMsg+= _T("Class: BayTieTubeTemplates.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), BAY_TIE_TUBE_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

void BayTieTubeTemplates::DeleteAll()
{
	BayTieTubeTemplate *pTTT;

	while( GetSize()>0 )
	{
		pTTT = NULL;
		pTTT = GetAt(0);
		if( pTTT!=NULL )
		{
			delete pTTT;
		}
		else
		{
			assert( false );
		}
		RemoveAt(0);
	}
}

BayTieTubeTemplates & BayTieTubeTemplates::operator =( const BayTieTubeTemplates &Original)
{
	BayTieTubeTemplate	*pOriginalTTT, *pTTT;

	DeleteAll();
	for( int i=0; i<Original.GetSize(); i++ )
	{
		pOriginalTTT = Original.GetAt(i);
		assert( pOriginalTTT!=NULL );
		pTTT = new BayTieTubeTemplate();
		*pTTT = *pOriginalTTT;
		Add( pTTT );
	}
	return *this;
}

bool BayTieTubeTemplates::HasTubeOnStandard(CornerOfBayEnum eCnr)
{
	BayTieTubeTemplate	*pTTT;
	for( int i=0; i<GetSize(); i++ )
	{
		pTTT = GetAt(i);
		assert( pTTT!=NULL );
		if( pTTT->m_eStandard==eCnr )
			return true;
	}
	return false;
}

BayTieTubeTemplate & BayTieTubeTemplate::operator =(const BayTieTubeTemplate &Original)
{
	m_dAngle		= Original.m_dAngle;
	m_eStandard		= Original.m_eStandard;
	m_dFullLength	= Original.m_dFullLength;
	return *this;
}

