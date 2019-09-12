// BayDetailsDialog.cpp : implementation file
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
#include "BayDetailsDialog.h"
#include "Bay.h"
#include "Run.h"
#include "Controller.h"
#include "StandardHeightPickerDialog.h"
//#include "Afxwin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum SoleBoardComboSelection
{
	SOLEBOARD_SELECTION_NONE,
	SOLEBOARD_SELECTION_500,
	SOLEBOARD_SELECTION_1500
};


enum StagePositioningEnum
{
	STAGE_POS_SOUTH_ONLY,
	STAGE_POS_DECKS_ONLY
};

const CString BAY_DIM_MULTI_SELECTION_TEXT = _T("<Multi Select>");

/////////////////////////////////////////////////////////////////////////////
// BayDetailsDialog dialog

//#define NO_DECK_STRING_DEFINE "No Deck"

//BayDetailsDialog::BayDetailsDialog(CWnd* pParent)
//	//: CDialog(BayDetailsDialog::IDD, pParent)
//	//, m_editShape(_T(""))
//	//, m_cboSize1(_T(""))
//	//, m_cboSize2(_T(""))
//	//, m_cboSize3(_T(""))
//	//, m_ctrlBitmapImg(0)
//{
//	m_bHandOptionsChain = FALSE;
//	m_bHandOptionsShadecloth = FALSE;
//	m_bStaggeredHeights = FALSE;
//	m_iCourseFitRadio = -1;
//	m_iRailOptions = HRO_INVALID;
//	m_sNumberOfStages = _T("");
//	m_iStagePositioning = STAGE_POS_SOUTH_ONLY;
//	m_bSoleboardNE = FALSE;
//	m_bSoleboardSE = FALSE;
//	m_bSoleboardSW = FALSE;
//	m_bSoleboardNW = FALSE;
//	m_sStandardRLNW = _T("");
//	m_sStandardRLNE = _T("");
//	m_sStandardRLSE = _T("");
//	m_sStandardRLSW = _T("");
//	m_bFullMeshDecksOnly = FALSE;
//	m_bLedgersEveryMetre = FALSE;
//	m_bUseMidrailWithChain = FALSE;
//	m_bMovingDeck = FALSE;
//	m_iRoofProtection = 0;
//	m_bMatchDecksToLevels = FALSE;
//	//}}AFX_DATA_INIT
//
//	m_pParent = pParent;
//	m_nID = BayDetailsDialog::IDD;
//	m_pBay = NULL;
//	SetApplyRequired( false );
//	SetShowApplyWarning( false );
//
////	m_pBayCopy = new Bay();
////	*m_pBayCopy = *pBay;
//	m_bStandardsManuallyChanged = false;
//	m_btnFullMeshDecksOnly.EnableWindow( false );
//	m_bFullMeshDecksOnly = false;
//}
//////////////////////////////////////////////////////////////////////
//Constructor
//

BayDetailsDialog::BayDetailsDialog(CWnd* pParent /*=NULL*/, Bay *pBay)
	: KwikscafDialog(BayDetailsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(BayDetailsDialog)
	m_bHandOptionsChain = FALSE;
	m_bHandOptionsShadecloth = FALSE;
	m_bStaggeredHeights = FALSE;
	m_iCourseFitRadio = -1;
	m_iRailOptions = HRO_INVALID;
	m_sNumberOfStages = _T("");
	m_iStagePositioning = STAGE_POS_SOUTH_ONLY;
	m_bSoleboardNE = FALSE;
	m_bSoleboardSE = FALSE;
	m_bSoleboardSW = FALSE;
	m_bSoleboardNW = FALSE;
	m_sStandardRLNW = _T("");
	m_sStandardRLNE = _T("");
	m_sStandardRLSE = _T("");
	m_sStandardRLSW = _T("");
	m_bFullMeshDecksOnly = FALSE;
	m_bLedgersEveryMetre = FALSE;
	m_bUseMidrailWithChain = FALSE;
	m_bMovingDeck = FALSE;
	m_iRoofProtection = 0;
	m_bMatchDecksToLevels = FALSE;
	//}}AFX_DATA_INIT

	m_pParent = pParent;
	m_nID = BayDetailsDialog::IDD;
	m_pBay = pBay;
	SetApplyRequired( false );
	SetShowApplyWarning( false );

//	m_pBayCopy = new Bay();
//	*m_pBayCopy = *pBay;
	m_bStandardsManuallyChanged = false;
	m_btnFullMeshDecksOnly.EnableWindow( false );
	m_bFullMeshDecksOnly = false;
}
 
//////////////////////////////////////////////////////////////////////
//Destructor
//
BayDetailsDialog::~BayDetailsDialog()
{
	m_pBay->DeleteLiftDetailsDialog();		
}

//////////////////////////////////////////////////////////////////////
//Create
//
BOOL BayDetailsDialog::Create()
{
	return KwikscafDialog::Create(m_nID, m_pParent);
}

//////////////////////////////////////////////////////////////////////
//DoDataExchange
//
void BayDetailsDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BayDetailsDialog)
	DDX_Control(pDX, IDC_STAGES_POSITON_SOUTH, m_ctrlSouthSideOnly);
	DDX_Control(pDX, IDC_MATCH_DECK_TO_LEVELS, m_ctrlMatchDecksToLevels);
	DDX_Control(pDX, IDC_OK_BUTTON_CTRL, m_btnOK);
	DDX_Control(pDX, IDC_APPLY_BUTTON, m_ApplyButton);
	DDX_Control(pDX, IDC_USE_MIDRAIL_WITH_CHAIN, m_btnUseMidrailWithChain);
	DDX_Control(pDX, IDC_HANDOPTIONS_FULL_MESH_DECKS_ONLY, m_btnFullMeshDecksOnly);
	DDX_Control(pDX, IDC_MATERIAL_TYPE_COMBO, m_MaterialTypeCombo);
	DDX_Control(pDX, IDC_BAY_WIDTH_COMBO2, m_BayWidthComboCtrl);
	DDX_Control(pDX, IDC_BAY_LENGTH_COMBO2, m_BayLengthComboCtrl);
	DDX_Control(pDX, IDC_NUMBER_STAGES_EDIT, m_NumberOfStagesEditCtrl);
	DDX_Control(pDX, IDC_APPLY_SELECTION_BUTTON, m_ApplyToSelectionButtonCtrl);
	DDX_Control(pDX, IDC_NUMBER_DECKS_EDIT, m_NumberOfDecksEditCtrl);
	DDX_Control(pDX, IDC_TOP_WORKING_HEIGHT_EDIT, m_TopWorkingHeightEditCtrl);
	DDX_Control(pDX, IDC_STAGGERED_HEIGHTS_CHECK, m_StaggeredHeightsButtonCtrl);
	DDX_Control(pDX, IDC_STAGE_BRACKET_WIDTHS_COMBO, m_StageBracketWidthsComboCtrl);
 	DDX_Control(pDX, IDC_TOP_WORKING_HEIGHT_SPIN, m_TopWorkingHeightSpinCtrl);
	DDX_Control(pDX, IDC_NUMBER_DECKS_SPIN, m_NumberOfDecksSpinCtrl);
	DDX_Control(pDX, IDC_NUMBER_STAGES_SPIN, m_NumberOfStagesSpinCtrl);
	DDX_Check(pDX, IDC_HANDOPTIONS_CHAIN_CHECK, m_bHandOptionsChain);
	DDX_Check(pDX, IDC_HANDOPTIONS_SHADECLOTH_CHECK, m_bHandOptionsShadecloth);
	DDX_Check(pDX, IDC_STAGGERED_HEIGHTS_CHECK, m_bStaggeredHeights);
	DDX_Radio(pDX, IDC_COURSE_FIT_RADIO, m_iCourseFitRadio);
	DDX_Radio(pDX, IDC_RAILOPTIONS_HAND_RADIO, m_iRailOptions);
	DDX_Text(pDX, IDC_NUMBER_STAGES_EDIT, m_sNumberOfStages);
	DDX_Radio(pDX, IDC_STAGES_POSITON_SOUTH, m_iStagePositioning);
	DDX_Control(pDX, IDC_NE_STANDARD_EDIT, m_StandardEditCtrlNE);
	DDX_Control(pDX, IDC_SE_STANDARD_EDIT, m_StandardEditCtrlSE);
	DDX_Control(pDX, IDC_SW_STANDARD_EDIT, m_StandardEditCtrlSW);
	DDX_Control(pDX, IDC_NW_STANDARD_EDIT, m_StandardEditCtrlNW);
	DDX_Control(pDX, IDC_NE_SOLEBOARD_CHECK, m_SoleboardCheckCtrlNE);
	DDX_Control(pDX, IDC_SE_SOLEBOARD_CHECK, m_SoleboardCheckCtrlSE);
	DDX_Control(pDX, IDC_SW_SOLEBOARD_CHECK, m_SoleboardCheckCtrlSW);
	DDX_Control(pDX, IDC_NW_SOLEBOARD_CHECK, m_SoleboardCheckCtrlNW);
	DDX_Control(pDX, IDC_NE_STANDARD_LBOX, m_dStandardListCtrlNE);
	DDX_Control(pDX, IDC_SE_STANDARD_LBOX, m_dStandardListCtrlSE);
	DDX_Control(pDX, IDC_SW_STANDARD_LBOX, m_dStandardListCtrlSW);
	DDX_Control(pDX, IDC_NW_STANDARD_LBOX, m_dStandardListCtrlNW);
	DDX_Check(pDX, IDC_NE_SOLEBOARD_CHECK, m_bSoleboardNE);
	DDX_Check(pDX, IDC_SE_SOLEBOARD_CHECK, m_bSoleboardSE);
	DDX_Check(pDX, IDC_SW_SOLEBOARD_CHECK, m_bSoleboardSW);
	DDX_Check(pDX, IDC_NW_SOLEBOARD_CHECK, m_bSoleboardNW);
	DDX_Text(pDX, IDC_NW_STANDARD_RL, m_sStandardRLNW);
	DDX_Text(pDX, IDC_NE_STANDARD_RL, m_sStandardRLNE);
	DDX_Text(pDX, IDC_SE_STANDARD_RL, m_sStandardRLSE);
	DDX_Text(pDX, IDC_SW_STANDARD_RL, m_sStandardRLSW);
	DDX_Check(pDX, IDC_HANDOPTIONS_FULL_MESH_DECKS_ONLY, m_bFullMeshDecksOnly);
	DDX_Check(pDX, IDC_LEDGERS_EVERY_METRE, m_bLedgersEveryMetre);
	DDX_Check(pDX, IDC_USE_MIDRAIL_WITH_CHAIN, m_bUseMidrailWithChain);
	DDX_Check(pDX, IDC_MOVING_DECK, m_bMovingDeck);
	DDX_Radio(pDX, IDC_ROOF_PROTECTION, m_iRoofProtection);
	DDX_Check(pDX, IDC_MATCH_DECK_TO_LEVELS, m_bMatchDecksToLevels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BayDetailsDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(BayDetailsDialog)
	ON_BN_CLICKED(IDC_BAY_DETAILS_BUTTON, OnBayDetailsCancelButton)
	ON_BN_CLICKED(IDC_LIFTDETAILS_BUTTON, OnLiftdetailsButton)
	ON_BN_CLICKED(IDC_NW_SOLEBOARD_CHECK, OnNwSoleboardCheck)
	ON_BN_CLICKED(IDC_SE_SOLEBOARD_CHECK, OnSeSoleboardCheck)
	ON_BN_CLICKED(IDC_SW_SOLEBOARD_CHECK, OnSwSoleboardCheck)
	ON_BN_CLICKED(IDC_NE_SOLEBOARD_CHECK, OnNeSoleboardCheck)
	ON_BN_CLICKED(IDC_COURSE_FIT_RADIO, OnCourseFitRadio)
	ON_BN_CLICKED(IDC_FINE_FIT_RADIO, OnFineFitRadio)
	ON_BN_CLICKED(IDC_SAME_FIT_RADIO, OnSameFitRadio)
	ON_BN_CLICKED(IDC_STAGGERED_HEIGHTS_CHECK, OnStaggeredHeightsCheck)
	ON_BN_CLICKED(IDC_APPLY_SELECTION_BUTTON, OnApplySelectionButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TOP_WORKING_HEIGHT_SPIN, OnDeltaposTopWorkingHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NUMBER_DECKS_SPIN, OnDeltaposNumberDecksSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NUMBER_STAGES_SPIN, OnDeltaposNumberStagesSpin)
	ON_BN_CLICKED(IDC_HANDOPTIONS_MESH_RADIO, OnHandoptionsMeshRadio)
	ON_BN_CLICKED(IDC_HANDOPTIONS_FULLMESH_RADIO, OnHandoptionsFullmeshRadio)
	ON_BN_CLICKED(IDC_HANDOPTIONS_SHADECLOTH_CHECK, OnHandoptionsShadeclothCheck)
	ON_BN_CLICKED(IDC_HANDOPTIONS_CHAIN_CHECK, OnHandoptionsChainCheck)
	ON_EN_CHANGE(IDC_TOP_WORKING_HEIGHT_EDIT, OnChangeTopWorkingHeightEdit)
	ON_EN_KILLFOCUS(IDC_TOP_WORKING_HEIGHT_EDIT, OnKillfocusTopWorkingHeightEdit)
	ON_EN_KILLFOCUS(IDC_NUMBER_DECKS_EDIT, OnKillfocusNumberDecksEdit)
	ON_EN_KILLFOCUS(IDC_NUMBER_STAGES_EDIT, OnKillfocusNumberStagesEdit)
	ON_CBN_SELCHANGE(IDC_BAY_LENGTH_COMBO2, OnSelchangeBayLengthCombo2)
	ON_CBN_SELCHANGE(IDC_BAY_WIDTH_COMBO2, OnSelchangeBayWidthCombo2)
	ON_LBN_DBLCLK(IDC_NE_STANDARD_LBOX, OnDblclkNeStandardLbox)
	ON_LBN_DBLCLK(IDC_SE_STANDARD_LBOX, OnDblclkSeStandardLbox)
	ON_LBN_DBLCLK(IDC_SW_STANDARD_LBOX, OnDblclkSwStandardLbox)
	ON_LBN_DBLCLK(IDC_NW_STANDARD_LBOX, OnDblclkNwStandardLbox)
	ON_CBN_SELCHANGE(IDC_STAGE_BRACKET_WIDTHS_COMBO, OnSelchangeStageBracketWidthsCombo)
	ON_EN_CHANGE(IDC_NUMBER_DECKS_EDIT, OnChangeNumberDecksEdit)
	ON_EN_CHANGE(IDC_NUMBER_STAGES_EDIT, OnChangeNumberStagesEdit)
	ON_CBN_EDITCHANGE(IDC_MATERIAL_TYPE_COMBO, OnEditchangeDeckMaterialTypeCombo)
	ON_BN_CLICKED(IDC_STAGES_POSITON_DECKS, OnStagesPositonDecks)
	ON_BN_CLICKED(IDC_STAGES_POSITON_SOUTH, OnStagesPositonSouth)
 //ON_MESSAGE(WM_ACAD_KEEPFOCUS, onAcadKeepFocus )
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_HANDOPTIONS_FULL_MESH_DECKS_ONLY, OnHandoptionsFullMeshDecksOnly)
	ON_BN_CLICKED(IDC_USE_MIDRAIL_WITH_CHAIN, OnUseMidrailWithChain)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_RAILOPTIONS_HAND_RADIO, OnRailoptionsHandRadio)
	ON_BN_CLICKED(IDC_LEDGERS_EVERY_METRE, OnLedgersEveryMetre)
	ON_BN_CLICKED(IDC_OK_BUTTON_CTRL, OnOkButtonCtrl)
	ON_BN_CLICKED(IDC_MOVING_DECK, OnMovingDeck)
	ON_BN_CLICKED(IDC_ROOF_PROTECTION, OnRoofProtection)
	ON_BN_CLICKED(IDC_ROOF_PROTECTION_1500, OnRoofProtection1500)
	ON_BN_CLICKED(IDC_ROOF_PROTECTION_2000, OnRoofProtection2000)
	ON_WM_SHOWWINDOW()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_MATCH_DECK_TO_LEVELS, OnMatchDeckToLevels)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
 
