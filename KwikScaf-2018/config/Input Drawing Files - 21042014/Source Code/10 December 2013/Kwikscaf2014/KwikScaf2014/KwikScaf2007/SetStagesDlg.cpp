// SetStagesDlg.cpp : implementation file
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
#include "SetStagesDlg.h"
#include "Controller.h"
#include "AssignStageDialog.h"
#include "LevelList.h"
#include "StageLevelShowHideDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum ColumnsEnum
{
	COLUMN_BAY_NUMBER,
	COLUMN_COMMITTED,
	COLUMN_LEVEL,
	COLUMN_STAGE,
	COLUMN_HIDDEN
};

const int INVALID_LEVEL = -1;
const CString sCommitText = _T("C");
const CString sVisibleText = _T(" ");
const CString sNotVisibleText = _T("x");

/////////////////////////////////////////////////////////////////////////////
// SetStagesDlg dialog

SetStagesDlg::SetStagesDlg( CWnd* pParent /*=NULL*/)
	: KwikscafDialog(SetStagesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetStagesDlg)
	m_bColour3DByStageLevel = FALSE;
	//}}AFX_DATA_INIT
}


void SetStagesDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetStagesDlg)
	DDX_Control(pDX, IDC_SETSTAGES_UPPER_LEVEL_COMBO, m_UpperLevelCombo);
	DDX_Control(pDX, IDC_SETSTAGES_LOWER_LEVEL_COMBO, m_LowerLevelCombo);
	DDX_Control(pDX, IDC_SETSTAGES_LISTBOX, m_ListBoxCtrl);
	DDX_Check(pDX, IDC_COLOUR_BY_STAGE_LEVEL, m_bColour3DByStageLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetStagesDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(SetStagesDlg)
	ON_BN_CLICKED(IDC_SETSTAGES_ASSIGN_STAGE_BUTTON, OnSetstagesAssignStageButton)
	ON_CBN_SELCHANGE(IDC_SETSTAGES_LOWER_LEVEL_COMBO, OnSelchangeLowerLevelCombo)
	ON_BN_CLICKED(IDC_SETSTAGES_SELECT_BAYS_BUTTON, OnSelectBaysButton)
	ON_CBN_SELCHANGE(IDC_SETSTAGES_UPPER_LEVEL_COMBO, OnSelchangeUpperLevelCombo)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SHOW_HIDE, OnShowHide)
	ON_NOTIFY(NM_DBLCLK, IDC_SETSTAGES_LISTBOX, OnDblclkSetstagesListbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetStagesDlg message handlers

void SetStagesDlg::OnSetstagesAssignStageButton() 
{
	int					iIndex;
	CString				sCommit, sStageName, sBay, sLvl;
	POSITION			pos;
	CStringArray		saCommittedBayLevels;
	AssignStageDialog	*pAssignStageDialog;

	//////////////////////////////////////////////////////
	//Does the current selection contain any committed stages?
	pos = m_ListBoxCtrl.GetFirstSelectedItemPosition();
	saCommittedBayLevels.RemoveAll();
	if (pos != NULL)
	{
		while (pos) // go through each selection
		{
			iIndex = m_ListBoxCtrl.GetNextSelectedItem(pos);
			sCommit = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_COMMITTED );
			if( sCommit==sCommitText )
			{
				sBay = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_BAY_NUMBER );
				sLvl = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_LEVEL );
				saCommittedBayLevels.Add( sBay );
				saCommittedBayLevels.Add( sLvl );
				m_ListBoxCtrl.SetItemState( iIndex, 0, LVIS_SELECTED );
			}
		}
	}
	if( saCommittedBayLevels.GetSize()>0 )
	{
		CString sMsg;
    sMsg = _T("Error: The following Bays and Levels have already been\n");
		sMsg+= _T("committed and cannot be re-assigned a new stage name!\n");
		sMsg+= _T("If this was committed in error, then you can remove all\n");
		sMsg+= _T("commit flags by using the command 'ClearCommitted'.\n\n");
		for( int i=0; i<saCommittedBayLevels.GetSize(); i+=2 )
		{
      sMsg+= _T("Bay: ");
			sMsg+= saCommittedBayLevels.GetAt(i);
      sMsg+= _T(", Level: .");
			sMsg+= saCommittedBayLevels.GetAt(i+1);
			sMsg+= _T("\n");
		}

		int iRet = MessageBox( sMsg, _T("Selection constains committed"), MB_OKCANCEL|MB_ICONSTOP );
		if( iRet!=IDOK )
		{
			return;
		}
	}
	
	pAssignStageDialog = new AssignStageDialog( this, &sStageName, m_pController );
	if( pAssignStageDialog->DoModal( ) == IDOK )
	{
		AssignStageToSelections( sStageName );
	}
	delete( pAssignStageDialog );
	pAssignStageDialog = NULL;
}

