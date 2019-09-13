// ShowComponentTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "ShowComponentTypeDialog.h"
#include "doubleArray.h"
#include "StageLevelShowHideDlg.h"
#include "ColourCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ShowComponentTypeDialog dialog


ShowComponentTypeDialog::ShowComponentTypeDialog(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(ShowComponentTypeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(ShowComponentTypeDialog)
	m_bChainLink = FALSE;
	m_bBracing = FALSE;
	m_bCornerStageBoard = FALSE;
	m_bDeckingPlank = FALSE;
	m_bHopupBracket = FALSE;
	m_bJack = FALSE;
	m_bLadder = FALSE;
	m_bLadderPutlog = FALSE;
	m_bLapboard = FALSE;
	m_bLedger = FALSE;
	m_bMeshGuard = FALSE;
	m_bMidRail = FALSE;
	m_bRail = FALSE;
	m_bShadeCloth = FALSE;
	m_bSoleBoards = FALSE;
	m_bStageBoard = FALSE;
	m_bStair = FALSE;
	m_bStairRail = FALSE;
	m_bStairRailStopend = FALSE;
	m_bStandard = FALSE;
	m_bStandardConnector = FALSE;
	m_bStandardOpenend = FALSE;
	m_bText = FALSE;
	m_bTieBar = FALSE;
	m_bTieClamp90Deg = FALSE;
	m_bTieClampColumn = FALSE;
	m_bTieClampMasonary = FALSE;
	m_bTieClampYoke = FALSE;
	m_bTieTube = FALSE;
	m_bToeBoard = FALSE;
	m_bToeBoardClip = FALSE;
	m_bTransom = FALSE;
	//}}AFX_DATA_INIT


	m_iColourTransom			= 1;
	m_iColourToeBoard			= 2;
	m_iColourTieClampYoke		= 3;
	m_iColourTieClampMasonary	= 4;
	m_iColourTieClampColumn		= 5;
	m_iColourTieClamp90Deg		= 6;
	m_iColourTieBar				= 7;
	m_iColourStandardsOpenEnd	= 8;
	m_iColourStandards			= 9;
	m_iColourStair				= 10;
	m_iColourStageBoard			= 11;
	m_iColourSoleBoards			= 12;
	m_iColourMidRail			= 13;
	m_iColourShadeCloth			= 14;
	m_iColourRail				= 15;
	m_iColourMeshGuard			= 16;
	m_iColourLedger				= 17;
	m_iColourJacks				= 18;
	m_iColourHopupBrackets		= 19;
	m_iColourDeckingPlank		= 20;
	m_iColourCornerStageBoard	= 21;
	m_iColourChainLink			= 22;
	m_iColourBracing			= 23;
	m_iColourStairRail			= 24;
	m_iColourStairRailStopend	= 25;
	m_iColourStandardConnector	= 26;
	m_iColourToeBoardClip		= 27;
	m_iColourText				= 28;
	m_iColourLadder				= 29;
	m_iColourLadderPutlog		= 30;
	m_iColourLapboard			= 31;
	m_iColourTieTubes			= 32;
}


void ShowComponentTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ShowComponentTypeDialog)
	DDX_Control(pDX, IDC_COLOUR_PARTIAL_COMMITTED, m_btnColourPartialCommitted);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_RL_TEXT, m_btnColourMatrixRLText);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_RL_SPAN, m_btnColourMatrixRLSpan);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_RL_MINOR, m_btnColourMatrixRLMinor);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_RL_MAJOR, m_btnColourMatrixRLMajor);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_LEVEL_TEXT, m_btnColourMatrixLevelText);
	DDX_Control(pDX, IDC_COLOUR_MATRIX_LEVEL_LINES, m_btnColourMatrixLevelLines);
	DDX_Control(pDX, IDC_COLOUR_CROSSHAIR_ARC, m_btnColourCrosshairArc);
	DDX_Control(pDX, IDC_COLOUR_CROSSHAIR, m_btnColourCrosshair);
	DDX_Control(pDX, IDC_COLOUR_COMMITTED, m_btnColourCommitted);
	DDX_Control(pDX, IDC_COLOUR_BAY_2400, m_btnColourBay2400);
	DDX_Control(pDX, IDC_COLOUR_BAY_1800, m_btnColourBay1800);
	DDX_Control(pDX, IDC_COLOUR_BAY_0800, m_btnColourBay0800);
	DDX_Control(pDX, IDC_COLOUR_BAY_1200, m_btnColourBay1200);
	DDX_Control(pDX, IDC_COLOUR_LAPBOARD, m_btnColourLapboard);
	DDX_Control(pDX, IDC_COLOUR_LADDER_PUTLOG, m_btnColourLadderPutlog);
	DDX_Control(pDX, IDC_COLOUR_LADDER, m_btnColourLadder);
	DDX_Control(pDX, IDC_COLOUR_TIE_TUBES, m_btnColourTieTubes);
	DDX_Control(pDX, IDC_COLOUR_TRANSOM, m_btnColourTransom);
	DDX_Control(pDX, IDC_COLOUR_TOE_BOARD, m_btnColourToeBoard);
	DDX_Control(pDX, IDC_COLOUR_TIE_CLAMP_YOKE, m_btnColourTieClampYoke);
	DDX_Control(pDX, IDC_COLOUR_TIE_CLAMP_MASONARY, m_btnColourTieClampMasonary);
	DDX_Control(pDX, IDC_COLOUR_TIE_CLAMP_COLUMN, m_btnColourTieClampColumn);
	DDX_Control(pDX, IDC_COLOUR_TIE_CLAMP_90DEGREE, m_btnColourTieClamp90Deg);
	DDX_Control(pDX, IDC_COLOUR_TIE_BAR, m_btnColourTieBar);
	DDX_Control(pDX, IDC_COLOUR_STANDARDS_OPEN_END, m_btnColourStandardsOpenEnd);
	DDX_Control(pDX, IDC_COLOUR_STANDARDS, m_btnColourStandards);
	DDX_Control(pDX, IDC_COLOUR_STAIR, m_btnColourStair);
	DDX_Control(pDX, IDC_COLOUR_STAGE_BOARD, m_btnColourStageBoard);
	DDX_Control(pDX, IDC_COLOUR_SOLEBOARDS, m_btnColourSoleBoards);
	DDX_Control(pDX, IDC_COLOUR_MID_RAIL, m_btnColourMidRail);
	DDX_Control(pDX, IDC_COLOUR_SHADE_CLOTH, m_btnColourShadeCloth);
	DDX_Control(pDX, IDC_COLOUR_RAIL, m_btnColourRail);
	DDX_Control(pDX, IDC_COLOUR_MESH_GUARD, m_btnColourMeshGuard);
	DDX_Control(pDX, IDC_COLOUR_LEDGER, m_btnColourLedger);
	DDX_Control(pDX, IDC_COLOUR_JACKS, m_btnColourJacks);
	DDX_Control(pDX, IDC_COLOUR_HOPUP_BRACKETS, m_btnColourHopupBrackets);
	DDX_Control(pDX, IDC_COLOUR_DECKING_PLANK, m_btnColourDeckingPlank);
	DDX_Control(pDX, IDC_COLOUR_CORNER_STAGE_BOARD, m_btnColourCornerStageBoard);
	DDX_Control(pDX, IDC_COLOUR_CHAIN_LINK, m_btnColourChainLink);
	DDX_Control(pDX, IDC_COLOUR_BRACING, m_btnColourBracing);
	DDX_Control(pDX, IDC_COLOUR_STAIR_RAIL, m_btnColourStairRail);
	DDX_Control(pDX, IDC_COLOUR_STAIR_RAIL_STOPEND, m_btnColourStairRailStopend);
	DDX_Control(pDX, IDC_COLOUR_STANDARD_CONNECTOR, m_btnColourStandardConnector);
	DDX_Control(pDX, IDC_COLOUR_TOE_BOARD_CLIP, m_btnColourToeBoardClip);
	DDX_Control(pDX, IDC_COLOUR_TEXT, m_btnColourText);
	DDX_Check(pDX, IDC_CT_CHAIN_LINK, m_bChainLink);
	DDX_Check(pDX, IDC_CT_BRACING, m_bBracing);
	DDX_Check(pDX, IDC_CT_CORNER_STAGE_BOARD, m_bCornerStageBoard);
	DDX_Check(pDX, IDC_CT_DECKING_PLANK, m_bDeckingPlank);
	DDX_Check(pDX, IDC_CT_HOPUP_BRACKET, m_bHopupBracket);
	DDX_Check(pDX, IDC_CT_JACK, m_bJack);
	DDX_Check(pDX, IDC_CT_LADDER, m_bLadder);
	DDX_Check(pDX, IDC_CT_LADDER_PUTLOG, m_bLadderPutlog);
	DDX_Check(pDX, IDC_CT_LAPBOARD, m_bLapboard);
	DDX_Check(pDX, IDC_CT_LEDGER, m_bLedger);
	DDX_Check(pDX, IDC_CT_MESH_GUARD, m_bMeshGuard);
	DDX_Check(pDX, IDC_CT_MID_RAIL, m_bMidRail);
	DDX_Check(pDX, IDC_CT_RAIL, m_bRail);
	DDX_Check(pDX, IDC_CT_SHADE_CLOTH, m_bShadeCloth);
	DDX_Check(pDX, IDC_CT_SOLEBOARD, m_bSoleBoards);
	DDX_Check(pDX, IDC_CT_STAGE_BOARD, m_bStageBoard);
	DDX_Check(pDX, IDC_CT_STAIR, m_bStair);
	DDX_Check(pDX, IDC_CT_STAIR_RAIL, m_bStairRail);
	DDX_Check(pDX, IDC_CT_STAIR_RAIL_STOPEND, m_bStairRailStopend);
	DDX_Check(pDX, IDC_CT_STANDARD, m_bStandard);
	DDX_Check(pDX, IDC_CT_STANDARD_CONNECTOR, m_bStandardConnector);
	DDX_Check(pDX, IDC_CT_STANDARD_OPENEND, m_bStandardOpenend);
	DDX_Check(pDX, IDC_CT_TEXT, m_bText);
	DDX_Check(pDX, IDC_CT_TIE_BAR, m_bTieBar);
	DDX_Check(pDX, IDC_CT_TIE_CLAMP_90DEGREE, m_bTieClamp90Deg);
	DDX_Check(pDX, IDC_CT_TIE_CLAMP_COLUMN, m_bTieClampColumn);
	DDX_Check(pDX, IDC_CT_TIE_CLAMP_MASONARY, m_bTieClampMasonary);
	DDX_Check(pDX, IDC_CT_TIE_CLAMP_YOKE, m_bTieClampYoke);
	DDX_Check(pDX, IDC_CT_TIE_TUBE, m_bTieTube);
	DDX_Check(pDX, IDC_CT_TOE_BOARD, m_bToeBoard);
	DDX_Check(pDX, IDC_CT_TOE_BOARD_CLIP, m_bToeBoardClip);
	DDX_Check(pDX, IDC_CT_TRANSOM, m_bTransom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ShowComponentTypeDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(ShowComponentTypeDialog)
	ON_BN_CLICKED(IDC_HIDE_ALL, OnHideAll)
	ON_BN_CLICKED(IDC_SHOW_ALL, OnShowAll)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_COLOUR_TEXT, OnColourText)
	ON_BN_CLICKED(IDC_COLOUR_STAIR_RAIL, OnColourStairRail)
	ON_BN_CLICKED(IDC_COLOUR_STAIR_RAIL_STOPEND, OnColourStairRailStopend)
	ON_BN_CLICKED(IDC_COLOUR_STANDARD_CONNECTOR, OnColourStandardConnector)
	ON_BN_CLICKED(IDC_COLOUR_TOE_BOARD_CLIP, OnColourToeBoardClip)
	ON_BN_CLICKED(IDC_COLOUR_BRACING, OnColourBracing)
	ON_BN_CLICKED(IDC_COLOUR_CHAIN_LINK, OnColourChainLink)
	ON_BN_CLICKED(IDC_COLOUR_CORNER_STAGE_BOARD, OnColourCornerStageBoard)
	ON_BN_CLICKED(IDC_COLOUR_DECKING_PLANK, OnColourDeckingPlank)
	ON_BN_CLICKED(IDC_COLOUR_HOPUP_BRACKETS, OnColourHopupBrackets)
	ON_BN_CLICKED(IDC_COLOUR_JACKS, OnColourJacks)
	ON_BN_CLICKED(IDC_COLOUR_LEDGER, OnColourLedger)
	ON_BN_CLICKED(IDC_COLOUR_MESH_GUARD, OnColourMeshGuard)
	ON_BN_CLICKED(IDC_COLOUR_MID_RAIL, OnColourMidRail)
	ON_BN_CLICKED(IDC_COLOUR_RAIL, OnColourRail)
	ON_BN_CLICKED(IDC_COLOUR_SHADE_CLOTH, OnColourShadeCloth)
	ON_BN_CLICKED(IDC_COLOUR_SOLEBOARDS, OnColourSoleBoards)
	ON_BN_CLICKED(IDC_COLOUR_STAGE_BOARD, OnColourStageBoard)
	ON_BN_CLICKED(IDC_COLOUR_STAIR, OnColourStair)
	ON_BN_CLICKED(IDC_COLOUR_STANDARDS, OnColourStandards)
	ON_BN_CLICKED(IDC_COLOUR_STANDARDS_OPEN_END, OnColourStandardsOpenEnd)
	ON_BN_CLICKED(IDC_COLOUR_TIE_BAR, OnColourTieBar)
	ON_BN_CLICKED(IDC_COLOUR_TIE_CLAMP_90DEGREE, OnColourTieClamp90Deg)
	ON_BN_CLICKED(IDC_COLOUR_TIE_CLAMP_COLUMN, OnColourTieClampColumn)
	ON_BN_CLICKED(IDC_COLOUR_TIE_CLAMP_MASONARY, OnColourTieClampMasonary)
	ON_BN_CLICKED(IDC_COLOUR_TIE_CLAMP_YOKE, OnColourTieClampYoke)
	ON_BN_CLICKED(IDC_COLOUR_TOE_BOARD, OnColourToeBoard)
	ON_BN_CLICKED(IDC_COLOUR_TRANSOM, OnColourTransom)
	ON_BN_CLICKED(IDC_COLOUR_LADDER, OnColourLadder)
	ON_BN_CLICKED(IDC_COLOUR_LADDER_PUTLOG, OnColourLadderPutlog)
	ON_BN_CLICKED(IDC_COLOUR_LAPBOARD, OnColourLapboard)
	ON_BN_CLICKED(IDC_COLOUR_TIE_TUBES, OnColourTieTubes)
	ON_BN_CLICKED(IDC_COLOUR_BAY_0800, OnColourBay0800)
	ON_BN_CLICKED(IDC_COLOUR_BAY_1200, OnColourBay1200)
	ON_BN_CLICKED(IDC_COLOUR_BAY_1800, OnColourBay1800)
	ON_BN_CLICKED(IDC_COLOUR_BAY_2400, OnColourBay2400)
	ON_BN_CLICKED(IDC_COLOUR_COMMITTED, OnColourCommitted)
	ON_BN_CLICKED(IDC_COLOUR_CROSSHAIR, OnColourCrosshair)
	ON_BN_CLICKED(IDC_COLOUR_CROSSHAIR_ARC, OnColourCrosshairArc)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_LEVEL_LINES, OnColourMatrixLevelLines)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_LEVEL_TEXT, OnColourMatrixLevelText)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_RL_MAJOR, OnColourMatrixRLMajor)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_RL_MINOR, OnColourMatrixRLMinor)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_RL_SPAN, OnColourMatrixRLSpan)
	ON_BN_CLICKED(IDC_COLOUR_MATRIX_RL_TEXT, OnColourMatrixRLText)
	ON_BN_CLICKED(IDC_COLOUR_PARTIAL_COMMITTED, OnColourPartialCommitted)
	ON_BN_CLICKED(IDC_STAGE_LEVEL, OnStageLevel)
	ON_BN_CLICKED(IDC_RESET_COLOUR, OnResetColour)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ShowComponentTypeDialog message handlers

