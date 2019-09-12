// EditStandardsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "EditStandardsDlg.h"
#include "Matrix.h"
#include "MatrixElement.h"
#include "Controller.h"
#include "doubleArray.h"
#include "StandardHeightPickerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EditStandardsDlg dialog

//#define	SAME_ONLY_IF_HEIGHTS_MATCH

enum COLUMNS_MATRIX
{
	COLUMN_MATRIX_LABEL,
	COLUMN_MATRIX_OUTER_STD,
	COLUMN_MATRIX_INNER_STD
};

enum COLUMNS_BAYS
{
	COLUMN_BAYS_NUMBER,
	COLUMN_BAYS_SIDE
};

//#define SHOW_HEIGHTS_OF_STANDARDS


EditStandardsDlg::EditStandardsDlg( Controller *pController, CWnd* pParent /*=NULL*/)
	: KwikscafDialog(EditStandardsDlg::IDD, pParent)
{
	;//assert( pController!=NULL );
	m_pController = pController;
	m_pMatrix = m_pController->GetMatrix();
	;//assert( m_pMatrix!=NULL );
	ApplyRequired(false);
	m_bExpandedView = true;
	m_daStandardsInner.RemoveAll();
	m_iaOpenEndedInner.RemoveAll();
	m_daStandardsOuter.RemoveAll();
	m_iaOpenEndedOuter.RemoveAll();
	m_eSystem = S_INVALID;

	//{{AFX_DATA_INIT(EditStandardsDlg)
	m_sHeightInner = _T("");
	m_sHeightOuter = _T("");
	m_bStaggered = FALSE;
	m_iStandardsFit = -1;
	m_bSBInner = FALSE;
	m_bSBOuter = FALSE;
	//}}AFX_DATA_INIT
}


void EditStandardsDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditStandardsDlg)
	DDX_Control(pDX, IDC_APPLY, m_btnApply);
	DDX_Control(pDX, IDC_BAYS, m_lstctrlBays);
	DDX_Control(pDX, IDC_STANDARDS_OUTER, m_lstboxStandardsOuter);
	DDX_Control(pDX, IDC_STANDARDS_INNER, m_lstboxStandardsInner);
	DDX_Control(pDX, IDC_MATRIX_SECTIONS_LIST, m_lstctrlMatrixSelection);
	DDX_Text(pDX, IDC_HEIGHT_INNER, m_sHeightInner);
	DDX_Text(pDX, IDC_HEIGHT_OUTER, m_sHeightOuter);
	DDX_Check(pDX, IDC_STAGGERED_HEIGHTS_CHECK, m_bStaggered);
	DDX_Radio(pDX, IDC_COURSE_FIT_RADIO, m_iStandardsFit);
	DDX_Check(pDX, IDC_SOLEBOARD_INNER, m_bSBInner);
	DDX_Check(pDX, IDC_SOLEBOARD_OUTER, m_bSBOuter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditStandardsDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(EditStandardsDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_COURSE_FIT_RADIO, OnCourseFitRadio)
	ON_BN_CLICKED(IDC_FINE_FIT_RADIO, OnFineFitRadio)
	ON_BN_CLICKED(IDC_SAME_FIT_RADIO, OnSameFitRadio)
	ON_BN_CLICKED(IDC_SELECT_SIMILAR, OnSelectSimilar)
	ON_BN_CLICKED(IDC_STAGGERED_HEIGHTS_CHECK, OnStaggeredHeightsCheck)
	ON_LBN_DBLCLK(IDC_STANDARDS_OUTER, OnDblclkStandardsOuter)
	ON_LBN_DBLCLK(IDC_STANDARDS_INNER, OnDblclkStandardsInner)
	ON_NOTIFY(NM_CLICK, IDC_MATRIX_SECTIONS_LIST, OnClickMatrixSectionsList)
	ON_BN_CLICKED(IDC_SOLEBOARD_INNER, OnSoleboardInner)
	ON_BN_CLICKED(IDC_SOLEBOARD_OUTER, OnSoleboardOuter)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditStandardsDlg message handlers

#include "actrans.h"

void EditStandardsDlg::OnApply() 
{
	int				i, iIndex, iBayNumber;
	Bay				*pBay;
	bool			bEast, bAllOK, bStdIn, bStdOut, bValid;
	DWORD			dwData;
	CString			sInvalidBays, sTemp, sNumber;
	POSITION		pos;
	doubleArray		daInner, daStarInner, daOuter, daStarOuter, daStdIn, daStdOut;

	UpdateData( );

	/////////////////////////////////////////////////////////////////////////
	//Since FillStandards will clobber the m_daStandardsInner, etc I will store them now!
	bStdIn		= ConvertBOOLTobool(m_bSBInner);
	bStdOut		= ConvertBOOLTobool(m_bSBOuter);
	daStdIn		= m_daStandardsInner;
	daStdOut	= m_daStandardsOuter;

	/////////////////////////////////////////////////////////////////////////
	//We must have some bays to apply the changes to!
	pos = m_lstctrlBays.GetFirstSelectedItemPosition();
	if( pos<=0 )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		ApplyRequired(false);
	}

	daInner = m_daStandardsInner;
	daOuter = m_daStandardsOuter;
	daInner.ReverseOrder();
	daOuter.ReverseOrder();

	daStarInner.RemoveAll();
	for( i=0; i<daInner.GetSize(); i++ )
	{
		daStarInner.Add(ConvertRLtoStarRL(daInner.GetAt(i), m_dStarSeparation ));
	}
	daStarOuter.RemoveAll();
	for( i=0; i<daOuter.GetSize(); i++ )
	{
		daStarOuter.Add(ConvertRLtoStarRL(daOuter.GetAt(i), m_dStarSeparation));
	}
	

	/////////////////////////////////////////////////////////////////////////
	//Is the new arrangement tall enough?
	double			dHeightOut, dHeightIn;
	CString			sMatrixSection;
	CStringArray	saShortMatrixSections;

	saShortMatrixSections.RemoveAll();

	/////////////////////////////////////////////////////////////////////////
	//Check the new standard height is tall enough to support the components
#ifdef SHOW_HEIGHTS_OF_STANDARDS
	pos			= m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();
	doubleArray		daShortHeights;
	daShortHeights.RemoveAll();
	while (pos) // go through each selected matrix
	{
		iIndex		= m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
		sNumber		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_OUTER_STD );
		dHeightOut	= (_tstof( sNumber )*CONVERT_M_TO_MM);
		sNumber		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_INNER_STD );
		dHeightIn	= (_tstof( sNumber )*CONVERT_M_TO_MM);
		if( (daInner.GetSum()+ROUND_ERROR)<dHeightIn || (daOuter.GetSum()+ROUND_ERROR)<dHeightOut )
		{
			sMatrixSection		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_LABEL );
			saShortMatrixSections.Add( sMatrixSection );
			daShortHeights.Add( dHeightIn );
			daShortHeights.Add( dHeightOut );
		}
	}
	if( saShortMatrixSections.GetSize()>0 )
	{
		CString sMsg, sTemp;
		sMsg.Format( _T("The new standard arrangement (%0.1fm,%0.1fm) you wish to\napply "),
						daInner.GetSum()*CONVERT_MM_TO_M, daOuter.GetSum()*CONVERT_MM_TO_M );
		if( saShortMatrixSections.GetSize()==1 )
		{
			sTemp.Format( _T("to matrix section %s(%0.1fm,%0.1fm) is\n"), saShortMatrixSections.GetAt(0),
						daShortHeights.GetAt(0)*CONVERT_MM_TO_M, daShortHeights.GetAt(1)*CONVERT_MM_TO_M );
			sMsg+= sTemp;
		}
		else
		{
			sMsg+= _T("to matrix sections ");
			sTemp.Format( _T("%s(%0.1fm,%0.1fm)"), saShortMatrixSections.GetAt(0),
						daShortHeights.GetAt(0)*CONVERT_MM_TO_M, daShortHeights.GetAt(1)*CONVERT_MM_TO_M);
			sMsg+= sTemp;
			for( i=1; i<saShortMatrixSections.GetSize()-1; i++ )
			{
				sTemp.Format( _T(", %s(%0.1fm,%0.1fm)"), saShortMatrixSections.GetAt(i),
							daShortHeights.GetAt(i*2)*CONVERT_MM_TO_M, daShortHeights.GetAt((i*2)+1)*CONVERT_MM_TO_M );
				sMsg+= sTemp;
			}
			sTemp.Format( _T(" and %s(%0.1fm,%0.1fm) are\n"), saShortMatrixSections.GetAt(i),
							daShortHeights.GetAt(i*2)*CONVERT_MM_TO_M, daShortHeights.GetAt((i*2)+1)*CONVERT_MM_TO_M );
			sMsg+= sTemp;


		}
		sMsg+= _T("shorter than the existing standards.\n\n");
		sMsg+= _T("Is this OK?");

		MessageBeep(MB_ICONWARNING);
		if( MessageBox( sMsg, _T("Apply changes warning"), MB_OKCANCEL|MB_ICONWARNING )!=IDOK )
			return;
	}
