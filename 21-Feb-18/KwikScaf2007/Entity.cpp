// Entity.cpp: implementation of the Entity class.
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
#include "Entity.h"
#include "Component.h"
#include "Controller.h"
#include <dbents.h>
#include <Acestext.h>
#include <dbgroup.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								Entity CLASS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:32p $
//	Version			: $Revision: 57 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//This class is directly related to the AcDbEntity class, and is really just
//	an attempt to trap notification messages for things such as delete, etc
///////////////////////////////////////////////////////////////////////////////

ACRX_DXF_DEFINE_MEMBERS(Entity, AcDbObject, 
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
    0, ENTITY, KWIKSCAF);

int	giGroupID;
extern Controller *gpController;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Entity
//
Entity::Entity()
{
}

//////////////////////////////////////////////////////////////////////
//~Entity
//
Entity::~Entity()
{

}

//////////////////////////////////////////////////////////////////////
Acad::ErrorStatus Entity::open()
{
	Entity				*pEnt;
	Acad::ErrorStatus	es;
	es = acdbOpenAcDbEntity( (AcDbEntity*&)pEnt, objectId(), AcDb::kForWrite );

	if( es==Acad::eOk )
	{
		if( pEnt!=this )
		{
			;//assert(false);
			return Acad::eInetUnknownError;
		}
		;//assert( isWriteEnabled() );
	}
	else if( es==Acad::eWasOpenForWrite )
	{
		//Fine
		1;
	}
	else if( es==Acad::eWasErased )
	{
		//could be a problem, let the caller handle this!
	}
	else
	{
		;//assert(false);
	}

	return es;
}

//////////////////////////////////////////////////////////////////////////////////
//postToDatabase
//Purpose:
//  Adds an entity to the MODEL_SPACE of the CURRENT database.
//Note:
//  It could be generalized to add it to any block table record of
//  any database, but for now let's keep it simple...
Acad::ErrorStatus Entity::postToDatabase( CString sLayerName )
{
	AcDbObjectId			idObj;
	Acad::ErrorStatus		es;
	AcDbBlockTable*			pBlockTable;
	AcDbBlockTableRecord*	pSpaceRecord;
	
	//Make sure that we have a working database
    if (acdbHostApplicationServices()->workingDatabase()==NULL)
	{
		;//assert( false );	//why don't we have a database?
        return Acad::eNoDatabase;
	}

	AcDb::OpenMode mode = AcDb::kForWrite;
	AcDbLayerTable* pTable;
	es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, mode);
    if( es==Acad::eOk )
	{
		if( !pTable->has( sLayerName ) )
		{
/*			//don't draw it if the layer doesn't exist
			;//assert( gpController!=NULL );
			gpController->CreateLayers();
*/
			AcDbLayerTableRecord *pRecord;
			pRecord = new AcDbLayerTableRecord();
			pRecord->setName( sLayerName );
			pTable->add( pRecord );
			pRecord->close();
		}
	}
	es = pTable->close();
	;//assert( es==Acad::eOk );

    //Get a pointer to the current drawing
    //and get the drawing's block table.  Open it for read.
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
    if(es==Acad::eOk)
	{
		//Then get the Model Space record and open it for write.  
		//This will be the owner of the new line.
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite);
        if(es==Acad::eOk)
		{
			//Append to Model Space, then close it and the Model Space record.
			es = pSpaceRecord->appendAcDbEntity(idObj, (AcDbEntity*)this);
            if(es==Acad::eOk)
			{
				es = setLayer( sLayerName );
				if( es==Acad::eKeyNotFound )
				{
					gpController->CreateLayers();
					es = setLayer( sLayerName );
				}
			}
			else if(es==Acad::eAlreadyInDb)
			{
				;//assert( false );	//why is this already here?
				idObj = objectId();
			}
            es = close();
			;//assert( es==Acad::eOk );
        }
		else
		{
			;//assert( false );	//why is the model space not available?
		}
		es = pSpaceRecord->close();
		;//assert( es==Acad::eOk );
    }
	else
	{
		;//assert( false );	//why can't we get the block table?
	}

	//Close the block table
	es = pBlockTable->close();
	;//assert( es==Acad::eOk );

	;//assert( objectId()==idObj );
	//it is good programming practice to return an error status
	return es;
}