/////////////////////////////////////////////////////////////////////////////
// BayDetailsDialog message handlers

//////////////////////////////////////////////////////////////////////
//onAcadKeepFocus
//
afx_msg LONG BayDetailsDialog::onAcadKeepFocus( UINT, LONG ) 
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//OnBayDetailsCancelButton
//
void BayDetailsDialog::OnBayDetailsCancelButton() 
{
	EndDialog( CANCELLED );
//	KwikscafDialog::OnCancel();
//	ShowWindow(SW_HIDE);
//	m_pParent->EnableWindow(true);
}

//////////////////////////////////////////////////////////////////////
//OnLiftDetailsButton
//
void BayDetailsDialog::OnLiftdetailsButton() 
{
	double dBefore, dAfter;

	if( OkToExit() )
	{
		dBefore = GetTopDeckHeightFromEdit();
		m_pBay->DisplayLiftDetailsDialog(this);

		GetCurrentSettings();
		SetApplyRequired( false );

		//We may have to adjust the standards to fit this lift!
		dAfter=  GetTopDeckHeightFromEdit();
		if( dAfter<dBefore-ROUND_ERROR || dAfter>dBefore+ROUND_ERROR )
		{
			if( m_bStandardsManuallyChanged )
			{
				CString sMsg, sTemp;
        sMsg = _T("Warning: your top working height has been changed during");
				sMsg+= _T("the Lift Details operation, would you like me to adjust");
				sMsg+= _T("your standard arrangement to suit the new height?");
				int iRet = MessageBox( sMsg, _T("Warning!"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON1 );
				if( iRet!=IDNO )
				{
					RecreateAllStandardsArrangments();
					GetCurrentSettings();
				}
			}
			else
			{
				RecreateAllStandardsArrangments();
				GetCurrentSettings();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//OnInitDialog
//
BOOL BayDetailsDialog::OnInitDialog() 
{
	int			i, iIndex;
	bool		bFound;
	Lift		*pLift;
	Component	*pComponent;

	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("BayDetailsDialog"));

	LoadDefaults();
	FillBayDimensionCombos();
	GetCurrentSettings();

	m_btnOK.ShowWindow(SW_HIDE);
	m_ApplyToSelectionButtonCtrl.ShowWindow(SW_HIDE);

	m_bHandOptionsChain = false;
	if( m_pBay->DoesABayComponentExistOnASide( CT_CHAIN_LINK, NORTH) ||
		m_pBay->DoesABayComponentExistOnASide( CT_CHAIN_LINK, EAST)	 ||
		m_pBay->DoesABayComponentExistOnASide( CT_CHAIN_LINK, SOUTH) ||
		m_pBay->DoesABayComponentExistOnASide( CT_CHAIN_LINK, WEST) )
	{
		m_bHandOptionsChain = true;
	}
	m_btnUseMidrailWithChain.EnableWindow( m_bHandOptionsChain );

	m_bHandOptionsShadecloth = false;
	if( m_pBay->DoesABayComponentExistOnASide( CT_SHADE_CLOTH, NORTH) ||
		m_pBay->DoesABayComponentExistOnASide( CT_SHADE_CLOTH, EAST)  ||
		m_pBay->DoesABayComponentExistOnASide( CT_SHADE_CLOTH, SOUTH) ||
		m_pBay->DoesABayComponentExistOnASide( CT_SHADE_CLOTH, WEST) )
		m_bHandOptionsShadecloth = true;

	//////////////////////////////////////////////////
	//Does this have a handrail?
	m_iRailOptions = GetBaysRailSetting();

	m_bLedgersEveryMetre	= ConvertboolToBOOL( m_pBay->GetUseLedgerEveryMetre() );
	m_bUseMidrailWithChain	= ConvertboolToBOOL( m_pBay->GetUseMidrailWithChainMesh() );
	m_bMovingDeck			= ConvertboolToBOOL( m_pBay->HasMovingDeck() );
	;//assert( m_pBay->GetRoofProtection()>=0 );
	m_iRoofProtection		= (int)m_pBay->GetRoofProtection();

	UpdateData( false );

	//////////////////////////////////////////////////
	//Fill decking material type combo
	m_MaterialTypeCombo.Clear();
	m_MaterialTypeCombo.ResetContent();
//	iIndex = m_MaterialTypeCombo.AddString(NO_DECK_STRING_DEFINE);
//	m_MaterialTypeCombo.SetItemData(iIndex, MT_OTHER);
	iIndex = m_MaterialTypeCombo.AddString(GetComponentMaterialStr( MT_STEEL ));
	m_MaterialTypeCombo.SetItemData(iIndex, MT_STEEL);
	iIndex = m_MaterialTypeCombo.AddString(GetComponentMaterialStr( MT_TIMBER ));
	m_MaterialTypeCombo.SetItemData(iIndex, MT_TIMBER);

	//////////////////////////////////////////////////
	// set material type for decking
	bFound = false;
	for( i=0; i<m_pBay->GetNumberOfLifts(); i++ )
	{
		pLift = m_pBay->GetLift( m_pBay->GetNumberOfLifts()-1 );
		pComponent = pLift->GetComponent( CT_DECKING_PLANK, 0, SOUTH );
		if( pComponent!=NULL )
		{
			bFound = true;
			break;
		}
	}
	if( bFound )
		m_MaterialTypeCombo.SelectString(0, GetComponentMaterialStr( pComponent->GetMaterialType() ));
	else
		m_MaterialTypeCombo.SelectString(0, GetComponentMaterialStr( MT_STEEL ) );

	//////////////////////////////////////////////////
	//if this is an autobuild then we don't want them to edit the standards
	int		iRunID;
	bool	bIsAutobuild;
	iRunID = m_pBay->GetRunPointer()->GetController()->GetRunID( m_pBay->GetRunPointer() );
	bIsAutobuild = ( (iRunID==ID_NONE_MATCHING) || (iRunID==ID_NO_IDS) );

#ifdef	ALLOW_STANDARD_EDIT
	if (bIsAutobuild)
	{
		EnableStandardEdits( FALSE );
	}
#else
	EnableStandardEdits( FALSE );
#endif	//#ifdef	ALLOW_STANDARD_EDIT

	m_ctrlSouthSideOnly.EnableWindow( TRUE );
	m_ctrlMatchDecksToLevels.EnableWindow(FALSE);
	SetMatchDecksToLevels(true);
	if( m_pBay!=NULL &&
		m_pBay->GetBayType()==BAY_TYPE_BAY &&
		m_pBay->GetRunPointer()!=NULL &&
		m_pBay->GetRunPointer()->GetController()!=NULL )
	{
		LevelList *pllLevels, llDecks;
		pllLevels = m_pBay->GetRunPointer()->GetController()->GetLevelList();
		llDecks = *pllLevels;

		//Convert the levels to the nearest deck height below the level
		double dDeck;
		for( i=0; i<llDecks.GetSize(); i++ )
		{
			dDeck = llDecks.GetLevel(i);
			llDecks.RemoveLevel(i);
			dDeck = double(int(dDeck/GetStarSeparation())*GetStarSeparation());
			llDecks.AddLevel(dDeck);
		}

		//The spacing between the 'Deck Levels' must be tall enought to stand in
		if( llDecks.GetMinimumSpacing()>=4.00*GetStarSeparation()-ROUND_ERROR)
		{
			if( pllLevels->GetSize()>0 )
			{
				//There are some levels, so we can match them to this bay's lifts!
				m_ctrlMatchDecksToLevels.EnableWindow(TRUE);
				m_ctrlSouthSideOnly.EnableWindow( FALSE );

				////////////////////////////////////////////////////////////////////////
				//Go through each decking lift and check if there is a matching
				//	level RL within 500mm above the lift's RL, if we find a lift
				//	that does not have a matching level line then this is not matched!
				int		iLift;
				bool	bNotFound, bFound;
				Lift	*pLift;
				double	dLevelRL, dLiftRL;
				//flag it that we haven't found a match yet
				bNotFound = true;
				for( iLift=0; iLift<m_pBay->GetNumberOfLifts(); iLift++ )
				{
					pLift = m_pBay->GetLift(iLift);
					if( pLift->IsDeckingLift() )
					{
						dLiftRL = pLift->GetRL();
						bFound = false;
						for( i=0; i<pllLevels->GetSize() && bNotFound; i++ )
						{
							dLevelRL = pllLevels->GetLevel(i);
							if( dLiftRL<dLevelRL+ROUND_ERROR &&
								dLiftRL>dLevelRL-GetStarSeparation()-ROUND_ERROR )
							{
								//We have found a level that matches this deck lift
								bFound = true;
								bNotFound = false;
								break;
							}
						}
						if( !bFound )
						{
							//This decking lift does not have a matching level line
							bNotFound = true;
							break;
						}
					}
				}
				if( bNotFound )
				{
					//We either hand no matchs at all or not all of
					//	the lifts had a matching level line
					SetMatchDecksToLevels(false);
				}
				else
				{
					//All the decking lifts had a matching level
					SetMatchDecksToLevels(true);
				}
			}
			else
			{
				//it is not possible to create decking since the levels are too far appart
				acutPrintf( _T("\nMatching decks to levels not possible!") );
				acutPrintf( _T("\nSince there are no levels") );
				SetMatchDecksToLevels(false);
			}
		}
		else
		{
			//it is not possible to create decking since the levels are too far appart
			acutPrintf( _T("\nMatching decks to levels not possible!") );
			acutPrintf( _T("\nSince some levels, after dropping to nearest star,") );
			acutPrintf( _T("\nare within 2.0m of each other") );
			SetMatchDecksToLevels(false);
		}
	}
	else
	{
		SetMatchDecksToLevels(false);
	}

	//////////////////////////////////////////////////
	//no changes yet
	SetApplyRequired( false );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////
// LoadDefaults
//
void BayDetailsDialog::LoadDefaults()
{
	if (m_pBay->GetStaggeredHeights())
		m_bStaggeredHeights = true;
	else
		m_bStaggeredHeights = false;

	switch(m_pBay->GetStandardFit() )
	{
	case( STD_FIT_COURSE ):
		m_iCourseFitRadio = 0;
		break;
	case( STD_FIT_FINE ):
		m_iCourseFitRadio = 1;
		break;
	case( STD_FIT_SAME ):
		m_iCourseFitRadio = 2;
		break;
	default:
		;//assert( false );
	}

	//height is relative to the bay so soleboards are always at zero height
	m_bSoleboardNE = m_pBay->DoesABayComponentExistOnASide( CT_SOLEBOARD, NORTH_EAST);
	m_bSoleboardSE = m_pBay->DoesABayComponentExistOnASide( CT_SOLEBOARD, SOUTH_EAST);
	m_bSoleboardSW = m_pBay->DoesABayComponentExistOnASide( CT_SOLEBOARD, SOUTH_WEST);
	m_bSoleboardNW = m_pBay->DoesABayComponentExistOnASide( CT_SOLEBOARD, NORTH_WEST);

	int iIndex;
	m_StageBracketWidthsComboCtrl.Clear();
	m_StageBracketWidthsComboCtrl.ResetContent();
	iIndex = m_StageBracketWidthsComboCtrl.AddString(_T("No Planks"));
	m_StageBracketWidthsComboCtrl.SetItemData(iIndex, 0);
	iIndex = m_StageBracketWidthsComboCtrl.AddString(_T("1 Plank"));
	m_StageBracketWidthsComboCtrl.SetItemData(iIndex, NUM_PLANKS_FOR_STAGE_BRD_1_WIDE);
	iIndex = m_StageBracketWidthsComboCtrl.AddString(_T("2 Planks"));
	m_StageBracketWidthsComboCtrl.SetItemData(iIndex, NUM_PLANKS_FOR_STAGE_BRD_2_WIDE);
	iIndex = m_StageBracketWidthsComboCtrl.AddString(_T("3 Planks"));
	m_StageBracketWidthsComboCtrl.SetItemData(iIndex, NUM_PLANKS_FOR_STAGE_BRD_3_WIDE);

	int		iNumberOfStages, iBracketWidth;
	CString	sText;
	iNumberOfStages = m_pBay->GetNumberOfHopupsInBay( iBracketWidth );
	iBracketWidth = max( iBracketWidth, 0 );
	m_StageBracketWidthsComboCtrl.SetCurSel(iBracketWidth);  // defaults

	m_sStandardRLNE.Format( _T("%0.2f"), m_pBay->GetStandardPosition( CNR_NORTH_EAST ).z );
	m_sStandardRLSE.Format( _T("%0.2f"), m_pBay->GetStandardPosition( CNR_SOUTH_EAST ).z );
	m_sStandardRLSW.Format( _T("%0.2f"), m_pBay->GetStandardPosition( CNR_SOUTH_WEST ).z );
	m_sStandardRLNW.Format( _T("%0.2f"), m_pBay->GetStandardPosition( CNR_NORTH_WEST ).z );

	m_sNumberOfStages.Format( _T("%i"), iNumberOfStages );
}

//////////////////////////////////////////////////////////////////////
//GetCurrentSettings
//
void BayDetailsDialog::GetCurrentSettings()
{
	bool	bApplySetting;
	double	dTopLiftHeight;
	CString sTopLiftRL;
	CString sNumberOfDecks;
	CString sLength;

	//Don't let this function change the apply setting
	bApplySetting = IsApplyRequired();

	UpdateAllStandards();

	m_pBay->GetLiftRL( m_pBay->GetNumberOfLifts()-1, m_dTopWorkingRL );	//get RL of top lift
	dTopLiftHeight = m_dTopWorkingRL - GetTopBottomStdRL();
	SetTopDeckHeight( dTopLiftHeight );

	SetNumberOfDecksInEdit(GetCurrentNumberOfDecks());

	sLength.Format(_T("%.2f"), m_pBay->GetBayLength());
	if( m_pBay->GetBayLength()<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		m_pBay->GetBayLength()>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		sLength.Format( _T("%0.2f"), COMPONENT_LENGTH_0800 );

	if( !m_pBay->WasBayLengthChangedDuringEdit() &&
		m_pBay->IsBaySizeMultipleSelection() )
	{
		m_BayLengthComboCtrl.SetWindowText( BAY_DIM_MULTI_SELECTION_TEXT );
	}
	else
	{
		m_BayLengthComboCtrl.SelectString( 0, sLength );
	}

	sLength.Format(_T("%.2f"), m_pBay->GetBayWidth());
	if( m_pBay->GetBayWidth()<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		m_pBay->GetBayWidth()>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		sLength.Format( _T("%0.2f"), COMPONENT_LENGTH_0800 );

	if( !m_pBay->WasBayWidthChangedDuringEdit() &&
		m_pBay->IsBaySizeMultipleSelection() )
	{
		m_BayWidthComboCtrl.SetWindowText( BAY_DIM_MULTI_SELECTION_TEXT );
	}
	else
	{
		m_BayWidthComboCtrl.SelectString( 0, sLength );
	}

	//restore the apply setting
	SetApplyRequired(bApplySetting);
}

//////////////////////////////////////////////////////////////////////
//OnNwSoleboardCheck
//
void BayDetailsDialog::OnNwSoleboardCheck() 
{
	ChangeMadeToDialog();
	if(m_SoleboardCheckCtrlNW.GetCheck() == 0)
	 	m_pBay->DeleteAllBayComponentsOfTypeFromSide(CT_SOLEBOARD, NORTH_WEST);
	else
		m_pBay->AddComponent(CT_SOLEBOARD, NORTH_WEST, 0.0, 0.0, MT_TIMBER);
}
 
//////////////////////////////////////////////////////////////////////
//OnNeSoleboardCheck
//
void BayDetailsDialog::OnNeSoleboardCheck() 
{
	ChangeMadeToDialog();
	if(m_SoleboardCheckCtrlNE.GetCheck() == 0)
		m_pBay->DeleteAllBayComponentsOfTypeFromSide(CT_SOLEBOARD, NORTH_EAST);
	else
		m_pBay->AddComponent(CT_SOLEBOARD, NORTH_EAST, 0.0, 0.0, MT_TIMBER);
}

//////////////////////////////////////////////////////////////////////
//OnSwSoleboardCheck
//
void BayDetailsDialog::OnSwSoleboardCheck() 
{
	ChangeMadeToDialog();
	if(m_SoleboardCheckCtrlSW.GetCheck() == 0)
		m_pBay->DeleteAllBayComponentsOfTypeFromSide(CT_SOLEBOARD, SOUTH_WEST);
	else
		m_pBay->AddComponent(CT_SOLEBOARD, SOUTH_WEST, 0.0, 0.0, MT_TIMBER);
}

//////////////////////////////////////////////////////////////////////
//OnSeSoleboardCheck
//
void BayDetailsDialog::OnSeSoleboardCheck() 
{
	ChangeMadeToDialog();
	if(m_SoleboardCheckCtrlSE.GetCheck() == 0)
		m_pBay->DeleteAllBayComponentsOfTypeFromSide(CT_SOLEBOARD, SOUTH_EAST);
	else
		m_pBay->AddComponent(CT_SOLEBOARD, SOUTH_EAST, 0.0, 0.0, MT_TIMBER);
}
 
//////////////////////////////////////////////////////////////////////
// OnCourseFitRadio
//
void BayDetailsDialog::OnCourseFitRadio() 
{
	UpdateData();

	if( !CheckBeforeOverwritingStandards() )
	{
		//reapply the settings without changeing them
		GetCurrentSettings();
		return;
	}

	ChangeMadeToDialog();
	m_pBay->SetStandardsFit(STD_FIT_COURSE);	

	RecreateAllStandardsArrangments();
	UpdateAllStandards();

	UpdateData( false );
}


//////////////////////////////////////////////////////////////////////
// OnFineFitRadio
//
void BayDetailsDialog::OnFineFitRadio() 
{
	UpdateData();

	if( !CheckBeforeOverwritingStandards() )
	{
		//reapply the settings without changeing them
		GetCurrentSettings();
		return;
	}

	ChangeMadeToDialog();
	m_pBay->SetStandardsFit(STD_FIT_FINE);

	RecreateAllStandardsArrangments();
	UpdateAllStandards();

	UpdateData( false );
}


//////////////////////////////////////////////////////////////////////
// OnSameFitRadio
//
void BayDetailsDialog::OnSameFitRadio() 
{
	UpdateData();

	if( !CheckBeforeOverwritingStandards() )
	{
		//reapply the settings without changeing them
		GetCurrentSettings();
		return;
	}

	ChangeMadeToDialog();
	m_pBay->SetStandardsFit(STD_FIT_SAME);	

	RecreateAllStandardsArrangments();
	UpdateAllStandards();

	UpdateData( false );
}


//////////////////////////////////////////////////////////////////////
// OnStaggeredHeightsCheck
//
void BayDetailsDialog::OnStaggeredHeightsCheck() 
{
	int iCheck;

	UpdateData();

	if( !CheckBeforeOverwritingStandards() )
	{
		//reapply the settings without changeing them
		GetCurrentSettings();
		return;
	}

	ChangeMadeToDialog();

	iCheck = m_StaggeredHeightsButtonCtrl.GetCheck();
	if (iCheck == 0)
		m_pBay->SetStaggeredHeights(false);
	else
		m_pBay->SetStaggeredHeights(true);

	RecreateAllStandardsArrangments();
	UpdateAllStandards();
}


//////////////////////////////////////////////////////////////////////
// UpdateStandardsArrangementCorner
//
void BayDetailsDialog::UpdateStandardsArrangementCorner(CListBox *pStandardsListBoxCtrl, CEdit *pStandardEditCtrl, CornerOfBayEnum eStandardCorner)
{
	int index;
	CString sHeight;
	doubleArray daArrangement;
	double dStandardHeight;
	double dTotalHeight;

	pStandardsListBoxCtrl->ResetContent();
	m_pBay->GetStandardsArrangement( daArrangement, eStandardCorner ); 
	index = daArrangement.GetSize()-1;
	dTotalHeight = 0;
	pStandardsListBoxCtrl->AddString(_T("<Add Std>"));
	while (index>=0)
	{	
		dStandardHeight = daArrangement.GetAt(index);
		dTotalHeight += dStandardHeight;
		sHeight.Format(_T("%.2f"), ConvertStarRLtoRL(dStandardHeight, m_pBay->GetStarSeparation()));
		pStandardsListBoxCtrl->AddString(sHeight);
		index--;
	}
	sHeight.Format(_T("%.2f"), ConvertStarRLtoRL(dTotalHeight, m_pBay->GetStarSeparation()));
	pStandardEditCtrl->SetWindowText(sHeight);
}


//////////////////////////////////////////////////////////////////////
// SetStandardsArrangementDirtyFlag
//
void BayDetailsDialog::SetStandardsArrangementDirtyFlag(bool dirty)
{
	BOOL EnableFlag;
	if (dirty != bStandardsArrangementDirty)
	{
		bStandardsArrangementDirty = dirty;
		if (bStandardsArrangementDirty)
			EnableFlag = TRUE;
		else
			EnableFlag = FALSE;

		m_StaggeredHeightsButtonCtrl.EnableWindow(EnableFlag);
		
#ifdef	ALLOW_STANDARD_EDIT
		EnableStandardEdits( EnableFlag );
#endif	//#ifdef	ALLOW_STANDARD_EDIT
	}
}


//////////////////////////////////////////////////////////////////////
// GetCurrentNumberOfDecks
//
int BayDetailsDialog::GetCurrentNumberOfDecks()
{
	int iNumberOfLifts;
	CString sLiftSelection;
	Lift *pLift;
	int iNumberOfDecks;

	iNumberOfDecks = 0;
	// fill selection list combo
	iNumberOfLifts = m_pBay->GetNumberOfLifts();
	for ( int index = 0; index < iNumberOfLifts; ++index )
	{
		pLift = m_pBay->GetLift(index);
		if( pLift->IsDeckingLift() )
			++iNumberOfDecks;
	}

	return iNumberOfDecks;
}


//////////////////////////////////////////////////////////////////////
// OnApplySelectionButton
//
void BayDetailsDialog::OnApplySelectionButton() 
{
	if( OkToExit() )
		EndDialog( APPLY_TO_SELECTION );
}

void BayDetailsDialog::OnOkButtonCtrl() 
{
	if( OkToExit() )
		EndDialog( OK_PRESSED );	
	
}

//////////////////////////////////////////////////////////////////////
// GetMaximumNumberOfDecks()
//
int BayDetailsDialog::GetMaximumNumberOfDecks()
{
	int     iMaxNumberOfDecks;
	if( m_bMatchDecksToLevels )
	{
		iMaxNumberOfDecks = GetPossibleDeckRLs().GetSize();
	}
	else
	{
		CString sTopWorkingHeight;
		double	dTopWorkingHeight;

		iMaxNumberOfDecks = 0;
		dTopWorkingHeight = GetTopDeckHeightFromEdit();

		if (dTopWorkingHeight > 0)
		{
			iMaxNumberOfDecks = 1 +(int)( dTopWorkingHeight / (4*GetStarSeparation())); // handrail 500mm, lift rise 2000mm
		}
	}
	return iMaxNumberOfDecks;
}


//////////////////////////////////////////////////////////////////////
// OnDeltaposTopWorkingHeightSpin
//
void BayDetailsDialog::OnDeltaposTopWorkingHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	double		dTopDeckHeight;
	NM_UPDOWN	*pNMUpDown = (NM_UPDOWN*)pNMHDR;

	SetApplyRequired( );
	ChangeMadeToDialog();
	dTopDeckHeight = GetTopDeckHeightFromEdit();

	dTopDeckHeight -= (GetStarSeparation()*pNMUpDown->iDelta);
	if (dTopDeckHeight<(3.00*GetStarSeparation())-ROUND_ERROR )
		dTopDeckHeight = (3.00*GetStarSeparation());
	SetTopDeckHeight(dTopDeckHeight);

	OnKillfocusTopWorkingHeightEdit(); // update other controls affected
	
	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////
// OnDeltaposNumberDecksSpin
//
void BayDetailsDialog::OnDeltaposNumberDecksSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int	iNumberOfDecks;

	SetApplyRequired( );
	ChangeMadeToDialog();
	iNumberOfDecks = GetNumberOfDecksFromEdit();

	if (pNMUpDown->iDelta < 0)
	{
		if (iNumberOfDecks < GetMaximumNumberOfDecks()) 
			iNumberOfDecks += 1;
		else
			MessageBeep(MB_ICONEXCLAMATION);
	}
	if (pNMUpDown->iDelta > 0)
	{
		if (iNumberOfDecks > 0 )
			iNumberOfDecks -= 1;
		else
			MessageBeep(MB_ICONEXCLAMATION);
	}

	SetNumberOfDecksInEdit(iNumberOfDecks);
	OnKillfocusNumberDecksEdit();
	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////
// OnDeltaposNumberStagesSpin
//
void BayDetailsDialog::OnDeltaposNumberStagesSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int	iNumberOfStages;

	UpdateData();
	SetApplyRequired( );
	ChangeMadeToDialog();
	iNumberOfStages = GetNumberOfStagesFromEdit();

	if (pNMUpDown->iDelta < 0)
	{
		if (iNumberOfStages < GetMaximumNumberOfStages())
			iNumberOfStages += 1;
	}
	if (pNMUpDown->iDelta > 0)
	{
		if (iNumberOfStages > 0 )
			iNumberOfStages -= 1;
	}

	SetNumberOfStagesInEdit( iNumberOfStages );

	UpdateData( false );
	*pResult = 1;
}


//////////////////////////////////////////////////////////////////////
// GetTopDeckHeightFromEdit
//
double BayDetailsDialog::GetTopDeckHeightFromEdit()
{
	CString sDeckHeight;
	double  dDeckHeight;

	m_TopWorkingHeightEditCtrl.GetWindowText(sDeckHeight);
	dDeckHeight = double(int((_tstof(sDeckHeight)/STAR_SEPARATION_APPROX))*STAR_SEPARATION_APPROX);
	dDeckHeight = ConvertRLtoStarRL(dDeckHeight-RL_ADJUST_APPROX, m_pBay->GetStarSeparation());

	return dDeckHeight;
}


//////////////////////////////////////////////////////////////////////
// SetTopDeckHeight
//
void BayDetailsDialog::SetTopDeckHeight(double dNewDeckHeight)
{
	// set DeckHeight
	CString sValue;

	sValue.Format(_T("%.2f"), ConvertStarRLtoRL(dNewDeckHeight+m_pBay->RLAdjust(), m_pBay->GetStarSeparation()) );
	m_TopWorkingHeightEditCtrl.SetWindowText(sValue);
}

//////////////////////////////////////////////////////////////////////
// OnApplyButton
//
void BayDetailsDialog::OnApplyButton() 
{
	int					iNumberOfDecks;
	int					iNumberOfEmptyLifts;
	int					iNumberOfLifts;
	int				    iEmptyLift;
	int					iSouthStage;   // this contains the value of deck above which stageboards are present on the south side.
	int					iNorthStage;
	int					iNumberOfStages;
	int					iSelection;
	int					iNumberOfStagePlanks;
	int					iIndex;
	Lift				*pLift;
	double				dTopWorkingHeight;
	MaterialTypeEnum	eMaterial;

	UpdateData();

	if( ShowApplyWarning() )
	{
		CString	sMsg;

		MessageBeep(MB_ICONQUESTION);

		sMsg = _T("Appling these changes will overwrite any recent changes you");
		sMsg+= _T("\nhave made to the standards, or the individual lift settings?");
		sMsg+= _T("\n\nDo you wish to process with these changes?");
		int iRet = MessageBox( sMsg, _T("Warning!"), MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1 );
		if( iRet!=IDYES )
		{
			SetApplyRequired( true );
			return;
		}
		SetShowApplyWarning( false );
	}

	int			iRet;
	bool		bSoleBoard[4], bHandrail[4];
	double		dTWHEdit;
	CString		sMsg;
	doubleArray StandardArrangements[4];

	StandardArrangements[CNR_NORTH_EAST].RemoveAll();
	StandardArrangements[CNR_SOUTH_EAST].RemoveAll();
	StandardArrangements[CNR_NORTH_WEST].RemoveAll();
	StandardArrangements[CNR_SOUTH_WEST].RemoveAll();
	bSoleBoard[CNR_NORTH_EAST] = false;
	bSoleBoard[CNR_SOUTH_EAST] = false;
	bSoleBoard[CNR_NORTH_WEST] = false;
	bSoleBoard[CNR_SOUTH_WEST] = false;

	if( m_bStandardsManuallyChanged )
	{
		dTWHEdit	= GetTopDeckHeightFromEdit();

		double dLowestTWHByStandardsAndRails;
		bHandrail[CNR_NORTH_EAST] = ( m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, NORTH ) ||
									  m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, EAST ) );
		bHandrail[CNR_SOUTH_EAST] = ( m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, SOUTH ) ||
									  m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, EAST ) );
		bHandrail[CNR_NORTH_WEST] = ( m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, NORTH ) ||
									  m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, WEST ) );
		bHandrail[CNR_SOUTH_WEST] = ( m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, SOUTH ) ||
									  m_pBay->GetLift( (m_pBay->GetNumberOfLifts()-1) )->HasComponentOfTypeOnSide( CT_RAIL, WEST ) );
		if( m_pBay->GetStandardFit()==STD_FIT_SAME )
		{
			if( bHandrail[NORTH] || bHandrail[EAST] ||
				bHandrail[SOUTH] || bHandrail[WEST] )
			{
				//make them the same height, no need to do the else
				//	since they are all false anyway!
				bHandrail[NORTH]	= true;
				bHandrail[EAST]		= true;
				bHandrail[SOUTH]	= true;
				bHandrail[WEST]		= true;
			}
		}

		dLowestTWHByStandardsAndRails =      m_pBay->GetHeightOfStandards(CNR_NORTH_EAST) - ((bHandrail[CNR_NORTH_EAST])? (2.00*GetStarSeparation()): 0.00);
		dLowestTWHByStandardsAndRails = min( m_pBay->GetHeightOfStandards(CNR_SOUTH_EAST) - ((bHandrail[CNR_SOUTH_EAST])? (2.00*GetStarSeparation()): 0.00), dLowestTWHByStandardsAndRails );
		dLowestTWHByStandardsAndRails = min( m_pBay->GetHeightOfStandards(CNR_NORTH_WEST) - ((bHandrail[CNR_NORTH_WEST])? (2.00*GetStarSeparation()): 0.00), dLowestTWHByStandardsAndRails );
		dLowestTWHByStandardsAndRails = min( m_pBay->GetHeightOfStandards(CNR_SOUTH_WEST) - ((bHandrail[CNR_SOUTH_WEST])? (2.00*GetStarSeparation()): 0.00), dLowestTWHByStandardsAndRails );
		dLowestTWHByStandardsAndRails-= m_pBay->RLAdjust();

		if( dLowestTWHByStandardsAndRails<dTWHEdit )
		{
			MessageBeep(MB_ICONQUESTION);
			sMsg = _T("Your Top Working Height (plus the Handrail, were applicable)\n");
			sMsg+= _T("now exceeds the height capability of one or more of your\n");
			sMsg+= _T("standards.\n\n");
			sMsg+= _T("I will have to overwrite your modifications to perform this\n");
			sMsg+= _T("update operation!");
      iRet = MessageBox( sMsg, _T("WARNING: Standards Change"), MB_OKCANCEL|MB_ICONEXCLAMATION );
			if( iRet==IDOK )
			{
				//continue and modify the standards
				m_bStandardsManuallyChanged = false;
			}
			else
			{
				//user want to stop!
				return;
			}
		}
		else
		{
			//we need to keep the existing standard arrangements!
			m_pBay->GetStandardsArrangement( StandardArrangements[CNR_NORTH_EAST], CNR_NORTH_EAST );
			m_pBay->GetStandardsArrangement( StandardArrangements[CNR_SOUTH_EAST], CNR_SOUTH_EAST );
			m_pBay->GetStandardsArrangement( StandardArrangements[CNR_NORTH_WEST], CNR_NORTH_WEST );
			m_pBay->GetStandardsArrangement( StandardArrangements[CNR_SOUTH_WEST], CNR_SOUTH_WEST );
			bSoleBoard[CNR_NORTH_EAST] = m_pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
			bSoleBoard[CNR_SOUTH_EAST] = m_pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SOUTH_EAST );
			bSoleBoard[CNR_NORTH_WEST] = m_pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_WEST );
			bSoleBoard[CNR_SOUTH_WEST] = m_pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SOUTH_WEST );
		}
	}

	//////////////////////////////////////////////////////////////////
	//If they have eastern or western hopups, then we need to recreate them
	int		iPlanks[4], iRunID;
	bool	bStage[4], bIsAutobuild;
	bStage[E] = m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST );
	bStage[W] = m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST );
	iRunID = m_pBay->GetRunPointer()->GetController()->GetRunID( m_pBay->GetRunPointer() );
	bIsAutobuild = ( (iRunID==ID_NONE_MATCHING) || (iRunID==ID_NO_IDS) );
	iPlanks[E] = m_pBay->GetLift( m_pBay->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( EAST );
	iPlanks[W] = m_pBay->GetLift( m_pBay->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( WEST );
	if( !bIsAutobuild && (bStage[E] || bStage[W]) )
	{
		//////////////////////////////////////////////////////////////////
		//how many planks do we need to create?
		CString sPlanks;
		iPlanks[E] = m_pBay->GetLift( m_pBay->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( EAST );
		iPlanks[W] = m_pBay->GetLift( m_pBay->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( WEST );
		if( iPlanks[E]>0 && iPlanks[W]<=0 )
			sPlanks.Format( _T("%i"), iPlanks[E] );
		else if( iPlanks[W]>0 && iPlanks[E]<=0 )
			sPlanks.Format( _T("%i"), iPlanks[W] );
		else if( iPlanks[W]==iPlanks[E] )
			sPlanks.Format( _T("%i"), iPlanks[E] );
		else
			sPlanks.Format( _T("%i & %i"), iPlanks[E], iPlanks[W] );

		//////////////////////////////////////////////////////////////////
		//Form the Warning
		CString sMsg, sTemp, sTemp1, sTemp2, sTemp3, sTemp4;
		if( bStage[E] && !bStage[W] )
		{
			sTemp1 = _T("Eastern");
			sTemp2 = _T("this");
			sTemp3 = _T("side");
		}
		else if( bStage[W] && !bStage[E] )
		{
			sTemp1 = _T("Western");
			sTemp2 = _T("this");
			sTemp3 = _T("side");
		}
		else
		{
			sTemp1 = _T("Eastern and Western");
			sTemp2 = _T("these");
			sTemp3 = _T("sides");
		}

    sMsg.Format( _T("WARNING: The first bay you selected has hopups on the %s %s.\n\n"),
						sTemp1, sTemp3 );
		sTemp.Format( _T("To perform this update, I must overwrite these existing hopups!\n\n"),
						sTemp2 );
		sMsg+= sTemp;
		sTemp.Format( _T("I will create %s board hopups on the %s %s, at the same heights as\n"), 
						sPlanks, sTemp1, sTemp3 );
		sMsg+= sTemp;
		sTemp.Format( _T("the southern hopups.\n\nTo customise these hopups, please use 'Lift Details'.\n\n"), 
						sTemp2 );
		sMsg+= sTemp;
		sTemp.Format( _T("Yes    = Create the %s board %s hopups,\n"), sPlanks, sTemp1 );
		sMsg+= sTemp;
		sTemp.Format( _T("No     = Delete the %s hopups,\n"), sTemp1 );
		sMsg+= sTemp;
		sMsg+= _T("Cancel = Abort this operation.");

		MessageBeep(MB_ICONWARNING);
		int iRet = MessageBox( sMsg, _T("Warning! Eastern & Western Hopups"), MB_YESNOCANCEL|MB_ICONWARNING|MB_DEFBUTTON1 );
		switch( iRet )
		{
		case( IDYES ):
			//create the hopups
			break;
		case( IDNO ):
			//delete the hopups
			bStage[E] = false;
			bStage[W] = false;
			iPlanks[E] = 0;
			iPlanks[W] = 0;
			break;
		case( IDCANCEL ):
			return;
		default:
			;//assert( false );
			return;
		}
	}

	iIndex = m_MaterialTypeCombo.GetCurSel();
	eMaterial = (MaterialTypeEnum)m_MaterialTypeCombo.GetItemData(iIndex);

	//////////////////////////////////////////////////////////////////
	// delete all current lifts
	m_pBay->DeleteAllComponents(); //	DeleteAllLifts();	DeleteAllBayComponents();

	//////////////////////////////////////////////////////////////////
	// get top deck height
	dTopWorkingHeight = GetTopDeckHeightFromEdit();

	//////////////////////////////////////////////////////////////////
	// get number of decks
	iNumberOfDecks = GetNumberOfDecksFromEdit();
	;//assert( iNumberOfDecks<=GetMaximumNumberOfDecks() );

	//////////////////////////////////////////////////////////////////
	//how many planks
	iSelection = m_StageBracketWidthsComboCtrl.GetCurSel();
	iNumberOfStagePlanks = m_StageBracketWidthsComboCtrl.GetItemData(iSelection);

	//////////////////////////////////////////////////////////////////
	//How many lifts are we going to create!
	iNumberOfLifts = GetNumberOfLiftFromTWH();

	//////////////////////////////////////////////////////////////////
	//How many empty lifts?
	int			i, j;
	double		dNextDeckRL, dRL;
	LevelList	llDecks, llAllLifts;
	llDecks.RemoveAll();
	llAllLifts.RemoveAll();
	if( m_bMatchDecksToLevels )
	{
		llDecks = GetPossibleDeckRLs();
		llDecks.SelectionSort(true);

		//From the top down remove any unwanted decks
		int iSize = llDecks.GetSize()-iNumberOfDecks;
		for( i=0; i<iSize; i++ )
		{
			llDecks.RemoveLevel((int)0);
		}

		//We should now have a list of all the RLs for the
		//	decks we are going to create
		double dTWH, dTWRL, dBWH;
		dTWH	= GetTopDeckHeightFromEdit();
		dBWH	= GetTopBottomStdRL();
		dTWRL	= dTWH-dBWH;
		//Round to nearest star beneath RL;
		dTWRL	= double(int(dTWRL/GetStarSeparation())*GetStarSeparation());
		dBWH	= double(int(dBWH/GetStarSeparation())*GetStarSeparation());

		//We will have a lift at each deck;
		llAllLifts = llDecks;

		//Where else can we have some lifts?
		//Start at the bottom RL
		if( llDecks.GetSize()>0 )
		{
			for( i=0; i<=llDecks.GetSize(); i++ )
			{
				//The last time through we need to use the TWRL, not a deck lift
				//	if these are the same value then the separation is 0 anyway
				if( i==llDecks.GetSize() )
					dNextDeckRL = dTWRL;
				else
					dNextDeckRL = llDecks.GetLevel(i);

				//We now need to add as many 2.0m lifts as possible, and
				//	then fill the slack with the largest possible lift
				if( i==0 )
				{
					//The first time through we need to build downward,
					//	add all lifts between these values, except the
					//	very bottom one, and the very top one!
					for( dRL=dNextDeckRL-(4.00*GetStarSeparation()) ; dRL>dBWH+ROUND_ERROR; dRL-=(4.00*GetStarSeparation()) )
					{
						//Add a lift here!
						llAllLifts.AddLevel( dRL, false );
					}

					//Add one more at the BWH, unless the top in this range==the bottom in this range
					if( dNextDeckRL!=dBWH )
						llAllLifts.AddLevel( dBWH, false );
				}
				else
				{
					for( ; dRL<dNextDeckRL-ROUND_ERROR; dRL+= (4.00*GetStarSeparation()) )
					{
						//Add a lift here!
						llAllLifts.AddLevel( dRL, false );
					}
					if( i==llDecks.GetSize() )
						llAllLifts.AddLevel( dTWRL, false );
				}
				//The next time through this loop we need to use the previous
				//	ceiling at the starting point!
				dRL = dNextDeckRL;
			}
		}
		else
		{
			//There are no decks!
			for( dRL=dTWRL; dRL>dBWH+ROUND_ERROR; dRL-=(4.00*GetStarSeparation()) )
			{
				//Add a lift here!
				llAllLifts.AddLevel( dRL );
			}

			//Add one more at the BWH
			llAllLifts.AddLevel( dBWH );
		}

		//We can now recalc the number of deck, and empty lifts
		iNumberOfLifts		= llAllLifts.GetSize();
		iNumberOfDecks		= llDecks.GetSize();
		iNumberOfEmptyLifts = iNumberOfLifts-iNumberOfDecks;
	}
	else
	{
		iNumberOfEmptyLifts = iNumberOfLifts-iNumberOfDecks;
	}


	//////////////////////////////////////////////////////////////////
	//work out staging
	iNumberOfStages = GetNumberOfStagesFromEdit();
	if( m_iStagePositioning == STAGE_POS_DECKS_ONLY )
	{
		if (iNumberOfDecks >= iNumberOfStages)
		{
			//these are on the south side only anyway
			iSouthStage = iNumberOfDecks - iNumberOfStages;
			iNorthStage = iNumberOfDecks;
		}
		else
		{
			//we need to use some northside decks too 
			;//assert( iNumberOfDecks*2 >= iNumberOfStages );
			iSouthStage = 0; // all of southside decks
			iNumberOfStages -= iNumberOfDecks;
			iNorthStage = iNumberOfDecks - iNumberOfStages;
		}
	}
	else
	{
		;//assert( !m_bMatchDecksToLevels );
		;//assert( m_iStagePositioning == STAGE_POS_SOUTH_ONLY );
		;//assert( iNumberOfLifts >= iNumberOfStages );
		iNorthStage = iNumberOfLifts;
		iSouthStage = iNumberOfLifts - iNumberOfStages;
	}

	if( StandardArrangements[CNR_NORTH_EAST].GetSize()<=0 &&
		StandardArrangements[CNR_SOUTH_EAST].GetSize()<=0 &&
		StandardArrangements[CNR_NORTH_WEST].GetSize()<=0 &&
		StandardArrangements[CNR_SOUTH_WEST].GetSize()<=0 )
	{
		//we can overwrite existing standards
		RecreateAllStandardsArrangments();
	}
	else
	{
		//we stored the changes they made to the standards, so lets re apply the original config!
		m_pBay->SetStandardsArrangement( StandardArrangements[CNR_NORTH_EAST], CNR_NORTH_EAST, bSoleBoard[CNR_NORTH_EAST] );
		m_pBay->SetStandardsArrangement( StandardArrangements[CNR_SOUTH_EAST], CNR_SOUTH_EAST, bSoleBoard[CNR_SOUTH_EAST] );
		m_pBay->SetStandardsArrangement( StandardArrangements[CNR_NORTH_WEST], CNR_NORTH_WEST, bSoleBoard[CNR_NORTH_WEST] );
		m_pBay->SetStandardsArrangement( StandardArrangements[CNR_SOUTH_WEST], CNR_SOUTH_WEST, bSoleBoard[CNR_SOUTH_WEST] );
	}

	if( m_bMatchDecksToLevels )
	{
		llAllLifts.SelectionSort();
		llDecks.SelectionSort();
#ifdef	_DEBUG
		for( i=0; i<llAllLifts.GetSize(); i++)
		{
			acutPrintf( _T("\nLevel%i = %1.0f"), i, llAllLifts.GetLevel(i) );
		}
#endif	//#ifdef _DEBUG

		//////////////////////////////////////////////////////////////////
		//Create the lifts
		int		iDeck = 0;
		bool	bDeck;
		double	dRise;
		for( i=0; i<llAllLifts.GetSize(); i++)
		{
			pLift = new Lift();

			if( i<llAllLifts.GetSize()-1 )
			{
				dRise = llAllLifts.GetLevel(i+1)-llAllLifts.GetLevel(i);
			}
			else
			{
				dRise = 4.00*GetStarSeparation();
			}
			pLift->SetRise( GetRiseEnumFromRise(dRise) );
			m_pBay->AddLift(pLift);

			//////////////////////////////////////////////////////////////////
			//Ledgers
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, MT_STEEL );

			//////////////////////////////////////////////////////////////////
			//transoms
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	EAST,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	WEST,	LIFT_RISE_0000MM, MT_STEEL );

			bDeck = false;
			for( j=0; j<llDecks.GetSize(); j++ )
			{
				if( llAllLifts.GetLevel(i)>llDecks.GetLevel(j)-ROUND_ERROR &&
					llAllLifts.GetLevel(i)<llDecks.GetLevel(j)+ROUND_ERROR )
				{
					bDeck = true;
					break;
				}
			}
			if( bDeck )
			{
				iDeck++;
				;//assert( pLift->GetRise()==LIFT_RISE_2000MM );

				//Create the decking
				int iNumberOfPlanks;
				iNumberOfPlanks = GetNumberOfPlanksForWidth( m_pBay->GetBayWidth() );
				int i;
				for( i=0; i<iNumberOfPlanks; i++ )
				{
					pLift->AddComponent( CT_DECKING_PLANK, m_pBay->GetBayLength(),	SOUTH,	i, eMaterial );
				}
			
				//////////////////////////////////////////////////////////////////
				//Stageboards
				if (iNumberOfStagePlanks>0)
				{
					if (iDeck >= iSouthStage)
						pLift->AddStage( SOUTH,	iNumberOfStagePlanks, eMaterial, false );
					if (iDeck > iNorthStage)
						pLift->AddStage( NORTH,	iNumberOfStagePlanks, eMaterial, false );

					if( bStage[E] )
					{
						;//assert( iPlanks[E]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[E]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
						pLift->AddStage( EAST,	iPlanks[E], eMaterial, false );
					}
					if( bStage[W] )
					{
						;//assert( iPlanks[W]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[W]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
						pLift->AddStage( WEST,	iPlanks[W], eMaterial, false );
					}
				}
				else
				{
					if( pLift->GetLiftID()==m_pBay->GetNumberOfLifts()-1 )
					{
						m_pBay->GetTemplate()->SetWidthOfSStage( 0 );
					}
				}
			}
		}
	}
	else	//if( m_bMatchDecksToLevels )
	{
		//////////////////////////////////////////////////////////////////
		//Create the empty lifts
		for (iEmptyLift = 0; iEmptyLift < iNumberOfEmptyLifts; ++iEmptyLift)
		{
			pLift = new Lift();

			if( iEmptyLift==0 )
			{
				//find the height remaining once we remove all the 2.0m lifts
				//	don't include the top lift, or the bottom one (since we are
				//	trying to find the height of the bottom one)
				double dHeight = dTopWorkingHeight-((4*GetStarSeparation())*(iNumberOfLifts-2));  
				;//assert( dHeight>-1.00*ROUND_ERROR );
				;//assert( dHeight<(4*GetStarSeparation())+ROUND_ERROR );
				pLift->SetRise( GetRiseEnumFromRise( dHeight ) );
			}
			else
			{
				pLift->SetRise( LIFT_RISE_2000MM );
			}
			m_pBay->AddLift(pLift);

			//////////////////////////////////////////////////////////////////
			//Ledgers
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, MT_STEEL );

			//////////////////////////////////////////////////////////////////
			//transoms
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	EAST,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	WEST,	LIFT_RISE_0000MM, MT_STEEL );

			//////////////////////////////////////////////////////////////////
			//Stageboards
			if( m_iStagePositioning==STAGE_POS_SOUTH_ONLY  && 
				iEmptyLift >= iSouthStage &&
				iNumberOfStagePlanks>0 )
			{
				pLift->AddStage( SOUTH,	iNumberOfStagePlanks, eMaterial, false );

				if( bStage[E] )
				{
					;//assert( iPlanks[E]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[E]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
					pLift->AddStage( EAST,	iPlanks[E], eMaterial, false );
				}
				if( bStage[W] )
				{
					;//assert( iPlanks[W]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[W]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
					pLift->AddStage( WEST,	iPlanks[W], eMaterial, false );
				}
			}
			else
			{
				if( pLift->GetLiftID()==m_pBay->GetNumberOfLifts()-1 )
				{
					//This is the top lift so set the width of the stage board!
					m_pBay->GetTemplate()->SetWidthOfSStage( 0 );
				}
			}
		}

		if( m_iStagePositioning == STAGE_POS_SOUTH_ONLY )
		{
			//calc the starting southside deck
			iSouthStage = iNumberOfDecks - iNumberOfStages;
			//north will still be out of bounds
		}

		//////////////////////////////////////////////////////////////////
		// work out number of decking lifts
		int iDeck;
		for (iDeck = 0; iDeck < iNumberOfDecks; ++iDeck)
		{
			pLift = new Lift();
			pLift->SetRise( LIFT_RISE_2000MM );
			m_pBay->AddLift(pLift);
			//////////////////////////////////////////////////////////////////
			//Ledgers
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_LEDGER,	m_pBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, MT_STEEL );

			//////////////////////////////////////////////////////////////////
			//transoms
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	EAST,	LIFT_RISE_0000MM, MT_STEEL );
			pLift->AddComponent( CT_TRANSOM, m_pBay->GetBayWidth(),	WEST,	LIFT_RISE_0000MM, MT_STEEL );

			//Create the decking
			int iNumberOfPlanks;
			iNumberOfPlanks = GetNumberOfPlanksForWidth( m_pBay->GetBayWidth() );
			int i;
			for( i=0; i<iNumberOfPlanks; i++ )
			{
				pLift->AddComponent( CT_DECKING_PLANK, m_pBay->GetBayLength(),	SOUTH,	i, eMaterial );
			}
		
			//////////////////////////////////////////////////////////////////
			//Stageboards
			if (iNumberOfStagePlanks > 0)
			{
				if (iDeck >= iSouthStage)
					pLift->AddStage( SOUTH,	iNumberOfStagePlanks, eMaterial, false );
				if (iDeck >= iNorthStage)
					pLift->AddStage( NORTH,	iNumberOfStagePlanks, eMaterial, false );

				if( bStage[E] )
				{
					;//assert( iPlanks[E]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[E]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
					pLift->AddStage( EAST,	iPlanks[E], eMaterial, false );
				}
				if( bStage[W] )
				{
					;//assert( iPlanks[W]>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE && iPlanks[W]<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
					pLift->AddStage( WEST,	iPlanks[W], eMaterial, false );
				}
			}
			else
			{
				if( pLift->GetLiftID()==m_pBay->GetNumberOfLifts()-1 )
				{
					m_pBay->GetTemplate()->SetWidthOfSStage( 0 );
				}
			}

	/*		//////////////////////////////////////////////////////////////////
			//corner stageboards
			pLift->AddCornerStageBoard( SideOfBayAsCorner( NORTH_EAST ), eMaterial );
			pLift->AddCornerStageBoard( SideOfBayAsCorner( SOUTH_EAST ), eMaterial );
			pLift->AddCornerStageBoard( SideOfBayAsCorner( SOUTH_WEST ), eMaterial );
			pLift->AddCornerStageBoard( SideOfBayAsCorner( NORTH_WEST ), eMaterial );
	*/
		}
	}//if( m_bMatchDecksToLevels )

	m_pBay->SetRLsForAllLifts( GetTopBottomStdRL() );

	//////////////////////////////////////////////////////////////////
	ApplyShadeCloth();
	ApplyChainMesh();
	ApplyHandrailOptionToBay();

	m_pBay->SetRoofProtection( RP_NONE );
	m_pBay->RemoveRoofProtectionComponents();

	switch( m_iRoofProtection )
	{
	case(RP_NONE):
		//already done!
		break;
	case(RP_1500MM):
		m_pBay->SetRoofProtection( RP_1500MM );
		m_pBay->AddRoofProtectionComponents();
		break;
	case(RP_2000MM):
		m_pBay->SetRoofProtection( RP_2000MM );
		m_pBay->AddRoofProtectionComponents();
		break;
	default:
		;//assert( false );
	}

	//////////////////////////////////////////////////////////////////
	// refresh dialog
	GetCurrentSettings();
	SetApplyRequired( false );
}

