// RunPropDialog.cpp : implementation file
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
#include "RunPropDialog.h"
#include "BayDetailsDialog.h"
#include "autobuildtools.h"
#include "Controller.h"
#include "Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MEMFILE_GROW_INCREMENT 10240  // 10K bytes inc.

/////////////////////////////////////////////////////////////////////////////
// RunPropDialog dialog


//////////////////////////////////////////////////////////////////////
//Constructor
//
RunPropDialog::RunPropDialog(CWnd* pParent /*=NULL*/, BayDetailsDialog *pBayDetailsDialog, Run *pRun)
	: KwikscafDialog(RunPropDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(RunPropDialog)
	m_BaysPerTieBayLbl = _T("");
	m_BaysPerBracingBayLbl = _T("");
	m_sTiesVerticallyEvery = _T("");
	m_sDistanceFromWall = _T("");
	m_sRLEnd = _T("");
	m_sRLStart = _T("");
	m_bUseLongerBracing = FALSE;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_nID = RunPropDialog::IDD;
	m_pBayDetailsDialog = pBayDetailsDialog;
	m_pSelectedRun = pRun;
	m_BaysPerTieBayLbl = _T("Bays");
	m_BaysPerBracingBayLbl = _T("Bays");
	m_bIsAutobuild = false;
}


