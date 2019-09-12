// LiftDetailsDialog.cpp : implementation file
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
#include "LiftDetailsDialog.h"
#include "LiftList.h"
#include "Bay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ALL_LIFTS	3000
#define NO_DECK_STRING_DEFINE _T("No Deck")

/////////////////////////////////////////////////////////////////////////////
// LiftDetailsDialog dialog


//////////////////////////////////////////////////////////////////////
//Constructor
//
LiftDetailsDialog::LiftDetailsDialog(CWnd* pParent /*=NULL*/, LiftList *pLiftList)
	: KwikscafDialog(LiftDetailsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(LiftDetailsDialog)
	m_sDeckHeight = _T("");
	m_sHandRailStyle = _T("");
	m_sLiftHeight = _T("");
	m_sLiftLength = _T("");
	m_sLiftLevel = _T("");
	m_sLiftSelection = _T("");
	m_sLiftStage = _T("");
	m_sLiftWidth = _T("");
	m_sMaterialHeight = _T("");
	//}}AFX_DATA_INIT

	m_pParent = pParent;
	m_nID = LiftDetailsDialog::IDD;
	m_pLiftList = pLiftList;
	m_pSelectedLift = NULL;

	iSelectedLiftWestStageboardWidth	= 0;
	iSelectedLiftEastStageboardWidth	= 0;
	iSelectedLiftSouthStageboardWidth	= 0;
	iSelectedLiftNorthStageboardWidth	= 0;

	eSelectedLiftDeckMaterial = MT_STEEL;

	EWButtonBitmapNoPlanks.LoadBitmap(IDB_EW_NO_PLANKS_BITMAP);
	EWButtonBitmapOnePlank.LoadBitmap(IDB_EW_ONE_PLANK_BITMAP);
	EWButtonBitmapTwoPlanks.LoadBitmap(IDB_EW_TWO_PLANKS_BITMAP);
	EWButtonBitmapThreePlanks.LoadBitmap(IDB_EW_THREE_PLANKS_BITMAP); 
	NSButtonBitmapNoPlanks.LoadBitmap(IDB_NS_NO_PLANKS_BITMAP);
	NSButtonBitmapOnePlank.LoadBitmap(IDB_NS_ONE_PLANK_BITMAP);
	NSButtonBitmapTwoPlanks.LoadBitmap(IDB_NS_TWO_PLANKS_BITMAP);
	NSButtonBitmapThreePlanks.LoadBitmap(IDB_NS_THREE_PLANKS_BITMAP); 
}

//////////////////////////////////////////////////////////////////////
//Destructor
//
LiftDetailsDialog::~LiftDetailsDialog()
{
}

//////////////////////////////////////////////////////////////////////
//DoDataExchange
//
void LiftDetailsDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LiftDetailsDialog)
	DDX_Control(pDX, IDC_MOVE_LIFT_DOWN, m_btnMoveLiftDown);
	DDX_Control(pDX, IDC_MOVE_LIFT_UP, m_btnMoveLiftUp);
	DDX_Control(pDX, IDC_SW_STAGEBOARD_CHECK, m_SWStageboardCheckCtrl);
	DDX_Control(pDX, IDC_NE_STAGEBOARD_CHECK, m_NEStageboardCheckCtrl);
	DDX_Control(pDX, IDC_SE_STAGEBOARD_CHECK, m_SEStageboardCheckCtrl);
	DDX_Control(pDX, IDC_NW_STAGEBOARD_CHECK, m_NWStageboardCheckCtrl);
	DDX_Control(pDX, IDC_EAST_STAGEBOARD_BUTTON, m_EastStageboardButtonCtrl);
	DDX_Control(pDX, IDC_WEST_STAGEBOARD_BUTTON, m_WestStageboardButtonCtrl);
	DDX_Control(pDX, IDC_SOUTH_STAGEBOARD_BUTTON, m_SouthStageboardButtonCtrl);
	DDX_Control(pDX, IDC_NORTH_STAGEBOARD_BUTTON, m_NorthStageboardButtonCtrl);
	DDX_Control(pDX, IDC_DELETE_LIFT_BUTTON, m_DeleteLiftButtonCtrl);
	DDX_Control(pDX, IDC_HANDRAIL_STYLE_COMBO, m_HandrailStyleComboCtrl);
	DDX_Control(pDX, IDC_WEST_HANDRAIL_BUTTON, m_WestHandrailButtonCtrl);
	DDX_Control(pDX, IDC_SOUTH_HANDRAIL_BUTTON, m_SouthHandrailButtonCtrl);
	DDX_Control(pDX, IDC_EAST_HANDRAIL_BUTTON, m_EastHandrailButtonCtrl);
	DDX_Control(pDX, IDC_NORTH_HANDRAIL_BUTTON, m_NorthHandrailButtonCtrl);
	DDX_Control(pDX, IDC_DECKING_CHECK, m_DeckingCheckCtrl);
	DDX_Control(pDX, IDC_INSERT_LIFT_BUTTON, m_InsertLiftButtonCtrl);
	DDX_Control(pDX, IDC_LIFT_LEVEL_EDIT, m_LevelEditCtrl);
	DDX_Control(pDX, IDC_LIFT_STAGE_EDIT, m_StageEditCtrl);
	DDX_Control(pDX, IDC_MATERIAL_TYPE_COMBO, m_MaterialTypeCombo);
	DDX_Control(pDX, IDC_LIFT_HEIGHT_COMBO, m_LiftHeightComboCtrl);
	DDX_Control(pDX, IDC_DECK_HEIGHT_EDIT, m_DeckHeightEditCtrl);
	DDX_Control(pDX, IDC_LIFT_WIDTH_EDIT, m_LiftWidthEditCtrl);
	DDX_Control(pDX, IDC_LIFT_LENGTH_EDIT, m_LiftLengthEditCtrl);
	DDX_Control(pDX, IDC_LIFT_SELECTION_COMBO, m_LiftSelectionComboCtrl);
	DDX_Control(pDX, IDC_DECK_HEIGHT_SPIN, m_DeckHeightSpinCtrl);
	DDX_Text(pDX, IDC_DECK_HEIGHT_EDIT, m_sDeckHeight);
	DDX_CBString(pDX, IDC_HANDRAIL_STYLE_COMBO, m_sHandRailStyle);
	DDX_CBString(pDX, IDC_LIFT_HEIGHT_COMBO, m_sLiftHeight);
	DDX_Text(pDX, IDC_LIFT_LENGTH_EDIT, m_sLiftLength);
	DDX_Text(pDX, IDC_LIFT_LEVEL_EDIT, m_sLiftLevel);
	DDX_CBString(pDX, IDC_LIFT_SELECTION_COMBO, m_sLiftSelection);
	DDX_Text(pDX, IDC_LIFT_STAGE_EDIT, m_sLiftStage);
	DDX_Text(pDX, IDC_LIFT_WIDTH_EDIT, m_sLiftWidth);
	DDX_CBString(pDX, IDC_MATERIAL_TYPE_COMBO, m_sMaterialHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LiftDetailsDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(LiftDetailsDialog)
	ON_BN_CLICKED(IDC_DELETE_LIFT_BUTTON, OnDeleteLiftButton)
	ON_BN_CLICKED(IDC_INSERT_LIFT_BUTTON, OnInsertLiftButton)
	ON_BN_CLICKED(IDC_LIFT_DETAILS_CANCEL_BUTTON, OnLiftDetailsCancelButton)
	ON_CBN_EDITCHANGE(IDC_LIFT_SELECTION_COMBO, OnEditchangeLiftSelectionCombo)
	ON_EN_CHANGE(IDC_DECK_HEIGHT_EDIT, OnChangeDeckHeightEdit)
	ON_CBN_SELCHANGE(IDC_LIFT_SELECTION_COMBO, OnSelchangeLiftSelectionCombo)
	ON_BN_CLICKED(IDC_DECKING_CHECK, OnDeckingCheck)
	ON_BN_CLICKED(IDC_NORTH_HANDRAIL_BUTTON, OnNorthHandrailButton)
	ON_BN_CLICKED(IDC_WEST_HANDRAIL_BUTTON, OnWestHandrailButton)
	ON_BN_CLICKED(IDC_EAST_HANDRAIL_BUTTON, OnEastHandrailButton)
	ON_BN_CLICKED(IDC_SOUTH_HANDRAIL_BUTTON, OnSouthHandrailButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_NORTH_STAGEBOARD_BUTTON, OnNorthStageboardButton)
	ON_BN_CLICKED(IDC_SOUTH_STAGEBOARD_BUTTON, OnSouthStageboardButton)
	ON_BN_CLICKED(IDC_WEST_STAGEBOARD_BUTTON, OnWestStageboardButton)
	ON_BN_CLICKED(IDC_EAST_STAGEBOARD_BUTTON, OnEastStageboardButton)
	ON_CBN_SELCHANGE(IDC_MATERIAL_TYPE_COMBO, OnSelchangeMaterialTypeCombo)
	ON_CBN_SELCHANGE(IDC_LIFT_HEIGHT_COMBO, OnSelchangeLiftHeightCombo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DECK_HEIGHT_SPIN, OnDeltaposDeckHeightSpin)
	ON_BN_CLICKED(IDC_MOVE_LIFT_UP, OnMoveLiftUp)
	ON_BN_CLICKED(IDC_MOVE_LIFT_DOWN, OnMoveLiftDown)
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_HANDRAIL_STYLE_COMBO, OnSelchangeHandrailStyleCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
//Create
//
BOOL LiftDetailsDialog::Create()
{
	return KwikscafDialog::Create(m_nID, m_pParent);
}


//////////////////////////////////////////////////////////////////////
//OnInitDialog
//
BOOL LiftDetailsDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("LiftDetailsDialog"));
	
	LoadDefaults();
	GetCurrentSettings();
	m_LiftSelectionComboCtrl.SetCurSel(1); // select first lift
	OnSelchangeLiftSelectionCombo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////
