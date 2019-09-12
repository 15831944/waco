#if !defined(AFX_STAGELEVELSHOWHIDEDLG_H__E764F033_9FC6_11D4_9FE8_0008C7999B1D__INCLUDED_)
#define AFX_STAGELEVELSHOWHIDEDLG_H__E764F033_9FC6_11D4_9FE8_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StageLevelShowHideDlg.h : header file
//

#include "KwikscafDialog.h"
#include "StageLevelVisible.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// StageLevelShowHideDlg dialog
class StageLevelShowHideDlg : public KwikscafDialog
{
// Construction
public:
	StageLevelShowHideDlg( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StageLevelShowHideDlg)
	enum { IDD = IDD_SHOW_STAGE_LEVEL };
	CListCtrl	m_lstStageLevel;
	CButton	m_btnShow;
	CButton	m_btnHide;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StageLevelShowHideDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	StageLevelVisible m_StageLevelVisible;

	// Generated message map functions
	//{{AFX_MSG(StageLevelShowHideDlg)
	afx_msg void OnHide();
	afx_msg void OnShow();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDblclkStageLevelList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void HideShowSelection( bool bShow );
	void FillStageLevelList();
	void SetUpColumns();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STAGELEVELSHOWHIDEDLG_H__E764F033_9FC6_11D4_9FE8_0008C7999B1D__INCLUDED_)
