//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_SETSTAGESDLG_H__661ADB81_A571_11D3_B807_00804890115F__INCLUDED_)
#define AFX_SETSTAGESDLG_H__661ADB81_A571_11D3_B807_00804890115F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetStagesDlg.h : header file
//

#include "Bay.h"
#include "KwikscafDialog.h"

class Controller;
class BayStageLevelArray;
class LevelList;

/////////////////////////////////////////////////////////////////////////////
// SetStagesDlg dialog

class SetStagesDlg : public KwikscafDialog
{
// Construction
public:
	bool IsColourByStageLevel();
	void SetColourByStageLevel( bool bColourByStageLevel );
	void SelectUnassigned();
	SetStagesDlg( CWnd* pParent = NULL);   // standard constructor
	void SetBayList( BayList *pBays );
	void SetController( Controller *pController );

// Dialog Data
	//{{AFX_DATA(SetStagesDlg)
	enum { IDD = IDD_SET_STAGES_DIALOG };
	CComboBox	m_UpperLevelCombo;
	CComboBox	m_LowerLevelCombo;
	CListCtrl	m_ListBoxCtrl;
	BOOL	m_bColour3DByStageLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SetStagesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BayList		*m_pBays;
	Controller	*m_pController;
	LevelList	*m_Levels;

	// Generated message map functions
	//{{AFX_MSG(SetStagesDlg)
	afx_msg void OnSetstagesAssignStageButton();
	afx_msg void OnSelchangeLowerLevelCombo();
	afx_msg void OnSelectBaysButton();
	afx_msg void OnSelchangeUpperLevelCombo();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowHide();
	afx_msg void OnDblclkSetstagesListbox(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void AssignStageToSelections( CString sStageName );
	void UpdateBayListFromTable();
	void UpdateTableSelectionFromBayList();
	void FillLevelCtrls( int iMinLevel, int iMaxLevel );
	void GetRangeOfLevelsForBay(Bay *pSelectedBay, int &iMinLevel, int &iMaxLevel);
	void GetRangeOfLevelsForSelectedBays( int &iMinLevel, int &iMaxLevel );
	void SetColumnHeadings();
	void FillStageLevelBox();
	void OnSelchangeSetLevelCombo( bool bUpper );
	BayStageLevelArray *GetBayStageLevelArray();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETSTAGESDLG_H__661ADB81_A571_11D3_B807_00804890115F__INCLUDED_)
