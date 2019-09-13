// StageLevelVisible.h: interface for the StageLevelVisible class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STAGELEVELVISIBLE_H__264D0CC4_9EFA_11D4_9FE7_0008C7999B1D__INCLUDED_)
#define AFX_STAGELEVELVISIBLE_H__264D0CC4_9EFA_11D4_9FE7_0008C7999B1D__INCLUDED_

#include "doubleArray.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StageLevelVisibleElement  
{
public:
	void Serialize(CArchive &ar);
	StageLevelVisibleElement &operator=(const StageLevelVisibleElement &Original);

	//Hidden Levels
	bool IsLevelHidden( int iLevel );
	void SetLevelHidden( int iLevel, bool bHidden );

	//Stage
	CString GetStageName();
	void SetStageName( CString sStageName );

	StageLevelVisibleElement();
	virtual ~StageLevelVisibleElement();

protected:
	intArray m_iaHiddenLevels;
	CString m_sStageName;
};

#include <afxtempl.h>

class StageLevelVisible : protected CArray<StageLevelVisibleElement*, StageLevelVisibleElement*>
{
public:
	void Serialize(CArchive &ar);
	StageLevelVisible &operator=(const StageLevelVisible &Original);

	void RemoveStage( int iIndex );
	void DeleteAll();
	void SetVisible( CString sStageName, int iLevel, bool bVisible );
	void SetHidden( CString sStageName, int iLevel, bool bHidden );
	bool IsVisible( CString sStageName, int iLevel );
	int GetNumberOfStages();
	int AddStage( CString sStage );
	CString GetStage( int iStageIndex );
	StageLevelVisible();
	virtual ~StageLevelVisible();

private:
	StageLevelVisibleElement * GetElement( CString sStageName );
};



#endif // !defined(AFX_STAGELEVELVISIBLE_H__264D0CC4_9EFA_11D4_9FE7_0008C7999B1D__INCLUDED_)