//////////////////////////////////////////////////////////////////////
//DoDataExchange
//
void RunPropDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RunPropDialog)
	DDX_Control(pDX, IDC_BRACE_LENGTH_0700, m_drplstLength0700);
	DDX_Control(pDX, IDC_BRACE_LENGTH_1200, m_drplstLength1200);
	DDX_Control(pDX, IDC_BRACE_LENGTH_1800, m_drplstLength1800);
	DDX_Control(pDX, IDC_BRACE_LENGTH_2400, m_drplstLength2400);
	DDX_Control(pDX, IDC_FIT_GROUP_BOX, m_FitGroupBox);
	DDX_Control(pDX, IDC_OK, m_OKBtn);
	DDX_Control(pDX, IDC_AB_CLOSE_BUTTON, m_CloseBtn);
	DDX_Control(pDX, IDC_SET_AS_DEFAULT, m_SetAsDefaultBtn);
	DDX_Control(pDX, IDC_DISTANCE_FROM_WALL_UINTS_LBL, m_DistanceFromWallUnitsLbl);
	DDX_Control(pDX, IDC_DISTANCE_FROM_WALL_LBL, m_DistanceFromWallLbl);
	DDX_Control(pDX, IDC_BAY_FIT_LBL, m_BayFitLbl);
	DDX_Control(pDX, IDC_DISTANCE_FROM_WALL, m_DistanceFromWallEdit);
	DDX_Control(pDX, IDC_RL_START_SPIN, m_RLStartSpin);
	DDX_Control(pDX, IDC_RL_END_SPIN, m_RLEndSpin);
	DDX_Control(pDX, IDC_DISTANCE_FROM_WALL_SPIN, m_DistanceFromWallSpin);
	DDX_Control(pDX, IDC_AB_TIES_VERTICALLY_EVERY_EDIT, m_TiesVerticallyEveryEditCtrl);
	DDX_Control(pDX, IDC_AB_BRACES_EVERY_COMBO, m_BracesEveryComboCtrl);
	DDX_Control(pDX, IDC_AB_TIES_EVERY_COMBO, m_TiesEveryComboCtrl);
	DDX_Control(pDX, IDC_AB_TIE_TYPE_COMBO, m_TieTypeComboCtrl);
	DDX_Control(pDX, IDC_AB_TIES_VERTICALLY_EVERY_SPIN, m_TiesVerticallyEverySpinCtrl);
	DDX_Control(pDX, IDC_AB_TIGHT_FIT_RADIO, m_TightFitRadioCtrl);
	DDX_Control(pDX, IDC_AB_LOOSE_FIT_RADIO, m_LooseFitRadioCtrl);
	DDX_Text(pDX, IDC_BAYS_PER_TIE_BAY_LBL, m_BaysPerTieBayLbl);
	DDX_Text(pDX, IDC_BAYS_PER_BRACING_BAY_LBL, m_BaysPerBracingBayLbl);
	DDX_Control(pDX, IDC_AB_EDIT_DETAILS_BUTTON, m_EditDetailsCtrl);
	DDX_Text(pDX, IDC_AB_TIES_VERTICALLY_EVERY_EDIT, m_sTiesVerticallyEvery);
	DDX_Text(pDX, IDC_DISTANCE_FROM_WALL, m_sDistanceFromWall);
	DDX_Text(pDX, IDC_RL_END, m_sRLEnd);
	DDX_Text(pDX, IDC_RL_START, m_sRLStart);
	DDX_Check(pDX, IDC_LONGER_BRACING, m_bUseLongerBracing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RunPropDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(RunPropDialog)
	ON_BN_CLICKED(IDC_AB_CLOSE_BUTTON, OnAbCloseButton)
	ON_BN_CLICKED(IDC_AB_EDIT_DETAILS_BUTTON, OnAbEditDetailsButton)
	ON_BN_CLICKED(IDC_AB_LOOSE_FIT_RADIO, OnAbLooseFitRadio)
	ON_BN_CLICKED(IDC_AB_TIGHT_FIT_RADIO, OnAbTightFitRadio)
	ON_CBN_EDITCHANGE(IDC_AB_TIES_EVERY_COMBO, OnEditchangeAbTiesEveryCombo)
	ON_CBN_EDITCHANGE(IDC_AB_TIE_TYPE_COMBO, OnEditchangeAbTieTypeCombo)
	ON_EN_CHANGE(IDC_AB_TIES_VERTICALLY_EVERY_EDIT, OnChangeAbTiesVerticallyEveryEdit)
	ON_CBN_EDITCHANGE(IDC_AB_BRACES_EVERY_COMBO, OnEditchangeAbBracesEveryCombo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AB_TIES_VERTICALLY_EVERY_SPIN, OnDeltaposAbTiesVerticallyEverySpin)
	ON_CBN_SELCHANGE(IDC_AB_BRACES_EVERY_COMBO, OnSelchangeAbBracesEveryCombo)
	ON_CBN_SELENDOK(IDC_AB_BRACES_EVERY_COMBO, OnSelendokAbBracesEveryCombo)
	ON_CBN_SELCHANGE(IDC_AB_TIES_EVERY_COMBO, OnSelchangeAbTiesEveryCombo)
	ON_CBN_SELCHANGE(IDC_AB_TIE_TYPE_COMBO, OnSelchangeAbTieTypeCombo)
	ON_EN_CHANGE(IDC_DISTANCE_FROM_WALL, OnChangeDistanceFromWall)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DISTANCE_FROM_WALL_SPIN, OnDeltaposDistanceFromWallSpin)
	ON_EN_CHANGE(IDC_RL_END, OnChangeRlEnd)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RL_END_SPIN, OnDeltaposRlEndSpin)
	ON_EN_CHANGE(IDC_RL_START, OnChangeRlStart)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RL_START_SPIN, OnDeltaposRlStartSpin)
	ON_BN_CLICKED(IDC_SET_AS_DEFAULT, OnSetAsDefault)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_LOAD_RUN, OnLoadRun)
	ON_WM_MOVE()
	ON_CBN_KILLFOCUS(IDC_AB_TIES_EVERY_COMBO, OnKillfocusAbTiesEveryCombo)
	ON_BN_CLICKED(IDC_LONGER_BRACING, OnLongerBracing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RunPropDialog message handlers

//////////////////////////////////////////////////////////////////////
//Create
//
BOOL RunPropDialog::Create()
{
	return KwikscafDialog::Create(m_nID, m_pParent);
}

//////////////////////////////////////////////////////////////////////
//OnAbCloseButton
//
void RunPropDialog::OnAbCloseButton() 
{
	UpdateData();
	ReadBracingLengths();
	ShowWindow(SW_HIDE);
	EndDialog( IDCANCEL );
}

//////////////////////////////////////////////////////////////////////
//OnAbEditDetailsButton
//
void RunPropDialog::OnAbEditDetailsButton() 
{
	Bay *pBay;
	pBay = m_pSelectedRun->GetBay(0);
	pBay->DisplayBayDetailsDialog(this);
}

//////////////////////////////////////////////////////////////////////
//OnInitDialog
//
BOOL RunPropDialog::OnInitDialog() 
{
	int		iRunID;

	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("RunPropDialog"));

	//Is this an autobuild?
	iRunID = m_pSelectedRun->GetController()->GetRunID( m_pSelectedRun );
	m_bIsAutobuild = ( (iRunID==ID_NONE_MATCHING) || (iRunID==ID_NO_IDS) );


	m_TightFitRadioCtrl.EnableWindow(m_bIsAutobuild);
	m_LooseFitRadioCtrl.EnableWindow(m_bIsAutobuild);
	m_DistanceFromWallEdit.EnableWindow(m_bIsAutobuild);
	m_DistanceFromWallUnitsLbl.EnableWindow(m_bIsAutobuild);
	m_DistanceFromWallLbl.EnableWindow(m_bIsAutobuild);
	m_BayFitLbl.EnableWindow(m_bIsAutobuild);

	if( m_bIsAutobuild )
	{
		//we just want a close button if autobuild
		m_OKBtn.ShowWindow(SW_HIDE);
		m_EditDetailsCtrl.ShowWindow(SW_SHOW);
	}
	else
	{
		//show it but disable it until a change takes place
		m_OKBtn.ShowWindow(SW_SHOW);
		m_OKBtn.EnableWindow(false);
		m_EditDetailsCtrl.ShowWindow(SW_HIDE);
	}

	//set the close button
	m_CloseBtn.SetWindowText( _T("Close") );

	LoadDefaults();
	GetCurrentSettings();

	m_TiesVerticallyEverySpinCtrl.SetRange(0, 10000);

	m_bUseLongerBracing = ConvertboolToBOOL( m_pSelectedRun->GetUseLongerBracing() );

	FillBraceLengths();
	LoadBracingLengths();

	UpdateData( false );

	SetDialogChanged( false );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////
//Load Defaults
//
void RunPropDialog::LoadDefaults()
{
	int i;
	CString sNumber;

	m_TiesEveryComboCtrl.Clear();
	m_TiesEveryComboCtrl.ResetContent();
	for (i=0; i <= TIES_EVERY_MAX_NUMBER_OF_BAYS; ++i)
	{
		sNumber.Format(_T("%d"), i);
		m_TiesEveryComboCtrl.AddString(sNumber);
	}

	m_BracesEveryComboCtrl.Clear();
	m_BracesEveryComboCtrl.ResetContent();
	for (i=0; i <= BRACES_EVERY_MAX_NUMBER_OF_BAYS; ++i)
	{
		sNumber.Format(_T("%d"), i);
		m_BracesEveryComboCtrl.AddString(sNumber);
	}

	m_TieTypeComboCtrl.Clear();
	m_TieTypeComboCtrl.ResetContent();
	for (i=0; i < TIE_TYPES_NUMBER_OF; ++i)
	{
		m_TieTypeComboCtrl.AddString(GetTieTypeDescriptionStr((TieTypesEnum)i )); 
	};
}

//////////////////////////////////////////////////////////////////////
//GetCurrentSettings
//
void RunPropDialog::GetCurrentSettings()
{
	CString sNumber;

	m_sTiesVerticallyEvery.Format( _T("%i"), (int)m_pSelectedRun->GetRunTemplate()->GetTiesVerticallyEvery() );
	m_sRLStart.Format( _T("%i"), (int)m_pSelectedRun->GetBay(0)->GetStandardPosition( CNR_SOUTH_WEST ).z );
	m_sRLEnd.Format( _T("%i"), (int)m_pSelectedRun->GetBay(m_pSelectedRun->GetNumberOfBays()-1)->GetStandardPosition( CNR_SOUTH_EAST ).z );
	m_sDistanceFromWall.Format( _T("%i"), (int)m_pSelectedRun->GetRunTemplate()->GetDistanceFromWall() );

	// Get Ties every
	int BaysPerTie;
	BaysPerTie = m_pSelectedRun->GetRunTemplate()->GetBaysPerTie();
	sNumber.Format( _T("%d"), BaysPerTie );
	m_TiesEveryComboCtrl.SelectString( 0, sNumber );

	// Get Braces every
	int BaysPerBrace;
	BaysPerBrace = m_pSelectedRun->GetBaysPerBrace();
	sNumber.Format( _T("%d"), BaysPerBrace );
	m_BracesEveryComboCtrl.SelectString( 0, sNumber );

	// get tie type
	SetTieType( m_pSelectedRun->GetTieType() );

	//GET AUTOBUILD FIT	
	if ((m_pSelectedRun->GetRunTemplate())->GetFit() == RF_TIGHT)
		m_TightFitRadioCtrl.SetCheck(1);
	else
		m_TightFitRadioCtrl.SetCheck(0);

	UpdateData(false);
	OnSelchangeAbTiesEveryCombo();
	UpdateData(false);
}

//////////////////////////////////////////////////////////////////////
//OnAbLooseFitRadio
//
void RunPropDialog::OnAbLooseFitRadio() 
{
	SetDialogChanged();
	if (m_TightFitRadioCtrl.GetCheck() == 1)
		m_pSelectedRun->SetRunFit(RF_TIGHT);
	else
		m_pSelectedRun->SetRunFit(RF_LOOSE);
}

//////////////////////////////////////////////////////////////////////
//OnAbTightFitRadio
//
void RunPropDialog::OnAbTightFitRadio() 
{
	SetDialogChanged();
	if (m_TightFitRadioCtrl.GetCheck() == 1)
		m_pSelectedRun->SetRunFit(RF_TIGHT);
	else
		m_pSelectedRun->SetRunFit(RF_LOOSE);
}

//////////////////////////////////////////////////////////////////////
//OnEditChangeAbTiesCombo
//
void RunPropDialog::OnEditchangeAbTiesEveryCombo() 
{
	SetDialogChanged();
}

//////////////////////////////////////////////////////////////////////
//OnEditChangeAbTieTypeCombo
//
void RunPropDialog::OnEditchangeAbTieTypeCombo() 
{
	SetDialogChanged();
}

//////////////////////////////////////////////////////////////////////
//OnChangeAbTiesVerticallyEveryEdit
//
void RunPropDialog::OnChangeAbTiesVerticallyEveryEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	double dNumber;

	SetDialogChanged();

	dNumber = _tstof ( m_sTiesVerticallyEvery );
	m_pSelectedRun->SetTiesVerticallyEvery( dNumber );

}

