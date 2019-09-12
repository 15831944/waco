// MatrixElement.cpp: implementation of the MatrixElement class.
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
#include "MatrixElement.h"
#include "Entity.h"
#include "Matrix.h"
#include "Controller.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//MatrixElement
MatrixElement::MatrixElement()
{
	m_MatrixElementID = ID_INVALID;
	m_pMatrix = NULL;
	m_Transform.setToIdentity();

	m_daStandardsArrangements[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daStandardsArrangements[MATRIX_STANDARDS_OUTER].RemoveAll();

	SetHeight( 0.00, MATRIX_STANDARDS_INNER );
	SetHeight( 0.00, MATRIX_STANDARDS_OUTER );
	m_daStagesHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daStagesHeights[MATRIX_STANDARDS_OUTER].RemoveAll();
	m_daHopupRailsHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daHopupRailsHeights[MATRIX_STANDARDS_OUTER].RemoveAll();
	m_daDeckHeights.RemoveAll();
	m_daTransomHeights.RemoveAll();
	m_daLedgerHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daLedgerHeights[MATRIX_STANDARDS_OUTER].RemoveAll();
	m_daHandrailHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daHandrailHeights[MATRIX_STANDARDS_OUTER].RemoveAll();
	m_daMidrailHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daMidrailHeights[MATRIX_STANDARDS_OUTER].RemoveAll();
	m_daToeBoardHeights[MATRIX_STANDARDS_INNER].RemoveAll();
	m_daToeBoardHeights[MATRIX_STANDARDS_OUTER].RemoveAll();

	RemoveAllBayLinks();

	m_dBorderTop	= 200.00;
	m_dBorderRight	= 200.00;
	m_dBorderBottom	= 200.00;
	m_dBorderLeft	= 200.00;

	m_ptBottomLeft.set( 0.00, 0.00 );
	m_ptTopRight.set( 0.00, 0.00 );
	m_bDisplay = false;
	SetGroup( NULL );
}

////////////////////////////////////////////////////////////////////////////////
//~MatrixElement
MatrixElement::~MatrixElement()
{
}


////////////////////////////////////////////////////////////////////////////////
//GetMatrixElementID
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GetMatrixElementID
//Queries the Matrix Object to find its number in the list
int MatrixElement::GetMatrixElementID()
{
	return m_MatrixElementID;
}

////////////////////////////////////////////////////////////////////////////////
//SetMatrixElementID
//Sets the MatrixElementID.  This function should be called from the Matrix.
void MatrixElement::SetMatrixElementID( int iID )
{
	int			iOldID, iMovement;
	Matrix3D	Transform;
	Vector3D	Vector;

	iOldID = m_MatrixElementID;
	m_MatrixElementID = iID;

	m_bDisplay = ( GetController()->EditingStandards() || GetController()->ShowMatrixElement(iID) );
	if( IsCutThrough() )
	{
		Vector.set( 0.00, 0.00, 0.00 );
		if( iID>0 && GetMatrixPointer()!=NULL )
		{
			MatrixElement *pPrevious;
			for( int i=0; i<iID; i++ )
			{
				pPrevious = GetMatrixPointer()->GetMatrixElement(i);
				if( pPrevious!=NULL )
				{
					Vector.x+= pPrevious->GetMatrixBayWidthActual();
				}
			}
		}
	}
	else
	{
		Vector.set( (GetController()->HowManyElementsShownBefore(iID)+1)*MATRIX_SEPARATION_ALONG_X, 0.00, 0.00 );
	}

	Transform.setTranslation( Vector );
	SetTransform( Transform );
	if( !IsCutThrough() && iOldID>=0 )
	{
		//we have inserted a new matrix element in the list, so we need to
		//	move the existing one
		iMovement = iID-iOldID;
		Vector.set( iID*MATRIX_SEPARATION_ALONG_X, 0.00, 0.00 );
		Transform.setTranslation( Vector );
		Move( Transform, false );
	}
}

////////////////////////////////////////////////////////////////////////////////
//Matrix Pointer
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//SetMatrixPointer
//Set the pointer to the Matrix object.
void	MatrixElement::SetMatrixPointer( Matrix *pMatrix )
{
	m_pMatrix = pMatrix;
}

////////////////////////////////////////////////////////////////////////////////
//GetMatrixPointer
//Set the pointer to the Matrix object.
Matrix	*MatrixElement::GetMatrixPointer( )
{
	return m_pMatrix;
}

Controller * MatrixElement::GetController()
{
	return GetMatrixPointer( )->GetController();
}


////////////////////////////////////////////////////////////////////////////////
//Transformations within matrix
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GetTransform
//Retrieves the origin location of this element
Matrix3D MatrixElement::GetTransform()
{
	return m_Transform;
}

////////////////////////////////////////////////////////////////////////////////
//SetTransform
void MatrixElement::SetTransform( Matrix3D Transform )
{
	m_Transform = Transform;
}


////////////////////////////////////////////////////////////////////////////////
//Movement
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Move
//Moves the Origin, and thus the entire object.  Should consider Redrawing the object in this function.
void MatrixElement::Move( Matrix3D Transform, bool bStore )
{
	int			i, iSize;
	Point3D		pt;
	Component	*pComponent;

	iSize = GetNumberOfComponents();
	for( i=0; i<iSize; i++ )
	{
		pComponent = GetComponent(i);
		pComponent->Move( Transform, false );
	}

	for( i=0; i<m_ptsStandardPointsInner.length(); i++ )
	{
		pt = m_ptsStandardPointsInner[i];
		pt.transformBy( Transform );
		m_ptsStandardPointsInner[i] = pt;
	}

	for( i=0; i<m_ptsStandardPointsOuter.length(); i++ )
	{
		pt = m_ptsStandardPointsOuter[i];
		pt.transformBy( Transform );
		m_ptsStandardPointsOuter[i] = pt;
	}

	//Move the bounding box bottom left
	pt.x = m_ptBottomLeft.x;
	pt.y = m_ptBottomLeft.y;
	pt.z = 0.00;
	pt = pt.transformBy( Transform );
	m_ptBottomLeft.x = pt.x;
	m_ptBottomLeft.y = pt.y;

	//Move the bounding box top right
	pt.x = m_ptTopRight.x;
	pt.y = m_ptTopRight.y;
	pt.z = 0.00;
	pt = pt.transformBy( Transform );
	m_ptTopRight.x = pt.x;
	m_ptTopRight.y = pt.y;

	//store it
	if( bStore )
	{
		SetTransform ( Transform * GetTransform() );
	}
}

////////////////////////////////////////////////////////////////////////////////
//UnMove
Matrix3D	MatrixElement::UnMove()
{
	Matrix3D	Original, Inverse;

	//store the original transformation for later use
	Original = GetTransform();

	//we need to move the Bay back to its original position
	Inverse = GetTransform();
	Inverse.invert();

	Move( Inverse, true );

	//return the original matrix
	return Original;
}


////////////////////////////////////////////////////////////////////////////////
//Labels
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GetLabel
//Labels will have to be dynamically allocated, according to the MatrixElementID,
//	so as to ensure that each matrix element is sequentially numbered (or
//	lettered).  The outer label will be postfixed with a dash symbol
CString	MatrixElement::GetLabel( )
{
	CString sLabel;
	ConvertNumberToString( m_MatrixElementID, sLabel );
	return sLabel;
}


////////////////////////////////////////////////////////////////////////////////
//Gets
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GetHeight
//Retrieves the Height of a set of Standard.  Note that SetStandardsArrangment would be used to set the height
double MatrixElement::GetHeight( MatrixStandardEnum eStandards )
{
	return m_daHeight[eStandards];
}

//////////////////////////////////////////////////////////////////////////////////
//GetStandardsArrangement
void MatrixElement::GetStandardsArrangement( doubleArray &daArrangement, bool &bSoleBoard, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	daArrangement.RemoveAll();
	iSize = m_daStandardsArrangements[eStandards].GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daStandardsArrangements[eStandards].GetAt( i );
		daArrangement.Add( dHeight );
	}
	bSoleBoard = m_bSoleBoard[eStandards];
}

//////////////////////////////////////////////////////////////////
//GetTransomHeights
void MatrixElement::GetTransomHeights( doubleArray &daHeights )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daTransomHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daTransomHeights.GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetDeckingHeights
void MatrixElement::GetDeckingHeights( doubleArray &daHeights )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daDeckHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daDeckHeights.GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetStageHeights
void MatrixElement::GetStageHeights( doubleArray &daHeights, intArray &iaPlanks, MatrixStandardEnum eStandards )
{
	int		i, iSize, iPlanks;
	double	dHeight;

	daHeights.RemoveAll();
	iaPlanks.RemoveAll();
	iSize = m_daStagesHeights[eStandards].GetSize();
	;//assert( m_iaStagesPlanks[eStandards].GetSize()==iSize );
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daStagesHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
		iPlanks = m_iaStagesPlanks[eStandards].GetAt( i );
		iaPlanks.Add( iPlanks );
	}
}

