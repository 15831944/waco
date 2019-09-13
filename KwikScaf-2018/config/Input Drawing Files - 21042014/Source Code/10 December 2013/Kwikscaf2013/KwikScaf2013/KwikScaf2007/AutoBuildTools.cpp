// AutoBuildTools.cpp: implementation of the AutoBuildTools class.
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
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoBuildTools.h"
#include <Math.h>
#include <dbents.h>
#include <geassign.h>
#include "Controller.h"
#include "FreeTieTube.h"
#include "AutobuildDialog.h"

//These should always be off in release mode
#ifdef _DEBUG
//	#define _SHOW_ANGLES_1
//	#define _SHOW_ANGLES_2
//	#define _SHOW_ANGLES_3
//	#define _SHOW_ANGLES_4
//	#define _SHOW_ANGLES_5
//	#define _DRAW_BOUND_BOX_
//	#define _DRAW_DEBUG_BOUND_BOX_
//	#define _DRAW_ARC_BOUND_BOX_
//	#define _DRAW_LAPBOARD_POINTS_
//	#define _SHOW_GRAVITY_POINTS_
//	#define _SHOW_BIRDCAGE_DATA_
#endif //#ifdef _DEBUG

//#define USE_TWO_BUTTON_TOOLBAR

#ifdef USE_TWO_BUTTON_TOOLBAR
	#include "AutobuildPropertiesToolbar.h"
	#include "AutobuildPropertiesWnd.h"
#endif	//USE_TWO_BUTTON_TOOLBAR

enum FilterTypeEnum
{
	FT_AUTOBUILD,
	FT_LAPBOARD,
	FT_TIE_TUBE
};

//const	bool	ALLOW_NEW_SECOND_POINT	= true;
const	bool	ALLOW_NEW_SECOND_POINT	= false;
const	double	DEBUG_OFFSET_TO_WALL	= 0.00;
const	bool	DRAW_RIGHT_OF_CENTRE	= false;
const	double 	dONE_DEGREE				= pi/180.00;
const	double 	d15_DEGREE				= dONE_DEGREE*15.00;
const	double 	d0Deg				= 0.00;
const	double 	d45Deg				= pi/4.00;
const	double 	d90Deg				= 2.00*d45Deg;
const	double 	d135Deg				= 3.00*d45Deg;
const	double 	d180Deg				= pi;
const	double 	d225Deg				= d180Deg+d45Deg;
const	double 	d270Deg				= d180Deg+d90Deg;
const	double 	d315Deg				= d180Deg+d135Deg;
const	double 	d360Deg				= 2.00*d180Deg;
const	double 	dminus45Deg			= -1.00*d45Deg;
const	double 	dminus90Deg			= 2.00*dminus45Deg;
const	double 	dminus135Deg		= 3.00*dminus45Deg;
//#define	ACURATELY_POSITION_LAP_COMPONENTS

//const	double	DEBUG_OFFSET_TO_WALL = 2000.00;

//////////////////////////////////////////////////////////////////////
//Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//AutoBuildTools
//
AutoBuildTools::AutoBuildTools()
{
	m_bBackDoorEntry		= false;
	m_pAutobuildDialog		= NULL;
	m_dAutoRunOffset		= 0.00;
	m_bAccessDenied			= false;
	SetGlobalsToDefault();
	SetUseLastPoints( false );
	m_iCurrentFilterType	= FT_AUTOBUILD;
	m_bUseEasternHopup		= false;
	m_iDrawWhenZero			= 10;
	m_bBuildForward			= false;
	m_iPreviousPreviousRunID	= ID_INVALID;
	m_iPreviousRunID			= ID_INVALID;
	m_dRLFirstBirdcage		= LARGE_NEGATIVE_NUMBER;
	m_bUseCalcRL			= false;
}

//////////////////////////////////////////////////////////////////////////////////
//~AutoBuildTools
//
AutoBuildTools::~AutoBuildTools()
{

}


//////////////////////////////////////////////////////////////////////////////
//MDI Aware considerations
//////////////////////////////////////////////////////////////////////////////
void AutoBuildTools::ShowDialogs(bool bShowDialogs)
{
	if (m_pAutobuildDialog!=NULL)
	{
		if (bShowDialogs)
			m_pAutobuildDialog->ShowWindow(SW_SHOW);		
		else
			m_pAutobuildDialog->ShowWindow(SW_HIDE);		
	}
	RefreshScreen( false );		
}

//////////////////////////////////////////////////////////////////////
//Autobuild Preview Creation
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//CreateLapboard
//This function is used to create a preview of bays, whilst the use is dragging
//	the mouse around, once a run is completed then the bays are created using the
//	Run and Bay classes
Acad::ErrorStatus AutoBuildTools::CreateLapboard()
{
	int				iRet, iTemp;
	Bay				*p1stBaySelected;
	TCHAR			keywrd[512];
	bool			bLoop;
	CString			sKeyWrd, sPrompt, sTemp;
	Point3D			pt;
	HitLocationEnum	eHit;

	//////////////////////////////////////////////////////////////////////////////
	//Set globals;
	try 
	{
		do
		{
			m_bAddToDB						= false;
			m_LBTplt.m_bShowLapBoards		= true;
//			m_LBTplt.m_bShowLapBoards		= false;
			m_LBTplt.SetLapBoardReqdThisRun(true);
			m_ptMousePoint[MP_1ST].set( 0.00, 0.00, 0.00 );
			m_ptMousePoint[MP_2ND] = m_ptMousePoint[MP_1ST];
			m_ptMousePoint[MP_3RD] = m_ptMousePoint[MP_1ST];

			////////////////////////////////////////////////////////////
			//get the first point from the user.  Do not specify a base point.
			sPrompt.Format( _T("\nSpecify first Point of Lapboard") );
			switch( iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_1ST] ) )
			{
			////////////////////////////////////////////////////////////
			case( RTKWORD ):			//Keyword returned from getxxx() routine
				//not valid
				MessageBeep(MB_ICONEXCLAMATION);
				continue;

			////////////////////////////////////////////////////////////
			case( RTNONE ):				//fallthrough
			case( RTERROR ):			//fallthrough
			default:
				//This should not have occured
				RefreshScreen( false );
				MessageBeep(MB_ICONEXCLAMATION);
				return Acad::eInetUnknownError;	//Unknown error

			////////////////////////////////////////////////////////////
			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				RefreshScreen( false );
				return Acad::eUserBreak;

			////////////////////////////////////////////////////////////
			case( RTNORM ):	
				//everthing is fine
	
				p1stBaySelected = GetBayByInternalPoint(m_ptMousePoint[MP_1ST], eHit);
				if( p1stBaySelected==NULL )
				{
					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox( NULL, _T("The first point of the lapboard must be within a bay!"),
								_T("Invalid Point"), MB_OK );
					continue;
				}

				break;
			}

			assert( p1stBaySelected!=NULL );

			////////////////////////////////////////////////////////////
			SetFirstPointFlag(true);

			////////////////////////////////////////////////////////////
			//is this within a bay?  if it is then store that bay for later use
			int iBoards;
			iBoards = GetNumberOfPlanksForWidth( p1stBaySelected->GetBayWidth() );
			iBoards+= p1stBaySelected->GetNumberOfPlanksOfStageFromWall( SOUTH );
			iBoards+= p1stBaySelected->GetNumberOfPlanksOfStageFromWall( NORTH );
			m_LBTplt.SetNumberOfBoards(iBoards);

			bLoop = true;
			do
			{
				acedInitGet( RSG_OTHER, _T("Number Of Planks"));

				////////////////////////////////////////////////////////////
        sPrompt.Format( _T("\nSpecify next point of lapboard or change number of boards<%i>:"), iBoards );
				switch( iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_2ND] ) )
				{
				////////////////////////////////////////////////////////////
				case( RTKWORD ):	//Keyword returned from getxxx() routine
					//we don't want keys, we want positions!
					acedGetInput( keywrd );
					sKeyWrd = keywrd;
					sKeyWrd.MakeUpper();

					if( _istdigit( keywrd[0] ) )
					{
						iTemp = _ttoi( keywrd );
						if( iTemp<MIN_PLANKS_FOR_LAPBOARD || iTemp>MAX_PLANKS_FOR_LAPBOARD )
						{
							sTemp.Format( _T("You typed %i boards!  However, you may only create\nlapboards with between %i and %i boards"), 
											MIN_PLANKS_FOR_LAPBOARD, MAX_PLANKS_FOR_LAPBOARD );
							sTemp += _T("\nPlease specify a valid number of boards!");
							MessageBox( NULL, sTemp, _T("Invalid Number"), MB_OK );
						}
						else
						{
							iBoards = iTemp;
							RefreshScreen( false );
							SetUseLastPoints();
							MSG Msg;
							Msg.message=WM_MOUSEMOVE;
							filterMouseForLapboard( &Msg );
							m_LBTplt.SetNumberOfBoards(iBoards);
						}
					}
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
					continue;

				////////////////////////////////////////////////////////////
				case( RTNONE ):		//fallthrough
				case( RTERROR ):	//fallthrough
				default:
					//This should not have occured
					RefreshScreen( false );
					MessageBeep(MB_ICONEXCLAMATION);
					return Acad::eInetUnknownError; //Unknown error

				////////////////////////////////////////////////////////////
				case( RTCAN ):		//User cancelled request = Ctl-C
					RefreshScreen( false );
					return Acad::eUserBreak;

				////////////////////////////////////////////////////////////
				case( RTNORM ):		//Request succeeded
					AllowForOrtho( m_ptMousePoint[MP_2ND], MP_2ND );
					m_bAddToDB = true;
					m_LBTplt.m_dFullLength = m_ptMousePoint[MP_1ST].distanceTo( m_ptMousePoint[MP_2ND] );
					DrawLapboard( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], false, true );
					m_bAddToDB = false;

					///////////////////////////////////////////
					//refresh the screen
					RefreshScreen();
					bLoop = false;
					break;
				}

			} while( bLoop );

			SetFirstPointFlag(true);

			/////////////////////////////
			//refresh the screen
			RefreshScreen( false );
		}while( true );
	}
	catch (int e)
	{
		//Beep
		MessageBeep(MB_ICONEXCLAMATION);
		assert( false );

		if (e == RTCAN)				//(-5002) User cancelled request -- Ctl-C
		{
			return Acad::eUserBreak;
		}

		if (e == RTERROR)			//(-5001) Some other error
		{
			return Acad::eInvalidInput;
		}

		return Acad::eInetUnknownError;	//Unknown error
	}
	return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////////////
//CreateTieTube
Acad::ErrorStatus AutoBuildTools::CreateTieTube()
{
	int				iRet;
	Bay				*pBay;
	TCHAR			keywrd[512];
	bool			bLoop;
	double			dSeparation;
	CString			sKeyWrd, sPrompt, sTemp;
	Point3D			pt;
	CornerOfBayEnum eCnr;


	//////////////////////////////////////////////////////////////////////////////
	//Set globals;
	try 
	{
		do
		{
			m_bAddToDB						= false;
			m_ptMousePoint[MP_1ST].set( 0.00, 0.00, 0.00 );
			m_ptMousePoint[MP_2ND] = m_ptMousePoint[MP_1ST];
			m_ptMousePoint[MP_3RD] = m_ptMousePoint[MP_1ST];

			////////////////////////////////////////////////////////////
			//get the first point from the user.  Do not specify a base point.
      sPrompt.Format( _T("\nChoose a Standard to attach TieTube to: ") );
			iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_1ST] );
			switch( iRet )
			{
			////////////////////////////////////////////////////////////
			case( RTKWORD ):			//Keyword returned from getxxx() routine
				//not valid
				MessageBeep(MB_ICONEXCLAMATION);
				continue;

			////////////////////////////////////////////////////////////
			case( RTNONE ):				//fallthrough
			case( RTERROR ):			//fallthrough
			default:
				//This should not have occured
				RefreshScreen( false );
				MessageBeep(MB_ICONEXCLAMATION);
				return Acad::eInetUnknownError;	//Unknown error

			////////////////////////////////////////////////////////////
			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				RefreshScreen( false );
				return Acad::eUserBreak;

			////////////////////////////////////////////////////////////
			case( RTNORM ):	
				//everthing is fine
				pBay = GetStandardByProxyToPoint(m_ptMousePoint[MP_1ST], eCnr );
				if( pBay==NULL )
				{
					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox( NULL, _T("The first point of the Tie Tube must attach to a standard!"),
								_T("Invalid Point"), MB_OK );
					continue;
				}

				break;
			}

			////////////////////////////////////////////////////////////
			SetFirstPointFlag(true);

			////////////////////////////////////////////////////////////
			//get the second point
			bLoop = true;
			dSeparation = pBay->GetRunPointer()->GetRunTemplate()->GetTiesVerticallyEvery();
			sTemp.Format( _T("\nTie Tube attached to %s corner of Bay %i!"),
						GetSideOfBayDescStr( CornerAsSideOfBay( eCnr ) ),
						pBay->GetBayNumber() );
			do
			{
				acedInitGet( RSG_OTHER, _T("Separation"));

				////////////////////////////////////////////////////////////
				//default to the tie separation for the run!
        sPrompt.Format( _T("%s\nSpecify end point of Tie Tube or [Vertical Separation(mm) <%1.0f>]: "), sTemp, dSeparation );
				switch( iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_2ND] ) )
				{
				////////////////////////////////////////////////////////////
				case( RTKWORD ):	//Keyword returned from getxxx() routine
					acedGetInput( keywrd );
					sKeyWrd = keywrd;
					sKeyWrd.MakeUpper();

					sTemp.Empty();

					if( _istdigit( keywrd[0] ) )
					{
						dSeparation = _tstof( keywrd );
						RefreshScreen();
					}
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
					break;

				////////////////////////////////////////////////////////////
				case( RTNONE ):		//fallthrough
				case( RTERROR ):	//fallthrough
				default:
					//This should not have occured
					RefreshScreen( false );
					MessageBeep(MB_ICONEXCLAMATION);
					return Acad::eInetUnknownError; //Unknown error

				////////////////////////////////////////////////////////////
				case( RTCAN ):		//User cancelled request = Ctl-C
					RefreshScreen( false );
					return Acad::eUserBreak;

				////////////////////////////////////////////////////////////
				case( RTNORM ):		//Request succeeded
					AllowForOrtho( m_ptMousePoint[MP_2ND], MP_2ND );
					m_bAddToDB = true;
					pBay->GetTemplate()->SetTieTubeVertSeparation( dSeparation );
					DrawTieTube( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], false );
					m_bAddToDB = false;

					///////////////////////////////////////////
					//refresh the screen
					RefreshScreen();
					bLoop = false;
					break;
				}

			} while( bLoop );

			SetFirstPointFlag(true);

			/////////////////////////////
			//refresh the screen
			RefreshScreen( false );
		}while( true );
	}
	catch (int e)
	{
		//Beep
		MessageBeep(MB_ICONEXCLAMATION);
		assert( false );

		if (e == RTCAN)				//(-5002) User cancelled request -- Ctl-C
		{
			return Acad::eUserBreak;
		}

		if (e == RTERROR)			//(-5001) Some other error
		{
			return Acad::eInvalidInput;
		}

		return Acad::eInetUnknownError;	//Unknown error
	}
	return Acad::eOk;
}

#include "BayPropToolbarWnd.h"
#include "BayPropertiesToolbar.h"

void AutoBuildTools::StartAutobuild()
{
	double	dRL;

	m_iPreviousRunID = ID_INVALID;
	m_iPreviousPreviousRunID = ID_INVALID;

#ifdef USE_TWO_BUTTON_TOOLBAR
/*
	InitCommonControls();
	CMDIFrameWnd* pFrame = acedGetAcadFrame();

	BayPropertiesToolbar *pBar;
	BayPropToolbarWnd	*pWnd;
	
	pWnd = new BayPropToolbarWnd();
	pWnd->Create(NULL, NULL, WS_CHILD | WS_MINIMIZE, 
	    CRect(0,0,1,1), pFrame, 0, NULL);
	pBar = new BayPropertiesToolbar();
	pBar->Create( pFrame,
			WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_FLYBY |    
			CBRS_TOP | CBRS_TOOLTIPS | CCS_ADJUSTABLE, 
			IDR_BAYPROPERTIES_TOOLBAR);

	pBar->LoadToolBar(IDR_BAYPROPERTIES_TOOLBAR);

	pBar->GetToolBarCtrl().SetOwner(pWnd);
	pBar->EnableDocking(CBRS_ALIGN_ANY);
	pBar->SetWindowText(_T("Autobuild Toolbar"));

	pFrame->EnableDocking(CBRS_ALIGN_ANY);
	pFrame ->DockControlBar(pBar, AFX_IDW_DOCKBAR_TOP);
	pFrame->ShowControlBar(pBar, TRUE, TRUE);
	pFrame ->RecalcLayout();

	dRL = 0.00;
	CreateRun( dRL );

	if( pWnd!=NULL )
	{
		delete pWnd;
		pWnd = NULL;
	}
	if( pBar!=NULL )
	{
		delete pBar;
		pBar = NULL;
	}
	pFrame->RecalcLayout();
*/
	AutobuildPropertiesToolbar	APTool;

	CMDIFrameWnd* pFrame = acedGetAcadFrame();

	pAPTool = new AutobuildPropertiesToolbar();
	if( !pAPTool->Create( pFrame, _T("Autobuild Properties"), IDR_BAYPROPERTIES_TOOLBAR,
		 WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CCS_ADJUSTABLE ) )
	{
		acutPrintf( _T("\nUnable to create toolbar") );
		if( pAPTool!=NULL )
		{
			delete pAPTool;
			pAPTool = NULL;
		}
		return;
	}
	
	pAPTool->EnableDocking(CBRS_ALIGN_ANY);

	pFrame->EnableDocking(CBRS_ALIGN_ANY);
	pFrame ->DockControlBar(pAPTool, AFX_IDW_DOCKBAR_FLOAT);
	pFrame->ShowControlBar(pAPTool, TRUE, TRUE);
	pFrame ->RecalcLayout();

	dRL = 0.00;
	CreateRun( dRL );

	if( pAPTool!=NULL )
	{
		delete pAPTool;
		pAPTool = NULL;
	}
	pFrame->RecalcLayout();

#else	//#ifdef USE_TWO_BUTTON_TOOLBAR

	//////////////////////////////////////////////////////////////////////////////
	// Bay properties Dialog
	//////////////////////////////////////////////////////////////////////////////
	if( m_pAutobuildDialog==NULL )
	{
		m_pAutobuildDialog = new AutobuildDialog(NULL, this);
		m_pAutobuildDialog->Create();
	}

	dRL = 0.00;
	CreateRun( dRL );

	DELETE_PTR(m_pAutobuildDialog);
#endif	//#ifdef USE_TWO_BUTTON_TOOLBAR
}

//////////////////////////////////////////////////////////////////////////////////
//CreateRun
//This function is used to create a preview of bays, whilst the use is dragging
//	the mouse around, once a run is completed then the bays are created using the
//	Run and Bay classes
Acad::ErrorStatus AutoBuildTools::CreateRun( double dRL )
{
	int		i, iCount, iCloseablecount, iRet;
	bool	bCloseable, bBirdCageable, bOldOrthoMode;
	TCHAR	keywrd[512];
	double	dOldRL;
	CString	sKeyWrd, sPrompt, sTemp, sForward, sMsg;
	Point3D	pt;

	//Set globals;
	SetArcFlag( false );
	m_bAddToDB						= false;
	m_bClosePointSet				= false;
	m_bCalcRLErrorShown				= false;
	m_dLastRunAngle					= d360Deg;
	m_LBTplt.SetLapBoardReqdThisRun(false);

	iCloseablecount	= 2;
	bCloseable		= false;
	bBirdCageable	= false;

	m_ptMousePoint[MP_1ST].x = 0.00;
	m_ptMousePoint[MP_1ST].y = 0.00;
	m_ptMousePoint[MP_1ST].z = 0.00;
	m_ptMousePoint[MP_2ND] = m_ptMousePoint[MP_1ST];
	m_ptMousePoint[MP_3RD] = m_ptMousePoint[MP_1ST];

	try 
	{
		if( acedInitGet( RSG_OTHER, _T("RL") )==RTERROR )
		{
			assert( false );
		}

		//This is the First RL, if they change the RL, it will call this funciton
		//	recursively, so this will be set once again!
		m_dRL[MP_1ST]=dRL;
		SetTemplateRLStart( m_dRL[MP_1ST] );
		SetTemplateRLEnd( m_dRL[MP_1ST] );

		if( GetController()!=NULL && GetController()->IsRLCalcPossible() )
		{
			if( m_bUseCalcRL )
			{
				//get the first point from the user.  Do not specify a base point.
        sPrompt.Format( _T("\nSpecify first Point or RL<auTocalc rl>[use Last rl]:") );
			}
			else
			{
				//get the first point from the user.  Do not specify a base point.
        sPrompt.Format( _T("\nSpecify first Point or RL<%0.1f>[auTocalc rl]:"), dRL );
			}
		}
		else
		{
			//get the first point from the user.  Do not specify a base point.
      sPrompt.Format( _T("\nSpecify first Point or RL<%0.1f>:"), dRL );
		}

		switch( iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_1ST] ) )
		{
		case( RTNONE ):				//fallthrough (5000) No result
		case( RTERROR ):			//fallthrough (-5001) Some other error
		default:
			//This should not have occured
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);
			return Acad::eInetUnknownError;	//Unknown error
		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);
			return Acad::eUserBreak;
		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
			acedGetInput( keywrd );
			sKeyWrd = keywrd;
			sKeyWrd.MakeUpper();

			if( _istdigit( keywrd[0] ) || keywrd[0]==_T('-')  )
			{
				dOldRL = dRL;

				dRL = _tstof( keywrd );

				RefreshScreen();

				///////////////////////////////////////////////////////
				//ensure this RL is valid
				if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
					dRL = dOldRL;
			}
			else
			{
				if( GetController()->IsRLCalcPossible() )
				{
					if( sKeyWrd[0]==_T('T') )
						m_bUseCalcRL = true;
					else if( sKeyWrd[0]==_T('L') )
						m_bUseCalcRL = false;
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
				}
				else
				{
					MessageBeep(MB_ICONEXCLAMATION);
				}
			}

			//we need to go to back to the prompt again, with this new RL!
			//	we can to this recursively, so long as we kill this recursion
			return CreateRun( dRL );
		case( RTNORM ):				//(5100) Request succeeded
			//everthing is fine
			break;
		}

		//If this is the very first point entered then we need to store it!
		if( GetController()->IsRLCalcPossible() && m_bUseCalcRL )
		{
			dOldRL = dRL;
			dRL = GetController()->GetRLAtPoint( m_ptMousePoint[MP_1ST] );
			if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
			{
				dRL = dOldRL;
				return CreateRun( dRL );
			}

			m_dRL[MP_1ST]=dRL;
			SetTemplateRLStart( m_dRL[MP_1ST] );
			SetTemplateRLEnd( m_dRL[MP_1ST] );
		}

		SetClosePoint(m_ptMousePoint[MP_1ST]);

		SetFirstPointFlag(true);
		
		iCount = 0;
		do	
		{
			if( acedInitGet( RSG_OTHER, NULL )==RTERROR )
			{
				assert( false );
			}

			//create the acad prompt
			sTemp.Empty();
			if( bCloseable )
				sTemp = _T("Close/");
			if( bBirdCageable )
				sTemp+= _T("Birdcage/");

			//This is the 2nd RL of 3 for the arc
			//	or last RL for a simple Wall
			m_dRL[MP_2ND] = dRL;
			SetTemplateRLEnd( m_dRL[MP_2ND] );

			if( m_bBuildForward )
				sForward = _T("build Reverse");
			else
				sForward = _T("build Forward");


			if( m_bOverrunMousePoint )
			{
				if( GetController()!=NULL && GetController()->IsRLCalcPossible() )
				{
					if( m_bUseCalcRL )
					{
						//get the first point from the user.  Do not specify a base point.
            sPrompt.Format( _T("\nSpecify next point or RL<auTocalc rl>[Arc/%sUnderrun/%s/use Last rl]:"), sTemp, sForward );
					}
					else
					{
						//get the first point from the user.  Do not specify a base point.
            sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sUnderrun/%s/auTocalc rl]:"), dRL, sTemp, sForward );
					}
				}
				else
				{
					//get the first point from the user.  Do not specify a base point.
          sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sUnderrun/%s]:"), dRL, sTemp, sForward );
				}
			}
			else
			{
				double dWidth;
				dWidth = GetABBay()->GetTemplate()->GetWidthOfSStage(); 
				if( dWidth>0.00 )
				{
					//leave the user setting for the hopup
				}
				else
				{
					//it is impossible to have a hopup
					m_bUseEasternHopup = false;
				}

				//This means we have to be in underrun mode to change the hopup
				//do we need the eastern hopup
				if( m_bUseEasternHopup )
				{
					if( GetController()!=NULL && GetController()->IsRLCalcPossible() )
					{
						if( m_bUseCalcRL )
						{
							//get the first point from the user.  Do not specify a base point.
              sPrompt.Format( _T("\nSpecify next point or RL<auTocalc rl>[Arc/%sOverrun/Delete hopup/%s/use Last rl]:"), sTemp, sForward );
						}
						else
						{
							//get the first point from the user.  Do not specify a base point.
              sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sOverrun/Delete hopup/%s/auTocalc rl]:"), dRL, sTemp, sForward );
						}
					}
					else
					{
						//get the first point from the user.  Do not specify a base point.
            sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sOverrun/Delete hopup/%s]:"), dRL, sTemp, sForward );
					}

					GetABBay()->GetTemplate()->SetWidthOfEStage( dWidth ); 
					GetABBay()->GetTemplate()->SetSEStageCnr( true ); 
					if( GetABBay()->GetTemplate()->GetWidthOfNStage()>0 )
					{
						GetABBay()->GetTemplate()->SetNEStageCnr( true ); 
					}

					//copy the hopup and corners from the southern arrangement
					int		iNumberOfStageBoards, iLiftID;
					Lift	*pLift;
					for( iLiftID=0; iLiftID<GetABBay()->GetNumberOfLifts(); iLiftID++ )
					{
						pLift = GetABBay()->GetLift( iLiftID );
						iNumberOfStageBoards = pLift->GetNumberOfPlanksOnStageBoard( SOUTH );
						if( iNumberOfStageBoards>0 &&
							!pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) )
						{
							pLift->AddStage( EAST, iNumberOfStageBoards, MT_STEEL );
							pLift->AddCornerStageBoard( CNR_SOUTH_EAST, MT_STEEL );
							if( GetABBay()->GetTemplate()->GetNEStageCnr() )
							{
								pLift->AddCornerStageBoard( CNR_NORTH_EAST, MT_STEEL );
							}
						}
					}
				}
				else
				{
					if( GetController()!=NULL && GetController()->IsRLCalcPossible() )
					{
						if( m_bUseCalcRL )
						{
							//get the first point from the user.  Do not specify a base point.
              sPrompt.Format( _T("\nSpecify next point or RL<auTocalc rl>[Arc/%sOverrun/Hopup/%s/use Last rl]:"), sTemp, sForward );
						}
						else
						{
							//get the first point from the user.  Do not specify a base point.
              sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sOverrun/Hopup/%s/auTocalc rl]:"), dRL, sTemp, sForward );
						}
					}
					else
					{
						//get the first point from the user.  Do not specify a base point.
            sPrompt.Format( _T("\nSpecify next point or RL<%0.1f>[Arc/%sOverrun/Hopup/%s]:"), dRL, sTemp, sForward );
					}

					//Delete the hopup and corner fillers
					GetABBay()->GetTemplate()->SetWidthOfEStage( 0 ); 
					GetABBay()->GetTemplate()->SetSEStageCnr( false ); 
					GetABBay()->GetTemplate()->SetNEStageCnr( false );
					GetABBay()->CreateBracingArrangment ( EAST, MT_STEEL );
				}
			}

			iRet=GetPoint3D( NULL, sPrompt, m_ptMousePoint[MP_2ND] );

			switch( iRet )
			{
			case( RTNONE ):				//fallthrough (5000) No result
			case( RTERROR ):			//fallthrough (-5001) Some other error
			default:
				//This should not have occured
				RefreshScreen( false );
				MessageBeep(MB_ICONEXCLAMATION);
				return Acad::eInetUnknownError; //Unknown error
			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				RefreshScreen( false );
				MessageBeep(MB_ICONEXCLAMATION);
				return Acad::eUserBreak;
			case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
				acedGetInput( keywrd );
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();

				if ( sKeyWrd[0]==_T('A') )
				{
					bBirdCageable = false;

					bOldOrthoMode = acdbHostApplicationServices()->
										workingDatabase()->orthomode();
					acdbHostApplicationServices()->
										workingDatabase()-> setOrthomode( false );
					Acad::ErrorStatus eRetval;
					eRetval = CreateArcRun(dRL);

					acdbHostApplicationServices()->
										workingDatabase()->setOrthomode( bOldOrthoMode );

					if ( eRetval!=Acad::eOk )
					{
						return eRetval;
					}

					//////////////////////////////////////////////////////////
					//We are now going to be doing a wall, so the current
					//	second RL will now become the First RL
					m_dRL[MP_1ST] = m_dRL[MP_2ND];
					SetTemplateRLStart( m_dRL[MP_1ST] );
					SetTemplateRLEnd( m_dRL[MP_1ST] );
				}
				else if ( sKeyWrd[0]==_T('B') )
				{
					if( bBirdCageable )
					{
						m_bBirdCageMode = true;
						Acad::ErrorStatus eRetval;
						eRetval = CreateBirdCage(dRL);

						///////////////////////////////////////////////////////
						//we are automatically now in wall mode, unless they
						//	are in arc mode
						m_bBirdCageMode = false;
						bBirdCageable	= false;

						//////////////////////////////////////////////////////////
						//We are now going to be doing a wall, or an arc so the
						//	current second RL will now become the First RL
						//	either we we have lost the purpose of the current
						//	m_dRL[MP_1ST];
						m_dRL[MP_1ST] = m_dRL[MP_2ND];
						m_dRL[MP_2ND] = m_dRL[MP_3RD];
						SetTemplateRLStart( m_dRL[MP_1ST] );
						SetTemplateRLEnd( m_dRL[MP_2ND] );

						//refresh the screen
						RefreshScreen();

						if ( eRetval!=Acad::eOk )
						{
							return eRetval;
						}

						if( GetArcFlag( ) )
						{
							////////////////////////////////////////////////////
							//They want to create an arc instead
							Acad::ErrorStatus eRetval;

							bOldOrthoMode = acdbHostApplicationServices()->
												workingDatabase()->orthomode();
							acdbHostApplicationServices()->
										workingDatabase()->setOrthomode( false );

							eRetval = CreateArcRun(dRL);

							acdbHostApplicationServices()->
										workingDatabase()->setOrthomode( bOldOrthoMode );

							if ( eRetval!=Acad::eOk )
							{
								return eRetval;
							}
						}
					}
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
				}
				else if ( sKeyWrd[0]==_T('C') )
				{
					if( bCloseable )
					{
						bBirdCageable = false;

						//They want to close the run
						m_ptMousePoint[MP_2ND] = m_ptClosePoint;

						m_bAddToDB = true;

						for( i=(int)MP_1ST; i<=(int)MP_2ND; i++ )
						{
							m_ptMousePoint[i].z = m_dRL[i];
						}

						m_bCalcRLErrorShown = false;
						DrawRun( false, m_bAddToDB, false, 0, 100 );
						acedRestoreStatusBar();
	
						if( m_LBTplt.m_bShowLapBoards )
							m_LBTplt.SetLapBoardReqdThisRun(true);

						SetFirstPointFlag(true);
						m_bAddToDB = false;

						//refresh the screen
						RefreshScreen( false );

						//We are just about to exit so there is no need to update
						//	RL's at the moment!
						return Acad::eOk;
					}
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
				}
				else if ( _istdigit(keywrd[0]) || keywrd[0]==_T('-')  )
				{
					dOldRL = dRL;

					dRL = _tstof( keywrd );
					//don't decrement 'iCloseablecount'

					RefreshScreen();

					///////////////////////////////////////////////////////
					//ensure this RL is valid
					if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
						dRL = dOldRL;

					continue;
				}

				else if( sKeyWrd[0]==_T('F') )
				{
					RefreshScreen();
					m_iDrawWhenZero = 0;
					m_bBuildForward = true;
				}
				else if( sKeyWrd[0]==_T('R') )
				{
					RefreshScreen();
					m_iDrawWhenZero = 0;
					m_bBuildForward = false;
				}
				else if( sKeyWrd[0]==_T('O') )
				{
					RefreshScreen();
					m_iDrawWhenZero = 0;
					m_bBuildForward = true;

					m_bOverrunMousePoint = true;
					//Delete the hopup and corner fillers
	 				m_bUseEasternHopup = false;
					GetABBay()->GetTemplate()->SetWidthOfEStage( 0 ); 
					GetABBay()->GetTemplate()->SetSEStageCnr( false ); 
					GetABBay()->GetTemplate()->SetNEStageCnr( false );
					GetABBay()->CreateBracingArrangment ( EAST, MT_STEEL );
					continue;
				}
				else if( sKeyWrd[0]==_T('U') )
				{
					RefreshScreen();
					m_iDrawWhenZero = 0;
					m_bBuildForward = false;
					m_bUseEasternHopup = true;
					m_bOverrunMousePoint = false;
					continue;
				}
				else if( sKeyWrd[0]==_T('D') )
				{
					if( !m_bOverrunMousePoint )
					{
		 				m_bUseEasternHopup = false;
						RefreshScreen();
						continue;
					}
				}
				else if( sKeyWrd[0]==_T('H') )
				{
					if( !m_bOverrunMousePoint )
					{
						m_bUseEasternHopup = true;
						RefreshScreen();
						continue;
					}
				}
				else if( sKeyWrd[0]==_T('T') )
				{
					if( GetController()->IsRLCalcPossible() )
					{
						m_bUseCalcRL = true;
					}
				}
				else if( sKeyWrd[0]==_T('L') )
				{
					if( GetController()->IsRLCalcPossible() )
					{
						m_bUseCalcRL = false;
					}
				}
				else
				{
					RefreshScreen();

					MessageBeep(MB_ICONEXCLAMATION);
					//don't decrement 'iCloseablecount'
					continue;
				}
				break;
			case( RTNORM ):				//(5100) Request succeeded
				//InsetPoints();

				//We can now use these points for a birdcage
				AllowForOrtho( m_ptMousePoint[MP_2ND], MP_2ND );

				if( GetController()->IsRLCalcPossible() && m_bUseCalcRL )
				{
					dOldRL = dRL;
					dRL = GetController()->GetRLAtPoint( m_ptMousePoint[MP_2ND] );
					if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
					{
						dRL = dOldRL;
						//Get the point again
						break;
					}
					m_dRL[MP_2ND] = dRL;
					SetTemplateRLEnd( m_dRL[MP_2ND] );
				}

				bBirdCageable = true;
				for( i=MP_1ST; i<MP_SIZE; i++ )
				{
					m_ptBirdcagePoints[i] = m_ptMousePoint[i];
				}

				m_bAddToDB = true;

				m_ptMousePoint[MP_1ST].z = m_dRL[MP_1ST];
				m_ptMousePoint[MP_2ND].z = m_dRL[MP_2ND];

				m_bCalcRLErrorShown = false;
				DrawRun( false, m_bAddToDB, false, 0, 100 );
				acedRestoreStatusBar();

				if( m_LBTplt.m_bShowLapBoards )
					m_LBTplt.SetLapBoardReqdThisRun(true);

				SetFirstPointFlag(true);
				m_bAddToDB = false;

				m_ptMousePoint[MP_1ST]	= m_ptMousePoint[MP_2ND];

				///////////////////////////////////////////
				//refresh the screen
				RefreshScreen();

				////////////////////////////////////////////////////////////
				//We are now going to be doing another wall, so the current
				//	second RL will now become the First RL
				m_dRLFirstBirdcage = m_dRL[MP_1ST];
				m_dRL[MP_1ST] = m_dRL[MP_2ND];
				SetTemplateRLStart( m_dRL[MP_1ST] );
				SetTemplateRLEnd( m_dRL[MP_1ST] );

				break;
			}

			iCloseablecount--;
			if( iCloseablecount<=0 )
				bCloseable = true;
		}while( true );

		m_bAddToDB = false;
		SetFirstPointFlag(true);

		/////////////////////////////
		//refresh the screen
		RefreshScreen( false );
	}
	catch (int e)
	{
		//Beep
		MessageBeep(MB_ICONEXCLAMATION);
		assert( false );

		if (e == RTCAN)				//(-5002) User cancelled request -- Ctl-C
		{
			return Acad::eUserBreak;
		}

		if (e == RTERROR)			//(-5001) Some other error
		{
			return Acad::eInvalidInput;
		}
		return Acad::eInetUnknownError;	//Unknown error
	}
	return Acad::eOk;
}

