// AssignStageDialog.cpp : implementation file
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
#include "meccano.h"
#include "AssignStageDialog.h"
#include "Controller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AssignStageDialog dialog

 
AssignStageDialog::AssignStageDialog(CWnd* pParent, CString *pStageNameToAssign, Controller *pController)
	: KwikscafDialog(AssignStageDialog::IDD, pParent)
{ 
	m_pStageNameToAssign = pStageNameToAssign;
	m_pController = pController;
	//{{AFX_DATA_INIT(AssignStageDialog)
	m_sAssignStageNameCombo = _T("");
	//}}AFX_DATA_INIT
	m_sAssignStageNameCombo = *m_pStageNameToAssign;
}
 
 
void AssignStageDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AssignStageDialog)
	DDX_Control(pDX, IDC_ASSIGN_STAGENAME_COMBO, m_AssignStageNameComboCtrl);
	DDX_CBString(pDX, IDC_ASSIGN_STAGENAME_COMBO, m_sAssignStageNameCombo);
	//}}AFX_DATA_MAP
	m_pStageNameToAssign->Format(_T("%s"), m_sAssignStageNameCombo);
}


BEGIN_MESSAGE_MAP(AssignStageDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(AssignStageDialog)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL AssignStageDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("AssignStageDialog"));

	int iNumberOfExistingStages;
	int iStageNameIndex;

	iNumberOfExistingStages = m_pController->GetNumberOfStages();
	for (iStageNameIndex = 0; iStageNameIndex < iNumberOfExistingStages; iStageNameIndex++)
	{
		m_AssignStageNameComboCtrl.AddString(m_pController->GetStage(iStageNameIndex));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void AssignStageDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("AssignStageDialog") );
}
