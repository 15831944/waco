// Component.cpp: implementation of the Component class.
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
#include "StdAfx.h"
#include "Component.h"
#include "Lift.h"
#include "Bay.h"
#include "Run.h"
#include "Controller.h"
#include <math.h>
#include <dbents.h>
#include "Entity.h"
#include "MatrixElement.h"
#include "LevelList.h"
#include "StockListElement.h"
#include "VisualComponents.h"
#include <geassign.h>
#include "Kwik3DPolygonMesh.h"

ACRX_DXF_DEFINE_MEMBERS(Component,AcDbObject,
AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
0,COMPONENT,KWIKSCAF);

extern int	giaLoopCounter[10];
extern CString	gsaLoopCounter[10];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Default Constructor
//
Component::Component()
{
	m_iSettingToDefault = 0;
	SetVariablesToDefault();

#ifdef _DEBUG
	assert( gpController!=NULL );
	gpController->ComponentDebuggerAdd( this );
#endif	//#ifdef _DEBUG
}

//////////////////////////////////////////////////////////////////////
// Constructor
//
Component::Component( double dLength, ComponentTypeEnum eComponentType, MaterialTypeEnum eMaterialType, SystemEnum eSystem ) 
{
	m_iSettingToDefault = 0;
	SetEntity( NULL );

	SetVariablesToDefault();

	//don't allow the update for the stock list element yet
	m_iSettingToDefault++;
	SetSystem( eSystem );
	SetType( eComponentType );
	SetLengthCommon( dLength );
	SetPreviousLength( dLength );
	SetMaterialType( eMaterialType );

	m_iSettingToDefault--;
	//lets update it manually
	SetStockDetailsPointer();

	DeletePoints();

	SetVisible();

	SetTransform( GetTransform().setToIdentity() );
	SetVisualTransformUsed(false);
	SetVisualTransform( GetVisualTransform().setToIdentity() );

	SetMoveWhenBayLengthChanges( false );
	SetMoveWhenBayWidthChanges( false );

	SetDrawMatrixElementSideOn( true );

	SetDirtyFlag( DF_CLEAN );

#ifdef _DEBUG
	assert( gpController!=NULL );
	gpController->ComponentDebuggerAdd( this );
#endif	//#ifdef _DEBUG
}

void Component::SetVariablesToDefault()
{
	m_iSettingToDefault++;
	m_iKwikScafComponent			= KWIKSCAF_COMPONENT_ID;
	SetAllPointerToNULL();
	m_bMoveWhenBayWidthChanges		= false;
	m_bMoveWhenBayLengthChanges		= false;
	SetPreviousLength( 0.00 );
	m_dLength						= 0.00;
	m_eComponentType				= CT_DECKING_PLANK;
	m_eMaterialType					= MT_OTHER;
	m_eVisible						= VIS_INVALID;
	m_pLift							= NULL;
	m_pBay							= NULL;
	m_pElement						= NULL;
	SetController( gpController );
	m_sStage						= STAGE_DEFAULT_VALUE;
	m_iLevel						= LEVEL_DEFAULT_VALUE;
	m_dfDirtyFlag					= DF_CLEAN;
	m_iID							= ID_INVALID;
	SetEntity( NULL );
	SetReactor( NULL );
	//m_ptPoints3D.RemoveAll();
	//m_Nodes.RemoveAll();
	m_Transform.setToIdentity();
	m_sText.Empty();
	SetTextHorizontalMode(AcDb::kTextCenter);
	SetColour( COLOUR_UNDEFINED );
	SetTextVerticalMode( AcDb::kTextVertMid );
	SetTextHeightMultiplier( 1.00 );
	SetCommitted(false);
	SetRemoveComponentFromBOM( false );
	SetStockDetailsPointer();
	m_iSettingToDefault--;
}

//////////////////////////////////////////////////////////////////////
// Destructor
//
Component::~Component()
{
#ifdef _DEBUG
	if( GetController()!=NULL )
		GetController()->ComponentDebuggerRemove(this);
#endif	//#ifdef _DEBUG
	DeleteEntity();
}



//////////////////////////////////////////////////////////////////////
// Set component attribute functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	SetID
//
//	Sets the ID of the component(determined by the index of the component in
//	the lift list[only if the component is in the lift list]).
void Component::SetID( int iID )
{
	m_iID = iID;
}

//////////////////////////////////////////////////////////////////////
//GetID
//Gets the ID of the component(determined by the index of the component in
//	the lift list[only if the component is in the lift list]).
int Component::GetID() const
{
	return m_iID;
}

//////////////////////////////////////////////////////////////////////
//GetIDs
//retrieves all the iID which are relevent for this component
bool Component::GetIDs(int &iComponentID, int &iLiftID, int &iBayID, int &iRunID) const
{
	if( GetLift()!=NULL )
	{
		iLiftID = GetLift()->GetLiftID();
	}
	else if( GetBay()!=NULL )
	{
		iLiftID = ID_INVALID;
	}
	else
	{
		iLiftID	= ID_INVALID;
		iBayID	= ID_INVALID;
		iRunID	= ID_INVALID;
		return false;
	}
	iComponentID	= GetID();
	iBayID			= GetBay()->GetID();
	iRunID			= GetRun()->GetRunID();
	return true;
}

//////////////////////////////////////////////////////////////////////
//SetType
//Sets the type the component.
void Component::SetType( ComponentTypeEnum eComponentType )
{
	if( m_eComponentType!=eComponentType )
		SetStockDetailsPointer();
	m_eComponentType = eComponentType;
}

//////////////////////////////////////////////////////////////////////
//	GetType
//	Returns the component type, ie transom, ledger etc.
ComponentTypeEnum Component::GetType() const
{
	return m_eComponentType;	
}

//////////////////////////////////////////////////////////////////////
//	SetComponent
//	Sets the length, width, type and material of construction characteristics of 
//	the component.
void Component::SetComponent( double dLength, ComponentTypeEnum eComponentType, MaterialTypeEnum eMaterialType )
{
	m_iSettingToDefault++;
	SetLengthCommon( dLength );
	SetType( eComponentType );
	SetMaterialType( eMaterialType );
	m_iSettingToDefault--;
	SetStockDetailsPointer();
}

//////////////////////////////////////////////////////////////////////
//GetWeightActual
//
double Component::GetWeightActual(bool bShowWarning) const
{
	if( !IsAPointerSet() )
	{
		assert( false );
		return CDT_UNDEFINED;
	}

	return GetStockDetails()->GetWeight();
}

//////////////////////////////////////////////////////////////////////
//SetMaterialType
//
void Component::SetMaterialType( MaterialTypeEnum eMaterialType )
{
	if( m_eMaterialType!=eMaterialType )
		SetStockDetailsPointer();
	m_eMaterialType = eMaterialType;
}

//////////////////////////////////////////////////////////////////////
//GetMaterialType
//
MaterialTypeEnum Component::GetMaterialType() const
{
	return m_eMaterialType;
}

//////////////////////////////////////////////////////////////////////
//	SetLevel
//	Sets the level of a component.
void Component::SetLevel( int iLevel)
{
	m_iLevel = iLevel;
}

//////////////////////////////////////////////////////////////////////
//	SetStage
//	Sets the stage of a component.
void Component::SetStage(CString sStage)
{
	m_sStage = sStage;
}

//////////////////////////////////////////////////////////////////////
//	SetLengthCommon
//	Sets the length of the component.
void Component::SetLengthCommon(double dLength)
{
	SetPreviousLength( m_dLength );
	m_dLength = dLength;

	if( GetPreviousLength()<dLength-ROUND_ERROR ||
		GetPreviousLength()>dLength+ROUND_ERROR )
			SetStockDetailsPointer();
}

//////////////////////////////////////////////////////////////////////
//GetWidthActual
//Gets the bounding box for the component
double Component::GetWidthActual(bool bShowWarning/*=fasle*/) const
{
	if( !IsAPointerSet() )
	{
		assert( false );
		return CDT_UNDEFINED;
	}

	return GetStockDetails()->GetWidth();
}

//////////////////////////////////////////////////////////////////////
//GetHeightActual
//Get the bounding box for the component
double Component::GetHeightActual(bool bShowWarning/*=false*/) const
{
	if( !IsAPointerSet() )
	{
		assert( false );
		return CDT_UNDEFINED;
	}

	return GetStockDetails()->GetHeight();
}

//////////////////////////////////////////////////////////////////////
//	Copy
//Creates a new component, moves it to a new location, and returns a pointer to it
bool Component::Copy(Matrix3D TransformationMatrix, bool bMove) const
{
	bMove;
//	Move( TransformationMatrix );
	return 0;
}


//////////////////////////////////////////////////////////////////////
//Move
//Moves the Entity pointer, and stores the movement in the Transform matrix
bool Component::Move(Matrix3D TransformationMatrix, bool bStoreMovement/*=true*/, bool bMoveEntity/*=true*/ )
{
	Acad::ErrorStatus	es;

	if( GetDirtyFlag()!=DF_CLEAN )
		return false;

	//move Entity
	if( bMoveEntity )
	{
		assert( GetController()!=NULL );
		GetController()->IgnorMovement(true);
		if( GetEntity()!=NULL )
		{
			es = GetEntity()->open();
			assert( es==Acad::eOk );

			assert( GetEntity()->isWriteEnabled() );
			es = GetEntity()->transformBy( TransformationMatrix );
			//assert( es==Acad::eOk );

			es = GetEntity()->close();
			assert( es==Acad::eOk );
		}
		GetController()->IgnorMovement(false);
	}
//	ResetXDataPositions();
	StoreLastPosition();


	//store it
	if( bStoreMovement )
	{
		SetTransform ( TransformationMatrix*GetTransform() );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//VisualMove
//Moves a component, it is the last movement done to a component, in that it is
//	applied after all the other movements.
bool Component::VisualMove(Matrix3D VisualTransform, bool bStoreMovement/*=true*/, bool bMoveEntity/*=true*/)
{
	Acad::ErrorStatus	es;

	if( GetDirtyFlag()!=DF_CLEAN )
		return false;

	//move Entity
	if( bMoveEntity )
	{
		assert( GetController()!=NULL );
		GetController()->IgnorMovement(true);
		if( GetEntity()!=NULL )
		{
			es = GetEntity()->open();
			assert( es==Acad::eOk );

			assert( GetEntity()->isWriteEnabled() );
			es = GetEntity()->transformBy( VisualTransform );
			//assert( es==Acad::eOk );

			es = GetEntity()->close();
			assert( es==Acad::eOk );
		}
		GetController()->IgnorMovement(false);
	}
//	ResetXDataPositions();
	StoreLastPosition();

	//store it
	if( bStoreMovement )
	{
		SetVisualTransformUsed(true);
		SetVisualTransform( VisualTransform*GetVisualTransform() );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// Attribute access functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	GetLengthCommon
//	Returns the length of the component.
double Component::GetLengthCommon() const
{
	return m_dLength;
}


//////////////////////////////////////////////////////////////////////
//GetLengthActual
//Returns the actual length of the component, includes at mills or quickscaff.
double Component::GetLengthActual( bool bShowWarning/*=false*/ ) const
{
	return FindLengthActual( GetLengthCommon(), bShowWarning );
}

//////////////////////////////////////////////////////////////////////
//FindLengthActual
//Returns the actual length, given a length, using this component's
//	type and material, includes at mills or quickscaff.
double Component::FindLengthActual( double dLength, bool bShowWarning=false ) const
{
	if( !IsAPointerSet() )
	{
		assert( false );
		return CDT_UNDEFINED;
	}
	assert( GetStockDetails()!=NULL );

	return GetStockDetails()->GetLength();
}

//////////////////////////////////////////////////////////////////////
//	GetLevel
//	Returns the level of component.
int Component::GetLevel() const
{
	int				iLvl;
	Bay				*pB;
	Lift			*pLft;
	bool			bHandrail;
	double			dRL, dB, dC, dStdRL, dRLofLift, dStandardToLift;
	Component		*pCmpnt, *pComp;
	CornerOfBayEnum	eCnrStandard;

	//if m_iLevel has not be assigned to this component then calculate it
	if( m_iLevel==LEVEL_DEFAULT_VALUE )
	{
		if( GetVisualComponentsPointer()!=NULL)
		{
			return 1;
		}

		//Since Items belonging to the same lift should all belong
		//	to the same level, (except 2.0m Mesh Guard) we should find
		//	the level of a component that is at the bottom of this lift
		//	such as the ledger!
		switch( GetType() )
		{
		case( CT_TIE_TUBE ):
		case( CT_TIE_CLAMP_COLUMN ):
		case( CT_TIE_CLAMP_MASONARY ):
		case( CT_TIE_CLAMP_YOKE ):
		case( CT_TIE_CLAMP_90DEGREE ):
			//These components should be attached to a standards,
			//	so find what standard it is attached to, and
			//	return the RL for that standard
			break;

		////////////////////////////////////////////////////////////
		//These should all be on the bottom of a lift
		case( CT_LEDGER ):
		case( CT_TRANSOM ):
			if( GetLiftPointer()!=NULL &&
				GetLiftPointer()->GetBayPointer()!=NULL &&
				(	GetLiftPointer()->GetBayPointer()->GetBayType()==BAY_TYPE_STAIRS ||
					GetLiftPointer()->GetBayPointer()->GetBayType()==BAY_TYPE_LADDER ||
					GetLiftPointer()->GetBayPointer()->GetBayType()==BAY_TYPE_BUTTRESS ) )
			{
				iLvl = GetLevelForRL( GetRL()+GetLiftPointer()->GetBayPointer()->RLAdjust()
						- GetRiseFromRiseEnum( LIFT_RISE_1000MM, GetStarSeparation() ) );
				return iLvl;
			}
			break;

		case( CT_TOE_BOARD ):
		case( CT_LADDER_PUTLOG ):
			assert( GetRise()==LIFT_RISE_0000MM );
			break;

		////////////////////////////////////////////////////////////
		//These all belong to the same lift and should not be split by RL line
		case( CT_MESH_GUARD ):
			if( GetType()==CT_MESH_GUARD && GetRise()==LIFT_RISE_2000MM )
			{
				//This is a 2.0m mesh guard so we had better just
				//calculate the level from the RL value!
				break;
			}
			//fallthrough
		case( CT_HOPUP_BRACKET ):
			//Hopup brackets less than or equal to 1000mm should be included
			//	with any decking
			if( GetType()==CT_HOPUP_BRACKET && GetRise()>LIFT_RISE_1000MM )
			{
				//We need to exclude any hopup brackets that are above 1000m, otherwise include
				break;
			}
			//fallthrough
		case( CT_RAIL ):
		case( CT_MID_RAIL ):

			pLft = GetLift();
			if( pLft!=NULL )
			{
				if( pLft->GetBayPointer()->GetBayType()==BAY_TYPE_LAPBOARD )
				{
					//Lapboards don't have ledgers!
					pCmpnt = pLft->GetComponent( CT_LAPBOARD, LIFT_RISE_0000MM, SOUTH );
				}
				else
				{
					//We need to use the northern one here since the southern one
					//	may be owned by a different bay!
					pCmpnt = pLft->GetComponent( CT_LEDGER, LIFT_RISE_0000MM, NORTH );
				}

				if( pCmpnt!=NULL )
				{
					return pCmpnt->GetLevel();
				}
				else
				{
					assert( false /*We have a rail without a ledger or lapboard*/ );
					iLvl = GetLevelForRL( GetRL()+pLft->GetBayPointer()->RLAdjust() );
					return iLvl;
				}
			}
			else
			{
				assert( GetBayPointer()!=NULL );
				iLvl = GetLevelForRL( GetRL()+GetBayPointer()->RLAdjust() );
				return iLvl;
			}
			break;

		////////////////////////////////////////////////////////////
		//These components should not be be asking for a calculated level
		case( CT_UNDEFINED ):
		case( CT_STANDARD_CONNECTOR ):
		default:
			assert( false /*Wrong Type to be asking for a level*/);
			break;

		////////////////////////////////////////////////////////////
		//Standards belong to a level
		case( CT_STANDARD ):
		case( CT_STANDARD_OPENEND ):
			pB = GetBay();
			dStdRL = GetRL();
			if( pB!=NULL )
			{
				pLft = pB->GetLiftAtRL( dStdRL );
				dStdRL+= pB->RLAdjust()-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
				if( pLft!=NULL && pLft->IsDeckingLift() )
				{
					//how far are we above this deck?
					dRLofLift = pLft->GetRL()+pB->RLAdjust();
					dStandardToLift = dStdRL-dRLofLift;
					if( dStandardToLift>0.00-ROUND_ERROR &&
						dStandardToLift<(2.00*GetStarSeparation())-ROUND_ERROR )
					{
						//This is within 2*GetStarSeparation() of the deck, if there is a handrail
						//	that needs this standard for the lift then the RL for this
						//	standard is tied to the RL for that lift!
						bHandrail = false;
						switch( GetSideOfBay() )
						{
						case( NE ):
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, N );
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, E )|bHandrail;
							break;
						case( SE ):
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, S );
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, E )|bHandrail;
							break;
						case( SW ):
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, S );
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, W )|bHandrail;
							break;
						case( NW ):
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, N );
							bHandrail = pLft->HasComponentOfTypeOnSide( CT_RAIL, W )|bHandrail;
							break;
						default:
							assert( false );
						}

						if( bHandrail )
						{
							//Need to find the RL for this component
							// Then determine which Level it is within!
							return GetLevelForRL( dRLofLift );
						}
					}
				}
			}
			else
			{
				if( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL )
					dStdRL+= GetLiftPointer()->GetBayPointer()->RLAdjust()+EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
				else
					dStdRL+= RL_ADJUST+EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
			}
			return GetLevelForRL( dStdRL );
			break;

		////////////////////////////////////////////////////////////
		//Stair and bracing levels should be calculated from the top
		//	of the component not the bottom
		case( CT_BRACING ):
			assert( GetBayPointer()!=NULL );
			eCnrStandard = CNR_INVALID;

			//How high is this brace
			switch( GetSideOfBay() )
			{
			case( N ):
				dB = GetBayPointer()->GetBayLengthActual();
				if( GetRise()%2==0 )
					eCnrStandard = CNR_NORTH_WEST;
				else
					eCnrStandard = CNR_NORTH_EAST;
				break;
			case( S ):
				dB = GetBayPointer()->GetBayLengthActual();
				if( GetRise()%2==0 )
					eCnrStandard = CNR_SOUTH_EAST;
				else
					eCnrStandard = CNR_SOUTH_WEST;
				break;
			case( E ):
				dB = GetBayPointer()->GetBayWidthActual();
				if( GetRise()%2==0 )
					eCnrStandard = CNR_NORTH_EAST;
				else
					eCnrStandard = CNR_SOUTH_EAST;
				break;
			case( W ):
				dB = GetBayPointer()->GetBayWidthActual();
				if( GetRise()%2==0 )
					eCnrStandard = CNR_SOUTH_WEST;
				else
					eCnrStandard = CNR_NORTH_WEST;
				break;
			default:
				assert( false );
				break;
			}
			dC = GetLengthActual();
			dC*= dC;
			dB*= dB;
			//since
			//C^2 = A^2 + B^2
			//thus
			//A = sqt( C^2 - B^2 )
			dRL = sqrt( dC-dB );
			//Round down to the nearest star
			dRL = double(int(dRL/GetStarSeparation())*GetStarSeparation());
			dRL = GetRL()+dRL;//+GetLiftPointer()->GetBayPointer()->RLAdjust();
			pComp = GetBayPointer()->GetStandardAtRL( dRL, eCnrStandard );
			if( pComp!=NULL )
				return pComp->GetLevel();
			else
				return GetLevelForRL( GetRL()+GetBayPointer()->RLAdjust()+dRL );
		case( CT_STAIR ):
			//Stair always go up 3 stars
			assert( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL );
			if( GetRise()==SPR_OUTER )
				iLvl = GetLevelForRL( GetRL()+GetLiftPointer()->GetBayPointer()->RLAdjust()+(3*GetStarSeparation()) );
			else
				iLvl = GetLevelForRL( GetRL()+GetLiftPointer()->GetBayPointer()->RLAdjust()+(6*GetStarSeparation()) );
			return iLvl;

		////////////////////////////////////////////////////////////
		//Just calculate the level!
		case( CT_STAIR_RAIL ):
		case( CT_STAIR_RAIL_STOPEND ):
		case( CT_TOE_BOARD_CLIP ):
		case( CT_PUTLOG_CLIP ):
			assert( GetVisualComponentsPointer()!=NULL );
			break;

		////////////////////////////////////////////////////////////
		//Just calculate the level!
		case( CT_DECKING_PLANK ):
		case( CT_LAPBOARD ):
		case( CT_STAGE_BOARD ):
		case( CT_LADDER ):
		case( CT_JACK ):
		case( CT_SOLEBOARD ):
		case( CT_TIE_BAR ):
		case( CT_TEXT ):
		case( CT_CHAIN_LINK ):
		case( CT_SHADE_CLOTH ):
		case( CT_CORNER_STAGE_BOARD ):
			//fine, just calculate the level based on the RL
			break;
		}	//switch( GetType() )
		
		//Need to find the RL for this component
		// Then determine which Level it is within!
		if( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL )
		{
			iLvl = GetLevelForRL( GetRL()+GetLiftPointer()->GetBayPointer()->RLAdjust() );
		}
		else if( GetBayPointer()!=NULL )
		{
			iLvl = GetLevelForRL( GetRL()+GetBayPointer()->RLAdjust() );
		}
		else if( GetMatrixElementPointer()!=NULL )
		{
			iLvl = GetLevelForRL( GetRL() );
		}
		else
		{

			assert( false );
		}
	}
	else
	{
		iLvl = m_iLevel;
	}
	return iLvl;
}


