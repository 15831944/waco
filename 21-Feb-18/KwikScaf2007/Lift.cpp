// Lift.cpp: implementation of the Lift class.
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
#include "Lift.h"
#include "Bay.h"
#include "Run.h"
#include "Controller.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//
Lift::Lift() : ComponentListArray()
{
	CreateLift();
	m_pBay = NULL;
	m_LiftType = LIFT_TYPE_UNDEFINED;
	DeleteAllComponentsFromLift();
	SetTransform( GetTransform().setToIdentity() );
	SetDirtyFlag( DF_CLEAN );
	m_eRise = LIFT_RISE_INVALID;
	m_iLiftID = ID_INVALID;
}


//////////////////////////////////////////////////////////////////////
// Destructor
//
Lift::~Lift()
{
	DeleteAllComponentsFromLift();
	m_pBay = NULL;
}


/////////////////////////////////////////////////////////////////////////////////
//	Lift level functions
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// SetLiftID
//
// Sets the lift ID - called by the bay class.
// The lift ID is it's position in the lift list array contained within
// the bay class.
//
void Lift::SetLiftID( int iLiftID )
{
	m_iLiftID = iLiftID;
}


//////////////////////////////////////////////////////////////////////
// GetLiftID
//
// returns the lift ID. 
//
int Lift::GetLiftID() const
{
	return m_iLiftID;
}


//////////////////////////////////////////////////////////////////////
//	CreateLift
//
//
bool Lift::CreateLift()
{
	return true;
}


//////////////////////////////////////////////////////////////////////
//	RemoveLift
//
//
void Lift::RemoveLift()
{
	SetDirtyFlag( DF_DELETE );
}


//////////////////////////////////////////////////////////////////////
// SetRise
// This function sets the rise of the lift.
//
void Lift::SetRise(LiftRiseEnum eRise)
{
	m_eRise = eRise;
	//adjust the RL's for all lifts
	if( GetBayPointer()!=NULL && GetBayPointer()->GetNumberOfLifts()>0 )
		GetBayPointer()->SetRLsForAllLifts( GetBayPointer()->GetRLOfLift(0) );


	if( eRise<LIFT_RISE_1500MM )
	{
		Component *pComp;
		pComp = GetComponent( CT_LEDGER, LIFT_RISE_1000MM, NORTH );
		if( pComp!=NULL )
			DeleteComponent( pComp->GetID() );
	}
	else
	{
		if( GetBayPointer()!=NULL &&
			GetBayPointer()->GetUseLedgerEveryMetre() )
		{
			Component *pComp;
			pComp = GetComponent( CT_LEDGER, LIFT_RISE_1000MM, NORTH );
			if( pComp==NULL )
				AddComponent( CT_LEDGER, GetBayPointer()->GetBayLength(), NORTH, LIFT_RISE_1000MM, MT_STEEL );
		}
	}
}


//////////////////////////////////////////////////////////////////////
// GetRise
//
// This function returns the rise of the lift.
//
LiftRiseEnum Lift::GetRise()
{
	return m_eRise;
}


