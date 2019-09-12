// FreeTieTube.h: interface for the FreeTieTube class.
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

#if !defined(AFX_FREETIETUBE_H__D7C17561_A7DC_11D3_B807_00804890115F__INCLUDED_)
#define AFX_FREETIETUBE_H__D7C17561_A7DC_11D3_B807_00804890115F__INCLUDED_

#include "BayDefinitions.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class BayTieTubeTemplate
{
public:
	BayTieTubeTemplate();
	virtual ~BayTieTubeTemplate();

	double m_dAngle;
	CornerOfBayEnum m_eStandard;
	double m_dFullLength;
	BayTieTubeTemplate &operator=(const BayTieTubeTemplate &Original);
};


#include <afxtempl.h>

class BayTieTubeTemplates : public CArray<BayTieTubeTemplate*, BayTieTubeTemplate*>
{
public:
	bool HasTubeOnStandard( CornerOfBayEnum eCnr );
	void DeleteAll();
	void Serialize(CArchive &ar);
	BayTieTubeTemplates();
	~BayTieTubeTemplates();
	BayTieTubeTemplates &operator=( const BayTieTubeTemplates &Original);

};

#endif // !defined(AFX_FREETIETUBE_H__D7C17561_A7DC_11D3_B807_00804890115F__INCLUDED_)
