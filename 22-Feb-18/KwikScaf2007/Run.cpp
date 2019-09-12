// Run.cpp: implementation of the Run class.
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
//								RUN CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 20/12/00 3:19p $
//	Version			: $Revision: 135 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This class was really added on as an after thought, I do believe we currently
//	have enough information to be able to produce a workable system, however
//	there may be a several of benefits to using this class!
//1. Storage of RL values, so that we can at a later stage, say during an edit
//		bay process be able to prompt the user to reapply RL calculations to
//		modifications.
//2. Control the lists of Bays
//3. Unique Run Number
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Run.h"
#include "Controller.h"
#include <math.h>
#include "PreviewTemplate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Run
//
Run::Run()
{
	m_baBays.RemoveAll();
	m_pController = NULL;
	m_pRTplt = new RunTemplate();
	CreateEmptyRun();
	SetAutobuildNumber();
	SetUseLongerBracing(true);
	SetLongerBracingValues();
}

//////////////////////////////////////////////////////////////////////
//~Run
//
Run::~Run()
{
	delete m_pRTplt;
	DeleteRun();
}


////////////////////////
////////////////////////
/////Functionality//////
////////////////////////
////////////////////////

//////////////////////////////////////////////////////////////////////////
//CreateRun
//This function is used to create a default run object containing one
//	simple Bay.  This would mainly be used by the Controller class to
//	create the template run object.  from there on in, all Runs would be a
//	copy of the existing template, and so would not need a CreateRun
//	function.
void Run::CreateEmptyRun( )
{
	SetGlobalsToDefault();
}

//////////////////////////////////////////////////////////////////////////
//DeleteRun
//Delete all elements of a run.  This fuction should be called by the
//	controller class
bool Run::DeleteRun( )
{
	return DeleteBays( 0, GetNumberOfBays(), false );
}

//////////////////////////////////////////////////////////////////////////
//RemoveRun
//Posts a message to the Controller to tell it to remove this run.
void Run::RemoveRun( )
{
	SetDirtyFlag( DF_DELETE );
}


//////////////////////////////////////////////////////////////////////////
//Split/Join
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//SplitRun
//This function has to consider the following functionality, some of this
//	functionality belongs to the Run Class, some belongs to the Bay class:
//	Break Links
//		Break Forward Pointer
//		Break Backward Pointer
//		Break Outer Pointer
//		Bread Inner Pointer
//	Tell Run object to recalc RL's for the new split points
//	Create a new Run and add to the list of Runs
//	Move Split Bays from this run to the new run (actually uses copy to
//	create a new bay and then delete to remove copied bay).
Run *Run::SplitRun( int iBayID )
{
	Run			*pNewRun;
	Matrix3D	Transform;
	int			iSize, iCount;

	iSize = GetNumberOfBays();

	if( iBayID<=0 )
	{
		//the new run is this entire run
		return this;
	}

	if( iBayID>=iSize )
	{
		//There are no runs to split from the end of the run
		return NULL;
	}
	
	//we don't want to move the bays at all
	Transform.setToIdentity();

	pNewRun = CopyRun( Transform, true, false );

	//we now have two exact copies of the run
	//	remove all bays before iBayID from one
	///////////////////////////////////////////////////////////////
	//Store the movement
	double dDeletedLength = 0;
	Bay	*pBay;
	for( int i=0; i<=iBayID; i++ )
	{
		pBay			= GetBay( i );
		dDeletedLength	+=pBay->GetBayLengthActual();
	}

	///////////////////////////////////////////////////////////////
	//Delete the bays - note this will also delete the inner and
	//	outer pointers for the copied bay, we will have to reestablish those	
	pNewRun->EraseBays( 0, iBayID+1 );
	pNewRun->ReIDSubsequentBays( 0 );
	pNewRun->MoveStartOfRunBackward( dDeletedLength );
	pNewRun->SetBaysWithBracing();
	pNewRun->SetBaysWithTies();
	
	for(int i=0; i<=iBayID; i++ )
	{
		pBay = GetBay( i );
		if( pBay->GetInner()!=NULL )
		{
			;//assert( pBay->GetInner()->GetOuter()==NULL );
			pBay->GetInner()->SetOuter( pBay );
		}
		if( pBay->GetOuter()!=NULL )
		{
			;//assert( pBay->GetOuter()->GetInner()==NULL );
			pBay->GetOuter()->SetInner( pBay );
		}
	}

//	GetController()->TracePointers();

	//	remove all bays after iBayID from the other
	iCount = iSize - iBayID - 1;
	EraseBays( iBayID+1, iCount );
	//no need to reshuffle here, since there is nothing left to reshuffle
	SetBaysWithBracing();
	SetBaysWithTies();

//	GetController()->TracePointers();

	//the pointer for the new run should be correct, however, nothing
	//	will be pointing to this runs bay's
	for(int i=0; i<pNewRun->GetNumberOfBays(); i++ )
	{
		pBay = pNewRun->GetBay( i );
		if( pBay->GetInner()!=NULL )
		{
			;//assert( pBay->GetInner()->GetOuter()==NULL );
			pBay->GetInner()->SetOuter( pBay );
		}
		if( pBay->GetOuter()!=NULL )
		{
			;//assert( pBay->GetOuter()->GetInner()==NULL );
			pBay->GetOuter()->SetInner( pBay );
		}
	}

	//runs cannot be joint!
	pNewRun->GetBay(0)->SetBackward( NULL, false, true );
	GetBay(iBayID)->SetForward( NULL, false, true );

//	GetController()->TracePointers();
	
	return pNewRun;
}

//////////////////////////////////////////////////////////////////////////
//JoinRun
//This function will join another run to the end of this run, and make them
//	into one run.  The major issue with this function will be how to set
//	the RL's for this new, longer, Run.  Some other considerations will be
//	moving the second run to meet the first, testing the Transom lengths
//	are the same and other rules between bays, so it is possible for this
//	operation to fail.  Naturally the runs will have to follow on from one
//	another, so a MoveRun command will be needed to perform it's duties
bool	Run::JoinRun( Run *pRun, JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	JoinType;
	pRun;

	//insert a copy of each bay from the other run onto the end of this run.
	;//assert( FALSE); //JSB todo 990729 - complete this function
/*
	JOIN_TYPE_FORWARD,
	JOIN_TYPE_BACKWARD,
	JOIN_TYPE_INNER,
	JOIN_TYPE_OUTER
*/
	//delete the other run

	//calculate the new RLs
	return true;
}

//////////////////////////////////////////////////////////////////////////
//JoinRun
//
bool	Run::JoinRun( int iRunID, JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	Run *pRun;
	pRun = GetController()->GetRun(iRunID);

	return JoinRun( pRun, JoinType );
}


//////////////////////////////////////////////////////////////////////////
//Controller class
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//GetController
//Access to the controller pointer
Controller *Run::GetController( ) const
{
	return m_pController;
}

//////////////////////////////////////////////////////////////////////////
//SetController
void Run::SetController( Controller *pController )
{
	m_pController = pController;

}


//////////////////////////////////////////////////////////////////////////
//Move/Copy etc
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Move
//Moves this run.  Don't forget to notify the lapboards that the run has
//	been moved!
void Run::Move( Matrix3D Transform, bool bStore, int iFixedDirection/*=FIXED_DIRECTION_BOTH*/ )
{
	int		i, iBayID, iSize;
	Bay		*pB;
	bool	bFound;
	RunList	InnerRuns, OuterRuns;

	InnerRuns.RemoveAll();
	OuterRuns.RemoveAll();

	//store it
	if( bStore )
	{
		SetTransform ( Transform*GetTransform() );
	}

	iSize = GetNumberOfBays();
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pB=GetBay(iBayID);
		pB->Move( Transform, false );

		if( iFixedDirection!=FIXED_DIRECTION_NEITHER )
		{
			if( (iFixedDirection==FIXED_DIRECTION_BOTH ||
				iFixedDirection==FIXED_DIRECTION_INNER ) &&
				pB->GetInner()!=NULL )
			{
				;//assert( pB->GetInner()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( i=0; i<InnerRuns.GetSize(); i++ )
				{
					if( InnerRuns[i]==pB->GetInner()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					InnerRuns.Add( pB->GetInner()->GetRunPointer() );
				}
			}

			if( (iFixedDirection==FIXED_DIRECTION_BOTH ||
				iFixedDirection==FIXED_DIRECTION_OUTER ) &&
				pB->GetOuter()!=NULL )
			{
				;//assert( pB->GetOuter()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( i=0; i<OuterRuns.GetSize(); i++ )
				{
					if( OuterRuns[i]==pB->GetOuter()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					OuterRuns.Add( pB->GetOuter()->GetRunPointer() );
				}
			}
		}
	}

	for( i=0; i<InnerRuns.GetSize(); i++ )
	{
		InnerRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_INNER );
	}

	for( i=0; i<OuterRuns.GetSize(); i++ )
	{
		OuterRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_INNER );
	}
}

//////////////////////////////////////////////////////////////////////////
//CopyRun
//Copies this run, a copy command will always use a Move command so that
//	the two copies are not sitting on top of one another.
Run *Run::CopyRun( Matrix3D Transform, bool bCopyInnerOuterPointers, bool bMove/*=true*/ )
{
	Run *pNewRun;

	pNewRun = new Run;

	*pNewRun = *this;

	GetController()->InsertRun( GetRunID()+1, pNewRun );

	;//assert( pNewRun->GetNumberOfBays()==GetNumberOfBays() );

	//Copy will also retain the transform info
	pNewRun->SetTransform( GetTransform() );
	pNewRun->SetSchematicTransform( GetSchematicTransform() );
	int i;
	Bay	*pBay, *pPrevBay;
	pPrevBay = NULL;
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		pBay = pNewRun->GetBay(i);
		;//assert( pBay!=NULL );
		;//assert( GetBay(i)!=NULL );
		pBay->SetTransform( GetBay(i)->GetTransform() );
		pBay->SetSchematicTransform( GetBay(i)->GetSchematicTransform() );
	
		//we need to re-establish the bay pointers, and copy those pointers
		if( pPrevBay!=NULL )
		{
			pBay->SetBackward( pPrevBay );
			pPrevBay->SetForward( pBay );
		}

		if( bCopyInnerOuterPointers )
		{
			pBay->SetInner( GetBay(i)->GetInner() );
			pBay->SetOuter( GetBay(i)->GetOuter() );
		}

		pPrevBay = pBay;
	}


	if( bMove )
	{
		pNewRun->Move( Transform, true );
		pNewRun->MoveSchematic( Transform, true );
	}

	return pNewRun;
}


