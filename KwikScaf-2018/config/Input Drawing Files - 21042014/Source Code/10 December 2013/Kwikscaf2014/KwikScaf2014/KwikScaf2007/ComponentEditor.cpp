// ComponentEditor.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "Component.h"
#include "Controller.h"
#include "StockListElement.h"
#include "ComponentEditor.h"
#include "ComponentPartNumberSelectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString	MULTIPLE_SELECT_STRING	= _T("<Multiple Selection>");
const CString	NA_STRING				= _T("<N/A>");
const CString	ACTUAL_NOT_AVAILABLE	= _T("<Not available!>");


/////////////////////////////////////////////////////////////////////////////
// ComponentEditor dialog


ComponentEditor::ComponentEditor(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(ComponentEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(ComponentEditor)
	m_sDetailsMaterial = _T("");
	m_sDetailsDesc = _T("");
	m_sDetailsPartNumber = _T("");
	m_sDetailsType = _T("");
	m_sDimHeightActual = _T("");
	m_sDimLengthActual = _T("");
	m_sDimLengthCommon = _T("");
	m_sDimWeightActual = _T("");
	m_sDimWidthActual = _T("");
	m_sOtherHire = _T("");
	m_sOtherSale = _T("");
	m_sOtherSystem = _T("");
	m_iSLCommitted = FALSE;
	m_sVisualActualCompAvail = _T("");
	m_iVisualsVisible = FALSE;
	m_sStage = _T("");
	//}}AFX_DATA_INIT

	SetComponentList(NULL);
	SetController(NULL);
	SetInsertingComponent(false);
}


void ComponentEditor::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ComponentEditor)
	DDX_Control(pDX, IDC_VC_INSERT_PIC, m_picVClogoInsert);
	DDX_Control(pDX, IDC_VC_EDIT_PIC, m_picVClogoEdit);
	DDX_Control(pDX, IDC_VISUALS_VISIBLE, m_btnVisualsVisible);
	DDX_Control(pDX, IDC_SL_COMMITTED, m_btnCommitted);
	DDX_Control(pDX, IDC_VISUALS_COMPONENT_COLOUR, m_btnVisualsCompColour);
	DDX_Control(pDX, IDC_SL_STAGE_COMBO, m_cmboSLStage);
	DDX_Control(pDX, IDC_SL_LEVEL_COMBO, m_cmboSLLevel);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_DETAILS_MATERIAL, m_sDetailsMaterial);
	DDX_Text(pDX, IDC_DETAILS_DESCRIPTION, m_sDetailsDesc);
	DDX_Text(pDX, IDC_DETAILS_PART_NUMBER, m_sDetailsPartNumber);
	DDX_Text(pDX, IDC_DETAILS_TYPE, m_sDetailsType);
	DDX_Text(pDX, IDC_DIMENSIONS_HEIGHT_ACTUAL, m_sDimHeightActual);
	DDX_Text(pDX, IDC_DIMENSIONS_LENGTH_ACTUAL, m_sDimLengthActual);
	DDX_Text(pDX, IDC_DIMENSIONS_LENGTH_COMMON, m_sDimLengthCommon);
	DDX_Text(pDX, IDC_DIMENSIONS_WEIGHT_ACTUAL, m_sDimWeightActual);
	DDX_Text(pDX, IDC_DIMENSIONS_WIDTH_ACTUAL, m_sDimWidthActual);
	DDX_Text(pDX, IDC_OTHER_HIRE, m_sOtherHire);
	DDX_Text(pDX, IDC_OTHER_SALE, m_sOtherSale);
	DDX_Text(pDX, IDC_OTHER_SYSTEM, m_sOtherSystem);
	DDX_Check(pDX, IDC_SL_COMMITTED, m_iSLCommitted);
	DDX_Text(pDX, IDC_VISUALS_ACTUAL_COMP_AVAIL, m_sVisualActualCompAvail);
	DDX_Check(pDX, IDC_VISUALS_VISIBLE, m_iVisualsVisible);
	DDX_CBString(pDX, IDC_SL_STAGE_COMBO, m_sStage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ComponentEditor, KwikscafDialog)
	//{{AFX_MSG_MAP(ComponentEditor)
	ON_BN_CLICKED(IDC_DETAILS_CHANGE_PART_NO, OnDetailsChangePartNo)
	ON_BN_CLICKED(IDC_SL_COMMITTED, OnStageLevelCommitted)
	ON_CBN_EDITCHANGE(IDC_SL_LEVEL_COMBO, OnEditchangeSLLevelCombo)
	ON_CBN_SELCHANGE(IDC_SL_LEVEL_COMBO, OnSelchangeSLLevelCombo)
	ON_CBN_EDITCHANGE(IDC_SL_STAGE_COMBO, OnEditchangeSLStageCombo)
	ON_CBN_SELCHANGE(IDC_SL_STAGE_COMBO, OnSelchangeSLStageCombo)
	ON_BN_CLICKED(IDC_VISUALS_COMPONENT_COLOUR, OnVisualsComponentColour)
	ON_BN_CLICKED(IDC_VISUALS_VISIBLE, OnVisualsVisible)
	ON_CBN_KILLFOCUS(IDC_SL_STAGE_COMBO, OnKillfocusSlStageCombo)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ComponentEditor message handlers