#else	//#ifdef SHOW_HEIGHTS_OF_STANDARDS
	pos			= m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();
	while (pos) // go through each selected matrix
	{
		iIndex		= m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
		sNumber		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_OUTER_STD );
		dHeightOut	= (_tstof( sNumber )*CONVERT_M_TO_MM);
		sNumber		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_INNER_STD );
		dHeightIn	= (_tstof( sNumber )*CONVERT_M_TO_MM);
		if( (daInner.GetSum()+ROUND_ERROR)<dHeightIn || (daOuter.GetSum()+ROUND_ERROR)<dHeightOut )
		{
			sMatrixSection		= m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_LABEL );
			saShortMatrixSections.Add( sMatrixSection );
		}
	}
	if( saShortMatrixSections.GetSize()>0 )
	{
		CString sMsg, sTemp;
		sMsg = _T("The new standard arrangement you wish to use is\n");
		sMsg+= _T("shorter then the existing standards for matrix\n");
		if( saShortMatrixSections.GetSize()==1 )
		{
			sTemp.Format( _T("section %s!"), saShortMatrixSections.GetAt(0) );
		}
		else
		{
			sTemp.Format( _T("sections %s"), saShortMatrixSections.GetAt(0) );
			sMsg+= sTemp;
			for( i=1; i<saShortMatrixSections.GetSize()-1; i++ )
			{
				sTemp.Format( _T(", %s"), saShortMatrixSections.GetAt(i) );
				sMsg+= sTemp;
			}
			sTemp.Format( _T(" and %s!"), saShortMatrixSections.GetAt(i) );
		}
		sMsg+= sTemp;
		sMsg+= _T("\n\nIs this OK?");

		MessageBeep(MB_ICONWARNING);
    if( MessageBox( sMsg, _T("WARNING: Short Arrangement"), MB_OKCANCEL|MB_ICONWARNING )!=IDOK )
			return;
	}