//////////////////////////////////////////////////////////////////////
//OnEditChangeAbBracesEveryCombo
//
void RunPropDialog::OnEditchangeAbBracesEveryCombo() 
{
	SetDialogChanged();
}

//////////////////////////////////////////////////////////////////////
//OnDeltaPosAbTiesVerticallyEverySpin
//
void RunPropDialog::OnDeltaposAbTiesVerticallyEverySpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CString sNumber;

	SetDialogChanged();

	sNumber.Format(_T("%d"), m_TiesVerticallyEverySpinCtrl.GetPos());	
	m_TiesVerticallyEveryEditCtrl.SetWindowText(sNumber);
	*pResult = 0;
}

void RunPropDialog::OnSelchangeAbBracesEveryCombo() 
{
	int iBracesEvery;
	CString sNumber;
	int iCurrentSelection;

	SetDialogChanged();

	iCurrentSelection = m_BracesEveryComboCtrl.GetCurSel();

	m_BaysPerBracingBayLbl = _T("Bays");
	switch(iCurrentSelection)
	{
	case(1):
		m_BaysPerBracingBayLbl = _T("Bay");
		break;
	default:
		m_BracesEveryComboCtrl.GetLBText(iCurrentSelection, sNumber);
		//	m_BracesEveryComboCtrl.GetWindowText(sNumber);
		iBracesEvery = _ttoi(sNumber);
		m_pSelectedRun->SetBaysPerBrace(iBracesEvery);			
		break;
	case(LB_ERR):
		;//assert( false );
		break;
	}
	UpdateData( false );
}

