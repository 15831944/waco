// Lift.h: interface for the Lift class.
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

#if !defined(AFX_LIFT_H__6F68EDE3_3A5A_11D3_8A53_00508B043A6C__INCLUDED_)
#define AFX_LIFT_H__6F68EDE3_3A5A_11D3_8A53_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
///////////////////////////// INCLUDES ///////////////////////////////
#include "MeccanoDefinitions.h"
#include "ComponentList.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								LIFT CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 13/10/00 11:37a $
//	Version			: $Revision: 66 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The lift class really is just a smart container for individual components.  
// The lift class is responsible for controlling the actions of its constituents.
///////////////////////////////////////////////////////////////////////////////

//forward declarations
class Bay;
class Component;

#include "BayDefinitions.h"	// Added by ClassView
class Lift
{
public:
	SystemEnum GetSystem( );
	double GetStarSeparation() const;
	void SetAllComponentsVisible();
	HandrailStyleEnum GetHandrailStyle(SideOfBayEnum eSide);
	double ChangeLengthAccordingToMillsCorner( ComponentTypeEnum eType, SideOfBayEnum eSide, double dLength );
	bool HasMatchingTransomLedgerHandrailOrMidrail( ComponentTypeEnum eType, int iRise, SideOfBayEnum eSide );
	bool HasMatchingHopupBracket( ComponentTypeEnum eComponentType, int iRise, SideOfBayEnum eSideOfBay );
	void CheckForHopupDoubleups( SideOfBayEnum eSide );
	void UnconvertFromRoofProtection();
	void ConvertToRoofProtection();
	void ResetComponentIDs();
	bool DeleteAllComponentsOfTypeOnSideExceptRise( ComponentTypeEnum eType, SideOfBayEnum eSide, LiftRiseEnum eRise );
	bool AddCornerStageBoard( CornerOfBayEnum eCnr, int iWidthLeft, int iWidthRight, MaterialTypeEnum eMaterial );
	CommittedProportionEnum GetCommitProportionForComponentOnSide(ComponentTypeEnum eType, SideOfBayEnum eSide);
	CommittedProportionEnum GetCommittedProportion();
	double GetRL();
	bool DeleteAllComponentsOfType( ComponentTypeEnum eType );
	int GetLengthOfHopupBracket( SideOfBayEnum eSide ) const;
	bool IsDeckingLift();
	int HowManyDeckLiftsAboveMe();
	int HowManyEmptyLiftsAboveMe(double &dEmptyLiftHeight);
	////////////////////
	////////////////////
	////Functionality///
	////////////////////
	////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	//construction/destruction
	/////////////////////////////////////////////////////////////////////////////////
	Lift();
	virtual ~Lift();


	/////////////////////////////////////////////////////////////////////////////////
	// Lift Level functions 
	/////////////////////////////////////////////////////////////////////////////////
	//ID
	void SetLiftID( int iLiftID );
	int	 GetLiftID() const;

	//default values
	bool CreateLift();
	void RemoveLift();


	/////////////////////////////////////////////////////////////////////////////////
	//Bay Pointer stuff
	/////////////////////////////////////////////////////////////////////////////////
	Bay	*GetBayPointer() const;
	void SetBayPointer( Bay *pBay );

	
	/////////////////////////////////////////////////////////////////////////////////
	//Lift Dimensions
	/////////////////////////////////////////////////////////////////////////////////
	//Rise
	void SetRise( LiftRiseEnum eRise );
	SideOfBayEnum GetPosition(int iId) const;
	int GetComponentRise( int iId ) const;
	LiftRiseEnum GetRise( );
	//Dimensions
	void SetLiftLength( double dLength, bool bMovePermitted=true );
	void SetLiftHeight( double dNewHeight );
	void SetLiftWidth( double dNewWidth );
	//Apply any changes made to the bay's dimensions
	void ApplyBayResize( Matrix3D TransformX, Matrix3D TransformY );


	/////////////////////////////////////////////////////////////////////////////////
	//BOM
	/////////////////////////////////////////////////////////////////////////////////
	bool WriteBOMTo( CStdioFile *pFile );


	//////////////////////////////////////////////////////////////////
	//Has the been marked for deletion
	//////////////////////////////////////////////////////////////////
	DirtyFlagEnum	GetDirtyFlag() const;
	//set the object to be deleted
	void	SetDirtyFlag( DirtyFlagEnum dfDirtyFlag, bool bSetDownward=false );
	//delete all objects, that this object owns, if they are marked
	//	for deletion
	void	CleanUp();