#endif //#ifdef SHOW_HEIGHTS_OF_STANDARDS

	/////////////////////////////////////////////////////////////////////////
	//First lets check that we are not clobbering committed standards
	sInvalidBays = _T("The following bays contain committed standards\n");
  sInvalidBays+= _T("which would be affected by this change:\n");
	bAllOK = true;
	pos = m_lstctrlBays.GetFirstSelectedItemPosition();
	while (pos) // go through each selected bay
	{
		iIndex	= m_lstctrlBays.GetNextSelectedItem(pos);
		dwData	= (DWORD)m_lstctrlBays.GetItemData( iIndex );
		bEast	= dwData&0x1;
		dwData	= dwData>>1;
		iBayNumber	= (int)dwData;

		pBay = m_pController->GetBayFromBayNumber(iBayNumber);
		;//assert( pBay!=NULL );
		if( bEast )
		{
			bValid = TestCommitted( daStarInner, CNR_SOUTH_EAST, pBay ); 
			if(bValid) 
				bValid = TestCommitted( daStarOuter, CNR_NORTH_EAST, pBay ); 
		}
		else
		{
			bValid = TestCommitted( daStarInner, CNR_SOUTH_WEST, pBay ); 
			if(bValid) 
				bValid = TestCommitted( daStarOuter, CNR_NORTH_WEST, pBay ); 
		}
		if(!bValid)
		{
			//This bay cannot be edited!
      sTemp.Format( _T("Bay %i: %s\n"), pBay->GetBayNumber(), bEast? _T("East"): _T("West") );
			sInvalidBays+= sTemp;
			//Unselect the committed bay
			m_lstctrlBays.SetItemState( iIndex, 0, LVIS_SELECTED );
			bAllOK = false;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	//Are there any bays left to edit?
	if( m_lstctrlBays.GetFirstSelectedItemPosition()<=0 )
	{
		sInvalidBays = _T("All the selected bays have standards that are currently\n");
		sInvalidBays+= _T("committed and would be affected by these changes\n");
		sInvalidBays+= _T("\nTherefore these changes cannot be applied!");
		MessageBox( sInvalidBays, _T("Apply changes"), MB_OK|MB_ICONSTOP );
		ApplyRequired(false);
		return;
	}

	/////////////////////////////////////////////////////////////////////////
	//Did we have any that we were not able to change?
	if( !bAllOK )
	{
		sInvalidBays+= _T("\nThese changed cannot be applied to these bays!\n");
		sInvalidBays+= _T("Would you like to continue and apply the changes\n");
		sInvalidBays+= _T("to the remaining valid bays?");
		if( MessageBox( sInvalidBays, _T("Continue"), MB_YESNO|MB_ICONQUESTION )==IDNO )
			return;
	}

	intArray		iaCommitted, iaLevels;
	doubleArray		daRLs;
	CStringArray	saStages;

	/////////////////////////////////////////////////////////////////////
	//Lets do the changes
	pos = m_lstctrlBays.GetFirstSelectedItemPosition();
	while (pos) // go through each selection
	{
		iIndex	= m_lstctrlBays.GetNextSelectedItem(pos);
		dwData	= (DWORD)m_lstctrlBays.GetItemData( iIndex );
		bEast	= dwData&0x1;
		dwData	= dwData>>1;
		iBayNumber	= (int)dwData;

		pBay = m_pController->GetBayFromBayNumber(iBayNumber);

		;//assert( pBay!=NULL );

		if( bEast )
		{
			GetCommittedStageLevel( pBay, CNR_SOUTH_EAST, iaCommitted, iaLevels, daRLs, saStages );
			pBay->SetStandardsArrangement( daStarInner, CNR_SOUTH_EAST, ConvertBOOLTobool(m_bSBInner) );
			SetCommittedStageLevel( pBay, CNR_SOUTH_EAST, iaCommitted, iaLevels, daRLs, saStages );

			GetCommittedStageLevel( pBay, CNR_NORTH_EAST, iaCommitted, iaLevels, daRLs, saStages );
			pBay->SetStandardsArrangement( daStarOuter, CNR_NORTH_EAST, ConvertBOOLTobool(m_bSBOuter) );
			SetCommittedStageLevel( pBay, CNR_NORTH_EAST, iaCommitted, iaLevels, daRLs, saStages );
		}
		else
		{
			GetCommittedStageLevel( pBay, CNR_SOUTH_WEST, iaCommitted, iaLevels, daRLs, saStages );
			pBay->SetStandardsArrangement( daStarInner, CNR_SOUTH_WEST, ConvertBOOLTobool(m_bSBInner) );
			SetCommittedStageLevel( pBay, CNR_SOUTH_WEST, iaCommitted, iaLevels, daRLs, saStages );

			GetCommittedStageLevel( pBay, CNR_NORTH_WEST, iaCommitted, iaLevels, daRLs, saStages );
			pBay->SetStandardsArrangement( daStarOuter, CNR_NORTH_WEST, ConvertBOOLTobool(m_bSBOuter) );
			SetCommittedStageLevel( pBay, CNR_NORTH_WEST, iaCommitted, iaLevels, daRLs, saStages );
		}

		//Store the fit and staggered
		pBay->SetStandardsFit( (StandardFitEnum)m_iStandardsFit );
		pBay->SetStaggeredHeights( ConvertBOOLTobool(m_bStaggered) );
	}

	/////////////////////////////////////////////////////////////////////////
	//recreate the matrix
	m_pController->CreateMatrix( true );
	m_pMatrix = m_pController->GetMatrix();

	/////////////////////////////////////////////////////////////////////////
	//force redraw of the screen
	actrTransactionManager->queueForGraphicsFlush();
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();

	/////////////////////////////////////////////////////////////////////////
	//Update this dialog box
	FillMatrixListBox();
	SelectSimilar( daStdIn, daStdOut, bStdIn, bStdOut, m_eSystem );
	FillStandards();
	FillBaysList();
	ApplyRequired(false);

	SetViewportByMatrix();
}

void EditStandardsDlg::OnCourseFitRadio() 
{
	AdjustStandards();
}

void EditStandardsDlg::OnFineFitRadio() 
{
	AdjustStandards();
}

void EditStandardsDlg::OnSameFitRadio() 
{
	AdjustStandards();
}

void EditStandardsDlg::OnSelectSimilar() 
{
	MatrixElement		*pElement;

	pElement = GetFirstSelectedElement();

#ifdef SAME_ONLY_IF_HEIGHTS_MATCH
	int					i, iIndex;
	double				dStdInner, dStdOuter;
	dStdInner = pElement->GetStandardsHeight( MATRIX_STANDARDS_INNER );
	dStdOuter = pElement->GetStandardsHeight( MATRIX_STANDARDS_OUTER );
	pElement->ConvertRLArrangeToStdHeightArrange( dStdInner, true );
	pElement->ConvertRLArrangeToStdHeightArrange( dStdOuter, true );
	SelectSimilar( dStdInner, dStdOuter, pElement->GetSystem() );
#else 	//#ifdef SAME_ONLY_IF_HEIGHTS_MATCH
	bool				bSBInner, bSBOuter;
	doubleArray			daStdInner, daStdOuter;
	pElement->GetStandardsArrangement( daStdInner, bSBInner, MATRIX_STANDARDS_INNER );
	pElement->GetStandardsArrangement( daStdOuter, bSBOuter, MATRIX_STANDARDS_OUTER );
	pElement->ConvertRLArrangeToStdHeightArrange( daStdInner, true );
	pElement->ConvertRLArrangeToStdHeightArrange( daStdOuter, true );
	SelectSimilar( daStdInner, daStdOuter, bSBInner, bSBOuter, pElement->GetSystem() );
#endif SAME_ONLY_IF_HEIGHTS_MATCH
	FillBaysList();
}

void EditStandardsDlg::SelectSimilar(double dStdInner, double dStdOuter, SystemEnum eSystem )
{
	int					i, iIndex;
	double				dEleInner, dEleOuter;
	MatrixElement		*pElement;

	//go through each element in the matrix list and see if it matches the above specs
	for( i=0; i<m_lstctrlMatrixSelection.GetItemCount(); i++ )
	{
		iIndex = (int)m_lstctrlMatrixSelection.GetItemData( i );
		pElement = m_pMatrix->GetMatrixElement( iIndex );
		dEleInner = pElement->GetStandardsHeight( MATRIX_STANDARDS_INNER );
		dEleOuter = pElement->GetStandardsHeight( MATRIX_STANDARDS_OUTER );
		if( dEleInner>dStdInner-ROUND_ERROR && dEleInner<dStdInner+ROUND_ERROR &&
			dEleOuter>dStdOuter-ROUND_ERROR && dEleOuter<dStdOuter+ROUND_ERROR &&
			pElement->GetSystem()==eSystem )
		{
			m_lstctrlMatrixSelection.SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
		}
		else
		{
			m_lstctrlMatrixSelection.SetItemState( iIndex, 0, LVIS_SELECTED );
		}
	}
	m_lstctrlMatrixSelection.SetFocus();
}

void EditStandardsDlg::SelectSimilar(doubleArray &daStdInner, doubleArray &daStdOuter, bool bSBInner, bool bSBOuter, SystemEnum eSystem)
{
	int					i, iIndex;
	bool				bSBInnerTest, bSBOuterTest;
	doubleArray			daStdInnerTest, daStdOuterTest;
	MatrixElement		*pElement;

	//go through each element in the matrix list and see if it matches the above specs
	for( i=0; i<m_lstctrlMatrixSelection.GetItemCount(); i++ )
	{
		iIndex = (int)m_lstctrlMatrixSelection.GetItemData( i );
		pElement = m_pMatrix->GetMatrixElement( iIndex );

		pElement->GetStandardsArrangement( daStdInnerTest, bSBInnerTest, MATRIX_STANDARDS_INNER );
		pElement->GetStandardsArrangement( daStdOuterTest, bSBOuterTest, MATRIX_STANDARDS_OUTER );
		pElement->ConvertRLArrangeToStdHeightArrange( daStdInnerTest, true );
		pElement->ConvertRLArrangeToStdHeightArrange( daStdOuterTest, true );
		if( bSBInner==bSBInnerTest &&
			bSBOuter==bSBOuterTest &&
			daStdInner==daStdInnerTest &&
			daStdOuter==daStdOuterTest &&
			pElement->GetSystem()==eSystem )
		{
			m_lstctrlMatrixSelection.SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
		}
		else
		{
			m_lstctrlMatrixSelection.SetItemState( iIndex, 0, LVIS_SELECTED );
		}
	}
	m_lstctrlMatrixSelection.SetFocus();
}

void EditStandardsDlg::OnStaggeredHeightsCheck() 
{
	AdjustStandards();
}

void EditStandardsDlg::OnCancel() 
{
	KwikscafDialog::OnCancel();
}

BOOL EditStandardsDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("EditStandardsDlg"));

	SetupListControls();

	FillMatrixListBox();
	FillStandards();
	FillBaysList();

	m_lstctrlMatrixSelection.SetFocus();

	SetViewportByMatrix();

	ApplyRequired( IsApplyRequired() );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void EditStandardsDlg::SetupListControls()
{
	m_lstctrlMatrixSelection.InsertColumn( COLUMN_MATRIX_LABEL,		_T("Label"),	LVCFMT_LEFT,	39 );
	m_lstctrlMatrixSelection.InsertColumn( COLUMN_MATRIX_OUTER_STD,	_T("Outer(m)"),	LVCFMT_RIGHT,	52 );
	m_lstctrlMatrixSelection.InsertColumn( COLUMN_MATRIX_INNER_STD,	_T("Inner(m)"),	LVCFMT_RIGHT,	51 );
	m_lstctrlBays.InsertColumn( COLUMN_BAYS_NUMBER,	_T("Bay"),	LVCFMT_LEFT, 30 );
	m_lstctrlBays.InsertColumn( COLUMN_BAYS_SIDE,	_T("Side"),	LVCFMT_RIGHT, 40 );
}