//////////////////////////////////////////////////////////////////////
// WriteBOMTo
//
// This function writes a BOM( the list of components ) to a text file 
// pointed to in the arguement.
//
bool Lift::WriteBOMTo(CStdioFile *pFile)
{
	CString		sLine;
	Component	*pComponent;

	for( int iComponentID=0; iComponentID<GetNumberOfComponents(); iComponentID++ )
	{
		pComponent = GetComponent(iComponentID);
		pComponent->WriteBOMTo( pFile );
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
//	Group component functions
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//	Individual component functions
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AddComponent
//
// This function creates and adds a component to the list.
//
Component *Lift::AddComponent( ComponentTypeEnum eComponentType, double dLength,
							  SideOfBayEnum eSideOfBay, int iRise,
							  MaterialTypeEnum eMaterialType )
{
	bool			bCompRail, bCompMidRail, bCompToeBoard, bCompMesh,
						bCompBracing, bCompChain, bCompShade, bCompDeck;
	double			dStageWidth;
	CString			sMsg;
	Component		*pComp;
	SideOfBayEnum	eSide, eSide1, eSide2;

	/////////////////////////////////////////////////////////////////////////
	//we need to half the lenght of any mills system ledgers
	dLength = ChangeLengthAccordingToMillsCorner( eComponentType, eSideOfBay, dLength );

	////////////////////////////////////////////////////////////////////////////
	//check if the neighboring lift has matching hopup bracket
	if( HasMatchingHopupBracket( eComponentType, iRise, eSideOfBay ) )
	{
		return NULL;
	}

	////////////////////////////////////////////////////////////////////////////
	//check for midrails and chain mesh combo
	if( eComponentType==CT_MID_RAIL &&
		!GetBayPointer()->GetUseMidrailWithChainMesh() &&
		GetBayPointer()->HasBayComponentOfTypeOnSide( CT_CHAIN_LINK, eSideOfBay ) )
	{
		//We have stated that we shouldn't use a midrail with chainmesh so don't allow this
		return NULL;
	}

	////////////////////////////////////////////////////////////////////////////
	//check for complementing midrails, rails, ledgers or Trannies
	if( HasMatchingTransomLedgerHandrailOrMidrail( eComponentType, iRise, eSideOfBay ) )
	{
		return NULL;
	}

	////////////////////////////////////////////////////////////////////////////
	//check we are not creating a double up of components
	pComp = GetComponent( eComponentType, iRise, eSideOfBay );
	if( pComp!=NULL )
	{
		//This component already exists!
		//;//assert( false );
		return pComp;
	}

	;//assert( GetBayPointer()!=NULL );
	Component *pComponent = new Component(dLength, eComponentType, eMaterialType, GetSystem() );

	pComponent->SetLiftPointer(this);

	int	iID = ComponentListArray.AddComponent( eSideOfBay, iRise, pComponent );

	SetPositionOfComponent( pComponent, eComponentType, dLength, eSideOfBay, iRise );

	///////////////////////////////////////////////////////////////////
	//Some components cannot exist whilst other of a type exist!
	eSide = eSideOfBay;

	int				iLiftID;
	bool			bLowest, bRailRequired;
	Lift			*pLift;
	SideOfBayEnum	eSideLeft, eSideRight;
	switch( eComponentType )
	{
	default:
		break;

	case( CT_TRANSOM ):
	case( CT_LEDGER ):
		//If we are the top lift, and roof protection is required
		if( iRise==LIFT_RISE_0000MM && 
			GetBayPointer()->HasMovingDeck() &&
			GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 )
		{
			GetBayPointer()->RemoveRoofProtectionComponents();
			if( GetBayPointer()->GetRoofProtection()>=RP_1500MM )
			{
				//If the component doesn't already exist, then add it!
				bRailRequired = false;
				if( !GetBayPointer()->LapboardCrossesSideDoes(eSide) )
				{
					if( eSide==NORTH && GetBayPointer()->GetOuter()==NULL )
						bRailRequired = true;
					if( eSide==EAST && GetBayPointer()->GetForward()==NULL )
						bRailRequired = true;
					if( eSide==WEST && GetBayPointer()->GetBackward()==NULL )
						bRailRequired = true;
				}
				else
				{
					;//assert( false );
				}

				if( bRailRequired )
				{
					if( GetComponent( eComponentType, LIFT_RISE_1500MM, eSide )==NULL )
						AddComponent( eComponentType, dLength, eSide, LIFT_RISE_1500MM, MT_STEEL );
					if( GetBayPointer()->GetRoofProtection()==RP_2000MM &&
						GetComponent( eComponentType, LIFT_RISE_2000MM, eSide )==NULL )
						AddComponent( eComponentType, dLength, eSide, LIFT_RISE_2000MM, MT_STEEL );
				}
			}
		}
		break;

	case( CT_HOPUP_BRACKET ):
		//Fallthrough
		pLift = NULL;
		break;
	case( CT_STAGE_BOARD ):		//fallthrough
		bCompDeck		= IsDeckingLift();
		bCompRail		= HasComponentOfTypeOnSide( CT_RAIL, eSide );
		bCompMidRail	= HasComponentOfTypeOnSide( CT_MID_RAIL, eSide );
		bCompToeBoard	= HasComponentOfTypeOnSide( CT_TOE_BOARD, eSide );
		bCompMesh		= HasComponentOfTypeOnSide( CT_MESH_GUARD, eSide );
		bCompBracing	= GetBayPointer()->HasBayComponentOfTypeOnSide( CT_BRACING, eSide );
		bCompChain		= GetBayPointer()->HasBayComponentOfTypeOnSide( CT_CHAIN_LINK, eSide );
		bCompShade		= GetBayPointer()->HasBayComponentOfTypeOnSide( CT_SHADE_CLOTH, eSide );

		//if there is no deck then don't remove the rails
		if( bCompDeck )
		{
			if( bCompRail || bCompMidRail || bCompToeBoard || bCompMesh )
			{
				eSideLeft	= SIDE_INVALID;
				eSideRight	= SIDE_INVALID;
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
					;//assert( false );
				}

				DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
				DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
				DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );
				if( eSide==NORTH || eSide==SOUTH )
					DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideLeft );
				if( pComp!=NULL && eSideLeft!=SIDE_INVALID )
					DeleteComponent( pComp->GetID() );
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideRight );
				if( pComp!=NULL && eSideRight!=SIDE_INVALID )
					DeleteComponent( pComp->GetID() );
			}
		}
			
		if( bCompBracing || bCompChain || bCompShade )
		{
/*
			MessageBeep(MB_ICONQUESTION);
			sMsg.Format( "Placing a Stage Board on the %s side of lift%i, in bay%i, will remove the existing Handrail,\n",
				GetSideOfBayDescStr( eSide ), GetLiftID()+1, GetBayPointer()->GetBayNumber() );
			sMsg+= "Bracing, Chain Link and Shade Cloth from that side!\n\nDo you wish to remove them?";
			if( MessageBox( NULL, sMsg, "Warning", MB_YESNO|MB_ICONQUESTION )==IDYES )
			{
				DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
				DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
				DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
				DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );
				GetBayPointer()->DeleteAllBayComponentOfTypeFromSide( CT_BRACING, eSide );
				GetBayPointer()->DeleteAllBayComponentOfTypeFromSide( CT_CHAIN_LINK, eSide );
				GetBayPointer()->DeleteAllBayComponentOfTypeFromSide( CT_SHADE_CLOTH, eSide );
			}
*/
			GetBayPointer()->DeleteAllBayComponentsOfTypeFromSide( CT_BRACING, eSide );
			GetBayPointer()->DeleteAllBayComponentsOfTypeFromSide( CT_CHAIN_LINK, eSide );
			GetBayPointer()->DeleteAllBayComponentsOfTypeFromSide( CT_SHADE_CLOTH, eSide );
		}

		if( GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
		{
			//
			switch( eSide )
			{
			case( N ):
				eSideLeft	= NNE;
				eSideRight	= NNW;
				break;
			case( E ):
				eSideLeft	= ENE;
				eSideRight	= ESE;
				break;
			case( S ):
				eSideLeft	= SSE;
				eSideRight	= SSW;
				break;
			case( W ):
				eSideLeft	= WNW;
				eSideRight	= WSW;
				break;
			default:
				;//assert( false );
			}
			//if this is the lowest hopup in the bay then
			//	this should be the setting for the template
			bLowest = true;
			for( iLiftID=GetLiftID()-1; iLiftID>=0; iLiftID-- )
			{
				pLift = GetBayPointer()->GetLift( iLiftID );
				if( pLift->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSideLeft ) ||
					pLift->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSideRight ))
				{
					bLowest = false;
					break;
				}
			}

			if( bLowest )
			{
				pComponent = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eSideLeft );
				if( pComponent==NULL )
					pComponent = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eSideRight );
				if( pComponent!=NULL )
				{
					dStageWidth = pComponent->GetLengthActual();

					//This is the top lift so set the width of the stage board!
					switch( eSideLeft )
					{
					case( NNE ):
					case( NNW ):
						GetBayPointer()->GetTemplate()->SetWidthOfNStage( dStageWidth );
						break;
					case( ENE ):
					case( ESE ):
						GetBayPointer()->GetTemplate()->SetWidthOfEStage( dStageWidth );
						break;
					case( SSE ):
					case( SSW ):
						GetBayPointer()->GetTemplate()->SetWidthOfSStage( dStageWidth );
						break;
					case( WSW ):
					case( WNW ):
						GetBayPointer()->GetTemplate()->SetWidthOfWStage( dStageWidth );
						break;
					default:
						;//assert( false );
						break;
					}
				}
			}
		}
		break;
	case( CT_MESH_GUARD ):
		DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
		if( eSide==NORTH || eSide==SOUTH )
			DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );
		//fallthrough
	case( CT_RAIL ):
		if( eSide==SOUTH )
			eSide=S;
		pComp = GetComponent( CT_LEDGER, LIFT_RISE_1000MM, NORTH );
		if( pComp!=NULL && eSide==NORTH )
			DeleteComponent( pComp->GetID() );

		//If we are the top lift, and roof protection is required
		if( GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 && 
			GetBayPointer()->GetRoofProtection()>=RP_1500MM )
		{
			GetBayPointer()->RemoveRoofProtectionComponents();

			if( GetComponent( (eSide==N || eSide==S)? CT_LEDGER : CT_TRANSOM, LIFT_RISE_1500MM, eSide )==NULL )
				AddComponent( (eSide==N || eSide==S)? CT_LEDGER : CT_TRANSOM, dLength, eSide, LIFT_RISE_1500MM, MT_STEEL );
			if( GetBayPointer()->GetRoofProtection()==RP_2000MM &&
				GetComponent( (eSide==N || eSide==S)? CT_LEDGER : CT_TRANSOM, LIFT_RISE_2000MM, eSide )==NULL )
				AddComponent( (eSide==N || eSide==S)? CT_LEDGER : CT_TRANSOM, dLength, eSide, LIFT_RISE_2000MM, MT_STEEL );
		}
		//fallthrough

	case( CT_MID_RAIL ):		//fallthrough
	case( CT_TOE_BOARD ):
		//if there is no deck then don't remove the rails
		if( eComponentType==CT_TOE_BOARD && GetLiftID()==0 )
		{
			eComponentType=CT_TOE_BOARD;
		}

		if( IsDeckingLift() )
		{
			DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
			switch( eSide )
			{
			case( N ):	//Fallthrough
				eSide1 = NNE;
				eSide2 = NNW;
				break;
			case( E ):
				eSide1 = ENE;
				eSide2 = ESE;
				break;
			case( S ):
				eSide1 = SSE;
				eSide2 = SSW;
				break;
			case( W ):
				eSide1 = WSW;
				eSide2 = WNW;
				break;
			default:
				;//assert( false );
				break;
			}
			DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide1 );
			DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eSide2 );
		}
		break;
	}

	///////////////////////////////////////////////////////////////////
	//The schematic can be drawn if this is the 'schematic lift'
	bool bSchematicLift = false;
	if( GetBayPointer()!=NULL &&
		GetBayPointer()->GetController()!=NULL &&
		GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
	{
		bSchematicLift = ( GetLiftID()==GetBayPointer()->GetLowestHopupLiftID() );
	}
	else
	{
		bSchematicLift = ( GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 );
	}

	if( eComponentType==CT_CORNER_STAGE_BOARD )
	{
		;//assert(eComponentType==CT_CORNER_STAGE_BOARD);
	}

	if( bSchematicLift )
	{
		switch( eComponentType )
		{
		case( CT_HOPUP_BRACKET ):
			if( !GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
			{
				dStageWidth = GetBayPointer()->GetCompDetails()->
								GetActualLength( GetSystem(), CT_HOPUP_BRACKET, dLength, MT_STEEL );
				//This is the top lift so set the width of the stage board!
				switch( eSideOfBay )
				{
				case( NNE ):
				case( NNW ):
					GetBayPointer()->GetTemplate()->SetWidthOfNStage( dStageWidth );
					break;
				case( ENE ):
				case( ESE ):
					GetBayPointer()->GetTemplate()->SetWidthOfEStage( dStageWidth );
					break;
				case( SSE ):
				case( SSW ):
					GetBayPointer()->GetTemplate()->SetWidthOfSStage( dStageWidth );
					break;
				case( WSW ):
				case( WNW ):
					GetBayPointer()->GetTemplate()->SetWidthOfWStage( dStageWidth );
					break;
				default:
					;//assert( false );
					break;
				}
			}
			break;
		case( CT_CORNER_STAGE_BOARD ):
			switch( eSideOfBay )
			{
			case( NE ):
				GetBayPointer()->GetTemplate()->SetNEStageCnr( true );
				break;
			case( SE ):
				GetBayPointer()->GetTemplate()->SetSEStageCnr( true );
				break;
			case( SW ):
				GetBayPointer()->GetTemplate()->SetSWStageCnr( true );
				break;
			case( NW ):
				GetBayPointer()->GetTemplate()->SetNWStageCnr( true );
				break;
			default:
				;//assert( false );
				break;
			}
			break;
		case( CT_RAIL ):
			switch( eSideOfBay )
			{
			case( NORTH ):
				GetBayPointer()->GetTemplate()->SetNHandRail( true );
				break;
			case( EAST ):
				GetBayPointer()->GetTemplate()->SetEHandRail( true );
				break;
			case( SOUTH ):
				GetBayPointer()->GetTemplate()->SetSHandRail( true );
				break;
			case( WEST ):
				GetBayPointer()->GetTemplate()->SetWHandRail( true );
				break;
			default:
				;//assert( false );
				break;
			}
			break;
		case( CT_TIE_TUBE ):
			;//assert( false );
			break;
		default:
			//do nothing
			break;
		}
	}

	return pComponent;
}

//////////////////////////////////////////////////////////////////////
// RemoveComponent
// This function removes a component from the list based on the component ptr.
// NOTE: this function does not delete the component itself, just the pointer to the component
/*
bool Lift::RemoveComponent(Component *pComponent, int iCount)
{
	int iComponentIDToDelete;
	iComponentIDToDelete = -1;
	;//assert( false ); //I don't think this function is deleting the object properly
	for( int iComponentID = 0; iComponentID < GetNumberOfComponents();iComponentID++ )
	{
		if (GetComponent( iComponentID ) == pComponent)
			iComponentIDToDelete = iComponentID;
	}
	if (iComponentIDToDelete >= 0)
	{
		ComponentListArray.RemoveAt(iComponentIDToDelete, iCount);
		return true;
	}
	else
		return false;
}
*/

//////////////////////////////////////////////////////////////////////
// GetComponentType
//
// This function retrieves the component type of a component in the list based
// on the componentID.
//
ComponentTypeEnum Lift::GetComponentType( int iComponentID ) const
{
	return GetComponent( iComponentID )->GetType();
}


//////////////////////////////////////////////////////////////////////
// GetComponent
//
// This function returns a pointer to the component in the list which matches the
// component type and is at the rise height specified in the arguement.
//
Component * Lift::GetComponent(ComponentTypeEnum eComponentType, int iRise, SideOfBayEnum eSideOfBay) const
{
//	return ComponentListArray.GetComponent( eSideOfBay, iRise, eComponentType );
	int			i, iSize;
	Component	*pComponent;

	#ifdef _DEBUG
	switch( eComponentType )
	{
	///////////////////////////////////////////////////////
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
		;//assert( eSideOfBay==SOUTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_LEDGER ):
		;//assert( eSideOfBay==NORTH || eSideOfBay==SOUTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_TRANSOM ):
		if( GetBayPointer()->GetSystem()==S_KWIKSTAGE )
			;//assert( eSideOfBay==WEST || eSideOfBay==EAST );
		break;

	///////////////////////////////////////////////////////
	case( CT_STAGE_BOARD ):
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_MID_RAIL ):
	case( CT_BRACING ):
	case( CT_TIE_BAR ):
		;//assert( eSideOfBay>=NORTH && eSideOfBay<=WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_TOE_BOARD ):
		;//assert( eSideOfBay==SOUTH || eSideOfBay==NORTH );
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
	default:
		break;

	///////////////////////////////////////////////////////
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
	case( CT_CORNER_STAGE_BOARD ):
		;//assert( eSideOfBay>=NORTH_EAST && eSideOfBay<=SOUTH_WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_HOPUP_BRACKET ):
		;//assert( eSideOfBay>=NORTH_NORTH_EAST && eSideOfBay<=NORTH_NORTH_WEST );
		break;
	}
	#endif	//#ifdef _DEBUG

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent( i );
		;//assert( pComponent!=NULL );
		if( pComponent->GetType()==eComponentType )
		{
			//Same type
			if( GetPosition( i )==eSideOfBay )
			{
				//Same side of bay
				if( ((int)GetComponentRise(i))==iRise )
				{
					//Same rise - We have found it!
					return pComponent;
				}
			}
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// GetComponent
//
// This function returns a pointer to the component in the list identified by
// the componentID of a component in the list.
// It returns NULL if no corresponding component is found at the componentID
// location. 
//
Component *Lift::GetComponent( int iComponentID ) const
{
	return ComponentListArray.GetComponent( iComponentID );
}


//////////////////////////////////////////////////////////////////////
// DeleteComponent
//
// This function deletes a component from the list based on the componentID.
// Returns true if the component is deleted.
//
bool Lift::DeleteComponent( int iComponentID )
{
	bool				bReturn;
	Vector3D			Vector;
	Matrix3D			TransOrigRun, TransOrigRunSch, Trans;
	Component			*pComponent;
	SideOfBayEnum		eSide;
	ComponentTypeEnum	eType;
	

	//Delete from the schematic representation!
	pComponent = GetComponent(iComponentID);
	;//assert( pComponent!=NULL );
	eType = pComponent->GetType();
	eSide = GetPosition(iComponentID);

	if( eType==CT_RAIL && eSide==SOUTH )
	{
		eType=CT_RAIL;
		eSide=SOUTH;
	}

	bool bSchematicLift = false;
	if( GetBayPointer()!=NULL &&
		GetBayPointer()->GetController()!=NULL &&
		GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
	{
		bSchematicLift = ( GetLiftID()==GetBayPointer()->GetLowestHopupLiftID() );
	}
	else
	{
		bSchematicLift = ( GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 );
	}

	if( bSchematicLift )
	{
		switch( eType )
		{
		case( CT_HOPUP_BRACKET ):
			break;
		case( CT_CORNER_STAGE_BOARD ):
			//Stage corners
			switch( eSide )
			{
			case( NORTH_EAST ):
				GetBayPointer()->GetTemplate()->SetNEStageCnr( false );
				break;
			case( SOUTH_EAST ):
				GetBayPointer()->GetTemplate()->SetSEStageCnr( false );
				break;
			case( SOUTH_WEST ):
				GetBayPointer()->GetTemplate()->SetSWStageCnr( false );
				break;
			case( NORTH_WEST ):
				GetBayPointer()->GetTemplate()->SetNWStageCnr( false );
				break;
			default:
				;//assert( false );
				break;
			}	//switch( eSide )
			break;
		case( CT_STAGE_BOARD ):
			//Stage corners
			if( GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 )
			{
				//This is the top lift so set the width of the stage board!
				switch( eSide )
				{
				case( NORTH ):
					GetBayPointer()->GetTemplate()->SetWidthOfNStage( 0.00 );
					break;
				case( EAST ):
					GetBayPointer()->GetTemplate()->SetWidthOfEStage( 0.00 );
					break;
				case( SOUTH ):
					GetBayPointer()->GetTemplate()->SetWidthOfSStage( 0.00 );
					break;
				case( WEST ):
					GetBayPointer()->GetTemplate()->SetWidthOfWStage( 0.00 );
					break;
				default:
					;//assert( false );
					break;
				}	//switch( eSide )
			}
			break;
		case( CT_RAIL ):
			//HandRails
			switch( eSide )
			{
			case( NORTH ):
				GetBayPointer()->GetTemplate()->SetNHandRail( false );
				break;
			case( EAST ):
				GetBayPointer()->GetTemplate()->SetEHandRail( false );
				break;
			case( SOUTH ):
				GetBayPointer()->GetTemplate()->SetSHandRail( false );
				break;
			case( WEST ):
				GetBayPointer()->GetTemplate()->SetWHandRail( false );
				break;
			default:
				;//assert( false );
			}	//switch( eSide )
			break;
		case( CT_BRACING ):
			//HandRails
			switch( eSide )
			{
			case( NORTH ):
				GetBayPointer()->GetTemplate()->SetNBrace( false );
				break;
			case( EAST ):
				GetBayPointer()->GetTemplate()->SetEBrace( false );
				break;
			case( SOUTH ):
				GetBayPointer()->GetTemplate()->SetSBrace( false );
				break;
			case( WEST ):
				GetBayPointer()->GetTemplate()->SetWBrace( false );
				break;
			default:
				;//assert( false );
			}	//switch( eSide )
			break;
		default:
			//fine do nothing
			break;
		}	//switch( eType )
	}

	bReturn = ComponentListArray.DeleteComponent(iComponentID);

	if( GetNumberOfComponents()<=0 )
	{
		SetDirtyFlag( DF_DELETE );
	}

	//ResetComponentIDs();

	//If we are the top lift, and we have just removed the rail
	//	then we should also remove roof protection elements
	//	NOTE: this should come after we reset the component ids
	if( eType==CT_RAIL && GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 )
	{
		//The component ID's will be invalid at this stage
		pComponent = GetComponent( (eSide==NORTH||eSide==SOUTH)? CT_LEDGER:CT_TRANSOM, LIFT_RISE_1500MM, eSide );
		if( pComponent!=NULL )
			DeleteComponent( pComponent->GetID() );
		pComponent = GetComponent( (eSide==NORTH||eSide==SOUTH)? CT_LEDGER:CT_TRANSOM, LIFT_RISE_2000MM, eSide );
		if( pComponent!=NULL )
			DeleteComponent( pComponent->GetID() );
	}

	return bReturn;	
}


/////////////////////////////////////////////////////////////////////////////////
//	Lift 3D manipulation functions
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Move
//
//
bool Lift::Move(Matrix3D TransformMatrix, bool bStoreMovement/*=true*/)
{
	Component	*pComponent;

	for( int iComponentID=0; iComponentID<GetNumberOfComponents(); iComponentID++ )
	{
		pComponent = GetComponent(iComponentID);
		pComponent->Move( TransformMatrix, false );
	}

	//store it
	if( bStoreMovement )
	{
		SetTransform ( TransformMatrix * GetTransform() );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
//	Copy
//
//
bool Lift::Copy(Matrix3D TransformMatrix, bool bMove)
{
	TransformMatrix;
	bMove;
// CODE HERE!!
	return 0;
}



void Lift::SetBayPointer(Bay *pBay)
{
	m_pBay = pBay;
}

Bay * Lift::GetBayPointer() const
{
	return m_pBay;
}

void Lift::SetVisible(VisibilityEnum eVisible/*=VIS_VISIBLE*/)
{
	int			i, iSize;
	Component	*pComponent;

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent(i);
		pComponent->SetVisible( eVisible );
	}
}

VisibilityEnum Lift::GetVisible() const
{
	VisibilityEnum	eVisible;
	int				i, iSize;
	Component		*pComponent;

	iSize = GetNumberOfComponents();
	if( iSize<=0 )
	{
		return VIS_INVALID;
	}

	//find the first visibility
	pComponent = GetComponent(0);
	eVisible = pComponent->GetVisible();
	for( i=1; i<iSize; i++ )
	{
		pComponent = GetComponent(i);
		if( eVisible!=pComponent->GetVisible() )
			return VIS_MULTI_SELECTION;
	}
	return eVisible;
}

void Lift::Create3DView()
{
	int			i, iSize;
	Component	*pComponent;

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent(i);
		pComponent->Create3DView();
	}
}

void Lift::Delete3DView()
{
	int			i, iSize;
	Component	*pComponent;

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent(i);
		pComponent->Delete3DView();
	}
}

void Lift::SetPositionOfComponent( Component *pComponent, ComponentTypeEnum eComponentType, double dLength, SideOfBayEnum eSideOfBay, int iRise)
{
	int				iNumberOfPlanks, iStars;
	bool			bMoveX, bMoveY;
	double			dWidth, dRatio, dAngle, dExtraDistance, dExtraHeight, dSpace, dHeight, dPlankLength;
	Point3D			Standards[4];
	Vector3D		Vector, Vect;
	Matrix3D		Rotation, Transform, Trans, Aux;
	Component		*pCompTemp;

	//////////////////////////////////////////////////////////////////
	//Get the positions of the 4 standards for referance
	double dStandardWidthActual = GetBayPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL );

	Standards[CNR_SOUTH_EAST] = GetBayPointer()->GetStandardPosition( CNR_SOUTH_EAST );
	Standards[CNR_SOUTH_EAST].x-= dStandardWidthActual;

	Standards[CNR_NORTH_EAST] = GetBayPointer()->GetStandardPosition( CNR_NORTH_EAST );
	Standards[CNR_NORTH_EAST].x-= dStandardWidthActual;
	Standards[CNR_NORTH_EAST].y-= dStandardWidthActual;

	Standards[CNR_SOUTH_WEST] = GetBayPointer()->GetStandardPosition( CNR_SOUTH_WEST );

	Standards[CNR_NORTH_WEST] = GetBayPointer()->GetStandardPosition( CNR_NORTH_WEST );
	Standards[CNR_NORTH_WEST].y-= dStandardWidthActual;

	//////////////////////////////////////////////////////////////////
	//We will build this flat on the ground with the SE corner as the origin
	Standards[CNR_SOUTH_EAST].z = 0.00;
	Standards[CNR_NORTH_EAST].z = 0.00;
	Standards[CNR_SOUTH_WEST].z = 0.00;
	Standards[CNR_NORTH_WEST].z = 0.00;

	//////////////////////////////////////////////////////////////////
	//calculate the rotation required for the lift so that it will be
	//	the SE standard is at the origin, with the ledger along the +ve
	//	x axis, the transom along the +ve Yaxis and the lift extending
	//	upward in the +ve z direction!
	Vect = Standards[CNR_SOUTH_EAST] - Standards[CNR_SOUTH_WEST];
	//JSB todo 991201 - this /2.00 may be incorrect
	dAngle = CalculateAngle( Vect )/2.00;
	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( dAngle, Vector );

	//we will alway need a Matrix3D in this large switch statement coming up
	Trans.setToIdentity();

	//These indicate if we have to reposition this component during resizing
	bMoveX = false;
	bMoveY = false;

	//The dExtraDistance is used by lapboards to compensate for the fact
	//	the they are drawn from East to West, not SouthEast to SouthWest
	dExtraDistance	= 0.00;
	dExtraHeight	= 0.00;

	//Calculate the position of the component, given its type, length, etc
	switch( eComponentType )
	{
	//////////////////////////////////////////////////////////////////////////////
	case( CT_MESH_GUARD ):

		//Mesh guards cannot be less than 1200!
		#ifdef	SMALLEST_MESH_IS_1200
		dLength = max( dLength, COMPONENT_LENGTH_1200 );
		#endif	//#ifdef	SMALLEST_MESH_IS_1200

		//Position the Mesh as if it was being put on the West side
		Vector.set( 0.00, 0.00, 1.00 );
		Trans.setToRotation( pi/2.00, Vector );
		//move it to the outside of the standard
		dSpace = GetBayPointer()->GetBayWidthActual()-pComponent->GetLengthActual();
		if( eSideOfBay==NORTH || eSideOfBay==SOUTH )
			dSpace = GetBayPointer()->GetBayLengthActual()-pComponent->GetLengthActual();
		dSpace/= 2.00;
		Vector.set( 0.00, dSpace, (pComponent->GetHeightActual()/-2.00) );
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS);
		Transform.setToTranslation( Vector );
		Trans = Transform * Trans;

		Vector.set( 0.00, 0.00, 1.00 );
		switch( eSideOfBay )
		{
		case( NORTH ):
			//does this have to be moved when we scale?
			bMoveY = true;
			Transform.setToRotation( pi/-2.00, Vector );
			Trans = Transform * Trans;
			Vector.set( 0.00, GetBayPointer()->GetBayWidthActual(), 0.00);
			Transform.setToTranslation( Vector );
			Trans = Transform * Trans;
			break;
		case( EAST ):
			//does this have to be moved when we scale?
			bMoveX = true;
			bMoveY = true;
			Transform.setToRotation( pi, Vector );
			Trans = Transform * Trans;
			Vector.set( GetBayPointer()->GetBayLengthActual(),
						GetBayPointer()->GetBayWidthActual(), 0.00 );
			Transform.setToTranslation( Vector );
			Trans = Transform * Trans;
			break;
		case( SOUTH ):
			bMoveX = true;
			Transform.setToRotation( pi/2.00, Vector );
			Trans = Transform * Trans;
			Vector.set( GetBayPointer()->GetBayLengthActual(),
						0.00, 0.00 );
			Transform.setToTranslation( Vector );
			Trans = Transform * Trans;
			break;
		case( WEST ):
			//It is already positioned on the western side
			break;
		default:
			;//assert( false );
		}
		//It is currently positioned such that the top of the Mesh is
		//	at the top of the ledger or transom, so we need to move it
		//	up to the appropriate star!
		Vector.set( 0.00, 0.00, GetRiseFromRiseEnum((LiftRiseEnum)iRise, GetStarSeparation())-GetRiseFromRiseEnum(LIFT_RISE_0000MM, GetStarSeparation()) );
		break;

	//////////////////////////////////////////////////////////////////////////////
	case( CT_TIE_CLAMP_COLUMN ):	//fallthrough
	case( CT_TIE_CLAMP_MASONARY ):	//fallthrough
	case( CT_TIE_CLAMP_YOKE ):		//fallthrough
	case( CT_TIE_CLAMP_90DEGREE ):
		//Vector.set( GetLengthOfTie, 0.00, 0.00 );
		Vector.set( 100.00, 0.00, 0.00 );
		Aux.setToTranslation( Vector );
		Trans = Aux * Trans;
		//fallthrough

	//////////////////////////////////////////////////////////////////////////////
	case( CT_TIE_TUBE ):
		Vector.set( 0.00, 0.00, 1.00 );
		switch( eSideOfBay )
		{
		case( NNE ):
			//does this have to be moved when we scale?
			Aux.setToRotation( pi/2.00, Vector );
			Trans = Aux * Trans;
			//fallthrough
		case( ENE ):
			bMoveX = true;
			bMoveY = true;
			Vector.set( Standards[CNR_NORTH_EAST].x, Standards[CNR_NORTH_EAST].y, Standards[CNR_NORTH_EAST].z );
			Aux.setToTranslation( Vector );
			Trans = Aux * Trans;
			break;
		case( SSE ):
			Aux.setToRotation( pi/-2.00, Vector );
			Trans = Aux * Trans;
			//fallthrough
		case( ESE ):
			//does this have to be moved when we scale?
			bMoveX = true;
			Vector.set( Standards[CNR_SOUTH_EAST].x, Standards[CNR_SOUTH_EAST].y, Standards[CNR_SOUTH_EAST].z );
			Aux.setToTranslation( Vector );
			Trans = Aux * Trans;
			break;
		case( SSW ):
			Aux.setToRotation( pi/-2.00, Vector );
			Trans = Aux * Trans;
			break;
		case( WSW ):
			Aux.setToRotation( pi, Vector );
			Trans = Aux * Trans;
			break;
		case( WNW ):
			bMoveY = true;
			Aux.setToRotation( pi, Vector );
			Trans = Aux * Trans;
			Vector.set( Standards[CNR_NORTH_WEST].x, Standards[CNR_NORTH_WEST].y, Standards[CNR_NORTH_WEST].z );
			Aux.setToTranslation( Vector );
			Trans = Aux * Trans;
			break;
		case( NNW ):
			bMoveY = true;
			Aux.setToRotation( pi/2.00, Vector );
			Trans = Aux * Trans;
			Vector.set( Standards[CNR_NORTH_WEST].x, Standards[CNR_NORTH_WEST].y, Standards[CNR_NORTH_WEST].z );
			Aux.setToTranslation( Vector );
			Trans = Aux * Trans;
			break;
		default:
			;//assert( false );
		}
		Vector.set( 0.00, 0.00, GetRiseFromRiseEnum((LiftRiseEnum)iRise, GetStarSeparation())-GetRiseFromRiseEnum(LIFT_RISE_1000MM, GetStarSeparation()) );
		break;

	//////////////////////////////////////////////////////////////////////////////
	case( CT_STAIR ):
		//bay cannot be resized
		bMoveX = false;
		bMoveY = false;
		Vector.set( 0.00, 1.00, 0.00 );
		//the stair goes up 3 stars
		Aux.setToRotation( -1.00*atan((3.00*GetStarSeparation())/GetBayPointer()->GetBayLengthActual() ), Vector );
		Trans = Aux * Trans;
		switch( iRise )
		{
		case( SPR_OUTER ):
			//the stair goes up 3 stars
			Vector.set( 0.00, (pComponent->GetWidthActual()/-2.00), 0.00 );
			Vector.y+= GetBayPointer()->GetBayWidthActual();
			Vector.y-= dStandardWidthActual/2.00;
			break;
		case( SPR_INNER ):
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( -1.00*pi, Vector );
			Trans = Aux * Trans;
			Vector.set( GetBayPointer()->GetBayLengthActual(), (pComponent->GetWidthActual()/2.00), GetRiseFromRiseEnum(LIFT_RISE_1500MM, GetStarSeparation()) );
			Vector.y+= dStandardWidthActual/2.00;
			break;
		default:
			;//assert( false );
			break;
		}
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS);
		break;

	//////////////////////////////////////////////////////////////////////////////
	case( CT_LADDER ):
		bMoveX = false;
		bMoveY = false;

		//Note: a 4m ladder goes up 8 stars, whilst
		//	a 3.6m ladder goes up 6 stars
		//	2.0m Lift = 3.6m ladder = 6 Stars
		//	2.5m Lift = 3.6m ladder = 6 Stars
		//	3.0m Lift = 4.2m ladder = 8 Stars
		//	3.5m Lift = 4.2m ladder = 8 Stars
		//	4.0m Lift = 6.0m ladder = 11 Stars
		//	4.5m Lift = 6.0m ladder = 11 Stars
		if( dLength<=COMPONENT_LENGTH_6000+ROUND_ERROR &&
			dLength>=COMPONENT_LENGTH_6000-ROUND_ERROR )
			iStars = 11;
		else if( dLength<=COMPONENT_LENGTH_4200+ROUND_ERROR &&
				 dLength>=COMPONENT_LENGTH_4200-ROUND_ERROR )
			iStars = 8;
		else if( dLength<=COMPONENT_LENGTH_3600+ROUND_ERROR &&
				 dLength>=COMPONENT_LENGTH_3600-ROUND_ERROR )
			iStars = 6;
		else
			;//assert( false );

		Vector.set( 0.00, 1.00, 0.00 );
		Aux.setToRotation( -1.00*atan(((double)iStars*GetStarSeparation())/(GetBayPointer()->GetBayLengthActual()/2.00)), Vector );
		Trans = Aux * Trans;
		//Position the Ladder on the NE side of the bay just within the standards
		Vector.set( (GetBayPointer()->GetBayLengthActual()/2.00)-(dStandardWidthActual/2.00),
					(GetBayPointer()->GetBayWidthActual()-(dStandardWidthActual/2.00)
						-(pComponent->GetWidthActual()/2.00)), 0.00 );
		//The ladder must sit on the top of the boards
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS);
		Vector.z+= GetBayPointer()->GetController()->GetCompDetails()->
						GetActualHeight( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL, false );
		break;

	//////////////////////////////////////////////////////////////////////////////
	case( CT_TRANSOM ):			//fallthrough
	case( CT_LEDGER ):
	case( CT_RAIL ):			//fallthrough
	case( CT_MID_RAIL ):
		if( !GetTransAndVector( eSideOfBay, bMoveX, bMoveY, Trans, Vector, iRise, Standards, GetStarSeparation() ) )
			return;
		//This should position the ledger ontop of the star
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS)+(pComponent->GetHeightActual()/2.00);
		break;


	//////////////////////////////////////////////////////////////////////////////
	case( CT_LAPBOARD ):
		dExtraHeight = GetBayPointer()->GetController()->GetCompDetails()->
					GetActualHeight( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL, false );
		//fallthrough
		
	case( CT_DECKING_PLANK ):
		iNumberOfPlanks = GetNumberOfPlanksForWidth( GetBayPointer()->GetBayWidth() );

		//We need the planks to be positioned evenly along the length of the transom
		dWidth = GetBayPointer()->GetController()->GetCompDetails()->
					GetActualLength( GetSystem(), CT_TRANSOM, GetBayPointer()->GetBayWidth(),
															MT_STEEL, false );
		Vector.set( 0.00, 0.00, 0.00 );
		if( iNumberOfPlanks>1 )
		{
			;//assert( (iNumberOfPlanks*pComponent->GetWidthActual())<dWidth );
			dSpace = dWidth-(iNumberOfPlanks*pComponent->GetWidthActual());
			//If we had 5 planks then we need 4 spaces between the planks
			dSpace = dSpace/double(iNumberOfPlanks-1);
			Vector.y+= double(iRise)*(pComponent->GetWidthActual()+dSpace);
		}
		else
		{
			//There is only one plank
			1;
		}
		//Place next to the southern ledger
		Vector.y+= (pComponent->GetWidthActual()/2.00)+(dStandardWidthActual/2.00);
		//Lapboards need extra compensation
		if( eComponentType==CT_LAPBOARD )
		{
			iNumberOfPlanks = ((LapboardBay*)GetBayPointer())->GetTemplate()->m_iNumberOfBoards;
			Vector.y-= ((pComponent->GetWidthActual()+dSpace)*(double)iNumberOfPlanks)/2.00;
		}
		//The transom sits on top of the first star
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS)+(pComponent->GetHeightActual()/2.00);
		//Lapboards sit on top of decking planks
		Vector.z+= dExtraHeight;
		//Position the planks to be centred horizontally
		dPlankLength = pComponent->GetLengthActual();
		//If the plank is a short plank for a ladder tower then we
		//	need to position it from the left as if it was a long plank
		if( GetBayPointer()->GetBayLengthActual()*3.00/4.00>pComponent->GetLengthActual() )
			dPlankLength = GetBayPointer()->GetController()->GetCompDetails()->
							GetActualLength( GetSystem(), CT_DECKING_PLANK, GetBayPointer()->GetBayLength(),
							MT_STEEL, false );
		dSpace = GetBayPointer()->GetBayLengthActual()-dPlankLength;
		Vector.x+= (dSpace/2.00);
		break;


	//////////////////////////////////////////////////////////////////////////////
	case( CT_STAGE_BOARD ):
		//Q: How far appart should we space these planks?
		//A: Evenly along the hopup bracket
		switch( eSideOfBay )
		{
		case( NORTH ):
			pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, NORTH_NORTH_EAST );
			if(pCompTemp==NULL)
				pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, NORTH_NORTH_WEST );
			break;
		case( SOUTH ):
			pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, SOUTH_SOUTH_EAST );
			if(pCompTemp==NULL)
				pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, SOUTH_SOUTH_WEST );
			break;
		case( EAST ):
			pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, EAST_NORTH_EAST );
			if(pCompTemp==NULL)
				pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, EAST_SOUTH_EAST );
			break;
		case( WEST ):
			pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, WEST_NORTH_WEST );
			if(pCompTemp==NULL)
				pCompTemp = GetComponent( CT_HOPUP_BRACKET, 0, WEST_SOUTH_WEST );
			break;
		default:
			//invalid side
			;//assert( false );
			return;
		}
		//We must have the hopup bracket!
		//;//assert(pCompTemp==NULL);
		if(pCompTemp==NULL)
		{
			//;//assert( pCompTemp!=NULL );
			return;
		}

		if(pCompTemp != NULL)
		{
		  iNumberOfPlanks = (int)pCompTemp->GetLengthCommon(); // *** ERROR APPEARS HERE, AS pCompTemp SEEMS TO BE INVALID.  ~SJ, 28-04-2009
		}
		Vector.set( 0.00, 0.00, 0.00 );
		dRatio = 0.00;
		if( iNumberOfPlanks>1 )
		{
			//How long is the Bracket and the tie bar assembly
			dWidth = pCompTemp->GetLengthActual() + pComponent->GetAcurate3DAdjust( Y_AXIS );
			//If all the planks were pushed together how much space would we have
			dSpace = dWidth - ((double)iNumberOfPlanks*pComponent->GetWidthActual(false));
			//If we need 3 planks then we need 2 gaps between the planks
			dSpace/= double(iNumberOfPlanks-1);
			//Ratio is the distance from the start of one plank to the start of the next
			dRatio = pComponent->GetWidthActual(false)+dSpace;
		}
		//The iRise is the number of the plank (0 base)
		Vector.y+= dRatio*(double)iRise;

		Vector.y+= (pComponent->GetWidthActual()/2.00);
		dWidth = GetBayPointer()->GetController()->GetCompDetails()->
				GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
		Vector.y+= (dWidth/2.00);
		//The 'GetTransAndVector' function below will move the component 1/2
		//	the standard width right so we need to cancel this movement out
		Vector.x-= dWidth/2.00;
		//Position this component in the middle between the two standards
		dSpace = GetBayPointer()->GetBayWidthActual()-pComponent->GetLengthActual();
		if( eSideOfBay==NORTH || eSideOfBay==SOUTH )
			dSpace = GetBayPointer()->GetBayLengthActual()-pComponent->GetLengthActual();
		Vector.x+= (dSpace/2.00);
		//This stage board must sit on top of the lip of the bracket
		Vector.z+= (pComponent->GetHeightActual()/2.00);
		Vector.z+= pComponent->GetAcurate3DAdjust( Z_AXIS );
		Aux.setTranslation( Vector );
		Trans = Aux * Trans;

		//move it into its position around the lift
		if( !GetTransAndVector( eSideOfBay, bMoveX, bMoveY, Trans, Vector, iRise, Standards, 0.00 ) )
			return;

		break;


	//////////////////////////////////////////////////////////////////////////////
	case( CT_TOE_BOARD ):
		//Rotate the board on its side
		Vector.set( 1.00, 0.00, 0.00 );
		Trans.setToRotation( pi/2.00, Vector );
		dWidth = GetBayPointer()->GetController()->GetCompDetails()->
						GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
		dWidth/= 2.00;
		dSpace = dWidth-(pComponent->GetHeightActual()/2.00);
		if( eSideOfBay==NORTH )
		{
			Vector.set( 0.00, GetBayPointer()->GetBayWidth()+dSpace, 0.00 );
			bMoveY = true;
		}
		else if( eSideOfBay==SOUTH )
		{
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi, Vector );
			Trans = Aux * Trans;
			Vector.set( GetBayPointer()->GetBayLength(), dSpace+pComponent->GetHeightActual(), 0.00 );
			bMoveX = true;
		}
		else
		{
			//can't have toeboard anywhere else, since it
			//	will push the planks off!
			;//assert( false );
		}
		//The transom sits on top of the Decking plank
		dHeight = GetBayPointer()->GetController()->GetCompDetails()->
						GetActualHeight( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL, false );
		Vector.z+= pComponent->GetAcurate3DAdjust(Z_AXIS)+dHeight+(pComponent->GetWidthActual()/2.00);

		//Position the planks to be centred horizontally
		dSpace = GetBayPointer()->GetBayLengthActual()-pComponent->GetLengthActual();
		Vector.x+= (dSpace/2.00);
		break;


	//////////////////////////////////////////////////////////////////////////////
	case( CT_HOPUP_BRACKET ):
		//Position Bracket at SSW
		//rotate -90 deg around Z axis
		Vector.set( 0.00, 0.00, 1.00 );
		Trans.setToRotation( pi/-2.00, Vector );
		Vector.x = 0.00;
		//Move in Y to just touch standard
		Vector.y = (GetBayPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false )/-2.00);
		//Position just top inline with top of star
		Vector.z-= (pComponent->GetHeightActual()/2.00);
		Vector.z+= pComponent->GetAcurate3DAdjust( Z_AXIS );
		//Move in Z to be just beneath the star
		Vector.z+= GetRiseFromRiseEnum((LiftRiseEnum)iRise, GetStarSeparation());
		Aux.setTranslation( Vector );
		Trans = Aux * Trans;

		switch( eSideOfBay )
		{
		case( NORTH_NORTH_EAST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi, Vector );
			Trans = Aux * Trans;
			//translate
			Vector.set( GetBayPointer()->GetBayLengthActual(), GetBayPointer()->GetBayWidthActual(), 0.00 );
			//does this have to be moved when we scale?
			bMoveX = true;
			bMoveY = true;
			break;
		case( EAST_NORTH_EAST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi/2.00, Vector );
			Trans = Aux * Trans;
			//translate
			Vector.set( GetBayPointer()->GetBayLengthActual(), GetBayPointer()->GetBayWidthActual(), 0.00 );
			//does this have to be moved when we scale?
			bMoveX = true;
			bMoveY = true;
			break;
		case( EAST_SOUTH_EAST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi/2.00, Vector );
			Trans = Aux * Trans;
			//translate
			Vector.set( GetBayPointer()->GetBayLengthActual(), 0.00, 0.00 );
			//does this have to be moved when we scale?
			bMoveX = true;
			break;
		case( SOUTH_SOUTH_EAST ):
			//translate
			Vector.set( GetBayPointer()->GetBayLengthActual(), 0.00, 0.00 );
			//does this have to be moved when we scale?
			bMoveX = true;
			break;
		case( SOUTH_SOUTH_WEST ):
			Vector.set( 0.00, 0.00, 0.00 );
			break;
		case( WEST_SOUTH_WEST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi/-2.00, Vector );
			Trans = Aux * Trans;
			Vector.set( 0.00, 0.00, 0.00 );
			break;
		case( WEST_NORTH_WEST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi/-2.00, Vector );
			Trans = Aux * Trans;
			//translate
			Vector.set( 0.00, GetBayPointer()->GetBayWidthActual(), 0.00 );
			//does this have to be moved when we scale?
			bMoveY = true;
			break;
		case( NORTH_NORTH_WEST ):
			//rotate
			Vector.set( 0.00, 0.00, 1.00 );
			Aux.setToRotation( pi, Vector );
			Trans = Aux * Trans;
			//translate
			Vector.set( 0.00, GetBayPointer()->GetBayWidthActual(), 0.00 );
			//does this have to be moved when we scale?
			bMoveY = true;
			break;
		default:
			;//assert( false );
			return;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////
	case( CT_CORNER_STAGE_BOARD ):
		//Position the board inside the SW corner
		//Move the board so its SW cnr touches the NE cnr of the SW standard
		//	The 'GetTransAndVector' function will move the component in the X,
		//	so lets do the Y
		Vector.set( 0.00, 0.00, 0.00 );
		Vector.y+= (pComponent->GetWidthActual()/2.00);
		Vector.y+= (GetBayPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false )/2.00);
		//We need to position the bottom of the Corner filler up to the top of the higher star
		Vector.z+= (pComponent->GetHeightActual()/2.00);
		Vector.z+= pComponent->GetAcurate3DAdjust( Z_AXIS );
		Aux.setTranslation( Vector );
		Trans = Aux * Trans;

		//translate
		if( !GetTransAndVector( eSideOfBay, bMoveX, bMoveY, Trans, Vector, iRise, Standards, 0.00 ) )
			return;
		break;


	//////////////////////////////////////////////////////////////////////////////
	case( CT_BRACING ):				//fallthrough
	case( CT_CHAIN_LINK ):			//fallthrough
	case( CT_SHADE_CLOTH ):			//fallthrough
	case( CT_STANDARD ):			//fallthrough
	case( CT_STANDARD_OPENEND ):	//fallthrough
	case( CT_JACK ):				//fallthrough
	case( CT_SOLEBOARD ):			//fallthrough
	default:
		//These components don't belong to the lifts
		;//assert( false );
		return;
	}
	pComponent->SetMoveWhenBayLengthChanges( bMoveX );
	pComponent->SetMoveWhenBayWidthChanges( bMoveY );
	pComponent->SetLengthCommon( dLength );
	pComponent->CreatePointsIfPossible();

	Transform.setTranslation( Vector );
	Transform = Transform * Rotation;
	Transform = Transform * Trans;
	pComponent->Move( Transform, true );
}

