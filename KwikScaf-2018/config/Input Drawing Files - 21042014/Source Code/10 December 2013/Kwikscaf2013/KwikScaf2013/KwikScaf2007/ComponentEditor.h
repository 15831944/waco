#if !defined(AFX_COMPONENTEDITOR_H__60A2EDC4_7F8C_11D4_9FD5_0008C7999B1D__INCLUDED_)
#define AFX_COMPONENTEDITOR_H__60A2EDC4_7F8C_11D4_9FD5_0008C7999B1D__INCLUDED_

#include "componentList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComponentEditor.h : header file
//

#include "KwikScafButton.h"
#include "KwikscafDialog.h"

enum VCEFieldEnum
{
	VCEF_DETAILS_PART_NO,
	VCEF_DETAILS_TYPE,
	VCEF_DETAILS_DESC,
	VCEF_DETAILS_MATERAIL,
	VCEF_DIMENSIONS_LENGTH_COMMON,
	VCEF_DIMENSIONS_LENGTH_ACTUAL,
	VCEF_DIMENSIONS_WIDTH_ACTUAL,
	VCEF_DIMENSIONS_HEIGHT_ACTUAL,
	VCEF_DIMENSIONS_WEIGHT_ACTUAL,
	VCEF_VISUALS_COLOUR,
	VCEF_VISUALS_VISIBLE,
	VCEF_VISUALS_ACTUAL_AVAIL,
	VCEF_SL_STAGE,
	VCEF_SL_LEVEL,
	VCEF_SL_COMMITTED,
	VCEF_OTHER_HIRE,
	VCEF_OTHER_SALE,
	VCEF_OTHER_SYSTEM
};

class	Controller;

/////////////////////////////////////////////////////////////////////////////
// ComponentEditor dialog
class ComponentEditor : public KwikscafDialog
{
// Construction
public:
	void SetInsertingComponent( bool bInserting );
	bool IsInsertingComponent();
	void SetController( Controller *pController );
	void FillDetails(CString sPartNumber);
	void SetComponentList( CompArray *pCompList );
	ComponentEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ComponentEditor)
	enum { IDD = IDD_VISUAL_COMPONENT_DETAILS_DIALOG };
	CStatic	m_picVClogoInsert;
	CStatic	m_picVClogoEdit;
	CButton	m_btnVisualsVisible;
	CButton	m_btnCommitted;
	CKwikScafButton	m_btnVisualsCompColour;
	CComboBox	m_cmboSLStage;
	CComboBox	m_cmboSLLevel;
	CButton	m_btnCancel;
	CButton	m_btnOK;
	CString	m_sDetailsMaterial;
	CString	m_sDetailsDesc;
	CString	m_sDetailsPartNumber;
	CString	m_sDetailsType;
	CString	m_sDimHeightActual;
	CString	m_sDimLengthActual;
	CString	m_sDimLengthCommon;
	CString	m_sDimWeightActual;
	CString	m_sDimWidthActual;
	CString	m_sOtherHire;
	CString	m_sOtherSale;
	CString	m_sOtherSystem;
	int		m_iSLCommitted;
	CString	m_sVisualActualCompAvail;
	int		m_iVisualsVisible;
	CString	m_sStage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ComponentEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool		m_bInsertingComponent;
	bool		m_bSLCommittedMultiselect;
	bool		m_bVisualVisibleMultiselect;
	bool		m_bSLLevelMultiExists;
	bool		m_bSLStageMultiExists;
	Controller	*m_pController;
	CompArray	*m_pclComponentList;

	// Generated message map functions
	//{{AFX_MSG(ComponentEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnDetailsChangePartNo();
	afx_msg void OnStageLevelCommitted();
	afx_msg void OnEditchangeSLLevelCombo();
	afx_msg void OnSelchangeSLLevelCombo();
	afx_msg void OnEditchangeSLStageCombo();
	afx_msg void OnSelchangeSLStageCombo();
	afx_msg void OnVisualsComponentColour();
	afx_msg void OnVisualsVisible();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnKillfocusSlStageCombo();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void FillSLStageList();
	void FillSLLevelList();
	Controller * GetController();
	void DialogHasChanged( bool bChanged );
//	bool IsFieldMultiselect( VCEFieldEnum eField );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPONENTEDITOR_H__60A2EDC4_7F8C_11D4_9FD5_0008C7999B1D__INCLUDED_)
