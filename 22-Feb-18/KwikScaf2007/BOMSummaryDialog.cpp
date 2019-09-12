// BOMSummaryDialog.cpp : implementation file
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
#include "BOMSummaryDialog.h"
#include "SummaryInfo.h"
#include "Controller.h"
#include "winspool.h"  // for printing
#include "PrintManager.h"
#include "ListCtrlPrint.h"
#include "math.h"
#include <afxdisp.h>

extern class Controller *gpController;  // external global to controller so I can get the comp details array

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum SummaryColumnsEnum
{
	COLUMN_PART,	//Part Number
	COLUMN_DESC,	//Description (Stock Description)
	COLUMN_QNTY,	//Quantity
	COLUMN_WGHT,	//Weight (Kg)
	COLUMN_HIRE,	//Hire ($)
	COLUMN_SALE 	//Sale ($)
};

const double COLUMN_RATIO_PART = 0.12;
const double COLUMN_RATIO_DESC = 0.38;
const double COLUMN_RATIO_QNTY = 0.12;
const double COLUMN_RATIO_WGHT = 0.13;
const double COLUMN_RATIO_HIRE = 0.13;
const double COLUMN_RATIO_SALE = 0.12;


/////////////////////////////////////////////////////////////////////////////
// BOMSummaryDialog dialog

/////////////////////////////////////////////////////////////////////////////
// Constructor
//

BOMSummaryDialog::BOMSummaryDialog(CWnd* pParent /*=NULL*/, CListBox *pSelectedListBox /* NULL */, SummaryInfo *pSummaryInfo /* NULL */, bool bAmalgamate/*=false*/)
	: KwikscafDialog(BOMSummaryDialog::IDD, pParent)
{
	m_pSelectedListBox			= pSelectedListBox;
	m_pSummaryInfo				= pSummaryInfo;
	;//assert( gpController!=NULL );
	m_pComponentDetailsArray	= gpController->GetCompDetails();
	m_pStockListArray			= gpController->GetStockList();
	m_bAmalgamate				= bAmalgamate;

	m_saCSVData.RemoveAll();
	//{{AFX_DATA_INIT(BOMSummaryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
 
///////////////////////////////////////////////////////////////////////////////
//// Default Destructor
//
BOMSummaryDialog::~BOMSummaryDialog()
{
 //   // array clean up
	//DELETE_PTR(m_pSummaryInfo);
	
}

/////////////////////////////////////////////////////////////////////////////
// DoDataExchange
//
void BOMSummaryDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BOMSummaryDialog)
	DDX_Control(pDX, IDC_COMMIT, m_btnCommit);
	DDX_Control(pDX, IDC_UNCOMMIT, m_btnUncommit);
	DDX_Control(pDX, IDC_BOM_EXTRA, m_BOMExtraButton);
	DDX_Control(pDX, IDC_STOCK_TEST_LISTBOX, m_StockTestListBoxCtrl);
	DDX_Control(pDX, IDC_BOM_SUMMARY_LISTCTRL, m_BOMSummaryNewListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BOMSummaryDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(BOMSummaryDialog)
	ON_BN_CLICKED(IDC_PRINT_BUTTON, OnPrintButton)
	ON_BN_CLICKED(IDC_BOM_EXTRA, OnBomExtra)
	ON_BN_CLICKED(IDC_COMMIT, OnCommit)
	ON_BN_CLICKED(IDOK, OnClose)
	ON_BN_CLICKED(IDC_UNCOMMIT, OnUncommit)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SAVE_REPORT, OnSaveReport)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BOM_SUMMARY_LISTCTRL, &BOMSummaryDialog::OnLvnItemchangedBomSummaryListctrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BOMSummaryDialog message handlers

/////////////////////////////////////////////////////////////////////////////
// OnPrintButton
//
void BOMSummaryDialog::OnPrintButton() 
{
// All we need is to setup the header, footer and page format, and to provide a pointer
// to the CListCtrl control
	CListCtrlPrint cJob;
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime ) ;  // Get the current time from the 
                         // operating system.
	COleDateTime *pcdtDate;
	pcdtDate = new COleDateTime(osBinaryTime);

	cJob.csPageHead = _T("Bill Of Materials"); //"This is the header of the file\r\nMultiline!\r\n";

	//QA number
	CString sFooter = _T("FC3.17/6-00\n");

	//find the Database filename
	CString sTemp;
	gpController->GetDatabaseFilename( sTemp );
	if( !sTemp.IsEmpty() )
	{
    sFooter+= _T("DWG filename: ");
		sFooter+= sTemp;
	}

	//Kwikscaf version
  sFooter+= _T("\nKwikscaf Version: ");
	sFooter+= MECCANO_LATEST_VERSION;

	//Date
  sFooter+= _T("\nPrinted: ");
	sFooter+= pcdtDate->Format(VAR_DATEVALUEONLY); // "date here!"; "Just a test for the footers\r\neventually multiline too ..";

	cJob.csPageFooter = sFooter;
	cJob.csPage = _T("Page %d of %d");
	cJob.pList = &m_BOMSummaryNewListCtrl;
	cJob.OnFilePrint();
	DELETE_PTR(pcdtDate);
	KwikscafDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
//
BOOL BOMSummaryDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("BOMSummaryDialog"));

	LoadList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// LoadList
