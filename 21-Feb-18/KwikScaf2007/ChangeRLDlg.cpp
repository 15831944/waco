// hangeRLDlg.cpp : implementation file
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
#include "ChangeRLDlg.h"
#include "Bay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const double RL_LIMIT_TOP = 45000.00;
const double RL_LIMIT_BOTTOM = -45000.00;

/////////////////////////////////////////////////////////////////////////////
// ChangeRLDlg dialog


ChangeRLDlg::ChangeRLDlg(CWnd* pParent /*=NULL*/)
	: KwikscafDialog(ChangeRLDlg::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(ChangeRLDlg)
	m_sBayNumberEnd = _T("");
	m_sBayNumberStart = _T("");
	m_sBayLengthEnd = _T("");
	m_sBayLengthStart = _T("");
	m_sRLEnd = _T("");
	m_sRLStart = _T("");
	m_sSeparationDistance = _T("");
	m_sBaysSelectedLbl = _T("");
	//}}AFX_DATA_INIT

	m_pBays = NULL;
	m_pdLastValue = NULL;
}


void ChangeRLDlg::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ChangeRLDlg)
	DDX_Control(pDX, IDC_RL_START, m_ctrlRLStart);
	DDX_Control(pDX, IDC_RL_END, m_ctrlRLEnd);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_BAY_NUMBER_END_LBL, m_sBayNumberEnd);
	DDX_Text(pDX, IDC_BAY_NUMBER_START_LBL, m_sBayNumberStart);
	DDX_Text(pDX, IDC_BAY_WIDTH_END_LBL, m_sBayLengthEnd);
	DDX_Text(pDX, IDC_BAY_WIDTH_START_LBL, m_sBayLengthStart);
	DDX_Text(pDX, IDC_RL_END, m_sRLEnd);
	DDX_Text(pDX, IDC_RL_START, m_sRLStart);
	DDX_Text(pDX, IDC_SEPARATION_DISTANCE_LBL, m_sSeparationDistance);
	DDX_Text(pDX, IDC_BAYS_SELECTED_LBL, m_sBaysSelectedLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ChangeRLDlg, KwikscafDialog)
	//{{AFX_MSG_MAP(ChangeRLDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RL_END_SPIN, OnDeltaposRlEndSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RL_START_SPIN, OnDeltaposRlStartSpin)
	ON_EN_KILLFOCUS(IDC_RL_START, OnKillfocusRlStart)
	ON_EN_KILLFOCUS(IDC_RL_END, OnKillfocusRlEnd)
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_RL_START, OnChangeRlStart)
	ON_EN_CHANGE(IDC_RL_END, OnChangeRlEnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChangeRLDlg message handlers

BOOL ChangeRLDlg::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("ChangeRLDlg"));

	;//assert( m_pBays!=NULL );	//You must call 'SetBayList' first
	;//assert( m_pdLastValue!=NULL );	//You must call 'SetLastValue' first

	/////////////////////////////////////////////////////
	//calculate the Length of the bays
	int		i, iBayNumberStart, iBayNumberEnd;
	Bay		*pBay;
	double	dLength, dRL;

	m_dSeparation = 0.00;
	;//assert( m_pBays->GetSize()>0 );
	for( i=0; i<m_pBays->GetSize(); i++ )
	{
		pBay = m_pBays->GetAt(i);
		;//assert( pBay!=NULL );
		m_dSeparation+= pBay->GetBayLengthActual();
	}
	;//assert( m_dSeparation>0 );
	m_sSeparationDistance.Format( _T("%imm"), (int)m_dSeparation );

	////////////////////////////////////////////////////
	//Set the start bay details
	pBay = m_pBays->GetAt(0);
	//bay number
	iBayNumberStart = pBay->GetBayNumber();
	m_sBayNumberStart.Format( _T("%i"), iBayNumberStart );
	//bay width
	dLength = pBay->GetBayLength();
	if( dLength>=COMPONENT_LENGTH_0700+ROUND_ERROR && 
		dLength<=COMPONENT_LENGTH_0700-ROUND_ERROR )
		dLength = COMPONENT_LENGTH_0800;

	m_sBayLengthStart.Format( _T("%01.1fm"), dLength*CONVERT_MM_TO_M );
	//RL
	dRL = pBay->GetStandardPosition( CNR_SOUTH_WEST ).z;
	m_sRLStart.Format( _T("%i"), (int)ConvertStarRLtoRL(dRL, GetStarSeparation()) );

	////////////////////////////////////////////////////
	//Set the end bay details
	pBay = m_pBays->GetAt(m_pBays->GetSize()-1);
	//bay number
	iBayNumberEnd = pBay->GetBayNumber();
	m_sBayNumberEnd.Format( _T("%i"), iBayNumberEnd );
	//bay width
	dLength = pBay->GetBayLength();
	if( dLength>=COMPONENT_LENGTH_0700+ROUND_ERROR && 
		dLength<=COMPONENT_LENGTH_0700-ROUND_ERROR )
		dLength = COMPONENT_LENGTH_0800;

	m_sBayLengthEnd.Format( _T("%01.1fm"), dLength*CONVERT_MM_TO_M );
	//RL
	dRL = pBay->GetStandardPosition( CNR_SOUTH_EAST ).z;
	m_sRLEnd.Format( _T("%i"), (int)ConvertStarRLtoRL(dRL, GetStarSeparation()) );

	////////////////////////////////////////////////////
	//Label at top of dialog
  m_sBaysSelectedLbl.Format( _T("Change RLs for Bays: %i -> %i."), iBayNumberStart, iBayNumberEnd );

	UpdateData( false );

	m_ctrlRLStart.SetFocus();
	return FALSE;