//////////////////////////////////////////////////////////////////////
//	GetStage
//	Returns the stage of a component.
CString Component::GetStage() const
{
	CString sStage;

	sStage = STAGE_DEFAULT_VALUE;
	if( m_sStage.IsEmpty() || m_sStage==STAGE_DEFAULT_VALUE )
	{
		if( GetBay()!=NULL )
		{
			if( GetBay()->GetBayType()==BAY_TYPE_LAPBOARD )
			{
				LapboardBay *pLap;
				pLap = (LapboardBay*)GetBay();
				sStage = GetController()->GetBayStageLevelArray()->GetStage(
							pLap->GetBayWest()->GetBayNumber(), GetLevel() );
			}
			else
			{
				sStage = GetController()->GetBayStageLevelArray()->GetStage(
							GetBay()->GetBayNumber(), GetLevel() );
			}
		}
	}
	else
	{
		sStage = m_sStage;
	}
	return sStage;
}


//////////////////////////////////////////////////////////////////////
//	GetComponentDetails
//	Gets the length, width, type and material of construction characteristics of 
//	the component.
void Component::GetComponentDetails(double &dLength, double &dWeight, ComponentTypeEnum &Type, MaterialTypeEnum &MaterialType) const
{
	dLength			= GetLengthCommon();
	dWeight			= GetWeightActual();
	Type			= GetType();
	MaterialType	= GetMaterialType();
}


//////////////////////////////////////////////////////////////////////
//	GetRL
//	Returns the star height the component is connected to
double Component::GetRL() const
{
	//The only things that affect vertical movement are the position
	//	within the lift and the lifts postion within the bay;
	Lift		*pLift;
	double		dRL;

	pLift = GetLiftPointer();

	dRL = 0.00;
/*
	This is not quite working correctly, some of the ties are ending up on the
	wrong rl - this is probably due to being set incorrectly
	if( GetVirtualRL()>LARGE_NEGATIVE_NUMBER+ROUND_ERROR )
	{
		dRL = GetVirtualRL();
	}
	else*/
	if( pLift!=NULL )
	{
		dRL = pLift->GetRL();
		//Decking, lapboards and stageboards use the rise as
		//	its position in the array!  
		switch( GetType() )
		{
		default:
			dRL+= GetRiseFromRiseEnum( (LiftRiseEnum)GetRise(), GetStarSeparation() );
			break;
		case( CT_STAIR ):
		case( CT_DECKING_PLANK ):
		case( CT_LAPBOARD ):
		case( CT_STAGE_BOARD ):
		case( CT_CORNER_STAGE_BOARD ):
			//These components are always on the deck,
			//	and rise has a different meaning
			break;
		}
	}
	else
	{
		switch( GetType() )
		{
		default:
			//don't round to the nearest star
			return GetRLActual();

		case( CT_MESH_GUARD ):
		case( CT_TOE_BOARD ):
		case( CT_TIE_TUBE ):
		case( CT_TIE_CLAMP_COLUMN ):
		case( CT_TIE_CLAMP_MASONARY ):
		case( CT_TIE_CLAMP_YOKE ):
		case( CT_TIE_CLAMP_90DEGREE ):
			if( GetBay()!=NULL )
			{
				pLift = GetBay()->GetLiftAtRL( GetRLActual() );
				if( pLift!=NULL )
				{
					dRL = pLift->GetRL();
				}
				else
				{
					//don't round to nearest
					return GetRLActual();
				}
			}
		}
	}

	//round to the nearest star
	int		iTemp;
	double	dTemp;
	dTemp = dRL/GetStarSeparation();
	iTemp = (int)(dTemp+(dTemp>=0.00? 0.50: -0.05));
	dTemp = (double)iTemp;
	dRL   = dTemp*GetStarSeparation();

	return dRL;
}

//////////////////////////////////////////////////////////////////////
//	GetRLActual
//	Returns the 'z component' of the first point of the component.
double Component::GetRLActual() const
{
	//The only things that affect vertical movement are the position
	//	within the lift and the lifts postion within the bay;
	Bay			*pBay;
	Lift		*pLift;
	double		dRL;
	Point3D		pt;
	Matrix3D	Transform, Trans;

	pLift = GetLiftPointer();

	dRL = 0.00;
	pt.set( 0.00, 0.00, 0.00 );
	if( pLift!=NULL )
	{
		pt.transformBy( pLift->GetTransform()*GetTransform() );
		dRL = pt.z;

		if( GetType()==CT_HOPUP_BRACKET )
		{
			//We are interested in the RL at the top of the Bracket not the bottom!
			dRL+= GetHeightActual();
		}
	}
	else
	{
		pBay = GetBayPointer();
		if( pBay!=NULL )
		{
			pt.transformBy( GetTransform() );
			dRL = pt.z;
		}
	}

	int		iTemp;
	double	dTemp;
	dTemp = dRL/GetStarSeparation();
	iTemp = (int)(dTemp+(dTemp>=0.00? 0.50: -0.05));
	dTemp = (double)iTemp;
	dRL   = dTemp*GetStarSeparation();

	return dRL;
}


//////////////////////////////////////////////////////////////////////
// Bay functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	GetBayPointer
//	Gets the pointer to the Bay [via a Lift] which the component belongs to (if the
//	component is owned by a Bay [via a Lift]).
Bay * Component::GetBayPointer() const
{
	return m_pBay;
}


//////////////////////////////////////////////////////////////////////
//	SetBayPointer
//	Sets the pointer to the Bay [via a Lift] which the component belongs to (if the
//	component is owned by a Bay [via a Lift]).
void Component::SetBayPointer(Bay *pSetBay)
{
	//set the pointer
	SetAllPointerToNULL();
	m_pBay = pSetBay;
}


void Component::SetMatrixElementPointer(MatrixElement *pElement)
{
	//set the pointer
	SetAllPointerToNULL();
	m_pElement = pElement;
}

MatrixElement * Component::GetMatrixElementPointer() const
{
	return m_pElement;
}


//////////////////////////////////////////////////////////////////////
// Lift functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	GetLiftPointer
//	Gets the pointer to the lift which the component belongs to (if the
//	component is owned by a lift).
Lift * Component::GetLiftPointer() const
{
	return m_pLift;
}


//////////////////////////////////////////////////////////////////////
//	SetLiftPointer
//	Sets the pointer to the lift which the component belongs to (if the
//	component is owned by a lift).
void Component::SetLiftPointer(Lift *pSetLift)
{
	//set the pointer
	SetAllPointerToNULL();
	m_pLift		= pSetLift;
}


//////////////////////////////////////////////////////////////////////
// Node functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	GetNode
//	This function returns the element of the array at position iNodesPos.
Point3D Component::GetNode( int iNodePos ) const
{
	return m_Nodes.at( iNodePos );
}


//////////////////////////////////////////////////////////////////////
//	AddNode
//	This function adds a 3D point node to the end of the node list and 
//	returns the index of the new last element.
int Component::AddNode( Point3D NodeToAdd )
{
	return m_Nodes.append( NodeToAdd );
}

//////////////////////////////////////////////////////////////////////////////////////////
//DeleteNodes
//deletes all node in the array
bool Component::DeleteNodes()
{
	if ( m_Nodes.logicalLength()<=0 )
		return false;

	m_Nodes.removeSubArray( 0, m_Nodes.logicalLength()-1 );	//remove all nodes
	assert( m_Nodes.logicalLength()<=0 );
	return true;
}



//////////////////////////////////////////////////////////////////////
// Point functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	SetPoint
//	Sets either the first point or second point depending on the 
//	enum argument [CP_FIRST_POINT or CP_SECOND_POINT] to the point ptPoint in
//	the argument.
bool Component::SetPoint( int iWhichPoint, Point3D ptPoint )
{
	m_ptPoints3D[iWhichPoint] = ptPoint;

	return true;
}

//////////////////////////////////////////////////////////////////////
//AddPoint
//Adds a point to the points array
int Component::AddPoint(Point3D ptPoint)
{
	return m_ptPoints3D.append( ptPoint );
}


//////////////////////////////////////////////////////////////////////
//	GetPoint
//	Returns either the first point or second point depending on the 
//	enum argument [CP_FIRST_POINT or CP_SECOND_POINT].
Point3D Component::GetPoint( int iWhichPoint ) const
{
	Point3D pt;
	if( GetNumberOfPoints()<=0 || iWhichPoint<0 ||
		iWhichPoint>GetNumberOfPoints() )
	{
		pt.set( 0.00, 0.00, 0.00 );
		pt.transformBy( GetTransform() );
		return pt;
	}
	return m_ptPoints3D[iWhichPoint];
}


//////////////////////////////////////////////////////////////////////
//	SetPoints
//	Sets the first point and second point as well as emptying 
//	the node array of all elements(if any) and adding one node 'ptNode'
//	point to the empty node array.
bool Component::SetPoints(Point3D pt1stPoint, Point3D pt2ndPoint, Point3D ptNode)
{
	DeletePoints();
	AddPoint( pt1stPoint );
	AddPoint( pt2ndPoint );

	DeleteNodes();

	m_Nodes.append(ptNode);									// add first node

	return true;
}



//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Create3DView
bool Component::Create3DView()
{
	Matrix3D	Trans;

	///////////////////////////////////////////////////////////////
	//GetEntity() must be null
	if( GetEntity()!=NULL )
	{
		return false;
	}

	///////////////////////////////////////////////////////////////
	//Move the 3D points
	Trans = GetEntireTransform();

	if( CreatePoints() )
	{
		Move( Trans, false );

		if( GetVisible()==VIS_VISIBLE )
		{
			if( GetMatrixElementPointer()!=NULL )
			{
				ApplyVisibility();
			}
			else
			{
				if( !GetController()->IsComponentTypeVisible( GetType() ) )
				{
					ApplyVisibility( VIS_NOT_VISIBLE);
				}
				else if( !GetController()->IsStageLevelVisible( GetStage(), GetLevel() ) )
				{
					ApplyVisibility( VIS_NOT_VISIBLE);
				}
				else
				{
					ApplyVisibility();
				}
			}
		}
		else
		{
			ApplyVisibility();
		}

		return true;
	}

//	GetController()->RefreshScreen();
	return false;
}

//////////////////////////////////////////////////////////////////////
//Delete3DView
void Component::Delete3DView()
{
	DeleteEntity();
}

void Component::ApplyBayResize( Matrix3D TransformX, Matrix3D TransformY )
{
	Matrix3D Transform, Original, Trans, SizeChangeX;
	double	dPreviousActualLength;

	if( GetPreviousLength()<0.001 && GetPreviousLength()>-0.001 )
	{
		//prevent divide by zero
		return;
	}

	//store the original transformation
	Original = GetTransform();

	//move the components back so that the origins of the component and the WC intersect
	Trans = GetTransform();
	Trans.invert();
	
	//The component may have changed size when the bay size changed
	//	so apply the lenght changes to the component
	SizeChangeX.setToIdentity();
	dPreviousActualLength = FindLengthActual( GetPreviousLength() );
	SizeChangeX.entry[0][0] = GetLengthActual()/dPreviousActualLength;

	Trans = SizeChangeX*Trans;

	//move the component back to its initial position within the bay
	Trans= Original*Trans;

	//move to its new position within the bay, due to bay size change
	Transform.setToIdentity();
	if( GetMoveWhenBayLengthChanges() )
		Transform = TransformX * Transform;
	if( GetMoveWhenBayWidthChanges() )
		Transform = TransformY * Transform;
	Trans = Transform * Trans;

	Move( Trans, true );

	SetPreviousLength( GetLengthCommon() );
}


