// controller.h: interface for the Controller class.
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

#include "VisualComponents.h"	// Added by ClassView
#include "MdiAware.h"	// Added by ClassView
#if !defined(AFX_CONTROLLER_H__9F66E165_43A6_11D3_885D_0008C7999B1D__INCLUDED_)
#define AFX_CONTROLLER_H__9F66E165_43A6_11D3_885D_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//						CONTROLLER CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:31p $
//	Version			: $Revision: 165 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This is the controlling class for the entire ARX.
///////////////////////////////////////////////////////////////////////////////

//includes
#include "stdafx.h"
#include "MeccanoDefinitions.h"
#include "Run.h"
#include "AutoBuildTools.h"
#include "ComponentDetails.h"
#include "Matrix.h"
#include "JobDescriptionInfo.h"
#include "SummaryInfo.h"
#include "LapboardBay.h"
#include "StockListElement.h"
#include "BayStageLevel.h"
#include "LevelList.h"
#include "EntityDefinitions.h"
#include "EntityDebugger.h"
#include "ObjectPosition.h"
#include "VisualComponents.h"
#include "StageLevelVisible.h"

//forward declartions
class Component;
class BOMExtra;

extern EntityDebugger gED;
extern int giDebugNumber;
extern int giAutobuildNumber;

#include "GraphicTypes.h"	// Added by ClassView
#include "doubleArray.h"	// Added by ClassView
#include "ObjectPosition.h"	// Added by ClassView
#include "componentList.h"	// Added by ClassView

#include "Bay.h"	// Added by ClassView
#include "ComponentDefinitions.h"	// Added by ClassView
#include "MaterialTypeDefinitions.h"	// Added by ClassView

enum RoundingDevisorEnum
{
	RD_WHOLE		= 1,
	RD_HALF			= 2,
	RD_QUATER		= 4,
	RD_FIFTH		= 5,
	RD_TENTH		= 10,
	RD_HUNDREDTH	= 100,
	RD_UNDEFINDE	= -1
};

enum RoundingListOrderEnum
{
	RLO_WHOLE,
	RLO_HALF,
	RLO_QUATER,
	RLO_FIFTH,
	RLO_TENTH,
	RLO_HUNDREDTH
};

extern int	giaLoopCounter[10];
extern CString	gsaLoopCounter[10];

enum KwikRLsArrayEnum
{
	KRL_1ST,
	KRL_2ND,
	KRL_3RD,
	KRL_SIZE,
};

int ModifyMatrix(ads_point usrpt, ads_matrix matrix );

class Controller
{
public:
	void DrawKwikRLs();
	int BOMExtraSinceInsert();
	int VisualCompsSinceInsert();
	int BaysSinceInsert();
	int LapsSinceInsert();
	int RunsSinceInsert();

	void ClearActualComponents();
	void ShowRLforPoint();
	bool IsRLCalcPossible();
	double GetRLAtPoint( double x, double y );
	double GetRLAtPoint( Point3D pt );
	void SetRLs();
	bool AreObjectIDsSelected( AcDbObjectIdArray &objIDs );
	void UpdateAllVeiws();
	void SetSaveSelected( bool bSaveSelected );
	bool SaveSelectedOnly();


	//////////////////////////////////////////////////////////////////////
	//Per roll

	//Storage
	void ReadPerRollFromRegistry();
	void CalcPerRoll();

	//Conversion for Rounding
	RoundingListOrderEnum	ConvertToListOrder( RoundingDevisorEnum eDevisor );
	RoundingDevisorEnum		ConvertToDevisor( RoundingListOrderEnum eIndex );

	//Get
	RoundingDevisorEnum GetRoundingDivisorShadeCloth();
	RoundingDevisorEnum GetRoundingDivisorChainMesh();
	bool CalcPerRollChainMesh();
	bool CalcPerRollShadeCloth();
	CString GetPartNumberShadeCloth();
	CString GetPartNumberChainMesh();
	double GetRollSizeShadeCloth();
	double GetRollSizeChainMesh();

	//set
	void SetRoundingDivisorShadeCloth(RoundingDevisorEnum eRD );
	void SetRoundingDivisorChainMesh(RoundingDevisorEnum eRD );
	void SetCalcPerRollChainMesh( bool bPerRoll );
	void SetCalcPerRollShadeCloth(bool bPerRoll );
	void SetPartNumberShadeCloth( CString sPartNumber );
	void SetPartNumberChainMesh( CString sPartNumber );
	void SetRollSizeShadeCloth( double dArea );
	void SetRollSizeChainMesh( double dArea );

	//////////////////////////////////////////////////////////////////////
	void EditStock();
	Point3D GetLowerLeftOfSchematic();
	bool IsBlockInserting();
	void SetBlockInserting( bool bInserting );
	void SetStageLevelVisible( StageLevelVisible &Original );
	void CopyStageLevelVisible( StageLevelVisible &New );
	void SetStageLevelVisible( CString sStage, int iLevel, bool bVisible );
	bool IsStageLevelVisible( CString sStage, int iLevel );
	int GetBOMExtraB4Insert();
	int GetVisualCompsB4Insert();
	int GetBaysB4Insert();
	int GetLapsB4Insert();
	int GetRunsB4Insert();
	bool IsInsertingBlock();
	void SetInsertingBlock( bool bInserting, bool bResetValues=false );
	double GetStarSeperation();
	bool IsKwikStage();
	intArray GetVisitedBays();

	//Saved history
	void SavedHistoryClear();
	void SavedHistoryShow();