// 
void BOMSummaryDialog::LoadList()
{
	int iIndex, iSIIndex;
	intArray iaSIIndex;
	
	iCurrentListCtrlIndex = 0;

	int		iWidth;
	double	dRatioTotal, dSSWidth;

	dSSWidth = 780.00;
	;//assert( gpController!=NULL );
	if( gpController->IsDisplaySalePrice() )
	{
		dRatioTotal = COLUMN_RATIO_PART + COLUMN_RATIO_DESC
					+ COLUMN_RATIO_QNTY + COLUMN_RATIO_WGHT
					+ COLUMN_RATIO_HIRE + COLUMN_RATIO_SALE;

		iWidth = int((COLUMN_RATIO_PART/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_PART, _T("Part Number"),						LVCFMT_LEFT,	iWidth,	 0);
		iWidth = int((COLUMN_RATIO_DESC/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_DESC, _T("Description (Stock Description)"),	LVCFMT_LEFT,	iWidth, 1);
		iWidth = int((COLUMN_RATIO_QNTY/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_QNTY, _T("Quantity"),							LVCFMT_CENTER,	iWidth,  2);
		iWidth = int((COLUMN_RATIO_WGHT/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_WGHT, _T("Weight (Kg)"),						LVCFMT_RIGHT,	iWidth, 3);
		iWidth = int((COLUMN_RATIO_HIRE/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_HIRE, _T("Hire ($/week)*"),						LVCFMT_RIGHT,	iWidth, 4);
		iWidth = int((COLUMN_RATIO_SALE/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_SALE, _T("Sale ($)"),							LVCFMT_RIGHT,	iWidth,	 5);
	}
	else
	{
		dRatioTotal = COLUMN_RATIO_PART + COLUMN_RATIO_DESC
					+ COLUMN_RATIO_QNTY + COLUMN_RATIO_WGHT
					+ COLUMN_RATIO_HIRE;

		iWidth = int((COLUMN_RATIO_PART/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_PART, _T("Part Number"),						LVCFMT_LEFT,	iWidth, 0);
		iWidth = int((COLUMN_RATIO_DESC/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_DESC, _T("Description (Stock Description)"),	LVCFMT_LEFT,	iWidth, 1);
		iWidth = int((COLUMN_RATIO_QNTY/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_QNTY, _T("Quantity"),							LVCFMT_CENTER,	iWidth, 2);
		iWidth = int((COLUMN_RATIO_WGHT/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_WGHT, _T("Weight (Kg)"),						LVCFMT_RIGHT,	iWidth, 3);
		iWidth = int((COLUMN_RATIO_HIRE/dRatioTotal)*dSSWidth);
		m_BOMSummaryNewListCtrl.InsertColumn(COLUMN_HIRE, _T("Price ($)*"),							LVCFMT_RIGHT,	iWidth, 4);
	}

	DisplayJobDetails( iCurrentListCtrlIndex );

	m_CompsChanged.RemoveAll();
	CheckStandardsAndTransoms();

	m_daQuantityTotals.RemoveAll();
	m_daWeightTotals.RemoveAll();
	m_daHireTotals.RemoveAll();
	m_daSaleTotals.RemoveAll();
	m_daSaleQuantityTotals.RemoveAll();
	m_daSaleWeightTotals.RemoveAll();

	m_bShowCaret = false;

	int iCommittedCount = 0;
	for (iIndex = 0; iIndex < m_pSelectedListBox->GetCount(); ++iIndex)
	{
		if( !m_bAmalgamate && iIndex>0 ) // insert a blank line
		{
			m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" "));
			++iCurrentListCtrlIndex;
		}
		iSIIndex = m_pSelectedListBox->GetItemData(iIndex);
		if( !m_bAmalgamate )
		{
			iaSIIndex.RemoveAll();
		}
		iaSIIndex.Add(iSIIndex);

		//we are not amalgamating so do the List now!
		if( !m_bAmalgamate )
		{
			if( m_pSelectedListBox->GetCount()>1 && iIndex==m_pSelectedListBox->GetCount()-1 )
				MakeListOfComponents(iaSIIndex, true);
			else
				MakeListOfComponents(iaSIIndex, false);
		}

		if( m_pSummaryInfo->GetElement( iSIIndex )->IsCommitted() )
			iCommittedCount++;
	}
	if( m_bAmalgamate )
		MakeListOfComponents(iaSIIndex, false);


	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
	iCurrentListCtrlIndex++;
	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
	iCurrentListCtrlIndex++;
	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
	m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, _T("* Prices shown are LIST PRICE ONLY and") );
	iCurrentListCtrlIndex++;
	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
	m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, _T("  do not include GST nor Stamp Duty!") );
	if( m_bShowCaret )
	{
		iCurrentListCtrlIndex++;
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, _T("^ This is a sale price and is not") );
		iCurrentListCtrlIndex++;
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, _T("  included in total hire cost") );
	}
	iCurrentListCtrlIndex++;

	m_btnCommit.ShowWindow( SW_SHOW );
	m_btnUncommit.ShowWindow( SW_SHOW );
	if( iCommittedCount==m_pSelectedListBox->GetCount() )
	{
		//everything is committed, so hide the committed button
		m_btnCommit.ShowWindow( SW_HIDE );
	}
	else if( iCommittedCount==0 )
	{
		//everything is uncommitted, so hide the uncommit button
		m_btnUncommit.ShowWindow( SW_HIDE );
	}
}


enum
{
	BAY_SIZE_0800,
	BAY_SIZE_1200,
	BAY_SIZE_1800,
	BAY_SIZE_2400,
	BAY_SIZE_SIZE
};

/////////////////////////////////////////////////////////////////////////////
// MakeListOfComponents
//
void BOMSummaryDialog::MakeListOfComponents(intArray iaSIIndex, bool bShowGrandTotal/*=false*/ )
{
	int					i, iIndex, iBOMIndex, iLvl, iQnty, iBayNumber,
						iSIIndex, iaBaySizes[BAY_SIZE_SIZE];
	Bay					*pBay;
	bool				bFound;
	Lift				*pLift;
	double				dCompLength, dTestLength, dArea, dTotalArea, dTotalLapArea, dAreaOfThisBay,
							dLower, dUpper, dBayLength,
							dBayLower, dBayUpper,
							dMaterialLower, dMaterialUpper;
	CString				sStageLevel, sPartNumber,
							sSTarget, sLTarget, sSAim, sLAim;
	intArray			iaBays, iaLaps;
	LevelList			*pLevels;
	Component			*pComponent;
	CornerOfBayEnum		eCnr1, eCnr2;
	doubleArray			daLengths;
	BOMExtraArray		BOMDetailsArray, BOMSaleArray;
	SummaryElement		*pSummaryElement;
	ComponentTypeEnum	eType;

	//M2 Calc parameters
	double	dBayTotalLength=0;
	double	dEndTotalLength=0;
	double	dLapTotalLength=0;
	int		deckQty=0;
	double	slabHeight=0;
	double	deckHeight=0;
	double endLength=0;
	double endQty=0;

	double deckFaceArea=0;
	double totalFaceArea=0;
	double toTopDeckArea=0;
	double toTopGuardRailArea=0;
	int lbCnt=0;

	int etypeArray[1000];
	for(int ietype=0;ietype<1000;ietype++)
		etypeArray[ietype]=0;

	BOMDetailsArray.RemoveAll();

	;//assert( gpController!=NULL );
	pLevels	= gpController->GetLevelList();

	iaBays.RemoveAll();
	iaLaps.RemoveAll();
	for( i=0; i<4; i++ )
	{
		iaBaySizes[i] = 0;
	}

	dTotalArea		= 0.00;
	dTotalLapArea	= 0.00;

	int iAmalga;
	for( iAmalga=0; iAmalga<iaSIIndex.GetSize(); iAmalga++ )
	{
		iSIIndex = iaSIIndex.GetAt( iAmalga );
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);
		
		iLvl = pSummaryElement->GetLevel();

		//this is a 1 based number!
		;//assert( iLvl>=1 );
		//now its a zero based number
		iLvl--;
		//pLevels is a zero based system!
		if(pLevels->GetSize()>0)
		{
			;//assert( iLvl<=pLevels->GetSize() );
		}
		else
		{
			;//assert( iLvl==0 );
		}
		for (iIndex = 0; iIndex < pSummaryElement->GetNumberOfComponents(); iIndex++)
		{   
			// make a BOM element
			pComponent = pSummaryElement->GetComponent(iIndex);

			//What bay does this component belong to
			pLift = pComponent->GetLiftPointer();
			if( pLift!=NULL )
			{
				pBay = pLift->GetBayPointer();
			}
			else
			{
				pBay = pComponent->GetBayPointer();
			}

			if( pBay==NULL )
			{
				//This must belong to the BOMExtra
				;//assert( pComponent->GetController()!=NULL );
			}


			if( pBay!=NULL )
			{
				/////////////////////////////////////////////////////////////////////
				//Calculate the m2 coverage for this Stage and Level, we will need a bay component for this
				iBayNumber = pBay->GetBayNumber();

				if( pBay->GetBayType()==BAY_TYPE_LAPBOARD && !iaLaps.IsElementOf( pBay->GetID() ) )
				{
					;//assert( pBay->GetID()>=0 );
					iaLaps.Add( pBay->GetID() );

					double dRLOfTopLift = pBay->GetRLOfLift( pBay->GetNumberOfLifts()-1 );
					double dRLOfBottomLift = pBay->GetRLOfLift( 0 );
					dAreaOfThisBay = ( dRLOfTopLift-dRLOfBottomLift ) * pBay->GetBayLengthActual();
					dTotalLapArea+= dAreaOfThisBay;
					dLapTotalLength += pBay->GetBayLength()*CONVERT_MM_TO_M-0.6;
				}

				//If this bay has not been processed yet, and it is not a lapboard
				if( pBay->GetBayType()>BAY_TYPE_UNDEFINED && pBay->GetBayType()!=BAY_TYPE_LAPBOARD &&
						!iaBays.IsElementOf( iBayNumber ) )
				{
					;//assert( iBayNumber>0 );
					iaBays.Add( iBayNumber );
					Bay *pBayB=pBay->GetBackward();
					Bay *pBayF=pBay->GetForward();

					int compID=pComponent->GetID();//16;
					//SideOfBayEnum sbe= pBay->GetPosition(pBay->GetID());
					SideOfBayEnum sbeB;
					SideOfBayEnum sbeF;

					//pComp = m_caComponents.GetComponent( iComp );
					if(pBayB!=NULL)
						 sbeB= pBayB->GetPosition(compID);
					if(pBayF!=NULL)
						 sbeF= pBayF->GetPosition(compID);

					

int bCnt=0;
for( int iLoop=0; iLoop<pBay->GetNumberOfBayComponents(); iLoop++ )
	{
	
					
		Component* pCompon = pBay->m_caComponents.GetComponent( iLoop );

		switch( pCompon->GetType() )
		{
		case( CT_BRACING ):
			switch( sbeB=pBay->GetPosition( iLoop ) )
			{
				case( NORTH ):			//fallthrough
				case( SOUTH ):
					
				case( EAST ):			//fallthrough
				case( WEST ):
					//no need to resize
				default:bCnt++;
					break;
			}
			break;
		case( CT_CHAIN_LINK ):			//fallthrough
		case( CT_SHADE_CLOTH ):
		case( CT_TIE_TUBE ):			//fallthrough
		
			break;
		case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
		case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
		case( CT_TIE_CLAMP_YOKE ):		//fallthrough
		case( CT_TIE_CLAMP_90DEGREE ):	//fallthrough
		case( CT_TEXT ):				//fallthrough
		case( CT_STANDARD ):			//fallthrough
		case( CT_STANDARD_OPENEND ):	//fallthrough
		case( CT_JACK ):				//fallthrough
		case( CT_SOLEBOARD ):			//fallthrough
			//We don't need to resize these components so skip the next section
			break;
		case( CT_DECKING_PLANK ):		//fallthrough
		case( CT_STAGE_BOARD ):			//fallthrough
		case( CT_LAPBOARD ):			//fallthrough
		case( CT_LEDGER ):				//fallthrough
		case( CT_TRANSOM ):				//fallthrough
		case( CT_MESH_GUARD ):			//fallthrough
		case( CT_RAIL ):				//fallthrough
		case( CT_MID_RAIL ):			//fallthrough
		case( CT_TOE_BOARD ):			//fallthrough
		case( CT_STAIR ):				//fallthrough
		case( CT_LADDER ):				//fallthrough
		case( CT_HOPUP_BRACKET ):		//fallthrough
		case( CT_CORNER_STAGE_BOARD ):	//fallthrough
		default:
			break;
		}

		
	}
if(lbCnt>0 && bCnt>0)
	endQty++;
lbCnt=bCnt;
					////////////////////////////////////////////////////////////
					//Count the length of the bays
					dBayLength = pBay->GetBayLength();
					if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
						dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR)
						iaBaySizes[BAY_SIZE_2400]++;
					else if( dBayLength>COMPONENT_LENGTH_1800-ROUND_ERROR &&
							 dBayLength<COMPONENT_LENGTH_1800+ROUND_ERROR)
						iaBaySizes[BAY_SIZE_1800]++;
					else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
							 dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR)
						iaBaySizes[BAY_SIZE_1200]++;
					else if( dBayLength>COMPONENT_LENGTH_0800-ROUND_ERROR &&
							 dBayLength<COMPONENT_LENGTH_0800+ROUND_ERROR)
						iaBaySizes[BAY_SIZE_0800]++;
					else if( dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
							 dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR)
						iaBaySizes[BAY_SIZE_0800]++;
					else
						;//assert( false );

					//	This is an unprocessed bay, so find out how
					//	much area it covers, we know the width
					//	so find the height
					dBayLower = (pBay->GetStandardPosition( CNR_NORTH_WEST ).z + 
								 pBay->GetStandardPosition( CNR_NORTH_EAST ).z ) / 2.00;
					dBayLower+= pBay->RLAdjust();
					//dBayLower-= pBay->JackLength();
					dBayUpper = pBay->GetRLOfTopLiftPlusHandrail(NORTH);
					dBayUpper+= pBay->RLAdjust();

					//We are only interested in the area of the bay between these levels
					//	default to infinity (1x10^6 will have to do)
					pLevels->GetRLLimitsForLevel( iLvl, dLower, dUpper );
					dBayLower = max( dBayLower, dLower );
					dBayUpper = min( dBayUpper, dUpper );
					;//assert( dBayUpper>=dBayLower );

					if( pLevels->GetSize()<=0 )
					{
						//There is no level info, so they must have selected the only level they can - ie Level1!
						//	also all the components should have 0 as thier default value!
						;//assert( pSummaryElement->GetLevel()==1 );
						;//assert( pComponent->GetLevel()==1 );
					}

					if( dBayUpper>dBayLower )
					{
						//Mark doesn't have the faintest fuck of an idea what
						//	to do here, so this is his best guess!

						//This is the old method from TWH+handrail to bottom top of jack
						//	dAreaOfThisBay = (dBayUpper-dBayLower) * pBay->GetBayLengthActual();
						double dRLOfTopLift = pBay->GetRLOfLift( pBay->GetNumberOfLifts()-1 )+pBay->RLAdjust();
						dAreaOfThisBay = (dRLOfTopLift-dBayLower+pBay->JackLength()) * pBay->GetBayLengthActual();
						dTotalArea+= dAreaOfThisBay;
					}
				}
				else
				{
					//we have already process the area for this bay at this level
				}
			}

			eType		= pComponent->GetType();
			dCompLength	= pComponent->GetLengthCommon();
			etypeArray[eType]++;	
			if( eType==CT_TEXT )
			{
				//don't count text!
				continue;
			}

			if( eType==CT_UNDEFINED )
			{
				//How did this happen?
				;//assert( false );
				continue;
			}

			if( eType==CT_BOM_EXTRA )
			{
				sPartNumber = pComponent->GetText();
			}
			else
			{
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(), eType, dCompLength, pComponent->GetMaterialType() );
				if( sPartNumber==UNKNOWN_COMP_TYPE )
				{
					sPartNumber = pComponent->GetStockDetails()->GetPartNumber();
				}
			}

			iQnty = 1;
			if( pComponent->GetRemoveComponentFromBOM() )
				iQnty = -1;
			if( (eType==CT_SHADE_CLOTH || eType==CT_CHAIN_LINK) )
			{
				//we need to work out which parts of this component belong to which stage and Level!
				;//assert( pBay->GetController()!=NULL );
				//This bay must already contain this stage, for this component, otherwise it wouldn't
				//	be in this list, so all we really need to do is find which levels of
 				//	this component belong to the desired level!
				pLevels->GetRLLimitsForLevel( iLvl, dLower, dUpper );
				double dWidth;
				switch( pComponent->GetSideOfBay() )
				{
				case( NORTH ):
					dWidth = pBay->GetBayLengthActual();
					eCnr1 = CNR_NORTH_EAST;
					eCnr2 = CNR_NORTH_WEST;
					break;
				case( EAST ):
					dWidth = pBay->GetBayWidthActual();
					eCnr1 = CNR_NORTH_EAST;
					eCnr2 = CNR_SOUTH_EAST;
					break;
				case( SOUTH ):
					dWidth = pBay->GetBayLengthActual();
					eCnr1 = CNR_SOUTH_EAST;
					eCnr2 = CNR_SOUTH_WEST;
					break;
				case( WEST ):
					dWidth = pBay->GetBayWidthActual();
					eCnr1 = CNR_NORTH_WEST;
					eCnr2 = CNR_SOUTH_WEST;
					break;
				default:
					;//assert( false ); //What type of chain mesh or shade cloth is this?
					continue;
				}

				dMaterialLower = pBay->GetStandardPosition( eCnr1 ).z + pBay->RLAdjust() - pBay->JackLength();
				dMaterialLower = max( dMaterialLower, pBay->GetStandardPosition( eCnr2 ).z + pBay->RLAdjust() - pBay->JackLength() );
				//Up to the top of the shade cloth - Rise stores the Height of the shade cloth
				dMaterialUpper = dMaterialLower + pComponent->GetRise();

				//Compare this range for the material with the range for the level
				if( pLevels->GetSize()>0 )
				{
					dMaterialUpper = min( dMaterialUpper, dUpper );
					dMaterialLower = max( dMaterialLower, dLower );
				}

				if( dMaterialUpper<dMaterialLower )
				{
					continue;
				}

				//find the area of the material within the levels
				dArea = dWidth * ( dMaterialUpper-dMaterialLower );
				dArea/= 1000000.00;

				if( eType==CT_SHADE_CLOTH && gpController->CalcPerRollShadeCloth() ||
					eType==CT_CHAIN_LINK && gpController->CalcPerRollChainMesh() )
				{
					if( eType==CT_SHADE_CLOTH && gpController->CalcPerRollShadeCloth() )
					{
						sPartNumber = gpController->GetPartNumberShadeCloth();
					}
					else if( eType==CT_CHAIN_LINK && gpController->CalcPerRollChainMesh() )
					{
						sPartNumber = gpController->GetPartNumberChainMesh();
					}
				}

				AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, eType, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty, dArea );
			}
			else if( (eType==CT_LEDGER) && (dCompLength>(COMPONENT_LENGTH_0700-ROUND_ERROR)) &&
				(dCompLength<(COMPONENT_LENGTH_0700+ROUND_ERROR)) && 
				(gpController->GetUse0700TransomForLedger()) )
			{
				//we have to substitute transom's for ledgers
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(), CT_TRANSOM, dCompLength, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, CT_TRANSOM, MT_STEEL, pComponent->GetSystem(), iQnty );
			}
			else if( (eType==CT_LEDGER) && (dCompLength>(COMPONENT_LENGTH_1200-ROUND_ERROR)) &&
				(dCompLength<(COMPONENT_LENGTH_1200+ROUND_ERROR)) && 
				(gpController->GetUse1200TransomForLedger()) )
			{
				//we have to substitute transom's for ledgers
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TRANSOM, dCompLength, pComponent->GetMaterialType() );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, CT_TRANSOM, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty );
			}
			else if( eType==CT_TIE_CLAMP_COLUMN )
			{
				//Column Ties have 2x90deg clamps, & 2xshort tie tubes
				//NOTE: The masonary tie(CT_TIE_CLAMP_MASONARY) is made up of special components, and is thus handled like any other component
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TIE_CLAMP_90DEGREE, 0.00, pComponent->GetMaterialType() );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, 0.00, CT_TIE_CLAMP_90DEGREE, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty*2 );
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TIE_TUBE, COMPONENT_LENGTH_0600, pComponent->GetMaterialType() );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, COMPONENT_LENGTH_0600, CT_TIE_TUBE, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty*2 );
			}
			else if( eType==CT_TIE_CLAMP_YOKE )
			{
				//Column Ties have 4x90deg clamps, & 3xShort tie tubes
				//NOTE: The masonary tie(CT_TIE_CLAMP_MASONARY) is made up of special components, and is thus handled like any other component
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TIE_CLAMP_90DEGREE, 0.00, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, 0.00, CT_TIE_CLAMP_90DEGREE, MT_STEEL, pComponent->GetSystem(), iQnty*4 );
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TIE_TUBE, COMPONENT_LENGTH_0600, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, COMPONENT_LENGTH_0600, CT_TIE_TUBE, MT_STEEL, pComponent->GetSystem(), iQnty*3 );
			}
			else if( eType==CT_LADDER && gpController->GetUsePutLogClipsInLadder() )
			{
				//In Melbourne they use a 2 putlog clips per ladder

				//Add the ladder
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, eType, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty );
				//add 2 putlog clips
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_PUTLOG_CLIP, 0, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, 0.00, CT_PUTLOG_CLIP, MT_STEEL, pComponent->GetSystem(), 2*iQnty );
				//They also replace a 1.2m transom with a 1.2m ledger to tie it to
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_LEDGER, COMPONENT_LENGTH_1200, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, COMPONENT_LENGTH_1200, CT_LEDGER, MT_STEEL, pComponent->GetSystem(), 1 );
				//Remove the transom
				sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TRANSOM, COMPONENT_LENGTH_1200, MT_STEEL );
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, COMPONENT_LENGTH_1200, CT_TRANSOM, MT_STEEL, pComponent->GetSystem(), -1 );
			}
			else if( eType==CT_SOLEBOARD && gpController->GetUse1500Soleboards() &&
					pComponent!=NULL &&
					pComponent->GetBayPointer()!=NULL &&
					pComponent->GetBayPointer()->GetBayWidth()==COMPONENT_LENGTH_1200 &&
					pComponent->GetBayPointer()->GetInner()==NULL &&
					pComponent->GetBayPointer()->GetOuter()==NULL )
			{
				//NOTE 20000731 - I have implemented 1.5m soleboards as an after thought,
				//	so I actually have to perform this calculation a few times, once here and
				//	once Just before I draw it in 3D & Matix, I hope these calculations produce
				//	the same result.
				//	There are several different methods I could have used to create 1.5m
				//	soleboards, we are currently using method 3:
				//	1) THE PROPER WAY - allow the user to edit the soleboard style and
				//		quantity via the Bay details dialog.
				//		PROs - Most adaptable to future changes;
				//		CONs - Lots of Code changes; Lots of error checking for chaning RLs;
				//	2) THE COMPONENT WAY - When you add the component, change the length or
				//		remove the component
				//		PROs - Simple to code;
				//		CONs - Has to assume the other leg has a soleboard; Changing RLs
				//			must force a recalc; no changes to Matrix & 3D code;
				//	3) *THE CHEAT WAY - work internally as .5m soleboards then when creating
				//		the BOM, or drawing the 3D or Matrix, at those stages then check
				//		if it is possible to add 1.5m soleboards
				//		PROs - Simpler to code; 
				//		CONs - calculated in several places thus could generate diff results;
				//			changes to 3 places in the code;

				//This is a soleboard for a 1200 wide bay, 
				//	and we are trying to use 1.5m soleboards
				//	plus there are no N/S neighbors
				bool			bRLsSame;
				double			dRL[2];
				SideOfBayEnum	eSide;

				eSide = pComponent->GetSideOfBay();
				bRLsSame = false;
				if( eSide==NE || eSide==SE )
				{
					dRL[0] = pComponent->GetBayPointer()->GetStandardPosition(CNR_NORTH_EAST).z;
					dRL[1] = pComponent->GetBayPointer()->GetStandardPosition(CNR_SOUTH_EAST).z;
					if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR )
						bRLsSame = true;
				}
				if( eSide==NW || eSide==SW )
				{
					dRL[0] = pComponent->GetBayPointer()->GetStandardPosition(CNR_NORTH_WEST).z;
					dRL[1] = pComponent->GetBayPointer()->GetStandardPosition(CNR_SOUTH_WEST).z;
					if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR )
						bRLsSame = true;
				}

				if( bRLsSame )
				{
					//The RL's are the same between NORTH/SOUTH standard sets
					if( eSide==SW || eSide==SE )
					{
						//change the component lenght to 1.5m
						sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_SOLEBOARD, COMPONENT_LENGTH_1500, pComponent->GetMaterialType() );
						AddBOMComponentToList( BOMDetailsArray, sPartNumber, COMPONENT_LENGTH_1500, eType, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty );
					}
					else
					{
						//These must be the northern side, so
						//	make sure we don't add a component
					}
				}
				else
				{
					//Just add the soleboard
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, eType, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty );
				}
			}
			else
			{
				//NOTE: This will also handle the masonary tie(CT_TIE_CLAMP_MASONARY)
				AddBOMComponentToList( BOMDetailsArray, sPartNumber, dCompLength, eType, pComponent->GetMaterialType(), pComponent->GetSystem(), iQnty );
			}

			///////////////////////////////////////////////////////
			//every stage board #2 also needs a tiebar!
			if( eType==CT_STAGE_BOARD && pLift!=NULL && pComponent->GetRise()==1 )
			{
				daLengths = m_pComponentDetailsArray->GetCommonLengthsForComponent( CT_TIE_BAR, MT_STEEL );

				//find the right lenght of tie bar!
				bFound = false;
				if( dCompLength==COMPONENT_LENGTH_0700 )
					dCompLength=COMPONENT_LENGTH_0800;
				for( int iCount=0; iCount<daLengths.GetSize(); iCount++ )
				{
					dTestLength = daLengths[iCount];
					if( dTestLength>dCompLength-ROUND_ERROR &&
						dTestLength<dCompLength+ROUND_ERROR )
					{ 
						bFound = true;
						break;
					}
				}

				if( bFound )
				{
					//We need to add a tie bar!
					sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(), CT_TIE_BAR, dCompLength, MT_STEEL );
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dTestLength, CT_TIE_BAR, MT_STEEL, pComponent->GetSystem(), iQnty );
				}
				else
				{
					;//assert( false );	//why is there a stage board without matching tie bar?
				}
			}

			if( pComponent->GetSystem()==S_MILLS && 
				(eType==CT_STANDARD || eType==CT_STANDARD_OPENEND ) )
			{
				//Standard connectors
				//This is a standard in the mill system, therefore we need a conector
				//	unless it is the bottom standard in a pole, Rise indicates it
				//	position in the pole!
				if( pComponent->GetRise()>0 )
				{
					double dLength = 150.00;
					sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_STANDARD_CONNECTOR, dLength, MT_STEEL );
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dLength, CT_STANDARD_CONNECTOR, MT_STEEL, pComponent->GetSystem(), iQnty );
				}
			}
		
			if( eType==CT_DECKING_PLANK )
			{
				//We need a putlog if the last plank is shorter than the baywidth!
				if( pLift!=NULL && pBay!=NULL && 
					dCompLength<pBay->GetBayLength() &&
					pComponent->GetRise()==GetNumberOfPlanksForWidth( pBay->GetBayWidth() )-1 )
				{
					//The Ladder needs to have a Putlog
					double dLength = COMPONENT_LENGTH_1200;
					if( pBay!=NULL )
					{
						dLength = pBay->GetBayWidth();
					}

					sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_LADDER_PUTLOG, dLength, MT_STEEL );
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dLength, CT_LADDER_PUTLOG, MT_STEEL, pComponent->GetSystem(), iQnty );
				}
			}

			if( eType==CT_STAIR	)
			{
				//The stair must belong to a lift
				if( pLift!=NULL && pBay!=NULL )
				{
					//If it is the first lift, then use a stopend, otherwise use a normal 
					double dLength;
					dLength = pComponent->GetLengthCommon();
					ComponentTypeEnum eRailType;
					if( pLift->GetLiftID()==0 && pComponent->GetRise()==SPR_OUTER )
						eRailType = CT_STAIR_RAIL_STOPEND;
					else
						eRailType = CT_STAIR_RAIL;

					sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  eRailType, dLength, MT_STEEL );
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dLength, eRailType, MT_STEEL, pComponent->GetSystem(), iQnty );
				}
			}
			if(eType==CT_CORNER_STAGE_BOARD)
			{
			}

			if( eType==CT_TOE_BOARD	)
			{
				//The stair must belong to a lift
				if( pLift!=NULL && pBay!=NULL )
				{
					double dLength;
					dLength = 0;
					if( pBay->GetID()==0 )
					{
						//If it is the first bay in a run then we need two toe board clips
						sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TOE_BOARD_CLIP, dLength, MT_STEEL );
						AddBOMComponentToList( BOMDetailsArray, sPartNumber, dLength, CT_TOE_BOARD_CLIP, MT_STEEL, pComponent->GetSystem(), iQnty);
					}
					sPartNumber = m_pComponentDetailsArray->GetComponentPartNumberStr( pComponent->GetSystem(),  CT_TOE_BOARD_CLIP, dLength, MT_STEEL );
					AddBOMComponentToList( BOMDetailsArray, sPartNumber, dLength, CT_TOE_BOARD_CLIP, MT_STEEL, pComponent->GetSystem(), iQnty);
				}
			}
		}
	}
	//Sort the list by PartNumber, ascending
	BOMDetailsArray.Sort();

	int iCommitted;

	sStageLevel.Empty();
	iCommitted = 0;
	CString sText;
	m_saCSVData.Add( _T("\n!") );
	sText = _T("\nStage/Level,");
	for( iAmalga=0; iAmalga<iaSIIndex.GetSize(); iAmalga++ )
	{
		CString sTemp;

		iSIIndex = iaSIIndex.GetAt( iAmalga );
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);

		if( iAmalga>0 )
		{
			sStageLevel += _T(", ");
			sText+= _T(",");
		}

		sTemp.Format( _T("%s/%i"), pSummaryElement->GetStage(), pSummaryElement->GetLevel() );
		sStageLevel+= sTemp;
		sText+= sTemp;

		if( iAmalga==iaSIIndex.GetSize()-1 )
			sStageLevel += _T(".");

		if( pSummaryElement->IsCommitted() )
			iCommitted++;
	}
	m_saCSVData.Add( sText );
	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Stage/Level"));
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);

	if(iaBays.GetSize()>0)
	{
		/////////////////////////////////////////////////////
		//print the Bay numbers
		int iLines=0;
		if( gpController->GetBOMSummaryToShowBays() )
		{
			iaBays.SelectionSort();
			int	iBayStart, iBay, iBayCounter;
			CString sBays, sTemp;
			iBayStart = -1;
			sBays.Empty();
			for( i=0; i<iaBays.GetSize(); i++ )
			{
				iBay = iaBays.GetAt(i);
				;//assert( iBay>0 );
/*				if( sBays.GetLength()/10>0 )
				{
					sTemp.Empty();
					if( iLines>0 )
					{
						if( iaBays.GetSize()>1 )
							sTemp = "Bays";
						else
							sTemp = "Bay";
					}
					iCurrentListCtrlIndex++;
					iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, sTemp );
					m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sBays );
					sBays.Empty();
					iLines++;
				}
*/				if( iBayStart<=0 )
				{
					iBayStart = iBay;
					iBayCounter	= iBay;

					if( i==iaBays.GetSize()-1 )
					{
						if( !sBays.IsEmpty() || iLines>0)
							sBays+= _T(" & ");
						sTemp.Format( _T("%i"), iBay );
						sBays+= sTemp;
					}
				}
				else
				{
					iBayCounter++;
					if( iBay!=iBayCounter || i==iaBays.GetSize()-1 )
					{
						//not sequential
						if( !sBays.IsEmpty() || iLines>0 )
						{
							if( i==iaBays.GetSize()-1 )
								sBays+= _T(" & ");
							else
								sBays+= _T(", ");
						}
						if( (iBayStart!=iBay) && ((iBayCounter-1)==iBayStart) )
						{
							//There is only one bay in this sequence
							sTemp.Format( _T("%i"), iBayStart );
							sBays+= sTemp;
							//This is non-sequential, so process this one again
							i--;
						}
						else
						{
							//There is more than one bay in this sequence
							if( iBay==iBayCounter && i==iaBays.GetSize()-1 )
							{
								//The last bay is sequential
								sTemp.Format( _T("%i-%i"), iBayStart, iBayCounter );
							}
							else
							{
								//This is either not the last bay or it is not
								//	sequential, or both
								sTemp.Format( _T("%i-%i"), iBayStart, iBayCounter-1 );
								//This is non-sequential, so process this one again
								i--;
							}
							sBays+= sTemp;
						}

						//next time we will be starting a new sequence
						iBayStart=-1;
					}
				}
			}
			
/*			sTemp.Empty();
			if( iLines>0 )
			{
*/				if( iaBays.GetSize()>1 )
					sTemp = _T("Bays");
				else
					sTemp = _T("Bay");
/*			}
*/
			if( sBays.GetLength()>0 )
			{
				sBays+= _T(".");
				iCurrentListCtrlIndex++;
				iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, sTemp );
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sBays );
			}
		}
		
		/////////////////////////////////////////////////////
		//print the Quantity vs size of bays
		if( gpController->GetBOMSummaryToShowBaySizes() )
		{
			int		iCount;
			CString	sText, sTemp;

			//Count how many we need
			iCount = 0;
			for( i=0; i<BAY_SIZE_SIZE; i++ )
			{
				if( iaBaySizes[i]>0 )
					iCount++;
			}

			;//assert( iCount>0 );
			sTemp.Empty();
			dBayTotalLength=0;
			for( i=0; i<BAY_SIZE_SIZE; i++ )
			{
				if( iaBaySizes[i]>0 )
				{
					if( !sText.IsEmpty() )
						sText+= _T("; ");

					switch(i)
					{
					case( BAY_SIZE_0800 ):
						dBayLength = COMPONENT_LENGTH_0800;
						break;
					case( BAY_SIZE_1200 ):
						//Added on 08-Dec-2017, to handle deck case
						if(pLevels->GetSize() == 0){
							dBayLength = COMPONENT_LENGTH_1270;
						}
						else
						{
						dBayLength = COMPONENT_LENGTH_1200;
						}
						break;
					case( BAY_SIZE_1800 ):
						//Added on 08-Dec-2017, to handle deck case
						if(pLevels->GetSize() == 0){
							dBayLength = COMPONENT_LENGTH_1830;
						}
						else
						{
						dBayLength = COMPONENT_LENGTH_1800;
						}
						break;
					case( BAY_SIZE_2400 ):
						//Added on 08-Dec-2017, to handle deck case
						if(pLevels->GetSize() == 0){
							dBayLength = COMPONENT_LENGTH_2439;
						}
						else
						{
						dBayLength = COMPONENT_LENGTH_2400;
						}
						break;
					default:
						;//assert( false );
					}

					sTemp.Format( _T("%ix%1.1fm"), iaBaySizes[i], dBayLength*CONVERT_MM_TO_M );
					sText+= sTemp;
					dBayTotalLength += iaBaySizes[i]*dBayLength*CONVERT_MM_TO_M;
					iCount--;
				}
			}

			sText+= _T(".");
			iCurrentListCtrlIndex++;
			iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Bay sizes") );
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sText );
		}
	}

	double dTemp;
	double dLower1=0;
	pLevels->GetRLLimitsForLevel( iLvl, dLower, dUpper );
	double dUpper1=dUpper;
	 dLower1=dLower;
	if(dUpper>=10000000)
	{
	dUpper=dLower;
	pLevels->GetRLLimitsForLevel( iLvl-1,dLower,  dTemp  );
	}
	if(dLower<0) dLower=0;

	if(iaSIIndex.GetSize()==1 && iLvl==0)
		dUpper1=dUpper=3000;
	slabHeight=(dUpper-dLower)*CONVERT_MM_TO_M;

