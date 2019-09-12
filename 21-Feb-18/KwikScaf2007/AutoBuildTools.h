// AutoBuildTools.h: interface for the AutoBuildTools class.
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

#if !defined(AFX_AUTOBUILDTOOLS_H__3B04BEB4_485B_11D3_8861_0008C7999B1D__INCLUDED_)
#define AFX_AUTOBUILDTOOLS_H__3B04BEB4_485B_11D3_8861_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeccanoDefinitions.h"
#include "PreviewTemplate.h"
#include "Run.h"
#include "Entity.h"			// Added by ClassView
#include "EntityReactor.h"	// Added by ClassView
#include "GraphicTypes.h"	// Added by ClassView
#include "BayDefinitions.h"	// Added by ClassView

//Forward declaration
class Controller;
class AutobuildDialog;
class AcDb3dPolyline;

enum MousePointsEnum
{
	MP_1ST,
	MP_2ND,
	MP_3RD,
	MP_SIZE
};

enum HitLocationEnum
{
	HL_BAY,
	HL_HOPUP_NORTH,
	HL_HOPUP_EAST,
	HL_HOPUP_SOUTH,
	HL_HOPUP_WEST,
	HL_CNR_FILLER_NE,
	HL_CNR_FILLER_SE,
	HL_CNR_FILLER_SW,
	HL_CNR_FILLER_NW,
	HL_MISSED,
	HL_INVALID
};

class AutobuildPropertiesToolbar;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////AutoBuildTools//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class AutoBuildTools : public Run
{
public:
	bool m_bUseCalcRL;
	SystemEnum GetSystem();
	double GetStarSeparation();
	double GetSchematicStandardRadius( );
	AutobuildPropertiesToolbar	*pAPTool;
	void SetPredefinedBayLengths(Controller *pController);
	void SetController( Controller *pController );
	void StartAutobuild();
	double GetSensibleAngle( double dAngle );
	Matrix3D m_PrevRunLastBayTransform;
	bool GetUseLastPoints( );
	void SetUseLastPoints( bool bUseLast=true );



	////////////////////////////////
	////////////////////////////////
	////////Functionality///////////
	////////////////////////////////
	////////////////////////////////


	//////////////////////////////////////////////////////////////
	//RL calculations
	bool FindZOfXYPointOnLine(Point3D &pt, Point3D ptRLs[2]);
	bool FindPointOfDistanceAlongLine(double dDistance, Point3D	&pt, Point3D ptRLs[2]);
	bool FindZOfXYPointOnPlane( Point3D &pt, Point3D ptRLs[MP_SIZE] );
	bool CompareRLWithABBayRL( double dRL, double dABRL, bool bCausedByOverrun=false );
	double GetTopRLFromAutobuild();


	///////////////////////////////////////////////////////////////
	//Arc stuff
	double GetHalfAngleInnerStandards(double ABCD, double EF, double GH );
	double GetDistanceToArcOfMidBay(double ABCD, double EF );
	double GetDistanceToArcOfInnerStandard( double dBayLength, double dRadius);
	double GetHalfAngleOuterStandards(	double dOuterStandardSeparation,
										double dBayWidth, double dBayLength,
										double dRadius );
	void Serialize(CArchive &ar);
	Bay * GetABBay() const;
	bool GetMouseOverrun();
	void Overrun2ndPoint( double dDistanceOverrun );
	void SetMouseOverrun( bool bOverrun=true );
 

	//////////////////////////////////////////////////////////////////////////////
	//Construction/Destruction
	//////////////////////////////////////////////////////////////////////////////
	AutoBuildTools();
	virtual ~AutoBuildTools();


	//////////////////////////////////////////////////////////////////////////////
	//MDI Aware considerations
	//////////////////////////////////////////////////////////////////////////////
	void ShowDialogs(bool bShowDialogs);

	
	//////////////////////////////////////////////////////////////////////////////
	//Controllers Autobuild Run
	//////////////////////////////////////////////////////////////////////////////
	bool				IsFirstRunInAutobuild();
	void				SetFirstRunInAutobuild( bool bFirstRun=true );
	BayTemplate			*GetBayTemplate();
	RunTemplate			*GetRunTemplate();
	LapBoardTemplate	*GetLapBoardTemplate();


