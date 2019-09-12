// BayPropertiesToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "BayPropertiesToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BayPropertiesToolbar

BayPropertiesToolbar::BayPropertiesToolbar()
{
}

BayPropertiesToolbar::~BayPropertiesToolbar()
{
}


//BEGIN_MESSAGE_MAP(BayPropertiesToolbar, CToolBarCtrl)
BEGIN_MESSAGE_MAP(BayPropertiesToolbar, CToolBar)
	//{{AFX_MSG_MAP(BayPropertiesToolbar)
	ON_COMMAND(ID_AUTOBUILD_BAY_DETAILS_BUTTON, OnAutobuildBayDetailsButton)
	ON_COMMAND(ID_RUN_DETAILS_BUTTON, OnRunDetailsButton)
	ON_UPDATE_COMMAND_UI(ID_AUTOBUILD_BAY_DETAILS_BUTTON, OnUpdateAutobuildBayDetailsButton)
	ON_UPDATE_COMMAND_UI(ID_RUN_DETAILS_BUTTON, OnUpdateRunDetailsButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BayPropertiesToolbar message handlers

void BayPropertiesToolbar::OnAutobuildBayDetailsButton() 
{
	acutPrintf( _T("Bay Details Pressed") );
	
}

void BayPropertiesToolbar::OnRunDetailsButton() 
{
	acutPrintf( _T("Run Details Pressed") );
}

void BayPropertiesToolbar::OnUpdateAutobuildBayDetailsButton(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void BayPropertiesToolbar::OnUpdateRunDetailsButton(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}
