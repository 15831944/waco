// LapboardBay.cpp: implementation of the LapboardBay class.
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
#include "meccano.h"
#include "LapboardBay.h"

#include "controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern Controller *gpController;

//#define EXACT_NUMBER_OF_LAPS

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LapboardBay::LapboardBay()
{
	SetBayType( BAY_TYPE_LAPBOARD );
	SetRunPointer(NULL);
	SetController(NULL);
	SetBayEast(NULL);
	SetBayWest(NULL);
	GetTemplate()->SetBayPointer( (Bay*)this );
}

LapboardBay::~LapboardBay()
{
	//This should call the Bay destructor
}

/////////////////////////////////////////////////////////////
//
Run *LapboardBay::GetRunPointer()
{
	//This is a lapboard not a bay!
	;//assert( false );	
	return NULL;
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetBackward(Bay *pBay)
{
	pBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetForward(Bay *pBay)
{
	pBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetOuter(Bay *pBay)
{
	pBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetInner(Bay *pBay)
{
	pBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetCornerEast( LapboardBay *pLapBay )
{
	pLapBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
void LapboardBay::SetCornerWest( LapboardBay *pLapBay )
{
	pLapBay;
	//This is a lapboard not a bay!
	;//assert( false );
}

/////////////////////////////////////////////////////////////
//
Bay			*LapboardBay::GetBackward() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}

/////////////////////////////////////////////////////////////
//
Bay			*LapboardBay::GetForward() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}

/////////////////////////////////////////////////////////////
//
Bay			*LapboardBay::GetOuter() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}

/////////////////////////////////////////////////////////////
//
Bay			*LapboardBay::GetInner() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}

/////////////////////////////////////////////////////////////
//
LapboardBay	*LapboardBay::GetCornerEast() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}

/////////////////////////////////////////////////////////////
//
LapboardBay	*LapboardBay::GetCornerWest() const
{
	//This is a lapboard not a bay!
	;//assert( false );
	return NULL;
}



void LapboardBay::SetBayEast(Bay *pBay)
{
	m_BayEast = pBay;
}

void LapboardBay::SetBayWest(Bay *pBay)
{
	m_BayWest = pBay;
}

Bay * LapboardBay::GetBayWest() const
{
	return m_BayWest;
}

Bay * LapboardBay::GetBayEast() const
{
	return m_BayEast;
}

LapBoardTemplate * LapboardBay::GetTemplate()
{
	return &m_LBTplt;
}


//////////////////////////////////////////////////////////////////////////
//LapboardList()
LapboardList::LapboardList()
{
	DeleteAllLapboards();
}

//////////////////////////////////////////////////////////////////////////
//~LapboardList()
LapboardList::~LapboardList()
{
	LapboardBay	*pLBB;

	while( GetSize()>0 )
	{
		pLBB=NULL;
		pLBB = GetAt(0);
		if( pLBB!=NULL )
		{
			delete pLBB;
		}
		else
		{
			;//assert( false );
		}

		RemoveAt(0);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//retrieves the number of lapboards owned by the controller
int LapboardList::GetNumberOfLapboards() const
{
	return GetSize();
}

//////////////////////////////////////////////////////////////////////////
//
//controls the initial user interface such that the appropriate function below
//	is called.
int	LapboardList::AddNewLapboard( LapboardBay *pLapboard )
{
	int		iPos;
	;//assert( pLapboard!=NULL );
	iPos = Add( pLapboard );
	pLapboard->SetID( iPos );

	return iPos;
}

//////////////////////////////////////////////////////////////////////////
//
bool LapboardList::DeleteLapboard( int iLapboardID, int iCount/*=1*/ )
{
	int			i;
	LapboardBay	*pLapboard;

	//This is fine if there are no lapboards!
	if( iCount==0 && GetNumberOfLapboards()==0 )
		return false;

	if( iLapboardID<0 || iCount<1 || iLapboardID+iCount>GetNumberOfLapboards() )
	{
		;//assert( false );
		return false;
	}

	Bay	*pBay;
	for( i=iLapboardID; i<iLapboardID+iCount; i++ )
	{
		pLapboard = NULL;
		pLapboard = GetLapboard( i );
		if( pLapboard!=NULL )
		{
			pBay = pLapboard->GetBayEast();
			if( pBay!=NULL )
			{
				pBay->RemoveLapboardPointer( pLapboard );
			}

			pBay = pLapboard->GetBayWest();
			if( pBay!=NULL )
			{
				pBay->RemoveLapboardPointer( pLapboard );
			}

			delete pLapboard;
		}
		else
		{
			;//assert( false );
		}
	}
	RemoveAt( iLapboardID, iCount );

	ReIDAllLapboards( iLapboardID ); 

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
void LapboardList::DeleteAllLapboards()
{
	DeleteLapboard( 0, GetNumberOfLapboards() );
}

//////////////////////////////////////////////////////////////////////////
//ReIDAllLapboards
void LapboardList::ReIDAllLapboards( int iStartID/*=0*/ )
{
	LapboardBay *pLapboard;
	int i, iSize;

	iSize = GetNumberOfLapboards();
	for( i=iStartID; i<iSize; i++ )
	{
		pLapboard = GetLapboard( i );
		pLapboard->SetID(i);
	}
}


LapboardBay *LapboardList::CreateNewLapboard()
{
	int			iPos;
	LapboardBay	*pLapboard;

	pLapboard = new LapboardBay;
	iPos = AddNewLapboard( pLapboard );
	pLapboard->SetID( iPos );
	pLapboard->PositionLapboardStandards();
	return pLapboard;
}

LapboardBay *LapboardList::GetLapboard( int iLapboardID )
{
	if( iLapboardID<0 )
		return NULL;
	if( iLapboardID>=GetNumberOfLapboards() )
		return NULL;

	return GetAt( iLapboardID );
}

void LapboardBay::CreateSchematicPoints()
{
	GetTemplate()->CreatePoints();
}

void LapboardBay::MoveSchematic(Matrix3D Transform, bool bStore)
{
	;//assert( GetTemplate()!=NULL );
	GetTemplate()->MoveSchematic( Transform, bStore );
}

void LapboardBay::CreateSchematic()
{
	GetTemplate()->CreateSchematic();
}

void LapboardBay::SetBayPointers(Bay *pBayWest, Bay *pBayEast, SideOfBayEnum eSideWest, SideOfBayEnum eSideEast, bool bShowWarning/*=true*/ )
{
	int				i, j, iSize, jSize;
	bool			bDraw;
	double			dRLWest, dRLEast;
	SideOfBayEnum	eSideLeft, eSideRight;
	CornerOfBayEnum	stdLeft, stdRight;

	//we must have a bay to copy from
	;//assert( pBayWest!=NULL );
	bDraw = pBayWest->IsDrawAllowed();
	if( bDraw )
		pBayWest->SetAllowDraw( false );

	eSideLeft	= SIDE_INVALID;
	eSideRight	= SIDE_INVALID;
	stdLeft		= CNR_INVALID;
	stdRight	= CNR_INVALID;
	switch( eSideWest )
	{
	case( N ):
		eSideLeft	= WNW;
		eSideRight	= ENE;
		stdLeft		= CNR_NORTH_WEST;
		stdRight	= CNR_NORTH_EAST;
		pBayWest->GetTemplate()->SetNHandRail( false );
		pBayWest->GetTemplate()->SetNBrace( false );
		break;
	case( E ):
		eSideLeft	= NNE;
		eSideRight	= SSE;
		stdLeft		= CNR_NORTH_EAST;
		stdRight	= CNR_SOUTH_EAST;
		pBayWest->GetTemplate()->SetEHandRail( false );
		pBayWest->GetTemplate()->SetEBrace( false );
		break;
	case( S ):
		eSideLeft	= ESE;
		eSideRight	= WSW;
		stdLeft		= CNR_SOUTH_EAST;
		stdRight	= CNR_SOUTH_WEST;
		pBayWest->GetTemplate()->SetSHandRail( false );
		pBayWest->GetTemplate()->SetSBrace( false );
		break;
	case( W ):
		eSideLeft	= SSW;
		eSideRight	= NNW;
		stdLeft		= CNR_SOUTH_WEST;
		stdRight	= CNR_SOUTH_WEST;
		pBayWest->GetTemplate()->SetWHandRail( false );
		pBayWest->GetTemplate()->SetWBrace( false );
		break;
	default:
		;//assert( false );
	}

	//Is the angle between the runs in the 3rd or 4th quadrant
	if( eSideWest!=SIDE_INVALID )
	{
		pBayWest->AddLapboardPointer( this, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_BRACING, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, eSideWest );
		pBayWest->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSideWest );
		if( eSideWest==NORTH || eSideWest==SOUTH )
		{
			pBayWest->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSideWest );
			//remove roof protection stuff
			pBayWest->DeleteAllComponentsOfTypeFromSide( CT_LEDGER, eSideWest, LIFT_RISE_1500MM );
			pBayWest->DeleteAllComponentsOfTypeFromSide( CT_LEDGER, eSideWest, LIFT_RISE_2000MM );
		}
		else
		{
			//remove roof protection stuff
			pBayWest->DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, eSideWest, LIFT_RISE_1500MM );
			pBayWest->DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, eSideWest, LIFT_RISE_2000MM );
		}
		pBayWest->Delete1000mmHopupRailFromSide( eSideLeft );
		pBayWest->Delete1000mmHopupRailFromSide( eSideRight );
		if( m_bAllowRecreateStandards )
		{
			pBayWest->MakeAStandardFitLifts( stdLeft );
			pBayWest->MakeAStandardFitLifts( stdRight );
		}
		if( bDraw )
			pBayWest->SetAllowDraw( true );
		pBayWest->UpdateSchematicView();
	}

	//set the pointers
	SetBayWest( pBayWest );

	if( pBayEast!=NULL )
	{
		bDraw = pBayEast->IsDrawAllowed();
		if( bDraw )
			pBayEast->SetAllowDraw( false );

		eSideLeft	= SIDE_INVALID;
		eSideRight	= SIDE_INVALID;
		stdLeft		= CNR_INVALID;
		stdRight	= CNR_INVALID;
		switch( eSideEast )
		{
		case( N ):
			eSideLeft	= WNW;
			eSideRight	= ENE;
			stdLeft		= CNR_NORTH_WEST;
			stdRight	= CNR_NORTH_EAST;
			pBayEast->GetTemplate()->SetNHandRail( false );
			pBayEast->GetTemplate()->SetNBrace( false );
			break;
		case( E ):
			eSideLeft	= NNE;
			eSideRight	= SSE;
			stdLeft		= CNR_NORTH_EAST;
			stdRight	= CNR_SOUTH_EAST;
			pBayEast->GetTemplate()->SetEHandRail( false );
			pBayEast->GetTemplate()->SetEBrace( false );
			break;
		case( S ):
			eSideLeft	= ESE;
			eSideRight	= WSW;
			stdLeft		= CNR_SOUTH_EAST;
			stdRight	= CNR_SOUTH_WEST;
			pBayEast->GetTemplate()->SetSHandRail( false );
			pBayEast->GetTemplate()->SetSBrace( false );
			break;
		case( W ):
			eSideLeft	= SSW;
			eSideRight	= NNW;
			stdLeft		= CNR_SOUTH_WEST;
			stdRight	= CNR_NORTH_WEST;
			pBayEast->GetTemplate()->SetWHandRail( false );
			pBayEast->GetTemplate()->SetWBrace( false );
			break;
		default:
			;//assert( pBayEast==NULL );
		}

		iSize = pBayWest->GetNumberOfLifts();
		jSize = 0;
		if( eSideEast!=SIDE_INVALID )
		{
			pBayEast->AddLapboardPointer( this, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_BRACING, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK, eSideEast );
			pBayEast->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSideEast );
			pBayEast->Delete1000mmHopupRailFromSide( eSideLeft );
			pBayEast->Delete1000mmHopupRailFromSide( eSideRight );
			if( eSideEast==NORTH || eSideEast==SOUTH )
			{
				pBayEast->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSideEast );
				//remove roof protection stuff
				pBayEast->DeleteAllComponentsOfTypeFromSide( CT_LEDGER, eSideEast, LIFT_RISE_1500MM );
				pBayEast->DeleteAllComponentsOfTypeFromSide( CT_LEDGER, eSideEast, LIFT_RISE_2000MM );
			}
			else
			{
				//remove roof protection stuff
				pBayEast->DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, eSideEast, LIFT_RISE_1500MM );
				pBayEast->DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, eSideEast, LIFT_RISE_2000MM );
			}

			pBayEast->Delete1000mmHopupRailFromSide( eSideLeft );
			pBayEast->Delete1000mmHopupRailFromSide( eSideRight );
			if( m_bAllowRecreateStandards )
			{
				pBayEast->MakeAStandardFitLifts( stdLeft );
				pBayEast->MakeAStandardFitLifts( stdRight );
			}
			if( bDraw )
				pBayEast->SetAllowDraw( true );
			pBayEast->UpdateSchematicView();
		}
		SetBayEast( pBayEast );
		jSize = pBayEast->GetNumberOfLifts();
	}

	DeleteAllComponents();

	//////////////////////////////////////////////////////////////
	//Find all lifts that share the same RL!

	#ifdef EXACT_NUMBER_OF_LAPS
	int iBoards;
	int iMaxBoards;
	iMaxBoards = GetTemplate()->m_bLapBoardReqdThisRun;
	#endif	//#ifdef EXACT_NUMBER_OF_LAPS

	int iBoards;
	iBoards = GetTemplate()->m_iNumberOfBoards;
	if( iBoards==0 )
	{
		iBoards = GetNumberOfPlanksForWidth( pBayWest->GetBayWidth() );
		iBoards+= pBayWest->GetNumberOfPlanksOfLongestStage( NORTH );
		iBoards+= pBayWest->GetNumberOfPlanksOfLongestStage( SOUTH );
		GetTemplate()->SetNumberOfBoards( iBoards );
	}

	for( i=0; i<iSize; i++ )
	{
		dRLWest = pBayWest->GetRLOfLift( i );
		if( pBayWest->GetLift( i )->IsDeckingLift() )
		{
			#ifdef EXACT_NUMBER_OF_LAPS
			iBoards = GetNumberOfPlanksForWidth( pBayWest->GetBayWidth() );
			iBoards+= pBayWest->GetLift( i )->GetNumberOfPlanksOnStageBoard( SOUTH );
			iBoards+= pBayWest->GetLift( i )->GetNumberOfPlanksOnStageBoard( NORTH );
			//limit the boards to the maximum number!
			iBoards = min( iBoards, iMaxBoards );
			GetTemplate()->SetNumberOfBoards( iBoards );
			#endif	//#ifdef EXACT_NUMBER_OF_LAPS

			if( pBayEast==NULL )
			{
				//Ok Both lifts have decking at this RL, create lapboard here!
				CreateLapboardLift( dRLWest );
			}
			else
			{
				for( j=0; j<jSize; j++ )
				{
					dRLEast = pBayEast->GetRLOfLift( j );
					if( dRLEast>=dRLWest-ROUND_ERROR && dRLEast<=dRLWest+ROUND_ERROR )
					{
						/////////////////////////////////////////////////////////////
						//We have a matching RL, so create a lift with lapboards
						/////////////////////////////////////////////////////////////

						///////////////////////////////////////////////////////////
						//if both lifts have decking then add a lift to the lapboardbay and
						//  remove all components except a handrail and lapboards
						if( pBayEast->GetLift( j )->IsDeckingLift() )
						{
							//Ok Both lifts have decking at this RL, create lapboard here!
							CreateLapboardLift( dRLEast );
						}
						else
						{
							//JSB todo 20000125 - maybe we should add a handrail here?
						}
						break;
					}
					else if( dRLEast>dRLWest )
					{
						//we have gone too far
						break;
					}
				}
			}
		}
	}

	if( bShowWarning && GetNumberOfLifts()<=0 )
	{
		CString sErrMsg;
		MessageBeep(MB_ICONEXCLAMATION);
		if( pBayEast!=NULL )
		{
			sErrMsg.Format( _T("\r\nWarning Bays %i and %i have no decking at the same height\r\n"),
							pBayWest->GetBayNumber(), pBayEast->GetBayNumber() );
			sErrMsg+= _T("to run a lapboard between!");
			sErrMsgG += sErrMsg;
		}
		else
		{
			sErrMsg.Format( _T("\r\nWarning Bay %i has no decking to run the lapboard from."),
							pBayWest->GetBayNumber() );
			sErrMsg+= _T("or there was an error creating the lapboards!");
			sErrMsgG += sErrMsg;
		}
		//MessageBox( NULL, sErrMsg, _T("Lapboard has no planks"), MB_OK );
		GetTemplate()->SetNumberOfBoards( 0 );
	}

	#ifdef EXACT_NUMBER_OF_LAPS
	/*
	//finally set the number of boards for the template to be the same as the
	//	number of boards for the top lift
	iBoards = GetNumberOfPlanksForWidth( pBayWest->GetBayWidth() );
	iBoards+= pBayWest->GetLift( pBayWest->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( SOUTH );
	iBoards+= pBayWest->GetLift( pBayWest->GetNumberOfLifts()-1 )->GetNumberOfPlanksOnStageBoard( NORTH );
	GetTemplate()->SetNumberOfBoards( iBoards );
	*/
	//Set the boards description in the schematic to the maximum
	GetTemplate()->SetNumberOfBoards( iMaxBoards );
	#endif	//#ifdef EXACT_NUMBER_OF_LAPS
}