//////////////////////////////////////////////////////////////////////
//CreateEntity
//This will create the entity based on the component type and the points array.
//	Requires a valid points array for its type and also the previous Entity object
//	should have been deleted by now!
void Component::CreateEntity( UseActualComponentsEnum eUse )
{
	int					iColour;
	double				dTextHeight;
	Point3D				pt;
	CString				sBayNumber;
	Matrix3D			Transform;
	Vector3D			Vector;
	AcDbText			*pText;
	AcDbPolygonMesh		*pMesh;
	AcDbObjectId		recordId;
	EntityReactor		*pReactor;
//	AcDb3dPolyline		*pLine;
	Acad::ErrorStatus	es;
	AcDbTextStyleTable	*pTable;

	//we are about to create a new entity so any existing one should be
	//	removed by now
	assert( GetEntity()==NULL );

	if((( (GetMatrixElementPointer()!=NULL) && (eUse&UAC_MATRIX) ) ||
		( (GetMatrixElementPointer()==NULL) && (eUse&UAC_3D_VIEW) )) &&
		  GetType()!=CT_TEXT )
	{
		///////////////////////////////////////////////////////////////
		//Use actual components if you can!

		ViewTypeEnum eVT;
		if( !GetMatrixElementPointer() )
		{
			eVT = VT_3D;
		}
		else
		{
			if( GetDrawMatrixElementSideOn() )
			{
				//This is side on
				eVT = VT_2D;
			}
			else
			{
				//Not side on this must be end on
				eVT = VT_2DEO;
			}
		}

		SetEntity( GetStockDetails()->GetClonedEntity(eVT) );

		if( GetEntity()==NULL )
		{
			//It didn't work, try again, this time use the
			//	bounding box algorithm!
			CreateEntity( UAC_NEITHER );
			return;
		}

		es = GetEntity()->open();
		assert( es==Acad::eOk );
		if( GetMatrixElementPointer()!=NULL )
		{
			if( GetMatrixElementPointer()->IsCutThrough() )
				es = GetEntity()->setLayer( LAYER_NAME_SECTIONS );
			else
				es = GetEntity()->setLayer( LAYER_NAME_MATRIX );
			assert( es==Acad::eOk );
		}
		else
		{
			CString sLayer, sStage;
			sLayer = LAYER_NAME_3D;
			sStage = GetStage();
			if( ( sStage!=STAGE_DEFAULT_VALUE || GetLevel()!=LEVEL_DEFAULT_VALUE ) &&
				!( sStage.IsEmpty() && GetLevel()==1 ) )
			{
				if( sStage.IsEmpty() )
					sStage = STAGE_DEFAULT_VALUE;

				sLayer.Format( _T("%s_%s_%i"), LAYER_NAME_3D, GetStage(), GetLevel() );
			}

			es = GetEntity()->setLayer( sLayer );
			if( es==Acad::eKeyNotFound || es==Acad::eInvalidLayer )
			{
				//The layer musn't exist, create it!
				AcDb::OpenMode mode = AcDb::kForWrite;
				AcDbLayerTable* pTable;
				pTable = NULL;
				es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, mode);
				if( es==Acad::eOk || es==Acad::eWasOpenForWrite )
				{
					if( !pTable->has( sLayer ) )
					{
						AcDbLayerTableRecord *pRecord;
						pRecord = new AcDbLayerTableRecord();
						pRecord->setName( sLayer );
						pTable->add( pRecord );
						pRecord->close();
					}
				}
				if( pTable!=NULL )
				{
					es = pTable->close();
					assert( es==Acad::eOk );
				}
				es = GetEntity()->setLayer( sLayer );
			}
			assert( es==Acad::eOk );
		}
		es = GetEntity()->close();
		assert( es==Acad::eOk );

		SetXData();
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////////////
		//Use wireframe version
		//we should already have the points array created, hopefully in the correct
		//	format for the type of component
//		assert( GetNumberOfPoints()>2 );

		//////////////////////////////////////////////////////////////////////
		//create the entity
		switch( GetType() )
		{

		//////////////////////////////////////////////////////////
		//rectangular prism objects
		case( CT_DECKING_PLANK ):		//Fallthrough
		case( CT_STAGE_BOARD ):			//fallthrough
		case( CT_LAPBOARD ):			//Fallthrough
		case( CT_TOE_BOARD ):			//Fallthrough
		case( CT_SOLEBOARD ):			//Fallthrough
		case( CT_LEDGER ):				//Fallthrough
		case( CT_TRANSOM ):				//Fallthrough
		case( CT_RAIL ):				//Fallthrough
		case( CT_MID_RAIL ):			//Fallthrough
		case( CT_CHAIN_LINK ):			//Fallthrough
		case( CT_SHADE_CLOTH ):			//Fallthrough
		case( CT_MESH_GUARD ):			//Fallthrough
		case( CT_TIE_TUBE ):			//Fallthrough
		case( CT_BRACING ):				//Fallthrough
		case( CT_STANDARD ):			//Fallthrough
		case( CT_STANDARD_OPENEND ):	//Fallthrough
		case( CT_JACK ):				//Fallthrough
		case( CT_TIE_CLAMP_COLUMN ):	//Fallthrough
		case( CT_TIE_CLAMP_MASONARY ):	//Fallthrough
		case( CT_TIE_CLAMP_YOKE ):		//Fallthrough
		case( CT_TIE_CLAMP_90DEGREE ):	//Fallthrough
		case( CT_STAIR ):				//Fallthrough
		case( CT_LADDER ):				//Fallthrough
		case( CT_CORNER_STAGE_BOARD ):	//Fallthrough
		case( CT_TIE_BAR ):				//Fallthrough
		case( CT_STANDARD_CONNECTOR ):	//Fallthrough
		case( CT_LADDER_PUTLOG ):		//Fallthrough
		case( CT_STAIR_RAIL ):			//Fallthrough
		case( CT_STAIR_RAIL_STOPEND ):	//Fallthrough
		case( CT_TOE_BOARD_CLIP ):		//Fallthrough
		case( CT_BOM_EXTRA ):			//Fallthrough
		case( CT_PUTLOG_CLIP ):			//Fallthrough
		case( CT_VISUAL_COMPONENT ):	//Fallthrough
		case( CT_HOPUP_BRACKET ):
			if( GetMatrixElementPointer()!=NULL )
			{
				assert( GetNumberOfPoints()>1 );
				pMesh = new Kwik3DPolygonMesh(AcDb::kSimpleMesh, 2, 2, m_ptPoints3D, Adesk::kFalse, Adesk::kFalse );
			}
			else
			{
				assert( GetNumberOfPoints()>1 );
				pMesh = new Kwik3DPolygonMesh(AcDb::kSimpleMesh, 6, 3, m_ptPoints3D, Adesk::kFalse, Adesk::kFalse );
			}
			pMesh->setDatabaseDefaults();
			SetEntity( (Entity*)pMesh );
			break; 

		case( CT_TEXT ):
			acdbHostApplicationServices()->workingDatabase()->
							getSymbolTable( pTable, AcDb::kForRead);
			es = pTable->getAt( TEXT_STYLE_COMPONENT, recordId );
			if( es==Acad::eKeyNotFound )
			{
				es = pTable->getAt( TEXT_STYLE_STANDARD, recordId );
			}
			if( es==Acad::eOk )
			{
				sBayNumber = GetTextSmart();
				dTextHeight = GetHeightActual();
				dTextHeight*= m_dTextHeightMultiplier;
				pt.set( 0.00, 0.00, 0.00 );
				pText = new AcDbText( pt, sBayNumber, recordId, dTextHeight );
				pText->setDatabaseDefaults();

				SetEntity( (Entity*)((AcDbEntity*) pText ));

				es = ((AcDbText*)GetEntity())->setHorizontalMode(m_TextHAlign);
				es = ((AcDbText*)GetEntity())->setVerticalMode(m_TextVAlign);
			}
			es = pTable->close();
			assert( es==Acad::eOk );
			break;
		default:	
			assert( FALSE ); //invalid type
		}

		if( GetEntity()==NULL )
		{
			acedAlert(_T("Not enough memory to create a Line!"));
			return;
		}

		//////////////////////////////////////////////////////////////////////
		//add to database
		if( GetMatrixElementPointer()!=NULL )
		{
			if( GetMatrixElementPointer()->IsCutThrough() )
				GetEntity()->postToDatabase( LAYER_NAME_SECTIONS );
			else
				GetEntity()->postToDatabase( LAYER_NAME_MATRIX );
		}
		else
		{
			CString sLayer, sStage;
			sLayer = LAYER_NAME_3D;
			sStage = GetStage();
			if( ( sStage!=STAGE_DEFAULT_VALUE || GetLevel()!=LEVEL_DEFAULT_VALUE ) &&
				!( sStage.IsEmpty() && GetLevel()==1 ) &&
				!( sStage==STAGE_DEFAULT_VALUE && GetLevel()==1 ) )
			{
				if( sStage.IsEmpty() )
					sStage = STAGE_DEFAULT_VALUE;

				sLayer.Format( _T("%s_%s_%i"), LAYER_NAME_3D, GetStage(), GetLevel() );
			}

			GetEntity()->postToDatabase( sLayer );
		}
		SetXData();

		assert( !GetEntity()->objectId().isNull() );
		es = GetEntity()->close();
		assert( es==Acad::eOk );
	}

	assert( GetEntity()!=NULL );

	//////////////////////////////////////////////////////////////////////
	//colour
	iColour = GetColour();

	if( m_eVisible==VIS_ERASE )
		iColour=COLOUR_ERASE;
	es = GetEntity()->open();
	assert( es==Acad::eOk );
	if (GetEntity()->setColorIndex((Adesk::UInt16)iColour) != Acad::eOk)
	{
		acutPrintf(_T("\nInvalid Colour chosen.\n"));
	}
	es = GetEntity()->close();
	assert( es==Acad::eOk );

	pReactor = NULL;
	if( GetMatrixElementPointer()==NULL || !GetMatrixElementPointer()->IsCutThrough() )
	{
		GetEntity()->AttachedReactors( &pReactor );
		SetReactor( pReactor );
		GetReactor()->SetComponentPointer( this );
		if( GetMatrixElementPointer()!=NULL )
		{
			GetReactor()->SetMatrixPointer( 
					GetMatrixElementPointer()->GetMatrixPointer() );
		}
	}

	if( GetType()==CT_TEXT )
	{
		//move the text to the centre of the bay
		Vector.set( m_ptPoints3D[0].x, m_ptPoints3D[0].y, m_ptPoints3D[0].z ); 
		Transform.setTranslation( Vector );
		//don't store this movement as a component movement, otherwise
		//	when the text is redrawn the numbers will be offset AGAIN
		//	by this transformation
		Move( Transform, false );
	}
	es = GetEntity()->close();
	assert( es==Acad::eOk );
}


//////////////////////////////////////////////////////////////////////
//Remove
//This function removes a component from a lift by notifying the parent lift
//	of the deletion. This function is called directly.
void Component::Remove()
{
	SetDirtyFlag( DF_DELETE );
}

void Component::SetBOMExtraPointer(BOMExtra *pBOMExtra)
{
	//set the pointer
	SetAllPointerToNULL();
	m_pBOMExtra = pBOMExtra;
}


///////////////////////////////////////////////////////////////////////
//	WriteBOMTo
//
bool Component::WriteBOMTo(CStdioFile *pFile) const
{
	CString				sLine, sType, sPartNo, sMaterial;
	double				dLength;
	double				dWeight;
	ComponentTypeEnum	eType;
	MaterialTypeEnum	eMaterialType;
	int					iRunID, iBayID, iLiftID, iCompID, iBayNumber;

	sLine.Empty();

	GetComponentDetails( dLength, dWeight, eType, eMaterialType);

	if( eType==CT_TEXT )
		return true;

	if( eType==CT_UNDEFINED )
		return false;

	sPartNo	= GetStockDetails()->GetPartNumber();
	assert( !sPartNo.IsEmpty() );

	//Get the description, default to the one supplied by the compdet.csv
//	sType	= GetStockDetails()->GetDescription();
	sType	= GetComponentDescStr( eType );
	if( GetController()->GetStockList()!=NULL )
	{
		if( GetController()->GetStockList()->GetMatchingComponent(sPartNo)!=NULL )
		{
			GetController()->GetStockList()->GetDescription( sPartNo, sType );
		}
	}

	sMaterial = GetComponentMaterialStr( eMaterialType );
	if( GetBay()->GetBayType()==BAY_TYPE_LAPBOARD )
	{
		assert( GetLift()!=NULL );
		iLiftID = GetLift()->GetLiftID();
		assert( GetBay()!=NULL );
		iBayID	= GetBay()->GetID();
		iCompID	= GetID();

		iCompID	= ( iCompID<0	)? 0: ++iCompID;
		iLiftID	= ( iLiftID<0	)? 0: ++iLiftID;
		iBayID	= ( iBayID<0	)? 0: ++iBayID;
		sLine.Format( _T("%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
						iBayID, iLiftID, iCompID,
						sType, sPartNo, sMaterial, dLength, dWeight,
						GetLevel(), GetStage() );
		pFile->WriteString( (LPCTSTR)sLine );
	}
	else
	{
		/////////////////////////////////////////////////
		//standard bay

		GetIDs( iCompID, iLiftID, iBayID, iRunID );
		iCompID	= ( iCompID<0	)? 0: ++iCompID;
		iLiftID	= ( iLiftID<0	)? 0: ++iLiftID;
		iBayID	= ( iBayID<0	)? 0: ++iBayID;
		iRunID	= ( iRunID<0	)? 0: ++iRunID;
		iBayNumber = GetBay()->GetBayNumber();


		/////////////////////////////////////////////////
		//shade cloth and chain link should be handled differently
		if( eType==CT_SHADE_CLOTH ||
			eType==CT_CHAIN_LINK )
		{
			//These are the only objects which span several levels
			// so we need to divide it up into levels
			double dArea, dLevel, dHeight, dTotalHeight, dLevelBottom, dLevelTop, dUnitWeight;
			CString sStage;

			dTotalHeight = (double)GetRise();

			dUnitWeight = dWeight;
			dLength = GetBay()->GetBayWidthActual();

			//The x,y values of the points are set to the x,y values of the standards
			switch( GetSideOfBay() )
			{
				case( NORTH ):
					dLevelBottom= GetBay()->GetStandardPosition(CNR_NORTH_EAST).z;
					dLevelBottom= max( dLevelBottom, GetBay()->GetStandardPosition(CNR_NORTH_WEST).z );
					break;
				case( SOUTH ):
					dLevelBottom= GetBay()->GetStandardPosition(CNR_SOUTH_EAST).z;
					dLevelBottom= max( dLevelBottom, GetBay()->GetStandardPosition(CNR_SOUTH_WEST).z );
					break;
				case( EAST ):
					dLevelBottom= GetBay()->GetStandardPosition(CNR_NORTH_EAST).z;
					dLevelBottom= max( dLevelBottom, GetBay()->GetStandardPosition(CNR_SOUTH_EAST).z );
					break;
				case( WEST ):
					dLevelBottom= GetBay()->GetStandardPosition(CNR_NORTH_WEST).z;
					dLevelBottom= max( dLevelBottom, GetBay()->GetStandardPosition(CNR_SOUTH_WEST).z );
					break;
				default:
					//what the hell happend to eSide
					assert(FALSE);
					break;
			}
			dLevelTop = dLevelBottom + dTotalHeight;

			//We now know that this materail extends from dLevelBottom and dLevelTop
			//	where do the Level lines cut this materail?  At each cut, and also
			//	at dLevelTop, we need to include an new element in out BOM.
			LevelList *pLevels;
			pLevels = GetController()->GetLevelList();

			int iLevel=0;
			dLevel = dLevelBottom;
			while( iLevel<pLevels->GetSize() )
			{
				dLevel = pLevels->GetLevel( iLevel );
				if( dLevel>dLevelBottom && dLevel<dLevelTop )
				{
					//we have a new component!
					dHeight	= dLevel-dLevelBottom;
					dArea	= dLength * dHeight;
					dWeight	= dArea * dUnitWeight;
					sStage	= GetController()->GetBayStageLevelArray()->GetStage(
									GetBay()->GetBayNumber(), iLevel+1 );

					sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
									iBayNumber, iRunID, iBayID, iLiftID, iCompID,
									sType, sPartNo, sMaterial, dArea, dWeight,
									(iLevel+1), sStage );
					pFile->WriteString( (LPCTSTR)sLine );

					//we are now only interested in values above this line
					dLevelBottom = dLevel;
				}
				iLevel++;
			}
			if( dLevelTop>dLevel )
			{
				//The level lines don't reach this height, so this is part of the top level
				dHeight	= dLevelTop-dLevelBottom;
				dArea	= dLength * dHeight;
				dWeight	= dArea * dUnitWeight;
				sStage	= GetController()->GetBayStageLevelArray()->GetStage(
								GetBay()->GetBayNumber(), iLevel+1 );
				sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
								iBayNumber, iRunID, iBayID, iLiftID, iCompID,
								sType, sPartNo, sMaterial, dArea, dWeight,
								(iLevel+1), sStage );
				pFile->WriteString( (LPCTSTR)sLine );
			}
		}
		else
		{
			//not chain link or shade cloth
			sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
							iBayNumber, iRunID, iBayID, iLiftID, iCompID,
							sType, sPartNo, sMaterial, dLength, dWeight,
							GetLevel(), GetStage() );
			pFile->WriteString( (LPCTSTR)sLine );


			////////////////////////////////////////////////////////////
			//We may need a Tie bar!
			if( eType==CT_STAGE_BOARD && GetLiftPointer()!=NULL && GetRise()==1 )
			{
				//This is the second stage board, so we also need to create a tie bar
				//	This tie bar will not be drawn on the screen but will only appear
				//	in the BOM
				doubleArray	daLengths;

				daLengths = GetController()->GetCompDetails()->GetCommonLengthsForComponent( CT_TIE_BAR, MT_STEEL );

				//find the right lenght of tie bar!
				bool bFound;
        int iCount=0;
				bFound = false;
				for( iCount=0; iCount<daLengths.GetSize(); iCount++ )
				{
					if( daLengths[iCount]>dLength-ROUND_ERROR &&
						daLengths[iCount]<dLength+ROUND_ERROR )
					{
						bFound = true;
						break;
					}
				}
				if( !bFound )
					return false;

				//we have a matching length
				sMaterial = GetComponentMaterialStr( MT_STEEL );
				sPartNo = GetController()->GetCompDetails()->GetComponentPartNumberStr( GetSystem(), CT_TIE_BAR, daLengths[iCount], MT_STEEL );
				GetController()->GetStockList()->GetDescription( sPartNo, sType );
				GetController()->GetStockList()->GetWeight( sPartNo, dWeight );
				sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
								iBayNumber, iRunID, iBayID, iLiftID, iCompID,
								sType, sPartNo, sMaterial, dLength, dWeight,
								GetLevel(), GetStage() );
				pFile->WriteString( (LPCTSTR)sLine );
			}
			else if( GetSystem()==S_MILLS && 
					 (eType==CT_STANDARD || eType==CT_STANDARD_OPENEND ) )
			{
				//Standard connectors
				//This is a standard in the mill system, therefore we need a conector
				//	unless it is the bottom standard in a pole, Rise indicates it
				//	position in the pole!
				if( GetRise()>0 )
				{
					sType = _T("STANDARD COUPLER");
					sPartNo = _T("109000");
					dLength = 150.00;
					dWeight = 0.50;
					sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
									iBayNumber, iRunID, iBayID, iLiftID, iCompID,
									sType, sPartNo, sMaterial, dLength, dWeight,
									GetLevel(), GetStage() );
					pFile->WriteString( (LPCTSTR)sLine );
				}
			}
			else if( eType==CT_DECKING_PLANK )
			{
				//We need a putlog if the last plank is shorter than the baywidth!
				if( GetLiftPointer()!=NULL &&
					GetLiftPointer()->GetBayPointer()!=NULL && 
					dLength<GetLiftPointer()->GetBayPointer()->GetBayLength() &&
					GetRise()==GetNumberOfPlanksForWidth( GetLiftPointer()->GetBayPointer()->GetBayWidth() )-1 )
				{
					//NOTE: The Ladder needs to have a Putlog, this is an approximation, since
					//	under some conditions there may be 2 putlog per ladder, however
					//	since we are only doing approximate ladder bays, I have restricted
					//	this to 1 ladder per bay, so this approximation should work, with
					//	the exception that the bottom lift does not have a putlog!  However
					//	this is made up for by the fact that the top lift doesn't have a ladder!
					if( GetRise()>0 )
					{
						dLength = COMPONENT_LENGTH_1200;
						if( GetBayPointer()!=NULL )
						{
							dLength = GetBayPointer()->GetBayLength();
						}

						assert( dLength==COMPONENT_LENGTH_1200 ||
								dLength==COMPONENT_LENGTH_1800 ||
								dLength==COMPONENT_LENGTH_2400 );

						sPartNo = GetController()->GetCompDetails()->GetComponentPartNumberStr( GetSystem(), CT_LADDER_PUTLOG, dLength, MT_STEEL );
						sMaterial = GetComponentMaterialStr( MT_STEEL );
						sType = _T("LADDER TOWER PUTLOG");
						dWeight	= 0.00;
						if( GetController()->GetStockList()->GetMatchingComponent(sPartNo)!=NULL )
						{
							GetController()->GetStockList()->GetDescription(sPartNo, sType);
							GetController()->GetStockList()->GetWeight(sPartNo, dWeight);
						}

						sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
										iBayNumber, iRunID, iBayID, iLiftID, iCompID,
										sType, sPartNo, sMaterial, dLength, dWeight,
										GetLevel(), GetStage() );
						pFile->WriteString( (LPCTSTR)sLine );
					}
				}
			}
			else if( eType==CT_STAIR )
			{
				if( GetLiftPointer()!=NULL ) 
				{
					//The bottom lift requires a stop end (actuall the top lift)
					ComponentTypeEnum eRailType;
					if( GetLiftPointer()->GetLiftID()==0 && GetRise()==SPR_OUTER )
					{
						eRailType = CT_STAIR_RAIL_STOPEND;
						sType = _T("STAIR RAIL STOPEND");
						dWeight	= 16.2;
					}
					else
					{
						eRailType = CT_STAIR_RAIL;
						sType = _T("STAIR RAIL");
						dWeight	= 5.7;
					}

					dLength = GetLengthCommon();
					sPartNo = GetController()->GetCompDetails()->GetComponentPartNumberStr( GetSystem(), eRailType, dLength, MT_STEEL );
					sMaterial = GetComponentMaterialStr( MT_STEEL );
					if( GetController()->GetStockList()->GetMatchingComponent(sPartNo)!=NULL )
					{
						GetController()->GetStockList()->GetDescription(sPartNo, sType);
						GetController()->GetStockList()->GetWeight(sPartNo, dWeight);
					}

					sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
									iBayNumber, iRunID, iBayID, iLiftID, iCompID,
									sType, sPartNo, sMaterial, dLength, dWeight,
									GetLevel(), GetStage() );
					pFile->WriteString( (LPCTSTR)sLine );
				}
			}
			else if( eType==CT_LADDER && GetController()->GetUsePutLogClipsInLadder() )
			{
				sType = _T("PUTLOG CLIP");
				dWeight	= 0.6;
				dLength = 0;
				sPartNo = GetController()->GetCompDetails()->GetComponentPartNumberStr( GetSystem(), CT_PUTLOG_CLIP, dLength, MT_STEEL );
				sMaterial = GetComponentMaterialStr( MT_STEEL );
				if( GetController()->GetStockList()->GetMatchingComponent(sPartNo)!=NULL )
				{
					GetController()->GetStockList()->GetDescription(sPartNo, sType);
					GetController()->GetStockList()->GetWeight(sPartNo, dWeight);
				}

				sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
								iBayNumber, iRunID, iBayID, iLiftID, iCompID,
								sType, sPartNo, sMaterial, dLength, dWeight,
								GetLevel(), GetStage() );
				pFile->WriteString( (LPCTSTR)sLine );
				pFile->WriteString( (LPCTSTR)sLine );
			}
			else if( eType==CT_TRANSOM && GetController()->GetUsePutLogClipsInLadder() && GetLiftPointer()->GetBayPointer()->GetBayType()==BAY_TYPE_LADDER )
			{
				//In Melbourne the use a 1.2m Ledger instead of a 1.2m transom for ladder towers
				//however, we have already placed a transom so just leave it!
				1;
			}
			else if( eType==CT_TOE_BOARD )
			{
				if( GetLiftPointer()!=NULL &&
					GetLiftPointer()->GetBayPointer()!=NULL ) 
				{
					//The bottom lift requires a stop end (actuall the top lift)
					sType = _T("TOE BOARD CLIP");
					dWeight	= 0.6;
					dLength = 0;
					sPartNo = GetController()->GetCompDetails()->GetComponentPartNumberStr( GetSystem(), CT_TOE_BOARD_CLIP, dLength, MT_STEEL );
					sMaterial = GetComponentMaterialStr( MT_STEEL );
					if( GetController()->GetStockList()->GetMatchingComponent(sPartNo)!=NULL )
					{
						GetController()->GetStockList()->GetDescription(sPartNo, sType);
						GetController()->GetStockList()->GetWeight(sPartNo, dWeight);
					}

					sLine.Format( _T("%i,%i,%i,%i,%i,%s,%s,%s,%0.2f,%0.2f,%i,%s\n"),
									iBayNumber, iRunID, iBayID, iLiftID, iCompID,
									sType, sPartNo, sMaterial, dLength, dWeight,
									GetLevel(), GetStage() );
					pFile->WriteString( (LPCTSTR)sLine );

					//we need two for the first bay in the run
					if( GetLiftPointer()->GetBayPointer()->GetID()==0 )
						pFile->WriteString( (LPCTSTR)sLine );
				}
			}
		}
	}

	return true;
}

