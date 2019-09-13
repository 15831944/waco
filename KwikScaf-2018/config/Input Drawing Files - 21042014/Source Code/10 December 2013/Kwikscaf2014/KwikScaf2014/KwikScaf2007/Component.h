// Component.h: interface for the cComponent class.
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

#include "ComponentDefinitions.h"	// Added by ClassView
#if !defined(AFX_COMPONENT_H__4F7AB303_39B2_11D3_8A4C_00508B043A6C__INCLUDED_)
#define AFX_COMPONENT_H__4F7AB303_39B2_11D3_8A4C_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//includes
#include "MeccanoDefinitions.h"
#include <dbmain.h>
#include <acdb.h>
#include "EntityReactor.h"	// Added by ClassView


//forward declarations
class Lift;
class Bay;
class Run;
class Controller;
class Entity;
class EntityReactor;
class BOMExtra;
class MatrixElement;
class StockListElement;
class VisualComponents;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//									COMPONENT CLASS
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 22/11/00 3:41p $
//	Version			: $Revision: 82 $
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	Components are the basic building blocks of the Meccano system.  
//	Each instantiation of the Component Class has one physical object 
//	associated with it, e.g. a single Ledger or Transom.  
//	Each component needs to know which Lift and Bay it belongs to.  
//	So that can refer back to them for filtering of the BOM.
//////////////////////////////////////////////////////////////////////

#include "BayDefinitions.h"	// Added by ClassView

const int	KWIKSCAF_COMPONENT_ID = 0xabcd;

#include "GraphicTypes.h"	// Added by ClassView
class Component : public AcDbObject 
{
public:
	int	m_iKwikScafComponent;
	bool IsComponentSelected();
	CString GetTextSmart();
	bool GetVisualTransformUsed();
	void SetVisualTransformUsed( bool bUsed );

	//visual transform
	bool VisualMove( Matrix3D VisualTransform, bool bStoreMovement=true, bool bMoveEntity=true );
	void SetVisualTransform( Matrix3D VisualTransform );
	Matrix3D GetVisualTransform();

	Matrix3D GetEntireTransform();
	double GetStarSeparation() const;
	void SetSystem( SystemEnum eSystem );
	SystemEnum GetSystem() const;
	void StoreLastPosition();
	void ComponentMove();
	Vector3D GetLastDirection();
	Vector3D GetLastDirectionAlt();
	Vector3D GetLastPosition();
	void ResetXDataPositions();
	void SetXData();
	void ApplyVisibility( VisibilityEnum eVisible=VIS_USE_DEFAULT );
	void SetVisualComponentsPointer( VisualComponents *pVC );
	VisualComponents *GetVisualComponentsPointer() const;
	int GetColour();

	void SetStockDetailsPointer();
	void SetStockDetailsPointerDumb( StockListElement *pElement );

	StockListElement *GetStockDetails() const;
	double GetAcurate3DAdjust( AxisEnum Axis, bool bMatrixElementSideOn=true );
	void SetRemoveComponentFromBOM( bool m_bRemove );
	bool GetRemoveComponentFromBOM();
	CString GetText();
	void SetCommitted( bool bCommitted=true );
	bool IsCommitted();
	int GetLevelForRL( double dRelativeLevel ) const;
	SideOfBayEnum GetSideOfBay( ) const;
	int GetRise() const;
	void SetPreviousLength( double dLength );
	double GetPreviousLength();
	void SetController( Controller *pController );
	ACRX_DECLARE_MEMBERS(Component);

	UseActualComponentsEnum IsUseWireFrame();
	void SetTextHeightMultiplier(double dMultiplier);
	void SetColour( int iColour );
	void SetTextHorizontalMode( AcDb::TextHorzMode HorzMode );
	void SetTextVerticalMode( AcDb::TextVertMode VertMode );
	Component();

	bool GetDrawMatrixElementSideOn( );
	void SetDrawMatrixElementSideOn( bool bSideOn );
	MatrixElement *m_pElement;
	MatrixElement *GetMatrixElementPointer() const;
	void SetMatrixElementPointer( MatrixElement *pElement );
	EntityReactor *GetReactor() const;
	void SetReactor( EntityReactor *pReactor );
	Entity * GetEntity() const;
	void SetEntity( Entity *pEntity );
    virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;