	//Convert system
	void SetConvertToSystemOnNextOpen( bool bConvert );
	bool GetConvertToSystemOnNextOpen();

	//BOM
	void ShowBaySizesInBOMS();
	void ShowBaysInBOMS();
	void SetBOMSummaryToShowBays( bool bShow );
	void SetBOMSummaryToShowBaySizes( bool bShow );
	bool GetBOMSummaryToShowBays();
	bool GetBOMSummaryToShowBaySizes();

	//Individual components stuff
	void SetAllComponentsVisible();
	int GetNumberOfVisualComponents();
	VisualComponents *GetVisualComponents();
	void SelectComponents( CompArray &Components );
	void ComponentInsert();
	void ComponentsEdit();
	Component *ComponentCopy();
	bool ComponentRotate();
	void ComponentMove();

	//Zig Zag
	void DisplayZigZagLine();
	bool GetDisplayZigZagLine();
	void SetDisplayZigZagLine( bool bDisplay );

	void SetShowStandardLengths( bool bShow );
	bool GetShowStandardLengths();
	void ChangeShowStandardLengths();
	void ChangeSystem();
	intArray * GetSelectedMatrixBays();
	intArray * GetSelected3DBays();
	bool GetUse0700TransomForLedger();
	bool GetUse1200TransomForLedger();
	void SetUse0700TransomForLedger( bool bUseTransomForLedger );
	void SetUse1200TransomForLedger( bool bUseTransomForLedger );
	void ChangeUse0700TransomForLedger();
	void ChangeUse1200TransomForLedger();
	void SetShowMatrixLabelsOnSchematic( bool bShow );
	bool GetShowMatrixLabelsOnSchematic();
	void ChangeShowMatrixLabelsOnSchematic();
	void GetDatabaseFilename( CString &sFilename );
	void GetUsersName( CString &sUserName );
	void SetUse1500Soleboards( bool bUse1500 );
	bool GetUse1500Soleboards();
	bool m_bUse1500Soleboards;
	void Use1500Soleboards();
	bool GetCutThroughLabelFromUser( CString &sTop, CString &sBottom, bool bSetTopLabel );
	void GetNextSuggestedLabel( CString &sSuggestion );
	void ResetLabelCount();
	AcDbObjectId CreateText(Point3D ptPosition, Matrix3D Transform, LPCTSTR strText, CString sLayer, double dHeight, int iColour, bool bAddToDB=true );
	void SetDeleteSchematic( bool bDelete );
	bool IsDeleteSchematic();
	void ChangeDeleteSchematic();
	void CreateCutThrough();
	void EnterLevels( bool bAddLevel=true );
	void UsePutLogClipsInLadder();
	void SetUsePutLogClipsInLadder( bool bUse );
	bool GetUsePutLogClipsInLadder();
	void SetShowTypicalMatrixSectionsOnly( bool bShowTypicalOnly );
	bool ShowTypicalMatrixSectionsOnly();
	void SetIsOpeningDocument( bool bOpening );
	bool IsOpeningDocument();

#ifdef _DEBUG
	void ComponentDebuggerRemove( Component *pComponent );
	void ComponentDebuggerAdd( Component *pComponent );
#endif

	///////////////////////////////////////////////////////////////////////////////
	//construction/destruction
	Controller();
	virtual ~Controller();
	void SetDestructingController( bool bDestructing );
	bool IsDestructingController();

	///////////////////////////////////////////////////////////////////////////////
	//debug tools
	void TraceContents();
	void TracePointers();

	/////////////////////////////////////////////////////////////
	//Displaying sale prices
	void DisplaySalePrice();
	bool IsDisplaySalePrice();
	void SetDisplaySalePrice( bool bDisplay );

	/////////////////////////////////////////////////////////////
	//Visit
	void SetHasBeenVisited( bool bVisited );

	/////////////////////////////////////////////////////////////
	//Standard radius in schematic
	void	SetRadiusOfSchematicStandard( double dRadius );
	double	GetRadiusOfSchematicStandard();

	/////////////////////////////////////////////////////////////
	//Sydney corner stuff
	void SydneyCornerSeparation();
	void SydneyCorner();
	void SetSydneyCornerDistBwBays( double dSeparation );
	double GetSydneyCornerDistBwBays();
	void ToggleSydneyCorner();
	void SetSydneyCorner( bool bSydneyCorner );
	bool IsSydneyCorner();

	/////////////////////////////////////////////////////////////
	//Mill system stuff
	void SetSystem( SystemEnum eSystem );
	SystemEnum GetSystem() const;
	void ConvertToMillsCorner();
	void UnconvertMillsCorner();
	void CheckBaysAreSameSize( BayList *pBays );
	void MillsSystemCorner();
	bool IsMillsSystemCorner();
	void SetMillsSystemCorner(bool bMillsSystemCorner);

	/////////////////////////////////////////////////////////////
	//wireframe vs Actual
	void UseActualComponents();
	void SetUseActualComponents( UseActualComponentsEnum eUse );
	UseActualComponentsEnum GetUseActualComponents();

	////////////////////////////////////////////////////////////
	//Displaying Component Types
	void ShowAllComponentTypes();
	bool IsComponentTypeVisible( ComponentTypeEnum eType );
	bool IsComponentTypeHidden( ComponentTypeEnum eType );
	void HideComponentType( ComponentTypeEnum eType );
	void ShowComponentType( ComponentTypeEnum eType );

