//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_STANDARDHEIGHTPICKERDIALOG_H__B073EF63_8B50_11D3_9EC2_00508B043A6C__INCLUDED_)
#define AFX_STANDARDHEIGHTPICKERDIALOG_H__B073EF63_8B50_11D3_9EC2_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StandardHeightPickerDialog.h : header file
//

#include "KwikscafDialog.h"

/////////////////////////////////////////////////////////////////////////////
// StandardHeightPickerDialog dialog


class StandardHeightPickerDialog : public KwikscafDialog
{
// Construction
public:
	StandardHeightPickerDialog(CWnd* pParent = NULL, CString *psSelection = NULL, double *pdNewStandardLength = NULL, int *piOpenEnded=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StandardHeightPickerDialog)
	enum { IDD = IDD_STANDARD_HEIGHT_DIALOG };
	CButton	m_btnOK;
	CComboBox	m_StandardHeightsComboCtrl;
	CString	m_sStandardHeightsCombo;
	int		m_iOpenEnded;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StandardHeightPickerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		*m_piOpenEnded;
	CString *m_psSelection;
	double  *m_pdNewStandardLength;

	// Generated message map functions
	//{{AFX_MSG(StandardHeightPickerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkStandardComboCtrl();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnOpenEnded();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STANDARDHEIGHTPICKERDIALOG_H__B073EF63_8B50_11D3_9EC2_00508B043A6C__INCLUDED_)