//////////////////////////////////////////////////////////////////////////
//RL's
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//SetRL
//Sets/Gets the RL for a Run, this should then be used to determine the
//	Height of each bay in the Run.
bool	Run::SetRL( Point3D ptRLBegin, Point3D ptRLEnd, bool bAdjustBayHeights/*=true*/ )
{
	int			iBayID;
	double		dSlope, dDistance,
				dNewHeight, dBayHeight, dBayLength,
				dX, dY, dZ, ddX, ddY, ddZ,
				dAngle;
	Point3D		ptNewPos, pBayPos, ptStandard[4], ptTemp, ptTemp2;
	Bay			*pBay;
	Matrix3D	Transform, Rotation;
	Vector3D	Vector;

	SetPoint( WP_START, ptRLBegin );
	SetPoint( WP_END, ptRLEnd );

	for( iBayID=0; iBayID<GetNumberOfBays(); iBayID++ )
	{
		pBay = GetBay(iBayID);
		ptStandard[CNR_SOUTH_WEST].set( 0.00, 0.00, 0.00 );
		ptStandard[CNR_SOUTH_EAST].set( pBay->GetBayLength(), 0.00, 0.00 );
		ptStandard[CNR_NORTH_WEST].set( 0.00, pBay->GetBayWidth(), 0.00 );
		ptStandard[CNR_NORTH_EAST].set( pBay->GetBayLength(), pBay->GetBayWidth(), 0.00 );
		pBay->SetStandardPosition( ptStandard[CNR_NORTH_EAST], CNR_NORTH_EAST );
		pBay->SetStandardPosition( ptStandard[CNR_SOUTH_EAST], CNR_SOUTH_EAST );
		pBay->SetStandardPosition( ptStandard[CNR_NORTH_WEST], CNR_NORTH_WEST );
		pBay->SetStandardPosition( ptStandard[CNR_SOUTH_WEST], CNR_SOUTH_WEST );
	}

	if( bAdjustBayHeights )
	{
		;//assert( FALSE ); //JSB todo 990727 - complete this functionality

		//calculate the drop per mm
		dX = GetPointStart().x - GetPointEnd().x;
		dY = GetPointStart().y - GetPointEnd().y;
		dDistance = (dX*dX) + ( dY*dY );
		dDistance = sqrt( dDistance );
		dZ = GetPointStart().z - GetPointEnd().z;
		dSlope = dZ/dDistance;

		//calculate the angle of rotation of these two points from the x Axis,
		//	measured around the z axis
		;//assert( FALSE );	//??? JSB todo 990728 - complete this functionality
		dAngle = 0.00;

		ptNewPos = GetPointStart();
		//for each bay in the run
		for( iBayID=0; iBayID<GetNumberOfBays(); iBayID )
		{
			pBay			= GetBay(iBayID);

			//find the original height and rl
			dBayHeight		= pBay->GetHeightOfStandards( CNR_SOUTH_WEST );
			pBayPos			= pBay->GetStandardPosition( CNR_SOUTH_WEST );
			dBayLength		= pBay->GetBayLength();

			//How far has this bay moved?
			ddX				= ptNewPos.x - pBayPos.x;
			ddY				= ptNewPos.y - pBayPos.y;
			ddZ				= ptNewPos.z - pBayPos.z;

			//Had the Height been affected by the change in the RLs?
			dNewHeight		= dBayHeight + ddZ;
			
			//we need to move from original location to new location
			Vector.x		= ddX;
			Vector.y		= ddY;
			Vector.z		= ddZ;
			//Vector.z		= 0.00;
			Transform.setToTranslation( Vector );
			pBay->Move( Transform, true );
			pBay->MoveSchematic( Transform, true );

			//This needs to be parallel to the Begin and End RLs
			//Vector.x		= 0.00
			//Vector.y		= 0.00;
			//Vector.z		= 1.00;
			//Rotation.setToRotation( dAngle, Vector, ptNewPos );
/*			pBay->RotateBayAbsolute( dAngle );

			//This will recreate the bay so that if fits the new height,
			//	without adjusting the RLs, which we have just calculated.
			pBay->AdjustBayHeight( dNewHeight );
*/
			//The next bay will be dBayLength further along
			ptNewPos.x+= dBayLength*(dX/dDistance);
			ptNewPos.y+= dBayLength*(dY/dDistance);
			ptNewPos.z+= dBayLength*(dZ/dDistance);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//GetRL
//retrieves the RLs for the begining and ending points
bool	Run::GetRL( Point3D &ptRLBegin, Point3D &ptRLEnd ) const
{
	ptRLBegin	= GetPointStart();
	ptRLEnd		= GetPointEnd();
	return true;
}




//////////////////////////////////////////////////////////////////////////
//Run ID
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//SetRunID
//Queries the Controller class to obtain its number in the RunList
//This number would be translated into a letter from 'A' through to 'ZZZZZ…'.
bool	Run::SetRunID( int iRunID )
{
	m_iRunID = iRunID;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//GetRunID
//
int		Run::GetRunID( ) const
{
	return m_iRunID;
}

//////////////////////////////////////////////////////////////////////////
//GetSurfaceArea
//call all the bays in turn to find their surface area.
double Run::GetSurfaceArea( ) const
{
	double	dArea;
	Bay		*pBay;

	int iBayID, iSize;

	iSize = GetNumberOfBays();
	dArea = 0.00;
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pBay=GetBay(iBayID);
		dArea+= pBay->GetSurfaceArea();
	}
	return dArea;
}

//////////////////////////////////////////////////////////////////////////
//WriteBOMTo
//used to create the BOM
void Run::WriteBOMTo( CStdioFile *pFile ) const
{
	Bay			*pBay;
	int			iBayID, iSize;
	CString		sLine;

	iSize = GetNumberOfBays();
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pBay=GetBay(iBayID);
		pBay->WriteBOMTo( pFile );
	}
}


//////////////////////////////////////////////////////////////////////////
//Bay Stuff
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//InsertBay
//Inserts a bay into a run attached to pBay.  This will be an 'AtGrow' function
//needs to consider repositioning of RL and End point.  Need to check that
//	the bay to insert before exists within the run.
Bay		*Run::InsertBay( Bay *pBay, JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	int	iBayID;

	iBayID = GetID(pBay);

	if( iBayID<0 )
		return NULL;

	return InsertBay( iBayID, JoinType );
}

//////////////////////////////////////////////////////////////////////////
//InsertBay
//Inserts a bay into a run attached to iBayID.  This will be an 'AtGrow' function
//needs to consider repositioning of RL and End point.  Need to check that
//	the bay to insert before exists within the run.
Bay		*Run::InsertBay( int iBayID, JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	Bay *pNewBay, *pSelectedBay;

	////////////////////////////////////////////////////////////////
	//test the bay id
	if( !IsBayIDValid(iBayID) )
		return NULL;

	////////////////////////////////////////////////////////////////
	//create a new bay
	pNewBay = new Bay();
	pNewBay->SetRunPointer( this );

	////////////////////////////////////////////////////////////////
	//Make the new bay a duplicate of the neighboring bay
	pSelectedBay = GetBay( iBayID );

	*pNewBay = *pSelectedBay;

	////////////////////////////////////////////////////////////////
	//insert the bay into the run, or create a new run if required
	if( !InsertBay( iBayID, pNewBay, JoinType ) )
	{
		//DeleteBays( iBayID, 1, false ) will not work here since it will not be in the bay list
		delete pNewBay;
		return NULL;
	}

	pNewBay->ReapplyBayPointers();

	return pNewBay;
}

//////////////////////////////////////////////////////////////////////////
//InsertBay
//Inserts pNewBay into the run, and attaches it to pBay, using the JoinType
bool Run::InsertBay( int iBayID, Bay *pNewBay, JoinTypeEnum JoinType/*=DEFAULT_JOIN_TYPE*/ )
{
	Bay			*pSelectedBay;

	;//assert( iBayID>=0 );
	;//assert( iBayID<GetNumberOfBays() );

	pSelectedBay = GetBay( iBayID );
	;//assert( pSelectedBay!=NULL );	//Whoa! Something has fucked up!

	switch( JoinType )
	{
	case( JOIN_TYPE_FORWARD ):
		if( !JoinForward( iBayID, pSelectedBay, pNewBay ) )
			return false;
		break;
	case( JOIN_TYPE_BACKWARD ):
		if( !JoinBackward( iBayID, pSelectedBay, pNewBay ) )
			return false;
		break;
	case( JOIN_TYPE_INNER ):
		if( !JoinInner( pSelectedBay, pNewBay ) )
			return false;
		break;
	case( JOIN_TYPE_OUTER ):
		if( !JoinOuter( pSelectedBay, pNewBay ) )
			return false;
		break;
	default:
		;//assert( false );
		return false;
	}

	GetController()->RenumberAllBays();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//GetBay
//retrieves a pointer to a bay
Bay		*Run::GetBay( int iBayID ) const
{
	if( !IsBayIDValid(iBayID) )
		return NULL;

	return m_baBays.GetAt( iBayID );
}

//////////////////////////////////////////////////////////////////////////
//GetID
//retrieves the number of the bay from the pointer
int	Run::GetID( Bay *pBay ) const
{
	int iBayID, iSize;

	iSize = GetNumberOfBays();

	if( iSize<=0 )
		return ID_NO_IDS;

	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		if( GetBay(iBayID)==pBay )
			return iBayID;
	}
	return ID_NONE_MATCHING;
}

//////////////////////////////////////////////////////////////////////////
//DeleteBays
//Removes bay from the specified run, and then tells each of the
//	following Bays to shift closer to fill the void.
bool Run::DeleteBays(int iBayID, int iCount/*=1*/, bool bShuffleSubsequentBays/*=true*/ )
{
	int			i, iSize, iMaxDelete;
	Bay			*pBay, *pBayOuter, *pBayInner, *pBayForward, *pBayBackward;
	double		dDeletedLength;

	iMaxDelete	= iCount+iBayID;
	iSize		= GetNumberOfBays();

	if( GetController()!=NULL && GetController()->IsDestructingController() )
		bShuffleSubsequentBays = false;

	///////////////////////////////////////////////////////////////
	//check the ranges
	if( (iCount<=0) || (iBayID<0) || (iBayID>=iSize) ||
		 (iMaxDelete>iSize) )
		return false;

	SetAllowDrawBays(false);
	
	pBay = GetBay( iBayID );
	pBayOuter		= NULL;
	pBayInner		= NULL;
	pBayForward		= NULL;
	pBayBackward	= NULL;
	if( pBay!=NULL )
	{
		pBayOuter		= pBay->GetOuter();
		pBayInner		= pBay->GetInner();
		pBayForward		= pBay->GetForward();
		pBayBackward	= pBay->GetBackward();
	}

	///////////////////////////////////////////////////////////////
	//We need to find if we can shuffle this run along without any
	//	repocussions, ie we cannot resuffle a birdcage!
	if( bShuffleSubsequentBays )
	{
		Matrix3D	Transform;
		//To find out if this bay is in a birdcage, we will traverse
		//	each connected bay, starting with the bay to the right,
		//	and excluding the currently selected bay, and then we
		//	will see if the bay to the left is visited during that
		//	traversing!  If it is then there is another connection
		//	which will not allow us to rejoin the bays either side
		//	of the deleted bays!

		//mark all bays as unvisited!
		GetController()->SetHasBeenVisited(false);

		if( pBayForward!=NULL && pBayBackward!=NULL )
		{
			//we need to exclude this bay, so mark it as visited!
			pBay->SetHasBeenVisited( true );

			//when visiting don't move the bays!
			Transform.setToIdentity();
			pBayForward->Visit( Transform );

			//see if the bay to the west have been visitied!
			if( pBayBackward->HasBeenVisited() )
			{
				//Yes there was a connection between the two neighboring
				//	bay, other than via the selected bay!  So we can't
				//	shuffle!
				bShuffleSubsequentBays = false;
			}
		}
		else
		{
			//There is no bay to the left or right!  So
			//	Who cares there is nothing to shuffle anyway!

			if( iBayID==0 )
			{
				//This is the first bay in the run, so delete from the left
				bShuffleSubsequentBays = false;
			}
		}

		//Reset the visited flag on all bays!
		GetController()->SetHasBeenVisited(false);
	}
	
	if( !bShuffleSubsequentBays )
	{
		SplitRun( iBayID );
	}

	///////////////////////////////////////////////////////////////
	//Store the movement
	dDeletedLength = 0;
	for( i=iBayID; i<iMaxDelete; i++ )
	{
		pBay			= GetBay( i );
		dDeletedLength	+=pBay->GetBayLengthActual();
/*
		//break the inner/outer links for the deleted bays
		if( pBay->GetInner()!=NULL )
		{
			pBay->GetInner()->SetOuter(NULL, false, true );
		}

		if( pBay->GetOuter()!=NULL )
		{
			pBay->GetOuter()->SetInner(NULL, false, true );
		}
*/
	}

	///////////////////////////////////////////////////////////////
	//Delete the bays
	EraseBays( iBayID, iCount);
/*
	if( !bShuffleSubsequentBays )
	{
		//We are not suffling the bays, so we better add a handrail!
		if( GetBay(iBayID-1)!=NULL )
			GetBay(iBayID-1)->SetForward( NULL, false, true );
	}
*/
	///////////////////////////////////////////////////////////////
	//re-ID subsequent bays
	ReIDSubsequentBays( iBayID );

	Bay *pPrevBay;
	pPrevBay = NULL;
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		pBay			= GetBay( i );
		if( pPrevBay!=NULL )
		{
			pBay->SetBackward( pPrevBay );
			pPrevBay->SetForward( pBay );
		}
		pPrevBay = pBay;
	}

	///////////////////////////////////////////////////////////////
	//move subsequent bays
	if( bShuffleSubsequentBays && iBayID<GetNumberOfBays() )
	{
		ShuffleSubsequentBays( iBayID, dDeletedLength, false );
//		ShuffleSubsequentBays( iBayID, dDeletedLength );
	}

	///////////////////////////////////////////////////////////////
	//If there are no bays then this run should be deleted!
	if( GetNumberOfBays()<=0 )
	{
		SetDirtyFlag( DF_DELETE );
	}

	Run	*pRun;
	//redraw any bays that are not reshuffled, provided this is not the autobuild run
	if( !bShuffleSubsequentBays && GetRunID()>=0 )
	{
		pRun = GetController()->GetRun( GetRunID()+1 );
		if( pRun!=NULL )
		{
			pRun->SetAllowDrawBays(true);
			pRun->UpdateSchematicView();
		}
	}

	SetAllowDrawBays(true);
	UpdateSchematicView();

	return true;
}


////////////////////////
////////////////////////
////Hidden Functions////
////////////////////////
////////////////////////

//////////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//sets the member variables to 'empty' values
void Run::SetGlobalsToDefault()
{
	DeleteRun();
	SetRunID( ID_INVALID );
	m_pController	= NULL;

	SetPointStart( Point3D(0.00, 0.00, 0.00 ) );
	SetPointEnd( Point3D(0.00, 0.00, 0.00 ) );

	SetTieType( DEFAULT_TIE_TYPE );
	SetTiesVerticallyEvery( DEFAULT_TIE_VERTICAL_SPACING );

	SetTransform( GetTransform().setToIdentity() );
	SetDirtyFlag( DF_CLEAN );
}

//////////////////////////////////////////////////////////////////////////
//IsBayIDValid
//Tests the ID to see if it is valid!
bool Run::IsBayIDValid(int iBayID) const
{
	int	iSize;

	iSize = GetNumberOfBays();

	if( iSize<0 )
		return false;
	
	if( (iBayID<0) || (iBayID>=iSize) )
		return false;

	return true;
}


/////////////////////////////////////////////////////////////////////
//operator= 
//
Run & Run::operator=(const Run &OriginalRun )
{
	int	iBayID, iSize;
	Bay	*pNewBay;

	//remove all element from this run
	SetGlobalsToDefault();

	m_pController = OriginalRun.m_pController;

	//the run id has not been set
	SetRunID( ID_INVALID );

	//copy the bays
	iSize = OriginalRun.GetNumberOfBays();
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pNewBay = new Bay();
		pNewBay->SetRunPointer( this );
		*pNewBay = *(OriginalRun.GetBay(iBayID));
		if( iBayID>=GetNumberOfBays() )
			AddBay( pNewBay );
		else
			InsertBay( iBayID, pNewBay );
	}
	ReIDSubsequentBays(0);

	Point3D ptBegin, ptEnd;
	OriginalRun.GetRL( ptBegin, ptEnd );
	SetRL( ptBegin, ptEnd, false ); 

	GetRunTemplate()->SetBaysPerBrace( OriginalRun.GetRunTemplate()->GetBaysPerBrace() );

	SetTieType( OriginalRun.GetRunTemplate()->GetTieType() );
	SetTiesVerticallyEvery( OriginalRun.GetRunTemplate()->GetTiesVerticallyEvery() );

	*(GetRunTemplate())		= *(OriginalRun.GetRunTemplate());

	SetUseLongerBracing(OriginalRun.GetUseLongerBracing());
	SetBracingLength( COMPONENT_LENGTH_2400, OriginalRun.GetBracingLength( COMPONENT_LENGTH_2400 ) );
	SetBracingLength( COMPONENT_LENGTH_1800, OriginalRun.GetBracingLength( COMPONENT_LENGTH_1800 ) );
	SetBracingLength( COMPONENT_LENGTH_1200, OriginalRun.GetBracingLength( COMPONENT_LENGTH_1200 ) );
	SetBracingLength( COMPONENT_LENGTH_0700, OriginalRun.GetBracingLength( COMPONENT_LENGTH_0700 ) );

/*
	Run::CopyRun is the only function that also copies the Transform
	SetTransform( OriginalRun.GetTransform() );
	SetSchematicTransform( OriginalRun.gets
*/
	return *this;
}

