// AutobuildDialog.cpp : implementation file
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

//This file & class was originally called BayPropertiesDialog

#include "stdafx.h"
#include "meccano.h"
#include "AutobuildDialog.h"
#include "RunPropDialog.h"
#include "LiftDetailsDialog.h"
#include "BayDetailsDialog.h"
#include "Controller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString BAY_LENGTH_TEXT = _T("<Bay Length>");
const CString BAY_WIDTH_TEXT = _T("<Bay Width>");

/////////////////////////////////////////////////////////////////////////////
// AutobuildDialog dialog


//////////////////////////////////////////////////////////////////////
//Constructor
//
AutobuildDialog::AutobuildDialog(CWnd* pParent /*=NULL*/, AutoBuildTools *pAutobuildTools /*=NULL*/ )
	: KwikscafDialog(AutobuildDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AutobuildDialog)
	m_sBayLength = _T("");
	m_sBayWidth = _T("");
	m_sDeck = _T("");
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_nID = AutobuildDialog::IDD;
	m_pAutobuildTools = pAutobuildTools;
	m_pBayDetailsDialog = NULL;
	m_hInst = NULL;
}

//////////////////////////////////////////////////////////////////////
// Destructor
//
AutobuildDialog::~AutobuildDialog()
{
	Bay *pBay;

	pBay = m_pAutobuildTools->GetBay(0);
	if (pBay)
		pBay->DeleteBayDetails();
}

//////////////////////////////////////////////////////////////////////
//DoDataExchnage
//
void AutobuildDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AutobuildDialog)
	DDX_Control(pDX, IDC_DECK_EDIT, m_sDeckCtrl);
	DDX_Control(pDX, IDC_BAY_WIDTH_COMBO, m_sBayWidthCtrl);
	DDX_Control(pDX, IDC_BAY_LENGTH_COMBO, m_sBayLengthCtrl);
	DDX_Control(pDX, IDC_DECK_SPIN, m_DeckSpinCtrl);
	DDX_CBString(pDX, IDC_BAY_LENGTH_COMBO, m_sBayLength);
	DDX_CBString(pDX, IDC_BAY_WIDTH_COMBO, m_sBayWidth);
	DDX_Text(pDX, IDC_DECK_EDIT, m_sDeck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AutobuildDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(AutobuildDialog)
	ON_BN_CLICKED(IDC_ADVANCED_BUTTON, OnAdvancedButton)
	ON_BN_CLICKED(IDC_EDIT_DETAILS_BUTTON, OnEditDetailsButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DECK_SPIN, OnDeltaposDeckSpin)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_BAY_LENGTH_COMBO, OnSelchangeBayLengthCombo)
	ON_CBN_SELCHANGE(IDC_BAY_WIDTH_COMBO, OnSelchangeBayWidthCombo)
	ON_WM_SHOWWINDOW()
	ON_WM_MOVE()
	//ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AutobuildDialog message handlers

//////////////////////////////////////////////////////////////////////
//OnAdvancedButton
//
void AutobuildDialog::OnAdvancedButton() 
{ 
	RunPropDialog RunPropDlg(this, m_pBayDetailsDialog, m_pAutobuildTools);

	RunPropDlg.m_dLength2400 = m_pAutobuildTools->GetBracingLength( COMPONENT_LENGTH_2400 );
	RunPropDlg.m_dLength1800 = m_pAutobuildTools->GetBracingLength( COMPONENT_LENGTH_1800 );
	RunPropDlg.m_dLength1200 = m_pAutobuildTools->GetBracingLength( COMPONENT_LENGTH_1200 );
	RunPropDlg.m_dLength0700 = m_pAutobuildTools->GetBracingLength( COMPONENT_LENGTH_0700 );
	RunPropDlg.DoModal();
	m_pAutobuildTools->SetBracingLength( COMPONENT_LENGTH_2400, RunPropDlg.m_dLength2400 );
	m_pAutobuildTools->SetBracingLength( COMPONENT_LENGTH_1800, RunPropDlg.m_dLength1800 );
	m_pAutobuildTools->SetBracingLength( COMPONENT_LENGTH_1200, RunPropDlg.m_dLength1200 );
	m_pAutobuildTools->SetBracingLength( COMPONENT_LENGTH_0700, RunPropDlg.m_dLength0700 );

}

//////////////////////////////////////////////////////////////////////
//OnEditDetailsButton
//
void AutobuildDialog::OnEditDetailsButton() 
{
	Bay *pBay;

	pBay = m_pAutobuildTools->GetBay(0);
	pBay->DisplayBayDetailsDialog(this);

	m_pAutobuildTools->SetAccessEnabled( true );
/*
	MSG Msg;
	Msg.message=WM_MOUSEMOVE;
	m_pAutobuildTools->RefreshScreen();
//	m_pAutobuildTools->SetUseLastPoints();
	m_pAutobuildTools->SetMouseTrack( true );
	m_pAutobuildTools->SetAccessEnabled( true );
	m_pAutobuildTools->filterMouse( &Msg );
*/
}


//////////////////////////////////////////////////////////////////////
//Create
//
BOOL AutobuildDialog::Create()
{
	return KwikscafDialog::Create(m_nID, m_pParent);
	//return KwikscafDialog::Create(m_nID, NULL);
}


//////////////////////////////////////////////////////////////////////
//OnInitDialog
//
BOOL AutobuildDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("AutobuildDialog"));

	LoadDefaults();
	SetCurrentSettings();	

	m_sBayLengthCtrl.GetLBText(m_sBayLengthCtrl.GetCurSel(), m_sOriginalLength );
	m_sBayWidthCtrl.GetLBText(m_sBayWidthCtrl.GetCurSel(), m_sOriginalWidth );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////