	////////////////////////////////////////////////////////////
	//Wall offset from lowest hopup
	void WallOffsetFromLowestHopup();
	void SetWallOffsetFromLowestHopup( bool bUseLowest );
	bool IsWallOffsetFromLowestHopup();

	////////////////////////////////////////////////////////////
	//Job desc
	void SaveJobDescription();
	void DisplayJobDetail();
	JobDescriptionInfo * GetJobDescription();

	///////////////////////////////////////////////////////////////////////////////
	//Layers
	void CreateLayers();
	void DeleteLayers();
	Acad::ErrorStatus DeleteLayer( CString sLayerName );

	///////////////////////////////////////////////////////////////////////////////
	//Edit
	void EditBay();
	void EditRun();
	void InsertRun( int iPos, Run *pNewRun );
	bool InsertBay();
	Bay *InsertAndRedimensionBay(Bay *pOriginalBay, SideOfBayEnum eSide, double dNewWidth,
										 double dNewLength, double dNewWidthActual,
										 double dNewLengthActual, bool bChangeWidth=true,
										 bool bChangeLength=true);
	void ResizeBay();
	void SetLengthForBay( Bay *pBay, double dNewLength, bool bAllowMovement );
	bool EditingStandards();
	void EditStandards();
	void StoreCurrentView();	//stores the current view
	void RestoreCurrentView();	//restores the view to previous view
	void SetViewportPosition( AcGePoint2d ptBottomLeft, AcGePoint2d ptTopRight );

	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	void Serialize(CArchive &ar);
	void CleanUpLoadedInnerOuterPointers();

	///////////////////////////////////////////////////////////////////////////////
	//Autobuild
	void DoAutoBuild();					//This is the primary autoubuild command!
	void ShowDialogs(bool bShowDialogs);
	void SetArcLapSpan();
	void SetArcLapboardSpan( double dSpan );
	double GetArcLapboardSpan();
	void SetOverrun( bool bOverrun );
	bool GetOverrun();


	/////////////////////////////////////////////////////////////
	//Entity
	/////////////////////////////////////////////////////////////

	//Entity type, set by the reactor, retrieved by the EditorReactor
	void			ClearAllEntityTypes();
	void			SetErasureEntityType( EntityTypeEnum eType );
	void			SetMovementEntityType( EntityTypeEnum eType );
	void			AddEntityBayNumberID( int iBayNumberID );
	void			AddEntityLapboardID( int iLapboardID );
	void			AddEntityLevelID( int iLevelID );
	int				*GetGroupNumber();
	Bay				*GetBayPointerForEntityId( AcDbObjectId &Id );
	Component		*GetComponentPointerForEntityId(AcDbObjectId &Id);
	Bay				*GetBayOwningObjectID( AcDbObjectId objectID );
	intArray		GetEntityBayNumberID( );
	intArray		GetEntityLapboardID( );
	intArray		GetEntityLevelID( );
	EntityTypeEnum	GetErasureEntityType();
	EntityTypeEnum	GetMovementEntityType();
	Acad::ErrorStatus selectLines(AcDbObjectIdArray& idArray, ads_name selection);
	Acad::ErrorStatus selectComponents(AcDbObjectIdArray& idArray, ads_name selection);
	Acad::ErrorStatus selectGroups(AcDbObjectIdArray& idArray, ads_name selection);
	AcDbObjectIdArray *GetCopiedObjectArray();
	ObjectPositionArray m_opaObjectPositions;

	//Entity Reactor
	bool		IsIgnorMovement();
	void		IgnorMovement( bool bIgnor );
	bool		IsIgnorErase();
	void		IgnorErase( bool bIgnor );
	double		m_dLastAngleX;
	double		m_dLastAngleY;
	double		m_dLastAngleZ;
	Point3D		m_ptLastPosition[2];
	void		SetMovedComponent( Component *pComponent );
	Component	*GetMovedComponent();


	///////////////////////////////////////////////////////////////////////////////
	//Runs
	///////////////////////////////////////////////////////////////////////////////

	//retrieves the number of runs owned by the controller
	int GetNumberOfRuns() const;

	//Creates the RunTemplate Object
	int CreateNewRun( );

	//Destroys a run
	bool DeleteRuns( Run *pRun, int iCount=1, bool bShowProgress=true );
	bool DeleteRuns( int iRunID, int iCount=1, bool bShowProgress=true );

	//This function joins two runs.  The ends that are about to be joined need
	//	to have null pointers in the appropriate direction.
	Run *JoinRuns( Run *pRunFirst, Run *pRunSecond, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );
	Run *JoinRuns( int RunIDFirst, int RunIDSecond, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );

	//finds the position of the run in the run list
	int	GetRunID( Run *pRun ) const;

	//Retrieves the pointer to the run
	Run	*GetRun( int iRunID ) const;

	//Adds a run to the run list and sets the appropriate numbers
	int AddRun( Run *pRun );

	//copies a run from the autobuild to the end of the last run
	Bay	*AddFromAutoBuildToRun( int iRunID, double dRLs[2] );
	
	//create a basic template for the autobuild to work from
	void CreateDefaultAutoBuildRun();

	//retives a pointer to the Autobuild tools
	AutoBuildTools * GetABTools();

	//find the run which own a particular bay
	Run * GetRunWhichOwnsBay( Bay *pBayFirst );


	///////////////////////////////////////////////////////////////////////////////
	//Bays
	///////////////////////////////////////////////////////////////////////////////