Lift & Lift::operator =(const Lift &Original)
{
	int			i;
	Component	*pComponent, *pOriginComp;
	double		dLength;
	SideOfBayEnum	eSide;

//	ComponentListArray	= Original.ComponentListArray;
	for( i=0; i<Original.GetNumberOfComponents(); i++ )
	{
		pOriginComp = Original.GetComponent( i );

		//The length of the component may change depending on the size
		//	of the bay, so lets find the lenght required
		eSide = Original.GetPosition(i);
		switch( eSide )
		{
		case( NORTH ):
			dLength = GetBayPointer()->GetBayLength();
			break;
		case( SOUTH ):
			if( pOriginComp->GetType()!=CT_DECKING_PLANK &&
				GetBayPointer()->GetInner()!=NULL )
				continue;
			if( pOriginComp->GetType()==CT_LADDER || 
				pOriginComp->GetType()==CT_STAIR )
				dLength = pOriginComp->GetLengthCommon();
			else
				dLength = GetBayPointer()->GetBayLength();
			break;
		case( EAST ):
			dLength = GetBayPointer()->GetBayWidth();
			break;
		case( WEST ):
			if( GetBayPointer()->GetBackward()!=NULL )
				continue;
			dLength = GetBayPointer()->GetBayWidth();
			break;
		case( NORTH_EAST ):
			if( GetBayPointer()->GetOuter()!=NULL ||
				GetBayPointer()->GetForward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( SOUTH_EAST ):
			if( GetBayPointer()->GetInner()!=NULL ||
				GetBayPointer()->GetForward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( NORTH_WEST ):
			if( GetBayPointer()->GetOuter()!=NULL ||
				GetBayPointer()->GetBackward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( SOUTH_WEST ):
			if( GetBayPointer()->GetInner()!=NULL ||
				GetBayPointer()->GetBackward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( NORTH_NORTH_WEST ):
			//we don'w want a doubleup between the NNW and the NNE of the previous bay
			if( GetBayPointer()->GetBackward()!=NULL )
				continue;
			//fallthrough
		case( NORTH_NORTH_EAST ):
			if( GetBayPointer()->GetOuter()!=NULL )
				continue;
			//find the number of planks
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( EAST_SOUTH_EAST ):
			//we don'w want a doubleup between the ESE and the ENE of the inner bay
			if( GetBayPointer()->GetInner()!=NULL )
				continue;
			//fallthrough
		case( EAST_NORTH_EAST ):
			if( GetBayPointer()->GetForward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( SOUTH_SOUTH_WEST ):
			//we don'w want a doubleup between the SSW and the SSE of the previous bay
			if( GetBayPointer()->GetBackward()!=NULL )
				continue;
			//fallthrough
		case( SOUTH_SOUTH_EAST ):
			if( GetBayPointer()->GetInner()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		case( WEST_SOUTH_WEST ):
			//we don'w want a doubleup between the WSW and the WNW of the inner bay
			if( GetBayPointer()->GetInner()!=NULL )
				continue;
			//fallthrough
		case( WEST_NORTH_WEST ):
			if( GetBayPointer()->GetBackward()!=NULL )
				continue;
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		default:
			//side not set or invalid
			;//assert( false );
			dLength = (double)pOriginComp->GetLengthCommon();
			break;
		}
		//corner stage boards can have a zero length
		if( dLength==0.00 && pOriginComp->GetType()!=CT_CORNER_STAGE_BOARD )
		{
			;//assert( false );
			continue;
		}

		pComponent = AddComponent( pOriginComp->GetType(), dLength,
									eSide, Original.GetComponentRise(i),
									MT_STEEL );
	}
	if( m_eRise==LIFT_RISE_INVALID )
		m_eRise = Original.m_eRise;

	return *this;
}

int Lift::GetNumberOfComponents() const
{
	return ComponentListArray.GetSize();
}

void Lift::DeleteAll()
{
/*	//These items have a representation in the schematic, and so should be deleted properly
	DeleteAllHandrailComponentsFromSide( N );
	DeleteAllHandrailComponentsFromSide( E );
	DeleteAllHandrailComponentsFromSide( S );
	DeleteAllHandrailComponentsFromSide( W );
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, N );
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, E );
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, S );
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, W );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNE );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ENE );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, ESE );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSE );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, SSW );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WSW );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, WNW );
	DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, NNW );
	DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NE );
	DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SE );
	DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, SW );
	DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, NW );
*/
	ComponentListArray.DeleteAll();
/*
	Component *pComponent;
	while( GetNumberOfComponents()>0 )
	{
		pComponent = NULL;
		pComponent = GetComponent( 0 );
		if( pComponent!=NULL )
		{
			delete pComponent;
		}
		else
		{
			;//assert( false );
		}

		ComponentListArray.RemoveAt(0);
	}
*/
}

void Lift::SetLiftWidth(double dNewWidth)
{
	bool				bHasDecking;
	double				dNewWidthChange;
	Vector3D			Vect;
	Matrix3D			Transf;
	Component			*pComp;
	MaterialTypeEnum	eMT;

	;//assert( GetBayPointer()!=NULL );
	;//assert( GetBayPointer()->GetController()!=NULL );

	//how far do we need to move these suckers?
	dNewWidthChange = GetBayPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, dNewWidth, MT_STEEL );
	dNewWidthChange-= GetBayPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, GetBayPointer()->GetPreviousWidth(), MT_STEEL );
	Vect.set( 0.00, dNewWidthChange, 0.00 );
	Transf.setToTranslation( Vect );

	//default to steel
	eMT = MT_STEEL;
	bHasDecking = HasComponentOfTypeOnSide( CT_DECKING_PLANK, SOUTH );
	if( bHasDecking )
	{
		pComp = GetComponent( CT_DECKING_PLANK, LIFT_RISE_0000MM, SOUTH );
		;//assert( pComp!=NULL );
		eMT = pComp->GetMaterialType();
		
		//we will have to recreate the decking so remove them now,
		//	before the loop, so we don't have to process them!
		DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );
	}

	for( int iId=0; iId<GetNumberOfComponents(); iId++ )
	{
		pComp = GetComponent( iId );

		;//assert( pComp!=NULL );

		//There is a limit for the mesh guards
		dNewWidthChange = dNewWidth;
		#ifdef	SMALLEST_MESH_IS_1200
		if( pComp->GetType()==CT_MESH_GUARD )
			dNewWidthChange = max( dNewWidthChange, COMPONENT_LENGTH_1200 );
		#endif	//#ifdef	SMALLEST_MESH_IS_1200

		switch( GetPosition(iId) )
		{
		case( EAST ):				//fallthrough
		case( WEST ):				//fallthrough
			pComp->SetLengthCommon( dNewWidthChange );
			break;
		default:
			break;
		}

		if( pComp->GetMoveWhenBayWidthChanges() )
		{
			pComp->Move( Transf, true );
		}
	}

	if( bHasDecking )
	{
		//recreate the number decking planks, which we deleted before!
		int iNoOfPlanks = GetNumberOfPlanksForWidth( dNewWidth );
		double	dLngth;
		//dLngth = GetBayPointer()->GetBayLengthActual();
		dLngth = GetBayPointer()->GetBayLength();

		#ifdef _DEBUG
/*		if( GetBayPointer()==NULL )
			acutPrintf( "\nNumber Of Planks (B-,L%i): %i(%i)x%2.0f", GetLiftID(), iNoOfPlanks, GetNumberOfPlanksForWidth( dNewWidth ), dLngth );
		else
			acutPrintf( "\nNumber Of Planks (B%i,L%i): %i(%i)x%2.0f", GetBayPointer()->GetID(), GetLiftID(), iNoOfPlanks,GetNumberOfPlanksForWidth( dNewWidth ), dLngth );
*/		#endif	//#ifdef _DEBUG

		for( int l=0; l<iNoOfPlanks; l++ )
		{
			AddComponent( CT_DECKING_PLANK,	dLngth,	SOUTH, l, eMT );
		}
	}
}

void Lift::SetLiftHeight(double dNewHeight)
{
	dNewHeight;
/*	int			i;
	Component	*pComponent;

	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		pComponent->SetWidth( dNewHeight );
	}
*/
}

void Lift::SetLiftLength(double dNewLength, bool bMovePermitted/*=true*/)
{
	int			iCount;
	double		dChangeInLength;
	#ifdef	SMALLEST_MESH_IS_1200
	double		dOriginal;
	#endif	//#ifdef	SMALLEST_MESH_IS_1200
	Vector3D	Vector;
	Matrix3D	Transform;
	Component	*pComponent;

	;//assert( GetBayPointer()!=NULL );
	;//assert( GetBayPointer()->GetController()!=NULL );

	//how far do we need to move these suckers?
	dChangeInLength = GetBayPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, dNewLength, MT_STEEL );
	dChangeInLength-= GetBayPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_TRANSOM, GetBayPointer()->GetPreviousLength(), MT_STEEL );
	Vector.set( dChangeInLength, 0.00, 0.00 );
	Transform.setToTranslation( Vector );

	for( iCount=0; iCount<GetNumberOfComponents(); iCount++ )
	{
		pComponent = GetComponent( iCount );
		;//assert( pComponent!=NULL );

		//There is a limit for the mesh guards
		#ifdef	SMALLEST_MESH_IS_1200
		if( pComponent->GetType()==CT_MESH_GUARD )
		{
			dOriginal = dNewLength;
			dNewLength = max( dNewLength, COMPONENT_LENGTH_1200 );
		}
		#endif	//#ifdef	SMALLEST_MESH_IS_1200

		switch( GetPosition(iCount) )
		{
		case( NORTH ):				//fallthrough
		case( SOUTH ):				//fallthrough
			pComponent->SetLengthCommon( dNewLength );
			break;
		default:
			//Do nothing!
			break;
		}

		if( bMovePermitted && pComponent->GetMoveWhenBayLengthChanges() )
		{
			pComponent->Move( Transform, true );
		}

		#ifdef	SMALLEST_MESH_IS_1200
		if( pComponent->GetType()==CT_MESH_GUARD )
		{
			//restore the length!
			dNewLength = dOriginal;
		}
		#endif	//#ifdef	SMALLEST_MESH_IS_1200
	}
}

void Lift::ApplyBayResize( Matrix3D TransformX, Matrix3D TransformY )
{
	int					i, iNumberOfPlanks;
	Component			*pComponent;
	Matrix3D			OriginalTransform, Temp;
	MaterialTypeEnum	eMaterial;

	OriginalTransform = UnMove();

	/////////////////////////////////////////////////////////////////////////////
	//resize and move all components
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		pComponent->ApplyBayResize( TransformX, TransformY );
	}

	/////////////////////////////////////////////////////////////////////////////
	//The number of planks may have changed!
	Temp.setToIdentity();
	if( TransformY!=Temp )
	{
		/////////////////////////////////////////////////////////////////////////////
		//Set some default values
		eMaterial = MT_OTHER;

		/////////////////////////////////////////////////////////////////////////////
		//firstly lets get the number stage, level and mattype from the existing component
		for( i=0; i<GetNumberOfComponents(); i++ )
		{
			pComponent = GetComponent( i );
			if( (pComponent->GetType()==CT_DECKING_PLANK) &&
				(GetPosition(i)==SOUTH) )
			{
				eMaterial	= pComponent->GetMaterialType();
				break;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		//Delete the decking planks and replace
		DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );
		iNumberOfPlanks = GetNumberOfPlanksForWidth( GetBayPointer()->GetBayWidth() );
		for( i=0; i<iNumberOfPlanks; i++ )
		{
//			AddComponent( CT_DECKING_PLANK,	GetBayPointer()->GetBayLengthActual(),	SOUTH,	i, eMaterial );
			AddComponent( CT_DECKING_PLANK,	GetBayPointer()->GetBayLength(),	SOUTH,	i, eMaterial );
		}
	}

	Move( OriginalTransform, true );
}

void Lift::SetTransform(Matrix3D Transform)
{
	m_Transform = Transform;
}

Matrix3D Lift::GetTransform() const
{
	return m_Transform;
}

Matrix3D Lift::UnMove()
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

//////////////////////////////////////////////////////////////////
//Has the been marked for deletion
//////////////////////////////////////////////////////////////////
//Says this object needs to be deleted
DirtyFlagEnum	Lift::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

//set the object to be deleted
void	Lift::SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward/*=false*/)
{
	if( dfDirtyFlag==DF_CLEAN )
	{
		//set the dirty flag
		m_dfDirtyFlag = dfDirtyFlag;
		if(bSetDownward)
		{
			for( int i=0; i<GetNumberOfComponents(); i++ )
			{
				GetComponent(i)->SetDirtyFlag(dfDirtyFlag);
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
	if( GetBayPointer()!=NULL )
		GetBayPointer()->SetDirtyFlag( DF_DIRTY );
}

//delete all objects, that this object owns, if they are marked
//	for deletion
void	Lift::CleanUp()
{
	Component	*pComponent;
	int			i;

	SetDirtyFlag( DF_CLEAN );

	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent(i);
		if( pComponent->GetDirtyFlag()==DF_DELETE )
		{
			DeleteComponent(i);
			i--;	//we have just deleted the element, so don't increment i;
		}
	}

	if( GetNumberOfComponents()==0 )
		SetDirtyFlag( DF_DELETE );
}

int Lift::GetNumberOfPlanksOnStageBoard( SideOfBayEnum eSide ) const
{
	switch( eSide )
	{

	////////////////////////////////////////////////////////////////
	case( NORTH ):	//fallthrough
	case( SOUTH ):	//fallthrough
	case( WEST ):	//fallthrough
	case( EAST ):
		if( GetComponent( CT_STAGE_BOARD, PLANK03, eSide )!=NULL ) return NUM_PLANKS_FOR_STAGE_BRD_3_WIDE;
		if( GetComponent( CT_STAGE_BOARD, PLANK02, eSide )!=NULL ) return NUM_PLANKS_FOR_STAGE_BRD_2_WIDE;
		if( GetComponent( CT_STAGE_BOARD, PLANK01, eSide )!=NULL ) return NUM_PLANKS_FOR_STAGE_BRD_1_WIDE;
		break;

	////////////////////////////////////////////////////////////////
	case( SOUTH_SOUTH_WEST ):	//fallthrough
	case( NORTH_NORTH_WEST ):	//fallthrough
	case( NORTH_NORTH_EAST ):	//fallthrough
	case( EAST_NORTH_EAST ):	//fallthrough
	case( EAST_SOUTH_EAST ):	//fallthrough
	case( SOUTH_SOUTH_EAST ):	//fallthrough
	case( WEST_SOUTH_WEST ):	//fallthrough
	case( WEST_NORTH_WEST ):	//fallthrough
		return GetLengthOfHopupBracket(eSide);

	////////////////////////////////////////////////////////////////
	case( NORTH_EAST ):			//fallthrough
	case( SOUTH_EAST ):			//fallthrough
	case( NORTH_WEST ):			//fallthrough
	case( SOUTH_WEST ):			//fallthrough
	case( ALL_SIDES ):			//fallthrough
	case( ALL_VISIBLE_SIDES ):	//fallthrough
	case( SIDE_INVALID ):		//fallthrough
	default:
		//invalid side
		;//assert( false );
		break;
	}
	return 0;
}

bool Lift::AddStage( SideOfBayEnum eSide, int iNumberOfPlanks, MaterialTypeEnum eMaterial, bool bAdjustStandards/*=true*/ )
{
	int				iPlank;
	double			dLength;
	SideOfBayEnum	eCnrLeft, eCnrRight,
					eCornerTestLeft, eCornerTestRight,
					eCornerLeft, eCornerRight,
					eHopupHandrailLeft, eHopupHandrailRight;

	/////////////////////////////////////////////////////////////////////
	//We only have a certain number we can do!
	if( iNumberOfPlanks<NUM_PLANKS_FOR_STAGE_BRD_1_WIDE ||
		iNumberOfPlanks>NUM_PLANKS_FOR_STAGE_BRD_3_WIDE )
	{
		return false;
	}

/*	/////////////////////////////////////////////////////////////////////
	//We can't add a stage if there is bracing
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_BRACING, eSide ) )
	{
		//There is bracing in the way here, we can't add the stage
		//??? JSB todo 990907 - we should ask the user which they prefer!
		return false;
	}
*/

	/////////////////////////////////////////////////////////////////////
	//We need some of the particulars relating to the side
	eCnrLeft	= SIDE_INVALID;
	eCnrRight	= SIDE_INVALID;
	eHopupHandrailLeft	= SIDE_INVALID;
	eHopupHandrailRight	= SIDE_INVALID;
	switch( eSide )
	{
	case( NORTH ):
		dLength		= GetBayPointer()->GetBayLength();
		eCnrRight	= NORTH_NORTH_EAST;
		eCornerTestLeft		= WEST;
		eCornerTestRight	= EAST;
		eCornerLeft			= NW;
		eCornerRight		= NE;
		eHopupHandrailLeft	= NNW;
		eHopupHandrailRight	= NNE;
		if( GetBayPointer()->GetBackward()==NULL )
			eCnrLeft	= NORTH_NORTH_WEST;
		break;
	case( EAST ):
		dLength		= GetBayPointer()->GetBayWidth();
		eCnrLeft	= EAST_NORTH_EAST;
		eCornerTestLeft		= NORTH;
		eCornerTestRight	= SOUTH;
		eCornerLeft			= NE;
		eCornerRight		= SE;
		eHopupHandrailLeft	= ENE;
		eHopupHandrailRight	= ESE;
		if( GetBayPointer()->GetInner()==NULL )
			eCnrRight	= EAST_SOUTH_EAST;
		break;
	case( SOUTH ):
		dLength		= GetBayPointer()->GetBayLength();
		eCnrLeft	= SOUTH_SOUTH_EAST;
		eCornerTestLeft		= EAST;
		eCornerTestRight	= WEST;
		eCornerLeft			= SE;
		eCornerRight		= SW;
		eHopupHandrailLeft	= SSE;
		eHopupHandrailRight	= SSW;
		if( GetBayPointer()->GetBackward()==NULL )
			eCnrRight	= SOUTH_SOUTH_WEST;
		break;
	case( WEST ):
		dLength		= GetBayPointer()->GetBayWidth();
		eCnrRight	= WEST_NORTH_WEST;
		eCornerTestLeft		= SOUTH;
		eCornerTestRight	= NORTH;
		eCornerLeft			= SW;
		eCornerRight		= NW;
		eHopupHandrailLeft	= WSW;
		eHopupHandrailRight	= WNW;
		if( GetBayPointer()->GetInner()==NULL )
			eCnrLeft	= WEST_SOUTH_WEST;
		break;
	default:
		//invalid side
		;//assert( false );
		return false;
	}

	/////////////////////////////////////////////////////////////////////
	//Add the brackets
	if( eCnrLeft!=SIDE_INVALID )
	{
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eCnrLeft );
		AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks,	eCnrLeft,	LIFT_RISE_0000MM, MT_STEEL );
	}
	if( eCnrRight!=SIDE_INVALID )
	{
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET, eCnrRight );
		AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks,	eCnrRight,	LIFT_RISE_0000MM, MT_STEEL );
	}

	/////////////////////////////////////////////////////////////////////
	//Add the boards
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD, eSide );
	for( iPlank=PLANK01; iPlank<iNumberOfPlanks; iPlank++ )
	{
		AddComponent( CT_STAGE_BOARD, dLength, eSide, iPlank, eMaterial );
	}

	Component	*pComp;
	;//assert( iNumberOfPlanks>0 );

	//////////////////////////////////////////////////////////////////////
	//Do we need to add any corner hopups
	if( HasComponentOfTypeOnSide( CT_STAGE_BOARD, eCornerTestLeft ) )
	{
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, eCornerLeft );
		AddCornerStageBoard( SideOfBayAsCorner(eCornerLeft), MT_STEEL );
	}
	else
	{
		if( HasComponentOfTypeOnSide( CT_RAIL, eCornerTestLeft ) )
		{
			//we don't want a corner hopup, but we do have a handrail, so we will most likely need a hopup rail
			if( eHopupHandrailLeft!=SIDE_INVALID )
			{
				//make sure we don't already have one!
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupHandrailLeft );
				if( pComp==NULL )
					AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, eHopupHandrailLeft, LIFT_RISE_1000MM, MT_STEEL );
			}
		}
	}

	if( HasComponentOfTypeOnSide( CT_STAGE_BOARD, eCornerTestRight ) )
	{
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, eCornerRight );
		AddCornerStageBoard( SideOfBayAsCorner(eCornerRight), MT_STEEL );
	}
	else
	{
		if( HasComponentOfTypeOnSide( CT_RAIL, eCornerTestRight ) )
		{
			//we don't want a corner hopup, but we do have a handrail, so we will most likely need a hopup rail
			if( eHopupHandrailRight!=SIDE_INVALID )
			{
				//make sure we don't already have one!
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupHandrailRight );
				if( pComp==NULL )
					AddComponent( CT_HOPUP_BRACKET, (double)iNumberOfPlanks, eHopupHandrailRight, LIFT_RISE_1000MM, MT_STEEL );
			}
		}
	}

	//Adjust the standard heights
	if( bAdjustStandards )
	{
		GetBayPointer()->MakeAStandardFitLifts( SideOfBayAsCorner(eCornerLeft) );
		GetBayPointer()->MakeAStandardFitLifts( SideOfBayAsCorner(eCornerRight) );
	}

	return true;
}


