#pragma once


// msgboxdlg dialog

class msgboxdlg : public CDialogEx
{
	DECLARE_DYNAMIC(msgboxdlg)

public:
	msgboxdlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~msgboxdlg();
	CString msg;

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	virtual BOOL OnInitDialog();
};