//////////////////////////////////////////////////////////////////////
// OnRailoptionsHandRadio
//
void BayDetailsDialog::OnRailoptionsHandRadio() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
	m_btnFullMeshDecksOnly.EnableWindow( false );
}

//////////////////////////////////////////////////////////////////////
// OnHandoptionsMeshRadio
//
void BayDetailsDialog::OnHandoptionsMeshRadio() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
	m_btnFullMeshDecksOnly.EnableWindow( false );
}

//////////////////////////////////////////////////////////////////////
// OnHandoptionsFullmeshRadio
//
void BayDetailsDialog::OnHandoptionsFullmeshRadio() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
	m_btnFullMeshDecksOnly.EnableWindow( true );
}

//////////////////////////////////////////////////////////////////////
// OnHandoptionsShadeclothCheck
//
void BayDetailsDialog::OnHandoptionsShadeclothCheck() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
	UpdateData( );
}

//////////////////////////////////////////////////////////////////////
// OnHandoptionsChainCheck() 
//
void BayDetailsDialog::OnHandoptionsChainCheck() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
	UpdateData();
	m_btnUseMidrailWithChain.EnableWindow( m_bHandOptionsChain );
}

//////////////////////////////////////////////////////////////////////
// GetNumberOfDecksFromEdit()
//
int BayDetailsDialog::GetNumberOfDecksFromEdit()
{
	CString sNumberOfDecks;
	int iNumberOfDecks;

	m_NumberOfDecksEditCtrl.GetWindowText(sNumberOfDecks);
	iNumberOfDecks = _ttoi(sNumberOfDecks);

	return iNumberOfDecks;
}


