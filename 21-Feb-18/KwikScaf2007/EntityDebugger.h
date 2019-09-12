// EntityDebugger.h: interface for the EntityDebugger class.
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

#if !defined(AFX_ENTITYDEBUGGER_H__89CA39A2_ACEB_11D3_B807_00804890115F__INCLUDED_)
#define AFX_ENTITYDEBUGGER_H__89CA39A2_ACEB_11D3_B807_00804890115F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EntityDebuggerElement
{
public:
	int m_iCreateCount;
	EntityDebuggerElement();
	~EntityDebuggerElement();
	AcDbObjectId m_ID;
	AcDbObjectId m_ReactorID;
	void	*m_pPointer;
	int	m_iNumber;
	CString m_sDesc;
	int m_iDeleteCount;
};


#include <afxtempl.h>

class EntityDebugger : public CArray<EntityDebuggerElement*, EntityDebuggerElement*>
{
public:
	void RemoveAll();
	EntityDebugger();
	virtual ~EntityDebugger();
	bool AddEntityInfo( void *pPointer, CString sDesc );
	bool AddEntityInfo( AcDbObjectId id, CString sText );
	bool DeleteEntityInfo( AcDbObjectId id );
	bool DeleteEntityInfo( void *pPointer );
	void PrintRemaining();
};

#endif // !defined(AFX_ENTITYDEBUGGER_H__89CA39A2_ACEB_11D3_B807_00804890115F__INCLUDED_)
