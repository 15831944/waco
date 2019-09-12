// Bay.h: interface for the Bay class.
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

#if !defined(AFX_BAY_H__E6E8AA84_3A32_11D3_884B_0008C7999B1D__INCLUDED_)
#define AFX_BAY_H__E6E8AA84_3A32_11D3_884B_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								BAY CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:31p $
//	Version			: $Revision: 149 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This Class is the Cen+tral Spoke for most of this program, we will be
// basically taking a Bay-centric approach in the Meccano project.  This
// class with some changes can be used to handle Stairs, where the rules
// are slightly different
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeccanoDefinitions.h"
#include "Component.h"
#include "LiftList.h"
#include "doubleArray.h"

//forward declarations
class LapboardBay;
class LapboardList;
class Matrix;
class MatrixElement;
class Run;
class BayDetailsDialog;
class AcDbGroup;
class ComponentDetailsArray;
class BayList;

#include "PreviewTemplate.h"	// Added by ClassView
#include "BayDefinitions.h"	// Added by ClassView
#include "GraphicTypes.h"	// Added by ClassView
//#include "MatrixElement.h"	// Added by ClassView

class Bay
{
public:
	void RecreateLapsBoardsAndTieTubes( BayList *pBays=NULL );
	bool IsBaySelected( );
	void SetSWStandardEntityID( AcDbObjectId ID );
	AcDbObjectId GetSWStandardEntityID();
	double JackLength() const;
	double RLAdjust() const;
	void SetStarSeparation( double dSeparation );
	double GetStarSeparation() const;
	SystemEnum GetSystem( ) const;
	void SetSystem( SystemEnum eSystem );
	void SetAllComponentsVisible();
	bool m_bAllowRecreateStandards;
	HandrailStyleEnum GetHandrailStyle( SideOfBayEnum eSide, bool bCheckNeighborIfNone=false );
	void CreateCutThrough( Matrix *pCutThrough, SideOfBayEnum eSide );
	void AddHandrailToNeighborAboveRL( double dRL, Bay *pNeighbor, SideOfBayEnum eNeighborSide );

	//Tie tube creation;
	void RecreateAllTieTubesFromTemplate();
	void CreateTieTubeFromTemplate( BayTieTubeTemplate *pTTT, bool bAddToTemplate=true );
	void CreateTieTube( double dRL, CornerOfBayEnum eCnr, double dFullLength, Matrix3D Transform3D, Matrix3D Rotation, bool bCreateMeshGuard=false, bool bCreateToeboard=false, bool bHasLapboard=false );
	void	SetTieTubeVerticalTolerance( double dTolerance );
	double	GetTieTubeVerticalTolerance();

	int GetLowestHopupLiftID();
	void ClearAllowDraw();
	void Visit( Matrix3D Transform, bool bMoveRun=false );
	void SetHasBeenVisited( bool bVisited );
	bool HasBeenVisited();

	SideOfBayEnum GetSideFromMillsType( MillsType mtMillsTypeValue );

	//Roof Protection
	SideOfBayEnum FindInvalidRoofProtectionStandards( ) const;
	void RemoveRoofProtectionComponents( bool bExceptTopLift=true );
	void AddRoofProtectionComponents();
	void SetRoofProtection( RoofProtectionEnum eRP );
	RoofProtectionEnum GetRoofProtection( ) const;

	//////////////////////////////////////////////////////////////////////////////
	//Mill system corner stuff
	MillsType GetMillsSystemType() const;
	void SetMillsSystemBay( MillsType mtMillsCnrType );
	void SetMillsTypeDumb( MillsType mtMillsCnrType );
	void CreateComponentsForMillsCnrType(SideOfBayEnum eSideToRestore);
	bool RemoveMillsTypeStandard( SideOfBayEnum eSide );
	bool HasMillsTypeMissingStandard( SideOfBayEnum eSide ) const;
	bool SetMillsTypeGap( SideOfBayEnum eSide );
	bool HasMillsTypeHalfSize( SideOfBayEnum eSide ) const;
	bool HasMillsTypeGap( SideOfBayEnum eSide ) const;
	void ClearMillsType();
	bool AddMillsType( MillsType mtType );
	bool IsMillsTypeBay() const;
	void SetCanBeMillsCnrBay( MillsType mtCanBeType );
	MillsType CanBeMillsCnrBay();

	//////////////////////////////////////////////////////////////////////////////
	//Sydney corner bay stuff
	bool IsSydneyCornerBay();
	void SetSydneyCornerBay( bool bIsSydneyCornerBay );

	void BraceExposedSides();
	bool DeleteUnneededComponentsFromEmptyLifts( SideOfBayEnum eSide );
	bool DoesLiftHaveNeighborAtSameRL( int iLiftID, Bay *pNeighboringBay, bool bInclude500UpOrDown=true  ) const;
	void SetHasMovingDeck( bool bMovingDeck );
	bool HasMovingDeck() const;
	void EnsureStandardsAreTallEnough( SideOfBayEnum eSide );
	void ReapplyWallTiesFromTemplate();
	void ReapplyBracingToSide( SideOfBayEnum eSide );
	Bay * GetBayThatOwnsSide( SideOfBayEnum eThisBaySide, SideOfBayEnum &eOtherBaySide ) const;
	CommittedProportionEnum GetCommitProportionForComponentOnSide( ComponentTypeEnum eType, SideOfBayEnum );
	CommittedProportionEnum GetCommittedProportion( );
	////////////////////
	////////////////////
	////Functionality///
	////////////////////
	////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	//construction/destruction
	/////////////////////////////////////////////////////////////////////////////////

	Bay();
	
	virtual			~Bay();

	//Creates a simple bay
	void			CreateBay();

	//This is really a method to handle the RL adjustments.  This function would be used for a few reasons:
	//Add empty lifts, to the bottom of the Bay, where each lift should be 2m high, if possible.
	//Delete lifts, from the bottom, to adjust for RL.
	void			AdjustBayHeight( double dNewHeight );