////////////////////////////////////////////////////////////////////
//operator==
//
bool Run::operator==(const Run &OtherRun ) const
{
	int			iBayID, iSize;
	Point3D		ptBegin, ptEnd;

	//Runs - the run id is unique so don't test
	//if( GetRunID()	!=OtherRun.GetRunID() ) return false;

	//Bays
	iSize = GetNumberOfBays();
	if( iSize!=OtherRun.GetNumberOfBays() ) return false;
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		if( GetBay(iBayID) != OtherRun.GetBay(iBayID) ) return false;
	}

	//controller
	if( m_pController	!= OtherRun.m_pController ) return false;

	//position
	//the position of two runs should be unique so don't test
	//OtherRun.GetRL( ptBegin, ptEnd );
	//if( ptBegin	!=GetPointStart() )	return false;
	//if( ptEnd		!=GetPointEnd() )	return false;

	//bracing/ties
	;
	if( GetRunTemplate()->GetBaysPerBrace()
		!= OtherRun.GetRunTemplate()->GetBaysPerBrace() )
			return false;
	if( GetRunTemplate()->GetBaysPerTie()
		!= OtherRun.GetRunTemplate()->GetBaysPerTie() )
			return false;

	if( GetRunTemplate()->GetTieType()		!= OtherRun.GetRunTemplate()->GetTieType() )				return false;
	if( GetRunTemplate()->GetTiesVerticallyEvery()
				!= OtherRun.GetRunTemplate()->GetTiesVerticallyEvery() )	return false;

	return true;
}

////////////////////////////////////////////////////////////////////
//operator!=
//
bool Run::operator!=(const Run &OtherRun ) const
{
	return !operator==( OtherRun );
}

////////////////////////////////////////////////////////////////////
//GetNumberOfBays
//retrieves the number of bays in the run
int Run::GetNumberOfBays() const
{
	return m_baBays.GetSize();
}

////////////////////////////////////////////////////////////////////
//AddBay
//Adds a bay and sets all the appropriate pointes and numbers for that bay
int Run::AddBay(Bay *pBay, bool bAdjustStandards/*=true*/, bool bSetBayPointers/*=true*/ )
{
	Bay		*pPrevBay;
	int		iID;

	iID = m_baBays.Add( pBay );
	pBay->SetRunPointer( this );
	pBay->SetID( iID );
	if( bSetBayPointers )
	{
		if( iID>0 )
		{
			pPrevBay = GetBay( iID-1 );
			;//assert( pPrevBay!=NULL );
			;//assert( pPrevBay!=pBay );
			pPrevBay->SetForward( pBay, true, bAdjustStandards );
			pBay->SetBackward( pPrevBay, true, bAdjustStandards );

		}
		else if( iID==0 )
		{
			pBay->SetBackward( NULL, false, true );
		}
		else
		{
			;//assert( false );
		}
		pBay->SetForward( NULL, true, bAdjustStandards );
//		This doesn't seem to do anything
//		pBay->SetInner( NULL, false, bAdjustStandards );
//		pBay->SetOuter( NULL, false, bAdjustStandards );
	}

	return iID;
}

////////////////////////////////////////////////////////////////////
//Create3DView
//Tells the components of the run to draw themselves
bool Run::Create3DView( int &iPos )
{
	Bay *pBay;
	int	i, iSize;
	iSize = GetNumberOfBays();

	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		if( !pBay->Create3DView() )
			return false;
		acedSetStatusBarProgressMeterPos(iPos++);
	}
	return true;
}

////////////////////////////////////////////////////////////////////
//Delete3DView
//Tells the components of the run to draw themselves
void Run::Delete3DView()
{
	Bay *pBay;
	int	i, iSize;
	iSize = GetNumberOfBays();

	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		pBay->Delete3DView();
	}
}

////////////////////////////////////////////////////////////////////
//GetVisible
//finds the visibility for all components of the run, if they are the
//	same, otherwise it return MULTISELECT or some other constant
VisibilityEnum Run::GetVisible() const
{
	VisibilityEnum	eVisible, eVis;
	Bay				*pBay;
	int				i, iSize;

	iSize = GetNumberOfBays();
	if( iSize<=0 )
		return VIS_INVALID;

	pBay		= GetBay(0);
	eVisible	= pBay->GetVisible();

	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		eVis = pBay->GetVisible();
		if( eVis==VIS_MULTI_SELECTION )
			return VIS_MULTI_SELECTION;
		else if( eVis==VIS_INVALID )
			return VIS_INVALID;
		else if( eVisible!=eVis )
			return VIS_MULTI_SELECTION;
	}
	//they are all the same
	return eVisible;
}

////////////////////////////////////////////////////////////////////
//SetVisible
//Sets the visibility all components of the run
void Run::SetVisible(VisibilityEnum eVisible/*=VIS_VISIBLE*/)
{
	Bay *pBay;
	int	i, iSize;
	iSize = GetNumberOfBays();

	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		pBay->SetVisible(eVisible);
	}
}


int Run::GetBaysPerTie()
{
	return GetRunTemplate()->GetBaysPerTie();	
}

int Run::GetBaysPerBrace()
{
	return GetRunTemplate()->GetBaysPerBrace();
}

TieTypesEnum Run::GetTieType()
{
	return GetRunTemplate()->GetTieType();
}

double Run::GetTiesVerticallyEvery()
{
	return GetRunTemplate()->GetTiesVerticallyEvery();
}

//set funcs
void Run::SetTiesEvery(int iTiesEvery)
{
	GetRunTemplate()->SetBaysPerTie( iTiesEvery  );
}

void Run::SetBaysPerBrace(int iBracingEvery)
{
	GetRunTemplate()->SetBaysPerBrace( iBracingEvery );
}

void Run::SetTieType(TieTypesEnum eTieType)
{
	GetRunTemplate()->SetTieType(eTieType);
}

void Run::SetTiesVerticallyEvery( double dEvery )
{
	GetRunTemplate()->SetTiesVerticallyEvery( dEvery );	
}

void Run::SetTransform(Matrix3D Transform)
{
	m_Transform = Transform;
}

Matrix3D Run::GetTransform() const
{
	return m_Transform;
}

Matrix3D Run::UnMove()
{
	Matrix3D	Original, Inverse;

	//store the original transformation for later use
	Original = GetTransform();

	//we need to move the Bay back to its original position
	Inverse = GetTransform();
	Inverse.invert();

	Move( Inverse, true );

	//return the original matrix
	return Original;
}

Matrix3D Run::CalculateRunTransform(Point3D pt1st, Point3D pt2nd)
{
	//we have now created the new run, and we have to rotate the entire run
	Matrix3D	Transform, Rotation;
	Vector3D	Vector;
	double		dAngle;

	//Calculate the transformation Matrix required to move the run from the
	//	Origin and || to the Xaxis to a the first mouse point and correctly
	//	rotated.
	pt1st.z = 0.00;
	Vector	= FromOriginToPointAsVector( pt1st );
	pt2nd.z = 0.00;
	pt2nd	= pt2nd - Vector;
	dAngle	= CalculateAngle( pt2nd, X_AXIS );
	Transform.setToIdentity();
	Transform.setTranslation( Vector );
	//rotation
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToIdentity();
	Rotation.setToRotation( dAngle, Vector );
	Transform = Transform * Rotation;
	return Transform;
}


//////////////////////////////////////////////////////////////////
//Has the been marked for deletion
//////////////////////////////////////////////////////////////////
//Says this object needs to be deleted
DirtyFlagEnum	Run::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

//set the object to be deleted
void	Run::SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward/*=false*/ )
{
	if( dfDirtyFlag==DF_CLEAN )
	{
		//set the dirty flag
		m_dfDirtyFlag = dfDirtyFlag;
		if(bSetDownward)
		{
			for( int i=0; i<GetNumberOfBays(); i++ )
			{
				GetBay(i)->SetDirtyFlag(dfDirtyFlag, bSetDownward);
			}
		}
		return;
	}

	if( m_dfDirtyFlag==DF_DELETE ||
		m_dfDirtyFlag==DF_DIRTY )
	{
		//don't worry about it, it is already marked
		return;
	}

	//set the dirty flag
	m_dfDirtyFlag = dfDirtyFlag;

	//tell the one above that it is dirty
	if( GetController()!=NULL )
		GetController()->SetDirtyFlag( DF_DIRTY );
}

//delete all objects, that this object owns, if they are marked
//	for deletion
bool	Run::CleanUp( int &iPos )
{
	Bay		*pBay, *pOut, *pIn, *pFor, *pBack;
	int		i, j;
	bool	bBayDeleted, bReplaceHandrails;

	SetDirtyFlag( DF_CLEAN );

	bBayDeleted = false;

	for( i=0; (GetNumberOfBays()>0) && (i<GetNumberOfBays()); i++ )
	{
		pBay = GetBay(i);
		if( iPos>=0 )
			acedSetStatusBarProgressMeterPos(++iPos);

		switch( pBay->GetDirtyFlag() )
		{
		case( DF_DELETE ):
			if( !GetController()->IsDestructingController() &&
				pBay->GetCommittedProportion()!=COMMIT_NONE )
			{
				//This bay has components which are committed thus it cannot be deleted
				CString sMsg;
				sMsg.Format( _T("Bay%i constains one or more components which are committed,\n"), pBay->GetBayNumber() );
				sMsg+= _T("thus it cannot be deleted at this time!\n\n");
        sMsg+= _T("Note: To delete this bay firstly uncommit all the components\n");
				sMsg+= _T("of this bay, either through the BOM summary or via the\n");
				sMsg+= _T("'ClearCommitted' command.\n\n");
        sMsg+= _T("Hint:  The white components in the 3D view are committed!");
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox( NULL, sMsg, _T("Committed components Error"), MB_OK|MB_ICONEXCLAMATION );
				pBay->SetDirtyFlag(DF_CLEAN);
				pBay->RedrawPlanView();
			}
			else
			{
				bReplaceHandrails = true;
				pIn = NULL;
				pOut = NULL;
				pFor = NULL;
				pBack = NULL;
				//Buttress bays already have the handrail, so always replace them
				if( pBay->GetBayType()==BAY_TYPE_STAIRS ||
					pBay->GetBayType()==BAY_TYPE_LADDER )
				{
					intArray	iaBayNumber;
					iaBayNumber.RemoveAll();
					if( pBay->GetInner()!=NULL	&& pBay->GetInner()->GetDirtyFlag()!=DF_DELETE )
					{
						pIn = pBay->GetInner();
						iaBayNumber.Add( pBay->GetInner()->GetBayNumber() );
					}
					if( pBay->GetOuter()!=NULL	&& pBay->GetOuter()->GetDirtyFlag()!=DF_DELETE )
					{
						pOut = pBay->GetOuter();
						iaBayNumber.Add( pBay->GetOuter()->GetBayNumber() );
					}
					if( pBay->GetForward()!=NULL	&& pBay->GetForward()->GetDirtyFlag()!=DF_DELETE )
					{
						pFor = pBay->GetForward();
						iaBayNumber.Add( pBay->GetForward()->GetBayNumber() );
					}
					if( pBay->GetBackward()!=NULL	&& pBay->GetBackward()->GetDirtyFlag()!=DF_DELETE )
					{
						pBack = pBay->GetBackward();
						iaBayNumber.Add( pBay->GetBackward()->GetBayNumber() );
					}

					if( iaBayNumber.GetSize()>0 )
					{
						CString sErrMsg, sTemp;

						sTemp = _T("stair");
						if( pBay->GetBayType()==BAY_TYPE_LADDER )
							sTemp = _T("ladder");
						if( pBay->GetBayType()==BAY_TYPE_BUTTRESS )
							sTemp = _T("buttress");
						//This stair bay has an attached bay which is not going to be deleted
						if( iaBayNumber.GetSize()==1 )
						{
							sErrMsg.Format( _T("You have just deleted Bay%i, which is a %s bay!  This means that Bay%i\n"),
											pBay->GetBayNumber(), sTemp, iaBayNumber.GetAt(0) );
						}
						else
						{
							sErrMsg.Format( _T("You have just deleted Bay%i, which is a %s bay!\nThis means that\n"),
											pBay->GetBayNumber(), sTemp );
							sTemp.Format( _T("Bay%i"), iaBayNumber.GetAt(0) );
							for( j=1; j<iaBayNumber.GetSize()-1; j++ )
							{
								sTemp.Format( _T(", Bay%i"), iaBayNumber.GetAt(j) );
							}
							sTemp.Format( _T(" and Bay%i "), iaBayNumber.GetAt(j) );
						}
						sErrMsg+= _T("may now requires some hand rails.\n\n");
						sErrMsg+= _T("Would you like me to automatically add those handrails?");
						MessageBeep(MB_ICONQUESTION);
						if( MessageBox( NULL, sErrMsg, _T("Handrail warning"), MB_YESNO|MB_ICONQUESTION )==IDNO )
							bReplaceHandrails = false;
					}
				}

				if( !DeleteBays(i) )
				{
					;//assert( false );
				}

				if( bReplaceHandrails )
				{
					if( pIn!=NULL )
					{
						pIn->DeleteUnneededComponentsFromEmptyLifts( NORTH );
						pIn->AddHandrail( NORTH, true );
					}
					if( pOut!=NULL )
					{
						pOut->DeleteUnneededComponentsFromEmptyLifts( SOUTH );
						pOut->AddHandrail( SOUTH, true );
					}
					if( pFor!=NULL )
					{
						pFor->DeleteUnneededComponentsFromEmptyLifts( WEST );
						pFor->AddHandrail( WEST, true );
					}
					if( pBack!=NULL )
					{
						pBack->DeleteUnneededComponentsFromEmptyLifts( EAST );
						pBack->AddHandrail( EAST, true );
					}
				}

				bBayDeleted=true;
				i--;	//we have just deleted the element, so don't increment i;
			}
			break;
		case( DF_DIRTY ):
			pBay->CleanUp();
			if( pBay->GetDirtyFlag()!=DF_CLEAN )
			{
				//it is not finished yet, do it again
				i--;
			}
			break;
		case( DF_CLEAN ):
		default:
			//Nothing to do!
			break;
		}
	}

	if( GetNumberOfBays()<=0 )
		SetDirtyFlag( DF_DELETE );

	//tell the controller that a bay has been deleted!
	return bBayDeleted;
}