void SetStagesDlg::OnSelectBaysButton() 
{
	EndDialog( IDRETRY );
}

void SetStagesDlg::OnSelchangeLowerLevelCombo() 
{
	OnSelchangeSetLevelCombo( false );
}

void SetStagesDlg::OnSelchangeUpperLevelCombo() 
{
	OnSelchangeSetLevelCombo( true );
}

void SetStagesDlg::OnSelchangeSetLevelCombo(bool bUpper)
{
	int iUpperLevel, iLowerLevel;

	iLowerLevel = m_LowerLevelCombo.GetCurSel();
	iUpperLevel = m_UpperLevelCombo.GetCurSel();

	if( iLowerLevel>iUpperLevel )
	{
		if( bUpper )
			m_LowerLevelCombo.SetCurSel( iUpperLevel );
		else
			m_UpperLevelCombo.SetCurSel( iLowerLevel );
	}

	UpdateTableSelectionFromBayList();
	m_ListBoxCtrl.SetFocus();
}

void SetStagesDlg::OnCancel() 
{
	int		i, iSize;
	bool	bFound;
	CString	sBayNumber, sLevel, sStage;

	UpdateData();

	///////////////////////////////////////////////////////////
	//do we have any unassigned?
	iSize = m_ListBoxCtrl.GetItemCount();
	bFound = false;
	for( i=0; i<iSize; i++ )
	{
		sStage = m_ListBoxCtrl.GetItemText( i, COLUMN_STAGE );

		if( sStage==STAGE_DEFAULT_VALUE )
		{
			bFound = true;
			break;
		}
	}

	///////////////////////////////////////////////////////////
	//Better warn them!
	if( bFound )
	{
		CString sMsg;
    sMsg = _T("Warning: Some Stages and levels are currently unassigned!\n\n");
		sMsg+= _T("Would you like me to highlight the unassigned stages?");
		int iRet = MessageBox( sMsg, _T("Unassigned Stages"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2 );
		if( iRet==IDYES )
		{
			SelectUnassigned();
			m_ListBoxCtrl.SetFocus();
			return;
		}
	}
	
	KwikscafDialog::OnCancel();
}

BOOL SetStagesDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("SetStagesDlg"));

	int iMinLevel, iMaxLevel;

	m_ListBoxCtrl.SetExtendedStyle( LVS_EX_TRACKSELECT );

	//setup the stage level box
	SetColumnHeadings( );
	FillStageLevelBox( );

	//set the level combos from the selection
	GetRangeOfLevelsForSelectedBays( iMinLevel, iMaxLevel );
	FillLevelCtrls( iMinLevel, iMaxLevel );
	m_LowerLevelCombo.SetCurSel(0);
	m_UpperLevelCombo.SetCurSel(m_LowerLevelCombo.GetCount()-1);
	
	//find the selected bays from the list
	UpdateTableSelectionFromBayList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SetStagesDlg::SetController(Controller *pController)
{
	m_pController = pController;
	m_Levels = m_pController->GetLevelList( );
}

void SetStagesDlg::SetBayList(BayList *pBays)
{
	m_pBays = pBays;
}

