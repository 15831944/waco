// MillsSystemBayEditor.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "MillsSystemBayEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MillsSystemBayEditor dialog


MillsSystemBayEditor::MillsSystemBayEditor(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(MillsSystemBayEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(MillsSystemBayEditor)
	m_iStandardSide = -1;
	m_iConnectionSide = -1;
	//}}AFX_DATA_INIT
}


void MillsSystemBayEditor::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MillsSystemBayEditor)
	DDX_Control(pDX, IDC_STANDARD_NE, m_btnStandardSide);
	DDX_Control(pDX, IDC_CONNECT_NNE, m_btnConnection);
	DDX_Control(pDX, IDC_PLANK9, m_plank9);
	DDX_Control(pDX, IDC_PLANK8, m_plank8);
	DDX_Control(pDX, IDC_PLANK7, m_plank7);
	DDX_Control(pDX, IDC_PLANK6, m_plank6);
	DDX_Control(pDX, IDC_PLANK5, m_plank5);
	DDX_Control(pDX, IDC_PLANK4, m_plank4);
	DDX_Control(pDX, IDC_PLANK3, m_plank3);
	DDX_Control(pDX, IDC_PLANK2, m_plank2);
	DDX_Control(pDX, IDC_PLANK10, m_plank10);
	DDX_Control(pDX, IDC_PLANK1, m_plank1);
	DDX_Radio(pDX, IDC_STANDARD_NE, m_iStandardSide);
	DDX_Radio(pDX, IDC_CONNECT_NNE, m_iConnectionSide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MillsSystemBayEditor, KwikscafDialog)
	//{{AFX_MSG_MAP(MillsSystemBayEditor)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MillsSystemBayEditor message handlers

void MillsSystemBayEditor::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("MillsSystemBayEditor") );
}

BOOL MillsSystemBayEditor::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("MillsSystemBayEditor"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
