//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_LIFTDETAILSDIALOG_H__5E7C3CA6_4A14_11D3_9E8E_00508B043A6C__INCLUDED_)
#define AFX_LIFTDETAILSDIALOG_H__5E7C3CA6_4A14_11D3_9E8E_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiftDetailsDialog.h : header file
//
#include "meccano.h"
#include "MaterialTypeDefinitions.h"
#include "BayDefinitions.h"
#include "KwikscafDialog.h"
	

class LiftList;
class Lift;
class Bay; 

/////////////////////////////////////////////////////////////////////////////
// LiftDetailsDialog dialog
class LiftDetailsDialog : public KwikscafDialog
{
// Construction
public:
	bool AnyMidrail(SideOfBayEnum eSideOfBay, Lift *pSelectedLift );
	void MoveLift( bool bUp );
	void GetAndSetHandrailStyle( Lift *pSelectedLift );
	CString AddDecking( int iSelectedLiftID, Lift *pSelectedLift );
	~LiftDetailsDialog();
	LiftDetailsDialog(CWnd* pParent = NULL, LiftList *pLiftList = NULL);   // standard constructor
	BOOL Create();

// Dialog Data
	//{{AFX_DATA(LiftDetailsDialog)
	enum { IDD = IDD_LIFT_DETAILS_DIALOG };
	CButton	m_btnMoveLiftDown;
	CButton	m_btnMoveLiftUp;
	CButton	m_SWStageboardCheckCtrl;
	CButton	m_NEStageboardCheckCtrl;
	CButton	m_SEStageboardCheckCtrl;
	CButton	m_NWStageboardCheckCtrl;
	CButton	m_EastStageboardButtonCtrl;
	CButton	m_WestStageboardButtonCtrl;
	CButton	m_SouthStageboardButtonCtrl;
	CButton	m_NorthStageboardButtonCtrl;
	CButton	m_DeleteLiftButtonCtrl;
	CComboBox	m_HandrailStyleComboCtrl;
	CButton	m_WestHandrailButtonCtrl;
	CButton	m_SouthHandrailButtonCtrl;
	CButton	m_EastHandrailButtonCtrl;
	CButton	m_NorthHandrailButtonCtrl;
	CButton	m_DeckingCheckCtrl;
	CButton	m_InsertLiftButtonCtrl;
	CEdit	m_LevelEditCtrl;
	CEdit	m_StageEditCtrl;
	CComboBox	m_MaterialTypeCombo;
	CComboBox	m_LiftHeightComboCtrl;
	CEdit	m_DeckHeightEditCtrl;
	CEdit	m_LiftWidthEditCtrl;
	CEdit	m_LiftLengthEditCtrl;
	CComboBox	m_LiftSelectionComboCtrl;
	CSpinButtonCtrl	m_DeckHeightSpinCtrl;
	CString	m_sDeckHeight;
	CString	m_sHandRailStyle;
	CString	m_sLiftHeight;
	CString	m_sLiftLength;
	CString	m_sLiftLevel;
	CString	m_sLiftSelection;
	CString	m_sLiftStage;
	CString	m_sLiftWidth;
	CString	m_sMaterialHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LiftDetailsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap EWButtonBitmapNoPlanks;
	CBitmap EWButtonBitmapOnePlank;
	CBitmap EWButtonBitmapTwoPlanks;
	CBitmap EWButtonBitmapThreePlanks; 
	CBitmap NSButtonBitmapNoPlanks;
	CBitmap NSButtonBitmapOnePlank;
	CBitmap NSButtonBitmapTwoPlanks;
	CBitmap NSButtonBitmapThreePlanks; 

	int iSelectedLiftWestStageboardWidth;
	int iSelectedLiftEastStageboardWidth;
	int iSelectedLiftSouthStageboardWidth;
	int iSelectedLiftNorthStageboardWidth;

	MaterialTypeEnum eSelectedLiftDeckMaterial;

	Lift	   *m_pSelectedLift;
	int			m_iSelectedLiftID;
	int			m_iSelectedLiftComboIndex;
	Bay		   *m_pBaySelectedLift;
	int			m_nID;
	CWnd	   *m_pParent;
	LiftList   *m_pLiftList;

	void LoadDefaults();
	void GetCurrentSettings();
	void UpdateLiftDetails( Lift *pSelectedLift );

	// handrail functions
	bool AnyHandrail(SideOfBayEnum eSideOfBay, Lift *pSelectedLift);
	bool AnyMeshFull(SideOfBayEnum eSideOfBay, Lift *pSelectedLift);
	bool AnyMeshHalf(SideOfBayEnum eSideOfBay, Lift *pSelectedLift);
	void DeleteAnyHandrailsMidrailsToeboards(SideOfBayEnum eSideOfBay, Lift *pSelectedLift);
	void AddHandrailMidrailToeBoard(SideOfBayEnum eSideOfBay, Lift *pSelectedLift);
	void AddHandrailMidrailToeBoard(SideOfBayEnum eSideOfBay);

	// decking functions
	void AddDeckOff();
	void DeckOn();

	// stageboard functions
	void UpdateStageboards();
	void UpdateStageboardBitmap(SideOfBayEnum eSideOfBay);
	int  AnyStageBoards(SideOfBayEnum eSideOfBay);
	void StageBoardWidthClicked(SideOfBayEnum eSideOfBay);
	void IncrementAndSetStageButton(SideOfBayEnum eSideOfBay, int &iStageBoardWidth);
	void DeleteAllStageboardsCornersHopups( Lift *pSelectedLift );
	void DisplayCornerStageboard(CButton *pButton, int iStageboardWidthX, int iStageboardWidthY);
	void SetAndCheckCornerStageboards();
	void AddStageBoardsCornersHopups( Lift *pSelectedLift );
	void SetDeckHeight(double dNewDeckHeight);
	double GetDeckHeightFromEdit();

	// Generated message map functions
	//{{AFX_MSG(LiftDetailsDialog)
	afx_msg void OnDeleteLiftButton();
	afx_msg void OnInsertLiftButton();
	afx_msg void OnLiftDetailsCancelButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeLiftSelectionCombo();
	afx_msg void OnChangeDeckHeightEdit();
	afx_msg void OnSelchangeLiftSelectionCombo();
	afx_msg void OnDeckingCheck();
	afx_msg void OnNorthHandrailButton();
	afx_msg void OnWestHandrailButton();
	afx_msg void OnEastHandrailButton();
	afx_msg void OnSouthHandrailButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNorthStageboardButton();
	afx_msg void OnSouthStageboardButton();
	afx_msg void OnWestStageboardButton();
	afx_msg void OnEastStageboardButton();
	afx_msg void OnSelchangeMaterialTypeCombo();
	virtual void OnOK();
	afx_msg void OnSelchangeLiftHeightCombo();
	afx_msg void OnDeltaposDeckHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMoveLiftUp();
	afx_msg void OnMoveLiftDown();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSelchangeHandrailStyleCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DeleteAllHandrailComponents( SideOfBayEnum eSide );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIFTDETAILSDIALOG_H__5E7C3CA6_4A14_11D3_9E8E_00508B043A6C__INCLUDED_)