//////////////////////////////////////////////////////////////////////
// SetNumberOfDecksInEdit
//
void BayDetailsDialog::SetNumberOfDecksInEdit(int iNumberOfDecks)
{
	CString sNumberOfDecks;

	sNumberOfDecks.Format(_T("%d"), iNumberOfDecks);
	m_NumberOfDecksEditCtrl.SetWindowText(sNumberOfDecks);

	UpdateData(false);
}

//////////////////////////////////////////////////////////////////////
// OnChangeTopWorkingHeightEdit() 
//
void BayDetailsDialog::OnChangeTopWorkingHeightEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
/*	CString sStr;
	sStr.Format("%.2f", GetTopDeckHeightFromEdit());
	MessageBox(sStr, "ENCHANGE");
*/
	UpdateData();
	SetApplyRequired( true );
	ChangeMadeToDialog();
//	OnKillfocusTopWorkingHeightEdit();
}

//////////////////////////////////////////////////////////////////////
// OnKillfocusTopWorkingHeightEdit() 
//
void BayDetailsDialog::OnKillfocusTopWorkingHeightEdit() 
{
	int		iNumberOfDecks;
	int		iNumberOfStages;

	UpdateData();
	ChangeMadeToDialog();
	SetApplyRequired( true );

	SetTopDeckHeight( GetTopDeckHeightFromEdit() );
	UpdateData( false );

	iNumberOfDecks = min( GetNumberOfDecksFromEdit(), GetMaximumNumberOfDecks() );
	SetNumberOfDecksInEdit(iNumberOfDecks);

	iNumberOfStages	= min( GetNumberOfStagesFromEdit(), GetMaximumNumberOfStages() );
	SetNumberOfStagesInEdit(iNumberOfStages);

	UpdateData( false );
}