double rise;
	deckQty=GetCurrentNumberOfDecks(pBay,dLower1,dUpper1,rise);
	deckHeight=1.8;
	endLength =  pBay->GetBayWidthActual()*CONVERT_MM_TO_M;
	endLength = ((int)((endLength+0.05)*10))*0.1;
	dEndTotalLength=endQty*endLength;
	
	deckHeight=rise*CONVERT_MM_TO_M-0.225;
	deckHeight=((int)((deckHeight+0.05)*10))*0.1;
	double deckHeightTop=0;
	deckHeightTop=rise*CONVERT_MM_TO_M;
	deckHeightTop=((int)((deckHeightTop+0.05)*10))*0.1;
	double lenIncLaps=dBayTotalLength+dLapTotalLength;
	double lenIncLapsEnds=dBayTotalLength+dLapTotalLength+dEndTotalLength;
	double nExternalTubeLink = 0;


	deckFaceArea=deckQty*deckHeight*lenIncLapsEnds;
	totalFaceArea=slabHeight*lenIncLapsEnds;
	toTopDeckArea=slabHeight*lenIncLaps;//deckQty*deckHeightTop*lenIncLaps;

	if(dUpper1>=10000000)
	slabHeight+=2;
	toTopGuardRailArea=slabHeight*lenIncLaps;
	
	//Added on 08-Dec-2017, to handle deck case
	if(pLevels->GetSize() == 0)
	{
		deckQty = GetCurrentNumberOfDecks(pBay);
		// Need to add Ext. tube link for all 4 formulae
		if(iIndex == 614 || iIndex == 1495 || iIndex == 2015)
		{
			slabHeight=10;
		}
		else
		{
			slabHeight=9;
		}
		if(iIndex == 1495 || iIndex == 1385)
		{
			endQty = 1;
			nExternalTubeLink = ((1.2*1)); //(1.83*3) + (1.27*2) +
		}
		if(iIndex == 2015 || iIndex == 1887)
		{
			endQty=0;
			nExternalTubeLink = ( (1.8*4)); //(1.83*1) + (1.27*4) +
		}
		deckFaceArea = (deckQty*1.8*(dBayTotalLength+ ( nExternalTubeLink + ((pBay->GetBayWidthActual()*CONVERT_MM_TO_M) *(2+ endQty))+ 0.508*2)));
		totalFaceArea = (slabHeight*(dBayTotalLength+ ( nExternalTubeLink + ((pBay->GetBayWidthActual()*CONVERT_MM_TO_M) *(2+ endQty))+ 0.508*2))); //Need to multiply with Total outside standard height
		if((iIndex == 1385) || (iIndex == 2015) || (iIndex == 1887))
		{
			toTopDeckArea = ((dBayTotalLength + nExternalTubeLink)*8); // Constant=8 <- for breadth of lift 
			toTopGuardRailArea = (slabHeight*(dBayTotalLength + nExternalTubeLink)); //Need to multiply with Total outside standard height		
		}
		else
		{
			toTopDeckArea = (dBayTotalLength*8); // Constant=8 <- for breadth of lift 
			toTopGuardRailArea = (slabHeight*dBayTotalLength); //Need to multiply with Total outside standard height
		}
	}
	

	//double newval=((int)(deckFaceArea * 100 + .5) / 100.0);

	/////////////////////////////////////////////////////
	//print the Area and committed
	iCurrentListCtrlIndex++;
	sStageLevel.Format(_T("%0.2fm2(bays) + %0.2fm2(Laps) = %0.2fm2"),
					dTotalArea/1000000.00,
					dTotalLapArea/1000000.00,
					(dTotalArea+dTotalLapArea)/1000000.00 );
	//sText is saved to the CSV file
	sText.Format(_T("\nAREA,%0.2fm2"), (dTotalArea+dTotalLapArea)/1000000.00 );
	if( iCommitted==iaSIIndex.GetSize() )
	{
		sStageLevel+= _T("   COMMITTED");
		sText+= _T("\n!COMMITTED");
	}
	else if( iCommitted>0 )
	{
		sStageLevel+= _T("   SOME COMMITTED");
		sText+= _T("\n!SOME COMMITTED");
	}
  iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("AREA:"));
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);
	m_saCSVData.Add( sText );
	++iCurrentListCtrlIndex;
	
	//List deck Face Area
	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("DECK FACE AREA:"));
	sStageLevel.Format(_T("%0.2fm2"),deckFaceArea);
	sText.Format(_T("\nDECK FACE AREA,%0.2fm2"),deckFaceArea );
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);
	m_saCSVData.Add( sText );
	++iCurrentListCtrlIndex;

	//List total face Area
	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("TOTAL FACE AREA:"));
	sStageLevel.Format(_T("%0.2fm2"),totalFaceArea);
	sText.Format(_T("\nTOTAL FACE AREA,%0.2fm2"),totalFaceArea );
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);
	m_saCSVData.Add( sText );
	++iCurrentListCtrlIndex;
	
	//List to top deck Area
	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("TO TOP DECK AREA:"));
	sStageLevel.Format(_T("%0.2fm2"),toTopDeckArea);
	sText.Format(_T("\nTO TOP DECK AREA,%0.2fm2"),toTopDeckArea );
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);
	m_saCSVData.Add( sText );
	++iCurrentListCtrlIndex;

	//List to top guardrail Area
	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("TO TOP GUARDRAIL AREA:"));
	sStageLevel.Format(_T("%0.2fm2"),toTopGuardRailArea);
	sText.Format(_T("\nTO TOP GUARDRAIL AREA,%0.2fm2"),toTopGuardRailArea );
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sStageLevel);
	m_saCSVData.Add( sText );
	++iCurrentListCtrlIndex;

	// display the list
	if( gpController->IsDisplaySalePrice() )
	{
		//Show all components
		DisplayList( &BOMDetailsArray, iCurrentListCtrlIndex, BOMSHOW_SALE_AND_HIRE );
	}
	else
	{
		BOMSaleArray.RemoveAll();
		bool bShowSale;
		bShowSale = SeperateOutSaleOnlyItems( &BOMDetailsArray, &BOMSaleArray );

		//Show components that have a hire price
		iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		++iCurrentListCtrlIndex;
		iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Hired Components ($/week)"));
		++iCurrentListCtrlIndex;
		DisplayList( &BOMDetailsArray, iCurrentListCtrlIndex, BOMSHOW_HIRE_ONLY );

		if( bShowSale )
		{
			//Since we have a hire and sale section we need a grand total
			bShowGrandTotal = true;
			;//assert( BOMSaleArray.GetSize()>0 );
			//Show components that ONLY have a sale price
			iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
			++iCurrentListCtrlIndex;
			iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Sale Components ($)"));
			++iCurrentListCtrlIndex;
			DisplayList( &BOMSaleArray, iCurrentListCtrlIndex, BOMSHOW_SALE_ONLY );
		}
	}

	if( bShowGrandTotal )
	{
		DispalyGrandTotals( iCurrentListCtrlIndex );
	}

	//cleanup the BOMDetailsArray
	BOMDetailsArray.RemoveAll();
	BOMSaleArray.RemoveAll();
}