//////////////////////////////////////////////////////////////////
//GetHopupRailHeights
void MatrixElement::GetHopupRailsHeights( doubleArray &daHeights, intArray &iaPlanks, MatrixStandardEnum eStandards )
{
	int		i, iSize, iPlanks;
	double	dHeight;

	daHeights.RemoveAll();
	iaPlanks.RemoveAll();
	iSize = m_daHopupRailsHeights[eStandards].GetSize();
	;//assert( m_iaHopupRailsPlanks[eStandards].GetSize()==iSize );
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daHopupRailsHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
		iPlanks = m_iaHopupRailsPlanks[eStandards].GetAt( i );
		iaPlanks.Add( iPlanks );
	}
}

//////////////////////////////////////////////////////////////////
//GetEndHandrailHeights
void MatrixElement::GetEndHandrailHeights( doubleArray &daHeights )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daEndHandrailHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daEndHandrailHeights.GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetEndMidrailHeights
void MatrixElement::GetEndMidrailHeights( doubleArray &daHeights )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daEndMidrailHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daEndMidrailHeights.GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetHandrailHeights
void MatrixElement::GetHandrailHeights( doubleArray &daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daHandrailHeights[eStandards].GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daHandrailHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetLedgerHeights
void MatrixElement::GetLedgerHeights( doubleArray &daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daLedgerHeights[eStandards].GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daLedgerHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//GetMidrailHeights
void MatrixElement::GetMidrailHeights( doubleArray &daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daMidrailHeights[eStandards].GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daMidrailHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
	}
}


//////////////////////////////////////////////////////////////////
//GetToeBoardsHeights
void MatrixElement::GetToeBoardHeights( doubleArray &daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	daHeights.RemoveAll();
	iSize = m_daToeBoardHeights[eStandards].GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = m_daToeBoardHeights[eStandards].GetAt( i );
		daHeights.Add( dHeight );
	}
}

////////////////////////////////////////////////////////////////////////////////
//Sets
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//SetHeight
//This will adjust the composition of the standards, this will also need to recalculate the height value.
void MatrixElement::SetHeight( double dHeight, MatrixStandardEnum eStandards )
{
	m_daHeight[eStandards] = dHeight;
}

//////////////////////////////////////////////////////////////////
//SetStandardsArrangement
void MatrixElement::SetStandardsArrangement( doubleArray &daArrangement, bool bSoleBoard, MatrixStandardEnum eStandards/*=INNER*/ )
{
	int i, iSize;
	double	dHeight, dPoleHeight;

	m_daStandardsArrangements[eStandards].RemoveAll();
	iSize = daArrangement.GetSize();
	dPoleHeight = 0.00;
	if( iSize>0 )
	{
		dPoleHeight = daArrangement.GetAt( iSize-1 ) - daArrangement.GetAt( 0 );
		for( i=0; i<iSize; i++ )
		{
			dHeight = daArrangement.GetAt( i );
			m_daStandardsArrangements[eStandards].Add( dHeight );
		}
	}

	SetHeight( dPoleHeight, eStandards );

	m_bSoleBoard[eStandards] = bSoleBoard;
}

//////////////////////////////////////////////////////////////////
//SetTransomHeights
void MatrixElement::SetTransomHeights( doubleArray &daHeights )
{
	int i, iSize;
	double	dHeight;

	m_daTransomHeights.RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daTransomHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetDeckingHeights
void MatrixElement::SetDeckingHeights( doubleArray & daHeights )
{
	int i, iSize;
	double	dHeight;

	m_daDeckHeights.RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daDeckHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetStageHeights
void MatrixElement::SetStageHeights( doubleArray & daHeights,
						intArray &iaBoards, MatrixStandardEnum eStandards )
{
	int i, iSize, iPlanks;
	double	dHeight;

	m_daStagesHeights[eStandards].RemoveAll();
	m_iaStagesPlanks[eStandards].RemoveAll();
	iSize = daHeights.GetSize();

	;//assert( iaBoards.GetSize()==iSize );
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daStagesHeights[eStandards].Add( dHeight );
		iPlanks = iaBoards.GetAt( i );
		m_iaStagesPlanks[eStandards].Add( iPlanks );
	}
}

//////////////////////////////////////////////////////////////////
//SetStageHeights
void MatrixElement::SetHopupRailHeights( doubleArray & daHeights,
						intArray &iaBoards, MatrixStandardEnum eStandards )
{
	int i, iSize, iPlanks;
	double	dHeight;

	m_daHopupRailsHeights[eStandards].RemoveAll();
	m_iaHopupRailsPlanks[eStandards].RemoveAll();
	iSize = daHeights.GetSize();
	;//assert( iaBoards.GetSize()==iSize );

	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daHopupRailsHeights[eStandards].Add( dHeight );
		iPlanks = iaBoards.GetAt( i );
		m_iaHopupRailsPlanks[eStandards].Add( iPlanks );
	}
}