	//////////////////////////////////////////////////////////////////////////////
	//Allowing access to the drawing routines
	//////////////////////////////////////////////////////////////////////////////
	bool GetAccessEnabled();
	void SetAccessEnabled( bool bEnabled );


	//////////////////////////////////////////////////////////////////////////////
	//Creating Run previews
	//////////////////////////////////////////////////////////////////////////////
	Acad::ErrorStatus CreateRun( double dRL=0.00 );
	Acad::ErrorStatus CreateBirdCage( double &dRL );
	Acad::ErrorStatus CreateArcRun( double &dRL );
	Acad::ErrorStatus CreateLapboard( );
	Acad::ErrorStatus ProcessArcThirdPoint( double &dRL );
	Acad::ErrorStatus CreateTieTube();


	//////////////////////////////////////////////////////////////////////////////
	//Mouse points
	//////////////////////////////////////////////////////////////////////////////
	void SetPoint1( Point3D pt );
	void SetPoint2( Point3D pt );
	void SetPoint3( Point3D pt );
	Point3D GetPoint1();
	Point3D GetPoint2();
	Point3D GetPoint3();


	//////////////////////////////////////////////////////////////////////////////
	//Flags
	//////////////////////////////////////////////////////////////////////////////
	bool GetFirstPointFlag();
	void SetFirstPointFlag( bool bFirstPoint );
	void SetMouseTrack( bool bTracking );
	bool GetMouseTrack();
	void SetArcFlag( bool bArc );
	bool GetArcFlag();


	//////////////////////////////////////////////////////////////////////////////
	//Other
	//////////////////////////////////////////////////////////////////////////////
	BOOL	filterMouseForLapboard(MSG *pMsg);
	BOOL	filterMouseForTieTube(MSG *pMsg);
	BOOL	filterMouse(MSG *pMsg);
	void	SetBayWidth( double dWidth );
	void	SetBayLength( double dLength );
	void	RefreshScreen( bool bFilterMouse=true );

protected:
	double m_dRLFirstBirdcage;
	int m_iPreviousPreviousRunID;
	int m_iPreviousRunID;
	double m_dMillsPreviousSouthStageWidth;
	SideOfBayEnum m_eLapboardWillCrossBaySide;
	SideOfBayEnum m_eLapboardWillCrossPreviousBaySide;
	bool m_bBuildForward;
	bool m_bUseEasternHopup;
	int m_iCurrentFilterType;
	////////////////////////////////
	////////////////////////////////
	/////////Attributes/////////////
	////////////////////////////////
	////////////////////////////////


	double m_dBayLengthActual0700;
	double m_dBayLengthActual1200;
	double m_dBayLengthActual1800;
	double m_dBayLengthActual2400;
	double m_dStandardWidthActual;

	//////////////////////////////////////////////////////////////////////////////
	//Flags
	//////////////////////////////////////////////////////////////////////////////
	bool	m_bArc;				//Are we creating an arc run?
	bool	m_bAddToDB;			//do we want to add this to the database or just draw to screen
	bool	m_bFirstPoint;		//Is this the first point in a run?
	bool	m_bMouseTrack;		//are we currently tracking the mouse?
	bool	m_bAccessDenied;
	bool	m_bBirdCageMode;
	bool	m_bAccessEnabled;	//Used to prevent re-entry into a function
	bool	m_bNortherBirdcage;
	bool	m_bCalcRLErrorShown;
	bool	m_bOverrunMousePoint;
	bool	m_bFirstRunInThisAutobuild;


	//////////////////////////////////////////////////////////////////////////////
	//Colour
	//////////////////////////////////////////////////////////////////////////////
	int	m_iColour;		//General colour variable
	int	m_iBayColour;	//Colour of the bay - size dependant

	//////////////////////////////////////////////////////////////////////////////
	//Points
	//////////////////////////////////////////////////////////////////////////////
	Point3D m_ptMousePoint[MP_SIZE];	//The 3 mouse location point (3 for arc, 2 for wall)
	bool	m_bClosePointSet;			//Has the closing point been set?
	Point3D	m_ptClosePoint;				//The closing point
	Point3D m_ptBirdcagePoints[MP_SIZE];


	//////////////////////////////////////////////////////////////////////////////
	//Bay and lapboard dimension templates
	//////////////////////////////////////////////////////////////////////////////
	LapBoardTemplate	m_LBTplt;	//The lapboard template