void Component::SetVisible( VisibilityEnum eVisible/*=VIS_VISIBLE*/)
{
	assert( eVisible==VIS_VISIBLE || eVisible==VIS_NOT_VISIBLE );
	m_eVisible = eVisible;
	ApplyVisibility(m_eVisible);
}

void Component::ApplyVisibility(VisibilityEnum eVisible/*=VIS_USE_DEFAULT*/)
{
	if( eVisible<=VIS_MULTI_SELECTION )
	{
		//Invalid value use the default instead!
		eVisible=GetVisible();
	}

	if( GetEntity()!=NULL )
	{
		if( GetEntity()->objectId().isNull() )
			return;

		GetEntity()->open();

		AcDb::Visibility Vis;
		if( eVisible==VIS_VISIBLE )
			Vis = AcDb::kVisible;
		else
			Vis = AcDb::kInvisible;
		
		GetEntity()->setVisibility( Vis, false );

		GetEntity()->close();
	}
}

VisibilityEnum Component::GetVisible() const
{
	return m_eVisible;
}

//////////////////////////////////////////////////////////////////////////////////////////
//GetNumberOfPoints
//
int Component::GetNumberOfPoints() const
{
	return m_ptPoints3D.logicalLength();
}

//////////////////////////////////////////////////////////////////////////////////////////
//DeletePoints
//
void Component::DeletePoints()
{
	if( GetNumberOfPoints()>0 ) 
		m_ptPoints3D.removeSubArray( 0, GetNumberOfPoints()-1 );
//	while( GetNumberOfPoints()>0)
//		m_ptPoints3D.removeLast();
}

//////////////////////////////////////////////////////////////////////////////////////////
//CreatePoints
//Creates the points depending on the type, will set height and width, but length
//	must already be set.
bool Component::CreatePoints()
{
	Point3D	pt;
	DeletePoints();

	assert( GetNumberOfPoints()==0 );

	if( GetBay()!=NULL &&
		GetType()==CT_CHAIN_LINK ||
		GetType()==CT_SHADE_CLOTH )
	{
		switch( GetSideOfBay( ) )
		{
		case( NORTH ):
		case( SOUTH ):
			CreatePointsRectangleMesh( GetBay()->GetBayLengthActual(), 10.00, (double)GetRise() );
			break;
		case( EAST ):
		case( WEST ):
			CreatePointsRectangleMesh( GetBay()->GetBayWidthActual(), 10.00, (double)GetRise() );
			break;
		default:
			assert( false ); //what type of shade cloth/chain link is this?
		}
	}
	else if( GetType()==CT_SOLEBOARD && 
			GetBayPointer()!=NULL &&
			GetBayPointer()->GetBayWidth()==COMPONENT_LENGTH_1200 &&
			GetBayPointer()->GetController()->GetUse1500Soleboards() &&
			GetBayPointer()->GetInner()==NULL &&
			GetBayPointer()->GetOuter()==NULL )
	{
		//NOTE 20000731 - I have implemented 1.5m soleboards as an after thought,
		//	so I actually have to perform this calculation a few times, once here and
		//	once Just before I draw it in 3D & Matix, I hope these calculations produce
		//	the same result.
		//	There are several different methods I could have used to create 1.5m
		//	soleboards, we are currently using method 3:
		//	1) THE PROPER WAY - allow the user to edit the soleboard style and
		//		quantity via the Bay details dialog.
		//		PROs - Most adaptable to future changes;
		//		CONs - Lots of Code changes; Lots of error checking for chaning RLs;
		//	2) THE COMPONENT WAY - When you add the component, change the length or
		//		remove the component
		//		PROs - Simple to code;
		//		CONs - Has to assume the other leg has a soleboard; Changing RLs
		//			must force a recalc; no changes to Matrix & 3D code;
		//	3) *THE CHEAT WAY - work internally as .5m soleboards then when creating
		//		the BOM, or drawing the 3D or Matrix, at those stages then check
		//		if it is possible to add 1.5m soleboards
		//		PROs - Simpler to code; 
		//		CONs - calculated in several places thus could generate diff results;
		//			changes to 3 places in the code;

		//This is a soleboard for a 1200 wide bay, 
		//	and we are trying to use 1.5m soleboards
		//	plus there are no N/S neighbors
		bool			bRLsSame;
		double			dRL[2];
		SideOfBayEnum	eSide;

		eSide = GetSideOfBay();
		bRLsSame = false;
		if( eSide==NE || eSide==SE )
		{
			if( (eSide==NE && GetBayPointer()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SE ) ) ||
				(eSide==SE && GetBayPointer()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NE ) ) )
			{
				dRL[0] = GetBayPointer()->GetStandardPosition(CNR_NORTH_EAST).z;
				dRL[1] = GetBayPointer()->GetStandardPosition(CNR_SOUTH_EAST).z;
				if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR )
					bRLsSame = true;
			}
		}
		if( eSide==NW || eSide==SW )
		{
			if( (eSide==NW && GetBayPointer()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, SW ) ) ||
				(eSide==SW && GetBayPointer()->HasBayComponentOfTypeOnSide( CT_SOLEBOARD, NW ) ) )
			{
				dRL[0] = GetBayPointer()->GetStandardPosition(CNR_NORTH_WEST).z;
				dRL[1] = GetBayPointer()->GetStandardPosition(CNR_SOUTH_WEST).z;
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
				double dLength, dWidth;
				dLength = GetBayPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(),
									CT_SOLEBOARD, COMPONENT_LENGTH_1500, MT_TIMBER, false );
				dWidth = GetBayPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(),
									CT_SOLEBOARD, COMPONENT_LENGTH_1500, MT_TIMBER, false );
				CreatePointsRectangleMesh( dLength, dWidth, GetHeightActual(false) );

				//This longer component will need adjustments to the points
				Matrix3D	Trans;
				Vector3D	Vector;
				Vector.set( 0.00, GetBayPointer()->GetBayWidthActual()/2.00, 0.00 );
				Trans.setToTranslation( Vector );
				m_ptPoints3D.transformBy( Trans );
			}
			else
			{
				//These must be the northern side, so
				//	make sure we don't add a component
				return false;
			}
		}
		else
		{
			//Just add the soleboard
			CreatePointsRectangleMesh();
		}
	}
	else if( GetType()==CT_SOLEBOARD && 
			GetMatrixElementPointer()!=NULL &&
			GetMatrixElementPointer()->GetMatrixBayWidth()==COMPONENT_LENGTH_1200 &&
			GetMatrixElementPointer()->GetController()->GetUse1500Soleboards() &&
			!GetMatrixElementPointer()->IsCutThrough() )
	{
		//NOTE 20000731 - I have implemented 1.5m soleboards as an after thought,
		//	so I actually have to perform this calculation a few times, once here and
		//	once Just before I draw it in 3D & Matix, I hope these calculations produce
		//	the same result.
		//	There are several different methods I could have used to create 1.5m
		//	soleboards, we are currently using method 3:
		//	1) THE PROPER WAY - allow the user to edit the soleboard style and
		//		quantity via the Bay details dialog.
		//		PROs - Most adaptable to future changes;
		//		CONs - Lots of Code changes; Lots of error checking for chaning RLs;
		//	2) THE COMPONENT WAY - When you add the component, change the length or
		//		remove the component
		//		PROs - Simple to code;
		//		CONs - Has to assume the other leg has a soleboard; Changing RLs
		//			must force a recalc; no changes to Matrix & 3D code;
		//	3) *THE CHEAT WAY - work internally as .5m soleboards then when creating
		//		the BOM, or drawing the 3D or Matrix, at those stages then check
		//		if it is possible to add 1.5m soleboards
		//		PROs - Simpler to code; 
		//		CONs - calculated in several places thus could generate diff results;
		//			changes to 3 places in the code;

		//This is a soleboard for a 1200 wide matrix section, 
		//	and we are trying to use 1.5m soleboards
		//	plus there are no N/S neighbors
		bool			bRLsSame, bSoleBoardInner, bSoleBoardOuter;
		double			dRL[2];
		doubleArray		daArrangement;
		SideOfBayEnum	eSide;

		GetMatrixElementPointer()->GetStandardsArrangement( daArrangement, bSoleBoardInner, MATRIX_STANDARDS_INNER );
		dRL[0] = daArrangement[0];

		GetMatrixElementPointer()->GetStandardsArrangement( daArrangement, bSoleBoardOuter, MATRIX_STANDARDS_OUTER );
		dRL[1] = daArrangement[0];

		bRLsSame = false;
		if( dRL[0]<dRL[1]+ROUND_ERROR && dRL[0]>dRL[1]-ROUND_ERROR &&
			bSoleBoardInner && bSoleBoardOuter )
			bRLsSame = true;

		eSide = GetSideOfBay();
		if( bRLsSame )
		{
			//The RL's are the same between NORTH/SOUTH standard sets
			if( eSide==SOUTH_EAST )
			{
				//change the component lenght to 1.5m
				double dLength, dWidth;
				dLength = GetMatrixElementPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(),
									CT_SOLEBOARD, COMPONENT_LENGTH_1500, MT_TIMBER, false );
				dWidth = GetMatrixElementPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(),
									CT_SOLEBOARD, COMPONENT_LENGTH_1500, MT_TIMBER, false );
				CreatePointsRectangleMesh( dLength, dWidth, GetHeightActual(false) );
				Matrix3D	Trans;
				Vector3D	Vector;
				Vector.set( GetMatrixElementPointer()->GetMatrixBayWidthActual()/2.00, 0.00, 0.00 );
				Trans.setToTranslation( Vector );
				m_ptPoints3D.transformBy( Trans );
			}
			else
			{
				assert( eSide==NORTH_EAST );
				//These must be the northern side, so
				//	make sure we don't add a component
				return false;
			}
		}
		else
		{
			//Just add the soleboard
			CreatePointsRectangleMesh();
		}
	}
	else
	{
		if( GetLengthActual(false)<=ROUND_ERROR ||
			GetWidthActual(false)<=ROUND_ERROR ||
			GetHeightActual(false)<=ROUND_ERROR )
		{
			if( GetLift()!=NULL || GetBay()!=NULL )
			{
				acutPrintf( _T("\n------------------------------------------------------------"));
				acutPrintf( _T("\nError!  There is a %s component in "), GetComponentDescStr( GetType() ) );
				if( GetLift()!=NULL )
				{
					switch( GetLift()->GetBayPointer()->GetBayType() )
					{
					case( BAY_TYPE_BAY ):
						acutPrintf( _T("Lift %i of Bay %i that has"), GetLift()->GetLiftID()+1, GetLift()->GetBayPointer()->GetBayNumber() );
						break;
					case( BAY_TYPE_LAPBOARD ):
						acutPrintf( _T("Lift %i of Lapboard Bay %i that has"), GetLift()->GetLiftID()+1, GetLift()->GetBayPointer()->GetBayNumber() );
						break;
					case( BAY_TYPE_STAIRS ):
						acutPrintf( _T("Lift %i of Stair Bay %i that has"), GetLift()->GetLiftID()+1, GetLift()->GetBayPointer()->GetBayNumber() );
						break;
					case( BAY_TYPE_LADDER ):
						acutPrintf( _T("Lift %i of Ladder Bay %i that has"), GetLift()->GetLiftID()+1, GetLift()->GetBayPointer()->GetBayNumber() );
						break;
					case( BAY_TYPE_BUTTRESS ):
						acutPrintf( _T("Lift %i of Buttress Bay %i that has"), GetLift()->GetLiftID()+1, GetLift()->GetBayPointer()->GetBayNumber() );
						break;
					default:
					case( BAY_TYPE_UNDEFINED ):
						assert( false );
						break;
					}
				}
				else if( GetBay()!=NULL )
				{
					switch( GetBay()->GetBayType() )
					{
					case( BAY_TYPE_BAY ):
						acutPrintf( _T("Bay %i that has"), GetBay()->GetBayNumber() );
						break;
					case( BAY_TYPE_LAPBOARD ):
						acutPrintf( _T("Lapboard Bay %i that has"), GetBay()->GetBayNumber() );
						break;
					case( BAY_TYPE_STAIRS ):
						acutPrintf( _T("Stair Bay %i that has"), GetBay()->GetBayNumber() );
						break;
					case( BAY_TYPE_LADDER ):
						acutPrintf( _T("Ladder Bay %i that has"), GetBay()->GetBayNumber() );
						break;
					case( BAY_TYPE_BUTTRESS ):
						acutPrintf( _T("Buttress Bay %i that has"), GetBay()->GetBayNumber() );
						break;
					default:
					case( BAY_TYPE_UNDEFINED ):
						assert( false );
						break;
					}

				}
				acutPrintf( _T(" invalid dimensions.\nTo correct this fault you should try deleting the ") );
        acutPrintf( _T("Component or Bay\nand recreating it!  Details of the component are as follows:\n") );
			}
		}

		//we need these points regardless if it is wireframe or not
		switch( GetType() )
		{

		//////////////////////////////////////////////////////////
		//rectangular prism objects
		case( CT_DECKING_PLANK ):	//fallthrough
		case( CT_STAGE_BOARD ):		//fallthrough
		case( CT_LAPBOARD ):		//fallthrough
		case( CT_TOE_BOARD ):		//fallthrough
		case( CT_SOLEBOARD ):		CreatePointsRectangleMesh();		break;

		//////////////////////////////////////////////////////////
		//ledger\transom shapes
		case( CT_LEDGER ):			//fallthrough
		case( CT_TRANSOM ):			//fallthrough
		case( CT_RAIL ):			//fallthrough
		case( CT_MID_RAIL ):		//fallthrough
		case( CT_MESH_GUARD ):		CreatePointsRectangleMesh();		break;

		//////////////////////////////////////////////////////////
		//cylindrical based shapes
		case( CT_TIE_TUBE ):			//fallthrough
		case( CT_BRACING ):				//fallthrough
		case( CT_JACK ):				CreatePointsRectangleMesh();		break;

		case( CT_STANDARD ):			CreatePointsRectangleMesh( GetLengthActual(),
														GetWidthActual(),
														GetHeightActual(),
														-1.00*EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD );
										break;
		case( CT_STANDARD_OPENEND ):	CreatePointsRectangleMesh( GetLengthActual(),
														GetWidthActual(),
														GetHeightActual(),
														-1.00*EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD );
										break;
			

		//////////////////////////////////////////////////////////
		//odd shapes
		case( CT_TIE_CLAMP_COLUMN ):	CreatePointsRectangleMesh();		break;
		case( CT_TIE_CLAMP_MASONARY ):	CreatePointsRectangleMesh();		break;
		case( CT_TIE_CLAMP_YOKE ):		CreatePointsRectangleMesh();		break;
		case( CT_TIE_CLAMP_90DEGREE ):	CreatePointsRectangleMesh();		break;
		case( CT_STAIR ):				CreatePointsRectangleMesh();		break;
		case( CT_LADDER ):				CreatePointsRectangleMesh();		break;
		case( CT_HOPUP_BRACKET ):		CreatePointsTriangularMesh();		break;
		case( CT_CORNER_STAGE_BOARD ):	CreatePointsRectangleMesh();		break;
		case( CT_TEXT ):	CreatePointsRectangleMesh();
			//find the centre of the bay
			pt.set( 0.00, 0.00, 0.00 );
			if( GetMatrixElementPointer()==NULL && GetBay()!=NULL )
				pt.set( (GetBay()->GetBayLength()/2.00), (GetBay()->GetBayWidth()/2.00), 0.00 );
			m_ptPoints3D[0] = pt;
			break;
		default:
		case( CT_TIE_BAR ):
		case( CT_STANDARD_CONNECTOR ):
		case( CT_LADDER_PUTLOG ):
		case( CT_STAIR_RAIL ):
		case( CT_STAIR_RAIL_STOPEND ):
		case( CT_TOE_BOARD_CLIP ):
		case( CT_BOM_EXTRA ):
		case( CT_PUTLOG_CLIP ):
		case( CT_VISUAL_COMPONENT ):
			assert( GetVisualComponentsPointer()!=NULL ); //invalid type
			CreatePointsRectangleMesh();		break;
			return false;
		}
	}

	CreateEntity( IsUseWireFrame() );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//CreatePointsRectanglePrism