bool Lift::AddCornerStageBoard( CornerOfBayEnum eCnr, MaterialTypeEnum eMaterial )
{
	int				iPlankLeft, iPlankRight;
	SideOfBayEnum	eCnrLeft, eCnrRight;

	/////////////////////////////////////////////////////////////////////
	//We need some of the particulars relating to the side
	switch( eCnr )
	{
	case( CNR_NORTH_EAST ):
		eCnrLeft	= NORTH_NORTH_EAST;
		eCnrRight	= EAST_NORTH_EAST;
		break;
	case( CNR_SOUTH_EAST ):
		eCnrLeft	= EAST_SOUTH_EAST;
		eCnrRight	= SOUTH_SOUTH_EAST;
		break;
	case( CNR_SOUTH_WEST ):
		eCnrLeft	= SOUTH_SOUTH_WEST;
		eCnrRight	= WEST_SOUTH_WEST;
		break;
	case( CNR_NORTH_WEST ):
		eCnrLeft	= WEST_NORTH_WEST;
		eCnrRight	= NORTH_NORTH_WEST;
		break;
	default:
		//invalid side
		;//assert( false );
		return false;
	}

	iPlankLeft	= GetNumberOfPlanksOnStageBoard( eCnrLeft );
	iPlankRight	= GetNumberOfPlanksOnStageBoard( eCnrRight );

	return AddCornerStageBoard( eCnr, iPlankLeft, iPlankRight, eMaterial );
}

