// Kwik3DPolygonMesh.cpp: implementation of the Kwik3DPolygonMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "Kwik3DPolygonMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Kwik3DPolygonMesh::Kwik3DPolygonMesh()
{

}

Kwik3DPolygonMesh::Kwik3DPolygonMesh(AcDb::PolyMeshType pType,
                    Adesk::Int16       mSize,
                    Adesk::Int16       nSize,
                    const AcGePoint3dArray& vertices,
                    Adesk::Boolean     mClosed,
                    Adesk::Boolean     nClosed ) :
		AcDbPolygonMesh( pType, mSize, nSize, vertices, mClosed, nClosed )
{

}
Kwik3DPolygonMesh::~Kwik3DPolygonMesh()
{

}

Acad::ErrorStatus	Kwik3DPolygonMesh::subgetOsnapPoints(	AcDb::OsnapMode     osnapMode,
						int                 gsSelectionMark,	const AcGePoint3d&  pickPoint,
						const AcGePoint3d&  lastPoint,			const AcGeMatrix3d& viewXform,
						AcGePoint3dArray&   snapPoints,			AcDbIntArray&       geomIds) const
{
	return Acad::eOk;
}
