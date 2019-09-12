// PreviewTemplate1.cpp: implementation of the PreviewTemplate class.
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		PreviewTemplate, RunTemplate, BayTemplate, LapBoardTemplate
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 13/12/00 11:11a $
//	Version			: $Revision: 116 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// These Classes are used for the Templates to create the preview bays.  It may
//	be possible in the future remove these classes and take the details directly
//	from the AutobuildRun class
///////////////////////////////////////////////////////////////////////////////

//includes
#include "stdafx.h"
#include "PreviewTemplate.h"
#include "Bay.h"
#include "Run.h"
#include "Controller.h"
#include "Entity.h"
#include <dbents.h>
#include <dbgroup.h>
#include "Matrix.h"
#include "ComponentDetails.h"
#include <geassign.h>
#include <acdb.h>

#ifdef	USE_LINE_CLASS
#include "Line.h"
#endif	//#ifdef	USE_LINE_CLASS

//#define	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC

//JSB Note 20000511 - Mark assures me that Tie should not be copied when copying bays
//#define	ALLOW_COPY_TIES		

extern Controller *gpController;

const double	DEFAULT_DISTANCE_FROM_WALL	= 225.00;
const double	DEFAULT_RL_START			=   0.00;
const double	DEFAULT_RL_END				=   0.00;


//////////////////////////////// PREVIEW TEMPLATE MEMBER FUNCTIONS BELOW **********

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//PreviewTemplate
//
PreviewTemplate::PreviewTemplate()
{
	SetSchematicGroup( NULL, false );
	m_SchematicTransform.setToIdentity();
	SetGlobalsToDefault();
	m_bCreateEntities = true;
	Matrix3D	Temp;
	Temp.setToIdentity();
	SetSchematicTransform( Temp );
}

//////////////////////////////////////////////////////////////////////
//~PreviewTemplate
//
PreviewTemplate::~PreviewTemplate()
{
	DeleteSchematic();
	SetSchematicGroup(NULL, false);
}


