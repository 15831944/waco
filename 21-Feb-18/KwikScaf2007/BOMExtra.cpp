// BOMExtra.cpp : implementation file
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
#include "BOMExtra.h"
#include "meccano.h"
#include "componentDetails.h"
#include "componentList.h"
#include "Component.h"
#include "Main.h"
#include "Controller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Controller *gpController;

//const int	QUANTITY_LIMIT_BOTTOM	= 1;
const int	QUANTITY_LIMIT_BOTTOM	= -10000;
const int	QUANTITY_LIMIT_TOP		= 10000;

enum BOMExtraColumns
{
	COLUMN_DESC,
	COLUMN_QUANITY,
	COLUMN_STAGE,
	COLUMN_LEVEL,
};


//The following should always add to 100.00
const double	COLUMN_WIDTH_PERCENT_DESC		= 52.00;
const double	COLUMN_WIDTH_PERCENT_QUANITY	= 17.00;
const double	COLUMN_WIDTH_PERCENT_STAGE		= 18.00;
const double	COLUMN_WIDTH_PERCENT_LEVEL		= 13.00;

/////////////////////////////////////////////////////////////////////////////
// BOMExtraElement

BOMExtraElement::BOMExtraElement()
{
	m_dArea =0.00;
}

BOMExtraElement::BOMExtraElement(	ComponentTypeEnum	eComponentType,
									MaterialTypeEnum	eMaterialType,
									double				dLength,
									CString				sName,
									CString				sStage,
									int					iLevel,
									int					iQuantity,
									SystemEnum			eSystem )
{
	m_eComponentType	= eComponentType;
	m_eMaterialType		= eMaterialType;
	m_dLen			= dLength;
	m_sName				= sName;
	m_sStage			= sStage;
	m_iLevel			= iLevel;
	m_iQuantity			= iQuantity;
	m_eSystem			= eSystem;
}

BOMExtraElement &BOMExtraElement::operator=( const BOMExtraElement &Original)
{
	m_eSystem			= Original.m_eSystem;
	m_eComponentType	= Original.m_eComponentType;
	m_eMaterialType		= Original.m_eMaterialType;
	m_dLen			= Original.m_dLen;
	m_sName				= Original.m_sName;
	m_sStage			= Original.m_sStage;
	m_iLevel			= Original.m_iLevel;
	m_sPartNumber		= Original.m_sPartNumber;
	m_iQuantity			= Original.m_iQuantity;
	return *this;
}

BOMExtraArray &BOMExtraArray::operator=( const BOMExtraArray &Original)
{
	int				i;
	BOMExtraElement *pElement;

	//Delete any existing elements
	for( i=0; i<GetSize(); i++ )
	{
		pElement = Original.GetAt(i);
		delete pElement;
		RemoveAt(i);
	}
	RemoveAll();

	//Copy the original Array
	for( i=0; i<Original.GetSize(); i++ )
	{
		pElement = new BOMExtraElement();
		pElement = Original.GetAt(i);
		Add( pElement );
	}

	//check the sizes
	;//assert( GetSize()==Original.GetSize() );

	return *this;
}

bool BOMExtraElement::Compare(BOMExtraElement *pBOMExtraElement)
{
	bool match;
	match = false;
	
/*	if ( (pBOMExtraElement->m_dLength == m_dLength) &&
		 (pBOMExtraElement->m_eComponentType == m_eComponentType) &&
		 (pBOMExtraElement->m_eMaterialType == m_eMaterialType) 		)
*/
	if( pBOMExtraElement->m_sPartNumber == m_sPartNumber &&
		pBOMExtraElement->m_sStage == m_sStage &&
		pBOMExtraElement->m_iLevel == m_iLevel )
		match = true;

	return match;
}