//////////////////////////////////////////////////////////////////////
void Entity::AttachedReactors( EntityReactor **pEntityReactor )
{
	Acad::ErrorStatus es;
    AcDbDictionary *pNamedObj;
    AcDbDictionary *pNameList;
	AcDbObjectId	ObjId, ReactorId;

	try
	{
		//;//assert( (*pEntityReactor)==NULL );

		//get the NamedObjectsDictionary
		pNamedObj = NULL;
		es = acdbHostApplicationServices()->workingDatabase()->
					getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
		if( es==Acad::eOk )
		{
			//get the NameList
			pNameList = NULL;
			es = pNamedObj->getAt(REACTOR_DICTIONARY_NAME, (AcDbObject*&)pNameList, AcDb::kForWrite); 
			if( es==Acad::eKeyNotFound || es==Acad::ePermanentlyErased )
			{
				pNameList = new AcDbDictionary;
				AcDbObjectId DictId;
				pNamedObj->setAt( REACTOR_DICTIONARY_NAME, pNameList, DictId );
			}

			//create the reactor
			(*pEntityReactor) = new EntityReactor;
			es = (*pEntityReactor)->setOwnerId( objectId() );
			;//assert( es==Acad::eOk );
			
			//create the NamedList entity for the reactor
			CString sComponentID;
			sComponentID.Format( _T("%s%i"), COMP_PREFIX, ++giGroupID );

			//does the component already exist?
			ReactorId.setNull();
			es = pNameList->getAt( sComponentID, ReactorId );
			while( es==Acad::eOk )
			{
				//see bug 1430
				sComponentID.Format( _T("%s%i"), COMP_PREFIX, ++giGroupID );
				ReactorId.setNull();
				es = pNameList->getAt( sComponentID, ReactorId );
			}

			if( es==Acad::eKeyNotFound)
			{
				;//assert( ReactorId.isNull() );
				//No! so create an entity in the named list and add the reactor
				es = pNameList->setAt(sComponentID, (*pEntityReactor), ObjId );
				if( es==Acad::eOk )
				{
					ReactorId = (*pEntityReactor)->objectId();
					;//assert( !ReactorId.isNull() );
					;//assert( ReactorId==ObjId );
				}
				else
				{
					CString	sMsg;
          sMsg.Format( _T("Can't create named list: %s\n%s"),
									sComponentID, acadErrorStatusText(es) );
					delete (*pEntityReactor);
				}
				(*pEntityReactor)->close();
			}
			else
			{
				//it should be unique, we have a problem
				;//assert( false );
				acutPrintf(sComponentID + _T(" already exists in database\n"));
				delete (*pEntityReactor);
			}
			es = pNameList->close();
			;//assert( es==Acad::eOk );

			//Set up persistent reactor link
			es = open();
			;//assert( es==Acad::eOk );
			addPersistentReactor( ReactorId );

			int iLength = reactors()->length();
			//Issue 25 
			//;//assert( iLength==1 );
			
			es = close();
			;//assert( es==Acad::eOk );
		}
		else
		{
			acutPrintf(_T("Error opening the database for the reactors\n"));
			;//assert( false );
			return;
		}
		es = pNamedObj->close();
		;//assert( es==Acad::eOk );
	}
	catch(...)
	{
		if( pNameList!=NULL )
			pNameList->close();

		if( pNamedObj!=NULL )
			pNamedObj->close();

		;//assert( false );
	}
}

//////////////////////////////////////////////////////////////////////
AcDbGroup *Entity::CreateGroup(AcDbObjectIdArray& objIds, TCHAR* pGroupName)
{
	Acad::ErrorStatus es;

    AcDbGroup *pGroup = new AcDbGroup( pGroupName, true );

    // Put the group in the group dictionary that resides
    // in the named object dictionary.
    AcDbDictionary *pGroupDict;
	pGroupDict = NULL;
    es = acdbHostApplicationServices()->workingDatabase()->
						getGroupDictionary(pGroupDict, AcDb::kForWrite);
	;//assert( es==Acad::eOk && pGroupDict!=NULL );

    AcDbObjectId pGroupId;
	if( pGroupDict->has(pGroupName) )
	{
		//see bug 1430
		es = pGroupDict->close();
		;//assert( es==Acad::eOk );
		return NULL;
	}

#ifdef _DEBUG
//	int iSize = (int)pGroupDict->numEntries();
//	acutPrintf("\nAdding %s(%i)", pGroupName,iSize );
#endif	//#ifdef _DEBUG

	es = pGroupDict->setAt(pGroupName, pGroup, pGroupId);
	;//assert( es==Acad::eOk );

    es = pGroupDict->close();
	;//assert( es==Acad::eOk );

	int					i, iGroupSize, iObjLength;
	AcDbObjectId		pId;
	AcDbObjectIdArray	GroupIDs;

	iObjLength = 0;
    for( i=0; i<objIds.length(); i++ )
	{
		pId = objIds[i];
		es = pGroup->append(pId);
		;//assert( es==Acad::eOk || es==Acad::eAlreadyInGroup );
		if( es==Acad::eOk )
			iObjLength++;

#ifdef _DEBUG
		while( GroupIDs.length()>0 )
		{
			GroupIDs.removeLast();
		}
		iGroupSize = (int)pGroup->allEntityIds(GroupIDs);
		;//assert( iGroupSize==GroupIDs.length() );
		;//assert( i+1==iGroupSize && iGroupSize==iObjLength);
#endif
    }
	
	;//assert( i==objIds.length() && i==iObjLength );
	while( GroupIDs.length()>0 )
	{
		GroupIDs.removeLast();
	}
	i = (int)pGroup->allEntityIds(GroupIDs);
	iGroupSize = GroupIDs.length();
	;//assert(i==iGroupSize && iGroupSize==iObjLength);

	es = pGroup->setSelectable( true );
	;//assert( es==Acad::eOk );

	;//assert( pGroup->isSelectable() );

    es = pGroup->close();
	;//assert( es==Acad::eOk );

	return pGroup;
}



