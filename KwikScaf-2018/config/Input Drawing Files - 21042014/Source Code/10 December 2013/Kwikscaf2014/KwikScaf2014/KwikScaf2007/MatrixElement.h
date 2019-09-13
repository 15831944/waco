// MatrixElement.h: interface for the MatrixElement class.
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

#if !defined(AFX_MATRIXELEMENT_H__A3C209A5_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_)
#define AFX_MATRIXELEMENT_H__A3C209A5_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_

#include "GraphicTypes.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Includes
#include "Bay.h"
#include "MeccanoDefinitions.h"
#include "dbgroup.h"
#include "componentList.h"	// Added by ClassView
#include "BayDefinitions.h"	// Added by ClassView


//forward declarations
class Matrix;

class MatrixElement  
{
public:


	////////////////////////////////////////////////////////////////////////////////
	//Star separtaions
	double	JackLength() const;
	double	RLAdjust() const;
	void	SetStarSeparation(double dSeparation);
	double	GetStarSeparation() const;

	////////////////////////////////////////////////////////////////////////////////
	//System
	SystemEnum GetSystem( );
	void SetSystem( SystemEnum eSystem );

	////////////////////////////////////////////////////////////////////////////////
	//Entity Groups
	void SetGroup( AcDbGroup *pGroup );
	AcDbGroup * GetGroup();

	/////////////////
	//Functionality//
	/////////////////

	////////////////////////////////////////////////////////////////////////////////
	//Other functions
	SideOfBayEnum	GetSideOfBay( const Component *pComponent ) const;
	void			SetMatrixBayWidthActual( double dWidth );
	double			GetMatrixBayWidthActual();
	bool			IsCutThrough();
	bool			GetCommitted( Bay *pBay, ComponentTypeEnum eType, SideOfBayEnum eSide, double dRL, int &iCommitted );
	CommittedProportionEnum GetCommittedProtion(double dRL, ComponentTypeEnum eType, SideOfBayEnum eSide, SideOfBayEnum eSideAlt);
	bool			GetBoundingBox( AcGePoint2d &ptBottomLeft, AcGePoint2d &ptTopRight );
	void			ConvertStdHeightArrangeToRLArrange( doubleArray &daArrangement, double dBottomRL );
	void			ConvertRLArrangeToStdHeightArrange( doubleArray &daArrangement, bool bReverseOrder );
	Point3DArray	m_ptsStandardPointsOuter;
	Point3DArray	m_ptsStandardPointsInner;

	////////////////////////////////////////////////////////////////////////////////
	//Constrution/Destruction
	MatrixElement();
	virtual			~MatrixElement();
	Controller*		GetController();

	////////////////////////////////////////////////////////////////////////////////
	//ID
	//Queries the Matrix Object to find its number in the list
	int				GetMatrixElementID();
	//Sets the MatrixElementID.  This function should be called from the Matrix.
	void			SetMatrixElementID( int iID );

	////////////////////////////////////////////////////////////////////////////////
	//Matrix
	//Set the pointer to the Matrix object.
	void			SetMatrixPointer( Matrix *pMatrix );
	//Set the pointer to the Matrix object.
	Matrix			*GetMatrixPointer( );
	void			SetMatrixBayWidth( double dWidth );
	double			GetMatrixBayWidth();

	////////////////////////////////////////////////////////////
	//Component Stuff
	Component*		GetComponent( int iID );
	int				GetNumberOfComponents();
	Component*		CreateComponent( ComponentTypeEnum eComponentType, MatrixStandardEnum eSide, int iRise, double dLength, MaterialTypeEnum eMaterialType/*=MT_STEEL*/ );

	////////////////////////////////////////////////////////////////////////////////
	//Transformations within matrix
	//Retrieves the origin location of this element
	Matrix3D		GetTransform();
	void			SetTransform( Matrix3D Transform );

	////////////////////////////////////////////////////////////////////////////////
	//Movement
	//Moves the Origin, and thus the entire object.  Should consider Redrawing the object in this function.
	void			Move( Matrix3D Transform, bool bStore );
	Matrix3D		UnMove();

