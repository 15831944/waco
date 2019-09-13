// ObjectPosition.cpp: implementation of the ObjectPosition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "ObjectPosition.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjectPosition::ObjectPosition()
{

}

ObjectPosition::~ObjectPosition()
{

}

ObjectPositionArray::ObjectPositionArray()
{

}

ObjectPositionArray::~ObjectPositionArray()
{
	RemoveAll();
}

int ObjectPositionArray::Add(AcDbObjectId dbObjID, Point3D pt1, Point3D pt2)
{
	int				i;
	bool			bFound = false;
	ObjectPosition	*popElement;

	for( i=0; i<GetSize(); i++ )
	{
		popElement = GetAt(i);
		if( popElement->m_dbObjID==dbObjID )
		{
			bFound = true;
			//update the position
			popElement->m_pt[0] = pt1;
			popElement->m_pt[1] = pt2;
		}
	}

	if( !bFound )
	{
		//add a new element!
		popElement = new ObjectPosition();

		popElement->m_dbObjID = dbObjID;
		popElement->m_pt[0] = pt1;
		popElement->m_pt[1] = pt2;
		i = CArray<ObjectPosition*, ObjectPosition*>::Add( popElement );
	}

	return i;
}

void ObjectPositionArray::RemoveAll()
{
	ObjectPosition	*popElement;
	while( GetSize()>0 )
	{
		popElement = NULL;
		popElement = GetAt(0);
		assert( popElement!=NULL );
		delete popElement;
		RemoveAt(0);
	}
}

bool ObjectPositionArray::FindPoints(AcDbObjectId dbObjID, Point3D &pt1, Point3D &pt2)
{
	int				i;
	bool			bFound = false;
	ObjectPosition	*popElement;

	for( i=0; i<GetSize(); i++ )
	{
		popElement = GetAt(i);
		if( popElement->m_dbObjID==dbObjID )
		{
			bFound = true;
			//update the position
			pt1 = popElement->m_pt[0];
			pt2 = popElement->m_pt[1];
			break;
		}
	}

	return bFound;
}

bool ObjectPositionArray::GetPoints(AcDbObjectId dbObjID, Point3D &pt1, Point3D &pt2)
{
	return FindPoints( dbObjID, pt1, pt2 );
}
