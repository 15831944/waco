#if !defined(AFX_AUTOBUILDPROPERTIESWND_H__1404FB29_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
#define AFX_AUTOBUILDPROPERTIESWND_H__1404FB29_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutobuildPropertiesWnd.h : header file
//

class AutoBuildTools;

/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesWnd window

class AutobuildPropertiesWnd : public CWnd
{
// Construction
public:
	AutobuildPropertiesWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutobuildPropertiesWnd)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetABToolPointer( AutoBuildTools* pABTools );
	virtual ~AutobuildPropertiesWnd();

	// Generated message map functions
protected:
	AutoBuildTools* m_pABTools;
	//{{AFX_MSG(AutobuildPropertiesWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOBUILDPROPERTIESWND_H__1404FB29_0FF3_11D4_9F77_0008C7999B1D__INCLUDED_)
