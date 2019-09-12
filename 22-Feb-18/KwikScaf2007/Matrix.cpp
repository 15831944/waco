// Matrix.cpp: implementation of the Matrix class.
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
#include "Matrix.h"
#include "Controller.h"
#include "dbents.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//#define IGNOR_JACK_OFFSET
//#define ALLOW_CONTINUOUS_Z_LINE
//#define SHOW_MATRIX_XHAIR

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Matrix::Matrix()
{
	Matrix3D	Transform;
	Vector3D	Vector;
	SetMoving( false );

	m_pController = NULL;

	m_pCrosshair = NULL;
	m_pCrosshairReactor = NULL;
	SetCrosshairCreated( false );

	m_LevelTransform.setToIdentity();

	//Cutthrough initialisation
	SetCutThrough(false);

	m_sCutThroughLabel.Empty();
	m_bCutThroughLabelSet = false;
	SetAllowCancel(true);
}

Matrix::~Matrix()
{
	if( !IsCutThrough() )
		DeleteAllElements();
}

int Matrix::Add(MatrixElement *pMatrixElement)
{
	int	iNewID;

	iNewID = CArray<MatrixElement*, MatrixElement*>::Add( pMatrixElement );
	pMatrixElement->SetMatrixPointer( this );
	pMatrixElement->SetMatrixElementID(iNewID);
	return iNewID;
}

Matrix3D Matrix::GetMatrixCrosshairTransform()
{
	if( IsCutThrough() )
		return GetController()->GetCutThroughCrosshairTransform();
	else
		return GetController()->GetMatrixCrosshairTransform();
}

void Matrix::SetMatrixCrosshairTransform(Matrix3D Transform)
{
	if( IsCutThrough() )
		GetController()->SetCutThroughCrosshairTransform( Transform );
	else
		GetController()->SetMatrixCrosshairTransform( Transform );
}

void Matrix::Move(Matrix3D Transform, bool bStore)
{
	int					i;
	MatrixElement		*pElement;

	SetMoving( true );

	GetController()->MoveMatrixLastPoint( Transform );

	//store it
	if( bStore )
	{
		SetMatrixCrosshairTransform ( Transform * GetMatrixCrosshairTransform() );
	}

	for( i=0; i<GetMatrixSize(); i++ )
	{
		pElement = GetAt(i);
		pElement->Move( Transform, false );
	}

	MoveEntities( &m_RLIds, Transform );
	MoveEntities( &m_RLIdsText, Transform );
	MoveEntities( &m_Levels, Transform );
	MoveEntities( &m_LevelsLables, Transform );

	SetMoving( false );
}

Matrix3D Matrix::UnMove()
{
	Matrix3D	Original, Inverse;

	//store the original transformation for later use
	Original = GetMatrixCrosshairTransform();

	//we need to move the Bay back to its original position
	Inverse = GetMatrixCrosshairTransform();
	Inverse.invert();

	Move( Inverse, true );

	//return the original matrix
	return Original;
}

MatrixElement *Matrix::GetMatrixElement(int iID) const
{
	if( (iID<0) || (iID>=GetMatrixSize()) )
		return NULL;

	return GetAt(iID);
}

int Matrix::GetMatrixElementID(MatrixElement *pMatrixElement) const
{
	MatrixElement	*pElement;
	int				i, iSize;

	iSize = GetMatrixSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		if( pElement==pMatrixElement )
		{
			return pElement->GetMatrixElementID();
		}
	}
	return ID_NONE_MATCHING;
}

int Matrix::GetMatrixSize() const
{
	return GetSize();
}

void Matrix::ArrangeElements()
{

}

bool Matrix::DeleteMatrixElement(int iID)
{
	MatrixElement	*pElement;

	if( (iID<0) || (iID>=GetMatrixSize()) )
		return false;
	
	pElement = NULL;
	pElement = GetMatrixElement(iID);
	if( pElement!=NULL )
	{
		delete pElement;
	}
	else
	{
		;//assert( false );
	}

	RemoveAt(iID);

	//////////////////////////////////////////////
	//Renumber subsequent matrix elements
	for( iID=0; iID<GetMatrixSize(); iID++ )
	{
		pElement = GetAt(iID);
		pElement->SetMatrixElementID(iID);
	}
	return true;
}

bool Matrix::DeleteMatrixElement(MatrixElement *pElement)
{
	int iID;
	iID = GetMatrixElementID(pElement);

	if( iID==ID_NONE_MATCHING )
		return false;

	return DeleteMatrixElement( iID );
}

bool Matrix::DeleteAllElements()
{
	Delete3DView();

	return true;
}

void Matrix::WriteBOMTo(CStdioFile *pFile)
{
	MatrixElement	*pElement;
	int				i, iSize;

	iSize = GetMatrixSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		pElement->WriteBOMTo(pFile);
	}
}

Controller * Matrix::GetController()
{
	return m_pController;
}

void Matrix::SetController(Controller *pController)
{
	m_pController = pController;
}

