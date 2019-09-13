// KwikscafDialog.h: interface for the KwikscafDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KWIKSCAFDIALOG_H__71964A06_E593_11D3_9F45_0008C7999B1D__INCLUDED_)
#define AFX_KWIKSCAFDIALOG_H__71964A06_E593_11D3_9F45_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KwikscafDialog : public CDialog  
{
public:
	BOOL OnInitDialog();
	KwikscafDialog();
	KwikscafDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~KwikscafDialog();

protected:
	bool m_bAllowWinPosSave;
	void GetWindowPositionInReg( CString sWindowName );
	void StoreWindowPositionInReg( CString sWindowName );
};

#endif // !defined(AFX_KWIKSCAFDIALOG_H__71964A06_E593_11D3_9F45_0008C7999B1D__INCLUDED_)