	/////////////////////////////////////////////////////////////////////////////////
	//Component functions 
	/////////////////////////////////////////////////////////////////////////////////
	//number of compoents owned by this lift
	int GetNumberOfComponents() const;
	//adds a component to the componentn list and sets its id
	Component			   *AddComponent( ComponentTypeEnum eComponentType, double dLength, SideOfBayEnum eSideOfBay, int iRise, MaterialTypeEnum eMaterialType = MT_STEEL);
	//destroys the lift and all its components
	bool					DeleteComponent( int iComponentID);
	//marks the component for deletion
//	bool					RemoveComponent( Component *pComponent, int iCount = 1 );
	void					DeleteAll();
	//retrieves the component type given the id
	ComponentTypeEnum	GetComponentType( int iComponentID) const;
	//find the component
	Component			   *GetComponent( ComponentTypeEnum eComponentType, int iRise, SideOfBayEnum eSideOfBay ) const;
	//find the component
	Component			   *GetComponent( int iComponentID ) const;
	//position a component within the lift
	void SetPositionOfComponent( Component *pComponent, ComponentTypeEnum eComponentType, double dLength, SideOfBayEnum eSideOfBay, int iRise );
	//
	Component * GetComponentOfTypeOnSide( ComponentTypeEnum eType, SideOfBayEnum eSide );
	//
	bool HasComponentOfTypeOnSide( ComponentTypeEnum eType, SideOfBayEnum eSide );
	//
	bool IsThereACornerStageBoard( SideOfBayEnum eSide );
	//
	void DeleteAllComponentsFromLift();
	//
	bool DeleteAllComponentsFromSide( SideOfBayEnum eSide );
	//
	bool DeleteAllComponentsOfTypeFromSide( ComponentTypeEnum eType, SideOfBayEnum eSide, int iRise=LIFT_RISE_INVALID );



	/////////////////////////////////////////////////////////////////////////////////
	//Stageboards, brackets and corner boards
	/////////////////////////////////////////////////////////////////////////////////
	//Adds a corner stage board, using the size of the brackets as a guide,
	//	if no bracket then it uses a size of one plank wide
	bool AddCornerStageBoard( CornerOfBayEnum eCnr, MaterialTypeEnum eMaterial );
	//Adds brackets and planks for a stage
	bool AddStage( SideOfBayEnum eSide, int iNumberOfPlanks, MaterialTypeEnum eMaterial, bool bAdjustStandards=true );
	//return the number of planks on the side of the bay
	int GetNumberOfPlanksOnStageBoard(SideOfBayEnum eSide) const;


	/////////////////////////////////////////////////////////////////////////////////
	// lift 3D manipulation functions 
	/////////////////////////////////////////////////////////////////////////////////
	//copies the lift
	bool Copy( Matrix3D TransformMatrix, bool bMove = true);
	Lift &operator =(const Lift &Original);
	//moves the lift, this movedment can be applied to m_Transform if required
	bool Move( Matrix3D TransformMatrix, bool bStoreMovement=true );
	//move the lift back to its original location and returns the previous m_Transform
	Matrix3D UnMove();
	//returns the lift's m_Transform
	Matrix3D GetTransform() const;
	//Sets the lift's m_Transform
	void SetTransform( Matrix3D Transform );


	/////////////////////////////////////////////////////////////////////////////////
	//Visibility
	/////////////////////////////////////////////////////////////////////////////////
	VisibilityEnum GetVisible() const;
	void SetVisible( VisibilityEnum eVisible=VIS_VISIBLE );

	/////////////////////////////////////////////////////////////////////////////////
	//Handrails
	/////////////////////////////////////////////////////////////////////////////////
	bool CopyHandrailToSide(SideOfBayEnum eSideToCopyFrom, SideOfBayEnum eDestinationSide);
	void AddFullMeshToSide( SideOfBayEnum eSide );
	void AddMeshToSide( SideOfBayEnum eSide, bool bFull=false );
	void AddHandrailToSide( SideOfBayEnum, bool bAddMidRailAndToeBoard=true );
	void DeleteAllHandrailComponentsFromSide( SideOfBayEnum eSide );
	void Add1000mmHopupIfReqd( SideOfBayEnum eHopupSideToTest );
	bool DeleteUnneeded1000mmHopup(Lift *pNeighbor, SideOfBayEnum eCompSide, SideOfBayEnum eStageSide);