bool Matrix::RedrawMatrix()
{
	int					i, iSize;
	CString				sProg;
	AcDbGroup			*pGroup;
	MatrixElement		*pMatrixEle;
	AcDbObjectIdArray	objIds, GroupIDs;

	iSize = GetMatrixSize();
	if( !IsCrosshairCreated() )
	{
		for( i=0; i<iSize; i++ )
		{
			if( IsCancelAllowed() && (acedUsrBrk()==1) )
			{
				CString sMsg;
				sMsg = _T("Do you wish to stop the creation of the matrix?");

				if( GetController()!=NULL && GetController()->EditingStandards() )
				{
          sMsg+= _T("\n\nWARNING: You will not be able to edit the\nstandards without the matrix.");
				}

				if( MessageBox( NULL, sMsg, _T("Cancel Request"), MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2 )==IDYES )
				{
					acedRestoreStatusBar();
					return false;
				}
			}

			sProg.Format( _T("Constructing Matrix View %i/%i"), i+1, iSize );
			acedSetStatusBarProgressMeter( sProg, 0, iSize );
			acedSetStatusBarProgressMeterPos( i );

			pMatrixEle = GetMatrixElement( i );
			pMatrixEle->Create3DView( );
			pGroup = pMatrixEle->GetGroup();
			if( pGroup!=NULL )
			{
				pGroup->allEntityIds(GroupIDs);
				while( GroupIDs.length()>0 )
				{
					objIds.append(GroupIDs[0]);
					GroupIDs.removeAt(0);
				}
			}
		}

		if( IsCutThrough() )
		{
			////////////////////////////////////////////////////////////////////////
			//Place the Label aligned on the left STAR_SEPARATION_APPROX below the lowest RL
			double	dRLTop, dRLBottom;
			Entity	*pLabel;
			CString sLabel;
			Point3D	pt;

			GetRLExtents( dRLTop, dRLBottom );
			pt.set( 0.00, dRLBottom-STAR_SEPARATION_APPROX, 0.00 );
			GetCutThroughLabel( sLabel );
			pLabel = DrawText( pt, sLabel, COLOUR_WHITE, 1.5 );
			objIds.append(pLabel->objectId());

			//The matrix needs level lines and scales
			DrawRLs();
			for( i=0; i<m_Levels.length(); i++ )
			{
				objIds.append( m_Levels[i] );
			}

			////////////////////////////////////////////////////////////////////////
			//Group the entire Cutthrough
			int				*piGroupNumber;
			TCHAR			cGroupName[255];
			Entity			entTemp;
			CString			sGroupName, sCutthroughTextTop, sCutthroughTextBottom;
			AcDbObjectId	id;

 			piGroupNumber = GetController()->GetGroupNumber();
			(*piGroupNumber)++;
			sGroupName.Format( _T("%s%03i"), GROUP_PREFIX_CUT_THROUGH, *piGroupNumber );
			for( i=0; i<sGroupName.GetLength(); i++ )
			{
				if( i>30 ) break;
				cGroupName[i] = sGroupName[i];
			}
			cGroupName[i] = _T('\0');
			entTemp.CreateGroup( objIds, cGroupName );
		}
		else
		{
			//The matrix needs level lines and scales
			DrawRLs();
		}
		Create3DCrosshair();
	}
	acedSetStatusBarProgressMeterPos(iSize);
	acedRestoreStatusBar();

	return true;
}

void Matrix::Delete3DView()
{
	int					i, iSize;
	MatrixElement		*pMatrixEle;

	if( IsCrosshairCreated() )
	{
		iSize = GetMatrixSize();
		for( i=0; i<iSize; i++ )
		{
			pMatrixEle = GetMatrixElement( i );

			while( pMatrixEle->GetNumberOfBays()>0 )
			{
				pMatrixEle->RemoveBayLink( pMatrixEle->GetBay(0) );
			}

			pMatrixEle->Delete3DView(  );
			
		}

		///////////////////////////////////////////////////////////////////
		//Delete any existing RL scales!
		DeleteAllEntities();

		///////////////////////////////////////////////////////////////////
		//Delete the crosshair
		Destroy3DCrosshair();
	}
}

void Matrix::MoveEntities(AcDbObjectIdArray *paEntities, Matrix3D Transform )
{
	int					i;
	Entity				*pEnt;
	Acad::ErrorStatus	es;

	GetController()->IgnorMovement(true);
	for( i=0; i<paEntities->length(); i++ )
	{
		es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, paEntities->at(i), AcDb::kForWrite );
		if( es==Acad::eOk )
		{
			es = pEnt->transformBy( Transform );
			;//assert( es==Acad::eOk );
		}
		es = pEnt->close();
		;//assert( es==Acad::eOk );
	}
	GetController()->IgnorMovement(false);
}

void Matrix::Create3DCrosshair()
{
	//Just like highlander, "There can be only one"!
	if( !IsCrosshairCreated() )
	{
		#ifdef	SHOW_MATRIX_XHAIR
			int					iCrosshairColour;
			Point3D				pt;
			Adesk::Int16		mode;
			Acad::ErrorStatus	es;

			/////////////////////////////////////////////////////////////////////
			//Create the crosshair point at the origin
			pt.set( 0.00, 0.00, 0.00 );
			m_pCrosshair = (Entity*)((AcDbEntity*) new AcDbPoint(pt));

			/////////////////////////////////////////////////////////////////////
			//Set the point display mode
			mode = 66;
			es = acdbHostApplicationServices()->workingDatabase()->setPdmode( mode );

			/////////////////////////////////////////////////////////////////////
			//Colour
			iCrosshairColour = COLOUR_CROSSHAIR_MATRIX;
			m_pCrosshair->setColorIndex( (Adesk::UInt16)iCrosshairColour );
			m_pCrosshair->postToDatabase( LAYER_NAME_MATRIX );
			m_pCrosshair->AttachedReactors( &m_pCrosshairReactor );
			m_pCrosshairReactor->SetMatrixPointer( this );
			
			/////////////////////////////////////////////////////////////////////
			//shift it to the origin
			MoveCrosshairOnly( GetMatrixCrosshairTransform() );
		#endif	//#ifdef	SHOW_MATRIX_XHAIR
		//finished
		SetCrosshairCreated( true );
	}
}

void Matrix::Destroy3DCrosshair()
{
	#ifdef	SHOW_MATRIX_XHAIR
		Acad::ErrorStatus	es;

		/////////////////////////////////////////////////////////////
		//Delete the entity
		es = m_pCrosshair->_topen();
		if( es==Acad::eOk )
		{
			m_pCrosshair->removePersistentReactor( m_pCrosshairReactor->objectId() );
			m_pCrosshair->erase();
		}
		else if( es==Acad::eWasErased )
		{
			//fine
			1;
		}
		else if( es==Acad::eWasOpenForNotify )
		{
			//They obviously deleted the crosshair directly
			1;
		}
		else
		{
			;//assert( false );
		}
		es = m_pCrosshair->close();
		;//assert( es==Acad::eOk );

		m_pCrosshair = NULL;

		/////////////////////////////////////////////////////////////
		//Delete the reactor
		es = m_pCrosshairReactor->_topen();
		if( es==Acad::eOk )
		{
			m_pCrosshairReactor->erase();
		}
		else if( es==Acad::eWasErased )
		{
			//fine
			1;
		}
		else if( es==Acad::eWasOpenForNotify )
		{
			//fine
			1;
		}
		else
		{
			;//assert( false );
		}
		es = m_pCrosshairReactor->close();
		;//assert( es==Acad::eOk );
		m_pCrosshairReactor = NULL;
	#endif	//#ifdef	SHOW_MATRIX_XHAIR

	SetCrosshairCreated( false );
}

