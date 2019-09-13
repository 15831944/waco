// KwikscafProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "KwikscafProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKwikscafProgressDlg dialog


CKwikscafProgressDlg::CKwikscafProgressDlg(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(CKwikscafProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKwikscafProgressDlg)
	//}}AFX_DATA_INIT
	m_bInitialized = false;
	m_sProgressTitleTemp.Empty();
}


void CKwikscafProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKwikscafProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS_CTRL, m_ctrlProgressBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKwikscafProgressDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(CKwikscafProgressDlg)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKwikscafProgressDlg message handlers

BOOL CKwikscafProgressDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("CKwikscafProgressDlg"));

	m_bInitialized = true;
	m_ctrlProgressBar.SetRange( 0, 100 );
	m_ctrlProgressBar.SetPos( 0 );

	if( !m_sProgressTitleTemp.IsEmpty() )
	{
		SetWindowText(m_sProgressTitleTemp);
	}

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKwikscafProgressDlg::SetPercent( int iPercent )
{
	m_ctrlProgressBar.SetPos( iPercent );
}

void CKwikscafProgressDlg::SetText(CString sText)
{

	if( m_bInitialized )
	{
		SetWindowText( sText );
		UpdateData(false);
	}
	else
	{
		m_sProgressTitleTemp = sText;
	}
}

void CKwikscafProgressDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("CKwikscafProgressDlg") );
	
}