CString BOMExtraElement::GetCompDetailsDescription()
{
	CString sOutString, sPartNo, sType, sDesc;

	//create the default, incase the part number is invalid
	;//assert( gpController!=NULL );
	gpController->GetStockList()->GetDescription( m_sPartNumber, sDesc );
	if( m_dLen>100.00 )
		sOutString.Format(_T("%s %s %.1fm"), sDesc, GetComponentMaterialStr( m_eMaterialType ), m_dLen*CONVERT_MM_TO_M );
	else
		sOutString.Format(_T("%s %s %.0f"), sDesc, GetComponentMaterialStr( m_eMaterialType ), m_dLen);

	//try to create the string again, using this part number
	;//assert( gpController!=NULL );
	sPartNo	= gpController->GetCompDetails()->GetComponentPartNumberStr( m_eSystem, m_eComponentType, m_dLen, m_eMaterialType );

	if( sPartNo==UNKNOWN_COMP_TYPE && !m_sPartNumber.IsEmpty() )
		sPartNo = m_sPartNumber;

	if( gpController->GetStockList()!=NULL )
	{
		if( !gpController->GetStockList()->GetDescription( sPartNo, sOutString ) )
		{
			;//assert( false );	//why can't we find the component
		}
	}
	else
	{
		;//assert( false );	//why can't we find the Stock details
	}

	return sOutString;
}

CString BOMExtraElement::GetBOMDescription()
{
	CString sOutString;

	sOutString.Format( _T("%-47s%-6d%-10s%-10i"), GetCompDetailsDescription(), m_iQuantity,
								m_sStage, (m_iLevel) );
	return sOutString;
}


CString BOMExtraElement::GetBOMPartNumberDescription()
{
	CString sOutString, sType;
	CString sDesc;

	sDesc.Format(_T("%s    %s"), m_sPartNumber, GetCompDetailsDescription() );
	sOutString.Format(_T("%-56s %d"), sDesc, m_iQuantity);

	return sOutString;
}

CString BOMExtraElement::GetCompDetailsPartNumberDescription()
{
	CString sOutString, sType;
	CString sDesc;

	sOutString.Format(_T("%-12s%-56s"), m_sPartNumber, GetCompDetailsDescription() );
	return sOutString;
}

CString BOMExtraElement::GetBOMPNumDescript()
{
	return m_sPartNumber;
}

CString BOMExtraElement::GetBOMDescript()
{
	return GetCompDetailsDescription();
}

CString BOMExtraElement::GetBOMQuantityDescript()
{
	CString sOutString;

	if( m_dArea>0.00 )
	{
//		sOutString.Format("%0.1fm2 x %d", m_dArea, m_iQuantity);
		if( ( gpController->GetPartNumberChainMesh()==m_sPartNumber && 
			  gpController->CalcPerRollChainMesh() ) ||
			( gpController->GetPartNumberShadeCloth()==m_sPartNumber && 
			  gpController->CalcPerRollShadeCloth() ) )
		{
			if( m_dArea>1.00-ROUND_ERROR_SMALL && 
				m_dArea<1.00+ROUND_ERROR_SMALL )
			{
				sOutString.Format(_T("%0.2froll"), m_dArea );
			}
			else
			{
				sOutString.Format(_T("%0.2frolls"), m_dArea );
			}
		}
		else
		{
			sOutString.Format(_T("%0.1fm2"), m_dArea );
		}
	}
	else
	{
		sOutString.Format(_T("%d"), m_iQuantity);
	}

	return sOutString;
}

/////////////////////////////////////////////////////////////////////////////
// BOMExtra dialog

BOMExtra::BOMExtra(CWnd* pParent, ComponentDetailsArray *pComponentDetailsList, ComponentList *pBOMExtraCompList)
	: KwikscafDialog(BOMExtra::IDD, pParent)
{
	m_bInitialised = false;
	m_pComponentDetailsList = pComponentDetailsList;
	m_pBOMExtraCompList = pBOMExtraCompList;

	m_bSortStageLevel = false;
	m_sQuantity = _T("1");

	//{{AFX_DATA_INIT(BOMExtra)
	m_sStage = _T("");
	m_sQuantity = _T("");
	m_iLevel = -1;
	m_sFilterString = _T("");
	m_sMatchingElements = _T("");
	//}}AFX_DATA_INIT
}


BOMExtra::~BOMExtra()
{
    // array clean up
	DestroyBOMExtraArray();
	DestroyBOMDetailsArray();
}