void Matrix::SetCrosshairCreated(bool bCreated)
{
	m_bCrosshairCreated = bCreated;
}

bool Matrix::IsCrosshairCreated()
{
	return m_bCrosshairCreated;
}


void Matrix::DrawRLs()
{
	int			i, iSize, iColour, iExclusions, iTemp;
	double		dRL, dXOffset, dRLMax, dRLMin, dXOffMin, dXOffMax, dMin, dMax;
	Entity		*pEntity;
	intArray	*iaExcludes;
	Point3D		pt;

	//Not all the matrix elements need to be displayed
	iSize = GetMatrixSize();
	if( !GetController()->EditingStandards() )
	{
		iaExcludes = GetController()->GetMatrixElementToDisplay();
		iExclusions = 0;
		for( i=0; i<iaExcludes->GetSize(); i++ )
		{
			iTemp = iaExcludes->GetAt(i);
			if( iTemp>=0 && iTemp<iSize )
			{
				iExclusions++;
			}
		}
		iSize-= iExclusions;
	}

	///////////////////////////////////////////////////////////////////
	//Delete any existing RL scales!
	DeleteAllEntities();

	///////////////////////////////////////////////////////////////////
	//find the vertical limits of the matrix
	GetRLExtents( dRLMax, dRLMin);

	///////////////////////////////////////////////////////////////////
	//Find the X axis extents of the matrix
	//dXOffMin = 0.00;
	dXOffMin = 0.40*MATRIX_SEPARATION_ALONG_X;
	dXOffMax = ((double)iSize+0.75)*MATRIX_SEPARATION_ALONG_X;

	/////////////////////////////////////////////////////////////////////
	//we need to create a long, dotted line which runs from one scale to
	//	the other
	dMin = dRLMin;
	dMax = dRLMax;

	#ifndef IGNOR_JACK_OFFSET
	dMin+= JACK_LENGTH_APPROX;
	dMax+= JACK_LENGTH_APPROX;
	#endif	//#ifndef IGNOR_JACK_OFFSET

	if( GetController()->ShowScaleLines() && !IsCutThrough() )
	{
		for( dRL=dMin; dRL<=dMax; dRL+=STAR_SEPARATION_APPROX )
		{
			if( ((int)dRL%2000)==0 )
			{
				#ifndef IGNOR_JACK_OFFSET
				pt.set( dXOffMin, ConvertRLtoStarRL(dRL-JACK_LENGTH_APPROX, GetController()->GetStarSeperation() ), 0.00 );
				#else	//#ifndef IGNOR_JACK_OFFSET
				pt.set( dXOffMin, ConvertRLtoStarRL(dRL, GetController()->GetStarSeperation() ), 0.00 );
				#endif	//#ifndef IGNOR_JACK_OFFSET

				iColour = COLOUR_MATRIX_RL_LINES_SPAN;
				pEntity = DrawHorizontalLine( pt, dXOffMax - dXOffMin, iColour, PS_DOTS_DISTANT );
				if( pEntity!=NULL )
					m_RLIds.append( pEntity->objectId() );
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	//Draw the first RL Scale
	if( GetController()->ShowFirstScale() && !IsCutThrough() )
	{
		DrawRLScale( dXOffMin, dRLMin, dRLMax );
	}

	///////////////////////////////////////////////////////////////////
	//for each NUMBER_OF_MATRIX_ELEMENTS_PER_SCALE matrix elements, draw an RL scale 
	for( i=NUMBER_OF_MATRIX_ELEMENTS_PER_SCALE; i<iSize; i+=NUMBER_OF_MATRIX_ELEMENTS_PER_SCALE )
	{
		dXOffset = ((double)i+0.75)*MATRIX_SEPARATION_ALONG_X;
		if( !IsCutThrough() )
		{
			DrawRLScale( dXOffset, dRLMin, dRLMax );
		}
	}

	if( !IsCutThrough() )
	{
		///////////////////////////////////////////////////////////////////
		//Draw another scale at the end of the Matrix
		DrawRLScale( dXOffMax, dRLMin, dRLMax );
	}

	///////////////////////////////////////////////////////////////////
	//Draw the Levels
	DrawLevels( dXOffMin, dXOffMin+(0.10*MATRIX_SEPARATION_ALONG_X), dXOffMax-dXOffMin );
}

void Matrix::GetRLExtents(double &dRLMax, double &dRLMin)
{
	int					i, iSize;
	bool				bSoleBoard, bFound;
	double				dRLTop, dRLBottom;
	doubleArray			daArrangement;
	MatrixElement		*pElement;
	MatrixStandardEnum	eStandards;


	iSize		= GetMatrixSize();
	bFound = false;
	dRLMax = LARGE_NEGATIVE_NUMBER;
	dRLMin = LARGE_NUMBER;
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		eStandards	= MATRIX_STANDARDS_INNER;
		pElement->GetStandardsArrangement( daArrangement, bSoleBoard, eStandards );
		if( daArrangement.GetSize()>0 )
		{
			bFound = true;
			dRLBottom	= daArrangement[0];
			dRLTop		= daArrangement[daArrangement.GetSize()-1];
			dRLMax = max( dRLTop,		dRLMax );
			dRLMin = min( dRLBottom,	dRLMin );
		}
		eStandards	= MATRIX_STANDARDS_OUTER;
		pElement->GetStandardsArrangement( daArrangement, bSoleBoard, eStandards );
		if( daArrangement.GetSize()>0 )
		{
			bFound = true;
			dRLBottom	= daArrangement[0];
			dRLTop		= daArrangement[daArrangement.GetSize()-1];
			dRLMax = max( dRLTop,		dRLMax );
			dRLMin = min( dRLBottom,	dRLMin );
		}
	}

	if( !bFound )
	{
		dRLMax = 0.00;
		dRLMin = 0.00;
	}

	//Better add a couple to the bottom for the Jack and Soleboard
	dRLMin-= 2.00*STAR_SEPARATION_APPROX;

	;//assert( dRLMax>dRLMin );
}


Entity * Matrix::DrawHorizontalLine(Point3D pt, double dLength, int iColour, PlotSyleEnum eHL/*=PS_LINE*/, int iLevelLine/*=-1*/ )
{
	TCHAR				PlotStyleName[30];
	Entity				*pLine;
	Point3D				ptEnd;
	AcDbObjectId		idCompare;
	EntityReactor		*pReactor;
	Acad::ErrorStatus	es;

	ptEnd	= pt;
	ptEnd.x+= dLength;
	pLine = (Entity*) new AcDbLine( pt, ptEnd );

	/////////////////////////////////////////////////////////////////////
	//Colour
	es = pLine->setColorIndex( (Adesk::UInt16)iColour );
	;//assert( es==Acad::eOk );

	/////////////////////////////////////////////////////////////////////
	//Move according to crosshair
	es = pLine->transformBy( GetMatrixCrosshairTransform() );
	;//assert( es==Acad::eOk );

	///////////////////////////////////////////////////////////////////
	//Style
	GetPlotStyleName( eHL, PlotStyleName );
	es = pLine->setLinetype(PlotStyleName);
	if( es!=Acad::eOk )
	{
		acutPrintf(_T("\nCan't find %s Style!\n"), PlotStyleName );
	}

	/////////////////////////////////////////////////////////////////////
	//add to database
	pLine->postToDatabase( LAYER_NAME_MATRIX );
	pLine->AttachedReactors( &pReactor );
	pReactor->SetMatrixPointer( this );
	pReactor->SetLevelLine( iLevelLine );

	es=pLine->close();
	;//assert( es==Acad::eOk );

	return pLine;
}

Entity * Matrix::DrawText(Point3D pt, CString &sText, int iColour, double dTextHeightMultiplier )
{
	double				dTextHeight;
	Entity				*pText;
	Point3D				ptOrigin;
	Vector3D			Vector;
	Matrix3D			Transform;
	EntityReactor		*pReactor;
	AcDbObjectId		recordId;
	AcDbTextStyleTable	*pTable;
	Acad::ErrorStatus	es;

	acdbHostApplicationServices()->workingDatabase()->
					getSymbolTable( pTable, AcDb::kForRead);
	es = pTable->getAt( TEXT_STYLE_MATRIX, recordId );
	if( es==Acad::eKeyNotFound )
	{
		es = pTable->getAt( TEXT_STYLE_STANDARD, recordId );
	}
	pText = NULL;
	if( es==Acad::eOk )
	{
		/////////////////////////////////////////////////////////////////////
		//Create Text
		dTextHeight = GetController()->GetCompDetails()->GetActualHeight( GetController()->GetSystem(), CT_TEXT, 0, MT_STEEL);
		dTextHeight*= dTextHeightMultiplier;
		ptOrigin.set( 0.00, 0.00, 0.00 );
		pText = (Entity*) new AcDbText( ptOrigin, sText, recordId, dTextHeight );
		es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextLeft);
		es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);

		/////////////////////////////////////////////////////////////////////
		//Movement
		Vector.set( pt.x, pt.y, pt.z );
		Transform.setToTranslation( Vector );
		//Move according to crosshair
		Transform = GetMatrixCrosshairTransform() * Transform;
		pText->transformBy( Transform );

		/////////////////////////////////////////////////////////////////////
		//add to database
		if( IsCutThrough() )
		{
			pText->postToDatabase( LAYER_NAME_SECTIONS );
		}
		else
		{
			pText->postToDatabase( LAYER_NAME_MATRIX );
			pText->AttachedReactors( &pReactor );
			pReactor->SetMatrixPointer( this );
		}

/*		AcCmColor Color;
		Color.setColor( iColour );
*/		pText->open();

		pText->setColorIndex( iColour );

		pText->close();
	}
	es = pTable->close();
	;//assert(es==Acad::eOk);

	return pText;
}

