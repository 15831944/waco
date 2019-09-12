#if !defined(AFX_KWIKSCAFCOMPONENTEDITOR_H__6CF910E1_E9B2_486E_BABE_F0E5B5814EAE__INCLUDED_)
#define AFX_KWIKSCAFCOMPONENTEDITOR_H__6CF910E1_E9B2_486E_BABE_F0E5B5814EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KwikScafComponentEditor.h : header file
//

#include "KwikscafDialog.h"
#include "StockListElement.h"	// Added by ClassView

//Forward Declaration
class StockListArray;

enum ComponentEditabilityEnum
{
	CE_FULLY_EDITABLE,
	CE_SALE_PRICE_ONLY,
	CE_NOT_EDITABLE
};


/////////////////////////////////////////////////////////////////////////////
// KwikScafComponentEditor dialog

class KwikScafComponentEditor : public KwikscafDialog
{
// Construction
public:
	void OnChange();
	bool AllowExit();
	void SetStockListElement( StockListElement *pElement, bool bSetEditability, bool bShowWarning=true );
	StockListElement * GetCurrentElement();
	void FillDialog();
	KwikScafComponentEditor(StockListArray *pStockArray, CString sPartNumber, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KwikScafComponentEditor)
	enum { IDD = IDD_KWIKSCAF_COMPONENT_EDITOR };
	CButton	m_btnImport;
	CButton	m_btnExport;
	CButton	m_btnUseSalePrice;
	CButton	m_btnClose;
	CButton	m_btnApply;
	CEdit	m_editOtherSale;
	CEdit	m_editOtherHire;
	CEdit	m_editDimY;
	CEdit	m_editDimWeight;
	CStatic	m_editDimLenthCommon;
	CEdit	m_editDimX; 
	CEdit	m_editDimZ;
	CStatic	m_editDetailsType;
	CEdit	m_editDetailsPartNumber;
	CStatic	m_editDetailsMaterail;
	CEdit	m_editDetailsDescription;
	CButton	m_btnDelete;
	CButton	m_btnACF3DBrowse;
	CEdit	m_editACF_3D;
	CButton	m_btnACF2DeoBrowse;
	CButton	m_btnACF2DBrowse;
	CEdit	m_editACF_2DEO;
	CEdit	m_editACF_2D;
	CString	m_sACF2D;
	CString	m_sACF2Deo;
	CString	m_sACF3D;
	CString	m_sDetailsType;
	CString	m_sDetailsDescription;
	CString	m_sDetailsMaterial;
	CString	m_sDetailsPartNumber;
	CString	m_sDimLengthCommon;
	CString	m_sOtherHire;
	CString	m_sOtherSale;
	CString	m_sDimWeight;
	CString	m_sDimZ;
	CString	m_sDimX;
	CString	m_sDimY;
	BOOL	m_bUseSalePrice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KwikScafComponentEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ComponentEditabilityEnum	m_eCE;
	bool m_bEditSalePriceOnly;
	bool m_bNewComponent;
	CString *m_psLastPartNumber;
	StockListElement *m_pCurrentStockElement;
	StockListArray *m_pStockListArray;

	// Generated message map functions
	//{{AFX_MSG(KwikScafComponentEditor)
	afx_msg void OnAcf2dBrowse();
	afx_msg void OnAcf2deoBrowse();
	afx_msg void OnAcf3dBrowse();
	afx_msg void OnDetailsSelectPartNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnNewComponent();
	afx_msg void OnDelete();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeAcf2d();
	afx_msg void OnChangeAcf2deo();
	afx_msg void OnChangeAcf3d();
	afx_msg void OnChangeDetailsDescription();
	afx_msg void OnChangeDetailsPartNumber();
	afx_msg void OnChangeDimHeight();
	afx_msg void OnChangeDimLengthActual();
	afx_msg void OnChangeDimWeight();
	afx_msg void OnChangeDimWidth();
	afx_msg void OnChangeOtherHire();
	afx_msg void OnChangeOtherSale();
	afx_msg void OnApplyChanges();
	afx_msg void OnClose();
	afx_msg void OnOtherUseSalePrice();
	afx_msg void OnStorageExport();
	afx_msg void OnStorageImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowInvalidFileWarning();
	bool DoesFileExist( CString sFilename );
	CString GetFilename();
	bool IsNewComponent();
	void SetNewComponent( bool bNewComonent );
	bool ApplyChanges();
	void EnableACFs( bool bEnable );
	void EnableApplyButton( bool bEnable=true );
	void SetEditability();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KWIKSCAFCOMPONENTEDITOR_H__6CF910E1_E9B2_486E_BABE_F0E5B5814EAE__INCLUDED_)
