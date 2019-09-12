// EntityReactor.cpp: implementation of the EntityReactor class.
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
#include "EntityReactor.h"
#include "Component.h"
#include "Controller.h"
#include "Matrix.h"
#include "EntityDefinitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//This number is simply a madeup number to identify this entity as having
//	been created by kwikscaf, this is used since other reactors are type
//	cast to an EntityReactor, and if there is no way to determine that
//	kwikscaf created the reactor, then we start getting garbage values
//	returned from these function!
const int KWIKSCAF_REACTOR = 0xabcd;

extern Controller	*gpController;
extern EntityDebugger gReactorED;

//#define SHOW_REACTORS_AS_THEY_GO_OFF

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ACRX_DXF_DEFINE_MEMBERS(EntityReactor, AcDbObject, 
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
    0, ENTITYREACTOR, KWIKSCAF);


EntityReactor::EntityReactor()
{
	//This must be first or we may have problems
	m_iKwikscafReactorCheck = KWIKSCAF_REACTOR;

	SetComponentPointer( NULL );
	SetControllerPointer( NULL );
	SetBayPointer( NULL );
	SetMatrixPointer( NULL );
	SetLevelLine( -1 );
	SetIsGroup( false );
}

EntityReactor::~EntityReactor()
{
	SetComponentPointer( NULL );
	SetControllerPointer( NULL );
	SetBayPointer( NULL );
	SetMatrixPointer( NULL );
}

//////////////////////////////////////////////////////////////////////////
//Overrides
//////////////////////////////////////////////////////////////////////////
void EntityReactor::cancelled(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("cancelled")) ) return;
	dbObj;
}

void EntityReactor::copied(const AcDbObject* dbObj, const AcDbObject* newObj)
{
	if( !IsKwikscaffReactor(_T("copied")) ) return;
	Point3DArray		gripPoints;
	AcDbIntArray		osnapModes, geomIds;
	Acad::ErrorStatus	es;
	newObj;
	if( GetBayPointer()!=NULL )
	{
		BayTypeEnum	eBType;
		eBType = GetBayPointer()->GetBayType();
		if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS ||
			eBType==BAY_TYPE_LADDER	|| eBType==BAY_TYPE_BUTTRESS ) 
		{
			if( !GetBayPointer()->GetRunPointer()->GetController()->IsIgnorMovement() )
			{
				es = ((AcDbEntity*)dbObj)->getGripPoints( gripPoints, osnapModes, geomIds);
				GetBayPointer()->GetRunPointer()->GetController()->m_ptLastPosition[0] = gripPoints[0];
				GetBayPointer()->GetRunPointer()->GetController()->m_ptLastPosition[1] = gripPoints[1];
				#ifdef _DEBUG
/*				acutPrintf("\nBay: %i (%1.1f, %1.1f, %1.1f)", GetBayPointer()->GetBayNumber(),
						gripPoints[0].x, gripPoints[0].y, gripPoints[0].z );
*/				#endif	//#ifdef _DEBUG

				GetBayPointer()->GetRunPointer()->GetController()->m_dLastAngleX = CalculateAngle( gripPoints[1]-gripPoints[0], X_AXIS );
				GetBayPointer()->GetRunPointer()->GetController()->m_dLastAngleY = CalculateAngle( gripPoints[1]-gripPoints[0], Y_AXIS );
				GetBayPointer()->GetRunPointer()->GetController()->m_dLastAngleZ = CalculateAngle( gripPoints[1]-gripPoints[0], Z_AXIS );
			}
		}
		if( eBType==BAY_TYPE_LAPBOARD )
		{
			if( !GetBayPointer()->GetController()->IsIgnorMovement() )
			{
				es = ((AcDbEntity*)dbObj)->getGripPoints( gripPoints, osnapModes, geomIds);
				GetBayPointer()->GetController()->m_ptLastPosition[0] = gripPoints[0];
				GetBayPointer()->GetController()->m_ptLastPosition[1] = gripPoints[1];
				#ifdef _DEBUG
/*				acutPrintf("\nBay: %i (%1.1f, %1.1f, %1.1f)", GetBayPointer()->GetID(),
						gripPoints[0].x, gripPoints[0].y, gripPoints[0].z );
*/				#endif	//#ifdef _DEBUG
				GetBayPointer()->GetController()->m_dLastAngleX = CalculateAngle( gripPoints[1]-gripPoints[0], X_AXIS );
				GetBayPointer()->GetController()->m_dLastAngleY = CalculateAngle( gripPoints[1]-gripPoints[0], Y_AXIS );
				GetBayPointer()->GetController()->m_dLastAngleZ = CalculateAngle( gripPoints[1]-gripPoints[0], Z_AXIS );
			}
		}
	}
	else if( GetMatrixPointer()!=NULL )
	{
		AcDbObjectIdArray *pCopiedObjects;
		SetControllerPointer( GetMatrixPointer()->GetController() );
		pCopiedObjects = GetControllerPointer()->GetCopiedObjectArray();
		pCopiedObjects->append( newObj->objectId() );
	}
	else if( GetComponentPointer( )!=NULL )
	{
		AcDbObjectIdArray *pCopiedObjects;
		SetControllerPointer( GetComponentPointer()->GetController() );
		pCopiedObjects = GetControllerPointer()->GetCopiedObjectArray();
		pCopiedObjects->append( dbObj->objectId() );
		pCopiedObjects->append( newObj->objectId() );
	}

}