void Matrix::DeleteEntities( AcDbObjectIdArray *pIdArray )
{
	int					i;
	void	            *pSomething;
	Entity				*pEnt;
	AcDbObjectId		ReactorID;
	EntityReactor		*pReactor;
	AcDbVoidPtrArray	*pReactors;
	Acad::ErrorStatus	es;

	for( i=0; i<pIdArray->length(); i++ )
	{
		es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, pIdArray->at(i), AcDb::kForWrite );
		if( es==Acad::eOk )
		{
			pReactors	= (AcDbVoidPtrArray*)pEnt->reactors();

			if ( (pReactors!=NULL) && (pReactors->logicalLength()>0) )
			{
				while( pReactors->logicalLength()>0 )
				{
					pSomething = pReactors->at(0);
					// Is it a persistent reactor?
					if (acdbIsPersistentReactor(pSomething))
					{
						ReactorID = acdbPersistentReactorObjectId( pSomething);
						if( ReactorID.isNull() )
							continue;

						pEnt->removePersistentReactor( ReactorID );

						if( ReactorID.isValid() )
						{
							es = acdbOpenAcDbObject((AcDbObject*&)pReactor, ReactorID, AcDb::kForWrite);

							if(es==Acad::eOk)
							{
								es = pReactor->erase();
								;//assert( es==Acad::eOk);
								es = pReactor->close(); 
								;//assert( es==Acad::eOk);
								pReactor=NULL;
							}
							else if( es==Acad::eWasErased )
							{
								//fine
								1;
							}
							else
							{
								;//assert( false );
							}
						}
					}
				}
			}
			es = pEnt->erase();
			;//assert( es==Acad::eOk);
			es = pEnt->close();
			;//assert( es==Acad::eOk);
			pReactors = NULL;
			pEnt = NULL;
		}
		else if( es==Acad::eWasErased )
		{
			//fine
			1;
		}
		else
		{
			;//assert( false );
		}
	}
	pIdArray->setPhysicalLength(0);
}