void Run::MoveSchematic(Matrix3D Transform, bool bStore, int iFixedDirection/*=FIXED_DIRECTION_BOTH*/ )
{
	int		i, iSize, iBayID;
	Bay		*pB;
	bool	bFound;
	RunList	InnerRuns, OuterRuns;

	;//assert( GetRunID()>=0 );
	InnerRuns.RemoveAll();
	OuterRuns.RemoveAll();

	//store it
	if( bStore )
	{
		SetSchematicTransform( Transform*GetSchematicTransform() );
	}

	iSize = GetNumberOfBays();
	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pB=GetBay(iBayID);
		pB->MoveSchematic( Transform, false );

		if( iFixedDirection!=FIXED_DIRECTION_NEITHER )
		{
			if( (iFixedDirection==FIXED_DIRECTION_BOTH ||
				iFixedDirection==FIXED_DIRECTION_INNER ) &&
				pB->GetInner()!=NULL )
			{
				;//assert( pB->GetInner()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( i=0; i<InnerRuns.GetSize(); i++ )
				{
					if( InnerRuns[i]==pB->GetInner()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					InnerRuns.Add( pB->GetInner()->GetRunPointer() );
				}
			}

			if( (iFixedDirection==FIXED_DIRECTION_BOTH ||
				iFixedDirection==FIXED_DIRECTION_OUTER ) &&
				pB->GetOuter()!=NULL )
			{
				;//assert( pB->GetOuter()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( i=0; i<OuterRuns.GetSize(); i++ )
				{
					if( OuterRuns[i]==pB->GetOuter()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					OuterRuns.Add( pB->GetOuter()->GetRunPointer() );
				}
			}
		}
	}

	for( i=0; i<InnerRuns.GetSize(); i++ )
	{
		InnerRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_INNER );
	}

	for( i=0; i<OuterRuns.GetSize(); i++ )
	{
		OuterRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_OUTER );
	}
}

void Run::SetSchematicTransform(Matrix3D Transform)
{
	m_SchematicTransform = Transform;
}

Matrix3D Run::GetSchematicTransform() const
{
	return m_SchematicTransform;
}

Matrix3D Run::UnMoveSchematic()
{
	Matrix3D	Original, Inverse;

	//store the original transformation for later use
	Original = GetSchematicTransform();

	//we need to move the Bay back to its original position
	Inverse = GetSchematicTransform();
	Inverse.invert();

	MoveSchematic( Inverse, true );

	//return the original matrix
	return Original;
}

void Run::UpdateSchematicView( bool bOnlyUpdateVisited/*=false*/, double dProgress/*=-1.00*/, double dProgSpan/*=-1.00*/ )
{
	int		iBayID, iSize;
	Bay		*pBay;
	bool	bShowProgBar;
	double	dIncr;
	CString	sProgressPrompt;

	bShowProgBar = dProgress>0.00;

  // Commented by ~SJ~ 03.08.2007, since it has no effect apart from 
  // showing a Runtime Error for progress bars. Someone loves ;//asserts.
	//;//assert( dProgress+dProgSpan<=100.00 );

	iSize = GetNumberOfBays();
	if( bShowProgBar && iSize>0 )
	{
		dIncr = dProgSpan/(double)iSize;
	}

	for( iBayID=0; iBayID<iSize; iBayID++ )
	{
		pBay=GetBay(iBayID);

		if( bShowProgBar )
		{
			dProgress+= dIncr;
			sProgressPrompt.Format( _T("Bay %i - Drawing"), pBay->GetBayNumber() );
			acedSetStatusBarProgressMeter( sProgressPrompt, 0, 100 );
			acedSetStatusBarProgressMeterPos( (int)dProgress );
		}

		if( bOnlyUpdateVisited )
		{
			if( pBay->HasBeenVisited() )
				pBay->UpdateSchematicView();
		}
		else
		{
			pBay->UpdateSchematicView();
		}
	}
}

void Run::SetBaysWithBracing( bool bRemoveExisting/*=true*/ )
{
	Bay			*pBay;
	int			i, j, iBay, iSize, jSize, iSchematicLift;
	CString		sProgressPrompt;
	intArray	iaBaysWithBracing;
	
	iSize = GetNumberOfBays();
	CreatePascalsTriangle( GetRunTemplate()->GetBaysPerBrace(), iSize, iaBaysWithBracing );

	//remove all existing bracing on all Bays
	if( bRemoveExisting )
	{
		for( i=0; i<iSize; i++ )
		{
			pBay = GetBay( i );
			pBay->DeleteBrace( NORTH );
			pBay->DeleteBrace( SOUTH );
			pBay->DeleteBrace( EAST );
			pBay->DeleteBrace( WEST );
		}
	}

	//add bracing to those bays selected from the pascal's triangle
	jSize = iaBaysWithBracing.GetSize();
	for( j=0; j<jSize; j++ )
	{
		iBay = iaBaysWithBracing.GetAt( j );
		pBay = GetBay( iBay );
		;//assert( pBay!=NULL );

		if( GetController()->IsWallOffsetFromLowestHopup() )
			iSchematicLift = pBay->GetLowestHopupLiftID();
		else
			iSchematicLift = pBay->GetNumberOfLifts()-1;

		////////////////////////////////////////////////////////////
		//North
		if( pBay->GetOuter()==NULL &&
			!pBay->GetLift(iSchematicLift)->HasComponentOfTypeOnSide( CT_STAGE_BOARD, NORTH ) &&
			!pBay->LapboardCrossesSideDoes( NORTH ) )
		{
			pBay->CreateBracingArrangment( NORTH, MT_STEEL, true );
		}

		////////////////////////////////////////////////////////////
		//East
		if( pBay->GetForward()==NULL &&
			!pBay->GetLift(iSchematicLift)->HasComponentOfTypeOnSide( CT_STAGE_BOARD, EAST ) &&
			!pBay->LapboardCrossesSideDoes( EAST ) )
		{
			pBay->CreateBracingArrangment( EAST, MT_STEEL, true );
		}

		////////////////////////////////////////////////////////////
		//West
		if( pBay->GetBackward()==NULL &&
			!pBay->GetLift(iSchematicLift)->HasComponentOfTypeOnSide( CT_STAGE_BOARD, WEST ) &&
			!pBay->LapboardCrossesSideDoes( WEST ) )
		{
			Run	*pPrevRun;
			Bay	*pPrevBay;
			pPrevRun = GetController()->GetRun( GetRunID()-1 );
			pPrevBay = NULL;
			if( pPrevRun!=NULL )
			{
				pPrevBay = pPrevRun->GetBay( pPrevRun->GetNumberOfBays()-1 );
			}

			MillsType mt;
			mt = MILLS_TYPE_NONE;
			if( pPrevBay!=NULL )
			{
				mt = pPrevBay->GetMillsSystemType();
			}

			if( mt!=MILLS_TYPE_CONNECT_SSE && mt!=MILLS_TYPE_CONNECT_NNE )
				pBay->CreateBracingArrangment( WEST, MT_STEEL, true );
			
		}
	}

	//redraw the schematic?
	if( bRemoveExisting )
	{
		UpdateSchematicView();
	}
}

void Run::SetBaysWithTies( bool bRemoveExisting/*=true*/, bool bRemoveLastTie/*=false*/ )
{
	Bay			*pBay;
	int			iBayID, j, iBay, iSize, jSize;
	double		dHeight, dStandardsHeight, dTieLength;
	intArray	iaBaysWithTies;
	
	iSize = GetNumberOfBays();
	CreatePascalsTriangle( GetRunTemplate()->GetBaysPerTie(), iSize, iaBaysWithTies );

	///////////////////////////////////////////////////////////////////
	//Pascal's triangle states that the last bay must have a tie,
	//	which is wrong we need to remove the tie from the last bay
	for( int i=iaBaysWithTies.GetSize()-1; i>=0; i-- )
	{
		iBayID = iaBaysWithTies.GetAt(i);
		if( iBayID==iSize-1 )
		{
			pBay = GetBay( iBayID );
			//does this bay have a southern lapboard?
			if( pBay!=NULL && pBay->LapboardCrossesSideDoes( SOUTH ) )
			{
				iaBaysWithTies.RemoveAt(i);
			}
			break;
		}
	}

	//under certain circumstances we need to remove the last tie!
	if( bRemoveLastTie )
		iaBaysWithTies.RemoveAt(iaBaysWithTies.GetSize()-1);

	///////////////////////////////////////////////////////////////////
	//RemoveAll existing Ties, but not Manual ties
	if( bRemoveExisting )
	{
		for( iBayID=0; iBayID<iSize; iBayID++ )
		{
			pBay = GetBay( iBayID );
			;//assert( pBay!=NULL );
			pBay->DeleteTieFromSide( NNE );
			pBay->DeleteTieFromSide( ENE );
			pBay->DeleteTieFromSide( ESE );
			pBay->DeleteTieFromSide( SSE );
			pBay->DeleteTieFromSide( SSW );
			pBay->DeleteTieFromSide( WSW );
			pBay->DeleteTieFromSide( WNW );
			pBay->DeleteTieFromSide( NNW );
		}
	}

	double			dNewWidth, dNewLenght, dNewWidthActual, dNewLengthActual;
	TieTypesEnum	eTieType;
	jSize = iaBaysWithTies.GetSize();
	for( j=0; j<jSize; j++ )
	{
		iBay = iaBaysWithTies.GetAt( j );
		pBay = GetBay( iBay );

		eTieType = GetTieType();
		if( eTieType==TIE_TYPE_BUTTRESS_12 ||
			eTieType==TIE_TYPE_BUTTRESS_18 ||
			eTieType==TIE_TYPE_BUTTRESS_24 )
		{
			if( pBay->GetOuter()!=NULL )
			{
				//JSB todo 20000705 - maybe we should give a warning here
				//Delete the existing bay!
				pBay->GetOuter()->GetRunPointer()->DeleteBays( pBay->GetOuter()->GetID(), 1, false );
			}

			switch( eTieType )
			{
			case( TIE_TYPE_BUTTRESS_12 ):
				dNewWidth		= COMPONENT_LENGTH_1200;
				dNewWidthActual	= pBay->GetBayDimensionActual( COMPONENT_LENGTH_1200, false );
				break;
			case( TIE_TYPE_BUTTRESS_18 ):
				dNewWidth		= COMPONENT_LENGTH_1800;
				dNewWidthActual	= pBay->GetBayDimensionActual( COMPONENT_LENGTH_1800, false );
				break;
			case( TIE_TYPE_BUTTRESS_24 ):
				dNewWidth		= COMPONENT_LENGTH_2400;
				dNewWidthActual	= pBay->GetBayDimensionActual( COMPONENT_LENGTH_2400, false );
				break;
			default:
				;//assert( false );
			}
			dNewLenght			= pBay->GetBayLength();
			dNewLengthActual	= pBay->GetBayLengthActual();
			Bay	*pButtressBay;
			pButtressBay = GetController()->InsertAndRedimensionBay( pBay,
						NORTH, dNewWidth, dNewLenght, dNewWidthActual, dNewLengthActual );
			if( pButtressBay!=NULL )
			{
				//since we are at run level, the run transform is not copied
//				pButtressBay->Move( pBay->GetTransform(), true );
//				pButtressBay->MoveSchematic( pBay->GetSchematicTransform(), true );

				//Convert to a buttress bay
				pButtressBay->ChangeToButtressBay();
				pButtressBay->UpdateSchematicView();
				bRemoveExisting = true;
			}
		}
		else if( pBay->GetInner()==NULL )
		{
			dHeight = GetTiesVerticallyEvery();
			dStandardsHeight = pBay->GetHeightOfStandards( CNR_SOUTH_EAST );

			dTieLength = pBay->GetSmallestRequiredWallTieLength(SSE);
			pBay->GetTemplate()->SetSSETie(true);

			if( dHeight>=dStandardsHeight )
			{
				//we need at least one tie
				SetTiesIfPossible( pBay, dStandardsHeight, dTieLength, MT_STEEL );
			}
			else
			{
				while( dHeight<dStandardsHeight )
				{
					SetTiesIfPossible( pBay, dHeight, dTieLength, MT_STEEL );
					dHeight+= GetTiesVerticallyEvery();
				}
			}
		}
	}

	//redraw the schematic?
	if( bRemoveExisting )
	{
		UpdateSchematicView();
	}

}

void Run::CreatePascalsTriangle(int iBaysPerItem, int iTotalNumberOfBays, intArray &iaBays)
{
	int		iItemedBay, iBay, iNumerator, iInternalBays, iInternamItems;
	double	dDenominator, dMulitplier;

	iInternalBays	= iTotalNumberOfBays-2;
	iaBays.RemoveAll();
	if( iBaysPerItem>0 )
	{
		iItemedBay = 0;
		iaBays.Add( iItemedBay );
		if( (iBaysPerItem>0) && (iInternalBays>0) )
		{
			//round down
			iInternamItems	= (int)((double)iInternalBays/(double)iBaysPerItem);
			dDenominator	= (double)iInternamItems + 1.00;

			for( iNumerator=1; iNumerator<=iInternamItems; iNumerator++)
			{
				dMulitplier	= (double)iNumerator/dDenominator;
				iBay		= (int)(dMulitplier*(double)iInternalBays);
				iItemedBay = iBay+1;
				iaBays.Add( iItemedBay );
			}
		}

		//The last bay also requires an item, but no point in adding the first bay again!
		if( (iTotalNumberOfBays-1)>0 )
		{
			iItemedBay = iTotalNumberOfBays-1;
			iaBays.Add( iItemedBay );
		}
	}
}

RunTemplate * Run::GetRunTemplate() const
{
	return m_pRTplt;
}