//
void Component::CreatePointsRectanglePrism()
{
	CreatePointsRectanglePrism( GetLengthActual(), GetWidthActual(), GetHeightActual() );
}


void Component::CreatePointsRectanglePrism(double dLength, double dWidth, double dHeight,
									 double dLengthOffset/*=0.00*/,
									 double dWidthOffset/*=0.00*/,
									 double dHeightOffset/*=0.00*/ )
{
	Point3D pt;
	pt.set( 0.00, dWidth/-2.00, dHeight/-2.00 );

	assert( GetNumberOfPoints()==0 );

	if( GetMatrixElementPointer()==NULL )
	{
		//	_____________________
		//	|\       12          \
		//	|  \ 13              | \9  
		//	|  14\               |   \
		//	|      \___________________\
		//  |       |       3    |      |
		//	|15     |            |10    |
		//	|       |          11|      |
		//	|       |4           |      |
		//	|       |            |      |2
		//	|_______|____________|     8|
		//	 \      |    6        \     |
		//	   \5   |              7\   |
		//       \  |                 \ |
		//(0,0,0)  \|___________________|
		//                        1
		pt.x+= dLengthOffset;
		pt.y+= dWidthOffset;
		pt.z+= dHeightOffset;
		AddPoint( pt );
		pt.x+= dLength;	//1
		AddPoint( pt );
		pt.z+= dHeight;	//2
		AddPoint( pt );
		pt.x-= dLength;	//3
		AddPoint( pt );
		pt.z-= dHeight;	//4
		AddPoint( pt );
		pt.y+= dWidth;	//5
		AddPoint( pt );
		pt.x+= dLength;	//6
		AddPoint( pt );
		pt.y-= dWidth;	//7
		AddPoint( pt );
		pt.z+= dHeight;	//8
		AddPoint( pt );
		pt.y+= dWidth;	//9
		AddPoint( pt );
		pt.z-= dHeight;	//10
		AddPoint( pt );
		pt.z+= dHeight;	//11
		AddPoint( pt );
		pt.x-= dLength;	//12
		AddPoint( pt );
		pt.y-= dWidth;	//13
		AddPoint( pt );
		pt.y+= dWidth;	//14
		AddPoint( pt );
		pt.z-= dHeight;	//15
		AddPoint( pt );
	}
	else
	{
		double	dX, dY;

		if( GetDrawMatrixElementSideOn() )
		{
			dX = dLength;
			dY = dWidth;
			pt.x+= dLengthOffset;
			pt.y+= dWidthOffset;
		}
		else
		{
			dX = dWidth;
			dY = dHeight;
			pt.x+= dWidthOffset;
			pt.y+= dHeightOffset;
		}

		//	___________3_________
		//	|        dX          |
		//  |dY                  |2
		// 4|                    |
		//	|____________________|
		//             1

		AddPoint( pt );
		pt.x+= dX;	//1
		AddPoint( pt );
		pt.y+= dY;	//2
		AddPoint( pt );
		pt.x-= dX;	//3
		AddPoint( pt );
		pt.y-= dY;	//4
		AddPoint( pt );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//CreatePointsTriangularPrism
//
void Component::CreatePointsTriangularPrism()
{
	CreatePointsTriangularPrism( GetLengthActual(), GetWidthActual(), GetHeightActual() );
}

//////////////////////////////////////////////////////////////////////////////////////////
//CreatePointsTriangularPrism
//
void Component::CreatePointsTriangularPrism(double dLength, double dWidth, double dHeight, double dLengthOffset/*=0.00*/, double dWidthOffset/*=0.00*/, double dHeightOffset/*=0.00*/)
{
	Point3D pt;
	pt.set( 0.00, dWidth/-2.00, dHeight/-2.00 );

	assert( GetNumberOfPoints()==0 );

	if( GetMatrixElementPointer()==NULL )
	{
		//	____________________
		//	|\       8         /\
		//	|  \ 7           /   9\ 10
		//	|  3 \         /        \
		//	|      \__________________\
		// 2|      |   /    6        /
		//	|      | /11           /
		//	|      |             /
		//	|    / |           /
		//	|  /   |4        /5
		//	|/     |       /
		//	 \     |     /
		//	   \   |   /
		//     1 \ | /
		//         `  <-(0,0,0)
		pt.x+= dLengthOffset;
		pt.y+= dWidthOffset;
		pt.z+= dHeightOffset;
		AddPoint( pt );
		pt.y+= dWidth;	//1
		AddPoint( pt );
		pt.z+= dHeight;	//2
		AddPoint( pt );
		pt.y-= dWidth;	//3
		AddPoint( pt );
		pt.z-= dHeight;	//4
		AddPoint( pt );
		pt.x+= dLength;	//5a
		pt.z+= dHeight;	//5b
		AddPoint( pt );
		pt.x-= dLength;	//6
		AddPoint( pt );
		pt.y+= dWidth;	//7
		AddPoint( pt );
		pt.x+= dLength;	//8
		AddPoint( pt );
		pt.y-= dWidth;	//9
		AddPoint( pt );
		pt.y+= dWidth;	//10
		AddPoint( pt );
		pt.x-= dLength;	//11a
		pt.z-= dHeight;	//11b
		AddPoint( pt );
	}
	else
	{
		double	dX, dY;

		if( GetDrawMatrixElementSideOn() )
		{
			dX = dLength;
			dY = dHeight;

			//	_______2________
			//	|        dX    /
			//	|            /
			//	|          /
			//	|dY      /
			// 1|      /3
			//	|    /
			//	|  /
			//	|/
			//

			AddPoint( pt );
			pt.y+= dY;	//1
			AddPoint( pt );
			pt.x+= dX;	//2
			AddPoint( pt );
			pt.x-= dX;	//3a
			pt.y-= dY;	//3b
			AddPoint( pt );
		}
		else
		{
			dX = dWidth;
			dY = dHeight;

			//	__2___
			//	| dX |
			//	|    |
			// 1|    |3
			//	|dY  |
			//	|    |
			//	|    |
			//	|____|
			//    4+

			AddPoint( pt );
			pt.y+= dY;	//1
			AddPoint( pt );
			pt.x+= dX;	//2
			AddPoint( pt );
			pt.y-= dY;	//3
			AddPoint( pt );
			pt.x-= dX;	//4
			AddPoint( pt );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//operator 
//
Component & Component::operator =(const Component &Original)
{
	m_ptPoints3D		= Original.m_ptPoints3D;
	m_Nodes				= Original.m_Nodes;
	SetID( ID_INVALID );
	SetLengthCommon( Original.GetLengthCommon() );
	SetType  ( Original.GetType() );
	SetMaterialType( Original.GetMaterialType() );

	SetPreviousLength(  Original.GetLengthCommon() );

	m_eVisible			= Original.m_eVisible;
	m_pLift				= Original.m_pLift;	
	m_pBay				= Original.m_pBay;
	m_pElement			= Original.m_pElement;
	m_pBOMExtra			= Original.m_pBOMExtra;
	m_sStage			= Original.m_sStage;
	m_iLevel			= Original.m_iLevel;
	SetEntity( NULL );
	SetReactor( NULL );
	if( Original.GetEntity()!=NULL )
		CreatePoints();
	Move( Original.GetTransform(), false );
	return *this;
}


void Component::SetAllPointerToNULL()
{
	m_pBay			= NULL;
	m_pLift			= NULL;
	m_pElement		= NULL;
	m_pBOMExtra		= NULL;
	m_pvcComponents	= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
//IsAPointerSet
//confirms if a bay, 
bool Component::IsAPointerSet() const
{
	//JSB todo 990811 - BOMExtra not implented yet, add this once it is written
	if( m_pBay==NULL && m_pLift==NULL && m_pElement==NULL && m_pBOMExtra==NULL && m_pvcComponents==NULL )
		return false;
	return true;
}

BOMExtra *Component::GetBOMExtraPointer() const
{
	return m_pBOMExtra;
}

void Component::DeleteEntity()
{
	Acad::ErrorStatus	es;

	if( GetEntity()!=NULL )
	{
		/////////////////////////////////////////////////////////////
		//Delete the entity
		es = GetEntity()->open();
		if( es==Acad::eOk )
		{
			assert( GetReactor()!=NULL );
			es = GetEntity()->removePersistentReactor( GetReactor()->objectId() );
			assert( es==Acad::eOk );
			es = GetEntity()->erase();
			//assert( es==Acad::eOk );
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
		es = GetEntity()->close();
		assert( es==Acad::eOk );
		SetEntity(NULL);
	}

	if( GetReactor()!=NULL )
	{
		/////////////////////////////////////////////////////////////
		//Delete the reactor
		es = GetReactor()->open(true);
		if( es==Acad::eOk )
		{
			es = GetReactor()->erase();
			assert( es==Acad::eOk );
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
		es = GetReactor()->close();
		assert( es==Acad::eOk );
		SetReactor( NULL );
	}
}

void Component::SetMoveWhenBayLengthChanges(bool bMove/*=true*/ )
{
	m_bMoveWhenBayLengthChanges = bMove;
}

void Component::SetMoveWhenBayWidthChanges(bool bMove/*=true*/ )
{
	m_bMoveWhenBayWidthChanges = bMove;
}

bool Component::GetMoveWhenBayLengthChanges() const
{
	return m_bMoveWhenBayLengthChanges;
}

bool Component::GetMoveWhenBayWidthChanges() const
{
	return m_bMoveWhenBayWidthChanges;
}

void Component::SetTransform(Matrix3D Transform)
{
	m_Transform = Transform;
}

Matrix3D Component::GetTransform() const
{
	return m_Transform;
}

Matrix3D Component::GetLiftMatrix() const
{
	Matrix3D	Trans;

	Trans.setToIdentity();
	if( GetLift()!=NULL )
	{
		Trans = GetLift()->GetTransform();
	}
	return Trans;
}

Matrix3D Component::GetBayMatrix() const
{
	return GetBay()->GetTransform();
}

Matrix3D Component::GetOtherMatrix() const
{
	Matrix3D Trans;
	Trans = GetLiftMatrix();
	Trans = GetBayMatrix() * Trans;
	return Trans;
}

Matrix3D Component::UnMove()
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

/*****************************************************************
 *	History Records
 *****************************************************************
 * $History: Component.cpp $ 
 * 
 * *****************  Version 189  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 188  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 187  *****************
 * User: Jsb          Date: 22/11/00   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * I have now got WBlock operational, also I have fixed Andrew Smith's 3D
 * components to new layer bug fixed.
 * 
 * *****************  Version 186  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 185  *****************
 * User: Jsb          Date: 16/11/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * About to release KwikScaf version 1.1k
 * 
 * *****************  Version 184  *****************
 * User: Jsb          Date: 13/11/00   Time: 4:20p
 * Updated in $/Meccano/Stage 1/Code
 * About to work from home
 * 
 * *****************  Version 183  *****************
 * User: Jsb          Date: 9/11/00    Time: 12:17p
 * Updated in $/Meccano/Stage 1/Code
 * Building r1.1j
 * 
 * *****************  Version 182  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 181  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 180  *****************
 * User: Jsb          Date: 20/10/00   Time: 4:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 179  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 178  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 177  *****************
 * User: Jsb          Date: 10/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 176  *****************
 * User: Jsb          Date: 10/10/00   Time: 10:22a
 * Updated in $/Meccano/Stage 1/Code
 * Building R1.1f (Version 1.5.1.6) which has the BOM with Stage/Level bug
 * fixed
 * 
 * *****************  Version 175  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 174  *****************
 * User: Jsb          Date: 29/09/00   Time: 9:09a
 * Updated in $/Meccano/Stage 1/Code
 * Just about to change the mills & kwikscaf _tsystem from bool to enum
 * 
 * *****************  Version 173  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:49p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 172  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 171  *****************
 * User: Jsb          Date: 21/09/00   Time: 3:25p
 * Updated in $/Meccano/Stage 1/Code
 * built 1.0m
 * 
 * *****************  Version 170  *****************
 * User: Jsb          Date: 20/09/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Acad rotation is now working with individual components just need to
 * store b4 release to WA
 * 
 * *****************  Version 169  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 168  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 166  *****************
 * User: Jsb          Date: 14/09/00   Time: 9:40a
 * Updated in $/Meccano/Stage 1/Code
 * About to Rewind back to 1.0h (the one after UK release) since we need
 * to send a verision Australia wide
 * 
 * *****************  Version 165  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 164  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 163  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 162  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 161  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 160  *****************
 * User: Jsb          Date: 31/08/00   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 159  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 158  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 157  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 155  *****************
 * User: Jsb          Date: 11/08/00   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed crash where we couldn't _topen any previous drawings thanks to
 * AGT's change of fonts
 * 
 * *****************  Version 154  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 153  *****************
 * User: Jsb          Date: 7/08/00    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 152  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 151  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:22p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 150  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 2/08/00    Time: 10:31a
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8s
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 25/07/00   Time: 5:07p
 * Updated in $/Meccano/Stage 1/Code
 * Column Ties are not oriented correctly
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 25/07/00   Time: 1:09p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix with exact positioning is not finished
 * 
 * *****************  Version 142  *****************
 * User: Jsb          Date: 24/07/00   Time: 1:38p
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to B4 3D position exact started
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 21/07/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly completed the SetDecks to Levels bug
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
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 7/07/00    Time: 7:50a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 136  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
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
 * User: Jsb          Date: 16/06/00   Time: 3:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 8/06/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Rotation of Bays is operational, rotation of laps is nearly working
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 20/04/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 20/04/00   Time: 12:53p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 14/04/00   Time: 9:57a
 * Updated in $/Meccano/Stage 1/Code
 * Colour By Stage & level
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 31/03/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 30/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 10/03/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 25/02/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 15/02/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 11/02/00   Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC2
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 11/02/00   Time: 9:12a
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the save and load bug in the stages & levels stuff
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 10/02/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * I think I have complete the JM bug
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 10/02/00   Time: 10:56a
 * Updated in $/Meccano/Stage 1/Code
 * Just trying to get the committed stuff working
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 9/02/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 28/01/00   Time: 2:56p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.13
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 15/12/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Adjusting RL's nearly finished, just need a dialogbox or other UI to do
 * it!
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 6/12/99    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * See upto here comment
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 6/12/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * Got everything working as well as I had them at home
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 18/11/99   Time: 11:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:53a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 28/10/99   Time: 3:53p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Shade cloth and chain link now operational!
 * 2) Crash on setting TWH fixed
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 5/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * Added the PartNumber to the ComponentDetails
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 22/09/99   Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:01a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 17/09/99   Time: 12:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 16/09/99   Time: 9:47a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 13/09/99   Time: 4:00p
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
 * User: Jsb          Date: 8/09/99    Time: 3:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
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
 * *****************  Version 45  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 9/01/99    Time: 9:22a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on the cleanup operation
 * 
 * *****************  Version 43  *****************
 * User: Dar          Date: 8/31/99    Time: 3:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
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
 * User: Jsb          Date: 8/19/99    Time: 3:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/19/99    Time: 2:55p
 * Updated in $/Meccano/Stage 1/Code
 * fixed the soleboard size error
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/19/99    Time: 1:40p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/13/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/12/99    Time: 7:20p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the layers, visibility of autobuild, tight fit bay working
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/12/99    Time: 2:50p
 * Updated in $/Meccano/Stage 1/Code
 * Updating for andrews build
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/12/99    Time: 1:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/12/99    Time: 10:32a
 * Updated in $/Meccano/Stage 1/Code
 * 3d view working for the most part
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/10/99    Time: 12:19p
 * Updated in $/Meccano/Stage 1/Code
 * Added the Entity class
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 12  *****************
 * User: Dar          Date: 3/08/99    Time: 5:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Dar          Date: 2/08/99    Time: 3:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 29/07/99   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 28/07/99   Time: 12:40p
 * Updated in $/Meccano/Stage 1/Code
 * Converted _T("Get()") functions to _T("Get() const") functions
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
 * User: Dar          Date: 20/07/99   Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * compilible group of classes componentList, componentListElement, Lift,
 * Component
 * 
 *****************************************************************/

//////////////////////////////////////////////////////////////////
//Has the been marked for deletion
//////////////////////////////////////////////////////////////////
//Says this object needs to be deleted
DirtyFlagEnum	Component::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

//set the object to be deleted
void	Component::SetDirtyFlag( DirtyFlagEnum dfDirtyFlag )
{
	if( dfDirtyFlag==DF_CLEAN )
	{
		//set the dirty flag
		m_dfDirtyFlag = dfDirtyFlag;
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
	if( GetLift()!=NULL )
		GetLift()->SetDirtyFlag( DF_DIRTY );
	else if( GetBay()!=NULL )
		GetBay()->SetDirtyFlag( DF_DIRTY );
	else if( GetVisualComponentsPointer()!=NULL )
		GetVisualComponentsPointer()->SetDirtyFlag( DF_DIRTY );
	else
		assert( false );
}


void Component::CreatePointsIfPossible()
{
//	if( GetEntity()!=NULL || GetController()->IsCrosshairCreated() )
	if( GetEntity()!=NULL )
		CreatePoints();
}

Controller *Component::GetController() const
{
	if( GetLiftPointer()!=NULL )
	{
		return GetLiftPointer()->GetBayPointer()->GetController();
	}
	else if( GetBayPointer()!=NULL )
	{
		return GetBayPointer()->GetController();
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		return GetMatrixElementPointer()->GetController();
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		return GetMatrixElementPointer()->GetController();
	}
	else if( GetVisualComponentsPointer()!=NULL )
	{
		return GetVisualComponentsPointer()->GetController();
	}
	else if( m_pController!=NULL )
	{
		return m_pController;
	}
	else
	{
		assert( false );
	}
	return NULL;
}

Run * Component::GetRun() const
{
	if( GetLiftPointer()!=NULL )
	{
		return GetLiftPointer()->GetBayPointer()->GetRunPointer();
	}
	if( GetBayPointer()!=NULL )
	{
		return GetBayPointer()->GetRunPointer();
	}
	return NULL;
}

Bay * Component::GetBay() const
{
	if( GetLiftPointer()!=NULL )
	{
		return GetLiftPointer()->GetBayPointer();
	}
	return GetBayPointer();
}

Lift * Component::GetLift() const
{
	return GetLiftPointer();
}

void Component::SetText(CString &sText)
{
	Acad::ErrorStatus	es;

	if( GetEntity()!=NULL )
	{
		assert( GetType()==CT_TEXT );	//must be text here

		GetEntity()->open();
		es = ((AcDbText*)GetEntity())->setTextString( sText );
		GetEntity()->close();

	}
	m_sText = sText;
}

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// Test DWG files below
// Files data in from a DWG file.
//
Acad::ErrorStatus
Component::dwgInFields(AcDbDwgFiler* pFiler)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);

    pFiler->readItem(&m_dLength);
//    pFiler->readItem(&m_eComponentType);

    return pFiler->filerStatus();
}

// Files data out to a DWG file.
//
Acad::ErrorStatus
Component::dwgOutFields(AcDbDwgFiler* pFiler) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);

    pFiler->writeItem(m_dLength);
//    pFiler->writeItem(m_eComponentType);
    return pFiler->filerStatus();
}




void Component::SetEntity(Entity *pEntity)
{
	if( pEntity!=NULL && m_pEnt!=NULL )
	{
		//we have not deleted the previous one!
		assert( false );
	}
	m_pEnt = pEntity;
}

Entity * Component::GetEntity() const
{
	return m_pEnt;
}

void Component::SetReactor(EntityReactor *pReactor)
{
	m_pReactor = pReactor;
}

EntityReactor *Component::GetReactor() const
{
	return m_pReactor;
}



void Component::SetDrawMatrixElementSideOn(bool bSideOn)
{
	m_DrawMatrixElementSideOn = bSideOn; 
}

bool Component::GetDrawMatrixElementSideOn()
{
	return m_DrawMatrixElementSideOn;
}

///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void Component::Serialize(CArchive &ar)
{
	int		iPointIndex, iSystem;
	int		iNumberOfPoints;
	int		iComponentType;	// ComponentTypeEnum //plank, transom, etc	
	int		iMaterialType;	// MaterialTypeEnum //steel wood etc
	int		iVisible;	// VisibilityEnum //steel wood etc
	bool	bCommitted, bDummy;
	bool	bMills;
	BOOL	BTemp;
	CString sTemp, sMsg, sPN;
	Point3D ptPoint;

	if (ar.IsStoring())    // Store Object?
	{
		ar << COMPONENT_VERSION_LATEST;

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_6
		if( GetStockDetails()==NULL )
			SetStockDetailsPointer();
		if( GetStockDetails()==NULL )
			SetStockDetailsPointerDumb( GetController()->GetStockList()->GetMatchingComponent( GetText(), false ) );
		assert( GetStockDetails()!=NULL );

		sPN = GetStockDetails()->GetPartNumber();
		ar << sPN;

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_5
		iSystem = (int)GetSystem();
		ar << iSystem;

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_4
		bDummy = GetRemoveComponentFromBOM();
		STORE_bool_IN_AR( bDummy );

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_3
		bMills = GetSystem()==S_MILLS;
		STORE_bool_IN_AR( bMills );

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_2
		sTemp = GetText();
		ar << sTemp;

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_1
		STORE_bool_IN_AR( m_bCommitted );

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_0 
		iNumberOfPoints = m_ptPoints3D.logicalLength(); //  get number of points in array
		ar << iNumberOfPoints; // store number of 3d points
		for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // store points
		{
			ar << m_ptPoints3D[iPointIndex].x;
			ar << m_ptPoints3D[iPointIndex].y;
			ar << m_ptPoints3D[iPointIndex].z;
		}

		iNumberOfPoints = m_Nodes.logicalLength(); //  get number of points in array
		ar << iNumberOfPoints; // store number of 3d points
		for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // store points
		{
			ar << m_Nodes[iPointIndex].x;
			ar << m_Nodes[iPointIndex].y;
			ar << m_Nodes[iPointIndex].z;
		}

		ar << m_dLength;
		ar << m_eComponentType; // ComponentTypeEnum //plank, transom, etc	
		ar << m_eMaterialType; // MaterialTypeEnum	//steel wood etc
		ar << m_eVisible; // VisibilityEnum			

		//Stage/Level
		ar << m_sStage;

		//We store this as a string for historical reasons
		sTemp.Format(_T("%i"), m_iLevel );
		ar << sTemp;

		m_Transform.Serialize(ar);

		STORE_bool_IN_AR(m_DrawMatrixElementSideOn);
		STORE_bool_IN_AR(m_bMoveWhenBayWidthChanges);
		STORE_bool_IN_AR(m_bMoveWhenBayLengthChanges);
		ar << GetPreviousLength();
	}
	else					// or Load Object?
	{
		SetSystem( gpController->GetSystem() );
		sTemp.Empty();
		SetText(sTemp);
		bCommitted = false;
		SetRemoveComponentFromBOM(false);
		iSystem = (int)S_INVALID;
		sPN.Empty();

		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case COMPONENT_VERSION_1_0_6 :
			ar >> sPN;

		case COMPONENT_VERSION_1_0_5 :
			ar >> iSystem;
			//fallthrough

		case COMPONENT_VERSION_1_0_4 :
			LOAD_bool_IN_AR(bDummy);
			SetRemoveComponentFromBOM(bDummy);
			//fallthrough

		case COMPONENT_VERSION_1_0_3 :
			LOAD_bool_IN_AR(bMills);

			if( GetController()!=NULL && GetController()->GetConvertToSystemOnNextOpen() )
			{
				//On the next load set this component to the controller's system value
				iSystem = GetController()->GetSystem();
			}

			if( iSystem<=S_INVALID )
			{
				//This is prior to COMPONENT_VERSION_1_0_5, so use the bMills
				SetSystem( (bMills)? S_MILLS: S_KWIKSTAGE );
			}
			else
			{
				//This is a new file than COMPONENT_VERSION_1_0_5, so us iSystem
				SetSystem( (SystemEnum)iSystem );
			}

			//fallthrough

		case COMPONENT_VERSION_1_0_2 :
			ar >> sTemp;
			SetText(sTemp);
			//fallthrough

		case COMPONENT_VERSION_1_0_1 :
			LOAD_bool_IN_AR(bCommitted);

			//fallthrough
		case COMPONENT_VERSION_1_0_0 :
			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_ptPoints3D.append(ptPoint);
			}

			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_Nodes.append(ptPoint);
			}

			ar >> m_dLength;

			ar >> iComponentType;	// ComponentTypeEnum //plank, transom, etc	
			m_eComponentType = (ComponentTypeEnum)iComponentType;

			ar >> iMaterialType;	// MaterialTypeEnum //steel wood etc
			m_eMaterialType = (MaterialTypeEnum)iMaterialType;

			ar >> iVisible;	// VisibilityEnum //steel wood etc
			m_eVisible = (VisibilityEnum)iVisible;

			//Stage/Level
			ar >> m_sStage;
			ar >> sTemp;
			m_iLevel = _ttoi(sTemp);

			m_Transform.Serialize(ar);

			LOAD_bool_IN_AR(m_DrawMatrixElementSideOn);
			LOAD_bool_IN_AR(m_bMoveWhenBayWidthChanges);
			LOAD_bool_IN_AR(m_bMoveWhenBayLengthChanges);
			double dLength;
			ar >> dLength;
			SetPreviousLength( dLength );
			m_bCommitted = bCommitted;
			break;
		default:
			assert( false );
			if( uiVersion>COMPONENT_VERSION_LATEST )
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
      sMsg+= _T("Class: Component.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), COMPONENT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
		SetStockDetailsPointer();

		if( GetStockDetails()==NULL && !sPN.IsEmpty() )
		{
			SetStockDetailsPointerDumb( GetController()->GetStockList()->GetMatchingComponent( sPN, false ) );
		}
		assert( GetStockDetails()!=NULL );
	}
}
/*
void Component::Serialize(CArchive &ar)
{
	int		iPointIndex, iSystem;
	int		iNumberOfPoints;
	bool	bCommitted, bDummy;
	bool	bMills;
	BOOL	BTemp;
	CString sTemp, sMsg, sPN;
	Point3D ptPoint;

	if (ar.IsStoring())    // Store Object?
	{
		ar << COMPONENT_VERSION_LATEST;

		///////////////////////////////////////////////////////
		//COMPONENT_VERSION_1_0_7
		if( GetStockDetails()==NULL )
			SetStockDetailsPointer();
		if( GetStockDetails()==NULL )
			SetStockDetailsPointerDumb( GetController()->GetStockList()->GetMatchingComponent( GetText(), false ) );
		assert( GetStockDetails()!=NULL );

		sPN = GetStockDetails()->GetPartNumber();
		ar << sPN;

		iSystem = (int)GetSystem();
		ar << iSystem;

		bDummy = GetRemoveComponentFromBOM();
		STORE_bool_IN_AR( bDummy );

		sTemp = GetText();
		ar << sTemp;

		STORE_bool_IN_AR( m_bCommitted );

		iNumberOfPoints = m_ptPoints3D.logicalLength(); //  get number of points in array
		ar << iNumberOfPoints; // store number of 3d points
		for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // store points
		{
			ar << m_ptPoints3D[iPointIndex].x;
			ar << m_ptPoints3D[iPointIndex].y;
			ar << m_ptPoints3D[iPointIndex].z;
		}

		iNumberOfPoints = m_Nodes.logicalLength(); //  get number of points in array
		ar << iNumberOfPoints; // store number of 3d points
		for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // store points
		{
			ar << m_Nodes[iPointIndex].x;
			ar << m_Nodes[iPointIndex].y;
			ar << m_Nodes[iPointIndex].z;
		}

		ar << m_dLength;
		ar << m_eComponentType; // ComponentTypeEnum //plank, transom, etc	
		ar << m_eMaterialType; // MaterialTypeEnum	//steel wood etc
		ar << m_eVisible; // VisibilityEnum			

		//Stage/Level
		ar << m_sStage;
		//We store this as a string for historical reasons
		ar << m_iLevel;

		m_Transform.Serialize(ar);

		STORE_bool_IN_AR(m_DrawMatrixElementSideOn);
		STORE_bool_IN_AR(m_bMoveWhenBayWidthChanges);
		STORE_bool_IN_AR(m_bMoveWhenBayLengthChanges);
		ar << GetPreviousLength();
	}
	else					// or Load Object?
	{
		int		iComponentType;	// ComponentTypeEnum //plank, transom, etc	
		int		iMaterialType;	// MaterialTypeEnum //steel wood etc
		int		iVisible;	// VisibilityEnum //steel wood etc
		double	dLength;

		VersionNumber uiVersion;
		ar >> uiVersion;

#ifdef _DEBUG
		giaLoopCounter[0]++;
#endif	//_DEBUG

		if( uiVersion<COMPONENT_VERSION_1_0_7 )
		{
			assert( gpController!=NULL );
			SetSystem( gpController->GetSystem() );
			sTemp.Empty();
			SetText(sTemp);
			bCommitted = false;
			SetRemoveComponentFromBOM(false);
			iSystem = (int)S_INVALID;
			sPN.Empty();
		}

		switch (uiVersion)
		{
		case COMPONENT_VERSION_1_0_7 :
			ar >> sPN;

			ar >> iSystem;
			if( iSystem<=S_INVALID )
			{
				iSystem = GetController()->GetSystem();
			}
			SetSystem( (SystemEnum)iSystem );

			LOAD_bool_IN_AR(bDummy);
			SetRemoveComponentFromBOM(bDummy);

			ar >> sTemp;
			SetText(sTemp);

			LOAD_bool_IN_AR(bCommitted);

			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_ptPoints3D.append(ptPoint);
			}

			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_Nodes.append(ptPoint);
			}

			ar >> m_dLength;

			ar >> iComponentType;	// ComponentTypeEnum //plank, transom, etc	
			m_eComponentType = (ComponentTypeEnum)iComponentType;

			ar >> iMaterialType;	// MaterialTypeEnum //steel wood etc
			m_eMaterialType = (MaterialTypeEnum)iMaterialType;

			ar >> iVisible;	// VisibilityEnum //steel wood etc
			m_eVisible = (VisibilityEnum)iVisible;

			//Stage/Level
			ar >> m_sStage;
			ar >> m_iLevel;

			m_Transform.Serialize(ar);

			LOAD_bool_IN_AR(m_DrawMatrixElementSideOn);
			LOAD_bool_IN_AR(m_bMoveWhenBayWidthChanges);
			LOAD_bool_IN_AR(m_bMoveWhenBayLengthChanges);
			ar >> dLength;
			SetPreviousLength( dLength );
			m_bCommitted = bCommitted;

			//don't fall throught
			break;

		case COMPONENT_VERSION_1_0_6 :
			ar >> sPN;
			//fallthrough

		case COMPONENT_VERSION_1_0_5 :
			ar >> iSystem;
			//fallthrough

		case COMPONENT_VERSION_1_0_4 :
			LOAD_bool_IN_AR(bDummy);
			SetRemoveComponentFromBOM(bDummy);
			//fallthrough

		case COMPONENT_VERSION_1_0_3 :
			LOAD_bool_IN_AR(bMills);

			if( gpController->GetConvertToSystemOnNextOpen() )
			{
				//On the next load set this component to the controller's system value
				iSystem = GetController()->GetSystem();
			}

			if( iSystem<=S_INVALID )
			{
				//This is prior to COMPONENT_VERSION_1_0_5, so use the bMills
				SetSystem( (bMills)? S_MILLS: S_KWIKSTAGE );
			}
			else
			{
				//This is a new file than COMPONENT_VERSION_1_0_5, so us iSystem
				SetSystem( (SystemEnum)iSystem );
			}

			//fallthrough

		case COMPONENT_VERSION_1_0_2 :
			ar >> sTemp;
			SetText(sTemp);
			//fallthrough

		case COMPONENT_VERSION_1_0_1 :
			LOAD_bool_IN_AR(bCommitted);

			//fallthrough
		case COMPONENT_VERSION_1_0_0 :
			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_ptPoints3D.append(ptPoint);
			}

			ar >> iNumberOfPoints; // load number of 3d points
			for( iPointIndex=0; iPointIndex < iNumberOfPoints; iPointIndex++ ) // load points
			{
				ar >> ptPoint.x;
				ar >> ptPoint.y;
				ar >> ptPoint.z;
				m_Nodes.append(ptPoint);
			}

			ar >> m_dLength;

			ar >> iComponentType;	// ComponentTypeEnum //plank, transom, etc	
			m_eComponentType = (ComponentTypeEnum)iComponentType;

			ar >> iMaterialType;	// MaterialTypeEnum //steel wood etc
			m_eMaterialType = (MaterialTypeEnum)iMaterialType;

			ar >> iVisible;	// VisibilityEnum //steel wood etc
			m_eVisible = (VisibilityEnum)iVisible;

			//Stage/Level
			ar >> m_sStage;
			ar >> sTemp;
			m_iLevel = _ttoi(sTemp);

			m_Transform.Serialize(ar);

			LOAD_bool_IN_AR(m_DrawMatrixElementSideOn);
			LOAD_bool_IN_AR(m_bMoveWhenBayWidthChanges);
			LOAD_bool_IN_AR(m_bMoveWhenBayLengthChanges);
			double dLength;
			ar >> dLength;
			SetPreviousLength( dLength );
			m_bCommitted = bCommitted;
			break;
		default:
			assert( false );
			if( uiVersion>COMPONENT_VERSION_LATEST )
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
			sMsg+= _T("Class: Component.\n");
			sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), COMPONENT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
		SetStockDetailsPointer();

		if( GetStockDetails()==NULL && !sPN.IsEmpty() )
		{
			SetStockDetailsPointerDumb( GetController()->GetStockList()->GetMatchingComponent( sPN, false ) );
		}
		assert( GetStockDetails()!=NULL );
	}
}
*/

void Component::SetTextHorizontalMode( AcDb::TextHorzMode HorzMode)
{
	m_TextHAlign = HorzMode;
}

void Component::SetColour( int iColour )
{
	m_iColour = iColour;
}

void Component::SetTextVerticalMode( AcDb::TextVertMode VertMode )
{
	m_TextVAlign = VertMode;
}

void Component::SetTextHeightMultiplier(double dMultiplier)
{
	m_dTextHeightMultiplier = dMultiplier;
}

UseActualComponentsEnum Component::IsUseWireFrame()
{
	return GetController()->GetUseActualComponents();
}

void Component::SetController(Controller *pController)
{
	m_pController = pController;
}

double Component::GetPreviousLength()
{
	return m_dPreviousLength;
}

void Component::SetPreviousLength(double dLength)
{
	m_dPreviousLength = dLength;
}

int Component::GetRise() const
{
	if( GetLiftPointer()!=NULL )
	{
		return GetLiftPointer()->GetComponentRise( GetID() );
	}
	else if ( GetBayPointer()!=NULL )
	{
		return GetBayPointer()->GetComponentsRise( GetID() );
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		assert( false );
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		assert( false );
	}
	else if( m_pController!=NULL )
	{
		assert( false );
	}
	return (int)LIFT_RISE_INVALID;
}

SideOfBayEnum Component::GetSideOfBay( ) const
{
	if( GetLiftPointer()!=NULL )
	{
		return GetLiftPointer()->GetPosition( GetID() );
	}
	else if ( GetBayPointer()!=NULL )
	{
		return GetBayPointer()->GetPosition( GetID() );
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		return GetMatrixElementPointer()->GetSideOfBay( this );
	}
	else if( m_pController!=NULL )
	{
		assert( false );
	}
	return SIDE_INVALID;
}

int Component::GetLevelForRL(double dRelativeLevel) const
{
	return GetController()->GetLevelList()->GetLevelForRL(dRelativeLevel);
}

bool Component::IsCommitted()
{
	return m_bCommitted;
}

void Component::SetCommitted(bool bCommitted)
{
	m_bCommitted = bCommitted;

	if( GetBay()!=NULL &&
		GetType()!=CT_SHADE_CLOTH &&
		GetType()!=CT_CHAIN_LINK )
	{
		#ifdef	_DEBUG
    acutPrintf( _T("\nCommitting Bay:%i Level:%i"), GetBay()->GetBayNumber(), GetLevel() );
		#endif	//#ifdef	_DEBUG
		if( GetBay()->GetBayType() == BAY_TYPE_LAPBOARD )
		{
			LapboardBay	*pLap;
			pLap = (LapboardBay	*)GetBay();
			GetController()->GetBayStageLevelArray()->SetCommitted( pLap->GetBayWest()->GetBayNumber(), GetLevel(), GetStage(), m_bCommitted );
		}
		else
		{
			GetController()->GetBayStageLevelArray()->SetCommitted( GetBay()->GetBayNumber(), GetLevel(), GetStage(), m_bCommitted );
		}
	}
}

void Component::CreatePointsRectangleMesh()
{
	CreatePointsRectangleMesh( GetLengthActual(true), GetWidthActual(false), GetHeightActual(false) );
}

void Component::CreatePointsRectangleMesh(double dLength, double dWidth, double dHeight,
									double dLengthOffset/*=0.00*/,
									double dWidthOffset/*=0.00*/,
									double dHeightOffset/*=0.00*/)
{
	Point3D pt;

	assert( GetNumberOfPoints()==0 );

	if( GetMatrixElementPointer()==NULL )
	{
		// MxN = 6x3
		//	_____________________
		//	|\[2,0]              \[3,0]
		//	|  \[5,0],[5,1]      | \[4,0],[4,1]
		//	|    \               |   \
		//	|      \___________________\
		//  |       |[1,0]       |      |[0,0]
		//	|       |[5,2]       |      |[4,2]
		//	|       |            |      |
		//	|       |            |      |
		//	|       |            |      |
		//	|_______|____________|[3,1] |
		//	 \[2,1] |             \     |
		//	   \    |               \   |
		//       \  |                 \ |
		//         \|___________________|[0,1],[0,2]
		//        [1,1],[1,2]         [3,2]
		//        [2,2]

		pt.set( 0.00, dWidth/-2.00, dHeight/-2.00 );
		pt.x+= dLengthOffset;
		pt.y+= dWidthOffset;
		pt.z+= dHeightOffset;
		pt.x+= dLength;
		pt.z+= dHeight;

		AddPoint( pt );	//[0,1];
		pt.z-= dHeight;
		AddPoint( pt );	//[0,1];
		AddPoint( pt );	//[0,2];

		pt.x-= dLength;
		pt.z+= dHeight;
		AddPoint( pt );	//[1,0];
		pt.z-= dHeight;
		AddPoint( pt );	//[1,1];
		AddPoint( pt );	//[1,2];

		pt.y+= dWidth;
		pt.z+= dHeight;
		AddPoint( pt );	//[2,0];
		pt.z-= dHeight;
		AddPoint( pt );	//[2,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[2,2];

		pt.x+= dLength;
		pt.y+= dWidth;
		pt.z+= dHeight;
		AddPoint( pt );	//[3,0];
		pt.z-= dHeight;
		AddPoint( pt );	//[3,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[3,2];

		pt.z+= dHeight;
		pt.y+= dWidth;
		AddPoint( pt );	//[4,0];
		AddPoint( pt );	//[4,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[4,2];

		pt.x-= dLength;
		pt.y+= dWidth;
		AddPoint( pt );	//[5,0];
		AddPoint( pt );	//[5,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[5,2];
	}
	else
	{
		double	dX, dY;

		if( GetDrawMatrixElementSideOn() )
		{
			pt.set( 0.00, dHeight/-2.00, 0.00 );
			dX = dLength;
			dY = dWidth;
			pt.x+= dLengthOffset;
			pt.y+= dWidthOffset;
		}
		else
		{
			pt.set( dWidth/-2.00, dHeight/-2.00, 0.00 );
			dX = dWidth;
			dY = dHeight;
			pt.x+= dWidthOffset;
			pt.y+= dHeightOffset;
		}

		//[1,0]                 [1,1]
		//	______________________
		//	|        dX          |
		//  |dY                  | 
		//  |                    |
		//	|____________________|
		//[0,0]                 [0,1]

		AddPoint( pt );	//[0,0]
		pt.x+= dX;	
		AddPoint( pt );	//[0,1];
		pt.x-= dX;	
		pt.y+= dY;
		AddPoint( pt );	//[1,0];
		pt.x+= dX;	
		AddPoint( pt );	//[1,1];
	}
}

void Component::CreatePointsTriangularMesh()
{
	CreatePointsTriangularMesh( GetLengthActual(), GetWidthActual(), GetHeightActual() );
}

void Component::CreatePointsTriangularMesh(double dLength, double dWidth, double dHeight, double dLengthOffset, double dWidthOffset, double dHeightOffset)
{
	Point3D pt;

	assert( GetNumberOfPoints()==0 );

	if( GetMatrixElementPointer()==NULL )
	{
		//[1,1],[1,2]
		//	 __________________________[0,0],[0,1],[0,2]
		//	|\[2,2]                   /\[3,2],[4,2]
		//	|  \                    /    \   
		//	|    \                /        \
		//	|      \ [2,1]      /            \
		//  |        \_________________________\[3,0],[3,1]
		//	|         |[5,2]/                  /[4,0],[4,1]
		//	|         |   /                  /
		//	|         | /                  /
		//	|         |                  / 
		//	|       / |                /
		//	|     /   |              /
		//	|   /     |            /
		//  | /[1,0]  |          /
		//   \        |        /   
		//     \      |      /
		//       \    |    /
		//         \  |  /
		//           \|/[2,0]
		//           [5,0],[5,1]
		pt.set( 0.00, dWidth/-2.00, dHeight/-2.00 );
		pt.x+= dLengthOffset;
		pt.y+= dWidthOffset;
		pt.z+= dHeightOffset;

		pt.x+= dLength;
		pt.y+= dWidth;
		pt.z+= dHeight;
		AddPoint( pt ); //[0,0];
		AddPoint( pt );	//[0,1];
		AddPoint( pt );	//[0,2];

		pt.x-= dLength;
		pt.z-= dHeight;
		AddPoint( pt );	//[1,0];
		pt.z+= dHeight;
		AddPoint( pt );	//[1,1];
		AddPoint( pt );	//[1,2];

		pt.y-= dWidth;
		pt.z-= dHeight;
		AddPoint( pt );	//[2,0];
		pt.z+= dHeight;
		AddPoint( pt );	//[2,1];
		pt.y+= dWidth;
		AddPoint( pt );	//[2,2];

		pt.y-= dWidth;
		pt.x+= dLength;
		AddPoint( pt );	//[3,0];
		AddPoint( pt );	//[3,1];
		pt.y+= dWidth;
		AddPoint( pt );	//[3,2];

		pt.y-= dWidth;
		AddPoint( pt );	//[4,0];
		AddPoint( pt );	//[4,1];
		pt.y+= dWidth;
		AddPoint( pt );	//[4,2];

		pt.x-= dLength;
		pt.y-= dWidth;
		pt.z-= dHeight;
		AddPoint( pt );	//[5,0];
		AddPoint( pt );	//[5,1];
		pt.y+= dWidth;
		AddPoint( pt );	//[5,2];


		//Alternatively
		//
		//[2,0],[5,0]
		//	 __________________________[3,0],[3,1],[4,0]
		//	|\[5,1]                   /\[4,1]
		//	|  \                    /    \   
		//	|    \                /        \
		//	|      \ [5,2]      /            \
		//  |        \_________________________\[0,0],[0,1]
		//	|         |[1,0]/                  /[0,2],[3,2],[4,2]
		//	|         |   /                  /
		//	|         | /                  /
		//	|         |                  / 
		//	|       / |                /
		//	|     /   |              /
		//	|   /     |            /
		//  | /[2,1]  |          /
		//   \        |        /   
		//     \      |      /
		//       \    |    /
		//         \  |  /
		//           \|/[1,1]
		//           [1,2],[2,2]
/*		pt.x+= dLengthOffset;
		pt.y+= dWidthOffset;
		pt.z+= dHeightOffset;
		pt.x+= dLength;
		pt.z+= dHeight;

		AddPoint( pt );	//[0,0];
		AddPoint( pt );	//[0,1];
		AddPoint( pt );	//[0,2];

		pt.x-= dLength;
		AddPoint( pt );	//[1,0];
		pt.z-= dHeight;
		AddPoint( pt );	//[1,1];
		AddPoint( pt );	//[1,2];

		pt.y+= dWidth;
		pt.z+= dHeight;
		AddPoint( pt );	//[2,0];
		pt.z-= dHeight;
		AddPoint( pt );	//[2,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[2,2];

		pt.x+= dLength;
		pt.y+= dWidth;
		pt.z+= dHeight;
		AddPoint( pt );	//[3,0];
		AddPoint( pt );	//[3,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[3,2];

		pt.y+= dWidth;
		AddPoint( pt );	//[4,0];
		AddPoint( pt );	//[4,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[4,2];

		pt.x-= dLength;
		pt.y+= dWidth;
		AddPoint( pt );	//[5,0];
		AddPoint( pt );	//[5,1];
		pt.y-= dWidth;
		AddPoint( pt );	//[5,2];
*/
	}
	else
	{
		double	dX, dY;

		if( GetDrawMatrixElementSideOn() )
		{
			pt.set( 0.00, dHeight/-2.00, 0.00 );
			dX = dLength;
			dY = dHeight;
			pt.x+= dLengthOffset;
			pt.y+= dHeightOffset;

			//	________________
			//	|[1,0]   dX    /[1,1]
			//	|            /
			//	|          /
			//	|dY      /
			//  |      / 
			//	|    /
			//	|  /
			//	|/
			//[0,0],[0,1]

			AddPoint( pt );	//[0,0];
			AddPoint( pt );	//[0,1];
			pt.y+= dY;
			AddPoint( pt );	//[1,0];
			pt.x+= dX;
			AddPoint( pt );	//[1,1];
		}
		else
		{
			pt.set( dWidth/-2.00, dHeight/-2.00, 0.00 );
			dX = dWidth;
			dY = dHeight;
			pt.x+= dWidthOffset;
			pt.y+= dHeightOffset;

			//[1,0] [1,1]
			//	______
			//	| dX |
			//	|    |
			//  |    | 
			//	|dY  |
			//	|    |
			//	|    |
			//	|____|
			//[0,0]  [0,1]

			AddPoint( pt );	//[0,0]
			pt.x+= dX;	
			AddPoint( pt );	//[0,1];
			pt.x-= dX;	
			pt.y+= dY;
			AddPoint( pt );	//[1,0];
			pt.x+= dX;	
			AddPoint( pt );	//[1,1];
		}
	}
}


CString Component::GetText()
{
	return m_sText;
}

CString Component::GetTextSmart()
{
	CString sText;

	if( !m_sText.IsEmpty() )
	{
		sText = m_sText;
	}
	else
	{
		if( GetMatrixElementPointer()!=NULL )
		{
			double dHeight;

			dHeight = GetMatrixElementPointer()->GetHeight( MATRIX_STANDARDS_INNER );
			dHeight = ConvertStarRLtoRL( dHeight, GetStarSeparation() );
			dHeight*= CONVERT_MM_TO_M;
			if( GetMatrixElementPointer()->GetMatrixPointer()!=NULL &&
				!GetMatrixElementPointer()->GetMatrixPointer()->AllSameSystem() )
			{
				if( GetMatrixElementPointer()->GetSystem()==S_MILLS )
          sText.Format( _T("%s: %1.1fm <%s>"), GetMatrixElementPointer()->GetLabel(), dHeight, SYSTEM_TEXT_MILLS );
				else
          sText.Format( _T("%s: %1.1fm <%s>"), GetMatrixElementPointer()->GetLabel(), dHeight, SYSTEM_TEXT_KWIKSTAGE );
			}
			else
			{
        sText.Format( _T("%s: %1.1fm"), GetMatrixElementPointer()->GetLabel(), dHeight );
			}
		}
		else if( GetBay()!=NULL )
		{
			sText.Format( _T("%i"), GetBay()->GetBayNumber() );
		}
		else
		{
			//What the hell is this?
			assert( false );
			sText = _T(" ");
		}
	}
	return sText;
}

bool Component::GetRemoveComponentFromBOM()
{
	return m_bRemoveComponentFromBOM;
}

void Component::SetRemoveComponentFromBOM(bool m_bRemove)
{
	m_bRemoveComponentFromBOM = m_bRemove;
}

double Component::GetAcurate3DAdjust(AxisEnum Axis, bool bMatrixElementSideOn/*=true*/ )
{
	double dAdjust = 0.00;

	switch( GetType() )
	{
	case( CT_TRANSOM ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;

		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_LEDGER ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = HIGH_STAR_MILLS_Z_ADJUST;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = HIGH_STAR_KWIKS_Z_ADJUST;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_RAIL ):
	case( CT_MID_RAIL ):
		if( ( GetMatrixElementPointer()==NULL && ( GetSideOfBay()==NORTH || GetSideOfBay()==SOUTH ) ) ||
			( GetMatrixElementPointer()!=NULL && !bMatrixElementSideOn ) )
		{
			//This is positioned in the same way a ledger is positioned
			switch( Axis )
			{
			case( X_AXIS ):
				break;
			case( Y_AXIS ):
				break;
			case( Z_AXIS ):
				switch( GetSystem() )
				{
				case( S_MILLS ):
					dAdjust = HIGH_STAR_MILLS_Z_ADJUST;
					break;
				case( S_KWIKSTAGE ):
					dAdjust = HIGH_STAR_KWIKS_Z_ADJUST;
					break;
				case( S_OTHER ):
					dAdjust = 0.00;
					break;
				default:
					assert( false );
				}
				break;
			default:
				assert( false );
			}
		}
		else
		{
			//This is positioned in the same way a transom is positioned
			switch( Axis )
			{
			case( X_AXIS ):
				break;
			case( Y_AXIS ):
				break;
			case( Z_AXIS ):
				if( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL &&
					GetLiftPointer()->GetBayPointer()->GetBayWidth()<=COMPONENT_LENGTH_0700+ROUND_ERROR )
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS;
						break;
					case( S_KWIKSTAGE ):
						dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
				}
				else
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust = LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust = LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
				}
				break;
			default:
				assert( false );
			}
		}
		break;


	case( CT_LAPBOARD ):
		//fallthrough

	case( CT_DECKING_PLANK ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS + TRANSOM_MILLS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS + TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_TOE_BOARD ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS + TRANSOM_MILLS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS + TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_HOPUP_BRACKET ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			//These distance are from the top of the Hopup to the top of the star,
			//	so to use this the top of the component should be lined up with the top
			//	of the star
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = HOPUP_BRACKET_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = HOPUP_BRACKET_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}

			if( GetMatrixElementPointer()==NULL )
			{
				switch( GetSideOfBay() )
				{
				case( SSE ):
				case( SSW ):
				case( NNE ):
				case( NNW ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				case( ENE ):
				case( ESE ):
				case( WSW ):
				case( WNW ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				default:
					assert( false );
				}
			}
			else
			{
				if( bMatrixElementSideOn )
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
				else
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
			}


			break;
		default:
			assert( false );
		}
		break;
	case( CT_STAGE_BOARD ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = 0.00;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = 0.00 - HOPUP_BRACKET_KWIKS_TIE_BAR_HOLE_CNTR + TIE_BAR_KWIKS_CNTR_PEG_TO_EDGE - TIE_BAR_KWIKS_OUTER_EDGE_TO_INNER_EDGE;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = TRANSOM_MILLS_WANTFA_THICKNESS + TRANSOM_MILLS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = TRANSOM_KWIKS_WANTFA_THICKNESS + TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}

			if( GetMatrixElementPointer()==NULL )
			{
				switch( GetSideOfBay() )
				{
				case( NORTH ):
				case( SOUTH ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				case( EAST ):
				case( WEST ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				default:
					assert( false );
				}
			}
			else
			{
				if( bMatrixElementSideOn )
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
				else
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
			}
			break;
		default:
			assert( false );
		}
		break;
		
	case( CT_CORNER_STAGE_BOARD ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = HIGH_STAR_MILLS_Z_ADJUST;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = HIGH_STAR_KWIKS_Z_ADJUST;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_BRACING ):
		switch( Axis )
		{
		case( X_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = BRACING_MILLS_HOLE_TO_EDGE;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = BRACING_KWIKS_HOLE_TO_EDGE;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			if( GetMatrixElementPointer()==NULL )
			{
				switch( GetSideOfBay() )
				{
				case( NORTH ):
				case( SOUTH ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				case( EAST ):
				case( WEST ):
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				default:
					assert( false );
				}
			}
			else
			{
				if( bMatrixElementSideOn )
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
				else
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
					break;
				}
			}
			break;
		default:
			assert( false );
		}
		break;
		
	case( CT_STANDARD ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = STANDARD_MILLS_SPRIGET_LENGTH;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = STANDARD_KWIKS_SPRIGET_LENGTH;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_STAIR ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS + TRANSOM_MILLS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS + TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_LADDER ):
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSystem() )
			{
			case( S_MILLS ):
				dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS + TRANSOM_MILLS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_KWIKSTAGE ):
				dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS + TRANSOM_KWIKS_ANGLE_SECTION_THICKNESS;
				break;
			case( S_OTHER ):
				dAdjust = 0.00;
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

	case( CT_MESH_GUARD ):
		//The top of this guard must be alligned with the centre of a star
		switch( Axis )
		{
		case( X_AXIS ):
			break;
		case( Y_AXIS ):
			break;
		case( Z_AXIS ):
			switch( GetSideOfBay() )
			{
			case( NORTH ):
			case( SOUTH ):
				switch( GetSystem() )
				{
				case( S_MILLS ):
					dAdjust+= HIGH_STAR_MILLS_Z_ADJUST;
					break;
				case( S_KWIKSTAGE ):
					dAdjust+= HIGH_STAR_KWIKS_Z_ADJUST;
					break;
				case( S_OTHER ):
					dAdjust = 0.00;
					break;
				default:
					assert( false );
				}
				dAdjust+= GetController()->GetCompDetails()->GetActualHeight( GetSystem(),
								CT_LEDGER, COMPONENT_LENGTH_2400, MT_STEEL, false );
				break;
			case( EAST ):
			case( WEST ):
				if( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL &&
					GetLiftPointer()->GetBayPointer()->GetBayWidth()<=COMPONENT_LENGTH_0700+ROUND_ERROR )
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust = LOW_STAR_MILLS_Z_ADJUST + TRANSOM_MILLS_WANTFA_THICKNESS;
						break;
					case( S_KWIKSTAGE ):
						dAdjust = LOW_STAR_KWIKS_Z_ADJUST + TRANSOM_KWIKS_WANTFA_THICKNESS;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
				}
				else
				{
					switch( GetSystem() )
					{
					case( S_MILLS ):
						dAdjust = LOW_STAR_MILLS_Z_ADJUST;
						break;
					case( S_KWIKSTAGE ):
						dAdjust = LOW_STAR_KWIKS_Z_ADJUST;
						break;
					case( S_OTHER ):
						dAdjust = 0.00;
						break;
					default:
						assert( false );
					}
				}
				dAdjust+= GetController()->GetCompDetails()->GetActualHeight( GetSystem(),
								CT_TRANSOM, COMPONENT_LENGTH_2400, MT_STEEL, false );
				break;
			default:
				assert( false );
			}
			break;
		default:
			assert( false );
		}
		break;

		default:
		break;
	}

	return dAdjust;
}

StockListElement * Component::GetStockDetails() const
{
//	assert(m_pStockElement!=NULL);
	return m_pStockElement;
}

void Component::SetStockDetailsPointerDumb(StockListElement *pElement)
{
	m_pStockElement = pElement;
}

void Component::SetStockDetailsPointer()
{
	if( m_iSettingToDefault<=0 )
	{
		ComponentTypeEnum eType;
		eType = GetType();
		SetStockDetailsPointerDumb( GetController()->GetCompDetails()->GetStockDetails( eType, GetLengthCommon(), GetMaterialType(), GetSystem() ) );
		if( eType!=CT_BOM_EXTRA && eType!=CT_VISUAL_COMPONENT &&
			eType!=CT_UNDEFINED )
		{
			assert(m_pStockElement!=NULL);
		}
	}
}

int Component::GetColour()
{
	int iColour;

	if( m_iColour==COLOUR_UNDEFINED )
	{
		if( IsCommitted() )
		{
			iColour = COLOUR_COMMITTED_FULLY;
		}
		else if( GetController()->IsColourByStageLevel() &&
					GetMatrixElementPointer()==NULL &&
					GetBay()!=NULL )
		{
			iColour = GetController()->GetBayStageLevelArray()->GetColour( GetStage(), GetLevel() );

			//skip the white and grays colours
			if( iColour>=COLOUR_WHITE )
				iColour+= 3;
		}
		else
		{
			//We have not defined a colour, so use the component type to set colour
			iColour = GetComponentColour( GetType() );
		}
	}
	else
	{
		//Lets use the predifined value
		iColour = m_iColour;
	}
	return iColour;
}

VisualComponents * Component::GetVisualComponentsPointer() const
{
	return m_pvcComponents;
}

void Component::SetVisualComponentsPointer(VisualComponents *pVC)
{
	SetAllPointerToNULL();
	m_pvcComponents = pVC;
}


// Changes made to this function by ~SJ~, 03.08.2007
void Component::SetXData()
{
	int					i;
    TCHAR				appName[132];
	Entity				*pEnt;
	CString				sName;
    struct resbuf		*pRb, *pTemp;
	Vector3D			Vector;
	Acad::ErrorStatus	es;

	pEnt = GetEntity();
	if( pEnt!=NULL && !pEnt->objectId().isNull() )
	{
		es = pEnt->open();
		assert( es==Acad::eOk );

		Vector = AcGeVector3d::kIdentity;
		Vector.x = 1.00;
		Vector.y = 0.00;
		Vector.z = 0.00;

		if( GetMatrixElementPointer()!=NULL && 
			GetMatrixElementPointer()->IsCutThrough() )
			sName = XDATA_LABEL_KUT_COMPONENT;
		else
			sName = XDATA_LABEL_COMPONENT;

    /*
		for( i=0; i<sName.GetLength(); i++ )
		{
			appName[i] = sName[i];
		}
		appName[i] = _T('\0');
    
		pRb = pEnt->xData(appName);
    */
      
    pRb = pEnt->xData(sName);
		if (pRb != NULL)
		{
			// If xdata is present, then walk to the
			// end of the list.
			//
			for (pTemp = pRb; pTemp->rbnext != NULL; pTemp = pTemp->rbnext)
			{
				;
			}
		}
		else
		{
			// If xdata is not present, register the application
			// and add appName to the first resbuf in the list.
			// Notice that there is no -3 group as there is in

			// AutoLISP. This is ONLY the xdata so
			// the -3 xdata-start marker isn't needed.
			////////////////////////////////////////////////////////////
			//XDSC_APPNAME
			//acdbRegApp(appName);
      acdbRegApp(sName);

			//pRb = acutNewRb(AcDb::kDxfRegAppName);  
      pRb = acutBuildList(AcDb::kDxfRegAppName, sName, NULL);
      pTemp = pRb;
			//pTemp->resval.rstring = (TCHAR*) malloc(_tcslen(appName) + 1);
			//_tcscpy(pTemp->resval.rstring, appName);
		}
		////////////////////////////////////////////////////////////
		//XDSC_COMPONENT_POINTER - Pointer to the component
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger32);
		pTemp = pTemp->rbnext;
		pTemp->resval.rlong = (long)this;

		////////////////////////////////////////////////////////////
		//XDSC_POSITION		- 3D Position
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXCoord);
		pTemp = pTemp->rbnext;
		pTemp->resval.rpoint[0] = 0.00;
		pTemp->resval.rpoint[1] = 0.00;
		pTemp->resval.rpoint[2] = 0.00;

		////////////////////////////////////////////////////////////
		//XDSC_DISPLACEMENT - Direction of first arm
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXDisp);
		pTemp = pTemp->rbnext;
		pTemp->resval.rpoint[0] = 1.00;
		pTemp->resval.rpoint[1] = 0.00;
		pTemp->resval.rpoint[2] = 0.00;

		////////////////////////////////////////////////////////////
		//XDSC_DIRECTION	- Direction of second arm	
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXDir);
		pTemp = pTemp->rbnext;
		pTemp->resval.rpoint[0] = 0.00;
		pTemp->resval.rpoint[1] = 1.00;
		pTemp->resval.rpoint[2] = 0.00;

		////////////////////////////////////////////////////////////
		//XDSC_DISTANCE		- Distance moved
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdDist);
		pTemp = pTemp->rbnext;
		pTemp->resval.rreal = 0.00;

		////////////////////////////////////////////////////////////
		//XDSC_SCALE		- Scale factor
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdScale);
		pTemp = pTemp->rbnext;
		pTemp->resval.rreal = 0.00;

		// The following code shows the use of upgradeOpen()
		// to change the entity from read to write.
		pEnt->upgradeOpen();
		pEnt->setXData(pRb);
    
		pEnt->close();
		acutRelRb(pRb);
	}
}	