AcDbEntity *Entity::readDwg( CString sFile )
{
	int								i;
	TCHAR							strFileName[100];
    AcDbEntity						*pEnt;
    AcDbBlockTable					*pBlkTbl;
	Acad::ErrorStatus				es;
    AcDbBlockTableRecord			*pBlkTblRcd;
    AcDbBlockTableRecordIterator	*pBlkTblRcdItr;

    // Set constructor parameter to kFalse so that the
    // database will be constructed empty.  This way only
    // what is read in will be in the database.
    //
    AcDbDatabase *pDb = new AcDbDatabase(Adesk::kFalse);

    // The AcDbDatabase::readDwgFile() function
    // automatically appends a DWG extension if it is not
    // specified in the filename parameter.
    //
	for( i =0; (i<100)&&(i<sFile.GetLength()); i++ )
	{
		strFileName[i] = sFile[i];
	}
	strFileName[i] = _T('\0');
    pDb->readDwgFile(strFileName);

    //Open the model space block table record.
    es = pDb->getSymbolTable(pBlkTbl, AcDb::kForRead);
	;//assert( es==Acad::eOk );

    es = pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	;//assert( es==Acad::eOk );

    es = pBlkTbl->close();
	;//assert( es==Acad::eOk );

    es = pBlkTblRcd->newIterator(pBlkTblRcdItr);
	;//assert( es==Acad::eOk );

    for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
    {
        es = pBlkTblRcdItr->getEntity( pEnt, AcDb::kForRead);
		;//assert( es==Acad::eOk );

    acutPrintf(_T("classname: %s\n"), (pEnt->isA())->name());

        es = pEnt->close();
		;//assert( es==Acad::eOk );
    }
	delete pBlkTblRcdItr;

    es = pBlkTblRcd->close();
	;//assert( es==Acad::eOk );

    delete pBlkTblRcdItr;
    delete pDb;
	return pEnt;
}


/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Entity.cpp $
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 25/10/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 19/10/00   Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 10/10/00   Time: 10:22a
 * Updated in $/Meccano/Stage 1/Code
 * Building R1.1f (Version 1.5.1.6) which has the BOM with Stage/Level bug
 * fixed
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 12/09/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * This is pretty much the version that has gone to the UK, except:
 * 1. improved matrix progress bar
 * 2. Colour by stage level working correctly
 * 3. Matrix double length comparisons now include rounding error
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 24/08/00   Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * This should be the final code b4 version 1.5.0 is released to the
 * populus
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 21/08/00   Time: 3:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 18/08/00   Time: 1:00p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed terries _topen bug! - There was a drawing with some runs in it that
 * he had copied, the copy was causing a crash, and the drawing data file
 * was not able to be loaded again!
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 4/08/00    Time: 1:24p
 * Updated in $/Meccano/Stage 1/Code
 * About to Build RC8v
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 13/07/00   Time: 4:39p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 12/07/00   Time: 5:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 28/06/00   Time: 4:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 20/06/00   Time: 12:28p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way moving decks and roof protect lapboards look
 * for  lapboards 
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 16/06/00   Time: 3:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 31/05/00   Time: 11:41a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 26/05/00   Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 23/05/00   Time: 3:44p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC6
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 2/05/00    Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 11/04/00   Time: 3:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 13/01/00   Time: 8:04a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 12/01/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 21/12/99   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 15/09/99   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * checked in by request of Andrew Taylor for backup purposes
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 15/09/99   Time: 12:16p
 * Updated in $/Meccano/Stage 1/Code
 * Saving now:
 * 1) Does not delete the Entities, prior to saving
 * Loading now:
 * 1) Deletes all entities that are not on one of the specified layers, so
 * we can now export all data to non-meccano people, as simple lines, etc.
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 9/09/99    Time: 2:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 9/09/99    Time: 12:36p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 9/06/99    Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Programmed Pascal's Triangle routine for Bracing and Ties
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 9/06/99    Time: 1:59p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed:
 * 1) Crash upon deleting the schematic reactors
 * 2) Fixed the Railings lift problem Bug#194
 * 3) Selection mechanism almost working! Bug# 137
 * 4) Bays now responsible for their own schematic representation! Bug#186
 * 5) Set forward now cleaning up Bug# 187
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/31/99    Time: 5:42p
 * Updated in $/Meccano/Stage 1/Code
 * currently adding the reactors to the schematic view
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/20/99    Time: 4:17p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
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


//////////////////////////////////////////////////////////////////////////////////
//postToDatabase
//Purpose:
//  Adds an entity to the MODEL_SPACE of the CURRENT database.
//Note:
//  It could be generalized to add it to any block table record of
//  any database, but for now let's keep it simple...
Acad::ErrorStatus Entity::DeleteAllDatabases( bool bOpeningDocument, bool bClosingDocument )
{
	Acad::ErrorStatus				es;
	
	///////////////////////////////////////////////////////////////////
	//Delete the group names
	//AfxMessageBox(_T("DeleteAllDatabases-DeleteAllGroups"));
	es = DeleteAllGroups( bOpeningDocument, bClosingDocument );
	if(es!=Acad::eOk)
		return es;

	///////////////////////////////////////////////////////////////////
	//Delete the Component names
	//AfxMessageBox(_T("DeleteAllDatabases-DeleteAllComponentNames"));
	es = DeleteAllComponentNames( bOpeningDocument, bClosingDocument );
	if(es!=Acad::eOk)
		return es;
	///////////////////////////////////////////////////////////////////
	//Delete the Entities
	//Model Space
	//AfxMessageBox(_T("DeleteAllDatabases-DeleteAllEntities"));
	es = DeleteAllEntities( bOpeningDocument, bClosingDocument, false, true );
	if(es!=Acad::eOk)
		return es;
	//Paper Space
	//AfxMessageBox(_T("DeleteAllDatabases-DeleteAllEntities"));
	es = DeleteAllEntities( bOpeningDocument, bClosingDocument, false, false );
	if(es!=Acad::eOk)
		return es;

	//AfxMessageBox(_T("DeleteAllDatabases-DeleteAllReactors"));
	DeleteAllReactors( bOpeningDocument, bClosingDocument );

	//it is good programming practice to return an error status
	return es;
}


Acad::ErrorStatus Entity::addToCurrentSpaceAndClose( AcDbDatabase* db )
{
    Acad::ErrorStatus es, esAppend;

    esAppend = addToCurrentSpace(db);
    ;//assert(esAppend==Acad::eOk);

    es = close();
    ;//assert(es==Acad::eOk);

    return esAppend;
}