void RunPropDialog::OnSelendokAbBracesEveryCombo() 
{
	SetDialogChanged();

//	OnEditchangeAbBracesEveryCombo();
}

void RunPropDialog::OnSelchangeAbTiesEveryCombo() 
{
	int iTiesEvery;
	CString sNumber;
	int iCurrentSelection;

	SetDialogChanged();

	iCurrentSelection = m_TiesEveryComboCtrl.GetCurSel();

	m_BaysPerTieBayLbl = _T("Bays");

	TieTypesEnum	eType;
	eType = m_pSelectedRun->GetTieType();
	if( eType==TIE_TYPE_BUTTRESS_12 ||
		eType==TIE_TYPE_BUTTRESS_18 ||
		eType==TIE_TYPE_BUTTRESS_24 )
		m_TiesVerticallyEveryEditCtrl.EnableWindow(false);
	else
		m_TiesVerticallyEveryEditCtrl.EnableWindow(true);

	m_TieTypeComboCtrl.EnableWindow(true);
	switch(iCurrentSelection)
	{
	case(0):
		m_TiesVerticallyEveryEditCtrl.EnableWindow(false);
		m_TieTypeComboCtrl.EnableWindow(false);
		break;
	case(1):
		m_BaysPerTieBayLbl = _T("Bay");
		break;
	default:
		break;
	case(LB_ERR):
		;//assert( false );
		break;
	}
	m_TiesEveryComboCtrl.GetLBText(iCurrentSelection, sNumber);
	iTiesEvery = _ttoi(sNumber);
	m_pSelectedRun->SetTiesEvery(iTiesEvery);			
	UpdateData( false );
}