//////////////////////////////////////////////////////////////////////
// GetMaximumNumberOfStages()
//
int BayDetailsDialog::GetMaximumNumberOfStages()
{
	int iNumberOfDecks, iNumberOfLifts;
	iNumberOfDecks = GetNumberOfDecksFromEdit();
	iNumberOfLifts = GetNumberOfLiftFromTWH();

	if( m_iStagePositioning == STAGE_POS_DECKS_ONLY )
	{
		return iNumberOfDecks*2;
	}

	;//assert( m_iStagePositioning == STAGE_POS_SOUTH_ONLY );
	return iNumberOfLifts;
}


//////////////////////////////////////////////////////////////////////
// GetNumberOfStagesFromEdit()
//
int BayDetailsDialog::GetNumberOfStagesFromEdit()
{
	int iNumberOfStages;
	CString sNumberOfStages;

	iNumberOfStages = _ttoi(m_sNumberOfStages);

	return iNumberOfStages;
}


//////////////////////////////////////////////////////////////////////
// SetNumberOfStagesInEdit
//
void BayDetailsDialog::SetNumberOfStagesInEdit(int iNumberOfStages)
{
	CString sNumberOfStages;

	m_sNumberOfStages.Format(_T("%i"), iNumberOfStages);

	NumberOfStagesChanged( iNumberOfStages );
}