	////////////////////////////////////////////////////////////////////////////////
	//Labels
	//Labels will have to be dynamically allocated, according to the MatrixElementID, so as to ensure that each matrix element is sequentially numbered (or lettered).  The outer label will be postfixed with a dash symbol
	CString			GetLabel();

	////////////////////////////////////////////////////////////////////////////////
	//Gets
	//Retrieves the Height of a set of Standard.  Note that SetStandardsArrangment would be used to set the height
	double			GetHeight( MatrixStandardEnum eStandards );
	void			GetStandardsArrangement(	doubleArray &daArrangement, bool &bSoleBoard, MatrixStandardEnum eStandards );
	double			GetStandardsHeight(			MatrixStandardEnum eStandards );
	void			GetTransomHeights(			doubleArray &daHeights );
	void			GetDeckingHeights(			doubleArray &daHeights );
	void			GetStageHeights(			doubleArray &daHeights, intArray &iaBoards, MatrixStandardEnum eStandards );
	void			GetHopupRailsHeights(		doubleArray &daHeights, intArray &iaBoards, MatrixStandardEnum eStandards );
	void			GetEndHandrailHeights(		doubleArray &daHeights );
	void			GetEndMidrailHeights(		doubleArray &daHeights );
	void			GetHandrailHeights(		doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			GetLedgerHeights(		doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			GetMidrailHeights(			doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			GetToeBoardHeights(		doubleArray &daHeights, MatrixStandardEnum eStandards );

	////////////////////////////////////////////////////////////////////////////////
	//Sets
	//This will adjust the composition of the standards, this will also need to recalculate the height value.
	void			SetHeight( double dHeight, MatrixStandardEnum eStandards );
	void			SetStandardsArrangement(	doubleArray &daArrangement, bool bSoleBoard, MatrixStandardEnum eStandards );
	void			SetTransomHeights(			doubleArray &daHeights );
	void			SetDeckingHeights(			doubleArray &daHeights );
	void			SetStageHeights(			doubleArray &daHeights, intArray &iaBoards, MatrixStandardEnum eStandards );
	void			SetHopupRailHeights(		doubleArray &daHeights, intArray &iaBoards, MatrixStandardEnum eStandards );
	void			SetEndHandrailHeights(		doubleArray &daHeights  );
	void			SetEndMidrailHeights(		doubleArray &daHeights  );
	void			SetHandrailHeights(			doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			SetLedgerHeights(			doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			SetMidrailHeights(			doubleArray &daHeights, MatrixStandardEnum eStandards );
	void			SetToeBoardHeights(			doubleArray &daHeights, MatrixStandardEnum eStandards );

	////////////////////////////////////////////////////////////////////////////////
	//Selection of bays
	//Selects the Matrix Element (really selects the corresponding Bays using SelectBays, below.)  It is envisioned that this function would be called by the AutoCAD select routine, such that when the object was selected this routine would be called.
	void			SelectMatrixElement();

	////////////////////////////////////////////////////////////////////////////////
	//Create a new default matrix element
	void			CreateMatrixElement();
	//Call this to delete this matrix element (like destructor).  Normally this would be called by the Matrix object.
	void			DeleteMatrixElement();
	//Posts a message to the Matrix object to destroy this element.
	void			RemoveMatrixElemnt();

	////////////////////////////////////////////////////////////////////////////////
	//Used to create the BOM
	void			WriteBOMTo(CStdioFile *pFile);

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	//Bay stuff
	int				GetNumberOfBays();
	Bay*			GetBay( int iBayID );

	////////////////////////////////////////////////////////////////////////////////
	//bay links
	//Adds a link to a bay into the ListOfBayPointers object
	void			AddBayLink ( int iBayNumber );
	void			AddBayLink ( Bay *pBay );
	//removes a link to a bay object.  If the ListOfBayPointers object is empty then this MatrixElement should be removed.
	bool			RemoveBayLink ( int iBayNumber );
	bool			RemoveBayLink ( Bay *pBay );
	void			RemoveAllBayLinks();


	////////////////////////////////////////////////////////////////////////////////
	//Selects the bays associated with this matrix element.
	void			SelectBays();

	////////////////////////////////////////////////////////////////////////////////
	//Ensures that a Bay exists
	bool			CheckBayExist( int iBayNumber );
	bool			CheckBayExist( Bay *pBay );
	//Ensure that all bays in the selection exist
	bool			CheckBaysExist();

	////////////////////////////////////////////////////////////////////////////////
	//Ensure that a bay matches this matrix
	void			EnsureBaySame( int iBayID );
	void			EnsureBaySame( Bay *pBay );
	//Ensure that all bays pointed to are valid
	void			EnsureBaysSame();

	////////////////////////////////////////////////////////////
	//Draw Stuff
	void			CreateEntities();
	void			Create3DView( );
	void			Delete3DView();
	double			DrawStandards( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterailType );
	void			DrawSoleboards( bool bSoleBoard, double dRL, bool bInner, MaterialTypeEnum eMaterailType);
	void			DrawStages( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawHopupRails( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth );
	void			DrawHandRails( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawLedgers( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawMidRails( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawToeBoards( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawTransoms( doubleArray &daArrangement, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawDeckings( doubleArray &daArrangement, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawEndHandRails( doubleArray &daArrangement, MaterialTypeEnum eMaterailType, double dStandardWidth );
	void			DrawEndMidRails( doubleArray &daArrangement, MaterialTypeEnum eMaterailType, double dStandardWidth );
	double			DrawLabel( double dRL );

protected:
	double			m_dMatrixStarSeparation;
	SystemEnum		m_eSystem;
	AcDbGroup		*m_pElementGroup;
	double			m_dMatrixBayWidthActual;
	bool			m_bDisplay;
	AcGePoint2d		m_ptTopRight;
	AcGePoint2d		m_ptBottomLeft;
	double			m_dMatrixBayWidth;
	ComponentList	m_caComponents;
	Point3DArray	m_Points;

	//////////////
	//Attributes//
	//////////////

	int				m_MatrixElementID;

	//a pointer to the Matrix
	Matrix			*m_pMatrix;
	//Transformation Matrix for this element
	Matrix3D		m_Transform;

	//////////////////////////////////////////////////////////////////////////////	
	//Standards
	doubleArray			m_daStandardsArrangements[MATRIX_STANDARDS_SIZE];
	//this is a total height for each of the above standards, stored here for speed reasons.
	double				m_daHeight[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daStagesHeights[MATRIX_STANDARDS_SIZE];
	intArray			m_iaStagesPlanks[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daHopupRailsHeights[MATRIX_STANDARDS_SIZE];
	intArray			m_iaHopupRailsPlanks[MATRIX_STANDARDS_SIZE];
	bool				m_bSoleBoard[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daDeckHeights;
	doubleArray			m_daTransomHeights;
	doubleArray			m_daLedgerHeights[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daHandrailHeights[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daMidrailHeights[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daToeBoardHeights[MATRIX_STANDARDS_SIZE];
	doubleArray			m_daEndHandrailHeights;
	doubleArray			m_daEndMidrailHeights;

	//ListOfBayPointers - This is a list of pointers to various Bays that are represented by this matrix element, each bay may be including in this list once or twice (only), since if the Bay has 4 standards, then it will have two matrix pointers!
	BayList				m_Bays;

	double				m_dBorderTop;
	double				m_dBorderRight;
	double				m_dBorderBottom;
	double				m_dBorderLeft;

private:
	int					GetCommittedColour( double dRL, ComponentTypeEnum eType, SideOfBayEnum eSide, SideOfBayEnum eSideAlt );

	//I have made this function global
//	CString				ConvertNumberToString( int iNumber );
};

#endif // !defined(AFX_MATRIXELEMENT_H__A3C209A5_64A9_11D3_9EB0_0008C7999B1D__INCLUDED_)