void EntityReactor::erased(const AcDbObject* dbObj, Adesk::Boolean pErasing/*=true*/ )
{
	if( !IsKwikscaffReactor(_T("erased")) ) return;
	dbObj;
	pErasing;
	if( GetBayPointer()!=NULL )
	{
		BayTypeEnum	eBType;
		eBType = GetBayPointer()->GetBayType();
		if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS ||
			eBType==BAY_TYPE_LADDER	|| eBType==BAY_TYPE_BUTTRESS ) 
		{
			GetBayPointer()->RemoveBay();
			GetBayPointer()->GetRunPointer()->GetController()->SetErasureEntityType( ET_BAY );
			GetBayPointer()->GetRunPointer()->GetController()->AddEntityBayNumberID( GetBayPointer()->GetBayNumber() );
		}
		else if( eBType==BAY_TYPE_LAPBOARD )
		{
			GetBayPointer()->RemoveBay();
			GetBayPointer()->GetController()->SetErasureEntityType( ET_LAPBOARD );
			GetBayPointer()->GetController()->AddEntityLapboardID( GetBayPointer()->GetID() );
		}
	}
	else if( GetMatrixPointer()!=NULL )
	{
		if( GetMatrixPointer()->IsCrosshairCreated() )
		{
			//They have tried to delete the matrix crosshair we should delete the 3DView
			if( IsLevelLine() )
			{
				GetMatrixPointer()->GetController()->
						SetErasureEntityType( ET_LEVEL );
				GetMatrixPointer()->GetController()->AddEntityLevelID( m_iLevelLine );
			}
			else
			{
				GetMatrixPointer()->GetController()->
						SetErasureEntityType( ET_MATRIX );
				GetMatrixPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
			}
		}
		else
		{
			//we are still creating the matrix, ignor this
			1;
		}
	}
	else if( GetComponentPointer( )!=NULL )
	{
		GetComponentPointer( )->Remove();
		if( GetComponentPointer( )->GetBayPointer()!=NULL )
		{
			;//assert( GetComponentPointer( )->GetBayPointer()->GetRunPointer()!=NULL );
			;//assert( GetComponentPointer( )->GetBayPointer()->GetRunPointer()->GetController()!=NULL );
			GetComponentPointer( )->GetBayPointer()->GetRunPointer()->GetController()->
					SetErasureEntityType( ET_3D );
		}
		else if( GetComponentPointer( )->GetLiftPointer()!=NULL )
		{
			;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()!=NULL );
			if( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetBayType()==BAY_TYPE_LAPBOARD )
			{
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetController()->
						SetErasureEntityType( ET_3D );
			}
			else
			{
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()!=NULL );
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetController()->
						SetErasureEntityType( ET_3D );
			}
		}
		else if( GetComponentPointer( )->GetVisualComponentsPointer()!=NULL )
		{
			GetComponentPointer( )->GetVisualComponentsPointer()->GetController()->SetErasureEntityType( ET_3D );
		}
		else
		{
			;//assert( false );
		}
	}
	else if( GetControllerPointer()!=NULL )
	{
		if( GetControllerPointer()->IsCrosshairCreated() )
		{
			//They have tried to delete the 3D crosshair we should delete the 3DView
			GetControllerPointer( )->SetErasureEntityType( ET_3D );
		}
		else
		{
			//we are still creating the 3d, ignor this
			1;
		}
	} 
	else
	{
		//we have a renegade entity reactor
//		;//assert( false );
	}
}