int LapboardBay::AddLift(Lift *pLift, double dRL)
{
	//This is slightly different to the Bay::AddLift funtion since we don't have to be
	//	smack on top of the previous lift
	int			iID;
	Vector3D	Vector;
	Matrix3D	Transform;

	iID = m_LiftList.AddLift( pLift, 0.00 );
	pLift->SetBayPointer( this );

	//move this lift above the previous lift
	Vector.set( 0.00, 0.00, dRL );
	Transform.setTranslation( Vector );
	pLift->Move( Transform, true );

	m_LiftList.SetRL( iID, dRL );

	return iID;
}

int LapboardBay::CreateLapboardLift( double dRL )
{
	int		i, iLiftID, iNumberOfPlanks;
	Lift	*pLift;
	double	dFullLength;

	pLift = new Lift();

	//The lift must be 2m, otherwise no one could walk
	pLift->SetRise( LIFT_RISE_2000MM );
	iLiftID = AddLift( pLift, dRL );

	iNumberOfPlanks = GetTemplate()->m_iNumberOfBoards;
	if( iNumberOfPlanks<MIN_PLANKS_FOR_LAPBOARD || iNumberOfPlanks>MAX_PLANKS_FOR_LAPBOARD )
	{
		dFullLength = GetCommonBayLength( GetTemplate()->m_dFullWidth );
		iNumberOfPlanks = GetNumberOfPlanksForWidth( dFullLength );
	}

	for( i=0; i<iNumberOfPlanks; i++ )
	{
		dFullLength = GetCommonBayLength( GetTemplate()->m_dFullLength );
		pLift->AddComponent( CT_LAPBOARD, dFullLength, SOUTH, i, MT_TIMBER );
	}

	return iLiftID;
}

