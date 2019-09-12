// PerRollDialog.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "PerRollDialog.h"
#include "Controller.h"
#include "ComponentPartNumberSelectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// PerRollDialog dialog


PerRollDialog::PerRollDialog( Controller *pController, CWnd* pParent /*=NULL*/)
	: CDialog(PerRollDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(PerRollDialog)
	m_sAreaPerRoll = _T("");
	m_sMainLabel = _T("");
	m_iCalculationType = -1;
	m_sProductCode = _T("");
	//}}AFX_DATA_INIT
	;//assert( pController!=NULL );
	m_pController = pController;
	SetShadeCloth(true);

	saRoundingTexts.Add( _T("roll") );
	saRoundingTexts.Add( _T("half a roll") );
	saRoundingTexts.Add( _T("quater of a roll") );
	saRoundingTexts.Add( _T("fifth of a roll") );
	saRoundingTexts.Add( _T("tenth of a roll") );
	saRoundingTexts.Add( _T("hundredth of a roll") );
}


void PerRollDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PerRollDialog)
	DDX_Control(pDX, IDC_PRODUCT_CODE, m_editProductCode);
	DDX_Control(pDX, IDC_ROUNDING_DEVISOR, m_comboRoundingDevisor);
	DDX_Control(pDX, IDC_AREA_SPINNER, m_spinAreaSpinner);
	DDX_Control(pDX, IDC_AREA_PER_ROLL, m_editAreaPerRoll);
	DDX_Text(pDX, IDC_AREA_PER_ROLL, m_sAreaPerRoll);
	DDX_Text(pDX, IDC_MAIN_LABEL, m_sMainLabel);
	DDX_Radio(pDX, IDC_CALC_PER_METRE, m_iCalculationType);
	DDX_Text(pDX, IDC_PRODUCT_CODE, m_sProductCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PerRollDialog, CDialog)
	//{{AFX_MSG_MAP(PerRollDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AREA_SPINNER, OnDeltaposAreaSpinner)
	ON_BN_CLICKED(IDC_CALC_PER_METRE, OnCalcPerMetre)
	ON_BN_CLICKED(IDC_CALC_PER_ROLL, OnCalcPerRoll)
	ON_BN_CLICKED(IDC_SELECT_PART_NUMBER, OnSelectPartNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PerRollDialog message handlers

void PerRollDialog::OnDeltaposAreaSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	double dArea;

	UpdateData();
	dArea = _tstof(m_sAreaPerRoll);
	dArea+= -1.0*pNMUpDown->iDelta;
	m_sAreaPerRoll.Format( _T("%0.1f"), dArea );
	UpdateData(false);
	*pResult = 0;
}

void PerRollDialog::OnCancel() 
{
	bool	bUseRoll, bChangesMade;
	double	dArea;
	RoundingDevisorEnum		eRD;

	eRD			= (RoundingDevisorEnum)m_comboRoundingDevisor.GetItemData(
											m_comboRoundingDevisor.GetCurSel() );
	dArea		= _tstof(m_sAreaPerRoll);
	bUseRoll	= (m_eCalculationType==CALC_PER_ROLL);

	bChangesMade = false;
	if( m_bShadeCloth )
	{
		if( m_pController->CalcPerRollShadeCloth()!=bUseRoll )
			bChangesMade = true;
		
		if( gpController->GetRollSizeShadeCloth()<dArea-ROUND_ERROR ||
			gpController->GetRollSizeShadeCloth()>dArea+ROUND_ERROR )
			bChangesMade = true;

		if( m_pController->GetPartNumberShadeCloth()!=m_sProductCode )
			bChangesMade = true;

		if( eRD!=m_pController->GetRoundingDivisorShadeCloth() )
			bChangesMade = true;
	}
	else
	{
		if( m_pController->CalcPerRollChainMesh()!=bUseRoll )
			bChangesMade = true;
		
		if( gpController->GetRollSizeChainMesh()<dArea-ROUND_ERROR ||
			gpController->GetRollSizeChainMesh()>dArea+ROUND_ERROR )
			bChangesMade = true;

		if( m_pController->GetPartNumberChainMesh()!=m_sProductCode )
			bChangesMade = true;

		if( eRD!=m_pController->GetRoundingDivisorChainMesh() )
			bChangesMade = true;
	}

	if( bChangesMade )
	{
		CString sMsg;
		sMsg = _T("Your changes will be lost.  Are you sure you want to cancel?");
		if( MessageBox( sMsg, _T("Warning!"), MB_YESNO|MB_ICONWARNING )==IDNO )
			return;
	}
	
	CDialog::OnCancel();
}