BayStageLevelArray *SetStagesDlg::GetBayStageLevelArray()
{
	return m_pController->GetBayStageLevelArray();
}

void SetStagesDlg::FillStageLevelBox()
{
	int				i, iBayNumber, iIndex, iRun, iBay,
					iMinLevel, iMaxLevel, iListIndex, iSize,
					iProg, iProgTotal;
	Bay				*pBay;
	Run				*pRun;
	CString			sStage, sBayNumber, sLevel;
	BayStageLevel	*pElement;

	m_ListBoxCtrl.DeleteAllItems( ); // clear list box
	m_ListBoxCtrl.UpdateWindow( );

	iIndex=0;

	///////////////////////////////////////////////////////
	//Find out how many stages and levels there will be!
	iProgTotal = 0;
	for( iRun=0; iRun<m_pController->GetNumberOfRuns(); iRun++ )
	{
		pRun = m_pController->GetRun(iRun);
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay( iBay );
			GetRangeOfLevelsForBay( pBay, iMinLevel, iMaxLevel );
			iProgTotal+= iMaxLevel-iMinLevel;
		}
	}
			

	///////////////////////////////////////////////////////
	iProg = 0;
	acedSetStatusBarProgressMeter( _T("Determining stages and levels"), iProg, iProgTotal );
	acedSetStatusBarProgressMeterPos( iProg );
/*
	COLORREF cr;
*/
	bool bCommitted, bVisible;
	for( iRun=0; iRun<m_pController->GetNumberOfRuns(); iRun++ )
	{
		pRun = m_pController->GetRun(iRun);
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay( iBay );
			GetRangeOfLevelsForBay( pBay, iMinLevel, iMaxLevel );
			iBayNumber = pBay->GetBayNumber();
			sBayNumber.Format( _T("%i"), iBayNumber );

			for( ; iMinLevel<=iMaxLevel; iMinLevel++ )
			{
				iListIndex = m_ListBoxCtrl.InsertItem( iIndex, sBayNumber );

				sLevel.Format( _T("%i"), iMinLevel+1 );
				m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_LEVEL, sLevel ); 

				sStage = STAGE_DEFAULT_VALUE;
				iSize = GetBayStageLevelArray()->GetSize();
/*
				#ifdef _DEBUG
				acutPrintf( "\nSizeOfBayStageArray = %i", iSize );
				#endif	//#ifdef _DEBUG
*/
				acedSetStatusBarProgressMeterPos( iProg );

				bCommitted = false;
				bVisible = true;
				for( i=0; i<iSize; i++ )
				{
					pElement = GetBayStageLevelArray()->GetAt(i);
/*
					#ifdef _DEBUG
					acutPrintf( "\nBayStageArray[%i] = Bay(%i), Level(%i), Stage(%s)",
							i, pElement->GetBay()->GetBayNumber(), pElement->GetLevel(),
							pElement->GetStage() );
					#endif	//#ifdef _DEBUG
*/
					if( iBayNumber==pElement->GetBay()->GetBayNumber() &&
						iMinLevel+1==pElement->GetLevel() )
					{
						sStage = pElement->GetStage();
						bCommitted = pElement->IsCommitted();
						bVisible = m_pController->IsStageLevelVisible( sStage, iMinLevel+1 );
						break;
					}
				}

/*				///////////////////////////////////////////////////////////////
				//We need to find out if any of the components for this level are
				//	committed!
				bCommitted = false;
				bVisible = true;
				for( i=0; i<pBay->GetNumberOfBayComponents(); i++ )
				{
					pComp = pBay->GetBayComponent( i );
					if( pComp->IsCommitted() && pComp->GetLevel()==iMinLevel &&
						pComp->GetStage()==sStage)
					{
						bCommitted = true;
						bVisible = false;
						break;
					}
				}
*/
				if( bCommitted )
					m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_COMMITTED, sCommitText ); 
				else
					m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_COMMITTED, _T(" ") ); 

				if( bVisible )
					m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_HIDDEN, sVisibleText ); 
				else				
					m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_HIDDEN, sNotVisibleText ); 


				m_ListBoxCtrl.SetItemText( iListIndex, COLUMN_STAGE, sStage ); 