// OnShowWindow
//
void LiftDetailsDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	KwikscafDialog::OnShowWindow(bShow, nStatus);
	m_pParent->EnableWindow(false);	
}

//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// LiftDetailsDialog message handlers
//***************************************************************************

//////////////////////////////////////////////////////////////////////
//OnDeleteLiftButton
//
void LiftDetailsDialog::OnDeleteLiftButton() 
{
	int		 iAtLift;
	int		 iCurSel;
	Bay		*pBay;

	if (m_LiftSelectionComboCtrl.GetCount() > 2 ) // can't delete every lift!
	{
		pBay = m_pSelectedLift->GetBayPointer();

		iCurSel = m_LiftSelectionComboCtrl.GetCurSel();
		iAtLift = m_LiftSelectionComboCtrl.GetItemData(iCurSel);
		pBay->DeleteLift(iAtLift);
		//reenters selection list
		GetCurrentSettings();
		m_LiftSelectionComboCtrl.SetCurSel(iCurSel - 1);
		OnSelchangeLiftSelectionCombo();
	}
	else // only 1 lift remains
	{
		MessageBox(_T("Not allowed to delete all lifts. At least one must remain"), _T("User Error"));
	}
}

//////////////////////////////////////////////////////////////////////
//OnInsertLiftButton
//
void LiftDetailsDialog::OnInsertLiftButton() 
{
	int		 iAtLift;
	int		 iCurSel;
	Bay		*pBay;
	Lift	*pNewLift;
//	int		iRise;
//	int		iIndex;

//	iIndex = m_LiftHeightComboCtrl.GetCurSel();        // get rise height
//	iRise = m_LiftHeightComboCtrl.GetItemData(iIndex);

	pNewLift = new Lift();
	pNewLift->SetRise( LIFT_RISE_2000MM );
	pBay = m_pSelectedLift->GetBayPointer();

	iCurSel = m_LiftSelectionComboCtrl.GetCurSel();
	iAtLift = m_LiftSelectionComboCtrl.GetItemData(iCurSel);
	pBay->InsertLift(iAtLift, pNewLift);
	*pNewLift = *m_pSelectedLift;

	//reenters selection list
	GetCurrentSettings();
	m_LiftSelectionComboCtrl.SetCurSel(iCurSel + 1);
	OnSelchangeLiftSelectionCombo();
}



//////////////////////////////////////////////////////////////////////
//OnLiftDetailsCancelButton
//
void LiftDetailsDialog::OnLiftDetailsCancelButton() 
{
	KwikscafDialog::OnCancel();
//	ShowWindow(SW_HIDE);
//	m_pParent->EnableWindow(true);	
}



//////////////////////////////////////////////////////////////////////
// LoadDefaults
//
void LiftDetailsDialog::LoadDefaults()
{
	int			iIndex;

	iIndex = m_MaterialTypeCombo.AddString(NO_DECK_STRING_DEFINE);
	m_MaterialTypeCombo.SetItemData(iIndex, MT_OTHER);
	iIndex = m_MaterialTypeCombo.AddString(GetComponentMaterialStr( MT_STEEL ));
	m_MaterialTypeCombo.SetItemData(iIndex, MT_STEEL);
	iIndex = m_MaterialTypeCombo.AddString(GetComponentMaterialStr( MT_TIMBER ));
	m_MaterialTypeCombo.SetItemData(iIndex, MT_TIMBER);

	iIndex = m_HandrailStyleComboCtrl.AddString(GetHandrailTypeDescriptionStr( CT_HANDRAIL_MIDRAIL_TB ));
	m_HandrailStyleComboCtrl.SetItemData(iIndex, CT_HANDRAIL_MIDRAIL_TB );
	iIndex = m_HandrailStyleComboCtrl.AddString(GetHandrailTypeDescriptionStr( CT_HANDRAIL_HALFMESH ));
	m_HandrailStyleComboCtrl.SetItemData(iIndex, CT_HANDRAIL_HALFMESH );
	iIndex = m_HandrailStyleComboCtrl.AddString(GetHandrailTypeDescriptionStr( CT_HANDRAIL_FULLMESH ));
	m_HandrailStyleComboCtrl.SetItemData(iIndex, CT_HANDRAIL_FULLMESH );
}


//////////////////////////////////////////////////////////////////////
//GetCurrentSettings
//
void LiftDetailsDialog::GetCurrentSettings()
{
	int		iComboIndex, iNumberOfLifts;
	Lift	*pLift;
	double	dRL;
	CString sLiftSelection;

	// empty selection list combo
	m_LiftSelectionComboCtrl.ResetContent();
	// fill selection list combo
	iComboIndex = m_LiftSelectionComboCtrl.AddString(_T("<All Lifts>"));
	m_LiftSelectionComboCtrl.SetItemData(iComboIndex, ALL_LIFTS);

	iNumberOfLifts = m_pLiftList->GetSize();
	for ( int index=iNumberOfLifts-1; index>=0; index-- )
	{
		pLift = (m_pLiftList->GetLift(index));
		dRL = m_pLiftList->GetRL(index)+pLift->GetBayPointer()->RLAdjust();
    sLiftSelection.Format(_T("Lift %d: %.2f "), index+1, ConvertStarRLtoRL(dRL, pLift->GetStarSeparation()) );

		if(pLift->IsDeckingLift())
			sLiftSelection+= _T("Deck ");
		if(pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) )
			sLiftSelection+= _T("N");
		if(pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) )
			sLiftSelection+= _T("E");
		if(pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, SOUTH ) )
			sLiftSelection+= _T("S");
		if(pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) )
			sLiftSelection+= _T("W");
		
		iComboIndex = m_LiftSelectionComboCtrl.AddString(sLiftSelection);		
		m_LiftSelectionComboCtrl.SetItemData(iComboIndex, index);
	}
}