Acad::ErrorStatus Entity::addToCurrentSpaceAndClose()
{
	return addToCurrentSpaceAndClose( acdbHostApplicationServices()->workingDatabase());
}

Acad::ErrorStatus Entity::addToCurrentSpace(AcDbDatabase* db)
{
	;//assert(db != NULL);

    AcDbBlockTableRecord* blkRec = openCurrentSpaceBlock(AcDb::kForWrite, db);

    if(blkRec==NULL)
	{
		;//assert( false );
        return Acad::eInvalidInput;
	}

	Acad::ErrorStatus es, esAppend;

    // append new entity to current space block
    esAppend = blkRec->appendAcDbEntity((AcDbEntity*)this);
    if (esAppend != Acad::eOk)
	{
		;//assert( false );
        acutPrintf(_T("\nERROR: could not add entity to current space (%s)"), acadErrorStatusText(esAppend));
    }

    es = blkRec->close();
	;//assert( es==Acad::eOk );
    return esAppend;
}

AcDbBlockTableRecord *Entity::openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db)
{
	;//assert(db != NULL);
    AcDbBlockTableRecord* blkRec;

    Acad::ErrorStatus es;
	es = acdbOpenObject(blkRec, db->currentSpaceId(), mode);
    if (es!=Acad::eOk)
	{
		;//assert( false );	//why couldn't we open the space block?
        return NULL;
	}

	return blkRec;
}

void Entity::List()
{
	;//assertReadEnabled();
	AcDbEntity::list();
}

Acad::ErrorStatus Entity::DeleteAllGroups(bool bOpeningDocument/*=true*/, bool bShuttingDown/*=false*/)
{
	///////////////////////////////////////////////////////////////////
	//Delete the group names
	int					i, iSize;
	CString				sGroupName, sGroupPrefixSchematic, sGroupPrefixMatrix;
	AcDbGroup			*pGroup;
	AcDbObjectId		ID;
	AcDbDictionary		*pGroupDict;
	Acad::ErrorStatus	es;
	AcDbObjectIdArray	ids;

	try
	{
		//Make sure that we have a working database
		if( acdbHostApplicationServices()->workingDatabase()==NULL )
		{
			;//assert( false );
			return Acad::eNoDatabase;
		}
		
		es = acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pGroupDict, AcDb::kForWrite);
		if( es==Acad::eOk )
		{
			AcDbDictionaryIterator* pDictIter= pGroupDict->newIterator();
			for ( ; !pDictIter->done(); pDictIter->next() )
			{
				sGroupName = pDictIter->name();

				//Delete any Matrix or Schematic groups
				sGroupPrefixSchematic	= sGroupName.Left( GROUP_PREFIX_SCHEMATIC.GetLength() );
				sGroupPrefixMatrix		= sGroupName.Left( GROUP_PREFIX_MATRIX.GetLength() );
				if( sGroupPrefixSchematic	== GROUP_PREFIX_SCHEMATIC ||
					sGroupPrefixMatrix		== GROUP_PREFIX_MATRIX || bShuttingDown )
				{
					es = pGroupDict->getAt( sGroupName, (AcDbObject*&)pGroup, AcDb::kForWrite);
					;//assert( es==Acad::eOk );
/*
					es = pGroup->remove(ids);
					;//assert( es==Acad::eOk );
*/

					iSize = pGroup->allEntityIds( ids );
					for( i=0; i<iSize; i++ )
					{
						ID = ids[i];
						if( ID.isValid() )
						{
							es = pGroup->remove(ID);
//							;//assert( es==Acad::eOk );
						}
					}


					es = pGroup->erase();
					;//assert( es==Acad::eOk );
					es = pGroup->close();

					;//assert( es==Acad::eOk );
					pGroupDict->remove( sGroupName );
					;//assert( !pGroupDict->has(sGroupName) );
				}
			}
			delete pDictIter;
		}
		es = pGroupDict->close();
		;//assert( es==Acad::eOk );
	}
    catch(const Acad::ErrorStatus es)
    {
		es;
		;//assert( false );
    }
    catch(const HRESULT hr)
    {
		hr;
		;//assert( false );
    }
	catch(...)
	{
		;//assert( false );
	}
	return es;
}