void RunPropDialog::OnSelchangeAbTieTypeCombo() 
{
	int iCurrentSelection;

	SetDialogChanged();

	iCurrentSelection = m_TieTypeComboCtrl.GetCurSel();

	if (iCurrentSelection != LB_ERR)
	{
		TieTypesEnum eType;
		eType = (TieTypesEnum)iCurrentSelection;
		SetTieType( eType );
	}
	else
	{
		;//assert( false );
	}
}

void RunPropDialog::OnChangeDistanceFromWall() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	double dNumber;

	SetDialogChanged();

	dNumber = _tstof ( m_sDistanceFromWall );
	m_pSelectedRun->SetDistanceToWall( dNumber );
}

void RunPropDialog::OnDeltaposDistanceFromWallSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	SetDialogChanged();
	
	*pResult = 0;
}

void RunPropDialog::OnChangeRlEnd() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	double dNumber;

	SetDialogChanged();

	dNumber = _tstof ( m_sRLEnd );
	m_pSelectedRun->GetRunTemplate()->SetRLEnd( dNumber );
}

void RunPropDialog::OnDeltaposRlEndSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	SetDialogChanged();
	
	*pResult = 0;
}

void RunPropDialog::OnChangeRlStart() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	double dNumber;

	SetDialogChanged();

	dNumber = _tstof ( m_sRLStart );
	m_pSelectedRun->GetRunTemplate()->SetRLStart( dNumber );
}

void RunPropDialog::OnDeltaposRlStartSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	SetDialogChanged();

	*pResult = 0;
}

void RunPropDialog::OnSetAsDefault() 
{
	SaveDefaultRun();
}

void RunPropDialog::OnLoadRun() 
{
	LoadDefaultRun();
}

void RunPropDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	KwikscafDialog::OnShowWindow(bShow, nStatus);
	GetCurrentSettings();
	UpdateData( false );
}

void RunPropDialog::SetDialogChanged(bool bChanged)
{
	UpdateData();

	m_bDialogChanged = bChanged;

	//ok is only available if a change has occured
	m_OKBtn.EnableWindow(m_bDialogChanged);
	m_OKBtn.ShowWindow(SW_SHOW);

	if( m_bDialogChanged && !m_bIsAutobuild )
	{
		//we can cancel a change

		m_CloseBtn.SetWindowText( _T("Cancel") );
	}
	else
	{
		//we are in autobuild which we can't cancel
		//	or the dialog has not changed
		m_CloseBtn.SetWindowText( _T("Close") );
		m_OKBtn.ShowWindow(SW_HIDE);
		m_OKBtn.EnableWindow(false);
	}
}

bool RunPropDialog::IsDialogChanged()
{
	return m_bDialogChanged;
}

void RunPropDialog::OnOk() 
{
	UpdateData();
	ReadBracingLengths();
	EndDialog( IDOK );
}


bool RunPropDialog::SaveDefaultRun()
{
	CFile		File;
	CArchive	*pArchive;
	UINT		uiMode;

	uiMode = CFile::modeWrite|CFile::modeCreate|CFile::typeBinary;

	GetFile( File, uiMode, true );

	pArchive	= new CArchive( &File, CArchive::store);

	// ************** Start Serialize here
	m_pSelectedRun->Serialize(*pArchive); 	// Serialize Controller into m_pArchive
	*pArchive << _T("check this out");			// test code 
	// ************** End Serialize here

	pArchive->Close();
	File.Close();

	MessageBox( _T("Run stored as default!"), _T("Storage OK!"), MB_OK );

	DELETE_PTR(pArchive);

	return true;
}

