// Matrix.h: interface for the Matrix class.
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

#if !defined(AFX_MATRIX_H__4B107B04_64CD_11D3_9EB0_0008C7999B1D__INCLUDED_)
#define AFX_MATRIX_H__4B107B04_64CD_11D3_9EB0_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MatrixElement.h"
#include "GraphicTypes.h"	// Added by ClassView
#include "MeccanoDefinitions.h"	// Added by ClassView

class Matrix : public CArray<MatrixElement*, MatrixElement*>
{
public:
	bool AllSameSystem();
	bool IsCancelAllowed();
	void SetAllowCancel( bool bAllow );
	bool GetCutThroughLabel( CString &sLabel );
	void SetCutThroughLabel( LPCTSTR strLabel );
	CString m_sCutThroughLabel;
	void SetCutThrough( bool bCutThrough );
	bool IsCutThrough();
	void SelectionSort(bool bIncremental=true);
	Entity * DrawLine( Point3D pt1, Point3D pt2, int iColour, PlotSyleEnum eHL=PS_LINE );
	void MoveCrosshairOnly( Matrix3D Transform );
	void SetMoving( bool bMoving );
/*	void StopMovingLevel();
	void MovingLevel();
*/	void DeleteLevel( const Entity *pEntity );
	void SetMatrixLevelsChanged( bool bChanged=true );
	bool HaveMatrixLevelsChanged();
	void RegenLevels();
	Matrix3D GetLevelTransform();
	Matrix3D m_LevelTransform;
	void MoveEntities( AcDbObjectIdArray *paEntities, Matrix3D Transform );
	void DoMoveLevel( AcDbObjectId id );
	void MoveLevel( Entity *pEntity );
	void DeleteAllEntities();
	bool IsMoving();
	void DeleteEntities( AcDbObjectIdArray *pIdArray );
	bool IsCrosshairCreated();
	void SetCrosshairCreated( bool bCreated );
	void Destroy3DCrosshair();
	void Create3DCrosshair();
	void Delete3DView();
	bool RedrawMatrix();

	////////////////////////////////////////////////////////////////
	//Constructon/Destruction
	Matrix();
	virtual ~Matrix();

	////////////////////////////////////////////////////////////////
	//Controller
	void SetController( Controller *pController );
	Controller * GetController();

	////////////////////////////////////////////////////////////////
	//Cleanup
	bool DeleteAllElements( );
	bool DeleteMatrixElement( MatrixElement *pElement );
	bool DeleteMatrixElement( int iID );

	//Positions the MatrixElement objects!
	void ArrangeElements();
	//How many matrix element are there?
	int GetMatrixSize() const;

	////////////////////////////////////////////////////////////////
	//ID
	int GetMatrixElementID( MatrixElement *pMatrixElement ) const;
	MatrixElement * GetMatrixElement( int iID ) const;

	////////////////////////////////////////////////////////////////
	//Movement
	Matrix3D	UnMove();
	void		Move( Matrix3D Transform, bool bStore );
	//Transformation
	void		SetMatrixCrosshairTransform( Matrix3D Transform );
	Matrix3D	GetMatrixCrosshairTransform();
	
	////////////////////////////////////////////////////////////////
	//Add
	int Add( MatrixElement *pMatrixElement );

	////////////////////////////////////////////////////////////////
	//BOM
	void WriteBOMTo( CStdioFile *pFile );


protected:
	bool m_bAllowCancel;
	bool m_bCutThroughLabelSet;
	bool m_bCutThrough;
	bool m_bMatrixLevelsChanged;

	bool				m_bMoving;
	bool				m_bCrosshairCreated;
	Entity				*m_pCrosshair;
	Matrix3D			m_Transform;
	Controller			*m_pController;
	EntityReactor		*m_pCrosshairReactor;
	AcDbObjectIdArray	m_RLIds;
	AcDbObjectIdArray	m_RLIdsText;
	AcDbObjectIdArray	m_Levels;
	AcDbObjectIdArray	m_LevelsLables;

private:

	void DrawLevels( double dXPosStart, double dTextXValue, double dLineLength );
	void DrawRLScale( double dXPos, double dRLMin, double dRLMax );
	Entity * DrawText( Point3D pt, CString &sText, int iColour, double dTextHeightMultiplier=1.0 );
	Entity * DrawHorizontalLine( Point3D pt, double dLength, int iColour, PlotSyleEnum eHL=PS_LINE, int iLevelLine=-1 );
	void GetRLExtents( double &dRLMax, double &dRLMin );
	void DrawRLs();
};

#endif // !defined(AFX_MATRIX_H__4B107B04_64CD_11D3_9EB0_0008C7999B1D__INCLUDED_)