	void SetText( CString &sText );
	void CreatePointsIfPossible();

	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	void Serialize(CArchive &ar);

	//////////////////////////////////////////////////////////////////
	//Has the been marked for deletion
	//////////////////////////////////////////////////////////////////
	//Says this object needs to be deleted
	DirtyFlagEnum	GetDirtyFlag() const;
	//set the object to be deleted
	void	SetDirtyFlag( DirtyFlagEnum dfDirtyFlag );


	Matrix3D UnMove();
	Matrix3D GetOtherMatrix() const;
	Matrix3D GetBayMatrix() const;
	Matrix3D GetLiftMatrix() const;
	Matrix3D GetTransform() const;
	void SetTransform( Matrix3D Transform );
	bool GetMoveWhenBayWidthChanges() const;
	bool GetMoveWhenBayLengthChanges() const;
	void SetMoveWhenBayWidthChanges( bool bMove=true );
	void SetMoveWhenBayLengthChanges( bool bMove=true );
	void DeleteEntity();
	void ApplyBayResize( Matrix3D TransformX, Matrix3D TransformY );
	////////////////////
	////////////////////
	////Functionality///
	////////////////////
	////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	// construction/destruction
	/////////////////////////////////////////////////////////////////////////////////
	Component( double dLength, ComponentTypeEnum eComponentType, MaterialTypeEnum eMaterialType, SystemEnum eSystem );
	virtual ~Component();
	//Deletes the object by notifying its parent object it will be destroyed
	void Remove();

	/////////////////////////////////////////////////////////////////////////////////
	// set component attribute functions
	/////////////////////////////////////////////////////////////////////////////////
	void SetID( int iID );
	void SetComponent( double dLength, ComponentTypeEnum eComponentType, MaterialTypeEnum eMaterialType );
	void SetType( ComponentTypeEnum eComponentType );
	void SetMaterialType( MaterialTypeEnum eMaterialType );
	//Stage/Level
	void SetStage( CString sStage );
	void SetLevel( int iLevel );
	//Set the bounding box for the component, where the component will extend to
	void SetLengthCommon( double dLength );
	//Changes the size of the m_pEnt object, within the bounding box,
	//	the size of the object is always shrunk toward the first point
	//	int the points array.
	void ApplySizeChange();
	//Creates a new component, moves it to a new location, and returns a pointer to it
	bool Copy( Matrix3D TransformationMatrix, bool bMove = true ) const;
	//for copying a component
	Component &operator =(const Component &Original);
	//Moves the m_pEnt pointer, and stores the movement in the Transform matrix
	bool Move( Matrix3D TransformationMatrix, bool bStoreMovement=true, bool bMoveEntity=true );

	/////////////////////////////////////////////////////////////////////////////////
	// attribute access functions
	/////////////////////////////////////////////////////////////////////////////////

	//Gets the ID of the component(determined by the index of the component in
	//	the lift list[only if the component is in the lift list]).
	int GetID() const;
	//retrieves all the iID which are relevent for this component
	bool GetIDs( int &iComponentID, int &iLiftID, int &iBayID, int &iRunID ) const;

	MaterialTypeEnum GetMaterialType() const;
	double GetWeightActual( bool bShowWarning=true ) const;
	//Get the bounding box for the component
	double	GetLengthCommon() const;
	double	GetLengthActual( bool bShowWarning=true ) const;
	double	GetHeightActual( bool bShowWarning=true ) const;
	double	GetWidthActual( bool bShowWarning=true ) const;
	//Stage/Level
	int		GetLevel() const;
	CString	GetStage() const;
	//component detail
	void	GetComponentDetails( double &dLength, double &dWeight, ComponentTypeEnum &eComponentType, MaterialTypeEnum &eMaterialType ) const;
	//the z value of the lowest object
	double	GetRL() const;
	double	GetRLActual() const;
	ComponentTypeEnum GetType() const;

	/////////////////////////////////////////////////////////////////////////////////
	// bay functions 
	/////////////////////////////////////////////////////////////////////////////////
	Bay	   *GetBayPointer() const;
	void	SetBayPointer( Bay *pSetBay );

	/////////////////////////////////////////////////////////////////////////////////
	// lift functions 
	/////////////////////////////////////////////////////////////////////////////////
	Lift   *GetLiftPointer() const;
	void	SetLiftPointer( Lift *pSetLift);