void EditStandardsDlg::FillMatrixListBox()
{
	int				i, iSize, iListIndex;
	CString			sText;
	MatrixElement	*pElement;

	iSize = m_pMatrix->GetMatrixSize();

	m_lstctrlMatrixSelection.DeleteAllItems( );
	for( i=0; i<iSize; i++ )
	{
		pElement = m_pMatrix->GetMatrixElement(i);

		;//assert( pElement!=NULL );
		sText = pElement->GetLabel();
 		iListIndex = m_lstctrlMatrixSelection.InsertItem( i, sText );
		sText.Format( _T("%0.1f"), ConvertStarRLtoRL(pElement->GetStandardsHeight(MATRIX_STANDARDS_OUTER), pElement->GetStarSeparation())*CONVERT_MM_TO_M );
		m_lstctrlMatrixSelection.SetItemText( iListIndex, COLUMN_MATRIX_OUTER_STD, sText ); 
		sText.Format( _T("%0.1f"), ConvertStarRLtoRL(pElement->GetStandardsHeight(MATRIX_STANDARDS_INNER), pElement->GetStarSeparation())*CONVERT_MM_TO_M );
		m_lstctrlMatrixSelection.SetItemText( iListIndex, COLUMN_MATRIX_INNER_STD, sText ); 
		m_lstctrlMatrixSelection.SetItemData( iListIndex, (DWORD)i);
	}
	m_lstctrlMatrixSelection.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
	m_lstctrlMatrixSelection.ShowScrollBar( SB_VERT );
	m_lstctrlMatrixSelection.UpdateWindow( );
}

