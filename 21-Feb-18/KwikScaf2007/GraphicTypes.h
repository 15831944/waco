//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              GraphicTypes
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 19/05/00 12:03p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/GRAPHICTYPES.H $
//	Version			: $Revision: 6 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//This is the list of various graphics types and objects
///////////////////////////////////////////////////////////////////////////////

// GraphicTypes.h: interface for the Matrix3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICTYPES_H__A3C209A4_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_)
#define AFX_GRAPHICTYPES_H__A3C209A4_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gepnt3d.h> 
#include <gevec3d.h>
#include <gemat3d.h>

/////////////////////////////////////////////////////////////////////
//3D Graphics Types
typedef AcGePoint3d			Point3D;
typedef AcGeVector3d		Vector3D;
typedef AcGeVector3dArray	Vector3DArray;

class Matrix3D : public AcGeMatrix3d  
{
public:
	Matrix3D();
	virtual ~Matrix3D();
	Matrix3D( const Matrix3D &Original );
	Matrix3D( const AcGeMatrix3d &Original );

	Matrix3D &operator=( const Matrix3D &Original );
	Matrix3D &operator=( const AcGeMatrix3d &Original );
	void Serialize( CArchive &ar );
};


class Point3DArray : public AcGePoint3dArray
{
public:
	Point3DArray();
	~Point3DArray();
	void RemoveAll();
	void transformBy( Matrix3D &Transform );
};

#endif // !defined(AFX_GRAPHICTYPES_H__A3C209A4_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_)



/*******************************************************************************
*	History Records
********************************************************************************
* $History: GraphicTypes.h $
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 24/09/99   Time: 3:45p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
*
*******************************************************************************/