Acad::ErrorStatus AutoBuildTools::CreateBirdCage( double &dRL )
{
	int		iRet;
	TCHAR	keywrd[512];
	double	dOldRL;
	CString	sKeyWrd, sPrompt;

	SetArcFlag( false );
	SetFirstPointFlag(true);

	//get the third point from the user, using the first 2 points as begining and end.
	acedInitGet( RSG_OTHER, _T("Wall RelativeLevel"));

	//this is the Third RL for an arc, this will become the
	//	2nd RL if we continue in Arc mode
	m_dRL[MP_3RD] = dRL;

	//create the acad prompt
	if( GetController()!=NULL && GetController()->IsRLCalcPossible() )
	{
		if( m_bUseCalcRL )
		{
      sPrompt.Format( _T("\nSpecify final point of Birdcage or [Wall/Arc/RL<auTocalc rl>/use Last rl]:") );
		}
		else
		{
      sPrompt.Format( _T("\nSpecify final point of Birdcage or [Wall/Arc/RL<%0.2f>/auTocalc rl]:"), dRL );
		}
	}
	else
	{
    sPrompt.Format( _T("\nSpecify final point of Birdcage or [Wall/Arc/RL<%0.2f>]:"), dRL );
	}

	switch( iRet=GetPoint3D( &m_ptMousePoint[MP_1ST], sPrompt, m_ptMousePoint[MP_3RD]) )
	{
	case( RTNONE ):				//fallthrough (5000) No result
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen( false );
		MessageBeep(MB_ICONEXCLAMATION);
		return Acad::eInetUnknownError;	//Unknown error
	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen( false );
		MessageBeep(MB_ICONEXCLAMATION);
		return Acad::eUserBreak;
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if ( sKeyWrd[0]==_T('W') )
		{
			ReturnToWallMode();

			//we need to go back and create an wall
			return Acad::eOk;
		}
		else if ( sKeyWrd[0]==_T('A') )
		{
			SetArcFlag( true );

			//we need to go back and create an arc
			return Acad::eOk;
		}
		else if ( _istdigit(keywrd[0]) || sKeyWrd[0]==_T('-') )
		{
			dOldRL = dRL;

			dRL = _tstof( keywrd );
			//Ask for the point again, we can do this recursively since the point is
			//	global and will not be changed by this recursion of this function

			RefreshScreen();

			///////////////////////////////////////////////////////
			//ensure this RL is valid
			if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
				dRL = dOldRL;

			return CreateBirdCage( dRL );
		}
		else if ( sKeyWrd[0]==_T('T') )
		{
			if( GetController()->IsRLCalcPossible() )
			{
				m_bUseCalcRL = true;
			}
			return CreateBirdCage( dRL );
		}
		else if ( sKeyWrd[0]==_T('L') )
		{
			if( GetController()->IsRLCalcPossible() )
			{
				m_bUseCalcRL = false;
			}
			return CreateBirdCage( dRL );
		}
		else
		{
			MessageBeep(MB_ICONEXCLAMATION);
		}
		break;

	case( RTNORM ):				//(5100) Request succeeded
		//create the bays
		AllowForOrtho( m_ptMousePoint[MP_3RD], MP_3RD );

		if( GetController()->IsRLCalcPossible() && m_bUseCalcRL )
		{
			dOldRL = dRL;
			dRL = GetController()->GetRLAtPoint( m_ptMousePoint[MP_3RD] );
			if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
			{
				dRL = dOldRL;
				return CreateBirdCage( dRL );
			}

			m_dRL[MP_3RD] = dRL;
		}
		
		m_bAddToDB = true;
		DrawBirdcage( false );
		m_bAddToDB = false;
		SetArcFlag( false );
		SetFirstPointFlag(true);
		m_ptMousePoint[MP_1ST] = m_ptMousePoint[MP_3RD];

		//refresh the screen
		RefreshScreen();
		break;
	}

	//This should only occur if RTNORM or RTKWORD+'W' was set
	return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////////////
//CreateArcRun
//
Acad::ErrorStatus  AutoBuildTools::CreateArcRun( double &dRL )
{
	int		iRet;
	TCHAR	keywrd[512];
	double	dOldRL;
	CString	sKeyWrd, sPrompt;

	//refresh the screen
	RefreshScreen();

	//Arc Run
	do
	{
		SetFirstPointFlag(true);

		//get the second point from the user, using the first point as a base point.
		acedInitGet( RSG_OTHER, _T("Wall RelativeLevel"));

		//this is the Second RL for an arc, this will become the
		//	1st RL if we continue in Arc mode
		m_dRL[MP_2ND] = dRL;
		SetTemplateRLEnd( m_dRL[MP_2ND] );

		RefreshScreen( true );

		//create the acad prompt
    sPrompt.Format( _T("\nSpecify second point of Arc or [Wall/RL<%0.2f>]:"), dRL );

		switch( iRet=GetPoint3D( &m_ptMousePoint[MP_1ST], sPrompt, m_ptMousePoint[MP_2ND]) )
		{
		case( RTNONE ):				//fallthrough (5000) No result
		case( RTERROR ):			//fallthrough (-5001) Some other error
		default:
			//This should not have occured
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);

			return Acad::eInetUnknownError;	//Unknown error
		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);
			return Acad::eUserBreak;
		case( RTNORM ):				//(5100) Request succeeded
			Acad::ErrorStatus eRetval;
			eRetval = ProcessArcThirdPoint(dRL);

			if ( eRetval!=Acad::eOk )
				return eRetval;

			//if the user pressed 'w' in the ProcessArcThirdPoint function then 
			//	eRetval==Acad::eOk and Mousetracker would be true, otherwise
			//	Mousetracker would be false;
			if( GetMouseTrack() )
			{
				//Note: ReturnToWallMode() has already been called no need
				//	to do it again

				//we need to go back to the wall mode
				return Acad::eOk;
			}
			break;

		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
			acedGetInput( keywrd );
			sKeyWrd = keywrd;
			sKeyWrd.MakeUpper();

			if ( sKeyWrd[0]==_T('W') )
			{
				ReturnToWallMode();
				//??? JSB 990803 - is this required?
				//m_ptMousePoint[MP_1ST] = m_ptMousePoint[MP_2ND];

				return Acad::eOk;
			}
			else if ( _istdigit(keywrd[0]) || sKeyWrd[0]==_T('-') )
			{
				dOldRL = dRL;

				dRL = _tstof( keywrd );
				//Ask for the point again

				RefreshScreen();

				///////////////////////////////////////////////////////
				//ensure this RL is valid
				if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
					dRL = dOldRL;

				continue;
			}
			else
			{
				MessageBeep(MB_ICONEXCLAMATION);
			}
			break;
		}
	}
	while( true );
}

//////////////////////////////////////////////////////////////////////////////////
//ProcessArcThirdPoint
//
Acad::ErrorStatus  AutoBuildTools::ProcessArcThirdPoint( double &dRL )
{
	int		iRet;
	TCHAR	keywrd[512];
	double	dOldRL;
	CString	sKeyWrd, sPrompt;

//	SetMouseTrack(true);
	SetArcFlag( true );
	SetFirstPointFlag(true);

	//get the third point from the user, using the first 2 points as begining and end.
	acedInitGet( RSG_OTHER, _T("Wall RelativeLevel"));

	//this is the Second RL for an arc, this will become the
	//	1st RL if we continue in Arc mode
	m_dRL[MP_3RD] = dRL;

	//create the acad prompt
  sPrompt.Format( _T("\nSpecify End point of Arc or [Wall/RL<%0.2f>]:"), dRL );
	switch( iRet=GetPoint3D( &m_ptMousePoint[MP_1ST], sPrompt, m_ptMousePoint[MP_3RD]) )
	{
		case( RTNONE ):				//fallthrough (5000) No result
		case( RTERROR ):			//fallthrough (-5001) Some other error
		default:
			//This should not have occured
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);
			return Acad::eInetUnknownError;	//Unknown error
		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
			RefreshScreen( false );
			MessageBeep(MB_ICONEXCLAMATION);
			return Acad::eUserBreak;
		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
			acedGetInput( keywrd );
			sKeyWrd = keywrd;
			sKeyWrd.MakeUpper();

			if ( sKeyWrd[0]==_T('W') )
			{
				ReturnToWallMode();

				//we need to go back to the wall
				break;
			}
			else if ( _istdigit(keywrd[0]) || keywrd[0]==_T('-') )
			{
				dOldRL = dRL;

				dRL = _tstof( keywrd );

				RefreshScreen();

				///////////////////////////////////////////////////////
				//ensure this RL is valid
				if( !CompareRLWithABBayRL( dRL, GetTopRLFromAutobuild() ) )
					dRL = dOldRL;

				//Ask for the point again, we can do this recursively since the point is
				//	global and will not be changed by this recursion of this function
				return ProcessArcThirdPoint( dRL );
			}
			else
			{
				MessageBeep(MB_ICONEXCLAMATION);
			}
			break;

		case( RTNORM ):				//(5100) Request succeeded
			//create the bays
			m_bAddToDB = true;
			DrawArcBays( false );
			m_bAddToDB = false;

//			SetMouseTrack(false);	//NOTE: This is used to signify that it is not returning to wall mode so change it with care
			SetArcFlag( false );
			SetFirstPointFlag(true);
			m_ptMousePoint[MP_1ST] = m_ptMousePoint[MP_3RD];

			//refresh the screen
			RefreshScreen();
			break;
	}

	//This should only occur if RTNORM or RTKWORD+'W' was set
	return Acad::eOk;
}

void AutoBuildTools::DrawBirdcage( bool bErase )
{
	int			i, iBPT, iBPB, iNumberOfRunsToDraw, iNumberOfRuns, iFrom, iTo;
	Run			*pPreviousRun;
	Bay			*pPreviousBay, *pABBay;
	bool		bNESC, bSESC, bSWSC, bNWSC,
				bNB, bEB, bSB, bWB,
				bNHR, bEHR, bSHR, bWHR,
				bNNETie, bENETie, bESETie, bSSETie,
				bSSWTie, bWSWTie, bWNWTie, bNNWTie;
	double		dAngle, dBirdCageWidth, dFirstBayYPos,
				dWNS, dWES, dWSS, dWWS, dNumberOfRuns, dWidthOfSStage;
	Point3D		ptBirdcagePoints[MP_SIZE], ptMovedPoints[MP_SIZE],
				ptTemp, ptRLPlane[MP_SIZE];
	Matrix3D	Rotation, Transform, Trans;
	Vector3D	Vector;
	BayTemplate	*pABBayTemplate;

	pABBay = GetABBay();
	pABBayTemplate = pABBay->GetTemplate();

	////////////////////////////////////////////////////////////
	//We now must have 3 RL's which define a plane!  We will need to calculate
	//	the RL for the base of each standard on this plane.  We will need to
	//	do an intersection test between a line and a plane to determine this point!
	//	Firstly lets define the equation for the plane!
	//
	//	We know - m_dRL[MP_1ST] and m_ptMousePoint[MP_1ST]
	//			- m_dRL[MP_2ND] and m_ptMousePoint[MP_2ND]
	//			- m_dRL[MP_3RD] and m_ptMousePoint[MP_3RD]
	for( i=(int)MP_1ST; i<(int)MP_SIZE; i++ )
	{
		ptRLPlane[i] = m_ptBirdcagePoints[i];
		ptRLPlane[i].z = m_dRL[i];
	}
	ptRLPlane[MP_1ST].z = m_dRLFirstBirdcage;

	////////////////////////////////////////////////////////////
	//temporarity store the original points
	for( i=MP_1ST; i<MP_SIZE; i++ )
	{
		ptBirdcagePoints[i] = m_ptMousePoint[i];
	}

	////////////////////////////////////////////////////////////
	//We need to use the first two points that we stored
	for( i=MP_1ST; i<MP_SIZE; i++ )
	{
		m_ptMousePoint[i] = m_ptBirdcagePoints[i];
	}
	
	////////////////////////////////////////////////////////////
	//rotate the points around the 1st point until 2nd point is on x axis
	ptTemp.x = m_ptMousePoint[MP_2ND].x-m_ptMousePoint[MP_1ST].x;
	ptTemp.y = m_ptMousePoint[MP_2ND].y-m_ptMousePoint[MP_1ST].y;
	ptTemp.z = m_ptMousePoint[MP_2ND].z-m_ptMousePoint[MP_1ST].z;
	dAngle = -1.00*CalculateAngle( ptTemp );
	Rotation.setToRotation( dAngle, VECTOR_ROTATE_AROUND_ZAXIS, m_ptMousePoint[MP_1ST] );

	////////////////////////////////////////////////////////////
	//apply the rotation
	for( i=MP_1ST; i<MP_SIZE; i++ )
	{
		m_ptMousePoint[i] = Rotation*m_ptMousePoint[i];
	}

	////////////////////////////////////////////////////////////
	//move the 3rd point to be above the 2nd point (same x-value)
	m_ptMousePoint[MP_3RD].x = m_ptMousePoint[MP_2ND].x;

	dBirdCageWidth = m_ptMousePoint[MP_3RD].y - m_ptMousePoint[MP_2ND].y;

	////////////////////////////////////////////////////////////
	//copy the points temporarily
	for( i=MP_1ST; i<MP_SIZE; i++ )
	{
		ptMovedPoints[i] = m_ptMousePoint[i];
	}

	////////////////////////////////////////////////////////////
	//look at the properties of the Bay
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	//skip over the first run, which we have already drawn
	pPreviousRun = GetController()->GetRun( m_iPreviousRunID );
	assert( pPreviousRun!=NULL );
	pPreviousBay = pPreviousRun->GetBay( 0 );
	if( dBirdCageWidth>0 )
	{
		m_bNortherBirdcage = true;
		//They have dragged the mouse north of the first bay
		dFirstBayYPos = pPreviousBay->GetTemplate()->GetBayWidthActual()
						+ pABBayTemplate->GetWidthOfSStage();
		dBirdCageWidth-= (dFirstBayYPos+ GetDistanceFromWall());
		if( dBirdCageWidth<=ROUND_ERROR )
			return;
	}
	else
	{
		m_bNortherBirdcage = false;

		//This is a case where they are draggin in a southerly direction from the last bay

		//find the top of the new bay such that the top touches the previous bay's standards
		dFirstBayYPos = pPreviousBay->GetTemplate()->GetWidthOfSStage();

		dBirdCageWidth-= dFirstBayYPos;

		if( dBirdCageWidth>-1.00*ROUND_ERROR )
			return;

		//We have enough to draw the first bay!
	}

	//Create transform for first new run's SW point
	Vector.set( 0.00, dFirstBayYPos, 0.00 ); 
	Transform.setToTranslation( Vector );
	//Move temp points
	for( i=MP_1ST; i<MP_3RD; i++ )
	{
		ptMovedPoints[i] = Transform*ptMovedPoints[i];
	}

	////////////////////////////////////////////////////////////
	//store the ties, braces, & stages
	dWNS	= pABBayTemplate->GetWidthOfNStage( );
	dWES	= pABBayTemplate->GetWidthOfEStage( );
	dWSS	= pABBayTemplate->GetWidthOfSStage( );
	dWWS	= pABBayTemplate->GetWidthOfWStage( );
	bNESC	= pABBayTemplate->GetNEStageCnr( );
	bSESC	= pABBayTemplate->GetSEStageCnr( );
	bSWSC	= pABBayTemplate->GetSWStageCnr( );
	bNWSC	= pABBayTemplate->GetNWStageCnr( );
	bNB		= pABBayTemplate->GetNBrace( );
	bEB		= pABBayTemplate->GetEBrace( );
	bSB		= pABBayTemplate->GetSBrace( );
	bWB		= pABBayTemplate->GetWBrace( );
	iBPT	= GetRunTemplate()->GetBaysPerTie( );
	iBPB	= GetRunTemplate()->GetBaysPerBrace( );
	bNHR	= pABBayTemplate->GetNHandRail( );
	bEHR	= pABBayTemplate->GetEHandRail( );
	bSHR	= pABBayTemplate->GetSHandRail( );
	bWHR	= pABBayTemplate->GetWHandRail( );
	bNNETie = pABBayTemplate->GetNNETie( );
	bENETie = pABBayTemplate->GetENETie( );
	bESETie = pABBayTemplate->GetESETie( );
	bSSETie = pABBayTemplate->GetSSETie( );
	bSSWTie = pABBayTemplate->GetSSWTie( );
	bWSWTie = pABBayTemplate->GetWSWTie( );
	bWNWTie = pABBayTemplate->GetWNWTie( );
	bNNWTie = pABBayTemplate->GetNNWTie( );

	if( m_bNortherBirdcage )
	{
		pABBayTemplate->SetWidthOfSStage( 0.00 );
		pABBayTemplate->SetSEStageCnr( false );
		pABBayTemplate->SetSWStageCnr( false );
		pABBayTemplate->SetSBrace( false );
		pABBayTemplate->SetSHandRail( false );
	}
	else
	{
		pABBayTemplate->SetWidthOfNStage( 0.00 );
		pABBayTemplate->SetNEStageCnr( false );
		pABBayTemplate->SetNWStageCnr( false );
		pABBayTemplate->SetNBrace( false );
		pABBayTemplate->SetNHandRail( false );
	}

	//"Birdcages don't need ties" (AGT 991026)
	pABBayTemplate->SetNNETie( false );
	pABBayTemplate->SetENETie( false );
	pABBayTemplate->SetESETie( false );
	pABBayTemplate->SetSSETie( false );
	pABBayTemplate->SetSSWTie( false );
	pABBayTemplate->SetWSWTie( false );
	pABBayTemplate->SetWNWTie( false );
	pABBayTemplate->SetNNWTie( false );

	////////////////////////////////////////////////////////////
	//work out the rotation matrix
	Rotation.setToRotation( dAngle*-1.00, VECTOR_ROTATE_AROUND_ZAXIS, m_ptMousePoint[MP_1ST] );

	////////////////////////////////////////////////////////////
	//Find how many runs we can add
	double	dSmallRunSize, dRemainder;
	dWidthOfSStage = pABBayTemplate->GetWidthOfSStage();
	dSmallRunSize = 0.00;
	if( m_bNortherBirdcage )
	{
		dNumberOfRuns = dBirdCageWidth/GetABBay()->GetBayWidthActual();
	}
	else
	{
		//southern birdcages need to account for the stage board
		dNumberOfRuns = (fabs(dBirdCageWidth)-dWidthOfSStage-GetDistanceFromWall())/GetABBay()->GetBayWidthActual();
//		dNumberOfRuns-=1.00;
	}
	iNumberOfRuns = int(dNumberOfRuns);

	if( GetRunTemplate()->GetFit()==RF_TIGHT )
	{
		//////////////////////////////////////////////////////
		//find which bays will fit
		if( m_bNortherBirdcage )
		{
			dRemainder = dBirdCageWidth-(double)(iNumberOfRuns*pABBay->GetBayWidthActual());
		}
		else
		{
			dRemainder = (fabs(dBirdCageWidth)-dWidthOfSStage-GetDistanceFromWall())-(double(iNumberOfRuns)*pABBay->GetBayWidthActual());
		}

		if( dRemainder>=m_dBayLengthActual1800-ROUND_ERROR )
			dSmallRunSize = m_dBayLengthActual1800;
		else if( dRemainder>=m_dBayLengthActual1200-ROUND_ERROR )
			dSmallRunSize = m_dBayLengthActual1200;
		else if( dRemainder>=m_dBayLengthActual0700-ROUND_ERROR )
			dSmallRunSize = m_dBayLengthActual0700;
		else
			dSmallRunSize = 0.00;

		if( dSmallRunSize>0.00+ROUND_ERROR )
		{
			dNumberOfRuns+=1.00;
			iNumberOfRuns++;
		}
	}

/*	if( (dNumberOfRuns-(double)iNumberOfRuns)>ROUND_ERROR )
		iNumberOfRuns++;
*/
	iNumberOfRunsToDraw = iNumberOfRuns;

	int iMiddleRunNumber;
	if( DRAW_RIGHT_OF_CENTRE )
		iMiddleRunNumber = iNumberOfRunsToDraw-(iNumberOfRunsToDraw/2);
	else
		iMiddleRunNumber = (iNumberOfRunsToDraw/2)+1;

	if( iNumberOfRunsToDraw==1 )
		iMiddleRunNumber = 1;

	double dIncr, dBirdCageOriginalWidth;
	if( m_bAddToDB && iNumberOfRunsToDraw>0 )
	{
		//Drawing the runs is approximately 60% of the effort here
		dIncr = 60.00/(double)iNumberOfRunsToDraw;
	}
	while( iNumberOfRunsToDraw>0 || dSmallRunSize>0.00+ROUND_ERROR )
	{
		//we need this value to tell the run not to overrun by the width of the bay
		//	otherwise it will overrun by the incorrect amount!
		dBirdCageOriginalWidth	= 0.00;
		m_dBirdCageOriginalWidthActual =0.00;
		if( dSmallRunSize>0.00 && (iNumberOfRunsToDraw==iMiddleRunNumber) )
		{
			dBirdCageOriginalWidth			= (double)pABBayTemplate->GetBayWidth();
			m_dBirdCageOriginalWidthActual	= (double)pABBayTemplate->GetBayWidthActual();
			SetBayWidth( GetController()->GetCompDetails()->GetCommonLength( GetSystem(), CT_LEDGER, dSmallRunSize-m_dStandardWidthActual, MT_STEEL ));
		}

		if( !m_bNortherBirdcage )
		{
			////////////////////////////////////////////////////////////
			//work out the transformation matrix
			Vector.set( 0.00, -1.00*GetABBay()->GetBayWidthActual(), 0.00 ); 
			if( iNumberOfRunsToDraw==1 )
			{
				//We are going to draw a stageboard on this one so
				//	move the mouse points to compensate
				Vector.y -= pABBayTemplate->GetWidthOfSStage();
			}
			Transform.setToTranslation( Vector );

			////////////////////////////////////////////////////////////
			//move the mouse points
			for( i=MP_1ST; i<MP_3RD; i++ )
			{
				ptMovedPoints[i] = Transform*ptMovedPoints[i];
			}
		}

		////////////////////////////////////////////////////////////
		//Copy temp points back
		for( i=MP_1ST; i<MP_SIZE; i++ )
		{
			m_ptMousePoint[i] = ptMovedPoints[i];
		}

		////////////////////////////////////////////////////////////
		//rotate the points back to their original location
		for( i=MP_1ST; i<MP_SIZE; i++ )
		{
			m_ptMousePoint[i] = Rotation*m_ptMousePoint[i];
		}

		////////////////////////////////////////////////////////////
		//what are the z coordinates of the points?
		if( !FindZOfXYPointOnPlane( m_ptMousePoint[MP_1ST], ptRLPlane) )
		{
			m_ptMousePoint[MP_1ST].z = 0.00;
		}
		if( !FindZOfXYPointOnPlane( m_ptMousePoint[MP_2ND], ptRLPlane) )
		{
			m_ptMousePoint[MP_2ND].z = 0.00;
		}

		m_bCalcRLErrorShown = false;

		////////////////////////////////////////////////////////////
		//birdcages may not have ties, stages or braces
		//North
		if( iNumberOfRunsToDraw>1 )
		{
			if( m_bNortherBirdcage )
			{
				pABBayTemplate->SetWidthOfNStage( 0.00 );
				pABBayTemplate->SetNEStageCnr( false );
				pABBayTemplate->SetNWStageCnr( false );
				pABBayTemplate->SetNBrace( false );
				pABBayTemplate->SetNHandRail( false );
				GetRunTemplate()->SetBaysPerBrace( 0 );
			}
			else
			{
				//South
				pABBayTemplate->SetWidthOfSStage( 0.00 );
				pABBayTemplate->SetSEStageCnr( false );
				pABBayTemplate->SetSWStageCnr( false );
				pABBayTemplate->SetSBrace( false );
				pABBayTemplate->SetSHandRail( false );
				GetRunTemplate()->SetBaysPerBrace( 0 );
			}
		}
		
		if( m_bAddToDB )
		{
			iFrom	= (int)(dIncr*(double)(iNumberOfRuns-iNumberOfRunsToDraw));
			iTo		= (int)(dIncr*(double)(iNumberOfRuns-(iNumberOfRunsToDraw-1)));
		}
		else
		{
			iFrom	= 0;
			iTo		= iFrom;
		}

		DrawRun( bErase, false, false, iFrom, iTo );

		//restore settings if they were changed
		if( iNumberOfRunsToDraw>1 )
		{
			if( m_bNortherBirdcage )
			{
				pABBayTemplate->SetWidthOfNStage( dWNS );
				pABBayTemplate->SetNEStageCnr( bNESC );
				pABBayTemplate->SetNWStageCnr( bNWSC );
				pABBayTemplate->SetNBrace( bNB );
				GetRunTemplate()->SetBaysPerBrace( iBPB );
				pABBayTemplate->SetNHandRail( bNHR );
			}
			else
			{
				pABBayTemplate->SetWidthOfSStage( dWSS );
				pABBayTemplate->SetSEStageCnr( bSESC );
				pABBayTemplate->SetSWStageCnr( bSWSC );
				pABBayTemplate->SetSBrace( bSB );
				GetRunTemplate()->SetBaysPerBrace( iBPB );
				pABBayTemplate->SetSHandRail( bSHR );
			}
		}

		if( m_bNortherBirdcage )
		{
			////////////////////////////////////////////////////////////
			//work out the transformation matrix
			Vector.set( 0.00, GetABBay()->GetBayWidthActual(), 0.00 ); 
			Vector.y*= (m_bNortherBirdcage)? 1.00: -1.00;
			Transform.setToTranslation( Vector );

			////////////////////////////////////////////////////////////
			//move the mouse points
			for( i=MP_1ST; i<MP_3RD; i++ )
			{
				ptMovedPoints[i] = Transform*ptMovedPoints[i];
			}
		}

		//restore the run width
		if( dSmallRunSize>0 && (iNumberOfRunsToDraw==iMiddleRunNumber) )
		{
			SetBayWidth(dBirdCageOriginalWidth);
			//ensure it doesn't come back in here again
			dSmallRunSize=0.00;
		}

		iNumberOfRunsToDraw--;
	}

	if( m_bAddToDB )
	{
		int			iRunID, iPreviousRunID, iSize, j, iBraceBay;
		Run			*pRun;
		Bay			*pBay;
		bool		bFound, bWesternBracing, bEasternBracing, 
					bRunReqBracing, bPrevRunReqBracing;
		Lift		*pLift;
		Component	*pComponent;
		Vector3D	Vector;
		Matrix3D	TransOrigRun, TransOrigRunSch, Trans;
		intArray	iaBaysWithBracing;

		acedSetStatusBarProgressMeterPos( 65 );

		////////////////////////////////////////////////////////////
		//Set all the innner and outer pointers
		iNumberOfRunsToDraw = iNumberOfRuns;

		////////////////////////////////////////////////////////////
		//The first bay will be moved if we Delete stageboards on the south
		iRunID = GetController()->GetNumberOfRuns()-iNumberOfRunsToDraw;
		iPreviousRunID = m_iPreviousRunID;
		
		//If we Delete the SouthSouthWestHopup, then we must be Deleteing the 
		//	Southern Stageboards, thus we must offset the whole bay North by 
		//	the width of the hopup
		assert( iRunID>=0 );
		assert( iRunID<GetController()->GetNumberOfRuns() );
		assert( iPreviousRunID>=0 );
		assert( iPreviousRunID<GetController()->GetNumberOfRuns() );

		pPreviousRun = GetController()->GetRun( iPreviousRunID );
		pPreviousBay = pPreviousRun->GetBay(0);

		acedSetStatusBarProgressMeterPos( 70 );
		if( !m_bNortherBirdcage )
		{
			bFound = false;
			for( i=0; i<pPreviousBay->GetNumberOfLifts(); i++ )
			{
				pLift = pPreviousBay->GetLift( i );
				pComponent = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, WSW );
				if( pComponent!=NULL )
				{
					bFound = true;
					break;
				}
			}
			if( bFound )
			{
				TransOrigRun	= pPreviousRun->UnMove();
				TransOrigRunSch	= pPreviousRun->UnMoveSchematic();

				Vector.set( 0.00, pComponent->GetLengthActual(), 0.00 );
				Trans.setToTranslation( Vector );

				pPreviousRun->Move( TransOrigRun*Trans, true );
				pPreviousRun->MoveSchematic( TransOrigRunSch*Trans, true );
			}
		}

		//////////////////////////////////////////////////////
		//Calculate Pascal's Triangle for Bracing
		CreatePascalsTriangle( GetRunTemplate()->GetBaysPerBrace(),
								iNumberOfRunsToDraw+1, iaBaysWithBracing );

		////////////////////////////////////////////////////////////////
		//Does the first bay have bracing?
		pPreviousBay = pPreviousRun->GetBay(0);
		bWesternBracing = pPreviousBay->DoesABayComponentExistOnASide( CT_BRACING, WEST );

		////////////////////////////////////////////////////////////////
		//Does the last bay have bracing?
		pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
		bEasternBracing = pPreviousBay->DoesABayComponentExistOnASide( CT_BRACING, EAST );

		dIncr = 30.00/(double)iNumberOfRunsToDraw;
		int iNumberOfRun = iNumberOfRunsToDraw;
		while( iNumberOfRunsToDraw>0 )
		{
			iRunID = GetController()->GetNumberOfRuns()-iNumberOfRunsToDraw;
			iPreviousRunID = iRunID-1;
			
			//They must both be valid runs!
			assert( iRunID>=0 );
			assert( iRunID<GetController()->GetNumberOfRuns() );
			assert( iPreviousRunID>=0 );
			assert( iPreviousRunID<GetController()->GetNumberOfRuns() );

			pRun = GetController()->GetRun( iRunID );
			pPreviousRun = GetController()->GetRun( iPreviousRunID );
			assert( pRun!=NULL );
			assert( pPreviousRun!=NULL );

			/////////////////////////////////////////////////////////////////
			//according to the pascal's triangle does this run require bracing?
			bRunReqBracing = false;
			bPrevRunReqBracing = false;
			for( j=0; j<iaBaysWithBracing.GetSize(); j++ )
			{
				iBraceBay = iaBaysWithBracing[j];
				if( iBraceBay==iRunID-(GetController()->GetNumberOfRuns()-(iNumberOfRuns+1)) )
				{
					bRunReqBracing = true;
				}
				if( iBraceBay==iPreviousRunID-(GetController()->GetNumberOfRuns()-(iNumberOfRuns+1)) )
				{
					bPrevRunReqBracing = true;
				}
			}

			acedSetStatusBarProgressMeterPos( ((int)(dIncr*(iNumberOfRun-(iNumberOfRunsToDraw+1))))+70 );

			/////////////////////////////////////////////////////////////////////
			//go through and set pointers bracing etc for each bay in the run!
			iSize = pRun->GetNumberOfBays();
			assert( iSize==pPreviousRun->GetNumberOfBays() );
			for( i=0; i<iSize; i++ )
			{
				pBay = pRun->GetBay(i);
				pPreviousBay = pPreviousRun->GetBay(i);

				assert( pBay!=NULL );
				assert( pPreviousBay!=NULL );
				assert( pPreviousBay!=pBay );

				//////////////////////////////////////////////////////////////
				//if this is the first bay in the run
				if( i==0 )
				{
					/////////////////////////////////////////////////////////////////
					//setup the western bracing
					if( !bWesternBracing || !bRunReqBracing )
					{
						//no western bracing
						pBay->DeleteBrace( WEST );
					}

					/////////////////////////////////////////////////////////////////
					//setup the western bracing
					if( !bWesternBracing || !bPrevRunReqBracing )
					{
						//no western bracing
						pPreviousBay->DeleteBrace( WEST );
					}
				}

				//////////////////////////////////////////////////////////////
				//if this is the last bay in the run
				if( i==iSize-1 )
				{
					/////////////////////////////////////////////////////////////////
					//setup the western bracing
					if( !bEasternBracing || !bRunReqBracing )
					{
						//no western bracing
						pBay->DeleteBrace( EAST );
					}

					/////////////////////////////////////////////////////////////////
					//setup the western bracing
					if( !bEasternBracing || !bPrevRunReqBracing )
					{
						//no western bracing
						pPreviousBay->DeleteBrace( EAST );
					}
				}

				if( m_bNortherBirdcage )
				{
					pBay->SetInner( pPreviousBay );
					pPreviousBay->SetOuter( pBay );
				}
				else
				{
					pPreviousBay->SetInner( pBay );
					pBay->SetOuter( pPreviousBay );
				}

				////////////////////////////////////////////////////////////////////////////////
				//Whilst we are here we should set the RL's for all these bays
				SetRLsForBayByPlane( pBay, ptRLPlane );
				pBay->UpdateSchematicView();
				if( iNumberOfRunsToDraw==0 )
				{
					//When we do the last bay we should also do the initial run of the birdcage
					SetRLsForBayByPlane( pPreviousBay, ptRLPlane );
					pPreviousBay->UpdateSchematicView();
				}
			}
			iNumberOfRunsToDraw--;
		}
		acedRestoreStatusBar();
	}

	////////////////////////////////////////////////////////////
	//restore the original points
	for( i=MP_1ST; i<MP_SIZE; i++ )
	{
		m_ptMousePoint[i] = ptBirdcagePoints[i];
	}

	////////////////////////////////////////////////////////////
	//Restore ties, stages or braces
	pABBayTemplate->SetWidthOfNStage( dWNS );
	pABBayTemplate->SetWidthOfEStage( dWES );
	pABBayTemplate->SetWidthOfSStage( dWSS );
	pABBayTemplate->SetWidthOfWStage( dWWS );
	pABBayTemplate->SetNEStageCnr( bNESC );
	pABBayTemplate->SetSEStageCnr( bSESC );
	pABBayTemplate->SetSWStageCnr( bSWSC );
	pABBayTemplate->SetNWStageCnr( bNWSC );
	pABBayTemplate->SetNBrace( bNB );
	pABBayTemplate->SetEBrace( bEB );
	pABBayTemplate->SetSBrace( bSB );
	pABBayTemplate->SetWBrace( bWB );
	GetRunTemplate()->SetBaysPerTie( iBPT );
	GetRunTemplate()->SetBaysPerBrace( iBPB );
	pABBayTemplate->SetNHandRail( bNHR );
	pABBayTemplate->SetEHandRail( bEHR );
	pABBayTemplate->SetSHandRail( bSHR );
	pABBayTemplate->SetWHandRail( bWHR );
	pABBayTemplate->SetNNETie( bNNETie );
	pABBayTemplate->SetENETie( bENETie );
	pABBayTemplate->SetESETie( bESETie );
	pABBayTemplate->SetSSETie( bSSETie );
	pABBayTemplate->SetSSWTie( bSSWTie );
	pABBayTemplate->SetWSWTie( bWSWTie );
	pABBayTemplate->SetWNWTie( bWNWTie );
	pABBayTemplate->SetNNWTie( bNNWTie );
}