//////////////////////////////////////////////////////////////////////
//OnEditchangeLiftSelectionCombo
//
void LiftDetailsDialog::OnEditchangeLiftSelectionCombo() 
{
}

//////////////////////////////////////////////////////////////////////
//OnChangeDeckHeightEdit
//
void LiftDetailsDialog::OnChangeDeckHeightEdit() 
{
	CString sValue;
	double dValue;

	UpdateData();

	dValue = GetDeckHeightFromEdit();
	if (dValue != 0.00)
	{
		if( m_iSelectedLiftID==ALL_LIFTS )
		{
			int		iSelectedLiftID;
			for( iSelectedLiftID=0; iSelectedLiftID<m_pBaySelectedLift->GetNumberOfLifts(); iSelectedLiftID++ )
			{
				m_pLiftList->SetRL(iSelectedLiftID, dValue);		
			}
		}
		else
		{
			m_pLiftList->SetRL(m_iSelectedLiftID, dValue);		
		}
	}
}

//////////////////////////////////////////////////////////////////////
// OnSelchangeLiftHeightCombo
//
void LiftDetailsDialog::OnSelchangeLiftHeightCombo() 
{
	int				iRise;
	int				iIndex;

	if( m_iSelectedLiftID==ALL_LIFTS )
	{
		int		iSelectedLiftID;
		Lift	*pSelectedLift;

		for( iSelectedLiftID=0; iSelectedLiftID<m_pBaySelectedLift->GetNumberOfLifts(); iSelectedLiftID++ )
		{
			pSelectedLift = m_pBaySelectedLift->GetLift( iSelectedLiftID );
			iIndex = m_LiftHeightComboCtrl.GetCurSel();
			iRise = m_LiftHeightComboCtrl.GetItemData(iIndex);
			pSelectedLift->SetRise((LiftRiseEnum)iRise);
		}
	}
	else
	{
		iIndex = m_LiftHeightComboCtrl.GetCurSel();
		iRise = m_LiftHeightComboCtrl.GetItemData(iIndex);
		m_pSelectedLift->SetRise((LiftRiseEnum)iRise);
	}

	m_pBaySelectedLift->MakeAStandardFitLifts( CNR_NORTH_EAST );
	m_pBaySelectedLift->MakeAStandardFitLifts( CNR_SOUTH_EAST );
	m_pBaySelectedLift->MakeAStandardFitLifts( CNR_SOUTH_WEST );
	m_pBaySelectedLift->MakeAStandardFitLifts( CNR_NORTH_WEST );

//	OnSelchangeLiftSelectionCombo();
	GetCurrentSettings();
	UpdateLiftDetails( m_pSelectedLift );
	m_LiftSelectionComboCtrl.SetCurSel(m_iSelectedLiftComboIndex);
}

//////////////////////////////////////////////////////////////////////
//DeckOn
//
void LiftDetailsDialog::DeckOn()
{
	int		iIndex;
	int		iLiftRise;
	CString	sRise;

	m_LiftHeightComboCtrl.ResetContent();

	for (iLiftRise=LIFT_RISE_0500MM; iLiftRise <= LIFT_RISE_2000MM; ++iLiftRise)
	{
		sRise.Format(_T("%2.0f mm"), ((double)iLiftRise * STAR_SEPARATION_APPROX));
		iIndex = m_LiftHeightComboCtrl.AddString(sRise);
		m_LiftHeightComboCtrl.SetItemData(iIndex, iLiftRise);
	}
}


//////////////////////////////////////////////////////////////////////
//AddDeckOff
//
void LiftDetailsDialog::AddDeckOff()
{
	int		iIndex;
	CString	sRise;

	m_LiftHeightComboCtrl.ResetContent();
	sRise.Format(_T("%2.0f mm"), ((double)LIFT_RISE_2000MM * STAR_SEPARATION_APPROX));
	iIndex = m_LiftHeightComboCtrl.AddString(sRise);
	m_LiftHeightComboCtrl.SetItemData(iIndex, LIFT_RISE_2000MM);
}


//////////////////////////////////////////////////////////////////////
//OnDeckingCheck
//
void LiftDetailsDialog::OnDeckingCheck() 
{
	if( m_iSelectedLiftID==ALL_LIFTS )
	{
		int		iSelectedLiftID;
		Lift	*pSelectedLift;
		for( iSelectedLiftID=0; iSelectedLiftID<m_pBaySelectedLift->GetNumberOfLifts(); iSelectedLiftID++ )
		{
			pSelectedLift = m_pBaySelectedLift->GetLift( iSelectedLiftID );
			AddDecking( iSelectedLiftID, pSelectedLift);
		}
	}
	else
	{
		CString sLiftSelection;
		sLiftSelection = AddDecking( m_iSelectedLiftID, m_pSelectedLift );

		// change deck state text in lift selection combo
		m_LiftSelectionComboCtrl.DeleteString(m_iSelectedLiftComboIndex);
		m_LiftSelectionComboCtrl.InsertString(m_iSelectedLiftComboIndex, sLiftSelection);
		m_LiftSelectionComboCtrl.SetItemData(m_iSelectedLiftComboIndex, m_iSelectedLiftID);
		m_LiftSelectionComboCtrl.SetCurSel(m_iSelectedLiftComboIndex);
		UpdateLiftDetails( m_pSelectedLift );
	}
}

CString LiftDetailsDialog::AddDecking(int iSelectedLiftID, Lift *pSelectedLift)
{
	int			iNumberOfPlanks;
	int			i;

	CString		sLiftSelection;
	double		dRL;

	dRL	= m_pLiftList->GetRL(iSelectedLiftID)+pSelectedLift->GetBayPointer()->RLAdjust();

	if (m_DeckingCheckCtrl.GetCheck())
	{
		// update combobox
    sLiftSelection.Format(_T("Lift %d: %.2f Deck"), iSelectedLiftID, ConvertStarRLtoRL(dRL, pSelectedLift->GetStarSeparation()));
		//Create the decking
		iNumberOfPlanks = GetNumberOfPlanksForWidth( 
								pSelectedLift->GetBayPointer()->GetBayWidth() );
		for( i=0; i < iNumberOfPlanks; i++ )
			pSelectedLift->AddComponent( CT_DECKING_PLANK, (pSelectedLift->GetBayPointer())->GetBayLength(), SOUTH,	i, eSelectedLiftDeckMaterial );
	}
	else
	{
		// update combobox
    sLiftSelection.Format(_T("Lift %d: %.2f"), iSelectedLiftID, ConvertStarRLtoRL(dRL, pSelectedLift->GetStarSeparation()) );
		//delete the decking
		pSelectedLift->DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );

		//Delete all handrails
		if( !pSelectedLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, N ) )
			DeleteAnyHandrailsMidrailsToeboards( N, pSelectedLift );
		if( !pSelectedLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, E ) )
			DeleteAnyHandrailsMidrailsToeboards( E, pSelectedLift );
		if( !pSelectedLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, S ) )
			DeleteAnyHandrailsMidrailsToeboards( S, pSelectedLift );
		if( !pSelectedLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, W ) )
			DeleteAnyHandrailsMidrailsToeboards( W, pSelectedLift );
	}
	return sLiftSelection;
}