void EntityReactor::goodbye(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("goodbye")) ) return;
	dbObj;
}

void EntityReactor::openedForModify(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("openedForModify")) ) return;

	if( GetBayPointer()!=NULL )
	{
		BayTypeEnum	eBType;
		eBType = GetBayPointer()->GetBayType();
		if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS ||
			eBType==BAY_TYPE_LADDER	|| eBType==BAY_TYPE_BUTTRESS ) 
		{
			if( !GetBayPointer()->GetRunPointer()->GetController()->IsIgnorMovement() )
			{
				Point3DArray		gripPoints;
				AcDbIntArray		osnapModes, geomIds;
				Acad::ErrorStatus	es;
				es = ((AcDbEntity*)dbObj)->getGripPoints( gripPoints, osnapModes, geomIds);
				GetBayPointer()->GetRunPointer()->GetController()->m_ptLastPosition[0] = gripPoints[0];
				GetBayPointer()->GetRunPointer()->GetController()->m_ptLastPosition[1] = gripPoints[1];
				GetBayPointer()->GetRunPointer()->GetController()->m_opaObjectPositions.Add( dbObj->objectId(), gripPoints[0], gripPoints[1] );
				#ifdef _DEBUG
/*				acutPrintf("\nBay: %i (%1.1f, %1.1f, %1.1f)", GetBayPointer()->GetBayNumber(),
						gripPoints[0].x, gripPoints[0].y, gripPoints[0].z );
*/				#endif	//#ifdef _DEBUG
				((AcDbEntity*)dbObj)->close();
			}
		}
		else if( eBType==BAY_TYPE_LAPBOARD )
		{
			if( !GetBayPointer()->GetController()->IsIgnorMovement() )
			{
				Point3DArray		gripPoints;
				AcDbIntArray		osnapModes, geomIds;
				Acad::ErrorStatus	es;
				es = ((AcDbEntity*)dbObj)->getGripPoints( gripPoints, osnapModes, geomIds);
				GetBayPointer()->GetController()->m_ptLastPosition[0] = gripPoints[0];
				GetBayPointer()->GetController()->m_ptLastPosition[1] = gripPoints[1];
				GetBayPointer()->GetController()->m_opaObjectPositions.Add( dbObj->objectId(), gripPoints[0], gripPoints[1] );
				((AcDbEntity*)dbObj)->close();
			}
		}
	}
}

void EntityReactor::modified(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("modified")) ) return;
	dbObj;
}

void EntityReactor::subObjModified(const AcDbObject* dbObj, const AcDbObject* subObj)
{
	if( !IsKwikscaffReactor(_T("subObjModified")) ) return;
	dbObj;
	subObj;
}

void EntityReactor::modifyUndone(const AcDbObject* dbObj)
{

	if( !IsKwikscaffReactor(_T("modifyUndone")) ) return;
	dbObj;
}

void EntityReactor::modifiedXData(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("modifiedXData")) ) return;
	dbObj;
	if( GetControllerPointer()!=NULL )
		;//assert( false );
}

void EntityReactor::unappended(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("unappended")) ) return;
	dbObj;
}

void EntityReactor::reappended(const AcDbObject* dbObj)
{
	if( !IsKwikscaffReactor(_T("reappended")) ) return;
	dbObj;
}

void EntityReactor::objectClosed(const AcDbObjectId objId)
{
	if( !IsKwikscaffReactor(_T("objectClosed")) ) return;
	objId;
/*	if( (GetMatrixPointer()!=NULL) && (GetMatrixPointer()->IsCrosshairCreated()) 
		&& GetMatrixPointer()->IsMoving() )
	{
		GetMatrixPointer()->SetMoving( false );
	}
*/
/*	if( GetMatrixPointer()->HaveMatrixLevelsChanged() )
	{
		GetMatrixPointer()->RegenLevels();
	}
*/
}