bool Matrix::IsMoving()
{
	return m_bMoving;
}

void Matrix::SetMoving(bool bMoving)
{
	m_bMoving = bMoving;
}

/*
void Matrix::MovingLevel()
{
	SetMoving( true );
	GetController()->RestrictMouse();
}

void Matrix::StopMovingLevel()
{

}
*/

void Matrix::DrawRLScale(double dXPos, double dRLMin, double dRLMax)
{
	int					iColour;
	double				dRL;
	Entity				*pEntity;
	CString				sRL;
	Point3D				pt, ptEnd;
	Vector3D			Vector;
	Matrix3D			Transform;
	Acad::ErrorStatus	es;

#ifndef IGNOR_JACK_OFFSET
	dRLMin+= JACK_LENGTH_APPROX;
	dRLMax+= JACK_LENGTH_APPROX;
#endif	//#ifndef IGNOR_JACK_OFFSET

	//round the Min down to the nearest STAR_SEPARATION_APPROX
	dRLMin = double(int(dRLMin/STAR_SEPARATION_APPROX))*STAR_SEPARATION_APPROX;
	//Round the Max up to the nearest STAR_SEPARATION_APPROX (will actually go one past, if exact)
	dRLMax = double(int(dRLMax/STAR_SEPARATION_APPROX)+1)*STAR_SEPARATION_APPROX;

	for( dRL=dRLMin; dRL<=dRLMax; dRL+=STAR_SEPARATION_APPROX )
	{
#ifndef IGNOR_JACK_OFFSET
		pt.set( dXPos, dRL-JACK_LENGTH_APPROX, 0.00 );
#else
		pt.set( dXPos, dRL, 0.00 );
#endif	//#ifndef IGNOR_JACK_OFFSET
		if( ((int)dRL%2000)==0 )
		{
			//we need to create a line at -2.0m, 0.0m, 2.0m, 4.0m, etc
			sRL.Format( _T("%1.1fm"), dRL*CONVERT_MM_TO_M );
			iColour = COLOUR_MATRIX_RL_TEXT;
			pEntity = DrawText( pt, sRL, iColour, 1.25 );
			if( pEntity!=NULL )
				m_RLIdsText.append( pEntity->objectId() );

			pEntity->open();
			es = ((AcDbText*)pEntity)->setHorizontalMode(AcDb::kTextRight);
			pEntity->close();
		}
		else
		{
			/////////////////////////////////////////////////////////////////////
			//we need to create a smaller line and text at -0.5m, 0.5m, 1.0m,
			//	1.5m, 2.5m, 3.0m
			iColour = COLOUR_MATRIX_RL_LINES_MINOR;
			pEntity = DrawHorizontalLine( pt, MATRIX_LINE_LENGTH_MINOR, iColour );
			if( pEntity!=NULL )
				m_RLIds.append( pEntity->objectId() );
		}
	}
}

