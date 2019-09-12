// Kwik3DPolygonMesh.h: interface for the Kwik3DPolygonMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KWIK3DPOLYGONMESH_H__97D47814_9D67_11D4_9FE6_0008C7999B1D__INCLUDED_)
#define AFX_KWIK3DPOLYGONMESH_H__97D47814_9D67_11D4_9FE6_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dbents.h"


class Kwik3DPolygonMesh : public AcDbPolygonMesh
{
public:
	Kwik3DPolygonMesh();
    Kwik3DPolygonMesh(AcDb::PolyMeshType pType,
                    Adesk::Int16       mSize,
                    Adesk::Int16       nSize,
                    const AcGePoint3dArray& vertices,
                    Adesk::Boolean     mClosed = Adesk::kTrue,
                    Adesk::Boolean     nClosed = Adesk::kTrue);
protected:	 virtual Acad::ErrorStatus   subgetOsnapPoints(	AcDb::OsnapMode     osnapMode,
										int                 gsSelectionMark,
										const AcGePoint3d&  pickPoint,
										const AcGePoint3d&  lastPoint,
										const AcGeMatrix3d& viewXform,
										AcGePoint3dArray&   snapPoints,
										AcDbIntArray&       geomIds) const;
	virtual ~Kwik3DPolygonMesh();

};

#endif // !defined(AFX_KWIK3DPOLYGONMESH_H__97D47814_9D67_11D4_9FE6_0008C7999B1D__INCLUDED_)