	//This function joins two bays, it is envisioned that this function would be
	//	called by the JoinRuns command above to complete it's task.  This function
	//	belongs here, and not in the run class, nor bay class since it is likely
	//	that the two bays that are to be joined are in two separate Runs.
	bool JoinBays( Bay *pBayFirst, Bay *pBaySecond, JoinTypeEnum JoinType=DEFAULT_JOIN_TYPE );

	//Analysis the bay and redraws from the components useful for when they edit
	//	lifts within a bay, and they have made a change which should be displayed
	//	in the Plan view such as a handrail
	void RedrawSchematic( bool bOnlyUpdateVisited );

	void	UpdateAllBayText();
	Bay		*GetBayFromBayNumber( int iBayNumber );
	bool	SelectBays( BayList &Bays, bool bAllowMultipleSystem );

	//Ladders, stairs & buttress
	void LadderBay();
	void StairBay();
	void ButtressBay();
	Bay *RecreateStairOrLadder(Bay *pBayToRecreate, Bay *pNeighbor, SideOfBayEnum eSideOfNeighbor);
	void ChangeRLs();

	//roof protect
	bool CheckRoofProtectionIsValid( bool bContinueCancel );

	//Goes through each bay and numbers them sequentially
	void RenumberAllBays();


	///////////////////////////////////////////////////////////////////////////////
	//Lapboards
	///////////////////////////////////////////////////////////////////////////////
	LapboardBay * GetLapboard( int iLapboardID );
	
	//Creates a new lapboard and adds it to the list of lapboards
	LapboardBay *CreateNewLapboard( );

	//retrieves the number of lapboards owned by the controller
	int GetNumberOfLapboards() const;

	//controls the initial user interface such that the appropriate function below
	//	is called.
	int	AddNewLapboard( LapboardBay *pLapboard );

	//Creates a new LapboardBay, and attaches to the correct Run.  B4 this can occur
	//	we need to test if the corners are already used, and that the forward/
	//	backward pointer is not being used for the bay
	LapboardBay *AddNewLapboardBayToBay( Bay *pBayFirst, Bay *pBaySecond );

	//deletes an existing lapboards
	void DeleteAllLapboards();
	bool DeleteLapboard( LapboardBay *pLapboard, int iCount=1 );
	bool DeleteLapboard( int iLapboardID, int iCount=1 );

	//ID's
	void ReIDAllLapboards( int iStartID=0 );

	//toeboards on lapboards
	void UseToeboardOnLapboard();
	void SetUseToeboardOnLapboard( bool bUseToeboard );
	bool IsUseToeboardOnLapboard();

	

	///////////////////////////////////////////////////////////////////////////////
	//Components
	///////////////////////////////////////////////////////////////////////////////

	//hidden components
	void SelectHiddenComponentTypes();

	//Get all lenghts from component details file
	double GetAvailableLength( ComponentTypeEnum eType, MaterialTypeEnum eMaterail, double dLength, RoundingTypeEnum eRound=RT_ROUND_UP, bool bNotFoundReturnNearest=false );

	//Add/Delete components
	void AddBracingToBay();
	void AddChainLinkToBay();
	void AddCornerStageBoard();
	void AddHopupsToBay();
	void AddHandrailsToBay();
	void AddIndividualCornerFiller( Bay *pBay, SideOfBayEnum eSide, int iWidthLeft=-1, int iWidthRight=-1 );
	void AddShadeClothToBay();
	void AddTieToBay();
	void DeleteBracingFromBays();
	void DeleteChainLinkFromBay();
	void RemoveCornerStageBoard();
	void DeleteHandrails();
	void DeleteHopups();
	void DeleteShadeClothFromBay();
	bool DeleteTiesFromBay();

	//Other 
	void ClearCommitted();


	///////////////////////////////////////////////////////////////////////////////
	//Surface Areas
	///////////////////////////////////////////////////////////////////////////////

	//Queries the Runs and the Lapboards to get the Surface Area required.
	double GetSurfaceArea() const;

	
	///////////////////////////////////////////////////////////////////////////////
	//Stage&Level
	///////////////////////////////////////////////////////////////////////////////
	void	ClearStages();
	int		AddStage( CString sStage );
	CString GetStage( int iIndex );
	int		GetNumberOfStages();

	// selects and assigns stages to bays and levels
	bool SetStageForSelectedBays();

	void CreateListOfUniqueSAndLs( SummaryInfo &sSLList );
	bool ShowScaleLines();
	bool ShowFirstScale();
	bool ShowLongLevelLine();
	void ToggleScaleLines();
	void ToggleFirstScale();
	void ToggleLvlLength();

	//Level stuff
	BayStageLevelArray *GetBayStageLevelArray();
	void RegenMatrixLevels( bool bShowWarning=true );
	bool RemoveLevel( int iLevelID, int nCount = 1 );
	bool RemoveLevel( double dRL );
	void AddLevel( double dRL=0.00 );
	LevelList *GetLevelList();

	//Colouration by stage/level
	void ColourByStageLevel();
	bool IsColourByStageLevel();
	void SetColourByStageLevel(bool bColourByStageLevel);
	

	///////////////////////////////////////////////////////////////////////////////
	//Matrix
	///////////////////////////////////////////////////////////////////////////////
	//Show/Hide
	int			HowManyElementsShownBefore( int iElement );
	void		HideMatrixElement( int iElementID );
	bool		ShowMatrixElement( int iElementID );
	void		ExcludeMatrixElements();
	void		SelectMatrixElements( intArray &iaElements );
	void		ShowAllMatrixElements();
	intArray	*GetMatrixElementToDisplay();