void EntityReactor::modifiedGraphics(const AcDbEntity* dbEnt)
{
	if( !IsKwikscaffReactor(_T("modifiedGraphics")) ) return;

	if( GetBayPointer()!=NULL )
	{
		BayTypeEnum	eBType;
		eBType = GetBayPointer()->GetBayType();
		if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS	||
			eBType==BAY_TYPE_LADDER	|| eBType==BAY_TYPE_BUTTRESS ) 
		{
			;//assert( GetBayPointer()->GetRunPointer()!=NULL );
			;//assert( GetBayPointer()->GetRunPointer()->GetController()!=NULL );
			GetBayPointer()->GetRunPointer()->GetController()->SetMovementEntityType( ET_BAY );
			GetBayPointer()->GetRunPointer()->GetController()->AddEntityBayNumberID( GetBayPointer()->GetBayNumber() );
		}
		else if( eBType==BAY_TYPE_LAPBOARD )
		{
			;//assert( GetBayPointer()->GetController()!=NULL );
			GetBayPointer()->GetController()->SetMovementEntityType( ET_LAPBOARD );
			GetBayPointer()->GetController()->AddEntityLapboardID( GetBayPointer()->GetID() );
		}
	}
	else if( (GetMatrixPointer()!=NULL) )
	{
		if( GetMatrixPointer()->IsCrosshairCreated() )
		{
			;//assert( GetMatrixPointer()->GetController()!=NULL );
			//They have tried to delete the matrix crosshair we should delete the 3DView
			if( IsLevelLine() )
			{
				GetMatrixPointer()->GetController()->
						SetMovementEntityType( ET_LEVEL );
				GetMatrixPointer()->GetController()->AddEntityLevelID( m_iLevelLine );
			}
			else
			{
				GetMatrixPointer()->GetController()->
						SetMovementEntityType( ET_MATRIX );
				GetMatrixPointer()->GetController()->
						SetMovedComponent( GetComponentPointer() );
			}
		}
		else
		{
			//we are still creating the matrix, so ignor this
			1;
		}
	}
	else if( GetComponentPointer( )!=NULL )
	{
		//This must be either a lift, a stair, ladder, normal or lapboard bay
		if( GetComponentPointer( )->GetVisualComponentsPointer()!=NULL )
		{
			;//assert(GetComponentPointer( )->GetVisualComponentsPointer()->GetController()!=NULL);
			GetComponentPointer( )->GetVisualComponentsPointer()->GetController()->SetMovementEntityType( ET_3D );
			GetComponentPointer( )->GetVisualComponentsPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
		}
		else if( GetComponentPointer( )->GetLiftPointer()!=NULL )
		{
			//This is either a stair, ladder, normal or lapboard bay
			;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()!=NULL );
			BayTypeEnum	eBType;
			eBType = GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetBayType();
			if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS ||
				eBType==BAY_TYPE_LADDER	|| eBType==BAY_TYPE_BUTTRESS ) 
			{
				//This is either a stair, ladder, normal
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()!=NULL );
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetController()->SetMovementEntityType( ET_3D );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
			}
			else
			{
				//This is a lapboard Bay
				;//assert( GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetController()->SetMovementEntityType( ET_3D );
				GetComponentPointer( )->GetLiftPointer()->GetBayPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
			}
		}
		else
		{
			//This is a bay Component such as a standard
			;//assert( GetComponentPointer( )->GetBayPointer()!=NULL );
			BayTypeEnum	eBType;
			eBType = GetComponentPointer( )->GetBayPointer()->GetBayType();
			if( eBType==BAY_TYPE_BAY	|| eBType==BAY_TYPE_STAIRS ||
				eBType==BAY_TYPE_LADDER || eBType==BAY_TYPE_BUTTRESS ) 
			{
				//This is a stair, ladder or normal bay
				;//assert( GetComponentPointer( )->GetBayPointer()->GetRunPointer()!=NULL );
				;//assert( GetComponentPointer( )->GetBayPointer()->GetRunPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetBayPointer()->GetRunPointer()->GetController()->SetMovementEntityType( ET_3D );
				GetComponentPointer( )->GetBayPointer()->GetRunPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
			}
			else
			{
				//This is a lapboard bay
				;//assert( GetComponentPointer( )->GetBayPointer()!=NULL );
				;//assert( GetComponentPointer( )->GetBayPointer()->GetController()!=NULL );
				GetComponentPointer( )->GetBayPointer()->GetController()->SetMovementEntityType( ET_3D );
				GetComponentPointer( )->GetBayPointer()->GetController()->SetMovedComponent( GetComponentPointer() );
			}
		}
	}
	else if( (GetControllerPointer()!=NULL) )
	{
		//This is the 3D crosshair
		if( GetControllerPointer()->IsCrosshairCreated() )
		{
			//They have tried to delete the 3D crosshair we should delete the 3DView
			GetControllerPointer( )->SetMovementEntityType( ET_3D );
		}
		else
		{
			//we are still creating the 3d, so ignor this
			1;
		}
	} 
	else
	{
		//we have a renegade entity reactor
//		;//assert( false );
	}