//////////////////////////////////////////////////////////////////////
// OnKillfocusNumberDecksEdit
//
void BayDetailsDialog::OnKillfocusNumberDecksEdit() 
{
	UpdateData();
	ChangeMadeToDialog();
	SetApplyRequired( true );

	OnKillfocusTopWorkingHeightEdit(); 
}

//////////////////////////////////////////////////////////////////////
// OnKillfocusNumberStages
//
void BayDetailsDialog::OnKillfocusNumberStagesEdit() 
{
	UpdateData();
	ChangeMadeToDialog();
	SetApplyRequired( true );

	OnKillfocusTopWorkingHeightEdit(); 
}

void BayDetailsDialog::OnSelchangeBayLengthCombo2() 
{
	double dNewBayLength;
	CString sNewBayLength;


	UpdateData();
	ChangeMadeToDialog();
	m_BayLengthComboCtrl.GetLBText(m_BayLengthComboCtrl.GetCurSel(), sNewBayLength);

	m_pBay->SetBayLengthChangedDuringEdit(true);

	dNewBayLength = _tstof(sNewBayLength);
//	m_pAutobuildTools->SetBayLength(dNewBayLength);
	if( dNewBayLength>=COMPONENT_LENGTH_0800-ROUND_ERROR && dNewBayLength<=COMPONENT_LENGTH_0800+ROUND_ERROR)
		dNewBayLength = COMPONENT_LENGTH_0700;

	m_pBay->SetBayLength(dNewBayLength);
//	m_pBay->ApplyBayResize();
}

void BayDetailsDialog::OnSelchangeBayWidthCombo2() 
{
	double dNewBayWidth; 
	CString sNewBayWidth;

	m_pBay->SetBayWidthChangedDuringEdit(true);

	ChangeMadeToDialog();
	m_BayWidthComboCtrl.GetLBText(m_BayWidthComboCtrl.GetCurSel(), sNewBayWidth);

	dNewBayWidth = _tstof(sNewBayWidth);
//	m_pAutobuildTools->SetBayWidth(dNewBayWidth);
	if( dNewBayWidth>=COMPONENT_LENGTH_0800-ROUND_ERROR &&
		dNewBayWidth<=COMPONENT_LENGTH_0800+ROUND_ERROR )
		dNewBayWidth = COMPONENT_LENGTH_0700;

	m_pBay->SetBayWidth(dNewBayWidth);
//	m_pBay->ApplyBayResize();
}

void BayDetailsDialog::OnDblclkNeStandardLbox() 
{
	DoubleClickStandardBox( NORTH_EAST, m_dStandardListCtrlNE, CNR_NORTH_EAST, m_StandardEditCtrlNE );
}

void BayDetailsDialog::OnDblclkSeStandardLbox() 
{
	DoubleClickStandardBox( SOUTH_EAST, m_dStandardListCtrlSE, CNR_SOUTH_EAST, m_StandardEditCtrlSE );
}

void BayDetailsDialog::OnDblclkSwStandardLbox() 
{
	DoubleClickStandardBox( SOUTH_WEST, m_dStandardListCtrlSW, CNR_SOUTH_WEST, m_StandardEditCtrlSW );
}

void BayDetailsDialog::OnDblclkNwStandardLbox() 
{
	DoubleClickStandardBox( NORTH_WEST, m_dStandardListCtrlNW, CNR_NORTH_WEST, m_StandardEditCtrlNW );
}

void BayDetailsDialog::DoubleClickStandardBox( SideOfBayEnum eSide, CListBox &StandardListCtrl, CornerOfBayEnum eCnr, CEdit &StandardEditCtrl )
{
	int			iStandardPosition, iArrayPosition, iOpenEnded;
	bool		bSoleboard;
	double		dNewStandardLength;
	CString		sStandardHeight;
	doubleArray	daArrangement;

	ChangeMadeToDialog();
	if(m_pBay->GetComponent(CT_SOLEBOARD, eSide, 0.0))
		bSoleboard = true;
	else
		bSoleboard = false;

	iStandardPosition = StandardListCtrl.GetCurSel();
	m_pBay->GetStandardsArrangement(daArrangement, eCnr);
	iArrayPosition = daArrangement.GetSize()-iStandardPosition;

	StandardListCtrl.GetText(iStandardPosition, sStandardHeight);
	iOpenEnded = (int)false;
	if( m_pBay!=NULL && m_pBay->GetStandard( iStandardPosition, eCnr )!=NULL )
		iOpenEnded = (int)m_pBay->GetStandard( iStandardPosition, eCnr )->GetType()==CT_STANDARD_OPENEND;
	StandardHeightPickerDialog StandardHeightPicker(this, &sStandardHeight, &dNewStandardLength, &iOpenEnded );	
	if (StandardHeightPicker.DoModal() == IDOK)
	{
		m_pBay->GetStandardsArrangement(daArrangement, eCnr);
		if( iArrayPosition>=daArrangement.GetSize() )
			daArrangement.Add( dNewStandardLength );
		else
			daArrangement[iArrayPosition] = dNewStandardLength;

		m_pBay->DeleteStandards(eCnr);
		m_pBay->SetStandardsArrangement(daArrangement, eCnr, bSoleboard);

		if( iOpenEnded==(int)true )
		{
			;//assert( m_pBay->GetStandard( iArrayPosition, eCnr )!=NULL );
			m_pBay->GetStandard( iArrayPosition, eCnr )->SetType( CT_STANDARD_OPENEND );
		}
		else if( iOpenEnded!=(int)false )
		{
			//We need to recall what which were openeneded and convert those back to openended standards.
			m_pBay->GetStandard( iArrayPosition, eCnr )->SetType( CT_STANDARD );
		}
	}
	UpdateStandardsArrangementCorner(&StandardListCtrl, &StandardEditCtrl, eCnr);

	m_bStandardsManuallyChanged = true;
}