	//Matrix crosshairs
	Matrix3D GetMatrixCrosshairTransform( );
	void SetMatrixCrosshairTransform( Matrix3D Transform );
	void MoveMatrixLastPoint( Matrix3D Transform );

	//Cut through crosshairs
	Matrix3D GetCutThroughCrosshairTransform();
	void SetCutThroughCrosshairTransform(Matrix3D Transform);

	//retrieves the number of lapboards owned by the controller
	int GetNumberOfMatrixElements() const;

	//Create a new matrix object
	bool CreateMatrix(bool bUseLast=false);

	//Creates a new matrix element
	MatrixElement *CreateMatrixElement(Matrix *pMatrix=NULL);

	//Obtains a point to the Matrix Object
	Matrix *GetMatrix() const;

	//Destroys the matrix
	bool DeleteMatrix();
	bool DeleteMatrixElements( int iMatrixElementID, int iCount=1 );

	//Redraw the Matrix View
	bool RedrawMatrix() const;

	
	///////////////////////////////////////////////////////////////////////////////
	//The 3D View
	///////////////////////////////////////////////////////////////////////////////

	//destroys the entities and reactors
	void Delete3DView();

	//Creates the entities and reactors, and allows them to be continuously redrawn
	void Create3DView(bool bUseLast=false);

	//Are there some components/bays/runs/etc that can be deleted?
	DirtyFlagEnum GetDirtyFlag() const;
	void SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward=false );

	//deletes all objects, that this object owns, if they are marked for deletion
	void CleanUp( bool bShowProgressBar=true );

	void Move3DLastPoint( Matrix3D Transform );


	///////////////////////////////////////////////////////////////////////////////
	//Redraws
	///////////////////////////////////////////////////////////////////////////////
	//Same as Autocad 'regen'
	void RefreshScreen();
	void ClearAllowDraw();
	
	///////////////////////////////////////////////////////////////////////////////
	//BOM/BOMExtra
	///////////////////////////////////////////////////////////////////////////////
	//BOM
	void DoBOMSummary();
	void DisplayBOMExtra();
	//prompts the user for a file name and creates the BOM
	void BOM();


	///////////////////////////////////////////////////////////////////////////////
	//3D Origin Crosshair
	///////////////////////////////////////////////////////////////////////////////
	bool IsCrosshairCreated();
	void SetCrosshairCreated( bool bCreated );

	void Create3DCrosshair();
	void Destroy3DCrosshair();

	//Sets/Gets the transformation which is applied to all 3D objects
	void Set3DCrosshairTransform( Matrix3D Transform );
	Matrix3D Get3DCrosshairTransform( ) const;

	//Moves all 3D view elements by this matrix, this movement tranfrom can be
	//	stored for future referance
	void Move3DCrosshair( Matrix3D Transform, bool bStoreMovement=true );

	//Moves this object back by the inverse of the stored transformation, and
	//	returns the original transformation, incase you want to use it again
	Matrix3D UnMove3DCrosshair();


	///////////////////////////////////////////////////////////////////////////////
	//Component details/Stock files
	///////////////////////////////////////////////////////////////////////////////

	//Component details
	//Gets the details for a component which are stored in the
	//	ComponentDetailsArray, which is loaded from a file
	//	(normally ComponentDetails.csv) the actual name of the
	//	file is stored in the registry, so this may change if
	//	the file is not found
	ComponentDetailsArray *GetCompDetails();
	void SetComponentDetailsLoaded( bool bLoaded=true );
	bool IsComponentDetailsLoaded();

	//Stock file
	//Gets the price and weight details for a component which are stored in the
	//	StockListArray, which is loaded from a file
	//	(normally Stock.csv) the actual name of the
	//	file is stored in the registry, so this may change if
	//	the file is not found
	StockListArray *GetStockList();
	void SetStockFileLoaded( bool bLoaded=true );
	bool IsStockFileLoaded();

	AcDbObjectIdArray m_objIDSaveSelection;

protected:
	bool m_bRLCalcPossible;
	Point3D m_ptKwikRLs[KRL_SIZE];
	bool m_bSaveSelected;
	bool m_bIgnorPerRoll;

	/////////////////////////////////////////////////////////////////////////
	//Per roll
	bool	m_bCalcPerRollChainMesh;
	bool	m_bCalcPerRollShadeCloth;
	double	m_dRollSizeChainMesh;
	double	m_dRollSizeShadeCloth;
	CString	m_sPartNumberChainMesh;
	CString	m_sPartNumberShadeCloth;
	RoundingDevisorEnum 	m_eRDShadeCloth;
	RoundingDevisorEnum 	m_eRDChainMesh;
	/////////////////////////////////////////////////////////////////////////

	bool m_bKwikScafBlockInserted;

	//Inserting Block stuff
	bool m_bInsertingBlock;
	int m_iVisualCompsB4Insert;
	int m_iBOMExtraB4Insert;
	int m_iLapboardsB4Insert;
	int m_iBaysB4Insert;
	int m_iRunsB4Insert;

	double m_dLastComponentRotationAngle;
	Point3D m_ptLastComponentRotatePoint[2];
	CStringArray m_saSaveHistory;
	bool m_bConvertToSystemOnNextOpen;
	bool m_bBOMSummaryToShowBays;
	bool m_bBOMSummaryToShowBaySizes;

	//Last Insert Component Details
	CString				m_sLastInsertComponentPartNumber;
	SystemEnum			m_eLastInsertComponentSystem;
	MaterialTypeEnum	m_eLastInsertComponentMaterial;
	ComponentTypeEnum	m_eLastInsertComponentType;
	double				m_dLastInsertComponentLength;

	bool m_bDisplayZigZagLine;
	SystemEnum	m_eSystem;
	bool m_bShowStandardLengths;
	bool m_bUse0700TransomForLedger;
	bool m_bUse1200TransomForLedger;
	bool m_bShowMatrixLabelsOnSchematic;
	CString m_sUserName;
	int m_iSuggestedCutThroughLabel;
	bool m_bUsePutLogClipsInLadder;
	intArray	m_ia3DBaysSelected;
	bool	m_bShowTypicalMatrixSectionsOnly;
	intArray	m_iaMatrixBaysSelected;
	bool	m_bOpeningDocument;