//////////////////////////////////////////////////////////////////
//SetEndHandrailHeights
void MatrixElement::SetEndHandrailHeights( doubleArray & daHeights  )
{
	int i, iSize;
	double	dHeight;

	m_daEndHandrailHeights.RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daEndHandrailHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetEndMidrailHeights
void MatrixElement::SetEndMidrailHeights( doubleArray & daHeights  )
{
	int i, iSize;
	double	dHeight;

	m_daEndMidrailHeights.RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daEndMidrailHeights.Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetHandrailHeights
void MatrixElement::SetHandrailHeights( doubleArray & daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	m_daHandrailHeights[eStandards].RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daHandrailHeights[eStandards].Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetLedgerHeights
void MatrixElement::SetLedgerHeights( doubleArray & daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	m_daLedgerHeights[eStandards].RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daLedgerHeights[eStandards].Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetMidrailHeights
void MatrixElement::SetMidrailHeights( doubleArray & daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	m_daMidrailHeights[eStandards].RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daMidrailHeights[eStandards].Add( dHeight );
	}
}

//////////////////////////////////////////////////////////////////
//SetToeBoardsHeights
void MatrixElement::SetToeBoardHeights( doubleArray & daHeights, MatrixStandardEnum eStandards )
{
	int i, iSize;
	double	dHeight;

	m_daToeBoardHeights[eStandards].RemoveAll();
	iSize = daHeights.GetSize();
	for( i=0; i<iSize; i++ )
	{
		dHeight = daHeights.GetAt( i );
		m_daToeBoardHeights[eStandards].Add( dHeight );
	}
}


////////////////////////////////////////////////////////////////////////////////
//Selection of bays
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//SelectMatrixElement
//Selects the Matrix Element (really selects the corresponding Bays using SelectBays, below.)  It is envisioned that this function would be called by the AutoCAD select routine, such that when the object was selected this routine would be called.
void MatrixElement::SelectMatrixElement()
{
}


////////////////////////////////////////////////////////////////////////////////
//Create a new default matrix element
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//CreateMatrixElement
void MatrixElement::CreateMatrixElement()
{
}

//////////////////////////////////////////////////////////////////
//DeleteMatrixElement
//Call this to delete this matrix element (like destructor).  Normally this would be called by the Matrix object.
void MatrixElement::DeleteMatrixElement()
{
}

//////////////////////////////////////////////////////////////////
//RemoveMatrixElemnt
//Posts a message to the Matrix object to destroy this element.
void MatrixElement::RemoveMatrixElemnt()
{
}

////////////////////////////////////////////////////////////////////////////////
//Used to create the BOM
void MatrixElement::WriteBOMTo(CStdioFile *pFile)
{
	pFile;
	//Reserved for future use!
	1;
}


///////////////////////////////////////////////////////////////////////////////
//Bay stuff
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//bay links
////////////////////////////////////////////////////////////////////////////////


int MatrixElement::GetNumberOfBays()
{
	return m_Bays.GetSize();
}

//////////////////////////////////////////////////////////////////
//AddBayLink 
//Adds a link to a bay into the ListOfBayPointers object
void MatrixElement::AddBayLink( int iBayNumber )
{
	Bay	*pBay;

	;//assert( iBayNumber>0 );
	pBay = GetController()->GetBayFromBayNumber( iBayNumber );

	if( pBay!=NULL )
	{
		AddBayLink( pBay );
	}
}

//////////////////////////////////////////////////////////////////
//AddBayLink 
void MatrixElement::AddBayLink ( Bay *pBay )
{
	m_Bays.Add( pBay );
}

//////////////////////////////////////////////////////////////////
//RemoveBayLink 
//removes a link to a bay object.  If the ListOfBayPointers object is empty then this MatrixElement should be removed.
bool MatrixElement::RemoveBayLink( int iBayNumber )
{
	int		i, iSize;
	Bay		*pBay;

	//go through each bay in the list and remove the link if we find it!
	iSize = GetNumberOfBays();
	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		if( pBay->GetBayNumber()==iBayNumber )
		{
			//found it!
			m_Bays.RemoveAt( i );
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////
//RemoveBayLink 
bool MatrixElement::RemoveBayLink ( Bay *pBay )
{
	int		i, iSize;
	Bay		*pBayTemp;
	bool	bFound;

	//go through each bay in the list and remove the link if we find it!
	iSize = GetNumberOfBays();
	bFound = false;

  // Change made by ~SJ, 22.04.2008
  // This should never have worked even in the earlier version, since the loop size changes within the loop!
  //
	// for( i=0; i<iSize; i++ )
  //
  // is replaced with
  //
  for( i = iSize - 1; i > -1; i-- )
	{
		pBayTemp = GetBay(i);

		if( pBayTemp==pBay )
		{
			//found it!
			bFound = true;

			//remove the link
			if( pBay->GetMatrixElementPointer()==this )
      {
				pBay->SetMatrixElementPointer( NULL, false );
      }

			if( pBay->GetMatrixElementPointerAlt()==this )
      {
				pBay->SetMatrixElementPointer( NULL, true );
      }

			m_Bays.RemoveAt( i );
		}
	}
	return bFound;
}

void MatrixElement::RemoveAllBayLinks()
{
	while( GetNumberOfBays()>0 )
	{
		RemoveBayLink( GetBay(0) );
	}
}

////////////////////////////////////////////////////////////////////////////////
//Selects the bays associated with this matrix element.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//SelectBays
void MatrixElement::SelectBays()
{
	//??? JSB todo 990907 - complete this function!
}


////////////////////////////////////////////////////////////////////////////////
//Ensures that a Bay exists
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//CheckBayExist
bool MatrixElement::CheckBayExist( int iBayNumber )
{
	int		i, iSize;
	Bay		*pBay;

	//go through each bay in the list and remove the link if we find it!
	iSize = GetNumberOfBays();
	for( i=0; i<iSize; i++ )
	{
		pBay = GetBay(i);
		if( pBay->GetBayNumber()==iBayNumber )
		{
			//found it!
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////
//CheckBayExist
bool MatrixElement::CheckBayExist( Bay *pBay )
{
	int		i, iSize;
	Bay		*pBayTemp;

	//go through each bay in the list and remove the link if we find it!
	iSize = GetNumberOfBays();
	for( i=0; i<iSize; i++ )
	{
		pBayTemp = GetBay(i);
		if( pBayTemp==pBay )
		{
			//found it!
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////
//CheckBaysExist
//Ensure that all bays in the selection exist
bool MatrixElement::CheckBaysExist()
{
	//??? JSB todo 990907 - complete this function
	;//assert( false );	//this function has been supperceeded
	return false;
}


////////////////////////////////////////////////////////////////////////////////
//Ensure that a bay matches this matrix
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//EnsureBaySame
void MatrixElement::EnsureBaySame( int iBayID )
{
	iBayID;
	;//assert( false );	//this function has been supperceeded
}

//////////////////////////////////////////////////////////////////
//EnsureBaySame
void MatrixElement::EnsureBaySame( Bay *pBay )
{
	pBay;
	;//assert( false );	//this function has been supperceeded
}

//////////////////////////////////////////////////////////////////
//EnsureBaysSame
//Ensure that all bays pointed to are valid
void MatrixElement::EnsureBaysSame()
{
	//I think this was part of the original design,
	//	but it has now
	;//assert( false );
}

void MatrixElement::Create3DView( )
{
	bool					bSoleBoardI, bSoleBoardO;
	Point3D					ptA, ptB;
	double					dStandardWidth, dTemp;
	intArray				iaPlanksInner, iaPlanksOuter,
							iaHopupRailPlanksInner, iaHopupRailPlanksOuter;
	doubleArray				daDecking,	daEndHandRails,	daEndMidRails,
							daTransoms,
							daStdInner,			daStdOuter,
							daStageInner,		daStageOuter,
							daHopupRailInner,	daHopupRailOuter,
							daHandRailsInner,	daHandRailsOuter,
							daLedgersInner,		daLedgersOuter,
							daToeBoardInner,	daToeBoardOuter,
							daMidRailsInner,	daMidRailsOuter;

	if( !m_bDisplay )
		return;

	/////////////////////////////////////////////////////////////////////////////////
	//Get the heights of the components
	GetStandardsArrangement	( daStdInner,		bSoleBoardI,			MATRIX_STANDARDS_INNER );
	GetStandardsArrangement	( daStdOuter,		bSoleBoardO,			MATRIX_STANDARDS_OUTER );
	GetStageHeights			( daStageInner,		iaPlanksInner,			MATRIX_STANDARDS_INNER );
	GetStageHeights			( daStageOuter,		iaPlanksOuter,			MATRIX_STANDARDS_OUTER );
	GetHopupRailsHeights	( daHopupRailInner,	iaHopupRailPlanksInner,	MATRIX_STANDARDS_INNER );
	GetHopupRailsHeights	( daHopupRailOuter,	iaHopupRailPlanksOuter,	MATRIX_STANDARDS_OUTER );
	GetHandrailHeights		( daHandRailsInner,							MATRIX_STANDARDS_INNER );
	GetHandrailHeights		( daHandRailsOuter,							MATRIX_STANDARDS_OUTER );
	GetLedgerHeights		( daLedgersInner,							MATRIX_STANDARDS_INNER );
	GetLedgerHeights		( daLedgersOuter,							MATRIX_STANDARDS_OUTER );
	GetMidrailHeights		( daMidRailsInner,							MATRIX_STANDARDS_INNER );
	GetMidrailHeights		( daMidRailsOuter,							MATRIX_STANDARDS_OUTER );
	GetToeBoardHeights		( daToeBoardInner,							MATRIX_STANDARDS_INNER );
	GetToeBoardHeights		( daToeBoardOuter,							MATRIX_STANDARDS_OUTER );
	GetTransomHeights		( daTransoms );
	GetDeckingHeights		( daDecking );
	GetEndHandrailHeights	( daEndHandRails );
	GetEndMidrailHeights	( daEndMidRails );

	/////////////////////////////////////////////////////////////////////////////////
	//We need the bay width
	if( !IsCutThrough() )
		;//assert( GetNumberOfBays()>0 );

	//////////////////////////////////////////////////////////////////////////
	//Standards
	dStandardWidth = 0.00;
	if( daStdInner.GetSize()>0 )
		dStandardWidth = DrawStandards( daStdInner, true, MT_STEEL );
	if( daStdOuter.GetSize()>0 )
		dTemp = DrawStandards( daStdOuter, false, MT_STEEL );

	//The rest of this function requires a valid standard width;
	if( dStandardWidth<ROUND_ERROR )
		dStandardWidth = dTemp;
	if( dStandardWidth<ROUND_ERROR )
		dStandardWidth = GetMatrixPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL );
	;//assert( dStandardWidth>ROUND_ERROR );

	//////////////////////////////////////////////////////////////////////////
	//Soleboards
	if( daStdInner.GetSize()>0 )
		DrawSoleboards( bSoleBoardI, daStdInner.GetAt(0), true, MT_TIMBER );
	if( daStdOuter.GetSize()>0 )
		DrawSoleboards( bSoleBoardO, daStdOuter.GetAt(0), false, MT_TIMBER );

	/////////////////////////////////////////////////////////////////////////////////
	//Stages
	DrawStages( daStageInner, iaPlanksInner, true, MT_STEEL, dStandardWidth );
	DrawStages( daStageOuter, iaPlanksOuter, false, MT_STEEL, dStandardWidth );

	/////////////////////////////////////////////////////////////////////////////////
	//HandRails
	DrawHopupRails(	daHopupRailInner,	iaHopupRailPlanksInner,	true,	MT_STEEL, dStandardWidth );
	DrawHopupRails( daHopupRailOuter,	iaHopupRailPlanksOuter,	false,	MT_STEEL, dStandardWidth );
	DrawHandRails(	daHandRailsInner,	true,	MT_STEEL, dStandardWidth );
	DrawHandRails(	daHandRailsOuter,	false,	MT_STEEL, dStandardWidth );
	DrawLedgers(	daLedgersInner,		true,	MT_STEEL, dStandardWidth );
	DrawLedgers(	daLedgersOuter,		false,	MT_STEEL, dStandardWidth );
	DrawMidRails(	daMidRailsInner,	true,	MT_STEEL, dStandardWidth );
	DrawMidRails(	daMidRailsOuter,	false,	MT_STEEL, dStandardWidth );
	DrawToeBoards(	daToeBoardInner,	true,	MT_STEEL, dStandardWidth );
	DrawToeBoards(	daToeBoardOuter,	false,	MT_STEEL, dStandardWidth );

	/////////////////////////////////////////////////////////////////////////////////
	//Transoms
	DrawTransoms( daTransoms, MT_STEEL, dStandardWidth );

	/////////////////////////////////////////////////////////////////////////////////
	//Decking
	DrawDeckings( daDecking, MT_TIMBER, dStandardWidth );

	/////////////////////////////////////////////////////////////////////////////////
	//EndHandRails
	DrawEndHandRails( daEndHandRails, MT_STEEL, dStandardWidth );
	DrawEndMidRails( daEndMidRails, MT_STEEL, dStandardWidth );

	double dRLBottom = 0.00;
	double dRLTop	= 0.00;
	if( IsCutThrough() )
	{
		if( daStdInner.GetSize()>0 && daStdOuter.GetSize()<=0 )
		{
			dRLBottom = daStdInner.GetAt(0);
			dRLTop = daStdInner.GetAt( daStdInner.GetSize()-1);
		}
		else if( daStdInner.GetSize()<=0 && daStdOuter.GetSize()>0 )
		{
			dRLBottom = daStdOuter.GetAt(0);
			dRLTop = daStdOuter.GetAt( daStdOuter.GetSize()-1);
		}
		else if( daStdInner.GetSize()>0 && daStdOuter.GetSize()>0 )
		{
			dRLBottom	= min( daStdInner.GetAt(0), daStdOuter.GetAt(0) );
			dRLTop		= max( daStdInner.GetAt( daStdInner.GetSize()-1), daStdOuter.GetAt( daStdOuter.GetSize()-1) );
		}
		else
		{
			;//assert( false );
		}
	}
	else
	{
		if( daStdInner.GetSize()>0 )
			dRLBottom = DrawLabel( daStdInner.GetAt(0) );

		if( daStdInner.GetSize()>0 && daStdOuter.GetSize()<=0 )
		{
			dRLTop = daStdInner.GetAt( daStdInner.GetSize()-1);
		}
		else if( daStdInner.GetSize()<=0 && daStdOuter.GetSize()>0 )
		{
			dRLTop = daStdOuter.GetAt( daStdOuter.GetSize()-1);
		}
		else if( daStdInner.GetSize()>0 && daStdOuter.GetSize()>0 )
		{
			dRLTop		= max( daStdInner.GetAt( daStdInner.GetSize()-1), daStdOuter.GetAt( daStdOuter.GetSize()-1) );
		}
		else
		{
			;//assert( false );
		}
	}

	/////////////////////////////////////////////////////////////////////
	Point3D	pt;
	//Move the bounding box bottom left
	pt.set( -1.00*(MATRIX_SEPARATION_ALONG_X/2.00), dRLBottom, 0.00 );
	pt.set( 0.00, dRLBottom, 0.00 );
	pt		= pt.transformBy( GetTransform() );
	pt		= pt.transformBy( GetMatrixPointer()->GetMatrixCrosshairTransform() );
	m_ptBottomLeft.x = pt.x;
	m_ptBottomLeft.y = pt.y;

	//Move the bounding box top right
	pt.x	= dStandardWidth + GetMatrixBayWidthActual() + dStandardWidth + (MATRIX_SEPARATION_ALONG_X/2.00);
	pt.y	= dRLTop;
	pt.z	= 0.00;
	pt		= pt.transformBy( GetTransform() );
	pt		= pt.transformBy( GetMatrixPointer()->GetMatrixCrosshairTransform() );
	m_ptTopRight.x = pt.x;
	m_ptTopRight.y = pt.y;

	/////////////////////////////////////////////////////////////////////
	//We have now created all the components, we just need to tell them
	//	to draw themselves, and create a group for this element
	CreateEntities();
}

Component * MatrixElement::CreateComponent(ComponentTypeEnum eComponentType,
							MatrixStandardEnum eSide, int iRise, double dLength,
							MaterialTypeEnum eMaterialType)
{
	int				iCompID;
	Component		*pComponent;
	SideOfBayEnum	eSideOfBay;
	
	pComponent = new Component( dLength, eComponentType, eMaterialType, GetSystem() );
	pComponent->SetMatrixElementPointer( this );

	//Add the new component to the component list
	if( eSide==MATRIX_STANDARDS_INNER)
		eSideOfBay = NORTH_EAST;
	else
		eSideOfBay = SOUTH_EAST;
	iCompID = m_caComponents.AddComponent( eSideOfBay, iRise, pComponent );
	pComponent->CreatePointsIfPossible();

	return pComponent;
}


void MatrixElement::CreateEntities()
{
	int					i, iID, *piGroupNumber;
	TCHAR				cGroupName[255];
	CString				sGroupName;
	Component			*pComponent;
	AcDbObjectId		id;
	AcDbObjectIdArray	objIds;

	/////////////////////////////////////////////////////////////////////
	//Create the entities for each component, store their id's
	for( iID=0; iID<GetNumberOfComponents(); iID++ )
	{
		pComponent = GetComponent(iID);
		if( pComponent->Create3DView() )
		{
			;//assert( pComponent->GetEntity()!=NULL );
			id = pComponent->GetEntity()->objectId();
			objIds.append( id );
		}
	}

	/////////////////////////////////////////////////////////////////////
	//contruct a group
	piGroupNumber = GetController()->GetGroupNumber();
	(*piGroupNumber)++;
	sGroupName.Format( _T("%s%03i"), GROUP_PREFIX_MATRIX, *piGroupNumber );
	for( i=0; i<sGroupName.GetLength(); i++ )
	{
		if( i>30 ) break;
		cGroupName[i] = sGroupName[i];
	}
	cGroupName[i] = _T('\0');

	/////////////////////////////////////////////////////////////////////
	//create the group
	Entity entTemp;
	SetGroup( entTemp.CreateGroup( objIds, cGroupName ) );
}

int MatrixElement::GetNumberOfComponents()
{
	return m_caComponents.GetSize();
}

Component * MatrixElement::GetComponent(int iComponentID)
{
	if( (iComponentID<0) || (iComponentID>GetNumberOfComponents()) )
		return NULL;

	return m_caComponents.GetComponent(iComponentID);
}

Bay * MatrixElement::GetBay(int iBayID)
{
	//Cut throughs don't have a bay pointer
	;//assert( !IsCutThrough() );

	if( (iBayID<0) || (iBayID>GetNumberOfBays()) )
		return NULL;

	return m_Bays.GetAt(iBayID);
}

double MatrixElement::DrawStandards(doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterialType)
{
	int					i;
	int					iColour;
	double				dLength, dLengthActual, dStandardWidth, dj, dStandardRL;
	Entity				pLine;
	CString				sText;
	Vector3D			Vector;
	Point3D				pt;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	dStandardWidth = 0.00;

	;//assert( daArrangement.GetSize()>0 );

	//clear the points array
	if( bInner )
		m_ptsStandardPointsInner.RemoveAll();
	else
		m_ptsStandardPointsOuter.RemoveAll();

	for( i=0; i<daArrangement.GetSize()-1; i++ )
	{

		////////////////////////////////////////////////////////////////////
		//Add the standard
		dStandardRL = daArrangement.GetAt(i);
		dLengthActual = daArrangement.GetAt(i+1) - dStandardRL;
		dLength = ConvertStarRLtoRL(dLengthActual, GetStarSeparation());
		iColour = GetCommittedColour( dStandardRL+(dLength/2.00), CT_STANDARD, bInner? SE: NE, bInner? SW: NW );
		pComponent = CreateComponent( CT_STANDARD, eMS, i, dLength, eMaterialType );

		Vector.set( 0.00, 0.00, 1.00 );
		Rotation.setToRotation( pi/2.00, Vector );
		Transform = Rotation;

		dStandardWidth = pComponent->GetWidthActual();

		Vector.set( 0.00, dStandardRL, 0.00 );
		if( bInner )
		{
			Vector.x+= GetMatrixBayWidthActual();
		}

		Trans.setToTranslation( Vector );
		Transform = Trans * Transform;

		if( bInner )
		{
			//This is the base point of the standard
			pt.set( GetMatrixBayWidthActual(), dStandardRL-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD, 0.00 );
			//Move the standards to where the matrix is
			pt.transformBy( GetTransform()*GetMatrixPointer()->GetMatrixCrosshairTransform() );
			if( i==0 )
			{
				//This is the first standard, we need to componensate for the Jack and Soleboard
				pt.y-= GetMatrixPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_JACK, COMPONENT_LENGTH_0500, MT_STEEL );
				pt.y-= GetMatrixPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_SOLEBOARD, COMPONENT_LENGTH_0500, MT_TIMBER );
			}
			m_ptsStandardPointsInner.append(pt);

			if( i==daArrangement.GetSize()-2 )
			{
				//If this is the last inner standard then add an extra StandardPointInner at the top!
				pt.set( GetMatrixBayWidthActual(), daArrangement.GetAt(i+1)+EXTRA_LENGTH_AT_TOP_OF_STANDARD+EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD, 0.00 );
				pt.transformBy( GetTransform()*GetMatrixPointer()->GetMatrixCrosshairTransform() );
				m_ptsStandardPointsInner.append(pt);
			}
		}
		else
		{
			//This is the base point of the standard
			pt.set( 0.00, dStandardRL-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD, 0.00 );
			//Move the standards to where the matrix is
			pt.transformBy( GetTransform()*GetMatrixPointer()->GetMatrixCrosshairTransform() );
			if( i==0 )
			{
				//This is the first standard, we need to componensate for the Jack and Soleboard
				pt.y-= GetMatrixPointer()->GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_JACK, COMPONENT_LENGTH_0500, MT_STEEL );
				pt.y-= GetMatrixPointer()->GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_SOLEBOARD, COMPONENT_LENGTH_0500, MT_TIMBER );
			}
			m_ptsStandardPointsOuter.append(pt);

			if( i==daArrangement.GetSize()-2 )
			{
				//If this is the last standard then add an extra StandardPointOuter at the top!
				pt.set( 0.00, daArrangement.GetAt(i+1)+EXTRA_LENGTH_AT_TOP_OF_STANDARD+EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD, 0.00 );
				pt.transformBy( GetTransform()*GetMatrixPointer()->GetMatrixCrosshairTransform() );
				m_ptsStandardPointsOuter.append(pt);
			}
		}

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );
		if( iColour>0 )
			pComponent->SetColour( iColour );

		if( !IsCutThrough() )
		{
			if( !(GetController()->GetUseActualComponents()&UAC_MATRIX) )
			{
				////////////////////////////////////////////////////////////////////
				//Add small ticks to indicate the positions of the stars
				for( dj=dStandardRL; dj<dStandardRL+dLengthActual; dj+=GetStarSeparation() )
				{
					pComponent = CreateComponent( CT_TEXT, eMS, 0, 0, MT_STEEL );
					sText = _T("-");
					pComponent->SetText( sText );
					pComponent->SetColour( COLOUR_WHITE );
					pComponent->SetTextHeightMultiplier(0.25);
					Vector.set( 0.00, dj, 0.00 );
					if( bInner )
					{
						pComponent->SetTextHorizontalMode(AcDb::kTextLeft);
						Vector.x+= GetMatrixBayWidthActual()+(dStandardWidth/2.00);
					}
					else
					{
						pComponent->SetTextHorizontalMode(AcDb::kTextRight);
						Vector.x-= (dStandardWidth/2.00);
					}
					Trans.setToTranslation( Vector );
					pComponent->Move( Trans, true );
				}
				////////////////////////////////////////////////////////////////////
				//Draw a longer tick to indicate the separation between the standards
				if( i<daArrangement.GetSize()-2 )
				{
					pComponent = CreateComponent( CT_TEXT, eMS, 0, 0, MT_STEEL );
					sText = _T("\\/\\/\\");
					pComponent->SetText( sText );
					pComponent->SetColour( COLOUR_WHITE );
					pComponent->SetTextHorizontalMode(AcDb::kTextCenter);
					pComponent->SetTextHeightMultiplier(0.20);
					dj-= EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;

					Vector.set( 0.00, dj, 0.00 );
					if( bInner )
					{
						pComponent->SetTextHorizontalMode(AcDb::kTextLeft);
						Vector.x+= GetMatrixBayWidthActual()+(dStandardWidth/2.00);;
					}
					else
					{
						pComponent->SetTextHorizontalMode(AcDb::kTextRight);
						Vector.x-= (dStandardWidth/2.00);
					}
					Trans.setToTranslation( Vector );
					pComponent->Move( Trans, true );
				}
			}
		}

		if( GetController()->GetShowStandardLengths() )
		{
			////////////////////////////////////////////////////////////////////
			//Draw the Length of the standards, half way up its length
			pComponent = CreateComponent( CT_TEXT, eMS, 0, 0, MT_STEEL );
			sText.Format( _T("  %2.1f  "), dLength*CONVERT_MM_TO_M );
			pComponent->SetText( sText );
			pComponent->SetColour( COLOUR_WHITE );
			pComponent->SetTextHeightMultiplier(1.25);
			Vector.set( 0.00, dStandardRL+(dLengthActual/2.00), 0.00 );
			if( bInner )
			{
				if( IsCutThrough() && GetMatrixElementID()<GetMatrixPointer()->GetMatrixSize()-1 )
					pComponent->SetTextHorizontalMode(AcDb::kTextRight);
				else
					pComponent->SetTextHorizontalMode(AcDb::kTextLeft);
				Vector.x+= GetMatrixBayWidthActual();
			}
			else
			{
				pComponent->SetTextHorizontalMode(AcDb::kTextRight);
			}
			Trans.setToTranslation( Vector );
			pComponent->Move( Trans, true );
		}
	}

	////////////////////////////////////////////////////////////
	//Add a Jack
	dLength = COMPONENT_LENGTH_0500;
	pComponent = CreateComponent( CT_JACK, eMS, i, dLength, eMaterialType );

	Vector.set( 0.00, 0.00, 1.00 );
	Rotation.setToRotation( pi/2.00, Vector );
	Transform = Rotation;

	Vector.set( 0.00, daArrangement.GetAt(0)-pComponent->GetLengthActual(), 0.00 );
	if( bInner )
		Vector.x+= GetMatrixBayWidthActual();

	Trans.setToTranslation( Vector );
	Transform = Trans * Transform;

	pComponent->SetDrawMatrixElementSideOn( true );
	pComponent->Move( Transform, true );
	iColour = GetCommittedColour( Vector.y, CT_JACK, bInner? SE: NE, bInner? SW: NW );
	if( iColour>0 )
		pComponent->SetColour( iColour );

	if( daArrangement.GetSize()<=0 )
		return 0.00;

	return dStandardWidth;
}

void MatrixElement::DrawSoleboards( bool bSoleBoard, double dRL, bool bInner, MaterialTypeEnum eMaterialType)
{
	if( bSoleBoard )
	{
		int					iColour;
		Vector3D			Vector;
		Matrix3D			Transform, Rotation, Trans;
		double				dLength, dLengthOfJack, dLengthActualOfJack;
		MatrixStandardEnum	eMS;
		Component			*pComponent;

		eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

		//We need to place this below the jack
		dLengthOfJack = COMPONENT_LENGTH_0500;
		dLengthActualOfJack = GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_JACK, dLengthOfJack, MT_STEEL );

		dLength = COMPONENT_LENGTH_0500;
		pComponent = CreateComponent( CT_SOLEBOARD, eMS, 0, dLength, eMaterialType );

		//Position the Soleboard to just touch the bottom of the jack
		Vector.set( 0.00,
					((dRL-dLengthActualOfJack)-(pComponent->GetHeightActual()/2.00)),
					0.00 );
		if( bInner )
			Vector.x+= GetMatrixBayWidthActual();
		Trans.setToTranslation( Vector );
		Transform = Trans;
		
		pComponent->SetDrawMatrixElementSideOn( false );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( dRL-dLengthActualOfJack, CT_SOLEBOARD, bInner? SE: NE, bInner? SW: NW );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawStages( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i, j;
	double				dLength, dHopUpLength, dSpace;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	int	iColour;
	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_HOPUP_BRACKET, bInner? SSE: NNE, bInner? SSW: NNW );

		dLength = (double)iaPlanks.GetAt(i);
		pComponent = CreateComponent( CT_HOPUP_BRACKET, eMS, LIFT_RISE_0000MM, dLength, eMaterialType );

		if( bInner )
		{
			Transform.setToIdentity();
		}
		else
		{
			Vector.set( 0.00, 1.00, 0.00 );
			Rotation.setToRotation( pi, Vector );
			Transform = Rotation;
		}

		Vector.set( 0.00, (daArrangement.GetAt(i)-(pComponent->GetHeightActual()/2.00)), 0.00 );
		if( bInner )
		{
			//Move if to the other side
			Vector.x+= GetMatrixBayWidthActual();
			//The Bracket sits outside the standard
			Vector.x+= (dStandardWidth/2.00);
		}
		else
		{
			//The Bracket sits outside the standard
			Vector.x-= (dStandardWidth/2.00);
		}
		Vector.y+= pComponent->GetAcurate3DAdjust( Z_AXIS, true );
		Trans.setToTranslation( Vector );
		Transform = Trans * Transform;

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );

		if( iColour>0 )
			pComponent->SetColour( iColour );

		dHopUpLength = pComponent->GetLengthActual();
		;//assert( dHopUpLength>0 );

		for( j=0; j<iaPlanks.GetAt(i); j++ )
		{
			//The length here is not important, since all the rails are the same width & height
			//	and besides which, this element may represent several lenghts anyway
			pComponent = CreateComponent( CT_STAGE_BOARD, eMS, j, COMPONENT_LENGTH_2400, eMaterialType );

			//The Hopup has a tie bar at the end
			dHopUpLength+= pComponent->GetAcurate3DAdjust(Y_AXIS);

			//How much space would we have left if we put all the boards together?
			dSpace = dHopUpLength-(pComponent->GetWidthActual()*iaPlanks.GetAt(i));
			if( iaPlanks.GetAt(i)>1 )
			{
				//There is more that one plank, so if we had 2 planks there would be
				//	1 gap between them
				dSpace/= iaPlanks.GetAt(i);
				//The spacing between planks centre-to-centre is
				dSpace+= pComponent->GetWidthActual();
			}

			Vector.set( double(j)*dSpace, daArrangement.GetAt(i), 0.00 );
			if( bInner )
			{
				Vector.x+= GetMatrixBayWidthActual();
			}
			Vector.x+= (dStandardWidth/2.00);
			Vector.x+= (pComponent->GetWidthActual()/2.00);
			Vector.y+= (pComponent->GetHeightActual()/2.00);
			Vector.y+= pComponent->GetAcurate3DAdjust( Z_AXIS, true );
			Trans.setToTranslation( Vector );
			Transform = Trans;

			if( !bInner )
			{
				Vector.set( 0.00, 1.00, 0.00 );
				Rotation.setToRotation( pi, Vector );
				Transform = Rotation * Transform;
			}

			pComponent->SetDrawMatrixElementSideOn( false );
			pComponent->Move( Transform, true );

			if( iColour>0 )
				pComponent->SetColour( iColour );
		}
	}
}

void MatrixElement::DrawHopupRails( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth)
{
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	double				dLength;
	int					i;
	MatrixStandardEnum	eMS;
	Component			*pComponent;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		dLength = (double)iaPlanks.GetAt(i);
		pComponent = CreateComponent( CT_HOPUP_BRACKET, eMS, LIFT_RISE_1000MM, dLength, eMaterialType );

		if( bInner )
		{
			Transform.setToIdentity();
		}
		else
		{
			Vector.set( 0.00, 1.00, 0.00 );
			Rotation.setToRotation( pi, Vector );
			Transform = Rotation;
		}

		Vector.set( 0.00, (daArrangement.GetAt(i)-(pComponent->GetHeightActual()/2.00)), 0.00 );
		if( bInner )
		{
			//Move if to the other side
			Vector.x+= GetMatrixBayWidthActual();
			//The Bracket sits outside the standard
			Vector.x+= (dStandardWidth/2.00);
		}
		else
		{
			//The Bracket sits outside the standard
			Vector.x-= (dStandardWidth/2.00);
		}
		Vector.y+= pComponent->GetAcurate3DAdjust( Z_AXIS, true );
		Trans.setToTranslation( Vector );
		Transform = Trans * Transform;

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_HOPUP_BRACKET, bInner? SSE: NNE, bInner? SSW: NNW );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawHandRails( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	int					i;
	MatrixStandardEnum	eMS;
	Component			*pComponent;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		//The length here is not important, since all the rails are the same width & height
		//	and besides which, this element may represent several lenghts anyway
		pComponent = CreateComponent( CT_RAIL, eMS, LIFT_RISE_1000MM, COMPONENT_LENGTH_2400, eMaterialType );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_RAIL, bInner? S: N, bInner? S: N );

		Vector.set( 0.00, daArrangement.GetAt(i), 0.00 );
		if( bInner )
			Vector.x+= GetMatrixBayWidthActual();
		Vector.y+= (pComponent->GetHeightActual()/2.00);
		Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS, false );
		Trans.setToTranslation( Vector );
		Transform = Trans;

		pComponent->SetDrawMatrixElementSideOn( false );
		pComponent->Move( Transform, true );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawLedgers( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	int					i;
	MatrixStandardEnum	eMS;
	Component			*pComponent;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_LEDGER, eMS, LIFT_RISE_0000MM, COMPONENT_LENGTH_2400, eMaterialType );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_LEDGER, bInner? S: N, bInner? S: N );

		//Position the ledger
		Vector.set( 0.00, daArrangement.GetAt(i)+(pComponent->GetHeightActual()/2.00), 0.00 );
		if( bInner )
			Vector.x+= GetMatrixBayWidthActual();
		Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS);
		Trans.setToTranslation( Vector );
		Transform = Trans;

		//This is an end on component
		pComponent->SetDrawMatrixElementSideOn( false );
		pComponent->Move( Transform, true );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawMidRails( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i;
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_MID_RAIL, eMS, LIFT_RISE_0500MM, COMPONENT_LENGTH_2400, eMaterialType );

		Vector.set( 0.00, daArrangement.GetAt(i), 0.00 );
		if( bInner )
			Vector.x+= GetMatrixBayWidthActual();
		Vector.y+= (pComponent->GetHeightActual()/2.00);
		Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS, false);
		Trans.setToTranslation( Vector );
		Transform = Trans;

		pComponent->SetDrawMatrixElementSideOn( false );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_MID_RAIL, bInner? S: N, bInner? S: N );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawToeBoards( doubleArray &daArrangement, bool bInner, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i;
	int					iColour;
	double				dPlankHeight;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = ( bInner )? MATRIX_STANDARDS_INNER: MATRIX_STANDARDS_OUTER;

	dPlankHeight = GetController()->GetCompDetails()->GetActualHeight( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL );
	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_TOE_BOARD, eMS, LIFT_RISE_0000MM, COMPONENT_LENGTH_2400, eMaterialType );

		Vector.set( 0.00, 0.00, 1.00 );
		Rotation.setToRotation( pi/2.00, Vector ); 

		Vector.set( 0.00, daArrangement.GetAt(i), 0.00 );
		//The width of the bay - the width of the standard if on the RHS
		if( bInner )
		{
			Vector.x+= GetMatrixBayWidthActual();
			//Position on the inside of the standard
			Vector.x-= (pComponent->GetHeightActual()/2.00);
			Vector.x-= (dStandardWidth/2.00);
		}
		else
		{
			//Position on the inside of the standard
			Vector.x+= (pComponent->GetHeightActual()/2.00);
			Vector.x+= (dStandardWidth/2.00);
		}
		//Toeboards sit on top of the other boards
		Vector.y+= dPlankHeight;
		Vector.y+= (pComponent->GetWidthActual()/2.00);
		Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS,false);

		Trans.setToTranslation( Vector );
		Transform = Trans * Rotation;

		pComponent->SetDrawMatrixElementSideOn( false );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_TOE_BOARD, bInner? S: N, bInner? S: N );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawTransoms( doubleArray &daArrangement, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i;
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = MATRIX_STANDARDS_INNER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_TRANSOM, eMS, LIFT_RISE_0000MM, GetMatrixBayWidth(), eMaterialType );

		//Position the transom, so that the bottom of the Wantfa is resting directly
		//	on the top of the higher star
		Vector.set( (dStandardWidth/2.00), daArrangement.GetAt(i), 0.00 );
		Vector.y+= pComponent->GetHeightActual()/2.00;
		Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS);
		Trans.setToTranslation( Vector );
		Transform = Trans;

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );
		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_TRANSOM, E, W );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawDeckings( doubleArray &daArrangement, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i, j, iNumberOfPlanks;
	int					iColour;
	double				dPlankWidth, dSpace;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = MATRIX_STANDARDS_INNER;

	//Get a typical plank width
	iNumberOfPlanks = GetNumberOfPlanksForWidth( GetMatrixBayWidth() );
	dPlankWidth = GetController()->GetCompDetails()->GetActualWidth( GetSystem(), CT_DECKING_PLANK, COMPONENT_LENGTH_2400, MT_STEEL );
	//If all the planks were place side by side, ALONG THE TRANSOM
	//	(ie -2*half standard widths), how much space would we have at the end
	dSpace = GetMatrixBayWidthActual() - dStandardWidth - ( dPlankWidth*(double)iNumberOfPlanks );
	;//assert( dSpace>0 );
	//If we have 4 planks then we need this space to form even gaps between the boards
	//	thus we will have 3 gaps
	if( iNumberOfPlanks>1 )
	{
		dSpace/= (double(iNumberOfPlanks-1) );
		//So the centre to centre spacing between planks is
		dSpace+= dPlankWidth;
	}
	else
	{
		dSpace = 0.00;
	}

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_DECKING_PLANK, S, S );

		//Place each plank
		for( j=0; j<iNumberOfPlanks; j++ )
		{
			pComponent = CreateComponent( CT_DECKING_PLANK, eMS, LIFT_RISE_0000MM, COMPONENT_LENGTH_2400, eMaterialType );

			Vector.set( (j*dSpace)+(dStandardWidth/2.00), daArrangement.GetAt(i), 0.00 );
			Vector.x+= (pComponent->GetWidthActual()/2.00);
			Vector.y+= (pComponent->GetHeightActual()/2.00);
			Vector.y+= pComponent->GetAcurate3DAdjust(Z_AXIS);
			Trans.setToTranslation( Vector );
			Transform = Trans;

			pComponent->SetDrawMatrixElementSideOn( false );
			pComponent->Move( Transform, true );
			if( iColour>0 )
				pComponent->SetColour( iColour );
		}
	}
}

