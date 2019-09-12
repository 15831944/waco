// KwikscafDialog.cpp: implementation of the KwikscafDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "KwikscafDialog.h"
#include "MeccanoDefinitions.h"

#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afxdisp.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KwikscafDialog::KwikscafDialog()
{
	m_bAllowWinPosSave = false;
}

KwikscafDialog::KwikscafDialog(UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/ )
	: CDialog( nIDTemplate, pParentWnd )
{
	m_bAllowWinPosSave = false;
}

KwikscafDialog::~KwikscafDialog()
{
}

void KwikscafDialog::GetWindowPositionInReg(CString sWindowName)
{
	CRect	rWindowPos;
	CPoint	ptTopLeft;

	;//assert( !sWindowName.IsEmpty() );
	ptTopLeft.x = GetIntInRegistry( sWindowName+_T("_X"), WINDOW_POSTION_X_DEFAULT );
	ptTopLeft.y = GetIntInRegistry( sWindowName+_T("_Y"), WINDOW_POSTION_Y_DEFAULT );

	if( !(ptTopLeft.x==-1 && ptTopLeft.y==-1) )
	{
		CDialog::GetWindowRect( &rWindowPos );
		//rWindowPos.TopLeft() = ptTopLeft;
		CDialog::SetWindowPos( NULL, ptTopLeft.x, ptTopLeft.y, rWindowPos.Width(), rWindowPos.Height(), SWP_SHOWWINDOW );
	}
}

void KwikscafDialog::StoreWindowPositionInReg(CString sWindowName)
{
	;//assert( !sWindowName.IsEmpty() );
	if( m_bAllowWinPosSave )
	{
		CRect	rWindowPos;
		CPoint	ptTopLeft;

		CDialog::GetWindowRect( &rWindowPos );
		ptTopLeft = rWindowPos.TopLeft();
		//acutPrintf( "\nWindowPos(%i,%i)", ptTopLeft.x, ptTopLeft.y );
		SetIntInRegistry( sWindowName+_T("_X"), (int)ptTopLeft.x );
		SetIntInRegistry( sWindowName+_T("_Y"), (int)ptTopLeft.y );
	}
}


BOOL KwikscafDialog::OnInitDialog()
{
	BOOL bReturn;
	AfxEnableControlContainer();
	bReturn = CDialog::OnInitDialog();
	m_bAllowWinPosSave = true;

	return bReturn;
}


/*
	KwikscafDialog
#include "KwikscafDialog.h"
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg("");
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( "" );
*/	