bool Lift::AddCornerStageBoard(CornerOfBayEnum eCnr, int iPlankLeft, int iPlankRight, MaterialTypeEnum eMaterial)
{
	int iSize;

	iSize = GetCornerFillerFromDimensions( iPlankLeft, iPlankRight );
	if( iSize>=0 )
	{
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD, CornerAsSideOfBay( eCnr ) );

		/////////////////////////////////////////////////////////////////////
		//Add the brackets
		AddComponent( CT_CORNER_STAGE_BOARD, (double)iSize, CornerAsSideOfBay( eCnr ), iSize, eMaterial );

		return true;
	}
	return false;
}

bool Lift::GetTransAndVector(SideOfBayEnum eSide, bool &bMoveX, bool &bMoveY,
								Matrix3D &Trans, Vector3D &Vector, int iRise,
								Point3D Standards[4], double dRiseMultiplier/*=1.00*/ )
{
	//??? JSB optimization 990827 - try calculating all the possible
	//	results and hardcodeing the results directly into thier variables
	//	this should work for eveything except the last two lines
	//	regarding the Vector!
	double			dAngle;
	CornerOfBayEnum	eCnr;
	Matrix3D		Aux;

	bMoveX = false;
	bMoveY = false;
	switch( eSide )
	{
	case( NORTH ):
		dAngle = 0.00;
		eCnr = CNR_NORTH_WEST;
		bMoveY = true;
		break;
	case( SOUTH ):
		dAngle = pi;
		eCnr = CNR_SOUTH_EAST;
		bMoveX = true;
		break;
	case( EAST ):
		dAngle = pi/-2.00;
		eCnr = CNR_NORTH_EAST;
		bMoveX = true;
		bMoveY = true;
		break;
	case( WEST ):
		dAngle = pi/2.00;
		eCnr = CNR_SOUTH_WEST;
		break;
	case( NORTH_EAST ):
		dAngle = 0.00;
		eCnr = CNR_NORTH_EAST;
		bMoveX = true;
		bMoveY = true;
		break;
	case( SOUTH_EAST ):
		dAngle = pi/-2.00;
		eCnr = CNR_SOUTH_EAST;
		bMoveX = true;
		break;
	case( SOUTH_WEST ):
		dAngle = pi;
		eCnr = CNR_SOUTH_WEST;
		break;
	case( NORTH_WEST ):
		dAngle = pi/2.00;
		eCnr = CNR_NORTH_WEST;
		bMoveY = true;
		break;
	default:
		//invalid side!
		;//assert( false );
		return false;
	}
	Vector.set( 0.00, 0.00, 1.00 );
	Aux.setToRotation( dAngle, Vector );
	Trans = Aux * Trans;

	/////////////////////////////////////////////////////////////////////
	//Attach this to the standard, not the middle of the standard
	double dStdWidth=0.00;
	dStdWidth = GetBayPointer()->GetController()->GetCompDetails()->
							GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
	Vector.set( (dStdWidth/2.00), 0.00, 0.00 );
	Aux.setToTranslation( Vector );
	Trans = Trans*Aux;

	/////////////////////////////////////////////////////////////////////
	//iRise in the height relative to the bay
	Vector.set( 0.00, 0.00, 0.00 );
	if( bMoveX ) Vector.x+= GetBayPointer()->GetBayLengthActual();
	if( bMoveY ) Vector.y+= GetBayPointer()->GetBayWidthActual();
//	Vector = FromOriginToPointAsVector( Standards[eCnr] );
	Vector.z+= (double)iRise*dRiseMultiplier;
	return true;
}

bool Lift::DeleteAllComponentsOfTypeFromSide(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise/*=LIFT_RISE_INVALID*/)
{
	int			i;
	bool		bRemoved;
	Component	*pComponent;

	#ifdef _DEBUG
	switch( eType )
	{
	///////////////////////////////////////////////////////
	case( CT_DECKING_PLANK ):
	case( CT_LAPBOARD ):
		;//assert( eSide==SOUTH );
		break;

	///////////////////////////////////////////////////////
	case( CT_MESH_GUARD ):
	case( CT_RAIL ):
	case( CT_STAGE_BOARD ):
	case( CT_LEDGER ):
	case( CT_TRANSOM ):
	case( CT_CHAIN_LINK ):
	case( CT_SHADE_CLOTH ):
	case( CT_MID_RAIL ):
	case( CT_BRACING ):
	case( CT_TIE_BAR ):
		;//assert( eSide>=NORTH && eSide<=WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_TOE_BOARD ):
		;//assert( eSide==SOUTH || eSide==NORTH );
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
	default:
		break;

	///////////////////////////////////////////////////////
	case( CT_STANDARD ):
	case( CT_STANDARD_OPENEND ):
	case( CT_JACK ):
	case( CT_SOLEBOARD ):
	case( CT_CORNER_STAGE_BOARD ):
		;//assert( eSide>=NORTH_EAST && eSide<=SOUTH_WEST );
		break;

	///////////////////////////////////////////////////////
	case( CT_HOPUP_BRACKET ):
		;//assert( eSide>=NORTH_NORTH_EAST && eSide<=NORTH_NORTH_WEST );
		break;
	}
	#endif	//#endif _DEBUG

	bRemoved = false;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		if( (pComponent->GetType()==eType) &&
			(GetPosition(i)==eSide) )
		{
			if( iRise>=0 && pComponent->GetRise()!=(LiftRiseEnum)iRise )
				continue;

			DeleteComponent(i);
			i--;
			bRemoved = true;
		}
	}
	
	return bRemoved;
}

bool Lift::DeleteAllComponentsFromSide(SideOfBayEnum eSide)
{
	////////////////////////////////////////////////////////////////////////
	//Delete the Corner stageboards
	switch( eSide )
	{
	case( NORTH ):
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	NORTH_WEST			);
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	NORTH_WEST			);
		DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD,				eSide				);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			NORTH_NORTH_WEST	);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			NORTH_NORTH_EAST	);
		break;
	case( EAST ):
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	NORTH_EAST			);
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	SOUTH_EAST			);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			EAST_NORTH_EAST		);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			EAST_SOUTH_EAST		);
		break;
	case( SOUTH ):
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	SOUTH_WEST			);
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	SOUTH_EAST			);
		DeleteAllComponentsOfTypeFromSide( CT_LEDGER,				eSide				);
		DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD,				eSide				);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			SOUTH_SOUTH_EAST	);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			SOUTH_SOUTH_WEST	);
		break;
	case( WEST ):
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	SOUTH_WEST			);
		DeleteAllComponentsOfTypeFromSide( CT_CORNER_STAGE_BOARD,	NORTH_WEST			);
		DeleteAllComponentsOfTypeFromSide( CT_TRANSOM,				eSide				);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			WEST_SOUTH_WEST		);
		DeleteAllComponentsOfTypeFromSide( CT_HOPUP_BRACKET,			WEST_NORTH_WEST		);
		break;
	default:
		//invalid side
		;//assert( false );
		return false;
	}

	DeleteAllComponentsOfTypeFromSide( CT_RAIL,			eSide );
	DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL,		eSide );
	DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD,	eSide );
	DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD,	eSide );

	Component	*pComponent;
	pComponent = GetComponent( (eSide==N||eSide==S)? CT_LEDGER: CT_TRANSOM, LIFT_RISE_0500MM, eSide );
	if( pComponent!=NULL ) DeleteComponent( pComponent->GetID() );
	pComponent = GetComponent( (eSide==N||eSide==S)? CT_LEDGER: CT_TRANSOM, LIFT_RISE_1500MM, eSide );
	if( pComponent!=NULL ) DeleteComponent( pComponent->GetID() );
	pComponent = GetComponent( (eSide==N||eSide==S)? CT_LEDGER: CT_TRANSOM, LIFT_RISE_2000MM, eSide );
	if( pComponent!=NULL ) DeleteComponent( pComponent->GetID() );

	//These are internal and not attached to a side (well, south actually)
	//DeleteAllComponentsOfTypeFromSide( CT_STAIR,		eSide );
	//DeleteAllComponentsOfTypeFromSide( CT_LADDER,		eSide );
	//DeleteAllComponentsOfTypeFromSide( CT_STAGE_BOARD,	eSide );
	return true;
}

bool Lift::HasComponentOfTypeOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide)
{
	return (GetComponentOfTypeOnSide( eType, eSide )!=NULL );
}