void MatrixElement::DrawEndHandRails( doubleArray &daArrangement, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i;
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = MATRIX_STANDARDS_INNER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_RAIL, eMS, LIFT_RISE_1000MM, GetMatrixBayWidth(), eMaterialType );

		Vector.set( (dStandardWidth/2.00), daArrangement.GetAt(i), 0.00 );
		Vector.y+= (pComponent->GetHeightActual()/2.00);
		Vector.y+= pComponent->GetAcurate3DAdjust( Z_AXIS, true );
		Trans.setToTranslation( Vector );
		Transform = Trans;

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_RAIL, E, W );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}

void MatrixElement::DrawEndMidRails( doubleArray &daArrangement, MaterialTypeEnum eMaterialType, double dStandardWidth )
{
	int					i;
	int					iColour;
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	Component			*pComponent;
	MatrixStandardEnum	eMS;

	eMS = MATRIX_STANDARDS_INNER;

	for( i=0; i<daArrangement.GetSize(); i++ )
	{
		pComponent = CreateComponent( CT_MID_RAIL, eMS, LIFT_RISE_0500MM, GetMatrixBayWidth(), eMaterialType );

		Vector.set( (dStandardWidth/2.00), daArrangement.GetAt(i), 0.00 );
		Vector.y+= (pComponent->GetHeightActual()/2.00);
		Vector.y+= pComponent->GetAcurate3DAdjust( Z_AXIS, true );
		Trans.setToTranslation( Vector );
		Transform = Trans;

		pComponent->SetDrawMatrixElementSideOn( true );
		pComponent->Move( Transform, true );

		iColour = GetCommittedColour( daArrangement.GetAt(i), CT_MID_RAIL, E, W );
		if( iColour>0 )
			pComponent->SetColour( iColour );
	}
}


