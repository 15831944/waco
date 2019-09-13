#if !defined(AFX_MILLSSYSTEMBAYEDITOR_H__C60BDBC4_2D29_11D4_9F92_0008C7999B1D__INCLUDED_)
#define AFX_MILLSSYSTEMBAYEDITOR_H__C60BDBC4_2D29_11D4_9F92_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MillsSystemBayEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MillsSystemBayEditor dialog
#include "KwikscafDialog.h"

class MillsSystemBayEditor : public KwikscafDialog
{
// Construction
public:
	MillsSystemBayEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MillsSystemBayEditor)
	enum { IDD = IDD_VISUAL_MILLS_TYPE };
	CButton	m_btnStandardSide;
	CButton	m_btnConnection;
	CButton	m_plank9;
	CButton	m_plank8;
	CButton	m_plank7;
	CButton	m_plank6;
	CButton	m_plank5;
	CButton	m_plank4;
	CButton	m_plank3;
	CButton	m_plank2;
	CButton	m_plank10;
	CButton	m_plank1;
	int		m_iStandardSide;
	int		m_iConnectionSide;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MillsSystemBayEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MillsSystemBayEditor)
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MILLSSYSTEMBAYEDITOR_H__C60BDBC4_2D29_11D4_9F92_0008C7999B1D__INCLUDED_)
