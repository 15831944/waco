// MsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "MsgBox.h"
#include "afxdialogex.h"


// CMsgBox dialog

IMPLEMENT_DYNAMIC(CMsgBox, CDialogEx)

CMsgBox::CMsgBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMsgBox::IDD, pParent)
{

}

CMsgBox::~CMsgBox()
{
}

void CMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