Component * Lift::GetComponentOfTypeOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide)
{
	int			i;
	Component	*pComponent;

	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		if( (pComponent->GetType()==eType) &&
			(GetPosition(i)==eSide) )
		{
			return pComponent;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void Lift::Serialize(CArchive &ar)
{
	int				iNumberOfComponents, iComponentNumber, iRise, iSideOfBay;
	CString			sMsg, sTemp;
	SideOfBayEnum	eSideOfBay;
	Component		*pComponent;

	if (ar.IsStoring())    // Store Object?
	{
		ar << LIFT_VERSION_LATEST;

		iRise = m_eRise;
		ar << iRise;			//LiftRiseEnum		//Rise(height) of this lift

		m_Transform.Serialize(ar);		//this transformation is applied to all elements in this lift
		
		// Component List Storage - These are the components that are owned by this bay
		iNumberOfComponents = GetNumberOfComponents();			// store number of components
		ar << iNumberOfComponents;
		if (iNumberOfComponents > 0)
		{
			for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
			{
				iRise		= GetComponentRise(iComponentNumber); 
				eSideOfBay	= GetPosition(iComponentNumber);
				pComponent	= GetComponent(iComponentNumber);

				ar << iRise;

				iSideOfBay = eSideOfBay;
				ar << iSideOfBay;			//SideOfBayEnum		

				pComponent->Serialize(ar);				// store each component
			}
		}
		// End Component List Storage

	}
	else					// or Load Object?
	{
		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case LIFT_VERSION_1_0_1 :

			ar >> iRise;				//LiftRiseEnum
			m_eRise = (LiftRiseEnum)iRise;

			m_Transform.Serialize(ar);		//this transformation is applied to all elements in this lift
			
			// Component List Storage - These are the components that are owned by this bay
			ar >> iNumberOfComponents;
			if (iNumberOfComponents > 0)
			{
				for( iComponentNumber=0; iComponentNumber < iNumberOfComponents; iComponentNumber++ )
				{
					ar >> iRise;
					ar >> iSideOfBay;				//SideOfBayEnum
					eSideOfBay = (SideOfBayEnum)iSideOfBay;

					pComponent = new Component();
					pComponent->SetSystem(GetSystem());
					pComponent->SetID(iComponentNumber);
					pComponent->SetLiftPointer(this);
					pComponent->Serialize(ar);				// store each component
					ComponentListArray.AddComponent(eSideOfBay, iRise, pComponent);
				}
			}
			// End Component List Storage
			break;
		default:
			;//assert( false );
			if( uiVersion>LIFT_VERSION_LATEST )
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
      sMsg+= _T("Class: Lift.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), LIFT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}


/*****************************************************************
 *	History Records
 *****************************************************************
 * $History: Lift.cpp $ 
 * 
 * *****************  Version 158  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 157  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 156  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 155  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:50p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 154  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 153  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 152  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 151  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 150  *****************
 * User: Jsb          Date: 31/08/00   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 10/08/00   Time: 4:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 142  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 25/07/00   Time: 5:07p
 * Updated in $/Meccano/Stage 1/Code
 * Column Ties are not oriented correctly
 * 
 * *****************  Version 140  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 139  *****************
 * User: Jsb          Date: 18/07/00   Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 138  *****************
 * User: Jsb          Date: 17/07/00   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the 500mm star separation to 495.3
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 136  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 28/06/00   Time: 1:27p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create a seperate Toolbar project
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 9/06/00    Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * taking code home
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 8/06/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * Rotation of Bays is operational, rotation of laps is nearly working
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 1/06/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 4/05/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 3/05/00    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 3/04/00    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 28/03/00   Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 10/03/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 1/03/00    Time: 4:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 31/01/00   Time: 11:22a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:37p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 20/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 14/01/00   Time: 2:37p
 * Updated in $/Meccano/Stage 1/Code
 * Fixing the Dialog boxes at the moment
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 13/01/00   Time: 8:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 12/01/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 5/01/00    Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished the hopup without decking adds handrail
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12 (Beta13)
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 23/12/99   Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 23/12/99   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to move the move standards operation from the Bay::operator= to
 * the  
 * SetBayLength operation
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 23/12/99   Time: 9:26a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:41a
 * Updated in $/Meccano/Stage 1/Code
 * have removed all the _T('enum')s from the component details list
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 10/12/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * fixing up the chain link delete problem
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 9/12/99    Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 6/12/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * Got everything working as well as I had them at home
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 28/10/99   Time: 3:53p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Shade cloth and chain link now operational!
 * 2) Crash on setting TWH fixed
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 27/10/99   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 22/10/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Birdcaging pointers now fully operational
 * 2) Birdcaging now uses pascal's triangle for End of runs
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 19/10/99   Time: 9:03a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on Birdcaging
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 6/10/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * Schematic Corner Stage boards are now correct!
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Dar          Date: 9/15/99    Time: 8:50a
 * Updated in $/Meccano/Stage 1/Code
 * fixed iNumber << to >> serialize fault
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 9/09/99    Time: 3:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 45  *****************
 * User: Dar          Date: 9/09/99    Time: 1:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 43  *****************
 * User: Dar          Date: 9/08/99    Time: 3:31p
 * Updated in $/Meccano/Stage 1/Code
 * adding more serialize stuff
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/23/99    Time: 9:48a
 * Updated in $/Meccano/Stage 1/Code
 * Delete (so far)  is now working correctly
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/19/99    Time: 1:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/12/99    Time: 7:20p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the layers, visibility of autobuild, tight fit bay working
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 2:50p
 * Updated in $/Meccano/Stage 1/Code
 * Updating for andrews build
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
 * User: Dar          Date: 2/08/99    Time: 3:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 29/07/99   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Dar          Date: 27/07/99   Time: 11:28a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Dar          Date: 27/07/99   Time: 9:43a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Dar          Date: 21/07/99   Time: 5:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Dar          Date: 21/07/99   Time: 8:51a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Dar          Date: 20/07/99   Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * compilible group of classes componentList, componentListElement, Lift,
 * Component
 * 
 *****************************************************************/



LiftTypeEnum Lift::GetLiftType()
{
	return m_LiftType;
}

void Lift::SetLiftType(LiftTypeEnum LiftType)
{
	m_LiftType = LiftType;
}


void Lift::DeleteAllComponentsFromLift()
{
	DeleteAll();
}

void Lift::AddSpecialStairsLaddersComponentsToLift()
{
	int		iRise;

	////////////////////////////////////////////////////////////////////////////
	//according to bay type
	switch( GetLiftType() )
	{
	case( LIFT_TYPE_STAIRS ):
		if( GetRise()>=LIFT_RISE_1500MM )
		{
			AddComponent( CT_STAIR, COMPONENT_LENGTH_1500, SOUTH, SPR_OUTER, MT_STEEL );
			if( GetRise()==LIFT_RISE_3000MM )
			{
				AddComponent( CT_STAIR, COMPONENT_LENGTH_1500, SOUTH, SPR_INNER, MT_STEEL );
			}
		}
		break;
	case( LIFT_TYPE_LADDER ):
		//This should already be taken care of by the bay - the bays should call
		//	the CreateSpecialLadderLiftXXX() group of functions
		break;
	case( LIFT_TYPE_BUTTRESS ):
		//This should already be taken care of by the bay - the bays should call
		//	the CreateSpecialLadderLiftXXX() group of functions
		break;
	default:
		//you must set the Lift type before calling this funciton 
		;//assert( false );
		return;
	}

	////////////////////////////////////////////////////////////////////////////
	//Transoms/ledgers
	int		iRiseLimit, iRiseOfComponent;
	Lift	*pNeighborLift;
	double	dRLofLift, dRLofNeighborLift, dHandrailHeight, dRLofComponent;

	iRiseLimit = (int)GetRise();
	dRLofLift = GetRL()+GetBayPointer()->RLAdjust();
	if( GetLiftType()==LIFT_TYPE_LADDER &&
		GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 )
	{
		//This is the top lift of a ladder bay, 
		dHandrailHeight = GetBayPointer()->GetRLOfTopLiftPlusHandrail(N);
		dHandrailHeight = max( GetBayPointer()->GetRLOfTopLiftPlusHandrail(E), dHandrailHeight );
		dHandrailHeight = max( GetBayPointer()->GetRLOfTopLiftPlusHandrail(S), dHandrailHeight );
		dHandrailHeight = max( GetBayPointer()->GetRLOfTopLiftPlusHandrail(W), dHandrailHeight );
		if( IsDeckingLift() )
		{
			//We must have a handrail if we have decking
			dHandrailHeight = max( dRLofLift+(2*GetStarSeparation()), dHandrailHeight );
		}
		iRiseLimit = GetRiseEnumFromRise( dHandrailHeight - dRLofLift );
	}

	int iDirection;
	for( iDirection=(int)NORTH; iDirection<=(int)WEST; iDirection++ )
	{
		DeleteAllComponentsOfTypeFromSide( CT_RAIL, (SideOfBayEnum)iDirection );
		DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, (SideOfBayEnum)iDirection );
		if( iDirection==EAST || iDirection==WEST )
		{
			DeleteAllComponentsOfTypeFromSide( CT_TRANSOM, (SideOfBayEnum)iDirection );
		}
		else
		{
			DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, (SideOfBayEnum)iDirection );
			DeleteAllComponentsOfTypeFromSide( CT_LEDGER, (SideOfBayEnum)iDirection );
		}
	}

	if( GetLiftType()==LIFT_TYPE_BUTTRESS )
	{
		//Buttress bay only has transoms and ledgers at 2.0m intervals
		AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	NORTH,	LIFT_RISE_0000MM, MT_STEEL );
		AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	SOUTH,	LIFT_RISE_0000MM, MT_STEEL );
		AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		EAST,	LIFT_RISE_0000MM, MT_STEEL );
		AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		WEST,	LIFT_RISE_0000MM, MT_STEEL );
		iRise = LIFT_RISE_INVALID;
		if( iRiseLimit>LIFT_RISE_0000MM )
			iRise = iRiseLimit;
	}
	else
	{
		for( iRise=(int)LIFT_RISE_0000MM; iRise<iRiseLimit; iRise++ )
		{
			AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	NORTH,	iRise, MT_STEEL );
			AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		EAST,	iRise, MT_STEEL );

			if( GetBayPointer()->GetInner()==NULL )
			{
				AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	SOUTH,	iRise, MT_STEEL );
			}
			else
			{
				dRLofComponent = dRLofLift+GetRiseFromRiseEnum( (LiftRiseEnum)iRise, GetStarSeparation() );
				pNeighborLift = GetBayPointer()->GetInner()->GetLiftAtRL( dRLofComponent );
				if( pNeighborLift==NULL )
				{
					//we have just copied this bay so they should match
					;//assert( false );
					AddComponent( CT_LEDGER, GetBayPointer()->GetBayLength(), SOUTH, LIFT_RISE_0000MM, MT_STEEL );
				}
				else
				{
					dRLofNeighborLift = pNeighborLift->GetRL()+pNeighborLift->GetBayPointer()->RLAdjust();
					iRiseOfComponent = GetRiseEnumFromRise( dRLofComponent-dRLofNeighborLift );
					if( pNeighborLift->GetComponent( CT_LEDGER, iRiseOfComponent, NORTH )==NULL )
						pNeighborLift->AddComponent( CT_LEDGER, GetBayPointer()->GetBayLength(), NORTH, iRiseOfComponent, MT_STEEL );
				}
			}

			if( GetBayPointer()->GetBackward()==NULL )
			{
				AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		WEST,	iRise, MT_STEEL );
			}
			else
			{
				dRLofComponent = dRLofLift+GetRiseFromRiseEnum( (LiftRiseEnum)iRise, GetStarSeparation() );
				pNeighborLift = GetBayPointer()->GetBackward()->GetLiftAtRL( dRLofComponent );
				if( pNeighborLift==NULL )
				{
					//we have just copied this bay so they should match
					;//assert( false );
					AddComponent( CT_TRANSOM, GetBayPointer()->GetBayLength(), WEST, LIFT_RISE_0000MM, MT_STEEL );
				}
				else
				{
					dRLofNeighborLift = pNeighborLift->GetRL()+pNeighborLift->GetBayPointer()->RLAdjust();
					iRiseOfComponent = GetRiseEnumFromRise( dRLofComponent-dRLofNeighborLift );
					if( pNeighborLift->GetComponent( CT_TRANSOM, iRiseOfComponent, EAST )==NULL )
						pNeighborLift->AddComponent( CT_TRANSOM, GetBayPointer()->GetBayWidth(), EAST, iRiseOfComponent, MT_STEEL );
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//if this is the top lift then we should add one more at the top
	if( GetBayPointer()->GetNumberOfLifts()-1==GetLiftID() && iRise>=LIFT_RISE_0000MM )
	{
		AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		EAST,	iRise, MT_STEEL );
		AddComponent( CT_TRANSOM,	GetBayPointer()->GetBayWidth(),		WEST,	iRise, MT_STEEL );
		AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	NORTH,	iRise, MT_STEEL );
		AddComponent( CT_LEDGER,	GetBayPointer()->GetBayLength(),	SOUTH,	iRise, MT_STEEL );
	}
}

//creates an empty lift which a ladder could pass through
void Lift::CreateSpecialLadderLiftEmpty()
{
	DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );
	;//assert( IsEmptyLift() );
}

/////////////////////////////////////////////////////////////////////////////////////
//creates a special lift which only has two planks on which the ladder
//	is placed, this is only ever used a the bottom of the lift!
void Lift::CreateSpecialLadderLift2Board(double dLadderLength)
{
	int i, iBoards;
	MaterialTypeEnum	eMT;

	//default to steel;
	eMT = MT_STEEL;

	;//assert( GetBayPointer()!=NULL );

	if( HasComponentOfTypeOnSide( CT_DECKING_PLANK, SOUTH ) )
	{
		//We already have decking here, it should be a full deck, so just leave it!
	}
	else
	{
		//We don't have any decking here, better add two planks
		DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );

		iBoards = GetNumberOfPlanksForWidth( GetBayPointer()->GetBayWidth() );
		for( i=iBoards-2; i<iBoards; i++ )
		{
			AddComponent( CT_DECKING_PLANK, GetBayPointer()->GetBayLength(), SOUTH, i, eMT );
		}
	}

	if( dLadderLength==COMPONENT_LENGTH_3600 ||
		dLadderLength==COMPONENT_LENGTH_4200 ||
		dLadderLength==COMPONENT_LENGTH_6000 )
	{
		AddComponent( CT_LADDER, dLadderLength, SOUTH, 0, MT_STEEL );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//This is a special type of lift, which is only used for decks of <=
//	2.0m from the ground (ie bottom lift rise is only 0.5m->1.5m
//	This lift must have decking, of which we will remove the last
//	two planks.
void Lift::CreateSpecialLadderLiftLowLift()
{
	int					i, iBoards;
	Bay					*pBay;
	Lift				*pLowerLift;
	MaterialTypeEnum	eMT = MT_STEEL;

	;//assert( GetLiftID()==1 );
	
	Component	*pComp = GetComponent( CT_DECKING_PLANK, LIFT_RISE_0000MM, SOUTH );
	if( pComp!=NULL )
		eMT = pComp->GetMaterialType();

	DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );

	pBay = GetBayPointer();
	;//assert( pBay!=NULL );
	pLowerLift = pBay->GetLift( 0 );
	;//assert( pLowerLift!=NULL );
	;//assert( pLowerLift->GetRise()>=LIFT_RISE_0500MM );
	;//assert( pLowerLift->GetRise()<=LIFT_RISE_1500MM );

	iBoards = GetNumberOfPlanksForWidth( pBay->GetBayWidth() );
	for( i=0; i<iBoards-2; i++ )
	{
		AddComponent( CT_DECKING_PLANK, pBay->GetBayLength(), SOUTH, i, eMT );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//This will create a 'normal' ladder lift, with 2x1.2m planks
//	and a putlog!  If dLadderLength is a valid ladder length
//	then a ladder will be added to this lift
void Lift::CreateSpecialLadderLiftNormal(double dLadderLength)
{
	int					iBoards, i;
	double				dLength;
	MaterialTypeEnum	eMT = MT_STEEL;

	Component	*pComp = GetComponent( CT_DECKING_PLANK, LIFT_RISE_0000MM, SOUTH );
	if( pComp!=NULL )
		eMT = pComp->GetMaterialType();

	DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );

	//Find how many boards we need, the last two will be 1.2m boards
	//all boards, except the last two are normal bay lenght
	iBoards = GetNumberOfPlanksForWidth( GetBayPointer()->GetBayWidth() );
	dLength = GetBayPointer()->GetBayLength();
	iBoards-= 2;
	for( i=0; i<iBoards; i++ )
	{
		AddComponent( CT_DECKING_PLANK, dLength, SOUTH, i, eMT );
	}

	//The last two boards are 1.2m long
	iBoards+= 2;
	dLength = COMPONENT_LENGTH_1200;
	for( ; i<iBoards; i++ )
	{
		AddComponent( CT_DECKING_PLANK, dLength, SOUTH, i, eMT );
	}

	if( dLadderLength==COMPONENT_LENGTH_3600 ||
		dLadderLength==COMPONENT_LENGTH_4200 ||
		dLadderLength==COMPONENT_LENGTH_6000 )
	{
		AddComponent( CT_LADDER, dLadderLength, SOUTH, 0, MT_STEEL );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//This will create a 'normal' ladder lift, however, this time
//	the two short planks will be 0.8m long instead of 1.2m
void Lift::CreateSpecialLadderLiftShortPlank()
{
	int					iBoards, i;
	double				dLength;
	MaterialTypeEnum	eMT = MT_STEEL;

	Component *pComp = GetComponent( CT_DECKING_PLANK, LIFT_RISE_0000MM, SOUTH );
	if( pComp!=NULL )
		eMT = pComp->GetMaterialType();

	DeleteAllComponentsOfTypeFromSide( CT_DECKING_PLANK, SOUTH );

	//Find how many boards we need, the last two will be 1.2m boards
	//all boards, except the last two are normal bay lenght
	iBoards = GetNumberOfPlanksForWidth( GetBayPointer()->GetBayWidth() );
	dLength = GetBayPointer()->GetBayLength();
	iBoards-= 2;
	for( i=0; i<iBoards; i++ )
	{
		AddComponent( CT_DECKING_PLANK, dLength, SOUTH, i, eMT );
	}

	//The last two boards are 1.2m long
	iBoards+= 2;
	dLength = COMPONENT_LENGTH_0700;
	for( ; i<iBoards; i++ )
	{
		AddComponent( CT_DECKING_PLANK, dLength, SOUTH, i, eMT );
	}
}

bool Lift::IsThereACornerStageBoard(SideOfBayEnum eSide)
{
	int iRise;
	for( iRise=CNR_STAGE_BOARD_RISE_1X1; iRise<=CNR_STAGE_BOARD_RISE_3X2L; iRise++ )
	{
		if( GetComponent(CT_CORNER_STAGE_BOARD, iRise, eSide )!=NULL )
		{
			return true;
		}
	}
	return false;
}

void Lift::TraceContents( bool bTraceComponents ) const
{
#ifdef _DEBUG
	int			i, iSize;
	CString		sText;
	Component	*pComponent;

	acutPrintf( _T("\t\tLift %i\n"), GetLiftID()+1 );

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent( i );
		if( !GetBayPointer()->GetController()->IsComponentTypeHidden( pComponent->GetType() ) )
		{
			sText = GetComponentDescStrAlign( pComponent->GetType() );
			acutPrintf( _T("\t\t\t%s"), sText );
			sText = GetSideOfBayDescStrAlign( GetPosition( i ) );
			acutPrintf( _T(",\t%s"), sText );
			acutPrintf( _T(",\t%i(%i)"), (int)pComponent->GetLengthCommon(), (int)pComponent->GetLengthActual() );
			acutPrintf( _T(",\t%1.0fmm"), pComponent->GetRise()*STAR_SEPARATION_APPROX );
			acutPrintf( _T(",\t%s"), (pComponent->GetSystem()==S_MILLS)? SYSTEM_TEXT_MILLS: SYSTEM_TEXT_KWIKSTAGE );
			bTraceComponents;
			acutPrintf( _T(".\n") );
		}
	}
#endif	//#ifdef _DEBUG
}

void Lift::DeleteAllHandrailComponentsFromSide(SideOfBayEnum eSide)
{
	if( eSide==ALL_SIDES )
	{
		DeleteAllHandrailComponentsFromSide(NORTH);
		DeleteAllHandrailComponentsFromSide(EAST);
		DeleteAllHandrailComponentsFromSide(SOUTH);
		DeleteAllHandrailComponentsFromSide(WEST);
		return;
	}
	if( eSide==ALL_VISIBLE_SIDES )
	{
		if( GetBayPointer()->GetOuter()==NULL )
			DeleteAllHandrailComponentsFromSide(NORTH);
		if( GetBayPointer()->GetForward()==NULL )
			DeleteAllHandrailComponentsFromSide(EAST);
		if( GetBayPointer()->GetInner()==NULL )
			DeleteAllHandrailComponentsFromSide(SOUTH);
		if( GetBayPointer()->GetBackward()==NULL )
			DeleteAllHandrailComponentsFromSide(WEST);

		return;
	}
		
	DeleteAllComponentsOfTypeFromSide( CT_MESH_GUARD, eSide );
	DeleteAllComponentsOfTypeFromSide( CT_RAIL, eSide );
	DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
	if( eSide==NORTH || eSide==SOUTH )
		DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );

	SideOfBayEnum	eSideLeft, eSideRight;
	eSideLeft	= SIDE_INVALID;
	eSideRight	= SIDE_INVALID;
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
		;//assert( false );
	}
	Component	*pComp;
	pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideLeft );
	if( pComp!=NULL && eSideLeft!=SIDE_INVALID )
		DeleteComponent( pComp->GetID() );
	pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eSideRight );
	if( pComp!=NULL && eSideRight!=SIDE_INVALID )
		DeleteComponent( pComp->GetID() );
}

void Lift::AddHandrailToSide(SideOfBayEnum eSide, bool bAddMidRailAndToeBoard/*=true*/ )
{
	double	dLength;
	SideOfBayEnum	eHopupHandrailLeft, eHopupHandrailRight;

	eHopupHandrailLeft	= SIDE_INVALID;
	eHopupHandrailRight	= SIDE_INVALID;
	switch( eSide )
	{
	case( ALL_SIDES ):
		AddHandrailToSide(NORTH,	bAddMidRailAndToeBoard);
		AddHandrailToSide(EAST,		bAddMidRailAndToeBoard);
		AddHandrailToSide(SOUTH,	bAddMidRailAndToeBoard);
		AddHandrailToSide(WEST,		bAddMidRailAndToeBoard);
		return;

	case( ALL_VISIBLE_SIDES ):
		if( GetBayPointer()->GetOuter()==NULL )
			AddHandrailToSide(NORTH,	bAddMidRailAndToeBoard);
		if( GetBayPointer()->GetForward()==NULL )
			AddHandrailToSide(EAST,		bAddMidRailAndToeBoard);
		if( GetBayPointer()->GetInner()==NULL )
			AddHandrailToSide(SOUTH,	bAddMidRailAndToeBoard);
		if( GetBayPointer()->GetBackward()==NULL )
			AddHandrailToSide(WEST),	bAddMidRailAndToeBoard;
		return;

	case( NORTH ):
		eHopupHandrailLeft	= WNW;
		eHopupHandrailRight	= ENE;
		dLength = GetBayPointer()->GetBayLength();
		break;
	case( SOUTH ):
		eHopupHandrailLeft	= ESE;
		eHopupHandrailRight	= WSW;
		dLength = GetBayPointer()->GetBayLength();
		break;
	case( EAST ):
		eHopupHandrailLeft	= NNE;
		eHopupHandrailRight	= SSE;
		dLength = GetBayPointer()->GetBayWidth();
		break;
	case( WEST ):
		eHopupHandrailLeft	= SSW;
		eHopupHandrailRight	= NNW;
		dLength = GetBayPointer()->GetBayWidth();
		break;

	default:
		;//assert( false );
	}

	AddComponent( CT_RAIL, dLength, eSide, LIFT_RISE_1000MM, MT_STEEL );

	if( bAddMidRailAndToeBoard )
	{
		AddComponent( CT_MID_RAIL, dLength, eSide, LIFT_RISE_0500MM, MT_STEEL );

		//You can't have a toe board on the east or west, since this would
		//	push the planks off!
		if( eSide==NORTH || eSide==SOUTH )
			AddComponent( CT_TOE_BOARD, dLength, eSide, LIFT_RISE_0000MM, MT_STEEL );
	}

	Component	*pComp;
	int iBoards;
	iBoards = GetNumberOfPlanksOnStageBoard( eSide );
	if( iBoards>0 && eHopupHandrailLeft!=SIDE_INVALID )
	{
		//make sure we don't already have one!
		pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupHandrailLeft );
		if( pComp==NULL )
		{
			Add1000mmHopupIfReqd(eHopupHandrailLeft);
		}
	}
	if( iBoards>0 && eHopupHandrailRight!=SIDE_INVALID )
	{
		//make sure we don't already have one!
		pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupHandrailRight );
		if( pComp==NULL )
		{
			Add1000mmHopupIfReqd(eHopupHandrailRight);
		}
	}

	//Check the handrail
}

