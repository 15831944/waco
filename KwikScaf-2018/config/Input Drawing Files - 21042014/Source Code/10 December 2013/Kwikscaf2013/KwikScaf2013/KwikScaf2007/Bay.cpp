// Bay.cpp: implementation of the Bay class.
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
//								BAY CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 13/12/00 11:10a $
//	Version			: $Revision: 255 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This Class is the Central Spoke for most of this program, we will be
// basically taking a Bay-centric approach in the Meccano project.  This
// class with some changes can be used to handle Stairs, where the rules
// are slightly different
///////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Bay.h"
#include "Run.h"
#include "Controller.h"
#include "BayDetailsDialog.h"
#include "dbents.h"
#include <dbgroup.h>
#include "Entity.h"

//See bug 642 as to why this is commented out
//#define	ALLOW_OPEN_END_STANDARDS

#define USE_MATRIX_WIDTH

//See bug 1155 as to why this is nescessary, and why it is commmented out!
//#define WARN_IF_LOWER_STANDARDS;

const	double	TIETUBE_TOLERANCE	= 300.00;
const	double 	d0Deg				= 0.00;
const	double 	d45Deg				= pi/4.00;
const	double 	d90Deg				= 2.00*d45Deg;
const	double 	dminus45Deg			= -1.00*d45Deg;
const	double 	dminus90Deg			= -1.00*d90Deg;

//////////////////////////////////////////////////////////////////////
// Commented by SJ, 26.07.2007, as part of the migration process
// extern HINSTANCE _hdllInstance;
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Bay
//The default constructor for the Bay class
Bay::Bay()
{
	m_iAllowDraw = 0;
	m_iLoadedOuterRunID	= ID_INVALID;
	m_iLoadedOuterBayID	= ID_INVALID;
	m_iLoadedInnerRunID	= ID_INVALID;
	m_iLoadedInnerBayID	= ID_INVALID;
	SetBayType( BAY_TYPE_BAY );

	m_LiftList.DeleteAll();		//list of lifts
	m_caComponents.DeleteAll();	//Array of components

	m_iBayID = ID_INVALID;
	for( int i=0; i<4; i++ )
	{
		m_daHeights[i] = 0.00;
		m_ptaStandardsRL[i].set( 0.00, 0.00, 0.00 );
	}
	SetStaggeredHeights(true);
	m_eStandardsFit = STD_FIT_FINE;
	m_dBayLength = 0.00;
	m_dBayWidth = 0.00;
	SetDirtyFlag(DF_CLEAN);

	m_pBTplt = NULL;	//The Bay template
	m_pRun = NULL;		//pointer back to its run
	SetForwardDumb(NULL);	//East
	SetBackwardDumb(NULL);	//West
	SetInnerDumb(NULL);		//South
	SetOuterDumb(NULL);		//North
	m_pMatrixElement = NULL;
	m_pMatrixElementAlt = NULL;
	m_pBayDetailsDialog = NULL;
	m_pControllerForLapboard = NULL;

	SetTemplate( new BayTemplate() );	//The Bay template

	SetSystem( S_KWIKSTAGE );

	GetTemplate()->SetBayPointer( this );
	
	m_Transform.setToIdentity();
	SetPreviousLength( 0.00 );
	SetPreviousWidth( 0.00 );
	LapboardCrossesSideRemoveAll();

	CreateBay();
	m_pLapboards = new LapboardList();
	m_pLapboards->RemoveAll();

	SetUseMidrailWithChainMesh(false);
	SetUseLedgerEveryMetre(false);
	SetHasMovingDeck(false);
	SetSydneyCornerBay(false);
	SetRoofProtection(RP_NONE);

	m_pUndoMillsBay = NULL;
	SetMillsTypeDumb(MILLS_TYPE_NONE);
	SetCanBeMillsCnrBay(MILLS_TYPE_NONE);
	SetTieTubeVerticalTolerance(TIETUBE_TOLERANCE);

	m_bAllowRecreateStandards = true;

/*	m_bRequiresSoleBoard[CNR_NORTH_EAST] = false;
	m_bRequiresSoleBoard[CNR_SOUTH_EAST] = false;
	m_bRequiresSoleBoard[CNR_SOUTH_WEST] = false;
	m_bRequiresSoleBoard[CNR_NORTH_WEST] = false;
*/
	AcDbObjectId ID;
	ID.setNull();
	SetSWStandardEntityID(ID);
}

//////////////////////////////////////////////////////////////////////
//~Bay
//The default destructor for the Bay class
Bay::~Bay()
{
	SetDirtyFlag(DF_DELETE);
	ClearAllBayPointers( true, false );
	DeleteBay();
	DELETE_PTR(m_pBTplt);
	DELETE_PTR(m_pBayDetailsDialog);
	LapboardCrossesSideRemoveAll();
	m_pLapboards->RemoveAll();
	delete(m_pLapboards);
	m_pLapboards = NULL;
	if( m_pUndoMillsBay!=NULL )
	{
		delete m_pUndoMillsBay;
		m_pUndoMillsBay = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
//CreateBay
//Creates a simple bay
void Bay::CreateBay()
{
	SetGlobalValuesToDefault();
}

//////////////////////////////////////////////////////////////////////
//SetGlobalValuesToDefault
//Sets all member variable to default values
void Bay::SetGlobalValuesToDefault()
{
	/////////////////////////////////////////////////////////////////////////////////
	//General
	m_iBayID		= ID_INVALID;
	SetRunPointer( NULL );
	m_LiftList.DeleteAll();
	m_caComponents.DeleteAll();
	SetTransform( GetTransform().setToIdentity() );

	/////////////////////////////////////////////////////////////////////////////////
	//Pointers to Neighboring bays
	SetBackward( NULL );
	SetForward(NULL);
	SetOuter(NULL);
	SetInner(NULL);

	/////////////////////////////////////////////////////////////////////////////////
	//Matrix
	m_pMatrixElement = NULL;
	m_pMatrixElementAlt = NULL;

	/////////////////////////////////////////////////////////////////////////////////
	//Standards
	m_daHeights[CNR_NORTH_EAST] = 0.00;
	m_daHeights[CNR_SOUTH_EAST] = 0.00;
	m_daHeights[CNR_NORTH_WEST] = 0.00;
	m_daHeights[CNR_SOUTH_WEST] = 0.00;

	Point3D pt;
	pt.set( 0.00, 0.00, 0.00 );
	SetStandardPosition( pt, CNR_NORTH_EAST );
	SetStandardPosition( pt, CNR_SOUTH_EAST );
	SetStandardPosition( pt, CNR_NORTH_WEST );
	SetStandardPosition( pt, CNR_SOUTH_WEST );

	/////////////////////////////////////////////////////////////////////////////////
	//Dialog Box stuff
	m_pBayDetailsDialog = NULL;
	SetStaggeredHeights(false);
	m_eStandardsFit		= STD_FIT_SAME;
	SetBayLength( COMPONENT_LENGTH_2400 );
	SetBayWidth( COMPONENT_LENGTH_1200 );
	SetPreviousLength( GetBayLength() );
	SetPreviousWidth( GetBayWidth() );
	SetDirtyFlag( DF_CLEAN );

	GetTemplate()->DeleteSchematic();
}


/////////////////////////////////////////////////////////////////////////////////
//Bay Dimensions
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//AdjustBayHeight
//This is really a method to handle the RL adjustments.  This function would be used for a few reasons:
//Add empty lifts, to the bottom of the Bay, where each lift should be 2m high, if possible.
//Delete lifts, from the bottom, to adjust for RL.
void Bay::AdjustBayHeight( double dNewHeight )
{
	dNewHeight;
	assert( FALSE );
}

//////////////////////////////////////////////////////////////////////
//GetBayWidth
//
double Bay::GetBayWidth() const
{
	return m_dBayWidth;
}

//////////////////////////////////////////////////////////////////////
//GetBayLength
//
double Bay::GetBayLength() const
{
	return m_dBayLength;
}

//////////////////////////////////////////////////////////////////////
//GetBayLengthActual
//
double Bay::GetBayLengthActual() const
{
	double dLength;
	dLength = GetBayLength();
	return GetBayDimensionActual( dLength, true );
}

//////////////////////////////////////////////////////////////////////
//GetBayWidthActual
//
double Bay::GetBayWidthActual() const
{
	double dWidth;
	dWidth = GetBayWidth();
	return GetBayDimensionActual( dWidth, false );
}

//////////////////////////////////////////////////////////////////////
//GetBayDimensionActual
//
double Bay::GetBayDimensionActual( double dCommonLength, bool bLength, bool bShowWarning/*=true*/ ) const
{
	ComponentTypeEnum	eType;
	double				dLength;

	//Calculate the bay length
	eType = ( bLength )? CT_LEDGER : CT_TRANSOM;

	dLength = GetCompDetails()->GetActualLength( GetSystem(), eType, dCommonLength, MT_STEEL, bShowWarning );

	//we also need the STANDARD's WIDTH since this each bay includes a standard
	dLength+= GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, bShowWarning );

	return dLength;
}

//////////////////////////////////////////////////////////////////////
//SetBayWidth
//
bool Bay::SetBayWidth( double dNewWidth, bool bAllowMovement/*=true*/ )
{
	Lift		*pLift;
	double		dLength, dLengthChange;
	Vector3D	Vector;
	Matrix3D	Transform, OriginalRunTrans, OriginalBayTrans, Original3DTrans;
	Component	*pComp;

	assert( (dNewWidth>=(COMPONENT_LENGTH_0700-ROUND_ERROR) && dNewWidth<=(COMPONENT_LENGTH_0700+ROUND_ERROR)) ||
			(dNewWidth>=(COMPONENT_LENGTH_1200-ROUND_ERROR) && dNewWidth<=(COMPONENT_LENGTH_1200+ROUND_ERROR)) ||
			(dNewWidth>=(COMPONENT_LENGTH_1800-ROUND_ERROR) && dNewWidth<=(COMPONENT_LENGTH_1800+ROUND_ERROR)) ||
			(dNewWidth>=(COMPONENT_LENGTH_2400-ROUND_ERROR) && dNewWidth<=(COMPONENT_LENGTH_2400+ROUND_ERROR)) );

	if( dNewWidth<=m_dBayWidth+ROUND_ERROR && dNewWidth>=m_dBayWidth-ROUND_ERROR )
	{
		//The bay size has not changed
		return false;
	}

	SetPreviousWidth( m_dBayWidth );
	m_dBayWidth	= dNewWidth;

	bool bIsAutobuild;

	bIsAutobuild=false;
	if( GetRunPointer()!=NULL )
	{
		if( GetRunPointer()->GetController()!=NULL && GetRunPointer()->GetController()->IsCrosshairCreated() )
		{
			int iRunID = GetRunPointer()->GetController()->GetRunID( GetRunPointer() );
			bIsAutobuild = (iRunID==ID_NONE_MATCHING);
			if( !bIsAutobuild )
				Original3DTrans = GetRunPointer()->GetController()->UnMove3DCrosshair();
		}
		OriginalRunTrans = GetRunPointer()->UnMove();
	}
	OriginalBayTrans = UnMove();

	RepositionStandards();

	//how far do we need to move these things?
	dLengthChange = GetCompDetails()->GetActualLength( GetSystem(), CT_LEDGER, dNewWidth, MT_STEEL );
	dLengthChange-= GetCompDetails()->GetActualLength( GetSystem(), CT_LEDGER, GetPreviousWidth(), MT_STEEL );
	Vector.set( 0.00, dLengthChange, 0.00 );
	Transform.setToTranslation( Vector );

	for( int iLift=0; iLift<GetNumberOfLifts(); iLift++ )
	{
		pLift = GetLift( iLift );
		pLift->SetLiftWidth( dNewWidth );
	}

	for( int iComp=0; iComp<GetNumberOfBayComponents(); iComp++ )
	{
		pComp = m_caComponents.GetComponent( iComp );

		switch( pComp->GetType() )
		{
		case( CT_BRACING ):
			switch( GetPosition( iComp ) )
			{
				case( NORTH ):			//fallthrough
				case( SOUTH ):
					break;
				case( EAST ):			//fallthrough
				case( WEST ):
					//Remove the bracing, but don't delete it from the template, see 1608
					if( !DeleteBayComponent( iComp ) )
					{
						assert( false );
						continue;
					}
					iComp--;
					break;
				default:
					assert( false );	//we couldn't be talking about bracing something has gone wrong!
					return false;
			}
			break;
		case( CT_CHAIN_LINK ):			//fallthrough
		case( CT_SHADE_CLOTH ):			//fallthrough
			//we will deal with chainlink and shade cloth later, since we need to delete them
			//	from this loop we are currently in!
			continue;
		case( CT_TIE_TUBE ):			//fallthrough
			if( GetPosition(iComp)==NNE ||
				GetPosition(iComp)==SSE ||
				GetPosition(iComp)==NNW ||
				GetPosition(iComp)==SSW )
			{
				//reduce the length of the tube by the change in bay length
				dLength = pComp->GetLengthCommon();
				dLength+= dLengthChange;
				dLength = GetCompDetails()->GetAvailableLength( CT_TIE_TUBE, pComp->GetMaterialType(), dLength, RT_ROUND_UP, true );
				pComp->SetLengthCommon( dLength );

				//the clamps need to be repositioned
			}
			break;
		case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
		case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
		case( CT_TIE_CLAMP_YOKE ):		//fallthrough
		case( CT_TIE_CLAMP_90DEGREE ):	//fallthrough
		case( CT_TEXT ):				//fallthrough
			//Do nothing - many need to be moved though
			break;
		case( CT_STANDARD ):			//fallthrough
		case( CT_STANDARD_OPENEND ):	//fallthrough
		case( CT_JACK ):				//fallthrough
		case( CT_SOLEBOARD ):			//fallthrough
		case( CT_DECKING_PLANK ):		//fallthrough
			break;
//			//We don't need to move these components so skip the next section
//			continue;
		case( CT_STAGE_BOARD ):			//fallthrough
		case( CT_LAPBOARD ):			//fallthrough
		case( CT_LEDGER ):				//fallthrough
		case( CT_TRANSOM ):				//fallthrough
		case( CT_MESH_GUARD ):			//fallthrough
		case( CT_RAIL ):				//fallthrough
		case( CT_MID_RAIL ):			//fallthrough
		case( CT_TOE_BOARD ):			//fallthrough
		case( CT_STAIR ):				//fallthrough
		case( CT_LADDER ):				//fallthrough
		case( CT_HOPUP_BRACKET ):		//fallthrough
		case( CT_CORNER_STAGE_BOARD ):	//fallthrough
		default:
			//These belong to the Lift not the bay!!!
			assert( FALSE );
			break;
		}

		if( pComp->GetMoveWhenBayWidthChanges() )
		{
			pComp->Move( Transform, true );
		}
	}

	//We deleted all east and west bracing in the last step
	//	however the template we left intact, so recreate
	//	the bracing from the template.  See 1608
	ReapplyBracingToSide(EAST);
	ReapplyBracingToSide(WEST);

	//replace all shade cloth and chain link
	ReapplyShadeAndChainOnAllSides();

	Move(OriginalBayTrans, true);
	if( GetRunPointer()!=NULL )
	{
		GetRunPointer()->Move(OriginalRunTrans, true);
		if( GetRunPointer()->GetController()!=NULL && GetRunPointer()->GetController()->IsCrosshairCreated() )
		{
			if( !bIsAutobuild )
				GetRunPointer()->GetController()->Move3DCrosshair( Original3DTrans, true );
		}
	}

	GetTemplate()->SetBayWidth( dNewWidth );

	return true;
}

//////////////////////////////////////////////////////////////////////
//SetBayLength
//
bool Bay::SetBayLength( double dNewSize, bool bMovePermitted/*=true*/ )
{
	Lift			*pLiftPointer;
	double			dSize, dSizeChange;
	Vector3D		Vect;
	Matrix3D		Transf, OriginalRunTrans, OriginalBayTrans, Original3DTrans;
	Component		*pCompon;

	assert( (dNewSize>=(COMPONENT_LENGTH_0700-ROUND_ERROR) && dNewSize<=(COMPONENT_LENGTH_0700+ROUND_ERROR)) ||
			(dNewSize>=(COMPONENT_LENGTH_1200-ROUND_ERROR) && dNewSize<=(COMPONENT_LENGTH_1200+ROUND_ERROR)) ||
			(dNewSize>=(COMPONENT_LENGTH_1800-ROUND_ERROR) && dNewSize<=(COMPONENT_LENGTH_1800+ROUND_ERROR)) ||
			(dNewSize>=(COMPONENT_LENGTH_2400-ROUND_ERROR) && dNewSize<=(COMPONENT_LENGTH_2400+ROUND_ERROR)) );

	if( dNewSize<=GetBayLength()+ROUND_ERROR && dNewSize>=GetBayLength()-ROUND_ERROR )
	{
		//The bay has not changed
		return false;
	}
	SetPreviousLength( GetBayLength() );
	m_dBayLength		= dNewSize;

	if( GetRunPointer()!=NULL )
	{
		int		iRunID;
		bool	bIsAutobuild;

		iRunID = GetController()->GetRunID( GetRunPointer() );
		bIsAutobuild = (iRunID==ID_NONE_MATCHING);
		if( !bIsAutobuild )
		{
			if( GetRunPointer()->GetController()!=NULL && GetRunPointer()->GetController()->IsCrosshairCreated() )
				Original3DTrans = GetRunPointer()->GetController()->UnMove3DCrosshair();
			OriginalRunTrans = GetRunPointer()->UnMove();
		}
	}
	OriginalBayTrans = UnMove();

	RepositionStandards();

	for( int iLiftIdentification=0; iLiftIdentification<GetNumberOfLifts(); iLiftIdentification++ )
	{
		pLiftPointer = GetLift( iLiftIdentification );
		pLiftPointer->SetLiftLength( GetBayLength() );
	}

	//how far do we need to move these components?
	dSizeChange = GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, dNewSize, MT_STEEL );
	dSizeChange-= GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, GetPreviousLength(), MT_STEEL );
	Vect.set( dSizeChange, 0.00, 0.00 );
	Transf.setToTranslation( Vect );

	for( int iLoop=0; iLoop<GetNumberOfBayComponents(); iLoop++ )
	{
		pCompon = m_caComponents.GetComponent( iLoop );

		switch( pCompon->GetType() )
		{
		case( CT_BRACING ):
			switch( GetPosition( iLoop ) )
			{
				case( NORTH ):			//fallthrough
				case( SOUTH ):
					dSize = GetBracingLength( dNewSize );
					pCompon->SetLengthCommon( dSize );
					break;
				case( EAST ):			//fallthrough
				case( WEST ):
					//no need to resize
					break;
				default:
					assert( false );	//we couldn't be talking about bracing something has gone wrong!
					return false;
			}
			break;
		case( CT_CHAIN_LINK ):			//fallthrough
		case( CT_SHADE_CLOTH ):
			//we will deal with chainlink and shade cloth later, since we need to delete them
			//	from this loop we are currently in!
			continue;
		case( CT_TIE_TUBE ):			//fallthrough
			if( GetPosition(iLoop)==ENE ||
				GetPosition(iLoop)==ESE ||
				GetPosition(iLoop)==WNW ||
				GetPosition(iLoop)==WSW )
			{
				//reduce the length of the tube by the change in bay length
				dSize = pCompon->GetLengthCommon();
				dSize-= GetPreviousLength()-dSize;
				pCompon->SetLengthCommon( dSize );
			}
			break;
		case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
		case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
		case( CT_TIE_CLAMP_YOKE ):		//fallthrough
		case( CT_TIE_CLAMP_90DEGREE ):	//fallthrough
		case( CT_TEXT ):				//fallthrough
		case( CT_STANDARD ):			//fallthrough
		case( CT_STANDARD_OPENEND ):	//fallthrough
		case( CT_JACK ):				//fallthrough
		case( CT_SOLEBOARD ):			//fallthrough
			//We don't need to resize these components so skip the next section
			break;
		case( CT_DECKING_PLANK ):		//fallthrough
		case( CT_STAGE_BOARD ):			//fallthrough
		case( CT_LAPBOARD ):			//fallthrough
		case( CT_LEDGER ):				//fallthrough
		case( CT_TRANSOM ):				//fallthrough
		case( CT_MESH_GUARD ):			//fallthrough
		case( CT_RAIL ):				//fallthrough
		case( CT_MID_RAIL ):			//fallthrough
		case( CT_TOE_BOARD ):			//fallthrough
		case( CT_STAIR ):				//fallthrough
		case( CT_LADDER ):				//fallthrough
		case( CT_HOPUP_BRACKET ):		//fallthrough
		case( CT_CORNER_STAGE_BOARD ):	//fallthrough
		default:
			//These belong to the Lift not the bay!!!
			assert( FALSE );
			continue;
		}

		if( pCompon->GetMoveWhenBayLengthChanges() )
		{
			pCompon->Move( Transf, true );
		}
	}

	//replace all shade cloth and chain link
	ReapplyShadeAndChainOnAllSides();

//	GetController()->TracePointers();

	if( bMovePermitted && GetForward()!=NULL )
	{
		GetRunPointer()->MoveSubsequentBays( GetID()+1, Transf, true, false );
#ifdef _DEBUG
		acutPrintf( _T("\nMoving Subsequent Bays for Run%i, starting at Bay%i "), GetRunPointer()->GetRunID(), GetID()+1 );
#endif	//_DEBUG
	}

	Move(OriginalBayTrans, true);
	if( GetRunPointer()!=NULL )
	{
		int		iRunID;
		bool	bIsAutobuild;

		iRunID = GetController()->GetRunID( GetRunPointer() );
		bIsAutobuild = (iRunID==ID_NONE_MATCHING);
		if( !bIsAutobuild )
		{
			GetRunPointer()->Move(OriginalRunTrans, true);
			if( GetRunPointer()->GetController()!=NULL && GetRunPointer()->GetController()->IsCrosshairCreated() )
				GetRunPointer()->GetController()->Move3DCrosshair( Original3DTrans, true );
		}
	}

	GetTemplate()->SetBayLength( dNewSize );
	return true;
}


//////////////////////////////////////////////////////////////////////
//Run
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetRunPointer
//returns the run associated with this Bay
Run *Bay::GetRunPointer() const
{
	return m_pRun;
}


/////////////////////////////////////////////////////////////////////////////////
//SetRunPointer
//set the run pointer for this Bay
void Bay::SetRunPointer( Run *pRun )
{
	//if (pRun!=NULL)
	m_pRun = pRun;
}


//////////////////////////////////////////////////////////////////////
//ID
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//SetID
//called by Run Class to set ID
void Bay::SetID( int iNewID )		
{
	m_iBayID = iNewID;
}

/////////////////////////////////////////////////////////////////////////////////
//GetID
//Queries the list of Bays for its index in that list (Ie the Bay number is
//	simply its position in the Bay list, which is owned by the Run). (1
//	based number)
int Bay::GetID() const
{
	return m_iBayID;
}

/////////////////////////////////////////////////////////////////////////////////
//GetBayNumber
//Sequencial numbering for bays
int Bay::GetBayNumber() const
{
	bool		bFound;
	Run			*pRun;
	Bay			*pBay;
	Controller	*pController;
	int			iRunID, iBayID, iCounter;
	iCounter = 1;

	pController = GetController();
	if( GetBayType()==BAY_TYPE_LAPBOARD )
	{
		for( int i=0; i<pController->GetNumberOfLapboards(); i++ )
		{
			if( this==pController->GetLapboard(i) )
			{
				//let a -ve number idicate a lab
				return (i+1)*-1;
			}
		}
		assert( false );
		return 0;
	}

	pBay=NULL;
	bFound = false;
	for( iRunID=0; iRunID<pController->GetNumberOfRuns(); iRunID++ )
	{
		pRun = pController->GetRun( iRunID );
		if( pRun==NULL )
			break;
		for( iBayID=0; iBayID<pRun->GetNumberOfBays(); iBayID++ )
		{
			pBay = pRun->GetBay( iBayID );
			if( pBay==this )
				return iCounter;
			iCounter++;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
//Dialog box
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//Show
//displays the various edit bay dialog boxes
void Bay::Show() 
{
}

/////////////////////////////////////////////////////////////////////////////////
//ApplyChanges
void Bay::ApplyChanges()	
{
}

/////////////////////////////////////////////////////////////////////////////////
//CancelChanges
void Bay::CancelChanges()
{
}

/////////////////////////////////////////////////////////////////////////////////
//void Auto-Create Handrails()	//don't remember


//////////////////////////////////////////////////////////////////////
//Pointers to Neighbors
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetBackward
Bay *Bay::GetBackward() const
{
	return m_pBackward;
}

/////////////////////////////////////////////////////////////////////////////////
//GetForward
Bay *Bay::GetForward() const
{
	return m_pForward;
}

/////////////////////////////////////////////////////////////////////////////////
//GetOuter
Bay *Bay::GetOuter() const
{
	if( m_iLoadedOuterRunID!=ID_INVALID )
	{
		if( m_iLoadedOuterBayID!=ID_INVALID )
		{
			if( (m_iLoadedOuterRunID>=0 && GetController()->GetNumberOfRuns()>m_iLoadedOuterRunID ) && 
				(m_iLoadedOuterBayID>=0 && GetController()->GetRun(m_iLoadedOuterRunID)->GetNumberOfBays()>m_iLoadedOuterBayID ) )
			{
				return GetController()->GetRun(m_iLoadedOuterRunID)->GetBay( m_iLoadedOuterBayID );
				/* can't use the following since this function is const
				SetOuterDumb(GetController()->GetRun(m_iLoadedOuterRunID)->GetBay( m_iLoadedOuterBayID ));
				//we have now loaded the values so we don't want to come back in here again!
				m_iLoadedOuterRunID=ID_INVALID;
				m_iLoadedOuterBayID=ID_INVALID;
				*/
			}
			else
			{
				//The Run or bay still has not been loaded
				//assert( false );
				return NULL;
			}
		}
		else
		{
			assert( false );	//what happened to our bay?
			return NULL;
		}
	}

	return m_pOuter;
}

/////////////////////////////////////////////////////////////////////////////////
//GetInner
Bay *Bay::GetInner() const
{
	if( m_iLoadedInnerRunID!=ID_INVALID )
	{
		if( m_iLoadedInnerBayID!=ID_INVALID )
		{
			if( (m_iLoadedInnerRunID>=0 && GetController()->GetNumberOfRuns()>m_iLoadedInnerRunID ) && 
				(m_iLoadedInnerBayID>=0 && GetController()->GetRun(m_iLoadedInnerRunID)->GetNumberOfBays()>m_iLoadedInnerBayID ) )
			{
				return GetController()->GetRun(m_iLoadedInnerRunID)->GetBay( m_iLoadedInnerBayID );
				/* can't use the following since this function is const
				SetInnerDumb(GetController()->GetRun(m_iLoadedInnerRunID)->GetBay( m_iLoadedInnerBayID ));
				//we have now loaded the values so we don't want to come back in here again!
				m_iLoadedInnerRunID=ID_INVALID;
				m_iLoadedInnerBayID=ID_INVALID;
				*/
			}
			else
			{
				//The Run or bay still has not been loaded
				//assert( false );
				return NULL;
			}
		}
		else
		{
			assert( false );	//what happened to our bay?
			return NULL;
		}
	}
	return m_pInner;
}

/////////////////////////////////////////////////////////////////////////////////
//GetEastCorner
LapboardBay *Bay::GetLapboard( int iIndex ) const
{
	assert( iIndex>=0 );
	assert( iIndex<GetNumberOfLapboardPointers() );
	return m_pLapboards->GetAt( iIndex );
}

int Bay::FindLapboardIndex(LapboardBay *pLapboard)
{
	int			i;
	LapboardBay *pLap;
	for( i=0; i<GetNumberOfLapboardPointers(); i++ )
	{
		pLap = m_pLapboards->GetAt(i);
		if( pLap==pLapboard )
		{
			return i;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////
//GetWestCorner
LapboardBay *Bay::GetLapboard( SideOfBayEnum eSide ) const
{
	int iIndex;

	iIndex = LapboardCrossesSideWhich( eSide );
	if( iIndex>=0 )
	{
		return GetLapboard( iIndex );
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//SetBackward
void Bay::SetBackward( const Bay *pBay, bool bIgnorNulls/*=true*/, bool bAdjustStandards/*=true*/ )
{
	int		i, iNumberOfPlanks;
	bool	bChangesMade;
	Lift	*pLift, *pLiftNeighbor;
	double	dLiftRL, dRLNeighbor;

	bChangesMade = false;
	if( pBay==GetBackward() )
	{
		//if we are not ignoring nulls and the new value is a
		//	null then don't leave this function yet!
		if( !(!bIgnorNulls && pBay==NULL ) )
		{
			//already done
			return;
		}
	}

	if( pBay!=NULL && GetBackward()!=NULL )
	{
		assert( false );
	}

	SetBackwardDumb(pBay);

	if( GetNumberOfBayComponents()<=0 )
	{
		return;
	}
	
/*	#ifdef _DEBUG
	if( GetBackward()==NULL )
		acutPrintf( "\nBay%i backward = NULL", GetBayNumber() );
	else
		acutPrintf( "\nBay%i backward = Bay%i", GetBayNumber(), GetBackward()->GetBayNumber() );
	#endif	//#ifdef _DEBUG
*/
	if( GetController()->IsDestructingController() )
		return;

	if( GetBackward()!=NULL )
	{
		bChangesMade = true;
		DeleteAllComponentsFromSide(WEST);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW );
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WNW );
		GetBackward()->DeleteAllComponentsFromSide(EAST);
		GetBackward()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE );
		GetBackward()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ENE );

/*
		pLift->CheckForHopupDoubleups(NNW);
		pLift->CheckForHopupDoubleups(SSW);
		//Delete all 0mm hopup brackets, since the previous bay will own them
		Component *pComp;
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, SSW );
			if( pComp!=NULL )
			{
				pLift->DeleteComponent( pComp->GetID() );
			}
			pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, NNW );
			if( pComp!=NULL )
			{
				pLift->DeleteComponent( pComp->GetID() );
			}
		}
*/
		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );

			if( pLift->IsDeckingLift() &&
				!DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetBackward() ) )
			{
				if( pLift->CopyHandrailToSide( N, W ) ||
					pLift->CopyHandrailToSide( E, W ) ||
					pLift->CopyHandrailToSide( S, W ) )
				{
					//////////////////////////////////////////////////////////////////////
					//We may need to add hopup brackets at 1.0m
					pLift->Add1000mmHopupIfReqd( SSW );
					pLift->Add1000mmHopupIfReqd( NNW );
				}
			}

			LiftRiseEnum eRise;
			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets due to ownership issues!
			if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, SSW )==NULL &&
				pLift->GetComponent( CT_STAGE_BOARD, 0, SOUTH )!=NULL &&
				GetInner()==NULL )
			{
				//we have a stage board without a hopup bracket!
				iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( SOUTH );
				dLiftRL = pLift->GetRL();

				//we are only interested if there is a neighboring deck at the same
				//	height, otherwise this component should belong to this bay
				if( DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetBackward(), false ) )
				{
					pLiftNeighbor = GetBackward()->GetLiftAtRL( dLiftRL );
					dRLNeighbor = pLiftNeighbor->GetRL();
					if( !((dRLNeighbor>dLiftRL-ROUND_ERROR &&
						 dRLNeighbor<dLiftRL+ROUND_ERROR) ||
						(dRLNeighbor+GetBackward()->GetStarSeparation()>dLiftRL-ROUND_ERROR &&
						 dRLNeighbor+GetBackward()->GetStarSeparation()<dLiftRL+ROUND_ERROR)) )
					{
						//This lift is not at the same level, nor is it GetBackward()->GetStarSeparation() lower, it
						//	must be GetBackward()->GetStarSeparation() higher!
						pLiftNeighbor = GetBackward()->GetLiftAtRL( dLiftRL+GetBackward()->GetStarSeparation() );
						dRLNeighbor = pLiftNeighbor->GetRL();
					}

					if( pLiftNeighbor!=NULL )
					{
						if( GetBackward()->GetInner()==NULL &&
							!(GetBackward()->HasComponentAtRL( CT_HOPUP_BRACKET, dRLNeighbor, SSE )) )
						{
							eRise = GetRiseEnumFromRise( dLiftRL-dRLNeighbor );
							pLiftNeighbor->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, SSE, eRise, MT_STEEL );
						}
					}
				}
				else
				{
					//Add a hopup to this bay
					pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, SSW, LIFT_RISE_0000MM, MT_STEEL );
				}
			}

			if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, NNW )==NULL &&
				pLift->GetComponent( CT_STAGE_BOARD, 0, NORTH )!=NULL &&
				GetOuter()==NULL )
			{
				//we have a stage board without a hopup bracket!
				iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( NORTH );
				dLiftRL = pLift->GetRL();
				if( DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetBackward() ) )
				{
					pLiftNeighbor = GetBackward()->GetLiftAtRL( dLiftRL );
					if( pLiftNeighbor!=NULL )
					{
						if( GetBackward()->GetOuter()==NULL &&
							!(GetBackward()->HasComponentAtRL( CT_HOPUP_BRACKET, dLiftRL, NNE )) )
						{
							dRLNeighbor = pLiftNeighbor->GetRL();
							eRise = GetRiseEnumFromRise( dLiftRL-dRLNeighbor );
							pLiftNeighbor->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, NNE, eRise, MT_STEEL );
						}
					}
				}
				else
				{
					//Add a hopup
					pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, NNW, LIFT_RISE_0000MM, MT_STEEL );
				}
			}

			pLift->CheckForHopupDoubleups(NNW);
			pLift->CheckForHopupDoubleups(SSW);
		}

		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for( i=0; i<GetBackward()->GetNumberOfLifts(); i++ )
		{
			pLift = GetBackward()->GetLift( i );

			if( pLift->IsDeckingLift() &&
				!GetBackward()->DoesDeckLiftHaveNeighborDeckAtSameRL( i, this ) )
			{
				if( pLift->CopyHandrailToSide( N, E ) ||
					pLift->CopyHandrailToSide( W, E ) ||
					pLift->CopyHandrailToSide( S, E ) )
				{
					//////////////////////////////////////////////////////////////////////
					//We may need to add hopup brackets at 1.0m
					pLift->Add1000mmHopupIfReqd( SSE );
					pLift->Add1000mmHopupIfReqd( NNE );
				}
			}
		}


		//if the forward bay has a stageboard on the south we need to remove
		//	the hopup bracket we used as a handrail
		DeleteUnneeded1000mmHopups( GetBackward(), WEST );

		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_NORTH_WEST );
			AutoAdjustStandard( CNR_SOUTH_WEST );
		}
	}
	else if( !bIgnorNulls )
	{
		int		iLiftID;
		Lift	*pLift;

		//Each lift will also need a transom
		for( iLiftID=0; iLiftID<GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetLift( iLiftID );
			//does this lift already have a transom?
			if( pLift->GetComponent( CT_TRANSOM, LIFT_RISE_0000MM, WEST )==NULL )
			{
				pLift->AddComponent( CT_TRANSOM, GetBayWidth(), WEST, LIFT_RISE_0000MM, MT_STEEL );
				bChangesMade = true;
			}

			//does this lift have a deck or a stageboard?
			bool bDeck, bHopup;
			bDeck = pLift->IsDeckingLift();
			bHopup = pLift->GetComponent( CT_STAGE_BOARD, 0, WEST )!=NULL;

			//We need a hand rail if there is either a Deck or a Hopup but not both (XOR) and not neither
			//	I can't remember the code for XOR!
			if( (bDeck || bHopup) && (!bDeck || !bHopup) )
			{
				if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, WEST )==NULL )
				{
					bChangesMade = true;
					//better add a handrail!
					pLift->AddComponent( CT_RAIL, GetBayWidth(), WEST, LIFT_RISE_1000MM, MT_STEEL );

					if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, EAST )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, SOUTH )!=NULL )
					{
						pLift->AddComponent( CT_MESH_GUARD, GetBayWidth(), WEST, LIFT_RISE_1000MM, MT_STEEL );

						if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, EAST )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, SOUTH )!=NULL )
						{
							pLift->AddComponent( CT_MESH_GUARD, GetBayWidth(), WEST, LIFT_RISE_2000MM, MT_STEEL );
						}
					}
					else
					{
						pLift->AddComponent( CT_MID_RAIL, GetBayWidth(), WEST, LIFT_RISE_0500MM, MT_STEEL );
					}
				}
			}

			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets due to ownership issues!
			if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, SSW )==NULL &&
				pLift->GetComponent( CT_STAGE_BOARD, 0, SOUTH )!=NULL &&
				GetInner()==NULL )
			{
				//we have a stage board without a hopup bracket!
				iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( SOUTH );
				pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, SSW, LIFT_RISE_0000MM, MT_STEEL );
			}

			if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, NNW )==NULL &&
				pLift->GetComponent( CT_STAGE_BOARD, 0, NORTH )!=NULL &&
				GetOuter()==NULL )
			{
				//we have a stage board without a hopup bracket!
				iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( NORTH );
				pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, NNW, LIFT_RISE_0000MM, MT_STEEL );
			}

			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets at 1.0m
			pLift->Add1000mmHopupIfReqd( SSW );
			pLift->Add1000mmHopupIfReqd( NNW );
		}

		//we need to look at adding some standards here
		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_NORTH_WEST );
			AutoAdjustStandard( CNR_SOUTH_WEST );
		}
	}
/*	if( bChangesMade )
		UpdateSchematicView();
*/
}

/////////////////////////////////////////////////////////////////////////////////
//SetForward
void Bay::SetForward( const Bay *pBay, bool bIgnorNulls/*=true*/, bool bAdjustStandards/*=true*/ )
{
	bool	bChangesMade;
	Lift	*pLift;

	bChangesMade = false;
	if( pBay==GetForward() )
	{
		//if we are not ignoring nulls and the new value is a
		//	null then don't leave this function yet!
		if( !(!bIgnorNulls && pBay==NULL ) )
		{
			//already done
			return;
		}
	}

	if( pBay!=NULL && GetForward()!=NULL )
	{
		assert( false );
	}

	SetForwardDumb(pBay);

	if( GetNumberOfBayComponents()<=0 )
	{
		return;
	}

/*	#ifdef _DEBUG
	if( GetForward()==NULL )
		acutPrintf( "\nBay%i forward = NULL", GetBayNumber() );
	else
		acutPrintf( "\nBay%i forward = Bay%i", GetBayNumber(), GetForward()->GetBayNumber() );
	#endif	//#ifdef _DEBUG
*/

	if( GetController()->IsDestructingController() )
		return;

	if( GetForward()!=NULL )
	{
		bChangesMade = true;
		DeleteAllComponentsFromSide(EAST);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE );
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ENE );
		GetForward()->DeleteAllComponentsFromSide(WEST);
		GetForward()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW );
		GetForward()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WNW );

		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for( int i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			if( pLift->IsDeckingLift() &&
				!DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetForward() ) )
			{
				if( pLift->CopyHandrailToSide( N, E ) ||
					pLift->CopyHandrailToSide( W, E ) ||
					pLift->CopyHandrailToSide( S, E ) )
				{
					//////////////////////////////////////////////////////////////////////
					//We may need to add hopup brackets at 1.0m
					pLift->Add1000mmHopupIfReqd( SSE );
					pLift->Add1000mmHopupIfReqd( NNE );
				}
			}

			pLift->CheckForHopupDoubleups(NNE);
			pLift->CheckForHopupDoubleups(SSE);
		}

		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for(int i=0; i<GetForward()->GetNumberOfLifts(); i++ )
		{
			pLift = GetForward()->GetLift( i );
			if( pLift->IsDeckingLift() &&
				!GetForward()->DoesDeckLiftHaveNeighborDeckAtSameRL( i, this ) )
			{
				if( pLift->CopyHandrailToSide( N, W ) ||
					pLift->CopyHandrailToSide( E, W ) ||
					pLift->CopyHandrailToSide( S, W ) )
				{
					//////////////////////////////////////////////////////////////////////
					//We may need to add hopup brackets at 1.0m
					pLift->Add1000mmHopupIfReqd( SSW );
					pLift->Add1000mmHopupIfReqd( NNW );
				}
			}
		}

		//if the forward bay has a stageboard on the south we need to remove
		//	the hopup bracket we used as a handrail
		DeleteUnneeded1000mmHopups( GetForward(), EAST );

		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_NORTH_EAST );
			AutoAdjustStandard( CNR_SOUTH_EAST );
		}
	}
	else if( !bIgnorNulls )
	{
		int		iLiftID;
		Lift	*pLift;

		//Each lift will also need a transom
		for( iLiftID=0; iLiftID<GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetLift( iLiftID );
			//does this lift already have a transom?
			bChangesMade = true;
			if( pLift->GetComponent( CT_TRANSOM, LIFT_RISE_0000MM, EAST )==NULL )
			{
				pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, LIFT_RISE_0000MM, MT_STEEL );
			}

			//does this lift have a deck or a stageboard?
			bool bDeck, bHopup;
			bDeck = pLift->IsDeckingLift();
			bHopup = pLift->GetComponent( CT_STAGE_BOARD, 0, EAST )!=NULL;

			//We need a hand rail if there is either a Deck or a Hopup but not both (XOR)
			//	I can't remember the code for XOR!
			if( (bDeck || bHopup) && (!bDeck || !bHopup))
			{
				//N0!  Better add a handrail!
				if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, EAST )==NULL )
				{
					pLift->AddComponent( CT_RAIL, GetBayWidth(), EAST, LIFT_RISE_1000MM, MT_STEEL );

					if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, WEST )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, SOUTH )!=NULL )
					{
						pLift->AddComponent( CT_MESH_GUARD, GetBayWidth(), EAST, LIFT_RISE_1000MM, MT_STEEL );

						if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, WEST )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, SOUTH )!=NULL )
						{
							pLift->AddComponent( CT_MESH_GUARD, GetBayWidth(), EAST, LIFT_RISE_2000MM, MT_STEEL );
						}
					}
					else
					{
						pLift->AddComponent( CT_MID_RAIL, GetBayWidth(), EAST, LIFT_RISE_0500MM, MT_STEEL );
					}
				}
			}

			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets at 1.0m
			pLift->Add1000mmHopupIfReqd( SSE );
			pLift->Add1000mmHopupIfReqd( NNE );
		}

		//if we added a handrail need to look at adding some standards
		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_NORTH_EAST );
			AutoAdjustStandard( CNR_SOUTH_EAST );
		}
	}
	if( bChangesMade )
		UpdateSchematicView();
}

/////////////////////////////////////////////////////////////////////////////////
//SetOuter
void Bay::SetOuter( const Bay *pBay, bool bIgnorNulls/*=true*/, bool bAdjustStandards/*=true*/ )
{
	Lift	*pLift;

	if( pBay==GetOuter() )
	{
		//if we are not ignoring nulls and the new value is a
		//	null then don't leave this function yet!
		if( !(!bIgnorNulls && pBay==NULL ) )
		{
			//already done
			return;
		}
	}

	if( pBay!=NULL && GetOuter()!=NULL )
	{
		//We should have set pointer to null prior to setting it to another bay
		assert( false );
	}

	SetOuterDumb(pBay);

	if( GetNumberOfBayComponents()<=0 )
	{
		return;
	}

/*	#ifdef _DEBUG
	if( GetOuter()==NULL )
		acutPrintf( "\nBay%i Outer = NULL", GetBayNumber() );
	else
		acutPrintf( "\nBay%i Outer = Bay%i", GetBayNumber(), GetOuter()->GetBayNumber() );
	#endif	//#ifdef _DEBUG
*/
	//If the new get Outer is null then set these to invalid, if they are
	//	not null also set them to invalid
	m_iLoadedOuterRunID = ID_INVALID;
	m_iLoadedOuterBayID = ID_INVALID;

	if( GetController()->IsDestructingController() )
		return;

	if( GetOuter()!=NULL )
	{
		DeleteAllComponentsFromSide(NORTH);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNE );
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNW );
		GetOuter()->DeleteAllComponentsFromSide(SOUTH);
		GetOuter()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE );
		GetOuter()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSW );

		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for( int i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			if( pLift->IsDeckingLift() &&
				!DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetOuter() ) )
			{
				if( pLift->CopyHandrailToSide( E, N ) ||
					pLift->CopyHandrailToSide( S, N ) ||
					pLift->CopyHandrailToSide( W, N ) )
				{
					//handrail added! Do nothing!
				}
			}
			pLift->CheckForHopupDoubleups(ENE);
			pLift->CheckForHopupDoubleups(WNW);
		}

		//if the forward bay has a stageboard on the south we need to remove
		//	the hopup bracket we used as a handrail
		DeleteUnneeded1000mmHopups( GetOuter(), NORTH );

		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_NORTH_EAST );
			AutoAdjustStandard( CNR_NORTH_WEST );
		}
	}
	else if( !bIgnorNulls )
	{
		int		iLiftID;
		Lift	*pLift;

		//Each lift will also need a transom
		for( iLiftID=0; iLiftID<GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetLift( iLiftID );
			//does this lift already have a ledger?
			if( pLift->GetComponent( CT_LEDGER, LIFT_RISE_0000MM, NORTH )==NULL )
			{
				pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, LIFT_RISE_0000MM, MT_STEEL );
			}

			//does this lift have a deck or a stageboard?
			bool bDeck, bHopup;
			bDeck = pLift->IsDeckingLift();
			bHopup = pLift->GetComponent( CT_STAGE_BOARD, 0, NORTH )!=NULL;

			//We need a hand rail if there is either a Deck or a Hopup but not both (XOR)
			//	I can't remember the code for XOR!
			if( (bDeck || bHopup) && (!bDeck || !bHopup))
			{
				if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, NORTH )==NULL )
				{
					//better add a handrail!
					pLift->AddComponent( CT_RAIL, GetBayLength(), NORTH, LIFT_RISE_1000MM, MT_STEEL );

					if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, WEST )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, EAST )!=NULL ||
						pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, SOUTH )!=NULL )
					{
						pLift->AddComponent( CT_MESH_GUARD, GetBayLength(), NORTH, LIFT_RISE_1000MM, MT_STEEL );

						if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, WEST )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, EAST )!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, SOUTH )!=NULL )
						{
							pLift->AddComponent( CT_MESH_GUARD, GetBayLength(), NORTH, LIFT_RISE_2000MM, MT_STEEL );
						}
					}
					else
					{
						pLift->AddComponent( CT_MID_RAIL, GetBayLength(), NORTH, LIFT_RISE_0500MM, MT_STEEL );
						pLift->AddComponent( CT_TOE_BOARD, GetBayLength(), NORTH, LIFT_RISE_0000MM, MT_STEEL );
					}
				}
			}

			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets at 1.0m
			pLift->Add1000mmHopupIfReqd( ENE );
			pLift->Add1000mmHopupIfReqd( WNW );

			if( bAdjustStandards )
			{
				AutoAdjustStandard( CNR_NORTH_EAST );
				AutoAdjustStandard( CNR_NORTH_WEST );
			}
		}
		UpdateSchematicView();
	}
}

/////////////////////////////////////////////////////////////////////////////////
//SetInner
void Bay::SetInner( const Bay *pBay, bool bIgnorNulls/*=true*/, bool bAdjustStandards/*=true*/ )
{
	int		i, iNumberOfPlanks;
	Lift	*pLift;

	if( pBay==GetInner() )
	{
		//if we are not ignoring nulls and the new value is a
		//	null then don't leave this function yet!
		if( !(!bIgnorNulls && pBay==NULL ) )
		{
			//already done
			return;
		}
	}

	if( pBay!=NULL && GetInner()!=NULL )
	{
		assert( false );
	}

	SetInnerDumb(pBay);

	if( GetNumberOfBayComponents()<=0 )
	{
		return;
	}

/*	#ifdef _DEBUG
	if( GetInner()==NULL )
		acutPrintf( "\nBay%i Inner = NULL", GetBayNumber() );
	else
		acutPrintf( "\nBay%i Inner = Bay%i", GetBayNumber(), GetInner()->GetBayNumber() );
	#endif	//#ifdef _DEBUG
*/
	//if the new get inner is null then set these to invalid, if they are
	//	null also set them to invalid
	m_iLoadedInnerRunID = ID_INVALID;
	m_iLoadedInnerBayID = ID_INVALID;

	if( GetController()->IsDestructingController() )
		return;

	if( GetInner()!=NULL )
	{
		DeleteAllComponentsFromSide(SOUTH);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE );
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSW );
		GetInner()->DeleteAllComponentsFromSide(NORTH);
		GetInner()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNE );
		GetInner()->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNW );


		///////////////////////////////////////////////////////////////
		//Neighboring bays share hopups
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW );

			pLift->CheckForHopupDoubleups(ESE);
			pLift->CheckForHopupDoubleups(WSW);
		}

		//CHECK HANDRAILS at end of decking
		//go through each lift and see if it has decking
		//	if there is a neighbor
		//		if the neighbor doesn't have decking at this height
		//			if it has a handrail on any of the other sides,
		//				then copy that hand rail style to this side
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			if( pLift->IsDeckingLift() &&
				!DoesDeckLiftHaveNeighborDeckAtSameRL( i, GetInner() ) )
			{
				if( pLift->CopyHandrailToSide( N, S ) ||
					pLift->CopyHandrailToSide( E, S ) ||
					pLift->CopyHandrailToSide( W, S ) )
				{
					//handrail added! Do nothing!
				}
			}
		}

		DeleteUnneeded1000mmHopups( GetInner(), SOUTH );

		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_SOUTH_EAST );
			AutoAdjustStandard( CNR_SOUTH_WEST );
		}
	}
	else if( !bIgnorNulls)
	{
		int		iLiftID;
		Lift	*pLift;

		//Each lift will also need a transom
		for( iLiftID=0; iLiftID<GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetLift( iLiftID );
			//does this lift already have a transom?
			if( pLift->GetComponent( CT_LEDGER, LIFT_RISE_0000MM, SOUTH )==NULL )
			{
				pLift->AddComponent( CT_LEDGER, GetBayLength(), SOUTH, LIFT_RISE_0000MM, MT_STEEL );
			}

			if( IsSydneyCornerBay() )
			{
				//remove the southern hopup
				pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE, LIFT_RISE_0000MM );
				//pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSW, LIFT_RISE_0000MM );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, SOUTH );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, SOUTH );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, SOUTH );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, SOUTH );
				//if you delete the rail you must delete the matching hopup rails
				pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE, LIFT_RISE_1000MM );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW, LIFT_RISE_1000MM );
			}
			else
			{
				//does this lift have a deck or a stageboard?
				bool bDeck, bHopup;
				bDeck = pLift->IsDeckingLift();
				bHopup = pLift->GetComponent( CT_STAGE_BOARD, 0, SOUTH )!=NULL;

				//We need a hand rail if there is either a Deck or a Hopup but not both (XOR)
				//	I can't remember the code for XOR!
				if( (bDeck || bHopup) && (!bDeck || !bHopup))
				{
					if( bDeck )
					{
						//Don't allow a handrail on the sothern side! - see bug 1293
						1;			
					}
					else
					{
						if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, SOUTH )==NULL )
						{
							//better add a handrail!
							pLift->AddComponent( CT_RAIL, GetBayLength(), SOUTH, LIFT_RISE_1000MM, MT_STEEL );

							if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH )!=NULL ||
								pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, EAST )!=NULL ||
								pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, WEST )!=NULL )
							{
								pLift->AddComponent( CT_MESH_GUARD, GetBayLength(), SOUTH, LIFT_RISE_1000MM, MT_STEEL );

								if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH )!=NULL ||
									pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, EAST )!=NULL ||
									pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, WEST )!=NULL )
								{
									pLift->AddComponent( CT_MESH_GUARD, GetBayLength(), SOUTH, LIFT_RISE_2000MM, MT_STEEL );
								}
							}
							else
							{
								pLift->AddComponent( CT_MID_RAIL, GetBayLength(), SOUTH, LIFT_RISE_0500MM, MT_STEEL );
								pLift->AddComponent( CT_TOE_BOARD, GetBayLength(), SOUTH, LIFT_RISE_0000MM, MT_STEEL );
							}
						}
					}
				}

				//////////////////////////////////////////////////////////////////////
				//We may need to add hopup brackets due to ownership issues!
				if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, ESE )==NULL &&
					pLift->GetComponent( CT_STAGE_BOARD, 0, EAST )!=NULL &&
					GetForward()==NULL )
				{
					//we have a stage board without a hopup bracket!
					iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( EAST );
					pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, ESE, LIFT_RISE_0000MM, MT_STEEL );
				}

				if( pLift->GetComponent( CT_HOPUP_BRACKET, 0, WSW )==NULL &&
					pLift->GetComponent( CT_STAGE_BOARD, 0, WEST )!=NULL &&
					GetBackward()==NULL )
				{
					//we have a stage board without a hopup bracket!
					iNumberOfPlanks = pLift->GetNumberOfPlanksOnStageBoard( WEST );
					pLift->AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, WSW, LIFT_RISE_0000MM, MT_STEEL );
				}
			}


			//////////////////////////////////////////////////////////////////////
			//We may need to add hopup brackets at 1.0m
			pLift->Add1000mmHopupIfReqd( ESE );
			pLift->Add1000mmHopupIfReqd( WSW );
		}

		if( bAdjustStandards )
		{
			AutoAdjustStandard( CNR_SOUTH_EAST );
			AutoAdjustStandard( CNR_SOUTH_WEST );
		}

		UpdateSchematicView();
	}
}

/////////////////////////////////////////////////////////////////////////////////
//AddLapboardPointer
void Bay::AddLapboardPointer( LapboardBay *pLapboard, SideOfBayEnum eSide )
{
	m_pLapboards->Add( pLapboard );
	LapboardCrossesSideAdd( eSide );
}

/////////////////////////////////////////////////////////////////////////////////
//RemoveLapboardPointer
void Bay::RemoveLapboardPointer( const LapboardBay *pLapboard )
{
	int			i;
	LapboardBay *pLap;

	if( m_pLapboards!=NULL )
	{
		for( i=0; i<GetNumberOfLapboardPointers(); i++ )
		{
			pLap = m_pLapboards->GetAt(i);
			if( pLap==pLapboard )
				RemoveLapboardPointer(i);
		}
	}
}

void Bay::RemoveLapboardPointer( int iIndex )
{
	assert( iIndex>=0 );
	assert( iIndex<GetNumberOfLapboardPointers() );

	if( m_pLapboards!=NULL )
	{
		m_pLapboards->RemoveAt(iIndex);
		LapboardCrossesSideRemove(iIndex);
	}
}

/////////////////////////////////////////////////////////////////////////////////
//JoinBay
//Joins one bay to another, by setting the appropriate pointer.  It also owns
//	a UI to perform this task.  The UI to do this has not been worked out yet.
//	It maybe able to do this by looking at various proximities value, or at
//	least testing for proximities.
//void Bay::JoinBay( Bay *pBay, enum JoinType/JoinDirection )
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//JoinLapboard
//Joins one bay to a lapboard, by setting the appropriate pointer.  It also owns
//	a UI to perform this task.  The UI to do this has not been worked out yet.
//	It maybe able to do this by looking at various proximities value, or at
//	least testing for proximities.
//void Bay::JoinLapboard( Lapboard *pLapboard, enum JoinType/JoinDirection )
//{
//}


/////////////////////////////////////////////////////////////////////////////////
//Lifts
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetNumberOfLifts
//The number of lifts in the m_LiftList
int Bay::GetNumberOfLifts() const
{
	return m_LiftList.GetSize();
}

/////////////////////////////////////////////////////////////////////////////////
//GetLift
//return a lift given an ID
//	returns NULL if iLiftID is invalid
Lift *Bay::GetLift( int iLiftID ) const
{
	if( GetNumberOfLifts()<=0 )
	{
		return NULL;
	}
	return m_LiftList.GetLift( iLiftID );
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftAtRL
//Finds which lift occupies at a particular height (RL) from the height
//	stored in the m_LiftList, checks height of Standards first.  Since the
//	Height passed to us may not necessarily be the actual absolute height
//	for the bottom of the lift, we may need some way of returning the
//	actual height of the bottom of the lift, this would be useful to
//	determine the height, relative to the lift, of a certain star, since
//	we may know the absolute height of a star we need to add some
//	component to. 
//Returns NULL if the m_LiftList is empty or the Height is below the
//	bottom lift
Lift *Bay::GetLiftAtRL( double dRL ) const
{
	int iLiftID;
	iLiftID = GetLiftIDAtRL( dRL );
	if( iLiftID<0 )
	{
		//the return value was ID_NO_IDS or ID_NONE_MATCHING
		return NULL;
	}

	return GetLift( iLiftID );
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftIDAtRL
//Similar to above (GetLiftAtRL) except that it returns the LiftID
int	Bay::GetLiftIDAtRL( double dRL ) const
{
	double dHeight, dBottomLiftRL;

	GetLiftRL( 0, dBottomLiftRL );
	dHeight = dRL-dBottomLiftRL;

	return GetLiftIDAtHeight( dHeight );
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftAtHeight
//Same as GetLiftAtRL except that the height is relative to the bottom transom
//	/ledger combo
Lift *Bay::GetLiftAtHeight( double dHeight ) const
{
	int iLiftID;
	iLiftID = GetLiftIDAtHeight( dHeight );
	if( iLiftID<0 )
	{
		//the return value was ID_NO_IDS or ID_NONE_MATCHING
		return NULL;
	}

	return GetLift( iLiftID );
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftIDAtHeight
//Similar to above (GetLiftAtHeight) except that it returns the LiftID
int	Bay::GetLiftIDAtHeight( double dHeight ) const
{
	double dBottomLiftRL, dLiftTopRL, dLiftBottomRL, dHeightRL;
	int iLiftID, iSize;

	iSize = GetNumberOfLifts();
	if( iSize<=0 )
	{
		return ID_NO_IDS;
	}

	//find the RL of the bottom (lowest) transom/Ledger
	iLiftID = 0;
	GetLiftRL( iLiftID, dBottomLiftRL );
	dHeightRL = dHeight + dBottomLiftRL;

	//go through the lifts and find one which occupies the height
	for( iLiftID=0; iLiftID<iSize; iLiftID++ )
	{
		GetLiftRL( iLiftID, dLiftBottomRL );
		dLiftTopRL = GetRiseFromRiseEnum( GetLift( iLiftID )->GetRise(), GetStarSeparation() )+dLiftBottomRL;
		if( dHeightRL>=dLiftBottomRL-ROUND_ERROR && dHeightRL<dLiftTopRL-ROUND_ERROR )
		{
			//the height we are looking for lies within the range of this lift
			break;
		}
		if( iLiftID==iSize-1 && 
			dHeightRL>dLiftTopRL-ROUND_ERROR &&
			dHeightRL<dLiftTopRL+ROUND_ERROR )
		{
			//the height we are looking for is equal to the top of the top lift
			break;
		}
	}

	//there may be no match
	if( iLiftID>=iSize )					return ID_NONE_MATCHING;
	if( dHeightRL>dLiftTopRL+ROUND_ERROR )	return ID_NONE_MATCHING;

	return iLiftID;
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftComponent
//find the component within a lift or for a lift at a particular height.
//	Returns LIFT_TRUE, LIFT_FALSE or LIFT_MULTI_SELECT
bool Bay::GetLiftComponent( int iLiftID, ComponentTypeEnum eComponentType, SideOfBayEnum eSide, int iRise, Component **pComponent ) const
{
	int		iSize;

	*pComponent = NULL;

	iSize = GetNumberOfLifts();

	//bounds checking
	if( (iSize<=0) || (iLiftID<0) || (iLiftID>=iSize) )
	{
		return false;
	}

	//us the functionality of the component class
	*pComponent = GetLift( iLiftID )->GetComponent( eComponentType, iRise, eSide );

	return *pComponent!=NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftComponent
//find the component within a lift or for a lift at a particular height.
//	Returns LIFT_TRUE, LIFT_FALSE or LIFT_MULTI_SELECT
bool Bay::GetLiftComponent( double dHeight, ComponentTypeEnum eComponentType, SideOfBayEnum eSide, int iRise, Component **pComponent ) const
{
	Lift *pLift;
	pLift = GetLiftAtHeight( dHeight );
	return GetLiftComponent( pLift->GetLiftID(), eComponentType, eSide, iRise, pComponent );
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftRise
//retrieves the height of an individaul lift (0m-3m), relative to the bottom set
//	of transoms and ledgers owned by the lift itself.  This function can also
//	find the height of all lifts, if the heights are all the same.
//	possible return values are from LiftRiseEnum
LiftRiseEnum Bay::GetLiftRise( int iLiftID ) const
{
	Lift *pLift;
	int iSize;
	LiftRiseEnum eHeight;
	iSize = GetNumberOfLifts();

	if( iSize<=0 )
	{
		return LIFT_RISE_INVALID;
	}

	//We need the height of all lifts
	if( iLiftID == ID_ALL_IDS )
	{
		//find the height of the first lift
		iLiftID	= 0;
		pLift	= GetLift( iLiftID );
		assert( pLift!=NULL );
		eHeight	= pLift->GetRise();

		//move on to the next lift
		iLiftID++;

		//get the height of all supsequent lifts, they should be consistant
		while( iLiftID<iSize )
		{
			pLift = GetLift( iLiftID );
			assert( pLift!=NULL );
			if( eHeight != pLift->GetRise() )
			{
				//The heights are not consistant
				return LIFT_RISE_MULTI_SELECTION;
			}
			iLiftID++;
		}
		return eHeight;
	}

	//The liftID is for a valid lift, get its height
	pLift = GetLift( iLiftID );
	assert( pLift!=NULL );

	return pLift->GetRise();
}


/////////////////////////////////////////////////////////////////////////////////
//GetLiftRL
//find the RL for a lift
bool Bay::GetLiftRL( int iLiftID, double &dRL ) const
{
	int iSize;

	iSize = GetNumberOfLifts();

	dRL = 0.00;

	if( (iSize<=0) || (iLiftID<0) )
		return false;

	//We need the height of all lifts
	if( iLiftID == ID_ALL_IDS )
		return false;

	dRL = m_LiftList.GetRL( iLiftID );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetLiftHeight
//find the Height above the bottom transom/ledger
bool Bay::GetLiftHeight( int iLiftID, double &dHeight ) const
{
	double dLiftRL, dBottomRL;

	dHeight = 0.00;

	//find the height of the bottom lift
	if( !GetLiftRL( 0, dBottomRL ) )
		return false;

	//find the height of iLiftID
	if( !GetLiftRL( iLiftID, dLiftRL ) )
		return false;

	//we need the height relative to the bottom transom/ledger
	dHeight = dLiftRL-dBottomRL;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
//GetSurfaceArea
//retrieves the surface area for the given direction (N,E,S,W).  If
//	ALL_VISIBLE_SIDES retrieves the surface area using logic to
//	determine which sides are exposed, it can do this by looking at the
//	forward, backward and outer pointers.
double Bay::GetSurfaceArea( SideOfBayEnum eSide/*=ALL_VISIBLE_SIDES*/  ) const
{
	double		dLength, dHeight;
	Component	*pComponent;

	//set the values to default
	dLength = 0.00;
	dHeight = 0.00;

	pComponent = NULL;

	dLength = GetFaceWidth( eSide );

	//Face Height include the jack height,
	//	we don't want the jack for the surface area
	dHeight = GetFaceHeight( eSide ) - RLAdjust();

	return dLength * dHeight;
}


/////////////////////////////////////////////////////////////////////////////////
//Standards
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetHeightOfStandards
//retrieves the height of a standard
double Bay::GetHeightOfStandards( CornerOfBayEnum eStandardCorner /*=CNR_NORTH_EAST*/ ) const
{
	double dHeight;
	assert( eStandardCorner>=0 && eStandardCorner<4 );
	if( (eStandardCorner!=CNR_NORTH_EAST) &&
		(eStandardCorner!=CNR_SOUTH_EAST) &&
		(eStandardCorner!=CNR_NORTH_WEST) &&
		(eStandardCorner!=CNR_SOUTH_WEST) )
	{
		//invlid side
		assert( false );
		return 0.00;
	}
	dHeight = m_daHeights[eStandardCorner];
	if( eStandardCorner==CNR_SOUTH_EAST && GetInner()!=NULL )
	{
		dHeight = GetInner()->GetHeightOfStandards( CNR_NORTH_EAST );
	}
	else if( eStandardCorner==CNR_SOUTH_WEST && GetInner()!=NULL )
	{
		if( GetInner()->GetBackward()!=NULL )
		{
			dHeight = GetInner()->GetBackward()->GetHeightOfStandards( CNR_NORTH_EAST );
		}
		else
		{
			dHeight = GetInner()->GetHeightOfStandards( CNR_NORTH_WEST );
		}
	}
	else if( eStandardCorner==CNR_SOUTH_WEST && GetBackward()!=NULL )
	{
		if( GetBackward()->GetInner()!=NULL )
		{
			dHeight = GetBackward()->GetInner()->GetHeightOfStandards( CNR_NORTH_EAST );
		}
		else
		{
			dHeight = GetBackward()->GetHeightOfStandards( CNR_SOUTH_EAST );
		}
	}
	else if( eStandardCorner==CNR_NORTH_WEST && GetBackward()!=NULL )
	{
		dHeight = GetBackward()->GetHeightOfStandards( CNR_NORTH_EAST );
	}

	return dHeight;
}


/////////////////////////////////////////////////////////////////////////////////
//SetHeightOfStandards
//sets the height and RL of the Standards
bool Bay::SetHeightOfStandards( Point3D ptRL, double dHeight, CornerOfBayEnum eStandardCorner /*=CNR_ALL_SIDES*/ )
{
	if( (eStandardCorner<CNR_NORTH_EAST) || (eStandardCorner>CNR_SOUTH_WEST) )
		return false;

	m_daHeights[eStandardCorner] = dHeight;
	SetStandardPosition( ptRL, eStandardCorner );

	if( eStandardCorner==CNR_SOUTH_EAST )
	{
		if( GetInner()!=NULL )
		{
			ptRL.y+= GetInner()->GetBayWidthActual();
			GetInner()->SetHeightOfStandards( ptRL, dHeight, CNR_NORTH_EAST );
			ptRL.y-= GetInner()->GetBayWidthActual();
		}
	}
	else if( eStandardCorner==CNR_SOUTH_WEST )
	{
		if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
			{
				ptRL.y+= GetInner()->GetBackward()->GetBayWidthActual();
				ptRL.x+= GetInner()->GetBackward()->GetBayLengthActual();
				GetInner()->GetBackward()->SetHeightOfStandards( ptRL, dHeight, CNR_NORTH_EAST );
				ptRL.y-= GetInner()->GetBackward()->GetBayWidthActual();
				ptRL.x-= GetInner()->GetBackward()->GetBayLengthActual();
			}
			else
			{
				ptRL.y+= GetInner()->GetBayWidthActual();
				GetInner()->SetHeightOfStandards( ptRL, dHeight, CNR_NORTH_WEST );
				ptRL.y-= GetInner()->GetBayWidthActual();
			}
		}
		else if( GetBackward()!=NULL )
		{
			ptRL.x+= GetBackward()->GetBayLengthActual();
			GetBackward()->SetHeightOfStandards( ptRL, dHeight, CNR_SOUTH_EAST );
			ptRL.x-= GetBackward()->GetBayLengthActual();
		}
	}
	else if( eStandardCorner==CNR_NORTH_WEST )
	{
		if( GetBackward()!=NULL )
		{
			ptRL.x+= GetBackward()->GetBayLengthActual();
			GetBackward()->SetHeightOfStandards( ptRL, dHeight, CNR_NORTH_EAST );
			ptRL.x-= GetBackward()->GetBayLengthActual();
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetStandardPosition
Point3D Bay::GetStandardPosition(CornerOfBayEnum eCorner) const
{
	Point3D	pt, ptNeighbor;

	pt = m_ptaStandardsRL[eCorner];
	if( eCorner==CNR_SOUTH_EAST && GetInner()!=NULL )
	{
		ptNeighbor = GetInner()->GetStandardPosition( CNR_NORTH_EAST );
		ptNeighbor.y-= GetInner()->GetBayWidthActual();
//		if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
		if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
			pt = ptNeighbor;
	}
	else if( eCorner==CNR_SOUTH_WEST && GetInner()!=NULL )
	{
		if( GetInner()->GetBackward()!=NULL )
		{
			ptNeighbor = GetInner()->GetBackward()->GetStandardPosition( CNR_NORTH_EAST );
			ptNeighbor.x-= GetInner()->GetBackward()->GetBayLengthActual();
			ptNeighbor.y-= GetInner()->GetBackward()->GetBayWidthActual();
//			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
				pt = ptNeighbor;
		}
		else
		{
			ptNeighbor = GetInner()->GetStandardPosition( CNR_NORTH_WEST );
			ptNeighbor.y-= GetInner()->GetBayWidthActual();
//			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
				pt = ptNeighbor;
		}
	}
	else if( eCorner==CNR_SOUTH_WEST && GetBackward()!=NULL )
	{
		if( GetBackward()->GetInner()!=NULL )
		{
			ptNeighbor = GetBackward()->GetInner()->GetStandardPosition( CNR_NORTH_EAST );
			ptNeighbor.x-= GetBackward()->GetInner()->GetBayLengthActual();
			ptNeighbor.y-= GetBackward()->GetInner()->GetBayWidthActual();
//			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
				pt = ptNeighbor;
		}
		else
		{
			ptNeighbor = GetBackward()->GetStandardPosition( CNR_SOUTH_EAST );
			ptNeighbor.x-= GetBackward()->GetBayLengthActual();
//			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
			if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
				pt = ptNeighbor;
		}
	}
	else if( eCorner==CNR_NORTH_WEST && GetBackward()!=NULL )
	{
		ptNeighbor = GetBackward()->GetStandardPosition( CNR_NORTH_EAST );
		ptNeighbor.x-= GetBackward()->GetBayLengthActual();
//		if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y && ptNeighbor.z<pt.z )
		if( ptNeighbor.x==pt.x && ptNeighbor.y==pt.y )
			pt = ptNeighbor;
	}
	return pt;
}

/////////////////////////////////////////////////////////////////////////////////
//SetStandardPosition
bool Bay::SetStandardPosition( Point3D ptStandard, CornerOfBayEnum eCorner )
{
	m_ptaStandardsRL[eCorner] = ptStandard;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetHeightOfBay
double Bay::GetHeightOfBay() const
{
	double dHeight;
	GetLiftHeight( 0, dHeight );
	return dHeight;
}


/////////////////////////////////////////////////////////////////////////////////
//GetStandard
//Retrieves a standard from the ordered list of standards
//	ground to top!
Component *Bay::GetStandard( int iStandardPosition, CornerOfBayEnum eStandardCorner ) const
{
	int					iComponentID, iSize;
	Component			*pComponent;
	ComponentTypeEnum	CompType;

	pComponent = NULL;

	iSize = GetNumberOfBayComponents();
	for( iComponentID=0; iComponentID<iSize; iComponentID++ )
	{
		CompType = m_caComponents.GetType( iComponentID );
		if( CompType==CT_STANDARD || CompType==CT_STANDARD_OPENEND )
		{
			//This is a standard, is it the correct pole?
			if( GetPosition( iComponentID )==CornerAsSideOfBay(eStandardCorner) )
			{
				//we are looking for standard number iStandardPosition!
				//??? JSB todo 990726 - if we don't have a GetRise function
				//	then we have could use the function below, assuming the standards are
				//	ordered within the list.
				//if( iStandardPosition>0 )
				//{
					//iStandardPosition--;
					//continue;
				//}

				//This is the correct pole, is it the correct standard?
				if( GetComponentsRise( iComponentID )==iStandardPosition )
				{
					//We have found it

					//copy the Length of the component
					pComponent = m_caComponents.GetComponent( iComponentID );
					break;
				}
			}
		}
	}

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//GetStandardsArrangement
//retrieves an ORDERED array of lengths for each Standard, ordered from
//	ground to top!
bool Bay::GetStandardsArrangement( doubleArray &daArrangement, CornerOfBayEnum eStandardCorner/*=CNR_NORTH_EAST*/ ) const
{
	bool				bFound;
	int					iComponentID, iSize;
	double				dLength;
	Component			*pComponent;
	ComponentTypeEnum	CompType;

	switch( eStandardCorner )
	{
	case( CNR_NORTH_EAST ):
		//This bay always owns the north-east standard
		break;
	case( CNR_SOUTH_EAST ):
		if( GetInner()!=NULL )
		{
			return GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
		}
		break;
	case( CNR_SOUTH_WEST ):
		if( GetBackward()!=NULL )
		{
			if( GetBackward()->GetInner()!=NULL )
				return GetBackward()->GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
			else
				return GetBackward()->GetStandardsArrangement( daArrangement, CNR_SOUTH_EAST );
		}
		else if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
				return GetInner()->GetBackward()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
			else
				return GetInner()->GetStandardsArrangement( daArrangement, CNR_NORTH_WEST );
		}
		break;
	case( CNR_NORTH_WEST ):
		if( GetBackward()!=NULL )
		{
			return GetBackward()->GetStandardsArrangement( daArrangement, CNR_NORTH_EAST );
		}
		break;
	default:
		assert( false );
		return false;
	}

	bFound = false;
	daArrangement.RemoveAll();
	iSize = GetNumberOfBayComponents();
	for( iComponentID=0; iComponentID<iSize; iComponentID++ )
	{
		CompType = m_caComponents.GetType( iComponentID );

		if( CompType==CT_STANDARD || CompType==CT_STANDARD_OPENEND )
		{
			//This is a standard, is it the correct pole?
			if( GetPosition( iComponentID )==CornerAsSideOfBay(eStandardCorner) )
			{
				//We have found one

				//copy the Length of the component
				pComponent = m_caComponents.GetComponent( iComponentID );
				dLength = pComponent->GetLengthCommon();
				assert( dLength>0.00 );
				daArrangement.Add( ConvertRLtoStarRL(dLength, GetStarSeparation() ) );
				
				bFound = true;
			}
		}
	}

	return bFound;
}

/////////////////////////////////////////////////////////////////////////////////
//SetStandardsArrangement
//replaces existing arrangement with new ordered arrangement (Ground to
//	Top).  Also places a Jack at the bottom of the pole
//	???JSB todo 990716 - this function should pass in the material type.
bool Bay::SetStandardsArrangement( doubleArray &daArrangement, CornerOfBayEnum eStandardCorner/*=CNR_NORTH_EAST*/, bool bSoleboard/*=false*/ )
{
	int					i;
	bool				bMoveX, bMoveY;
	double				dLength, dAngle, dHeight;
	Point3D				ptFirst, ptSecond, ptNode;
	Matrix3D			Rotation, Spin, Transform, Translation, Correction;
	Vector3D			Vector;
	Component			*pComponent;
	MaterialTypeEnum	eMaterial;
	ComponentTypeEnum	eType;

	assert( m_bAllowRecreateStandards );

	ptFirst = GetStandardPosition(eStandardCorner);

	dHeight = daArrangement.GetTotal();

	//delete the standards
	DeleteStandards( eStandardCorner );

	switch( eStandardCorner )
	{
	case( CNR_NORTH_EAST ):
		break;
	case( CNR_SOUTH_EAST ):
		if( GetInner()!=NULL )
		{
			if( dHeight>GetInner()->GetHeightOfStandards( CNR_NORTH_EAST ) )
			{
				return GetInner()->SetStandardsArrangement( daArrangement, CNR_NORTH_EAST, bSoleboard );
			}
			return false;
		}
		break;
	case( CNR_SOUTH_WEST ):
		if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
			{
				if( dHeight>GetInner()->GetBackward()->GetHeightOfStandards( CNR_NORTH_EAST ) )
				{
					return GetInner()->GetBackward()->SetStandardsArrangement( daArrangement, CNR_NORTH_EAST, bSoleboard );
				}
			}
			else
			{
				if( dHeight>GetInner()->GetHeightOfStandards( CNR_NORTH_WEST ) )
				{
					return GetInner()->SetStandardsArrangement( daArrangement, CNR_NORTH_WEST, bSoleboard );
				}
			}
			return false;
		}
		if( GetBackward()!=NULL )
		{
			if( GetBackward()->GetInner()!=NULL )
			{
				if( dHeight>GetBackward()->GetInner()->GetHeightOfStandards( CNR_NORTH_EAST ) )
				{
					return GetBackward()->GetInner()->SetStandardsArrangement( daArrangement, CNR_NORTH_EAST, bSoleboard );
				}
			}
			else
			{
				if( dHeight>GetBackward()->GetHeightOfStandards( CNR_SOUTH_EAST ) )
				{
					return GetBackward()->SetStandardsArrangement( daArrangement, CNR_SOUTH_EAST, bSoleboard );
				}
			}
			return false;
		}
		break;
	case( CNR_NORTH_WEST ):
		if( GetBackward()!=NULL )
		{
			if( dHeight>GetBackward()->GetHeightOfStandards( CNR_NORTH_EAST ) )
			{
				return GetBackward()->SetStandardsArrangement( daArrangement, CNR_NORTH_EAST, bSoleboard );
			}
			return false;
		}
		break;
	default:
		return false;
	}
	
	m_daHeights[eStandardCorner]=0.00;

	eMaterial = MT_STEEL;

	////////////////////////////////////////////////////////////////
	//Standards and Jacks are vertical, so we need to rotate them
	Vector.set( 0.00, 1.00, 0.00 );
	dAngle = pi/-2.00;
	Rotation.setToRotation( dAngle, Vector );

	//Mark says Wedge near spriget should face inward!
	Vector.set( 0.00, 0.00, 1.00 );
	switch( eStandardCorner )
	{
	case( CNR_NORTH_EAST ):	//fallthrough
	case( CNR_NORTH_WEST ):
		dAngle = pi;
		break;
	case( CNR_SOUTH_EAST ):	//fallthrough
	case( CNR_SOUTH_WEST ):
		dAngle = 0.00;
		break;
	default:
		assert( FALSE );
	}

	Spin.setToRotation( dAngle, Vector );
	Rotation = Spin * Rotation;

	////////////////////////////////////////////
	//Add the standards
	for ( i=0; i<daArrangement.GetSize(); i++ )
	{
		dLength = daArrangement.GetAt( i );
		
		eType = CT_STANDARD;
		#ifdef ALLOW_OPEN_END_STANDARDS
		if( i==daArrangement.GetSize()-1 )
			eType = CT_STANDARD_OPENEND;
		#endif	//#ifdef ALLOW_OPEN_END_STANDARDS

		pComponent = CreateComponent( eType, CornerAsSideOfBay(eStandardCorner), i,
									ConvertStarRLtoRL(dLength, GetStarSeparation() ), eMaterial );
		//location
		ptSecond	= ptFirst;
		//The component length includes the spiget
		ptSecond.z += pComponent->GetLengthActual();
		ptSecond.z -= pComponent->GetAcurate3DAdjust(Z_AXIS);
		//we need to move the standard to the first point
		Vector = FromOriginToPointAsVector( ptFirst );
		Translation.setTranslation( Vector );
		Transform.setToIdentity();
		Transform = Translation * Rotation;
		Transform = Correction * Transform;

		pComponent->Move( Transform, true );
		//does this require movement if the bay size is changed?
		switch( eStandardCorner )
		{
		case( CNR_NORTH_EAST ):	bMoveX = true; bMoveY = true; break;
		case( CNR_SOUTH_EAST ):	bMoveX = true; bMoveY = false; break;
		case( CNR_NORTH_WEST ):	bMoveX = false; bMoveY = true; break;
		case( CNR_SOUTH_WEST ):	bMoveX = false; bMoveY = false; break;
		default:
			assert( FALSE );
		}
		pComponent->SetMoveWhenBayLengthChanges( bMoveX );
		pComponent->SetMoveWhenBayWidthChanges( bMoveY );
		//star nodes
		pComponent->AddNode( ptFirst );
		pComponent->AddNode( ptSecond );
		ptNode = ptFirst;
		ptNode.z+= GetStarSeparation();
		while( ptNode.z<ptSecond.z )
		{
			pComponent->AddNode( ptNode );
			ptNode.z+= GetStarSeparation();
		}
		//Adjust the height of the pole
		m_daHeights[eStandardCorner]+= dLength;
		ptFirst = ptSecond;
	}

	if( daArrangement.GetSize()>0 )
	{
		////////////////////////////////////////////
		//We always have a Jack
		AddAJack( CornerAsSideOfBay(eStandardCorner), eMaterial );
		//m_daHeights[eStandardCorner]-=RLAdjust();

		////////////////////////////////////////////
		//Do we need a sole board?
		if( bSoleboard )
			AddASoleboard( CornerAsSideOfBay(eStandardCorner), MT_TIMBER );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//CreateStandardsArrangment
// This is used to automatically create a set of standards, the lengths
//	used will depend upon the location of the standards.  The default
//	argument would also look at which standards are owned by the bay, and
//	determine a height for that bay.  Also should create jacks.
//	the function which call this will have to consider the fit of the standard
//	(i.e. STD_FIT_COURSE/STD_FIT_FINE/STD_FIT_SAME ) since this should be used
//	to determine 'dHeight'
bool Bay::CreateStandardsArrangment( double dHeight, CornerOfBayEnum eStandardCorner/*=CNR_NORTH_EAST*/, bool bSoleboard/*=false*/ )
{
	bool		bInner;
	doubleArray	daArrangement;

	if( dHeight<ROUND_ERROR )
		return false;

	switch( eStandardCorner )
	{
		case( CNR_NORTH_EAST ):
		case( CNR_NORTH_WEST ):
			bInner = false;
			break;
		case( CNR_SOUTH_EAST ):
		case( CNR_SOUTH_WEST ):
			bInner = true;
			break;
		default:
			assert( FALSE );
			break;
	}

	MakeStandardsArrangment( daArrangement, dHeight, bInner, m_bStaggeredHeights, m_eStandardsFit );

	return SetStandardsArrangement( daArrangement, eStandardCorner, bSoleboard );
}

void Bay::MakeStandardsArrangment( doubleArray &daArrangement, double dHeight, bool bInnerStandard, bool bStaggered, StandardFitEnum eStandardsFit )
{
	int			iStars;
	double		dNewStandardHeight, dHeightRemaining;

	//////////////////////////////////////////////////////////////
	//we need to create a double array with standards in the
	//	correct height order
	//////////////////////////////////////////////////////////////
#ifdef _DEBUG
	int		iTemp;
	double	dTemp, dRemainder;
	dTemp = dHeight/GetStarSeparation();
	iTemp = (int)(dTemp+0.50);
	dRemainder = dTemp-(double)iTemp;
	assert( dRemainder<ROUND_ERROR_SMALL && dRemainder>(-1.00*ROUND_ERROR_SMALL) );
#endif	//#ifdef _DEBUG

	daArrangement.RemoveAll();
	//we must have at least one standard!
	while( daArrangement.GetSum()<dHeight-ROUND_ERROR )
	{
		//default to 3m
		dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation());

		//if this is the first standard, and we are using staggering standards
		if( daArrangement.GetSize()<=0 && bStaggered )
		{
			//set the start height, depending upon if the bay is staggered or not
			if( bInnerStandard )
				dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());
			else
				dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation());
		}

		//Do we need to use the full standard here?
		if( dHeight-daArrangement.GetSum()<dNewStandardHeight )
		{
			//we don't need a 3 meter standard, or do we?
			switch( eStandardsFit )
			{
				case( STD_FIT_COURSE ):
					//use the exact height.  Note that Standards only come in
					//	1.0m, 1.5m, 2.0m, 2.5m and 3.0m.
					if( daArrangement.GetSize()>0 &&
						(dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())+ROUND_ERROR &&
						(dHeight-daArrangement.GetSum())<=ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation())+ROUND_ERROR )
					{
						//1x2.0m standard is fine
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());
					}
					else if( (dHeight-daArrangement.GetSum())<=ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())+ROUND_ERROR )
					{
						//1x2.0m or 2x2.0m standards
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());
						if( daArrangement.GetSize()>0 )
						{
							//2x2.0m standard
							daArrangement.RemoveAt( daArrangement.GetSize()-1 );
							daArrangement.Add( dNewStandardHeight );
						}
					}
					else
					{
						//1x3.0m
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation());
					}
					break;
				case( STD_FIT_FINE ):	//fallthrough
				case( STD_FIT_SAME ):
					//It is the responsibility of the calling function to 
					//	set the height, so if 'same' is used then no adjustment
					//	will be made here to the height.
					//	Note that Standards only come in 1.0m, 1.5m, 2.0m, 2.5m
					//	and 3.0m.
					if( daArrangement.GetSize()>0 &&
						daArrangement.GetAt( daArrangement.GetSize()-1 )>ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())-ROUND_ERROR &&
						daArrangement.GetAt( daArrangement.GetSize()-1 )<ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())+ROUND_ERROR &&
						(dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())-ROUND_ERROR &&
						(dHeight-daArrangement.GetSum())<ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())+ROUND_ERROR )
					{
						//replace the standard below with a 2 meter standard, we
						//	will use 2x2m standards instead
						//	thus we will replace the 1x3m + 1x1m with 2x2m
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());
						daArrangement.RemoveAt( daArrangement.GetSize()-1 );
						daArrangement.Add( dNewStandardHeight );
					}
					else if( daArrangement.GetSize()>0 &&
						daArrangement.GetAt( daArrangement.GetSize()-1 )>ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation())-ROUND_ERROR &&
						daArrangement.GetAt( daArrangement.GetSize()-1 )<ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation())+ROUND_ERROR &&
						(dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())-ROUND_ERROR &&
						(dHeight-daArrangement.GetSum())<ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())+ROUND_ERROR)
					{
						//this must be a staggared arrangement to have a 2m standard!
						//	so lets replace the 1x2m + 1x1m with a 1x3m
						assert( daArrangement.GetSize()==1 && bStaggered );
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation());
						//just remove the one below, and at the bottom of this funct we will add the 3m
						daArrangement.RemoveAt( daArrangement.GetSize()-1 );
					}
					else if( (dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())-ROUND_ERROR &&
							 (dHeight-daArrangement.GetSum())<ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation())+ROUND_ERROR)
					{
						//but if it is, we will just add a 1x1m
						dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation());
					}
					else if( (dHeight-daArrangement.GetSum())<ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation()) )
					{
						if( (dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_0500, GetStarSeparation())+ROUND_ERROR )
						{
							//it will be a little bit over but that is ok!
							dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation());
						}
						else
						{
							//This standards is less than 1m long
							if( daArrangement.GetSize()<1 )
							{
								//There are no other standards in the array so far,
								//	so we are forced to use 1x1m length
								dNewStandardHeight = ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation());
							}
							else
							{
								//remove the last standard and replace it with one which
								//	is 500mm shorter(GetStarSeparation()) and try again
								daArrangement.RemoveAt( daArrangement.GetSize()-1 );
								dNewStandardHeight-= GetStarSeparation();
							}
						}
					}
					else
					{
						///////////////////////////////////////////////////////////////////////
						//we know that we have to fit a standard that is between 1m and 3m
						//	long
						assert( (dHeight-daArrangement.GetSum())<ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation()) );
						assert( (dHeight-daArrangement.GetSum())>ConvertRLtoStarRL(COMPONENT_LENGTH_1000, GetStarSeparation()) );

						///////////////////////////////////////////////////////////////////////
						//how many stars does this cover
						dHeightRemaining = (dHeight-daArrangement.GetSum())/GetStarSeparation();
						//round up
						iStars = (int)dHeightRemaining;
						if( (dHeightRemaining-(double)iStars)>ROUND_ERROR_SMALL )
							iStars++;

						///////////////////////////////////////////////////////////////////////
						//We now know the height of the standard we need
						dNewStandardHeight = double( iStars*GetStarSeparation() );
					}
					break;
				default:
					assert( FALSE ); //What the fuck is this? (eStandardsFit is invalid)
					break;
			}
		}

		daArrangement.Add( dNewStandardHeight );
	}
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteStandards
//destroys all the standards for a set of standards
bool Bay::DeleteStandards( CornerOfBayEnum eStandardCorner )
{
	int					iComponentID;
	bool				bFound;
	ComponentTypeEnum	CompType;
	SideOfBayEnum		eSide, eSideComp;

	bFound = false;

	eSideComp = CornerAsSideOfBay(eStandardCorner);

	if( (eStandardCorner>=CNR_NORTH_EAST) && (eStandardCorner<=CNR_SOUTH_WEST) )
	{
		//go through each of the componenents, the Size of the Array will change, so
		//	we need to call GetSize here, and not use a local variable 'iSize'
		for( iComponentID=0; iComponentID<GetNumberOfBayComponents(); iComponentID++ )
		{
			CompType = m_caComponents.GetType( iComponentID );
			if( CompType==CT_STANDARD	|| CompType==CT_STANDARD_OPENEND	||
				CompType==CT_JACK		|| CompType==CT_SOLEBOARD			)
			{
				//This is a standard, is it the correct pole?
				eSide = GetPosition( iComponentID );
				if( eSide==eSideComp )
				{
					//we have a found a matching component
					m_caComponents.DeleteComponent(iComponentID);
					bFound = true;
					//we have just removed an element from the list, and thus
					//	all elements after this will move down, so don't increase
					//	the componentID
					iComponentID--;
				}
			}
		}
	}

	return bFound;
}


/////////////////////////////////////////////////////////////////////////////////
//Braces
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetBracesArrangement
//not required
//GetBracesArrangement()
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CreateBracingArrangment
//creates a set of bracing using default spacing.
BracingReturnEnums Bay::CreateBracingArrangment ( SideOfBayEnum eSide, MaterialTypeEnum eMaterial, bool bReplaceStageboards/*=false*/ )
{
	//This function only creates an arrangment and then lets SetBracingArrangement
	//	do all the hard work of creating the actual arrangment.
	double			dWidth, dRaise, dLength, dBraceSeparation,
					dRL, dRLTop, dRLBtm,
					daHeights[4];
	doubleArray		daArrangement;
	int				iNumberOfBraces;

	bReplaceStageboards;

	//the bracing will be spaced 1 star apart
	dBraceSeparation	= GetStarSeparation();

#ifdef _DEBUG
	if( eSide==EAST )
	{
		dBraceSeparation	= GetStarSeparation();
	}
#endif

	//////////////////////////////////////////////////////////
	//Length of bracing required depends upon the width the bracing has to span
	dWidth = 0.00;
	switch( eSide )
	{
	case( NORTH ):
		//If there is a bay to the north then we don't need bracing here
		if( GetOuter()!=NULL )
			return BRACING_ERROR_STANDARDS;

		GetBracingDimensions( GetBackward(), CNR_NORTH_EAST, CNR_NORTH_WEST, daHeights );

		dWidth = GetBayLength();
		break;
	case( EAST ):
		//If there is a bay to the east then we don't need bracing here
		if( GetForward()!=NULL )
			return BRACING_ERROR_STANDARDS;

		GetBracingDimensions( GetInner(), CNR_NORTH_EAST, CNR_SOUTH_EAST, daHeights );

		dWidth = GetBayWidth();
		break;
	case( SOUTH ):
		//If there is a bay to the south then we don't need bracing here
		if( GetInner()!=NULL )
			return BRACING_ERROR_STANDARDS;

		GetBracingDimensions( GetBackward(), CNR_SOUTH_EAST, CNR_SOUTH_WEST, daHeights );

		dWidth = GetBayLength();
		break;
	case( WEST ):
		//If there is a bay to the west then we don't need bracing here
		if( GetBackward()!=NULL )
			return BRACING_ERROR_STANDARDS;

		GetBracingDimensions( GetInner(), CNR_NORTH_WEST, CNR_SOUTH_WEST, daHeights );

		dWidth = GetBayWidth();
		break;
	default:					//fallthrough
		assert( FALSE );	//we couldn't be talking about bracing something has gone wrong!
		return BRACING_ERROR_ENUM;
	}
	//we can only place bracing where the standards overlap
	dRLTop = min( daHeights[0]+daHeights[1], daHeights[2]+daHeights[3] );
	//dHeight is the RL where the bottom brace is placed.
	dRLBtm = max( daHeights[1], daHeights[3] );

	//////////////////////////////////////////////////////////
	//This is the common length for the bracing component!
	//	NOTE: bracing is diagonal, so this will be longer than
	//	the bay width
	dLength = GetBracingLength( dWidth );

	//////////////////////////////////////////////////////////
	//don't forget that the bracing is seperated by one star
	//dRaise = ConvertRLtoStarRL(GetBracingRaise( dWidth, dLength ), GetStarSeparation());
	dRaise = ConvertRLtoStarRL(GetBracingRaise( dWidth ), GetStarSeparation());
	dRaise+= dBraceSeparation;

	//////////////////////////////////////////////////////////
	//Buttress bays have one extra brace at the top, however it
	//	needs to finish such that the top of that brace is on the
	//	south, yet start at the bottom
	double dBracesInBay;
	if( GetBayType()==BAY_TYPE_BUTTRESS && ( eSide==EAST || eSide==WEST ) )
	{
		dRLTop+= (dRaise-GetStarSeparation());
		double dGapAtTop;
		//Exactly many braces will we get(in real numbers)?
		dBracesInBay = (dRLTop-dRLBtm)/dRaise;
		//What is the distance between the top brace and the top of the bay?
		dGapAtTop = dBracesInBay-double(int(dBracesInBay));
		//The top brace should now finish dGapAtTop below the top of the bay
		dRLTop-= (dGapAtTop*dRaise);
	}

	//////////////////////////////////////////////////////////
	//How many braces can we fit?  Round down!
	assert( dRaise!=0.00 );
	dBracesInBay = (dRLTop-dRLBtm)/dRaise;
	iNumberOfBraces = (int)dBracesInBay;

	//////////////////////////////////////////////////////////
	//Create an array of RL's for the bottom of each brace!
	if( GetBayType()==BAY_TYPE_BUTTRESS && ( eSide==EAST || eSide==WEST ) )
	{
		daArrangement.RemoveAll();
		dRL = dRLTop - dRaise;
		while( dRL>=dRLBtm && iNumberOfBraces>0 )
		{
			daArrangement.Add( dRL );
			dRL-= dRaise;
			iNumberOfBraces--;
		}
	}
	else
	{
		daArrangement.RemoveAll();
		dRL = dRLBtm;
		while( dRL<dRLTop && iNumberOfBraces>0 )
		{
			daArrangement.Add( dRL );
			dRL+= dRaise;
			iNumberOfBraces--;
		}
	}

	return SetBracingArrangement( daArrangement, eSide, eMaterial );
}

/////////////////////////////////////////////////////////////////////////////////
//SetBracingArrangement
//Uses the RL's in daArrangment as a list of starting positions for the bracing
BracingReturnEnums Bay::SetBracingArrangement ( doubleArray &daArrangement,
											   SideOfBayEnum eSide,
											   MaterialTypeEnum eMaterial )
{
	int					i;
	bool				bMoveX, bMoveY, bZigZag;
	double				dLength, dLengthActual, dAngle,
						dWidth,	dWidthActual, dStandardWidth;
	Point3D				Standards[4], Centroid;
	Matrix3D			Rotation, Spin, Transform, Translation, Aux,
						ZigZagTrans, Trans;
	Vector3D			Vector, ZigZagVector, Vect;
	Component			*pComponent;
	CornerOfBayEnum		eCnr;

	/////////////////////////////////////////////////////
	//Is this ZigZag bracing?
	bZigZag = true;

	/////////////////////////////////////////////////////
	//Check the arrangment
	if( daArrangement.GetSize()<=0 )
		return BRACING_ERROR_ARRANGMENT;

	//////////////////////////////////////////////////////////////////
	//Get the positions of the 4 standards for referance
	Standards[CNR_SOUTH_EAST] = GetStandardPosition( CNR_SOUTH_EAST );
	Standards[CNR_NORTH_EAST] = GetStandardPosition( CNR_NORTH_EAST );
	Standards[CNR_SOUTH_WEST] = GetStandardPosition( CNR_SOUTH_WEST );
	Standards[CNR_NORTH_WEST] = GetStandardPosition( CNR_NORTH_WEST );

	//////////////////////////////////////////////////////////////////
	//We will build this flat on the ground with the SE corner as the origin
	Standards[CNR_SOUTH_EAST].z = 0.00;
	Standards[CNR_NORTH_EAST].z = 0.00;
	Standards[CNR_SOUTH_WEST].z = 0.00;
	Standards[CNR_NORTH_WEST].z = 0.00;

	//////////////////////////////////////////////////////////////////
	//calculate the rotation required for the lift so that 
	//	the SW standard is at the origin, with the ledger along the +ve
	//	x axis, the transom along the +ve Yaxis and the lift extending
	//	upward in the +ve z direction!
	Vect = Standards[CNR_SOUTH_EAST] - Standards[CNR_SOUTH_WEST];

	dAngle = CalculateAngle( Vect )/2.00;
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( dAngle, Vector );
	Transform = Rotation;

	//Length of bracing required depends upon the width the bracing has to span
	dWidth = 0.00;
	switch( eSide )
	{
		case( NORTH ):			//fallthrough
		case( SOUTH ):
			dWidth = GetBayLength();
			dWidthActual = GetBayLengthActual();
			break;
		case( EAST ):			//fallthrough
		case( WEST ):
			dWidth = GetBayWidth();
			dWidthActual = GetBayWidthActual();
			break;
		default:
			assert( false );	//we couldn't be talking about bracing something has gone wrong!
			return BRACING_ERROR_ENUM;
	}
	assert( dWidth>0.00 );
	dLength = GetBracingLength( dWidth );
	assert( dLength>0.00 );
	dLengthActual	= GetCompDetails()->GetActualLength( GetSystem(), CT_BRACING, dLength, eMaterial );
	dStandardWidth	= GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, eMaterial );

	////////////////////////////////////////////////////////////////
	//B4 the rotation, we need to centre the hole at the origin
	double	dHoleXPos;
	switch( GetCompDetails()->GetSystem() )
	{
	case( S_MILLS ):
		dHoleXPos = BRACING_MILLS_HOLE_TO_EDGE;
		break;
	case( S_KWIKSTAGE ):
		dHoleXPos = BRACING_KWIKS_HOLE_TO_EDGE;
		break;
	case( S_OTHER ):
	default:
		dHoleXPos = 0.00;
		assert( false );
	}
	Vector.set( -1.00*dHoleXPos, 0.00, 0.00 );
	Aux.setTranslation( Vector );
	Transform = Aux * Transform;

	////////////////////////////////////////////////////////////////
	//Braces are at an angle, so we need to rotate them around the Yaxis
	Vector.set( 0.00, 1.00, 0.00 );
	assert( dLengthActual!=0.00 );
	assert( dLengthActual!=(2.00*dHoleXPos));
	dAngle = -1.00*acos(dWidthActual/(dLengthActual-(2.00*dHoleXPos)));
	Rotation.setToRotation( dAngle, Vector );
	Transform = Rotation * Transform;

	////////////////////////////////////////////////////////////////
	//Move the brace so it is outside the bay and positioned on the south side
	Vector.set( 0.00, 0.00, 1.00 );
	dAngle = pi;
	Rotation.setToRotation( dAngle, Vector );
	Transform = Rotation * Transform;

	//this movement will require a translation
	Vector.set( dWidthActual, -1.00*dStandardWidth, 0.00 );
	double dAdjust;
	switch( eSide )
	{
	case( NORTH ):
	case( SOUTH ):
		switch( GetCompDetails()->GetSystem() )
		{
		case( S_MILLS ):
			dAdjust = LOW_STAR_MILLS_Z_ADJUST;
			break;
		case( S_KWIKSTAGE ):
			dAdjust = LOW_STAR_KWIKS_Z_ADJUST;
			break;
		case( S_OTHER ):
		default:
			dAdjust = 0.00;
			assert( false );
		}
		break;
	case( EAST ):
	case( WEST ):
		switch( GetCompDetails()->GetSystem() )
		{
		case( S_MILLS ):
			dAdjust = HIGH_STAR_MILLS_Z_ADJUST;
			break;
		case( S_KWIKSTAGE ):
			dAdjust = HIGH_STAR_KWIKS_Z_ADJUST;
			break;
		case( S_OTHER ):
		default:
			dAdjust = 0.00;
			assert( false );
		}
		break;
	default:
		assert( false );
	}
	Vector.z+= dAdjust;
	Aux.setTranslation( Vector );
	Transform = Aux * Transform;

	////////////////////////////////////////////////////////////////
	//These indicate if we have to reposition this component during resizing
	bMoveX = false;
	bMoveY = false;

	////////////////////////////////////////////////////////////////
	//Position the brace on the correct side of the bay
	switch( eSide )
	{
	case( NORTH ):
		dAngle = pi;
		eCnr = CNR_NORTH_EAST;
		bMoveX = true;
		bMoveY = true;
		break;
	case( SOUTH ):
		dAngle = 0.00;
		eCnr = CNR_SOUTH_WEST;
		break;
	case( EAST ):
		dAngle = pi/2.00;
		eCnr = CNR_SOUTH_EAST;
		bMoveX = true;
		break;
	case( WEST ):
		dAngle = pi/-2.00;
		eCnr = CNR_NORTH_WEST;
		bMoveY = true;
		break;
	default:
		//invalid side!
		assert( false );
		return BRACING_ERROR_ENUM;
	}
	Vector.set( 0.00, 0.00, 1.00 );
	Spin.setToRotation( dAngle, Vector );
	Transform = Spin * Transform;

	//We need to translate as part of this movement
	Vector.set( 0.00, 0.00, 0.00 );
	if( bMoveX ) Vector.x+= GetBayLengthActual();
	if( bMoveY ) Vector.y+= GetBayWidthActual();
	Aux.setTranslation( Vector );
	Transform = Aux * Transform;

	////////////////////////////////////////////////////////////////
	//delete any existing bracing
	DeleteBracing( eSide );

	////////////////////////////////////////////////////////////////
	//Every second Bracing needs to be rotated 180deg around its
	//	central z position
	if( bZigZag )
		ZigZagVector.set( 0.00, 0.00, 1.00 );

	////////////////////////////////////////////
	//For Buttress bays the top must always be higher
	//	on the south
	int iMatch = 0;
	if( GetBayType()==BAY_TYPE_BUTTRESS && eSide==EAST )
	{
		iMatch = 1;
	}

	////////////////////////////////////////////
	//Add the bracing
	for ( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_BRACING, eSide, i, dLength, eMaterial );

		/////////////////////////////////////////////////
		//Vertical location from array
		Vector.set( 0.00, 0.00, daArrangement.GetAt(i) );
		Aux.setToTranslation( Vector );
		Trans = Aux * Transform;

		//rotate every second Brace 180deg around Z axis
		if( bZigZag && ((i%2)==iMatch) )
		{
			/////////////////////////////////////////////////
			//The centroid is the centre of mass for the component
			Centroid.set(	pComponent->GetLengthActual()/2.00, 0.00, 0.00 );
			//The centroid needs to be moved too!
			Centroid.transformBy( Transform );

			/////////////////////////////////////////////////
			//rotate the component around its centre of
			//	gravity, 180deg about the Z axis
			ZigZagTrans.setToRotation( pi, ZigZagVector, Centroid );
			Trans = ZigZagTrans * Trans;
		}
		pComponent->Move( Trans, true );

		/////////////////////////////////////////////////
		//does this require movement upon scaling?
		pComponent->SetMoveWhenBayLengthChanges( bMoveX );
		pComponent->SetMoveWhenBayWidthChanges( bMoveY );
	}

	return BRACING_OK;
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteBracing
//Deletes all bracing from a bay
bool Bay::DeleteBracing( SideOfBayEnum eSide /*=NORTH*/ )
{
	int iComponentID;
	bool bFound;

	bFound = false;

	switch( eSide )
	{
		case( NORTH_NORTH_EAST ):	//fallthrough
		case( EAST_NORTH_EAST ):	//fallthrough
		case( EAST_SOUTH_EAST ):	//fallthrough
		case( SOUTH_SOUTH_EAST ):	//fallthrough
		case( SOUTH_SOUTH_WEST ):	//fallthrough
		case( WEST_SOUTH_WEST ):	//fallthrough
		case( WEST_NORTH_WEST ):	//fallthrough
		case( NORTH_NORTH_WEST ):	//fallthrough
		case( NORTH_EAST ):			//fallthrough
		case( SOUTH_EAST ):			//fallthrough
		case( NORTH_WEST ):			//fallthrough
		case( SOUTH_WEST ):			//fallthrough
		default:
			break;
		case ( ALL_SIDES ):
			//use recusion to Delete the bracing from all sides
			bFound = ( DeleteBracing( NORTH )	|| bFound );
			bFound = ( DeleteBracing( EAST )	|| bFound );
			bFound = ( DeleteBracing( SOUTH )	|| bFound );
			bFound = ( DeleteBracing( WEST )	|| bFound );
			break;

		case ( ALL_VISIBLE_SIDES ):
			//use recusion to Delete the bracing from all visible sides
			if( GetOuter()==NULL )		bFound = ( DeleteBracing( NORTH )	|| bFound );
			if( GetForward()==NULL )	bFound = ( DeleteBracing( EAST )	|| bFound );
			if( GetInner()==NULL )		bFound = ( DeleteBracing( SOUTH )	|| bFound );
			if( GetBackward()==NULL )	bFound = ( DeleteBracing( WEST )	|| bFound );
			break;

		case( NORTH ):	//fallthrough
		case( EAST ):	//fallthrough
		case( SOUTH ):	//fallthrough
		case( WEST ):
			//go through each of the componenents, the Size of the Array will change, so
			//	we need to call GetSize here, and not use a local variable 'iSize'
			for( iComponentID=0; iComponentID<GetNumberOfBayComponents(); iComponentID++ )
			{
				if( m_caComponents.GetType( iComponentID )==CT_BRACING )
				{
					//This is a standard, is it the correct pole?
					if( GetPosition( iComponentID )==eSide )
					{
						//we have a found a matching component
						m_caComponents.DeleteComponent(iComponentID);

						iComponentID--;

						bFound = true;
					}
				}
			}
			break;
	}

	return bFound;
}


/////////////////////////////////////////////////////////////////////////////////
//Movement
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//MoveBay
//Moves the Bay
void Bay::Move( Matrix3D Transform, bool bStoreMovement/*=true*/ )
{
	int			i;
	Component	*pComponent;
	Lift		*pLift;

	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComponent = m_caComponents.GetComponent( i );
		pComponent->Move( Transform, false );
	}

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = m_LiftList.GetLift( i );
		pLift->Move( Transform, false );
	}

	//store it
	if( bStoreMovement )
	{
		SetTransform ( Transform * GetTransform() );
	}
}


/////////////////////////////////////////////////////////////////////////////////
//MoveRun
//calls the run object to move each Bay in the Run to a certain offset
void Bay::MoveRun( Matrix3D Transform )
{
	GetRunPointer()->Move( Transform, true );
	GetRunPointer()->MoveSchematic( Transform, true );
}


/////////////////////////////////////////////////////////////////////////////////
//Rules
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//CheckLapboards
//Ensure that any Lapboards are at correct height etc.
//Bay::CheckLapboards
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CheckRulesForThisBay
//Ensure that this bay is Legit
//Bay::CheckRulesForThisBay
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CheckRulesForLifts
//Ensure all the lifts are behaving themselves
//Bay::CheckRulesForLifts
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CheckRulesRelatingToNextBay
//Ensure the Next Bay is Congruent 
//Bay::CheckRulesRelatingToNextBay
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CheckRulesRelatingToPreviousBay
//Ensure the previous Bay is Congruent
//Bay::CheckRulesRelatingToPreviousBay
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CheckRulesRelatingToRunBay
//Ensure that we are not way off track for the entire Run
//Bay::CheckRulesRelatingToRunBay
//{
//}


/////////////////////////////////////////////////////////////////////////////////
//Split
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//SplitRunAtThisBay
//This function is used to tell the Run to split this Bay & subsequent Bays from
//	'pPrevious' Bay to become a new run.  Firstly needs to check if pPrevious is
//	NULL
bool Bay::SplitRunAtThisBay()
{
	Run *pRun;
	pRun = GetRunPointer()->SplitRun( m_iBayID );
	return pRun!=NULL;
}
	

/////////////////////////////////////////////////////////////////////////////////
//Move/Copy/Delete
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//DeleteBay
//Destroys all lifts in Bay.  Checks Matrix.  This function would typically be
//	called from within the Run!  Also notifies the Matrix that the Bay has been
//	removed.
bool Bay::DeleteBay()
{
	LapboardBay	*pLap;

	GetController()->GetBayStageLevelArray()->BayDeleted(this);

	////////////////////////////////////////////////////////////////////
	while( GetNumberOfLapboardPointers()>0 )
	{
		pLap = m_pLapboards->GetAt(0);
		GetController()->DeleteLapboard( pLap->GetID() );
	}

	//Tell the next bay to adjust its forward and backward pointers
	//	and to add standards, etc if required!
	ClearAllBayPointers( false );

	///////////////////////////////////////////////////
	//Delete the lifts and Bay components
	DeleteAllComponents();

	//is the second set of standard represented in the matrix?
	MatrixRemoveLink( false );
	if( GetBackward()==NULL )
	{
		//Yep, delete that matrix too if not used any more
		MatrixRemoveLink( true );
	}

	if( !GetController()->IsDestructingController() )
		GetTemplate()->DeleteSchematic();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//RemoveBay
//notifies the Run, via a message, that this Bay needs to be deleted.
void Bay::RemoveBay()
{
	SetDirtyFlag( DF_DELETE );
}

/////////////////////////////////////////////////////////////////////////////////
//CopyBay
//Copies the contents of one bay to another new bay!  Set the ID's for the new Bay.
//	Will also be required to check pointers of the new bay to ensure that we don't
//	need an extra set of standards, transom etc.  We may even need to offset the
//	new bay to separate it from the first bay.
Bay *Bay::CopyBay( Matrix3D TransformationMatrix, bool bMove/*=true*/) const
{
	Bay *pNewBay;

	pNewBay = NULL;

	//memcpy this bay to create a new bay
	pNewBay = new Bay();

	//assert( FALSE ); //??? JSB todo 990719 - This function has to be completed using a memcpy
	*pNewBay = *this;

	if( bMove )
	{
		pNewBay->Move( TransformationMatrix, true );
		pNewBay->MoveSchematic( TransformationMatrix, true );
	}

	return pNewBay;
}

////////////////////////////////////////////////////////////////////////////////////////
//CopyBay
Bay & Bay::CopyBay(const Bay &OriginalBay, BayList *pBays/*=NULL*/)
{
	Bay			*pForwardBay, *pBackwardBay, *pInnerBay, *pOuterBay;
	bool		bSoleBoards[4];
	Lift		*pLift, *pOriginLift;
	Component	*pComp;

	//////////////////////////////////////////////////
	//store the pointers
	pForwardBay		= GetForward();
	pBackwardBay	= GetBackward();
	pInnerBay		= GetInner();
	pOuterBay		= GetOuter();

	//Delete the existing stuff
	DeleteBay();
	/////////////////////////////////////////////////////////////////////////////////
	//m_iBayID		= ID_INVALID;
	//SetRunPointer( OriginalBay.GetRunPointer( ) );

	//copy the system across
	SetSystem( OriginalBay.GetSystem() );

	/////////////////////////////////////////////////////////////////////////////////
	//Pointers to Neighboring bays
	//SetBackward( NULL );
	//SetForward( NULL );
	//SetOuter( NULL );
	//SetInner( NULL );
	//SetCornerEast( NULL );
	//SetCornerWest( NULL );
	
	SetUseMidrailWithChainMesh( OriginalBay.GetUseMidrailWithChainMesh() );
	SetUseLedgerEveryMetre( OriginalBay.GetUseLedgerEveryMetre() );
	SetHasMovingDeck( OriginalBay.HasMovingDeck() );
	SetRoofProtection( OriginalBay.GetRoofProtection() );

	/////////////////////////////////////////////////////////////////////////////////
	//Matrix
	m_pMatrixElement	= OriginalBay.m_pMatrixElement;
	m_pMatrixElementAlt	= OriginalBay.m_pMatrixElementAlt;


	/////////////////////////////////////////////////////////////////////////////////
	//Dialog Box stuff
	m_bStaggeredHeights	= OriginalBay.m_bStaggeredHeights;
	m_eStandardsFit		= OriginalBay.m_eStandardsFit;
	SetBayLength( OriginalBay.GetBayLength() );
	SetBayWidth( OriginalBay.GetBayWidth() );

	/////////////////////////////////////////////////////////////////////////////////
	//SOLEBOARDS: Where do we have soleboards?
	Bay				*pBay;
	SideOfBayEnum	eBaySide, eSde;
	CornerOfBayEnum	eCnr;
	for( int iCnr=CNR_NORTH_EAST; iCnr<=CNR_SOUTH_WEST; iCnr++ )
	{
		eCnr = (CornerOfBayEnum)iCnr;
		eSde = CornerAsSideOfBay(eCnr);

		pBay = OriginalBay.GetBayThatOwnsSide( eSde, eBaySide );
		if( pBay==NULL )
			bSoleBoards[eCnr] = OriginalBay.HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSde );
		else
			bSoleBoards[eCnr] = pBay->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eBaySide );
	}

	/////////////////////////////////////////////////////////////////////////////////
	//LIFTS: Lift elements copy
	int iNewLiftID;
	for( int iLift=0; iLift<OriginalBay.m_LiftList.GetSize(); iLift++ )
	{
		pOriginLift = OriginalBay.GetLift( iLift );
		pLift = new Lift();
		iNewLiftID = AddLift( pLift );
		assert( iLift==iNewLiftID );
		*pLift = *pOriginLift;
	}

	//We will need to set the RL's
	double dRL = OriginalBay.GetRLOfLift(0);
	SetRLsForAllLifts( dRL );

	////////////////////////////////////////////////////////////////////////////
	//Pointers to other bays
	if( pForwardBay!=NULL )
	{
		SetForward( pForwardBay, true, false );
		pForwardBay->SetBackward( this, true, true );
	}
	if( pBackwardBay!=NULL )
	{
		SetBackward( pBackwardBay, true, false );
		pBackwardBay->SetForward( this, true, true );
	}
	if( pInnerBay!=NULL )
	{
		SetInner( pInnerBay, true, false );
		pInnerBay->SetOuter( this, true, true );
	}
	if( pOuterBay!=NULL )
	{
		SetOuter( pOuterBay, true, false );
		pOuterBay->SetInner( this, true, true );
	}

	/////////////////////////////////////////////////////////////////////////////////
	//STANDARDS - copy from the original
	doubleArray	daArrangement;

	//NE
	for( int iCorner=CNR_NORTH_EAST; iCorner<=(int)CNR_SOUTH_WEST; iCorner++ )
	{
		OriginalBay.GetStandardsArrangement( daArrangement, (CornerOfBayEnum)iCorner );
		SetStandardPosition( OriginalBay.GetStandardPosition((CornerOfBayEnum)iCorner), (CornerOfBayEnum)iCorner );
		AdjustStandardSmart( (CornerOfBayEnum)iCorner, daArrangement, bSoleBoards[iCorner], pBays );
	}

	/////////////////////////////////////////////////////////////////////////////////
	//We need to check that the standards are tall enough, since
	//	we may have added a handrail to this bay during the setforward
	//	/setbackward functions above, and this handrail would not have
	//	been in the original bay
	int	iSide;
	for( iSide=NORTH_EAST; iSide<=(int)SOUTH_WEST; iSide++ )
	{
		EnsureStandardsAreTallEnough((SideOfBayEnum)iSide);
	}

	/////////////////////////////////////////////////////////////////////////////////
	//BRACING: If there is bracing on the side to be copied, then we need to apply the bracing
	//	to the same side of the bay
	if( GetOuter()==NULL )
		CopyBracingSmart( NORTH, &OriginalBay );

	if( GetForward()==NULL )
		CopyBracingSmart( EAST, &OriginalBay );

	if( GetInner()==NULL )
		CopyBracingSmart( SOUTH, &OriginalBay );

	if( GetBackward()==NULL )
		CopyBracingSmart( WEST, &OriginalBay );

	/////////////////////////////////////////////////////////////////////////////////
	for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
	{
		//Chain link
		if( OriginalBay.DoesABayComponentExistOnASide( CT_CHAIN_LINK, (SideOfBayEnum)iSide ) )
			AddAChainLink( (SideOfBayEnum)iSide );
		else
			DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, (SideOfBayEnum)iSide );

		//Shade cloth
		if( OriginalBay.DoesABayComponentExistOnASide( CT_SHADE_CLOTH, (SideOfBayEnum)iSide ) )
			AddAShadeCloth( (SideOfBayEnum)iSide );
		else
			DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, (SideOfBayEnum)iSide );
	}
		
	/////////////////////////////////////////////////////////////////////////////////
	//TEXT: Create a new text object
	pComp = CreateComponent( CT_TEXT, SOUTH, 0, 0.00, MT_STEEL );

	*(GetTemplate()) = *OriginalBay.GetTemplate( );
//	CreateSchematic();
	GetTemplate()->SetBayPointer( this );

/*
	Run::CopyRun is the only function that also copies the Transform
	SetTransform( OriginalBay.GetTransform() );
	SetSchematicTransform( OriginalBay.GetSchematicTransform() );
*/

/*	double dStdRL[4];
	dStdRL[CNR_NORTH_EAST] = OriginalBay.GetBottomRLofStandard( CNR_NORTH_EAST );
	dStdRL[CNR_SOUTH_EAST] = OriginalBay.GetBottomRLofStandard( CNR_SOUTH_EAST );
	dStdRL[CNR_SOUTH_WEST] = OriginalBay.GetBottomRLofStandard( CNR_SOUTH_WEST );
	dStdRL[CNR_NORTH_WEST] = OriginalBay.GetBottomRLofStandard( CNR_NORTH_WEST );
	SetAndAdjustRLs( dStdRL );
*/
	SetDirtyFlag( DF_CLEAN );

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////
//operator=
//
Bay & Bay::operator=(const Bay &OriginalBay)
{
	return CopyBay( OriginalBay );
}

/////////////////////////////////////////////////////////////////////////////////
//operator==
//
bool Bay::operator ==(const Bay &OtherBay) const
{
	/////////////////////////////////////////////////////////////////////////////////
	//if( m_iBayID		!= OtherBay.GetID() )			return false;
	if( GetRunPointer()	!= OtherBay.GetRunPointer() )	return false;
	assert( FALSE );	//JSB todo 990727 - finish the following two functions
//	if( m_LiftList		!= OtherBay.m_LiftList )		return false;
//	if( m_caComponents	!= OtherBay.m_caComponents )	return false;

	/////////////////////////////////////////////////////////////////////////////////
	//Pointers to Neighboring bays
//	if( GetBackward()	!= OtherBay.GetBackward() )		return false;
//	if( GetForward()	!= OtherBay.GetForward() )		return false;
//	if( GetOuter()		!= OtherBay.GetOuter() )		return false;
//	if( GetInner()		!= OtherBay.GetInner() )		return false;
//	if( GetCornerEast()	!= OtherBay.GetCornerEast() )	return false;
//	if( GetCornerWest()	!= OtherBay.GetCornerWest() )	return false;

	/////////////////////////////////////////////////////////////////////////////////
	//Matrix
	if( m_pMatrixElement	!= OtherBay.m_pMatrixElement )		return false;
	if( m_pMatrixElementAlt	!= OtherBay.m_pMatrixElementAlt )	return false;

	/////////////////////////////////////////////////////////////////////////////////
	//Standards
	if( m_daHeights[CNR_NORTH_EAST] != OtherBay.m_daHeights[CNR_NORTH_EAST] )		return false;
	if( m_daHeights[CNR_SOUTH_EAST] != OtherBay.m_daHeights[CNR_SOUTH_EAST] )		return false;
	if( m_daHeights[CNR_NORTH_WEST] != OtherBay.m_daHeights[CNR_NORTH_WEST] )		return false;
	if( m_daHeights[CNR_SOUTH_WEST] != OtherBay.m_daHeights[CNR_SOUTH_WEST] )		return false;

	if( GetStandardPosition(CNR_NORTH_EAST) != OtherBay.GetStandardPosition(CNR_NORTH_EAST) )		return false;
	if( GetStandardPosition(CNR_SOUTH_EAST) != OtherBay.GetStandardPosition(CNR_SOUTH_EAST) )		return false;
	if( GetStandardPosition(CNR_NORTH_WEST) != OtherBay.GetStandardPosition(CNR_NORTH_WEST) )		return false;
	if( GetStandardPosition(CNR_SOUTH_WEST) != OtherBay.GetStandardPosition(CNR_SOUTH_WEST) )		return false;

	/////////////////////////////////////////////////////////////////////////////////
	//Dialog Box stuff
	if( m_bStaggeredHeights	!= OtherBay.m_bStaggeredHeights )	return false;
	if( m_eStandardsFit		!= OtherBay.m_eStandardsFit )		return false;
	if( GetBayLength()		!= OtherBay.GetBayLength() )		return false;
	if( GetBayWidth()		!= OtherBay.GetBayWidth() )			return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//operator!=
//
bool Bay::operator !=(const Bay &OtherBay) const
{
	return !operator==( OtherBay );
}

/////////////////////////////////////////////////////////////////////////////////
//BOM
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//WriteBOMTo
//This is used to write info for the BOM, it calls lift.WriteBOMTo, and also for
//	the components.WriteBOMTo 
void Bay::WriteBOMTo( CStdioFile *pFile)
{
	int			i;
	Component	*pComponent;
	Lift		*pLift;
	CString	sLine;

//	sLine.Format("  **Bay%i**\n", GetID() );
//	pFile->WriteString( sLine );
	
	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComponent = m_caComponents.GetComponent( i );
		pComponent->WriteBOMTo( pFile );
	}

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = m_LiftList.GetLift(i);
		pLift->WriteBOMTo( pFile );
	}
}



/////////////////////////////////////////////////////////////////////////////////
//Matrix
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//SetMatrixElementPointer
//Sets the pointer to the MatrixElement representation, may also need to set the
//	Alternate Matrix Pointer
bool Bay::SetMatrixElementPointer( MatrixElement *pMatrixElement, bool bSetAlt/*=false*/ )
{
	if( bSetAlt )
	{
		assert( GetBackward()==NULL );	//This should be null
		m_pMatrixElementAlt	= pMatrixElement;
	}
	else
	{
		m_pMatrixElement	= pMatrixElement;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetMatrixElementPointer
//Retrives the pointer to the Matrix Element.
//	Matrix Pointer
MatrixElement *Bay::GetMatrixElementPointer() const
{
	return m_pMatrixElement;
}

/////////////////////////////////////////////////////////////////////////////////
//GetMatrixElementPointerAlt
//Retrives the pointer to the Matrix ElementAlt
//	Matrix Pointer
MatrixElement *Bay::GetMatrixElementPointerAlt() const
{
	return m_pMatrixElementAlt;
}

/////////////////////////////////////////////////////////////////////////////////
//CheckMatrixPointer
//This is really an internal (private) function.  This function does several
//	things:
//		Checks matrix pointer is valid
//		Ensure Matrix Element is representative of the Bay
//		Checks that the MatrixElement points back to this Bay
//		[optional] - ensure that the Matrix Element is in the Matrix
bool Bay::CheckMatrixPointer( bool bUseAlt )
{
	MatrixElement	*pElement;
	Matrix			*pMatrix;

	pElement = m_pMatrixElement;
	if( bUseAlt )
		pElement = m_pMatrixElementAlt;

	//Check the matrix pointer is valid
	if( pElement==NULL )
		return false;

	//Ensure Matrix Element is representative of the Bay
	if( !CheckExistsInMatrix( bUseAlt ) )
		return false;

	//Checks that the MatrixElement points back to this Bay
	if( !pElement->CheckBayExist( m_iBayID ) ) //Alternatively pElement->CheckBayExists( this );
		return false;

	//[optional] - ensure that the Matrix Element is part of the Matrix
	pMatrix=GetController()->GetMatrix();
	if( (pMatrix!=NULL) && (pMatrix->GetMatrixElementID(pElement)>=0) )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////
//FindMatrix
//Asks the Controller class via the Run class to search for an existing matrix
//	that is compatible with this Bay, if it is found it returns the matrixID,
//	otherwise it returns some -ve enum value.  Need to also consider the
//	pMatrixPointerAlt case.
int Bay::FindMatrix( bool bUseAlt )
{
	int				i, iSize;
	bool			bFound;
	Matrix			*pMatrix;
	MatrixElement	*pMatrixElement;
	double			dStandardHeight, dMatrixHeight;
	CornerOfBayEnum	eCnrInner, eCnrOuter;

	eCnrInner = ( bUseAlt )? CNR_SOUTH_WEST: CNR_SOUTH_EAST;
	eCnrOuter = ( bUseAlt )? CNR_NORTH_WEST: CNR_NORTH_EAST;

	pMatrix=GetController()->GetMatrix();
	if( pMatrix==NULL )
		return MATRIX_NO_MATRIX_OBJECT;

	iSize = pMatrix->GetMatrixSize();
	if( iSize<=0 )
		return MATRIX_EMPTY_MATRIX;

	//Shortcut - check the existing pointer first to see if that is valid
	if( CheckMatrixPointer( bUseAlt ) )
	{
		pMatrixElement = m_pMatrixElement;
		if( bUseAlt )
			pMatrixElement = m_pMatrixElementAlt;

		return pMatrix->GetMatrixElementID(pMatrixElement);
	}

	//The existing point doesn't work, so we need to go through the matrix
	//	to find a match
	bFound = false;

	#ifdef USE_MATRIX_WIDTH
	double	dMatrixWidth, dBayWidth;
	#endif	//#ifdef USE_MATRIX_WIDTH

	for( i=0; i<iSize; i++ )
	{
		pMatrixElement = pMatrix->GetMatrixElement( i );

		if( (int)pMatrixElement->GetSystem()!=(int)GetSystem() )
			continue;

		#ifdef USE_MATRIX_WIDTH
		dMatrixWidth	= pMatrixElement->GetMatrixBayWidth();
		dBayWidth		= GetBayWidth();
		if( dMatrixWidth<dBayWidth-ROUND_ERROR ||
			dMatrixWidth>dBayWidth+ROUND_ERROR )
			continue;
		#endif	//#ifdef USE_MATRIX_WIDTH

		///////////////////////////////////////////////////////////////////
		//Test the inner standard heights
		dStandardHeight	= GetHeightOfStandards(eCnrInner);
		dMatrixHeight = 0.00;
		dMatrixHeight = pMatrixElement->GetHeight( MATRIX_STANDARDS_INNER );
		if( dMatrixHeight<dStandardHeight-ROUND_ERROR )
		{
			//The matrix is sorted by descending order of height of inner standard, so we don't
			//	have a matrix of this height
			return MATRIX_NO_MATCH;
		}
		if( dMatrixHeight>dStandardHeight+ROUND_ERROR ||
			dMatrixHeight<dStandardHeight-ROUND_ERROR )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the outer standard heights
		dStandardHeight	= GetHeightOfStandards(eCnrOuter);
		dMatrixHeight = pMatrixElement->GetHeight( MATRIX_STANDARDS_OUTER );
		if( dMatrixHeight>dStandardHeight+ROUND_ERROR ||
			dMatrixHeight<dStandardHeight-ROUND_ERROR )
			continue;
		
		///////////////////////////////////////////////////////////////////
		//Test the inner standards arrangement
		if( !TestStandardsArrangment( pMatrixElement, true, bUseAlt ) )
			continue;

		//Test the outer standards arrangement
		if( !TestStandardsArrangment( pMatrixElement, false, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the transoms heights match
		if( !TestTransomHeights( pMatrixElement, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the decks
		if( !TestDeckingHeights( pMatrixElement, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the Inner(South) & Outer(North) Stage boards
		if( !TestStageBoardHeights( pMatrixElement, true, bUseAlt ) )
			continue;
		if( !TestStageBoardHeights( pMatrixElement, false, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the Inner(South) & Outer(North) HopupRail boards
		if( !TestHopupRailBoardHeights( pMatrixElement, true, bUseAlt ) )
			continue;
		if( !TestHopupRailBoardHeights( pMatrixElement, false, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the end hand rails
		if( !TestEndHandRailHeights( pMatrixElement, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the end hand rails
		if( !TestEndMidRailHeights( pMatrixElement, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the hand rails for North and South
		if( !TestHandRailHeights( pMatrixElement, true, bUseAlt ) )
			continue;
		if( !TestHandRailHeights( pMatrixElement, false, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the ledger for North and South
		if( !TestLedgerHeights( pMatrixElement, true, bUseAlt ) )
			continue;
		if( !TestLedgerHeights( pMatrixElement, false, bUseAlt ) )
			continue;

		///////////////////////////////////////////////////////////////////
		//Test the hand rails for North and South
		if( !TestMidRailHeights( pMatrixElement, true, bUseAlt ) )
			continue;
		if( !TestMidRailHeights( pMatrixElement, false, bUseAlt ) )
			continue;

		//We found a match, why wasn't it in the Baylist?
		bFound = true;
		pMatrixElement->AddBayLink( GetBayNumber() );
		SetMatrixElementPointer( pMatrixElement, bUseAlt );
		break;
	}

	if( bFound )
		return i;
	else
		return MATRIX_NO_MATCH;
}

/////////////////////////////////////////////////////////////////////////////////
//CheckExistsInMatrix
//May be the same as above.  Need to also consider the pMatrixPointerAlt case.
bool Bay::CheckExistsInMatrix( bool bUseAlt )
{
	MatrixElement	*pMatrixElement;

	pMatrixElement = m_pMatrixElement;
	if( bUseAlt )
		pMatrixElement = m_pMatrixElementAlt;

	if( (int)pMatrixElement->GetSystem()!=(int)GetSystem() )
		return false;

	#ifdef USE_MATRIX_WIDTH
	double dMatrixWidth, dBayWidth;
	dMatrixWidth	= pMatrixElement->GetMatrixBayWidth();
	dBayWidth		= GetBayWidth();
	if( dMatrixWidth<dBayWidth-ROUND_ERROR ||
		dMatrixWidth>dBayWidth+ROUND_ERROR )
		return false;
	#endif	//#ifdef USE_MATRIX_WIDTH

	///////////////////////////////////////////////////////////////////
	//Test the inner standard heights
	double dStandardHeight, dMatrixHeight;
	CornerOfBayEnum	eCnrInner, eCnrOuter;

	eCnrInner = ( bUseAlt )? CNR_SOUTH_WEST: CNR_SOUTH_EAST;
	eCnrOuter = ( bUseAlt )? CNR_NORTH_WEST: CNR_NORTH_EAST;

	dStandardHeight	= GetHeightOfStandards(eCnrInner);
	dMatrixHeight = 0.00;
	dMatrixHeight = pMatrixElement->GetHeight( MATRIX_STANDARDS_INNER );
	if( dMatrixHeight<dStandardHeight )
	{
		//The matrix is sorted by descending order of height of inner standard, so we don't
		//	have a matrix of this height
		return false;
	}
	if( dMatrixHeight!=dStandardHeight )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the outer standard heights
	dStandardHeight	= GetHeightOfStandards(eCnrOuter);
	dMatrixHeight = pMatrixElement->GetHeight( MATRIX_STANDARDS_OUTER );
	if( dMatrixHeight!=dStandardHeight )
		return false;
	
	///////////////////////////////////////////////////////////////////
	//Test the inner standards arrangement
	if( !TestStandardsArrangment( pMatrixElement, true, bUseAlt ) )
		return false;

	//Test the outer standards arrangement
	if( !TestStandardsArrangment( pMatrixElement, false, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the transoms heights match
	if( !TestTransomHeights( pMatrixElement, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the decks
	if( !TestDeckingHeights( pMatrixElement, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the Inner(South) & Outer(North) Stage boards
	if( !TestStageBoardHeights( pMatrixElement, true, bUseAlt ) )
		return false;
	if( !TestStageBoardHeights( pMatrixElement, false, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the Inner(South) & Outer(North) HopupRail boards
	if( !TestHopupRailBoardHeights( pMatrixElement, true, bUseAlt ) )
		return false;
	if( !TestHopupRailBoardHeights( pMatrixElement, false, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the end hand rails
	if( !TestEndHandRailHeights( pMatrixElement, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the end hand rails
	if( !TestEndMidRailHeights( pMatrixElement, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the hand rails for North and South
	if( !TestHandRailHeights( pMatrixElement, true, bUseAlt ) )
		return false;
	if( !TestHandRailHeights( pMatrixElement, false, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the ledger for North and South
	if( !TestLedgerHeights( pMatrixElement, true, bUseAlt ) )
		return false;
	if( !TestLedgerHeights( pMatrixElement, false, bUseAlt ) )
		return false;

	///////////////////////////////////////////////////////////////////
	//Test the hand rails for North and South
	if( !TestMidRailHeights( pMatrixElement, true, bUseAlt ) )
		return false;
	if( !TestMidRailHeights( pMatrixElement, false, bUseAlt ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//EnsureMatrix
//Checks the matrix to see if this Bay is represented.  Sets the appropriate
//	values and pointers if it is
//I don't think we need this function anymore, since I am sure the above two
//	functions can handle this.  Need to also consider the pMatrixPointerAlt case.
//bool Bay::EnsureMatrix()
//{
//}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrix
//Tells the Controller class, via the Run, to create a new MatrixElement,
//	should probably double check that the matrix element doesn't exist first!.
//	Need to also consider the pMatrixPointerAlt case.
bool Bay::CreateMatrix( bool bUseAlt )
{
	bool				bSoleBoard;
	MatrixElement		*pElement;
	doubleArray			daArrangement;
	intArray			iaPlanks;

	daArrangement.SetUniqueElementsOnly();

	if( FindMatrix( bUseAlt )>=0 )
	{
		//we already have one!
		return false;
	}

	//we don't have a matching element
	pElement = GetController()->CreateMatrixElement();
	pElement->SetSystem( GetSystem() );

	if( bUseAlt )
		m_pMatrixElementAlt	= pElement;
	else
		m_pMatrixElement	= pElement;

	//Setup the bay link!
	pElement->AddBayLink ( this );
	SetMatrixElementPointer( pElement, bUseAlt );

	///////////////////////////////////////////////////////////////////////////////
	//Bay Width
	pElement->SetMatrixBayWidth( GetBayWidth() );
	pElement->SetMatrixBayWidthActual( GetBayWidthActual() );

	///////////////////////////////////////////////////////////////////////////////
	//Standards
	//NORTHERN
	bSoleBoard = false;
	daArrangement.RemoveAll();
	if( CreateMatrixStandardArrangment( daArrangement, bSoleBoard, false, bUseAlt ) )
		pElement->SetStandardsArrangement( daArrangement, bSoleBoard, MATRIX_STANDARDS_OUTER );
	//SOUTHERN
	bSoleBoard = false;
	daArrangement.RemoveAll();
	if( CreateMatrixStandardArrangment( daArrangement, bSoleBoard, true, bUseAlt ) )
		pElement->SetStandardsArrangement( daArrangement, bSoleBoard, MATRIX_STANDARDS_INNER );


	///////////////////////////////////////////////////////////////////////////////
	//Transoms
	daArrangement.RemoveAll();
	if( CreateMatrixTransomArrangment( daArrangement, bUseAlt ) )
	{
		pElement->SetTransomHeights( daArrangement );
	}

	///////////////////////////////////////////////////////////////////////////////
	//Decking
	daArrangement.RemoveAll();
	if( CreateMatrixDeckingArrangement( daArrangement, bUseAlt ) )
		pElement->SetDeckingHeights( daArrangement );


	///////////////////////////////////////////////////////////////////////////////
	//Stages
	//NORTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixStageArrangement( daArrangement, iaPlanks, false, bUseAlt ) )
		pElement->SetStageHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixStageArrangement( daArrangement, iaPlanks, true, bUseAlt ) )
		pElement->SetStageHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//HopupRails
	//NORTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, false, bUseAlt ) )
		pElement->SetHopupRailHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, true, bUseAlt ) )
		pElement->SetHopupRailHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_INNER );


	///////////////////////////////////////////////////////////////////////////////
	//End Handrails
	daArrangement.RemoveAll();
	if( CreateMatrixEndHandrailArrangement( daArrangement, bUseAlt ) )
		pElement->SetEndHandrailHeights( daArrangement );

	///////////////////////////////////////////////////////////////////////////////
	//End Midrails
	daArrangement.RemoveAll();
	if( CreateMatrixEndMidrailArrangement( daArrangement, bUseAlt ) )
		pElement->SetEndMidrailHeights( daArrangement );

	///////////////////////////////////////////////////////////////////////////////
	//Handrails
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixHandrailArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetHandrailHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixHandrailArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetHandrailHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//Ledgers
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixLedgerArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetLedgerHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixLedgerArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetLedgerHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//Midrails
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixMidrailArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetMidrailHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixMidrailArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetMidrailHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//ToeBoards
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixToeBoardArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetToeBoardHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixToeBoardArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetToeBoardHeights( daArrangement, MATRIX_STANDARDS_INNER );

	GetController()->GetMatrix()->SelectionSort(false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//CreateCutThrough
//This will create a cutthrough section and add it to the cut through matrix
//	it will also call the next bay north of it and ask it to do the same
void Bay::CreateCutThrough( Matrix *pCutThrough, SideOfBayEnum eSide)
{
	//The Western side stuff is alway owned by the bay to the west
	if( eSide==WEST && GetBackward()!=NULL )
	{
		GetBackward()->CreateCutThrough( pCutThrough, EAST );
		return;
	}

	bool				bSoleBoard, bUseAlt;
	MatrixElement		*pElement;
	doubleArray			daArrangement;
	intArray			iaPlanks;

	bUseAlt = false;
	if( eSide==WEST )
		bUseAlt = true;

	daArrangement.SetUniqueElementsOnly();

	//Each section needs a new element;
	pElement = GetController()->CreateMatrixElement(pCutThrough);
	pElement->SetSystem( GetSystem() );

	//Note: we don't want to touch the matrix pointers for this bay

	///////////////////////////////////////////////////////////////////////////////
	//Bay Width
	pElement->SetMatrixBayWidth( GetBayWidth() );
	pElement->SetMatrixBayWidthActual( GetBayWidthActual() );

	///////////////////////////////////////////////////////////////////////////////
	//Standards
	//NORTHERN
	bSoleBoard = false;
	daArrangement.RemoveAll();
	if( CreateMatrixStandardArrangment( daArrangement, bSoleBoard, false, bUseAlt, false ) )
		pElement->SetStandardsArrangement( daArrangement, bSoleBoard, MATRIX_STANDARDS_OUTER );
	//SOUTHERN
	daArrangement.RemoveAll();
	if( CreateMatrixStandardArrangment( daArrangement, bSoleBoard, true, bUseAlt, false ) )
		pElement->SetStandardsArrangement( daArrangement, bSoleBoard, MATRIX_STANDARDS_INNER );


	///////////////////////////////////////////////////////////////////////////////
	//Transoms
	daArrangement.RemoveAll();
	if( CreateMatrixTransomArrangment( daArrangement, bUseAlt ) )
	{
		pElement->SetTransomHeights( daArrangement );
	}

	///////////////////////////////////////////////////////////////////////////////
	//Decking
	daArrangement.RemoveAll();
	if( CreateMatrixDeckingArrangement( daArrangement, bUseAlt ) )
		pElement->SetDeckingHeights( daArrangement );


	///////////////////////////////////////////////////////////////////////////////
	//Stages
	//NORTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixStageArrangement( daArrangement, iaPlanks, false, bUseAlt ) )
		pElement->SetStageHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixStageArrangement( daArrangement, iaPlanks, true, bUseAlt ) )
		pElement->SetStageHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//HopupRails
	//NORTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, false, bUseAlt ) )
		pElement->SetHopupRailHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	iaPlanks.RemoveAll();
	if( CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, true, bUseAlt ) )
		pElement->SetHopupRailHeights( daArrangement, iaPlanks, MATRIX_STANDARDS_INNER );


	///////////////////////////////////////////////////////////////////////////////
	//End Handrails
	daArrangement.RemoveAll();
	if( CreateMatrixEndHandrailArrangement( daArrangement, bUseAlt ) )
		pElement->SetEndHandrailHeights( daArrangement );

	///////////////////////////////////////////////////////////////////////////////
	//End Midrails
	daArrangement.RemoveAll();
	if( CreateMatrixEndMidrailArrangement( daArrangement, bUseAlt ) )
		pElement->SetEndMidrailHeights( daArrangement );

	///////////////////////////////////////////////////////////////////////////////
	//Handrails
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixHandrailArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetHandrailHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixHandrailArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetHandrailHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//Ledgers
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixLedgerArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetLedgerHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixLedgerArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetLedgerHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//Midrails
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixMidrailArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetMidrailHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixMidrailArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetMidrailHeights( daArrangement, MATRIX_STANDARDS_INNER );

	///////////////////////////////////////////////////////////////////////////////
	//ToeBoards
	//NORTH;
	daArrangement.RemoveAll();
	if( CreateMatrixToeBoardArrangement( daArrangement, false, bUseAlt ) )
		pElement->SetToeBoardHeights( daArrangement, MATRIX_STANDARDS_OUTER );
	//SOUTH;
	daArrangement.RemoveAll();
	if( CreateMatrixToeBoardArrangement( daArrangement, true, bUseAlt ) )
		pElement->SetToeBoardHeights( daArrangement, MATRIX_STANDARDS_INNER );

	//We have now create a cut through for this section, we
	//	now need to create one for the next bay south along
	//	the cut through path
	if( GetInner()!=NULL )
	{
		GetInner()->CreateCutThrough( pCutThrough, eSide );
	}
	else
	{
		if( eSide==EAST && GetForward()!=NULL && 
				GetForward()->GetInner()!=NULL )
		{
			//This is designed to handle this situation
			//		---------- ----------
			//     |          |          |
			//     |   this --|----      |
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
			GetForward()->GetInner()->CreateCutThrough( pCutThrough, WEST );
		}
		else if( eSide==WEST && GetBackward()!=NULL && 
					GetBackward()->GetInner()!=NULL )
		{
			//This is designed to handle this situation
			//		---------- ----------
			//     |          |          |
			//     |      ----|--this    |
			//     |     |    |          |
			//		---------- ----------
			//     |     |    |
			//     |     V    |  Void
			//     |          |
			//		---------- ----------
			//     |          |          |
			//     |          |          |
			//     |          |          |
			//		---------- ----------
			GetBackward()->GetInner()->CreateCutThrough( pCutThrough, EAST );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
//MatrixRemoveLink
//Notifies the Matrix that this MatrixElementID should remove its link back to
//	this bay.  Need to also consider the pMatrixPointerAlt case.
bool Bay::MatrixRemoveLink( bool bUseAlt )
{
	MatrixElement *pElement;
	
	pElement = ( bUseAlt )? m_pMatrixElementAlt: m_pMatrixElement;

	if( pElement!=NULL )
		return pElement->RemoveBayLink( this );

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
//Add/Remove Components General
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//AddComponent
//Adds a component to a lift.  The lift is determined by using the
//	GetLiftAtHeight function.
//	eSide would expect an argument of either SideOfBayEnum,
//	BracketCornerOfBayEnum or CornerOfBayEnum
Component *Bay::AddComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
						 double dHeight, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Component *pComponent;

	if( GetMillsSystemType()==MILLS_TYPE_CONNECT_NNE &&
		(eComponentType==CT_STANDARD || eComponentType==CT_STANDARD_OPENEND) &&
		eSide==NE )
		return NULL;

	if( GetMillsSystemType()==MILLS_TYPE_CONNECT_SSE &&
		(eComponentType==CT_STANDARD || eComponentType==CT_STANDARD_OPENEND) &&
		eSide==SE )
		return NULL;

	if( GetMillsSystemType()==MILLS_TYPE_CONNECT_SSW &&
		(eComponentType==CT_STANDARD || eComponentType==CT_STANDARD_OPENEND) &&
		eSide==SW )
		return NULL;

	if( GetMillsSystemType()==MILLS_TYPE_CONNECT_NNW &&
		(eComponentType==CT_STANDARD || eComponentType==CT_STANDARD_OPENEND) &&
		eSide==NW )
		return NULL;

	//Does this component already exist?
	if( GetComponent( eComponentType, eSide, dHeight )!=NULL ) return NULL;

	switch( eComponentType )
	{
		case( CT_STANDARD_OPENEND ):
			pComponent = AddAOpenEndStandard( eSide, dLength, eMaterialType );
			break;
		case( CT_STANDARD ):
			pComponent = AddAStandard( eSide, dLength, eMaterialType );
			break;
		case( CT_JACK ):
			pComponent = AddAJack( eSide, eMaterialType );
			break;
		case( CT_SOLEBOARD ):
			assert( eMaterialType==MT_TIMBER );
			pComponent = AddASoleboard( eSide, eMaterialType );
			break;
		case( CT_CHAIN_LINK ):
			pComponent = AddAChainLink( eSide );
			break;
		case( CT_SHADE_CLOTH ):
			pComponent = AddAShadeCloth( eSide );
			break;
		case( CT_TIE_TUBE ):
			//Note: This will add a tube, 2x90degree clamps, and a clamp
			pComponent = AddATie( eSide, dHeight, dLength, eMaterialType );
			break;
		case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
			pComponent = AddATieClampColumn( eSide, dHeight, dLength, eMaterialType );
			break;
		case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
			pComponent = AddATieClampMasonary( eSide, dHeight, dLength, eMaterialType );
			break;
		case( CT_TIE_CLAMP_YOKE ):		//fallthrough
			pComponent = AddATieClampYoke( eSide, dHeight, dLength, eMaterialType );
			break;
		case( CT_TIE_CLAMP_90DEGREE ):
			pComponent = AddATieClamp90deg( eSide, dHeight, eMaterialType );
			break;
		case( CT_BRACING ):
			//you either have bracing the entire way up a side of a Bay or you don't
			//	so you can't add an individual brace this way
			assert( FALSE );
			return NULL;
		case( CT_STAIR ):
			//you either have stairs the entire way up a Bay or you don't
			//	so you can't add an individual stairs this way
			assert( FALSE );
			return NULL;
		case( CT_LADDER ):
			//you either have ladder the entire way up a Bay or you don't
			//	so you can't add an individual ladder this way
			assert( FALSE );
			return NULL;
		case( CT_LAPBOARD ):
			//eSide and dHeight are not enough to determine which lapboard you
			//	are wishing to add, you need a PlankPosRiseEnum value too
			assert( FALSE );
			return NULL;
		case( CT_DECKING_PLANK ):
			//eSide and dHeight are not enough to determine which board you
			//	are wishing to add, you need a PlankPosRiseEnum value too
			assert( FALSE );
			return NULL;
		case( CT_STAGE_BOARD ):
			//eSide and dHeight are not enough to determine which board you
			//	are wishing to add, you need a PlankPosRiseEnum value too
			assert( FALSE );
			return NULL;
		case( CT_LEDGER ):
			pComponent = AddALedger( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_TRANSOM ):
			pComponent = AddATransom( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_MESH_GUARD ):
			pComponent = AddAMeshguard( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_RAIL ):
			pComponent = AddARail( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_MID_RAIL ):
			pComponent = AddAMidrail( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_TOE_BOARD ):
			pComponent = AddAToeBoard( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_HOPUP_BRACKET ):
			pComponent = AddAHopUpBracket( eSide, dLength, dHeight, eMaterialType );
			break;
		case( CT_CORNER_STAGE_BOARD ):
			pComponent = AddACornerStageBoard( eSide, dLength, dHeight, eMaterialType );
			break;
		default:
			assert( FALSE );	//this is not the way to add this component
			return NULL;
	}
	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//GetComponent
//retrieves a pointer to a component in a lift.  I see that this will mostly be
//	used to determine if the component exists or not by testing for NULL!
Component *Bay::GetComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight  )const
{
	Component *pComponent;

	#ifdef _DEBUG
	switch( eComponentType )
	{
	///////////////////////////////////////////////////////
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
	case( CT_LADDER_PUTLOG ):
		assert( eSide==SOUTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_MID_RAIL ):
	case( CT_BRACING ):
	case( CT_TIE_BAR ):
	case( CT_STANDARD_CONNECTOR ):
		
		assert( eSide>=NORTH && eSide<=WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_TOE_BOARD ):
		assert( eSide==SOUTH || eSide==NORTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_TIE_TUBE ):
	case( CT_TIE_CLAMP_COLUMN ):
	case( CT_TIE_CLAMP_MASONARY ):
	case( CT_TIE_CLAMP_YOKE ):
	case( CT_TIE_CLAMP_90DEGREE ):
	case( CT_STAIR ):
	case( CT_LADDER ):
	case( CT_TEXT ):
	case( CT_STAIR_RAIL ):
	case( CT_STAIR_RAIL_STOPEND ):
	case( CT_TOE_BOARD_CLIP ):
	case( CT_PUTLOG_CLIP ):
	default:
		break;

	///////////////////////////////////////////////////////
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
	case( CT_CORNER_STAGE_BOARD ):
		assert( eSide>=NORTH_EAST && eSide<=SOUTH_WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_HOPUP_BRACKET ):
		assert( eSide>=NORTH_NORTH_EAST && eSide<=NORTH_NORTH_WEST );
		break;
	}
	#endif	//#ifdef _DEBUG

	switch( eComponentType )
	{
		case( CT_JACK ):		//fallthrough
		case( CT_SOLEBOARD ):	//fallthrough
		case( CT_CHAIN_LINK ):	//fallthrough
		case( CT_SHADE_CLOTH ):
			//There is only one component of this type on any side, so ignor height
			//	value(iRise==0)
			pComponent = m_caComponents.GetComponent( eSide, 0, eComponentType );
			break;
		case( CT_TIE_TUBE ):			//fallthrough
		case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
		case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
		case( CT_TIE_CLAMP_YOKE ):	//fallthrough
		case( CT_TIE_CLAMP_90DEGREE ):
			pComponent = GetABayHorizontalComponent( eComponentType, eSide, dHeight );
			break;
		case( CT_STANDARD ):	//fallthrough
		case( CT_STANDARD_OPENEND ):	//fallthrough
		case( CT_STAIR ):		//fallthrough
		case( CT_LADDER ):
			pComponent = GetABayVerticalComponent( eComponentType, eSide, dHeight );
			break;
		case( CT_LEDGER ):				//fallthrough
		case( CT_TRANSOM ):				//fallthrough
		case( CT_MESH_GUARD ):			//fallthrough
		case( CT_RAIL ):				//fallthrough
		case( CT_MID_RAIL ):			//fallthrough
		case( CT_TOE_BOARD ):			//fallthrough
		case( CT_HOPUP_BRACKET ):		//fallthrough
		case( CT_CORNER_STAGE_BOARD ):
			pComponent = GetALiftComponent( eComponentType, eSide, dHeight  );
			break;
		case( CT_LAPBOARD ):			//fallthrough
		case( CT_STAGE_BOARD ):			//fallthrough
		case( CT_DECKING_PLANK ):
			//eSide and dHeight are not enough to determine which plank you
			//	are talking about, you need a PlankPosRiseEnum value too
			return NULL;
		case( CT_BRACING ):
			//eSide and dHeight are not enough to determine which plank you
			//	are talking about, you need a PlankPosRiseEnum value too
			return NULL;
		default:
			assert( FALSE );	//this is not the way to add this component
			return NULL;
	}
	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteComponent
//Deletes a particular component/s from a lift or Bay.
bool Bay::DeleteComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight/*=ID_ALL_IDS*/ )
{
	int			iRise;
	double		dLiftHeight;
	Lift		*pLift;
	Component	*pComponent;

	pComponent = GetComponent( eComponentType, eSide, dHeight );
	if( pComponent==NULL )
		return false;

	switch( eComponentType )
	{
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
	case( CT_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_RAIL ):
	case( CT_MID_RAIL ):
	case( CT_TOE_BOARD ):
	case( CT_TIE_TUBE ):
	case( CT_TIE_CLAMP_COLUMN ):
	case( CT_TIE_CLAMP_MASONARY ):
	case( CT_TIE_CLAMP_YOKE ):
	case( CT_TIE_CLAMP_90DEGREE ):
	case( CT_STAIR ):
	case( CT_LADDER ):
	case( CT_HOPUP_BRACKET ):
	case( CT_CORNER_STAGE_BOARD ):
	case( CT_TEXT ):
		pLift = GetLiftAtHeight( dHeight );
		if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )
			return NULL;

		//what is the height relative to the bottom of the lift?
		dHeight-= dLiftHeight;
		iRise = GetRiseEnumFromRise( dHeight );
		pComponent = pLift->GetComponent( eComponentType, iRise, eSide );
		if( pComponent==NULL )
			return false;

		pLift->DeleteComponent( pComponent->GetID() );
		break;

	case( CT_MESH_GUARD ):
	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_BRACING ):
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
		if( !DeleteBayComponent( m_caComponents.GetComponentID( pComponent ) ) )
			return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
//Add/Remove Components to Bay (Plan View)
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//DeleteBrace
//Deletes all bracing from that side of the bay - NOT REQUIRED (see Bracing above)
//	eSide should be a  binary (0x0001, 0x0002, 0x0004…) value so that we can do multiple sides at a time - NOT REQUIRED (see Bracing above)
bool Bay::DeleteBrace( SideOfBayEnum eSide )
{
	return DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eSide);
}

/////////////////////////////////////////////////////////////////////////////////
//AddHandrail
//adds individual handrail to a side of the bay
//	Needs a Toolbar with
//	Handrail Style
//	UI to position Handrail visually on Bay (also consider Lapboards)
//	User should be prompted to add to entire run
bool Bay::AddHandrail( SideOfBayEnum eSide/*=NORTH*/, bool bCheckNeighborSytle/*=false*/ )
{
	EnsureHandrailsAreCorrect(eSide, bCheckNeighborSytle );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteHandrail
//Deletes all Handrails from that side of a bay.
//	User should be prompted to Delete from entire run
//	eSide should be a  binary (0x0001, 0x0002, 0x0004…) value so that we can do multiple sides at a time AddStages( SideOfBayEnum eSide/*=NORTH*/ ) - adds individual bracing to a side of the bay
bool Bay::DeleteHandrail( SideOfBayEnum eSide )
{
	bool bReturn;
	bReturn = true;
	if( !DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide) )
		bReturn = false;
	DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide);
	DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide);
	//hopup rails
	SideOfBayEnum eSide1, eSide2;
	switch( eSide )
	{
	case( NORTH ):
		eSide1 = ENE;
		eSide2 = WNW;
		break;
	case( SOUTH ):
		eSide1 = ESE;
		eSide2 = WSW;
		break;
	case( EAST ):
		eSide1 = NNE;
		eSide2 = SSE;
		break;
	case( WEST ):
		eSide1 = NNW;
		eSide2 = NNE;
		break;
	default:
		eSide1 = SIDE_INVALID;
		eSide2 = SIDE_INVALID;
	}
	if( eSide1!=SIDE_INVALID )
	{
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide1, LIFT_RISE_1000MM );
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide2, LIFT_RISE_1000MM );
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////////
//AddBoards
//adds boards to all 2m high lifts within a bay (or maybe only apply to those decks above RL0
//	Needs a toolbar with
//	Material Type
//	User should be prompted to apply to entire run
bool Bay::AddBoards( )
{
	//??JSB todo 990828 - complete this function
	assert( false );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteBoards
//Deletes all boards from all lifts within a bay
//	User should be prompted to apply to entire run
bool Bay::DeleteBoards( )
{
	return DeleteAllBayComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );
}

/////////////////////////////////////////////////////////////////////////////////
//AddTie
//adds ties 
//	Needs a Toolbar - It can now get this information from the Run
//	Type of Tie ( Column, Yolk, Masonry )
//	Clamps ( 90 deg, Standard )
//	ties should be added every X meters from the first star, not from RL0.
//	Vertical separation
bool Bay::AddTie( )
{
	//??JSB todo 990828 - complete this function
	assert( false );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//DeleteTies
//Deletes all ties from a bay
bool Bay::DeleteTies( SideOfBayEnum eSide )
{
	bool bReturn;
	bReturn = false;

	switch( eSide )
	{
	default:
	case( SIDE_INVALID ):		//fallthrough
	case( NORTH ):				//fallthrough
	case( EAST ):				//fallthrough
	case( SOUTH ):				//fallthrough
	case( WEST ):				//fallthrough
		assert( false );
		break;
	case( NORTH_EAST ):			//fallthrough
	case( SOUTH_EAST ):			//fallthrough
	case( NORTH_WEST ):			//fallthrough
	case( SOUTH_WEST ):
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_TUBE, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_90DEGREE, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_MESH_GUARD, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TOE_BOARD, eSide);
		break;
	case( NORTH_NORTH_EAST ):	//fallthrough
	case( EAST_NORTH_EAST ):	//fallthrough
	case( EAST_SOUTH_EAST ):	//fallthrough
	case( SOUTH_SOUTH_EAST ):	//fallthrough
	case( SOUTH_SOUTH_WEST ):	//fallthrough
	case( WEST_SOUTH_WEST ):	//fallthrough
	case( WEST_NORTH_WEST ):	//fallthrough
	case( NORTH_NORTH_WEST ):	//fallthrough
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_TUBE, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_COLUMN, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_MASONARY, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_YOKE, eSide);
		bReturn|= !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_90DEGREE, eSide);
		break;
	case( ALL_SIDES ):			//fallthrough
		bReturn|= DeleteTies( NORTH_NORTH_EAST );
		bReturn|= DeleteTies( EAST_NORTH_EAST );
		bReturn|= DeleteTies( EAST_SOUTH_EAST );
		bReturn|= DeleteTies( SOUTH_SOUTH_EAST );
		bReturn|= DeleteTies( SOUTH_SOUTH_WEST );
		bReturn|= DeleteTies( WEST_SOUTH_WEST );
		bReturn|= DeleteTies( WEST_NORTH_WEST );
		bReturn|= DeleteTies( NORTH_NORTH_WEST );
		bReturn|= DeleteTies( NORTH_EAST );
		bReturn|= DeleteTies( SOUTH_EAST );
		bReturn|= DeleteTies( SOUTH_WEST );
		bReturn|= DeleteTies( NORTH_WEST );
		break;
	case( ALL_VISIBLE_SIDES ):
		if( GetOuter()==NULL )
		{
			bReturn|= DeleteTies( NORTH_NORTH_WEST );
			bReturn|= DeleteTies( NORTH_NORTH_EAST );
		}
		if( GetForward()==NULL )
		{
			bReturn|= DeleteTies( EAST_NORTH_EAST );
			bReturn|= DeleteTies( EAST_SOUTH_EAST );
		}
		if( GetInner()==NULL )
		{
			bReturn|= DeleteTies( SOUTH_SOUTH_EAST );
			bReturn|= DeleteTies( SOUTH_SOUTH_WEST );
		}
		if( GetBackward()==NULL )
		{
			bReturn|= DeleteTies( WEST_SOUTH_WEST );
			bReturn|= DeleteTies( WEST_NORTH_WEST );
		}
		break;
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////////
//Drawing routines
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//Redraw
//Redraws the Plan and 3D views.
void Bay::Redraw()
{
	UpdateSchematicView();
}

/////////////////////////////////////////////////////////////////////////////////
//RedrawPlanView
//Deletes the Plan view and recreates using the object data.  To complete this we will need to be able to determine if the bays have Handrails, Bracing, Ties etc (i.e. any component visible in Plan view).  Colour coding should also be done by this function, colour would be determined by Bay width
void Bay::RedrawPlanView()
{
	UpdateSchematicView();
}

/////////////////////////////////////////////////////////////////////////////////
//Deletes coordinates of all components and repositions them again
void Bay::Redraw3Dview()
{
	//??JSB todo 990828 - complete this function
	assert( false );
}

/////////////////////////////////////////////////////////////////////////////////
//Private functionality
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetRLOfStars
//This function will find the RL of a star if give a corner, or if given a side
//	it will find the lowest RL which has the give StarNumber, provided that the
//	star has a corresponding star on the other pole. 
bool Bay::GetRLOfStars(	int iStarNumber, double &dRL, SideOfBayEnum eSide/*=NORTH*/ ) const
{
	double dRL1, dRL2, dHgt1, dHgt2,
			dBottomStarRL1, dBottomStarRL2,
			dTopStarRL1, dTopStarRL2;
	switch( eSide )
	{
		case( NORTH_NORTH_EAST ):	//fallthrough
		case( EAST_NORTH_EAST ):	//fallthrough
		case( EAST_SOUTH_EAST ):	//fallthrough
		case( SOUTH_SOUTH_EAST ):	//fallthrough
		case( SOUTH_SOUTH_WEST ):	//fallthrough
		case( WEST_SOUTH_WEST ):	//fallthrough
		case( WEST_NORTH_WEST ):	//fallthrough
		case( NORTH_NORTH_WEST ):	//fallthrough
		default:					//fallthrough
		case ( ALL_VISIBLE_SIDES ):	//fallthrough
			assert( FALSE );	//we couldn't be talking about bracing something has gone wrong!
			return false;
			break;
		case ( ALL_SIDES ):
			assert( FALSE );	//Not implemented yet
			return false;
			break;
		case( NORTH_EAST ):  return GetRLOfStar( iStarNumber, dRL, CNR_NORTH_EAST );
		case( SOUTH_EAST ):  return GetRLOfStar( iStarNumber, dRL, CNR_SOUTH_EAST );
		case( NORTH_WEST ):  return GetRLOfStar( iStarNumber, dRL, CNR_NORTH_WEST );
		case( SOUTH_WEST ):  return GetRLOfStar( iStarNumber, dRL, CNR_SOUTH_WEST );
		case( NORTH ):
			if( !GetRLOfStar( iStarNumber, dRL1, CNR_NORTH_EAST ) ) return false;
			if( !GetRLOfStar( iStarNumber, dRL2, CNR_NORTH_WEST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL1, CNR_NORTH_EAST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL2, CNR_NORTH_WEST ) ) return false;
			dHgt1 = GetHeightOfStandards( CNR_NORTH_EAST );
			dHgt2 = GetHeightOfStandards( CNR_NORTH_WEST );
			break;
		case( EAST ):
			if( !GetRLOfStar( iStarNumber, dRL1, CNR_NORTH_EAST ) ) return false;
			if( !GetRLOfStar( iStarNumber, dRL2, CNR_SOUTH_EAST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL1, CNR_NORTH_EAST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL2, CNR_SOUTH_EAST ) ) return false;
			dHgt1 = GetHeightOfStandards( CNR_NORTH_EAST );
			dHgt2 = GetHeightOfStandards( CNR_SOUTH_EAST );
			break;
		case( SOUTH ):
			if( !GetRLOfStar( iStarNumber, dRL1, CNR_SOUTH_EAST ) ) return false;
			if( !GetRLOfStar( iStarNumber, dRL2, CNR_SOUTH_WEST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL1, CNR_SOUTH_EAST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL2, CNR_SOUTH_WEST ) ) return false;
			dHgt1 = GetHeightOfStandards( CNR_SOUTH_EAST );
			dHgt2 = GetHeightOfStandards( CNR_SOUTH_WEST );
			break;
		case( WEST ):
			if( !GetRLOfStar( iStarNumber, dRL1, CNR_NORTH_WEST ) ) return false;
			if( !GetRLOfStar( iStarNumber, dRL2, CNR_SOUTH_WEST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL1, CNR_NORTH_WEST ) ) return false;
			if( !GetRLOfStar( 0, dBottomStarRL2, CNR_SOUTH_WEST ) ) return false;
			dHgt1 = GetHeightOfStandards( CNR_NORTH_WEST );
			dHgt2 = GetHeightOfStandards( CNR_SOUTH_WEST );
			break;
	}

	//find the height of the top star
	dTopStarRL1 = dBottomStarRL1+dHgt1;
	dTopStarRL2 = dBottomStarRL2+dHgt2;

	//check if the poles overlap - they must
	if( (dTopStarRL1<dBottomStarRL2) || (dTopStarRL2<dBottomStarRL1) )
		return false;

	if( dRL1<dRL2 )
	{
		//RL1 is the lower star so is there a corresponding star on the second pole
		if( (dRL1>=dBottomStarRL2) && (dRL1<=dTopStarRL2) )
		{
			dRL = dRL1;
		}
		else
		{
			//RL1 didn't work, so we need to check if the RL2 we found will work
			if( (dRL2>=dBottomStarRL1) && (dRL2<=dTopStarRL1) )
			{
				dRL = dRL2;
			}
			else
			{
				//the two RL's don't have a corresponding star on the other pole
				return false;
			}
		}
	}
	else
	{
		//RL2 is the lower, or the same height, star so is there a
		//	corresponding star on the first pole
		if( (dRL2>=dBottomStarRL1) && (dRL2<=dTopStarRL1) )
		{
			dRL = dRL2;
		}
		else
		{
			//RL2 didn't work, so we need to check if the RL1 we found will work
			if( (dRL1>=dBottomStarRL2) && (dRL1<=dTopStarRL2) )
			{
				dRL = dRL1;
			}
			else
			{
				//the two RL's don't have a corresponding star on the other pole
				return false;
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetRLOfStar
//This funciton will find the RL for an inidivual star.  Will return false if the
//	star is higher than the height of the pole
bool Bay::GetRLOfStar(	int iStarNumber, double &dRL, CornerOfBayEnum eStandardCorner ) const
{
	double dStarHeight;

	dStarHeight = iStarNumber*GetStarSeparation();
	//the star we are trying to find can't be highter than the post.
	if( dStarHeight>m_daHeights[eStandardCorner] )
		return false;

	dRL = GetStandardPosition(eStandardCorner).z + dStarHeight;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//GetBracingHeight
//Retrieves the Bracing Height (ie the number of stars) covered by some particular
//	bracing.  The function needs the Baywidth and since in most case there are two
//	Lengths of bracing for any particular bay with, you can specify if you want the
//	longer or the shorter of the two Lengths.
double Bay::GetBracingRaise(double dBayWidth ) const
{
	double dRaise;

	dRaise = GetRunPointer()->GetBracingRaise( dBayWidth );

/*	dRaise =0.00;
	if( GetRunPointer()->GetUseLongerBracing() )
	{
		if( dBayWidth==COMPONENT_LENGTH_2400 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_3600 );
		if( dBayWidth==COMPONENT_LENGTH_1800 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_2700 );
		if( dBayWidth==COMPONENT_LENGTH_1200 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_2100 );
		if( dBayWidth==COMPONENT_LENGTH_0700 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_1700 );
	}
	else
	{
		if( dBayWidth==COMPONENT_LENGTH_2400 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_3200 );
		if( dBayWidth==COMPONENT_LENGTH_1800 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_2700 );
		if( dBayWidth==COMPONENT_LENGTH_1200 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_1900 );
		if( dBayWidth==COMPONENT_LENGTH_0700 ) dRaise = GetBracingRaise( dBayWidth, COMPONENT_LENGTH_1300 );
	}
*/
	return dRaise;
}

/////////////////////////////////////////////////////////////////////////////////
//GetBracingHeight
double Bay::GetBracingRaise(double dBayWidth, double dBraceLength ) const
{
	assert( false );
	return 0.00;
/*	double dRaise;
	dRaise =0.00;

	if( dBayWidth==COMPONENT_LENGTH_2400 && dBraceLength==COMPONENT_LENGTH_3600 )	dRaise = COMPONENT_LENGTH_2500;
	if( dBayWidth==COMPONENT_LENGTH_2400 && dBraceLength==COMPONENT_LENGTH_3200 )	dRaise = COMPONENT_LENGTH_2000;
	if( dBayWidth==COMPONENT_LENGTH_1800 && dBraceLength==COMPONENT_LENGTH_2700 )	dRaise = COMPONENT_LENGTH_2000;
	if( dBayWidth==COMPONENT_LENGTH_1200 && dBraceLength==COMPONENT_LENGTH_2100 )	dRaise = COMPONENT_LENGTH_1500;
	if( dBayWidth==COMPONENT_LENGTH_1200 && dBraceLength==COMPONENT_LENGTH_1900 )	dRaise = COMPONENT_LENGTH_1500;
	if( dBayWidth==COMPONENT_LENGTH_0700 && dBraceLength==COMPONENT_LENGTH_1700 )	dRaise = COMPONENT_LENGTH_1500;
	if( dBayWidth==COMPONENT_LENGTH_0700 && dBraceLength==COMPONENT_LENGTH_1300 )	dRaise = COMPONENT_LENGTH_1000;

	return dRaise;
*/
}

/////////////////////////////////////////////////////////////////////////////////
//GetBracingLength
double Bay::GetBracingLength(double dBayWidth ) const
{
	double dBraceLength;
	dBraceLength = GetRunPointer()->GetBracingLength( dBayWidth );
/*
	dBraceLength = 0.00;
	if( GetRunPointer()->GetUseLongerBracing() )
	{
		if( dBayWidth==COMPONENT_LENGTH_2400 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_2500 );
		if( dBayWidth==COMPONENT_LENGTH_1800 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_2000 );
		if( dBayWidth==COMPONENT_LENGTH_1200 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_1500 );
		if( dBayWidth==COMPONENT_LENGTH_0700 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_1500 );
	}
	else
	{
		if( dBayWidth==COMPONENT_LENGTH_2400 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_2000 );
		if( dBayWidth==COMPONENT_LENGTH_1800 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_2000 );
		if( dBayWidth==COMPONENT_LENGTH_1200 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_1500 );
		if( dBayWidth==COMPONENT_LENGTH_0700 ) dBraceLength = GetBracingLength( dBayWidth, COMPONENT_LENGTH_1000 );
	}
*/
	return dBraceLength;
}

/////////////////////////////////////////////////////////////////////////////////
//GetBracingLength
double Bay::GetBracingLength(double dBayWidth, double dRaise ) const
{
	assert( false );
	return 0.00;
/*
	double dBraceLength;
	dBraceLength = 0.00;

	if     ( dBayWidth==COMPONENT_LENGTH_2400 && dRaise==COMPONENT_LENGTH_2500 )	dBraceLength = COMPONENT_LENGTH_3600;
	else if( dBayWidth==COMPONENT_LENGTH_2400 && dRaise==COMPONENT_LENGTH_2000 )	dBraceLength = COMPONENT_LENGTH_3200;
	else if( dBayWidth==COMPONENT_LENGTH_1800 && dRaise==COMPONENT_LENGTH_2000 )	dBraceLength = COMPONENT_LENGTH_2700;
	else if( dBayWidth==COMPONENT_LENGTH_1200 && dRaise==COMPONENT_LENGTH_1500 )
	{
		if( GetController()->GetSystem()==S_MILLS )
			dBraceLength = COMPONENT_LENGTH_1900;
		else
			dBraceLength = COMPONENT_LENGTH_2100;
	}
	else if( dBayWidth==COMPONENT_LENGTH_0700 && dRaise==COMPONENT_LENGTH_1500 )	dBraceLength = COMPONENT_LENGTH_1700;
	else if( dBayWidth==COMPONENT_LENGTH_0700 && dRaise==COMPONENT_LENGTH_1000 )	dBraceLength = COMPONENT_LENGTH_1300;
	else
	{
		assert( false );
	}

	return dBraceLength;
*/
}


/////////////////////////////////////////////////////////////////////////////////
//Testing the Matrix Element
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//TestHandRailHeights
//This Test will confirm that the Handrail that exist in the matrix match the
//	Handrail in the bay this has to be a 1:1 match to succeed.
bool Bay::TestHandRailHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetHandrailHeights( daMatrixArrange, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetHandrailHeights( daMatrixArrange, MATRIX_STANDARDS_OUTER );

	CreateMatrixHandrailArrangement( daArrangement, bInner, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestLedgerHeights
//This Test will confirm that the Ledger that exist in the matrix match the
//	Ledger in the bay this has to be a 1:1 match to succeed.
bool Bay::TestLedgerHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetLedgerHeights( daMatrixArrange, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetLedgerHeights( daMatrixArrange, MATRIX_STANDARDS_OUTER );

	CreateMatrixLedgerArrangement( daArrangement, bInner, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestMidRailHeights
//This Test will confirm that the Midrail that exist in the matrix match the
//	Midrail in the bay this has to be a 1:1 match to succeed.
bool Bay::TestMidRailHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetMidrailHeights( daMatrixArrange, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetMidrailHeights( daMatrixArrange, MATRIX_STANDARDS_OUTER );

	CreateMatrixMidrailArrangement( daArrangement, bInner, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestToeBoardHeights
//This Test will confirm that the Toe Board that exist in the matrix match the
//	Midrail in the bay this has to be a 1:1 match to succeed.
bool Bay::TestToeBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetToeBoardHeights( daMatrixArrange, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetToeBoardHeights( daMatrixArrange, MATRIX_STANDARDS_OUTER );

	CreateMatrixToeBoardArrangement( daArrangement, bInner, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestEndHandRailHeights
//This Test will confirm that the End Handrail that exist in the matrix match the
//	End Handrail in the bay this has to be a 1:1 match to succeed.
bool Bay::TestEndHandRailHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	pMatrixElement->GetEndHandrailHeights( daMatrixArrange );
	
	CreateMatrixEndHandrailArrangement( daArrangement, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestEndMidRailHeights
//This Test will confirm that the End Midrail that exist in the matrix match the
//	End Midrail in the bay this has to be a 1:1 match to succeed.
bool Bay::TestEndMidRailHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	pMatrixElement->GetEndMidrailHeights( daMatrixArrange );

	CreateMatrixEndMidrailArrangement( daArrangement, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestStageBoardHeights
//This Test will confirm that the Stage boards that exist in the matrix match the
//	Stageboards in the bay this has to be a 1:1 match to succeed.
bool Bay::TestStageBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	intArray	iaPlanks, iaMatrixPlanks;
	doubleArray	daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetStageHeights( daMatrixArrange, iaMatrixPlanks, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetStageHeights( daMatrixArrange, iaMatrixPlanks, MATRIX_STANDARDS_OUTER );

	CreateMatrixStageArrangement( daArrangement, iaPlanks, bInner, bUseAlt );

	if( AreArrangementsSame( daMatrixArrange, daArrangement ) )
	{
		if( AreArrangementsSame( iaMatrixPlanks, iaPlanks ) )
		{
			return true;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////
//TestHopupRailBoardHeights
//This Test will confirm that the HopupRail boards that exist in the matrix match the
//	HopupRailboards in the bay this has to be a 1:1 match to succeed.
bool Bay::TestHopupRailBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	intArray	iaPlanks, iaMatrixPlanks;
	doubleArray	daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	if( bInner )
		pMatrixElement->GetHopupRailsHeights( daMatrixArrange, iaMatrixPlanks, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetHopupRailsHeights( daMatrixArrange, iaMatrixPlanks, MATRIX_STANDARDS_OUTER );

	CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, bInner, bUseAlt );

	if( AreArrangementsSame( daMatrixArrange, daArrangement ) )
	{
		if( AreArrangementsSame( iaMatrixPlanks, iaPlanks ) )
		{
			return true;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////
//TestDeckingHeights
//This Test will confirm that the Decking that exist in the matrix match the
//	Decking in the bay this has to be a 1:1 match to succeed.
bool Bay::TestDeckingHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	pMatrixElement->GetDeckingHeights( daMatrixArrange );

	CreateMatrixDeckingArrangement( daArrangement, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestTransomHeights
//This Test will confirm that the trasoms that exist in the matrix match the
//	transoms in the bay this has to be a 1:1 match to succeed.
bool Bay::TestTransomHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const
{
	doubleArray		daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	pMatrixElement->GetTransomHeights( daMatrixArrange );

	CreateMatrixTransomArrangment( daArrangement, bUseAlt );

	return AreArrangementsSame( daMatrixArrange, daArrangement );
}

/////////////////////////////////////////////////////////////////////////////////
//TestStandardsArrangment
//This Test will confirm that the arrangement of the standards that exist in the
//	matrix match the Arrangement in the bay, this has to be a 1:1 match to succeed.
bool Bay::TestStandardsArrangment( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const
{
	bool		bSoleBoard, bMatrixSoleBoard;
	doubleArray	daMatrixArrange, daArrangement;

	daMatrixArrange.SetUniqueElementsOnly();
	daArrangement.SetUniqueElementsOnly();

	bSoleBoard = false;
	bMatrixSoleBoard = false;
	if( bInner )
		pMatrixElement->GetStandardsArrangement( daMatrixArrange, bMatrixSoleBoard, MATRIX_STANDARDS_INNER );
	else
		pMatrixElement->GetStandardsArrangement( daMatrixArrange, bMatrixSoleBoard, MATRIX_STANDARDS_OUTER );

	CreateMatrixStandardArrangment( daArrangement, bSoleBoard, bInner, bUseAlt );

	if( bSoleBoard==bMatrixSoleBoard )
		return AreArrangementsSame( daMatrixArrange, daArrangement );

	return false;
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixDeckingArrangement
//Set the Matrix element's Decking to be the same as this bay's
bool Bay::CreateMatrixDeckingArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dRL;
	SideOfBayEnum	eSide;
	Lift			*pLift;

	eSide =	( bUseAlt )? WEST: EAST;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a deck in the lift?
		if( pLift->IsDeckingLift() )
		{
			//get the RL of the lift
			dRL = m_LiftList.GetRL( i );

			daArrangement.Add( dRL );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixDeckingArrangement( daArrangement, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixTransomArrangment
//Set the Matrix element's Transoms to be the same as this bay's
bool Bay::CreateMatrixTransomArrangment( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i, j;
	double			dHeight;
	SideOfBayEnum	eSide;
	Lift			*pLift;
	Component		*pComponent;

	eSide =	( bUseAlt )? WEST: EAST;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		for( j=(int)LIFT_RISE_0000MM; j<=(int)pLift->GetRise(); j++ )
		{
			//do we have a transom in the lift?
			pComponent = NULL;
			pComponent = pLift->GetComponent( CT_TRANSOM, (LiftRiseEnum)j, eSide );

			if( pComponent!=NULL )
			{
				//Yep we have a transom in this lift!

				//get the RL of the lift
				dHeight = pComponent->GetRL();
				assert( dHeight<(pLift->GetRL()+(j*GetStarSeparation())+ROUND_ERROR) &&
						dHeight>(pLift->GetRL()+(j*GetStarSeparation())-ROUND_ERROR) );

				daArrangement.Add( dHeight );
			}
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixTransomArrangment( daArrangement, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixStandardArrangment
//Set the Matrix element's Standards to be the same as this bay's
bool Bay::CreateMatrixStandardArrangment( doubleArray &daArrangement, bool &bSoleBoard, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dRL;
	Component		*pComponent;
	doubleArray		dStandards;
	SideOfBayEnum	eNeighborSide;
	CornerOfBayEnum	eCnr;

	if( bInner )
		eCnr = ( bUseAlt )? CNR_SOUTH_WEST: CNR_SOUTH_EAST;
	else
		eCnr = ( bUseAlt )? CNR_NORTH_WEST: CNR_NORTH_EAST;

	if( !GetStandardsArrangement( dStandards, eCnr ) )
		return false;

	daArrangement.RemoveAll();

	if( bCheckNeighbor || GetBayThatOwnsSide( CornerAsSideOfBay(eCnr), eNeighborSide )==NULL )
	{
		//We are either checking the neighbor, or this standard is owned by this bay
		dRL = GetStandardPosition( eCnr ).z;
		for( i=0; i<dStandards.GetSize(); i++ )
		{
			daArrangement.Add( dRL );
			dRL+= dStandards.GetAt( i );
		}
		daArrangement.Add( dRL );
		pComponent = GetComponent( CT_SOLEBOARD, CornerAsSideOfBay(eCnr), 0.00 );
		bSoleBoard = (pComponent!=NULL);

		if( bCheckNeighbor )
		{
			Bay *pNeighbor;
			pNeighbor = GetBayThatOwnsSide( CornerAsSideOfBay(eCnr), eNeighborSide );
			if( pNeighbor!=NULL )
			{
				pComponent = pNeighbor->GetComponent( CT_SOLEBOARD, eNeighborSide, 0.00 );
				bSoleBoard = (pComponent!=NULL);
			}
		}
	}

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixStageArrangement
//Set the Matrix element's Stages to be the same as this bay's
bool Bay::CreateMatrixStageArrangement( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/  ) const
{
	int				i, iNumberOfBoards;
	double			dHeight;
	SideOfBayEnum	eSide;
	Lift			*pLift;

	eSide =	( bInner )? SOUTH: NORTH;

	if( bCheckNeighbor )
	{
		daArrangement.RemoveAll();
		iaPlanks.RemoveAll();
	}

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a stage in the lift?
		iNumberOfBoards	= pLift->GetNumberOfPlanksOnStageBoard( eSide );

		if( iNumberOfBoards>0 )
		{
			//Yep we have a planks in this lift!

			//get the RL of the lift
			dHeight = m_LiftList.GetRL( i );
	
			if( daArrangement.Add( dHeight )>=0 )
				iaPlanks.Add( iNumberOfBoards );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixStageArrangement( daArrangement, iaPlanks, bInner, !bUseAlt, false );
		}

		assert( daArrangement.GetSize()==iaPlanks.GetSize() );
	}

	//////////////////////////////////////////////////////////
	//Sort the two arrays here, by the RL
	int		j, iValue, iTemp;
	double	dValue, dTemp;
	assert( daArrangement.GetSize()==iaPlanks.GetSize() );
	for( i=0; i<daArrangement.GetSize()-1; i++ )
	{
		dValue = daArrangement.GetAt(i);
		iValue = iaPlanks.GetAt(i);
		for( j=i+1; j<daArrangement.GetSize(); j++ )
		{
			//Sort by RL
			if( dValue>daArrangement.GetAt(j) )
			{
				dTemp = daArrangement.GetAt(j);
				iTemp = iaPlanks.GetAt(j);
				daArrangement.SetAt( i, dTemp );
				iaPlanks.SetAt( i, iTemp );
				daArrangement.SetAt( j, dValue );
				iaPlanks.SetAt( j, iValue );
				dValue = dTemp;
				iValue = iTemp;
			}
		}
	}

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixHopupRailArrangement
//Set the Matrix element's HopupRails to be the same as this bay's
bool Bay::CreateMatrixHopupRailArrangement( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/  ) const
{
	int				i, iNumberOfBoards;
	Lift			*pLift;
	double			dHeight;
	Component		*pComp;
	SideOfBayEnum	eSide;

	if( bUseAlt )
		eSide =	( bInner )? SSW: NNW;
	else
		eSide =	( bInner )? SSE: NNE;

	if( bCheckNeighbor )
	{
		daArrangement.RemoveAll();
		iaPlanks.RemoveAll();
	}

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1.0m HopupRail in the lift?
		pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSide);
		iNumberOfBoards = 0;
		if( pComp!=NULL )
		{
			iNumberOfBoards = (int)pComp->GetLengthCommon();

			if( iNumberOfBoards>0 )
			{
				//Yep we have a planks in this lift!

				//get the RL of the lift
				dHeight = m_LiftList.GetRL( i )+GetRiseFromRiseEnum( LIFT_RISE_1000MM, GetStarSeparation() );
				if( daArrangement.Add( dHeight )>=0 )
					iaPlanks.Add( iNumberOfBoards );
			}
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixHopupRailArrangement( daArrangement, iaPlanks, bInner, !bUseAlt, false );
		}

		assert( daArrangement.GetSize()==iaPlanks.GetSize() );
	}

	//////////////////////////////////////////////////////////
	//Sort the two arrays here, by the RL
	int		j, iValue, iTemp;
	double	dValue, dTemp;
	for( i=0; i<daArrangement.GetSize()-1; i++ )
	{
		dValue = daArrangement.GetAt(i);
		iValue = iaPlanks.GetAt(i);
		for( j=i+1; j<daArrangement.GetSize(); j++ )
		{
			//Sort by RL
			if( dValue>daArrangement.GetAt(j) )
			{
				dTemp = daArrangement.GetAt(j);
				iTemp = iaPlanks.GetAt(j);
				daArrangement.SetAt( i, dTemp );
				iaPlanks.SetAt( i, iTemp );
				daArrangement.SetAt( j, dValue );
				iaPlanks.SetAt( j, iValue );
				dValue = dTemp;
				iValue = iTemp;
			}
		}
	}

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixEndHandrailArrangement
//Set the Matrix element's end handrail to be the same as this bay's
bool Bay::CreateMatrixEndHandrailArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dHeight;
	SideOfBayEnum	eSide;
	Lift			*pLift;
	Component		*pComponent;

	eSide =	( bUseAlt )? WEST: EAST;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1m handrail in the lift?
		pComponent	= pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSide );

		if( pComponent!=NULL )
		{
			//Yep we have a handrail in this lift!

			//get the RL of the lift + the height of the handrail (2 stars up = 1m)
			dHeight = m_LiftList.GetRL( i ) + (GetStarSeparation()*2);

			daArrangement.Add( dHeight );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixEndHandrailArrangement( daArrangement, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixEndMidrailArrangement
//Set the Matrix element's end Midrail to be the same as this bay's
bool Bay::CreateMatrixEndMidrailArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dHeight;
	SideOfBayEnum	eSide;
	Lift			*pLift;
	Component		*pComponent;

	eSide =	( bUseAlt )? WEST: EAST;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1m Midrail in the lift?
		pComponent	= pLift->GetComponent( CT_MID_RAIL, LIFT_RISE_0500MM, eSide );

		if( pComponent!=NULL )
		{
			//Yep we have a Midrail in this lift!

			//get the RL of the lift + the height of the Midrail (1 stars up = .5m)
			dHeight = m_LiftList.GetRL( i ) + (GetStarSeparation()*1);

			daArrangement.Add( dHeight );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixEndMidrailArrangement( daArrangement, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixHandrailArrangement
//Set the Matrix element's handrail to be the same as this bay's
bool Bay::CreateMatrixHandrailArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dHeight;
	Lift			*pLift;
	Component		*pComponent;
	SideOfBayEnum	eSide;

	//JSB todo 991129 - should this be used?
	bUseAlt;

	eSide =	( bInner )? SOUTH: NORTH;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1m handrail in the lift?
		pComponent	= pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSide );

		if( pComponent!=NULL )
		{
			//Yep we have a handrail in this lift!

			//get the RL of the lift + the height of the handrail (2 stars up = 1m)
			dHeight = m_LiftList.GetRL( i ) + (GetStarSeparation()*2);

			daArrangement.Add( dHeight );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixHandrailArrangement( daArrangement, bInner, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixLedgerArrangement
//Set the Matrix element's Ledger to be the same as this bay's
bool Bay::CreateMatrixLedgerArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i, iRise;
	double			dHeight;
	Lift			*pLift;
	Component		*pComponent;
	SideOfBayEnum	eSide;

	//JSB todo 991129 - should this be used?
	bUseAlt;

	eSide =	( bInner )? SOUTH: NORTH;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		for( iRise=LIFT_RISE_0000MM; iRise<=pLift->GetRise(); iRise++ )
		{
			//do we have a 1m Ledger in the lift?
			pComponent	= pLift->GetComponent( CT_LEDGER, (LiftRiseEnum)iRise, eSide );

			if( pComponent!=NULL )
			{
				//Yep we have a Ledger in this lift!

				//get the RL of the lift + the height of the Ledger (2 stars up = 1m)
				dHeight = m_LiftList.GetRL( i ) + GetRiseFromRiseEnum( (LiftRiseEnum)iRise, GetStarSeparation() );
				daArrangement.Add( dHeight );
			}
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;

		if( eSide==SOUTH )
		{
			pNeighbor = GetInner();
			if( pNeighbor!=NULL )
			{
				pNeighbor->CreateMatrixLedgerArrangement( daArrangement, !bInner, bUseAlt, false );
			}
		}
		else
		{
			pNeighbor = GetOuter();
			if( pNeighbor!=NULL )
			{
				pNeighbor->CreateMatrixLedgerArrangement( daArrangement, !bInner, bUseAlt, false );
			}
		}

		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixLedgerArrangement( daArrangement, bInner, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}

/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixMidrailArrangement
//Set the Matrix element's Midrail to be the same as this bay's
bool Bay::CreateMatrixMidrailArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dHeight;
	Lift			*pLift;
	Component		*pComponent;
	SideOfBayEnum	eSide;

	//JSB todo 991129 - should this be used?
	bUseAlt;

	eSide =	( bInner )? SOUTH: NORTH;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1m Midrail in the lift?
		pComponent	= pLift->GetComponent( CT_MID_RAIL, LIFT_RISE_0500MM, eSide );

		if( pComponent!=NULL )
		{
			//Yep we have a Midrail in this lift!

			//get the RL of the lift + the height of the Midrail (1 stars up = .5m)
			dHeight = m_LiftList.GetRL( i ) + (GetStarSeparation()*1);

			daArrangement.Add( dHeight );
		}
	}

	if( bCheckNeighbor )
	{
		//check if the neighbor has any that we don't
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixMidrailArrangement( daArrangement, bInner, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}


/////////////////////////////////////////////////////////////////////////////////
//CreateMatrixToeBoardArrangement
//Set the Matrix element's Midrail to be the same as this bay's
bool Bay::CreateMatrixToeBoardArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor/*=true*/ ) const
{
	int				i;
	double			dHeight;
	Lift			*pLift;
	Component		*pComponent;
	SideOfBayEnum	eSide;

	//JSB todo 991129 - should this be used?
	bUseAlt;

	eSide =	( bInner )? SOUTH: NORTH;

	if( bCheckNeighbor )
		daArrangement.RemoveAll();

	//go through each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );

		//do we have a 1m Midrail in the lift?
		pComponent	= pLift->GetComponent( CT_TOE_BOARD, LIFT_RISE_0000MM, eSide );

		if( pComponent!=NULL )
		{
			//Yep we have a Midrail in this lift!

			//get the RL of the lift
			dHeight = m_LiftList.GetRL( i );

			daArrangement.Add( dHeight );
		}
	}

	//check if the neighbor has any that we don't
	if( bCheckNeighbor )
	{
		Bay			*pNeighbor;
		if( bUseAlt )
			pNeighbor = GetBackward();
		else
			pNeighbor = GetForward();

		if( pNeighbor!=NULL )
		{
			pNeighbor->CreateMatrixToeBoardArrangement( daArrangement, bInner, !bUseAlt, false );
		}
	}

	//sort the arrangement
	daArrangement.SelectionSort();

	return (daArrangement.GetSize()>0);
}


/////////////////////////////////////////////////////////////////////////////////
//Component creation functions
/////////////////////////////////////////////////////////////////////////////////

Component * Bay::CreateComponentDumb(ComponentTypeEnum eComponentType, SideOfBayEnum eSide, int iRise, double dLength, MaterialTypeEnum eMaterialType)
{
	Component *pComponent;

	pComponent = new Component( dLength, eComponentType, eMaterialType, GetSystem() );
	pComponent->SetBayPointer( this );

	//Add the new component to the component list
	m_caComponents.AddComponent( eSide, iRise, pComponent );
	pComponent->CreatePointsIfPossible();

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//CreateComponent
//creates a component using the new operator, if you don't added to the
//	component list, then you will be responsible for destroying/removing it

Component *Bay::CreateComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
									int iRise, double dLength,
									MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	int			iCompID, i, iSize;
	Lift		*pLift;
	Component	*pComponent;
	
	pComponent = new Component( dLength, eComponentType, eMaterialType, GetSystem() );
	pComponent->SetBayPointer( this );

	//Add the new component to the component list
	iCompID = m_caComponents.AddComponent( eSide, iRise, pComponent );
	pComponent->CreatePointsIfPossible();

	///////////////////////////////////////////////////////////////////
	//Some components cannot exist whilst other of a type exist!
	switch( eComponentType )
	{
		case( CT_CHAIN_LINK ):		//fallthrough
		case( CT_SHADE_CLOTH ):		//fallthrough
		case( CT_BRACING ):
			//we can't have stageboards!
 			iSize = GetNumberOfLifts();
			for( i=0; i<iSize; i++ )
			{
				pLift = m_LiftList.GetLift(i);
				pLift->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
				
				switch( eSide )
				{
				case( NORTH ):
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NE );
					break;
				case( EAST ):
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ENE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SE );
					break;
				case( SOUTH ):
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SE );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SW );
					break;
				case( WEST ):
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WNW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SW );
					pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NW );
					break;
				default:
					assert( false );
					break;
				}
			}
			break;
		case( CT_STAGE_BOARD ):
			//this component type belongs to a lift, so we should not be here!
			assert( false );
			break;
		default:
			//Do nothing
			break;
	}

	switch( eComponentType )
	{
	case( CT_CHAIN_LINK ):
		GetTemplate()->SetChainMeshSide( eSide, true );
		break;
	case( CT_SHADE_CLOTH ):
		GetTemplate()->SetShadeClothSide( eSide, true );
		break;
	case( CT_TIE_TUBE ):
		switch( eSide )
		{
		case( ALL_SIDES ):
			GetTemplate()->SetNNETie( true );
			GetTemplate()->SetENETie( true );
			GetTemplate()->SetESETie( true );
			GetTemplate()->SetSSETie( true );
			GetTemplate()->SetSSWTie( true );
			GetTemplate()->SetWSWTie( true );
			GetTemplate()->SetWNWTie( true );
			GetTemplate()->SetNNWTie( true );
			break;
		case( ALL_VISIBLE_SIDES ):
			if( GetOuter()==NULL )
			{
				GetTemplate()->SetNNWTie( true );
				GetTemplate()->SetNNETie( true );
			}
			if( GetForward()==NULL )
			{
				GetTemplate()->SetENETie( true );
				GetTemplate()->SetESETie( true );
			}
			if( GetBackward()==NULL )
			{
				GetTemplate()->SetWSWTie( true );
				GetTemplate()->SetWNWTie( true );
			}
			if( GetInner()==NULL )
			{
				GetTemplate()->SetSSETie( true );
				GetTemplate()->SetSSWTie( true );
			}
			break;
		case( NNE ):
			GetTemplate()->SetNNETie( true );
			break;
		case( ENE ):
			GetTemplate()->SetENETie( true );
			break;
		case( ESE ):
			GetTemplate()->SetESETie( true );
			break;
		case( SSE ):
			GetTemplate()->SetSSETie( true );
			break;
		case( SSW ):
			GetTemplate()->SetSSWTie( true );
			break;
		case( WSW ):
			GetTemplate()->SetWSWTie( true );
			break;
		case( WNW ):
			GetTemplate()->SetWNWTie( true );
			break;
		case( NNW ):
			GetTemplate()->SetNNWTie( true );
			break;
		default:
			assert( false );
			break;
		}
		break;
	case( CT_BRACING ):
		switch( eSide )
		{
		case( NORTH ):
			GetTemplate()->SetNBrace( true );
			break;
		case( EAST ):
			GetTemplate()->SetEBrace( true );
			break;
		case( SOUTH ):
			GetTemplate()->SetSBrace( true );
			break;
		case( WEST ):
			GetTemplate()->SetWBrace( true );
			break;
		default:
			assert( false );
			break;
		}
		break;
	case( CT_STAIR ):	//fallthrough
	case( CT_LADDER ):	//fallthrough
	default:
		//do nothing
		break;
	}

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddAStandard
//Creates a standard and attaches it to the Bay components list
Component *Bay::AddAStandard( SideOfBayEnum eSide, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	CornerOfBayEnum	eCnr;
	int				iNumber;
	double			dHeight;
	Component		*pComponent;
	bool			bSoleboard;

	//JSB todo 991129 - should this be used?
	eMaterialType;

	//this must be attached to a star
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )	return NULL;
	if( HasBayComponentOfTypeOnSide( CT_STANDARD, eSide ) ) return NULL;

	//since we are adding to the bay, we must add to the top of the pole
	//	so we may as well create the whole pole again
	eCnr = SideOfBayAsCorner(eSide);
	dHeight = GetHeightOfStandards( eCnr );
	bSoleboard = (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSide ) );

	if( !CreateStandardsArrangment( dHeight+dLength, eCnr, bSoleboard ) )
		return NULL;

	//return a pointer to the top element of the pole
	iNumber = m_caComponents.GetNumberOfPosTypes( eSide, CT_STANDARD );
	pComponent = m_caComponents.GetComponent( eSide, iNumber, CT_STANDARD );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddAOpenEndStandard
//Creates a standard and attaches it to the Bay components list
Component *Bay::AddAOpenEndStandard( SideOfBayEnum eSide, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	CornerOfBayEnum	eCnr;
	int				iNumber;
	double			dHeight;
	Component		*pComponent;
	bool			bSoleboard;

	#ifndef ALLOW_OPEN_END_STANDARDS
	assert( false );
	#endif	//#ifndef ALLOW_OPEN_END_STANDARDS

	//JSB todo 991129 - should this be used?
	eMaterialType;

	dHeight = 0.00;
	//this must be attached to a star
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )	return NULL;
	if( HasBayComponentOfTypeOnSide( CT_STANDARD_OPENEND, eSide ) ) return NULL;

	//since we are adding to the bay, we must add to the top of the pole
	//	so we may as well create the whole pole again
	eCnr = SideOfBayAsCorner(eSide);
	dHeight = GetHeightOfStandards( eCnr );
	bSoleboard = (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSide ) );

	if( !CreateStandardsArrangment( dHeight+dLength, eCnr, bSoleboard ) )
		return NULL;

	//return a pointer to the top element of the pole
	iNumber = m_caComponents.GetNumberOfPosTypes( eSide, CT_STANDARD_OPENEND );
	pComponent = m_caComponents.GetComponent( eSide, iNumber, CT_STANDARD_OPENEND );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddAJack
//Creates a Jack and attaches it to the Bay components list
Component *Bay::AddAJack( SideOfBayEnum eSide,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Point3D			ptFirst, ptSecond;
	CornerOfBayEnum eCnr;
	Component		*pComponent;
	double			dHeight, dAngle;
	bool			bMoveX, bMoveY;
	Matrix3D		Transform, Rotation, Spin, Translation, Correction;
	Vector3D		Vector;

	////////////////////////////////////////////////////////////////
	//Standards and Jacks are vertical, so we need to rotate them
	Vector.set( 0.00, 1.00, 0.00 );
	dAngle = pi/-2.00;
	Rotation.setToRotation( dAngle, Vector );

	Vector.set( 0.00, 0.00, 1.00 );
	switch( SideOfBayAsCorner(eSide) )
	{
	case( CNR_NORTH_WEST ):	//fallthrough
	case( CNR_NORTH_EAST ):
		dAngle = pi;
		break;
	case( CNR_SOUTH_EAST ):	//fallthrough
	case( CNR_SOUTH_WEST ):
		dAngle = 0.00;
		break;
	default:
		assert( FALSE );
	}

	Spin.setToRotation( dAngle, Vector );
	Rotation = Spin * Rotation;

	////////////////////////////////////////////////////////////////
	dHeight = 0.00;
	//this must be attached to a stanard
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )
		return NULL;
	//there can only be one JACK
	if( HasBayComponentOfTypeOnSide( CT_JACK, eSide ) )
		return NULL;

	pComponent = CreateComponent( CT_JACK, eSide, 0, JACK_LENGTH_APPROX, eMaterialType );

	//does this require movement if the bay size is changed?
	switch( SideOfBayAsCorner(eSide) )
	{
	case( CNR_NORTH_EAST ):	bMoveX = true; bMoveY = true; break;
	case( CNR_SOUTH_EAST ):	bMoveX = true; bMoveY = false; break;
	case( CNR_NORTH_WEST ):	bMoveX = false; bMoveY = true; break;
	case( CNR_SOUTH_WEST ):	bMoveX = false; bMoveY = false; break;
	default:
		assert( FALSE );
	}

	//set the 3D location
	eCnr = SideOfBayAsCorner(eSide);
	ptFirst = GetStandardPosition(eCnr);
	ptSecond = ptFirst;
	//move the base of the jack down so that the
	//	top aligns with the bottom of the standard
	ptSecond.z-= pComponent->GetLengthActual();
	Vector.set( 0.00, 0.00, ptSecond.z );
	if( bMoveX )	Vector.x+= GetBayLengthActual();
	if( bMoveY )	Vector.y+= GetBayWidthActual();
	Translation.setTranslation( Vector );
	Transform = Translation * Rotation;
	Transform = Correction * Transform;
	pComponent->Move( Transform, true );
	Transform = GetTransform();
	Transform = GetRunPointer()->GetTransform() * Transform;
	Transform = GetController()->Get3DCrosshairTransform() * Transform;
	pComponent->Move( Transform, false );

	pComponent->SetMoveWhenBayLengthChanges( bMoveX );
	pComponent->SetMoveWhenBayWidthChanges( bMoveY );
	//location
	pComponent->AddNode( ptFirst );
	pComponent->AddNode( ptSecond );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddASoleboard
//Creates a Soleboard and attaches it to the Bay components list
Component *Bay::AddASoleboard( SideOfBayEnum eSide,
						 MaterialTypeEnum eMaterialType/*=MT_TIMBER*/ )
{
	bool			bMoveX, bMoveY;
	double			dAngle, dJackLength, dSoleboardLength;
	Point3D			ptFirst, ptSecond;
	Matrix3D		Transform, Trans, Spin;
	Vector3D		Vector;
	Component		*pComponent;
	CornerOfBayEnum eCnr;

	dSoleboardLength = COMPONENT_LENGTH_0500;

/*	if( GetController()->GetUse1500Soleboards() &&
				GetBayWidth()==COMPONENT_LENGTH_1200 &&
				GetInner()==NULL && GetOuter()==NULL )
	{
		//This is a soleboard for a 1200 wide bay, 
		//	and we are trying to use 1.5m soleboards
		//	plus there are no N/S neighbors
		bool			bRLsSame;
		double			dRL[2];

		bRLsSame = false;
		if( eSide==NE || eSide==SE )
		{
			//We should check if there is a soleboard on the other side
			//if( (eSide==NE && (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SE ) || m_bRequiresSoleBoard[CNR_SOUTH_EAST] ) ) ||
			//	(eSide==SE && (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NE ) || m_bRequiresSoleBoard[CNR_NORTH_EAST] ) ) )
			{
				dRL[0] = GetStandardPosition(CNR_NORTH_EAST).z;
				dRL[1] = GetStandardPosition(CNR_SOUTH_EAST).z;
				if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR )
					bRLsSame = true;
			}
		}
		if( eSide==NW || eSide==SW )
		{
			//We should check if there is a soleboard on the other side
			//if( (eSide==NW && (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SW ) || m_bRequiresSoleBoard[CNR_SOUTH_WEST] ) ) ||
			//	(eSide==SW && (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NW ) || m_bRequiresSoleBoard[CNR_NORTH_WEST] ) ) )
			{
				dRL[0] = GetStandardPosition(CNR_NORTH_WEST).z;
				dRL[1] = GetStandardPosition(CNR_SOUTH_WEST).z;
				if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR )
					bRLsSame = true;
			}
		}

		if( bRLsSame )
		{
			//The RL's are the same between NORTH/SOUTH standard sets
			if( eSide==SW || eSide==SE )
			{
				//change the component lenght to 1.5m
				dSoleboardLength = COMPONENT_LENGTH_1500;
			}
			else
			{
				//These must be the northern side, so
				//	make sure we don't add a component
				return NULL;
			}
		}
	}
*/

	//This must be attached to a star
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )		return NULL;
	if( HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSide ) )	return NULL;

	pComponent = CreateComponent( CT_SOLEBOARD, eSide, 0, dSoleboardLength, eMaterialType );

	//Centre around z axis
	eCnr = SideOfBayAsCorner(eSide);
	//How long is the jack?
	dJackLength = GetCompDetails()->GetActualLength( GetSystem(), CT_JACK, JACK_LENGTH_APPROX, MT_STEEL, false );
	//Position Soleboard just below Jack
	Vector.set( 0.00-(pComponent->GetLengthActual()/2.00), 0.00, 0.00-dJackLength-(pComponent->GetHeightActual()/2.00) );
	Transform.setTranslation( Vector );

	//Spin around z axis
	Vector.set( 0.00, 0.00, 1.00 );
	switch( SideOfBayAsCorner(eSide) )
	{
	case( CNR_NORTH_WEST ):	//fallthrough
	case( CNR_NORTH_EAST ):
		dAngle = pi;
		break;
	case( CNR_SOUTH_EAST ):	//fallthrough
	case( CNR_SOUTH_WEST ):
		dAngle = 0.00;
		break;
	default:
		assert( FALSE );
	}
	Spin.setToRotation( dAngle, Vector );
	Transform = Spin * Transform;

	//does this require movement if the bay size is changed?
	switch( SideOfBayAsCorner(eSide) )
	{
	case( CNR_NORTH_EAST ):	bMoveX = true; bMoveY = true; break;
	case( CNR_SOUTH_EAST ):	bMoveX = true; bMoveY = false; break;
	case( CNR_NORTH_WEST ):	bMoveX = false; bMoveY = true; break;
	case( CNR_SOUTH_WEST ):	bMoveX = false; bMoveY = false; break;
	default:
		assert( FALSE );
	}

	//Move to position in bay
	ptFirst = GetStandardPosition(eCnr);
	ptSecond = ptFirst;

	//Position the board around the bay, consider only the x/y plane
	Vector.set( 0.00, 0.00, ptFirst.z);
	if( bMoveX )	Vector.x+= GetBayLengthActual();
	if( bMoveY )	Vector.y+= GetBayWidthActual();
	Trans.setTranslation( Vector );
	Transform = Trans * Transform;

	pComponent->Move( Transform, true );

	Transform = GetTransform();
	Transform = GetRunPointer()->GetTransform() * Transform;
	Transform = GetController()->Get3DCrosshairTransform() * Transform;
	pComponent->Move( Transform, false );

	pComponent->SetMoveWhenBayLengthChanges( bMoveX );
	pComponent->SetMoveWhenBayWidthChanges( bMoveY );
	//location
	pComponent->AddNode( ptFirst );
	pComponent->AddNode( ptSecond );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddAChainLink
//Creates a Chainlink and attaches it to the Bay components list
Component *Bay::AddAChainLink( SideOfBayEnum eSide )
{
	int			iRise;
	double		dLength, dAngle;
	Point3D		ptFirst, ptSecond, ptOther;
	Vector3D	Vector;
	Matrix3D	Transform, Rotation;
	Component	*pComponent;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) ) return NULL;

	//chek to see if there is a bay attached to this side
	if( (eSide==NORTH)	&& (GetOuter()!=NULL) )		return NULL;
	if( (eSide==EAST)	&& (GetForward()!=NULL) )	return NULL;
	if( (eSide==SOUTH)	&& (GetInner()!=NULL) )		return NULL;
	if( (eSide==WEST)	&& (GetBackward()!=NULL) )	return NULL;

	//for shade clothe, we will use the Length to store the area since Length
	//	has no meaning
//	dLength = GetFaceWidth( eSide );
	dLength = 0.00;
	iRise = (int)GetFaceHeight( eSide );

	if( !GetUseMidrailWithChainMesh() )
		DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );

	pComponent = CreateComponent( CT_CHAIN_LINK, eSide, iRise, dLength, MT_STEEL );

	//We want the Shadecloth to sit in the middle of the bracing
	double dStdWidth = GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );

	//The x,y values of the points are set to the x,y values of the standars
	switch( eSide )
	{
		case( NORTH ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_NORTH_WEST);
			dAngle		= pi;
			Vector.set( ptFirst.x, ptFirst.y, 0.00 );
			Vector.y+= dStdWidth;
			break;
		case( SOUTH ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			dAngle		= 0.00;
			Vector.set( ptSecond.x, ptSecond.y, 0.00 );
			Vector.y-= dStdWidth;
			break;
		case( EAST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_EAST);
			Vector.set( ptFirst.x, ptFirst.y, 0.00 );
			dAngle		= pi/-2.00;
			Vector.x+= dStdWidth;
			break;
		case( WEST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_WEST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			dAngle		= pi/2.00;
			Vector.set( ptSecond.x, ptSecond.y, 0.00 );
			Vector.x-= dStdWidth;
			break;
		default:
			//what the hell happend to eSide
			assert(FALSE);
			break;
	}
	//The z value for the first point is the higher of the two standards
	ptFirst.z = max( ptFirst.z, ptSecond.z );
	Vector.z = ConvertRLtoStarRL( ptFirst.z+(((double)iRise-ptFirst.z)/2.00) , GetStarSeparation());

	//The second point is the top handrail or top of the bay, on the other standard
	Transform.setToTranslation( Vector );
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( dAngle, Vector );
	Transform = Transform * Rotation;

	pComponent->Move( Transform, true );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddAShadeCloth
//Creates a shade cloth and attaches it to the Bay components list
Component *Bay::AddAShadeCloth( SideOfBayEnum eSide )
{
	int			iRise;
	double		dLength, dAngle;
	Point3D		ptFirst, ptSecond, ptOther;
	Vector3D	Vector;
	Matrix3D	Transform, Rotation;
	Component	*pComponent;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) ) return NULL;

	//check to see if there is a bay attached to this side
	if( (eSide==NORTH)	&& (GetOuter()!=NULL) )		return NULL;
	if( (eSide==EAST)	&& (GetForward()!=NULL) )	return NULL;
	if( (eSide==SOUTH)	&& (GetInner()!=NULL) )		return NULL;
	if( (eSide==WEST)	&& (GetBackward()!=NULL) )	return NULL;

	//for shade clothe, we will use the Length to store the area since Length
	//	has no meaning
//	dLength = GetFaceWidth( eSide );
	dLength = 0.00;
	iRise = (int)GetFaceHeight( eSide );

	pComponent = CreateComponent( CT_SHADE_CLOTH, eSide, iRise, dLength, MT_OTHER );

	//We want the Shadecloth to sit in the middle of the bracing
	double dStdWidth = GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );

	//The x,y values of the points are set to the x,y values of the standards
	switch( eSide )
	{
		case( NORTH ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_NORTH_WEST);
			dAngle		= pi;
			Vector.set( ptFirst.x, ptFirst.y, 0.00 );
			Vector.y+= dStdWidth;
			break;
		case( SOUTH ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			dAngle		= 0.00;
			Vector.set( ptSecond.x, ptSecond.y, 0.00 );
			Vector.y-= dStdWidth;
			break;
		case( EAST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_EAST);
			dAngle		= pi/-2.00;
			Vector.set( ptFirst.x, ptFirst.y, 0.00 );
			Vector.x+= dStdWidth;
			break;
		case( WEST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_WEST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			dAngle		= pi/2.00;
			Vector.set( ptSecond.x, ptSecond.y, 0.00 );
			Vector.x-= dStdWidth;
			break;
		default:
			//what the hell happend to eSide
			assert(FALSE);
			break;
	}
	//The z value for the first point is the higher of the two standards
	ptFirst.z = max( ptFirst.z, ptSecond.z );
	Vector.z = ConvertRLtoStarRL( ptFirst.z+(((double)iRise-ptFirst.z)/2.00) , GetStarSeparation());

	//The second point is the top handrail or top of the bay, on the other standard
	Transform.setToTranslation( Vector );
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( dAngle, Vector );
	Transform = Transform * Rotation;

	pComponent->Move( Transform, true );

	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddATie
//Creates a Tie, two tie clamps and a clamp and attaches them to the Bay components list
Component *Bay::AddATie( SideOfBayEnum eSide, double dHeight, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	bool		bMoveX, bMoveY;
	Point3D		ptFirst, ptSecond, ptThird;
	double		dDistance, dX, dY;
	Component	*pTie, *pTieClamp, *pTie90deg1, *pTie90deg2;
	Vector3D	Vector;
	Matrix3D	VertTransform, Rotation, ClampTrans, Trans90deg1, Trans, TubeTrans,
				Trans90deg2, BayRotation, BayTransform, Spin, OffsetFromStd;

	//check if there is any need for a tie here
	switch( eSide )
	{
		case( NORTH_NORTH_EAST ):
		case( NORTH_NORTH_WEST ):
			if( GetOuter()!=NULL )		return NULL;
			break;
		case( EAST_NORTH_EAST ):
		case( EAST_SOUTH_EAST ):
			if( GetForward()!=NULL )	return NULL;
			break;
		case( SOUTH_SOUTH_EAST ):
		case( SOUTH_SOUTH_WEST ):
			if( GetInner()!=NULL )		return NULL;
			break;
		case( WEST_SOUTH_WEST ):
		case( WEST_NORTH_WEST ):
			if( GetBackward()!=NULL )	return NULL;
			break;
		default:
			//what the fuck happend here!
			assert( FALSE );
			return NULL;
	}

	//Set the x and the y values for the component
	switch( eSide )
	{
		case( NORTH_NORTH_EAST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_EAST);
			break;
		case( EAST_NORTH_EAST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_EAST);
			ptSecond	= GetStandardPosition(CNR_NORTH_WEST);
			break;
		case( EAST_SOUTH_EAST ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_EAST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			break;
		case( SOUTH_SOUTH_EAST ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_EAST);
			ptSecond	= GetStandardPosition(CNR_NORTH_EAST);
			break;
		case( SOUTH_SOUTH_WEST ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_WEST);
			ptSecond	= GetStandardPosition(CNR_NORTH_WEST);
			break;
		case( WEST_SOUTH_WEST ):
			ptFirst		= GetStandardPosition(CNR_SOUTH_WEST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_EAST);
			break;
		case( WEST_NORTH_WEST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_WEST);
			ptSecond	= GetStandardPosition(CNR_NORTH_EAST);
			break;
		case( NORTH_NORTH_WEST ):
			ptFirst		= GetStandardPosition(CNR_NORTH_WEST);
			ptSecond	= GetStandardPosition(CNR_SOUTH_WEST);
			break;
		default:
			//what the fuck happend here!
			assert( FALSE );
			return NULL;
	}
	ptFirst.z	= ConvertHeightToRL( dHeight );
	ptSecond.z	= ptFirst.z;

	//The third point should form a stright line 
	dDistance = ptFirst.distanceTo( ptSecond );
	if( dDistance<=0.00 )
	{
		//Huston, we have a problem
		assert(FALSE);
		return NULL;
	}
	if( dDistance>dLength )
	{
		//tube not long enought to extend past the bay and be attached to both standards
		assert(FALSE);
		return NULL;
	}

	//The third point should be dLength away from the second point
	//	in the same direction as the second point
	dX = ptSecond.x-ptFirst.x;
	dY = ptSecond.y-ptFirst.y;
	ptThird.x = ( (dLength/dDistance) * dX ) + ptFirst.x;
	ptThird.y = ( (dLength/dDistance) * dY ) + ptFirst.y;
	
	//create the tie, 2 90degree clamps and a Tie clamp
	pTie		= CreateComponent( CT_TIE_TUBE,				eSide, 0, dLength,	eMaterialType );
	switch( GetRunPointer()->GetTieType() )
	{
	case( TIE_TYPE_COLUMN ):
		pTieClamp	= CreateComponent( CT_TIE_CLAMP_COLUMN,		eSide, 0, 0.00,		eMaterialType );
		break;
	case( TIE_TYPE_MASONARY ):
		pTieClamp	= CreateComponent( CT_TIE_CLAMP_MASONARY,	eSide, 0, 0.00,		eMaterialType );
		break;
	case( TIE_TYPE_YOKE ):
		pTieClamp	= CreateComponent( CT_TIE_CLAMP_YOKE,		eSide, 0, 0.00,		eMaterialType );
		break;
	case( TIE_TYPE_BUTTRESS_12 ):	//fallthrough
	case( TIE_TYPE_BUTTRESS_18 ):	//fallthrough
	case( TIE_TYPE_BUTTRESS_24 ):	//fallthrough
	default:
		assert( false );
		break;
	}
	pTie90deg1	= CreateComponent( CT_TIE_CLAMP_90DEGREE,	eSide, 0, 0.00,		eMaterialType );
	pTie90deg2	= CreateComponent( CT_TIE_CLAMP_90DEGREE,	eSide, 0, 0.00,		eMaterialType );

	double	dWidthOfTube, dLength90deg, dStandardWidth;
	dWidthOfTube = pTie->GetWidthActual();
	dLength90deg = pTie90deg1->GetLengthActual();
	dStandardWidth = GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );

	//Rotate all the clamps side on to the tube
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( pi/2, Vector );
	ClampTrans	= Rotation;
	Trans90deg1	= Rotation;
	Trans90deg2	= Rotation;

	//Move the Wall clamp to the end of the pole, and position it just above the standard in the y
	Vector.set( dLength-(pTieClamp->GetWidthActual()/2.00), (dStandardWidth/2.00), 0.00 );
	Trans.setToTranslation( Vector );
	ClampTrans	= Trans * ClampTrans;

	//Move the clamps & pole just north of the standard
	Vector.set( 0.00, (dStandardWidth/2.00), 0.00 );
	Trans.setToTranslation( Vector );
	Trans90deg1 = Trans * Trans90deg1;
	TubeTrans	= Trans * TubeTrans;

	//Move the 2nd clamp just north of the standard and along to its position on the pole
	Vector.set( dDistance, (dStandardWidth/2.00), 0.00 );
	Trans.setToTranslation( Vector );
	Trans90deg2 = Trans * Trans90deg2;

	//Move all components down till the north side of the 90deg clamps
	//	meet the north side of the standards
	Vector.set( 0.00, 0.00-dLength90deg, 0.00 );
	Trans.setToTranslation( Vector );
	ClampTrans	= Trans * ClampTrans;
	Trans90deg1 = Trans * Trans90deg1;
	Trans90deg2 = Trans * Trans90deg2;
	TubeTrans	= Trans * TubeTrans;

	//Rotation within bay
	Vector.set( 0.00, 0.00, 1.00 );
	BayRotation.setToIdentity();
	bMoveX = false;
	bMoveY = false;
	bool bSwapSide;
	bSwapSide = false;
	switch( eSide )
	{
		case( NNW ):	//fallthrough
			BayRotation.setToRotation( pi/2.00, Vector );
			break;
		case( NNE ):
			bSwapSide = true;
			bMoveX = true;
			BayRotation.setToRotation( pi/2.00, Vector );
			break;
		case( ENE ):	//fallthrough
			bMoveY = true;
			break;
		case( ESE ):
			bSwapSide = true;
			break;
		case( SSE ):	//fallthrough
			bMoveX = true;
			bMoveY = true;
			BayRotation.setToRotation( pi/-2.00, Vector );
			break;
		case( SSW ):
			bSwapSide = true;
			bMoveY = true;
			BayRotation.setToRotation( pi/-2.00, Vector );
			break;
		case( WSW ):	//fallthrough
			bMoveX = true;
			BayRotation.setToRotation( pi, Vector );
			break;
		case( WNW ):
			bSwapSide = true;
			bMoveX = true;
			bMoveY = true;
			BayRotation.setToRotation( pi, Vector );
			break;
		default:
			assert( false );
			break;
	}

	//for some of the tubes we need to swap the side the tube is attached to
	//	so we will simply turn the whole assembly upside down!
	Matrix3D	SwapSide;
	SwapSide.setToIdentity();
	if( bSwapSide )
	{
		Vector.set( 1.00, 0.00, 0.00 );
		SwapSide.setToRotation( pi, Vector );
	}
	ClampTrans	= SwapSide * ClampTrans;
	Trans90deg1 = SwapSide * Trans90deg1;
	Trans90deg2 = SwapSide * Trans90deg2;
	TubeTrans	= SwapSide * TubeTrans;

	//Above we also calcuated the rotation reqd to position the tube on the bay
	ClampTrans	= BayRotation * ClampTrans;
	Trans90deg1 = BayRotation * Trans90deg1;
	Trans90deg2 = BayRotation * Trans90deg2;
	TubeTrans	= BayRotation * TubeTrans;
	
	//Vertical transformation applies to all components equally
	//	We need to find the closest lift, below the first point
	Lift	*pLift;
	double dDistUnderLift = 0.00;
	double dVertical = ptFirst.z;
	dDistUnderLift = 2.00*GetCompDetails()->GetActualHeight( GetSystem(), CT_LEDGER, COMPONENT_LENGTH_2400, MT_STEEL, false );
	for( int i=GetNumberOfLifts()-1; i>=0; i-- )
	{
		pLift = GetLift(i);
		if( pLift->GetRL()-dDistUnderLift<ptFirst.z )
		{
			//This lift should be fine
			dVertical = pLift->GetRL()-dDistUnderLift;
			break;
		}
	}
	Vector.set( 0.00, 0.00, dVertical );
	VertTransform.setToTranslation( Vector );
	ClampTrans	= VertTransform * ClampTrans;
	Trans90deg1 = VertTransform * Trans90deg1;
	Trans90deg2 = VertTransform * Trans90deg2;
	TubeTrans	= VertTransform * TubeTrans;

	//Translation within bay
	Vector.set( 0.00, 0.00, 0.00 );
	if( bMoveX )	Vector.x+= GetBayLengthActual();
	if( bMoveY )	Vector.y+= GetBayWidthActual();
	BayTransform.setToTranslation( Vector );
	ClampTrans	= BayTransform * ClampTrans;
	Trans90deg1 = BayTransform * Trans90deg1;
	Trans90deg2 = BayTransform * Trans90deg2;
	TubeTrans	= BayTransform * TubeTrans;

	//Move component
	pTie->Move( TubeTrans, true );
	pTie->SetMoveWhenBayLengthChanges( bMoveX );
	pTie->SetMoveWhenBayWidthChanges( bMoveY );

	pTieClamp->Move( ClampTrans, true );
	pTieClamp->SetMoveWhenBayLengthChanges( bMoveX );
	//pTieClamp->SetMoveWhenBayWidthChanges( bMoveY );

	pTie90deg1->Move( Trans90deg1, true );
	pTie90deg1->SetMoveWhenBayLengthChanges( bMoveX );
	pTie90deg1->SetMoveWhenBayWidthChanges( bMoveY );

	pTie90deg2->Move( Trans90deg2, true );
	pTie90deg2->SetMoveWhenBayLengthChanges( bMoveX );
	//pTie90deg2->SetMoveWhenBayWidthChanges( bMoveY );

	return pTie;
}

/////////////////////////////////////////////////////////////////////////////////
//AddATieClamp90deg
//Creates a Tie clamp 90 degrees and attaches it to the Bay components list
Component *Bay::AddATieClamp90deg( SideOfBayEnum eSide, double dHeight,
									MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Component		*pComponent;
	Point3D			ptPoint, ptFirst;
	SideOfBayEnum	eSideA, eSideB;

	//this must be attached to a star
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )	return NULL;

	//we need a tie tube on this standard
	switch( eSide )
	{
		case( NORTH_EAST ):
			eSideA = NORTH_NORTH_EAST;
			eSideB = EAST_NORTH_EAST;
			ptFirst= GetStandardPosition(CNR_NORTH_WEST);
			break;
		case( NORTH_WEST ):
			eSideA = NORTH_NORTH_WEST;
			eSideB = WEST_NORTH_WEST;
			ptFirst= GetStandardPosition(CNR_NORTH_WEST);
			break;
		case( SOUTH_EAST ):
			eSideA = EAST_SOUTH_EAST;
			eSideB = SOUTH_SOUTH_EAST;
			ptFirst= GetStandardPosition(CNR_SOUTH_EAST);
			break;
		case( SOUTH_WEST ):
			eSideA = SOUTH_SOUTH_WEST;
			eSideB = WEST_SOUTH_WEST;
			ptFirst= GetStandardPosition(CNR_SOUTH_WEST);
			break;
	}
	if( GetComponent( CT_TIE_TUBE, eSideA, dHeight )!=NULL ||
		GetComponent( CT_TIE_TUBE, eSideB, dHeight )!=NULL ) return NULL;

	pComponent = CreateComponent( CT_TIE_TUBE, eSide, 0, 0.00, eMaterialType );
	pComponent->SetPoints( ptFirst, ptFirst, ptFirst );
	return pComponent;
}

/////////////////////////////////////////////////////////////////////////////////
//AddATieClampColumn
//Creates a clamp and attaches it to the Bay components list
Component *Bay::AddATieClampColumn( SideOfBayEnum eSide, double dHeight, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Point3D		ptPoint;
	Component	*pTie, *pTieClamp;

	dLength;

	pTie = GetComponent( CT_TIE_TUBE, eSide, dHeight );
	if( pTie!=NULL ) return NULL;
	//This must come off a side of a star
	if( (eSide<NORTH_NORTH_EAST) || (eSide>NORTH_NORTH_WEST) )	return NULL;

	//the tie clamp is the 3rd node on the tie tube
	ptPoint = pTie->GetNode( 2 );

	pTieClamp = CreateComponent( CT_TIE_CLAMP_COLUMN, eSide, 0, 0.00, eMaterialType );
	pTieClamp->SetPoints( ptPoint, ptPoint, ptPoint );

	return pTie;
}

/////////////////////////////////////////////////////////////////////////////////
//AddATieClampMasonary
//Creates a clamp and attaches it to the Bay components list
Component *Bay::AddATieClampMasonary( SideOfBayEnum eSide, double dHeight, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Point3D		ptPoint;
	Component	*pTie, *pTieClamp;

	dLength;

	pTie = GetComponent( CT_TIE_TUBE, eSide, dHeight );
	if( pTie!=NULL ) return NULL;
	//This must come off a side of a star
	if( (eSide<NORTH_NORTH_EAST) || (eSide>NORTH_NORTH_WEST) )	return NULL;

	//the tie clamp is the 3rd node on the tie tube
	ptPoint = pTie->GetNode( 2 );

	pTieClamp = CreateComponent( CT_TIE_CLAMP_MASONARY, eSide, 0, 0.00, eMaterialType );
	pTieClamp->SetPoints( ptPoint, ptPoint, ptPoint );

	return pTie;
}

/////////////////////////////////////////////////////////////////////////////////
//AddATieClamp
//Creates a clamp and attaches it to the Bay components list
Component *Bay::AddATieClampYoke( SideOfBayEnum eSide, double dHeight, double dLength,
						 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Point3D		ptPoint;
	Component	*pTie, *pTieClamp;

	dLength;

	pTie = GetComponent( CT_TIE_TUBE, eSide, dHeight );
	if( pTie!=NULL ) return NULL;
	//This must come off a side of a star
	if( (eSide<NORTH_NORTH_EAST) || (eSide>NORTH_NORTH_WEST) )	return NULL;

	//the tie clamp is the 3rd node on the tie tube
	ptPoint = pTie->GetNode( 2 );

	pTieClamp = CreateComponent( CT_TIE_CLAMP_YOKE, eSide, 0, 0.00, eMaterialType );
	pTieClamp->SetPoints( ptPoint, ptPoint, ptPoint );

	return pTie;
}

/////////////////////////////////////////////////////////////////////////////////
//AddALedger
//Creates a Ledger and attaches it to the correct lift
Component *Bay::AddALedger( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	LiftRiseEnum	eRise;
	Lift			*pLift;
	double			dLiftHeight;

	if( (eSide!=NORTH) || (eSide!=SOUTH) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//Calc the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	return pLift->AddComponent( CT_LEDGER, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddATransom
//Creates a Transom and attaches it to the correct lift
Component *Bay::AddATransom( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	if( (eSide!=EAST) || (eSide!=WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	return pLift->AddComponent( CT_TRANSOM, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddAMeshguard
//Creates a Mesh guard and attaches it to the correct lift
Component *Bay::AddAMeshguard( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	//the meshguard can only be added at 1000 or 2000
	if( (eRise!=LIFT_RISE_2000MM) || (eRise!=LIFT_RISE_1000MM) )
		return NULL;

	//Meshguard doesn't need some items;
	pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
	pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );

	//cannot be less then 1200
	return pLift->AddComponent( CT_MESH_GUARD, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddARail
//Creates a Hand rail and attaches it to the correct lift
Component *Bay::AddARail( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	//The rail can only be added at 1000
	if( eRise!=LIFT_RISE_1000MM )
		return NULL;

	return pLift->AddComponent( CT_RAIL, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddAMidrail
//Creates a Mid rail and attaches it to the correct lift
Component *Bay::AddAMidrail( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	//the midrail can only be added at GetStarSeparation()
	if( eRise!=LIFT_RISE_0500MM )
		return NULL;

	return pLift->AddComponent( CT_MID_RAIL, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddAToeBoard
//Creates a Toe board and attaches it to the correct lift
Component *Bay::AddAToeBoard( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	//This must run between two standards
	if( (eSide<NORTH) || (eSide>WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	//the toeboard can only be added at the bottom of the lift
	if( eRise!=LIFT_RISE_0000MM )
		return NULL;

	return pLift->AddComponent( CT_TOE_BOARD, dLength, eSide, eRise, eMaterialType );
}

/////////////////////////////////////////////////////////////////////////////////
//AddAHopUpBracket
//Creates a Hop Up bracket and attaches it to the correct lift
Component *Bay::AddAHopUpBracket( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	////////////////////////////////////////////////////////////////
	//This must come off one side of a star
	if( (eSide<NORTH_NORTH_EAST) || (eSide>NORTH_NORTH_WEST) )	return NULL;


	switch( eSide )
	{
	case( NORTH_NORTH_EAST ):
		assert( GetOuter()==NULL );
		break;
	case( EAST_NORTH_EAST ):
		assert( GetForward()==NULL );
		break;
	case( EAST_SOUTH_EAST ):
		assert( GetForward()==NULL );
		if( GetInner()!=NULL )
			return NULL;
		break;
	case( SOUTH_SOUTH_EAST ):
		assert( GetInner()==NULL );
		break;
	case( SOUTH_SOUTH_WEST ):
		assert( GetInner()==NULL );
		if( GetBackward()!=NULL )
			return NULL;
		break;
	case( WEST_SOUTH_WEST ):
		assert( GetBackward()==NULL );
		if( GetInner()!=NULL )
			return NULL;
		break;
	case( WEST_NORTH_WEST ):
		assert( GetBackward()==NULL );
		break;
	case( NORTH_NORTH_WEST ):
		assert( GetOuter()==NULL );
		if( GetBackward()!=NULL )
			return NULL;
		break;
	default:
		assert( false );
		return NULL;
	}

	pLift = GetLiftAtHeight( dHeight );

	////////////////////////////////////////////////////////////////
	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	////////////////////////////////////////////////////////////////
	//the Hopup can only be added at the bottom of the lift
	if( eRise!=LIFT_RISE_0000MM )
		return NULL;

	AddStageBoardCheck( eSide );

	return pLift->AddComponent( CT_HOPUP_BRACKET, dLength, eSide, eRise, eMaterialType );
}


void Bay::AddStageBoardCheck(SideOfBayEnum eSide, bool ShowWarning/*=true*/ )
{
	int				iCount;
	CString			sTemp, sMsg;
	SideOfBayEnum	eCompSide;

	////////////////////////////////////////////////////////////////
	//We are adding a stage board, so we will have to Delete any existing
	//	Bracing, Chain mesh, or Shadecloth
	switch( eSide )
	{
	case( N ):		//fallthrough
	case( NNW ):	//fallthrough
	case( NNE ):
		eCompSide = NORTH;
		break;
	case( E ):		//fallthrough
	case( ENE ):	//fallthrough
	case( ESE ):
		eCompSide = EAST;
		break;
	case( S ):		//fallthrough
	case( SSE ):	//fallthrough
	case( SSW ):
		eCompSide = SOUTH;
		break;
	case( W ):		//fallthrough
	case( WSW ):	//fallthrough
	case( WNW ):
		eCompSide = WEST;
		break;
	default:
		assert( false );
	}

	if( ShowWarning )
	{
		sTemp.Empty();
		iCount=0;
		if( DoesABayComponentExistOnASide( CT_BRACING, eCompSide ) )
		{
			sTemp = GetComponentDescStr( CT_BRACING );
			iCount++;
		}
		if( DoesABayComponentExistOnASide( CT_SHADE_CLOTH, eCompSide ) )
		{
			if( !DoesABayComponentExistOnASide( CT_CHAIN_LINK, eCompSide ) )
			{
				sTemp+= _T(", ");
			}
			else
			{
				sTemp+= _T(" & ");
			}
			sTemp = GetComponentDescStr( CT_SHADE_CLOTH );
			iCount++;
		}
		if( DoesABayComponentExistOnASide( CT_CHAIN_LINK, eCompSide ) )
		{
			if( !sTemp.IsEmpty() )
			{
				sTemp+= _T(" & ");
			}
			sTemp = GetComponentDescStr( CT_CHAIN_LINK );
			iCount++;
		}
		if( iCount>0 )
		{
			assert( !sTemp.IsEmpty() );
			//Display a warning box first!
			sMsg.Format( _T("Placing a Stage Board on the %s side of bay%i,\nwill Delete the existing %s.\n"),
				GetSideOfBayDescStr( eCompSide ), GetBayNumber(), sTemp );
			if( iCount>1 )
				sMsg+= _T("Do you wish to Delete these components?");
			else
				sMsg+= _T("Do you wish to Delete this component");

			MessageBeep(MB_ICONQUESTION);
			if( MessageBox( NULL, sMsg, _T("Warning"), MB_YESNO|MB_ICONQUESTION )!=IDYES )
			{
				return;
			}
		}
	}

	DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eCompSide );
	DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, eCompSide );
	DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, eCompSide );
}

/////////////////////////////////////////////////////////////////////////////////
//AddACornerStageBoard
//Creates a Corner stage board and attaches it to the correct lift
Component *Bay::AddACornerStageBoard( SideOfBayEnum eSide, double dLength, double dHeight,
							 MaterialTypeEnum eMaterialType/*=MT_STEEL*/ )
{
	Lift			*pLift;
	double			dLiftHeight;
	LiftRiseEnum	eRise;

	//this must be attached to a star
	if( (eSide<NORTH_EAST) || (eSide>SOUTH_WEST) )	return NULL;

	pLift = GetLiftAtHeight( dHeight );

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	//the Hopup bo can only be added at the bottom of the lift
	if( eRise!=LIFT_RISE_0000MM )
		return NULL;

	//the length for the corner stage boards must be one of the following sizes
	if( dLength!=CNR_STAGE_BOARD_RISE_1X1	&&
		dLength!=CNR_STAGE_BOARD_RISE_2X2	&&
		dLength!=CNR_STAGE_BOARD_RISE_3X3	&&
		dLength!=CNR_STAGE_BOARD_RISE_1X2L	&&
		dLength!=CNR_STAGE_BOARD_RISE_1X3L	&&
		dLength!=CNR_STAGE_BOARD_RISE_2X3L	&&
		dLength!=CNR_STAGE_BOARD_RISE_1X2R	&&
		dLength!=CNR_STAGE_BOARD_RISE_1X3R	&&
		dLength!=CNR_STAGE_BOARD_RISE_2X3R	) return NULL;

	pLift->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, eSide );

	return pLift->AddComponent( CT_CORNER_STAGE_BOARD, dLength, eSide, eRise, eMaterialType );
}

bool Bay::AddStage( int iLiftID, SideOfBayEnum eSide, int iNumberOfPlanks, MaterialTypeEnum eMaterail )
{
	Lift	*pLift;

	pLift = GetLift( iLiftID );
	if( pLift!=NULL )
	{
		return pLift->AddStage( eSide, iNumberOfPlanks, eMaterail );
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////
//Component location functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//GetAStandard
//retrieve a pointer to a standard
Component *Bay::GetAStandard( SideOfBayEnum eSide, double dHeight  ) const
{
	//need to find out which number standard is at this height
	int i;
	doubleArray daArrangement;
	Component	*pComponent;
	GetStandardsArrangement( daArrangement, SideOfBayAsCorner(eSide) );
	if( dHeight<0.00 ) return NULL;
	i=0;
	while( i<daArrangement.GetSize() && dHeight>=0.00 )
	{
		dHeight-= daArrangement[i];
		i++;
	}
	//we have added an extra one to i
	i--;
	assert( i>=0 );	//We must have hit the i++ value

	pComponent = m_caComponents.GetComponent( eSide, i, CT_STANDARD );
	if( pComponent!=NULL )
		return pComponent;
	return m_caComponents.GetComponent( eSide, i, CT_STANDARD_OPENEND );
}

/////////////////////////////////////////////////////////////////////////////////
//GetABayVerticalComponent
//retrieve a pointer to a component which is vertically spaced along the bay
Component *Bay::GetABayVerticalComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight  ) const
{
	int			k;
	Component	*pComponent;

	//find the RL value of this Height
	dHeight = ConvertHeightToRL( dHeight );

	//go through each component and see if it is at the same height
	for( k=0; k<GetNumberOfBayComponents(); k++ )
	{
		pComponent = m_caComponents.GetComponent( k );
		if( pComponent==NULL )
		{
			assert( false );
		}

		if( eSide == m_caComponents.GetPosition( k ) &&
			eComponentType	== pComponent->GetType() &&
			pComponent->GetPoint( CP_FIRST_POINT ).z >= dHeight &&
			pComponent->GetPoint( CP_SECOND_POINT ).z <= dHeight )
		{
			return pComponent;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//GetABayHorizontalComponent
//retrieve a pointer to a component which has no vertical spread
Component *Bay::GetABayHorizontalComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight ) const
{
	int			k;
	Component	*pComponent;

	//find the RL value of this Height
	dHeight = ConvertHeightToRL( dHeight );

	//go through each component and see if it is at the same height
	for( k=0; k<GetNumberOfBayComponents(); k++ )
	{
		pComponent = m_caComponents.GetComponent( k );
		if( pComponent==NULL )
		{
			assert( false );
		}

		if( eSide == m_caComponents.GetPosition( k ) &&
			eComponentType	== pComponent->GetType() &&
			pComponent->GetPoint( CP_FIRST_POINT ).z == dHeight )
		{
			return pComponent;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//GetALiftComponent
//retrieve a pointer to a component which is owned by a lift
Component *Bay::GetALiftComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight  ) const
{
	double			dLiftHeight;
	Lift			*pLift;
	LiftRiseEnum	eRise;

	pLift = GetLiftAtHeight( dHeight );
	if( pLift==NULL ) return NULL;

	//get the height relative to the lift
	if( !GetLiftHeight( pLift->GetLiftID(), dLiftHeight ) )	return NULL;
	eRise = GetRiseEnumFromRise( dHeight-dLiftHeight );

	return pLift->GetComponent( eComponentType, eRise, eSide );
}

/////////////////////////////////////////////////////////////////////////////////
//Converting RLs and Heights
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//ConvertRLtoHeight
//takes an RL and converts it to a height reletive to the South east standards
double	Bay::ConvertRLtoHeight( double dRL ) const
{
	return (dRL - GetStandardPosition(CNR_SOUTH_EAST).z);
}

/////////////////////////////////////////////////////////////////////////////////
//ConvertHeightToRL
//takes an Height and converts it to a RL reletive to the South east standards
double	Bay::ConvertHeightToRL( double dHeight ) const
{
	return (dHeight + GetStandardPosition(CNR_SOUTH_EAST).z);
}

/////////////////////////////////////////////////////////////////////////////////
//AddLift
//adds a lift to the lift array and sets the appropriate pointers and number
int Bay::AddLift(Lift *pLift)
{
	int			iID;
	Vector3D	Vector;
	Matrix3D	Transform;

	iID = m_LiftList.AddLift( pLift, 0.00 );
	pLift->SetBayPointer( this );

	//move this lift above the previous lift
	Vector.set( 0.00, 0.00, 0.00 );
	double dRL = 0.00;
	if( iID>0 )
	{
		dRL = GetRLOfLift( iID-1 );
		dRL+= GetRiseFromRiseEnum( GetLift( iID-1 )->GetRise(), GetStarSeparation() );
		Vector.z = dRL;
		Transform.setTranslation( Vector );
//		pLift->SetTransform( Transform * pLift->GetTransform() );
		pLift->Move( Transform, true );
	}
	m_LiftList.SetRL( iID, dRL);

//	MakeStandardsFitLifts(NORTH);
//	MakeStandardsFitLifts(SOUTH);

	if( pLift!=NULL && pLift->GetBayPointer()!=NULL &&
		pLift->GetBayPointer()->GetUseLedgerEveryMetre() )
	{
		Component *pComp;
		pComp = pLift->GetComponent( CT_LEDGER, LIFT_RISE_1000MM, NORTH );
		if( pComp==NULL )
			pLift->AddComponent( CT_LEDGER, pLift->GetBayPointer()->GetBayLength(), NORTH, LIFT_RISE_1000MM, MT_STEEL );
	}

	return iID;
}

////////////////////////////////////////////////////////////////////////////////////
//InsertLift
//Inserts a lift, renumbers and moves all lifts above the new lift
bool Bay::InsertLift(int iAtLift, Lift *pLift, bool bMoveLiftsUp/*=true*/, bool bAdjustStandards/*=true*/ )
{
	int			iID, iSize, iRunID;
	Vector3D	Vector;
	Matrix3D	Transform;
	double		dRise, dRL;
	bool		bIsAutobuild;

	iRunID = GetController()->GetRunID( GetRunPointer() );
	bIsAutobuild = (iRunID==ID_NONE_MATCHING);

	assert( (iAtLift>=0) && (iAtLift<=GetNumberOfLifts()) );

	if( !m_LiftList.InsertLift( pLift, 0.00, iAtLift) )
		return false;
	pLift->SetBayPointer( this );

	if( bIsAutobuild )
		pLift->Move( GetController()->Get3DCrosshairTransform().invert(), false );

	if( bMoveLiftsUp )
	{
		//move this lift above the previous lift
		Vector.set( 0.00, 0.00, 0.00 );
		iID = iAtLift;
		if( iID>0 )
		{
			dRL = GetRLOfLift( iID-1 );
			dRise = GetRiseFromRiseEnum( GetLift( iID-1 )->GetRise(), GetStarSeparation() );
			Vector.z = ConvertRLtoStarRL(dRL+dRise, GetStarSeparation());
			Transform.setTranslation( Vector );
			if( bIsAutobuild )
			{
	//			pLift->SetTransform( Transform * pLift->GetTransform() );
				pLift->Move( Transform, true );
			}
			m_LiftList.SetRL( iID, dRL+dRise );
		}

		////////////////////////////////////////////////////////////
		//renumber and move all remaining lifts
		assert( pLift->GetRise()>0.00 );
		dRise = GetRiseFromRiseEnum(pLift->GetRise(), GetStarSeparation());
		Vector.set( 0.00, 0.00, dRise );
		Transform.setTranslation( Vector );
		iSize = GetNumberOfLifts();
		for( iID=iAtLift+1; iID<iSize; iID++ )
		{
			pLift = GetLift( iID );
			assert( pLift->GetLiftID()==iID );
			pLift->Move( Transform, true );
			m_LiftList.SetRL( iID, (pLift->GetRL()+dRise) );
		}
	}

	if( bAdjustStandards )
	{
		MakeStandardsFitLifts(NORTH);
		MakeStandardsFitLifts(SOUTH);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
//DeleteLift
//Deletes a lift, renumbers and moves all lifts above the deleted lift
bool Bay::DeleteLift(int iLiftID, bool bMoveLiftsAboveDown/*=true*/ )
{
	int			iID, iSize;
	double		dDeletedHeight;
	Lift		*pLift;
	Vector3D	Vector;
	Matrix3D	Transform, 	OriginalOriginTransform,
				OriginalRunTransform, OriginalBayTransform;

	/////////////////////////////////////////////////////////////////
	iSize = GetNumberOfLifts();
	if( iLiftID>=0 && iLiftID<iSize )
	{
		pLift=NULL;
		pLift = GetLift( iLiftID );
		assert(pLift!=NULL);
		dDeletedHeight = GetRiseFromRiseEnum(GetLiftRise(iLiftID), GetStarSeparation());
		delete pLift;
		pLift=NULL;
		m_LiftList.RemoveAt( iLiftID );
	}

	/////////////////////////////////////////////////////////////////
	//Size has changed!
	iSize = GetNumberOfLifts();

	/////////////////////////////////////////////////////////////////
	//UnMove Bay
/*	OriginalOriginTransform	= GetController()->UnMove3DCrosshair();
	OriginalRunTransform	= GetRunPointer()->UnMove();
	OriginalBayTransform	= UnMove();
*/
	////////////////////////////////////////////////////////////
	//renumber and move all lifts above the deleted one
	if( bMoveLiftsAboveDown )
	{
		Vector.set( 0.00, 0.00, (-1.00*dDeletedHeight) );
		Transform.setTranslation( Vector );
	}

	for( iID=iLiftID; iID<iSize; iID++ )
	{
		pLift = GetLift( iID );
		pLift->SetLiftID( iID );
		if( bMoveLiftsAboveDown )
		{
			pLift->Move( Transform, true );
			m_LiftList.SetRL( iID, (pLift->GetRL()-dDeletedHeight) );
		}
	}

/*	/////////////////////////////////////////////////////////////////
	//Restore the bays original location
	Move( OriginalBayTransform, true );
	GetRunPointer()->Move( OriginalRunTransform, true );
	GetController()->Move3DCrosshair( OriginalOriginTransform, false );
*/
	/////////////////////////////////////////////////////////////////
	//Are there any elements left in this bay?
	if( (GetNumberOfLifts()<=0) && (GetNumberOfBayComponents()<=0) )
	{
		SetDirtyFlag( DF_DELETE );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//RemoveLift
//Removes a lift
bool Bay::RemoveLift( int iLiftID, int iCount/*=1*/ )
{
	int iSize;

	iSize = GetNumberOfLifts();
	if( (iLiftID>=0) && (iLiftID<iSize) && ((iCount+iLiftID)<=iSize) )
	{
		//JSB todo 990930 - I don't think this is deleting the components, thus a memory leak
		m_LiftList.RemoveAt( iLiftID, iCount );
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////
//Create3DView
//Tells the components of the bay to draw them selves
bool  Bay::Create3DView()
{
	int			i, iSize;
	Lift		*pLift;
	Component	*pComponent;

 	iSize = GetNumberOfLifts();
	for( i=0; i<iSize; i++ )
	{
		pLift = m_LiftList.GetLift(i);
		pLift->Create3DView();
	}

	iSize = GetNumberOfBayComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = m_caComponents.GetComponent(i);
		pComponent->Create3DView();
	}

	if(acedUsrBrk()==1)
	{
		CString sMsg;
		sMsg = _T("Do you wish to stop the creation of the 3D view?");
		if( MessageBox( NULL, sMsg, _T("Cancel Request"), MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2 )==IDYES )
			return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
//Delete3DView
//Tells the components of the bay to delete themselves
void Bay::Delete3DView()
{
	int			i, iSize;
	Lift		*pLift;
	Component	*pComponent;

 	iSize = GetNumberOfLifts();
	for( i=0; i<iSize; i++ )
	{
		pLift = m_LiftList.GetLift(i);
		pLift->Delete3DView();
	}

	iSize = GetNumberOfBayComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = m_caComponents.GetComponent(i);
		pComponent->Delete3DView();
	}
}

////////////////////////////////////////////////////////////////////////////////////
//GetVisible
//determines the visibility of this bay, if all components have the same
//	visibility setting then it ruturns that, otherwise it returns MULTI_SELECT
VisibilityEnum Bay::GetVisible()
{
	VisibilityEnum	eVisible, eVis;
	int				i, iSize;
	Lift			*pLift;
	Component		*pComponent;

	iSize = GetNumberOfLifts();
	if( iSize<=0 )
		return VIS_INVALID;

	pLift = m_LiftList.GetLift(0);
	eVisible = pLift->GetVisible();
	for( i=1; i<iSize; i++ )
	{
		pLift = m_LiftList.GetLift(i);
		eVis=pLift->GetVisible();
		if( eVis==VIS_MULTI_SELECTION )
			return VIS_MULTI_SELECTION;
		if( eVis==VIS_INVALID )
			return VIS_INVALID;
		if( eVisible!=eVis )
			return VIS_MULTI_SELECTION;
	}

	iSize = GetNumberOfBayComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = m_caComponents.GetComponent(i);
		eVis=pComponent->GetVisible();
		if( eVis==VIS_MULTI_SELECTION )
			return VIS_MULTI_SELECTION;
		if( eVis==VIS_INVALID )
			return VIS_INVALID;
		if( eVisible!=eVis )
			return VIS_MULTI_SELECTION;
	}

	return eVisible;
}

///////////////////////////////////////////////////////////////////////////////
//SetVisible
//sets the visibility for the component of this bay
void Bay::SetVisible(VisibilityEnum eVisible/*=VIS_VISIBLE*/)
{
	int			i, iSize;
	Lift		*pLift;
	Component	*pComponent;

	iSize = GetNumberOfLifts();
	for( i=0; i<iSize; i++ )
	{
		pLift = m_LiftList.GetLift(i);
		pLift->SetVisible( eVisible );
	}

	iSize = GetNumberOfBayComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = m_caComponents.GetComponent(i);
		pComponent->SetVisible( eVisible );
	}
}

////////////////////////////////////////////////////////////////////////////////////
//GetRLOfLift
//finds the RL of a particular lift
double Bay::GetRLOfLift(int iLiftID) const
{
	double dRL;
	dRL = 0.00;
	if( GetNumberOfLifts()>0 )
		dRL = m_LiftList.GetRL( iLiftID );
	return dRL;
}

////////////////////////////////////////////////////////////////////////////////////
//GetNumberOfBayComponents
//Then number of compoents that belong to the bay but not in a lift, eg standards
int Bay::GetNumberOfBayComponents() const 
{
	return m_caComponents.GetSize();
}

/////////////////////////////////////////////////////////////////////////////////
//ApplyBayResize
//
void Bay::ApplyBayResize( bool bChangeComponents/*=true*/ )
{
	Run		*pRun;
	bool	bWidthChange, bLengthChange;
	double	dNewLength, dNewWidth, dPrevL, dPrevW, dW;

	dNewLength	= GetBayLength();
	dNewWidth	= GetBayWidth();
	dPrevL		= GetPreviousLength();
	dPrevW		= GetPreviousWidth();

	bWidthChange	= ( dNewWidth<dPrevW-ROUND_ERROR	|| dNewWidth>dPrevW+ROUND_ERROR );
	bLengthChange	= ( dNewLength<dPrevL-ROUND_ERROR	|| dNewLength>dPrevL+ROUND_ERROR );

	if( !bWidthChange && !bLengthChange )
	{
		//dimensions not changed
		return;
	}

	if( bLengthChange )
	{
		if( GetOuter()!=NULL )
			GetOuter()->ResizeAllBaysInDirection( dNewLength, NORTH );

		if( GetInner()!=NULL )
			GetInner()->ResizeAllBaysInDirection( dNewLength, SOUTH );

		pRun = GetRunPointer();
		assert( pRun!=NULL );
		GetTemplate()->SetBayLength( dNewLength );
		pRun->ShuffleSubsequentBays( GetID()+1, dPrevL-dNewLength );
	}

	if( bWidthChange )
	{
		if( GetForward()!=NULL )
			GetForward()->ResizeAllBaysInDirection( dNewWidth, EAST );
		if( GetBackward()!=NULL )
			GetBackward()->ResizeAllBaysInDirection( dNewWidth, WEST );

		Vector3D	Vect;
		Matrix3D	Mtrx;

		dW = GetBayDimensionActual( dNewWidth, false )
			-GetBayDimensionActual( dPrevW, false );
		Vect.set( 0.00, dW, 0.00 );
		Mtrx.setToTranslation( Vect );

		pRun = GetRunPointer();
		assert( pRun!=NULL );
		//pRun->MoveAllNorthernRuns( Mtrx, GetID(), true );

		pRun->GetController()->SetHasBeenVisited( false );

		//set this run as no movement required!
		pRun->SetHasBeenVisited( true );
		Visit( Mtrx, true );
		
		GetTemplate()->SetBayWidth( dNewWidth );
	}

	GetTemplate()->SetAllSchematicText();
}

////////////////////////////////////////////////////////////////////////////////////
//DisplayBayDetailsDialog
//
int Bay::DisplayBayDetailsDialog(CWnd *pParent/*=NULL*/)
{
	int iButtonPressed;

	//switch resources to the dll's resources
	HINSTANCE hInst = NULL;
	if( _hdllInstance!=NULL )
	{
		hInst = AfxGetResourceHandle();
		AfxSetResourceHandle(_hdllInstance);
	}

	//Create and display the dialog box
	m_pBayDetailsDialog = new BayDetailsDialog(pParent, this);
	iButtonPressed = m_pBayDetailsDialog->DoModal();
	DELETE_PTR(m_pBayDetailsDialog);

	//Restore the resources
	if( _hdllInstance!=NULL )
	{
	    AfxSetResourceHandle(hInst);
	}

	return iButtonPressed;
}


/////////////////////////////////////////////////////////////////////////////
//DeleteBayDetails()
//
void Bay::DeleteBayDetails()
{
	DELETE_PTR(m_pBayDetailsDialog);
}


////////////////////////////////////////////////////////////////////////////////////
//GetStaggeredHeights
//
bool Bay::GetStaggeredHeights()
{
	return m_bStaggeredHeights;
}

////////////////////////////////////////////////////////////////////////////////////
//SetStaggeredHeights
//
void Bay::SetStaggeredHeights(bool bStaggeredHeights)
{
	m_bStaggeredHeights = bStaggeredHeights;
}

////////////////////////////////////////////////////////////////////////////////////
//GetStandardFit
//
StandardFitEnum Bay::GetStandardFit()
{
	return m_eStandardsFit;
}

////////////////////////////////////////////////////////////////////////////////////
//SetStandardsFit
//
void Bay::SetStandardsFit(StandardFitEnum eStandardsFit)
{
	m_eStandardsFit = eStandardsFit;
}


/////////////////////////////////////////////////////////////////////////////////
//SetTransform
//
void Bay::SetTransform(Matrix3D Transform)
{
	m_Transform = Transform;
}

/////////////////////////////////////////////////////////////////////////////////
//GetTransform
//
Matrix3D Bay::GetTransform() const
{
	return m_Transform;
}

/////////////////////////////////////////////////////////////////////////////////
//UnMove
//returns the original transformation which has now been lost
Matrix3D Bay::UnMove()
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

/////////////////////////////////////////////////////////////////////////////////
//UnMoveSchematic
//returns the original transformation which has now been lost
Matrix3D Bay::UnMoveSchematic()
{
	return GetTemplate()->UnMoveSchematic();
}

/////////////////////////////////////////////////////////////////////////////////
//MoveBays
//
void Bay::MoveBays(Matrix3D Transform, BayDirectionEnum eDirection )
{
	Bay *pNextBay;

	//get the next bay
	switch( eDirection )
	{
	default:
		eDirection = DIRECTION_FORWARD;
		//fallthrough
	case( DIRECTION_FORWARD ):
		pNextBay = GetForward();
		break;
	case( DIRECTION_BACKWARD ):
		pNextBay = GetBackward();
		break;
	case( DIRECTION_OUTER ):
		pNextBay = GetOuter();
		break;
	case( DIRECTION_INNER ):
		pNextBay = GetInner();
		break;
	case( DIRECTION_CORNER_FORWARD ):
		pNextBay = GetInner();
		break;
	case( DIRECTION_CORNER_BACKWARD ):
		pNextBay = GetInner();
		break;
	}

	if( pNextBay!=NULL )
	{
		pNextBay->Move( Transform, true );
		pNextBay->MoveSchematic( Transform, true );
		pNextBay->MoveBays( Transform, eDirection );
	}
}

////////////////////////////////////////////////////////////////////////////////////
//DisplayLiftDetailsDialog
//
int Bay::DisplayLiftDetailsDialog(CWnd *pParent/*=NULL*/)
{
	int iButtonPressed;

	iButtonPressed = m_LiftList.DisplayLiftDetailsDialog(pParent);	

	return iButtonPressed;
}


////////////////////////////////////////////////////////////////////////////////////
//DeleteLiftDetailsDialog
//
void Bay::DeleteLiftDetailsDialog()
{
	m_LiftList.DeleteLiftDetailsDialog();
}


//////////////////////////////////////////////////////////////////
//Has the been marked for deletion
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//Says this object needs to be deleted
DirtyFlagEnum	Bay::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

/////////////////////////////////////////////////////////////////////////////////
//SetDirtyFlag
//set the object to be deleted
void	Bay::SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward/*=false*/ )
{
	if( dfDirtyFlag==DF_CLEAN )
	{
		//set the dirty flag
		m_dfDirtyFlag = dfDirtyFlag;
		if(bSetDownward)
		{
			for( int i=0; i<GetNumberOfLifts(); i++ )
			{
				GetLift(i)->SetDirtyFlag(dfDirtyFlag, bSetDownward);
			}
			for(int i=0; i<GetNumberOfBayComponents(); i++ )
			{
				GetBayComponent(i)->SetDirtyFlag(dfDirtyFlag);
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
	if( GetRunPointer()!=NULL )
		GetRunPointer()->SetDirtyFlag( DF_DIRTY );
	if( GetBayType()==BAY_TYPE_LAPBOARD )
	{
		assert( GetController()!=NULL );
		GetController()->SetDirtyFlag( DF_DIRTY );
	}
}

/////////////////////////////////////////////////////////////////////////////////
//CleanUp
//delete all objects, that this object owns, if they are marked
//	for deletion
void	Bay::CleanUp()
{
	Component	*pComponent;
	Lift		*pLift;
	int			i;

	SetDirtyFlag( DF_CLEAN );

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		switch( pLift->GetDirtyFlag() )
		{
		case( DF_DELETE ):
			DeleteLift(i);
			i--;	//we have just deleted the element, so don't increment i;
			break;
		case( DF_DIRTY ):
			pLift->CleanUp();
			if( pLift->GetDirtyFlag()!=DF_CLEAN )
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

	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComponent = m_caComponents.GetComponent(i);
		if( pComponent->GetDirtyFlag()==DF_DELETE )
		{
			DeleteBayComponent(i);
			i--;	//we have just deleted the element, so don't increment i;
		}
	}


	//Are there any component left in this bay
	if( GetNumberOfLifts()==0 )
	{
		//There are no lifts in this bay, do we have any bay components?

		bool dCompletlyDeleted = false;
		if( GetNumberOfBayComponents()==0 )
		{
			//There are no component in this bay, so mark it for delete
			dCompletlyDeleted = true;
		}
		else
		{
			//There are no component in this bay, so mark it for delete
			bool bFoundNonText = false;
			for( i=0; i<GetNumberOfBayComponents(); i++ )
			{
				pComponent = m_caComponents.GetComponent(i);
				if( pComponent->GetType()!=CT_TEXT )
				{
					//we have found a non-text item, we don't need to delete!
					bFoundNonText = true;
					break;
				}
			}
			if( !bFoundNonText )
			{
				//All the components we found were text so we can delete this bay!
				dCompletlyDeleted = true;
			}
		}

		if( dCompletlyDeleted )
		{
			//We can delete this bay!
			SetDirtyFlag( DF_DELETE );
		}
	}

}


/////////////////////////////////////////////////////////////////////////////////
//DeleteBayComponent
//
bool Bay::DeleteBayComponent( int iComponentID )
{
	bool		bReturn;

	bReturn = m_caComponents.DeleteComponent(iComponentID);

	if( GetNumberOfLifts()<=0 && GetNumberOfBayComponents()<=0 )
	{
		SetDirtyFlag( DF_DELETE );
	}
	return bReturn;
}

//////////////////////////////////////////////////////////////////////////////////
//UpdateNumberText()
//Sequencially number this bay
void Bay::UpdateNumberText()
{
	int			i;
	CString		sText;
	Component	*pComponent;

	if( IsDrawAllowed() )
	{
		//going backward would probably be quicker
		for( i=GetNumberOfBayComponents()-1; i>=0; i-- )
		{
			pComponent = m_caComponents.GetComponent(i);
			if( pComponent->GetType()==CT_TEXT )
			{
				//BayNumber,BayLength,Forward Matrix Label, Backward Matrix Label, 
				sText.Format( _T("%i"), GetBayNumber() );
				pComponent->SetText( sText );
				GetTemplate()->SetAllSchematicText( );
				break;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//MakeStandardsFitLifts
//adjusts the height of the standards if they are too short,
//	such that they will support the lifts
//	dTop and dBottom are used by neighboring lifts to set the height requirement
//	if dTop<dBottom then these values are ignored
void Bay::MakeStandardsFitLifts( SideOfBayEnum eSideToTest, double dTop/*=-2.00*/, double dBottom/*=-1.00*/ )
{
	CornerOfBayEnum	eCornerLeft, eCornerRight;
	double			dRLOfBottomLift, dRLOfTopLift;

	dRLOfBottomLift = m_LiftList.GetRL( 0 );
	if( dTop>dBottom )
		dRLOfBottomLift = min( dBottom, dRLOfBottomLift );
		
	dRLOfTopLift = GetRLOfTopLiftPlusHandrail( eSideToTest );
	if( dTop>dBottom )
		dRLOfTopLift = max( dTop, dRLOfTopLift );

	eCornerLeft		= CNR_INVALID;
	eCornerRight	= CNR_INVALID;
	switch( eSideToTest )
	{
	case( NORTH ):
		eCornerRight	= CNR_NORTH_EAST;
		if( GetBackward()==NULL )
		{
			eCornerLeft		= CNR_NORTH_WEST;
		}
		else
		{
			MakeAStandardFitLifts( NORTH, CNR_INVALID, CNR_NORTH_EAST, dRLOfTopLift, dRLOfBottomLift );
			GetBackward()->MakeAStandardFitLifts( NORTH, CNR_NORTH_EAST, CNR_INVALID, dRLOfTopLift, dRLOfBottomLift );
			return;
		}

		break;
	case( SOUTH ):
		if( GetInner()==NULL )
		{
			eCornerLeft		= CNR_SOUTH_EAST;
			if( GetBackward()==NULL )
			{
				eCornerRight	= CNR_SOUTH_WEST;
			}
			else
			{
				MakeAStandardFitLifts( SOUTH, CNR_INVALID, CNR_SOUTH_EAST, dRLOfTopLift, dRLOfBottomLift );
				GetBackward()->MakeAStandardFitLifts( SOUTH, CNR_SOUTH_EAST, CNR_INVALID, dRLOfTopLift, dRLOfBottomLift );
				return;
			}
		}
		else
		{
			//tell the inner bay to make NORTHERN standards fit this height
			GetInner()->MakeStandardsFitLifts( NORTH, dRLOfTopLift, dRLOfBottomLift );
			return;
		}
		break;
	case( EAST ):
		eCornerLeft	= CNR_NORTH_EAST;
		if( GetInner()==NULL )
		{
			eCornerRight	= CNR_SOUTH_EAST;
		}
		else
		{
			MakeAStandardFitLifts( EAST, CNR_INVALID, CNR_NORTH_EAST, dRLOfTopLift, dRLOfBottomLift );
			GetInner()->MakeAStandardFitLifts( EAST, CNR_NORTH_EAST, CNR_INVALID, dRLOfTopLift, dRLOfBottomLift );
			return;
		}
		break;
	case( WEST ):
		if( GetBackward()==NULL  )
		{
			eCornerRight	= CNR_NORTH_WEST;
			eCornerLeft		= CNR_SOUTH_WEST;
		}
		else
		{
			//tell the backward bay to make EASTERN standards fit this height
			GetBackward()->MakeStandardsFitLifts( EAST, dRLOfTopLift, dRLOfBottomLift );
			return;
		}
		break;
	}

	//fit for this side - so make top<bottom
	MakeAStandardFitLifts( eSideToTest, eCornerLeft, eCornerRight, -2.00, -1.00 );
}


//////////////////////////////////////////////////////////////////////////////////
//MakeAStandardFitLifts
//Adjust the height of the standards on the specified corners
//	note the corners will be ignored if they are INVALID
//	dTop and dBottom are used by neighboring lifts to set the height requirement
//	if dTop<dBottom then these values are ignored
void Bay::MakeAStandardFitLifts( SideOfBayEnum eSideToTest, CornerOfBayEnum	eCornerLeft,
								CornerOfBayEnum eCornerRight, double dTop, double dBottom )
{
	Point3D			ptLeft, ptRight;
	double			dRLOfTopLift,
					dRLOfBottomLift, dRLOfStandardBottomLeft, dRLOfStandardBottomRight;
	bool			bSoleboardLeft, bSoleboardRight;

	if( GetNumberOfLifts()>0 )
	{
		///////////////////////////////////////////////////////////////
		//Bottom of either standards
		dRLOfBottomLift = m_LiftList.GetRL( 0 );
		if( dTop>dBottom )
			dRLOfBottomLift = min( dRLOfBottomLift, dBottom );

		///////////////////////////////////////////////////////////////
		//Top of either standards
		dRLOfTopLift = GetRLOfTopLiftPlusHandrail( eSideToTest );
		if( dTop>dBottom )
			dRLOfTopLift = max( dRLOfTopLift, dTop );

		///////////////////////////////////////////////////////////////
		//LeftSide standard only
		if( eCornerLeft!=CNR_INVALID )
		{
			bSoleboardLeft	= (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, CornerAsSideOfBay(eCornerLeft) ) );

			ptLeft	= GetStandardPosition( eCornerLeft	);
			dRLOfStandardBottomLeft		= ptLeft.z;

			//Bottom of the left standard
			if( dRLOfBottomLift<dRLOfStandardBottomLeft )
			{
				//The left standard is not low enough
				ptLeft.z = dRLOfBottomLift;
				SetStandardPosition( ptLeft, eCornerLeft );
			}
			CreateStandardsArrangment( dRLOfTopLift, eCornerLeft, bSoleboardLeft ); 
		}

		///////////////////////////////////////////////////////////////
		//RightSide standard only
		if( eCornerRight!=CNR_INVALID )
		{
			bSoleboardRight	= (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, CornerAsSideOfBay(eCornerRight) ) );

			//Bottom of the left standard
			ptRight	= GetStandardPosition( eCornerRight	);
			dRLOfStandardBottomRight	= ptRight.z;
			if( dRLOfBottomLift>dRLOfStandardBottomRight )
			{
				//The right standard is not low enough
				ptRight.z = dRLOfBottomLift;
				SetStandardPosition( ptRight, eCornerRight );
			}
			CreateStandardsArrangment( dRLOfTopLift, eCornerRight, bSoleboardRight ); 
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//MakeAStandardFitLifts
//Adjust the height of the standards on the specified corners
//	note the corners will be ignored if they are INVALID
//	dTop and dBottom are used by neighboring lifts to set the height requirement
//	if dTop<dBottom then these values are ignored
void Bay::MakeAStandardFitLifts( CornerOfBayEnum eCorner )
{
	Point3D			pt;
	double			dRLBottom, dRLTop, dRLMin, dRLMax;

	if( GetNumberOfLifts()>0 )
	{
		FindTopBottomRLsAtCornerByLifts( eCorner, dRLMin, dRLMax );

		////////////////////////////////////////////////////////////////////////
		//look at all the surrounding bays and find the Height required!
		switch( eCorner )
		{
		case( CNR_NORTH_EAST ):
			if( GetForward()!=NULL )
			{
				// |   ?  |
				//_|______x______
				// |      |      |
				//?|   *--|->    |
				//_|______|______|
				// |   ?  |
				GetForward()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_WEST, dRLBottom, dRLTop );
				dRLMin = min( dRLMin, dRLBottom );
				dRLMax = max( dRLMax, dRLTop );
				if( GetForward()->GetOuter()!=NULL )
				{
					//         ______
					//        |      |
					//     ?  |   ^  |
					//_|______x___|__|
					// |      |   |  |
					//?|   *--|---   |
					//_|______|______|
					// |   ?  |
					GetForward()->GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
					dRLMin = min( dRLMin, dRLBottom );
					dRLMax = max( dRLMax, dRLTop );
				}
			}
			if( GetOuter()!=NULL )
			{
				//  ______ 
				// |      |
				// |   ^  |
				//_|___|__x_
				// |   |  |
				//?|   *  |?
				//_|______|_
				// |   ?  |
				GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_EAST, dRLBottom, dRLTop );
				dRLMin = min( dRLMin, dRLBottom );
				dRLMax = max( dRLMax, dRLTop );
			}
			break;
		case( CNR_SOUTH_EAST ):
			if( GetInner()!=NULL )
			{
				// |   ?  |
				//_|______|_
				// |      |
				//?|   *  |?
				//_|___|__x_
				// |   |  |
				// |   v  |
				// |______|
				//This bay does't own this standard
				GetInner()->MakeAStandardFitLifts( CNR_NORTH_EAST );
				return;
			}
			else if( GetForward()!=NULL )
			{
				if( GetForward()->GetInner()!=NULL )
				{
					// |   ?  |
					//_|______|______
					// |      |      |
					//?|   *--|---   |
					//_|______x___|__|
					//  No Bay|   |  |
					//   Here |   v  |
					//        |______|
					//This bay does't own this standard
					GetForward()->GetInner()->MakeAStandardFitLifts( CNR_NORTH_WEST );
					return;
				}
				else
				{
					// |   ?  |
					//_|______|______
					// |      |      |
					//?|   *--|-->   |
					//_|______x______|
					//  No Bay No Bay
					//   Here   Here
					GetForward()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
					dRLMin = min( dRLMin, dRLBottom );
					dRLMax = max( dRLMax, dRLTop );
				}
			}
			break;
		case( CNR_SOUTH_WEST ):
			if( GetInner()!=NULL )
			{
				// |   ?  |
				//_|______|_
				// |      |
				//?|   *  |?
				//_x___|__|_
				// |   |  |
				// |   v  |
				// |______|
				//This bay does't own this standard
				GetInner()->MakeAStandardFitLifts( CNR_NORTH_WEST );
				return;
			}
			else if( GetBackward()!=NULL )
			{
				//        |   ?  |
				//  ______|______|_
				// |      |      |
				// |   <--|--*   |?
				// |______x______|_
				//         No Bay
				//          Here
				//This bay does't own this standard
				GetBackward()->MakeAStandardFitLifts( CNR_SOUTH_EAST );
				return;
			}
			break;
		case( CNR_NORTH_WEST ):
			if( GetBackward()!=NULL )
			{
				//        |   ?  |
				//  ______x______|_
				// |      |      |
				// |   <--|---*  |?
				// |______|______|_
				//        |   ?  |
				//This bay does't own this standard
				GetBackward()->MakeAStandardFitLifts( CNR_NORTH_EAST );
				return;
			}
			else if( GetOuter()!=NULL )
			{
				//       ______ 
				//      |      |
				//      |   ^  |
				//      x___|__|_
				//No Bay|   |  |
				// Here |   *  |?
				//      |______|_
				//      |   ?  |
				GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
				dRLMin = min( dRLMin, dRLBottom );
				dRLMax = max( dRLMax, dRLTop );
			}
			break;
		default:
			//invalid corner
			assert( false );
			break;
		}

		SetStandardToRLs( eCorner, dRLMax, dRLMin );
	}
}


bool Bay::FindTopBottomRLsAtCornerByLifts( CornerOfBayEnum eCorner, double &dRLBottom, double &dRLTop, bool bLookAtStandardsToo/*=false*/)
{
	double			dHeightOfExistingStandard;
	SideOfBayEnum	eSideToTest;

	dRLBottom = 0.00;
	dRLTop = 0.00;

	if( GetNumberOfLifts()<=0 )
		return false;

	///////////////////////////////////////////////////////////////
	//Bottom of standards
	dRLBottom = m_LiftList.GetRL( 0 );

	///////////////////////////////////////////////////////////////
	//Top of standards
	eSideToTest = CornerAsSideOfBay( eCorner );
	dRLTop = GetRLOfTopLiftPlusHandrail( eSideToTest );
	if( bLookAtStandardsToo )
	{
		dHeightOfExistingStandard = GetHeightOfStandards( eCorner );
		dRLTop = max( dHeightOfExistingStandard, dRLTop );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//GetRLOfTopLiftPlusHandrail
//Find the RL of a lift including the handrail if it has one
double Bay::GetRLOfTopLiftPlusHandrail( SideOfBayEnum eSide ) const
{
	bool			bTestingCorner;
	double			dRLOfTopLift;
	Lift			*pLift;
	SideOfBayEnum	eSideLeft, eSideRight, eHopupSide1, eHopupSide2, eHopupSide3, eHopupSide4;

	dRLOfTopLift = 0.00;

	///////////////////////////////////////////////////////////////////
	//we must have at least one lift;
	if( GetNumberOfLifts()<=0 )
		return dRLOfTopLift;

	///////////////////////////////////////////////////////////////////
	//get the RL for the lift
	dRLOfTopLift = m_LiftList.GetRL( GetNumberOfLifts()-1 );
	dRLOfTopLift+= RLAdjust();

	eSideLeft		= SIDE_INVALID;
	eSideRight		= SIDE_INVALID;
	eHopupSide1		= SIDE_INVALID;
	eHopupSide2		= SIDE_INVALID;
	eHopupSide3		= SIDE_INVALID;
	eHopupSide4		= SIDE_INVALID;
	pLift = GetLift( GetNumberOfLifts()-1 );
	switch( eSide )
	{
	case( NORTH ):
		eSideLeft		= WEST;
		eSideRight		= EAST;
		eHopupSide1		= WNW;
		eHopupSide2		= NNW;
		eHopupSide3		= NNE;
		eHopupSide4		= ENE;
		bTestingCorner	= false;
		break;
	case( EAST ):
		eSideLeft		= NORTH;
		eSideRight		= SOUTH;
		eHopupSide1		= NNE;
		eHopupSide2		= ENE;
		eHopupSide3		= ESE;
		eHopupSide4		= SSE;
		bTestingCorner	= false;
		break;
	case( SOUTH ):
		eSideLeft		= EAST;
		eSideRight		= WEST;
		eHopupSide1		= ESE;
		eHopupSide2		= SSE;
		eHopupSide3		= SSW;
		eHopupSide4		= WSW;
		bTestingCorner	= false;
		break;
	case( WEST ):
		eSideLeft		= SOUTH;
		eSideRight		= NORTH;
		eHopupSide1		= SSW;
		eHopupSide2		= WSW;
		eHopupSide3		= WNW;
		eHopupSide4		= NNW;
		bTestingCorner	= false;
		break;
	case( NE ):
		eSideLeft		= NORTH;
		eSideRight		= EAST;
		eHopupSide1		= NNE;
		eHopupSide2		= ENE;
		bTestingCorner	= true;
		break;
	case( SE ):
		eSideLeft		= EAST;
		eSideRight		= SOUTH;
		eHopupSide1		= ESE;
		eHopupSide2		= SSE;
		bTestingCorner	= true;
		break;
	case( SW ):
		eSideLeft		= SOUTH;
		eSideRight		= WEST;
		eHopupSide1		= SSW;
		eHopupSide2		= WSW;
		bTestingCorner	= true;
		break;
	case( NW ):
		eSideLeft		= WEST;
		eSideRight		= NORTH;
		eHopupSide1		= WNW;
		eHopupSide2		= NNW;
		bTestingCorner	= true;
		break;
	default:
		assert( false );
		return 0.00;
	}

	bool bHasRail;

	bHasRail = false;
	///////////////////////////////////////////////////////////////////
	//do we have a handrail on any of the surrounding sides?
	assert( eSideLeft!=SIDE_INVALID );
	assert( eSideRight!=SIDE_INVALID );
	assert( eHopupSide1!=SIDE_INVALID );
	assert( eHopupSide2!=SIDE_INVALID );
	if( bTestingCorner )
	{
		//we are only interested in the left and right sides
		if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSideLeft	)!=NULL ||
			pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSideRight	)!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide1 )!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide2 )!=NULL)
		{
			bHasRail = true;
		}
	}
	else
	{
		assert( eSide!=SIDE_INVALID );
		assert( eHopupSide3!=SIDE_INVALID );
		assert( eHopupSide4!=SIDE_INVALID );
		if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSide		)!=NULL ||
			pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSideLeft	)!=NULL ||
			pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSideRight	)!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide1 )!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide2 )!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide3 )!=NULL ||
			pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSide4 )!=NULL)
		{
			bHasRail = true;
		}
	}

	if( !bHasRail && GetID()>=0 && HasMovingDeck() )
	{
		//we don't already have a rail, so keep looking!
		switch( eSide )
		{
		case( NORTH ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetOuter() ) )
				bHasRail|= !LapboardCrossesSideDoes(NORTH);
			break;
		case( EAST ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetForward() ) )
				bHasRail|= !LapboardCrossesSideDoes(EAST);
			break;
		case( SOUTH ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetInner() ) )
				bHasRail|= !LapboardCrossesSideDoes(SOUTH);
			break;
		case( WEST ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetBackward() ) )
				bHasRail|= !LapboardCrossesSideDoes(WEST);
			break;
		case( NE ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetForward() ) )
				bHasRail|= !LapboardCrossesSideDoes(EAST);
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetOuter() ) )
				bHasRail|= !LapboardCrossesSideDoes(NORTH);
			break;
		case( NW ):
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetBackward() ) )
				bHasRail|= !LapboardCrossesSideDoes(WEST);
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetOuter() ) )
				bHasRail|= !LapboardCrossesSideDoes(NORTH);
			break;
		case( SE ):
			//If it doesn't have a bay, nor a lapboard, to the east
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetForward() ) )
				bHasRail|= !LapboardCrossesSideDoes(EAST);
			break;
		case( SW ):
			//If it doesn't have a bay, nor a lapboard, to the West
			if( !DoesLiftHaveNeighborAtSameRL( GetNumberOfLifts()-1, GetBackward() ) )
				bHasRail|= !LapboardCrossesSideDoes(WEST);
			break;
		default:
			assert( false );
			return 0.00;
		}
	}

	//If we should have a handrail here, either from having
	//	an actual handrail, or because we have moving decks
	//	(or because we have a floating 1.0m hopup bracket
	//	stopend?).  Then we will need to add roof protection
	if( bHasRail && GetRoofProtection()!=RP_NONE )
	{
		switch( GetRoofProtection() )
		{
		case( RP_1500MM ):
			return dRLOfTopLift+ConvertRLtoStarRL(COMPONENT_LENGTH_1500, GetStarSeparation());
		case( RP_2000MM ):
			return dRLOfTopLift+ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());

		case( RP_INVALID ):	//fallthrough
		case( RP_NONE ):	//fallthrough
		default:
			assert( false );
		}
	}

	
	//It is still possible to require extra height on the standards,
	//	eg This bay may have a neighbor to the north that has roof
	//	protection and a neighbor to the east or west that may have
	//	roof protection, thus this bay should also have roof protection
	int				i;
	BayList			Neighbors;
	SideOfBayEnum	eNeighborSide[8];

	Neighbors.RemoveAll();
	for( i=0; i<8; i++ )
		eNeighborSide[i]=SIDE_INVALID;

	switch( eSide )
	{
	case( NORTH ):
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetOuter()->GetForward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetForward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetForward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetOuter()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetBackward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		break;
	case( EAST ):
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetOuter()->GetForward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetForward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetForward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetInner()->GetForward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetForward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetForward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		break;
	case( SOUTH ):
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetInner()->GetForward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetForward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetForward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetInner()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetBackward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		break;
	case( WEST ):
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetOuter()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetBackward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetInner()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetBackward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		break;
	case( NE ):
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetOuter()->GetForward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetForward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetForward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		break;
	case( NW ):
		if( GetOuter()!=NULL )
		{
			Neighbors.Add( GetOuter() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetOuter()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetOuter()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			if( GetBackward()->GetOuter()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetOuter() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		break;
	case( SE ):
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = EAST;
			if( GetInner()->GetForward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetForward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetForward()!=NULL )
		{
			Neighbors.Add( GetForward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetForward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetForward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = WEST;
			}
		}
		break;
	case( SW ):
		if( GetInner()!=NULL )
		{
			Neighbors.Add( GetInner() );
			eNeighborSide[Neighbors.GetSize()-1] = WEST;
			if( GetInner()->GetBackward()!=NULL )
			{
				Neighbors.Add( GetInner()->GetBackward() );
				eNeighborSide[Neighbors.GetSize()-1] = NORTH;
			}
		}
		if( GetBackward()!=NULL )
		{
			Neighbors.Add( GetBackward() );
			eNeighborSide[Neighbors.GetSize()-1] = SOUTH;
			if( GetBackward()->GetInner()!=NULL )
			{
				Neighbors.Add( GetBackward()->GetInner() );
				eNeighborSide[Neighbors.GetSize()-1] = EAST;
			}
		}
		break;
	default:
		assert( false );
		return 0.00;
	}

	Bay				*pNeighbor;
	SideOfBayEnum	eNeighbor;
	for( i=0; i<Neighbors.GetSize(); i++ )
	{
		pNeighbor = Neighbors.GetAt(i);
		assert( pNeighbor!=NULL );
		eNeighbor = eNeighborSide[i];
		assert( eNeighbor!=SIDE_INVALID );

		if( pNeighbor->GetRoofProtection()>=RP_1500MM &&
			pNeighbor->GetLift( pNeighbor->GetNumberOfLifts()-1 )
			->GetComponent( (eNeighbor==N||eNeighbor==S)? CT_LEDGER: CT_TRANSOM,
									LIFT_RISE_1500MM, eNeighbor )!=NULL )
		{
			if( pNeighbor->GetRoofProtection()==RP_1500MM )
				return dRLOfTopLift+ConvertRLtoStarRL(COMPONENT_LENGTH_1500, GetStarSeparation());
			if( pNeighbor->GetRoofProtection()==RP_2000MM )
				return dRLOfTopLift+ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation());
		}
	}

	if( bHasRail ) 
	{
		//LIFT_RISE_1000MM = two stars up
		dRLOfTopLift+= GetStarSeparation()*2.00;
	}

	////////////////////////////////////////////////////////////
	//Buttress bays have a virtual handrail at the top of the
	//	top lift, so we should set the height to the top of the
	//	top lift!
	if( GetBayType()==BAY_TYPE_BUTTRESS )
	{
		dRLOfTopLift+= GetRiseFromRiseEnum( GetLiftRise( GetNumberOfLifts()-1), GetStarSeparation() );
	}

	return dRLOfTopLift;
}




/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Bay.cpp $
 * 
 * *****************  Version 255  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:10a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 254  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 253  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:42a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 252  *****************
 * User: Jsb          Date: 13/11/00   Time: 4:20p
 * Updated in $/Meccano/Stage 1/Code
 * About to work from home
 * 
 * *****************  Version 251  *****************
 * User: Jsb          Date: 25/10/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 250  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 249  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 248  *****************
 * User: Jsb          Date: 6/10/00    Time: 1:29p
 * Updated in $/Meccano/Stage 1/Code
 * About to build R1.1e - This should have all of Mark's current small
 * bugs fixed, ready for the second release to Perth (still awaiting
 * feedback from Perth from the first one we sent them)  This is another
 * version for mark to test, and is a candidate for Perth release
 * 
 * *****************  Version 247  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 246  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:49p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 245  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 244  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 242  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 241  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 240  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 239  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 238  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 237  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 236  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 235  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 234  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 233  *****************
 * User: Jsb          Date: 4/08/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed most of the buttress bugs
 * 
 * *****************  Version 232  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:22p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 230  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 229  *****************
 * User: Jsb          Date: 2/08/00    Time: 10:31a
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8s
 * 
 * *****************  Version 228  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 227  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 226  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 225  *****************
 * User: Jsb          Date: 27/07/00   Time: 1:07p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 8p
 * 
 * *****************  Version 224  *****************
 * User: Jsb          Date: 27/07/00   Time: 11:10a
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to find out how SetRL's worked
 * 
 * *****************  Version 223  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:13p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 222  *****************
 * User: Jsb          Date: 25/07/00   Time: 5:07p
 * Updated in $/Meccano/Stage 1/Code
 * Column Ties are not oriented correctly
 * 
 * *****************  Version 221  *****************
 * User: Jsb          Date: 24/07/00   Time: 1:38p
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to B4 3D position exact started
 * 
 * *****************  Version 220  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 219  *****************
 * User: Jsb          Date: 18/07/00   Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 218  *****************
 * User: Jsb          Date: 17/07/00   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the 500mm star separation to 495.3
 * 
 * *****************  Version 217  *****************
 * User: Jsb          Date: 13/07/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 216  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 215  *****************
 * User: Jsb          Date: 7/07/00    Time: 7:50a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 214  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 213  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 212  *****************
 * User: Jsb          Date: 28/06/00   Time: 1:27p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create a seperate Toolbar project
 * 
 * *****************  Version 211  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 210  *****************
 * User: Jsb          Date: 26/06/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * auto changing heights of adjoining stairs and ladders
 * 
 * *****************  Version 209  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 208  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 207  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 206  *****************
 * User: Jsb          Date: 20/06/00   Time: 12:28p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way moving decks and roof protect lapboards look
 * for  lapboards 
 * 
 * *****************  Version 205  *****************
 * User: Jsb          Date: 19/06/00   Time: 1:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 204  *****************
 * User: Jsb          Date: 16/06/00   Time: 4:59p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 203  *****************
 * User: Jsb          Date: 16/06/00   Time: 3:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 202  *****************
 * User: Jsb          Date: 15/06/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 201  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 200  *****************
 * User: Jsb          Date: 14/06/00   Time: 8:03a
 * Updated in $/Meccano/Stage 1/Code
 * Just Built RC8b
 * 
 * *****************  Version 199  *****************
 * User: Jsb          Date: 13/06/00   Time: 11:28a
 * Updated in $/Meccano/Stage 1/Code
 * RC8a
 * 
 * *****************  Version 198  *****************
 * User: Jsb          Date: 9/06/00    Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * taking code home
 * 
 * *****************  Version 197  *****************
 * User: Jsb          Date: 9/06/00    Time: 1:47p
 * Updated in $/Meccano/Stage 1/Code
 * About to play with the way Bay::SetAndAdjustRLs works
 * 
 * *****************  Version 196  *****************
 * User: Jsb          Date: 8/06/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Rotation of Bays is operational, rotation of laps is nearly working
 * 
 * *****************  Version 195  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 193  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 192  *****************
 * User: Jsb          Date: 2/06/00    Time: 3:02p
 * Updated in $/Meccano/Stage 1/Code
 * need to find why tietubes are removing hopups
 * 
 * *****************  Version 191  *****************
 * User: Jsb          Date: 1/06/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 190  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 189  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 188  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 187  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 186  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 185  *****************
 * User: Jsb          Date: 22/05/00   Time: 9:12a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 184  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 183  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 182  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 181  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 180  *****************
 * User: Jsb          Date: 12/05/00   Time: 8:33a
 * Updated in $/Meccano/Stage 1/Code
 * About to start the mills _tsystem corners
 * 
 * *****************  Version 179  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 178  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 177  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 176  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 175  *****************
 * User: Jsb          Date: 4/05/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 174  *****************
 * User: Jsb          Date: 3/05/00    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 173  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 172  *****************
 * User: Jsb          Date: 20/04/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 171  *****************
 * User: Jsb          Date: 20/04/00   Time: 12:53p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 170  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 169  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 168  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 167  *****************
 * User: Jsb          Date: 11/04/00   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 166  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 165  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 164  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 163  *****************
 * User: Jsb          Date: 3/04/00    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 162  *****************
 * User: Jsb          Date: 30/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 161  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 160  *****************
 * User: Jsb          Date: 28/03/00   Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 159  *****************
 * User: Jsb          Date: 27/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 158  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 157  *****************
 * User: Jsb          Date: 10/03/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 156  *****************
 * User: Jsb          Date: 9/03/00    Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 155  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 154  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 153  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 152  *****************
 * User: Jsb          Date: 1/03/00    Time: 4:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 151  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 150  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 25/02/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 23/02/00   Time: 2:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 21/02/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Working from home tomorrow
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 15/02/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 11/02/00   Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC2
 * 
 * *****************  Version 142  *****************
 * User: Jsb          Date: 11/02/00   Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC2
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 10/02/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * I think I have complete the JM bug
 * 
 * *****************  Version 140  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 139  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 138  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 136  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 2/02/00    Time: 3:59p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 31/01/00   Time: 11:22a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:17p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 20/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 14/01/00   Time: 2:37p
 * Updated in $/Meccano/Stage 1/Code
 * Fixing the Dialog boxes at the moment
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 5/01/00    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished the hopup without decking adds handrail
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12 (Beta13)
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 23/12/99   Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 23/12/99   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to move the move standards operation from the Bay::operator= to
 * the  
 * SetBayLength operation
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 23/12/99   Time: 9:26a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 13/12/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix is now correct!
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:06a
 * Updated in $/Meccano/Stage 1/Code
 * fixing up the chain link delete problem
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 9/12/99    Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 28/10/99   Time: 3:53p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Shade cloth and chain link now operational!
 * 2) Crash on setting TWH fixed
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 22/10/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Birdcaging pointers now fully operational
 * 2) Birdcaging now uses pascal's triangle for End of runs
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 19/10/99   Time: 9:03a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on Birdcaging
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 15/10/99   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:25p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 6/10/99    Time: 9:54a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards now correct
 * 2) Lapboards now move with 3D comps
 * 3) Delete Standards Arranginement now working
 * 4) Bay offset within sloped runs
 * 5) RL's working correctly
 * 6) Summary info now working
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 5/10/99    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 5/10/99    Time: 2:41p
 * Updated in $/Meccano/Stage 1/Code
 * Working on Bug 296
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 5/10/99    Time: 1:35p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished SummaryInfo class
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 5/10/99    Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Dar          Date: 4/10/99    Time: 2:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 1/10/99    Time: 1:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Dar          Date: 29/09/99   Time: 2:45p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:05a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 69  *****************
 * User: Dar          Date: 9/15/99    Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 15/09/99   Time: 12:15p
 * Updated in $/Meccano/Stage 1/Code
 * Saving now:
 * 1) Does not delete the Entities, prior to saving
 * Loading now:
 * 1) Deletes all entities that are not on one of the specified layers, so
 * we can now export all data to non-meccano people, as simple lines, etc.
 * 
 * *****************  Version 67  *****************
 * User: Dar          Date: 9/15/99    Time: 9:17a
 * Updated in $/Meccano/Stage 1/Code
 * add create schematic in serialize
 * 
 * *****************  Version 66  *****************
 * User: Dar          Date: 9/15/99    Time: 8:50a
 * Updated in $/Meccano/Stage 1/Code
 * fixed iNumber << to >> serialize fault
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 13/09/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 9/09/99    Time: 1:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 62  *****************
 * User: Dar          Date: 9/09/99    Time: 12:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 9/09/99    Time: 7:54a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 59  *****************
 * User: Dar          Date: 9/08/99    Time: 3:31p
 * Updated in $/Meccano/Stage 1/Code
 * adding more serialize stuff
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 56  *****************
 * User: Dar          Date: 9/07/99    Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 7/09/99    Time: 3:30p
 * Updated in $/Meccano/Stage 1/Code
 * 1) plot line styles
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 9/01/99    Time: 9:22a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on the cleanup operation
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Dar          Date: 8/27/99    Time: 2:45p
 * Updated in $/Meccano/Stage 1/Code
 * fixed lift details not being displayed after autobuild twice
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/23/99    Time: 5:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/23/99    Time: 11:43a
 * Updated in $/Meccano/Stage 1/Code
 * Insert Lift function completed
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/23/99    Time: 9:48a
 * Updated in $/Meccano/Stage 1/Code
 * Delete (so far)  is now working correctly
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Dar          Date: 8/19/99    Time: 5:01p
 * Updated in $/Meccano/Stage 1/Code
 * added displayliftlistdialog
 * 
 * *****************  Version 30  *****************
 * User: Dar          Date: 8/19/99    Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/19/99    Time: 2:55p
 * Updated in $/Meccano/Stage 1/Code
 * fixed the soleboard size error
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/19/99    Time: 1:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Dar          Date: 8/19/99    Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/19/99    Time: 10:44a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Dar          Date: 8/16/99    Time: 1:22p
 * Updated in $/Meccano/Stage 1/Code
 * added some protected var _taccess functions
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Dar          Date: 8/16/99    Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 7:20p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the layers, visibility of autobuild, tight fit bay working
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/12/99    Time: 1:15p
 * Updated in $/Meccano/Stage 1/Code
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
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 29/07/99   Time: 12:18
 * Updated in $/Meccano/Stage 1/Code
 * Completing functionality for Controller class
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 28/07/99   Time: 15:55
 * Updated in $/Meccano/Stage 1/Code
 * Continueing with Run class
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 27/07/99   Time: 14:44
 * Updated in $/Meccano/Stage 1/Code
 * Added the Controller class framework
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 27/07/99   Time: 12:18
 * Updated in $/Meccano/Stage 1/Code
 * I can finally compile with only 8 warnings
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 15/07/99   Time: 11:43
 * Updated in $/Meccano/Stage 1/Code
 * MFC should now be working via stdafx.h
 *
 *******************************************************************************/




void Bay::CopyBracingSmart( SideOfBayEnum eSide, const Bay *pOriginalBay )
{
	Component *pComponent;

	//does the original bay have any bracing on this side?
	pComponent = pOriginalBay->m_caComponents.GetComponent( eSide, 0, CT_BRACING );
	if( pComponent!=NULL )
		CreateBracingArrangment( eSide, pComponent->GetMaterialType() );
}

void Bay::GetBracingDimensions(Bay *pNeighboringBay, CornerOfBayEnum Cnr1, CornerOfBayEnum Cnr2, double daHeights[4] )
{
	daHeights[0] = GetHeightOfStandards( Cnr1 );
	daHeights[1] = GetStandardPosition( Cnr1 ).z;

	if( pNeighboringBay!=NULL )
	{
		daHeights[2] = pNeighboringBay->GetHeightOfStandards( Cnr1 );
		daHeights[3] = pNeighboringBay->GetStandardPosition( Cnr1 ).z;
	}
	else
	{
		daHeights[2] = GetHeightOfStandards( Cnr2 );
		daHeights[3] = GetStandardPosition( Cnr2 ).z;
	}
}

bool Bay::DeleteAllComponentsFromSide(SideOfBayEnum eSide)
{
	int		i, iSize;
	Lift	*pLift;

	///////////////////////////////////////////////////////////////
	//Remove from the Lift
	iSize = GetNumberOfLifts();
	for( i=0; i<iSize; i++ )
	{
		pLift = GetLift( i );
		pLift->DeleteAllComponentsFromSide(eSide);
	}

	///////////////////////////////////////////////////////////////
	//Delete from the Bay
	switch( eSide )
	{
	case( NORTH ):
		//DeleteStandards( CNR_NORTH_EAST );
		//DeleteStandards( CNR_NORTH_WEST );
		DeleteTieFromSide(NORTH_NORTH_WEST);
		DeleteTieFromSide(NORTH_NORTH_EAST);
		break;
	case( EAST ):
		//DeleteStandards( CNR_NORTH_EAST );
		//DeleteStandards( CNR_SOUTH_EAST );
		DeleteTieFromSide(EAST_NORTH_EAST);
		DeleteTieFromSide(EAST_SOUTH_EAST);
		break;
	case( SOUTH ):
		DeleteStandards( CNR_SOUTH_WEST );
		DeleteStandards( CNR_SOUTH_EAST );
		DeleteTieFromSide(SOUTH_SOUTH_EAST);
		DeleteTieFromSide(SOUTH_SOUTH_WEST);
		break;
	case( WEST ):
		DeleteStandards( CNR_SOUTH_WEST );
		DeleteStandards( CNR_NORTH_WEST );
		DeleteTieFromSide(WEST_SOUTH_WEST);
		DeleteTieFromSide(WEST_NORTH_WEST);
		break;
	default:
		assert(false);
		break;
	}
	DeleteBrace(eSide);
	DeleteChainlinkFromSide(eSide);
	DeleteShadeClothFromSide(eSide);

//	CreateSchematic( );

	return true;
}

bool Bay::DeleteAllComponentsOfTypeFromSide(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise/*=LIFT_RISE_INVALID*/ )
{
	int		i;
	bool	bReturn;
	Lift	*pLift;

	#ifdef _DEBUG
	switch( eType )
	{
	///////////////////////////////////////////////////////
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
	case( CT_LADDER_PUTLOG ):
		assert( eSide==SOUTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_MID_RAIL ):
	case( CT_BRACING ):
	case( CT_TIE_BAR ):
	case( CT_STANDARD_CONNECTOR ):
		assert( eSide>=NORTH && eSide<=WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_TOE_BOARD ):
		assert( eSide==SOUTH || eSide==NORTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_TIE_TUBE ):
	case( CT_TIE_CLAMP_COLUMN ):
	case( CT_TIE_CLAMP_MASONARY ):
	case( CT_TIE_CLAMP_YOKE ):
	case( CT_TIE_CLAMP_90DEGREE ):
	case( CT_STAIR ):
	case( CT_LADDER ):
	case( CT_TEXT ):
	case( CT_STAIR_RAIL ):
	case( CT_STAIR_RAIL_STOPEND ):
	case( CT_TOE_BOARD_CLIP ):
	case( CT_PUTLOG_CLIP ):
	default:
		break;

	///////////////////////////////////////////////////////
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
	case( CT_CORNER_STAGE_BOARD ):
		assert( eSide>=NORTH_EAST && eSide<=SOUTH_WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_HOPUP_BRACKET ):
		assert( eSide>=NORTH_NORTH_EAST && eSide<=NORTH_NORTH_WEST );
		break;
	}
	#endif	//#ifdef _DEBUG

	bReturn = false;
	switch( eType )
	{
	case( CT_TOE_BOARD ):
		//There are no eastern or western toeboards
		if( eSide==EAST || eSide==WEST )
			break;
		//fallthrough
	case( CT_DECKING_PLANK ):		//fallthrough
	case( CT_LAPBOARD ):			//fallthrough
	case( CT_STAGE_BOARD ):			//fallthrough
	case( CT_LEDGER ):				//fallthrough
	case( CT_TRANSOM ):				//fallthrough
	case( CT_MESH_GUARD ):			//fallthrough
	case( CT_RAIL ):				//fallthrough
	case( CT_MID_RAIL ):			//fallthrough
	case( CT_STAIR ):				//fallthrough
	case( CT_LADDER ):				//fallthrough
	case( CT_HOPUP_BRACKET ):		//fallthrough
	case( CT_CORNER_STAGE_BOARD ):
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			bReturn = pLift->DeleteAllComponentsOfTypeFromSide( eType, eSide, iRise );
		}
		break;
	case( CT_CHAIN_LINK ):			//fallthrough
	case( CT_SHADE_CLOTH ):			//fallthrough
	case( CT_BRACING ):				//fallthrough
	case( CT_STANDARD ):			//fallthrough
	case( CT_STANDARD_OPENEND ):	//fallthrough
	case( CT_JACK ):				//fallthrough
	case( CT_TIE_TUBE ):			//fallthrough
	case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
	case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
	case( CT_TIE_CLAMP_YOKE ):		//fallthrough
	case( CT_TIE_CLAMP_90DEGREE ):	//fallthrough
	case( CT_SOLEBOARD ):
		bReturn = DeleteAllBayComponentsOfTypeFromSide( eType, eSide, iRise );
		break;
	default:
		assert( false );
		break;
	}
	return bReturn;
}

bool Bay::DeleteAllBayComponentsOfTypeFromSide(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise/*=LIFT_RISE_INVALID*/)
{
	int			i;
	bool		bRemoved;
	Component	*pComponent;

	bRemoved = false;
	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComponent = m_caComponents.GetComponent( i );
		if( (pComponent->GetType()==eType) &&
			(GetPosition(i)==eSide) )
		{
			if( iRise>=0 && pComponent->GetRise()!=(LiftRiseEnum)iRise )
				continue;

			DeleteBayComponent(i);
			i--;	//???JSB todo 990901 - is this required?
			bRemoved = true;
		}
	}


	//remove from the schematic representation!
	switch( eType )
	{
	case( CT_CHAIN_LINK ):
		GetTemplate()->SetChainMeshSide( eSide, false );
		break;
	case( CT_SHADE_CLOTH ):
		GetTemplate()->SetShadeClothSide( eSide, false );
		break;
	case( CT_TIE_TUBE ):
		switch( eSide )
		{
		case( ALL_SIDES ):
			GetTemplate()->SetNNETie( false );
			GetTemplate()->SetENETie( false );
			GetTemplate()->SetESETie( false );
			GetTemplate()->SetSSETie( false );
			GetTemplate()->SetSSWTie( false );
			GetTemplate()->SetWSWTie( false );
			GetTemplate()->SetWNWTie( false );
			GetTemplate()->SetNNWTie( false );
			GetTemplate()->RemoveTieTubeTemplate( ALL_SIDES );
			break;
		case( ALL_VISIBLE_SIDES ):
			if( GetOuter()==NULL )
			{
				GetTemplate()->SetNNWTie( false );
				GetTemplate()->SetNNETie( false );
				GetTemplate()->RemoveTieTubeTemplate( NW );
				GetTemplate()->RemoveTieTubeTemplate( NE );
			}
			if( GetForward()==NULL )
			{
				GetTemplate()->SetENETie( false );
				GetTemplate()->SetESETie( false );
				GetTemplate()->RemoveTieTubeTemplate( NE );
				GetTemplate()->RemoveTieTubeTemplate( SE );
			}
			if( GetBackward()==NULL )
			{
				GetTemplate()->SetWSWTie( false );
				GetTemplate()->SetWNWTie( false );
				GetTemplate()->RemoveTieTubeTemplate( SE );
				GetTemplate()->RemoveTieTubeTemplate( SW );
			}
			if( GetInner()==NULL )
			{
				GetTemplate()->SetSSETie( false );
				GetTemplate()->SetSSWTie( false );
				GetTemplate()->RemoveTieTubeTemplate( SW );
				GetTemplate()->RemoveTieTubeTemplate( NW );
			}
			break;
		case( NE ):
			GetTemplate()->RemoveTieTubeTemplate( NE );
			break;
		case( SE ):
			GetTemplate()->RemoveTieTubeTemplate( SE );
			break;
		case( SW ):
			GetTemplate()->RemoveTieTubeTemplate( SW );
			break;
		case( NW ):
			GetTemplate()->RemoveTieTubeTemplate( NW );
			break;
		case( NNE ):
			GetTemplate()->SetNNETie( false );
			break;
		case( ENE ):
			GetTemplate()->SetENETie( false );
			break;
		case( ESE ):
			GetTemplate()->SetESETie( false );
			break;
		case( SSE ):
			GetTemplate()->SetSSETie( false );
			break;
		case( SSW ):
			GetTemplate()->SetSSWTie( false );
			break;
		case( WSW ):
			GetTemplate()->SetWSWTie( false );
			break;
		case( WNW ):
			GetTemplate()->SetWNWTie( false );
			break;
		case( NNW ):
			GetTemplate()->SetNNWTie( false );
			break;
		default:
			assert( false );
			break;
		}
		break;
	case( CT_BRACING ):
		switch( eSide )
		{
		case( NORTH ):
			GetTemplate()->SetNBrace( false );
			break;
		case( EAST ):
			GetTemplate()->SetEBrace( false );
			break;
		case( SOUTH ):
			GetTemplate()->SetSBrace( false );
			break;
		case( WEST ):
			GetTemplate()->SetWBrace( false );
			break;
		default:
			assert( false );
			break;
		}
		break;
	default:
		//Fine! do nothing
		break;
	}

	return bRemoved;
}


bool Bay::DeleteChainlinkFromSide(SideOfBayEnum eSide)
{
	return DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, eSide);
}

bool Bay::DeleteShadeClothFromSide(SideOfBayEnum eSide)
{
	return DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, eSide);
}

bool Bay::DeleteTieFromSide(SideOfBayEnum eSide)
{
	bool bReturn;

	bReturn = true;
	if( !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_TUBE,				eSide ) )	bReturn = false;
	if( !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_COLUMN,		eSide ) )	bReturn = false;
	if( !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_MASONARY,	eSide ) )	bReturn = false;
	if( !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_YOKE,		eSide ) )	bReturn = false;
	if( !DeleteAllBayComponentsOfTypeFromSide( CT_TIE_CLAMP_90DEGREE,	eSide ) )	bReturn = false;

	return bReturn;
}

BayTemplate * Bay::GetTemplate() const
{
	return m_pBTplt;
}

void Bay::SetTemplate(BayTemplate *pTemplate)
{
	if( pTemplate==NULL )
	{
		assert( false );
	}
	m_pBTplt = pTemplate;
}

bool Bay::DoesABayComponentExistOnASide(ComponentTypeEnum eType, SideOfBayEnum eSide) const
{
	return HasBayComponentOfTypeOnSide( eType, eSide );
}

Controller * Bay::GetController() const
{
	if( GetRunPointer()!=NULL )
	{
		return GetRunPointer()->GetController();
	}
	if( GetBayType()==BAY_TYPE_LAPBOARD && m_pControllerForLapboard!=NULL )
	{
		return m_pControllerForLapboard;
	}

	assert( gpController!=NULL );
	return gpController;
}

ComponentDetailsArray * Bay::GetCompDetails() const
{
	return GetController()->GetCompDetails();
}

void Bay::UpdateSchematicView()
{
  //don't redraw if it is to be deleted, or it is dirty
	if( IsDrawAllowed()
		&& GetDirtyFlag()==DF_CLEAN
		&& !GetController()->IsDestructingController() )
	{
  	assert( GetRunPointer()!=NULL );
		assert( GetController()!=NULL );
		if( GetController()->GetRunID( GetRunPointer() )>=0 )
		{
  		assert( GetTemplate()!=NULL );
			GetTemplate()->UpdateSchematicView();
			MoveSchematic( GetRunPointer()->GetSchematicTransform(), false );
		}
#ifdef SHOW_AUTOBUILD_BAY
		else
		{
			assert( GetTemplate()!=NULL );
			GetTemplate()->UpdateSchematicView();
			MoveSchematic( GetRunPointer()->GetSchematicTransform(), false );
		}
#endif	//#ifdef SHOW_AUTOBUILD_BAY
	}
}


///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void Bay::Serialize(CArchive &ar)
{
	int				iStandardsFit, i, iCompID, iNumberOfComponents,
					iComponentNumber, iRise, iSideOfBay, iNumberOfLifts,
					iLiftNumber, iRoofProtection;
	Bay				*pBay;
	BOOL			BTemp; // for bool conversion macros
	bool			bUse, bHas, bDummy;
	Lift			*pLift;
	double			dRL, dLength, dWidth, dDummy;
	CString			sMsg, sTemp;
	Component		*pComponent;
	SideOfBayEnum	eSideOfBay;

	if (ar.IsStoring())    // Store Object?
	{
		ar << BAY_VERSION_LATEST;

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_8 :
		dDummy = GetStarSeparation();
		ar << dDummy;

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_7 :
		iRoofProtection = (int)GetRoofProtection();
		ar << iRoofProtection;

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_6 :
		bHas = (m_pUndoMillsBay!=NULL);
		STORE_bool_IN_AR( bHas );
		if( bHas )
		{
			m_pUndoMillsBay->Serialize(ar);
		}
		ar << m_mtCanBeMillsCnrBay;

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_5 :
		bUse = IsSydneyCornerBay();
		STORE_bool_IN_AR( bUse );
		bDummy = false;
		STORE_bool_IN_AR( bDummy );

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_4 :
		bUse = HasMovingDeck();
		STORE_bool_IN_AR( bUse );

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_3 :
		bUse = GetUseMidrailWithChainMesh();
		STORE_bool_IN_AR( bUse );
		bUse = GetUseLedgerEveryMetre();
		STORE_bool_IN_AR( bUse );

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_2 :
		STORE_bool_IN_AR(m_bBaySizeMultipleSelection);
		STORE_bool_IN_AR(false);
		ar << m_dTieTubeSeparation;
		STORE_bool_IN_AR(m_bBayRecentlyMoved);
		ar << (int)m_BayType;
		ar << m_dPreviousLength;
		ar << m_dPreviousWidth;

		if( GetInner()!=NULL )
		{
			ar << GetInner()->GetRunPointer()->GetRunID();		//South
			ar << GetInner()->GetID();		//South
		}
		else
		{
			ar << ID_INVALID;
		}
		if( GetOuter()!=NULL )
		{
			ar << GetOuter()->GetRunPointer()->GetRunID();		//South
			ar << GetOuter()->GetID();		//South
		}
		else
		{
			ar << ID_INVALID;
		}
		ar << (int)GetDirtyFlag();

		//////////////////////////////////////////////////////
		//BAY_VERSION_1_0_0 :
		GetTemplate()->Serialize(ar);	//The Bay template
		
		m_Transform.Serialize(ar);
		ar << GetPreviousLength();
		ar << GetPreviousWidth();

		ar << m_iBayID;		//Unique id within run

		if( m_BayType!=BAY_TYPE_LAPBOARD )
		{
			// Lift List Storage - These are the Lifts that are owned by this bay
			iNumberOfLifts = m_LiftList.GetSize();		// store number of Lifts
			ar << iNumberOfLifts;
			if (iNumberOfLifts > 0)
			{
				for( iLiftNumber=0; iLiftNumber < iNumberOfLifts; iLiftNumber++ )
				{
					dRL = m_LiftList.GetRL(iLiftNumber);
					pLift = m_LiftList.GetLift(iLiftNumber);
					pLift->SetBayPointer(this);
					ar << dRL;
					pLift->Serialize(ar);				// store each Lift
				}
			}
			// End Lift List Storage

			// Component List Storage - These are the components that are owned by this bay
			iNumberOfComponents = GetNumberOfBayComponents();			// store number of components
			ar << iNumberOfComponents;
			for( iComponentNumber=0; iComponentNumber<iNumberOfComponents; iComponentNumber++ )
			{
				iRise		= GetComponentsRise(iComponentNumber); 
				eSideOfBay	= GetPosition(iComponentNumber);
				iSideOfBay = eSideOfBay;
				pComponent	= m_caComponents.GetComponent(iComponentNumber);

				ar << iRise;
				ar << iSideOfBay;			//SideOfBayEnum		
				pComponent->Serialize(ar);				// store each component
			}
			// End Component List Storage
		}

		for (i=0; i < 4; i++)
			ar << m_daHeights[i];

		for (i=0; i < 4; i++)
		{
			ar << m_ptaStandardsRL[i].x;
			ar << m_ptaStandardsRL[i].y;
			ar << m_ptaStandardsRL[i].z;
		}

		STORE_bool_IN_AR(m_bStaggeredHeights);

		iStandardsFit = m_eStandardsFit;
		ar << iStandardsFit;					// StandardFitEnum		 
		
		ar << m_dBayLength;
		ar << m_dBayWidth;
	}
	else					// or Load Object?
	{
		int iEnum;

		//set default values
		m_bBaySizeMultipleSelection = false;
		m_iaLapboardCrossesSide.RemoveAll();	//this is reacreated by the lapboard's serialize
		m_dTieTubeSeparation		= DEFAULT_TIE_VERTICAL_SPACING;
		m_bBayRecentlyMoved			= false;
		assert( gpController!=NULL );
		m_pControllerForLapboard	= gpController;
		m_BayType					= BAY_TYPE_BAY;
		m_dPreviousLength			= 0.00;
		m_dPreviousWidth			= 0.00;
		SetInnerDumb(NULL);
		SetOuterDumb(NULL);
		m_pLapboards->RemoveAll();	//CornerForward
		SetUseMidrailWithChainMesh(false);
		SetUseLedgerEveryMetre(false);
		SetHasMovingDeck(false);
		SetSydneyCornerBay( false );
		SetMillsSystemBay( MILLS_TYPE_NONE );
		m_pUndoMillsBay = NULL;
		SetRoofProtection( RP_NONE );
		SetStarSeparation(STAR_SEPARATION);

		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case BAY_VERSION_1_0_8 :
			ar >> dDummy;
			assert( dDummy==STAR_SEPARATION_MILLS ||
					dDummy==STAR_SEPARATION_KWIKSTAGE );
			SetStarSeparation(dDummy);
			//fallthrough

		case BAY_VERSION_1_0_7 :
			ar >> iRoofProtection;
			SetRoofProtection( (RoofProtectionEnum)iRoofProtection );
			//fallthrough

		case BAY_VERSION_1_0_6 :
			LOAD_bool_IN_AR( bHas );
			if( bHas )
			{
				m_pUndoMillsBay = new Bay();
				m_pUndoMillsBay->SetRunPointer(GetRunPointer());
				m_pUndoMillsBay->Serialize(ar);
			}
			ar >> m_mtCanBeMillsCnrBay;
			//fallthrough

		case BAY_VERSION_1_0_5 :
			LOAD_bool_IN_AR( bUse );
			SetSydneyCornerBay( bUse );
			LOAD_bool_IN_AR( bDummy );
			//fallthrough

		case BAY_VERSION_1_0_4 :
			LOAD_bool_IN_AR( bUse );
			SetHasMovingDeck( bUse );
			//fallthrough

		case BAY_VERSION_1_0_3 :
			LOAD_bool_IN_AR( bUse );
			SetUseMidrailWithChainMesh(bUse);
			LOAD_bool_IN_AR( bUse );
			SetUseLedgerEveryMetre(bUse);
			//fallthrough

		case BAY_VERSION_1_0_2 :
			LOAD_bool_IN_AR(m_bBaySizeMultipleSelection);
			LOAD_bool_IN_AR(bDummy);
			ar >> m_dTieTubeSeparation;
			LOAD_bool_IN_AR(m_bBayRecentlyMoved);
			ar >> iEnum;
			m_BayType = (BayTypeEnum)iEnum;
			ar >> m_dPreviousLength;
			ar >> m_dPreviousWidth;

			ar >> m_iLoadedInnerRunID;
			if( m_iLoadedInnerRunID!=ID_INVALID )
			{
				ar >> m_iLoadedInnerBayID;
				if( (m_iLoadedInnerRunID>=0 && 
					 GetController()->GetNumberOfRuns()>m_iLoadedInnerRunID ) && 
					(m_iLoadedInnerBayID>=0 &&
					 GetController()->GetRun(m_iLoadedInnerRunID)->GetNumberOfBays()>m_iLoadedInnerBayID ) )
				{
					pBay = GetController()->GetRun(m_iLoadedInnerRunID)->GetBay( m_iLoadedInnerBayID );
					assert( pBay!=NULL );
					SetInnerDumb( pBay );
					pBay->SetOuterDumb( this );
				}
				else
				{
					//It is possible that this run has not be unserialized
					//	yet, inwhich case this is not an error! So just store
					//	the values, and we will try again during GetInner()
					SetInnerDumb(NULL);
				}
			}
			ar >> m_iLoadedOuterRunID;
			if( m_iLoadedOuterRunID!=ID_INVALID )
			{
				ar >> m_iLoadedOuterBayID;
				if( (m_iLoadedOuterRunID>=0 && 
					 GetController()->GetNumberOfRuns()>m_iLoadedOuterRunID ) && 
					(m_iLoadedOuterBayID>=0 &&
					 GetController()->GetRun(m_iLoadedOuterRunID)->GetNumberOfBays()>m_iLoadedOuterBayID ) )
				{
					pBay = GetController()->GetRun(m_iLoadedOuterRunID)->GetBay( m_iLoadedOuterBayID );
					assert( pBay!=NULL );
					SetOuterDumb( pBay );
					pBay->SetInnerDumb( this );
				}
				else
				{
					//It is possible that this run has not be unserialized
					//	yet, inwhich case this is not an error! So just store
					//	the values, and we will try again during GetOuter()
					SetOuterDumb(NULL);
				}
			}
			ar >> iEnum;
			SetDirtyFlag( (DirtyFlagEnum)iEnum );

			GetTemplate()->Serialize(ar);	//The Bay template

			m_Transform.Serialize(ar);

			ar >> dLength;
			SetPreviousLength( dLength );
			ar >> dWidth;
			SetPreviousWidth( dWidth );

			ar >> m_iBayID;		//Unique id within run

			if( m_BayType!=BAY_TYPE_LAPBOARD )
			{
				// Lift List Retrieval - These are the Lifts that are owned by this bay
				ar >> iNumberOfLifts;		// load number of Lifts
				if (iNumberOfLifts > 0)
				{
					for( iLiftNumber=0; iLiftNumber < iNumberOfLifts; iLiftNumber++ )
					{
						ar >> dRL;

						if( GetController()!=NULL && GetController()->GetConvertToSystemOnNextOpen() )
						{
							int		iTemp;
							double	dTemp, dRemainder;
							dTemp = dRL/GetStarSeparation();
							iTemp = (int)(dTemp+(dTemp>0.00? 0.50: -0.50));
							dRemainder = dTemp-(double)iTemp;
							if( dRemainder>ROUND_ERROR_SMALL || dRemainder<(-1.00*ROUND_ERROR_SMALL) )
							{
								dRL = ConvertRLtoStarRL(dRL, GetStarSeparation());
							}
						}

						pLift = new Lift();
						pLift->SetBayPointer( this );
						m_LiftList.AddLift(pLift, dRL);
						pLift->Serialize(ar);				// store each Lift
					}
				}
				// End Lift List Storage
				
				// Component List Storage - These are the components that are owned by this bay
				ar >> iNumberOfComponents;
				for( iComponentNumber=0; iComponentNumber<iNumberOfComponents; iComponentNumber++ )
				{
					ar >> iRise;
					ar >> iSideOfBay;				//SideOfBayEnum
					pComponent = new Component();
					pComponent->Serialize(ar);				// store each component
					iCompID = m_caComponents.AddComponent((SideOfBayEnum)iSideOfBay, iRise, pComponent);
					pComponent->SetID(iCompID);
					pComponent->SetBayPointer(this);
				}
				// End Component List Storage
			}

			for (i=0; i < 4; i++)
				ar >> m_daHeights[i];

			for (i=0; i < 4; i++)
			{
				ar >> m_ptaStandardsRL[i].x;
				ar >> m_ptaStandardsRL[i].y;
				ar >> m_ptaStandardsRL[i].z;
			}

			LOAD_bool_IN_AR(m_bStaggeredHeights);

			ar >> iStandardsFit;				// StandardFitEnum		 
			m_eStandardsFit = (StandardFitEnum)iStandardsFit;

			ar >> m_dBayLength;
			ar >> m_dBayWidth;

			// called last
/*			if( (GetController()->GetRunID( GetRunPointer() )!=ID_NONE_MATCHING) ||
				(GetController()->GetRunID( GetRunPointer() )!=ID_NO_IDS) )
*/			if( GetController()->GetRunID( GetRunPointer() )>=0 && GetID()>=0 )
			{
				UpdateSchematicView();
			}
			break;

		case BAY_VERSION_1_0_1 :
			LOAD_bool_IN_AR(m_bBaySizeMultipleSelection);
			LOAD_bool_IN_AR(bDummy);
			ar >> m_dTieTubeSeparation;
			LOAD_bool_IN_AR(m_bBayRecentlyMoved);
			ar >> iEnum;
			m_BayType = (BayTypeEnum)iEnum;
			ar >> m_dPreviousLength;
			ar >> m_dPreviousWidth;

			ar >> m_iLoadedInnerRunID;
			if( m_iLoadedInnerRunID!=ID_INVALID )
			{
				ar >> m_iLoadedInnerBayID;
				if( (m_iLoadedInnerRunID>=0 && 
					 GetController()->GetNumberOfRuns()>m_iLoadedInnerRunID ) && 
					(m_iLoadedInnerBayID>=0 &&
					 GetController()->GetRun(m_iLoadedInnerRunID)->GetNumberOfBays()>m_iLoadedInnerBayID ) )
				{
					SetInnerDumb(GetController()->GetRun(m_iLoadedInnerRunID)->GetBay( m_iLoadedInnerBayID ));
					m_iLoadedInnerRunID=ID_INVALID;
					m_iLoadedInnerBayID=ID_INVALID;
				}
				else
				{
					//It is possible that this run has not be unserialized
					//	yet, inwhich case this is not an error! So just store
					//	the values, and we will try again during GetInner()
					SetInnerDumb(NULL);
				}
			}
			ar >> m_iLoadedOuterRunID;
			if( m_iLoadedOuterRunID!=ID_INVALID )
			{
				ar >> m_iLoadedOuterBayID;
				if( (m_iLoadedOuterRunID>=0 && 
					 GetController()->GetNumberOfRuns()>m_iLoadedOuterRunID ) && 
					(m_iLoadedOuterBayID>=0 &&
					 GetController()->GetRun(m_iLoadedOuterRunID)->GetNumberOfBays()>m_iLoadedOuterBayID ) )
				{
					SetOuterDumb(GetController()->GetRun(m_iLoadedOuterRunID)->GetBay( m_iLoadedOuterBayID ));
					m_iLoadedOuterRunID=ID_INVALID;
					m_iLoadedOuterBayID=ID_INVALID;
				}
				else
				{
					//It is possible that this run has not be unserialized
					//	yet, inwhich case this is not an error! So just store
					//	the values, and we will try again during GetOuter()
					SetOuterDumb(NULL);
				}
			}
			ar >> iEnum;
			SetDirtyFlag( (DirtyFlagEnum)iEnum );

			//fallthrough
		case BAY_VERSION_1_0_0 :

			GetTemplate()->Serialize(ar);	//The Bay template
			m_Transform.Serialize(ar);

			ar >> dLength;
			SetPreviousLength( dLength );
			ar >> dWidth;
			SetPreviousWidth( dWidth );

			ar >> m_iBayID;		//Unique id within run

			// Lift List Retrieval - These are the Lifts that are owned by this bay
			ar >> iNumberOfLifts;		// load number of Lifts
			if (iNumberOfLifts > 0)
			{
				for( iLiftNumber=0; iLiftNumber < iNumberOfLifts; iLiftNumber++ )
				{
					ar >> dRL;
					pLift = new Lift();
					pLift->SetBayPointer( this );
					m_LiftList.AddLift(pLift, dRL);
					pLift->Serialize(ar);				// store each Lift
				}
			}
			// End Lift List Storage
			
			// Component List Storage - These are the components that are owned by this bay
			ar >> iNumberOfComponents;
			for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
			{
				ar >> iRise;
				ar >> iSideOfBay;				//SideOfBayEnum

				pComponent = new Component();
				pComponent->Serialize(ar);				// store each component
				iCompID = m_caComponents.AddComponent((SideOfBayEnum)iSideOfBay, iRise, pComponent);
				pComponent->SetID(iCompID);
				pComponent->SetBayPointer(this);
			}
			// End Component List Storage

			for (i=0; i < 4; i++)
				ar >> m_daHeights[i];

			for (i=0; i < 4; i++)
			{
				ar >> m_ptaStandardsRL[i].x;
				ar >> m_ptaStandardsRL[i].y;
				ar >> m_ptaStandardsRL[i].z;
			}

			LOAD_bool_IN_AR(m_bStaggeredHeights);

			ar >> iStandardsFit;				// StandardFitEnum		 
			m_eStandardsFit = (StandardFitEnum)iStandardsFit;

			ar >> m_dBayLength;
			ar >> m_dBayWidth;

			// called last
/*			if( (GetController()->GetRunID( GetRunPointer() )!=ID_NONE_MATCHING) ||
				(GetController()->GetRunID( GetRunPointer() )!=ID_NO_IDS) )
*/			if( GetController()->GetRunID( GetRunPointer() )>=0 )
			{
				UpdateSchematicView();
			}
			break;
		default:
			assert( false );
			if( uiVersion>BAY_VERSION_LATEST )
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
      sMsg+= _T("Class: Bay.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), BAY_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

bool Bay::AreArrangementsSame(doubleArray &da1, doubleArray &da2) const
{
	int i;

	/////////////////////////////////////////////
	//They should be the same size
	if( da1.GetSize()!=da2.GetSize() )
		return false;

	/////////////////////////////////////////////
	//The contents must be the same
	for( i=0; i<da2.GetSize(); i++ )
	{
		if( da1.GetAt(i)<da2.GetAt(i)-ROUND_ERROR || 
			da1.GetAt(i)>da2.GetAt(i)+ROUND_ERROR )
			return false;
	}

	/////////////////////////////////////////////
	return true;
}

bool Bay::AreArrangementsSame(intArray &ia1, intArray &ia2) const
{
	int i;

	/////////////////////////////////////////////
	//They should be the same size
	if( ia1.GetSize()!=ia2.GetSize() )
		return false;

	/////////////////////////////////////////////
	//The contents must be the same
	for( i=0; i<ia2.GetSize(); i++ )
	{
		if( ia1.GetAt(i)!=ia2.GetAt(i) )
			return false;
	}

	/////////////////////////////////////////////
	return true;
}


int Bay::GetNumberOfPlanksOfStageFromWall(SideOfBayEnum eSide)
{
	if( GetController()->IsWallOffsetFromLowestHopup() )
		return GetNumberOfPlanksOfBottomStage(eSide);
	return GetNumberOfPlanksOfLongestStage( eSide );
}

int Bay::GetNumberOfPlanksOfLongestStage(SideOfBayEnum eSide)
{
	double dWidth, dPlankWidth;
	dPlankWidth = GetCompDetails()->GetActualWidth( GetSystem(), CT_STAGE_BOARD, GetBayLength(), MT_TIMBER );
	dWidth = GetWidthOfLongestStage( eSide );
	if( dPlankWidth<=0 )
		return 0;
	return (int)(dWidth/dPlankWidth);
}

int Bay::GetNumberOfPlanksOfBottomStage(SideOfBayEnum eSide)
{
	double dWidth, dPlankWidth;
	dPlankWidth = GetCompDetails()->GetActualWidth( GetSystem(), CT_STAGE_BOARD, GetBayLength(), MT_TIMBER );
	dWidth = GetWidthOfBottomStage( eSide );
	if( dPlankWidth<=0 )
		return 0;
	return (int)(dWidth/dPlankWidth);
}

double Bay::GetWidthOfStageFromWall(SideOfBayEnum eSide)
{
	if( GetController()->IsWallOffsetFromLowestHopup() )
		return GetWidthOfBottomStage(eSide);
	return GetWidthOfLongestStage( eSide );
}

double Bay::GetWidthOfLongestStage( SideOfBayEnum eSide )
{
	int			i;
	Lift		*pLift;
	Component	*pComponent;
	double		dLength, dTempLength;

	switch( eSide )
	{
	case( SOUTH ):
		eSide = SOUTH_SOUTH_EAST;
		break;
	case( NORTH ):
		eSide = NORTH_NORTH_EAST;
		break;
	case( EAST ):
		eSide = EAST_NORTH_EAST;
		break;
	case( WEST ):
		eSide = WEST_NORTH_WEST;
		break;
	case( NORTH_EAST ):
	case( SOUTH_EAST ):
	case( NORTH_WEST ):
	case( SOUTH_WEST ):
	default:
		//This value is ambiguous
		assert( false );
		return 0.00;
	case( NORTH_NORTH_EAST ):
	case( EAST_NORTH_EAST ):
	case( EAST_SOUTH_EAST ):
	case( SOUTH_SOUTH_EAST ):
	case( SOUTH_SOUTH_WEST ):
	case( WEST_SOUTH_WEST ):
	case( WEST_NORTH_WEST ):
	case( NORTH_NORTH_WEST ):
		//fine
		break;
	}

	dLength = 0.00;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pComponent = pLift->GetComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSide );
		if( pComponent!=NULL )
		{
			dTempLength = pComponent->GetLengthActual();
			dLength = max( dTempLength, dLength );
		}
	}

	return dLength;
}

double Bay::GetWidthOfBottomStage(SideOfBayEnum eSide)
{
	int			i;
	Lift		*pLift;
	Component	*pComponent;
	double		dLength;

	switch( eSide )
	{
	case( SOUTH ):
		eSide = SOUTH_SOUTH_EAST;
		break;
	case( NORTH ):
		eSide = NORTH_NORTH_EAST;
		break;
	case( EAST ):
		eSide = EAST_NORTH_EAST;
		break;
	case( WEST ):
		eSide = WEST_NORTH_WEST;
		break;
	case( NORTH_EAST ):
	case( SOUTH_EAST ):
	case( NORTH_WEST ):
	case( SOUTH_WEST ):
	default:
		//This value is ambiguous
		assert( false );
		return 0.00;
	case( NORTH_NORTH_EAST ):
	case( EAST_NORTH_EAST ):
	case( EAST_SOUTH_EAST ):
	case( SOUTH_SOUTH_EAST ):
	case( SOUTH_SOUTH_WEST ):
	case( WEST_SOUTH_WEST ):
	case( WEST_NORTH_WEST ):
	case( NORTH_NORTH_WEST ):
		//fine
		break;
	}

	dLength = 0.00;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pComponent = pLift->GetComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSide );
		if( pComponent!=NULL )
		{
			dLength = pComponent->GetLengthActual();
			break;
		}
	}

	return dLength;
}

bool Bay::SetBayType(BayTypeEnum BayType)
{
	int				i;
	Lift			*pLift;
	LiftTypeEnum	LiftType;

	if( GetBayType()==BAY_TYPE_LAPBOARD &&
		(BayType==BAY_TYPE_STAIRS || BayType==BAY_TYPE_LADDER ||
		 BayType==BAY_TYPE_BUTTRESS) )
	{
		//Lapboards don't contain standards
		assert( false );
		return false;
	}

	if( BayType==BAY_TYPE_STAIRS || BayType==BAY_TYPE_LADDER || BayType==BAY_TYPE_BUTTRESS )
		SetHasMovingDeck(false);

	m_BayType = BayType;

	LiftType = ConvertBayTypeToLiftType( BayType );
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pLift->SetLiftType( LiftType );
	}

	return true;
}

BayTypeEnum Bay::GetBayType() const
{
	return m_BayType;
}

void Bay::MoveSchematic(Matrix3D Transform, bool bStore)
{
	assert( GetTemplate()!=NULL );
	GetTemplate()->MoveSchematic( Transform, bStore );
}

Matrix3D Bay::GetSchematicTransform() const
{
	assert( GetTemplate()!=NULL );
	return GetTemplate()->GetSchematicTransform();
}

void Bay::CreateSchematic()
{
	GetTemplate()->CreateSchematic();
}

void Bay::DeleteAllBayComponents()
{
	m_caComponents.DeleteAll();
	assert( m_caComponents.GetSize()==0 );
}

void Bay::DeleteAllLifts()
{
	m_LiftList.DeleteAll();
	assert( m_LiftList.GetSize()==0 );
}

void Bay::DeleteAllComponents()
{
	DeleteAllLifts();
	DeleteAllBayComponents();
}

void Bay::SetController(Controller *pController)
{
	assert( GetBayType()==BAY_TYPE_LAPBOARD );
	m_pControllerForLapboard = pController;
}

bool Bay::ChangeToStairBay()
{
	if( SetBayType( BAY_TYPE_STAIRS ) )
	{
		return ChangeComponentsForStairBay();
	}
	return false;
}

bool Bay::ChangeToLadderBay()
{
	if( SetBayType( BAY_TYPE_LADDER ) )
	{
		return ChangeComponentsForLadderBay();
	}
	return false;
}

bool Bay::ChangeToButtressBay()
{
	if( SetBayType( BAY_TYPE_BUTTRESS ) )
	{
		return ChangeComponentsForButtressBay();
	}
	return false;
}

bool Bay::ChangeComponentsForStairBay()
{
	int		iRequiredNumber;
	bool	bAdjustmentStandards, bHalfBayReqd;
	double	dRLBottom, dRLTop, dRLTopLift, dRLBottomLift, dHeight;

	assert (GetBayType()==BAY_TYPE_STAIRS );

	////////////////////////////////////////////////////////////////
	//1) Find the number of lifts we need
	//1a) Determine how many lifts we can have for the standards
	GetRLsForBayByStandards( dRLBottom, dRLTop );
	//1b) Determine how many lifts we can have according to existing lifts heights

	GetRLsForBayByExistingLifts( dRLBottomLift, dRLTopLift );
	//They are able to step up 500mm, so we only need to match this height
	//dRLTop-= GetStarSeparation();

	dHeight = dRLTop-dRLBottom;
	bAdjustmentStandards = false;
	if( dHeight<GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() ) )
	{
		bAdjustmentStandards = true;
		dHeight = GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() );
	}

	////////////////////////////////////////////////////////////////////
	//How many full 3.0m lifts do we need
	dHeight-= RLAdjust();
	iRequiredNumber = GetNumberOfLiftsForBay( LIFT_RISE_3000MM, dHeight );
	double dGapToTopLift;

	////////////////////////////////////////////////////////////////////
	//Will these lifts reach the top deck?
	bHalfBayReqd = false;
	dGapToTopLift = dHeight-(GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation())*(double)iRequiredNumber );
	if( dGapToTopLift> GetRiseFromRiseEnum(LIFT_RISE_0000MM, GetStarSeparation())+ROUND_ERROR &&
		dGapToTopLift<=GetRiseFromRiseEnum(LIFT_RISE_1500MM, GetStarSeparation() ) )
	{
		///////////////////////////////////////////////////////////////////
		//From the point of view of the standards the Height has changed
		dHeight = GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() )*(double)iRequiredNumber;

		///////////////////////////////////////////////////////////////////
		//We need to add a half stair bay!
		bHalfBayReqd = true;
		iRequiredNumber++;
/*		//dHeight+= GetRiseFromRiseEnum(LIFT_RISE_1500MM, GetStarSeparation() );
		dHeight+= GetRiseFromRiseEnum( GetRiseEnumFromRise(dGapToTopLift, GetStarSeparation()) );


		///////////////////////////////////////////////////////////////////
		//we will need to adjust the standards
		bAdjustmentStandards = true;
*/	}
	else if( dGapToTopLift> GetRiseFromRiseEnum(LIFT_RISE_1500MM, GetStarSeparation() ) &&
			 dGapToTopLift<=GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() ) )
	{
		///////////////////////////////////////////////////////////////////
		//From the point of view of the standards the Height has changed
		dHeight = GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() )*(double)iRequiredNumber;

		///////////////////////////////////////////////////////////////////
		//We need to add a half stair bay!
		bHalfBayReqd = true;
		iRequiredNumber++;

/*		dHeight+= GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() );

		///////////////////////////////////////////////////////////////////
		//we need to add another full lift!
		iRequiredNumber++;

		///////////////////////////////////////////////////////////////////
		//we will need to adjust the standards
		bAdjustmentStandards = true;
*/	}
	else
	{
		//fits perfectly! - nothing to do
		1;
	}

	////////////////////////////////////////////////////////////////
	//2)  Adjust Number of Lifts
	//2a) Delete Unneeded lifts
	//2b) Add extra lifts
	AdjustNumberOfLifts( iRequiredNumber );

	////////////////////////////////////////////////////////////////
	//3a)  Set Rises for lifts
	SetRisesForAllLifts( LIFT_RISE_3000MM );
	if( bHalfBayReqd )
	{
		assert( GetBayType()==BAY_TYPE_STAIRS );
		//set the top lift to be 1.5m
		GetLift( GetNumberOfLifts()-1 )->SetRise( GetRiseEnumFromRise(dGapToTopLift) );
	}

	////////////////////////////////////////////////////////////////
	//3b)  Set RL's for lifts, and move to new RL
	SetRLsForAllLifts( dRLBottom );

	////////////////////////////////////////////////////////////////
	//4)  Delete unsuitable components
	DeleteAllComponentsFromLifts();

	////////////////////////////////////////////////////////////////
	//5)  Add special components only found in ladder and stair bays
	AddSpecialStairsLaddersComponentsToAllLifts();

	////////////////////////////////////////////////////////////////
	//6)  Delete transoms and ledgers according to neighbouring bays
	DeleteAccessObsticles();

	if( bAdjustmentStandards )
	{
		SetStandardToRLs( CNR_NORTH_EAST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_SOUTH_EAST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_SOUTH_WEST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_NORTH_WEST, dRLBottom+dHeight, dRLBottom);
	}

	////////////////////////////////////////////////////////////////
	//The Stairs only need to go the top lift
	double	dLiftHeight;
	dLiftHeight = dRLTopLift-dRLBottomLift;
	dLiftHeight-= GetStarSeparation();
	if( dLiftHeight>GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() ) )
	{
		int			i, iRise;
		Lift		*pLift;
		double		dStairHeight;
		Component	*pComponent;

		dStairHeight = 0.00;
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift(i);
			if( pLift->GetRise()==LIFT_RISE_3000MM )
			{
				dStairHeight+= GetRiseFromRiseEnum( LIFT_RISE_3000MM, GetStarSeparation() );
				iRise = SPR_INNER;
			}
			else if( pLift->GetRise()>=LIFT_RISE_1500MM )
			{
				dStairHeight+= GetRiseFromRiseEnum( LIFT_RISE_1500MM, GetStarSeparation() );
				iRise = SPR_OUTER;
			}

			if( dStairHeight>=dLiftHeight+(2.00*GetStarSeparation())-ROUND_ERROR )
			{
				pComponent = pLift->GetComponent( CT_STAIR, iRise, SOUTH );
				if( pComponent!=NULL )
				{
					pLift->DeleteComponent( pComponent->GetID() );
				}
			}
/*
			if( pLift->GetRise()==LIFT_RISE_3000MM && 
				dStairHeight-GetRiseFromRiseEnum( LIFT_RISE_1500MM, GetStarSeparation() )>dLiftHeight+(2.00*GetStarSeparation())-ROUND_ERROR )
			{
				pComponent = pLift->GetComponent( CT_STAIR, SPR_INNER, SOUTH );
				if( pComponent!=NULL )
				{
					pLift->DeleteComponent( pComponent->GetID() );
				}
			}
*/		}
	}

	BraceExposedSides();

	return true;
}

bool Bay::ChangeComponentsForLadderBay()
{
	bool	bAdjustmentStandards;
	double	dRLBottom, dRLTop, dHeight;

	assert (GetBayType()==BAY_TYPE_LADDER );

	////////////////////////////////////////////////////////////////
	//1) Find the number of lifts we need
	//1a) Determine how many lifts we can have for the standards
	GetRLsForBayByStandards( dRLBottom, dRLTop );

	//1b) Determine how many lifts we can have according to existing lifts heights
	//GetRLsForBayByExistingLifts( dRLBottom, dRLTop );
	dHeight = dRLTop-dRLBottom;
	bAdjustmentStandards = false;
	if( dHeight<GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() ) )
	{
		bAdjustmentStandards = true;
		dHeight = GetRiseFromRiseEnum(LIFT_RISE_3000MM, GetStarSeparation() );
	}

	////////////////////////////////////////////////////////////////////
	//All the lifts where the decks are must remain, the rest can be anything
	//	between 2m and 4m lifts
	//	2.0m Lift = 3.6m ladder
	//	2.5m Lift = 3.6m ladder
	//	3.0m Lift = 4.2m ladder
	//	3.5m Lift = 4.2m ladder
	//	4.0m Lift = 6.0m ladder
	//	4.5m Lift = 6.0m ladder
	int		i;
	int		j, k, iEmptyLifts;
	Lift	*pLift;
	double	dEmptyLiftHeight;

	////////////////////////////////////////////////////////////////
	//4)  Delete unsuitable components, except full decking
	Component *pComp;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		for( j=0; j<pLift->GetNumberOfComponents(); j++ )
		{
			pComp = pLift->GetComponent( j );
			if( pComp->GetType()!=CT_DECKING_PLANK &&
				pComp->GetType()!=CT_MESH_GUARD )
			{
				pLift->DeleteComponent( pComp->GetID() );
			}
		}
	}
	assert( GetNumberOfLifts()>0 );

	int		iDeckLifts, iLifts;
	Lift	*pTempLift;
	double	dLadder, dValue;

	//If the second lift is less than 2.0m tall then the bottom lift cannot be
	//	accessed from the east or west, so it will have to be accessed from the
	//	northern side instead!
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);

		iEmptyLifts = pLift->HowManyEmptyLiftsAboveMe( dEmptyLiftHeight );
		iDeckLifts = pLift->HowManyDeckLiftsAboveMe();
		//one, or more of them must be 0, we can't have both being nonzero!
		assert( iEmptyLifts==0 || iDeckLifts==0 );

		//if bottom lift
		if( i==0 )
		{
			//if deck is above
			if( iDeckLifts>0 )
			{
				//if lift is == 0.5m
				if( pLift->GetRise()==LIFT_RISE_0500MM )
				{
					//if number of decks above>=3
					if( iDeckLifts>=3 )
					{
						//ladder is 6.0m
						dLadder = COMPONENT_LENGTH_6000;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd lift is missing 2 planks
						GetLift(i+1)->CreateSpecialLadderLiftLowLift();
						//3rd lift is missing 2 planks
						GetLift(i+2)->CreateSpecialLadderLiftShortPlank();
						//4th lift is normal
						i+=2;
					}
					//if number of decks above>=2
					else if( iDeckLifts>=2 )
					{
						//ladder is 6.0m
						dLadder = COMPONENT_LENGTH_3600;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd lift is missing 2 planks
						GetLift(i+1)->CreateSpecialLadderLiftLowLift();
						//3rd lift is normal
						i++;
					}
					else	//else
					{
						//number of decks above must ==1
						assert( iDeckLifts==1 );
						//if number of empty lifts above next lift>=1
						iLifts = GetLift(i+1)->HowManyEmptyLiftsAboveMe( dHeight );
						if( iLifts>=1 )
						{
							//if sum all empty lifts above is <= 2.0m
							if( dHeight<=(4*GetStarSeparation())+ROUND_ERROR )
							{
								//if Deck above these empty lifts
								if( GetLift(i+1+iLifts+1)!=NULL && GetLift(i+1+iLifts+1)->IsDeckingLift() )
								{
									//ladder is 6.0m
									dLadder = COMPONENT_LENGTH_6000;
									//this lift has 2 planks
									pLift->CreateSpecialLadderLift2Board(dLadder);
									//2nd lift is missing 2 planks
									GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
									//3rd->Deck lift are empty lifts
									for( j=i+2; j<=(i+1+iLifts); j++ )
									{
										GetLift( j )->CreateSpecialLadderLiftEmpty();
									}
									//Deck lift is normal lift
									i+= 1+iLifts;
								}
								//else
								else
								{
									//must be empty lift at top
									assert( i+1+iLifts==GetNumberOfLifts() );
									//no ladder required
									dLadder=0.00;
									//this lift has 2 planks
									pLift->CreateSpecialLadderLift2Board(dLadder);
									//2nd lift is missing 2 planks
									pLift->CreateSpecialLadderLiftLowLift();
								}
							}
							//else if sum all empty lifts >2.0 && <4.5m
							else if( dHeight>(4*GetStarSeparation())-ROUND_ERROR && dHeight<(9*GetStarSeparation())+ROUND_ERROR )
							{
								//ladder is 4.2m
								dLadder = COMPONENT_LENGTH_4200;
								//this lift has 2 planks
								pLift->CreateSpecialLadderLift2Board( dLadder );
								//2nd lift is missing 2 planks
								GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
								//3rd lift is normal lift
								i++;
							}
							else //else 
							{
								//sum all empty lifts >=4.5m
								assert( dHeight>=4500.00-ROUND_ERROR );
								//find highest lift<=2.5m above 1st empty lift
								dValue = 0.00;
								for( j=i+2; j<=i+1+iLifts; j++ )
								{
									dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
									if( dValue>2500.00 )
									{
										//gone one too far
										j--;
										break;
									}
								}
								assert( j>=i+2 );
								//ladder is 6.0m
								dLadder = COMPONENT_LENGTH_3600;
								//this lift has 2 planks
								pLift->CreateSpecialLadderLift2Board( dLadder );
								//2nd lift is missing 2 planks
								GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
								//3rd lift is an empty lift
								k=j;
								for( ; j>=i+2; j-- )
								{
									GetLift( j )->CreateSpecialLadderLiftEmpty();
								}
								//highest lift found above is normal lift!
								i=k-1;
							}
						}
						else //else
						{
							//no ladder required
							dLadder = 0.00;
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//2nd lift must be top lift
							assert( i+2==GetNumberOfLifts() );
							//2nd lift is missing 2 planks
							GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
							//no ladder! we must be finished
							break;
						}
					}
				}
				//else if lift is == 1.0m or ==1.5m
				else if( pLift->GetRise()==LIFT_RISE_1000MM || pLift->GetRise()==LIFT_RISE_1500MM )
				{
					//if number of decks above>=2
					if( iDeckLifts>=2 )
					{
						//ladder is 4.2m
						dLadder = COMPONENT_LENGTH_4200;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd lift is missing 2 planks
						GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
						//3rd lift is normal
						i++;
					}
					//else
					else
					{
						//number of decks above must ==1
						assert( iDeckLifts==1 );
						//if number of empty lifts above next lift>=1
						iLifts = GetLift( i+1 )->HowManyEmptyLiftsAboveMe( dHeight );
						if( i+1+iLifts==GetNumberOfLifts()-1 )
						{
							//Empty lifts at the top!
							dLadder = 0.00;
							if( pLift->GetRise()==LIFT_RISE_1500MM )
								dLadder = COMPONENT_LENGTH_3600;
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//2nd lift is missing 2 planks
							GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
							break;
						}
						else if( iLifts>=1 )
						{
							//if sum all empty lifts above + this lift rise + 2.0m <= 4.5m
							dHeight+= GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
							dHeight+= (4*GetStarSeparation());
							if( dHeight<=(9*GetStarSeparation())+ROUND_ERROR )
							{
								//if Deck above these empty lifts
								if( GetLift( i+1+iLifts+1 )!=NULL && GetLift( i+1+iLifts+1 )->IsDeckingLift() )
								{
									//ladder is 6.0m
									dLadder = COMPONENT_LENGTH_6000;
									//this lift has 2 planks
									pLift->CreateSpecialLadderLift2Board( dLadder );
									//2nd lift is missing 2 planks
									GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
									//3rd->Deck lift are empty lifts
									for( j=i+1+1; j<=i+1+iLifts; j++ )
									{
										GetLift(j)->CreateSpecialLadderLiftEmpty();
									}
									//Deck lift is normal lift
									i = i+1+iLifts;
								}
								//else
								else
								{
									//must be empty lift at top
									assert( i+1+iLifts+1==GetNumberOfLifts() );
									//**ladder is 3.6m (could this be shorter)
									dLadder = COMPONENT_LENGTH_3600;
									//this lift has 2 planks
									pLift->CreateSpecialLadderLift2Board( dLadder );
									//2nd lift is missing 2 planks
									GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
									break;
								}
							}
							//else if sum all empty lifts + this lift rise + 2.0m ==5.0m
							else if( dHeight>=4500.00+ROUND_ERROR &&
									 dHeight<=5500.00-ROUND_ERROR )
							{
								//if bottom lift is 1.5m
								if( pLift->GetRise()==LIFT_RISE_1500MM )
								{
									//special case - ladder has to sit on the ground
									//ladder is 3.6m
									dLadder = COMPONENT_LENGTH_3600;
									//this lift is and empty lift
									pLift->CreateSpecialLadderLift2Board( dLadder );
									//overwrite the lift with an empty lift
									pLift->CreateSpecialLadderLiftEmpty();
									//2nd lift is a normal lift
								}
								//else
								else
								{
									//ladder is 4.2m
									dLadder = COMPONENT_LENGTH_4200;
									//this lift has 2 planks
									pLift->CreateSpecialLadderLift2Board( dLadder );
									//2nd lift is missing 2 planks
									GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
									//3rd lift is normal lift
									i++;
								}
							}
							//else 
							else
							{
								//sum all empty lifts + this lift rise + 2.0m >=5.5m
								assert( dHeight>=5000.00+ROUND_ERROR );
								//find highest lift<=4.5m above bottom lift
								dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() ) + (4*GetStarSeparation());
								for( j=i+2; j<=i+1+iLifts; j++ )
								{
									dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
									if( dValue>=4500.00 )
									{
										dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
										j--;
										break;
									}
								}
								//ladder is 6.0m or 4.2m or even 3.6m
								if( dValue<4000.00-ROUND_ERROR )
									dLadder = COMPONENT_LENGTH_3600;
								else if( dValue< 4500.00-ROUND_ERROR )
									dLadder = COMPONENT_LENGTH_4200;
								else
									dLadder = COMPONENT_LENGTH_6000;
								//this lift has 2 planks
								pLift->CreateSpecialLadderLift2Board( dLadder );
								//2nd lift is missing 2 planks
								GetLift( i+1 )->CreateSpecialLadderLiftLowLift();
								//3rd -> highest lift is an empty lift
								k=j;
								for( ; j>i+1; j-- )
								{
									GetLift(j)->CreateSpecialLadderLiftEmpty();
								}
								//highest lift found above is normal lift!
								i=k-1;
							}
						}
						//else
						else
						{
							assert( false /*how the hell did this happen?*/ );
							break;
						}
					}
				}
				//else
				else
				{
					//lift must == 2.0m
					assert( pLift->GetRise()==LIFT_RISE_2000MM );
					//if number of decks above >=2
					if( iDeckLifts>=2 )
					{
						//ladder = 6.0m
						dLadder = COMPONENT_LENGTH_6000;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd lift has short planks
						GetLift( i+1 )->CreateSpecialLadderLiftShortPlank();
						//3rd lift is normal lift
						i++;
					}
					//else
					else
					{
						//if Sum all empty lifts above Deck>=2.0m
						iLifts = GetLift( i+1 )->HowManyEmptyLiftsAboveMe( dHeight );
						if( i+1+iLifts==GetNumberOfLifts()-1 )
						{
							//ladder = 3.6m
							dLadder = COMPONENT_LENGTH_3600;
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//2nd lift is normal lift
						}
						else if( iLifts>=1 && dHeight>=(4*GetStarSeparation())-ROUND_ERROR )
						{
							//ladder = 6.0m
							dLadder = COMPONENT_LENGTH_6000;
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//2nd lift has short planks
							GetLift( i+1 )->CreateSpecialLadderLiftShortPlank();
							//3rd lift is normal lift
							i++;
						}
						//else
						else
						{
							//ladder = 3.6m
							dLadder = COMPONENT_LENGTH_3600;
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//2nd lift is normal lift
						}
					}
				}
			}
			else if( iEmptyLifts>0 )	//else empty lift is above
			{
				//if there are no decks above this deck
				if( iEmptyLifts+1==GetNumberOfLifts() )
				{
					//No ladder needed
					dLadder = 0.00;
					//this lift has 2 planks
					pLift->CreateSpecialLadderLift2Board( dLadder );
					//2nd->top lift are empty lifts
					for( j=i+1; j<GetNumberOfLifts(); j++ )
					{
						GetLift( j )->CreateSpecialLadderLiftEmpty();
					}
					break;
				}
				else
				{
					//if sum empty lifts + this lift>=6.5m
					dEmptyLiftHeight+= GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
					if( dEmptyLiftHeight>6500.00+ROUND_ERROR )
					{
						//find top lift <=4.5m
						dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
						for( j=i+1; j<=i+iEmptyLifts; j++ )
						{
							dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
							if( dValue>4500.00 )
							{
								dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
								j--;
								break;
							}
						}
						assert( j>=i+1 );
						//Ladder is 6.0m;
						dLadder = COMPONENT_LENGTH_6000;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd->top lift are empty lifts
						k=j;
						for( ; j>=i+1; j-- )
						{
							GetLift( j )->CreateSpecialLadderLiftEmpty();
						}
						//top lift is normal lift
						i=k;
					}
					//else if sum empty lifts + this lift>=4.5m
					else if( dEmptyLiftHeight>4500.00+ROUND_ERROR )
					{
						//find top lift <=2.5m
						dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
						for( j=i+1; j<=i+iEmptyLifts; j++ )
						{
							dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
							if( dValue>2500.00+ROUND_ERROR )
							{
								dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
								j--;
								break;
							}
						}
						assert( j>=i );
						//Ladder is 3.6m;
						dLadder = COMPONENT_LENGTH_3600;
						//this lift has 2 planks
						pLift->CreateSpecialLadderLift2Board( dLadder );
						//2nd->top lift are empty lifts
						k=j;
						for( ; j>=i+1; j-- )
						{
							GetLift( j )->CreateSpecialLadderLiftEmpty();
						}
						//top lift is normal lift
						i=k;
					}
					//else
					else
					{
						//if Deck above these empty lifts
						if( i+1+iEmptyLifts<GetNumberOfLifts() )
						{
							//if sum empty lifts + this lift<=2.5m
							if( dEmptyLiftHeight<=2500.00+ROUND_ERROR )
							{
								//ladder = 3.6m
								dLadder = COMPONENT_LENGTH_3600;
							}
							else if( dEmptyLiftHeight<=3500.00+ROUND_ERROR )
							{
								//ladder = 4.2m
								dLadder = COMPONENT_LENGTH_4200;
							}
							//else
							else
							{
								//ladder = 6.0m
								dLadder = COMPONENT_LENGTH_6000;
							}
							//this lift has 2 planks
							pLift->CreateSpecialLadderLift2Board( dLadder );
							//all empty lifts are empty
							for( j=i+1; j<=i+iEmptyLifts; j++ )
							{
								GetLift( j )->CreateSpecialLadderLiftEmpty();
							}
							//deck lift is normal lift
							i+= iEmptyLifts;
						}
						//else
						else
						{
							//no deck above empty lifts
							//no ladder
							//why do they need a ladder bay?
							break;
						}
					}
				}
			}
			else //else I am the top lift!
			{
				//no ladder!
				//why do they need a ladder bay?
				break;
			}
		}
		else	//else not bottom lift
		{
			//deck must be >=2.0m above me
			assert( dEmptyLiftHeight>=(4*GetStarSeparation()) || pLift->GetRise()==LIFT_RISE_2000MM );
			//if deck is above
			if( iDeckLifts>0 )
			{
				//I must be 2.0m
				assert( pLift->GetRise()==LIFT_RISE_2000MM );
				//if number of decks above >=2
				if( iDeckLifts>=2 )
				{
					//ladder = 6.0m
					dLadder = COMPONENT_LENGTH_6000;
					//this is normal lift
					pLift->CreateSpecialLadderLiftNormal( dLadder );
					//2nd lift has short planks
					GetLift( i+1 )->CreateSpecialLadderLiftShortPlank();
					//3rd lift is normal lift
					i++;
				}
				//else
				else
				{
					//if Sum all empty lifts above Deck>=2.0m
					iLifts = GetLift( i+1 )->HowManyEmptyLiftsAboveMe( dHeight );
					if( dHeight>=(4*GetStarSeparation()) )
					{
						//find top lift <=4.0m above this lift
						dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
						bool bFound = false;
						for( j=i+1; j<=i+iLifts; j++ )
						{
							dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
							if( dValue>4000.00+ROUND_ERROR )
							{
								bFound = true;
								dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
								j--;
								break;
							}
						}
						assert( j>i+1 );
						if( !bFound )
							j--;
						//ladder = 6.0m
						dLadder = COMPONENT_LENGTH_6000;
						//this is normal lift
						pLift->CreateSpecialLadderLiftNormal( dLadder );
						//2nd lift has short planks
						GetLift( i+1 )->CreateSpecialLadderLiftShortPlank();
						//3rd->top lift = empty lifts
						k=j;
						for( ; j>i+1; j-- )
						{
							GetLift( j )->CreateSpecialLadderLiftEmpty();
						}
						//top lift is normal lift
						i=k-1;
					}
					//else
					else
					{
						//ladder = 3.6m
						dLadder = COMPONENT_LENGTH_3600;
						//this is normal lift
						pLift->CreateSpecialLadderLiftNormal( dLadder );
						//2nd lift is normal lift
					}
				}
			}
			//else if empty lift is above
			else if( iEmptyLifts>0 )
			{
				//if sum empty lifts + this lift>=6.5m
				dEmptyLiftHeight+= GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
				if( dEmptyLiftHeight>=6500.00 )
				{
					//find top lift <=4.5m
					dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
					for( j=i+1; j<i+1+iEmptyLifts; j++ )
					{
						dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
						if( dValue>4500.00+ROUND_ERROR )
						{
							dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
							j--;
							break;
						}
					}
					assert( j>=i+1 );
					dLadder = COMPONENT_LENGTH_6000;
					//this is normal lift
					pLift->CreateSpecialLadderLiftNormal( dLadder );
					//2nd->top lift are empty lifts
					k=j;
					for( ; j>=i+1; j-- )
					{
						GetLift( j )->CreateSpecialLadderLiftEmpty();
					}
					//top lift is normal lift
					i=k;
				}
				//else if sum empty lifts + this lift>=4.5m
				else if( dEmptyLiftHeight>=4500.00 )
				{
					//find top lift <= 4.5m
					dValue = GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
					for( j=i+1; j<=i+iEmptyLifts; j++ )
					{
						dValue+= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
						if( dValue>4500.00+ROUND_ERROR )
						{
							//Gone one too far
							dValue-= GetRiseFromRiseEnum( GetLift(j)->GetRise(), GetStarSeparation() );
							j--;
							break;
						}
					}
					assert( j>=i+1 );
					//Ladder is 6.0m
					dLadder = COMPONENT_LENGTH_6000;
					//this is normal lift
					pLift->CreateSpecialLadderLiftNormal( dLadder );
					//2nd->top lift are empty lifts
					k=j;
					for( ; j>=i+1; j-- )
					{
						GetLift( j )->CreateSpecialLadderLiftEmpty();
					}
					//top lift is normal lift
					i=k;
				}
				//else
				else
				{
					//if Deck above these empty lifts
					pTempLift = GetLift(i+1+iEmptyLifts);
					if( pTempLift!=NULL && pTempLift->IsDeckingLift() )
					{
						//if sum empty lifts + this lift<=2.5m
						if( dEmptyLiftHeight<=2500.00+ROUND_ERROR )
						{
							//ladder = 3.6m
							dLadder = COMPONENT_LENGTH_3600;
						}
						//else if sum empty lifts + this lift<=3.5m
						else if( dEmptyLiftHeight<=3500.00+ROUND_ERROR )
						{
							//ladder = 4.0m
							dLadder = COMPONENT_LENGTH_4200;
						}
						//else
						else
						{
							//ladder = 6.0m
							dLadder = COMPONENT_LENGTH_6000;
						}
						//this is normal lift
						pLift->CreateSpecialLadderLiftNormal( dLadder );
						//all empty lifts are empty
						for( j=(i+1); j<=(i+iEmptyLifts); j++ )
						{
							GetLift(j)->CreateSpecialLadderLiftEmpty();
						}
						//deck lift if normal lift
						i = j-1;
					}
					//else
					else
					{
						//no ladder
						dLadder = 0.00;
						//This is just a normal lift!
						pLift->CreateSpecialLadderLiftNormal( dLadder );
						//no deck above empty lifts
						break;
					}
				}
			}
			//else I am the top lift!
			else
			{
				assert( pLift->GetLiftID()==GetNumberOfLifts()-1 );
				//no ladder!
				dLadder = 0.00;
				pLift->CreateSpecialLadderLiftNormal( dLadder );
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//3b)  Set RL's for lifts, and move to new RL
	SetRLsForAllLifts( dRLBottom );

	////////////////////////////////////////////////////////////////
	//5)  Add special components only found in ladder and stair bays
	AddSpecialStairsLaddersComponentsToAllLifts();

	////////////////////////////////////////////////////////////////
	//6)  Delete transoms and ledgers according to neighbouring bays
	DeleteAccessObsticles();

	//The functions above may mark some of the lifts as requiring
	//	delete, so stop them being deleted
	SetDirtyFlag( DF_CLEAN, true );

	//////////////////////////////////////////////////////////////////
	//We need to remove some obsticles at the bottom of the bay to allow
	//	access to the first ladder, this will normally be on the West,
	//	however it may be elsewhere under certain conditions
	SideOfBayEnum		eSide;
	ComponentTypeEnum	eType;
	int iRise, iLiftId = 0;
	pLift = GetLift(iLiftId);
	assert( pLift!=NULL );

	if( pLift->GetRise()<(int)LIFT_RISE_2000MM &&					/*The bottom lift is a short lift			*/
		GetNumberOfLifts()>1 &&										/*which is not the only lift				*/
		( pLift->HowManyEmptyLiftsAboveMe( dEmptyLiftHeight )<=0 ||	/*It does not have any empty decks above it	*/
		  dEmptyLiftHeight<ConvertRLtoStarRL(COMPONENT_LENGTH_2000, GetStarSeparation())-ROUND_ERROR) )		/*or there is a deck less than 2.0m above	*/
	{
		//We need to remove the first 3 ledgers from the northern side
		eType = CT_LEDGER;
		eSide = NORTH;
		if( GetOuter()!=NULL && GetInner()==NULL )
		{
			//we have an obsticle on the north better use the south
			eSide = SOUTH;
		}
	}
	else
	{
		//We need to remove the first 3 transoms from the western side
		eType = CT_TRANSOM;
		eSide = WEST;
		if( GetBackward()!=NULL && GetForward()==NULL )
		{
			//we have an obsticle on the west better use the east
			eSide = EAST;
		}
	}

	//////////////////////////////////////////////////////////////////
	//Remove the 3 trannies/ledgers that prevent access
	iRise = (int)LIFT_RISE_0500MM;
	for( i=0; i<4; i++ )
	{
		///////////////////////////////////////////////////////////
		//remove transom/ledger
		pComp = pLift->GetComponent( eType, iRise, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove rail
		pComp = pLift->GetComponent( CT_RAIL, iRise, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove Midrail
		pComp = pLift->GetComponent( CT_MID_RAIL, iRise, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove Meshguard
		pComp = pLift->GetComponent( CT_MESH_GUARD, iRise, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove Meshguard from one star up, which would also block access
		pComp = pLift->GetComponent( CT_MESH_GUARD, iRise+1, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove Meshguard from two stars up, which could also block access
		pComp = pLift->GetComponent( CT_MESH_GUARD, iRise+2, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
		}
		///////////////////////////////////////////////////////////
		//remove the bottom Bracing
		DeleteBayComponent( CT_BRACING, eSide, 0 );
		
		///////////////////////////////////////////////////////////
		//move up one star
		iRise++;
		if( iRise>pLift->GetRise() )
		{
			//move up to the next lift!
			iLiftId++;
			pLift = GetLift(iLiftId);
			if( i==2 && pLift->IsDeckingLift() )
			{
				//This is possible in the special case scenario
				iRise = (int)LIFT_RISE_0500MM;
				break;
			}
			else
			{
				iRise = (int)LIFT_RISE_0000MM;
			}
		}
	}

	if( bAdjustmentStandards )
	{
		SetStandardToRLs( CNR_NORTH_EAST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_SOUTH_EAST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_SOUTH_WEST, dRLBottom+dHeight, dRLBottom);
		SetStandardToRLs( CNR_NORTH_WEST, dRLBottom+dHeight, dRLBottom);
	}

	BraceExposedSides();

	return true;
}

bool Bay::ChangeComponentsForButtressBay()
{
	double	dRLBottom, dRLTop, dHeight, dButtressHeight;

	assert (GetBayType()==BAY_TYPE_BUTTRESS );

	////////////////////////////////////////////////////////////////
	//1) Find the number of lifts we need
	//1a) Determine how many lifts we can have for the standards
	GetRLsForBayByStandards( dRLBottom, dRLTop );
	dHeight = dRLTop-dRLBottom;

	//buttress bay are shorter than thier neighbor by the height
	//	of the bracing
	double dBayWidth, dBracingHeight;
	dBayWidth = GetBayWidth();
	//dBracingHeight = GetBracingRaise( dBayWidth, GetBracingLength( dBayWidth ) );
	dBracingHeight = GetBracingRaise( dBayWidth );
	dButtressHeight = dHeight - ConvertRLtoStarRL(dBracingHeight, GetStarSeparation());
	dButtressHeight-= RLAdjust();

	if( dButtressHeight<ROUND_ERROR )
	{
		//This is not tall enough to be a buttress bay!
		acutPrintf( _T("Bay not tall enough to become buttress bay") );
		return false;
	}

	////////////////////////////////////////////////////////////////
	//We should remove any lifts that are above the new top
	int		i;
	Lift	*pLift;
	for( i=GetNumberOfLifts()-1; i>=0; i-- )
	{
		pLift = GetLift( i );
		if( pLift->GetRL()<dRLBottom+dButtressHeight )
		{
			//All the rest of the lifts will be below this one
			break;
		}

		//mark the lift for delete
		pLift->SetDirtyFlag( DF_DELETE );
	}
	//Do the cleanup
	CleanUp();

	////////////////////////////////////////////////////////////////
	//Set the Rise of the top lift!
	double dTopLiftRL;
	pLift = GetLift(GetNumberOfLifts()-1);
	dTopLiftRL = pLift->GetRL();
	dHeight = dRLBottom+dButtressHeight-dTopLiftRL;
	pLift->SetRise( GetRiseEnumFromRise(dHeight) );

	////////////////////////////////////////////////////////////////
	//4)  Delete unsuitable components
	DeleteAllComponentsFromLifts();

	////////////////////////////////////////////////////////////////
	//5)  Add special components only found in ladder and stair bays
	AddSpecialStairsLaddersComponentsToAllLifts();

/*	////////////////////////////////////////////////////////////////
	//6)  Buttress bays also require handrails to neighboring decks
	AddHandrailToNeighborAboveRL( dRLBottom, GetInner(),	NORTH );
	AddHandrailToNeighborAboveRL( dRLBottom, GetBackward(), EAST );
	AddHandrailToNeighborAboveRL( dRLBottom, GetOuter(),	SOUTH );
	AddHandrailToNeighborAboveRL( dRLBottom, GetForward(),	WEST );
*/
	SetStandardToRLs( CNR_NORTH_EAST, dRLBottom+dButtressHeight+RLAdjust(), dRLBottom);
	SetStandardToRLs( CNR_SOUTH_EAST, dRLBottom+dButtressHeight+RLAdjust(), dRLBottom);
	SetStandardToRLs( CNR_SOUTH_WEST, dRLBottom+dButtressHeight+RLAdjust(), dRLBottom);
	SetStandardToRLs( CNR_NORTH_WEST, dRLBottom+dButtressHeight+RLAdjust(), dRLBottom);

	BraceExposedSides();

	//Remove Bracing from adjoining sides.   We also need to ensure
	//	that the handrails of neighboring bays prevent access to the
	//	buttress bay.
	if( GetInner()!=NULL )
	{
		GetInner()->DeleteAllComponentsOfTypeFromSide( CT_BRACING, NORTH );
		GetInner()->EnsureHandrailsAreCorrect( NORTH, true );
	}
	if( GetOuter()!=NULL )
	{
		GetOuter()->DeleteAllComponentsOfTypeFromSide( CT_BRACING, SOUTH );
		GetOuter()->EnsureHandrailsAreCorrect( SOUTH, true );
	}
	if( GetForward()!=NULL )
	{
		GetForward()->DeleteAllComponentsOfTypeFromSide( CT_BRACING, WEST );
		GetForward()->EnsureHandrailsAreCorrect( WEST, true );
	}
	if( GetBackward()!=NULL )
	{
		GetBackward()->DeleteAllComponentsOfTypeFromSide( CT_BRACING, EAST );
		GetBackward()->EnsureHandrailsAreCorrect( EAST, true );
	}

	return true;
}

bool Bay::GetRLsForBayByExistingLifts( double &dRLBottom, double &dRLTop )
{
	int				i, iSize;
	double			dLiftRise;
	LiftRiseEnum	eLiftRise;

	//we are going to examine the heights of all lifts except the top lift
	iSize = GetNumberOfLifts()-1;
	if( iSize<=0 )
		return false;

	////////////////////////////////////////////////////////////
	//Find the RL at the bottom
	dRLBottom = GetRLOfLift(0);

	////////////////////////////////////////////////////////////
	//Find the RL at the top
	dRLTop = dRLBottom;
	for( i=0; i<iSize; i++ )
	{
		eLiftRise = GetLiftRise( i );
		dLiftRise = GetRiseFromRiseEnum( eLiftRise, GetStarSeparation() );
		dRLTop+= dLiftRise;
	}

	////////////////////////////////////////////////////////////
	//double check - the top lift's RL + its rise should be = dRLTop
	assert( dRLTop==dLiftRise+GetRLOfLift( iSize-1 ) );

	dRLTop+= RLAdjust();

	return true;
}


bool Bay::GetRLsForBayByStandards(double &dMinRL, double &dMaxRL)
{
	double	dRLBottom, dRLTop;

	/////////////////////////////////////////////////
	//Find Standard Span for North East
	GetRLsForStandard( CNR_NORTH_EAST, dRLBottom, dRLTop);
	dMinRL = dRLBottom;
	dMaxRL = dRLTop;

	/////////////////////////////////////////////////
	//Find Standard Span for South East
	GetRLsForStandard( CNR_SOUTH_EAST, dRLBottom, dRLTop);
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	/////////////////////////////////////////////////
	//Find Standard Span for South West
	GetRLsForStandard( CNR_SOUTH_WEST, dRLBottom, dRLTop);
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	/////////////////////////////////////////////////
	//Find Standard Span for North West
	GetRLsForStandard( CNR_NORTH_WEST, dRLBottom, dRLTop);
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	assert( dMinRL<=dMaxRL );

	return true;
}

bool Bay::GetRLsForFaceByStandards(SideOfBayEnum eSide, double &dMinRL, double &dMaxRL)
{
	double			dRLBottom, dRLTop;
	CornerOfBayEnum	eCnrLeft, eCnrRight;


	switch( eSide )
	{
	case( NORTH ):
		eCnrLeft	= CNR_NORTH_WEST;
		eCnrRight	= CNR_NORTH_EAST;
		break;
	case( EAST ):
		eCnrLeft	= CNR_NORTH_EAST;
		eCnrRight	= CNR_SOUTH_EAST;
		break;
	case( SOUTH ):
		eCnrLeft	= CNR_SOUTH_EAST;
		eCnrRight	= CNR_SOUTH_WEST;
		break;
	case( WEST ):
		eCnrLeft	= CNR_SOUTH_WEST;
		eCnrRight	= CNR_NORTH_WEST;
		break;
	default:
		assert( false );
		return false;
	}

	/////////////////////////////////////////////////
	//Find Standard Span for North East
	GetRLsForStandard( eCnrLeft, dRLBottom, dRLTop);
	dMinRL = dRLBottom;
	dMaxRL = dRLTop;

	/////////////////////////////////////////////////
	//Find Standard Span for South East
	GetRLsForStandard( eCnrRight, dRLBottom, dRLTop);
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	assert( dMinRL<=dMaxRL );

	return true;
}

bool Bay::GetRLsForBayByHeights(double &dMinRL, double &dMaxRL, bool bIncludeComponents/*=true*/, bool bIncludeJacksSoleBoards/*=true*/)
{
	int			i, j;
	Lift		*pLift;
	double		dRL, dRLBottom, dRLTop;
	Component	*pComponent;

	/////////////////////////////////////////////////
	//Find Standard Span for North East
	dRLBottom	= GetStandardPosition( CNR_NORTH_EAST ).z;
	dRLTop		= GetHeightOfStandards( CNR_NORTH_EAST ) + dRLBottom;
	dMinRL		= dRLBottom;
	dMaxRL		= dRLTop;

	/////////////////////////////////////////////////
	//Find Standard Span for South East
	dRLBottom	= GetStandardPosition( CNR_SOUTH_EAST ).z;
	dRLTop		= GetHeightOfStandards( CNR_SOUTH_EAST ) + dRLBottom;
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	/////////////////////////////////////////////////
	//Find Standard Span for South West
	dRLBottom	= GetStandardPosition( CNR_SOUTH_WEST ).z;
	dRLTop		= GetHeightOfStandards( CNR_SOUTH_WEST ) + dRLBottom;
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );

	/////////////////////////////////////////////////
	//Find Standard Span for North West
	dRLBottom	= GetStandardPosition( CNR_NORTH_WEST ).z;
	dRLTop		= GetHeightOfStandards( CNR_NORTH_WEST ) + dRLBottom;
	dMinRL = max( dMinRL, dRLBottom );
	dMaxRL = min( dMaxRL, dRLTop );


	if( bIncludeComponents )
	{
		/////////////////////////////////////////////////
		//for each component find its RL
		for( i=0; i<GetNumberOfBayComponents(); i++ )
		{
			pComponent = GetBayComponent( i );
			if( !bIncludeJacksSoleBoards )
			{
				if( pComponent->GetType()==CT_JACK ||
					pComponent->GetType()==CT_SOLEBOARD )
				{
					continue;
				}
			}

			dRL = pComponent->GetRL();
			dMinRL = min( dRL, dMinRL );
			dMaxRL = max( dRL, dMaxRL );
		}

		for( j=0; j<GetNumberOfLifts(); j++ )
		{
			pLift = GetLift( j );

			if( pLift!=NULL )
			{
				for( i=0; i<pLift->GetNumberOfComponents(); i++ )
				{
					pComponent = pLift->GetComponent( i );
					dRL = pComponent->GetRL();
					dMinRL = min( dRL, dMinRL );
					dMaxRL = max( dRL, dMaxRL );
				}
			}
		}
	}

	if( dMinRL>dMaxRL )
		return false;

	return true;
}

void Bay::GetRLsForStandard(CornerOfBayEnum eCnr, double &dRLBottom, double &dRLTop)
{
	int			i, iSize;
	double		dHeight;
	doubleArray	daArrangement;

	/////////////////////////////////////////////////////////
	//find the RL at the bottom of the standard
	dRLBottom = GetStandardPosition( eCnr ).z;

	/////////////////////////////////////////////////////////
	//Find what standards are on this side
	GetStandardsArrangement( daArrangement, eCnr );

	/////////////////////////////////////////////////////////
	//Get the total height of those standards
	dHeight = 0.00;
	iSize = daArrangement.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight+= daArrangement[i];  
	}

	assert( dHeight>=0 );

	/////////////////////////////////////////////////////////
	//find the RL at the top of the pole
	dRLTop = dRLBottom + dHeight;
}


int Bay::GetNumberOfLiftsForBay( LiftRiseEnum eLiftRise, double dHeight)
{
	int		iNumberOfLifts;
	double	dLiftRise;

	/////////////////////////////////////////////////
	//How many lifts fit in this space?
	dLiftRise = GetRiseFromRiseEnum( eLiftRise, GetStarSeparation() );
	if( dLiftRise<=0.01 )
		return 0;

	/////////////////////////////////////////////////
	//
	iNumberOfLifts = (int)(dHeight/dLiftRise);

	return iNumberOfLifts;
}

void Bay::AdjustNumberOfLifts( int iRequiredNumber )
{
	int		iTarget;
	Lift	*pLift;

	iTarget = iRequiredNumber;
	while( iRequiredNumber!=GetNumberOfLifts() )
	{
		if( iRequiredNumber<GetNumberOfLifts() )
		{
			DeleteLift( iRequiredNumber );
		}
		else
		{
			///////////////////////////////////////////////
			//add a new lift
			pLift = new Lift();
			pLift->SetBayPointer( this );
			//All new lifts get created at the orgins so set the RL at the origin,
			//	we will move it later!
			m_LiftList.AddLift( pLift, 0.00 );
			pLift->SetLiftType( ConvertBayTypeToLiftType( GetBayType() ) );
			iRequiredNumber--;
		}
	}
	assert( GetNumberOfLifts()==iTarget );
}

void Bay::SetRLsForAllLifts( double dRLBottom )
{
	m_LiftList.SetRLsForAllLifts(dRLBottom);
}

void Bay::DeleteAllComponentsFromLifts()
{
	int		i;
	Lift	*pLift;

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pLift->DeleteAllComponentsFromLift();
	}

	DeleteBrace( NORTH );
	DeleteBrace( SOUTH );
	DeleteBrace( EAST );
	DeleteBrace( WEST );
}

void Bay::AddSpecialStairsLaddersComponentsToAllLifts()
{
	int		i;
	Lift	*pLift;

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pLift->AddSpecialStairsLaddersComponentsToLift();
	}
}


void Bay::SetRisesForAllLifts(LiftRiseEnum eRise)
{
	int		i;
	Lift	*pLift;

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		pLift->SetRise( eRise );
	}
}

void Bay::DeleteAccessObsticles()
{
	if( GetInner()!=NULL )
		DeleteObsticales( GetInner(), NORTH );

	//stairs can only be accessed by the southern side
	if( GetOuter()!=NULL )
		DeleteObsticales( GetOuter(), SOUTH );
	if( GetForward()!=NULL )
		DeleteObsticales( GetForward(), WEST );
	if( GetBackward()!=NULL )
		DeleteObsticales( GetBackward(), EAST );

	int				i;
	LapboardBay		*pLap;
	SideOfBayEnum	eSide;

	for( i=0; i<GetNumberOfLapboardPointers(); i++ )
	{
		pLap = m_pLapboards->GetAt(i);
		eSide = LapboardCrossesSideGet(i);
		DeleteObsticales( (Bay*)pLap, eSide );
	}
}

void Bay::DeleteObsticales( Bay *pNeighboringBay, SideOfBayEnum eSideOfNeighboringBay )
{
	int					i, iLiftID;
	bool				bDeleteRequired;
	Lift				*pLift, *pLiftNeighbor;
	double				dLiftRL, dLiftNeighborRL, dLiftRLBottom, dRise, dRL, dRLGoal, dLiftRLdeck;
	Component			*pComponent;
	doubleArray			daDeckingHeights, daDeckingRLs;
	LiftRiseEnum		eRise;
	SideOfBayEnum		eSideOfThisBay;
	ComponentTypeEnum	eComponentType;

	///////////////////////////////////////////////////////
	//Where are the decks of the neighboring bay?
	if( pNeighboringBay->GetNumberOfLifts()<=0 )
		return;
	if( GetNumberOfLifts()<=0 )
		return;
	dLiftRLBottom = pNeighboringBay->m_LiftList.GetRL(0);
	daDeckingHeights.RemoveAll();
	pNeighboringBay->CreateMatrixDeckingArrangement( daDeckingHeights, false, false );

	/////////////////////////////////////////////////////////////////////////
	//Bay components
	if( daDeckingHeights.GetSize()>0 )
	{
		pNeighboringBay->DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK,	eSideOfNeighboringBay );
		pNeighboringBay->DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH,	eSideOfNeighboringBay );
		pNeighboringBay->DeleteAllComponentsOfTypeFromSide( CT_BRACING,		eSideOfNeighboringBay );
	}

	////////////////////////////////////////////////////
	//Delete the components of that bay
	for( i=0; i<daDeckingHeights.GetSize(); i++ )
	{
		//dLiftRL = dLiftRLBottom+daDeckingHeights[i];
		dLiftNeighborRL = daDeckingHeights[i];

		pLiftNeighbor = pNeighboringBay->GetLiftAtRL( dLiftNeighborRL );
		if( pLiftNeighbor==NULL )
			break;

		assert( pLiftNeighbor->IsDeckingLift() );
		assert( pLiftNeighbor->GetRise()==LIFT_RISE_2000MM );
		daDeckingRLs.Add( pLiftNeighbor->GetRL() );

		//The Decking is at this height, so the lift must have 2m clearance
		//	it if it was any other sort of bay (except ladder )it would not have decking!

		/////////////////////////////////////////////////////////////////////////
		//Handrails
		bDeleteRequired = true;
		if( !(	GetBayType()==BAY_TYPE_STAIRS ||
				GetBayType()==BAY_TYPE_LADDER ) )
		{
			pLift = GetLiftAtRL( dLiftNeighborRL );
			if( pLift==NULL )
			{
				bDeleteRequired = false;
			}
			else
			{
				//if the neighboring lift is <GetStarSeparation() below or >GetStarSeparation() above the
				//	lift then we need the handrail
				dLiftRL = pLift->GetRL();
				if( dLiftNeighborRL-dLiftRL<((-1.00*GetStarSeparation())-ROUND_ERROR) ||
					dLiftNeighborRL-dLiftRL>(GetStarSeparation()+ROUND_ERROR) )
				{
					bDeleteRequired = false;
				}
			}
		}

		if( bDeleteRequired )
		{
			pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSideOfNeighboringBay );
			pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSideOfNeighboringBay );
			pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSideOfNeighboringBay );

			//hopup rails
			SideOfBayEnum eSide1, eSide2;
			switch( eSideOfNeighboringBay )
			{
			case( NORTH ):
				eSide1 = ENE;
				eSide2 = WNW;
				break;
			case( SOUTH ):
				eSide1 = ESE;
				eSide2 = WSW;
				break;
			case( EAST ):
				eSide1 = NNE;
				eSide2 = SSE;
				break;
			case( WEST ):
				eSide1 = NNW;
				eSide2 = NNE;
				break;
			default:
				eSide1 = SIDE_INVALID;
				eSide2 = SIDE_INVALID;
			}
			if( eSide1!=SIDE_INVALID )
			{
				pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide1, LIFT_RISE_1000MM );
				pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide2, LIFT_RISE_1000MM );
			}
			/////////////////////////////////////////////////////////////////////////
			//Toe boards
			eComponentType = CT_TRANSOM;
			if( eSideOfNeighboringBay==SOUTH || eSideOfNeighboringBay==NORTH )
			{
				pLiftNeighbor->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSideOfNeighboringBay );
				eComponentType = CT_LEDGER;
			}

			//Delete transom/ledgers
			pComponent = pLiftNeighbor->GetComponent( eComponentType, LIFT_RISE_0500MM, eSideOfNeighboringBay );
			if( pComponent!=NULL )
				pLiftNeighbor->DeleteComponent( pComponent->GetID() );
			pComponent = pLiftNeighbor->GetComponent( eComponentType, LIFT_RISE_1000MM, eSideOfNeighboringBay );
			if( pComponent!=NULL )
				pLiftNeighbor->DeleteComponent( pComponent->GetID() );
			pComponent = pLiftNeighbor->GetComponent( eComponentType, LIFT_RISE_1500MM, eSideOfNeighboringBay );
			if( pComponent!=NULL )
				pLiftNeighbor->DeleteComponent( pComponent->GetID() );
		}
	}

	assert( daDeckingRLs.GetSize()==daDeckingHeights.GetSize() );

	//Find out which side of THIS bay we are talking about
	switch( eSideOfNeighboringBay )
	{
	case( NORTH ):	//fallthrough
		eComponentType = CT_LEDGER;
		eSideOfThisBay = SOUTH;
		break;
	case( SOUTH ):
		eComponentType = CT_LEDGER;
		eSideOfThisBay = NORTH;
		break;
	case( EAST ):	//fallthrough
		eComponentType = CT_TRANSOM;
		eSideOfThisBay = WEST;
		break;
	case( WEST ):
		eComponentType = CT_TRANSOM;
		eSideOfThisBay = EAST;
		break;
	default:
		assert( false );
		return;
	}

	/////////////////////////////////////////////////////////////////////////
	//Bay components
	if( daDeckingRLs.GetSize()>0 )
	{
		DeleteAllComponentsOfTypeFromSide( CT_CHAIN_LINK,	eSideOfThisBay );
		DeleteAllComponentsOfTypeFromSide( CT_SHADE_CLOTH,	eSideOfThisBay );
		DeleteAllComponentsOfTypeFromSide( CT_BRACING,		eSideOfThisBay );
	}

	////////////////////////////////////////////////////
	//Delete the component belonging to this bay that would block access to the
	//	inner bay's deck!  Remember that these obsticals could span 1 or 2 bays
	for( i=0; i<daDeckingRLs.GetSize(); i++ )
	{
		//Get the RL of the neighboring bays decking!
		dLiftRLdeck = daDeckingRLs[i];

		//We need to remove any obsticals from the first star above the neighbors
		//	deck, up to the height of the neighoring bay's decks, which must be 
		//	2000mm since it has decking!
		dRL		= dLiftRLdeck+GetStarSeparation();
		dRLGoal	= dLiftRLdeck+GetRiseFromRiseEnum( LIFT_RISE_2000MM, GetStarSeparation() );


		//What Lifts are at 1, 2 or 3 * GetStarSeparation() above the level of the neighboring bay's
		//	decks, since these may contain components that could block access!
		while( dRL<=dRLGoal+ROUND_ERROR )
		{
			//////////////////////////////////////////////
			//Which belonging to this lift covers this RL?
			iLiftID = GetLiftIDAtRL( dRL );
			if( iLiftID==ID_NONE_MATCHING )
			{
				dRL+= GetStarSeparation();
				continue;
			}
			pLift = GetLift( iLiftID );
			assert( pLift!=NULL );

			///////////////////////////////////////////////
			//What Rise within this lift is this RL?
			GetLiftRL( iLiftID, dLiftRL );
			dRise = dRL-dLiftRL; 
			eRise = GetRiseEnumFromRise( dRise );


			//////////////////////////////////////////////////////
			//We need to remove certain component from this lift that will
			//	be blocking the path
			if( eRise==LIFT_RISE_0000MM )
			{
				if( eSideOfThisBay==NORTH || eSideOfThisBay==SOUTH )
					pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSideOfThisBay );
				if( iLiftID>0 )
				{
					//the offending component may be a meshguard from the lift underneath!
					pComponent = GetLift(iLiftID-1)->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eSideOfThisBay );
					if( pComponent!=NULL )
						GetLift(iLiftID-1)->DeleteComponent( pComponent->GetID() );
				}
			}
			else if( eRise==LIFT_RISE_0500MM )
			{
				pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSideOfThisBay );
			}
			else if( eRise==LIFT_RISE_1000MM )
			{
				DeleteUnneeded1000mmHopups( pNeighboringBay, eSideOfNeighboringBay );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSideOfThisBay );
				pComponent = pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, eSideOfThisBay );
				if( pComponent!=NULL )
					pLift->DeleteComponent( pComponent->GetID() );
			}
			else if( eRise==LIFT_RISE_1500MM )
			{
				//Do nothing - could only be a transom or ledgers, which is removed below!
				1;
			}
			else if( eRise==LIFT_RISE_2000MM )
			{
				pComponent = pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eSideOfThisBay );
				if( pComponent!=NULL )
					pLift->DeleteComponent( pComponent->GetID() );
			}
			else
			{
				assert( eRise==LIFT_RISE_2500MM || eRise==LIFT_RISE_3000MM );
			}

			///////////////////////////////////////////////
			//Transoms and Ledges can potentially be at any rise within the lift
			//	However, we don't want to delete a supporting Transom nor ledger
			//	(ie ones on the bottom of a lift) otherwise the planks will have
			//	nothing to rest upon
			if( !(eRise==LIFT_RISE_0000MM && pLift->IsDeckingLift()) )
			{
				//Delete the component
				pComponent = pLift->GetComponent( eComponentType, eRise, eSideOfThisBay );
				if( pComponent!=NULL )
				{
					pLift->DeleteComponent( pComponent->GetID() );
					pComponent=NULL;
				}
			}

			///////////////////////////////////////////////
			//go to the next star
			dRL+= GetStarSeparation();
		}
	}
}

bool Bay::SetAndAdjustRLs(double dRLs[4], bool bAutobuild/*=false*/ )
{
	int				i;
	bool			bReIDRequired;
	Lift			*pLift;
	double			dRLBottom, dRLTop, dRL, dLiftRise;
	Point3D			pt;
	Vector3D		Vector;
	Matrix3D		Transform;
	LiftRiseEnum	eLiftRise;

	dRL = GetRLOfLift( GetNumberOfLifts()-1 );
	for( i=(int)CNR_NORTH_EAST; i<=CNR_SOUTH_WEST; i++ )
	{
		if( dRL<=dRLs[i] )
		{
			CString sErrMsg, sTemp;
			MessageBeep(MB_ICONEXCLAMATION);
			sErrMsg.Format( _T("You have set this bay to be %1.1fm high!\n"), dRLs[i]+GetStarSeparation() );
			sTemp.Format( _T("However the top bay height for this bay is %1.1fm"), dRL+GetStarSeparation() );
			sErrMsg+=sTemp;
			if( bAutobuild )
			{
				sErrMsg+= _T("\nI will create the bay directly from the template,");
				sErrMsg+= _T("\nwithout adjusting the Reletive Level's!");
				sErrMsg+= _T("\n\nThis error can be corrected with the 'ChangeRLs' command");
			}
			else
			{
				sErrMsg+= _T("\nPlease choose appropriate RL's and try again.");
			}
			MessageBox( NULL, sErrMsg, ERROR_MSG_INVALID_RL, MB_OK );
			return false;
		}
	}

	AdjustStandardBottomToRL( dRLs[CNR_NORTH_EAST],	CNR_NORTH_EAST );
	AdjustStandardBottomToRL( dRLs[CNR_SOUTH_EAST],	CNR_SOUTH_EAST );
	AdjustStandardBottomToRL( dRLs[CNR_SOUTH_WEST],	CNR_SOUTH_WEST );
	AdjustStandardBottomToRL( dRLs[CNR_NORTH_WEST],	CNR_NORTH_WEST );

	bReIDRequired = false;

	////////////////////////////////////////////////////////////////
	//What are the RL limits that we can use
	if( !GetRLsForBayByHeights( dRLBottom, dRLTop, false, false ) )
		return false;

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		dRL = GetRLOfLift( i );
		if( dRL<dRLBottom || dRL>dRLTop )
		{
			AddComponentToNeighborIfReqd( i, CT_TRANSOM, EAST, WEST, GetForward() );
			AddComponentToNeighborIfReqd( i, CT_TRANSOM, WEST, EAST, GetBackward() );
			AddComponentToNeighborIfReqd( i, CT_LEDGER, NORTH, SOUTH, GetOuter() );
			AddComponentToNeighborIfReqd( i, CT_LEDGER, SOUTH, NORTH, GetInner() );

			////////////////////////////////////////////////////////////////
			//Delete the lift if doesn't fit, don't move the rest down though
			if( DeleteLift( i, false ) )
			{
				i--;
				bReIDRequired = true;
			}
		}
		else
		{
			//Do we need a trannie or ledger?
			AddTransomIfReqd( i );
			AddLedgerIfReqd( i );
		}
	}

	if( GetNumberOfLifts()>0 )
	{
		//JSB NOTE 991221 - This code will adjust the height of
		//	the existing bottom lift so that only the very bottom
		//	lift will be less than 2000mm high
		//	however, after speaking with Mark & Terry, they assure me
		//	that it would be better to keep the existing lift at the
		//	same height, this will also save on an extra transom!
		//JSB NOTE 20000209 - We can't remember now why this was removed
		//	but it is wrong, so we have reinstated this code!
		////////////////////////////////////////////////////////////////
		//Start with the bottom lift! (lift 0)
		if( GetLiftRise(0)!=LIFT_RISE_2000MM )
		{
			//we can increase the size of the bottom lift(downward)!
			dLiftRise = GetRiseFromRiseEnum( GetLiftRise(0), GetStarSeparation() );
			dRL = GetRLOfLift(0);
			while( dRL>dRLBottom )
			{
				dRL-=GetStarSeparation();
				dLiftRise+=GetStarSeparation();
				m_LiftList.SetRL( 0, dRL );
				GetLift(0)->SetRise( GetRiseEnumFromRise( dLiftRise ) );
				Vector.set( 0.00, 0.00, -1.00*RLAdjust() );
				Transform.setToTranslation(Vector);
				GetLift(0)->Move( Transform, true );

				AddTransomIfReqd( 0 );

				if( dLiftRise>=GetRiseFromRiseEnum( LIFT_RISE_2000MM, GetStarSeparation() ) - ROUND_ERROR,
								GetStarSeparation() )
				{
					//can't get any bigger!
					break;
				}
			}
		}

		dRL = GetRLOfLift(0);
	}
	else
	{
		//There is no prevous lift so start at the top
		dRL = dRLTop;
	}
	//	The first RL will be 2m below the previous lift
	dLiftRise = GetRiseFromRiseEnum( LIFT_RISE_2000MM, GetStarSeparation() );
	dRL -= dLiftRise;

	///////////////////////////////////////////////////////
	//whilst we can fit a 2m lift, add one

	while( dRL>dRLBottom )
	{
		////////////////////////////////////////////////////////////////
		//insert an empty lift at position 0 and RL=dRL
		pLift = new Lift();
		pLift->SetBayPointer( this );
		pLift->SetRise( LIFT_RISE_2000MM );
		InsertLift( 0, pLift, false, false );
		m_LiftList.SetRL( 0, dRL );
		SetRLsForAllLifts( dRL );

		////////////////////////////////////////////////////////////////
		//This is an empty lift so it only needs Transoms & Ledgers
		pLift->AddComponent( CT_LEDGER,			GetBayLength(),	NORTH,	LIFT_RISE_0000MM,	MT_STEEL );
		pLift->AddComponent( CT_TRANSOM,		GetBayWidth(),	EAST,	LIFT_RISE_0000MM,	MT_STEEL );
		AddLedgerIfReqd( pLift->GetLiftID(), true );
		AddLedgerIfReqd( pLift->GetLiftID(), false );
		AddTransomIfReqd( pLift->GetLiftID(), true );
		AddTransomIfReqd( pLift->GetLiftID(), false );

		/////////////////////////////////////////
		//Move the lift vertically
		Vector.set( 0.00, 0.00, dRL );
		Transform.setToTranslation( Vector );
		pLift->Move( Transform, true );

		//GetReady for another Loop, or to exit loop
		dRL-= dLiftRise;

		bReIDRequired = true;
	}

	//We couldn't fit a 2m lift at dRL, but can we fit a smaller one?
	//	Note, at this stage dRLBottom should be < dRL
	dRL+= dLiftRise;
	dLiftRise = dRL - dRLBottom;
	if( dLiftRise>=GetStarSeparation() )
	{
		assert( (dRL+dLiftRise)>dRLBottom );

		////////////////////////////////////////////////////////////////
		//insert an empty lift at position 0 and RL=dRLBottom
		pLift = new Lift();
		pLift->SetBayPointer( this );
		eLiftRise = GetRiseEnumFromRise(dLiftRise);
		pLift->SetRise( eLiftRise );
		InsertLift( 0, pLift, false, false );
		m_LiftList.SetRL( 0, dRL-dLiftRise );
		SetRLsForAllLifts( dRL-dLiftRise );

		////////////////////////////////////////////////////////////////
		//This is an empty lift so it only needs Transoms & Ledgers
		pLift->AddComponent( CT_LEDGER,			GetBayLength(),	NORTH,	LIFT_RISE_0000MM, MT_STEEL );
		pLift->AddComponent( CT_LEDGER,			GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, MT_STEEL );
		pLift->AddComponent( CT_TRANSOM,		GetBayWidth(),	EAST,	LIFT_RISE_0000MM, MT_STEEL );
		pLift->AddComponent( CT_TRANSOM,		GetBayWidth(),	WEST,	LIFT_RISE_0000MM, MT_STEEL );

		/////////////////////////////////////////
		//Move the lift vertically
		Vector.set( 0.00, 0.00, dRL-dLiftRise );
		Transform.setToTranslation( Vector );
		pLift->Move( Transform, true );

		bReIDRequired = true;
	}
	if( bReIDRequired )
		ReIDAllLifts();

	bool bAdjustStandardArrangement;
	bAdjustStandardArrangement = true;
	if( bAutobuild &&
		dRLs[CNR_NORTH_EAST]>=-ROUND_ERROR && dRLs[CNR_NORTH_EAST]<=ROUND_ERROR &&
		dRLs[CNR_SOUTH_EAST]>=-ROUND_ERROR && dRLs[CNR_SOUTH_EAST]<=ROUND_ERROR &&
		dRLs[CNR_SOUTH_WEST]>=-ROUND_ERROR && dRLs[CNR_SOUTH_WEST]<=ROUND_ERROR &&
		dRLs[CNR_NORTH_WEST]>=-ROUND_ERROR && dRLs[CNR_NORTH_WEST]<=ROUND_ERROR )
	{
		bAdjustStandardArrangement = false;
	}

	if( bAdjustStandardArrangement )
	{
		int				iCnr;
		for( iCnr=(int)CNR_NORTH_EAST; iCnr<=(int)CNR_SOUTH_WEST; iCnr++ )
		{
			AutoAdjustStandard((CornerOfBayEnum)iCnr);
		}
	}

	ReapplyShadeAndChainOnAllSides();
	ReapplyWallTiesFromTemplate();
	RecreateLapsBoardsAndTieTubes();

	///////////////////////////////////////////////////////////////////////////
	//Modify the Bracing
	for( int j=(int)NORTH; j<=(int)WEST; j++ )
	{
		EnsureHandrailsAreCorrect((SideOfBayEnum)j);

		if( DeleteAllComponentsOfTypeFromSide( CT_BRACING, (SideOfBayEnum)j ) )
		{
			//we deleted something, better, recreate the bracing at the correct
			//	height this time!
			CreateBracingArrangment ( (SideOfBayEnum)j, MT_STEEL, false );
		}
	}

	return true;
}

void Bay::AdjustStandardBottomToRL(double dRL, CornerOfBayEnum eCnr)
{
	Point3D	pt;
	double	dHeight, dTemp;

	//round up RL to nearest GetStarSeparation()!
	dRL/= GetStarSeparation();
	dTemp = (int)dRL;	//rounds down
	if( dRL-dTemp>0.00 )
	{
		dRL = dTemp+1.0;
	}
	else if( dRL-dTemp<0.00 )
	{
		dRL = dTemp;
	}
	dRL*= GetStarSeparation();

	pt		= GetStandardPosition( eCnr );
	dHeight = GetHeightOfStandards( eCnr );
	dHeight+= (pt.z-dRL);
	pt.z	= dRL;
	SetHeightOfStandards( pt, dHeight, eCnr );
}

bool Bay::ReIDAllLifts()
{
	int		iID, iSize;
	Lift	*pLift;

	iSize = GetNumberOfLifts();
	if( iSize<=0 )
		return false;

	for( iID=0; iID<iSize; iID++ )
	{
		pLift = GetLift( iID );
		pLift->SetLiftID( iID );
	}

	return true;
}


Component * Bay::GetBayComponent(int iComponentID) const
{
	if( iComponentID<0 || iComponentID>=GetNumberOfBayComponents() )
		return NULL;
	return m_caComponents.GetComponent( iComponentID );
}


void Bay::TraceContents( CString sComment, bool bBayCompsOnly/*=false*/, int iLift/*=ID_INVALID*/ ) const
{
#ifdef _DEBUG
	int			i, iSize;
	Lift		*pLift;
	CString		sText;
	Component	*pComponent;

	acutPrintf( _T("\nvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n") );
	if( !sComment.IsEmpty() )
		acutPrintf( sComment );

	if( iLift==ID_INVALID )
	{
		if( GetBayType()==BAY_TYPE_LAPBOARD )
		{
			acutPrintf( _T("\nLap %i\n"), GetID()+1 );

			LapboardBay	*pLap;
			pLap = (LapboardBay*)this;
			if( pLap->GetBayWest()==NULL )
				acutPrintf( _T("\nWest = NULL") );
			else
				acutPrintf( _T("\nWest = %i(%lx)"), pLap->GetBayWest()->GetBayNumber(), pLap->GetBayWest() );

			if( pLap->GetBayEast()==NULL )
				acutPrintf( _T("\nEast = NULL") );
			else
				acutPrintf( _T("\nEast = %i(%lx)"), pLap->GetBayEast()->GetBayNumber(), pLap->GetBayEast() );

			acutPrintf( _T("\n") );
		}
		else
		{
			acutPrintf( _T("\nBay %i(%i in Run%i)\n"), GetBayNumber(), GetID(), GetRunPointer()->GetRunID() );

			if( GetForward()==NULL )
				acutPrintf( _T("\nForward = NULL") );
			else
				acutPrintf( _T("\nForward = %i(%lx)"), GetForward()->GetBayNumber(), GetForward() );

			if( GetBackward()==NULL )
				acutPrintf( _T("\nBackward = NULL") );
			else
				acutPrintf( _T("\nBackward = %i(%lx)"), GetBackward()->GetBayNumber(), GetBackward() );

			if( GetInner()==NULL )
				acutPrintf( _T("\nInner = NULL") );
			else
				acutPrintf( _T("\nInner = %i(%lx)"), GetInner()->GetBayNumber(), GetInner() );

			if( GetOuter()==NULL )
				acutPrintf( _T("\nOuter = NULL") );
			else
				acutPrintf( _T("\nOuter = %i(%lx)"), GetOuter()->GetBayNumber(), GetOuter() );

			acutPrintf( _T("\nBay Components\n"), GetID()+1 );

			iSize = GetNumberOfBayComponents();
			for( i=0; i<iSize; i++ )
			{
				pComponent = m_caComponents.GetComponent( i );
				if( !GetController()->IsComponentTypeHidden( pComponent->GetType() ) )
				{
					sText = GetComponentDescStrAlign( pComponent->GetType() );
					acutPrintf( _T("\t\t%s"), sText );
					sText = GetSideOfBayDescStrAlign( GetPosition( i ) );
					acutPrintf( _T(",\t%s"), sText );
					acutPrintf( _T(",\t%i(%i)"), (int)pComponent->GetLengthCommon(), (int)pComponent->GetLengthActual() );
					acutPrintf( _T(",\t%i"), pComponent->GetRise() );
					acutPrintf( _T(",\t%s"), (pComponent->GetSystem()==S_MILLS)? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE );
					acutPrintf( _T("\n") );
				}
			}
		}
	}

	if( !bBayCompsOnly )
	{
		acutPrintf( _T("\tLifts\n") );

		if( iLift==ID_INVALID )
		{
			for( i=0; i<GetNumberOfLifts(); i++ )
			{
				pLift = GetLift( i );
				pLift->TraceContents( true );
				acutPrintf( _T("\n") );
			}
		}
		else
		{
			pLift = GetLift( iLift );
			if( pLift!=NULL )
			{
				pLift->TraceContents( true );
				acutPrintf( _T("\n") );
			}
		}
	}
	acutPrintf( _T("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") );
#endif	//#ifdef _DEBUG
}

void Bay::MakeStandardsSameTopRL()
{
	double	dRLTop[4], dRLBottom[4], dRLMax, dRLMin;

	//////////////////////////////////////////////////////////////////////////
	//We will ask the run to look at all bays in this run and
	//	find the extants of those bays, in this case we are only
	//	really after the dRLMax value
	GetRunPointer()->GetRLExtentsForAllBays( dRLMax, dRLMin );

	//////////////////////////////////////////////////////////////////////////
	//We will still need to know the RL for the bottom of each standard, so lets
	//	get that for this bay!
	FindAllRLsForEachCornerOfThisBayByLifts( dRLTop, dRLBottom );

	//////////////////////////////////////////////////////////////////////////
	//Set the NORTH EAST Standard
	SetStandardToRLs( CNR_NORTH_EAST, dRLMax, dRLBottom[CNR_NORTH_EAST] );

	//////////////////////////////////////////////////////////////////////////
	//Set the SOUTH EAST Standard
	if( GetInner()!=NULL )
	{
		// |   ?  |
		//_|______|_
		// |      |
		//?|   *  |?
		//_|___|__x_
		// |   |  |
		// |   v  |
		// |______|
		//This bay does't own this standard
		GetInner()->SetStandardToRLs( CNR_NORTH_EAST, dRLMax, dRLBottom[CNR_SOUTH_EAST] );
	}
	else if( GetForward()!=NULL )
	{
		if( GetForward()->GetInner()!=NULL )
		{
			// |   ?  |
			//_|______|______
			// |      |      |
			//?|   *--|---   |
			//_|______x___|__|
			//  No Bay|   |  |
			//   Here |   v  |
			//        |______|
			//This bay does't own this standard
			GetForward()->GetInner()->SetStandardToRLs( CNR_NORTH_WEST, dRLMax, dRLBottom[CNR_SOUTH_EAST] );
		}
		else
		{
			SetStandardToRLs( CNR_SOUTH_EAST, dRLMax, dRLBottom[CNR_SOUTH_EAST] );
		}
	}
	else
	{
		SetStandardToRLs( CNR_SOUTH_EAST, dRLMax, dRLBottom[CNR_SOUTH_EAST] );
	}

	//////////////////////////////////////////////////////////////////////////
	//Set the SOUTH WEST Standard
	if( GetInner()!=NULL )
	{
		// |   ?  |
		//_|______|_
		// |      |
		//?|   *  |?
		//_x___|__|_
		// |   |  |
		// |   v  |
		// |______|
		//This bay does't own this standard
		GetInner()->SetStandardToRLs( CNR_NORTH_WEST, dRLMax, dRLBottom[CNR_SOUTH_WEST] );
	}
	else if( GetBackward()!=NULL )
	{
		//        |   ?  |
		//  ______|______|_
		// |      |      |
		// |   <--|--*   |?
		// |______x______|_
		//         No Bay
		//          Here
		//This bay does't own this standard
		GetBackward()->SetStandardToRLs( CNR_SOUTH_EAST, dRLMax, dRLBottom[CNR_SOUTH_WEST] );
	}
	else
	{
		SetStandardToRLs( CNR_SOUTH_WEST, dRLMax, dRLBottom[CNR_SOUTH_WEST] );
	}

	//////////////////////////////////////////////////////////////////////////
	//Set the NORTH WEST Standard
	if( GetBackward()!=NULL )
	{
		//        |   ?  |
		//  ______x______|_
		// |      |      |
		// |   <--|---*  |?
		// |______|______|_
		//        |   ?  |
		//This bay does't own this standard
		GetBackward()->SetStandardToRLs( CNR_NORTH_EAST, dRLMax, dRLBottom[CNR_NORTH_WEST] );
	}
	else
	{
		SetStandardToRLs( CNR_NORTH_WEST, dRLMax, dRLBottom[CNR_NORTH_WEST] );
	}
}

void Bay::FindAllRLsForEachCornerOfThisBayByLifts( double dRLMax[4], double dRLMin[4] )
{
	double dRLTop, dRLBottom;

	/////////////////////////////////////////////////////////////////////
	//We need to find the top and bottom RLs for this bay, and for any surrounding bays
	//	since each of the surrounding bays will have to be supported by the
	//	standards from this bay, and vice versa
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//Firstly lets find the top (&bottom) rl for this bay
	FindTopBottomRLsAtCornerByLifts( CNR_NORTH_EAST, dRLMin[CNR_NORTH_EAST], dRLMax[CNR_NORTH_EAST] );
	FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_EAST, dRLMin[CNR_SOUTH_EAST], dRLMax[CNR_SOUTH_EAST] );
	FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLMin[CNR_SOUTH_WEST], dRLMax[CNR_SOUTH_WEST] );
	FindTopBottomRLsAtCornerByLifts( CNR_NORTH_WEST, dRLMin[CNR_NORTH_WEST], dRLMax[CNR_NORTH_WEST] );

	/////////////////////////////////////////////////////////////////////
	//There may be up to 8 bays surrounding us, so lets find their RLs
	//    _______________________
	//   |       |       |       |
	//   |   5   |   7   |   2   |
	//   |_______|_______|_______|
	//   |       |       |       |
	//   |   4   |   *   |   1   |
	//   |_______|_______|_______|
	//   |       |       |       |
	//   |   6   |   8   |   3   |
	//   |_______|_______|_______|


	/////////////////////////////////////
	//Bay 1
	if( GetForward()!=NULL )
	{
		GetForward()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_WEST, dRLBottom, dRLTop );
		dRLMin[CNR_NORTH_EAST] = min( dRLMin[CNR_NORTH_EAST], dRLBottom );
		dRLMax[CNR_NORTH_EAST] = max( dRLMax[CNR_NORTH_EAST], dRLTop );

		GetForward()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
		dRLMin[CNR_SOUTH_EAST] = min( dRLMin[CNR_SOUTH_EAST], dRLBottom );
		dRLMax[CNR_SOUTH_EAST] = max( dRLMax[CNR_SOUTH_EAST], dRLTop );

		/////////////////////////////////////
		//Bay 2
		if( GetForward()->GetOuter()!=NULL )
		{
			GetForward()->GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
			dRLMin[CNR_NORTH_EAST] = min( dRLMin[CNR_NORTH_EAST], dRLBottom );
			dRLMax[CNR_NORTH_EAST] = max( dRLMax[CNR_NORTH_EAST], dRLTop );
		}

		/////////////////////////////////////
		//Bay 3
		if( GetForward()->GetInner()!=NULL )
		{
			GetForward()->GetInner()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_WEST, dRLBottom, dRLTop );
			dRLMin[CNR_SOUTH_EAST] = min( dRLMin[CNR_SOUTH_EAST], dRLBottom );
			dRLMax[CNR_SOUTH_EAST] = max( dRLMax[CNR_SOUTH_EAST], dRLTop );
		}

	}

	/////////////////////////////////////
	//Bay 4
	if( GetBackward()!=NULL )
	{
		GetBackward()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_EAST, dRLBottom, dRLTop );
		dRLMin[CNR_NORTH_WEST] = min( dRLMin[CNR_NORTH_WEST], dRLBottom );
		dRLMax[CNR_NORTH_WEST] = max( dRLMax[CNR_NORTH_WEST], dRLTop );

		GetBackward()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_EAST, dRLBottom, dRLTop );
		dRLMin[CNR_SOUTH_WEST] = min( dRLMin[CNR_SOUTH_WEST], dRLBottom );
		dRLMax[CNR_SOUTH_WEST] = max( dRLMax[CNR_SOUTH_WEST], dRLTop );

		/////////////////////////////////////
		//Bay 5
		if( GetBackward()->GetOuter()!=NULL )
		{
			GetBackward()->GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_EAST, dRLBottom, dRLTop );
			dRLMin[CNR_NORTH_WEST] = min( dRLMin[CNR_NORTH_WEST], dRLBottom );
			dRLMax[CNR_NORTH_WEST] = max( dRLMax[CNR_NORTH_WEST], dRLTop );
		}

		/////////////////////////////////////
		//Bay 6
		if( GetBackward()->GetInner()!=NULL )
		{
			GetBackward()->GetInner()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_EAST, dRLBottom, dRLTop );
			dRLMin[CNR_SOUTH_WEST] = min( dRLMin[CNR_SOUTH_WEST], dRLBottom );
			dRLMax[CNR_SOUTH_WEST] = max( dRLMax[CNR_SOUTH_WEST], dRLTop );
		}
	}

	/////////////////////////////////////
	//Bay 7
	if( GetOuter()!=NULL )
	{
		GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_EAST, dRLBottom, dRLTop );
		dRLMin[CNR_NORTH_EAST] = min( dRLMin[CNR_NORTH_EAST], dRLBottom );
		dRLMax[CNR_NORTH_EAST] = max( dRLMax[CNR_NORTH_EAST], dRLTop );

		GetOuter()->FindTopBottomRLsAtCornerByLifts( CNR_SOUTH_WEST, dRLBottom, dRLTop );
		dRLMin[CNR_NORTH_WEST] = min( dRLMin[CNR_NORTH_WEST], dRLBottom );
		dRLMax[CNR_NORTH_WEST] = max( dRLMax[CNR_NORTH_WEST], dRLTop );
	}

	/////////////////////////////////////
	//Bay 8
	if( GetInner()!=NULL )
	{
		GetInner()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_EAST, dRLBottom, dRLTop );
		dRLMin[CNR_SOUTH_EAST] = min( dRLMin[CNR_SOUTH_EAST], dRLBottom );
		dRLMax[CNR_SOUTH_EAST] = max( dRLMax[CNR_SOUTH_EAST], dRLTop );

		GetInner()->FindTopBottomRLsAtCornerByLifts( CNR_NORTH_WEST, dRLBottom, dRLTop );
		dRLMin[CNR_SOUTH_WEST] = min( dRLMin[CNR_SOUTH_WEST], dRLBottom );
		dRLMax[CNR_SOUTH_WEST] = max( dRLMax[CNR_SOUTH_WEST], dRLTop );
	}

}

void Bay::SetStandardToRLs( CornerOfBayEnum eCorner, double dRLTop, double dRLBottom )
{
	bool			bSoleboard;
	Point3D			pt;
	SideOfBayEnum	eSideToTest;

	///////////////////////////////////////////////////////////////
	//Do we currently have a soleboard?
	eSideToTest = CornerAsSideOfBay( eCorner );
	bSoleboard	= (HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSideToTest ) );

	///////////////////////////////////////////////////////////////
	//What is the existing bottom RL?
	pt		= GetStandardPosition( eCorner );
	pt.z	= dRLBottom;
	SetStandardPosition( pt, eCorner );

	///////////////////////////////////////////////////////////////
	//adjust the left standard
	CreateStandardsArrangment( (dRLTop-dRLBottom), eCorner, bSoleboard ); 
}

void Bay::AlignBay( double dRequiredWidth )
{
	double		dWidth, dOffset;
	Vector3D	Vector;
	Matrix3D	Transform,
				OriginalBay, OriginalBaySchematic,
				OriginalRun, OriginalRunSchematic;

	OriginalRun				= GetRunPointer()->UnMove();
	OriginalRunSchematic	= GetRunPointer()->UnMoveSchematic();
	OriginalBay				= UnMove();
	OriginalBaySchematic	= UnMoveSchematic();

	dWidth = GetWidthOfStageFromWall( SOUTH );
	dWidth+= GetTemplate()->GetOffsetForShortStage();

	dOffset = dRequiredWidth-dWidth;
	GetTemplate()->SetOffsetForShortStage( dOffset );
	Vector.set( 0.00, dOffset, 0.00 );
	Transform.setToTranslation( Vector );

	Move( Transform, true );
	MoveSchematic( Transform, true );

	MoveSchematic( OriginalBaySchematic, true );
	Move( OriginalBay, true );
	GetRunPointer()->MoveSchematic( OriginalRunSchematic, true );
	GetRunPointer()->Move( OriginalRun, true );

}

double Bay::GetFaceWidth(SideOfBayEnum eFace) const
{
	double		dWidth;
	Component	*pComponent;

	//set the values to default
	dWidth = 0.00;

	pComponent = NULL;

	switch( eFace )
	{
		case( NORTH ):				//fallthrough
		case( SOUTH ):
			if( GetLiftComponent( 0, CT_LEDGER, eFace, LIFT_RISE_0000MM, &pComponent ) )
				dWidth = pComponent->GetLengthCommon();
			break;
		case( EAST ):				//fallthrough
		case( WEST ):
			if( GetLiftComponent( 0, CT_TRANSOM, eFace, LIFT_RISE_0000MM, &pComponent ) )
				dWidth = pComponent->GetLengthCommon();
			break;
		default:					//fallthrough
			assert( FALSE );	//invalid side
			break;
	}

	return dWidth;
}

double Bay::GetFaceHeight(SideOfBayEnum eFace) const
{
	double		dTopRL, dBottomRL;

	//set the values to default
	dTopRL		= 0.00;
	dBottomRL	= 0.00;

	//calculate the height for the surface area
	if(	(eFace==NORTH) || (eFace==EAST) ||
		(eFace==SOUTH) || (eFace==WEST) )
	{
		dTopRL = GetRLOfTopLiftPlusHandrail( eFace );
//		dTopRL-= RLAdjust();
		//The bottom lift is always just above the highest jack
		GetLiftRL( 0, dBottomRL );
		//we need to include the jack in this estimate
		dBottomRL-= JackLength();
	}

	if( dTopRL<dBottomRL )
	{
		//This might happend if there is only one lift
		dBottomRL = dTopRL;
	}
	return dTopRL-dBottomRL;
}

int Bay::GetComponentsRise(int iComponentID) const
{
	return m_caComponents.GetRise(iComponentID);
}

SideOfBayEnum Bay::GetPosition(int iComponentID) const
{
	return m_caComponents.GetPosition( iComponentID );
}

void Bay::ClearAllBayPointers( bool bIgnorNulls/*=true*/, bool bAdjustStandards/*=true*/ )
{
	Bay *pNeighbor;

	pNeighbor = GetForward();
	if( pNeighbor!=NULL )
		pNeighbor->SetBackward(NULL, bIgnorNulls, bAdjustStandards );
	SetForward( NULL, bIgnorNulls, bAdjustStandards );

	pNeighbor = GetBackward();
	if( pNeighbor!=NULL )
		pNeighbor->SetForward(NULL, bIgnorNulls, bAdjustStandards);
	SetBackward( NULL, bIgnorNulls, bAdjustStandards );

	pNeighbor = GetInner();
	if( pNeighbor!=NULL )
		pNeighbor->SetOuter(NULL, bIgnorNulls, bAdjustStandards);
	SetInner( NULL, bIgnorNulls, bAdjustStandards );

	pNeighbor = GetOuter();
	if( pNeighbor!=NULL )
		pNeighbor->SetInner(NULL, bIgnorNulls, bAdjustStandards);
	SetOuter( NULL, bIgnorNulls, bAdjustStandards );
}

void Bay::ReapplyBayPointers( bool bAdjustStandards/*=true*/ )
{
	SetOuter( GetOuter( ), false, bAdjustStandards );
	SetForward( GetForward( ), false, bAdjustStandards );
	SetInner( GetInner( ), false, bAdjustStandards );
	SetBackward( GetBackward( ), false, bAdjustStandards );
}

#define	REPORT_NUMBER_OF_LEVELS

int Bay::GetNumberOfHopupsInBay(int &iNumberOfBoards)
{
	bool	bFoundInThisLift;
	int		i, j, iNumberOfHopups, iBoards;
	Lift	*pLift;

	iNumberOfBoards=0;
	iNumberOfHopups=0;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		iBoards = 0;
		bFoundInThisLift = false;
		for( j=NORTH; j<=WEST; j++ )
		{
			if( (iBoards = pLift->GetNumberOfPlanksOnStageBoard( (SideOfBayEnum)j ))>0 )
			{
				bFoundInThisLift = true;
				if( iNumberOfBoards==0 )
				{
					//This is the first one!
					iNumberOfBoards = iBoards;
				}
				else
				{
					if( j==SOUTH )
					{
						//Southern hopups widths are more important to report the quantity of
						assert( iBoards>0 );
						iNumberOfBoards = iBoards;
					}
				}
#ifndef	REPORT_NUMBER_OF_LEVELS
				//count every north or south hopup as one hopup
				if( j==NORTH || j==SOUTH )
					iNumberOfHopups++;
#endif	REPORT_NUMBER_OF_LEVELS
			}
		}

#ifdef	REPORT_NUMBER_OF_LEVELS
		//Only count one hopup per lift
		//	NOTE: This was added according to Bug#1022 as per mark's request!
		if( bFoundInThisLift )
			iNumberOfHopups++;
#endif	REPORT_NUMBER_OF_LEVELS
	}
	return iNumberOfHopups;
}


void Bay::UserMovedBay( Matrix3D Transform )
{
	//the bay has already been moved
	SetSchematicTransform( GetSchematicTransform()*Transform );
	//But the 3dView hasn't
	Move( Transform, true );
/*	int				i, iNumberOfBaysSelected, iNumberOfRunsSelected, iSize;
	Run				*pRun;
	char			keywrd[255], prompt[255];
	bool			bOnlyOneRun;
	CString			sPrompt, sKeyWrd;
	BayList			Bays;
	JoinTypeEnum	JoinType;

	SelectBays( Bays );

	iNumberOfBaysSelected = Bays.GetSize();

	bOnlyOneRun = true;
	if( iNumberOfBaysSelected>0 )
	{
		///////////////////////////////////////////////////
		//Are we only talking about one run?
		pRun = Bays[0]->GetRunPointer();
		for( i=0; i<iNumberOfBaysSelected; i++ )
		{
			if( Bays[i]->GetRunPointer()!=pRun )
			{
				//We have found another run
				bOnlyOneRun = false;
			}
		}

		///////////////////////////////////////////////////
		//Construct the command prompt line
		sPrompt = "\nWould you like to move the Bay";
		if( iSize>1 )
			sPrompt+= "s";

		sPrompt+= " or the Run";
		if( !bOnlyOneRun )
			sPrompt+= "?";
		else
			sPrompt+= "s? [<Bay>/Run]:";

		for( i=0; i<sPrompt.GetLength(); i++ )
		{
			prompt[i] = sPrompt[i];
		}
		prompt[i] = '\0';
		
		///////////////////////////////////////////////////
		//repeat until valid input
		while( true )
		{
			acedInitGet( RSG_OTHER, "Bay Run");
			int iRet = acedGetKword( prompt, keywrd);
			switch( iRet )
			{
			case( RTERROR ):			//fallthrough (-5001) Some other error
			default:
				//This should not have occured
				MessageBeep(MB_ICONEXCLAMATION);
				continue;	//Unknown error
			case( RTCAN ):				//(-5002) User cancelled request -- Ctl-C
				return;
			case( RTNONE ):				//(5000) No result
				//default to Bay
				keywrd[0] = _T('B');
				//Fallthrough
			case( RTKWORD ):			//(-5005) Keyword returned from getxxx() routine
			case( RTNORM ):				//(5100) Request succeeded
				sKeyWrd = keywrd;
				sKeyWrd.MakeUpper();

				if( sKeyWrd[0]==_T('B') )
				{
					MoveIndividualBays( Transform );
					return;
				}
				else if( sKeyWrd[0]==_T('R') )
				{
					return;
				}
				else 
				{
					//invalid input
					MessageBeep(MB_ICONEXCLAMATION);
					continue;
				}
				break;
			}
		}
	}
*/
}


bool Bay::DoesDeckLiftHaveNeighborDeckAtSameRL( int iLiftID, Bay *pNeighboringBay, bool bInclude500UpOrDown/*=true*/ ) const
{
	int		j;
	Lift	*pLift, *pNeighborLift;
	double	dHeight, dNeighborHeight, dRLAdjust;

	if( pNeighboringBay==NULL )			return false;
	if( iLiftID<0 )						return false;
	if( iLiftID>=GetNumberOfLifts() )	return false;

	pLift = GetLift( iLiftID );
	if( pLift->IsDeckingLift() )
	{
		//it must have a neighbor, or we wouldn't be in here
		GetLiftRL( iLiftID, dHeight );
		for( j=0; j<pNeighboringBay->GetNumberOfLifts(); j++ )
		{
			pNeighborLift = pNeighboringBay->GetLift(j);
			pNeighboringBay->GetLiftRL( j, dNeighborHeight );

			//We need to consider bays which are GetStarSeparation() up or down (see bug#1132)
			for( dRLAdjust=bInclude500UpOrDown? (-1.00*GetStarSeparation()): 0.00;
					bInclude500UpOrDown? (dRLAdjust<GetStarSeparation()+ROUND_ERROR) : (dRLAdjust<ROUND_ERROR);
					dRLAdjust+=GetStarSeparation() )
			{
				if( dHeight<dNeighborHeight+dRLAdjust+ROUND_ERROR && 
					dHeight>dNeighborHeight+dRLAdjust-ROUND_ERROR )
				{
					//We have a matching lift!
					if( pNeighborLift->IsDeckingLift() )
					{
						return true;
					}
				}
				else if( dHeight<dNeighborHeight-GetStarSeparation() )
				{
					//we missed it
					return false;
				}
			}
		}
	}
	return false;
}

bool Bay::DoesLiftHaveNeighborAtSameRL(int iLiftID, Bay *pNeighboringBay, bool bInclude500UpOrDown/*=true*/ ) const
{
	int		j;
	Lift	*pLift, *pNeighborLift;
	double	dHeight, dNeighborHeight;

	if( pNeighboringBay==NULL )			return false;
	if( iLiftID<=0 )					return false;
	if( iLiftID>=GetNumberOfLifts() )	return false;

	pLift = GetLift( iLiftID );

	//it must have a neighbor, or we wouldn't be in here
	GetLiftRL( iLiftID, dHeight );
	double dRLAdjust;
	for( j=0; j<pNeighboringBay->GetNumberOfLifts(); j++ )
	{
		pNeighborLift = pNeighboringBay->GetLift(j);
		pNeighboringBay->GetLiftRL( j, dNeighborHeight );

		//We need to consider bays which are GetStarSeparation() up or down (see bug#1132)
		for( dRLAdjust=bInclude500UpOrDown? (-1.00*GetStarSeparation()): 0.00;
				bInclude500UpOrDown? (dRLAdjust<GetStarSeparation()+ROUND_ERROR) : (dRLAdjust<ROUND_ERROR);
				dRLAdjust+=GetStarSeparation() )
		{
			if( dHeight<dNeighborHeight+dRLAdjust+ROUND_ERROR && 
				dHeight>dNeighborHeight+dRLAdjust-ROUND_ERROR )
			{
				//We have a matching lift!
				return true;
			}
			else if( dHeight<dNeighborHeight-GetStarSeparation() )
			{
				//we missed it
				return false;
			}
		}
	}
	return false;
}


BayList::BayList()
{
	SetUniqueBaysOnly(true);
	SetMustBeSameSystem(true);
}

BayList::~BayList()
{
/*	This is a list of pointers, and it is used in several places to
		point to lists of existing bays, if we delete the bays here
		we run the risk of deleting existing bays
	Bay	*pBay;
	while( GetSize()>0 )
	{
		pBay = NULL;
		pBay = GetAt(0);
		if( pBay!=NULL )
		{
			delete pBay;
		}
		else
		{
			assert( false );
		}
		DeleteAt(0);
	}
*/
}


void Bay::SetSchematicTransform(Matrix3D Transform)
{
	assert( GetTemplate()!=NULL );
	GetTemplate()->SetSchematicTransform( Transform );
}

void Bay::LapboardCrossesSideAdd(SideOfBayEnum eSide)
{
	int iSide;
	iSide = (int)eSide;
	m_iaLapboardCrossesSide.Add( iSide );
	assert( GetNumberOfLapboardPointers()==m_iaLapboardCrossesSide.GetSize() );
}

void Bay::LapboardCrossesSideRemove( int iIndex )
{
	//remove from the array
	m_iaLapboardCrossesSide.RemoveAt( iIndex );
	assert( GetNumberOfLapboardPointers()==m_iaLapboardCrossesSide.GetSize() );
}

SideOfBayEnum Bay::LapboardCrossesSideGet(int iIndex) const
{
	//retreive from the array
	return (SideOfBayEnum) m_iaLapboardCrossesSide.GetAt( iIndex );
}

bool Bay::LapboardCrossesSideDoes(SideOfBayEnum eSide) const
{
	return LapboardCrossesSideWhich(eSide)>=0;
}

void Bay::LapboardCrossesSideRemoveAll()
{
	m_iaLapboardCrossesSide.RemoveAll();
}

int Bay::LapboardCrossesSideWhich(SideOfBayEnum eSide) const
{
	int				i;
	SideOfBayEnum	eEle;

	for( i=0; i<m_iaLapboardCrossesSide.GetSize(); i++ )
	{
		eEle = LapboardCrossesSideGet(i);
		if( eEle==eSide )
		{
			//lapboard does cross this side
			return i;
		}
	}
	return -1;
}



double Bay::GetPreviousLength()
{
	return m_dPreviousLength;
}

void Bay::SetPreviousLength( double dLength )
{
	m_dPreviousLength = dLength;
}

double Bay::GetPreviousWidth()
{
	return m_dPreviousWidth;
}

void Bay::SetPreviousWidth( double dWidth )
{
	m_dPreviousWidth = dWidth;
}

void Bay::ResizeAllBaysInDirection( double dNewSize, SideOfBayEnum eSide )
{
	switch( eSide )
	{
	case( NORTH ):
		SetBayLength( dNewSize );
		GetTemplate()->SetBayLength( dNewSize );
		if( GetOuter()!=NULL )
			GetOuter()->ResizeAllBaysInDirection( dNewSize, NORTH );
		break;
	case( EAST ):
		SetBayWidth( dNewSize );
		GetTemplate()->SetBayWidth( dNewSize );
		if( GetForward()!=NULL )
			GetForward()->ResizeAllBaysInDirection( dNewSize, EAST );
		break;
	case( SOUTH ):
		SetBayLength( dNewSize );
		GetTemplate()->SetBayLength( dNewSize );
		if( GetInner()!=NULL )
			GetInner()->ResizeAllBaysInDirection( dNewSize, SOUTH );
		break;
	case( WEST ):
		SetBayWidth( dNewSize );
		GetTemplate()->SetBayWidth( dNewSize );
		if( GetBackward()!=NULL )
			GetBackward()->ResizeAllBaysInDirection( dNewSize, WEST );
		break;
	default:
		assert( false );
		break;
	}
	GetTemplate()->SetAllSchematicText();
	UpdateSchematicView();
}

void Bay::RepositionStandards()
{
//	bool	bSoleBoards[4];
	double	dWidth, dLength;

	/////////////////////////////////////////////////////////////////////////////////
	//Position the Standards
	//the dimensions of the bay may have changed
	dWidth	= GetBayWidthActual();
	dLength	= GetBayLengthActual();
	if( dWidth>0 && dLength>0 )
	{
		SetStandardPosition( Point3D( 0.00,		0.00,	GetStandardPosition(CNR_SOUTH_WEST).z ), CNR_SOUTH_WEST );
		SetStandardPosition( Point3D( 0.00,		dWidth,	GetStandardPosition(CNR_NORTH_WEST).z ), CNR_NORTH_WEST );
		SetStandardPosition( Point3D( dLength,	dWidth,	GetStandardPosition(CNR_NORTH_EAST).z ), CNR_NORTH_EAST );
		SetStandardPosition( Point3D( dLength,	0.00,	GetStandardPosition(CNR_SOUTH_EAST).z ), CNR_SOUTH_EAST );
	}
}

Bay &Bay::CopyAllComponents(const Bay &OriginalBay, BayList *pBays/*=NULL*/ )
{
	int					j;
	bool				bBracing[4], bCornerOfTies[8];
	double				dVertSep;
	BayTieTubeTemplate	*pTTT, *pTieTubeTemplate;
	BayTieTubeTemplates LooseTieTubes;

	//////////////////////////////////////////////////
	//Store the wall ties and tie tubes
	bCornerOfTies[ NORTH_NORTH_EAST	-NORTH_NORTH_EAST ] = GetTemplate()->GetNNETie();
	bCornerOfTies[ EAST_NORTH_EAST	-NORTH_NORTH_EAST ] = GetTemplate()->GetENETie();
	bCornerOfTies[ EAST_SOUTH_EAST	-NORTH_NORTH_EAST ] = GetTemplate()->GetESETie();
	bCornerOfTies[ SOUTH_SOUTH_EAST	-NORTH_NORTH_EAST ] = GetTemplate()->GetSSETie();
	bCornerOfTies[ SOUTH_SOUTH_WEST	-NORTH_NORTH_EAST ] = GetTemplate()->GetSSWTie();
	bCornerOfTies[ WEST_SOUTH_WEST	-NORTH_NORTH_EAST ] = GetTemplate()->GetWSWTie();
	bCornerOfTies[ WEST_NORTH_WEST	-NORTH_NORTH_EAST ] = GetTemplate()->GetWNWTie();
	bCornerOfTies[ NORTH_NORTH_WEST	-NORTH_NORTH_EAST ] = GetTemplate()->GetNNWTie();

	//Store tie tubes
	for( j=0; j<GetTemplate()->GetNumberOfTieTubeTemplates(); j++ )
	{
		pTTT = GetTemplate()->GetTieTubeTemplate(j);
		pTieTubeTemplate = new BayTieTubeTemplate();
		*pTieTubeTemplate = *pTTT;

		LooseTieTubes.Add( pTieTubeTemplate );
	}
	dVertSep = GetTemplate()->GetTieTubeVertSeparation();


	//////////////////////////////////////////////////
	//Store the bracing setting
	bBracing[N] = GetTemplate()->GetNBrace();
	bBracing[E] = GetTemplate()->GetEBrace();
	bBracing[S] = GetTemplate()->GetSBrace();
	bBracing[W] = GetTemplate()->GetWBrace();

	/////////////////////////////////////////////////
	//Copy the edited bay to the selected bay
	CopyBay(OriginalBay, pBays);

	///////////////////////////////////////////////////////////////////////////
	//Restore the bracing setting
	for( j=NORTH; j<=WEST; j++ )
	{
		//We may have added a stage board, if we did then don't create the brace
		if( bBracing[j] && GetWidthOfStageFromWall((SideOfBayEnum)j)<=0 )
			CreateBracingArrangment ( (SideOfBayEnum)j, MT_STEEL, false );
		else
			DeleteBracing( (SideOfBayEnum)j );
	}

	/////////////////////////////////////////////////////////////////////
	//Create Wall Ties
	DeleteAllComponentsOfType(CT_TIE_TUBE);
	DeleteAllComponentsOfType(CT_TIE_CLAMP_90DEGREE);
	DeleteAllComponentsOfType(CT_TIE_CLAMP_COLUMN);
	DeleteAllComponentsOfType(CT_TIE_CLAMP_MASONARY);
	DeleteAllComponentsOfType(CT_TIE_CLAMP_YOKE);

	int		iCnr, iSide;
	double	dHeight, dStandardsHeight, dTieLength;


	for( iSide=(int)NORTH_NORTH_EAST; iSide<=(int)NORTH_NORTH_WEST; iSide++ ) 
	{
		if( bCornerOfTies[iSide-NORTH_NORTH_EAST] )
		{
			switch( (SideOfBayEnum)iSide )
			{
			case( NORTH_NORTH_EAST ):	iCnr = CNR_NORTH_EAST;	break;
			case( EAST_NORTH_EAST ):	iCnr = CNR_NORTH_EAST;	break;
			case( EAST_SOUTH_EAST ):	iCnr = CNR_SOUTH_EAST;	break;
			case( SOUTH_SOUTH_EAST ):	iCnr = CNR_SOUTH_EAST;	break;
			case( SOUTH_SOUTH_WEST ):	iCnr = CNR_SOUTH_WEST;	break;
			case( WEST_SOUTH_WEST ):	iCnr = CNR_SOUTH_WEST;	break;
			case( WEST_NORTH_WEST ):	iCnr = CNR_NORTH_WEST;	break;
			case( NORTH_NORTH_WEST ):	iCnr = CNR_NORTH_WEST;	break;
			default:
				assert( false );
			}

			dTieLength = GetSmallestRequiredWallTieLength((SideOfBayEnum)iSide);

			dHeight = GetRunPointer()->GetTiesVerticallyEvery();
			dStandardsHeight = GetHeightOfStandards( (CornerOfBayEnum)iCnr );

			if( dHeight>=dStandardsHeight )
			{
				//we need at least one tie
				AddComponent( CT_TIE_TUBE, (SideOfBayEnum)iSide, dStandardsHeight, dTieLength, MT_STEEL );
			}
			else
			{
				while( dHeight<dStandardsHeight )
				{
					AddComponent( CT_TIE_TUBE, (SideOfBayEnum)iSide, dHeight, dTieLength, MT_STEEL );
					dHeight+= GetRunPointer()->GetTiesVerticallyEvery();
				}
			}
		}
		else
		{
			switch( (SideOfBayEnum)iSide )
			{
			case( NORTH_NORTH_EAST ):	GetTemplate()->SetNNETie( false ); break;
			case( EAST_NORTH_EAST ):	GetTemplate()->SetENETie( false ); break;
			case( EAST_SOUTH_EAST ):	GetTemplate()->SetESETie( false ); break;
			case( SOUTH_SOUTH_EAST ):	GetTemplate()->SetSSETie( false ); break;
			case( SOUTH_SOUTH_WEST ):	GetTemplate()->SetSSWTie( false ); break;
			case( WEST_SOUTH_WEST ):	GetTemplate()->SetWSWTie( false ); break;
			case( WEST_NORTH_WEST ):	GetTemplate()->SetWNWTie( false ); break;
			case( NORTH_NORTH_WEST ):	GetTemplate()->SetNNWTie( false ); break;
			default:
				assert( false );
			}
		}
	}

/*	///////////////////////////////////////////////////////////////////////////
	//Restore free tie tubes
	double				dRL, dBottomBayRL, dTopBayRL;
	Point3D				ptSE, ptSW;
	Component			*pTieTube, *pClamp1, *pClamp2;
	Matrix3D			Transform, Rotation, Trans;
	Vector3D			Vector;

	GetTemplate()->RemoveTieTubeTemplate( 0, GetTemplate()->GetNumberOfTieTubeTemplates() );

	for( j=0; j<LooseTieTubes.GetSize(); j++ )
	{
		pTieTube	= CreateComponentDumb( CT_TIE_TUBE,				CornerAsSideOfBay( pTTT->m_eStandard ), 0, pTTT->m_dFullLength, MT_STEEL );
		pClamp1		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( pTTT->m_eStandard ), 0, 0.00,				MT_STEEL );
		pClamp2		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( pTTT->m_eStandard ), 0, 0.00,				MT_STEEL );

		pTTT = LooseTieTubes.GetAt(j);
		pTieTubeTemplate = new BayTieTubeTemplate();
		*pTieTubeTemplate = *pTTT;
		GetTemplate()->AddTieTubeTemplate( pTieTubeTemplate );

		GetRLsForBayByHeights( dBottomBayRL, dTopBayRL );

		Vector.set( 0.00, 0.00, 1.00 );
		Rotation.setToRotation( pTTT->m_dAngle, Vector );

		for( dRL = min( (dBottomBayRL+dVertSep), dTopBayRL );
							dRL<=dTopBayRL; dRL+=dVertSep )
		{
			pTieTube	= CreateComponentDumb( CT_TIE_TUBE,				CornerAsSideOfBay( pTTT->m_eStandard ), 0, pTTT->m_dFullLength, MT_STEEL );
			pClamp1		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( pTTT->m_eStandard ), 0, 0.00,				MT_STEEL );
			pClamp2		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( pTTT->m_eStandard ), 0, 0.00,				MT_STEEL );

			//The 2nd clamp belongs at the other end of the tie tube
			Vector.set( -1.00*OVERRUN_TIE_TUBE, 0.00, 0.00 );
			Transform.setToTranslation( Vector );
			pTieTube->Move( Transform, true );

			//The 2nd clamp belongs at the other end of the tie tube
			Vector.set( pTTT->m_dFullLength-OVERRUN_TIE_TUBE-pClamp2->GetLengthActual(), 0.00, 0.00 );
			Transform.setToTranslation( Vector );
			pClamp2->Move( Transform, true );

			/////////////////////////////
			//3D Movement horizontally within Bay
			switch( pTTT->m_eStandard )
			{
			case( CNR_NORTH_EAST ):
				Vector.set( GetBayLengthActual(), GetBayWidthActual(), 0.00 );
				break;
			case( CNR_SOUTH_EAST ):
				Vector.set( GetBayLengthActual(), 0.00, 0.00 );
				break;
			case( CNR_SOUTH_WEST ):
				Vector.set( 0.00, 0.00, 0.00 );
				break;
			case( CNR_NORTH_WEST ):
				Vector.set( 0.00, GetBayWidthActual(), 0.00 );
				break;
			default:
				assert( false );
			}
			Transform.setToTranslation( Vector );

			//find the standards for the bay
			Transform*= Rotation;

			/////////////////////////////
			//3D Movement vertically within Bay
			Vector.set( 0.00, 0.00, dRL );
			Trans.setToTranslation( Vector );
			Transform*= Trans;

			//Apply movement within bay!
			pTieTube->Move( Transform, true );
			pClamp1->Move( Transform, true );
			pClamp2->Move( Transform, true );
		}
	}

	LooseTieTubes.DeleteAll();
*/	return *this;
}

void Bay::ReapplyShadeAndChainOnAllSides()
{
	//replace all shade cloth and chain link
	int iSide;
	for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
	{
		/////////////////////////////////////////////////////////////////////////////////
		//Chain link
		if( DoesABayComponentExistOnASide( CT_CHAIN_LINK, (SideOfBayEnum)iSide ) )
		{
			DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, (SideOfBayEnum)iSide );
			AddAChainLink( (SideOfBayEnum)iSide );
		}
	
		/////////////////////////////////////////////////////////////////////////////////
		//Shade cloth
		if( DoesABayComponentExistOnASide( CT_SHADE_CLOTH, (SideOfBayEnum)iSide ) )
		{
			DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, (SideOfBayEnum)iSide );
			AddAShadeCloth( (SideOfBayEnum)iSide );
		}
	}
}

bool Bay::HasBayComponentOfTypeOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide) const
{
	int			i;
	bool		bFound;
	Component	*pComp;

	bFound = false;
	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComp = GetBayComponent( i );
		if( pComp->GetType()==eType &&
			GetPosition( i )==eSide )
		{
			bFound = true;
			break;
		}
	}
	return bFound;
}

bool Bay::HasComponentOfTypeOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide)
{
	if( HasBayComponentOfTypeOnSide( eType, eSide) )
		return true;

	int		iLift;
	Lift	*pLift;
	for( iLift=0; iLift<GetNumberOfLifts(); iLift++ )
	{
		pLift = GetLift( iLift );
		if( pLift->HasComponentOfTypeOnSide( eType, eSide ) )
			return true;
	}
	return false;
}


bool Bay::MoveLiftUp(int iLiftID)
{
	return MoveLift( iLiftID, true );
}

bool Bay::MoveLiftDown(int iLiftID)
{
	return MoveLift( iLiftID, true );
}

bool Bay::MoveLift( int iLiftID, bool bUp)
{
	int		iLifts;
	CString	sMsg;

	iLifts = GetNumberOfLifts();
	if( bUp && iLiftID>=iLifts-1 )
	{
		sMsg = _T("This is already the top lift, there are no more lifts to move it above!\n");
		sMsg+= _T("I suggest that you choose 'insert' lift instead!");
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox( NULL, sMsg, _T("Error"), MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	if( !bUp && iLiftID<=0 )
	{
		sMsg = _T("This is already the bottom lift, there are no more lifts to move it below!\n");
		sMsg+= _T("I suggest that you choose 'insert' lift instead!");
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox( NULL, sMsg, _T("Error"), MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	//swap the lifts!
	LiftListElement	*pTempLift;
	LiftListElement	*pOtherLift;
	double	dTempRL;
	
	pTempLift = m_LiftList.GetAt( iLiftID );

	if( bUp )
	{
		GetLiftRL( iLiftID, dTempRL );
		pOtherLift = m_LiftList.GetAt( iLiftID+1 );
		m_LiftList.SetAt( iLiftID, pOtherLift );
		//The RL for the new lower lift is the same as the RL for the old lower lift
		m_LiftList.SetRL( iLiftID, dTempRL );
		
		//the RL for the new top lift could be different
		dTempRL+= GetRiseFromRiseEnum( GetLift( iLiftID )->GetRise(), GetStarSeparation() );
		m_LiftList.SetAt( iLiftID+1, pTempLift );
		m_LiftList.SetRL( iLiftID+1, dTempRL );

		//swap their ID's
		GetLift( iLiftID )->SetLiftID( iLiftID );
		GetLift( iLiftID+1 )->SetLiftID( iLiftID+1 );
	}
	else
	{
		GetLiftRL( iLiftID-1, dTempRL );
		pOtherLift = m_LiftList.GetAt( iLiftID-1 );
		m_LiftList.SetAt( iLiftID, pOtherLift );

		//the RL for the new top lift could be different
		m_LiftList.SetAt( iLiftID-1, pTempLift );

		//The RL for the new lower lift is the same as the RL for the old lower lift
		m_LiftList.SetRL( iLiftID-1, dTempRL );

		dTempRL+= GetRiseFromRiseEnum( GetLift( iLiftID )->GetRise(), GetStarSeparation() );
		m_LiftList.SetRL( iLiftID, dTempRL );

		//swap their ID's
		GetLift( iLiftID )->SetLiftID( iLiftID );
		GetLift( iLiftID-1 )->SetLiftID( iLiftID-1 );
	}

	return true;
}

void Bay::DeleteUnneeded1000mmHopups( Bay *pNeighborBay, SideOfBayEnum eNeighborDirection )
{
	SideOfBayEnum eSideComp1, eSideComp2, eSideStage1, eSideStage2;

	if( pNeighborBay==NULL )
		return;

	switch( eNeighborDirection )
	{
	case( NORTH ):
		eSideComp1	= ENE;
		eSideComp2	= WNW;
		eSideStage1	= E;
		eSideStage2	= W;
		break;
	case( EAST ):
		eSideComp1	= SSE;
		eSideComp2	= NNE;
		eSideStage1	= S;
		eSideStage2	= N;
		break;
	case( SOUTH ):
		eSideComp1	= ESE;
		eSideComp2	= ENE;
		eSideStage1	= E;
		eSideStage2	= W;
		break;
	case( WEST ):
		eSideComp1	= SSW;
		eSideComp2	= NNW;
		eSideStage1	= S;
		eSideStage2	= N;
		break;
	default:
		assert( false );
	}


	Lift		*pLift, *pLiftNeighbor;
	double		dRL, dRLNeighbor;
	Component	*pComp;
	for( int i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		assert( pLift!=NULL );
		GetLiftRL( i, dRL );
		pLiftNeighbor = pNeighborBay->GetLiftAtRL( dRL );
		if( pLiftNeighbor!=NULL )
		{
			dRLNeighbor = pLiftNeighbor->GetRL();
			if( dRL>dRLNeighbor-ROUND_ERROR && dRL<dRLNeighbor+ROUND_ERROR )
			{
				pLift->DeleteUnneeded1000mmHopup( pLiftNeighbor, eSideComp1, eSideStage1 );
				pLift->DeleteUnneeded1000mmHopup( pLiftNeighbor, eSideComp2, eSideStage2 );
			}
		}
		else
		{
			//There is no neighboring lift!
			if( !pLift->HasComponentOfTypeOnSide( CT_RAIL, eNeighborDirection ) )
			{
				//There is no railing on this side so we had better remove the hopup
				pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideComp1 );
				if( pComp!=NULL )
					pLift->DeleteComponent( pComp->GetID() );

				pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideComp2 );
				if( pComp!=NULL )
					pLift->DeleteComponent( pComp->GetID() );
			}
		}
	}
}

bool Bay::WasBayWidthChangedDuringEdit()
{
	return m_bBayWidthChangedDuringEdit;
}

bool Bay::WasBayLengthChangedDuringEdit()
{
	return m_bBayLengthChangedDuringEdit;
}

void Bay::SetBayWidthChangedDuringEdit(bool bResized)
{
	m_bBayWidthChangedDuringEdit = bResized;
}

void Bay::SetBayLengthChangedDuringEdit(bool bResized)
{
	m_bBayLengthChangedDuringEdit = bResized;
}


void Bay::SetBaySizeMultipleSelection(bool bDifferentSizes)
{
	m_bBaySizeMultipleSelection = bDifferentSizes;
}

bool Bay::IsBaySizeMultipleSelection()
{
	return m_bBaySizeMultipleSelection;
}

void Bay::AddHopupToBayIfReqd(SideOfBayEnum eSide, int iNumberOfBoards)
{
	Lift			*pLift;
	SideOfBayEnum	eSideLeft, eSideRight;

	switch( eSide )
	{
	case( N ):
		eSideLeft	= WNW;
		eSideRight	= ENE;
		break;
	case( E ):
		eSideLeft	= NNE;
		eSideRight	= SSE;
		break;
	case( S ):
		eSideLeft	= ESE;
		eSideRight	= WSW;
		break;
	case( W ):
		eSideLeft	= SSW;
		eSideRight	= NNW;
		break;
	default:
		assert( false );
	}

	if( iNumberOfBoards>0 )
	{
		//They have not specifically selected this bay, and there are
		//	hopups on the east side, so we need to recreate the hopups
		for( int iLiftID=0; iLiftID<GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetLift( iLiftID );
			if( pLift->IsDeckingLift() )
			{
				//This has a deck so add hopup, removing any handrail!
				AddStageBoardCheck( eSide, false );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
				if( eSide==NORTH || eSide==NORTH )
					pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );

				pLift->AddStage( eSide, iNumberOfBoards, MT_TIMBER );
			}
		}

		Delete1000mmHopupRailFromSide( eSideLeft );
		Delete1000mmHopupRailFromSide( eSideRight );
	}
}

void Bay::SetBayLengthDumb(double dNewLength)
{
	SetPreviousLength( GetBayLength() );
	m_dBayLength		= dNewLength;
}

void Bay::SetBayWidthDumb(double dNewWidth)
{
	SetPreviousLength( GetBayLength() );
	m_dBayWidth		= dNewWidth;
}

void Bay::RecreateAllLapboards( BayList *pBays/*=NULL*/ )
{
	Bay			*pBayWest, *pBayEast;
	bool		bBothBaysInSelection;
	LapboardBay	*pLap, *pLapboard;

	if( GetNumberOfLapboardPointers()<=0 )
		return;

	assert( GetRunPointer()!=NULL );
	assert( GetRunPointer()->GetController()!=NULL );
	for( int i=0; i<GetRunPointer()->GetController()->GetNumberOfLapboards(); i++ )
	{
		//find any lapboard that points to this bay
		pLap = GetRunPointer()->GetController()->GetLapboard( i );
		assert( pLap!=NULL );
		pBayWest = pLap->GetBayWest();
		pBayEast = pLap->GetBayEast();

		if( pBayEast==this || pBayWest==this )
		{
			//////////////////////////////////////////////
			//Create a new one, by copying the old one, only the number of decks might change
			pLapboard = GetController()->CreateNewLapboard();
			pLapboard->SetBayWidth(		pLap->GetBayWidth() );
			pLapboard->SetBayLength(	pLap->GetBayLength() );
			*pLapboard->GetTemplate() = *pLap->GetTemplate();

			//find out which side this crosses!
			int iIndexWest, iIndexEast;
			SideOfBayEnum eSideWest, eSideEast;
			if( pBayWest!=NULL )
			{
				iIndexWest	= pBayWest->FindLapboardIndex( pLap );
				eSideWest	= pBayWest->LapboardCrossesSideGet( iIndexWest );
			}
			if( pBayEast!=NULL )
			{
				iIndexEast	= pBayEast->FindLapboardIndex( pLap );
				eSideEast	= pBayEast->LapboardCrossesSideGet( iIndexEast );
			}

			/////////////////////////////////////////////////////////////////////////
			//See Bug #1319 - We should not show the warning if both the decks
			//	are in the selection set (otherwise we will change one, show
			//	warning, then change the other to a valid value).
			bBothBaysInSelection = false;
			if( pBays!=NULL && pBays->IsInList( pBayWest ) && pBays->IsInList( pBayEast ) )
				bBothBaysInSelection = true;
			//Setting the pointer will also adjust the number of decks for this lapboard
			pLapboard->SetBayPointers( pBayWest, pBayEast, eSideWest, eSideEast, bBothBaysInSelection );
			//draw the new one
			pLapboard->CreateSchematic();
			pLapboard->Move( pLap->GetTransform(), true );
			pLapboard->MoveSchematic( pLap->GetSchematicTransform(), true );

			////////////////////////////////////////////////////////////
			//Remove the old lapboard from the list
			GetRunPointer()->GetController()->DeleteLapboard( i );
		}
	}
}


int Bay::GetNumberOfLapboardPointers() const
{
	return m_pLapboards->GetSize();
}

void Bay::SetForwardDumb(const Bay *pBay)
{
	m_pForward = (Bay*)pBay;	
}

void Bay::SetBackwardDumb(const Bay *pBay)
{
	m_pBackward = (Bay*)pBay;
}

void Bay::SetInnerDumb(const Bay *pBay)
{
	m_iLoadedInnerRunID=ID_INVALID;
	m_iLoadedInnerBayID=ID_INVALID;

	m_pInner = (Bay*)pBay;
}

void Bay::SetOuterDumb(const Bay *pBay)
{
	m_iLoadedOuterRunID=ID_INVALID;
	m_iLoadedOuterBayID=ID_INVALID;

	m_pOuter = (Bay*)pBay;
}

void BayList::DisplaySelectedBays()
{
	int i;

	if( GetSize()>0 )
	{
		if( GetSize()>1 )
		{
      acutPrintf( _T("\n%i Bays selected: %i"), GetSize(), GetAt(0)->GetBayNumber() );
			for( i=1; i<GetSize()-1; i++ )
			{
				acutPrintf( _T(", %i"), GetAt(i)->GetBayNumber() );
			}
			for( ; i<GetSize(); i++ )
			{
				acutPrintf( _T(" & %i\n"), GetAt(i)->GetBayNumber() );
			}
		}
		else
		{
      acutPrintf( _T("\nBay selected: %i\n"), GetAt(0)->GetBayNumber() );
		}
	}
	else
	{
		acutPrintf( _T("\nNo Bays Selected!\n") );
	}
}

bool Bay::IsDrawAllowed()
{
	assert( m_iAllowDraw>=0 );
	return m_iAllowDraw==0;

}	

void Bay::SetAllowDraw(bool bAllow)
{
	if( bAllow )
		m_iAllowDraw--;
	else
		m_iAllowDraw++;

	if( m_iAllowDraw<0 )
	{
		m_iAllowDraw = 0;
	}
}

void Bay::ClearAllowDraw()
{
	m_iAllowDraw=0;
}

bool Bay::Delete1000mmHopupRailFromSide(SideOfBayEnum eSide)
{
	int			i;
	Lift		*pLift;
	bool		bDeleted;
	Component	*pComp;
	CornerOfBayEnum	eCnr;

	assert( eSide>=NORTH_NORTH_EAST );
	assert( eSide<=NORTH_NORTH_WEST );

	switch( eSide )
	{
	case( NORTH_NORTH_EAST ):
	case( EAST_NORTH_EAST ):
		eCnr = CNR_NORTH_EAST;
		break;
	case( EAST_SOUTH_EAST ):
	case( SOUTH_SOUTH_EAST ):
		eCnr = CNR_SOUTH_EAST;
		break;
	case( SOUTH_SOUTH_WEST ):
	case( WEST_SOUTH_WEST ):
		eCnr = CNR_SOUTH_WEST;
		break;
	case( WEST_NORTH_WEST ):
	case( NORTH_NORTH_WEST ):
		eCnr = CNR_NORTH_WEST;
		break;
	default:
		assert( false );
		return false;
	}


	bDeleted = false;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		assert( pLift!=NULL );
		pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSide );
		if( pComp!=NULL )
		{
			pLift->DeleteComponent( pComp->GetID() );
			bDeleted = true;

			AutoAdjustStandard( eCnr );
		}

	}
	return bDeleted;
}


void Bay::AddTransomIfReqd( int iLiftId, bool bWest/*=true*/ )
{
	Lift	*pLift, *pLiftNeighbor;

	pLift = GetLift( iLiftId );
	assert( pLift!=NULL );

	if( bWest )
	{
		if( GetBackward()==NULL )
		{
			//This is the first bay in a run!
			if( !pLift->HasComponentOfTypeOnSide( CT_TRANSOM, WEST ) )
			{
				//we are missing a tranny so add one!
				pLift->AddComponent( CT_TRANSOM, GetBayWidth(), WEST, LIFT_RISE_0000MM, MT_STEEL );
			}
		}
		else
		{
			double dRLOfTranny;
			dRLOfTranny = pLift->GetRL();
			pLiftNeighbor = GetBackward()->GetLiftAtRL( dRLOfTranny );

			if( pLiftNeighbor!=NULL )
			{
				if( !GetBackward()->HasComponentAtRL( CT_TRANSOM, dRLOfTranny, EAST ) )
				{
					double dRLOfTrannyAboveLift;
					dRLOfTrannyAboveLift = dRLOfTranny;
					dRLOfTrannyAboveLift-= pLiftNeighbor->GetRL();
					LiftRiseEnum eRise;
					eRise = GetRiseEnumFromRise(dRLOfTrannyAboveLift);
					pLiftNeighbor->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, eRise, MT_STEEL );
				}
			}
			else
			{
				/////////////////////////////////////////////
				//SPECIAL CASE!!!
				//	The previous bay does not have a lift at this RL, so we must
				//	add the Trannie to this bay's lift!
				if( !pLift->HasComponentOfTypeOnSide( CT_TRANSOM, WEST ) )
				{
					//we are missing a tranny so add one!
					pLift->AddComponent( CT_TRANSOM, GetBayWidth(), WEST, LIFT_RISE_0000MM, MT_STEEL );
				}
			}
		}
	}
	else
	{
		if( GetForward()==NULL )
		{
			//This is the last bay in a run!
			if( !pLift->HasComponentOfTypeOnSide( CT_TRANSOM, EAST ) )
			{
				//we are missing a tranny so add one!
				pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, LIFT_RISE_0000MM, MT_STEEL );
			}
		}
		else
		{
			double dRLOfTranny;
			dRLOfTranny = pLift->GetRL();
			pLiftNeighbor = GetForward()->GetLiftAtRL( dRLOfTranny );

			if( pLiftNeighbor!=NULL )
			{
				if( GetForward()->HasComponentAtRL( CT_TRANSOM, dRLOfTranny, WEST ) )
				{
					double dRLOfTrannyAboveLift;
					dRLOfTrannyAboveLift = dRLOfTranny;
					dRLOfTrannyAboveLift-= pLiftNeighbor->GetRL();
					LiftRiseEnum eRise;
					eRise = GetRiseEnumFromRise(dRLOfTrannyAboveLift);
					pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, eRise, MT_STEEL );
				}
			}
			else
			{
				/////////////////////////////////////////////
				//SPECIAL CASE!!!
				//	The next bay does not have a lift at this RL, so we must
				//	add the Trannie to this bay's lift!
				if( !pLift->HasComponentOfTypeOnSide( CT_TRANSOM, EAST ) )
				{
					//we are missing a tranny so add one!
					pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, LIFT_RISE_0000MM, MT_STEEL );
				}
			}
		}
	}
}

void Bay::AddLedgerIfReqd(int iLiftId, bool bSouth/*=true*/)
{
	Lift	*pLift, *pLiftNeighbor;

	pLift = GetLift( iLiftId );
	assert( pLift!=NULL );

	if( bSouth )
	{
		if( GetInner()==NULL )
		{
			//This is the first bay in a run!
			if( !pLift->HasComponentOfTypeOnSide( CT_LEDGER, SOUTH ) )
			{
				//we are missing a ledger so add one!
				pLift->AddComponent( CT_LEDGER, GetBayLength(), SOUTH, LIFT_RISE_0000MM, MT_STEEL );
			}
		}
		else
		{
			double dRLOfTranny;
			dRLOfTranny = GetRLOfLift(iLiftId);
			pLiftNeighbor = GetInner()->GetLiftAtRL( dRLOfTranny );

			if( pLiftNeighbor!=NULL )
			{
				if( !GetInner()->HasComponentAtRL( CT_LEDGER, dRLOfTranny, NORTH ) )
				{
					double dRLOfTrannyAboveLift;
					dRLOfTrannyAboveLift = dRLOfTranny;
					dRLOfTrannyAboveLift-= pLiftNeighbor->GetRL();
					LiftRiseEnum eRise;
					eRise = GetRiseEnumFromRise(dRLOfTrannyAboveLift);
					pLift->AddComponent( CT_LEDGER, GetBayLength(), SOUTH, eRise, MT_STEEL );
				}
			}
			else
			{
				/////////////////////////////////////////////
				//SPECIAL CASE!!!
				//	The previous bay does not have a lift at this RL, so we must
				//	add the Ledger to this bay's lift!
				if( !pLift->HasComponentOfTypeOnSide( CT_LEDGER, SOUTH ) )
				{
					//we are missing a tranny so add one!
					pLift->AddComponent( CT_LEDGER, GetBayLength(), SOUTH, LIFT_RISE_0000MM, MT_STEEL );
				}
			}
		}
	}
	else
	{
		if( GetOuter()==NULL )
		{
			//This is the first bay in a run!
			if( !pLift->HasComponentOfTypeOnSide( CT_LEDGER, NORTH ) )
			{
				//we are missing a ledger so add one!
				pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, LIFT_RISE_0000MM, MT_STEEL );
			}
		}
		else
		{
			double dRLOfTranny;
			dRLOfTranny = GetRLOfLift(iLiftId);
			pLiftNeighbor = GetOuter()->GetLiftAtRL( dRLOfTranny );

			if( pLiftNeighbor!=NULL )
			{
				if( !GetOuter()->HasComponentAtRL( CT_LEDGER, dRLOfTranny, SOUTH ) )
				{
					double dRLOfTrannyAboveLift;
					dRLOfTrannyAboveLift = dRLOfTranny;
					dRLOfTrannyAboveLift-= pLiftNeighbor->GetRL();
					LiftRiseEnum eRise;
					eRise = GetRiseEnumFromRise(dRLOfTrannyAboveLift);
					pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, eRise, MT_STEEL );
				}
			}
			else
			{
				/////////////////////////////////////////////
				//SPECIAL CASE!!!
				//	The previous bay does not have a lift at this RL, so we must
				//	add the Ledger to this bay's lift!
				if( !pLift->HasComponentOfTypeOnSide( CT_LEDGER, NORTH ) )
				{
					//we are missing a tranny so add one!
					pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, LIFT_RISE_0000MM, MT_STEEL );
				}
			}
		}
	}
}


void Bay::AddComponentToNeighborIfReqd(int iLiftId, ComponentTypeEnum eType, SideOfBayEnum eSideRemove, SideOfBayEnum eSideAdd, Bay *pBayNeighbor)
{
	double dLength;
	if( GetLift(iLiftId)->HasComponentOfTypeOnSide( eType, eSideRemove ) && pBayNeighbor!=NULL )
	{
		//we have a neighboring bay, and we are about to delete a component that is may require, so better replace it!
		Lift	*pLiftNeighbor;
		double	dRLOfLift, dRLOfNeighborLift;

		dRLOfLift = GetRLOfLift(iLiftId);
		pLiftNeighbor = pBayNeighbor->GetLiftAtRL( dRLOfLift );
		if( pLiftNeighbor!=NULL )
		{
			//we have a neighboring lift that covers this RL, are the lifts at the same height?
			dRLOfNeighborLift = pLiftNeighbor->GetRL();
			if( dRLOfNeighborLift==dRLOfLift )
			{
				//Yep the neighboring lift requires this component!
				dLength = (eSideAdd==N||eSideAdd==S)? GetBayLength(): GetBayWidth();

				pLiftNeighbor->AddComponent( eType, dLength, eSideAdd, LIFT_RISE_0000MM, MT_STEEL );
			}
		}
	}

}

Component * Bay::GetStandardAtRL(double dRL, CornerOfBayEnum eCnr)
{
	Bay				*pBay;
	double			dStdRL, dStdRLMin, dStdRLMax;
	doubleArray		daStds;
	CornerOfBayEnum	eCorner;

	GetStandardsArrangement( daStds, eCnr );

	dStdRL	= GetStandardPosition( eCnr ).z;
	pBay	= this;
	eCorner = eCnr;
	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):
		break;
	case( CNR_SOUTH_EAST ):
		if( GetInner()!=NULL )
		{
			pBay = GetInner();
			eCorner = CNR_NORTH_EAST;
		}
		break;
	case( CNR_SOUTH_WEST ):
		if( GetBackward()!=NULL )
		{
			if( GetBackward()->GetInner()!=NULL )
			{
				pBay = GetBackward()->GetInner();
				eCorner = CNR_NORTH_EAST;
			}
			else
			{
				pBay = GetBackward();
				eCorner = CNR_SOUTH_EAST;
			}
		}
		else if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
			{
				pBay = GetInner()->GetBackward();
				eCorner = CNR_NORTH_EAST;
			}
			else
			{
				pBay = GetInner();
				eCorner = CNR_NORTH_WEST;
			}
		}
		break;
	case( CNR_NORTH_WEST ):
		if( GetBackward()!=NULL )
		{
			pBay = GetBackward();
			eCorner = CNR_NORTH_EAST;
		}
		break;
	default:
		assert( false );
	}
	for( int i=0; i<daStds.GetSize(); i++ )
	{
		dStdRLMin = dStdRL-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;

		dStdRL   += daStds.GetAt( i );
		dStdRLMax = dStdRL+EXTRA_LENGTH_AT_TOP_OF_STANDARD;

		if( dRL>=dStdRLMin-ROUND_ERROR && dRL<=dStdRLMax+ROUND_ERROR )
		{
			return pBay->GetStandard( i, eCorner );
		}
	}
	return NULL;
}

bool Bay::DeleteBayComponent(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise)
{
	Component *pComp;

	pComp = GetBayComponent( eType, eSide, iRise );
	if( pComp!=NULL )
	{
		return m_caComponents.DeleteComponent( pComp->GetID() );
	}
	return false;
}

Component * Bay::GetBayComponent(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise)
{
	Component *pCompon;
	for( int iLoop=0; iLoop<GetNumberOfBayComponents(); iLoop++ )
	{
		pCompon = m_caComponents.GetComponent( iLoop );
		if( pCompon->GetType()==eType &&
			pCompon->GetSideOfBay()==eSide &&
			m_caComponents.GetRise(iLoop)==iRise )
		{
			return pCompon;
		}
	}
	return NULL;		
}

bool Bay::DeleteAllComponentsOfType(ComponentTypeEnum eType)
{
	int			i;
	bool		bReturn;
	Lift		*pLift;
	Component	*pComponent;

	bReturn = false;
	switch( eType )
	{
	case( CT_TOE_BOARD ):			//fallthrough
	case( CT_DECKING_PLANK ):		//fallthrough
	case( CT_LAPBOARD ):			//fallthrough
	case( CT_STAGE_BOARD ):			//fallthrough
	case( CT_LEDGER ):				//fallthrough
	case( CT_TRANSOM ):				//fallthrough
	case( CT_MESH_GUARD ):			//fallthrough
	case( CT_RAIL ):				//fallthrough
	case( CT_MID_RAIL ):			//fallthrough
	case( CT_STAIR ):				//fallthrough
	case( CT_LADDER ):				//fallthrough
	case( CT_HOPUP_BRACKET ):		//fallthrough
	case( CT_CORNER_STAGE_BOARD ):
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift( i );
			bReturn = pLift->DeleteAllComponentsOfType( eType );
		}
		break;
	case( CT_CHAIN_LINK ):			//fallthrough
	case( CT_SHADE_CLOTH ):			//fallthrough
	case( CT_BRACING ):				//fallthrough
	case( CT_STANDARD ):			//fallthrough
	case( CT_STANDARD_OPENEND ):	//fallthrough
	case( CT_JACK ):				//fallthrough
	case( CT_TIE_TUBE ):			//fallthrough
	case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
	case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
	case( CT_TIE_CLAMP_YOKE ):		//fallthrough
	case( CT_TIE_CLAMP_90DEGREE ):	//fallthrough
	case( CT_SOLEBOARD ):

		bReturn = false;
		for( i=0; i<GetNumberOfBayComponents(); i++ )
		{
			pComponent = GetBayComponent( i );
			if( (pComponent->GetType()==eType) )
			{
				m_caComponents.DeleteComponent(i);
				i--;
				bReturn = true;
			}
		}
		break;
	default:
		assert( false );
		break;
	}
	return bReturn;
}

void Bay::EnsurePlanksAreSupported(SideOfBayEnum eSide)
{
	int				i, iPlanks;
	Bay				*pBayNeighbor;
	bool			bPreferAddToNeighbor;
	Lift			*pLift;
	SideOfBayEnum	eSideNeighbor, eSideStageN, eSideStageS, eSideStageNNeighbor, eSideStageSNeighbor;

	switch( eSide )
	{
	case( EAST ):
		pBayNeighbor = GetForward();
		eSideNeighbor = WEST;
		eSideStageN = NNE;
		eSideStageS = SSE;
		eSideStageNNeighbor = NNW;
		eSideStageSNeighbor = SSW;
		bPreferAddToNeighbor = false;
		break;
	case( WEST ):
		pBayNeighbor = GetBackward();
		eSideNeighbor = EAST;
		eSideStageN = NNW;
		eSideStageS = SSW;
		eSideStageNNeighbor = NNE;
		eSideStageSNeighbor = SSE;
		bPreferAddToNeighbor = true;
		break;
	default:
		assert( false );
		return;
	}

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		assert( pLift!=NULL );
		EnsureComponentExists( pBayNeighbor, pLift, CT_TRANSOM, eSide, eSideNeighbor, GetBayWidth(), LIFT_RISE_0000MM, bPreferAddToNeighbor );

		iPlanks = pLift->GetNumberOfPlanksOnStageBoard( NORTH );
		if( iPlanks>0 )
		{
			EnsureComponentExists( pBayNeighbor, pLift, CT_HOPUP_BRACKET, eSideStageN, eSideStageNNeighbor, (int)iPlanks, LIFT_RISE_0000MM, bPreferAddToNeighbor );
		}

		iPlanks = pLift->GetNumberOfPlanksOnStageBoard( SOUTH );
		if( iPlanks>0 )
		{
			EnsureComponentExists( pBayNeighbor, pLift, CT_HOPUP_BRACKET, eSideStageS, eSideStageSNeighbor, (int)iPlanks, LIFT_RISE_0000MM, bPreferAddToNeighbor );
		}
	}
}

void Bay::AutoAdjustStandard(CornerOfBayEnum eCnr)
{
	Bay				*pOwner;
	double			dTopRL, dBtmRL;
	CornerOfBayEnum eOwnerCnr;

	dTopRL = LARGE_NEGATIVE_NUMBER;
	dBtmRL = LARGE_NUMBER;
	eOwnerCnr = eCnr;
	pOwner = this;

	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):

		dBtmRL = min( dBtmRL, GetStandardPosition( CNR_NORTH_EAST ).z );
		dTopRL = max( dTopRL, GetRLOfTopLiftPlusHandrail( NE ) );

		if( GetForward()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetForward()->GetStandardPosition( CNR_NORTH_WEST ).z );
			dTopRL = max( dTopRL, GetForward()->GetRLOfTopLiftPlusHandrail( NW ) );

			if( GetForward()->GetOuter()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetForward()->GetOuter()->GetStandardPosition( CNR_SOUTH_WEST ).z );
				dTopRL = max( dTopRL, GetForward()->GetOuter()->GetRLOfTopLiftPlusHandrail( SW ) );
			}
		}
		if( GetOuter()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetOuter()->GetStandardPosition( CNR_SOUTH_EAST ).z );
			dTopRL = max( dTopRL, GetOuter()->GetRLOfTopLiftPlusHandrail( SE ) );
			if( GetOuter()->GetForward()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetOuter()->GetForward()->GetStandardPosition( CNR_SOUTH_WEST ).z );
				dTopRL = max( dTopRL, GetOuter()->GetForward()->GetRLOfTopLiftPlusHandrail( SW ) );
			}
		}
		break;
	case( CNR_SOUTH_EAST ):
		dBtmRL = min( dBtmRL, GetStandardPosition( CNR_SOUTH_EAST ).z );
		dTopRL = max( dTopRL, GetRLOfTopLiftPlusHandrail( SE ) );

		if( GetForward()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetForward()->GetStandardPosition( CNR_SOUTH_WEST ).z );
			dTopRL = max( dTopRL, GetForward()->GetRLOfTopLiftPlusHandrail( SW ) );
			if( GetForward()->GetInner()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetForward()->GetInner()->GetStandardPosition( CNR_NORTH_WEST ).z );
				dTopRL = max( dTopRL, GetForward()->GetInner()->GetRLOfTopLiftPlusHandrail( NW ) );
			}
		}
		if( GetInner()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetInner()->GetStandardPosition( CNR_NORTH_EAST ).z );
			dTopRL = max( dTopRL, GetInner()->GetRLOfTopLiftPlusHandrail( NE ) );
			eOwnerCnr = CNR_NORTH_EAST;
			pOwner = GetInner();
			if( GetInner()->GetForward()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetInner()->GetForward()->GetStandardPosition( CNR_NORTH_WEST ).z );
				dTopRL = max( dTopRL, GetInner()->GetForward()->GetRLOfTopLiftPlusHandrail( NW ) );
			}

		}
		break;
	case( CNR_NORTH_WEST ):
		dBtmRL = min( dBtmRL, GetStandardPosition( CNR_NORTH_WEST ).z );
		dTopRL = max( dTopRL, GetRLOfTopLiftPlusHandrail( NW ) );

		if( GetBackward()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetBackward()->GetStandardPosition( CNR_NORTH_EAST ).z );
			dTopRL = max( dTopRL, GetBackward()->GetRLOfTopLiftPlusHandrail( NE ) );
			eOwnerCnr = CNR_NORTH_EAST;
			pOwner = GetBackward();
			if( GetBackward()->GetOuter()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetBackward()->GetOuter()->GetStandardPosition( CNR_SOUTH_EAST ).z );
				dTopRL = max( dTopRL, GetBackward()->GetOuter()->GetRLOfTopLiftPlusHandrail( SE ) );
			}
		}
		if( GetOuter()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetOuter()->GetStandardPosition( CNR_SOUTH_WEST ).z );
			dTopRL = max( dTopRL, GetOuter()->GetRLOfTopLiftPlusHandrail( SW ) );
			if( GetOuter()->GetBackward()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetOuter()->GetBackward()->GetStandardPosition( CNR_SOUTH_EAST ).z );
				dTopRL = max( dTopRL, GetOuter()->GetBackward()->GetRLOfTopLiftPlusHandrail( SE ) );
			}
		}
		break;
	case( CNR_SOUTH_WEST ):
		dBtmRL = min( dBtmRL, GetStandardPosition( CNR_SOUTH_WEST ).z );
		dTopRL = max( dTopRL, GetRLOfTopLiftPlusHandrail( SW ) );

		if( GetBackward()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetBackward()->GetStandardPosition( CNR_SOUTH_EAST ).z );
			dTopRL = max( dTopRL, GetBackward()->GetRLOfTopLiftPlusHandrail( SE ) );
			eOwnerCnr = CNR_SOUTH_EAST;
			pOwner = GetBackward();
			if( GetBackward()->GetInner()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetBackward()->GetInner()->GetStandardPosition( CNR_NORTH_EAST ).z );
				dTopRL = max( dTopRL, GetBackward()->GetInner()->GetRLOfTopLiftPlusHandrail( NE ) );
				eOwnerCnr = CNR_NORTH_EAST;
				pOwner = GetBackward()->GetInner();
			}
		}
		if( GetInner()!=NULL )
		{
			dBtmRL = min( dBtmRL, GetInner()->GetStandardPosition( CNR_NORTH_WEST ).z );
			dTopRL = max( dTopRL, GetInner()->GetRLOfTopLiftPlusHandrail( NW ) );
			eOwnerCnr = CNR_NORTH_WEST;
			pOwner = GetInner();
			if( GetInner()->GetBackward()!=NULL )
			{
				dBtmRL = min( dBtmRL, GetInner()->GetBackward()->GetStandardPosition( CNR_NORTH_EAST ).z );
				dTopRL = max( dTopRL, GetInner()->GetBackward()->GetRLOfTopLiftPlusHandrail( NE ) );
				eOwnerCnr = CNR_NORTH_EAST;
				pOwner = GetInner()->GetBackward();
			}
		}
		break;
	}
	
	if( pOwner!=NULL )
		pOwner->SetStandardToRLs( eOwnerCnr, dTopRL, dBtmRL );
	else
		assert( false );
}

double Bay::GetSmallestRequiredWallTieLength( SideOfBayEnum eSide )
{
	double dWallTieDistance, dTieLength;

	switch( GetRunPointer()->GetTieType() )
	{
	case( TIE_TYPE_COLUMN ):
		dWallTieDistance = DEFAULT_TIE_INTO_WALL_COLUMN;
		break;
	case( TIE_TYPE_MASONARY ):
		dWallTieDistance = DEFAULT_TIE_INTO_WALL_MASONARY;
		break;
	case( TIE_TYPE_YOKE ):
		dWallTieDistance = DEFAULT_TIE_INTO_WALL_YOKE;
		break;
	case( TIE_TYPE_BUTTRESS_12 ):	//fallthrough
	case( TIE_TYPE_BUTTRESS_18 ):	//fallthrough
	case( TIE_TYPE_BUTTRESS_24 ):	//fallthrough
	default:
		assert( false );
	}

	switch( eSide )
	{
	case( NNW ):
	case( NNE ):
		dTieLength = GetBayWidthActual()+GetTemplate()->GetWidthOfNStage();
		break;
	case( SSE ):
	case( SSW ):
		dTieLength = GetBayWidthActual()+GetTemplate()->GetWidthOfSStage();
		break;
	case( WSW ):
	case( WNW ):
		dTieLength = GetBayLengthActual()+GetTemplate()->GetWidthOfWStage();
		break;
	case( ENE ):
	case( ESE ):
		dTieLength = GetBayLengthActual()+GetTemplate()->GetWidthOfEStage();
		break;
	default:
		assert( false );
	}

	dTieLength+= GetRunPointer()->GetDistanceFromWall() + dWallTieDistance;
	dTieLength = GetController()->GetAvailableLength( CT_TIE_TUBE, MT_STEEL, dTieLength, RT_ROUND_UP );

	assert( dTieLength!=3500.00 );

	return dTieLength;
}

double Bay::GetTopRLOfStandard(CornerOfBayEnum eCnr)
{
	double dRL;
	dRL = GetStandardPosition( eCnr ).z;
	dRL+= GetHeightOfStandards( eCnr );
	return dRL;
}

double Bay::GetBottomRLofStandard(CornerOfBayEnum eCnr) const
{
	double dRL;
	dRL = GetStandardPosition( eCnr ).z;
	return dRL;
}

bool BayList::IsInList(Bay *pBay)
{
	return GetBayPos(pBay)>=0;
}

int BayList::GetBayPos(Bay *pBay)
{
	int i;
	for( i=0; i<GetSize(); i++ )
	{
		if( pBay==GetAt(i) )
			return i;
	}
	return -1;
}

void Bay::AdjustStandardSmart(CornerOfBayEnum eCnr, doubleArray &daArrangement, bool bSoleBoard, BayList *pBays/*=NULL*/)
{
	Bay				*pBayOwner, *pBayEffect1, *pBayEffect2, *pBayEffect3, *pBayEffect4;
	bool			bAdjustStds, bSoleboard, bNeighborOwns;
	SideOfBayEnum	eSideOwner, eSideEffect1, eSideEffect2, eSideEffect3, eSideEffect4;

	eSideOwner		= SIDE_INVALID;
	pBayOwner		= NULL;
	bNeighborOwns	= false;
	pBayEffect1		= NULL;
	pBayEffect2		= NULL;
	pBayEffect3		= NULL;
	pBayEffect4		= NULL;
	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):
		pBayOwner		= NULL;
		pBayEffect1		= GetForward();
		pBayEffect2		= GetOuter();
		if( GetOuter()!=NULL )
		pBayEffect3		= GetOuter()->GetForward();
		if( GetForward()!=NULL )
		pBayEffect4		= GetForward()->GetOuter();

		eSideOwner		= SIDE_INVALID;
		eSideEffect1	= NE;
		eSideEffect2	= SE;
		eSideEffect3	= SW;
		eSideEffect4	= SW;
		break;
	case( CNR_SOUTH_EAST ):
		pBayOwner		= GetInner();
		pBayEffect1		= GetInner();
		pBayEffect2		= GetForward();
		if( GetInner()!=NULL )
		pBayEffect3		= GetInner()->GetForward();
		if( GetForward()!=NULL )
		pBayEffect4		= GetForward()->GetInner();

		eSideOwner		= NE;
		eSideEffect1	= NE;
		eSideEffect2	= SW;
		eSideEffect3	= NW;
		eSideEffect4	= NW;
		bNeighborOwns	= true;
		break;
	case( CNR_SOUTH_WEST ):

		pBayEffect1		= GetInner();
		pBayEffect2		= GetBackward();
		if( GetInner()!=NULL )
		pBayEffect3		= GetInner()->GetBackward();
		if( GetBackward()!=NULL )
		pBayEffect4		= GetBackward()->GetInner();

		eSideEffect1	= NW;
		eSideEffect2	= SE;
		eSideEffect3	= NE;
		eSideEffect4	= NE;
		if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
			{
				pBayOwner	= GetInner()->GetBackward();
				eSideOwner	= NE;
			}
			else
			{
				pBayOwner	= GetInner();
				eSideOwner	= NW;
			}
		}
		else if( GetBackward()!=NULL )
		{
			if( GetBackward()->GetInner()!=NULL )
			{
				pBayOwner	= GetBackward()->GetInner();
				eSideOwner	= NE;
			}
			else
			{
				pBayOwner	= GetBackward();
				eSideOwner	= SE;
			}
		}
		bNeighborOwns	= true;
		break;
	case( CNR_NORTH_WEST ):
		pBayOwner		= GetBackward();
		pBayEffect1		= GetBackward();
		pBayEffect2		= GetOuter();
		if( GetBackward()!=NULL )
		pBayEffect3		= GetBackward()->GetOuter();
		if( GetOuter()!=NULL )
		pBayEffect4		= GetOuter()->GetBackward();

		eSideOwner		= NE;
		eSideEffect1	= NE;
		eSideEffect2	= SW;
		eSideEffect3	= SE;
		eSideEffect4	= SE;
		bNeighborOwns	= true;
		break;
	default:
		assert( false );
	}
	if( pBayOwner==NULL )
		bNeighborOwns = false;

	double	dTopStdThisBay, dTopEffect1, dTopEffect2, dTopEffect3, dTopEffect4;

	dTopEffect1	= LARGE_NEGATIVE_NUMBER;
	dTopEffect2	= LARGE_NEGATIVE_NUMBER;
	dTopEffect3	= LARGE_NEGATIVE_NUMBER;
	dTopEffect4	= LARGE_NEGATIVE_NUMBER;

	//Why was this line inserted, the soleboard value should be passed into this function
	//bSoleboard = HasBayComponentOfTypeOnSide( CT_SOLEBOARD, CornerAsSideOfBay( eCnr ) );
	dTopStdThisBay	= GetBottomRLofStandard( eCnr ) + daArrangement.GetTotal();
	if( pBayEffect1!=NULL && ( pBays==NULL || !pBays->IsInList( pBayEffect1 ) ) )
	{
		dTopEffect1	= pBayEffect1->GetRLOfTopLiftPlusHandrail( eSideEffect1 )+RLAdjust();
		bSoleboard	= bSoleboard|pBayEffect1->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSideEffect1 );
	}
	if( pBayEffect2!=NULL && ( pBays==NULL || !pBays->IsInList( pBayEffect2 ) ) )
	{
		dTopEffect2	= pBayEffect2->GetRLOfTopLiftPlusHandrail( eSideEffect2 )+RLAdjust();
		bSoleboard	= bSoleboard|pBayEffect2->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSideEffect2 );
	}
	if( pBayEffect3!=NULL && ( pBays==NULL || !pBays->IsInList( pBayEffect3 ) ) )
	{
		dTopEffect3	= pBayEffect3->GetRLOfTopLiftPlusHandrail( eSideEffect3 )+RLAdjust();
		bSoleboard	= bSoleboard|pBayEffect3->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSideEffect3 );
	}
	if( pBayEffect4!=NULL && ( pBays==NULL || !pBays->IsInList( pBayEffect4 ) ) )
	{
		dTopEffect4	= pBayEffect4->GetRLOfTopLiftPlusHandrail( eSideEffect4 )+RLAdjust();
		bSoleboard	= bSoleboard|pBayEffect4->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, eSideEffect4 );
	}

	double	dHeight;
	bAdjustStds = true;
	int iBay;

	dHeight = dTopStdThisBay;
	if( dHeight<dTopEffect1 )
	{
		iBay = pBayEffect1->GetBayNumber();
		dHeight = dTopEffect1;
	}
	if( dHeight<dTopEffect2 )
	{
		iBay = pBayEffect2->GetBayNumber();
		dHeight = dTopEffect2;
	}
	if( dHeight<dTopEffect3 )
	{
		iBay = pBayEffect3->GetBayNumber();
		dHeight = dTopEffect3;
	}
	if( dHeight<dTopEffect4 )
	{
		iBay = pBayEffect4->GetBayNumber();
		dHeight = dTopEffect4;
	}
	
#ifdef WARN_IF_LOWER_STANDARDS
	if( dHeight>dTopStdThisBay )
	{
		CString	sMsg, sTemp;
		sMsg.Format( _T("You are trying to set the %s standards for Bay%i\n"), GetSideOfBayDescStr( CornerAsSideOfBay( eCnr ) ), GetBayNumber() );
		sTemp.Format( _T("to %0.1fm, however Bay%i requires them to be %0.1fm high.\n\n"), dTopStdThisBay, 
					iBay, dHeight );
		sMsg+= sTemp;
		sMsg+= _T("Do you really want to use these shorter standards?");
		MessageBeep( MB_ICONWARNING );
		if( MessageBox( NULL, sMsg, _T("Selection Error"), MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2 )==IDNO )
			bAdjustStds=false;
	}
#else	//#ifdef WARN_IF_LOWER_STANDARDS
	bAdjustStds=false;
#endif	//#ifdef WARN_IF_LOWER_STANDARDS

	if( bAdjustStds )
	{
		if( bNeighborOwns )
			pBayOwner->SetStandardsArrangement( daArrangement, SideOfBayAsCorner(eSideOwner), bSoleBoard );
		else
			SetStandardsArrangement( daArrangement, eCnr, bSoleBoard );
	}
	else
	{
		if( bNeighborOwns )
			pBayOwner->CreateStandardsArrangment( dHeight-RLAdjust()-GetStandardPosition(SideOfBayAsCorner(eSideOwner)).z,
							SideOfBayAsCorner(eSideOwner), bSoleBoard );
		else
			CreateStandardsArrangment( dHeight-RLAdjust()-GetStandardPosition(eCnr).z,
							eCnr, bSoleBoard );
	}
}


Component * Bay::GetComponentAtRL(ComponentTypeEnum eType, double dRL, SideOfBayEnum eSide)
{
	int			k;
	Lift		*pLift;
	double		dLiftRL, dCompRLBot, dCompRLTop;
	Component	*pComponent;
	LiftRiseEnum	eRise;

	switch( eType )
	{
	///////////////////////////////////////////////////////
	//Components that need more info
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
	case( CT_STAGE_BOARD ):
		//find the lift that covers this RL
		pLift = GetLiftAtRL( dRL );
		if( pLift!=NULL )
		{
			//find the RL of the lift
			dLiftRL = pLift->GetRL();
			return pLift->GetComponent( eType, 0, SOUTH );
		}
		break;		

	case( CT_LADDER_PUTLOG ):
	case( CT_STAIR_RAIL ):
	case( CT_STAIR_RAIL_STOPEND ):
	case( CT_TOE_BOARD_CLIP ):
	case( CT_PUTLOG_CLIP ):
		assert( false );
		//fallthrough
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_MID_RAIL ):
	case( CT_STANDARD_CONNECTOR ):
	case( CT_TOE_BOARD ):
	case( CT_STAIR ):
	case( CT_LADDER ):
	case( CT_CORNER_STAGE_BOARD ):
	case( CT_HOPUP_BRACKET ):
		//find the lift that covers this RL
		pLift = GetLiftAtRL( dRL );
		if( pLift!=NULL )
		{
			//find the RL of the lift
			dLiftRL = pLift->GetRL();

			//find the rise within this lift
			eRise = GetRiseEnumFromRise( dRL-dLiftRL );

			return pLift->GetComponent( eType, eRise, eSide );
		}
		break;		

	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_BRACING ):
	case( CT_TIE_BAR ):
	case( CT_TIE_TUBE ):
	case( CT_TIE_CLAMP_COLUMN ):
	case( CT_TIE_CLAMP_MASONARY ):
	case( CT_TIE_CLAMP_YOKE ):
	case( CT_TIE_CLAMP_90DEGREE ):
		assert( false );
		break;		

	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
		for( k=0; k<GetNumberOfBayComponents(); k++ )
		{
			pComponent = m_caComponents.GetComponent( k );
			assert( pComponent!=NULL );

			if( eSide == m_caComponents.GetPosition( k ) )
			{
				if( eType == pComponent->GetType() )
				{
					dCompRLBot = pComponent->GetRLActual();
					dCompRLTop = dCompRLBot+pComponent->GetLengthActual();
					if( dRL>dCompRLBot-ROUND_ERROR &&
						dRL<dCompRLTop+ROUND_ERROR )
					{
						return pComponent;
					}
				}
			}
		}
		break;		

	case( CT_SOLEBOARD ):
		for( k=0; k<GetNumberOfBayComponents(); k++ )
		{
			pComponent = m_caComponents.GetComponent( k );
			assert( pComponent!=NULL );

			if( eSide == m_caComponents.GetPosition( k ) )
			{
				if( eType == pComponent->GetType() )
				{
					dCompRLBot = pComponent->GetRLActual();
					dCompRLTop = dCompRLBot+pComponent->GetHeightActual();
					if( dRL>dCompRLBot-ROUND_ERROR &&
						dRL<dCompRLTop+ROUND_ERROR )
					{
						return pComponent;
					}
				}
			}
		}
		break;		

	case( CT_TEXT ):
	default:
		assert( false );

	}

	return NULL;
}

bool Bay::HasComponentAtRL(ComponentTypeEnum eType, double dRL, SideOfBayEnum eSide)
{
	return (GetComponentAtRL( eType, dRL, eSide )!=NULL);
}

void Bay::EnsureHandrailsAreCorrect(SideOfBayEnum eSide, bool bCheckNeighborSytle/*=false*/)
{
	int					i;
	Bay					*pNeighbor;
	Lift				*pLift, *pNeighborLift, *pNeighborLiftAbove, *pLapLift;
	double				dRLLift, dRLNeighbor;
	Component			*pComponent;
	LapboardBay			*pLap;
	SideOfBayEnum		eSideHopupBracket1, eSideHopupBracket2,
						eSideHopupBracketEL1, eSideHopupBracketEL2,
						eSideHopup1, eSideHopup2,
						eSideNeighborHB1, eSideNeighborHB2,
						eSideNeighbor;
	HandrailStyleEnum	eHRS, eHRSSide;

	switch( eSide )
	{
	case( NORTH ):
		eSideHopupBracket1 = WNW;
		eSideHopupBracket2 = ENE;
		eSideHopupBracketEL1 = NNW;
		eSideHopupBracketEL2 = NNE;
		eSideHopup1 = W;
		eSideHopup2 = E;
		eSideNeighbor = SOUTH;
		eSideNeighborHB1 = WSW;
		eSideNeighborHB2 = ESE;
		pNeighbor = GetOuter();
		break;
	case( SOUTH ):
		eSideHopupBracket1 = WSW;
		eSideHopupBracket2 = ESE;
		eSideHopupBracketEL1 = SSW;
		eSideHopupBracketEL2 = SSE;
		eSideHopup1 = W;
		eSideHopup2 = E;
		eSideNeighbor = NORTH;
		eSideNeighborHB1 = WNW;
		eSideNeighborHB2 = ENE;
		pNeighbor = GetInner();
		break;
	case( EAST ):
		eSideHopupBracket1 = NNE;
		eSideHopupBracket2 = SSE;
		eSideHopupBracketEL1 = ESE;
		eSideHopupBracketEL2 = ENE;
		eSideHopup1 = N;
		eSideHopup2 = S;
		eSideNeighbor = WEST;
		eSideNeighborHB1 = NNW;
		eSideNeighborHB2 = SSW;
		pNeighbor = GetForward();
		break;
	case( WEST ):
		eSideHopupBracket1 = NNW;
		eSideHopupBracket2 = SSW;
		eSideHopupBracketEL1 = WSW;
		eSideHopupBracketEL2 = WNW;
		eSideHopup1 = N;
		eSideHopup2 = S;
		eSideNeighbor = EAST;
		eSideNeighborHB1 = NNE;
		eSideNeighborHB2 = SSE;
		pNeighbor = GetBackward();
		break;
	default:
		assert( false );
	}

	//////////////////////////////////////////////////////////////////////////
	//What style of handrail are we using?
	eHRS	= GetHandrailStyle(ALL_SIDES, bCheckNeighborSytle );

	//////////////////////////////////////////////////////////////////////////
	//Apply handrails to each lift
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		assert( pLift!=NULL );

		//What is the existing HRS for this lift?
		eHRSSide = pLift->GetHandrailStyle(eSide);

		dRLLift = pLift->GetRL();

		pLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
		pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
		pLift->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );
		if( eSide==NORTH || eSide==SOUTH )
		{
			pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
		}

		//Firstly try deleting any unrequired 1000mm hopups, without knowing the
		//	nieghbor, once we calculate the neighbor we will try again!
		pLift->DeleteUnneeded1000mmHopup( NULL, eSideHopupBracket1, eSideHopup1 );
		pLift->DeleteUnneeded1000mmHopup( NULL, eSideHopupBracket2, eSideHopup2 );

		if( pNeighbor!=NULL )
		{
			if( pNeighbor->GetBayType()==BAY_TYPE_BAY ||
				pNeighbor->GetBayType()==BAY_TYPE_BUTTRESS )
			{
				//Does this neighboring bay have a lift of equal height?
				pNeighborLift = pNeighbor->GetLiftAtRL( dRLLift );
				if( pNeighborLift!=NULL )
				{
					//there is a neighboring lift
					//is the decking at the same height?
					dRLNeighbor = pNeighborLift->GetRL();
					if( ( dRLLift>dRLNeighbor-ROUND_ERROR &&
						  dRLLift<dRLNeighbor+ROUND_ERROR ) ||
						( dRLLift>dRLNeighbor+GetStarSeparation()-ROUND_ERROR &&
						  dRLLift<dRLNeighbor+GetStarSeparation()+ROUND_ERROR ) )
					{
						//These two lifts are at the same height

						//now that we know the lift, try deleting unneeded hopups again
						pLift->DeleteUnneeded1000mmHopup( pNeighborLift, eSideHopupBracket1, eSideHopup1 );
						pLift->DeleteUnneeded1000mmHopup( pNeighborLift, eSideHopupBracket2, eSideHopup2 );

						if( pLift->IsDeckingLift() )
						{
							if( pNeighborLift->IsEmptyLift() )
							{
								switch( eHRS )
								{
								case(HRS_FULL_MESH_TO_GROUND):
								case(HRS_FULL_MESH):
									pLift->AddFullMeshToSide(eSide);
									break;
								case(HRS_HALF_MESH):
									pLift->AddMeshToSide(eSide);
									break;
								case(HRS_RAIL_ONLY):
									pLift->AddHandrailToSide(eSide);
									break;
								default:
									assert(false);
								case(HRS_NONE):
									//fine, nothing to do
									assert(false);
									break;
								}
								//////////////////////////////////////////////////////////////////////
								//We may need to add hopup brackets at 1.0m
								pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
								pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
							}
							else
							{
								pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
								pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
							}
						}
						else
						{
							if( pNeighborLift->IsDeckingLift() )
							{
								if( eSide==NORTH || eSide==EAST )
								{
									switch( eHRS )
									{
									case(HRS_FULL_MESH_TO_GROUND):
									case(HRS_FULL_MESH):
										pLift->AddFullMeshToSide(eSide);
										break;
									case(HRS_HALF_MESH):
										pLift->AddMeshToSide(eSide);
										break;
									case(HRS_RAIL_ONLY):
										pLift->AddHandrailToSide(eSide);
										break;
									default:
										assert(false);
									case(HRS_NONE):
										//fine, nothing to do
										assert(false);
										break;
									}
									//////////////////////////////////////////////////////////////////////
									//We may need to add hopup brackets at 1.0m
									pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
									pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
								}
							}
						}
					}
					else
					{
						pNeighborLiftAbove = pNeighbor->GetLiftAtRL( dRLLift+GetStarSeparation() );
						if( pNeighborLiftAbove!=NULL && pNeighborLift!=pNeighborLiftAbove )
						{
							//there is another lift just above this one that might be more usefull
							pNeighborLift = pNeighborLiftAbove;
							assert( pNeighborLift!=NULL );

							//It should be at the same height?
							dRLNeighbor = pNeighborLift->GetRL();
							assert( dRLLift+GetStarSeparation()>dRLNeighbor-ROUND_ERROR &&
									dRLLift+GetStarSeparation()<dRLNeighbor+ROUND_ERROR);

							//now that we know the lift, try deleting unneeded hopups again
							pLift->DeleteUnneeded1000mmHopup( pNeighborLift, eSideHopupBracket1, eSideHopup1 );
							pLift->DeleteUnneeded1000mmHopup( pNeighborLift, eSideHopupBracket2, eSideHopup2 );

							//These two lifts are at the same height
							if( pLift->IsDeckingLift() )
							{
								if( pNeighborLift->IsEmptyLift() )
								{
									if( eSide==NORTH || eSide==EAST )
									{
										switch( eHRS )
										{
										case(HRS_FULL_MESH_TO_GROUND):
										case(HRS_FULL_MESH):
											pLift->AddFullMeshToSide(eSide);
											break;
										case(HRS_HALF_MESH):
											pLift->AddMeshToSide(eSide);
											break;
										case(HRS_RAIL_ONLY):
											pLift->AddHandrailToSide(eSide);
											break;
										default:
											assert(false);
										case(HRS_NONE):
											//fine, nothing to do
											assert(false);
											break;
										}
										//We may need to add hopup brackets at 1.0m
										pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
										pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
									}
								}
								else
								{
									pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
									pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
								}
							}
							else
							{
								if( pNeighborLift->IsDeckingLift() )
								{
									if( eSide==NORTH || eSide==EAST )
									{
										if( pNeighborLift->CopyHandrailToSide( N, eSideNeighbor ) ||
											pNeighborLift->CopyHandrailToSide( S, eSideNeighbor ) ||
											pNeighborLift->CopyHandrailToSide( E, eSideNeighbor ) ||
											pNeighborLift->CopyHandrailToSide( W, eSideNeighbor ) )
										{
											//////////////////////////////////////////////////////////////////////
											//We may need to add hopup brackets at 1.0m
											pNeighborLift->Add1000mmHopupIfReqd( eSideNeighborHB1 );
											pNeighborLift->Add1000mmHopupIfReqd( eSideNeighborHB2 );
										}
										else
										{
											pNeighborLift->AddHandrailToSide( eSideNeighbor, true );
										}
									}
								}
							}
						}
						else if( pLift->IsDeckingLift() )
						{
							//These are not at the same height
							switch( eHRS )
							{
							case(HRS_FULL_MESH_TO_GROUND):
							case(HRS_FULL_MESH):
								pLift->AddFullMeshToSide(eSide);
								break;
							case(HRS_HALF_MESH):
								pLift->AddMeshToSide(eSide);
								break;
							case(HRS_RAIL_ONLY):
								pLift->AddHandrailToSide(eSide);
								break;
							default:
								assert(false);
							case(HRS_NONE):
								//fine, nothing to do
								assert(false);
								break;
							}
							//////////////////////////////////////////////////////////////////////
							//We may need to add hopup brackets at 1.0m
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
						}
						else
						{
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
						}
					}
				}
				else
				{
					if( pLift->IsDeckingLift() )
					{
						//This is no neighboring lift
						switch( eHRS )
						{
						case(HRS_FULL_MESH_TO_GROUND):
						case(HRS_FULL_MESH):
							pLift->AddFullMeshToSide(eSide);
							break;
						case(HRS_HALF_MESH):
							pLift->AddMeshToSide(eSide);
							break;
						case(HRS_RAIL_ONLY):
							pLift->AddHandrailToSide(eSide);
							break;
						default:
							assert(false);
						case(HRS_NONE):
							//fine, nothing to do
							assert(false);
							break;
						}
						//////////////////////////////////////////////////////////////////////
						//We may need to add hopup brackets at 1.0m
						pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
						pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
					}
					else
					{
						if( eHRS==HRS_FULL_MESH_TO_GROUND )
							pLift->AddFullMeshToSide(eSide);
						pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
						pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
					}
				}
			}
		}
		else
		{
			//There is no neighboring bay!
			if( pLift->IsDeckingLift() )
			{
				//We have a deck
				if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
				{
					//We have a deck but no hopup!
					//see bug 1477
					if( eSide!=SOUTH || eHRSSide!=HRS_NONE )
					{
						//we have decking, no hopup and we are not talking about the south
						//do we have a lapboard?
						pLap = GetLapboard(eSide);
						pLapLift = NULL;
						if( pLap!=NULL )
						{
							pLapLift = pLap->GetLiftAtRL(pLift->GetRL());
							if( pLapLift!=NULL
								&& ( pLapLift->IsEmptyLift()
									 || pLapLift->GetRL()!=pLift->GetRL() ) )
							{
								//The laplift is either empty or it does not match
								//	the bay's deck.  It is as good as empty!
								pLapLift=NULL;
							}
						}
						if( pLap==NULL || pLapLift==NULL )
						{
							//we have decking, no hopup, no lapboard and we are not talking
							//	about the south!  So we need a handrail?
							//	It is also possible we do have a lapboard on
							//	this side of the bay, but no laps matching this deck!
							switch( eHRS )
							{
							case(HRS_FULL_MESH_TO_GROUND):
							case(HRS_FULL_MESH):
								pLift->AddFullMeshToSide(eSide);
								break;
							case(HRS_HALF_MESH):
								pLift->AddMeshToSide(eSide);
								break;
							case(HRS_RAIL_ONLY):
								pLift->AddHandrailToSide(eSide);
								break;
							default:
								assert(false);
							case(HRS_NONE):
								//fine, nothing to do
								assert(false);
								break;
							}
							//////////////////////////////////////////////////////////////////////
							//We may need to add hopup brackets at 1.0m
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
							pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
						}
					}
				}
			}
			else
			{
				//This is an empty lift
				if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
				{
					//We cannot add full mesh to the south side,
					//	unless there is a hopup
					if( eSide!=SOUTH && eHRS==HRS_FULL_MESH_TO_GROUND )
						pLift->AddFullMeshToSide(eSide);
					pLift->Add1000mmHopupIfReqd( eSideHopupBracket1 );
					pLift->Add1000mmHopupIfReqd( eSideHopupBracket2 );
				}
				else
				{
					//This is an empty lift with a hopup on this(eSide) side,
					//	we should have a handrail here!
					if( !pLift->HasComponentOfTypeOnSide( CT_RAIL, eSide ) )
					{
						switch( eHRSSide )
						{
						case(HRS_FULL_MESH_TO_GROUND):
						case(HRS_FULL_MESH):
							//Terry said he could not think of a reason to use full mesh
							//Mark said they would use half but not full mesh!
							//Mark said "don't loose any sleep over it"
							pLift->AddFullMeshToSide(eSide);
							break;
						case(HRS_HALF_MESH):
							pLift->AddMeshToSide(eSide);
							break;
						case(HRS_RAIL_ONLY):
							pLift->AddHandrailToSide(eSide);
							break;
						default:
							assert(false);
						case(HRS_NONE):
							//fine, nothing to do
							break;
						}
						//////////////////////////////////////////////////////////////////////
						//We may need to add hopup brackets at 1.0m
						pLift->Add1000mmHopupIfReqd( eSideHopupBracketEL1 );
						pLift->Add1000mmHopupIfReqd( eSideHopupBracketEL2 );
					}
				}

				if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSideHopup1 ) )
				{
					pComponent = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideHopupBracket1 );
					if( pComponent!=NULL )
						pLift->DeleteComponent( pComponent->GetID() );
				}
				if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSideHopup2 ) )
				{
					pComponent = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideHopupBracket2 );
					if( pComponent!=NULL )
						pLift->DeleteComponent( pComponent->GetID() );
				}
			}
		}
	}
}

void Bay::EnsureComponentExists(Bay *pBayNeighbor, Lift *pLift, ComponentTypeEnum eType, SideOfBayEnum eSide, SideOfBayEnum eSideNeighbor, double dWidth, LiftRiseEnum eRise, bool bPreferAddToNeighbor )
{
	double	dRL;
	Lift	*pLiftNeighbor;
	if( pLift->GetComponent( eType, eRise, eSide )==NULL )
	{
		if( pBayNeighbor!=NULL )
		{
			dRL = pLift->GetRL();
			if( !pBayNeighbor->HasComponentAtRL( eType, dRL, eSideNeighbor ) )
			{
				//The Neighboring bay doesn't have a component here either!
				pLiftNeighbor = pBayNeighbor->GetLiftAtRL( dRL );
				if( bPreferAddToNeighbor && pLiftNeighbor!=NULL )
				{
					double dRLNeighbor;
					LiftRiseEnum	eRiseNeighbor;
					dRLNeighbor = pLiftNeighbor->GetRL();
					eRiseNeighbor = GetRiseEnumFromRise( dRL - dRLNeighbor );
					if( eRiseNeighbor>=LIFT_RISE_0000MM && eRiseNeighbor<=LIFT_RISE_1500MM )
					{
						//Hopup does not exist for neighbor lift
						pLiftNeighbor->AddComponent( eType, dWidth, eSideNeighbor, eRiseNeighbor, MT_STEEL );
					}
					else
					{
						//Not a valid location within the lift, better add it to myself instead
						pLift->AddComponent( eType, dWidth, eSide, eRise, MT_STEEL );
					}
				}
				else
				{
					//no neighbor lift better add it to myself
					pLift->AddComponent( eType, dWidth, eSide, eRise, MT_STEEL );
				}
			}
		}
		else
		{
			//no neighbor better add it to myself
			pLift->AddComponent( eType, dWidth, eSide, eRise, MT_STEEL );
		}
	}
}

bool Bay::GetUseMidrailWithChainMesh() const
{
	return m_bUseMidrailWithChainMesh;
}

void Bay::SetUseMidrailWithChainMesh(bool bUse)
{
	m_bUseMidrailWithChainMesh = bUse;
}

bool Bay::GetUseLedgerEveryMetre() const
{
	return m_bUseLedgerEveryMetre;
}

void Bay::SetUseLedgerEveryMetre(bool bUse)
{
	m_bUseLedgerEveryMetre = bUse;
}

#define TEST_COMMITTED				\
switch( eCommProp )					\
{									\
case( COMMIT_PARTIAL ):				\
	return COMMIT_PARTIAL;			\
case( COMMIT_NONE ):				\
	if( eProportion==COMMIT_FULL)	\
		return COMMIT_PARTIAL;		\
	break;							\
case( COMMIT_FULL ):				\
	if( eProportion==COMMIT_NONE)	\
		return COMMIT_PARTIAL;		\
	break;							\
}									\
eProportion = eCommProp;

CommittedProportionEnum Bay::GetCommittedProportion()
{
	int						i, iCommitted;
	Lift					*pLift;
	Component				*pComp;
	CommittedProportionEnum	eCommProp, eProportion;

	eProportion=COMMIT_INVALID;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		eCommProp = pLift->GetCommittedProportion();
		TEST_COMMITTED
	}

	iCommitted=0;
	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComp = GetBayComponent(i);
		assert( pComp!=NULL );

		if( pComp->IsCommitted() )
			iCommitted++;

		if( iCommitted>0 )
		{
			if( iCommitted<i+1 )
			{
				//So components are committed, but not all the components!
				return COMMIT_PARTIAL;
			}

			if( eProportion==COMMIT_NONE )
			{
				//No lift is committed yet some components are committed!
				return COMMIT_PARTIAL;
			}
		}

		if( eProportion==COMMIT_FULL && iCommitted==0 && i>0 )
		{
			//All the lift components are committed, however one (or maybe more) of the
			//	bay components are not committed!
			return COMMIT_PARTIAL;
		}
	}

	//This bay also requires standards that may not belong to it
	if( GetBackward()!=NULL )
	{
		if( GetBackward()->GetInner()!=NULL )
		{
			eCommProp = GetBackward()->GetInner()->GetCommitProportionForComponentOnSide( CT_STANDARD, NORTH_EAST );
			TEST_COMMITTED
		}

		eCommProp = GetBackward()->GetCommitProportionForComponentOnSide( CT_STANDARD, SOUTH_EAST );
		TEST_COMMITTED

		eCommProp = GetBackward()->GetCommitProportionForComponentOnSide( CT_STANDARD, NORTH_EAST );
		TEST_COMMITTED
	}

	if( GetInner()!=NULL )
	{
		if( GetInner()->GetBackward()!=NULL )
		{
			eCommProp = GetInner()->GetBackward()->GetCommitProportionForComponentOnSide( CT_STANDARD, NORTH_EAST );
			TEST_COMMITTED
		}

		eCommProp = GetInner()->GetCommitProportionForComponentOnSide( CT_STANDARD, NORTH_EAST );
		TEST_COMMITTED

		eCommProp = GetInner()->GetCommitProportionForComponentOnSide( CT_STANDARD, NORTH_WEST );
		TEST_COMMITTED
	}

	if( iCommitted>0 && iCommitted==GetNumberOfBayComponents() )
	{
		assert( eProportion==COMMIT_FULL );
		return COMMIT_FULL;
	}

	if( i>0 )
	{
		assert( eProportion==COMMIT_NONE );
	}
	assert( iCommitted==0 );
	return COMMIT_NONE;
}

CommittedProportionEnum Bay::GetCommitProportionForComponentOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide)
{
	int						i, iCommitted, iUnCommitted;
	Lift					*pLift;
	Component				*pComp;
	CommittedProportionEnum	eCommProp, eProportion;

	eProportion		= COMMIT_NONE;
	iCommitted		= 0;
	iUnCommitted	= 0;

	switch( eType )
	{
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
	case( CT_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_MID_RAIL ):
	case( CT_TOE_BOARD ):
	case( CT_STAIR ):
	case( CT_LADDER ):
	case( CT_HOPUP_BRACKET ):
	case( CT_CORNER_STAGE_BOARD ):
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift(i);
			eCommProp = pLift->GetCommitProportionForComponentOnSide( eType, eSide );
			if( eCommProp==COMMIT_PARTIAL )
			{
				return COMMIT_PARTIAL;
			}

			if( eCommProp==COMMIT_NONE && eProportion==COMMIT_FULL)
			{
				//Some of this has already been marked as committed,
				//	yet we found some others that aren't
				return COMMIT_PARTIAL;
			}

			eProportion = eCommProp;
		}

		if( eProportion==COMMIT_FULL )
			return COMMIT_FULL;

		break;

	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_TIE_TUBE ):
	case( CT_TIE_CLAMP_COLUMN ):
	case( CT_TIE_CLAMP_MASONARY ):
	case( CT_TIE_CLAMP_YOKE ):
	case( CT_TIE_CLAMP_90DEGREE ):
	case( CT_BRACING ):
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
		for( i=0; i<GetNumberOfBayComponents(); i++ )
		{
			pComp = GetBayComponent(i);

			if( pComp->GetType()==eType &&
				pComp->GetSideOfBay()==eSide )
			{
				assert( pComp!=NULL );

				if( pComp->IsCommitted() )
					iCommitted++;
				else
					iUnCommitted++;

				if( iCommitted>0 && iUnCommitted>0 )
				{
					return COMMIT_PARTIAL;
				}
			}
		}

		if( iCommitted>0 )
		{
			assert( iUnCommitted<=0 );
			return COMMIT_FULL;
		}
		break;

	case( CT_TEXT ):
	case( CT_TIE_BAR ):
	case( CT_STANDARD_CONNECTOR ):
	case( CT_LADDER_PUTLOG ):
	case( CT_STAIR_RAIL ):
	case( CT_STAIR_RAIL_STOPEND ):
	case( CT_TOE_BOARD_CLIP ):
	case( CT_PUTLOG_CLIP ):
	case( CT_UNDEFINED ):
		assert( false );
	}

	return COMMIT_NONE;
}

Bay * Bay::GetBayThatOwnsSide(SideOfBayEnum eThisBaySide, SideOfBayEnum &eOtherBaySide) const
{
	Bay *pNeighbor;

	pNeighbor = NULL;
	eOtherBaySide = SIDE_INVALID;

	switch( eThisBaySide )
	{
	case( NORTH ):
		break;
	case( EAST ):
		break;
	case( SOUTH ):
		if( GetInner()!=NULL )
		{
			pNeighbor = GetInner();
			eOtherBaySide = N;
		}
		break;
	case( WEST ):
		if( GetBackward()!=NULL )
		{
			pNeighbor = GetBackward();
			eOtherBaySide = E;
		}
		break;
	case( NORTH_EAST ):
		break;
	case( SOUTH_EAST ):
		if( GetInner()!=NULL )
		{
			pNeighbor = GetInner();
			eOtherBaySide = NE;
		}
		break;
	case( SOUTH_WEST ):
		if( GetInner()!=NULL )
		{
			if( GetInner()->GetBackward()!=NULL )
			{
				pNeighbor = GetInner()->GetBackward();
				eOtherBaySide = NE;
			}
			else
			{
				pNeighbor = GetInner();
				eOtherBaySide = NW;
			}
		}
		if( GetBackward()!=NULL )
		{
			if( GetBackward()->GetInner()!=NULL )
			{
				pNeighbor = GetBackward()->GetInner();
				eOtherBaySide = NE;
			}
			else
			{
				pNeighbor = GetBackward();
				eOtherBaySide = SE;
			}
		}
		break;
	case( NORTH_WEST ):
		if( GetBackward()!=NULL )
		{
			pNeighbor = GetBackward();
			eOtherBaySide = NE;
		}
		break;
	default:
		assert( false );
	}

	return pNeighbor;
}

int BayList::Add(Bay *pBay)
{
	int i;
	if( m_bUniqueBaysOnly )
	{
		for( i=0; i<GetSize(); i++ )
		{
			if( pBay==GetAt(i) )
			{
				//we already have this bay in the list!
				return -1;
			}
		}
	}

	if( GetMustBeSameSystem() )
	{
		if( GetSize()<=0 || GetSystem()==pBay->GetSystem() )
		{
			//either there are no element in the array yet
			//	or it is the same system, either way add
			//	it to the list
			return CArray<Bay*,Bay*>::Add( pBay );
		}

		//Don't add it to the array, not the same system!
		return -1;
	}

	return CArray<Bay*,Bay*>::Add( pBay );
}

void BayList::SetUniqueBaysOnly(bool bUnique)
{
	m_bUniqueBaysOnly = bUnique;
}

BayList &BayList::operator=(const BayList &Original)
{
	RemoveAll();
	SetUniqueBaysOnly(Original.m_bUniqueBaysOnly);
	SetMustBeSameSystem(Original.GetMustBeSameSystem());
	for( int i=0; i<Original.GetSize(); i++ )
	{
		Add( Original.GetAt(i) );
	}

	assert( GetSize()==Original.GetSize() );

	return *this;
}

//This is especially usefull for reapplying bracing after a basy size change
void Bay::ReapplyBracingToSide(SideOfBayEnum eSide)
{
	bool bBracing;

	switch( eSide )
	{
	case( NORTH ):
		bBracing = GetTemplate()->GetNBrace();
		break;
	case( EAST ):
		bBracing = GetTemplate()->GetEBrace();
		break;
	case( SOUTH ):
		bBracing = GetTemplate()->GetSBrace();
		break;
	case( WEST ):
		bBracing = GetTemplate()->GetWBrace();
		break;
	case( ALL_SIDES ):
		ReapplyBracingToSide(NORTH);
		ReapplyBracingToSide(EAST);
		ReapplyBracingToSide(SOUTH);
		ReapplyBracingToSide(WEST);
		return;
	case( ALL_VISIBLE_SIDES ):
		if( GetForward()==NULL )
			ReapplyBracingToSide(EAST);
		if( GetInner()==NULL )
			ReapplyBracingToSide(SOUTH);
		if( GetBackward()==NULL )
			ReapplyBracingToSide(WEST);
		if( GetOuter()==NULL )
			ReapplyBracingToSide(NORTH);
		return;
	default:
		assert( false );
	}

	DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eSide );
	if( bBracing )
		CreateBracingArrangment ( eSide, MT_STEEL, false );
}

void Bay::ReapplyWallTiesFromTemplate()
{
	int iSide, iCnr;
	double dHeight, dStandardsHeight, dTieLength;

	TieTypesEnum	eTieType;

	eTieType = GetRunPointer()->GetTieType();
	if( eTieType==TIE_TYPE_BUTTRESS_12 ||
		eTieType==TIE_TYPE_BUTTRESS_18 ||
		eTieType==TIE_TYPE_BUTTRESS_24 )
	{
		//don't add it here
		1;
	}
	else
	{

		for( iSide=(int)NORTH_NORTH_EAST; iSide<=(int)NORTH_NORTH_WEST; iSide++ ) 
		{
			dTieLength = GetSmallestRequiredWallTieLength((SideOfBayEnum)iSide);
			iCnr = CNR_INVALID;
			switch( (SideOfBayEnum)iSide )
			{
			case( NORTH_NORTH_EAST ):
				if( GetTemplate()->GetNNETie() )
					iCnr = CNR_NORTH_EAST;
				break;
			case( EAST_NORTH_EAST ):	
				if( GetTemplate()->GetENETie() )
					iCnr = CNR_NORTH_EAST;	
				break;
			case( EAST_SOUTH_EAST ):	
				if( GetTemplate()->GetESETie() )
					iCnr = CNR_SOUTH_EAST;	
				break;
			case( SOUTH_SOUTH_EAST ):	
				if( GetTemplate()->GetSSETie() )
					iCnr = CNR_SOUTH_EAST;	
				break;
			case( SOUTH_SOUTH_WEST ):	
				if( GetTemplate()->GetSSWTie() )
					iCnr = CNR_SOUTH_WEST;	
				break;
			case( WEST_SOUTH_WEST ):	
				if( GetTemplate()->GetWSWTie() )
					iCnr = CNR_SOUTH_WEST;	
				break;
			case( WEST_NORTH_WEST ):	
				if( GetTemplate()->GetWNWTie() )
					iCnr = CNR_NORTH_WEST;	
				break;
			case( NORTH_NORTH_WEST ):	
				if( GetTemplate()->GetNNWTie() )
					iCnr = CNR_NORTH_WEST;	
				break;
			default:
				assert( false );
			}
			if( iCnr==CNR_INVALID )
				continue;

			DeleteAllComponentsOfTypeFromSide( CT_TIE_TUBE,				(SideOfBayEnum)iSide );
			DeleteAllComponentsOfTypeFromSide( CT_TIE_CLAMP_90DEGREE,	(SideOfBayEnum)iSide );
			DeleteAllComponentsOfTypeFromSide( CT_TIE_CLAMP_COLUMN,		(SideOfBayEnum)iSide );
			DeleteAllComponentsOfTypeFromSide( CT_TIE_CLAMP_MASONARY,	(SideOfBayEnum)iSide );
			DeleteAllComponentsOfTypeFromSide( CT_TIE_CLAMP_YOKE,		(SideOfBayEnum)iSide );

			dHeight = GetRunPointer()->GetTiesVerticallyEvery();
			dStandardsHeight = GetHeightOfStandards( (CornerOfBayEnum)iCnr );

			if( dHeight>=dStandardsHeight )
			{
				//we need at least one tie
				AddComponent( CT_TIE_TUBE, (SideOfBayEnum)iSide, dStandardsHeight, dTieLength, MT_STEEL );
			}
			else
			{
				while( dHeight<dStandardsHeight )
				{
					AddComponent( CT_TIE_TUBE, (SideOfBayEnum)iSide, dHeight, dTieLength, MT_STEEL );
					dHeight+= GetRunPointer()->GetTiesVerticallyEvery();
				}
			}
		}
	}
}

void Bay::EnsureStandardsAreTallEnough(SideOfBayEnum eSide)
{
	double			dHandrailHeight, dStandardHeight;
	CornerOfBayEnum	eCnr;

	eCnr = SideOfBayAsCorner(eSide);
	dHandrailHeight = GetRLOfTopLiftPlusHandrail(eSide)+RLAdjust()-GetStandardPosition(eCnr).z;
	dStandardHeight = GetHeightOfStandards(eCnr);
	if( dHandrailHeight>dStandardHeight+ROUND_ERROR )
	{
		//we need to increase the height of the standard, the new handrail is not supported!
		AutoAdjustStandard(eCnr);
	}
}

bool Bay::HasMovingDeck() const
{
	return m_bContainsMovingDeck;
}

void Bay::SetHasMovingDeck(bool bMovingDeck)
{
	m_bContainsMovingDeck = bMovingDeck;
}


bool Bay::DeleteUnneededComponentsFromEmptyLifts(SideOfBayEnum eSide)
{
	int		iLift;
	bool	bFound;
	Lift	*pLift;

	bFound = false;
	for( iLift=0; iLift<GetNumberOfLifts(); iLift++ )
	{
		pLift = GetLift(iLift);
		assert( pLift!=NULL );
		if( pLift->IsEmptyLift() )
		{
			if( !pLift->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eSide ) )
				pLift->DeleteAllHandrailComponentsFromSide(eSide);
			switch( eSide )
			{
			case( NORTH ):
			case( SOUTH ):
				bFound |= pLift->DeleteAllComponentsOfTypeOnSideExceptRise( CT_LEDGER, eSide, LIFT_RISE_0000MM );
				break;
			case( EAST ):
			case( WEST ):
				bFound |= pLift->DeleteAllComponentsOfTypeOnSideExceptRise( CT_TRANSOM, eSide, LIFT_RISE_0000MM );
				break;
			default:
				assert( false );
				return false;
			}
		}
	}
	return bFound;
}

void Bay::BraceExposedSides()
{
	if( GetForward()==NULL )
		CreateBracingArrangment( EAST, MT_STEEL, false );
	if( GetBackward()==NULL )
		CreateBracingArrangment( WEST, MT_STEEL, false );
	if( GetInner()==NULL )
		CreateBracingArrangment( SOUTH, MT_STEEL, false );
	if( GetOuter()==NULL )
		CreateBracingArrangment( NORTH, MT_STEEL, false );
}

void Bay::SetSydneyCornerBay(bool bIsSydneyCornerBay)
{
	m_bSydneyCornerBay = bIsSydneyCornerBay;
}

bool Bay::IsSydneyCornerBay()
{
	return m_bSydneyCornerBay;
}

void Bay::SetMillsSystemBay( MillsType mtMillsCnrType )
{
	SideOfBayEnum eSide;

	//now set the variable
	if( AddMillsType(mtMillsCnrType) )
	{
		Bay	*pBay;
		pBay = this;
		eSide = SIDE_INVALID;
		if( (mtMillsCnrType&MILLS_TYPE_MISSING_STND_NE)>0 )
		{
			eSide = NE;
		}
		if( (mtMillsCnrType&MILLS_TYPE_MISSING_STND_SE)>0 )
		{
			if( GetInner()!=NULL )
			{
				pBay = GetInner();
				pBay->AddMillsType( MILLS_TYPE_MISSING_STND_NE );
				eSide = NE;
			}
			else
			{
				eSide = SE;
			}
		}
		if( (mtMillsCnrType&MILLS_TYPE_MISSING_STND_SW)>0 )
		{
			if( GetInner()!=NULL )
			{
				if( GetInner()->GetBackward()!=NULL )
				{
					pBay = GetInner()->GetBackward();
					pBay->AddMillsType( MILLS_TYPE_MISSING_STND_NE );
					eSide = NE;
				}
				else
				{
					pBay = GetInner();
					pBay->AddMillsType( MILLS_TYPE_MISSING_STND_NW );
					eSide = NW;
				}
			}
			else if( GetBackward()!=NULL )
			{
				if( GetBackward()->GetInner()!=NULL )
				{
					pBay = GetBackward()->GetInner();
					pBay->AddMillsType( MILLS_TYPE_MISSING_STND_NE );
					eSide = NE;
				}
				else
				{
					pBay = GetBackward();
					pBay->AddMillsType( MILLS_TYPE_MISSING_STND_SE );
					eSide = SE;
				}
			}
			else
			{
				eSide = SE;
			}
			eSide = SW;
		}
		if( (mtMillsCnrType&MILLS_TYPE_MISSING_STND_NW)>0 )
		{
			if( GetBackward()!=NULL )
			{
				pBay = GetBackward();
				pBay->AddMillsType( MILLS_TYPE_MISSING_STND_NE );
				eSide = NE;
			}
			else
			{
				eSide = NW;
			}
		}

		if( eSide!=SIDE_INVALID )
		{
			pBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD, eSide );
			pBay->DeleteAllBayComponentsOfTypeFromSide( CT_STANDARD_OPENEND, eSide );
			pBay->DeleteAllBayComponentsOfTypeFromSide( CT_JACK, eSide );
			pBay->DeleteAllBayComponentsOfTypeFromSide( CT_SOLEBOARD, eSide );
			pBay->DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, eSide );
		}

		if( (mtMillsCnrType&MILLS_TYPE_CONNECT_TEST)>0 )
			CreateComponentsForMillsCnrType( GetSideFromMillsType(mtMillsCnrType) );
	}
}


void Bay::CreateComponentsForMillsCnrType(SideOfBayEnum eSideToConvert)
{
	double		dNewSize;
	Vector3D	Vector, VectorHopupMove;
	Matrix3D	Transform, TransformHopupMove;

	//it must also be mill system
	assert(	GetController()->GetSystem()==S_MILLS );

	int				iLift;
	Lift			*pLift;
	SideOfBayEnum	eExposedSide, eHopupMove, eHopupOther, eHopupRail1, eHopupRail2;

	MillsType	mtType;

	mtType = GetMillsSystemType();

	//If the other side is already exposed, then we need
	//	to remove the entire side!
	switch( eSideToConvert )
	{
	case( NNE ):
		if( mtType&MILLS_TYPE_CONNECT_NNW )
			eSideToConvert = NORTH;
		break;
	case( ENE ):
		if( mtType&MILLS_TYPE_CONNECT_ESE )
			eSideToConvert = EAST;
		break;
	case( ESE ):
		if( mtType&MILLS_TYPE_CONNECT_ENE )
			eSideToConvert = EAST;
		break;
	case( SSE ):
		if( mtType&MILLS_TYPE_CONNECT_SSW )
			eSideToConvert = SOUTH;
		break;
	case( SSW ):
		if( mtType&MILLS_TYPE_CONNECT_SSE )
			eSideToConvert = SOUTH;
		break;
	case( WSW ):
		if( mtType&MILLS_TYPE_CONNECT_WNW )
			eSideToConvert = WEST;
		break;
	case( WNW ):
		if( mtType&MILLS_TYPE_CONNECT_WSW )
			eSideToConvert = WEST;
		break;
	case( NNW ):
		if( mtType&MILLS_TYPE_CONNECT_NNE )
			eSideToConvert = NORTH;
		break;
	case( N ):	//fallthrough
	case( E ):	//fallthrough
	case( S ):	//fallthrough
	case( W ):
		//Fine, nothing to do!
		break;
	default:
		assert( false );

	}

	bool bRemoveEntireSide = false;
	dNewSize	= 0.00;
	switch( eSideToConvert )
	{
	case( NNE ):
		eExposedSide = NORTH;
		eHopupRail1	= ENE;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= WNW;
		eHopupMove	= NNE;
		eHopupOther	= NNW;
		dNewSize	= GetBayLength()/2.00;
		Vector.set( 0.00, 0.00, 0.00 );
		VectorHopupMove.set( -1.00*dNewSize, 0.00, 0.00 );
		break;
	case( ENE ):
		eExposedSide = EAST;
		eHopupRail1	= NNE;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= SSE;
		eHopupMove	= ENE;
		eHopupOther	= ESE;
		dNewSize	= GetBayWidth()/2.00;
		Vector.set( 0.00, -1.00*dNewSize, 0.00 );
		VectorHopupMove.set( 0.00, -1.00*dNewSize, 0.00 );
		break;
	case( ESE ):
		eExposedSide = EAST;
		eHopupRail1	= SSE;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= NNE;
		eHopupMove	= ESE;
		eHopupOther	= ENE;
		dNewSize	= GetBayWidth()/2.00;
		Vector.set( 0.00, 0.00, 0.00 );
		VectorHopupMove.set( 0.00, dNewSize, 0.00 );
		break;
	case( SSE ):
		eExposedSide = SOUTH;
		eHopupRail1	= ESE;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= WSW;
		eHopupMove	= SSE;
		eHopupOther	= SSW;
		dNewSize	= GetBayLength()/2.00;
		Vector.set( GetBayLength()/-2.00, 0.00, 0.00 );
		VectorHopupMove.set( -1.00*dNewSize, 0.00, 0.00 );
		break;
	case( SSW ):
		eExposedSide = SOUTH;
		eHopupRail1	= WSW;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= ESE;
		eHopupMove	= SSW;
		eHopupOther	= SSE;
		dNewSize	= GetBayLength()/2.00;
		Vector.set( 0.00, 0.00, 0.00 );
		VectorHopupMove.set( dNewSize, 0.00, 0.00 );
		break;
	case( WSW ):
		eExposedSide = WEST;
		eHopupRail1	= SSW;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= NNW;
		eHopupMove	= WSW;
		eHopupOther	= WNW;
		dNewSize	= GetBayWidth()/2.00;
		Vector.set( 0.00, dNewSize, 0.00 );
		VectorHopupMove.set( 0.00, dNewSize, 0.00 );
		break;
	case( WNW ):
		eExposedSide = WEST;
		eHopupRail1	= NNW;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= SSW;
		eHopupMove	= WNW;
		eHopupOther	= WSW;
		dNewSize	= GetBayWidth()/2.00;
		Vector.set( 0.00, 0.00, 0.00 );
		VectorHopupMove.set( 0.00, -1.00*dNewSize, 0.00 );
		break;
	case( NNW ):
		eExposedSide = NORTH;
		eHopupRail1	= WNW;	//important since if !bRemoveEntireSide this will be removed
		eHopupRail2	= ENE;
		eHopupMove	= NNW;
		eHopupOther	= NNE;
		dNewSize	= GetBayLength()/2.00;
		Vector.set( dNewSize, 0.00, 0.00 );
		VectorHopupMove.set( dNewSize, 0.00, 0.00 );
		break;
	case( NORTH ):
		eExposedSide = NORTH;
		eHopupRail1	= WNW;
		eHopupRail2	= ENE;
		Vector.set( GetBayLength()/-2.00, 0.00, 0.00 );
		eHopupMove	= NNW;
		eHopupOther	= NNE;
		bRemoveEntireSide = true;
		break;
	case( EAST ):
		eExposedSide = EAST;
		eHopupRail1	= NNE;
		eHopupRail2	= SSE;
		Vector.set( GetBayLength()/-2.00, 0.00, 0.00 );
		eHopupMove	= ENE;
		eHopupOther	= ESE;
		bRemoveEntireSide = true;
		break;
	case( SOUTH ):
		eExposedSide = SOUTH;
		eHopupRail1	= ESE;
		eHopupRail2	= WSW;
		Vector.set( GetBayLength()/-2.00, 0.00, 0.00 );
		eHopupMove	= SSE;
		eHopupOther	= SSW;
		bRemoveEntireSide = true;
		break;
	case( WEST ):
		eExposedSide = WEST;
		eHopupRail1	= NNW;
		eHopupRail2	= SSW;
		Vector.set( GetBayLength()/-2.00, 0.00, 0.00 );
		eHopupMove	= WSW;
		eHopupOther	= WNW;
		bRemoveEntireSide = true;
		break;
	default:
		assert( false );
		return;
	}

	//We did have a mills corner, better remove bracing
	DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eExposedSide );

	int			iRise;
	Component	*pComp;

	Transform.setToTranslation( Vector );
	TransformHopupMove.setToTranslation( VectorHopupMove );
	//Now we need to add some correctly sized components back
	for( iLift=0; iLift<GetNumberOfLifts(); iLift++ )
	{
		pLift = GetLift( iLift );

		if( !bRemoveEntireSide )
		{
			//convert the hopups to 1/2 width
			for( int i=0; i<pLift->GetNumberOfPlanksOnStageBoard( eExposedSide ); i++ )
			{
				pComp = pLift->GetComponent( CT_STAGE_BOARD, i, eExposedSide );
				if( pComp!=NULL )
				{
					pComp->SetLengthCommon( dNewSize );
					pComp->Move( Transform, true );
				}
			}
			pComp = pLift->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eHopupMove );
			if( pComp!=NULL )
			{
				pComp->Move( TransformHopupMove, true );
			}

			//convert the handrails to 1/2 width
			pComp = pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, eExposedSide );
			if( pComp!=NULL )
			{
				pComp->SetLengthCommon( dNewSize );
				pComp->Move( Transform, true );
			}
			pComp = pLift->GetComponent( CT_MID_RAIL, LIFT_RISE_0500MM, eExposedSide );
			if( pComp!=NULL )
			{
				pComp->SetLengthCommon( dNewSize );
				pComp->Move( Transform, true );
			}
			//The hand rail will no long touch the standard so remove Hopup bracket 1
			Delete1000mmHopupRailFromSide( eHopupRail1 );

			if( eExposedSide==NORTH || eExposedSide==SOUTH )
			{
				pComp = pLift->GetComponent( CT_TOE_BOARD, LIFT_RISE_0000MM, eExposedSide );
				if( pComp!=NULL )
				{
					pComp->SetLengthCommon( dNewSize );
					pComp->Move( Transform, true );
				}
				for( iRise=(int)LIFT_RISE_0000MM; iRise<=(int)pLift->GetRise(); iRise++ )
				{
					pComp = pLift->GetComponent( CT_LEDGER, (LiftRiseEnum)iRise, eExposedSide );
					if( pComp!=NULL )
					{
						pComp->SetLengthCommon( dNewSize );
						//pComp->SetType( CT_TRANSOM );
						pComp->Move( Transform, true );
					}
				}
			}

			for( iRise=(int)LIFT_RISE_0000MM; iRise<=(int)pLift->GetRise(); iRise++ )
			{
				pComp = pLift->GetComponent( CT_TRANSOM, (LiftRiseEnum)iRise, eExposedSide );
				if( pComp!=NULL )
				{
					pComp->SetLengthCommon( dNewSize );
					//pComp->SetType( CT_TRANSOM );
					pComp->Move( Transform, true );
				}
			}
			pComp = pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eExposedSide );
			if( pComp!=NULL )
			{
				pComp->SetLengthCommon( dNewSize );
				pComp->Move( Transform, true );
			}
			pComp = pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, eExposedSide );
			if( pComp!=NULL )
			{
				pComp->SetLengthCommon( dNewSize );
				pComp->Move( Transform, true );
			}
		}
		else
		{
			pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eHopupMove, LIFT_RISE_0000MM );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eHopupOther, LIFT_RISE_0000MM );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, eExposedSide );

			pLift->DeleteAllComponentsOfTypeFromSide( CT_RAIL, eExposedSide );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eExposedSide );
			Delete1000mmHopupRailFromSide( eHopupRail1 );
			Delete1000mmHopupRailFromSide( eHopupRail2 );

			if( eExposedSide==NORTH || eExposedSide==SOUTH )
			{
				pLift->DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eExposedSide );
				pLift->DeleteAllComponentsOfTypeFromSide( CT_LEDGER, eExposedSide );
			}
			pLift->DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eExposedSide );
			pLift->DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, eExposedSide );
		}
	}
}

MillsType Bay::CanBeMillsCnrBay()
{
	if( GetController()->GetSystem()!=S_MILLS )
		return MILLS_TYPE_NONE;
	return m_mtCanBeMillsCnrBay;
}

void Bay::SetCanBeMillsCnrBay( MillsType mtCanBeType )
{
	m_mtCanBeMillsCnrBay = mtCanBeType;
}

bool Bay::IsMillsTypeBay() const
{
	if( GetMillsSystemType()==MILLS_TYPE_NONE )
		return false;
	return true;
}

MillsType Bay::GetMillsSystemType() const
{
	return GetTemplate()->GetMillsSystemType();
}

void Bay::SetMillsTypeDumb( MillsType mtMillsCnrType )
{
	GetTemplate()->SetMillsSystemBay( mtMillsCnrType );
}


bool Bay::AddMillsType(MillsType mtType)
{
	MillsType mtValue;

	mtValue = GetMillsSystemType();


	if( mtValue==MILLS_TYPE_NONE && mtType!=MILLS_TYPE_NONE )
	{
		//store this bay so it can be retrived later
		m_pUndoMillsBay = new Bay();

		m_pUndoMillsBay->SetRunPointer(GetRunPointer());

		*m_pUndoMillsBay	= *this;
	}
	else if( m_pUndoMillsBay!=NULL && mtValue!=MILLS_TYPE_NONE && mtType==MILLS_TYPE_NONE )
	{
		//We are restoring a bay
		*this		= *m_pUndoMillsBay;

		m_pUndoMillsBay->SetDirtyFlag( DF_DELETE );
		m_pUndoMillsBay->ClearAllBayPointers( true, false );

		GetController()->IgnorErase(true);
		delete m_pUndoMillsBay;
		GetController()->IgnorErase(false);

		m_pUndoMillsBay		= NULL;

		mtValue = MILLS_TYPE_NONE;
	}

	//check it isn't already set
	if( (mtValue&mtType)>0 )
		return false;

	//combine and set
	mtValue = mtValue|mtType;
	SetMillsTypeDumb( mtValue );

	//fine
	return true;
}

void Bay::ClearMillsType()
{
	SetMillsTypeDumb( MILLS_TYPE_NONE );
}

bool Bay::HasMillsTypeGap(SideOfBayEnum eSide) const
{
	MillsType		mtType;
	bool	bResult;
	mtType = GetMillsSystemType();

	bResult = false;

	if( (mtType&MILLS_TYPE_CONNECT_TEST)>0 )
	{
		switch( eSide )
		{
		case( N ):
			if( (mtType&MILLS_TYPE_CONNECT_N)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_1200 );
				bResult = true;
			}
			break;
		case( E ):
			if( (mtType&MILLS_TYPE_CONNECT_E)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_1200 );
				bResult = true;
			}
			break;
		case( S ):
			if( (mtType&MILLS_TYPE_CONNECT_S)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_1200 );
				bResult = true;
			}
			break;
		case( W ):
			if( (mtType&MILLS_TYPE_CONNECT_W)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_1200 );
				bResult = true;
			}
			break;
		case( NNE ):
			if( (mtType&MILLS_TYPE_CONNECT_NNE)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( ENE ):
			if( (mtType&MILLS_TYPE_CONNECT_ENE)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( ESE ):
			if( (mtType&MILLS_TYPE_CONNECT_ESE)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( SSE ):
			if( (mtType&MILLS_TYPE_CONNECT_SSE)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( SSW ):
			if( (mtType&MILLS_TYPE_CONNECT_SSW)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( WSW ):
			if( (mtType&MILLS_TYPE_CONNECT_WSW)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( WNW ):
			if( (mtType&MILLS_TYPE_CONNECT_WNW)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( NNW ):
			if( (mtType&MILLS_TYPE_CONNECT_NNW)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool Bay::HasMillsTypeHalfSize(SideOfBayEnum eSide) const
{
	MillsType		mtType;
	bool	bResult;
	mtType = GetMillsSystemType();

	bResult = false;

	if( (mtType&MILLS_TYPE_CONNECT_TEST)>0 )
	{
		switch( eSide )
		{
		case( N ):
		case( S ):
			bResult = false;
			break;
		case( E ):
		case( W ):
			bResult = false;
			break;
		case( NNE ):
			if( (mtType&MILLS_TYPE_CONNECT_NNE)==0 && (mtType&MILLS_TYPE_CONNECT_NNW)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( ENE ):
			if( (mtType&MILLS_TYPE_CONNECT_ENE)==0 && (mtType&MILLS_TYPE_CONNECT_ESE)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( ESE ):
			if( (mtType&MILLS_TYPE_CONNECT_ESE)==0 && (mtType&MILLS_TYPE_CONNECT_ENE)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( SSE ):
			if( (mtType&MILLS_TYPE_CONNECT_SSE)==0 && (mtType&MILLS_TYPE_CONNECT_SSW)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( SSW ):
			if( (mtType&MILLS_TYPE_CONNECT_SSW)==0 && (mtType&MILLS_TYPE_CONNECT_SSE)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( WSW ):
			if( (mtType&MILLS_TYPE_CONNECT_WSW)==0 && (mtType&MILLS_TYPE_CONNECT_WNW)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( WNW ):
			if( (mtType&MILLS_TYPE_CONNECT_WNW)==0 && (mtType&MILLS_TYPE_CONNECT_WSW)>0 )
			{
				assert( GetBayWidth()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		case( NNW ):
			if( (mtType&MILLS_TYPE_CONNECT_NNW)==0 && (mtType&MILLS_TYPE_CONNECT_NNE)>0 )
			{
				assert( GetBayLength()==COMPONENT_LENGTH_2400 );
				bResult = true;
			}
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool Bay::SetMillsTypeGap(SideOfBayEnum eSide)
{
	if( HasMillsTypeGap(eSide) )
	{
		assert( false );
		return false;
	}

	switch( eSide )
	{
	case( NNE ):
		AddMillsType(MILLS_TYPE_CONNECT_NNE);
		break;
	case( ENE ):
		AddMillsType(MILLS_TYPE_CONNECT_ENE);
		break;
	case( ESE ):
		AddMillsType(MILLS_TYPE_CONNECT_ESE);
		break;
	case( SSE ):
		AddMillsType(MILLS_TYPE_CONNECT_SSE);
		break;
	case( SSW ):
		AddMillsType(MILLS_TYPE_CONNECT_SSW);
		break;
	case( WSW ):
		AddMillsType(MILLS_TYPE_CONNECT_WSW);
		break;
	case( WNW ):
		AddMillsType(MILLS_TYPE_CONNECT_WNW);
		break;
	case( NNW ):
		AddMillsType(MILLS_TYPE_CONNECT_NNW);
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}

bool Bay::HasMillsTypeMissingStandard(SideOfBayEnum eSide) const
{
	MillsType		mtType;
	bool	bResult;
	mtType = GetMillsSystemType();

	bResult = false;

	if( (mtType&MILLS_TYPE_MISSING_STND_TEST)>0 )
	{
		switch( eSide )
		{
		case( NE ):
			if( (mtType&MILLS_TYPE_MISSING_STND_NE)>0 )
				bResult = true;
			break;
		case( SE ):
			if( (mtType&MILLS_TYPE_MISSING_STND_SE)>0 )
				bResult = true;
			break;
		case( SW ):
			if( (mtType&MILLS_TYPE_MISSING_STND_SW)>0 )
				bResult = true;
			break;
		case( NW ):
			if( (mtType&MILLS_TYPE_MISSING_STND_NW)>0 )
				bResult = true;
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool Bay::RemoveMillsTypeStandard(SideOfBayEnum eSide)
{
	if( HasMillsTypeGap(eSide) )
	{
		assert( false );
		return false;
	}

	switch( eSide )
	{
	case( NE ):
		AddMillsType(MILLS_TYPE_MISSING_STND_NE);
		break;
	case( SE ):
		AddMillsType(MILLS_TYPE_MISSING_STND_SE);
		break;
	case( SW ):
		AddMillsType(MILLS_TYPE_MISSING_STND_SW);
		break;
	case( NW ):
		AddMillsType(MILLS_TYPE_MISSING_STND_NW);
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}

SideOfBayEnum Bay::GetSideFromMillsType( MillsType mtMillsCnrType)
{
	//i have assumed here that only one bit is set!

	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_NNE)>0 ) return NNE;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_ENE)>0 ) return ENE;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_ESE)>0 ) return ESE;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_SSE)>0 ) return SSE;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_SSW)>0 ) return SSW;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_WSW)>0 ) return WSW;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_WNW)>0 ) return WNW;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_NNW)>0 ) return NNW;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_N)>0 ) return N;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_E)>0 ) return E;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_S)>0 ) return S;
	if( (mtMillsCnrType&MILLS_TYPE_CONNECT_W)>0 ) return W;
	return SIDE_INVALID;
}

RoofProtectionEnum Bay::GetRoofProtection() const
{
	return m_eRoofProtection;
}

void Bay::SetRoofProtection(RoofProtectionEnum eRP)
{
	m_eRoofProtection = eRP;
}

void Bay::AddRoofProtectionComponents()
{
	bool	bSoleboard;
	double	dRLTop, dRLBottom;


	//we must have roof protection set prior to getting here
	assert( GetRoofProtection()==RP_1500MM ||
			GetRoofProtection()==RP_2000MM );

	////////////////////////////////////////////////////////////
	//NE
	dRLBottom	= GetStandardPosition( CNR_NORTH_EAST ).z;
	dRLTop		= GetRLOfTopLiftPlusHandrail( NE );
	bSoleboard	= HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NE );
	//Move the standards up to the correct height
	CreateStandardsArrangment( dRLTop-dRLBottom, CNR_NORTH_EAST, bSoleboard );

	////////////////////////////////////////////////////////////
	//NW
	dRLBottom	= GetStandardPosition( CNR_NORTH_WEST ).z;
	dRLTop		= GetRLOfTopLiftPlusHandrail( NW );
	bSoleboard	= HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NW );
	//Move the standards up to the correct height
	CreateStandardsArrangment( dRLTop-dRLBottom, CNR_NORTH_WEST, bSoleboard );

	////////////////////////////////////////////////////////////
	//Ledgers
	//Add some Ledgers at the correct height, they should
	//	belong to the top lift
	Lift	*pLift;
	pLift = GetLift( GetNumberOfLifts()-1 );
	if( pLift->GetComponent( CT_LEDGER, LIFT_RISE_1500MM, NORTH )==NULL )
		pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, LIFT_RISE_1500MM, MT_STEEL );

	if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, EAST )!=NULL &&
		pLift->GetComponent( CT_TRANSOM, LIFT_RISE_1500MM, EAST )==NULL )
		pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, LIFT_RISE_1500MM, MT_STEEL );

	if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, WEST )!=NULL &&
		pLift->GetComponent( CT_TRANSOM, LIFT_RISE_1500MM, WEST )==NULL )
		pLift->AddComponent( CT_TRANSOM, GetBayWidth(), WEST, LIFT_RISE_1500MM, MT_STEEL );

	if( GetRoofProtection()==RP_2000MM )
	{
		if( pLift->GetComponent( CT_LEDGER, LIFT_RISE_2000MM, NORTH )==NULL )
			pLift->AddComponent( CT_LEDGER, GetBayLength(), NORTH, LIFT_RISE_2000MM, MT_STEEL );

		if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, EAST )!=NULL &&
			pLift->GetComponent( CT_TRANSOM, LIFT_RISE_2000MM, EAST )==NULL )
			pLift->AddComponent( CT_TRANSOM, GetBayWidth(), EAST, LIFT_RISE_2000MM, MT_STEEL );

		if( pLift->GetComponent( CT_RAIL, LIFT_RISE_1000MM, WEST )!=NULL &&
			pLift->GetComponent( CT_TRANSOM, LIFT_RISE_2000MM, WEST )==NULL )
			pLift->AddComponent( CT_TRANSOM, GetBayWidth(), WEST, LIFT_RISE_2000MM, MT_STEEL );
	}
}

void Bay::RemoveRoofProtectionComponents( bool bExceptTopLift/*=true*/)
{
	bool	bSoleboard;
	double	dRLTop, dRLBottom;

/*	//we cannot have roof protection set prior to getting here
	assert( GetRoofProtection()!=RP_1500MM &&
			GetRoofProtection()!=RP_2000MM );
*/
	////////////////////////////////////////////////////////////
	//NE
	dRLBottom	= GetStandardPosition( CNR_NORTH_EAST ).z;
	dRLTop		= GetRLOfTopLiftPlusHandrail( NE );
	bSoleboard	= HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NE );
	CreateStandardsArrangment( (dRLTop-dRLBottom), CNR_NORTH_EAST, bSoleboard );

	////////////////////////////////////////////////////////////
	//NW
	dRLBottom	= GetStandardPosition( CNR_NORTH_WEST ).z;
	dRLTop		= GetRLOfTopLiftPlusHandrail( NW );
	bSoleboard	= HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NW );
	CreateStandardsArrangment( (dRLTop-dRLBottom), CNR_NORTH_WEST, bSoleboard );

	////////////////////////////////////////////////////////////
	//Ledgers
	//Remove the ledgers at 1.5m and 2.0m, for each lift, since
	//	each lift at some time was probably the top lift!
	int			i, iSide, iSize;
	Lift		*pLift;
	Component	*pComp;

	iSize = GetNumberOfLifts();
	if( bExceptTopLift )
		iSize--;

	for( i=0; i<iSize; i++ )
	{
		pLift = GetLift( i );

		for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
		{
			//delete the 1.5m ledger
			pComp = pLift->GetComponent( (iSide==(int)NORTH || iSide==(int)SOUTH )? CT_LEDGER : CT_TRANSOM, LIFT_RISE_1500MM, (SideOfBayEnum)iSide );
			if( pComp!=NULL )
				pLift->DeleteComponent( pComp->GetID() );
			//delete the 2.0m ledger
			pComp = pLift->GetComponent( (iSide==(int)NORTH || iSide==(int)SOUTH )? CT_LEDGER : CT_TRANSOM, LIFT_RISE_2000MM, (SideOfBayEnum)iSide );
			if( pComp!=NULL )
				pLift->DeleteComponent( pComp->GetID() );
		}
	}
}

//SIDE_INVALID is a good value to return here, that means everything
//	is ok, otherwise this will return the invalid side!
SideOfBayEnum Bay::FindInvalidRoofProtectionStandards( ) const
{

	double			dLength;
	doubleArray		daArrangment;
	SideOfBayEnum	eInvalidSide;

	eInvalidSide = SIDE_INVALID;
	//Does this bay have roof protection?
	if( GetRoofProtection()==RP_1500MM ||
		GetRoofProtection()==RP_2000MM )
	{
		//////////////////////////////////////////////////////////////
		//NE
		//It has roof protection so check top standard is 3.0m long!
		if( GetStandardsArrangement( daArrangment, CNR_NORTH_EAST ) )
		{
			//Warn if not 3.0m long!
			dLength = daArrangment.GetAt( daArrangment.GetSize()-1 );
			if( dLength>ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())-ROUND_ERROR &&
				dLength<ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())+ROUND_ERROR )
			{
				//Fine it is 3.0m long
			}
			else
			{
				//
				eInvalidSide = NORTH_EAST;
			}
		}
		else
		{
			//Why don't we have a standard arrangment on this side?
			//	 it must mean a mills standard is missing
			if( GetMillsSystemType()!=MILLS_TYPE_MISSING_STND_NE )
			{
				//we should have a standard here why don't we?
				assert( false );
			}
		}

		//////////////////////////////////////////////////////////////
		//NW
		if( GetStandardsArrangement( daArrangment, CNR_NORTH_WEST ) )
		{
			//Warn if not 3.0m long!
			dLength = daArrangment.GetAt( daArrangment.GetSize()-1 );
			if( dLength>ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())-ROUND_ERROR &&
				dLength<ConvertRLtoStarRL(COMPONENT_LENGTH_3000, GetStarSeparation())+ROUND_ERROR )
			{
				//Fine it is 3.0m long
			}
			else
			{
				if( eInvalidSide==NORTH_EAST )
				{
					//Both NE & NW are invalid
					eInvalidSide = NORTH;
				}
				else
				{
					//Just the nw is invalid
					eInvalidSide = NORTH_WEST;
				}
			}
		}
		else
		{
			//Why don't we have a standard arrangment on this side?
			//	 it must mean a mills standard is missing
			if( GetMillsSystemType()!=MILLS_TYPE_MISSING_STND_NW )
			{
				//we should have a standard here why don't we?
				assert( false );
			}
		}
	}
	else
	{
		//check the standard arrangment is OK!
	}

	//SIDE_INVALID is a good value to return here, that means everything
	//	is ok, otherwise this will return the invalid side!
	return eInvalidSide;
}

bool Bay::HasBeenVisited()
{
	return m_bVisited;
}

void Bay::SetHasBeenVisited(bool bVisited)
{
	m_bVisited = bVisited;
}

void Bay::Visit(Matrix3D Transform, bool bMoveRun/*=false*/)
{
	if( HasBeenVisited() )
		return;

	SetHasBeenVisited( true );

	if( bMoveRun )
	{
		assert( GetRunPointer()!=NULL );
		GetRunPointer()->VisitRun(Transform);
	}
	else
	{
		Move( Transform, true );
		MoveSchematic( Transform, true );
	}

	if( GetInner()!=NULL )
		GetInner()->Visit( Transform, bMoveRun );
	if( GetOuter()!=NULL )
		GetOuter()->Visit( Transform, bMoveRun );
	if( GetForward()!=NULL )
		GetForward()->Visit( Transform, bMoveRun );
	if( GetBackward()!=NULL )
		GetBackward()->Visit( Transform, bMoveRun );
}


int Bay::GetLowestHopupLiftID()
{
	int		i, iSide;
	Lift	*pLift;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		for( iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
		{
			if( pLift->GetComponentOfTypeOnSide( CT_STAGE_BOARD, (SideOfBayEnum)iSide )!=NULL )
			{
				//This is the lowest lift with a stage board
				return pLift->GetLiftID();
			}
		}
	}

	//If no hopups then we default to the top lift!
	return GetNumberOfLifts()-1;
}

void Bay::RecreateAllTieTubesFromTemplate()
{
	int					iTTT;
	BayTieTubeTemplate	*pTTT;
	BayTieTubeTemplates	pTTTs;

	pTTTs = *GetTemplate()->GetTieTubeTemplates();

	DeleteTies( NORTH_EAST );
	DeleteTies( SOUTH_EAST );
	DeleteTies( NORTH_WEST );
	DeleteTies( SOUTH_WEST );

	for( iTTT=0; iTTT<pTTTs.GetSize(); iTTT++ )
	{
		pTTT = new BayTieTubeTemplate();
		*pTTT = *pTTTs.GetAt(iTTT);
		CreateTieTubeFromTemplate( pTTT );
	}
}

void Bay::CreateTieTubeFromTemplate( BayTieTubeTemplate *pTTT, bool bAddToTemplate/*=true*/ )
{
	double				dFullLength;
	double				dBottomBayRL, dTopBayRL;
	Vector3D			Vector;
	Matrix3D			Transform3D, Rotation;
	LapboardBay			*pLapboardBay;
	SideOfBayEnum		eSide;
	CornerOfBayEnum		eCnr;

	GetRLsForBayByHeights( dBottomBayRL, dTopBayRL );

	//3D movement within Run
	Transform3D = GetTransform();
	//3D movement within World
	Transform3D*= GetRunPointer()->GetTransform();
	//3D movement of crosshair
	Transform3D*= GetRunPointer()->GetController()->Get3DCrosshairTransform();

	dFullLength	= pTTT->m_dFullLength;
	eCnr		= pTTT->m_eStandard;
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( pTTT->m_dAngle, Vector );

	if( bAddToTemplate )
		GetTemplate()->AddTieTubeTemplate( pTTT );

	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):
		eSide = N;
		pLapboardBay = GetLapboard( eSide );
		if( pLapboardBay==NULL )
		{
			eSide = E;
			pLapboardBay = GetLapboard( eSide );
			if( pLapboardBay==NULL )
			{
				eSide = N;
				if( GetForward()!=NULL )
					pLapboardBay = GetForward()->GetLapboard( eSide );
				if( pLapboardBay==NULL )
				{
					eSide = E;
					if( GetOuter()!=NULL )
						pLapboardBay = GetOuter()->GetLapboard( eSide );
				}
			}
		}
		break;
	case( CNR_SOUTH_EAST ):
		eSide = E;
		pLapboardBay = GetLapboard( eSide );
		if( pLapboardBay==NULL )
		{
			eSide = S;
			pLapboardBay = GetLapboard( eSide );
			if( pLapboardBay==NULL )
			{
				eSide = S;
				if( GetForward()!=NULL )
					pLapboardBay = GetForward()->GetLapboard( eSide );
				if( pLapboardBay==NULL )
				{
					eSide = E;
					if( GetInner()!=NULL )
						pLapboardBay = GetInner()->GetLapboard( eSide );
				}
			}
		}
		break;
	case( CNR_SOUTH_WEST ):
		eSide = S;
		pLapboardBay = GetLapboard( eSide );
		if( pLapboardBay==NULL )
		{
			eSide = W;
			pLapboardBay = GetLapboard( eSide );
			if( pLapboardBay==NULL )
			{
				eSide = S;
				if( GetBackward()!=NULL )
					pLapboardBay = GetBackward()->GetLapboard( eSide );
				if( pLapboardBay==NULL )
				{
					eSide = W;
					if( GetInner()!=NULL )
						pLapboardBay = GetInner()->GetLapboard( eSide );
				}
			}
		}
		break;
	case( CNR_NORTH_WEST ):
		eSide = W;
		pLapboardBay = GetLapboard( eSide );
		if( pLapboardBay==NULL )
		{
			eSide = N;
			pLapboardBay = GetLapboard( eSide );
			if( pLapboardBay==NULL )
			{
				eSide = N;
				if( GetBackward()!=NULL )
					pLapboardBay = GetBackward()->GetLapboard( eSide );
				if( pLapboardBay==NULL )
				{
					eSide = W;
					if( GetOuter()!=NULL )
						pLapboardBay = GetOuter()->GetLapboard( eSide );
				}
			}
		}
		break;
	default:
		assert( false );
	}

	int					i;
	bool				bUseMesh, bToeBoard, bAddRoofProtection = false;
	Lift				*pLift;
	double				dRL;
	Component			*pComp;
	doubleArray			daDecks;
	HandrailStyleEnum	eHRS = HRS_RAIL_ONLY;

	bToeBoard = false;
	if( pLapboardBay!=NULL )
	{
		Component	*pComp;
		//Add tubes and meshguard to these lapboard bays
		for( i=0; i<pLapboardBay->GetNumberOfLifts(); i++ )
		{
			pLift = pLapboardBay->GetLift(i);
			pComp = pLift->GetComponentOfTypeOnSide( CT_LAPBOARD, SOUTH );
			if( pComp!=NULL )
			{
				dRL = pLift->GetRL();
				dRL+= 2.00*GetStarSeparation();
				daDecks.Add( dRL );

				/////////////////////////////////////////////////////////////////////
				//Find the lift this lapboard comes off, preferrably with a meshguard!
				Lift *pNeighborLift;
				if( pLapboardBay->GetBayEast()!=NULL )
				{
					pNeighborLift = pLapboardBay->GetBayEast()->GetLiftAtRL( dRL-2.00*GetStarSeparation() );
				}
				if( pLapboardBay->GetBayWest()!=NULL && ( pNeighborLift==NULL ||
					(pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH )==NULL &&
					 pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH )==NULL ) ) )
				{
					Lift *pOldLift;
					pOldLift = pNeighborLift;
					pNeighborLift = pLapboardBay->GetBayWest()->GetLiftAtRL( dRL-2.00*GetStarSeparation() );
					if( pNeighborLift==NULL && pOldLift!=NULL )
					{
						//The old one was at least valid!
						pNeighborLift = pOldLift;
					}
				}
				if( pNeighborLift!=NULL )
				{
					///////////////////////////////////////////////////////////////////
					//Full Mesh
					if( pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH	)!=NULL ||
						pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, EAST	)!=NULL ||
						pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, WEST	)!=NULL )
					{
						eHRS = HRS_FULL_MESH;
						//no point having roof protection if you already have 2.0m mesh
						if( pLift->GetLiftID()==(pLapboardBay->GetNumberOfLifts()-1) )
						{
							bAddRoofProtection = false;
						}
					}
					///////////////////////////////////////////////////////////////////
					//Half Mesh
					else if(	pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH	)!=NULL ||
								pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, EAST	)!=NULL ||
								pNeighborLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, WEST	)!=NULL)
					{
						eHRS = HRS_HALF_MESH;
					}
					///////////////////////////////////////////////////////////////////
					//Midrail & toeboard
					else
					{
						if( GetController()->IsUseToeboardOnLapboard() )
						{
							bToeBoard = true;
						}
					}
				}

				bUseMesh = false;
				if( eHRS==HRS_FULL_MESH || eHRS==HRS_HALF_MESH )
					bUseMesh = true;
				//Rail (1.0m above deck)
				CreateTieTube( dRL, eCnr, dFullLength, Transform3D, Rotation, bUseMesh );
				//Mid Rail (0.5m above deck)
				CreateTieTube( dRL-GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation, false, bToeBoard, true );
				if( eHRS==HRS_FULL_MESH )
				{
					//Mesh rail (2.0m above deck)
					CreateTieTube( dRL+GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation );
					bUseMesh = true;
					//Mesh Mid Rail (1.5m above deck)
					CreateTieTube( dRL+(2.00*GetStarSeparation()), eCnr, dFullLength, Transform3D, Rotation, bUseMesh );
				}

				//If the bay has roof protection, and this is the top lift, Full mesh already has a 1.5m ledger
				if( eHRS!=HRS_FULL_MESH && GetRoofProtection()>=RP_1500MM &&
					pLift->GetLiftID()==(pLapboardBay->GetNumberOfLifts()-1) )
				{
					bAddRoofProtection = true;
				}
				if( bAddRoofProtection )
				{
					//add an extra tube or two
					//Roof protect Mid Rail (1.5m above deck)
					CreateTieTube( dRL+GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation );
					if( GetRoofProtection()==RP_2000MM )
						//Roof protect rail (2.0m above deck)
						CreateTieTube( dRL+(2.00*GetStarSeparation()), eCnr, dFullLength, Transform3D, Rotation );
				}
			}
		}
	}
	else
	{
		//There is no lapboard, so we should just place tiebars wherever there is a deck!
		//Add tubes and meshguard to these decks
		for( i=0; i<GetNumberOfLifts(); i++ )
		{
			pLift = GetLift(i);
			if( pLift->IsDeckingLift() )
			{
				dRL = pLift->GetRL();
				dRL+= 2.00*GetStarSeparation();
				daDecks.Add( dRL );

				if( pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, NORTH	)!=NULL ||
					pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, EAST	)!=NULL ||
					pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, WEST	)!=NULL )
				{
					eHRS = HRS_FULL_MESH;
					//no point having roof protection if you already have 2.0m mesh
					if( pLift->GetLiftID()==(GetNumberOfLifts()-1) )
					{
						bAddRoofProtection = false;
					}
				}
				///////////////////////////////////////////////////////////////////
				//Half Mesh
				else if(	pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, NORTH	)!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, EAST	)!=NULL ||
							pLift->GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, WEST	)!=NULL)
				{
					eHRS = HRS_HALF_MESH;
				}
				///////////////////////////////////////////////////////////////////
				//Midrail & toeboard
				else
				{
					if( GetController()->IsUseToeboardOnLapboard() )
					{
						bToeBoard = true;
					}
				}

				bUseMesh = false;
				if( eHRS==HRS_FULL_MESH || eHRS==HRS_HALF_MESH )
					bUseMesh = true;
				//Rail (1.0m above deck)
				CreateTieTube( dRL, eCnr, dFullLength, Transform3D, Rotation, bUseMesh );
				//Mid Rail (0.5m above deck)
				CreateTieTube( dRL-GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation, false, bToeBoard );
				if( eHRS==HRS_FULL_MESH )
				{
					//Mesh rail (2.0m above deck)
					CreateTieTube( dRL+GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation );
					bUseMesh = true;
					//Mesh Mid Rail (1.5m above deck)
					CreateTieTube( dRL+(2.00*GetStarSeparation()), eCnr, dFullLength, Transform3D, Rotation, bUseMesh );
				}

				//If the bay has roof protection, and this is the top lift, Full mesh already has a 1.5m ledger
				if( eHRS!=HRS_FULL_MESH && GetRoofProtection()>=RP_1500MM &&
					pLift->GetLiftID()==(GetNumberOfLifts()-1) )
				{
					bAddRoofProtection = true;
				}
				if( bAddRoofProtection )
				{
					//add an extra tube or two
					//Roof protect Mid Rail (1.5m above deck)
					CreateTieTube( dRL+GetStarSeparation(), eCnr, dFullLength, Transform3D, Rotation );
					if( GetRoofProtection()==RP_2000MM )
						//Roof protect rail (2.0m above deck)
						CreateTieTube( dRL+(2.00*GetStarSeparation()), eCnr, dFullLength, Transform3D, Rotation );
				}
			}
		}
	}

	//Add the other tie tubes not associated with a lapboard
	bool	bFound;
	double	dDeckRL, dSeparation;
	dSeparation = GetTemplate()->GetTieTubeVertSeparation();
	for( dRL = min( (dBottomBayRL+dSeparation), dTopBayRL );
						dRL<=dTopBayRL; dRL+=dSeparation )
	{
		bFound = false;
		for( i=0; i<daDecks.GetSize(); i++ )
		{
			dDeckRL = daDecks.GetAt(i);
			if( dRL>(dDeckRL-GetTieTubeVerticalTolerance()+ROUND_ERROR) &&
				dRL<(dDeckRL+dSeparation-ROUND_ERROR) )
			{
				bFound = true;

				//The next RL we test will be dDeckRL+dSeparation
				dRL = dDeckRL;
				break;
			}
		}
		if( !bFound )
		{
			//Normal tube, not associated with deck
			CreateTieTube( dRL, eCnr, dFullLength, Transform3D, Rotation );
		}
	}

	/////////////////////////////
	//Remove any 1.0m hopup attached to this standard!
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		assert( pLift!=NULL );
		SideOfBayEnum	eSide1, eSide2;
		switch( eCnr )
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
	
	/////////////////////////////
	//Schematic should be taken care of by the TieTubeTemplate object
	CreateSchematic();

	//schematic movement
	Matrix3D	Transform;
	Transform = GetSchematicTransform();
	Transform = GetRunPointer()->GetSchematicTransform() * Transform;
	MoveSchematic( Transform, false );
}

void Bay::CreateTieTube(double dRL, CornerOfBayEnum eCnr, double dFullLength, Matrix3D Transform3D, Matrix3D Rotation, bool bCreateMeshGuard/*=false*/, bool bCreateToeBoard/*=false*/, bool bHasLapboard/*=false*/ )
{
	double		dLength;
	Matrix3D	Transform, Trans;
	Vector3D	Vector;
	Component	*pTieTube, *pClamp1, *pClamp2, *pMesh, *pToeBoard;

	//Without this an assert goes off in the GetCommonBayLength function
	dLength = max( dFullLength, COMPONENT_LENGTH_0700 );
	dLength = min( dLength, COMPONENT_LENGTH_2400 );
	double		dMeshLength = GetCommonBayLength(dLength);

	pTieTube	= CreateComponentDumb( CT_TIE_TUBE, CornerAsSideOfBay( eCnr ), 0, dFullLength, MT_STEEL );
	pClamp1		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( eCnr ), 0, 0.00,		MT_STEEL );
	pClamp2		= CreateComponentDumb( CT_TIE_CLAMP_90DEGREE,	CornerAsSideOfBay( eCnr ), 0, 0.00,		MT_STEEL );

	if( bCreateMeshGuard )	pMesh		= CreateComponentDumb( CT_MESH_GUARD, CornerAsSideOfBay( eCnr ), 0, dMeshLength, MT_STEEL );
	if( bCreateToeBoard )
	{
		pToeBoard	= CreateComponentDumb( CT_TOE_BOARD, CornerAsSideOfBay( eCnr ), 0, dMeshLength, MT_STEEL );
		Vector.set( 1.00, 0.00, 0.00 );
		Transform.setToRotation( d90Deg, Vector );
		pToeBoard->Move( Transform, true );
	}

	//Without this the tube would cut into the
	//	standard and tube in actual components
	Vector.set( -1.00*pClamp1->GetLengthActual(false), 0.00, 0.00 );
	Transform.setToTranslation( Vector );
	pClamp1->Move( Transform, true );
	pClamp2->Move( Transform, true );
	//Both Clamps should be rotated 90 deg
	Vector.set( 0.00, 0.00, 1.00 );
	Transform.setToRotation( d90Deg,Vector );
	pClamp1->Move( Transform, true );
	pClamp2->Move( Transform, true );

	//The 2nd clamp belongs at the other end of the tie tube
	//The Tube needs to be on the other end of the ties
	Vector.set( -1.00*OVERRUN_TIE_TUBE, -1.00*(pClamp2->GetLengthActual()-(pTieTube->GetWidthActual()/2.00)), 0.00 );
	Transform.setToTranslation( Vector );
	pTieTube->Move( Transform, true );
	//the Meshgaurd hangs off the tube, so it needs to follow
	if( bCreateMeshGuard )	pMesh->Move( Transform, true );
	
	//The 2nd clamp belongs at the other end of the tie tube
	Vector.set( dFullLength-OVERRUN_TIE_TUBE-pClamp2->GetLengthActual(), 0.00, 0.00 );
	Transform.setToTranslation( Vector );
	pClamp2->Move( Transform, true );

	if( bCreateToeBoard )
	{
		//The toeboard should sit infront of the standard
		//	Note that is is now on its side so use the Height
		Vector.set( 0.00-OVERRUN_TIE_TUBE, (pToeBoard->GetHeightActual()/-2.00), 0.00 );
		//It should also be on the inside of the Standard
		Vector.y-= (GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false )/2.00);
		Transform.setToTranslation( Vector );
		pToeBoard->Move( Transform, true );
	}

	////////////////////////////////////////////////////////////////////////////
	//3D Movement around the Bay
	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):
		Vector.set( GetBayLengthActual(), GetBayWidthActual(), 0.00 );
		break;
	case( CNR_SOUTH_EAST ):
		Vector.set( GetBayLengthActual(), 0.00, 0.00 );
		break;
	case( CNR_SOUTH_WEST ):
		Vector.set( 0.00, 0.00, 0.00 );
		break;
	case( CNR_NORTH_WEST ):
		Vector.set( 0.00, GetBayWidthActual(), 0.00 );
		break;
	default:
		assert( false );
	}
	Transform.setToTranslation( Vector );

	//find the standards for the bay
	Transform*= Rotation;

	////////////////////////////////////////////////////////////////////////////
	//3D Movement vertically within Bay
	Vector.set( 0.00, 0.00, dRL );
	Trans.setToTranslation( Vector );
	Transform*= Trans;
	pTieTube->Move( Transform, true );
	if( bCreateMeshGuard )	pMesh->Move( Transform, true );
	if( bCreateToeBoard )	pToeBoard->Move( Transform, true );
	pClamp1->Move( Transform, true );
	pClamp2->Move( Transform, true );

	//The whole assembly(ex. Toeboards) needs to be moved so that
	//	the clamps sit above the rails
	Vector.set( 0.00, 0.00, 0.00  );
	switch( GetCompDetails()->GetSystem() )
	{
	case( S_MILLS ):
		Vector.z+= HIGH_STAR_MILLS_Z_ADJUST;
		break;
	case( S_KWIKSTAGE ):
		Vector.z+= HIGH_STAR_KWIKS_Z_ADJUST;
		break;
	case( S_OTHER ):
	default:
		assert( false );
		break;
	}
	Vector.z+= (pClamp1->GetHeightActual()/2.00);
	Vector.z+= GetCompDetails()->GetActualHeight( GetSystem(), CT_RAIL, COMPONENT_LENGTH_2400, MT_STEEL, false );
	Transform.setToTranslation( Vector );
	if( bCreateMeshGuard )	pMesh->Move( Transform, true );
	pTieTube->Move( Transform, true );
	pClamp1->Move( Transform, true );
	pClamp2->Move( Transform, true );

	//The Mesh need to be moved up 1/2 its height
	if( bCreateMeshGuard )
	{
		Vector.set( 0.00, 0.00, (pMesh->GetHeightActual()/2.00)-(2.00*GetStarSeparation())  );
		Transform.setToTranslation( Vector );
		pMesh->Move( Transform, true );
	}

	if( bCreateToeBoard )
	{
		//The toeboard need to be moved up 1/2 its height
		//	Note: it is on its side so we have to use the width
		Vector.set( 0.00, 0.00, (pToeBoard->GetWidthActual()/2.00)-GetStarSeparation()  );
		//and should sit on top of the existing planks
		Vector.z+= pToeBoard->GetAcurate3DAdjust( Z_AXIS );
		Vector.z+= GetCompDetails()->GetActualHeight( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL, false );
		//If we have a lapboard then the toe board will sit on that!
		if( bHasLapboard )
			Vector.z+= GetCompDetails()->GetActualHeight( GetSystem(), CT_STAGE_BOARD, COMPONENT_LENGTH_2400, MT_STEEL, false );
		Transform.setToTranslation( Vector );
		pToeBoard->Move( Transform, true );
	}

	//other 3d movement
	pTieTube->Move( Transform3D, false );
	//Mesh must be getting 3D movement from the bay it is attached to!
//	if( bCreateMeshGuard )	pMesh->Move( Transform3D, true );
//	if( bCreateToeBoard )	pToeBoard->Move( Transform3D, true );
	pClamp1->Move( Transform3D, false );
	pClamp2->Move( Transform3D, false );

}

double Bay::GetTieTubeVerticalTolerance()
{
	return m_dTieTubeVerticalTolerance;
}

void Bay::SetTieTubeVerticalTolerance(double dTolerance)
{
	m_dTieTubeVerticalTolerance = dTolerance;
}


void Bay::AddHandrailToNeighborAboveRL(double dRL, Bay *pNeighbor, SideOfBayEnum eNeighborSide )
{
	if( pNeighbor==NULL )
		return;

	int		iLift;
	Lift	*pLift;

	pLift = pNeighbor->GetLiftAtRL(dRL);

	for( iLift = pLift->GetLiftID(); iLift<pNeighbor->GetNumberOfLifts(); iLift++ )
	{
		pLift = pNeighbor->GetLift( iLift );
		if( pLift->IsDeckingLift() )
		{
			pLift->AddHandrailToSide( eNeighborSide ); 
		}
	}
}


HandrailStyleEnum Bay::GetHandrailStyle(SideOfBayEnum eSide, bool bCheckNeighborIfNone/*=false*/ )
{
	int					i;
	Lift				*pLift;
	HandrailStyleEnum	eHRS;

	if( eSide==ALL_SIDES )
	{
		int iHRS;
		iHRS = (int)GetHandrailStyle(NORTH, bCheckNeighborIfNone );
		iHRS = max( iHRS, (int)GetHandrailStyle(EAST, bCheckNeighborIfNone) );
		iHRS = max( iHRS, (int)GetHandrailStyle(SOUTH, bCheckNeighborIfNone) );
		iHRS = max( iHRS, (int)GetHandrailStyle(WEST, bCheckNeighborIfNone) );

		if( bCheckNeighborIfNone && iHRS==(int)HRS_NONE )
		{
			if( GetForward()!=NULL )
			{
				//You must ensure a false argument here, otherwise it will form an infinite loop
				iHRS = max( iHRS, (int)GetForward()->GetHandrailStyle( ALL_SIDES, false ) );
			}
			if( iHRS<HRS_FULL_MESH_TO_GROUND && GetBackward()!=NULL )
			{
				//You must ensure a false argument here, otherwise it will form an infinite loop
				iHRS = max( iHRS, (int)GetBackward()->GetHandrailStyle( ALL_SIDES, false ) );
			}
			if( iHRS<HRS_FULL_MESH_TO_GROUND && GetOuter()!=NULL )
			{
				//You must ensure a false argument here, otherwise it will form an infinite loop
				iHRS = max( iHRS, (int)GetOuter()->GetHandrailStyle( ALL_SIDES, false ) );
			}
			if( iHRS<HRS_FULL_MESH_TO_GROUND && GetInner()!=NULL )
			{
				//You must ensure a false argument here, otherwise it will form an infinite loop
				iHRS = max( iHRS, (int)GetInner()->GetHandrailStyle( ALL_SIDES, false ) );
			}
		}
		
		return (HandrailStyleEnum)iHRS;
	}

	eHRS = HRS_NONE;
	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift(i);
		assert( pLift!=NULL );

		eHRS = max( eHRS, pLift->GetHandrailStyle(eSide) );


		if( eHRS==HRS_FULL_MESH_TO_GROUND )
			break;	//got full mesh don't keep looking
	}

	return eHRS;
}

void Bay::SetAllComponentsVisible()
{
	int			i;
	Lift		*pLift;
	Component	*pComponent;

	for( i=0; i<GetNumberOfLifts(); i++ )
	{
		pLift = GetLift( i );
		assert( pLift!=NULL );
		pLift->SetAllComponentsVisible();
	}

	for( i=0; i<GetNumberOfBayComponents(); i++ )
	{
		pComponent = GetBayComponent( i );
		assert( pComponent!=NULL );
		pComponent->SetVisible(VIS_VISIBLE);
	}
}

void Bay::SetSystem(SystemEnum eSystem)
{
	GetTemplate()->SetSystem( eSystem );

	double dStarSeparation;
	switch( eSystem )
	{
	case( S_MILLS ):
		dStarSeparation = STAR_SEPARATION_MILLS;
		break;

	case( S_KWIKSTAGE ):
		dStarSeparation = STAR_SEPARATION_KWIKSTAGE;
		break;

	default:
	case( S_OTHER ):
		//We should not be setting a bay to this!
		assert( false );
		//fallthrough

	case( S_INVALID ):
		dStarSeparation = STAR_SEPARATION_APPROX;
	}
	SetStarSeparation( dStarSeparation );
}

SystemEnum Bay::GetSystem() const
{
	return GetTemplate()->GetSystem();
}

double Bay::GetStarSeparation() const
{
	return m_dBayStarSeparation;
}

void Bay::SetStarSeparation(double dSeparation)
{
	m_dBayStarSeparation = dSeparation;
}

double Bay::RLAdjust() const
{
	//The RL adjust is due to the fact that the first star
	//	is at 0m in the z & the jack sits beneath this
	//	this error was introduced at an early stage of coding
	//	and it was decided that it would be easier to fix it
	//	this way, this decision has since proved to be incorrect
	return GetStarSeparation();
}

double Bay::JackLength() const
{
	return GetStarSeparation();
}

void BayList::SetMustBeSameSystem(bool bEnsureSame)
{
	m_bMustBeSameSystem = bEnsureSame;
}

bool BayList::GetMustBeSameSystem() const
{
	return m_bMustBeSameSystem;
}

SystemEnum BayList::GetSystem() const
{
	SystemEnum eSystem;

	//default to invalid
	eSystem = S_INVALID;

	if( GetMustBeSameSystem() && GetSize()>0 )
	{
		//we can only have one system, so just return the first
		eSystem = GetAt(0)->GetSystem();
	}

	return eSystem;
}

AcDbObjectId Bay::GetSWStandardEntityID()
{
	return m_pSWStandardEntityID;
}

void Bay::SetSWStandardEntityID(AcDbObjectId ID)
{
	m_pSWStandardEntityID = ID;
}

bool Bay::IsBaySelected()
{
	return GetController()->AreObjectIDsSelected(GetTemplate()->GetObjectIds());
}

void Bay::RecreateLapsBoardsAndTieTubes( BayList *pBays/*=NULL*/ )
{
	int			i;
	Bay			*pBayWest, *pBayEast;
	LapboardBay	*pLap;

	RecreateAllLapboards();
	RecreateAllTieTubesFromTemplate();
	for( i=0; i<GetNumberOfLapboardPointers(); i++ )
	{
		pLap = GetLapboard( i );
		if( pLap!=NULL )
		{
			pBayWest = pLap->GetBayWest();
			pBayEast = pLap->GetBayEast();

			if( pBayEast==this && pBayWest!=NULL )
			{
				//check if it is in the list of bays that have been processed
				if( pBays==NULL || !pBays->IsInList( pBayWest ) )
				{
					//Not processed yet so do it now
					pBayWest->RecreateAllTieTubesFromTemplate();
				}
			}
			else if( pBayWest==this && pBayEast!=NULL )
			{
				//check if it is in the list of bays that have been processed
				if( pBays==NULL || !pBays->IsInList( pBayEast ) )
				{
					//Not processed yet so do it now
					pBayEast->RecreateAllTieTubesFromTemplate();
				}
			}
		}
		else
		{
			assert( false );
		}
	}
}