void EditStandardsDlg::FillStandards()
{
	doubleArray			daArrangement;
	MatrixElement		*pElement;


	pElement = GetFirstSelectedElement();
	;//assert( pElement!=NULL );
	UpdateArrayFromElement(pElement);
	FillStandardsFromArray();

	Bay	*pBay;
	pBay = pElement->GetBay(0);
	;//assert( pBay!=NULL );

	//Are the standards staggered?
	m_bStaggered = ConvertboolToBOOL( pBay->GetStaggeredHeights() );

	//What type of fit did they use?
	m_iStandardsFit = pBay->GetStandardFit();

	UpdateData( false );
}

void EditStandardsDlg::UpdateArrayFromElement(MatrixElement *pElement)
{
	bool		bSB;
	doubleArray	daArrangement;

	m_dStarSeparation = pElement->GetStarSeparation();

	pElement->GetStandardsArrangement( daArrangement, bSB, MATRIX_STANDARDS_INNER );
	pElement->ConvertRLArrangeToStdHeightArrange( daArrangement, true );
	m_bSBInner = ConvertboolToBOOL( bSB );
	m_daStandardsInner = daArrangement;

	pElement->GetStandardsArrangement( daArrangement, bSB, MATRIX_STANDARDS_OUTER );
	pElement->ConvertRLArrangeToStdHeightArrange( daArrangement, true );
	m_bSBOuter = ConvertboolToBOOL( bSB );
	m_daStandardsOuter = daArrangement;

	m_eSystem = pElement->GetSystem();
}

void EditStandardsDlg::FillStandardsFromArray()
{
	FillAStandardBox( m_lstboxStandardsInner, m_daStandardsInner, ConvertBOOLTobool(m_bSBInner), m_sHeightInner );
	FillAStandardBox( m_lstboxStandardsOuter, m_daStandardsOuter, ConvertBOOLTobool(m_bSBOuter), m_sHeightOuter );
	UpdateData( false );
}

void EditStandardsDlg::FillAStandardBox(CListBox &EditBox, doubleArray &daArrangement, bool bSoleBoard, CString &sTotal )
{
	int				i;
	double			dStdHgt, dTotal;
	CString			sText;

	EditBox.ResetContent();
	dTotal = 0.00;
	EditBox.AddString( _T("<Add Std>") );
	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		dStdHgt = daArrangement.GetAt(i);
		dTotal+= dStdHgt;
		sText.Format(_T("%0.1f"), dStdHgt );
		EditBox.AddString( sText );
	}
	sTotal.Format( _T("%0.1f"), dTotal );
}

void EditStandardsDlg::UpdateMatrixElementView()
{
}