void BayDetailsDialog::ChangeMadeToDialog()
{
	int iRunID;
	bool bIsAutobuild;

	iRunID = m_pBay->GetRunPointer()->GetController()->GetRunID( m_pBay->GetRunPointer() );
	bIsAutobuild = ( (iRunID==ID_NONE_MATCHING) || (iRunID==ID_NO_IDS) );

	if (bIsAutobuild)
	{
		m_btnOK.ShowWindow(SW_SHOW);
		m_ApplyToSelectionButtonCtrl.ShowWindow(SW_HIDE);
	}
	else
	{
		m_btnOK.ShowWindow(SW_HIDE);
		m_ApplyToSelectionButtonCtrl.ShowWindow(SW_SHOW);
	}
}

void BayDetailsDialog::ApplyHandrailOptionToBay()
{
	bool bHandrailExists[4];

	UpdateData( );
	;//assert( m_pBay!=NULL );

	int		iLift, iSide;
	Lift	*pLift;

	bHandrailExists[NORTH]	= m_pBay->GetTemplate()->GetNHandRail();
	bHandrailExists[EAST]	= m_pBay->GetTemplate()->GetEHandRail();
	bHandrailExists[SOUTH]	= m_pBay->GetTemplate()->GetSHandRail();
	bHandrailExists[WEST]	= m_pBay->GetTemplate()->GetWHandRail();

	//go through each lift
	for( iLift=0; iLift<m_pBay->GetNumberOfLifts(); iLift++ )
	{
		pLift = m_pBay->GetLift( iLift );

		//remove any existing handrails
		pLift->DeleteAllHandrailComponentsFromSide(ALL_SIDES);

		//Find the lifts that have decking
		if( pLift->IsDeckingLift() )
		{
			for( iSide=(int)NORTH; iSide<=WEST; iSide++ )
			{
				if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, (SideOfBayEnum)iSide ) )
				{
					//it doesn't have a hopup either, so we need a handrail!
					switch( m_iRailOptions )
					{
					case( HRO_HRO ):
						if( bHandrailExists[iSide] )
							pLift->AddHandrailToSide( (SideOfBayEnum)iSide );
						break;
					case( HRO_MESH ):
						if( bHandrailExists[iSide] )
						{
							pLift->AddMeshToSide( (SideOfBayEnum)iSide );
						}
						break;
					case( HRO_FULLMESH ):
						if( bHandrailExists[iSide] )
						{
							pLift->AddFullMeshToSide( (SideOfBayEnum)iSide );
						}
						break;
					default:
						;//assert( false );
					}
				}
			}
		}
		else
		{
			//if there is no decking but there is a stage board
			//	then we also need a handrail
			for( iSide=(int)NORTH; iSide<=WEST; iSide++ )
			{
				if( pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, (SideOfBayEnum)iSide ) )
				{
					switch( m_iRailOptions )
					{
					case( HRO_HRO ):
						pLift->AddHandrailToSide( (SideOfBayEnum)iSide );
						break;
					case( HRO_MESH ):
						pLift->AddMeshToSide( (SideOfBayEnum)iSide );
						break;
					case( HRO_FULLMESH ):
						//don't add full mesh to south side
						if( iSide==(int)SOUTH )
							pLift->AddHandrailToSide( (SideOfBayEnum)iSide );
						else
							pLift->AddFullMeshToSide( (SideOfBayEnum)iSide );
						break;
					default:
						;//assert( false );
					}
				}
				else
				{
					if( iSide!=(int)SOUTH && !m_bFullMeshDecksOnly && m_iRailOptions==HRO_FULLMESH &&
						!pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, SOUTH ))
					{
						if( pLift->GetRise()>=LIFT_RISE_2000MM )
						{
							pLift->AddFullMeshToSide( (SideOfBayEnum)iSide );
						}
						else if( pLift->GetRise()>=LIFT_RISE_1000MM )
						{
							pLift->AddMeshToSide( (SideOfBayEnum)iSide );
						}
					}
				}
			}
		}
	}
}

HandRailOptionEnum BayDetailsDialog::GetBaysRailSetting()
{
	int			iLift, iComp;
	bool		bHasFullMesh;
	Lift		*pLift;
	Component	*pComp;

	//Do any of the lifts have full mesh?
	m_bFullMeshDecksOnly = true;
	bHasFullMesh = false;
	for( iLift=0; iLift<m_pBay->GetNumberOfLifts(); iLift++ )
	{
		pLift = m_pBay->GetLift( iLift );

		for( iComp=0; iComp<pLift->GetNumberOfComponents(); iComp++ )
		{
			if( pLift->GetComponentRise( iComp )==LIFT_RISE_2000MM )
			{
				pComp = pLift->GetComponent( iComp );
				if( pComp->GetType()==CT_MESH_GUARD )
				{
					bHasFullMesh = true;
					if( pLift->IsEmptyLift() )
					{
						m_bFullMeshDecksOnly = false;
						break;
					}
				}
			}
		}
	}
	if( bHasFullMesh )
	{
		m_btnFullMeshDecksOnly.EnableWindow( true );
		if( m_bFullMeshDecksOnly )
		{
			bool bEmpty;
			//do all the lifts have decking?
			bEmpty = false;
			for( iLift=0; iLift<m_pBay->GetNumberOfLifts(); iLift++ )
			{
				pLift = m_pBay->GetLift( iLift );
				if( pLift->IsEmptyLift() )
				{
					bEmpty = true;
					break;
				}
			}
			m_bFullMeshDecksOnly = bEmpty;
		}
		return HRO_FULLMESH;
	}

	m_btnFullMeshDecksOnly.EnableWindow( false );

	if( (m_pBay->HasComponentOfTypeOnSide( CT_MESH_GUARD, N )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_MESH_GUARD, E )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_MESH_GUARD, S )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_MESH_GUARD, W )!=NULL) )
	{
		return HRO_MESH;
	}

	if(	(m_pBay->HasComponentOfTypeOnSide( CT_RAIL, NORTH )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_RAIL, EAST )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_RAIL, SOUTH )!=NULL) ||
		(m_pBay->HasComponentOfTypeOnSide( CT_RAIL, WEST )!=NULL) )
	{
		return HRO_HRO;
	}

	//;//assert( false );
	return HRO_FULLMESH;
}

void BayDetailsDialog::FillBayDimensionCombos()
{
	CString sLength;

	int iIndex = 0;
	m_BayLengthComboCtrl.Clear();
	m_BayLengthComboCtrl.ResetContent();
//	m_BayLengthComboCtrl.AddString("<Bay Length>");
	while (BayLengths(iIndex) != 0)
	{
		sLength.Format(_T("%.2f"), BayLengths(iIndex)); // inline func from meccanodefinitions file
		if( BayLengths(iIndex)<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			BayLengths(iIndex)>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength.Format( _T("%0.2f"), COMPONENT_LENGTH_0800 );
		m_BayLengthComboCtrl.AddString(sLength);
		++iIndex;
	}

	iIndex = 0;
	m_BayWidthComboCtrl.Clear();
	m_BayWidthComboCtrl.ResetContent();
//	m_BayWidthComboCtrl.AddString("<Bay Width>");
	while (BayWidths(iIndex) != 0)
	{
		sLength.Format(_T("%.2f"), BayLengths(iIndex));// inline func from meccanodefinitions file
		if( BayLengths(iIndex)<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
			BayLengths(iIndex)>=COMPONENT_LENGTH_0700-ROUND_ERROR)
			sLength.Format( _T("%0.2f"), COMPONENT_LENGTH_0800 );
		m_BayWidthComboCtrl.AddString(sLength);
		++iIndex;
	}

	UpdateData( FALSE );
}

void BayDetailsDialog::OnSelchangeStageBracketWidthsCombo() 
{
	ChangeMadeToDialog();
	SetApplyRequired( true );
}

void BayDetailsDialog::OnChangeNumberDecksEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	ChangeMadeToDialog();
	SetApplyRequired( true );

//	OnKillfocusTopWorkingHeightEdit();
}

void BayDetailsDialog::OnChangeNumberStagesEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KwikscafDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	int iNumberOfStages;

	UpdateData();
	ChangeMadeToDialog();
	SetApplyRequired( true );

	iNumberOfStages = min( GetNumberOfStagesFromEdit(), GetMaximumNumberOfStages() );
	SetNumberOfStagesInEdit( iNumberOfStages );

	UpdateData( false );
}

bool BayDetailsDialog::OkToExit()
{
	int		iRet;
	CString	sMsg;
	if( IsApplyRequired() )
	{
		MessageBeep(MB_ICONQUESTION);
		sMsg = _T("Would you like to apply changes before proceeding?");
		iRet = MessageBox( sMsg, _T("Warning!"), MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON1 );
		switch( iRet )
		{
		case( IDCANCEL ):
			return false;

		case( IDYES ):
			OnApplyButton();
			//fallthrough
		case( IDNO ):
			break;
		}
	}
	return true;
}

void BayDetailsDialog::SetApplyRequired(bool bApplyReqd/*=true*/)
{
	m_bApplyRequired = bApplyReqd;
	m_ApplyButton.EnableWindow( m_bApplyRequired );
}

bool BayDetailsDialog::IsApplyRequired()
{
	return m_bApplyRequired;
}

void BayDetailsDialog::OnEditchangeDeckMaterialTypeCombo() 
{
	// TODO: Add your control notification handler code here
	
}

void BayDetailsDialog::UpdateAllStandards()
{
	UpdateStandardsArrangementCorner(&m_dStandardListCtrlNE, &m_StandardEditCtrlNE, CNR_NORTH_EAST);
	UpdateStandardsArrangementCorner(&m_dStandardListCtrlNW, &m_StandardEditCtrlNW, CNR_NORTH_WEST);
	UpdateStandardsArrangementCorner(&m_dStandardListCtrlSE, &m_StandardEditCtrlSE, CNR_SOUTH_EAST);
	UpdateStandardsArrangementCorner(&m_dStandardListCtrlSW, &m_StandardEditCtrlSW, CNR_SOUTH_WEST);
}

void BayDetailsDialog::RecreateAllStandardsArrangments()
{
	bool	bHandrailExists[4];
	double	dTopWorkingHeight, dHeight[4];

	dTopWorkingHeight = GetTopDeckHeightFromEdit(); 
	dTopWorkingHeight+= m_pBay->JackLength();

	bHandrailExists[NORTH]	= m_pBay->GetTemplate()->GetNHandRail();
	bHandrailExists[EAST]	= m_pBay->GetTemplate()->GetEHandRail();
	bHandrailExists[SOUTH]	= m_pBay->GetTemplate()->GetSHandRail();
	bHandrailExists[WEST]	= m_pBay->GetTemplate()->GetWHandRail();

	if( m_pBay->GetStandardFit()==STD_FIT_SAME )
	{
		if( bHandrailExists[NORTH] || bHandrailExists[EAST] ||
			bHandrailExists[SOUTH] || bHandrailExists[WEST] )
		{
			//make them the same height, no need to do the else
			//	since they are all false anyway!
			bHandrailExists[NORTH]	= true;
			bHandrailExists[EAST]	= true;
			bHandrailExists[SOUTH]	= true;
			bHandrailExists[WEST]	= true;
		}
	}

	m_dTopWorkingRL = dTopWorkingHeight + GetTopBottomStdRL();
	dHeight[CNR_NORTH_EAST] = m_dTopWorkingRL - m_pBay->GetStandardPosition( CNR_NORTH_EAST ).z;
	dHeight[CNR_SOUTH_EAST] = m_dTopWorkingRL - m_pBay->GetStandardPosition( CNR_SOUTH_EAST ).z;
	dHeight[CNR_SOUTH_WEST] = m_dTopWorkingRL - m_pBay->GetStandardPosition( CNR_SOUTH_WEST ).z;
	dHeight[CNR_NORTH_WEST] = m_dTopWorkingRL - m_pBay->GetStandardPosition( CNR_NORTH_WEST ).z;

	if( bHandrailExists[NORTH] || bHandrailExists[EAST] )
		dHeight[CNR_NORTH_EAST]+= (GetStarSeparation()*2.00);
	if( bHandrailExists[SOUTH] || bHandrailExists[EAST] )
		dHeight[CNR_SOUTH_EAST]+= (GetStarSeparation()*2.00);
	if( bHandrailExists[SOUTH] || bHandrailExists[WEST] )
		dHeight[CNR_SOUTH_WEST]+= (GetStarSeparation()*2.00);
	if( bHandrailExists[NORTH] || bHandrailExists[WEST] )
		dHeight[CNR_NORTH_WEST]+= (GetStarSeparation()*2.00);

	m_pBay->CreateStandardsArrangment( dHeight[CNR_NORTH_EAST], CNR_NORTH_EAST, true);
	m_pBay->CreateStandardsArrangment( dHeight[CNR_SOUTH_EAST], CNR_SOUTH_EAST, true);
	m_pBay->CreateStandardsArrangment( dHeight[CNR_SOUTH_WEST], CNR_SOUTH_WEST, true);
	m_pBay->CreateStandardsArrangment( dHeight[CNR_NORTH_WEST], CNR_NORTH_WEST, true);

	m_bStandardsManuallyChanged = false;
}