void Lift::AddMeshToSide(SideOfBayEnum eSide, bool bFull/*=false*/ )
{
	double	dLength;

	switch( eSide )
	{
	case( ALL_SIDES ):
		AddMeshToSide(NORTH,	bFull);
		AddMeshToSide(EAST,		bFull);
		AddMeshToSide(SOUTH,	bFull);
		AddMeshToSide(WEST,		bFull);
		return;

	case( ALL_VISIBLE_SIDES ):
		if( GetBayPointer()->GetOuter()==NULL )
			AddMeshToSide(NORTH,	bFull);
		if( GetBayPointer()->GetForward()==NULL )
			AddMeshToSide(EAST,	bFull);
		if( GetBayPointer()->GetInner()==NULL )
			AddMeshToSide(SOUTH,	bFull);
		if( GetBayPointer()->GetBackward()==NULL )
			AddMeshToSide(WEST,	bFull);
		return;

	case( NORTH ):	//fallthrough
	case( SOUTH ):
		dLength = GetBayPointer()->GetBayLength();
		break;

	case( EAST ):	//fallthrough
	case( WEST ):
		dLength = GetBayPointer()->GetBayWidth();
		break;

	default:
		;//assert( false );
	}

	DeleteAllComponentsOfTypeFromSide( CT_MID_RAIL, eSide );
	if( eSide==NORTH || eSide==SOUTH )
		DeleteAllComponentsOfTypeFromSide( CT_TOE_BOARD, eSide );

	if( GetRise()>=LIFT_RISE_1000MM )
		AddComponent( CT_MESH_GUARD, dLength, eSide, LIFT_RISE_1000MM, MT_STEEL );

	//if we are doing a full mesh, and this is not the top lift,
	//	then add another Mesh guard at 2000mm
	if( GetRise()>=LIFT_RISE_2000MM && bFull &&
		GetLiftID()<(GetBayPointer()->GetNumberOfLifts()-1) )
		AddComponent( CT_MESH_GUARD, dLength, eSide, LIFT_RISE_2000MM, MT_STEEL );

	//We also need a handrail
	AddHandrailToSide(eSide, false);
}

void Lift::AddFullMeshToSide(SideOfBayEnum eSide)
{
	AddMeshToSide( eSide, true );
}

bool Lift::CopyHandrailToSide(SideOfBayEnum eSideToCopyFrom, SideOfBayEnum eDestinationSide)
{
	//This neighboring lift doesn't have decking,
	//	or there is no neighbor lift we need a
	//	either way we need a handrail, if there
	//	is already one existing
	double			dLength;
	Component		*pComponent;

	switch( eDestinationSide )
	{
	case( N ):	//fallthrough
	case( S ):
		dLength = GetBayPointer()->GetBayLength();
		break;

	case( E ):	//fallthrough
	case( W ):
		dLength = GetBayPointer()->GetBayWidth();
		break;

	default:
		//invalid side
		;//assert( false );
	}

	if( HasComponentOfTypeOnSide( CT_RAIL, eSideToCopyFrom ) )
	{
		////////////////////////////////////////////////////////////////////////////////
		//Hand Rail
		pComponent = GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSideToCopyFrom );
		if( pComponent!=NULL )
			AddComponent( CT_RAIL, dLength, eDestinationSide, LIFT_RISE_1000MM, pComponent->GetMaterialType() );

		////////////////////////////////////////////////////////////////////////////////
		//Meshguard 1000
		pComponent = GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, eSideToCopyFrom );
		if( pComponent!=NULL )
		{
			AddComponent( CT_MESH_GUARD, dLength, eDestinationSide, LIFT_RISE_1000MM, pComponent->GetMaterialType() );

			////////////////////////////////////////////////////////////////////////////////
			//Meshguard 2000
			pComponent = GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eSideToCopyFrom );
			if( pComponent!=NULL )
				AddComponent( CT_MESH_GUARD, dLength, eDestinationSide, LIFT_RISE_2000MM, pComponent->GetMaterialType() );
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////////
			//Mid Rail
			pComponent = GetComponent( CT_MID_RAIL, LIFT_RISE_0500MM, eSideToCopyFrom );
			if( pComponent!=NULL )
			{
				AddComponent( CT_MID_RAIL, dLength, eDestinationSide, LIFT_RISE_0500MM, pComponent->GetMaterialType() );

				////////////////////////////////////////////////////////////////////////////////
				//Toe board
				if( eDestinationSide==NORTH || eDestinationSide==SOUTH )
				{
					AddComponent( CT_TOE_BOARD, dLength, eDestinationSide, LIFT_RISE_0000MM, MT_TIMBER );
				}
			}
		}

		return true;
	}
	return false;
}

int Lift::GetComponentRise(int iId) const
{
	return ComponentListArray.GetRise(iId);
}

SideOfBayEnum Lift::GetPosition(int iId) const
{
	return ComponentListArray.GetPosition( iId );
}

bool Lift::DeleteUnneeded1000mmHopup(Lift *pNeighbor, SideOfBayEnum eCompSide, SideOfBayEnum eStageSide)
{
	bool			bElementDeleted = false;
	SideOfBayEnum	eCorner;

	switch( eCompSide )
	{
	case( NNE ):	//fallthrough
	case( ENE ):
		eCorner = NE;
		break;
	case( ESE ):	//fallthrough
	case( SSE ):
		eCorner = SE;
		break;
	case( SSW ):	//fallthrough
	case( WSW ):
		eCorner = SW;
		break;
	case( WNW ):	//fallthrough
	case( NNW ):
		eCorner = NW;
		break;
	default:
		;//assert( false );
	}
	Component *pComp;
	pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eCompSide );
	if( pComp!=NULL )
	{
		//We have a 1.0m hopup bracket
		//We have a neighboring lift
		if( pNeighbor!=NULL )
		{
			if( eCompSide==SSE && pNeighbor->GetBayPointer()->IsSydneyCornerBay() )
			{
				//There is a sydney corner bay ahead so we don't want this sse hopup
				bElementDeleted|= DeleteComponent( pComp->GetID() );
			}
			else if( HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) &&
				pNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
			{
				//There is a stageboard on the neighboring lift plus
				//	we have a hopup so we don't need the 1.0m Hopup bracket anymore
				bElementDeleted|= DeleteComponent( pComp->GetID() );
			}
			else if( !HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) &&
				!pNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
			{
				//Nither of use have a hopup so we don't need a hopup bracket
				bElementDeleted|= DeleteComponent( pComp->GetID() );

				//some times we get a problem with cleaning up the 0mm hopups, so lets
				//	check them now
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= DeleteComponent( pComp->GetID() );
				switch( eCompSide )
				{
				case( NNE ): eCompSide = NNW; break;
				case( ENE ): eCompSide = ESE; break;
				case( ESE ): eCompSide = ENE; break;
				case( SSE ): eCompSide = SSW; break;
				case( SSW ): eCompSide = SSE; break;
				case( WSW ): eCompSide = WNW; break;
				case( WNW ): eCompSide = WSW; break;
				case( NNW ): eCompSide = NNE; break;
				}
				pComp = pNeighbor->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= pNeighbor->DeleteComponent( pComp->GetID() );
			}

		}	
		else
		{
			if( !HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
			{
				//we don't have a stage board, no point having a hopup bracket handrail
				bElementDeleted|= DeleteComponent( pComp->GetID() );

				//some times we get a problem with cleaning up the 0mm hopups, so lets
				//	check them now
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= DeleteComponent( pComp->GetID() );
			}
			else
			{
				//we have a stageboard, we could also have a corner filler
				if( HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, eCorner ) )
					bElementDeleted|= DeleteComponent( pComp->GetID() );
			}
		}
	}
	else
	{
		//some times we get a problem with cleaning up the 0mm hopups, so lets
		//	check them now
		if( pNeighbor!=NULL )
		{
			if( !HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) &&
				!pNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
			{
				//Nither of use have a hopup so we don't need a hopup bracket
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= DeleteComponent( pComp->GetID() );

				switch( eCompSide )
				{
				case( NNE ): eCompSide = NNW; break;
				case( ENE ): eCompSide = ESE; break;
				case( ESE ): eCompSide = ENE; break;
				case( SSE ): eCompSide = SSW; break;
				case( SSW ): eCompSide = SSE; break;
				case( WSW ): eCompSide = WNW; break;
				case( WNW ): eCompSide = WSW; break;
				case( NNW ): eCompSide = NNE; break;
				}
				pComp = pNeighbor->GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= pNeighbor->DeleteComponent( pComp->GetID() );
			}

		}	
		else
		{
			if( !HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
			{
				//we don't have a stage board, no point having a hopup bracket handrail
				pComp = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eCompSide );
				if( pComp!=NULL )
					bElementDeleted|= DeleteComponent( pComp->GetID() );
			}
		}
	}

	if( bElementDeleted && GetLiftID()==GetBayPointer()->GetNumberOfLifts()-1 )
	{
		//We just deleted a Hopup handrail on the top lift,
		//	better recreate the standards
		GetBayPointer()->AutoAdjustStandard(SideOfBayAsCorner(eCorner));
	}

	return bElementDeleted;
}

void Lift::Add1000mmHopupIfReqd(SideOfBayEnum eHopupSideToTest)
{
	Bay				*pBayNeighbor;
	Component		*pComp;
	SideOfBayEnum	eStageSide, eRailSide, eSideHopupNeighbor;
	CornerOfBayEnum	eStandardCnr;

	;//assert( GetBayPointer()!=NULL );
	switch( eHopupSideToTest )
	{
	case( NORTH_NORTH_EAST ):
		eStageSide			= NORTH;
		eRailSide			= EAST;
		eStandardCnr		= CNR_NORTH_EAST;
		pBayNeighbor		= GetBayPointer()->GetForward();
		eSideHopupNeighbor	= NNW;
		break;
	case( EAST_NORTH_EAST ):
		eStageSide			= EAST;
		eRailSide			= NORTH;
		eStandardCnr		= CNR_NORTH_EAST;
		pBayNeighbor		= GetBayPointer()->GetOuter();
		eSideHopupNeighbor	= ESE;
		break;
	case( EAST_SOUTH_EAST ):
		eStageSide			= EAST;
		eRailSide			= SOUTH;
		eStandardCnr		= CNR_SOUTH_EAST;
		pBayNeighbor		= GetBayPointer()->GetInner();
		eSideHopupNeighbor	= ENE;
		break;
	case( SOUTH_SOUTH_EAST ):
		eStageSide			= SOUTH;
		eRailSide			= EAST;
		eStandardCnr		= CNR_SOUTH_EAST;
		pBayNeighbor		= GetBayPointer()->GetForward();
		eSideHopupNeighbor	= SSW;
		break;
	case( SOUTH_SOUTH_WEST ):
		eStageSide			= SOUTH;
		eRailSide			= WEST;
		eStandardCnr		= CNR_SOUTH_WEST;
		pBayNeighbor		= GetBayPointer()->GetBackward();
		eSideHopupNeighbor	= SSE;
		break;
	case( WEST_SOUTH_WEST ):
		eStageSide			= WEST;
		eRailSide			= SOUTH;
		eStandardCnr		= CNR_SOUTH_WEST;
		pBayNeighbor		= GetBayPointer()->GetInner();
		eSideHopupNeighbor	= WNW;
		break;
	case( WEST_NORTH_WEST ):
		eStageSide			= WEST;
		eRailSide			= NORTH;
		eStandardCnr		= CNR_NORTH_WEST;
		pBayNeighbor		= GetBayPointer()->GetOuter();
		eSideHopupNeighbor	= WSW;
		break;
	case( NORTH_NORTH_WEST ):
		eStageSide			= NORTH;
		eRailSide			= WEST;
		eStandardCnr		= CNR_NORTH_WEST;
		pBayNeighbor		= GetBayPointer()->GetBackward();
		eSideHopupNeighbor	= NNE;
		break;
	}

	bool bAddBracket;

	bAddBracket = false;

	double dNumberOfPlanks = (double)GetNumberOfPlanksOnStageBoard( eStageSide );

	if( HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
	{
		if( HasComponentOfTypeOnSide( CT_RAIL, eRailSide ) )
		{
			if( !GetBayPointer()->GetTemplate()->GetTieTubeTemplates()->HasTubeOnStandard(eStandardCnr) )
			{
				if( (GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_1000MM, eHopupSideToTest )==NULL) )
				{
					//Check if there is a neighboring lift with a hopup
					if( pBayNeighbor!=NULL )
					{
						//we don't want a hopup if is on the ese and the neighbor is a sydney corner
						if( !(eHopupSideToTest==SSE && pBayNeighbor->IsSydneyCornerBay()) )
						{
							bAddBracket = true;

							Lift		*pLiftNeighbor;
							double		dRLLift, dRLNeighbor;

							dRLLift			= GetRL();
							pLiftNeighbor	= pBayNeighbor->GetLiftAtRL( dRLLift );
							if( pLiftNeighbor!=NULL )
							{
								dRLNeighbor		= pLiftNeighbor->GetRL();
								if( dRLNeighbor>dRLLift-ROUND_ERROR && dRLNeighbor<dRLLift+ROUND_ERROR )
								{
									//The neighboring lift is at the same height!
									if( pLiftNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
									{
										//The neighbor has a stageboard on the same side, so we don't need
										//	 the hopup bracket to block access!
										bAddBracket = false;
									}
								}
								else if( dRLNeighbor>dRLLift-GetStarSeparation()-ROUND_ERROR && dRLNeighbor<dRLLift-GetStarSeparation()+ROUND_ERROR )
								{
									//the neighboring lift is GetStarSeparation() down from this one, we don't need a bracket!
									if( pLiftNeighbor->IsDeckingLift() )
										bAddBracket = false;
								}
								else
								{
									//The neighboring lift may be GetStarSeparation() up from the deck!
									pLiftNeighbor	= pBayNeighbor->GetLiftAtRL( dRLLift+GetStarSeparation() );
									if( pLiftNeighbor!=NULL )
									{
										dRLNeighbor		= pLiftNeighbor->GetRL();
										if( dRLNeighbor>dRLLift+GetStarSeparation()-ROUND_ERROR && dRLNeighbor<dRLLift+GetStarSeparation()+ROUND_ERROR )
										{
											//The neighboring lift is GetStarSeparation() above this deck!
											if( pLiftNeighbor->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eSideHopupNeighbor ) )
											{
												//The neighbor has a stageboard on the same side, so we don't need
												//	 the hopup bracket to block access!
												bAddBracket = false;
											}
										}
									}
								}
							}

							//check if there is already a hopup at this height owned by the neighbor
							//	the hopup is 2 stars above the deck
							dRLLift+= (2.00*GetStarSeparation());
							if( pBayNeighbor->HasComponentAtRL( CT_HOPUP_BRACKET, dRLLift, eSideHopupNeighbor ) )
							{
								//The neighboring bay already has a hopup at that RL, so don't create another
								bAddBracket = false;
							}
						}
					}
					else
					{
						bAddBracket = true;
						if( HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, CornerAsSideOfBay(eStandardCnr) ) )
						{
							bAddBracket = false;
						}
					}
				}
			}
		}
		else
		{
			//Check if there is a neighboring lift with a hopup
			if( pBayNeighbor!=NULL )
			{
				if( !(eHopupSideToTest==SSE && pBayNeighbor->IsSydneyCornerBay()) )
				{
					Lift		*pLiftNeighbor;
					double		dRLLift, dRLNeighbor;

					dRLLift			= GetRL();
					pLiftNeighbor	= pBayNeighbor->GetLiftAtRL( dRLLift );
					if( pLiftNeighbor!=NULL )
					{
						dRLNeighbor		= pLiftNeighbor->GetRL();
						if( dRLNeighbor>dRLLift-ROUND_ERROR && dRLNeighbor<dRLLift+ROUND_ERROR )
						{
							//The neighboring lift is at the same height!
							if( !pLiftNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
							{
								//The neighboring lift is at the same height and does not have a
								//	hopup, so we need a hopup bracket as a handrail
								bAddBracket = true;
							}
						}
					}
				}
			}
			else
			{
				bAddBracket = true;
				if( HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, CornerAsSideOfBay(eStandardCnr) ) )
				{
					bAddBracket = false;
				}
			}
		}
	}
	else
	{
		//Check if there is a neighboring lift with a hopup
		if( pBayNeighbor!=NULL )
		{
			Lift		*pLiftNeighbor;
			double		dRLLift, dRLNeighbor;

			dRLLift			= GetRL();
			pLiftNeighbor	= pBayNeighbor->GetLiftAtRL( dRLLift );
			if( pLiftNeighbor!=NULL )
			{
				dRLNeighbor		= pLiftNeighbor->GetRL();
				if( dRLNeighbor>dRLLift-ROUND_ERROR && dRLNeighbor<dRLLift+ROUND_ERROR )
				{
					//The neighboring lift is at the same height!
					if( pLiftNeighbor->HasComponentOfTypeOnSide( CT_STAGE_BOARD, eStageSide ) )
					{
						if(	HasComponentOfTypeOnSide( CT_RAIL, eRailSide ) ||
							pLiftNeighbor->HasComponentOfTypeOnSide( CT_RAIL, GetOppositeSide( eRailSide ) ) ) 
						{
							//The neighbor has a stageboard on the same side, but we don't, so we
							//	need a hopup to block access
							bAddBracket = true;
							dNumberOfPlanks = (double)pLiftNeighbor->GetNumberOfPlanksOnStageBoard( eStageSide );
						}
					}
				}
			}
		}
		else
		{
			if( HasComponentOfTypeOnSide( CT_CORNER_STAGE_BOARD, CornerAsSideOfBay(eStandardCnr) ) )
			{
				bAddBracket = false;
			}
		}
	}

	if( bAddBracket )
	{
		//There is a stage board on the correct side, there is also a hand rail
		//	however we are missing both a tie tube and a 1.0m hopup, so we need
		//	to add a 1.0m hopup!
		//we have a stage board without a hopup bracket!
		pComp = AddComponent( CT_HOPUP_BRACKET, dNumberOfPlanks,
							eHopupSideToTest, LIFT_RISE_1000MM, MT_STEEL );

		//the height of the standards may now have to changed
		GetBayPointer()->AutoAdjustStandard(eStandardCnr);
	}
}