//LoadDefaults
//
void AutobuildDialog::LoadDefaults()
{
	CString sLength;
	CString sVersion;

	sVersion = _T("Autobuild Properties");

	KwikscafDialog::SetWindowText(sVersion);

	int index;
	index = 0;
	m_sBayLengthCtrl.AddString(BAY_LENGTH_TEXT);
	while (BayLengths(index) != 0)
	{
		if( BayLengths(index)<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			BayLengths(index)>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength = _T("800.00");
		else
			sLength.Format(_T("%.2f"), BayLengths(index)); // inline func from meccanodefinitions file
		m_sBayLengthCtrl.AddString(sLength);
		++index;
	}
	index = 0;
	m_sBayWidthCtrl.AddString(BAY_WIDTH_TEXT);
	while (BayWidths(index) != 0)
	{
		if( BayLengths(index)<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			BayLengths(index)>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength = _T("800.00");
		else
			sLength.Format(_T("%.2f"), BayLengths(index));// inline func from meccanodefinitions file
		m_sBayWidthCtrl.AddString(sLength);
		++index;
	}

	m_DeckSpinCtrl.SetRange( 0, m_pAutobuildTools->GetNumberOfBays());
	m_DeckSpinCtrl.SetPos( 0 );
	m_sDeckCtrl.SetWindowText(_T("0"));
}

//////////////////////////////////////////////////////////////////////
//SetCurrentSettings
//
void AutobuildDialog::SetCurrentSettings()
{
	int		iID;
	CString sDeck;

	iID = 0;
	m_sDeckCtrl.GetWindowText( sDeck );

	iID = _ttoi(sDeck);

	if (iID < m_pAutobuildTools->GetNumberOfBays())
	{
		CString sLength;
		Bay *pBay;

		pBay = m_pAutobuildTools->GetBay( iID );
		sLength.Format(_T("%.2f"), pBay->GetBayLength());
		if( pBay->GetBayLength()<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			pBay->GetBayLength()>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength = _T("800.00");
		m_sBayLengthCtrl.SelectString( 0, sLength );
		sLength.Format(_T("%.2f"), pBay->GetBayWidth());
		if( pBay->GetBayWidth()<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			pBay->GetBayWidth()>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength = _T("800.00");
		m_sBayWidthCtrl.SelectString( 0, sLength );
	}
}

//////////////////////////////////////////////////////////////////////
//OnDeltaPosDeckSpin
//
void AutobuildDialog::OnDeltaposDeckSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int		iID;
	CString sDeck;

	iID = m_DeckSpinCtrl.GetPos();

	sDeck.Format(_T("%d"), iID);
	m_sDeckCtrl.SetWindowText( sDeck );

	SetCurrentSettings();
	
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////
//OnDestroy
//
void AutobuildDialog::OnDestroy() 
{
	KwikscafDialog::OnDestroy();
}

//////////////////////////////////////////////////////////////////////
//OnSelChangeBayLengthCombo
//
void AutobuildDialog::OnSelchangeBayLengthCombo() 
{
	double dNewBayLength;
	CString sNewBayLength;

	m_sBayLengthCtrl.GetLBText(m_sBayLengthCtrl.GetCurSel(), sNewBayLength);
	if( sNewBayLength==BAY_LENGTH_TEXT )
	{
		sNewBayLength = m_sOriginalLength;
		m_sBayLengthCtrl.SelectString( 0, m_sOriginalLength );
	}

	dNewBayLength = _tstof(sNewBayLength);
	if( dNewBayLength<=COMPONENT_LENGTH_0800+ROUND_ERROR &&
		dNewBayLength>=COMPONENT_LENGTH_0800-ROUND_ERROR)
		dNewBayLength = COMPONENT_LENGTH_0700;
	m_pAutobuildTools->SetBayLength(dNewBayLength);
}

//////////////////////////////////////////////////////////////////////
//OnSelChangeBayWidthCombo
//
void AutobuildDialog::OnSelchangeBayWidthCombo() 
{
	double dNewBayWidth;
	CString sNewBayWidth;

	m_sBayWidthCtrl.GetLBText(m_sBayWidthCtrl.GetCurSel(), sNewBayWidth);
	if( sNewBayWidth==BAY_WIDTH_TEXT )
	{
		sNewBayWidth = m_sOriginalWidth;
		m_sBayWidthCtrl.SelectString( 0, m_sOriginalWidth );
	}

	dNewBayWidth = _tstof(sNewBayWidth);
	if( dNewBayWidth<=COMPONENT_LENGTH_0800+ROUND_ERROR &&
		dNewBayWidth>=COMPONENT_LENGTH_0800-ROUND_ERROR )
		dNewBayWidth = COMPONENT_LENGTH_0700;
	m_pAutobuildTools->SetBayWidth(dNewBayWidth);
}


void AutobuildDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	KwikscafDialog::OnShowWindow(bShow, nStatus);
}

//DEL void AutobuildDialog::ShowOKButton(bool bShow)
//DEL {
//DEL 	if( bShow )
//DEL 		m_OkButtonCtrl.ShowWindow(SW_SHOW);
//DEL 	else
//DEL 		m_OkButtonCtrl.ShowWindow(SW_SHOW);
//DEL }

void AutobuildDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("AutobuildDialog") );
}

//////////////////////////////////////////////////////////////////////
//onAcadKeepFocus
//
afx_msg LONG AutobuildDialog::onAcadKeepFocus( UINT, LONG ) 
{
	m_pAutobuildTools->SetAccessEnabled( true );
	return FALSE;
}
