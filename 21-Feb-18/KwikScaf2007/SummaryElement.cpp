// SummaryElement.cpp: implementation of the SummaryElement class.
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
#include "SummaryElement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SummaryElement::SummaryElement()
{
	m_sStage.Empty();
	m_iLevel=0;
	m_CompList.RemoveAll();
	SetCommitted(false);
}

SummaryElement::~SummaryElement()
{
	//Do not delete the element here, this guy is just a list of pointers to them!
	m_CompList.RemoveAll();
}

void SummaryElement::SetStage(CString sStage)
{
	;//assert( !sStage.IsEmpty() );
	m_sStage = sStage;
}

void SummaryElement::SetLevel(int iLevel)
{
	;//assert( iLevel>0 );
	m_iLevel = iLevel;
}

void SummaryElement::AddComponent(Component *pComponent)
{
	m_CompList.Add( pComponent );
}

CString SummaryElement::GetStage() /* Changed by SJ, 26.07.2007, as part of the migration process */
{
	return m_sStage;
}

int SummaryElement::GetLevel()
{
	return m_iLevel;
}

Component *SummaryElement::GetComponent( int iComponentID )
{
	return m_CompList.GetAt( iComponentID );
}


int SummaryElement::GetNumberOfComponents()
{
	return m_CompList.GetSize();
}

bool SummaryElement::IsCommitted()
{
	return m_bContainsCommitted;
}

void SummaryElement::SetCommitted(bool bCommitted)
{
	m_bContainsCommitted = bCommitted;
}