bool RunPropDialog::LoadDefaultRun()
{
	CString		sTemp;
	CFile		File;
	CArchive	*pArchive;
	UINT		uiMode;

	uiMode = CFile::modeRead|CFile::typeBinary;

	GetFile( File, uiMode, false );

	pArchive = new CArchive( &File, CArchive::load, 100000);

	// *********** Start Serialize here
	m_pSelectedRun->DeleteRun();
	m_pSelectedRun->Serialize(*pArchive);      // Serialize Run
	CString testStr;							// test code
	*pArchive >> testStr;						// test code
  acutPrintf( _T("Test string :%s "), testStr );  // test code
	// *********** End serialize here

	pArchive->Close();
	File.Close();

	DELETE_PTR(pArchive);

	GetCurrentSettings();

	return true;
}

bool RunPropDialog::GetFile(CFile &File, UINT uiMode, bool bSave)
{
	CString					sFilename;

	sFilename = _T("DefaultRun");
	sFilename+= EXTENTION_DEFAULT_RUN_FILE;

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox( !bSave, EXTENTION_DEFAULT_RUN_FILE, sFilename,
		OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_DEFAULT_RUN_FILE ); //, acedGetAcadFrame() /*CWnd* pParentWnd = NULL*/ );
	if( bSave )
	{
		dbox.m_ofn.lpstrTitle = TITLE_SAVE_DEFAULT_RUN_FILE;
	}
	else
	{
		dbox.m_ofn.lpstrTitle = TITLE_LOAD_DEFAULT_RUN_FILE;
	}

	gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
	if (dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		gbOpenFileDialogActive = false;
		return false;
	}
	gbOpenFileDialogActive = false; 

	sFilename = dbox.GetPathName();

	//open it this time
	CFileException Error;
	if( !File.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString strFormatted;
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		if( bSave )
			sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;
		else
			sMessage = ERROR_MSG_NOT_OPEN_FILE_READ;

		sMessage+= szCause;
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return false;
	}

	return true;
}

void RunPropDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("RunPropDialog") );
}

void RunPropDialog::OnKillfocusAbTiesEveryCombo() 
{
	UpdateData();

	CString sNumber;
	m_TiesEveryComboCtrl.GetWindowText(sNumber);
	m_TiesEveryComboCtrl.SelectString( 0, sNumber );
	OnSelchangeAbTiesEveryCombo();
}

void RunPropDialog::OnLongerBracing() 
{
	UpdateData();
	m_pSelectedRun->SetUseLongerBracing( ConvertBOOLTobool(m_bUseLongerBracing) );
	SetDialogChanged();
}

void RunPropDialog::SetTieType(TieTypesEnum eType)
{
	CString sTieType;

	sTieType = GetTieTypeDescriptionStr( eType );
	m_TieTypeComboCtrl.SelectString(0, sTieType);

	m_pSelectedRun->SetTieType(eType);
	if( eType==TIE_TYPE_BUTTRESS_12 ||
		eType==TIE_TYPE_BUTTRESS_18 ||
		eType==TIE_TYPE_BUTTRESS_24 )
	{
		m_TiesVerticallyEveryEditCtrl.EnableWindow( FALSE );
		m_TiesVerticallyEverySpinCtrl.EnableWindow( FALSE );
	}
	else
	{
		m_TiesVerticallyEveryEditCtrl.EnableWindow( TRUE );
		m_TiesVerticallyEverySpinCtrl.EnableWindow( TRUE );
	}
}

void RunPropDialog::FillBraceLengths()
{
	int			i;
	double		dLength;
	CString		sLength;
	doubleArray daLengths;

	if( m_pSelectedRun!=NULL )
	{
		////////2400//////////////////////////
		daLengths = m_pSelectedRun->GetAvailBraceLengths( COMPONENT_LENGTH_2400 );
		for( i=0; i<daLengths.GetSize(); i++ )
		{
			dLength = daLengths.GetAt(i);
			sLength.Format( _T("%2.0f"), dLength );
			m_drplstLength2400.AddString( sLength );
			m_drplstLength2400.SetItemData( i, (DWORD)dLength );
		}
		////////1800//////////////////////////
		daLengths = m_pSelectedRun->GetAvailBraceLengths( COMPONENT_LENGTH_1800 );
		for( i=0; i<daLengths.GetSize(); i++ )
		{
			dLength = daLengths.GetAt(i);
			sLength.Format( _T("%2.0f"), dLength );
			m_drplstLength1800.AddString( sLength );
			m_drplstLength1800.SetItemData( i, (DWORD)dLength );
		}
		////////1200//////////////////////////
		daLengths = m_pSelectedRun->GetAvailBraceLengths( COMPONENT_LENGTH_1200 );
		for( i=0; i<daLengths.GetSize(); i++ )
		{
			dLength = daLengths.GetAt(i);
			sLength.Format( _T("%2.0f"), dLength );
			m_drplstLength1200.AddString( sLength );
			m_drplstLength1200.SetItemData( i, (DWORD)dLength );
		}
		////////0700//////////////////////////
		daLengths = m_pSelectedRun->GetAvailBraceLengths( COMPONENT_LENGTH_0700 );
		for( i=0; i<daLengths.GetSize(); i++ )
		{
			dLength = daLengths.GetAt(i);
			sLength.Format( _T("%2.0f"), dLength );
			m_drplstLength0700.AddString( sLength );
			m_drplstLength0700.SetItemData( i, (DWORD)dLength );
		}
	}
}