void ShowComponentTypeDialog::OnHideAll() 
{
	SetAll(FALSE);
	UpdateData(FALSE);
}

void ShowComponentTypeDialog::OnShowAll() 
{
	SetAll(TRUE);
	UpdateData(FALSE);
}

void ShowComponentTypeDialog::OnOK() 
{
	UpdateData();
	m_piaComponentsToHide->RemoveAll();

	if( !m_bChainLink )			m_piaComponentsToHide->Add( (int)CT_CHAIN_LINK );
	if( !m_bBracing )			m_piaComponentsToHide->Add( (int)CT_BRACING );
	if( !m_bCornerStageBoard )	m_piaComponentsToHide->Add( (int)CT_CORNER_STAGE_BOARD );
	if( !m_bDeckingPlank )		m_piaComponentsToHide->Add( (int)CT_DECKING_PLANK );
	if( !m_bHopupBracket )		m_piaComponentsToHide->Add( (int)CT_HOPUP_BRACKET );
	if( !m_bJack )				m_piaComponentsToHide->Add( (int)CT_JACK );
	if( !m_bLadder )			m_piaComponentsToHide->Add( (int)CT_LADDER );
	if( !m_bLadderPutlog )		m_piaComponentsToHide->Add( (int)CT_LADDER_PUTLOG );
	if( !m_bLapboard )			m_piaComponentsToHide->Add( (int)CT_LAPBOARD );
	if( !m_bLedger )			m_piaComponentsToHide->Add( (int)CT_LEDGER );
	if( !m_bMeshGuard )			m_piaComponentsToHide->Add( (int)CT_MESH_GUARD );
	if( !m_bMidRail )			m_piaComponentsToHide->Add( (int)CT_MID_RAIL );
	if( !m_bRail )				m_piaComponentsToHide->Add( (int)CT_RAIL );
	if( !m_bShadeCloth )		m_piaComponentsToHide->Add( (int)CT_SHADE_CLOTH );
	if( !m_bSoleBoards )		m_piaComponentsToHide->Add( (int)CT_SOLEBOARD );
	if( !m_bStageBoard )		m_piaComponentsToHide->Add( (int)CT_STAGE_BOARD );
	if( !m_bStair )				m_piaComponentsToHide->Add( (int)CT_STAIR );
	if( !m_bStairRail )			m_piaComponentsToHide->Add( (int)CT_STAIR_RAIL );
	if( !m_bStairRailStopend )	m_piaComponentsToHide->Add( (int)CT_STAIR_RAIL_STOPEND );
	if( !m_bStandard )			m_piaComponentsToHide->Add( (int)CT_STANDARD );
	if( !m_bStandardConnector )	m_piaComponentsToHide->Add( (int)CT_STANDARD_CONNECTOR );
	if( !m_bStandardOpenend )	m_piaComponentsToHide->Add( (int)CT_STANDARD_OPENEND );
	if( !m_bText )				m_piaComponentsToHide->Add( (int)CT_TEXT );
	if( !m_bTieBar )			m_piaComponentsToHide->Add( (int)CT_TIE_BAR );
	if( !m_bTieClamp90Deg )		m_piaComponentsToHide->Add( (int)CT_TIE_CLAMP_90DEGREE );
	if( !m_bTieClampColumn )	m_piaComponentsToHide->Add( (int)CT_TIE_CLAMP_COLUMN );
	if( !m_bTieClampMasonary )	m_piaComponentsToHide->Add( (int)CT_TIE_CLAMP_MASONARY );
	if( !m_bTieClampYoke )		m_piaComponentsToHide->Add( (int)CT_TIE_CLAMP_YOKE );
	if( !m_bTieTube )			m_piaComponentsToHide->Add( (int)CT_TIE_TUBE );
	if( !m_bToeBoard )			m_piaComponentsToHide->Add( (int)CT_TOE_BOARD );
	if( !m_bToeBoardClip )		m_piaComponentsToHide->Add( (int)CT_TOE_BOARD_CLIP );
	if( !m_bTransom )			m_piaComponentsToHide->Add( (int)CT_TRANSOM );

	KwikscafDialog::OnOK();
}