	//////////////////////////////////////////////////////////////////
	//Has the been marked for deletion
	//////////////////////////////////////////////////////////////////
	//Says this object needs to be deleted
	DirtyFlagEnum	GetDirtyFlag() const;
	//set the object to be deleted
	void			SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward=false );

	//delete all objects, that this object owns, if they are marked
	//	for deletion
	void			CleanUp();


	/////////////////////////////////////////////////////////////////////////////
	//Other functions
	/////////////////////////////////////////////////////////////////////////////
	double			GetSmallestRequiredWallTieLength(SideOfBayEnum eSide);
	void			AlignBay( double dRequiredWidth );
	void			TraceContents( CString sComment, bool bBayCompsOnly=false, int iLift=ID_INVALID ) const;

	int				GetNumberOfPlanksOfStageFromWall( SideOfBayEnum eSide );
	int				GetNumberOfPlanksOfLongestStage( SideOfBayEnum eSide );
	int				GetNumberOfPlanksOfBottomStage( SideOfBayEnum eSide );

	double			GetWidthOfStageFromWall( SideOfBayEnum eSide );
	double			GetWidthOfLongestStage( SideOfBayEnum );
	double			GetWidthOfBottomStage( SideOfBayEnum eSide );

	///////////////////////////////////////////
	//Ledgers every metre
	bool			GetUseLedgerEveryMetre() const;
	void			SetUseLedgerEveryMetre( bool bUse );

	///////////////////////////////////////////
	//Midrail with chainmesh
	bool			GetUseMidrailWithChainMesh() const;
	void			SetUseMidrailWithChainMesh( bool bUse );


	/////////////////////////////////////////////////////////////////////////////
	//Serialize
	/////////////////////////////////////////////////////////////////////////////
	void			Serialize( CArchive &ar );


	/////////////////////////////////////////////////////////////////////////////
	//Stairs/Ladders
	/////////////////////////////////////////////////////////////////////////////
	bool			ChangeToLadderBay();
	bool			ChangeToStairBay();
	bool			ChangeToButtressBay();
	void			AddSpecialStairsLaddersComponentsToAllLifts();
	bool			ChangeComponentsForLadderBay();
	bool			ChangeComponentsForStairBay();
	bool			ChangeComponentsForButtressBay();
	void			DeleteObsticales( Bay *pNeighboringBay, SideOfBayEnum eSide );
	void			DeleteAccessObsticles();
	void			AdjustNumberOfLifts( int iRequiredNumber );


	/////////////////////////////////////////////////////////////////////////////
	//RL Stuff
	/////////////////////////////////////////////////////////////////////////////
	bool			SetAndAdjustRLs( double dRLs[4], bool bAutobuild=false );
	bool			GetRLsForBayByStandards(double &dMinRL, double &dMaxRL);
	bool			GetRLsForBayByExistingLifts( double &dRLBottom, double &dRLTop );
	void			GetRLsForStandard(CornerOfBayEnum eCnr, double &dRLBottom, double &dRLTop);
	int				GetNumberOfLiftsForBay( LiftRiseEnum eLiftRise, double dHeight);
	void			SetRisesForAllLifts( LiftRiseEnum eRise );
	void			SetRLsForAllLifts( double dRLBottom );
	bool			HasComponentAtRL(ComponentTypeEnum eType, double dRL, SideOfBayEnum eSide);
	Component*		GetComponentAtRL( ComponentTypeEnum eType, double dRL, SideOfBayEnum eSide );
	void			FindAllRLsForEachCornerOfThisBayByLifts( double dRLMax[4], double dRLMin[4] );
	bool			GetRLsForBayByHeights(double &dMinRL, double &dMaxRL, bool bIncludeComponents=true, bool bIncludeJacksSoleBoards=true );

	
	/////////////////////////////////////////////////////////////////////////////
	//Controller
	/////////////////////////////////////////////////////////////////////////////
	void			SetController( Controller *pController );
	ComponentDetailsArray*	GetCompDetails() const;
	Controller*		GetController() const;


	/////////////////////////////////////////////////////////////////////////////
	//Schematic View Stuff
	/////////////////////////////////////////////////////////////////////////////
	void			CreateSchematic();
	Matrix3D		GetSchematicTransform() const;
	void			MoveSchematic( Matrix3D Transform, bool bStore );
	Matrix3D		UnMoveSchematic();
	void			UpdateSchematicView();
	void			SetSchematicTransform( Matrix3D Transform );


	/////////////////////////////////////////////////////////////////////////////
	//Bay Type
	/////////////////////////////////////////////////////////////////////////////
	BayTypeEnum		GetBayType() const;
	bool			SetBayType( BayTypeEnum BayType );


	/////////////////////////////////////////////////////////////////////////////
	//Bay Template
	/////////////////////////////////////////////////////////////////////////////
	BayTemplate *GetTemplate() const;
	void SetTemplate( BayTemplate *pTemplate );


	/////////////////////////////////////////////////////////////////////////////////
	//Bay dimensions
	/////////////////////////////////////////////////////////////////////////////////

	//retrieves the dimensions of the bay, this is the 'common' dimension
	double			GetBayWidth() const; 
	double			GetBayLength() const;
	
	//finds the actual dimensions of the bay, NOT the 'common' dimensions
	double			GetBayLengthActual() const;
	double			GetBayWidthActual() const;

	void			ApplyBayResize(bool bChangeComponents=true);

	//Adjusts the dimensions of the bay
	bool			SetBayWidth( double dNewWidth, bool bAllowMovement=true );
	bool			SetBayLength( double dNewLength, bool bAllowMovement=true );
	void			SetBayWidthDumb( double dNewWidth );
	void			SetBayLengthDumb( double dNewLength );
	bool			IsBaySizeMultipleSelection();
	void			SetBaySizeMultipleSelection( bool bDifferentSizes );
	void			SetBayLengthChangedDuringEdit( bool bResized );
	void			SetBayWidthChangedDuringEdit( bool bResized );
	bool			WasBayLengthChangedDuringEdit();
	bool			WasBayWidthChangedDuringEdit();

	void ResizeAllBaysInDirection( double dNewSize, SideOfBayEnum eSide );

	//Previous dimension - used when dimension changes occur
	double			GetPreviousWidth();
	double			GetPreviousLength();
	void			SetPreviousLength2( double dLength );
	void			SetPreviousWidth( double dWidth );


	/////////////////////////////////////////////////////////////////////////////////
	//Visibility/Drawing
	/////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////
	//sets the visibility for the component of this bay, inc Lift components
	void			SetVisible( VisibilityEnum eVisible=VIS_VISIBLE );

	//determines the visibility of this bay, if all components have the same
	//	visibility setting then it ruturns that, otherwise it returns MULTI_SELECT
	VisibilityEnum	GetVisible();

	//Tells the components of the bay to draw them selves
	bool			Create3DView();
	void			Delete3DView();

	void			SetAllowDraw( bool bAllow );
	bool			IsDrawAllowed();


	/////////////////////////////////////////////////////////////////////////////////
	//Run
	/////////////////////////////////////////////////////////////////////////////////

	//returns the run associated with this Bay
	Run *GetRunPointer() const; 
	
	//set the run pointer for this Bay
	void SetRunPointer( Run *pRun );


	/////////////////////////////////////////////////////////////////////////////////
	//ID
	/////////////////////////////////////////////////////////////////////////////////

	//called by Run Class to set ID
	void			SetID( int iNewID );

	//Queries the list of Bays for its index in that list (Ie the Bay number is
	//	simply its position in the Bay list, which is owned by the Run). (1
	//	based number)
	int				GetID() const;

	//Adjust the displayed text that Sequencially numbers this bay
	void			UpdateNumberText();

	//Sequencial numbering for bays
	int				GetBayNumber() const;


	/////////////////////////////////////////////////////////////////////////////////
	// Dialog box
	/////////////////////////////////////////////////////////////////////////////////

	void			Show();

	//There are 3 ways changes made through the dialog box could be handled:
	void			ApplyChanges();
	
	//rewind back to the undo mark (see above)!
	void			CancelChanges();


	/////////////////////////////////////////////////////////////////////////////////
	//Pointers to Neighbors
	/////////////////////////////////////////////////////////////////////////////////
	Bay*			GetBackward() const;
	Bay*			GetForward() const;
	Bay*			GetOuter() const;
	Bay*			GetInner() const;
	LapboardBay*	GetLapboard( int iIndex ) const;
	LapboardBay*	GetLapboard( SideOfBayEnum eSide ) const;
	void			ReapplyBayPointers( bool bAdjustStandards=true );
	void			ClearAllBayPointers( bool bIgnorNulls=true, bool bAdjustStandards=true );

	void			SetBackward( const Bay *pBay, bool bIgnorNulls=true, bool bAdjustStandards=true );
	void			SetForward( const Bay *pBay, bool bIgnorNulls=true, bool bAdjustStandards=true );
	void			SetOuter( const Bay *pBay, bool bIgnorNulls=true, bool bAdjustStandards=true );
	void			SetInner( const Bay *pBay, bool bIgnorNulls=true, bool bAdjustStandards=true );
	void			SetOuterDumb( const Bay *pBay );
	void			SetInnerDumb( const Bay *pBay );
	void			SetBackwardDumb( const Bay *pBay );
	void			SetForwardDumb( const Bay *pBay );

	//Joins one bay to another, by setting the appropriate pointer.  It also
	//	owns a UI to perform this task.  The UI to do this has not been worked
	//	out yet.  It maybe able to do this by looking at various proximities
	//	value, or at least testing for proximities
	//void JoinBay( Bay *, enum JoinType/JoinDirection );

	//Joins one bay to a lapboard, by setting the appropriate pointer.  It also
	//	owns a UI to perform this task.  The UI to do this has not been worked
	//	out yet.  It maybe able to do this by looking at various proximities
	//	value, or at least testing for proximities
	//void JoinLapboard( Lapboard *, enum JoinType/JoinDirection );


	/////////////////////////////////////////////////////////////////////////////////
	//Lapboard Array stuff
	/////////////////////////////////////////////////////////////////////////////////
	void			AddLapboardPointer( LapboardBay *pLapboard, SideOfBayEnum eSide );
	void			RemoveLapboardPointer( const LapboardBay *pLapboard );
	void			RemoveLapboardPointer( int iIndex );
	int				FindLapboardIndex( LapboardBay *pLapboard );

	int				GetNumberOfLapboardPointers() const;
	void			RecreateAllLapboards(BayList *pBays=NULL);

	//lapboard crosses side of bay
	void			LapboardCrossesSideAdd( SideOfBayEnum eSide );
	void			LapboardCrossesSideRemove( int iIndex );
	SideOfBayEnum	LapboardCrossesSideGet( int iIndex ) const;
	bool			LapboardCrossesSideDoes( SideOfBayEnum eSide ) const;
	int				LapboardCrossesSideWhich( SideOfBayEnum eSide ) const;
	void			LapboardCrossesSideRemoveAll();


	/////////////////////////////////////////////////////////////////////////////////
	//Lifts
	/////////////////////////////////////////////////////////////////////////////////

	//The number of lifts in the LiftList
	int				GetNumberOfLifts() const;

	//returns a lift given an ID
	Lift*			GetLift ( int iLiftID ) const;

	//Finds which lift occupies at a particular height (RL) from the height
	//	stored in the LiftList, checks height of Standards first.  Since the
	//	Height passed to us may not necessarily be the actual absolute height
	//	for the bottom of the lift, we may need some way of returning the
	//	actual height of the bottom of the lift, this would be useful to
	//	determine the height, relative to the lift, of a certain star, since
	//	we may know the absolute height of a star we need to add some
	//	component to.
	Lift*			GetLiftAtRL( double dRL ) const;
	int				GetLiftIDAtRL( double dRL ) const;

	bool			ReIDAllLifts();

	//Same as above except that the height is relative to the bottom transom
	//	/ledger combo
	Lift*			GetLiftAtHeight( double dHeight ) const;
	int				GetLiftIDAtHeight( double dHeight ) const;

	//retrieves the height of an individaul lift (0m-3m), relative to the bottom
	//	set of transoms and ledgers owned by the lift itself.  This function can
	//	also find the height of all lifts, if the heights are all the same.
	//	possible return values are from LiftRiseEnum
	LiftRiseEnum	GetLiftRaise( int iLiftID ) const;

	//find the RL for a lift
	bool			GetLiftRL( int iLiftID, double &dRL ) const;


	//find the Height above the bottom transom/ledger
	bool			GetLiftHeight( int iLiftID, double &dHeight ) const;

	//finds the RL of a particular lift
	double			GetRLOfLift(int iLiftID) const;

	//retrieves the rise(height) of the lift reletive to the bottom of the lift
	LiftRiseEnum	GetLiftRise( int iLiftID ) const;

	//find the component within a lift or for a lift at a particular height.
	//	Returns LIFT_TRUE, LIFT_FALSE or LIFT_MULTI_SELECT
	bool			GetLiftComponent( int iLiftID, ComponentTypeEnum eComponentType,
							SideOfBayEnum eSide, int iRise,
							Component **pComponent ) const;
	bool			GetLiftComponent( double dHeight, ComponentTypeEnum eComponentType,
							SideOfBayEnum eSide, int iRise,
							Component **pComponent ) const;

	//retrieves the surface area for the given direction (N,E,S,W).  If
	//	ALL_VISIBLE_SIDES retrieves the surface area using logic to
	//	determine which sides are exposed, it can do this by looking at the
	//	forward, backward and outer pointers.
	double			GetSurfaceArea( SideOfBayEnum eSide=ALL_VISIBLE_SIDES  ) const;
	double			GetFaceHeight( SideOfBayEnum eSide ) const;
	double			GetFaceWidth( SideOfBayEnum eFace ) const;

	//Adds a lift to the lift array and sets the appropriate pointers and number
	int				AddLift( Lift *pLift );

	//Inserts a lift, renumbers and moves all lifts above the new lift
	bool			InsertLift( int iAtLift, Lift *pLift, bool bMoveLiftsUp=true, bool bAdjustStandards=true );

	//Deletes a lift, renumbers and moves all lifts above the deleted lift
	bool			DeleteLift( int iLiftID, bool bMoveLiftsAboveDown=true );

	//deletes a lift, and notifies the bay that it has been deleted
	bool			RemoveLift( int iLiftID, int iCount=1 );

	//Movement
	bool			MoveLift( int iLiftID, bool bUp );
	bool			MoveLiftDown( int iLiftID );
	bool			MoveLiftUp( int iLiftID );


	/////////////////////////////////////////////////////////////////////////////////
	//Standards
	/////////////////////////////////////////////////////////////////////////////////

	//retrieves the height of a standard
	double			GetHeightOfStandards( CornerOfBayEnum eStandardsName=CNR_NORTH_EAST ) const;
	
	//sets the height and RL of the Standards
	bool			SetHeightOfStandards( Point3D dRL, double dHeight, CornerOfBayEnum eStandardSide=CNR_NORTH_EAST );

	double			GetHeightOfBay() const;
	
	//Retrieves a standard from the ordered list of standards
	Component*		GetStandard( int iStandardPosition, CornerOfBayEnum eStandardSide ) const;

	//retrieves an ORDERED array of lengths for each Standard, ordered from
	//	ground to top!
	bool			GetStandardsArrangement( doubleArray &daArrangement, CornerOfBayEnum eStandardsName=CNR_NORTH_EAST ) const;
	
	//replaces existing arrangement with new ordered arrangement (Ground to
	//	Top).
	bool			SetStandardsArrangement( doubleArray &daArrangement, CornerOfBayEnum eStandardsName=CNR_NORTH_EAST, bool bSoleboard=false );
	
	// This is used to automatically create a set of standards, the lengths
	//	used will depend upon the loca tion of the standards.  The default
	//	argument would also look at which standards are owned by the bay, and
	//	determine a height for that bay.  Also should create jacks.
	bool			CreateStandardsArrangment( double dHeight, CornerOfBayEnum eStandardsName=CNR_NORTH_EAST, bool bSoleboard=false );

	//creating standard functions
	void			AutoAdjustStandard( CornerOfBayEnum eCnr );
	void			AdjustStandardSmart(CornerOfBayEnum eCnr, doubleArray &daArrangement, bool bSoleBoard, BayList *pBays=NULL);
	void			MakeStandardsArrangment( doubleArray &daArrangement, double dHeight, bool bInnerStandard, bool bStaggered, StandardFitEnum eStandardsFit );

	//destroys the standards for a set of standards
	bool			DeleteStandards( CornerOfBayEnum eStandardsName );

	//finds the position of the bottom of a standard
	Point3D			GetStandardPosition( CornerOfBayEnum eCorner ) const;

	//set the position of the bottom of a standard
	bool			SetStandardPosition( Point3D ptStandard, CornerOfBayEnum eCorner );

	//Find the RL of a lift including the handrail if it has one
	double			GetRLOfTopLiftPlusHandrail( SideOfBayEnum eSide ) const;

	//adjusts the height of the standards if they are too short,
	//	such that they will support the lifts
	//	dTop and dBottom are used by neighboring lifts to set the height requirement
	//	if dTop<dBottom then these values are ignored
	void			MakeStandardsFitLifts( SideOfBayEnum eSideToTest, double dTop=-2.00, double dBottom=-1.00 );

	//Adjust the height of the standards on the specified corners
	//	note the corners will be ignored if they are INVALID
	//	dTop and dBottom are used by neighboring lifts to set the height requirement
	//	if dTop<dBottom then these values are ignored
	void			MakeAStandardFitLifts( SideOfBayEnum eSideToTest, CornerOfBayEnum	eCornerLeft,
								CornerOfBayEnum eCornerRight, double dTop, double dBottom );
	void			MakeAStandardFitLifts( CornerOfBayEnum eCorner );

	//Standards and RLs
	void			AdjustStandardBottomToRL( double dRL, CornerOfBayEnum eCnr );
	void			MakeStandardsSameTopRL();
	void			SetStandardToRLs( CornerOfBayEnum eCorner, double dRLTop, double dRLBottom );
	Component*		GetStandardAtRL( double dRL, CornerOfBayEnum eCnr );
	double			GetBottomRLofStandard( CornerOfBayEnum eCnr ) const;
	double			GetTopRLOfStandard( CornerOfBayEnum eCnr );
	bool			GetRLsForFaceByStandards( SideOfBayEnum eSide, double &dMinRL, double &dMaxRL );

	//other standards functions
	void			RepositionStandards();

	/////////////////////////////////////////////////////////////////////////////////
	//Braces
	/////////////////////////////////////////////////////////////////////////////////

	//not required
	//GetBracesArrangement();

	//creates a set of bracing using default spacing.  Default argument uses bay pointer logic to create bracing for all exposed sides.
	BracingReturnEnums	CreateBracingArrangment ( SideOfBayEnum eSide, MaterialTypeEnum eMaterial, bool bReplaceStageboards=false );
	void				CopyBracingSmart( SideOfBayEnum eSide, const Bay *pOriginalBay );

	//uses the heights as a list of starting positions for the bracing
	BracingReturnEnums	SetBracingArrangement ( doubleArray &daArrangement, SideOfBayEnum eSide, MaterialTypeEnum eMaterial );

	//Deletes all bracing from a bay
	bool				DeleteBracing( SideOfBayEnum eSide=NORTH );


	/////////////////////////////////////////////////////////////////////////////////
	//Movement
	/////////////////////////////////////////////////////////////////////////////////

	//Moves the Bay
	void			Move( Matrix3D Transfrom, bool bStoreMovement=true );

	//calls the run object to move each Bay in the Run to a certain offset
	void			MoveRun( Matrix3D Transfrom );
	void			MoveBays( Matrix3D Transform, BayDirectionEnum eDirection );

	Matrix3D		UnMove();

	//access the Transform directly
	Matrix3D		GetTransform() const;
	void			SetTransform( Matrix3D Transform );

	void			UserMovedBay( Matrix3D Transform );
	SideOfBayEnum	GetPosition( int iComponentID ) const;


	/////////////////////////////////////////////////////////////////////////////////
	//Rules
	/////////////////////////////////////////////////////////////////////////////////

	//Ensure that any Lapboards are at correct height etc.
	//CheckLapboards

	//Ensure that this bay is Legit
	//CheckRulesForThisBay

	//Ensure all the lifts are behaving themselves
	//CheckRulesForLifts
	
	//Ensure the Next Bay is Congruent 
	//CheckRulesRelatingToNextBay
	
	//Ensure the previous Bay is Congruent
	//CheckRulesRelatingToPreviousBay
	
	//Ensure that we are not way off track for the entire Run
	//CheckRulesRelatingToRunBay


	/////////////////////////////////////////////////////////////////////////////////
	//Split
	/////////////////////////////////////////////////////////////////////////////////

	//This function is used to tell the Run to split this Bay & subsequent Bays from
	//	'pPrevious' Bay to become a new run.  Firstly needs to check if pPrevious is
	//	NULL
	bool				SplitRunAtThisBay();

		
	/////////////////////////////////////////////////////////////////////////////////
	//Move/Copy/Delete
	/////////////////////////////////////////////////////////////////////////////////

	//Destroys all lifts in Bay.  Checks Matrix.  This function would typically be
	//	called from within the Run!  Also notifies the Matrix that the Bay has been
	//	removed.
	bool				DeleteBay();

	//notifies the Run, via a message, that this Bay needs to be deleted.
	void				RemoveBay();

	//Copies the contents of one bay to another new bay!  Set the ID's for the new Bay.
	//	Will also be required to check pointers of the new bay to ensure that we don't
	//	need an extra set of standards, transom etc.  We may even need to offset the
	//	new bay to separate it from the first bay.
	Bay*				CopyBay( Matrix3D TransformationMatrix, bool bMove=true ) const;
	Bay&				CopyBay( const Bay &OriginalBay, BayList *pBays=NULL );

	Bay&				operator= (const Bay &OriginalBay);
	bool				operator ==(const Bay &OtherBay) const;
	bool				operator !=(const Bay &OtherBay) const;


	/////////////////////////////////////////////////////////////////////////////////
	//BOM
	/////////////////////////////////////////////////////////////////////////////////

	//This is used to write info for the BOM, it calls lift.WriteBOMTo, and also for
	//	the components.WriteBOMTo 
	void				WriteBOMTo( CStdioFile *pFile);


	/////////////////////////////////////////////////////////////////////////////////
	//Matrix
	/////////////////////////////////////////////////////////////////////////////////

	//Sets the pointer to the MatrixElement representation, may also need to set the
	//	Alternate Matrix Pointer
	bool				SetMatrixElementPointer( MatrixElement *pMatrixElement, bool bSetAlt=false  );

	//Retrives the pointer to the Matrix Element, may also need to get the Alternate
	//	Matrix Pointer
	MatrixElement*		GetMatrixElementPointer() const;
	MatrixElement*		GetMatrixElementPointerAlt() const;

	//This is really an internal (private) function.  This function does several
	//	things:
	//		Checks matrix pointer is valid
	//		Ensure Matrix Element is representative of the Bay
	//		Checks that the MatrixElement points back to this Bay
	//		[optional] - ensure that the Matrix Element is in the Matrix
	bool				CheckMatrixPointer( bool bUseAlt );

	//Asks the Controller class via the Run class to search for an existing matrix
	//	that is compatible with this Bay, if it is found it returns the matrixID,
	//	otherwise it returns some -ve enum value.  Need to also consider the
	//	pMatrixPointerAlt case.
	int					FindMatrix( bool bUseAlt );

	//May be the same as above.  Need to also consider the pMatrixPointerAlt case.
	bool				CheckExistsInMatrix( bool bUseAlt );

	//Checks the matrix to see if this Bay is represented.  Sets the appropriate
	//	values and pointers if it is - I don't think we need this function anymore,
	//	I am sure the above two functions can handle this.  Need to also consider
	//	the pMatrixPointerAlt case.
	//EnsureMatrix();

	//Tells the Controller class, via the Run, to create a new MatrixElement,
	//	should probably double check that the matrix element doesn't exist first!.
	//	Need to also consider the pMatrixPointerAlt case.
	bool				CreateMatrix( bool bUseAlt );

	//Notifies the Matrix that this MatrixElementID should remove its link back to
	//	this bay.  Need to also consider the pMatrixPointerAlt case.
	bool				MatrixRemoveLink( bool bUseAlt );


	/////////////////////////////////////////////////////////////////////////////////
	//Add Components General
	/////////////////////////////////////////////////////////////////////////////////

	//Adds a component to a lift.  The lift is determined by using the
	//	GetLiftAtHeight function.
	Component*		AddComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
								double dHeight, double dLength,
								MaterialTypeEnum MaterialType=MT_STEEL );

	Component*		CreateComponentDumb( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
								int iRise, double dLength,
								MaterialTypeEnum eMaterialType=MT_STEEL );

	//adds individual handrail to a side of the bay
	//	Needs a Toolbar with
	//	Handrail Style
	//	UI to position Handrail visually on Bay (also consider Lapboards)
	//	User should be prompted to add to entire run
	bool			AddHandrail( SideOfBayEnum eSide=NORTH, bool bCheckNeighborSytle=false );
	//adds boards to all 2m high lifts within a bay (or maybe only apply to those decks above RL0
	//	Needs a toolbar with
	//	Material Type
	//	User should be prompted to apply to entire run
	bool			AddBoards( );
	//adds ties 
	//	Needs a Toolbar - It can now get this information from the Run
	//	Type of Tie ( Column, Yolk, Masonry )
	//	Clamps ( 90 deg, Standard )
	//	ties should be added every X meters from the first star, not from RL0.
	//	Vertical separation
	bool			AddStage( int iLiftID, SideOfBayEnum eSide, int iNumberOfPlanks, MaterialTypeEnum eMaterail );
	bool			AddTie( );
	void			AddLedgerIfReqd( int iLiftId, bool bSouth=true );
	void			AddTransomIfReqd( int iLiftId, bool bWest=true );
	void			AddHopupToBayIfReqd(SideOfBayEnum eSide, int iNumberOfBoards);
	void			AddComponentToNeighborIfReqd( int iLiftId, ComponentTypeEnum eType, SideOfBayEnum eSideRemove, SideOfBayEnum eSideAdd, Bay *pBayNeighbor );
	void			AddStageBoardCheck( SideOfBayEnum eSide, bool ShowWarning=true );


	/////////////////////////////////////////////////////////////////////////////////
	//Get Components General
	/////////////////////////////////////////////////////////////////////////////////

	//Then number of compoents that belong to the bay but not in a lift, eg standards
	int				GetNumberOfBayComponents() const;
	//retrieves a pointer to a component in a lift.  I see that this will mostly be
	//	used to determine if the component exists or not by testing for NULL!
	Component*		GetComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
								double dHeight  ) const;
	Component*		GetBayComponent( ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise );
	Component*		GetBayComponent( int iComponentID ) const;
	int				GetComponentsRise( int iComponentID ) const;
	int				GetNumberOfHopupsInBay( int &iNumberOfBoards );


	/////////////////////////////////////////////////////////////////////////////////
	//Remove Components General
	/////////////////////////////////////////////////////////////////////////////////

	//removes a particular component/s from a lift.  The lift is determined by using
	//	the GetLiftAtHeight function.
	bool			DeleteComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
								double dHeight=ID_ALL_IDS );
	void			DeleteAllComponents();
	void			DeleteAllLifts();
	void			DeleteAllComponentsFromLifts();
	bool			DeleteAllComponentsFromSide( SideOfBayEnum eSide );
	bool			DeleteAllComponentsOfType( ComponentTypeEnum eType );
	bool			DeleteAllComponentsOfTypeFromSide(ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise=LIFT_RISE_INVALID );

	bool			DeleteBayComponent( int iComponentID );
	bool			DeleteBayComponent( ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise );
	void			DeleteAllBayComponents();
	bool			DeleteAllBayComponentsOfTypeFromSide( ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise=LIFT_RISE_INVALID );

	bool			DeleteTieFromSide( SideOfBayEnum eSide );
	bool			DeleteShadeClothFromSide( SideOfBayEnum eSide );
	bool			DeleteChainlinkFromSide( SideOfBayEnum eSide );
	void			DeleteUnneeded1000mmHopup( Bay *pNeighbor, SideOfBayEnum eCompSide, SideOfBayEnum eStageSide );
	void			DeleteUnneeded1000mmHopups( Bay *pNeighborBay, SideOfBayEnum eNeighborDirection );
	bool			Delete1000mmHopupRailFromSide( SideOfBayEnum eSide );
	//removes all bracing from that side of the bay - NOT REQUIRED (see Bracing above)
	//	eSide should be a  binary (0x0001, 0x0002, 0x0004…) value so that we can do multiple sides at a time - NOT REQUIRED (see Bracing above)
	bool			DeleteBrace( SideOfBayEnum eSide );
	//Deletes all ties from a bay
	bool			DeleteTies( SideOfBayEnum eSide );
	//Deletes all boards from all lifts within a bay
	//	User should be prompted to apply to entire run
	bool			DeleteBoards( );
	//Deletes all Handrails from that side of a bay.
	//	User should be prompted to Delete from entire run
	//	eSide should be a  binary (0x0001, 0x0002, 0x0004…) value so that we can do multiple sides at a time AddStages( SideOfBayEnum eSide/*=NORTH*/ ) - adds individual bracing to a side of the bay
	bool			DeleteHandrail( SideOfBayEnum eSide );


	void			EnsureHandrailsAreCorrect( SideOfBayEnum eSide, bool bCheckNeighborSytle=false );
	void			EnsurePlanksAreSupported( SideOfBayEnum eSide );

	//Copy
	Bay&			CopyAllComponents(const Bay &OriginalBay, BayList *pBays=NULL );

	//Has functions
	void			EnsureComponentExists( Bay *pBayNeighbor, Lift *pLift, ComponentTypeEnum eType,
								SideOfBayEnum eSide, SideOfBayEnum eSideNeighbor, double dWidth,
								LiftRiseEnum eRise, bool bPreferAddToNeighbor );
	bool			HasComponentOfTypeOnSide( ComponentTypeEnum eType, SideOfBayEnum eSide );
	bool			HasBayComponentOfTypeOnSide( ComponentTypeEnum eType, SideOfBayEnum eSide ) const;
	bool			DoesABayComponentExistOnASide( ComponentTypeEnum eType, SideOfBayEnum eSide ) const;


	// Bay details dialog access functions
	void			SetStandardsFit(StandardFitEnum eStandardsFit);
	StandardFitEnum	GetStandardFit();

	void			SetStaggeredHeights(bool bStaggeredHeights);
	bool			GetStaggeredHeights();

	//display dialogs
	int				DisplayBayDetailsDialog(CWnd *pParent = NULL);
	void			DeleteLiftDetailsDialog();

	int				DisplayLiftDetailsDialog(CWnd *pParent = NULL);
	void			DeleteBayDetails();


	/////////////////////////////////////////////////////////////////////////////////
	//Drawing routines
	/////////////////////////////////////////////////////////////////////////////////
	//Redraws the Plan and 3D views.
	void			Redraw();

	//Deletes the Plan view and recreates using the object data.  To complete this we will need to be able to determine if the bays have Handrails, Bracing, Ties etc (i.e. any component visible in Plan view).  Colour coding should also be done by this function, colour would be determined by Bay width
	void			RedrawPlanView();

	//Deletes coordinates of all components and repositions them again
	void			Redraw3Dview();

	//find the actual dimensions of the bay, not the common dimensions
	double			GetBayDimensionActual( double dCommonLength, bool bLength, bool bShowWarning=true ) const;