void Matrix::DrawLevels(double dXPosStart, double dTextXValue, double dLineLength )
{
	int				i, j, l, iColour;
	bool			bSoleboard, bFound;
	double			dRL, dRLMax, dRLMin, dRLTemp1, dRLTemp2, dRLStd,
					dDistRLtoRail, dHandrail, dRLOriginal;
	Entity			*pEntity;
	CString			sLevel;
	Point3D			pt;
	LevelList		*pLevels;
	doubleArray		daTextPos, daArrangement, daHandrails;
	Point3DArray	ptsZigZag;
	MatrixElement	*pEle;

	///////////////////////////////////////////////////////////////////
	//Get the Levels from the controller
	pLevels = GetController()->GetLevelList();

	///////////////////////////////////////////////////////////////////
	//Create a list of the positions for the text
	GetRLExtents( dRLMax, dRLMin);
	dRLMin-= (4*STAR_SEPARATION_APPROX);
	dRLMax+= (4*STAR_SEPARATION_APPROX);
	daTextPos.Add( dRLMin );
	daTextPos.Add( dRLMax );

	///////////////////////////////////////////////////////////////////
	//Draw the lines
	iColour = COLOUR_MATRIX_LEVELS_LINES;
	for( i=0; i<pLevels->GetSize(); i++ )
	{
		///////////////////////////////////////////////////////////////////
		dRL =  pLevels->GetLevel(i);
		dRLOriginal = dRL;

		#ifndef IGNOR_JACK_OFFSET
		pt.set( dXPosStart, dRL-JACK_LENGTH_APPROX, 0.00 );
		#else	//#ifndef IGNOR_JACK_OFFSET
		pt.set( dXPosStart, dRL, 0.00 );
		#endif	//#ifndef IGNOR_JACK_OFFSET

		if( !IsCutThrough() )
		{
			if( GetController()->ShowLongLevelLine() )
			{
				pEntity = DrawHorizontalLine( pt, dLineLength, iColour, PS_DASHED, i );
			}
			else
			{
#ifdef ALLOW_CONTINUOUS_Z_LINE
				pEntity = DrawHorizontalLine( pt, MATRIX_SEPARATION_ALONG_X/4.00, iColour, PS_DASHED, i );
#else	//#ifdef ALLOW_CONTINUOUS_Z_LINE
				pEntity = DrawHorizontalLine( pt, MATRIX_SEPARATION_ALONG_X/2.50, iColour, PS_DASHED, i );
#endif	//#ifdef ALLOW_CONTINUOUS_Z_LINE
			}

			if( pEntity!=NULL )
				m_Levels.append( pEntity->objectId() );

		}


		///////////////////////////////////////////////////////////////////
		//whilst we are going through the RLs, lets check the positions of the text
		dRLTemp1 = dRL-(4*STAR_SEPARATION_APPROX);
		dRLTemp2 = dRL+(4*STAR_SEPARATION_APPROX);
		if( dRLTemp1<daTextPos[0] )
		{
			//Move the first point 2m from the start
			daTextPos.RemoveAt( 0 );
			daTextPos.InsertAt( 0, dRLTemp1 );
		}
		else if( dRLTemp2>daTextPos[daTextPos.GetSize()-1] )
		{
			//Move the last point 2m from the end
			daTextPos.RemoveAt(daTextPos.GetSize()-1);
			daTextPos.Add( dRLTemp2 );
		}
		//find where in the list this belongs
		j=0;
		while( j<daTextPos.GetSize() && dRL>daTextPos[j] )
		{
			j++;
		}
		//we should not have any collisions!
		if( dRL==daTextPos[j] )
		{
			;//assert( false );
		}
		daTextPos.InsertAt( j, dRL );

		ptsZigZag.RemoveAll();
		pt.transformBy( GetMatrixCrosshairTransform() );
		pt.x+=MATRIX_SEPARATION_ALONG_X/4.00;
		ptsZigZag.append( pt );
		for( j=0; j<GetSize(); j++ )
		{
			if( !GetController()->ShowMatrixElement(j) )
				continue;
			pEle = GetAt(j);
			;//assert( pEle!=NULL );

			//create points for the outer arrangement
			pEle->GetStandardsArrangement( daArrangement, bSoleboard, MATRIX_STANDARDS_OUTER );
			;//assert( pEle->m_ptsStandardPointsOuter.length()==daArrangement.GetSize() );
			for( int k=0; k<daArrangement.GetSize(); k++ )
			{
				dRL = dRLOriginal;

				dRLStd = daArrangement[k];
				dRLStd+= pEle->GetStarSeparation()-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
				//See if there is a deck 1000mm below this level that requires a handrail
				//	if there is then we need to include the standard
				pEle->GetHandrailHeights( daHandrails, MATRIX_STANDARDS_OUTER );
				//is there a handrails <STAR_SEPARATION above the rl?
				bFound=false;
				for( l=0; l<daHandrails.GetSize(); l++ )
				{
					dHandrail = daHandrails[l]+pEle->RLAdjust();
					dDistRLtoRail = dHandrail-dRL;
					if( dDistRLtoRail>0-ROUND_ERROR &&
						dDistRLtoRail<(2.00*pEle->GetStarSeparation())-ROUND_ERROR )
					{
						//there is a handrail <STAR_SEPARATION above this rl, we have a virtual
						//	RL at this lift height!
						bFound = true;
						dRL = dHandrail;
						break;
					}
				}
				if( !bFound )
				{
					pEle->GetEndHandrailHeights( daHandrails );
					for( l=0; l<daHandrails.GetSize(); l++ )
					{
						dHandrail = daHandrails[l]+pEle->RLAdjust();
						dDistRLtoRail = dHandrail-dRL;
						if( dDistRLtoRail>0-ROUND_ERROR &&
							dDistRLtoRail<(2.00*pEle->GetStarSeparation())-ROUND_ERROR )
						{
							//there is a handrail <STAR_SEPARATION above this rl, we have a virtual
							//	RL at this lift height!
							bFound = true;
							dRL = dHandrail;
							break;
						}
					}
				}
				
				if( dRLStd>=dRL-ROUND_ERROR || (dRLStd<dRL+ROUND_ERROR && k==daArrangement.GetSize()-1) )
				{
					pt = pEle->m_ptsStandardPointsOuter[k];
					pt.x-= pEle->GetStarSeparation();
//					pt.transformBy( pEle->GetTransform() );
					ptsZigZag.append( pt );
					pt.x+= pEle->GetStarSeparation()*2.00;
//					pt.transformBy( pEle->GetTransform() );
					ptsZigZag.append( pt );
					break;
				}
			}

			dRL = dRLOriginal;
			//create points for the inner arrangement
			pEle->GetStandardsArrangement( daArrangement, bSoleboard, MATRIX_STANDARDS_INNER );
			;//assert( pEle->m_ptsStandardPointsInner.length()==daArrangement.GetSize() );
			for(int k=0; k<daArrangement.GetSize(); k++ )
			{
				dRLStd	= daArrangement[k];
				dRLStd+= pEle->GetStarSeparation()-EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;

				//See if there is a deck 1000mm below this level that requires a handrail
				//	if there is then we need to include the standard
				pEle->GetHandrailHeights( daHandrails, MATRIX_STANDARDS_INNER );
				//is there a handrails <STAR_SEPARATION above the rl?
				bFound=false;
				for( l=0; l<daHandrails.GetSize(); l++ )
				{
					dHandrail = daHandrails[l]+pEle->RLAdjust();
					dDistRLtoRail = dHandrail-dRL;
					if( dDistRLtoRail>0-ROUND_ERROR &&
						dDistRLtoRail<(2.00*pEle->GetStarSeparation())-ROUND_ERROR )
					{
						//there is a handrail <STAR_SEPARATION above this rl, we have a virtual
						//	RL at this lift height!
						bFound = true;
						dRL = dHandrail;
						break;
					}
				}
				if( !bFound )
				{
					pEle->GetEndHandrailHeights( daHandrails );
					for( l=0; l<daHandrails.GetSize(); l++ )
					{
						dHandrail = daHandrails[l]+pEle->RLAdjust();
						dDistRLtoRail = dHandrail-dRL;
						if( dDistRLtoRail>0-ROUND_ERROR &&
							dDistRLtoRail<(2.00*pEle->GetStarSeparation())-ROUND_ERROR )
						{
							//there is a handrail <STAR_SEPARATION above this rl, we have a virtual
							//	RL at this lift height!
							bFound = true;
							dRL = dHandrail;
							break;
						}
					}
				}
				
				if( dRLStd>=dRL-ROUND_ERROR || (dRLStd<dRL+ROUND_ERROR && k==daArrangement.GetSize()-1) )
				{
					pt = pEle->m_ptsStandardPointsInner[k];
					pt.x-= pEle->GetStarSeparation();
//					pt.transformBy( pEle->GetTransform() );
					ptsZigZag.append( pt );
					pt.x+= pEle->GetStarSeparation()*2.00;
//					pt.transformBy( pEle->GetTransform() );
					ptsZigZag.append( pt );
					break;
				}
			}
		}

		//there should be an even number of points
		if( GetController()->GetDisplayZigZagLine() )
		{
#ifdef ALLOW_CONTINUOUS_Z_LINE
			;//assert( ((ptsZigZag.length()-1)%2)==0 );
			for( j=0; j<ptsZigZag.length()-1; j++ )
			{
				pEntity = DrawLine( ptsZigZag[j], ptsZigZag[j+1], iColour, PS_DASHED );  
				if( pEntity!=NULL )
					m_Levels.append( pEntity->objectId() );
			}
#else	//#ifdef ALLOW_CONTINUOUS_Z_LINE
			if( IsCutThrough() )
			{
				;//assert( ((ptsZigZag.length()-1)%2)==0 );
				for( j=1; j<ptsZigZag.length()-1; j++ )
				{
					pEntity = DrawLine( ptsZigZag[j], ptsZigZag[j+1], iColour, PS_DASHED );  
					if( pEntity!=NULL )
						m_Levels.append( pEntity->objectId() );
				}
			}
			else
			{
				;//assert( ((ptsZigZag.length()-1)%4)==0 );
				for( j=1; j<ptsZigZag.length()-3; j+=4 )
				{
					pEntity = DrawLine( ptsZigZag[j], ptsZigZag[j+1], iColour, PS_DASHED );  
					if( pEntity!=NULL )
						m_Levels.append( pEntity->objectId() );
					pEntity = DrawLine( ptsZigZag[j+1], ptsZigZag[j+2], iColour, PS_DASHED );  
					if( pEntity!=NULL )
						m_Levels.append( pEntity->objectId() );
					pEntity = DrawLine( ptsZigZag[j+2], ptsZigZag[j+3], iColour, PS_DASHED );  
					if( pEntity!=NULL )
						m_Levels.append( pEntity->objectId() );
				}
			}
#endif	//#ifdef ALLOW_CONTINUOUS_Z_LINE
		}
		iColour++;
	}

	if( !IsCutThrough() )
	{
		///////////////////////////////////////////////////////////////////
		//Draw the Level text
		iColour = COLOUR_MATRIX_LEVELS_TEXT;
		for( i=0; i<daTextPos.GetSize()-1; i++ )
		{
			dRL =  ((daTextPos[i+1]-daTextPos[i])/2.00)+daTextPos[i];
	#ifndef IGNOR_JACK_OFFSET
			pt.set( dTextXValue, dRL-JACK_LENGTH_APPROX, 0.00 );
	#else	//#ifndef IGNOR_JACK_OFFSET
			pt.set( dTextXValue, dRL, 0.00 );
	#endif	//#ifndef IGNOR_JACK_OFFSET
			sLevel.Format( _T("LEVEL %i"), i+1 );
			pEntity = DrawText( pt, sLevel, iColour, 1.25 );
			if( pEntity!=NULL )
				m_LevelsLables.append( pEntity->objectId() );
		}
	}
}