void LapboardBay::SetBayLength(double dLength)
{
	Bay::SetBayLength( dLength );
	PositionLapboardStandards();
}

void LapboardBay::SetBayWidth(double dWidth)
{
	Bay::SetBayWidth( dWidth );
	PositionLapboardStandards();
}

void LapboardBay::PositionLapboardStandards()
{
	//Set the positions of the standards, since they are used to dimension the bay
	Point3D		ptStandard[4];
	ptStandard[CNR_SOUTH_WEST].set( 0.00, 0.00, 0.00 );
	ptStandard[CNR_SOUTH_EAST].set( GetBayLength(), 0.00, 0.00 );
	ptStandard[CNR_NORTH_WEST].set( 0.00, GetBayWidth(), 0.00 );
	ptStandard[CNR_NORTH_EAST].set( GetBayLength(), GetBayWidth(), 0.00 );
	SetStandardPosition( ptStandard[CNR_NORTH_EAST], CNR_NORTH_EAST );
	SetStandardPosition( ptStandard[CNR_SOUTH_EAST], CNR_SOUTH_EAST );
	SetStandardPosition( ptStandard[CNR_NORTH_WEST], CNR_NORTH_WEST );
	SetStandardPosition( ptStandard[CNR_SOUTH_WEST], CNR_SOUTH_WEST );
}


