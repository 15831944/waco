#if !defined(AFX_HELPABOUTDLG_H__7B925164_2D08_11D4_9F92_0008C7999B1D__INCLUDED_)
#define AFX_HELPABOUTDLG_H__7B925164_2D08_11D4_9F92_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpAboutDlg.h : header file
//

#include "KwikscafDialog.h"
#include "resource.h"
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// HelpAboutDlg dialog

class HelpAboutDlg : public KwikscafDialog
{
// Construction
public:
	void SetModal( bool bModal );
	void SetHardwareLockID( int iLockID );
	bool ShowHardwareKeyMsg();
	void SetShowHardwareKeyMsg( bool bShow );
	HelpAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HelpAboutDlg)
	enum { IDD = IDD_HELP_ABOUT };
	CStatic	m_Pic;
	CAnimateCtrl	m_Avi;
	CButton	m_btnOK;
	CString	m_sVersionNumber;
	CString	m_sCopyrightMessage;
	CString	m_sHardwardLockID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HelpAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTime m_StartTime;
	bool m_bModal;
	bool m_bShowHardwareKeyMsg;

	// Generated message map functions
	//{{AFX_MSG(HelpAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedPic();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPABOUTDLG_H__7B925164_2D08_11D4_9F92_0008C7999B1D__INCLUDED_)