void BOMSummaryDialog::OnBomExtra() 
{
	EndDialog( IDRETRY );
}

void BOMSummaryDialog::AddBOMComponentToList(BOMExtraArray &BOMDetailsArray, CString sPartNumber, double dLength, ComponentTypeEnum eType, MaterialTypeEnum eMT, SystemEnum eSystem, int iQnty/*=1*/, double dArea/*=0.00*/ )
{
	int				iBOMIndex, iBOMDetailsArraySize;
	bool			bFound;
	CString			sPartNoInList;
	BOMExtraElement	*pBOMComponent, *pBOMComponentInList;

	if( iQnty<0 )
		dArea*= -1.00;

	pBOMComponent = new BOMExtraElement();
	pBOMComponent->m_sPartNumber    = sPartNumber;
	pBOMComponent->m_dLen		= dLength;
	pBOMComponent->m_eComponentType = eType;
	pBOMComponent->m_eMaterialType	= eMT;
	pBOMComponent->m_iQuantity		= iQnty;
	pBOMComponent->m_dArea			= dArea;
	pBOMComponent->m_eSystem		= eSystem;

	bFound = false;
	// see if component alrealy exist in list
	iBOMDetailsArraySize = BOMDetailsArray.GetSize();
	sPartNumber.MakeUpper();
	for( iBOMIndex=0; iBOMIndex<iBOMDetailsArraySize; iBOMIndex++ )
	{
		pBOMComponentInList = BOMDetailsArray.GetAt(iBOMIndex);
		sPartNoInList = pBOMComponentInList->m_sPartNumber;
		sPartNoInList.MakeUpper();
		if( sPartNoInList==sPartNumber )
		{
			bFound = true;
			break;
		}
	}	

	
	if( bFound )
	{
		//if so add 1 to list and delete new element
		pBOMComponentInList->m_iQuantity	+=pBOMComponent->m_iQuantity;
		pBOMComponentInList->m_dArea		+=pBOMComponent->m_dArea;
		if( pBOMComponentInList->m_iQuantity<=0 || pBOMComponentInList->m_dArea<(-1.00*ROUND_ERROR_SMALL) )
		{
			;//assert( iBOMIndex<iBOMDetailsArraySize );
			BOMDetailsArray.RemoveAt( iBOMIndex );
		}
		DELETE_PTR(pBOMComponent);
	}
	else
	{
		if( pBOMComponent->m_iQuantity>0 )
		{
			// else add the new element to the list
			BOMDetailsArray.Add(pBOMComponent);
		}
	}
}