/*	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
*/
}

void ChangeRLDlg::OnDeltaposRlEndSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData();

	double dRL;
	dRL	= ConvertRLtoStarRL(_tstof( m_sRLEnd ), GetStarSeparation());
	dRL+= -GetStarSeparation()*pNMUpDown->iDelta;
	m_sRLEnd.Format( _T("%i"), (int)ConvertStarRLtoRL(dRL, GetStarSeparation()) );
	*m_pdLastValue = dRL;

	UpdateData( false );
	
	*pResult = 0;
}

void ChangeRLDlg::OnDeltaposRlStartSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData();

	double dRL;
	dRL	= ConvertRLtoStarRL(_tstof( m_sRLStart ), GetStarSeparation());
	dRL+= -GetStarSeparation()*pNMUpDown->iDelta;
	m_sRLStart.Format( _T("%i"), (int)ConvertStarRLtoRL(dRL, GetStarSeparation()) );
	*m_pdLastValue = dRL;

	UpdateData( false );
	
	*pResult = 0;
}

void ChangeRLDlg::OnKillfocusRlStart() 
{
	if( !CheckAndWarnForRL(true) )
		//don't allow them to loose focus if data is invalid
		m_ctrlRLStart.SetFocus();
}

void ChangeRLDlg::OnKillfocusRlEnd() 
{
	if( !CheckAndWarnForRL(false) )
		//don't allow them to loose focus if data is invalid
		m_ctrlRLEnd.SetFocus();

}

bool ChangeRLDlg::CheckAndWarnForRL(bool bStart)
{
	UpdateData();

	int		i, iSize;
	TCHAR	cLetter;
	double	dRL;

	if( bStart )
		iSize = m_sRLStart.GetLength();
	else
		iSize = m_sRLEnd.GetLength();

	for( i=0; i<iSize; i++ )
	{
		if( bStart )
			cLetter = m_sRLStart[i];
		else
			cLetter = m_sRLEnd[i];

		if( !_istdigit( cLetter ) && cLetter!=_T('-') &&
			cLetter!=_T('.') )
		{
			//invalid character in the RL
			CString sMsg;
			sMsg = _T("Please enter a number for this RL");
			MessageBeep( MB_ICONEXCLAMATION );
			MessageBox( sMsg, _T("Invalid Data"), MB_OK );
			return false;
		}
	}

	if( bStart )
		dRL	= ConvertRLtoStarRL(_tstof( m_sRLStart ), GetStarSeparation());
	else
		dRL	= ConvertRLtoStarRL(_tstof( m_sRLEnd ), GetStarSeparation());

	if( dRL<RL_LIMIT_BOTTOM || dRL>RL_LIMIT_TOP )
	{
		//invalid character in the RL
		CString sMsg;
		sMsg.Format( _T("Please enter a number between -%1.0fmm and %1.0fmm."),
						RL_LIMIT_BOTTOM, RL_LIMIT_TOP );
		MessageBeep( MB_ICONEXCLAMATION );
		MessageBox( sMsg, _T("Invalid value"), MB_OK );
	
		return false;
	}

	return true;
}