/*	//Movement of a group within a bay
	if( GetBayPointer()!=NULL && IsGroup() )
	{
		/////////////////////////////////////////////////////////////////////////
		//movement of the a Bay
		Bay					*pBay;
		Point3D				pt;
		Vector3D			Vector;
		Matrix3D			Trans, Transform;
		Controller			*pController;
		Point3DArray		gripPoints;
		AcDbIntArray		osnapModes, geomIds;
		Acad::ErrorStatus	es;

		es = dbEnt->getGripPoints( gripPoints, osnapModes, geomIds);

		pBay		= GetBayPointer();
		pController = pBay->GetRunPointer()->GetController();
		pController->CleanUp();

		if( gripPoints.length()>0 )
		{
			//lets find out where the point 0,0,0 should be!
			pt.set( 0.00, 0.00, 0.00 );
			pt.transformBy( pBay->GetSchematicTransform() );
			pt.transformBy( pBay->GetRunPointer()->GetSchematicTransform() );

			//how far has it been moved?
			Trans = pController->Get3DCrosshairTransform();
			Trans.invert();
			Vector = gripPoints[0]-pt; 
			Transform.setTranslation( Vector );
			GetBayPointer()->UserMovedBay( Transform );
		}
	}
	else if( (GetControllerPointer()!=NULL) && (GetControllerPointer()->IsCrosshairCreated()) )
	{
		/////////////////////////////////////////////////////////////////////////
		//movement of the  3D crosshair
		Vector3D			Vector;
		Matrix3D			Trans, Transform;
		Controller			*pController;
		Point3DArray		gripPoints;
		AcDbIntArray		osnapModes, geomIds;
		Acad::ErrorStatus	es;

		es = dbEnt->getGripPoints( gripPoints, osnapModes, geomIds);

		pController = GetControllerPointer();
		pController->CleanUp();

		if( gripPoints.length()>0 )
		{
			Trans = pController->Get3DCrosshairTransform();
			Trans.invert();
			Vector.set( gripPoints[0].x, gripPoints[0].y, gripPoints[0].z ); 
			Transform.setTranslation( Vector );
			Trans = Transform * Trans;
			pController->Move3DCrosshair( Trans, false );
			pController->Set3DCrosshairTransform( Transform );
		}
	}
	else if( (GetMatrixPointer()!=NULL) && (GetMatrixPointer()->IsCrosshairCreated()) 
					&& !GetMatrixPointer()->IsMoving() )
	{
		/////////////////////////////////////////////////////////////////////////
		//move the Matrix
		if( IsLevelLine() )
		{
			//This is one of the Matrix's Level lines
			GetMatrixPointer()->MoveLevel( (Entity*)dbEnt );
		}
		else
		{
			//This is a normal Matrix element
			Acad::ErrorStatus	es;
			Point3DArray		gripPoints;
			AcDbIntArray		osnapModes, geomIds;
			Vector3D			Vector;
			Matrix3D			Trans, Transform;

			GetMatrixPointer()->GetController()->CleanUp();

			es = dbEnt->getGripPoints( gripPoints, osnapModes, geomIds);

			if( gripPoints.length()>0 )
			{
				Trans = GetMatrixPointer()->GetMatrixCrosshairTransform();
				Trans.invert();
				Vector.set( gripPoints[0].x, gripPoints[0].y, gripPoints[0].z ); 
				Transform.setTranslation( Vector );
				Trans = Transform * Trans;
				GetMatrixPointer()->Move( Trans, false );
				GetMatrixPointer()->SetMatrixCrosshairTransform( Transform );
			}
		}
	}
*/
}