void AutoBuildTools::DrawRun( bool bErase, bool bStoreLastPoint/*=false*/, bool bArc/*=false*/, int iStart/*=0*/, int iFin/*=0*/ )
{
	Point3D ptOld1stPoint, ptOld2ndPoint, ptNew2ndPoint;

	//////////////////////////////////////////////////////
	//store the 2nd mouse point, since overrunning will move it
	ptOld1stPoint	= m_ptMousePoint[MP_1ST];
	ptOld2ndPoint	= m_ptMousePoint[MP_2ND];
	
	if( DrawRun( bErase, iStart, iFin, ptNew2ndPoint, bStoreLastPoint, bArc ) && m_bAddToDB )
	{
		ptOld2ndPoint = ptNew2ndPoint;
	}

	//////////////////////////////////////////////////////
	//restore the mouse points
	m_ptMousePoint[MP_1ST] = ptOld1stPoint;
	m_ptMousePoint[MP_2ND] = ptOld2ndPoint;
}

//////////////////////////////////////////////////////////////////////////////////
//DrawRun
//
bool AutoBuildTools::DrawRun( bool bErase, int iStart, int iFin, Point3D &ptNew2ndPoint, bool bStoreLastPoint/*=false*/, bool bArc/*=false*/ )
{
	int					iNumberOfBaysToDraw, iBaysRemaining, iMiddleBayNumber, iRunID,
							iBayID, iBay, i, iSize, iPreviousRunID;
	Run					*pRun, *pPreviousRun;
	Bay					*pBay, *pABBay, *pPreviousBay;
	bool				bTie, bLastRunBoxExists, bNESC, bSESC, bSWSC, bNWSC,
						bNB, bEB, bSB, bWB, bEHR, bWHR, bUseBoundingBoxCorner,
						bUseNew2ndPoint, bLeft;
	double				dDistance, dSmallBayDistance, dOriginalLength, dOffset,
						dAngle, dNewAngle, dMouseSeparation, dWidth, dWES, dWWS, dWall;
	Point3D				ptSWStd, ptSEStd, pt,
						ptFirst, ptSecond, ptRLRun[2], ptTemp1, ptTemp2;
	Vector3D			Vector, Vect;
	Matrix3D			Transform, Rotation, Trans;
	BayTemplate			*pABBayTemplate, *pBayTemplate;
	AcDbIntArray		osnapModes, geomIds;
	Point3DArray		TempBoundBox, *pPreviousRunBoundBox,
						*pCurrentRunBoundBox;

	pABBay = GetABBay();
	pABBayTemplate = pABBay->GetTemplate();
	double dWidthNStage = pABBay->GetWidthOfStageFromWall( N );
	double dWidthEStage = pABBay->GetWidthOfStageFromWall( E );
	double dWidthSStage = pABBay->GetWidthOfStageFromWall( S );
	double dWidthWStage = pABBay->GetWidthOfStageFromWall( W );

	if( m_bAddToDB )
	{
		dSmallBayDistance		= 0.00;
	}

	///////////////////////////////////////////////////////////////////
	dSmallBayDistance		= 0.00;
	bLastRunBoxExists		= false;
	bUseNew2ndPoint			= false;
	dWall					= GetDistanceFromWall();
	ptNew2ndPoint.set( 0.00, 0.00, 0.00 );

	///////////////////////////////////////////////////////////////////
	//Bounding box stuff
	pCurrentRunBoundBox		= new Point3DArray();
	pPreviousRunBoundBox	= NULL;
	iPreviousRunID			= m_iPreviousRunID;
	//if there is not previous run, or if this is the first run in an
	//	autobuild then we DON'T want to avoid the last bay
	if( !bArc && iPreviousRunID>=0 && !IsFirstRunInAutobuild() && !m_bBirdCageMode )
	{
		pRun = GetController()->GetRun( iPreviousRunID );
		assert( pRun!=NULL );
		if( pRun->GetAutobuildNumber()==giAutobuildNumber )
		{
			pPreviousRunBoundBox = GetController()->GetRun(iPreviousRunID)->
									GetRunTemplate()->GetBoundingBoxPointer();
			bLastRunBoxExists	= ( !m_bBirdCageMode
									&& pPreviousRunBoundBox->length()>0 );
		}
	}

	Vect = m_ptMousePoint[MP_2ND]-m_ptMousePoint[MP_1ST];
	dNewAngle = CalculateAngle( Vect );
	dAngle = dNewAngle;

	//if the 2nd point is 'left' of the previous bounding box then
	//	the 1st point should move to the bounding box corner
	//	(-distance form wall.x and -distance form wall.y)
	if( pPreviousRunBoundBox!=NULL )
	{
		Point3D	pt1, pt2;
		
		///////////////////////////////////////////////////////////
		//Key for Cases below
		//A = pPreviousRunBoundBox[1]
		//B = pPreviousRunBoundBox[2]
		//C = m_ptMousePoint[MP_1ST]
		//D = m_ptMousePoint[MP_2ND]

		////////////////////////////////////////////////////////
		//         Case: 'left' of previous                   //
		//                  ****   ****                       //
		//             ***               ***      /\          //
		//          *                         * /    \        //
		//        *                           / *      \D     //
		//      *                           /     *   /       //
		//     *                          /        */         //
		//    *           _______________\        / *         //
		//    *          |               | \    /   *         //
		//   *           |               |   \/      *        //
		//   *           |               |  /        *        //
		//   V           |_______________|/          V        //
		//	             A               BC                   //
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		//         Case: 'right' of previous                  //
		//               |               |                    //
		//               |               |                    //
		//               |               |                    //
		//               |_______________| /\                 //
		//   ^           A           C   B    \      ^        //
		//   *                        \/        \    *        //
		//   *                          \         \  *        //
		//    *                           \         *         //
		//    *                             \       * \       //
		//     *                              \    *    \     //
		//      *                               \ *      /    //
		//        *                             * \    /      //
		//          *                         *     \/        //
		//            ***                ***        D         //
		//                 ****    ****                      //
		////////////////////////////////////////////////////////
		
		//The trick we have to perform here is when the 2nd point moves
		//	to the 'left' we need to move m_ptMousePoint[MP_1ST] to be
		//	DistanceFromWall below point pPreviousRunBoundBox[2]!
		////////////////////////////////////////////////////////
		//Find out if we are 'left' or 'right'
		ptSWStd = pPreviousRunBoundBox->at(1);
		ptSEStd = pPreviousRunBoundBox->at(2);
		Vect = ptSEStd-ptSWStd;
		dNewAngle-= CalculateAngle( Vect );
		if( dNewAngle<dminus90Deg )
			dNewAngle+= d360Deg;
		if( dNewAngle>d270Deg )
			dNewAngle-= d360Deg;

		bLeft = false;
		if( (dNewAngle>=d0Deg) && (dNewAngle<d180Deg) )
			bLeft=true;

		if( bLeft )
		{
			//we are on the 'left' (see diagram, above), we need to move C to B
#ifdef _SHOW_ANGLES_1
      acutPrintf(_T("\nLeft Angle:%0.2f"), dNewAngle );
#endif	//#ifdef _SHOW_ANGLES_1
			//retain the existing z value
			ptSEStd.z = m_ptMousePoint[MP_1ST].z;
			m_ptMousePoint[MP_1ST] = ptSEStd;
		}
		else
		{
			//we are on the 'right' (see diagram, above), point C can stay where it is!
#ifdef _SHOW_ANGLES_2
      acutPrintf(_T("\nRight Angle:%0.2f"), dNewAngle );
#endif	//#ifdef _SHOW_ANGLES_2
		}

		///////////////////////////////////////////////////////////
		//
		if( acdbHostApplicationServices()->workingDatabase()->orthomode() )
		{
#ifdef _SHOW_ANGLES_3
      acutPrintf(_T("\nAngle:%0.2f"), dAngle*CONVERT_TO_DEG );
			acutPrintf(_T("\t[%1.0f - %1.0f - %1.0f - %1.0f]"),
							(dminus45Deg*CONVERT_TO_DEG ),
							(d45Deg*CONVERT_TO_DEG ),
							(d135Deg*CONVERT_TO_DEG ),
							(d225Deg*CONVERT_TO_DEG ) );

#endif	//#ifdef _SHOW_ANGLES_3

			if( (dAngle>dminus45Deg) && (dAngle<=d45Deg) )
			{
#ifdef _SHOW_ANGLES_3
				acutPrintf(_T("(A)") );
#endif	//#ifdef _SHOW_ANGLES_3
				m_ptMousePoint[MP_2ND].y = m_ptMousePoint[MP_1ST].y;
			}
			else if( dAngle>d45Deg && dAngle<=d135Deg )
			{
#ifdef _SHOW_ANGLES_3
				acutPrintf(_T("(B)") );
#endif	//#ifdef _SHOW_ANGLES_3
				m_ptMousePoint[MP_2ND].x = m_ptMousePoint[MP_1ST].x;
			}
			else if( dAngle>d135Deg && dAngle<=d225Deg )
			{
#ifdef _SHOW_ANGLES_3
				acutPrintf(_T("(C)") );
#endif	//#ifdef _SHOW_ANGLES_3
				m_ptMousePoint[MP_2ND].y = m_ptMousePoint[MP_1ST].y;
			}
			else
			{
#ifdef _SHOW_ANGLES_3
				acutPrintf(_T("(D)") );
#endif	//#ifdef _SHOW_ANGLES_3
				m_ptMousePoint[MP_2ND].x = m_ptMousePoint[MP_1ST].x;
			}
		}
	}

	////////////////////////////////////////////////////////
	//Overrun/Underrun

	if( m_bOverrunMousePoint )
	{
		double dDistOverrun;
		//if we are drawing a birdcage then we may have altered the width
		//	so we need to use the original width
		if( m_bBirdCageMode && m_dBirdCageOriginalWidthActual>0.00 )
			dDistOverrun = m_dBirdCageOriginalWidthActual+pABBay->GetWidthOfStageFromWall( SOUTH )+dWall;
		else
			dDistOverrun = pABBay->GetBayWidthActual()+pABBay->GetWidthOfStageFromWall( SOUTH )+dWall;

#ifdef	_SHOW_BIRDCAGE_DATA_
		if( m_bBirdCageMode )
		{
      acutPrintf( _T("\nDistOverrun: %0.1f, ABWidth: %0.1f, OriginalWidth: %0.1f, WidthOfSStage: %0.1f"),
					dDistOverrun, pABBay->GetBayWidthActual(),
					m_dBirdCageOriginalWidthActual, pABBay->GetWidthOfStageFromWall( SOUTH ) );
		}
#endif	//#ifdef	_SHOW_BIRDCAGE_DATA_

		Overrun2ndPoint( dDistOverrun );
	}
	else
	{
		//The arc already has a set distance, no need to compenstate
		if( !bArc )
			Overrun2ndPoint( -1.00*dWall );
	}


	dOffset = 0.00;
	if( !bArc && bLastRunBoxExists && !GetArcFlag() )
	{
		dOffset = DistToAvoidBoundBox( bUseBoundingBoxCorner, bErase, dAngle );
		dOffset = max( 0.00, dOffset );
	}

	pPreviousBay = NULL;
	if( !IsFirstRunInAutobuild() )
	{
		pPreviousRun = GetController()->GetRun( m_iPreviousRunID );
		if( pPreviousRun!=NULL )
		{
			pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
		}
	}

	///////////////////////////////////////////////////////////////////////
	//Mills system corner
	MillsType mtMillsCornerType = MILLS_TYPE_NONE;

	//The Autobuild bay MUST be 1200 wide or this won't work!
	if( UseMillsSystemCorner() )
	{
		if( (dAngle>d270Deg-dONE_DEGREE	&& dAngle<d270Deg+dONE_DEGREE) ||
			(dAngle>dminus90Deg-dONE_DEGREE	&& dAngle<dminus90Deg+dONE_DEGREE) )
		{
			//angle is directly south of previous bay
			mtMillsCornerType = MILLS_TYPE_CONNECT_SSE;
		}
		else if( dAngle>d90Deg-dONE_DEGREE	&& dAngle<d90Deg+dONE_DEGREE)
		{
			//angle is directly north of previous bay
			mtMillsCornerType = MILLS_TYPE_CONNECT_NNE;
		}

		if( mtMillsCornerType!=MILLS_TYPE_NONE)
		{
			if( pPreviousBay!=NULL )
			{
				if( !m_bBuildForward )
					mtMillsCornerType=MILLS_TYPE_NONE;

				if( pPreviousBay->CanBeMillsCnrBay()!=mtMillsCornerType )
					mtMillsCornerType=MILLS_TYPE_NONE;

				if( pPreviousBay->GetBayLength()!=COMPONENT_LENGTH_2400 )
				{
					//the previous bay MUST be 2400 long or this won't work!
					mtMillsCornerType=MILLS_TYPE_NONE;
				}
				else
				{
					if( m_bAddToDB )
						pPreviousBay->SetMillsSystemBay( mtMillsCornerType );

					if( mtMillsCornerType==MILLS_TYPE_CONNECT_SSE )
					{
						dOffset -= pPreviousBay->GetWidthOfStageFromWall( SOUTH );
						dOffset -= GetDistanceFromWall();
					}
					else
					{
						dOffset -= GetDistanceFromWall();
/*						m_dMillsPreviousSouthStageWidth = GetABBay()->GetTemplate()->GetWidthOfSStage();

						int iNewPlanks;
						double dNewWidth;
						dNewWidth = pPreviousBay->GetTemplate()->GetWidthOfEStage();
						//iNewPlanks = GetNumberOfPlanksForWidth( dNewWidth );
						iNewPlanks = GetABBay()->GetNumberOfPlanksOfBottomStage( SOUTH );
						//GetABBay()->AddHopupToBayIfReqd( SOUTH, iNewPlanks );
						GetABBay()->GetTemplate()->SetWidthOfSStage( dNewWidth );
*/					}
				}
			}
			else
			{
				//there is no previous bay
				dOffset -= GetABBay()->GetWidthOfStageFromWall( SOUTH );
				dOffset -= GetDistanceFromWall();
				mtMillsCornerType = MILLS_TYPE_NONE;
			}

			if( mtMillsCornerType!=MILLS_TYPE_NONE)
			{
				m_LBTplt.SetLapBoardReqdThisRun(false);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	//Sydney corner
	bool bSydneyCornerUsed = false;
	if( !bArc && GetController()->IsSydneyCorner() &&
		( (dAngle>d225Deg		&& dAngle<d315Deg) ||
		  (dAngle<dminus45Deg	&& dAngle>dminus135Deg) ) )
	{
		//This is an external sydney corner
		//	and the angle is within 45deg either side of directly south of previous bay
		if( pPreviousBay!=NULL )
		{
			if( m_bAddToDB )
				pPreviousBay->SetSydneyCornerBay( true );
			if( m_bBuildForward )
			{
				dOffset -= pPreviousBay->GetWidthOfStageFromWall( SOUTH );
				dOffset -= GetDistanceFromWall();
			}
			dOffset += GetController()->GetSydneyCornerDistBwBays();
		}
		else
		{
			dOffset -= GetABBay()->GetWidthOfStageFromWall( SOUTH );
			dOffset -= GetDistanceFromWall();
			dOffset += GetController()->GetSydneyCornerDistBwBays();
		}
		m_LBTplt.SetLapBoardReqdThisRun(true);
		bSydneyCornerUsed = true;

	}

	//This correction was done by eye
	if( mtMillsCornerType==MILLS_TYPE_NONE && !bSydneyCornerUsed && !m_bBuildForward && m_bOverrunMousePoint )
	{
		dOffset += GetABBay()->GetWidthOfStageFromWall( SOUTH );
		dOffset += GetDistanceFromWall();
	}

#ifdef	_DRAW_BOUND_BOX_
	DrawCrosshair( m_ptMousePoint[MP_1ST], 500.00, 500.00, bErase );
	DrawCrosshair( m_ptMousePoint[MP_2ND], 500.00, 500.00, bErase );
#endif	//#ifdef	_DRAW_BOUND_BOX_

	InsetPoints();

#ifdef	_DRAW_BOUND_BOX_
	DrawCrosshair( m_ptMousePoint[MP_1ST], 500.00, 500.00, bErase );
	DrawCrosshair( m_ptMousePoint[MP_2ND], 500.00, 500.00, bErase );
#endif	//#ifdef	_DRAW_BOUND_BOX_

	ptFirst			= m_ptMousePoint[MP_1ST];
	ptRLRun[0]		= ptFirst; 
	ptFirst.z		= 0.00;

	ptSecond		= m_ptMousePoint[MP_2ND];
	ptRLRun[1]		= ptSecond; 
	ptSecond.z		= 0.00;

	///////////////////////////////////////////////////////
	//if we overrun the mouse point then check the second point
	if( m_bOverrunMousePoint )
	{
		if( !m_bCalcRLErrorShown || m_bAddToDB )
		{
			//we have already checked the input as it was typed, however
			//	the RL for the second mouse point is calculated if there is
			//	an overrun situation!
			if( !CompareRLWithABBayRL( ptRLRun[1].z, GetTopRLFromAutobuild(), true ) )
			{
				m_bCalcRLErrorShown = true;
				return bUseNew2ndPoint;
			}
			m_bCalcRLErrorShown = false;
		}
	}

/*	//show the calculated RL's
	if( m_bAddToDB )
		acutPrintf("RL1:%1.2f RL2:%1.2f\n", ptRLRun[0].z, ptRLRun[1].z );
*/

	//////////////////////////////////////////////////////
	//how long is this run?
	ptTemp1.set( ptFirst.x, ptFirst.y, 0.00 );
	ptTemp2.set( ptSecond.x, ptSecond.y, 0.00 );
	dMouseSeparation = ptTemp1.distanceTo( ptTemp2 );
	dDistance = dMouseSeparation;
	dDistance-= dOffset;
	dDistance-= dWidthEStage;
	dDistance-= dWidthWStage;
	if( dDistance<=0 )
	{
		return bUseNew2ndPoint;
	}

	//////////////////////////////////////////////////////
	//How many whole bays can we fit? - round down
	iNumberOfBaysToDraw = (int)( dDistance/(double)pABBay->GetBayLengthActual());

	//////////////////////////////////////////////////////
	//if we are using a tight fit
	double dRemainder;
	dRemainder = 0.00;
	if( GetRunTemplate()->GetFit()==RF_TIGHT )
	{
		//////////////////////////////////////////////////////
		//find which bays will fit
		dRemainder = dDistance-(double)(iNumberOfBaysToDraw*pABBay->GetBayLengthActual());

		//////////////////////////////////////////////////////
		//what is the smallest size of smaller bay that we can use
		if( m_bOverrunMousePoint )
		{
			if( dRemainder==0.00 )
				dSmallBayDistance = 0.00;
			else if( dRemainder<=m_dBayLengthActual0700 )
				dSmallBayDistance = m_dBayLengthActual0700;
			else if( dRemainder<=m_dBayLengthActual1200 )
				dSmallBayDistance = m_dBayLengthActual1200;
			else if( dRemainder<=m_dBayLengthActual1800 )
				dSmallBayDistance = m_dBayLengthActual1800;
			else
				dSmallBayDistance = m_dBayLengthActual2400;
		}
		else
		{
			if( dRemainder>=m_dBayLengthActual1800 )
				dSmallBayDistance = m_dBayLengthActual1800;
			else if( dRemainder>=m_dBayLengthActual1200 )
				dSmallBayDistance = m_dBayLengthActual1200;
			else if( dRemainder>=m_dBayLengthActual0700 )
				dSmallBayDistance = m_dBayLengthActual0700;
			else
				dSmallBayDistance = 0.00;
		}
		dRemainder -= dSmallBayDistance;

		if( dSmallBayDistance>0.00 )
		{
			iNumberOfBaysToDraw++;
		}
	}
	dDistance-= dRemainder;

	//////////////////////////////////////////////////////
	//do we need an offset to avoid the previous bay?
	if( !GetArcFlag() )
	{
#ifdef	_DRAW_BOUND_BOX_
		DrawPolyLine( *pPreviousRunBoundBox, (bErase)? COLOUR_ERASE: COLOUR_DARK_GREY );
#endif	//#ifdef	_DRAW_BOUND_BOX_

		//remainder will always be zero from overrun
		if( !m_bBuildForward )
		{
			dOffset+= dRemainder;
		}
	}

	ptTemp1 = ptFirst;
	ptTemp2 = ptSecond;
	dOffset+= dWidthWStage;
	ptFirst.set(	(dOffset*((ptTemp2.x - ptTemp1.x)/dMouseSeparation))+ptTemp1.x,
					(dOffset*((ptTemp2.y - ptTemp1.y)/dMouseSeparation))+ptTemp1.y,
					(dOffset*((ptTemp2.z - ptTemp1.z)/dMouseSeparation))+ptTemp1.z );

	ptSecond.set(	(dDistance*((ptTemp2.x - ptTemp1.x)/dMouseSeparation))+ptFirst.x,
					(dDistance*((ptTemp2.y - ptTemp1.y)/dMouseSeparation))+ptFirst.y,
					(dDistance*((ptTemp2.z - ptTemp1.z)/dMouseSeparation))+ptFirst.z );
	double dSeparation = ptSecond.distanceTo( ptFirst );
	assert( dDistance<=dSeparation+0.01 &&
			dDistance>=dSeparation-0.01 );

	/////////////////////////////////////////////////////////////
	//calculate the transform matrix to apply to the bounding box
	Vect = ptSecond-ptFirst;
	dAngle = CalculateAngle( Vect );
	//create the rotation matix
	Rotation.setToRotation( dAngle, VECTOR_ROTATE_AROUND_ZAXIS );

	Vector.set( ptFirst.x, ptFirst.y, ptFirst.z );
	Transform.setToTranslation( Vector );
	Transform = Transform * Rotation;
	
	///////////////////////////////////////////////////////////
	//Calculate the new location for the second mouse point
	ptNew2ndPoint = ptSecond;
	if( ALLOW_NEW_SECOND_POINT )
		bUseNew2ndPoint			= true;


	//////////////////////////////////////////////////////
	//We will use the first point as the origin corner of the bay
	ptSWStd = ptFirst;
	
	////////////////////////////////////////////////////////////////////////
	//Create Bounding Box
	///////////////////////////////////////
	//      Create a box around above    //
	//        bounding box, at Wall      //
	//        separation thickness       //
	//        4                3  |      //
	//       0 x______________x___v      //
	//         |____________. |___ dWall //
	//         |            | |   ^      //
	//         |            | |   |      //
	//         |            | |          //
	//         |   ptSecond | |   |      //
	// ptFirst x____________x |___V      //
	//         |______________|___ dWall //
	//         x              x   ^      //
	//        1                2  |      //
	//         |            | |          //
	//       ->|<-       -->| |<--       //
	//        0.00         dWall         //
	//                                   //
	//Note: ptFirst->ptSecond==dDistance //
	///////////////////////////////////////

	///////////////////////////////////////////////////////////
	//Create the points
	dWidth = pABBay->GetBayWidthActual() + dWidthSStage + dWidthNStage;
	pCurrentRunBoundBox->RemoveAll();
	pCurrentRunBoundBox->append( Point3D( -dWidthWStage, dWidth+dWall-dWidthSStage+dWidthNStage, 0.00 ) );
	pCurrentRunBoundBox->append( Point3D( -dWidthWStage, -dWall-dWidthSStage, 0.00 ) );
	pCurrentRunBoundBox->append( Point3D( dDistance+dWall+dWidthEStage, -dWall-dWidthSStage, 0.00 ) );
	pCurrentRunBoundBox->append( Point3D( dDistance+dWall+dWidthEStage, dWidth+dWall-dWidthSStage+dWidthNStage, 0.00 ) );
	pCurrentRunBoundBox->append( Point3D( -dWidthWStage, dWidth+dWall-dWidthSStage+dWidthNStage, 0.00 ) );
	pCurrentRunBoundBox->transformBy( Transform );

	///////////////////////////////////////////////////////////
#ifdef	_DRAW_BOUND_BOX_
	DrawPolyLine( *pCurrentRunBoundBox, (bErase)? COLOUR_ERASE: COLOUR_BLUE );
#endif	//#ifdef	_DRAW_BOUND_BOX_

	//////////////////////////////////////////////////////
	//No point if there are no bays!
	if( iNumberOfBaysToDraw<=0 )
	{
		return bUseNew2ndPoint;
	}

	//////////////////////////////////////////////////////
	//Draw the smallest bay to the left of the centre if even
	if( DRAW_RIGHT_OF_CENTRE )
		iMiddleBayNumber = iNumberOfBaysToDraw-(iNumberOfBaysToDraw/2);
	else
		iMiddleBayNumber = (iNumberOfBaysToDraw/2)+1;

	//////////////////////////////////////////////////////
	//We better set a limit, or it could get nasty
	if( iNumberOfBaysToDraw>1000 || dDistance>(1000*(double)pABBay->GetBayLengthActual()))
	{
		//something fucked up!
		return bUseNew2ndPoint;
	}

	//////////////////////////////////////////////////////
	//This info is usefull for lapboards
	m_LBTplt.m_bLastBayInRun = false;
	m_LBTplt.m_bFirstBayInRun = true;

	//////////////////////////////////////////////////////
	//Create a new run
	if( m_bAddToDB )
	{
		iRunID = GetController()->CreateNewRun();
		pRun = GetController()->GetRun(iRunID);
		*pRun->GetRunTemplate() = 
				*pABBay->GetRunPointer()->GetRunTemplate();
		pRun->SetBracingLength( COMPONENT_LENGTH_2400, pABBay->GetRunPointer()->GetBracingLength( COMPONENT_LENGTH_2400 ) );
		pRun->SetBracingLength( COMPONENT_LENGTH_1800, pABBay->GetRunPointer()->GetBracingLength( COMPONENT_LENGTH_1800 ) );
		pRun->SetBracingLength( COMPONENT_LENGTH_1200, pABBay->GetRunPointer()->GetBracingLength( COMPONENT_LENGTH_1200 ) );
		pRun->SetBracingLength( COMPONENT_LENGTH_0700, pABBay->GetRunPointer()->GetBracingLength( COMPONENT_LENGTH_0700 ) );
		pRun->SetUseLongerBracing( pABBay->GetRunPointer()->GetUseLongerBracing() );
		pRun->SetRL( ptFirst, ptSecond, false );

		//We have now created a new run, so pCurrentRunBoundBox belongs
		//	to this Run, so lets make them equal and then point the
		//	pCurrentRunBoundBox to the one belonging to the run!
		*pRun->GetRunTemplate()->GetBoundingBoxPointer() = *pCurrentRunBoundBox;

		pCurrentRunBoundBox->RemoveAll();
		pCurrentRunBoundBox = pRun->GetRunTemplate()->GetBoundingBoxPointer();
	}

	//////////////////////////////////////////////////////
	//Calculate Pascal's Triangle for Bracing
	intArray	iaBaysWithBracing, iaBracingBays,
				iaBaysWithTies;

	CreatePascalsTriangle( GetRunTemplate()->GetBaysPerBrace(),
				iNumberOfBaysToDraw, iaBaysWithBracing );
	iaBracingBays = iaBaysWithBracing;

	//////////////////////////////////////////////////////
	//Calculate Pascal's Triangle for Ties
	CreatePascalsTriangle( GetRunTemplate()->GetBaysPerTie(),
				iNumberOfBaysToDraw, iaBaysWithTies );

	///////////////////////////////////////////////////////////////////
	//Pascal's triangle states that the last bay must have a tie,
	//	which is wrong, for an internal corner, so we need to Delete
	//	the tie from the last bay
	if( m_bOverrunMousePoint && iaBaysWithTies.GetSize()>0 &&
	   ( GetRunTemplate()->GetTieType()==TIE_TYPE_MASONARY	||
		 GetRunTemplate()->GetTieType()==TIE_TYPE_COLUMN	||
		 GetRunTemplate()->GetTieType()==TIE_TYPE_YOKE		) )
		iaBaysWithTies.RemoveAt(0);
	else if( !m_bOverrunMousePoint && iaBaysWithTies.GetSize()>0 &&
	   ( GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_12	||
		 GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_18	||
		 GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_24	) )
		iaBaysWithTies.RemoveAt(0);

	//////////////////////////////////////////////////////
	//Loop until we have created all the bays we said we would
	iBaysRemaining = iNumberOfBaysToDraw;

	double dProg, dIncr, dProgSpan;
	bool	bShowProgBar;
	dProg = (double)iStart;
	dProgSpan = ((double)iFin)-dProg;
	dIncr = dProgSpan/((double)iNumberOfBaysToDraw);
	//10% of the time is spent in the next loop
	dIncr*= 0.10;
	//There are 4 markers!
	dIncr/= 4.00;
	bShowProgBar = (iStart!=iFin);
	CString sProgressPrompt;
	if( bShowProgBar )
	{
		sProgressPrompt = _T("Creating Run");
		acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
		acedSetStatusBarProgressMeterPos( (int)dProg );
	}

	///////////////////////////////////////////////////////////////
	//Bracing
	//Store the existing settings
	bNB = pABBayTemplate->GetNBrace( );
	bSB = pABBayTemplate->GetSBrace( );

	///////////////////////////////////////////////////////////////
	//Store the eastern bay properties
	dWES	= pABBayTemplate->GetWidthOfEStage( );
	bNESC	= pABBayTemplate->GetNEStageCnr( );
	bSESC	= pABBayTemplate->GetSEStageCnr( );
	bEB		= pABBayTemplate->GetEBrace( );
	bEHR	= pABBayTemplate->GetEHandRail( );

	///////////////////////////////////////////////////////////////
	//Store the western bay properties
	dWWS	= pABBayTemplate->GetWidthOfWStage( );
	bSWSC	= pABBayTemplate->GetSWStageCnr( );
	bNWSC	= pABBayTemplate->GetNWStageCnr( );
	bWB		= pABBayTemplate->GetWBrace( );
	bWHR	= pABBayTemplate->GetWHandRail( );

	Point3D pt2ndPoint, pt2ndPointNew;
	while( iBaysRemaining>0 || dSmallBayDistance>0.00 )
	{
		///////////////////////////////////////////////////////////////
		//The second point is where we are working toward
		ptSEStd = ptSecond;

		///////////////////////////////////////////////////////////////
		//change the length of the middle bay
		if( dSmallBayDistance>0.00 && (iBaysRemaining==iMiddleBayNumber) )
		{
			dOriginalLength = (double)pABBayTemplate->GetBayLength();
			SetBayLength(GetController()->GetCompDetails()->GetCommonLength( GetSystem(), CT_LEDGER, dSmallBayDistance-m_dStandardWidthActual, MT_STEEL ));
		}

		GetInnerStandardPos( dDistance, pABBay->GetBayLengthActual(), ptSWStd, ptSEStd );

		///////////////////////////////////////////////////////////////
		//Ties
		bTie = false;
		iSize = iaBaysWithTies.GetSize();
		for( i=0; i<iSize; i++ )
		{
			iBay = iaBaysWithTies.GetAt( i );
			if( iBay==(iBaysRemaining-1) )
			{
				bTie = true;
				iaBaysWithTies.RemoveAt( i );
				break;
			}
		}

		//we don't want any southern bracing
		pABBayTemplate->SetSBrace( false );

		//if there is the possibility of northern bracing then determine
		//	which bays have bracing!
		if( pABBayTemplate->GetNBrace( ) )
		{
			iSize = iaBaysWithBracing.GetSize();
			for( i=0; i<iSize; i++ )
			{
				iBay = iaBaysWithBracing.GetAt( i );
				if( iBay==(iBaysRemaining-1) )
				{
					if( pABBayTemplate->GetWidthOfNStage()<=0 )
						pABBayTemplate->SetNBrace( true );
					else
						pABBayTemplate->SetNBrace( false );

					iaBaysWithBracing.RemoveAt( i );
					break;
				}
				else
				{
					pABBayTemplate->SetNBrace( false );
				}
			}
		}

		///////////////////////////////////////////////////////////////
		//Is this the last bay in run where we are storing the points to the database?
		if ( iBaysRemaining==1 &&  bStoreLastPoint )
			m_LBTplt.m_bLastBayInRun = true;

		if ( iBaysRemaining==1 && bArc )
			m_LBTplt.m_bLastBayInRun = true;

		///////////////////////////////////////////////////////////////
		//Find the RL's of the standards for this bay, by looking at the
		//	RL's for this run!
		FindZOfXYPointOnLine( ptSWStd, ptRLRun );
		FindZOfXYPointOnLine( ptSEStd, ptRLRun );

		//if there is a backward bay then don't draw the western stuff
		if( iBaysRemaining<iNumberOfBaysToDraw )
		{
			pABBayTemplate->SetWidthOfWStage( 0.00 );
			pABBayTemplate->SetSWStageCnr( false );
			pABBayTemplate->SetNWStageCnr( false );
			pABBayTemplate->SetWBrace( false );
			pABBayTemplate->SetWHandRail( false );
		}
		else
		{
			//otherwise brace and handrail the end bay
			if( pABBayTemplate->GetWidthOfEStage()>0.00 )
			{
				if( pPreviousBay!=NULL && pPreviousBay->IsMillsTypeBay() )
					pABBayTemplate->SetWBrace( false );
				else
					pABBayTemplate->SetWBrace( true );

				pABBayTemplate->SetWHandRail( true );
			}
		}

		//if there is a forward bay then don't draw the eastern stuff
		if( iBaysRemaining>1 )
		{
			pABBayTemplate->SetWidthOfEStage( 0.00 );
			pABBayTemplate->SetNEStageCnr( false );
			pABBayTemplate->SetSEStageCnr( false );
			pABBayTemplate->SetEBrace( false );
			pABBayTemplate->SetEHandRail( false );
		}
		else
		{
			//otherwise brace and handrail the end bay
			if( pABBayTemplate->GetWidthOfEStage()>0.00 )
			{
				pABBayTemplate->SetEBrace( true );
				pABBayTemplate->SetEHandRail( true );
			}
		}

		///////////////////////////////////////////////////////////////
		//Draw this bay
		if( m_bBuildForward )
			dRemainder = 0.00;

		if( bArc && m_LBTplt.m_bFirstBayInRun && m_LBTplt.m_bLastBayInRun )
		{
			//we are about to draw an arc which only has one bay per run
			//	so store this second point to otherwise it will point to itself
			//	note:  the calcs for the position of the lapboard is done
			//	within the draw bay routine.
			pt2ndPoint = m_LBTplt.m_pt2ndPoint; 
		}

		///////////////////////////////////////////////////////////////
		//Update Progress bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Bay %i - Creating"), iBaysRemaining );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		DrawBay( iRunID, ptSWStd, ptSEStd, bErase, dRemainder, bTie, bArc );

		///////////////////////////////////////////////////////////////
		//Update Progress bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Creating Lapboard") );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		if( bArc && m_LBTplt.m_bFirstBayInRun && m_LBTplt.m_bLastBayInRun )
		{
			//we need to store the second point we just calc'd, but this
			//	time we need to use the old second point
			pt2ndPointNew = m_LBTplt.m_pt2ndPoint;
			m_LBTplt.m_pt2ndPoint = pt2ndPoint; 
		}

		if(	!m_bBirdCageMode && m_LBTplt.m_bFirstBayInRun && m_LBTplt.IsLapBoardReqdThisRun() )
		{
			if( bSydneyCornerUsed || mtMillsCornerType==MILLS_TYPE_NONE || (!m_bBuildForward && dRemainder>GetDistanceFromWall() ) )
			{
				//draw the lapboard
				m_LBTplt.m_dFullWidth = pABBay->GetBayWidth();
				int iBoards = GetNumberOfPlanksForWidth( m_LBTplt.m_dFullWidth );
				iBoards+= pABBay->GetNumberOfPlanksOfStageFromWall( SOUTH );
				iBoards+= pABBay->GetNumberOfPlanksOfStageFromWall( NORTH );
				m_LBTplt.SetNumberOfBoards( iBoards );
				DrawLapboard( m_LBTplt.m_pt1stPoint, m_LBTplt.m_pt2ndPoint, bErase, false );
			}

			if( m_bAddToDB && (bSydneyCornerUsed||mtMillsCornerType==MILLS_TYPE_CONNECT_SSE) && pPreviousBay!=NULL )
			{
				if( mtMillsCornerType==MILLS_TYPE_CONNECT_SSE )
				{
					pPreviousBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD, SE );
					pPreviousBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD_OPENEND, SE );
				}

				Lift *pL;
				for( i=0; i<pPreviousBay->GetNumberOfLifts(); i++ )
				{
					pL = pPreviousBay->GetLift(i);
					pL->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE );
					pL->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, S );
				}
				pPreviousBay->DeleteTieFromSide( SSE );
				pPreviousBay->GetTemplate()->SetSEStageCnr( false );
				pPreviousBay->GetTemplate()->SetWidthOfSStage( 0.00 );
				pPreviousBay->Redraw();
			}
			else if( m_bAddToDB && mtMillsCornerType==MILLS_TYPE_CONNECT_NNE && pPreviousBay!=NULL )
			{
				pPreviousBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD, NE );
				pPreviousBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD_OPENEND, NE );

				Lift *pL;
				for( i=0; i<pPreviousBay->GetNumberOfLifts(); i++ )
				{
					pL = pPreviousBay->GetLift(i);
					pL->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNE );
					pL->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, N );
				}
				pPreviousBay->DeleteTieFromSide( NNE );
				pPreviousBay->GetTemplate()->SetNEStageCnr( false );
				pPreviousBay->GetTemplate()->SetWidthOfNStage( 0.00 );
				pPreviousBay->Redraw();
			}
		}

		if( bArc && m_LBTplt.m_bFirstBayInRun && m_LBTplt.m_bLastBayInRun )
		{
			//next time round we will need the 2nd point we just calculated
			m_LBTplt.m_pt2ndPoint = pt2ndPointNew;
		}

		///////////////////////////////////////////////////////////////
		//Update Progress bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Bay %i - Moving"), iBaysRemaining );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		m_LBTplt.m_bLastBayInRun = false;
		m_LBTplt.m_bFirstBayInRun = false;

		//move the bay within the run
		if( m_bAddToDB )
		{
			//Calculate and apply the Transformation required for the run
			pRun	= GetController()->GetRun(iRunID);
			iBayID	= pRun->GetNumberOfBays()-1;
			pBay	= pRun->GetBay(iBayID);

			//During the DrawBay routine, we did a set forward, through AddBay,
			//	However, this time we know it is the last bay, so we will need
			//	to adjust the standards & not ignor the null value
			if( iBaysRemaining==1 )
				pBay->SetForward( NULL, false, true );

			ptTemp1.set( ptFirst.x, ptFirst.y, 0.00 );
			ptTemp2.set( ptSWStd.x, ptSWStd.y, 0.00 );
			Vector.set( ptTemp1.distanceTo( ptTemp2 ), 0.00, 0.00 );
			Transform.setToTranslation( Vector );

			pBay->Move( Transform, true );
			pBay->MoveSchematic( Transform, true );

			//Copy the Tie setting from the Autobuild template to the runtemplate,
			//	This will later be used to determine if a tie should be created!
			pBayTemplate = pBay->GetTemplate();
			pBayTemplate->SetNNETie( pABBayTemplate->GetNNETie() );
			pBayTemplate->SetENETie( pABBayTemplate->GetENETie() );
			pBayTemplate->SetESETie( pABBayTemplate->GetESETie() );
			pBayTemplate->SetSSETie( pABBayTemplate->GetSSETie() );
			pBayTemplate->SetSSWTie( pABBayTemplate->GetSSWTie() );
			pBayTemplate->SetWSWTie( pABBayTemplate->GetWSWTie() );
			pBayTemplate->SetWNWTie( pABBayTemplate->GetWNWTie() );
			pBayTemplate->SetNNWTie( pABBayTemplate->GetNNWTie() );
		}

		/////////////////////////////////////////////////////////
		//restore the variables
		pABBayTemplate->SetWidthOfEStage( dWES );
		pABBayTemplate->SetWidthOfWStage( dWWS );
		pABBayTemplate->SetNEStageCnr( bNESC );
		pABBayTemplate->SetSEStageCnr( bSESC );
		pABBayTemplate->SetSWStageCnr( bSWSC );
		pABBayTemplate->SetNWStageCnr( bNWSC );
		pABBayTemplate->SetNBrace( bNB );
		pABBayTemplate->SetSBrace( bSB );
		pABBayTemplate->SetEBrace( bEB );
		pABBayTemplate->SetWBrace( bWB );
		pABBayTemplate->SetEHandRail( bEHR );
		pABBayTemplate->SetWHandRail( bWHR );

		ptSWStd = ptSEStd;

		dDistance-=pABBay->GetBayLengthActual();

		//restore the bay length
		if( dSmallBayDistance>0 && (iBaysRemaining==iMiddleBayNumber) )
		{
			SetBayLength(dOriginalLength);
			//ensure it doesn't come back in here again
			dSmallBayDistance=0.00;
		}
		iBaysRemaining--;
	}

	//5% of the time is spent in the following code
	dIncr = dProgSpan*0.05;
	//lets to this in 3 steps
	dIncr/= 3.00;

	/////////////////////////////////////////////////////////////////
	//move the run within the controller
	if( m_bAddToDB )
	{

		////////////////////////////////////////////////////////
		//Update Progress Bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Applying Bracing") );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		/////////////////////////////////////////////////////////
		//Bracing
		pRun->SetBaysWithBracing( true );

		////////////////////////////////////////////////////////
		//Update Progress Bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Applying Ties") );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		/////////////////////////////////////////////////////////
		//Ties
		///////////////////////////////////////////////////////////////////
		//Pascal's triangle states that the last bay must have a tie,
		//	which is wrong, for an internal corner, so we need to Delete
		//	the tie from the last bay
		bool bRemoveLastTie = false;
		if( m_bOverrunMousePoint &&
		   ( GetRunTemplate()->GetTieType()==TIE_TYPE_MASONARY	||
			 GetRunTemplate()->GetTieType()==TIE_TYPE_COLUMN	||
			 GetRunTemplate()->GetTieType()==TIE_TYPE_YOKE		) )
			bRemoveLastTie = true;
		else if( !m_bOverrunMousePoint &&
		   ( GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_12	||
			 GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_18	||
			 GetRunTemplate()->GetTieType()==TIE_TYPE_BUTTRESS_24	) )
			bRemoveLastTie = true;

		pRun->SetBaysWithTies( true, bRemoveLastTie );

		if( bLastRunBoxExists && !bLeft )
		{
			//Delete the tie from the preceeding bay
			if( pPreviousBay!=NULL )
			{
				pPreviousBay->DeleteTieFromSide(SSE);
				pPreviousBay->Redraw();
			}
		}

		/////////////////////////////////////////////////////////////////
		//Each Bay may have a different width of stageboard, however we
		//	need ensure that within the run, the standards all lineup
		pRun->AlignStandards();

		////////////////////////////////////////////////////////
		//Update Progress Bar
		if( bShowProgBar )
		{
			dProg+=dIncr;
			sProgressPrompt.Format( _T("Moving Runs") );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProg );
		}

		////////////////////////////////////////////////////////
		//Calculate and apply the Transformation required for the run
		pRun		= GetController()->GetRun(iRunID);
		Transform	= pRun->CalculateRunTransform( ptFirst, ptSecond );
		pRun->GetController()->SetHasBeenVisited(false);
		pRun->GetBay(0)->Visit( Transform, true );
		//don't store the origin as part of the run transform
		pRun->Move( GetController()->Get3DCrosshairTransform(), false );

		SetFirstRunInAutobuild( false );

	}

