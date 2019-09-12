#pragma once

// CMsgBox dialog
#include "KwikScafDialog.h"

class CMsgBox : public KwikscafDialog
{
	DECLARE_DYNAMIC(CMsgBox)

public:
	CMsgBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgBox();

// Dialog Data
	enum { IDD = IDD_MSG_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