#ifdef _DEBUG
	CArray<Component*, Component*> m_clComponentDebugger;
#endif

	//Set RL dialog value
	double m_dLastRLSetValue;

	//Radius of standard in schematic
	double m_dRadiusOfSchematicStandard;

	//arc lapboard span
	double m_dArcLapboardSpan;

	//Hidden Components & Matrix Elements
	intArray m_iaComponentsToHide;
	intArray m_iaInvisibleMatrixElement;

	//Flag to say shutting down (don't allow draw, etc)
	bool m_bDestructingController;

	//Flags to enable/disable features
	bool m_bShowFirstScale;
	bool m_bShowScaleLines;
	bool m_bDisplaySalePrice;
	bool m_bColourByStageLevel;
	bool m_bShowLongLevelLength;
	bool m_bShowingEditStandards;
	bool m_bUseLowestHopupForWallSeparation;

	//Sydney corner
	bool m_bSydneyCorner;
	double m_dSydneyCornerDistBwBays;

	//Entity ID stuff
	intArray			m_iaEntityLevelIDs;
	intArray			m_iaEntityLapboardIDs;
	intArray			m_iaEntityBayNumberIDs;
	EntityTypeEnum		m_eEraseTypeEnum;
	EntityTypeEnum		m_eMovementEntityType;

	//Entity Reactor
	bool				m_bIgnorMovement;
	bool				m_bIgnorErase;
	Component			*m_MovedComponent;
	AcDbObjectIdArray	m_oiaCopiedObjects;

	//Actual vs Wireframe
	bool					m_bActualEntitiesLoaded;
	UseActualComponentsEnum	m_uaceUseActualComponents;

	//list of names available for stages
	StageLevelVisible	m_StageNames;
	LevelList			m_daLevels;
	BayStageLevelArray	m_StageListArray;//The mapping of stage and levels and bays

	//Is the file loaded already
	bool				m_bStockFileLoaded;
	bool				m_bComponentDetailsLoaded;

	//not sure
	int					m_iGroupNumber;

	/////////////////////////
	/////////////////////////
	///////Attributes////////
	/////////////////////////
	/////////////////////////

	RunList					m_raRuns;					//The list of runs, not including the Autobuild run
	LapboardList			m_Lapboards;				//The list of Lapboards	
	Matrix					*m_pMatrix;					//The Matrix list
	AutoBuildTools			m_AutoBuildTool;			//The autobuild run and bay template
	Matrix3D				m_3DCrosshairTransform;		//The Transformation matrix for the 3D View
	Matrix3D				m_MatrixCrosshairTransform;
	Matrix3D				m_CutThroughCrosshairTransform;
	ComponentDetailsArray	m_ltCompDetails;			//The table of component details loaded from the .csv file
	ComponentList			m_BOMExtraCompList;			//The table of component details loaded from the .csv file
	VisualComponents		m_vcComponents;
	JobDescriptionInfo		m_JobDescripInfo;			//The information stored with the drawing containing the job information.
	DirtyFlagEnum			m_dfDirtyFlag;				//Has any of the components/Bay/Lifts/etc been deleted?
	StockListArray			m_ltStockList;

	/////////////////////////////////////////////////////
	//crosshair
	Entity			*m_pCrosshair;			//The 3D View Origin crosshair entity
	EntityReactor	*m_pCrosshairReactor;	//A rector for the crosshair
	bool			m_bCrosshairCreated;	//Has the crosshair been created?