void MatrixElement::Delete3DView()
{
	m_caComponents.DeleteAll();
	RemoveAllBayLinks();
/*
	Component	*pComponent;

	while( GetNumberOfComponents()>0 )
	{
		m_caComponents.DeleteComponent(0);
		pComponent = GetComponent(0);
		if( pComponent!=NULL )
		{
			pComponent->Delete3DView();
			delete pComponent;
		}
		else
		{
			//someone has already deleted this baby
			;//assert( false );
		}
		m_caComponents.RemoveAt(0);
	}
*/
}
/*
CString MatrixElement::ConvertNumberToString(int iNumber)
{
	const int	iLettersInAlphabet = 26;
	CString		sText, sTemp;

	sText.Empty();
	do
	{
		sTemp.Format( "%c", ('A'+(iNumber%iLettersInAlphabet)) );
		sText = sTemp + sText;

		iNumber = int( iNumber/iLettersInAlphabet );
	}
	while( iNumber>0 );
	return sText;
}
*/

double MatrixElement::DrawLabel( double dRL )
{
	Vector3D			Vector;
	Matrix3D			Transform, Rotation, Trans;
	double				dLength, dLengthActualOfJack, dRLPos;
	MatrixStandardEnum	eMS;
	Component			*pComponent;

	eMS = MATRIX_STANDARDS_INNER;

	dLengthActualOfJack = GetController()->GetCompDetails()->GetActualLength( GetSystem(), CT_JACK, COMPONENT_LENGTH_0500, MT_STEEL );

	dLength = 0.00;
	pComponent = CreateComponent( CT_TEXT, eMS, 0, dLength, MT_STEEL );
	pComponent->SetTextHorizontalMode( AcDb::kTextMid );
	pComponent->SetTextVerticalMode( AcDb::kTextBase );

	//-COMPONENT_LENGTH_0500 is for the jack
	dRLPos = (dRL-dLengthActualOfJack)-pComponent->GetHeightActual();
	Vector.set( 0.00, dRLPos, 0.00 );

	Vector.x+= GetMatrixBayWidthActual()/2.00;
	Trans.setToTranslation( Vector );
	Transform = Trans;
	
	pComponent->SetDrawMatrixElementSideOn( false );
	pComponent->Move( Transform, true );

	//allow for the height of the text
	dRLPos -= pComponent->GetHeightActual();

	return dRLPos;
}


