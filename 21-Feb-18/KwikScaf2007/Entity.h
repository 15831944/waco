// Entity.h: interface for the Entity class.
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

#include "EntityReactor.h"	// Added by ClassView
#if !defined(AFX_ENTITY_H__A93426C5_4EB4_11D3_9E95_0008C7999B1D__INCLUDED_)
#define AFX_ENTITY_H__A93426C5_4EB4_11D3_9E95_0008C7999B1D__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								Entity CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:32p $
//	Version			: $Revision: 23 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This class is directly related to the AcDbEntity class, and is really just
//	an attempt to trap notification messages for things such as delete, etc
///////////////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//forward declarations
class Component;
class AcDbGroup;

extern int	giGroupID;

class Entity  : public AcDbEntity
{
public:
	bool MarkAllComponentForPreserve( bool bPreserve );
	void DeleteAllReactors(bool bOpeningDocument/*=true*/, bool bShuttingDown/*=false*/);
	void List();
	ACRX_DECLARE_MEMBERS(Entity);

	AcDbEntity *readDwg( CString sFile );
	Acad::ErrorStatus DeleteAllDatabases( bool bOpeningDocument, bool bClosingDocument );
	Acad::ErrorStatus DeleteAllGroups( bool bOpeningDocument=true, bool bShuttingDown=false );
	Acad::ErrorStatus DeleteAllComponentNames( bool bOpeningDocument=true, bool bShuttingDown=false );
	Acad::ErrorStatus DeleteAllEntities( bool bOpeningDocument=true, bool bShuttingDown=false, bool bDeleteNonPreserved=false, bool bModelSpace=true );
	Acad::ErrorStatus ExplodeAllEntities( bool bExplodeOnlyNonPreserved=true );
	AcDbGroup *CreateGroup(AcDbObjectIdArray& objIds, TCHAR* pGroupName);

	//////////////////////////////////////////////////////////////////////////
	//construction/destruciton
	Entity();
	virtual ~Entity();

	Acad::ErrorStatus open();

	//////////////////////////////////////////////////////////////////////////
	//Component Pointer
	//////////////////////////////////////////////////////////////////////////

	Acad::ErrorStatus postToDatabase( CString sLayerName );
	void AttachedReactors( EntityReactor **pEntityReactor );

	Acad::ErrorStatus addToCurrentSpaceAndClose( AcDbDatabase* db );
	Acad::ErrorStatus addToCurrentSpaceAndClose( );
	Acad::ErrorStatus addToCurrentSpace( AcDbDatabase* db);
	AcDbBlockTableRecord *openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db);
};

#endif // !defined(AFX_ENTITY_H__A93426C5_4EB4_11D3_9E95_0008C7999B1D__INCLUDED_)

/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Entity.h $
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 13/07/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 13/01/00   Time: 8:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 12/10/99   Time: 9:32a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Level Assignment now operational - but have to call _T("RegenLevels")
 * 2) Fixed the Schematic View Serialize error.
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 15/09/99   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * checked in by request of Andrew Taylor for backup purposes
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 15/09/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * Saving now:
 * 1) Does not delete the Entities, prior to saving
 * Loading now:
 * 1) Deletes all entities that are not on one of the specified layers, so
 * we can now export all data to non-meccano people, as simple lines, etc.
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 9/09/99    Time: 12:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 8/16/99    Time: 12:18p
 * Updated in $/Meccano/Stage 1/Code
 * Component Erase, and resize is now working
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 8/12/99    Time: 9:04a
 * Updated in $/Meccano/Stage 1/Code
 *
 *******************************************************************************/