void EditStandardsDlg::OnDblclkStandardsOuter() 
{
	int				iStandardPosition, iArrayPos, iOpenEnded;
	double			dNewStandardLength;
	CString			sStandardHeight;
	doubleArray		daArrangement;

	iStandardPosition = m_lstboxStandardsOuter.GetCurSel();

	m_lstboxStandardsOuter.GetText(iStandardPosition, sStandardHeight);
	iOpenEnded = (int)false;
	StandardHeightPickerDialog StandardHeightPicker( this, &sStandardHeight, &dNewStandardLength, &iOpenEnded );	
	if( StandardHeightPicker.DoModal() == IDOK)
	{
		iArrayPos = iStandardPosition-1;
		if( iArrayPos>=0 )
		{
			if( m_daStandardsOuter.GetAt( iArrayPos )!=dNewStandardLength );
			{
				//They have changed the height of this standard!
				m_daStandardsOuter.SetAt( iArrayPos, dNewStandardLength );
				ApplyRequired( true );
			}
		}
		else
		{
			//They double clicked on the '<add std>' section
			m_daStandardsOuter.InsertAt( 0, dNewStandardLength );
			ApplyRequired( true );
		}
	}
	if( IsApplyRequired() )
	{
		FillStandardsFromArray();
	}
}

void EditStandardsDlg::OnDblclkStandardsInner() 
{
	int				iStandardPosition, iArrayPos, iOpenEnded;
	double			dNewStandardLength;
	CString			sStandardHeight;
	doubleArray		daArrangement;

	iStandardPosition = m_lstboxStandardsInner.GetCurSel();

	m_lstboxStandardsInner.GetText(iStandardPosition, sStandardHeight);
	iOpenEnded = (int)false;
	StandardHeightPickerDialog StandardHeightPicker( this, &sStandardHeight, &dNewStandardLength, &iOpenEnded );	
	if( StandardHeightPicker.DoModal() == IDOK)
	{
		iArrayPos = iStandardPosition-1;
		if( iArrayPos>=0 )
		{
			if( m_daStandardsInner.GetAt( iArrayPos )!=dNewStandardLength );
			{
				//They have changed the height of this standard!
				m_daStandardsInner.SetAt( iArrayPos, dNewStandardLength );
				ApplyRequired( true );
			}
		}
		else
		{
			//They double clicked on the '<add std>' section
			m_daStandardsInner.InsertAt( 0, dNewStandardLength );
			ApplyRequired( true );
		}
	}
	if( IsApplyRequired() )
	{
		FillStandardsFromArray();
	}
}

#include "gs.h"
#include "acgs.h"
#include "dbsymtb.h"

void EditStandardsDlg::OnClickMatrixSectionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( IsApplyRequired() )
	{
		CString sMsg;
		sMsg = _T("Would you like to apply your changes before changing your selection?");
		if( MessageBox( sMsg, _T("Apply changes"), MB_YESNO|MB_ICONQUESTION )==IDYES )
			OnApply();
		ApplyRequired( false );
	}
	FillStandards();
	FillBaysList();

	SetViewportByMatrix();

	*pResult = 0;
}

MatrixElement * EditStandardsDlg::GetFirstSelectedElement()
{
	int				iIndex, iPosition;
	MatrixElement	*pElement;
	POSITION		pos;

	pos			= m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();
	iIndex		= m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
	iPosition	= (int)m_lstctrlMatrixSelection.GetItemData( iIndex );
	;//assert( iPosition>=0 );
	;//assert( iPosition<m_pMatrix->GetMatrixSize() );
	pElement	= m_pMatrix->GetMatrixElement( iPosition );
	;//assert( pElement!=NULL );
	return pElement;
}


void EditStandardsDlg::ApplyRequired(bool bReqd)
{
	m_bApplyRequired = bReqd;

	m_btnApply.EnableWindow( m_bApplyRequired );
}

bool EditStandardsDlg::IsApplyRequired()
{
	return m_bApplyRequired;
}

void EditStandardsDlg::OnSoleboardInner() 
{
	ApplyRequired( true );
}

void EditStandardsDlg::OnSoleboardOuter() 
{
	ApplyRequired( true );
}

BOOL EditStandardsDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
	{
		if(	(int)pMsg->wParam == DELETE_KEY		|| 
			(int)pMsg->wParam == BACKSPACE_KEY  )
		{
			DeleteRequested();
		}
	}
/*
	else if( pMsg->message == WM_RBUTTONUP )
	{
		if(	(int)pMsg->wParam == 0 )
		{
			DeleteRequested();
		}
	}
*/	
	return KwikscafDialog::PreTranslateMessage(pMsg);
}