void ChangeRLDlg::OnOK() 
{
	int	i;
	Bay	*pBay, *pNeighbor;
	double	dRLs[4], dRLStart, dRLEnd, dSlope, dRLBayStart, dRLBayEnd, dRLChangeBay, dRLChangeBays;

	UpdateData();

	dRLStart	= ConvertRLtoStarRL(_tstof( m_sRLStart ), GetStarSeparation());
	dRLEnd		= ConvertRLtoStarRL(_tstof( m_sRLEnd ), GetStarSeparation());

	dRLChangeBays	= dRLEnd-dRLStart;
	dRLBayStart		= dRLStart;
	dRLBayEnd		= 0.00;
	;//assert( m_dSeparation );
	dSlope			= dRLChangeBays/m_dSeparation;

	//run entirely through first to ensure that they will all fit!
	for( i=0; i<m_pBays->GetSize(); i++ )
	{
		pBay = m_pBays->GetAt(i);
		;//assert( pBay!=NULL );

		//calculate the end RL for this bay.  We know
		//	the start RL, the bay length, and the slope!
		dRLChangeBay	= pBay->GetBayLengthActual()*dSlope;
		dRLBayEnd		= dRLBayStart + dRLChangeBay;

		if( !CheckRLisValidForBay( pBay, dRLBayStart, dRLBayEnd ) )
			return;

		//The end of this bay is the begining of the next bay
		dRLBayStart		= dRLBayEnd;
	}
	
	//the RL's check out apply the changes
	dRLBayStart		= dRLStart;
	dRLBayEnd		= 0.00;
	for( i=0; i<m_pBays->GetSize(); i++ )
	{
		pBay = m_pBays->GetAt(i);
		;//assert( pBay!=NULL );

		//calculate the end RL for this bay.  We know
		//	the start RL, the bay length, and the slope!
		dRLChangeBay	= pBay->GetBayLengthActual()*dSlope;
		dRLBayEnd		= dRLBayStart + dRLChangeBay;

		//apply the changes to the bay
		dRLs[CNR_NORTH_EAST] = dRLBayEnd;
		dRLs[CNR_SOUTH_EAST] = dRLBayEnd;
		dRLs[CNR_SOUTH_WEST] = dRLBayStart;
		dRLs[CNR_NORTH_WEST] = dRLBayStart;
		if( !pBay->SetAndAdjustRLs( dRLs ) )
		{
			;//assert( false );	//the check above should have caught this one!
			return;
		}

		pNeighbor = pBay->GetOuter();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pBay->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pBay->GetStandardPosition(CNR_NORTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}

		pNeighbor = pBay->GetInner();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pBay->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pBay->GetStandardPosition(CNR_SOUTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}

		}

		//The end of this bay is the begining of the next bay
		dRLBayStart		= dRLBayEnd;
	}

	pBay = m_pBays->GetAt(0);
	pNeighbor = pBay->GetBackward();
	if( pNeighbor!=NULL )
	{
		dRLs[CNR_NORTH_EAST] = pBay->GetStandardPosition(CNR_NORTH_WEST).z;
		dRLs[CNR_SOUTH_EAST] = pBay->GetStandardPosition(CNR_SOUTH_WEST).z;
		dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
		dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
		if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
		{
			;//assert( false );	//the check above should have caught this one!
			return;
		}

		pNeighbor = pBay->GetBackward()->GetInner();
		if( pNeighbor==NULL && pBay->GetInner()!=NULL )
			pNeighbor = pBay->GetInner()->GetBackward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pBay->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}

		pNeighbor = pBay->GetBackward()->GetOuter();
		if( pNeighbor==NULL && pBay->GetOuter()!=NULL )
			pNeighbor = pBay->GetOuter()->GetBackward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pBay->GetStandardPosition(CNR_NORTH_WEST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}
	}
	else
	{
		pNeighbor=NULL;
		if( pBay->GetInner()!=NULL )
			pNeighbor = pBay->GetInner()->GetBackward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pBay->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}

		pNeighbor=NULL;
		if( pBay->GetOuter()!=NULL )
			pNeighbor = pBay->GetOuter()->GetBackward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pBay->GetStandardPosition(CNR_NORTH_WEST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}
	}

	pBay = m_pBays->GetAt(m_pBays->GetSize()-1);
	pNeighbor = pBay->GetForward();
	if( pNeighbor!=NULL )
	{
		dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
		dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
		dRLs[CNR_SOUTH_WEST] = pBay->GetStandardPosition(CNR_SOUTH_EAST).z;
		dRLs[CNR_NORTH_WEST] = pBay->GetStandardPosition(CNR_NORTH_EAST).z;
		if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
		{
			;//assert( false );	//the check above should have caught this one!
			return;
		}

		pNeighbor = pBay->GetForward()->GetInner();
		if( pNeighbor==NULL && pBay->GetInner()!=NULL )
			pNeighbor = pBay->GetInner()->GetForward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pBay->GetStandardPosition(CNR_SOUTH_EAST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}

		pNeighbor = pBay->GetForward()->GetOuter();
		if( pNeighbor==NULL && pBay->GetOuter()!=NULL )
			pNeighbor = pBay->GetOuter()->GetForward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pBay->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}
	}
	else
	{
		pNeighbor = NULL;
		if( pBay->GetInner()!=NULL )
			pNeighbor = pBay->GetInner()->GetForward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pNeighbor->GetStandardPosition(CNR_SOUTH_WEST).z;
			dRLs[CNR_NORTH_WEST] = pBay->GetStandardPosition(CNR_SOUTH_EAST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}

		pNeighbor = NULL;
		if( pBay->GetOuter()!=NULL )
			pNeighbor = pBay->GetOuter()->GetForward();
		if( pNeighbor!=NULL )
		{
			dRLs[CNR_NORTH_EAST] = pNeighbor->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_SOUTH_EAST] = pNeighbor->GetStandardPosition(CNR_SOUTH_EAST).z;
			dRLs[CNR_SOUTH_WEST] = pBay->GetStandardPosition(CNR_NORTH_EAST).z;
			dRLs[CNR_NORTH_WEST] = pNeighbor->GetStandardPosition(CNR_NORTH_WEST).z;
			if( !pNeighbor->SetAndAdjustRLs( dRLs ) )
			{
				;//assert( false );	//the check above should have caught this one!
				return;
			}
		}
	}

	KwikscafDialog::OnOK();
}

