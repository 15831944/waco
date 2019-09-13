//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_HANGERLDLG_H__E2F7F804_B337_11D3_9F0C_0008C7999B1D__INCLUDED_)
#define AFX_HANGERLDLG_H__E2F7F804_B337_11D3_9F0C_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeRLDlg.h : header file
//

#include "KwikscafDialog.h"

//forward declarations
class BayList;
class Bay;

/////////////////////////////////////////////////////////////////////////////
// ChangeRLDlg dialog

class ChangeRLDlg : public KwikscafDialog
{
// Construction
public:
	double GetStarSeparation() const;
	void SetBayList( BayList *pBays );
	void SetLastValue(double *pdLastValue);
	ChangeRLDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ChangeRLDlg)
	enum { IDD = IDD_SET_RL_DLG };
	CEdit	m_ctrlRLStart;
	CEdit	m_ctrlRLEnd;
	CButton	m_btnCancel;
	CString	m_sBayNumberEnd;
	CString	m_sBayNumberStart;
	CString	m_sBayLengthEnd;
	CString	m_sBayLengthStart;
	CString	m_sRLEnd;
	CString	m_sRLStart;
	CString	m_sSeparationDistance;
	CString	m_sBaysSelectedLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ChangeRLDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool	m_bStoreAsLast;
	double *m_pdLastValue;
	double m_dSeparation;
	BayList *m_pBays;

	// Generated message map functions
	//{{AFX_MSG(ChangeRLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposRlEndSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRlStartSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusRlStart();
	afx_msg void OnKillfocusRlEnd();
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeRlStart();
	afx_msg void OnChangeRlEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool CheckAndWarnForRL( bool bStart );
	bool CheckRLisValidForBay( Bay *pBay, double dRLStart, double dRLEnd );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HANGERLDLG_H__E2F7F804_B337_11D3_9F0C_0008C7999B1D__INCLUDED_)