void PerRollDialog::OnOK() 
{
	int						iIndex;
	bool					bUseRoll;
	double					dArea;
	RoundingDevisorEnum		eRD;

	UpdateData();

	///////////////////////////////////////////////////////////////////////
	//Convert data
	bUseRoll	= (m_eCalculationType==CALC_PER_ROLL);
	dArea		= _tstof(m_sAreaPerRoll);
	iIndex		= m_comboRoundingDevisor.GetCurSel();
	eRD			= (RoundingDevisorEnum)m_comboRoundingDevisor.GetItemData( iIndex );

	///////////////////////////////////////////////////////////////////////
	//check data limits
	if( bUseRoll )
	{
		if( dArea<ROUND_ERROR )
		{
			CString sMsg;
			sMsg.Format( _T("The area(m2) per roll must be greater than %0.1fm2"), ROUND_ERROR );
			MessageBox( sMsg, _T("Area Invalid") );
			return;
		}
		if( m_sProductCode.IsEmpty() )
		{
			CString sMsg;
			sMsg = _T("The product code cannot be null");
			MessageBox( sMsg, _T("Product Code Invalid") );
			return;
		}
	}

	///////////////////////////////////////////////////////////////////////
	//write back to controller
	if( m_bShadeCloth )
	{
		m_pController->SetCalcPerRollShadeCloth( bUseRoll );
		gpController->SetRollSizeShadeCloth( dArea );
		m_pController->SetPartNumberShadeCloth( m_sProductCode );
		m_pController->SetRoundingDivisorShadeCloth(eRD);
	}
	else
	{
		m_pController->SetCalcPerRollChainMesh( bUseRoll );
		gpController->SetRollSizeChainMesh( dArea );
		m_pController->SetPartNumberChainMesh( m_sProductCode );
		m_pController->SetRoundingDivisorChainMesh(eRD);
	}

	CDialog::OnOK();
}

BOOL PerRollDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillDialog();

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PerRollDialog::FillDialog()
{
	int						i;
	RoundingDevisorEnum		eRD;
	RoundingListOrderEnum	eRLO;
	m_comboRoundingDevisor.Clear();
	m_comboRoundingDevisor.ResetContent();

	for( i=0; i<saRoundingTexts.GetSize(); i++ )
	{
		m_comboRoundingDevisor.AddString( saRoundingTexts.GetAt(i) );
		m_comboRoundingDevisor.SetItemData( i, (DWORD)m_pController->ConvertToDevisor( (RoundingListOrderEnum)i ) );
	}
	;//assert( m_comboRoundingDevisor.GetCount()==saRoundingTexts.GetSize() );

	;//assert( m_pController!=NULL );
	if( m_bShadeCloth )
	{
		m_sMainLabel = _T("The price of Shade Cloth is calculated");
		SetWindowText( _T("Shade Cloth Price Calculation") );
		m_sAreaPerRoll.Format( _T("%0.1f"), gpController->GetRollSizeShadeCloth() );
		m_sProductCode = m_pController->GetPartNumberShadeCloth();
		if( m_sProductCode.IsEmpty() )
			m_sProductCode = _T("147000");
		m_eCalculationType = m_pController->CalcPerRollShadeCloth()? CALC_PER_ROLL: CALC_PER_METRE;
		eRD = m_pController->GetRoundingDivisorShadeCloth();
	}
	else
	{
		m_sMainLabel = _T("The price of Chain Mesh is calculated");
		SetWindowText( _T("Chain Mesh Price Calculation") );
		m_sAreaPerRoll.Format( _T("%0.1f"), gpController->GetRollSizeChainMesh() );
		m_sProductCode = m_pController->GetPartNumberChainMesh();
		if( m_sProductCode.IsEmpty() )
			m_sProductCode = _T("147003");
		m_eCalculationType = m_pController->CalcPerRollChainMesh()? CALC_PER_ROLL: CALC_PER_METRE;
		eRD = m_pController->GetRoundingDivisorChainMesh();
	}
	eRLO = m_pController->ConvertToListOrder( eRD );
	m_comboRoundingDevisor.SelectString( 0, saRoundingTexts[(int)eRLO] );
	m_iCalculationType = (int)m_eCalculationType;

	AdjustControls();
}

void PerRollDialog::SetShadeCloth(bool bShadeCloth)
{
	m_bShadeCloth = bShadeCloth;
}

void PerRollDialog::AdjustControls()
{
	m_editProductCode.EnableWindow(false);
	m_spinAreaSpinner.EnableWindow(false);
	m_editAreaPerRoll.EnableWindow(false);
	m_comboRoundingDevisor.EnableWindow(false);
	if( m_eCalculationType==CALC_PER_ROLL )
	{
		m_editProductCode.EnableWindow(true);
		m_spinAreaSpinner.EnableWindow(true);
		m_editAreaPerRoll.EnableWindow(true);
		m_comboRoundingDevisor.EnableWindow(true);
	}
}

void PerRollDialog::OnCalcPerMetre() 
{
	CalcTypeChanged();
}

void PerRollDialog::OnCalcPerRoll() 
{
	CalcTypeChanged();
}

void PerRollDialog::CalcTypeChanged()
{
	UpdateData();
	m_eCalculationType = (CalculationTypeEnum)m_iCalculationType;
	AdjustControls();
	UpdateData( false );
}

void PerRollDialog::OnSelectPartNumber() 
{
	CString							sPartNumber;
	ComponentPartNumberSelectorDlg	Dlg;

	Dlg.SetPartNumber( m_sProductCode );
	Dlg.SetAllowNonVisual(true);
	if( Dlg.DoModal()==IDOK )
	{
		Dlg.GetPartNumber( sPartNumber );
		if( !sPartNumber.IsEmpty() )
		{
			m_sProductCode = sPartNumber;
			UpdateData(false);
		}
	}
}