void BOMSummaryDialog::DisplayJobDetails(int &iCurrentListCtrlIndex)
{
	CString				sJobDetails, sTemp;
	JobDescriptionInfo	*pJobDetails;

	;//assert( gpController!=NULL );
	pJobDetails = gpController->GetJobDescription();

	//Job Details
	if( !pJobDetails->m_sMarketingEnquiryNumber.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("M.E. #:") ); 	
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sMarketingEnquiryNumber );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sRefNumber.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Ref #:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sRefNumber );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sJobTitle.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Job:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sJobTitle );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sJobDescription.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Desc:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sJobDescription );
		iCurrentListCtrlIndex++;
	}

	//Draftsman details
	if( !pJobDetails->m_sDraftsmanName.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Draftsman:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sDraftsmanName );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sJobBranch.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Branch:") );
  		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sJobBranch );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sPhoneNumber.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Phone:") );
		sTemp.Empty();
		if( !pJobDetails->m_sPhoneCountryCode.IsEmpty() && !pJobDetails->m_sPhoneStateCode.IsEmpty() )
			sTemp.Format( _T("+%s(%s) "), pJobDetails->m_sPhoneCountryCode, pJobDetails->m_sPhoneStateCode );
		sTemp+= pJobDetails->m_sPhoneNumber;
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, sTemp );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sFaxNumber.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Fax:") );
		sTemp.Empty();
		if( !pJobDetails->m_sFaxCountryCode.IsEmpty() && !pJobDetails->m_sFaxStateCode.IsEmpty() )
			sTemp.Format( _T("+%s(%s) "), pJobDetails->m_sFaxCountryCode, pJobDetails->m_sFaxStateCode );
		sTemp+= pJobDetails->m_sFaxNumber;
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, sTemp );
		iCurrentListCtrlIndex++;
	}

	//Client Details
	if( !pJobDetails->m_sClientName.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Client:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sClientName );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sClientCompany.IsEmpty() )
	{
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sClientCompany );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sClientAddress.IsEmpty() )
	{
    m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T("Address:") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sClientAddress );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sClientCity.IsEmpty() )
	{
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sClientCity );
		iCurrentListCtrlIndex++;
	}
	if( !pJobDetails->m_sClientZip.IsEmpty() )
	{
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sClientZip );
		iCurrentListCtrlIndex++;
	}

	//no one seems to want these ones
//	pJobDetails->m_sProjectDuration;
//	pJobDetails->m_sStartDate;

	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, _T(" ") );
	iCurrentListCtrlIndex++;
	