	/////////////////////////////////////////////////////////////////////////////////
	//BOMExtra stuff
	/////////////////////////////////////////////////////////////////////////////////
	BOMExtra *GetBOMExtraPointer() const;
	void SetBOMExtraPointer( BOMExtra *pBOMExtra );

	/////////////////////////////////////////////////////////////////////////////////
	// node functions 
	/////////////////////////////////////////////////////////////////////////////////
	Point3D GetNode( int iNodePos ) const;
	int		AddNode( Point3D NodeToAdd );
	bool	DeleteNodes();

	/////////////////////////////////////////////////////////////////////////////////
	// point functions 
	/////////////////////////////////////////////////////////////////////////////////
	//bool	SetPoint( ComponentPointsEnum eWhichPoint, Point3D ptPoint );
	//Point3D GetPoint( ComponentPointsEnum eWhichPoint ) const;
	bool SetPoint( int iWhichPoint, Point3D ptPoint );
	Point3D GetPoint( int iWhichPoint ) const;
	//This function is now superseeded by the CreatePoints funciton
	bool SetPoints( Point3D pt1stPoint, Point3D pt2ndPoint, Point3D ptNode );
	//Creates the points depending on the type, will set height and width again according
	//	the the type, but length must already be set.
	bool CreatePoints();
	//adds a point to the points array
	int AddPoint( Point3D ptPoint );

	/////////////////////////////////////////////////////////////////////////////////
	//Visibility
	/////////////////////////////////////////////////////////////////////////////////
	VisibilityEnum GetVisible() const;
	void SetVisible( VisibilityEnum eVisible=VIS_VISIBLE );

	/////////////////////////////////////////////////////////////////////////////////
	// other
	/////////////////////////////////////////////////////////////////////////////////
	//This function seems to be superseeded
	bool Create3DView();
	void Delete3DView();

	Controller	*GetController() const;

	//This will create the entity based on the component type and the points array.
	//	Requires a valid points array for its type and also the previous Entity object
	//	should have been deleted by now!
	void CreateEntity( UseActualComponentsEnum eUse );
	//BOM create
	bool WriteBOMTo( CStdioFile *pFile ) const;

protected:
	bool m_bVisualTransformUsed;
	Matrix3D m_VisualTransform;
	Vector3D m_LastDirection;
	Vector3D m_LastDirectionAlt;
	Vector3D m_LastPosition;
	VisualComponents	*m_pvcComponents;
	int					m_iSettingToDefault;
	StockListElement	*m_pStockElement;
	SystemEnum			m_eSystem;
	bool m_bRemoveComponentFromBOM;

	bool				m_bCommitted;
	
	int					m_iColour;

	double				m_dTextHeightMultiplier;
	CString				m_sText;
	AcDb::TextHorzMode	m_TextHAlign;
	AcDb::TextVertMode	m_TextVAlign;

	bool				m_DrawMatrixElementSideOn;

	bool				m_bMoveWhenBayWidthChanges;
	bool				m_bMoveWhenBayLengthChanges;
	double				m_dPreviousLength;

	////////////////////
	////////////////////
	/////Attributes/////
	////////////////////
	////////////////////
	//list of points to create the m_pEnt from!
	//	NOTE: These points should not be moved, 'm_Transform' store
	//	the movement info
	Point3DArray			m_ptPoints3D;
	//clutch points
	Point3DArray			m_Nodes;
	//the position within the bay or lift owned array
	int						m_iID;

	double					m_dLength;

	//plank, transom, etc
	ComponentTypeEnum		m_eComponentType;
	//steel wood etc
	MaterialTypeEnum		m_eMaterialType;
	VisibilityEnum			m_eVisible;
	//A Component may belong to either a lift, a bay, or the BOMExtra
	Lift				   *m_pLift;
	Bay					   *m_pBay;
	Controller			   *m_pController;
	BOMExtra			   *m_pBOMExtra;
	//Stage/Level
	CString					m_sStage;
	int						m_iLevel;

	//This method seems to work so far
	Entity				   *m_pEnt;
	EntityReactor		   *m_pReactor;

	//This stores all movement to the m_pEnt object, so that we can
	//	Change the original object (m_pEnt) and move it straight back to it
	//	orientation and position quickley
	Matrix3D				m_Transform;

