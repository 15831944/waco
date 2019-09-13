//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_BOMSELECTIONDIALOG_H__CA3E3FA3_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_)
#define AFX_BOMSELECTIONDIALOG_H__CA3E3FA3_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BOMSelectionDialog.h : header file
//

#include "KwikscafDialog.h"
class SummaryInfo;

/////////////////////////////////////////////////////////////////////////////
// BOMSelectionDialog dialog

class BOMSelectionDialog : public KwikscafDialog
{
// Construction
public:
	BOMSelectionDialog(CWnd* pParent = NULL, SummaryInfo *pSummaryInfo = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BOMSelectionDialog)
	enum { IDD = IDD_BOM_STAGE_LEVEL_DIALOG };
	CButton	m_chkAmalgamate;
	CButton	m_SummaryButtonCtrl;
	CButton	m_RemoveButtonCtrl;
	CButton	m_RemoveAllButtonCtrl;
	CButton	m_AddButtonCtrl;
	CButton	m_AddAllButtonCtrl;
	CListBox	m_BOMSSelectedListCtrl;
	CListBox	m_BOMSSelectionListCtrl;
	BOOL	m_bAmalgamate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BOMSelectionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateVisibleButtons();
	SummaryInfo *m_pSummaryInfo;

	void LoadList();
	// Generated message map functions
	//{{AFX_MSG(BOMSelectionDialog)
	afx_msg void OnBomslAddButton();
	afx_msg void OnBomslAddAllButton();
	afx_msg void OnBomslRemoveAllButton();
	afx_msg void OnBomslRemoveButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBomSelectionListboxCtrl();
	afx_msg void OnSelchangeBomSelectedListboxCtrl();
	afx_msg void OnBomsSummaryButton();
	virtual void OnCancel();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDblclkBomSelectionListboxCtrl();
	afx_msg void OnDblclkBomSelectedListboxCtrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOMSELECTIONDIALOG_H__CA3E3FA3_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_)
