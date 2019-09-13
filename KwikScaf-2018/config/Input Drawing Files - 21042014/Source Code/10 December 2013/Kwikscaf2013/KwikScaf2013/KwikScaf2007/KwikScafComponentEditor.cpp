// KwikScafComponentEditor.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "KwikScafComponentEditor.h"
#include "StockListElement.h"
#include "Controller.h"
#include "ComponentPartNumberSelectorDlg.h"
//#include "MaterialTypeDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Controller *gpController;

const CString	KS_STOCK_COMP_FILE_PREFIX		= _T("!KwikScaf Stock Component File");
const CString	KS_STOCK_COMP_PREFIX_VERSION	= _T("!Version,");

#define MEMFILE_GROW_INCREMENT 10240  // 10K bytes inc.

const CString	DEFAULT_ACF2D	= _T("Enter filename of side on Matrix component");
const CString	DEFAULT_ACF2DEO	= _T("Enter filename of end on Matrix component");
const CString	DEFAULT_ACF3D	= _T("Enter filename of 3D component");
const CString	FILENAME_NOT_AVAIL	= _T("<N/A>");

const CString	DEFAULT_DETAILS_DESCRIPTION	= _T("Enter component's description");
const CString	DEFAULT_DETAILS_PART_NUMBER	= _T("Enter unique Part#");

const CString	DEFAULT_OTHER_HIRE	= _T("Hire Price");
const CString	DEFAULT_OTHER_SALE	= _T("Sale Price");

const CString	DEFAULT_DIM_Z		= _T("Enter wireframe height");
const CString	DEFAULT_DIM_X		= _T("Enter wireframe length");
const CString	DEFAULT_DIM_Y		= _T("Enter wireframe width");
const CString	DEFAULT_DIM_WEIGHT	= _T("Enter Weight");

const ComponentTypeEnum	DEFAULT_DETAILS_TYPE		= CT_BOM_EXTRA;
const MaterialTypeEnum	DEFAULT_DETAILS_MATERIAL	= MT_OTHER;
const CString			DEFAULT_DIM_LENGTH_COMMON	= _T("n/a");

/////////////////////////////////////////////////////////////////////////////
// KwikScafComponentEditor dialog


KwikScafComponentEditor::KwikScafComponentEditor( StockListArray *pStockArray, CString sPartNumber, CWnd* pParent /*=NULL*/)
	: KwikscafDialog(KwikScafComponentEditor::IDD, pParent)
{
	m_bEditSalePriceOnly = false;

	//{{AFX_DATA_INIT(KwikScafComponentEditor)
	m_sACF2D = _T("");
	m_sACF2Deo = _T("");
	m_sACF3D = _T("");
	m_sDetailsType = _T("");
	m_sDetailsDescription = _T("");
	m_sDetailsMaterial = _T("");
	m_sDetailsPartNumber = _T("");
	m_sDimLengthCommon = _T("");
	m_sOtherHire = _T("");
	m_sOtherSale = _T("");
	m_sDimWeight = _T("");
	m_sDimZ = _T("");
	m_sDimX = _T("");
	m_sDimY = _T("");
	m_bUseSalePrice = FALSE;
	//}}AFX_DATA_INIT

	assert( pStockArray!=NULL );
	m_pStockListArray = pStockArray;

	//We must have a part number!
	if( sPartNumber.IsEmpty() )
	{
		ComponentDetailsElement *pCDEle;
		pCDEle = gpController->GetCompDetails()->GetMatchingComponent( CT_LEDGER, 2400, MT_STEEL, false );
		if( pCDEle==NULL )
		{
			assert( gpController->GetCompDetails()->GetSize()>0 );
			pCDEle = gpController->GetCompDetails()->GetAt(0);
		}

		if( pCDEle!=NULL )
		{
			switch( gpController->GetSystem() )
			{
			case( S_KWIKSTAGE ):
				sPartNumber = pCDEle->m_sKwikstagePartNumber;
				break;
			case( S_MILLS ):
				sPartNumber = pCDEle->m_sMillsPartNumber;
				break;
			case( S_OTHER ):
				sPartNumber = pCDEle->m_sOtherPartNumber;
				break;
			case( S_INVALID ):
			default:
				assert( false );
				sPartNumber = pCDEle->m_sKwikstagePartNumber;
				break;
			}
		}
		else
		{
			assert( false );
		}
	}

	//Set the current element
	assert( !sPartNumber.IsEmpty() );
	SetStockListElement( m_pStockListArray->GetMatchingComponent( sPartNumber ), false, false );

	SetNewComponent(false);

	m_bUseSalePrice = FALSE;
}