///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void Run::Serialize(CArchive &ar)
{
	int		iTieType, iBayNumber, iNumberOfBays;
	Bay		*pBay, *pLastBay;
	bool	bDummy;
	BOOL	BTemp;
	double	dTemp;
	CString	sMsg, sTemp;
	Point3D	pt;

	if (ar.IsStoring())    // Store Object?
	{
		ar << RUN_VERSION_LATEST;

		//RUN_VERSION_1_0_1
		bDummy = GetUseLongerBracing();
		STORE_bool_IN_AR(bDummy);

		//RUN_VERSION_1_0_0
		//Point3Ds			
		ar << GetPointEnd().x;
		ar << GetPointEnd().y;
		ar << GetPointEnd().z;

		ar << GetPointStart().x;
		ar << GetPointStart().y;
		ar << GetPointStart().z;

		m_pRTplt->Serialize(ar); //The Run template

		//Transformation maticies
		m_SchematicTransform.Serialize(ar);
		m_Transform.Serialize(ar);
		
		iTieType = GetTieType();
		ar << iTieType;				//Type of tie used
		ar << GetTiesVerticallyEvery(); //How far apart tie are spaced vertically

		// Bay List Storage - These are the bays that are owned by this run
		if( !GetController()->SaveSelectedOnly() )
		{
			iNumberOfBays = GetNumberOfBays();			// store number of bays
		}
		else
		{
			iNumberOfBays = 0;
			for( iBayNumber=0; iBayNumber<GetNumberOfBays(); iBayNumber++ )
			{
				pBay = m_baBays.GetAt(iBayNumber);
				if( pBay->IsBaySelected() )
					iNumberOfBays++;
			}
		}
		ar << iNumberOfBays;
		if (iNumberOfBays>0)
		{
			for( iBayNumber=0; iBayNumber < GetNumberOfBays(); iBayNumber++ )
			{
				pBay = m_baBays.GetAt(iBayNumber);
				if( !GetController()->SaveSelectedOnly() )
				{
					pBay->Serialize(ar);				// store each bay
				}
				else
				{
					if( pBay->IsBaySelected() )
					{
						pBay->Serialize(ar);				// store each bay
					}
				}
			}
		}
		// End Bay List Storage
	}
	else					// or Load Object?
	{
		SetUseLongerBracing(true);
		SetLongerBracingValues();

		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case RUN_VERSION_1_0_1 :
			LOAD_bool_IN_AR(bDummy);
			SetUseLongerBracing(bDummy);
			//fallthrough

		case RUN_VERSION_1_0_0 :
			//  the following are set by the controller during retrieval serialization
			//	int				m_iRunID; //This is the unique run ID assigned by the controller
			//	Controller		*m_pController; // //pointer to the controller object

			/////////////////////////////////////////////////////////////////////
			//Point3Ds			
			ar >> pt.x;
			ar >> pt.y;
			ar >> pt.z;
			SetPointEnd( pt );
	
			ar >> pt.x;
			ar >> pt.y;
			ar >> pt.z;
			SetPointStart( pt );

			/////////////////////////////////////////////////////////////////////
			//Template
//				m_pRTplt = new RunTemplate();
			m_pRTplt->Serialize(ar);

			/////////////////////////////////////////////////////////////////////
			//Transformation maticies
			m_SchematicTransform.Serialize(ar);
			m_Transform.Serialize(ar);

			/////////////////////////////////////////////////////////////////////
			//ties
			ar >> iTieType;				//Type of tie used
			SetTieType( (TieTypesEnum)iTieType );
			ar >> dTemp; //How far apart tie are spaced vertically
			SetTiesVerticallyEvery( dTemp );

			/////////////////////////////////////////////////////////////////////
			//Bay List Retrieval - These are the bays that are owned by this run
			pLastBay = NULL;
			ar >> iNumberOfBays;
			if (iNumberOfBays > 0)
			{
				for( iBayNumber=0; iBayNumber < iNumberOfBays; iBayNumber++ )
				{
					pBay = new Bay();
					pBay->SetAllowDraw( false );

					//Set the bays system to the same as the rest of the array
					//	otherwise it cannot be added to the list of bays.
					//	This value will be overridden by the serialize function
					if( iBayNumber>0 )
						pBay->SetSystem( m_baBays.GetSystem() );

					AddBay(pBay, false, false);
					pBay->Serialize(ar);	// load each bay

					//check that the system has remained the same
					;//assert( pBay->GetSystem()==m_baBays.GetSystem() );

					pBay->SetForwardDumb( NULL );
					pBay->SetBackwardDumb( NULL );
					if( pLastBay!=NULL )	// set bay forward and backward pointers
					{
						pBay->SetBackwardDumb(pLastBay);
						pLastBay->SetForwardDumb(pBay);
					} 
					pLastBay = pBay;
				}
			}
			// End Bay List Retrieval

			/////////////////////////////////////////////////////////////////////
			//Bay Pointers and drawing
			for( iBayNumber=0; iBayNumber < iNumberOfBays; iBayNumber++ )
			{
				pBay = GetBay(iBayNumber);
				;//assert( pBay!=NULL );
//					pBay->ReapplyBayPointers( false );
				pBay->SetAllowDraw( true );
				if( pBay->GetNumberOfLifts()<=0 )
				{
					;//assert( false );
					CString sMsg;
					sMsg.Format( _T("Bay %i is reporting that it contains no lifts!\n"), pBay->GetBayNumber() );
					sMsg+= _T("In order to continue opening this file, I must delete\n");
					sMsg+= _T("this bay!  You will have to recreate this bay manually.\n\n");
          sMsg+= _T("Note: The remaining bays will be renumbered after this action.");
					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox( NULL, sMsg, _T("Bay Open Error"), MB_OK|MB_ICONEXCLAMATION );
					pBay->SetDirtyFlag( DF_DELETE );
					continue;
				}
				if( !GetController()->IsInsertingBlock() )
					pBay->Redraw();
			}

			break;
		default:
			;//assert( false );
			if( uiVersion>RUN_VERSION_LATEST )
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
      sMsg+= _T("Class: Run.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), RUN_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

void Run::ReIDSubsequentBays(int iBayIdToStartAt)
{
	int i, iSize;
	Bay	*pBay;

	///////////////////////////////////////////////////////////////
	//re-ID subsequent bays
	iSize = GetNumberOfBays();
	for( i=iBayIdToStartAt; i<iSize; i++ )
	{
		pBay = GetBay( i );
		pBay->SetID(i);
	}
}

void Run::MoveSubsequentBays(int iBayToStartWith, Matrix3D Transform, bool bStore, bool bMoveConnectedRuns/*=true*/ )
{
	int		i, j, iSize;
	Bay		*pB;
	bool	bFound;
	RunList	InnerRuns, OuterRuns;

	InnerRuns.RemoveAll();
	OuterRuns.RemoveAll();

	///////////////////////////////////////////////////////////////
	//Move the subsequent bays
	iSize = GetNumberOfBays();
	for( i=iBayToStartWith; i<iSize; i++ )
	{
		pB = GetBay( i );
		pB->Move( Transform, bStore );
		pB->MoveSchematic( Transform, bStore );

		if( bMoveConnectedRuns )
		{
			if( pB->GetInner()!=NULL )
			{
				;//assert( pB->GetInner()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( j=0; j<InnerRuns.GetSize(); j++ )
				{
					if( InnerRuns[j]==pB->GetInner()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					InnerRuns.Add( pB->GetInner()->GetRunPointer() );
				}
			}

			if( pB->GetOuter()!=NULL )
			{
				;//assert( pB->GetOuter()->GetRunPointer()!=NULL );

				/////////////////////////////////////////////////////
				//set the inner pointer so we can get a run pointer
				bFound = false;
				for( j=0; j<OuterRuns.GetSize(); j++ )
				{
					if( OuterRuns[j]==pB->GetOuter()->GetRunPointer() )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
				{
					OuterRuns.Add( pB->GetOuter()->GetRunPointer() );
				}
			}
		}
	}

	for( i=0; i<InnerRuns.GetSize(); i++ )
	{
		;//assert( bMoveConnectedRuns );
		InnerRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_INNER );
		InnerRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_INNER );
	}

	for( i=0; i<OuterRuns.GetSize(); i++ )
	{
		;//assert( bMoveConnectedRuns );
		OuterRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_OUTER );
		OuterRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_OUTER );
	}
}

void Run::MovePreviousBays(int iBayToStartWith, Matrix3D Transform, bool bStore)
{
	int		i, j, iSize;
	Bay		*pBay;
	bool	bFound;
	RunList	InnerRuns, OuterRuns;

	InnerRuns.RemoveAll();
	OuterRuns.RemoveAll();

	///////////////////////////////////////////////////////////////
	//Move the subsequent bays
	iSize = GetNumberOfBays();
	for( i=iBayToStartWith; i>=0; i-- )
	{
		pBay = GetBay( i );
		pBay->Move( Transform, bStore );
		pBay->MoveSchematic( Transform, bStore );

		if( pBay->GetInner()!=NULL )
		{
			;//assert( pBay->GetInner()->GetRunPointer()!=NULL );

			/////////////////////////////////////////////////////
			//set the inner pointer so we can get a run pointer
			bFound = false;
			for( j=0; j<InnerRuns.GetSize(); j++ )
			{
				if( InnerRuns[j]==pBay->GetInner()->GetRunPointer() )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				InnerRuns.Add( pBay->GetInner()->GetRunPointer() );
			}
		}

		if( pBay->GetOuter()!=NULL )
		{
			;//assert( pBay->GetOuter()->GetRunPointer()!=NULL );

			/////////////////////////////////////////////////////
			//set the inner pointer so we can get a run pointer
			bFound = false;
			for( j=0; j<OuterRuns.GetSize(); j++ )
			{
				if( OuterRuns[j]==pBay->GetOuter()->GetRunPointer() )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				OuterRuns.Add( pBay->GetOuter()->GetRunPointer() );
			}
		}
	}

	for( i=0; i<InnerRuns.GetSize(); i++ )
	{
		InnerRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_INNER );
		InnerRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_INNER );
	}

	for( i=0; i<OuterRuns.GetSize(); i++ )
	{
		OuterRuns[i]->Move( Transform, bStore, FIXED_DIRECTION_OUTER );
		OuterRuns[i]->MoveSchematic( Transform, bStore, FIXED_DIRECTION_OUTER );
	}
}


/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Run.cpp $
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 20/12/00   Time: 3:19p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the Corner hopup problem, this should hopefully be the final 1.2
 * release
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:11a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 6/12/00    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * About to build 1.1n
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 6/10/00    Time: 1:29p
 * Updated in $/Meccano/Stage 1/Code
 * About to build R1.1e - This should have all of Mark's current small
 * bugs fixed, ready for the second release to Perth (still awaiting
 * feedback from Perth from the first one we sent them)  This is another
 * version for mark to test, and is a candidate for Perth release
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:50p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 4/08/00    Time: 4:23p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed most of the buttress bugs
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 27/07/00   Time: 1:07p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 8p
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 21/07/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly completed the SetDecks to Levels bug
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 17/07/00   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the 500mm star separation to 495.3
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 7/07/00    Time: 7:50a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 28/06/00   Time: 1:27p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create a seperate Toolbar project
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 19/06/00   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 15/06/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 2/06/00    Time: 3:02p
 * Updated in $/Meccano/Stage 1/Code
 * need to find why tietubes are removing hopups
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 4/05/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 11/04/00   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 1/03/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 23/02/00   Time: 2:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:37p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 20/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 4/01/00    Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:58p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 14/12/99   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to build Beta11
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 9/12/99    Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 6/12/99    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * See upto here comment
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 6/12/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * Got everything working as well as I had them at home
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 18/11/99   Time: 3:45p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on the Run Properties dialog
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 12/11/99   Time: 3:47p
 * Updated in $/Meccano/Stage 1/Code
 * Changing the Bay Avoidance mechanism
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 11/11/99   Time: 3:56p
 * Updated in $/Meccano/Stage 1/Code
 * Just completed:
 * 491 - Inertbay needs better error msg
 * 408 - Can only inert one bay at a timeinsert
 * 467 - Inert bay north has offset
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 27/10/99   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 27/10/99   Time: 2:49p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Mesh Guard now working
 * 2) Ties nearly working
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 22/10/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Birdcaging pointers now fully operational
 * 2) Birdcaging now uses pascal's triangle for End of runs
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 15/10/99   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 5/10/99    Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 1/10/99    Time: 1:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 22/09/99   Time: 2:00p
 * Updated in $/Meccano/Stage 1/Code
 * Insert Bay finally working
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 21/09/99   Time: 5:25p
 * Updated in $/Meccano/Stage 1/Code
 * Almost finished the insert bay
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 9/09/99    Time: 12:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Dar          Date: 9/09/99    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 35  *****************
 * User: Dar          Date: 9/07/99    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * more serialize shit
 * 
 * *****************  Version 34  *****************
 * User: Dar          Date: 9/07/99    Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/23/99    Time: 11:43a
 * Updated in $/Meccano/Stage 1/Code
 * Insert Lift function completed
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/23/99    Time: 9:48a
 * Updated in $/Meccano/Stage 1/Code
 * Delete (so far)  is now working correctly
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Dar          Date: 8/12/99    Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * fixed double type for bracing every and ties every bay to ints
 * 
 * *****************  Version 15  *****************
 * User: Dar          Date: 8/12/99    Time: 2:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/10/99    Time: 12:19p
 * Updated in $/Meccano/Stage 1/Code
 * Added the Entity class
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 29/07/99   Time: 10:05
 * Updated in $/Meccano/Stage 1/Code
 * I have now added a test routine to the controller which calls the
 * _T("LiftList.Test") function
 * I have added operator functions to the Run class
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 28/07/99   Time: 15:55
 * Updated in $/Meccano/Stage 1/Code
 * Continueing with Run class
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 *******************************************************************************/



bool Run::JoinForward( int iBayID, Bay *pSelectedBay, Bay *pNewBay )
{
	int			iNewBayID;
	Bay			*pNextBay, *pPreviousBay;
	Vector3D	Vector;
	Matrix3D	Transform, OriginalRunTrans;

	////////////////////////////////////////////////////////////////////
	//We will need to convert the list of Bays from bay numbers
	//	to bays, since all the number system will be screwed up
	int			i, iBay;
	Bay			*pBay;
	BayList		ba3D, baMatrix;
	intArray	*pia3D, *piaMatrix;	
	pia3D		= GetController()->GetSelected3DBays();
	piaMatrix	= GetController()->GetSelectedMatrixBays();
	ba3D.RemoveAll();
	for( i=0; i<pia3D->GetSize(); i++ )
	{
		iBay = pia3D->GetAt(i);
		pBay = GetController()->GetBayFromBayNumber(iBay);
		if( pBay!=NULL )
			ba3D.Add(pBay);
	}
	baMatrix.RemoveAll();
	for( i=0; i<piaMatrix->GetSize(); i++ )
	{
		iBay = pia3D->GetAt(i);
		pBay = GetController()->GetBayFromBayNumber(iBay);
		if( pBay!=NULL )
			baMatrix.Add(pBay);
	}

	Vector.set( 0.00, 0.00, 0.00 );

	////////////////////////////////////////////////////////////////////
	//runs must all have the same width, etc.
	if( pSelectedBay->GetBayWidth()!=pNewBay->GetBayWidth() )
	{
		pNewBay->SetBayWidth( pSelectedBay->GetBayWidth() );
/*		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( "\nCannot insert Bay of different width east of Bay %i!", pSelectedBay->GetBayNumber() );
		return false;
*/	}

	if( pSelectedBay->GetOuter()!=NULL && pSelectedBay->GetForward()!=NULL && pSelectedBay->GetForward()->GetOuter()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot insert Bay between Bays %i and %i, since both these bays have northern bays"),
						pSelectedBay->GetBayNumber(), pSelectedBay->GetBayNumber()+1 );
		return false;
	}
	if( pSelectedBay->GetInner()!=NULL && pSelectedBay->GetForward()!=NULL && pSelectedBay->GetForward()->GetInner()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot insert Bay between Bays %i and %i, since both these bays have southern bays"),
						pSelectedBay->GetBayNumber(), pSelectedBay->GetBayNumber()+1 );
		return false;
	}

	iNewBayID = iBayID+1;
	if( iNewBayID==GetNumberOfBays() )
	{
		AddBay( pNewBay );
	}
	else
	{
		//////////////////////////////////////////////////
		//Insert into array
		m_baBays.InsertAt( iNewBayID, pNewBay );
		pNewBay->SetID( iNewBayID );

		//////////////////////////////////////////////////
		//setup relationship with next bay
		if( iNewBayID+1<GetNumberOfBays() )
			pNextBay = GetBay(iNewBayID+1);
		if( pNextBay!=NULL )
		{
			pNextBay->SetBackward( pNewBay );
			pNewBay->SetForward( pNextBay );
		}

		//////////////////////////////////////////////////
		//setup relationship with previous bay
		if( iNewBayID-1>=0 )
			pPreviousBay = GetBay(iNewBayID-1);
		if( pPreviousBay!=NULL )
		{
			pNewBay->SetBackward( pPreviousBay );
			pPreviousBay->SetForward( pNewBay );
		}

		if( pNextBay->GetOuter()!=NULL )
		{
			if( pNextBay->GetOuter()->GetBackward()!=NULL )
			{
				SplitRun( pNextBay->GetOuter()->GetBackward()->GetID() );
			}
		}

		if( pNextBay->GetInner()!=NULL )
		{
			if( pNextBay->GetInner()->GetBackward()!=NULL )
			{
				SplitRun( pNextBay->GetInner()->GetBackward()->GetID() );
			}
		}

		//////////////////////////////////////////////////
		//Other pointers
		pNewBay->SetInner( NULL );
		pNewBay->SetOuter( NULL );
		pNewBay->SetRunPointer( this );

		ReIDSubsequentBays( iNewBayID );
	}

	pNewBay->UpdateSchematicView();

	///////////////////////////////////////////////////
	//Move
	OriginalRunTrans = UnMoveSchematic();
	Transform = pSelectedBay->GetSchematicTransform();
	pNewBay->GetTemplate()->MoveSchematic( Transform, true );