double MatrixElement::GetMatrixBayWidth()
{
	return m_dMatrixBayWidth;
}

void MatrixElement::SetMatrixBayWidth(double dWidth)
{
	m_dMatrixBayWidth = dWidth;
}

double MatrixElement::GetMatrixBayWidthActual()
{
	return m_dMatrixBayWidthActual;
}

void MatrixElement::SetMatrixBayWidthActual(double dWidth)
{
	m_dMatrixBayWidthActual = dWidth;
}

double MatrixElement::GetStandardsHeight(MatrixStandardEnum eStandards)
{
	int			i;
	bool		bSoleBoard;
	double		dHeight, dTotal;
	doubleArray	daArrangement;

	GetStandardsArrangement( daArrangement, bSoleBoard, eStandards );
	dTotal = 0.00;
	for( i=0; i<daArrangement.GetSize()-1; i++ )
	{
		dHeight	= daArrangement.GetAt(i+1)-daArrangement.GetAt(i);
		dTotal	+=dHeight;
	}
	return dTotal;
}

void MatrixElement::ConvertRLArrangeToStdHeightArrange(doubleArray &daArrangement, bool bReverseOrder )
{
	int			i;
	double		dValue;
	doubleArray daStdHeightArrange;

	daStdHeightArrange.RemoveAll();
	//The arrangement we get back above was RLs, we need heights, so convert them
	if( bReverseOrder )
	{
		for( i=daArrangement.GetSize()-1; i>0; i-- )
		{
			dValue = ConvertStarRLtoRL(daArrangement.GetAt(i), GetStarSeparation())-ConvertStarRLtoRL(daArrangement.GetAt(i-1), GetStarSeparation());
			daStdHeightArrange.Add( dValue );
		}
	}
	else
	{
		for( i=0; i<daArrangement.GetSize()-1; i++ )
		{
			dValue = ConvertStarRLtoRL(daArrangement.GetAt(i+1), GetStarSeparation())-ConvertStarRLtoRL(daArrangement.GetAt(i), GetStarSeparation());
			daStdHeightArrange.Add( dValue );
		}
	}
	daArrangement = daStdHeightArrange; 
}