void EditStandardsDlg::DeleteRequested()
{
	//Delete key pressed
	if( GetFocus() == &m_lstboxStandardsInner )
	{
		DeleteInner();
	}
	else if( GetFocus() == &m_lstboxStandardsOuter )
	{
		DeleteOuter();
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
	ApplyRequired( true );
}

void EditStandardsDlg::DeleteInner()
{
	int	iSelection;

	iSelection = m_lstboxStandardsInner.GetCurSel();
	if( iSelection>0 )
	{
		m_lstboxStandardsInner.DeleteString( iSelection );
		m_daStandardsInner.RemoveAt( iSelection-1 );
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
}

void EditStandardsDlg::DeleteOuter()
{
	int	iSelection;

	iSelection = m_lstboxStandardsOuter.GetCurSel();
	if( iSelection>0 )
	{
		m_lstboxStandardsOuter.DeleteString( iSelection );
		m_daStandardsOuter.RemoveAt( iSelection-1 );
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
}

void EditStandardsDlg::FillBaysList()
{
	int				i, iIndex, iElementIndex, iListIndex;
	Bay				*pBay;
	bool			bEastern;
	DWORD			dwData;
	CString			sText;
	POSITION		pos;
	MatrixElement	*pElement;

	pos = m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();

	m_lstctrlBays.DeleteAllItems( );
	while (pos) // go through each selection
	{
		iIndex = m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
		iElementIndex = (int)m_lstctrlMatrixSelection.GetItemData( iIndex );
		pElement = m_pMatrix->GetMatrixElement( iElementIndex );
		;//assert( pElement!=NULL );
		for( i=0; i<pElement->GetNumberOfBays(); i++ )
		{
			pBay = pElement->GetBay( i );

			//Bay number
			dwData = pBay->GetBayNumber();
			sText.Format( _T("%i"), (int)dwData );
 			iListIndex = m_lstctrlBays.InsertItem( i, sText );

			//Side
			bEastern = pBay->GetMatrixElementPointer()==pElement;
			dwData = dwData<<1;
			if( bEastern )
			{
				sText = _T("East");
				dwData = dwData|1;
			}
			else
			{
				;//assert( pBay->GetMatrixElementPointerAlt()==pElement );
				sText = _T("West");
			}
			m_lstctrlBays.SetItemText( iListIndex, COLUMN_BAYS_SIDE, sText );
			m_lstctrlBays.SetItemData( iListIndex, dwData );
		}
	}

	//Select all bays by default
	pos = m_lstctrlBays.GetFirstSelectedItemPosition();
	for( i=0; i<m_lstctrlBays.GetItemCount(); i++ )
	{
		m_lstctrlBays.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
	}
	m_lstctrlBays.ShowScrollBar( SB_VERT );
	m_lstctrlBays.UpdateWindow( );
	UpdateData();
}

void EditStandardsDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("EditStandardsDlg") );
}

void EditStandardsDlg::SetViewportByMatrix()
{
	int				iIndex, iElementIndex;
	bool			bFirst;
	POSITION		pos;
	AcGePoint2d		ptBottomLeft, ptTopRight, ptTempBL, ptTempTR;
	MatrixElement	*pElement;

	pos = m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();

	bFirst = true;
	while (pos) // go through each selection
	{
		iIndex = m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
		iElementIndex = (int)m_lstctrlMatrixSelection.GetItemData( iIndex );
		pElement = m_pMatrix->GetMatrixElement( iElementIndex );
		;//assert( pElement!=NULL );
		if( bFirst )
		{
			if( pElement->GetBoundingBox( ptBottomLeft, ptTopRight ) )
			{
				//we have the first postion now
				bFirst=false;
			}
		}
		else
		{
			if( pElement->GetBoundingBox( ptTempBL, ptTempTR ) )
			{
				ptBottomLeft.x = min( ptBottomLeft.x, ptTempBL.x );
				ptBottomLeft.y = min( ptBottomLeft.y, ptTempBL.y );
				ptTopRight.x = max( ptTopRight.x, ptTempTR.x );
				ptTopRight.y = max( ptTopRight.y, ptTempTR.y );
			}
		}
	}

	if( !bFirst )
		m_pController->SetViewportPosition( ptBottomLeft, ptTopRight );
}

void EditStandardsDlg::AdjustStandards()
{
	int			iIndex;
	Bay			TempBay;
	double		dHeightIn, dHeightOut, dStarSeparation, dRLAdjust;
	CString		sText;
	POSITION	pos;

	UpdateData();

	pos		= m_lstctrlMatrixSelection.GetFirstSelectedItemPosition();
	iIndex	= m_lstctrlMatrixSelection.GetNextSelectedItem(pos);
	dStarSeparation = m_pMatrix->GetMatrixElement(iIndex)->GetStarSeparation();
	dRLAdjust = dStarSeparation;
	sText = m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_OUTER_STD );
	dHeightOut = ConvertRLtoStarRL(_tstof( sText )*CONVERT_M_TO_MM, dStarSeparation)-dRLAdjust;
	sText = m_lstctrlMatrixSelection.GetItemText( iIndex, COLUMN_MATRIX_INNER_STD );
	dHeightIn = ConvertRLtoStarRL(_tstof( sText )*CONVERT_M_TO_MM, dStarSeparation)-dRLAdjust;
	if( m_iStandardsFit==STD_FIT_SAME )
	{
		dHeightIn = max( dHeightIn, dHeightOut );
		dHeightOut = dHeightIn;
	}
	int			i;
	doubleArray	daArrangment;
	daArrangment.RemoveAll();
	for( i=0; i<m_daStandardsInner.GetSize(); i++ )
	{
		daArrangment.Add( ConvertRLtoStarRL(m_daStandardsInner.GetAt(i), dStarSeparation) );
	}
	TempBay.MakeStandardsArrangment( m_daStandardsInner, ConvertStarRLtoRL(dHeightIn, dStarSeparation), true, ConvertBOOLTobool(m_bStaggered), (StandardFitEnum)m_iStandardsFit );
	m_daStandardsInner.ReverseOrder();

	daArrangment.RemoveAll();
	for( i=0; i<m_daStandardsOuter.GetSize(); i++ )
	{
		daArrangment.Add( ConvertRLtoStarRL(m_daStandardsOuter.GetAt(i), dStarSeparation) );
	}
	TempBay.MakeStandardsArrangment( m_daStandardsOuter, ConvertStarRLtoRL(dHeightOut, dStarSeparation), false, ConvertBOOLTobool(m_bStaggered), (StandardFitEnum)m_iStandardsFit );
	m_daStandardsOuter.ReverseOrder();
	FillStandardsFromArray();
	ApplyRequired(true);
}