	//does this object need to be deleted
	DirtyFlagEnum			m_dfDirtyFlag;


private:
	void SetVariablesToDefault();
	Lift		*GetLift() const;
	Bay			*GetBay() const;
	Run			*GetRun() const;

	bool IsAPointerSet() const;
	void SetAllPointerToNULL();
	//sets up some points to create a rectangular prism via a polyline, the prism
	//	will be the same dimensions as the bounding box, see lenght,width,height
	void CreatePointsRectanglePrism();
	void CreatePointsRectanglePrism( double dLength, double dWidth, double dHeight, double dLengthOffset=0.00, double dWidthOffset=0.00, double dHeightOffset=0.00 );
	void CreatePointsRectangleMesh();
	void CreatePointsRectangleMesh(double dLength, double dWidth, double dHeight, double dLengthOffset=0.00, double dWidthOffset=0.00, double dHeightOffset=0.00 );
	void CreatePointsTriangularPrism();
	void CreatePointsTriangularPrism(double dLength, double dWidth, double dHeight, double dLengthOffset=0.00, double dWidthOffset=0.00, double dHeightOffset=0.00);
	void CreatePointsTriangularMesh();
	void CreatePointsTriangularMesh(double dLength, double dWidth, double dHeight, double dLengthOffset=0.00, double dWidthOffset=0.00, double dHeightOffset=0.00);

	//Clears the points array
	void DeletePoints();
	//how many points are there?
	int GetNumberOfPoints() const;
	//creates the DB structure for the m_pEnt object to be attached to
	Acad::ErrorStatus postToDatabase( Entity *pEnt );
	//Returns the actual length, given a length, using this component's
	//	type and material, includes at mills or quickscaff.
	double FindLengthActual( double dLength, bool bShowWarning ) const;
};

typedef	CArray<Component*, Component*>  CompArray;

#endif // !defined(AFX_COMPONENT_H__4F7AB303_39B2_11D3_8A4C_00508B043A6C__INCLUDED_)

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Component.h $
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 22/11/00   Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * I have now got WBlock operational, also I have fixed Andrew Smith's 3D
 * components to new layer bug fixed.
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 22/11/00   Time: 10:43a
 * Updated in $/Meccano/Stage 1/Code
 * About to create KwikScaf 1.1l (release 1.5.1.12)
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 20/10/00   Time: 4:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 21/09/00   Time: 3:25p
 * Updated in $/Meccano/Stage 1/Code
 * built 1.0m
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 20/09/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Acad rotation is now working with individual components just need to
 * store b4 release to WA
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going _T('forwarding')
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 25/07/00   Time: 1:09p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix with exact positioning is not finished
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 25/02/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 9/02/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 20/12/99   Time: 5:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 15/12/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Adjusting RL's nearly finished, just need a dialogbox or other UI to do
 * it!
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 5/11/99    Time: 8:53a
 * Updated in $/Meccano/Stage 1/Code
 * Updating code from Home
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 28/10/99   Time: 1:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Dar          Date: 9/09/99    Time: 9:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 34  *****************
 * User: Dar          Date: 8/31/99    Time: 3:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 8/19/99    Time: 3:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 8/19/99    Time: 2:55p
 * Updated in $/Meccano/Stage 1/Code
 * fixed the soleboard size error
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/12/99    Time: 10:32a
 * Updated in $/Meccano/Stage 1/Code
 * 3d view working for the most part
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/10/99    Time: 12:19p
 * Updated in $/Meccano/Stage 1/Code
 * Added the Entity class
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/09/99    Time: 10:46a
 * Updated in $/Meccano/Stage 1/Code
 * 3D view now displays something
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 12  *****************
 * User: Dar          Date: 3/08/99    Time: 5:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Dar          Date: 29/07/99   Time: 5:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 28/07/99   Time: 12:40p
 * Updated in $/Meccano/Stage 1/Code
 * Converted _T("Get()") functions to _T("Get() const") functions
 * 
 * *****************  Version 9  *****************
 * User: Dar          Date: 27/07/99   Time: 11:28a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 8  *****************
 * User: Dar          Date: 27/07/99   Time: 9:43a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Dar          Date: 21/07/99   Time: 5:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Dar          Date: 21/07/99   Time: 8:51a
 * Updated in $/Meccano/Stage 1/Code
 * 
 *
 *******************************************************************************/