void MatrixElement::ConvertStdHeightArrangeToRLArrange(doubleArray &daArrangement, double dBottomRL )
{
	int			i;
	double		dValue;
	doubleArray daRLArrange;

	daRLArrange.RemoveAll();
	//The arrangement we get back above was RLs, we need heights, so convert them
	if( daArrangement.GetSize()>0 )
	{
		daRLArrange.Add(dBottomRL);
		for( i=0; i<daArrangement.GetSize(); i++ )
		{
			dValue = daArrangement.GetAt(i);
			dBottomRL+= dValue;
			daRLArrange.Add( dBottomRL );
		}
	}
	daArrangement = daRLArrange; 
}


bool MatrixElement::GetBoundingBox(AcGePoint2d &ptBottomLeft, AcGePoint2d &ptTopRight)
{
	ptBottomLeft	= m_ptBottomLeft;
	ptTopRight		= m_ptTopRight;

	if( ptBottomLeft.x	== 0.00 &&
		ptBottomLeft.y	== 0.00 &&
		ptTopRight.x	== 0.00 &&
		ptTopRight.y	== 0.00 )
	{
		//has not been set yet!
		return false;
	}
	return true;
}

int MatrixElement::GetCommittedColour(double dRL, ComponentTypeEnum eType, SideOfBayEnum eSide, SideOfBayEnum eSideAlt)
{
	int						iColour;
	CommittedProportionEnum ePortion;
		
	ePortion = GetCommittedProtion( dRL, eType, eSide, eSideAlt );

	iColour=COLOUR_UNDEFINED;
	switch(ePortion)
	{
	case( COMMIT_FULL ):
		iColour=COLOUR_COMMITTED_FULLY;
		break;
	case( COMMIT_PARTIAL ):
		iColour=COLOUR_COMMITTED_PARTIALLY;
		break;
	case( COMMIT_NONE ):
		break;
	case( COMMIT_INVALID ):
		;//assert( false );
		break;
	}
	return iColour;
}



