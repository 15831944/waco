#if !defined(AFX_EDITSTANDARDSDLG_H__7AC49974_F94D_11D3_9F5F_0008C7999B1D__INCLUDED_)
#define AFX_EDITSTANDARDSDLG_H__7AC49974_F94D_11D3_9F5F_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditStandardsDlg.h : header file
//

#include "KwikScafDialog.h"
#include "BayDefinitions.h"
#include "MeccanoDefinitions.h"
#include "doubleArray.h"	// Added by ClassView


//forward declarations
class Controller;
class Matrix;
class MatrixElement;
class AcGsView;
class Bay;

/////////////////////////////////////////////////////////////////////////////
// EditStandardsDlg dialog

class EditStandardsDlg : public KwikscafDialog
{
// Construction
public:
	double JackLength() const;
	void SetCommittedStageLevel(Bay *pBay, CornerOfBayEnum eCnr, intArray &iaCommitted, intArray &iaLevels, doubleArray &daRLs, CStringArray &saStages);
	bool TestCommitted( doubleArray daStdComp, CornerOfBayEnum eCnr, Bay *pBay );
	void DeleteOuter();
	void DeleteInner();
	void DeleteRequested();
	void FillStandardsFromArray();
	MatrixElement * GetFirstSelectedElement();
	EditStandardsDlg( Controller *pController, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EditStandardsDlg)
	enum { IDD = IDD_EDIT_STANDARDS };
	CButton	m_btnApply;
	CListCtrl	m_lstctrlBays;
	CListBox	m_lstboxStandardsOuter;
	CListBox	m_lstboxStandardsInner;
	CListCtrl	m_lstctrlMatrixSelection;
	CString	m_sHeightInner;
	CString	m_sHeightOuter;
	BOOL	m_bStaggered;
	int		m_iStandardsFit;
	BOOL	m_bSBInner;
	BOOL	m_bSBOuter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditStandardsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double		m_dStarSeparation;
	Matrix		*m_pMatrix;
	Controller	*m_pController;
	bool		m_bApplyRequired;
	doubleArray m_daStandardsOuter;
	intArray	m_iaOpenEndedOuter;
	doubleArray m_daStandardsInner;
	intArray	m_iaOpenEndedInner;
	SystemEnum	m_eSystem;

	// Generated message map functions
	//{{AFX_MSG(EditStandardsDlg)
	afx_msg void OnApply();
	afx_msg void OnCourseFitRadio();
	afx_msg void OnFineFitRadio();
	afx_msg void OnSameFitRadio();
	afx_msg void OnSelectSimilar();
	afx_msg void OnStaggeredHeightsCheck();
	afx_msg void OnDblclkStandardsOuter();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkStandardsInner();
	afx_msg void OnClickMatrixSectionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSoleboardInner();
	afx_msg void OnSoleboardOuter();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void GetCommittedStageLevel( Bay *pBay, CornerOfBayEnum eCnr, intArray &iaCommitted, intArray &iaLevels, doubleArray &daRLs, CStringArray &saStages );
	void AdjustStandards();
	void SetViewportByMatrix();
	void FillBaysList();
	bool m_bExpandedView;
	void UpdateArrayFromElement( MatrixElement *pElement );
	void FillAStandardBox( CListBox &EditBox, doubleArray &daArrangement, bool bSoleBoard, CString &sTotal );
	void SelectSimilar( double dStdInner, double dStdOuter, SystemEnum eSystem );
	void SelectSimilar( doubleArray &daStdInner, doubleArray &daStdOuter, bool bSBInner, bool bSBOuter, SystemEnum eSystem );
	bool IsApplyRequired();
	void ApplyRequired( bool bReqd );
	void SetupListControls();
	void UpdateMatrixElementView();
	void FillMatrixListBox();
	void FillStandards();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSTANDARDSDLG_H__7AC49974_F94D_11D3_9F5F_0008C7999B1D__INCLUDED_)