/*
				JSB todo 20000209 - I am trying to set the colour for the
				currently added line but it is continuously changing the
				text colour for all elements in the list!!!!!
				cr = (COLORREF)0x000000;
				if( sStage==STAGE_DEFAULT_VALUE )
				{
					cr = (COLORREF)0x0000FF;
				}
				m_ListBoxCtrl.SetItemState( iListIndex, LVIS_SELECTED, LVIS_SELECTED );
				m_ListBoxCtrl.SetTextColor(cr);
				m_ListBoxCtrl.SetItemState( iListIndex, 0, LVIS_SELECTED );
*/
				iIndex++;
			}
		}
	}
	acedRestoreStatusBar();
}

void SetStagesDlg::SetColumnHeadings()
{
	const int iWidth = 280;
  m_ListBoxCtrl.InsertColumn( COLUMN_BAY_NUMBER,	_T("Bay #"),	LVCFMT_LEFT, (int)((double)iWidth*0.15), COLUMN_BAY_NUMBER );
	m_ListBoxCtrl.InsertColumn( COLUMN_COMMITTED,	_T("Commit"),	LVCFMT_LEFT, (int)((double)iWidth*0.18), COLUMN_COMMITTED );
	m_ListBoxCtrl.InsertColumn( COLUMN_LEVEL,		_T("Level"),	LVCFMT_LEFT, (int)((double)iWidth*0.16), COLUMN_LEVEL );
	m_ListBoxCtrl.InsertColumn( COLUMN_STAGE,		_T("Stage"),	LVCFMT_LEFT, (int)((double)iWidth*0.34), COLUMN_STAGE );
	m_ListBoxCtrl.InsertColumn( COLUMN_HIDDEN,		_T("Hidden"),	LVCFMT_LEFT, (int)((double)iWidth*0.17), COLUMN_HIDDEN );
}

void SetStagesDlg::GetRangeOfLevelsForSelectedBays(int &iMinLevel, int &iMaxLevel)
{
	int		i, iSize, iMin, iMax;
	Bay		*pBay;

	iSize = m_pBays->GetSize();

	if( iSize<=0 )
	{
		iMinLevel = INVALID_LEVEL;
		iMaxLevel = INVALID_LEVEL;
	}
	else
	{
		//we need to initialize the values
		GetRangeOfLevelsForBay( m_pBays->GetAt(0), iMinLevel, iMaxLevel );

/*		#ifdef _DEBUG
		acutPrintf( "\nRange for Bay%i = %i -> %i", m_pBays->GetAt(0)->GetBayNumber(),
						iMinLevel, iMaxLevel );
		#endif	//#ifdef _DEBUG
*/
		for( i=1; i<iSize; i++ )
		{
			pBay = m_pBays->GetAt(i);
			GetRangeOfLevelsForBay( pBay, iMin, iMax );

/*			#ifdef _DEBUG
			acutPrintf( "\nRange for Bay%i = %i -> %i", m_pBays->GetAt(0)->GetBayNumber(),
							iMin, iMax );
			#endif	//#ifdef _DEBUG
*/
			iMinLevel = min( iMin, iMinLevel );
			iMaxLevel = max( iMax, iMaxLevel );
		}
	}
/*	#ifdef _DEBUG
	acutPrintf( "\nRange for all Bays = %i -> %i", iMinLevel, iMaxLevel );
	#endif	//#ifdef _DEBUG
*/
}