//	Transform = pSelectedBay->GetTransform();
//	Transform = pSelectedBay->GetRunPointer()->GetTransform() * Transform;
	pNewBay->Move( pSelectedBay->GetTransform(), true );

	///////////////////////////////////////////////////////
	//Move the new bay and subsequent bays to thier correct postions
	Vector.set( pSelectedBay->GetBayLengthActual(), 0.00, 0.00 );
	Transform.setToTranslation( Vector );
	MoveSubsequentBays( iNewBayID, Transform, true );

	MoveSchematic( OriginalRunTrans, true );

	////////////////////////////////////////////////////////////////////
	//Restore the bay numbers
	pia3D->RemoveAll();
	for( i=0; i<ba3D.GetSize(); i++ )
	{
		pBay = ba3D.GetAt(i);
		if( pBay!=NULL )
			pia3D->Add( pBay->GetBayNumber() );
	}
	piaMatrix->RemoveAll();
	for( i=0; i<baMatrix.GetSize(); i++ )
	{
		pBay = baMatrix.GetAt(i);
		if( pBay!=NULL )
			piaMatrix->Add( pBay->GetBayNumber() );
	}


	return true;
}

bool Run::JoinBackward(int iBayID, Bay *pSelectedBay, Bay *pNewBay)
{
	int			iNewBayID;
	Bay			*pNextBay, *pPreviousBay;
	Vector3D	Vector;
	Matrix3D	Transform, OriginalRunTrans;

	////////////////////////////////////////////////////////////////////
	//We will need to convert the list of Bays from bay numbers
	//	to bays, since all the number system will be screwed up
	int			i, iBay;
	Bay			*pBay;
	BayList		ba3D, baMatrix;
	intArray	*pia3D, *piaMatrix;	
	pia3D		= GetController()->GetSelected3DBays();
	piaMatrix	= GetController()->GetSelectedMatrixBays();
	ba3D.RemoveAll();
	for( i=0; i<pia3D->GetSize(); i++ )
	{
		iBay = pia3D->GetAt(i);
		pBay = GetController()->GetBayFromBayNumber(iBay);
		if( pBay!=NULL )
			ba3D.Add(pBay);
	}
	baMatrix.RemoveAll();
	for( i=0; i<piaMatrix->GetSize(); i++ )
	{
		iBay = pia3D->GetAt(i);
		pBay = GetController()->GetBayFromBayNumber(iBay);
		if( pBay!=NULL )
			baMatrix.Add(pBay);
	}

	if( pSelectedBay->GetBayWidth()!=pNewBay->GetBayWidth() )
	{
		pNewBay->SetBayWidth( pSelectedBay->GetBayWidth() );
/*		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( "\nCannot insert Bay of different width west of Bay %i!", pSelectedBay->GetBayNumber() );
		return false;
*/	}

	if( pSelectedBay->GetOuter()!=NULL && pSelectedBay->GetBackward()!=NULL && pSelectedBay->GetBackward()->GetOuter()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot insert Bay between Bays %i and %i, since both these bays have northern bays"),
						pSelectedBay->GetBayNumber()-1, pSelectedBay->GetBayNumber() );
		return false;
	}
	if( pSelectedBay->GetInner()!=NULL && pSelectedBay->GetBackward()!=NULL && pSelectedBay->GetBackward()->GetInner()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot insert Bay between Bays %i and %i, since both these bays have southern bays"),
						pSelectedBay->GetBayNumber()-1, pSelectedBay->GetBayNumber() );
		return false;
	}

	//////////////////////////////////////////////////
	//Insert into array
	iNewBayID = iBayID;
	m_baBays.InsertAt( iNewBayID, pNewBay );
	pNewBay->SetID( iNewBayID );

	//////////////////////////////////////////////////
	//setup relationship with next bay
	//we have just inserted the new Bay before an existing bay, so the new bay
	//	must, by definition have a next bay!
	;//assert( iNewBayID+1<GetNumberOfBays() );
	pNextBay = GetBay(iNewBayID+1);
	;//assert( pNextBay!=NULL );
	pNewBay->SetForward( pNextBay );
	pNextBay->SetBackward( pNewBay );

	//////////////////////////////////////////////////
	//setup relationship with previous bay
	pPreviousBay = NULL;
	if( iNewBayID-1>=0 )
		pPreviousBay = GetBay(iNewBayID-1);
	if( pPreviousBay!=NULL )
	{
		pNewBay->SetBackward( pPreviousBay );
		pPreviousBay->SetForward( pNewBay );
	}

	//////////////////////////////////////////////////
	//Other pointers
	pNewBay->SetInner( NULL );
	pNewBay->SetOuter( NULL );
	pNewBay->SetRunPointer( this );

	ReIDSubsequentBays( iNewBayID );

	pNewBay->UpdateSchematicView();

	OriginalRunTrans = UnMoveSchematic();
	Transform = pSelectedBay->GetSchematicTransform();
//	Transform = pSelectedBay->GetRunPointer()->GetSchematicTransform() * Transform;
	pNewBay->GetTemplate()->MoveSchematic( Transform, true );

	Transform = pSelectedBay->GetTransform();
	Transform = pSelectedBay->GetRunPointer()->GetTransform() * Transform;
	pNewBay->Move( pSelectedBay->GetTransform(), true );

	///////////////////////////////////////////////////////
	//Move the new bay and subsequent bays to thier correct postions
	Vector.set( -1.00*pNewBay->GetBayLengthActual(), 0.00, 0.00 );
	Transform.setToTranslation( Vector );
	MovePreviousBays( iNewBayID, Transform, true );

	MoveSchematic( OriginalRunTrans, true );

	////////////////////////////////////////////////////////////////////
	//Restore the bay numbers
	pia3D->RemoveAll();
	for( i=0; i<ba3D.GetSize(); i++ )
	{
		pBay = ba3D.GetAt(i);
		if( pBay!=NULL )
			pia3D->Add( pBay->GetBayNumber() );
	}
	piaMatrix->RemoveAll();
	for( i=0; i<baMatrix.GetSize(); i++ )
	{
		pBay = baMatrix.GetAt(i);
		if( pBay!=NULL )
			piaMatrix->Add( pBay->GetBayNumber() );
	}


	return true;
}