private:
	void SetRLCalcPossible( bool bPossible );
	void StorePerRollInRegistry();
	////////////////////////
	////////////////////////
	//Hidden functionality//
	////////////////////////
	////////////////////////
	bool GetMillsTypeStdFromCommandLine( BayList *pBays, MillsType &mtType, MillsType mtAvailableTypes);
	bool GetMillsTypeGapFromCommandLine( BayList *pBays, MillsType &mtType, MillsType mtAvailableTypes);
	bool GetSizeOfBayFromCommandLine( double &dWidth, double &dLength, double &dHeight, bool &bWidthSet, bool &bLenghtSet, bool &bHeightSet, DimensionSelectionEnum eDim=DS_LENGTH );
	bool GetWidthOfCornerFillerFromCommandLine( SideOfBayEnum eSide, int &iNumberOfBoards, SideOfBayEnum eSetSide, int iSetNumberOfBoards );
	bool GetSideOfBayAndNumBoardsFromCommandLine( SideOfBayEnum &eSide, int &iNumberOfBoards, BayList *pBays, bool &bMatchToDecks );
	bool GetSideOfBayAndHRStyleFromCommandLine( SideOfBayEnum &eSide, HandrailStyleEnum &eHandRailStyle, BayList *pBays );
	bool GetSideOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays );
	bool GetCornerOfBayFromCommandLine( SideOfBayEnum &eSide, BayList *pBays );
	bool GetPositionOfBayFromCommandLine( SideOfBayEnum &eSide, TieTypesEnum &eTT, BayList *pBays );
	bool GetApplyToRunOrBay( bool &bApplyToRuns );
	int	GetSideOfBayFromPoint( BayList *pBays, Point3D ptPoint, intArray iaAvailableSides, SideOfBayEnum &eSideClicked );
	int	GetPoint3D( CString sPrompt, Point3D &ptPoint, bool bTurnSnapsOffTemporarily=true );
	int	GetPoint( LPCTSTR strPrompt, Point3D &point );

	void RemoveAndWarnOfCommittedBays( BayList *pBays );
	MatrixElement * GetMatrixElementPointerForEntityId(AcDbObjectId &Id);
	Acad::ErrorStatus selectPolymeshes(AcDbObjectIdArray& idArray, ads_name selection);
	Point3D m_ptMatrixPosition;
	Point3D m_pt3DPosition;
	void SetWidthForBay( Bay *pBay, double dNewWidth, bool bAllowMovement );
	void CheckForStairLadder( BayList *pBays, bool bShowWarning=true, bool bIgnorButtress=false );
	void ApplyToSelection( Bay *pBay, BayList *pBays );
	bool ApplyRLChangeToSequence( BayList *pBays );
	void AddComponentToSummaryInfo( SummaryInfo &sSLList, Component *pComponent=NULL );
	bool GetVerticalTieSpacing( double &dVertical, double dDefaultSpacing );
	bool GetNumberOfBaysPerTie( Run *pRun, int &iBaysPerTie );
	void ApplyTieToBay( Bay *pBay, SideOfBayEnum eSide );
	void ApplyTieToRun( Run *pRun, SideOfBayEnum eSide, RunList &CompletedRuns );
	bool GetRangeToApplyTies( Bay *pBay, int &iResponse, int iNumberOfRunsRemaining );
	void SetTemplateTiesForBay( Bay *pBay, SideOfBayEnum eSide );
	bool AddRunToUniqueList( Run *pRun, RunList &Runs );

	//cleans up memory
	void DestroyGlobals();

	//set all global variables to reasonable 'empty' values
	void SetGlobalsToDefault();

	//checks the bounds for a runid
	bool IsRunIDValid(int iRunID) const;
};