void SetStagesDlg::GetRangeOfLevelsForBay(Bay *pSelectedBay, int &iMinLevel, int &iMaxLevel)
{
	int		iLevelListSize;
	int		iLevelIndex;
	bool	bFound;
	double	dTopBayRL;
	double	dBottomBayRL;
	double	dLevel;

	assert( pSelectedBay!=NULL );

	pSelectedBay->GetRLsForBayByHeights( dBottomBayRL, dTopBayRL );

	//what is the bottom in?
	bFound = false;
	iLevelListSize = m_Levels->GetSize();

	if( iLevelListSize==0 )
	{
		iMinLevel = 0;
		iMaxLevel = 0;
	}
	else
	{

		for( iLevelIndex=0; iLevelIndex<iLevelListSize; iLevelIndex++ )
		{
			dLevel = m_Levels->GetLevel( iLevelIndex );
			if( dBottomBayRL<dLevel )
			{
				iMinLevel = iLevelIndex;
				bFound = true;
				break;
			}
		}

		if( !bFound )
		{
			//it must be in the top level
			iMinLevel = iLevelListSize;
			iMaxLevel = iLevelListSize;
		}
		else
		{
			//do the same for the upper level
			bFound = false;
			for( iLevelIndex=0; iLevelIndex<iLevelListSize; iLevelIndex++ )
			{
				dLevel = m_Levels->GetLevel( iLevelIndex );
				if( dTopBayRL<dLevel )
				{
					iMaxLevel = iLevelIndex;
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				iMaxLevel = iLevelListSize;
			}
		}
	}
}

void SetStagesDlg::FillLevelCtrls(int iMinLevel, int iMaxLevel)
{
	CString sNumber;

	assert( iMinLevel<=iMaxLevel );

	m_LowerLevelCombo.Clear();
	m_LowerLevelCombo.ResetContent();
	m_UpperLevelCombo.Clear();
	m_UpperLevelCombo.ResetContent();

	if( iMinLevel>=0 || iMaxLevel>=0 )
	{
		for ( ; iMinLevel<=iMaxLevel; iMinLevel++ )
		{
			sNumber.Format( _T("%d"), iMinLevel+1 );
			m_LowerLevelCombo.AddString( sNumber );	
			m_UpperLevelCombo.AddString( sNumber );	
		}
	}
}

void SetStagesDlg::UpdateTableSelectionFromBayList()
{
	int		j, i, iSize, iBayNumber, iLevel, iLevelLower, iLevelUpper;
	Bay		*pBay;
	bool	bFound;
	CString	sBayNumber, sLevel, sStage;

	iSize = m_ListBoxCtrl.GetItemCount();
	for( i=0; i<iSize; i++ )
	{
		sBayNumber = m_ListBoxCtrl.GetItemText( i, COLUMN_BAY_NUMBER );
		iBayNumber = _ttoi( sBayNumber );
		assert( iBayNumber>0 );//invalid bay selected!

		bFound = false;
		for( j=0; j<m_pBays->GetSize(); j++ )
		{
			pBay = m_pBays->GetAt(j);
			if( iBayNumber==pBay->GetBayNumber() )
			{
				//what level is this list item
				sLevel = m_ListBoxCtrl.GetItemText( i, COLUMN_LEVEL );
				iLevel = _ttoi( sLevel );

				//what is the range for the levels?
				iLevelLower = m_LowerLevelCombo.GetCurSel()+1;
				iLevelUpper = m_UpperLevelCombo.GetCurSel()+1;

				//Is this level in or out?
				if( iLevel>=iLevelLower &&
					iLevel<=iLevelUpper )
				{
					//Yep this one is selected!
					bFound = true;
					break;
				}
			}
		}

		if( bFound )
		{
			//Select it!
			m_ListBoxCtrl.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		}
		else
		{
			//Deselect it!
			m_ListBoxCtrl.SetItemState( i, 0, LVIS_SELECTED );
		}
	}
}

void SetStagesDlg::UpdateBayListFromTable()
{
	int			i, iBayNumber, iIndex;
	Bay			*pBay;
	bool		bFound;
	CString		sBayNumber;
	POSITION	pos;

	m_pBays->RemoveAll();
	pos = m_ListBoxCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos) // go through each selection
		{
			iIndex = m_ListBoxCtrl.GetNextSelectedItem(pos);
			sBayNumber = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_BAY_NUMBER );
			iBayNumber = _ttoi( sBayNumber );

			assert( iBayNumber>0 );//invalid bay selected!

			//is the bay already in the baylist?
			bFound = false;
			for( i=0; i<m_pBays->GetSize(); i++ )
			{
				pBay = m_pBays->GetAt(i);
				if( pBay->GetBayNumber()==iBayNumber )
				{
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				//not in the baylist so add it!
				pBay = m_pController->GetBayFromBayNumber( iBayNumber );
				m_pBays->Add( pBay );
			}
		}
	}
}

