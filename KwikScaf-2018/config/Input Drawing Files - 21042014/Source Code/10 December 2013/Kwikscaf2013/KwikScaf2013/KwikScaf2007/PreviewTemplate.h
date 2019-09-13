// PreviewTemplate.h: interface for the PreviewTemplate class.
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

#include "BayDefinitions.h"	// Added by ClassView
#include "FreeTieTube.h"	// Added by ClassView
#include "EntityReactor.h"	// Added by ClassView
#if !defined(AFX_PREVIEWTEMPLATE1_H__011E5534_4AB9_11D3_9E92_0008C7999B1D__INCLUDED_)
#define AFX_PREVIEWTEMPLATE1_H__011E5534_4AB9_11D3_9E92_0008C7999B1D__INCLUDED_


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		PreviewTemplate, RunTemplate, BayTemplate, LapBoardTemplate
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 13/12/00 11:11a $
//	Version			: $Revision: 42 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// These Classes are used for the Templates to create the preview bays.  It may
//	be possible in the future remove these classes and take the details directly
//	from the AutobuildRun class
///////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//includes
#include "stdafx.h"
#include "MeccanoDefinitions.h"
#include "GraphicTypes.h"	// Added by ClassView


//////////////////////////////////////////////////////////////////////////////
////////////////////////PreviewTemplate///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//forward declarations
class AcDbGroup;
class Bay;


#include "Entity.h"	// Added by ClassView
class PreviewTemplate  
{
public:
	void ClearObjectIds();
	AcDbObjectIdArray &GetObjectIds();

	///////////////////////////////////////////////////////////////////////////////
	//Create/destroy
	PreviewTemplate();
	virtual ~PreviewTemplate();

	///////////////////////////////////////////////////////////////////////////////
	//equivalence
	PreviewTemplate &operator=(const PreviewTemplate &Source);

	///////////////////////////////////////////////////////////////////////////////
	//Pointers to other objects
	void SetBayPointer( Bay *pBay );
	Bay *GetBayPointer();

	///////////////////////////////////////////////////////////////////////////////
	//flags
	bool m_bCreateEntities;

	///////////////////////////////////////////////////////////////////////////////
	//Drawing/adding to database routines
	virtual void DrawLines(AcDbObjectIdArray &objIds)=NULL;
	Acad::ErrorStatus DrawLine(Point3D pt1, Point3D pt2, int iColour, AcDbObjectIdArray &objIds, CString sDesc, PlotSyleEnum eHL=PS_LINE );
	Acad::ErrorStatus DrawCircle( Point3D pt, double dRadius, int iColour, AcDbObjectIdArray &objIds, PlotSyleEnum eHL=PS_LINE, bool bSWCircle=false );
	Acad::ErrorStatus AddCircleToDB( Point3D &pt, double dRadius, AcDbObjectIdArray &objIds, int iColour, PlotSyleEnum eHL=PS_LINE, bool bSWCircle=false );
	Acad::ErrorStatus AddLineToDB(Point3D &ptPointA, Point3D &ptPointB, AcDbObjectIdArray &objIds, int iColour, CString sDesc, PlotSyleEnum eHL=PS_LINE );

	///////////////////////////////////////////////////////////////////////////////
	//Text
	virtual AcDbObjectId AddTextToSchematic( CString &sText, TextPositionEnum eTextPosition )=NULL;
	virtual void CreateSchematicTextEntities( AcDbObjectIdArray &objIds )=NULL;
	virtual void SetAllSchematicText()=NULL;
	void SetSchematicText( CString sText, AcDbObjectId id );

	///////////////////////////////////////////////////////////////////////////////
	//Group
	virtual void CreateSchematicPoints()=NULL;
	virtual void SetSchematicGroup( AcDbGroup *pGroup, bool bShowWarning=true );
	AcDbGroup *GetSchematicGroup();

	///////////////////////////////////////////////////////////////////////////////
	//Deletion
	void DeleteSchematic();