Acad::ErrorStatus  Entity::DeleteAllComponentNames( bool bOpeningDocument/*=true*/, bool bShuttingDown/*=false*/ )
{
	Entity					*pEntity;
	CString					sCompName, sCompPrefix;
	AcDbObject				*Obj;
	AcDbDictionary			*pNamedObj;
    AcDbDictionary			*pNameList;
	Acad::ErrorStatus		es;
	const AcDb::OpenMode	modeWrite = AcDb::kForWrite;

	try
	{
		pNamedObj = NULL;

		//Make sure that we have a working database
		if( acdbHostApplicationServices()->workingDatabase()==NULL )
		{
			;//assert( false );
			return Acad::eNoDatabase;
		}
		
		es = acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
		;//assert( es==Acad::eOk );
		pNameList = NULL;
		if( pNamedObj==NULL )
			return es;
		es = pNamedObj->getAt(REACTOR_DICTIONARY_NAME, (AcDbObject*&)pNameList, AcDb::kForWrite); 
		if( es!=Acad::eKeyNotFound )
		{
			;//assert( pNameList!=NULL );
			AcDbDictionaryIterator* pNameIter= pNameList->newIterator();
			for ( ; !pNameIter->done(); pNameIter->next() )
			{
				sCompName = pNameIter->name();
				sCompPrefix = sCompName.Left( COMP_PREFIX.GetLength() );
				if( sCompPrefix==COMP_PREFIX || bShuttingDown )
				{
					es = pNameList->getAt( sCompName, Obj, AcDb::kForWrite);
					if( es==Acad::eOk )
					{
						pEntity = NULL;
						;//assert( Obj->isWriteEnabled()==Adesk::kTrue );
						es = Obj->erase();
						;//assert( es==Acad::eOk );
			
						es = Obj->close();
						;//assert( es==Acad::eOk );
			
						;//assert( !pNameList->has(sCompName) );
					}
					else
					{
						es = Obj->close();
						;//assert( es==Acad::eOk );
					}
				}
			}
			delete pNameIter;
			if( pNameList!=NULL )
			{
				es = pNameList->erase();
				;//assert( es==Acad::eOk );
			}
	//		es = pNamedObj->erase();
	//		;//assert( es==Acad::eOk );
		}
		else
		{
			if( pNameList!=NULL )
			{
				Acad::ErrorStatus estate;
				estate = pNameList->close();
				;//assert( estate==Acad::eOk );
			}
			if( pNamedObj!=NULL )
			{
				Acad::ErrorStatus estate;
				estate = pNamedObj->close();
				;//assert( estate==Acad::eOk );
			}
			return es;
		}
		if( pNameList!=NULL )
		{
			es = pNameList->close();
			;//assert( es==Acad::eOk );
		}

		if( pNamedObj!=NULL )
		{
			es = pNamedObj->close();
			;//assert( es==Acad::eOk );
		}
	}
    catch(const Acad::ErrorStatus es)
    {
		es;
		;//assert( false );
    }
    catch(const HRESULT hr)
    {
		hr;
		;//assert( false );
    }
	catch(...)
	{
		;//assert( false );
	}

	return es;
}