//////////////////////////////////////////////////////////////////////
//OnSelchangeLiftSelectionCombo() 
//
void LiftDetailsDialog::OnSelchangeLiftSelectionCombo() 
{
	m_iSelectedLiftComboIndex	= m_LiftSelectionComboCtrl.GetCurSel();
	m_iSelectedLiftID			= m_LiftSelectionComboCtrl.GetItemData( m_iSelectedLiftComboIndex );

	if (m_iSelectedLiftID == ALL_LIFTS)
	{
		m_InsertLiftButtonCtrl.SetWindowText(_T("Add a Lift"));		
		m_DeleteLiftButtonCtrl.ShowWindow(SW_HIDE);
		m_pSelectedLift = NULL;
		//However, we do know the bay!
		m_pBaySelectedLift = m_pLiftList->GetLift(0)->GetBayPointer();
	}
	else
	{
		m_InsertLiftButtonCtrl.SetWindowText(_T("Insert a Lift"));		
		m_DeleteLiftButtonCtrl.ShowWindow(SW_SHOW);
		m_pSelectedLift = m_pLiftList->GetLift(m_iSelectedLiftID);
		m_pBaySelectedLift = m_pSelectedLift->GetBayPointer();
		UpdateLiftDetails( m_pSelectedLift );
 		GetAndSetHandrailStyle( m_pSelectedLift );
	}
}


//////////////////////////////////////////////////////////////////////
//UpdateLiftDetails() 
//
void LiftDetailsDialog::UpdateLiftDetails( Lift *pSelectedLift )
{
	CString sText;
	Component *pComponent;

	/////////////////////////////////////////////////////////
	// update lift length and width
	sText.Format(_T("%.2f"), m_pBaySelectedLift->GetBayLength());
	m_LiftLengthEditCtrl.SetWindowText(sText);
	sText.Format(_T("%.2f"), m_pBaySelectedLift->GetBayWidth());
	m_LiftWidthEditCtrl.SetWindowText(sText);

	/////////////////////////////////////////////////////////
	// set material type for decking
	pComponent = pSelectedLift->GetComponent( CT_DECKING_PLANK, 0, SOUTH);
	if (pComponent)
	{
		m_MaterialTypeCombo.SelectString(0, GetComponentMaterialStr( pComponent->GetMaterialType() ));
		//set deck push button state
		m_DeckingCheckCtrl.SetCheck(1);
		AddDeckOff();
	}
	else
	{
		m_MaterialTypeCombo.SelectString(0, NO_DECK_STRING_DEFINE);
		//set deck push button state
		m_DeckingCheckCtrl.SetCheck(0);
		DeckOn();
	}

	/////////////////////////////////////////////////////////
	// set DeckHeight
	CString sValue;
	sValue.Format(_T("%.2f"), ConvertStarRLtoRL( m_pLiftList->GetRL(pSelectedLift->GetLiftID() )+pSelectedLift->GetBayPointer()->RLAdjust(), pSelectedLift->GetStarSeparation() ) );
	m_DeckHeightEditCtrl.SetWindowText(sValue);


	/////////////////////////////////////////////////////////
	// Handrails
	m_NorthHandrailButtonCtrl.SetCheck(AnyHandrail(NORTH, pSelectedLift));
	m_SouthHandrailButtonCtrl.SetCheck(AnyHandrail(SOUTH, pSelectedLift));
	m_EastHandrailButtonCtrl.SetCheck(AnyHandrail(EAST, pSelectedLift));
	m_WestHandrailButtonCtrl.SetCheck(AnyHandrail(WEST, pSelectedLift));

	/////////////////////////////////////////////////////////
	// Stageboards
	UpdateStageboards();

	/////////////////////////////////////////////////////////
	// select rise
	CString	sRise;
	int iRise;
	iRise = pSelectedLift->GetRise();
	sRise.Format(_T("%2.0f mm"), ((double)iRise * STAR_SEPARATION_APPROX));
	m_LiftHeightComboCtrl.SelectString(0, sRise);
}


//***************************************************************************
///////////////////////////////////////////////////////////////////////////
////	HANDRAILS
////
//***************************************************************************
//////////////////////////////////////////////////////////////////////
// OnNorthHandrailButton
//
void LiftDetailsDialog::OnNorthHandrailButton() 
{
	if (m_NorthHandrailButtonCtrl.GetCheck())
		AddHandrailMidrailToeBoard(NORTH);
	else //Delete all handrail components
		DeleteAllHandrailComponents(NORTH);
}

//////////////////////////////////////////////////////////////////////
// OnWestHandrailButton
//
void LiftDetailsDialog::OnWestHandrailButton() 
{
	if (m_WestHandrailButtonCtrl.GetCheck())
		AddHandrailMidrailToeBoard(WEST);
	else //Delete all handrail components
		DeleteAllHandrailComponents(WEST);
}

//////////////////////////////////////////////////////////////////////
// OnEastHandrailButton
//
void LiftDetailsDialog::OnEastHandrailButton() 
{
	if (m_EastHandrailButtonCtrl.GetCheck())
		AddHandrailMidrailToeBoard(EAST);
	else //Delete all handrail components
		DeleteAllHandrailComponents(EAST);
}

//////////////////////////////////////////////////////////////////////
// OnSouthHandrailButton
//
void LiftDetailsDialog::OnSouthHandrailButton() 
{
	if (m_SouthHandrailButtonCtrl.GetCheck())
		AddHandrailMidrailToeBoard(SOUTH);
	else //Delete all handrail components
		DeleteAllHandrailComponents(SOUTH);
}

//////////////////////////////////////////////////////////////////////
// AnyHandrail
//
bool LiftDetailsDialog::AnyHandrail(SideOfBayEnum eSideOfBay, Lift *pSelectedLift )
{
	//Handrails		Midrails		ToeBoard
	if( pSelectedLift->HasComponentOfTypeOnSide( CT_RAIL, eSideOfBay) )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// AnyMidrail
//
bool LiftDetailsDialog::AnyMidrail(SideOfBayEnum eSideOfBay, Lift *pSelectedLift)
{
	//Handrails		Midrails		ToeBoard
	if( pSelectedLift->HasComponentOfTypeOnSide( CT_MID_RAIL, eSideOfBay) )
		return true;
	else
		return false;
}

bool LiftDetailsDialog::AnyMeshHalf( SideOfBayEnum eSideOfBay, Lift *pSelectedLift)
{
	if( pSelectedLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, eSideOfBay) ) 
		return true;
	else
		return false;
}

