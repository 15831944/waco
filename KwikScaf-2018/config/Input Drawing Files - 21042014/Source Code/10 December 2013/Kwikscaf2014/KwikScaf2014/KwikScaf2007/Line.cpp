// Line.cpp: implementation of the Line class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "Line.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ACRX_DXF_DEFINE_MEMBERS(Line, AcDbLine,\
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,\
    0, LINE, KWIKSCAF);

Line::Line()
{

}

Line::Line(const AcGePoint3d& start, const AcGePoint3d& end, CString sDescription ) :
	AcDbLine( start, end )
{
	m_sDescription = sDescription;
}

Line::~Line()
{

}

void Line::sublist() const
{
	assertReadEnabled();
	AcDbEntity::list();
	acutPrintf( _T("\t\t%s\n"), m_sDescription );
}

Adesk::Boolean      Line::subworldDraw(AcGiWorldDraw* mode)
{
	return AcDbLine::worldDraw(mode);
}

Acad::ErrorStatus   Line::subgetGeomExtents(AcDbExtents& extents) const
{
	return AcDbLine::getGeomExtents(extents);
}

Acad::ErrorStatus   Line::subtransformBy(const AcGeMatrix3d& xform)
{
	xDataTransformBy( xform );
	return AcDbLine::transformBy(xform);
}

Acad::ErrorStatus   Line::subgetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
{
	return AcDbLine::getTransformedCopy( xform, ent);
}

/*
Acad::ErrorStatus   Line::getGripPoints( AcGePoint3dArray& gripPoints, AcDbIntArray&  osnapModes, AcDbIntArray&  geomIds) const
{
	return AcDbLine::getGripPoints( gripPoints, osnapModes, geomIds);
}
*/
Acad::ErrorStatus	Line::submoveGripPointsAt(  const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	return AcDbLine::moveGripPointsAt( indices, offset);
}

