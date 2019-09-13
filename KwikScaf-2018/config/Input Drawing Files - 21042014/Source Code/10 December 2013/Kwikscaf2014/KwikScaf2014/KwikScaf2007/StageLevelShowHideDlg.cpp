// StageLevelShowHideDlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "StageLevelShowHideDlg.h"
#include "controller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StageLevelShowHideDlg dialog
const CString VISIBLE_TEXT = _T(" ");
const CString HIDDEN_TEXT = _T("x");

extern Controller *gpController;

enum
{
	COLUMN_STAGE,
	COLUMN_LEVEL,
	COLUMN_VISIBLE
};

StageLevelShowHideDlg::StageLevelShowHideDlg( CWnd* pParent/*=NULL*/ )
	: KwikscafDialog(StageLevelShowHideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(StageLevelShowHideDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	assert( gpController!=NULL );
}


void StageLevelShowHideDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StageLevelShowHideDlg)
	DDX_Control(pDX, IDC_STAGE_LEVEL_LIST, m_lstStageLevel);
	DDX_Control(pDX, IDC_SHOW, m_btnShow);
	DDX_Control(pDX, IDC_HIDE, m_btnHide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StageLevelShowHideDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(StageLevelShowHideDlg)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	ON_BN_CLICKED(IDC_SHOW, OnShow)
	ON_WM_MOVE()
	ON_NOTIFY(NM_DBLCLK, IDC_STAGE_LEVEL_LIST, OnDblclkStageLevelList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StageLevelShowHideDlg message handlers

BOOL StageLevelShowHideDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("STAGE_LEVEL_SHOW_HIDE"));

	assert( gpController!=NULL );

	m_lstStageLevel.SetExtendedStyle( LVS_EX_TRACKSELECT );
	gpController->CopyStageLevelVisible( m_StageLevelVisible );

	SetUpColumns();
	FillStageLevelList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void StageLevelShowHideDlg::OnHide() 
{
	HideShowSelection( false );
}

void StageLevelShowHideDlg::OnShow() 
{
	HideShowSelection( true );
}

void StageLevelShowHideDlg::OnCancel() 
{
	KwikscafDialog::OnCancel();
}

void StageLevelShowHideDlg::OnOK() 
{
	gpController->SetStageLevelVisible( m_StageLevelVisible );

	KwikscafDialog::OnOK();
}


void StageLevelShowHideDlg::SetUpColumns()
{
	const int	iWidth = 261;
	m_lstStageLevel.InsertColumn( COLUMN_STAGE,		_T("Stage"),	LVCFMT_LEFT,	(int)(0.6*(double)iWidth) );
	m_lstStageLevel.InsertColumn( COLUMN_LEVEL,		_T("Level"),	LVCFMT_RIGHT,	(int)(0.3*(double)iWidth) );
	m_lstStageLevel.InsertColumn( COLUMN_VISIBLE,	_T("Hidden"),	LVCFMT_RIGHT,	(int)(0.2*(double)iWidth) );
}

void StageLevelShowHideDlg::FillStageLevelList()
{
	int			i, iLevel, iIndex;
	CString		sStage, sLevel, sVisible;
	LevelList	*pLL;
	bool		bVisible;

	assert( gpController!=NULL );
	pLL = gpController->GetLevelList();

	m_lstStageLevel.DeleteAllItems();
	iIndex = 0;
	COLORREF cr;
	for( i=0; i<m_StageLevelVisible.GetNumberOfStages(); i++ )
	{
		sStage = m_StageLevelVisible.GetStage(i);

		cr = 0x00ffffff;
		for( iLevel=0; iLevel<pLL->GetSize()+1; iLevel++ )
		{
			sLevel.Format( _T("%i"), iLevel+1 );

			bVisible = m_StageLevelVisible.IsVisible( sStage, iLevel+1 );
			sVisible = ( bVisible )? VISIBLE_TEXT: HIDDEN_TEXT;

			m_lstStageLevel.InsertItem( iIndex, sStage );
	  		m_lstStageLevel.SetItemText( iIndex, COLUMN_LEVEL, sLevel );
	  		m_lstStageLevel.SetItemText( iIndex, COLUMN_VISIBLE, sVisible );
			m_lstStageLevel.SetBkColor( cr );

			iIndex++;
		}
	}
}

void StageLevelShowHideDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("STAGE_LEVEL_SHOW_HIDE") );
}

void StageLevelShowHideDlg::HideShowSelection(bool bShow)
{
	POSITION	pos = m_lstStageLevel.GetFirstSelectedItemPosition();

	if( pos!=NULL )
	{
		int			nItem, iLevel;
		CString		sStage, sLevel;
		while (pos)
		{
			nItem	= m_lstStageLevel.GetNextSelectedItem(pos);
			sStage	= m_lstStageLevel.GetItemText( nItem, COLUMN_STAGE );
			sLevel	= m_lstStageLevel.GetItemText( nItem, COLUMN_LEVEL );
			iLevel	= _ttoi(sLevel);
			
			m_StageLevelVisible.SetVisible( sStage, iLevel, bShow );
		}
		FillStageLevelList();
	}
	else
	{
		//Invalid Selections
		CString sMsg;
		sMsg = _T("Please select the stages and levels that you wish to ");
		if( bShow )
			sMsg+= _T("be displayed");
		else
			sMsg+= _T("be hidden");

		MessageBox( sMsg, _T("Selection Error"), MB_OK );
	}
}

void StageLevelShowHideDlg::OnDblclkStageLevelList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int			nItem, iLevel;
	CString		sStage, sLevel, sHidden;

	nItem	= m_lstStageLevel.GetHotItem();
	if( nItem>=0 && nItem<m_lstStageLevel.GetItemCount() )
	{
		sStage	= m_lstStageLevel.GetItemText( nItem, COLUMN_STAGE );
		sLevel	= m_lstStageLevel.GetItemText( nItem, COLUMN_LEVEL );
		iLevel	= _ttoi(sLevel);
		sHidden	= m_lstStageLevel.GetItemText( nItem, COLUMN_VISIBLE );

		if( sHidden==HIDDEN_TEXT )
			m_StageLevelVisible.SetVisible( sStage, iLevel, true );
		else
			m_StageLevelVisible.SetVisible( sStage, iLevel, false );

		FillStageLevelList();

		m_lstStageLevel.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
	}
	
	*pResult = 0;
}
