#if !defined(AFX_COMPONENTPARTNUMBERSELECTORDLG_H__9D737733_8217_11D4_9FD7_0008C7999B1D__INCLUDED_)
#define AFX_COMPONENTPARTNUMBERSELECTORDLG_H__9D737733_8217_11D4_9FD7_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComponentPartNumberSelectorDlg.h : header file
//

#include "KwikscafDialog.h"

/////////////////////////////////////////////////////////////////////////////
// ComponentPartNumberSelectorDlg dialog

class ComponentPartNumberSelectorDlg : public KwikscafDialog
{
// Construction
public:
	void SetAllowNonVisual( bool bAllow );
	void GetPartNumber( CString &sPartNumber );
	void SetPartNumber( CString sPartNumber );
	ComponentPartNumberSelectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ComponentPartNumberSelectorDlg)
	enum { IDD = IDD_SELECT_VISIBLE_PART_NUMBER_DLG };
	CEdit	m_editFilterString;
	CListBox	m_ComponentListBoxCtrl;
	CButton	m_btnFilter;
	CString	m_sFilterString;
	CString	m_sMatchingElements;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ComponentPartNumberSelectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bAllowBOMNonVisual;
	CStringArray m_saPartNumbers;
	CString m_sSelectedPartNumber;

	// Generated message map functions
	//{{AFX_MSG(ComponentPartNumberSelectorDlg)
	afx_msg void OnSetfocusFilterString();
	afx_msg void OnFilter();
	afx_msg void OnDblclkComponentListbox();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool IsAllowNonVisual();
	void FillComponentList(bool bApplyingFilter);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPONENTPARTNUMBERSELECTORDLG_H__9D737733_8217_11D4_9FD7_0008C7999B1D__INCLUDED_)
