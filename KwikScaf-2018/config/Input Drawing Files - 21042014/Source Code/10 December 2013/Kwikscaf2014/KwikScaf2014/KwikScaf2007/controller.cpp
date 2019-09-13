// controller.cpp: implementation of the Controller class.
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//						CONTROLLER CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 20/12/00 3:19p $
//	Version			: $Revision: 309 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This is the controlling class for the entire ARX.
///////////////////////////////////////////////////////////////////////////////

//includes
#include "stdafx.h"
#include "controller.h"
#include <dbents.h>
#include <dbmain.h>
#include "ComponentDetails.h"
#include <dbgroup.h>
#include <dbray.h>
#include <dbxline.h>
#include "BOMExtra.h"
#include "JobDetailsDialog.h"
#include "BayDetailsDialog.h"
#include "Main.h"
#include "RunPropDialog.h"
#include "BOMSelectionDialog.h"
#include "SetStagesDlg.h"
#include "ChangeRLDlg.h"
#include "geassign.h"
#include "EditStandardsDlg.h"
#include "ShowComponentTypeDialog.h"
#include "dbsol3d.h"
#include "adsdef.h"
#include "PerRollDialog.h"

EntityDebugger gED;

ads_point gOffset;

int giDebugNumber;
int	giAutobuildNumber;
extern Controller *gpController;
int	giaLoopCounter[10];
CString	gsaLoopCounter[10];

const	Point3D DEFAULT_3D_POSITION( 0.00, 0.00, 0.00 );
const	Point3D DEFAULT_MATRIX_POSITION( 0.00, 10000.00, 0.00 );
const	CString VIEW_EDIT_STANDARD_CURRENT = _T("KwikScaf Edit Standards View");
//#define	SHOW_3D_XHAIR
#define SHOW_PROGRESS_DELETE_RUN

const	int IMPOSSIBLE_NE = 1;
const	int IMPOSSIBLE_SE = 2;
const	int IMPOSSIBLE_SW = 4;
const	int IMPOSSIBLE_NW = 8;
const	double SCHEMATIC_STANDARD_DELAULT_RADIUS = 100.00;

//extern HINSTANCE _hdllInstance;

//#define	USE_BAYS_ADDHANDRAIL

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
Controller::Controller()
{
	SetInsertingBlock(false);
	gpController = this;
	m_eSystem = S_KWIKSTAGE;
#ifdef _DEBUG
	m_clComponentDebugger.RemoveAll();
#endif	//ifdef _DEBUG

	m_iGroupNumber = 0;
	m_raRuns.RemoveAll();
	m_ltCompDetails.RemoveAll();
	m_ltStockList.RemoveAll();

	m_pMatrix = NULL;
	m_pCrosshair = NULL;			//The 3D View Origin crosshair entity
	m_pCrosshairReactor = NULL;	//A rector for the crosshair

	m_bActualEntitiesLoaded = false;

	m_daLevels.RemoveAll();
	m_Lapboards.DeleteAllLapboards();
	m_3DCrosshairTransform.setToIdentity();
	m_MatrixCrosshairTransform.setToIdentity();
	m_CutThroughCrosshairTransform.setToIdentity();
	m_BOMExtraCompList.DeleteAll();
	GetVisualComponents()->SetController(this);
	GetVisualComponents()->DeleteAll();
	SetDirtyFlag( DF_CLEAN );
	m_bCrosshairCreated = false;

	m_pt3DPosition = DEFAULT_3D_POSITION;
	m_ptMatrixPosition = DEFAULT_MATRIX_POSITION;

	SetGlobalsToDefault();

	m_StageListArray.SetController(this);
	//CreateLayers();
	SetMovedComponent(NULL);
//	m_JobDescripInfo.LoadSummInfo(); // load job description from NOD info for info dialog
	m_bIgnorMovement = false;
	m_bIgnorErase = false;
	giDebugNumber = 0;
	giAutobuildNumber = 0;
	SetArcLapboardSpan( COMPONENT_LENGTH_1500 );

	m_bShowLongLevelLength	= true;
	m_bShowFirstScale	= false;
	m_bShowScaleLines	= false;
	ClearAllEntityTypes();

	m_iaInvisibleMatrixElement.SetUniqueElementsOnly(true);
	ShowAllMatrixElements();
	m_bShowingEditStandards = false;
	SetDestructingController( false );

	m_iaComponentsToHide.SetUniqueElementsOnly( true );
	ShowAllComponentTypes();
	SetColourByStageLevel(false);
	SetDisplaySalePrice(false);
	SetUseActualComponents( UAC_NEITHER );
	SetRadiusOfSchematicStandard( SCHEMATIC_STANDARD_DELAULT_RADIUS );

	SetShowTypicalMatrixSectionsOnly( false );
	m_iaMatrixBaysSelected.RemoveAll();
	m_ia3DBaysSelected.RemoveAll();
	SetUsePutLogClipsInLadder(false);
	SetIsOpeningDocument(false);
	ResetLabelCount();
	SetUse1500Soleboards(false);
	SetShowMatrixLabelsOnSchematic(true);
	SetUse0700TransomForLedger( true );
	SetUse1200TransomForLedger( true );
	SetShowStandardLengths(true);
	SetBOMSummaryToShowBaySizes(true);
	SetBOMSummaryToShowBays(true);

	m_eLastInsertComponentMaterial	= MT_STEEL;
	m_eLastInsertComponentType		= CT_TRANSOM;
	m_dLastInsertComponentLength	= COMPONENT_LENGTH_2400;
	m_eLastInsertComponentSystem	= GetSystem();
	m_sLastInsertComponentPartNumber.Empty();

	m_ptLastComponentRotatePoint[0].set( LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER );
	m_ptLastComponentRotatePoint[1].set( LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER );
	m_dLastComponentRotationAngle = LARGE_NEGATIVE_NUMBER;

	SetConvertToSystemOnNextOpen(false);
	gOffset[X] = 0.00;
	gOffset[Y] = 0.00;
	gOffset[Z] = 0.00;
}	//Controller()

///////////////////////////////////////////////////////////////////////////

Controller::~Controller()
{
	try
	{
		int iRun, iBay;
		Run	*pRun;
		Bay	*pBay;

		SetDestructingController( true );

		//mark all bays for delete
		for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
		{
			pRun = GetRun(iRun);
			for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
			{
				pBay = pRun->GetBay(iBay);
				pBay->SetAllowDraw(false);
				pBay->SetDirtyFlag( DF_DELETE );
			}
		}
		CleanUp();

		DestroyGlobals();

		Entity Ent;
		Acad::ErrorStatus	es;
		es = Ent.DeleteAllDatabases( false, true );
		assert(es==Acad::eOk);

		//Clean up the Entity Debugger
		gED.PrintRemaining();
		gED.RemoveAll();

	#ifdef _DEBUG
		if( m_clComponentDebugger.GetSize()>0 )
		{
			CString sMsg, sTemp;
			sMsg = _T("The following components were not deleted ");
			for( int i=0; i<m_clComponentDebugger.GetSize(); i++ )
			{
				sMsg+= GetComponentDescStr( m_clComponentDebugger.GetAt(i)->GetType() );
				sMsg+= _T(", ");
			}
			MessageBox( NULL, sMsg, _T("Components Not Deleted Error"), MB_OK );
		}
	#endif	//ifdef _DEBUG

		SetDestructingController( false );
	}
	catch(...)
	{
		//something when haywire during shutdown
		assert( false );
	}
	StoreColoursInRegistry();
}	//~Controller()

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//Runs
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//CreateNewRun
//Creates the RunTemplate Object
int Controller::CreateNewRun( )
{
	Run		*pRun;
	pRun = new Run;
	return AddRun(pRun);
}	//CreateNe wRun( )

///////////////////////////////////////////////////////////////////////////////
//DeleteRun
//Destroys a run
bool Controller::DeleteRuns( Run *pRun, int iCount/*=1*/, bool bShowProgress/*=true*/ )
{
	return DeleteRuns( GetRunID(pRun), iCount, bShowProgress );
}	//DeleteRuns( Run *pRun, int iCount/*=1*/ )

///////////////////////////////////////////////////////////////////////////////
//DeleteRun
bool Controller::DeleteRuns( int iRunID, int iCount/*=1*/, bool bShowProgress/*=true*/ )
{
	Run	*pRun;
	int i, iSize;

	//test dimensions
	iSize = GetNumberOfRuns();
	if( (iRunID<0) || ((iRunID+iCount)>iSize) || (iSize<=0) )		
		return false;

	//Progress Bar init
#ifdef SHOW_PROGRESS_DELETE_RUN
	int iPos, iFull;
	if( bShowProgress )
	{
		iFull = 0;
		for( i=0; i<GetNumberOfRuns(); i++ )
		{
			pRun  = m_raRuns.GetAt( i );
			iFull+= pRun->GetNumberOfBays();
		}
		iFull+= GetNumberOfLapboards();
		acedSetStatusBarProgressMeter( _T("Shutting down"), 0, iFull );
		iPos = 0;
	}
#endif	//SHOW_PROGRESS_DELETE_RUN

	//delete runs
	for( i=iRunID; i<iRunID+iCount; i++ )
	{
		pRun = m_raRuns.GetAt( i );
		assert( pRun!=NULL );
#ifdef SHOW_PROGRESS_DELETE_RUN
		if( bShowProgress )
		{
			iPos+= pRun->GetNumberOfBays();
			acedSetStatusBarProgressMeterPos( iPos );
		}
#endif	//SHOW_PROGRESS_DELETE_RUN
		delete pRun;
		pRun = NULL;
	}
	m_raRuns.RemoveAt( iRunID, iCount );

#ifdef SHOW_PROGRESS_DELETE_RUN
	if( bShowProgress )
	{
		acedSetStatusBarProgressMeterPos(iFull);
		acedRestoreStatusBar();
	}
#endif	//SHOW_PROGRESS_DELETE_RUN

	//renumber the remaining runs
	iSize = GetNumberOfRuns();
	assert( iRunID<=iSize );
	for( i=iRunID; i<iSize; i++ )
	{
		pRun = m_raRuns.GetAt( i );
		pRun->SetRunID( i );
	}

	if( iSize<=0 )
	{
		//we have nothing left, so we can't have a matrix or 3D
		DeleteAllLapboards();
		Delete3DView();
		DeleteMatrix();
	}
	
	return true;
}	//DeleteRuns(...)

/*
///////////////////////////////////////////////////////////////////////////////
//DeleteLapboard
bool Controller::DeleteLapboard( int iID, int iCount/*=1*/ /* )
{
	int			i, iSize;
	LapboardBay	*pLapBay;

	//test dimensions
	iSize = GetNumberOfLapboards();
	if( (iID<0) || ((iID+iCount)>iSize) || (iSize<=0) )		
		return false;

	//delete runs
	for( i=iID; i<iID+iCount; i++ )
	{
		pLapBay = GetLapboard( i );
		assert( pLapBay!=NULL );
		delete pLapBay;
	}
	m_Lapboards.RemoveAt( iID, iCount );

	//re-id the remaining runs
	iSize = GetNumberOfLapboards();
	assert( iID<=iSize );
	for( i=iID; i<iSize; i++ )
	{
		pLapBay = GetLapboard( i );
		pLapBay->SetID( i );
	}
	
	return true;
}	//DeleteLapboard( int iID, int iCount/*=1*/ /* )

*/

///////////////////////////////////////////////////////////////////////////////
//JoinRuns
//This function joins two runs.  The ends that are about to be joined need to
//	have null pointers in the appropriate direction.  This function should take
//	either a pointer to a run or a RunID (overridden function).
Run *Controller::JoinRuns( Run *pRunFirst, Run *pRunSecond,
						  JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	assert( FALSE );	//JSB todo 990729 - This function has not been tested
	if( !pRunFirst->JoinRun( pRunSecond, JoinType ) )
		return NULL;
	return pRunFirst;
}	//JoinRuns( Run *pRunFirst, Run *pRunSecond,


///////////////////////////////////////////////////////////////////////////////
//JoinRuns
Run *Controller::JoinRuns( int RunIDFirst, int RunIDSecond, JoinTypeEnum
							JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	assert( FALSE );	//JSB todo 990729 - This function has not been tested
	Run *pRunFirst, *pRunSecond;
	pRunFirst	= GetRun( RunIDFirst );
	pRunSecond	= GetRun( RunIDSecond );
	return JoinRuns( pRunFirst, pRunSecond, JoinType );
}	//JoinRuns( int RunIDFirst, int RunIDSecond, JoinTypeEnum

///////////////////////////////////////////////////////////////////
//GetRunID
//finds the position of the run in the run list
int	Controller::GetRunID( Run *pRun ) const
{
	int	i, iSize;

	iSize = GetNumberOfRuns();

	if( iSize<=0 )
		return ID_NO_IDS;

	for( i=0; i<iSize; i++ )
	{
		if( m_raRuns.GetAt(i)==pRun )
			return i;
	}
	return ID_NONE_MATCHING;
}	//GetRunID( Run *pRun ) const


///////////////////////////////////////////////////////////////////
//GetRun
//Retrieves the pointer to the run
Run	*Controller::GetRun( int iRunID ) const
{
	if( !IsRunIDValid(iRunID) )
		return NULL;

	return m_raRuns.GetAt(iRunID);
}	//GetRun( int iRunID ) const


///////////////////////////////////////////////////////////////////
//GetNumberOfRuns
//retrieves the number of runs owned by the controller
int Controller::GetNumberOfRuns() const
{
	return m_raRuns.GetSize();
}	//GetNumberOfRuns() const


///////////////////////////////////////////////////////////////////////////////
//Bays
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//JoinBays
//This function joins two bays, it is envisioned that this function would be
//	called by the JoinRuns command above to complete it's task.  This function
//	belongs here, and not in the run class, nor bay class since it is likely
//	that the two bays that are to be joined are in two separate Runs.
bool Controller::JoinBays( Bay *pBayFirst, Bay *pBaySecond,
						  JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	assert( FALSE );	//JSB todo 990729 - This function has not been tested
	Run	*pRunFirst, *pRunSecond;

	if( (pBayFirst==NULL) || (pBaySecond==NULL) )
		return false;

	pRunFirst = GetRunWhichOwnsBay(pBayFirst);
	pRunSecond = GetRunWhichOwnsBay(pBaySecond);
	if( (pRunFirst==NULL) || (pRunSecond==NULL) )
		return false;

	return ( JoinRuns( pRunFirst, pRunSecond, JoinType )!=NULL );
}	//JoinBays( Bay *pBayFirst, Bay *pBaySecond,


///////////////////////////////////////////////////////////////////////////////
//RedrawSchematic
//Analysis the bay and redraws from the components
//useful for when they edit lifts within a bay, and they have made a change
//	which should be displayed in the Plan view such as a handrail
void Controller::RedrawSchematic( bool bOnlyUpdateVisited/*=false*/ )
{
	int i;
	Run	*pRun;

#ifdef SHOW_AUTOBUILD_BAY
	GetABTools()->UpdateSchematicView( bOnlyUpdateVisited );
#endif	//#ifdef SHOW_AUTOBUILD_BAY
	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun = GetRun( i );
		pRun->UpdateSchematicView( bOnlyUpdateVisited );
	}
}	//RedrawSchematic( bool bOnlyUpdateVisited/*=false*/ )


///////////////////////////////////////////////////////////////////////////////
//Lapboards
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//CreateNewLapboard
//	Creates a new lapboard object and attaches it to the controller
LapboardBay *Controller::CreateNewLapboard()
{
	LapboardBay *pLap;
	pLap = m_Lapboards.CreateNewLapboard();
	pLap->SetBayType( BAY_TYPE_LAPBOARD );
	pLap->SetController( this );
	return pLap;
}	//CreateNewLapboard()


///////////////////////////////////////////////////////////////////////////////
//AddNewLapboard
//Add the new lapboard to the list of lapboards
int Controller::AddNewLapboard( LapboardBay *pLapboard )
{
	int iID;
	
	iID = m_Lapboards.AddNewLapboard( pLapboard );
	pLapboard->SetController( this );

	return iID;
}	//AddNewLapboard( LapboardBay *pLapboard )


///////////////////////////////////////////////////////////////////////////////
//AddNewLapboardBayToBay
//Creates a new LapboardBay, and attaches to the correct Run.  B4 this can occur
//	we need to test if the corners are already used, and that the forward/
//	backward pointer is not being used for the bay
LapboardBay *Controller::AddNewLapboardBayToBay( Bay *pBayFirst, Bay *pBaySecond )
{
	LapboardBay *pLapboard;

	pLapboard = CreateNewLapboard();
	pLapboard->SetBayWest( pBayFirst );
	pLapboard->SetBayEast( pBaySecond );

	return pLapboard;
}	//AddNewLapboardBayToBay( Bay *pBayFirst, Bay *pBaySecond )


///////////////////////////////////////////////////////////////////////////////
//DeleteAllLapboards
//deletes all existing lapboards
void Controller::DeleteAllLapboards()
{
	m_Lapboards.DeleteAllLapboards();
}	//DeleteAllLapboards()


///////////////////////////////////////////////////////////////////////////////
//DeleteLapboard
//deletes an existing lapboards
bool Controller::DeleteLapboard( LapboardBay *pLapboard, int iCount/*=1*/ )
{
	return DeleteLapboard( pLapboard->GetID(), iCount );
}	//DeleteLapboard( LapboardBay *pLapboard, int iCount/*=1*/ )


///////////////////////////////////////////////////////////////////////////////
//DeleteLapboard
//
bool Controller::DeleteLapboard( int iLapboardID, int iCount/*=1*/ )
{
	return m_Lapboards.DeleteLapboard( iLapboardID, iCount );
}	//DeleteLapboard( int iLapboardID, int iCount/*=1*/ )


///////////////////////////////////////////////////////////////////
//GetNumberOfLapboards
//retrieves the number of lapboards owned by the controller
int Controller::GetNumberOfLapboards() const
{
	return m_Lapboards.GetNumberOfLapboards();
}	//GetNumberOfLapboards() const


///////////////////////////////////////////////////////////////////
//ReIDAllLapboards
//retrieves the number of lapboards owned by the controller
void Controller::ReIDAllLapboards( int iStartID/*=0*/ )
{
	m_Lapboards.ReIDAllLapboards( iStartID );
}	//ReIDAllLapboards( int iStartID/*=0*/ )


///////////////////////////////////////////////////////////////////
LapboardBay * Controller::GetLapboard(int iLapboardID)
{
	return m_Lapboards.GetLapboard( iLapboardID );
}	//GetLapboard(int iLapboardID)


///////////////////////////////////////////////////////////////////////////////
//Surface Areas
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//GetSurfaceArea
//Queries the Runs and the Lapboards to get the Surface Area required.
double Controller::GetSurfaceArea() const
{
	assert( FALSE );	//JSB todo 990729 - This function has not been tested
	Run		*pRun;
	int		i;
	double	dArea;

	dArea = 0.00;

	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun  = m_raRuns.GetAt( i );
		dArea+= pRun->GetSurfaceArea();
	}
	
	return dArea;
}	//GetSurfaceArea() const


///////////////////////////////////////////////////////////////////////////////
//Matrix
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//CreateMatrix
//Create a new matrix object
bool Controller::CreateMatrix( bool bUseLast/*=false*/ )
{
	int			j, iRunID, iBayID, iRet;
	Run			*pRun;
	Bay			*pBay;
	bool		bLoop;
	TCHAR		keywrd[255];
	BayList		Bays;
	CString		sKeyWrd, sMsg;

	/////////////////////////////////////////////////////
	//Delete it if it exists already
	DeleteMatrix();

	/////////////////////////////////////////////////////
	m_pMatrix = new Matrix;
	GetMatrix()->SetController(this);

	Point3D point;
	point = DEFAULT_MATRIX_POSITION;

	bool bShowTypicalStore;
	if( EditingStandards() )
	{
		//We must show all sections for all bays!
		//	So store the current settings
		assert( bUseLast );
		bShowTypicalStore = ShowTypicalMatrixSectionsOnly();
		SetShowTypicalMatrixSectionsOnly( false );
		for( j=0; j<m_iaMatrixBaysSelected.GetSize(); j++ )
		{
			pBay = GetBayFromBayNumber(m_iaMatrixBaysSelected.GetAt(j));
			if( pBay!=NULL )
				Bays.Add(pBay);
		}
		m_iaMatrixBaysSelected.RemoveAll();
	}

	do
	{
		bLoop = true;
		if( bUseLast )
		{
			point = m_ptMatrixPosition;
			bLoop = false;
		}
		else
		{
			if( acedInitGet( RSG_OTHER, NULL )==RTERROR )
			{
				assert( false );
			}

			sMsg = _T("\nChoose position for the origin of the Matrix ");

			if( m_iaMatrixBaysSelected.GetSize()>0 )
				sMsg+= _T("<selected bays,");
			else
				sMsg+= _T("<all bays,");

			if( ShowTypicalMatrixSectionsOnly() )
				sMsg+= _T("typical> ");
			else
				sMsg+= _T("all sections> ");

			sMsg+= _T("[Last/");

			if( m_iaMatrixBaysSelected.GetSize()>0 )
				sMsg+= _T("reSelect bays/all Bays/");
			else
				sMsg+= _T("Select bays/");

			if( ShowTypicalMatrixSectionsOnly() )
        sMsg+= _T("All sections]: ");
			else
        sMsg+= _T("Typical sections only]: ");

			iRet = GetPoint( sMsg, point );
			switch( iRet )
			{
			default:
				return false;

			case(RTNONE):
				//Right click, just use the default position
				bLoop = false;
				break;

			case(RTNORM):
				//Fine just use the user point, but store it first
				m_ptMatrixPosition = point;
				bLoop = false;
				break;

			case(RTKWORD):
				acedGetInput( keywrd );
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();
				if( sKeyWrd[0]==_T('L') )
				{
					//Want to use last point
					point = m_ptMatrixPosition;
					bLoop = false;
				}
				else if( sKeyWrd[0]==_T('S') )
				{
					//Select Bays
					m_iaMatrixBaysSelected.RemoveAll();
					if( !SelectBays( Bays, true ) )
						continue;
					CheckForStairLadder( &Bays );
					for( j=0; j<Bays.GetSize(); j++ )
					{
						m_iaMatrixBaysSelected.Add(Bays.GetAt(j)->GetBayNumber());
					}
				}
				else if( sKeyWrd[0]==_T('B') )
				{
					m_iaMatrixBaysSelected.RemoveAll();
				}
				else if( sKeyWrd[0]==_T('T') )
				{
					//Typical sections only
					SetShowTypicalMatrixSectionsOnly( true );
				}
				else if( sKeyWrd[0]==_T('A') )
				{
					//All sections
					SetShowTypicalMatrixSectionsOnly( false );
				}
				else
				{
					MessageBeep(MB_ICONASTERISK);
				}
				break;
			}
		}
	}while( bLoop );

	Vector3D	Vect;
	Matrix3D	Transform;
	Vect.set( point.x, point.y, point.z );
	Transform.setTranslation( Vect );
	SetMatrixCrosshairTransform( Transform );

	///////////////////////////////////////////
	//Progress Bar init
	int i, iPos, iFull;
	iFull = 0;
	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun  = m_raRuns.GetAt( i );
		iFull+= pRun->GetNumberOfBays();
	}
	acedSetStatusBarProgressMeter( _T("Calculating Matrix View"), 0, iFull );
	iPos = 0;

	/////////////////////////////////////////////////////
	//create a matrix element for each bay element!
	for( iRunID=0; iRunID<GetNumberOfRuns(); iRunID++ )
	{
		pRun = GetRun(iRunID);
		for( iBayID=0; iBayID<pRun->GetNumberOfBays(); iBayID++ )
		{
			pBay = pRun->GetBay(iBayID);

			if( m_iaMatrixBaysSelected.GetSize()<=0 ||
				(m_iaMatrixBaysSelected.GetSize()>0 && m_iaMatrixBaysSelected.IsElementOf( pBay->GetBayNumber() )) )
			{
				if( !(ShowTypicalMatrixSectionsOnly() && pBay->GetForward()==NULL) )
					pBay->CreateMatrix( false );
				if( !ShowTypicalMatrixSectionsOnly() && pBay->GetBackward()==NULL )
					pBay->CreateMatrix( true );
			}

			iPos++;
			acedSetStatusBarProgressMeterPos( iPos );
		}
	}
	acedSetStatusBarProgressMeterPos(iFull);
	acedRestoreStatusBar();

	RedrawMatrix();
	RenumberAllBays();

	if( EditingStandards() )
	{
		//Restore the settings
		SetShowTypicalMatrixSectionsOnly(bShowTypicalStore);
		for( int j=0; j<Bays.GetSize(); j++ )
		{
			m_iaMatrixBaysSelected.Add(Bays.GetAt(j)->GetBayNumber());
		}
	}

	MessageBeep(MB_ICONASTERISK);
	return true;
}	//CreateMatrix()



void Controller::CreateCutThrough()
{
	int				iRet;
	Bay				*pBay;
	bool			bLoop;
	TCHAR			keywrd[255];
	Point3D			ptPoint;
	Matrix			CutThroughMatrix;
	BayList			Bays;
	CString			sKeyWrd, sMsg, sTemp;
	intArray		iaAvailableSides;
	SideOfBayEnum	eSide, eSideChosen;
	const int		ARROW_POINTS = 5;

	/////////////////////////////////////////////////////
	CutThroughMatrix.SetController(this);
	CutThroughMatrix.SetCutThrough(true);

	Point3D point;
	point = DEFAULT_MATRIX_POSITION;

  acutPrintf(_T("Please select which bay you would like to create a cut through for: "));
	Bays.RemoveAll();
	if( !SelectBays( Bays, true ) )
		return;

	//Does this work for Ladder/Stair/Lap bays?
	//JSB todo 20000729 - this needs to be tested!
	//CheckForStairLadder( &Bays );

	if( Bays.GetSize()>0 )
	{
		//We only want one bay
		while( Bays.GetSize()>1 )
		{
			Bays.RemoveAt(1);
		}


		bLoop = true;
		eSide = EAST;

		while( bLoop )
		{
			sMsg.Format( _T("\nWhich Side of Bay %i ["), Bays[0]->GetBayNumber() );
			sMsg+= GetSideOfBayDescStr( eSide==EAST? WEST: EAST );
			sMsg+= _T("] <");
			sMsg+= GetSideOfBayDescStr( eSide );
      sMsg+= _T(">: ");

			acedInitGet( RSG_OTHER, _T("East West"));
			iRet = GetPoint3D( sMsg, ptPoint );
			switch( iRet )
			{
			case( RTERROR ):			//fallthrough (-5001) Some other error
			default:
				//This should not have occured
				RefreshScreen();
				MessageBeep(MB_ICONERROR);
				return;	//Unknown error

			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				RefreshScreen();
				MessageBeep(MB_ICONEXCLAMATION);
				return;

			case( RTNONE ):				//fallthrough (5000) No result
				bLoop = false;
				break;
			
			case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
				acedGetInput( keywrd );
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();

				if( _istdigit( keywrd[0] ) )
				{
					MessageBeep(MB_ICONEXCLAMATION);
					return;
				}
				else
				{
					if( sKeyWrd[0]==_T('E') )
						eSide = EAST;
					else if( sKeyWrd[0]==_T('W') )
						eSide = WEST;
					else
						//invalid input
						MessageBeep(MB_ICONERROR);
					break;
				}
				break;

			case( RTNORM ):				//(5100) Request succeeded
				iaAvailableSides.RemoveAll();
				iaAvailableSides.Add( BP_E_MILLS );
				iaAvailableSides.Add( BP_W_MILLS );
				GetSideOfBayFromPoint( &Bays, ptPoint, iaAvailableSides, eSideChosen );
				if( eSideChosen==EAST || eSideChosen==WEST )
					eSide = eSideChosen;
				break;
			}
		}

		//We should now have a valid side!
		assert( eSide==EAST || eSide==WEST );

		sMsg = _T("\nChoose the point where you would like the cut through placed");

		iRet = GetPoint( sMsg, point );
		switch( iRet )
		{
		default:
		case(RTNONE):
		case(RTNORM):				//(5100) Request succeeded
			//Right click, just use the default position
			break;
		case(RTKWORD):
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}

		Vector3D	Vect;
		Matrix3D	Transform;
		Vect.set( point.x, point.y, point.z );
		Transform.setTranslation( Vect );
		SetCutThroughCrosshairTransform( Transform );

		/////////////////////////////////////////////////////
		//Firstly we need to find the Bay farthest to the south
		//	so we can find the position of the standards
		bLoop = true;
		pBay = Bays[0];
		while( bLoop )
		{
			if( pBay->GetInner()!=NULL )
			{
				pBay = pBay->GetInner();
			}
			else
			{
				if( eSide==EAST && pBay->GetForward()!=NULL && 
						pBay->GetForward()->GetInner()!=NULL )
				{
					//This is designed to handle this situation
					//		---------- ----------
					//     |          |          |
					//     |   this---|----      |
					//     |          |    |     |
					//		---------- ----------
					//                |    |     |
					//         Void   |    V     |
					//                |          |
					//		---------- ----------
					//     |          |          |
					//     |          |          |
					//     |          |          |
					//		---------- ----------
					pBay = pBay->GetForward()->GetInner();
					assert( pBay->GetBackward()==NULL );
					eSide = WEST;
				}
				else if( eSide==WEST && pBay->GetBackward()!=NULL && 
							pBay->GetBackward()->GetInner()!=NULL )
				{
					//This is designed to handle this situation
					//		---------- ----------
					//     |          |          |
					//     |      ----|---this   |
					//     |     |    |          |
					//		---------- ----------
					//     |     |    |
					//     |     V    |   Void
					//     |          |
					//		---------- ----------
					//     |          |          |
					//     |          |          |
					//     |          |          |
					//		---------- ----------
					pBay = pBay->GetBackward()->GetInner();
					assert( pBay->GetForward()==NULL );
					eSide = EAST;
				}
				else
				{
					bLoop = false;
					break;
				}
			}
		}
		//pBay should now be at the Southern most bay for the cut through
		int				i;
		Point3D			ptsCutThroughArrowSouth[ARROW_POINTS], ptsCutThroughArrowNorth[ARROW_POINTS];
		Matrix3D		BayTrans;
		const double	dArrowLength	= 700.00;
		const double	dArrowYfromStd	= 950.00;
		const double	dArrowXfromStd	= 400.00;

		//Position the Cutthrough arrow to start 400 west of the standard(at worst
		//	this should be about 1/2 way through a 700 bay) and -900 to the south
		//	of the standard(this should be longer than the longest hopup)
		//
		//           |-400-|
		//           |-----700----|
		//   ---      ------------>Arrow
		//    |      |
		//    |      |
		//    |      |
		//   900     |
		//    |      |
		//    |      |
		//    |      |
		//   ---  ---|-----O---------------O---------------O
		//           |     |               |               | 
		//           |     |               |               | 
		//           |     |     BAY       |     BAY       |
		//           |-400-|               |               |
		//           |     |               |               |
		//           |     |               |               |
		//        ---|-----O---------------O---------------O
		//           | /   | /             | /             | /
		//           / /   / /             / /             / /
		//         / /   / /     BAYS    / /     BAYS    / /
		//         / |   / |             / |             / |
		//        ---|-----O---------------O---------------O
		//           |     |               |               |
		//           |     |               |               |
		//           |     |     BAY       |     BAY       |
		//           |-400-|               |               |
		//           |     |               |               |
		//           |     |               |               |
		//   ---  ---|-----O---------------O---------------O
		//    |      |
		//    |      |
		//    |      |
		//   950     |
		//    |      |
		//    |      |
		//    |      |
		//   ---      ------------>Arrow
		//           |-400-|
		//           |-----700----|
		//
		//
		//
		ptsCutThroughArrowSouth[0].set( 0.00-dArrowXfromStd, 0.00-dArrowYfromStd, 0.00 );
		if( eSide==EAST )
		{
			//We are looking at the east side so we need to create the cut
			ptsCutThroughArrowSouth[0].x+= pBay->GetBayLengthActual();
		}
		ptsCutThroughArrowSouth[1]	 = ptsCutThroughArrowSouth[0];
		ptsCutThroughArrowSouth[1].x+= dArrowLength;
		ptsCutThroughArrowSouth[2]	 = ptsCutThroughArrowSouth[1];
		ptsCutThroughArrowSouth[2].x-= dArrowLength/4.00;
		ptsCutThroughArrowSouth[2].y+= dArrowLength/6.00;
		ptsCutThroughArrowSouth[3]	 = ptsCutThroughArrowSouth[1];
		ptsCutThroughArrowSouth[3].x-= dArrowLength/4.00;
		ptsCutThroughArrowSouth[3].y-= dArrowLength/6.00;
		ptsCutThroughArrowSouth[4]  =  ptsCutThroughArrowSouth[1];
		ptsCutThroughArrowSouth[4].x+= dArrowLength/4.00;
		ptsCutThroughArrowSouth[4].y-= dArrowLength/6.00;

		//The southern arrow has to match the position of the Bay
		BayTrans = pBay->GetRunPointer()->GetSchematicTransform()*pBay->GetSchematicTransform();
		for( i=0; i<ARROW_POINTS; i++ )
		{
			//Store the arrow position for use on the Northern side
			ptsCutThroughArrowNorth[i] = ptsCutThroughArrowSouth[i];
			ptsCutThroughArrowSouth[i].transformBy( BayTrans );
		}

		/////////////////////////////////////////////////////
		//Find the Bay farthest to the north as a starting point
		bLoop = true;
		pBay = Bays[0];
		while( bLoop )
		{
			if( pBay->GetOuter()!=NULL )
			{
				pBay = pBay->GetOuter();
			}
			else
			{
				if( eSide==EAST && pBay->GetForward()!=NULL && 
						pBay->GetForward()->GetOuter()!=NULL )
				{
					//This is designed to handle this situation
					//		---------- ----------
					//     |          |          |
					//     |          |          |
					//     |          |          |
					//		---------- ----------
					//                |          |
					//         Void   |    ^     |
					//                |    |     |
					//		---------- ----------
					//     |          |    |     |
					//     |   this---|----      |
					//     |          |          |
					//		---------- ----------
					pBay = pBay->GetForward()->GetOuter();
					assert( pBay->GetBackward()==NULL );
					eSide = WEST;
				}
				else if( eSide==WEST && pBay->GetBackward()!=NULL && 
							pBay->GetBackward()->GetOuter()!=NULL )
				{
					//This is designed to handle this situation
					//		---------- ----------
					//     |          |          |
					//     |          |          |
					//     |          |          |
					//		---------- ----------
					//     |          |
					//     |     ^    |  Void
					//     |     |    |
					//		---------- ----------
					//     |     |    |          |
					//     |      ----|--this    |
					//     |          |          |
					//		---------- ----------
					pBay = pBay->GetBackward()->GetOuter();
					assert( pBay->GetForward()==NULL );
					eSide = EAST;
				}
				else
				{
					bLoop = false;
					break;
				}
			}
		}
		//The Northern arrow has to match the position of the northern Bay
		BayTrans = pBay->GetRunPointer()->GetSchematicTransform()*pBay->GetSchematicTransform();
		for( i=0; i<ARROW_POINTS; i++ )
		{
			//When talking about the northern side it has to be dArrowYfromStd ABOVE
			//	the NORTH standard not dArrowYfromStd BELOW the SOUTH standard
			ptsCutThroughArrowNorth[i].y+= dArrowYfromStd;
			ptsCutThroughArrowNorth[i].y+= pBay->GetBayWidthActual();
			ptsCutThroughArrowNorth[i].y+= dArrowYfromStd;
			ptsCutThroughArrowNorth[i].transformBy( BayTrans );
		}

		Entity		*pCutThroughEntities[7];
	
		pCutThroughEntities[0] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowNorth[0], ptsCutThroughArrowNorth[1] ));
		pCutThroughEntities[1] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowNorth[1], ptsCutThroughArrowNorth[2] ));
		pCutThroughEntities[2] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowNorth[1], ptsCutThroughArrowNorth[3] ));
		pCutThroughEntities[3] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowNorth[0], ptsCutThroughArrowSouth[0] ));
		pCutThroughEntities[4] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowSouth[0], ptsCutThroughArrowSouth[1] ));
		pCutThroughEntities[5] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowSouth[1], ptsCutThroughArrowSouth[2] ));
		pCutThroughEntities[6] = (Entity*)((AcDbEntity*) new AcDbLine( ptsCutThroughArrowSouth[1], ptsCutThroughArrowSouth[3] ));

		int					*piGroupNumber;
		TCHAR				cGroupName[255];
		CString				sGroupName;
		AcDbObjectId		id;
		Acad::ErrorStatus	es;
		AcDbObjectIdArray	objIds;

		for( i=0; i<7; i++ )
		{
			pCutThroughEntities[i]->postToDatabase( LAYER_NAME_SECTIONS );
			es = pCutThroughEntities[i]->open();
			assert( es==Acad::eOk );
			es = pCutThroughEntities[i]->setColorIndex( (Adesk::UInt16)COLOUR_WHITE );
			assert( es==Acad::eOk );
			es = pCutThroughEntities[i]->close();
			assert( es==Acad::eOk );
			id = pCutThroughEntities[i]->objectId();
			assert( !id.isNull() );
			objIds.append( id );
		}

		CString		sCutthroughTextTop, sCutthroughTextBottom;
		Matrix3D	Trans;

		Transform.setToIdentity();
		GetNextSuggestedLabel(sCutthroughTextTop);
		sCutthroughTextBottom	= sCutthroughTextTop + _T("'");
		GetCutThroughLabelFromUser( sCutthroughTextTop, sCutthroughTextBottom, true );
		sCutthroughTextBottom	= sCutthroughTextTop + _T("'");
		GetCutThroughLabelFromUser( sCutthroughTextTop, sCutthroughTextBottom, false );
		id = CreateText( ptsCutThroughArrowNorth[4], Transform, sCutthroughTextTop,
					LAYER_NAME_SECTIONS, dArrowLength/3.00, COLOUR_WHITE, true );
		assert( !id.isNull() );
		objIds.append( id );
		id = CreateText( ptsCutThroughArrowSouth[4], Transform, sCutthroughTextBottom,
					LAYER_NAME_SECTIONS, dArrowLength/3.00, COLOUR_WHITE, true );
		assert( !id.isNull() );
		objIds.append( id );

		AcDbGroup	*pGroup;
		piGroupNumber = GetGroupNumber();
		do	//see bug 1430
		{
			(*piGroupNumber)++;
			sGroupName.Format( _T("%s%03i"), GROUP_PREFIX_CUT_THROUGH, *piGroupNumber );
			for( i=0; i<sGroupName.GetLength(); i++ )
			{
				if( i>30 ) break;
				cGroupName[i] = sGroupName[i];
			}
			cGroupName[i] = _T('\0');

			//create the schematic cutthrough group
			Entity entTemp;
			pGroup = entTemp.CreateGroup( objIds, cGroupName );
		}
		while( pGroup==NULL );	//see bug 1430

		/////////////////////////////////////////////////////////////////////
		//We should now be at the southern most bay in the cut through
		assert( pBay->GetOuter()==NULL );
		pBay->CreateCutThrough( &CutThroughMatrix, eSide );

		//Draw the Cutthrough
		CString sLabel;
		sLabel.Empty();
		if( !sCutthroughTextTop.IsEmpty() || !sCutthroughTextBottom.IsEmpty() )
		{
      sLabel = _T("SECTION: ");
			if( !sCutthroughTextTop.IsEmpty() )
				sLabel+= sCutthroughTextTop;
			if( !sCutthroughTextTop.IsEmpty() && !sCutthroughTextBottom.IsEmpty() )
				sLabel+= _T(" - ");
			if( !sCutthroughTextBottom.IsEmpty() )
				sLabel+= sCutthroughTextBottom;
		}
		CutThroughMatrix.SetCutThroughLabel( sLabel );
		CutThroughMatrix.RedrawMatrix();

		//Clean up
		//objIds.RemoveAll();

		MessageBeep(MB_ICONASTERISK);
	}
	else
	{
		MessageBeep(MB_ICONERROR);
	}
}


///////////////////////////////////////////////////////////////////////////////
//CreateMatrixElement
//Creates a new matrix element
MatrixElement *Controller::CreateMatrixElement( Matrix *pMatrix/*=NULL*/ )
{
	MatrixElement *pElement;
	pElement = new MatrixElement();
	if( pMatrix==NULL )
		GetMatrix()->Add( pElement );
	else
		pMatrix->Add( pElement );
	return pElement;
}	//CreateMatrixElement()


///////////////////////////////////////////////////////////////////////////////
//GetMatrix
//Obtains a point to the Matrix Object
Matrix *Controller::GetMatrix() const
{
	return m_pMatrix;
}	//GetMatrix() const


///////////////////////////////////////////////////////////////////////////////
//DeleteMatrix
//Destroys the matrix
bool Controller::DeleteMatrix()
{
	bool bRet;
	if( GetMatrix()!=NULL )
	{
		bRet = GetMatrix()->DeleteAllElements();

		delete m_pMatrix;
		m_pMatrix = NULL;

		UpdateAllBayText();

		return bRet;
	}
	return false;
}	//DeleteMatrix()

///////////////////////////////////////////////////////////////////////////////
//DeleteMatrixElement
//
bool Controller::DeleteMatrixElements( int iMatrixElementID, int iCount/*=1*/ )
{
	int				i, iSize;

	iSize = GetNumberOfMatrixElements();
	if( (iSize<0) || (iMatrixElementID+iCount>=iSize) || (iMatrixElementID<0) )
		return false;

	for( i=iMatrixElementID; i<=iMatrixElementID+iCount; i++ )
	{
		GetMatrix()->DeleteMatrixElement( iMatrixElementID );
	}
	return true;
}	//DeleteMatrixElements( int iMatrixElementID, int iCount/*=1*/ )

///////////////////////////////////////////////////////////////////////////////
//RedrawMatrix
//Redraw the Matrix View
bool Controller::RedrawMatrix() const
{
	if( GetNumberOfRuns()<=0 )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return false;
	}

	return GetMatrix()->RedrawMatrix();
}	//RedrawMatrix() const

///////////////////////////////////////////////////////////////////
//GetNumberOfMatrixElements
//retrieves the number of lapboards owned by the controller
int Controller::GetNumberOfMatrixElements() const
{
	return GetMatrix()->GetMatrixSize();
}	//GetNumberOfMatrixElements() const


///////////////////////////////////////////////////////////////////////////////
//Birdcaging
///////////////////////////////////////////////////////////////////////////////

//AT's 3 point fill method!

///////////////////////////////////////////////////////////////////////////////
//IsRunIDValid
//checks the bounds for a runid
bool Controller::IsRunIDValid(int iRunID) const
{
	int	iSize;

	iSize = GetNumberOfRuns();

	if( iSize<0 )
		return false;
	
	if( (iRunID<0) || (iRunID>=iSize) )
		return false;

	return true;
}	//IsRunIDValid(int iRunID) const


///////////////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//set all global variables to reasonable 'empty' values
void Controller::SetGlobalsToDefault()
{
	bool bLoaded;

	LoadColoursFromRegistry();

	DestroyGlobals();
	if( GetMatrix()!=NULL )
		GetMatrix()->DeleteAllElements();

	////////////////////////////////////////////////////////////
	//Load the stocklist file
	bLoaded = m_ltStockList.LoadList();
	SetStockFileLoaded( bLoaded );
	if( !bLoaded )
	{
		m_ltStockList.ClearFilenameInRegistry(true);
		m_ltStockList.ClearFilenameInRegistry(false);
	}

	////////////////////////////////////////////////////////////
	//Load the component details file
 	bLoaded = m_ltCompDetails.Load(this);
	SetComponentDetailsLoaded( bLoaded );
	if( !bLoaded )
		m_ltCompDetails.SetFilenameInRegistry(_T(""));

	////////////////////////////////////////////////////////////
	//The autobuild tool needs to know where the autobuild run is
	m_AutoBuildTool.SetController( this );
	m_AutoBuildTool.SetPredefinedBayLengths(this);
	m_AutoBuildTool.SetRunID( 0 );

	////////////////////////////////////////////////////////////
	//Set the 3dMatrix crosshair
	Matrix3D	Transform;
	Vector3D	Vector;
	Vector.x = DEFAULT_3D_POSITION.x;
	Vector.y = DEFAULT_3D_POSITION.y;
	Vector.z = DEFAULT_3D_POSITION.z;
	Transform.setTranslation( Vector );
	Set3DCrosshairTransform(Transform);

	Vector.x = DEFAULT_MATRIX_POSITION.x;
	Vector.y = DEFAULT_MATRIX_POSITION.y;
	Vector.z = DEFAULT_MATRIX_POSITION.z;
	Transform.setTranslation( Vector );
	SetMatrixCrosshairTransform(Transform);
	SetCutThroughCrosshairTransform(Transform);
	m_pCrosshair = NULL;
	m_pCrosshairReactor = NULL;
	SetCrosshairCreated( false );

	SetDisplayZigZagLine(true);

	//Chain & Shade per roll settings, set to default
	m_bIgnorPerRoll = false;
	ReadPerRollFromRegistry();

	SetRLCalcPossible( false );

	SetSaveSelected(false);
}	//SetGlobalsToDefault()


///////////////////////////////////////////////////////////////////////////////
//DestroyGlobals
//
void Controller::DestroyGlobals()
{
	/////////////////////////////////////////////////////
	//Delete Views
	DeleteMatrix();
	Delete3DView();

	/////////////////////////////////////////////////////
	//Components
	DeleteAllLapboards();
	m_AutoBuildTool.DeleteRun();
	DeleteRuns( 0, GetNumberOfRuns() );

	/////////////////////////////////////////////////////
	//Stage&Level
	ClearStages();
	//LevelList	m_daLevels;	//This should clean itself up
	m_daLevels.RemoveAll();

	/////////////////////////////////////////////////////
	//Component & Stock details
	m_ltCompDetails.RemoveAll();
	m_ltStockList.RemoveAll();

	/////////////////////////////////////////////////////
	//BOM extra
	m_BOMExtraCompList.DeleteAll();

	m_vcComponents.DeleteAll();

//	m_JobDescripInfo
}	//DestroyGlobals()


///////////////////////////////////////////////////////////////////////////////
//GetRunWhichOwnsBay
//find the run which own a particular bay
Run * Controller::GetRunWhichOwnsBay(Bay *pBay )
{
	if( pBay==NULL )
		return NULL;
	return pBay->GetRunPointer();
}	//GetRunWhichOwnsBay(Bay *pBay )



///////////////////////////////////////////////////////////////////////////////
//Test
//
void Controller::DoAutoBuild()
{
	CreateDefaultAutoBuildRun();

	m_AutoBuildTool.SetFirstRunInAutobuild( true );
	m_AutoBuildTool.SetAccessEnabled( true );
	m_AutoBuildTool.StartAutobuild();
	m_AutoBuildTool.SetAccessEnabled( true );
}	//DoAutoBuild()



///////////////////////////////////////////////////////////////////////////////
//ShowDialogs
//This is called by the AsdkDocReactor when an MDI switch has taken place so any
//modeless dialogs can be hidden or shown if present from their last active state
void Controller::ShowDialogs(bool bShowDialogs)
{
	m_AutoBuildTool.ShowDialogs(bShowDialogs);
}	//ShowDialogs(bool bShowDialogs)



///////////////////////////////////////////////////////////////////////////////
//GetABTools
//retives a pointer to the Autobuild tools
AutoBuildTools * Controller::GetABTools()
{
	return &m_AutoBuildTool;
}	//GetABTools()


///////////////////////////////////////////////////////////////////////////////
//CreateDefaultAutoBuildRun
//create a basic template for the autobuild to work from
void Controller::CreateDefaultAutoBuildRun()
{
	int					i, iID, iNumberOfPlanks;
	Bay					*pNewBay;
	Lift				*pLift, *pNewLift;
	double				/*dMeshLength,*/ dHeight, dStandardsHeight;
	Point3D				ptRLBegin, ptRLEnd;
	MaterialTypeEnum	eMaterial;

	eMaterial = MT_STEEL;

	//////////////////////////////////////////////////////////////////
	//Have we already created the Autobuild?
	if( m_AutoBuildTool.GetBay( 0 )!=NULL )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////
	//Initialise
	m_AutoBuildTool.CreateEmptyRun();
	m_AutoBuildTool.SetController( this );
	m_AutoBuildTool.SetRunID( 0 );

	//////////////////////////////////////////////////////////////////
	//Create the bay and its first lift
	pNewBay = new Bay();
	pNewBay->SetSystem( GetSystem() );
	iID = m_AutoBuildTool.AddBay( pNewBay );

 	pLift = new Lift();
	pLift->SetRise( LIFT_RISE_1500MM );
	iID = pNewBay->AddLift( pLift );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, eMaterial );

	pLift = new Lift();
	pLift->SetRise( LIFT_RISE_2000MM );
	iID = pNewBay->AddLift( pLift );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, eMaterial );

	pLift = new Lift();
	pLift->SetRise( LIFT_RISE_2000MM );
	iID = pNewBay->AddLift( pLift );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, eMaterial );

	pLift = new Lift();
	pLift->SetRise( LIFT_RISE_2000MM );
	iID = pNewBay->AddLift( pLift );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, eMaterial );

	pLift = new Lift();
	pLift->SetRise( LIFT_RISE_2000MM );
	iID = pNewBay->AddLift( pLift );

	m_AutoBuildTool.SetBayLength( COMPONENT_LENGTH_2400 );
	m_AutoBuildTool.SetBayWidth( COMPONENT_LENGTH_1200 );
	m_AutoBuildTool.SetRunFit( RF_TIGHT );

	//////////////////////////////////////////////////////////////////
	//Set to a dummy position
	ptRLBegin.x = 0.00;
	ptRLBegin.y = 0.00;
	ptRLBegin.z = 0.00;
	ptRLEnd = ptRLBegin;
	ptRLEnd.x+= GetCompDetails()->GetActualLength( pNewBay->GetSystem(), CT_LEDGER, pNewBay->GetBayLength(), eMaterial );
	m_AutoBuildTool.SetRL( ptRLBegin, ptRLEnd, false);

	//////////////////////////////////////////////////////////////////
	//Create the Standards with soleboards
	pNewBay->SetStandardsFit(STD_FIT_FINE);
	pNewBay->CreateStandardsArrangment( ConvertRLtoStarRL(11000.00, pNewBay->GetStarSeparation()), CNR_NORTH_EAST, true );
	pNewBay->CreateStandardsArrangment( ConvertRLtoStarRL(10000.00, pNewBay->GetStarSeparation()), CNR_SOUTH_EAST, true );
	pNewBay->CreateStandardsArrangment( ConvertRLtoStarRL(10000.00, pNewBay->GetStarSeparation()), CNR_SOUTH_WEST, true );
	pNewBay->CreateStandardsArrangment( ConvertRLtoStarRL(11000.00, pNewBay->GetStarSeparation()), CNR_NORTH_WEST, true );

	//////////////////////////////////////////////////////////////////
	//Create the Bracing
	pNewBay->CreateBracingArrangment ( NORTH, eMaterial );
	pNewBay->GetTemplate()->SetNBrace( true );
	pNewBay->CreateBracingArrangment ( SOUTH, eMaterial );
	pNewBay->CreateBracingArrangment ( EAST, eMaterial );
	pNewBay->CreateBracingArrangment ( WEST, eMaterial );

	/////////////////////////////////////////////////////////////////////
	//Create Ties
	dHeight = pNewBay->GetStandardPosition(CNR_SOUTH_EAST).z + m_AutoBuildTool.GetTiesVerticallyEvery();
	dStandardsHeight = pNewBay->GetStandardPosition(CNR_SOUTH_EAST).z + pNewBay->GetHeightOfStandards( CNR_SOUTH_EAST );

	double dTieLength;
	dTieLength = pNewBay->GetSmallestRequiredWallTieLength(SSE);

	if( dHeight>=dStandardsHeight )
	{
		//we need at least one tie
		pNewBay->AddComponent( CT_TIE_TUBE, SSE, dHeight, dTieLength, MT_STEEL );
	}
	else
	{
		while( dHeight<dStandardsHeight )
		{
			pNewBay->AddComponent( CT_TIE_TUBE, SSE, dHeight, dTieLength, MT_STEEL );
			dHeight+= m_AutoBuildTool.GetTiesVerticallyEvery();
		}
	}

	//////////////////////////////////////////////////////////////////
	//Ledgers
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_LEDGER,		pNewBay->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, eMaterial );

	//////////////////////////////////////////////////////////////////
	//transoms
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, eMaterial );
	pLift->AddComponent( CT_TRANSOM,	pNewBay->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, eMaterial );

	//////////////////////////////////////////////////////////////////
	//Handrails
	pLift->AddComponent( CT_RAIL,		pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_1000MM, eMaterial );
	m_AutoBuildTool.GetBayTemplate()->SetHandRailDist( 200.00 );

	//////////////////////////////////////////////////////////////////
	//Midrails
	pLift->AddComponent( CT_MID_RAIL,	pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0500MM, eMaterial );

	//////////////////////////////////////////////////////////////////
	//ToeBoard
	pLift->AddComponent( CT_TOE_BOARD,	pNewBay->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, eMaterial );

/*	//////////////////////////////////////////////////////////////////
	//Mesh guard - cannot be less then 1200
	#ifdef SMALLEST_MESH_IS_1200
	dMeshLength = max( pNewBay->GetBayLength(), COMPONENT_LENGTH_1200 );
	#else
	dMeshLength = pNewBay->GetBayLength();
	#endif	//#ifdef SMALLEST_MESH_IS_1200

	pLift->AddComponent( CT_MESH_GUARD,	dMeshLength,	NORTH,	LIFT_RISE_1000MM, eMaterial );
	pLift->AddComponent( CT_MESH_GUARD,	dMeshLength,	NORTH,	LIFT_RISE_2000MM, eMaterial );
*/
	//////////////////////////////////////////////////////////////////
	//Stageboards
	pLift->AddStage( SOUTH,	NUM_PLANKS_FOR_STAGE_BRD_2_WIDE, eMaterial, false );

	//Create the decking
	iNumberOfPlanks = GetNumberOfPlanksForWidth( pNewBay->GetBayWidth() );
	for( i=0; i<iNumberOfPlanks; i++ )
	{
		pLift->AddComponent( CT_DECKING_PLANK,	pNewBay->GetBayLength(),	SOUTH,	i, eMaterial );
	}

	//////////////////////////////////////////////////////////////////
	//create another lift
	pNewLift = new Lift();
	pNewLift->SetRise( LIFT_RISE_2000MM );
	iID = pNewBay->AddLift( pNewLift );
	*pNewLift = *pLift;
/*
	//////////////////////////////////////////////////////////////////
	//Chain link
	pNewBay->AddComponent( CT_SHADE_CLOTH,	NORTH,	0.00, 0.00, eMaterial );

	//////////////////////////////////////////////////////////////////
	//Shade cloth
	pNewBay->AddComponent( CT_CHAIN_LINK,	NORTH,	0.00, 0.00, eMaterial );
*/
	//////////////////////////////////////////////////////////////////
	//Remove the Chain mesh from the top lift!
	Component *pComp;
	for( int iComp=0; iComp<pNewLift->GetNumberOfComponents(); iComp++ )
	{
		if( pNewLift->GetComponentRise( iComp )==LIFT_RISE_2000MM )
		{
			pComp = pNewLift->GetComponent( iComp );
			if( pComp->GetType()==CT_MESH_GUARD )
			{
				pNewLift->DeleteComponent( iComp );
				//we have now removed this component so we better decrement the loop counter
				iComp--;
			}
		}
	}

	//////////////////////////////////////////////////////////////////
#ifndef _DEBUG
	m_AutoBuildTool.SetVisible( VIS_NOT_VISIBLE );
#endif
}	//CreateDefaultAutoBuildRun()


//////////////////////////////////////////////////////////////////////
//BOM
//prompts the user for a file name and creates the BOM
void Controller::BOM()
{
	int			i;
	Run			*pRun;
	UINT		uiMode;
	TCHAR		szCause[255];
	CString		sFileName, strFormatted, sMessage;
	CStdioFile	File;
	LapboardBay	*pLapboard;
	Component	*pComponent;


	///////////////////////////////////////////////////
	//Get the file name to use from the user
	uiMode = CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText;
//	uiMode = CFile::modeCreate|CFile::modeRead|CFile::shareExclusive|CFile::typeText;
	sFileName = _T("BOM.csv");
    CFileDialog dbox(FALSE, EXTENTION_BOM_FILE, sFileName,
		OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN,
		FILTER_BOM_FILE );
    dbox.m_ofn.lpstrTitle = _T("Create BOM file");
    if (dbox.DoModal()!=IDOK)
		return;
	sFileName = dbox.GetPathName();

	//Delete and ".."'s
	int iPos1, iPos2;
	sFileName.Replace( _T(".."), _T(".") );

	//Delete any second extensions
	iPos1=sFileName.Find( _T("."), 0 );
	iPos2=sFileName.Find( _T("."), iPos1 );
	if( iPos2>iPos1 )
	{
		//chop off from the last "." onward.
		sFileName = sFileName.Left( iPos2-1 );
	}

	///////////////////////////////////////////////////
	//open the file
	CFileException Error;
	if( !File.Open( sFileName, uiMode, &Error ) )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		Error.GetErrorMessage(szCause, 255);
    sMessage = _T("I couldn't open the file to write to it:\n\n");
		sMessage+= szCause;
		MessageBox( NULL, sMessage, _T("File Open Error"), MB_OK );
		return;
	}

#ifdef _DEBUG
	///////////////////////////////////////////////////
	//Autobuild
	File.WriteString( _T("****Components of Autobuild****\n") );
	m_AutoBuildTool.WriteBOMTo( &File );
#endif //#ifdef _DEBUG

	double	dPos, dIncr;
	dPos = 0.00;

	acedSetStatusBarProgressMeter( _T("Creating BOM"), 0, 100 );
	acedSetStatusBarProgressMeterPos( 0 );

	///////////////////////////////////////////////////
	//Runs
	if( GetNumberOfRuns()>0 )
	{
		dIncr = 85.00/((double)GetNumberOfRuns());

    File.WriteString( _T("Bay #,Run ID,Bay ID,Lift ID,Component ID,Type,Part Number,Material,Length,Weight,Level,Stage\n") );
		for( i=0; i<GetNumberOfRuns(); i++ )
		{
			pRun  = m_raRuns.GetAt( i );
			pRun->WriteBOMTo( &File );
			dPos+=dIncr;
			acedSetStatusBarProgressMeterPos( (int)dPos );
		}
	}

	///////////////////////////////////////////////////
	//Lapboards
	if( GetNumberOfLapboards()>0 )
	{
		dIncr = 10.00/GetNumberOfLapboards();

		///////////////////////////////////////////////////
		//Lapboards
		File.WriteString( _T("Lapboard ID,Lift ID,Component ID,Type,Part Number,Material,Length,Weight,Level,Stage\n") );
		for( i=0; i<GetNumberOfLapboards(); i++ )
		{
			pLapboard = m_Lapboards.GetAt( i );
			pLapboard->WriteBOMTo( &File );
			dPos+=dIncr;
			acedSetStatusBarProgressMeterPos( (int)dPos );
		}
	}

	///////////////////////////////////////////////////
	//BOMExtra
	if( m_BOMExtraCompList.GetNumberOfComponents()>0 )
	{
		dIncr = 5/GetNumberOfLapboards();

		File.WriteString( _T("****BOM Extra Components****\n") );
		for( i=0; i<m_BOMExtraCompList.GetNumberOfComponents(); i++ )
		{
			pComponent  = m_BOMExtraCompList.GetComponent(i);
			pComponent->WriteBOMTo( &File );
			dPos+=dIncr;
			acedSetStatusBarProgressMeterPos( (int)dPos );
		}
	}

	//BOM created!
	acedSetStatusBarProgressMeterPos( 100 );
  sMessage.Format( _T("The Bill of Materials file:\n\n%s\n\nwas successfully created!\n"), sFileName );
	MessageBox( NULL, sMessage, _T("BOM successful"), MB_OK );
	acedRestoreStatusBar( );
}	//BOM()


//////////////////////////////////////////////////////////////////////
//AddFromAutoBuildToRun
//copies a run from the autobuild to the end of the last run
Bay *Controller::AddFromAutoBuildToRun( int iRunID, double dRLs[2] )
{
	int	iID;
	Bay	*pABTBay, *pNewBay;
	Run	*pRun;

	//We always add to the last run, so find the last run
	assert( iRunID==GetNumberOfRuns()-1 );
	pRun = GetRun( iRunID );

	//create the new bay
	pNewBay = new Bay();

	pNewBay->SetAllowDraw( false );

	//copy the details from the AutoBuildTools bay
	pABTBay = m_AutoBuildTool.GetBay( 0 );
	assert( pABTBay!=NULL );
	pNewBay->SetRunPointer( pRun );
	*pNewBay = *pABTBay;

	//add the new bay to the end of the run
	iID = pRun->AddBay( pNewBay, true );

	//We always add to the end of the run so the new bay should be the same as
	//	the number of bays (since we use a zero based numbering system)
	assert( iID==pNewBay->GetID() );

	double daRLs[4];
	daRLs[CNR_NORTH_EAST] = dRLs[1];
	daRLs[CNR_SOUTH_EAST] = dRLs[1];
	daRLs[CNR_SOUTH_WEST] = dRLs[0];
	daRLs[CNR_NORTH_WEST] = dRLs[0];
	pNewBay->SetAndAdjustRLs( daRLs, true );

//	pNewBay->GetTemplate()->CreateSchematic( );

	return pNewBay;
}	//AddFromAutoBuildToRun( int iRunID, double dRLs[2] )


//////////////////////////////////////////////////////////////////////
//AddRun
//Adds a run to the run list and sets the appropriate numbers
int Controller::AddRun(Run *pNewRun)
{
	int		iRunID;

	iRunID = m_raRuns.Add( pNewRun );
	pNewRun->SetRunID( iRunID );
	pNewRun->SetController( this );
	pNewRun->SetLongerBracingValues();

	return iRunID;
}	//AddRun(Run *pNewRun)


void Controller::InsertRun(int iPos, Run *pNewRun)
{
	if( iPos<0 || iPos>GetNumberOfRuns() )
	{
		assert( false );
		return;
	}
	
	if( iPos==GetNumberOfRuns() )
	{
		AddRun( pNewRun );
	}
	else
	{
		m_raRuns.InsertAt( iPos, pNewRun );
		pNewRun->SetRunID( iPos );
		pNewRun->SetController( this );

		//renumber subsequent runs
		int	i;
		Run	*pRun;
		for( i=iPos+1; i<GetNumberOfRuns(); i++ )
		{
			pRun = GetRun( i );
			pRun->SetRunID( i );
		}
	}

	//RenumberAllBays();
}	//InsertRun(int iPos, Run *pNewRun)



/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: controller.cpp $
 * 
 * *****************  Version 309  *****************
 * User: Jsb          Date: 20/12/00   Time: 3:19p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the Corner hopup problem, this should hopefully be the final 1.2
 * release
 * 
 * *****************  Version 308  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:10a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 307  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 306  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 305  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 304  *****************
 * User: Jsb          Date: 17/11/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * HCT Colour seems to be fully working now
 * 
 * *****************  Version 303  *****************
 * User: Jsb          Date: 16/11/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * About to release KwikScaf version 1.1k
 * 
 * *****************  Version 302  *****************
 * User: Jsb          Date: 13/11/00   Time: 4:20p
 * Updated in $/Meccano/Stage 1/Code
 * About to work from home
 * 
 * *****************  Version 301  *****************
 * User: Jsb          Date: 9/11/00    Time: 12:17p
 * Updated in $/Meccano/Stage 1/Code
 * Building r1.1j
 * 
 * *****************  Version 300  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 299  *****************
 * User: Jsb          Date: 25/10/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 298  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 297  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 296  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 295  *****************
 * User: Jsb          Date: 5/10/00    Time: 11:59a
 * Updated in $/Meccano/Stage 1/Code
 * About to build 1.5.1.4 (R1.1d) - This should have the copied matrix
 * move commands sorted out.
 * 
 * *****************  Version 294  *****************
 * User: Jsb          Date: 5/10/00    Time: 8:14a
 * Updated in $/Meccano/Stage 1/Code
 * just finished building version 1.5.1.3 (R1.1c)
 * 
 * *****************  Version 293  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 292  *****************
 * User: Jsb          Date: 29/09/00   Time: 9:09a
 * Updated in $/Meccano/Stage 1/Code
 * Just about to change the mills & kwikscaf _tsystem from bool to enum
 * 
 * *****************  Version 291  *****************
 * User: Jsb          Date: 27/09/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * This is most likely the version we are sending to Perth for Testing of
 * 3D components
 * 
 * *****************  Version 290  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:49p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 289  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 288  *****************
 * User: Jsb          Date: 21/09/00   Time: 3:25p
 * Updated in $/Meccano/Stage 1/Code
 * built 1.0m
 * 
 * *****************  Version 287  *****************
 * User: Jsb          Date: 19/09/00   Time: 5:08p
 * Updated in $/Meccano/Stage 1/Code
 * rotate seems to be working correctly now
 * 
 * *****************  Version 286  *****************
 * User: Jsb          Date: 18/09/00   Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * Finished Bill Cross Stage 3
 * 
 * *****************  Version 285  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 284  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 282  *****************
 * User: Jsb          Date: 14/09/00   Time: 9:41a
 * Updated in $/Meccano/Stage 1/Code
 * About to Rewind back to 1.0h (the one after UK release) since we need
 * to send a verision Australia wide
 * 
 * *****************  Version 281  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 280  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 279  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 278  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 277  *****************
 * User: Jsb          Date: 5/09/00    Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * About to build R1.0d
 * 
 * *****************  Version 276  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 275  *****************
 * User: Jsb          Date: 31/08/00   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 274  *****************
 * User: Jsb          Date: 30/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Customize Colour complete and hidden - 
 * Shade and Chain mesh prices fixed
 * Bill Cross stage 1 fully complete
 * 
 * *****************  Version 273  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 272  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 271  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 268  *****************
 * User: Jsb          Date: 11/08/00   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed crash where we couldn't _topen any previous drawings thanks to
 * AGT's change of fonts
 * 
 * *****************  Version 267  *****************
 * User: Jsb          Date: 9/08/00    Time: 10:04a
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8x
 * 
 * *****************  Version 266  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 265  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 264  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 263  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 262  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 261  *****************
 * User: Jsb          Date: 2/08/00    Time: 10:31a
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8s
 * 
 * *****************  Version 260  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 259  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 258  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 257  *****************
 * User: Jsb          Date: 27/07/00   Time: 1:07p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 8p
 * 
 * *****************  Version 256  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 255  *****************
 * User: Jsb          Date: 24/07/00   Time: 1:38p
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to B4 3D position exact started
 * 
 * *****************  Version 254  *****************
 * User: Jsb          Date: 21/07/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly completed the SetDecks to Levels bug
 * 
 * *****************  Version 253  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 252  *****************
 * User: Jsb          Date: 18/07/00   Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 251  *****************
 * User: Jsb          Date: 17/07/00   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the 500mm star separation to 495.3
 * 
 * *****************  Version 250  *****************
 * User: Jsb          Date: 13/07/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 249  *****************
 * User: Jsb          Date: 13/07/00   Time: 8:08a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 248  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 247  *****************
 * User: Jsb          Date: 7/07/00    Time: 7:50a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 246  *****************
 * User: Jsb          Date: 6/07/00    Time: 12:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 245  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 244  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 243  *****************
 * User: Jsb          Date: 28/06/00   Time: 1:27p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create a seperate Toolbar project
 * 
 * *****************  Version 242  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 241  *****************
 * User: Jsb          Date: 26/06/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * auto changing heights of adjoining stairs and ladders
 * 
 * *****************  Version 240  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 239  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 238  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 237  *****************
 * User: Jsb          Date: 20/06/00   Time: 12:28p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way moving decks and roof protect lapboards look
 * for  lapboards 
 * 
 * *****************  Version 236  *****************
 * User: Jsb          Date: 19/06/00   Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 235  *****************
 * User: Jsb          Date: 16/06/00   Time: 4:59p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 234  *****************
 * User: Jsb          Date: 15/06/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 233  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 232  *****************
 * User: Jsb          Date: 14/06/00   Time: 8:03a
 * Updated in $/Meccano/Stage 1/Code
 * Just Built RC8b
 * 
 * *****************  Version 231  *****************
 * User: Jsb          Date: 9/06/00    Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * taking code home
 * 
 * *****************  Version 230  *****************
 * User: Jsb          Date: 9/06/00    Time: 1:47p
 * Updated in $/Meccano/Stage 1/Code
 * About to play with the way Bay::SetAndAdjustRLs works
 * 
 * *****************  Version 229  *****************
 * User: Jsb          Date: 8/06/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Rotation of Bays is operational, rotation of laps is nearly working
 * 
 * *****************  Version 228  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 226  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 225  *****************
 * User: Jsb          Date: 2/06/00    Time: 3:02p
 * Updated in $/Meccano/Stage 1/Code
 * need to find why tietubes are removing hopups
 * 
 * *****************  Version 224  *****************
 * User: Jsb          Date: 1/06/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 223  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 222  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 221  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 220  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 219  *****************
 * User: Jsb          Date: 23/05/00   Time: 3:44p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC6
 * 
 * *****************  Version 218  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 217  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 216  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 215  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 214  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 213  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 212  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 211  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 210  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 209  *****************
 * User: Jsb          Date: 4/05/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 208  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 207  *****************
 * User: Jsb          Date: 20/04/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 206  *****************
 * User: Jsb          Date: 20/04/00   Time: 12:53p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 205  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 204  *****************
 * User: Jsb          Date: 18/04/00   Time: 9:53a
 * Updated in $/Meccano/Stage 1/Code
 * about to stamp RC5 (1.4.05.00)
 * 
 * *****************  Version 203  *****************
 * User: Jsb          Date: 17/04/00   Time: 4:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 202  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 201  *****************
 * User: Jsb          Date: 14/04/00   Time: 9:57a
 * Updated in $/Meccano/Stage 1/Code
 * Colour By Stage & level
 * 
 * *****************  Version 200  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 199  *****************
 * User: Jsb          Date: 11/04/00   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 198  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 197  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 196  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 195  *****************
 * User: Jsb          Date: 3/04/00    Time: 4:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 194  *****************
 * User: Jsb          Date: 3/04/00    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 193  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 192  *****************
 * User: Jsb          Date: 27/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 191  *****************
 * User: Jsb          Date: 16/03/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 190  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 189  *****************
 * User: Jsb          Date: 13/03/00   Time: 5:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 188  *****************
 * User: Jsb          Date: 10/03/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 187  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 186  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 185  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 184  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 183  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 182  *****************
 * User: Jsb          Date: 25/02/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 181  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 180  *****************
 * User: Jsb          Date: 23/02/00   Time: 2:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 179  *****************
 * User: Jsb          Date: 21/02/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Working from home tomorrow
 * 
 * *****************  Version 178  *****************
 * User: Jsb          Date: 21/02/00   Time: 11:41a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 177  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 176  *****************
 * User: Jsb          Date: 16/02/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Big Day, just fixed Marks save bug, sydney corners operational
 * 
 * *****************  Version 175  *****************
 * User: Jsb          Date: 15/02/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 174  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 173  *****************
 * User: Jsb          Date: 11/02/00   Time: 9:12a
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the save and load bug in the stages & levels stuff
 * 
 * *****************  Version 172  *****************
 * User: Jsb          Date: 10/02/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * I think I have complete the JM bug
 * 
 * *****************  Version 171  *****************
 * User: Jsb          Date: 10/02/00   Time: 10:56a
 * Updated in $/Meccano/Stage 1/Code
 * Just trying to get the committed stuff working
 * 
 * *****************  Version 170  *****************
 * User: Jsb          Date: 9/02/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 169  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 168  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 167  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 166  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 165  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 164  *****************
 * User: Jsb          Date: 2/02/00    Time: 3:59p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 163  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 162  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 161  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 160  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 159  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 158  *****************
 * User: Jsb          Date: 20/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 157  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 156  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 155  *****************
 * User: Jsb          Date: 14/01/00   Time: 2:37p
 * Updated in $/Meccano/Stage 1/Code
 * Fixing the Dialog boxes at the moment
 * 
 * *****************  Version 154  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 153  *****************
 * User: Jsb          Date: 13/01/00   Time: 8:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 152  *****************
 * User: Jsb          Date: 12/01/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 151  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 150  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 5/01/00    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished the hopup without decking adds handrail
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12 (Beta13)
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 23/12/99   Time: 9:26a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 22/12/99   Time: 8:03a
 * Updated in $/Meccano/Stage 1/Code
 * about to _tremove code from the _tremove ties function
 * 
 * *****************  Version 142  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 140  *****************
 * User: Jsb          Date: 16/12/99   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Finished adding function:
 * AddShadeCloth
 * AddHopup
 * AddChainLink
 * AddHandrail
 * plus delete functions for the same!
 * 
 * *****************  Version 139  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 138  *****************
 * User: Jsb          Date: 16/12/99   Time: 10:26a
 * Updated in $/Meccano/Stage 1/Code
 * ChangeRLDlg now compilers, however I need to change the name of the
 * file
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 15/12/99   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 14/12/99   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to build Beta11
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all Deletes and deletes are correct
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * fixing up the chain link delete problem
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 9/12/99    Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 8/12/99    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * Over and Underruns working correctly
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 6/12/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * Got everything working as well as I had them at home
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 11/11/99   Time: 3:56p
 * Updated in $/Meccano/Stage 1/Code
 * Just completed:
 * 491 - Inertbay needs better error msg
 * 408 - Can only inert one bay at a timeinsert
 * 467 - Inert bay north has offset
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 8/11/99    Time: 3:57p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on bug #447
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 8/11/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * 445 - Moved matrix not stored in saved file
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 8/11/99    Time: 11:27a
 * Updated in $/Meccano/Stage 1/Code
 * Lapboard point to point now operational
 * 
 * *****************  Version 113  *****************
 * User: Dar          Date: 2/11/99    Time: 12:09p
 * Updated in $/Meccano/Stage 1/Code
 * final check in for me
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 2/11/99    Time: 10:57a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 28/10/99   Time: 3:53p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Shade cloth and chain link now operational!
 * 2) Crash on setting TWH fixed
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 27/10/99   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 27/10/99   Time: 2:49p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Mesh Guard now working
 * 2) Ties nearly working
 * 
 * *****************  Version 106  *****************
 * User: Dar          Date: 27/10/99   Time: 12:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 104  *****************
 * User: Dar          Date: 26/10/99   Time: 3:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 103  *****************
 * User: Dar          Date: 26/10/99   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 19/10/99   Time: 3:10p
 * Updated in $/Meccano/Stage 1/Code
 * Birdcaging is almost working correctly now
 * 
 * *****************  Version 101  *****************
 * User: Dar          Date: 19/10/99   Time: 10:59a
 * Updated in $/Meccano/Stage 1/Code
 * added stock.csv file for BOM summary report so hire/sale pricing and
 * weight can be calculated.
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 15/10/99   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 12/10/99   Time: 9:32a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Level Assignment now operational - but have to call _T("RegenLevels")
 * 2) Fixed the Schematic View Serialize error.
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Dar          Date: 8/10/99    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * added BOM Summary dialogs - print function still to be done
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 8/10/99    Time: 11:13a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 8/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * RLScale, and Levels are nearly working
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Dar          Date: 6/10/99    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 6/10/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * Schematic Corner Stage boards are now correct!
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 6/10/99    Time: 9:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 6/10/99    Time: 9:54a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards now correct
 * 2) Lapboards now move with 3D comps
 * 3) Delete Standards Arranginement now working
 * 4) Bay offset within sloped runs
 * 5) RL's working correctly
 * 6) Summary info now working
 * 
 * *****************  Version 87  *****************
 * User: Dar          Date: 5/10/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * worked on edit bays still not working however
 * 
 * *****************  Version 86  *****************
 * User: Dar          Date: 5/10/99    Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 5/10/99    Time: 1:35p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished SummaryInfo class
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 5/10/99    Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 5/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * Added the PartNumber to the ComponentDetails
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 5/10/99    Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 81  *****************
 * User: Dar          Date: 4/10/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * added code for bay selection
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 76  *****************
 * User: Dar          Date: 27/09/99   Time: 10:51a
 * Updated in $/Meccano/Stage 1/Code
 * still problem when no database exists already
 * 
 * *****************  Version 75  *****************
 * User: Dar          Date: 9/23/99    Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * basic load save of job description info in NOD needs work still
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 23/09/99   Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * All 3D Component Entity drawing now use the same path, and it will
 * prompt them for that path if it can't find it
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 22/09/99   Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 72  *****************
 * User: Dar          Date: 9/22/99    Time: 1:45p
 * Updated in $/Meccano/Stage 1/Code
 * job details load save done a little more work on ui needed
 * 
 * *****************  Version 71  *****************
 * User: Dar          Date: 9/22/99    Time: 10:40a
 * Updated in $/Meccano/Stage 1/Code
 * Bom extra alpha save load works
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 69  *****************
 * User: Dar          Date: 9/21/99    Time: 10:19a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 21/09/99   Time: 10:00a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:05a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 66  *****************
 * User: Dar          Date: 9/21/99    Time: 8:17a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:08a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 15/09/99   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * checked in by request of Andrew Taylor for backup purposes
 * 
 * *****************  Version 59  *****************
 * User: Dar          Date: 9/15/99    Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 15/09/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * Saving now:
 * 1) Does not delete the Entities, prior to saving
 * Loading now:
 * 1) Deletes all entities that are not on one of the specified layers, so
 * we can now export all data to non-meccano people, as simple lines, etc.
 * 
 * *****************  Version 57  *****************
 * User: Dar          Date: 9/15/99    Time: 9:57a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Dar          Date: 9/15/99    Time: 9:10a
 * Updated in $/Meccano/Stage 1/Code
 * added delete3dview to serialize store function
 * 
 * *****************  Version 55  *****************
 * User: Dar          Date: 9/14/99    Time: 5:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Dar          Date: 9/14/99    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Dar          Date: 9/13/99    Time: 11:53a
 * Updated in $/Meccano/Stage 1/Code
 * JSB: Releasing code, so it is available for me, since DAR is sick today
 * 
 * *****************  Version 52  *****************
 * User: Dar          Date: 9/09/99    Time: 1:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 9/09/99    Time: 12:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 9/09/99    Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Matrix now has crosshair which operates correctly
 * 2) Still only have one (1) MatrixElement
 * 
 * *****************  Version 49  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 47  *****************
 * User: Dar          Date: 9/07/99    Time: 3:44p
 * Updated in $/Meccano/Stage 1/Code
 * serialize added to controller
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 9/03/99    Time: 11:18a
 * Updated in $/Meccano/Stage 1/Code
 * Edit bay is now bringing up the dialog boxes
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 9/03/99    Time: 11:01a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 8/23/99    Time: 11:43a
 * Updated in $/Meccano/Stage 1/Code
 * Insert Lift function completed
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/23/99    Time: 9:48a
 * Updated in $/Meccano/Stage 1/Code
 * Delete (so far)  is now working correctly
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/19/99    Time: 1:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/13/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/12/99    Time: 7:20p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the layers, visibility of autobuild, tight fit bay working
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/12/99    Time: 2:50p
 * Updated in $/Meccano/Stage 1/Code
 * Updating for andrews build
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 1:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/12/99    Time: 10:32a
 * Updated in $/Meccano/Stage 1/Code
 * 3d view working for the most part
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/10/99    Time: 12:19p
 * Updated in $/Meccano/Stage 1/Code
 * Added the Entity class
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/05/99    Time: 4:43p
 * Updated in $/Meccano/Stage 1/Code
 * Taking files home to see If I can compile code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 3/08/99    Time: 10:51
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing the Autobuild
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 29/07/99   Time: 15:56
 * Updated in $/Meccano/Stage 1/Code
 * I will be away friday
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 29/07/99   Time: 12:18
 * Updated in $/Meccano/Stage 1/Code
 * Completing functionality for Controller class
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 29/07/99   Time: 10:05
 * Updated in $/Meccano/Stage 1/Code
 * I have now added a test routine to the controller which calls the
 * _T("LiftList.Test") function
 * I have added operator functions to the Run class
 * 
 * *****************  Version 4  *****************
* User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 *******************************************************************************/

//////////////////////////////////////////////////////////////////////////////////
//GetCompDetails
//
ComponentDetailsArray * Controller::GetCompDetails()
{
	return &m_ltCompDetails;
}

//////////////////////////////////////////////////////////////////////////////////////////
//Move3DCrosshair(Matrix3D Transform, bool bStoDeletement/*=true*/)
void Controller::Move3DCrosshair(Matrix3D Transform, bool bStoDeletement/*=true*/)
{
	int			i;
	Run			*pRun;
	LapboardBay	*pLapboard;

	assert( IsCrosshairCreated() );

	///////////////////////////////////////////////////////////////////
	//Move Bays
	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun = GetRun( i );
		pRun->Move( Transform, false );
	}

	///////////////////////////////////////////////////////////////////
	//Move LapboardBays
	for( i=0; i<GetNumberOfLapboards(); i++ )
	{
		pLapboard = m_Lapboards.GetAt( i );
		pLapboard->Move( Transform, false );
	}

	///////////////////////////////////////////////////////////////////
	//Store It
	if( bStoDeletement )
	{
		Set3DCrosshairTransform( Transform );
	}
}	//Move3DCrosshair(Matrix3D Transform, bool bStoDeletement/*=true*/)

/////////////////////////////////////////////////////////////////////////////////////////
//UnMove3DCrosshair()
//
Matrix3D Controller::UnMove3DCrosshair()
{
	Matrix3D	Original, Inverse;

	assert( IsCrosshairCreated() );

	//store the original transformation for later use
	Original = Get3DCrosshairTransform();

	//we need to move the Bay back to its original position
	Inverse = Get3DCrosshairTransform();
	Inverse.invert();

	Move3DCrosshair( Inverse, true );

	//return the original matrix
	return Original;
}	//UnMove3DCrosshair()

/////////////////////////////////////////////////////////////////////////////////////////
//Get3DCrosshairTransform() const
//	
Matrix3D Controller::Get3DCrosshairTransform() const
{
	return m_3DCrosshairTransform;
}	//Get3DCrosshairTransform() const

/////////////////////////////////////////////////////////////////////////////////////////
//Set3DCrosshairTransform(Matrix3D Transform)
//
void Controller::Set3DCrosshairTransform(Matrix3D Transform)
{
	m_3DCrosshairTransform = Transform;
}	//Set3DCrosshairTransform(Matrix3D Transform)


/////////////////////////////////////////////////////////////////////////////////////////
//CleanUp()
//delete all objects, that this object owns, if they are marked
//	for deletion
void	Controller::CleanUp( bool bShowProgressBar/*=true*/ )
{
	Run			*pRun;
	LapboardBay	*pLapBay;

	int iPos, iFull;

	//during the cleanup process we may have deleted all daughters
	//	for a parent class, so another cleanup may be required
	while( GetDirtyFlag()!=DF_CLEAN )
	{
		SetDirtyFlag( DF_CLEAN );

		iPos = 0;

		///////////////////////////////////////////////////////////////
		//Visual Components
		switch( GetVisualComponents()->GetDirtyFlag() )
		{
		case(DF_DIRTY):
			GetVisualComponents()->CleanUp(bShowProgressBar);
			break;
		case(DF_CLEAN):
			//fine
			break;
		default:
			assert( false );	//VC cannot be anything else
		}

		///////////////////////////////////////////////////////////////
		//Lapboards
		iPos = 0;
		iFull = GetNumberOfLapboards();
		if( bShowProgressBar )
			acedSetStatusBarProgressMeter( _T("Cleaning Lapboards"), 0, iFull );
		for( int iLapID=0; (GetNumberOfLapboards()>0) && (iLapID<GetNumberOfLapboards()); iLapID++ )
		{
			if( bShowProgressBar )
				acedSetStatusBarProgressMeterPos(++iPos);

			pLapBay = GetLapboard( iLapID );
			switch( pLapBay->GetDirtyFlag() )
			{
			case( DF_DELETE ):
				if( !DeleteLapboard( iLapID ) )
				{
					assert( false );
				}
				iLapID--;	//we have just deleted the element, so don't increment iLapID;
				break;
			case( DF_DIRTY ):
				pLapBay->CleanUp();
				if( pLapBay->GetDirtyFlag()!=DF_CLEAN )
				{
					//it is not finished yet, do it again
					iLapID--;
				}
				break;
			default:
				assert( false );
				//fallthrough
			case( DF_CLEAN ):
				//Do nothing
				break;
			}
		}
		if( bShowProgressBar )
			acedSetStatusBarProgressMeterPos(iFull);

		///////////////////////////////////////////////////////////////
		//Runs
		int iRunID;
		iPos = 0;
		iFull = 0;
		for( iRunID=0; iRunID<GetNumberOfRuns(); iRunID++ )
		{
			pRun = GetRun(iRunID);
			iFull+= pRun->GetNumberOfBays();
		}

		if( bShowProgressBar )
			acedSetStatusBarProgressMeter( _T("Cleaning Runs"), 0, iFull );

		for( iRunID=0; (GetNumberOfRuns()>0) && (iRunID<GetNumberOfRuns()); iRunID++ )
		{
			pRun = GetRun(iRunID);
			switch( pRun->GetDirtyFlag() )
			{
			case( DF_DELETE ):
				iPos += pRun->GetNumberOfBays();
				if( !DeleteRuns( iRunID, 1, false ) )
				{
					assert( false );
				}
				iRunID--;	//we have just deleted the element, so don't increment i;
				break;
			case( DF_DIRTY ):
				if( !bShowProgressBar )
					iPos = -1;
				pRun->CleanUp( iPos );
				if( pRun->GetDirtyFlag()!=DF_CLEAN )
				{
					//it is not finished yet, do it again
					iRunID--;
				}
				break;
			default:
				assert( false );
				//fallthrough
			case( DF_CLEAN ):
				iPos += pRun->GetNumberOfBays();
				//Do nothing
				break;
			}
			if( bShowProgressBar )
				acedSetStatusBarProgressMeterPos(iPos);
		}
		if( bShowProgressBar )
		{
			acedSetStatusBarProgressMeterPos(iFull);
			acedRestoreStatusBar();
		}
	}
	//do we need to renumber the bays?
	RenumberAllBays();
}	//CleanUp()

//////////////////////////////////////////////////////////////////
//Says this object needs to be deleted
DirtyFlagEnum	Controller::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

/////////////////////////////////////////////////////////////////////////////////
//SetDirtyFlag
//set the object to be deleted
void	Controller::SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward/*=false*/ )
{
	assert( m_dfDirtyFlag!=DF_DELETE );

	if(dfDirtyFlag==DF_CLEAN && bSetDownward)
	{
		for( int i=0; i<GetNumberOfRuns(); i++ )
		{
			GetRun(i)->SetDirtyFlag(dfDirtyFlag, bSetDownward);
		}

		for(int i=0; i<GetNumberOfVisualComponents(); i++ )
		{
			GetVisualComponents()->GetComponent(i)->SetDirtyFlag(dfDirtyFlag);
		}
	}

	//set the dirty flag
	m_dfDirtyFlag = dfDirtyFlag;
}

/////////////////////////////////////////////////////////////////////////////////////////
//Create3DView()
//
void Controller::Create3DView(bool bUseLast/*=false*/)
{
	int			i, j, iRet;
	Run			*pRun;
	bool		bLoop, bVisualComponentsOnly;
	TCHAR		keywrd[255];
	CString		sKeyWrd, sMsg;
	Point3D		point;
	BayList		Bays;
	Vector3D	Vect;
	LapboardBay	*pLapboard;

	point.set( 0.00, 0.00, 0.00 );

	bVisualComponentsOnly = false;
	do
	{
		bLoop = true;

		if( bUseLast )
		{
			Vect.set( m_pt3DPosition.x, m_pt3DPosition.y, m_pt3DPosition.z );
			bLoop = false;
		}
		else
		{
			if( acedInitGet( RSG_OTHER, NULL )==RTERROR )
			{
				assert( false );
			}

			sMsg = _T("\nChoose position for the origin for the 3D view ");
				
			if( m_ia3DBaysSelected.GetSize()>0 )
				sMsg+= _T("<selected bays> ");
			else
				sMsg+= _T("<all bays> ");
				
			sMsg+= _T("[Last/Visual comps only/");
			if( m_ia3DBaysSelected.GetSize()>0 )
        sMsg+= _T("reSelect bays/all Bays]: ");
			else
        sMsg+= _T("Select bays]: ");

			iRet = GetPoint( sMsg, point );
			switch(iRet)
			{
			default:
				//invalid user input
				return;
				
			case(RTNONE):
				//user did a right click!
				Vect.set( point.x, point.y, point.z );
				bLoop = false;
				break;

			case(RTKWORD):
				acedGetInput( keywrd );
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();
				if( sKeyWrd[0]==_T('L') )
				{
					//Want to use last point
					Vect.set( m_pt3DPosition.x, m_pt3DPosition.y, m_pt3DPosition.z );
					bLoop = false;
				}
				if( sKeyWrd[0]==_T('B') )
				{
					//Show all bays
					m_ia3DBaysSelected.RemoveAll();
				}
				if( sKeyWrd[0]==_T('V') )
				{
					//Only show visual components
					bVisualComponentsOnly = true;
				}
				if( sKeyWrd[0]==_T('S') )
				{
					//Show Selected bays
					if( !SelectBays( Bays, true ) )
						return;
//					CheckForStairLadder( &Bays );
					m_ia3DBaysSelected.RemoveAll();
					for( j=0; j<Bays.GetSize(); j++ )
					{
						m_ia3DBaysSelected.Add(Bays.GetAt(j)->GetBayNumber());
					}
				}
				else
				{
					MessageBeep(MB_ICONASTERISK);
				}
				break;

			case(RTNORM):
				//////////////////////////////////////////////////////////////////////
				//We need to find the point which is the lowest left side schematic
				Point3D	pt;
				pt = GetLowerLeftOfSchematic();
				Vect = point-pt;
				m_pt3DPosition.x = Vect.x;
				m_pt3DPosition.y = Vect.y;
				m_pt3DPosition.z = Vect.z;
				bLoop = false;
				break;
			}
		}
	}while( bLoop );

	Matrix3D	Transform;
	Transform.setTranslation( Vect );
	Set3DCrosshairTransform( Transform );

	if( GetNumberOfRuns()<=0 && GetNumberOfLapboards()<=0 && GetVisualComponents()->GetNumberOfComponents()<=0 )
	{
		//No components to show
		CString sMsg;
    sMsg = _T("STOP: There are no components in the drawing to display!");
		MessageBox( NULL, sMsg, _T("Nothing to do"), MB_OK|MB_ICONSTOP );
		MessageBeep(MB_ICONSTOP);
		return;
	}

	if( bVisualComponentsOnly && GetVisualComponents()->GetNumberOfComponents()<=0 )
	{
		//No components to show
		CString sMsg;
    sMsg = _T("STOP: There are no Visual components in the drawing to display!");
		MessageBox( NULL, sMsg, _T("Nothing to do"), MB_OK|MB_ICONSTOP );
		MessageBeep(MB_ICONSTOP);
		return;
	}

	///////////////////////////////////////////
	//Progress Bar init
	int iPos, iFull;
	iFull = 0;
	if( m_ia3DBaysSelected.GetSize()>0 )
	{
		for( i=0; i<GetNumberOfRuns(); i++ )
		{
			pRun  = m_raRuns.GetAt( i );
			iFull+= pRun->GetNumberOfBays();
		}
		iFull+= GetNumberOfLapboards();
		iFull+= GetVisualComponents()->GetNumberOfComponents();
	}
	else
	{
		iFull = m_ia3DBaysSelected.GetSize();
	}
	acedSetStatusBarProgressMeter( _T("Calculating 3D View"), 0, iFull );
	iPos = 0;

#ifdef SHOW_AUTOBUILD_BAY
	m_AutoBuildTool.Create3DView(iPos);
#endif	//#ifdef SHOW_AUTOBUILD_BAY

	Create3DCrosshair();

	if( bVisualComponentsOnly )
	{
		///////////////////////////////////////////////////
		//Visual Components
		GetVisualComponents()->Create3DView(iPos);
	}
	else
	{
		if( m_ia3DBaysSelected.GetSize()>0 )
		{
			Bay *pBay;

			///////////////////////////////////////////////////
			//Lapboards
			for( i=0; i<GetNumberOfLapboards(); i++ )
			{
				pLapboard = m_Lapboards.GetAt( i );
				if( pLapboard->GetBayEast()!=NULL && m_ia3DBaysSelected.IsElementOf( pLapboard->GetBayEast()->GetBayNumber() ) )
				{
					if( !pLapboard->Create3DView() )
					{
						MessageBeep(MB_ICONSTOP);
						acedRestoreStatusBar();
						return;
					}
				}
				if( pLapboard->GetBayWest()!=NULL && m_ia3DBaysSelected.IsElementOf( pLapboard->GetBayWest()->GetBayNumber() ) )
				{
					if( !pLapboard->Create3DView() )
					{
						MessageBeep(MB_ICONSTOP);
						acedRestoreStatusBar();
						return;
					}
				}
			}

			for( i=0; i<m_ia3DBaysSelected.GetSize(); i++ )
			{
				pBay = GetBayFromBayNumber(m_ia3DBaysSelected.GetAt(i));
				if( pBay!=NULL )
				{
					if( !pBay->Create3DView() )
					{
						MessageBeep(MB_ICONSTOP);
						acedRestoreStatusBar();
						return;
					}
				}
			}
		}
		else
		{
			///////////////////////////////////////////////////
			//Runs
			for( i=0; i<GetNumberOfRuns(); i++ )
			{
				pRun  = m_raRuns.GetAt( i );
				if( !pRun->Create3DView( iPos ) )
				{
					MessageBeep(MB_ICONSTOP);
					acedRestoreStatusBar();
					return;
				}
			}

			///////////////////////////////////////////////////
			//Lapboards
			for( i=0; i<GetNumberOfLapboards(); i++ )
			{
				pLapboard = m_Lapboards.GetAt( i );
				if( !pLapboard->Create3DView() )
				{
					MessageBeep(MB_ICONSTOP);
					acedRestoreStatusBar();
					return;
				}
				acedSetStatusBarProgressMeterPos(iPos++);
			}

			///////////////////////////////////////////////////
			//Visual Components
			GetVisualComponents()->Create3DView(iPos);
		}
	}

	///////////////////////////////////////////////////
	//Finished
	acedSetStatusBarProgressMeterPos(iFull);
	MessageBeep(MB_ICONASTERISK);
	acedRestoreStatusBar();
}	//Create3DView()

/////////////////////////////////////////////////////////////////////////////////////////
//Delete3DView()
//
void Controller::Delete3DView()
{
	int			i;
	Run			*pRun;
	LapboardBay	*pLapboard;

#ifdef _DEBUG
	m_AutoBuildTool.Delete3DView();
#endif	//#ifdef _DEBUG

	///////////////////////////////////////////////////
	//Delete the Runs, bays, lifts etc
	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun  = m_raRuns.GetAt( i );
		pRun->Delete3DView();
	}

	///////////////////////////////////////////////////
	//Delete the lapboards
	for( i=0; i<GetNumberOfLapboards(); i++ )
	{
		pLapboard = m_Lapboards.GetAt( i );
		pLapboard->Delete3DView();
	}

	///////////////////////////////////////////////////
	//Visual Components
	GetVisualComponents()->Delete3DView();

	Destroy3DCrosshair();

//	RefreshScreen();
}	//Delete3DView()

/////////////////////////////////////////////////////////////////////////////////////////
//Create3DCrosshair()
//
void Controller::Create3DCrosshair()
{
	//Just like highlander, "There can be only one"!
	if( !IsCrosshairCreated() )
	{
		#ifdef	SHOW_3D_XHAIR
			int					iCrosshairColour;
			Point3D				pt;
			Adesk::Int16		mode;
			Acad::ErrorStatus	es;

			/////////////////////////////////////////////////////////////////////
			//Create the crosshair point at the origin
			pt.set( 0.00, 0.00, 0.00 );
			m_pCrosshair = (Entity*)((AcDbEntity*) new AcDbPoint(pt));

			/////////////////////////////////////////////////////////////////////
			//Set the point display mode
			mode = 66;
			es = acdbHostApplicationServices()->workingDatabase()->setPdmode( mode );

			/////////////////////////////////////////////////////////////////////
			//Colour
			iCrosshairColour = COLOUR_CROSSHAIR_3D_VIEW;
			m_pCrosshair->setColorIndex((Adesk::UInt16)iCrosshairColour);
			m_pCrosshair->postToDatabase( LAYER_NAME_3D );
			m_pCrosshair->AttachedReactors( &m_pCrosshairReactor );
			m_pCrosshairReactor->SetControllerPointer( this );
			
			/////////////////////////////////////////////////////////////////////
			//shift it to the origin
			m_pCrosshair->_topen();
			m_pCrosshair->transformBy( Get3DCrosshairTransform() );
			m_pCrosshair->close();
		#endif	//#ifdef	SHOW_3D_XHAIR

		//finished
		SetCrosshairCreated( true );
	}
}	//Create3DCrosshair()

/////////////////////////////////////////////////////////////////////////////////////////
//Destroy3DCrosshair()
//
void Controller::Destroy3DCrosshair()
{
	#ifdef	SHOW_3D_XHAIR

		Acad::ErrorStatus	es;

		if( m_pCrosshair!=NULL )
		{
			/////////////////////////////////////////////////////////////
			//Delete the entity
			es = m_pCrosshair->_topen();
			if( es==Acad::eOk )
			{
				m_pCrosshair->removePersistentReactor( m_pCrosshairReactor->objectId() );
				m_pCrosshair->erase();
			}
			else if( es==Acad::eWasErased )
			{
				//fine
				1;
			}
			else if( es==Acad::eWasOpenForNotify )
			{
				//They obviously deleted the crosshair directly
				1;
			}
			else
			{
				assert( false );
			}
			m_pCrosshair->close();
			m_pCrosshair = NULL;
		}

		if( m_pCrosshairReactor!=NULL )
		{
			/////////////////////////////////////////////////////////////
			//Delete the reactor
			es = m_pCrosshairReactor->_topen();
			if( es==Acad::eOk )
			{
				m_pCrosshairReactor->erase();
			}
			else if( es==Acad::eWasErased )
			{
				//fine
				1;
			}
			else
			{
				assert( false );
			}
			m_pCrosshairReactor->close();
			m_pCrosshairReactor = NULL;
		}
	#endif	//#ifdef	SHOW_3D_XHAIR

	SetCrosshairCreated( false );
}	//Destroy3DCrosshair()

/////////////////////////////////////////////////////////////////////////////////////////
//IsCrosshairCreated()
//
bool Controller::IsCrosshairCreated()
{
	return m_bCrosshairCreated;
}	//IsCrosshairCreated()

/////////////////////////////////////////////////////////////////////////////////////////
//SetCrosshairCreated(bool bCreated)
//
void Controller::SetCrosshairCreated(bool bCreated)
{
	m_bCrosshairCreated = bCreated;
}	//SetCrosshairCreated(bool bCreated)

/////////////////////////////////////////////////////////////////////////////////////////
//RefreshScreen()
//
void Controller::RefreshScreen()
{
	acedRedraw( NULL, 1 );
}	//RefreshScreen()

/////////////////////////////////////////////////////////////////////////////////////////
//RenumberAllBays()
//
void Controller::RenumberAllBays()
{
	int iRunID, iBayID;
	Run	*pRun;
	Bay	*pBay;

	for( iRunID=0; iRunID<GetNumberOfRuns(); iRunID++ )
	{
		pRun = GetRun(iRunID);
		for( iBayID=0; iBayID<pRun->GetNumberOfBays(); iBayID++ )
		{
			pBay = pRun->GetBay( iBayID );
			pBay->UpdateNumberText();
		}
	}
}	//RenumberAllBays()

/////////////////////////////////////////////////////////////////////////////////////////
//GetOverrun()
//
bool Controller::GetOverrun()
{
	return m_AutoBuildTool.GetMouseOverrun();
}	//GetOverrun()

/////////////////////////////////////////////////////////////////////////////////////////
//SetOverrun(bool bOverrun)
//
void Controller::SetOverrun(bool bOverrun)
{
	m_AutoBuildTool.SetMouseOverrun( bOverrun );
}	//SetOverrun(bool bOverrun)

/////////////////////////////////////////////////////////////////////////////////////////
//SelectBays( BayList &Bays )
//
bool Controller::SelectBays( BayList &Bays, bool bAllowMultipleSystem )
{
	int					i, j;
	Bay					*pBay;
	bool				bFound;
	ads_name			ss;
	BayList				WrongSystemBays;
	AcDbObjectIdArray	objectIds;

	Bays.RemoveAll();

	//get the user selection
	if( acedSSGet( NULL, NULL, NULL, NULL, ss )!=RTNORM)
		return false;

	//reduce the list only entities for lines
	selectLines( objectIds, ss );
//	selectGroups( objectIds, ss );

	if( bAllowMultipleSystem )
		Bays.SetMustBeSameSystem(false);

	WrongSystemBays.RemoveAll();

	//for each id
	for( i=0; i<objectIds.length(); i++ )
	{
		//obtain the bay corresponding to the line's id
		pBay = GetBayPointerForEntityId( objectIds[i] );
		if( pBay!=NULL )
		{
			//Go through each bay in the list of bays
			//	to ensure it is unique
			bFound = false;
			for( j=0; j<Bays.GetSize(); j++ )
			{
				if( pBay==Bays.GetAt(j) )
				{
					//if is already inserted
					bFound=true;
					break;
				}
				if( Bays.GetAt(j)->GetBayNumber()>pBay->GetBayNumber() )
				{
					if( !bAllowMultipleSystem && Bays.GetSystem()!=S_INVALID && pBay->GetSystem()!=Bays.GetSystem() )
					{
						//remove all the current bays from the list
						Bays.RemoveAll();
						//this is the only bay in the list now
						Bays.Add(pBay);
						bFound=true;
						//restart again
						i=0;
						break;
					}
					else
					{
						Bays.InsertAt( j, pBay );
						bFound=true;
						break;
					}
				}
			}
			if( !bFound )
			{
				if( !bAllowMultipleSystem && Bays.GetSystem()!=S_INVALID && pBay->GetSystem()!=Bays.GetSystem() )
				{
					WrongSystemBays.Add( pBay );	
				}
				else
				{
					Bays.Add( pBay );
				}
			}
		}
	}
	if( Bays.GetSize()>0 && WrongSystemBays.GetSize()>0 )
	{
		assert( !bAllowMultipleSystem );
		CString sMsg, sTemp;
		if( WrongSystemBays.GetSize()==1 )
		{
			pBay = WrongSystemBays.GetAt(0);
			sMsg.Format( _T("Bay %i was created using %s system, the rest\n"),
				pBay->GetBayNumber(), WrongSystemBays.GetSystem()==S_MILLS? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE );
			sTemp.Format( _T("of the selected bays were created using %s system.\n\n"),
				Bays.GetSystem()==S_MILLS? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE );
			sMsg+= sTemp;
			sMsg+= _T("You may only operate on one type at a time!\n");
			sTemp.Format( _T("Bay %i has been removed from the selection."), pBay->GetBayNumber() );
			sMsg+= sTemp;
		}
		else
		{
			CString sSystem;
			sSystem = WrongSystemBays.GetSystem()==S_MILLS? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE;
      sMsg.Format( _T("The following bays were created using %s system:\n"), sSystem );
			for( i=0; i<WrongSystemBays.GetSize()-1; i++ )
			{
				pBay = WrongSystemBays.GetAt(i);
				if( i==WrongSystemBays.GetSize()-2 )
					sTemp.Format( _T("%i & "), pBay->GetBayNumber() );
				else
					sTemp.Format( _T("%i, "), pBay->GetBayNumber() );
				sMsg+= sTemp;
			}
			sTemp.Format( _T("%i.\n"), WrongSystemBays[i]->GetBayNumber() );
			sMsg+= sTemp;
			sTemp.Format( _T("The other bays in the selection were created using\n%s system.\n\n"),
				Bays.GetSystem()==S_MILLS? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE );
			sMsg+= sTemp;
			sMsg+= _T("You may only operate on one type at a time!\n");
			sTemp.Format( _T("The %s bays have been removed from the selection."), sSystem );
			sMsg+= sTemp;
		}
		if( MessageBox( NULL, sMsg, _T("Invalid System Selection"), MB_OKCANCEL )==IDCANCEL )
			return false;
	}
	return (Bays.GetSize()>0);
}	//SelectBays( BayList &Bays )

/////////////////////////////////////////////////////////////////////////////////////////
//selectLines(AcDbObjectIdArray& idArray, ads_name selection)
//
Acad::ErrorStatus Controller::selectLines(AcDbObjectIdArray& idArray, ads_name selection)
{
	long length;
	acedSSLength(selection, &length);
	if(length == 0)
	{
		acedSSFree(selection);
		return Acad::eInvalidInput;
	}
	ads_name entName;
	AcDbObjectId oId;
	AcDbEntity* pEnt;
	Acad::ErrorStatus es;
	long rejected = 0;
	for(long i=0; i<length; i++)
	{
		acedSSName(selection, i, entName);
		acdbGetObjectId(oId, entName);
		es = acdbOpenAcDbEntity((AcDbEntity*&)pEnt, oId, AcDb::kForRead);
		if(es != Acad::eOk)
		{
			acedSSFree(selection);
			return es;
		}
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			// does not work with old style polyline
			// ray and infinite lines
			//
			if( !pEnt->isKindOf(AcDb2dPolyline::desc())	&&
				!pEnt->isKindOf(AcDb3dPolyline::desc())	&&
				!pEnt->isKindOf(AcDbRay::desc())		&&
				!pEnt->isKindOf(AcDbXline::desc())		)
			{
				idArray.append(oId);
			}
			else
				rejected++;

		}
		else
			rejected++;

		pEnt->close();
	}

	acedSSFree(selection);
	if(idArray.length() == 0)
		return Acad::eInvalidInput;

	return Acad::eOk;
}	//selectLines(AcDbObjectIdArray& idArray, ads_name selection)


/////////////////////////////////////////////////////////////////////////////////////////
//selectComponents(AcDbObjectIdArray& idArray, ads_name selection)
//
Acad::ErrorStatus Controller::selectComponents(AcDbObjectIdArray& idArray, ads_name selection)
{
	long length;
	acedSSLength(selection, &length);
	if(length == 0)
	{
		acedSSFree(selection);
		return Acad::eInvalidInput;
	}
	ads_name entName;
	AcDbObjectId oId;
	AcDbEntity* pEnt;
	Acad::ErrorStatus es;
	long rejected = 0;
	for(long i=0; i<length; i++)
	{
		acedSSName(selection, i, entName);
		acdbGetObjectId(
			oId, entName);
		es = acdbOpenAcDbEntity((AcDbEntity*&)pEnt, oId, AcDb::kForRead);
		if(es != Acad::eOk)
		{
			acedSSFree(selection);
			return es;
		}
		if( pEnt->isKindOf(AcDb3dSolid::desc()) ||
			pEnt->isKindOf(AcDbPolygonMesh::desc()))
		{
			// does not work with old style polyline
			// ray and infinite lines
			//
			if( !pEnt->isKindOf(AcDb2dPolyline::desc())	&&
				!pEnt->isKindOf(AcDb3dPolyline::desc())	&&
				!pEnt->isKindOf(AcDbRay::desc())		&&
				!pEnt->isKindOf(AcDbXline::desc())		)
			{
				idArray.append(oId);
			}
			else
				rejected++;

		}
		else
			rejected++;

		pEnt->close();
	}

	acedSSFree(selection);
	if(idArray.length() == 0)
		return Acad::eInvalidInput;

	return Acad::eOk;
}	//selectComponents(AcDbObjectIdArray& idArray, ads_name selection)

/////////////////////////////////////////////////////////////////////////////////////////
//selectGroups(AcDbObjectIdArray& idArray, ads_name selection)
//
Acad::ErrorStatus Controller::selectGroups(AcDbObjectIdArray& idArray, ads_name selection)
{
	long length;
	acedSSLength(selection, &length);
	if(length == 0)
	{
		acedSSFree(selection);
		return Acad::eInvalidInput;
	}
	ads_name entName;
	AcDbObjectId oId;
	AcDbEntity* pEnt;
	Acad::ErrorStatus es;
	long rejected = 0;
	for(long i=0; i<length; i++)
	{
		acedSSName(selection, i, entName);
		acdbGetObjectId(oId, entName);
		es = acdbOpenAcDbEntity((AcDbEntity*&)pEnt, oId, AcDb::kForRead);
		if(es != Acad::eOk)
		{
			acedSSFree(selection);
			return es;
		}
		if(pEnt->isKindOf(AcDbGroup::desc()))
		{
			// does not work with old style polyline
			// ray and infinite lines
			//
			if( !pEnt->isKindOf(AcDb2dPolyline::desc())	&&
				!pEnt->isKindOf(AcDb3dPolyline::desc())	&&
				!pEnt->isKindOf(AcDbRay::desc())		&&
				!pEnt->isKindOf(AcDbXline::desc())		)
			{
				idArray.append(oId);
			}
			else
				rejected++;

		}
		else
			rejected++;

		pEnt->close();
	}

#ifdef _DEBUG
	if(rejected > 0)
		acutPrintf(_T("%d entities filtered out.\n"), rejected);
#endif	//#ifdef _DEBUG

	acedSSFree(selection);
	if(idArray.length() == 0)
		return Acad::eInvalidInput;

	return Acad::eOk;
}	//selectGroups(AcDbObjectIdArray& idArray, ads_name selection)

/////////////////////////////////////////////////////////////////////////////////////////
//GetBayPointerForEntityId(AcDbObjectId &Id)
//
Bay * Controller::GetBayPointerForEntityId(AcDbObjectId &Id)
{
	Bay					*pBay;
	AcDbEntity			*pEnt;
	Acad::ErrorStatus	es, retStat;
	AcDbObjectIdArray	ids;

	pBay = NULL;
	es = acdbOpenAcDbEntity( pEnt, Id, AcDb::kForWrite );
  if (es != Acad::eOk) { if (es != Acad::eOnLockedLayer) acutPrintf(_T("GetBayPointerForEntityId: Error %s opening entity.\n"), acadErrorStatusText(es)); return pBay; }

	AcDbVoidPtrArray *pReactors;
	void             *pSomething;

	AcDbObjectId       persObjId;
	EntityReactor     *pPersReacObj;

	pReactors = pEnt->reactors();

	if (pReactors != NULL && pReactors->length() > 0)
	{
		for (int i = 0; i < pReactors->length(); i++)
		{
			pSomething = pReactors->at(i);

			// Is it a persistent reactor?
			//
			if (acdbIsPersistentReactor(pSomething))
			{
				persObjId = acdbPersistentReactorObjectId( pSomething);

				if((retStat=acdbOpenAcDbObject((AcDbObject*&)pPersReacObj, persObjId, AcDb::kForWrite))!=Acad::eOk)
				{
          acutPrintf(_T("\nFailure for openAcDbObject: retStat==%d\n"), retStat);
					return pBay;
				}

				pBay = pPersReacObj->GetBayPointer( );
				pPersReacObj->close();
				pEnt->close();
				return pBay;
			}
		}
	}
	pEnt->close();

	return pBay;
}	//GetBayPointerForEntityId(AcDbObjectId &Id)

/////////////////////////////////////////////////////////////////////////////////////////
//EditBay()
//
void Controller::EditBay()
{
	BayList	Bays;

	if( !SelectBays( Bays, false ) )
		return;

	CheckForStairLadder( &Bays );

	RemoveAndWarnOfCommittedBays( &Bays );

	//Size may have changed above!
	int		i, iSize, iBayDetailsDialogAction;
	iSize = Bays.GetSize();

	/////////////////////////////////////////////////////////////////////////////
	//Perform the edit!
	Bay		*pBay, *pSelectedBay;
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		pBay = new Bay();
		pBay->SetRunPointer(Bays[0]->GetRunPointer());
		pSelectedBay = Bays[0];
		*pBay = *pSelectedBay; // copy bay to temporary bay

		pBay->SetStandardPosition( pSelectedBay->GetStandardPosition( CNR_NORTH_EAST ), CNR_NORTH_EAST );
		pBay->SetStandardPosition( pSelectedBay->GetStandardPosition( CNR_SOUTH_EAST ), CNR_SOUTH_EAST );
		pBay->SetStandardPosition( pSelectedBay->GetStandardPosition( CNR_SOUTH_WEST ), CNR_SOUTH_WEST );
		pBay->SetStandardPosition( pSelectedBay->GetStandardPosition( CNR_NORTH_WEST ), CNR_NORTH_WEST );

		//The selected bay may not own some of the standards, so copy them through
		doubleArray daArrangement;
		bool		bSoleboard;
		if( pSelectedBay->GetBackward()!=NULL )
		{
			//North West
			pSelectedBay->GetBackward()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
			bSoleboard = pSelectedBay->GetBackward()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
			pBay->SetStandardsArrangement( daArrangement, CNR_NORTH_WEST, bSoleboard );

			//South West
			if( pSelectedBay->GetBackward()->GetInner()!=NULL )
			{
				pSelectedBay->GetBackward()->GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
				bSoleboard = pSelectedBay->GetBackward()->GetInner()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
				pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_WEST, bSoleboard );
			}
			else if( pSelectedBay->GetInner()!=NULL )
			{
				pSelectedBay->GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_WEST );
				bSoleboard = pSelectedBay->GetInner()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_WEST );
				pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_WEST, bSoleboard );
			}
			else
			{
				pSelectedBay->GetBackward()->GetStandardsArrangement( daArrangement, CNR_SOUTH_EAST );
				bSoleboard = pSelectedBay->GetBackward()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SOUTH_EAST );
				pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_WEST, bSoleboard );
			}
		}

		//South East
		if( pSelectedBay->GetInner()!=NULL )
		{
			pSelectedBay->GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
			bSoleboard = pSelectedBay->GetInner()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
			pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_EAST, bSoleboard );

			//South West
			if( pSelectedBay->GetBackward()!=NULL )
			{
				//already done, above
			}
			else
			{
				if( pSelectedBay->GetInner()->GetBackward()!=NULL )
				{
					pSelectedBay->GetInner()->GetBackward()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
					bSoleboard = pSelectedBay->GetInner()->GetBackward()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
					pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_WEST, bSoleboard );
				}
				else
				{
					pSelectedBay->GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_WEST );
					bSoleboard = pSelectedBay->GetInner()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_WEST );
					pBay->SetStandardsArrangement( daArrangement, CNR_SOUTH_WEST, bSoleboard );
				}
			}
		}

		pBay->SetID(ID_INVALID); // set as non zero as dialog uses bay id = 0 to display 
						// as autobuild buttonsds


		////////////////////////////////////////////////////////////////
		//Control Bay dimensioning
		double	dWidth, dLength, dPreviousL, dPreviousW;

		dWidth	= Bays[0]->GetBayWidth();
		dPreviousW	= dWidth;
		dLength	= Bays[0]->GetBayLength();
		dPreviousL	= dLength;
		pBay->SetBaySizeMultipleSelection( false );
		pBay->SetBayLengthChangedDuringEdit(false);
		pBay->SetBayWidthChangedDuringEdit(false);
		for( i=0; i<iSize; i++ )
		{
			pSelectedBay = Bays[i];
			if( dWidth	!= pSelectedBay->GetBayWidth()	||
				dLength	!= pSelectedBay->GetBayLength()	)
				pBay->SetBaySizeMultipleSelection( true );
		}

		iBayDetailsDialogAction = pBay->DisplayBayDetailsDialog(); 

		switch( iBayDetailsDialogAction )
		{
		case(APPLY_TO_SELECTION):
			ApplyToSelection( pBay, &Bays );
			break;
		case( IDCANCEL ):
		case( CANCELLED ):
			//Do nothing
			break;
		default:
			assert( false );
			break;
		}

		DELETE_PTR(pBay);
	}
}	//EditBay()

/////////////////////////////////////////////////////////////////////////////////////////
//EditRun()
//
void Controller::EditRun()
{
	int		iSize, i;
	Run		*pRun;
	CString	sMsg, sText;
	BayList	Bays;

	if( !SelectBays( Bays, false ) )
		return;

	CheckForStairLadder( &Bays, false );

	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();

	if( iSize>0 )
	{
		pRun = Bays[0]->GetRunPointer();
	
		//Check the selection contains only one run!
		for( i=1; i<iSize; i++ )
		{
			if( pRun!=Bays[i]->GetRunPointer() )
			{
				sMsg.Format( _T("Bay %i is not part of the same run as Bay %i"),
							Bays[i]->GetBayNumber(), Bays[0]->GetBayNumber() );
				sMsg+= _T("\nYou may only select one run at a time to edit!");
				sText.Format( _T("\n\nI will only edit Run %i, which contains Bay %i."),
							Bays[0]->GetRunPointer()->GetRunID()+1, Bays[0]->GetBayNumber() );
				sMsg+= sText;

				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox( NULL, sMsg, _T("Run Selection Error"), MB_OK );
				break;
			}
		}

		//Store some Variables
		RunTemplate	 RT;
		RT = *(pRun->GetRunTemplate());
		bool bUseLongerBracing = pRun->GetUseLongerBracing();
		//we are only interested in the first run!
		RunPropDialog *pRunPropDlg;
		pRunPropDlg = new RunPropDialog(NULL, NULL, pRun);

		pRunPropDlg->m_dLength2400 = pRun->GetBracingLength( COMPONENT_LENGTH_2400 );
		pRunPropDlg->m_dLength1800 = pRun->GetBracingLength( COMPONENT_LENGTH_1800 );
		pRunPropDlg->m_dLength1200 = pRun->GetBracingLength( COMPONENT_LENGTH_1200 );
		pRunPropDlg->m_dLength0700 = pRun->GetBracingLength( COMPONENT_LENGTH_0700 );

		int	iRet;
		bool bRetry;
		bRetry = true;
		while( bRetry )
		{
			iRet = pRunPropDlg->DoModal();
			
			switch( iRet )
			{
			case( IDRETRY ):
				bRetry = true;
				break;
			default:			//fallthrough
			case( IDCANCEL ):
				*(pRun->GetRunTemplate()) = RT;
				bRetry = false;
				break;
			case( IDOK ):

				///////////////////////////////////////////////////////////////
				//apply changes! to the run
				pRun->SetAllowDrawBays(false);

				///////////////////////////////////////////////////////////////
				//Apply changes to bracing
				if( RT.GetBaysPerBrace() != pRun->GetRunTemplate()->GetBaysPerBrace() ||
					pRunPropDlg->m_dLength2400<pRun->GetBracingLength( COMPONENT_LENGTH_2400 )-ROUND_ERROR ||
					pRunPropDlg->m_dLength2400>pRun->GetBracingLength( COMPONENT_LENGTH_2400 )+ROUND_ERROR ||
					pRunPropDlg->m_dLength1800<pRun->GetBracingLength( COMPONENT_LENGTH_1800 )-ROUND_ERROR ||
					pRunPropDlg->m_dLength1800>pRun->GetBracingLength( COMPONENT_LENGTH_1800 )+ROUND_ERROR ||
					pRunPropDlg->m_dLength1200<pRun->GetBracingLength( COMPONENT_LENGTH_1200 )-ROUND_ERROR ||
					pRunPropDlg->m_dLength1200>pRun->GetBracingLength( COMPONENT_LENGTH_1200 )+ROUND_ERROR ||
					pRunPropDlg->m_dLength0700<pRun->GetBracingLength( COMPONENT_LENGTH_0700 )-ROUND_ERROR ||
					pRunPropDlg->m_dLength0700>pRun->GetBracingLength( COMPONENT_LENGTH_0700 )+ROUND_ERROR )
				{
					pRun->SetBracingLength( COMPONENT_LENGTH_2400, pRunPropDlg->m_dLength2400 );
					pRun->SetBracingLength( COMPONENT_LENGTH_1800, pRunPropDlg->m_dLength1800 );
					pRun->SetBracingLength( COMPONENT_LENGTH_1200, pRunPropDlg->m_dLength1200 );
					pRun->SetBracingLength( COMPONENT_LENGTH_0700, pRunPropDlg->m_dLength0700 );
					pRun->SetBaysWithBracing();
				}

				///////////////////////////////////////////////////////////////
				//Apply changes to Ties
				if( RT.GetBaysPerTie()			!= pRun->GetRunTemplate()->GetBaysPerTie() ||
					RT.GetTiesVerticallyEvery()	!= pRun->GetRunTemplate()->GetTiesVerticallyEvery() ||
					RT.GetTieType()				!= pRun->GetRunTemplate()->GetTieType() )
				{
					pRun->SetBaysWithTies();
				}

				///////////////////////////////////////////////////////////////
				//Apply changes to RLs
				if( RT.GetRLStart() != pRun->GetRunTemplate()->GetRLStart() ||
					RT.GetRLEnd()	!= pRun->GetRunTemplate()->GetRLEnd() )
				{
					Point3D	ptRLStart, ptRLEnd;
					pRun->GetRL( ptRLStart, ptRLEnd );
					pRun->SetRL( ptRLStart, ptRLEnd );
				}

				///////////////////////////////////////////////////////////////
				//refresh the schematic
				pRun->SetAllowDrawBays(true);
				pRun->UpdateSchematicView();

				bRetry = false;
				break;
			}
		}
		DELETE_PTR( pRunPropDlg );
	}
}	//EditRun()

/////////////////////////////////////////////////////////////////////////////////////////
//InsertBay()
//
bool Controller::InsertBay()
{
	int				i;
	bool			bIllegal, bWidthChanged, bLengthChanged, bHeightChanged;
	double			dWidth, dLength, dHeight, dWidthActual, dLengthActual;
	BayList			Bays;
	SideOfBayEnum	eSide;

	acutPrintf( _T("\nSelect Bay to attach to!") );
	if( !SelectBays( Bays, true ) )
		return false;

	CheckForStairLadder( &Bays, false, true );

	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return false;

		bool bApplyToRuns = false;
		if( !GetApplyToRunOrBay( bApplyToRuns ) )
			return false;

		dWidth			= Bays[0]->GetBayWidth();
		dWidthActual	= Bays[0]->GetBayWidthActual();
		dLength			= Bays[0]->GetBayLength();
		dLengthActual	= Bays[0]->GetBayLengthActual();
/*
		dHeight = Bays[0]->GetBayHeight();
*/
		dHeight = 0.00;
		DimensionSelectionEnum eDim;

		eDim = DS_LENGTH;
		if( eSide==NORTH || eSide==SOUTH )
			eDim = DS_WIDTH;
			
		bWidthChanged	= false;
		bLengthChanged	= false;
		bHeightChanged	= false;
		if( !GetSizeOfBayFromCommandLine( dWidth, dLength, dHeight, bWidthChanged, bLengthChanged, bHeightChanged, eDim ) )
			return false;

		if( bApplyToRuns )
		{
			Bay		*pBay;
			BayList	blOriginalBays;
			blOriginalBays = Bays;
			Bays.SetUniqueBaysOnly(true);
			for( i=0; i< blOriginalBays.GetSize(); i++ )
			{
				//Backward
				pBay = blOriginalBays.GetAt(i);
				while( pBay->GetBackward()!=NULL )
				{
					pBay = pBay->GetBackward();
					Bays.Add( pBay );
				}
				//Forward
				pBay = blOriginalBays.GetAt(i);
				while( pBay->GetForward()!=NULL )
				{
					pBay = pBay->GetForward();
					Bays.Add( pBay );
				}
			}
		}

		double dNewWidth, dNewLength;
		dNewWidth	= dWidth;
		dNewLength	= dLength;
		for( i=0; i<Bays.GetSize(); i++ )
		{
			bIllegal = false;
			switch( eSide )
			{
			case( NORTH ):
				if( Bays[i]->GetOuter()!=NULL )
					bIllegal = true;
				break;
			case( EAST ):
				//need to test for a birdcage here!
				if( Bays[i]->GetForward()!=NULL )
				{
					if( Bays[i]->GetForward()->GetOuter()!=NULL )
					{
						if( Bays[i]->GetForward()->GetOuter()->GetBackward()!=NULL )
							bIllegal = true;
					}
					else if( Bays[i]->GetForward()->GetInner()!=NULL )
					{
						if( Bays[i]->GetForward()->GetInner()->GetBackward()!=NULL )
							bIllegal = true;
					}
				}
				break;
			case( SOUTH ):
				if( Bays[i]->GetInner()!=NULL )
					bIllegal = true;
				break;
			case( WEST ):
				//need to test for a birdcage here!
				if( Bays[i]->GetBackward()!=NULL )
				{
					if( Bays[i]->GetBackward()->GetOuter()!=NULL )
					{
						if( Bays[i]->GetBackward()->GetOuter()->GetForward()!=NULL )
							bIllegal = true;
					}
					else if( Bays[i]->GetBackward()->GetInner()!=NULL )
					{
						if( Bays[i]->GetBackward()->GetInner()->GetForward()!=NULL )
							bIllegal = true;
					}
				}
				break;
			default:
				assert( false );
				return false;
			}
			if( bIllegal )
			{
				CString sMsg;
				MessageBeep(MB_ICONEXCLAMATION);
        sMsg.Format( _T("\nError:  Bay%i already has a bay to the %s.  I will skip this bay!"), Bays[i]->GetBayNumber(), GetSideOfBayDescStr( eSide ) );
				MessageBox( NULL, sMsg, _T("Bay already exists!"), MB_OK|MB_ICONSTOP );
				continue;
			}

			dWidth	= Bays[i]->GetBayWidth();
			dLength	= Bays[i]->GetBayLength();
			dWidthActual	= Bays[i]->GetBayWidthActual();
			dLengthActual	= Bays[i]->GetBayLengthActual();
			if( bWidthChanged )
			{
				dWidth			= dNewWidth;
				dWidthActual	= Bays[i]->GetBayDimensionActual( dNewWidth, false, false );
			}
			if( bLengthChanged )
			{
				dLength			= dNewLength;
				dLengthActual	= Bays[i]->GetBayDimensionActual( dNewLength, true, false );
			}

			InsertAndRedimensionBay( Bays[i], eSide, dWidth, dLength, dWidthActual, dLengthActual, bWidthChanged, bLengthChanged );
		}
	}
	return false;
}	//InsertBay()

//////////////////////////////////////////////////////////////////
//Serialize
//
//Storage and loading function

/////////////////////////////////////////////////////////////////////////////////////////
//Serialize(CArchive &ar)
////
void Controller::Serialize(CArchive &ar)
{
	int					iRunNumber, iNumberOfRuns, iNumberOfComponents,
						iComponentNumber, iNumberOfLapboards, iLapboardNumber,
						i, iSize, iDummy;
	int					ikrlIndex;
	Run					*pRun;
	bool				bView3DCreated, bViewMatrixCreated, bMills, bDummy;
	BOOL				BTemp;
	double				dRL, dDummy;
	CString				sStage, ARXVersion, sName, sFile, sDummy;
	intArray			iaDummy;
	Matrix3D			Transform, Trans, DummyTrans;
	Component			*pComponent;
	doubleArray			daRLs;
	LapboardBay			*pLapboard;
	CStringArray		saSaveHistory;
	COleDateTime		date;
	AutoBuildTools		abtDummy;
	JobDescriptionInfo	jdiDummy;
	BayStageLevelArray	bslaDummy;


	if (ar.IsStoring())    // Store Object?
	{
		ar << CONTROLLER_VERSION_LATEST;

		//CONTROLLER_VERSION_1_0_24
		bDummy = IsRLCalcPossible();
		STORE_bool_IN_AR(bDummy);
		if( bDummy )
		{
			iDummy = KRL_SIZE;
			ar << iDummy;	
			for( ikrlIndex=(int)KRL_1ST; ikrlIndex<(int)KRL_SIZE; ikrlIndex++ )
			{
				ar << m_ptKwikRLs[ikrlIndex].x;
				ar << m_ptKwikRLs[ikrlIndex].y;
				ar << m_ptKwikRLs[ikrlIndex].z;
			}
		}

		//CONTROLLER_VERSION_1_0_23
		GetStockList()->Serialize(ar);

		//NOTE 20001031 JSB - There is currently no way to set this
		//	variable(m_bIgnorPerRoll) except programmatically it is
		//	used to create a template, where the PerRoll stuff
		//	should not be read from the file
		bDummy = m_bIgnorPerRoll;	
		STORE_bool_IN_AR(bDummy);
		bDummy = CalcPerRollShadeCloth();
		STORE_bool_IN_AR(bDummy);
		bDummy = CalcPerRollChainMesh();
		STORE_bool_IN_AR(bDummy);
		dDummy = GetRollSizeShadeCloth();
		ar << dDummy;
		dDummy = GetRollSizeChainMesh();
		ar << dDummy;
		sDummy = GetPartNumberShadeCloth();
		ar << sDummy;
		sDummy = GetPartNumberChainMesh();
		ar << sDummy;
		iDummy = (int)GetRoundingDivisorShadeCloth();
		ar << iDummy;
		iDummy = (int)GetRoundingDivisorChainMesh();
		ar << iDummy;

		//CONTROLLER_VERSION_1_0_22
		m_StageNames.Serialize(ar);

		//CONTROLLER_VERSION_1_0_21
		m_vcComponents.Serialize(ar);

		//CONTROLLER_VERSION_1_0_20
		bDummy = GetConvertToSystemOnNextOpen();
		STORE_bool_IN_AR(bDummy);

		GetUsersName( sName );
		date = COleDateTime::GetCurrentTime();
		GetDatabaseFilename( sFile );
    ARXVersion.Format( _T("%s, %i/%i/%i, %i:%i, %s, %s, %s"), MECCANO_LATEST_VERSION,
							date.GetDay(), date.GetMonth(), date.GetYear(),
							date.GetHour(), date.GetMinute(),
							sName, sFile, (GetSystem()==S_MILLS)? _T("Mills"): _T("KwikStage") );
		m_saSaveHistory.Add(ARXVersion);

		iSize = m_saSaveHistory.GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			ARXVersion = m_saSaveHistory.GetAt(i);
			ar << ARXVersion;
		}	

		//CONTROLLER_VERSION_1_0_19
		bDummy = GetDisplayZigZagLine();
		STORE_bool_IN_AR(bDummy);

		//CONTROLLER_VERSION_1_0_18
		bDummy = GetShowStandardLengths();
		STORE_bool_IN_AR(bDummy);

		//CONTROLLER_VERSION_1_0_17
		ar << m_iSuggestedCutThroughLabel;
		bDummy = GetUse0700TransomForLedger();
		STORE_bool_IN_AR(bDummy);
		bDummy = GetUse1200TransomForLedger();
		STORE_bool_IN_AR(bDummy);

		//CONTROLLER_VERSION_1_0_16
		bDummy = GetShowMatrixLabelsOnSchematic();
		STORE_bool_IN_AR(bDummy);
		
		//CONTROLLER_VERSION_1_0_15
		bDummy = GetUse1500Soleboards();
		STORE_bool_IN_AR(bDummy);
		
		//CONTROLLER_VERSION_1_0_14
		bDummy = GetUsePutLogClipsInLadder();
		STORE_bool_IN_AR(bDummy);
		
		//CONTROLLER_VERSION_1_0_13
		iSize = m_ia3DBaysSelected.GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			iDummy = m_ia3DBaysSelected.GetAt(i);
			ar << iDummy;
		}
		iSize = m_iaMatrixBaysSelected.GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			iDummy = m_iaMatrixBaysSelected.GetAt(i);
			ar << iDummy;
		}
		STORE_bool_IN_AR(m_bShowTypicalMatrixSectionsOnly);

		//CONTROLLER_VERSION_1_0_12
		iDummy = (int)GetUseActualComponents();
		ar << iDummy;

		//CONTROLLER_VERSION_1_0_11
		ARXVersion = MECCANO_LATEST_VERSION;
		ar << ARXVersion;

		//CONTROLLER_VERSION_1_0_10
		m_iaInvisibleMatrixElement.Serialize(ar);

		//CONTROLLER_VERSION_1_0_9
		STORE_bool_IN_AR(m_bDisplaySalePrice);
		
		//CONTROLLER_VERSION_1_0_8
		STORE_bool_IN_AR(m_bColourByStageLevel);

		//CONTROLLER_VERSION_1_0_7
		ar << m_pt3DPosition.x;
		ar << m_pt3DPosition.y;
		ar << m_pt3DPosition.z;
		ar << m_ptMatrixPosition.x;
		ar << m_ptMatrixPosition.y;
		ar << m_ptMatrixPosition.z;

		//CONTROLLER_VERSION_1_0_6
		STORE_bool_IN_AR(m_bShowLongLevelLength);
		STORE_bool_IN_AR(m_bShowFirstScale);
		STORE_bool_IN_AR(m_bShowScaleLines);

		iSize = m_daLevels.GetSize();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			dRL = m_daLevels.GetLevel(i);
			ar << dRL;
		}

		bMills = GetSystem()==S_MILLS;
		STORE_bool_IN_AR(bMills);

		///////////////////////////////////
		bView3DCreated = IsCrosshairCreated();     
		bViewMatrixCreated=false;
		if( GetMatrix()!=NULL && GetMatrix()->IsCrosshairCreated() )
			bViewMatrixCreated = true;     

		STORE_bool_IN_AR(bView3DCreated);
		STORE_bool_IN_AR(bViewMatrixCreated);
		
		if( bViewMatrixCreated )
		{
			Transform = GetMatrix()->GetMatrixCrosshairTransform();
			Transform.Serialize(ar);
		}

		// Run List Storage - The list of runs, not including the Autobuild run
		if( !SaveSelectedOnly() )
		{
			iNumberOfRuns = GetNumberOfRuns();			// store number of runs
		}
		else
		{
			iNumberOfRuns = 0;
			for( iRunNumber=0; iRunNumber <GetNumberOfRuns(); iRunNumber++ )
			{
				pRun = m_raRuns.GetAt(iRunNumber);
				if( pRun->IsAnyBaySelected() )
				{
					iNumberOfRuns++;
				}
			}
		}
		ar << iNumberOfRuns;
		if (iNumberOfRuns > 0)
		{
			for( iRunNumber=0; iRunNumber<GetNumberOfRuns(); iRunNumber++ )
			{
				pRun = m_raRuns.GetAt(iRunNumber);
				if( !SaveSelectedOnly() )
				{
					pRun->Serialize(ar);				// store each run
				}
				else
				{
					if( pRun->IsAnyBaySelected() )
					{
						pRun->Serialize(ar);				// store each run
					}
				}
			}
		}
		// End Run List Storage

		m_3DCrosshairTransform.Serialize(ar);			//The Transformation matrix for the 3D View
		m_AutoBuildTool.Serialize(ar);	//The autobuild run and bay template
		
		// JobDescription Storage
		m_JobDescripInfo.Serialize(ar);

		// BOM Extra Component List Storage - These are the components that are owned by this bay
		if( !SaveSelectedOnly() )
		{
			iNumberOfComponents = m_BOMExtraCompList.GetNumberOfComponents();	// store number of components
		}
		else
		{
			iNumberOfComponents = 0;
		}
		ar << iNumberOfComponents;
		if (iNumberOfComponents > 0)
		{
			for( iComponentNumber=0; iComponentNumber<iNumberOfComponents; iComponentNumber++ )
			{
				pComponent	= m_BOMExtraCompList.GetComponent(iComponentNumber);
				pComponent->Serialize(ar);				// store each component
			}
		}
		// BOM Extra End Component List Storage

		if( !SaveSelectedOnly() )
		{
			iNumberOfLapboards = m_Lapboards.GetSize();
		}
		else
		{
			iNumberOfLapboards = 0;
			for( iLapboardNumber=0; iLapboardNumber<m_Lapboards.GetSize(); iLapboardNumber++ )
			{
				pLapboard = m_Lapboards.GetAt(iLapboardNumber);
				if( pLapboard->IsBaySelected() )
				{
					iNumberOfLapboards = m_Lapboards.GetSize();
				}
			}
		}
		ar << iNumberOfLapboards;
		for( iLapboardNumber=0; iLapboardNumber<m_Lapboards.GetSize(); iLapboardNumber++ )
		{
			pLapboard = m_Lapboards.GetAt(iLapboardNumber);
			if( !SaveSelectedOnly() ||
				pLapboard->IsBaySelected() )
			{
				pLapboard->Serialize(ar);
			}
		}			

		///////////////////////////////////
		//bay stage list array
		m_StageListArray.Serialize( ar );
	}
	else					// or Load Object?
	{
#ifdef _DEBUG
		CTime		TimeStart, TimeMatrix, TimeFin, TimeRuns, TimeBOMExtra, TimeLap, Time3D;
		CTimeSpan	TimeTaken;
		TimeStart = CTime::GetCurrentTime();

		for( i=0; i<10; i++ )
			giaLoopCounter[i] = 0;
		gsaLoopCounter[0] = _T("Component");
		int iNumberOfCounter = 1;

#endif //_DEBUG


		bool			bIgnorPerRoll;
		Entity			TempEntity;
		CString			sMsg, sTemp;
		intArray		ia3DBays, iaMatrixBays;
		VersionNumber	uiVersion;

		bIgnorPerRoll = false;
		if( !IsInsertingBlock() )
		{
			SetColourByStageLevel( false );

			m_ptMatrixPosition = DEFAULT_MATRIX_POSITION;
			m_pt3DPosition = DEFAULT_3D_POSITION;

			m_bShowLongLevelLength	= false;
			m_bShowFirstScale	= false;
			m_bShowScaleLines	= false;

			m_iaInvisibleMatrixElement.RemoveAll();

			m_daLevels.RemoveAll();

			SetIsOpeningDocument(true);
			if( IsDeleteSchematic() )
			{
				TempEntity.DeleteAllDatabases( true, false );
			}
			else
			{
				acutPrintf( _T("\nThe schematic will not be deleted during this load, type 'DS' to change this setting!") );
			}
			SetIsOpeningDocument(false);

			CreateLayers();
			daRLs.RemoveAll();
			SetUseActualComponents( UAC_NEITHER );
			m_ia3DBaysSelected.RemoveAll();
			m_iaMatrixBaysSelected.RemoveAll();
			ia3DBays.RemoveAll();
			iaMatrixBays.RemoveAll();
			SetUsePutLogClipsInLadder(false);
			SetUse1500Soleboards(false);
			SetShowMatrixLabelsOnSchematic(true);
			SetUse0700TransomForLedger(true);
			SetUse1200TransomForLedger(true);
			m_iSuggestedCutThroughLabel = 0;
			SetShowStandardLengths(true);
			SetDisplayZigZagLine(true);
			SavedHistoryClear();
			SetConvertToSystemOnNextOpen(false);

			//Shade & chain per roll default settings
			ReadPerRollFromRegistry();
/*
			SetCalcPerRollShadeCloth(false);
			SetCalcPerRollChainMesh(false);
			SetRollSizeChainMesh(0.00);
			SetRollSizeShadeCloth(0.00);
			SetPartNumberChainMesh("");
			SetPartNumberShadeCloth("");
			SetRoundingDivisorShadeCloth(RD_WHOLE);

  SetRoundingDivisorChainMesh(RD_WHOLE);
*/
		}

		ar >> uiVersion;
		switch (uiVersion)
		{
		case(CONTROLLER_VERSION_1_0_24):
			LOAD_bool_IN_AR(bDummy);
			if( bDummy )
			{
				ar >> iDummy;	
				assert( iDummy>=0 && iDummy<=KRL_SIZE );
				for( ikrlIndex=(int)KRL_1ST; ikrlIndex<iDummy; ikrlIndex++ )
				{
					ar >> dDummy;
					if( !IsInsertingBlock() )
					{
						m_ptKwikRLs[ikrlIndex].x = dDummy;
					}
					ar >> dDummy;
					if( !IsInsertingBlock() )
					{
						m_ptKwikRLs[ikrlIndex].y = dDummy;
					}
					ar >> dDummy;
					if( !IsInsertingBlock() )
					{
						m_ptKwikRLs[ikrlIndex].z = dDummy;
					}
				}
				if( !IsInsertingBlock() )
				{
					SetRLCalcPossible(true);
				}
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_23):
			//We need to load the extra stock items even if we are inserting a block
			GetStockList()->Serialize(ar);

			LOAD_bool_IN_AR(bIgnorPerRoll);

			LOAD_bool_IN_AR(bDummy);
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetCalcPerRollShadeCloth(bDummy);
			}
			LOAD_bool_IN_AR(bDummy);
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetCalcPerRollChainMesh(bDummy);
			}
			ar >> dDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetRollSizeShadeCloth(dDummy);
			}
			ar >> dDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetRollSizeChainMesh(dDummy);
			}
			ar >> sDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetPartNumberShadeCloth(sDummy);
			}
			ar >> sDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetPartNumberChainMesh(sDummy);
			}
			ar >> iDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetRoundingDivisorShadeCloth((RoundingDevisorEnum)iDummy);
			}
			ar >> iDummy;
			if( !bIgnorPerRoll && !IsInsertingBlock() )
			{
				SetRoundingDivisorChainMesh((RoundingDevisorEnum)iDummy);
			}

			//fallthrough

		case(CONTROLLER_VERSION_1_0_22):
			m_StageNames.Serialize(ar);
			//fallthrough

		case(CONTROLLER_VERSION_1_0_21):
			m_vcComponents.Serialize(ar);
			//fallthrough

		case(CONTROLLER_VERSION_1_0_20):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetConvertToSystemOnNextOpen(bDummy);
			}

			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> ARXVersion;
				if( !IsInsertingBlock() )
				{
					m_saSaveHistory.Add(ARXVersion);
				}
			}	
			//fallthrough

		case(CONTROLLER_VERSION_1_0_19):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetDisplayZigZagLine(bDummy);
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_18):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetShowStandardLengths(bDummy);
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_17):
			ar >> iDummy;
			if( !IsInsertingBlock() )
			{
				m_iSuggestedCutThroughLabel = iDummy;
			}
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetUse0700TransomForLedger(bDummy);
			}
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetUse1200TransomForLedger(bDummy);
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_16):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetShowMatrixLabelsOnSchematic(bDummy);
			}
			//fallthrough
		
		case(CONTROLLER_VERSION_1_0_15):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetUse1500Soleboards(bDummy);
			}
			//fallthrough
		
		case(CONTROLLER_VERSION_1_0_14):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				SetUsePutLogClipsInLadder(bDummy);
			}
			//fallthrough
		
		case(CONTROLLER_VERSION_1_0_13):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> iDummy;
				if( !IsInsertingBlock() )
				{
					ia3DBays.Add( iDummy );
				}
			}
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> iDummy;
				if( !IsInsertingBlock() )
				{
					iaMatrixBays.Add( iDummy );
				}
			}
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bShowTypicalMatrixSectionsOnly = bDummy;
			}

			//fallthrough

		case(CONTROLLER_VERSION_1_0_12):
			ar >> iDummy;
			if( !IsInsertingBlock() )
			{
				SetUseActualComponents( (UseActualComponentsEnum)iDummy );
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_11):
			ar >> ARXVersion;
			//fallthrough

		case(CONTROLLER_VERSION_1_0_10):
			if( uiVersion>=CONTROLLER_VERSION_1_0_11 )
			{
				assert( !ARXVersion.IsEmpty() );
        acutPrintf( _T("\nThis file was last saved with Kwikscaf ARX version: %s"), ARXVersion );
			}
			else
			{
				acutPrintf( _T("\nThis file was last saved Kwikscaf ARX version %s or earlier"), MECCANO_VERSION_1_4_08_3 );
			}

			if( !IsInsertingBlock() )
			{
				m_iaInvisibleMatrixElement.Serialize(ar);
			}
			else
			{
				iaDummy.Serialize(ar);
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_9):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bDisplaySalePrice = bDummy;
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_8):
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bColourByStageLevel = bDummy;
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_7):
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_pt3DPosition.x = dDummy;
			}
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_pt3DPosition.y = dDummy;
			}
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_pt3DPosition.z = dDummy;
			}
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_ptMatrixPosition.x = dDummy;
			}
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_ptMatrixPosition.y = dDummy;
			}
			ar >> dDummy;
			if( !IsInsertingBlock() )
			{
				m_ptMatrixPosition.z = dDummy;
			}
			//fallthrough

		case(CONTROLLER_VERSION_1_0_6):

			/////////////////////////////////////////////////////////////////////
			//Matrix appearance
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bShowLongLevelLength = bDummy;
			}
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bShowFirstScale = bDummy;
			}
			LOAD_bool_IN_AR(bDummy);
			if( !IsInsertingBlock() )
			{
				m_bShowScaleLines = bDummy;
			}

			/////////////////////////////////////////////////////////////////////
			//RL's
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> dRL;
				daRLs.Add( dRL );
			}

			/////////////////////////////////////////////////////////////////////
			//The default for this value is already set by the
			//	constructor of CompDetails, so no need to do it again
			//	for version <CONTROLLER_VERSION_1_0_3
			LOAD_bool_IN_AR(bMills);
			if( !IsInsertingBlock() )
			{
				SetSystem( bMills? S_MILLS: S_KWIKSTAGE );
			}

			/////////////////////////////////////////////////////////////////////
			//View settings
			LOAD_bool_IN_AR(bView3DCreated);
			LOAD_bool_IN_AR(bViewMatrixCreated);

			if( bViewMatrixCreated )
			{
				Transform.Serialize(ar);
			}

			/////////////////////////////////////////////////////////////////////
			//Run List Retrieval - The list of runs, not including the Autobuild run
			ar >> iNumberOfRuns; // get number of runs

			///////////////////////////////////////////
			//Progress Bar init
			acedSetStatusBarProgressMeter( _T("Loading Runs"), 0, iNumberOfRuns );
			if (iNumberOfRuns > 0)
			{
				for( iRunNumber=0; iRunNumber < iNumberOfRuns; iRunNumber++ )
				{
					acedSetStatusBarProgressMeterPos(iRunNumber);

					pRun = new Run();
					AddRun(pRun);
					pRun->Serialize(ar);				// load each run
					pRun->SetRunID(iRunNumber);
				}
			}
			acedSetStatusBarProgressMeterPos(iNumberOfRuns);
			acedRestoreStatusBar();
#ifdef _DEBUG
			TimeRuns = CTime::GetCurrentTime();
			TimeTaken = TimeRuns - TimeStart;
      acutPrintf( _T("\nKwikScaf Load Runs Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );
#endif //_DEBUG
			// End Run List Retrieval

			/////////////////////////////////////////////////////////////////////
			//We have now loaded the Bays, so lets find out which Bay belongs to 
			//	these bay numbers
			for( i=0; i<ia3DBays.GetSize(); i++ )
			{
				//we know the bay number that was stored, lets get the bay pointer!
				iDummy = ia3DBays.GetAt(i);
				m_ia3DBaysSelected.Add(iDummy);
			}
			for( i=0; i<iaMatrixBays.GetSize(); i++ )
			{
				//we know the bay number that was stored, lets get the bay pointer!
				iDummy = iaMatrixBays.GetAt(i);
				m_iaMatrixBaysSelected.Add( iDummy );
			}

			/////////////////////////////////////////////////////////////////////
			//3D crosshair
			if( !IsInsertingBlock() )
			{
				m_3DCrosshairTransform.Serialize(ar);			//The Transformation matrix for the 3D View
			}
			else
			{
				DummyTrans.Serialize(ar);
			}

			/////////////////////////////////////////////////////////////////////
			//Autobuild
			if( !IsInsertingBlock() )
			{
				m_AutoBuildTool.DeleteBays( 0, m_AutoBuildTool.GetNumberOfBays(), false );
				m_AutoBuildTool.Serialize(ar);					//The autobuild run and bay template
			}
			else
			{
				abtDummy = m_AutoBuildTool;
				abtDummy.Serialize(ar);					//The autobuild run and bay template
			}

			/////////////////////////////////////////////////////////////////////
			// JobDescription Retrieval
			if( !IsInsertingBlock() )
			{
				m_JobDescripInfo.Serialize(ar);
			}
			else
			{
				jdiDummy.Serialize(ar);
			}

			/////////////////////////////////////////////////////////////////////
			//BOM Extra Component List Retrieval - These are the components that are owned by this bay
			ar >> iNumberOfComponents;
			if (iNumberOfComponents > 0)
			{
				for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
				{
					pComponent = new Component();
					pComponent->SetController( this );
					pComponent->SetID(iComponentNumber);
					pComponent->Serialize(ar);			// Retrieve each component
					m_BOMExtraCompList.AddComponent((SideOfBayEnum)0, 0, pComponent);
				}
			}
#ifdef _DEBUG
		TimeBOMExtra = CTime::GetCurrentTime();
		TimeTaken = TimeBOMExtra - TimeRuns;
    acutPrintf( _T("\nKwikScaf Load BOMExtra Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );
#endif //_DEBUG
			// End BOM Extra Component List Retrieval

			/////////////////////////////////////////////////////////////////////
			//Lapboards
			ar >> iNumberOfLapboards;
			acedSetStatusBarProgressMeter( _T("Loading Lapboards"), 0, iNumberOfLapboards );
			for( iLapboardNumber=0; iLapboardNumber < iNumberOfLapboards; iLapboardNumber++ )
			{
				acedSetStatusBarProgressMeterPos(iLapboardNumber);
				pLapboard = new LapboardBay();
				pLapboard->SetController(this);
				pLapboard->m_bAllowRecreateStandards = false;
				pLapboard->Serialize(ar);
				pLapboard->m_bAllowRecreateStandards = true;
				m_Lapboards.AddNewLapboard(pLapboard);
				pLapboard->UpdateSchematicView();
			}			
			acedSetStatusBarProgressMeterPos(iNumberOfLapboards);
			acedRestoreStatusBar();
#ifdef _DEBUG
		TimeLap = CTime::GetCurrentTime();
		TimeTaken = TimeLap - TimeBOMExtra;
    acutPrintf( _T("\nKwikScaf Load Lap Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );
#endif //_DEBUG

			/////////////////////////////////////////////////////////////////////
			//levels
			iSize = daRLs.GetSize();
			for( i=0; i<iSize; i++ )
			{
				dRL = daRLs[i];
				m_daLevels.AddLevel( dRL );
			}

			/////////////////////////////////////////////////////////////////////
			//stages
			if( uiVersion<=CONTROLLER_VERSION_1_0_21 )
			{
				if( !IsInsertingBlock() )
				{
					ClearStages();
				}
				ar >> iSize;
				for( i=0; i<iSize; i++ )
				{
					ar >> sStage;
					AddStage( sStage );
				}
			}

			if( !IsInsertingBlock() )
			{
				m_StageListArray.Serialize( ar );
			}
			else
			{
				bslaDummy.SetController(this);
				bslaDummy.Serialize(ar);
			}

			/////////////////////////////////////////////////////////////////////
			//3D View
			if( !IsInsertingBlock() && bView3DCreated)
				Create3DView(true);
#ifdef _DEBUG
			Time3D = CTime::GetCurrentTime();
			TimeTaken = Time3D - TimeLap;
      acutPrintf( _T("\nKwikScaf Load Runs Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );
#endif //_DEBUG

			/////////////////////////////////////////////////////////////////////
			//matrix
			if( !IsInsertingBlock()  && bViewMatrixCreated)
			{
				if( GetMatrix()!=NULL )
					RedrawMatrix();
				else
					CreateMatrix( true );

				Trans = GetMatrixCrosshairTransform( ).invert();
				//GetMatrix()->UnMove();
				Transform = Trans * Transform;
				GetMatrix()->Move( Transform, true );
				GetMatrix()->MoveCrosshairOnly( Transform );
			}
#ifdef _DEBUG
			TimeMatrix = CTime::GetCurrentTime();
			TimeTaken = TimeMatrix - Time3D;
      acutPrintf( _T("\nKwikScaf Load Runs Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );
#endif //_DEBUG

			break;	//incompatible versions, can't fall through

		//////////////////////////////////////////////////////////
		case(CONTROLLER_VERSION_1_0_5):
			if( IsInsertingBlock() )
			{
				//Block insert not supported from this version back
				assert( false );
				sMsg = _T("Insert Operation not supported for this file version!\n");
				MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
				ar.Close();
				return;
			}
			LOAD_bool_IN_AR(m_bShowLongLevelLength);
			LOAD_bool_IN_AR(m_bShowFirstScale);
			LOAD_bool_IN_AR(m_bShowScaleLines);
			//fallthrough;

		case(CONTROLLER_VERSION_1_0_4):
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> dRL;
				daRLs.Add( dRL );
			}
			//fallthrough;

		case(CONTROLLER_VERSION_1_0_3):
			//The default for this value is already set by the
			//	constructor of CompDetails, so no need to do it again
			//	for version <CONTROLLER_VERSION_1_0_3
			LOAD_bool_IN_AR(bMills);
			SetSystem( bMills? S_MILLS: S_KWIKSTAGE );
			//This could have changed the predefined bay length, better reset them
			m_AutoBuildTool.SetPredefinedBayLengths( this );
			//fallthrough;

		case(CONTROLLER_VERSION_1_0_2):
			m_StageListArray.Serialize( ar );
			ClearStages();
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> sStage;
				AddStage( sStage );
			}
			//fallthrough

		case CONTROLLER_VERSION_1_0_1 :
			LOAD_bool_IN_AR(bView3DCreated);
			LOAD_bool_IN_AR(bViewMatrixCreated);

			if( bViewMatrixCreated )
			{
				Transform.Serialize(ar);
			}

			// Run List Retrieval - The list of runs, not including the Autobuild run
			ar >> iNumberOfRuns; // get number of runs
			if (iNumberOfRuns > 0)
			{
				for( iRunNumber=0; iRunNumber < iNumberOfRuns; iRunNumber++ )
				{
					pRun = new Run();
					AddRun(pRun);
					pRun->Serialize(ar);				// load each run
					pRun->SetRunID(iRunNumber);
				}
			}

			// End Run List Retrieval
			m_3DCrosshairTransform.Serialize(ar);			//The Transformation matrix for the 3D View
			m_AutoBuildTool.Serialize(ar);	//The autobuild run and bay template

			// JobDescription Retrieval
			m_JobDescripInfo.Serialize(ar);

			// BOM Extra Component List Retrieval - These are the components that are owned by this bay
			ar >> iNumberOfComponents;
			if (iNumberOfComponents > 0)
			{
				for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
				{
					pComponent = new Component();
					pComponent->SetController( this );
					pComponent->SetID(iComponentNumber);
					pComponent->Serialize(ar);				// Retrieve each component
					m_BOMExtraCompList.AddComponent((SideOfBayEnum)0, 0, pComponent);
				}
			}
			// End BOM Extra Component List Retrieval

			ar >> iNumberOfLapboards;
			for( iLapboardNumber=0; iLapboardNumber < iNumberOfLapboards; iLapboardNumber++ )
			{
				pLapboard = new LapboardBay();
				pLapboard->SetController(this);
				pLapboard->Serialize(ar);
				m_Lapboards.AddNewLapboard(pLapboard);
				pLapboard->UpdateSchematicView();
			}			

			if( !IsInsertingBlock() && bView3DCreated)
				Create3DView(true);

			iSize = daRLs.GetSize();
			for( i=0; i<iSize; i++ )
			{
				dRL = daRLs[i];
				m_daLevels.AddLevel( dRL );
			}

			if( !IsInsertingBlock() && bViewMatrixCreated)
			{
				if( GetMatrix()!=NULL )
					RedrawMatrix();
				else
					CreateMatrix(true);

				Trans = GetMatrixCrosshairTransform( ).invert();
				//GetMatrix()->UnMove();
				Transform = Trans * Transform;
				GetMatrix()->Move( Transform, true );
				GetMatrix()->MoveCrosshairOnly( Transform );

			}

			break;
		case CONTROLLER_VERSION_1_0_0 :
			LOAD_bool_IN_AR(bView3DCreated);
			LOAD_bool_IN_AR(bViewMatrixCreated);

			// Run List Retrieval - The list of runs, not including the Autobuild run
			ar >> iNumberOfRuns; // get number of runs
			if (iNumberOfRuns > 0)
			{
				for( iRunNumber=0; iRunNumber < iNumberOfRuns; iRunNumber++ )
				{
					pRun = new Run();
					AddRun(pRun);
					pRun->Serialize(ar);				// load each run
					pRun->SetRunID(iRunNumber);
				}
			}
			// End Run List Retrieval

			m_3DCrosshairTransform.Serialize(ar);			//The Transformation matrix for the 3D View
			m_AutoBuildTool.Serialize(ar);		//The autobuild run and bay template

			// JobDescription Retrieval
			m_JobDescripInfo.Serialize(ar);

			// BOM Extra Component List Retrieval - These are the components that are owned by this bay
			ar >> iNumberOfComponents;
			if (iNumberOfComponents > 0)
			{
				for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
				{
					pComponent = new Component();
					pComponent->SetController( this );
					pComponent->SetID(iComponentNumber);
					pComponent->Serialize(ar);				// Retrieve each component
					m_BOMExtraCompList.AddComponent((SideOfBayEnum)0, 0, pComponent);
				}
			}
			// End BOM Extra Component List Retrieval

			ar >> iNumberOfLapboards;
			for( iLapboardNumber=0; iLapboardNumber < iNumberOfLapboards; iLapboardNumber++ )
			{
				pLapboard = new LapboardBay();
				pLapboard->SetController(this);
				pLapboard->Serialize(ar);
				m_Lapboards.AddNewLapboard(pLapboard);
			}			

			if( !IsInsertingBlock() && bView3DCreated)
				Create3DView(true);
			if (bViewMatrixCreated)
			{
				if( GetMatrix()!=NULL )
					RedrawMatrix();
				else
					CreateMatrix(true);
			}

			break;
		default:
			assert( false );
			if( uiVersion>CONTROLLER_VERSION_LATEST )
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
      sMsg+= _T("Class: Controller.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), CONTROLLER_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}

		if( !IsInsertingBlock() )
		{
			CleanUp(false);
		}
		CleanUpLoadedInnerOuterPointers();
		SetConvertToSystemOnNextOpen(false);

#ifdef _DEBUG
		TimeFin = CTime::GetCurrentTime();
		TimeTaken = TimeFin - TimeStart;
    acutPrintf( _T("\nKwikScaf Load Time = %i:%02i"), TimeTaken.GetMinutes(), TimeTaken.GetSeconds() );

    acutPrintf( _T("\nCounters:") );
		for( i=0; i<iNumberOfCounter; i++ )
		{
			acutPrintf( _T("\n%s = %i"), gsaLoopCounter[i], giaLoopCounter[i] );
		}
#endif //_DEBUG
		//RedrawSchematic(false);
	}
}	//Serialize(CArchive &ar)

/////////////////////////////////////////////////////////////////////////////////////////
//GetBayFromBayNumber(int iBayNumber)
//
Bay * Controller::GetBayFromBayNumber(int iBayNumber)
{
	int			i, iBays, iSize;
	Run			*pRun;
	Bay			*pBay;

	//Baynumbers are 1 based, convert to zero based number to 
	//	search through the tree!
	iBayNumber--;
	if( iBayNumber<0 )
		return NULL;

	iSize = GetNumberOfRuns();
	iBays = 0;

	//go through each bay run until the bay we are looking for is within the run
	for( i=0; i<iSize; i++ )
	{
		pRun = GetRun(i);
		if( (iBays+pRun->GetNumberOfBays())>iBayNumber )
		{
			//The bay we are looking for is withing this run!
			pBay = pRun->GetBay( iBayNumber-iBays );
			return pBay;
		}
		iBays+= pRun->GetNumberOfBays();
	}
	return NULL;
}	//GetBayFromBayNumber(int iBayNumber)

/////////////////////////////////////////////////////////////////////////////////////////
//CreateLayers()
//
void Controller::CreateLayers()
{
	Acad::ErrorStatus		es;
	AcDb::OpenMode mode = AcDb::kForWrite;
	AcDbLayerTable	*pTable;
    if ((es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, mode))==Acad::eOk)
	{
		if( !pTable->has( LAYER_NAME_3D ) )
		{
			AcDbLayerTableRecord *pRecord;
			pRecord = new AcDbLayerTableRecord();
			pRecord->setName( LAYER_NAME_3D );
			pTable->add( pRecord );
			pRecord->close();
		}
		if( !pTable->has( LAYER_NAME_SCHEMATIC ) )
		{
			AcDbLayerTableRecord *pRecord;
			pRecord = new AcDbLayerTableRecord();
			pRecord->setName( LAYER_NAME_SCHEMATIC );
			pTable->add( pRecord );
			pRecord->close();
		}
		if( !pTable->has( LAYER_NAME_MATRIX ) )
		{
			AcDbLayerTableRecord *pRecord;
			pRecord = new AcDbLayerTableRecord();
			pRecord->setName( LAYER_NAME_MATRIX );
			pTable->add( pRecord );
			pRecord->close();
		}
		if( !pTable->has( LAYER_NAME_SECTIONS ) )
		{
			AcDbLayerTableRecord *pRecord;
			pRecord = new AcDbLayerTableRecord();
			pRecord->setName( LAYER_NAME_SECTIONS );
			pTable->add( pRecord );
			pRecord->close();
		}
		es = pTable->close();
		assert( es==Acad::eOk );
	}
}	//CreateLayers()

void Controller::DeleteLayers()
{
	Acad::ErrorStatus		es;

	es = DeleteLayer( LAYER_NAME_3D );
	assert( es==Acad::eOk );
	es = DeleteLayer( LAYER_NAME_SCHEMATIC );
	assert( es==Acad::eOk );
	es = DeleteLayer( LAYER_NAME_MATRIX );
	assert( es==Acad::eOk );
	es = DeleteLayer( LAYER_NAME_SECTIONS );
	assert( es==Acad::eOk );
}

Acad::ErrorStatus Controller::DeleteLayer( CString sLayerName )
{
	Acad::ErrorStatus		es;
	
	//Make sure that we have a working database
    if (acdbHostApplicationServices()->workingDatabase()==NULL)
	{
		assert( false );	//why don't we have a database?
        return Acad::eNoDatabase;
	}

	AcDb::OpenMode mode = AcDb::kForWrite;
	AcDbLayerTable* pTable;
	AcDbLayerTableRecord *pRec;

	pTable = NULL;
	es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, mode);
    if( es==Acad::eOk )
	{
		if( !pTable->has( sLayerName ) )
		{
			//don't draw it if the layer doesn't exist
			assert( false );	//why doesn't the layer exist?
			es = pTable->close();
			assert( es==Acad::eOk );
			return es;
		}

		//Delete the layer
		pRec = NULL;
		es = pTable->getAt( sLayerName, pRec, mode);
		assert( es==Acad::eOk );

		if( pRec!=NULL )
		{
			es = pRec->erase();
			assert( es==Acad::eOk );
			es = pRec->close();
			assert( es==Acad::eOk );
		}
		else
		{
			assert( false );
		}
	}
	else
	{
		assert( false );
		pTable = NULL;
	}

	if( pTable!=NULL )
	{
		es = pTable->close();
		assert( es==Acad::eOk );
	}

	return Acad::eOk;
}


/////////////////////////////////////////////////////////////////////////////////////////
//DisplayBOMExtra()
//
void Controller::DisplayBOMExtra()
{
	BOMExtra *pBOMExtraDialog;
	pBOMExtraDialog = new BOMExtra(NULL, &m_ltCompDetails, &m_BOMExtraCompList);
	pBOMExtraDialog->DoModal();
	DELETE_PTR(pBOMExtraDialog);
}	//DisplayBOMExtra()

/////////////////////////////////////////////////////////////////////////////////////////
//DisplayJobDetail()
//
void Controller::DisplayJobDetail()
{
	JobDetailsDialog *pJobDetailsDialog;
	pJobDetailsDialog = new JobDetailsDialog(NULL, &m_JobDescripInfo);
	pJobDetailsDialog->DoModal();
	DELETE_PTR(pJobDetailsDialog);
}	//DisplayJobDetail()

/////////////////////////////////////////////////////////////////////////////////////////
//SaveJobDescription()
//
void Controller::SaveJobDescription()
{
//	m_JobDescripInfo.StoreSummaryInfo(); // save job description from NOD info for info dialog
}	//SaveJobDescription()

/////////////////////////////////////////////////////////////////////////////////////////
//StairBay()
//
void Controller::StairBay()
{
	Bay		*pB;
	BayList Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	int i, iSize;

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pB = Bays[i];
      acutPrintf( _T("\nConverting bay %i to Stair bay:"), pB->GetBayNumber() );

			if( pB->GetBayWidth()	!= COMPONENT_LENGTH_1200 ||
				pB->GetBayLength()	!= COMPONENT_LENGTH_2400 )
			{
				CString sMsg;
        sMsg.Format( _T("Error: Unable to convert bay %i, since it is not a %0.1fm x %0.1fm bay!"),
							pB->GetBayNumber(),
							COMPONENT_LENGTH_2400*CONVERT_MM_TO_M,
							COMPONENT_LENGTH_1200*CONVERT_MM_TO_M );
				sMsg+=	_T("\n\nWould you like me to automatically redimension this bay?");

				MessageBeep(MB_ICONSTOP);
				if( MessageBox( NULL, sMsg, _T("Stair Bay Dimension Error"), MB_YESNO|MB_ICONSTOP )==IDYES )
				{
					pB->SetBayWidth( COMPONENT_LENGTH_1200 );
					pB->SetBayLength( COMPONENT_LENGTH_2400 );
					pB->ApplyBayResize(false);
				}
				else
				{
          sMsg.Format( _T("Conversion Unsuccessful: Would you like to delete bay %i?"), pB->GetBayNumber() );

					MessageBeep(MB_ICONSTOP);
					if( MessageBox( NULL, sMsg, _T("Stair Bay Dimension Error"), MB_YESNO|MB_ICONSTOP )==IDYES )
					{
						Run	*pR;
						pR = GetRunWhichOwnsBay( pB );
						if( pR!=NULL )
						{
							pR->DeleteBays( pB->GetID() );
						}
					}
					continue;
				}
			}

			pB->ChangeToStairBay();

			pB->UpdateSchematicView();

			acutPrintf( _T(" - Conversion successful") );
		}
		acutPrintf( _T("\nConversion complete") );
	}

	RefreshScreen();
}	//StairBay()

/////////////////////////////////////////////////////////////////////////////////////////
//LadderBay()
//
void Controller::LadderBay()
{
	Bay		*pB;
	BayList Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	int i, iSize;

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pB = Bays[i];
      acutPrintf( _T("\nConverting bay %i to Ladder bay:"), pB->GetBayNumber() );

			if( pB->GetBayWidth()	< COMPONENT_LENGTH_1200-ROUND_ERROR ||
				pB->GetBayLength()	< COMPONENT_LENGTH_2400-ROUND_ERROR ||
				pB->GetBayLength()	> COMPONENT_LENGTH_2400+ROUND_ERROR )
			{
				CString sMsg, sTemp;
				if( pB->GetBayWidth()	< COMPONENT_LENGTH_1200-ROUND_ERROR )
				{	
          sMsg.Format( _T("Error: Unable to convert bay %i, since it is less than %0.1fm wide!"),
							pB->GetBayNumber(), COMPONENT_LENGTH_1200*CONVERT_MM_TO_M );
				}
				else
				{
          sMsg.Format( _T("Error: Unable to convert bay %i, since it is not %0.1fm long!"),
							pB->GetBayNumber(), COMPONENT_LENGTH_2400*CONVERT_MM_TO_M );
				}

				sTemp.Format( _T("\n\nWould you like me to automatically redimension this bay to %0.1fm x %0.1fm?"),
								COMPONENT_LENGTH_1200*CONVERT_MM_TO_M, COMPONENT_LENGTH_2400*CONVERT_MM_TO_M );
				sMsg+= sTemp;

				MessageBeep(MB_ICONSTOP);
				if( MessageBox( NULL, sMsg, _T("Ladder Bay Dimension Error"), MB_YESNO|MB_ICONSTOP )==IDYES )
				{
					pB->SetBayWidth( COMPONENT_LENGTH_1200 );
					pB->SetBayLength( COMPONENT_LENGTH_2400 );
					pB->ApplyBayResize(false);
				}
				else
				{
          sMsg.Format( _T("Conversion Unsuccessful: Would you like to delete bay %i?"), pB->GetBayNumber() );

					MessageBeep(MB_ICONSTOP);
					if( MessageBox( NULL, sMsg, _T("Ladder Bay Dimension Error"), MB_YESNO|MB_ICONSTOP )==IDYES )
					{
						Run	*pR;
						pR = GetRunWhichOwnsBay( pB );
						if( pR!=NULL )
						{
							pR->DeleteBays( pB->GetID() );
						}
					}
					continue;
				}
			}

			pB->ChangeToLadderBay();

			pB->SetDirtyFlag( DF_CLEAN );
			pB->UpdateSchematicView();

			acutPrintf( _T(" - Conversion successful") );
		}
		acutPrintf( _T("\nConversion complete") );
	}
	RefreshScreen();
}	//LadderBay()

/////////////////////////////////////////////////////////////////////////////////////////
//ButtressBay()
//
void Controller::ButtressBay()
{
	Bay		*pB;
	BayList Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	int i, iSize;

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pB = Bays[i];
      acutPrintf( _T("\nConverting bay %i to Buttress bay:"), pB->GetBayNumber() );
			if( pB->ChangeToButtressBay() )
			{
				pB->UpdateSchematicView();
				acutPrintf( _T(" - Conversion successful") );
			}
			else
			{
				acutPrintf( _T(" - Conversion NOT successful") );
			}
		}
		acutPrintf( _T("\nConversion complete") );
	}

	RefreshScreen();
}	//ButtressBay()

/////////////////////////////////////////////////////////////////////////////////////////
//GetGroupNumber()
//
int * Controller::GetGroupNumber()
{
	return &m_iGroupNumber;
}	//GetGroupNumber()

/////////////////////////////////////////////////////////////////////////////////////////
//CreateListOfUniqueSAndLs(SummaryInfo &sSLList)
//
void Controller::CreateListOfUniqueSAndLs(SummaryInfo &sSLList)
{
	int			iRunID, iBayID, iLiftID, iComponentID;
	Run			*pR;
	Bay			*pB;
	Lift		*pL;
	double		dRLBottom, dRLTop, dRL;
	CString		sStage;	//, sStageLevel;
	LevelList	*pLL;
	Component	*pC;

	sSLList.RemoveAll();

	////////////////////////////////////////////////////////////////
	//Runs
	for( iRunID=0; iRunID<GetNumberOfRuns(); iRunID++ )
	{
		pR = GetRun( iRunID );
		assert( pR!=NULL );

		////////////////////////////////////////////////////////////////
		//Bays
		for( iBayID=0; iBayID<pR->GetNumberOfBays(); iBayID++ )
		{
			pB = pR->GetBay( iBayID );
			assert( pB!=NULL );

			////////////////////////////////////////////////////////////////
			//Bay Components
			for( iComponentID=0; iComponentID<pB->GetNumberOfBayComponents(); iComponentID++ )
			{
				pC = pB->GetBayComponent( iComponentID );
				assert( pC!=NULL );

				if( (	pC->GetType()==CT_SHADE_CLOTH)
					|| (pC->GetType()==CT_CHAIN_LINK) )
				{
					//Shade cloth and Chain mesh may span several levels
					pB->GetRLsForFaceByStandards( pC->GetSideOfBay(), dRLBottom, dRLTop );
					pLL = GetLevelList();
					if( pLL->GetSize()>0 )
					{
						int i;
						for( i=0; i<pLL->GetSize(); i++ )
						{
							dRL = pLL->GetLevel( i );
							if( dRL>dRLBottom && dRL<dRLTop )
							{
								sStage = GetBayStageLevelArray()->GetStage( pB->GetBayNumber(), i+1 );
								sSLList.AddElement( sStage, i+1, pC );
							}
							else if( dRL>dRLBottom && dRL>dRLTop )
							{
								sStage = GetBayStageLevelArray()->GetStage( pB->GetBayNumber(), i+1 );
								sSLList.AddElement( sStage, i+1, pC );
								break;	//don't let it do this again!
							}
						}
						if( i==pLL->GetSize() && dRL<dRLTop )
						{
							//The top of the lift is above all the RL's
							sStage = GetBayStageLevelArray()->GetStage( pB->GetBayNumber(), i+1 );
							sSLList.AddElement( sStage, i+1, pC );
						}
					}
					else
					{
						AddComponentToSummaryInfo( sSLList, pC );
					}
				}
				else
				{
					AddComponentToSummaryInfo( sSLList, pC );
				}
			}

			////////////////////////////////////////////////////////////////
			//Lifts
			for( iLiftID=0; iLiftID<pB->GetNumberOfLifts(); iLiftID++ )
			{
				pL = pB->GetLift( iLiftID );
				assert( pL!=NULL );

				////////////////////////////////////////////////////////////////
				//Lift Components
				for( iComponentID=0; iComponentID<pL->GetNumberOfComponents(); iComponentID++ )
				{
					pC = pL->GetComponent( iComponentID );
					assert( pC!=NULL );
					AddComponentToSummaryInfo( sSLList, pC );
				}
			}

		}
	}

	////////////////////////////////////////////////////////////////
	//Lapboards
	int			i, j, iLapID, iSize, jSize, iLaps;
	LapboardBay	*pLapboardBay;

	//A lapboard list is like a run, it has lapboard bays
	iLaps = m_Lapboards.GetNumberOfLapboards();
	for( iLapID=0; iLapID<iLaps; iLapID++ )
	{
		pLapboardBay = m_Lapboards.GetAt( iLapID );

		////////////////////////////////////////////////////////////////
		//lapboard bay components
		iSize = pLapboardBay->GetNumberOfBayComponents();
		for( i=0; i<iSize; i++ )
		{
			pC = pLapboardBay->GetBayComponent( i );
			AddComponentToSummaryInfo( sSLList, pC );
		}

		///////////////////////////////////////////////////////////////
		//Lapboard Lift components
		iSize = pLapboardBay->GetNumberOfLifts();
		for( i=0; i<iSize; i++ )
		{
			pL = pLapboardBay->GetLift( i );
			jSize = pL->GetNumberOfComponents();
			for( j=0; j<jSize; j++ )
			{
				pC = pL->GetComponent( j );
				AddComponentToSummaryInfo( sSLList, pC );
			}
		}	
	}

	////////////////////////////////////////////////////////////////
	//BOM Extra Component
	iSize = m_BOMExtraCompList.GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pC = m_BOMExtraCompList.GetComponent(i);
		AddComponentToSummaryInfo( sSLList, pC );
	}

	////////////////////////////////////////////////////////////////
	//BOM Extra Component
	iSize = GetVisualComponents()->GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pC = GetVisualComponents()->GetComponent(i);
		AddComponentToSummaryInfo( sSLList, pC );
	}
}	//CreateListOfUniqueSAndLs(SummaryInfo &sSLList)

/////////////////////////////////////////////////////////////////////////////////////////
//DoBOMSummary()
//
void Controller::DoBOMSummary()
{
	int					iRet;
	SummaryInfo			siList;
	BOMSelectionDialog	*pBOMSelectionDialog;

	if( !CheckRoofProtectionIsValid( true ) )
		return;

	iRet=IDRETRY;
	while( iRet==IDRETRY )
	{
		CreateListOfUniqueSAndLs(siList);

		//switch resources to the dll's resources
		HINSTANCE hInst = NULL;
		if( _hdllInstance!=NULL )
		{
			hInst = AfxGetResourceHandle();
			AfxSetResourceHandle(_hdllInstance);
		}


		pBOMSelectionDialog = new BOMSelectionDialog(NULL, &siList);
		iRet = pBOMSelectionDialog->DoModal();	
		DELETE_PTR(pBOMSelectionDialog);

		//Restore the resources
		if( _hdllInstance!=NULL )
		{
			AfxSetResourceHandle(hInst);
		}

		if( iRet==IDRETRY )
		{
			//show BOM Extra Dialog
			DisplayBOMExtra();
			//repeat!
		}
	}
}	//DoBOMSummary()


/////////////////////////////////////////////////////////////////////////////////////////
//GetLevelList()
//
LevelList * Controller::GetLevelList()
{
	return &m_daLevels;
}	//GetLevelList()

/////////////////////////////////////////////////////////////////////////////////////////
//AddLevel(double dRL)
//
void Controller::AddLevel(double dRL)
{
	if( GetMatrix()!=NULL )
	{
		m_daLevels.AddLevel( dRL );
		GetMatrix()->SetMatrixLevelsChanged(true);
	}
	else
	{
		CString sMsg;
		sMsg = _T("Please create a matrix prior to adding levels.");

		MessageBeep(MB_ICONSTOP);
		MessageBox( NULL, sMsg, _T("Add Level Error"), MB_OK|MB_ICONSTOP );
	}
}	//AddLevel(double dRL)

/////////////////////////////////////////////////////////////////////////////////////////
//EnterLevels()
//Allows user to type levels directly at the command prompt
void Controller::EnterLevels( bool bAddLevel/*=true*/ )
{
	int		i;
	TCHAR	keywrd[255], prompt[255];
	double	dRL;
	CString	sPrompt, sTemp, sKeyWrd;

	//we can't remove if there is nothing to remove
	if( m_daLevels.GetSize()<=0 )
		bAddLevel = true;

	if( bAddLevel )
	{
		sPrompt = _T("\nAdd new level(mm)");
		if( m_daLevels.GetSize()>0 )
		{
			sPrompt+= _T(" [Remove level/Clear all]");
		}
	}
	else
	{
		sPrompt = _T("\nRemove level(mm) [Add level/Clear all]");
	}
	if( m_daLevels.GetSize()>0 )
	{
		sPrompt+= _T(" <");
		sTemp.Format( _T("%1.0f"), m_daLevels.GetLevel(0) );
		sPrompt+= sTemp;

		for( i=1; i<m_daLevels.GetSize(); i++ )
		{
			sTemp.Format( _T(",%1.0f"), m_daLevels.GetLevel(i) );
			sPrompt+= sTemp;
		}
	}
	else
	{
		sPrompt+= _T(" <empty");
	}
  sPrompt+= _T(">: ");

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	acedInitGet( RSG_OTHER, _T("Remove Clear Add"));
	dRL = LARGE_NEGATIVE_NUMBER;
	int iRet = acedGetReal( prompt, &dRL );
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		EnterLevels( bAddLevel );
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( dRL>LARGE_NEGATIVE_NUMBER+ROUND_ERROR )
		{
			if( bAddLevel )
			{
				//dRL = atof( sKeyWrd );
				m_daLevels.AddLevel( dRL, false );
				m_daLevels.SelectionSort();
				EnterLevels( bAddLevel );
			}
			else
			{
				//dRL = atof( sKeyWrd );
				if( !m_daLevels.RemoveLevel( dRL ) )
				{
					MessageBeep(MB_ICONERROR);
          sPrompt = _T("Error: You may only remove existing levels!\n");
          sTemp.Format( _T("%1.0fmm does not exist in the following list:\n"), dRL );
					sPrompt+= sTemp;
					if( m_daLevels.GetSize()>0 )
					{
						sTemp.Format( _T("%1.0f"), m_daLevels.GetLevel(0) );
						sPrompt+= sTemp;
					}
					for( i=1; i<m_daLevels.GetSize()-1; i++ )
					{
						sTemp.Format( _T(", %1.0f"), m_daLevels.GetLevel(i) );
						sPrompt+= sTemp;
						if( i%5==0 )
							sPrompt+= _T("\n");
					}
					if( i<m_daLevels.GetSize() )
					{
						sTemp.Format( _T(" & %1.0f"), m_daLevels.GetLevel(i) );
						sPrompt+= sTemp;
					}
					MessageBox( NULL, sPrompt, _T("Must be an existing level"), MB_OK|MB_ICONSTOP );
				}
				EnterLevels( bAddLevel );
				return;
			}
			return;
		}
		if( sKeyWrd[0]==_T('A') )
		{
			EnterLevels( true );
			return;
		}
		else if( sKeyWrd[0]==_T('R') )
		{
			if( m_daLevels.GetSize()<=0 )
			{
				MessageBeep(MB_ICONERROR);
				EnterLevels( true );
			}
			else
			{
				EnterLevels( false );
			}
			return;
		}
		else if( sKeyWrd[0]==_T('C') )
		{
			m_daLevels.RemoveAll();
			EnterLevels( true );
			return;
		}
		else
		{
			//invalid input!
			MessageBeep(MB_ICONERROR);
			EnterLevels( bAddLevel );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//RemoveLevel(double dRL)
//
bool Controller::RemoveLevel(double dRL)
{
	if( GetMatrix()!=NULL )
	{
		return m_daLevels.RemoveLevel( dRL );
	}
	else
	{
		CString sMsg;
		sMsg = _T("Please create a matrix prior to removing levels.");

		MessageBeep(MB_ICONSTOP);
		MessageBox( NULL, sMsg, _T("Remove Level Error"), MB_OK|MB_ICONSTOP );
	}
	return false;
}	//RemoveLevel(double dRL)

/////////////////////////////////////////////////////////////////////////////////////////
//RemoveLevel(int iLevelID, int nCount/*=1*/)
//
bool Controller::RemoveLevel(int iLevelID, int nCount/*=1*/)
{
	assert( iLevelID>=0 );
	assert( iLevelID+nCount<=m_daLevels.GetSize() );
	return m_daLevels.RemoveLevel( iLevelID, nCount );
}	//RemoveLevel(int iLevelID, int nCount/*=1*/)

/////////////////////////////////////////////////////////////////////////////////////////
//RegenMatrixLevels()
//
void Controller::RegenMatrixLevels( bool bShowWarning/*=true*/ )
{
	if( GetMatrix()!=NULL )
	{
		if( GetMatrix()->HaveMatrixLevelsChanged() )
		{
			GetMatrix()->RegenLevels();
		}
	}
	else
	{
		if( bShowWarning )
		{
			CString sMsg;
			sMsg = _T("Please create a matrix prior to regenerating the levels.");

			MessageBeep(MB_ICONSTOP);
			MessageBox( NULL, sMsg, _T("Remove Level Error"), MB_OK|MB_ICONSTOP );
		}
	}
}	//RegenMatrixLevels()

/////////////////////////////////////////////////////////////////////////////////////////
//SetSystem(SystemEnum eSystem)
//
void Controller::SetSystem(SystemEnum eSystem)
{
	//Delete the autobuild bay
	m_AutoBuildTool.DeleteBays( 0, m_AutoBuildTool.GetNumberOfBays(), false );

	//NOTE:  it is not possible to set the system
	//	to 'other', only mills or kwikstage
	assert( eSystem==S_MILLS || eSystem==S_KWIKSTAGE );
	m_eSystem = eSystem;

	//This could have changed the predefined bay length, better reset them
	m_AutoBuildTool.SetPredefinedBayLengths(this);

	switch( eSystem )
	{
	case( S_MILLS ):
		STAR_SEPARATION						= STAR_SEPARATION_MILLS;
		STAR_SEPARATION_APPROX				= STAR_SEPARATION_APPROX_MILLS;
		EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD	= EXTRA_LENGTH_MS_BOTTOM_OF_STANDARD;
		EXTRA_LENGTH_AT_TOP_OF_STANDARD		= EXTRA_LENGTH_MS_BOTTOM_OF_STANDARD;
		break;

	case( S_OTHER ):
	default:
		assert( false );
		//fallthrough
	case( S_KWIKSTAGE ):
		STAR_SEPARATION						= STAR_SEPARATION_KWIKSTAGE;
		STAR_SEPARATION_APPROX				= STAR_SEPARATION_APPROX_KWIKSTAGE;
		EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD	= EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;
		EXTRA_LENGTH_AT_TOP_OF_STANDARD		= EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;
		break;

	}
	JACK_LENGTH				= STAR_SEPARATION;
	RL_ADJUST				= STAR_SEPARATION;
	JACK_LENGTH_APPROX		= STAR_SEPARATION_APPROX;
	RL_ADJUST_APPROX		= STAR_SEPARATION_APPROX;

	//We need to recreate the Autobuild Bay
	CreateDefaultAutoBuildRun();
}	//SetSystem(SystemEnum eSystem)

void Controller::ChangeSystem()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nUse Mills system scaffolding <%i>: "), (GetSystem()==S_MILLS)? 1: 0 );
 
	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = (GetSystem()==S_MILLS)? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeSystem();
		return;
	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetSystem( (i==0)? S_MILLS: S_KWIKSTAGE );
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//SetMatrixCrosshairTransform(Matrix3D Transform)
//
void Controller::SetMatrixCrosshairTransform(Matrix3D Transform)
{
	m_MatrixCrosshairTransform = Transform;
}	//SetMatrixCrosshairTransform(Matrix3D Transform)

/////////////////////////////////////////////////////////////////////////////////////////
//GetMatrixCrosshairTransform()
//
Matrix3D Controller::GetMatrixCrosshairTransform()
{
	return m_MatrixCrosshairTransform;
}	//GetMatrixCrosshairTransform()

/////////////////////////////////////////////////////////////////////////////////////////
//SetCutThroughCrosshairTransform(CutThrough3D Transform)
//
void Controller::SetCutThroughCrosshairTransform(Matrix3D Transform)
{
	m_CutThroughCrosshairTransform = Transform;
}	//SetCutThroughCrosshairTransform(Matrix3D Transform)

/////////////////////////////////////////////////////////////////////////////////////////
//GetCutThroughCrosshairTransform()
//
Matrix3D Controller::GetCutThroughCrosshairTransform()
{
	return m_CutThroughCrosshairTransform;
}	//GetCutThroughCrosshairTransform()

/////////////////////////////////////////////////////////////////////////////////////////
//GetStockList()
//
StockListArray *Controller::GetStockList()
{
	return &m_ltStockList;
}	//GetStockList()

/////////////////////////////////////////////////////////////////////////////////////////
//SetStageForSelectedBays()
//
bool Controller::SetStageForSelectedBays()
{
	int		iSize;
	bool	bExit;
	BayList Bays;

	if( !SelectBays( Bays, true ) )
		return false;
	//CheckForStairLadder( &Bays, false );
	//RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )   // if at least one bay selected
	{
		Bays.DisplaySelectedBays();

		SetStagesDlg *pSetStagesDlg;
		bExit = false;
		while (!bExit)
		{
			pSetStagesDlg = new SetStagesDlg( NULL );
			pSetStagesDlg->SetColourByStageLevel(IsColourByStageLevel());
			pSetStagesDlg->SetBayList( &Bays );
			pSetStagesDlg->SetController( this );

			if(pSetStagesDlg->DoModal() == IDRETRY )
			{
				while( !SelectBays( Bays, true ) )
				{
					1;
				}
				//See bug 1479
				//CheckForStairLadder( &Bays, false );
				Bays.DisplaySelectedBays();
			}
			else
			{
				bExit = true;
			}
			SetColourByStageLevel( pSetStagesDlg->IsColourByStageLevel() );
			DELETE_PTR(pSetStagesDlg);
		}
	}  

	return false;
}	//SetStageForSelectedBays()

/////////////////////////////////////////////////////////////////////////////////////////
//RestrictMouse()
//
/*
void Controller::RestrictMouse()
{
	::RestricMouse( MR_VERTICAL );
	::TrapMouse();	
}	//RestrictMouse()
*/

enum
{
	RESPONSE_INVALID,
	RESPONSE_YES,
	RESPONSE_NO,
	RESPONSE_ALL,
	RESPONSE_NOALL
};

/////////////////////////////////////////////////////////////////////////////////////////
//AddTieToBay()
//
void Controller::AddTieToBay()
{
	int				i, j, iSize, iResponse;
	Bay				*pBay;
	bool			bFound;
	BayList			Bays;
	RunList			Runs, CompletedRuns;
	TieTypesEnum	eTT;
	SideOfBayEnum	eSide;

	if( !SelectBays( Bays, true ) )
		return;

	//See bug 1456 (Add Tie to Stair Ladder Bay)
	//CheckForStairLadder( &Bays, false );

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		//////////////////////////////////////////////////////////////////
		Runs.RemoveAll();
		for( i=0; i<iSize; i++ )
		{
			//Create a list of unique runs!
			AddRunToUniqueList( Bays[i]->GetRunPointer(), Runs);
		}
		assert( Runs.GetSize()>0 );
		eTT = Bays[0]->GetRunPointer()->GetTieType();
		eSide = SSE;
		if( !GetPositionOfBayFromCommandLine( eSide, eTT, &Bays ) )
			return;

		//////////////////////////////////////////////////////////////////
		iResponse = RESPONSE_INVALID;
		CompletedRuns.RemoveAll();
		for( i=0; i<iSize; i++ )
		{
			pBay = Bays[i];

			bFound = false;
			for( j=0; j<CompletedRuns.GetSize(); j++ )
			{
				if( pBay->GetRunPointer()==CompletedRuns[j] )
				{
					bFound = true;
					break;
				}
			}

			//we found this bay in the list of completed runs, we had better ignor it
			//	since we will have already processed it!
			if( !bFound )
			{
				pBay->GetRunPointer()->SetTieType( eTT );
				if( iResponse==RESPONSE_INVALID || iResponse==RESPONSE_YES ||
					iResponse==RESPONSE_NO )
				{
					if( !GetRangeToApplyTies( pBay, iResponse, Runs.GetSize()-CompletedRuns.GetSize() ) )
						return;

					switch( iResponse )
					{
					default:
					case( RESPONSE_INVALID ):
						assert( false );
						MessageBeep(MB_ICONEXCLAMATION);
						return;	//Unknown error
					case( RESPONSE_ALL ):	//fallthrough
					case( RESPONSE_YES ):
						ApplyTieToRun( pBay->GetRunPointer(), eSide, CompletedRuns );
						break;
					case( RESPONSE_NOALL ):	//fallthrough
					case( RESPONSE_NO ):
						ApplyTieToBay( pBay, eSide );
						break;
					}
				}
				else if( iResponse==RESPONSE_ALL )
				{
					ApplyTieToRun( pBay->GetRunPointer(), eSide, CompletedRuns );
				}
				else if( iResponse==RESPONSE_NOALL )
				{
					ApplyTieToBay( pBay, eSide );
					break;
				}
			}
		}
		pBay = NULL;
	}
}	//AddTieToBay()

/////////////////////////////////////////////////////////////////////////////////////////
//AddBracingToBay()
//
void Controller::AddBracingToBay()
{
	int				i, iResponse;
	Bay				*pBay, *pNeighborBay;
	CString			sMsg;
	BayList			Bays;
	SideOfBayEnum	eSide, eLeft, eRight;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );

	Bays.GetSize();
	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		//check for stage boards
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->
						HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
			{
				sMsg.Format( _T("Bay %i has a stage board on the %s side,"), pBay->GetBayNumber(), GetSideOfBayDescStr( eSide ) );
				sMsg+= _T("\nplacing bracing here will Delete the stage board!");
				sMsg+= _T("\nReplace the stage board for this bay?");

				MessageBeep(MB_ICONQUESTION);
				iResponse = MessageBox( NULL, sMsg, _T("Existing Component Warning"), MB_YESNOCANCEL|MB_ICONQUESTION );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				else if( iResponse==IDCANCEL )
				{
					return;
				}

			}
		}

		//check for neighboring bays
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];

			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				pNeighborBay = pBay->GetOuter();
				break;
			case( EAST ):
				pNeighborBay = pBay->GetForward();
				break;
			case( SOUTH ):
				pNeighborBay = pBay->GetInner();
				break;
			case( WEST ):
				pNeighborBay = pBay->GetBackward();
				break;
			}
			if( pNeighborBay!=NULL )
			{
				sMsg.Format( _T("Bay %i is to the %s of Bay %i, therefore we cannot place bracing to the %s of Bay %i!"),
					pNeighborBay->GetBayNumber(), GetSideOfBayDescStr( eSide ), pBay->GetBayNumber(),
					GetSideOfBayDescStr( eSide ), pBay->GetBayNumber() );
				MessageBox( NULL, sMsg, _T("Existing Bay Warning"), MB_ICONSTOP );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				if( iResponse==IDCANCEL )
					return;

			}
		}

		//the size may have changed
		if( Bays.GetSize()>0 )
		{
			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				eLeft	= NNW;
				eRight	= NNE;
				break;
			case( EAST ):
				eLeft	= ENE;
				eRight	= ESE;
				break;
			case( SOUTH ):
				eLeft	= SSE;
				eRight	= SSW;
				break;
			case( WEST ):
				eLeft	= WSW;
				eRight	= WNW;
				break;
			}

			//apply changes to all remaining bays (some may have been removed
			//	from the original list)
			for( i=0; i<Bays.GetSize(); i++ )
			{
				pBay = Bays[i];

				//remove any obsticles
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eLeft );
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eRight );

				//create the bracing
				pBay->CreateBracingArrangment( eSide, MT_STEEL, true );

				//redraw the schematic;
				pBay->UpdateSchematicView();
			}
		}
	}
}	//AddBracingToBay()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteBracingFromBays()
//
void Controller::DeleteBracingFromBays()
{
	int				i, iSize;
	Bay				*pBay;
	BayList			Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();
		SideOfBayEnum	eSide;
		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays.GetAt(i);
			pBay->DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eSide );
			pBay->UpdateSchematicView();
		}
	}
}	//DeleteBracingFromBays()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteTiesFromBay()
//
bool Controller::DeleteTiesFromBay()
{
	int				i, iSize;
	BayList			Bays;
	RunList			Runs;

	if( !SelectBays( Bays, true ) )
		return false;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();
		SideOfBayEnum	eSide;
/*
		Which of these 3 shoud we use?
		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return false;
		This one will need modification
		if( !GetPositionOfBayFromCommandLine( SideOfBayEnum &eSide, TieTypesEnum &eTT, &Bays ) )
			return false;
		if( !GetCornerOfBayFromCommandLine( SideOfBayEnum &eSide ) )
			return false;
*/

		eSide = ALL_SIDES;
		for( i=0; i<iSize; i++ )
		{
			Bays[i]->DeleteTies( eSide );
			Bays[i]->UpdateSchematicView();
		}
	}
	return true;
}	//DeleteTiesFromBay()

/////////////////////////////////////////////////////////////////////////////////////////
//AddRunToUniqueList(Run *pNewRun, RunList &Runs)
//
bool Controller::AddRunToUniqueList(Run *pNewRun, RunList &Runs)
{
	int		i;
	Run		*pRun;
	bool	bFound;

	//Create a list of unique runs!
	bFound= false;
	for( i=0; i<Runs.GetSize(); i++ )
	{
		pRun = Runs[i];
		if( pNewRun==pRun )
		{
			bFound = true;
			break;
		}
	}
	if( !bFound )
		Runs.Add( pNewRun );

	return !bFound;
}	//AddRunToUniqueList(Run *pNewRun, RunList &Runs)

/////////////////////////////////////////////////////////////////////////////////////////
//SetTemplateTiesForBay(Bay *pBay, SideOfBayEnum eSide)
//
void Controller::SetTemplateTiesForBay(Bay *pBay, SideOfBayEnum eSide)
{
	assert( pBay!=NULL );
	pBay->GetTemplate()->SetNNETie( false );
	pBay->GetTemplate()->SetENETie( false );
	pBay->GetTemplate()->SetESETie( false );
	pBay->GetTemplate()->SetSSETie( false );
	pBay->GetTemplate()->SetSSWTie( false );
	pBay->GetTemplate()->SetWSWTie( false );
	pBay->GetTemplate()->SetWNWTie( false );
	pBay->GetTemplate()->SetNNWTie( false );
	switch( eSide )
	{
	case( NNE ):
		pBay->GetTemplate()->SetNNETie( true );
		break;
	case( ENE ):
		pBay->GetTemplate()->SetENETie( true );
		break;
	case( ESE ):
		pBay->GetTemplate()->SetESETie( true );
		break;
	case( SSE ):
		pBay->GetTemplate()->SetSSETie( true );
		break;
	case( SSW ):
		pBay->GetTemplate()->SetSSWTie( true );
		break;
	case( WSW ):
		pBay->GetTemplate()->SetWSWTie( true );
		break;
	case( WNW ):
		pBay->GetTemplate()->SetWNWTie( true );
		break;
	case( NNW ):
		pBay->GetTemplate()->SetNNWTie( true );
		break;
	default:
		assert(false);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}	//SetTemplateTiesForBay(Bay *pBay, SideOfBayEnum eSide)

/////////////////////////////////////////////////////////////////////////////////////////
//GetPositionOfBayFromCommandLine( SideOfBayEnum &eSide, TieTypesEnum &eTT, BayList &pBays )
//
bool Controller::GetPositionOfBayFromCommandLine( SideOfBayEnum &eSide, TieTypesEnum &eTT, BayList *pBays )
{
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd, sTemp, sSide;
	
	//////////////////////////////////////////////////////////////////
	//Choose a side
	switch( eSide )
	{
		case( NNW ): sSide = _T("NNW"); break;
		case( NNE ): sSide = _T("NNE"); break;
		case( ENE ): sSide = _T("ENE"); break;
		case( ESE ): sSide = _T("ESE"); break;
		case( SSE ): sSide = _T("SSE"); break;
		case( SSW ): sSide = _T("SSW"); break;
		case( WSW ): sSide = _T("WSW"); break;
		case( WNW ): sSide = _T("WNW"); break;
		default:
			assert( false );
	}
	sPrompt = _T("\nEnter tie type first, and which side of the bay [NNW/NNE/ENE/ESE/SSE/SSW/WSW/WNW/Column/Masonary/Yoke] ");
  sTemp.Format( _T("<%s/%s>: "), sSide, GetTieTypeDescriptionStr( eTT ) );
	sPrompt+= sTemp;

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	Point3D			ptPoint, pt, ptSide;
	Matrix3D		Transform;
	intArray		iaAvailableSides;

	acedInitGet( RSG_OTHER, _T("NNW NNE ENE ESE SSE SSW WSW WNW Column Masonary Yoke"));
	int iRet = GetPoint3D( sPrompt, ptPoint );

	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd==_T("NNE") )
				eSide = NNE;
			else if( sKeyWrd==_T("ENE") )
				eSide = ENE;
			else if( sKeyWrd==_T("ESE") )
				eSide = ESE;
			else if( sKeyWrd==_T("SSE") )
				eSide = SSE;
			else if( sKeyWrd==_T("SSW") )
				eSide = SSW;
			else if( sKeyWrd==_T("WSW") )
				eSide = WSW;
			else if( sKeyWrd==_T("WNW") )
				eSide = WNW;
			else if( sKeyWrd==_T("NNW") )
				eSide = NNW;
			else if( sKeyWrd[0]==_T('C') )
			{
				eTT = TIE_TYPE_COLUMN;
				return GetPositionOfBayFromCommandLine( eSide, eTT, pBays );
			}
			else if( sKeyWrd[0]==_T('M') )
			{
				eTT = TIE_TYPE_MASONARY;
				return GetPositionOfBayFromCommandLine( eSide, eTT, pBays );
			}
			else if( sKeyWrd[0]==_T('Y') )
			{
				eTT = TIE_TYPE_YOKE;
				return GetPositionOfBayFromCommandLine( eSide, eTT, pBays );
			}
			else 
				//invalid input
				return false;
			break;
		}
		break;

	case( RTNORM ):				//(5100) Request succeeded
		iaAvailableSides.RemoveAll();
		iaAvailableSides.Add( BP_NNE_STG );
		iaAvailableSides.Add( BP_ENE_STG );
		iaAvailableSides.Add( BP_ESE_STG );
		iaAvailableSides.Add( BP_SSE_STG );
		iaAvailableSides.Add( BP_SSW_STG );
		iaAvailableSides.Add( BP_WSW_STG );
		iaAvailableSides.Add( BP_WNW_STG );
		iaAvailableSides.Add( BP_NNW_STG );

		GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide );

		return GetPositionOfBayFromCommandLine( eSide, eTT, pBays );
	}
	return true;
}	//GetPositionOfBayFromCommandLine( SideOfBayEnum &eSide, TieTypesEnum &eTT, BayList *pBays )

/////////////////////////////////////////////////////////////////////////////////////////
//GetCornerOfBayFromCommandLine( SideOfBayEnum &eSide )
//
bool Controller::GetCornerOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays )
{
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd, sTemp;

	//////////////////////////////////////////////////////////////////
	//Choose a side
	switch( eSide )
	{
	case( NE ):
		sTemp = _T("NE");
		break;
	case( SE ):
		sTemp = _T("SE");
		break;
	case( SW ):
		sTemp = _T("SW");
		break;
	case( NW ):
		sTemp = _T("NW");
		break;
	default:
		assert( false );
	}
  sPrompt.Format( _T("\nWhich side of the bay [NE/SE/SW/NW] <%s>:"), sTemp);
	acedInitGet( RSG_OTHER, sTemp );

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
	Point3D			ptPoint;
	Matrix3D		Transform;
	intArray		iaAvailableSides;

	int iRet = GetPoint3D( sPrompt, ptPoint );
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd==_T("NE") )
			{
				eSide = NE;
			}
			else if( sKeyWrd==_T("SE") )
			{
				eSide = SE;
			}
			else if( sKeyWrd==_T("SW") )
			{
				eSide = SW;
			}
			else if( sKeyWrd==_T("NW") )
			{
				eSide = NW;
			}
			else 
			{
				//invalid input
				acutPrintf( _T("\nInvalid input error!") );
				return GetCornerOfBayFromCommandLine( eSide, pBays );
			}
			break;
		}
		break;

	case( RTNORM ):				//(5100) Request succeeded
		iaAvailableSides.RemoveAll();
		iaAvailableSides.Add( BP_SW_STD );
		iaAvailableSides.Add( BP_SE_STD );
		iaAvailableSides.Add( BP_NW_STD );
		iaAvailableSides.Add( BP_NE_STD );

		GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide );

		return GetCornerOfBayFromCommandLine( eSide, pBays );
	}
	return true;
}	//GetCornerOfBayFromCommandLine( SideOfBayEnum &eSide )

/////////////////////////////////////////////////////////////////////////////////////////
//GetSideOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays )
//
bool Controller::GetSideOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays )
{
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd, sTemp;

	//////////////////////////////////////////////////////////////////
	//Choose a side
	switch( eSide )
	{
	case( N ):
		sTemp = _T("N");
		break;
	case( E ):
		sTemp = _T("E");
		break;
	case( S ):
		sTemp = _T("S");
		break;
	case( W ):
		sTemp = _T("W");
		break;
	default:
		assert( false );
	}
  sPrompt.Format( _T("\nWhich side of the bay [N/E/S/W] <%s>:"), sTemp );

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	Point3D			ptPoint;
	Matrix3D		Transform;
	intArray		iaAvailableSides;

	acedInitGet( RSG_OTHER, _T("N E S W"));
	int iRet = GetPoint3D( sPrompt, ptPoint );
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		//fine!
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd==_T("N") )
				eSide = N;
			else if( sKeyWrd==_T("E") )
				eSide = E;
			else if( sKeyWrd==_T("S") )
				eSide = S;
			else if( sKeyWrd==_T("W") )
				eSide = W;
			else 
				//invalid input
				return false;
			break;
		}
		break;

	case( RTNORM ):				//(5100) Request succeeded
		iaAvailableSides.RemoveAll();
		iaAvailableSides.Add( BP_N_MILLS );
		iaAvailableSides.Add( BP_E_MILLS );
		iaAvailableSides.Add( BP_S_MILLS );
		iaAvailableSides.Add( BP_W_MILLS );

		GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide );

		return GetSideOfBayFromCommandLine( eSide, pBays );
	}
	return true;
}	//GetSideOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays )

/////////////////////////////////////////////////////////////////////////////////////////
//GetSideOfBayAndHRStyleFromCommandLine( SideOfBayEnum &eSide, HandrailStyleEnum &eHandRailStyle, BayList *pBays )
//
bool Controller::GetSideOfBayAndHRStyleFromCommandLine( SideOfBayEnum &eSide, HandrailStyleEnum &eHandRailStyle, BayList *pBays )
{
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd;

	//////////////////////////////////////////////////////////////////
	//Choose a side
	CString sStyle, sTemp;
	switch( eHandRailStyle )
	{
	case( HRS_RAIL_ONLY ):
		sStyle = _T("Hand");
		break;
	case( HRS_HALF_MESH ):
		sStyle = _T("Mesh");
		break;
	case( HRS_FULL_MESH ):
		sStyle = _T("Fullmesh");
		break;
	default:
		sStyle = _T("Hand");
		eHandRailStyle = HRS_RAIL_ONLY;
		assert( false );
	}

	switch( eSide )
	{
	case( NORTH ):
		sTemp = _T("N");
		break;
	case( EAST ):
		sTemp = _T("E");
		break;
	case( SOUTH ):
		sTemp = _T("S");
		break;
	case( WEST ):
		sTemp = _T("W");
		break;
	default:
		assert(false);
	}

  sPrompt.Format( _T("\nChoose handrail style and side of the bay [Hand/Mesh/Fullmesh/N/E/S/W] <%s,%s>:"),
							sStyle, sTemp);

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	Point3D			ptPoint;
	Matrix3D		Transform;
	intArray		iaAvailableSides;

	acedInitGet( RSG_OTHER, _T("N E S W R HM FM"));
	int iRet = GetPoint3D( sPrompt, ptPoint );
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		//fine
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd==_T("N") )
			{
				eSide = N;
			}
			else if( sKeyWrd==_T("E") )
			{
				eSide = E;
			}
			else if( sKeyWrd==_T("S") )
			{
				eSide = S;
			}
			else if( sKeyWrd==_T("W") )
			{
				eSide = W;
			}
			else if( sKeyWrd==_T("H") )
			{
				//calls itself recusively
				eHandRailStyle = HRS_RAIL_ONLY;
				return GetSideOfBayAndHRStyleFromCommandLine( eSide, eHandRailStyle, pBays );
			}
			else if( sKeyWrd==_T("M") )
			{
				//calls itself recusively
				eHandRailStyle = HRS_HALF_MESH;
				return GetSideOfBayAndHRStyleFromCommandLine( eSide, eHandRailStyle, pBays );
			}
			else if( sKeyWrd==_T("F") )
			{
				//calls itself recusively
				eHandRailStyle = HRS_FULL_MESH;
				return GetSideOfBayAndHRStyleFromCommandLine( eSide, eHandRailStyle, pBays );
			}
			else 
			{
				//invalid input
				return false;
			}
			break;
		}
		break;

	case( RTNORM ):				//(5100) Request succeeded
		iaAvailableSides.RemoveAll();
		iaAvailableSides.Add( BP_N_MILLS );
		iaAvailableSides.Add( BP_E_MILLS );
		iaAvailableSides.Add( BP_S_MILLS );
		iaAvailableSides.Add( BP_W_MILLS );

		GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide );

		return GetSideOfBayAndHRStyleFromCommandLine( eSide, eHandRailStyle, pBays );
	}
	return true;
}	//GetSideOfBayAndHRStyleFromCommandLine( SideOfBayEnum &eSide, HandrailStyleEnum &eHandRailStyle, BayList *pBays )

/////////////////////////////////////////////////////////////////////////////////////////
//GetSideOfBayAndNumBoardsFromCommandLine( SideOfBayEnum &eSide, int &iNumberOfBoards, BayList *pBays, bool &bMatchToDecks )
//
bool Controller::GetSideOfBayAndNumBoardsFromCommandLine( SideOfBayEnum &eSide, int &iNumberOfBoards, BayList *pBays, bool &bMatchToDecks )
{
	bool	bHopups, bDecks;	
	TCHAR	keywrd[255];
	CString	sPrompt, sKeyWrd, sTemp, sTemp2;

	switch( eSide )
	{
	case( N ):
		sTemp = _T("N");
		break;
	case( E ):
		sTemp = _T("E");
		break;
	case( S ):
		sTemp = _T("S");
		break;
	case( W ):
		sTemp = _T("W");
		break;
	default:
		assert( false );
	}

	//Are there any hopups on the first bay?
	bHopups = false;
	bHopups|= pBays->GetAt(0)->HasComponentOfTypeOnSide(CT_STAGE_BOARD, N );
	bHopups|= pBays->GetAt(0)->HasComponentOfTypeOnSide(CT_STAGE_BOARD, E );
	bHopups|= pBays->GetAt(0)->HasComponentOfTypeOnSide(CT_STAGE_BOARD, S );
	bHopups|= pBays->GetAt(0)->HasComponentOfTypeOnSide(CT_STAGE_BOARD, W );

	//Are there any decks on the first bay?
	bDecks = pBays->GetAt(0)->HasComponentOfTypeOnSide(CT_DECKING_PLANK, S );

	if( !bHopups )
	{
		if( !bDecks )
		{
			CString sMsg;
			sMsg.Format( _T("There are no decks and no existing hopups on bay %i\n"), pBays->GetAt(0)->GetBayNumber() );
			sMsg+= _T("I don't have anything to match the new hopups to.\n");
			sMsg+= _T("You may overcome this limitation by using either the\n");
			sMsg+= _T("Edit Bay or the Visual Lift Editor dialogues.");
			MessageBox( NULL, sMsg, _T("No Decks or hopups"), MB_OK );
			return false;
		}
		if( !bMatchToDecks )
		{
			bMatchToDecks = true;
		}
	}
	if( !bDecks )
	{
		if( bMatchToDecks )
		{
			bMatchToDecks = false;
		}
	}

	if( bMatchToDecks )
	{
		sPrompt = _T("\nEnter no. of boards && side of bay [1/2/3/N/E/S/W");
		if( bHopups )
		{
			sPrompt+= _T("/match eXisting");
      sTemp2.Format( _T("] <%i,%s,D>:"), iNumberOfBoards, sTemp );
		}
		else
		{
      sTemp2.Format( _T("] <%i,%s>:"), iNumberOfBoards, sTemp );
		}
		sPrompt+= sTemp2;
	}
	else
	{
		sPrompt = _T("\nEnter no. of boards && side of bay [1/2/3/N/E/S/W");
		if( bDecks )
		{
			sPrompt+= _T("/match Decks");
      sTemp2.Format( _T("] <%i,%s,X>:"), iNumberOfBoards, sTemp );
		}
		else
		{
      sTemp2.Format( _T("] <%i,%s>:"), iNumberOfBoards, sTemp );
		}
		sPrompt+= sTemp2;
	}

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	Point3D			ptPoint;
	Matrix3D		Transform;
	intArray		iaAvailableSides;

	acedInitGet( RSG_OTHER, _T("N E S W"));
	int iRet = GetPoint3D( sPrompt, ptPoint);
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		//Fine
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			int iValue = _ttoi( keywrd );
			if( iValue>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE &&
				iValue<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE )
			{
				iNumberOfBoards = iValue;
			}
			else
			{
				CString sMsg;
				sMsg.Format( _T("Please Enter a number of Boards for the hopup, valid numbers are between %i and %i!"),
								NUM_PLANKS_FOR_STAGE_BRD_1_WIDE, NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox( NULL, sMsg, _T("Invalid Number of boards"), MB_OK );
			}
		}
		else
		{
			if( sKeyWrd==_T("N") )
			{
				eSide = N;
			}
			else if( sKeyWrd==_T("E") )
			{
				eSide = E;
			}
			else if( sKeyWrd==_T("S") )
			{
				eSide = S;
			}
			else if( sKeyWrd==_T("W") )
			{
				eSide = W;
			}
			else if( sKeyWrd==_T("D") )
			{
				if( bDecks )
				{
					//we have some decks, so allow the match to decks
					bMatchToDecks = true;
				}
			}
			else if( sKeyWrd==_T("X") )
			{
				if( bHopups )
				{
					//we have some hopups, so allow the match to eXisting
					bMatchToDecks = false;
				}
			}
			else 
			{
				//invalid input
				return false;
			}
		}
		return GetSideOfBayAndNumBoardsFromCommandLine( eSide, iNumberOfBoards, pBays, bMatchToDecks );

	case( RTNORM ):				//(5100) Request succeeded
		iaAvailableSides.RemoveAll();
		iaAvailableSides.Add( BP_N_MILLS );
		iaAvailableSides.Add( BP_E_MILLS );
		iaAvailableSides.Add( BP_S_MILLS );
		iaAvailableSides.Add( BP_W_MILLS );

		GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide );
		return GetSideOfBayAndNumBoardsFromCommandLine( eSide, iNumberOfBoards, pBays, bMatchToDecks );
	}
	return true;
}	//GetSideOfBayAndNumBoardsFromCommandLine

/////////////////////////////////////////////////////////////////////////////////////////
//GetRangeToApplyTies( Bay *pBay, int &iResponse, int iNumberOfRunsRemaining )
//
bool Controller::GetRangeToApplyTies( Bay *pBay, int &iResponse, int iNumberOfRunsRemaining )
{
	int				i;
	TCHAR			keywrd[255], prompt[255];
	CString			sPrompt, sKeyWrd;

	if( pBay->GetRunPointer()->GetNumberOfBays()<=1 )
	{
		iResponse = RESPONSE_NO;
		return true;
	}

	sPrompt.Format( _T("\nConsidering Bay %i, apply Tie to entire run? [Yes/No"),
						pBay->GetBayNumber() );

	if( iNumberOfRunsRemaining>1 )
	{
    sPrompt+= _T("/AllYes/AllNo] <Yes>:");
		acedInitGet( RSG_OTHER, _T("Yes No AllYes AllNo") );
	}
	else
	{
    sPrompt+= _T("] <Yes>:");
		acedInitGet( RSG_OTHER, _T("Yes No") );
	}

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');
	
	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	int iRet = acedGetKword( prompt, keywrd);
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;
	
	case( RTNONE ):				//fallthrough (5000) No result
		iResponse = RESPONSE_YES;
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd[0]==_T('Y') )
				iResponse = RESPONSE_YES;
			else if( sKeyWrd[0]==_T('N') )
				iResponse = RESPONSE_NO;
			else if( sKeyWrd[0]==_T('A') )
				iResponse = RESPONSE_ALL;
			else if( sKeyWrd==_T("NoAll") )
				iResponse = RESPONSE_NOALL;
			else 
			{
				//invalid input
				MessageBeep(MB_ICONEXCLAMATION);
				return false;
			}
			break;
		}
	}
	return true;
}	//GetRangeToApplyTies( Bay *pBay, int &iResponse, int iNumberOfRunsRemaining )

/////////////////////////////////////////////////////////////////////////////////////////
//ApplyTieToRun(Run *pRun, SideOfBayEnum eSide, RunList &CompletedRuns)
//
void Controller::ApplyTieToRun(Run *pRun, SideOfBayEnum eSide, RunList &CompletedRuns)
{
	int i, iBaysPerTie;
	double dVertical;

	//////////////////////////////////////////////////////
	//ask how many bays to space the ties
	iBaysPerTie = pRun->GetRunTemplate()->GetBaysPerTie();
	if( !GetNumberOfBaysPerTie( pRun, iBaysPerTie) )
		return;
	assert( iBaysPerTie>0 );
	pRun->GetRunTemplate()->SetBaysPerTie( iBaysPerTie );

	//////////////////////////////////////////////////////
	//ask how far appart to space the ties
	if( !GetVerticalTieSpacing( dVertical, pRun->GetTiesVerticallyEvery() ) )
		return;
	assert( dVertical>0 );
	pRun->SetTiesVerticallyEvery( dVertical );

	//Create a list of unique runs!
	for( i=0; i<pRun->GetNumberOfBays(); i++ )
	{
		//clear the existing template settings for this bay
		SetTemplateTiesForBay( pRun->GetBay(i), eSide);
	}
	//apply ties to this run
	pRun->SetBaysWithTies();

	//add the run to the list of completed runs
	AddRunToUniqueList( pRun, CompletedRuns );
}	//ApplyTieToRun(Run *pRun, SideOfBayEnum eSide, RunList &CompletedRuns)

/////////////////////////////////////////////////////////////////////////////////////////
//ApplyTieToBay(Bay *pBay, SideOfBayEnum eSide)
//
void Controller::ApplyTieToBay(Bay *pBay, SideOfBayEnum eSide)
{
	double		dHeight, dStandardsHeight, dVertical;

	if( !GetVerticalTieSpacing( dVertical, pBay->GetRunPointer()->GetTiesVerticallyEvery() ) )
		return;
	assert( dVertical>0 );

	//clear the existing template settings for this bay
	SetTemplateTiesForBay( pBay, eSide);

	dHeight = dVertical;
	dStandardsHeight = pBay->GetHeightOfStandards( CNR_SOUTH_EAST );

	double dTieLength;
	dTieLength = pBay->GetSmallestRequiredWallTieLength(eSide);

	if( dHeight>=dStandardsHeight )
	{
		//we need at least one tie
		pBay->GetRunPointer()->SetTiesIfPossible( pBay, dStandardsHeight, dTieLength, MT_STEEL );
	}
	else
	{
		while( dHeight<dStandardsHeight )
		{
			pBay->GetRunPointer()->SetTiesIfPossible( pBay, dHeight, dTieLength, MT_STEEL );
			dHeight+= dVertical;
		}
	}

	//redraw the schematic;
	pBay->UpdateSchematicView();
}	//ApplyTieToBay(Bay *pBay, SideOfBayEnum eSide)

/////////////////////////////////////////////////////////////////////////////////////////
//GetNumberOfBaysPerTie( Run *pRun, int &iBaysPerTie)
//
bool Controller::GetNumberOfBaysPerTie( Run *pRun, int &iBaysPerTie)
{
	int				i;
	bool			bPlural;
	TCHAR			prompt[255];
	CString			sPrompt, sTemp;


	iBaysPerTie = 4;

	bPlural = pRun->GetNumberOfBays()>1;
	sPrompt.Format( _T("How many Bays per Tie would you like to apply to Run number %i, which contains Bay <%i>"),
					pRun->GetRunID()+1, iBaysPerTie );
	if( bPlural )
	{
		sTemp.Format( _T("s %i"), pRun->GetBay(0)->GetBayNumber() );
		sPrompt+= sTemp;
		if( pRun->GetNumberOfBays()>2 )
		{
			sTemp.Format( _T("...%i"), pRun->GetBay(pRun->GetNumberOfBays()-1)->GetBayNumber() );
			sPrompt+= sTemp;
		}
		else
		{
			sTemp.Format( _T(" and %i"), pRun->GetBay(1)->GetBayNumber() );
			sPrompt+= sTemp;
		}
	}
	else
	{
		sTemp.Format( _T(" %i"), pRun->GetBay(0)->GetBayNumber() );
		sPrompt+= sTemp;
	}
  sTemp.Format( _T("? <%i>:"), DEFAULT_TIE_SPACING );
	sPrompt+= sTemp;

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	
	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	acedInitGet( RSG_NOZERO|RSG_NONEG, NULL );
	int iRet = acedGetInt( prompt, &iBaysPerTie);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;	//Unknown error
	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;
	case( RTNONE ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		//fine
		break;
	}
	return true;
}	//GetNumberOfBaysPerTie( Run *pRun, int &iBaysPerTie)

/////////////////////////////////////////////////////////////////////////////////////////
//GetVerticalTieSpacing( double &dVertical, double dDefaultSpacing )
//
bool Controller::GetVerticalTieSpacing( double &dVertical, double dDefaultSpacing )
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sTemp;
	ads_real		dResult;

  sPrompt.Format( _T("How far apart, vertically, would you like to space the ties? <%2.0f>:"),
						dDefaultSpacing );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');
	
	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	acedInitGet( RSG_NOZERO|RSG_NONEG, NULL );
	int iRet = acedGetReal( prompt, &dResult );
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error
	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;
	case( RTREAL ):				//fallthrough
	case( RTNORM ):				//(5100) Request succeeded
		dVertical = dResult;
		break;
	case( RTNONE ):				//(5100) Request succeeded
		dVertical = dDefaultSpacing;
		break;
	}
	return true;
}	//GetVerticalTieSpacing( double &dVertical, double dDefaultSpacing )

/////////////////////////////////////////////////////////////////////////////////////////
//IsComponentDetailsLoaded()
//
bool Controller::IsComponentDetailsLoaded()
{
	return m_bComponentDetailsLoaded;
}	//IsComponentDetailsLoaded()

/////////////////////////////////////////////////////////////////////////////////////////
//IsStockFileLoaded()
//
bool Controller::IsStockFileLoaded()
{
	return m_bStockFileLoaded;
}	//IsStockFileLoaded()

/////////////////////////////////////////////////////////////////////////////////////////
//SetStockFileLoaded(bool bLoaded/*=true*/)
//
void Controller::SetStockFileLoaded(bool bLoaded/*=true*/)
{
	m_bStockFileLoaded = bLoaded;
}	//SetStockFileLoaded(bool bLoaded/*=true*/)

/////////////////////////////////////////////////////////////////////////////////////////
//SetComponentDetailsLoaded(bool bLoaded/*=true*/)
//
void Controller::SetComponentDetailsLoaded(bool bLoaded/*=true*/)
{
	m_bComponentDetailsLoaded = bLoaded;
}	//SetComponentDetailsLoaded(bool bLoaded/*=true*/)



/////////////////////////////////////////////////////////////////////////////////////////
//AddComponentToSummaryInfo(SummaryInfo &sSLList, Component *pComponent/*=NULL*/)
//
void Controller::AddComponentToSummaryInfo(SummaryInfo &sSLList, Component *pComponent/*=NULL*/)
{
	int		iLevel;
	CString sStage;	//, sStageLevel;

	//we must have a valid pComponent!
	assert( pComponent!=NULL );

	sStage = pComponent->GetStage();
	iLevel = pComponent->GetLevel();

	sSLList.AddElement( sStage, iLevel, pComponent );
}	//AddComponentToSummaryInfo(SummaryInfo &sSLList, Component *pComponent/*=NULL*/)

/////////////////////////////////////////////////////////////////////////////////////////
//GetBayStageLevelArray()
//
BayStageLevelArray * Controller::GetBayStageLevelArray()
{
	return &m_StageListArray;
}	//GetBayStageLevelArray()

/////////////////////////////////////////////////////////////////////////////////////////
//GetMovementEntityType()
//
EntityTypeEnum Controller::GetMovementEntityType()
{
	return m_eMovementEntityType;
}	//GetMovementEntityType()

/////////////////////////////////////////////////////////////////////////////////////////
//GetErasureEntityType()
//
EntityTypeEnum Controller::GetErasureEntityType()
{
	return m_eEraseTypeEnum;
}	//GetErasureEntityType()

/////////////////////////////////////////////////////////////////////////////////////////
//SetMovementEntityType(EntityTypeEnum eType)
//
void Controller::SetMovementEntityType(EntityTypeEnum eType)
{
//	assert( m_eMovementEntityType==ET_NONE );
//	assert( m_eEraseTypeEnum==ET_NONE );
	if( !IsIgnorMovement() )
		m_eMovementEntityType = eType;
}	//SetMovementEntityType(EntityTypeEnum eType)

/////////////////////////////////////////////////////////////////////////////////////////
//SetErasureEntityType(EntityTypeEnum eType)
//
void Controller::SetErasureEntityType(EntityTypeEnum eType)
{
//	assert( m_eMovementEntityType==ET_NONE );
//	assert( m_eEraseTypeEnum==ET_NONE );
	if( !IsIgnorMovement() )
		m_eEraseTypeEnum = eType;
}	//SetErasureEntityType(EntityTypeEnum eType)

/////////////////////////////////////////////////////////////////////////////////////////
//ClearAllEntityTypes()
//
void Controller::ClearAllEntityTypes()
{
	SetMovementEntityType(ET_NONE);
	SetErasureEntityType(ET_NONE);
	m_iaEntityBayNumberIDs.RemoveAll();
	m_iaEntityLapboardIDs.RemoveAll();
	m_iaEntityLevelIDs.RemoveAll();
}	//ClearAllEntityTypes()

/////////////////////////////////////////////////////////////////////////////////////////
//AddEntityBayNumberID( int iBayNumberID )
//
void Controller::AddEntityBayNumberID( int iBayNumberID )
{
	int i;
	bool bFound;

	if( !IsIgnorMovement() )
	{
		bFound=false;
		for( i=0; i<m_iaEntityBayNumberIDs.GetSize(); i++ )
		{
			if( iBayNumberID==m_iaEntityBayNumberIDs.GetAt(i) )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
			m_iaEntityBayNumberIDs.Add( iBayNumberID );
	}
}	//AddEntityBayNumberID( int iBayNumberID )

/////////////////////////////////////////////////////////////////////////////////////////
//AddEntityLapboardID( int iLapboardID )
//
void Controller::AddEntityLapboardID( int iLapboardID )
{
	int i;
	bool bFound;

	if( !IsIgnorMovement() )
	{
		bFound=false;
		for( i=0; i<m_iaEntityLapboardIDs.GetSize(); i++ )
		{
			if( iLapboardID==m_iaEntityLapboardIDs.GetAt(i) )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
			m_iaEntityLapboardIDs.Add( iLapboardID );
	}
}	//AddEntityLapboardID( int iLapboardID )

/////////////////////////////////////////////////////////////////////////////////////////
//AddEntityLevelID( int iLevelID )
//
void Controller::AddEntityLevelID( int iLevelID )
{
	int i;
	bool bFound;

	if( !IsIgnorMovement() )
	{
		bFound=false;
		for( i=0; i<m_iaEntityLevelIDs.GetSize(); i++ )
		{
			if( iLevelID==m_iaEntityLevelIDs.GetAt(i) )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
			m_iaEntityLevelIDs.Add( iLevelID );
	}
}	//AddEntityLevelID( int iLevelID )

/////////////////////////////////////////////////////////////////////////////////////////
//GetEntityBayNumberID( )
//
intArray Controller::GetEntityBayNumberID( )
{
	return m_iaEntityBayNumberIDs;
}	//GetEntityBayNumberID( )

/////////////////////////////////////////////////////////////////////////////////////////
//GetEntityLapboardID( )
//
intArray Controller::GetEntityLapboardID( )
{
	return m_iaEntityLapboardIDs;
}	//GetEntityLapboardID( )

/////////////////////////////////////////////////////////////////////////////////////////
//GetEntityLevelID( )
//
intArray Controller::GetEntityLevelID( )
{
	return m_iaEntityLevelIDs;
}	//GetEntityLevelID( )

/////////////////////////////////////////////////////////////////////////////////////////
//SetMovedComponent(Component *pComponent)
//
void Controller::SetMovedComponent(Component *pComponent)
{
	if( !IsIgnorMovement() )
		m_MovedComponent = pComponent;
}	//SetMovedComponent(Component *pComponent)

/////////////////////////////////////////////////////////////////////////////////////////
//GetMovedComponent()
//
Component * Controller::GetMovedComponent()
{
	return m_MovedComponent;
}	//GetMovedComponent()

/////////////////////////////////////////////////////////////////////////////////////////
//IgnorMovement(bool bIgnor)
//
void Controller::IgnorMovement(bool bIgnor)
{
	//This should not be called several times with the same value
	assert( m_bIgnorMovement!=bIgnor ); //NOTE: harmless if startup!
	m_bIgnorMovement = bIgnor;
}	//IgnorMovement(bool bIgnor)

/////////////////////////////////////////////////////////////////////////////////////////
//IsIgnorMovement()
//
bool Controller::IsIgnorMovement()
{
	return m_bIgnorMovement;
}	//IsIgnorMovement()

/////////////////////////////////////////////////////////////////////////////////////////
//IgnorErase(bool bIgnor)
//
void Controller::IgnorErase(bool bIgnor)
{
	//This should not be called several times with the same value
	assert( m_bIgnorErase!=bIgnor ); //NOTE: harmless if startup!
	m_bIgnorErase = bIgnor;
}	//IgnorErase(bool bIgnor)

/////////////////////////////////////////////////////////////////////////////////////////
//IsIgnorErase()
//
bool Controller::IsIgnorErase()
{
	return m_bIgnorErase;
}	//IsIgnorErase()


/////////////////////////////////////////////////////////////////////////////////////////
//ChangeRLs()
//ChangeRLs
void Controller::ChangeRLs()
{
	int			i, j;
	Bay			*pBay;
	BayList		Bays, BaysInSameRun, SeqentialBays;
	Run			*pRun;

	Bays.RemoveAll();
	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false, true );
	RemoveAndWarnOfCommittedBays(&Bays);

	if( Bays.GetSize()<=0 )
	{
		MessageBeep(MB_ICONASTERISK);
		return;
	}

	if( BaysInSameRun.GetSize()>0 )
	{
		pBay = BaysInSameRun.GetAt(0);
		m_dLastRLSetValue = pBay->GetStandardPosition( CNR_SOUTH_WEST ).z;
	}

	Bays.DisplaySelectedBays();

	while( Bays.GetSize()>0 )
	{
		pBay = Bays.GetAt(0);
		pRun = pBay->GetRunPointer();

		//move the bay from the current list to the new list
		BaysInSameRun.RemoveAll();
		BaysInSameRun.Add( pBay );
		Bays.RemoveAt(0);

		//move all the other bays in the current list to the new list
		for( j=0; j<Bays.GetSize(); j++ )
		{
			pBay = Bays.GetAt(j);
			if( pBay->GetRunPointer()==pRun )
			{
				BaysInSameRun.Add( pBay );
				Bays.RemoveAt(j);
				j--;
			}
		}

		//sort the bays in ascending order
		for( i=0; i<BaysInSameRun.GetSize()-1; i++ )
		{
			for( j=i+1; j<BaysInSameRun.GetSize(); j++ )
			{
				if( BaysInSameRun.GetAt(i)->GetID()>BaysInSameRun.GetAt(j)->GetID() )
				{
					//out of order, swap them!
					pBay = BaysInSameRun.GetAt(j);
					BaysInSameRun.SetAt(j, BaysInSameRun.GetAt(i) );
					BaysInSameRun.SetAt(i, pBay );
				}
			}
		}

		bool bFound;
		int iSequence;

		while( BaysInSameRun.GetSize()>0 )
		{
			SeqentialBays.RemoveAll();
			pBay = BaysInSameRun.GetAt(0);
			SeqentialBays.Add( pBay );
			BaysInSameRun.RemoveAt(0);
			iSequence = pBay->GetID();
			bFound = true;
			while( bFound && BaysInSameRun.GetSize()>0 )
			{
				pBay = BaysInSameRun.GetAt(0);
				iSequence++;
				if( pBay->GetID()==iSequence )
				{
					//this is the next bay in the sequence
					SeqentialBays.Add( pBay );
					BaysInSameRun.RemoveAt(0);
				}
				else
				{
					//missed!
					bFound = false;
				}
			}

			if( !ApplyRLChangeToSequence( &SeqentialBays ) )
			{
				//user must have stopped the process!
				return;
			}
		}
	}
}	//ChangeRLs()

/////////////////////////////////////////////////////////////////////////////////////////
//ApplyRLChangeToSequence(BayList *pBays)
//
bool Controller::ApplyRLChangeToSequence(BayList *pBays)
{
	ChangeRLDlg	*Dialog;

	Dialog = new ChangeRLDlg();

	Dialog->SetBayList( pBays );
	Dialog->SetLastValue( &m_dLastRLSetValue );

	int iReturn;

	iReturn = Dialog->DoModal();

	DELETE_PTR(Dialog);
	if( iReturn==IDCANCEL || iReturn==IDCLOSE )
	{
		return false;
	}
	return true;
}	//ApplyRLChangeToSequence(BayList *pBays)



/////////////////////////////////////////////////////////////////////////////////////////
//AddHandrailsToBay()
//
void Controller::AddHandrailsToBay()
{
#ifndef	USE_BAYS_ADDHANDRAIL
	int				iBoards;
	Bay				*pNeighbor;
	bool			bSoleBoard;
	Lift			*pLift;
	double			dLength, dStdHgt, dReqdHgt;
/*	Component		*pComp;
*/	SideOfBayEnum	eSideHopupBracket1, eSideHopupBracket2,
					eSideHopup1, eSideHopup2,
					eStdNeighborLeft, eStdNeighborRight;
#endif	//#ifdef	USE_BAYS_ADDHANDRAIL
	int				i, iResponse;
	Bay				*pBay;
	CString			sMsg;
	BayList			Bays;
	SideOfBayEnum	eSide;
	SideOfBayEnum	eLeft, eRight,
					eStdSideLeft, eStdSideRight;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	Bays.GetSize();
	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		//choose the most likely side
		eSide = NORTH;
		if( Bays[0]->GetInner()==NULL && !Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, SOUTH ) )
			eSide = SOUTH;
		if( Bays[0]->GetForward()==NULL && !Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, EAST ) )
			eSide = EAST;
		if( Bays[0]->GetBackward()==NULL && !Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, WEST ) )
			eSide = WEST;
		if( Bays[0]->GetOuter()==NULL && !Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, NORTH ) )
			eSide = NORTH;

#ifdef	USE_BAYS_ADDHANDRAIL
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;
#else	//USE_BAYS_ADDHANDRAIL
		HandrailStyleEnum eHandRailStyle;
		eHandRailStyle = HRS_RAIL_ONLY;
		if( !GetSideOfBayAndHRStyleFromCommandLine( eSide,  eHandRailStyle, &Bays ) )
			return;
#endif	//USE_BAYS_ADDHANDRAIL

		//////////////////////////////////////////////////////
		//check for stage boards
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->
						HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
			{
				sMsg.Format( _T("Bay %i has a stage board on the %s side,"), pBay->GetBayNumber(), GetSideOfBayDescStr( eSide ) );
				sMsg+= _T("\nplacing handrails here will delete the stage board!");
				sMsg+= _T("\nReplace the stage board for this bay?");

				MessageBeep(MB_ICONQUESTION);
				iResponse = MessageBox( NULL, sMsg, _T("Existing Component Warning"), MB_YESNOCANCEL|MB_ICONQUESTION );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				else if( iResponse==IDCANCEL )
				{
					return;
				}

			}
		}

		//////////////////////////////////////////////////////
		//the size may have changed
		if( Bays.GetSize()>0 )
		{
			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				eLeft	= NNW;
				eRight	= NNE;
				eStdSideLeft		= NW;
				eStdSideRight		= NE;
				eStdNeighborLeft	= SW;
				eStdNeighborRight	= SE;
				break;
			case( EAST ):
				eLeft	= ENE;
				eRight	= ESE;
				eStdSideLeft		= NE;
				eStdSideRight		= SE;
				eStdNeighborLeft	= NW;
				eStdNeighborRight	= SW;
				break;
			case( SOUTH ):
				eLeft	= SSE;
				eRight	= SSW;
				eStdSideLeft		= SE;
				eStdSideRight		= SW;
				eStdNeighborLeft	= NE;
				eStdNeighborRight	= NW;
				break;
			case( WEST ):
				eLeft	= WSW;
				eRight	= WNW;
				eStdSideLeft		= SW;
				eStdSideRight		= NW;
				eStdNeighborLeft	= SE;
				eStdNeighborRight	= NE;
				break;
			}

			//Apply changes to all remaining bays (some may have been removed
			//	from the original list)
			for( i=0; i<Bays.GetSize(); i++ )
			{
				pBay = Bays[i];
#ifdef	USE_BAYS_ADDHANDRAIL
				pBay->AddHandrail(eSide);
#else	//USE_BAYS_ADDHANDRAIL
				eSideHopupBracket1	= SIDE_INVALID;
				eSideHopupBracket2	= SIDE_INVALID;
				eSideHopup1	= SIDE_INVALID;
				eSideHopup2	= SIDE_INVALID;
				pNeighbor = NULL;
				switch( eSide )
				{
				case( NORTH ):
					eSideHopupBracket1 = WNW;
					eSideHopupBracket2 = ENE;
					eSideHopup1 = W;
					eSideHopup2 = E;
					pNeighbor = pBay->GetOuter();
					dLength = pBay->GetBayLength();
					break;
				case( SOUTH ):
					eSideHopupBracket1 = WSW;
					eSideHopupBracket2 = ESE;
					eSideHopup1 = W;
					eSideHopup2 = E;
					pNeighbor = pBay->GetInner();
					dLength = pBay->GetBayLength();
					break;
				case( EAST ):
					eSideHopupBracket1 = NNE;
					eSideHopupBracket2 = SSE;
					eSideHopup1 = N;
					eSideHopup2 = S;
					pNeighbor = pBay->GetForward();
					dLength = pBay->GetBayWidth();
					break;
				case( WEST ):
					eSideHopupBracket1 = NNW;
					eSideHopupBracket2 = SSW;
					eSideHopup1 = N;
					eSideHopup2 = S;
					pNeighbor = pBay->GetBackward();
					dLength = pBay->GetBayWidth();
					break;
				default:
					assert( false );
				}

				///////////////////////////////////////////////////////////////////////
				//Remove existing handrails from this side
				pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
				if( eSide==NORTH || eSide==SOUTH )
					pBay->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );

				///////////////////////////////////////////////////////////////////////
				//add appropriate components to the required lifts
				for( int iLiftID=0; iLiftID<pBay->GetNumberOfLifts(); iLiftID++ )
				{
					pLift = pBay->GetLift( iLiftID );
					assert( pLift!=NULL );

					if( pLift->IsDeckingLift() ||
						pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
					{
						//this lift has either decking or a stage board
						pLift->AddHandrailToSide( eSide );
						if( eHandRailStyle==HRS_FULL_MESH )
						{
							if( pLift->GetLiftID()<pBay->GetNumberOfLifts()-1 )
							{
								//This is not the top lift, so we can add 2m mesh guard
								pLift->AddComponent( CT_MESH_GUARD, dLength, eSide, LIFT_RISE_2000MM, MT_STEEL );
							}
							pLift->AddComponent( CT_MESH_GUARD, dLength, eSide, LIFT_RISE_1000MM, MT_STEEL );
						}
						else if( eHandRailStyle==HRS_HALF_MESH )
						{
							pLift->AddComponent( CT_MESH_GUARD, dLength, eSide, LIFT_RISE_1000MM, MT_STEEL );
						}
					}

					/////////////////////////////////////////////////////////////////////////
					//hopup bracket handrails
					if(pNeighbor==NULL)
					{
						iBoards = pLift->GetNumberOfPlanksOnStageBoard( eSideHopup1 );
						if( iBoards>0 && eSideHopupBracket1!=SIDE_INVALID )
						{
							//make sure we don't already have one!
							pLift->Add1000mmHopupIfReqd(eSideHopupBracket1);
/*							pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideHopupBracket1 );
							if( pComp==NULL )
								pLift->AddComponent( CT_HOPUP_BRACKET, (double)iBoards, eSideHopupBracket1, LIFT_RISE_1000MM, MT_STEEL );
*/						}
						iBoards = pLift->GetNumberOfPlanksOnStageBoard( eSideHopup2 );
						if( iBoards>0 && eSideHopupBracket2!=SIDE_INVALID )
						{
							pLift->Add1000mmHopupIfReqd(eSideHopupBracket2);
/*							//make sure we don't already have one!
							pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_2000MM, eSideHopupBracket2 );
							if( pComp==NULL )
								pLift->AddComponent( CT_HOPUP_BRACKET, (double)iBoards, eSideHopupBracket2, LIFT_RISE_2000MM, MT_STEEL );
*/						}
					}
				}

				///////////////////////////////////////////////////////////////////////
				//Check the standards are correct
				//Left
				dStdHgt		= pBay->GetHeightOfStandards( SideOfBayAsCorner( eStdSideLeft ) );
				if( pNeighbor!=NULL )
				{
					dReqdHgt	= max( pBay->GetRLOfTopLiftPlusHandrail( eStdSideLeft ), pNeighbor->GetRLOfTopLiftPlusHandrail( eStdNeighborLeft ) )
								- min( pBay->GetStandardPosition( SideOfBayAsCorner( eStdSideLeft ) ).z,  pNeighbor->GetStandardPosition( SideOfBayAsCorner( eStdNeighborLeft ) ).z );
				}
				else
				{
					dReqdHgt	= pBay->GetRLOfTopLiftPlusHandrail( eStdSideLeft ) 
								- pBay->GetStandardPosition( SideOfBayAsCorner( eStdSideLeft ) ).z;
				}
				bSoleBoard	= pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eStdSideLeft );
				if( dStdHgt<dReqdHgt-ROUND_ERROR || dStdHgt>dReqdHgt+ROUND_ERROR )
				{
					pBay->CreateStandardsArrangment( dReqdHgt, SideOfBayAsCorner( eStdSideLeft ), bSoleBoard );
				}

				//Right
				dStdHgt		= pBay->GetHeightOfStandards( SideOfBayAsCorner( eStdSideRight ) );
				if( pNeighbor!=NULL )
				{
					dReqdHgt	= max( pBay->GetRLOfTopLiftPlusHandrail( eStdSideRight ), pNeighbor->GetRLOfTopLiftPlusHandrail( eStdNeighborRight ) )
								- min( pBay->GetStandardPosition( SideOfBayAsCorner( eStdSideRight ) ).z,  pNeighbor->GetStandardPosition( SideOfBayAsCorner( eStdNeighborRight ) ).z );
				}
				else
				{
					dReqdHgt	= pBay->GetRLOfTopLiftPlusHandrail( eStdSideRight ) 
								- pBay->GetStandardPosition( SideOfBayAsCorner( eStdSideRight ) ).z;
				}
				bSoleBoard	= pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eStdSideRight );
				if( dStdHgt<dReqdHgt-ROUND_ERROR || dStdHgt>dReqdHgt+ROUND_ERROR )
				{
					pBay->CreateStandardsArrangment( dReqdHgt, SideOfBayAsCorner( eStdSideRight ), bSoleBoard );
				}

/*
				///////////////////////////////////////////////////////////////////////
				//JSB Note 20000528 - I have removed this because I just added
				//	a handrail between two adjacent bays, and these functions
				//	have overridden my new setting, I can't recall why this was
				//	added in the first place
				pBay->EnsureHandrailsAreCorrect( WEST );
				pBay->EnsureHandrailsAreCorrect( EAST );
				if( pBay->GetForward()!=NULL )
					pBay->GetForward()->EnsureHandrailsAreCorrect( WEST );
				if( pBay->GetBackward()!=NULL )
					pBay->GetBackward()->EnsureHandrailsAreCorrect( EAST );
*/					

#endif		//USE_BAYS_ADDHANDRAIL
				//redraw the schematic;
				pBay->UpdateSchematicView();
			}
		}
	}
}	//AddHandrailsToBay()

/////////////////////////////////////////////////////////////////////////////////////////
//AddHopupsToBay()
//
void Controller::AddHopupsToBay()
{
	int				i, iResponse, iNumberOfPlanks;
	Bay				*pBay, *pNeighborBay;
	bool			bMatchToDecks, bAddToThisLift;
	CString			sMsg;
	BayList			Bays;
	SideOfBayEnum	eSide, eLeft, eRight, eLeftHopup, eRightHopup;

	if( !SelectBays( Bays, true ) )
		return;
	RemoveAndWarnOfCommittedBays(&Bays);

	CheckForStairLadder( &Bays, false );

	Bays.GetSize();
	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		eSide = SOUTH;
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			assert( pBay );
			iNumberOfPlanks = pBay->GetNumberOfPlanksOfBottomStage(eSide);
			if( iNumberOfPlanks>0 )
				break;
		}
		if( iNumberOfPlanks>0 )
			eSide = NORTH;
		iNumberOfPlanks = max( iNumberOfPlanks, NUM_PLANKS_FOR_STAGE_BRD_1_WIDE );

		//Default to match to existing hopups
		bMatchToDecks = false;
		if( !GetSideOfBayAndNumBoardsFromCommandLine( eSide, iNumberOfPlanks, &Bays, bMatchToDecks ) )
			return;

		//check for bracing, shade cloth and chainmesh, and handrails
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			CString			sMsg, sTemp;
			CStringArray	saComps;

			saComps.RemoveAll();
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->HasComponentOfTypeOnSide( CT_MESH_GUARD,		eSide ) ) saComps.Add(GetComponentDescStr(CT_MESH_GUARD));
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->HasComponentOfTypeOnSide( CT_RAIL,			eSide ) ) saComps.Add(GetComponentDescStr(CT_RAIL));
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->HasComponentOfTypeOnSide( CT_MID_RAIL,		eSide ) ) saComps.Add(GetComponentDescStr(CT_MID_RAIL));
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->HasComponentOfTypeOnSide( CT_TOE_BOARD,		eSide ) ) saComps.Add(GetComponentDescStr(CT_TOE_BOARD));
			if( pBay->HasBayComponentOfTypeOnSide( CT_CHAIN_LINK,	eSide ) ) saComps.Add(GetComponentDescStr(CT_CHAIN_LINK));
			if( pBay->HasBayComponentOfTypeOnSide( CT_SHADE_CLOTH,	eSide ) ) saComps.Add(GetComponentDescStr(CT_SHADE_CLOTH));
			if( pBay->HasBayComponentOfTypeOnSide( CT_BRACING,		eSide ) ) saComps.Add(GetComponentDescStr(CT_BRACING));

			if( saComps.GetSize()>0 )
			{
        int j=0;
				sMsg.Format( _T("Bay %i has "), pBay->GetBayNumber() );
				for( j=0; j<saComps.GetSize()-2; j++ )
				{
					sMsg+= saComps.GetAt(j);
					sMsg+= _T(", ");
				}
				for( ; j<saComps.GetSize()-1; j++ )
				{
					sMsg+= saComps.GetAt(j);
					sMsg+= _T(" and ");
				}
				sMsg+= saComps.GetAt(j);
				sTemp.Format( _T("\n on the %s side of the bay!"), GetSideOfBayDescStr( eSide ) );
				sMsg+= sTemp;

				if( saComps.GetSize()>1 )
				{
					sMsg+= _T("\nplacing a hopup here will delete these components!");
					sMsg+= _T("\n\nContinue and Replace these components?");
				}
				else
				{
					sMsg+= _T("\nplacing a hopup here will delete that component!");
					sMsg+= _T("\n\nContinue and Replace these components?");
				}

				MessageBeep(MB_ICONQUESTION);
				iResponse = MessageBox( NULL, sMsg, _T("Existing Component Warning"), MB_YESNOCANCEL|MB_ICONQUESTION );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				else if( iResponse==IDCANCEL )
				{
					return;
				}

			}
		}

		//check for neighboring bays
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];

			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				pNeighborBay = pBay->GetOuter();
				break;
			case( EAST ):
				pNeighborBay = pBay->GetForward();
				break;
			case( SOUTH ):
				pNeighborBay = pBay->GetInner();
				break;
			case( WEST ):
				pNeighborBay = pBay->GetBackward();
				break;
			}

			if( pNeighborBay!=NULL )
			{
				sMsg.Format( _T("Bay %i is to the %s of Bay %i, therefore we cannot place a hopup to the %s of Bay %i!"),
					pNeighborBay->GetBayNumber(), GetSideOfBayDescStr( eSide ), pBay->GetBayNumber(),
					GetSideOfBayDescStr( eSide ), pBay->GetBayNumber() );
				MessageBox( NULL, sMsg, _T("Existing Bay Warning"), MB_ICONSTOP );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				if( iResponse==IDCANCEL )
					return;

			}
		}

		//the size may have changed
		if( Bays.GetSize()>0 )
		{
			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				eLeft		= NNW;
				eRight		= NNE;
				eLeftHopup	= WNW;
				eRightHopup	= ENE;
				break;
			case( EAST ):
				eLeft		= ENE;
				eRight		= ESE;
				eLeftHopup	= NNE;
				eRightHopup	= SSE;
				break;
			case( SOUTH ):
				eLeft		= SSE;
				eRight		= SSW;
				eLeftHopup	= ESE;
				eRightHopup	= WSW;
				break;
			case( WEST ):
				eLeft		= WSW;
				eRight		= WNW;
				eLeftHopup	= SSW;
				eRightHopup	= NNW;
				break;
			}

			//apply changes to all remaining bays (some may have been removed
			//	from the original list)
			Lift	*pLift;
			for( i=0; i<Bays.GetSize(); i++ )
			{
				pBay = Bays[i];

				//remove any obsticles
				pBay->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD,	eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,	eLeft );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,	eRight );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD,		eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL,			eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL,		eSide );
				if( eSide==NORTH || eSide==SOUTH )
					pBay->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD,		eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK,		eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH,	eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING,		eSide );

				for( int iLiftID=0; iLiftID<pBay->GetNumberOfLifts(); iLiftID++ )
				{
					pLift = pBay->GetLift( iLiftID );
					assert( pLift!=NULL );

					bAddToThisLift = false;
					if( bMatchToDecks )
					{
						//We are matching the hopups to the decks
						bAddToThisLift = pLift->IsDeckingLift();
					}
					else
					{
						//We are matching the hopups to the existing hopups
						bAddToThisLift =(	pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) ||
											pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) ||
											pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, SOUTH ) ||
											pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) );
					}

					if( bAddToThisLift )
					{
						//This lift has either decking or a stage board
						pLift->AddStage( eSide, iNumberOfPlanks, MT_STEEL );

						//Remove any stop ends that are no longer needed!
						//JSB Note 20000614 - These NULL values may be incorrect!
						pLift->DeleteUnneeded1000mmHopup( NULL, eLeft,			eSide );
						pLift->DeleteUnneeded1000mmHopup( NULL, eRight,			eSide );
						pLift->DeleteUnneeded1000mmHopup( NULL, eLeftHopup,		eSide );
						pLift->DeleteUnneeded1000mmHopup( NULL, eRightHopup,	eSide );
					}
				}

				pBay->EnsureHandrailsAreCorrect( eSide );

				//redraw the schematic;
				pBay->UpdateSchematicView();
			}
		}
	}
}	//AddHopupsToBay()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteHandrails()
//
void Controller::DeleteHandrails()
{
	int				i, iSize;
	Bay				*pBay;
	BayList			Bays;
	SideOfBayEnum	eSide, eSideLeft, eSideRight;
	CornerOfBayEnum	eCnrLeft, eCnrRight;

	if( !SelectBays( Bays, true ) )
		return;

	eSide = NORTH;

	if( Bays.GetSize()>0 )
	{
		//Make the suggestion more intelligent
		if( Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, NORTH ) )
			eSide = NORTH;
		if( Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, WEST ) )
			eSide = WEST;
		if( Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, EAST ) )
			eSide = EAST;
		if( Bays[0]->HasComponentOfTypeOnSide( CT_RAIL, SOUTH ) )
			eSide = SOUTH;

		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		CheckForStairLadder( &Bays, false );
		RemoveAndWarnOfCommittedBays(&Bays);

		eSideLeft	= SIDE_INVALID;
		eSideRight	= SIDE_INVALID;
		switch( eSide )
		{
		case( N ):
			eSideLeft	= WNW;
			eSideRight	= ENE;
			eCnrLeft	= CNR_NORTH_WEST;
			eCnrRight	= CNR_NORTH_EAST;
			break;
		case( E ):
			eSideLeft	= NNE;
			eSideRight	= SSE;
			eCnrLeft	= CNR_NORTH_EAST;
			eCnrRight	= CNR_SOUTH_EAST;
			break;
		case( S ):
			eSideLeft	= ESE;
			eSideRight	= WSW;
			eCnrLeft	= CNR_SOUTH_EAST;
			eCnrRight	= CNR_SOUTH_WEST;
			break;
		case( W ):
			eSideLeft	= SSW;
			eSideRight	= NNW;
			eCnrLeft	= CNR_SOUTH_WEST;
			eCnrRight	= CNR_NORTH_WEST;
			break;
		default:
			assert( false );
		}

		iSize = Bays.GetSize();
		if( iSize>0 )
		{
			Bays.DisplaySelectedBays();

			for( i=0; i<iSize; i++ )
			{
				pBay = Bays.GetAt(i);
				pBay->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
				if( eSide==NORTH || eSide==SOUTH )
					pBay->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
				pBay->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );
				pBay->Delete1000mmHopupRailFromSide( eSideLeft );
				pBay->Delete1000mmHopupRailFromSide( eSideRight );
				pBay->AutoAdjustStandard( eCnrLeft );
				pBay->AutoAdjustStandard( eCnrRight );
				pBay->UpdateSchematicView();
			}
		}
	}
}	//DeleteHandrails()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteHopups()
//
void Controller::DeleteHopups()
{
	int				i, j, iSize, iLength, iSide;
	Bay				*pBay, *pReliantBay;
	Lift			*pReliantLift;
	BayList			Bays;
	SideOfBayEnum	eSide, eSideUnOwned, eSideOwned, eCnrHopupLeft, eCnrHopupRight;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		//Find the first side that has a hopup!
		eSide = SIDE_INVALID;
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
			{
				if( pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, (SideOfBayEnum)iSide ) )
				{
					eSide = (SideOfBayEnum)iSide;
					break;
				}
			}

			if( eSide!=SIDE_INVALID )
			{
				//we found one
				break;
			}
		}

		if( eSide==SIDE_INVALID )
		{
			CString sMsg;
			sMsg = _T("Your selection set does not include any bays that\n");
			sMsg = _T("have hopups.  Therefore we cannot remove any hopups!\n\n");
			sMsg = _T("I have stopped your 'DeleteHopups' command!");
      MessageBox( NULL, sMsg, _T("Error: Invalid Selection!"), MB_OK|MB_ICONSTOP );
			return;
		}

		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		switch( eSide )
		{
		case( NORTH ):
			eSideUnOwned	= NNW;
			eSideOwned		= NNE;
			eCnrHopupLeft	= NW;
			eCnrHopupRight	= NE;
			break;
		case( EAST ):
			eSideUnOwned	= ESE;
			eSideOwned		= ENE;
			eCnrHopupLeft	= NE;
			eCnrHopupRight	= SE;
			break;
		case( SOUTH ):
			eSideUnOwned	= SSW;
			eSideOwned		= SSE;
			eCnrHopupLeft	= SE;
			eCnrHopupRight	= SW;
			break;
		case( WEST ):
			eSideUnOwned	= WSW;
			eSideOwned		= WNW;
			eCnrHopupLeft	= SW;
			eCnrHopupRight	= NW;
			break;
		default:
			assert( false );
			return;
		}

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays.GetAt(i);
			switch( eSide )
			{
			case( NORTH ):
				pReliantBay = pBay->GetForward();
				break;
			case( EAST ):
				pReliantBay = pBay->GetOuter();
				break;
			case( SOUTH ):
				pReliantBay = pBay->GetForward();
				break;
			case( WEST ):
				pReliantBay = pBay->GetOuter();
				break;
			default:
				assert( false );
				return;
			}

			pBay->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD,	eSide );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,	eSideUnOwned );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,	eSideOwned );

			//We need to remove any corner stage boards too!
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	eCnrHopupLeft );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	eCnrHopupRight );

			if( pReliantBay!=NULL )
			{
				for( j=0; j<pReliantBay->GetNumberOfLifts(); j++ )
				{
					pReliantLift = pReliantBay->GetLift( j );
					if( !pReliantLift->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSideUnOwned ) )
					{
						//it appears we have remove the hopup bracket that was being used
						iLength = pReliantLift->GetNumberOfPlanksOnStageBoard( eSide );
						if( iLength>0 )
							pReliantLift->AddComponent( CT_HOPUP_BRACKET, iLength, eSideUnOwned, LIFT_RISE_0000MM, MT_STEEL );
					}
				}
			}

			pBay->UpdateSchematicView();
		}
	}
}	//DeleteHopups()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteShadeClothFromBay()
//
void Controller::DeleteShadeClothFromBay()
{
	int			i, iSize;
	Bay			*pBay;
	BayList		Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays.GetAt(i);
			pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, NORTH );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, EAST );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, SOUTH );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, WEST );
			pBay->UpdateSchematicView();
		}
	}
}	//DeleteShadeClothFromBay()

/////////////////////////////////////////////////////////////////////////////////////////
//DeleteChainLinkFromBay()
//
void Controller::DeleteChainLinkFromBay()
{
	int			i, iSize;
	Bay			*pBay;
	BayList		Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	iSize = Bays.GetSize();
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays.GetAt(i);
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, NORTH );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, EAST );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, SOUTH );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, WEST );
			pBay->UpdateSchematicView();
		}
	}
}	//DeleteChainLinkFromBay()

/////////////////////////////////////////////////////////////////////////////////////////
//AddShadeClothToBay()
//
void Controller::AddShadeClothToBay()
{
	int				i, iResponse;
	Bay				*pBay, *pNeighborBay;
	CString			sMsg;
	BayList			Bays;
	SideOfBayEnum	eSide, eLeft, eRight;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	Bays.GetSize();
	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		//check for stage boards
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->
						HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
			{
				sMsg.Format( _T("Bay %i has a stage board on the %s side,"), pBay->GetBayNumber(), GetSideOfBayDescStr( eSide ) );
				sMsg+= _T("\nplacing Shade cloth here will delete the stage board!");
				sMsg+= _T("\nReplace the stage board for this bay?");

				MessageBeep(MB_ICONQUESTION);
				iResponse = MessageBox( NULL, sMsg, _T("Existing Component Warning"), MB_YESNOCANCEL|MB_ICONQUESTION );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				else if( iResponse==IDCANCEL )
				{
					return;
				}

			}
		}

		//check for neighboring bays
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];

			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				pNeighborBay = pBay->GetOuter();
				break;
			case( EAST ):
				pNeighborBay = pBay->GetForward();
				break;
			case( SOUTH ):
				pNeighborBay = pBay->GetInner();
				break;
			case( WEST ):
				pNeighborBay = pBay->GetBackward();
				break;
			}
			if( pNeighborBay!=NULL )
			{
				sMsg.Format( _T("Bay %i is to the %s of Bay %i, therefore we cannot place shade cloth to the %s of Bay %i!"),
					pNeighborBay->GetBayNumber(), GetSideOfBayDescStr( eSide ), pBay->GetBayNumber(),
					GetSideOfBayDescStr( eSide ), pBay->GetBayNumber() );
				MessageBox( NULL, sMsg, _T("Existing Bay Warning"), MB_ICONSTOP );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				if( iResponse==IDCANCEL )
					return;

			}
		}

		//the size may have changed
		if( Bays.GetSize()>0 )
		{
			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				eLeft	= NNW;
				eRight	= NNE;
				break;
			case( EAST ):
				eLeft	= ENE;
				eRight	= ESE;
				break;
			case( SOUTH ):
				eLeft	= SSE;
				eRight	= SSW;
				break;
			case( WEST ):
				eLeft	= WSW;
				eRight	= WNW;
				break;
			}

			//apply changes to all remaining bays (some may have been removed
			//	from the original list)
			for( i=0; i<Bays.GetSize(); i++ )
			{
				pBay = Bays[i];

				//remove any obsticles
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eLeft );
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eRight );

				//create the bracing
				pBay->AddComponent( CT_SHADE_CLOTH, eSide, 0.00, 0.00, MT_STEEL );

				//redraw the schematic;
				pBay->UpdateSchematicView();
			}
		}
	}
}	//AddShadeClothToBay()

/////////////////////////////////////////////////////////////////////////////////////////
//AddChainLinkToBay()
//
void Controller::AddChainLinkToBay()
{
	int				i, iResponse;
	Bay				*pBay, *pNeighborBay;
	CString			sMsg;
	BayList			Bays;
	SideOfBayEnum	eSide, eLeft, eRight;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays, false );
	RemoveAndWarnOfCommittedBays(&Bays);

	Bays.GetSize();
	if( Bays.GetSize()>0 )
	{
		Bays.DisplaySelectedBays();

		eSide = NORTH;
		if( !GetSideOfBayFromCommandLine( eSide, &Bays ) )
			return;

		//check for stage boards
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			if( pBay->GetLift(pBay->GetNumberOfLifts()-1)->
						HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
			{
				sMsg.Format( _T("Bay %i has a stage board on the %s side,"), pBay->GetBayNumber(), GetSideOfBayDescStr( eSide ) );
				sMsg+= _T("\nplacing Chain Mesh here will delete the stage board!");
				sMsg+= _T("\nReplace the stage board for this bay?");

				MessageBeep(MB_ICONQUESTION);
				iResponse = MessageBox( NULL, sMsg, _T("Existing Component Warning"), MB_YESNOCANCEL|MB_ICONQUESTION );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				else if( iResponse==IDCANCEL )
				{
					return;
				}

			}
		}

		//check for neighboring bays
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];

			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				pNeighborBay = pBay->GetOuter();
				break;
			case( EAST ):
				pNeighborBay = pBay->GetForward();
				break;
			case( SOUTH ):
				pNeighborBay = pBay->GetInner();
				break;
			case( WEST ):
				pNeighborBay = pBay->GetBackward();
				break;
			}
			if( pNeighborBay!=NULL )
			{
				sMsg.Format( _T("Bay %i is to the %s of Bay %i, therefore we cannot place Chain Mesh to the %s of Bay %i!"),
					pNeighborBay->GetBayNumber(), GetSideOfBayDescStr( eSide ), pBay->GetBayNumber(),
					GetSideOfBayDescStr( eSide ), pBay->GetBayNumber() );
				MessageBox( NULL, sMsg, _T("Existing Bay Warning"), MB_ICONSTOP );
				if( iResponse==IDNO )
				{
					//Delete this bay from the list of bays
					Bays.RemoveAt(i);
					i--;
					continue;
				}
				if( iResponse==IDCANCEL )
					return;

			}
		}

		//the size may have changed
		if( Bays.GetSize()>0 )
		{
			switch( eSide )
			{
			default:
				//invalid side
				assert( false );
				return;

			case( NORTH ):
				eLeft	= NNW;
				eRight	= NNE;
				break;
			case( EAST ):
				eLeft	= ENE;
				eRight	= ESE;
				break;
			case( SOUTH ):
				eLeft	= SSE;
				eRight	= SSW;
				break;
			case( WEST ):
				eLeft	= WSW;
				eRight	= WNW;
				break;
			}

			//apply changes to all remaining bays (some may have been removed
			//	from the original list)
			for( i=0; i<Bays.GetSize(); i++ )
			{
				pBay = Bays[i];

				//remove any obsticles
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
				if( !pBay->GetUseMidrailWithChainMesh() )
					pBay->DeleteAllBayComponentsOfTypeFromSide( CT_MID_RAIL, eSide );

				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eLeft );
				pBay->DeleteAllBayComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eRight );

				//create the bracing
				pBay->AddComponent( CT_CHAIN_LINK, eSide, 0.00, 0.00, MT_STEEL );

				//redraw the schematic;
				pBay->UpdateSchematicView();
			}
		}
	}
}	//AddChainLinkToBay()


/////////////////////////////////////////////////////////////////////////////////////////
//TracePointers()
//
void Controller::TracePointers()
{
#ifdef _DEBUG
	int	iRun, iBay;
	Run	*pRun;
	Bay	*pBay;
	acutPrintf( _T("\nTracing Pointers"));
	for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
	{
		pRun = GetRun( iRun );
		acutPrintf( _T("\n\tRun%i"), iRun );
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay( iBay );
			acutPrintf( _T("\n\t\tBay%i"), iBay );

			if( pBay->GetForward()==NULL )
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Forward = NULL"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID() );
			else
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Forward = Bay%i(%i,%i)"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID()
								, pBay->GetForward()->GetBayNumber(), pBay->GetForward()->GetRunPointer()->GetRunID(), pBay->GetForward()->GetID() );

			if( pBay->GetBackward()==NULL )
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Backward = NULL"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID() );
			else
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Backward = Bay%i(%i,%i)"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID()
								, pBay->GetBackward()->GetBayNumber(), pBay->GetBackward()->GetRunPointer()->GetRunID(), pBay->GetBackward()->GetID() );

			if( pBay->GetInner()==NULL )
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Inner = NULL"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID() );
			else
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Inner = Bay%i(%i,%i)"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID()
								, pBay->GetInner()->GetBayNumber(), pBay->GetInner()->GetRunPointer()->GetRunID(), pBay->GetInner()->GetID() );

			if( pBay->GetOuter()==NULL )
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Outer = NULL"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID() );
			else
				acutPrintf( _T("\n\t\t\tBay%i(%i,%i) Outer = Bay%i(%i,%i)"), pBay->GetBayNumber(), pBay->GetRunPointer()->GetRunID(), pBay->GetID()
								, pBay->GetOuter()->GetBayNumber(), pBay->GetOuter()->GetRunPointer()->GetRunID(), pBay->GetOuter()->GetID() );

			assert( pBay->GetTemplate()!=NULL );
		}
	}
#endif	//#ifdef _DEBUG
}	//TracePointers()

/////////////////////////////////////////////////////////////////////////////////////////
//GetJobDescription()
//
JobDescriptionInfo * Controller::GetJobDescription()
{
	return &m_JobDescripInfo;
}	//GetJobDescription()

/////////////////////////////////////////////////////////////////////////////////////////
//UpdateAllBayText()
//
void Controller::UpdateAllBayText()
{
	int	i, iBayID;
	Run	*pRun;
	Bay	*pBay;

	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun = GetRun( i );
		assert( pRun!=NULL );
		for( iBayID=0; iBayID<pRun->GetNumberOfBays(); iBayID++ )
		{
			pBay = pRun->GetBay(iBayID);
			assert( pBay!=NULL );
			assert( pBay->GetTemplate()!=NULL );
			pBay->GetTemplate()->SetAllSchematicText();
		}
	}
}	//UpdateAllBayText()

/////////////////////////////////////////////////////////////////////////////////////////
//GetArcLapboardSpan()
//
double Controller::GetArcLapboardSpan()
{
	return m_dArcLapboardSpan;
}	//GetArcLapboardSpan()

/////////////////////////////////////////////////////////////////////////////////////////
//SetArcLapSpan()
//
void Controller::SetArcLapSpan()
{
	int				i;
	TCHAR			keywrd[255], prompt[255];
	double			dSpan;
	CString			sPrompt, sKeyWrd, sMessage;

	dSpan = GetArcLapboardSpan();
	while( true )
	{
		///////////////////////////////////////////////////
		//Construct the command prompt line
		sPrompt.Format( _T("\nPlease Set the separation value for the Arc Lapboards <%0.1f>"), dSpan );

		for( i=0; i<sPrompt.GetLength(); i++ )
		{
			prompt[i] = sPrompt[i];
		}
		prompt[i] = _T('\0');

		acedInitGet( RSG_OTHER, _T("Span"));
		int iRet = acedGetKword( prompt, keywrd);
		switch( iRet )
		{
		case( RTERROR ):			//fallthrough (-5001) Some other error
		default:
			//This should not have occured
			MessageBeep(MB_ICONERROR);
			continue;	//Unknown error
		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		case( RTNONE ):				//(5000) No result
			return;
		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		case( RTNORM ):				//(5100) Request succeeded
			sKeyWrd = keywrd;
			sKeyWrd.MakeUpper();

			if( !_istdigit( sKeyWrd[0] ) )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sMessage.Format( _T("You must give a number between %0.1f and %0.1f!"),
								COMPONENT_LENGTH_0500, COMPONENT_LENGTH_1500 );
				MessageBox( NULL, sMessage, _T("Invalid Span"), MB_OK );
				continue;	//invalid data, we need a number
			}
			dSpan = _tstof( sKeyWrd );
			if( dSpan>COMPONENT_LENGTH_1500 ||
				dSpan<COMPONENT_LENGTH_0500 )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sMessage.Format( _T("You must give a number between %0.1f and %0.1f!"),
								COMPONENT_LENGTH_0500, COMPONENT_LENGTH_1500 );
				MessageBox( NULL, sMessage, _T("Invalid Span"), MB_OK );
				continue;	//invalid data, we need a number
			}
			SetArcLapboardSpan(dSpan);
			return;
		}
	}
}	//SetArcLapSpan()

/////////////////////////////////////////////////////////////////////////////////////////
//SetArcLapboardSpan(double dSpan)
//
void Controller::SetArcLapboardSpan(double dSpan)
{
	dSpan = min( dSpan, COMPONENT_LENGTH_1500 );
	dSpan = max( dSpan, COMPONENT_LENGTH_0500 );
	m_dArcLapboardSpan = dSpan;
}	//SetArcLapboardSpan(double dSpan)


/////////////////////////////////////////////////////////////////////////////////////////
//ToggleLvlLength()
//
void Controller::ToggleLvlLength()
{
	m_bShowLongLevelLength = !m_bShowLongLevelLength;
}	//ToggleLvlLength()

/////////////////////////////////////////////////////////////////////////////////////////
//ToggleFirstScale()
//
void Controller::ToggleFirstScale()
{
	m_bShowFirstScale = !m_bShowFirstScale;
}	//ToggleFirstScale()

/////////////////////////////////////////////////////////////////////////////////////////
//ToggleScaleLines()
//
void Controller::ToggleScaleLines()
{
	m_bShowScaleLines = !m_bShowScaleLines;
}	//ToggleScaleLines()

/////////////////////////////////////////////////////////////////////////////////////////
//ShowLongLevelLine()
//
bool Controller::ShowLongLevelLine()
{
	return m_bShowLongLevelLength;
}	//ShowLongLevelLine()

/////////////////////////////////////////////////////////////////////////////////////////
//ShowFirstScale()
//
bool Controller::ShowFirstScale()
{
	return m_bShowFirstScale;
}	//ShowFirstScale()

/////////////////////////////////////////////////////////////////////////////////////////
//ShowScaleLines()
//
bool Controller::ShowScaleLines()
{
	return m_bShowScaleLines;
}	//ShowScaleLines()

/////////////////////////////////////////////////////////////////////////////////////////
//ClearCommitted()
//
void Controller::ClearCommitted()
{
	int			iRID, iBID, iLID, iCID;
	Run			*pR;
	Bay			*pB;
	Lift		*pL;
	Component	*pC;

	///////////////////////////////////////////////////////////////////
	//Warning
	CString sMsg;
  sMsg = _T("Warning:  Using this command will clear ALL committed flags from\n");
	sMsg+= _T("this drawing.  If it is your intension to remove only some of the\n");
	sMsg+= _T("committed flags then please use the 'Uncommit' command in the\n");
	sMsg+= _T("BOM Summary dialog box.\n\nProceed and remove all committed flags?");
	MessageBeep(MB_ICONWARNING);
	if( MessageBox( NULL, sMsg, _T("Add Level Error"), MB_OKCANCEL|MB_ICONWARNING )!=IDOK )
		return;

	///////////////////////////////////////////////////////////////////
	//Runs
	for( iRID=0; iRID<GetNumberOfRuns(); iRID++ )
	{
		pR = GetRun(iRID);
		for( iBID=0; iBID<pR->GetNumberOfBays(); iBID++ )
		{
			pB = pR->GetBay(iBID);
			for( iCID=0; iCID<pB->GetNumberOfBayComponents(); iCID++ )
			{
				pC = pB->GetBayComponent( iCID );
				pC->SetCommitted( false );
				pC->SetStage( STAGE_DEFAULT_VALUE );
				pC->SetLevel( LEVEL_DEFAULT_VALUE );
			}

			for( iLID=0; iLID<pB->GetNumberOfLifts(); iLID++ )
			{
				pL = pB->GetLift( iLID );
				for( iCID=0; iCID<pL->GetNumberOfComponents(); iCID++ )
				{
					pC = pL->GetComponent( iCID );
					pC->SetCommitted( false );
					pC->SetStage( STAGE_DEFAULT_VALUE );
					pC->SetLevel( LEVEL_DEFAULT_VALUE );
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	//Lapboards
	for( iBID=0; iBID<GetNumberOfLapboards(); iBID++ )
	{
		pB = GetLapboard(iBID);
		for( iCID=0; iCID<pB->GetNumberOfBayComponents(); iCID++ )
		{
			pC = pB->GetBayComponent( iCID );
			pC->SetCommitted( false );
			pC->SetStage( STAGE_DEFAULT_VALUE );
			pC->SetLevel( LEVEL_DEFAULT_VALUE );
		}

		for( iLID=0; iLID<pB->GetNumberOfLifts(); iLID++ )
		{
			pL = pB->GetLift( iLID );
			for( iCID=0; iCID<pL->GetNumberOfComponents(); iCID++ )
			{
				pC = pL->GetComponent( iCID );
				pC->SetCommitted( false );
				pC->SetStage( STAGE_DEFAULT_VALUE );
				pC->SetLevel( LEVEL_DEFAULT_VALUE );
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	//BOM Extra
	for( iCID=0; iCID<m_BOMExtraCompList.GetNumberOfComponents(); iCID++ )
	{
		pC = m_BOMExtraCompList.GetComponent( iCID );
		pC->SetCommitted( false );
	}

	GetBayStageLevelArray()->ClearCommitted();

}	//ClearCommitted()

/////////////////////////////////////////////////////////////////////////////////////////
//GetPoint(LPCSTR strPrompt, Point3D &point)
//
int Controller::GetPoint(LPCTSTR strPrompt, Point3D &point)
{
	int			iRetVal;
	ads_point	pt;

	iRetVal = acedGetPoint( NULL, strPrompt, pt );
	switch( iRetVal )
	{
	default:
		//This should not have occured
		MessageBeep(MB_ICONEXCLAMATION);
		break;

	////////////////////////////////////////////////////////////
	case( RTKWORD ):			//(-5005) Keyword
		break;

	////////////////////////////////////////////////////////////
	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		break;

	////////////////////////////////////////////////////////////
	case( RTNONE ):	
		break;

	////////////////////////////////////////////////////////////
	case( RTNORM ):	
		//everthing is fine
		point = asPnt3d(pt);
		break;
	}

	return iRetVal;
}	//GetPoint(LPCSTR strPrompt, Point3D &point)

/////////////////////////////////////////////////////////////////////////////////////////
//GetPoint3D()
//
int Controller::GetPoint3D( CString sPrompt, Point3D &ptPoint, bool bTurnSnapsOffTemporarily/*=true*/ )
{
	int			iRetVal;
	ads_point	pt;

	int				iRetSnap;
	resbuf			result, off;

	if( bTurnSnapsOffTemporarily )
	{
		//store the current object snap mode
		iRetSnap = acedGetVar( _T("osmode"), &result );
		if( iRetSnap==RTNORM )
		{
			//switch off the object snap mode
			off.restype = RTSHORT;
			off.resval.rint = 20517;
			acedSetVar( _T("osmode"), &off );
		}
	}

	iRetVal = acedGetPoint( NULL, sPrompt, pt );

	//restore the object snap mode
	if( bTurnSnapsOffTemporarily && iRetSnap==RTNORM )
	{
		acedSetVar( _T("osmode"), &result );
	}

	ptPoint = asPnt3d(pt);

	return iRetVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
//IsMillsSystemCorner()
//
bool Controller::IsMillsSystemCorner()
{
	if( GetSystem()!=S_MILLS )
		return false;	//We must be in MillsSystem!

	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( MILLS_SYSTEM_CORNER_FLAG, MILLS_SYSTEM_CORNER_FLAG_DEFAULT );
	return (iRet==0)? false: true;
}	//IsMillsSystemCorner()

/////////////////////////////////////////////////////////////////////////////////////////
//SetMillsSystemCorner(bool bMillsSystemCorner)
//
void Controller::SetMillsSystemCorner(bool bMillsSystemCorner)
{
	int iVal;

	//Store in the registry
	iVal = bMillsSystemCorner? 1: 0;
	SetIntInRegistry( MILLS_SYSTEM_CORNER_FLAG, iVal );
}	//SetMillsSystemCorner(bool bMillsSystemCorner)

/////////////////////////////////////////////////////////////////////////////////////////
//MillsSystemCorner()
//
void Controller::MillsSystemCorner()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nEnter new value for MillsSystem Corner <%i>:"), IsMillsSystemCorner()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = IsMillsSystemCorner()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		MillsSystemCorner();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetMillsSystemCorner( (i==0)? false: true );
		break;
	}
}	//MillsSystemCorner()

/////////////////////////////////////////////////////////////////////////////////////////
//IsSydneyCorner()
//
bool Controller::IsSydneyCorner()
{
	if( GetSystem()!=S_KWIKSTAGE )
		return false;	//we must be in kwikstage, since sydney only use kwikstage

	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( SYDNEY_CORNER_FLAG, SYDNEY_CORNER_FLAG_DEFAULT );
	return (iRet==0)? false: true;
}	//IsSydneyCorner()

/////////////////////////////////////////////////////////////////////////////////////////
//SetSydneyCorner(bool bSydneyCorner)
//
void Controller::SetSydneyCorner(bool bSydneyCorner)
{
	int iVal;

	//Store in the registry
	iVal = bSydneyCorner? 1: 0;
	SetIntInRegistry( SYDNEY_CORNER_FLAG, iVal );
}	//SetSydneyCorner(bool bSydneyCorner)

/////////////////////////////////////////////////////////////////////////////////////////
//ToggleSydneyCorner()
//
void Controller::ToggleSydneyCorner()
{
	SetSydneyCorner( !IsSydneyCorner() );
}	//ToggleSydneyCorner()

/////////////////////////////////////////////////////////////////////////////////////////
//GetSydneyCornerDistBwBays()
//
double Controller::GetSydneyCornerDistBwBays()
{
	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( SYDNEY_CORNER_SEPARATION, SYDNEY_CORNER_SEPARATION_DEFAULT );
	return (double)iRet;
}	//GetSydneyCornerDistBwBays()

/////////////////////////////////////////////////////////////////////////////////////////
//SetSydneyCornerDistBwBays(double dSeparation)
//
void Controller::SetSydneyCornerDistBwBays(double dSeparation)
{
	//Store in the registry
	SetIntInRegistry( SYDNEY_CORNER_SEPARATION, (int)dSeparation );
}	//SetSydneyCornerDistBwBays(double dSeparation)

/////////////////////////////////////////////////////////////////////////////////////////
//SydneyCorner()
//
void Controller::SydneyCorner()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nEnter new value for Sydney Corner <%i>:"), IsSydneyCorner()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = IsSydneyCorner()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		SydneyCorner();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetSydneyCorner( (i==0)? false: true );
		break;
	}
}	//SydneyCorner()

/////////////////////////////////////////////////////////////////////////////////////////
//SydneyCornerSeparation()
//
void Controller::SydneyCornerSeparation()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nEnter new separation distance for Sydney Corner <%1.1fmm>:"), GetSydneyCornerDistBwBays() );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
//	acedInitGet( RSG_OTHER, "");
	ads_real result;

	int iRet = acedGetReal( prompt, &result);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		SetSydneyCornerDistBwBays( result );
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		if( iRet>=0 )
		{
			SetSydneyCornerDistBwBays( result );
		}
		else
		{
			MessageBeep( MB_ICONERROR );
			SydneyCornerSeparation();
		}
		break;
	}
}		//SydneyCornerSeparation()

/////////////////////////////////////////////////////////////////////////////////////////
//GetUseActualComponents()
//
UseActualComponentsEnum Controller::GetUseActualComponents()
{
	return m_uaceUseActualComponents;
}	//GetUseActualComponents()

/////////////////////////////////////////////////////////////////////////////////////////
//SetUseActualComponents(  UseActualComponentsEnum eUse )
//
void Controller::SetUseActualComponents(  UseActualComponentsEnum eUse )
{
	m_uaceUseActualComponents = eUse;
}	//SetUseActualComponents(  UseActualComponentsEnum eUse )

/////////////////////////////////////////////////////////////////////////////////////////
//UseActualComponents()
//
void Controller::UseActualComponents()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nEnter new value for actual components <%i>:"), (int)GetUseActualComponents() );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	int iResult;
	int iRet = acedGetInt( prompt, &iResult);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		UseActualComponents();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		if( iResult==(int)UAC_NEITHER	|| iResult==(int)UAC_MATRIX	||
			iResult==(int)UAC_3D_VIEW	|| iResult==(int)UAC_BOTH )
		{
			SetUseActualComponents( (UseActualComponentsEnum)iResult );
		}
		else
		{
			UseActualComponents();
		}
		break;
	}
}	//UseActualComponents()

/////////////////////////////////////////////////////////////////////////////////////////
//ApplyToSelection(Bay *pBay, BayList *pBays )
//
void Controller::ApplyToSelection(Bay *pBay, BayList *pBays )
{
	int		i, j, iSize, iWidthLeft[4], iWidthRight[4], iSide;
	Bay		*pSelectedBay, *pForward, *pBackward, *pOuter, *pInner;
	bool	bChangeLength, bChangeWidth, bResizeRequired, bUniqueCornerFillers[4], bStages[4];
	Lift	*pLift;
	double	dIncr, dProg, dWidth, dLength, dPreviousL,
			dPreviousW, dRLs[4];

	iSize = pBays->GetSize();

	if( iSize>0 )
	{
		dIncr = 100.00/(double)iSize;
		dIncr/= 5.00;
		dProg = 0.00;
	}

	bChangeLength	= false;
	bChangeWidth	= false;
	acedSetStatusBarProgressMeter( _T("Applying to Selection"), 0, 100 );

	int			iLiftID, iCnrSize;
	bool		bRecreateMatrixReqd = false;
	Component	*pComponent;
	//BayTieTubeTemplates	TTT;

	for( i=0; i<iSize; i++ )
	{
		pSelectedBay = pBays->GetAt(i);
		assert( pSelectedBay!=NULL );

		pSelectedBay->SetAllowDraw( false );

		///////////////////////////////////////////////////////////////////////////////
		//Where are it's corner individual fillers
		//	(ie corner fillers with only one hopup? )
		for( j=(int)CNR_NORTH_EAST; j<=(int)CNR_SOUTH_WEST; j++ )
		{
			bUniqueCornerFillers[j] = false;
			iWidthLeft[j] = -1;
			iWidthRight[j] = -1;
		}
		bStages[N]	= pSelectedBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, N );
		bStages[E]	= pSelectedBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, E );
		bStages[S]	= pSelectedBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, S );
		bStages[W]	= pSelectedBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, W );
		if( pSelectedBay->HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, NE ) )
		{
			bUniqueCornerFillers[CNR_NORTH_EAST] = ( ( bStages[N] && !bStages[E] ) ||
													 ( !bStages[N] && bStages[E] ) );
			for( iLiftID=0; iLiftID<pSelectedBay->GetNumberOfLifts(); iLiftID++ )
			{
				pLift = pSelectedBay->GetLift( iLiftID );
				pComponent = pLift->GetComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, NE );
				if( pComponent!=NULL )
				{
					iCnrSize = (int)pComponent->GetLengthCommon();
					GetDimensionsForCornerFiller( iCnrSize, iWidthLeft[CNR_NORTH_EAST], iWidthRight[CNR_NORTH_EAST] );
					break;
				}
			}
		}
		if( pSelectedBay->HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, SE ) )
		{
			bUniqueCornerFillers[CNR_SOUTH_EAST] = ( ( bStages[S] && !bStages[E] ) ||
													 ( !bStages[S] && bStages[E] ) );
			for( iLiftID=0; iLiftID<pSelectedBay->GetNumberOfLifts(); iLiftID++ )
			{
				pLift = pSelectedBay->GetLift( iLiftID );
				pComponent = pLift->GetComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, SE );
				if( pComponent!=NULL )
				{
					iCnrSize = (int)pComponent->GetLengthCommon();
					GetDimensionsForCornerFiller( iCnrSize, iWidthLeft[CNR_SOUTH_EAST], iWidthRight[CNR_SOUTH_EAST] );
					break;
				}
			}
		}
		if( pSelectedBay->HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, SW ) )
		{
			bUniqueCornerFillers[CNR_SOUTH_WEST] = ( ( bStages[S] && !bStages[W] ) ||
													 ( !bStages[S] && bStages[W] ) );
			for( iLiftID=0; iLiftID<pSelectedBay->GetNumberOfLifts(); iLiftID++ )
			{
				pLift = pSelectedBay->GetLift( iLiftID );
				pComponent = pLift->GetComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, SW );
				if( pComponent!=NULL )
				{
					iCnrSize = (int)pComponent->GetLengthCommon();
					GetDimensionsForCornerFiller( iCnrSize, iWidthLeft[CNR_SOUTH_WEST], iWidthRight[CNR_SOUTH_WEST] );
					break;
				}
			}
		}
		if( pSelectedBay->HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, NW ) )
		{
			bUniqueCornerFillers[CNR_NORTH_WEST] = ( ( bStages[N] && !bStages[W] ) ||
													 ( !bStages[N] && bStages[W] ) );
			for( iLiftID=0; iLiftID<pSelectedBay->GetNumberOfLifts(); iLiftID++ )
			{
				pLift = pSelectedBay->GetLift( iLiftID );
				pComponent = pLift->GetComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, NW );
				if( pComponent!=NULL )
				{
					iCnrSize = (int)pComponent->GetLengthCommon();
					GetDimensionsForCornerFiller( iCnrSize, iWidthLeft[CNR_NORTH_WEST], iWidthRight[CNR_NORTH_WEST] );
					break;
				}
			}
		}
		

		///////////////////////////////////////////////////////////////////////////////
		//Where are it's Tie Tubes?
		//TTT = *pSelectedBay->GetTemplate()->GetTieTubeTemplates();
		
		dRLs[CNR_NORTH_EAST] = pSelectedBay->GetBottomRLofStandard( CNR_NORTH_EAST );
		dRLs[CNR_SOUTH_EAST] = pSelectedBay->GetBottomRLofStandard( CNR_SOUTH_EAST );
		dRLs[CNR_SOUTH_WEST] = pSelectedBay->GetBottomRLofStandard( CNR_SOUTH_WEST );
		dRLs[CNR_NORTH_WEST] = pSelectedBay->GetBottomRLofStandard( CNR_NORTH_WEST );

		/////////////////////////////////////////////////////////////
		//Do we need to resize the bay?
		dLength	= pSelectedBay->GetBayLength();
		dWidth	= pSelectedBay->GetBayWidth();
		dPreviousL	= dLength;
		dPreviousW	= dWidth;
		bChangeLength	= (dLength !=pBay->GetBayLength() );
		bChangeWidth	= (dWidth  !=pBay->GetBayWidth() );
		bResizeRequired = false;
		if( bChangeLength || bChangeWidth )
		{
			//The selected bay is a different size than the edited bay
			//	so when we do the copy the size will have changed
			//	if the bay dimensions were altered then we need
			//	to use the new dimensions, otherwise, if they didn't
			//	deliberately edit the Bay size then we do need set
			//	the bay back to original dimensions
			bResizeRequired = true;

			if( pBay->WasBayWidthChangedDuringEdit() )
			{
				dWidth	= pBay->GetBayWidth();
			}
			if( pBay->WasBayLengthChangedDuringEdit() )
			{
				//The dimensions were changed, so use the new size!
				dLength	= pBay->GetBayLength();
			}
		}

		/////////////////////////////////////////////////////////////
		//Does this Bay have Hopups
		int iNumberOfBoardsEast, iNumberOfBoardsWest;
		if( pSelectedBay->GetNumberOfLifts()>0 )
		{
			pLift = pSelectedBay->GetLift( pSelectedBay->GetNumberOfLifts()-1 );
			assert( pLift!=NULL );
			iNumberOfBoardsEast = pLift->GetNumberOfPlanksOnStageBoard( EAST );
			iNumberOfBoardsWest = pLift->GetNumberOfPlanksOnStageBoard( WEST );
		}

		/////////////////////////////////////////////////////////////
		//Store the bay pointers
		pForward	= pSelectedBay->GetForward();
		pBackward	= pSelectedBay->GetBackward();
		pOuter		= pSelectedBay->GetOuter();
		pInner		= pSelectedBay->GetInner();

		dProg+= dIncr;
		acedSetStatusBarProgressMeterPos( (int)dProg );

		///////////////////////////////////////////////////
		//Store the existing Lapboard pointers
		int				j=0;
		intArray		iaLapboardCrossesSide;
		LapboardBay		*pLapBay;
		LapboardList	Lapboards;
		SideOfBayEnum	eSide;
		while( pSelectedBay->GetNumberOfLapboardPointers()>0 )
		{
			pLapBay = pSelectedBay->GetLapboard(0);
			Lapboards.AddNewLapboard( pLapBay );

			eSide = pSelectedBay->LapboardCrossesSideGet(0);
			iaLapboardCrossesSide.Add( (int)eSide );

			//The copy routine is going to destroy this lapboard unless
			//	we remove it from this list!
			pSelectedBay->RemoveLapboardPointer(0);
		}

		/////////////////////////////////////////////////
		//Copy the edited bay to the selected bay
		pSelectedBay->CopyAllComponents( *pBay, pBays );
		dProg+= 2.00*dIncr;
		acedSetStatusBarProgressMeterPos( (int)dProg );

		//reapply the pointers so that any descrepancies are cleaned up
		if( iSize>1 )
		{
			pSelectedBay->SetForward( NULL );
			pSelectedBay->SetBackward( NULL );
			pSelectedBay->SetOuter( NULL );
			pSelectedBay->SetInner( NULL );
			pSelectedBay->SetForward( pForward, false, (pForward==NULL) );
			pSelectedBay->SetBackward( pBackward, false, (pBackward==NULL) );
			pSelectedBay->SetOuter( pOuter, false, (pOuter==NULL) );
			pSelectedBay->SetInner( pInner, false, (pInner==NULL) );
		}
		else
		{
			pSelectedBay->SetForwardDumb( pForward );
			pSelectedBay->SetBackwardDumb( pBackward );
			pSelectedBay->SetOuterDumb( pOuter );
			pSelectedBay->SetInnerDumb( pInner );
		}

		dProg+= dIncr;
		acedSetStatusBarProgressMeterPos( (int)dProg );

		//You must resize after the update schatic, otherwise there
		//	is not group to act upon
		if( bResizeRequired )
		{
			if( bChangeLength )
			{
				if( pBay->WasBayLengthChangedDuringEdit() )
					pSelectedBay->SetBayLength(dPreviousL, false );
				SetLengthForBay( pSelectedBay, dLength, pBay->WasBayLengthChangedDuringEdit() );
			}
			if( bChangeWidth )
			{
				if( pBay->WasBayWidthChangedDuringEdit() )
					pSelectedBay->SetBayWidth(dPreviousW, false);
				SetWidthForBay( pSelectedBay, dWidth, pBay->WasBayWidthChangedDuringEdit() );
			}
		}

		//if there is an attached stair of ladder bay then recreate that bay
		if( pForward!=NULL )	RecreateStairOrLadder( pForward, pSelectedBay, EAST );
		if( pBackward!=NULL )	RecreateStairOrLadder( pBackward, pSelectedBay, WEST );
		if( pOuter!=NULL )		RecreateStairOrLadder( pOuter, pSelectedBay, NORTH );
		if( pInner!=NULL )		RecreateStairOrLadder( pInner, pSelectedBay, SOUTH );

		if( iSize>1 )
		{
			//They have not specifically selected this bay only (ie this is
			//	a multiselection), and there are hopups on the east side, so
			//	we need to recreate the hopups
			pSelectedBay->AddHopupToBayIfReqd( EAST, iNumberOfBoardsEast );
			pSelectedBay->AddHopupToBayIfReqd( WEST, iNumberOfBoardsWest );
		}

		//////////////////////////////////////////////////////////////////////////
		//restore the lapboard pointers
		for( j=0; j<Lapboards.GetNumberOfLapboards(); j++ )
		{
			pLapBay = Lapboards.GetLapboard(j);
			eSide = (SideOfBayEnum)iaLapboardCrossesSide.GetAt(j);
			pSelectedBay->AddLapboardPointer( pLapBay, eSide );
		}
		Lapboards.RemoveAll();
		iaLapboardCrossesSide.RemoveAll();
		pSelectedBay->RecreateAllLapboards(pBays);

		dProg+= dIncr;
//		pSelectedBay->DeleteAccessObsticles();

		pSelectedBay->RecreateAllTieTubesFromTemplate();
		if( pSelectedBay->GetNumberOfLapboardPointers()>0 )
		{
			//We have a lapboards but not tie tubes, we should
			//	see if the bays on the other side of the lap has
			//	tie tubes, and then recreate those (again if
			//	need be)
			for( j=0; j<pSelectedBay->GetNumberOfLapboardPointers(); j++ )
			{
				pLapBay = pSelectedBay->GetLapboard(j);
				if( pLapBay->GetBayWest()!=NULL && pLapBay->GetBayWest()!=pSelectedBay )
				{
					pLapBay->GetBayWest()->RecreateAllTieTubesFromTemplate();
				}
				if( pLapBay->GetBayEast()!=NULL && pLapBay->GetBayEast()!=pSelectedBay )
				{
					pLapBay->GetBayEast()->RecreateAllTieTubesFromTemplate();
				}
			}
		}

		////////////////////////////////////////////////////
		//ensure that transoms support the decks
		pSelectedBay->EnsurePlanksAreSupported(EAST);
		pSelectedBay->EnsurePlanksAreSupported(WEST);
		if( pSelectedBay->GetForward()!=NULL && !pBays->IsInList(pSelectedBay->GetForward()) )
			pSelectedBay->GetForward()->EnsurePlanksAreSupported(WEST);
		if( pSelectedBay->GetBackward()!=NULL )
			pSelectedBay->GetBackward()->EnsurePlanksAreSupported(EAST);

		if( iSize>1 )
		{
			//They have not specifically selected this bay only (ie this is
			//	a multiselection), so we have to check the handrails are
			//	correct!  If they had only selected one bay then we would
			//	just accept the handrails exactly as they edited them!
			pSelectedBay->EnsureHandrailsAreCorrect(EAST);
			pSelectedBay->EnsureHandrailsAreCorrect(WEST);
			if( pSelectedBay->GetForward()!=NULL && !pBays->IsInList(pSelectedBay->GetForward()) )
				pSelectedBay->GetForward()->EnsureHandrailsAreCorrect(WEST);
			if( pSelectedBay->GetBackward()!=NULL )
				pSelectedBay->GetBackward()->EnsureHandrailsAreCorrect(EAST);
		}

		if( iSize>1 )
		{
			pSelectedBay->SetAndAdjustRLs( dRLs );
		}

/*		//Clear any matrix labels
		pSelectedBay->MatrixRemoveLink(false);
		pSelectedBay->GetTemplate()->ClearMatrixForwardText();
		if( pSelectedBay->GetBackward()==NULL )
		{
			pSelectedBay->MatrixRemoveLink(true);
		}
		else
		{
			pSelectedBay->GetBackward()->MatrixRemoveLink(false);
			pSelectedBay->GetBackward()->UpdateSchematicView();
		}
*/
		if( GetMatrix()!=NULL )
		{
			int iMatrixID;
			if( pSelectedBay->GetBackward()==NULL )
			{
				iMatrixID = pSelectedBay->FindMatrix( true );
				if( iMatrixID>=0 )
					pSelectedBay->SetMatrixElementPointer( GetMatrix()->GetMatrixElement(iMatrixID), true );
				else
					bRecreateMatrixReqd = true;
			}

			iMatrixID = pSelectedBay->FindMatrix( false );
			if( iMatrixID>=0 )
				pSelectedBay->SetMatrixElementPointer( GetMatrix()->GetMatrixElement(iMatrixID), false );
			else
				bRecreateMatrixReqd = true;

			if( pSelectedBay->GetBackward()!=NULL )
			{
				iMatrixID = pSelectedBay->GetBackward()->FindMatrix( false );
				if( iMatrixID>=0 )
					pSelectedBay->GetBackward()->SetMatrixElementPointer( GetMatrix()->GetMatrixElement(iMatrixID), false );
				else
					bRecreateMatrixReqd = true;

				if( !bRecreateMatrixReqd )
					pSelectedBay->GetBackward()->UpdateSchematicView();
			}
		}

		for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
		{
			pSelectedBay->EnsureHandrailsAreCorrect( (SideOfBayEnum)iSide );
		}

		///////////////////////////////////////////////////////////////////////////////
		//replace any individual corner fillers
		if( bUniqueCornerFillers[CNR_NORTH_EAST] )
		{
			assert( iWidthLeft[CNR_NORTH_EAST]>0 );
			assert( iWidthRight[CNR_NORTH_EAST]>0 );
			AddIndividualCornerFiller( pSelectedBay, NE, iWidthLeft[CNR_NORTH_EAST], iWidthRight[CNR_NORTH_EAST] );
		}
		if( bUniqueCornerFillers[CNR_SOUTH_EAST] )
		{
			assert( iWidthLeft[CNR_SOUTH_EAST]>0 );
			assert( iWidthRight[CNR_SOUTH_EAST]>0 );
			AddIndividualCornerFiller( pSelectedBay, SE, iWidthLeft[CNR_SOUTH_EAST], iWidthRight[CNR_SOUTH_EAST] );
		}
		if( bUniqueCornerFillers[CNR_SOUTH_WEST] )
		{
			assert( iWidthLeft[CNR_SOUTH_WEST]>0 );
			assert( iWidthRight[CNR_SOUTH_WEST]>0 );
			AddIndividualCornerFiller( pSelectedBay, SW, iWidthLeft[CNR_SOUTH_WEST], iWidthRight[CNR_SOUTH_WEST] );
		}
		if( bUniqueCornerFillers[CNR_NORTH_WEST] )
		{
			assert( iWidthLeft[CNR_NORTH_WEST]>0 );
			assert( iWidthRight[CNR_NORTH_WEST]>0 );
			AddIndividualCornerFiller( pSelectedBay, NW, iWidthLeft[CNR_NORTH_WEST], iWidthRight[CNR_NORTH_WEST] );
		}

		pSelectedBay->SetAllowDraw( true );

		//delete obsticles may have deleted some obsticles
		CleanUp(false);

		pSelectedBay->UpdateSchematicView();

		acedSetStatusBarProgressMeterPos( (int)dProg );
	}

	if( bRecreateMatrixReqd )
	{
		CString	sMsg;
		sMsg = _T("Your changes require a regeneration of the matrix!\n\n");
		sMsg+= _T("Would you like to recreate the matrix now?");
		MessageBeep( MB_ICONQUESTION );
		if( MessageBox( NULL, sMsg, _T("Selection Error"), MB_ICONQUESTION|MB_YESNO )==IDYES )
			CreateMatrix(true);
	}

	acedRestoreStatusBar();
}	//ApplyToSelection(Bay *pBay, BayList *pBays )

/////////////////////////////////////////////////////////////////////////////////////////
//CheckForStairLadder(BayList *pBays)
//
void Controller::CheckForStairLadder(BayList *pBays, bool bShowWarning/*=true*/, bool bIgnorButtress/*=false*/ )
{
	int		iNumberOfLapBays, iNumberOfStairBays,
			iNumberOfLadderBays, iNumberOfButtressBays;
	CString	sTemp, sTemp2;

	iNumberOfLapBays		= 0;
	iNumberOfStairBays		= 0;
	iNumberOfLadderBays		= 0;
	iNumberOfButtressBays	= 0;

	/////////////////////////////////////////////////////////////////////
	//find out what type of selection we have!
	//	NOTE: The size may change here
	int		i, iSize;
	iSize = pBays->GetSize();
	for( i=0; i<pBays->GetSize(); i++ )
	{
		switch( pBays->GetAt(i)->GetBayType() )
		{
		case( BAY_TYPE_BAY ):
			//fine!
			break;
		case( BAY_TYPE_LAPBOARD ):
			iNumberOfLapBays++;
			sTemp = _T("Lapboard");
			sTemp2 = _T("object");
			pBays->RemoveAt(i);
			i--;
			break;
		case( BAY_TYPE_STAIRS ):
			iNumberOfStairBays++;
			sTemp = _T("Stair bay");
			sTemp2 = _T("bay");
			pBays->RemoveAt(i);
			i--;
			break;
		case( BAY_TYPE_LADDER ):
			iNumberOfLadderBays++;
			sTemp = _T("Ladder bay");
			sTemp2 = _T("bay");
			pBays->RemoveAt(i);
			i--;
			break;
		case( BAY_TYPE_BUTTRESS ):
			if( !bIgnorButtress )
			{
				iNumberOfButtressBays++;
				sTemp = _T("Buttress bay");
				sTemp2 = _T("bay");
				pBays->RemoveAt(i);
				i--;
			}
			break;
		}
	}

	/////////////////////////////////////////////////////////////////////
	//Show warnings if reqired
	//	NOTE: Don't recalc iSize yet, we still need the original value!
	if( bShowWarning && (iNumberOfLapBays>0		|| iNumberOfStairBays>0		||
						 iNumberOfLadderBays>0	|| iNumberOfButtressBays>0	) )
	{
		CString sMsg;
		if( iSize==1 )
		{
      sMsg. Format( _T("STOP: Your selection contains only 1 %s!\n"), sTemp );
			sTemp.Format( _T("There are no editable components in this type of %s.\n\n"), sTemp2 );
			sMsg+= sTemp;
			sTemp.Format( _T("If you are trying to edit this object then I suggest you delete\n") );
			sMsg+= sTemp;
			sTemp.Format( _T("it and recreate it to make modifications!\n\n"), sTemp2 );
			sMsg+= sTemp;
			sMsg+= _T("I will cancel this edit operation now!");
			MessageBeep( MB_ICONEXCLAMATION );
			MessageBox( NULL, sMsg, _T("Selection Error"), MB_ICONSTOP|MB_OK );
			return;
		}

		if( iNumberOfLapBays+iNumberOfStairBays+iNumberOfLadderBays+iNumberOfButtressBays==iSize )
		{
      sMsg. Format( _T("STOP: Your selection is made up of:\n") );
			if( iNumberOfLapBays>0 )
			{
				sTemp.Format( _T("%i Lapboards\n"), iNumberOfLapBays );
				sMsg+= sTemp;
			}

			if( iNumberOfStairBays>0 )
			{
				sTemp.Format( _T("%i Stair Bays\n"), iNumberOfStairBays );
				sMsg+= sTemp;
			}

			if( iNumberOfLadderBays>0 )
			{
				sTemp.Format( _T("%i Ladder Bays\n"), iNumberOfLadderBays );
				sMsg+= sTemp;
			}

			if( iNumberOfButtressBays>0 )
			{
				sTemp.Format( _T("%i Buttress Bays\n"), iNumberOfButtressBays );
				sMsg+= sTemp;
			}
			
			if( iNumberOfLapBays<=0 )
			{
				sTemp.Format( _T("\nThere are no editable components in these bays\n\n") );
				sMsg+= sTemp;
				sTemp.Format( _T("If you are trying to adjust these bays then I suggest you delete\n") );
			}
			else if( iNumberOfStairBays<=0 && iNumberOfLadderBays<=0 && iNumberOfButtressBays<=0)
			{
				sTemp.Format( _T("\nThere are no editable components in these lapboards\n\n") );
				sMsg+= sTemp;
				sTemp.Format( _T("If you are trying to edit these lapboard then I suggest you delete\n") );
			}
			else
			{
				sTemp.Format( _T("\nThere are no editable components in these items\n\n") );
				sMsg+= sTemp;
				sTemp.Format( _T("If you are trying to edit them then I suggest you delete\n") );
			}
			sMsg+= sTemp;

			sMsg+=  _T("them and then recreate them!\n\n");
			sMsg+= _T("I will cancel this edit operation now!");
			MessageBeep( MB_ICONEXCLAMATION );
			MessageBox( NULL, sMsg, _T("Selection Error"), MB_ICONSTOP|MB_OK );
			return;
		}

    sMsg. Format( _T("NOTE: Your selection includes:\n") );
		if( iNumberOfLapBays>0 )
		{
			sTemp.Format( _T("%i Lapboard%s\n"), iNumberOfLapBays, (iNumberOfLapBays>1)? _T("s"): _T("") );
			sMsg+= sTemp;
		}

		if( iNumberOfStairBays>0 )
		{
			sTemp.Format( _T("%i Stair Bay%s\n"), iNumberOfStairBays, (iNumberOfStairBays>1)? _T("s"): _T("") );
			sMsg+= sTemp;
		}

		if( iNumberOfLadderBays>0 )
		{
			sTemp.Format( _T("%i Ladder Bay%s\n"), iNumberOfLadderBays, (iNumberOfLadderBays>1)? _T("s"): _T("") );
			sMsg+= sTemp;
		}

		if( iNumberOfButtressBays>0 )
		{
			sTemp.Format( _T("%i Buttress Bay%s\n"), iNumberOfButtressBays, (iNumberOfButtressBays>1)? _T("s"): _T("") );
			sMsg+= sTemp;
		}


		if( iNumberOfLapBays<=0 )
		{
			sTemp.Format( _T("\nThere are no editable components in these bays\n\n") );
			sMsg+= sTemp;
			sTemp.Format( _T("During this edit process I will ignor these bays\n") );
		}
		else if( iNumberOfStairBays<=0 && iNumberOfLadderBays<=0 && iNumberOfButtressBays<=0 )
		{
			sTemp.Format( _T("\nThere are no editable components in these lapboards\n\n") );
			sMsg+= sTemp;
			sTemp.Format( _T("During this edit process I will ignor these lapboards\n") );
		}
		else
		{
			sTemp.Format( _T("\nThere are no editable components in these items\n\n") );
			sMsg+= sTemp;
			sTemp.Format( _T("During this edit process I will ignor these items\n") );
		}
		sMsg+= sTemp;

		MessageBeep( MB_ICONEXCLAMATION );
		int iRet = MessageBox( NULL, sMsg, _T("Selection Warning"), MB_ICONWARNING|MB_OKCANCEL );
		if( iRet!=IDOK )
			return;
	}

}	//CheckForStairLadder(BayList *pBays)

/////////////////////////////////////////////////////////////////////////////////////////
//ResizeBay()
//
void Controller::ResizeBay()
{
	Bay		*pBay;
	bool	bChanged, bWidthChanged, bLenghtSet, bHeightSet;
	double	dWidth, dLength, dHeight;
	doubleArray	daWidths, daLengths/*, daHeights*/;
	BayList	Bays;

	acutPrintf( _T("\nSelect Bays to resize!") );

	if( !SelectBays( Bays, true )) return;

	RemoveAndWarnOfCommittedBays(&Bays);

	if( Bays.GetSize()>0 )
	{
		dWidth = Bays[0]->GetBayWidth();
		dLength = Bays[0]->GetBayLength();
/*		dHeight = Bays[0]->GetBayHeight();
*/		dHeight = 0.00;

		bWidthChanged	= false;
		bLenghtSet	= false;
		bHeightSet	= false;
		if( !GetSizeOfBayFromCommandLine( dWidth, dLength, dHeight, bWidthChanged, bLenghtSet, bHeightSet ) )
			return;

		int i;
		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			bChanged = false;
			daLengths.Add(pBay->GetBayLength()); 
			daWidths.Add(pBay->GetBayWidth());
			//daHeights.Add(pBay->GetBayHeight());
		}

		for( i=0; i<Bays.GetSize(); i++ )
		{
			pBay = Bays[i];
			if( pBay->GetBayLength()!= dLength )
				SetLengthForBay( pBay, dLength, true );
			if( pBay->GetBayWidth()!= dWidth )
				SetWidthForBay( pBay, dWidth, true );
/*			if( pBay->GetBayHeight()!= dHeight )
				pBay->SetBayHeight( dHeight, true );
*/
			bChanged = false;
			if( pBay->GetBayLength()!= daLengths.GetAt(i) )
				bChanged = true;
			if( pBay->GetBayWidth()!= daWidths.GetAt(i) )
				bChanged = true;
/*			if( pBay->GetBayHeight()!= daHeights.GetAt(i) )
				bChanged = true;
*/
			if( bChanged )
			{
				pBay->ReapplyBracingToSide(ALL_SIDES);
				pBay->Redraw();
			}
		}
	}
}	//ResizeBay()

/////////////////////////////////////////////////////////////////////////////////////////
//GetSizeOfBayFromCommandLine(double &dWidth, double &dLength, double &dHeight, bool &bWidthSet, bool &bLenghtSet, bool &bHeightSet, DimensionSelectionEnum eDim/*=DS_LENGTH*/ )
//
bool Controller::GetSizeOfBayFromCommandLine(double &dWidth, double &dLength, double &dHeight, bool &bWidthSet, bool &bLenghtSet, bool &bHeightSet, DimensionSelectionEnum eDim/*=DS_LENGTH*/ )
{
	int						i;
	TCHAR					keywrd[255], prompt[255];
	CString					sPrompt, sKeyWrd, sTemp, sSide;
	double					dValue;

	bWidthSet	= false;
	bLenghtSet	= false;
	bHeightSet	= false;

	while( true )
	{
		switch( eDim )
		{
		case( DS_LENGTH ):
			//sPrompt.Format( "\nPlease set the new Length for the bay [Width/Height] <%0.0fmm>", dLength );
			sPrompt.Format( _T("\nPlease set the new Length for the bay [Width] <%0.0fmm>"), dLength );
			break;
		case( DS_WIDTH ):
			//sPrompt.Format( "\nPlease set the new Width for the bay [Length/Height] <%0.0fmm>", dWidth );
			sPrompt.Format( _T("\nPlease set the new Width for the bay [Length] <%0.0fmm>"), dWidth );
			break;
		case( DS_HEIGHT ):
			//sPrompt.Format( "\nPlease set the new Height for the bay [Length/Width] <%0.0fmm>", dHeight );
			break;
		default:
			assert( false );
		}

		//////////////////////////////////////////////////////////////////
		//copy from CString to char*
		for( i=0; i<sPrompt.GetLength(); i++ )
		{
			prompt[i] = sPrompt[i];
		}
		prompt[i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		//get the first point from the user.  Do not specify a base point.
		
		//acedInitGet( RSG_OTHER, "Width Length Height");
		acedInitGet( RSG_OTHER, _T("Width Length"));
		int iRet = acedGetKword( prompt, keywrd);
		switch( iRet )
		{
		case( RTERROR ):			//fallthrough (-5001) Some other error
		default:
			//This should not have occured
			MessageBeep(MB_ICONERROR);
			continue;

		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
			return false;

		case( RTNONE ):				//fallthrough (5000) No result
			return true;
		
		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		case( RTNORM ):				//(5100) Request succeeded
			sKeyWrd = keywrd;
			sKeyWrd.MakeUpper();

			if( _istdigit( keywrd[0] ) )
			{
				dValue = _tstof( keywrd );
				switch( eDim )
				{
				case( DS_LENGTH ):
					if( dValue == COMPONENT_LENGTH_2400 ||
						dValue == COMPONENT_LENGTH_1800 ||
						dValue == COMPONENT_LENGTH_1200 ||
						dValue == COMPONENT_LENGTH_0800 ||
						dValue == COMPONENT_LENGTH_0700 )
					{
						if( dValue == COMPONENT_LENGTH_0800 )
							dValue = COMPONENT_LENGTH_0700;
						dLength		= dValue;
						bLenghtSet	= true;
					}
					break;
				case( DS_WIDTH ):
					if( dValue == COMPONENT_LENGTH_2400 ||
						dValue == COMPONENT_LENGTH_1800 ||
						dValue == COMPONENT_LENGTH_1200 ||
						dValue == COMPONENT_LENGTH_0800 ||
						dValue == COMPONENT_LENGTH_0700 )
					{
						if( dValue == COMPONENT_LENGTH_0800 )
							dValue = COMPONENT_LENGTH_0700;
						dWidth		= dValue;
						bWidthSet	= true;
					}
					break;
				case( DS_HEIGHT ):
					dValue/= STAR_SEPARATION;
					dValue = (double)int(dValue+0.5);
					dValue*= STAR_SEPARATION;
					dHeight		= dValue;
					bHeightSet	= true;
				default:
					assert( false );
				}
			}
			else
			{
				if( sKeyWrd==_T("Length") || sKeyWrd[0]==_T('L'))
					eDim = DS_LENGTH;
				else if( sKeyWrd==_T("Width") || sKeyWrd[0]==_T('W') )
					eDim = DS_WIDTH;
/*				else if( sKeyWrd=="Height" || sKeyWrd[0]=='H' )
					eDim = DS_HEIGHT;
*/				else
					MessageBeep(MB_ICONEXCLAMATION);
			}
		}
	}
	return true;
}	//GetSizeOfBayFromCommandLine(double &dWidth, double &dLength, double &dHeight, bool &bWidthSet, bool &bLenghtSet, bool &bHeightSet, DimensionSelectionEnum eDim/*=DS_LENGTH*/)

void Controller::SetWidthForBay(Bay *pBay, double dNewWidth, bool bAllowMovement )
{
	double dPreviousW;
	dPreviousW = pBay->GetBayWidth();
	pBay->SetBayWidth(dNewWidth, bAllowMovement );

	if( pBay->GetForward()!=NULL )
		pBay->GetForward()->ResizeAllBaysInDirection( dNewWidth, EAST );
	if( pBay->GetBackward()!=NULL )
		pBay->GetBackward()->ResizeAllBaysInDirection( dNewWidth, WEST );

	if( bAllowMovement )
	{
		double dSizeChange;
		dSizeChange = GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_LEDGER, dPreviousW, MT_STEEL );
		dSizeChange-= GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_LEDGER, dNewWidth, MT_STEEL );

		if( dSizeChange<0.00-ROUND_ERROR || dSizeChange>0.00+ROUND_ERROR)
		{
			Vector3D	Vector;
			Matrix3D	Transform, MoveSchematic, UnMoveSchematic;

			///////////////////////////////////////////////////////////////////
			//Set back at origin
			SetHasBeenVisited( false );
			MoveSchematic	= pBay->GetRunPointer()->GetSchematicTransform();
			UnMoveSchematic = MoveSchematic;
			UnMoveSchematic.invert();
			pBay->Visit( UnMoveSchematic, true );

			///////////////////////////////////////////////////////////////////
			//Move the outer runs
			Vector.set( 0.00, -1.00*dSizeChange, 0.00 );
			Transform.setToTranslation( Vector );
			SetHasBeenVisited( false );
			pBay->GetRunPointer()->SetHasBeenVisited( true );
			if( pBay->GetOuter() )
				pBay->GetOuter()->Visit( Transform, true );

			///////////////////////////////////////////////////////////////////
			//Restore Position
			SetHasBeenVisited( false );
			pBay->Visit( MoveSchematic, true );
		}
	}
}

void Controller::SetLengthForBay(Bay *pBay, double dNewLength, bool bAllowMovement)
{
	if( bAllowMovement )
	{
		double dSizeChange, dPreviousL;

		dPreviousL = pBay->GetBayLength();
		dSizeChange = GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_TRANSOM, dPreviousL, MT_STEEL );
		dSizeChange-= GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_TRANSOM, dNewLength, MT_STEEL );
		pBay->SetBayLength(dNewLength, false);

		if( dSizeChange<0.00-ROUND_ERROR || dSizeChange>0.00+ROUND_ERROR)
		{
			bool bBirdcage;
			bBirdcage = true;
			//Should we shuffle all the bays to the right? or Visit them?
			if( pBay->GetForward()!=NULL )
			{
				Bay	*pNeighbor[3];
				int	i, iCount;

				//set the pointers to null
				for( i=0; i<3; i++ )
				{
					pNeighbor[i] = NULL;
				}

				//get pointers to the surrounding bays
				iCount=0;
				if( pBay->GetOuter()!=NULL )
				{
					pNeighbor[iCount] = pBay->GetOuter();
					iCount++;
				}
				if( pBay->GetInner()!=NULL )
				{
					pNeighbor[iCount] = pBay->GetInner();
					iCount++;
				}
				if( pBay->GetBackward()!=NULL )
				{
					pNeighbor[iCount] = pBay->GetBackward();
					iCount++;
				}

				if( iCount==0 )
				{
					//we only have a bay to the east,
					//	so we cannot be in a birdcage here
					bBirdcage = false;
				}
				else
				{
					//Lets find out if we are in a birdcage situation
					SetHasBeenVisited(false);
					//mark this bay as visited
					pBay->SetHasBeenVisited(true);
					//create a identity movement matrix
					Matrix3D Trans;
					Trans.setToIdentity();
					//run each bay throught the movement matrix
					assert( pBay->GetForward()!=NULL );
					pBay->GetForward()->Visit( Trans, false );
					//if any of the bays around us have been visited
					//	then we are in a birdcage situation
					bool bVisited = false;
					for( i=0; i<iCount; i++ )
					{
						assert( pNeighbor[i]!=NULL );
						if( pNeighbor[i]->HasBeenVisited() )
						{
							bVisited = true;
						}
					}
					if( !bVisited )
						bBirdcage = false;
				}
			}

			if( bBirdcage )
			{
				//This is a birdcage situation, so we need to shuffle
				//	each run individually, after resizeing the corresponding bay
				pBay->GetRunPointer()->ShuffleSubsequentBays( pBay->GetID()+1, dSizeChange, true );

				///////////////////////////////////////////////////////////////////
				//Inner Bays
				Bay	*pNeighborBay;
				pNeighborBay = pBay->GetInner();
				while( pNeighborBay!=NULL )
				{
					pNeighborBay->SetBayLength(dNewLength, false);
					pNeighborBay->GetRunPointer()->ShuffleSubsequentBays( pNeighborBay->GetID()+1, dSizeChange, true );
					pNeighborBay->Redraw();
					pNeighborBay = pNeighborBay->GetInner();
				}

				///////////////////////////////////////////////////////////////////
				//Outer Bays
				pNeighborBay = pBay->GetOuter();
				while( pNeighborBay!=NULL )
				{
					pNeighborBay->SetBayLength(dNewLength, false);
					pNeighborBay->GetRunPointer()->ShuffleSubsequentBays( pNeighborBay->GetID()+1, dSizeChange, true );
					pNeighborBay->Redraw();
					pNeighborBay = pNeighborBay->GetOuter();
				}
			}
			else
			{
				//Not a birdcage situation, so just move each bay using the visit
				Vector3D	Vector;
				Matrix3D	Trans, Transform, TransSchm;

				//Unmove all the RUNS
				TransSchm = pBay->GetRunPointer()->GetSchematicTransform();
				Transform = TransSchm;
				Transform.invert();
				//move subsequent bays
				SetHasBeenVisited(false);
				//run each run throught the movement matrix
				pBay->Visit( Transform, true );

				//move the subsequent BAYS
				Vector.set( (-1.00*dSizeChange), 0.00, 0.00 );
				Transform.setToTranslation( Vector );
				//move subsequent bays
				SetHasBeenVisited(false);
				//mark this bay as visited
				pBay->SetHasBeenVisited(true);
				//run each bay throught the movement matrix
				pBay->GetForward()->Visit( Transform, false );

				//Move all the RUNS back
				Transform = TransSchm;
				//move subsequent bays
				SetHasBeenVisited(false);
				//run each run throught the movement matrix
				pBay->Visit( Transform, true );
				//Unmark them
				SetHasBeenVisited(false);

				///////////////////////////////////////////////////////////////////
				//Inner Bays
				Bay	*pNeighborBay;
				pNeighborBay = pBay->GetInner();
				while( pNeighborBay!=NULL )
				{
					pNeighborBay->SetBayLength(dNewLength, true);
					pNeighborBay->Redraw();
					pNeighborBay = pNeighborBay->GetInner();
				}

				///////////////////////////////////////////////////////////////////
				//Outer Bays
				pNeighborBay = pBay->GetOuter();
				while( pNeighborBay!=NULL )
				{
					pNeighborBay->SetBayLength(dNewLength, true);
					pNeighborBay->Redraw();
					pNeighborBay = pNeighborBay->GetOuter();
				}
			}
		}
	}
	else
	{
		pBay->SetBayLength(dNewLength, false );
	}

	//I have removed this since I have moved it into the set bay length
	//pSelectedBay->GetTemplate()->SetBayLength( dNewLength );
}

double Controller::GetAvailableLength(ComponentTypeEnum eType, MaterialTypeEnum eMaterail, double dLength, RoundingTypeEnum eRound, bool bNotFoundReturnNearest/*=false*/)
{
	return GetCompDetails()->GetAvailableLength( eType, eMaterail, dLength, eRound, bNotFoundReturnNearest );
}

#include "actrans.h"
#include "gs.h"
#include "acgs.h"
#include "dbsymtb.h"

void Controller::EditStandards()
{
	m_bShowingEditStandards = true;
	if( CreateMatrix(true) )
	{
		StoreCurrentView();

		actrTransactionManager->queueForGraphicsFlush();
		actrTransactionManager->flushGraphics();
		acedUpdateDisplay();

		EditStandardsDlg EditDlg(this, NULL );

		EditDlg.DoModal();

		m_bShowingEditStandards = false;
		CreateMatrix(true);

		RestoreCurrentView();
	}
}

void Controller::SetViewportPosition(AcGePoint2d ptBottomLeft, AcGePoint2d ptTopRight)
{
	CString sBL, sTR;
	sBL.Format( _T("%i,%i"), (int)ptBottomLeft.x, (int)ptBottomLeft.y );
	sTR.Format( _T("%i,%i"), (int)ptTopRight.x, (int)ptTopRight.y );
    acedCommand(RTSTR, _T("_ZOOM"), RTSTR, _T("_WINDOW"), RTSTR, sBL, RTSTR, sTR, 0);    // zoom to a reasonable spot
}

void Controller::StoreCurrentView()
{
    acedCommand(RTSTR, _T("_VIEW"), RTSTR, _T("_SAVE"), RTSTR, VIEW_EDIT_STANDARD_CURRENT, 0);
}

void Controller::RestoreCurrentView()
{
    acedCommand(RTSTR, _T("_VIEW"), RTSTR, _T("_RESTORE"), RTSTR, VIEW_EDIT_STANDARD_CURRENT, 0);
}

intArray * Controller::GetMatrixElementToDisplay()
{
	return &m_iaInvisibleMatrixElement;
}

bool Controller::ShowMatrixElement(int iElementID)
{
	for( int i=0; i<m_iaInvisibleMatrixElement.GetSize(); i++ )
	{
		if( m_iaInvisibleMatrixElement.GetAt(i)==iElementID )
			return false;
	}
	return true;
}

void Controller::HideMatrixElement(int iElementID)
{
	m_iaInvisibleMatrixElement.Add( iElementID );
}

void Controller::ShowAllMatrixElements()
{
	m_iaInvisibleMatrixElement.RemoveAll();
}

void Controller::SelectMatrixElements(intArray &iaElements)
{
	int					i;
	ads_name			ss;
	MatrixElement		*pElement;
	AcDbObjectIdArray	objectIds;

	//get the user selection
	if( acedSSGet( NULL, NULL, NULL, NULL, ss )!=RTNORM)
		return;

	if( (GetUseActualComponents()&UAC_MATRIX)>0 )
	{
		long length;
		acedSSLength(ss, &length);
		if(length == 0)
		{
			acedSSFree(ss);
			return;
		}
		ads_name entName;
		AcDbObjectId oId;
		AcDbEntity* pEnt;
		Acad::ErrorStatus es;
		long rejected = 0;
		for(long i=0; i<length; i++)
		{
			acedSSName(ss, i, entName);
			acdbGetObjectId(oId, entName);
			es = acdbOpenAcDbEntity((AcDbEntity*&)pEnt, oId, AcDb::kForRead);
			if(es != Acad::eOk)
			{
				acedSSFree(ss);
				break;
			}
			else
			{
				objectIds.append(oId);
				pEnt->close();
			}
		}
	}
	else
	{
		//reduce the list only entities for polymeshes
		selectPolymeshes( objectIds, ss );
	}

	iaElements.SetUniqueElementsOnly(true);
	//for each id
	for( i=0; i<objectIds.length(); i++ )
	{
		//obtain the bay corresponding to the line's id
		pElement = GetMatrixElementPointerForEntityId( objectIds[i] );
		if( pElement!=NULL )
		{
			iaElements.Add( pElement->GetMatrixElementID() );
		}
	}
}

Acad::ErrorStatus  Controller::selectPolymeshes(AcDbObjectIdArray &idArray, ads_name selection)
{
	long length;
	acedSSLength(selection, &length);
	if(length == 0)
	{
		acedSSFree(selection);
		return Acad::eInvalidInput;
	}
	ads_name entName;
	AcDbObjectId oId;
	AcDbEntity* pEnt;
	Acad::ErrorStatus es;
	long rejected = 0;
	for(long i=0; i<length; i++)
	{
		acedSSName(selection, i, entName);
		acdbGetObjectId(oId, entName);
		es = acdbOpenAcDbEntity((AcDbEntity*&)pEnt, oId, AcDb::kForRead);
		if(es != Acad::eOk)
		{
			acedSSFree(selection);
			return es;
		}
		if(pEnt->isKindOf(AcDbPolygonMesh::desc()))
			idArray.append(oId);
		else
			rejected++;

		pEnt->close();
	}

	acedSSFree(selection);
	if(idArray.length() == 0)
		return Acad::eInvalidInput;

	return Acad::eOk;
}

MatrixElement * Controller::GetMatrixElementPointerForEntityId(AcDbObjectId &Id)
{
	AcDbEntity			*pEnt;
	Component			*pComp;
	Acad::ErrorStatus	es, retStat;
	AcDbObjectIdArray	ids;

	es = acdbOpenAcDbEntity( pEnt, Id, AcDb::kForWrite );

	AcDbVoidPtrArray *pReactors;
	void             *pSomething;

	AcDbObjectId       persObjId;
	EntityReactor     *pPersReacObj;

	pReactors = pEnt->reactors();

	if (pReactors != NULL && pReactors->length() > 0)
	{
		for (int i = 0; i < pReactors->length(); i++)
		{
			pSomething = pReactors->at(i);

			// Is it a persistent reactor?
			if (acdbIsPersistentReactor(pSomething))
			{
				persObjId = acdbPersistentReactorObjectId( pSomething);

				if((retStat=acdbOpenAcDbObject((AcDbObject*&)pPersReacObj, persObjId, AcDb::kForWrite))!=Acad::eOk)
				{
          acutPrintf(_T("\nFailure for openAcDbObject: retStat==%d\n"), retStat);
					return NULL;
				}

				pComp = pPersReacObj->GetComponentPointer();
				pPersReacObj->close();
				pEnt->close();

				if( pComp!=NULL )
				{
					if( pComp->GetMatrixElementPointer()!=NULL )
					{
						return pComp->GetMatrixElementPointer();
					}
				}
				return NULL;
			}
		}
	}
	pEnt->close();

	return NULL;
}

void Controller::ExcludeMatrixElements()
{
	intArray iaElements;
	SelectMatrixElements( iaElements );
	for( int i=0; i<iaElements.GetSize(); i++ )
	{
		HideMatrixElement( iaElements.GetAt(i) );
	}
}

bool Controller::EditingStandards()
{
	return m_bShowingEditStandards;
}


int Controller::HowManyElementsShownBefore(int iElement)
{
	int i, iCount;
	iCount = iElement;
	if( !EditingStandards() )
	{
		for( i=0; i<m_iaInvisibleMatrixElement.GetSize() && iCount>0; i++ )
		{
			if( m_iaInvisibleMatrixElement.GetAt(i)<iElement )
				iCount--;
		}
	}
	return iCount;
}

void Controller::RemoveAndWarnOfCommittedBays(BayList *pBays)
{
	int			i;
	Bay			*pBay;
	intArray	iaCommittedBays;

	iaCommittedBays.RemoveAll();
	for( i=0; i<pBays->GetSize(); i++ )
	{
		pBay = pBays->GetAt(i);
		if( pBay->GetCommittedProportion()!=COMMIT_NONE )
		{
			//There are some committed component in this bay!
			iaCommittedBays.Add( pBay->GetBayNumber() );
			pBays->RemoveAt(i);
			i--;
		}
	}

	if( iaCommittedBays.GetSize()>0 )
	{
		CString sMsg, sTitle, sTemp;

		sTitle = _T("Selection Warning");
		if( iaCommittedBays.GetSize()==1 )
		{
		 	sMsg.Format( _T("You cannot edit Bay%i since it contains some components which are\n"), iaCommittedBays.GetAt(0) );
			sMsg+= _T("committed!\n\n");
      sMsg+= _T("NOTE: To edit this bay, firstly uncommit the committed component/s\n");
			sMsg+= _T("via either the BOM Summary dialog or use the command\n");
			sMsg+= _T("'ClearCommitted' to reset the committed flag for the entire drawing.\n");
			sMsg+= _T("You will then have to manually reapply the committed flag once your\n");
			sMsg+= _T("Changes have been completed!");
			if( pBays->GetSize()==1 )
			{
        sTemp.Format( _T("\n\nYou are still able to edit Bay%i at this time:"), pBays->GetAt(0)->GetBayNumber() );
				sMsg+= sTemp;
			}
			else if( pBays->GetSize()>1 )
			{
        sMsg+= _T("\n\nYou are still able to edit the following bays at this time:");
				for( i=0; i<pBays->GetSize(); i++ )
				{
					sTemp.Format( _T("\nBay%i"), pBays->GetAt(i)->GetBayNumber() );
					sMsg+= sTemp;
				}
			}
		}
		else if( pBays->GetSize()<=0 )
		{
			sTitle = _T("Selection Error");
			sMsg+= _T("All of the bays you selected cannot be edited since they contain\n");
			sMsg+= _T("component/s which are committed!\n\n");
      sMsg+= _T("NOTE: To edit this bay, firstly uncommit the committed component/s\n");
			sMsg+= _T("via either the BOM Summary dialog or use the command\n");
			sMsg+= _T("'ClearCommitted' to reset the committed flag for the entire drawing.\n");
			sMsg+= _T("You will then have to manually reapply the committed flag once your\n");
			sMsg+= _T("Changes have been completed!");
		}
		else
		{
			assert( iaCommittedBays.GetSize()>1 );
			assert( pBays->GetSize()>0 );
			sMsg+= _T("The following bays cannot be edited since they contain component/s\n");
      sMsg+= _T("which are committed:");
			for( i=0; i<iaCommittedBays.GetSize(); i++ )
			{
				sTemp.Format( _T("\nBay%i"), iaCommittedBays.GetAt(i) );
				sMsg+= sTemp;
			}
      sMsg+= _T("\n\nNOTE: To edit these bay, firstly uncommit the committed component/s\n");
			sMsg+= _T("via either the BOM Summary dialog or use the command\n");
			sMsg+= _T("'ClearCommitted' to reset the committed flag for the entire drawing.\n");
			sMsg+= _T("You will then have to manually reapply the committed flag once your\n");
			sMsg+= _T("Changes have been completed!");

			if( pBays->GetSize()==1 )
			{
        sTemp.Format( _T("\n\nYou are still able to edit Bay%i at this time:"), pBays->GetAt(0)->GetBayNumber() );
				sMsg+= sTemp;
			}
			else
			{
				assert( pBays->GetSize()>1 );
        sMsg+= _T("\n\nYou are still able to edit the following bays at this time:");
				for( i=0; i<pBays->GetSize(); i++ )
				{
					sTemp.Format( _T("\nBay%i"), pBays->GetAt(i)->GetBayNumber() );
					sMsg+= sTemp;
				}
			}
		}

    sMsg+= _T("\n\nHint:  The white components in the 3D view are committed!");
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox( NULL, sMsg, sTitle, MB_OK );
	}
}

bool Controller::IsDestructingController()
{
	return m_bDestructingController;
}

void Controller::SetDestructingController( bool bDestructing )
{
	m_bDestructingController = bDestructing;
}

void Controller::MoveMatrixLastPoint(Matrix3D Transform)
{
	m_ptMatrixPosition.transformBy( Transform );
}

void Controller::Move3DLastPoint(Matrix3D Transform)
{
	m_pt3DPosition.transformBy( Transform );
}

bool Controller::GetApplyToRunOrBay(bool &bApplyToRuns)
{
	int				i;
	TCHAR			keywrd[255], prompt[255];
	CString			sPrompt, sKeyWrd;

	//////////////////////////////////////////////////////////////////
	//Choose a side
	if( bApplyToRuns )
	{
    sPrompt.Format( _T("\nWould you like to insert Single Bay or Copy Entire Run[Bay] <Run>:") );
		acedInitGet( RSG_OTHER, _T("B"));
	}
	else
	{
    sPrompt.Format( _T("\nWould you like to insert Single Bay or Copy Entire Run[Run] <Bay>:") );
		acedInitGet( RSG_OTHER, _T("R"));
	}

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
	acedInitGet( RSG_OTHER, _T("B "));
	int iRet = acedGetKword( prompt, keywrd);
	switch( iRet )
	{

	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
		else
		{
			if( sKeyWrd==_T("R") )
				bApplyToRuns = true;
			else if( sKeyWrd==_T("B") )
				bApplyToRuns = false;
			else 
				//invalid input
				return false;
			break;
		}
	}
	return true;
}

void Controller::ShowComponentType(ComponentTypeEnum eType)
{
	for( int i=0; i<m_iaComponentsToHide.GetSize(); i++ )
	{
		if( eType==m_iaComponentsToHide.GetAt(i) )
		{
			m_iaComponentsToHide.RemoveAt(i);
			break;
		}
	}
}

void Controller::HideComponentType(ComponentTypeEnum eType)
{
	m_iaComponentsToHide.Add( (int)eType );
}

bool Controller::IsComponentTypeHidden(ComponentTypeEnum eType)
{
	for( int i=0; i<m_iaComponentsToHide.GetSize(); i++ )
	{
		if( eType==m_iaComponentsToHide.GetAt(i) )
			return true;
	}
	return false;
}

bool Controller::IsComponentTypeVisible(ComponentTypeEnum eType)
{
	return !IsComponentTypeHidden(eType);
}

void Controller::ShowAllComponentTypes()
{
	m_iaComponentsToHide.RemoveAll();
}

void Controller::SelectHiddenComponentTypes()
{
	intArray	iaOriginalComponents;
	ShowComponentTypeDialog Dlg;

	iaOriginalComponents = m_iaComponentsToHide;
	Dlg.m_iColourDeckingPlank		= COLOUR_DECKING_PLANK;
	Dlg.m_iColourStageBoard			= COLOUR_STAGE_BOARD;
	Dlg.m_iColourLapboard			= COLOUR_LAPBOARD;
	Dlg.m_iColourLedger				= COLOUR_LEDGER;
	Dlg.m_iColourTransom			= COLOUR_TRANSOM;
	Dlg.m_iColourMeshGuard			= COLOUR_MESH_GUARD;
	Dlg.m_iColourRail				= COLOUR_RAIL;
	Dlg.m_iColourMidRail			= COLOUR_MID_RAIL;
	Dlg.m_iColourToeBoard			= COLOUR_TOE_BOARD;
	Dlg.m_iColourChainLink			= COLOUR_CHAIN_LINK;
	Dlg.m_iColourShadeCloth			= COLOUR_SHADE_CLOTH;
	Dlg.m_iColourTieTubes			= COLOUR_TIE_TUBE;
	Dlg.m_iColourTieClampColumn		= COLOUR_TIE_CLAMP_COLUMN;
	Dlg.m_iColourTieClampMasonary	= COLOUR_TIE_CLAMP_MASONARY;
	Dlg.m_iColourTieClampYoke		= COLOUR_TIE_CLAMP_YOKE;
	Dlg.m_iColourTieClamp90Deg		= COLOUR_TIE_CLAMP_90DEGREE;
	Dlg.m_iColourBracing			= COLOUR_BRACING;
	Dlg.m_iColourStair				= COLOUR_STAIR;
	Dlg.m_iColourLadder				= COLOUR_LADDER;
	Dlg.m_iColourStandards			= COLOUR_STANDARD;
	Dlg.m_iColourJacks				= COLOUR_JACK;
	Dlg.m_iColourSoleBoards			= COLOUR_SOLEBOARD;
	Dlg.m_iColourHopupBrackets		= COLOUR_HOPUP_BRACKET;
	Dlg.m_iColourCornerStageBoard	= COLOUR_CORNER_STAGE_BOARD;
	Dlg.m_iColourText				= COLOUR_TEXT;

	Dlg.m_iColourCrosshair			= COLOUR_CROSSHAIR;
	Dlg.m_iColourCrosshairArc		= COLOUR_CROSSHAIR_ARC;
	Dlg.m_iColourMatrixRLText		= COLOUR_MATRIX_RL_TEXT;
	Dlg.m_iColourMatrixRLMinor		= COLOUR_MATRIX_RL_LINES_MINOR;
	Dlg.m_iColourMatrixRLMajor		= COLOUR_MATRIX_RL_LINES_MAJOR;
	Dlg.m_iColourMatrixRLSpan		= COLOUR_MATRIX_RL_LINES_SPAN;
	Dlg.m_iColourMatrixLevelText	= COLOUR_MATRIX_LEVELS_TEXT;
	Dlg.m_iColourMatrixLevelLines	= COLOUR_MATRIX_LEVELS_LINES;

	Dlg.m_iColourBay0800			= COLOUR_0700;
	Dlg.m_iColourBay1200			= COLOUR_1200;
	Dlg.m_iColourBay1800			= COLOUR_1800;
	Dlg.m_iColourBay2400			= COLOUR_2400;

	Dlg.m_iColourCommitted			= COLOUR_COMMITTED_FULLY;
	Dlg.m_iColourPartialCommitted	= COLOUR_COMMITTED_PARTIALLY;


	Dlg.SetComponentTypeHideArray( &m_iaComponentsToHide );
	if( Dlg.DoModal()==IDOK )
	{
		bool	b3DRedrawnAlready;
		CString sMsg, sTemp1, sTemp2, sTemp3;
		b3DRedrawnAlready = false;

	#ifdef _DEBUG
		ComponentTypeEnum		eType;
    acutPrintf(_T("\n****Hiding the following sections:****"));
		for( int i=0; i<m_iaComponentsToHide.GetSize(); i++ )
		{
			eType= (ComponentTypeEnum)m_iaComponentsToHide.GetAt(i);
			acutPrintf( _T("\n%s"), GetComponentDescStr(eType) );
		}
		acutPrintf(_T("\n****End Of List****\n"));
	#endif //#ifdef _DEBUG

		if( IsCrosshairCreated() && iaOriginalComponents!=m_iaComponentsToHide )
		{
			//We have a 3D shown and the selection has change, better recreate the 3D view
			CString sMsg;
			sMsg = _T("You have changed some of the component type that are visible.\n");
			sMsg+= _T("Your changes will not be visible untill the 3D View has been redrawn\n\n");
			sMsg+= _T("Would you like me to update the 3D View now?");

			if( MessageBox( NULL, sMsg, _T("Apply Colour Change"), MB_YESNO )==IDYES )
			{
				b3DRedrawnAlready = true;
				Delete3DView();
				Create3DView( true );
			}
		}
		///////////////////////////////////////////////////////////
		//3D and Matrix
		if(	COLOUR_DECKING_PLANK		!= Dlg.m_iColourDeckingPlank ||
			COLOUR_STAGE_BOARD			!= Dlg.m_iColourStageBoard ||
			COLOUR_LAPBOARD				!= Dlg.m_iColourLapboard ||
			COLOUR_LEDGER				!= Dlg.m_iColourLedger ||
			COLOUR_TRANSOM				!= Dlg.m_iColourTransom ||
			COLOUR_MESH_GUARD			!= Dlg.m_iColourMeshGuard ||
			COLOUR_RAIL					!= Dlg.m_iColourRail ||
			COLOUR_MID_RAIL				!= Dlg.m_iColourMidRail ||
			COLOUR_TOE_BOARD			!= Dlg.m_iColourToeBoard ||
			COLOUR_CHAIN_LINK			!= Dlg.m_iColourChainLink ||
			COLOUR_SHADE_CLOTH			!= Dlg.m_iColourShadeCloth ||
			COLOUR_TIE_TUBE				!= Dlg.m_iColourTieTubes ||
			COLOUR_TIE_CLAMP_COLUMN		!= Dlg.m_iColourTieClampColumn ||
			COLOUR_TIE_CLAMP_MASONARY	!= Dlg.m_iColourTieClampMasonary ||
			COLOUR_TIE_CLAMP_YOKE		!= Dlg.m_iColourTieClampYoke ||
			COLOUR_TIE_CLAMP_90DEGREE	!= Dlg.m_iColourTieClamp90Deg ||
			COLOUR_BRACING				!= Dlg.m_iColourBracing ||
			COLOUR_STAIR				!= Dlg.m_iColourStair ||
			COLOUR_LADDER				!= Dlg.m_iColourLadder ||
			COLOUR_STANDARD				!= Dlg.m_iColourStandards ||
			COLOUR_JACK					!= Dlg.m_iColourJacks ||
			COLOUR_SOLEBOARD			!= Dlg.m_iColourSoleBoards ||
			COLOUR_HOPUP_BRACKET		!= Dlg.m_iColourHopupBrackets ||
			COLOUR_CORNER_STAGE_BOARD	!= Dlg.m_iColourCornerStageBoard ||
			COLOUR_TEXT					!= Dlg.m_iColourText )
		{
			COLOUR_DECKING_PLANK		= Dlg.m_iColourDeckingPlank;
			COLOUR_STAGE_BOARD			= Dlg.m_iColourStageBoard;
			COLOUR_LAPBOARD				= Dlg.m_iColourLapboard;
			COLOUR_LEDGER				= Dlg.m_iColourLedger;
			COLOUR_TRANSOM				= Dlg.m_iColourTransom;
			COLOUR_MESH_GUARD			= Dlg.m_iColourMeshGuard;
			COLOUR_RAIL					= Dlg.m_iColourRail;
			COLOUR_MID_RAIL				= Dlg.m_iColourMidRail;
			COLOUR_TOE_BOARD			= Dlg.m_iColourToeBoard;
			COLOUR_CHAIN_LINK			= Dlg.m_iColourChainLink;
			COLOUR_SHADE_CLOTH			= Dlg.m_iColourShadeCloth;
			COLOUR_TIE_TUBE				= Dlg.m_iColourTieTubes;
			COLOUR_TIE_CLAMP_COLUMN		= Dlg.m_iColourTieClampColumn;
			COLOUR_TIE_CLAMP_MASONARY	= Dlg.m_iColourTieClampMasonary;
			COLOUR_TIE_CLAMP_YOKE		= Dlg.m_iColourTieClampYoke;
			COLOUR_TIE_CLAMP_90DEGREE	= Dlg.m_iColourTieClamp90Deg;
			COLOUR_BRACING				= Dlg.m_iColourBracing;
			COLOUR_STAIR				= Dlg.m_iColourStair;
			COLOUR_LADDER				= Dlg.m_iColourLadder;
			COLOUR_STANDARD				= Dlg.m_iColourStandards;
			COLOUR_JACK					= Dlg.m_iColourJacks;
			COLOUR_SOLEBOARD			= Dlg.m_iColourSoleBoards;
			COLOUR_HOPUP_BRACKET		= Dlg.m_iColourHopupBrackets;
			COLOUR_CORNER_STAGE_BOARD	= Dlg.m_iColourCornerStageBoard;
			COLOUR_TEXT					= Dlg.m_iColourText;

			sTemp1.Empty();
			sTemp2.Empty();
			sTemp3.Empty();
			if( !b3DRedrawnAlready && IsCrosshairCreated() && GetMatrix()!=NULL )
			{
				sTemp1 = _T("3D and Matrix views");
				sTemp2 = _T(" have");
				sTemp3 = _T(" them now?");
			}
			else if( GetMatrix()!=NULL )
			{
				sTemp1 = _T("Matrix view");
				sTemp2 = _T(" has");
				sTemp3 = _T(" it now?");
			}
			else if( !b3DRedrawnAlready && IsCrosshairCreated() )
			{
				sTemp1 = _T("3D view");
				sTemp2 = _T(" has");
				sTemp3 = _T(" it now?");
			}

			if( !sTemp1.IsEmpty() )
			{
				sMsg = _T("You have changed a colour which will affect the ");
				sMsg+= sTemp1;
				sMsg+= _T(".\nYour new colours will not take effect until the ");
				sMsg+= sTemp1;
				sMsg+= sTemp2;
				sMsg+= _T(" been redrawn.\n\n");
				sMsg+= _T("Would you like to redraw");
				sMsg+= sTemp3;

				if( MessageBox( NULL, sMsg, _T("Apply Colour Change"), MB_YESNO )==IDYES )
				{
					if( GetMatrix()!=NULL )
					{
						DeleteMatrix();
						CreateMatrix(true);
					}
					if( IsCrosshairCreated() )
					{
						assert( !b3DRedrawnAlready );
						Delete3DView();
						Create3DView( true );
					}
				}
			}
		}

		///////////////////////////////////////////////////////////
		//Schematic
		if(	COLOUR_0700					!= Dlg.m_iColourBay0800 ||
			COLOUR_1200					!= Dlg.m_iColourBay1200 ||
			COLOUR_1800					!= Dlg.m_iColourBay1800 ||
			COLOUR_2400					!= Dlg.m_iColourBay2400 )
		{
			COLOUR_0700					= Dlg.m_iColourBay0800;
			COLOUR_1200					= Dlg.m_iColourBay1200;
			COLOUR_1800					= Dlg.m_iColourBay1800;
			COLOUR_2400					= Dlg.m_iColourBay2400;

			if( GetNumberOfRuns()>0 )
			{
				sMsg = _T("You have changed a colour which will affect the Schematic View.\n");
				sMsg+= _T("Your new colours will not take effect until the Schematic has\n");
				sMsg+= _T("been redrawn.\n\n");
				sMsg+= _T("Would you like to redraw it now?\n");

				if( MessageBox( NULL, sMsg, _T("Apply Colour Change"), MB_YESNO )==IDYES )
				{
					RedrawSchematic( false );
				}
			}
		}

		///////////////////////////////////////////////////////////
		//Other
		if( COLOUR_CROSSHAIR				!= Dlg.m_iColourCrosshair ||
			COLOUR_CROSSHAIR_ARC			!= Dlg.m_iColourCrosshairArc ||
			COLOUR_MATRIX_RL_TEXT			!= Dlg.m_iColourMatrixRLText ||
			COLOUR_MATRIX_RL_LINES_MINOR	!= Dlg.m_iColourMatrixRLMinor ||
			COLOUR_MATRIX_RL_LINES_MAJOR	!= Dlg.m_iColourMatrixRLMajor ||
			COLOUR_MATRIX_RL_LINES_SPAN		!= Dlg.m_iColourMatrixRLSpan ||
			COLOUR_MATRIX_LEVELS_TEXT		!= Dlg.m_iColourMatrixLevelText ||
			COLOUR_MATRIX_LEVELS_LINES		!= Dlg.m_iColourMatrixLevelLines ||
			COLOUR_COMMITTED_FULLY			!= Dlg.m_iColourCommitted ||
			COLOUR_COMMITTED_PARTIALLY		!= Dlg.m_iColourPartialCommitted )
		{
			COLOUR_CROSSHAIR				= Dlg.m_iColourCrosshair;
			COLOUR_CROSSHAIR_ARC			= Dlg.m_iColourCrosshairArc;
			COLOUR_MATRIX_RL_TEXT			= Dlg.m_iColourMatrixRLText;
			COLOUR_MATRIX_RL_LINES_MINOR	= Dlg.m_iColourMatrixRLMinor;
			COLOUR_MATRIX_RL_LINES_MAJOR	= Dlg.m_iColourMatrixRLMajor;
			COLOUR_MATRIX_RL_LINES_SPAN		= Dlg.m_iColourMatrixRLSpan;
			COLOUR_MATRIX_LEVELS_TEXT		= Dlg.m_iColourMatrixLevelText;
			COLOUR_MATRIX_LEVELS_LINES		= Dlg.m_iColourMatrixLevelLines;
			COLOUR_COMMITTED_FULLY			= Dlg.m_iColourCommitted			;
			COLOUR_COMMITTED_PARTIALLY		= Dlg.m_iColourPartialCommitted	;
		}
	}
}

void Controller::SetColourByStageLevel( bool bColourByStageLevel )
{
	m_bColourByStageLevel = bColourByStageLevel;
}

bool Controller::IsColourByStageLevel()
{
	return m_bColourByStageLevel;
}

void Controller::ColourByStageLevel()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nColour 3D view by Stage and Level<%i>:"), IsColourByStageLevel()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
//	acedInitGet( RSG_OTHER, "");
	i = IsColourByStageLevel()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ColourByStageLevel();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetColourByStageLevel( (i==0)? false: true );
		break;
	}
}

void Controller::AddCornerStageBoard()
{
	int		iSize;
	BayList	Bays;

	acutPrintf( _T("\nSelect Bay to attach corner fillter to!") );
	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays );
	RemoveAndWarnOfCommittedBays(&Bays);

	//We are adding here, not removing, ignor the committed flag!
	//RemoveAndWarnOfCommittedBays( &Bays );

	//Size may have changed above!
	iSize = Bays.GetSize();

	/////////////////////////////////////////////////////////////////////////////
	//Perform the add
	Bay		*pBay;
	if( iSize>0 )
	{
		pBay = Bays.GetAt(0);

		//////////////////////////////////////////////////////////////////////
		//which corner are we talking about?  Lets figure which side is most likely?
		SideOfBayEnum eSide;
		int	iImpossibleSides = 0xF;
		if( pBay->GetForward()!=NULL )
		{
			iImpossibleSides^= IMPOSSIBLE_NE;	//can't be NE
			iImpossibleSides^= IMPOSSIBLE_SE;	//can't be SE
		}
		if( pBay->GetBackward()!=NULL )
		{
			iImpossibleSides^= IMPOSSIBLE_NW;	//can't be NW
			iImpossibleSides^= IMPOSSIBLE_SW;	//can't be SW
		}
		if( pBay->GetInner()!=NULL )
		{
			iImpossibleSides^= IMPOSSIBLE_SE;	//can't be SE
			iImpossibleSides^= IMPOSSIBLE_SW;	//can't be SW
		}
		if( pBay->GetOuter()!=NULL )
		{
			iImpossibleSides^= IMPOSSIBLE_NE;	//can't be NE
			iImpossibleSides^= IMPOSSIBLE_NW;	//can't be NW
		}

		
		if( iImpossibleSides&IMPOSSIBLE_NW )
		{
			if( !pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) && 
				!pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) )
				iImpossibleSides^= IMPOSSIBLE_NW;	//can't be NW
			else
				eSide = NW;
		}
		if( iImpossibleSides&IMPOSSIBLE_NE )
		{
			if( !pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) && 
				!pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) )
				iImpossibleSides^= IMPOSSIBLE_NE;	//can't be NE
			else
				eSide = SE;
		}
		if( iImpossibleSides&IMPOSSIBLE_SW )
		{
			if( !pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, SOUTH ) && 
				!pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) )
				iImpossibleSides^= IMPOSSIBLE_SW;	//can't be SW
			else
				eSide = SW;
		}
		if( iImpossibleSides&IMPOSSIBLE_SE )
		{
			if( !pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) && 
				!pBay->HasComponentOfTypeOnSide( CT_STAGE_BOARD, SOUTH ) )
				iImpossibleSides^= IMPOSSIBLE_SE;	//can't be SE
			else
				eSide = SE;
		}
			
		if( !GetCornerOfBayFromCommandLine( eSide, &Bays ) )
			return;

		AddIndividualCornerFiller( pBay, eSide );
	}
}

void Controller::RemoveCornerStageBoard()
{
	BayList	Bays;

	if( !SelectBays( Bays, true ) )
		return;

	CheckForStairLadder( &Bays );

	RemoveAndWarnOfCommittedBays( &Bays );

	//Size may have changed above!
	int		iSize;
	iSize = Bays.GetSize();

	/////////////////////////////////////////////////////////////////////////////
	//Perform the edit!
	Bay		*pBay;
	if( iSize>0 )
	{
		pBay = Bays.GetAt(0);

		//Which corner are we talking about?
		SideOfBayEnum eSide;
		eSide = SE;
		if( pBay->GetTemplate()->GetNWStageCnr() )
			eSide = NW;
		if( pBay->GetTemplate()->GetSWStageCnr() )
			eSide = SW;
		if( pBay->GetTemplate()->GetSEStageCnr() )
			eSide = SE;
		if( pBay->GetTemplate()->GetNEStageCnr() )
			eSide = NE;
		if( !GetCornerOfBayFromCommandLine( eSide, &Bays ) )
			return;

		/////////////////////////////////////////////////////////////////////
		//We need some of the particulars relating to the side
		int		i;
		SideOfBayEnum	eSideLeft, eSideRight, eSideHopupLeft, eSideHopupRight;
		switch( eSide )
		{
		case( NORTH_EAST ):
			eSideLeft	= NORTH_NORTH_EAST;
			eSideRight	= EAST_NORTH_EAST;
			eSideHopupLeft	= N;
			eSideHopupRight = W;
			break;
		case( SOUTH_EAST ):
			eSideLeft	= EAST_SOUTH_EAST;
			eSideRight	= SOUTH_SOUTH_EAST;
			eSideHopupLeft	= E;
			eSideHopupRight = S;
			break;
		case( SOUTH_WEST ):
			eSideLeft	= SOUTH_SOUTH_WEST;
			eSideRight	= WEST_SOUTH_WEST;
			eSideHopupLeft	= S;
			eSideHopupRight = W;
			break;
		case( NORTH_WEST ):
			eSideLeft	= WEST_NORTH_WEST;
			eSideRight	= NORTH_NORTH_WEST;
			eSideHopupLeft	= W;
			eSideHopupRight = N;
			break;
		default:
			//invalid side
			assert( false );
			return;
		}

		Lift	*pLift;
		for( i=pBay->GetNumberOfLifts()-1; i>=0; i-- )
		{
			pLift = pBay->GetLift(i);
			pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, eSide );
			if( pLift->GetComponentOfTypeOnSide( CT_STAGE_BOARD, eSideHopupLeft )==NULL )
			{
				pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSideLeft );
			}
			if( pLift->GetComponentOfTypeOnSide( CT_STAGE_BOARD, eSideHopupRight )==NULL )
			{
				pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSideRight );
			}
		}

		switch( eSide )
		{
		case( NORTH_EAST ):
			pBay->GetTemplate()->SetNEStageCnr(false);
			break;
		case( SOUTH_EAST ):
			pBay->GetTemplate()->SetSEStageCnr(false);
			break;
		case( SOUTH_WEST ):
			pBay->GetTemplate()->SetSWStageCnr(false);
			break;
		case( NORTH_WEST ):
			pBay->GetTemplate()->SetNWStageCnr(false);
			break;
		default:
			//invalid side
			assert( false );
			return;
		}
		pBay->UpdateSchematicView();
	}
}

void Controller::SetDisplaySalePrice(bool bDisplay)
{
	m_bDisplaySalePrice = bDisplay;
}

bool Controller::IsDisplaySalePrice()
{
	return m_bDisplaySalePrice;
}

void Controller::DisplaySalePrice()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nDisplay the Sale Price in the BOM<%i>:"), IsDisplaySalePrice()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
//	acedInitGet( RSG_OTHER, "");
	i = IsDisplaySalePrice()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		DisplaySalePrice();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetDisplaySalePrice( (i==0)? false: true );
		break;
	}
}

AcDbObjectIdArray *Controller::GetCopiedObjectArray()
{
	return &m_oiaCopiedObjects;
}

bool Controller::GetWidthOfCornerFillerFromCommandLine( SideOfBayEnum eSide, int &iNumberOfBoards, SideOfBayEnum eSetSide, int iSetNumberOfBoards )
{
	int				i;
	TCHAR			keywrd[255], prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nThis Lift has a %i board hopup to the %s, how many boards wide should this corner fill be on the %s side? [1/2/3] <%i>:"),
						iSetNumberOfBoards, GetSideOfBayDescStr(eSetSide), GetSideOfBayDescStr(eSide), iNumberOfBoards );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
	acedInitGet( RSG_OTHER, _T("1 2 3"));
	int iRet = acedGetKword( prompt, keywrd);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			int iValue = _ttoi( keywrd );
			if( iValue>=NUM_PLANKS_FOR_STAGE_BRD_1_WIDE &&
				iValue<=NUM_PLANKS_FOR_STAGE_BRD_3_WIDE )
			{
				iNumberOfBoards = iValue;
			}
			else
			{
				CString sMsg;
				sMsg.Format( _T("Please Enter a number of Boards for the corner filler, valid numbers are between %i and %i!"),
								NUM_PLANKS_FOR_STAGE_BRD_1_WIDE, NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox( NULL, sMsg, _T("Invalid Number of boards"), MB_OK );
				return GetWidthOfCornerFillerFromCommandLine( eSide, iNumberOfBoards, eSetSide, iSetNumberOfBoards );
			}
		}
		else
		{
			CString sMsg;
			sMsg.Format( _T("Please Enter a number of Boards for the corner filler, valid numbers are between %i and %i!"),
							NUM_PLANKS_FOR_STAGE_BRD_1_WIDE, NUM_PLANKS_FOR_STAGE_BRD_3_WIDE );
			MessageBeep(MB_ICONEXCLAMATION);
			MessageBox( NULL, sMsg, _T("Invalid Number of boards"), MB_OK );
			return GetWidthOfCornerFillerFromCommandLine( eSide, iNumberOfBoards, eSetSide, iSetNumberOfBoards );
		}
	}
	return true;
}

void Controller::WallOffsetFromLowestHopup()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt;

  sPrompt.Format( _T("\nEnter new value for 'Use Bottom Hopup for Wall Separation' <%i>:"),
			IsWallOffsetFromLowestHopup()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the Setting from the user.
	i = IsWallOffsetFromLowestHopup()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		WallOffsetFromLowestHopup();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetWallOffsetFromLowestHopup( (i==0)? false: true );
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//IsWallOffsetFromLowestHopup()
//
bool Controller::IsWallOffsetFromLowestHopup()
{
	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( WALL_OFFSET_FROM_BOTTOM_HOPUP, WALL_OFFSET_FROM_BOTTOM_HOPUP_FLAG_DEFAULT );
	return (iRet==0)? false: true;
}	//IsWallOffsetFromLowestHopup()

/////////////////////////////////////////////////////////////////////////////////////////
//SetWallOffsetFromLowestHopup( bool bLowestHopup )
//
void Controller::SetWallOffsetFromLowestHopup( bool bLowestHopup )
{
	int iVal;

	//Store in the registry
	iVal = bLowestHopup? 1: 0;
	SetIntInRegistry( WALL_OFFSET_FROM_BOTTOM_HOPUP, iVal );

	double dWidth;
	dWidth = GetABTools()->GetABBay()->GetWidthOfStageFromWall( SOUTH );
	GetABTools()->GetABBay()->GetTemplate()->SetWidthOfSStage( dWidth );
}	//SetWallOffsetFromLowestHopup(bool bLowestHopup)


bool Controller::IsUseToeboardOnLapboard()
{
	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( USE_TOE_BOARD_ON_LAPBOARD, USE_TOEBOARD_ON_LAPBOARDS_FLAG_DEFAULT );
	return (iRet==0)? false: true;
}

void Controller::SetUseToeboardOnLapboard(bool bUseToeboard)
{
	int iVal;

	//Store in the registry
	iVal = bUseToeboard? 1: 0;
	SetIntInRegistry( USE_TOE_BOARD_ON_LAPBOARD, iVal );
}

void Controller::UseToeboardOnLapboard()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt;

  sPrompt.Format( _T("\nEnter new value for 'Use Toeboard On Lapboards' <%i>:"),
			IsUseToeboardOnLapboard()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the Setting from the user.  Do not specify a base point.
	i = IsUseToeboardOnLapboard()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		UseToeboardOnLapboard();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetUseToeboardOnLapboard( (i==0)? false: true );
		break;
	}
}

void Controller::ConvertToMillsCorner()
{
	if( GetSystem()!=S_MILLS )
	{
		MessageBeep(MB_ICONSTOP);
		CString sMessage;
		sMessage = _T("You must be in mills system to convert to a mills system bay!");
		MessageBox( NULL, sMessage, _T("Convert to Mills system bay error"), MB_OK|MB_ICONSTOP );
		return;
	}

	Bay			*pBay;
	BayList		Bays;

	if( !SelectBays( Bays, false ) )
		return;

	CheckForStairLadder( &Bays );

	//check the bays are all the same dimensions
	CheckBaysAreSameSize( &Bays );

	RemoveAndWarnOfCommittedBays( &Bays );

	int				i, iSize;
	MillsType		mtMillsCnrType, mtAvailalbeTypes;

	//Size may have changed above!
	iSize = Bays.GetSize();

	/////////////////////////////////////////////////////////////////////////////
	//Perform the edit!
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		double dWidth, dLength;
		pBay = Bays[0];
		dWidth	= pBay->GetBayWidth();
		dLength = pBay->GetBayLength();
		mtAvailalbeTypes = MILLS_TYPE_NONE;
		mtMillsCnrType = MILLS_TYPE_NONE;
		if( dWidth>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			dWidth<COMPONENT_LENGTH_1200+ROUND_ERROR )
		{
			if( !pBay->HasMillsTypeGap(W) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_W;
				mtMillsCnrType = MILLS_TYPE_CONNECT_W;
			}
			if( !pBay->HasMillsTypeGap(E) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_E;
				mtMillsCnrType = MILLS_TYPE_CONNECT_E;
			}
		}
		if( dWidth>COMPONENT_LENGTH_2400-ROUND_ERROR &&
			dWidth<COMPONENT_LENGTH_2400+ROUND_ERROR )
		{
			if( !pBay->HasMillsTypeGap(WSW) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_WSW;
				mtMillsCnrType = MILLS_TYPE_CONNECT_WSW;
			}
			if( !pBay->HasMillsTypeGap(WNW) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_WNW;
				mtMillsCnrType = MILLS_TYPE_CONNECT_WNW;
			}
			if( !pBay->HasMillsTypeGap(ESE) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_ESE;
				mtMillsCnrType = MILLS_TYPE_CONNECT_ESE;
			}
			if( !pBay->HasMillsTypeGap(ENE) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_ENE;
				mtMillsCnrType = MILLS_TYPE_CONNECT_ENE;
			}
		}
		if( dLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			dLength<COMPONENT_LENGTH_1200+ROUND_ERROR )
		{
			if( !pBay->HasMillsTypeGap(S) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_S;
				mtMillsCnrType = MILLS_TYPE_CONNECT_S;
			}
			if( !pBay->HasMillsTypeGap(N) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_N;
				mtMillsCnrType = MILLS_TYPE_CONNECT_N;
			}
		}
		if( dLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
			dLength<COMPONENT_LENGTH_2400+ROUND_ERROR )
		{
			if( !pBay->HasMillsTypeGap(NNW) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_NNW;
				mtMillsCnrType = MILLS_TYPE_CONNECT_NNW;
			}
			if( !pBay->HasMillsTypeGap(SSW) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_SSW;
				mtMillsCnrType = MILLS_TYPE_CONNECT_SSW;
			}
			if( !pBay->HasMillsTypeGap(SSE) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_SSE;
				mtMillsCnrType = MILLS_TYPE_CONNECT_SSE;
			}
			if( !pBay->HasMillsTypeGap(NNE) )
			{
				mtAvailalbeTypes|=MILLS_TYPE_CONNECT_NNE;
				mtMillsCnrType = MILLS_TYPE_CONNECT_NNE;
			}
		}

		if( !pBay->HasMillsTypeMissingStandard(NE) )
			mtAvailalbeTypes|=MILLS_TYPE_MISSING_STND_NE;
		if( !pBay->HasMillsTypeMissingStandard(SE) )
			mtAvailalbeTypes|=MILLS_TYPE_MISSING_STND_SE;
		if( !pBay->HasMillsTypeMissingStandard(SW) )
			mtAvailalbeTypes|=MILLS_TYPE_MISSING_STND_SW;
		if( !pBay->HasMillsTypeMissingStandard(NW) )
			mtAvailalbeTypes|=MILLS_TYPE_MISSING_STND_NW;

		if( (mtMillsCnrType&MILLS_TYPE_CONNECT_TEST)==0 ||
			(mtAvailalbeTypes&MILLS_TYPE_CONNECT_TEST)==0 )
		{
			MessageBeep(MB_ICONSTOP);
			CString sMessage;
			sMessage = _T("It is not possible to convert this bay for mills connections!");
			MessageBox( NULL, sMessage, _T("Convert to Mills system bay error"), MB_OK|MB_ICONSTOP );
			return;
		}

		if( !GetMillsTypeGapFromCommandLine( &Bays, mtMillsCnrType, mtAvailalbeTypes ) )
			return;
		if( !GetMillsTypeStdFromCommandLine( &Bays, mtMillsCnrType, mtAvailalbeTypes ) )
			return;

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays[i];
			pBay->SetMillsSystemBay( mtMillsCnrType );
			pBay->RedrawPlanView();
		}
	}
}

void Controller::UnconvertMillsCorner()
{
	if( GetSystem()!=S_MILLS )
	{
		MessageBeep(MB_ICONSTOP);
		CString sMessage;
		sMessage = _T("You must be in mills system to convert from a mills system bay!");
		MessageBox( NULL, sMessage, _T("Convert to Mills system bay error"), MB_OK|MB_ICONSTOP );
		return;
	}

	BayList	Bays;

	if( !SelectBays( Bays, false ) )
		return;

	CheckForStairLadder( &Bays );

	RemoveAndWarnOfCommittedBays( &Bays );

	//Size may have changed above!
	int	i, iSize;
	iSize = Bays.GetSize();

	/////////////////////////////////////////////////////////////////////////////
	//Perform the edit!
	Bay		*pBay;
	if( iSize>0 )
	{
		Bays.DisplaySelectedBays();

		for( i=0; i<iSize; i++ )
		{
			pBay = Bays[i];
//			if( pBay->GetMillsSystemType()!=MILLS_TYPE_CONNECT_NNE &&
//				pBay->GetMillsSystemType()!=MILLS_TYPE_CONNECT_SSE )

			pBay->SetMillsSystemBay( MILLS_TYPE_NONE );
			pBay->RedrawPlanView();
		}
/*
		MessageBeep(MB_ICONWARNING);
		CString sMessage;
		sMessage = "You may have to manually recreate the hopups and bracing for ";
		if( iSize==1 )
			sMessage+= "this bay!";
		else
			sMessage+= "these bays!";
		MessageBox( NULL, sMessage, "Reminder", MB_OK|MB_ICONWARNING );
*/	}
}

bool Controller::GetMillsTypeGapFromCommandLine( BayList *pBays, MillsType &mtType, MillsType mtAvailableTypes )
{
	int				i;
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd, sSide, sTemp, sOptions;
	intArray		iaAvailableSides;
	CStringArray	saSide;

	//////////////////////////////////////////////////////////////////
	//What are our choices
	saSide.RemoveAll();
	iaAvailableSides.RemoveAll();
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_N )
	{
		iaAvailableSides.Add( BP_N_MILLS );
		saSide.Add( _T("N") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_E )
	{
		iaAvailableSides.Add( BP_E_MILLS );
		saSide.Add( _T("E") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_S )
	{
		iaAvailableSides.Add( BP_S_MILLS );
		saSide.Add( _T("S") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_W )
	{
		iaAvailableSides.Add( BP_W_MILLS );
		saSide.Add( _T("W") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_NNE )
	{
		iaAvailableSides.Add( BP_NNE_STG );
		saSide.Add( _T("NNE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_ENE )
	{
		iaAvailableSides.Add( BP_ENE_STG );
		saSide.Add( _T("ENE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_ESE )
	{
		iaAvailableSides.Add( BP_ESE_STG );
		saSide.Add( _T("ESE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_SSE )
	{
		iaAvailableSides.Add( BP_SSE_STG );
		saSide.Add( _T("SSE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_SSW )
	{
		iaAvailableSides.Add( BP_SSW_STG );
		saSide.Add( _T("SSW") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_WSW )
	{
		iaAvailableSides.Add( BP_WSW_STG );
		saSide.Add( _T("WSW") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_WNW )
	{
		iaAvailableSides.Add( BP_WNW_STG );
		saSide.Add( _T("WNW") );
	}
	if( mtAvailableTypes&MILLS_TYPE_CONNECT_NNW )
	{
		iaAvailableSides.Add( BP_NNW_STG );
		saSide.Add( _T("NNW") );
	}


	/////////////////////////////////////////////////////////////////////////
	//Get the current settings
	sSide.Empty();
	if( mtType&MILLS_TYPE_CONNECT_N )
		sSide = _T("N");
	if( mtType&MILLS_TYPE_CONNECT_E )
		sSide = _T("E");
	if( mtType&MILLS_TYPE_CONNECT_S )
		sSide = _T("S");
	if( mtType&MILLS_TYPE_CONNECT_W )
		sSide = _T("W");
	if( mtType&MILLS_TYPE_CONNECT_NNE )
		sSide = _T("NNE");
	if( mtType&MILLS_TYPE_CONNECT_ENE )
		sSide = _T("ENE");
	if( mtType&MILLS_TYPE_CONNECT_ESE )
		sSide = _T("ESE");
	if( mtType&MILLS_TYPE_CONNECT_SSE )
		sSide = _T("SSE");
	if( mtType&MILLS_TYPE_CONNECT_SSW )
		sSide = _T("SSW");
	if( mtType&MILLS_TYPE_CONNECT_WSW )
		sSide = _T("WSW");
	if( mtType&MILLS_TYPE_CONNECT_WNW )
		sSide = _T("WNW");
	if( mtType&MILLS_TYPE_CONNECT_NNW )
		sSide = _T("NNW");

	sPrompt = _T("\nConnect to which side of the bay[");
	sOptions.Empty();
	for( i=0; i<saSide.GetSize(); i++ )
	{
		sPrompt += saSide[i];
		sOptions+= saSide[i];

		if( i<saSide.GetSize()-1) 
		{
			sPrompt += _T("/");
			sOptions+= _T(" ");
		}
	}
	sPrompt+= _T("] <");
	sPrompt+= sSide;
  sPrompt+= _T(">:");

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	bool			bFound;
	Point3D			ptPoint;
	SideOfBayEnum	eSide;

	acedInitGet( RSG_OTHER, sOptions );
	int iRet = GetPoint3D( sPrompt, ptPoint );
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		mtType&=MILLS_TYPE_CONNECT_TEST;
		if     ( (mtType&MILLS_TYPE_CONNECT_N)>0 )		mtType|=MILLS_TYPE_MISSING_STND_NE;
		else if( (mtType&MILLS_TYPE_CONNECT_E)>0 )		mtType|=MILLS_TYPE_MISSING_STND_NE;
		else if( (mtType&MILLS_TYPE_CONNECT_S)>0 )		mtType|=MILLS_TYPE_MISSING_STND_SE;
		else if( (mtType&MILLS_TYPE_CONNECT_W)>0 )		mtType|=MILLS_TYPE_MISSING_STND_NW;
		else if( (mtType&MILLS_TYPE_CONNECT_NNE)>0 )	mtType|=MILLS_TYPE_MISSING_STND_NE;
		else if( (mtType&MILLS_TYPE_CONNECT_ENE)>0 )	mtType|=MILLS_TYPE_MISSING_STND_NE;
		else if( (mtType&MILLS_TYPE_CONNECT_ESE)>0 )	mtType|=MILLS_TYPE_MISSING_STND_SE;
		else if( (mtType&MILLS_TYPE_CONNECT_SSE)>0 )	mtType|=MILLS_TYPE_MISSING_STND_SE;
		else if( (mtType&MILLS_TYPE_CONNECT_SSW)>0 )	mtType|=MILLS_TYPE_MISSING_STND_SW;
		else if( (mtType&MILLS_TYPE_CONNECT_WSW)>0 )	mtType|=MILLS_TYPE_MISSING_STND_SW;
		else if( (mtType&MILLS_TYPE_CONNECT_WNW)>0 )	mtType|=MILLS_TYPE_MISSING_STND_NW;
		else if( (mtType&MILLS_TYPE_CONNECT_NNW)>0 )	mtType|=MILLS_TYPE_MISSING_STND_NW;
		else
		{
			assert( false );
			return GetMillsTypeGapFromCommandLine( pBays, mtType, mtAvailableTypes );
		}
		break;
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return GetMillsTypeGapFromCommandLine( pBays, mtType, mtAvailableTypes );
		}
		else
		{
			bFound = false;
			for( i=0; i<saSide.GetSize(); i++ )
			{
				if( sKeyWrd==saSide[i] )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{			
				MessageBeep(MB_ICONEXCLAMATION);
				return GetMillsTypeGapFromCommandLine( pBays, mtType, mtAvailableTypes );
			}
	
			//set the connection type an also pick the most likely standard
			mtType = MILLS_TYPE_NONE;
			if(	sKeyWrd == _T("N") )
			{
				mtType|=MILLS_TYPE_CONNECT_N;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
			}
			else if(	sKeyWrd == _T("E") )
			{
				mtType|=MILLS_TYPE_CONNECT_E;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
			}
			else if(	sKeyWrd == _T("S") )
			{
				mtType|=MILLS_TYPE_CONNECT_S;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
			}
			else if(	sKeyWrd == _T("W") )
			{
				mtType|=MILLS_TYPE_CONNECT_W;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
			}
			else if(	sKeyWrd == _T("NNE") )
			{
				mtType|=MILLS_TYPE_CONNECT_NNE;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
			}
			else if(	sKeyWrd == _T("ENE") )
			{
				mtType|=MILLS_TYPE_CONNECT_ENE;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
			}
			else if(	sKeyWrd == _T("ESE") )
			{
				mtType|=MILLS_TYPE_CONNECT_ESE;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
			}
			else if(	sKeyWrd == _T("SSE") )
			{
				mtType|=MILLS_TYPE_CONNECT_SSE;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
			}
			else if(	sKeyWrd == _T("SSW") )
			{
				mtType|=MILLS_TYPE_CONNECT_SSW;
				mtType|=MILLS_TYPE_MISSING_STND_SW;
			}
			else if(	sKeyWrd == _T("WSW") )
			{
				mtType|=MILLS_TYPE_CONNECT_WSW;
				mtType|=MILLS_TYPE_MISSING_STND_SW;
			}
			else if(	sKeyWrd == _T("WNW") )
			{
				mtType|=MILLS_TYPE_CONNECT_WNW;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
			}
			else if(	sKeyWrd == _T("NNW") )
			{
				mtType|=MILLS_TYPE_CONNECT_NNW;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
			}
			else 
			{
				//what happend in the test above, this should not happen
				assert( false );
				return false;
			}
			break;
		}
	case( RTNORM ):				//(5100) Request succeeded
		if( GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide )>=0 )
		{
			//the have clicked a valid side, better reset the type 
			mtType = MILLS_TYPE_NONE;

			switch( eSide )
			{
			case( N ):
				mtType|=MILLS_TYPE_CONNECT_N;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
				break;
			case( E ):
				mtType|=MILLS_TYPE_CONNECT_E;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
				break;
			case( S ):
				mtType|=MILLS_TYPE_CONNECT_S;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
				break;
			case( W ):
				mtType|=MILLS_TYPE_CONNECT_W;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
				break;
			case( NNE ):
				mtType|=MILLS_TYPE_CONNECT_NNE;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
				break;
			case( ENE ):
				mtType|=MILLS_TYPE_CONNECT_ENE;
				mtType|=MILLS_TYPE_MISSING_STND_NE;
				break;
			case( ESE ):
				mtType|=MILLS_TYPE_CONNECT_ESE;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
				break;
			case( SSE ):
				mtType|=MILLS_TYPE_CONNECT_SSE;
				mtType|=MILLS_TYPE_MISSING_STND_SE;
				break;
			case( SSW ):
				mtType|=MILLS_TYPE_CONNECT_SSW;
				mtType|=MILLS_TYPE_MISSING_STND_SW;
				break;
			case( WSW ):
				mtType|=MILLS_TYPE_CONNECT_WSW;
				mtType|=MILLS_TYPE_MISSING_STND_SW;
				break;
			case( WNW ):
				mtType|=MILLS_TYPE_CONNECT_WNW;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
				break;
			case( NNW ):
				mtType|=MILLS_TYPE_CONNECT_NNW;
				mtType|=MILLS_TYPE_MISSING_STND_NW;
				break;
			default:
				assert( false );	//I thought this was a valid side!!!!
				break;
			}
		}

		return GetMillsTypeGapFromCommandLine( pBays, mtType, mtAvailableTypes );
	}
	return true;
}

bool Controller::GetMillsTypeStdFromCommandLine( BayList *pBays, MillsType &mtType, MillsType mtAvailableTypes )
{
	int				i;
	TCHAR			keywrd[255];
	CString			sPrompt, sKeyWrd, sStandard, sTemp, sOptions;
	Point3D			ptPoint;
	intArray		iaAvailableSides;
	CStringArray	saStandards;
	SideOfBayEnum	eSide;

	//////////////////////////////////////////////////////////////////
	//What are our choices
	saStandards.RemoveAll();

	iaAvailableSides.RemoveAll();
	saStandards.Add( _T("NONE") );
	if( mtAvailableTypes&MILLS_TYPE_MISSING_STND_NE )
	{
		iaAvailableSides.Add( BP_NE_STD );
		saStandards.Add( _T("NE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_MISSING_STND_SE )
	{
		iaAvailableSides.Add( BP_SE_STD );
		saStandards.Add( _T("SE") );
	}
	if( mtAvailableTypes&MILLS_TYPE_MISSING_STND_SW )
	{
		iaAvailableSides.Add( BP_SW_STD );
		saStandards.Add( _T("SW") );
	}
	if( mtAvailableTypes&MILLS_TYPE_MISSING_STND_NW )
	{
		iaAvailableSides.Add( BP_NW_STD );
		saStandards.Add( _T("NW") );
	}

	/////////////////////////////////////////////////////////////////////////
	//Get the current settings
	sStandard.Empty();
	if( mtType&MILLS_TYPE_MISSING_STND_NE )
		sStandard = _T("NE");
	else if( mtType&MILLS_TYPE_MISSING_STND_SE )
		sStandard = _T("SE");
	else if( mtType&MILLS_TYPE_MISSING_STND_SW )
		sStandard = _T("SW");
	else if( mtType&MILLS_TYPE_MISSING_STND_NW )
		sStandard = _T("NW");
	else 
		sStandard = _T("NONE");

	sPrompt = _T("\nWhich standard should we remove [");
	sOptions.Empty();
	for( i=0; i<saStandards.GetSize(); i++ )
	{
		sPrompt += saStandards[i];
		sOptions+= saStandards[i];

		if( i<saStandards.GetSize()-1) 
		{
			sPrompt += _T("/");
			sOptions+= _T(" ");
		}
	}
	sPrompt+= _T("] <");
	sPrompt+= sStandard;
  sPrompt+= _T(">:");

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	acedInitGet( RSG_OTHER, sOptions );
	int iRet = GetPoint3D( sPrompt, ptPoint );
	bool	bFound;
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		RefreshScreen();
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		RefreshScreen();
		MessageBeep(MB_ICONEXCLAMATION);
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		acedGetInput( keywrd );
		sKeyWrd = keywrd;
		sKeyWrd.MakeUpper();

		if( _istdigit( keywrd[0] ) )
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return GetMillsTypeStdFromCommandLine( pBays, mtType, mtAvailableTypes );
		}
		else
		{
			bFound = false;
			for( i=0; i<saStandards.GetSize(); i++ )
			{
				if( sKeyWrd==saStandards[i] )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{			
				MessageBeep(MB_ICONEXCLAMATION);
				return GetMillsTypeStdFromCommandLine( pBays, mtType, mtAvailableTypes );
			}
	
			//Keep the connection but clear the rest!
			mtType&=MILLS_TYPE_CONNECT_TEST;
			if(	sKeyWrd == _T("NE") )
			{
				mtType|=MILLS_TYPE_MISSING_STND_NE;
			}
			else if(	sKeyWrd == _T("SE") )
			{
				mtType|=MILLS_TYPE_MISSING_STND_SE;
			}
			else if(	sKeyWrd == _T("SW") )
			{
				mtType|=MILLS_TYPE_MISSING_STND_SW;
			}
			else if(	sKeyWrd == _T("NW") )
			{
				mtType|=MILLS_TYPE_MISSING_STND_NW;
			}
			else if(	sKeyWrd == _T("NONE") )
			{
				//fine
				1;
			}
			else 
			{
				//what happend in the test above, this should not happen
				assert( false );
				return false;
			}
			break;
		}
	case( RTNORM ):				//(5100) Request succeeded
		if( GetSideOfBayFromPoint( pBays, ptPoint, iaAvailableSides, eSide )>=0 )
		{
			//Keep the connection but clear the rest!
			mtType&=MILLS_TYPE_CONNECT_TEST;

			switch( eSide )
			{
			case( NE ):
				mtType|=MILLS_TYPE_MISSING_STND_NE;
				break;
			case( SE ):
				mtType|=MILLS_TYPE_MISSING_STND_SE;
				break;
			case( SW ):
				mtType|=MILLS_TYPE_MISSING_STND_SW;
				break;
			case( NW ):
				mtType|=MILLS_TYPE_MISSING_STND_NW;
				break;
			default:
				assert( false );	//I thought this was supposed to be a valid side!!
				break;
			}
		}

		GetMillsTypeStdFromCommandLine( pBays, mtType, mtAvailableTypes );
	}
	return true;
}

void Controller::CheckBaysAreSameSize(BayList *pBays)
{
	int			i, iSize;
	Bay			*pBay;
	double		dWidth, dLength;
	intArray	iaBaysRemoved;

	iSize = pBays->GetSize();
	if( iSize>0 )
	{
		iaBaysRemoved.RemoveAll();
		pBay = pBays->GetAt(0);
		dLength	= pBay->GetBayLength();
		dWidth	= pBay->GetBayWidth();
		
		for( i=1; i<pBays->GetSize(); i++ )
		{
			pBay = pBays->GetAt(i);
			if( dLength<pBay->GetBayLength()-ROUND_ERROR || 
				dLength>pBay->GetBayLength()+ROUND_ERROR ||
				dWidth <pBay->GetBayWidth() -ROUND_ERROR || 
				dWidth >pBay->GetBayWidth() +ROUND_ERROR )
			{
				iaBaysRemoved.Add(pBay->GetBayNumber());
				pBays->RemoveAt(i);
				i--;
			}
		}

		if( pBays->GetSize()<=0 )
		{
			//How the hell can this happen it is impossible!!!!
			assert( false );
		}

		if( iaBaysRemoved.GetSize()>0 )
		{
			CString sMsg, sTemp;
			if( iaBaysRemoved.GetSize()==1 )
			{
				sMsg.Format( _T("Bay %i "), iaBaysRemoved.GetAt(0) );
				sTemp.Format( _T("is not the same dimensions as bay %i, so I\n"), pBays->GetAt(0)->GetBayNumber() );
				sMsg+= sTemp;
				sMsg+= _T("have removed it from the selection!\n\n");
			}
			if( iaBaysRemoved.GetSize()>1 )
			{
				sMsg.Format( _T("Bays %i "), iaBaysRemoved.GetAt(0) );
				for( i=1; i<iaBaysRemoved.GetSize()-1; i++ )
				{
					sTemp.Format( _T(", %i "), iaBaysRemoved.GetAt(i) );
					sMsg+= sTemp;
				}
				sTemp.Format( _T(" and %i\n"), iaBaysRemoved.GetAt(iaBaysRemoved.GetSize()-1) );
				sMsg+= sTemp;

				sTemp.Format( _T("are not the same dimensions as the bay %i, so I\n"), pBays->GetAt(0)->GetBayNumber() );
				sMsg+= sTemp;
				sMsg+= _T("have removed them from the selection!\n\n");
			}
      sMsg+= _T("NOTE: All bays selected must be of the same size!\n");
			MessageBox( NULL, sMsg, _T("Mills Convert Bay Selection Error"), MB_OK|MB_ICONEXCLAMATION );
		}
	}
}

bool Controller::CheckRoofProtectionIsValid( bool bContinueCancel )
{
	int				iRun, iBay, iNumberInvalid;
	Run				*pRun;
	Bay				*pBay;
	CString			sInvalidBays, sTemp, sMatrix, sMatrixAlt;
	SideOfBayEnum	eInvalidSide;

	sInvalidBays.Empty();

	//go through each bay run until the bay we are looking for is within the run
	iNumberInvalid = 0;
	for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
	{
		pRun = GetRun(iRun);
		assert( pRun!=NULL );
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay( iBay );
			assert( pBay!=NULL );
			eInvalidSide = pBay->FindInvalidRoofProtectionStandards();
			switch( eInvalidSide )
			{
			case( NORTH ):
        sTemp.Format( _T("\nBay%i: North-East & North-West"), pBay->GetBayNumber() );
				sInvalidBays+= sTemp;
				if( GetMatrix()!=NULL && pBay->GetMatrixElementPointer()!=NULL
					 && pBay->GetMatrixElementPointerAlt()!=NULL )
				{
          sTemp.Format( _T("(Matrix Elements: %s & %s)"), 
							pBay->GetMatrixElementPointer()->GetLabel(),
							pBay->GetMatrixElementPointerAlt()->GetLabel() );
					sInvalidBays+= sTemp;
				}
				iNumberInvalid++;
				break;
			case( NORTH_WEST ):
        sTemp.Format( _T("\nBay%i: North-West"), pBay->GetBayNumber() );
				sInvalidBays+= sTemp;
				if( GetMatrix()!=NULL && pBay->GetMatrixElementPointerAlt()!=NULL )
				{
          sTemp.Format( _T("(Matrix Element: %s)"),
							pBay->GetMatrixElementPointerAlt()->GetLabel() );
					sInvalidBays+= sTemp;
				}
				iNumberInvalid++;
				break;
			case( NORTH_EAST ):
        sTemp.Format( _T("\nBay%i: North-East"), pBay->GetBayNumber() );
				sInvalidBays+= sTemp;
				if( GetMatrix()!=NULL && pBay->GetMatrixElementPointer()!=NULL )
				{
          sTemp.Format( _T("(Matrix Element: %s)"), 
							pBay->GetMatrixElementPointer()->GetLabel() );
					sInvalidBays+= sTemp;
				}
				iNumberInvalid++;
				break;
			case( SIDE_INVALID ):
				//Fine!
				break;
			default:
				assert( false );
			}
		}
	}

	if( !sInvalidBays.IsEmpty() )
	{
		if( iNumberInvalid>10 )
		{
			CString sMsg;
      sMsg = _T("Warning:  There are a substantail number of bays that do\n");
			sMsg+= _T("not have a 3.0m standard as the top standard, yet each of\n");
			sMsg+= _T("these bays are marked as requiring roof protection!  Due\n");
			sMsg+= _T("to the length of this list I have sent the full list of\n");
			sMsg+= _T("affected bays to the Autocad command line.\n");
			sMsg+= _T("\nYou should edit the standards so that top standard is\n");
			sMsg+= _T("3.0m long!\n");
      sMsg+= _T("\nHINT: press 'F2' to enlarge the command line.");
			MessageBeep(MB_ICONEXCLAMATION);

			acutPrintf( _T("\n****************************************************") );
      acutPrintf( _T("\nRoof protection bays with invalid standards:\n") );
			acutPrintf( sInvalidBays );
			acutPrintf( _T("\n****************************************************") );

			if( bContinueCancel )
			{
				if( MessageBox( NULL, sMsg, _T("Roof Protection Warning"), MB_ICONEXCLAMATION|MB_OKCANCEL )==IDCANCEL )
					return false;
			}
			else
			{
				MessageBox( NULL, sMsg, _T("Roof Protection Warning"), MB_ICONEXCLAMATION|MB_OK );
			}
		}
		else
		{
			CString sMsg;
      sMsg = _T("Warning:  The following bays do not have a 3.0m\n");
			sMsg+= _T("standard as the top standard, yet they are marked\n");
      sMsg+= _T("as requiring roof protection:\n");
			sMsg+= sInvalidBays;
			sMsg+= _T("\n\nYou should edit the standards so that top\n");
			sMsg+= _T("standard is 3.0m long!\n");
			MessageBeep(MB_ICONEXCLAMATION);

			if( bContinueCancel )
			{
				if( MessageBox( NULL, sMsg, _T("Roof Protection Warning"), MB_ICONEXCLAMATION|MB_OKCANCEL )==IDCANCEL )
					return false;
			}
			else
			{
				MessageBox( NULL, sMsg, _T("Roof Protection Warning"), MB_ICONEXCLAMATION|MB_OK );
			}
		}

		return true;
	}

	return true;
}


void Controller::SetHasBeenVisited(bool bVisited)
{
	int i;

	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		GetRun(i)->SetHasBeenVisited( bVisited );
	}
}

int Controller::GetSideOfBayFromPoint(BayList *pBays, Point3D ptPoint, intArray iaAvailableSides, SideOfBayEnum &eSideClicked)
{
	int				iSide, i, j, iBayNumber;
	Bay				*pBay;
	double			dDist, dSmallestDist;
	Point3D			pt, ptSide;
	Matrix3D		Transform;
	SideOfBayEnum	eClosestSide;

	eSideClicked = SIDE_INVALID;
	dSmallestDist = 1.5*GRAVITY_TO_STANDARD;
	iBayNumber = ID_NONE_MATCHING;
	for( i=0; i<pBays->GetSize(); i++ )
	{
		pBay = pBays->GetAt(i);
		if( pBay->GetBayType()!=BAY_TYPE_BAY )
			continue;

		Transform = pBay->GetRunPointer()->GetSchematicTransform().inverse();
		Transform = pBay->GetSchematicTransform().inverse() * Transform;
		pt = ptPoint;
		pt.transformBy( Transform );

		for( j=0; j<iaAvailableSides.GetSize(); j++ )
		{
			iSide = iaAvailableSides.GetAt(j);

			switch( iSide )
			{
			case( BP_NNE_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NE_STD];
				ptSide.y+= dSmallestDist;
				eClosestSide = NNE;
				break;
			case( BP_ENE_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NE_STD];
				ptSide.x+= dSmallestDist;
				eClosestSide = ENE;
				break;
			case( BP_ESE_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SE_STD];
				ptSide.x+= dSmallestDist;
				eClosestSide = ESE;
				break;
			case( BP_SSE_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SE_STD];
				ptSide.y-= dSmallestDist;
				eClosestSide = SSE;
				break;
			case( BP_SSW_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SW_STD];
				ptSide.y-= dSmallestDist;
				eClosestSide = SSW;
				break;
			case( BP_WSW_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SW_STD];
				ptSide.x-= dSmallestDist;
				eClosestSide = WSW;
				break;
			case( BP_WNW_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NW_STD];
				ptSide.x-= dSmallestDist;
				eClosestSide = WNW;
				break;
			case( BP_NNW_STG ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NW_STD];
				ptSide.y+= dSmallestDist;
				eClosestSide = NNW;
				break;
			case( BP_SW_STD ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SW_STD];
				eClosestSide = SW;
				break;
			case( BP_SE_STD ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_SE_STD];
				eClosestSide = SE;
				break;
			case( BP_NW_STD ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NW_STD];
				eClosestSide = NW;
				break;
			case( BP_NE_STD ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_NE_STD];
				eClosestSide = NE;
				break;
			case( BP_N_MILLS ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_N_MILLS];
				eClosestSide = N;
				break;
			case( BP_E_MILLS ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_E_MILLS];
				eClosestSide = E;
				break;
			case( BP_S_MILLS ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_S_MILLS];
				eClosestSide = S;
				break;
			case( BP_W_MILLS ):
				ptSide = pBay->GetTemplate()->m_aPoints[BP_W_MILLS];
				eClosestSide = W;
				break;
			default:
				assert(false);
			}

			dDist = pt.distanceTo(ptSide);
			if( dDist<dSmallestDist )
			{
				iBayNumber = i;
				dSmallestDist = dDist;
				eSideClicked = eClosestSide;
			}
		}
	}

	if( iBayNumber>=0 )
	{
		//They did click a valid point!  Make a noise!
		MessageBeep(MB_OK);
	}
	return iBayNumber;
}

void Controller::TraceContents()
{
	int		i;
	Bay		*pBay;
	BayList	Bays;

	if( !SelectBays( Bays, true ) )
		return;

	Bays.DisplaySelectedBays();

	for( i=0; i<Bays.GetSize(); i++ )
	{
		pBay = Bays.GetAt(i);
		pBay->TraceContents( _T("") );
	}
}

void Controller::ClearAllowDraw()
{
	int iRun;
	for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
	{
		GetRun(iRun)->ClearAllowDraw();
	}
}

#ifdef _DEBUG
void Controller::ComponentDebuggerAdd(Component *pComponent)
{
	bool bFound;
	bFound = false;
	for( int i=0; i<m_clComponentDebugger.GetSize(); i++ )
	{
		if( m_clComponentDebugger.GetAt(i)==pComponent )
		{
			assert( false );
			bFound = true;
			break;
		}
	}
	if( !bFound )
		m_clComponentDebugger.Add( pComponent );
}

void Controller::ComponentDebuggerRemove(Component *pComponent)
{
	bool bFound;
	bFound = false;
	for( int i=0; i<m_clComponentDebugger.GetSize(); i++ )
	{
		if( m_clComponentDebugger.GetAt(i)==pComponent )
		{
			m_clComponentDebugger.RemoveAt(i);
			bFound = true;
			break;
		}
	}
	if( !bFound )
		acutPrintf( _T("\nDeleting component that was not created or deleted twice! %lx"), pComponent  );
}

#endif	//#ifdef _DEBUG

void Controller::CleanUpLoadedInnerOuterPointers()
{
	//Go through each inner and outer pointer and set the
	//	pointer to the bay, rather than using the loaded
	//	value which uses the bays numbers
	int iRun, iBay;
	Run	*pRun;
	Bay	*pBay, *pBayOuter, *pBayInner;

	for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
	{
		pRun = GetRun(iRun);
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay(iBay);
			pBayOuter = pBay->GetOuter();
			pBayInner = pBay->GetInner();
			pBay->SetOuterDumb( pBayOuter );
			pBay->SetInnerDumb( pBayInner );
			//We don't need to do this since we will be visiting each bay anyhow
			//pBayOuter->SetInnerDumb( pBay );
			//pBayInner->SetOuterDumb( pBay );
		}
	}
}


Bay* Controller::InsertAndRedimensionBay(Bay *pOriginalBay, SideOfBayEnum eSide, double dNewWidth,
										 double dNewLength, double dNewWidthActual,
										 double dNewLengthActual, bool bChangeWidth/*=true*/,
										 bool bChangeLength/*=true*/)
{
	Run				*pRun;
	Bay				*pBay;
	bool			bChanged;
	JoinTypeEnum	JoinType;

	switch( eSide )
	{
	case( NORTH ):
		JoinType = JOIN_TYPE_OUTER;
		break;
	case( EAST ):
		JoinType = JOIN_TYPE_FORWARD;
		break;
	case( SOUTH ):
		JoinType = JOIN_TYPE_INNER;
		break;
	case( WEST ):
		JoinType = JOIN_TYPE_BACKWARD;
		break;
	default:
		assert( false );
		return NULL;
	}

	pRun = pOriginalBay->GetRunPointer();
	assert( pRun!=NULL );

	pBay = pRun->InsertBay( pOriginalBay, JoinType );

	bChanged = false;
	if( pBay!=NULL )
	{
		if( bChangeWidth && (pBay->GetBayWidth()<dNewWidth-ROUND_ERROR || 
							 pBay->GetBayWidth()>dNewWidth+ROUND_ERROR) )
		{
			SetWidthForBay( pBay, dNewWidth, true );
			bChanged = true;
			
			double dDist = dNewWidthActual-pBay->GetBayWidthActual();
			if( eSide==SOUTH && (dDist>ROUND_ERROR || dDist<0.00-ROUND_ERROR ) )
			{
				Vector3D	Vector;
				Matrix3D	Transform, MoveSchematic, UnMoveSchematic;

				///////////////////////////////////////////////////////////////////
				//Set back at origin
				SetHasBeenVisited( false );
				MoveSchematic	= pBay->GetRunPointer()->GetSchematicTransform();
				UnMoveSchematic = MoveSchematic;
				UnMoveSchematic.invert();
				pBay->Visit( UnMoveSchematic, true );

				///////////////////////////////////////////////////////////////////
				//Move the outer runs
				Vector.set( 0.00, dDist, 0.00 );
				Transform.setToTranslation( Vector );
				SetHasBeenVisited( false );
				if( pBay->GetOuter() )
					pBay->GetOuter()->Visit( Transform, true );

				///////////////////////////////////////////////////////////////////
				//Restore Position
				SetHasBeenVisited( false );
				pBay->Visit( MoveSchematic, true );
			}
		}
		if( bChangeLength && (pBay->GetBayLength()<dNewLength-ROUND_ERROR || 
							  pBay->GetBayLength()>dNewLength+ROUND_ERROR) )
		{
			double dDist = 0.00;
			if( pBay->GetID()==0 && eSide==WEST )
			{
				dDist = GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_LEDGER, pBay->GetBayLength(), MT_STEEL ) -
						GetCompDetails()->GetActualLength( pBay->GetSystem(), CT_LEDGER, dNewLength, MT_STEEL );
			}
			SetLengthForBay( pBay, dNewLength, true );
			if( dDist>ROUND_ERROR || dDist<0.00-ROUND_ERROR )
			{
				Vector3D Vector;
				Matrix3D	Transform, MoveSchematic, UnMoveSchematic, OriginalSch, Original;

				Vector.set( dDist, 0.00, 0.00 );
				Transform.setToTranslation( Vector );
				if( pBay->GetID()==0 && eSide==WEST )
				{
					OriginalSch = pBay->GetRunPointer()->UnMoveSchematic();
					Original = pBay->GetRunPointer()->UnMove();
				}
				SetHasBeenVisited( false );
				pBay->SetHasBeenVisited(true);
				if( pBay->GetForward()!=NULL )
					pBay->GetForward()->Visit( Transform, true );
				if( pBay->GetBackward()!=NULL && !pBay->GetBackward()->HasBeenVisited() )
					pBay->GetBackward()->Visit( Transform, true );

				///////////////////////////////////////////////////////////////////
				//Restore Position
				SetHasBeenVisited( false );

				if( pBay->GetID()==0 && eSide==WEST )
				{
					pBay->GetRunPointer()->MoveSchematic( OriginalSch, true );
					pBay->GetRunPointer()->Move( Original, true );
				}
			}
			bChanged = true;
		}
		if( bChanged )
			pBay->Redraw();
	}
	return pBay;
}

Bay *Controller::RecreateStairOrLadder(Bay *pBayToRecreate, Bay *pNeighbor, SideOfBayEnum eSideOfNeighbor )
{
	Bay		*pNewBay;
	bool	bStairs, bLadder, bButtress, bWidthChange, bLengthChange;
	double	dWidth, dWidthActual, dLength, dLengthActual;

	bStairs		= pBayToRecreate->GetBayType()==BAY_TYPE_STAIRS;
	bLadder		= pBayToRecreate->GetBayType()==BAY_TYPE_LADDER;
	bButtress	= pBayToRecreate->GetBayType()==BAY_TYPE_BUTTRESS;
	if( bStairs || bLadder || bButtress )
	{
		//Store the existing bay dimensions
		dWidth			= pBayToRecreate->GetBayWidth();
		dLength			= pBayToRecreate->GetBayLength();
		dWidthActual	= pBayToRecreate->GetBayWidthActual();
		dLengthActual	= pBayToRecreate->GetBayLengthActual();

		//Delete the existing bay
		pBayToRecreate->GetRunPointer()->DeleteBays(pBayToRecreate->GetID());
		if( pBayToRecreate->GetRunPointer()->GetNumberOfBays()<=0 )
		{
			if( !DeleteRuns( pBayToRecreate->GetRunPointer()->GetRunID(), 1, false ) )
			{
				assert( false );
			}
		}

		//Reinsert a new bay where the old one was
		bWidthChange = (dWidth<pNeighbor->GetBayWidth()-ROUND_ERROR)||(dWidth>pNeighbor->GetBayWidth()+ROUND_ERROR);
		bLengthChange = (dLength<pNeighbor->GetBayLength()-ROUND_ERROR)||(dLength>pNeighbor->GetBayLength()+ROUND_ERROR);
		pNewBay = InsertAndRedimensionBay( pNeighbor, eSideOfNeighbor, dWidth, dLength,
											dWidthActual, dLengthActual, bWidthChange, bLengthChange );

		if( pNewBay!=NULL )
		{
			if( bStairs )
			{
				//convert to stair
				pNewBay->ChangeToStairBay();
			}

			if( bLadder )
			{
				//convert to stair
				pNewBay->ChangeToLadderBay();
			}

			if( bButtress )
			{
				//convert to stair
				pNewBay->ChangeToButtressBay();
			}

			pNewBay->SetDirtyFlag( DF_CLEAN );
			pNewBay->UpdateSchematicView();
		}
	}
	else
	{
		return NULL;
	}
	return pNewBay;
}

void Controller::AddIndividualCornerFiller( Bay *pBay, SideOfBayEnum eSide, int iWidthLeft/*=-1*/, int iWidthRight/*=-1*/ )
{
	//////////////////////////////////////////////////////////////////////
	int		i;
	Bay		*pNeighbor;

	SideOfBayEnum	eSideLeft, eSideRight, eSideHopupLeft, eSideHopupRight;
	switch( eSide )
	{
	case( NORTH_EAST ):
		eSideLeft	= NORTH_NORTH_EAST;
		eSideRight	= EAST_NORTH_EAST;
		eSideHopupLeft	= N;
		eSideHopupRight = W;
		pNeighbor = pBay->GetForward();
		if( pNeighbor==NULL )
			pNeighbor = pBay->GetOuter();
		break;
	case( SOUTH_EAST ):
		eSideLeft	= EAST_SOUTH_EAST;
		eSideRight	= SOUTH_SOUTH_EAST;
		eSideHopupLeft	= E;
		eSideHopupRight = S;
		pNeighbor = pBay->GetForward();
		if( pNeighbor==NULL )
			pNeighbor = pBay->GetInner();
		break;
	case( SOUTH_WEST ):
		eSideLeft	= SOUTH_SOUTH_WEST;
		eSideRight	= WEST_SOUTH_WEST;
		eSideHopupLeft	= S;
		eSideHopupRight = W;
		pNeighbor = pBay->GetBackward();
		if( pNeighbor==NULL )
			pNeighbor = pBay->GetInner();
		break;
	case( NORTH_WEST ):
		eSideLeft	= WEST_NORTH_WEST;
		eSideRight	= NORTH_NORTH_WEST;
		eSideHopupLeft	= W;
		eSideHopupRight = N;
		pNeighbor = pBay->GetBackward();
		if( pNeighbor==NULL )
			pNeighbor = pBay->GetOuter();
		break;
	default:
		//invalid side
		assert( false );
		return;
	}

	//////////////////////////////////////////////////////////////////////
	if( pNeighbor!=NULL )
	{
		CString sMsg;
		sMsg.Format( _T("Bay%i would block the placement of a corner hopup on the\n%s side of Bay%i!\n\n"),
						pNeighbor->GetBayNumber(), GetSideOfBayDescStr( eSide ), pBay->GetBayNumber() );
		sMsg+= _T("This operation has been cancelled!");
		MessageBeep(MB_ICONSTOP);
		MessageBox( NULL, sMsg, _T("Invalid Side Error"), MB_OK|MB_ICONSTOP );
		return;
	}

	//////////////////////////////////////////////////////////////////////
	//Start at the top lift and work our way down
	Lift	*pLift;
	int		iWidthLeftTemp, iWidthRightTemp, iNumberAdded;
	bool	bDeleteHandrailHopupLeft, bDeleteHandrailHopupRight;
	iNumberAdded = 0;
	for( i=pBay->GetNumberOfLifts()-1; i>=0; i-- )
	{
		bDeleteHandrailHopupLeft	= false;
		bDeleteHandrailHopupRight	= false;
		pLift = pBay->GetLift(i);
		if( pLift->GetComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, eSide )!=NULL )
			continue;	//It has a corner stage board already!

		iWidthLeftTemp	= pLift->GetNumberOfPlanksOnStageBoard( eSideLeft );
		iWidthRightTemp	= pLift->GetNumberOfPlanksOnStageBoard( eSideRight );

		if( iWidthLeftTemp>0 && iWidthRightTemp>0 )
		{
			//we don't have a choice about the corner filler size;
			iWidthLeft = iWidthLeftTemp;
			iWidthRight = iWidthRightTemp;
			bDeleteHandrailHopupLeft	= true;
			bDeleteHandrailHopupRight	= true;
		}
		else if( iWidthLeftTemp>0 )
		{
			if( iWidthRight>0 || iWidthLeft==iWidthLeftTemp )
			{
				//Use the same as right one as last time!
				assert( iWidthRight>0 );
				iWidthRightTemp = iWidthRight;
			}
			else
			{
				//Ask the user for the Right width!
				iWidthRightTemp = iWidthLeftTemp;
				GetWidthOfCornerFillerFromCommandLine( eSideHopupRight, iWidthRightTemp, eSideHopupLeft, iWidthLeftTemp );
			}
			bDeleteHandrailHopupLeft	= true;
			pLift->AddComponent( CT_HOPUP_BRACKET, iWidthRightTemp, eSideRight, LIFT_RISE_0000MM );
			pLift->AddComponent( CT_HOPUP_BRACKET, iWidthRightTemp, eSideRight, LIFT_RISE_1000MM );
		}
		else if( iWidthRightTemp>0 )
		{
			if( iWidthLeft>0 || iWidthRight==iWidthRightTemp )
			{
				//Use the same left one as last time!
				assert( iWidthLeft>0 );
				iWidthLeftTemp = iWidthLeft;
			}
			else
			{
				//Ask the user for the Left width!
				iWidthLeftTemp = iWidthRightTemp;
				GetWidthOfCornerFillerFromCommandLine( eSideHopupLeft, iWidthLeftTemp, eSideHopupRight, iWidthRightTemp );
			}
			bDeleteHandrailHopupRight	= true;
			pLift->AddComponent( CT_HOPUP_BRACKET, iWidthLeftTemp, eSideLeft, LIFT_RISE_0000MM );
			pLift->AddComponent( CT_HOPUP_BRACKET, iWidthLeftTemp, eSideLeft, LIFT_RISE_1000MM );
		}
		else
		{
			//We don't have any stage boards, so don't bother adding corner filler
			continue;
		}

		iWidthLeft = iWidthLeftTemp;
		iWidthRight = iWidthRightTemp;
		if( iWidthLeft>0 && iWidthRight>0 )
		{
			if( pLift->AddCornerStageBoard( SideOfBayAsCorner(eSide), iWidthLeft, iWidthRight, MT_STEEL ) )
				iNumberAdded++;
			else
				assert( false );
		}

		//we may have to delete some hopups, we had to add the corner filler first
		if( bDeleteHandrailHopupLeft )
			pLift->DeleteUnneeded1000mmHopup( NULL, eSideLeft, eSideHopupLeft );
		if( bDeleteHandrailHopupRight )
			pLift->DeleteUnneeded1000mmHopup( NULL, eSideRight, eSideHopupRight );
	}

	//We couldn't add any here!
	if(	iNumberAdded<=0 )
	{
		CString sMsg;
		sMsg = _T("It was not possible to add any corner\n");
		sMsg+= _T("fillers on this side of the bay!");
    sMsg+= _T("The possible reason for this are:\n");
		sMsg+= _T("1) No adjacent Hopups in any lift, or\n");
		sMsg+= _T("2) Corner fillers already exist,\n");
		sMsg+= _T("3) Drawing corrupted!");
		MessageBeep(MB_ICONSTOP);
		MessageBox( NULL, sMsg, _T("Error none added"), MB_OK|MB_ICONSTOP );
		return;
	}
	else
	{
		switch( eSide )
		{
		case( NORTH_EAST ):
			pBay->GetTemplate()->SetNEStageCnr(true);
			break;
		case( SOUTH_EAST ):
			pBay->GetTemplate()->SetSEStageCnr(true);
			break;
		case( SOUTH_WEST ):
			pBay->GetTemplate()->SetSWStageCnr(true);
			break;
		case( NORTH_WEST ):
			pBay->GetTemplate()->SetNWStageCnr(true);
			break;
		default:
			//invalid side
			assert( false );
			return;
		}
		pBay->UpdateSchematicView();
	}
}

double Controller::GetRadiusOfSchematicStandard()
{
	return m_dRadiusOfSchematicStandard;
}

void Controller::SetRadiusOfSchematicStandard( double dRadius )
{
	m_dRadiusOfSchematicStandard = dRadius;
}

bool Controller::IsOpeningDocument()
{
	return m_bOpeningDocument;
}

void Controller::SetIsOpeningDocument(bool bOpening)
{
	m_bOpeningDocument = bOpening;
}


bool Controller::ShowTypicalMatrixSectionsOnly()
{
	return m_bShowTypicalMatrixSectionsOnly;
}

void Controller::SetShowTypicalMatrixSectionsOnly(bool bShowTypicalOnly)
{
	m_bShowTypicalMatrixSectionsOnly = bShowTypicalOnly;
}

bool Controller::GetUsePutLogClipsInLadder()
{
	return m_bUsePutLogClipsInLadder;
}

void Controller::SetUsePutLogClipsInLadder(bool bUse)
{
	m_bUsePutLogClipsInLadder = bUse;
}

void Controller::UsePutLogClipsInLadder()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nEnter new value for Using Putlog clips in Ladder access <%i>:"), GetUsePutLogClipsInLadder()? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetUsePutLogClipsInLadder()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		UsePutLogClipsInLadder();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetUsePutLogClipsInLadder( (i==0)? false: true );
		break;
	}
}




void Controller::ChangeDeleteSchematic()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we delete the schematic on load <%i>:"), (IsDeleteSchematic())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = IsDeleteSchematic()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeDeleteSchematic();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetDeleteSchematic( (i==0)? false: true );
		break;
	}
}

bool Controller::IsDeleteSchematic()
{
	int iRet;
	//Read from the registry
	iRet = GetIntInRegistry( DELETE_SCHEMATIC_FLAG, DELETE_SCHEMATIC_FLAG_DEFAULT );
	return (iRet==0)? false: true;
}

void Controller::SetDeleteSchematic(bool bDelete)
{
	int iVal;

	//Store in the registry
	iVal = bDelete? 1: 0;
	SetIntInRegistry( DELETE_SCHEMATIC_FLAG, iVal );
}

AcDbObjectId Controller::CreateText(Point3D ptPosition, Matrix3D Transform, LPCTSTR strText, CString sLayer, double dHeight, int iColour, bool bAddToDB )
{
	Entity				*pText;
	Acad::ErrorStatus	es;
	AcDbObjectId		recordId, id;
	AcDbTextStyleTable	*pTable;

	es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pTable, AcDb::kForRead);
	assert( es==Acad::eOk );

	es = pTable->getAt( TEXT_STYLE_GENERAL, recordId );
	if( es==Acad::eKeyNotFound )
	{
		es = pTable->getAt( TEXT_STYLE_STANDARD, recordId );
	}

	id.setNull();
	if( es==Acad::eOk )
	{
		pText = (Entity*)((AcDbEntity*) new AcDbText( ptPosition, strText, recordId, dHeight ) );
		pText->setDatabaseDefaults();
		pText->postToDatabase( sLayer );
		es = pText->open();
		assert( es==Acad::eOk );
		es = pText->setColorIndex( (Adesk::UInt16)iColour );
		assert( es==Acad::eOk );
		es = pText->close();
		assert( es==Acad::eOk );
	}
	pTable->close();

	return pText->objectId();
}

void Controller::ResetLabelCount()
{
	m_iSuggestedCutThroughLabel = 0;
}

void Controller::GetNextSuggestedLabel(CString &sSuggestion)
{
	ConvertNumberToString( m_iSuggestedCutThroughLabel, sSuggestion );
	m_iSuggestedCutThroughLabel++;
}

bool Controller::GetCutThroughLabelFromUser(CString &sTop, CString &sBottom, bool bSetTopLabel )
{
	int				i;
	TCHAR			keywrd[255], prompt[255];
	CString			sPrompt;

	if( bSetTopLabel )
    sPrompt.Format( _T("\nPlease Enter the Northern Label? <%s>: "), sTop );
	else
    sPrompt.Format( _T("\nPlease Enter the Southern Label? <%s>: "), sBottom );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the first point from the user.  Do not specify a base point.
	
	acedInitGet( RSG_OTHER, _T("T B"));
	int iRet = acedGetKword( prompt, keywrd);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		//This should not have occured
		MessageBeep(MB_ICONERROR);
		return false;	//Unknown error

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		return false;

	case( RTNONE ):				//fallthrough (5000) No result
		break;
	
	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		if( bSetTopLabel )
			sTop	= keywrd;
		else
			sBottom	= keywrd;
		break;
	}

	return true;
}

void Controller::Use1500Soleboards()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nUse 1500mm Soleboards where possible? <%i>:"), (GetUse1500Soleboards())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetUse1500Soleboards()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		Use1500Soleboards();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetUse1500Soleboards( (i==0)? false: true );
		break;
	}
}

bool Controller::GetUse1500Soleboards()
{
	return m_bUse1500Soleboards;
}

void Controller::SetUse1500Soleboards(bool bUse1500)
{
	m_bUse1500Soleboards = bUse1500;
}

void Controller::GetUsersName(CString &sUserName)
{
	BOOL			bResult;
	TCHAR			strUser[50];
	unsigned long	nSize =50;
	bResult = GetUserName( strUser, &nSize );

	sUserName = _T("Unknown User");
	if( bResult )
		sUserName = strUser;
}

void Controller::GetDatabaseFilename(CString &sFilename)
{
	const TCHAR* fname;
	acdbHostApplicationServices()->workingDatabase()->getFilename(fname);
	sFilename = fname;
}


void Controller::ChangeShowMatrixLabelsOnSchematic()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we display matrix section labels on schematic <%i>:"), (GetShowMatrixLabelsOnSchematic())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetShowMatrixLabelsOnSchematic()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeShowMatrixLabelsOnSchematic();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		return;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetShowMatrixLabelsOnSchematic( (i==0)? false: true );
		break;
	}
}

bool Controller::GetShowMatrixLabelsOnSchematic()
{
	return m_bShowMatrixLabelsOnSchematic;
}

void Controller::SetShowMatrixLabelsOnSchematic(bool bShow)
{
	m_bShowMatrixLabelsOnSchematic = bShow;
}

void Controller::ChangeUse1200TransomForLedger()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we substitute 1200mm Transoms for all 1200mm Ledgers <%i>:"), (GetUse1200TransomForLedger())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetUse1200TransomForLedger()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeUse1200TransomForLedger();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetUse1200TransomForLedger( (i==0)? false: true );
		break;
	}
}

void Controller::ChangeUse0700TransomForLedger()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we substitute 700mm Transoms for all 700mm Ledgers <%i>:"), (GetUse0700TransomForLedger())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetUse0700TransomForLedger()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeUse0700TransomForLedger();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetUse0700TransomForLedger( (i==0)? false: true );
		break;
	}
}

void Controller::SetUse1200TransomForLedger(bool bUseTransomForLedger)
{
	m_bUse1200TransomForLedger = bUseTransomForLedger;
}

void Controller::SetUse0700TransomForLedger(bool bUseTransomForLedger)
{
	m_bUse0700TransomForLedger = bUseTransomForLedger;
}

bool Controller::GetUse1200TransomForLedger()
{
	return m_bUse1200TransomForLedger;
}

bool Controller::GetUse0700TransomForLedger()
{
	return m_bUse0700TransomForLedger;
}

intArray * Controller::GetSelected3DBays()
{
	return &m_ia3DBaysSelected;
}

intArray * Controller::GetSelectedMatrixBays()
{
	return &m_iaMatrixBaysSelected;
}


void Controller::ChangeShowStandardLengths()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we display the lengths of individual standards in the Matrix and Kuts? <%i>:"),
						(GetShowStandardLengths())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetShowStandardLengths()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ChangeShowStandardLengths();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetShowStandardLengths( (i==0)? false: true );
		break;
	}
}

bool Controller::GetShowStandardLengths()
{
	return m_bShowStandardLengths;
}

void Controller::SetShowStandardLengths(bool bShow)
{
	m_bShowStandardLengths = bShow;
}

SystemEnum Controller::GetSystem() const
{
	assert( m_eSystem>=S_MILLS && m_eSystem<=S_OTHER );
	return m_eSystem;
}

void Controller::SetDisplayZigZagLine(bool bDisplay)
{
	m_bDisplayZigZagLine = bDisplay;
}

bool Controller::GetDisplayZigZagLine()
{
	return m_bDisplayZigZagLine;
}

void Controller::DisplayZigZagLine()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we display the Z line in the Matrix and Kuts? <%i>:"),
						(GetDisplayZigZagLine())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetDisplayZigZagLine()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		DisplayZigZagLine();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetDisplayZigZagLine( (i==0)? false: true );
		break;
	}
}

#include "ComponentEditor.h"

void Controller::ComponentsEdit()
{
	CompArray Components;

	SelectComponents(Components);

	if( Components.GetSize()>0 )
	{
		ComponentEditor	ceDialog;
		ceDialog.SetController( this );
		ceDialog.SetComponentList( &Components );
		ceDialog.DoModal();
	}
}

void Controller::SelectComponents(CompArray &Components)
{
	Component			*pComponent;
	AcDbObjectIdArray	objectIds;
	ads_name			ss;
	bool				bFound;
	int					i, j;

	Components.RemoveAll();

	//get the user selection
	if( acedSSGet( NULL, NULL, NULL, NULL, ss )!=RTNORM)
		return;

	//reduce the list only entities for lines
	selectComponents( objectIds, ss );

	//for each id
	for( i=0; i<objectIds.length(); i++ )
	{
		//obtain the Component corresponding to the line's id
		pComponent = GetComponentPointerForEntityId( objectIds[i] );
		if( pComponent!=NULL )
		{
			//Go through each Component in the list of Components
			//	to ensure it is unique
			bFound = false;
			for( j=0; j<Components.GetSize(); j++ )
			{
				if( pComponent==Components.GetAt(j) )
				{
					//it is already inserted
					bFound=true;
					break;
				}
			}
			if( !bFound )
			{
				Components.Add( pComponent );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//GetComponentPointerForEntityId(AcDbObjectId &Id)
//
Component * Controller::GetComponentPointerForEntityId(AcDbObjectId &Id)
{
	resbuf				*pRes;
	Component			*pComponent;
	AcDbEntity			*pEnt;
	Acad::ErrorStatus	es;


	es = acdbOpenAcDbEntity( pEnt, Id, AcDb::kForRead );
	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes==NULL )
		return NULL;

	pRes = pRes->rbnext;
	if(pRes->restype!=AcDb::kDxfXdInteger32 )
		return NULL;

	pComponent = (Component*)pRes->resval.rlong;

    acutRelRb(pRes);
	pEnt->close();

	return pComponent;

	/*
	Component			*pComponent;
	AcDbEntity			*pEnt;
	Acad::ErrorStatus	es, retStat;
	AcDbObjectIdArray	ids;

	pComponent = NULL;
	es = acdbOpenAcDbEntity( pEnt, Id, AcDb::kForWrite );

	AcDbVoidPtrArray *pReactors;
	void             *pSomething;

	AcDbObjectId       persObjId;
	EntityReactor     *pPersReacObj;

	pReactors = pEnt->reactors();

	if (pReactors != NULL && pReactors->length() > 0)
	{
		for (int i = 0; i < pReactors->length(); i++)
		{
			pSomething = pReactors->at(i);

			// Is it a persistent reactor?
			if (acdbIsPersistentReactor(pSomething))
			{
				persObjId = acdbPersistentReactorObjectId( pSomething);

				if((retStat=acdbOpenAcDbObject((AcDbObject*&)pPersReacObj, persObjId, AcDb::kForWrite))!=Acad::eOk)
				{
					acutPrintf("\nFailure for openAcDbObject: retStat==%d\n", retStat);
					return pComponent;
				}

				pComponent = pPersReacObj->GetComponentPointer( );
				pPersReacObj->close();
				pEnt->close();
				return pComponent;
			}
		}
	}
	pEnt->close();
	*/

	return pComponent;
}	//GetComponentPointerForEntityId(AcDbObjectId &Id)


void Controller::ComponentInsert()
{
	int			iPos;
	Component	*pComponent;
	CompArray	Components;

	//Create a new 2.4m steel transom
	pComponent = new Component( m_dLastInsertComponentLength,
								m_eLastInsertComponentType,
								m_eLastInsertComponentMaterial,
								m_eLastInsertComponentSystem );

	iPos = GetVisualComponents()->AddComponent(pComponent);
	Components.RemoveAll();
	if( iPos>=0 )
	{
		Components.Add( pComponent );
		if( pComponent->GetStockDetails()==NULL && !m_sLastInsertComponentPartNumber.IsEmpty() )
		{
			pComponent->SetStockDetailsPointerDumb( GetStockList()->GetMatchingComponent( m_sLastInsertComponentPartNumber ) );
		}
		assert( pComponent->GetStockDetails()!=NULL );

		if( Components.GetSize()>0 )
		{
			ComponentEditor	ceDialog;
			ceDialog.SetController( this );
			ceDialog.SetInsertingComponent(true);
			ceDialog.SetComponentList( &Components );

			if( ceDialog.DoModal()==IDOK )
			{	
				m_dLastInsertComponentLength		= pComponent->GetLengthCommon();
				m_eLastInsertComponentType			= pComponent->GetType();
				m_eLastInsertComponentMaterial		= pComponent->GetMaterialType();
				m_eLastInsertComponentSystem		= pComponent->GetSystem();
				m_sLastInsertComponentPartNumber	= pComponent->GetStockDetails()->GetPartNumber();

				////////////////////////////////////////////////////////////
				//Locate the component at the origin
				Point3D		pt;
				Vector3D	Vector;
				Matrix3D	Trans;

				pt.set( 0.00, 0.00, 0.00 );

				Vector.set( pt.x, pt.y, pt.z );
				Trans.setToTranslation( Vector );
				pComponent->SetTransform( Trans );

				pComponent->Delete3DView();
				pComponent->Create3DView();

				ads_name name;	// Entity names
				ads_name ourset;		// Selection set name
 
				acdbGetAdsName( name, pComponent->GetEntity()->objectId() );

				//Create a selection set that contains the last entity.
				if (acedSSAdd( name, NULL, ourset) != RTNORM)
				{ 
					assert( false );
					if( Components.GetSize()>0 )
						Components.RemoveAt(0);
					GetVisualComponents()->DeleteComponent( pComponent->GetID() );
					acdbFail(_T("Unable to create selection set\n")); 
					return; 
				} 

				ads_point point;
				int iReturn;
				iReturn = acedDragGen( ourset, NULL, 1, ModifyMatrix, point );
				if( iReturn!=RTNORM )
				{
					if( iReturn!=RTCAN )
					{
						//What happened?
						assert( false );
						acdbFail(_T("Unable to Get selection set\n")); 
					}
					if( Components.GetSize()>0 )
						Components.RemoveAt(0);
					GetVisualComponents()->DeleteComponent( pComponent->GetID() );
					return; 
				}

				struct resbuf  from, to;
				from.restype = RTSHORT;
				from.resval.rint = 1;
				to.restype = RTSHORT;
				to.resval.rint = 0;

				acedTrans( point, &from, &to, 0, point );

				Vector3D Vect;
				Vect.set( point[X], point[Y], point[Z] ); 
				Trans.setToTranslation( Vect );

				pComponent->SetTransform( Trans );
/*
				//Ensure we have a stock details info for this component
				if( pComponent->GetStockDetails()==NULL )
				{
					//try the usual method to get the stock code
					pComponent->SetStockDetailsPointer();
				}

				//if that didn't work lets go directly to the source
				if( pComponent->GetStockDetails()==NULL )
				{
					pComponent->SetStockDetailsPointerDumb( GetStockList()->GetMatchingComponent( pComponent->m_s ) );
				}
*/
				if( pComponent->GetStockDetails()==NULL )
				{
					//How the hell did this happen? they have just inserted a
					//	component that is not in the stock file!
					assert( false );
				}
				else
				{
					pComponent->Delete3DView();
					pComponent->Create3DView();
				}

				if( acedSSFree( ourset )!=RTNORM )
				{
					assert( false );
					acdbFail(_T("Unable to destroy selection set\n")); 
					return; 
				}
			}
			else
			{
				if( Components.GetSize()>0 )
					Components.RemoveAt(0);
				GetVisualComponents()->DeleteComponent( pComponent->GetID() );
			}
		}
		else
		{
			assert( false );
			if( Components.GetSize()>0 )
				Components.RemoveAt(0);
			GetVisualComponents()->DeleteComponent( pComponent->GetID() );
		}
	}
}

VisualComponents *Controller::GetVisualComponents()
{
	return &m_vcComponents;
}

int Controller::GetNumberOfVisualComponents()
{
	return GetVisualComponents()->GetNumberOfComponents();
}

bool Controller::GetBOMSummaryToShowBaySizes()
{
	return m_bBOMSummaryToShowBaySizes;
}

bool Controller::GetBOMSummaryToShowBays()
{
	return m_bBOMSummaryToShowBays;
}

void Controller::SetBOMSummaryToShowBaySizes(bool bShow)
{
	m_bBOMSummaryToShowBaySizes = bShow;
}

void Controller::SetBOMSummaryToShowBays(bool bShow)
{
	m_bBOMSummaryToShowBays = bShow;
}

void Controller::ShowBaysInBOMS()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we display the Bay numbers in the BOM Summary? <%i>:"),
						(GetBOMSummaryToShowBays())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetBOMSummaryToShowBays()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ShowBaysInBOMS();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetBOMSummaryToShowBays( (i==0)? false: true );
		break;
	}
}

void Controller::ShowBaySizesInBOMS()
{
	int				i;
	TCHAR			prompt[255];
	CString			sPrompt, sKeyWrd;

  sPrompt.Format( _T("\nShould we display the Bay sizes in the BOM Summary? <%i>:"),
						(GetBOMSummaryToShowBaySizes())? 1: 0 );

	//////////////////////////////////////////////////////////////////
	//copy from CString to char*
	for( i=0; i<sPrompt.GetLength(); i++ )
	{
		prompt[i] = sPrompt[i];
	}
	prompt[i] = _T('\0');

	//////////////////////////////////////////////////////////////////
	//get the setting from the user
	i = GetBOMSummaryToShowBaySizes()? 1: 0;
	int iRet = acedGetInt( prompt, &i);
	switch( iRet )
	{
	case( RTERROR ):			//fallthrough (-5001) Some other error
	default:
		ShowBaySizesInBOMS();
		return;

	case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
	case( RTNONE ):				//fallthrough (5000) No result
		break;

	case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
	case( RTNORM ):				//(5100) Request succeeded
		SetBOMSummaryToShowBaySizes( (i==0)? false: true );
		break;
	}
}

void Controller::SetAllComponentsVisible()
{
	int			i;
	Run			*pRun;
	LapboardBay	*pLapboard;

	///////////////////////////////////////////////////
	//Runs
	for( i=0; i<GetNumberOfRuns(); i++ )
	{
		pRun  = GetRun( i );
		assert( pRun!=NULL );
		pRun->SetAllComponentsVisible();
	}

	///////////////////////////////////////////////////
	//Lapboards
	for( i=0; i<GetNumberOfLapboards(); i++ )
	{
		pLapboard = GetLapboard( i );
		assert( pLapboard!=NULL );
		pLapboard->SetAllComponentsVisible();
	}

	///////////////////////////////////////////////////
	//Visual Components
	GetVisualComponents()->SetAllComponentsVisible();
}


bool Controller::GetConvertToSystemOnNextOpen()
{
	return m_bConvertToSystemOnNextOpen;
}

void Controller::SetConvertToSystemOnNextOpen(bool bConvert)
{
	m_bConvertToSystemOnNextOpen = bConvert;
}

void Controller::SavedHistoryShow()
{
	CString sHistory;
	for( int i=0; i<m_saSaveHistory.GetSize(); i++ )
	{
		sHistory = m_saSaveHistory.GetAt(i);
		acutPrintf(_T("\n"));
		acutPrintf(sHistory);
	}
}

void Controller::SavedHistoryClear()
{
	m_saSaveHistory.RemoveAll();
	CString sTitle;
	sTitle.Format( _T("Version, Date, Time, Who, Filename, System") );
	m_saSaveHistory.Add( sTitle );
}

void Controller::ComponentMove()
{
/*	Component	*pComponent;
	CompArray	Components;

	SelectComponents(Components);

	if( Components.GetSize()>0 )
	{
		pComponent = Components.GetAt(0);
//		pComponent->ComponentMove();

		////////////////////////////////////////////////////////////
		//Locate the component at the origin
		int			track, type;
		resbuf		result;
		Point3D		pt;
		Vector3D	Vector;
		Matrix3D	Trans;

		track = 0x0001;
		acedGrRead( track, &type, &result );
		if( type==5 )
		{
//			pt = asPnt3d(result.resval.rpoint);
*/
/*
			pt.x = pComponent->GetLastPosition().x;
			pt.y = pComponent->GetLastPosition().y;
			pt.z = pComponent->GetLastPosition().z;
*/
/*			pt.set( 0.00, 0.00, 0.00 );
			pt.transformBy( pComponent->GetTransform() );
			if( pComponent->GetLiftPointer()!=NULL )
			{
				pt.transformBy( pComponent->GetLiftPointer()->GetTransform() );
				pt.transformBy( pComponent->GetLiftPointer()->GetBayPointer()->GetTransform() );
				pt.transformBy( pComponent->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetTransform() );
			}
			if( pComponent->GetBayPointer()!=NULL )
			{
				pt.transformBy( pComponent->GetBayPointer()->GetTransform() );
				pt.transformBy( pComponent->GetBayPointer()->GetRunPointer()->GetTransform() );
			}
			pt.transformBy( Get3DCrosshairTransform() );
		}
		else
		{
			pt.set( 0.00, 0.00, 0.00 );
		}

*/
/*
		Vector.set( pt.x, pt.y, pt.z );
		Trans.setToTranslation( Vector );
		pComponent->SetTransform( Trans );
*/
/*
		pComponent->Delete3DView();
		pComponent->Create3DView();

		CString sPt;
		sPt.Format( "%i,%i,%i", (int)pt.x, (int)pt.y, (int)pt.z );
		acedCommand(RTSTR, _T("_MOVE"), RTSTR, _T("_LAST"), RTSTR, _T(""), RTSTR, _T(sPt), RTSTR, _T("\\"), RTNONE );
	}
*/
	acedCommand(RTSTR, _T("_MOVE"), RTSTR, _T(""), RTSTR, _T("\\"), RTNONE );
}

bool Controller::ComponentRotate()
{
	int			i, iRet;
	bool		bLoopInner, bLoopOuter;
	TCHAR		prompt[255], keywrd[255];
	double		dAngle;
	Point3D		pt[2], point;
	CString		sMsg, sKeyWrd;
	Point3D		CentreOfRotation;
	Vector3D	RotationAxis;
	Matrix3D	Trans, Rotate;
	Component	*pComponent;
	CompArray	Components;

	SelectComponents(Components);

	if( Components.GetSize()>0 )
	{
		bLoopOuter = true;
		do
		{
			sMsg = _T("\nSpecify 1st point on axis or define axis by [");
			if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
				sMsg+= _T("Last/");
			sMsg+= _T("Xaxis/Yaxis/Zaxix/2points] <");
			if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
        sMsg+= _T("Last>:");
			else
        sMsg+= _T("2points>:");

			if( acedInitGet( RSG_OTHER, NULL )==RTERROR )
			{
				assert( false );
			}

			iRet = GetPoint( sMsg, point );

			pt[0].set( LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER, LARGE_NEGATIVE_NUMBER );
			pt[1] = pt[0];

			switch( iRet )
			{
			default:
				return false;

			case(RTNONE):
				//Right click, just use the default position
				if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
				{
					pt[0] = m_ptLastComponentRotatePoint[0];
					pt[1] = m_ptLastComponentRotatePoint[1];
					bLoopOuter = false;
				}
				else
				{
					bLoopOuter = true;
				}
				break;

			case(RTNORM):
				//Fine just use the user point, but store it first
				bLoopOuter = false;
				pt[0] = point;
				acutPrintf( _T("\n1st Point = (%i,%i,%i)"), (int)pt[0].x, (int)pt[0].y, (int)pt[0].z );

				bLoopInner = true;
				do
				{
          sMsg = _T("\nSpecify 2nd point on axis:");
					iRet = GetPoint( sMsg, point );
					switch( iRet )
					{
					default:
						return false;

					case(RTNONE):
						//Right click, just use the default position
						bLoopInner = false;
						break;

					case(RTNORM):
						//Fine just use the user point, but store it first
						pt[1] = point;
						acutPrintf( _T("\n2nd Point = (%i,%i,%i)"),
								(int)pt[1].x, (int)pt[1].y, (int)pt[1].z );
						if( pt[0]==pt[1] )
						{
							//Same point give message and try again
							sMsg.Format( _T("These 2 points are at the same location (%i,%i,%i).\n"),
										(int)pt[0].x, (int)pt[0].y, (int)pt[0].z );
							sMsg+= _T("To form an axis you need two points forming a line!\n");
							sMsg+= _T("Please specify the second point again!");
							MessageBox( NULL, sMsg, _T("Invalid Point"), MB_OK );
							bLoopInner = true;
						}
						else
						{
							bLoopInner = false;
						}
						break;

					case(RTKWORD):
						//invalid input loop around again
						break;
					}
				} while( bLoopInner );
				break;

			case(RTKWORD):
				acedGetInput( keywrd );
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();
				if( sKeyWrd[0]==_T('L') )
				{
					bLoopOuter = false;
					//Want to use last axis
					pt[0] = m_ptLastComponentRotatePoint[0];
					pt[1] = m_ptLastComponentRotatePoint[1];
				}
				else if( sKeyWrd[0]==_T('X') || sKeyWrd[0]==_T('Y') || sKeyWrd[0]==_T('Z') )
				{
					bLoopOuter = false;
					bLoopInner = true;
					do
					{
            sMsg.Format( _T("\nSpecify a point on the %caxis: <0,0,0>"), sKeyWrd[0] );
						iRet = GetPoint( sMsg, point );
						switch( iRet )
						{
						default:
							return false;

						case(RTNONE):
							//Right click, just use the default position
							pt[0].set( 0.00, 0.00, 0.00 );
							pt[1] = pt[0];
							acutPrintf( _T("\nPoint on %caxis = 0,0,0"), sKeyWrd[0] );
							if( sKeyWrd[0]==_T('X') )
							{
								pt[1].x = 1.00;
							}
							else if( sKeyWrd[0]==_T('Y') )
							{
								pt[1].y = 1.00;
							}
							else if( sKeyWrd[0]==_T('Z') )
							{
								pt[1].z = 1.00;
							}
							else
							{
								assert( false );
							}
							bLoopInner = false;
							break;

						case(RTNORM):
							//Fine just use the user point, but store it first
							pt[0] = point;
							pt[1] = pt[0];
							acutPrintf( _T("\nPoint on %caxis = (%i,%i,%i)"), sKeyWrd[0],
										(int)pt[0].x, (int)pt[0].y, (int)pt[0].z );
							if( sKeyWrd[0]==_T('X') )
							{
								pt[1].x+= 1.00;
							}
							else if( sKeyWrd[0]==_T('Y') )
							{
								pt[1].y+= 1.00;
							}
							else if( sKeyWrd[0]==_T('Z') )
							{
								pt[1].z+= 1.00;
							}
							else
							{
								assert( false );
							}
							bLoopInner = false;
							break;

						case(RTKWORD):
							//invalid input loop around again
							break;
						}
					} while( bLoopInner );
				}
				else
				{
					//invalid input loop around again
					bLoopOuter = true;
					MessageBeep(MB_ICONASTERISK);
				}
				break;
			}
		} while( bLoopOuter );

		//We now have 2 point which form a line to rotate around, we just need an angle
		if( pt[0].x==LARGE_NEGATIVE_NUMBER && pt[0].y==LARGE_NEGATIVE_NUMBER && pt[0].z==LARGE_NEGATIVE_NUMBER )
		{
			//On of the points not set
			sMsg.Format( _T("The 1st point (%i,%i,%i) is invalid or not set.\n"),
						(int)pt[0].x, (int)pt[0].y, (int)pt[0].z );
			sMsg+= _T("To form an axis you need two points forming a line!\n");
			sMsg+= _T("The rotate component command cannot continue.");
			MessageBox( NULL, sMsg, _T("Invalid Point"), MB_OK|MB_ICONSTOP );
			bLoopInner = true;
			return false;
		}
		if( pt[1].x==LARGE_NEGATIVE_NUMBER && pt[1].y==LARGE_NEGATIVE_NUMBER && pt[1].z==LARGE_NEGATIVE_NUMBER )
		{
			//On of the points not set
			sMsg.Format( _T("The 2nd point (%i,%i,%i) is invalid or not set.\n"),
						(int)pt[1].x, (int)pt[1].y, (int)pt[1].z );
			sMsg+= _T("To form an axis you need two points forming a line!\n");
			sMsg+= _T("The rotate component command cannot continue.");
			MessageBox( NULL, sMsg, _T("Invalid Point"), MB_OK|MB_ICONSTOP );
			bLoopInner = true;
			return false;
		}
		if( pt[0]==pt[1] )
		{
			//Same point give message and try again
			sMsg.Format( _T("These 2 points are at the same location (%i,%i,%i).\n"),
						(int)pt[0].x, (int)pt[0].y, (int)pt[0].z );
			sMsg+= _T("To form an axis you need two points forming a line!\n");
			sMsg+= _T("Please specify the second point again!");
			MessageBox( NULL, sMsg, _T("Invalid Point"), MB_OK|MB_ICONSTOP );
			bLoopInner = true;
			return false;
		}

		dAngle = LARGE_NEGATIVE_NUMBER;
		bLoopInner = true;
		do
		{
			sMsg = _T("\nSpecify angle for rotation (-ve = anti-clockwise)");
			if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
			{
				CString sTemp;
				sTemp.Format( _T(" [Last] <%0.0f>"), m_dLastComponentRotationAngle );
				sMsg+= sTemp;

				if( acedInitGet( RSG_OTHER, NULL )==RTERROR )
				{
					assert( false );
				}
			}
      sMsg+= _T(":");

			//////////////////////////////////////////////////////////////////
			//copy from CString to char*
			for( i=0; i<sMsg.GetLength(); i++ )
			{
				prompt[i] = sMsg[i];
			}
			prompt[i] = _T('\0');

			//////////////////////////////////////////////////////////////////
			//get the first point from the user.  Do not specify a base point.
			iRet = acedGetReal( prompt, &dAngle );
			switch( iRet )
			{
			case( RTERROR ):			//fallthrough (-5001) Some other error
			default:
				//This should not have occured
				bLoopInner = true;
				break;

			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				MessageBeep(MB_ICONEXCLAMATION);
				return false;

			case( RTNONE ):				//fallthrough (5000) No result
				bLoopInner = true;
				if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
				{
					bLoopInner = false;
					dAngle = m_dLastComponentRotationAngle;
				}
				break;

			case( RTNORM ):				//(5100) Request succeeded
				if( dAngle>LARGE_NEGATIVE_NUMBER )
					bLoopInner = false;
				break;

			case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();

				if( _istdigit( sKeyWrd[0] ) )
				{
					dAngle = _tstof( sKeyWrd );
					bLoopInner = false;
					break;
				}
				else if( sKeyWrd[0]==_T('L') )
				{
					if( m_dLastComponentRotationAngle>LARGE_NEGATIVE_NUMBER )
					{
						bLoopInner = false;
						dAngle = m_dLastComponentRotationAngle;
					}
				}
				else
				{
					MessageBeep(MB_ICONEXCLAMATION);
					bLoopInner = true;
					break;
				}
			}
		} while( bLoopInner );

		assert( dAngle>LARGE_NEGATIVE_NUMBER );

		CentreOfRotation = pt[0];
		RotationAxis.set( pt[1].x-pt[0].x, pt[1].y-pt[0].y, pt[1].z-pt[0].z );
		RotationAxis.normalize();

		//Store this as the last rotation
		m_ptLastComponentRotatePoint[0] = pt[0];
		m_ptLastComponentRotatePoint[1] = pt[1];
		m_dLastComponentRotationAngle = dAngle;

		Bay			*pBay;
		Run			*pRun;
		Lift		*pLift;
		Matrix3D	Transform, Trans, Temp;

		dAngle = (dAngle/360.00)*(pi*2.00);

		for( i=0; i<Components.GetSize(); i++ )
		{
			pComponent = Components.GetAt(i);

			Trans.setToIdentity();
			pRun = NULL;
			pBay = NULL;
			pBay = pComponent->GetBayPointer();
			pLift = pComponent->GetLiftPointer();
			if( pLift!=NULL )
			{
				//Lift
				Temp = pLift->GetTransform();
				Temp.invert();
				Trans = Temp*Trans;
				pBay = pLift->GetBayPointer();
			}
			if( pBay!=NULL )
			{
				Temp = pBay->GetTransform();
				Temp.invert();
				Trans = Temp*Trans;
				pRun = pBay->GetRunPointer();
			}
			if( pRun!=NULL )
			{
				Temp = pRun->GetTransform();
				Temp.invert();
				Trans = Temp*Trans;
			}
			Temp = Get3DCrosshairTransform();
			Temp.invert();
			Trans = Temp*Trans;
			point = CentreOfRotation;
			point.transformBy( Trans );

			Transform.setToRotation( dAngle, RotationAxis, point );
			pComponent->Move( Transform, true );
			pComponent->Delete3DView();
			pComponent->Create3DView();
		}
	}

	return true;
}

Component *Controller::ComponentCopy()
{
	int			iPos, i;
	Vector3D	Before, After;
	Matrix3D	Trans, Movement;
	Component	*pComponent, *pNewComp;
	CompArray	Components;

	SelectComponents(Components);

	Movement.setToIdentity();

	if( Components.GetSize()>0 )
	{
		////////////////////////////////////////////////////////////
		//Locate the component at the origin
		Point3D		ptFrom, ptTo;
		Vector3D	Vect;
		Matrix3D	Trans;

		CString sPrompt;
    sPrompt = _T("Specify base point for displacement: ");
		GetPoint3D( sPrompt, ptFrom, false );

		ads_name name;	// Entity names
		ads_name ourset;		// Selection set name

		for( i=0; i<Components.GetSize(); i++ )
		{
			pComponent = Components.GetAt(i);
			acdbGetAdsName( name, pComponent->GetEntity()->objectId() );

			if( i==0 )
			{
				//Create a selection set that contains the last entity.
				if (acedSSAdd( name, NULL, ourset) != RTNORM)
				{ 
					assert( false );
					acdbFail(_T("Unable to create selection set\n")); 
					return NULL; 
				} 
			}
			else
			{
				//Create a selection set that contains the last entity.
				if (acedSSAdd( name, ourset, ourset) != RTNORM)
				{ 
					assert( false );
					acdbFail(_T("Unable to add component to selection set\n")); 
					return NULL; 
				} 
			}
		}

		ads_point point;
		gOffset[X] = -1.00*ptFrom.x;
		gOffset[Y] = -1.00*ptFrom.y;
		gOffset[Z] = -1.00*ptFrom.z;

		int iReturn;
		iReturn = acedDragGen( ourset, NULL, 0, ModifyMatrix, point );
		if( iReturn!= RTNORM )
		{
			if( iReturn!=RTCAN )
			{
				assert( false );
				acdbFail(_T("Unable to Get selection set\n")); 
			}
			gOffset[X] = 0.00;
			gOffset[Y] = 0.00;
			gOffset[Z] = 0.00;
			return NULL; 
		}

		struct resbuf  from, to;
		from.restype = RTSHORT;
		from.resval.rint = 1;
		to.restype = RTSHORT;
		to.resval.rint = 0;

		acedTrans( point, &from, &to, 0, point );

		Vect.set( point[X]-ptFrom.x, point[Y]-ptFrom.y, point[Z]-ptFrom.z ); 
		Movement.setToTranslation( Vect );

/*		sPrompt = "Specify offset point: ";
		GetPoint3D( sPrompt, ptTo, false );

		Vect.set( ptTo.x-ptFrom.x, ptTo.y-ptFrom.y, ptTo.z-ptFrom.z );
		Movement.setToTranslation( Vect );
*/
		for( i=0; i<Components.GetSize(); i++ )
		{
			pComponent = Components.GetAt(i);
			pNewComp = new Component( pComponent->GetLengthCommon(), pComponent->GetType(), pComponent->GetMaterialType(), pComponent->GetSystem() );
			iPos = gpController->GetVisualComponents()->AddComponent( pNewComp );
			Trans = pComponent->GetEntireTransform();
			pNewComp->SetTransform( Movement*Trans );
			pNewComp->Delete3DView();
			pNewComp->Create3DView();
		}
		gOffset[X] = 0.00;
		gOffset[Y] = 0.00;
		gOffset[Z] = 0.00;
	}

	return pNewComp;
}

intArray Controller::GetVisitedBays()
{
	int			iRun, iBay, iBayNumber;
	Bay			*pBay;
	Run			*pRun;
	intArray	iaBaysVisited;

	iaBaysVisited.RemoveAll();

	for( iRun=0; iRun<GetNumberOfRuns(); iRun++ )
	{
		pRun = GetRun(iRun);
		for( iBay=0; iBay<pRun->GetNumberOfBays(); iBay++ )
		{
			pBay = pRun->GetBay(iBay);
			if( pBay->HasBeenVisited() )
			{
				iBayNumber = pBay->GetBayNumber();
				iaBaysVisited.Add( iBayNumber );
			}
		}
	}

	return iaBaysVisited;
}


double Controller::GetStarSeperation()
{
	return STAR_SEPARATION;
}

int Controller::GetNumberOfStages()
{
	return m_StageNames.GetNumberOfStages();
}

CString Controller::GetStage( int iIndex )
{
	CString sStage;
	sStage = STAGE_DEFAULT_VALUE;
	if( iIndex>=0 && iIndex<GetNumberOfStages() )
	{
		sStage = m_StageNames.GetStage(iIndex);
	}
	else
	{
		assert( false );
	}
	return sStage;
}

int Controller::AddStage(CString sStage)
{
	return m_StageNames.AddStage( sStage );
}

void Controller::ClearStages()
{
	m_StageNames.DeleteAll();
}

bool Controller::IsStageLevelVisible(CString sStage, int iLevel)
{
	return m_StageNames.IsVisible( sStage, iLevel );
}

void Controller::SetStageLevelVisible(CString sStage, int iLevel, bool bVisible)
{
	m_StageNames.SetVisible( sStage, iLevel, bVisible );
}


void Controller::SetInsertingBlock(bool bInserting, bool bResetValues/*=false*/)
{
	Run	*pRun;
	Bay	*pBay;

	m_bInsertingBlock = bInserting;
	if( bResetValues )
	{
		m_iVisualCompsB4Insert = GetNumberOfVisualComponents();
		m_iBOMExtraB4Insert = m_BOMExtraCompList.GetNumberOfComponents();
		m_iLapboardsB4Insert = GetNumberOfLapboards();
		m_iRunsB4Insert = GetNumberOfRuns();
		pRun = GetRun(m_iRunsB4Insert-1);
		m_iBaysB4Insert = 0;
		if( pRun!=NULL )
		{
			pBay = pRun->GetBay(pRun->GetNumberOfBays()-1);
			if( pBay!=NULL )
				m_iBaysB4Insert = pBay->GetBayNumber();
		}

	}
}

bool Controller::IsInsertingBlock()
{
	return m_bInsertingBlock;
}


int Controller::GetRunsB4Insert()
{
	return m_iRunsB4Insert;
}

int Controller::GetLapsB4Insert()
{
	return m_iLapboardsB4Insert;
}

int Controller::GetBaysB4Insert()
{
	return m_iBaysB4Insert;
}

int Controller::GetVisualCompsB4Insert()
{
	return m_iVisualCompsB4Insert;
}

int Controller::GetBOMExtraB4Insert()
{
	return m_iBOMExtraB4Insert;
}

int Controller::RunsSinceInsert()
{
	return GetNumberOfRuns()-GetRunsB4Insert();
}

int Controller::LapsSinceInsert()
{
	return GetNumberOfLapboards()-GetLapsB4Insert();
}

int Controller::BaysSinceInsert()
{
	Run	*pRun;
	Bay	*pBay;
	int iBayNumber;
	pRun = GetRun(GetNumberOfRuns()-1);
	if( pRun!=NULL )
	{
		pBay = pRun->GetBay(pRun->GetNumberOfBays()-1);
		if( pBay!=NULL )
			iBayNumber = pBay->GetBayNumber();
	}
	return iBayNumber-GetBaysB4Insert();
}

int Controller::VisualCompsSinceInsert()
{
	return GetNumberOfVisualComponents()-GetVisualCompsB4Insert();
}

int Controller::BOMExtraSinceInsert()
{
	return m_BOMExtraCompList.GetNumberOfComponents()-GetBOMExtraB4Insert();
}

void Controller::CopyStageLevelVisible( StageLevelVisible &New )
{
	New = m_StageNames;
}

void Controller::SetStageLevelVisible(StageLevelVisible &Original)
{
	m_StageNames = Original;
}

void Controller::SetBlockInserting(bool bInserting)
{
	m_bKwikScafBlockInserted = bInserting;
}

bool Controller::IsBlockInserting()
{
	return m_bKwikScafBlockInserted;
}

Point3D Controller::GetLowerLeftOfSchematic()
{
	int		iRunID, iBayID, iVC;
	Bay		*pBay;
	Run		*pRun;
	double	dX, dY;
	Point3D	pt, ptTemp;
	dX = LARGE_NUMBER;
	dY = LARGE_NUMBER;

	//Check the limits of the runs
	for( iRunID=0; iRunID<GetNumberOfRuns(); iRunID++ )
	{
		pRun = GetRun(iRunID);
		for( iBayID=0; iBayID<pRun->GetNumberOfBays(); iBayID++ )
		{
			pBay = pRun->GetBay( iBayID );
			ptTemp.set( 0.00, 0.00, 0.00 );
			ptTemp.transformBy( pBay->GetTransform() );
			ptTemp.transformBy( pRun->GetTransform() );
			dX = min( dX, ptTemp.x );
			dY = min( dY, ptTemp.y );
		}
	}

	//Visual Components
	Component *pComp;
	for( iVC=0; iVC<GetNumberOfVisualComponents(); iVC++ )
	{
		pComp = GetVisualComponents()->GetComponent( iVC );
		ptTemp.set( 0.00, 0.00, 0.00 );
		ptTemp.transformBy( pComp->GetTransform() );
		dX = min( dX, ptTemp.x );
		dY = min( dY, ptTemp.y );
	}

	pt.set( dX, dY, 0.00 );
	return pt;
}

int ModifyMatrix(ads_point usrpt, ads_matrix matrix )
{ 
	for( int i=0; i<4; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			matrix[i][j] = 0.00;
			if( i==j )
			{
				matrix[i][j] = 1.00;
			}
		}
	}

	struct resbuf  from, to;
	from.restype = RTSHORT;
	from.resval.rint = 1;
	to.restype = RTSHORT;
	to.resval.rint = 0;

	acedTrans( usrpt, &from, &to, 0, usrpt );

// Initialize translation vector.
    matrix[0][T] = usrpt[X]+gOffset[X]; 
    matrix[1][T] = usrpt[Y]+gOffset[Y]; 
    matrix[2][T] = usrpt[Z]+gOffset[Z]; 

	
 
    return RTNORM; // Matrix was modified.

} 

void Controller::EditStock()
{
	GetStockList()->EditStockElement();
}

void Controller::SetCalcPerRollShadeCloth(bool bPerRoll)
{
	m_bCalcPerRollShadeCloth = bPerRoll;
}

void Controller::SetCalcPerRollChainMesh(bool bPerRoll)
{
	m_bCalcPerRollChainMesh = bPerRoll;
}

bool Controller::CalcPerRollShadeCloth()
{
	return m_bCalcPerRollShadeCloth;
}

bool Controller::CalcPerRollChainMesh()
{
	return m_bCalcPerRollChainMesh;
}

void Controller::CalcPerRoll()
{
	PerRollDialog	Dlg(this);

	Dlg.SetShadeCloth(true);
	if( Dlg.DoModal()==IDOK )
	{
		StorePerRollInRegistry();
		Dlg.SetShadeCloth(false);
		if( Dlg.DoModal()==IDOK )
		{
			StorePerRollInRegistry();
		}
	}
}

void Controller::SetRollSizeChainMesh(double dArea)
{
	m_dRollSizeChainMesh = dArea;
}

void Controller::SetRollSizeShadeCloth(double dArea)
{
	m_dRollSizeShadeCloth = dArea;
}

void Controller::SetPartNumberChainMesh(CString sPartNumber)
{
	m_sPartNumberChainMesh = sPartNumber;
}

void Controller::SetPartNumberShadeCloth(CString sPartNumber)
{
	m_sPartNumberShadeCloth = sPartNumber;
}

double Controller::GetRollSizeChainMesh()
{
	return m_dRollSizeChainMesh;
}

double Controller::GetRollSizeShadeCloth()
{
	return m_dRollSizeShadeCloth;
}

CString Controller::GetPartNumberChainMesh()
{
	return m_sPartNumberChainMesh;
}

CString Controller::GetPartNumberShadeCloth()
{
	return m_sPartNumberShadeCloth;
}

void Controller::StorePerRollInRegistry()
{
	CString sSectionNameChain, sSectionNameShade;

	sSectionNameShade = _T("PerRoll_Shade_");
	sSectionNameChain = _T("PerRoll_Chain_");

	::SetIntInRegistry( sSectionNameShade+_T("CalcType"), CalcPerRollShadeCloth()? 1: 0 );
	::SetIntInRegistry( sSectionNameChain+_T("CalcType"), CalcPerRollChainMesh()? 1: 0 );

	::SetIntInRegistry( sSectionNameShade+_T("RollSize"), (int)(GetRollSizeShadeCloth()*100) );
	::SetIntInRegistry( sSectionNameChain+_T("RollSize"), (int)(GetRollSizeChainMesh()*100) );

	::SetStringInRegistry( sSectionNameShade+_T("PartNumber"), GetPartNumberShadeCloth() );
	::SetStringInRegistry( sSectionNameChain+_T("PartNumber"), GetPartNumberChainMesh() );

	::SetIntInRegistry( sSectionNameShade+_T("RoundingDevisor"), (int)GetRoundingDivisorShadeCloth() );
	::SetIntInRegistry( sSectionNameChain+_T("RoundingDevisor"), (int)GetRoundingDivisorChainMesh() );
}

void Controller::ReadPerRollFromRegistry()
{
	CString sSectionNameChain, sSectionNameShade;

	sSectionNameShade = _T("PerRoll_Shade_");
	sSectionNameChain = _T("PerRoll_Chain_");

	SetCalcPerRollShadeCloth(	(::GetIntInRegistry( sSectionNameShade+_T("CalcType"), 0 )==0)? false: true );
	SetCalcPerRollChainMesh(	(::GetIntInRegistry( sSectionNameChain+_T("CalcType"), 0 )==0)? false: true );

	SetRollSizeShadeCloth(	((double)::GetIntInRegistry( sSectionNameShade+_T("RollSize"), 0 ))/100.00);
	SetRollSizeChainMesh(	((double)::GetIntInRegistry( sSectionNameChain+_T("RollSize"), 0 ))/100.00);

	SetPartNumberShadeCloth(::GetStringInRegistry( sSectionNameShade+_T("PartNumber") ));
	SetPartNumberChainMesh(	::GetStringInRegistry( sSectionNameChain+_T("PartNumber") ));

	SetRoundingDivisorShadeCloth(	(RoundingDevisorEnum)::GetIntInRegistry( sSectionNameShade+_T("RoundingDevisor") ));
	SetRoundingDivisorChainMesh(	(RoundingDevisorEnum)::GetIntInRegistry( sSectionNameChain+_T("RoundingDevisor") ));

}

RoundingDevisorEnum Controller::ConvertToDevisor(RoundingListOrderEnum eIndex)
{
	RoundingDevisorEnum eRD;
	switch( eIndex )
	{
	default:
		assert( false );
		//fallthrough
	case( RLO_WHOLE ):
		eRD = RD_WHOLE;
		break;

	case( RLO_HALF ):
		eRD = RD_HALF;
		break;

	case( RLO_QUATER ):
		eRD = RD_QUATER;
		break;

	case( RLO_FIFTH ):
		eRD = RD_FIFTH;
		break;

	case( RLO_TENTH ):
		eRD = RD_TENTH;
		break;

	case( RLO_HUNDREDTH ):
		eRD = RD_HUNDREDTH;
		break;
	}
	return eRD;
}

RoundingListOrderEnum Controller::ConvertToListOrder(RoundingDevisorEnum eDevisor)
{
	RoundingListOrderEnum eRLO;
	switch( eDevisor )
	{
	default:
		assert( false );
		//fallthrough
	case( RD_WHOLE ):
		eRLO = RLO_WHOLE;
		break;

	case( RD_HALF ):
		eRLO = RLO_HALF;
		break;

	case( RD_QUATER ):
		eRLO = RLO_QUATER;
		break;

	case( RD_FIFTH ):
		eRLO = RLO_FIFTH;
		break;

	case( RD_TENTH ):
		eRLO = RLO_TENTH;
		break;

	case( RD_HUNDREDTH ):
		eRLO = RLO_HUNDREDTH;
		break;
	}
	return eRLO;
}

void Controller::SetRoundingDivisorShadeCloth(RoundingDevisorEnum eRD )
{
	m_eRDShadeCloth = eRD;
}

RoundingDevisorEnum Controller::GetRoundingDivisorShadeCloth()
{
	return 	m_eRDShadeCloth;
}

void Controller::SetRoundingDivisorChainMesh(RoundingDevisorEnum eRD )
{
	m_eRDChainMesh = eRD;
}

RoundingDevisorEnum Controller::GetRoundingDivisorChainMesh()
{
	return 	m_eRDChainMesh;
}


bool Controller::SaveSelectedOnly()
{
	return m_bSaveSelected;
}

void Controller::SetSaveSelected(bool bSaveSelected)
{
	m_bSaveSelected = bSaveSelected;
}


void Controller::UpdateAllVeiws()
{
	if( IsCrosshairCreated() )
	{
		Delete3DView();
		Create3DView( true );
	}
	if( GetMatrix()!=NULL )
	{
		DeleteMatrix();
		CreateMatrix( true );
	}
	RedrawSchematic( false );
	RegenMatrixLevels( false );
}

bool Controller::AreObjectIDsSelected(AcDbObjectIdArray &objIDs)
{
	int	i, j;
	AcDbObjectId	iID, jID;
	for( i=0; i<objIDs.length(); i++ )
	{
		iID = objIDs.at(i);
		for( j=0; j<m_objIDSaveSelection.length(); j++ )
		{
			jID = m_objIDSaveSelection.at(j);
			if( jID==iID )
				return true;
		}
	}
	return false;
}

void Controller::SetRLs()
{
	int					iRet, ikrlIndex, jkrlIndex;
	bool				bPossible;
	double				dValue;
	Point3D				point;
	CString				sMsg;

	SetRLCalcPossible( false );
	for( ikrlIndex=(int)KRL_1ST; ikrlIndex<(int)KRL_SIZE; ikrlIndex++ )
	{
		bPossible = true;
		switch( ikrlIndex )
		{
		case( KRL_1ST ):
			sMsg = _T("\nPlease provide 3 points with RLs for each\nWhere is the 1st point?");
			break;
		case( KRL_2ND ):
			sMsg = _T("\nWhere is the 2nd point?");
			break;
		case( KRL_3RD ):
			sMsg = _T("\nWhere is the 3rd point?");
			break;
		default:
			assert( false );
		}

		iRet = GetPoint( sMsg, point );
		switch( iRet )
		{
		default:
		case(RTNONE):
			//Right click, just use the default position
			break;
		case(RTNORM):				//(5100) Request succeeded
			m_ptKwikRLs[ikrlIndex] = point;	

			for( jkrlIndex=ikrlIndex-1; jkrlIndex>=(int)KRL_1ST; jkrlIndex-- )
			{
				if( m_ptKwikRLs[jkrlIndex].x==m_ptKwikRLs[ikrlIndex].x &&
					m_ptKwikRLs[jkrlIndex].y==m_ptKwikRLs[ikrlIndex].y )
				{
					bPossible = false;
				}
			}
			break;

		case(RTKWORD):
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}

		sMsg = _T("\nWhat is the RL at this point?");
		iRet = acedGetReal( sMsg, &dValue);
		switch( iRet )
		{
		case( RTERROR ):			//fallthrough (-5001) Some other error
		case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
		case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
		case( RTNONE ):				//fallthrough (5000) No result
			return;
		case( RTNORM ):				//(5100) Request succeeded
			m_ptKwikRLs[ikrlIndex].z = dValue;
			break;
		}

		if( bPossible && ikrlIndex==(int)KRL_3RD )
			SetRLCalcPossible( true );
	}

#ifdef _DEBUG
	Point3D ptOrigin;
  acutPrintf( _T("\nOringal 1st Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_1ST].x, (int)m_ptKwikRLs[KRL_1ST].y, (int)m_ptKwikRLs[KRL_1ST].z ); 
  acutPrintf( _T("\nCalcltd 1st Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_1ST].x, (int)m_ptKwikRLs[KRL_1ST].y, (int)GetRLAtPoint(m_ptKwikRLs[KRL_1ST]) ); 
  acutPrintf( _T("\nOringal Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_2ND].x, (int)m_ptKwikRLs[KRL_2ND].y, (int)m_ptKwikRLs[KRL_2ND].z ); 
  acutPrintf( _T("\nCalcltd Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_2ND].x, (int)m_ptKwikRLs[KRL_2ND].y, (int)GetRLAtPoint(m_ptKwikRLs[KRL_2ND]) ); 
  acutPrintf( _T("\nOringal Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_3RD].x, (int)m_ptKwikRLs[KRL_3RD].y, (int)m_ptKwikRLs[KRL_3RD].z ); 
  acutPrintf( _T("\nCalcltd Point: %i, %i, %i"), (int)m_ptKwikRLs[KRL_3RD].x, (int)m_ptKwikRLs[KRL_3RD].y, (int)GetRLAtPoint(m_ptKwikRLs[KRL_3RD]) ); 
	ptOrigin.x = 0.00;
	ptOrigin.y = 0.00;
	ptOrigin.z = 0.00;
  acutPrintf( _T("\nOrigin Point: %i, %i, %i"), (int)ptOrigin.x, (int)ptOrigin.y, (int)GetRLAtPoint(ptOrigin) ); 
#endif //_DEBUG
}

double Controller::GetRLAtPoint(Point3D pt)
{
	if( !IsRLCalcPossible() )
		return -1.00;

	double		D;
	Point3D		A, B, C;
	Vector3D	Vector;

	///////////////////////////////////////////////////////////////////
	//ptRLs forms a plane, provided that two of the points aren't the same
	if( (m_ptKwikRLs[KRL_1ST].distanceTo( m_ptKwikRLs[KRL_2ND] ) < 0.001) ||
		(m_ptKwikRLs[KRL_2ND].distanceTo( m_ptKwikRLs[KRL_3RD] ) < 0.001) ||
		(m_ptKwikRLs[KRL_3RD].distanceTo( m_ptKwikRLs[KRL_1ST] ) < 0.001) )
		return -1.00;

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
	A = m_ptKwikRLs[KRL_1ST];
	B = m_ptKwikRLs[KRL_2ND];
	C = m_ptKwikRLs[KRL_3RD];
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

	return pt.z;
}

double Controller::GetRLAtPoint(double x, double y)
{
	Point3D point;
	point.x = x;
	point.y = y;
	point.z = -1.00;
	return GetRLAtPoint( point );
}

bool Controller::IsRLCalcPossible()
{
	return m_bRLCalcPossible;
}

void Controller::SetRLCalcPossible(bool bPossible)
{
	m_bRLCalcPossible = bPossible;
}

void Controller::ShowRLforPoint()
{
	int		iRet;
	Point3D point;
	CString sMsg;

	while( !IsRLCalcPossible() )
	{
		sMsg = _T("Before I can calculate an RL, I need to you to tell me\n");
		sMsg+= _T("the RL of three seperate points for which the RL is know!\n\n");
		sMsg+= _T("These three points will form a plane which will be used\n");
		sMsg+= _T("to determine the RL at any other point on that plane.\n");
		MessageBeep(MB_ICONSTOP);
		if( MessageBox( NULL, sMsg, _T("Plane Data Missing"), MB_ICONSTOP|MB_OKCANCEL )==IDCANCEL )
		{
			return;
		}
		SetRLs();
	}

	assert( IsRLCalcPossible() );

	do
	{
		sMsg = _T("\nWhat point would you like to know the RL of?");
		iRet = GetPoint( sMsg, point );
		switch( iRet )
		{
		default:
		case(RTNONE):
			//Right click, just use the default position
			return;
		case(RTNORM):				//(5100) Request succeeded
			acutPrintf( _T("\nRL = %imm at point %i,%i"), (int)GetRLAtPoint( point ), (int)point.x, (int)point.y ); 
			break;

		case(RTKWORD):
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
	}
	while( true );	//loop continuously until they cancel command
}

void Controller::ClearActualComponents()
{
	GetStockList()->ClearActualComponents();
}


#include "Kwik3DPolygonMesh.h"

void Controller::DrawKwikRLs()
{
	int					ikrlIndex;
	Entity				*pEnt;
	Point3D				point;
	Point3DArray		points;
	Kwik3DPolygonMesh	*pMesh;
	
	for( ikrlIndex=(int)KRL_1ST; ikrlIndex<(int)KRL_SIZE; ikrlIndex++ )
	{
		point = m_ptKwikRLs[ikrlIndex];
		point.z-= GetStarSeperation();
		points.append( point );
	}
	points.append( point );

	pMesh = new Kwik3DPolygonMesh(AcDb::kSimpleMesh, 2, 2, points, Adesk::kFalse, Adesk::kFalse );
	pEnt = (Entity*)pMesh;
	pEnt->postToDatabase( _T("0") );
}