/*
	//Delivery Details
	if( !pJobDetails->m_sDeliveryInstructions.IsEmpty() )
	{
		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, "Delivery Notes:" );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sDeliveryInstructions );
		iCurrentListCtrlIndex++;

		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, "Delivery Time:" );
		m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, pJobDetails->m_sDeliveryTime );
		iCurrentListCtrlIndex++;

		m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, "Drawing Sent:" );
		if( pJobDetails->m_bDrawingSent )
			m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, "Yes" );
		else
			m_BOMSummaryNewListCtrl.SetItemText(iCurrentListCtrlIndex, COLUMN_DESC, "No" );
		iCurrentListCtrlIndex++;
	}

	m_BOMSummaryNewListCtrl.InsertItem(iCurrentListCtrlIndex, " " );
	iCurrentListCtrlIndex++;
*/
}


void BOMSummaryDialog::CheckStandardsAndTransoms()
{
	int				i, iIndex, iSIIndex;
	Bay				*pBayNeighbor;
	Component		*pComponent;
	CompArray		CompList;
	SummaryElement	*pSummaryElement;

	///////////////////////////////////////////////////////////
	//Build a list of all components in this printout!
	CompList.RemoveAll();
	for (iIndex = 0; iIndex < m_pSelectedListBox->GetCount(); ++iIndex)
	{
		iSIIndex = m_pSelectedListBox->GetItemData(iIndex);
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);

		for( i=0; i<pSummaryElement->GetNumberOfComponents(); i++ )
		{   
			// make a BOM element
			pComponent = pSummaryElement->GetComponent(i);
			CompList.Add( pComponent );
		}
	}

	///////////////////////////////////////////////////////////
	//Every time we find a Northern Ledger at 0000mm we need a
	//	Western tranny at 0000mm!
	for( iIndex=0; iIndex < m_pSelectedListBox->GetCount(); ++iIndex)
	{
		iSIIndex = m_pSelectedListBox->GetItemData(iIndex);
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);

		for( i=0; i<pSummaryElement->GetNumberOfComponents(); i++ )
		{   
			// make a BOM element
			pComponent = pSummaryElement->GetComponent(i);

			if( pComponent->GetLiftPointer()!=NULL && 
				pComponent->GetLiftPointer()->GetBayPointer()!=NULL )
			{
				//////////////////////////////////////////////////////////////////////
				//Ensure we have a matching tranny on the West!
				pBayNeighbor = pComponent->GetLiftPointer()->GetBayPointer()->GetBackward();
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_LEDGER, NORTH, CT_TRANSOM, EAST, LIFT_RISE_0000MM,
										pBayNeighbor, WEST );
				//Ensure we have a matching Hopup on the SSW
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_STAGE_BOARD, SOUTH, CT_HOPUP_BRACKET, SSE, LIFT_RISE_0000MM,
										pBayNeighbor, SSW );
				//Ensure we have a matching Hopup on the NNW
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_STAGE_BOARD, NORTH, CT_HOPUP_BRACKET, NNE, LIFT_RISE_0000MM,
										pBayNeighbor, NNW );
				//Ensure we have a Supporting standards on the NW
				ReStageStandards( pComponent, pSummaryElement, &CompList, pBayNeighbor, NE, NORTH, WEST, NORTH );
				//Ensure we have a Supporting standards on the SW
				ReStageStandards( pComponent, pSummaryElement, &CompList, pBayNeighbor, SE, SOUTH, WEST, NORTH );

				//////////////////////////////////////////////////////////////////////
				//Ensure we have a matching Ledger on the South!
				pBayNeighbor = pComponent->GetLiftPointer()->GetBayPointer()->GetInner();
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_LEDGER, NORTH, CT_LEDGER, NORTH, LIFT_RISE_0000MM,
										pBayNeighbor, SOUTH );
				//Ensure we have a matching Hopup on the ESE
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_STAGE_BOARD, EAST, CT_HOPUP_BRACKET, ENE, LIFT_RISE_0000MM,
										pBayNeighbor, ESE );
				//Ensure we have a matching Hopup on the WSW
				ReStageMatchingComp( pComponent, pSummaryElement, &CompList,
										CT_STAGE_BOARD, WEST, CT_HOPUP_BRACKET, WNW, LIFT_RISE_0000MM,
										pBayNeighbor, WSW );
				//Ensure we have a Supporting standards on the SW
				ReStageStandards( pComponent, pSummaryElement, &CompList, pBayNeighbor, NW, SOUTH, WEST, NORTH );
				//Ensure we have a Supporting standards on the SE
				ReStageStandards( pComponent, pSummaryElement, &CompList, pBayNeighbor, NE, SOUTH, WEST, NORTH );
			}
			else if( pComponent->GetBayPointer()!=NULL )
			{
				pBayNeighbor = pComponent->GetBayPointer()->GetBackward();

				//We may need a wall tie
				if( pBayNeighbor!=NULL && pBayNeighbor->GetTemplate()->GetSSETie() )
				{
					int			j, k;
					bool		bMatchFound;
					Component	*pComp;

					for( j=0; j<pBayNeighbor->GetNumberOfBayComponents(); j++ )
					{
						pComp = pBayNeighbor->GetBayComponent( j );
						if( pComp->GetSideOfBay()==SSE && (
							pComp->GetType()==CT_TIE_TUBE ||
							pComp->GetType()==CT_TIE_CLAMP_COLUMN ||
							pComp->GetType()==CT_TIE_CLAMP_MASONARY ||
							pComp->GetType()==CT_TIE_CLAMP_YOKE ||
							pComp->GetType()==CT_TIE_CLAMP_90DEGREE ) &&
							pComp->GetLevel()==pSummaryElement->GetLevel() )
						{
							//We know the component exist, is it in the list of selected components?
							bMatchFound = false;
							//find a matching tranny
							for( k=0; k<CompList.GetSize(); k++ )
							{
								if( CompList.GetAt(k)==pComp )
								{
									bMatchFound = true;
									break;
								}
							}

							if( !bMatchFound )
							{
								//Store the component stage and level for next time!
								pComp->SetStage( pComponent->GetStage() );	
								pComp->SetLevel( pComponent->GetLevel() );
								//Add the moved Component to the pSummaryElement component list
								pSummaryElement->AddComponent( pComp );
								m_CompsChanged.Add(pComp);
								CompList.Add(pComp);
							}
						}
					}
				}
			}
		}
	}
}


void BOMSummaryDialog::OnClose() 
{
	//Reset all the ones we changed
	Component *pComp;
	for( int i=0; i<m_CompsChanged.GetSize(); i++ )
	{
		pComp = m_CompsChanged.GetAt(i);
		if( !pComp->IsCommitted() )
		{
			pComp->SetStage( STAGE_DEFAULT_VALUE );
			pComp->SetLevel( LEVEL_DEFAULT_VALUE );
		}
	}
	KwikscafDialog::OnOK();
}

void BOMSummaryDialog::OnCommit() 
{
	int				iIndex, iSIIndex, jIndex;
	Component		*pComponent;
	SummaryElement	*pSummaryElement;

	//Show a warning about the committed button
	CString sMsg;
  sMsg = _T("Warning: This action will mark all these items as\n");
	sMsg+= _T("committed, i.e. they have been sent out to site!\n");
	sMsg+= _T("These components will then appear as grey in the\n");
	sMsg+= _T("3D view, and their stage value cannot be reset.\n");
	sMsg+= _T("To reset this committed flag, please use the\n");
	sMsg+= _T("'ClearCommitted' command at the prompt.");
	if( MessageBox( sMsg, _T("Commit"), MB_OKCANCEL )==IDCANCEL )
		return;

	for (iIndex = 0; iIndex < m_pSelectedListBox->GetCount(); ++iIndex)
	{
		iSIIndex = m_pSelectedListBox->GetItemData(iIndex);
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);

		for (jIndex = 0; jIndex < pSummaryElement->GetNumberOfComponents(); jIndex++)
		{   
			// make a BOM element
			pComponent = pSummaryElement->GetComponent(jIndex);
			pComponent->SetCommitted( true );
		}
	}
}

void BOMSummaryDialog::OnUncommit() 
{
	int				iIndex, iSIIndex, jIndex;
	Component		*pComponent;
	SummaryElement	*pSummaryElement;

	//Show a warning about the committed button
	CString sMsg;
  sMsg = _T("Warning: This action will REMOVE the committed flag\n");
	sMsg+= _T("from ALL these items\n\n");
	sMsg+= _T("To reset this committed flag for the entire drawing,\n");
	sMsg+= _T("please use the 'ClearCommitted' command at the prompt.");
	if( MessageBox( sMsg, _T("Uncommit"), MB_OKCANCEL )!=IDOK )
		return;

	for (iIndex = 0; iIndex < m_pSelectedListBox->GetCount(); ++iIndex)
	{
		iSIIndex = m_pSelectedListBox->GetItemData(iIndex);
		pSummaryElement = m_pSummaryInfo->GetElement(iSIIndex);

		for (jIndex = 0; jIndex < pSummaryElement->GetNumberOfComponents(); jIndex++)
		{   
			// make a BOM element
			pComponent = pSummaryElement->GetComponent(jIndex);
			pComponent->SetCommitted( false );
		}
	}
}

void BOMSummaryDialog::ShowCommitButtions()
{
	
}

void BOMSummaryDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("BOMSummaryDialog") );
	
	
}

void BOMSummaryDialog::ReStageMatchingComp(Component *pComponent, SummaryElement *pSummaryElement,
										   CompArray *pCompList, ComponentTypeEnum eType, SideOfBayEnum eSide,
										   ComponentTypeEnum eTypeMatch, SideOfBayEnum eSideMatch, int iRise,
										   Bay *pBayNeighbor, SideOfBayEnum eSideOwned )
{
	int				j;
	bool			bMatchFound;
	Lift			*pLiftNeighbor;
	double			dRL;
	Component		*pCompMatch;
	LiftRiseEnum	eRise;

	;//assert( pComponent->GetLiftPointer()!=NULL );
	if( pComponent->GetLiftPointer()->HasComponentOfTypeOnSide( eTypeMatch, eSideOwned ) )
	{
		//we already own this component, so it will have the correct stage & level!
		return;
	}

	if( pComponent->GetType()==eType &&
		pComponent->GetSideOfBay()==eSide &&
		pComponent->GetRise()==iRise )
	{
		if( pBayNeighbor!=NULL )
		{
			//Find the component pointer for the eastern tranny of the previous bay
			dRL = pComponent->GetRL();
			pLiftNeighbor = pBayNeighbor->GetLiftAtRL( dRL );
			if( pLiftNeighbor!=NULL )
			{
				eRise = GetRiseEnumFromRise( dRL-pLiftNeighbor->GetRL() );
				pCompMatch = pLiftNeighbor->GetComponent( eTypeMatch, eRise, eSideMatch );

				if( pCompMatch!=NULL && !pCompMatch->IsCommitted() )
				{
					//We know the component exist, is it in the list of selected components?
					bMatchFound = false;
					//find a matching tranny
					for( j=0; j<pCompList->GetSize(); j++ )
					{
						if( pCompList->GetAt(j)==pCompMatch )
						{
							bMatchFound = true;
							break;
						}
					}

					if( !bMatchFound )
					{
						//Store the component stage and level for next time!
						pCompMatch->SetStage( pComponent->GetStage() );	
						pCompMatch->SetLevel( pComponent->GetLevel() );
						//Add the moved Component to the pSummaryElement component list
						pSummaryElement->AddComponent( pCompMatch );
						m_CompsChanged.Add(pCompMatch);
						pCompList->Add(pCompMatch);
					}
				}
			}
		}
	}
}