BOOL ShowComponentTypeDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("ShowComponentTypeDialog"));

	assert( m_piaComponentsToHide!=NULL );
	
	OnShowAll();
	for( int i=0; i<m_piaComponentsToHide->GetSize(); i++ )
	{
		switch( m_piaComponentsToHide->GetAt(i) )
		{
		case( CT_CHAIN_LINK ):			m_bChainLink		= FALSE;	break;
		case( CT_BRACING ):				m_bBracing			= FALSE;	break;
		case( CT_CORNER_STAGE_BOARD ):	m_bCornerStageBoard	= FALSE;	break;
		case( CT_DECKING_PLANK ):		m_bDeckingPlank		= FALSE;	break;
		case( CT_HOPUP_BRACKET ):		m_bHopupBracket		= FALSE;	break;
		case( CT_JACK ):				m_bJack				= FALSE;	break;
		case( CT_LADDER ):				m_bLadder			= FALSE;	break;
		case( CT_LADDER_PUTLOG ):		m_bLadderPutlog		= FALSE;	break;
		case( CT_LAPBOARD ):			m_bLapboard			= FALSE;	break;
		case( CT_LEDGER ):				m_bLedger			= FALSE;	break;
		case( CT_MESH_GUARD ):			m_bMeshGuard		= FALSE;	break;
		case( CT_MID_RAIL ):			m_bMidRail			= FALSE;	break;
		case( CT_RAIL ):				m_bRail				= FALSE;	break;
		case( CT_SHADE_CLOTH ):			m_bShadeCloth		= FALSE;	break;
		case( CT_SOLEBOARD ):			m_bSoleBoards		= FALSE;	break;
		case( CT_STAGE_BOARD ):			m_bStageBoard		= FALSE;	break;
		case( CT_STAIR ):				m_bStair			= FALSE;	break;
		case( CT_STAIR_RAIL ):			m_bStairRail		= FALSE;	break;
		case( CT_STAIR_RAIL_STOPEND ):	m_bStairRailStopend	= FALSE;	break;
		case( CT_STANDARD ):			m_bStandard			= FALSE;	break;
		case( CT_STANDARD_CONNECTOR ):	m_bStandardConnector= FALSE;	break;
		case( CT_STANDARD_OPENEND ):	m_bStandardOpenend	= FALSE;	break;
		case( CT_TEXT ):				m_bText				= FALSE;	break;
		case( CT_TIE_BAR ):				m_bTieBar			= FALSE;	break;
		case( CT_TIE_CLAMP_90DEGREE ):	m_bTieClamp90Deg	= FALSE;	break;
		case( CT_TIE_CLAMP_COLUMN ):	m_bTieClampColumn	= FALSE;	break;
		case( CT_TIE_CLAMP_MASONARY ):	m_bTieClampMasonary	= FALSE;	break;
		case( CT_TIE_CLAMP_YOKE ):		m_bTieClampYoke		= FALSE;	break;
		case( CT_TIE_TUBE ):			m_bTieTube			= FALSE;	break;
		case( CT_TOE_BOARD ):			m_bToeBoard			= FALSE;	break;
		case( CT_TOE_BOARD_CLIP ):		m_bToeBoardClip		= FALSE;	break;
		case( CT_TRANSOM ):				m_bTransom			= FALSE;	break;
		default:
			assert( false );
		}
	}

	FillColours();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ShowComponentTypeDialog::SetAll(BOOL bNewValue)
{
	m_bChainLink = bNewValue;
	m_bBracing = bNewValue;
	m_bCornerStageBoard = bNewValue;
	m_bDeckingPlank = bNewValue;
	m_bHopupBracket = bNewValue;
	m_bJack = bNewValue;
	m_bLadder = bNewValue;
	m_bLadderPutlog = bNewValue;
	m_bLapboard = bNewValue;
	m_bLedger = bNewValue;
	m_bMeshGuard = bNewValue;
	m_bMidRail = bNewValue;
	m_bRail = bNewValue;
	m_bShadeCloth = bNewValue;
	m_bSoleBoards = bNewValue;
	m_bStageBoard = bNewValue;
	m_bStair = bNewValue;
	m_bStairRail = bNewValue;
	m_bStairRailStopend = bNewValue;
	m_bStandard = bNewValue;
	m_bStandardConnector = bNewValue;
	m_bStandardOpenend = bNewValue;
	m_bText = bNewValue;
	m_bTieBar = bNewValue;
	m_bTieClamp90Deg = bNewValue;
	m_bTieClampColumn = bNewValue;
	m_bTieClampMasonary = bNewValue;
	m_bTieClampYoke = bNewValue;
	m_bTieTube = bNewValue;
	m_bToeBoard = bNewValue;
	m_bToeBoardClip = bNewValue;
	m_bTransom = bNewValue;
}

