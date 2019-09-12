// ObjectPosition.h: interface for the ObjectPosition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTPOSITION_H__38B9F926_35AD_11D4_9F98_0008C7999B1D__INCLUDED_)
#define AFX_OBJECTPOSITION_H__38B9F926_35AD_11D4_9F98_0008C7999B1D__INCLUDED_

#include "GraphicTypes.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ObjectPosition  
{
public:
	Point3D m_pt[2];
	AcDbObjectId m_dbObjID;

	ObjectPosition();
	virtual ~ObjectPosition();

};

#include "afxtempl.h"

class ObjectPositionArray : public CArray<ObjectPosition*, ObjectPosition*>
{
public:
	ObjectPositionArray();
	~ObjectPositionArray();
	bool GetPoints(AcDbObjectId dbObjID, Point3D &pt1, Point3D &pt2);
	bool FindPoints(AcDbObjectId dbObjID, Point3D &pt1, Point3D &pt2 );
	void RemoveAll();
	int Add(AcDbObjectId dbObjID, Point3D pt1, Point3D pt2 );
};


#endif // !defined(AFX_OBJECTPOSITION_H__38B9F926_35AD_11D4_9F98_0008C7999B1D__INCLUDED_)