//////////////////////////////////////////////////////////////////
//Serialize
//
//Storage and loading function
//
void LapboardBay::Serialize(CArchive &ar)
{
	Bay					*pBayWest, *pBayEast;
	int					iRunIDWest, iRunIDEast,
						iBayIDWest, iBayIDEast,
						iIndex, iSide, i,
						iSize, iRail;
	Lift				*pL;
	double				dLength;
	CString				sMsg, sTemp;
	intArray			iaHROs;
	Component			*pC;
	doubleArray			daHRLs;
	SideOfBayEnum		eSideWest, eSideEast;
	HandRailOptionEnum	eRail;

	iRunIDWest = ID_INVALID;
	iRunIDEast = ID_INVALID;
	iBayIDWest = ID_INVALID;
	iBayIDEast = ID_INVALID;

	iaHROs.RemoveAll();
	daHRLs.RemoveAll();

	if (ar.IsStoring())    // Store Object?
	{
		ar << LAPBOARDBAY_VERSION_LATEST;

		//////////////////////////////////////////////////////////////
		//LAPBOARDBAY_VERSION_1_0_2
		//store the handrail settings
		iSize = GetNumberOfLifts();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pL = GetLift( i );
			;//assert( pL->HasComponentOfTypeOnSide( CT_LAPBOARD, SOUTH ) );
			if( pL->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, SOUTH )!=NULL )
			{
				pC = pL->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, SOUTH );
				dLength = pC->GetLengthCommon();
				eRail = HRO_FULLMESH;
			}
			else if( pL->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, SOUTH )!=NULL )
			{
				pC = pL->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, SOUTH );
				dLength = pC->GetLengthCommon();
				eRail = HRO_MESH;
			}
			else if( pL->HasComponentOfTypeOnSide( CT_TOE_BOARD, SOUTH ) )
			{
				pC = pL->GetComponent( CT_TOE_BOARD, LIFT_RISE_0000MM, SOUTH );
				dLength = pC->GetLengthCommon();
				eRail = HRO_HRO;
			}
			else
			{
				eRail = HRO_NONE;
				dLength = 0.00;
			}
			iRail = (int)eRail;
			ar << iRail;
			ar << dLength;
		}
		
		//////////////////////////////////////////////////////////////
		//LAPBOARDBAY_VERSION_1_0_1
		pBayWest = GetBayWest();
		pBayEast = GetBayEast();

		if( pBayWest!=NULL )
		{
			iIndex = pBayWest->FindLapboardIndex( this );
			eSideWest = pBayWest->LapboardCrossesSideGet( iIndex );
		}

		if( pBayEast!=NULL )
		{
			iIndex = pBayEast->FindLapboardIndex( this );
			eSideEast = pBayEast->LapboardCrossesSideGet( iIndex );
		}
		iSide = (int)eSideWest;
		ar << iSide;
		iSide = (int)eSideEast;
		ar << iSide;

		//////////////////////////////////////////////////////////////
		//LAPBOARDBAY_VERSION_1_0_0
		m_LBTplt.Serialize(ar);

		if( pBayWest!=NULL )
		{
			;//assert( pBayWest->GetRunPointer()!=NULL );
			iRunIDWest = (pBayWest->GetRunPointer())->GetRunID();
			iBayIDWest = pBayWest->GetID();
		}
		if( pBayEast!=NULL )
		{
			;//assert( pBayEast->GetRunPointer()!=NULL );
			iRunIDEast = (pBayEast->GetRunPointer())->GetRunID();
			iBayIDEast = pBayEast->GetID();
		}
		
		ar << iRunIDWest;
		ar << iRunIDEast;
		ar << iBayIDWest;
		ar << iBayIDEast;
	}
	else
	{
		VersionNumber	uiVersion;

		eSideWest = SIDE_INVALID;
		eSideEast = SIDE_INVALID;


		ar >> uiVersion;
		Run *pRun;
		switch (uiVersion)
		{
		case LAPBOARDBAY_VERSION_1_0_2 :
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> iRail;
				ar >> dLength;
				//store these in an array and we will use them later
				iaHROs.Add( iRail );
				daHRLs.Add( dLength );
			}
			//fallthrough

		case LAPBOARDBAY_VERSION_1_0_1 :
			ar >> iSide;
			eSideWest = (SideOfBayEnum)iSide;
			ar >> iSide;
			eSideEast = (SideOfBayEnum)iSide;

			//fallthrough 
		case LAPBOARDBAY_VERSION_1_0_0 :
			m_LBTplt.Serialize(ar);

			ar >> iRunIDWest;
			ar >> iRunIDEast;
			ar >> iBayIDWest;
			ar >> iBayIDEast;

			pBayEast = NULL;
			pBayWest = NULL;

			;//assert( gpController!=NULL );
			if( iRunIDWest!=ID_INVALID )
			{
				pRun = gpController->GetRun(iRunIDWest);
				if( iBayIDWest!=ID_INVALID && pRun!=NULL )
					pBayWest = pRun->GetBay(iBayIDWest);
			}

			if( iRunIDEast!=ID_INVALID )
			{
				pRun = gpController->GetRun(iRunIDEast);
				if( iBayIDEast!=ID_INVALID && pRun!=NULL )
					pBayEast = pRun->GetBay(iBayIDEast);
			}

			if( pBayWest==NULL )
			{
				if( pBayEast==NULL )
				{
					//We have a floating lap
					;//assert( false );
					CString sMsg;
					sMsg.Format( _T("Lapboard Bay %i is reporting that it is not attached!\n"), GetBayNumber() );
					sMsg+= _T("to a bay.  In order to continue opening this file, I must\n");
					sMsg+= _T("delete this Lapboard!  You will have to recreate this manually.\n\n");
          sMsg+= _T("Note: The remaining bays will be renumbered after this action.");
					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox( NULL, sMsg, _T("Bay Open Error"), MB_OK|MB_ICONEXCLAMATION );
					SetDirtyFlag( DF_DELETE );
				}
				else
				{
					//swap the bays around
					pBayWest = pBayEast;
					pBayEast = NULL;
					SetBayPointers( pBayWest, pBayEast, eSideWest, eSideEast );
				}
			}
			else
			{
				if( pBayWest->GetNumberOfLifts()<=0 )
				{
					if( pBayEast==NULL )
					{
						;//assert(false);
						CString sMsg;
						sMsg.Format( _T("A Lapboard that is attached to bay %i cannot be created,\n"), pBayWest->GetBayNumber() );
						sMsg+= _T("since that bay is invalid.  In order to continue opening this file,\n");
						sMsg+= _T("I must delete this Lapboard!  You will have to recreate this manually.");
            sMsg+= _T("Note: This bay may be renumbered after this action.");
						MessageBeep(MB_ICONEXCLAMATION);
						MessageBox( NULL, sMsg, _T("Bay Open Error"), MB_OK|MB_ICONEXCLAMATION );
						SetDirtyFlag( DF_DELETE );
					}
					else if( pBayEast->GetNumberOfLifts()<=0 )
					{
						;//assert(false);
						CString sMsg;
						sMsg.Format( _T("I cannot create the lapboard that runs between bays %i and %i\n"), GetBayNumber() );
						sMsg+= _T("since these two bays are invalid.  In order to continue opening this\n");
						sMsg+= _T("file, I must delete this Lapboard!  You will have to recreate this manually.\n\n");
            sMsg+= _T("Note: These bays may be renumbered after this action.");
						MessageBeep(MB_ICONEXCLAMATION);
						MessageBox( NULL, sMsg, _T("Bay Open Error"), MB_OK|MB_ICONEXCLAMATION );
						SetDirtyFlag( DF_DELETE );
					}
					else
					{
						pBayWest = pBayEast;						
						pBayEast = NULL;
						SetBayPointers( pBayWest, pBayEast, eSideWest, eSideEast );
					}
				}
				else if( pBayEast!=NULL && pBayEast->GetNumberOfLifts()<=0 )
				{
					pBayEast = NULL;
					SetBayPointers( pBayWest, pBayEast, eSideWest, eSideEast );
				}
				else
				{
					SetBayPointers( pBayWest, pBayEast, eSideWest, eSideEast );
				}
			}

			break;
		default:
			;//assert( false );
			if( uiVersion>LAPBOARDBAY_VERSION_LATEST )
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
      sMsg+= _T("Class: LapboardBay.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), LAPBOARDBAY_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}

	//The bay serialize will reset the dirty flag
	bool bDirty, bDelete;
	bDirty	= GetDirtyFlag()==DF_DIRTY;
	bDelete	= GetDirtyFlag()==DF_DELETE;
	Bay::Serialize( ar );

	if( bDirty )
		SetDirtyFlag( DF_DIRTY );
	if( bDelete )
		SetDirtyFlag( DF_DELETE );

	if( iaHROs.GetSize()>0 )
	{
		//better create the handrail
		;//assert(	!(ar.IsStoring()) );
		;//assert( iaHROs.GetSize()==GetNumberOfLifts() );
		;//assert( iaHROs.GetSize()==daHRLs.GetSize() );
		int iSize = min( GetNumberOfLifts(), iaHROs.GetSize() );
		for( i=0; i<iSize; i++ )
		{
			pL = GetLift(i);
			eRail = (HandRailOptionEnum)iaHROs.GetAt(i);
			dLength = daHRLs.GetAt(i);
			switch( eRail )
			{
			case( HRO_HRO ):
				pL->AddComponent( CT_TOE_BOARD, dLength, SOUTH, LIFT_RISE_0000MM, MT_STEEL );
				break;
			case( HRO_FULLMESH ):
				pL->AddComponent( CT_MESH_GUARD, dLength, SOUTH, LIFT_RISE_2000MM, MT_STEEL );
				//fallthrough
			case( HRO_MESH ):
				pL->AddComponent( CT_MESH_GUARD, dLength, SOUTH, LIFT_RISE_1000MM, MT_STEEL );
				break;
			case( HRO_NONE ):
				break;
			case( HRO_INVALID ):
			default:
				;//assert( false /*Load or save error*/);
			}
		}
	}
}

void LapboardBay::UpdateSchematicView()
{
	;//assert( GetTemplate()!=NULL );
	GetTemplate()->UpdateSchematicView();
//	MoveSchematic( GetSchematicTransform(), false );
}

Matrix3D LapboardBay::GetSchematicTransform()
{
	;//assert( GetTemplate()!=NULL );
	return GetTemplate()->GetSchematicTransform();
}

void LapboardBay::SetSchematicTransform(Matrix3D Transform)
{
	;//assert( GetTemplate()!=NULL );
	GetTemplate()->SetSchematicTransform( Transform );
}

bool LapboardBay::IsBaySelected()
{
	return GetController()->AreObjectIDsSelected(GetTemplate()->GetObjectIds());
}