void BOMExtra::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BOMExtra)
	DDX_Control(pDX, IDC_FILTER, m_btnFilter);
	DDX_Control(pDX, IDC_EXTRA_COMPONENTS_LISTCONTROL, m_ExtraComponentsListControl);
	DDX_Control(pDX, IDC_LEVEL_COMBO, m_cmboLevel);
	DDX_Control(pDX, IDC_STAGE_COMBO, m_cmboStage);
	DDX_Control(pDX, IDC_QUANTITY_EDIT, m_ctrlQuantity);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_BOMRemoveButtonCtrl);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_BOMAddButtonCtrl);
	DDX_Control(pDX, IDC_COMPONENT_LISTBOX, m_ComponentListBoxCtrl);
	DDX_CBString(pDX, IDC_STAGE_COMBO, m_sStage);
	DDX_Text(pDX, IDC_QUANTITY_EDIT, m_sQuantity);
	DDX_CBIndex(pDX, IDC_LEVEL_COMBO, m_iLevel);
	DDX_Text(pDX, IDC_FILTER_STRING, m_sFilterString);
	DDX_Text(pDX, IDC_MATCHING_ELEMENTS, m_sMatchingElements);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// BOMExtra message handlers

BOOL BOMExtra::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("BOMExtra"));

	m_bInitialised = true;
	FillLevelList();
	FillStageList();
	m_sQuantity = _T("1");

	ConvertBOMCompListToDialogList();
//	AddBOMExtraList();
	AddComponentDetailsList(false);
	
	m_cmboStage.GetLBText( 0, m_sStage );
	m_iLevel = 0;

	UpdateData( false );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(BOMExtra, KwikscafDialog)
	//{{AFX_MSG_MAP(BOMExtra)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	ON_EN_CHANGE(IDC_QUANTITY_EDIT, OnChangeQuantityEdit)
	ON_CBN_EDITCHANGE(IDC_STAGE_COMBO, OnEditchangeStageCombo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_QUANTITY_SPIN, OnDeltaposQuantitySpin)
	ON_CBN_EDITUPDATE(IDC_STAGE_COMBO, OnEditupdateStageCombo)
	ON_CBN_KILLFOCUS(IDC_STAGE_COMBO, OnKillfocusStageCombo)
	ON_BN_CLICKED(IDC_REMOVEALL_DONE, OnRemoveallDone)
	ON_WM_MOVE()
	ON_LBN_DBLCLK(IDC_COMPONENT_LISTBOX, OnDblclkComponentListbox)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_NOTIFY(NM_DBLCLK, IDC_EXTRA_COMPONENTS_LISTCONTROL, OnDblclkExtraComponentsListcontrol)
	ON_EN_SETFOCUS(IDC_FILTER_STRING, OnSetfocusFilterString)
	ON_CBN_SELCHANGE(IDC_LEVEL_COMBO, OnSelchangeLevelCombo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &BOMExtra::OnBnClickedOk)
END_MESSAGE_MAP()


void BOMExtra::OnAddButton() 
{
	int iSelection;

	iSelection = m_ComponentListBoxCtrl.GetCurSel();
	if (iSelection != LB_ERR) // if a selection
	{
		int iBOMSelection;
		BOMExtraElement *pSelectedBOMElement;

		iBOMSelection = m_ComponentListBoxCtrl.GetItemData(iSelection);
		pSelectedBOMElement = m_BOMDetailsArray.ElementAt(iBOMSelection);
		pSelectedBOMElement->m_sStage		= GetStageAsString();
		pSelectedBOMElement->m_iLevel		= GetLevelAsInt();
		pSelectedBOMElement->m_iQuantity	= _ttoi( m_sQuantity );
		AddBOMSelection(pSelectedBOMElement);
	}
}

void BOMExtra::OnRemoveButton() 
{
	POSITION pos = m_ExtraComponentsListControl.GetFirstSelectedItemPosition();
	if( pos!=NULL )
	{
		int				nItem, iQnty;
		CString			sQnty;
		BOMExtraElement	*pSelectedBOMElement;
		while (pos)
		{
			nItem = m_ExtraComponentsListControl.GetNextSelectedItem(pos);
			pSelectedBOMElement = m_BOMExtraArray.ElementAt(nItem);

			iQnty = _ttoi( m_sQuantity );
			if( pSelectedBOMElement->m_iQuantity>iQnty )
			{
				pSelectedBOMElement->m_iQuantity-= iQnty;
				sQnty.Format( _T("%i"), pSelectedBOMElement->m_iQuantity );
				m_ExtraComponentsListControl.SetItemText( nItem, COLUMN_QUANITY, sQnty );
			}
			else
			{
				delete pSelectedBOMElement;
				m_ExtraComponentsListControl.DeleteItem( nItem );
				m_BOMExtraArray.RemoveAt(nItem);

				//Go through the rest of them and reset the item data
				for( ; nItem<m_ExtraComponentsListControl.GetItemCount(); nItem++ )
				{
					;//assert( (int)m_ExtraComponentsListControl.GetItemData(nItem)==nItem+1 );
					m_ExtraComponentsListControl.SetItemData( nItem, nItem );
				}
			}
	   }
	}
}

