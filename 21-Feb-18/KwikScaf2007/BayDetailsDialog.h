//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_BAYDETAILSDIALOG_H__C057798A_493D_11D3_8A68_00508B043A6C__INCLUDED_)
#define AFX_BAYDETAILSDIALOG_H__C057798A_493D_11D3_8A68_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BayDetailsDialog.h : header file
//
#include "meccano.h"
#include "meccanoDefinitions.h" 
#include "BayDefinitions.h"	// Added by ClassView
#include "KwikscafDialog.h"
#include "LevelList.h"	// Added by ClassView

const int CANCELLED				= 10;
const int OK_PRESSED			= 11;
const int APPLY_TO_SELECTION	= 12;
const int APPLY_TO_RUN			= 13;

enum StandardListControlsEnum
{
	SLC_NORTH_EAST,
	SLC_SOUTH_EAST,
	SLC_SOUTH_WEST,
	SLC_NORTH_WEST,
	SLC_SIZE
};

/////////////////////////////////////////////////////////////////////////////
// BayDetailsDialog dialog
class Bay;

class BayDetailsDialog : public KwikscafDialog
{
// Construction
public:
	double GetStarSeparation();
	LevelList GetPossibleDeckRLs();
	bool CheckBeforeOverwritingStandards();
	void EnableStandardEdits( BOOL bEnable );
	void SetShowApplyWarning( bool bShow );
	bool ShowApplyWarning();
	int GetNumberOfLiftFromTWH();
	bool DeleteStandardBoxElement( SideOfBayEnum eSide, CListBox &StandardListCtrl, CornerOfBayEnum eCnr, CEdit &m_StandardEditCtrl );
	void RecreateAllStandardsArrangments();
	void UpdateAllStandards();
	~BayDetailsDialog();
	//BayDetailsDialog(CWnd* pParent = NULL);
	BayDetailsDialog(CWnd* pParent = NULL, Bay *pBay = NULL);   // standard constructor
	BOOL Create();

// Dialog Data
	//{{AFX_DATA(BayDetailsDialog)
	enum { IDD = IDD_BAY_DETAILS_DIALOG };
	CButton	m_ctrlSouthSideOnly;
	CButton	m_ctrlMatchDecksToLevels;
	CButton	m_btnOK;
	CButton	m_ApplyButton;
	CButton	m_btnUseMidrailWithChain;
	CButton	m_btnFullMeshDecksOnly;
	CComboBox	m_MaterialTypeCombo;
	CComboBox	m_BayWidthComboCtrl;
	CComboBox	m_BayLengthComboCtrl;
	CEdit	m_NumberOfStagesEditCtrl;
	CButton	m_ApplyToSelectionButtonCtrl;
	CEdit	m_NumberOfDecksEditCtrl;
	CEdit	m_TopWorkingHeightEditCtrl;
	CButton	m_StaggeredHeightsButtonCtrl;
	CComboBox	m_StageBracketWidthsComboCtrl;
	CSpinButtonCtrl	m_TopWorkingHeightSpinCtrl;
	CSpinButtonCtrl	m_NumberOfDecksSpinCtrl;
	CSpinButtonCtrl	m_NumberOfStagesSpinCtrl;
	BOOL	m_bHandOptionsChain;
	BOOL	m_bHandOptionsShadecloth;
	BOOL	m_bStaggeredHeights;
	int		m_iCourseFitRadio;
	int		m_iRailOptions;
	CString	m_sNumberOfStages;
	int		m_iStagePositioning;
	CEdit	m_StandardEditCtrlNE;
	CEdit	m_StandardEditCtrlSE;
	CEdit	m_StandardEditCtrlSW;
	CEdit	m_StandardEditCtrlNW;
	CButton	m_SoleboardCheckCtrlNE;
	CButton	m_SoleboardCheckCtrlSE;
	CButton	m_SoleboardCheckCtrlSW;
	CButton	m_SoleboardCheckCtrlNW;
	CListBox	m_dStandardListCtrlNE;
	CListBox	m_dStandardListCtrlSE;
	CListBox	m_dStandardListCtrlSW;
	CListBox	m_dStandardListCtrlNW;
	BOOL	m_bSoleboardNE;
	BOOL	m_bSoleboardSE;
	BOOL	m_bSoleboardSW;
	BOOL	m_bSoleboardNW;
	CString	m_sStandardRLNW;
	CString	m_sStandardRLNE;
	CString	m_sStandardRLSE;
	CString	m_sStandardRLSW;
	BOOL	m_bFullMeshDecksOnly;
	BOOL	m_bLedgersEveryMetre;
	BOOL	m_bUseMidrailWithChain;
	BOOL	m_bMovingDeck;
	int		m_iRoofProtection;
	BOOL	m_bMatchDecksToLevels;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BayDetailsDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double m_dTopWorkingRL;
	bool m_bStandardsManuallyChanged;
	bool m_bShowApplyWarning;
	double m_dArtificialTWH;
	int		m_nID;
	Bay		*m_pBay;
	Bay		*m_pBayCopy;
	bool	m_bApplyRequired;
	bool	bStandardsArrangementDirty;
	void	LoadDefaults();
	void	GetCurrentSettings();
	void	UpdateStandardsArrangementCorner(CListBox *pStandardsListBoxCtrl, CEdit *pStandardEditCtrl, CornerOfBayEnum eStandardCorner);
	void	SetStandardsArrangementDirtyFlag(bool dirty);
	void	SetTopDeckHeight(double dNewDeckHeight);
	void	SetNumberOfDecksInEdit(int iNumberOfDecks);
	void	SetNumberOfStagesInEdit(int iNumberOfStages);
	CWnd	* m_pParent;
	double	GetTopDeckHeightFromEdit();
	