bool LiftDetailsDialog::AnyMeshFull(SideOfBayEnum eSideOfBay, Lift *pSelectedLift)
{
	if( pSelectedLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eSideOfBay) ) 
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// DeleteAnyHandrailsMidrailsToeboards
//
void LiftDetailsDialog::DeleteAnyHandrailsMidrailsToeboards(SideOfBayEnum eSideOfBay, Lift *pSelectedLift )
{
	SideOfBayEnum	eSideLeft, eSideRight;
	eSideLeft	= SIDE_INVALID;
	eSideRight	= SIDE_INVALID;
	switch( eSideOfBay )
	{
	case( N ):
		eSideLeft	= WNW;
		eSideRight	= ENE;
		break;
	case( E ):
		eSideLeft	= NNE;
		eSideRight	= SSE;
		break;
	case( S ):
		eSideLeft	= ESE;
		eSideRight	= WSW;
		break;
	case( W ):
		eSideLeft	= SSW;
		eSideRight	= NNW;
		break;
	default:
		;//assert( false );
	}

	;//assert( pSelectedLift!=NULL );
	//Handrail
	pSelectedLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSideOfBay );
	pSelectedLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSideOfBay );
	pSelectedLift->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSideOfBay );
	if( eSideOfBay==NORTH || eSideOfBay==SOUTH )
		pSelectedLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSideOfBay );

	Component	*pComp;
	pComp = pSelectedLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideLeft );
	if( pComp!=NULL && eSideLeft!=SIDE_INVALID )
		pSelectedLift->DeleteComponent( pComp->GetID() );
	pComp = pSelectedLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideRight );
	if( pComp!=NULL && eSideRight!=SIDE_INVALID )
		pSelectedLift->DeleteComponent( pComp->GetID() );
}

//////////////////////////////////////////////////////////////////////
// AddHandrailMidrailToeBoard
//
void LiftDetailsDialog::AddHandrailMidrailToeBoard(SideOfBayEnum eSide)
{
	if( m_pSelectedLift==NULL )
	{
		int		iLift;
		Lift	*pSelectedLift;
		for( iLift=0; iLift<m_pBaySelectedLift->GetNumberOfLifts(); iLift++ )
		{
			pSelectedLift = m_pBaySelectedLift->GetLift( iLift );
			AddHandrailMidrailToeBoard(eSide, pSelectedLift);
		}
	}
	else
	{
		AddHandrailMidrailToeBoard(eSide, m_pSelectedLift);
	}
}

//////////////////////////////////////////////////////////////////////
// AddHandrailMidrailToeBoard
//
void LiftDetailsDialog::AddHandrailMidrailToeBoard(SideOfBayEnum eSideOfBay, Lift *pSelectedLift)
{
	HandrailTypeEnum eHandrailType;
	double			 dLength;
	bool			 bToeBoardAllowed;

	//Delete them first
	DeleteAllHandrailComponents(eSideOfBay);

	dLength = m_pBaySelectedLift->GetBayWidth();
	bToeBoardAllowed = false;
	if ((eSideOfBay == NORTH) || (eSideOfBay == SOUTH))
	{
		dLength = m_pBaySelectedLift->GetBayLength();
		bToeBoardAllowed = true;
	}

	pSelectedLift->AddComponent( CT_RAIL, dLength, eSideOfBay, LIFT_RISE_1000MM, eSelectedLiftDeckMaterial );

	// get Handrail type
	eHandrailType = (HandrailTypeEnum)m_HandrailStyleComboCtrl.GetItemData(m_HandrailStyleComboCtrl.GetCurSel());
	switch (eHandrailType)
	{
		default:
			;//assert( false );
			break;
		case CT_HANDRAIL_MIDRAIL_TB :
			pSelectedLift->AddComponent( CT_MID_RAIL, dLength, eSideOfBay, LIFT_RISE_0500MM, eSelectedLiftDeckMaterial ); 
			if (bToeBoardAllowed)
				pSelectedLift->AddComponent( CT_TOE_BOARD, dLength, eSideOfBay, LIFT_RISE_0000MM, eSelectedLiftDeckMaterial ); 
			break;
		case CT_HANDRAIL_FULLMESH	:
			if( pSelectedLift->GetLiftID()<(pSelectedLift->GetBayPointer()->GetNumberOfLifts()-1) )
			{
				//we are not the top lift so we can add meshguard at 2000mm
				pSelectedLift->AddComponent( CT_MESH_GUARD, dLength, eSideOfBay, LIFT_RISE_2000MM, eSelectedLiftDeckMaterial );
			}
			//fallthrough
		case CT_HANDRAIL_HALFMESH	:
			pSelectedLift->AddComponent( CT_MESH_GUARD, dLength, eSideOfBay, LIFT_RISE_1000MM, eSelectedLiftDeckMaterial );
			break;
	};
}


//***************************************************************************
///////////////////////////////////////////////////////////////////////////
////	STAGEBOARDS
////
//***************************************************************************

//////////////////////////////////////////////////////////////////////
// OnNorthStageboardButton
//
void LiftDetailsDialog::OnNorthStageboardButton() 
{
	StageBoardWidthClicked(NORTH);
}


//////////////////////////////////////////////////////////////////////
// OnSouthStageboardButton
//
void LiftDetailsDialog::OnSouthStageboardButton() 
{
	StageBoardWidthClicked(SOUTH);
}


//////////////////////////////////////////////////////////////////////
// OnWestStageboardButton
//
void LiftDetailsDialog::OnWestStageboardButton() 
{
	StageBoardWidthClicked(WEST);
}


//////////////////////////////////////////////////////////////////////
// OnEastStageboardButton
//
void LiftDetailsDialog::OnEastStageboardButton() 
{
	StageBoardWidthClicked(EAST);
}


//////////////////////////////////////////////////////////////////////
// StageBoardWidthClicked
//
void LiftDetailsDialog::StageBoardWidthClicked(SideOfBayEnum eSideOfBay)
{
	switch (eSideOfBay)
	{
		case NORTH : IncrementAndSetStageButton( NORTH, iSelectedLiftNorthStageboardWidth );
					 break;
		case SOUTH : IncrementAndSetStageButton( SOUTH, iSelectedLiftSouthStageboardWidth );
					 break;
		case EAST  : IncrementAndSetStageButton( EAST, iSelectedLiftEastStageboardWidth );
					 break;
		case WEST  : IncrementAndSetStageButton( WEST, iSelectedLiftWestStageboardWidth);
					 break;
	}
	UpdateStageboards();
	GetCurrentSettings();
	m_LiftSelectionComboCtrl.SetCurSel(m_iSelectedLiftComboIndex);
}


//////////////////////////////////////////////////////////////////////
// IncrementAndSetStageButton
//
void LiftDetailsDialog::IncrementAndSetStageButton(SideOfBayEnum eSideOfBay, int &iStageBoardWidth)
{
	CString sNumberOfPlanks;

	++iStageBoardWidth; 
	if (iStageBoardWidth > 3)
		iStageBoardWidth = 0;

	UpdateStageboardBitmap(eSideOfBay);
	SetAndCheckCornerStageboards();

	if( m_pSelectedLift!=NULL )
	{
		// regenerate stages
		DeleteAllStageboardsCornersHopups(m_pSelectedLift);
		AddStageBoardsCornersHopups(m_pSelectedLift);
	}
	else
	{
		int		iLift;
		Lift	*pSelectedLift;

		for( iLift=0; iLift<m_pBaySelectedLift->GetNumberOfLifts(); iLift++ )
		{
			pSelectedLift = m_pBaySelectedLift->GetLift( iLift );
			// regenerate stages
			DeleteAllStageboardsCornersHopups(pSelectedLift);
			AddStageBoardsCornersHopups(pSelectedLift);
		}
	}
}


//////////////////////////////////////////////////////////////////////
// AnyStageBoards
//
int LiftDetailsDialog::AnyStageBoards(SideOfBayEnum eSideOfBay)
{
	int iNumberOfStageboards;

	iNumberOfStageboards = 0;
	if (m_pSelectedLift)
	{
		if (m_pSelectedLift->GetComponent( CT_STAGE_BOARD, PLANK01, eSideOfBay ))
			++iNumberOfStageboards;
		if (m_pSelectedLift->GetComponent( CT_STAGE_BOARD, PLANK02, eSideOfBay ))
			++iNumberOfStageboards;
		if (m_pSelectedLift->GetComponent( CT_STAGE_BOARD, PLANK03, eSideOfBay ))
			++iNumberOfStageboards;
	}
	return iNumberOfStageboards;
}


