// ListPrintSetup.cpp : implementation file
//
//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.

#include "stdafx.h"
#include "PrintManager.h"
#include "ListPrintSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListPrintSetup dialog

	

CListPrintSetup::CListPrintSetup(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(CListPrintSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListPrintSetup)
	//}}AFX_DATA_INIT
}


void CListPrintSetup::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListPrintSetup)
	DDX_Control(pDX, IDOK, wndOk);
	DDX_Control(pDX, IDCANCEL, wndCancel);
	DDX_Control(pDX, IDC_TAB1, wndTab);
	DDX_Control(pDX, IDC_BT_PRINTOPT, wndPrintOpt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListPrintSetup, KwikscafDialog)
	//{{AFX_MSG_MAP(CListPrintSetup)
	ON_BN_CLICKED(IDC_BT_PRINTOPT, OnBtPrintopt)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListPrintSetup message handlers
void CListPrintSetup::OnUpdateDialog()
{
	dlgSheet1.Update();
	dlgSheet2.Update();

}

BOOL CListPrintSetup::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("CListPrintSetup"));
	
	CRect rc, drc;
	wndTab.GetClientRect(rc);

	wndOk.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	wndCancel.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	wndPrintOpt.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	dlgSheet1.Create(CListPrintPage1::IDD, &wndTab);
	dlgSheet1.GetClientRect(drc);
	dlgSheet1.SetWindowPos(0,4,27,drc.Width(),rc.Height()-30,SWP_NOZORDER);
	dlgSheet2.Create(CListPrintPage2::IDD, &wndTab);
	dlgSheet2.SetWindowPos(0,4,27,drc.Width(),rc.Height()-30,SWP_NOZORDER);

	GfxAddTabItem(wndTab, 0, _T("Headers and Fonts"));
	GfxAddTabItem(wndTab, 1, _T("Margins"));

	dlgSheet1.ShowWindow(SW_SHOW);

	GfxSetTabDialog(0, &dlgSheet1);
	GfxSetTabDialog(1, &dlgSheet2);

	dlgSheet1.UpdateData(FALSE);
	dlgSheet2.UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CListPrintSetup::OnBtPrintopt() 
{
	CPrintDialog pd(TRUE);
	AfxGetApp()->DoPrintDialog(&pd);
}

void CListPrintSetup::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	pNMHDR;
	int idx = wndTab.GetCurSel();
	GfxSwitchTabDialog(idx, iNumDialog);
	*pResult = 0;
}

void CListPrintSetup::OnOK() 
{
	OnUpdateDialog();	
	KwikscafDialog::OnOK();
}

void CListPrintSetup::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("CListPrintSetup") );
}