	// Generated message map functions
	//{{AFX_MSG(BayDetailsDialog)
	afx_msg void OnBayDetailsCancelButton();
	afx_msg void OnLiftdetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnNwSoleboardCheck();
	afx_msg void OnSeSoleboardCheck();
	afx_msg void OnSwSoleboardCheck();
	afx_msg void OnNeSoleboardCheck();
	afx_msg void OnCourseFitRadio();
	afx_msg void OnFineFitRadio();
	afx_msg void OnSameFitRadio();
	afx_msg void OnStaggeredHeightsCheck();
	afx_msg void OnApplySelectionButton();
	afx_msg void OnDeltaposTopWorkingHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNumberDecksSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposNumberStagesSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHandoptionsMeshRadio();
	afx_msg void OnHandoptionsFullmeshRadio();
	afx_msg void OnHandoptionsShadeclothCheck();
	afx_msg void OnHandoptionsChainCheck();
	afx_msg void OnChangeTopWorkingHeightEdit();
	afx_msg void OnKillfocusTopWorkingHeightEdit();
	afx_msg void OnKillfocusNumberDecksEdit();
	afx_msg void OnKillfocusNumberStagesEdit();
	afx_msg void OnSelchangeBayLengthCombo2();
	afx_msg void OnSelchangeBayWidthCombo2();
	afx_msg void OnDblclkNeStandardLbox();
	afx_msg void OnDblclkSeStandardLbox();
	afx_msg void OnDblclkSwStandardLbox();
	afx_msg void OnDblclkNwStandardLbox();
	afx_msg void OnSelchangeStageBracketWidthsCombo();
	afx_msg void OnChangeNumberDecksEdit();
	afx_msg void OnChangeNumberStagesEdit();
	afx_msg void OnEditchangeDeckMaterialTypeCombo();
	afx_msg void OnStagesPositonDecks();
	afx_msg void OnStagesPositonSouth();
	afx_msg LONG onAcadKeepFocus( UINT, LONG );
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHandoptionsFullMeshDecksOnly();
	afx_msg void OnUseMidrailWithChain();
	afx_msg void OnApplyButton();
	afx_msg void OnRailoptionsHandRadio();
	afx_msg void OnLedgersEveryMetre();
	afx_msg void OnOkButtonCtrl();
	afx_msg void OnMovingDeck();
	afx_msg void OnRoofProtection();
	afx_msg void OnRoofProtection1500();
	afx_msg void OnRoofProtection2000();
	afx_msg void OnMatchDeckToLevels();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void SetMatchDecksToLevels( bool bMatch );
	double GetTopBottomStdRL();
	void	DeleteRequested();
	void	ApplyChainMesh();
	void	ApplyShadeCloth();
	void	NumberOfStagesChanged( int iNewNumber );
	bool	IsApplyRequired();
	void	SetApplyRequired( bool bApplyReqd=true );
	int		GetMaximumNumberOfDecks();
	int		GetCurrentNumberOfDecks();
	int		GetNumberOfDecksFromEdit();
	int		GetMaximumNumberOfStages();
	int		GetNumberOfStagesFromEdit();
	bool	OkToExit();
	void	FillBayDimensionCombos();
	void	ApplyHandrailOptionToBay();
	void	ChangeMadeToDialog();
	void	DoubleClickStandardBox( SideOfBayEnum eSide, CListBox &StandardListCtrl, CornerOfBayEnum eCnr, CEdit &StandardEditCtrl );
	HandRailOptionEnum	GetBaysRailSetting();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAYDETAILSDIALOG_H__C057798A_493D_11D3_8A68_00508B043A6C__INCLUDED_)