void BOMExtra::OnRemoveallDone() 
{
	if( m_ExtraComponentsListControl.GetItemCount()>3 )
	{
		int iResult;
		CString sMsg;
		sMsg = _T("This is your last chance to cancel this Remove All operation");
		sMsg+= _T("\nProceed with Remove All?");
		iResult = MessageBox( sMsg, _T("Warning"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2 );
		if( iResult==IDNO )
		return;
	}

	m_ExtraComponentsListControl.DeleteAllItems();
	DestroyBOMExtraArray();
}

void BOMExtra::OnChangeQuantityEdit() 
{
	int		i, iSize;
	TCHAR	cLetter;

	if( !m_bInitialised )
		return;

	UpdateData();

	iSize = m_sQuantity.GetLength();
	for( i=0; i<iSize; i++ )
	{
		cLetter = m_sQuantity[i];

//		if( !isdigit( cLetter ) && cLetter!='-' &&
//			cLetter!='.' )
		if( !_istdigit( cLetter ) && cLetter!=_T('.') )
		{
			//invalid character in the RL
			CString sMsg;
			sMsg = _T("Please enter a quantity number for this component");
			MessageBeep( MB_ICONEXCLAMATION );
			MessageBox( sMsg, _T("Invalid Data"), MB_OK );

			m_sQuantity.Format( _T("1") );
			m_ctrlQuantity.SetFocus();
			UpdateData( false );

			return;
		}
	}

	int iQnty	= _ttoi( m_sQuantity );
	if( iQnty<QUANTITY_LIMIT_BOTTOM || iQnty>QUANTITY_LIMIT_TOP )
	{
		//invalid character in the RL
		CString sMsg;
		sMsg.Format( _T("Please enter a number between - %i and %i."),
						QUANTITY_LIMIT_BOTTOM, QUANTITY_LIMIT_TOP );
		MessageBeep( MB_ICONEXCLAMATION );
		MessageBox( sMsg, _T("Invalid value"), MB_OK );

		m_sQuantity.Format( _T("1") );
		m_ctrlQuantity.SetFocus();
		UpdateData( false );
		
		return;
	}

}

#include "KwikscafProgressDlg.h"

void BOMExtra::AddComponentDetailsList(bool bApplyingFilter)
{
	int					index;
	int					iStringIndex;
	int					iDetailsIndex;
	int					numberOfComponents;
	int					iNumberInList;
	bool				bStringEmpty;
	CString				sPartNumber, sFilterString, sPN, sDesc;
	StockListArray		*pStocks;
	StockListElement	*pElement;
	BOMExtraElement		*pNewCompDetailElement;

	sFilterString	= m_sFilterString;
	;//assert( gpController!=NULL );
	pStocks			= gpController->GetStockList();
	bStringEmpty	= ConvertBOOLTobool( sFilterString.IsEmpty() );
	sFilterString.MakeUpper();
	iNumberInList = 0;
	if( pStocks!=NULL )
	{
		index = 0;
		numberOfComponents = pStocks->GetSize();

		CKwikscafProgressDlg ProgDlg;
		if( bApplyingFilter )
			ProgDlg.SetText( _T("Filtering Stock List...") );
		else
			ProgDlg.SetText( _T("Loading Stock List...") );

		ProgDlg.Create(IDD_KWIKSCAF_PROGRESS);
		ProgDlg.ShowWindow( SW_SHOW );

		while ( index<numberOfComponents )
		{
			ProgDlg.SetPercent( (int)((double)index/(double)numberOfComponents*100.00) );
			pElement = pStocks->GetAt(index);
			sPartNumber = pElement->GetPartNumber();
			sPN = sPartNumber;
			sPN.MakeUpper();
			if( !IsInBOMList( sPartNumber ) )
			{
				pNewCompDetailElement = new BOMExtraElement( CT_BOM_EXTRA, MT_STEEL, 0, _T(""), _T(""), -1, 0, S_KWIKSTAGE );
				pNewCompDetailElement->m_sPartNumber = sPartNumber;

				sDesc = pNewCompDetailElement->GetBOMDescription();
				sDesc.MakeUpper();
				if( !bStringEmpty && (sPN.Find( sFilterString, 0 )<0 ) &&
					(sDesc.Find( sFilterString, 0 )<0 ) )
				{
					//Doesn't match filter string!
					delete pNewCompDetailElement;
					++index;
					continue;
				}
				iNumberInList++;

				iDetailsIndex = m_BOMDetailsArray.Add(pNewCompDetailElement); // stage and level and quantity
				iStringIndex = m_ComponentListBoxCtrl.AddString(pNewCompDetailElement->GetCompDetailsPartNumberDescription());
				m_ComponentListBoxCtrl.SetItemData(iStringIndex, iDetailsIndex);
			}
			++index;
		}
	}
	m_sMatchingElements.Format( _T("Matching Elements = %i"), iNumberInList );
}

void BOMExtra::AddBOMExtraList()
{
	int iIndex;
	int iNumberOfComponents;

	iIndex = 0;
	iNumberOfComponents = m_pBOMExtraCompList->GetSize();

	while (iIndex < iNumberOfComponents)
	{
//		AddExtra(m_pBOMExtraCompList->GetAt(iIndex));
		iIndex++;
	}
}

bool BOMExtra::IsComponentDetailInList()
{
	return false;
}

void BOMExtra::AddBOMSelection( BOMExtraElement *pBOMExtraElement, bool bInitializeList /* default false */)
{
	// does the same component exist ?
	int iNumberOfItems;
	int iIndex;
	int iCurrentBOMItem;
	bool found;

	CString sQuantity;
	int iQuantity;

	iQuantity = _ttoi(m_sQuantity);

	if (bInitializeList)
		iQuantity = pBOMExtraElement->m_iQuantity;

	found = false;
	iIndex = 0;
	iNumberOfItems = m_ExtraComponentsListControl.GetItemCount();
	
	while (iIndex < iNumberOfItems)
	{
		iCurrentBOMItem = m_ExtraComponentsListControl.GetItemData(iIndex);
		found = m_BOMExtraArray.ElementAt(iCurrentBOMItem)->Compare(pBOMExtraElement);
		if (found)
			break;
		iIndex++;
	}
	if (found)
	{
		CString			sQnty;
		BOMExtraElement *pUpdateBOMElement; 

		pUpdateBOMElement = m_BOMExtraArray.ElementAt(iCurrentBOMItem);
		pUpdateBOMElement->m_iQuantity += iQuantity;
		// update string in list by deleting and reinserting
		sQnty.Format( _T("%i"), pUpdateBOMElement->m_iQuantity );
		m_ExtraComponentsListControl.SetItemText( iIndex, COLUMN_QUANITY, sQnty );
		m_ExtraComponentsListControl.SetItemData( iIndex, iCurrentBOMItem);
	}
	else // does it have to be added new ?
	{
		CString			sText;
		BOMExtraElement *pNewBOMElement; 

		pNewBOMElement = new BOMExtraElement(); // copy element to add
		*pNewBOMElement = *pBOMExtraElement;
		iCurrentBOMItem = m_BOMExtraArray.Add(pNewBOMElement);
		sText.Format( _T("%s"), pNewBOMElement->GetBOMDescript() );
		m_ExtraComponentsListControl.InsertItem( iIndex, sText );
		sText.Format( _T("%s"), pNewBOMElement->GetBOMQuantityDescript() );
		m_ExtraComponentsListControl.SetItemText( iIndex, COLUMN_QUANITY, sText );
		sText.Format( _T("%s"), pNewBOMElement->m_sStage );
		m_ExtraComponentsListControl.SetItemText( iIndex, COLUMN_STAGE, sText );
		sText.Format( _T("%i"), pNewBOMElement->m_iLevel );
		m_ExtraComponentsListControl.SetItemText( iIndex, COLUMN_LEVEL, sText );

		m_ExtraComponentsListControl.SetItemData(iIndex, iCurrentBOMItem);
	}
}


void BOMExtra::OnOK() 
{
	ConvertDialogListToBOMCompList();
	
	KwikscafDialog::OnOK();
}

void BOMExtra::ConvertBOMCompListToDialogList()
{
	int				iIndex, iNumberOfComponents, iLength;
	double			dFullLength;
	CString			sOldQnty;
	Component		*pComponent;
	BOMExtraElement	*pBOMComponent;

	iNumberOfComponents = m_pBOMExtraCompList->GetNumberOfComponents();
	iIndex = 0;

	dFullLength = 313.00;

	;//assert( COLUMN_WIDTH_PERCENT_DESC+COLUMN_WIDTH_PERCENT_QUANITY \
			+COLUMN_WIDTH_PERCENT_STAGE+COLUMN_WIDTH_PERCENT_LEVEL==100.00 );

	iLength = int( dFullLength*(COLUMN_WIDTH_PERCENT_DESC	/ 100.00));
	m_ExtraComponentsListControl.InsertColumn( COLUMN_DESC,		_T("Description"),	LVCFMT_LEFT,	iLength, 1);
	iLength = int( dFullLength*(COLUMN_WIDTH_PERCENT_QUANITY	/ 100.00));
	m_ExtraComponentsListControl.InsertColumn( COLUMN_QUANITY,	_T("Quantity"),		LVCFMT_LEFT,	iLength, 1);
	iLength = int( dFullLength*(COLUMN_WIDTH_PERCENT_STAGE	/ 100.00));
	m_ExtraComponentsListControl.InsertColumn( COLUMN_STAGE,	_T("Stage"),		LVCFMT_LEFT,	iLength, 1);
	iLength = int( dFullLength*(COLUMN_WIDTH_PERCENT_LEVEL	/ 100.00));
	m_ExtraComponentsListControl.InsertColumn( COLUMN_LEVEL,	_T("Level"),		LVCFMT_LEFT,	iLength, 1);

	sOldQnty = m_sQuantity;
	m_sQuantity = _T("1");
	while (iIndex < iNumberOfComponents)
	{
		pComponent = m_pBOMExtraCompList->GetComponent( iIndex );
		pBOMComponent = new BOMExtraElement();
		pBOMComponent->m_dLen		= pComponent->GetLengthCommon();
		pBOMComponent->m_eSystem		= pComponent->GetSystem();
		pBOMComponent->m_eComponentType = pComponent->GetType();
		pBOMComponent->m_eMaterialType	= pComponent->GetMaterialType();
		pBOMComponent->m_iQuantity		= 1; // this not necessary as the true arguement in the next statement
		if( pComponent->GetRemoveComponentFromBOM() )
			pBOMComponent->m_iQuantity = -1;
		pBOMComponent->m_sStage			= pComponent->GetStage();
		pBOMComponent->m_iLevel			= pComponent->GetLevel();
		;//assert( gpController!=NULL );
		pBOMComponent->m_sPartNumber	= gpController->GetCompDetails()->GetComponentPartNumberStr( 
												pBOMComponent->m_eSystem, 
												pBOMComponent->m_eComponentType, 
												pBOMComponent->m_dLen, 
												pBOMComponent->m_eMaterialType);
		if( pBOMComponent->m_sPartNumber==UNKNOWN_COMP_TYPE &&
			pComponent->GetType()==CT_BOM_EXTRA &&
			!pComponent->GetText().IsEmpty() )
		{
			pBOMComponent->m_sPartNumber = pComponent->GetText();
		}

		AddBOMSelection( pBOMComponent, true );
		iIndex++;

		//we can now deallocate this since it is not stored anywhere
		delete pBOMComponent;
		pBOMComponent = NULL;
	};
	m_sQuantity = sOldQnty;
}