void BOMSummaryDialog::ReStageStandards(Component *pComponent, SummaryElement *pSummaryElement,
						CompArray *pCompList, Bay *pBayNeighbor, SideOfBayEnum eSideStandard1,
						SideOfBayEnum eSideRail1, SideOfBayEnum eSideRail2,
						SideOfBayEnum eSideLedger)
{
	int			j;
	bool		bMatchFound;
	double		dRL;
	Component	*pCompMatch;

	//////////////////////////////////////////////////////////////////////
	//Ensure we have a Supporting standards on the NE
	if( ( pComponent->GetType()==CT_RAIL	&& pComponent->GetSideOfBay()==eSideRail1  && pComponent->GetRise()==LIFT_RISE_1000MM ) ||
		( pComponent->GetType()==CT_RAIL	&& pComponent->GetSideOfBay()==eSideRail2  && pComponent->GetRise()==LIFT_RISE_1000MM ) ||
		( pComponent->GetType()==CT_LEDGER	&& pComponent->GetSideOfBay()==eSideLedger && pComponent->GetRise()==LIFT_RISE_0000MM ) )
	{
		if( pBayNeighbor!=NULL )
		{
			//Find the component pointer for the eastern tranny of the previous bay
			dRL	= pComponent->GetRL();
			pCompMatch = pBayNeighbor->GetStandardAtRL( dRL, SideOfBayAsCorner(eSideStandard1) );

			if( pCompMatch!=NULL && !pCompMatch->IsCommitted() &&
				pCompMatch->GetStage()!=pComponent->GetStage() )
			{
				//We know the component exist, is it in the list of selected components?
				bMatchFound = false;
				//find a matching tranny
				for( j=0; j<pCompList->GetSize(); j++ )
				{
					if( pCompList->GetAt(j)==pCompMatch )
					{
						bMatchFound = true;
						break;
					}
				}

				if( !bMatchFound )
				{
					//Store the component stage and level for next time!
					pCompMatch->SetStage( pComponent->GetStage() );	
					pCompMatch->SetLevel( pComponent->GetLevel() );
					//Add the moved Component to the pSummaryElement component list
					pSummaryElement->AddComponent( pCompMatch );
					m_CompsChanged.Add(pCompMatch);
				}
			}

			if( dRL>=pBayNeighbor->GetStandardPosition( SideOfBayAsCorner(eSideStandard1) ).z-ROUND_ERROR &&
				dRL<=pBayNeighbor->GetStandardPosition( SideOfBayAsCorner(eSideStandard1) ).z+ROUND_ERROR)
			{
				//This is the bottom standard, we need the jack and soleboard
				pCompMatch=pBayNeighbor->GetComponent( CT_JACK, eSideStandard1, 0.00 );
				if( pCompMatch!=NULL && !pCompMatch->IsCommitted() &&
					pCompMatch->GetStage()!=pComponent->GetStage() )
				{
					//We know the component exist, is it in the list of selected components?
					bMatchFound = false;
					//find a matching tranny
					for( j=0; j<pCompList->GetSize(); j++ )
					{
						if( pCompList->GetAt(j)==pCompMatch )
						{
							bMatchFound = true;
							break;
						}
					}

					if( !bMatchFound )
					{
						//Store the component stage and level for next time!
						pCompMatch->SetStage( pComponent->GetStage() );	
						pCompMatch->SetLevel( pComponent->GetLevel() );
						//Add the moved Component to the pSummaryElement component list
						pSummaryElement->AddComponent( pCompMatch );
						m_CompsChanged.Add(pCompMatch);
					}
				}
				//This is the bottom standard, we need the jack and soleboard
				pCompMatch=pBayNeighbor->GetComponent( CT_SOLEBOARD, eSideStandard1, 0.00 );
				if( pCompMatch!=NULL && !pCompMatch->IsCommitted() &&
					pCompMatch->GetStage()!=pComponent->GetStage() )
				{
					//We know the component exist, is it in the list of selected components?
					bMatchFound = false;
					//find a matching tranny
					for( j=0; j<pCompList->GetSize(); j++ )
					{
						if( pCompList->GetAt(j)==pCompMatch )
						{
							bMatchFound = true;
							break;
						}
					}

					if( !bMatchFound )
					{
						//Store the component stage and level for next time!
						pCompMatch->SetStage( pComponent->GetStage() );	
						pCompMatch->SetLevel( pComponent->GetLevel() );
						//Add the moved Component to the pSummaryElement component list
						pSummaryElement->AddComponent( pCompMatch );
						m_CompsChanged.Add(pCompMatch);
					}
				}
			}
		}
	}
}


void BOMSummaryDialog::OnSaveReport() 
{
	CStdioFile File;

	if( GetFile( File ) )
	{
		CString sLine, sTemp;

		//find the user name
		gpController->GetUsersName( sTemp );
		sLine.Format( _T("!User ID, %s"), sTemp );
		File.WriteString( sLine );

		//find the Draftsman
		sTemp = gpController->GetJobDescription()->m_sDraftsmanName;
		sLine.Format( _T("\n!Draftsman, %s"), sTemp );
		File.WriteString( sLine );

		//find the marketing number
		sTemp = gpController->GetJobDescription()->m_sMarketingEnquiryNumber;
    sLine.Format( _T("\n!M. E. #, %s"), sTemp );
		File.WriteString( sLine );

		//find the job number
		sTemp = gpController->GetJobDescription()->m_sJobTitle;
		sLine.Format( _T("\n!Job Title, %s"), sTemp );
		File.WriteString( sLine );

		//find the job desc
		sTemp = gpController->GetJobDescription()->m_sJobDescription;
		sLine.Format( _T("\n!Job Desc, %s"), sTemp );
		File.WriteString( sLine );

		//find the Client name
		sTemp = gpController->GetJobDescription()->m_sClientName;
		sLine.Format( _T("\n!Client Name, %s"), sTemp );
		File.WriteString( sLine );

		//find the Database filename
		gpController->GetDatabaseFilename( sTemp );
		sLine.Format( _T("\n!DWG Filename, %s"), sTemp );
		File.WriteString( sLine );

		//find the Kwikscaf version
		sLine.Format( _T("\n!Kwikscaf version, %s"), MECCANO_LATEST_VERSION );
		File.WriteString( sLine );

		//find the current date
		COleDateTime date;
		date = COleDateTime::GetCurrentTime();
    sLine.Format( _T("\n!File created, %i/%i/%i, %i:%i"),
				date.GetDay(), date.GetMonth(), date.GetYear(),
				date.GetHour(), date.GetMinute() );
		File.WriteString( sLine );

		//find the CSV filename
		sLine.Format( _T("\n!Filename, %s"), File.GetFilePath() );
		File.WriteString( sLine );

		//Column labels
		sLine = _T("\n!\n!\n!Part Number, Quantity, Area(mm^2), Description, Hire*($), Sale*($), Weight(kg)") ;
		File.WriteString( sLine );

		//Data
		for( int i=0; i<m_saCSVData.GetSize(); i++ )
		{
			sLine = m_saCSVData.GetAt(i);
			File.WriteString( sLine );
		}

		sLine = _T("\n\n\n!* Prices shown are LIST PRICE ONLY and do not include GST nor Stamp Duty!");
		File.WriteString( sLine );
		
		CString sMsg;
		sMsg.Format( _T("File '%s' successfully written"), File.GetFileName() );
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox( sMsg, _T("BOM Summary CSV file"), MB_OK );
	}
	else
	{
		MessageBeep(MB_ICONSTOP);
	}
	File.Close();
}

bool BOMSummaryDialog::GetFile(CStdioFile &File)
{
	UINT					uiMode;
	CString					sFilename;

	uiMode = CFile::modeWrite|CFile::typeText|CFile::modeCreate;

	//read the filename from the registry
	sFilename = _T("KwikScaf BOM Summary File");

	///////////////////////////////////////////////////
	//Get the file name to use from the user
	CFileDialog dbox(FALSE, EXTENTION_SAVE_BOM_SUMMARY_FILE, sFilename,
			OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN,
			FILTER_SAVE_BOM_SUMMARY_FILE );
	dbox.m_ofn.lpstrTitle = TITLE_SAVE_BOM_SUMMARY_FILE;
	//gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
	if(dbox.DoModal()!=IDOK)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		gbOpenFileDialogActive = false;
		return false;
	}
	gbOpenFileDialogActive = false; 

	sFilename = dbox.GetPathName();
	CString sTemp, sExt;
	sTemp = sFilename.Right(1);
	if( sTemp==_T('.') )
		sFilename = sFilename.Left( sFilename.GetLength()-1 );

	sExt = EXTENTION_SAVE_BOM_SUMMARY_FILE;
	sExt.MakeUpper();
	sTemp = sFilename.Right(sExt.GetLength());
	sTemp.MakeUpper();
	if( sTemp!=sExt )
		sFilename+= EXTENTION_SAVE_BOM_SUMMARY_FILE;

	//open it this time
	CFileException Error;
	if( !File.Open( sFilename, uiMode, &Error ) )
	{
		TCHAR   szCause[255];
		CString strFormatted;
		CString sMessage;

		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
		sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;
		sMessage+= szCause;
		MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

		return GetFile(File);
	}
	return true;
}

