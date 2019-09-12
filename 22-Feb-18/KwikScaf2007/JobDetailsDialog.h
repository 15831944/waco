//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_JOBDETAILSDIALOG_H__03DB8696_6F16_11D3_9EB1_00508B043A6C__INCLUDED_)
#define AFX_JOBDETAILSDIALOG_H__03DB8696_6F16_11D3_9EB1_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afxdisp.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif
#include "KwikscafDialog.h"

class COleDateTime;

// JobDetailsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// JobDetailsDialog dialog

class JobDetailsDialog : public KwikscafDialog
{
// Construction
public:
	bool ExtractCSVField( CString &sCSVData, CString &sField );
	void LoadBranchNames();
	JobDetailsDialog(CWnd* pParent = NULL, JobDescriptionInfo *pJobDescription = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(JobDetailsDialog)
	enum { IDD = IDD_JOB_DETAILS_DIALOG };
	CComboBox	m_cmboBrachName;
	CString	m_sJobBranch;
	CString	m_sClientAddress;
	CString	m_sClientCity;
	CString	m_sClientName;
	CString	m_sClientZip;
	CString	m_sDraftsmanName;
	CString	m_sFaxCountryCode;
	CString	m_sFaxNumber;
	CString	m_sFaxStateCode;
	CString	m_sMarketingEnquiryNumber;
	CString	m_sPhoneCountryCode;
	CString	m_sPhoneNumber;
	CString	m_sPhoneStateCode;
	CString	m_sProjectDuration;
	CString	m_sRefNumber;
	CString	m_sJobTitle;
	COleDateTime	m_ctStartDate;
	CString	m_sClientCompany;
	CString	m_sJobDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JobDetailsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStringArray m_saBranchDetails[BDI_SIZE];

	JobDescriptionInfo * m_pJobDescriptionInfo;

	// Generated message map functions
	//{{AFX_MSG(JobDetailsDialog)
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOBDETAILSDIALOG_H__03DB8696_6F16_11D3_9EB1_00508B043A6C__INCLUDED_)