Acad::ErrorStatus Entity::DeleteAllEntities(bool bOpeningDocument/*=true*/, bool bShuttingDown/*=false*/, bool bDeleteNonPreserved/*=false*/, bool bModelSpace/*=true*/ )
{
	int								i;//, j;
	TCHAR							sLayerName[255], appName[4][132], sLayer3DName[255];
	bool							bDelete;
	Entity							*pEntity;
	CString							sLayer3D;
//	AcDbEntity						*pEnt;
	AcDbObjectId					idObj, entityId;
    struct resbuf					*pRb;
	AcDbBlockTable					*pBlockTable;
	Acad::ErrorStatus				es;
	AcDbBlockTableRecord			*pSpaceRecord;
	const AcDb::OpenMode			modeWrite = AcDb::kForWrite;
	const AcDb::OpenMode			modeRead = AcDb::kForRead;
	AcDbBlockTableRecordIterator	*pIterator;
	int pTemp;

	sLayer3D = LAYER_NAME_3D;

	if( bDeleteNonPreserved )
	{
		//AfxMessageBox(_T("DeleteAllEntities-1"));
		for( i=0; i<XDATA_LABEL_PRESERVE.GetLength(); i++ )
		{
			appName[0][i] = XDATA_LABEL_PRESERVE[i];
		}
		appName[0][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		//AfxMessageBox(_T("DeleteAllEntities-2"));
		for( i=0; i<XDATA_LABEL_COMPONENT.GetLength(); i++ )
		{
			appName[1][i] = XDATA_LABEL_COMPONENT[i];
		}
		appName[1][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		//AfxMessageBox(_T("DeleteAllEntities-3"));
		for( i=0; i<XDATA_LABEL_BAY.GetLength(); i++ )
		{
			appName[2][i] = XDATA_LABEL_BAY[i];
		}
		appName[2][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		//AfxMessageBox(_T("DeleteAllEntities-4"));
		for( i=0; i<XDATA_LABEL_KUT_COMPONENT.GetLength(); i++ )
		{
			appName[3][i] = XDATA_LABEL_KUT_COMPONENT[i];
		}
		appName[3][i] = _T('\0');
	}

	try
	{
		//AfxMessageBox(_T("DeleteAllEntities-5"));
		//Make sure that we have a working database
		if( acdbHostApplicationServices()->workingDatabase()==NULL )
		{
			;//assert( false );
			return Acad::eNoDatabase;
		}

		//AfxMessageBox(_T("DeleteAllEntities-6"));
		//Get a pointer to the current drawing
		//and get the drawing's block table.  Open it for read.
		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, modeRead);
		if (es==Acad::eOk)
		{
			//Then get the Model Space record and open it for write.  
			//This will be the owner of the new line.
			if( bModelSpace )
				es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, modeWrite);
			else
				es = pBlockTable->getAt(ACDB_PAPER_SPACE, pSpaceRecord, modeWrite);

			if (es==Acad::eOk)
			{
				//Append to Model Space, then close it and the Model Space record.
				if( (es = pSpaceRecord->newIterator( pIterator ))==Acad::eOk )
				{
					int cnt=0;
					while( !pIterator->done() )
					{
						pEntity = NULL;
						cnt++;
						if(cnt%100==0)
						{
							//AfxMessageBox(_T("DeleteAllEntities-7-While"));
						}

						es = pIterator->getEntity( (AcDbEntity*&)pEntity, modeWrite );
   	  				if( es==Acad::eOk )
						{
							;//assert( pEntity!=NULL );
							_tcscpy( sLayerName, pEntity->layer() );
							_tcscpy( sLayer3DName, pEntity->layer() );
							//sLayer3DName should only be as long as the text of LAYER_NAME_3D
							sLayer3DName[sLayer3D.GetLength()] = _T('\0');
							
//							if( bOpeningDocument )
//							{
								bDelete = false;
								if( bDeleteNonPreserved )
								{
									pRb = pEntity->xData(appName[0]);
									if (pRb==NULL)
									{
										bDelete = true;
/*										//This element was not marked for preserve
										if( pEntity->xData(appName[1])!=NULL )
										{
											//The element has some KwikScaf info, so it was not drawn by hand
											bDelete = true;
										}
										else if( pEntity->xData(appName[2])!=NULL )
										{
											//The element has some KwikScaf info, so it was not drawn by hand
											bDelete = true;
										}
										else if( pEntity->xData(appName[3])!=NULL )
										{
											//The element has some KwikScaf info, so it was not drawn by hand
											bDelete = true;
										}
*/									}
									else
									{
										//pRes = XDPF_APPNAME - XData name,
										pRb = pRb->rbnext;	//pRes = XDPF_PRESERVE_FLAG	- Preserve Flag
										if(pRb->restype==AcDb::kDxfXdInteger16 )
										{
											bDelete = (pRb->resval.rint==0)? true: false;
										}
										else
										{
											;//assert( false );
										}
									}
								}
								else
								{
									if( _tcscmp( sLayer3DName, LAYER_NAME_3D )==0 ||
										_tcscmp( sLayerName, LAYER_NAME_SCHEMATIC )==0 ||
										_tcscmp( sLayerName, LAYER_NAME_MATRIX )==0 )
									{
										bDelete = true;
									}
								}

								if( bDelete )
								{
//									int					i;
									void	            *pSomething;
									AcDbObjectId		ReactorID;
									EntityReactor		*pReactor;
									AcDbVoidPtrArray	*pReactors;

									pReactors = NULL;
									pReactors = (AcDbVoidPtrArray*)pEntity->reactors();

									int cnt2=0;
									if ( (pReactors!=NULL) && (pReactors->logicalLength()>0) )
									{
										while( pReactors->logicalLength()>0 )
										{
											cnt2++;
											if(cnt2%10==0)
											{
												//AfxMessageBox(_T("DeleteAllEntities-8-While-reactor"));
												//AutoCAD MAP3D Hang Patch
												break;
											}
											pSomething = pReactors->at(0);
											// Is it a persistent reactor?
											if (acdbIsPersistentReactor(pSomething)==Adesk::kTrue )
											{
												ReactorID = acdbPersistentReactorObjectId( pSomething);
												if( ReactorID.isNull() || !ReactorID.isValid() )
												{
													//AfxMessageBox(_T("DeleteAllEntities-8-While-reactor-continue"));
													continue;
												}

												es = pEntity->removePersistentReactor( ReactorID );
												assert(es==Acad::eOk);

												pReactor = NULL;
												es = acdbOpenAcDbObject((AcDbObject*&)pReactor, ReactorID, AcDb::kForWrite);
												if(es==Acad::eOk)
												{
													assert( pReactor!=NULL );
													pReactor->assertWriteEnabled(false,false);
													pEntity->close();	//see bug 1430
													es = pReactor->erase();
													assert( es==Acad::eOk);
													es = pReactor->close(); 
													assert( es==Acad::eOk);
													pReactor=NULL;
													pEntity->open();	//see bug 1430
												}
												else if( es==Acad::eWasErased )
												{
													//fine
													1;
												}
												else
												{
													assert( false );
												}
											}
										}
									}
									es = pEntity->erase();
									;//assert( es==Acad::eOk );
								}
/*							}
							else
							{
								;//assert( false );
							}
*/						}
						else if( es==Acad::eOnLockedLayer )
						{
							pEntity = NULL;
						}
						else
						{
							;//assert( false );
							pEntity = NULL;
						}

						if( pEntity!=NULL )
						{
							es = pEntity->close();
							;//assert( es==Acad::eOk );
						}
							pIterator->step();
					}
						delete pIterator;
				}
				else
				{
					;//assert( false );
				}
			}
			else
			{
				;//assert( false );
			}
			es = pSpaceRecord->close();
			;//assert(es==Acad::eOk);
		}
		else
		{
			;//assert( false );
		}
		//Close the block table
		es = pBlockTable->close();
		;//assert(es==Acad::eOk);

		;//assert( objectId()==idObj );
	}
    catch(const Acad::ErrorStatus es)
    {
		es;
		assert( false );
    }
    catch(const HRESULT hr)
    {
		hr;
		assert( false );
    }
	catch(...)
	{
		assert( false );
	}

	return es;
}

void Entity::DeleteAllReactors(bool bOpeningDocument, bool bShuttingDown)
{
	AcDbObjectId					idObj;
	AcDbObjectId					entityId;
	Acad::ErrorStatus				es;
	const AcDb::OpenMode			modeWrite = AcDb::kForWrite;
	const AcDb::OpenMode			modeRead = AcDb::kForRead;

    AcDbDictionary *pNamedObj;
    AcDbDictionary *pNameList;
	AcDbObjectId	ObjId, ReactorId;

	pNameList = NULL;
	pNamedObj = NULL;
	try
	{
		es = acdbHostApplicationServices()->workingDatabase()->
					getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
		if( es==Acad::eOk )
		{
			//get the NameList
			es = pNamedObj->getAt(REACTOR_DICTIONARY_NAME, (AcDbObject*&)pNameList, AcDb::kForWrite); 
			if( es==Acad::eOk )
			{
				;//assert( pNameList!=NULL );
				AcDbDictionaryIterator* pNameIter= pNameList->newIterator();
				AcDbObject	*pObject;
				for ( ; !pNameIter->done(); pNameIter->next() )
				{
					pObject = NULL;
					es = pNameIter->getObject( pObject, modeWrite );
					if( es==Acad::eOk )
					{
						es = pObject->erase();
						;//assert( es==Acad::eOk );
						es = pObject->close();
						;//assert( es==Acad::eOk );
					}
				}
			}
		}
	}
	catch(...)
	{
		;//assert( false );
	}

	if( pNameList!=NULL )
	{
		es = pNameList->close();
		;//assert( es==Acad::eOk );
	}

	if( pNamedObj!=NULL )
	{
		es = pNamedObj->close();
		;//assert( es==Acad::eOk );
	}
}


// Changes made to this function by ~SJ~, 03.08.2007
bool Entity::MarkAllComponentForPreserve(bool bPreserve)
{
	int								i;
    TCHAR							appName[132];
	bool							bReturn;
	AcDbEntity						*pEnt;
	AcDbObjectId					idObj;
	AcDbObjectId					entityId;
    struct resbuf					*pRb, *pTemp;
	AcDbBlockTable					*pBlockTable;
	Acad::ErrorStatus				es;
	AcDbBlockTableRecord			*pSpaceRecord;
	const AcDb::OpenMode			modeWrite = AcDb::kForWrite;
	const AcDb::OpenMode			modeRead = AcDb::kForRead;
	AcDbBlockTableRecordIterator	*pIterator;

	bReturn = true;

  /*
	for( i=0; i<XDATA_LABEL_PRESERVE.GetLength(); i++ )
	{
		appName[i] = XDATA_LABEL_PRESERVE[i];
	}
	appName[i] = _T('\0');
  */

  CString sName = XDATA_LABEL_PRESERVE;

	try
	{
		//Make sure that we have a working database
		if( acdbHostApplicationServices()->workingDatabase()==NULL )
		{
			;//assert( false );
			return false;
		}

		//Get a pointer to the current drawing
		//and get the drawing's block table.  Open it for read.
		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, modeRead);
		if (es==Acad::eOk)
		{
			//Then get the Model Space record and open it for write.  
			//This will be the owner of the new line.
			es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, modeWrite);
			if (es==Acad::eOk)
			{
				//Append to Model Space, then close it and the Model Space record.
				if( (es = pSpaceRecord->newIterator( pIterator ))==Acad::eOk )
				{
					while( !pIterator->done() )
					{
						pEnt = NULL;
						es = pIterator->getEntity( pEnt, modeWrite );
						if( es==Acad::eOk || es==Acad::eWasOpenForWrite )
						{
							;//assert( pEnt!=NULL );

							//pRb = pEnt->xData(appName);
              pRb = pEnt->xData(sName);
							if (pRb==NULL)
							{
								// If xdata is not present, register the application
								// and add appName to the first resbuf in the list.
								// Notice that there is no -3 group as there is in

								// AutoLISP. This is ONLY the xdata so
								// the -3 xdata-start marker isn't needed.
								//acdbRegApp(appName);
                acdbRegApp(sName);

								////////////////////////////////////////////////////////
								//XDPF_APPNAME		- XData reference name
								//pRb = acutNewRb(AcDb::kDxfRegAppName);
                pRb = acutBuildList(AcDb::kDxfRegAppName, sName, NULL);
								pTemp = pRb;
								//pTemp->resval.rstring = (TCHAR*) malloc(_tcslen(appName) + 1);
								//_tcscpy(pTemp->resval.rstring, appName);

								////////////////////////////////////////////////////////
								//XDPF_PRESERVE_FLAG	- Preserve Flag
								pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger16);
								pTemp = pTemp->rbnext;
								pTemp->resval.rint = (int)bPreserve;
								pTemp->rbnext = NULL;
							}
							else
							{
								pTemp = pRb->rbnext;
								pTemp->resval.rint = (int)bPreserve;
								pTemp->rbnext = NULL;
							}

							es = pEnt->setXData(pRb);
							;//assert( es==Acad::eOk );

							acutRelRb(pRb);
						}
						else if( es==Acad::eOnLockedLayer )
						{
							pEnt = NULL;
						}
						else
						{
							;//assert( false );
							pEnt = NULL;
						}

						if( pEnt!=NULL )
						{
							es = pEnt->close();
							;//assert( es==Acad::eOk );
						}
						pIterator->step();
					}
					delete pIterator;
				}
				else
				{
					;//assert( false );
				}
			}
			else
			{
				;//assert( false );
			}
			es = pSpaceRecord->close();
			;//assert(es==Acad::eOk);
		}
		else
		{
			;//assert( false );
		}
		//Close the block table
		es = pBlockTable->close();
		;//assert(es==Acad::eOk);

		;//assert( objectId()==idObj );
	}
    catch(const Acad::ErrorStatus es)
    {
		bReturn = false;
		es;
		;//assert( false );
    }
    catch(const HRESULT hr)
    {
		bReturn = false;
		hr;
		;//assert( false );
    }
	catch(...)
	{
		bReturn = false;
		;//assert( false );
	}

	return bReturn;
}

