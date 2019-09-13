// ComponentPartNumberSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "Controller.h"
#include "ComponentPartNumberSelectorDlg.h"
#include "KwikscafProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Controller *gpController;

const CString MATCHING_COMPONENTS_TEXT = _T("Matching Components");

/////////////////////////////////////////////////////////////////////////////
// ComponentPartNumberSelectorDlg dialog


ComponentPartNumberSelectorDlg::ComponentPartNumberSelectorDlg(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(ComponentPartNumberSelectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ComponentPartNumberSelectorDlg)
	m_sFilterString = _T("");
	m_sMatchingElements = _T("");
	//}}AFX_DATA_INIT

	CString sTemp;
	sTemp.Empty();
	SetPartNumber( sTemp );
	SetAllowNonVisual(false);
}


void ComponentPartNumberSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ComponentPartNumberSelectorDlg)
	DDX_Control(pDX, IDC_FILTER_STRING, m_editFilterString);
	DDX_Control(pDX, IDC_COMPONENT_LISTBOX, m_ComponentListBoxCtrl);
	DDX_Control(pDX, IDC_FILTER, m_btnFilter);
	DDX_Text(pDX, IDC_FILTER_STRING, m_sFilterString);
	DDX_Text(pDX, IDC_MATCHING_ELEMENTS, m_sMatchingElements);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ComponentPartNumberSelectorDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(ComponentPartNumberSelectorDlg)
	ON_EN_SETFOCUS(IDC_FILTER_STRING, OnSetfocusFilterString)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_LBN_DBLCLK(IDC_COMPONENT_LISTBOX, OnDblclkComponentListbox)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ComponentPartNumberSelectorDlg message handlers

void ComponentPartNumberSelectorDlg::OnSetfocusFilterString() 
{
	SetDefID( IDC_FILTER );
}

void ComponentPartNumberSelectorDlg::OnFilter() 
{
	UpdateData();
	m_ComponentListBoxCtrl.ResetContent();
	FillComponentList(true);
	MessageBeep(MB_OK);
//	SetDefID( IDOK );
	UpdateData(false);
}

void ComponentPartNumberSelectorDlg::OnDblclkComponentListbox() 
{
	OnOK();
}

void ComponentPartNumberSelectorDlg::OnOK() 
{
	UpdateData();

	int iSelection;

	iSelection = m_ComponentListBoxCtrl.GetCurSel();
	if( iSelection>=0 )
	{
		SetPartNumber(m_saPartNumbers.GetAt(iSelection));
		KwikscafDialog::OnOK();
	}
	else
	{
		CString sMsg;
		if( m_saPartNumbers.GetSize()>0 )
		{
			sMsg = _T("Please select a component from the\n");
			sMsg+= _T("list before pressing the OK button!");
		}
		else
		{
			sMsg = _T("Your filter does not allow for any visual\n");
			sMsg+= _T("components.  Please refine your filter, and\n");
			sMsg+= _T("then choose the appropriate component from\n");
			sMsg+= _T("the list before pressing the OK button.\n\n");
		}
		MessageBeep( MB_ICONSTOP );
		MessageBox( sMsg, _T("Invalid Selection"), MB_ICONSTOP|MB_OK );
	}
}

void ComponentPartNumberSelectorDlg::OnCancel() 
{
	KwikscafDialog::OnCancel();
}

BOOL ComponentPartNumberSelectorDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("ComponentPartNumberSelectorDlg"));
	
	FillComponentList(false);

	UpdateData(false);

	//We need to select the preset part number as the current selection
	//	in the list, also we need to ensure that the selection is no
	//	more than the 5th element from the top of the list.
	CString sPartNumber;
	GetPartNumber(sPartNumber);
	if( !sPartNumber.IsEmpty() )
	{
		int iPos;
		iPos = m_ComponentListBoxCtrl.FindString( 0, sPartNumber );
		if( iPos>=0 )
		{
			m_ComponentListBoxCtrl.SetCurSel( iPos );
			if( iPos>5 )
				m_ComponentListBoxCtrl.SetTopIndex( iPos-5 );
			else
				m_ComponentListBoxCtrl.SetTopIndex( 0 );
		}
	}

	m_editFilterString.SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ComponentPartNumberSelectorDlg::FillComponentList(bool bApplyingFilter)
{
	int					i, iSize, iVisibleComponents;
	CString				sText, sPN, sDesc;
	StockListArray		*pStocks;
	StockListElement	*pElement;

	CKwikscafProgressDlg ProgDlg;
	if( bApplyingFilter )
		ProgDlg.SetText( _T("Filtering Stock List...") );
	else
		ProgDlg.SetText( _T("Loading Stock List...") );

	ProgDlg.Create(IDD_KWIKSCAF_PROGRESS);
	ProgDlg.ShowWindow( SW_SHOW );

	pStocks = gpController->GetStockList();

	m_saPartNumbers.RemoveAll();

	iSize = pStocks->GetSize();
	iVisibleComponents = 0;

	m_sFilterString.MakeUpper();
	for( i=0; i<iSize; i++ )
	{
		ProgDlg.SetPercent((int)((double)i/(double)iSize));

		pElement = pStocks->GetAt(i);
		if( IsAllowNonVisual() ||
			pElement->GetLength()>ROUND_ERROR &&
			pElement->GetHeight()>ROUND_ERROR &&
			pElement->GetWidth()>ROUND_ERROR )
		{
			//This component is visible
			iVisibleComponents++;
			sPN		= pElement->GetPartNumber();
			sPN.MakeUpper();
			sDesc	= pElement->GetDescription();
			sDesc.MakeUpper();

			//Does this visible component match the filter
			if( bApplyingFilter )
			{
				if( sPN.Find( m_sFilterString )<0 &&
					sDesc.Find( m_sFilterString )<0 ) 
				{
					//Nope does not match the filter
					continue;
				}
			}

			//This component has lenght, width, height, and
			//	matches the filter, we can use it!
			m_saPartNumbers.Add( sPN );
			sText.Format( _T("%-12s%-56s"), sPN, sDesc );
			m_ComponentListBoxCtrl.AddString( sText );
		}
	}

	m_sMatchingElements.Format( _T("%s = %i/%i"), MATCHING_COMPONENTS_TEXT,
						m_saPartNumbers.GetSize(), iVisibleComponents );

	if( m_saPartNumbers.GetSize()<=0 )
	{
		CString sMsg;
		sMsg+= _T("The component you are looking for is not in the\n");
		sMsg+= _T("list of ");
		if( IsAllowNonVisual() )
		{
			sMsg+= _T("stock");
		}
		else
		{
			sMsg+= _T("visible");
		}
    sMsg+= _T(" components, this means either:\n");
		sMsg+= _T("A typo; or\nThe component does not exist.\n\n");
    sMsg+= _T("TIP: You can create a new component using the\n");
		sMsg+= _T("KwikScaf Component Editor which is available\n");
		sMsg+= _T("throught the 'EditStock' command.");
		MessageBeep( MB_ICONWARNING );
		MessageBox( sMsg, _T("Invalid Selection"), MB_ICONWARNING|MB_OK );
	}

}

void ComponentPartNumberSelectorDlg::SetPartNumber(CString sPartNumber)
{
	m_sSelectedPartNumber = sPartNumber;
}

void ComponentPartNumberSelectorDlg::GetPartNumber(CString &sPartNumber)
{
	sPartNumber = m_sSelectedPartNumber;
}

void ComponentPartNumberSelectorDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("ComponentPartNumberSelectorDlg") );
}

void ComponentPartNumberSelectorDlg::SetAllowNonVisual(bool bAllow)
{
	m_bAllowBOMNonVisual = bAllow;
}

bool ComponentPartNumberSelectorDlg::IsAllowNonVisual()
{
	return m_bAllowBOMNonVisual;
}