void Matrix::DeleteAllEntities()
{
	DeleteEntities(&m_RLIds);
	DeleteEntities(&m_RLIdsText);
	DeleteEntities(&m_Levels);
	DeleteEntities(&m_LevelsLables);
}

void Matrix::MoveLevel( Entity *pEntity )
{
	int				i;
	bool			bFound;
	AcDbObjectId	id, idCompare;

	//confirm the find the entity's id is in the list of Level lines
	i=0;
	id = pEntity->objectId();
	bFound = false;
	while( i<m_Levels.length() )
	{
		idCompare = m_Levels[i];
		if( idCompare==id )
		{
			bFound = true;
			break;
		}
		i++;
	}
	
	GetController()->IgnorMovement(true);
	if( bFound )
	{
		double				dRL;
		Point3D				pt;
		Vector3D			Vector;
		Matrix3D			Transform;
		Point3DArray		gripPoints;
		AcDbIntArray		osnapModes, geomIds;
		Acad::ErrorStatus	es;

		es = pEntity->getGripPoints( gripPoints, osnapModes, geomIds);
		;//assert( gripPoints.length()>0 );

		GetController()->RemoveLevel(i);

		pt.set( 0.00, 0.00, 0.00 );
		pt.transformBy( GetMatrixCrosshairTransform() );

		dRL = gripPoints[0].y-pt.y;

#ifndef IGNOR_JACK_OFFSET
		dRL+= JACK_LENGTH_APPROX;
#endif	//#ifndef IGNOR_JACK_OFFSET

		GetController()->AddLevel( dRL );

		SetMatrixLevelsChanged( true );
/*		
		pt.y+= dRL;
		Vector = pt - gripPoints[0];
		m_LevelTransform.setToTranslation( Vector );
*/
	}
	GetController()->IgnorMovement(false);
}

/*
void Matrix::DoMoveLevel( AcDbObjectId id )
{
	int				i;
	bool			bFound;
	AcDbObjectId	idCompare;

	//confirm the find the entity's id is in the list of Level lines
	i=0;
	bFound = false;
	while( i<m_Levels.length() )
	{
		idCompare = m_Levels[i];
		if( idCompare==id )
		{
			bFound = true;
			break;
		}
		i++;
	}
	
	if( bFound )
	{
		Entity				*pEnt;
		Acad::ErrorStatus	es;
		es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, id, AcDb::kForWrite );

		if( es==Acad::eOk )
		{
			;//assert( pEnt->isWriteEnabled() );
		}
		else if( es==Acad::eWasOpenForWrite )
		{
			//Fine
			1;
		}
		else if( es==Acad::eWasNotifying )
		{
			pEnt->close();
			es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, id, AcDb::kForWrite );
			;//assert(false);
			return;
		}
		else if( es==Acad::eWasOpenForNotify )
		{
			pEnt->close();
			es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, id, AcDb::kForWrite );
			;//assert(false);
			return;
		}
		else
		{
			;//assert(false);
			return;
		}

		pEnt->transformBy( GetLevelTransform() );
		pEnt->close();
		m_LevelTransform.setToIdentity();
	}
}
*/

