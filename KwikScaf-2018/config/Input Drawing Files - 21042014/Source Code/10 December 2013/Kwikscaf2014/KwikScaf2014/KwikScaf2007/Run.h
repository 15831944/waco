// Run.h: interface for the Run class.
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

#include "GraphicTypes.h"	// Added by ClassView
#if !defined(AFX_RUN_H__9F66E164_43A6_11D3_885D_0008C7999B1D__INCLUDED_)
#define AFX_RUN_H__9F66E164_43A6_11D3_885D_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								RUN CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 13/12/00 11:11a $
//	Version			: $Revision: 58 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This class was really added on as an after thought, I do believe we currently
//	have enough information to be able to produce a workable system, however
//	there may be a several of benefits to using this class!
//1. Storage of RL values, so that we can at a later stage, say during an edit
//		bay process be able to prompt the user to reapply RL calculations to
//		modifications.
//2. Control the lists of Bays
//3. Unique Run Number
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeccanoDefinitions.h"
#include "Bay.h"

enum	WhichPointEnum
{
	WP_START,
	WP_END
};

class Controller;
class RunTemplate;

enum FixedDirectionEnum
{
	FIXED_DIRECTION_INNER,
	FIXED_DIRECTION_OUTER,
	FIXED_DIRECTION_BOTH,
	FIXED_DIRECTION_NEITHER
};

class Run  
{
public:
	double GetBracingRaise( double dBayLength ) const;
	void SetLongerBracingValues();
	void SetBracingLength(double dBayLength, double dLength );
	double GetBracingLength( double dBayLength ) const;
	SystemEnum GetSystem() const;
	doubleArray GetAvailBraceLengths( double dBayLength );
	bool IsAnyBaySelected();
	void SetAllComponentsVisible();
	bool GetUseLongerBracing() const;
	void SetUseLongerBracing( bool bUseLonger );
	void ClearAllowDraw();
	void VisitRun( Matrix3D Transform );
	bool GetRunVisited();
	void SetRunVisited( bool bVisited );
	void SetHasBeenVisited( bool bVisited );
	void SetAllowDrawBays( bool bAllow );
	void MoveAllNorthernRuns(Matrix3D Movement, int iBaysToConsider, bool bStore);
	int GetAutobuildNumber();
	void SetAutobuildNumber();
	int m_iAutobuildNumber;


	//Movement of Bays and runs to allow for deleted bays
	void ShuffleSubsequentBays( int iBayID, double dDistanceToMove, bool bOnlyThisRun=true );
	void MoveStartOfRunBackward( double dDistanceToMove );

	Point3D GetPointStart() const;
	Point3D GetPointEnd() const;
	void SetPointStart(Point3D pt);
	void SetPointEnd(Point3D pt);
	Point3D GetPoint(int iPointNumber) const;
	void SetPoint(int iPointNumber, Point3D pt );
	bool SetRunFit(RunFitEnum eFit);

	bool SetTiesIfPossible( Bay *pBay, double dHeight, double dLength, MaterialTypeEnum eMT );
	bool SetTieIfPossible( Bay *pBay, Bay *pNeighbor, bool bTemplateTie, SideOfBayEnum eSide, double dLength, double dHeight, MaterialTypeEnum eMT );

	void AlignStandards( );

	double FindWidthOfSouthernStageboard();
	double FindWidthOfLongestSouthernStageboard();
	double FindWidthOfBottomSouthernStageboard();

