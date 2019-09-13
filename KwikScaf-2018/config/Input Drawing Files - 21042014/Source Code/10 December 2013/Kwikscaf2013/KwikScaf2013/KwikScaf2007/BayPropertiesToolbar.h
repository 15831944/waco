#if !defined(AFX_BAYPROPERTIESTOOLBAR_H__1404FB26_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
#define AFX_BAYPROPERTIESTOOLBAR_H__1404FB26_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BayPropertiesToolbar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BayPropertiesToolbar window

//class BayPropertiesToolbar : public CToolBarCtrl
class BayPropertiesToolbar : public CToolBar
{
// Construction
public:
	BayPropertiesToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BayPropertiesToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~BayPropertiesToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(BayPropertiesToolbar)
	afx_msg void OnAutobuildBayDetailsButton();
	afx_msg void OnRunDetailsButton();
	afx_msg void OnUpdateAutobuildBayDetailsButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunDetailsButton(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAYPROPERTIESTOOLBAR_H__1404FB26_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
