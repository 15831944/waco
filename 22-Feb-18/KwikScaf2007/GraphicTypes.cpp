// GraphicTypes.cpp: implementation of the Matrix3D class.
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
#include "GraphicTypes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Matrix3D::Matrix3D()
{
	AcGeMatrix3d::setToIdentity();
}

Matrix3D::Matrix3D( const Matrix3D &Original )
{
	*this = Original;
}

Matrix3D::Matrix3D( const AcGeMatrix3d &Original )
{
	*this = Original;
}

Matrix3D::~Matrix3D()
{

}

Matrix3D &Matrix3D::operator=( const Matrix3D &Original )
{
	int i, j;
	for( i=0; i<4; i++ )
	{
		for( j=0; j<4; j++ )
		{
			 entry[i][j] = Original.entry[i][j];
		}
	}
	return *this;
}

Matrix3D &Matrix3D::operator=( const AcGeMatrix3d &Original )
{
	int i, j;
	for( i=0; i<4; i++ )
	{
		for( j=0; j<4; j++ )
		{
			 entry[i][j] = Original.entry[i][j];
		}
	}
	return *this;
}

void Matrix3D::Serialize( CArchive &ar )
{
	int i, j;
	if( ar.IsStoring() )
	{
		for( i=0; i<4; i++ )
		{
			for( j=0; j<4; j++ )
			{
				ar << entry[i][j];
			}
		}
	}
	else
	{
		for( i=0; i<4; i++ )
		{
			for( j=0; j<4; j++ )
			{
				ar >> entry[i][j];
			}
		}
	}
}


Point3DArray::Point3DArray()
{

}

Point3DArray::~Point3DArray()
{
	RemoveAll();
}

void	Point3DArray::RemoveAll()
{
	while( length()>0 )
	{
		removeAt(0);
	}
}

void	Point3DArray::transformBy( Matrix3D &Transform )
{
	int		i, iSize;
	Point3D	pt;

	iSize = length();
	for( i=0; i<iSize; i++ )
	{
		pt = at(i).transformBy( Transform );
		setAt( i, pt );
	}
}
