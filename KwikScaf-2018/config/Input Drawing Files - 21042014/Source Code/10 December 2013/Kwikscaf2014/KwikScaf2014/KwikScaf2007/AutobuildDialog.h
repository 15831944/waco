//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_AUTOBUILDDIALOG_H__578249C3_4E02_11D3_9E92_00508B043A6C__INCLUDED_)
#define AFX_AUTOBUILDDIALOG_H__578249C3_4E02_11D3_9E92_00508B043A6C__INCLUDED_

//This file & class was originally called BayPropertiesDialog

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutobuildDialog.h : header file
//
#include "meccano.h"
#include "AutobuildTools.h"
#include "KwikscafDialog.h"

/////////////////////////////////////////////////////////////////////////////
// AutobuildDialog dialog
class BayDetailsDialog;
class RunPropDialog;

class AutobuildDialog : public KwikscafDialog
{
// Construction
public:
	AutobuildDialog(CWnd* pParent = NULL, AutoBuildTools *pAutobuildTools = NULL);   // standard constructor
	virtual ~AutobuildDialog();
	BOOL Create();

// Dialog Data
	//{{AFX_DATA(AutobuildDialog)
	enum { IDD = IDD_BAY_PROPERTIES_DIALOG };
	CEdit	m_sDeckCtrl;
	CComboBox	m_sBayWidthCtrl;
	CComboBox	m_sBayLengthCtrl;
	CSpinButtonCtrl	m_DeckSpinCtrl;
	CString	m_sBayLength;
	CString	m_sBayWidth;
	CString	m_sDeck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutobuildDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HINSTANCE m_hInst;
	CString m_sOriginalWidth;
	CString m_sOriginalLength;
	int m_nID;
	CWnd * m_pParent;
	AutoBuildTools * m_pAutobuildTools;
	BayDetailsDialog *m_pBayDetailsDialog;

	void LoadDefaults();
	void SetCurrentSettings();

	// Generated message map functions
	//{{AFX_MSG(AutobuildDialog)
	afx_msg void OnAdvancedButton();
	afx_msg void OnEditDetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposDeckSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeBayLengthCombo();
	afx_msg void OnSelchangeBayWidthCombo();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMove(int x, int y);
	afx_msg LONG onAcadKeepFocus( UINT, LONG );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOBUILDDIALOG_H__578249C3_4E02_11D3_9E92_00508B043A6C__INCLUDED_)