Component * EntityReactor::GetComponentPointer() const
{
	if( !IsKwikscaffReactor(_T("")) )
		return NULL;
	return m_pComponent;
}

Controller * EntityReactor::GetControllerPointer() const
{
	if( !IsKwikscaffReactor(_T("")) )
		return NULL;
	return m_pController;
}

Bay * EntityReactor::GetBayPointer() const
{
	if( !IsKwikscaffReactor(_T("")) )
		return NULL;
	return m_pBay;
}

Matrix * EntityReactor::GetMatrixPointer() const
{
	if( !IsKwikscaffReactor(_T("")) )
		return NULL;
	return m_pMatrix;
}

void EntityReactor::SetControllerPointer(Controller *pController)
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_pController = pController;
}

void EntityReactor::SetComponentPointer(Component *pComponent)
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_pComponent = pComponent;
}

void EntityReactor::SetBayPointer(Bay *pBay)
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_pBay = pBay;
}

void EntityReactor::SetMatrixPointer( Matrix *pMatrix )
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_pMatrix = pMatrix;
}

Acad::ErrorStatus EntityReactor::open( bool bForceOpen/*=false*/ )
{
	EntityReactor		*pReactor;
	Acad::ErrorStatus	es;

	if( !bForceOpen && !IsKwikscaffReactor(_T("open")) )
		return Acad::eInetUnknownError;

	AcDbObjectId		ID;

	ID = objectId();

	;//assert( !ID.isNull() );

	es = acdbOpenAcDbObject( (AcDbObject*&)pReactor, ID, AcDb::kForWrite );

	if( es==Acad::eOk )
	{
		if( pReactor!=this )
			return Acad::eInetUnknownError;
		;//assert( isWriteEnabled() );
	}
	else
	{
		;//assert( false );
	}

	return es;
}


void EntityReactor::SetLevelLine( int iLevelLine )
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_iLevelLine = iLevelLine;
}

bool EntityReactor::IsLevelLine()
{
	if( !IsKwikscaffReactor(_T("")) )
		return false;
	return m_iLevelLine>=0;
}

void EntityReactor::SetIsGroup(bool bIsGroup)
{
	if( !IsKwikscaffReactor(_T("")) ) return;
	m_bIsGroup = bIsGroup;
}

bool EntityReactor::IsGroup()
{
	if( !IsKwikscaffReactor(_T("")) )
		return false;
	return m_bIsGroup;
}

bool EntityReactor::IsKwikscaffReactor(LPCTSTR strReactorName) const
{
#ifdef SHOW_REACTORS_AS_THEY_GO_OFF
	CString sRN;
	sRN = strReactorName;
	if( sRN.GetLength()>0 )
    acutPrintf( _T("\nReactor: %s"), sRN );
#endif	//SHOW_REACTORS_AS_THEY_GO_OFF
	if( gpController==NULL )
		return false;
	if( gpController->IsDestructingController() )
		return false;
	if( gpController->IsOpeningDocument() )
		return false;
	return m_iKwikscafReactorCheck==KWIKSCAF_REACTOR;
}


Component * EntityReactor::GetComponent(AcDbEntity *pEnt)
{
	resbuf				*pRes;
	Component			*pComponent;
	Acad::ErrorStatus	es;

	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	;//assert( es==Acad::eOk ); 

	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes==NULL )
	{
		pEnt->close();
		return NULL;
	}

	pRes = pRes->rbnext;
	if(pRes->restype!=AcDb::kDxfXdInteger32 )
	{
		pEnt->close();
		return NULL;
	}

	pComponent = (Component*)pRes->resval.rlong;

    acutRelRb(pRes);
	pEnt->close();

	return pComponent;
}