void Component::ResetXDataPositions()
{
	Entity				*pEnt;
	resbuf				*pRB, *pRes;

	pEnt = GetEntity();
	if( pEnt!=NULL )
	{
		pEnt->open();

		if( GetMatrixElementPointer()!=NULL && GetMatrixElementPointer()->IsCutThrough() )
			pRB = pEnt->xData(XDATA_LABEL_KUT_COMPONENT);
		else
			pRB = pEnt->xData(XDATA_LABEL_COMPONENT);

		pRes = pRB;
		if( pRes==NULL )
		{
			SetXData();
			return;
		}

		pRes = pRes->rbnext;
		pRes = pRes->rbnext;
		if(pRes->restype==AcDb::kDxfXdWorldXCoord )
		{
			pRes->resval.rpoint[0] = 0.00;
			pRes->resval.rpoint[1] = 0.00;
			pRes->resval.rpoint[2] = 0.00;

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdWorldXDisp );
			pRes->resval.rpoint[0] = 1.00;
			pRes->resval.rpoint[1] = 0.00;
			pRes->resval.rpoint[2] = 0.00;

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdWorldXDir );
			pRes->resval.rpoint[0] = 0.00;
			pRes->resval.rpoint[1] = 1.00;
			pRes->resval.rpoint[2] = 0.00;

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdDist );
			pRes->resval.rreal = 0.00;

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdScale );
			pRes->resval.rreal = 0.00;
		}

		pEnt->setXData(pRB);

		acutRelRb(pRes);
		pEnt->close();
	}
}