//////////////////////////////////////////////////////////////////////
// UpdateStageboards
//
void LiftDetailsDialog::UpdateStageboards()
{
	// NORTH Stageboards
	iSelectedLiftNorthStageboardWidth = AnyStageBoards(NORTH);
	UpdateStageboardBitmap(NORTH);

	//	SOUTH Stageboards
	iSelectedLiftSouthStageboardWidth = AnyStageBoards(SOUTH);
	UpdateStageboardBitmap(SOUTH);

	//	EAST Stageboards
	iSelectedLiftEastStageboardWidth = AnyStageBoards(EAST);
	UpdateStageboardBitmap(EAST);

	//	WEST Stageboards
	iSelectedLiftWestStageboardWidth = AnyStageBoards(WEST);
	UpdateStageboardBitmap(WEST);

	if( m_pSelectedLift!=NULL )
	{
		if( m_pSelectedLift->IsEmptyLift() )
		{
			if(iSelectedLiftNorthStageboardWidth>0)
			{
				m_NorthHandrailButtonCtrl.SetCheck(true);
				OnNorthHandrailButton();
				if( iSelectedLiftSouthStageboardWidth==0 )
				{
					m_SouthHandrailButtonCtrl.SetCheck(false);
					OnSouthHandrailButton();
				}
				if( iSelectedLiftEastStageboardWidth==0 )
				{
					m_EastHandrailButtonCtrl.SetCheck(false);
					OnEastHandrailButton();
				}
				if( iSelectedLiftWestStageboardWidth==0 )
				{
					m_WestHandrailButtonCtrl.SetCheck(false);
					OnWestHandrailButton();
				}
			}
			if(iSelectedLiftSouthStageboardWidth>0)
			{
				m_SouthHandrailButtonCtrl.SetCheck(true);
				OnSouthHandrailButton();
				if( iSelectedLiftNorthStageboardWidth==0 )
				{
					m_NorthHandrailButtonCtrl.SetCheck(false);
					OnNorthHandrailButton();
				}
				if( iSelectedLiftEastStageboardWidth==0 )
				{
					m_EastHandrailButtonCtrl.SetCheck(false);
					OnEastHandrailButton();
				}
				if( iSelectedLiftWestStageboardWidth==0 )
				{
					m_WestHandrailButtonCtrl.SetCheck(false);
					OnWestHandrailButton();
				}
			}
			if(iSelectedLiftEastStageboardWidth>0)
			{
				m_EastHandrailButtonCtrl.SetCheck(true);
				OnEastHandrailButton();
				if( iSelectedLiftNorthStageboardWidth==0 )
				{
					m_NorthHandrailButtonCtrl.SetCheck(false);
					OnNorthHandrailButton();
				}
				if( iSelectedLiftSouthStageboardWidth==0 )
				{
					m_SouthHandrailButtonCtrl.SetCheck(false);
					OnSouthHandrailButton();
				}
				if( iSelectedLiftWestStageboardWidth==0 )
				{
					m_WestHandrailButtonCtrl.SetCheck(false);
					OnWestHandrailButton();
				}
			}
			if(iSelectedLiftWestStageboardWidth>0)
			{
				m_WestHandrailButtonCtrl.SetCheck(true);
				OnWestHandrailButton();
				if( iSelectedLiftNorthStageboardWidth==0 )
				{
					m_NorthHandrailButtonCtrl.SetCheck(false);
					OnNorthHandrailButton();
				}
				if( iSelectedLiftSouthStageboardWidth==0 )
				{
					m_SouthHandrailButtonCtrl.SetCheck(false);
					OnSouthHandrailButton();
				}
				if( iSelectedLiftEastStageboardWidth==0 )
				{
					m_EastHandrailButtonCtrl.SetCheck(false);
					OnEastHandrailButton();
				}
			}
		}
	}

	// now check and set cnr stageboards
	SetAndCheckCornerStageboards();
}


//////////////////////////////////////////////////////////////////////
// SetAndCheckCornerStageboards
//
void LiftDetailsDialog::SetAndCheckCornerStageboards()
{
	// NW
	DisplayCornerStageboard(&m_NWStageboardCheckCtrl, iSelectedLiftNorthStageboardWidth, iSelectedLiftWestStageboardWidth); 
	// NE
	DisplayCornerStageboard(&m_NEStageboardCheckCtrl, iSelectedLiftNorthStageboardWidth, iSelectedLiftEastStageboardWidth);
	// SE
	DisplayCornerStageboard(&m_SEStageboardCheckCtrl, iSelectedLiftSouthStageboardWidth, iSelectedLiftEastStageboardWidth); 
	// SW
	DisplayCornerStageboard(&m_SWStageboardCheckCtrl, iSelectedLiftSouthStageboardWidth, iSelectedLiftWestStageboardWidth); 
}


//////////////////////////////////////////////////////////////////////
// DisplayCornerStageboard
//
void LiftDetailsDialog::DisplayCornerStageboard(CButton *pButton, int iStageboardWidthX, int iStageboardWidthY)
{
	CString sText;

	if (( iStageboardWidthX != 0 ) && ( iStageboardWidthY != 0 ))
	{
		sText.Format(_T("%d x %d"), iStageboardWidthX, iStageboardWidthY );		
		pButton->SetWindowText(sText);
		pButton->ShowWindow(SW_SHOW);
	}
	else
		pButton->ShowWindow(SW_HIDE);
}


//////////////////////////////////////////////////////////////////////
// UpdateStageboardBitmap
//
void LiftDetailsDialog::UpdateStageboardBitmap(SideOfBayEnum eSideOfBay)
{
	CButton *pStageboardButton;
	int		iStageboardWidth;
	bool	vertical;
	CBitmap *pButtonBitmap;

	switch (eSideOfBay)
	{
		case NORTH : pStageboardButton = &m_NorthStageboardButtonCtrl;
					 iStageboardWidth = iSelectedLiftNorthStageboardWidth;
					 vertical = false;
					 break;
		case SOUTH : pStageboardButton = &m_SouthStageboardButtonCtrl;
					 iStageboardWidth = iSelectedLiftSouthStageboardWidth;
					 vertical = false;
					 break;
		case EAST  : pStageboardButton = &m_EastStageboardButtonCtrl;
					 iStageboardWidth = iSelectedLiftEastStageboardWidth;
					 vertical = true;
					 break;
		case WEST  : pStageboardButton = &m_WestStageboardButtonCtrl;
					 iStageboardWidth = iSelectedLiftWestStageboardWidth;
					 vertical = true;
					 break;
	}
	if (vertical)
	{
		switch (iStageboardWidth) 
		{
			case 0 :	pButtonBitmap = &EWButtonBitmapNoPlanks;	break;
			case 1 :	pButtonBitmap = &EWButtonBitmapOnePlank;	break;
			case 2 :	pButtonBitmap = &EWButtonBitmapTwoPlanks;	break;
			case 3 :	pButtonBitmap = &EWButtonBitmapThreePlanks; break;
		};
	} 
	else
	{
		switch (iStageboardWidth) 
		{
			case 0 :	pButtonBitmap = &NSButtonBitmapNoPlanks;	break;
			case 1 :	pButtonBitmap = &NSButtonBitmapOnePlank;	break;
			case 2 :	pButtonBitmap = &NSButtonBitmapTwoPlanks;	break;
			case 3 :	pButtonBitmap = &NSButtonBitmapThreePlanks; break;
		};
	}
	pStageboardButton->SetBitmap(*pButtonBitmap);
}