BOOL ComponentEditor::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("ComponentEditor"));

	assert( m_pclComponentList!=NULL );

	//////////////////////////////////////////////////////////////
	//Load the bitmap
	m_picVClogoEdit.ShowWindow(SW_HIDE);
	m_picVClogoInsert.ShowWindow(SW_HIDE);
	m_btnVisualsVisible.EnableWindow( true );
	if( IsInsertingComponent() )
	{
		SetWindowText( _T("Insert Visual Component") );
		m_picVClogoInsert.ShowWindow(SW_SHOW);
		m_iVisualsVisible = TRUE;
		m_btnVisualsVisible.EnableWindow( false );
	}
	else
	{
		SetWindowText( _T("Visual Component Editor") );
		m_picVClogoEdit.ShowWindow(SW_SHOW);
	}

	m_btnVisualsVisible.SetButtonStyle(BS_AUTOCHECKBOX);
	m_btnCommitted.SetButtonStyle(BS_AUTOCHECKBOX);
	
	if( IsInsertingComponent() )
	{
		m_btnOK.SetWindowText( _T("Insert") );
		m_btnOK.EnableWindow( true );
		m_btnCancel.SetWindowText( _T("Cancel") );
		m_btnCancel.EnableWindow( true );
	}
	else
	{
		m_btnOK.SetWindowText( _T("Apply") );
		m_btnOK.EnableWindow( false );
		m_btnCancel.SetWindowText( _T("Cancel") );
		m_btnCancel.EnableWindow( true );
	}
	
	FillSLLevelList();
	FillSLStageList();
	CString sPN;
	sPN.Empty();
	FillDetails(sPN);

	if( IsInsertingComponent() )
	{
		OnDetailsChangePartNo();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ComponentEditor::OnDetailsChangePartNo() 
{
	UpdateData();

	ComponentPartNumberSelectorDlg dialog;
	dialog.SetPartNumber( m_sDetailsPartNumber );
	if( dialog.DoModal()==IDOK )
	{
		CString sPN;
		dialog.GetPartNumber( sPN );
		if( sPN!=m_sDetailsPartNumber )
		{
			m_sDetailsPartNumber=sPN;
			UpdateData(false);
			FillDetails(sPN);
			DialogHasChanged(true);
		}
	}
}

void ComponentEditor::OnStageLevelCommitted() 
{
	DialogHasChanged(true);
}

void ComponentEditor::OnEditchangeSLLevelCombo() 
{
	DialogHasChanged(true);
}

void ComponentEditor::OnSelchangeSLLevelCombo() 
{
	DialogHasChanged(true);
}

void ComponentEditor::OnEditchangeSLStageCombo() 
{
	DialogHasChanged(true);
}

void ComponentEditor::OnSelchangeSLStageCombo() 
{
	DialogHasChanged(true);
}

void ComponentEditor::OnVisualsComponentColour() 
{
	int iColour;
	iColour = m_btnVisualsCompColour.GetAutoCADColour();
	acedSetColorDialog( iColour, Adesk::kFalse, 0 );
 	m_btnVisualsCompColour.SetAutoCADColour( iColour );
	m_btnVisualsCompColour.RedrawWindow();
	UpdateData(false);
	DialogHasChanged(true);
}

void ComponentEditor::OnVisualsVisible() 
{
	//m_bVisualVisibleMultiselect;
	DialogHasChanged(true);
}


void ComponentEditor::DialogHasChanged(bool bChanged)
{
	if( !IsInsertingComponent() )
	{
		if( bChanged )
		{
			m_btnCancel.SetWindowText( _T("Close") );
			m_btnOK.SetWindowText( _T("Apply") );
			m_btnCancel.EnableWindow( true );
			m_btnOK.EnableWindow( true );
		}
		else
		{
			m_btnOK.SetWindowText( _T("Apply") );
			m_btnCancel.EnableWindow( true );
			m_btnOK.EnableWindow( false );
		}
	}
}

void ComponentEditor::OnOK() 
{
	int						i;
	bool					bChanged;
	double					dCommonLength;
	Component				*pComponent;
	SystemEnum				eSystem;
	MaterialTypeEnum		eMaterial;
	ComponentTypeEnum		eType;
	ComponentDetailsElement	*pCDE;

	///////////////////////////////////////////////////////////////////////////
	//Partnumber
	if( IsInsertingComponent() ||
		  (	m_sDetailsPartNumber!=MULTIPLE_SELECT_STRING &&
			m_sDetailsPartNumber!=m_pclComponentList->GetAt(0)->GetStockDetails()->GetPartNumber()) )
	{
		//The Part number has changed
		pCDE = GetController()->GetCompDetails()->GetMatchingComponent(m_sDetailsPartNumber);
		if( pCDE==NULL )
		{
			//There is no matching info in the component details file
			eType			= CT_VISUAL_COMPONENT;
			eMaterial		= MT_OTHER;
			dCommonLength	= 0;
			eSystem			= S_OTHER;
		}
		else
		{
			eType			= pCDE->m_eCompType;
			eMaterial		= pCDE->m_eMaterialType;
			dCommonLength	= pCDE->m_dCommonLength;
			if( m_sDetailsPartNumber==pCDE->m_sKwikstagePartNumber )
				eSystem = S_KWIKSTAGE;
			else if( m_sDetailsPartNumber==pCDE->m_sMillsPartNumber )
				eSystem = S_MILLS;
			else if( m_sDetailsPartNumber==pCDE->m_sOtherPartNumber )
				eSystem = S_OTHER;
			else
				eSystem = S_OTHER;
		}
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetSystem(eSystem);
			pComponent->SetComponent( dCommonLength, eType, eMaterial );
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Commit
	int iCommitState = m_btnCommitted.GetState()&0x3;
	if( IsInsertingComponent() || ( iCommitState!=2 && (iCommitState!=(m_pclComponentList->GetAt(0)->IsCommitted()? 1: 0)) ) )
	{
		//Committed has been changed
		assert( iCommitState==0 || iCommitState==1 );
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetCommitted( iCommitState==1? true: false );
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Visible
	int iVisible = m_btnVisualsVisible.GetState()&0x3;
	if( IsInsertingComponent() )
	{
		assert( iVisible==1 );
	}
	if( IsInsertingComponent() || ( iVisible!=2 && (iVisible!=((m_pclComponentList->GetAt(0)->GetVisible()==VIS_VISIBLE)? 1: 0)) ) )
	{
		//Visibility has been changed
		assert( iVisible==0 || iVisible==1 );
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetVisible( iVisible==1? VIS_VISIBLE: VIS_NOT_VISIBLE );
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Level
	int iSelection = m_cmboSLLevel.GetCurSel();
	bChanged = false;
	if( m_bSLLevelMultiExists && iSelection!=0 )
	{
		//multilevel was present, it was not selected
		bChanged = true;
	}
	if( !m_bSLLevelMultiExists )
		iSelection++;
	else if( iSelection!=m_pclComponentList->GetAt(0)->GetLevel() )
	{
		//The level for the entire selection has changed
		bChanged = true;
	}
	if( IsInsertingComponent() )
	{
		bChanged = true;
	}
	if( bChanged )
	{
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetLevel(iSelection);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Stage
	CString sStg;
	iSelection = m_cmboSLStage.GetCurSel();
	bChanged = false;
	sStg = m_pclComponentList->GetAt(0)->GetStage();
	if( sStg.IsEmpty() )
		sStg = STAGE_DEFAULT_VALUE;
	if( m_bSLStageMultiExists && m_sStage!=STAGE_MULTIPLE_SELECTION )
	{
		//multilevel was present, it was not selected
		bChanged = true;
	}
	else if( m_sStage!=sStg )
	{
		//The level for the entire selection has changed
		bChanged = true;
	}
	if( IsInsertingComponent() )
	{
		bChanged = true;
	}
	if( bChanged )
	{
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetStage(m_sStage);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Colour
	iSelection = m_btnVisualsCompColour.GetAutoCADColour();
	int iColour = m_pclComponentList->GetAt(0)->GetColour();
	if( iSelection>=0 && ( iSelection!=iColour || IsInsertingComponent() ) )
	{
		for( i=0; i<m_pclComponentList->GetSize(); i++ )
		{
			pComponent = m_pclComponentList->GetAt(i);
			pComponent->SetColour( iSelection );
		}
	}

	if( IsInsertingComponent() )
	{
		KwikscafDialog::OnOK();
	}
	else
	{
		DialogHasChanged(false);
	}

	//Update the 3D
	for( i=0; i<m_pclComponentList->GetSize(); i++ )
	{
		pComponent = m_pclComponentList->GetAt(i);

		if( pComponent->GetStockDetails()==NULL )
			//try the usual method to get the stock code
			pComponent->SetStockDetailsPointer();

		//if that didn't work lets go directly to the source
		if( pComponent->GetStockDetails()==NULL )
			pComponent->SetStockDetailsPointerDumb( GetController()->GetStockList()->GetMatchingComponent( m_sDetailsPartNumber ) );

		if( pComponent->GetStockDetails()==NULL )
		{
			//How the hell did this happen? they have just inserted a
			//	component that is not in the stock file!
			assert( false );
		}
		else
		{
			pComponent->Delete3DView();
			pComponent->Create3DView();
		}
	}
}

void ComponentEditor::OnCancel() 
{
	//Close the dialog
	KwikscafDialog::OnCancel();
}

void ComponentEditor::SetComponentList( CompArray *pCompList )
{
	m_pclComponentList = pCompList;
}


void ComponentEditor::FillDetails( CString sPartNumber )
{
	int						i, iComponent;
	CString					sCompDetails, sComponent;
	Component				*pComponent;
	StockListElement		*pStckEle;
	ComponentDetailsElement	*pCDE;

	m_sDetailsPartNumber		= _T("");
	m_sDetailsDesc				= _T("");
	m_sDetailsMaterial			= _T("");
	m_sDetailsType				= _T("");
	m_sDimLengthCommon			= _T("");
	m_sDimLengthActual			= _T("");
	m_sDimWidthActual			= _T("");
	m_sDimHeightActual			= _T("");
	m_sDimWeightActual			= _T("");
	m_sOtherHire				= _T("");
	m_sOtherSale				= _T("");
	m_sOtherSystem				= _T("");
	m_iSLCommitted				= FALSE;
	m_sVisualActualCompAvail	= _T("");
	m_iVisualsVisible			= FALSE;
	m_bSLLevelMultiExists			= false;
	m_bSLStageMultiExists			= false;
	m_bSLCommittedMultiselect		= false;
	m_bVisualVisibleMultiselect		= false;

	if( !sPartNumber.IsEmpty() )
	{
		/////////////////////////////////////////////////////////////////
		//Part Number
		m_sDetailsPartNumber = sPartNumber;

		pStckEle = GetController()->GetStockList()->GetMatchingComponent( sPartNumber );
		assert( pStckEle!=NULL );
		pCDE = GetController()->GetCompDetails()->GetMatchingComponent(sPartNumber);

		/////////////////////////////////////////////////////////////////
		//Description
		sComponent = pStckEle->GetDescription();
		assert( !sComponent.IsEmpty() );
		m_sDetailsDesc = sComponent;

		/////////////////////////////////////////////////////////////////
		//Material
		if( pCDE==NULL )
			sComponent = NA_STRING;
		else
			sComponent = GetComponentMaterialStr( pCDE->m_eMaterialType );
		assert( !sComponent.IsEmpty() );
		m_sDetailsMaterial = sComponent;

		/////////////////////////////////////////////////////////////////
		//Type
		if( pCDE==NULL )
			sComponent = NA_STRING;
		else
			sComponent = GetComponentDescStr( pCDE->m_eCompType );
		m_sDetailsType = sComponent;

		/////////////////////////////////////////////////////////////////
		//Common Lenght
		if( pCDE==NULL )
			sComponent = NA_STRING;
		else
			sComponent.Format( _T("%2.0fmm"), pCDE->m_dCommonLength );
		assert( !sComponent.IsEmpty() );
		m_sDimLengthCommon = sComponent;

		/////////////////////////////////////////////////////////////////
		//Actual Lenght
		sComponent.Format( _T("%2.0fmm"), pStckEle->GetLength() );
		assert( !sComponent.IsEmpty() );
		m_sDimLengthActual = sComponent;

		/////////////////////////////////////////////////////////////////
		//Actual Width
		sComponent.Format( _T("%2.0fmm"), pStckEle->GetWidth() );
		assert( !sComponent.IsEmpty() );
		m_sDimWidthActual = sComponent;

		/////////////////////////////////////////////////////////////////
		//Actual Height
		sComponent.Format( _T("%2.0fmm"), pStckEle->GetHeight() );
		assert( !sComponent.IsEmpty() );
		m_sDimHeightActual = sComponent;

		/////////////////////////////////////////////////////////////////
		//Actual Weight
		sComponent.Format( _T("%2.0fmm"), pStckEle->GetWeight() );
		assert( !sComponent.IsEmpty() );
		m_sDimWeightActual = sComponent;

		/////////////////////////////////////////////////////////////////
		//Hire Price
		sComponent.Format( _T("$%2.02f"), pStckEle->GetHire() );
		assert( !sComponent.IsEmpty() );
		m_sOtherHire = sComponent;

		/////////////////////////////////////////////////////////////////
		//Sale Price
		sComponent.Format( _T("$%2.02f"), pStckEle->GetSale() );
		assert( !sComponent.IsEmpty() );
		m_sOtherSale = sComponent;

		/////////////////////////////////////////////////////////////////
		//System
		if( pCDE==NULL )
		{
			m_sOtherSystem = SYSTEM_TEXT_OTHER;
		}
		else
		{
			if( sPartNumber==pCDE->m_sKwikstagePartNumber )
			{
				m_sOtherSystem = SYSTEM_TEXT_KWIKSTAGE;
			}
			else if( sPartNumber==pCDE->m_sMillsPartNumber )
			{
				m_sOtherSystem = SYSTEM_TEXT_MILLS;
			}
			else if( sPartNumber==pCDE->m_sOtherPartNumber )
			{
				m_sOtherSystem = SYSTEM_TEXT_OTHER;
			}
			else
			{
				assert( false );
				m_sOtherSystem = SYSTEM_TEXT_OTHER;
			}
		}

		/////////////////////////////////////////////////////////////////
		//Actual component available
		sComponent = pStckEle->GetFilename3D();
		if( sComponent.IsEmpty() )
		{
			sComponent = ACTUAL_NOT_AVAILABLE;
		}
		m_sVisualActualCompAvail = sComponent;

		/////////////////////////////////////////////////////////////////
		//Colour
		if( pCDE==NULL )
		{
			m_btnVisualsCompColour.SetAutoCADColour( COLOUR_MULTISELECT );
		}
		else
		{
			iComponent = GetComponentColour( pCDE->m_eCompType );
			m_btnVisualsCompColour.SetAutoCADColour( iComponent );
		}
	}

	for( i=0; i<m_pclComponentList->GetSize(); i++ )
	{
		pComponent = m_pclComponentList->GetAt(i);

		pStckEle	= pComponent->GetStockDetails();
		assert( pStckEle!=NULL );

		/////////////////////////////////////////////////////////////////
		//Part No.
		sComponent = pStckEle->GetPartNumber();
		assert( !sComponent.IsEmpty() );

		if( sPartNumber.IsEmpty() )
		{
			if( m_sDetailsPartNumber.IsEmpty() )
				m_sDetailsPartNumber = sComponent;
			else if( sComponent!=m_sDetailsPartNumber )
				m_sDetailsPartNumber = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//We now need to find the part number in the component details
			pCDE = GetController()->GetCompDetails()->GetMatchingComponent(sComponent);

			/////////////////////////////////////////////////////////////////
			//Description
			sComponent = pStckEle->GetDescription();
			assert( !sComponent.IsEmpty() );
			if( m_sDetailsDesc.IsEmpty() )
				m_sDetailsDesc = sComponent;
			else if( sComponent!=m_sDetailsDesc )
				m_sDetailsDesc = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Material
			if( pCDE==NULL )
				sComponent = NA_STRING;
			else
				sComponent = GetComponentMaterialStr( pCDE->m_eMaterialType );
			assert( !sComponent.IsEmpty() );
			if( m_sDetailsMaterial.IsEmpty() )
				m_sDetailsMaterial = sComponent;
			else if( sComponent!=m_sDetailsMaterial )
				m_sDetailsMaterial = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Type
			if( pCDE==NULL )
				sComponent = NA_STRING;
			else
				sComponent = GetComponentDescStr( pCDE->m_eCompType );
			assert( !sComponent.IsEmpty() );
			if( m_sDetailsType.IsEmpty() )
				m_sDetailsType = sComponent;
			else if( sComponent!=m_sDetailsType )
				m_sDetailsType = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Common Lenght
			if( pCDE==NULL )
				sComponent = NA_STRING;
			else
				sComponent.Format( _T("%2.0fmm"), pCDE->m_dCommonLength );
			assert( !sComponent.IsEmpty() );
			if( m_sDimLengthCommon.IsEmpty() )
				m_sDimLengthCommon = sComponent;
			else if( sComponent!=m_sDimLengthCommon )
				m_sDimLengthCommon = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Actual Lenght
			sComponent.Format( _T("%2.0fmm"), pStckEle->GetLength() );
			assert( !sComponent.IsEmpty() );
			if( m_sDimLengthActual.IsEmpty() )
				m_sDimLengthActual = sComponent;
			else if( sComponent!=m_sDimLengthActual )
				m_sDimLengthActual = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Actual Width
			sComponent.Format( _T("%2.0fmm"), pStckEle->GetWidth() );
			assert( !sComponent.IsEmpty() );
			if( m_sDimWidthActual.IsEmpty() )
				m_sDimWidthActual = sComponent;
			else if( sComponent!=m_sDimWidthActual )
				m_sDimWidthActual = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Actual Height
			sComponent.Format( _T("%2.0fmm"), pStckEle->GetHeight() );
			assert( !sComponent.IsEmpty() );
			if( m_sDimHeightActual.IsEmpty() )
				m_sDimHeightActual = sComponent;
			else if( sComponent!=m_sDimHeightActual )
				m_sDimHeightActual = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Actual Weight
			sComponent.Format( _T("%2.0fmm"), pStckEle->GetWeight() );
			assert( !sComponent.IsEmpty() );
			if( m_sDimWeightActual.IsEmpty() )
				m_sDimWeightActual = sComponent;
			else if( sComponent!=m_sDimWeightActual )
				m_sDimWeightActual = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Hire Price
			sComponent.Format( _T("$%2.02f"), pStckEle->GetHire() );
			assert( !sComponent.IsEmpty() );
			if( m_sOtherHire.IsEmpty() )
				m_sOtherHire = sComponent;
			else if( sComponent!=m_sOtherHire )
				m_sOtherHire = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Sale Price
			sComponent.Format( _T("$%2.02f"), pStckEle->GetSale() );
			assert( !sComponent.IsEmpty() );
			if( m_sOtherSale.IsEmpty() )
				m_sOtherSale = sComponent;
			else if( sComponent!=m_sOtherSale )
				m_sOtherSale = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//System
			switch( pComponent->GetSystem() )
			{
			case( S_MILLS ):
				sComponent = SYSTEM_TEXT_MILLS;
				break;
			case( S_KWIKSTAGE ):
				sComponent = SYSTEM_TEXT_KWIKSTAGE;
				break;
			case( S_OTHER ):
				sComponent = SYSTEM_TEXT_OTHER;
				break;
			default:
				assert( false );
			}
			if( m_sOtherSystem.IsEmpty() )
				m_sOtherSystem = sComponent;
			else if( sComponent!=m_sOtherSystem )
				m_sOtherSystem = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Actual component available
			sComponent = pStckEle->GetFilename3D();
			if( sComponent.IsEmpty() )
			{
				sComponent = ACTUAL_NOT_AVAILABLE;
			}
			if( m_sVisualActualCompAvail.IsEmpty() )
				m_sVisualActualCompAvail = sComponent;
			else if( sComponent!=m_sVisualActualCompAvail )
				m_sVisualActualCompAvail = MULTIPLE_SELECT_STRING;

			/////////////////////////////////////////////////////////////////
			//Colour
			iComponent = pComponent->GetColour();
			if( i==0 )
			{
				m_btnVisualsCompColour.SetAutoCADColour( iComponent );
			}
			else
			{
				if( iComponent!=m_btnVisualsCompColour.GetAutoCADColour() )
				{
					m_btnVisualsCompColour.SetAutoCADColour( COLOUR_MULTISELECT );
				}
			}
		}

		/////////////////////////////////////////////////////////////////
		//Committed
		iComponent = ( pComponent->IsCommitted() )? true: false;
		if( i==0 )
		{
			m_iSLCommitted=iComponent;
		}
		else
		{
			if( m_iSLCommitted!=iComponent )
			{
				m_btnCommitted.SetButtonStyle(BS_AUTO3STATE);
				m_btnCommitted.SetCheck( 2 );
				m_bSLCommittedMultiselect = true;
			}
		}

		/////////////////////////////////////////////////////////////////
		//Visible
		iComponent = ( pComponent->GetVisible()==VIS_VISIBLE )? true: false;
		if( i==0 )
		{
			m_iVisualsVisible=iComponent;
		}
		else
		{
			if( m_iVisualsVisible!=iComponent )
			{
				m_btnVisualsVisible.SetButtonStyle(BS_AUTO3STATE);
				m_btnVisualsVisible.SetCheck( 2 );
				m_bVisualVisibleMultiselect = true;
			}
		}

		/////////////////////////////////////////////////////////////////
		//S&L Level
		iComponent = pComponent->GetLevel();
		sComponent.Format( _T("Level%i"), iComponent );
		iComponent = m_cmboSLLevel.FindString( 0, sComponent );
		if( iComponent==CB_ERR || iComponent<0 )
		{
			//should have been an existing level
			assert( false );
			//default to level 1
			iComponent = m_bSLLevelMultiExists? 0: 1;
		}
		if( i==0 )
		{
			assert( iComponent>=0 );
			assert( !m_bSLLevelMultiExists );
		}
		else
		{
			if( iComponent!=m_cmboSLLevel.GetCurSel() )
			{
				//the selection does not match the previous selections
				m_cmboSLLevel.InsertString( 0, LEVEL_MULTIPLE_SELECTION );
				m_bSLLevelMultiExists = true;
				iComponent = 0;
			}
		}
		m_cmboSLLevel.SetCurSel(iComponent);

		/////////////////////////////////////////////////////////////////
		//S&L Stage
		sComponent = pComponent->GetStage();
		if( sComponent.IsEmpty() )
			sComponent = STAGE_DEFAULT_VALUE;
		iComponent = m_cmboSLStage.FindString( 0, sComponent );
		if( iComponent==CB_ERR || iComponent<0 )
		{
			//should have been an existing level
			assert( false );
			//default to level 1
			iComponent = m_bSLStageMultiExists? 0: 1;
		}
		if( i==0 )
		{
			assert( iComponent>=0 );
			assert( !m_bSLStageMultiExists );
		}
		else
		{
			if( iComponent!=m_cmboSLStage.GetCurSel() )
			{
				//the selection does not match the previous selections
				m_cmboSLStage.InsertString( 0, STAGE_MULTIPLE_SELECTION );
				m_bSLStageMultiExists = true;
				iComponent = 0;
			}
		}
		m_cmboSLStage.SetCurSel(iComponent);
		m_cmboSLStage.GetLBText( iComponent, m_sStage );
	}

	UpdateData(false);
}

Controller * ComponentEditor::GetController()
{
	assert( m_pController!=NULL );	//This must be set before use
	return m_pController;
}

void ComponentEditor::SetController(Controller *pController)
{
	m_pController = pController;
}	

void ComponentEditor::FillSLLevelList()
{
	int			i;
	CString		sLevel;
	LevelList	*pLevels;

	m_cmboSLLevel.Clear();
	m_cmboSLLevel.AddString( _T("Level1") );

	assert( GetController()!=NULL );
	pLevels = GetController()->GetLevelList();

	for( i=0; i<pLevels->GetSize(); i++ )
	{
		sLevel.Format( _T("Level%i"), (i+2) );
		m_cmboSLLevel.AddString( sLevel );
	}
}

void ComponentEditor::FillSLStageList()
{
	int				i;
	CString			sStage;

	m_cmboSLStage.Clear();
	m_cmboSLStage.AddString( STAGE_DEFAULT_VALUE );

	for( i=0; i<GetController()->GetNumberOfStages(); i++ )
	{
		sStage = GetController()->GetStage(i);
		m_cmboSLStage.AddString( sStage );
	}
}

void ComponentEditor::OnKillfocusSlStageCombo() 
{
	int		i;
	bool	bFound;
	CString	sStage, sStageOld;

	UpdateData();

	bFound = false;
	assert( GetController()!=NULL );
	for( i=0; i<GetController()->GetNumberOfStages(); i++ )
	{
		sStage = GetController()->GetStage(i);
		if( sStage==m_sStage )
		{
			bFound = true;
			break;
		}
	}

	if( !bFound && !m_sStage.IsEmpty() && m_sStage!=STAGE_DEFAULT_VALUE 
		&& m_sStage!=STAGE_MULTIPLE_SELECTION )
	{
		GetController()->AddStage( m_sStage );
		m_cmboSLStage.AddString( m_sStage );
	}

	UpdateData(false);
}

bool ComponentEditor::IsInsertingComponent()
{
	return m_bInsertingComponent;
}

void ComponentEditor::SetInsertingComponent(bool bInserting)
{
	m_bInsertingComponent = bInserting;
}

void ComponentEditor::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("ComponentEditor") );
}