void KwikScafComponentEditor::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KwikScafComponentEditor)
	DDX_Control(pDX, IDC_STORAGE_IMPORT, m_btnImport);
	DDX_Control(pDX, IDC_STORAGE_EXPORT, m_btnExport);
	DDX_Control(pDX, IDC_OTHER_USE_SALE_PRICE, m_btnUseSalePrice);
	DDX_Control(pDX, IDC_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_APPLY_CHANGES, m_btnApply);
	DDX_Control(pDX, IDC_OTHER_SALE, m_editOtherSale);
	DDX_Control(pDX, IDC_OTHER_HIRE, m_editOtherHire);
	DDX_Control(pDX, IDC_DIM_WIDTH, m_editDimY);
	DDX_Control(pDX, IDC_DIM_WEIGHT, m_editDimWeight);
	DDX_Control(pDX, IDC_DIM_LENGTH_COMMON, m_editDimLenthCommon);
	DDX_Control(pDX, IDC_DIM_LENGTH_ACTUAL, m_editDimX);
	DDX_Control(pDX, IDC_DIM_HEIGHT, m_editDimZ);
	DDX_Control(pDX, IDC_DETAILS_TYPE, m_editDetailsType);
	DDX_Control(pDX, IDC_DETAILS_PART_NUMBER, m_editDetailsPartNumber);
	DDX_Control(pDX, IDC_DETAILS_MATERIAL, m_editDetailsMaterail);
	DDX_Control(pDX, IDC_DETAILS_DESCRIPTION, m_editDetailsDescription);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_ACF_3D_BROWSE, m_btnACF3DBrowse);
	DDX_Control(pDX, IDC_ACF_3D, m_editACF_3D);
	DDX_Control(pDX, IDC_ACF_2DEO_BROWSE, m_btnACF2DeoBrowse);
	DDX_Control(pDX, IDC_ACF_2D_BROWSE, m_btnACF2DBrowse);
	DDX_Control(pDX, IDC_ACF_2DEO, m_editACF_2DEO);
	DDX_Control(pDX, IDC_ACF_2D, m_editACF_2D);
	DDX_Text(pDX, IDC_ACF_2D, m_sACF2D);
	DDX_Text(pDX, IDC_ACF_2DEO, m_sACF2Deo);
	DDX_Text(pDX, IDC_ACF_3D, m_sACF3D);
	DDX_Text(pDX, IDC_DETAILS_TYPE, m_sDetailsType);
	DDX_Text(pDX, IDC_DETAILS_DESCRIPTION, m_sDetailsDescription);
	DDX_Text(pDX, IDC_DETAILS_MATERIAL, m_sDetailsMaterial);
	DDX_Text(pDX, IDC_DETAILS_PART_NUMBER, m_sDetailsPartNumber);
	DDX_Text(pDX, IDC_DIM_LENGTH_COMMON, m_sDimLengthCommon);
	DDX_Text(pDX, IDC_OTHER_HIRE, m_sOtherHire);
	DDX_Text(pDX, IDC_OTHER_SALE, m_sOtherSale);
	DDX_Text(pDX, IDC_DIM_WEIGHT, m_sDimWeight);
	DDX_Text(pDX, IDC_DIM_HEIGHT, m_sDimZ);
	DDX_Text(pDX, IDC_DIM_LENGTH_ACTUAL, m_sDimX);
	DDX_Text(pDX, IDC_DIM_WIDTH, m_sDimY);
	DDX_Check(pDX, IDC_OTHER_USE_SALE_PRICE, m_bUseSalePrice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KwikScafComponentEditor, KwikscafDialog)
	//{{AFX_MSG_MAP(KwikScafComponentEditor)
	ON_BN_CLICKED(IDC_ACF_2D_BROWSE, OnAcf2dBrowse)
	ON_BN_CLICKED(IDC_ACF_2DEO_BROWSE, OnAcf2deoBrowse)
	ON_BN_CLICKED(IDC_ACF_3D_BROWSE, OnAcf3dBrowse)
	ON_BN_CLICKED(IDC_DETAILS_SELECT_PART_NO, OnDetailsSelectPartNo)
	ON_BN_CLICKED(IDC_NEW_COMPONENT, OnNewComponent)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_ACF_2D, OnChangeAcf2d)
	ON_EN_CHANGE(IDC_ACF_2DEO, OnChangeAcf2deo)
	ON_EN_CHANGE(IDC_ACF_3D, OnChangeAcf3d)
	ON_EN_CHANGE(IDC_DETAILS_DESCRIPTION, OnChangeDetailsDescription)
	ON_EN_CHANGE(IDC_DETAILS_PART_NUMBER, OnChangeDetailsPartNumber)
	ON_EN_CHANGE(IDC_DIM_HEIGHT, OnChangeDimHeight)
	ON_EN_CHANGE(IDC_DIM_LENGTH_ACTUAL, OnChangeDimLengthActual)
	ON_EN_CHANGE(IDC_DIM_WEIGHT, OnChangeDimWeight)
	ON_EN_CHANGE(IDC_DIM_WIDTH, OnChangeDimWidth)
	ON_EN_CHANGE(IDC_OTHER_HIRE, OnChangeOtherHire)
	ON_EN_CHANGE(IDC_OTHER_SALE, OnChangeOtherSale)
	ON_BN_CLICKED(IDC_APPLY_CHANGES, OnApplyChanges)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_OTHER_USE_SALE_PRICE, OnOtherUseSalePrice)
	ON_BN_CLICKED(IDC_STORAGE_EXPORT, OnStorageExport)
	ON_BN_CLICKED(IDC_STORAGE_IMPORT, OnStorageImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KwikScafComponentEditor message handlers

void KwikScafComponentEditor::OnAcf2dBrowse() 
{
	UpdateData();
	m_sACF2D = GetFilename();
	UpdateData( false );
	OnChangeAcf2d();
}

void KwikScafComponentEditor::OnAcf2deoBrowse() 
{
	UpdateData();
	m_sACF2Deo = GetFilename();
	UpdateData( false );
	OnChangeAcf2deo();
}

void KwikScafComponentEditor::OnAcf3dBrowse() 
{
	UpdateData();
	m_sACF3D = GetFilename();
	UpdateData( false );
	OnChangeAcf3d();
}

void KwikScafComponentEditor::OnDetailsSelectPartNo() 
{
	if( !AllowExit() )
		return;

	CString							sPartNumber;
	ComponentPartNumberSelectorDlg	Dlg;

	assert( GetCurrentElement()!=NULL );
	sPartNumber = GetCurrentElement()->GetPartNumber();

	Dlg.SetPartNumber( sPartNumber );
	Dlg.SetAllowNonVisual(true);
	if( Dlg.DoModal()==IDOK )
	{
		Dlg.GetPartNumber( sPartNumber );
		SetStockListElement(m_pStockListArray->GetMatchingComponent( sPartNumber ), true );
		FillDialog();
	}
}

void KwikScafComponentEditor::OnClose() 
{
	if( !AllowExit() )
		return;

	EndDialog(IDOK);
}

void KwikScafComponentEditor::OnApplyChanges() 
{
	ApplyChanges();
}

BOOL KwikScafComponentEditor::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	
	assert( GetCurrentElement()!=NULL );
	FillDialog();
	
	GetWindowPositionInReg(_T("KwikScafComponentEditor"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KwikScafComponentEditor::OnNewComponent() 
{
	if( !AllowExit() )
		return;

	m_pCurrentStockElement = NULL;
	SetNewComponent(true);

	m_sACF2D	= DEFAULT_ACF2D;
	m_sACF2Deo	= DEFAULT_ACF2DEO;
	m_sACF3D	= DEFAULT_ACF3D;

	m_sDetailsDescription	= DEFAULT_DETAILS_DESCRIPTION;
	m_sDetailsPartNumber	= DEFAULT_DETAILS_PART_NUMBER;

	m_sOtherHire = DEFAULT_OTHER_HIRE;
	m_sOtherSale = DEFAULT_OTHER_SALE;
	m_sDimWeight = DEFAULT_DIM_WEIGHT;

	m_sDimZ = DEFAULT_DIM_Z;
	m_sDimX = DEFAULT_DIM_X;
	m_sDimY = DEFAULT_DIM_Y;

	m_sDetailsType		= GetComponentDescStr( DEFAULT_DETAILS_TYPE );
	m_sDetailsMaterial	= GetComponentMaterialStr( DEFAULT_DETAILS_MATERIAL );
	m_sDimLengthCommon	= DEFAULT_DIM_LENGTH_COMMON;

	UpdateData( false );

	m_eCE = CE_FULLY_EDITABLE;

	SetEditability();

	m_editDetailsPartNumber.SetSel( 0, m_sDetailsPartNumber.GetLength() );
	m_editDetailsPartNumber.SetFocus();
}

void KwikScafComponentEditor::FillDialog()
{
	assert( m_pCurrentStockElement!=NULL );

	m_sACF2D	= m_pCurrentStockElement->GetFilename2D();
	if( m_sACF2D.IsEmpty() )
	{
		m_sACF2D = ( m_eCE==CE_FULLY_EDITABLE )? DEFAULT_ACF2D: FILENAME_NOT_AVAIL;
	}

	m_sACF2Deo	= m_pCurrentStockElement->GetFilename2DEO();
	if( m_sACF2Deo.IsEmpty() )
	{
		m_sACF2Deo = ( m_eCE==CE_FULLY_EDITABLE )? DEFAULT_ACF2DEO: FILENAME_NOT_AVAIL;
	}

	m_sACF3D	= m_pCurrentStockElement->GetFilename3D();
	if( m_sACF3D.IsEmpty() )
	{
		m_sACF3D = ( m_eCE==CE_FULLY_EDITABLE )? DEFAULT_ACF3D: FILENAME_NOT_AVAIL;
	}

	m_sDetailsDescription	= m_pCurrentStockElement->GetDescription();
	m_sDetailsPartNumber	= m_pCurrentStockElement->GetPartNumber();

	m_sOtherHire.Format( _T("$%0.2f"), m_pCurrentStockElement->GetHire() );
	m_sOtherSale.Format( _T("$%0.2f"), m_pCurrentStockElement->GetSale() );
	m_sDimWeight.Format( _T("%0.1fkg"), m_pCurrentStockElement->GetWeight() );

	m_sDimZ.Format( _T("%1.0f"), m_pCurrentStockElement->GetHeight() );
	m_sDimX.Format( _T("%1.0f"), m_pCurrentStockElement->GetLength() );
	m_sDimY.Format( _T("%1.0f"), m_pCurrentStockElement->GetWidth() );

	m_bUseSalePrice = m_pCurrentStockElement->UseSalePrice();

	ComponentDetailsElement *pCDEle;
	pCDEle = gpController->GetCompDetails()->GetMatchingComponent( m_pCurrentStockElement->GetPartNumber() );

	if( pCDEle!=NULL )
	{
		m_sDetailsType		= GetComponentDescStr( pCDEle->m_eCompType );
		m_sDetailsMaterial	= GetComponentMaterialStr( pCDEle->m_eMaterialType );
		m_sDimLengthCommon.Format( _T("%1.0f"), pCDEle->m_dCommonLength );
	}
	else
	{
		//default to BOM extra
		m_sDetailsType		= GetComponentDescStr( DEFAULT_DETAILS_TYPE );
		if( m_pCurrentStockElement->GetHeight()>0.00 &&
			m_pCurrentStockElement->GetLength()>0.00 &&
			m_pCurrentStockElement->GetWidth()>0.00 )
		{
			//This is a 3D component, so it is a visual component
			m_sDetailsType	= GetComponentDescStr( CT_VISUAL_COMPONENT );
		}
		m_sDetailsMaterial	= GetComponentMaterialStr( MT_OTHER );
		m_sDimLengthCommon	= _T("n/a");
	}
	UpdateData( false );
	SetEditability();
}

StockListElement * KwikScafComponentEditor::GetCurrentElement()
{
	return m_pCurrentStockElement;
}

void KwikScafComponentEditor::SetStockListElement(StockListElement *pElement, bool bSetEditability, bool bShowWarning/*=true*/ )
{
	CString					sMsg;
	ComponentDetailsElement	*pCDEle;

	pCDEle = gpController->GetCompDetails()->GetMatchingComponent( pElement->GetPartNumber() );
	m_eCE = CE_NOT_EDITABLE;
	m_pCurrentStockElement = pElement;

	if( m_pCurrentStockElement->GetUserCreatedElement() )
	{
		m_eCE = CE_FULLY_EDITABLE;
		assert( pCDEle==NULL );
	}
	else if( pCDEle!=NULL )
	{
		if( pCDEle->m_eCompType==CT_CHAIN_LINK ||
			pCDEle->m_eCompType==CT_SHADE_CLOTH ||
			pCDEle->m_eCompType==CT_SOLEBOARD )
		{
			//This component is not editable
			if( bShowWarning )
			{
				sMsg = _T("You may only edit the SALE PRICE,\n");
				sMsg+= _T("of Chain Mesh, Shade Cloth and\n");
				sMsg+= _T("Soleboards!\n");
				sMsg+= _T("\nIf you are trying to adjust any\n");
				sMsg+= _T("other field, you should create your\n");
				sMsg+= _T("own, editable, component.");
				MessageBox( sMsg, _T("Limited Editablity Component") );
			}
			m_eCE = CE_SALE_PRICE_ONLY;
		}
		else
		{
			//This component is not editable
			if( bShowWarning )
			{
				sMsg = _T("You cannot edit the details of\n");
				sMsg+= _T("existing KwikScaf components!\n\n");
				sMsg+= _T("Try creating one of your own and\n");
				sMsg+= _T("using that instead!");
				MessageBox( sMsg, _T("Invalid Component Selection") );
			}
			m_eCE = CE_NOT_EDITABLE;
		}
	}
	else
	{
		assert( false );
		//This component is not editable
		if( bShowWarning )
		{
			sMsg = _T("You cannot edit the details of\n");
			sMsg+= _T("existing KwikScaf components!\n\n");
			sMsg+= _T("Try creating one of your own and\n");
			sMsg+= _T("using that instead!");
			MessageBox( sMsg, _T("Invalid Component Selection") );
		}
		m_eCE = CE_NOT_EDITABLE;
	}

	if( bSetEditability )
	{
		SetEditability();
	}
}

void KwikScafComponentEditor::OnDelete() 
{
	int		i;

	if( IsNewComponent() )
	{
		//We haven't created a component yet so just start another one
		OnNewComponent();
		return;
	}

	if( m_pCurrentStockElement->GetUserCreatedElement() )
	{
		//they are alowed to delete user components
		for( i=0; i<m_pStockListArray->GetSize(); i++ )
		{
			if( m_pStockListArray->GetAt(i)==m_pCurrentStockElement )
			{
				delete m_pCurrentStockElement;
				m_pStockListArray->RemoveAt(i);
				if( m_pStockListArray->GetNumberOfUserCreatedElements()>0 )
				{
					//Move to the last user created element.
					SetStockListElement( m_pStockListArray->GetUserCreatedElement(
								m_pStockListArray->GetNumberOfUserCreatedElements()-1 ), true );
				}
				else if( i==0 )
				{
					//They deleted the first element in the list, this should
					//	never happen since the first element is non-editable
					assert( false );
					if( m_pStockListArray->GetSize()>0 )
					{
						SetStockListElement(m_pStockListArray->GetAt(i), true);
					}
					else
					{
						OnNewComponent();
						return;
					}
				}
				else
				{
					//They have deleted a component from the list, not the
					//	first, so just move the current record to the previous
					//	component in the list
					SetStockListElement(m_pStockListArray->GetAt(i-1), true);
				}

				break;
			}
		}
		FillDialog();
	}
	else
	{
		//How the hell was the delete button activated?
		assert( false );
		CString sMsg;
		sMsg = _T("You may only delete user created components");
		MessageBox( sMsg, _T("Cannot Delete") );
	}
}

void KwikScafComponentEditor::SetEditability()
{
	ComponentDetailsElement *pCDEle;

	EnableApplyButton(false);
	m_editOtherSale.EnableWindow(FALSE);
	m_editOtherHire.EnableWindow(FALSE);
	m_editDimY.EnableWindow(FALSE);
	m_editDimWeight.EnableWindow(FALSE);
	m_editDimLenthCommon.EnableWindow(FALSE);
	m_editDimX.EnableWindow(FALSE);
	m_editDimZ.EnableWindow(FALSE);
	m_editDetailsType.EnableWindow(FALSE);
	m_editDetailsPartNumber.EnableWindow(FALSE);
	m_editDetailsMaterail.EnableWindow(FALSE);
	m_editDetailsDescription.EnableWindow(FALSE);
	m_btnDelete.EnableWindow(FALSE);
	m_btnUseSalePrice.ShowWindow(SW_HIDE);
	EnableACFs(false);

	switch( m_eCE )
	{
	case( CE_FULLY_EDITABLE ):
		m_editOtherHire.EnableWindow(TRUE);
		m_editDimY.EnableWindow(TRUE);
		m_editDimWeight.EnableWindow(TRUE);
		m_editDimLenthCommon.EnableWindow(TRUE);
		m_editDimX.EnableWindow(TRUE);
		m_editDimZ.EnableWindow(TRUE);
		m_editDetailsType.EnableWindow(TRUE);
		m_editDetailsPartNumber.EnableWindow(TRUE);
		m_editDetailsMaterail.EnableWindow(TRUE);
		m_editDetailsDescription.EnableWindow(TRUE);

		if( !IsNewComponent() || (m_pCurrentStockElement!=NULL &&
					m_pCurrentStockElement->GetUserCreatedElement() ) )
		{
			m_btnDelete.EnableWindow(TRUE);
		}

		EnableACFs(true);
		UpdateData( false );
		//fallthrough

	case( CE_SALE_PRICE_ONLY ):
		m_editOtherSale.EnableWindow(TRUE);
		if( m_pCurrentStockElement!=NULL )
		{
			pCDEle = gpController->GetCompDetails()->GetMatchingComponent( m_sDetailsPartNumber );

			if( pCDEle!=NULL )
			{
				if( pCDEle->m_eCompType==CT_SOLEBOARD &&
					m_pCurrentStockElement->GetHire()>ROUND_ERROR_SMALL &&
					m_pCurrentStockElement->GetSale()>ROUND_ERROR_SMALL )
				{
					m_btnUseSalePrice.ShowWindow(SW_SHOW);
				}
			}
		}
		break;

	default:
	case( CE_NOT_EDITABLE ):
		break;
	}
}

void KwikScafComponentEditor::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("KwikScafComponentEditor") );
}

void KwikScafComponentEditor::OnChange()
{
	UpdateData();
	assert( m_eCE!=CE_NOT_EDITABLE );
	assert( m_eCE!=CE_SALE_PRICE_ONLY );
	EnableApplyButton(true);
}

void KwikScafComponentEditor::OnChangeAcf2d() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeAcf2deo() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeAcf3d() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeDetailsDescription() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeDetailsPartNumber() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeDimHeight() 
{
	OnChange();
	EnableACFs(true);
	UpdateData( false );
}

void KwikScafComponentEditor::OnChangeDimLengthActual() 
{
	OnChange();
	EnableACFs(true);
	UpdateData( false );
}

void KwikScafComponentEditor::OnChangeDimWeight() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeDimWidth() 
{
	OnChange();
	EnableACFs(true);
	UpdateData( false );
}

void KwikScafComponentEditor::OnChangeOtherHire() 
{
	OnChange();
}

void KwikScafComponentEditor::OnChangeOtherSale() 
{
	UpdateData();
	assert( m_eCE!=CE_NOT_EDITABLE );
	EnableApplyButton(true);
}

void KwikScafComponentEditor::EnableApplyButton(bool bEnable)
{
	if( bEnable )
	{
		assert( m_eCE!=CE_NOT_EDITABLE );
	}
	m_btnApply.EnableWindow( ConvertboolToBOOL(bEnable) );
}


void KwikScafComponentEditor::EnableACFs(bool bEnable)
{
	if( _tstof(m_sDimX)<=0.00 ||
		_tstof(m_sDimY)<=0.00 ||
		_tstof(m_sDimZ)<=0.00 )
	{
		bEnable = false;
	}

	m_btnACF3DBrowse.EnableWindow(ConvertboolToBOOL(bEnable));
	m_editACF_3D.EnableWindow(ConvertboolToBOOL(bEnable));
	m_btnACF2DeoBrowse.EnableWindow(ConvertboolToBOOL(bEnable));
	m_btnACF2DBrowse.EnableWindow(ConvertboolToBOOL(bEnable));
	m_editACF_2DEO.EnableWindow(ConvertboolToBOOL(bEnable));
	m_editACF_2D.EnableWindow(ConvertboolToBOOL(bEnable));
	if( bEnable )
	{
		m_sDetailsType	= GetComponentDescStr( CT_VISUAL_COMPONENT );
	}
}

bool KwikScafComponentEditor::ApplyChanges()
{
	bool	bSaleChanged;
	double	dHire, dSale, dWeight, dX, dY, dZ;
	CString sTemp1, sTemp2;

	UpdateData();

	bSaleChanged = false;
	if( IsNewComponent() )
	{
		bool bInvalid;

		bInvalid = false;
		CString sMsg;
    sMsg = _T("The following fields contain invalid data:\n\n");

		m_sDetailsPartNumber.TrimLeft();
		m_sDetailsPartNumber.TrimRight();
		if( m_sDetailsPartNumber==DEFAULT_DETAILS_PART_NUMBER )
		{
			sMsg+= _T("Part Number is invalid,\n");
			bInvalid = true;
		}
		else if( m_pStockListArray->GetMatchingComponent( m_sDetailsPartNumber )!=NULL )
		{
			sMsg+= _T("Part Number is already in use,\n");
			bInvalid = true;
		}

		if( m_sDetailsDescription==DEFAULT_DETAILS_DESCRIPTION )
		{
			sMsg+= _T("Description is invalid,\n");
			bInvalid = true;
		}

		sTemp1 = m_sOtherHire;
		sTemp2 = m_sOtherSale;
		if( sTemp1[0]==_T('$') )
			sTemp1 = sTemp1.Right( sTemp1.GetLength()-1 );
		if( sTemp2[0]==_T('$') )
			sTemp2 = sTemp2.Right( sTemp2.GetLength()-1 );
/*
		if( atof(sTemp1)<=0.00 && atof(sTemp2)<=0.00 )
		{
			sMsg+= "Hire & Sale cannot both be zero,\n";
			bInvalid = true;
		}
		else
*/		{
			m_sOtherHire = sTemp1;
			m_sOtherSale = sTemp2;
		}

		if( _tstof(m_sDimX)<-1.00*ROUND_ERROR )
		{
			sMsg+= _T("Length is invalid,\n");
			bInvalid = true;
		}
		if( _tstof(m_sDimY)<-1.00*ROUND_ERROR )
		{
			sMsg+= _T("Width is invalid,\n");
			bInvalid = true;
		}
		if( _tstof(m_sDimZ)<-1.00*ROUND_ERROR )
		{
			sMsg+= _T("Height is invalid,\n");
			bInvalid = true;
		}

		if( bInvalid )
		{
			sMsg+= _T("\nPlease correct these and try again.");
			MessageBox( sMsg, _T("Invalid Data") );
			return false;
		}

		m_pCurrentStockElement = new StockListElement();
		m_pCurrentStockElement->SetUserCreatedElement(true);
		m_pStockListArray->Add( m_pCurrentStockElement );
	}

	CString sFN;

	switch( m_eCE )
	{
	default:
	case( CE_NOT_EDITABLE ):
		assert( false );
		break;

	case( CE_FULLY_EDITABLE ):
		sFN = m_sACF2D;
		if( sFN==DEFAULT_ACF2D	)
			sFN.Empty();
		m_pCurrentStockElement->SetFilename2D(sFN);

		sFN = m_sACF2Deo;
		if( sFN==DEFAULT_ACF2DEO	)
			sFN.Empty();
		m_pCurrentStockElement->SetFilename2DEO(sFN);

		sFN = m_sACF3D;
		if( sFN==DEFAULT_ACF3D	)
			sFN.Empty();
		m_pCurrentStockElement->SetFilename3D(sFN);

		m_pCurrentStockElement->SetDescription(m_sDetailsDescription);
		m_pCurrentStockElement->SetPartNumber(m_sDetailsPartNumber);

		//Sale is handled below
		if( m_sOtherHire[0]==_T('$') )
		{
			m_sOtherHire = m_sOtherHire.Right( m_sOtherHire.GetLength()-1 );
		}
		dHire	= _tstof(m_sOtherHire);
		dWeight	= _tstof(m_sDimWeight);
		m_pCurrentStockElement->SetHire(dHire);
		m_pCurrentStockElement->SetWeight(dWeight);

		dX = _tstof(m_sDimX);
		dY = _tstof(m_sDimY);
		dZ = _tstof(m_sDimZ);
		m_pCurrentStockElement->SetHeight(dZ);
		m_pCurrentStockElement->SetLength(dX);
		m_pCurrentStockElement->SetWidth(dY);
		//fallthrough for sale price

	case( CE_SALE_PRICE_ONLY ):

		m_pCurrentStockElement->SetUseSalePrice(m_bUseSalePrice? true: false);
		if( m_sOtherSale[0]==_T('$') )
		{
			m_sOtherSale = m_sOtherSale.Right( m_sOtherSale.GetLength()-1 );
		}
		dSale	= _tstof(m_sOtherSale);
		bSaleChanged = m_pCurrentStockElement->GetSale()!=dSale;
		m_pCurrentStockElement->SetSale(dSale);

		if( m_eCE==CE_SALE_PRICE_ONLY && dSale>0.00 && ( bSaleChanged || m_bUseSalePrice || m_pCurrentStockElement->IsEditedSSC() ) )
		{
			m_pCurrentStockElement->SetSSCEdited(true);
		}
		else
		{
			m_pCurrentStockElement->SetSSCEdited(false);
		}

		break;
	}
	EnableApplyButton(false);
	SetStockListElement( m_pCurrentStockElement, true, false );
	FillDialog();

	SetNewComponent(false);

	return true;
}

bool KwikScafComponentEditor::AllowExit()
{
	if( IsNewComponent() || m_btnApply.IsWindowEnabled() )
	{
		int iResult;
		CString sMsg;
		sMsg = _T("Do you wish to apply your changes before existing?");
		iResult = MessageBox( sMsg, _T("Changes detected"), MB_YESNOCANCEL );
		if( iResult==IDYES )
		{
			if( !ApplyChanges() )
				return false;
		}
		else if( iResult==IDCANCEL )
		{
			return false;
		}
	}
	return true;
}


void KwikScafComponentEditor::SetNewComponent( bool bNewComponent )
{
	m_bNewComponent = bNewComponent;
}

bool KwikScafComponentEditor::IsNewComponent()
{
	return m_bNewComponent;
}

CString KwikScafComponentEditor::GetFilename()
{
	UINT					uiMode;
	CString					sFilename;

	uiMode = CFile::modeRead|CFile::typeBinary;

	sFilename.Empty();

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox(FALSE, EXTENSION_ACTUAL_DRAWING_FILE, sFilename,
			OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
			FILTER_ACTUAL_DRAWING_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_ACTUAL_DRAWING_FILE;
	//gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
	if(dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		//gbOpenFileDialogActive = false;
		sFilename.Empty();
	}
	//gbOpenFileDialogActive = false; 

	sFilename = dbox.GetPathName();
	return sFilename;
}

void KwikScafComponentEditor::OnOtherUseSalePrice() 
{
	UpdateData();
	assert( m_eCE!=CE_NOT_EDITABLE );
	EnableApplyButton(true);
}

void KwikScafComponentEditor::OnStorageExport() 
{
	if( m_eCE!=CE_FULLY_EDITABLE )
	{
		CString sMsg;

		sMsg = _T("You can only export user created components!");
		MessageBeep( MB_ICONSTOP );
		MessageBox( sMsg, _T("Export Error") );
		return;
	}
	
	if( m_pCurrentStockElement==NULL )
	{
		return;
	}

	CString	sFilename;

	sFilename = m_pCurrentStockElement->GetPartNumber();
	sFilename+= KWIKSCAF_STOCK_COMPONENT_EXTENSION;

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox( false, KWIKSCAF_STOCK_COMPONENT_EXTENSION, sFilename,
		OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_EXPLORER,
		FILTER_KWIKSCAF_STOCK_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_KWIKSCAF_STOCK_SAVE_FILE;

	if (dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	sFilename = dbox.GetPathName();

	CString sTemp, sExt;
	sTemp = sFilename.Right(1);
	while( sTemp==_T('.') )
	{
		sFilename = sFilename.Left( sFilename.GetLength()-1 );
		sTemp = sFilename.Right(1);
	}

	sExt = KWIKSCAF_STOCK_COMPONENT_EXTENSION;
	sExt.MakeUpper();
	sTemp = sFilename.Right(sExt.GetLength());
	sTemp.MakeUpper();
	if( sTemp!=sExt )
	{
		sFilename+= EXTENTION_SAVE_BOM_SUMMARY_FILE;
	}
	
	//open it this time
	CFileException	Error;
	CStdioFile		file;
	UINT			uiMode;

	uiMode = CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText;
	if( !file.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;

		sMessage+= szCause;
		MessageBeep(MB_ICONSTOP);
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return;
	}

	////////////////////////////////////////////////////
	//Write file header info
	CString sLine;

	sLine = KS_STOCK_COMP_FILE_PREFIX;
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%i"), KS_STOCK_COMP_PREFIX_VERSION, STOCK_EXPORT_VERSION_LATEST );
	file.WriteString( sLine+_T("\n") );
	
	sLine = _T("!");
	file.WriteString( sLine+_T("\n") );

	//write element info to file
	if( !m_pCurrentStockElement->Export( file ) )
	{
		file.Close();
		file.Remove( sFilename );
		return;
	}

	file.Close();

	//////////////////////////////////////////////////
	CString sMsg;
	sMsg.Format( _T("KwikScaf Stock file, %s, created successfully!"), sFilename );
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBox( sMsg, _T("Export Complete"), MB_OK );
}

void KwikScafComponentEditor::OnStorageImport() 
{
	CString		sFilename;

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox( true, KWIKSCAF_STOCK_COMPONENT_EXTENSION, sFilename,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER,
			FILTER_KWIKSCAF_STOCK_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_KWIKSCAF_STOCK_LOAD_FILE;

	if (dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	sFilename = dbox.GetPathName();

	//open it this time
	CFileException	Error;
	CStdioFile		file;
	UINT			uiMode;

	uiMode = CFile::modeRead|CFile::typeText;
	if( !file.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;

		sMessage+= szCause;
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return;
	}

	//////////////////////////////////////////////////
	int		iValue, iLength;
	CString sValue, sLine, sMsg, sTemp;
	bool	bSuccess = true;

	if( !file.ReadString( sLine ) )	bSuccess=false;
	if( bSuccess && sLine!=KS_STOCK_COMP_FILE_PREFIX )	bSuccess=false;

	if( bSuccess && !file.ReadString( sLine ) )	bSuccess=false;
	if( bSuccess )
		iLength = KS_STOCK_COMP_PREFIX_VERSION.GetLength();
	if( bSuccess && sLine.Left(iLength)!=KS_STOCK_COMP_PREFIX_VERSION )	bSuccess=false;
	sValue = sLine.Right( sLine.GetLength()-iLength );
	iValue = _ttoi(sValue);

	StockListElement *pElement;
	bSuccess = false;
	switch( iValue )
	{
	case(STOCK_EXPORT_VERSION_1_0_0):
		//write element info to file
		pElement = new StockListElement();
		bSuccess = pElement->Import( file );
		break;

	default:
		assert( false );
		if( iValue>STOCK_EXPORT_VERSION_LATEST )
		{
			sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
			sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
			sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
		}
		else
		{
			sMsg = _T("An unidentified error has occured during loading of this file.\n");
			sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
		}
		sMsg+= _T("Details of error -\n");
    sMsg+= _T("Class: KwikScafComponentEditor Import.\n");
    sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), STOCK_EXPORT_VERSION_LATEST, iValue );
		sMsg+= sTemp;
		MessageBox( sMsg, _T("Invalid File Version"), MB_OK );
		bSuccess = false;
	}

	file.Close();

	if( bSuccess )
	{
		OnNewComponent();
		m_pCurrentStockElement = new StockListElement();
		*m_pCurrentStockElement = *pElement;
		FillDialog();
		UpdateData();
		ApplyChanges();

		ShowInvalidFileWarning();

		//////////////////////////////////////////////////
		CString sMsg;
		sMsg.Format( _T("KwikScaf Stock file, %s, imported successfully!"), sFilename );
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox( sMsg, _T("Import Complete"), MB_OK );
	}
	else
	{
		//They should have already got an error message!
	}
	delete pElement;
	pElement = NULL;
}


/*
void KwikScafComponentEditor::OnStorageExport() 
{
	if( m_eCE!=CE_FULLY_EDITABLE )
	{
		CString sMsg;

		sMsg = "You can only export user created components!";
		MessageBeep( MB_ICONSTOP );
		MessageBox( sMsg, "Export Error" );
		return;
	}
	
	if( m_pCurrentStockElement==NULL )
	{
		return;
	}

	DWORD		 dwBufferLength;
	BYTE		*pBuffer;

	CMemFile	*pMemFile;
	CArchive	*pArchive;

	pMemFile	= new CMemFile( MEMFILE_GROW_INCREMENT );
	pArchive	= new CArchive( pMemFile, CArchive::store);

	// ************** Start Serialize here
	m_pCurrentStockElement->Serialize(*pArchive); 	// Serialize Controller into m_pArchive
	// ************** End Serialize here

	pArchive->Close();

	dwBufferLength	= pMemFile->GetLength();
	pBuffer			= pMemFile->Detach();    // this closes the pMemFile [CMemfile class]

	CString	sFilename;

	sFilename = m_pCurrentStockElement->GetPartNumber();
	sFilename+= KWIKSCAF_STOCK_COMPONENT_EXTENSION;

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox( false, KWIKSCAF_STOCK_COMPONENT_EXTENSION, sFilename,
		OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_EXPLORER,
		FILTER_KWIKSCAF_STOCK_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_KWIKSCAF_STOCK_SAVE_FILE;

	if (dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	sFilename = dbox.GetPathName();

	CString sTemp, sExt;
	sTemp = sFilename.Right(1);
	while( sTemp==_T('.') )
	{
		sFilename = sFilename.Left( sFilename.GetLength()-1 );
		sTemp = sFilename.Right(1);
	}

	sExt = KWIKSCAF_STOCK_COMPONENT_EXTENSION;
	sExt.MakeUpper();
	sTemp = sFilename.Right(sExt.GetLength());
	sTemp.MakeUpper();
	if( sTemp!=sExt )
	{
		sFilename+= EXTENTION_SAVE_BOM_SUMMARY_FILE;
	}
	
	//open it this time
	CFileException	Error;
	CFile			file;
	UINT			uiMode;

	uiMode = CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeBinary;
	if( !file.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;

		sMessage+= szCause;
		MessageBeep(MB_ICONSTOP);
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return;
	}

	file.Write( pBuffer, dwBufferLength );
	file.Close();

	DELETE_PTR(pArchive);
	DELETE_PTR(pMemFile);

	CString sMsg;

	sMsg.Format( _T("KwikScaf Stock file, %s,\ncreated successfully!"), sFilename );
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBox( sMsg, _T("Export Complete"), MB_OK );
}

void KwikScafComponentEditor::OnStorageImport() 
{
	CString		sFilename;
	CMemFile	*pMemFile;
	CArchive	*pArchive;

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox( true, KWIKSCAF_STOCK_COMPONENT_EXTENSION, sFilename,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER,
			FILTER_KWIKSCAF_STOCK_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_KWIKSCAF_STOCK_LOAD_FILE;

	if (dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	sFilename = dbox.GetPathName();

	//open it this time
	CFileException	Error;
	CFile			file;
	UINT			uiMode;

	uiMode = CFile::modeRead|CFile::typeBinary;
	if( !file.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;

		sMessage+= szCause;
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return;
	}

	DWORD	dwBufferLength=file.GetLength();
	BYTE	pBuffer[MEMFILE_GROW_INCREMENT];

	file.Read( pBuffer, dwBufferLength );

	pMemFile = new CMemFile( pBuffer, dwBufferLength );

	pMemFile->Attach( pBuffer, (UINT)dwBufferLength, MEMFILE_GROW_INCREMENT );

	if( pMemFile!=NULL )
	{
		pArchive = new CArchive( pMemFile, CArchive::load, 100000);

		// *********** Start Serialize here
		try
		{
			OnNewComponent();
			m_pCurrentStockElement->Serialize(*pArchive);		    // Serialize Controller out m_pArchive
			FillDialog();
		}
		catch(...)
		{
			CString sMsg;
			sMsg = _T("There was an error reading the file.\nImport operation has been stopped!");
			MessageBox( sMsg, _T("Error Reading file"), MB_OK|MB_ICONSTOP );
			pArchive->Abort();
			return;
		}

		pArchive->Close();
		pMemFile->Close();

		DELETE_PTR(pArchive);
		DELETE_PTR(pMemFile);
	}
}
*/

bool KwikScafComponentEditor::DoesFileExist(CString sFilename)
{
	UINT			uiMode;
	CStdioFile		file;
	CFileException	Error;

	uiMode = CFile::modeRead|CFile::typeBinary;
	if( !file.Open( sFilename, uiMode, &Error ) )
	{
		if( Error.m_cause==CFileException::fileNotFound ||
			Error.m_cause==CFileException::badPath ||
			Error.m_cause==CFileException::invalidFile )
		{
			file.Close();
			return false;
		}
	}
	file.Close();

	return true;
}

void KwikScafComponentEditor::ShowInvalidFileWarning()
{

	if( (!m_pCurrentStockElement->GetFilename3D().IsEmpty() &&
		  m_pCurrentStockElement->GetFilename3D()!=DEFAULT_ACF3D &&
		  !DoesFileExist( m_pCurrentStockElement->GetFilename3D() ) ) ||
		(!m_pCurrentStockElement->GetFilename2D().IsEmpty() &&
		  m_pCurrentStockElement->GetFilename2D()!=DEFAULT_ACF2D &&
		  !DoesFileExist( m_pCurrentStockElement->GetFilename2D() ) ) ||
		(!m_pCurrentStockElement->GetFilename2DEO().IsEmpty() &&
		  m_pCurrentStockElement->GetFilename2DEO()!=DEFAULT_ACF2DEO &&
		  !DoesFileExist( m_pCurrentStockElement->GetFilename2DEO() ) ) )
	{
		CString sMsg, sFilename;
    sMsg+= _T("Warning: The following files do not exist!:\n\n");
		if( !DoesFileExist( m_pCurrentStockElement->GetFilename3D() ) )
		{
      sMsg+= _T("3D filename: ");
			sMsg+= m_pCurrentStockElement->GetFilename3D();
			sMsg+= _T("\n");
		}
		if( !DoesFileExist( m_pCurrentStockElement->GetFilename2D() ) )
		{
      sMsg+= _T("2D filename: ");
			sMsg+= m_pCurrentStockElement->GetFilename2D();
			sMsg+= _T("\n");
		}
		if( !DoesFileExist( m_pCurrentStockElement->GetFilename2DEO() ) )
		{
      sMsg+= _T("2D end on filename: ");
			sMsg+= m_pCurrentStockElement->GetFilename2DEO();
			sMsg+= _T("\n");
		}
		sMsg+= _T("\n");
		sMsg+= _T("Without these files the Actual components cannot be displayed.\n");
		sMsg+= _T("KwikScaf will prompt you for the file when using Actual\n");
		sMsg+= _T("component and then default to the wireframe model if still\n");
		sMsg+= _T("invalid");
		
		MessageBox( sMsg, _T("File Not Found Warning"), MB_OK );
	}
}
