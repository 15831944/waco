// PrintStatus.cpp : implementation file
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
#include "PrintStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool bGfxUserAbort;

/////////////////////////////////////////////////////////////////////////////
// CPrintStatus dialog

CPrintStatus::CPrintStatus(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(CPrintStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CPrintStatus::IDD, pParent);      // modeless !
	bGfxUserAbort = false;
}


void CPrintStatus::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintStatus)
	DDX_Control(pDX, IDC_PROGRESS1, wndProg);
	DDX_Control(pDX, IDC_ANIMATE1, wndAnima);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintStatus, KwikscafDialog)
	//{{AFX_MSG_MAP(CPrintStatus)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintStatus message handlers

BOOL CPrintStatus::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg("CPrintStatus");
	CenterWindow();
	wndAnima.Open(IDR_PRINT);
	wndAnima.Play(0,(UINT)-1,(UINT)-1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintStatus::OnCancel() 
{
	bGfxUserAbort = true;	
	KwikscafDialog::OnCancel();
}

void CPrintStatus::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( "CPrintStatus" );
}