	/////////////////////////////////////////////////////////////////////////////////
	//Stairs & Ladders
	/////////////////////////////////////////////////////////////////////////////////
	void AddSpecialStairsLaddersComponentsToLift();
	void CreateSpecialLadderLiftShortPlank();
	void CreateSpecialLadderLiftNormal( double dLadderLength );
	void CreateSpecialLadderLiftLowLift();
	void CreateSpecialLadderLift2Board( double dLadderLength );
	void CreateSpecialLadderLiftEmpty();

	/////////////////////////////////////////////////////////////////////////////////
	//Other functions
	/////////////////////////////////////////////////////////////////////////////////
	//passthrough for the creation of the 3d view
	void Create3DView();
	void Delete3DView();
	bool IsEmptyLift( );
	void TraceContents( bool bTraceComponents ) const;
	void SetLiftType( LiftTypeEnum LiftType );
	LiftTypeEnum GetLiftType();

	///////////////////////////////////////////////////////////////////////////////
	//Serialize storage/retrieval function
	void Serialize(CArchive &ar);

protected:
	////////////////////
	////////////////////
	/////Attributes/////
	////////////////////
	////////////////////
	Bay				   *m_pBay;				//pointer to the bay
	int					m_iLiftID;			//The ID for this lift = position if the bay's liftlist
	Matrix3D			m_Transform;		//this transformation is applied to all elements in this lift
	LiftTypeEnum		m_LiftType;
	LiftRiseEnum		m_eRise;			//Rise(height) of this lift
	ComponentList		ComponentListArray;	//The list of components
	DirtyFlagEnum		m_dfDirtyFlag;		//does this object need to be deleted

private:
	bool GetTransAndVector(SideOfBayEnum eSide, bool &bMoveX, bool &bMoveY,
							Matrix3D &Trans, Vector3D &Vector, int iRise,
							Point3D Standards[4], double dRiseMultiplier=1.00 );

};

#include "afxtempl.h"
typedef CArray <Lift, Lift> LiftArray;

#endif // !defined(AFX_LIFT_H__6F68EDE3_3A5A_11D3_8A53_00508B043A6C__INCLUDED_)

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Lift.h $
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 13/10/00   Time: 11:37a
 * Updated in $/Meccano/Stage 1/Code
 * About to build R 1.5.1.7 (Release 1.1g) (20001013) (Internal Release)
 * this should address some of Wayne's bugs, and add some new features to
 * help his cause
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 27/06/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * just fixed the mills problems
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 9/06/00    Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * taking code home
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 10/04/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 29/03/00   Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 10/03/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 21/01/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 14/01/00   Time: 2:37p
 * Updated in $/Meccano/Stage 1/Code
 * Fixing the Dialog boxes at the moment
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 12/01/00   Time: 12:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11 (Beta12)
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 13/12/99   Time: 1:41p
 * Updated in $/Meccano/Stage 1/Code
 * Lapboards now seem to be removeing components correctly
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 10/12/99   Time: 11:00a
 * Updated in $/Meccano/Stage 1/Code
 * It appears I was right, it was redefining the enum each time
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 6/10/99    Time: 3:14p
 * Updated in $/Meccano/Stage 1/Code
 * Schematic Corner Stage boards are now correct!
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Dar          Date: 9/08/99    Time: 3:31p
 * Updated in $/Meccano/Stage 1/Code
 * adding more serialize stuff
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 9/07/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 9/01/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Delete reactor crash fixed
 * 2) Now overrunning mouse by bay width
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 8/27/99    Time: 4:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:46p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/26/99    Time: 8:06a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 8/25/99    Time: 3:55p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 8/23/99    Time: 8:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 8/18/99    Time: 4:56p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 8/18/99    Time: 12:37p
 * Updated in $/Meccano/Stage 1/Code
 * Bay resize now working correctly
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 8/16/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/12/99    Time: 7:20p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the layers, visibility of autobuild, tight fit bay working
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/10/99    Time: 5:40p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/06/99    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * Taking code home
 * 
 * *****************  Version 8  *****************
 * User: Dar          Date: 27/07/99   Time: 11:28a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Dar          Date: 27/07/99   Time: 9:43a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Dar          Date: 21/07/99   Time: 5:20p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Dar          Date: 21/07/99   Time: 8:51a
 * Updated in $/Meccano/Stage 1/Code
 * 
 *
 *******************************************************************************/
