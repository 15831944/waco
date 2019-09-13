#if !defined(AFX_KWIKSCAFBUTTON_H__135B9FF7_7DF8_11D4_9FD4_0008C7999B1D__INCLUDED_)
#define AFX_KWIKSCAFBUTTON_H__135B9FF7_7DF8_11D4_9FD4_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KwikScafButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKwikScafButton window

class CKwikScafButton : public CButton
{
// Construction
public:
	CKwikScafButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKwikScafButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetRGBColour();
	void GetRGBColour( int &iRed, int &iGreen, int &iBlue );
	int GetAutoCADColour();
	void SetRGBColour( int iRed, int iGreen, int iBlue );
	void SetRGBColour( int iColour );
	void SetAutoCADColour( int iColour );
	virtual ~CKwikScafButton();

	// Generated message map functions
protected:
	int m_iColour;
	//{{AFX_MSG(CKwikScafButton)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KWIKSCAFBUTTON_H__135B9FF7_7DF8_11D4_9FD4_0008C7999B1D__INCLUDED_)