//////////////////////////////////////////////////////////////////////
// DeleteAllStageboardsCornersHopups
//
void LiftDetailsDialog::DeleteAllStageboardsCornersHopups( Lift *pSelectedLift )
{
	if (pSelectedLift)
	{
		// Delete stageboards from all sides
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_STAGE_BOARD, NORTH);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_STAGE_BOARD, SOUTH);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_STAGE_BOARD, EAST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_STAGE_BOARD, WEST);
		
		// Delete hopups from all sides
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, NORTH_NORTH_WEST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, NORTH_NORTH_EAST);

		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, EAST_NORTH_EAST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, EAST_SOUTH_EAST);

		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, SOUTH_SOUTH_EAST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, SOUTH_SOUTH_WEST);

		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, WEST_SOUTH_WEST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_HOPUP_BRACKET, WEST_NORTH_WEST);

		// Delete corner stageboards from all sides
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_CORNER_STAGE_BOARD, NORTH_EAST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_CORNER_STAGE_BOARD, SOUTH_EAST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_CORNER_STAGE_BOARD, NORTH_WEST);
		pSelectedLift->DeleteAllComponentsOfTypeFromSide(CT_CORNER_STAGE_BOARD, SOUTH_WEST);
	}
} 

//////////////////////////////////////////////////////////////////////
// AddStageBoardsCornersHopups
//
void LiftDetailsDialog::AddStageBoardsCornersHopups( Lift *pSelectedLift )
{
	// add stageboards and hopups
	if (iSelectedLiftNorthStageboardWidth > 0)
		pSelectedLift->AddStage( NORTH, iSelectedLiftNorthStageboardWidth, eSelectedLiftDeckMaterial );

	if (iSelectedLiftSouthStageboardWidth > 0)
		pSelectedLift->AddStage( SOUTH, iSelectedLiftSouthStageboardWidth, eSelectedLiftDeckMaterial );

	if (iSelectedLiftEastStageboardWidth > 0)
		pSelectedLift->AddStage( EAST, iSelectedLiftEastStageboardWidth, eSelectedLiftDeckMaterial );

	if (iSelectedLiftWestStageboardWidth > 0)
		pSelectedLift->AddStage( WEST, iSelectedLiftWestStageboardWidth, eSelectedLiftDeckMaterial );

	// add corner stageboards
	if ((iSelectedLiftNorthStageboardWidth > 0) && (iSelectedLiftEastStageboardWidth > 0))
		pSelectedLift->AddCornerStageBoard(SideOfBayAsCorner(NORTH_EAST), eSelectedLiftDeckMaterial );

	if ((iSelectedLiftNorthStageboardWidth > 0) && (iSelectedLiftWestStageboardWidth > 0))
		pSelectedLift->AddCornerStageBoard(SideOfBayAsCorner(NORTH_WEST), eSelectedLiftDeckMaterial );

	if ((iSelectedLiftSouthStageboardWidth > 0) && (iSelectedLiftEastStageboardWidth > 0))
		pSelectedLift->AddCornerStageBoard(SideOfBayAsCorner(SOUTH_EAST), eSelectedLiftDeckMaterial );

	if ((iSelectedLiftSouthStageboardWidth > 0) && (iSelectedLiftWestStageboardWidth > 0))
		pSelectedLift->AddCornerStageBoard(SideOfBayAsCorner(SOUTH_WEST), eSelectedLiftDeckMaterial );
}


//***************************************************************************
///////////////////////////////////////////////////////////////////////////
////	DECKING MATERIAL TYPE
////
//***************************************************************************
//////////////////////////////////////////////////////////////////////
// OnSelchangeMaterialTypeCombo
//
void LiftDetailsDialog::OnSelchangeMaterialTypeCombo() 
{
	MaterialTypeEnum eDeckMaterial;
	int iIndex;
	
	iIndex = m_MaterialTypeCombo.GetCurSel();
	eDeckMaterial = (MaterialTypeEnum)m_MaterialTypeCombo.GetItemData(iIndex);
	if (eDeckMaterial != MT_OTHER)
	{
		eSelectedLiftDeckMaterial = eDeckMaterial;
		// now update existing decking with new material
		// DAR TODO!!
	}
}


//////////////////////////////////////////////////////////////////////
// OnOK
//
void LiftDetailsDialog::OnOK() 
{
	KwikscafDialog::OnOK();
}


//////////////////////////////////////////////////////////////////////
// OnDeltaposDeckHeightSpin
//
void LiftDetailsDialog::OnDeltaposDeckHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
//	SpinCtrl structure below
/*	
//	hdr		NMHDR structure that contains additional information about the notification message. 
//	iPos	Signed integer value that represents the up-down control's current position. 
//	iDelta	Signed integer value that represents the proposed change in the up-down control's position. 

	typedef struct _NM_UPDOWN {
		NMHDR hdr;
		int   iPos;
		int   iDelta;
	} NMUPDOWN, FAR *LPNMUPDOWN;
*/
/*	CString sSpinText;
	sSpinText.Format("iPos : %d\niDelta :%d", pNMUpDown->iPos, pNMUpDown->iDelta); 
	m_pParent->MessageBox(sSpinText);
*/
	double dDeckHeight;

	UpdateData();

	dDeckHeight = GetDeckHeightFromEdit();

	dDeckHeight -= (STAR_SEPARATION_APPROX*pNMUpDown->iDelta);
	if (dDeckHeight >= 0 )
		SetDeckHeight((dDeckHeight/STAR_SEPARATION_APPROX)*m_pLiftList->GetLift(0)->GetStarSeparation());
	
	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////
// GetDeckHeightFromEdit
//
double LiftDetailsDialog::GetDeckHeightFromEdit()
{
	CString sDeckHeight;
	double  dDeckHeight;

	m_DeckHeightEditCtrl.GetWindowText(sDeckHeight);
	dDeckHeight = _tstof(sDeckHeight);
	dDeckHeight-= RL_ADJUST_APPROX;

	return ConvertRLtoStarRL(dDeckHeight, m_pLiftList->GetLift(0)->GetStarSeparation());
}


//////////////////////////////////////////////////////////////////////
// SetDeckHeight
//
void LiftDetailsDialog::SetDeckHeight(double dNewDeckHeight)
{
	// set DeckHeight
	CString sValue;

	if( m_iSelectedLiftID==ALL_LIFTS )
	{
		int iSelectedLiftID;
		for( iSelectedLiftID=0; iSelectedLiftID<m_pBaySelectedLift->GetNumberOfLifts(); iSelectedLiftID++ )
		{
			sValue.Format(_T("%.2f"), ConvertStarRLtoRL(m_pLiftList->GetRL(iSelectedLiftID)+
					m_pLiftList->GetLift(iSelectedLiftID)->GetBayPointer()->RLAdjust(),
					m_pLiftList->GetLift(iSelectedLiftID)->GetStarSeparation()));
			m_DeckHeightEditCtrl.SetWindowText(sValue);
			m_pLiftList->SetRL(iSelectedLiftID, dNewDeckHeight);
		}
	}
	else
	{
		sValue.Format(_T("%.2f"), ConvertStarRLtoRL( m_pLiftList->GetRL(m_iSelectedLiftID)+
			m_pLiftList->GetLift(m_iSelectedLiftID)->GetBayPointer()->RLAdjust(),
			m_pLiftList->GetLift(m_iSelectedLiftID)->GetStarSeparation()));
		m_DeckHeightEditCtrl.SetWindowText(sValue);
		m_pLiftList->SetRL(m_iSelectedLiftID, dNewDeckHeight);		
	}
}

