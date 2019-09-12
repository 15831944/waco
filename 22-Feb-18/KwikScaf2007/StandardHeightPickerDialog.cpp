// StandardHeightPickerDialog.cpp : implementation file
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
#include "StandardHeightPickerDialog.h"
#include "BayDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StandardHeightPickerDialog dialog


StandardHeightPickerDialog::StandardHeightPickerDialog(CWnd* pParent /*=NULL*/, CString *psSelection /*NULL*/, double *pdNewStandardLength /*NULL*/, int *piOpenEnded/*=NULL*/ )
	: KwikscafDialog(StandardHeightPickerDialog::IDD, pParent)
{
	if( !_istdigit( (*psSelection)[0] ) )
		psSelection->Format( _T("%.2f"), StandardLengths(STANDARD_LENGTH_3000) );

	m_psSelection = psSelection;
	m_pdNewStandardLength = pdNewStandardLength;
	//{{AFX_DATA_INIT(StandardHeightPickerDialog)
	m_sStandardHeightsCombo = _T("");
	m_iOpenEnded = FALSE;
	//}}AFX_DATA_INIT
	m_sStandardHeightsCombo = *psSelection;
	m_piOpenEnded = piOpenEnded;
}


void StandardHeightPickerDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StandardHeightPickerDialog)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_STANDARD_COMBO_CTRL, m_StandardHeightsComboCtrl);
	DDX_CBString(pDX, IDC_STANDARD_COMBO_CTRL, m_sStandardHeightsCombo);
	DDX_Check(pDX, IDC_OPEN_ENDED, m_iOpenEnded);
	//}}AFX_DATA_MAP

	m_psSelection->Format(_T("%s"), m_sStandardHeightsCombo);
	*m_pdNewStandardLength = _tstof(*m_psSelection);
}


BEGIN_MESSAGE_MAP(StandardHeightPickerDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(StandardHeightPickerDialog)
	ON_CBN_DBLCLK(IDC_STANDARD_COMBO_CTRL, OnDblclkStandardComboCtrl)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_OPEN_ENDED, OnOpenEnded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StandardHeightPickerDialog message handlers

BOOL StandardHeightPickerDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("StandardHeightPickerDialog"));
	
	CString sLength;
	int iIndex;

	m_btnOK.EnableWindow( true );

	iIndex = 0;
	m_StandardHeightsComboCtrl.Clear();
	m_StandardHeightsComboCtrl.ResetContent();
//	m_StandardHeightsComboCtrl.AddString("<Bay Length>");
	while (StandardLengths(iIndex) != 0)
	{
		sLength.Format(_T("%.2f"), StandardLengths(iIndex)); // inline func from meccanodefinitions file
		m_StandardHeightsComboCtrl.AddString(sLength);
		++iIndex;
	}
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void StandardHeightPickerDialog::OnDblclkStandardComboCtrl() 
{
	UpdateData();
	EndDialog(IDOK);
}

void StandardHeightPickerDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("StandardHeightPickerDialog") );
}

void StandardHeightPickerDialog::OnOpenEnded() 
{
	UpdateData();
	*m_piOpenEnded = m_iOpenEnded;
}