void ShowComponentTypeDialog::SetComponentTypeHideArray(intArray *pArray)
{
	m_piaComponentsToHide = pArray;
	assert( m_piaComponentsToHide!=NULL );
}

void ShowComponentTypeDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("ShowComponentTypeDialog") );
}

//#include <afxdlgs.h>
#include "aced.h"

#define OnColour( CompType ) \
void ShowComponentTypeDialog::OnColour##CompType() \
{ \
	do \
	{ \
		acedSetColorDialog( m_iColour##CompType, Adesk::kFalse, 0 ); \
		if( m_iColour##CompType==COLOUR_COMMITTED_FULLY ) \
			MessageBox( _T("This colour is reserved for fully committed components,\nplease choose another"), _T("Invalid Colour") ); \
		if( m_iColour##CompType==COLOUR_COMMITTED_PARTIALLY) \
			MessageBox( _T("This colour is reserved for partially committed components,\nplease choose another"), _T("Invalid Colour") ); \
	}while( m_iColour##CompType==COLOUR_COMMITTED_FULLY || m_iColour##CompType==COLOUR_COMMITTED_PARTIALLY); \
 	m_btnColour##CompType.SetAutoCADColour( m_iColour##CompType ); \
	m_btnColour##CompType.RedrawWindow(); \
	UpdateData(false); \
}\

OnColour(Text) 
OnColour(StairRail) 
OnColour(Transom)
OnColour(ToeBoard)
OnColour(TieClampYoke)
OnColour(TieClampMasonary)
OnColour(TieClampColumn)
OnColour(TieClamp90Deg)
OnColour(TieBar)
OnColour(StandardsOpenEnd)
OnColour(Standards)
OnColour(Stair)
OnColour(StageBoard)
OnColour(SoleBoards)
OnColour(MidRail)
OnColour(ShadeCloth)
OnColour(Rail)
OnColour(MeshGuard)
OnColour(Ledger)
OnColour(Jacks)
OnColour(HopupBrackets)
OnColour(DeckingPlank)
OnColour(CornerStageBoard)
OnColour(ChainLink)
OnColour(Bracing)
OnColour(StairRailStopend)
OnColour(StandardConnector)
OnColour(ToeBoardClip)
OnColour(Ladder)
OnColour(LadderPutlog)
OnColour(Lapboard)
OnColour(TieTubes)
OnColour(PartialCommitted)
OnColour(MatrixRLText)
OnColour(MatrixRLSpan)
OnColour(MatrixRLMinor)
OnColour(MatrixRLMajor)
OnColour(MatrixLevelText)
OnColour(MatrixLevelLines)
OnColour(CrosshairArc)
OnColour(Crosshair)
OnColour(Committed)
OnColour(Bay2400)
OnColour(Bay1800)
OnColour(Bay0800)
OnColour(Bay1200)

void ShowComponentTypeDialog::OnStageLevel() 
{
	StageLevelShowHideDlg	Dlg( this );
	Dlg.DoModal();
}

void ShowComponentTypeDialog::OnResetColour() 
{
	UpdateData();

	/////////////////////////////////////////////////////////////////////////////
	CString sMsg;
	sMsg = _T("By reseting the colours you will loose any colour changes you have made!\n\nContinue and loose changes?");
	if( MessageBox( sMsg, _T("Colour Reset Warning"), MB_OKCANCEL )==IDCANCEL )
		return;

	/////////////////////////////////////////////////////////////////////////////
	int iColours[100], i, j;
	i=0;
	iColours[i++]	= COLOUR_DECKING_PLANK;
	iColours[i++]	= COLOUR_STAGE_BOARD;
	iColours[i++]	= COLOUR_LAPBOARD;
	iColours[i++]	= COLOUR_LEDGER;
	iColours[i++]	= COLOUR_TRANSOM;
	iColours[i++]	= COLOUR_MESH_GUARD;
	iColours[i++]	= COLOUR_RAIL;
	iColours[i++]	= COLOUR_MID_RAIL;
	iColours[i++]	= COLOUR_TOE_BOARD;
	iColours[i++]	= COLOUR_CHAIN_LINK;
	iColours[i++]	= COLOUR_SHADE_CLOTH;
	iColours[i++]	= COLOUR_TIE_TUBE;
	iColours[i++]	= COLOUR_TIE_CLAMP_COLUMN;
	iColours[i++]	= COLOUR_TIE_CLAMP_MASONARY;
	iColours[i++]	= COLOUR_TIE_CLAMP_YOKE;
	iColours[i++]	= COLOUR_TIE_CLAMP_90DEGREE;
	iColours[i++]	= COLOUR_BRACING;
	iColours[i++]	= COLOUR_STAIR;
	iColours[i++]	= COLOUR_LADDER;
	iColours[i++]	= COLOUR_STANDARD;
	iColours[i++]	= COLOUR_JACK;
	iColours[i++]	= COLOUR_SOLEBOARD;
	iColours[i++]	= COLOUR_HOPUP_BRACKET;
	iColours[i++]	= COLOUR_CORNER_STAGE_BOARD;
	iColours[i++]	= COLOUR_TEXT;
	iColours[i++]	= COLOUR_TIE_BAR;
	iColours[i++]	= COLOUR_STANDARD_CONNECTOR;
	iColours[i++]	= COLOUR_LADDER_PUTLOG;
	iColours[i++]	= COLOUR_STAIR_RAIL;
	iColours[i++]	= COLOUR_STAIR_RAIL_STOPEND;
	iColours[i++]	= COLOUR_TOE_BOARD_CLIP;
	iColours[i++]	= COLOUR_BOM_EXTRA;
	iColours[i++]	= COLOUR_PUTLOG_CLIP;
	iColours[i++]	= COLOUR_VISUAL_COMPONENT;
	iColours[i++]	= COLOUR_CROSSHAIR;
	iColours[i++]	= COLOUR_CROSSHAIR_DEBUG;
	iColours[i++]	= COLOUR_CROSSHAIR_ARC;
	iColours[i++]	= COLOUR_CROSSHAIR_3D_VIEW;
	iColours[i++]	= COLOUR_CROSSHAIR_MATRIX;
	iColours[i++]	= COLOUR_MATRIX_RL_TEXT;
	iColours[i++]	= COLOUR_MATRIX_RL_LINES_MINOR;
	iColours[i++]	= COLOUR_MATRIX_RL_LINES_MAJOR;
	iColours[i++]	= COLOUR_MATRIX_RL_LINES_SPAN;
	iColours[i++]	= COLOUR_MATRIX_LEVELS_TEXT;
	iColours[i++]	= COLOUR_MATRIX_LEVELS_LINES;
	iColours[i++]	= COLOUR_0700;
	iColours[i++]	= COLOUR_1200;
	iColours[i++]	= COLOUR_1800;
	iColours[i++]	= COLOUR_2400;
	iColours[i++]	= COLOUR_COMMITTED_FULLY;
	iColours[i++]	= COLOUR_COMMITTED_PARTIALLY;

	/////////////////////////////////////////////////////////////////////////////
	ResetColours();

	/////////////////////////////////////////////////////////////////////////////
	m_iColourDeckingPlank		= COLOUR_DECKING_PLANK;
	m_iColourStageBoard			= COLOUR_STAGE_BOARD;
	m_iColourLapboard			= COLOUR_LAPBOARD;
	m_iColourLedger				= COLOUR_LEDGER;
	m_iColourTransom			= COLOUR_TRANSOM;
	m_iColourMeshGuard			= COLOUR_MESH_GUARD;
	m_iColourRail				= COLOUR_RAIL;
	m_iColourMidRail			= COLOUR_MID_RAIL;
	m_iColourToeBoard			= COLOUR_TOE_BOARD;
	m_iColourChainLink			= COLOUR_CHAIN_LINK;
	m_iColourShadeCloth			= COLOUR_SHADE_CLOTH;
	m_iColourTieTubes			= COLOUR_TIE_TUBE;
	m_iColourTieClampColumn		= COLOUR_TIE_CLAMP_COLUMN;
	m_iColourTieClampMasonary	= COLOUR_TIE_CLAMP_MASONARY;
	m_iColourTieClampYoke		= COLOUR_TIE_CLAMP_YOKE;
	m_iColourTieClamp90Deg		= COLOUR_TIE_CLAMP_90DEGREE;
	m_iColourBracing			= COLOUR_BRACING;
	m_iColourLadder				= COLOUR_LADDER;
	m_iColourStair				= COLOUR_STAIR;
	m_iColourStandards			= COLOUR_STANDARD;
	m_iColourJacks				= COLOUR_JACK;
	m_iColourSoleBoards			= COLOUR_SOLEBOARD;
	m_iColourHopupBrackets		= COLOUR_HOPUP_BRACKET;
	m_iColourCornerStageBoard	= COLOUR_CORNER_STAGE_BOARD;
	m_iColourText				= COLOUR_TEXT;
	m_iColourCrosshair			= COLOUR_CROSSHAIR;
	m_iColourCrosshairArc		= COLOUR_CROSSHAIR_ARC;
	m_iColourMatrixRLText		= COLOUR_MATRIX_RL_TEXT;
	m_iColourMatrixRLMinor		= COLOUR_MATRIX_RL_LINES_MINOR;
	m_iColourMatrixRLMajor		= COLOUR_MATRIX_RL_LINES_MAJOR;
	m_iColourMatrixRLSpan		= COLOUR_MATRIX_RL_LINES_SPAN;
	m_iColourMatrixLevelText	= COLOUR_MATRIX_LEVELS_TEXT;
	m_iColourMatrixLevelLines	= COLOUR_MATRIX_LEVELS_LINES;
	m_iColourBay0800			= COLOUR_0700;
	m_iColourBay1200			= COLOUR_1200;
	m_iColourBay1800			= COLOUR_1800;
	m_iColourBay2400			= COLOUR_2400;
	m_iColourCommitted			= COLOUR_COMMITTED_FULLY;
	m_iColourPartialCommitted	= COLOUR_COMMITTED_PARTIALLY;

	/////////////////////////////////////////////////////////////////////////////
	j=0;
	COLOUR_DECKING_PLANK			= iColours[j++];
	COLOUR_STAGE_BOARD				= iColours[j++];
	COLOUR_LAPBOARD					= iColours[j++];
	COLOUR_LEDGER					= iColours[j++];
	COLOUR_TRANSOM					= iColours[j++];
	COLOUR_MESH_GUARD				= iColours[j++];
	COLOUR_RAIL						= iColours[j++];
	COLOUR_MID_RAIL					= iColours[j++];
	COLOUR_TOE_BOARD				= iColours[j++];
	COLOUR_CHAIN_LINK				= iColours[j++];
	COLOUR_SHADE_CLOTH				= iColours[j++];
	COLOUR_TIE_TUBE					= iColours[j++];
	COLOUR_TIE_CLAMP_COLUMN			= iColours[j++];
	COLOUR_TIE_CLAMP_MASONARY		= iColours[j++];
	COLOUR_TIE_CLAMP_YOKE			= iColours[j++];
	COLOUR_TIE_CLAMP_90DEGREE		= iColours[j++];
	COLOUR_BRACING					= iColours[j++];
	COLOUR_STAIR					= iColours[j++];
	COLOUR_LADDER					= iColours[j++];
	COLOUR_STANDARD					= iColours[j++];
	COLOUR_JACK						= iColours[j++];
	COLOUR_SOLEBOARD				= iColours[j++];
	COLOUR_HOPUP_BRACKET			= iColours[j++];
	COLOUR_CORNER_STAGE_BOARD		= iColours[j++];
	COLOUR_TEXT						= iColours[j++];
	COLOUR_TIE_BAR					= iColours[j++];
	COLOUR_STANDARD_CONNECTOR		= iColours[j++];
	COLOUR_LADDER_PUTLOG			= iColours[j++];
	COLOUR_STAIR_RAIL				= iColours[j++];
	COLOUR_STAIR_RAIL_STOPEND		= iColours[j++];
	COLOUR_TOE_BOARD_CLIP			= iColours[j++];
	COLOUR_BOM_EXTRA				= iColours[j++];
	COLOUR_PUTLOG_CLIP				= iColours[j++];
	COLOUR_VISUAL_COMPONENT			= iColours[j++];
	COLOUR_CROSSHAIR				= iColours[j++];
	COLOUR_CROSSHAIR_DEBUG			= iColours[j++];
	COLOUR_CROSSHAIR_ARC			= iColours[j++];
	COLOUR_CROSSHAIR_3D_VIEW		= iColours[j++];
	COLOUR_CROSSHAIR_MATRIX			= iColours[j++];
	COLOUR_MATRIX_RL_TEXT			= iColours[j++];
	COLOUR_MATRIX_RL_LINES_MINOR	= iColours[j++];
	COLOUR_MATRIX_RL_LINES_MAJOR	= iColours[j++];
	COLOUR_MATRIX_RL_LINES_SPAN		= iColours[j++];
	COLOUR_MATRIX_LEVELS_TEXT		= iColours[j++];
	COLOUR_MATRIX_LEVELS_LINES		= iColours[j++];
	COLOUR_0700						= iColours[j++];
	COLOUR_1200						= iColours[j++];
	COLOUR_1800						= iColours[j++];
	COLOUR_2400						= iColours[j++];
	COLOUR_COMMITTED_FULLY			= iColours[j++];
	COLOUR_COMMITTED_PARTIALLY		= iColours[j++];
	assert( j==i );

	FillColours();

	UpdateData( false );
}


void ShowComponentTypeDialog::FillColours()
{
	/////////////////////////////////////////////////////////////////////////////
	m_btnColourTransom				.SetAutoCADColour( m_iColourTransom );
	m_btnColourToeBoard				.SetAutoCADColour( m_iColourToeBoard );
	m_btnColourTieClampYoke			.SetAutoCADColour( m_iColourTieClampYoke );
	m_btnColourTieClampMasonary		.SetAutoCADColour( m_iColourTieClampMasonary );
	m_btnColourTieClampColumn		.SetAutoCADColour( m_iColourTieClampColumn );
	m_btnColourTieClamp90Deg		.SetAutoCADColour( m_iColourTieClamp90Deg );
	m_btnColourTieBar				.SetAutoCADColour( m_iColourTieBar );
	m_btnColourStandardsOpenEnd		.SetAutoCADColour( m_iColourStandardsOpenEnd );
	m_btnColourStandards			.SetAutoCADColour( m_iColourStandards );
	m_btnColourStair				.SetAutoCADColour( m_iColourStair );
	m_btnColourStageBoard			.SetAutoCADColour( m_iColourStageBoard );
	m_btnColourSoleBoards			.SetAutoCADColour( m_iColourSoleBoards );
	m_btnColourMidRail				.SetAutoCADColour( m_iColourMidRail );
	m_btnColourShadeCloth			.SetAutoCADColour( m_iColourShadeCloth );
	m_btnColourRail					.SetAutoCADColour( m_iColourRail );
	m_btnColourMeshGuard			.SetAutoCADColour( m_iColourMeshGuard );
	m_btnColourLedger				.SetAutoCADColour( m_iColourLedger );
	m_btnColourJacks				.SetAutoCADColour( m_iColourJacks );
	m_btnColourHopupBrackets		.SetAutoCADColour( m_iColourHopupBrackets );
	m_btnColourDeckingPlank			.SetAutoCADColour( m_iColourDeckingPlank );
	m_btnColourCornerStageBoard		.SetAutoCADColour( m_iColourCornerStageBoard );
	m_btnColourChainLink			.SetAutoCADColour( m_iColourChainLink );
	m_btnColourBracing				.SetAutoCADColour( m_iColourBracing );
	m_btnColourStairRail			.SetAutoCADColour( m_iColourStairRail ); 
	m_btnColourStairRailStopend		.SetAutoCADColour( m_iColourStairRailStopend );
	m_btnColourStandardConnector	.SetAutoCADColour( m_iColourStandardConnector );
	m_btnColourToeBoardClip			.SetAutoCADColour( m_iColourToeBoardClip );
	m_btnColourText					.SetAutoCADColour( m_iColourText );
	m_btnColourLadder				.SetAutoCADColour( m_iColourLadder );
	m_btnColourLadderPutlog			.SetAutoCADColour( m_iColourLadderPutlog );
	m_btnColourLapboard				.SetAutoCADColour( m_iColourLapboard );
	m_btnColourTieTubes				.SetAutoCADColour( m_iColourTieTubes );
	m_btnColourPartialCommitted		.SetAutoCADColour( m_iColourPartialCommitted );
	m_btnColourMatrixRLText			.SetAutoCADColour( m_iColourMatrixRLText );
	m_btnColourMatrixRLSpan			.SetAutoCADColour( m_iColourMatrixRLSpan );
	m_btnColourMatrixRLMinor		.SetAutoCADColour( m_iColourMatrixRLMinor );
	m_btnColourMatrixRLMajor		.SetAutoCADColour( m_iColourMatrixRLMajor );
	m_btnColourMatrixLevelText		.SetAutoCADColour( m_iColourMatrixLevelText );
	m_btnColourMatrixLevelLines		.SetAutoCADColour( m_iColourMatrixLevelLines );
	m_btnColourCrosshairArc			.SetAutoCADColour( m_iColourCrosshairArc );
	m_btnColourCrosshair			.SetAutoCADColour( m_iColourCrosshair );
	m_btnColourCommitted			.SetAutoCADColour( m_iColourCommitted );
	m_btnColourBay2400				.SetAutoCADColour( m_iColourBay2400 );
	m_btnColourBay1800				.SetAutoCADColour( m_iColourBay1800 );
	m_btnColourBay0800				.SetAutoCADColour( m_iColourBay0800 );
	m_btnColourBay1200				.SetAutoCADColour( m_iColourBay1200 );

	/////////////////////////////////////////////////////////////////////////////
	m_btnColourTransom.RedrawWindow();
	m_btnColourToeBoard.RedrawWindow();
	m_btnColourTieClampYoke.RedrawWindow();
	m_btnColourTieClampMasonary.RedrawWindow();
	m_btnColourTieClampColumn.RedrawWindow();
	m_btnColourTieClamp90Deg.RedrawWindow();
	m_btnColourTieBar.RedrawWindow();
	m_btnColourStandardsOpenEnd.RedrawWindow();
	m_btnColourStandards.RedrawWindow();
	m_btnColourStair.RedrawWindow();
	m_btnColourStageBoard.RedrawWindow();
	m_btnColourSoleBoards.RedrawWindow();
	m_btnColourMidRail.RedrawWindow();
	m_btnColourShadeCloth.RedrawWindow();
	m_btnColourRail.RedrawWindow();
	m_btnColourMeshGuard.RedrawWindow();
	m_btnColourLedger.RedrawWindow();
	m_btnColourJacks.RedrawWindow();
	m_btnColourHopupBrackets.RedrawWindow();
	m_btnColourDeckingPlank.RedrawWindow();
	m_btnColourCornerStageBoard.RedrawWindow();
	m_btnColourChainLink.RedrawWindow();
	m_btnColourBracing.RedrawWindow();
	m_btnColourStairRail.RedrawWindow();
	m_btnColourStairRailStopend.RedrawWindow();
	m_btnColourStandardConnector.RedrawWindow();
	m_btnColourToeBoardClip.RedrawWindow();
	m_btnColourText.RedrawWindow();
	m_btnColourLadder.RedrawWindow();
	m_btnColourLadderPutlog.RedrawWindow();
	m_btnColourLapboard.RedrawWindow();
	m_btnColourTieTubes.RedrawWindow();
	m_btnColourPartialCommitted.RedrawWindow();
	m_btnColourMatrixRLText.RedrawWindow();
	m_btnColourMatrixRLSpan.RedrawWindow();
	m_btnColourMatrixRLMinor.RedrawWindow();
	m_btnColourMatrixRLMajor.RedrawWindow();
	m_btnColourMatrixLevelText.RedrawWindow();
	m_btnColourMatrixLevelLines.RedrawWindow();
	m_btnColourCrosshairArc.RedrawWindow();
	m_btnColourCrosshair.RedrawWindow();
	m_btnColourCommitted.RedrawWindow();
	m_btnColourBay2400.RedrawWindow();
	m_btnColourBay1800.RedrawWindow();
	m_btnColourBay0800.RedrawWindow();
	m_btnColourBay1200.RedrawWindow();
}