#endif // !defined(AFX_CONTROLLER_H__9F66E165_43A6_11D3_885D_0008C7999B1D__INCLUDED_);

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: controller.h $
 * 
 * *****************  Version 165  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 164  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 163  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 162  *****************
 * User: Jsb          Date: 16/11/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * About to release KwikScaf version 1.1k
 * 
 * *****************  Version 161  *****************
 * User: Jsb          Date: 9/11/00    Time: 12:17p
 * Updated in $/Meccano/Stage 1/Code
 * Building r1.1j
 * 
 * *****************  Version 160  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 159  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 158  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 157  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 156  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 155  *****************
 * User: Jsb          Date: 27/09/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * This is most likely the version we are sending to Perth for Testing of
 * 3D components
 * 
 * *****************  Version 154  *****************
 * User: Jsb          Date: 18/09/00   Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * Finished Bill Cross Stage 3
 * 
 * *****************  Version 153  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 152  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 150  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 149  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 148  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 147  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 146  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 145  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 144  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 143  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 141  *****************
 * User: Jsb          Date: 9/08/00    Time: 10:04a
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8x
 * 
 * *****************  Version 140  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 139  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 138  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 137  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 136  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 135  *****************
 * User: Jsb          Date: 27/07/00   Time: 1:07p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 8p
 * 
 * *****************  Version 134  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 133  *****************
 * User: Jsb          Date: 21/07/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly completed the SetDecks to Levels bug
 * 
 * *****************  Version 132  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 131  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 130  *****************
 * User: Jsb          Date: 6/07/00    Time: 12:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 129  *****************
 * User: Jsb          Date: 5/07/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create 0.5m, 0.5m Stage, and 0.8m Stage standards
 * 
 * *****************  Version 128  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 127  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 126  *****************
 * User: Jsb          Date: 26/06/00   Time: 4:49p
 * Updated in $/Meccano/Stage 1/Code
 * auto changing heights of adjoining stairs and ladders
 * 
 * *****************  Version 125  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 124  *****************
 * User: Jsb          Date: 21/06/00   Time: 12:43p
 * Updated in $/Meccano/Stage 1/Code
 * trying to locate the problem with loading the Actual comps into paper
 * space
 * 
 * *****************  Version 123  *****************
 * User: Jsb          Date: 19/06/00   Time: 1:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 122  *****************
 * User: Jsb          Date: 8/06/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Rotation of Bays is operational, rotation of laps is nearly working
 * 
 * *****************  Version 121  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 119  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 118  *****************
 * User: Jsb          Date: 2/06/00    Time: 3:02p
 * Updated in $/Meccano/Stage 1/Code
 * need to find why tietubes are removing hopups
 * 
 * *****************  Version 117  *****************
 * User: Jsb          Date: 30/05/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 116  *****************
 * User: Jsb          Date: 29/05/00   Time: 4:34p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 115  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 114  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 113  *****************
 * User: Jsb          Date: 18/05/00   Time: 5:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 112  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 111  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 110  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 109  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 108  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 107  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 106  *****************
 * User: Jsb          Date: 17/04/00   Time: 4:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 105  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 104  *****************
 * User: Jsb          Date: 14/04/00   Time: 9:57a
 * Updated in $/Meccano/Stage 1/Code
 * Colour By Stage & level
 * 
 * *****************  Version 103  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 102  *****************
 * User: Jsb          Date: 11/04/00   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 101  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 100  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 99  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 98  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 97  *****************
 * User: Jsb          Date: 27/03/00   Time: 4:30p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 96  *****************
 * User: Jsb          Date: 16/03/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 95  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 94  *****************
 * User: Jsb          Date: 9/03/00    Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 93  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 92  *****************
 * User: Jsb          Date: 29/02/00   Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 91  *****************
 * User: Jsb          Date: 25/02/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 90  *****************
 * User: Jsb          Date: 23/02/00   Time: 2:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 89  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 88  *****************
 * User: Jsb          Date: 16/02/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Big Day, just fixed Marks save bug, sydney corners operational
 * 
 * *****************  Version 87  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 86  *****************
 * User: Jsb          Date: 9/02/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 85  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 84  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 83  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 10/01/00   Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 7/01/00    Time: 4:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12 (Beta13)
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 4/01/00    Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 22/12/99   Time: 4:31p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 16/12/99   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Finished adding function:
 * AddShadeCloth
 * AddHopup
 * AddChainLink
 * AddHandrail
 * plus delete functions for the same!
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and _tremove handrails and hopups
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 16/12/99   Time: 10:26a
 * Updated in $/Meccano/Stage 1/Code
 * ChangeRLDlg now compilers, however I need to change the name of the
 * file
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 15/12/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Adjusting RL's nearly finished, just need a dialogbox or other UI to do
 * it!
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 14/12/99   Time: 2:13p
 * Updated in $/Meccano/Stage 1/Code
 * About to build Beta11
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 8/12/99    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * Over and Underruns working correctly
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 6/12/99    Time: 3:58p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way delete bay works
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 8/11/99    Time: 11:27a
 * Updated in $/Meccano/Stage 1/Code
 * Lapboard point to point now operational
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:53a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 2/11/99    Time: 10:57a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Dar          Date: 19/10/99   Time: 10:59a
 * Updated in $/Meccano/Stage 1/Code
 * added stock.csv file for BOM summary report so hire/sale pricing and
 * weight can be calculated.
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 15/10/99   Time: 1:26p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Ghosting of Shematic bays fixed
 * 2) Standards configuration & Fit
 * 3) Matrix Crosshair postion stored
 * 4) Bracing not needed if stage boards used
 * 5) Schematic offset from mouseline
 * 6) Schematic view not showing stair or ladder
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 12/10/99   Time: 9:32a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Level Assignment now operational - but have to call _T("RegenLevels")
 * 2) Fixed the Schematic View Serialize error.
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 8/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * RLScale, and Levels are nearly working
 * 
 * *****************  Version 48  *****************
 * User: Dar          Date: 6/10/99    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 5/10/99    Time: 1:35p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly finished SummaryInfo class
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 5/10/99    Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 4/10/99    Time: 1:44p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 28/09/99   Time: 1:34p
 * Updated in $/Meccano/Stage 1/Code
 * About to move the schematic data to the PreviewTemplate
 * 
 * *****************  Version 42  *****************
 * User: Dar          Date: 9/23/99    Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * basic load save of job description info in NOD needs work still
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 23/09/99   Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * All 3D Component Entity drawing now use the same path, and it will
 * prompt them for that path if it can't find it
 * 
 * *****************  Version 40  *****************
 * User: Dar          Date: 9/22/99    Time: 1:45p
 * Updated in $/Meccano/Stage 1/Code
 * job details load save done a little more work on ui needed
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 38  *****************
 * User: Dar          Date: 9/21/99    Time: 10:19a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 21/09/99   Time: 10:00a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:05a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 35  *****************
 * User: Dar          Date: 9/21/99    Time: 8:17a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 15/09/99   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * checked in by request of Andrew Taylor for backup purposes
 * 
 * *****************  Version 32  *****************
 * User: Dar          Date: 9/15/99    Time: 3:33p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 30  *****************
 * User: Dar          Date: 9/07/99    Time: 3:44p
 * Updated in $/Meccano/Stage 1/Code
 * serialize added to controller
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 9/03/99    Time: 11:01a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/12/99    Time: 1:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/05/99    Time: 4:43p
 * Updated in $/Meccano/Stage 1/Code
 * Taking files home to see If I can compile code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 3/08/99    Time: 10:51
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing the Autobuild
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 29/07/99   Time: 15:56
 * Updated in $/Meccano/Stage 1/Code
 * I will be away friday
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 29/07/99   Time: 12:18
 * Updated in $/Meccano/Stage 1/Code
 * Completing functionality for Controller class
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 29/07/99   Time: 10:05
 * Updated in $/Meccano/Stage 1/Code
 * I have now added a test routine to the controller which calls the
 * _T("LiftList.Test") function
 * I have added operator functions to the Run class
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 28/07/99   Time: 12:52
 * Updated in $/Meccano/Stage 1/Code
 * Still working on filling in the Run class, added all the const values
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 28/07/99   Time: 9:41
 * Updated in $/Meccano/Stage 1/Code
 * Filling in the Run class functionality
 * 
 *******************************************************************************/
