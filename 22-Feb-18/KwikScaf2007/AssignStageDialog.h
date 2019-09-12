//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_ASSIGNSTAGEDIALOG_H__A44C6BA1_8FE9_11D3_ADB6_FBF5250E4B1D__INCLUDED_)
#define AFX_ASSIGNSTAGEDIALOG_H__A44C6BA1_8FE9_11D3_ADB6_FBF5250E4B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AssignStageDialog.h : header file
//

#include "KwikscafDialog.h"

class Controller;

/////////////////////////////////////////////////////////////////////////////
// AssignStageDialog dialog

class AssignStageDialog : public KwikscafDialog
{
// Construction
public:
	AssignStageDialog(CWnd* pParent, CString *pStageNameToAssign, Controller *pController );   // standard constructor

// Dialog Data
	//{{AFX_DATA(AssignStageDialog)
	enum { IDD = IDD_ASSIGN_STAGE_DALOG };
	CComboBox	m_AssignStageNameComboCtrl;
	CString	m_sAssignStageNameCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AssignStageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString			*m_pStageNameToAssign;
	Controller		*m_pController;

	// Generated message map functions
	//{{AFX_MSG(AssignStageDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSIGNSTAGEDIALOG_H__A44C6BA1_8FE9_11D3_ADB6_FBF5250E4B1D__INCLUDED_)
