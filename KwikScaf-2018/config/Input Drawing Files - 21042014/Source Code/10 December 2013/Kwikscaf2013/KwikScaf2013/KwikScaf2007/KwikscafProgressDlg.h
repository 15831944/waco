//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_KWIKSCAFPROGRESSDLG_H__5E2B75B8_7EC2_11D4_9FD4_0008C7999B1D__INCLUDED_)
#define AFX_KWIKSCAFPROGRESSDLG_H__5E2B75B8_7EC2_11D4_9FD4_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KwikscafProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKwikscafProgressDlg dialog
#include "KwikscafDialog.h"

class CKwikscafProgressDlg : public KwikscafDialog
{
// Construction
public:
	CString m_sProgressTitleTemp;
	bool m_bInitialized;
	void SetText( CString sText );
	void SetPercent( int iPercent );
	CKwikscafProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKwikscafProgressDlg)
	enum { IDD = IDD_KWIKSCAF_PROGRESS };
	CProgressCtrl	m_ctrlProgressBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKwikscafProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKwikscafProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KWIKSCAFPROGRESSDLG_H__5E2B75B8_7EC2_11D4_9FD4_0008C7999B1D__INCLUDED_)
