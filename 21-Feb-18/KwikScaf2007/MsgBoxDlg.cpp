// msgboxdlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "msgboxdlg.h"
#include "afxdialogex.h"


// msgboxdlg dialog

IMPLEMENT_DYNAMIC(msgboxdlg, CDialogEx)

msgboxdlg::msgboxdlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(msgboxdlg::IDD, pParent)
{

}

msgboxdlg::~msgboxdlg()
{
}

void msgboxdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(msgboxdlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &msgboxdlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &msgboxdlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// msgboxdlg message handlers


void msgboxdlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void msgboxdlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


BOOL msgboxdlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_MSG_BOX_EDIT1,msg);
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