#ifdef	_DRAW_LAPBOARD_POINTS_
	//DrawCrosshair( m_LBTplt.m_pt1stPoint, 500.00, 500.00, bErase );
	DrawCrosshair( m_LBTplt.m_pt2ndPoint, 500.00, 500.00, bErase );
#endif	//#define	_DRAW_LAPBOARD_POINTS_

	//finally we can draw the run!
	if( m_bAddToDB )
	{
		pRun->SetAllowDrawBays( true );
		pRun->UpdateSchematicView( false, dProg, ((double)iFin-dProg) );

		m_iPreviousPreviousRunID = m_iPreviousRunID;
		m_iPreviousRunID = pRun->GetRunID();
	}
	return bUseNew2ndPoint;
}

//////////////////////////////////////////////////////////////////////////////////
//DrawArcBays
//
void AutoBuildTools::DrawArcBays( bool bErase )
{
	int			i, iBaysInRun, iTo, iFrom;
	Bay			*pABBay;
	bool		bOverRun;
	double		x1,y1,x2,y2,x3,y3,x12,y12,x22,y22,x32,y32,
					A, B, dNumerator, dDenominator,
					dRunLength, dHalfRunLength, dRunWidth, dDistInnerStdToArc,
					dRadius, dAngle, dRotationAngle, dStartAngle, dEndAngle,
					dArea;
	Point3D		ptCentre, ptA, ptB, ptC, ptSWStd, ptSEStd, ptMP1, ptMP2, ptMP3,
				ptRLPlane[MP_SIZE], ptTemp1, ptTemp2;
	RunFitEnum	eRunFit;

	////////////////////////////////////////////////////////////
	//We now must have 3 RL's which define a plane!  We will need to calculate
	//	the RL for the base of each standard on this plane.  We will need to
	//	do an intersection test between a line and a plane to determine this point!
	//	Firstly lets define the equation for the plane!
	//
	//	We know - m_dRL[MP_1ST] and m_ptMousePoint[MP_1ST]
	//			- m_dRL[MP_2ND] and m_ptMousePoint[MP_2ND]
	//			- m_dRL[MP_3RD] and m_ptMousePoint[MP_3RD]
	for( i=(int)MP_1ST; i<(int)MP_SIZE; i++ )
	{
		ptRLPlane[i] = m_ptMousePoint[i];
		ptRLPlane[i].z = m_dRL[i];
	}

	///////////////////////////////////////////////////////////////////////////////
	//Basically we, need to find the centre of the circle, which runs through
	//	m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND] & m_ptMousePoint[MP_3RD].
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	x1 = m_ptMousePoint[MP_1ST].x;
	y1 = m_ptMousePoint[MP_1ST].y;
	if( m_ptMousePoint[MP_1ST].y>=m_ptMousePoint[MP_3RD].y-ROUND_ERROR &&
		m_ptMousePoint[MP_1ST].y<=m_ptMousePoint[MP_3RD].y+ROUND_ERROR)
	{
		if( m_ptMousePoint[MP_1ST].y>=m_ptMousePoint[MP_2ND].y-ROUND_ERROR &&
			m_ptMousePoint[MP_1ST].y<=m_ptMousePoint[MP_2ND].y+ROUND_ERROR )
		{
			//we have a straight line!
			return;
		}
		x2 = m_ptMousePoint[MP_2ND].x;
		y2 = m_ptMousePoint[MP_2ND].y;
		x3 = m_ptMousePoint[MP_3RD].x;
		y3 = m_ptMousePoint[MP_3RD].y;
	}
	else
	{
		x2 = m_ptMousePoint[MP_3RD].x;
		y2 = m_ptMousePoint[MP_3RD].y;
		x3 = m_ptMousePoint[MP_2ND].x;
		y3 = m_ptMousePoint[MP_2ND].y;
	}
	x12 = x1*x1;
	y12 = y1*y1;
	x22 = x2*x2;
	y22 = y2*y2;
	x32 = x3*x3;
	y32 = y3*y3;

	///////////////////////////////////////////////////////////////////////////////
	//we need to solve the equation of a circle knowing 3 point on the circle
	// the equation of a circle is:
	//		x^2 + y^2 + Ax + By + C = 0
	// thus we need to solve for A and B since we know:
	//	1)  x12 + y12 + Ax1 + By1 + C = 0,
	//	2)  x22 + y22 + Ax2 + By2 + C = 0, and
	//	3)  x32 + y32 + Ax3 + By3 + C = 0
	dNumerator = (x32*y1)-(x32*y2)-(x32*y3)+(x32*y3)
				+(x22*y3)-(x22*y1)
				+(x12*y2)-(x12*y3)
				+(y32*y1)-(y32*y2)+(y32*y3)-(y32*y3)
				+(y22*y3)-(y22*y1)
				+(y12*y2)-(y12*y3);

	dDenominator = (x1*y3)-(x1*y2)
				  +(x2*y1)-(x2*y3)
				  +(x3*y2)-(x3*y1);

	///////////////////////////////////////////////////////////////////////////////
	//set the default centre to be a straight line
	ptCentre.x = (x1+x2)/2.00;
	ptCentre.y = (y1+y2)/2.00;
	ptCentre.z = (m_ptMousePoint[MP_1ST].z+m_ptMousePoint[MP_2ND].z)/2.00;

	///////////////////////////////////////////////////////////////////////////////
	//do the first division
	//prevent divide by zero
	if( dDenominator >0.00001 || dDenominator <-0.00001 )
	{
		///////////////////////////////////////////////////////////////////////////////
		//this is the 'A', mentioned above, in the equation of a circle
		A = dNumerator / dDenominator;

		///////////////////////////////////////////////////////////////////////////////
		//We need to solve for 'B' too
		dNumerator = x22+y22+(A*x2)-x12-y12-(A*x1);
		dDenominator = y1-y2;

		///////////////////////////////////////////////////////////////////////////////
		//do the second division
		//prevent divide by zero
		if( dDenominator >0.00001 || dDenominator <-0.00001 )
		{
			//this is the 'B', mentioned above, in the equation of a circle
			B = dNumerator / dDenominator;

			//The centre of the circle will be...
			ptCentre.x = A / -2.00;
			ptCentre.y = B / -2.00;
		}
	#ifdef _DEBUG
		else	//if( dDenominator >0.00001 || dDenominator <-0.00001 )
		{
			acutPrintf( _T("Denominator error 1\n"));
		}
	}
	else	//if( dDenominator >0.00001 || dDenominator <-0.00001 )
	{
		acutPrintf( _T("Denominator error 2\n"));
	#endif	//#ifdef _DEBUG
	}

	///////////////////////////////////////////////////////////////////////////////
	//We now need to work out how close we can position bays together
	// we will calculate this as an angle around the origin
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	//first calculate the Radius of the circle using pythagois's theorm
	ptTemp1.set( m_ptMousePoint[MP_1ST].x, m_ptMousePoint[MP_1ST].y, 0.00 );
	ptTemp2.set( ptCentre.x, ptCentre.y, 0.00 );
	dRadius = ptTemp1.distanceTo( ptTemp2 );

	///////////////////////////////////////////////////////////////////////////////
	//Draw the 3 crosshairs
	DrawCrosshair( ptCentre,					m_dCrossHairWidth, m_dCrossHairHeight, bErase );
	DrawCrosshair( m_ptMousePoint[MP_1ST],		m_dCrossHairWidth, m_dCrossHairHeight, bErase );
	DrawCrosshair( m_ptMousePoint[MP_2ND],		m_dCrossHairWidth, m_dCrossHairHeight, bErase );

	///////////////////////////////////////////////////////////////////////////////
	//if the radius is very small don't bother
	if( dRadius<-0.000001 || dRadius>0.000001 )
	{
		bool bOutside;

		///////////////////////////////////////////////////////////////////////////////
		//move the centre of the circle to the origin
		Matrix3D Transform, TransCentre;

		Vector3D Vector;
		Vector.set( -1.00*ptCentre.x, -1.00*ptCentre.y, -1.00*ptCentre.z );
		Transform.setToTranslation(Vector);

		ptA = Transform * m_ptMousePoint[MP_1ST];
		ptB = Transform * m_ptMousePoint[MP_3RD];
		ptC = Transform * m_ptMousePoint[MP_2ND];

		#ifdef _DEBUG
		DrawCrosshair( ptA,	m_dCrossHairWidth, m_dCrossHairHeight, bErase );
		DrawCrosshair( ptB,	m_dCrossHairWidth, m_dCrossHairHeight, bErase );
		DrawCrosshair( ptC,	m_dCrossHairWidth, m_dCrossHairHeight, bErase );
		#endif //#ifdef _DEBUG

		///////////////////////////////////////////////////////////////////////////////
		//determine if the centre is right or left(ie internal or external bays)
		dArea = ( (ptA.y+ptB.y)*(ptB.x-ptA.x)/2.00 )
			+	( (ptB.y+ptC.y)*(ptC.x-ptB.x)/2.00 )
			+	( (ptC.y+ptA.y)*(ptA.x-ptC.x)/2.00 );

		bOutside = ( dArea>0 )? false: true;

		if( (dArea>=-0.001) && (dArea<=0.001) )
		{
#ifdef _DEBUG
			acutPrintf(_T("Area of arc's circle is very small\n"));
//			assert( false );
#endif	//#ifdef _DEBUG
			return;
		}

		///////////////////////////////////////////////////////////////////////////////
		//calculate the angle covered by 1/2 a bay, plus a lapboard of specifed distance
		//	between the outer standards
		pABBay = GetABBay();
		dRunLength = pABBay->GetWidthOfStageFromWall( WEST )
					+ pABBay->GetWidthOfStageFromWall( EAST );
		dRunWidth = pABBay->GetBayWidthActual()
					+ pABBay->GetWidthOfStageFromWall( SOUTH );
		iBaysInRun = 0;
		if( bOutside )
		{
			assert( GetController()!=NULL );
			if( GetController()->GetArcLapboardSpan()>0.00 )
			{
				doubleArray dAvailableLengths;
				dAvailableLengths.RemoveAll();
				if( GetRunTemplate()->GetFit()==RF_TIGHT )
				{
					if( pABBay->GetBayLengthActual()>=m_dBayLengthActual2400-ROUND_ERROR &&
						pABBay->GetBayLengthActual()<=m_dBayLengthActual2400+ROUND_ERROR )
					{
						dAvailableLengths.Add( m_dBayLengthActual0700 );
						dAvailableLengths.Add( m_dBayLengthActual1200 );
						dAvailableLengths.Add( m_dBayLengthActual1800 );
						dAvailableLengths.Add( m_dBayLengthActual2400 );
					}
					else if(pABBay->GetBayLengthActual()>=m_dBayLengthActual1800-ROUND_ERROR &&
							pABBay->GetBayLengthActual()<=m_dBayLengthActual1800+ROUND_ERROR )
					{
						dAvailableLengths.Add( m_dBayLengthActual0700 );
						dAvailableLengths.Add( m_dBayLengthActual1200 );
						dAvailableLengths.Add( m_dBayLengthActual1800 );
					}
					else if(pABBay->GetBayLengthActual()>=m_dBayLengthActual1200-ROUND_ERROR &&
							pABBay->GetBayLengthActual()<=m_dBayLengthActual1200+ROUND_ERROR )
					{
						dAvailableLengths.Add( m_dBayLengthActual0700 );
						dAvailableLengths.Add( m_dBayLengthActual1200 );
					}
					else if(pABBay->GetBayLengthActual()>=m_dBayLengthActual0700-ROUND_ERROR &&
							pABBay->GetBayLengthActual()<=m_dBayLengthActual0700+ROUND_ERROR )
					{
						dAvailableLengths.Add( m_dBayLengthActual0700 );
					}
				}
				else
				{
					dAvailableLengths.Add( m_dBayLengthActual2400 );
				}
				assert( dAvailableLengths.GetSize()>0 );

				i=0;
				do
				{
					//Let the outer standards be dLapboard Distance apart
					dRunLength			+=dAvailableLengths[i];
					if( i>0 )
						dRunLength		-=dAvailableLengths[i-1];
					dAngle				= GetHalfAngleOuterStandards( GetController()->GetArcLapboardSpan(), dRunWidth, dRunLength, dRadius );
					dDistInnerStdToArc	= GetDistanceToArcOfInnerStandard( dRunLength, dRadius );
					iBaysInRun++;
					i++;
					if( i>=dAvailableLengths.GetSize() )
						i=0;
				}
				while( (dDistInnerStdToArc<m_dDistanceInnerStandardToArc) && (iBaysInRun<100) );

				//we have gone one too far now!
				if( i<=0 ) 
					i=dAvailableLengths.GetSize()-1;
				i--;
				dRunLength			-=dAvailableLengths[i];
				i--;
				if( i>0 )
					dRunLength		+=dAvailableLengths[i];

				dAngle				= GetHalfAngleOuterStandards( GetController()->GetArcLapboardSpan(), dRunWidth, dRunLength, dRadius );
			}
			else
			{
				assert( false );
			}
		}
		else
		{
			assert( GetController()!=NULL );
			if( GetController()->GetArcLapboardSpan()>0.00 )
			{
				do
				{
					dRunLength			+=pABBay->GetBayLengthActual();
					dAngle				= GetHalfAngleInnerStandards( dRadius, dRunLength, GetController()->GetArcLapboardSpan() );
					dDistInnerStdToArc	= GetDistanceToArcOfMidBay( dRadius, dRunLength );
					iBaysInRun++;
				}
				while( (dDistInnerStdToArc<m_dDistanceInnerStandardToArc) && (iBaysInRun<100) );

				//we have gone one too far now!
				dRunLength			-=pABBay->GetBayLengthActual();
				dAngle				= GetHalfAngleInnerStandards( dRadius, dRunLength, GetController()->GetArcLapboardSpan() );
			}
			else
			{
				assert( false );
			}
		}
//		dAngle /= 2.00;

		///////////////////////////////////////////////////////////////////////
		//we need to ensure we are not talking about a straight line
		if( dAngle>0.001 || dAngle<-0.001 )
		{
			///////////////////////////////////////////////////////////////////////
			//calculate the starting and finishing angle
			//	we also need to know if this is the same autobuild
			//	command or a new one, if it is a new one, the last
			//	run is completely irrelivant
			if( GetController()->GetNumberOfRuns()>0 )
			{
				int				iPreviousRunID;
				Point3D			pt;
				Point3DArray	pts;
				Run				*pRun;

				iPreviousRunID	= m_iPreviousRunID;
				pRun = GetController()->GetRun(iPreviousRunID);

				if( pRun!=NULL && pRun->GetAutobuildNumber()==giAutobuildNumber )
				{
					pts = *(pRun->GetRunTemplate()->GetBoundingBoxPointer());
					assert( pts.length()==5 );

					#ifdef	_DRAW_ARC_BOUND_BOX_
					DrawPolyLine( pts, (bErase)? COLOUR_ERASE: COLOUR_WHITE );
					#endif	//#ifdef	_DRAW_ARC_BOUND_BOX_

					if( bOutside )
					{
						pt = Transform * pts[2];
						#ifdef	_DRAW_ARC_BOUND_BOX_
						Point3D	ptCentre;
						ptCentre.set( 0.00, 0.00, 0.00 ); 
						DrawLine( ptCentre, pt, (bErase)? COLOUR_ERASE: 7 );
						#endif	//#ifdef	_DRAW_ARC_BOUND_BOX_
						dStartAngle = CalculateAngle( pt );
						pt = Transform * pts[3];
						#ifdef	_DRAW_ARC_BOUND_BOX_
						DrawLine( ptCentre, Transform * pts[3], (bErase)? COLOUR_ERASE: 7 );
						#endif	//#ifdef	_DRAW_ARC_BOUND_BOX_
						dStartAngle = min( dStartAngle, CalculateAngle( pt ) );
					}
					else
					{
						pt = Transform * pts[2];
						dStartAngle = CalculateAngle( pt );
						pt = Transform * pts[1];
						dStartAngle = max( dStartAngle, CalculateAngle( pt ) );
					}
				}
				else
				{
					dStartAngle = CalculateAngle( ptA );
				}
			}
			else
			{
				dStartAngle = CalculateAngle( ptA );
			}
			dEndAngle = CalculateAngle( ptB );
			dStartAngle = GetSensibleAngle( dStartAngle );
			dEndAngle = GetSensibleAngle( dEndAngle );

			Vector = FromOriginToPointAsVector( ptCentre );
			TransCentre.setToTranslation(Vector);

			if( bOutside )
			{
				while( dEndAngle>dStartAngle )
					dStartAngle += d360Deg;
			}
			else
			{
				while( dEndAngle<dStartAngle )
					dStartAngle -= d360Deg;
			}

			///////////////////////////////////////////////////////////////////////
			//position the two inner standards for the bay
			dHalfRunLength = dRunLength/2.00;
			dHalfRunLength+= 5.00;
			if( bOutside )
			{
				ptSWStd.set( 0.00, dHalfRunLength-pABBay->GetWidthOfStageFromWall( WEST ), 0.00 );
				ptSEStd.set( 0.00, pABBay->GetWidthOfStageFromWall( EAST )-dHalfRunLength, 0.00 );
			}
			else
			{
				ptSWStd.set( 0.00, pABBay->GetWidthOfStageFromWall( WEST )-dHalfRunLength, 0.00 );
				ptSEStd.set( 0.00, dHalfRunLength-pABBay->GetWidthOfStageFromWall( EAST ), 0.00 );
			}

			///////////////////////////////////////////////////////////////////////
			//move to the intersection of the Arc and the +ve Xaxis
			Vector.set( dRadius, 0.00, 0.00 );
			//Transform.setToTranslation(Vector);
			Transform.setToTranslation(Vector);
			ptSWStd = Transform * ptSWStd;
			ptSEStd = Transform * ptSEStd;

			///////////////////////////////////////////////////////////////////////
			//store the points
			ptA = ptSWStd;
			ptB = ptSEStd;
			
			dRotationAngle = ( bOutside )? dStartAngle-dAngle: dStartAngle+dAngle;

			////////////////////////////////////////
			//how many runs are we going to create?
			int iRunsToDraw, iNumberOfRuns;
			double dIncr;
			iNumberOfRuns = abs((int)((dEndAngle-dStartAngle)/(2.00 * dAngle)));
			if( m_bAddToDB && iNumberOfRuns>0 )
			{
				dIncr = 100.00/(double)iNumberOfRuns;
			}
			iRunsToDraw = iNumberOfRuns;

			bOverRun = GetMouseOverrun();
			SetMouseOverrun( false );
			eRunFit = GetRunTemplate()->GetFit();
			iFrom	= 0;
			iTo		= iFrom;

			///////////////////////////////////////////////////////////////////////
			//while dAngle<dFullArcAngle
			while( (  bOutside && (dRotationAngle>dEndAngle-dAngle) )
				|| ( !bOutside && (dRotationAngle<(dEndAngle-dAngle)) ) )
			{
				ptSWStd = ptA;
				ptSEStd = ptB;

				if( iRunsToDraw<=0 )
				{
					if( bOutside )
					{
						ptSEStd.y = ptSWStd.y-((ptA.y-ptB.y)*((dRotationAngle-(dEndAngle-dAngle))/(dAngle*2.00)));
					}
				}

				//////////////////////////////////////////////////////////////////
				//Rotate around the Zaxis to the correct position
				Transform.setToRotation( dRotationAngle, VECTOR_ROTATE_AROUND_ZAXIS );
				//translate back to original centre
				Transform = TransCentre * Transform;
				ptSWStd.transformBy( Transform );
				ptSEStd.transformBy( Transform );

				#ifdef	_DRAW_ARC_BOUND_BOX_
				DrawCrosshairDebug( ptSWStd, bErase, 3 );
				DrawLine( ptSWStd, ptSEStd, (bErase)? COLOUR_ERASE: 4 );
				DrawCrosshairDebug( ptSEStd, bErase, 5 );
				#endif	//#ifdef	_DRAW_ARC_BOUND_BOX_

				//////////////////////////////////////////////////////////
				//store the points
				ptMP1 = m_ptMousePoint[MP_1ST];
				ptMP2 = m_ptMousePoint[MP_2ND];
				ptMP3 = m_ptMousePoint[MP_3RD];
			
				//////////////////////////////////////////////////////////
				//set the first point to the start of the run and the second point
				//	to the end of the run!
				m_ptMousePoint[MP_1ST] = ptSWStd;
				m_ptMousePoint[MP_2ND] = ptSEStd;

				//////////////////////////////////////////////////////////////////
				//what are the z coordinates of the points?
				if( !FindZOfXYPointOnPlane( m_ptMousePoint[MP_1ST], ptRLPlane) )
					m_ptMousePoint[MP_1ST].z = 0.00;

				if( !FindZOfXYPointOnPlane( m_ptMousePoint[MP_2ND], ptRLPlane) )
					m_ptMousePoint[MP_2ND].z = 0.00;

				if( m_bAddToDB )
				{
					iFrom	= (int)(dIncr*(double)(iNumberOfRuns-(iRunsToDraw)));
					iTo		= (int)(dIncr*(double)(iNumberOfRuns-(iRunsToDraw-1)));
				}

				DrawRun( bErase, m_bAddToDB, true, iFrom, iTo );

				//////////////////////////////////////////////////////////
				//Restore the points
				m_ptMousePoint[MP_1ST] = ptMP1;
				m_ptMousePoint[MP_2ND] = ptMP2;
				m_ptMousePoint[MP_3RD] = ptMP3;

				if( bOutside )
				{
					dRotationAngle -= 2.00 * dAngle;
				}
				else
				{
					dRotationAngle += 2.00 * dAngle;
				}
				iRunsToDraw--;
			}
			SetMouseOverrun( bOverRun );
			SetRunFit( eRunFit );
//			acedRestoreStatusBar();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//DrawCrosshair
//
void AutoBuildTools::DrawCrosshair( Point3D ptCross, double dWidth, double dHeight, bool bErase )
{
	Point3D ptA, ptB, ptC, ptD;

	//////////////////////////////////////////////////////
	//create the points
	ptA = ptCross;
	ptB = ptCross;
	ptC = ptCross;
	ptD = ptCross;
	ptA.x += (dWidth/2.00);
	ptB.x -= (dWidth/2.00);
	ptC.y += (dHeight/2.00);
	ptD.y -= (dHeight/2.00);

	//////////////////////////////////////////////////////
	//colour
	int iCrosshairColour = (bErase)? COLOUR_ERASE: COLOUR_CROSSHAIR;

	//////////////////////////////////////////////////////
	//draw the crosshair
	AcDbObjectIdArray objIds;
	DrawLine( ptA, ptB, iCrosshairColour );
	DrawLine( ptC, ptD, iCrosshairColour );
}

//////////////////////////////////////////////////////////////////////////////////
//DrawDebugCrosshair
//
void AutoBuildTools::DrawCrosshairDebug( Point3D ptCross, bool bErase, int iColour )
{
	Point3D ptA, ptB, ptC, ptD;
	double dWidth, dHeight;

	dWidth = 500.00;
	dHeight = 500.00;
	//////////////////////////////////////////////////////
	//create the points
	ptA = ptCross;
	ptB = ptCross;
	ptC = ptCross;
	ptD = ptCross;
	ptA.x += (dWidth/2.00);
	ptB.x -= (dWidth/2.00);
	ptC.y += (dHeight/2.00);
	ptD.y -= (dHeight/2.00);

	//////////////////////////////////////////////////////
	//colour
	int iCrosshairColour = (bErase)? COLOUR_ERASE: iColour;

	//////////////////////////////////////////////////////
	//draw the crosshair
	AcDbObjectIdArray objIds;
	DrawLine( ptA, ptB, iCrosshairColour, 1 );
	DrawLine( ptC, ptD, iCrosshairColour, 1 );
}

//////////////////////////////////////////////////////////////////////////////////
//DrawBay
//
void AutoBuildTools::DrawBay( int iRunID, Point3D ptStart, Point3D ptOuterStd2Direction,
								bool bErase, double dRemainder, bool bTie/*=false*/, bool bArc/*=false*/ )
{
	////////////////////////////////////////////////////////////////////////////
	//rotate GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD] around a line parrallel to the
	//	Z_AXIS through GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD]!
	int					i, iColour;
	bool				bStage = true;
	double				dCurrentRunAngle, dAngleStartToOuter,
						dRLBay[2];
	Point3D				ptPoints[BP_SIZE], ptLapPoint;
	Matrix3D			Transform, Rotation, Translation;
	Vector3D			Vector, Vect, vStartToOuter;
	AcDbObjectIdArray	objIds;
	BayTemplate			*pABBayTemplate;

	pABBayTemplate = GetABBay()->GetTemplate();
	/////////////////////////////////////////////////////////////////
	//Store the RL's of the posts, round up to the nearest GetStarSeparation()
	dRLBay[0] = (double)((int)((ptStart.z/GetStarSeparation())+0.5)*GetStarSeparation());
	ptStart.z = 0.00;
	dRLBay[1] = (double)((int)((ptOuterStd2Direction.z/GetStarSeparation())+0.5)*GetStarSeparation());

/*
	if( m_bAddToDB )
		acutPrintf("\nRL for bay: %0.2f %0.2f \n", dRLBay[0], dRLBay[1]);
*/
	ptOuterStd2Direction.z	= 0.00;

	////////////////////////////////////////////////////////////////////////////
	//copy the points
	pABBayTemplate->CreatePoints();
	for( i=0; i<BP_SIZE; i++ )
	{
		ptPoints[i] = pABBayTemplate->m_aPoints[i]; 
	}

	////////////////////////////////////////////////////////////////////////////
	//rotate bay to be parrallel to drag line
	//the anglepoint uses ptStart as an origin
	vStartToOuter = ptOuterStd2Direction-ptStart;
	dAngleStartToOuter = GetSensibleAngle( CalculateAngle( vStartToOuter ) );
	//create the rotation matix
	Rotation.setToRotation( dAngleStartToOuter, VECTOR_ROTATE_AROUND_ZAXIS );

	////////////////////////////////////////////////////////////////////////////
	//translate bay to start at ptStart
	//Transform.Translate( ptStart.x, ptStart.y, ptStart.z );
	Vector = FromOriginToPointAsVector( ptStart );
	Translation.setToTranslation(Vector);
	Transform = Translation * Rotation;

	double dModulus;

	if( bArc )
	{
		if( m_LBTplt.m_bFirstBayInRun )
		{
			m_eLapboardWillCrossBaySide=WEST;	//This will be overwritten anyway
			m_LBTplt.SetLapBoardReqdThisRun(true);
			m_LBTplt.m_pt1stPoint.x = ( ptPoints[BP_SW_STD].x + ptPoints[BP_NW_STD].x ) / 2.00;
			m_LBTplt.m_pt1stPoint.y = ( ptPoints[BP_SW_STD].y + ptPoints[BP_NW_STD].y ) / 2.00;
			m_LBTplt.m_pt1stPoint.z = ( ptPoints[BP_SW_STD].z + ptPoints[BP_NW_STD].z ) / 2.00;
			m_LBTplt.m_pt1stPoint.x+= m_LBTplt.m_iOverLapReqd * 1.25;
			m_LBTplt.m_pt1stPoint.transformBy( Transform );
		}

		if( m_LBTplt.m_bLastBayInRun )
		{
			//calculate the second point but don't store it yet
			m_eLapboardWillCrossPreviousBaySide=EAST;
			m_LBTplt.m_pt2ndPoint.x = ( ptPoints[BP_SE_STD].x + ptPoints[BP_NE_STD].x ) / 2.00;
			m_LBTplt.m_pt2ndPoint.y = ( ptPoints[BP_SE_STD].y + ptPoints[BP_NE_STD].y ) / 2.00;
			m_LBTplt.m_pt2ndPoint.z = ( ptPoints[BP_SE_STD].z + ptPoints[BP_NE_STD].z ) / 2.00;
			m_LBTplt.m_pt2ndPoint.x-= m_LBTplt.m_iOverLapReqd * 1.25;
			m_LBTplt.m_pt2ndPoint.transformBy( Transform );
		}
	}
	else if( m_bBirdCageMode )
	{
		m_LBTplt.SetLapBoardReqdThisRun(false);
	}
	else
	{
		m_eLapboardWillCrossPreviousBaySide=SIDE_INVALID;
		////////////////////////////////////////////////////////////////////////////
		//if this is the first bay in the run then we need a point for the lapboard
		if( m_LBTplt.m_bFirstBayInRun && (m_dLastRunAngle<d360Deg) )
		{
			/////////////////////////////////////////////////////////////////////////////////////
			//We initially set m_dLastRunAngle to 360deg, so it should have
			//	been set already by the last bay in run
			dCurrentRunAngle = GetSensibleAngle( dAngleStartToOuter - m_dLastRunAngle );

			#ifdef _DRAW_LAPBOARD_POINTS_
      acutPrintf(_T("\nAngle: %0.2fdeg"), dCurrentRunAngle );
			#endif	//#ifdef _DRAW_LAPBOARD_POINTS_

			/////////////////////////////////////////////////////////////////////////////////////
			//create a point 1/2 way between the NW and SW standards and m_iOverLapReqd in from the side 
			double dX, dY, dZ;
			dX = ( ptPoints[BP_SW_STD].x + ptPoints[BP_NW_STD].x ) / 2.00;
			dY = ( ptPoints[BP_SW_STD].y + ptPoints[BP_NW_STD].y ) / 2.00;
			dZ = ( ptPoints[BP_SW_STD].z + ptPoints[BP_NW_STD].z ) / 2.00;
			m_LBTplt.m_pt1stPoint.set( dX+m_LBTplt.m_iOverLapReqd, dY, dZ );
			m_LBTplt.m_pt1stPoint.transformBy( Transform );

			/////////////////////////////////////////////////////////////////////////////////////
			//This time we need to create a point in the neighboring bay, make the point dependant
			//	upon the angle that the run are to each other!  however we MUST span the distance
			//	between the runs including the m_iOverLapReqd twice!
			double dOffset;
			dOffset = fabs( sin( dCurrentRunAngle*2.00 ) );
			dOffset*= m_dBayLengthActual2400-((2.00*m_LBTplt.m_iOverLapReqd) + GetDistanceFromWall() + dRemainder);
			dOffset+= m_LBTplt.m_iOverLapReqd + GetDistanceFromWall() + dRemainder;
			m_LBTplt.m_pt2ndPoint.set( dX-dOffset, dY, dZ );
			m_LBTplt.m_pt2ndPoint.transformBy( Transform );

			/////////////////////////////////////////////////////////////////////////////////////
			//The above function is causing the lapboards to be a bit short,
			//	so lets try to see if this has created a point that is within
			//	the previous bay (actaully could be any bay at present, but we
			//	can fix that if req'd!)
			Point3D pt = m_LBTplt.m_pt2ndPoint;
			HitLocationEnum eHit;
			GetBayByInternalPoint( m_LBTplt.m_pt2ndPoint, eHit, -1.00*m_LBTplt.m_iOverLapReqd );
			if( eHit!=HL_BAY )
			{
				//lets try a 1200mm bay does that fit?
				m_LBTplt.m_pt2ndPoint.set( dX-m_dBayLengthActual1200, dY, dZ );
				m_LBTplt.m_pt2ndPoint.transformBy( Transform );
				GetBayByInternalPoint( m_LBTplt.m_pt2ndPoint, eHit, -1.00*m_LBTplt.m_iOverLapReqd );
				if( eHit!=HL_BAY )
				{
					//No, lets try an 1800mm bay does that fit?
					m_LBTplt.m_pt2ndPoint.set( dX-m_dBayLengthActual1800, dY, dZ );
					m_LBTplt.m_pt2ndPoint.transformBy( Transform );
					GetBayByInternalPoint( m_LBTplt.m_pt2ndPoint, eHit, -1.00*m_LBTplt.m_iOverLapReqd );
					if( eHit!=HL_BAY )
					{
						//No, lets try an 2400mm bay does that fit?
						m_LBTplt.m_pt2ndPoint.set( dX-m_dBayLengthActual2400, dY, dZ );
						m_LBTplt.m_pt2ndPoint.transformBy( Transform );
						GetBayByInternalPoint( m_LBTplt.m_pt2ndPoint, eHit, -1.00*m_LBTplt.m_iOverLapReqd );
						if( eHit!=HL_BAY )
						{
							//nope, none of them fit, so lets just go back to the original point!
							m_LBTplt.m_pt2ndPoint = pt;
						}
					}
				}
			}
/*
			#ifdef _DEBUG
			This code has been removed since I added the incrementing lapboard
			lenght code above, since it continually goes off if we use one of
			the new lapbay sizes!
			double dSeparation;
			dSeparation = m_LBTplt.m_pt1stPoint.distanceTo( m_LBTplt.m_pt2ndPoint );
			assert( dOffset+m_LBTplt.m_iOverLapReqd<=m_dBayLengthActual2400+ROUND_ERROR );
			assert( dSeparation<dOffset+m_LBTplt.m_iOverLapReqd+ROUND_ERROR &&
					dSeparation>dOffset+m_LBTplt.m_iOverLapReqd-ROUND_ERROR);
			assert( dSeparation<=m_dBayLengthActual2400+ROUND_ERROR );
			#endif	//#ifdef _DEBUG
*/
			////////////////////////////////////////////////////////////////////
			//do we need a lapboard here?
			m_LBTplt.SetLapBoardReqdThisRun(false);
			if( GetController()!=NULL )
			{
				////////////////////////////////////////////////////////////////////
				//do we have a previous run
				int iRunID = m_iPreviousRunID;
				Run *pPreviousRun = GetController()->GetRun( iRunID );

				if( pPreviousRun!=NULL && pPreviousRun->GetAutobuildNumber()==giAutobuildNumber )
				{
					//we have a previous run, find the last bay
					Bay *pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
					if( pPreviousBay!=NULL )
					{
						if( ( dCurrentRunAngle>d135Deg && dCurrentRunAngle<d225Deg ) ||
							( dCurrentRunAngle<d135Deg && dCurrentRunAngle>d225Deg ) )
						{
							#ifdef _DRAW_LAPBOARD_POINTS_
							acutPrintf(_T("\n180deg"));
							#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
						}
						else
						{
							dModulus = dCurrentRunAngle/d90Deg;
							dModulus-= (int)dModulus;
							dModulus = fabs( dModulus );
							dModulus*= d90Deg;
							if( dModulus<d15_DEGREE || dModulus>d90Deg-d15_DEGREE )
							{
								m_LBTplt.SetLapBoardReqdThisRun(true);
							}
							else
							{
								#ifdef _DRAW_LAPBOARD_POINTS_
								acutPrintf(_T("\nOutside"));
								#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
							}
						}

						#ifdef _DRAW_LAPBOARD_POINTS_
						acutPrintf( _T(" - Offset=%0.1fmm, Angle=%0.1fdeg\n"), dOffset, dCurrentRunAngle*CONVERT_TO_DEG );
						#endif	//#ifdef _DRAW_LAPBOARD_POINTS_

						if( m_bAddToDB && m_LBTplt.IsLapBoardReqdThisRun() )
						{
							#ifdef _DRAW_LAPBOARD_POINTS_
              acutPrintf(_T("\nAngle Used: %0.2fdeg"), dCurrentRunAngle );
							#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
							RemoveLapboardObsticals( pPreviousBay, dCurrentRunAngle );
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////
		//if this is the last bay in the run then we need a point for the lapboard
		if( m_LBTplt.m_bLastBayInRun )
		{
			m_PrevRunLastBayTransform = Transform;
			m_dLastRunAngle = dAngleStartToOuter;
		}
	}
	
	
	////////////////////////////////////////////////////////////////////////////
	//if we are NOT writing to the database, then rotate the points
	//	apply to the points
	if( !m_bAddToDB )
	{
		for( i=0; i<BP_SIZE; i++ )
			ptPoints[i] = Transform * ptPoints[i]; 
	}

	////////////////////////////////////////////////////////////////////////////
	//Colour
	if( bErase )
		iColour = COLOUR_ERASE;
	else
		iColour = GetBayColour( pABBayTemplate->GetBayLength() );


	////////////////////////////////////////////////////////////////////////////
	//copy the bay to the new run
	if( m_bAddToDB )
	{
		Bay	*pBay;

		pBay = GetController()->AddFromAutoBuildToRun( iRunID, dRLBay );

		if( m_LBTplt.m_bLastBayInRun )
		{
			m_LBTplt.SetLastBayInRun( pBay );
			if( GetMouseOverrun() && !m_bBuildForward )
			{
				pBay->SetCanBeMillsCnrBay( MILLS_TYPE_CONNECT_SSE );
			}
			if( !GetMouseOverrun() && !m_bBuildForward )
			{
				pBay->SetCanBeMillsCnrBay( MILLS_TYPE_CONNECT_NNE );
			}
		}
		
		if( m_LBTplt.m_bFirstBayInRun )
		{
			m_eLapboardWillCrossBaySide = SIDE_INVALID;
			m_LBTplt.SetFirstBayInRun( pBay );

			//All first bays will will have a lapboard on the
			//	western side, so remove all lapboard obsticles

			int iRunID = m_iPreviousRunID;
			Run *pPreviousRun = GetController()->GetRun( iRunID );
			Bay *pPreviousBay;
			pPreviousBay = NULL;
			if( pPreviousRun!=NULL && pPreviousRun->GetAutobuildNumber()==giAutobuildNumber )
			{
				pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
			}
			if( m_LBTplt.IsLapBoardReqdThisRun() || (pPreviousBay!=NULL && pPreviousBay->GetMillsSystemType()!=MILLS_TYPE_NONE) )
			{
				pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL,		WEST );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL,	WEST );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING,		WEST );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH,	WEST );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK,	WEST );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD,	WEST );
				pBay->Delete1000mmHopupRailFromSide( NNW );
				pBay->Delete1000mmHopupRailFromSide( SSW );
				m_eLapboardWillCrossBaySide=WEST;
//				pBay->Redraw();
			}
		}
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////
		//draw the lines
		DrawLine( ptPoints[BP_SW_STD],	ptPoints[BP_SE_STD], iColour );
		DrawLine( ptPoints[BP_SW_STD],	ptPoints[BP_NW_STD], iColour );
		DrawLine( ptPoints[BP_NW_STD],	ptPoints[BP_NE_STD], iColour );
		DrawLine( ptPoints[BP_SE_STD],	ptPoints[BP_NE_STD], iColour );

		DrawCircle( ptPoints[BP_NE_STD], GetSchematicStandardRadius(), iColour );
		DrawCircle( ptPoints[BP_SE_STD], GetSchematicStandardRadius(), iColour );
		DrawCircle( ptPoints[BP_SW_STD], GetSchematicStandardRadius(), iColour );
		DrawCircle( ptPoints[BP_NW_STD], GetSchematicStandardRadius(), iColour );

		if( bStage )
		{
			if( pABBayTemplate->GetWidthOfNStage()>0 )
			{
				DrawLine(	ptPoints[BP_NW_STD],	ptPoints[BP_NNW_STG],	iColour );
				DrawLine(	ptPoints[BP_NNW_STG],	ptPoints[BP_NNE_STG],	iColour );
				DrawLine(	ptPoints[BP_NNE_STG],	ptPoints[BP_NE_STD],	iColour );
			}
			if( pABBayTemplate->GetWidthOfEStage()>0 )
			{
				DrawLine(	ptPoints[BP_NE_STD],	ptPoints[BP_ENE_STG],	iColour );
				DrawLine(	ptPoints[BP_ENE_STG],	ptPoints[BP_ESE_STG],	iColour );
				DrawLine(	ptPoints[BP_ESE_STG],	ptPoints[BP_SE_STD],	iColour );
			}
			if( pABBayTemplate->GetWidthOfSStage()>0 )
			{
				DrawLine(	ptPoints[BP_SE_STD],	ptPoints[BP_SSE_STG],	iColour );
				DrawLine(	ptPoints[BP_SSE_STG],	ptPoints[BP_SSW_STG],	iColour );
				DrawLine(	ptPoints[BP_SSW_STG],	ptPoints[BP_SW_STD],	iColour );
			}
			if( pABBayTemplate->GetWidthOfWStage()>0 )
			{
				DrawLine(	ptPoints[BP_SW_STD],	ptPoints[BP_WSW_STG],	iColour );
				DrawLine(	ptPoints[BP_WSW_STG],	ptPoints[BP_WNW_STG],	iColour );
				DrawLine(	ptPoints[BP_WNW_STG],	ptPoints[BP_NW_STD],	iColour );
			}
			if( pABBayTemplate->GetNEStageCnr() )
			{
				DrawLine(	ptPoints[BP_NE_STD],	ptPoints[BP_NNE_STG],	iColour );
				DrawLine(	ptPoints[BP_NNE_STG],	ptPoints[BP_NE_STG],	iColour );
				DrawLine(	ptPoints[BP_NE_STG],	ptPoints[BP_ENE_STG],	iColour );
				DrawLine(	ptPoints[BP_ENE_STG],	ptPoints[BP_NE_STD],	iColour );
			}
			if( pABBayTemplate->GetSEStageCnr() )
			{
				DrawLine(	ptPoints[BP_SE_STD],	ptPoints[BP_ESE_STG],	iColour );
				DrawLine(	ptPoints[BP_ESE_STG],	ptPoints[BP_SE_STG],	iColour );
				DrawLine(	ptPoints[BP_SE_STG],	ptPoints[BP_SSE_STG],	iColour );
				DrawLine(	ptPoints[BP_SSE_STG],	ptPoints[BP_SE_STD],	iColour );
			}
			if( pABBayTemplate->GetSWStageCnr() )
			{
				DrawLine(	ptPoints[BP_SW_STD],	ptPoints[BP_SSW_STG],	iColour );
				DrawLine(	ptPoints[BP_SSW_STG],	ptPoints[BP_SW_STG],	iColour );
				DrawLine(	ptPoints[BP_SW_STG],	ptPoints[BP_WSW_STG],	iColour );
				DrawLine(	ptPoints[BP_WSW_STG],	ptPoints[BP_SW_STD],	iColour );
			}
			if( pABBayTemplate->GetNWStageCnr() )
			{
				DrawLine(	ptPoints[BP_NW_STD],	ptPoints[BP_WNW_STG],	iColour );
				DrawLine(	ptPoints[BP_WNW_STG],	ptPoints[BP_NW_STG],	iColour );
				DrawLine(	ptPoints[BP_NW_STG],	ptPoints[BP_NNW_STG],	iColour );
				DrawLine(	ptPoints[BP_NNW_STG],	ptPoints[BP_NW_STD],	iColour );
			}
		}

		//Bracing
		if( pABBayTemplate->GetNBrace() )
			DrawLine( ptPoints[BP_NNE_BRC],	ptPoints[BP_NNW_BRC], iColour, 1 );
		if( pABBayTemplate->GetEBrace() )
			DrawLine( ptPoints[BP_ENE_BRC],	ptPoints[BP_ESE_BRC], iColour, 1 );
		if( pABBayTemplate->GetSBrace() )
			DrawLine( ptPoints[BP_SSE_BRC],	ptPoints[BP_SSW_BRC], iColour, 1 );
		if( pABBayTemplate->GetWBrace() )
			DrawLine( ptPoints[BP_WSW_BRC],	ptPoints[BP_WNW_BRC], iColour, 1 );

		//Arrows
		DrawLine( ptPoints[BP_END_ARW],	ptPoints[BP_PNT_ARW], iColour );
		DrawLine( ptPoints[BP_PNT_ARW],	ptPoints[BP_NTH_ARW], iColour );
		DrawLine( ptPoints[BP_PNT_ARW],	ptPoints[BP_STH_ARW], iColour );

		//ties
		if( bTie )
		{
			assert( pABBayTemplate!=NULL );
			TieTypesEnum	eType;
			eType = GetTieType();
			switch( eType )
			{
			case( TIE_TYPE_MASONARY ):	//fallthrough
			case( TIE_TYPE_COLUMN ):	//fallthrough
			case( TIE_TYPE_YOKE ):
				if( GetBaysPerTie()>0 )
				{
					if( pABBayTemplate->GetNNETie() )
					{
						DrawLine(	ptPoints[BP_NNE_STG],		ptPoints[BP_NNE_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_NNE_BEG_TIE],	ptPoints[BP_NNE_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetENETie() )
					{
						DrawLine(	ptPoints[BP_ENE_STG],		ptPoints[BP_ENE_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_ENE_BEG_TIE],	ptPoints[BP_ENE_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetESETie() )
					{
						DrawLine(	ptPoints[BP_ESE_STG],		ptPoints[BP_ESE_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_ESE_BEG_TIE],	ptPoints[BP_ESE_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetSSETie() )
					{
						DrawLine(	ptPoints[BP_SSE_STG],		ptPoints[BP_SSE_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_SSE_BEG_TIE],	ptPoints[BP_SSE_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetSSWTie() )
					{
						DrawLine(	ptPoints[BP_SSW_STG],		ptPoints[BP_SSW_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_SSW_BEG_TIE],	ptPoints[BP_SSW_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetWSWTie() )
					{
						DrawLine(	ptPoints[BP_WSW_STG],		ptPoints[BP_WSW_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_WSW_BEG_TIE],	ptPoints[BP_WSW_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetWNWTie() )
					{
						DrawLine(	ptPoints[BP_WNW_STG],		ptPoints[BP_WNW_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_WNW_BEG_TIE],	ptPoints[BP_WNW_END_TIE],	iColour );
					}

					if( pABBayTemplate->GetNNWTie() )
					{
						DrawLine(	ptPoints[BP_NNW_STG],		ptPoints[BP_NNW_WAL_TIE],	iColour );
						DrawLine(	ptPoints[BP_NNW_BEG_TIE],	ptPoints[BP_NNW_END_TIE],	iColour );
					}
				}	
				break;
			case( TIE_TYPE_BUTTRESS_12 ): 
				DrawLine(	ptPoints[BP_NW_STD],	ptPoints[BP_BUT12_NW],	iColour );
				DrawLine(	ptPoints[BP_BUT12_NW],	ptPoints[BP_BUT12_NE],	iColour );
				DrawLine(	ptPoints[BP_BUT12_NE],	ptPoints[BP_NE_STD],	iColour );
				DrawCircle( ptPoints[BP_BUT12_NW],	GetSchematicStandardRadius(), iColour );
				DrawCircle( ptPoints[BP_BUT12_NE],	GetSchematicStandardRadius(), iColour );
				break;			
			case( TIE_TYPE_BUTTRESS_18 ):
				DrawLine(	ptPoints[BP_NW_STD],	ptPoints[BP_BUT18_NW],	iColour );
				DrawLine(	ptPoints[BP_BUT18_NW],	ptPoints[BP_BUT18_NE],	iColour );
				DrawLine(	ptPoints[BP_BUT18_NE],	ptPoints[BP_NE_STD],	iColour );
				DrawCircle( ptPoints[BP_BUT18_NW],	GetSchematicStandardRadius(), iColour );
				DrawCircle( ptPoints[BP_BUT18_NE],	GetSchematicStandardRadius(), iColour );
				break;			
			case( TIE_TYPE_BUTTRESS_24 ):
				DrawLine(	ptPoints[BP_NW_STD],	ptPoints[BP_BUT24_NW],	iColour );
				DrawLine(	ptPoints[BP_BUT24_NW],	ptPoints[BP_BUT24_NE],	iColour );
				DrawLine(	ptPoints[BP_BUT24_NE],	ptPoints[BP_NE_STD],	iColour );
				DrawCircle( ptPoints[BP_BUT24_NW],	GetSchematicStandardRadius(), iColour );
				DrawCircle( ptPoints[BP_BUT24_NE],	GetSchematicStandardRadius(), iColour );
				break;
			default:
				assert( false );	//invalid tie type!
			}
		}
		if( pABBayTemplate->GetHandRailDist()>0 )
		{
/*			//See Bug# 327 - No handrails on Nth or Sth of the bay
			if( pABBayTemplate->GetNHandRail() )	DrawLine( ptPoints[BP_NW_HND], ptPoints[BP_NE_HND],	iColour, 1 );
			if( pABBayTemplate->GetSHandRail() )	DrawLine( ptPoints[BP_SE_HND], ptPoints[BP_SW_HND],	iColour, 1 );
*/			if( pABBayTemplate->GetEHandRail() )	DrawLine( ptPoints[BP_NE_HND], ptPoints[BP_SE_HND],	iColour, 1 );
			if( pABBayTemplate->GetWHandRail() )	DrawLine( ptPoints[BP_SW_HND], ptPoints[BP_NW_HND],	iColour, 1 );
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//DrawLapboard
//
void AutoBuildTools::DrawLapboard( Point3D ptStart, Point3D ptOuterStd2Direction,
									 bool bErase, bool bCreateSingleLap/*=false*/ )
{
	////////////////////////////////////////////////////////////////////////////
	//rotate GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD] around a line parrallel to the
	//	Z_AXIS through GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD]!
	int					i, iColour;
	Bay					*pBay;
	bool				bStage = true;
	double				dAngle, dBayAngle, dLapAngle, dCommonBayLength;
	Point3D				ptPoints[BP_SIZE], ptLapPoint;
	Matrix3D			Transform, Rotation, Translation;
	Vector3D			Vector, Vect;

	//when creating a single lapboard, we need to limit the
	//	mouse to a 2400 radius circle
	if( bCreateSingleLap )
	{
		if( m_LBTplt.m_dFullLength > m_dBayLengthActual2400 )
		{
			//adjust the second point so that it is 2400 from
			//	the first point in the same direction;
			double dRatio, dSeparation;
			dSeparation = ptStart.distanceTo( ptOuterStd2Direction );
			dRatio = m_dBayLengthActual2400/dSeparation;
			ptOuterStd2Direction.x = ( ( ptOuterStd2Direction.x - ptStart.x)*dRatio ) + ptStart.x;
			ptOuterStd2Direction.y = ( ( ptOuterStd2Direction.y - ptStart.y)*dRatio ) + ptStart.y;
			ptOuterStd2Direction.z = ( ( ptOuterStd2Direction.z - ptStart.z)*dRatio ) + ptStart.z;
			m_LBTplt.m_dFullLength = ptStart.distanceTo( ptOuterStd2Direction );
			//this should now be exactly 2400, give or take a bit
			assert( m_LBTplt.m_dFullLength<=m_dBayLengthActual2400+ROUND_ERROR );
		}
	}
	else
	{
		//////////////////////////////////////////////////////////
		//use the exact separation of the points
		m_LBTplt.m_dFullLength = ptStart.distanceTo( ptOuterStd2Direction );
	}

	//////////////////////////////////////////////////////////
	//Set the length bay
/*	
	The smallest lapboard is 1200!
	if( m_LBTplt.m_dFullLength<=m_dBayLengthActual0700 )	
	{
		m_LBTplt.m_dFullLength = m_dBayLengthActual0700;
		dCommonBayLength = COMPONENT_LENGTH_0700;
		iColour = COLOUR_0700;
	}
	else
*/	
	if( m_LBTplt.m_dFullLength<=m_dBayLengthActual1200+ROUND_ERROR )
	{
		if( !m_LBTplt.m_bAllowExactLength )
			m_LBTplt.m_dFullLength = COMPONENT_LENGTH_1200;
		dCommonBayLength = COMPONENT_LENGTH_1200;
		iColour = COLOUR_1200;
	}
	else if( m_LBTplt.m_dFullLength<=m_dBayLengthActual1800+ROUND_ERROR )
	{
		if( !m_LBTplt.m_bAllowExactLength )
			m_LBTplt.m_dFullLength = COMPONENT_LENGTH_1800;
		dCommonBayLength = COMPONENT_LENGTH_1800;
		iColour = COLOUR_1800;
	}
	else if( m_LBTplt.m_dFullLength<=m_dBayLengthActual2400+ROUND_ERROR )
	{
		if( !m_LBTplt.m_bAllowExactLength )
			m_LBTplt.m_dFullLength = m_dBayLengthActual2400;
		dCommonBayLength = COMPONENT_LENGTH_2400;
		iColour = COLOUR_2400;
	}
	else
	{
/*		assert( false );
		if( !m_LBTplt.m_bAllowExactLength )
			m_LBTplt.m_dFullLength = 0.00;
		dCommonBayLength = 0.00;
		iColour = 0;
*/
		return;
	}

	CString sTemp;
  sTemp.Format( _T("Lapboard Length: %2.0fmm"), dCommonBayLength );
	acedGrText( -2, sTemp, 0 );

	//////////////////////////////////////////////////////////
	//Adjust the point of the lapboard
	m_LBTplt.CreatePoints();

	////////////////////////////////////////////////////////////////////////////
	//copy the points
	for( i=0; i<BP_SIZE; i++ )
	{
		ptPoints[i] = m_LBTplt.m_aPoints[i]; 
	}

	////////////////////////////////////////////////////////////////////////////
	//rotate bay to be parrallel to drag line
	//the anglepoint uses ptStart as an origin
	Vect = ptOuterStd2Direction-ptStart;
	dAngle = CalculateAngle( Vect );
	//create the rotation matix
	Rotation.setToRotation( dAngle, VECTOR_ROTATE_AROUND_ZAXIS );

	////////////////////////////////////////////////////////////////////////////
	//translate bay to start at ptStart
	//Transform.Translate( ptStart.x, ptStart.y, ptStart.z );
	Vector = FromOriginToPointAsVector( ptStart );
	Translation.setToTranslation(Vector);
	Transform = Translation * Rotation;

	////////////////////////////////////////////////////////////////////////////
	//if we are NOT writing to the database, then rotate the points
	if( !m_bAddToDB )
	{
		//apply to the points
		for( i=0; i<BP_SIZE; i++ )
			ptPoints[i] = Transform * ptPoints[i]; 
	}

	//We need to move the lappoint too
	if( m_LBTplt.m_bFirstBayInRun || m_LBTplt.m_bLastBayInRun )
	{
		ptLapPoint = Transform * ptLapPoint;
	}

	////////////////////////////////////////////////////////////////////////////
	//move the 2nd point of the lapboard
	if( m_LBTplt.m_bLastBayInRun )
	{
		m_LBTplt.m_pt2ndPoint = ptLapPoint;
	}

	////////////////////////////////////////////////////////////////////////////
	//Colour
	if( bErase )
	{
		iColour = COLOUR_ERASE;
	}

	if( bCreateSingleLap )
	{
		//////////////////////////////////////////////
		//find the bay the the first point is within
		HitLocationEnum	eHit;
		pBay = GetBayByInternalPoint(ptStart, eHit);
		m_LBTplt.SetLastBayInRun( pBay );
		//the first point must be within a bay!
		if( pBay==NULL )
			return;

		//////////////////////////////////////////////
		//The second point may or may not be within a bay,
		// if it is then find the bay the the second point is within
		pBay = GetBayByInternalPoint(ptOuterStd2Direction, eHit);
		m_LBTplt.SetFirstBayInRun( pBay );
	}

	////////////////////////////////////////////////////////////////////////////
	//copy the lapboard to the new run
	if( m_bAddToDB )
	{
		LapboardBay *pLapboard;

		//////////////////////////////////////////////
		//Create one
		pLapboard = GetController()->CreateNewLapboard();

		double dWidth;
		dWidth = m_LBTplt.m_dFullWidth;
		if( dWidth<=0.001 )
			dWidth = m_LBTplt.GetLastBayInRun()->GetBayWidth();
		pLapboard->SetBayWidth( dWidth );
		pLapboard->SetBayLength( dCommonBayLength );

		if( !bCreateSingleLap )
		{
			//////////////////////////////////////////////
			//Set the Lapboard-Bay Pointers
			assert( m_LBTplt.GetLastBayInRun()!=NULL );
			Run *pPreviousRun = GetController()->GetRun( m_iPreviousRunID );
			if( pPreviousRun==NULL || pPreviousRun->GetAutobuildNumber()!=giAutobuildNumber )
			{
				return;
			}

			Bay *pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
			assert( pPreviousBay!=NULL );

			*pLapboard->GetTemplate() = m_LBTplt;

			pLapboard->SetBayPointers( pPreviousBay, m_LBTplt.GetFirstBayInRun(),
									m_eLapboardWillCrossPreviousBaySide, m_eLapboardWillCrossBaySide );
		}
		else	//if( !bCreateSingleLap )
		{
			//we need to calculate the angle between each of the bays and the lapboard, to 
			//	find out where the lapboard crosses the bay, we can then remove any obsticles
			SideOfBayEnum	eSideWest, eSideEast;
			eSideWest=SIDE_INVALID;
			eSideEast=SIDE_INVALID;

			pBay = m_LBTplt.GetLastBayInRun();
			assert( pBay!=NULL );
			assert( pBay->GetRunPointer()!=NULL );
			//what is the angle of this bay?
			Point3D ptA, ptB;
			ptA.set( 0.00, 0.00, 0.00 );
			ptB.set( pBay->GetBayWidth(), 0.00, 0.00 );
			ptA.transformBy( pBay->GetTransform() );
			ptB.transformBy( pBay->GetTransform() );
			ptA.transformBy( pBay->GetRunPointer()->GetTransform() );
			ptB.transformBy( pBay->GetRunPointer()->GetTransform() );
			Vector = ptB-ptA;
			dBayAngle = CalculateAngle( Vector );
			dLapAngle = GetSensibleAngle( dAngle-dBayAngle );
			if( (dLapAngle>dminus45Deg) && (dLapAngle<=d45Deg) )
			{
				eSideWest=EAST;
			}
			else if( (dLapAngle>d45Deg) && (dLapAngle<=d135Deg) )
			{
				eSideWest=NORTH;
			}
			else if( (dLapAngle>d135Deg) && (dLapAngle<=d225Deg) )
			{
				eSideWest=WEST;
			}
			else
			{
				assert( (dLapAngle>d225Deg) || (dLapAngle<=dminus45Deg) );
				eSideWest=SOUTH;
			}

			pBay = m_LBTplt.GetFirstBayInRun();
			if( pBay!=NULL )
			{
				assert( pBay!=NULL );
				assert( pBay->GetRunPointer()!=NULL );
				//what is the angle of this bay?
				ptA.set( 0.00, 0.00, 0.00 );
				ptB.set( pBay->GetBayWidth(), 0.00, 0.00 );
				ptA.transformBy( pBay->GetTransform() );
				ptB.transformBy( pBay->GetTransform() );
				ptA.transformBy( pBay->GetRunPointer()->GetTransform() );
				ptB.transformBy( pBay->GetRunPointer()->GetTransform() );
				Vector = ptB-ptA;
				dBayAngle = CalculateAngle( Vector );
				//The lapboard runs the other way now, ie from second
				//	to first point, so it will be 180deg out of alignment!
				dAngle+= (dAngle<=d90Deg)? d180Deg: (-1.00*d180Deg);
				dLapAngle = GetSensibleAngle( dAngle-dBayAngle );
				if( (dLapAngle>dminus45Deg) && (dLapAngle<=d45Deg) )
				{
					eSideEast=EAST;
				}
				else if( (dLapAngle>d45Deg) && (dLapAngle<=d135Deg) )
				{
					eSideEast=NORTH;
				}
				else if( (dLapAngle>d135Deg) && (dLapAngle<=d225Deg) )
				{
					eSideEast=WEST;
				}
				else
				{
					assert( (dLapAngle>d225Deg) || (dLapAngle<=dminus45Deg) );
					eSideEast=SOUTH;
				}
			}

			*pLapboard->GetTemplate() = m_LBTplt;
			pLapboard->SetBayPointers( m_LBTplt.GetLastBayInRun(), m_LBTplt.GetFirstBayInRun(),
									eSideWest, eSideEast );
		}

		pLapboard->CreateSchematic();
		pLapboard->Move( Transform, true );
		pLapboard->MoveSchematic( Transform, true );
	}
	else	//if( m_bAddToDB )
	{
		if( m_LBTplt.m_dFullLength>0.00 )
		{
			////////////////////////////////////////////////////////////////////////////
			//draw the lines
			//
			// 11_ _ _ _1_ _ _ _ _4_ _ _ _12
			//         /|         |\
			//       /  |_________|  \
			//     /    2         3    \
			//  0/                       \5
			// 10\                       /
			//     \    8_________7    /
			//       \  |         |  /
			// 13 _ _ _\|_ _ _ _ _|/_ _ _ 14
			//          9         6
			for( i=POINT_LAPBOARD_00; i<POINT_LAPBOARD_10; i++ )
			{
				DrawLine( ptPoints[i], ptPoints[i+1], iColour );
			}
			if( m_LBTplt.GetHandrails() )
			{
				DrawLine( ptPoints[POINT_LAPBOARD_11], ptPoints[POINT_LAPBOARD_12], iColour, PS_DASHED );
				DrawLine( ptPoints[POINT_LAPBOARD_13], ptPoints[POINT_LAPBOARD_14], iColour, PS_DASHED );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//DrawTieTube
//
void AutoBuildTools::DrawTieTube( Point3D pt1, Point3D pt2, bool bErase )
{
	////////////////////////////////////////////////////////////////////////////
	//rotate GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD] around a line parrallel to the
	//	Z_AXIS through GetABBay()->GetTemplate()->m_aPoints[BP_SE_STD]!
	int					iColour;
	Bay					*pBay;
	bool				bAllowExactLength;
	double				dAngle, dFullLength, dOriginalLength, dStdAngle;
	Matrix3D			Transform, Rotation, Translation, Trans;
	Vector3D			Vector;
	BayTieTubeTemplate	*pTieTubeTemplate;

	dOriginalLength = pt1.distanceTo(pt2);
	dFullLength = dOriginalLength+(2.00*OVERRUN_TIE_TUBE);
	bAllowExactLength = false;

	dFullLength = GetController()->GetAvailableLength( CT_TIE_TUBE, MT_STEEL, dFullLength, RT_ROUND_UP, true );

	CString sTemp;
  sTemp.Format( _T("Tie Tube Length: %2.0fmm"), dFullLength );
	acedGrText( -2, sTemp, 0 );

	////////////////////////////////////////////////////////////////////////////
	//rotate bay to be parrallel to drag line
	//the anglepoint uses pt1 as an origin
	dAngle = CalculateAngle( pt2-pt1 );

	//create the rotation matix
	Rotation.setToRotation( dAngle, VECTOR_ROTATE_AROUND_ZAXIS );

	//move the tube back by the overrun value
	Vector.set( -1.00*OVERRUN_TIE_TUBE, 0.00, 0.00 );
	Trans.setToTranslation( Vector );
	Rotation = Rotation * Trans;

	////////////////////////////////////////////////////////////////////////////
	//Colour
	iColour = COLOUR_TIE_TUBE;
	if( bErase )
		iColour = COLOUR_ERASE;

	//////////////////////////////////////////////
	//find the bay where the first point is within
	CornerOfBayEnum eCnr;
	pBay = GetStandardByProxyToPoint( pt1, eCnr );
	//the first point must be within a bay!
	if( eCnr==CNR_INVALID || pBay==NULL )
		return;

	////////////////////////////////////////////////////////////////////////////
	//copy the TieTube to the new run
	if( m_bAddToDB )
	{
		double		dBottomBayRL, dTopBayRL;
		Matrix3D	Transform3D;

		pBay->GetRLsForBayByHeights( dBottomBayRL, dTopBayRL );

		//3D movement within Run
		Transform3D = pBay->GetTransform();
		//3D movement within World
		Transform3D*= pBay->GetRunPointer()->GetTransform();
		//3D movement of crosshair
		Transform3D*= pBay->GetRunPointer()->GetController()->Get3DCrosshairTransform();

		Point3D		ptSE, ptSW;
		//What is the rotation of this TieTube as measured from the
		//	point of view of the bay?
		//	First we need the angle of the southern standards
		ptSE.set( pBay->GetBayLengthActual(), 0.00, 0.00 );
		ptSW.set( 0.00, 0.00, 0.00 );
		ptSE.transformBy( Transform3D );
		ptSW.transformBy( Transform3D );
		dStdAngle = CalculateAngle( ptSE-ptSW );

		//////////////////////////////////////////////
		//Create the Template entry
		pTieTubeTemplate				= new BayTieTubeTemplate();
		pTieTubeTemplate->m_dFullLength	= dFullLength;
		pTieTubeTemplate->m_dAngle		= dAngle-dStdAngle;
		pTieTubeTemplate->m_eStandard	= eCnr;

		pBay->CreateTieTubeFromTemplate(pTieTubeTemplate);

		//////////////////////////////////////////////////////////////////////
		//Remove any 1.0m hopup attached to the second point!
		Bay				*pBaySecond;
		Lift			*pLift;
		Component		*pComp;
		CornerOfBayEnum	eCnrSecond;

		pBaySecond = GetStandardByProxyToPoint( pt2, eCnrSecond );
		if( pBaySecond!=NULL )
		{
			for( int i=0; i<pBaySecond->GetNumberOfLifts(); i++ )
			{
				pLift = pBaySecond->GetLift(i);
				assert( pLift!=NULL );
				SideOfBayEnum	eSide1, eSide2;
				switch( eCnrSecond )
				{
				case( CNR_NORTH_EAST ):
					eSide1 = NNE;
					eSide2 = ENE;
					break;
				case( CNR_SOUTH_EAST ):
					eSide1 = ESE;
					eSide2 = SSE;
					break;
				case( CNR_SOUTH_WEST ):
					eSide1 = SSW;
					eSide2 = WSW;
					break;
				case( CNR_NORTH_WEST ):
					eSide1 = WNW;
					eSide2 = NNW;
					break;
				default:
					assert( false );
				}
				pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSide1 );
				if( pComp!=NULL )
				{
					pLift->DeleteComponent( pComp->GetID() );
				}
				pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSide2 );
				if( pComp!=NULL )
				{
					pLift->DeleteComponent( pComp->GetID() );
				}
			}
		}

	}
	else
	{
		int		j;
		Point3D ptPoints[6]; 

		////////////////////////////////////////////////////////////////////////////
		//translate bay to start at pt1
		//Transform.Translate( pt1.x, pt1.y, pt1.z );
		Vector = FromOriginToPointAsVector( pt1 );
		Translation.setToTranslation(Vector);
		Transform = Translation * Rotation;

		ptPoints[0].set( 0.00, 0.00, 0.00 );
		ptPoints[1].set( dFullLength, 0.00, 0.00 );
		ptPoints[2].set( dFullLength-OVERRUN_TIE_TUBE, -1.00*OVERRUN_TIE_TUBE, 0.00 );
		ptPoints[3].set( dFullLength-OVERRUN_TIE_TUBE, OVERRUN_TIE_TUBE, 0.00 );
		ptPoints[4].set( OVERRUN_TIE_TUBE, -1.00*OVERRUN_TIE_TUBE, 0.00 );
		ptPoints[5].set( OVERRUN_TIE_TUBE, OVERRUN_TIE_TUBE, 0.00 );

		//apply to the points
		for( j=0; j<6; j++ )
		{
			ptPoints[j].transformBy( Transform ); 
		}

		if( dFullLength>0.00 )
		{
			////////////////////////////////////////////////////////////////////////////
			//draw the line
			DrawLine( ptPoints[0], ptPoints[1], iColour );
			DrawLine( ptPoints[2], ptPoints[3], iColour );
			DrawLine( ptPoints[4], ptPoints[5], iColour );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//GetInnerStandardPos
//
void AutoBuildTools::GetInnerStandardPos( double dDistance, double dLength, Point3D ptStart, Point3D &ptStandard )
{
	double fX, fY;

	fX = ptStandard.x-ptStart.x;
	fY = ptStandard.y-ptStart.y;

	ptStandard.x = ((dLength/dDistance)*fX)+ptStart.x;
	ptStandard.y = ((dLength/dDistance)*fY)+ptStart.y;
}

//////////////////////////////////////////////////////////////////////////////////
//GetPoint3D
//
int AutoBuildTools::GetPoint3D( Point3D *pptUCS, CString sPrompt, Point3D &ptPoint )
{
	ads_point	pt;
	int	iRetVal;

	SetMouseTrack( true );
	pptUCS;
/*	This version will create a rubber band mark!
	if( pptUCS!=NULL )
	{
		iRetVal = acedGetPoint( asDblArray( *pptUCS ), sPrompt, pt );
	}
	else
	{
*/		iRetVal = acedGetPoint( NULL, sPrompt, pt );
/*	}
*/
	SetMouseTrack( false );

	ptPoint = asPnt3d(pt);

	return iRetVal;
}

//////////////////////////////////////////////////////////////////////////////////
//GrDraw
//
int AutoBuildTools::GrDraw( Point3D pt1, Point3D pt2, int iColour, int iHl/*=0*/ )
{
	return acedGrDraw( asDblArray( pt1 ), asDblArray( pt2 ), iColour, iHl );
}

//////////////////////////////////////////////////////////////////////////////////
//GetMouseTrack
//
bool AutoBuildTools::GetMouseTrack()
{
	return m_bMouseTrack;
}

//////////////////////////////////////////////////////////////////////////////////
//SetMouseTrack
//
void AutoBuildTools::SetMouseTrack(bool bTracking)
{
	if(	m_ptMousePoint[MP_1ST].x==0.00 &&
		m_ptMousePoint[MP_1ST].y==0.00 &&
		m_ptMousePoint[MP_1ST].z==0.00 &&
		m_ptMousePoint[MP_2ND].x==0.00 &&
		m_ptMousePoint[MP_2ND].y==0.00 &&
		m_ptMousePoint[MP_2ND].z==0.00 )
		bTracking = false;
	m_bMouseTrack = bTracking;
}

//////////////////////////////////////////////////////////////////////////////////
//GetFirstPointFlag
//
bool AutoBuildTools::GetFirstPointFlag()
{
	return m_bFirstPoint;
}

//////////////////////////////////////////////////////////////////////////////////
//SetFirstPointFlag
//
void AutoBuildTools::SetFirstPointFlag(bool bFirstPoint)
{
	m_bFirstPoint = bFirstPoint;
}

//////////////////////////////////////////////////////////////////////////////////
//SetPoint1
//
void AutoBuildTools::SetPoint1( Point3D pt )
{
	m_ptMousePoint[MP_1ST] = pt;
}

//////////////////////////////////////////////////////////////////////////////////
//SetPoint2
//
void AutoBuildTools::SetPoint2( Point3D pt )
{
	m_ptMousePoint[MP_2ND] = pt;
}

//////////////////////////////////////////////////////////////////////////////////
//SetPoint3
//
void AutoBuildTools::SetPoint3( Point3D pt )
{
	m_ptMousePoint[MP_3RD] = pt;
}

//////////////////////////////////////////////////////////////////////////////////
//GetPoint1
//
Point3D AutoBuildTools::GetPoint1()
{
	return m_ptMousePoint[MP_1ST];
}

//////////////////////////////////////////////////////////////////////////////////
//GetPoint2
//
Point3D AutoBuildTools::GetPoint2()
{
	return m_ptMousePoint[MP_2ND];
}

//////////////////////////////////////////////////////////////////////////////////
//GetPoint3
//
Point3D AutoBuildTools::GetPoint3()
{
	return m_ptMousePoint[MP_3RD];
}

//////////////////////////////////////////////////////////////////////////////////
//GetArcFlag
//
bool AutoBuildTools::GetArcFlag()
{
	return m_bArc;
}

//////////////////////////////////////////////////////////////////////////////////
//SetArcFlag
//
void AutoBuildTools::SetArcFlag(bool bArc)
{
	m_bArc = bArc;
}

//////////////////////////////////////////////////////////////////////////////////
//filterMouse
//
BOOL AutoBuildTools::filterMouse(MSG *pMsg)
{
	int		i;
	bool	bKey;
	Point3D pt;

//	acutPrintf("\n%i, %i, %i", pMsg->message, pMsg->wParam, pMsg->lParam );

	m_iCurrentFilterType = FT_AUTOBUILD;

	if( GetMouseTrack() && pMsg->message == WM_LBUTTONDOWN )
	{
		m_iDrawWhenZero = 0;
	}

	if( pMsg->message==WM_MOUSEMOVE )
	{
		if( m_iDrawWhenZero>0 )
		{
			m_iDrawWhenZero--;
			return false;
		}
		m_iDrawWhenZero = 2;
	}
	else
	{
		if( pMsg->message!=WM_LBUTTONDOWN ||
			pMsg->message!=WM_MOUSEMOVE ||
			pMsg->message!=WM_KEYDOWN )
		{
			if( pMsg->message==WM_NCMOUSEMOVE||
				pMsg->message==WM_NCLBUTTONDOWN )
			{
				m_iDrawWhenZero = 0;
			}
			//acutPrintf("\n%i, %i, %i", pMsg->message, pMsg->wParam, pMsg->lParam );
			return false;	
		}
	}

	if( m_bAccessDenied )
	{
		if(pMsg->message==WM_MOUSEMOVE || pMsg->message==WM_LBUTTONDOWN) 
		{
			m_bAccessDenied = false;
			SetAccessEnabled( true );
		}
	}

	bKey = false;
	if ( pMsg->message == WM_KEYDOWN )
	{
		bKey = true;
		//acutPrintf("\n%i, %i, %i", pMsg->message, pMsg->wParam, pMsg->lParam );
		if( GetAccessEnabled() )
		{
			m_bAccessDenied = true;
			SetAccessEnabled( false );
		}
	}

	if( GetAccessEnabled() )
	{
		SetAccessEnabled( false );

		if( GetMouseTrack() )
		{
			int track;
			int type;
			resbuf result;

			track = 0x0001;

			if( !GetUseLastPoints() )
			{
				acedGrRead( track, &type, &result );
			}
			else
			{
				type=5;
			}


			if( type==5 )
			{
				//don't delete the line the first time through
				if( GetFirstPointFlag() )
				{
					//delete the line next time through
					if( GetArcFlag() || m_bBirdCageMode )
					{
						DrawRun( true );
						//refresh the screen
						RefreshScreen();
					}
					SetFirstPointFlag(false);
				}
				else
				{
					//delete the previous line
					if( GetArcFlag() )
					{
						DrawArcBays( true );
					}
					else if( m_bBirdCageMode )
					{
						DrawBirdcage( true );
					}
					else
					{
						for( i=(int)MP_1ST; i<=(int)MP_2ND; i++ )
						{
							m_ptMousePoint[i].z = m_dRL[i];
						}
						DrawRun( true );
					}
				}

				if( !GetUseLastPoints() )
				{
					pt = asPnt3d(result.resval.rpoint);
				}
				else
				{
					if( GetArcFlag() )
					{
						pt = m_ptMousePoint[MP_3RD];
					}
					else if( m_bBirdCageMode )
					{
						pt = m_ptMousePoint[MP_3RD];
					}
					else
					{
						pt = m_ptMousePoint[MP_2ND];
					}
				}
				if( GetArcFlag() )
				{
					//store the point for delete purposes
					AllowForOrtho( pt, MP_3RD );
					SetPoint3( pt );
					DrawArcBays( false );
				}
				else if( m_bBirdCageMode )
				{
					AllowForOrtho( pt, MP_3RD );
					m_ptBirdcagePoints[MP_3RD] = pt;
					DrawBirdcage( false );
				}
				else
				{
					//store the point for delete purposes
					AllowForOrtho( pt, MP_2ND );
					SetPoint2( pt );

					for( i=(int)MP_1ST; i<=(int)MP_2ND; i++ )
					{
						m_ptMousePoint[i].z = m_dRL[i];
					}
					DrawRun( false );
				}
			}
/*			else
			{
				acutPrintf("\n%i, %i, %i, %s, %i, %i, %i", track, type,
						result.resval.rint,
						result.resval.rstring,
						result.resval.rlname[0],
						result.resval.rlname[1],
						result.resval.rlong);
			}
*/		}

		SetAccessEnabled( true );
	}

	SetUseLastPoints( false );

	return FALSE; // continue
}

//////////////////////////////////////////////////////////////////////////////////
//filterMouseForLapboard
//
BOOL AutoBuildTools::filterMouseForLapboard(MSG *pMsg)
{
	Point3D pt;

	if( GetMouseTrack() && pMsg->message == WM_LBUTTONDOWN )
	{
		m_iDrawWhenZero = 0;
	}

	if( pMsg->message==WM_MOUSEMOVE )
	{
		if( m_iDrawWhenZero>0 )
		{
			m_iDrawWhenZero--;
			return false;
		}
		m_iDrawWhenZero = 2;
		//proceed and do drawing
	}
	else
	{
		if( pMsg->message!=WM_LBUTTONDOWN ||
			pMsg->message!=WM_MOUSEMOVE ||
			pMsg->message!=WM_KEYDOWN )
		{
			if( pMsg->message==WM_NCMOUSEMOVE||
				pMsg->message==WM_NCLBUTTONDOWN )
			{
				m_iDrawWhenZero = 0;
			}
//			acutPrintf("\n%i, %i, %i", pMsg->message, pMsg->wParam, pMsg->lParam );
			return false;	
		}
	}

	m_iCurrentFilterType = FT_LAPBOARD;

	if( GetAccessEnabled() )
	{
		SetAccessEnabled( false );

		if( GetMouseTrack() )
		{
			int track;
			int type;
			resbuf result;

			track = 0x0001;

			acedGrRead( track, &type, &result );

			if( type==5 )
			{
				//don't delete the line the first time through
				if( GetFirstPointFlag() )
				{
					SetFirstPointFlag(false);
				}
				else
				{
					m_LBTplt.m_dFullLength = m_ptMousePoint[MP_1ST].distanceTo( m_ptMousePoint[MP_2ND] );
					DrawLapboard( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], true, true );
				}

				pt = asPnt3d(result.resval.rpoint);
				AllowForOrtho( pt, MP_2ND );

				//store the point for delete purposes
				SetPoint2( pt );

				m_LBTplt.m_dFullLength = m_ptMousePoint[MP_1ST].distanceTo( m_ptMousePoint[MP_2ND] );
				DrawLapboard( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], false, true );
			}
		}

		SetAccessEnabled( true );
	}

	return FALSE; // continue
}

/////////////////////////////////////////////////////////////////
//filterMouseForTieTube
BOOL AutoBuildTools::filterMouseForTieTube(MSG *pMsg)
{
	Point3D pt;

	m_iCurrentFilterType = FT_TIE_TUBE;

	if( GetMouseTrack() && pMsg->message == WM_LBUTTONDOWN )
	{
		m_iDrawWhenZero = 0;
	}

	if( pMsg->message==WM_MOUSEMOVE )
	{
		if( m_iDrawWhenZero>0 )
		{
			m_iDrawWhenZero--;
			return false;
		}
		m_iDrawWhenZero = 2;
	}
	else
	{
		if( pMsg->message!=WM_LBUTTONDOWN ||
			pMsg->message!=WM_MOUSEMOVE ||
			pMsg->message!=WM_KEYDOWN )
		{
			if( pMsg->message==WM_NCMOUSEMOVE||
				pMsg->message==WM_NCLBUTTONDOWN )
			{
				m_iDrawWhenZero = 0;
			}
//			acutPrintf("\n%i, %i, %i", pMsg->message, pMsg->wParam, pMsg->lParam );
			return false;	
		}
	}

	if( GetAccessEnabled() )
	{
		SetAccessEnabled( false );

		if( GetMouseTrack() )
		{
			int track;
			int type;
			resbuf result;

			track = 0x0001;

			acedGrRead( track, &type, &result );

			if( type==5 )
			{
				//don't delete the line the first time through
				if( GetFirstPointFlag() )
				{
					SetFirstPointFlag(false);
				}
				else
				{
					DrawTieTube( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], true );
				}

				pt = asPnt3d(result.resval.rpoint);
				AllowForOrtho( pt, MP_2ND );

				//store the point for delete purposes
				SetPoint2( pt );

				DrawTieTube( m_ptMousePoint[MP_1ST], m_ptMousePoint[MP_2ND], false );
			}
		}

		SetAccessEnabled( true );
	}

	return FALSE; // continue
}

//////////////////////////////////////////////////////////////////////////////////
//SetClosePoint
//
void AutoBuildTools::SetClosePoint(Point3D ptClose)
{
	if( !m_bClosePointSet )
	{
		m_ptClosePoint = ptClose;
		m_bClosePointSet = true;
	}
}


void AutoBuildTools::SetGlobalsToDefault()
{
	m_bBirdCageMode		= false;
	SetAccessEnabled( true );
	SetMouseTrack(false);

	m_dCrossHairWidth	= 1000.00;
	m_dCrossHairHeight	= 1000.00;
	m_iBayColour		= 2;

	m_dDistanceInnerStandardToArc	= GetDistanceFromWall();

	SetMouseOverrun( true );
}

void AutoBuildTools::RefreshScreen( bool bFilterMouse/*=true*/ )
{
	//The database is locked when adding to the database
	if( !m_bAddToDB )
	{
		int iRet = acedRedraw( NULL, 2 );
		assert( iRet==RTNORM );
	}

	if( bFilterMouse )
	{
		MSG Msg;
		Msg.message=WM_MOUSEMOVE;
		SetUseLastPoints();
		
		switch( m_iCurrentFilterType )
		{
		case( FT_AUTOBUILD ):
			filterMouse( &Msg );
			break;
		case( FT_LAPBOARD ):
			filterMouseForLapboard( &Msg );
			break;
		case( FT_TIE_TUBE ):
			filterMouseForTieTube( &Msg );
			break;
		default:
			assert( false );
		}
	}
	SetFirstPointFlag(true);
}

void AutoBuildTools::ReturnToWallMode()
{
	SetFirstPointFlag(true);
	m_bAddToDB = false;

	//cancel the arc
	SetArcFlag( false );

	//refresh the screen
	RefreshScreen();
}

void AutoBuildTools::DrawLine(Point3D pt1, Point3D pt2, int iColour, int iHL/*=0*/ )
{
	GrDraw(	pt1, pt2, iColour, iHL );
}

void AutoBuildTools::DrawCircle(Point3D pt, double dRadius, int iColour, int iHL/*=0*/ )
{
	int	i;	
	Point3D	Circle[8];

	for( i=0; i<8; i++ )
		Circle[i] = pt;

	Circle[0].y+= dRadius;

	Circle[1].x+= dRadius*cos(d45Deg);
	Circle[1].y+= dRadius*sin(d45Deg);

	Circle[2].x+= dRadius;

	Circle[3].x+= dRadius*cos(d45Deg);
	Circle[3].y-= dRadius*sin(d45Deg);

	Circle[4].y-= dRadius;

	Circle[5].x-= dRadius*cos(d45Deg);
	Circle[5].y-= dRadius*sin(d45Deg);

	Circle[6].x-= dRadius;
	
	Circle[7].x-= dRadius*cos(d45Deg);
	Circle[7].y+= dRadius*sin(d45Deg);

	GrDraw( Circle[0], Circle[1], iColour, iHL );
	GrDraw( Circle[1], Circle[2], iColour, iHL );
	GrDraw( Circle[2], Circle[3], iColour, iHL );
	GrDraw( Circle[3], Circle[4], iColour, iHL );
	GrDraw( Circle[4], Circle[5], iColour, iHL );
	GrDraw( Circle[5], Circle[6], iColour, iHL );
	GrDraw( Circle[6], Circle[7], iColour, iHL );
	GrDraw( Circle[7], Circle[0], iColour, iHL );
}

BayTemplate * AutoBuildTools::GetBayTemplate()
{
	return GetABBay()->GetTemplate();
}

LapBoardTemplate * AutoBuildTools::GetLapBoardTemplate()
{
	return &m_LBTplt;
}

RunTemplate * AutoBuildTools::GetRunTemplate()
{
	return Run::GetRunTemplate();
}


void AutoBuildTools::SetBayLength( double dLength )
{
	int		iID;

	GetABBay()->GetTemplate()->SetBayLength( dLength );
	for( iID=0; iID<GetNumberOfBays(); iID++ )
	{
		GetBay( iID )->SetBayLength( dLength );
	}
}

void AutoBuildTools::SetBayWidth(double dWidth)
{
	int		iID;

	GetABBay()->GetTemplate()->SetBayWidth( dWidth );
	for( iID=0; iID<GetNumberOfBays(); iID++ )
	{
		GetBay( iID )->SetBayWidth( dWidth );
	}
}

void AutoBuildTools::SetMouseOverrun(bool bOverrun/*=true*/)
{
	m_bOverrunMousePoint = bOverrun;
}

bool AutoBuildTools::GetMouseOverrun()
{
	return m_bOverrunMousePoint;
}

void AutoBuildTools::Overrun2ndPoint( double dDistanceOverrun )
{
	double	dDistance, dNewDistance, dRatio, dDX, dDY, dDZ;
	Point3D	ptTemp1, ptTemp2;

	ptTemp1.set( m_ptMousePoint[MP_1ST].x, m_ptMousePoint[MP_1ST].y, 0.00 );
	ptTemp2.set( m_ptMousePoint[MP_2ND].x, m_ptMousePoint[MP_2ND].y, 0.00 );
	dDistance = ptTemp1.distanceTo( ptTemp2 );
	dNewDistance = dDistance + dDistanceOverrun;

	//prevent divide by zero
	if( dDistance>=-1.00*ROUND_ERROR && dDistance<=ROUND_ERROR ) 
	{
		//copy the z-values to avoid the RL warning
		m_ptMousePoint[MP_2ND].z = m_ptMousePoint[MP_1ST].z;
		return;
	}

	dRatio = dNewDistance/dDistance;
	dDX = m_ptMousePoint[MP_2ND].x-m_ptMousePoint[MP_1ST].x;
	dDY = m_ptMousePoint[MP_2ND].y-m_ptMousePoint[MP_1ST].y;
	dDZ = m_ptMousePoint[MP_2ND].z-m_ptMousePoint[MP_1ST].z;

	m_ptMousePoint[MP_2ND].x = (dRatio * dDX) + m_ptMousePoint[MP_1ST].x;
	m_ptMousePoint[MP_2ND].y = (dRatio * dDY) + m_ptMousePoint[MP_1ST].y;
	m_ptMousePoint[MP_2ND].z = (dRatio * dDZ) + m_ptMousePoint[MP_1ST].z;
}


Bay * AutoBuildTools::GetABBay()  const
{
	return GetBay(0);
}


void AutoBuildTools::Serialize(CArchive &ar)
{
	BOOL BTemp;

	Run::Serialize(ar);

	if (ar.IsStoring())    // Store Object?
	{
		ar << AUTOBUILD_VERSION_LATEST;

		//AUTOBUILD_VERSION_1_0_1
		STORE_bool_IN_AR(m_bBuildForward);

		//AUTOBUILD_VERSION_1_0_0
		STORE_bool_IN_AR(m_bOverrunMousePoint);

		STORE_bool_IN_AR(m_bArc);				//Are we creating an arc run?
		STORE_bool_IN_AR(m_bAddToDB);			//do we want to add this to the database or just draw to screen
		STORE_bool_IN_AR(m_bMouseTrack);		//are we currently tracking the mouse?
		STORE_bool_IN_AR(m_bFirstPoint);		//Is this the first point in a run?
		STORE_bool_IN_AR(m_bAccessEnabled);	//Used to prevent re-entry into a function
		STORE_bool_IN_AR(m_bBirdCageMode);


		ar << m_iColour;		//General colour variable
		ar << m_iBayColour;	//Colour of the bay - size dependant

		int iMousePointIndex;
		for (iMousePointIndex = 0; iMousePointIndex < MP_SIZE; iMousePointIndex++)
		{
			ar << m_ptMousePoint[iMousePointIndex].x;	//The 3 mouse location point (3 for arc, 2 for wall)
			ar << m_ptMousePoint[iMousePointIndex].y;	
			ar << m_ptMousePoint[iMousePointIndex].z;	
			ar << m_ptBirdcagePoints[iMousePointIndex].x;
			ar << m_ptBirdcagePoints[iMousePointIndex].y;
			ar << m_ptBirdcagePoints[iMousePointIndex].z;
		}
		STORE_bool_IN_AR(m_bClosePointSet);			//Has the closing point been set?
		ar << m_ptClosePoint.x;				//The closing point
		ar << m_ptClosePoint.y;				
		ar << m_ptClosePoint.z;				

//		LapBoardTemplate	m_LBTplt;	//The lapboard template

		ar << m_dCrossHairWidth;		
		ar << m_dCrossHairHeight;

	}
	else					// or Load Object?
	{
		CString sMsg, sTemp;
		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case AUTOBUILD_VERSION_1_0_1 :
			LOAD_bool_IN_AR(m_bBuildForward);
		case AUTOBUILD_VERSION_1_0_0 :
			LOAD_bool_IN_AR(m_bOverrunMousePoint);

			LOAD_bool_IN_AR(m_bArc);				//Are we creating an arc run?
			LOAD_bool_IN_AR(m_bAddToDB);			//do we want to add this to the database or just draw to screen
			LOAD_bool_IN_AR(m_bMouseTrack);		//are we currently tracking the mouse?
			LOAD_bool_IN_AR(m_bFirstPoint);		//Is this the first point in a run?
			LOAD_bool_IN_AR(m_bAccessEnabled);	//Used to prevent re-entry into a function
			LOAD_bool_IN_AR(m_bBirdCageMode);

			ar >> m_iColour;		//General colour variable
			ar >> m_iBayColour;	//Colour of the bay - size dependant

			int iMousePointIndex;
			for (iMousePointIndex = 0; iMousePointIndex < MP_SIZE; iMousePointIndex++)
			{
				ar >> m_ptMousePoint[iMousePointIndex].x;	//The 3 mouse location point (3 for arc, 2 for wall)
				ar >> m_ptMousePoint[iMousePointIndex].y;	
				ar >> m_ptMousePoint[iMousePointIndex].z;	
				ar >> m_ptBirdcagePoints[iMousePointIndex].x;
				ar >> m_ptBirdcagePoints[iMousePointIndex].y;
				ar >> m_ptBirdcagePoints[iMousePointIndex].z;
			}
			LOAD_bool_IN_AR(m_bClosePointSet);			//Has the closing point been set?
			ar >> m_ptClosePoint.x;				//The closing point
			ar >> m_ptClosePoint.y;				
			ar >> m_ptClosePoint.z;				

	//		LapBoardTemplate	m_LBTplt;	//The lapboard template

			ar >> m_dCrossHairWidth;		
			ar >> m_dCrossHairHeight;

			break;
		default:
			assert( false );
			if( uiVersion>AUTOBUILD_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: Autobuild.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), AUTOBUILD_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

double AutoBuildTools::GetHalfAngleOuterStandards(double GH,
								double CD, double EF, double B)
{
	double	BCD, E, K, K2, G, A, J;	//, I;
///////////////////////////////////////////////////////////////////////////////
//
//                            _____H_________G_____
//                          / \         |        /. \
//                        /     \       |      / .    \E
//                      /         \    I|    /  .K      \
//                    /\         /  \   |  /  \.         /\
//                  /    \     /    /  *|*  \ . \     D/    \F
//                /        \ /    /*    |    *\   \  /        \
//              /          / \  /       |   .   \ C/\           \
//               \       /    /\        |  .     /\   \         /
//                 \   /    /*   \      | .    /   *\   \     /
//                   \    / *      \    |J.A /B     * \   \ / 
//                     \/  *         \  |. /         *  \ /
//                         *           \ /           *
//                         *                         * 
//                          *                       * 
//                           *                     *
//                            *                   *  
//                              *               *
//                                 *         *
//                                     * * 
//
//
///////////////////////////////////////////////////////////////////////////////
//Alternatively:
///////////////////////////////////////////////////////////////////////////////
//
//
//                              ____H________G___ 
//                             /\       |       .\
//                           /    \     |     /.   \ 
//                         /        \   |   /  .     \E
//                       /           /\ | /\  .K       \
//                     /           /  / | \  \.          \
//                   /           /  /   |   \. \           \
//                 / \         /  /   *****  .\  \         / \
//               /     \     /  / **    |   .** \  \     /D    \F
//             /         \ /  /*       I|   .    *\  \ /         \
//           /           / \/*          |  .       *\/C\           \
//         /           /  /* \          |  .       / *\  \           \
//       /           /  / *    \        | .      /    * \  \           \
//       \         /  /  *       \      |J. A  /B      *  \  \         /
//         \     /  /    *         \    |.   /         *    \  \     /
//           \ /  /     *            \  |. /            *     \  \ /
//             \/       *              \|/              *       \/
//                      *               |               *
//                      *               |               *
//                       *              |              *
//
//
//We are trying to find the angle	"JA"!
//	We know:
//	the Radius						"B",
//	the Standard Separation			"EF", 
//	the BayWidth plus stage width	"CD",
//	the Length of the Lapboard		"GH",
//	the Raduis of the Circle		"B",
//	H = G
//	E = F
//
///////////////////////////////////////////////////////////////////////////////

	BCD	= B + CD;
	E	= EF / 2.00;
	K2	= ( BCD*BCD ) + ( E*E );
	K	= sqrt( K2 );
	G	= GH / 2.00;
//	I	= sqrt( K2 - ( G*G ) );
	A	= atan( E / BCD );
	J	= asin( G / K );
	return ( A+J );
}


double AutoBuildTools::GetDistanceToArcOfInnerStandard( double BC, double A)
{
	double	B, D, E, DE, DE2;

///////////////////////////////////////////////////////////////////////////////
//
//                           ________________________ 
//                          |           |            |
//                          |           |            |
//                          |___________|____________|
//                          |_____B_____|______C_____|
//                           .        *****
//                           D.   **    |    **
//                             *        |        *
//                           *  .       |          *
//                         *     .      |            *
//                        *       .E    |A            *
//                       *         .    |              *
//                       *          .   |              *
//                      *            .  |               *
//                      *             . |               * 
//                      *              .|               *
//                      *               |               *
//                       *              |              *
//
//We are trying to find the distance from the inner standard to the arc (D)!
//	We know:
//	the Radius						"A",
//	the Standard Separation			"BC", 
//	E = A
//	B = C
//
///////////////////////////////////////////////////////////////////////////////

	E	= A;
	B	= BC/2.00;
	DE2	= (B*B) + (A*A);
	DE	= sqrt( DE2 );
	assert(DE>=E);
	D	= DE-E;
	return D;
}


double AutoBuildTools::GetHalfAngleInnerStandards(double ABCD,
								double EF, double GH )
{
	double	J, F, G, L, K, KL;

///////////////////////////////////////////////////////////////////////////////
//
//                                ************
//                          ****____H________G___****
//                      ***    /\       |       .\    ***
//                   **      /   /\     |     /.\  \      **
//                 *       /   /    \   |   /  .  \  \F      *
//               *       /   /        \ | /   .     \  \       *
//             *       /   /          / | \   .       \  \     /*
//            *      /   /          /   |   \.          \  \ /D   *
//          *      / \ /          /     |    .\           \/C\      *
//         *     /   / \        /       |   .J  \        /  \  \     * 
//        *    /   /     \    /        I|   .     \    /B     \  \E   *
//       *   /   /         \/           |  .        \/          \  \   *
//       * /   /          /  \          |  .       /  \           \  \ *
//      */   /          /      \        | .      /      \           \  \*
//      *\ /          /          \      |K. L  /A         \           \/*
//     *   \        /              \    |.   /              \        /   *
//     *     \    /                  \  |. /                  \    /     *
//     *       \/                      \|/                      \/       *
//     *                                |                                *
//     *                                |                                *
//     *                                |                                *
//      *                                                               *
//
//We are trying to find the angle	"KL"!
//	We know:
//	the Radius						"ABCD",
//	the Standard Separation			"EF", 
//	the Length of the Lapboard		"GH",
//	J = ABCD
//	E = F
//	G = H
//
///////////////////////////////////////////////////////////////////////////////

	J	= ABCD;
	F	= EF/2.00;
	G	= GH/2.00;
	L	= asin( F/J );
	K	= asin( G/J );
	KL	= L+K;
	return KL;
}

double AutoBuildTools::GetDistanceToArcOfMidBay(double ABCD, double EF )
{
	double	ABC, ABC2, E, G, D;

///////////////////////////////////////////////////////////////////////////////
//
//                                ************
//                          ****        |        ****
//                      ***             |D            ***
//                   **.______E_________|_______F________ **
//                 *   |.               |C               |  *
//               *     |_.______________|________________|     *
//             *       |  .             |                |      *
//            *        |   .            |                |        *
//          *          |    .           |B               |          *
//         *           |     .          |                |           * 
//        *            |______._________|________________|            *
//       *                     .        |                              *
//       *                      .G      |                              *
//      *                        .      |                               *
//      *                         .     |A                              *
//     *                           .    |                                *
//     *                            .   |                                *
//     *                             .  |                                *
//     *                              . |                                *
//     *                               .|                                *
//     *                                |                                *
//      *                                                               *
//
//We are trying to find the distance "D"!
//	We know:
//	the Radius						"ABCD",
//	the Standard Separation			"EF", 
//	G = ABCD
//	E = F
//
///////////////////////////////////////////////////////////////////////////////

	G		= ABCD;
	E		= EF/2.00;
	ABC2	= ( G*G ) - ( E*E );
	ABC		= sqrt( ABC2 );
	D		= ABCD - ABC;
	return D;
}


double AutoBuildTools::DistToAvoidBoundBox( bool &bUseBoundingBoxCorner, bool bErase, double &dAngle )
{
	//We are trying to find distance AC
    //                                            I
	//                                            /\             
	//                                          /    \         
	//                                        /        \       
	//                                      /            \     
	//                                    /                xB
	//                                 H/                 /
	//     D____________________________\ J             /
	//     |              |             G|\           / 
	//     |              |              |  \       /
	//     |              |              |    \   /
	//     |              |              |     C/
	//     |              |              |    /
	//     |              |              |  /
	//     |______________|______________|/
	//     E                             AF
	//
	//                                                
	// Alternatively,
	//                                                
	//                                                
	//     D______________________J______G
	//     |              |              |
	//     |              |       |      |
	//     |              |              |
	//     |              |       |      |
	//     |              |              |        
	//     |              |       |      |   /H\
	//     |______________|_______x______|F/     \
	//     E                      A \    /         \
	//                                \/             \
	//                                 C\              \
	//                                    \             I\
	//                                      \           /
 	//                                        \       /
	//                                          \   / 
	//                                           Bx  
	//
	//                                                
	// Alternatively,
	//           
	//         xB
	//       /   \                              
	//     /       \                              
	//   /           \                              
	// /               \                              
	// \I                \ C                          
	//   \                /\                          
	//     \            /    \                         
	//       \        /        \                      
	//         \  H /            \                    
	//     D_____\/________________\______________JG
	//     |            |            \            |
	//     |            |            | \          |
	//     |            |            |   \        |
	//     |            |            |     \      |
	//     |            |            |       \    |      
	//     |            |            |         \  |
	//     |____________|____________|___________\x
	//     E                                      AF
	//
	//
	//What we know:
	//-------------
	//	D,E,F,G,D	= list of points which make up the bounding rectangle of the previous run
	//	A			= First Mouse Point (m_ptMousePoint[MP_1ST])
	//	B			= Second Mouse Point (m_ptMousePoint[MP_2ND])
	//	CH			= Width of the bay
	//	angle(ACH)	= 90 degrees
	//
	//
	//What we need to find out
	//------------------------
	//	C			= point on line between A and B, such that H is outside of previous bounding box
	//	AC			= This function needs to return the distance from A to C!
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//Solution1:
	//	Use angles to determine maths involved, and calculate the distance directly!

	int					iPreviousRunID;
	double				AB, CH, AC, DE, EA, EF, AF, FG, GJ, GD, JD, MC, AM,
						angle, angleFAD, angleFAE, angleFAKalt, angleFAF,
						angleFAG, angleFAJ, angleFAB, angleFAK, angleDAL,
						angleFAL, angleCAG, angleCHM;
	Point3D				A, B, C, D, E, F, G, J, K;
	Vector3D			Vector;
	Matrix3D			Transform, Trans, Rotation;
	Point3DArray		DEFGD;

	bUseBoundingBoxCorner = false;

	//the current run would be "GetNumberOfRuns()-1"
	iPreviousRunID			= m_iPreviousRunID;
	assert( iPreviousRunID>=0 );

	////////////////////////////////////////////////////////////////
	//We know these items:
	DEFGD = *GetController()->GetRun(iPreviousRunID)->
							GetRunTemplate()->GetBoundingBoxPointer();

	A			= m_ptMousePoint[MP_1ST];
	B			= m_ptMousePoint[MP_2ND];
//	DrawCrosshair( A, 500.00, 200.00, bErase );
//	DrawCrosshair( B, 200.00, 500.00, bErase );

	assert( DEFGD.length()==5 );
	E			= DEFGD.at(1);
	F			= DEFGD.at(2);
	CH			= GetABBay()->GetBayWidthActual()
					+ GetABBay()->GetWidthOfStageFromWall( SOUTH ) + GetDistanceFromWall()
					+ GetABBay()->GetWidthOfStageFromWall( NORTH ) + GetDistanceFromWall();

	//we are not interested in the z value
	A.z			= 0.00;
	B.z			= 0.00;
	E.z			= 0.00;
	F.z			= 0.00;
	
	////////////////////////////////////////////////////////////////
	//move the points such that E is at the origin
	Vector.set( -1.00*E.x, -1.00*E.y, -1.00*E.z );
	Transform.setToTranslation( Vector );
	F.transformBy( Transform );

	////////////////////////////////////////////////////////////////
	//And F is on the +ve X-axis
	angle = -1.00*CalculateAngle( F );
	Rotation.setToRotation( angle, VECTOR_ROTATE_AROUND_ZAXIS );
	Transform = Rotation * Transform;

	////////////////////////////////////////////////////////////////
	//move all points 
	A.transformBy( Transform );
	B.transformBy( Transform );
//	DrawCrosshair( A, 500.00, 200.00, bErase );
//	DrawCrosshair( B, 200.00, 500.00, bErase );
/*
	if( A.x>F.x+5.00 )
		return 0.00;
*/
	DEFGD.transformBy( Transform );
	D = DEFGD.at(0);
	E = DEFGD.at(1);
	F = DEFGD.at(2);
	G = DEFGD.at(3);

#ifdef	_DRAW_DEBUG_BOUND_BOX_
	DrawLine( A, B, (bErase)? COLOUR_ERASE: COLOUR_GREY );
	DrawLine( D, E, (bErase)? COLOUR_ERASE: COLOUR_WHITE );
	DrawLine( E, F, (bErase)? COLOUR_ERASE: COLOUR_WHITE );
	DrawLine( F, G, (bErase)? COLOUR_ERASE: COLOUR_WHITE );
	DrawLine( G, D, (bErase)? COLOUR_ERASE: COLOUR_WHITE );
#endif	//#ifdef	_DRAW_DEBUG_BOUND_BOX_
	
	////////////////////////////////////////////////////////////////
	//We can now find J
	J	= A;
	J.y	= D.y;
	K	= A;
	K.y	= D.y*-1.00;

#ifdef	_DRAW_DEBUG_BOUND_BOX_
	DrawLine( A, J, (bErase)? COLOUR_ERASE: COLOUR_BLUE );
	DrawLine( A, D, (bErase)? COLOUR_ERASE: COLOUR_RED );
	DrawLine( A, G, (bErase)? COLOUR_ERASE: COLOUR_RED );
	DrawLine( A, K, (bErase)? COLOUR_ERASE: COLOUR_CYAN );
#endif	//#ifdef	_DRAW_DEBUG_BOUND_BOX_

	//         xB
	//       /   \                              
	//     /       \                              
	//   /           \                              
	// /               \                              
	// \I                \ C                          
	//   \                /\                          
	//     \            /    \                         
	//       \        /        \                      
	//         \  H /            \                    
	//     D_____\/________________\______________JG
	//     |            |            \            |
	//     |            |            | \          |
	//     |            |            |   \        |
	//     |            |            |     \      |
	//     |            |            |       \    |      
	//     |            |            |         \  |
	//     |____________|____________|___________\x
	//     E                                      AF
	//
	////////////////////////////////////////////////////////////////
	//We can now find some distances
	AB = A.distanceTo( B );
	DE = D.distanceTo( E );
	EA = E.distanceTo( A );
	EF = E.distanceTo( F );
	AF = A.distanceTo( F );
	FG = F.distanceTo( G );
	GJ = G.distanceTo( J );
	GD = G.distanceTo( D );
	JD = J.distanceTo( D );
	//double check some of these
	if( AB<0.001 && AB>-0.001 )
		return 0.00;
//	assert( EA-JD<0.001 && EA-JD>-0.001 );
//	assert( AF-GJ<0.001 && AF-GJ>-0.001 );
//	assert( DE-FG<0.001 && DE-FG>-0.001 );
//	assert( EF-GD<0.001 && EF-GD>-0.001 );

	////////////////////////////////////////////////////////////////
	//We can now find the angles involved
	Vector3D vFA = F-A;
	Vector3D vDA = D-A;
	Vector3D vEA = E-A;
	Vector3D vGA = G-A;
	Vector3D vJA = J-A;
	Vector3D vKA = K-A;
	Vector3D vBA = B-A;
	angleFAD	= GetSensibleAngle( (CalculateAngle( vDA )-CalculateAngle( vFA) ));
 	angleFAE	= d180Deg;
	angleFAF	= 0.00;
	angleFAG	= GetSensibleAngle( (CalculateAngle( vGA )-CalculateAngle( vFA) ));
	angleFAJ	= d90Deg;
	angleFAK	= dminus90Deg;
	angleFAB	= GetSensibleAngle( (CalculateAngle( vBA )-CalculateAngle( vFA) ));
 	angleFAKalt	= d270Deg;

	////////////////////////////////////////////////////////////////
	//Special case: When D==H we have special circumstances that will
	//	affect all angles between EAD and when D==H (Lets call it
	//	angleEAL, since the point H will nolonger lie on line DG.
	//	We can work this out because we know that angle DCL is a right
	//	anlge, we know HC and we know DA
	//
	//         xB
	//       /   \                              
	//     /       \                              
	//   /           \                              
	// /               \                              
	// \I                \ C                          
	//   \                /\                          
	//     \            /    \                         
	//       \        /        \                      
	//         \  H /            \                    
	//           D_________________\______________JG
	//           |          |        \            |
	//           |          |        | \          |
	//           |          |        |   \        |
	//           |          |        |     \      |
	//           |          |        |       \    |      
	//           |          |        |         \  |
	//           |__________|________|___________\x
	//           E                                AF
	angleDAL	= asin(CH/(D.distanceTo( A )));
	angleFAL	= angleFAD - angleDAL;

#ifdef _SHOW_ANGLES_5
  acutPrintf(_T("\nAngle:%3.4f"), angleFAB*CONVERT_TO_DEG );
	acutPrintf(_T("\t(%1.0f, %1.0f) (%1.0f, %1.0f) (%1.0f, %1.0f) (%1.0f, %1.0f) (%1.0f, %1.0f) (%1.0f, %1.0f)"),
				angleFAE*CONVERT_TO_DEG, angleFAKalt*CONVERT_TO_DEG,
				angleFAK*CONVERT_TO_DEG, angleFAF*CONVERT_TO_DEG,
				angleFAF*CONVERT_TO_DEG, angleFAG*CONVERT_TO_DEG,
				angleFAG*CONVERT_TO_DEG, angleFAL*CONVERT_TO_DEG,
				angleFAL*CONVERT_TO_DEG, angleFAD*CONVERT_TO_DEG,
				angleFAD*CONVERT_TO_DEG, angleFAE*CONVERT_TO_DEG );
#endif //#ifdef _SHOW_ANGLES_5

	dAngle = angleFAB;

	////////////////////////////////////////////////////////////////
	//which angle range does FAB lie in?
	if( angleFAB>angleFAE && angleFAB<=angleFAKalt )
	{
		////////////////////////////////////////////////////////////////
		//           D_______________________________J_____G
		//           |            |     L    |       |     |
		//           |            |          |             |
		//           |            |          |       |     |
		//           |            |          |             |
		//           |            |          |       |     |      
		//           |            |          |             |
		//           |____________|__________|_______x_____|
		//           E                              /A\    F
		//                                        /  C  \    
		//                                      /         \  
		//                                    /      |      \
		//                                  /                 \H
		//                                /          |        /
		//                              /            K      /
		//                         B  /             Kalt  /  
		//                          x                   /    
		//                            \               /      
		//                              \           /        
		//                                \       /          
		//                                  \  I/           
		//                                    \              
		//                                                   
		//                                                   
		//We know:
		//	1. AC==0.00,

		AC = 0.00;
	}
	else if( angleFAB>=angleFAK && angleFAB<angleFAF )
	{
		////////////////////////////////////////////////////////////////
		//           D_______________________________J_____G
		//           |            |     L    |       |     |
		//           |            |          |             |
		//           |            |          |       |     |
		//           |            |          |             |    H
		//           |            |          |       |     |    /\
		//           |            |          |             |  /    \
		//           |____________|__________|_______x_____|/        \
		//           E                               A\   /F           \
		//                                           |  \C               \
		//                                                \                \
		//                                           |      \                I
		//                                                    \            /
		//                                           |          \        /
		//                                           K            \    /
		//                                          Kalt            \/
		//                                                           B
		//We know:
		//	1. angleACF is right angles,
		//	2. distance AF,
		//	3. angleFAC==angleFAB,
		//	4. Cosine = Adjacent/Hypotenuse
		//	5. Adjacent = Cosine*Hypotenuse
		//	6. AC = cos(angleFAB)*AF

		if( angleFAB<angleFAK+dONE_DEGREE )
		{
			//This is nearly right angles
			AC = 0;
		}
		else
		{
			AC = cos(fabs(angleFAB))*AF;
		}
	}
	else if( angleFAB>=angleFAF && angleFAB<angleFAG )
	{
		bUseBoundingBoxCorner = true;
		////////////////////////////////////////////////////////////////
		//                                            I
		//                                            /\             
		//                                          /    \         
		//                                        /        \       
		//                                      /            \     
		//                                    /                xB
		//                                 H/                 /
		//     D____________________________\ J             /
		//     |              |             G|\           / 
		//     |              |              |  \       /
		//     |              |              |    \   /
		//     |              |              |     C/
		//     |              |              |    /
		//     |              |              |  /
		//     |______________|______________|/
		//     E                             AF
		//We know:
		//	1. angleCAG = angleFAG - angleFAB,
		//	2. AG,
		//	3. cos(angleCAG) = AC/AG
		//	4. AC = cos(angleCAG)*AG

		angleCAG = angleFAG - angleFAB,
		AC = cos(angleCAG)*A.distanceTo(G);
	}
	else if( angleFAB>=angleFAG && angleFAB<angleFAL )
	{
		bUseBoundingBoxCorner = true;
		////////////////////////////////////////////////////////////////
		//                                                                      
		//                           .xB                                        
		//                        ..   .                                        
		//                     ..       .                                       
		//                     I.        .                                      
		//                       .        .                                     
		//                        .        .                                    
		//                         .       ..C                                  
		//                          .   ..   .                                  
		//           D______________H.._______.M_____JG
		//           |          |   L      |   .     |
		//           |          |          |    .    | 
		//           |          |          |     .   |
		//           |          |          |      .  |
		//           |          |          |       . |
		//           |          |          |        .|
		//           |__________|__________|_________x
		//           E                               AF
		//We know:
		//	1. angleMCH = right angle
		//	2. angleHMC = angleEAB = angleFAE-angleFAB
		//	3. 180degrees = angleCHM + angleHMC + angleMCH
		//	4. angleCHM = 180degrees - angleMCH - angleHMC
		//	5. angleCHM = d90Deg - d45Deg - angleHMC  [from 1]
		//	6. angleCHM = d45Deg - angleHMC
		//	7. angleCHM = d45Deg - (angleFAE-angleFAB)	[from 2]
		//	8. tan = opposite/adjacent
		//	9. tan(angleCHM) = MC/CH
		//	10.MC = tan(angleCHM)*CH
		//	11.angleJMA = angleHMC
		//	12.angleAJM = right angle
		//	13.angleMAJ = angleCHM
		//	14.angleFAM = angleFAB
		//	15.angleMAJ = angleFAM - angleFAJ
		//	16.angleMAJ = angleFAB - angleFAJ
		//	13.cos = adjacent/hypotenuse
		//	14.cos(angleMAJ) = AJ/AM
		//	15.AM = AJ/cos(angleMAJ)
		//	16.AM = AJ/cos(angleCHM)	[combine 12 and 15]
		//	17.AC = AM+MC
		
		angleCHM = angleFAB-angleFAJ;
		MC = tan(fabs(angleCHM))*CH;
		AM = A.distanceTo(J)/cos(fabs(angleCHM));
		AC = AM + MC;
	}
	else if( angleFAB>=angleFAL && angleFAB<=angleFAD )
	{
		bUseBoundingBoxCorner = true;
		////////////////////////////////////////////////////////////////
		//        B                                                             
		//       /\                                                               
		//     /    \                                                           
		//   /        \                                                         
		// /            \                                                       
		//I               \                                                     
		//\                 \C                                                  
		//  \             /   \                                                 
		//    \         /       \                                               
		//      \    D/___________\____._________JG
		//        \ /|             |\  L         |
		//         H |             |  \          |
		//           |             |    \        |
		//           |             |      \      |
		//           |             |        \    |
		//           |             |          \  |         
		//           |_____________|____________\|
		//           E                           AF
		//We know:
		//	angleCAD = angleFAD - angleFAB,
		//	angleDCA = right angle,
		//	AD,
		//	cos = adjacent/hypotenuse
		//	adjacent = cos*hypotenuse
		//	AC = cos(angleCAD)*AD
		
		AC = cos(angleFAD-angleFAB)*A.distanceTo(D);
	}
	else if( angleFAB>angleFAD && angleFAB<=angleFAE )
	{
		////////////////////////////////////////////////////////////////
		//                                                                      
		//                                                                      
		//                   D_________________.______JG
		//        B          |           |    L      |
		//        x....      |           |           |
		//        .      .....C          |           |
		//        .          .  .....    |           |
		//       .          .|         .....         |
		//      I.          .|           |   .....   |
		//      .....      . |___________|________...x
		//             ..... E                        AF            
		//                H                                           
		//We know:
		//	angleEAC = angleFAE-angleFAB,
		//	angleAEC = right angle
		//	EA,
		//	cos = adjacent/hypotenuse
		//	cos(angleEAC) = EA/AC
		//	AC = EA/cos(angleFAE-angleFAB)
		
		AC = A.distanceTo(E)/cos(angleFAE-angleFAB);
	}
	else
	{
		assert( false );
#ifdef _SHOW_ANGLES_5
		acutPrintf(_T("Invalid"));
#endif //#ifdef _SHOW_ANGLES_5
		AC = 0.00;
	}

#ifdef	_DRAW_DEBUG_BOUND_BOX_

	double BC = B.distanceTo( C );
	Point3DArray	TempBoundBox;
	TempBoundBox.RemoveAll();
	TempBoundBox.append( Point3D( 0.00, CH, 0.00 ) );
	TempBoundBox.append( Point3D( 0.00, 0.00, 0.00 ) );
	TempBoundBox.append( Point3D( BC, 0.00, 0.00 ) );
	TempBoundBox.append( Point3D( BC, CH, 0.00 ) );
	TempBoundBox.append( Point3D( 0.00, CH, 0.00 ) );

	Vector.set( AC, 0.00, 0.00 );
	Transform.setToTranslation( Vector );

	Rotation.setToRotation( angleFAB+CalculateAngle(F-A), VECTOR_ROTATE_AROUND_ZAXIS );
	Transform = Rotation * Transform;
	
	Vector.set( A.x-E.x, A.y-E.y, 0.00 );
	Trans.setToTranslation( Vector );
	Transform = Trans * Transform;

	TempBoundBox.transformBy( Transform );

	DrawPolyLine( TempBoundBox, (bErase)? COLOUR_ERASE: COLOUR_MAGENTA );
#endif	//#ifdef	_DRAW_DEBUG_BOUND_BOX_

	return AC;
}


void AutoBuildTools::DrawPolyLine(Point3DArray pts, int iColour)
{
	int i, iSize;

	iSize = pts.length()-1;
	for( i=0; i<iSize; i++ )
	{
		DrawLine( pts[i], pts[i+1], iColour );
	}
}

bool AutoBuildTools::FindZOfXYPointOnPlane(Point3D &pt, Point3D ptRLs[MP_SIZE])
{
	double		D;
	Point3D		A, B, C;
	Vector3D	Vector;

	///////////////////////////////////////////////////////////////////
	//ptRLs forms a plane, provided that two of the points aren't the same
	if( (ptRLs[MP_1ST].distanceTo( ptRLs[MP_2ND] ) < 0.001) ||
		(ptRLs[MP_2ND].distanceTo( ptRLs[MP_3RD] ) < 0.001) ||
		(ptRLs[MP_3RD].distanceTo( ptRLs[MP_1ST] ) < 0.001) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Use Cross product to find equation for a plane
	//	__   __  |   i       j       k   |
	//	AB x AC	=|(Bx-Ax) (By-Ay) (Bz-Az)|
	//			 |(Cx-Ax) (Cy-Ay) (Cz-Az)|
	//
	//			= ((By-Ay)*(Cz-Az))-((Cy-Ay)*(Bz-Az))i
	//			 +((Bx-Ax)*(Cz-Az))-((Cx-Ax)*(Bz-Az))j
	//			 +((Bx-Ax)*(Cy-Ay))-((Cx-Ax)*(By-Ay))k
	//
	//		  D	= Ax + By + Cz
	//
	A = ptRLs[MP_1ST];
	B = ptRLs[MP_2ND];
	C = ptRLs[MP_3RD];
	Vector.set(	((B.y-A.y)*(C.z-A.z))-((C.y-A.y)*(B.z-A.z)),
				((C.x-A.x)*(B.z-A.z))-((B.x-A.x)*(C.z-A.z)),
				((B.x-A.x)*(C.y-A.y))-((C.x-A.x)*(B.y-A.y)) );
	D = (Vector.x*A.x) + (Vector.y*A.y) + (Vector.z*A.z);

	///////////////////////////////////////////////////////////////////
	//We need to find the z value for a point on this plane!
	//from the plane equation, we know
	//D = (Vector.x*pt.x) + (Vector.y*pt.y) + (Vector.z*pt.z);
	//we are trying to find pt.z
	pt.z = (D - ((Vector.x*pt.x) + (Vector.y*pt.y)))/Vector.z;

	return true;
}

bool AutoBuildTools::FindZOfXYPointOnLine(Point3D &pt, Point3D ptRLs[2])
{
	double	dX, dY, dZ, dPointSeparation, dRatio;

	dPointSeparation = ptRLs[0].distanceTo(ptRLs[1]);

	//////////////////////////////////////////////////
	//value must be +ve so no need to test for -ve values!
	if( dPointSeparation<0.001 )
	{
		pt = ptRLs[0];
		return true;
	}

	///////////////////////////////////////////////////////////////////////
	//if you look below at the distance version of this function, you will
	//	see where I get the ratio that I am talking about in this equation!
	//	i.e.
	//pt.set(	(dRatio*(ptRLs[1].x-ptRLs[0].x))+ptRLs[0].x,
	//			(dRatio*(ptRLs[1].y-ptRLs[0].y))+ptRLs[0].y,
	//			(dRatio*(ptRLs[1].z-ptRLs[0].z))+ptRLs[0].z );
	//where 	dRatio = dDistance/dPointSeparation;
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//ignoring the z values, the pt must lye on the line 
	//	formed by the other two points
	dX = ptRLs[1].x-ptRLs[0].x;
	dY = ptRLs[1].y-ptRLs[0].y;
	dZ = ptRLs[1].z-ptRLs[0].z;

	if( dX<0.00-ROUND_ERROR || dX>0.00+ROUND_ERROR )
	{
		/////////////////////////////////////////////
		//The RL points are seperated in X value

		//calculate the ratio (dDistance/dPointSeparation)
		dRatio = (pt.x-ptRLs[0].x)/dX;

		#ifdef	_DEBUG
		//Check that the point is on the line!
		if( dY<0.00-ROUND_ERROR || dY>0.00+ROUND_ERROR )
		{
			//Check: The calculated ratio's must be the same!
			assert( ((pt.y-ptRLs[0].y)/dY)>dRatio-ROUND_ERROR );
			assert( ((pt.y-ptRLs[0].y)/dY)<dRatio+ROUND_ERROR );
		}
		else
		{
			//We have a line || to the X axis, so the point should have the
			//	the same y value!
			assert( ptRLs[0].y>pt.y-ROUND_ERROR );
			assert( ptRLs[0].y<pt.y+ROUND_ERROR );
		}
		#endif	//#ifdef	_DEBUG
	}
	else if( dY<0.00-ROUND_ERROR || dY>0.00+ROUND_ERROR )
	{
		/////////////////////////////////////////////
		//The RL points form a line || to the y axis
		dRatio = (pt.y-ptRLs[0].y)/dY;

		#ifdef	_DEBUG
		//The point we are trying to find should also be on the line
		assert( ptRLs[0].x>pt.x-ROUND_ERROR );
		assert( ptRLs[0].x<pt.x+ROUND_ERROR );
		#endif	//#ifdef	_DEBUG
	}
	else
	{
		/////////////////////////////////////////////
		//The RL points have the same X&Y values.
		dRatio = 0.00;

		#ifdef	_DEBUG
		if( dZ>0.00+ROUND_ERROR || dZ<0.00-ROUND_ERROR )
		{
			//We have a line || to the Z-Axis, how can we give a z value!
			assert( false );
		}
		else
		{
			//No need to worry, the z value is the same too!  Do nothing
			1;
		}
		#endif	//#ifdef	_DEBUG
	}

	/////////////////////////////////////////////////////
	//if we got this far then it point's XY values lie on the
	//	line of the points.  Find the z value!
	pt.z = (dRatio*dZ)+ptRLs[0].z;

	return true;
}

bool AutoBuildTools::FindPointOfDistanceAlongLine(double dDistance, Point3D	&pt, Point3D ptRLs[2])
{
	double	dPointSeparation, dRatio;

	assert( false ); //function not tested

	dPointSeparation = ptRLs[0].distanceTo(ptRLs[1]);

	//////////////////////////////////////////////////
	//value must be +ve so no need to test for -ve values!
	if( dPointSeparation<0.001 )
	{
		pt.set( 0.00, 0.00, 0.00 );
		return false;
	}

	dRatio = dDistance/dPointSeparation;
	pt.set( (dRatio*(ptRLs[1].x-ptRLs[0].x))+ptRLs[0].x,
			(dRatio*(ptRLs[1].y-ptRLs[0].y))+ptRLs[0].y,
			(dRatio*(ptRLs[1].z-ptRLs[0].z))+ptRLs[0].z );
	return true;
}


void AutoBuildTools::SetFirstRunInAutobuild(bool bFirstRun/*=true*/ )
{
	m_bFirstRunInThisAutobuild = bFirstRun;
}

bool AutoBuildTools::IsFirstRunInAutobuild()
{
	return m_bFirstRunInThisAutobuild;
}

double AutoBuildTools::GetTopRLFromAutobuild()
{
	return GetABBay()->GetRLOfLift( GetABBay()->GetNumberOfLifts()-1 );
}

bool AutoBuildTools::CompareRLWithABBayRL(double dRL, double dABRL, bool bCausedByOverrun/*=false*/ )
{
	if( dRL>=dABRL )
	{
		CString sErrMsg, sTemp;
		MessageBeep(MB_ICONEXCLAMATION);
		sErrMsg.Format( _T("You have set the RL for a bay to be %1.2fm high!\n"), ConvertStarRLtoRL(dRL+GetABBay()->RLAdjust(), GetStarSeparation())*CONVERT_MM_TO_M );
		if( bCausedByOverrun )
			sErrMsg+= _T("This is a calculated RL, due to overrun of the mouse point.\n");
		sTemp.Format( _T("However the top lift height for the bay is %1.2fm\n\n"), ConvertStarRLtoRL(dABRL+GetABBay()->RLAdjust(), GetStarSeparation())*CONVERT_MM_TO_M );
		sErrMsg+=sTemp;
		sTemp.Format( _T("This RL is invalid!  Your RL for the bay must be LESS THAN %1.2fm.\n"), ConvertStarRLtoRL(dABRL+GetABBay()->RLAdjust(), GetStarSeparation())*CONVERT_MM_TO_M );
		sErrMsg+=sTemp;
		if( bCausedByOverrun )
		{
			sErrMsg+= _T("I suggest underrunning the mouse to avoid this problem!\n");
			sErrMsg+= _T("I cannot create the run due to this error!!");
		}
		MessageBox( NULL, sErrMsg, ERROR_MSG_INVALID_RL, MB_OK );
		return false;
	}
	return true;
}

void AutoBuildTools::AllowForOrtho(Point3D &pt, int iPointToConsider)
{
	double dAngle;
	Vector3D	Vect;

	if( acdbHostApplicationServices()->workingDatabase()->orthomode() )
	{
		Vect = pt-m_ptMousePoint[iPointToConsider-1];
		dAngle = CalculateAngle( Vect );

#ifdef _SHOW_ANGLES_4
//		acutPrintf("\nAngle:%0.2f (%0.2f, %0.2f, %0.2f, %0.2f)", dAngle, (pi/-8.00), (3.00*pi/8.00), (5.00*pi/-8.00), (pi/-8.00) );
#endif	//#ifdef _SHOW_ANGLES_4

		if( ( dAngle>dminus45Deg && dAngle<d45Deg ) ||
			( dAngle>d135Deg && dAngle<d225Deg ) )
		{
			pt.y = m_ptMousePoint[iPointToConsider-1].y;
		}
		else
		{
			pt.x = m_ptMousePoint[iPointToConsider-1].x;
		}
	}
}

void AutoBuildTools::SetAccessEnabled(bool bEnabled)
{
	m_bAccessEnabled = bEnabled;
}

bool AutoBuildTools::GetAccessEnabled()
{
	return m_bAccessEnabled;
}

Bay * AutoBuildTools::GetBayByInternalPoint( Point3D ptInternal, HitLocationEnum &eHit, double dGravity/*=0.00*/ )
{
	//find which bay the first point(ptStart) is within, we will
	//	then use that bay to create the lift heights
	int			iRun, iBay;
	Point3D		pt;
	Run			*pRun;
	Bay			*pBay;
	Matrix3D	RunTrans, BayTrans, Trans, Temp1, Temp2;
	double		dBayWidth, dBayLength, dStages[4];

	for( iRun=0; iRun<GetController()->GetNumberOfRuns(); iRun++ )
	{
		pRun = GetController()->GetRun( iRun );
		assert( pRun!=NULL );
		RunTrans = pRun->GetSchematicTransform().invert();
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			/////////////////////////////////////////////////
			//How far do we have to unmove this point to see
			//	if it is within this bay?
			pBay = pRun->GetBay( iBay );
			assert( pBay!=NULL );
			BayTrans = pBay->GetSchematicTransform().invert();
			dBayWidth  = pBay->GetBayWidthActual();
			dBayLength = pBay->GetBayLengthActual();
			dStages[NORTH]	= pBay->GetTemplate()->GetWidthOfNStage();
			dStages[EAST]	= pBay->GetTemplate()->GetWidthOfEStage();
			dStages[SOUTH]	= pBay->GetTemplate()->GetWidthOfSStage();
			dStages[WEST]	= pBay->GetTemplate()->GetWidthOfWStage();

			//Where is the point after unmoving
			pt = ptInternal;
			pt.transformBy( RunTrans );
			pt.transformBy( BayTrans );

			//don't forget the south stage, west stage should
			// be drawn in negative so it should be fine

			#ifdef _SHOW_GRAVITY_POINTS_
			//show the gravity points
			Point3D	pts[4];
			pts[0].set( -1.00*dGravity-dStages[WEST],		-1.00*dGravity-dStages[SOUTH],		100.00 );
			pts[1].set( dBayLength+dGravity+dStages[EAST],	-1.00*dGravity-dStages[SOUTH],		100.00 );
			pts[2].set( dBayLength+dGravity+dStages[EAST],	dBayWidth+dGravity+dStages[NORTH],	100.00 );
			pts[3].set( -1.00*dGravity-dStages[WEST],		dBayWidth+dGravity+dStages[NORTH],	100.00 );
			Temp1 = RunTrans;
			Temp2 = BayTrans;
			Trans = Temp1.invert()*Temp2.invert();
			pts[0].transformBy(Trans);
			pts[1].transformBy(Trans);
			pts[2].transformBy(Trans);
			pts[3].transformBy(Trans);
			DrawLine( pts[0], pts[1], COLOUR_WHITE, 1 );
			DrawLine( pts[1], pts[2], COLOUR_WHITE, 1 );
			DrawLine( pts[2], pts[3], COLOUR_WHITE, 1 );
			DrawLine( pts[3], pts[0], COLOUR_WHITE, 1 );
			#endif	//#ifdef _SHOW_GRAVITY_POINTS_

			/////////////////////////////////////////////////
			//Where did they hit?
			////////////////////////////////////////
			//                                    //
			//                9                   //
			//         ______________             //
			//        | 8 |  1   | 5 |            //
			//        |___|______|___|            //
			//        |   |      |   |            //
			//        |   |      |   |            //
			//   9    | 4 |  0   | 2 |     9      //  10
			//        |   |      |   |            //
			//        |___|______|___|            //
			//        | 7 |  3   | 6 |            //
			//        |___|______|___|            //
			//                                    //
			//               9                    //
			//                                    //
			////////////////////////////////////////
			//where:
			//	HL_BAY				= 0
			//	HL_HOPUP_NORTH		= 1
			//	HL_HOPUP_EAST		= 2
			//	HL_HOPUP_SOUTH		= 3
			//	HL_HOPUP_WEST		= 4
			//	HL_CNR_FILLER_NE	= 5
			//	HL_CNR_FILLER_SE	= 6
			//	HL_CNR_FILLER_SW	= 7
			//	HL_CNR_FILLER_NW	= 8
			//	HL_MISSED			= 9
			//	HL_INVALID			= 10

			eHit = HL_MISSED;
			if( pt.x>=0.00-dGravity-dStages[WEST] && pt.x<=dBayLength+dGravity+dStages[EAST] )
			{
				//this is within/on the X range of the bay
				if( pt.y>=0.00-dGravity-dStages[SOUTH] && pt.y<=dBayWidth+dGravity+dStages[NORTH] )
				{
					//we have a hit, but where?
					if( pt.x>=0.00-dGravity-dStages[WEST] && pt.x<0.00-dGravity )
					{
						//hit some where on the western side!
						if( pt.y>=0.00-dGravity-dStages[SOUTH] && pt.y<0.00-dGravity )
						{
							eHit = HL_CNR_FILLER_SW;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Corner Filler South West"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else if( pt.y>dBayWidth+dGravity && pt.y<=dBayWidth+dGravity+dStages[NORTH] )
						{
							eHit = HL_CNR_FILLER_NW;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Corner Filler North West"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else
						{
							eHit = HL_HOPUP_WEST;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Hopup West"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
					}
					else if( pt.x>dBayLength+dGravity && pt.x<=dBayLength+dGravity+dStages[EAST] )
					{
						//hit some where on the eastern side!
						if( pt.y>=0.00-dGravity-dStages[SOUTH] && pt.y<0.00-dGravity )
						{
							eHit = HL_CNR_FILLER_SE;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Corner Filler South East"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else if( pt.y>dBayWidth+dGravity && pt.y<=dBayWidth+dGravity+dStages[NORTH] )
						{
							eHit = HL_CNR_FILLER_NE;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Corner Filler North East"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else
						{
							eHit = HL_HOPUP_EAST;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Hopup East"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
					}
					else
					{
						//Hit some where in the Middle
						if( pt.y>=0.00-dGravity-dStages[SOUTH] && pt.y<0.00-dGravity )
						{
							eHit = HL_HOPUP_SOUTH;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Stage South"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else if( pt.y>dBayWidth+dGravity && pt.y<=dBayWidth+dGravity+dStages[NORTH] )
						{
							eHit = HL_HOPUP_NORTH;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Stage North"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
						else
						{
							eHit = HL_BAY;
							#ifdef _SHOW_GRAVITY_POINTS_
              acutPrintf(_T("\nHit: Inside Bay"));
							#endif	//#ifdef _SHOW_GRAVITY_POINTS_
						}
					}

					return pBay;
				}
			}
		}
	}
	return NULL;
}

Bay * AutoBuildTools::GetStandardByProxyToPoint( Point3D pt, CornerOfBayEnum &eCnr )
{
	//find which bay the first point(ptStart) is within, we will
	//	then use that bay to create the lift heights
	int			iRun, iBay;
	Point3D		ptStd[4], ptTest;
	Run			*pRun;
	Bay			*pBay;
	Matrix3D	Trans, TransRun;
	double		dBayWidth, dBayLength, dDistance;

	eCnr = CNR_INVALID;
	for( iRun=0; iRun<GetController()->GetNumberOfRuns(); iRun++ )
	{
		pRun = GetController()->GetRun( iRun );
		TransRun = pRun->GetSchematicTransform().invert();
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay( iBay );

			Trans = pBay->GetSchematicTransform().invert() * TransRun;
			dBayWidth  = pBay->GetBayWidthActual();
			dBayLength = pBay->GetBayLengthActual();

			#ifdef _DEBUG
			Point3D point;
			point.set( 0.00, 0.00, 0.00 );
			point.transformBy( pBay->GetSchematicTransform() );
			point.transformBy( pRun->GetSchematicTransform() );
			DrawCircle( point, GRAVITY_TO_STANDARD, COLOUR_WHITE );
			point.set( dBayLength, 0.00, 0.00 );
			point.transformBy( pBay->GetSchematicTransform() );
			point.transformBy( pRun->GetSchematicTransform() );
			DrawCircle( point, GRAVITY_TO_STANDARD, COLOUR_WHITE );
			point.set( dBayLength, dBayWidth, 0.00 );
			point.transformBy( pBay->GetSchematicTransform() );
			point.transformBy( pRun->GetSchematicTransform() );
			DrawCircle( point, GRAVITY_TO_STANDARD, COLOUR_WHITE );
			point.set( 0.00, dBayWidth, 0.00 );
			point.transformBy( pBay->GetSchematicTransform() );
			point.transformBy( pRun->GetSchematicTransform() );
			DrawCircle( point, GRAVITY_TO_STANDARD, COLOUR_WHITE );
			#endif	//#ifdef _DEBUG

			
			ptTest = pt;
			ptTest.transformBy( Trans );

			//NE
			eCnr = CNR_NORTH_EAST;
			ptStd[eCnr].set( dBayLength, dBayWidth, 0.00 );
			dDistance = ptStd[eCnr].distanceTo(ptTest);
			if( dDistance<GRAVITY_TO_STANDARD )
				return pBay;

			if( pBay->GetInner()==NULL )
			{
				//SE
				eCnr = CNR_SOUTH_EAST;
				ptStd[eCnr].set( dBayLength, 0.00, 0.00 );
				dDistance = ptStd[eCnr].distanceTo(ptTest);
				if( dDistance<GRAVITY_TO_STANDARD )
					return pBay;
			}

			if( pBay->GetBackward()==NULL )
			{
				//NW
				eCnr = CNR_NORTH_WEST;
				ptStd[eCnr].set( 0.00, dBayWidth, 0.00 );
				dDistance = ptStd[eCnr].distanceTo(ptTest);
				if( dDistance<GRAVITY_TO_STANDARD )
					return pBay;

				if( pBay->GetInner()==NULL )
				{
					//SW
					eCnr = CNR_SOUTH_WEST;
					ptStd[eCnr].set( 0.00, 0.00, 0.00 );
					dDistance = ptStd[eCnr].distanceTo(ptTest);
					if( dDistance<GRAVITY_TO_STANDARD )
						return pBay;
				}
			}
		}
	}
	return NULL;
}

void AutoBuildTools::InsetPoints()
{
	double dDistanceInset, dAngle;
	Point3D	ptTemp1, ptTemp2;
	Vector3D	Vector, Vect;
	Matrix3D	Transform, Rotation, Trans;

	//How far do we have to translate these two points?
	Vector.set( -1.00*m_ptMousePoint[MP_1ST].x, -1.00*m_ptMousePoint[MP_1ST].y, -1.00*m_ptMousePoint[MP_1ST].z ); 
	Transform.setToTranslation( Vector );

	//what is the anlge of the points?
	Vect = m_ptMousePoint[MP_2ND]-m_ptMousePoint[MP_1ST];
	dAngle = CalculateAngle( Vect );
	Rotation.setToRotation( -1.00*dAngle, VECTOR_ROTATE_AROUND_ZAXIS );
	Transform = Rotation * Transform;

	//Offset them from the wall and the width of the template
	dDistanceInset = GetDistanceFromWall()
					+ GetABBay()->GetTemplate()->GetWidthOfSStage( )
					+ DEBUG_OFFSET_TO_WALL;
	Vector.set( 0.00, dDistanceInset, 0.00 ); 
	Trans.setToTranslation( Vector );
	Transform = Trans * Transform;

	//Rotate them back
	Rotation.setToRotation( dAngle, VECTOR_ROTATE_AROUND_ZAXIS );
	Transform = Rotation * Transform;

	//translate them back
	Vector.set( m_ptMousePoint[MP_1ST].x, m_ptMousePoint[MP_1ST].y, m_ptMousePoint[MP_1ST].z ); 
	Trans.setToTranslation( Vector );
	Transform = Trans * Transform;

	m_ptMousePoint[MP_1ST].transformBy( Transform );
	m_ptMousePoint[MP_2ND].transformBy( Transform );
}


void AutoBuildTools::SetUseLastPoints( bool bUseLast/*=true*/)
{
	//if something has been drawn on screen
	if(	m_ptMousePoint[MP_1ST].x==0.00 &&
		m_ptMousePoint[MP_1ST].y==0.00 &&
		m_ptMousePoint[MP_1ST].z==0.00 &&
		m_ptMousePoint[MP_2ND].x==0.00 &&
		m_ptMousePoint[MP_2ND].y==0.00 &&
		m_ptMousePoint[MP_2ND].z==0.00 )
		bUseLast = false;

	m_bUseLastPoints = bUseLast;
}

bool AutoBuildTools::GetUseLastPoints()
{
	return m_bUseLastPoints;
}

double AutoBuildTools::GetSensibleAngle(double dAngle)
{
	while( dAngle<dminus90Deg )
		dAngle+=d360Deg;
	while( dAngle>d270Deg )
		dAngle-=d360Deg;

	assert( dAngle<=d270Deg );
	assert( dAngle>=dminus90Deg );
	return dAngle;
}

void AutoBuildTools::RemoveLapboardObsticals(Bay *pBay, double dAngle)
{
	if( pBay->GetMillsSystemType()!=MILLS_TYPE_NONE )
		return;

	assert( dAngle == GetSensibleAngle( dAngle ) );
	//Depending on the angle we will need to
	//	remove certain obsticals such as hand rails
	if( dAngle>d45Deg && dAngle<=d135Deg )
	{
		//Delete northern stuff
		#ifdef _DRAW_LAPBOARD_POINTS_
		acutPrintf(_T(" - Delete North!"));
		#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
		pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, NORTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, NORTH );
		pBay->Delete1000mmHopupRailFromSide( WNW );
		pBay->Delete1000mmHopupRailFromSide( ENE );
		m_eLapboardWillCrossPreviousBaySide=NORTH;
		pBay->Redraw();
	}
	else if( ( dAngle<dminus45Deg	&& dAngle>=dminus135Deg ) ||
			 ( dAngle>d225Deg		&& dAngle<=d315Deg ) )
	{
		//Delete southern stuff
		#ifdef _DRAW_LAPBOARD_POINTS_
		acutPrintf(_T(" - Delete South!"));
		#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
		pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, SOUTH );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, SOUTH );
		pBay->Delete1000mmHopupRailFromSide( WSW );
		pBay->Delete1000mmHopupRailFromSide( ESE );
		m_eLapboardWillCrossPreviousBaySide=SOUTH;
		pBay->Redraw();
	}
	else if( dAngle<d45Deg && dAngle>=dminus45Deg )
	{
		//Delete eastern stuff
		#ifdef _DRAW_LAPBOARD_POINTS_
		acutPrintf(_T(" - Delete East!"));
		#endif	//#ifdef _DRAW_LAPBOARD_POINTS_
		pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL, EAST );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, EAST );
		//There is no Toe board on the East nor West!
		pBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING, EAST );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, EAST );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, EAST );
		pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, EAST );
		pBay->Delete1000mmHopupRailFromSide( NNE );
		pBay->Delete1000mmHopupRailFromSide( SSE );
		m_eLapboardWillCrossPreviousBaySide=EAST;
		pBay->Redraw();
	}
	else
	{
		//Western side, do nothing!
	}
}


void AutoBuildTools::SetController(Controller *pController)
{
	assert( pController->GetCompDetails()!=NULL );
	Run::SetController( pController );
}


void AutoBuildTools::SetPredefinedBayLengths(Controller *pController)
{
	m_dBayLengthActual0700 = pController->GetCompDetails()->GetActualLength( pController->GetSystem(), CT_LEDGER, COMPONENT_LENGTH_0700, MT_STEEL );
	m_dBayLengthActual1200 = pController->GetCompDetails()->GetActualLength( pController->GetSystem(), CT_LEDGER, COMPONENT_LENGTH_1200, MT_STEEL );
	m_dBayLengthActual1800 = pController->GetCompDetails()->GetActualLength( pController->GetSystem(), CT_LEDGER, COMPONENT_LENGTH_1800, MT_STEEL );
	m_dBayLengthActual2400 = pController->GetCompDetails()->GetActualLength( pController->GetSystem(), CT_LEDGER, COMPONENT_LENGTH_2400, MT_STEEL );
	m_dStandardWidthActual = pController->GetCompDetails()->GetActualWidth( pController->GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL );
	m_dBayLengthActual0700+= m_dStandardWidthActual;
	m_dBayLengthActual1200+= m_dStandardWidthActual;
	m_dBayLengthActual1800+= m_dStandardWidthActual;
	m_dBayLengthActual2400+= m_dStandardWidthActual;
}

bool AutoBuildTools::UseMillsSystemCorner()
{
	if( GetController()->IsMillsSystemCorner() )
	{
		if( GetABBay()->GetBayWidth()!=COMPONENT_LENGTH_1200 )
			return false;

		Bay	*pPreviousBay;
		Run	*pPreviousRun;
		pPreviousBay = NULL;
		pPreviousRun = GetController()->GetRun( m_iPreviousRunID );
		if( pPreviousRun!=NULL )
		{
			pPreviousBay = pPreviousRun->GetBay( pPreviousRun->GetNumberOfBays()-1 );
			if( pPreviousBay->GetMillsSystemType()==MILLS_TYPE_CONNECT_SSE ||
				pPreviousBay->GetMillsSystemType()==MILLS_TYPE_CONNECT_NNE )
			{
				//the last corner was a mills corner, better not use it again
				return false;
			}
		}
		return true;
	}
	return false;
}



//For the Mills system corners there are 8 possibilities
//
//(1)
//        Variable distance(underrun)
//               | 
//               v  
//        _____  ____________________________ 
//       |     || |                          |
//       |   <LAP>|                          |
//       |     ||_|__________________________|
//       |     ||_|__________________________|
//       |     |
//       |_____|_                   x<-------> (Overrun)
//       |     | |             2nd Mouse Pt
//       |     | |
//
//
//
//(2)
//Variable distance(overrun)
//      |         
//      v
//      ____________________________________ 
//     | |          |                       |
//     | |  ^       |                       |
//     |_|__L_______|_______________________| 
//        __A___    |_______________________|
//       |  P  |
//       |  v  |                   x<-------> (Overrun)
//       |     |              2nd Mouse Pt
//       |     | 
//
//
//(3)
//        Variable distance(underrun)
//               | 
//               v  
//        _____  ___________________ 
//       |     || |                 |
//       |   <LAP>|                 |
//       |     ||_|_________________|
//       |     ||_|_________________|
//       |     |
//       |_____|_                   x (Underrun)
//       |     | |             2nd Mouse Pt
//       |     | |
//
//
//
//(4)
//Variable distance(underrun)
//      |         
//      v
//      ___________________________ 
//     | |          |              |
//     | |  ^       |              |
//     |_|__L_______|______________|
//        __A__     |______________|
//       |  P  |
//       |  v  |                   x (Underrun)
//       |     |              2nd Mouse Pt
//       |     | 
//
//
//(5)
//       Fixed distance    Variable distance(underrun)
//             |                  |
//             v                  v
//        _______________________ _ 
//       |     |                 | |
//       |     |                 | |
//       |     |_________________|_|
//       |     |_________________|_|
//       |     |
//       |_____|_                  x (Underrun)
//       |     | |            2nd Mouse Pt
//       |     | |
//
//
//(6)
//       Fixed distance          Variable distance(Overrun)
//             |                            |
//             v                            v
//        _________________________________ _
//       |     |                           | |
//       |     |                           | |
//       |     |___________________________|_|
//       |     |___________________________|_|
//       |     |
//       |_____|_                   x<-------> (Overrun)
//       |     | |            2nd Mouse Pt
//       |     | |            
//       |     | |
//
//
//(7)
//                    Variable distance(underrun)
//     | |     |                  |
//     | |     |                  v
//     | |     |_________________ _ 
//     | |     |                 | |
//     | |     |                 | |
//     |_|_____|_________________|_|
//     |_|_____|_________________|_|
//
//             ^
//             |                   x (Underrun)
//       Fixed distance       2nd Mouse Pt
//
//
//
//(8)
//                            Variable distance(underrun)
//     | |     |                            |
//     | |     |                            v
//     | |     |___________________________ _
//     | |     |                           | |
//     | |     |                           | |
//     |_|_____|___________________________|_|
//     |_|_____|___________________________|_|
//
//             ^
//             |                    x<-------> (Overrun)
//       Fixed distance       2nd Mouse Pt
//



double AutoBuildTools::GetSchematicStandardRadius()
{
	return GetController()->GetRadiusOfSchematicStandard();
}

void AutoBuildTools::SetRLsForBayByPlane(Bay *pBay, Point3D ptRLPlane[3] )
{
	int			iCnr;
	double		daRLs[4];
	Point3D		ptStds[4];
	Matrix3D	BayTrans;

	BayTrans = pBay->GetRunPointer()->GetSchematicTransform()*pBay->GetSchematicTransform();
	for( iCnr=CNR_NORTH_EAST; iCnr<=CNR_SOUTH_WEST; iCnr++ )
	{
		//Find the positions of each standard
		ptStds[iCnr] = pBay->GetStandardPosition( (CornerOfBayEnum)iCnr );
		//We are not interested in the current RL!
		ptStds[iCnr].z = 0.00;
		ptStds[iCnr].transformBy( BayTrans );
		//Lets find the RL's at each of these positions
		if( !FindZOfXYPointOnPlane( ptStds[iCnr], ptRLPlane) )
		{
			assert( false );
			ptStds[iCnr].z = 0.00;
		}
		daRLs[iCnr] = ptStds[iCnr].z;
	}
	//Adjust the RL's for each bay!
	pBay->SetAndAdjustRLs( daRLs, false );
}

double AutoBuildTools::GetStarSeparation()
{
	return GetABBay()->GetStarSeparation();
}

SystemEnum AutoBuildTools::GetSystem()
{
	return GetABBay()->GetSystem();
}
