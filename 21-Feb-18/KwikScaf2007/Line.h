// Line.h: interface for the Line class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_H__887AFA64_0A8A_11D4_9F6E_0008C7999B1D__INCLUDED_)
#define AFX_LINE_H__887AFA64_0A8A_11D4_9F6E_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dbents.h>

class Line : public AcDbLine  
{

protected: virtual void sublist() const;
		   public:
	Line();
	Line(const AcGePoint3d& start, const AcGePoint3d& end, CString sDescription );
	virtual ~Line();
	ACRX_DECLARE_MEMBERS(Line);
protected:
	virtual Adesk::Boolean      subworldDraw(AcGiWorldDraw* mode);
	virtual Acad::ErrorStatus   subgetGeomExtents(AcDbExtents& extents) const;
	virtual Acad::ErrorStatus   subtransformBy(const AcGeMatrix3d& xform);
	virtual Acad::ErrorStatus   subgetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const;
//	virtual Acad::ErrorStatus   getGripPoints( AcGePoint3dArray& gripPoints, AcDbIntArray&  osnapModes, AcDbIntArray&  geomIds) const;
	virtual Acad::ErrorStatus	submoveGripPointsAt(  const AcDbIntArray& indices, const AcGeVector3d& offset);
protected:
	CString m_sDescription;
};

#endif // !defined(AFX_LINE_H__887AFA64_0A8A_11D4_9F6E_0008C7999B1D__INCLUDED_)