bool Lift::IsEmptyLift()
{
	return !IsDeckingLift();
}

bool Lift::IsDeckingLift()
{
	;//assert( GetBayPointer()!=NULL );
	if(GetBayPointer()==NULL)
		return false;
	if( GetBayPointer()->GetBayType()==BAY_TYPE_LAPBOARD )
		return HasComponentOfTypeOnSide( CT_LAPBOARD, SOUTH );
	else
		return HasComponentOfTypeOnSide( CT_DECKING_PLANK, SOUTH );
}

int Lift::HowManyEmptyLiftsAboveMe( double &dEmptyLiftHeight )
{
	int		i, iEmptyLiftCount, iSize;
	Bay		*pBay;
	Lift	*pLift;
	
	pBay = GetBayPointer();
	;//assert( pBay!=NULL );

	iEmptyLiftCount		= 0;
	dEmptyLiftHeight	= 0.00;

	iSize = pBay->GetNumberOfLifts();

	for( i=GetLiftID()+1; i<iSize; i++ )
	{
		pLift = pBay->GetLift(i);

		if( pLift->IsEmptyLift() )
		{
			iEmptyLiftCount++;
			dEmptyLiftHeight+= GetRiseFromRiseEnum( pLift->GetRise(), GetStarSeparation() );
		}
		else
		{
			break;
		}
	}
	return iEmptyLiftCount;
}

int Lift::HowManyDeckLiftsAboveMe()
{
	int		i, iDeckLiftCount, iSize;
	Bay		*pBay;
	Lift	*pLift;
	
	pBay = GetBayPointer();
	;//assert( pBay!=NULL );

	iDeckLiftCount = 0;
	iSize = pBay->GetNumberOfLifts();

	for( i=GetLiftID()+1; i<iSize; i++ )
	{
		pLift = pBay->GetLift(i);

		if( pLift->IsDeckingLift() )
			iDeckLiftCount++;
		else
			break;
	}
	return iDeckLiftCount;
}

///////////////////////////////////////////////////////////////////////
//
int Lift::GetLengthOfHopupBracket(SideOfBayEnum eSide) const
{
	Bay				*pBayNeighbor;
	Lift			*pLift;
	Component		*pComp;
	SideOfBayEnum	eSideNeighbor;

	;//assert( GetBayPointer()!=NULL );
	pBayNeighbor	= NULL;
	eSideNeighbor	= SIDE_INVALID;
	switch( eSide )
	{
	case( NNE ):	//fallthrough
	case( ENE ):	//fallthrough
	case( WNW ):	//fallthrough
	case( SSE ):
		break;
	case( ESE ):
		eSideNeighbor	= ENE;
		pBayNeighbor	= GetBayPointer()->GetInner();
		break;
	case( SSW ):
		eSideNeighbor	= SSE;
		pBayNeighbor	= GetBayPointer()->GetBackward();
		break;
	case( WSW ):
		eSideNeighbor	= WNW;
		pBayNeighbor	= GetBayPointer()->GetInner();
		break;
	case( NNW ):
		eSideNeighbor	= NNE;
		pBayNeighbor	= GetBayPointer()->GetBackward();
		break;
	default:
		;//assert( false );
		return 0;
	}

	pComp = GetComponent( CT_HOPUP_BRACKET, 0, eSide );
	if( pComp!=NULL )
	{
		return (int)pComp->GetLengthCommon();
	}
	else if( pBayNeighbor!=NULL )
	{
		double dRL;
		GetBayPointer()->GetLiftRL( GetLiftID(), dRL );
		pLift = pBayNeighbor->GetLiftAtRL( dRL );
		if( pLift!=NULL )
		{
			return pLift->GetNumberOfPlanksOnStageBoard( eSideNeighbor );
		}
	}
	return 0;
}

bool Lift::DeleteAllComponentsOfType(ComponentTypeEnum eType)
{
	int			i;
	bool		bRemoved;
	Component	*pComponent;

	bRemoved = false;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		if( (pComponent->GetType()==eType) )
		{
			DeleteComponent(i);
			i--;
			bRemoved = true;
		}
	}
	
	return bRemoved;
}

double Lift::GetRL()
{
	;//assert( GetBayPointer()!=NULL );
	return GetBayPointer()->GetRLOfLift( GetLiftID() );
}

CommittedProportionEnum Lift::GetCommittedProportion()
{
	int			i, iCommitted;
	Component	*pComp;

	iCommitted=0;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComp = GetComponent(i);
		;//assert( pComp!=NULL );

		if( pComp->IsCommitted() )
			iCommitted++;

		if( iCommitted>0 && iCommitted<i+1 )
		{
			//Some components are committed, but not all the components!
			return COMMIT_PARTIAL;
		}
	}

	if( iCommitted==GetNumberOfComponents() )
	{
		return COMMIT_FULL;
	}

	;//assert( iCommitted==0 );
	return COMMIT_NONE;
}

CommittedProportionEnum Lift::GetCommitProportionForComponentOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide)
{
	int			i, iCommitted, iUnCommitted;
	Component	*pComp;

	iCommitted		= 0;
	iUnCommitted	= 0;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComp = GetComponent(i);

		if( pComp->GetType()==eType &&
			pComp->GetSideOfBay()==eSide )
		{
			;//assert( pComp!=NULL );

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
		;//assert( iUnCommitted<=0 );
		return COMMIT_FULL;
	}

	return COMMIT_NONE;
}


bool Lift::DeleteAllComponentsOfTypeOnSideExceptRise(ComponentTypeEnum eType, SideOfBayEnum eSide, LiftRiseEnum eRise)
{
	int			iComp;
	bool		bFound;
	Component	*pComp;
	bFound = false;
	for( iComp=0; iComp<GetNumberOfComponents(); iComp++ )
	{
		pComp = GetComponent( iComp );
		if( pComp!=NULL &&
			pComp->GetType()==eType &&
			pComp->GetSideOfBay()==eSide &&
			pComp->GetRise()!=eRise )
		{
			DeleteComponent( pComp->GetID() );
			iComp--;
			bFound = true;
		}
	}
	return bFound;
}

void Lift::ResetComponentIDs()
{
	Component *pComponent;
	for( int i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent(i);
		pComponent->SetID(i);
	}
}

void Lift::ConvertToRoofProtection()
{

}

void Lift::UnconvertFromRoofProtection()
{

}

void Lift::CheckForHopupDoubleups(SideOfBayEnum eSide)
{
	double		dRL;
	Component	*pComponent, *pMyComponent;
	GetBayPointer()->GetLiftRL( GetLiftID(), dRL );

	pMyComponent = GetComponent( CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eSide );
	if( pMyComponent==NULL )
		return;

	switch( eSide )
	{
	case( NNE ):
		if( GetBayPointer()->GetForward()!=NULL )
		{
			pComponent = GetBayPointer()->GetForward()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, NNW );
			if( pComponent!=NULL )
			{
				GetBayPointer()->GetForward()->GetLiftAtRL( dRL )->DeleteComponent( pComponent->GetID() );
			}
		}
		break;
	case( ENE ):
		if( GetBayPointer()->GetOuter()!=NULL )
		{
			pComponent = GetBayPointer()->GetOuter()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, ESE );
			if( pComponent!=NULL )
			{
				GetBayPointer()->GetOuter()->GetLiftAtRL( dRL )->DeleteComponent( pComponent->GetID() );
			}
		}
		break;
	case( ESE ):
		if( GetBayPointer()->GetInner()!=NULL )
		{
			pComponent = GetBayPointer()->GetInner()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, ENE );
			if( pComponent!=NULL )
			{
				DeleteComponent( pMyComponent->GetID() );
			}
		}
		break;
	case( SSE ):
		if( GetBayPointer()->GetForward()!=NULL )
		{
			pComponent = GetBayPointer()->GetForward()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, SSW );
			if( pComponent!=NULL )
			{
				GetBayPointer()->GetForward()->GetLiftAtRL( dRL )->DeleteComponent( pComponent->GetID() );
			}
		}
		break;
	case( SSW ):
		if( GetBayPointer()->GetBackward()!=NULL )
		{
			pComponent = GetBayPointer()->GetBackward()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, SSE );
			if( pComponent!=NULL )
			{
				DeleteComponent( pMyComponent->GetID() );
			}
		}
		break;
	case( WSW ):
		if( GetBayPointer()->GetInner()!=NULL )
		{
			pComponent = GetBayPointer()->GetInner()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, WNW );
			if( pComponent!=NULL )
			{
				DeleteComponent( pMyComponent->GetID() );
			}
		}
		break;
	case( WNW ):
		if( GetBayPointer()->GetOuter()!=NULL )
		{
			pComponent = GetBayPointer()->GetOuter()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, WSW );
			if( pComponent!=NULL )
			{
				GetBayPointer()->GetOuter()->GetLiftAtRL( dRL )->DeleteComponent( pComponent->GetID() );
			}
		}
		break;
	case( NNW ):
		if( GetBayPointer()->GetBackward()!=NULL )
		{
			pComponent = GetBayPointer()->GetBackward()->GetComponentAtRL( CT_HOPUP_BRACKET, dRL, NNE );
			if( pComponent!=NULL )
			{
				DeleteComponent( pMyComponent->GetID() );
			}
		}
		break;
	default:
		;//assert( false );
	}
}

bool Lift::HasMatchingHopupBracket( ComponentTypeEnum eComponentType, int iRise, SideOfBayEnum eSideOfBay )
{
	bool bFound=false;
	if( eComponentType==CT_HOPUP_BRACKET )
	{
		double dRL;
		GetBayPointer()->GetLiftRL( GetLiftID(), dRL );
		dRL+= GetRiseFromRiseEnum((LiftRiseEnum)iRise, GetStarSeparation());
		switch( eSideOfBay )
		{
		case( NNE ):
			if( GetBayPointer()->GetForward()!=NULL )
			{
				bFound = GetBayPointer()->GetForward()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, NNW );
			}
			break;
		case( ENE ):
			if( GetBayPointer()->GetOuter()!=NULL )
			{
				bFound = GetBayPointer()->GetOuter()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, ESE );
			}
			break;
		case( ESE ):
			if( GetBayPointer()->GetInner()!=NULL )
			{
				bFound = GetBayPointer()->GetInner()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, ENE );
			}
			break;
		case( SSE ):
			if( GetBayPointer()->GetForward()!=NULL )
			{
				bFound = GetBayPointer()->GetForward()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, SSW );
			}
			break;
		case( SSW ):
			if( GetBayPointer()->GetBackward()!=NULL )
			{
				bFound = GetBayPointer()->GetBackward()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, SSE );
			}
			break;
		case( WSW ):
			if( GetBayPointer()->GetInner()!=NULL )
			{
				bFound = GetBayPointer()->GetInner()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, WNW );
			}
			break;
		case( WNW ):
			if( GetBayPointer()->GetOuter()!=NULL )
			{
				bFound = GetBayPointer()->GetOuter()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, WSW );
			}
			break;
		case( NNW ):
			if( GetBayPointer()->GetBackward()!=NULL )
			{
				bFound = GetBayPointer()->GetBackward()->HasComponentAtRL( CT_HOPUP_BRACKET, dRL, NNE );
			}
			break;
		default:
			;//assert( false );
		}
	}
	return bFound;
}

bool Lift::HasMatchingTransomLedgerHandrailOrMidrail( ComponentTypeEnum eType, int iRise, SideOfBayEnum eSide )
{
	bool bFound=false;

	if( eType==CT_LEDGER || eType==CT_TRANSOM ||
		eType==CT_RAIL || eType==CT_MID_RAIL )
	{
		double dRL;
		GetBayPointer()->GetLiftRL( GetLiftID(), dRL );
		dRL+= GetRiseFromRiseEnum((LiftRiseEnum)iRise, GetStarSeparation());
		switch( eSide )
		{
		case( NORTH ):
			if( GetBayPointer()->GetOuter()!=NULL )
			{
				bFound|= GetBayPointer()->GetOuter()->HasComponentAtRL( CT_LEDGER,		dRL, SOUTH );
				bFound|= GetBayPointer()->GetOuter()->HasComponentAtRL( CT_RAIL,		dRL, SOUTH );
				bFound|= GetBayPointer()->GetOuter()->HasComponentAtRL( CT_MID_RAIL,	dRL, SOUTH );
			}
			break;
		case( EAST ):
			if( GetBayPointer()->GetForward()!=NULL )
			{
				bFound|= GetBayPointer()->GetForward()->HasComponentAtRL( CT_TRANSOM,	dRL, WEST );
				bFound|= GetBayPointer()->GetForward()->HasComponentAtRL( CT_RAIL,		dRL, WEST );
				bFound|= GetBayPointer()->GetForward()->HasComponentAtRL( CT_MID_RAIL,	dRL, WEST );
			}
			break;
		case( SOUTH ):
			if( GetBayPointer()->GetInner()!=NULL )
			{
				bFound|= GetBayPointer()->GetInner()->HasComponentAtRL( CT_LEDGER,		dRL, NORTH );
				bFound|= GetBayPointer()->GetInner()->HasComponentAtRL( CT_RAIL,		dRL, NORTH );
				bFound|= GetBayPointer()->GetInner()->HasComponentAtRL( CT_MID_RAIL,	dRL, NORTH );
			}
			break;
		case( WEST ):
			if( GetBayPointer()->GetBackward()!=NULL )
			{
				bFound|= GetBayPointer()->GetBackward()->HasComponentAtRL( CT_TRANSOM,	dRL, EAST );
				bFound|= GetBayPointer()->GetBackward()->HasComponentAtRL( CT_RAIL,		dRL, EAST );
				bFound|= GetBayPointer()->GetBackward()->HasComponentAtRL( CT_MID_RAIL,	dRL, EAST );
			}
			break;
		default:
			;//assert( false );
		}
	}
	return bFound;
}

double Lift::ChangeLengthAccordingToMillsCorner(ComponentTypeEnum eType, SideOfBayEnum eSide, double dLength)
{
	//we are only interested in 2400 long components
	if( dLength==COMPONENT_LENGTH_2400 )
	{
		//if we are talking about the north and this is a northern mills bay,
		//or
		//if we are talking about the south and this is a southern mills bay
		MillsType mt = GetBayPointer()->GetMillsSystemType();
		if( ( eSide==NORTH	&& ( mt&MILLS_TYPE_CONNECT_NNE || mt&MILLS_TYPE_CONNECT_NNW ) ) ||
			( eSide==SOUTH	&& ( mt&MILLS_TYPE_CONNECT_SSE || mt&MILLS_TYPE_CONNECT_SSW ) ) ||
			( eSide==EAST	&& ( mt&MILLS_TYPE_CONNECT_ENE || mt&MILLS_TYPE_CONNECT_ESE ) ) ||
			( eSide==WEST	&& ( mt&MILLS_TYPE_CONNECT_WSW || mt&MILLS_TYPE_CONNECT_WNW ) ) )
		{
			//Any of these types should be shortened
			if( eType==CT_TRANSOM		|| eType==CT_LEDGER		||
				eType==CT_MESH_GUARD	|| eType==CT_RAIL		||
			    eType==CT_MID_RAIL		|| eType==CT_TOE_BOARD	)
			{
				dLength = COMPONENT_LENGTH_1200;
			}
		}
	}
	return dLength;
}

HandrailStyleEnum Lift::GetHandrailStyle(SideOfBayEnum eSide)
{
	HandrailStyleEnum	eHRS;

	eHRS = HRS_NONE;

	//We want to find the highest level of handrails they use,
	//	full mesh being the highest, rail the lowest!
	if( eHRS<HRS_FULL_MESH_TO_GROUND )
	{
		//This bay may have full or half mesh lets check
		if( eHRS<HRS_RAIL_ONLY && GetComponent( CT_RAIL, LIFT_RISE_1000MM, eSide )!=NULL )
			eHRS = HRS_RAIL_ONLY;
		//This bay may have full or half mesh lets check
		if( eHRS<HRS_HALF_MESH && GetComponent( CT_MESH_GUARD, LIFT_RISE_1000MM, eSide )!=NULL )
			eHRS = HRS_HALF_MESH;
		if( eHRS<HRS_FULL_MESH_TO_GROUND && GetComponent( CT_MESH_GUARD, LIFT_RISE_2000MM, eSide )!=NULL )
		{
			if( eHRS<HRS_FULL_MESH )
				eHRS = HRS_FULL_MESH;
			if( IsEmptyLift() )
				eHRS = HRS_FULL_MESH_TO_GROUND;
		}
	}

	return eHRS;
}

void Lift::SetAllComponentsVisible()
{
	int			i;
	Component	*pComponent;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		;//assert( pComponent!=NULL );
		pComponent->SetVisible(VIS_VISIBLE);
	}
}

double Lift::GetStarSeparation() const
{
	;//assert( GetBayPointer()!=NULL );
	return GetBayPointer()->GetStarSeparation();
}

SystemEnum Lift::GetSystem()
{
	;//assert( GetBayPointer()!=NULL );
	return GetBayPointer()->GetSystem();
}