	///////////////////////////////////////////////////////////////////////////////
	//Movement
	void MoveSchematic( Matrix3D Transform, bool bStore );
	void SetSchematicTransform( Matrix3D Transform );
	Matrix3D GetSchematicTransform() const;
	Matrix3D UnMoveSchematic();

	///////////////////////////////////////////////////////////////////////////////
	//Schematic
	void CreateSchematic( );
	void UpdateSchematicView();

	///////////////////////////////////////////////////////////////////////////////
	//Storage
	void Serialize(CArchive &ar);

	///////////////////////////////////////////////////////////////////////////////
	//Points
	Point3D	m_aPoints[BP_SIZE];

protected:
	AcDbObjectIdArray	m_objIds;

	void SetXData( Entity *pEnt, CString sText );

	Bay				*m_pBay;
	Matrix3D		m_SchematicTransform;
	AcDbGroup		*m_pSchematicGroup;
//	EntityReactor	*m_pGroupReactor;

private:
	void SetGlobalsToDefault();
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////RunTemplate/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class RunTemplate
{
public:
	double GetBracingLength0700() const;
	double GetBracingLength1200() const;
	double GetBracingLength1800() const;
	double GetBracingLength2400() const;
	void SetBracingLength0700( double dLength );
	void SetBracingLength1200( double dLength );
	void SetBracingLength1800( double dLength );
	void SetBracingLength2400( double dLength );
	void SetPoint( int iPointNumber, Point3D pt );
	Point3D GetPoint( int iPointNumber );
	void SetPointEnd( Point3D pt );
	void SetPointStart( Point3D pt );
	Point3D GetPointEnd();
	Point3D GetPointStart( );
	void SetTieType( TieTypesEnum eType );
	TieTypesEnum GetTieType();
	double * GetTiesVerticallyEveryPointer();
	void SetTiesVerticallyEvery( double dEvery );
	double GetTiesVerticallyEvery();
	//init/destroy
	RunTemplate();
	virtual ~RunTemplate();
	void SetGlobalsToDefault();

	///////////////////////////////////////////////////////////////////////////////
	//equivalence
	RunTemplate &operator=(const RunTemplate &Source);


	///////////////////////////////////////////////////////////////////////////////
	//Fit
	RunFitEnum GetFit() const;
	void SetFit( RunFitEnum eFit );

	
	///////////////////////////////////////////////////////////////////////////////
	//Ties
	int		GetBaysPerTie() const;
	void	SetBaysPerTie( int iBaysPerTie );

	
	///////////////////////////////////////////////////////////////////////////////
	//Bracing
	int		GetBaysPerBrace() const;
	void	SetBaysPerBrace( int iBaysPerBrace );


	///////////////////////////////////////////////////////////////////////////////
	//Distance to wall
	double GetDistanceFromWall();
	void SetDistanceToWall( double dDistance );
	double* GetDistanceFromWallPointer();


	///////////////////////////////////////////////////////////////////////////////
	//RL Stuff
	void SetRLEnd( double dRL );
	void SetRLStart( double dRL );
	double GetRLEnd();
	double GetRLStart();
	double* GetRLStartPointer();
	double* GetRLEndPointer();


	///////////////////////////////////////////////////////////////////////////////
	//Bounding box stuff
	Point3DArray *GetBoundingBoxPointer();
	Point3DArray *GetLapboardBoxPointer();


	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	void Serialize(CArchive &ar);

protected:

	Point3D		m_ptPointStart;
	Point3D		m_ptPointEnd;
	TieTypesEnum m_eTieType;
	double m_dTiesVerticallyEvery;

	//Distance to wall
	double m_dDistanceFromWall;

	//Bounding box stuff
	Point3DArray m_pLapboardBox;
	Point3DArray m_BoundingBox;

	//Ties
	int	m_iBaysPerTie;
	
	//Bracing
	int	m_iBaysPerBrace;
	double	m_dBracingLength2400;
	double	m_dBracingLength1800;
	double	m_dBracingLength1200;
	double	m_dBracingLength0700;

	//fit
	RunFitEnum	m_eFit;
};

#include "FreeTieTube.h"	// Added by ClassView

//forward declaration
class Run;
const double	OVERRUN_TIE_TUBE			= 100.00;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////BayTemplate/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class BayTemplate : public PreviewTemplate
{
public:
	//System (Mills/KwikStage/Other)
	void SetSystem( SystemEnum eSystem );
	SystemEnum GetSystem() const;

	bool GetShadeClothSide( SideOfBayEnum eSide );
	bool GetChainMeshSide( SideOfBayEnum eSide );
	void RemoveAllChainMesh();
	void RemoveAllShadeCloth();
	void SetShadeClothSide( SideOfBayEnum eSide, bool bShadeCloth=true );
	void SetChainMeshSide( SideOfBayEnum eSide, bool bChainMesh );
	double GetSchematicStandardRadius();

	//Mills stuff
	MillsType GetMillsSystemType() const;
	void SetMillsSystemBay( MillsType mtType );

	void ClearMatrixBackwardText();
	void ClearMatrixForwardText();
	BayTieTubeTemplates* GetTieTubeTemplates();
	BayTieTubeTemplate *GetTieTubeTemplate( int iIndex );
	int GetNumberOfTieTubeTemplates();
	bool RemoveTieTubeTemplate( SideOfBayEnum eSide );
	double GetTieTubeVertSeparation();
	void SetTieTubeVertSeparation(double dSeparation);
	bool RemoveTieTubeTemplate(int iIndex, int nCount=1 );
	void AddTieTubeTemplate( BayTieTubeTemplate *pTemplate );

	//////////////////////////////////////////////////////////////////////////////
	//Create/Delete
	BayTemplate();
	virtual ~BayTemplate();

	//////////////////////////////////////////////////////////////////////////////
	//Bay dimensions
	double GetBayLengthActual();
	double GetBayWidthActual();

	//////////////////////////////////////////////////////////////////////////////
	//equivalence
	BayTemplate &operator=(const BayTemplate &Source);

	//////////////////////////////////////////////////////////////////////////////
	//Pointers to other objects
	Run *GetRunPointer();

	//////////////////////////////////////////////////////////////////////////////
	//Drawing routines
	virtual void DrawLines(AcDbObjectIdArray &objIds);

	//////////////////////////////////////////////////////////////////////////////
	//Schematic stuff
	virtual void CreateSchematicPoints();
	virtual void SetSchematicGroup(AcDbGroup *pGroup);

	//////////////////////////////////////////////////////////////////////////////
	//Text
	virtual void SetAllSchematicText( );
	virtual AcDbObjectId AddTextToSchematic(CString &sText, TextPositionEnum eTextPosition );
	virtual void CreateSchematicTextEntities(AcDbObjectIdArray &objIds);

	//////////////////////////////////////////////////////////////////////////////
	//Storage
	void Serialize(CArchive &ar);

	//////////////////////////////////////////////////////////////////////////////
	//Movement
	void		Transform( Matrix3D Transform );
	void		CreatePoints();

	//////////////////////////////////////////////////////////////////////////////
	//Bay Dimension Details
	double	GetBayWidth() const;
	void	SetBayWidth( double dWidth );

	double	GetBayLength() const;
	void	SetBayLength( double dLength );

	//////////////////////////////////////////////////////////////////////////////
	//Stages
	double	GetOffsetForShortStage();
	void	SetOffsetForShortStage( double dOffset );

	double	GetWidthOfNStage() const;
	void	SetWidthOfNStage( double dWidth );

	double	GetWidthOfEStage() const;
	void	SetWidthOfEStage( double dWidth );

	double	GetWidthOfSStage() const;
	void	SetWidthOfSStage( double dWidth );

	double	GetWidthOfWStage() const;
	void	SetWidthOfWStage( double dWidth );

	//////////////////////////////////////////////////////////////////////////////
	//Stage corners
	bool	GetNEStageCnr() const;
	void	SetNEStageCnr( bool bExists );

	bool	GetSEStageCnr() const;
	void	SetSEStageCnr( bool bExists );

	bool	GetSWStageCnr() const;
	void	SetSWStageCnr( bool bExists );

	bool	GetNWStageCnr() const;
	void	SetNWStageCnr( bool bExists );

	//////////////////////////////////////////////////////////////////////////////
	//Bracing
	bool	GetNBrace() const;
	void	SetNBrace( bool bExists );

	bool	GetEBrace() const;
	void	SetEBrace( bool bExists );

	bool	GetSBrace() const;
	void	SetSBrace( bool bExists );

	bool	GetWBrace() const;
	void	SetWBrace( bool bExists );

	double	GetBraceDist() const;
	void	SetBraceDist( double dBraceDist );

	//////////////////////////////////////////////////////////////////////////////
	//Ties
	double	GetWidthOfTie() const;
	void	SetWidthOfTie( double dWidth );

	bool	GetNNETie() const;
	void	SetNNETie( bool bTie );

	bool	GetENETie() const;
	void	SetENETie( bool bTie );

	bool	GetESETie() const;
	void	SetESETie( bool bTie );

	bool	GetSSETie() const;
	void	SetSSETie( bool bTie );

	bool	GetSSWTie() const;
	void	SetSSWTie( bool bTie );

	bool	GetWSWTie() const;
	void	SetWSWTie( bool bTie );

	bool	GetWNWTie() const;
	void	SetWNWTie( bool bTie );

	bool	GetNNWTie() const;
	void	SetNNWTie( bool bTie );

	//////////////////////////////////////////////////////////////////////////////
	//HandRails
	bool	GetNHandRail() const;
	void	SetNHandRail( bool bExists );

	bool	GetEHandRail() const;
	void	SetEHandRail( bool bExists );

	bool	GetSHandRail() const;
	void	SetSHandRail( bool bExists );

	bool	GetWHandRail() const;
	void	SetWHandRail( bool bExists );

	double	GetHandRailDist() const;
	void	SetHandRailDist( double dHandRailDist );


protected:
	SystemEnum			m_eSystem;
	double				m_dShadeChainDist;
	double				m_dTieTubeSeparation;
	MillsType			m_mtMillsCnrType;
	BayTieTubeTemplates	m_LooseTieTubes;

	//object ID's
	AcDbObjectId m_objIdBackwardMatrix;
	AcDbObjectId m_objIdForwardMatrix;
	AcDbObjectId m_objIdBayLength;
	AcDbObjectId m_objIdBayNumber;
	AcDbObjectIdArray m_objIdsTieTubes;


	//Dimensions
	double	m_dLengthOfBays;
	double	m_dWidthOfBays;
	double	m_dWidthOfTie;

	//Stages
	double	m_dOffsetForShortStage;
	double	m_dWidthOfNStage;	//North Stage
	double	m_dWidthOfEStage;	//East  Stage
	double	m_dWidthOfSStage;	//South Stage
	double	m_dWidthOfWStage;	//West  Stage

	//Stage corners
	bool	m_bNEStageCnr;
	bool	m_bSEStageCnr;
	bool	m_bSWStageCnr;
	bool	m_bNWStageCnr;

	//Bracing
	bool	m_bNBrace;
	bool	m_bSBrace;
	bool	m_bEBrace;
	bool	m_bWBrace;
	double	m_dBraceDist;

	//Ties
	bool	m_bNNETie;
	bool	m_bENETie;
	bool	m_bESETie;
	bool	m_bSSETie;
	bool	m_bSSWTie;
	bool	m_bWSWTie;
	bool	m_bWNWTie;
	bool	m_bNNWTie;

	//ShadeCloth/ChainMesh
	bool	m_bChainMeshSide[4];
	bool	m_bShadeClothSide[4];

	//HandRails
	bool	m_bNHandRail;
	bool	m_bEHandRail;
	bool	m_bSHandRail;
	bool	m_bWHandRail;
	double	m_dHandRailDist;


private:
	double GetLengthOfBracket( SideOfBayEnum eBracketSide );
	double CalcLengthOfHopup( SideOfBayEnum eHopupSide, SideOfBayEnum eBracketSide );
	void SetGlobalsToDefault();
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////LapBoardTemplate////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const	int		MIN_PLANKS_FOR_LAPBOARD	= 1;
const	int		MAX_PLANKS_FOR_LAPBOARD	= 16;

class LapBoardTemplate : public PreviewTemplate
{
public:
	void SetNumberOfBoards(int iBoards);
	bool IsLapBoardReqdThisRun();
	void SetLapBoardReqdThisRun( bool bReqd );
	//Create/Destroy
	LapBoardTemplate();
	virtual ~LapBoardTemplate();

	//equivalence
	LapBoardTemplate &operator=(const LapBoardTemplate &Source);

	//Handrails
	bool	GetHandrails();
	void	SetHandRails( bool bHandrails );

	//Text
	virtual AcDbObjectId AddTextToSchematic( CString &sText, TextPositionEnum eTextPosition );
	virtual void CreateSchematicTextEntities( AcDbObjectIdArray &objIds );
	virtual void SetAllSchematicText();

	//Drawing
	virtual void DrawLines(AcDbObjectIdArray &objIds);
	virtual void CreateSchematicPoints();
	void CreatePoints();

	//Group
	virtual void SetSchematicGroup( AcDbGroup *pGroup );

	//Flags
	void SetLastBayInRun( Bay *pBay );
	void SetFirstBayInRun( Bay *pBay );
	Bay * GetLastBayInRun();
	Bay * GetFirstBayInRun();

	//Movement
	void Transform( Matrix3D Transform );

	//storage
	void Serialize(CArchive &ar);

	//////////////////////////////////////////////////////////////////////////////
	//Lapboard Dimension Details
	//////////////////////////////////////////////////////////////////////////////
	bool	m_bShowLapBoards;
	double	m_dFullLength;
	double	m_dFullWidth;
	double	m_dArrowHeadPercentLong;
	double	m_dArrowHeadPercentWide;
	Point3D	m_pt1stPoint;
	Point3D	m_pt2ndPoint;
	int		m_iOverLapReqd;
	bool	m_bLapBoardReqdThisRun;
	bool	m_bDrawingLapBoard;
	bool	m_bFirstBayInRun;
	bool	m_bLastBayInRun;
	bool	m_bAllowExactLength;
	int		m_iNumberOfBoards;

private:
	void	SetGlobalsToDefault();

protected:
	double			m_dProposedLapboardWidth;
	bool			m_bHandrails;
	Bay				*m_pFirstBayInRun;
	Bay				*m_pLastBayInRun;
	AcDbObjectId	m_objIdLapLength;
};


#endif // !defined(AFX_PREVIEWTEMPLATE1_H__011E5534_4AB9_11D3_9E92_0008C7999B1D__INCLUDED_)

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: PreviewTemplate.h $
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:11a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 17/04/00   Time: 4:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 13/03/00   Time: 5:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 4/01/00    Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 13/12/99   Time: 8:22a
 * Updated in $/Meccano/Stage 1/Code
 * About to ensure all removes and deletes are correct
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:54a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 27/10/99   Time: 12:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Dar          Date: 27/10/99   Time: 9:11a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 26/10/99   Time: 2:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 19/10/99   Time: 3:10p
 * Updated in $/Meccano/Stage 1/Code
 * Birdcaging is almost working correctly now
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 19/10/99   Time: 9:03a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on Birdcaging
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 29/09/99   Time: 12:33p
 * Updated in $/Meccano/Stage 1/Code
 * Simple Curve avoidance functionality now operational
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Dar          Date: 9/07/99    Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * serialize for bay, preview and run templates done
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 *
 *******************************************************************************/
