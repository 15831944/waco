// HelpAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "HelpAboutDlg.h"
#include "Resource.h"
#include "MeccanoDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HelpAboutDlg dialog


HelpAboutDlg::HelpAboutDlg(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(HelpAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HelpAboutDlg)
	m_sVersionNumber = _T("");
	m_sCopyrightMessage = _T("");
	m_sHardwardLockID = _T("");
	//}}AFX_DATA_INIT

	m_StartTime.GetCurrentTime();

	SetModal(false);
	m_sHardwardLockID = _T("Kiwkscaf Lock not installed correctly!");
	SetShowHardwareKeyMsg(false);
}


void HelpAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HelpAboutDlg)
	DDX_Control(pDX, IDC_PIC, m_Pic);
	DDX_Control(pDX, IDC_AVI, m_Avi);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_VERSION_NUMBER, m_sVersionNumber);
	DDX_Text(pDX, IDC_COPYRIGHT_MESSAGE, m_sCopyrightMessage);
	DDX_Text(pDX, IDC_HARDWARE_LOCK_ID, m_sHardwardLockID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HelpAboutDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(HelpAboutDlg)
	ON_WM_MOVE()
	//ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HelpAboutDlg message handlers

#include "VersionDefinitions.h"

BOOL HelpAboutDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("HelpAboutDlg"));
	
  m_sVersionNumber.Format( _T("Version: %s"), MECCANO_LATEST_VERSION );

	m_sCopyrightMessage = _T("All rights reserved. No part of this work covered by copyright ");
	m_sCopyrightMessage+= _T("may be reproduced or copied in any form or by any means (graphic, ");
	m_sCopyrightMessage+= _T("electronic or mechanical, including photocopying, recording, ");
	m_sCopyrightMessage+= _T("recording taping or information retrieval system) without the ");
	m_sCopyrightMessage+= _T("written permission of Waco Kwikform Limited.");

	m_Avi.ShowWindow( SW_HIDE );
	m_Pic.ShowWindow( SW_HIDE );
	if( m_bModal )
	{
		m_btnOK.ShowWindow( SW_HIDE );
		m_Pic.ShowWindow( SW_SHOW );
	}
	else
	{
		m_Avi.ShowWindow( SW_SHOW );

		CString sAviFilename;

		CFileFind finder;
		if( finder.FindFile(_T("AboutKwikscaf.avi")) )
		{
			sAviFilename = finder.GetFileName();
			//assert( false );
		}
		else
		{
	/*		CWinApp	*pApp;
			pApp = AfxGetApp();
	*/		sAviFilename = GetStringInRegistry( AVI_FILENAME_SECTION );
		}

		if( sAviFilename.IsEmpty() )
		{
			sAviFilename = MOST_LIKELY_PATH;
			sAviFilename+= _T("AboutKwikscaf.avi");
		}

		if( !m_Avi.Open( sAviFilename ) )
		{
			//assert( false );
			m_Avi.ShowWindow( SW_HIDE );
			m_Pic.ShowWindow( SW_SHOW );
		}
		else if( m_Avi.Play(0,-1,-1) )
		{
			//we can play the avi, so store this filename
			SetStringInRegistry( AVI_FILENAME_SECTION, sAviFilename );
		}
		else
		{
			//assert( false );
			m_Avi.ShowWindow( SW_HIDE );
			m_Pic.ShowWindow( SW_SHOW );
		}
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void HelpAboutDlg::SetShowHardwareKeyMsg(bool bShow)
{
	m_bShowHardwareKeyMsg = bShow;
}

bool HelpAboutDlg::ShowHardwareKeyMsg()
{
	return m_bShowHardwareKeyMsg;
}

void HelpAboutDlg::SetHardwareLockID(int iLockID)
{
	if( iLockID>0 )
	{
		m_sHardwardLockID.Format( _T("KwikScaf Lock ID = %i"), iLockID );
	}
	else
	{
		m_sHardwardLockID = _T("Invalid KiwkScaf Lock!");
	}
}

void HelpAboutDlg::SetModal(bool bModal)
{
	m_bModal = bModal;
}

void HelpAboutDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("HelpAboutDlg") );
}

void HelpAboutDlg::OnTimer(UINT nIDEvent) 
{
	if( m_bModal )
	{
		CTime		CurrentTime;
		CTimeSpan	LengthOfTime;
		CurrentTime.GetCurrentTime();

		LengthOfTime =CurrentTime-m_StartTime;
		if( LengthOfTime.GetSeconds() > 6 )
			EndDialog(1);
	}
	
	KwikscafDialog::OnTimer(nIDEvent);
}
