// SummaryInfo.cpp: implementation of the SummaryInfo class.
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
#include "SummaryInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SummaryInfo::SummaryInfo()
{
	RemoveAll();
}

SummaryInfo::~SummaryInfo()
{
	SummaryElement *pEle;

	while( GetSize()>0 )
	{
		pEle = NULL;
		pEle = GetAt(0);
		if( pEle!=NULL )
		{
			delete pEle;
		}
		else
		{
			assert( false );
		}

		RemoveAt(0);
	}
}

bool SummaryInfo::AddElement(CString sStage, int iLevel, Component *pComponent)
{
	SummaryElement	*pEle;

	if( sStage.IsEmpty() )
		sStage = STAGE_DEFAULT_VALUE;
	assert( iLevel>=0 );

	pEle = GetElement( sStage, iLevel );
	if( pEle==NULL )
	{
		pEle = new SummaryElement();
		pEle->SetStage( sStage );
		pEle->SetLevel( iLevel );
		pEle->SetCommitted( pComponent->IsCommitted() );
		Add(pEle);
	}
	pEle->AddComponent( pComponent );

	return true;
}

SummaryElement * SummaryInfo::GetElement(int iElementID)
{
	if( iElementID>GetSize() || iElementID<0 )
		return NULL;
	return GetAt( iElementID );
}

SummaryElement * SummaryInfo::GetElement(CString sStage, int iLevel)
{
	int				iID, iEleLevel;
	CString			sEleStage;
	SummaryElement	*pEle;

	for( iID=0; iID<GetSize(); ++iID )
	{
		pEle = GetElement( iID );
		sEleStage = pEle->GetStage();
		if( sEleStage==sStage )
		{
			iEleLevel = pEle->GetLevel();
			if( iEleLevel==iLevel )
			{
				return pEle;
			}
		}
	}
	return NULL;
}
