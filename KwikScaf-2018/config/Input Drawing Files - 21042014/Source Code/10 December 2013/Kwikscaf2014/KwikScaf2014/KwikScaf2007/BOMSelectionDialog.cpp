// BOMSelectionDialog.cpp : implementation file
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
#include "BOMSelectionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SummaryInfo.h"
#include "BOMSummaryDialog.h"

/////////////////////////////////////////////////////////////////////////////
// BOMSelectionDialog dialog



BOMSelectionDialog::BOMSelectionDialog(CWnd* pParent /*=NULL*/, SummaryInfo *pSummaryInfo /*=NULL*/)
	: KwikscafDialog(BOMSelectionDialog::IDD, pParent)
{
	m_pSummaryInfo = pSummaryInfo;
	//{{AFX_DATA_INIT(BOMSelectionDialog)
	m_bAmalgamate = FALSE;
	//}}AFX_DATA_INIT
}


void BOMSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BOMSelectionDialog)
	DDX_Control(pDX, IDC_AMALGAMATE, m_chkAmalgamate);
	DDX_Control(pDX, IDC_BOMS_SUMMARY_BUTTON, m_SummaryButtonCtrl);
	DDX_Control(pDX, IDC_BOMSL_REMOVE_BUTTON, m_RemoveButtonCtrl);
	DDX_Control(pDX, IDC_BOMSL_REMOVE_ALL_BUTTON, m_RemoveAllButtonCtrl);
	DDX_Control(pDX, IDC_BOMSL_ADD_BUTTON, m_AddButtonCtrl);
	DDX_Control(pDX, IDC_BOMSL_ADD_ALL_BUTTON, m_AddAllButtonCtrl);
	DDX_Control(pDX, IDC_BOM_SELECTED_LISTBOX_CTRL, m_BOMSSelectedListCtrl);
	DDX_Control(pDX, IDC_BOM_SELECTION_LISTBOX_CTRL, m_BOMSSelectionListCtrl);
	DDX_Check(pDX, IDC_AMALGAMATE, m_bAmalgamate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BOMSelectionDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(BOMSelectionDialog)
	ON_BN_CLICKED(IDC_BOMSL_ADD_BUTTON, OnBomslAddButton)
	ON_BN_CLICKED(IDC_BOMSL_ADD_ALL_BUTTON, OnBomslAddAllButton)
	ON_BN_CLICKED(IDC_BOMSL_REMOVE_ALL_BUTTON, OnBomslRemoveAllButton)
	ON_BN_CLICKED(IDC_BOMSL_REMOVE_BUTTON, OnBomslRemoveButton)
	ON_LBN_SELCHANGE(IDC_BOM_SELECTION_LISTBOX_CTRL, OnSelchangeBomSelectionListboxCtrl)
	ON_LBN_SELCHANGE(IDC_BOM_SELECTED_LISTBOX_CTRL, OnSelchangeBomSelectedListboxCtrl)
	ON_BN_CLICKED(IDC_BOMS_SUMMARY_BUTTON, OnBomsSummaryButton)
	ON_WM_MOVE()
	ON_LBN_DBLCLK(IDC_BOM_SELECTION_LISTBOX_CTRL, OnDblclkBomSelectionListboxCtrl)
	ON_LBN_DBLCLK(IDC_BOM_SELECTED_LISTBOX_CTRL, OnDblclkBomSelectedListboxCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BOMSelectionDialog message handlers

/////////////////////////////////////////////////////////////////////////////
// OnBomslAddButton
//
void BOMSelectionDialog::OnBomslAddButton() 
{
	int iListIndex;
	int iIndex;
	CString sSLListEntry;

	if (m_BOMSSelectionListCtrl.GetCurSel() != LB_ERR) // if selected
	{
		m_BOMSSelectionListCtrl.GetText(m_BOMSSelectionListCtrl.GetCurSel(), sSLListEntry);
		iIndex = m_BOMSSelectionListCtrl.GetItemData(m_BOMSSelectionListCtrl.GetCurSel());
		iListIndex = m_BOMSSelectedListCtrl.AddString(sSLListEntry);		
		m_BOMSSelectedListCtrl.SetItemData(iListIndex, iIndex);	
		m_BOMSSelectionListCtrl.DeleteString(m_BOMSSelectionListCtrl.GetCurSel());
	}
	UpdateVisibleButtons();	
}


/////////////////////////////////////////////////////////////////////////////
// OnBomslAddAllButton
//
void BOMSelectionDialog::OnBomslAddAllButton() 
{
	int iListIndex;
	int iIndex;
	int iSelectionListIndex;
	CString sSLListEntry;

	for (iSelectionListIndex = 0; iSelectionListIndex < m_BOMSSelectionListCtrl.GetCount(); iSelectionListIndex++)
	{
		m_BOMSSelectionListCtrl.GetText(iSelectionListIndex, sSLListEntry);
		iIndex = m_BOMSSelectionListCtrl.GetItemData(iSelectionListIndex);
		iListIndex = m_BOMSSelectedListCtrl.AddString(sSLListEntry);		
		m_BOMSSelectedListCtrl.SetItemData(iListIndex, iIndex);	
	}
	m_BOMSSelectionListCtrl.ResetContent();
	UpdateVisibleButtons();	
}


/////////////////////////////////////////////////////////////////////////////
// OnBomslRemoveAllButton
//
void BOMSelectionDialog::OnBomslRemoveAllButton() 
{
	int iListIndex;
	int iIndex;
	int iSelectedListIndex;
	CString sSLListEntry;

	for (iSelectedListIndex = 0; iSelectedListIndex < m_BOMSSelectedListCtrl.GetCount(); iSelectedListIndex++)
	{
		m_BOMSSelectedListCtrl.GetText(iSelectedListIndex, sSLListEntry);
		iIndex = m_BOMSSelectedListCtrl.GetItemData(iSelectedListIndex);
		iListIndex = m_BOMSSelectionListCtrl.AddString(sSLListEntry);		
		m_BOMSSelectionListCtrl.SetItemData(iListIndex, iIndex);	
	}
	m_BOMSSelectedListCtrl.ResetContent();
	UpdateVisibleButtons();	
}


/////////////////////////////////////////////////////////////////////////////
// OnBomslRemoveButton
//
void BOMSelectionDialog::OnBomslRemoveButton() 
{
	int iListIndex;
	int iIndex;
	CString sSLListEntry;

	if (m_BOMSSelectedListCtrl.GetCurSel() != LB_ERR) // if selected
	{
		m_BOMSSelectedListCtrl.GetText(m_BOMSSelectedListCtrl.GetCurSel(), sSLListEntry);
		iIndex = m_BOMSSelectedListCtrl.GetItemData(m_BOMSSelectedListCtrl.GetCurSel());
		iListIndex = m_BOMSSelectionListCtrl.AddString(sSLListEntry);		
		m_BOMSSelectionListCtrl.SetItemData(iListIndex, iIndex);	
		m_BOMSSelectedListCtrl.DeleteString(m_BOMSSelectedListCtrl.GetCurSel());
	}
	UpdateVisibleButtons();	
}


/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
//
BOOL BOMSelectionDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("BOMSelectionDialog"));
	
	LoadList();
	UpdateVisibleButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// LoadList
//
void BOMSelectionDialog::LoadList()
{
	int iIndex;
	int iListIndex;
	SummaryElement *pSummaryElement;
	CString sSLListItem;

	for( iIndex=0; iIndex < m_pSummaryInfo->GetSize(); ++iIndex )
	{
		pSummaryElement = m_pSummaryInfo->GetElement(iIndex);
		sSLListItem.Format(_T("S%s L%i"), pSummaryElement->GetStage(), pSummaryElement->GetLevel());
		if( pSummaryElement->IsCommitted() )
			sSLListItem+= _T(" (Committed)");
		iListIndex = m_BOMSSelectionListCtrl.AddString(sSLListItem);
		m_BOMSSelectionListCtrl.SetItemData(iListIndex, iIndex);
	}

	if( m_pSummaryInfo->GetSize()<=0 )
	{
		CString sMsg;
    sMsg = _T("Aborting: There are no components to create a Bill of Materails with!");
		MessageBox( sMsg, _T("No Components Error") );
		EndDialog( IDABORT );
	}


	if( m_pSummaryInfo->GetSize()==1 )
	{
		//There is not much point is showing this selection dialog
		//	since there is only one item to choose from!
		CString sSLListEntry;

		m_BOMSSelectionListCtrl.GetText( 0, sSLListEntry );
		iIndex = m_BOMSSelectionListCtrl.GetItemData(0);
		iListIndex = m_BOMSSelectedListCtrl.AddString(sSLListEntry);		
		m_BOMSSelectedListCtrl.SetItemData(iListIndex, iIndex);	
		m_BOMSSelectionListCtrl.DeleteString(m_BOMSSelectionListCtrl.GetCurSel());

		int					iRet;
		BOMSummaryDialog	*pBOMSummaryDialog;

		pBOMSummaryDialog = new BOMSummaryDialog(this, &m_BOMSSelectedListCtrl, m_pSummaryInfo, ConvertBOOLTobool(m_bAmalgamate) );
		iRet = pBOMSummaryDialog->DoModal();
		DELETE_PTR(pBOMSummaryDialog);

		//Pass this back to the controller, so we will retry this,
		//	time we will show the BOM Extra dialog first and add those
		//	changes to the Summary
		EndDialog( iRet );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UpdateVisibleButtons
//
void BOMSelectionDialog::UpdateVisibleButtons()
{
	if (m_BOMSSelectionListCtrl.GetCurSel() != LB_ERR)
		m_AddButtonCtrl.EnableWindow(TRUE);
	else
		m_AddButtonCtrl.EnableWindow(FALSE);

	
	if (m_BOMSSelectedListCtrl.GetCurSel() != LB_ERR)
		m_RemoveButtonCtrl.EnableWindow(TRUE);
	else
		m_RemoveButtonCtrl.EnableWindow(FALSE);

	if (m_BOMSSelectionListCtrl.GetCount() > 0)
		m_AddAllButtonCtrl.EnableWindow(TRUE);
	else
		m_AddAllButtonCtrl.EnableWindow(FALSE);

	if (m_BOMSSelectedListCtrl.GetCount() > 0)
	{
		m_RemoveAllButtonCtrl.EnableWindow(TRUE);
		m_SummaryButtonCtrl.EnableWindow(TRUE);
	}
	else
	{
		m_RemoveAllButtonCtrl.EnableWindow(FALSE);
		m_SummaryButtonCtrl.EnableWindow(FALSE);
	}

	if( m_BOMSSelectedListCtrl.GetCount()>1 )
	{
		m_chkAmalgamate.EnableWindow( true );
	}
	else
	{
		m_chkAmalgamate.EnableWindow( false );
	}
}


/////////////////////////////////////////////////////////////////////////////
// OnSelchangeBomSelectionListboxCtrl
//
void BOMSelectionDialog::OnSelchangeBomSelectionListboxCtrl() 
{
	m_BOMSSelectedListCtrl.SetCurSel(-1); // remove selection from other list
	UpdateVisibleButtons();	
}


/////////////////////////////////////////////////////////////////////////////
// OnSelchangeBomSelectedListboxCtrl
//
void BOMSelectionDialog::OnSelchangeBomSelectedListboxCtrl() 
{
	m_BOMSSelectionListCtrl.SetCurSel(-1); // remove selection from other list
	UpdateVisibleButtons();
}


/////////////////////////////////////////////////////////////////////////////
// OnBomsSummaryButton
//
void BOMSelectionDialog::OnBomsSummaryButton() 
{
	int					iRet;
	BOMSummaryDialog	*pBOMSummaryDialog;

	UpdateData();

	pBOMSummaryDialog = new BOMSummaryDialog(this, &m_BOMSSelectedListCtrl, m_pSummaryInfo, ConvertBOOLTobool(m_bAmalgamate) );
	iRet = pBOMSummaryDialog->DoModal();
	DELETE_PTR(pBOMSummaryDialog);

	//Pass this back to the controller, so we will retry this,
	//	time we will show the BOM Extra dialog first and add those
	//	changes to the Summary
	EndDialog( iRet );
}

void BOMSelectionDialog::OnCancel() 
{
	KwikscafDialog::OnCancel();
}

void BOMSelectionDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("BOMSelectionDialog") );
}

void BOMSelectionDialog::OnDblclkBomSelectionListboxCtrl() 
{
	//Do an Add> fuction
	UpdateData();
	OnBomslAddButton();
	UpdateData(false);
}

void BOMSelectionDialog::OnDblclkBomSelectedListboxCtrl() 
{
	//Do an Remove< fuction
	UpdateData();
	OnBomslRemoveButton();
	UpdateData(false);
}