protected:
	AcDbObjectId	m_pSWStandardEntityID;
	double m_dBayStarSeparation;
//	bool m_bRequiresSoleBoard[4];
	double m_dTieTubeVerticalTolerance;
	bool m_bVisited;
	Bay				*m_pUndoMillsBay;
	MillsType		m_mtCanBeMillsCnrBay;
	bool			m_bSydneyCornerBay;
	////////////////////
	////////////////////
	/////Attributes/////
	////////////////////
	////////////////////

	/////////////////////////////////////////////////////////////////////////////////
	//General 
	int				m_iBayID;		//Unique id within run
	Run				*m_pRun;		//pointer back to its run
	LiftList		m_LiftList;		//list of lifts
	public:
	ComponentList	m_caComponents;	//Array of components
	protected:
	BayTemplate		*m_pBTplt;		//The Bay template
	Matrix3D		m_Transform;
	double			m_dPreviousLength;
	double			m_dPreviousWidth;
	int				m_iAllowDraw;
	intArray		m_iaLapboardCrossesSide;
	bool			m_bBayRecentlyMoved;
	Controller		*m_pControllerForLapboard;

	//other options
	bool				m_bUseLedgerEveryMetre;
	bool				m_bUseMidrailWithChainMesh;
	double				m_dTieTubeSeparation;
	BayTypeEnum			m_BayType;
	bool				m_bContainsMovingDeck;
	RoofProtectionEnum	m_eRoofProtection;

	//Bay link ids from serialize load, where bay has not been loaded yet
	int				m_iLoadedOuterRunID;
	int				m_iLoadedOuterBayID;
	int				m_iLoadedInnerRunID;
	int				m_iLoadedInnerBayID;

	/////////////////////////////////////////////////////////////////////////////////
	//Pointers to Neighboring bays
	Bay				*m_pForward;	//East
	Bay				*m_pBackward;	//West
	Bay				*m_pInner;		//South
	Bay				*m_pOuter;		//North
	LapboardList	*m_pLapboards;	//CornerForward

	/////////////////////////////////////////////////////////////////////////////////
	//Matrix
	//Pointer to an existing matrix element. [Default = NULL]
	MatrixElement	*m_pMatrixElement;
	//This is a pointer to the alternate matrix, which may be used if this bay
	//	has 4 sets of standards.
	MatrixElement	*m_pMatrixElementAlt;

	/////////////////////////////////////////////////////////////////////////////////
	//Standards - STND_NORTH_EAST, STND_SOUTH_EAST, STND_NORTH_WEST, STND_SOUTH_WEST
	//Height of standards
	double			m_daHeights[4];
	Point3D			m_ptaStandardsRL[4];

	/////////////////////////////////////////////////////////////////////////////////
	//Dialog Box stuff
	BayDetailsDialog	*m_pBayDetailsDialog;
	bool				m_bStaggeredHeights;
	StandardFitEnum		m_eStandardsFit;
	double				m_dBayLength;
	double				m_dBayWidth;
	bool				m_bBayWidthChangedDuringEdit;
	bool				m_bBayLengthChangedDuringEdit;
	bool				m_bBaySizeMultipleSelection;

	//does this object need to be deleted
	DirtyFlagEnum		m_dfDirtyFlag;