void BOMSummaryDialog::DisplayList(BOMExtraArray *pBOMArray, int &iListIndex, BOMShowType show)
{
	int					iIndex, iBOMIndex, iArea;
	bool				bSaleInHireColumn;
	double				dTotQnty, dTotHire, dTotSale, dTotWght;
	double				dQuantity, dArea, dRounding;
	double				dHire, dSale, dWeight;
	CString				sDescriptionTest, sQuantity, sHire, sSale, sWeight, sLine, sPartNumber;
	BOMExtraElement		*pBOMComponent;
	StockListElement	*pStockListElement;

	dTotQnty	= 0;
	dTotHire	= 0;
	dTotSale	= 0;
	dTotWght	= 0;

	bSaleInHireColumn = false;
	if( show==BOMSHOW_SALE_ONLY )
		bSaleInHireColumn = true;

	for (iIndex = 0; iIndex < pBOMArray->GetSize(); iIndex++)
	{
		pBOMComponent = pBOMArray->GetAt(iIndex);
		if (pBOMComponent)
		{
			sPartNumber = pBOMComponent->m_sPartNumber;
			pStockListElement = m_pStockListArray->GetMatchingComponent( sPartNumber );
			if (pStockListElement)
			{
				dQuantity	= pBOMComponent->m_iQuantity;
				dArea		= pBOMComponent->m_dArea;

				if( dArea>ROUND_ERROR_SMALL )
				{
					if( ( gpController->GetPartNumberChainMesh()==sPartNumber && 
						  gpController->CalcPerRollChainMesh() ) ||
						( gpController->GetPartNumberShadeCloth()==sPartNumber && 
						  gpController->CalcPerRollShadeCloth() ) )
					{
						if( gpController->GetPartNumberChainMesh()==sPartNumber && 
							gpController->CalcPerRollChainMesh() )
						{
							;//assert( gpController->GetRollSizeChainMesh()>ROUND_ERROR );
							dArea = dArea/gpController->GetRollSizeChainMesh();
							dRounding = (double)gpController->GetRoundingDivisorChainMesh();
						}
						else if( gpController->GetPartNumberShadeCloth()==sPartNumber && 
							  gpController->CalcPerRollShadeCloth() )
						{
							;//assert( gpController->GetRollSizeShadeCloth()>ROUND_ERROR );
							dArea = dArea/gpController->GetRollSizeShadeCloth();
							dRounding = (double)gpController->GetRoundingDivisorShadeCloth();
						}
						else
						{
							;//assert( false );
						}
						dArea = dArea*dRounding;
						iArea = (int)dArea;
						dArea-= (double)iArea;
						if( dArea>ROUND_ERROR_SMALL )
							iArea++;
						dArea = (double)iArea/dRounding;
						pBOMComponent->m_dArea = dArea;
					}

					dHire	= dArea * pStockListElement->GetHire();
					dSale	= dArea * pStockListElement->GetSale();
					dWeight	= dArea * pStockListElement->GetWeight();
				}
				else
				{
					//only include the item in the total quantity if it has no area!
					dTotQnty	+= dQuantity;
					dHire		= dQuantity * pStockListElement->GetHire();
					dSale		= dQuantity * pStockListElement->GetSale();
					dWeight		= dQuantity * pStockListElement->GetWeight();
				}
				//We need to round off to the nearest cent
				sHire.Format(_T("%.2f"),	(dHire+0.005) );
				sSale.Format(_T("%.2f"),	(dSale+0.005) );
				//We need to round off to the nearest 100th of a kg
				sWeight.Format(_T("%.2f"),	(dWeight+0.005) );

				iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iListIndex, pBOMComponent->GetBOMPNumDescript());
				sDescriptionTest.Format(_T("%s"), pStockListElement->GetDescription());
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sDescriptionTest); // for debug
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_QNTY, pBOMComponent->GetBOMQuantityDescript());
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_WGHT, sWeight);
				;//assert( gpController!=NULL );

				if(	bSaleInHireColumn )
				{
					m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sSale);
				}
				else
				{
					m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sHire);
					m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_SALE, sSale);
				}

				// add to totals
				dTotWght	+= dWeight;
				if( show==BOMSHOW_SALE_ONLY && 
						( dHire<ROUND_ERROR_SMALL && dHire>(-1.00*ROUND_ERROR_SMALL) ) &&
						( dSale>ROUND_ERROR_SMALL || dSale<(-1.00*ROUND_ERROR_SMALL) ) )
				{
					//We are only showing sale prices and this only has a sale price
					dTotSale		+= dSale;
				}
				else
				{
					dTotHire		+= dHire;
					if( gpController->IsDisplaySalePrice() )
						dTotSale		+= dSale;
				}

				sLine.Format( _T("\n%s, %1.0f, %1.1f, %s, %1.02f, %1.02f, %1.1f"),
						sPartNumber, dQuantity, dArea, sDescriptionTest,
						dHire, dSale, dWeight );
			}
			else
			{
				CString sError, sTemp;
				//sError.Format("Unknown Stock Item (Part Number: %s)", sPartNumber);
				sError.Format(_T("Unknown Component(%s)"), pBOMComponent->GetCompDetailsDescription() );
				sTemp.Format( _T("Unknown(%s)"), sPartNumber );
				iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iListIndex, sTemp);
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, sError);
			}
		}

		m_saCSVData.Add( sLine );
		++iListIndex;
	}
	// show totals

	if( dTotQnty>ROUND_ERROR || dTotWght>ROUND_ERROR_SMALL ||
		dTotHire>ROUND_ERROR_SMALL || dTotSale>ROUND_ERROR_SMALL )
	{
		//store the totals
		m_daHireTotals.Add(dTotHire);
		m_daSaleTotals.Add(dTotSale);

		if( show==BOMSHOW_SALE_ONLY )
		{
			m_daSaleQuantityTotals.Add(dTotQnty);
			m_daSaleWeightTotals.Add(dTotWght);
		}
		else
		{
			m_daQuantityTotals.Add(dTotQnty);
			m_daWeightTotals.Add(dTotWght);
		}

		//display totals
		//sQuantity.Format("%.0f items", dTotQnty);
		//NOTE 20001031 - don't show total for quantity
		sQuantity = _T("");
		sHire.Format(_T("$%.2f"), dTotHire);
		sSale.Format(_T("$%.2f"), dTotSale);
		sWeight.Format(_T("%.2f Kg"), dTotWght);

		// add totals to listbox
		iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iListIndex, _T(" "));
		switch( show )
		{
		case( BOMSHOW_SALE_ONLY ):
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Total Sale")); // for debug
			break;
		case( BOMSHOW_HIRE_ONLY ):
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Total Hire")); // for debug
			break;
		case( BOMSHOW_SALE_AND_HIRE ):
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Totals")); // for debug
			break;
		default:
			;//assert( false );
		}

		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_QNTY, sQuantity);
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_WGHT, sWeight);
		if( show==BOMSHOW_SALE_ONLY )
		{
			//put the sale total in the hire column
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sSale);
		}
		else
		{
			m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sHire);
			if( gpController->IsDisplaySalePrice() )
				m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_SALE, sSale);
		}
		++iListIndex;
	}
}

void BOMSummaryDialog::DispalyGrandTotals(int &iListIndex)
{
	int iBOMIndex;
	CString sQuantity, sHire, sSale, sWeight;

	//sQuantity.Format("%.0f items", m_daQuantityTotals.GetTotal() );
	//NOTE 20001031 - don't show total for quantity
	sQuantity = _T("");
	sHire.Format(_T("$%.2f"), m_daHireTotals.GetTotal());
	sSale.Format(_T("$%.2f"), m_daSaleTotals.GetTotal());
	sWeight.Format(_T("%.2f Kg"), m_daWeightTotals.GetTotal());

	// insert blank line
	m_BOMSummaryNewListCtrl.InsertItem(iListIndex, _T(" "));
	++iListIndex;

	iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iListIndex, _T(" "));
	if( gpController->IsDisplaySalePrice() )
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Grand Total")); // for debug
	else
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Grand Total Hire")); // for debug
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_QNTY, sQuantity);
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_WGHT, sWeight);
	m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sHire);
	;//assert( gpController!=NULL );
	++iListIndex;
	if( gpController->IsDisplaySalePrice() )
	{
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_SALE, sSale);
	}
	else if( m_daSaleTotals.GetTotal()>ROUND_ERROR_SMALL )
	{
		//sQuantity.Format("%.0f items", m_daSaleQuantityTotals.GetTotal() );
		//NOTE 20001031 - don't show total for quantity
		sQuantity = _T("");
		sWeight.Format(_T("%.2f Kg"), m_daSaleWeightTotals.GetTotal());

		iBOMIndex = m_BOMSummaryNewListCtrl.InsertItem(iListIndex, _T(" "));
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_DESC, _T("Grand Total Sale")); // for debug
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_QNTY, sQuantity);
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_WGHT, sWeight);
		m_BOMSummaryNewListCtrl.SetItemText(iBOMIndex, COLUMN_HIRE, sSale);
	}
	++iListIndex;
}

bool BOMSummaryDialog::SeperateOutSaleOnlyItems(BOMExtraArray *pOriginal, BOMExtraArray *pSaleOnly)
{
	int					i;
	bool				bSwapEle, bSwapDone;
	CString				sPartNumber;
	BOMExtraElement		*pEle;
	StockListElement	*pStockListElement;

	bSwapDone = false;
	for( i=0; i<pOriginal->GetSize(); i++ )
	{
		pEle = pOriginal->GetAt(i);

		bSwapEle = false;
		if( pEle!=NULL )
		{
			sPartNumber = pEle->m_sPartNumber;
			pStockListElement = m_pStockListArray->GetMatchingComponent(sPartNumber);
			if(pStockListElement!=NULL)
			{
				if( pStockListElement->GetHire()<ROUND_ERROR_SMALL &&
					pStockListElement->GetSale()>ROUND_ERROR_SMALL )
				{
					//the hire price is negligable, the sale price is usable
					bSwapEle = true;
				}
				else if( pStockListElement->UseSalePrice() )
				{
					//This component is marked for sale
					bSwapEle = true;
				}
			}
		}

		if( bSwapEle )
		{
			bSwapDone = true;
			pOriginal->RemoveAt(i);
			pSaleOnly->Add(pEle);
			i--;
		}
	}
	return bSwapDone;
}


void BOMSummaryDialog::OnLvnItemchangedBomSummaryListctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

int BOMSummaryDialog::GetCurrentNumberOfDecks(Bay *pBay)
{
	int iNumberOfLifts;
	CString sLiftSelection;
	Lift *pLift;
	int iNumberOfDecks;

	iNumberOfDecks = 0;
	// fill selection list combo
	iNumberOfLifts = pBay->GetNumberOfLifts();
	for ( int index = 0; index < iNumberOfLifts; ++index )
	{
		pLift = pBay->GetLift(index);
		if( pLift->IsDeckingLift() )
		{			
				++iNumberOfDecks;
		}
	}

	return iNumberOfDecks;
}

int BOMSummaryDialog::GetCurrentNumberOfDecks(Bay *pBay,int dLower,int dUpper,double &rise)
{
	int iNumberOfLifts;
	CString sLiftSelection;
	Lift *pLift;
	int iNumberOfDecks;

	iNumberOfDecks = 0;
	// fill selection list combo
	iNumberOfLifts = pBay->GetNumberOfLifts();
	for ( int index = 0; index < iNumberOfLifts; ++index )
	{
		pLift = pBay->GetLift(index);
		if( pLift->IsDeckingLift() )
		{
			double rl=pLift->GetRL();
			if(rl>=dLower && rl <= dUpper)
			{
				++iNumberOfDecks;
				rise = 500.0*(int)pLift->GetRise();
			}
		}
	}

	return iNumberOfDecks;
}

