//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_RUNPROPDIALOG_H__DC947116_5046_11D3_9E93_00508B043A6C__INCLUDED_)
#define AFX_RUNPROPDIALOG_H__DC947116_5046_11D3_9E93_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RunPropDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RunPropDialog dialog
class BayDetailsDialog;
class Run;

#include "KwikscafDialog.h"
#include "meccanodefinitions.h"

class RunPropDialog : public KwikscafDialog
{
// Construction
public:
	double	m_dLength2400;
	double	m_dLength1800;
	double	m_dLength1200;
	double	m_dLength0700;
	bool LoadDefaultRun();
	bool SaveDefaultRun();
	BOOL Create();
	RunPropDialog(CWnd* pParent = NULL, BayDetailsDialog *pBayDetailsDialog = NULL, Run *pRun = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(RunPropDialog)
	enum { IDD = IDD_AUTOBUILD_PROPERTIES_DIALOG };
	CComboBox	m_drplstLength0700;
	CComboBox	m_drplstLength1200;
	CComboBox	m_drplstLength1800;
	CComboBox	m_drplstLength2400;
	CButton	m_FitGroupBox;
	CButton	m_OKBtn;
	CButton	m_CloseBtn;
	CButton	m_SetAsDefaultBtn;
	CStatic	m_DistanceFromWallUnitsLbl;
	CStatic	m_DistanceFromWallLbl;
	CStatic	m_BayFitLbl;
	CEdit	m_DistanceFromWallEdit;
	CSpinButtonCtrl	m_RLStartSpin;
	CSpinButtonCtrl	m_RLEndSpin;
	CSpinButtonCtrl	m_DistanceFromWallSpin;
	int m_FitRadio;
	CEdit	m_TiesVerticallyEveryEditCtrl;
	CComboBox	m_BracesEveryComboCtrl;
	CComboBox	m_TiesEveryComboCtrl;
	CComboBox	m_TieTypeComboCtrl;
	CSpinButtonCtrl	m_TiesVerticallyEverySpinCtrl;
	CButton	m_TightFitRadioCtrl;
	CButton	m_LooseFitRadioCtrl;
	CString	m_sBracesEvery;
	CString	m_BaysPerTieBayLbl;
	CString	m_BaysPerBracingBayLbl;
	CButton	m_EditDetailsCtrl;
	CString	m_sTiesVerticallyEvery;
	CString	m_sDistanceFromWall;
	CString	m_sRLEnd;
	CString	m_sRLStart;
	BOOL	m_bUseLongerBracing;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RunPropDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool				m_bIsAutobuild;
	Run					*m_pSelectedRun;
	int					m_nID;
	CWnd				*m_pParent;
	bool				m_bDialogChanged;
	BayDetailsDialog	*m_pBayDetailsDialog;

	void LoadDefaults();
	void GetCurrentSettings();
	// Generated message map functions
	//{{AFX_MSG(RunPropDialog)
	afx_msg void OnAbCloseButton();
	afx_msg void OnAbEditDetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbLooseFitRadio();
	afx_msg void OnAbTightFitRadio();
	afx_msg void OnEditchangeAbTiesEveryCombo();
	afx_msg void OnEditchangeAbTieTypeCombo();
	afx_msg void OnChangeAbTiesVerticallyEveryEdit();
	afx_msg void OnEditchangeAbBracesEveryCombo();
	afx_msg void OnDeltaposAbTiesVerticallyEverySpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAbBracesEveryCombo();
	afx_msg void OnSelendokAbBracesEveryCombo();
	afx_msg void OnSelchangeAbTiesEveryCombo();
	afx_msg void OnSelchangeAbTieTypeCombo();
	afx_msg void OnChangeDistanceFromWall();
	afx_msg void OnDeltaposDistanceFromWallSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeRlEnd();
	afx_msg void OnDeltaposRlEndSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeRlStart();
	afx_msg void OnDeltaposRlStartSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetAsDefault();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnOk();
	afx_msg void OnLoadRun();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKillfocusAbTiesEveryCombo();
	afx_msg void OnLongerBracing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadBracingLengths();
	void LoadBracingLengths();
	void FillBraceLengths();
	void SetTieType( TieTypesEnum eType );
	bool GetFile(CFile &File, UINT uiMode, bool bSave);
	bool IsDialogChanged();
	void SetDialogChanged( bool bChanged=true );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNPROPDIALOG_H__DC947116_5046_11D3_9E93_00508B043A6C__INCLUDED_)