bool Run::JoinInner( Bay *pSelectedBay, Bay *pNewBay)
{
	int			iNewRunID;
	Bay			*pBay;
	Run			*pNewRun, *pRun;
	bool		bResult;
	Vector3D	Vector;
	Matrix3D	Transform,
				OriginalRunTransform,
				OriginalRunSchematicTransform;


	if( pSelectedBay->GetBayLength()!=pNewBay->GetBayLength() )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot attach a Bay of different length to south of bay %i!"), pSelectedBay->GetBayNumber() );
		return false;
	}

	////////////////////////////////////////////////////////////
	//Does it already have an inner bay?
	if( pSelectedBay->GetInner()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nThere is already a bay south of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
		return false;
	}

	////////////////////////////////////////////////////////////
	//Maybe it already has an outer bay and doesn't know it
	if( pSelectedBay->GetBackward()!=NULL )
	{
		if( pSelectedBay->GetBackward()->GetInner()!=NULL )
		{
			if( pSelectedBay->GetBackward()->GetInner()->GetForward()!=NULL )
			{
				//Somehow, we have a bay here that was not linked up properly
				;//assert( false );

				if( pSelectedBay->GetBayLength()!=pSelectedBay->GetBackward()->GetInner()->GetForward()->GetBayLength() )
				{
					//Major fuckup in structure;
					;//assert( false );
					return false;
				}

				//Linkup the bay;
				pSelectedBay->GetBackward()->GetInner()->GetForward()->SetOuter( pSelectedBay );
				pSelectedBay->SetInner( pSelectedBay->GetBackward()->GetInner()->GetForward() );
				
				MessageBeep(MB_ICONEXCLAMATION);
				acutPrintf( _T("\nThere is already a bay south of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
				return false;
			}
			else
			{
				pBay = pSelectedBay->GetBackward()->GetInner();
				pRun = pBay->GetRunPointer();
				bResult = pRun->JoinForward( pBay->GetID(), pBay, pNewBay );
				if( bResult )
				{
					pNewBay->SetOuter( pSelectedBay );
					pSelectedBay->SetInner( pNewBay );

					pNewBay->UpdateSchematicView();
					pBay->UpdateSchematicView();
					pSelectedBay->UpdateSchematicView();
				}
				return bResult;
			}
		}
	}

	////////////////////////////////////////////////////////////
	//Maybe it already has an outer bay and doesn't know it
	if( pSelectedBay->GetForward()!=NULL )
	{
		if( pSelectedBay->GetForward()->GetInner()!=NULL )
		{
			if( pSelectedBay->GetForward()->GetInner()->GetBackward()!=NULL )
			{
				//Somehow, we have a bay here that was not linked up properly
				;//assert( false );

				if( pSelectedBay->GetBayLength()!=pSelectedBay->GetForward()->GetInner()->GetBackward()->GetBayLength() )
				{
					//Major fuckup in structure;
					;//assert( false );
					return false;
				}

				//Linkup the bay;
				pSelectedBay->GetForward()->GetInner()->GetBackward()->SetOuter( pSelectedBay );
				pSelectedBay->SetInner( pSelectedBay->GetForward()->GetInner()->GetBackward() );
				
				MessageBeep(MB_ICONEXCLAMATION);
				acutPrintf( _T("\nThere is already a bay south of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
				return false;
			}
			else
			{
				pBay = pSelectedBay->GetForward()->GetInner();
				pRun = pBay->GetRunPointer();
				bResult = pRun->JoinBackward( pBay->GetID(), pBay, pNewBay );
				if( bResult )
				{
					pNewBay->SetOuter( pSelectedBay );
					pSelectedBay->SetInner( pNewBay );

					pNewBay->UpdateSchematicView();
					pBay->UpdateSchematicView();
					pSelectedBay->UpdateSchematicView();
				}
				return bResult;
			}
		}
	}

	/////////////////////////////////////////////////////////////////
	//Create a New Run for this bay
	iNewRunID = GetController()->CreateNewRun();
	pNewRun = GetController()->GetRun(iNewRunID);

	//copy the run data
	*(pNewRun->GetRunTemplate())		= *(pSelectedBay->GetRunPointer()->GetRunTemplate());
	pNewRun->SetUseLongerBracing(pSelectedBay->GetRunPointer()->GetUseLongerBracing());

	pNewRun->AddBay( pNewBay );

	/////////////////////////////////////////////////////////////////
	//Set the relationship between the bays
	pNewBay->SetOuter( pSelectedBay );
	pSelectedBay->SetInner( pNewBay );

	pNewBay->UpdateSchematicView();
	pSelectedBay->UpdateSchematicView();

	///////////////////////////////////////////////////////
	//Move the new bay and subsequent bays to thier correct postions
	Vector.set( 0.00, -1.00*pSelectedBay->GetBayWidthActual(), 0.00 );
	Transform.setToTranslation( Vector );
	Transform = pSelectedBay->GetSchematicTransform() * Transform;
	Transform = pSelectedBay->GetRunPointer()->GetSchematicTransform() * Transform;
	pNewRun->MoveSchematic( Transform, true, FIXED_DIRECTION_NEITHER );


	Transform.setToTranslation( Vector );
	Transform = pSelectedBay->GetTransform() * Transform;
	Transform = pSelectedBay->GetRunPointer()->GetTransform() * Transform;
	pNewRun->Move( Transform, true, FIXED_DIRECTION_NEITHER  );

	return true;
}

bool Run::JoinOuter(Bay *pSelectedBay, Bay *pNewBay)
{
	int			iNewRunID;
	Bay			*pBay;
	Run			*pNewRun, *pRun;
	bool		bResult;
	Vector3D	Vector;
	Matrix3D	Transform, OriginalRunTrans, Trans;

	if( pSelectedBay->GetBayLength()!=pNewBay->GetBayLength() )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nCannot attach a Bay of different length to north of bay %i!"), pSelectedBay->GetBayNumber() );
		return false;
	}

	////////////////////////////////////////////////////////////
	//Does it already have an outer bay?
	if( pSelectedBay->GetOuter()!=NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		acutPrintf( _T("\nThere is already a bay north of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
		return false;
	}

	////////////////////////////////////////////////////////////
	//Maybe it already has an outer bay and doesn't know it
	if( pSelectedBay->GetBackward()!=NULL )
	{
		if( pSelectedBay->GetBackward()->GetOuter()!=NULL )
		{
			if( pSelectedBay->GetBackward()->GetOuter()->GetForward()!=NULL )
			{
				//Somehow, we have a bay here that was not linked up properly
				;//assert( false );

				if( pSelectedBay->GetBayLength()!=pSelectedBay->GetBackward()->GetOuter()->GetForward()->GetBayLength() )
				{
					//Major fuckup in structure;
					;//assert( false );
					return false;
				}

				//Linkup the bay;
				pSelectedBay->GetBackward()->GetOuter()->GetForward()->SetInner( pSelectedBay );
				pSelectedBay->SetOuter( pSelectedBay->GetBackward()->GetOuter()->GetForward() );
				
				MessageBeep(MB_ICONEXCLAMATION);
				acutPrintf( _T("\nThere is already a bay north of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
				return false;
			}
			else
			{
				pBay = pSelectedBay->GetBackward()->GetOuter();
				pRun = pBay->GetRunPointer();
				bResult = pRun->JoinForward( pBay->GetID(), pBay, pNewBay );
				if( bResult )
				{
					pNewBay->SetInner( pSelectedBay );
					pSelectedBay->SetOuter( pNewBay );

					pNewBay->UpdateSchematicView();
					pBay->UpdateSchematicView();
					pSelectedBay->UpdateSchematicView();
				}
				return bResult;
			}
		}
	}

	////////////////////////////////////////////////////////////
	//Maybe it already has an outer bay and doesn't know it
	if( pSelectedBay->GetForward()!=NULL )
	{
		if( pSelectedBay->GetForward()->GetOuter()!=NULL )
		{
			if( pSelectedBay->GetForward()->GetOuter()->GetBackward()!=NULL )
			{
				//Somehow, we have a bay here that was not linked up properly
				;//assert( false );

				if( pSelectedBay->GetBayLength()!=pSelectedBay->GetForward()->GetOuter()->GetBackward()->GetBayLength() )
				{
					//Major fuckup in structure;
					;//assert( false );
					return false;
				}

				//Linkup the bay;
				pSelectedBay->GetForward()->GetOuter()->GetBackward()->SetInner( pSelectedBay );
				pSelectedBay->SetOuter( pSelectedBay->GetForward()->GetOuter()->GetBackward() );
				
				MessageBeep(MB_ICONEXCLAMATION);
				acutPrintf( _T("\nThere is already a bay north of bay %i, I cannot insert another!"), pSelectedBay->GetBayNumber() );
				return false;
			}
			else
			{
				pBay = pSelectedBay->GetForward()->GetOuter();
				pRun = pBay->GetRunPointer();
				bResult = pRun->JoinBackward( pBay->GetID(), pBay, pNewBay );
				if( bResult )
				{
					pNewBay->SetInner( pSelectedBay );
					pSelectedBay->SetOuter( pNewBay );

					pNewBay->UpdateSchematicView();
					pBay->UpdateSchematicView();
					pSelectedBay->UpdateSchematicView();
				}
				return bResult;
			}
		}
	}

	/////////////////////////////////////////////////////////////////
	//Create a New Run for this bay
	iNewRunID = GetController()->CreateNewRun();
	pNewRun = GetController()->GetRun(iNewRunID);

	//copy the run data
	*(pNewRun->GetRunTemplate())		= *(pSelectedBay->GetRunPointer()->GetRunTemplate());
	pNewRun->SetUseLongerBracing(pSelectedBay->GetRunPointer()->GetUseLongerBracing());

	pNewRun->AddBay( pNewBay );

	/////////////////////////////////////////////////////////////////
	//Set the relationship between the bays
	pNewBay->SetInner( pSelectedBay );
	pSelectedBay->SetOuter( pNewBay );

	pNewBay->UpdateSchematicView();
	pSelectedBay->UpdateSchematicView();

	///////////////////////////////////////////////////////
	//Move the new bay and subsequent bays to thier correct postions
	//normally the selected bay would be offset from the wall
	Vector.set( 0.00, pSelectedBay->GetBayWidthActual(), 0.00 );
	Trans.setToTranslation( Vector );
	Transform = pSelectedBay->GetSchematicTransform() * Trans;
	Transform = pSelectedBay->GetRunPointer()->GetSchematicTransform() * Transform;
	pNewRun->MoveSchematic( Transform, true, FIXED_DIRECTION_NEITHER );

	//move the components too
	Transform = pSelectedBay->GetTransform() * Trans;
	Transform = pSelectedBay->GetRunPointer()->GetTransform() * Transform;
	pNewRun->Move( Transform, true, FIXED_DIRECTION_NEITHER  );

//	MoveSchematic( OriginalRunTrans, true );

	return true;
}

void Run::GetRLExtentsForAllBays( double &dRLTop, double &dRLBottom )
{
	int		i, iBayID;
	Bay		*pBay;
	double	dRLMin[4], dRLMax[4];

	;//assert( GetNumberOfBays()>0 );

	////////////////////////////////////////////////////////////
	//get the RL's for the first bay
	pBay = GetBay(0);
	pBay->FindAllRLsForEachCornerOfThisBayByLifts( dRLMax, dRLMin );
	//Use the first RL as the yardstick to measure all the rest against
	dRLTop = dRLMax[0];
	dRLBottom = dRLMin[0];
	for( i=1; i<4; i++ )
	{
		dRLTop = max( dRLTop, dRLMax[i] );
		dRLBottom = max( dRLBottom, dRLMin[i] );
	}

	////////////////////////////////////////////////////////////
	//get the RL's for the rest of the bays
	for( iBayID=1; iBayID<GetNumberOfBays(); iBayID++ )
	{
		pBay = GetBay(iBayID);
		pBay->FindAllRLsForEachCornerOfThisBayByLifts( dRLMax, dRLMin );
		for( i=0; i<4; i++ )
		{
			dRLTop = max( dRLTop, dRLMax[i] );
			dRLBottom = max( dRLBottom, dRLMin[i] );
		}
	}
}

double Run::FindWidthOfSouthernStageboard()
{
	if( GetController()->IsWallOffsetFromLowestHopup() )
		return FindWidthOfBottomSouthernStageboard();
	return FindWidthOfLongestSouthernStageboard();
}

double Run::FindWidthOfLongestSouthernStageboard()
{
	int		iBayID;
	Bay		*pBay;
	double	dWidth, dMaxWidth;

	////////////////////////////////////////////////////////////
	//get the RL's for the rest of the bays
	dMaxWidth = 0.00;
	if( GetNumberOfBays()>0 )
	{
		dMaxWidth = GetBay(0)->GetWidthOfLongestStage( SOUTH );

		for( iBayID=1; iBayID<GetNumberOfBays(); iBayID++ )
		{
			pBay = GetBay(iBayID);
			dWidth = pBay->GetWidthOfLongestStage( SOUTH );
			dMaxWidth = max( dWidth, dMaxWidth );
		}
	}

	return dMaxWidth;
}

double Run::FindWidthOfBottomSouthernStageboard()
{
	int		iBayID;
	Bay		*pBay;
	double	dWidth, dMaxWidth;

	////////////////////////////////////////////////////////////
	//get the RL's for the rest of the bays
	dMaxWidth = 0.00;
	if( GetNumberOfBays()>0 )
	{
		dMaxWidth = GetBay(0)->GetWidthOfBottomStage( SOUTH );

		for( iBayID=1; iBayID<GetNumberOfBays(); iBayID++ )
		{
			pBay = GetBay(iBayID);
			dWidth = pBay->GetWidthOfBottomStage( SOUTH );
			dMaxWidth = max( dWidth, dMaxWidth );
		}
	}

	return dMaxWidth;
}

void Run::AlignStandards()
{
	int			iBayID;
	Bay			*pBay;
	double		dMaxWidth;

	////////////////////////////////////////////////////////////
	//get the RL's for the rest of the bays
	dMaxWidth = FindWidthOfSouthernStageboard();

	for( iBayID=0; iBayID<GetNumberOfBays(); iBayID++ )
	{
		pBay = GetBay(iBayID);
		pBay->AlignBay( dMaxWidth );
	}
}


bool Run::SetTiesIfPossible(Bay *pBay, double dHeight, double dLength, MaterialTypeEnum eMT)
{
	bool bResult = false;
	bResult|= SetTieIfPossible( pBay, pBay->GetOuter(),		pBay->GetTemplate()->GetNNETie(), NNE, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetForward(),	pBay->GetTemplate()->GetENETie(), ENE, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetForward(),	pBay->GetTemplate()->GetESETie(), ESE, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetInner(),		pBay->GetTemplate()->GetSSETie(), SSE, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetInner(),		pBay->GetTemplate()->GetSSWTie(), SSW, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetBackward(),	pBay->GetTemplate()->GetWSWTie(), WSW, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetBackward(),	pBay->GetTemplate()->GetWNWTie(), WNW, dLength, dHeight, eMT );
	bResult|= SetTieIfPossible( pBay, pBay->GetOuter(),		pBay->GetTemplate()->GetNNWTie(), NNW, dLength, dHeight, eMT );
	return bResult;
}

bool Run::SetTieIfPossible( Bay *pBay, Bay *pNeighbor, bool bTemplateTie, SideOfBayEnum eSide, double dLength, double dHeight, MaterialTypeEnum eMT)
{
	if( pNeighbor==NULL && bTemplateTie )
	{
		pBay->AddComponent( CT_TIE_TUBE, eSide, dHeight, dLength, eMT );
		return true;
	}

	pBay->DeleteTieFromSide( eSide );
	return false;
}

bool Run::SetRunFit(RunFitEnum eFit)
{
	GetRunTemplate()->SetFit( eFit );
	return true;
}

double Run::GetDistanceFromWall() const
{
	return GetRunTemplate()->GetDistanceFromWall();
}

void Run::SetDistanceToWall( double dDistance )
{
	GetRunTemplate()->SetDistanceToWall( dDistance );
}

void Run::SetTemplateRLEnd( double dRL )
{
	GetRunTemplate()->SetRLEnd( dRL );
}

void Run::SetTemplateRLStart( double dRL )
{
	GetRunTemplate()->SetRLStart( dRL );
}

double Run::GetTemplateRLEnd() const
{
	return GetRunTemplate()->GetRLEnd();
}

double Run::GetTemplateRLStart() const
{
	return GetRunTemplate()->GetRLStart();
}

Point3D Run::GetPointStart() const
{
	return GetRunTemplate()->GetPointStart();
}

Point3D Run::GetPointEnd() const
{
	return GetRunTemplate()->GetPointEnd();
}

void Run::SetPointStart(Point3D pt)
{
	GetRunTemplate()->SetPointEnd(pt);
}

void Run::SetPointEnd(Point3D pt)
{
	GetRunTemplate()->SetPointEnd(pt);
}

Point3D Run::GetPoint(int iPointNumber) const
{
	return GetRunTemplate()->GetPoint( iPointNumber );
}

void Run::SetPoint(int iPointNumber, Point3D pt )
{
	GetRunTemplate()->SetPoint( iPointNumber, pt );
}

RunList::RunList()
{

}

RunList::~RunList()
{
/*
	This is a list of pointers, and it is used in several places to
		point to lists of existing runs, if we delete the runs here
		we run the risk of deleting existing bays
	Run *pRun;

	while( GetSize()>0 )
	{
		pRun=NULL;
		pRun = GetAt(0);
		if( pRun!=NULL )
		{
			delete pRun;
		}
		else
		{
			;//assert( false );
		}
		RemoveAt(0);
	}
*/
}

bool Run::EraseBays(int iBayID, int nCount)
{
	Bay			*pBay, *pForwardBay;
	int			i, iSize, iMaxDelete;
	bool		bSoleBoard[2];
	doubleArray	daArrangement[2];

	iMaxDelete	= nCount+iBayID;
	iSize		= GetNumberOfBays();

	///////////////////////////////////////////////////////////////
	//check the ranges
	if( (nCount<=0) || (iBayID<0) || (iBayID>=iSize) ||
		 (iMaxDelete>iSize) )
		return false;

	////////////////////////////////////////////////////////////////////
	//We will need to convert the list of Bays from bay numbers
	//	to bays, since all the number system will be screwed up
	intArray	*pia3D, *piaMatrix;	
	BayList		ba3D, baMatrix;
	pia3D		= GetController()->GetSelected3DBays();
	piaMatrix	= GetController()->GetSelectedMatrixBays();
	int	iBay;
	ba3D.RemoveAll();
	for( i=0; i<pia3D->GetSize(); i++ )
	{
		iBay = pia3D->GetAt(i);
		if(iBay<iBayID || iBay>(iBayID+nCount))
		{
			pBay = GetController()->GetBayFromBayNumber(iBay);
			if( pBay!=NULL )
				ba3D.Add(pBay);
		}
	}
	baMatrix.RemoveAll();
	for( i=0; i<piaMatrix->GetSize(); i++ )
	{
		iBay = piaMatrix->GetAt(i);
		if(iBay<iBayID || iBay>(iBayID+nCount))
		{
			pBay = GetController()->GetBayFromBayNumber(iBay);
			if( pBay!=NULL )
				baMatrix.Add(pBay);
		}
	}

	for( i=iBayID; i<iMaxDelete; i++ )
	{
		pBay = NULL;
		pBay = GetBay( i );
		if( pBay!=NULL )
		{
			pBay->SetDirtyFlag( DF_DELETE );

			if( GetController()!=NULL && GetController()->IsDestructingController() )
			{
				//Just delete the thing!
				pBay->ClearAllBayPointers( true, false );
			}
			else if( pBay->GetID()==0 && pBay->GetForward()!=NULL )
			{
				//This is the first bay in a run, so we need to
				//	replace the standards on the next bay!

				pForwardBay = pBay->GetForward();
				//North west
				bSoleBoard[0] = pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NORTH_EAST );
				pBay->GetStandardsArrangement( daArrangement[0], CNR_NORTH_EAST );

				//South west
				bSoleBoard[1] = pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SOUTH_EAST );
				pBay->GetStandardsArrangement( daArrangement[1], CNR_SOUTH_EAST );

				pBay->ClearAllBayPointers( false, true );

				pForwardBay->SetStandardsArrangement( daArrangement[0], CNR_NORTH_WEST, bSoleBoard[0] );
				pForwardBay->SetStandardsArrangement( daArrangement[1], CNR_SOUTH_WEST, bSoleBoard[1] );
			}
			else
			{
				pBay->ClearAllBayPointers( true, false );
			}

			//delete the bay
			GetController()->IgnorErase(true);
			delete pBay;
			GetController()->IgnorErase(false);
		}
		else
		{
			;//assert( false );
		}
	}
	m_baBays.RemoveAt( iBayID, nCount );

	////////////////////////////////////////////////////////////////////
	//Restore the bay numbers
	pia3D->RemoveAll();
	for( i=0; i<ba3D.GetSize(); i++ )
	{
		pBay = ba3D.GetAt(i);
		if( pBay!=NULL )
			pia3D->Add( pBay->GetBayNumber() );
	}
	piaMatrix->RemoveAll();
	for( i=0; i<baMatrix.GetSize(); i++ )
	{
		pBay = baMatrix.GetAt(i);
		if( pBay!=NULL )
			piaMatrix->Add( pBay->GetBayNumber() );
	}

	return true;
}

void Run::MoveStartOfRunBackward(double dDistanceToMove)
{
	//This is just like the shufflesubsequent Bays, except that
	// instead to of the rear bays moveing forward, that movement
	// is applied to the start of the run instead of the bays
	Matrix3D	Transform;
	Vector3D	Vector;

	///////////////////////////////////////////////////////////////
	//Remove this movement from the Bays
	Vector.set( -1.00*dDistanceToMove, 0.00, 0.00 );
	Transform.setToTranslation( Vector );

	int			i;
	Bay			*pBay;
	Matrix3D	TransBay;
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		pBay = GetBay(i);
		TransBay = pBay->GetTransform();
		TransBay = Transform * TransBay;
		pBay->SetTransform( TransBay );
		TransBay = pBay->GetSchematicTransform();
		TransBay = Transform * TransBay;
		pBay->SetSchematicTransform( TransBay );
	}

	/////////////////////////////////////////////////////////
	//Add the movement to the Run
	Vector.set( dDistanceToMove, 0.00, 0.00 );
	Transform.setToTranslation( Vector );

	SetTransform( GetTransform() * Transform );
	SetSchematicTransform( GetSchematicTransform() * Transform );

	UpdateSchematicView();
}

void Run::ShuffleSubsequentBays( int iId, double dDistanceToMove, bool bOnlyThisRun/*=true*/ )
{
	int			iNumber;
	Bay			*pB, *pOtherBay;
	Matrix3D	Transform, OriginalOriginTransform,
				OriginalRunTransform, OriginalBayTransform,
				OriginalRunSchematicTransform;
	Vector3D	Vector;

	iNumber = GetNumberOfBays();

	if( iId<0 || iId>=iNumber )
		return;

	///////////////////////////////////////////////////////////////
	//These now join up so set the bay pointers
	pB=NULL;
	pOtherBay=NULL;
	if( (iId>=0) && (iId<iNumber) )
		pB = GetBay( iId );
	if( ((iId-1)>=0) && ((iId-1)<iNumber) )
		pOtherBay = GetBay( iId-1 );

	if( pB!=NULL )
		pB->SetBackward( pOtherBay );
	if( pOtherBay!=NULL )
		pOtherBay->SetForward( pB );

	;//assert( pB!=NULL );

	///////////////////////////////////////////////////////////////
	//move the run and the bay back to thier original positions
	//	which is parrallel to the Xaxis
	if( GetController()->IsCrosshairCreated() )
		OriginalOriginTransform			= GetController()->UnMove3DCrosshair();
	OriginalRunTransform			= UnMove();
	OriginalRunSchematicTransform	= UnMoveSchematic();

	///////////////////////////////////////////////////////////////
	//how far do we need to move each bay within this run
	Vector.set( (-1.00*dDistanceToMove), 0.00, 0.00 );
	Transform.setTranslation( Vector );

	///////////////////////////////////////////////////////////////
	//Move the subsequent bays

	if( bOnlyThisRun )
	{
		//We should only shuffle the bays in this run
		Bay	*pNeighborBay;
		pNeighborBay = pB;
		while( pNeighborBay!=NULL )
		{
			pNeighborBay->Move( Transform, true );
			pNeighborBay->MoveSchematic( Transform, true );
			pNeighborBay = pNeighborBay->GetForward();
		}
	}
	else
	{
		//We should shuffle all bays attached to this run
		GetController()->SetHasBeenVisited(false);
		//mark the bays to the west as already visited!
		if( pOtherBay!=NULL )
			pOtherBay->SetHasBeenVisited( true );
		//move all the bays to the right, including this bay!
		pB->Visit( Transform, false );
	}

	///////////////////////////////////////////////////////////////
	//move them back
	Move( OriginalRunTransform, true );
	MoveSchematic( OriginalRunSchematicTransform, true );
	if( GetController()->IsCrosshairCreated() )
		GetController()->Move3DCrosshair( OriginalOriginTransform, true );
}

void Run::SetAutobuildNumber()
{
	m_iAutobuildNumber = giAutobuildNumber;
}

int Run::GetAutobuildNumber()
{
	return m_iAutobuildNumber;
}


void Run::MoveAllNorthernRuns( Matrix3D Movement, int iBaysToConsider, bool bStore )
{
	Matrix3D	OriginalOriginTransform,
				OriginalRunTransform,
				OriginalRunSchematicTransform;

	///////////////////////////////////////////////////////////////
	//move the run and the bay back to thier original position
	//	which is parrallel to the Xaxis
	if( GetController()->IsCrosshairCreated() )
		OriginalOriginTransform			= GetController()->UnMove3DCrosshair();

	///////////////////////////////////////////////////////////////
	//Move the northern runs
	Bay	*pBayNeighbor;
	Run	*pRunNeighbor;

	;//assert( iBaysToConsider>=0 );
	;//assert( iBaysToConsider<GetNumberOfBays() );

	pBayNeighbor = GetBay(iBaysToConsider)->GetOuter();
	while( pBayNeighbor!=NULL )
	{
		pRunNeighbor = pBayNeighbor->GetRunPointer();
		;//assert( pRunNeighbor!=NULL );

		//restore the position of the run
		OriginalRunTransform			= pRunNeighbor->UnMove();
		OriginalRunSchematicTransform	= pRunNeighbor->UnMoveSchematic();

		//perform the desired movement
		pRunNeighbor->Move( Movement, bStore, FIXED_DIRECTION_NEITHER );
		pRunNeighbor->MoveSchematic( Movement, bStore, FIXED_DIRECTION_NEITHER );

		//restore the position of the run
		pRunNeighbor->Move( OriginalRunTransform, true );
		pRunNeighbor->MoveSchematic( OriginalRunSchematicTransform, true );

		//get ready for the next loop, using the northern bay(if it exists)!
		pBayNeighbor = pBayNeighbor->GetOuter();
	}

	///////////////////////////////////////////////////////////////
	//move them back
	if( GetController()->IsCrosshairCreated() )
		GetController()->Move3DCrosshair( OriginalOriginTransform, true );
}

void Run::SetAllowDrawBays(bool bAllow)
{
	int i;
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		GetBay(i)->SetAllowDraw( bAllow );
	}
}


void Run::SetHasBeenVisited(bool bVisited)
{
	int i;

	SetRunVisited(bVisited);
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		GetBay(i)->SetHasBeenVisited( bVisited );
	}
}

void Run::SetRunVisited(bool bVisited)
{
	m_bRunVisited = bVisited;
}

bool Run::GetRunVisited()
{
	return m_bRunVisited;
}

void Run::VisitRun(Matrix3D Transform)
{
	if( GetRunVisited() )
		return;

	SetRunVisited( true );

	Move( Transform, true, FIXED_DIRECTION_NEITHER );
	MoveSchematic( Transform, true, FIXED_DIRECTION_NEITHER );
}

void Run::ClearAllowDraw()
{
	int i;
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		GetBay(i)->ClearAllowDraw();
	}
}

