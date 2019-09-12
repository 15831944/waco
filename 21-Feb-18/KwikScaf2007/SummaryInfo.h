// SummaryInfo.h: interface for the SummaryInfo class.
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

#if !defined(AFX_SUMMARYINFO_H__BE8A8546_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_)
#define AFX_SUMMARYINFO_H__BE8A8546_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "ComponentList.h"
#include "SummaryElement.h"

class SummaryInfo : public CArray<SummaryElement*, SummaryElement*>
{
public:
	SummaryInfo();
	virtual ~SummaryInfo();

	SummaryElement *GetElement( CString sStage, int iLevel );
	SummaryElement *GetElement( int iElementID );
	bool AddElement( CString sStage, int iLevel, Component *pComponent );
};

#endif // !defined(AFX_SUMMARYINFO_H__BE8A8546_7AB1_11D3_9ED5_0008C7999B1D__INCLUDED_)