	//////////////////////////////////////////////////////////////////////////////
	//Crosshair dimensions
	//////////////////////////////////////////////////////////////////////////////
	double	m_dCrossHairWidth;		
	double	m_dCrossHairHeight;


	//////////////////////////////////////////////////////////////////////////////
	//Misc
	//////////////////////////////////////////////////////////////////////////////
	double				m_dRL[MP_SIZE];
	double				m_dAutoRunOffset;
	double				m_dDistanceInnerStandardToArc;
	double				m_dLastRunAngle;
	AutobuildDialog		*m_pAutobuildDialog;


private:
	void SetRLsForBayByPlane( Bay *pBay, Point3D ptRLPlane[3] );
	bool UseMillsSystemCorner();
	bool m_bBackDoorEntry;
	double m_dBirdCageOriginalWidthActual;
	int	m_iDrawWhenZero;
	void RemoveLapboardObsticals( Bay *pBay, double dAngle );

	bool m_bUseLastPoints;
	void InsetPoints();
	////////////////////////////////
	////////////////////////////////
	////////Hidden Functions////////
	////////////////////////////////
	////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////
	//Utilities
	//////////////////////////////////////////////////////////////////////////////
	void	SetGlobalsToDefault();
	void	SetClosePoint( Point3D ptClose );
	void	AllowForOrtho( Point3D &pt, int iPointToConsider );
	double	DistToAvoidBoundBox( bool &bUseBoundingBoxCorner, bool bErase, double &dAngle );
	Bay*	GetStandardByProxyToPoint(Point3D pt, CornerOfBayEnum &eCnr);
	Bay*	GetBayByInternalPoint( Point3D ptInternal, HitLocationEnum &eHit, double dGravity=0.00 );

	//////////////////////////////////////////////////////////////////////////////
	//Position Points for inidividual Bays
	//////////////////////////////////////////////////////////////////////////////
	void CreateBay( Point3D aptBay[BP_SIZE] );
	void CreateLapBoard( Point3D aptLapBrd[BP_SIZE] );

	
	//////////////////////////////////////////////////////////////////////////////
	//Draw collection items
	//////////////////////////////////////////////////////////////////////////////
	void DrawRun( bool bErase, bool bStoreLastPoint=false, bool bArc=false, int iStart=0, int iFin=0 );
	bool DrawRun( bool bErase, int iStart, int iFin, Point3D &ptNew2ndPoint, bool bStoreLastPoint=false, bool bArc=false );
	void DrawBirdcage( bool bErase );
	void DrawArcBays( bool bErase );
	void DrawBay( int iRunID, Point3D ptStart, Point3D ptOuterStd2Direction,
					 bool bErase, double dRemainder, bool bTie=false, bool bArc=false );
	void DrawLapboard( Point3D ptStart, Point3D ptOuterStd2Direction,
						 bool bErase, bool bCreateSingleLap=false );
	void DrawTieTube( Point3D pt1, Point3D pt2, bool bErase );

	
	//////////////////////////////////////////////////////////////////////////////
	//Drawing Individual items
	//////////////////////////////////////////////////////////////////////////////
	void DrawCrosshair( Point3D ptCross, double dWidth, double dHeight, bool bErase );
	void DrawCrosshairDebug( Point3D ptCross, bool bErase, int iColour );
	void DrawPolyLine( Point3DArray pts, int iColour );
	void DrawCircle(Point3D pt, double dRadius, int iColour, int iHL=0 );
	

	//////////////////////////////////////////////////////////////////////////////
	//Customised AcadFunction type
	//////////////////////////////////////////////////////////////////////////////
	void DrawLine( Point3D pt1, Point3D pt2, int iColour, int iHL=0 );
	int GrDraw( Point3D pt1, Point3D pt2, int iColour, int iHl=0 );
	int GetPoint3D( Point3D *pptUCS, CString sPrompt, Point3D &ptPoint );

	
	//////////////////////////////////////////////////////////////////////////////
	//Others
	//////////////////////////////////////////////////////////////////////////////
	void GetInnerStandardPos( double dDistance, double dLength, Point3D ptStart, Point3D &ptStandard );
	void ReturnToWallMode();
};


#endif // !defined(AFX_AUTOBUILDTOOLS_H__3B04BEB4_485B_11D3_8861_0008C7999B1D__INCLUDED_)