Vector3D Component::GetLastPosition()
{
	return m_LastPosition;
}

Vector3D Component::GetLastDirection()
{
	return m_LastDirection;
}

Vector3D Component::GetLastDirectionAlt()
{
	return m_LastDirectionAlt;
}

void Component::StoreLastPosition()
{
	Entity				*pEnt;
	resbuf				*pRB, *pRes;

	pEnt = GetEntity();
	if( pEnt!=NULL )
	{
		pEnt->open();

		if( GetMatrixElementPointer()!=NULL && GetMatrixElementPointer()->IsCutThrough() )
			pRB = pEnt->xData(XDATA_LABEL_KUT_COMPONENT);
		else
			pRB = pEnt->xData(XDATA_LABEL_COMPONENT);

		if( pRB==NULL )
		{
			SetXData();
			return;
		}

		pRes = pRB;

		pRes = pRes->rbnext;
		pRes = pRes->rbnext;
		if(pRes->restype==AcDb::kDxfXdWorldXCoord )
		{
			m_LastPosition.x = pRes->resval.rpoint[0];
			m_LastPosition.y = pRes->resval.rpoint[1];
			m_LastPosition.z = pRes->resval.rpoint[2];

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdWorldXDisp );
			m_LastDirection.x = pRes->resval.rpoint[0];
			m_LastDirection.y = pRes->resval.rpoint[1];
			m_LastDirection.z = pRes->resval.rpoint[2];

			pRes = pRes->rbnext;
			assert(pRes->restype==AcDb::kDxfXdWorldXDir );
			m_LastDirectionAlt.x = pRes->resval.rpoint[0];
			m_LastDirectionAlt.y = pRes->resval.rpoint[1];
			m_LastDirectionAlt.z = pRes->resval.rpoint[2];
		}
		acutRelRb(pRes);
		pEnt->close();
	}
}

