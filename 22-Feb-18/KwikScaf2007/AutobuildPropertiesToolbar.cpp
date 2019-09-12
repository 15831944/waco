// AutobuildPropertiesToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "AutobuildPropertiesToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesToolbar

AutobuildPropertiesToolbar::AutobuildPropertiesToolbar()
{
}

AutobuildPropertiesToolbar::~AutobuildPropertiesToolbar()
{
}


BEGIN_MESSAGE_MAP(AutobuildPropertiesToolbar, CAcUiDockControlBar)
	//{{AFX_MSG_MAP(AutobuildPropertiesToolbar)
	ON_UPDATE_COMMAND_UI(ID_RUN_DETAILS_BUTTON, OnUpdateRunDetailsButton)
	ON_UPDATE_COMMAND_UI(ID_AUTOBUILD_BAY_DETAILS_BUTTON, OnUpdateAutobuildBayDetailsButton)
	ON_COMMAND(ID_AUTOBUILD_BAY_DETAILS_BUTTON, OnAutobuildBayDetailsButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AutobuildPropertiesToolbar message handlers

void AutobuildPropertiesToolbar::OnUpdateRunDetailsButton(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void AutobuildPropertiesToolbar::OnUpdateAutobuildBayDetailsButton(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void AutobuildPropertiesToolbar::OnAutobuildBayDetailsButton() 
{
	// TODO: Add your command handler code here
	
}