void LiftDetailsDialog::DeleteAllHandrailComponents(SideOfBayEnum eSide)
{
	if( m_pSelectedLift==NULL )
	{
		int		iLift;
		Lift	*pSelectedLift;
		for( iLift=0; iLift<m_pBaySelectedLift->GetNumberOfLifts(); iLift++ )
		{
			pSelectedLift = m_pBaySelectedLift->GetLift( iLift );
			DeleteAnyHandrailsMidrailsToeboards(eSide, pSelectedLift);
		}
	}
	else
	{
		DeleteAnyHandrailsMidrailsToeboards(eSide, m_pSelectedLift);
	}
}




void LiftDetailsDialog::GetAndSetHandrailStyle( Lift *pSelectedLift )
{
	/////////////////////////////////////////////////////////
	//Handrail style
	m_HandrailStyleComboCtrl.SetCurSel( 0 );
	if( AnyMeshFull( N, pSelectedLift) || 
		AnyMeshFull( E, pSelectedLift) || 
		AnyMeshFull( S, pSelectedLift) || 
		AnyMeshFull( W, pSelectedLift) )
	{
		m_HandrailStyleComboCtrl.SetCurSel( 2 );
	}
	else if(AnyMeshHalf( N, pSelectedLift) || 
			AnyMeshHalf( E, pSelectedLift) || 
			AnyMeshHalf( S, pSelectedLift) || 
			AnyMeshHalf( W, pSelectedLift) )
	{
		m_HandrailStyleComboCtrl.SetCurSel( 1 );
	}
	else
	{
		//Check the rest of the lifts to find one!
		int			iLift, iComp;
		bool		bHasFullMesh;
		Lift		*pLift;
		Component	*pComp;

		//Do any of the lifts have full mesh?
		bHasFullMesh = false;
		for( iLift=0; iLift<m_pBaySelectedLift->GetNumberOfLifts(); iLift++ )
		{
			pLift = m_pBaySelectedLift->GetLift( iLift );

			for( iComp=0; iComp<pLift->GetNumberOfComponents(); iComp++ )
			{
				if( pLift->GetComponentRise( iComp )==LIFT_RISE_2000MM )
				{
					pComp = pLift->GetComponent( iComp );
					if( pComp->GetType()==CT_MESH_GUARD )
					{
						bHasFullMesh = true;
					}
				}
			}
		}
		if( bHasFullMesh )
		{
			m_HandrailStyleComboCtrl.SetCurSel( 2 );
		}
		else if((m_pBaySelectedLift->HasComponentOfTypeOnSide( CT_MESH_GUARD, N )!=NULL) ||
				(m_pBaySelectedLift->HasComponentOfTypeOnSide( CT_MESH_GUARD, E )!=NULL) ||
				(m_pBaySelectedLift->HasComponentOfTypeOnSide( CT_MESH_GUARD, S )!=NULL) ||
				(m_pBaySelectedLift->HasComponentOfTypeOnSide( CT_MESH_GUARD, W )!=NULL) )
		{
			m_HandrailStyleComboCtrl.SetCurSel( 1 );
		}
	}
}

void LiftDetailsDialog::OnMoveLiftUp()
{
	MoveLift( true );
}

void LiftDetailsDialog::OnMoveLiftDown()
{
	MoveLift( false );
}

void LiftDetailsDialog::MoveLift(bool bUp)
{
	if( m_pSelectedLift!=NULL )
	{
		Bay	*pBay;
		pBay = m_pSelectedLift->GetBayPointer();
		;//assert( pBay!=NULL );
		if( pBay->MoveLift( m_pSelectedLift->GetLiftID(), bUp ) )
		{
			GetCurrentSettings();

			m_iSelectedLiftComboIndex+= (bUp)? -1: 1;
			m_LiftSelectionComboCtrl.SetCurSel(m_iSelectedLiftComboIndex);
			OnSelchangeLiftSelectionCombo();
		}
	}
	GetCurrentSettings();
	m_LiftSelectionComboCtrl.SetCurSel(m_iSelectedLiftComboIndex);
}

void LiftDetailsDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("LiftDetailsDialog") );
}

void LiftDetailsDialog::OnSelchangeHandrailStyleCombo() 
{
	bool	bAskQuestion;
	CString sMsg;
	HandrailTypeEnum eHandrailType;

	UpdateData( );

	if( AnyHandrail( N, m_pSelectedLift ) ||
		AnyHandrail( S, m_pSelectedLift ) ||
		AnyHandrail( E, m_pSelectedLift ) ||
		AnyHandrail( W, m_pSelectedLift ) )
	{
		bAskQuestion = false;

		eHandrailType = (HandrailTypeEnum)m_HandrailStyleComboCtrl.GetItemData(m_HandrailStyleComboCtrl.GetCurSel());
		switch (eHandrailType)
		{
		default:
			;//assert( false );
			break;
		case CT_HANDRAIL_MIDRAIL_TB :
			if( AnyMeshFull( N, m_pSelectedLift ) ||
				AnyMeshFull( S, m_pSelectedLift ) ||
				AnyMeshFull( E, m_pSelectedLift ) ||
				AnyMeshFull( W, m_pSelectedLift ) ||
				AnyMeshHalf( N, m_pSelectedLift ) ||
				AnyMeshHalf( S, m_pSelectedLift ) ||
				AnyMeshHalf( E, m_pSelectedLift ) ||
				AnyMeshHalf( W, m_pSelectedLift ) )
			{
				bAskQuestion = true;
			}
			break;
		case CT_HANDRAIL_FULLMESH	:
			if( AnyMidrail( N, m_pSelectedLift ) ||
				AnyMidrail( S, m_pSelectedLift ) ||
				AnyMidrail( E, m_pSelectedLift ) ||
				AnyMidrail( W, m_pSelectedLift ) ||
				AnyMeshHalf( N, m_pSelectedLift ) ||
				AnyMeshHalf( S, m_pSelectedLift ) ||
				AnyMeshHalf( E, m_pSelectedLift ) ||
				AnyMeshHalf( W, m_pSelectedLift ) )
			{
				bAskQuestion = true;
			}
			break;
		case CT_HANDRAIL_HALFMESH	:
			if( AnyMidrail( N, m_pSelectedLift ) ||
				AnyMidrail( S, m_pSelectedLift ) ||
				AnyMidrail( E, m_pSelectedLift ) ||
				AnyMidrail( W, m_pSelectedLift ) ||
				AnyMeshFull( N, m_pSelectedLift ) ||
				AnyMeshFull( S, m_pSelectedLift ) ||
				AnyMeshFull( E, m_pSelectedLift ) ||
				AnyMeshFull( W, m_pSelectedLift ) )
			{
				bAskQuestion = true;
			}
			break;
		}

		if( bAskQuestion )
		{
			sMsg = _T("Would you like to apply this change to all the handrails?");
			if( MessageBox( sMsg, _T("Handrail Change"), MB_YESNO|MB_ICONQUESTION )==IDYES )
			{
				//Reapply the handrails
				OnNorthHandrailButton();
				OnSouthHandrailButton();
				OnEastHandrailButton();
				OnWestHandrailButton();
			}
		}
	}
}