void ChangeRLDlg::SetBayList(BayList *pBays)
{
	m_pBays = pBays;
}

void ChangeRLDlg::SetLastValue(double *pdLastValue)
{
	m_pdLastValue = pdLastValue;
}

bool ChangeRLDlg::CheckRLisValidForBay( Bay *pBay, double dRLStart, double dRLEnd )
{
	double	dRLTopLift;

	dRLTopLift = pBay->GetRLOfLift( pBay->GetNumberOfLifts()-1 );
	if( dRLTopLift<=dRLStart || dRLTopLift<=dRLEnd )
	{
		CString sErrMsg, sTemp;
		MessageBeep(MB_ICONEXCLAMATION);
		sErrMsg.Format( _T("You have set the RLs to be up to %1.1fm high!\n"),
										max( dRLStart, dRLEnd) );
		sTemp.Format( _T("However the top bay height for bay %i is %1.1fm"),
						pBay->GetBayNumber(), dRLTopLift );
		sErrMsg+=sTemp;
		sErrMsg+= _T("\n\nPlease choose appropriate RL's and try again.");

		MessageBox( sErrMsg, ERROR_MSG_INVALID_RL, MB_OK );
		return false;
	}
	return true;
}

void ChangeRLDlg::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("ChangeRLDlg") );
}


BOOL ChangeRLDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_MBUTTONDOWN )
	{
		RECT Boundary;
		m_ctrlRLStart.GetWindowRect( &Boundary );
		if( pMsg->pt.x>=Boundary.left && pMsg->pt.x<=Boundary.right &&
			pMsg->pt.y<=Boundary.bottom && pMsg->pt.y>=Boundary.top )
		{
			m_sRLStart.Format( _T("%i"), (int)ConvertStarRLtoRL(*m_pdLastValue, GetStarSeparation()) );
		}

		m_ctrlRLEnd.GetWindowRect( &Boundary );
		if( pMsg->pt.x>=Boundary.left && pMsg->pt.x<=Boundary.right &&
			pMsg->pt.y<=Boundary.bottom && pMsg->pt.y>=Boundary.top )
		{
			m_sRLEnd.Format( _T("%i"), (int)ConvertStarRLtoRL(*m_pdLastValue, GetStarSeparation()) );
		}

		UpdateData(false);
	}
	
	return KwikscafDialog::PreTranslateMessage(pMsg);
}

void ChangeRLDlg::OnChangeRlStart() 
{
	UpdateData();
	*m_pdLastValue = ConvertRLtoStarRL(_tstof( m_sRLStart ), GetStarSeparation());
}

void ChangeRLDlg::OnChangeRlEnd() 
{
	UpdateData();
	*m_pdLastValue = ConvertRLtoStarRL(_tstof( m_sRLEnd ), GetStarSeparation());
}

double ChangeRLDlg::GetStarSeparation() const
{
	;//assert( m_pBays->GetAt(0)!=NULL );
	return m_pBays->GetAt(0)->GetStarSeparation();
}