private:
	/////////////////////
	/////////////////////
	//Private functions//
	/////////////////////
	/////////////////////

	/////////////////////////////////////////////////////////////////////////////////
	//Other Stuff
	/////////////////////////////////////////////////////////////////////////////////

	//Sets the global variables to thier default values
	void			SetGlobalValuesToDefault();
	void			ReapplyShadeAndChainOnAllSides();
	bool			DoesDeckLiftHaveNeighborDeckAtSameRL( int iLiftID, Bay *pNeighboringBay, bool bInclude500UpOrDown=true ) const;
	bool			FindTopBottomRLsAtCornerByLifts( CornerOfBayEnum eCorner, double &dRLBottm, double &dRLTop, bool bLookAtStandardsToo=false );
	void			GetBracingDimensions( Bay *pNeighboringBay, CornerOfBayEnum Cnr1, CornerOfBayEnum Cnr2, double daHeights[4] );

	
	/////////////////////////////////////////////////////////////////////////////////
	//This function will find the RL of a star if give a corner, or if given a side
	//	it will find the lowest RL which has the give StarNumber, 
	bool			GetRLOfStars(	int iStarNumber, double &dRL, SideOfBayEnum eSide=NORTH ) const;
	bool			GetRLOfStar(	int iStarNumber, double &dRL, CornerOfBayEnum eStandardSide ) const;

	/////////////////////////////////////////////////////////////////////////////////
	//Retrieves the Bracing Height (ie the number of stars) covered by some particular
	//	bracing.  The function needs the Baywidth and since in most case there are two
	//	Lengths of bracing for any particular bay with, you can specify if you want the
	//	longer or the shorter of the two Lengths.
	double			GetBracingRaise(double dBayWidth ) const;
	double			GetBracingRaise(double dBayWidth, double dBraceLength ) const;
	
	//This is a variation of the above in that it takes the bay length, the raise
	//	(number of stars) and returns the Length of the bracing reqruied.
	double			GetBracingLength(double dBayWidth ) const;
	double			GetBracingLength(double dBayWidth, double dRaise ) const;


	/////////////////////////////////////////////////////////////////////////////////
	//Matrix Tests
	/////////////////////////////////////////////////////////////////////////////////

	//This Test will confirm that the Handrail that exist in the matrix match the
	//	Handrail in the bay this has to be a 1:1 match to succeed.
	bool			TestHandRailHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;

	//This Test will confirm that the Ledger that exist in the matrix match the
	//	Ledger in the bay this has to be a 1:1 match to succeed.
	bool			TestLedgerHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;

	//This Test will confirm that the Midrail that exist in the matrix match the
	//	Midrail in the bay this has to be a 1:1 match to succeed.
	bool			TestMidRailHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;

	//This Test will confirm that the Toe Board that exist in the matrix match the
	//	Toe Board in the bay this has to be a 1:1 match to succeed.
	bool			TestToeBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;

	//This Test will confirm that the End Handrail that exist in the matrix match the
	//	End Handrail in the bay this has to be a 1:1 match to succeed.
	bool			TestEndHandRailHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const;

	//This Test will confirm that the End Midrail that exist in the matrix match the
	//	End Midrail in the bay this has to be a 1:1 match to succeed.
	bool			TestEndMidRailHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const;

	//This Test will confirm that the Stage boards that exist in the matrix match the
	//	Stageboards in the bay this has to be a 1:1 match to succeed.
	bool			TestStageBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;
	
	//This Test will confirm that the 1.0m HopupRail boards that exist in the matrix match the
	//	1.0m HopupRails in the bay this has to be a 1:1 match to succeed.
	bool			TestHopupRailBoardHeights( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;
	
	//This Test will confirm that the Decking that exist in the matrix match the
	//	Decking in the bay this has to be a 1:1 match to succeed.
	bool			TestTransomHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const;

	//This Test will confirm that the trasoms that exist in the matrix match the
	//	transoms in the bay this has to be a 1:1 match to succeed.
	bool			TestDeckingHeights( MatrixElement *pMatrixElement, bool bUseAlt ) const;

	//This Test will confirm that the arrangement of the standards that exist in the
	//	matrix match the Arrangement in the bay, this has to be a 1:1 match to succeed.
	bool			TestStandardsArrangment( MatrixElement *pMatrixElement, bool bInner, bool bUseAlt ) const;

	//Array tests
	bool			AreArrangementsSame( intArray &ia1,		intArray &ia2		) const;
	bool			AreArrangementsSame( doubleArray &da1,	doubleArray &da2	) const;

	/////////////////////////////////////////////////////////////////////////////////
	//Matrix Create Arrangements
	/////////////////////////////////////////////////////////////////////////////////

	//Set the Matrix element's Decking to be the same as this bay's
	bool			CreateMatrixDeckingArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Transoms to be the same as this bay's
	bool			CreateMatrixTransomArrangment( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Standards to be the same as this bay's
	bool			CreateMatrixStandardArrangment( doubleArray &daArrangement, bool &bSoleBoard, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Stages to be the same as this bay's
	bool			CreateMatrixStageArrangement( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's 1.0m HopupRails to be the same as this bay's
	bool			CreateMatrixHopupRailArrangement( doubleArray &daArrangement, intArray &iaPlanks, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's end handrail to be the same as this bay's
	bool			CreateMatrixEndHandrailArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's end Midrail to be the same as this bay's
	bool			CreateMatrixEndMidrailArrangement( doubleArray &daArrangement, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's handrail to be the same as this bay's
	bool			CreateMatrixHandrailArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Ledger to be the same as this bay's
	bool			CreateMatrixLedgerArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Midrail to be the same as this bay's
	bool			CreateMatrixMidrailArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;

	//Set the Matrix element's Toe Board to be the same as this bay's
	bool			CreateMatrixToeBoardArrangement( doubleArray &daArrangement, bool bInner, bool bUseAlt, bool bCheckNeighbor=true ) const;


	/////////////////////////////////////////////////////////////////////////////////
	//Component creation functions
	/////////////////////////////////////////////////////////////////////////////////

	//creates a component using the new operator, if it is not added to a
	//	component list, then you will be responsible for destroying/removing it
	Component *CreateComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide,
								int iRise, double dLength,
								MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a standard and attaches it to the Bay components list
	Component *AddAOpenEndStandard( SideOfBayEnum eSide, double dLength,
							 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a standard and attaches it to the Bay components list
	Component *AddAStandard( SideOfBayEnum eSide, double dLength,
							 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Jack and attaches it to the Bay components list
	Component *AddAJack( SideOfBayEnum eSide, MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Soleboard and attaches it to the Bay components list
	Component *AddASoleboard( SideOfBayEnum eSide, MaterialTypeEnum eMaterialType=MT_TIMBER );

	//Creates a Chainlink and attaches it to the Bay components list
	Component *AddAChainLink( SideOfBayEnum eSide );

	//Creates a shade cloth and attaches it to the Bay components list
	Component *AddAShadeCloth( SideOfBayEnum eSide );

	//Creates a Tie, two tie clamps and a clamp and attaches them to the Bay components list
	Component *AddATie( SideOfBayEnum eSide, double dHeight, double dLength, MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Tie clamp 90 degrees and attaches it to the Bay components list
	Component *AddATieClamp90deg( SideOfBayEnum eSide, double dHeight,
									MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a clamp and attaches it to the Bay components list
	Component *AddATieClampColumn( SideOfBayEnum eSide, double dHeight, double dLength,
									MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a clamp and attaches it to the Bay components list
	Component *AddATieClampMasonary( SideOfBayEnum eSide, double dHeight, double dLength,
									MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a clamp and attaches it to the Bay components list
	Component *AddATieClampYoke( SideOfBayEnum eSide, double dHeight, double dLength,
									MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Ledger and attaches it to the correct lift
	Component *AddALedger( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Transom and attaches it to the correct lift
	Component *AddATransom( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Mesh guard and attaches it to the correct lift
	Component *AddAMeshguard( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Hand rail and attaches it to the correct lift
	Component *AddARail( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Mid rail and attaches it to the correct lift
	Component *AddAMidrail( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Toe board and attaches it to the correct lift
	Component *AddAToeBoard( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Hop Up bracket and attaches it to the correct lift
	Component *AddAHopUpBracket( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );

	//Creates a Corner stage board and attaches it to the correct lift
	Component *AddACornerStageBoard( SideOfBayEnum eSide, double dLength, double dHeight,
								 MaterialTypeEnum eMaterialType=MT_STEEL );


	/////////////////////////////////////////////////////////////////////////////////
	//Component location functions
	/////////////////////////////////////////////////////////////////////////////////

	//retrieve a pointer to a standard
	Component *GetAStandard( SideOfBayEnum eSide, double dHeight  ) const;

	//retrieve a pointer to a component which is vertically spaced along the bay
	Component *GetABayVerticalComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight  ) const;

	//retrieve a pointer to a component which has no vertical spread
	Component *GetABayHorizontalComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight ) const;

	//retrieve a pointer to a component which is owned by a lift
	Component *GetALiftComponent( ComponentTypeEnum eComponentType, SideOfBayEnum eSide, double dHeight  ) const;


	/////////////////////////////////////////////////////////////////////////////////
	//Converting RLs and Heights
	/////////////////////////////////////////////////////////////////////////////////

	//takes an RL and converts it to a height reletive to the South east standards
	double			ConvertRLtoHeight( double dRL ) const;

	//takes an Height and converts it to a RL reletive to the South east standards
	double			ConvertHeightToRL( double dHeight ) const;
};


#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afxtempl.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif
class BayList : public CArray<Bay*,Bay*>
{
public:
	SystemEnum GetSystem() const;
	bool GetMustBeSameSystem() const;
	void SetMustBeSameSystem( bool bEnsureSame );
	BayList();
	~BayList();
	
	BayList &operator=(const BayList &Original);

	int		GetBayPos( Bay *pBay );
	int		Add( Bay *pBay );
	bool	IsInList( Bay *pBay );
	void	SetUniqueBaysOnly( bool bUnique );

	void	DisplaySelectedBays();
protected:
	bool m_bMustBeSameSystem;
	bool m_bUniqueBaysOnly;
};


#endif // !defined(AFX_BAY_H__E6E8AA84_3A32_11D3_884B_0008C7999B1D__INCLUDED_)


/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Bay.h $
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:42a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 6/10/00    Time: 1:29p
 * Updated in $/Meccano/Stage 1/Code
 * About to build R1.1e - This should have all of Mark's current small
 * bugs fixed, ready for the second release to Perth (still awaiting
 * feedback from Perth from the first one we sent them)  This is another
 * version for mark to test, and is a candidate for Perth release
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 25/09/00   Time: 4:04p
 * Updated in $/Meccano/Stage 1/Code
 * Bay movement now seems correct!
 * 
 * *****************  Version 142  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 140  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 139  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 138  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 136  *****************
 * User: Jsb          Date: 24/07/00   Time: 1:38p
 * Updated in $/Meccano/Stage 1/Code
 * About to rewind to B4 3D position exact started
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 17/07/00   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the 500mm star separation to 495.3
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 3/07/00    Time: 7:57a
 * Updated in $/Meccano/Stage 1/Code
 * About to Try Divide and Concure to trap this error
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 26/06/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * auto changing heights of adjoining stairs and ladders
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 19/06/00   Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 9/06/00    Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * taking code home
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 1/06/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 120  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 23/05/00   Time: 11:55a
 * Updated in $/Meccano/Stage 1/Code
 * About to try the CAcUiDockControllBar class to handle toolbars
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 4/05/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 20/04/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 3/04/00    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 27/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 7/03/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 23/02/00   Time: 2:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 10/02/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * I think I have complete the JM bug
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 20/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 23/12/99   Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 23/12/99   Time: 9:26a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 13/12/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix is now correct!
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 10/12/99   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * Almost got the Lapboards working fully
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 22/10/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Birdcaging pointers now fully operational
 * 2) Birdcaging now uses pascal's triangle for End of runs
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 19/10/99   Time: 9:03a
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on Birdcaging
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 5/10/99    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 5/10/99    Time: 1:35p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished SummaryInfo class
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 5/10/99    Time: 9:29a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Dar          Date: 4/10/99    Time: 2:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 28/09/99   Time: 3:43p
 * Updated in $/Meccano/Stage 1/Code
 * I have now moved the schematic stuff into the PreviewTemplate, this is
 * usefull for drawing lapboards using same code as drawing schematic bays
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:05a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 13/09/99   Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 9/09/99    Time: 1:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 9/09/99    Time: 7:54a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 45  *****************
 * User: Dar          Date: 9/07/99    Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 7/09/99    Time: 3:30p
 * Updated in $/Meccano/Stage 1/Code
 * 1) plot line styles
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Dar          Date: 8/27/99    Time: 2:45p
 * Updated in $/Meccano/Stage 1/Code
 * fixed lift details not being displayed after autobuild twice
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/23/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Updated all the ::Removexxx() functions
 * 2) checked all the ::Deletexxx functions
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/23/99    Time: 11:43a
 * Updated in $/Meccano/Stage 1/Code
 * Insert Lift function completed
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Dar          Date: 8/19/99    Time: 5:01p
 * Updated in $/Meccano/Stage 1/Code
 * added displayliftlistdialog
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Dar          Date: 8/16/99    Time: 1:22p
 * Updated in $/Meccano/Stage 1/Code
 * added some protected var _taccess functions
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Dar          Date: 8/16/99    Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/12/99    Time: 1:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 28/07/99   Time: 15:55
 * Updated in $/Meccano/Stage 1/Code
 * Continueing with Run class
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 27/07/99   Time: 14:44
 * Updated in $/Meccano/Stage 1/Code
 * Added the Controller class framework
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 27/07/99   Time: 12:18
 * Updated in $/Meccano/Stage 1/Code
 * I can finally compile with only 8 warnings
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 15/07/99   Time: 11:43
 * Updated in $/Meccano/Stage 1/Code
 * MFC should now be working via stdafx.h
 *
 *******************************************************************************/