void BOMExtra::ConvertDialogListToBOMCompList()
{
	m_pBOMExtraCompList->DeleteAll(); // Destroy all the components in the list

	int iNumberOfItems;
	int iIndex;
	int iCount;
	int iNumberOfComponents;

	BOMExtraElement *pBOMExtraComp;
	Component		*pComponent;

	iIndex = 0;
	iNumberOfItems = m_BOMExtraArray.GetSize();
	
	while (iIndex < iNumberOfItems)
	{
		pBOMExtraComp = m_BOMExtraArray.ElementAt(iIndex);
		iNumberOfComponents = pBOMExtraComp->m_iQuantity;
		iCount = 0;
		while ( iCount < abs(iNumberOfComponents) )
		{
			pComponent = new Component();
			pComponent->SetSystem( pBOMExtraComp->m_eSystem );
			// add details to component
			;//assert( gpController!=NULL );
			pComponent->SetController(gpController);
			pComponent->SetComponent(pBOMExtraComp->m_dLen, pBOMExtraComp->m_eComponentType, pBOMExtraComp->m_eMaterialType);
			pComponent->SetID( iIndex );
			pComponent->SetStage(pBOMExtraComp->m_sStage);
			pComponent->SetLevel(pBOMExtraComp->m_iLevel);
			pComponent->SetText(pBOMExtraComp->m_sPartNumber);
			pComponent->SetStockDetailsPointer();
			if( pComponent->GetStockDetails()==NULL )
			{
				pComponent->SetStockDetailsPointerDumb( gpController->GetStockList()->GetMatchingComponent(pBOMExtraComp->m_sPartNumber) );
			}
			if( iNumberOfComponents<0 )
				pComponent->SetRemoveComponentFromBOM( true );
			m_pBOMExtraCompList->AddComponent((SideOfBayEnum)0,0, pComponent);
			iCount++;
		}

		iIndex++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// ExtraComponentList Events

void BOMExtra::OnDeltaposQuantitySpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	UpdateData();

	int iQnty;
	iQnty	= _ttoi( m_sQuantity );
	iQnty	+= -1*pNMUpDown->iDelta;
	iQnty	= min( iQnty, QUANTITY_LIMIT_TOP );
	iQnty	= max( iQnty, QUANTITY_LIMIT_BOTTOM );
	m_sQuantity.Format( _T("%i"), iQnty );

	UpdateData( false );
	
	*pResult = 0;
}

void BOMExtra::OnEditchangeStageCombo() 
{

}

void BOMExtra::FillLevelList()
{
	int			i;
	double		dLevel;
	CString		sLevel;
	LevelList	*pLevels;

	m_cmboLevel.Clear();

	m_cmboLevel.AddString( _T("Level1") );

	;//assert( gpController!=NULL );
	pLevels = gpController->GetLevelList();

	for( i=0; i<pLevels->GetSize(); i++ )
	{
		dLevel = pLevels->GetLevel(i);
		sLevel.Format( _T("Level%i(%2.0fm)"), (i+2), dLevel );
		m_cmboLevel.AddString( sLevel );
	}
}

void BOMExtra::FillStageList()
{
	int				i;
	CString			sStage;

	m_cmboStage.Clear();

	;//assert( gpController!=NULL );
	m_cmboStage.AddString( STAGE_DEFAULT_VALUE);

	for( i=0; i<gpController->GetNumberOfStages(); i++ )
	{
		sStage = gpController->GetStage(i);
		m_cmboStage.AddString( sStage );
	}
}

void BOMExtra::OnEditupdateStageCombo() 
{
	// TODO: Add your control notification handler code here
	
}

void BOMExtra::OnKillfocusStageCombo() 
{
	int		i;
	bool	bFound;
	CString	sStage, sStageOld;

	UpdateData();

	bFound = false;
	;//assert( gpController!=NULL );
	for( i=0; i<gpController->GetNumberOfStages(); i++ )
	{
		sStage = gpController->GetStage(i);
		if( sStage==m_sStage )
		{
			bFound = true;
			break;
		}
	}

	if( !bFound && !m_sStage.IsEmpty() && m_sStage!=STAGE_DEFAULT_VALUE )
	{
		gpController->AddStage( m_sStage );
		m_cmboStage.AddString( m_sStage );
	}
}

int BOMExtra::GetLevelAsInt()
{
	if( m_iLevel<0 )
	{
		m_iLevel = 0;
		m_cmboLevel.SetCurSel(m_iLevel);
		UpdateData( false );
	}
	return m_iLevel+1;
}

CString BOMExtra::GetStageAsString()
{
	if( m_sStage.IsEmpty() )
	{
		m_cmboStage.SetCurSel(0);
		UpdateData( false );
	}

	return m_sStage;
}



void BOMExtra::DestroyBOMExtraArray()
{
	BOMExtraElement *pBOMExtraElement;

	// CLEAN up m_BOMExtraArray
	while( m_BOMExtraArray.GetSize()>0 )
	{
		pBOMExtraElement = NULL;
		pBOMExtraElement = m_BOMExtraArray.GetAt(0);
		if( pBOMExtraElement )
		{
			delete pBOMExtraElement;
		}
		else
		{
			;//assert( false );
		}
		m_BOMExtraArray.RemoveAt(0);
	}
}

void BOMExtra::DestroyBOMDetailsArray()
{
	BOMExtraElement *pBOMExtraElement;

	// CLEAN up m_BOMDetailsArray
	while( m_BOMDetailsArray.GetSize()>0 )
	{
		pBOMExtraElement = NULL;
		pBOMExtraElement = m_BOMDetailsArray.GetAt(0);
		if( pBOMExtraElement )
		{
			delete pBOMExtraElement;
		}
		else
		{
			;//assert( false );
		}
		m_BOMDetailsArray.RemoveAt(0);
	}
}

void BOMExtraArray::Sort(bool bIncremental /*=true*/)
{
	int				a, b;
	CString			sA, sB;
	BOMExtraElement	*pA, *pB;

	for( a=0; a<GetSize()-1; a++ )
	{
		pA = GetAt(a);
		sA = pA->m_sPartNumber;
		for( b=a+1; b<GetSize(); b++ )
		{
			pB = GetAt(b);
			sB = pB->m_sPartNumber;
			if( bIncremental )
			{
				if( sA>sB )
				{
					SetAt( a, pB );
					SetAt( b, pA );
					//we have changed the value at index a, so get it again!
					pA = pB;
					sA = sB;
				}
			}
			else
			{
				if( sA<sB )
				{
					SetAt( a, pB );
					SetAt( b, pA );
					//we have changed the value at index a, so get it again!
					pA = pB;
					sA = sB;
				}
			}
		}
	}
}

void BOMExtraArray::TracePartNos()
{
#ifdef _DEBUG
	int				a;
	CString			sA;
	BOMExtraElement	*pA;

  acutPrintf(_T("\nTracing Part Numbers:") );
	for( a=0; a<GetSize()-1; a++ )
	{
		pA = GetAt(a);
		sA = pA->m_sPartNumber;
		acutPrintf(_T("\n%s"), sA);
	}
	acutPrintf(_T("\nTrace Complete") );
#endif	//#ifdef _DEBUG
}

void BOMExtra::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("BOMExtra") );
}