	void GetRLExtentsForAllBays( double &dRLTop, double &dRLBottom );
	void MovePreviousBays( int iBayToStartWith, Matrix3D Transform, bool bStore );
	void MoveSubsequentBays( int iBayToStartWith, Matrix3D Transform, bool bStore, bool bMoveConnectedRuns=true );
	void ReIDSubsequentBays( int iBayIdToStartAt );
	void SetBaysWithBracing( bool bRemoveExisting=true );
	void SetBaysWithTies( bool bRemoveExisting=true, bool bRemoveLastTie=false );
	RunTemplate *GetRunTemplate() const;
	void CreatePascalsTriangle(int iBaysPerItem, int iTotalNumberOfBays, intArray &iaBays );
	void UpdateSchematicView( bool bOnlyUpdateVisited=false, double dProgress=-1.00, double dProgSpan=-1.00 );
	Matrix3D UnMoveSchematic();
	Matrix3D GetSchematicTransform() const;
	void SetSchematicTransform( Matrix3D Transform );
	void MoveSchematic( Matrix3D Transform, bool bStore, int iFixedDirection=FIXED_DIRECTION_BOTH );
	//////////////////////////////////////////////////////////////////
	//Has the been marked for deletion
	//////////////////////////////////////////////////////////////////
	//Says this object needs to be deleted
	DirtyFlagEnum	GetDirtyFlag() const;
	//set the object to be deleted
	void	SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward=false );
	//delete all objects, that this object owns, if they are marked
	//	for deletion
	bool	CleanUp(int &iPos);

	Matrix3D CalculateRunTransform( Point3D pt1st, Point3D pt2nd );
	Matrix3D UnMove();
	Matrix3D GetTransform() const;
	void SetTransform( Matrix3D Transform );

	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	///////////////////////////////////////////////////////////////////////////////
	void Serialize(CArchive &ar);
	
	////////////////////////
	////////////////////////
	/////Functionality//////
	////////////////////////
	////////////////////////

	//This function is used to create a default run object containing one
	//	simple Bay.  This would mainly be used by the Controller class to
	//	create the template run object.  from there on in, all Runs would be a
	//	copy of the existing template, and so would not need a CreateRun
	//	function.
	Run();
	void CreateEmptyRun( );

	//Delete all elements of a run.  This fuction should be called by the
	//	controller class
	virtual ~Run();
	bool DeleteRun( );

	//Posts a message to the Controller to tell it to remove this run.
	void RemoveRun( );

	//retrieves the number of bays in the run
	int GetNumberOfBays() const;

	///////////////////////////////////////////////////////////////////////////////
	//Controller pointer
	///////////////////////////////////////////////////////////////////////////////

	//Access to the controller pointer
	Controller *GetController( ) const;
	void SetController( Controller *pController );


	///////////////////////////////////////////////////////////////////////////////
	//Move/Copy etc
	///////////////////////////////////////////////////////////////////////////////

	//Moves this run.  Don't forget to notify the lapboards that the run has
	//	been moved!
	void Move( Matrix3D Transform, bool bStoreMovement=true, int iFixedDirection=FIXED_DIRECTION_BOTH );

	//Copies this run, a copy command will always use a Move command so that
	//	the two copies are not sitting on top of one another.
	Run *CopyRun( Matrix3D Transform, bool bCopyInnerOuterPointers, bool bMove=true );

	Run &operator =(const Run &OriginalRun );
	bool operator==(const Run &OtherRun ) const;
	bool operator!=(const Run &OtherRun ) const;

	///////////////////////////////////////////////////////////////////////////////
	//Distance to wall
	double GetDistanceFromWall() const;
	void SetDistanceToWall( double dDistance );

	///////////////////////////////////////////////////////////////////////////////
	//RLs
	//Sets/Gets the RL for a Run, this should then be used to determine the
	//	Height of each bay in the Run.
	bool	SetRL( Point3D ptRLBegin, Point3D ptRLEnd, bool bAdjustBayHeights=true );

	//retrieves the RLs for the begining and ending points of this run
	bool	GetRL( Point3D &RLBegin, Point3D &RLEnd ) const;

	void SetTemplateRLEnd( double dRL );
	void SetTemplateRLStart( double dRL );

	double GetTemplateRLEnd() const;
	double GetTemplateRLStart() const;


	///////////////////////////////////////////////////////////////////////////////
	//RunID
	///////////////////////////////////////////////////////////////////////////////

	//Queries the Controller class to obtain its number in the RunList
	//This number would be translated into a letter from 'A' through to 'ZZZZZ…'.
	bool	SetRunID( int iRunID );
	int		GetRunID( ) const;


	///////////////////////////////////////////////////////////////////////////////
	//Joins/Splits
	///////////////////////////////////////////////////////////////////////////////

	//This function has to consider the following functionality, some of this
	//	functionality belongs to the Run Class, some belongs to the Bay class:
	//	Break Links
	//		Break Forward Pointer
	//		Break Backward Pointer
	//		Break Outer Pointer
	//		Bread Inner Pointer
	//	Tell Run object to recalc RL's for the new split points
	//	Create a new Run and add to the list of Runs
	//	Move Split Bays from this run to the new run (actually uses copy to
	//	create a new bay and then delete to remove copied bay).
	Run *SplitRun( int iBayID );
	
	//this function will join another run to the end of this run, and make them
	//	into one run.  The major issue with this function will be how to set
	//	the RL's for this new, longer, Run.  Some other considerations will be
	//	moving the second run to meet the first, testing the Transom lengths
	//	are the same and other rules between bays, so it is possible for this
	//	operation to fail.  Naturally the runs will have to follow on from one
	//	another, so a Move command will be needed to perform it's duties
	bool	JoinRun( Run *pRun, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );
	bool	JoinRun( int iRunID, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );


	///////////////////////////////////////////////////////////////////////////////
	//Surface Area
	///////////////////////////////////////////////////////////////////////////////

	//call all the bays in turn to find their surface area.
	double GetSurfaceArea( ) const;


	///////////////////////////////////////////////////////////////////////////////
	//BOM
	///////////////////////////////////////////////////////////////////////////////

	//used to create the BOM
	void WriteBOMTo( CStdioFile *pFile ) const;

	
	///////////////////////////////////////////////////////////////////////////////
	//Bay Stuff
	///////////////////////////////////////////////////////////////////////////////

	//Inserts a bay into a run after the specified location.  This will be an
	//	'AtGrow' function
	//needs to consider repositioning of RL and End point.  Need to check that
	//	the bay to insert before exists within the run.
	Bay		*InsertBay( Bay *pBay, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );
	Bay		*InsertBay( int iBayID, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );
	bool	InsertBay( int iBayID, Bay *pNewBay, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );

	//retrieves a pointer to a bay
	Bay		*GetBay( int BayID ) const;

	//retrieves the number of the bay from the pointer
	int		GetID( Bay *pBay ) const;

	//Removes a bay from the specified run, and then tells each of the
	//	following Bays to shift closer to fill the void.
	bool	DeleteBays(int iBayID, int iCount=1, bool bShuffleSubsequentBays=true );
	bool	EraseBays( int iBayID, int nCount );


	//Adds a bay and sets all the appropriate pointes and numbers for that bay
	int AddBay( Bay *pBay, bool bAdjustStandards=true, bool bSetBayPointers=true );

	//Sets the visibility all components of the run
	void SetVisible( VisibilityEnum eVisible=VIS_VISIBLE );

	//finds the visibility for all components of the run, if they are the
	//	same, otherwise it return MULTISELECT or some other constant
	VisibilityEnum GetVisible() const;

	//Tells the components of the run to draw themselves
	bool Create3DView( int &iPos );
	void Delete3DView();

	// access functions needed by the dialog box
	double			GetTiesVerticallyEvery();
	TieTypesEnum	GetTieType();
	int				GetBaysPerBrace();
	int				GetBaysPerTie();

	void SetTiesVerticallyEvery(double dEvery);
	void SetTieType(TieTypesEnum eTieType);
	void SetBaysPerBrace(int dBracingEvery);
	void SetTiesEvery(int dTiesEvery);

