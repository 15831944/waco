// SummaryElement.h: interface for the SummaryElement class.
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

#if !defined(AFX_SUMMARYELEMENT_H__BE8A8547_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_)
#define AFX_SUMMARYELEMENT_H__BE8A8547_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_

#include "component.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SummaryElement  
{
public:
	void SetCommitted( bool bCommitted=true );
	bool IsCommitted();
	SummaryElement();
	virtual ~SummaryElement();

	int			GetLevel();
	CString		GetStage();
	Component	*GetComponent( int iComponentID );

	void SetLevel( int iLevel );
	void SetStage( CString sStage );
	void AddComponent( Component *pComponent );
	int  GetNumberOfComponents();

protected:
	bool	m_bContainsCommitted;
	int		m_iLevel;
	CString	m_sStage;
	CArray<Component*, Component*> m_CompList;
};

#endif // !defined(AFX_SUMMARYELEMENT_H__BE8A8547_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_)