void BOMExtra::OnDblclkComponentListbox() 
{
	//Do an Add>> function
	UpdateData();
	OnAddButton();
	UpdateData(false);
}

void BOMExtra::OnDblclkExtraComponentsListcontrol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//Do a <<Remove function
	UpdateData();
	OnRemoveButton();
	UpdateData(false);

	*pResult = 0;
}

bool BOMExtra::IsInBOMList(LPCTSTR strPartNumber)
{
	int				i;
	BOMExtraElement	*pEle;
	for( i=0; i<m_BOMDetailsArray.GetSize(); i++ )
	{
		pEle = m_BOMDetailsArray.GetAt(i);
		if( pEle->m_sPartNumber==strPartNumber )
			return true;
	}
	return false;
}

void BOMExtra::OnFilter() 
{
	UpdateData();
	m_ComponentListBoxCtrl.ResetContent();
	DestroyBOMDetailsArray();
	AddComponentDetailsList(true);
	MessageBeep(MB_OK);
	SetDefID( IDC_ADD_BUTTON );
	UpdateData(false);
}


void BOMExtra::OnSetfocusFilterString() 
{
	SetDefID( IDC_FILTER );
}

void BOMExtra::OnSelchangeLevelCombo() 
{
	UpdateData();
#ifdef	_DEBUG
  acutPrintf(_T("\nLevel change to: %i"), m_iLevel );
#endif	_DEBUG
}

void BOMExtraArray::RemoveAll()
{
	BOMExtraElement *pEle;

	while( GetSize()>0 )
	{
		pEle = NULL;
		pEle = GetAt(0);
		if( pEle!=NULL )
		{
			delete pEle;
		}
		else
		{
			;//assert( false );
		}
		RemoveAt( 0 );
	}
}


void BOMExtra::OnBnClickedOk()
{
	ConvertDialogListToBOMCompList();
	
	KwikscafDialog::OnOK();
}