///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void PreviewTemplate::Serialize(CArchive &ar)
{
	int			iSize, iPointIndex;
	CString		sMsg, sTemp;
	Matrix3D	Transform;

	if (ar.IsStoring())    // Store Object?
	{
		ar << PREVIEW_TEMPLATE_VERSION_LATEST;

		//PREVIEW_TEMPLATE_VERSION_1_0_1 :
		iSize = BP_SIZE;
		ar << iSize;	//store the size from now onward
		//PREVIEW_TEMPLATE_VERSION_1_0_0 :
		for( iPointIndex=0; iPointIndex<iSize; iPointIndex++ )
		{
			ar << m_aPoints[iPointIndex].x;
			ar << m_aPoints[iPointIndex].y;
			ar << m_aPoints[iPointIndex].z;
		}
		GetSchematicTransform().Serialize(ar);
	}
	else					// or Load Object?
	{
		VersionNumber uiVersion;
		iSize = BP_5BLADDR+1;	//This was the original size b4 mills stuff
		ar >> uiVersion;
		switch (uiVersion)
		{
		case PREVIEW_TEMPLATE_VERSION_1_0_1 :
			ar >> iSize;
			//fallthrough;
		case PREVIEW_TEMPLATE_VERSION_1_0_0 :
			for( iPointIndex=0; iPointIndex < iSize; iPointIndex++ )
			{
				ar >> m_aPoints[iPointIndex].x;
				ar >> m_aPoints[iPointIndex].y;
				ar >> m_aPoints[iPointIndex].z;
			}
			Transform.Serialize(ar);
			SetSchematicTransform(Transform);
			break;
		default:
			;//assert( false );
			if( uiVersion>PREVIEW_TEMPLATE_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: PreviewTemplate.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), PREVIEW_TEMPLATE_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

///////////////////////////////////// RUN TEMPLATE MEMBER FUNCTIONS BELOW **********

//////////////////////////////////////////////////////////////////////
//RunTemplate
//
RunTemplate::RunTemplate()
{
	m_BoundingBox.setPhysicalLength(0);
	m_pLapboardBox.setPhysicalLength(0);
	SetGlobalsToDefault();
	SetRLStart( DEFAULT_RL_START );
	SetRLEnd( DEFAULT_RL_END );
	SetDistanceToWall( DEFAULT_DISTANCE_FROM_WALL );
}

//////////////////////////////////////////////////////////////////////
//~RunTemplate
//
RunTemplate::~RunTemplate()
{
	m_pLapboardBox.RemoveAll();
	m_BoundingBox.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
//BayTemplate
//
BayTemplate::BayTemplate()
{
	SetBayPointer(NULL);
	SetOffsetForShortStage( 0.00 );
	SetGlobalsToDefault();
	SetTieTubeVertSeparation( DEFAULT_TIE_VERTICAL_SPACING );
	m_mtMillsCnrType = MILLS_TYPE_NONE;
	RemoveAllChainMesh();
	RemoveAllShadeCloth();
}

//////////////////////////////////////////////////////////////////////
//~BayTemplate
//
BayTemplate::~BayTemplate()
{

}

//////////////////////////////////////////////////////////////////////
//LapBoardTemplate
//
LapBoardTemplate::LapBoardTemplate()
{
	SetGlobalsToDefault();
}

//////////////////////////////////////////////////////////////////////
//~LapBoardTemplate
//
LapBoardTemplate::~LapBoardTemplate()
{
}

//////////////////////////////////////////////////////////////////////
//Transform
//
void LapBoardTemplate::Transform(Matrix3D Transform)
{
	int i;
	for( i=0; i<BP_SIZE; i++ )
	{
		m_aPoints[i] = Transform * m_aPoints[i];
	}

}

//////////////////////////////////////////////////////////////////////
//Transform
//
void BayTemplate::Transform( Matrix3D Transform )
{
	int i;
	for( i=0; i<BP_SIZE; i++ )
	{
		m_aPoints[i] = Transform * m_aPoints[i];
	}
}

//////////////////////////////////////////////////////////////////////////////////
//CreatePoints
//
void LapBoardTemplate::CreatePoints()
{
	int			iArrowHeadLong, iArrowHeadWide, iArrowHeadHalfWidth;
	Vector3D	Vector;
	Matrix3D	Transform;
		
	iArrowHeadLong = int( ( m_dFullLength * m_dArrowHeadPercentLong ) + 0.5 );
	iArrowHeadWide = int( ( m_dFullWidth  * m_dArrowHeadPercentWide ) + 0.5 );
	iArrowHeadHalfWidth = int( ( m_dFullWidth/3.00 ) + 0.5 );

	//
	// 11_ _ _ _1_ _ _ _ _4_ _ _ _12
	//         /|         |\
	//       /  |_________|  \
	//     /    2         3    \
	//  0/                       \5
	// 10\                       /
	//     \    8_________7    /
	//       \  |         |  /
	// 13 _ _ _\|_ _ _ _ _|/_ _ _ 14
	//          9         6
	//

	//set the location of first point to the origin
	m_aPoints[POINT_LAPBOARD_00].x = 0.00;
	m_aPoints[POINT_LAPBOARD_00].y = 0.00;
	m_aPoints[POINT_LAPBOARD_00].z = 0.00;

	//m_aPoints[POINT_LAPBOARD_01]
	m_aPoints[POINT_LAPBOARD_01] = m_aPoints[POINT_LAPBOARD_00];
	m_aPoints[POINT_LAPBOARD_01].x += iArrowHeadLong;
	m_aPoints[POINT_LAPBOARD_01].y += iArrowHeadHalfWidth;

	//m_aPoints[POINT_LAPBOARD_02]
	m_aPoints[POINT_LAPBOARD_02] = m_aPoints[POINT_LAPBOARD_01];
	m_aPoints[POINT_LAPBOARD_02].y -= iArrowHeadWide;
	
	//m_aPoints[POINT_LAPBOARD_03]
	m_aPoints[POINT_LAPBOARD_03] = m_aPoints[POINT_LAPBOARD_02];
	m_aPoints[POINT_LAPBOARD_03].x = m_dFullLength - iArrowHeadLong;
	
	//m_aPoints[POINT_LAPBOARD_04]
	m_aPoints[POINT_LAPBOARD_04] = m_aPoints[POINT_LAPBOARD_03];
	m_aPoints[POINT_LAPBOARD_04].y += iArrowHeadWide;
	
	//m_aPoints[POINT_LAPBOARD_05]
	m_aPoints[POINT_LAPBOARD_05] = m_aPoints[POINT_LAPBOARD_00];
	m_aPoints[POINT_LAPBOARD_05].x += m_dFullLength;
	
	//m_aPoints[POINT_LAPBOARD_06]
	m_aPoints[POINT_LAPBOARD_06] = m_aPoints[POINT_LAPBOARD_04];
	m_aPoints[POINT_LAPBOARD_06].y = -1*m_aPoints[POINT_LAPBOARD_04].y;

	//m_aPoints[POINT_LAPBOARD_07]
	m_aPoints[POINT_LAPBOARD_07] = m_aPoints[POINT_LAPBOARD_03];
	m_aPoints[POINT_LAPBOARD_07].y = -1*m_aPoints[POINT_LAPBOARD_03].y;
	
	//m_aPoints[POINT_LAPBOARD_08]
	m_aPoints[POINT_LAPBOARD_08] = m_aPoints[POINT_LAPBOARD_02];
	m_aPoints[POINT_LAPBOARD_08].y = -1*m_aPoints[POINT_LAPBOARD_02].y;

	//m_aPoints[POINT_LAPBOARD_09]
	m_aPoints[POINT_LAPBOARD_09] = m_aPoints[POINT_LAPBOARD_01];
	m_aPoints[POINT_LAPBOARD_09].y = -1*m_aPoints[POINT_LAPBOARD_01].y;
	
	//m_aPoints[POINT_LAPBOARD_10]
	m_aPoints[POINT_LAPBOARD_10] = m_aPoints[POINT_LAPBOARD_00];
 
	//m_aPoints[POINT_LAPBOARD_11]
	m_aPoints[POINT_LAPBOARD_11] = m_aPoints[POINT_LAPBOARD_00];
	m_aPoints[POINT_LAPBOARD_11].y += iArrowHeadHalfWidth;

	//m_aPoints[POINT_LAPBOARD_12]
	m_aPoints[POINT_LAPBOARD_12] = m_aPoints[POINT_LAPBOARD_05];
	m_aPoints[POINT_LAPBOARD_12].y += iArrowHeadHalfWidth;
	
	//m_aPoints[POINT_LAPBOARD_13]
	m_aPoints[POINT_LAPBOARD_13] = m_aPoints[POINT_LAPBOARD_05];
	m_aPoints[POINT_LAPBOARD_13].y -= iArrowHeadHalfWidth;

	//m_aPoints[POINT_LAPBOARD_14]
	m_aPoints[POINT_LAPBOARD_14] = m_aPoints[POINT_LAPBOARD_00];
	m_aPoints[POINT_LAPBOARD_14].y -= iArrowHeadHalfWidth;
}

//////////////////////////////////////////////////////////////////////////////////
//CreatePoints
//
void BayTemplate::CreatePoints()
{
	double	dNStage, dEStage, dSStage, dWStage, dBayLength, dBayWidth;

	///////////////////////////////////////////////////////////////////
	dNStage = GetWidthOfNStage();
	dEStage = GetWidthOfEStage();
	dSStage = GetWidthOfSStage();
	dWStage = GetWidthOfWStage();

	dBayLength	= GetBayLengthActual();
	dBayWidth	= GetBayWidthActual();

	///////////////////////////////////////////////////////////////////
	//Corner fillers without 2 hopups
	//NE
	if( GetNEStageCnr() && dNStage<=0 )
		dNStage = GetLengthOfBracket( NNE );
	if( GetNEStageCnr() && dEStage<=0 )
		dEStage = GetLengthOfBracket( ENE );
	//SE
	if( GetSEStageCnr() && dEStage<=0 )
		dEStage = GetLengthOfBracket( ESE );
	if( GetSEStageCnr() && dSStage<=0 )
		dSStage = GetLengthOfBracket( SSE );
	//SW
	if( GetSWStageCnr() && dSStage<=0 )
		dSStage = GetLengthOfBracket( SSW );
	if( GetSWStageCnr() && dWStage<=0 )
		dWStage = GetLengthOfBracket( WSW );
	//NW
	if( GetNWStageCnr() && dWStage<=0 )
		dWStage = GetLengthOfBracket( WNW );
	if( GetNWStageCnr() && dNStage<=0 )
		dNStage = GetLengthOfBracket( NNW );

	///////////////////////////////////////////////////////////////////
	//set the location of first point to the origin
	m_aPoints[BP_SW_STD].set( 0.00, 0.00, 0.00 );

	///////////////////////////////////////////////////////////////////
	//m_aPoints[BP_SE_STD] is the other inner standard
	m_aPoints[BP_SE_STD] =	m_aPoints[BP_SW_STD];
	m_aPoints[BP_SE_STD].x += dBayLength;
	
	///////////////////////////////////////////////////////////////////
	//m_aPoints[BP_NW_STD] is the first outer standard
	m_aPoints[BP_NW_STD] =	m_aPoints[BP_SW_STD];
	m_aPoints[BP_NW_STD].y += dBayWidth;
	
	///////////////////////////////////////////////////////////////////
	//m_aPoints[BP_NE_STD] is the second outer standard
	m_aPoints[BP_NE_STD] =	m_aPoints[BP_SE_STD];
	m_aPoints[BP_NE_STD].y += dBayWidth;

	///////////////////////////////////////////////////////////////////
	//Mills system needs 1/2 points on each side
	m_aPoints[BP_N_MILLS].x = (m_aPoints[BP_NE_STD].x + m_aPoints[BP_NW_STD].x)/2.00;
	m_aPoints[BP_N_MILLS].y = (m_aPoints[BP_NE_STD].y + m_aPoints[BP_NW_STD].y)/2.00;
	m_aPoints[BP_N_MILLS].z = (m_aPoints[BP_NE_STD].z + m_aPoints[BP_NW_STD].z)/2.00;
	m_aPoints[BP_E_MILLS].x = (m_aPoints[BP_NE_STD].x + m_aPoints[BP_SE_STD].x)/2.00;
	m_aPoints[BP_E_MILLS].y = (m_aPoints[BP_NE_STD].y + m_aPoints[BP_SE_STD].y)/2.00;
	m_aPoints[BP_E_MILLS].z = (m_aPoints[BP_NE_STD].z + m_aPoints[BP_SE_STD].z)/2.00;
	m_aPoints[BP_S_MILLS].x = (m_aPoints[BP_SE_STD].x + m_aPoints[BP_SW_STD].x)/2.00;
	m_aPoints[BP_S_MILLS].y = (m_aPoints[BP_SE_STD].y + m_aPoints[BP_SW_STD].y)/2.00;
	m_aPoints[BP_S_MILLS].z = (m_aPoints[BP_SE_STD].z + m_aPoints[BP_SW_STD].z)/2.00;
	m_aPoints[BP_W_MILLS].x = (m_aPoints[BP_SW_STD].x + m_aPoints[BP_NW_STD].x)/2.00;
	m_aPoints[BP_W_MILLS].y = (m_aPoints[BP_SW_STD].y + m_aPoints[BP_NW_STD].y)/2.00;
	m_aPoints[BP_W_MILLS].z = (m_aPoints[BP_SW_STD].z + m_aPoints[BP_NW_STD].z)/2.00;

	////////////////////////////////////////////////////////////////////////////////
	//Stage
	m_aPoints[BP_NNE_STG] =	m_aPoints[BP_NE_STD];	//North North East Stage board 
	m_aPoints[BP_ENE_STG] =	m_aPoints[BP_NE_STD];	//East  North East Stage board 
	m_aPoints[BP_ESE_STG] =	m_aPoints[BP_SE_STD];	//East  South East Stage board 
	m_aPoints[BP_SSE_STG] =	m_aPoints[BP_SE_STD];	//South South East Stage board 
	m_aPoints[BP_SSW_STG] =	m_aPoints[BP_SW_STD];	//South South West Stage board 
	m_aPoints[BP_WSW_STG] =	m_aPoints[BP_SW_STD];	//West  South West Stage board 
	m_aPoints[BP_WNW_STG] =	m_aPoints[BP_NW_STD];	//West  North West Stage board 
	m_aPoints[BP_NNW_STG] =	m_aPoints[BP_NW_STD];	//North North West Stage board 

	m_aPoints[BP_NNE_STG].y += dNStage;	//East  North East Stage board 
	m_aPoints[BP_ENE_STG].x += dEStage;	//East  North East Stage board 
	m_aPoints[BP_ESE_STG].x += dEStage;	//East  South East Stage board 
	m_aPoints[BP_SSE_STG].y -= dSStage;	//South South East Stage board 
	m_aPoints[BP_SSW_STG].y -= dSStage;	//South South West Stage board 
	m_aPoints[BP_WSW_STG].x -= dWStage;	//West  South West Stage board 
	m_aPoints[BP_WNW_STG].x -= dWStage;	//West  North West Stage board 
	m_aPoints[BP_NNW_STG].y += dNStage;	//North North West Stage board 

	///////////////////////////////////////////////////////////////////
	//Mills system needs 1/2 points on each side of the stage 
	m_aPoints[BP_N_M_STG].x = (m_aPoints[BP_NNW_STG].x + m_aPoints[BP_NNE_STG].x)/2.00;
	m_aPoints[BP_N_M_STG].y = (m_aPoints[BP_NNW_STG].y + m_aPoints[BP_NNE_STG].y)/2.00;
	m_aPoints[BP_N_M_STG].z = (m_aPoints[BP_NNW_STG].z + m_aPoints[BP_NNE_STG].z)/2.00;
	m_aPoints[BP_E_M_STG].x = (m_aPoints[BP_ENE_STG].x + m_aPoints[BP_ESE_STG].x)/2.00;
	m_aPoints[BP_E_M_STG].y = (m_aPoints[BP_ENE_STG].y + m_aPoints[BP_ESE_STG].y)/2.00;
	m_aPoints[BP_E_M_STG].z = (m_aPoints[BP_ENE_STG].z + m_aPoints[BP_ESE_STG].z)/2.00;
	m_aPoints[BP_S_M_STG].x = (m_aPoints[BP_SSE_STG].x + m_aPoints[BP_SSW_STG].x)/2.00;
	m_aPoints[BP_S_M_STG].y = (m_aPoints[BP_SSE_STG].y + m_aPoints[BP_SSW_STG].y)/2.00;
	m_aPoints[BP_S_M_STG].z = (m_aPoints[BP_SSE_STG].z + m_aPoints[BP_SSW_STG].z)/2.00;
	m_aPoints[BP_W_M_STG].x = (m_aPoints[BP_WSW_STG].x + m_aPoints[BP_WNW_STG].x)/2.00;
	m_aPoints[BP_W_M_STG].y = (m_aPoints[BP_WSW_STG].y + m_aPoints[BP_WNW_STG].y)/2.00;
	m_aPoints[BP_W_M_STG].z = (m_aPoints[BP_WSW_STG].z + m_aPoints[BP_WNW_STG].z)/2.00;

	////////////////////////////////////////////////////////////////////////////////
	//Stage corners
	m_aPoints[BP_NE_STG] =	m_aPoints[BP_NNE_STG];	//South South West Stage board corner
	m_aPoints[BP_SE_STG] =	m_aPoints[BP_SSE_STG];	//West  South West Stage board corner
	m_aPoints[BP_SW_STG] =	m_aPoints[BP_SSW_STG];	//West  North West Stage board corner
	m_aPoints[BP_NW_STG] =	m_aPoints[BP_NNW_STG];	//North North West Stage board corner
	m_aPoints[BP_NE_STG].x += dEStage;	//East  North East Stage board 
	m_aPoints[BP_SE_STG].x += dEStage;	//East  North East Stage board 
	m_aPoints[BP_SW_STG].x -= dWStage;	//East  South East Stage board 
	m_aPoints[BP_NW_STG].x -= dWStage;	//South South East Stage board 

	////////////////////////////////////////////////////////////////////////////////
	//Brace
	m_aPoints[BP_NNE_BRC] =	m_aPoints[BP_NE_STD];	//North North East Stage board
	m_aPoints[BP_ENE_BRC] =	m_aPoints[BP_NE_STD];	//East  North East Stage board
	m_aPoints[BP_ESE_BRC] =	m_aPoints[BP_SE_STD];	//East  South East Stage board
	m_aPoints[BP_SSE_BRC] =	m_aPoints[BP_SE_STD];	//South South East Stage board
	m_aPoints[BP_SSW_BRC] =	m_aPoints[BP_SW_STD];	//South South West Stage board
	m_aPoints[BP_WSW_BRC] =	m_aPoints[BP_SW_STD];	//West  South West Stage board
	m_aPoints[BP_WNW_BRC] =	m_aPoints[BP_NW_STD];	//West  North West Stage board
	m_aPoints[BP_NNW_BRC] =	m_aPoints[BP_NW_STD];	//North North West Stage board
	m_aPoints[BP_NNE_BRC].y += m_dBraceDist;	//North North East Stage board
	m_aPoints[BP_ENE_BRC].x += m_dBraceDist;	//East  North East Stage board
	m_aPoints[BP_ESE_BRC].x += m_dBraceDist;	//East  South East Stage board
	m_aPoints[BP_SSE_BRC].y -= m_dBraceDist;	//South South East Stage board
	m_aPoints[BP_SSW_BRC].y -= m_dBraceDist;	//South South West Stage board
	m_aPoints[BP_WSW_BRC].x -= m_dBraceDist;	//West  South West Stage board
	m_aPoints[BP_WNW_BRC].x -= m_dBraceDist;	//West  North West Stage board
	m_aPoints[BP_NNW_BRC].y += m_dBraceDist;	//North North West Stage board

	////////////////////////////////////////////////////////////////////////////////
	//Ties
	m_aPoints[BP_NNE_WAL_TIE]	=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_NNE_WAL_TIE].y	+= dNStage;
	m_aPoints[BP_NNE_WAL_TIE].y	+= GetWidthOfTie();
	m_aPoints[BP_NNE_BEG_TIE]	=  m_aPoints[BP_NNE_WAL_TIE];
	m_aPoints[BP_NNE_BEG_TIE].y	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_NNE_BEG_TIE].x	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_NNE_END_TIE]	=  m_aPoints[BP_NNE_BEG_TIE];
	m_aPoints[BP_NNE_END_TIE].x	+= GetWidthOfTie()/2.00;

	m_aPoints[BP_ENE_WAL_TIE]	=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_ENE_WAL_TIE].x	+= dEStage;
	m_aPoints[BP_ENE_WAL_TIE].x	+= GetWidthOfTie();
	m_aPoints[BP_ENE_BEG_TIE]	=  m_aPoints[BP_ENE_WAL_TIE];
	m_aPoints[BP_ENE_BEG_TIE].y	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_ENE_BEG_TIE].x	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_ENE_END_TIE]	=  m_aPoints[BP_ENE_BEG_TIE];
	m_aPoints[BP_ENE_END_TIE].y	+= GetWidthOfTie()/2.00;

	m_aPoints[BP_ESE_WAL_TIE]	=  m_aPoints[BP_SE_STD];
	m_aPoints[BP_ESE_WAL_TIE].x	+= dEStage;
	m_aPoints[BP_ESE_WAL_TIE].x	+= GetWidthOfTie();
	m_aPoints[BP_ESE_BEG_TIE]	=  m_aPoints[BP_ESE_WAL_TIE];
	m_aPoints[BP_ESE_BEG_TIE].y	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_ESE_BEG_TIE].x	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_ESE_END_TIE]	=  m_aPoints[BP_ESE_BEG_TIE];
	m_aPoints[BP_ESE_END_TIE].y	+= GetWidthOfTie()/2.00;

	m_aPoints[BP_SSE_WAL_TIE]	=  m_aPoints[BP_SE_STD];
	m_aPoints[BP_SSE_WAL_TIE].y	-= dSStage;
	m_aPoints[BP_SSE_WAL_TIE].y	-= GetWidthOfTie();
	m_aPoints[BP_SSE_BEG_TIE]	=  m_aPoints[BP_SSE_WAL_TIE];
	m_aPoints[BP_SSE_BEG_TIE].x	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_SSE_BEG_TIE].y	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_SSE_END_TIE]	=  m_aPoints[BP_SSE_BEG_TIE];
	m_aPoints[BP_SSE_END_TIE].x	+= GetWidthOfTie()/2.00;

	m_aPoints[BP_SSW_WAL_TIE]	=  m_aPoints[BP_SW_STD];
	m_aPoints[BP_SSW_WAL_TIE].y	-= dSStage;
	m_aPoints[BP_SSW_WAL_TIE].y	-= GetWidthOfTie();
	m_aPoints[BP_SSW_BEG_TIE]	=  m_aPoints[BP_SSW_WAL_TIE];
	m_aPoints[BP_SSW_BEG_TIE].x	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_SSW_BEG_TIE].y	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_SSW_END_TIE]	=  m_aPoints[BP_SSW_BEG_TIE];
	m_aPoints[BP_SSW_END_TIE].x	+= GetWidthOfTie()/2.00;

	m_aPoints[BP_WSW_WAL_TIE]	=  m_aPoints[BP_SW_STD];
	m_aPoints[BP_WSW_WAL_TIE].x	-= dWStage;
	m_aPoints[BP_WSW_WAL_TIE].x	-= GetWidthOfTie();
	m_aPoints[BP_WSW_BEG_TIE]	=  m_aPoints[BP_WSW_WAL_TIE];
	m_aPoints[BP_WSW_BEG_TIE].y	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_WSW_BEG_TIE].x	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_WSW_END_TIE]	=  m_aPoints[BP_WSW_BEG_TIE];
	m_aPoints[BP_WSW_END_TIE].y	-= GetWidthOfTie()/2.00;

	m_aPoints[BP_WNW_WAL_TIE]	=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_WNW_WAL_TIE].x	-= dWStage;
	m_aPoints[BP_WNW_WAL_TIE].x	-= GetWidthOfTie();
	m_aPoints[BP_WNW_BEG_TIE]	=  m_aPoints[BP_WNW_WAL_TIE];
	m_aPoints[BP_WNW_BEG_TIE].y	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_WNW_BEG_TIE].x	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_WNW_END_TIE]	=  m_aPoints[BP_WNW_BEG_TIE];
	m_aPoints[BP_WNW_END_TIE].y	-= GetWidthOfTie()/2.00;

	m_aPoints[BP_NNW_WAL_TIE]	=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_NNW_WAL_TIE].y	+= dNStage;
	m_aPoints[BP_NNW_WAL_TIE].y	+= GetWidthOfTie();
	m_aPoints[BP_NNW_BEG_TIE]	=  m_aPoints[BP_NNW_WAL_TIE];
	m_aPoints[BP_NNW_BEG_TIE].y	-= GetWidthOfTie()/4.00;
	m_aPoints[BP_NNW_BEG_TIE].x	+= GetWidthOfTie()/4.00;
	m_aPoints[BP_NNW_END_TIE]	=  m_aPoints[BP_NNW_BEG_TIE];
	m_aPoints[BP_NNW_END_TIE].x	-= GetWidthOfTie()/2.00;

	///////////////////////////////////////////////////////////////////
	//Buttress Bay Points
	double dButtressWidth;
	dButtressWidth = GetBayPointer()->GetBayDimensionActual( COMPONENT_LENGTH_1200, false, false );
	m_aPoints[BP_BUT12_NW]		=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_BUT12_NW].y	+= dButtressWidth;
	m_aPoints[BP_BUT12_NE]		=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_BUT12_NE].y	+= dButtressWidth;
	dButtressWidth = GetBayPointer()->GetBayDimensionActual( COMPONENT_LENGTH_1800, false, false );
	m_aPoints[BP_BUT18_NW]		=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_BUT18_NW].y	+= dButtressWidth;
	m_aPoints[BP_BUT18_NE]		=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_BUT18_NE].y	+= dButtressWidth;
	dButtressWidth = GetBayPointer()->GetBayDimensionActual( COMPONENT_LENGTH_2400, false, false );
	m_aPoints[BP_BUT24_NW]		=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_BUT24_NW].y	+= dButtressWidth;
	m_aPoints[BP_BUT24_NE]		=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_BUT24_NE].y	+= dButtressWidth;

	///////////////////////////////////////////////////////////////////
	//Chain mesh needs two points
	m_aPoints[BP_CHAIN_NNE]		=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_CHAIN_NNE].y	+= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_ENE]		=  m_aPoints[BP_NE_STD];
	m_aPoints[BP_CHAIN_ENE].x	+= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_ESE]		=  m_aPoints[BP_SE_STD];
	m_aPoints[BP_CHAIN_ESE].x	+= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_SSE]		=  m_aPoints[BP_SE_STD];
	m_aPoints[BP_CHAIN_SSE].y	-= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_SSW]		=  m_aPoints[BP_SW_STD];
	m_aPoints[BP_CHAIN_SSW].y	-= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_WSW]		=  m_aPoints[BP_SW_STD];
	m_aPoints[BP_CHAIN_WSW].x	-= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_WNW]		=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_CHAIN_WNW].x	-= m_dShadeChainDist;
	m_aPoints[BP_CHAIN_NNW]		=  m_aPoints[BP_NW_STD];
	m_aPoints[BP_CHAIN_NNW].y	+= m_dShadeChainDist;

	///////////////////////////////////////////////////////////////////
	//Handrail
	m_aPoints[BP_NE_HND] = m_aPoints[BP_NE_STD];
	m_aPoints[BP_SE_HND] = m_aPoints[BP_SE_STD];
	m_aPoints[BP_SW_HND] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_NW_HND] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_NE_HND].x-= GetHandRailDist();
	m_aPoints[BP_SE_HND].x-= GetHandRailDist();
	m_aPoints[BP_SW_HND].x+= GetHandRailDist();
	m_aPoints[BP_NW_HND].x+= GetHandRailDist();
	m_aPoints[BP_NE_HND].y-= GetHandRailDist();
	m_aPoints[BP_SE_HND].y+= GetHandRailDist();
	m_aPoints[BP_SW_HND].y+= GetHandRailDist();
	m_aPoints[BP_NW_HND].y-= GetHandRailDist();

	///////////////////////////////////////////////////////////////////
	//Arrow
	//                 \NthArw
	//                   \
	//		_______________\
	//   EndArw            /PntArw
	//                   /
	//                 /SthArw
	//
	m_aPoints[BP_END_ARW] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_END_ARW].x+= dBayLength/4.00;
	m_aPoints[BP_END_ARW].y+= dBayWidth/8.00;

	m_aPoints[BP_PNT_ARW] = m_aPoints[BP_END_ARW];
	m_aPoints[BP_PNT_ARW].x+= dBayLength/2.00;

	m_aPoints[BP_NTH_ARW] = m_aPoints[BP_PNT_ARW];
	m_aPoints[BP_NTH_ARW].x-= dBayLength/8.00;
	m_aPoints[BP_NTH_ARW].y+= dBayWidth/16.00;

	m_aPoints[BP_STH_ARW] = m_aPoints[BP_NTH_ARW];
	m_aPoints[BP_STH_ARW].y-= dBayWidth/8.00;


	///////////////////////////////////////////////////////////////////
	//Stairs
	//     1A 2A 3A 4A 5A 6A
	//      |  |  |  |  |  |
	//      |  |  |  |  |  |
	// W____|__|__|__|__|__|____E
	//      |  |  |  |  |  |
	//      |  |  |  |  |  |
	//      |  |  |  |  |  |
	//     1B 2B 3B 4B 5B 6B

	//E
	m_aPoints[BP_E_STAIR] = m_aPoints[BP_SE_STD];
	m_aPoints[BP_E_STAIR].y = ((m_aPoints[BP_NE_STD].y-m_aPoints[BP_E_STAIR].y)/2.00)+m_aPoints[BP_E_STAIR].y;
	//W
	m_aPoints[BP_W_STAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_W_STAIR].y = ((m_aPoints[BP_NW_STD].y-m_aPoints[BP_W_STAIR].y)/2.00)+m_aPoints[BP_W_STAIR].y;
	//1A
	m_aPoints[BP_1ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_1ASTAIR].x = (3.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//1B
	m_aPoints[BP_1BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_1BSTAIR].x = (3.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;
	//2A
	m_aPoints[BP_2ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_2ASTAIR].x = (4.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//2B
	m_aPoints[BP_2BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_2BSTAIR].x = (4.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;
	//3A
	m_aPoints[BP_3ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_3ASTAIR].x = (5.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//3B
	m_aPoints[BP_3BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_3BSTAIR].x = (5.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;
	//4A
	m_aPoints[BP_4ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_4ASTAIR].x = (6.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//4B
	m_aPoints[BP_4BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_4BSTAIR].x = (6.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;
	//5A
	m_aPoints[BP_5ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_5ASTAIR].x = (7.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//5B
	m_aPoints[BP_5BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_5BSTAIR].x = (7.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;
	//6A
	m_aPoints[BP_6ASTAIR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_6ASTAIR].x = (8.00*(m_aPoints[BP_NW_STD].x-m_aPoints[BP_NE_STD].x)/11.00)+m_aPoints[BP_NE_STD].x;
	//6B
	m_aPoints[BP_6BSTAIR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_6BSTAIR].x = (8.00*(m_aPoints[BP_SW_STD].x-m_aPoints[BP_SE_STD].x)/11.00)+m_aPoints[BP_SE_STD].x;


	///////////////////////////////////////////////////////////////////
	//Ladder
	//               N 1A 2A 3A 4A 5A 6A
	//               |  |  |  |  |  |  |
	//               |  |  |  |  |  |  |
	//               |__|__|__|__|__|__|__E
	//               C 1B 2B 3B 4B 5B 6B
	//
	//
	//
	//
	//E
	m_aPoints[BP_E_LADDR] = m_aPoints[BP_SE_STD];
	m_aPoints[BP_E_LADDR].y = ((m_aPoints[BP_NE_STD].y-m_aPoints[BP_E_LADDR].y)/2.00)+m_aPoints[BP_E_LADDR].y;
	//N
	m_aPoints[BP_N_LADDR] = m_aPoints[BP_NW_STD];
	m_aPoints[BP_N_LADDR].x = ((m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/2.00)+m_aPoints[BP_N_LADDR].x;
	//C
	m_aPoints[BP_C_LADDR] = m_aPoints[BP_SW_STD];
	m_aPoints[BP_C_LADDR].x = ((m_aPoints[BP_NE_STD].x-m_aPoints[BP_C_LADDR].x)/2.00)+m_aPoints[BP_C_LADDR].x;
	m_aPoints[BP_C_LADDR].y = ((m_aPoints[BP_NE_STD].y-m_aPoints[BP_C_LADDR].y)/2.00)+m_aPoints[BP_C_LADDR].y;
	//1A
	m_aPoints[BP_1ALADDR] = m_aPoints[BP_N_LADDR];
	m_aPoints[BP_1ALADDR].x = (1.00*(m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/6.00)+m_aPoints[BP_N_LADDR].x;
	//1B
	m_aPoints[BP_1BLADDR] = m_aPoints[BP_C_LADDR];
	m_aPoints[BP_1BLADDR].x = (1.00*(m_aPoints[BP_E_LADDR].x-m_aPoints[BP_C_LADDR].x)/6.00)+m_aPoints[BP_C_LADDR].x;
	//2A
	m_aPoints[BP_2ALADDR] = m_aPoints[BP_N_LADDR];
	m_aPoints[BP_2ALADDR].x = (2.00*(m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/6.00)+m_aPoints[BP_N_LADDR].x;
	//2B
	m_aPoints[BP_2BLADDR] = m_aPoints[BP_C_LADDR];
	m_aPoints[BP_2BLADDR].x = (2.00*(m_aPoints[BP_E_LADDR].x-m_aPoints[BP_C_LADDR].x)/6.00)+m_aPoints[BP_C_LADDR].x;
	//3A
	m_aPoints[BP_3ALADDR] = m_aPoints[BP_N_LADDR];
	m_aPoints[BP_3ALADDR].x = (3.00*(m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/6.00)+m_aPoints[BP_N_LADDR].x;
	//3B
	m_aPoints[BP_3BLADDR] = m_aPoints[BP_C_LADDR];
	m_aPoints[BP_3BLADDR].x = (3.00*(m_aPoints[BP_E_LADDR].x-m_aPoints[BP_C_LADDR].x)/6.00)+m_aPoints[BP_C_LADDR].x;
	//4A
	m_aPoints[BP_4ALADDR] = m_aPoints[BP_N_LADDR];
	m_aPoints[BP_4ALADDR].x = (4.00*(m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/6.00)+m_aPoints[BP_N_LADDR].x;
	//4B
	m_aPoints[BP_4BLADDR] = m_aPoints[BP_C_LADDR];
	m_aPoints[BP_4BLADDR].x = (4.00*(m_aPoints[BP_E_LADDR].x-m_aPoints[BP_C_LADDR].x)/6.00)+m_aPoints[BP_C_LADDR].x;
	//5A
	m_aPoints[BP_5ALADDR] = m_aPoints[BP_N_LADDR];
	m_aPoints[BP_5ALADDR].x = (5.00*(m_aPoints[BP_NE_STD].x-m_aPoints[BP_N_LADDR].x)/6.00)+m_aPoints[BP_N_LADDR].x;
	//5B
	m_aPoints[BP_5BLADDR] = m_aPoints[BP_C_LADDR];
	m_aPoints[BP_5BLADDR].x = (5.00*(m_aPoints[BP_E_LADDR].x-m_aPoints[BP_C_LADDR].x)/6.00)+m_aPoints[BP_C_LADDR].x;
}

//////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//
void BayTemplate::SetGlobalsToDefault()
{
	m_dLengthOfBays		= 0.00;
	m_dWidthOfBays		= 0.00;
	SetWidthOfTie( COMPONENT_LENGTH_0700 );
	m_dWidthOfNStage	= 0.00;
	m_dWidthOfEStage	= 0.00;
	m_dWidthOfSStage	= 0.00;
	m_dWidthOfWStage	= 0.00;
	m_bNEStageCnr		= false;
	m_bSEStageCnr		= false;
	m_bSWStageCnr		= false;
	m_bNWStageCnr		= false;
	m_bNBrace			= false;
	m_bEBrace			= false;
	m_bSBrace			= false;
	m_bWBrace			= false;
	m_dBraceDist		= GetSchematicStandardRadius();
	m_dShadeChainDist	= GetSchematicStandardRadius()*1.50;
	m_bNHandRail		= false;
	m_bEHandRail		= false;
	m_bSHandRail		= false;
	m_bWHandRail		= false;
	m_dHandRailDist		= 100.00;
	SetNNETie( false );
	SetENETie( false );
	SetESETie( false );
	SetSSETie( false );
	SetSSWTie( false );
	SetWSWTie( false );
	SetWNWTie( false );
	SetNNWTie( false );
//	CreatePoints();
}

//////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//
void LapBoardTemplate::SetGlobalsToDefault()
{
	m_bShowLapBoards		= true;
//	m_bShowLapBoards		= false;
	m_dFullLength			= 0.00;
	m_dFullWidth			= 0.00;
	m_dArrowHeadPercentLong	= 0.20;
	m_dArrowHeadPercentWide	= 0.20;
	m_iOverLapReqd			= 300;
	m_bFirstBayInRun		= false;
	m_bLapBoardReqdThisRun	= false;
	m_bDrawingLapBoard		= false;
	m_bAllowExactLength		= false;
	m_iNumberOfBoards		= 0;
	CreatePoints();
	SetHandRails( false );
}

//////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//
void PreviewTemplate::SetGlobalsToDefault()
{
	int		i;
	Point3D	pt;

	pt.x = 0.00;
	pt.y = 0.00;
	pt.z = 0.00;
	for( i=0; i<BP_SIZE; i++ )
	{
		m_aPoints[i] = pt;
	}
}

//////////////////////////////////////////////////////////////////////
//operator 
//
PreviewTemplate & PreviewTemplate::operator =(const PreviewTemplate &Source)
{
	int i;
	for( i=0; i<BP_SIZE; i++ )
	{
		m_aPoints[i] = Source.m_aPoints[i];
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
//operator 
//
LapBoardTemplate & LapBoardTemplate::operator =(const LapBoardTemplate &Source)
{
	m_bShowLapBoards		= Source.m_bShowLapBoards;
	m_dFullLength			= Source.m_dFullLength;
	m_dFullWidth			= Source.m_dFullWidth;
	m_dArrowHeadPercentLong	= Source.m_dArrowHeadPercentLong;
	m_dArrowHeadPercentWide	= Source.m_dArrowHeadPercentWide;
	m_iOverLapReqd			= Source.m_iOverLapReqd;
	m_bFirstBayInRun		= Source.m_bFirstBayInRun;
	m_bLapBoardReqdThisRun	= Source.m_bLapBoardReqdThisRun;
	m_bDrawingLapBoard		= Source.m_bDrawingLapBoard;
	m_bAllowExactLength		= Source.m_bAllowExactLength;
	m_iNumberOfBoards		= Source.m_iNumberOfBoards;

	CreatePoints();
	return *this;
}

RunTemplate & RunTemplate::operator=(const RunTemplate &Source)
{
	m_ptPointStart			= Source.m_ptPointStart;
	m_ptPointEnd			= Source.m_ptPointEnd;
	m_eTieType				= Source.m_eTieType;
	m_dTiesVerticallyEvery	= Source.m_dTiesVerticallyEvery;

	//Distance to wall
	m_dDistanceFromWall		= Source.m_dDistanceFromWall;

	//Bounding box stuff
	m_pLapboardBox			= Source.m_pLapboardBox;
	m_BoundingBox			= Source.m_BoundingBox;

	//Ties
	SetBaysPerTie( Source.GetBaysPerTie() );
	
	//Bracing
	SetBaysPerBrace( Source.GetBaysPerBrace() );
	SetBracingLength2400( Source.GetBracingLength2400() );
	SetBracingLength1800( Source.GetBracingLength1800() );
	SetBracingLength1200( Source.GetBracingLength1200() );
	SetBracingLength0700( Source.GetBracingLength0700() );

	//fit
	SetFit( Source.GetFit() );

	return *this;
}

//////////////////////////////////////////////////////////////////////
//operator
//
BayTemplate & BayTemplate::operator=(const BayTemplate &Source)
{
	SetSystem( Source.GetSystem() );
	m_dWidthOfBays		= (int)Source.GetBayWidth();
	SetWidthOfTie( Source.GetWidthOfTie() );
	m_dWidthOfSStage	= Source.GetWidthOfSStage();
	m_dWidthOfNStage	= Source.GetWidthOfNStage();
	m_dWidthOfEStage	= Source.GetWidthOfEStage();
	m_dWidthOfWStage	= Source.GetWidthOfWStage();
	m_bNEStageCnr		= Source.GetNEStageCnr();
	m_bSEStageCnr		= Source.GetSEStageCnr();
	m_bSWStageCnr		= Source.GetSWStageCnr();
	m_bNWStageCnr		= Source.GetNWStageCnr();
	m_bNBrace			= Source.GetNBrace();
	m_bEBrace			= Source.GetEBrace();
	m_bSBrace			= Source.GetSBrace();
	m_bWBrace			= Source.GetWBrace();
	m_dLengthOfBays		= Source.GetBayLength();
	m_dBraceDist		= Source.GetBraceDist();
	m_bNHandRail		= Source.GetNHandRail();
	m_bEHandRail		= Source.GetEHandRail();
	m_bSHandRail		= Source.GetSHandRail();
	m_bWHandRail		= Source.GetWHandRail();
	m_dHandRailDist		= Source.GetHandRailDist();
	m_dTieTubeSeparation= Source.m_dTieTubeSeparation;
	SetMillsSystemBay( Source.GetMillsSystemType() );

	#ifdef	ALLOW_COPY_TIES
	m_LooseTieTubes		= Source.m_LooseTieTubes;
	SetNNETie( Source.GetNNETie() );
	SetENETie( Source.GetENETie() );
	SetESETie( Source.GetESETie() );
	SetSSETie( Source.GetSSETie() );
	SetSSWTie( Source.GetSSWTie() );
	SetWSWTie( Source.GetWSWTie() );
	SetWNWTie( Source.GetWNWTie() );
	SetNNWTie( Source.GetNNWTie() );
	#endif	//#ifdef	ALLOW_COPY_TIES
	CreatePoints();
	return *this;
}


//////////////////////////////////////////////////////////////////////
//SetGlobalsToDefault
//
void RunTemplate::SetGlobalsToDefault()
{
//	m_eFit = RF_LOOSE
	SetBaysPerTie( DEFAULT_TIE_SPACING );
	SetBaysPerBrace( DEFAULT_BRACING_SPACING );
	m_eFit = RF_TIGHT;
}

//////////////////////////////////////////////////////////////////////
//Dimensions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//SetBayLength
//
void BayTemplate::SetBayLength(double dLength)
{
	m_dLengthOfBays = dLength;
	CreatePoints();
}

//////////////////////////////////////////////////////////////////////
//SetBayWidth
//
void BayTemplate::SetBayWidth(double dWidth)
{
	m_dWidthOfBays = (int)dWidth;
	CreatePoints();
}

//////////////////////////////////////////////////////////////////////
//GetBayLength
//
double BayTemplate::GetBayLength() const
{
	return m_dLengthOfBays;
}

//////////////////////////////////////////////////////////////////////
//GetBayLengthActual
double BayTemplate::GetBayLengthActual()
{
	return GetBayPointer()->GetBayLengthActual();
}

//////////////////////////////////////////////////////////////////////
//GetBayWidth
//
double BayTemplate::GetBayWidth() const
{
	return m_dWidthOfBays;
}

//////////////////////////////////////////////////////////////////////
//GetBayWidthActual
double BayTemplate::GetBayWidthActual()
{
	return GetBayPointer()->GetBayWidthActual();
}

//////////////////////////////////////////////////////////////////////
//GetWidthOfTie
//
double  BayTemplate::GetWidthOfTie() const
{
	return m_dWidthOfTie;
}

//////////////////////////////////////////////////////////////////////
//SetWidthOfTie

void  BayTemplate::SetWidthOfTie( double dWidthOfTie )
{
	m_dWidthOfTie = dWidthOfTie;
}

//////////////////////////////////////////////////////////////////////
//Stages
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//GetWidthOfNStage
//
double  BayTemplate::GetWidthOfNStage() const
{
	return m_dWidthOfNStage;
}

//////////////////////////////////////////////////////////////////////
//SetWidthOfNStage
//
void  BayTemplate::SetWidthOfNStage( double dWidthOfNStage )
{
	m_dWidthOfNStage = dWidthOfNStage;
}

//////////////////////////////////////////////////////////////////////
//GetWidthOfEStage
//
double  BayTemplate::GetWidthOfEStage() const
{
	return m_dWidthOfEStage;
}

//////////////////////////////////////////////////////////////////////
//SetWidthOfEStage
//
void  BayTemplate::SetWidthOfEStage( double dWidthOfEStage )
{
	m_dWidthOfEStage = dWidthOfEStage;
}

//////////////////////////////////////////////////////////////////////
//GetWidthOfSStage
//
double  BayTemplate::GetWidthOfSStage() const
{
	return m_dWidthOfSStage;
}

//////////////////////////////////////////////////////////////////////
//SetWidthOfSStage
//
void  BayTemplate::SetWidthOfSStage( double dWidthOfSStage )
{
	m_dWidthOfSStage = dWidthOfSStage;
}

//////////////////////////////////////////////////////////////////////
//GetWidthOfWStage
//
double  BayTemplate::GetWidthOfWStage() const
{
	return m_dWidthOfWStage;
}

//////////////////////////////////////////////////////////////////////
//SetWidthOfWStage
//
void  BayTemplate::SetWidthOfWStage( double dWidthOfWStage )
{
	m_dWidthOfWStage = dWidthOfWStage;
}


//////////////////////////////////////////////////////////////////////
//Stage corners
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//GetNEStageCnr
//
bool	BayTemplate::GetNEStageCnr() const
{
	return m_bNEStageCnr;
}

//////////////////////////////////////////////////////////////////////
//SetNEStageCnr
//
void	BayTemplate::SetNEStageCnr( bool bExists )
{
	m_bNEStageCnr = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetSEStageCnr
//
bool	BayTemplate::GetSEStageCnr() const
{
	return m_bSEStageCnr;
}

//////////////////////////////////////////////////////////////////////
//SetSEStageCnr
//
void	BayTemplate::SetSEStageCnr( bool bExists )
{
	m_bSEStageCnr = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetSWStageCnr
//
bool	BayTemplate::GetSWStageCnr() const
{
	return m_bSWStageCnr;
}

//////////////////////////////////////////////////////////////////////
//SetSWStageCnr
//
void	BayTemplate::SetSWStageCnr( bool bExists )
{
	m_bSWStageCnr = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetNWStageCnr
//
bool	BayTemplate::GetNWStageCnr() const
{
	return m_bNWStageCnr;
}

//////////////////////////////////////////////////////////////////////
//SetNWStageCnr
//
void	BayTemplate::SetNWStageCnr( bool bExists )
{
	m_bNWStageCnr = bExists;
}


//////////////////////////////////////////////////////////////////////
//Bracing
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//GetNBrace
//
bool	BayTemplate::GetNBrace() const
{
	return m_bNBrace;
}

//////////////////////////////////////////////////////////////////////
//SetNBrace
//
void	BayTemplate::SetNBrace( bool bExists )
{
	m_bNBrace = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetEBrace
//
bool	BayTemplate::GetEBrace() const
{
	return m_bEBrace;
}

//////////////////////////////////////////////////////////////////////
//SetEBrace
//
void	BayTemplate::SetEBrace( bool bExists )
{
	m_bEBrace = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetSBrace
//
bool	BayTemplate::GetSBrace() const
{
	return m_bSBrace;
}

//////////////////////////////////////////////////////////////////////
//SetSBrace
//
void	BayTemplate::SetSBrace( bool bExists )
{
	m_bSBrace = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetWBrace
//
bool	BayTemplate::GetWBrace() const
{
	return m_bWBrace;
}

//////////////////////////////////////////////////////////////////////
//SetWBrace
//
void	BayTemplate::SetWBrace( bool bExists )
{
	m_bWBrace = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetBaysPerBrace
//
int		RunTemplate::GetBaysPerBrace() const
{
	return m_iBaysPerBrace;
}

//////////////////////////////////////////////////////////////////////
//SetBaysPerBrace
//
void	RunTemplate::SetBaysPerBrace( int iBaysPerBrace )
{
	m_iBaysPerBrace = iBaysPerBrace;
}

//////////////////////////////////////////////////////////////////////
//GetBraceDist
//
double  BayTemplate::GetBraceDist() const
{
	return m_dBraceDist;
}

//////////////////////////////////////////////////////////////////////
//SetBraceDist
//
void  BayTemplate::SetBraceDist( double dBraceDist )
{
	m_dBraceDist = dBraceDist;
}

//////////////////////////////////////////////////////////////////////
//Ties
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//GetBaysPerTie
//
int		RunTemplate::GetBaysPerTie() const
{
	return m_iBaysPerTie;
}

//////////////////////////////////////////////////////////////////////
//SetBaysPerTie
//
void	RunTemplate::SetBaysPerTie( int iBaysPerTie )
{
	m_iBaysPerTie = iBaysPerTie;
}

///////////////////////////////////////////////////////////////////////
//GetNNETie
//Does this bay have a Tie?
bool	BayTemplate::GetNNETie() const
{
	return m_bNNETie;
}

///////////////////////////////////////////////////////////////////////
//SetNNETie
//Set the Tie state for this bay
void	BayTemplate::SetNNETie( bool bTie )
{
	m_bNNETie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetENETie
//Does this bay have a Tie?
bool	BayTemplate::GetENETie() const
{
	return m_bENETie;
}

///////////////////////////////////////////////////////////////////////
//SetENETie
//Set the Tie state for this bay
void	BayTemplate::SetENETie( bool bTie )
{
	m_bENETie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetESETie
//Does this bay have a Tie?
bool	BayTemplate::GetESETie() const
{
	return m_bESETie;
}

///////////////////////////////////////////////////////////////////////
//SetESETie
//Set the Tie state for this bay
void	BayTemplate::SetESETie( bool bTie )
{
	m_bESETie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetSSETie
//Does this bay have a Tie?
bool	BayTemplate::GetSSETie() const
{
	return m_bSSETie;
}

///////////////////////////////////////////////////////////////////////
//SetSSETie
//Set the Tie state for this bay
void	BayTemplate::SetSSETie( bool bTie )
{
	m_bSSETie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetSSWTie
//Does this bay have a Tie?
bool	BayTemplate::GetSSWTie() const
{
	return m_bSSWTie;
}

///////////////////////////////////////////////////////////////////////
//SetSSWTie
//Set the Tie state for this bay
void	BayTemplate::SetSSWTie( bool bTie )
{
	m_bSSWTie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetWSWTie
//Does this bay have a Tie?
bool	BayTemplate::GetWSWTie() const
{
	return m_bWSWTie;
}

///////////////////////////////////////////////////////////////////////
//SetWSWTie
//Set the Tie state for this bay
void	BayTemplate::SetWSWTie( bool bTie )
{
	m_bWSWTie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetWNWTie
//Does this bay have a Tie?
bool	BayTemplate::GetWNWTie() const
{
	return m_bWNWTie;
}

///////////////////////////////////////////////////////////////////////
//SetWNWTie
//Set the Tie state for this bay
void	BayTemplate::SetWNWTie( bool bTie )
{
	m_bWNWTie = bTie;
}

///////////////////////////////////////////////////////////////////////
//GetNNWTie
//Does this bay have a Tie?
bool	BayTemplate::GetNNWTie() const
{
	return m_bNNWTie;
}

///////////////////////////////////////////////////////////////////////
//SetNNWTie
//Set the Tie state for this bay
void	BayTemplate::SetNNWTie( bool bTie )
{
	m_bNNWTie = bTie;
}


//////////////////////////////////////////////////////////////////////
//HandRails
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//GetNHandRail
//
bool	BayTemplate::GetNHandRail() const
{
	return m_bNHandRail;
}

//////////////////////////////////////////////////////////////////////
//SetNHandRail
//
void	BayTemplate::SetNHandRail( bool bExists )
{
	m_bNHandRail = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetEHandRail
//
bool	BayTemplate::GetEHandRail() const
{
	return m_bEHandRail;
}

//////////////////////////////////////////////////////////////////////
//SetEHandRail
//
void	BayTemplate::SetEHandRail( bool bExists )
{
	m_bEHandRail = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetSHandRail
//
bool	BayTemplate::GetSHandRail() const
{
	return m_bSHandRail;
}

//////////////////////////////////////////////////////////////////////
//SetSHandRail
//
void	BayTemplate::SetSHandRail( bool bExists )
{
	m_bSHandRail = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetWHandRail
//
bool	BayTemplate::GetWHandRail() const
{
	return m_bWHandRail;
}

//////////////////////////////////////////////////////////////////////
//SetWHandRail
//
void	BayTemplate::SetWHandRail( bool bExists )
{
	m_bWHandRail = bExists;
}

//////////////////////////////////////////////////////////////////////
//GetHandRailDist
//
double  BayTemplate::GetHandRailDist() const
{
	return m_dHandRailDist;
}

//////////////////////////////////////////////////////////////////////
//SetHandRailDist
//
void  BayTemplate::SetHandRailDist( double dHandRailDist )
{
	m_dHandRailDist = dHandRailDist;
}


///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void BayTemplate::Serialize(CArchive &ar)
{
	int			iSystem;
	BOOL		BTemp; // for bool conversion macros
	double		dTemp;
	CString		sMsg, sTemp;

	PreviewTemplate::Serialize(ar); // call base class serialize

	if (ar.IsStoring())    // Store Object?
	{
		ar << BAY_TEMPLATE_VERSION_LATEST;

		//BAY_TEMPLATE_VERSION_1_0_3
		iSystem = (int)GetSystem();
		ar << iSystem;

		//BAY_TEMPLATE_VERSION_1_0_2
		ar << m_mtMillsCnrType;

		//BAY_TEMPLATE_VERSION_1_0_1
		ar << m_dTieTubeSeparation;

		m_LooseTieTubes.Serialize(ar);

		//Stages
		ar << m_dOffsetForShortStage;

		//Ties
		STORE_bool_IN_AR(m_bNNETie);
		STORE_bool_IN_AR(m_bENETie);
		STORE_bool_IN_AR(m_bESETie);
		STORE_bool_IN_AR(m_bSSETie);
		STORE_bool_IN_AR(m_bSSWTie);
		STORE_bool_IN_AR(m_bWSWTie);
		STORE_bool_IN_AR(m_bWNWTie);
		STORE_bool_IN_AR(m_bNNWTie);

		//The following belongs to BAY_TEMPLATE_VERSION_1_0_0
		//Dimensions
		ar << m_dLengthOfBays;
		ar << m_dWidthOfBays;
		ar << GetWidthOfTie();
		//Stages
		ar << m_dWidthOfNStage;	//North Stage
		ar << m_dWidthOfEStage;	//East  Stage
		ar << m_dWidthOfSStage;	//South Stage
		ar << m_dWidthOfWStage;	//West  Stage
		//Stage corners
		STORE_bool_IN_AR(m_bNEStageCnr);
		STORE_bool_IN_AR(m_bSEStageCnr);
		STORE_bool_IN_AR(m_bSWStageCnr);
		STORE_bool_IN_AR(m_bNWStageCnr);
		//Bracing
		STORE_bool_IN_AR(m_bNBrace);
		STORE_bool_IN_AR(m_bSBrace);
		STORE_bool_IN_AR(m_bEBrace);
		STORE_bool_IN_AR(m_bWBrace);
		ar << m_dBraceDist;
		//HandRails
		STORE_bool_IN_AR(m_bNHandRail);
		STORE_bool_IN_AR(m_bEHandRail);
		STORE_bool_IN_AR(m_bSHandRail);
		STORE_bool_IN_AR(m_bWHandRail);
		ar << m_dHandRailDist;
	}
	else					// or Load Object?
	{
		//set default values;
		m_dTieTubeSeparation = 4000.00;
		m_LooseTieTubes.DeleteAll();

		//default to the system setting
		if( GetBayPointer()!=NULL && GetBayPointer()->GetController()!=NULL )
		{
			SetSystem( GetBayPointer()->GetController()->GetSystem() );
		}
		else
		{
			//try again
			if( gpController!=NULL )
				SetSystem( gpController->GetSystem() );
			else
				SetSystem( S_KWIKSTAGE );	//forget about it, just use kwikstage
		}

		//Stages
		m_dOffsetForShortStage=0.00;

		//Ties
		m_bNNETie = false;
		m_bENETie = false;
		m_bESETie = false;
		m_bSSETie = false;
		m_bSSWTie = false;
		m_bWSWTie = false;
		m_bWNWTie = false;
		m_bNNWTie = false;

		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case BAY_TEMPLATE_VERSION_1_0_3 :
			ar >> iSystem;
			SetSystem( (SystemEnum)iSystem );
			//fallthrough

		case BAY_TEMPLATE_VERSION_1_0_2 :
			ar >> m_mtMillsCnrType;
			//fallthrough

		case BAY_TEMPLATE_VERSION_1_0_1 :
			ar >> m_dTieTubeSeparation;

			m_LooseTieTubes.Serialize(ar);

			//Stages
			ar >> m_dOffsetForShortStage;

			//Ties
			LOAD_bool_IN_AR(m_bNNETie);
			LOAD_bool_IN_AR(m_bENETie);
			LOAD_bool_IN_AR(m_bESETie);
			LOAD_bool_IN_AR(m_bSSETie);
			LOAD_bool_IN_AR(m_bSSWTie);
			LOAD_bool_IN_AR(m_bWSWTie);
			LOAD_bool_IN_AR(m_bWNWTie);
			LOAD_bool_IN_AR(m_bNNWTie);

			//fallthrough
		case BAY_TEMPLATE_VERSION_1_0_0 :
			//Dimensions
			ar >> m_dLengthOfBays;
			ar >> m_dWidthOfBays;
			ar >> dTemp;
			SetWidthOfTie(dTemp);
			//Stages
			ar >> m_dWidthOfNStage;	//North Stage
			ar >> m_dWidthOfEStage;	//East  Stage
			ar >> m_dWidthOfSStage;	//South Stage
			ar >> m_dWidthOfWStage;	//West  Stage
			//Stage corners
			LOAD_bool_IN_AR(m_bNEStageCnr);
			LOAD_bool_IN_AR(m_bSEStageCnr);
			LOAD_bool_IN_AR(m_bSWStageCnr);
			LOAD_bool_IN_AR(m_bNWStageCnr);
			//Bracing
			LOAD_bool_IN_AR(m_bNBrace);
			LOAD_bool_IN_AR(m_bSBrace);
			LOAD_bool_IN_AR(m_bEBrace);
			LOAD_bool_IN_AR(m_bWBrace);
			ar >> m_dBraceDist;
			//HandRails
			LOAD_bool_IN_AR(m_bNHandRail);
			LOAD_bool_IN_AR(m_bEHandRail);
			LOAD_bool_IN_AR(m_bSHandRail);
			LOAD_bool_IN_AR(m_bWHandRail);
			ar >> m_dHandRailDist;
			break;
		default:
			;//assert( false );
			if( uiVersion>BAY_TEMPLATE_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: BayTemplate.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), BAY_TEMPLATE_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}


///////////////////////////////////// RUN TEMPLATE MEMBER FUNCTIONS BELOW **********

///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void RunTemplate::Serialize(CArchive &ar)
{
	int		iFit, iSize, i;
	CString	sMsg, sTemp;
	Point3D	pt;

	if (ar.IsStoring())    // Store Object?
	{
		ar << RUN_TEMPLATE_VERSION_LATEST;

		//RUN_TEMPLATE_VERSION_1_0_2
		ar << m_dBracingLength2400;
		ar << m_dBracingLength1800;
		ar << m_dBracingLength1200;
		ar << m_dBracingLength0700;

		//RUN_TEMPLATE_VERSION_1_0_1
		ar << m_ptPointStart.x;
		ar << m_ptPointStart.y;
		ar << m_ptPointStart.z;
		ar << m_ptPointEnd.x;
		ar << m_ptPointEnd.y;
		ar << m_ptPointEnd.z;
		ar << (int)m_eTieType;
		ar << m_dTiesVerticallyEvery;
		ar << m_dDistanceFromWall;
		iSize = m_pLapboardBox.length();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			ar << m_pLapboardBox[i].x;
			ar << m_pLapboardBox[i].y;
			ar << m_pLapboardBox[i].z;
		}
		iSize = m_BoundingBox.length();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			ar << m_BoundingBox[i].x;
			ar << m_BoundingBox[i].y;
			ar << m_BoundingBox[i].z;
		}
		//below here is the RUN_TEMPLATE_VERSION_1_0_0
		ar << m_iBaysPerTie;	//Ties
		ar << m_iBaysPerBrace;	//Bracing
		iFit = (int)m_eFit;
		ar << iFit;				//fit
	}
	else					// or Load Object?
	{
		//Set to defaults
		m_ptPointStart.set( 0.00, 0.00, 0.00 );
		m_ptPointEnd.set( 0.00, 0.00, 0.00 );
		m_eTieType = TIE_TYPE_MASONARY;
		m_dTiesVerticallyEvery = 4000.00;
		m_dDistanceFromWall = 225.00;
		m_pLapboardBox.RemoveAll();
		m_BoundingBox.RemoveAll();
		m_iBaysPerTie = 2;
		m_iBaysPerBrace = 4;
		m_eFit = RF_TIGHT;

		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
		case RUN_TEMPLATE_VERSION_1_0_2 :
			ar >> m_dBracingLength2400;
			ar >> m_dBracingLength1800;
			ar >> m_dBracingLength1200;
			ar >> m_dBracingLength0700;
			//fallthrough

		case RUN_TEMPLATE_VERSION_1_0_1 :
			ar >> m_ptPointStart.x;
			ar >> m_ptPointStart.y;
			ar >> m_ptPointStart.z;
			ar >> m_ptPointEnd.x;
			ar >> m_ptPointEnd.y;
			ar >> m_ptPointEnd.z;
			int iTemp;
			ar >> iTemp;
			m_eTieType = (TieTypesEnum)iTemp;
			ar >> m_dTiesVerticallyEvery;
			ar >> m_dDistanceFromWall;
			ar >> iSize;
			m_pLapboardBox.RemoveAll();
			for( i=0; i<iSize; i++ )
			{
				ar >> pt.x;
				ar >> pt.y;
				ar >> pt.z;
				m_pLapboardBox.append(pt);
			}
			ar >> iSize;
			m_BoundingBox.RemoveAll();
			for( i=0; i<iSize; i++ )
			{
				ar >> pt.x;
				ar >> pt.y;
				ar >> pt.z;
				m_BoundingBox.append(pt);
			}
			//fallthrough
		case RUN_TEMPLATE_VERSION_1_0_0 :
			ar >> m_iBaysPerTie;	//Ties
			ar >> m_iBaysPerBrace;	//Bracing
			ar >> iFit;				//fit
			m_eFit = (RunFitEnum)iFit;
			break;
		default:
			;//assert( false );
			if( uiVersion>RUN_TEMPLATE_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: RunTemplate.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), RUN_TEMPLATE_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}




//////////////////////////////////////////////////////////////////////
//GetFit
//
RunFitEnum RunTemplate::GetFit() const
{
	return m_eFit;
}

//////////////////////////////////////////////////////////////////////
//SetFit
//
void RunTemplate::SetFit( RunFitEnum eFit )
{
	m_eFit = eFit;
}


/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: PreviewTemplate.cpp $
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:11a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 6/10/00    Time: 1:29p
 * Updated in $/Meccano/Stage 1/Code
 * About to build R1.1e - This should have all of Mark's current small
 * bugs fixed, ready for the second release to Perth (still awaiting
 * feedback from Perth from the first one we sent them)  This is another
 * version for mark to test, and is a candidate for Perth release
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 27/09/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * This is most likely the version we are sending to Perth for Testing of
 * 3D components
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 26/09/00   Time: 1:50p
 * Updated in $/Meccano/Stage 1/Code
 * Just build 1.1
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 11/08/00   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed crash where we couldn't _topen any previous drawings thanks to
 * AGT's change of fonts
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 13/07/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 16/06/00   Time: 5:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 16/06/00   Time: 3:07p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 2/06/00    Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 31/05/00   Time: 9:56a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 20/04/00   Time: 4:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 17/04/00   Time: 4:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 13/03/00   Time: 5:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 11/02/00   Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC2
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 11/02/00   Time: 9:12a
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the save and load bug in the stages & levels stuff
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 28/01/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 12/01/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12 (Beta13)
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 4/01/00    Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 14/12/99   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to build Beta11
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 8/12/99    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * Over and Underruns working correctly
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 12/11/99   Time: 3:47p
 * Updated in $/Meccano/Stage 1/Code
 * Changing the Bay Avoidance mechanism
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 11/11/99   Time: 3:56p
 * Updated in $/Meccano/Stage 1/Code
 * Just completed:
 * 491 - Inertbay needs better error msg
 * 408 - Can only inert one bay at a timeinsert
 * 467 - Inert bay north has offset
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/11/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * 445 - Moved matrix not stored in saved file
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/11/99    Time: 11:27a
 * Updated in $/Meccano/Stage 1/Code
 * Lapboard point to point now operational
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 27/10/99   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 27/10/99   Time: 2:49p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Mesh Guard now working
 * 2) Ties nearly working
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Dar          Date: 27/10/99   Time: 9:11a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 26/10/99   Time: 3:17p
 * Updated in $/Meccano/Stage 1/Code
 * Added the ties to the schematic
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 26/10/99   Time: 7:53a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 19/10/99   Time: 3:10p
 * Updated in $/Meccano/Stage 1/Code
 * Birdcaging is almost working correctly now
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 19/10/99   Time: 9:03a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on Birdcaging
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 12/10/99   Time: 9:32a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Level Assignment now operational - but have to call _T("RegenLevels")
 * 2) Fixed the Schematic View Serialize error.
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 7/10/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 6/10/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * Schematic Corner Stage boards are now correct!
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 5/10/99    Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 29/09/99   Time: 2:39p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards object can now do shematic view, but it has no components
 * (thus no 3DView)
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 29/09/99   Time: 12:33p
 * Updated in $/Meccano/Stage 1/Code
 * Simple Curve avoidance functionality now operational
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 27/09/99   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * Working on Lapboards
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 13/09/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 9/09/99    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Dar          Date: 9/07/99    Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * serialize for bay, preview and run templates done
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 *
 *******************************************************************************/




Bay * PreviewTemplate::GetBayPointer()
{
	return m_pBay;
}

void PreviewTemplate::SetBayPointer(Bay *pBay)
{
	m_pBay = pBay;
}

Bay * LapBoardTemplate::GetFirstBayInRun()
{
	return m_pFirstBayInRun;
}

Bay * LapBoardTemplate::GetLastBayInRun()
{
	return m_pLastBayInRun;
}

void LapBoardTemplate::SetFirstBayInRun(Bay *pBay)
{
	m_pFirstBayInRun = pBay;
}

void LapBoardTemplate::SetLastBayInRun(Bay *pBay)
{
	m_pLastBayInRun = pBay;
}

void PreviewTemplate::SetSchematicText(CString sText, AcDbObjectId id)
{
	Entity				*pEnt;
	Acad::ErrorStatus	es;

	GetBayPointer()->GetController()->IgnorMovement(true);
	if( !id.isNull() && 
		(acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, id, AcDb::kForWrite )==Acad::eOk) )
	{
		es = ((AcDbText*)pEnt)->setTextString( sText );
		;//assert( es==Acad::eOk );
		pEnt->close();
	}
	GetBayPointer()->GetController()->IgnorMovement(false);
}

void PreviewTemplate::UpdateSchematicView()
{
	CreateSchematic( );
	MoveSchematic( GetSchematicTransform(), false );
	SetAllSchematicText();
}

void PreviewTemplate::CreateSchematic()
{
	int					i, *piGroupNumber;
	CString				sGroupName;
	Entity				Temp;
	TCHAR				cGroupName[30];

	/////////////////////////////////////////////////////////////////////
	//Delete the existing Schematic
	DeleteSchematic();

	/////////////////////////////////////////////////////////////////////
	//create the points using the bay structure
	CreateSchematicPoints();

	/////////////////////////////////////////////////////////////////////
	//clear the m_objIds
	ClearObjectIds();

	/////////////////////////////////////////////////////////////////////
	//create the bay number text
	CreateSchematicTextEntities( m_objIds );

	/////////////////////////////////////////////////////////////////////
	//convert the points into entities 
	DrawLines( m_objIds );

	/////////////////////////////////////////////////////////////////////
	//create the group name
	;//assert( GetBayPointer()->GetController()!=NULL );
	piGroupNumber = GetBayPointer()->GetController()->GetGroupNumber();
	(*piGroupNumber)++;
	CString sType;
	switch( GetBayPointer()->GetBayType() )
	{
	case( BAY_TYPE_BAY ):
		sType.Format( _T("_Bay%03i"), GetBayPointer()->GetBayNumber() );
		break;
	case( BAY_TYPE_LAPBOARD ):
		sType.Format( _T("_Lapboard%02i"), GetBayPointer()->GetID()+1 );
		break;
	case( BAY_TYPE_STAIRS ):
		sType.Format( _T("_Stair%02i"), GetBayPointer()->GetID()+1 );
		break;
	case( BAY_TYPE_LADDER ):
		sType.Format( _T("_Ladder%02i"), GetBayPointer()->GetID()+1 );
		break;
	case( BAY_TYPE_BUTTRESS ):
		sType.Format( _T("_Buttress%02i"), GetBayPointer()->GetID()+1 );
		break;
	default:
		;//assert( false );
		sType.Empty();
	}

	sGroupName.Format( _T("%s%03i%s"), GROUP_PREFIX_SCHEMATIC, *piGroupNumber, sType );
	for( i=0; i<sGroupName.GetLength(); i++ )
	{
		if( i>30 ) break;
		cGroupName[i] = sGroupName[i];
	}
	cGroupName[i] = _T('\0');

	/////////////////////////////////////////////////////////////////////
	//create the group
	SetSchematicGroup( Temp.CreateGroup( m_objIds, cGroupName ) );
}

Matrix3D PreviewTemplate::UnMoveSchematic()
{
	Matrix3D	Original, Inverse;

	//store the original transformation for later use
	Original = GetSchematicTransform();

	//we need to move the Bay back to its original position
	Inverse = GetSchematicTransform();
	Inverse.invert();

	MoveSchematic( Inverse, true );

	//return the original matrix
	return Original;
}

Matrix3D PreviewTemplate::GetSchematicTransform() const
{
	return m_SchematicTransform;
}

void PreviewTemplate::SetSchematicTransform(Matrix3D Transform)
{
	m_SchematicTransform = Transform;
}

void PreviewTemplate::MoveSchematic(Matrix3D Transform, bool bStore)
{
	int					i, iSize;
	Entity				*pEnt;
	Acad::ErrorStatus	es;
	AcDbObjectIdArray	ids;

	//store it
	if( bStore )
	{
		SetSchematicTransform ( Transform * GetSchematicTransform() );
	}

	if( GetSchematicGroup()==NULL )
		return;

	iSize = GetSchematicGroup()->allEntityIds( ids );
	GetBayPointer()->GetController()->IgnorMovement(true);
	for( i=0; i<iSize; i++ )
	{
		es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, ids[i], AcDb::kForWrite );

		if( es==Acad::eOk )
		{
			es = pEnt->transformBy( Transform );
			;//assert( es==Acad::eOk );
			es = pEnt->close();
			;//assert( es==Acad::eOk );
		}
		else
		{
			;//assert( false );
		}
	}
	GetBayPointer()->GetController()->IgnorMovement(false);

	//I have removed this so that when rotating muliple objects, it does
	//	not clear the list every time it does a visit see bug# 1553
	//GetBayPointer()->GetController()->ClearAllEntityTypes();
}

void PreviewTemplate::DeleteSchematic()
{
	if( gpController->IsDestructingController() )
		return;

	int					i, iSize;
	void				*pSomething;
	Entity				*pEnt;
	CString				sGroupName;
	AcDbGroup			*pGroup;
	AcDbObjectId		ReactorID, ID;
	EntityReactor		*pReactor;
	AcDbVoidPtrArray	*pReactors;
	Acad::ErrorStatus	es, groupes;
	AcDbObjectIdArray	ids;
	AcDbDictionary		*pGroupDict;

	pGroupDict = NULL;
	try
	{
		if( GetSchematicGroup()==NULL )
			return;

		/////////////////////////////////////////////////////////////////////
		//Delete the Entities and Reactors
		pEnt = NULL;
		pGroup = NULL;
		pReactor = NULL;
		pReactors = NULL;
		pSomething = NULL;
		es = acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pGroupDict, AcDb::kForWrite);
		;//assert( es==Acad::eOk );
		groupes = acdbOpenAcDbObject( (AcDbObject*&)pGroup, GetSchematicGroup()->objectId(), AcDb::kForWrite );
		if( (groupes==Acad::eOk) && (pGroup!=NULL) )
		{
			iSize = pGroup->allEntityIds( ids );

#ifdef _DEBUG
//			acutPrintf("\nGroups: %i", (int)pGroupDict->numEntries() );
#endif	//#ifdef _DEBUG

			/////////////////////////////////////////////////////////////////////
			//Delete the group
			;//assert( pGroup->isWriteEnabled() );

			if( pGroup->isTransactionResident() )
			{
				if( pReactor!=NULL )
				{
					es = pReactor->close();
					;//assert( es==Acad::eOk );
				}
				if( pGroup!=NULL )
				{
					es = pGroup->close();
					;//assert( es==Acad::eOk );
				}
				if( pEnt!=NULL )
				{
					es = pEnt->close();
					;//assert( es==Acad::eOk );
				}
				if( pGroupDict!=NULL )
				{
					es = pGroupDict->close();
					;//assert( es==Acad::eOk );
				}
				return;
			}

			sGroupName = pGroup->name();
			;//assert( pGroupDict->has(sGroupName) );
			if( pGroup->isErased()==Adesk::kFalse && !pGroup->isNotAccessible() )
			{
				for( i=0; i<iSize; i++ )
				{
					ID = ids[i];
					if( ID.isValid() )
					{
						es = pGroup->remove(ID);
						;//assert( es==Acad::eOk );

						pEnt = NULL;
						es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, ID, AcDb::kForWrite );

						if( (pEnt!=NULL) && (es==Acad::eOk) )
						{
							pReactors	= (AcDbVoidPtrArray*) pEnt->reactors();

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
											pReactor=NULL;
											es = acdbOpenAcDbObject((AcDbObject*&)pReactor, ReactorID, AcDb::kForWrite);

											if(es==Acad::eOk || es==Acad::eWasOpenForWrite )
											{
												es = pReactor->erase();
												;//assert( es==Acad::eOk);
		//										gED.DeleteEntityInfo( ReactorID );
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
							gED.DeleteEntityInfo( ID );
							es = pEnt->close();
							;//assert( es==Acad::eOk);
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
						pEnt = NULL;
					}
				}
				es = pGroup->erase();
				;//assert( es==Acad::eOk );
				es = pGroup->close();
				;//assert( es==Acad::eOk );
				pGroup = NULL;
			}

			;//assert( !pGroupDict->has(sGroupName) );
			es = pGroupDict->close();
			;//assert( es==Acad::eOk );

//			gED.DeleteEntityInfo( GetSchematicGroup()->objectId() );

		}
		else if( groupes==Acad::eWasErased )
		{
			//fine
			1;
		}
		else if( pGroup==NULL )
		{
			//We must be doing an initial clean up, DO NOTHING
			1;
		}
		else
		{
			gED.PrintRemaining();
			;//assert( false );
		}

		SetSchematicGroup(NULL, false );

	}
	catch(...)
	{
		;//assert( false );
	}

	//close all
	if( pReactor!=NULL )
	{
		es = pReactor->close();
		;//assert( es==Acad::eOk );
	}
	if( pGroup!=NULL )
	{
		es = pGroup->close();
		;//assert( es==Acad::eOk );
	}
	if( pEnt!=NULL )
	{
		es = pEnt->close();
		;//assert( es==Acad::eOk );
	}
	if( pGroupDict!=NULL )
	{
		es = pGroupDict->close();
		;//assert( es==Acad::eOk );
	}
}

void PreviewTemplate::SetAllSchematicText()
{
	;//assert( false );
}

AcDbGroup * PreviewTemplate::GetSchematicGroup()
{
	return m_pSchematicGroup;
}

void PreviewTemplate::SetSchematicGroup(AcDbGroup *pGroup, bool bShowWarning/*=true*/)
{
	if( bShowWarning && m_pSchematicGroup!=NULL && pGroup==NULL )
	{
		;//assert( false );	//we didn't delete the existing one!
	}
	m_pSchematicGroup = pGroup;
}

void PreviewTemplate::CreateSchematicPoints()
{
	;//assert( false );
}

void BayTemplate::CreateSchematicTextEntities(AcDbObjectIdArray &objIds)
{
	CString sText;

	m_objIdBackwardMatrix.setNull();
	m_objIdForwardMatrix.setNull();
	m_objIdBayLength.setNull();
	m_objIdBayNumber.setNull();
	while( m_objIdsTieTubes.length()>0 )
	{
		m_objIdsTieTubes[0].setNull();
		m_objIdsTieTubes.removeAt(0);
	}

	//////////////////////////////////////////////////////////////////
	//Bay Number Label
	sText.Format( _T("%i"), GetBayPointer()->GetBayNumber() );
	m_objIdBayNumber = AddTextToSchematic( sText, TP_BAY_NUMBER );

	//////////////////////////////////////////////////////////////////
	//set the bay number to a ? mark
	double dLength = GetBayLength();
	if( dLength<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		dLength>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		dLength = COMPONENT_LENGTH_0800;

	sText.Format( _T("%1.1fm"), dLength*CONVERT_MM_TO_M );
	m_objIdBayLength = AddTextToSchematic( sText, TP_BAY_LENGTH );
	
	//////////////////////////////////////////////////////////////////
	//Forward Matrix Label
	sText.Empty();
	MatrixElement *pElement;
	pElement = GetBayPointer()->GetMatrixElementPointer();
	;//assert( gpController!=NULL );
	if( pElement!=NULL && gpController->ShowMatrixElement( pElement->GetMatrixElementID() ) &&
		gpController->GetShowMatrixLabelsOnSchematic() )
		sText = pElement->GetLabel(); 
	m_objIdForwardMatrix = AddTextToSchematic( sText, TP_MATRIX_FORWARD );

	//////////////////////////////////////////////////////////////////
	//Backward Matrix Label
	sText.Empty();
	pElement = GetBayPointer()->GetMatrixElementPointerAlt();
	;//assert( gpController!=NULL );
	if( pElement!=NULL && gpController->ShowMatrixElement( pElement->GetMatrixElementID() ) &&
		gpController->GetShowMatrixLabelsOnSchematic() )
		sText = pElement->GetLabel(); 
	m_objIdBackwardMatrix = AddTextToSchematic( sText, TP_MATRIX_BACKWARD );

	;//assert( !m_objIdBayNumber.isNull() );
	;//assert( !m_objIdBayLength.isNull() );
	;//assert( !m_objIdForwardMatrix.isNull() );
	;//assert( !m_objIdBackwardMatrix.isNull() );

	objIds.append( m_objIdBayNumber );
	objIds.append( m_objIdBayLength );
	objIds.append( m_objIdForwardMatrix );
	objIds.append( m_objIdBackwardMatrix );


	////////////////////////////////////////////////////
	//Tie Tube Labels
	Entity				*pEnt;
	AcDbObjectId		objIdTieTube;
	BayTieTubeTemplate	*pTTT;
	Matrix3D			Translation, Rotation, Trans;
	Vector3D			Vector;
	Acad::ErrorStatus	es;
	for( int i=0; i<m_LooseTieTubes.GetSize(); i++ )
	{
		pTTT = m_LooseTieTubes.GetAt(i);
		;//assert( pTTT!=NULL );
		sText.Format( _T("%0.1fm"), pTTT->m_dFullLength*CONVERT_MM_TO_M );
		objIdTieTube = AddTextToSchematic( sText, TP_TIE_TUBE_LENGTH );
		;//assert( !objIdTieTube.isNull() );
		m_objIdsTieTubes.append( objIdTieTube );
		objIds.append( objIdTieTube );

		//this one still has some translations to perform
		es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, objIdTieTube, AcDb::kForWrite );
		if( es==Acad::eOk )
		{
			//position on the tube
			Vector.set( (pTTT->m_dFullLength/2.00), 0.00, 0.00 ); 
			Trans.setToTranslation( Vector );

			//rotate to angle of tube
			Vector.set( 0.00, 0.00, 1.00 ); 
			Rotation.setToRotation( pTTT->m_dAngle, Vector );
			Trans = Rotation * Trans;

			//movement within bay
			switch( pTTT->m_eStandard )
			{
			case( CNR_NORTH_EAST ):
				Vector.set( GetBayLength(), GetBayWidth(), 0.00 );
				break;
			case( CNR_SOUTH_EAST ):
				Vector.set( GetBayLength(), 0.00, 0.00 );
				break;
			case( CNR_NORTH_WEST ):
				Vector.set( 0.00, GetBayWidth(), 0.00 );
				break;
			case( CNR_SOUTH_WEST ):
				Vector.set( 0.00, 0.00, 0.00 );
				break;
			default:
				;//assert( false );
				break;
			}
			Translation.setToTranslation( Vector );
			Trans = Translation * Trans;

			//move
			pEnt->transformBy( Trans );
			pEnt->close();
		}
		
	}
}

AcDbObjectId BayTemplate::AddTextToSchematic(CString &sText, TextPositionEnum eTextPosition)
{
	double				dTextHeight, dBayWidth, dBayLength;
	Entity				*pText;
	Point3D				ptOrigin;
	Vector3D			Vector;
	Matrix3D			Translation;
	AcDbObjectId		recordId, Id;
	EntityReactor		*pReactor;
	Acad::ErrorStatus	es;
	AcDbTextStyleTable	*pTable;

	Id.setNull();

	dBayWidth = GetBayWidthActual();
	dBayLength = GetBayLengthActual();

	es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pTable, AcDb::kForRead);
	;//assert( es==Acad::eOk );
	es = pTable->getAt( TEXT_STYLE_SCHEMATIC, recordId );
	if( es==Acad::eKeyNotFound )
	{
		es = pTable->getAt( TEXT_STYLE_STANDARD, recordId );
	}
	if( es==Acad::eOk )
	{
		dTextHeight = GetBayPointer()->GetCompDetails()->GetActualHeight( GetBayPointer()->GetSystem(), CT_TEXT, 0, MT_STEEL );

		if( eTextPosition==TP_TIE_TUBE_LENGTH )
			dTextHeight*= (3.00/4.00);


		//postion the text at the origin
		ptOrigin.set( 0.00, 0.00, 0.00 );
		pText = (Entity*)((AcDbEntity*) new AcDbText( ptOrigin, sText, recordId, dTextHeight ) );

		//position the text in the middle of the bay, ie half way across the length
		//	of the bay, and halfway across the width of the bay, plus the width of
		//	the stageboard!
		switch(eTextPosition)
		{
		case( TP_BAY_NUMBER ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);
			;//assert( es==Acad::eOk );
			Vector.set( dBayLength/2.00, ((dBayWidth+(2.00*dTextHeight))/2.00), 0.00 );
			break;
		case( TP_BAY_LENGTH ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);
			;//assert( es==Acad::eOk );
			Vector.set( dBayLength/2.00, ((dBayWidth-(2.00*dTextHeight))/2.00), 0.00 );
			break;
		case( TP_MATRIX_FORWARD ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);
			;//assert( es==Acad::eOk );
			Vector.set( dBayLength, (dBayWidth/2.00), 0.00 );
			break;
		case( TP_MATRIX_BACKWARD ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);
			;//assert( es==Acad::eOk );
			Vector.set( 0.00, (dBayWidth/2.00), 0.00 );
			break;
		case( TP_TIE_TUBE_LENGTH ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextBase);
			;//assert( es==Acad::eOk );
			Vector.set( 0.00, 0.00, 0.00 );
			break;
		default:
			;//assert( false );
		}

		//perform the transformation
		Translation.setTranslation(Vector);
		es = pText->transformBy( Translation );
		;//assert( es==Acad::eOk );
		pText->postToDatabase( LAYER_NAME_SCHEMATIC );

    ///////////////////////////////////////////////////////////////////
    //XData
    SetXData( pText, _T("Bay Text") );

		//Reactor stuff
		pText->AttachedReactors( &pReactor );
		es = pText->open();
		;//assert( es==Acad::eOk );
		pReactor->SetBayPointer(GetBayPointer());
		es = pText->close();
		;//assert( es==Acad::eOk );

		//we can now store the id
		Id = pText->objectId();

		CString sTemp;
    sTemp = _T("Bay Template - Text: ");
		sTemp+= sText;
		gED.AddEntityInfo( Id, sTemp );

	}
	es = pTable->close();
	;//assert( es==Acad::eOk );
	return Id;
}


void BayTemplate::SetAllSchematicText()
{
	CString				sText;

	////////////////////////////////////////////////////
	//Bay Number Label
	sText.Format( _T("%i"), GetBayPointer()->GetBayNumber() );
	if( m_objIdBayNumber.isNull() )
	{
		;//assert(false);
	}
	else
	{
		SetSchematicText( sText, m_objIdBayNumber );
	}

	////////////////////////////////////////////////////
	//bay length label
	double dLength = GetBayLength();
	if( dLength<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		dLength>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		dLength = COMPONENT_LENGTH_0800;

	sText.Format( _T("%1.1fm"), GetBayLength()*CONVERT_MM_TO_M );
	if( m_objIdBayNumber.isNull() )
	{
		;//assert(false);
	}
	else
	{
		SetSchematicText( sText, m_objIdBayLength );
	}

	////////////////////////////////////////////////////
	//Forward Matrix Label
	sText = _T(" ");
	MatrixElement *pElement;
	pElement = GetBayPointer()->GetMatrixElementPointer();
	;//assert( gpController!=NULL );
	if( pElement!=NULL && gpController->ShowMatrixElement( pElement->GetMatrixElementID() ) &&
		gpController->GetShowMatrixLabelsOnSchematic() )
		sText = pElement->GetLabel(); 
	if( m_objIdForwardMatrix.isNull() )
	{
		;//assert(false);
	}
	else
	{
		SetSchematicText( sText, m_objIdForwardMatrix );
	}

	////////////////////////////////////////////////////
	//backward matrix label
	sText = _T(" ");
	pElement = GetBayPointer()->GetMatrixElementPointerAlt();
	;//assert( gpController!=NULL );
	if( pElement!=NULL && gpController->ShowMatrixElement( pElement->GetMatrixElementID() ) &&
		gpController->GetShowMatrixLabelsOnSchematic() )
		sText = pElement->GetLabel(); 
	if( m_objIdBackwardMatrix.isNull() )
	{
		;//assert(false);
	}
	else
	{
		SetSchematicText( sText, m_objIdBackwardMatrix );
	}

	////////////////////////////////////////////////////
	//Tie Tube Labels
	AcDbObjectId objIdTieTube;
	BayTieTubeTemplate	*pTTT;
	;//assert( m_objIdsTieTubes.length()==m_LooseTieTubes.GetSize() );
	for( int i=0; i<m_LooseTieTubes.GetSize(); i++ )
	{
		objIdTieTube = m_objIdsTieTubes[i];
		;//assert( !objIdTieTube.isNull() );

		pTTT = m_LooseTieTubes.GetAt(i);
		;//assert( pTTT!=NULL );
		sText.Format( _T("%0.1fm"), pTTT->m_dFullLength*CONVERT_MM_TO_M );
		SetSchematicText( sText, objIdTieTube );
	}
}

void BayTemplate::SetSchematicGroup(AcDbGroup *pGroup)
{
	PreviewTemplate::SetSchematicGroup(pGroup);

	if( pGroup == NULL )
	{
		m_objIdBackwardMatrix.setNull();
		m_objIdForwardMatrix.setNull();
		m_objIdBayLength.setNull();
		m_objIdBayNumber.setNull();
		while( m_objIdsTieTubes.length()>0 )
		{
			m_objIdsTieTubes[0].setNull();
			m_objIdsTieTubes.removeAt(0);
		}
	}
}

void BayTemplate::CreateSchematicPoints()
{
	Component	*pComponent;
	Lift		*pSchematicLift;

	if( GetBayPointer()->GetNumberOfLifts()<=0 )
	{
		;//assert( false );
		return;
	}

	pSchematicLift = GetBayPointer()->GetLift( GetBayPointer()->GetLowestHopupLiftID() );
	;//assert( pSchematicLift!=NULL );

	////////////////////////////////////////////////////////////////////
	//dimensions
//	SetBayWidth( GetBayWidthActual() );
//	SetBayLength( GetBayLengthActual() );

	////////////////////////////////////////////////////////////////////
	//Stages
	SetWidthOfNStage( max( CalcLengthOfHopup( NORTH,	NORTH_NORTH_EAST ),	CalcLengthOfHopup( NORTH,	NORTH_NORTH_EAST	) ) );
	SetWidthOfEStage( max( CalcLengthOfHopup( EAST,		EAST_NORTH_EAST ),	CalcLengthOfHopup( EAST,	EAST_NORTH_EAST		) ) );
	SetWidthOfSStage( max( CalcLengthOfHopup( SOUTH,	SOUTH_SOUTH_EAST ),	CalcLengthOfHopup( SOUTH,	SOUTH_SOUTH_WEST	) ) );
	SetWidthOfWStage( max( CalcLengthOfHopup( WEST,		WEST_NORTH_WEST ),	CalcLengthOfHopup( WEST,	WEST_NORTH_WEST		) ) );

	////////////////////////////////////////////////////////////////////
	//Stage corners
	SetNEStageCnr( pSchematicLift->IsThereACornerStageBoard(NORTH_EAST) );
	SetSEStageCnr( pSchematicLift->IsThereACornerStageBoard(SOUTH_EAST) );
	SetSWStageCnr( pSchematicLift->IsThereACornerStageBoard(SOUTH_WEST) );
	SetNWStageCnr( pSchematicLift->IsThereACornerStageBoard(NORTH_WEST) );

	////////////////////////////////////////////////////////////////////
	//Bracing
	SetNBrace( GetBayPointer()->DoesABayComponentExistOnASide( CT_BRACING, NORTH ) );
	SetEBrace( GetBayPointer()->DoesABayComponentExistOnASide( CT_BRACING, EAST ) );
	SetSBrace( GetBayPointer()->DoesABayComponentExistOnASide( CT_BRACING, SOUTH ) );
	SetWBrace( GetBayPointer()->DoesABayComponentExistOnASide( CT_BRACING, WEST ) );
//	SetBaysPerBrace( int iBaysPerBrace );
//	SetBraceDist( double dBraceDist );
/*
	////////////////////////////////////////////////////////////////////
	//Ties
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, NNE ) )
		SetNNETie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, ENE ) )
		SetENETie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, ESE ) )
		SetESETie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, SSE ) )
		SetSSETie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, SSW ) )
		SetSSWTie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, WSW ) )
		SetWSWTie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, WNW ) )
		SetWNWTie( true );
	if( GetBayPointer()->DoesABayComponentExistOnASide( CT_TIE_TUBE, NNW ) )
		SetNNWTie( true );
*/
	////////////////////////////////////////////////////////////////////
	//HandRails
	pComponent = pSchematicLift->GetComponent(CT_RAIL, LIFT_RISE_1000MM, NORTH );
	SetNHandRail( pComponent!=NULL );

	pComponent = pSchematicLift->GetComponent(CT_RAIL, LIFT_RISE_1000MM, EAST );
	SetEHandRail( pComponent!=NULL );

	pComponent = pSchematicLift->GetComponent(CT_RAIL, LIFT_RISE_1000MM, SOUTH );
	SetSHandRail( pComponent!=NULL );

	pComponent = pSchematicLift->GetComponent(CT_RAIL, LIFT_RISE_1000MM, WEST );
	SetWHandRail( pComponent!=NULL );

//	SetHandRailDist( double dHandRailDist );

	////////////////////////////////////////////////////////////////////
	//Create the points
	CreatePoints();
}

void PreviewTemplate::DrawLines(AcDbObjectIdArray &objIds)
{
	objIds;
	;//assert( false );
}

void BayTemplate::DrawLines(AcDbObjectIdArray &objIds)
{
	double	dLength;
	int		iColour;
	bool	bStage=true;
	bool	bBrace=true;
	bool	bTie=true;

	dLength = GetBayLength();

	iColour = GetBayColour( dLength );

	///////////////////////////////////////////////////////////////////
	//Transom & ledgers stages
	int iType;
	iType = GetMillsSystemType();
	if( iType!=MILLS_TYPE_NONE )
	{
		/////////////////////////////////////
		//North
		if( (iType&MILLS_TYPE_CONNECT_NNE)>0 || (iType&MILLS_TYPE_CONNECT_NNW)>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_NNE)>0 && (iType&MILLS_TYPE_CONNECT_NNW)==0)
			{
				DrawLine( m_aPoints[BP_NW_STD],		m_aPoints[BP_N_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
			else if( (iType&MILLS_TYPE_CONNECT_NNW)>0 && (iType&MILLS_TYPE_CONNECT_NNE)==0 )
			{
				DrawLine( m_aPoints[BP_NE_STD],		m_aPoints[BP_N_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
		}
		else
		{
			if( (iType&MILLS_TYPE_CONNECT_N)==0 )
				DrawLine( m_aPoints[BP_NW_STD],	m_aPoints[BP_NE_STD], iColour, objIds, _T("Northern Ledger") );
		}

		/////////////////////////////////////
		//East
		if( (iType&MILLS_TYPE_CONNECT_ENE)>0 || (iType&MILLS_TYPE_CONNECT_ESE)>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_ENE)>0 && (iType&MILLS_TYPE_CONNECT_ESE)==0 )
			{
				DrawLine( m_aPoints[BP_SE_STD],		m_aPoints[BP_E_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
			else if( (iType&MILLS_TYPE_CONNECT_ESE)>0 && (iType&MILLS_TYPE_CONNECT_ENE)==0 )
			{
				DrawLine( m_aPoints[BP_NE_STD],		m_aPoints[BP_E_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
		}
		else
		{
			if( (iType&MILLS_TYPE_CONNECT_E)==0 )
				DrawLine( m_aPoints[BP_NE_STD],	m_aPoints[BP_SE_STD], iColour, objIds, _T("Northern Ledger") );
		}

		/////////////////////////////////////
		//South
		if( (iType&MILLS_TYPE_CONNECT_SSE)>0 || (iType&MILLS_TYPE_CONNECT_SSW)>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_SSE)>0 && (iType&MILLS_TYPE_CONNECT_SSW)==0)
			{
				DrawLine( m_aPoints[BP_SW_STD],		m_aPoints[BP_S_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
			else if( (iType&MILLS_TYPE_CONNECT_SSW)>0 && (iType&MILLS_TYPE_CONNECT_SSE)==0 )
			{
				DrawLine( m_aPoints[BP_SE_STD],		m_aPoints[BP_S_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
		}
		else
		{
			if( (iType&MILLS_TYPE_CONNECT_S)==0 )
				DrawLine( m_aPoints[BP_SW_STD],	m_aPoints[BP_SE_STD], iColour, objIds, _T("Northern Ledger") );
		}

		/////////////////////////////////////
		//West
		if( (iType&MILLS_TYPE_CONNECT_WNW)>0 || (iType&MILLS_TYPE_CONNECT_WSW)>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_WNW)>0 && (iType&MILLS_TYPE_CONNECT_WSW)==0 )
			{
				DrawLine( m_aPoints[BP_SW_STD],		m_aPoints[BP_W_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
			else if( (iType&MILLS_TYPE_CONNECT_WSW)>0 && (iType&MILLS_TYPE_CONNECT_WNW)==0 )
			{
				DrawLine( m_aPoints[BP_NW_STD],		m_aPoints[BP_W_MILLS], iColour, objIds, _T("Northern Ledger") );
			}
		}
		else
		{
			if( (iType&MILLS_TYPE_CONNECT_W)==0 )
				DrawLine( m_aPoints[BP_NW_STD],	m_aPoints[BP_SW_STD], iColour, objIds, _T("Northern Ledger") );
		}
	}
	else
	{
		DrawLine( m_aPoints[BP_NW_STD],	m_aPoints[BP_NE_STD], iColour, objIds, _T("Northern Ledger") );
		DrawLine( m_aPoints[BP_SE_STD],	m_aPoints[BP_NE_STD], iColour, objIds, _T("Eastern Transom") );
		if( GetBayPointer()->GetInner()==NULL )
			DrawLine( m_aPoints[BP_SW_STD],	m_aPoints[BP_SE_STD], iColour, objIds, _T("Southern Ledger") );
		if( GetBayPointer()->GetBackward()==NULL )
			DrawLine( m_aPoints[BP_SW_STD],	m_aPoints[BP_NW_STD], iColour, objIds, _T("Western Transom") );
	}

	///////////////////////////////////////////////////////////////////
	//All stages
	if( bStage )
	{
		///////////////////////////////////////////////////////////////////
		//Normal stages
		if( GetWidthOfNStage()>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_NNE)>0 || (iType&MILLS_TYPE_CONNECT_NNW)>0 )
			{
				if( (iType&MILLS_TYPE_CONNECT_NNE)>0 && (iType&MILLS_TYPE_CONNECT_NNW)==0)
				{
					DrawLine(	m_aPoints[BP_NW_STD],	m_aPoints[BP_NNW_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine( 	m_aPoints[BP_NNW_STG],	m_aPoints[BP_N_M_STG],	iColour, objIds, _T("Northern Stage") );
				}
				else if( (iType&MILLS_TYPE_CONNECT_NNW)>0 && (iType&MILLS_TYPE_CONNECT_NNE)==0 )
				{
					DrawLine(	m_aPoints[BP_N_M_STG],	m_aPoints[BP_NNE_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_NNE_STG],	m_aPoints[BP_NE_STD],	iColour, objIds, _T("Northern Stage") );
				}
			}
			else
			{
				if( (iType&MILLS_TYPE_CONNECT_N)==0 )
				{
					DrawLine(	m_aPoints[BP_NW_STD],	m_aPoints[BP_NNW_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_NNW_STG],	m_aPoints[BP_NNE_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_NNE_STG],	m_aPoints[BP_NE_STD],	iColour, objIds, _T("Northern Stage") );
				}
			}
		}
		if( GetWidthOfEStage()>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_ENE)>0 || (iType&MILLS_TYPE_CONNECT_ESE)>0 )
			{
				if( (iType&MILLS_TYPE_CONNECT_ENE)>0 && (iType&MILLS_TYPE_CONNECT_ESE)==0)
				{
					DrawLine(	m_aPoints[BP_SE_STD],	m_aPoints[BP_ESE_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine( 	m_aPoints[BP_ESE_STG],	m_aPoints[BP_E_M_STG],	iColour, objIds, _T("Northern Stage") );
				}
				else if( (iType&MILLS_TYPE_CONNECT_ESE)>0 && (iType&MILLS_TYPE_CONNECT_ENE)==0 )
				{
					DrawLine(	m_aPoints[BP_E_M_STG],	m_aPoints[BP_ENE_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_ENE_STG],	m_aPoints[BP_NE_STD],	iColour, objIds, _T("Northern Stage") );
				}
			}
			else
			{
				if( (iType&MILLS_TYPE_CONNECT_E)==0 )
				{
					DrawLine(	m_aPoints[BP_NE_STD],	m_aPoints[BP_ENE_STG],	iColour, objIds, _T("Eastern Stage") );
					DrawLine(	m_aPoints[BP_ENE_STG],	m_aPoints[BP_ESE_STG],	iColour, objIds, _T("Eastern Stage") );
					DrawLine(	m_aPoints[BP_ESE_STG],	m_aPoints[BP_SE_STD],	iColour, objIds, _T("Eastern Stage") );
				}
			}
		}
		if( GetWidthOfSStage()>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_SSE)>0 || (iType&MILLS_TYPE_CONNECT_SSW)>0 )
			{
				if( (iType&MILLS_TYPE_CONNECT_SSE)>0 && (iType&MILLS_TYPE_CONNECT_SSW)==0)
				{
					DrawLine(	m_aPoints[BP_SW_STD],	m_aPoints[BP_SSW_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine( 	m_aPoints[BP_SSW_STG],	m_aPoints[BP_S_M_STG],	iColour, objIds, _T("Northern Stage") );
				}
				else if( (iType&MILLS_TYPE_CONNECT_SSW)>0 && (iType&MILLS_TYPE_CONNECT_SSE)==0 )
				{
					DrawLine(	m_aPoints[BP_S_M_STG],	m_aPoints[BP_SSE_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_SSE_STG],	m_aPoints[BP_SE_STD],	iColour, objIds, _T("Northern Stage") );
				}
			}
			else
			{
				if( (iType&MILLS_TYPE_CONNECT_S)==0 )
				{
					DrawLine(	m_aPoints[BP_SE_STD],	m_aPoints[BP_SSE_STG],	iColour, objIds, _T("Southern Stage") );
					DrawLine(	m_aPoints[BP_SSE_STG],	m_aPoints[BP_SSW_STG],	iColour, objIds, _T("Southern Stage") );
					DrawLine(	m_aPoints[BP_SSW_STG],	m_aPoints[BP_SW_STD],	iColour, objIds, _T("Southern Stage") );
				}
			}
		}
		if( GetWidthOfWStage()>0 )
		{
			if( (iType&MILLS_TYPE_CONNECT_WSW)>0 || (iType&MILLS_TYPE_CONNECT_WNW)>0 )
			{
				if( (iType&MILLS_TYPE_CONNECT_WSW)>0 && (iType&MILLS_TYPE_CONNECT_WNW)==0)
				{
					DrawLine(	m_aPoints[BP_NW_STD],	m_aPoints[BP_WNW_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine( 	m_aPoints[BP_WNW_STG],	m_aPoints[BP_W_M_STG],	iColour, objIds, _T("Northern Stage") );
				}
				else if( (iType&MILLS_TYPE_CONNECT_WNW)>0 && (iType&MILLS_TYPE_CONNECT_WSW)==0 )
				{
					DrawLine(	m_aPoints[BP_W_M_STG],	m_aPoints[BP_WSW_STG],	iColour, objIds, _T("Northern Stage") );
					DrawLine(	m_aPoints[BP_WSW_STG],	m_aPoints[BP_SW_STD],	iColour, objIds, _T("Northern Stage") );
				}
			}
			else
			{
				if( (iType&MILLS_TYPE_CONNECT_W)==0 )
				{
					DrawLine(	m_aPoints[BP_SW_STD],	m_aPoints[BP_WSW_STG],	iColour, objIds, _T("Western Stage") );
					DrawLine(	m_aPoints[BP_WSW_STG],	m_aPoints[BP_WNW_STG],	iColour, objIds, _T("Western Stage") );
					DrawLine(	m_aPoints[BP_WNW_STG],	m_aPoints[BP_NW_STD],	iColour, objIds, _T("Western Stage") );
				}
			}
		}

		///////////////////////////////////////////////////////////////////
		//Corner Stages
		if( GetNEStageCnr() )
		{
			DrawLine(	m_aPoints[BP_NE_STD],	m_aPoints[BP_NNE_STG],	iColour, objIds, _T("North-East Stage Corner") );
			DrawLine(	m_aPoints[BP_NNE_STG],	m_aPoints[BP_NE_STG],	iColour, objIds, _T("North-East Stage Corner") );
			DrawLine(	m_aPoints[BP_NE_STG],	m_aPoints[BP_ENE_STG],	iColour, objIds, _T("North-East Stage Corner") );
			DrawLine(	m_aPoints[BP_ENE_STG],	m_aPoints[BP_NE_STD],	iColour, objIds, _T("North-East Stage Corner") );
		}
		if( GetSEStageCnr() )
		{
			DrawLine(	m_aPoints[BP_SE_STD],	m_aPoints[BP_ESE_STG],	iColour, objIds, _T("South-East Stage Corner") );
			DrawLine(	m_aPoints[BP_ESE_STG],	m_aPoints[BP_SE_STG],	iColour, objIds, _T("South-East Stage Corner") );
			DrawLine(	m_aPoints[BP_SE_STG],	m_aPoints[BP_SSE_STG],	iColour, objIds, _T("South-East Stage Corner") );
			DrawLine(	m_aPoints[BP_SSE_STG],	m_aPoints[BP_SE_STD],	iColour, objIds, _T("South-East Stage Corner") );
		}
		if( GetSWStageCnr() )
		{
			DrawLine(	m_aPoints[BP_SW_STD],	m_aPoints[BP_SSW_STG],	iColour, objIds, _T("South-West Stage Corner") );
			DrawLine(	m_aPoints[BP_SSW_STG],	m_aPoints[BP_SW_STG],	iColour, objIds, _T("South-West Stage Corner") );
			DrawLine(	m_aPoints[BP_SW_STG],	m_aPoints[BP_WSW_STG],	iColour, objIds, _T("South-West Stage Corner") );
			DrawLine(	m_aPoints[BP_WSW_STG],	m_aPoints[BP_SW_STD],	iColour, objIds, _T("South-West Stage Corner") );
		}
		if( GetNWStageCnr() )
		{
			DrawLine(	m_aPoints[BP_NW_STD],	m_aPoints[BP_WNW_STG],	iColour, objIds, _T("North-West Stage Corner") );
			DrawLine(	m_aPoints[BP_WNW_STG],	m_aPoints[BP_NW_STG],	iColour, objIds, _T("North-West Stage Corner") );
			DrawLine(	m_aPoints[BP_NW_STG],	m_aPoints[BP_NNW_STG],	iColour, objIds, _T("North-West Stage Corner") );
			DrawLine(	m_aPoints[BP_NNW_STG],	m_aPoints[BP_NW_STD],	iColour, objIds, _T("North-West Stage Corner") );
		}
	}

	///////////////////////////////////////////////////////////////////
	//Braces
	if( bBrace )
	{
		if( GetNBrace() ) DrawLine(	m_aPoints[BP_NNE_BRC],	m_aPoints[BP_NNW_BRC],	iColour, objIds, _T("Northern Bracing"), PS_DASHED );
		if( GetEBrace() ) DrawLine(	m_aPoints[BP_ENE_BRC],	m_aPoints[BP_ESE_BRC],	iColour, objIds, _T("Eastern Bracing"), PS_DASHED );
		if( GetSBrace() ) DrawLine(	m_aPoints[BP_SSE_BRC],	m_aPoints[BP_SSW_BRC],	iColour, objIds, _T("Southern Bracing"), PS_DASHED );
		if( GetWBrace() ) DrawLine(	m_aPoints[BP_WSW_BRC],	m_aPoints[BP_WNW_BRC],	iColour, objIds, _T("Western Bracing"), PS_DASHED );
	}

	///////////////////////////////////////////////////////////////////
	//Ties
	if( bTie )
	{
		;//assert( GetRunPointer()!=NULL );
		;//assert( GetRunPointer()->GetRunTemplate()!=NULL );
		TieTypesEnum	eType;
		eType = GetRunPointer()->GetTieType();
		if( eType==TIE_TYPE_MASONARY || eType==TIE_TYPE_COLUMN || eType==TIE_TYPE_YOKE )
		{
			if( GetNNETie() )
			{
				DrawLine(	m_aPoints[BP_NNE_STG],		m_aPoints[BP_NNE_WAL_TIE],	iColour, objIds, _T("NNE Tie") );
				DrawLine(	m_aPoints[BP_NNE_BEG_TIE],	m_aPoints[BP_NNE_END_TIE],	iColour, objIds, _T("NNE Tie") );
			}

			if( GetENETie() )
			{
				DrawLine(	m_aPoints[BP_ENE_STG],		m_aPoints[BP_ENE_WAL_TIE],	iColour, objIds, _T("ENE Tie") );
				DrawLine(	m_aPoints[BP_ENE_BEG_TIE],	m_aPoints[BP_ENE_END_TIE],	iColour, objIds, _T("ENE Tie") );
			}

			if( GetESETie() )
			{
				DrawLine(	m_aPoints[BP_ESE_STG],		m_aPoints[BP_ESE_WAL_TIE],	iColour, objIds, _T("ESE Tie") );
				DrawLine(	m_aPoints[BP_ESE_BEG_TIE],	m_aPoints[BP_ESE_END_TIE],	iColour, objIds, _T("ESE Tie") );
			}

			if( GetSSETie() )
			{
				DrawLine(	m_aPoints[BP_SSE_STG],		m_aPoints[BP_SSE_WAL_TIE],	iColour, objIds, _T("SSE Tie") );
				DrawLine(	m_aPoints[BP_SSE_BEG_TIE],	m_aPoints[BP_SSE_END_TIE],	iColour, objIds, _T("SSE Tie") );
			}

			if( GetSSWTie() )
			{
				DrawLine(	m_aPoints[BP_SSW_STG],		m_aPoints[BP_SSW_WAL_TIE],	iColour, objIds, _T("SSW Tie") );
				DrawLine(	m_aPoints[BP_SSW_BEG_TIE],	m_aPoints[BP_SSW_END_TIE],	iColour, objIds, _T("SSW Tie") );
			}

			if( GetWSWTie() )
			{
				DrawLine(	m_aPoints[BP_WSW_STG],		m_aPoints[BP_WSW_WAL_TIE],	iColour, objIds, _T("WSW Tie") );
				DrawLine(	m_aPoints[BP_WSW_BEG_TIE],	m_aPoints[BP_WSW_END_TIE],	iColour, objIds, _T("WSW Tie") );
			}

			if( GetWNWTie() )
			{
				DrawLine(	m_aPoints[BP_WNW_STG],		m_aPoints[BP_WNW_WAL_TIE],	iColour, objIds, _T("WNW Tie") );
				DrawLine(	m_aPoints[BP_WNW_BEG_TIE],	m_aPoints[BP_WNW_END_TIE],	iColour, objIds, _T("WNW Tie") );
			}

			if( GetNNWTie() )
			{
				DrawLine(	m_aPoints[BP_NNW_STG],		m_aPoints[BP_NNW_WAL_TIE],	iColour, objIds, _T("NNW Tie") );
				DrawLine(	m_aPoints[BP_NNW_BEG_TIE],	m_aPoints[BP_NNW_END_TIE],	iColour, objIds, _T("NNW Tie") );
			}
		}
		else if( eType==TIE_TYPE_BUTTRESS_12 || eType==TIE_TYPE_BUTTRESS_18 || eType==TIE_TYPE_BUTTRESS_24 )
		{
			//do nothing
			1;
		}
		else
		{
			;//assert( false );
		}
	}


	///////////////////////////////////////////////////////////////////
	//Shadecloth / Chainmesh
	if( GetChainMeshSide(NORTH) && GetShadeClothSide(NORTH) )	DrawLine( m_aPoints[BP_CHAIN_NNW], m_aPoints[BP_CHAIN_NNE],	COLOUR_CHAIN_AND_SHADE,	objIds, _T("Northern Shade&Chain"),	PS_DOTS_CLOSE );
	else if( GetChainMeshSide(NORTH) )							DrawLine( m_aPoints[BP_CHAIN_NNW], m_aPoints[BP_CHAIN_NNE],	COLOUR_CHAIN_LINK,		objIds, _T("Northern ChainMesh"),	PS_DOTS_CLOSE );
	else if( GetShadeClothSide(NORTH) )							DrawLine( m_aPoints[BP_CHAIN_NNW], m_aPoints[BP_CHAIN_NNE],	COLOUR_SHADE_CLOTH,		objIds, _T("Northern ShadeCloth"),	PS_DOTS_CLOSE );

	if( GetChainMeshSide(EAST) && GetShadeClothSide(EAST) )		DrawLine( m_aPoints[BP_CHAIN_ENE], m_aPoints[BP_CHAIN_ESE],	COLOUR_CHAIN_AND_SHADE,	objIds, _T("Northern Shade&Chain"),	PS_DOTS_CLOSE );
	else if( GetChainMeshSide(EAST) )							DrawLine( m_aPoints[BP_CHAIN_ENE], m_aPoints[BP_CHAIN_ESE],	COLOUR_CHAIN_LINK,		objIds, _T("Northern ChainMesh"),	PS_DOTS_CLOSE );
	else if( GetShadeClothSide(EAST) )							DrawLine( m_aPoints[BP_CHAIN_ENE], m_aPoints[BP_CHAIN_ESE],	COLOUR_SHADE_CLOTH,		objIds, _T("Northern ShadeCloth"),	PS_DOTS_CLOSE );

	if( GetChainMeshSide(SOUTH) && GetShadeClothSide(SOUTH) )	DrawLine( m_aPoints[BP_CHAIN_SSE], m_aPoints[BP_CHAIN_SSW],	COLOUR_CHAIN_AND_SHADE,	objIds, _T("Northern Shade&Chain"),	PS_DOTS_CLOSE );
	else if( GetChainMeshSide(SOUTH) )							DrawLine( m_aPoints[BP_CHAIN_SSE], m_aPoints[BP_CHAIN_SSW],	COLOUR_CHAIN_LINK,		objIds, _T("Northern ChainMesh"),	PS_DOTS_CLOSE );
	else if( GetShadeClothSide(SOUTH) )							DrawLine( m_aPoints[BP_CHAIN_SSE], m_aPoints[BP_CHAIN_SSW],	COLOUR_SHADE_CLOTH,		objIds, _T("Northern ShadeCloth"),	PS_DOTS_CLOSE );

	if( GetChainMeshSide(WEST) && GetShadeClothSide(WEST) )		DrawLine( m_aPoints[BP_CHAIN_WSW], m_aPoints[BP_CHAIN_WNW],	COLOUR_CHAIN_AND_SHADE,	objIds, _T("Northern Shade&Chain"),	PS_DOTS_CLOSE );
	else if( GetChainMeshSide(WEST) )							DrawLine( m_aPoints[BP_CHAIN_WSW], m_aPoints[BP_CHAIN_WNW],	COLOUR_CHAIN_LINK,		objIds, _T("Northern ChainMesh"),	PS_DOTS_CLOSE );
	else if( GetShadeClothSide(WEST) )							DrawLine( m_aPoints[BP_CHAIN_WSW], m_aPoints[BP_CHAIN_WNW],	COLOUR_SHADE_CLOTH,		objIds, _T("Northern ShadeCloth"),	PS_DOTS_CLOSE );


	///////////////////////////////////////////////////////////////////
	//Handrails
	if( GetHandRailDist()>0 )
	{
/*
		//See Bug# 327 - No handrails on Nth or Sth of the bay
		if( GetNHandRail() )	DrawLine( m_aPoints[BP_NW_HND], m_aPoints[BP_NE_HND],	iColour, objIds, "Northern Handrail", PS_DASHED );
		if( GetSHandRail() )	DrawLine( m_aPoints[BP_SE_HND], m_aPoints[BP_SW_HND],	iColour, objIds, "Southern Handrail", PS_DASHED );
*/
		if( GetEHandRail() )	DrawLine( m_aPoints[BP_NE_HND], m_aPoints[BP_SE_HND],	iColour, objIds, _T("Eastern Handrail"), PS_DASHED );
		if( GetWHandRail() )	DrawLine( m_aPoints[BP_SW_HND], m_aPoints[BP_NW_HND],	iColour, objIds, _T("Western Handrail"), PS_DASHED );
	}

	///////////////////////////////////////////////////////////////////
	//Arrow
	if( GetBayPointer()->GetBayType()!=BAY_TYPE_BUTTRESS &&
		GetBayPointer()->GetBayType()!=BAY_TYPE_STAIRS )
	{
		DrawLine( m_aPoints[BP_END_ARW], m_aPoints[BP_PNT_ARW], iColour, objIds, _T("Direction Arrow1/3") );
		DrawLine( m_aPoints[BP_PNT_ARW], m_aPoints[BP_NTH_ARW], iColour, objIds, _T("Direction Arrow2/3") );
		DrawLine( m_aPoints[BP_PNT_ARW], m_aPoints[BP_STH_ARW], iColour, objIds, _T("Direction Arrow3/3") );
	}

	///////////////////////////////////////////////////////////////////
	//Stairs
	if( GetBayPointer()->GetBayType()==BAY_TYPE_STAIRS )
	{
		DrawLine( m_aPoints[BP_E_STAIR], m_aPoints[BP_W_STAIR],	iColour, objIds, _T("Stairs Centre") );
		DrawLine( m_aPoints[BP_1ASTAIR], m_aPoints[BP_1BSTAIR],	iColour, objIds, _T("Stairs Stair 1/6") );
		DrawLine( m_aPoints[BP_2ASTAIR], m_aPoints[BP_2BSTAIR],	iColour, objIds, _T("Stairs Stair 2/6") );
		DrawLine( m_aPoints[BP_3ASTAIR], m_aPoints[BP_3BSTAIR],	iColour, objIds, _T("Stairs Stair 3/6") );
		DrawLine( m_aPoints[BP_4ASTAIR], m_aPoints[BP_4BSTAIR],	iColour, objIds, _T("Stairs Stair 4/6") );
		DrawLine( m_aPoints[BP_5ASTAIR], m_aPoints[BP_5BSTAIR],	iColour, objIds, _T("Stairs Stair 5/6") );
		DrawLine( m_aPoints[BP_6ASTAIR], m_aPoints[BP_6BSTAIR],	iColour, objIds, _T("Stairs Stair 6/6") );
	}

	///////////////////////////////////////////////////////////////////
	//Ladder
	if( GetBayPointer()->GetBayType()==BAY_TYPE_LADDER )
	{
		DrawLine( m_aPoints[BP_E_LADDR], m_aPoints[BP_C_LADDR],	iColour, objIds, _T("Ladder Centre 1/2") );
		DrawLine( m_aPoints[BP_C_LADDR], m_aPoints[BP_N_LADDR],	iColour, objIds, _T("Ladder Centre 2/2") );
		DrawLine( m_aPoints[BP_1ALADDR], m_aPoints[BP_1BLADDR],	iColour, objIds, _T("Ladder Rung 1/5") );
		DrawLine( m_aPoints[BP_2ALADDR], m_aPoints[BP_2BLADDR],	iColour, objIds, _T("Ladder Rung 2/5") );
		DrawLine( m_aPoints[BP_3ALADDR], m_aPoints[BP_3BLADDR],	iColour, objIds, _T("Ladder Rung 3/5") );
		DrawLine( m_aPoints[BP_4ALADDR], m_aPoints[BP_4BLADDR],	iColour, objIds, _T("Ladder Rung 4/5") );
		DrawLine( m_aPoints[BP_5ALADDR], m_aPoints[BP_5BLADDR],	iColour, objIds, _T("Ladder Rung 5/5") );
	}

	/////////////////////////////////////////////////////////////////////
	//LooseTieTubes
	int					i, j;
	Point3D				pt[6];
	CString				sDesc;
	Vector3D			Vector;
	Matrix3D			Trans, Transform, Rotation;
	BayTieTubeTemplate	*pTemplate;

	for( i=0; i<GetNumberOfTieTubeTemplates(); i++ )
	{
		pTemplate = GetTieTubeTemplate(i);

		//create the points
		pt[0].set( 0.00, 0.00, 0.00 );
		pt[1].set( pTemplate->m_dFullLength, 0.00, 0.00 );
		pt[2].set( pTemplate->m_dFullLength-OVERRUN_TIE_TUBE, OVERRUN_TIE_TUBE, 0.00 );
		pt[3].set( pTemplate->m_dFullLength-OVERRUN_TIE_TUBE, -1.00*OVERRUN_TIE_TUBE, 0.00 );
		pt[4].set( OVERRUN_TIE_TUBE, OVERRUN_TIE_TUBE, 0.00 );
		pt[5].set( OVERRUN_TIE_TUBE, -1.00*OVERRUN_TIE_TUBE, 0.00 );

		Vector.set( -1.00*OVERRUN_TIE_TUBE, 0.00, 0.00 );
		Transform.setToTranslation( Vector );

		Vector.set( 0.00, 0.00, 1.00 );
		Rotation.setToRotation( pTemplate->m_dAngle, Vector );
		Transform = Rotation * Transform;

		switch( pTemplate->m_eStandard )
		{
		case( CNR_NORTH_EAST ):
			Vector = m_aPoints[BP_NE_STD] - m_aPoints[BP_SW_STD];
			break;
		case( CNR_SOUTH_EAST ):
			Vector = m_aPoints[BP_SE_STD] - m_aPoints[BP_SW_STD];
			break;
		case( CNR_SOUTH_WEST ):
			Vector = m_aPoints[BP_SW_STD] - m_aPoints[BP_SW_STD];
			break;
		case( CNR_NORTH_WEST ):
			Vector = m_aPoints[BP_NW_STD] - m_aPoints[BP_SW_STD];
			break;
		default:
			;//assert( false );
		}
		Trans.setToTranslation( Vector );
		Transform = Trans * Transform;

		for( j=0; j<6; j++ )
		{
			pt[j].transformBy( Transform );
		}

    sDesc.Format( _T("TieTube%i: Tie Tube ") );
		DrawLine( pt[0], pt[1],	iColour, objIds, sDesc+_T("1/3") );
		DrawLine( pt[2], pt[3],	iColour, objIds, sDesc+_T("2/3") );
		DrawLine( pt[4], pt[5],	iColour, objIds, sDesc+_T("3/3") );
	}

	///////////////////////////////////////////////////////////////////
	//Standards
	//The grip points function returns the last grip point of the bay
	//	which we have set to be the SW standards!  thus this must be
	//	the last point in the bay!

//	if( (iType&MILLS_TYPE_MISSING_STND_NE)==0 )
		DrawCircle( m_aPoints[BP_NE_STD],	GetSchematicStandardRadius(), iColour, objIds, PS_LINE );
//	if( (iType&MILLS_TYPE_MISSING_STND_SE)==0 && GetBayPointer()->GetInner()==NULL )
		DrawCircle( m_aPoints[BP_SE_STD],	GetSchematicStandardRadius(), iColour, objIds, PS_LINE );
//	if( (iType&MILLS_TYPE_MISSING_STND_NW)==0 && GetBayPointer()->GetBackward()==NULL )
		DrawCircle( m_aPoints[BP_NW_STD],	GetSchematicStandardRadius(), iColour, objIds, PS_LINE );
//	if( (iType&MILLS_TYPE_MISSING_STND_SW)==0 && GetBayPointer()->GetInner()==NULL && GetBayPointer()->GetBackward()==NULL )
		DrawCircle( m_aPoints[BP_SW_STD],	GetSchematicStandardRadius(), iColour, objIds, PS_LINE, true );

	GetBayPointer()->SetSWStandardEntityID(objIds.last());
}

void LapBoardTemplate::DrawLines(AcDbObjectIdArray &objIds)
{
	double	dLength;
	int		i, iColour;

	dLength = GetBayPointer()->GetBayLength();
	iColour = GetBayColour( dLength );

	//
	// 11_ _ _ _1_ _ _ _ _4_ _ _ _12
	//         /|         |\
	//       /  |_________|  \
	//     /    2         3    \
	//  0/                       \5
	// 10\                       /
	//     \    8_________7    /
	//       \  |         |  /
	// 13 _ _ _\|_ _ _ _ _|/_ _ _ 14
	//          9         6

	CString sDesc;
	for( i=POINT_LAPBOARD_00; i<POINT_LAPBOARD_10; i++ )
	{
		sDesc.Format( _T("Lapboard Line %i/%i"), i+1, POINT_LAPBOARD_10 );
		DrawLine( m_aPoints[i],	m_aPoints[i+1], iColour, objIds, sDesc );
	}
/*	if( GetHandrails() )
	{
		DrawLine( m_aPoints[POINT_LAPBOARD_11],	m_aPoints[POINT_LAPBOARD_12], iColour, objIds, "Lapboard Handrail 1/2", PS_DASHED );
		DrawLine( m_aPoints[POINT_LAPBOARD_13],	m_aPoints[POINT_LAPBOARD_14], iColour, objIds, "Lapboard Handrail 2/2", PS_DASHED );
	}
*/
}

AcDbObjectId LapBoardTemplate::AddTextToSchematic(CString &sText, TextPositionEnum eTextPosition)
{
	double				dTextHeight, dBayWidth, dBayLength;
	Entity				*pText;
	Point3D				ptOrigin;
	Vector3D			Vector;
	Matrix3D			Translation, Rotation;
	AcDbObjectId		recordId, Id;
	EntityReactor		*pReactor;
	Acad::ErrorStatus	es;
	AcDbTextStyleTable	*pTable;

	Id.setNull();

	dBayWidth = m_dFullWidth;
	dBayLength = m_dFullLength;

	es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pTable, AcDb::kForRead);
	;//assert( es==Acad::eOk );
	es = pTable->getAt( TEXT_STYLE_LAPBOARD, recordId );
	if( es==Acad::eKeyNotFound )
	{
		es = pTable->getAt( TEXT_STYLE_STANDARD, recordId );
	}
	if( es==Acad::eOk )
	{
		dTextHeight = GetBayPointer()->GetCompDetails()->GetActualHeight( GetBayPointer()->GetSystem(), CT_TEXT, 0, MT_STEEL );
		dTextHeight*= 3.00/4.00;

		//postion the text at the origin
		ptOrigin.set( 0.00, 0.00, 0.00 );
		pText = (Entity*)((AcDbEntity*) new AcDbText( ptOrigin, sText, recordId, dTextHeight ) );

		//position the text in the middle of the bay, ie half way across the length
		//	of the bay, and halfway across the width of the bay, plus the width of
		//	the stageboard!
		switch(eTextPosition)
		{
		case( TP_BAY_LENGTH ):
			es = ((AcDbText*)pText)->setHorizontalMode(AcDb::kTextCenter);
			;//assert( es==Acad::eOk );
			es = ((AcDbText*)pText)->setVerticalMode(AcDb::kTextVertMid);
			;//assert( es==Acad::eOk );
			Vector.set( dBayLength/2.00, 0.00, 0.00 );
			break;
		default:
			;//assert( false );
		}

		//perform the transformation
		Translation.setTranslation(Vector);
		Vector.set( 0.00, 0.00, 1.00 );
		Rotation.setToRotation( pi, Vector );
		Translation = Translation * Rotation;
		es = pText->transformBy( Translation );
		;//assert( es==Acad::eOk );
		pText->postToDatabase( LAYER_NAME_SCHEMATIC );

    ///////////////////////////////////////////////////////////////////
    //XData
    SetXData( pText, _T("Lapboard Text") );

		//Reactor stuff
		pText->AttachedReactors( &pReactor );
		es = pText->open();
		;//assert( es==Acad::eOk );
		pReactor->SetBayPointer(GetBayPointer());
		es = pText->close();
		;//assert( es==Acad::eOk );

		//we can now store the id
		Id = pText->objectId();

		CString sTemp;
    sTemp = _T("Lapboard Template - Text: ");
		sTemp+= sText;
		gED.AddEntityInfo( Id, sTemp );
	}

	es = pTable->close();
	;//assert( es==Acad::eOk );
	return Id;
}

/////////////////////////////////////////////////////////////////////////////////////
//
void LapBoardTemplate::CreateSchematicTextEntities( AcDbObjectIdArray &objIds )
{
	CString sText;

	m_objIdLapLength.setNull();

	//////////////////////////////////////////////////////////////////
	//set the bay number to a ? mark
	double dLength = m_dFullLength;
	if( dLength<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		dLength>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		dLength = COMPONENT_LENGTH_0800;

	sText.Format( _T("%ix%1.1fm"), m_iNumberOfBoards, dLength*CONVERT_MM_TO_M );
	m_objIdLapLength = AddTextToSchematic( sText, TP_BAY_LENGTH );
	;//assert( !m_objIdLapLength.isNull() );
	objIds.append( m_objIdLapLength );
}


/////////////////////////////////////////////////////////////////////////////////////
//
void LapBoardTemplate::SetAllSchematicText()
{
	CString	sText;

	////////////////////////////////////////////////////
	//bay length label
	double dLength = m_dFullLength;
	if( dLength<=COMPONENT_LENGTH_0700+ROUND_ERROR &&
		dLength>=COMPONENT_LENGTH_0700-ROUND_ERROR)
		dLength = COMPONENT_LENGTH_0800;

	sText.Format( _T("%ix%1.1fm"), m_iNumberOfBoards, dLength*CONVERT_MM_TO_M );
	;//assert( !m_objIdLapLength.isNull() );
	SetSchematicText( sText, m_objIdLapLength );
}


/////////////////////////////////////////////////////////////////////////////////////
//
void LapBoardTemplate::CreateSchematicPoints()
{
	CreatePoints();
}


/////////////////////////////////////////////////////////////////////////////////////
//
void LapBoardTemplate::SetSchematicGroup( AcDbGroup *pGroup )
{
	PreviewTemplate::SetSchematicGroup(pGroup);
}

Run * BayTemplate::GetRunPointer()
{
	return GetBayPointer()->GetRunPointer();
}

Acad::ErrorStatus  PreviewTemplate::DrawLine(Point3D pt1, Point3D pt2, int iColour, AcDbObjectIdArray &objIds, CString sDesc, PlotSyleEnum eHL/*=PS_LINE*/ )
{
	if( m_bCreateEntities )
	{
		if( GetBayPointer()!=NULL )
		{
			CString sTemp;
			switch( GetBayPointer()->GetBayType() )
			{
			case( BAY_TYPE_BAY ):
				//sTemp.Format( "Bay%i: %s", GetBayPointer()->GetBayNumber(), sDesc );
        sTemp.Format( _T("Bay: %s"), sDesc );
				break;
			case( BAY_TYPE_LAPBOARD ):
				//sTemp.Format( "Lapboard%i: %s", GetBayPointer()->GetID()+1, sDesc );
        sTemp.Format( _T("Lapboard: %s"), sDesc );
				break;
			case( BAY_TYPE_STAIRS ):
				//sTemp.Format( "Stair%i: %s", GetBayPointer()->GetID()+1, sDesc );
        sTemp.Format( _T("Stair: %s"), sDesc );
				break;
			case( BAY_TYPE_LADDER ):
				//sTemp.Format( "Ladder%i: %s", GetBayPointer()->GetID()+1, sDesc );
        sTemp.Format( _T("Ladder: %s"), sDesc );
				break;
			case( BAY_TYPE_BUTTRESS ):
				//sTemp.Format( "Buttress%i: %s", GetBayPointer()->GetID()+1, sDesc );
        sTemp.Format( _T("Buttress: %s"), sDesc );
				break;
			case( BAY_TYPE_UNDEFINED ):
				;//assert( false );
				break;
			}
			sDesc = _T("\t\t\tKwikscaf Details = ") + sTemp;
		}
		return AddLineToDB( pt1, pt2, objIds, iColour, sDesc, eHL );
	}
	else
	{
		acedGrDraw( asDblArray( pt1 ), asDblArray( pt2 ), iColour, (eHL==PS_DASHED)? 1: 0 );
		return Acad::eOk;
	}
}

Acad::ErrorStatus PreviewTemplate::AddLineToDB(Point3D &ptPointA, Point3D &ptPointB, AcDbObjectIdArray &objIds, int iColour, CString sDesc, PlotSyleEnum eHL/*=PS_LINE*/ )
{
	AcDbObjectId		id;
	Acad::ErrorStatus	es;
	Entity				*pEnt;
	EntityReactor		*pReactor;
	TCHAR				PlotStyleName[30];

	///////////////////////////////////////////////////////////////////
	//create the line
#ifdef	USE_LINE_CLASS
	pEnt = (Entity*)((AcDbEntity*) new Line( ptPointA, ptPointB, sDesc ));
#else	//#ifdef	USE_LINE_CLASS
	pEnt = (Entity*)((AcDbEntity*) new AcDbLine( ptPointA, ptPointB ));
#endif	//#ifdef	USE_LINE_CLASS

	if (!pEnt)
	{
		acedAlert(_T("Not enough memory to create a Line!"));
		return Acad::eOutOfMemory;
	}

	///////////////////////////////////////////////////////////////////
	//Colour
	if (pEnt->setColorIndex((Adesk::UInt16)iColour) != Acad::eOk)
	{
		acutPrintf(_T("\nInvalid Colour chosen.\n"));
	}

	///////////////////////////////////////////////////////////////////
	//Style
	GetPlotStyleName( eHL, PlotStyleName );
	if( pEnt->setLinetype(PlotStyleName)!=Acad::eOk )
	{
		acutPrintf(_T("\nCan't find %s Style!\n"), PlotStyleName );
	}

	///////////////////////////////////////////////////////////////////
	//put it in the Database
  if ((es = pEnt->postToDatabase( LAYER_NAME_SCHEMATIC )) != Acad::eOk)
  {
    acutPrintf(_T("\nCan't post entity to database!\n"));
  }

	///////////////////////////////////////////////////////////////////
	//Group entity stuff
	id = pEnt->objectId();
	gED.AddEntityInfo( id, _T("Preview Template - Add Line") );
	objIds.append( id );

	///////////////////////////////////////////////////////////////////
	//XData
	SetXData( pEnt, sDesc );

	///////////////////////////////////////////////////////////////////
	//reactor stuff
	pReactor = NULL;
	pEnt->AttachedReactors( &pReactor );
	pEnt->open();
	pReactor->SetBayPointer( GetBayPointer() );
	pEnt->close();
	pReactor->close();

  return es;
}

Acad::ErrorStatus  PreviewTemplate::DrawCircle(Point3D pt, double dRadius, int iColour, AcDbObjectIdArray &objIds, PlotSyleEnum eHL, bool bSWCircle/*=false*/ )
{
	int	i;	
	Point3D	Circle[8];

	if( m_bCreateEntities )
	{
		return AddCircleToDB( pt, dRadius, objIds, iColour, eHL, bSWCircle );
	}
	else
	{
		for( i=0; i<8; i++ )
			Circle[i] = pt;
		Circle[0].y+= dRadius;

		Circle[1].x+= dRadius*cos(pi/4.00);
		Circle[1].y+= dRadius*sin(pi/4.00);

		Circle[2].x+= dRadius;

		Circle[3].x+= dRadius*cos(pi/4.00);
		Circle[3].y-= dRadius*sin(pi/4.00);

		Circle[4].y-= dRadius;

		Circle[5].x-= dRadius*cos(pi/4.00);
		Circle[5].y-= dRadius*sin(pi/4.00);

		Circle[6].x-= dRadius;
		
		Circle[7].x-= dRadius*cos(pi/4.00);
		Circle[7].y+= dRadius;

		acedGrDraw( asDblArray(Circle[0]), asDblArray(Circle[1]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[1]), asDblArray(Circle[2]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[2]), asDblArray(Circle[3]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[3]), asDblArray(Circle[4]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[4]), asDblArray(Circle[5]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[5]), asDblArray(Circle[6]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[6]), asDblArray(Circle[7]), iColour, (eHL==PS_DASHED)? 1: 0 );
		acedGrDraw( asDblArray(Circle[7]), asDblArray(Circle[0]), iColour, (eHL==PS_DASHED)? 1: 0 );
		return Acad::eOk;
	}
}

Acad::ErrorStatus PreviewTemplate::AddCircleToDB(Point3D &pt, double dRadius, AcDbObjectIdArray &objIds, int iColour, PlotSyleEnum eHL, bool bSWCircle/*=false*/ )
{
	int					i;
	TCHAR				PlotStyleName[30];
	Entity				*pEnt;
	Point3D				Circle[4];
	Vector3D			Vector;
	AcDbObjectId		id;
	EntityReactor		*pReactor;
	AcGePoint3dArray	vertices;
	Acad::ErrorStatus	es;

	///////////////////////////////////////////////////////////////////
	//create the line
	Vector.set( 0.00, 0.00, 1.00 );

	for( i=0; i<4; i++ )
	{
		Circle[i] = pt;
	}
	Circle[0].y+= dRadius/2.00;
	Circle[1].x+= dRadius/2.00;
	Circle[2].y-= dRadius/2.00;
	Circle[3].x-= dRadius/2.00;
	for( i=0; i<4; i++ )
	{
		vertices.append( Circle[i] );
	}

	pEnt = (Entity*)((AcDbEntity*) new AcDb2dPolyline( AcDb::k2dFitCurvePoly,
								vertices, 0, Adesk::kTrue, dRadius, dRadius ) );
	if (!pEnt)
	{
		acedAlert(_T("Not enough memory to create a Line!"));
		return Acad::eOutOfMemory;
	}

	///////////////////////////////////////////////////////////////////
	//Colour
	if (pEnt->setColorIndex((Adesk::UInt16)iColour) != Acad::eOk)
	{
		acutPrintf(_T("\nInvalid Colour chosen.\n"));
	}

	///////////////////////////////////////////////////////////////////
	//Style
	GetPlotStyleName( eHL, PlotStyleName );
	if( pEnt->setLinetype(PlotStyleName)!=Acad::eOk )
	{
		acutPrintf(_T("\nCan't find %s Style!\n"), PlotStyleName );
	}

	///////////////////////////////////////////////////////////////////
	//put it in the Database
	es = pEnt->postToDatabase( LAYER_NAME_SCHEMATIC );

	///////////////////////////////////////////////////////////////////
	//Group entity stuff
	id = pEnt->objectId();
	gED.AddEntityInfo( id, _T("Preview Template - AddCircle") );
	objIds.append( id );

  ///////////////////////////////////////////////////////////////////
  //XData
  SetXData( pEnt, _T("Bay Circle") );

	///////////////////////////////////////////////////////////////////
	//reactor stuff
	pReactor = NULL;
	pEnt->AttachedReactors( &pReactor );
	pEnt->open();
	pReactor->SetBayPointer( GetBayPointer() );
	if( bSWCircle )
		pReactor->SetIsGroup( true );
 
	pEnt->close();
	pReactor->close();

	return es;
}


Point3DArray *RunTemplate::GetBoundingBoxPointer()
{
	return &m_BoundingBox;
}

Point3DArray *RunTemplate::GetLapboardBoxPointer()
{
	return &m_pLapboardBox;
}


void BayTemplate::SetOffsetForShortStage(double dOffset)
{
	m_dOffsetForShortStage = dOffset;
}

double BayTemplate::GetOffsetForShortStage()
{
	return m_dOffsetForShortStage;
}


///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void LapBoardTemplate::Serialize(CArchive &ar)
{
	int			iRunIDFirstBay, iRunIDLastBay, iBayIDFirstBay, iBayIDLastBay;
	BOOL		BTemp; // for bool conversion macros
	CString		sMsg, sTemp;
	Matrix3D	Transform;

	PreviewTemplate::Serialize(ar); // call base class serialize

	if (ar.IsStoring())    // Store Object?
	{
		ar << LAPBOARD_TEMPLATE_VERSION_LATEST;

		////////////////////////////////////////////////////////
		//LAPBOARD_TEMPLATE_VERSION_1_0_2
		ar << m_iNumberOfBoards;
		STORE_bool_IN_AR(m_bHandrails);

		////////////////////////////////////////////////////////
		//LAPBOARD_TEMPLATE_VERSION_1_0_1
		STORE_bool_IN_AR(m_bShowLapBoards);

		ar << m_dFullLength;
		ar << m_dFullWidth;
		ar << m_dArrowHeadPercentLong;
		ar << m_dArrowHeadPercentWide;

		ar << m_pt1stPoint.x;
		ar << m_pt1stPoint.y;
		ar << m_pt1stPoint.z;

		ar << m_pt2ndPoint.x;
		ar << m_pt2ndPoint.y;
		ar << m_pt2ndPoint.z;

		ar << m_iOverLapReqd;
		STORE_bool_IN_AR(m_bLapBoardReqdThisRun);
		STORE_bool_IN_AR(m_bDrawingLapBoard);
		STORE_bool_IN_AR(m_bFirstBayInRun);
		STORE_bool_IN_AR(m_bLastBayInRun);
		STORE_bool_IN_AR(m_bAllowExactLength);
/*
		iRunIDFirstBay = (m_pFirstBayInRun->GetRunPointer())->GetRunID();
		iRunIDLastBay = (m_pLastBayInRun->GetRunPointer())->GetRunID();
		iBayIDFirstBay = m_pFirstBayInRun->GetID();
		iBayIDLastBay = m_pLastBayInRun->GetID();

		iBayIDFirstBay = m_pFirstBayInRun->GetBayNumber();
		iBayIDLastBay = m_pLastBayInRun->GetBayNumber();

		ar << iRunIDFirstBay;
		ar << iRunIDLastBay;
		ar << iBayIDFirstBay;
		ar << iBayIDLastBay;
*/		
		GetSchematicTransform().Serialize(ar);
	}
	else					// or Load Object?
	{
		VersionNumber uiVersion;
		m_iNumberOfBoards = 5;
		m_bHandrails = false;

		ar >> uiVersion;
		switch (uiVersion)
		{
		case LAPBOARD_TEMPLATE_VERSION_1_0_2 :
			ar >> m_iNumberOfBoards;
			LOAD_bool_IN_AR(m_bHandrails);
			//fallthrough

		case LAPBOARD_TEMPLATE_VERSION_1_0_1 :

			LOAD_bool_IN_AR(m_bShowLapBoards);
				
			ar >> m_dFullLength;
			ar >> m_dFullWidth;
			ar >> m_dArrowHeadPercentLong;
			ar >> m_dArrowHeadPercentWide;

			ar >> m_pt1stPoint.x;
			ar >> m_pt1stPoint.y;
			ar >> m_pt1stPoint.z;

			ar >> m_pt2ndPoint.x;
			ar >> m_pt2ndPoint.y;
			ar >> m_pt2ndPoint.z;

			ar >> m_iOverLapReqd;
			LOAD_bool_IN_AR(m_bLapBoardReqdThisRun);
			LOAD_bool_IN_AR(m_bDrawingLapBoard);
			LOAD_bool_IN_AR(m_bFirstBayInRun);
			LOAD_bool_IN_AR(m_bLastBayInRun);
			LOAD_bool_IN_AR(m_bAllowExactLength);
/*
			ar >> iRunIDFirstBay;
			ar >> iRunIDLastBay;
			ar >> iBayIDFirstBay;
			ar >> iBayIDLastBay;

			Run *pRun;

			;//assert( gpController!=NULL );
			pRun = gpController->GetRun(iRunIDFirstBay);
			m_pFirstBayInRun = pRun->GetBay(iBayIDFirstBay);
			m_pFirstBayInRun = gpController->GetBayFromBayNumber(iBayIDFirstBay);

			pRun = gpController->GetRun(iRunIDLastBay);
			m_pLastBayInRun = pRun->GetBay(iBayIDLastBay);

			m_pLastBayInRun = gpController->GetBayFromBayNumber(iBayIDLastBay);
*/				
			Transform.Serialize(ar);
			SetSchematicTransform(Transform);
			break;
		case LAPBOARD_TEMPLATE_VERSION_1_0_0 :
			LOAD_bool_IN_AR(m_bShowLapBoards);
				
			ar >> m_dFullLength;
			ar >> m_dFullWidth;
			ar >> m_dArrowHeadPercentLong;
			ar >> m_dArrowHeadPercentWide;

			ar >> m_pt1stPoint.x;
			ar >> m_pt1stPoint.y;
			ar >> m_pt1stPoint.z;

			ar >> m_pt2ndPoint.x;
			ar >> m_pt2ndPoint.y;
			ar >> m_pt2ndPoint.z;

			ar >> m_iOverLapReqd;
			LOAD_bool_IN_AR(m_bLapBoardReqdThisRun);
			LOAD_bool_IN_AR(m_bDrawingLapBoard);
			LOAD_bool_IN_AR(m_bFirstBayInRun);
			LOAD_bool_IN_AR(m_bLastBayInRun);
			LOAD_bool_IN_AR(m_bAllowExactLength);

			ar >> iRunIDFirstBay;
			ar >> iRunIDLastBay;
			ar >> iBayIDFirstBay;
			ar >> iBayIDLastBay;
				
			break;
		default:
			;//assert( false );
			if( uiVersion>LAPBOARD_TEMPLATE_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: LapBoardTemplate.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), LAPBOARD_TEMPLATE_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}


bool LapBoardTemplate::GetHandrails()
{
	return m_bHandrails;
}

void LapBoardTemplate::SetHandRails( bool bHandrails )
{
	m_bHandrails = bHandrails;
}

double RunTemplate::GetRLStart()
{
	return m_ptPointStart.z;
}

double RunTemplate::GetRLEnd()
{
	return m_ptPointEnd.z;
}

double RunTemplate::GetDistanceFromWall()
{
	return m_dDistanceFromWall;
}

void RunTemplate::SetRLStart( double dRL )
{
	m_ptPointStart.z = dRL;
}

void RunTemplate::SetRLEnd(double dRL)
{
	m_ptPointEnd.z = dRL;
}

void RunTemplate::SetDistanceToWall(double dDistance)
{
	m_dDistanceFromWall = dDistance;
}

double* RunTemplate::GetRLStartPointer()
{
	return &(m_ptPointStart.z);
}

double* RunTemplate::GetRLEndPointer()
{
	return &(m_ptPointEnd.z);
}

double* RunTemplate::GetDistanceFromWallPointer()
{
	return &m_dDistanceFromWall;
}

double RunTemplate::GetTiesVerticallyEvery()
{
	return m_dTiesVerticallyEvery;
}

void RunTemplate::SetTiesVerticallyEvery(double dEvery)
{
	m_dTiesVerticallyEvery = dEvery;
}

double * RunTemplate::GetTiesVerticallyEveryPointer()
{
	return &m_dTiesVerticallyEvery;
}

TieTypesEnum RunTemplate::GetTieType()
{
	return m_eTieType;
}

void RunTemplate::SetTieType(TieTypesEnum eType)
{
	m_eTieType = eType;
}

Point3D RunTemplate::GetPointStart()
{
	return m_ptPointStart;
}

Point3D RunTemplate::GetPointEnd()
{
	return m_ptPointEnd;
}

void RunTemplate::SetPointStart(Point3D pt)
{
	m_ptPointStart = pt;
}

void RunTemplate::SetPointEnd(Point3D pt)
{
	m_ptPointEnd = pt;
}

Point3D RunTemplate::GetPoint(int iPointNumber)
{
	switch( iPointNumber )
	{
	case( WP_START ):
		return GetPointStart();
	case( WP_END ):
		return GetPointEnd();
	default:
		;//assert( false );
	}
	return GetPointStart();
}

void RunTemplate::SetPoint(int iPointNumber, Point3D pt )
{
	switch( iPointNumber )
	{
	case( WP_START ):
		SetPointStart( pt );
		break;
	case( WP_END ):
		SetPointEnd( pt );
		break;
	default:
		;//assert( false );
	}
}

void BayTemplate::AddTieTubeTemplate(BayTieTubeTemplate *pTemplate)
{
	m_LooseTieTubes.Add( pTemplate );
}

bool BayTemplate::RemoveTieTubeTemplate(int iIndex, int nCount/*=1*/)
{
	int		i;
	BayTieTubeTemplate	*pTemplate;

	if( iIndex<0 || iIndex+nCount>GetNumberOfTieTubeTemplates() || nCount==0 )
		return false;

	for( i=iIndex; i<iIndex+nCount; i++ )
	{
		pTemplate = GetTieTubeTemplate(i);
		delete pTemplate;
	}

	m_LooseTieTubes.RemoveAt( iIndex, nCount );

	return true;
}

bool BayTemplate::RemoveTieTubeTemplate(SideOfBayEnum eSide)
{
	int					i;
	bool				bReturn;
	BayTieTubeTemplate	*pTemplate;

	bReturn = false;
	if( eSide=ALL_SIDES )
	{
		bReturn = RemoveTieTubeTemplate( 0, GetNumberOfTieTubeTemplates() );
	}
	else
	{
		for( i=0; i<GetNumberOfTieTubeTemplates(); i++ )
		{
			pTemplate = GetTieTubeTemplate(i);
			if( eSide==CornerAsSideOfBay( pTemplate->m_eStandard ) )
			{
				//we need to delete this one!
				if( RemoveTieTubeTemplate( i ) )
					bReturn = true;
				//we have removed one from the array, so the the size will be smaller
				i--;
			}
		}
	}
	return bReturn;
}

void BayTemplate::SetTieTubeVertSeparation(double dSeparation)
{
	m_dTieTubeSeparation = dSeparation;
}

double BayTemplate::GetTieTubeVertSeparation()
{
	return m_dTieTubeSeparation;
}

int BayTemplate::GetNumberOfTieTubeTemplates()
{
	return m_LooseTieTubes.GetSize();
}

BayTieTubeTemplates* BayTemplate::GetTieTubeTemplates()
{
	return &m_LooseTieTubes;
}

BayTieTubeTemplate *BayTemplate::GetTieTubeTemplate(int iIndex)
{
	;//assert( iIndex>=0 );
	;//assert( iIndex<GetNumberOfTieTubeTemplates() );
	return m_LooseTieTubes.GetAt( iIndex );
}

void LapBoardTemplate::SetNumberOfBoards(int iBoards)
{
	m_iNumberOfBoards = iBoards;
	if( iBoards>NUM_PLANKS_FOR_BAY_SIZE_2400 )
		m_dFullWidth = GetWidthForNumberOfPlanks( NUM_PLANKS_FOR_BAY_SIZE_2400 );
	else if( iBoards>NUM_PLANKS_FOR_BAY_SIZE_1800 )
		m_dFullWidth = GetWidthForNumberOfPlanks( NUM_PLANKS_FOR_BAY_SIZE_1800 );
	else if( iBoards>NUM_PLANKS_FOR_BAY_SIZE_1200 )
		m_dFullWidth = GetWidthForNumberOfPlanks( NUM_PLANKS_FOR_BAY_SIZE_1200 );
	else if( iBoards>NUM_PLANKS_FOR_BAY_SIZE_0700 )
		m_dFullWidth = GetWidthForNumberOfPlanks( NUM_PLANKS_FOR_BAY_SIZE_0700 );
	else
		m_dFullWidth = GetWidthForNumberOfPlanks( NUM_PLANKS_FOR_BAY_SIZE_0700 );
}


void BayTemplate::ClearMatrixForwardText()
{
	if( !m_objIdForwardMatrix.isNull() )
		SetSchematicText( _T(" "), m_objIdForwardMatrix );
}

void BayTemplate::ClearMatrixBackwardText()
{
	if( !m_objIdBackwardMatrix.isNull() )
		SetSchematicText( _T(" "), m_objIdBackwardMatrix );
}

double BayTemplate::CalcLengthOfHopup(SideOfBayEnum eHopupSide, SideOfBayEnum eBracketSide)
{
	Component	*pComponent;
	;//assert(GetBayPointer()!=NULL);
	;//assert(GetBayPointer()->GetController()!=NULL);
	if( GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
	{
		int			iLiftID;
		Lift		*pLift;

		for( iLiftID=0; iLiftID<GetBayPointer()->GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetBayPointer()->GetLift( iLiftID );
			if( pLift->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eBracketSide ) )
			{
				break;
			}
		}

		if( iLiftID<GetBayPointer()->GetNumberOfLifts() )
		{
			pComponent = GetBayPointer()->GetLift( iLiftID )->
									GetComponent(CT_STAGE_BOARD, LIFT_RISE_0000MM, eHopupSide );
			if( pComponent!=NULL )
				return GetLengthOfBracket( eBracketSide );
		}
	}
	else
	{
		pComponent = GetBayPointer()->GetLift( GetBayPointer()->GetNumberOfLifts()-1 )->
								GetComponent(CT_STAGE_BOARD, LIFT_RISE_0000MM, eHopupSide );
		if( pComponent!=NULL )
			return GetLengthOfBracket( eBracketSide );
	}
	return 0.00;
}

double BayTemplate::GetLengthOfBracket(SideOfBayEnum eBracketSide)
{
	Component	*pComponent;
	if( GetBayPointer()->GetController()->IsWallOffsetFromLowestHopup() )
	{
		int			iLiftID;
		Lift		*pLift;

		for( iLiftID=0; iLiftID<GetBayPointer()->GetNumberOfLifts(); iLiftID++ )
		{
			pLift = GetBayPointer()->GetLift( iLiftID );
			if( pLift->HasComponentOfTypeOnSide( CT_HOPUP_BRACKET, eBracketSide ) )
			{
				break;
			}
		}

		if( iLiftID<GetBayPointer()->GetNumberOfLifts() )
		{
			pComponent = GetBayPointer()->GetLift( iLiftID )->
								GetComponent(CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eBracketSide );
			if(pComponent!=NULL)
				return pComponent->GetLengthActual();
		}
	}
	else
	{
		pComponent = GetBayPointer()->GetLift( GetBayPointer()->GetNumberOfLifts()-1 )->
								GetComponent(CT_HOPUP_BRACKET, LIFT_RISE_0000MM, eBracketSide );
		if(pComponent!=NULL)
			return pComponent->GetLengthActual();
	}
	return 0.00;
}

void BayTemplate::SetMillsSystemBay( MillsType mtType )
{
	m_mtMillsCnrType = mtType;
}

MillsType BayTemplate::GetMillsSystemType() const
{
	return m_mtMillsCnrType;
}


void LapBoardTemplate::SetLapBoardReqdThisRun(bool bReqd)
{
	m_bLapBoardReqdThisRun = bReqd;
}

bool LapBoardTemplate::IsLapBoardReqdThisRun()
{
	return m_bLapBoardReqdThisRun;
}

double BayTemplate::GetSchematicStandardRadius()
{
	return gpController->GetRadiusOfSchematicStandard();
}

void BayTemplate::SetChainMeshSide(SideOfBayEnum eSide, bool bChainMesh)
{
	;//assert( eSide>=NORTH && eSide<=WEST );
	m_bChainMeshSide[(int)eSide] = bChainMesh;
}

void BayTemplate::SetShadeClothSide(SideOfBayEnum eSide, bool bShadeCloth)
{
	;//assert( eSide>=NORTH && eSide<=WEST );
	m_bShadeClothSide[(int)eSide] = bShadeCloth;
}

void BayTemplate::RemoveAllShadeCloth()
{
	for( int iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
	{
		SetShadeClothSide( (SideOfBayEnum)iSide, false );
	}
}

void BayTemplate::RemoveAllChainMesh()
{
	for( int iSide=(int)NORTH; iSide<=(int)WEST; iSide++ )
	{
		SetChainMeshSide( (SideOfBayEnum)iSide, false );
	}
}

bool BayTemplate::GetChainMeshSide(SideOfBayEnum eSide)
{
	;//assert( eSide>=NORTH && eSide<=WEST );
#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
	return m_bChainMeshSide[(int)eSide];
#else	//#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
	return false;
#endif	//#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
}

bool BayTemplate::GetShadeClothSide(SideOfBayEnum eSide)
{
	;//assert( eSide>=NORTH && eSide<=WEST );
#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
	return m_bShadeClothSide[(int)eSide];
#else	//#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
	return false;
#endif	//#ifdef	SHOW_CHAIN_AND_SHADE_IN_SCHEMATIC
}

// Changes made to this function by ~SJ~, 03.08.2007
void PreviewTemplate::SetXData( Entity *pEnt, CString sText )
{
	int					  i;
  TCHAR				  appName[132];
	CString				sName;
  struct resbuf	*pRb, *pTemp;
	Vector3D			Vector;
	Acad::ErrorStatus	es;

	if( pEnt!=NULL && !pEnt->objectId().isNull() )
	{
		es = pEnt->open();
		if( es==Acad::eOk || es==Acad::eWasOpenForWrite )
		{
			Vector = AcGeVector3d::kIdentity;
			Vector.x = 1.00;
			Vector.y = 0.00;
			Vector.z = 0.00;

      /*
			for( i=0; i<XDATA_LABEL_BAY.GetLength(); i++ )
			{
				appName[i] = XDATA_LABEL_BAY[i];
			}
			appName[i] = _T('\0');
      pRb = pEnt->xData(appName);
      */

			pRb = pEnt->xData(XDATA_LABEL_BAY);
			if (pRb == NULL)
			{
				// If xdata is not present, register the application
				// and add appName to the first resbuf in the list.
				// Notice that there is no -3 group as there is in

				// AutoLISP. This is ONLY the xdata so
				// the -3 xdata-start marker isn't needed.
				//acdbRegApp(appName);
        acdbRegApp(XDATA_LABEL_BAY);

				////////////////////////////////////////////////////////
				//XDSB_APPNAME		- XData reference name
				//pRb = acutNewRb(AcDb::kDxfRegAppName);
        pRb = acutBuildList(AcDb::kDxfRegAppName, XDATA_LABEL_BAY, NULL);
				pTemp = pRb;
				//pTemp->resval.rstring = (TCHAR*) malloc(_tcslen(appName) + 1);
				//_tcscpy(pTemp->resval.rstring, appName);
			}

			////////////////////////////////////////////////////////
			//XDSB_BAY_POINTER	- Bay Pointer value
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger32);
			pTemp = pTemp->rbnext;
			pTemp->resval.rlong = (long)GetBayPointer();

			////////////////////////////////////////////////////////
			//XDSB_DESCRIPTION	- Description
			pTemp->rbnext = acutBuildList(AcDb::kDxfXdAsciiString, sText, NULL);
			pTemp = pTemp->rbnext;
      /*
			for( i=0; i<sText.GetLength(); i++ )
			{
				appName[i] = sText[i];
			}
			appName[i] = _T('\0');
      DCSPrintf(_T("\nDescription = [%s]\n"), appName);
			//pTemp->resval.rstring = (TCHAR*) malloc(_tcslen(appName) + 1);
			_tcscpy(pTemp->resval.rstring, appName);
      DCSPrintf(_T("\nrstring = [%s]\n"), pTemp->resval.rstring);
      */

			////////////////////////////////////////////////////////
			//XDSB_POSITION		- 3D Position
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXCoord);
			pTemp = pTemp->rbnext;
			pTemp->resval.rpoint[0] = 0.00;
			pTemp->resval.rpoint[1] = 0.00;
			pTemp->resval.rpoint[2] = 0.00;

			////////////////////////////////////////////////////////
			//XDSB_DISPLACEMENT	- Vector of first arm
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXDisp);
			pTemp = pTemp->rbnext;
			pTemp->resval.rpoint[0] = 1.00;
			pTemp->resval.rpoint[1] = 0.00;
			pTemp->resval.rpoint[2] = 0.00;

			////////////////////////////////////////////////////////
			//XDSB_DIRECTION	- Vector of second arm
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdWorldXDir);
			pTemp = pTemp->rbnext;
			pTemp->resval.rpoint[0] = 0.00;
			pTemp->resval.rpoint[1] = 1.00;
			pTemp->resval.rpoint[2] = 0.00;

			////////////////////////////////////////////////////////
			//XDSB_DISTANCE		- Distance moved
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdDist);
			pTemp = pTemp->rbnext;
			pTemp->resval.rreal = 0.00;

			////////////////////////////////////////////////////////
			//XDSB_SCALE		- Scale factor
			pTemp->rbnext = acutNewRb(AcDb::kDxfXdScale);
			pTemp = pTemp->rbnext;
			pTemp->resval.rreal = 0.00;

			// The following code shows the use of upgradeOpen()
			// to change the entity from read to write.
			pEnt->upgradeOpen();
			pEnt->setXData(pRb);
    
			if( es==Acad::eOk )	pEnt->close();

			acutRelRb(pRb);
		}
		else
		{
			//not able to open entity for write;
			;//assert( false );
		}
	}
}

SystemEnum BayTemplate::GetSystem() const
{
	return m_eSystem;
}

void BayTemplate::SetSystem(SystemEnum eSystem)
{
	m_eSystem = eSystem;
}

AcDbObjectIdArray &PreviewTemplate::GetObjectIds()
{
	return m_objIds;
}

void PreviewTemplate::ClearObjectIds()
{
	while( m_objIds.length()>0 )
	{
		m_objIds.removeLast();
	}
}

void RunTemplate::SetBracingLength2400(double dLength)
{
	m_dBracingLength2400 = dLength;
}

void RunTemplate::SetBracingLength1800(double dLength)
{
	m_dBracingLength1800 = dLength;
}

void RunTemplate::SetBracingLength1200(double dLength)
{
	m_dBracingLength1200 = dLength;
}

void RunTemplate::SetBracingLength0700(double dLength)
{
	m_dBracingLength0700 = dLength;
}

double RunTemplate::GetBracingLength2400() const
{
	return m_dBracingLength2400;
}

double RunTemplate::GetBracingLength1800() const
{
	return m_dBracingLength1800;
}

double RunTemplate::GetBracingLength1200() const
{
	return m_dBracingLength1200;
}

double RunTemplate::GetBracingLength0700() const
{
	return m_dBracingLength0700;
}