Acad::ErrorStatus Entity::ExplodeAllEntities(bool bExplodeOnlyNonPreserved/*=true*/ )
{
	int								i, j;
	TCHAR							appName[4][132];
	bool							bExplode;
	Entity							*pEntity;
	AcDbEntity						*pEnt;
	AcDbObjectId					idObj, entityId;
    struct resbuf					*pRb;
	AcDbBlockTable					*pBlockTable;
	Acad::ErrorStatus				es;
	AcDbBlockTableRecord			*pSpaceRecord;
	const AcDb::OpenMode			modeWrite = AcDb::kForWrite;
	const AcDb::OpenMode			modeRead = AcDb::kForRead;
	AcDbBlockTableRecordIterator	*pIterator;

	if( bExplodeOnlyNonPreserved )
	{
		for( i=0; i<XDATA_LABEL_PRESERVE.GetLength(); i++ )
		{
			appName[0][i] = XDATA_LABEL_PRESERVE[i];
		}
		appName[0][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		for( i=0; i<XDATA_LABEL_COMPONENT.GetLength(); i++ )
		{
			appName[1][i] = XDATA_LABEL_COMPONENT[i];
		}
		appName[1][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		for( i=0; i<XDATA_LABEL_BAY.GetLength(); i++ )
		{
			appName[2][i] = XDATA_LABEL_BAY[i];
		}
		appName[2][i] = _T('\0');

		//////////////////////////////////////////////////////////////////
		for( i=0; i<XDATA_LABEL_KUT_COMPONENT.GetLength(); i++ )
		{
			appName[3][i] = XDATA_LABEL_KUT_COMPONENT[i];
		}
		appName[3][i] = _T('\0');
	}

	try
	{
		//Make sure that we have a working database
		if( acdbHostApplicationServices()->workingDatabase()==NULL )
		{
			;//assert( false );
			return Acad::eNoDatabase;
		}

		//Get a pointer to the current drawing
		//and get the drawing's block table.  Open it for read.
		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, modeRead);
		if (es==Acad::eOk)
		{
			//Then get the Model Space record and open it for write.  
			//This will be the owner of the new line.
			es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, modeWrite);
			if (es==Acad::eOk)
			{
				if( (es = pSpaceRecord->newIterator( pIterator ))==Acad::eOk )
				{
					while( !pIterator->done() )
					{
						pEntity = NULL;
						es = pIterator->getEntity( (AcDbEntity*&)pEntity, modeWrite );
						if( es==Acad::eOk )
						{
							;//assert( pEntity!=NULL );
							
							//Explode everything!
							bExplode = true;

							//if we are only exploding non-preserved elements
							if( bExplodeOnlyNonPreserved )
							{
								//Get the preserve flag
								pRb = pEntity->xData(appName[0]);
								if (pRb!=NULL)
								{
									//pRes = XDPF_APPNAME - XData name,
									pRb = pRb->rbnext;	//pRes = XDPF_PRESERVE_FLAG	- Preserve Flag
									if(pRb->restype==AcDb::kDxfXdInteger16 )
									{
										//if the flag is 0 then it in not preserved, so explode it.
										//conversly if the flag is <>0 then it is preserved, so don't explode
										bExplode = (pRb->resval.rint==0)? true: false;
									}
									else
									{
										//what happened to it's preserve flag?
										;//assert( false );
									}
								}
								else
								{
									//This does not have a preserve flag, so we should explode it
									bExplode = true;
								}
							}

							if( bExplode )
							{
								if( pEntity->xData(appName[1])!=NULL ||
									pEntity->xData(appName[2])!=NULL ||
									pEntity->xData(appName[3])!=NULL )
								{
									//This block contains kwikscaf info, must be a single visual component
									es = pEntity->erase();
									;//assert(es==Acad::eOk);
									es = pEntity->close();
									;//assert(es==Acad::eOk);
								}
								else
								{
									AcDbVoidPtrArray entitySet;
									es = pEntity->explode( entitySet );
									if(es==Acad::eOk)
									{
										//this is an explodable entity.
										for( j=0; j<entitySet.length(); j++ )
										{
											pEnt = (AcDbEntity*)entitySet[j];
											es = pSpaceRecord->appendAcDbEntity(idObj, pEnt);
											if(es==Acad::eOk)
											{
												es = pEnt->setLayer( _T("0") );
											}
											if( !((pEnt->objectId()).isNull()) )
											{
												es = acdbOpenAcDbObject( (AcDbObject*&)pEnt, pEnt->objectId(), AcDb::kForWrite);
												if(es==Acad::eOk || es==Acad::eWasOpenForWrite )
												{
													;//assert( pEnt!=NULL );
													if( pEnt->xData(appName[1])!=NULL ||
														pEnt->xData(appName[2])!=NULL ||
														pEnt->xData(appName[3])!=NULL )
													{
														es = pEnt->erase();
														;//assert(es==Acad::eOk);
														j--;
													}
													es = pEnt->close();
													;//assert(es==Acad::eOk);
												}
											}
										}
										es = pEntity->erase();
										;//assert(es==Acad::eOk);
										es = pEntity->close();
										;//assert(es==Acad::eOk);
									}
									else
									{
										;//assert( es==Acad::eNotApplicable);
									}
								}
							}
						}
						else
						{
							;//assert( false );
							pEntity = NULL;
						}

						if( pEntity!=NULL )
						{
							es = pEntity->close();
							;//assert( es==Acad::eOk );
						}
						
					}
					delete pIterator;
				}
				else
				{
					;//assert( false );
				}
			}
			else
			{
				;//assert( false );
			}
			es = pSpaceRecord->close();
			;//assert(es==Acad::eOk);
		}
		else
		{
			;//assert( false );
		}
		//Close the block table
		es = pBlockTable->close();
		;//assert(es==Acad::eOk);

		;//assert( objectId()==idObj );
	}
    catch(const Acad::ErrorStatus es)
    {
		es;
		;//assert( false );
    }
    catch(const HRESULT hr)
    {
		hr;
		;//assert( false );
    }
	catch(...)
	{
		;//assert( false );
	}

	return es;
}
