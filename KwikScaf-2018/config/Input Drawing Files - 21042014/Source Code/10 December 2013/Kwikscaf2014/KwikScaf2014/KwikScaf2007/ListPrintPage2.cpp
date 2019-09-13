// ListPrintPage2.cpp : implementation file
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
#include "ListPrintPage2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListPrintPage2 dialog


CListPrintPage2::CListPrintPage2(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(CListPrintPage2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListPrintPage2)
	iDown = 0;
	iDx = 0;
	iSx = 0;
	iUp = 0;
	//}}AFX_DATA_INIT
}


void CListPrintPage2::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListPrintPage2)
	DDX_Text(pDX, IDC_ED_MMDOWN, iDown);
	DDV_MinMaxInt(pDX, iDown, 0, 300);
	DDX_Text(pDX, IDC_ED_MMDX, iDx);
	DDV_MinMaxInt(pDX, iDx, 0, 300);
	DDX_Text(pDX, IDC_ED_MMSX, iSx);
	DDV_MinMaxInt(pDX, iSx, 0, 300);
	DDX_Text(pDX, IDC_ED_MMUP, iUp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListPrintPage2, KwikscafDialog)
	//{{AFX_MSG_MAP(CListPrintPage2)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListPrintPage2 message handlers

void CListPrintPage2::Update()
{
	UpdateData();
}

void CListPrintPage2::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("CListPrintPage2") );
	
}

BOOL CListPrintPage2::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("CListPrintPage2"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
