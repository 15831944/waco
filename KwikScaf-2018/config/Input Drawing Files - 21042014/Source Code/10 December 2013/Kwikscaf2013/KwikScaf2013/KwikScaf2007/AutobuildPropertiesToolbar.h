#if !defined(AFX_AUTOBUILDPROPERTIESTOOLBAR_H__1404FB27_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
#define AFX_AUTOBUILDPROPERTIESTOOLBAR_H__1404FB27_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutobuildPropertiesToolbar.h : header file
//

#ifdef _DEBUG
	#undef _DEBUG
	#include "adui.h"
	#include "acui.h"
	#define _DEBUG
#else
	#include "adui.h"
	#include "acui.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesToolbar window

class AutobuildPropertiesToolbar : public CAcUiDockControlBar
{
// Construction
public:
	AutobuildPropertiesToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutobuildPropertiesToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~AutobuildPropertiesToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(AutobuildPropertiesToolbar)
	afx_msg void OnUpdateRunDetailsButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutobuildBayDetailsButton(CCmdUI* pCmdUI);
	afx_msg void OnAutobuildBayDetailsButton();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOBUILDPROPERTIESTOOLBAR_H__1404FB27_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