void Component::ComponentMove()
{
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
		pt = asPnt3d(result.resval.rpoint);
	}
	else
	{
		pt.set( 0.00, 0.00, 0.00 );
	}

	Vector.set( pt.x, pt.y, pt.z );
	Trans.setToTranslation( Vector );
	SetTransform( Trans );
	Delete3DView();
	Create3DView();

	CString sPt;
	sPt.Format( _T("%i,%i,%i"), (int)pt.x, (int)pt.y, (int)pt.z );
	acedCommand(RTSTR, _T("_MOVE"), RTSTR, _T("_LAST"), RTSTR, _T(""), RTSTR, sPt, RTSTR, _T("\\"), RTNONE );
}

SystemEnum Component::GetSystem() const
{
	return m_eSystem;
}

void Component::SetSystem(SystemEnum eSystem)
{
	m_eSystem = eSystem;
}

double Component::GetStarSeparation() const
{
	double dStarSeparation;
	if( GetLiftPointer()!=NULL && GetLiftPointer()->GetBayPointer()!=NULL )
	{
		dStarSeparation = GetLiftPointer()->GetBayPointer()->GetStarSeparation();
	}
	else if( GetBayPointer()!=NULL )
	{
		dStarSeparation = GetBayPointer()->GetStarSeparation();
	}
	else if( GetMatrixElementPointer()!=NULL )
	{
		dStarSeparation = GetMatrixElementPointer()->GetStarSeparation();
	}
	else
	{
		if( GetSystem()==S_MILLS )
			dStarSeparation = STAR_SEPARATION_MILLS;
		else
			dStarSeparation = STAR_SEPARATION_KWIKSTAGE;
	}
	return dStarSeparation;
}

Matrix3D Component::GetEntireTransform()
{
	int			iRunID;
	bool		bIsAutobuild;
	Vector3D	Vector;
	Matrix3D	Trans;

	///////////////////////////////////////////////////////////////
	//Is this the autobuild?
	bIsAutobuild = false;
	if( (GetRun()!=NULL) && (GetController()!=NULL) )
	{
		iRunID = GetController()->GetRunID( GetRun() );
		bIsAutobuild = ( (iRunID==ID_NONE_MATCHING) || (iRunID==ID_NO_IDS) );
	}

	Trans = GetTransform();
	if( GetLift()!=NULL )
	{
		Trans = GetLift()->GetTransform() * Trans;
	}
	if( GetBay()!=NULL )
	{
		Trans = GetBay()->GetTransform() * Trans;
		if( GetBay()->GetBayType()!=BAY_TYPE_LAPBOARD )
			Trans = GetRun()->GetTransform() * Trans;
	}
	if( GetMatrixElementPointer()!=NULL )
	{
		Trans = GetMatrixElementPointer()->GetTransform() * Trans;
		Trans = GetMatrixElementPointer()->GetMatrixPointer()->GetMatrixCrosshairTransform() * Trans;
	}
	if( !bIsAutobuild && (GetMatrixElementPointer()==NULL) )
	{
		//The controller's transform should not be applied to the autobuild
		Trans = GetController()->Get3DCrosshairTransform() * Trans;
	}

	if( GetVisualTransformUsed() )
	{
		Trans = GetVisualTransform()*Trans;
	}

	return Trans;
}

Matrix3D Component::GetVisualTransform()
{
	return m_VisualTransform;
}

void Component::SetVisualTransform(Matrix3D VisualTransform)
{
	m_VisualTransform = VisualTransform;
}

void Component::SetVisualTransformUsed(bool bUsed)
{
	m_bVisualTransformUsed = bUsed;
}

bool Component::GetVisualTransformUsed()
{
	return m_bVisualTransformUsed;
}


bool Component::IsComponentSelected()
{
	AcDbObjectIdArray	ids;
	ids.append(GetEntity()->objectId());
	return GetController()->AreObjectIDsSelected( ids );
}