void Run::SetUseLongerBracing(bool bUseLonger)
{
	m_bUseLongerBracing = bUseLonger;
}

bool Run::GetUseLongerBracing() const
{
	return m_bUseLongerBracing;
}

void Run::SetAllComponentsVisible()
{
	int	i;
	Bay	*pBay;

	///////////////////////////////////////////////////
	//Runs
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		pBay  = GetBay( i );
		;//assert( pBay!=NULL );
		pBay->SetAllComponentsVisible();
	}
}

bool Run::IsAnyBaySelected()
{
	int	i;
	Bay	*pBay;

	///////////////////////////////////////////////////
	//Runs
	for( i=0; i<GetNumberOfBays(); i++ )
	{
		pBay  = GetBay( i );
		;//assert( pBay!=NULL );
		if( pBay->IsBaySelected() )
		{
			return true;
		}
	}
	return false;
}

doubleArray Run::GetAvailBraceLengths(double dBayLength)
{
	doubleArray	daLengths;

	daLengths.RemoveAll();

	//////////////////////2400////////////////////
	if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR )
	{
		daLengths.Add( COMPONENT_LENGTH_3600 );
		daLengths.Add( COMPONENT_LENGTH_3200 );
	}
	//////////////////////1800////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1800-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1800+ROUND_ERROR )
	{
		daLengths.Add( COMPONENT_LENGTH_2700 );
	}
	//////////////////////1200////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR )
	{
		if( GetSystem()==S_MILLS )
			daLengths.Add( COMPONENT_LENGTH_1900 );
		else
			daLengths.Add( COMPONENT_LENGTH_2100 );
	}
	//////////////////////0700////////////////////
	else if( (	dBayLength>COMPONENT_LENGTH_0800-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0800+ROUND_ERROR ) ||
			 (	dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR ) )
	{
		if( GetSystem()==S_KWIKSTAGE )
			daLengths.Add( COMPONENT_LENGTH_1700 );
		daLengths.Add( COMPONENT_LENGTH_1300 );
	}
	else
	{
		;//assert( false );
	}

	return daLengths;
}


double Run::GetBracingLength(double dBayLength) const
{
	double	dLength;

	dLength = GetRunTemplate()->GetBracingLength2400();

	//////////////////////2400////////////////////
	if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR )
	{
		dLength = GetRunTemplate()->GetBracingLength2400();
	}
	//////////////////////1800////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1800-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1800+ROUND_ERROR )
	{
		dLength = GetRunTemplate()->GetBracingLength1800();
	}
	//////////////////////1200////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR )
	{
		dLength = GetRunTemplate()->GetBracingLength1200();
	}
	//////////////////////0700////////////////////
	else if( (	dBayLength>COMPONENT_LENGTH_0800-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0800+ROUND_ERROR ) ||
			 (	dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR ) )
	{
		dLength = GetRunTemplate()->GetBracingLength0700();
	}
	else
	{
		;//assert( false );
	}

	return dLength;
}

void Run::SetBracingLength(double dBayLength, double dLength )
{
	//////////////////////2400////////////////////
	if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR )
	{
		GetRunTemplate()->SetBracingLength2400( dLength );
	}
	//////////////////////1800////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1800-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1800+ROUND_ERROR )
	{
		GetRunTemplate()->SetBracingLength1800( dLength );
	}
	//////////////////////1200////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR )
	{
		GetRunTemplate()->SetBracingLength1200( dLength );
	}
	//////////////////////0700////////////////////
	else if( (	dBayLength>COMPONENT_LENGTH_0800-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0800+ROUND_ERROR ) ||
			 (	dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
				dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR ) )
	{
		GetRunTemplate()->SetBracingLength0700( dLength );
	}
	else
	{
		;//assert( false );
	}
}

SystemEnum Run::GetSystem() const
{
	if( GetRunID()<0 && GetController()!=NULL )
	{
		//This is the autobuild run
		return GetController()->GetSystem();
	}
	else if( GetBay(0)!=NULL )
	{
		return GetBay(0)->GetSystem();
	}
	else if( GetController()!=NULL )
	{
		return GetController()->GetSystem();
	}

	return S_MILLS;
}

void Run::SetLongerBracingValues()
{
	doubleArray daLengths;

	//////////////////////2400////////////////////
	daLengths = GetAvailBraceLengths( COMPONENT_LENGTH_2400 );
	if( daLengths.GetSize()>0 )
	{
		SetBracingLength( COMPONENT_LENGTH_2400, daLengths.GetAt(0) );
	}
	//////////////////////1800////////////////////
	daLengths = GetAvailBraceLengths( COMPONENT_LENGTH_1800 );
	if( daLengths.GetSize()>0 )
	{
		SetBracingLength( COMPONENT_LENGTH_1800, daLengths.GetAt(0) );
	}
	//////////////////////1200////////////////////
	daLengths = GetAvailBraceLengths( COMPONENT_LENGTH_1200 );
	if( daLengths.GetSize()>0 )
	{
		SetBracingLength( COMPONENT_LENGTH_1200, daLengths.GetAt(0) );
	}
	//////////////////////0700////////////////////
	daLengths = GetAvailBraceLengths( COMPONENT_LENGTH_0700 );
	if( daLengths.GetSize()>0 )
	{
		SetBracingLength( COMPONENT_LENGTH_0700, daLengths.GetAt(0) );
	}

}

double Run::GetBracingRaise(double dBayLength) const
{
	double dRaise;
	dRaise = 0.00;

	/////////////////////2400//////////////////////////
	if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength2400()>COMPONENT_LENGTH_3600-ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength2400()<COMPONENT_LENGTH_3600+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_2500;
	}
	else if( dBayLength>COMPONENT_LENGTH_2400-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_2400+ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength2400()>COMPONENT_LENGTH_3200-ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength2400()<COMPONENT_LENGTH_3200+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_2000;
	}

	/////////////////////1800//////////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1800-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_1800+ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength1800()>COMPONENT_LENGTH_2700-ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength1800()<COMPONENT_LENGTH_2700+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_2000;
	}

	/////////////////////1200//////////////////////////
	else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
		dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength1200()>COMPONENT_LENGTH_2100-ROUND_ERROR &&
		GetRunTemplate()->GetBracingLength1200()<COMPONENT_LENGTH_2100+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_1500;
	}
	else if( dBayLength>COMPONENT_LENGTH_1200-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_1200+ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength1200()>COMPONENT_LENGTH_1900-ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength1200()<COMPONENT_LENGTH_1900+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_1500;
	}
	
	/////////////////////0700//////////////////////////
	else if( dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength0700()>COMPONENT_LENGTH_1700-ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength0700()<COMPONENT_LENGTH_1700+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_1500;
	}
	else if( dBayLength>COMPONENT_LENGTH_0700-ROUND_ERROR &&
			 dBayLength<COMPONENT_LENGTH_0700+ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength0700()>COMPONENT_LENGTH_1300-ROUND_ERROR &&
			 GetRunTemplate()->GetBracingLength0700()<COMPONENT_LENGTH_1300+ROUND_ERROR )
	{
		dRaise = COMPONENT_LENGTH_1000;
	}

	else
	{
		;//assert( false );
	}
	return dRaise;
}