void RunPropDialog::LoadBracingLengths()
{
	if( m_pSelectedRun!=NULL )
	{
		int		i;
		double	dLength;

		///////////////////2400///////////////////////////////////
		for( i=0; i<m_drplstLength2400.GetCount(); i++ )
		{
			dLength = m_drplstLength2400.GetItemData(i);
			if( dLength>m_dLength2400-ROUND_ERROR &&
				dLength<m_dLength2400+ROUND_ERROR )
			{
				m_drplstLength2400.SetCurSel(i);
				break;
			}
		}

		///////////////////1800///////////////////////////////////
		for( i=0; i<m_drplstLength1800.GetCount(); i++ )
		{
			dLength = m_drplstLength1800.GetItemData(i);
			if( dLength>m_dLength1800-ROUND_ERROR &&
				dLength<m_dLength1800+ROUND_ERROR )
			{
				m_drplstLength1800.SetCurSel(i);
				break;
			}
		}

		///////////////////1200///////////////////////////////////
		for( i=0; i<m_drplstLength1200.GetCount(); i++ )
		{
			dLength = m_drplstLength1200.GetItemData(i);
			if( dLength>m_dLength1200-ROUND_ERROR &&
				dLength<m_dLength1200+ROUND_ERROR )
			{
				m_drplstLength1200.SetCurSel(i);
				break;
			}
		}

		///////////////////0700///////////////////////////////////
		for( i=0; i<m_drplstLength0700.GetCount(); i++ )
		{
			dLength = m_drplstLength0700.GetItemData(i);
			if( dLength>m_dLength0700-ROUND_ERROR &&
				dLength<m_dLength0700+ROUND_ERROR )
			{
				m_drplstLength0700.SetCurSel(i);
				break;
			}
		}
	}
	else
	{
		;//assert( false );
		m_drplstLength2400.SetCurSel(0);
		m_drplstLength1800.SetCurSel(0);
		m_drplstLength1200.SetCurSel(0);
		m_drplstLength0700.SetCurSel(0);
	}
}

void RunPropDialog::ReadBracingLengths()
{
	if( m_pSelectedRun!=NULL )
	{
    // Changes made here by ~SJ~ on 07.03.2008
    // The earlier method of specifying "m_dLength = m_drplst.GetItemData(m_drplst.GetCurSel())" will cause an invalid value in "m_dLength"
    // since Visual Studio 2005 does not allow GetItemData to have a -1 as argument (which will happen if no value is selected in the combo)
    // Hopefully, a 0.0 value if there is no selection in the combo is correct

    int iLength2400 = m_drplstLength2400.GetCurSel();
    int iLength1800 = m_drplstLength1800.GetCurSel();
    int iLength1200 = m_drplstLength1200.GetCurSel();
    int iLength0700 = m_drplstLength0700.GetCurSel();

    m_dLength2400 = ((iLength2400 > -1) ? m_drplstLength2400.GetItemData(iLength2400) : 0.0);
		m_dLength1800 = ((iLength1800 > -1) ? m_drplstLength1800.GetItemData(iLength1800) : 0.0);
		m_dLength1200 = ((iLength1200 > -1) ? m_drplstLength1200.GetItemData(iLength1200) : 0.0);
		m_dLength0700 = ((iLength0700 > -1) ? m_drplstLength0700.GetItemData(iLength0700) : 0.0);
	}
	else
	{
		;//assert( false );
	}
}