Matrix3D Matrix::GetLevelTransform()
{
	return m_LevelTransform;
}


void Matrix::RegenLevels()
{
	int		iSize;
	double	dRLMax, dRLMin, dXOffMin, dXOffMax;

	DeleteEntities(&m_Levels);
	DeleteEntities(&m_LevelsLables);

	///////////////////////////////////////////////////////////////////
	//find the vertical limits of the matrix
	GetRLExtents( dRLMax, dRLMin);

	///////////////////////////////////////////////////////////////////
	//Find the X axis extents of the matrix
	iSize = GetMatrixSize();
	dXOffMin = 0.40*MATRIX_SEPARATION_ALONG_X;
	dXOffMax = ((double)iSize+0.75)*MATRIX_SEPARATION_ALONG_X;

	DrawLevels( dXOffMin, dXOffMin+(0.10*MATRIX_SEPARATION_ALONG_X), dXOffMax-dXOffMin );
}

bool Matrix::HaveMatrixLevelsChanged()
{
	return m_bMatrixLevelsChanged;
}

void Matrix::SetMatrixLevelsChanged(bool bChanged/*=true*/)
{
	m_bMatrixLevelsChanged = bChanged;
}

void Matrix::DeleteLevel(const Entity *pEntity)
{
	int				i;
	AcDbObjectId	id, idCompare;

	//confirm the find the entity's id is in the list of Level lines
	i=0;
	id = pEntity->objectId();
	while( i<m_Levels.length() )
	{
		idCompare = m_Levels[i];
		if( idCompare==id )
		{
			GetController()->RemoveLevel(i);
			break;
		}
		i++;
	}
}


void Matrix::MoveCrosshairOnly(Matrix3D Transform)
{
	#ifdef	SHOW_MATRIX_XHAIR
		GetController()->IgnorMovement(true);
		m_pCrosshair->_topen();
		m_pCrosshair->transformBy( Transform );
		m_pCrosshair->close();
		GetController()->IgnorMovement(false);
	#endif	//#ifdef	SHOW_MATRIX_XHAIR
}

Entity * Matrix::DrawLine( Point3D pt1, Point3D pt2, int iColour, PlotSyleEnum eHL/*=PS_LINE*/ )
{
	TCHAR			PlotStyleName[30];
	Entity			*pLine;
	AcDbObjectId	idCompare;
	EntityReactor	*pReactor;

	pLine = (Entity*) new AcDbLine( pt1, pt2 );

	/////////////////////////////////////////////////////////////////////
	//Colour
	pLine->setColorIndex( (Adesk::UInt16)iColour );

	/////////////////////////////////////////////////////////////////////
	//Move according to crosshair
//	pLine->transformBy( GetMatrixCrosshairTransform() );

	///////////////////////////////////////////////////////////////////
	//Style
	GetPlotStyleName( eHL, PlotStyleName );
	if( pLine->setLinetype(PlotStyleName)!=Acad::eOk )
	{
		acutPrintf(_T("\nCan't find %s Style!\n"), PlotStyleName );
	}

	/////////////////////////////////////////////////////////////////////
	//add to database
	if( IsCutThrough() )
	{
		pLine->postToDatabase( LAYER_NAME_MATRIX );
	}
	else
	{
		pLine->postToDatabase( LAYER_NAME_MATRIX );
		pLine->AttachedReactors( &pReactor );
		pReactor->SetMatrixPointer( this );
	}
	pLine->close();

	return pLine;
}

void Matrix::SelectionSort(bool bIncremental/*=true*/)
{
	int				i, j;
	double			dElement, dTemp;
	MatrixElement	*pElement, *pTemp;

	for( i=0; i<GetSize()-1; i++ )
	{
		pElement = GetAt(i);
		dElement = pElement->GetHeight(MATRIX_STANDARDS_INNER);
		
		for( j=i+1; j<GetSize(); j++ )
		{
			pTemp = GetAt(j);
			dTemp = pTemp->GetHeight(MATRIX_STANDARDS_INNER);

			if( ( bIncremental && dElement>dTemp ) ||
				(!bIncremental && dElement<dTemp ) )
			{
				SetAt( i, pTemp );
				SetAt( j, pElement );
				pElement->SetMatrixElementID(j);
				pTemp->SetMatrixElementID(i);
				//we have changed the value at index i, so get it again!
				pElement = pTemp;
				dElement = dTemp;
			}
		}
	}
}

bool Matrix::IsCutThrough()
{
	return m_bCutThrough;
}

void Matrix::SetCutThrough(bool bCutThrough)
{
	m_bCutThrough = bCutThrough;
}

void Matrix::SetCutThroughLabel(LPCTSTR strLabel)
{
	m_sCutThroughLabel = strLabel;
	m_bCutThroughLabelSet = true;
}

bool Matrix::GetCutThroughLabel(CString &sLabel)
{
	sLabel = m_sCutThroughLabel;
	return m_bCutThroughLabelSet;
}

void Matrix::SetAllowCancel(bool bAllow)
{
	m_bAllowCancel = bAllow;
}

bool Matrix::IsCancelAllowed()
{
	return m_bAllowCancel;
}

bool Matrix::AllSameSystem()
{
	int					i, iSystem;
	MatrixElement		*pElement;

	for( i=0; i<GetMatrixSize(); i++ )
	{
		pElement = GetAt(i);
		if( i==0 )
		{
			iSystem = (int)pElement->GetSystem();
		}
		else
		{
			if( iSystem!=(int)pElement->GetSystem() )
			{
				return false;
			}
		}
	}
	return true;
}