CommittedProportionEnum MatrixElement::GetCommittedProtion(double dRL, ComponentTypeEnum eType, SideOfBayEnum eSide, SideOfBayEnum eSideAlt)
{
	Bay				*pBay;
	int				iCommitted, iBay;
	Component		*pComp;
	SideOfBayEnum	eCompSide;

	iCommitted	= 0;
	for( iBay=0; iBay<GetNumberOfBays(); iBay++ )
	{
		pBay	= GetBay(iBay);
		;//assert( pBay!=NULL );
		eCompSide	= SIDE_INVALID;
		if( pBay->GetMatrixElementPointer()==this )
			eCompSide = eSide;
		if( pBay->GetMatrixElementPointerAlt()==this )
			eCompSide = eSideAlt;

		;//assert( eSide!=SIDE_INVALID );

		pComp = pBay->GetComponentAtRL( eType, dRL, eCompSide );
		if( pComp!=NULL )
		{
			//we have found the component
			if( pComp->IsCommitted() )
				iCommitted++;
		}
		else
		{
			bool bFound;
			bFound = false;
			//The component does not belong to this bay, if the component is lower
			//	the this bays lowest lift, or higher, then it may belong to a neighbor
			switch( eCompSide )
			{
			case( NORTH ):
				if( eType==CT_LEDGER || eType==CT_RAIL ||
					eType==CT_MID_RAIL || eType==CT_TOE_BOARD )
				{
					//This ledger does not belong to the original bay, but it may
					//	belong to one of the forward or backward neighbors
					if( eCompSide==eSide )
					{
						//This matrix element is on the east side of this bay, so lets
						//	check if this ledger belongs to the forward neighbor
						if( pBay->GetForward()!=NULL )
						{
							bFound = GetCommitted( pBay->GetForward(), eType, NORTH, dRL, iCommitted );

							if( !bFound )
							{
								if( pBay->GetForward()->GetOuter()!=NULL )
								{
									bFound = GetCommitted( pBay->GetForward()->GetOuter(), eType, SOUTH, dRL, iCommitted );
								}
							}
						}
					}
					else
					{
						;//assert( eCompSide==eSideAlt );
						//This matrix element is on the west side of this bay, so lets
						//	check if this ledger belongs to the backward neighbor
						if( pBay->GetBackward()!=NULL )
						{
							bFound = GetCommitted( pBay->GetBackward(), eType, NORTH, dRL, iCommitted );

							if( !bFound )
							{
								if( pBay->GetBackward()->GetOuter()!=NULL )
								{
									bFound = GetCommitted( pBay->GetBackward()->GetOuter(), eType, SOUTH, dRL, iCommitted );
								}
							}
						}
					}

					if( !bFound && pBay->GetOuter()!=NULL )
					{
						//there is a small chance it could be on the north
						bFound = GetCommitted( pBay->GetOuter(), eType, SOUTH, dRL, iCommitted );
						if( !bFound )
						{
							if( eCompSide==eSide )
							{
								if( pBay->GetOuter()->GetForward()!=NULL )
								{
									bFound = GetCommitted( pBay->GetOuter()->GetForward(), eType, SOUTH, dRL, iCommitted );
								}
							}
							else
							{
								;//assert( eCompSide==eSideAlt );
								if( pBay->GetOuter()->GetBackward()!=NULL )
								{
									bFound = GetCommitted( pBay->GetOuter()->GetBackward(), eType, SOUTH, dRL, iCommitted );
								}
							}
						}
					}
				}
				else
				{
					if( pBay->GetOuter()!=NULL )
					{
						bFound = GetCommitted( pBay->GetOuter(), eType, SOUTH, dRL, iCommitted );
					}
				}
				;//assert( bFound );
				break;
			case( SOUTH ):

				//There are 8 bays this component could belong to, in order
				//	of likely hood:
				//1) this bay			- already failed!
				//2) Inner				- likely
				//3) forward			- only if eastern side
				//4) backward			- only if western side
				//5) forward->Inner		- only if eastern side
				//6) backward->Inner	- only if western side
				//7) Inner->forward		- only if eastern side
				//8) Inner->backward	- only if western side
				//most likely it will belong to the inner bay
				if( pBay->GetInner()!=NULL )
				{
					bFound = GetCommitted( pBay->GetInner(), eType, NORTH, dRL, iCommitted );
				}

				if( !bFound )
				{
					//!not the inner bay

					//This does not belong to the original bay, but it may
					//	belong to one of the forward or backward neighbors
					if( eCompSide==eSide )
					{
						//This is on the eastern side of this bay, so it could be either: 
						//3) forward			- only if eastern side
						//5) forward->Inner		- only if eastern side
						//7) Inner->forward		- only if eastern side
						if( pBay->GetForward()!=NULL )
						{
							bFound = GetCommitted( pBay->GetForward(), eType, SOUTH, dRL, iCommitted );
							if( !bFound && pBay->GetForward()->GetInner()!=NULL )
								bFound = GetCommitted( pBay->GetForward()->GetInner(), eType, NORTH, dRL, iCommitted );
						}
						else
						{
							if( pBay->GetInner()!=NULL && pBay->GetInner()->GetForward()!=NULL )
								bFound = GetCommitted( pBay->GetInner()->GetForward(), eType, NORTH, dRL, iCommitted );
						}
					}
					else
					{
						//This is on the western side of this bay, so it could be either: 
						//4) backward			- only if western side
						//6) backward->Inner	- only if western side
						//8) Inner->backward	- only if western side
						if( pBay->GetBackward()!=NULL )
						{
							bFound = GetCommitted( pBay->GetBackward(), eType, SOUTH, dRL, iCommitted );
							if( !bFound && pBay->GetBackward()->GetInner()!=NULL )
								bFound = GetCommitted( pBay->GetBackward()->GetInner(), eType, NORTH, dRL, iCommitted );
						}
						else
						{
							if( pBay->GetInner()!=NULL && pBay->GetInner()->GetBackward()!=NULL )
								bFound = GetCommitted( pBay->GetInner()->GetBackward(), eType, NORTH, dRL, iCommitted );
						}
					}
				}
				;//assert( bFound );
				break;
			case( EAST ):
				pBay = pBay->GetForward();
				if( pBay!=NULL )
				{
					pComp = pBay->GetComponentAtRL( eType, dRL, WEST );
					if( pComp!=NULL )
					{
						if( pComp->IsCommitted() )
							iCommitted++;
					}
					else
					{
						;//assert( false );
					}
				}
				else
				{
					;//assert( false );
				}
				break;
			case( WEST ):
				pBay = pBay->GetBackward();
				if( pBay!=NULL )
				{
					pComp = pBay->GetComponentAtRL( eType, dRL, EAST );
					if( pComp!=NULL )
					{
						if( pComp->IsCommitted() )
							iCommitted++;
					}
					else
					{
						;//assert( false );
					}
				}
				else
				{
					;//assert( false );
				}
				break;
			}
		}
	}

	if( iCommitted>0 )
	{
		if( iCommitted==GetNumberOfBays() )
			return COMMIT_FULL;
		return COMMIT_PARTIAL;
	}

	return COMMIT_NONE;
}

bool MatrixElement::GetCommitted(Bay *pBay, ComponentTypeEnum eType, SideOfBayEnum eSide, double dRL, int &iCommitted)
{
	bool		bFound;
	Component	*pComp;

	bFound = false;
	pComp = pBay->GetComponentAtRL( eType, dRL, eSide );
	if( pComp!=NULL )
	{
		bFound = true;
		if( pComp->IsCommitted() )
			iCommitted++;
	}
	return bFound;
}

bool MatrixElement::IsCutThrough()
{
	return GetMatrixPointer( )->IsCutThrough();
}


AcDbGroup * MatrixElement::GetGroup()
{
	return m_pElementGroup;
}

void MatrixElement::SetGroup(AcDbGroup *pGroup)
{
	m_pElementGroup = pGroup;
}

SideOfBayEnum MatrixElement::GetSideOfBay( const Component *pComponent ) const
{
	for( int i=0; i<m_caComponents.GetSize(); i++ )
	{
		if( m_caComponents.GetComponent(i)==pComponent )
		{
			return m_caComponents.GetPosition(i);
		}
	}
	return SIDE_INVALID;
}

void MatrixElement::SetSystem(SystemEnum eSystem)
{
	m_eSystem = eSystem;
	if( eSystem==S_KWIKSTAGE )
		SetStarSeparation(STAR_SEPARATION_KWIKSTAGE);
	else
		SetStarSeparation(STAR_SEPARATION_MILLS);
}

SystemEnum MatrixElement::GetSystem()
{
	return m_eSystem;
}

double MatrixElement::GetStarSeparation() const
{
	return m_dMatrixStarSeparation;
}

void MatrixElement::SetStarSeparation(double dSeparation)
{
	m_dMatrixStarSeparation = dSeparation;
}

double MatrixElement::RLAdjust() const
{
	return GetStarSeparation();
}

double MatrixElement::JackLength() const
{
	return GetStarSeparation();
}


