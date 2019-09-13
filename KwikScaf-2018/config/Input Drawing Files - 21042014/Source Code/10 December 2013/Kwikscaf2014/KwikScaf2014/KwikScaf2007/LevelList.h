// LevelList.h: interface for the LevelList class.
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

#if !defined(AFX_LEVELLIST_H__42E79461_A77D_11D3_B807_00804890115F__INCLUDED_)
#define AFX_LEVELLIST_H__42E79461_A77D_11D3_B807_00804890115F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "doubleArray.h"

class LevelList : private doubleArray  
{
public:
	void SelectionSort(bool bIncremental=true);
	bool GetMaxMinSpacing( double &dMaxSpacing, double &dMinSpacing );
	double GetMinimumSpacing();
	double GetMaximumSpacing();
	int GetLevelForRL( double dRL );
	void GetRLLimitsForLevel( int iLevel, double &dRLLower, double &dRLUpper );
	void RemoveAll();
	int GetSize();
	double GetLevel( int iPos );
	bool RemoveLevel( int iLevelID, int nCount=1 );
	bool RemoveLevel( double dRL );
	void AddLevel( double dRL, bool bEnsureSeparation=true );
	LevelList();
	virtual ~LevelList();

};

#endif // !defined(AFX_LEVELLIST_H__42E79461_A77D_11D3_B807_00804890115F__INCLUDED_)