void SetStagesDlg::AssignStageToSelections(CString sStageName)
{
	int				i, iBayNumber, iLevel, iIndex;
	bool			bFound;
	CString			sBayNumber, sLevel, sStage;
	POSITION		pos;
	BayStageLevel	*pElement;

	bFound = false;
	for( i=0; i<m_pController->GetNumberOfStages(); i++ )
	{
		sStage = m_pController->GetStage(i);
		if( sStage==sStageName )
		{
			bFound = true;
			break;
		}
	}

	if( !bFound )
	{
		m_pController->AddStage( sStageName );
	}

	m_pBays->RemoveAll();
	pos = m_ListBoxCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos) // go through each selection
		{
			iIndex = m_ListBoxCtrl.GetNextSelectedItem(pos);
			sBayNumber = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_BAY_NUMBER );
			iBayNumber = _ttoi( sBayNumber );
			sLevel = m_ListBoxCtrl.GetItemText( iIndex, COLUMN_LEVEL );
			iLevel = _ttoi( sLevel );

			assert( iBayNumber>0 );//invalid bay selected!

			//is the bay already in the baylist?
			bFound = false;
			for( i=0; i<GetBayStageLevelArray()->GetSize(); i++ )
			{
				pElement = GetBayStageLevelArray()->GetAt(i);
				if( (iBayNumber==pElement->GetBay()->GetBayNumber()) &&
					(iLevel==pElement->GetLevel()) )
				{
					pElement->SetStage( sStageName );
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				pElement = new BayStageLevel();
				pElement->SetBay( m_pController->GetBayFromBayNumber( iBayNumber ) );
				pElement->SetLevel( iLevel );
				pElement->SetStage( sStageName );
				GetBayStageLevelArray()->Add( pElement );
			}
													
			m_ListBoxCtrl.SetItemText( iIndex, COLUMN_STAGE, sStageName ); 
		}
	}
}

void SetStagesDlg::SelectUnassigned()
{	
	int		i, iSize;
	CString	sStage;

	iSize = m_ListBoxCtrl.GetItemCount();
	for( i=0; i<iSize; i++ )
	{
		sStage = m_ListBoxCtrl.GetItemText( i, COLUMN_STAGE );
		if( sStage==STAGE_DEFAULT_VALUE )
		{
			//Select it!
			m_ListBoxCtrl.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		}
		else
		{
			//Deselect it!
			m_ListBoxCtrl.SetItemState( i, 0, LVIS_SELECTED );
		}
	}
}

void SetStagesDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("SetStagesDlg") );
}


void SetStagesDlg::SetColourByStageLevel(bool bColourByStageLevel)
{
	m_bColour3DByStageLevel = ConvertboolToBOOL(bColourByStageLevel);
}

bool SetStagesDlg::IsColourByStageLevel()
{
	return ConvertBOOLTobool(m_bColour3DByStageLevel);
}

void SetStagesDlg::OnShowHide() 
{
	StageLevelShowHideDlg	Dialog;
	Dialog.DoModal();
	FillStageLevelBox();
}

void SetStagesDlg::OnDblclkSetstagesListbox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int			nItem;

	nItem	= m_ListBoxCtrl.GetHotItem();
	if( nItem>=0 && nItem<m_ListBoxCtrl.GetItemCount() )
	{
		m_ListBoxCtrl.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );

		OnSetstagesAssignStageButton();
	}
	
	*pResult = 0;
}