bool EditStandardsDlg::TestCommitted(doubleArray daStdComp, CornerOfBayEnum eCnr, Bay *pBay )
{
	int				i, j;
	bool			bValid;
	double			dRL, dStdLengthBay, dStdLength;
	Component		*pComp;
	doubleArray		daStdTest;
	MatrixElement	Temp;

	pBay->GetStandardsArrangement( daStdTest, eCnr );
	Temp.ConvertStdHeightArrangeToRLArrange( daStdTest, pBay->GetStandardPosition( eCnr ).z );
	Temp.ConvertStdHeightArrangeToRLArrange( daStdComp, pBay->GetStandardPosition( eCnr ).z );
	bValid = true;
	for( i=0; i<daStdTest.GetSize()-1 && bValid; i++ )
	{
		dRL = daStdTest.GetAt(i);
		pComp = pBay->GetComponentAtRL( CT_STANDARD, dRL, CornerAsSideOfBay( eCnr ) );
		if( pComp!=NULL )
		{
			if( pComp->IsCommitted() )
			{
				dStdLengthBay = daStdTest.GetAt(i+1)-dRL;
				//This standard is committed in the Bay!
				//The new arrangement better have a standard of the same height at this RL
				for( j=0; j<daStdComp.GetSize()-1; j++ )
				{
					if( daStdComp.GetAt(j)==dRL )
					{
						dStdLength = daStdComp.GetAt(j+1)-daStdComp.GetAt(j);
						if( dStdLength<dStdLengthBay-ROUND_ERROR ||
							dStdLength>dStdLengthBay+ROUND_ERROR )
						{
							//not the same length
							bValid = false;
							break;
						}
					}
				}
			}
		}
		else
		{
			//;//assert( false );
		}
	}
	return bValid;
}

void EditStandardsDlg::GetCommittedStageLevel(Bay *pBay, CornerOfBayEnum eCnr, intArray &iaCommitted, intArray &iaLevels, doubleArray &daRLs, CStringArray &saStages)
{
	int				i;
	double			dRL;
	Component		*pComp;
	doubleArray		daStdTest;
	MatrixElement	Temp;

	iaCommitted.RemoveAll();
	iaLevels.RemoveAll();
	daRLs.RemoveAll();
	saStages.RemoveAll();
	////////////////////////////////////////////////////////////////////////////////////
	//We need to store which standards are committed and also what stage and level they belong to!
	//	we will then need to reapply those committed flags and stage and level back to the new standards
	pBay->GetStandardsArrangement( daStdTest, eCnr );
	Temp.ConvertStdHeightArrangeToRLArrange( daStdTest, pBay->GetStandardPosition( eCnr ).z );
	for( i=0; i<daStdTest.GetSize()-1; i++ )
	{
		dRL = daStdTest.GetAt(i);
		pComp = pBay->GetComponentAtRL( CT_STANDARD, dRL, CornerAsSideOfBay( eCnr ) );
		if( pComp!=NULL )
		{
			daRLs.Add( dRL );
			iaCommitted.Add( (int)pComp->IsCommitted() );
			iaLevels.Add( pComp->GetLevel() );
			saStages.Add( pComp->GetStage() );
		}
		else
		{
			//;//assert( false );
		}
	}
}

void EditStandardsDlg::SetCommittedStageLevel(Bay *pBay, CornerOfBayEnum eCnr, intArray &iaCommitted, intArray &iaLevels, doubleArray &daRLs, CStringArray &saStages)
{
	int				i, j, iLevel;
	bool			bCommitted;
	double			dRL, dRLCommit;
	CString			sStage;
	Component		*pComp;
	doubleArray		daStdTest;
	MatrixElement	Temp;

	pBay->GetStandardsArrangement( daStdTest, eCnr );
	Temp.ConvertStdHeightArrangeToRLArrange( daStdTest, pBay->GetStandardPosition( eCnr ).z );
	for( i=0; i<daStdTest.GetSize()-1; i++ )
	{
		dRL = daStdTest.GetAt(i);
		pComp = pBay->GetComponentAtRL( CT_STANDARD, dRL, CornerAsSideOfBay( eCnr ) );
		if( pComp!=NULL )
		{
			for( j=0; j<iaCommitted.GetSize(); j++ )
			{
				//we are only interested in committed ones
				bCommitted = ConvertBOOLTobool((BOOL)iaCommitted.GetAt(j));
				if( bCommitted )
				{
					dRLCommit = daRLs.GetAt(j);
					if( dRL>dRLCommit-ROUND_ERROR && dRL<dRLCommit+ROUND_ERROR )
					{
						//Set the committed flag
						pComp->SetCommitted( bCommitted );

						//Don't set the level unless it is different!
						iLevel = iaLevels.GetAt(j);
						if( pComp->GetLevel()!=iLevel )
							pComp->SetLevel( iLevel );

						//Don't set the stage unless it is different!
						sStage = saStages.GetAt(j);
 						if( pComp->GetStage()!=sStage )
							pComp->SetStage( sStage );
					}
				}
			}
		}
		else
		{
			//;//assert( false );
		}
	}
}

double EditStandardsDlg::JackLength() const
{
	return m_dStarSeparation;
}