void BayDetailsDialog::NumberOfStagesChanged(int iNewNumber)
{
	int iNumberOfStagePlanks, iSelection;

	if( iNewNumber>0 )
	{
		m_StageBracketWidthsComboCtrl.EnableWindow( true );
		iSelection = m_StageBracketWidthsComboCtrl.GetCurSel();
		iNumberOfStagePlanks = m_StageBracketWidthsComboCtrl.GetItemData(iSelection);
		if( iNumberOfStagePlanks<=0 )
		{
			iNumberOfStagePlanks = 1;
			m_StageBracketWidthsComboCtrl.SetCurSel(iNumberOfStagePlanks);  // defaults
		}
	}
	else
	{
		m_StageBracketWidthsComboCtrl.EnableWindow( false );
		m_StageBracketWidthsComboCtrl.SetCurSel(0);
	}
}

void BayDetailsDialog::ApplyShadeCloth()
{
	;//assert( m_pBay!=NULL );
	if( m_bHandOptionsShadecloth )
	{
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) )
			m_pBay->AddComponent( CT_SHADE_CLOTH, NORTH, 0.00, 0.00, MT_STEEL );
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) )
			m_pBay->AddComponent( CT_SHADE_CLOTH, EAST, 0.00, 0.00, MT_STEEL );
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) )
			m_pBay->AddComponent( CT_SHADE_CLOTH, WEST, 0.00, 0.00, MT_STEEL );
	}
	else
	{
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, NORTH );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, EAST );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, SOUTH );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, WEST );
	}
}

void BayDetailsDialog::ApplyChainMesh()
{
	;//assert( m_pBay!=NULL );
	if( m_bHandOptionsChain )
	{
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) )
			m_pBay->AddComponent( CT_CHAIN_LINK, NORTH, 0.00, 0.00, MT_STEEL );
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) )
			m_pBay->AddComponent( CT_CHAIN_LINK, EAST, 0.00, 0.00, MT_STEEL );
		if( !m_pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) )
			m_pBay->AddComponent( CT_CHAIN_LINK, WEST, 0.00, 0.00, MT_STEEL );
	}
	else
	{
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, NORTH );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, EAST );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, SOUTH );
		m_pBay->DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, WEST );
	}
}

BOOL BayDetailsDialog::PreTranslateMessage(MSG* pMsg) 
{
	#ifdef	ALLOW_STANDARD_EDIT
	if (pMsg->message == WM_KEYDOWN)
	{
		if(	(int)pMsg->wParam == DELETE_KEY		|| 
			(int)pMsg->wParam == BACKSPACE_KEY  )
		{
			DeleteRequested();
		}
	}
	#endif	//#ifdef	ALLOW_STANDARD_EDIT
	
	return KwikscafDialog::PreTranslateMessage(pMsg);
}

void BayDetailsDialog::DeleteRequested()
{
	//Delete key pressed
	if( GetFocus() == &m_dStandardListCtrlNE )
	{
		//NORTH_EAST STANDARD
		DeleteStandardBoxElement( NORTH_EAST, m_dStandardListCtrlNE, CNR_NORTH_EAST, m_StandardEditCtrlNE );
		m_bStandardsManuallyChanged = true;
	}
	else if( GetFocus() == &m_dStandardListCtrlSE )
	{
		//SOUTH_EAST STANDARD
		DeleteStandardBoxElement( SOUTH_EAST, m_dStandardListCtrlSE, CNR_SOUTH_EAST, m_StandardEditCtrlSE );
		m_bStandardsManuallyChanged = true;
	}
	else if( GetFocus() == &m_dStandardListCtrlSW )
	{
		//SOUTH_WEST STANDARD
		DeleteStandardBoxElement( SOUTH_WEST, m_dStandardListCtrlSW, CNR_SOUTH_WEST, m_StandardEditCtrlSW );
		m_bStandardsManuallyChanged = true;
	}
	else if( GetFocus() == &m_dStandardListCtrlNW )
	{
		//NORTH_WEST STANDARD
		DeleteStandardBoxElement( NORTH_WEST, m_dStandardListCtrlNW, CNR_NORTH_WEST, m_StandardEditCtrlNW );
		m_bStandardsManuallyChanged = true;
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
}


bool BayDetailsDialog::DeleteStandardBoxElement( SideOfBayEnum eSide, CListBox	&StandardListCtrl, CornerOfBayEnum eCnr, CEdit &StandardEditCtrl )
{
	int			iStandardPosition, iArrayPosition;
	bool		bSoleboard;
	CString		sStandardHeight;
	doubleArray	daArrangement;

	ChangeMadeToDialog();
	if(m_pBay->GetComponent(CT_SOLEBOARD, eSide, 0.0))
		bSoleboard = true;
	else
		bSoleboard = false;

	iStandardPosition = StandardListCtrl.GetCurSel();

	if( iStandardPosition>0 )
	{
		m_pBay->GetStandardsArrangement(daArrangement, eCnr);
		iArrayPosition = daArrangement.GetSize()-iStandardPosition;

		if( iArrayPosition>=daArrangement.GetSize() ||
			iArrayPosition<0 )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			daArrangement.RemoveAt(iArrayPosition);
			m_pBay->DeleteStandards(eCnr);
			m_pBay->SetStandardsArrangement(daArrangement, eCnr, bSoleboard);
			UpdateStandardsArrangementCorner(&StandardListCtrl, &StandardEditCtrl, eCnr);
		}
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

void BayDetailsDialog::OnStagesPositonDecks() 
{
	UpdateData();
	SetApplyRequired( true );
	ChangeMadeToDialog();
	OnKillfocusTopWorkingHeightEdit();
}

void BayDetailsDialog::OnStagesPositonSouth() 
{
	UpdateData();
	SetApplyRequired( true );
	ChangeMadeToDialog();
	OnKillfocusTopWorkingHeightEdit();
}

int BayDetailsDialog::GetNumberOfLiftFromTWH()
{
	int		iNumberOfLifts;
	double	dTopWorkingHeight, dRemainder, dLiftHeight;

	dTopWorkingHeight = GetTopDeckHeightFromEdit();
	//dTopWorkingHeight-= GetTopBottomStdRL();	//this line removed - see bug 1508
	iNumberOfLifts = ((int)(dTopWorkingHeight/(4*GetStarSeparation())));
	dLiftHeight = ((double)iNumberOfLifts)*4*GetStarSeparation();
	dRemainder	= dTopWorkingHeight-dLiftHeight;
	//Round up
	if( dRemainder>ROUND_ERROR )
		iNumberOfLifts++;
	//lifts are a zero based item, so add another!
	iNumberOfLifts++;

	return iNumberOfLifts;
}

bool BayDetailsDialog::ShowApplyWarning()
{
	return m_bShowApplyWarning;
}

void BayDetailsDialog::SetShowApplyWarning( bool bShow )
{
	m_bShowApplyWarning = bShow;
}


void BayDetailsDialog::EnableStandardEdits( BOOL bEnable )
{
	m_StandardEditCtrlNE.EnableWindow(bEnable);
	m_StandardEditCtrlSE.EnableWindow(bEnable);
	m_StandardEditCtrlSW.EnableWindow(bEnable);
	m_StandardEditCtrlNW.EnableWindow(bEnable);

	m_dStandardListCtrlNE.EnableWindow(bEnable);
	m_dStandardListCtrlSE.EnableWindow(bEnable);
	m_dStandardListCtrlSW.EnableWindow(bEnable);
	m_dStandardListCtrlNW.EnableWindow(bEnable);
/*
	m_SoleboardCheckCtrlNE.EnableWindow(bEnable);
	m_SoleboardCheckCtrlSE.EnableWindow(bEnable);
	m_SoleboardCheckCtrlSW.EnableWindow(bEnable);
	m_SoleboardCheckCtrlNW.EnableWindow(bEnable);
*/
}

bool BayDetailsDialog::CheckBeforeOverwritingStandards()
{
	if( m_bStandardsManuallyChanged )
	{
		CString	sMsg;

		MessageBeep(MB_ICONQUESTION);

		sMsg = _T("This operation will overwrite your modifications to the standards.\n");
		sMsg+= _T("Continue and loose your changes?");
		int iRet = MessageBox( sMsg, _T("Warning!"), MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1 );
		if( iRet!=IDYES )
		{
			return false;
		}

		//We are allowed to modify the standards,
		//	so ignor any changes they have made
		m_bStandardsManuallyChanged = false;
	}
	return true;
}

void BayDetailsDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	
	StoreWindowPositionInReg( _T("BayDetailsDialog") );
	
}

void BayDetailsDialog::OnHandoptionsFullMeshDecksOnly() 
{
	UpdateData( true );
	;//assert( m_iRailOptions==HRO_FULLMESH );

	SetApplyRequired( );
	ChangeMadeToDialog();
}


double BayDetailsDialog::GetTopBottomStdRL()
{
	double dRL;
	dRL = m_pBay->GetStandardPosition(CNR_NORTH_EAST).z;
	dRL = max( dRL, m_pBay->GetStandardPosition(CNR_SOUTH_EAST).z );
	dRL = max( dRL, m_pBay->GetStandardPosition(CNR_SOUTH_WEST).z );
	dRL = max( dRL, m_pBay->GetStandardPosition(CNR_NORTH_WEST).z );
	return dRL;
}

void BayDetailsDialog::OnUseMidrailWithChain() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
	m_pBay->SetUseMidrailWithChainMesh( ConvertBOOLTobool(m_bUseMidrailWithChain) );
}


void BayDetailsDialog::OnLedgersEveryMetre() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
	m_pBay->SetUseLedgerEveryMetre( ConvertBOOLTobool(m_bLedgersEveryMetre) );
}


void BayDetailsDialog::OnMovingDeck() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
	m_pBay->SetHasMovingDeck( ConvertBOOLTobool(m_bMovingDeck) );
}

void BayDetailsDialog::OnRoofProtection() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
}

void BayDetailsDialog::OnRoofProtection1500() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
}

void BayDetailsDialog::OnRoofProtection2000() 
{
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData();
}

void BayDetailsDialog::OnMatchDeckToLevels() 
{
	UpdateData();
	SetMatchDecksToLevels(ConvertBOOLTobool(m_bMatchDecksToLevels));
	SetApplyRequired( );
	ChangeMadeToDialog();
	UpdateData(false);
}

LevelList BayDetailsDialog::GetPossibleDeckRLs()
{
	LevelList llDecks, *pllLevels;
	pllLevels = m_pBay->GetRunPointer()->GetController()->GetLevelList();
	llDecks = *pllLevels;

	//Convert the levels to the nearest deck height below the level
	double dDeck;
	for( int i=0; i<llDecks.GetSize(); i++ )
	{
		dDeck = llDecks.GetLevel(i);
		llDecks.RemoveLevel(i);
		dDeck = double(int(dDeck/GetStarSeparation())*GetStarSeparation());
		dDeck-= m_pBay->RLAdjust();
		llDecks.AddLevel(dDeck);
	}

	//We now know all the POSSIBLE places that we can put a deck
	//	However, we cannot put a deck above TWH or BWH
	double dTWH, dTWRL, dBWRL;
	dTWH	= GetTopDeckHeightFromEdit();
	dBWRL	= GetTopBottomStdRL();
	dTWRL	= dTWH-dBWRL;
	for(int i=0; i<llDecks.GetSize(); i++ )
	{
		dDeck	= llDecks.GetLevel(i);
		if( dDeck>dTWRL+ROUND_ERROR || dDeck<dBWRL-ROUND_ERROR )
		{
			//This deck level is outside the range of this bay!
			llDecks.RemoveLevel(dDeck);
			i--;
		}
	}

	return llDecks;
}

void BayDetailsDialog::SetMatchDecksToLevels(bool bMatch)
{
	m_bMatchDecksToLevels = ConvertboolToBOOL(bMatch);

	//if the match decks to levels is ticked then
	//	South side only cannot be ticked
	if( m_ctrlMatchDecksToLevels.IsWindowEnabled() )
	{
		m_iStagePositioning = STAGE_POS_DECKS_ONLY;
		m_ctrlSouthSideOnly.EnableWindow( !m_bMatchDecksToLevels );
	}
}

double BayDetailsDialog::GetStarSeparation()
{
	return m_pBay->GetStarSeparation();
}
