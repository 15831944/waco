// AutobuildPropertiesWnd.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "AutoBuildTools.h"
#include "BayDetailsDialog.h"
#include "RunPropDialog.h"
#include "AutobuildPropertiesToolbar.h"
#include "AutobuildPropertiesWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesWnd

AutobuildPropertiesWnd::AutobuildPropertiesWnd()
{
	m_pABTools = NULL;
}

AutobuildPropertiesWnd::~AutobuildPropertiesWnd()
{
}


BEGIN_MESSAGE_MAP(AutobuildPropertiesWnd, CWnd)
	//{{AFX_MSG_MAP(AutobuildPropertiesWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesWnd message handlers

BOOL AutobuildPropertiesWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	assert( m_pABTools!=NULL );

	BayDetailsDialog	BayDetails(NULL, m_pABTools->GetABBay());
	RunPropDialog		*pRunPropDlg;

	switch(wParam)
	{
		case ID_AUTOBUILD_BAY_DETAILS_BUTTON:
			BayDetails.DoModal();
			break;

		case ID_RUN_DETAILS_BUTTON:
			pRunPropDlg = new RunPropDialog(this, &BayDetails, m_pABTools);
			pRunPropDlg->m_dLength2400 = m_pABTools->GetBracingLength( COMPONENT_LENGTH_2400 );
			pRunPropDlg->m_dLength1800 = m_pABTools->GetBracingLength( COMPONENT_LENGTH_1800 );
			pRunPropDlg->m_dLength1200 = m_pABTools->GetBracingLength( COMPONENT_LENGTH_1200 );
			pRunPropDlg->m_dLength0700 = m_pABTools->GetBracingLength( COMPONENT_LENGTH_0700 );
			pRunPropDlg->DoModal();
			m_pABTools->SetBracingLength( COMPONENT_LENGTH_2400, pRunPropDlg->m_dLength2400 );
			m_pABTools->SetBracingLength( COMPONENT_LENGTH_1800, pRunPropDlg->m_dLength1800 );
			m_pABTools->SetBracingLength( COMPONENT_LENGTH_1200, pRunPropDlg->m_dLength1200 );
			m_pABTools->SetBracingLength( COMPONENT_LENGTH_0700, pRunPropDlg->m_dLength0700 );
			break;
		
		default:
			break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

void AutobuildPropertiesWnd::SetABToolPointer(AutoBuildTools *pABTools)
{
	m_pABTools = pABTools;
}