protected:
	bool m_bUseLongerBracing;
	bool m_bRunVisited;
	////////////////////////
	////////////////////////
	//////Attributes////////
	////////////////////////
	////////////////////////


	///////////////////////////////////////////////////////////////////////////////
	//General
	///////////////////////////////////////////////////////////////////////////////
	//This is the unique run ID assigned by the controller
	int				m_iRunID;
	//These are the bays that are owned by this run
	BayList			m_baBays;
	//pointer to the controller object
	Controller		*m_pController;
	//The Run template
	RunTemplate		*m_pRTplt;

	///////////////////////////////////////////////////////////////////////////////
	//General
	//Transformation maticies
	Matrix3D		m_SchematicTransform;
	Matrix3D		m_Transform;


	///////////////////////////////////////////////////////////////////////////////
	//Ties - See runtemplate for tie settings
	
	//does this object need to be deleted
	DirtyFlagEnum	m_dfDirtyFlag;

private:
	bool JoinOuter( Bay *pSelectedBay, Bay *pNewBay);
	bool JoinInner( Bay *pSelectedBay, Bay *pNewBay );
	bool JoinBackward( int iBayID, Bay *pSelectedBay, Bay *pNewBay );
	bool JoinForward( int iBayID, Bay *pSelectedBay, Bay *pNewBay );

	////////////////////////
	////////////////////////
	////Hidden Functions////
	////////////////////////
	////////////////////////

	//inserts a bay attached to another bay, according to the JoinType
	Bay *InsertBay( Bay *pBay, Bay*pNewBay, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );

	//deterimines if the bay ID is within bounds
	bool IsBayIDValid( int iBayID ) const;

	//sets the member variables to 'empty' values
	void SetGlobalsToDefault();
};



#include <afxtempl.h>
//#include <acarray.h>
class RunList : public CArray<Run*,Run*>
{
public:
	RunList();
	~RunList();
};


#endif // !defined(AFX_RUN_H__9F66E164_43A6_11D3_885D_0008C7999B1D__INCLUDED_)



/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Run.h $
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 13/12/00   Time: 11:11a
 * Updated in $/Meccano/Stage 1/Code
 * about to create 1.1o
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 15/06/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 2/06/00    Time: 3:02p
 * Updated in $/Meccano/Stage 1/Code
 * need to find why tietubes are removing hopups
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 14/12/99   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to build Beta11
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 18/11/99   Time: 3:45p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on the Run Properties dialog
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:54a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 27/10/99   Time: 2:49p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Mesh Guard now working
 * 2) Ties nearly working
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 22/10/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Birdcaging pointers now fully operational
 * 2) Birdcaging now uses pascal's triangle for End of runs
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 21/09/99   Time: 5:25p
 * Updated in $/Meccano/Stage 1/Code
 * Almost finished the insert bay
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Dar          Date: 9/07/99    Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 11  *****************
 * User: Dar          Date: 8/12/99    Time: 3:32p
 * Updated in $/Meccano/Stage 1/Code
 * fixed double type for bracing every and ties every bay to ints
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 8/12/99    Time: 2:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 29/07/99   Time: 10:05
 * Updated in $/Meccano/Stage 1/Code
 * I have now added a test routine to the controller which calls the
 * _T("LiftList.Test") function
 * I have added operator functions to the Run class
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 28/07/99   Time: 15:55
 * Updated in $/Meccano/Stage 1/Code
 * Continueing with Run class
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 27/07/99   Time: 14:44
 * Updated in $/Meccano/Stage 1/Code
 * Added the Controller class framework
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 27/07/99   Time: 12:19
 * Updated in $/Meccano/Stage 1/Code
 * I can finally compile with only 8 warnings
 *
 *******************************************************************************/
