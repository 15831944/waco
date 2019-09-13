//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.

#include "Stdafx.h"
#include "MdiAware.h"
#include "Controller.h"
#include "ScaffoldArchive.h"
#include <Acestext.h>
#include <gePlane.h>

enum ConsiderationEnum
{
	CONSIDER_RUN,
	CONSIDER_BAY,
	CONSIDER_INVALID
};


// ************************************************************
// printObj
//
// Prints out the basic information about the object pointed
// to by pObj.
void printObj(const AcDbObject* pObj)
{
    if (pObj == NULL) 
    {
        acutPrintf(_T("(NULL)"));
        return;
    }

    AcDbHandle objHand;
    TCHAR handbuf[17];

    // Get the handle as a string.
    //
    pObj->getAcDbHandle(objHand);
    objHand.getIntoAsciiBuffer(handbuf);

    acutPrintf( _T("\n   (class==%s, handle==%s, id==%lx, db==%lx)"),
        pObj->isA()->name(), handbuf,
        pObj->objectId().asOldId(), pObj->database());
}


// ************************************************************
// printDbEvent
//
// Prints the message passed in by pEvent; then
// proceeds to call printObj() to print the information about
// the object that triggered the notification.

void printDbEvent(const AcDbObject* pObj, const TCHAR* pEvent)
{
  acutPrintf(_T("  Event: AcDbDatabaseReactor::%s "), pEvent);
    printObj(pObj);
}


// ****************************************************************
// AsdkMeccanoEditorReactor - Member Functions ********************
// ****************************************************************

AsdkMeccanoEditorReactor::AsdkMeccanoEditorReactor()
{
	assert( gpController!=NULL );
	gpController->SetBlockInserting( false );
	m_iAlignCounter = 0;
}

AsdkMeccanoEditorReactor::~AsdkMeccanoEditorReactor()
{

}


// ************************************************************
// beginSave
// 
// This function is called when the editor is just about to save the drawing.
// Call save custom objects here.
// *******************
void AsdkMeccanoEditorReactor::beginSave( AcDbDatabase *pDwg, const TCHAR *pIntendedName )
{
	pDwg;
	pIntendedName;
	if (gpScaffoldArchive)
		gpScaffoldArchive->SaveArchive();
}


// ************************************************************
// databaseConstructed
// 
// This function is called when the editor is has just finished constructing the DB.
// I think! 
// Call load custom objects here.
// *******************
void AsdkMeccanoEditorReactor::databaseConstructed( AcDbDatabase *pDwg ) 
{ 
	pDwg;
//	if (gpScaffoldArchive)
//		gpScaffoldArchive->LoadArchive();
} 


void AsdkMeccanoEditorReactor::initialDwgFileOpenComplete( AcDbDatabase* pDwg )
{
	pDwg;
//	if (gpScaffoldArchive)
//		gpScaffoldArchive->LoadArchive();
}


// ************************************************************
// databaseToBeDestroyed
// *******************
void AsdkMeccanoEditorReactor::databaseToBeDestroyed(AcDbDatabase* pDwg)
{
	pDwg;
}


// ************************************************************
// AsdkDbReactor - Member Functions
// ************************************************************

ACRX_DXF_DEFINE_MEMBERS(AsdkDbReactor, AcDbDatabaseReactor, 
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
    0, ASDKDBREACTOR, KWIKSCAF);

// ************************************************************
// objectAppended
// ***************
// This is called whenever an object is added to the database.
//
void AsdkDbReactor::objectAppended(const AcDbDatabase* db,
    const AcDbObject* pObj)
{
    printDbEvent(pObj, _T("objectAppended"));
    acutPrintf(_T(" Db==%lx\n"), (long) db);
    gpAsdkAppDocGlobals->incrementEntityCount(); 
    acutPrintf(_T("Entity Count = %d\n"),  
        gpAsdkAppDocGlobals->entityCount()); 
}

// ************************************************************
// objectModified
// **************
// This is called whenever an object in the database is modified.
void AsdkDbReactor::objectModified(const AcDbDatabase* db,
    const AcDbObject* pObj)
{
    printDbEvent(pObj, _T("objectModified"));
    acutPrintf(_T(" Db==%lx\n"), (long) db);
}

// ************************************************************
// objectErased
// *************
// This is called whenever an object is erased from the database.
void AsdkDbReactor::objectErased(const AcDbDatabase* db,
    const AcDbObject* pObj, Adesk::Boolean pErased)
{
    if (pErased) 
    {
        printDbEvent(pObj, _T("objectErased"));
        gpAsdkAppDocGlobals->decrementEntityCount(); 
	}
    else 
    {
        printDbEvent(pObj, _T("object(Un)erased"));
        gpAsdkAppDocGlobals->incrementEntityCount(); 
    }
    acutPrintf(_T(" Db==%lx\n"), (long) db);

    acutPrintf(_T("Entity Count = %d\n"),  
        gpAsdkAppDocGlobals->entityCount()); 
}



// ************************************************************
// AsdkDocReactor - Member Functions
// ************************************************************

// ************************************************************
// documentToBeActivated
// *************
// Document swapping functions 
void AsdkDocReactor::documentToBeActivated(AcApDocument *pDoc) 
{ 
	Controller *pLastController;
	assert( gpController!=NULL );
	pLastController = gpController;

	if ( gbOpenFileDialogActive == false ) // see bug 301 - to prevent a new controller being created
	{
	  if (gpAsdkAppDocGlobals->setGlobals(pDoc)) // if executed
		{
			if (pLastController != gpController) // if controllers have changed
			{
				if (pLastController) // tell old controller to hide any dialogs
					pLastController->ShowDialogs(false);
				if (gpController) // tell new controller to show any dialogs
					gpController->ShowDialogs(true);
			}
		}
	}
} 

// ************************************************************
// documentCreated
// *************
void AsdkDocReactor::documentCreated(AcApDocument *pDoc) 
{ 
    if (gpAsdkAppDocGlobals->setGlobals(pDoc)) // if executed 
	{
		if (gpScaffoldArchive)
			gpScaffoldArchive->LoadArchive();
	}
} 

// ************************************************************
// documentToBeDestroyed
// *************
void AsdkDocReactor::documentToBeDestroyed(AcApDocument *pDoc) 
{ 
    gpAsdkAppDocGlobals->removeDocGlobals(pDoc); 
} 

/////////////////////////////////////////////////////////////////////////////////
// documentCreateStarted
//
// this is no good as the creation of the controller requires the DB to be fully
// loaded which it is not at this stage
//
void AsdkDocReactor::documentCreateStarted(AcApDocument* pDoc)
{
	pDoc;
//	gpAsdkAppDocGlobals->setGlobals(pDoc); 
}


// ************************************************************
// AsdkPerDocData - Member Functions
// ************************************************************

// ************************************************************
// Constructor
// *************
AsdkPerDocData::AsdkPerDocData(AcApDocument *pDoc) 
{ 
    m_pDoc = pDoc; 
    m_pNext = NULL; 
    m_EntAcc = 0; 
    m_pDbr = NULL; 
	m_pController = new Controller();
	m_pScaffoldArchive = new ScaffoldArchive( m_pController );
} 


// ************************************************************
// Destructor
// *************
AsdkPerDocData::~AsdkPerDocData()
{
	DELETE_PTR(m_pScaffoldArchive);
	DELETE_PTR(m_pController);
}


// ************************************************************
// GetController
// *************
Controller *AsdkPerDocData::GetController()
{
	return m_pController;
}

// ************************************************************
// GetScaffoldArchive
// *************
ScaffoldArchive *AsdkPerDocData::GetScaffoldArchive()
{
	return m_pScaffoldArchive;
}


// ************************************************************
// AsdkAppDocGlobals - Member Functions
// ************************************************************

// ************************************************************
// Constructor
// *************
AsdkAppDocGlobals::AsdkAppDocGlobals(AcApDocument *pDoc) 
{ 
	pDoc;

	bGetGlobalsLoopProtection = false;

    m_pData = m_pHead = NULL; 
    m_pDocReactor = new AsdkDocReactor(); 
    acDocManager->addReactor(m_pDocReactor); 
} 


// ************************************************************
// setGlobals
// *************
// Iterate through the list until the documents's global data is 
// found. If it is not found, create a new set of document globals. 
// 
bool AsdkAppDocGlobals::setGlobals(AcApDocument *pDoc) 
{ 
  bool bExecuted = false;

	if(!bGetGlobalsLoopProtection)
	{
		bExecuted = true;
		bGetGlobalsLoopProtection = true;

		AsdkPerDocData *p_data = m_pHead, *prev_data = m_pHead; 
		while (p_data != NULL)  
		{ 
			if (p_data->m_pDoc == pDoc)  
			{ 
				m_pData = p_data; 

				break; 
			} 
			prev_data = p_data; 
			p_data = p_data->m_pNext; 
		} 

		if (p_data == NULL)  
		{ 
			if (m_pHead == NULL) 
      {
				m_pHead = m_pData = new AsdkPerDocData(pDoc); 
      }
			else 
      {
				prev_data->m_pNext = m_pData = new AsdkPerDocData(pDoc); 
      }
		} 

		// set Controller Object
		gpController		= m_pData->GetController();
		gpScaffoldArchive	= m_pData->GetScaffoldArchive();
	}
	bGetGlobalsLoopProtection = false;

	return bExecuted;
} 

// ************************************************************
// removeDocGlobals
// *************
// Delete the globals associated with pDoc. 
void AsdkAppDocGlobals::removeDocGlobals(AcApDocument *pDoc) 
{ 
    AsdkPerDocData  *p_data = m_pHead, *prev_data = m_pHead; 
    while (p_data != NULL)  
    { 
        if (p_data->m_pDoc == pDoc)  
        { 
            if (p_data == m_pHead) 
                m_pHead = p_data->m_pNext; 
            else 
                prev_data->m_pNext = p_data->m_pNext; 

            if (m_pData == p_data) 
                m_pData = m_pHead; 
            DELETE_PTR(p_data); 
			if( m_pData!=NULL )
			{
				gpController		= m_pData->GetController();
				gpScaffoldArchive	= m_pData->GetScaffoldArchive();
			}
			else
			{
				gpController		= NULL;		
				gpScaffoldArchive	= NULL;
			}
            break; 
        } 
        prev_data = p_data; 
        p_data = p_data->m_pNext; 
    } 
} 

// ************************************************************
// removeAllDocGlobals
// *************
// Delete all the doc globals in the list (recursively). 
void AsdkAppDocGlobals::removeAllDocGlobals(AsdkPerDocData *p_target) 
{ 
    if (p_target == NULL) 
        return; 
    if (p_target->m_pNext != NULL) 
        removeAllDocGlobals(p_target->m_pNext); 
    if (p_target->m_pDbr != NULL) 
    { 
		acdbHostApplicationServices()->workingDatabase()->removeReactor(p_target->m_pDbr); 
	    delete p_target->m_pDbr; 
		p_target->m_pDbr = NULL; 
    } 
    delete p_target; 
} 

// ************************************************************
// unload
// *************
// Application was unloaded - delete everything associated with this 
// document. 
// 
void AsdkAppDocGlobals::unload() 
{ 
    acDocManager->removeReactor(m_pDocReactor); 
    delete m_pDocReactor; 
    removeAllDocGlobals(m_pHead); 
    m_pHead = m_pData = NULL; 
} 

// ************************************************************
// entityCount
// *************
long &AsdkAppDocGlobals::entityCount() 
{ 
    return m_pData->m_EntAcc; 
} 

// ************************************************************
// incrementEntityCount
// *************
void AsdkAppDocGlobals::incrementEntityCount() 
{ 
    m_pData->m_EntAcc++; 
} 

// ************************************************************
// decrementEntityCount
// *************
void AsdkAppDocGlobals::decrementEntityCount() 
{ 
    m_pData->m_EntAcc--; 
} 

// ************************************************************
// dbReactor
// *************
AsdkDbReactor *AsdkAppDocGlobals::dbReactor() 
{ 
    return m_pData->m_pDbr; 
} 

// ************************************************************
// setDbReactor
// *************
void AsdkAppDocGlobals::setDbReactor(AsdkDbReactor *pDb) 
{ 
    m_pData->m_pDbr = pDb; 
} 


enum CommandTypeEnum
{
	COMMAND_ERASE,
	COMMAND_DROP,
	COMMAND_COPY,
	COMMAND_ROTATE,
	COMMAND_ALIGN,
	COMMAND_INSERT,
	COMMAND_UNIMPORTANT,
	COMMAND_INVALID
};

void AsdkMeccanoEditorReactor::commandWillStart(const TCHAR *cmdStr)
{
	#ifdef _DEBUG
  acutPrintf( _T("\nCommandStarted: %s"), cmdStr );
	#endif	//#ifdef _DEBUG

	if( _tcscmp( cmdStr, _T("BLOCK") ) == 0 )
	{
		CString sMsg;
    sMsg = _T("WARNING: The Block command is not supported by KwikScaf!\n\n");
		sMsg+= _T("If you select KwikScaf components whilst using this feature\n");
		sMsg+= _T("the components will APPEAR correctly on the screen as a\n");
		sMsg+= _T("single block, however the original KwikScaf components will\n");
		sMsg+= _T("infact be DELETED!!\n\n");
		sMsg+= _T("KwikScaf is however compatible with AutoCAD's 'Group' command\n");
		sMsg+= _T("which has similar functionality.\n\n");
		sMsg+= _T("YOU HAVE BEEN WARNED!");
		MessageBox( NULL, sMsg, _T("AutoCAD's Block command WARNING"), MB_OK );
	}
}

#include "geAssign.h"

void AsdkMeccanoEditorReactor::commandEnded(const TCHAR *cmdStr)
{
	int					i, iSize;
	Bay					*pBay;
	bool				bMessageShown;
	void				*pSomething;
	Entity				Ent;
	CString				sMsg;
	Vector3D			Vector;
	Matrix3D			Move;
	Component			*pComponent/*, *pComp*/;
	AcDbEntity			*pEnt/*, *pMove*/;
	AcDbObjectId		reactorId;
	AcDbObjectId		id;
//	EntityReactor		*pReactor;
	AcDbVoidPtrArray	*reactors;
	Acad::ErrorStatus	es;
	AcDbObjectIdArray	*pCopiedObjects;

	pEnt = NULL;

	#ifdef _DEBUG
  acutPrintf( _T("\nCommandEnded: %s"), cmdStr );
	#endif	//#ifdef _DEBUG
	int iCommand;

	GetController()->SetHasBeenVisited(false);
	

	iCommand = COMMAND_UNIMPORTANT;
	m_bMove = false;
	if(		 _tcscmp( cmdStr, _T("ERASE")		) == 0 )	iCommand = COMMAND_ERASE;
	else if( _tcscmp( cmdStr, _T("DROPGEOM")		) == 0 )	iCommand = COMMAND_DROP;
	else if( _tcscmp( cmdStr, _T("COPY")			) == 0 )	iCommand = COMMAND_COPY;
	else if( _tcscmp( cmdStr, _T("ARRAY")		) == 0 )	iCommand = COMMAND_COPY;
	else if( _tcscmp( cmdStr, _T("PASTECLIP")	) == 0 )	iCommand = COMMAND_COPY;
	else if( _tcscmp( cmdStr, _T("ROTATE")		) == 0 )	iCommand = COMMAND_ROTATE;
	else if( _tcscmp( cmdStr, _T("INSERT")		) == 0 )	iCommand = COMMAND_INSERT;
	else if( _tcscmp( cmdStr, _T("MOVE")			) == 0 )
	{
		iCommand = COMMAND_DROP;
		m_bMove = true;
	}

	int			track, type;
#ifdef _DEBUG
	int			iCount, j;
#endif	//_DEBUG
	resbuf		result;
	Point3D		pt;
	Matrix3D	Transform;
	Run			*pRun;
	LapboardBay	*pLap;
	

	switch( iCommand )
	{
	////////////////////////////////////////////////////////////////
	case( COMMAND_INSERT ):

		track = 0x0001;
		acedGrRead( track, &type, &result );
		if( type==5 )
		{
			pt = asPnt3d(result.resval.rpoint);
#ifdef _DEBUG
      acutPrintf( _T("\nPosition: %1.0f, %1.0f, %1.0f"), pt.x, pt.y, pt.z );
#endif //_DEBUG

			Vector.set( pt.x, pt.y, pt.z );
			Transform.setToTranslation( Vector );

			for( i=GetController()->GetRunsB4Insert(); i<GetController()->GetNumberOfRuns(); i++ )
			{
				pRun = GetController()->GetRun(i);
				assert( pRun!=NULL );
				pRun->Move( Transform );
				pRun->MoveSchematic( Transform, true );
			}

			for( i=GetController()->GetLapsB4Insert(); i<GetController()->GetNumberOfLapboards(); i++ )
			{
				pLap = GetController()->GetLapboard(i);
				assert( pLap!=NULL );
				pLap->Move( Transform );
				pLap->MoveSchematic( Transform, true );
			}

			for( i=GetController()->GetVisualCompsB4Insert(); i<GetController()->GetNumberOfVisualComponents(); i++ )
			{
				pComponent = GetController()->GetVisualComponents()->GetComponent(i);
				assert( pComponent!=NULL );
				pComponent->Move( Transform );
			}
		}

		if( gpController->IsBlockInserting() )
		{
			//acedCommand(RTSTR, _T("_EXPLODE"), RTSTR, _T("_LAST"), RTNONE );
			//acedCommand(RTSTR, _T("_ERASE"), RTSTR, _T("_LAST"), RTNONE );

			////////////////////////////////////////////////////////////////////////
			//We are about to delete the 3D, matrix and schematic and recreate them
			bool b3D, bMatrix;
			//Do we have a 3D and Matrix?
			b3D = gpController->IsCrosshairCreated();
			bMatrix = gpController->GetMatrix()!=NULL;
			//Delete them if created
			if( b3D )
				gpController->Delete3DView();
			if( bMatrix )
				gpController->DeleteMatrix();
			
			//Clear the entire drawing
			Entity Ent;
			//Ent.ExplodeAllEntities( true );
			Ent.DeleteAllEntities( false, false, true, true );
			Ent.DeleteAllEntities( false, false, true, false );
			
			//Recreate the Required views
			gpController->RedrawSchematic(false);
			if( b3D )
				gpController->Create3DView(true);
			if( bMatrix )
				gpController->CreateMatrix(true);
		}
		gpController->SetBlockInserting( false );
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_INVALID ):	//Fallthrough
	default:
		//something went wrong!
		assert( false );
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_UNIMPORTANT ):
		//Unimportant who cares!
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_COPY ):
		pCopiedObjects = GetController()->GetCopiedObjectArray();
		bMessageShown = false;
		while( pCopiedObjects->length()>0 )
		{
			id = pCopiedObjects->at(0);

			pEnt = NULL;

	    es = acdbOpenObject(pEnt, id, AcDb::kForWrite);
			if( es==Acad::eOk )
			{
				assert( pEnt!=NULL );
			}
			else if( es==Acad::eWasOpenForRead )
			{
				// open it for read again to get a pointer to the
				// entity and then close it repeatedly until it's
				// down to one open for read.
				es = acdbOpenObject( pEnt, id, AcDb::kForRead );
				if( es==Acad::eOk )
				{
					assert( pEnt!=NULL );
					do
					{
						pEnt->close();
					}
					while( !pEnt->isReallyClosing( ) );

					// Now that we're down to one open for read, upgrade
					// the open to kForWrite.
					pEnt->upgradeOpen( );
				}
			}
			else
			{
        //acutPrintf(_T("\nacdbOpenObject failed with error %s."), acadErrorStatusText(es));
				pCopiedObjects->removeAt(0);
				continue;
			}

			if( pEnt==NULL ) continue;

			pComponent = GetComponent( pEnt );

			if( pComponent!=NULL )
			{
				if( !bMessageShown && _tcscmp( cmdStr, _T("ARRAY")	)==0 &&
					( pComponent->GetLiftPointer()!=NULL ||
					  pComponent->GetBayPointer()!=NULL ) )
				{
					if( pCopiedObjects->length()>2 )
					{
            sMsg = _T("WARNING:  Some of these components belong to existing bays.\n");
						sMsg+= _T("Arraying these components will delete the originals from those\n");
						sMsg+= _T("bays, I suggest that a better way to array these components is\n");
						sMsg+= _T("to copy the originals and then array those copies.\n\n");
						sMsg+= _T("Continue the operation anyway?\n\n");
            sMsg+= _T("HINT:  Once you have copied the originals, you can choose to\n");
						sMsg+= _T("view only the Visual components when displaying the 3D, please\n");
						sMsg+= _T("read the command line after choosing the 'Create3D' command.\n");
					}
					else
					{
            sMsg = _T("WARNING:  This component belongs to an existing bay.\n");
						sMsg+= _T("Arraying this component will delete the original from that\n");
						sMsg+= _T("bay, I suggest that a better way to array this component is\n");
						sMsg+= _T("to copy the original and then array that copy.\n\n");
						sMsg+= _T("Continue the operation anyway?\n\n");
            sMsg+= _T("HINT:  Once you have copied the original, you can choose to\n");
						sMsg+= _T("view only the Visual components when displaying the 3D, please\n");
						sMsg+= _T("read the command line after choosing the 'Create3D' command.\n");
					}
					if( MessageBox( NULL, sMsg, _T("Delete Existing Components Warning"), MB_OKCANCEL|MB_DEFBUTTON2 )==IDCANCEL )
					{
						GetController()->ClearAllEntityTypes();
						while( pCopiedObjects->length()>0 )
						{
							pCopiedObjects->removeAt(0);
						}
						return;
					}

					//Don't show this message again this time!
					bMessageShown = true;
				}

				if( pComponent->GetLiftPointer()!=NULL ||
					pComponent->GetBayPointer()!=NULL || pComponent->GetVisualComponentsPointer()!=NULL )
				{
					resbuf				*pRes;
					Acad::ErrorStatus	es;
					pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
					if( pRes!=NULL )
					{
						//pRes = XDSC_APPNAME		- XData name
						pRes = pRes->rbnext;		//pRes = XDSC_COMPONENT_POINTER		- Pointer to the component
						pRes = pRes->rbnext;		//pRes = XDSC_POSITION		- 3D Position
						if(pRes->restype==AcDb::kDxfXdWorldXCoord )
						{
							Vector.x = pRes->resval.rpoint[0];
							Vector.y = pRes->resval.rpoint[1];
							Vector.z = pRes->resval.rpoint[2];
						}

						if( _tcscmp( cmdStr, _T("COPY")	)==0 ||
							_tcscmp( cmdStr, _T("ARRAY")	)==0)
						{
							pEnt->close();
							id = pCopiedObjects->at(1);
							pEnt = NULL;
							es = acdbOpenObject(pEnt, id, AcDb::kForWrite);
							assert( es==Acad::eOk );
						}
						else
						{
							assert( _tcscmp( cmdStr, _T("PASTECLIP")	) == 0 );
							sMsg = _T("The paste operation is not supported by KwikScaf!\n");
							sMsg+= _T("The new entity you have just created does not contain\n");
							sMsg+= _T("any KwikScaf data and will not appear in your BOM.\n\n");
							sMsg+= _T("Please use AutoCAD's 'copy' operation instead.");
							MessageBox( NULL, sMsg, _T("Unsupported Operation error"), MB_OK );
							GetController()->ClearAllEntityTypes();
							while( pCopiedObjects->length()>0 )
							{
								pCopiedObjects->removeAt(0);
							}
							return;
	/*
							//the code below will copy the component, on top of the previous
							//	component.  We cannot implement this function since I cannot
							//	get a handle on the new pasted entity, thus I cannot delete it
							//	and I cannot move the now component to the location where they
							//	pasted-moved it!
							es = acdbOpenObject(pEnt, pEnt->objectId(), AcDb::kForWrite);
							assert( es==Acad::eOk );
	*/
						}

						if( pEnt!=NULL )
						{
							Move = GetMovement( pEnt, Vector );
							if( _tcscmp( cmdStr, _T("ARRAY")	)==0 )
							{
								Matrix3D	invTransform, Rotate, Trans;
								Component	*pComp;

								es = pEnt->close();
								assert( es==Acad::eOk );

								///////////////////////////////////////////////////////////////
								//Where should the component be?
								Vector = pComponent->GetLastPosition();
								if( pComponent->GetLiftPointer()!=NULL || pComponent->GetBayPointer()!=NULL )
								{
									//don't delete pComponent yet, just create a copy of it
									pComp = new Component( pComponent->GetLengthCommon(), pComponent->GetType(), pComponent->GetMaterialType(), pComponent->GetSystem() );
									pComp->SetStage(pComponent->GetStage());
									pComp->SetLevel(pComponent->GetLevel());
									pComp->SetController( GetController() );
									GetController()->GetVisualComponents()->AddComponent( pComp );
									pComp->SetTransform( pComponent->GetEntireTransform() );
									Vector = pComponent->GetLastPosition();
									Transform.setToTranslation( Vector );
									invTransform = Transform;
									invTransform.invert();
								}
								else
								{
									//Move the component from its last position to the origin
									Transform.setToTranslation( Vector );
									invTransform = Transform;
									invTransform.invert();
								}

								///////////////////////////////////////////////////////////////
								//Find a rotation matrix for the component based around the origin
								Rotate = GetRotation( pEnt, pComponent->GetLastDirection(), pComponent->GetLastDirectionAlt() );

								if( pComponent->GetLiftPointer()!=NULL || pComponent->GetBayPointer()!=NULL )
								{
									//Delete the existing component
									pComponent->SetDirtyFlag( DF_DELETE );
									//use the new component
									pComponent = pComp;
								}

								///////////////////////////////////////////////////////////////
								//calc the entire transform for the component
								Trans = invTransform;
								Trans = Rotate*Trans;
								Trans = Move*Trans;
								Trans = Transform*Trans;
								Move = Trans;

								es = acdbOpenObject(pEnt, id, AcDb::kForWrite);
								assert( es==Acad::eOk );
							}
							es = pEnt->erase();
							assert( es==Acad::eOk );
							es = pEnt->close();
							assert( es==Acad::eOk );
							//We have deleted a copied entity which has marked our pComponent for delete
							//	we will have to mark it as clean
							pComponent->SetDirtyFlag( DF_CLEAN );

							//Copy the component and redraw
							pComponent = CopyComponent( pComponent, Move );
							if( pComponent!=NULL )
							{
								pComponent->Delete3DView();
								pComponent->Create3DView();
							}
							else
							{
								assert( false );
							}
						}
						pCopiedObjects->removeAt(1);
					}
				}
				else if( pComponent->GetMatrixElementPointer()!=NULL )
				{
					reactors = pEnt->reactors();
					if (reactors != NULL)
					{
						while(reactors->length()>0)
						{
							pSomething = reactors->at(0);
							if (acdbIsPersistentReactor(pSomething))
							{
								reactorId = acdbPersistentReactorObjectId(pSomething);
								pEnt->removePersistentReactor(reactorId);
/*
								if( reactorId.isValid() )
								{
/*									es = acdbOpenAcDbObject((AcDbObject*&)pReactor, reactorId, AcDb::kForWrite);
									if(es==Acad::eOk)
									{
										pReactor->SetMatrixPointer(NULL);
										pReactor->SetControllerPointer(NULL);
										pReactor->SetComponentPointer(NULL);
										pReactor->SetBayPointer(NULL);
										pReactor->SetIsGroup(false);
										es = pReactor->erase();
										assert( es==Acad::eOk);
										es = pReactor->close(); 
										assert( es==Acad::eOk);
										pReactor=NULL;
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
*/							}
							reactors->removeAt(0);
						}
					}
					reactors = pEnt->reactors();
					assert( reactors->length()==0 );
					es = pEnt->setLayer( LAYER_NAME_SECTIONS );
					assert( es==Acad::eOk );
					es = pEnt->close();
					assert( es==Acad::eOk );
					ClearXData( pEnt );
				}
				else
				{
					//What type of component is this?
					assert( false );
				}
			}
			else
			{
				reactors = pEnt->reactors();
				if (reactors != NULL)
				{
					iSize = reactors->length();
					while(iSize>0)
					{
						pSomething = reactors->at(0);
						if (acdbIsPersistentReactor(pSomething))
						{
							reactorId = acdbPersistentReactorObjectId(pSomething);

							pEnt->removePersistentReactor(reactorId);
						}

						//removePersistentReactor may have already removed this from the list
						if( iSize==reactors->length() )
							reactors->removeAt(0);

						iSize = reactors->length();
					}
				}
				reactors = pEnt->reactors();
				assert( reactors->length()==0 );
				es = pEnt->close();
				assert( es==Acad::eOk );
			}

			pCopiedObjects->removeAt(0);
		}

		GetController()->ClearAllEntityTypes();

#ifdef	_DEBUG
    acutPrintf( _T("\nThere are %i visual components:\n"), GetController()->GetVisualComponents()->GetNumberOfComponents() );
		for( i=CT_DECKING_PLANK; i<=CT_VISUAL_COMPONENT; i++ )
		{
			iCount = 0;
			for( j=0; j<GetController()->GetVisualComponents()->GetNumberOfComponents(); j++ )
			{
				pComponent=GetController()->GetVisualComponents()->GetComponent(j);
				if( pComponent->GetType()==(ComponentTypeEnum)i )
				{
					iCount++;
				}
			}
			if( iCount>0 )
			{
				acutPrintf( _T("%i x %s\n"), iCount, GetComponentDescStr( (ComponentTypeEnum)i ) );
			}
		}
#endif	_DEBUG

		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_ERASE ):
/*		//Mark said that this is a message is a bad idea 20001124
		sMsg = "WARNING:  Are you sure you wish to delete these?\n";
		if( MessageBox( NULL, sMsg, "Delete Existing Components Warning", MB_OKCANCEL|MB_DEFBUTTON1 )==IDCANCEL )
		{
			//We need to delete all the current entities from the list
			GetController()->ClearAllEntityTypes();

			//Mark all the component as clean, since some would now be marked as dirty
			GetController()->SetDirtyFlag( DF_CLEAN, true );

			//AutoCAD will delete the entities, so I will need to recreate them
			//	however this could take some time so we will need to ask them if
			//	they want to update
			sMsg = "WARNING:  Some of your views need to be updated!\n\n";
			sMsg+= "Would you like me to update them now?\n";
			if( MessageBox( NULL, sMsg, "Update Views Required", MB_YESNO|MB_DEFBUTTON1 )==IDYES )
			{
				//Yep update the views.
				GetController()->UpdateAllVeiws();
			}
			return;
		}
*/
		pEnt = EnsureEntityWriteable();
		assert( pEnt==NULL );

		//Find out what was erased, and clean up
		ObjectErased();
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_ROTATE ):
		if( m_iAlignCounter<=0 )
		{
			pEnt = EnsureEntityWriteable();
			if( pEnt==NULL )
				return;

			for( i=0; i<m_CurrentIDs.length(); i++ )
			{
				id = m_CurrentIDs.at(i);
				es = acdbOpenObject( pEnt, id, AcDb::kForRead );
				if( es==Acad::eOk && pEnt!=NULL )
				{
					if( pEnt==NULL )
					{
						#ifdef _DEBUG
						acutPrintf( _T(" - No entity!") );
						#endif	//#ifdef _DEBUG
						continue;
					}
					pEnt->close();
					ObjectRotated(pEnt);
				}
				else if( es==Acad::eWasErased )
				{
					//Fine it must have been moved during 
					continue;
				}
				else
				{
					//What happened?
					assert( false );
				}
			}
			GetController()->ClearAllEntityTypes();
		}
		GetController()->CleanUp();
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_ALIGN ):
		assert( m_iAlignCounter==0 );

		pEnt = EnsureEntityWriteable();
		if( pEnt==NULL )
		{
			#ifdef _DEBUG
			acutPrintf( _T(" - No entity!") );
			#endif	//#ifdef _DEBUG
			return;
		}
		ObjectRotated(pEnt);
		GetController()->ClearAllEntityTypes();
		break;

	////////////////////////////////////////////////////////////////
	case( COMMAND_DROP ):
		if( m_iAlignCounter<=0 )
		{
			pEnt = EnsureEntityWriteable();
			if( pEnt!=NULL )
			{
				pEnt->close();
				for( i=0; i<m_CurrentIDs.length(); i++ )
				{
					id = m_CurrentIDs.at(i);
					es = acdbOpenObject( pEnt, id, AcDb::kForRead );
					if( es==Acad::eOk && pEnt!=NULL )
					{
						pEnt->close();
						pBay = GetBay(pEnt);
						if( pBay!=NULL )
						{
							if( pBay->GetBayType()!=BAY_TYPE_LAPBOARD )
								ObjectMovedBay(pEnt);
							else
								ObjectMovedLapboard(pEnt);
						}
						else if( GetComponent(pEnt)!=NULL )
						{
							if( GetComponent(pEnt)->GetMatrixElementPointer()!=NULL )
							{
								ObjectMovedMatrix(pEnt);
							}
							else
							{
								ObjectMoved3D(pEnt);
							}
						}
						else
						{
							if( GetController()->GetMovementEntityType()==ET_LEVEL )
							{
								ObjectMovedLevel(pEnt);
							}
						}
					}
				}
			}
		}
		GetController()->ClearAllEntityTypes();
		break;

	}//switch( iCommand )

	GetController()->SetHasBeenVisited(false);
}

AcDbEntity * AsdkMeccanoEditorReactor::EnsureEntityWriteable()
{
	while( m_CurrentIDs.length()>0 )
	{
		m_CurrentIDs.removeLast();
	}

    //If AutoCAD is shutting down, then do nothing.
    if (!acdbHostApplicationServices()->workingDatabase() )
        return NULL;
    
    // get entities just operated on
    ads_name sset;
    int err = acedSSGet(_T("p"), NULL, NULL, NULL, sset);
    if (err != RTNORM)
	{
        acutPrintf(_T("\nError acquiring previous selection set"));
        return NULL;
    }

    long length;
    acedSSLength(sset, &length);

    ads_name en;
    AcDbObjectId eId;
    AcDbEntity *pEnt;
	pEnt = NULL;
	Acad::ErrorStatus es;
    for( long i=0; i<length; i++ )
	{
        acedSSName(sset, i, en);

        es = acdbGetObjectId(eId, en);
        if( es!=Acad::eOk )
		{
      acutPrintf(_T("\nacdbGetObjectId failed: ")
                _T("Entity name <%lx,%lx>, error %s."), en[0], en[1],
                acadErrorStatusText(es));
            acedSSFree(sset);
            return NULL;
        }

        es = acdbOpenObject(pEnt, eId, AcDb::kForWrite);
        if( es==Acad::eOk )
		{
			assert( pEnt!=NULL );
            pEnt->close();
			m_CurrentIDs.append( eId );
        }
		else if( es==Acad::eWasOpenForRead )
		{
            // open it for read again to get a pointer to the
            // entity and then close it repeatedly until it's
            // down to one open for read.
            es = acdbOpenObject( pEnt, eId, AcDb::kForRead );
			if( es==Acad::eOk )
			{
				assert( pEnt!=NULL );
				do
				{
					pEnt->close();
				}
				while( !pEnt->isReallyClosing( ) );

				// Now that we're down to one open for read, upgrade
				// the open to kForWrite.
				pEnt->upgradeOpen( );

				//do what we want and then close
				m_CurrentIDs.append( eId );
				pEnt->close();
			}
        }
		else
		{
            acutPrintf(_T("\nacdbOpenObject failed with error %s."), acadErrorStatusText(es));
            acedSSFree(sset);
            return NULL;
        }
    }
    acedSSFree(sset);
	return pEnt;
}

void AsdkMeccanoEditorReactor::modelessOperationEnded(const TCHAR *contextStr)
{
#ifdef _DEBUG
	acutPrintf( _T("\nWe need to trap this event! find me in the code! - %s."), contextStr );
assert( false );
	#endif	//#ifdef _DEBUG
}

void AsdkMeccanoEditorReactor::ObjectErased( )
{
	EntityTypeEnum eType;

	assert( GetController()!=NULL );

	//This event may cause a cascade, so we need this flag to prevent this
	if( !GetController()->IsIgnorErase() )
	{
		//The user has erased an entity, identify which one it is and call
		//	the appropriate erase and/or draw procedure
		eType = GetController()->GetErasureEntityType();

		switch( eType )
		{
		case( ET_INVALID ):	//fallthrough
		default:
			assert( false );
			break;
		case( ET_NONE ):	//fallthrough
		case( ET_OTHER ):
			//do nothing
			break;
		case( ET_LEVEL ):
			ObjectErasedLevel();
			break;
		case( ET_BAY ):
			ObjectErasedBay();
			break;
		case( ET_LAPBOARD ):
			ObjectErasedLapboard();
			break;
		case( ET_3D ):
			ObjectErased3D();
			break;
		case( ET_MATRIX ):
			ObjectErasedMatrix();
			break;
		}
	}
	GetController()->ClearAllEntityTypes();
}

void AsdkMeccanoEditorReactor::ObjectErasedLevel()
{
	//Indetify which level it is and erase that one
	int			i, iLevel;
	intArray	iaLevel;

	iaLevel = GetController()->GetEntityLevelID();
	assert( iaLevel.GetSize()>0 );

	for( i=0; i<iaLevel.GetSize(); i++ )
	{
		iLevel = iaLevel.GetAt(i);
		GetController()->RemoveLevel( iLevel );
	}
	iaLevel.RemoveAll();
	GetController()->RegenMatrixLevels();
}

void AsdkMeccanoEditorReactor::ObjectErasedBay()
{
	//the reactor should have marked them for delete, so just
	//	clean them up!
	GetController()->CleanUp();
}

void AsdkMeccanoEditorReactor::ObjectErasedLapboard()
{
	//the reactor should have marked them for delete, so just
	//	clean them up!
	GetController()->CleanUp();
}

void AsdkMeccanoEditorReactor::ObjectErased3D()
{
	//call Cleanup
	//JSB todo 991206 - finish this function

	//the reactor should have marked the component for delete, so just
	//	clean it up!
	GetController()->CleanUp();
}

void AsdkMeccanoEditorReactor::ObjectErasedMatrix()
{
	//it doesn't matter which item they selected,
	//	just delete the entire matrix!
	GetController()->DeleteMatrix();
}


void AsdkMeccanoEditorReactor::ObjectMovedMatrix(AcDbEntity *pEnt)
{
	Component	*pComponent;
	Point3D		ptFrom, ptTo;
	Vector3D	Vector;
	Matrix3D	Transform, InvTrans;

	//This is where the first point should be!
	pComponent = GetController()->GetMovedComponent();
	ptFrom.set( 0.00, 0.00, 0.00 );
	if( pComponent==NULL )
	{
		//this must be the crosshair
		Transform = GetController()->GetMatrixCrosshairTransform();
	}
	else
	{
		Transform = pComponent->GetTransform();
		Transform*= pComponent->GetMatrixElementPointer()->GetTransform();
		Transform*= pComponent->GetMatrixElementPointer()->GetMatrixPointer()->GetMatrixCrosshairTransform();
	}
	ptFrom.transformBy( Transform );

	//This is where it is
	Point3DArray		gripPoints;
	AcDbIntArray		osnapModes, geomIds;
	Acad::ErrorStatus	es;
	es = pEnt->getGripPoints( gripPoints, osnapModes, geomIds);
	assert( gripPoints.length()>0 );
	
	ptTo = gripPoints[0];
	Vector = ptTo-ptFrom;
	Transform.setToTranslation( Vector );
	GetController()->IgnorMovement(true);
	InvTrans = Transform;
	InvTrans.invert();
	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForWrite );
	assert( es==Acad::eOk ); 
	pEnt->transformBy( InvTrans );
	pEnt->close();
	GetController()->IgnorMovement(false);
	GetController()->GetMatrix()->Move( Transform, true );
	GetController()->CreateMatrix(true);
}

void AsdkMeccanoEditorReactor::ObjectMoved3D(AcDbEntity *pEnt)
{
	Run					*pRun;
	Matrix3D			Trans, Original, InvOriginal;
	Vector3D			Vector;
	Component			*pComponent;
//	Acad::ErrorStatus	es;

	if( pEnt==NULL )
		return;

	PrintXData(pEnt);

	pComponent = GetComponent( pEnt );

	if( pComponent!=NULL )
	{
		pRun=NULL;
		if( pComponent->GetLiftPointer()!=NULL )
		{
			assert( pComponent->GetLiftPointer()->GetBayPointer()!=NULL );
			assert( pComponent->GetLiftPointer()->GetBayPointer()->GetRunPointer()!=NULL );
			pRun = pComponent->GetLiftPointer()->GetBayPointer()->GetRunPointer();
		}
		else if( pComponent->GetBayPointer()!=NULL )
		{
			assert( pComponent->GetBayPointer()!=NULL );
			assert( pComponent->GetBayPointer()->GetRunPointer()!=NULL );
			pRun = pComponent->GetBayPointer()->GetRunPointer();
		}

		if( pRun!=NULL )
		{
			Original = pRun->GetTransform();
			InvOriginal = Original;
			InvOriginal.invert();
		}

		Vector = pComponent->GetLastPosition();
#ifdef _DEBUG
    acutPrintf( _T("\nLastPosition: %0.2f, %0.2f, %0.2f"), Vector.x, Vector.y, Vector.z );
#endif //_DEBUG
		Trans = GetMovement( pEnt, Vector );

		//We need to take care of any rotation created by the run being rotated,
		//	but we need to ignor any offset, so create two points which describe
		//	the movement, and use one as the origin.
		Point3D Point1, Point2;
		Point1.set( 0.00, 0.00, 0.00 );
		Point2 = Point1;
		Point2.transformBy( Trans );
		Point1.transformBy( InvOriginal );
		Point2.transformBy( InvOriginal );
		Vector = Point2-Point1;
		Trans.setToTranslation( Vector );

		pComponent->Move( Trans, true, false );
	}
}

void AsdkMeccanoEditorReactor::ObjectRotated3D(AcDbEntity *pEnt)
{
	Matrix3D	Trans, Move, Rotate, Transform, invTransform;
	Vector3D	Vector;
	Component	*pComponent, *pComp;

	if( pEnt==NULL )
		return;

	PrintXData(pEnt);

	///////////////////////////////////////////////////////////////
	//This muse be either a visual component or a 3D component
	pComponent = GetComponent( pEnt );
	if( pComponent==NULL )
	{
		//This is not a visual component!
		assert( false );
		return;
	}

	///////////////////////////////////////////////////////////////
	//find the movement from the origin to the final destination
	Vector.set( 0.00, 0.00, 0.00);
	Move  = GetMovement( pEnt, Vector );

	///////////////////////////////////////////////////////////////
	//Where should the component be?
	Vector = pComponent->GetLastPosition();
	if( pComponent->GetLiftPointer()!=NULL || pComponent->GetBayPointer()!=NULL )
	{
		//don't delete pComponent yet, just create a copy of it
		pComp = new Component( pComponent->GetLengthCommon(), pComponent->GetType(), pComponent->GetMaterialType(), pComponent->GetSystem() );
		pComp->SetStage(pComponent->GetStage());
		pComp->SetLevel(pComponent->GetLevel());
		pComp->SetController( GetController() );
		GetController()->GetVisualComponents()->AddComponent( pComp );
		pComp->SetTransform( pComponent->GetEntireTransform() );
		Vector = pComponent->GetLastPosition();
		Transform.setToTranslation( Vector );
		invTransform = Transform;
		invTransform.invert();
	}
	else
	{
		//Move the component from its last position to the origin
		Transform.setToTranslation( Vector );
		invTransform = Transform;
		invTransform.invert();
	}

	///////////////////////////////////////////////////////////////
	//Find a rotation matrix for the component based around the origin
	Rotate = GetRotation( pEnt, pComponent->GetLastDirection(), pComponent->GetLastDirectionAlt() );

	if( pComponent->GetLiftPointer()!=NULL || pComponent->GetBayPointer()!=NULL )
	{
		//Delete the existing component
		pComponent->SetDirtyFlag( DF_DELETE );
		//use the new component
		pComponent = pComp;
	}

	///////////////////////////////////////////////////////////////
	//calc the entire transform for the component
	Trans = invTransform;
	Trans = Rotate*Trans;
	Trans = Move*Trans;

	///////////////////////////////////////////////////////////////
	//move the component
	pComponent->Move( Trans, true, false );

	///////////////////////////////////////////////////////////////
	//redraw the component in its new position, this will delete the current
	//	entity and recreate a new one
	pComponent->Delete3DView();
	pComponent->Create3DView();
	pComponent->StoreLastPosition();
	pEnt = pComponent->GetEntity();
}

void AsdkMeccanoEditorReactor::ObjectAligned3D(AcDbEntity *pEnt)
{
	if( pEnt==NULL )
		return;

	PrintXData(pEnt);

	Component *pComponent;
	pComponent = GetComponent( pEnt );

	Matrix3D Trans;
	Vector3D Vector;
	Vector = pComponent->GetLastDirection();
#ifdef _DEBUG
  acutPrintf( _T("\nLastDirection: %0.2f, %0.2f, %0.2f"), Vector.x, Vector.y, Vector.z );
#endif //_DEBUG
	Vector = pComponent->GetLastPosition();
#ifdef _DEBUG
  acutPrintf( _T("\nLastPosition: %0.2f, %0.2f, %0.2f"), Vector.x, Vector.y, Vector.z );
#endif //_DEBUG
	Trans  = GetMovement( pEnt, Vector );
//	Trans = GetMovement( pEnt, pComponent->GetLastPosition() );
//	Trans = GetRotation( pEnt )*Trans;

	pComponent->Move( Trans, true, false );
	pEnt = pComponent->GetEntity();
}

void AsdkMeccanoEditorReactor::ObjectMovedBay(AcDbEntity *pEnt)
{
	int					i, j, k, iSize;
	Bay					*pBay;
	Run					*pRun;
	Point3D				ptFrom;	
	Matrix3D			TransRun, TransRunInv;
	Vector3D			Vector;
	intArray			iaBays;
	AcDbGroup			*pGroup;
	AcDbObjectId		id;
	Acad::ErrorStatus	es;
	AcDbObjectIdArray	ids;

	assert( pEnt!=NULL );

	//This is the new method, using XData
	pBay = GetBay( pEnt );
	assert( pBay!=NULL );

	pGroup = pBay->GetTemplate()->GetSchematicGroup();
	if( pGroup!=NULL )
	{
		iSize = (int)pGroup->allEntityIds(ids);
		for( i=0; i<iSize; i++ )
		{
			id = ids[i];
			es = acdbOpenAcDbEntity( pEnt, id, AcDb::kForWrite );
			if( es==Acad::eWasOpenForRead || es==Acad::eWasOpenForWrite )
				PrintXData(pEnt);
			es = pEnt->close();
			assert( es==Acad::eOk );
		}

		if( pBay==NULL || pBay->GetRunPointer()==NULL )
		{
			assert( false );
			return;
		}
		pRun = pBay->GetRunPointer();
		//This is where the point should be
		ptFrom.set( 0.00, 0.00, 0.00 );
		ptFrom.transformBy( pBay->GetSchematicTransform() );
		ptFrom.transformBy( pRun->GetSchematicTransform() );
		Vector.set( ptFrom.x, ptFrom.y, ptFrom.z );
		TransRun = GetMovement( pEnt, Vector );

		////////////////////////////////////////////////////////////////
		//what is the matrix that describes that movement
		TransRunInv = TransRun;
		TransRunInv.invert();

		pRun->Move( TransRunInv, false );
		pBay->Visit( TransRun, true );

		//remove all the bays that have been rotated!
		iaBays = GetController()->GetVisitedBays();
		for( i=0; i<iaBays.GetSize(); i++ )
		{
			pBay = GetController()->GetBayFromBayNumber( iaBays.GetAt(i) );
			if( pBay->HasBeenVisited() )
			{
				iaBays.RemoveAt(i);
				i--;

				//Remove any entities that have already been processed
				pGroup = pBay->GetTemplate()->GetSchematicGroup();
				iSize = (int)pGroup->allEntityIds(ids);
				for( k=0; k<iSize; k++ )
				{
					id = ids[k];
					for( j=0; j<m_CurrentIDs.length(); j++ )
					{
						if( id==m_CurrentIDs[j] )
						{
							m_CurrentIDs.removeAt(j);
							j--;
							break;
						}
					}
					es = acdbOpenAcDbEntity( pEnt, id, AcDb::kForWrite );
					assert( es==Acad::eOk );
					es = pEnt->close();
					assert( es==Acad::eOk );
				}
				assert( iSize==(int)pGroup->numEntities() );
			}
		}
	}

	pEnt->close();
	GetController()->RedrawSchematic(true);
}

void AsdkMeccanoEditorReactor::ObjectMovedLapboard(AcDbEntity *pEnt)
{
	int					i, j, k, iSize;
	Point3D				ptFrom;	
	Matrix3D			Trans;
	Vector3D			Vector;
	AcDbGroup			*pGroup;
	LapboardBay			*pLap;
	AcDbObjectId		id;
	Acad::ErrorStatus	es;
	AcDbObjectIdArray	ids;

	assert( pEnt!=NULL );

	//This is the new method, using XData
	pLap = (LapboardBay*)GetBay( pEnt );
	pEnt->close();
	assert( pLap!=NULL );
	assert( pLap->GetBayType()==BAY_TYPE_LAPBOARD );

	pGroup = pLap->GetTemplate()->GetSchematicGroup();
	if( pGroup!=NULL )
	{
		iSize = (int)pGroup->allEntityIds(ids);

		//check each entity is closed
		for( i=0; i<iSize; i++ )
		{
			id = ids[i];
			es = acdbOpenAcDbEntity( pEnt, id, AcDb::kForWrite );
			if( es==Acad::eWasOpenForRead || es==Acad::eWasOpenForWrite )
			{
				pEnt->downgradeOpen();
				pEnt->downgradeOpen();
				PrintXData(pEnt);
			}
			es = pEnt->close();
			assert( es==Acad::eOk );

			//It must be closed now!
			es = acdbOpenAcDbEntity( pEnt, id, AcDb::kForWrite );
			assert( es!=Acad::eWasOpenForRead && es!=Acad::eWasOpenForWrite );
			es = pEnt->close();
			assert( es==Acad::eOk );
		}

		ptFrom.set( 0.00, 0.00, 0.00 );
		ptFrom.transformBy( pLap->GetSchematicTransform() );
		Vector.set( ptFrom.x, ptFrom.y, ptFrom.z );
		Trans = GetMovement( pEnt, Vector );

		pLap->Move( Trans, true );
		pLap->MoveSchematic( Trans, true );
		pEnt->close();

		//remove all the bays that have been rotated!
		for( k=0; k<iSize; k++ )
		{
			id = ids[k];
			for( j=0; j<m_CurrentIDs.length(); j++ )
			{
				if( id==m_CurrentIDs[j] )
				{
					m_CurrentIDs.removeAt(j);
					j--;
					break;
				}
			}
			es = acdbOpenAcDbEntity( pEnt, id, AcDb::kForWrite );
			assert( es==Acad::eOk );
			es = pEnt->close();
			assert( es==Acad::eOk );
		}
	}
	pEnt->close();
	pLap->UpdateSchematicView();
}

void AsdkMeccanoEditorReactor::ObjectMovedLevel(AcDbEntity *pEnt)
{
	int					i, iLevel;
	double				dNewLevel, dLevelDiff, dCurrentLevel;
	Point3D				ptFrom, ptTo;
	intArray			iaLevels;
	Vector3D			Vector;
	Matrix3D			Trans, Transform;
	Controller			*pController;
	Point3DArray		gripPoints;
	AcDbIntArray		osnapModes, geomIds;
	Acad::ErrorStatus	es;

	//Indetify which level it is and set the new value, and redraw it

	//Compare current y value with y value of the crosshair to get
	//	your new level.
	es = pEnt->getGripPoints( gripPoints, osnapModes, geomIds);

	pController = GetController();

	if( gripPoints.length()>0 )
	{
		//lets find out where the point 0,0,0 should be!
		ptFrom.set( 0.00, 0.00, 0.00 );
		ptFrom.transformBy( pController->GetMatrix()->GetMatrixCrosshairTransform() );

		//What is the new value?
		ptTo = gripPoints[0];
		dNewLevel = ptTo.y - ptFrom.y;
	}
	else
	{
		//why hasn't this got any grip points?
		assert( false );
		return;
	}

	//close the entity.
	pEnt->close();

	//Find the level that was moved, which the has reactor flagged,
	iaLevels = GetController()->GetEntityLevelID();
	assert( iaLevels.GetSize()>0 );
	assert( iaLevels.GetSize()<=GetController()->GetLevelList()->GetSize() );

	dLevelDiff = dNewLevel- (GetController()->GetLevelList()->GetLevel( iaLevels.GetAt(0) )-STAR_SEPARATION);

	for( i=0; i<iaLevels.GetSize(); i++ )
	{
		iLevel = iaLevels.GetAt(i);

		dCurrentLevel = GetController()->GetLevelList()->GetLevel( iLevel );

		//Delete that level!
		GetController()->RemoveLevel( iLevel );

		//Add the new level!
		GetController()->AddLevel( dLevelDiff+dCurrentLevel );
	}
	GetController()->RegenMatrixLevels();
}

Controller * AsdkMeccanoEditorReactor::GetController()
{
	return gpController;
}

void AsdkMeccanoEditorReactor::ObjectRotated(AcDbEntity *pEnt)
{
	EntityTypeEnum eType;

	assert( pEnt!=NULL );

	//The user has erased an entity, identify which one it is and call
	//	the appropriate erase and/or draw procedure
	assert( GetController()!=NULL );
	if( GetBay(pEnt)!=NULL )
	{
		eType = ET_BAY;
		if( GetBay(pEnt)->GetBayType()==BAY_TYPE_LAPBOARD )
			eType = ET_LAPBOARD;
	}
	else if( GetComponent(pEnt)!=NULL )
	{
		eType = ET_3D;
	}
	else
	{
		if( GetController()->GetMovementEntityType()==ET_3D && 
			GetComponent(pEnt)==NULL )
		{
			//This is an pure Autocad objects, not a KwikScaf object!
			return;
		}
		else
		{
			eType = GetController()->GetMovementEntityType();
		}
	}

	switch( eType )
	{
	case( ET_NONE ):
		//Nothing to do with me!
		break;
	case( ET_INVALID ):
	default:
		assert( false );
		break;
	case( ET_3D ):
		ObjectRotated3D(pEnt);
		break;
	case( ET_OTHER ):
	case( ET_MATRIX ):
	case( ET_LEVEL ):
		//do nothing
		break;
	case( ET_BAY ):
		ObjectRotatedBay(pEnt);
		break;
	case( ET_LAPBOARD ):
		ObjectRotatedLapboard(pEnt);
		break;
	}
	//GetController()->ClearAllEntityTypes();
}

void AsdkMeccanoEditorReactor::ObjectRotatedBay(AcDbEntity *pEnt)
{
	int					i; 
	Bay					*pBay;
	double				dAngle;
	Point3D				ptsTo[2], ptsFrom[2];	
	Matrix3D			Transform, Rotation;
	Vector3D			Vector;
	intArray			iaBays; 
//	Acad::ErrorStatus	es;

	assert( pEnt!=NULL );

	pBay = GetBay(pEnt);
	if( pBay!=NULL )
	{
		if( pBay->GetSWStandardEntityID()==pEnt->objectId() )
		{
			////////////////////////////////////////////////////////////////
			//Have we already moved this bay?
			if( pBay->HasBeenVisited() )
				return;

			////////////////////////////////////////////////////////////////
			//This is where it is
			GetBayPoints( pEnt, ptsTo[0], ptsTo[1] );

			////////////////////////////////////////////////////////////////
			//This is where it was
			ptsFrom[0].set( 0.00, 0.00, 0.00 );
			ptsFrom[1].set( 1.00, 0.00, 0.00 );
			Transform = pBay->GetRunPointer()->GetSchematicTransform();
			Transform = Transform*pBay->GetSchematicTransform();
			ptsFrom[0].transformBy(Transform);
			ptsFrom[1].transformBy(Transform);

			//Work out the tranformation matrix for this rotation!
			//	We can use the fact that if you know how far two pairs of points have moved
			//	Then if you draw a line that bisects those point pairs and the intersection
			//	of these two lines will be the 

			Transform.setToIdentity();

			//These 4points must fall on the same plane, and it is
			//	99% likely that this plane will be the z axis!
			//	This means we can essentially ignor the z axis
			assert( (ptsTo[0].z==ptsTo[1].z) &&
					(ptsTo[0].z==ptsFrom[0].z) &&
					(ptsTo[0].z==ptsFrom[1].z) );
			
			Point3D RotationCentre;

			RotationCentre = FindCentreOfRotation(ptsFrom[0], ptsTo[0], ptsFrom[1], ptsTo[1] );

			dAngle = CalculateAngle( ptsTo[0]-RotationCentre, X_AXIS );
			dAngle-= CalculateAngle( ptsFrom[0]-RotationCentre, X_AXIS );
			Vector.set( 0.00, 0.00, 1.00 );

			Rotation.setToRotation( dAngle, Vector, RotationCentre );
			
			pBay->Visit( Rotation, true );

			//remove all the bays that have been rotated!
			for( i=0; i<iaBays.GetSize(); i++ )
			{
				pBay = GetController()->GetBayFromBayNumber( iaBays.GetAt(i) );
				if( pBay->HasBeenVisited() )
				{
					iaBays.RemoveAt(i);
					i--;
				}
			}

			GetController()->RedrawSchematic(true);
		}
	}
}

void AsdkMeccanoEditorReactor::ObjectRotatedLapboard(AcDbEntity *pEnt)
{
	int					j;
	double				dAngle;
	Point3D				ptsTo[2], ptsFrom[2];	
	Matrix3D			Trans, TransInv, Transform, Rotation;
	Vector3D			Vector;
	intArray			iaLapBays;
	LapboardBay			*pLapBay;
	Point3DArray		gripPoints;
	AcDbIntArray		osnapModes, geomIds;
	Acad::ErrorStatus	es;

	assert( pEnt!=NULL );

	iaLapBays = GetController()->GetEntityLapboardID();

	for( j=0; j<iaLapBays.GetSize(); j++ )
	{
		//////////////////////////////////////////////////////
		//This is where the first point should be!
		pLapBay = GetController()->GetLapboard( iaLapBays.GetAt(j) );
		assert( pLapBay!=NULL );

		////////////////////////////////////////////////////////////////
		//This is where it is
		es = pEnt->getGripPoints( gripPoints, osnapModes, geomIds);
		assert( gripPoints.length()>0 );
		
		ptsTo[0] = gripPoints[0];
		ptsTo[1] = gripPoints[1];
		if( !GetController()->m_opaObjectPositions.GetPoints( pEnt->objectId(), ptsFrom[0], ptsFrom[1] ) )
		{
			continue;
		}

		//Work out the tranformation matrix for this rotation!
		//	We can use the fact that if you know how far two pairs of points have moved
		//	Then if you draw a line that bisects those point pairs and the intersection
		//	of these two lines will be the 

		Transform.setToIdentity();

		//These 4points must fall on the same plane, and it is
		//	99% likely that this plane will be the z axis!
		//	This means we can essentially ignor the z axis
		assert( (ptsTo[0].z==ptsTo[1].z) &&
				(ptsTo[0].z==ptsFrom[0].z) &&
				(ptsTo[0].z==ptsFrom[1].z) );
		
		Point3D RotationCentre;

		RotationCentre = FindCentreOfRotation(ptsFrom[0], ptsTo[0], ptsFrom[1], ptsTo[1] );

		dAngle = CalculateAngle( ptsTo[0]-RotationCentre, X_AXIS );
		dAngle-= CalculateAngle( ptsFrom[0]-RotationCentre, X_AXIS );
		Vector.set( 0.00, 0.00, 1.00 );

		Rotation.setToRotation( dAngle, Vector, RotationCentre );
		pLapBay->Move( Rotation, true );

		Rotation.setToRotation( dAngle, Vector, RotationCentre );
		pLapBay->MoveSchematic( Rotation, true );
		pLapBay->UpdateSchematicView();
	}
}

Point3D AsdkMeccanoEditorReactor::FindCentreOfRotation(Point3D A1, Point3D A2, Point3D B1, Point3D B2)
{
	double			dAngle, dTanAngle;
	Point3D			ptCentre, C1, C2;
	const double 	d45Deg = pi/4.00;
	const double 	d90Deg = 2.00*d45Deg;

	C1.x = (A2.x + A1.x)/2.00;
	C1.y = (A2.y + A1.y)/2.00;
	C1.z = (A2.z + A1.z)/2.00;
	C2.x = (B2.x + B1.x)/2.00;
	C2.y = (B2.y + B1.y)/2.00;
	C2.z = (B2.z + B1.z)/2.00;

	//The equation of the line throught the first set of points is
	//ptCentre.y = mx + c
	//where m = (ptsTo[0].y-ptsFrom[0].y) / (ptsTo[0].x-ptsFrom[0].x)
	//and   c = ptsTo[0].y / ptsTo[0].x * ( (ptsTo[0].x-ptsFrom[0].x)/ ptsTo[0].y-ptsFrom[0].y)

	//if( A2.x==A1.x )
	if( A2.x>A1.x-ROUND_ERROR && A2.x<A1.x+ROUND_ERROR  )
	{
		//if( B2.x==B1.x )
		if( B2.x>B1.x-ROUND_ERROR && B2.x<B1.x+ROUND_ERROR )
		{
			if( A1.y>A2.y-ROUND_ERROR && A1.y<A2.y+ROUND_ERROR )
			{
				ptCentre.x = A1.x;
				ptCentre.y = A1.y;
			}
			else if( B1.y>B2.y-ROUND_ERROR && B1.y<B2.y+ROUND_ERROR )
			{
				ptCentre.x = B1.x;
				ptCentre.y = B1.y;
			}
			else
			{
				//find angle change of the two original lines
				//dAngle1 = CalculateAngle( p12-p11, Z_AXIS )
				//dAngle2 = CalculateAngle( p22-p21, Z_AXIS )
				//dAngle = (dAngle2 - dAngle1)/2.00;
				//Tan(dAngle) = (A1.y-C1.y)/(C1.x-ptCentre.x)
				//(C1.x-ptCentre.x) * Tan(dAngle) = (A1.y-C1.y)
				//(C1.x-ptCentre.x) = ((A1.y-C1.y)/Tan(dAngle))
				//ptCentre.x = C1.x-((A1.y-C1.y)/Tan(dAngle))
				//ptCentre.x = C1.x-((A1.y-C1.y)/Tan((CalculateAngle( p22-p21, Z_AXIS ) - CalculateAngle( p12-p11, Z_AXIS ))/2.00))
				dAngle = CalculateAngle( A2-B2, X_AXIS ) - CalculateAngle( A1-B1, X_AXIS );
				if( dAngle==0.00 )
				{
					ptCentre.x = C1.x;
					ptCentre.y = C1.y;
				}
				else if( dAngle==2.00*d90Deg )
				{
					ptCentre.x = C2.x;
					ptCentre.y = C2.y;
				}
				else 
				{
					dTanAngle = tan(dAngle/2.00);
					ptCentre.x = C1.x-((C1.y-A1.y)/dTanAngle);
					ptCentre.y = C1.y;
				}
			}
		}
		else
		{
			assert( B2.x<B1.x-ROUND_ERROR || B2.x>B1.x+ROUND_ERROR );

			//Equation of perpendicular1 line
			//ptCentre.y = C1.y
			//Equation of perpendicular2 line
			//ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
			//if B2.y==B1.y
			//note that if B2.x==B1.x then
			//ptCentre.y = C2.y
			//it follows that
			//C1.y==C2.y
			//ie centre of rotation == C1.x,C1.y == C2.x,C2.y
			if( B2.y>B1.y-ROUND_ERROR && B2.y<B1.y+ROUND_ERROR )
			{
				assert( B2.x<B1.x-ROUND_ERROR || B2.x>B1.x+ROUND_ERROR );

				assert( C1.x>C2.x-ROUND_ERROR && C1.x<C2.x+ROUND_ERROR );
				assert( C1.y>C2.x-ROUND_ERROR && C1.y<C2.x+ROUND_ERROR );
				ptCentre.x = C1.x;
				ptCentre.y = C1.y;
			}
			else
			{
				//Equation of perpendicular1 line
				//ptCentre.y = C1.y
				//Equation of perpendicular2 line
				//ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
				//intersect occurs at
				//C1.y = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
				//C1.y - C2.y + ((B1.x-B2.x)/(B2.y-B1.y))*C2.x = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x)
				//((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) = C1.y - C2.y + ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
				//mulitply both side by (B2.y-B1.y)/(B1.x-B2.x)
				//ptCentre.x = (C1.y - C2.y + ((B1.x-B2.x)/(B2.y-B1.y))*C2.x)*((B2.y-B1.y)/(B1.x-B2.x))
				ptCentre.x = (C1.y - C2.y + ((B1.x-B2.x)/(B2.y-B1.y))*C2.x)*((B2.y-B1.y)/(B1.x-B2.x));
				ptCentre.y = C1.y;
			}
		}
	}
	else if( B2.x>B1.x-ROUND_ERROR && B2.x<B1.x+ROUND_ERROR )
	{
		//already taken care of!
		assert( A2.x<=A1.x-ROUND_ERROR || A2.x>=A1.x+ROUND_ERROR );

		//Equation of perpendicular2 line
		//ptCentre.y = C2.y
		//Equation of perpendicular1 line
		//ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		//if A2.y==A1.y
		//note that if A2.x==A1.x then
		//ptCentre.y = C1.y
		//it follows that
		//C2.y==C1.y
		//ie centre of rotation == C2.x,C2.y == C1.x,C1.y
		if( A2.y>A1.y-ROUND_ERROR && A2.y<A1.y+ROUND_ERROR )
		{
			assert( C2.x>C1.x-ROUND_ERROR && C2.x<C1.x+ROUND_ERROR );
			if( C2.y>C1.y-ROUND_ERROR && C2.y<C1.y+ROUND_ERROR )
			{
				ptCentre.x = C1.x;
				ptCentre.y = C1.y;
			}
			else
			{
				ptCentre.x = C2.x;
				ptCentre.y = C2.y;
			}
		}
		else
		{
			//Equation of perpendicular2 line
			//ptCentre.y = C2.y
			//Equation of perpendicular1 line
			//ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
			//intersect occurs at
			//C2.y = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
			//C2.y - C1.y + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x)
			//((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) = C2.y - C1.y + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
			//mulitply both side by (A2.y-A1.y)/(A1.x-A2.x)
			//ptCentre.x = (C2.y - C1.y + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x)*((A2.y-A1.y)/(A1.x-A2.x))
			ptCentre.x = (C2.y - C1.y + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x)*((A2.y-A1.y)/(A1.x-A2.x));
			ptCentre.y = C2.y;
		}
	}
	else if( A2.y>A1.y-ROUND_ERROR && A2.y<A1.y+ROUND_ERROR )
	{
		assert( A1.x<A2.x-ROUND_ERROR || A1.x>A2.x+ROUND_ERROR );
		if( B2.y>B1.y-ROUND_ERROR && B2.y<B1.y+ROUND_ERROR )
		{
			if( B2.y>B1.y-ROUND_ERROR && B2.y<B1.y+ROUND_ERROR )
			{
				ptCentre.x = C1.x;
				ptCentre.y = C1.y;
			}
			else
			{
				//find angle change of the two original lines
				//dAngle1 = CalculateAngle( p12-p11, Z_AXIS )
				//dAngle2 = CalculateAngle( p22-p21, Z_AXIS )
				//dAngle = (dAngle2 - dAngle1)/2.00;
				//Tan(dAngle) = (C1.x-A1.x)/(C1.y-ptCentre.y)
				//(C1.y-ptCentre.y) * Tan(dAngle) = (C1.x-A1.x)
				//(C1.y-ptCentre.y) = ((C1.x-A1.x)/Tan(dAngle))
				//ptCentre.y = C1.y-((C1.x-A1.x)/Tan(dAngle))
				//ptCentre.y = C1.y-((C1.x-A1.x)/Tan((CalculateAngle( p22-p21, Z_AXIS ) - CalculateAngle( p12-p11, Z_AXIS ))/2.00))
				dAngle = CalculateAngle( A1-A2, X_AXIS ) - CalculateAngle( B1-B2, X_AXIS );
				if( dAngle==0.00 )
				{
					ptCentre.x = C1.x;
					ptCentre.y = C1.y;
				}
				else if( dAngle==2.00*d90Deg )
				{
					ptCentre.x = C1.x;
					ptCentre.y = C1.y;
				}
				else 
				{
					dTanAngle = tan(dAngle/2.00);
					ptCentre.y = C1.y-((C1.x-A1.x)/dTanAngle);
					ptCentre.x = C1.x;
				}
			}
		}
		else
		{
			//Equation of perpendicular1 line
			//ptCentre.x = C1.x
			//Equation of perpendicular2 line
			//ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
			//intersect occurs at
			//ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)C1.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
			ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)*C1.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x;
			ptCentre.x = C1.x;
		}
	}
	else if( B2.y>B1.y-ROUND_ERROR && B2.y<B1.y+ROUND_ERROR )
	{
		assert( A2.y<=A1.y-ROUND_ERROR || A2.y>=A1.y+ROUND_ERROR );
		assert( A1.x<=A2.x-ROUND_ERROR || A1.x>=A2.x+ROUND_ERROR );
		assert( B1.x<=B2.x-ROUND_ERROR || B1.x>=B2.x+ROUND_ERROR );

		//Equation of perpendicular2 line
		//ptCentre.x = C2.x
		//Equation of perpendicular1 line
		//ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		//intersect occurs at
		//ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)C2.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)*C2.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x;
		ptCentre.x = C2.x;
	}
	else if( ((A2.x-A1.x)/(A2.y-A1.y))<((B2.x-B1.x)/(B2.y-B1.y))+ROUND_ERROR_ANGLE_SMALL &&
			 ((A2.x-A1.x)/(A2.y-A1.y))>((B2.x-B1.x)/(B2.y-B1.y))-ROUND_ERROR_ANGLE_SMALL )
	{
		if( (C1.x==C2.x) && (C1.y==C2.y) )
		{
			//these line are parrallel thus the intercept is the the mid point
			ptCentre.x = C1.x;
			ptCentre.y = C1.y;
		}
		else
		{
			//This must something like this:
			//                  |
			//                  xB1
			//                / |
			//              /   xA1
			//            /   / |
			//          /   /   |
			//        /   /     |
			//      /   /       |
			//    x___x_________|____
			//   B2  A2         |
			//                  |
			//                  |
			//Centre will be the intersection of the two lines
			//The B1A1 and B2A2 can't be parrallel also
			double dSlope1, dSlope2, dC1, dC2, dTemp;
			if( B1.x<A1.x+ROUND_ERROR &&
				B1.x>A1.x-ROUND_ERROR )
			{
				if( B2.x<A2.x+ROUND_ERROR &&
						 B2.x>A2.x-ROUND_ERROR )
				{
					//How the hell can this happen when we did a rotation
					assert( false );
				}

				dSlope2 = ((B2.y-A2.y)/(B2.x-A2.x));
				
				//Calculate the intercepts
				//Y = mX + c
				//c = Y - mX
				dC2 = B2.y - (dSlope2*B2.x);
				dTemp = A2.y - (dSlope2*A2.x);
				assert( dC2<dTemp+ROUND_ERROR &&
						dC2>dTemp-ROUND_ERROR );

				ptCentre.x = A1.x;
				//substitute back into Y = m1X + c1
				ptCentre.y = (dSlope2*ptCentre.x) + dC2;
			}
			else if( B2.x<A2.x+ROUND_ERROR &&
					 B2.x>A2.x-ROUND_ERROR )
			{
				dSlope1 = ((B1.y-A1.y)/(B1.x-A1.x));
				
				//Calculate the intercepts
				//Y = mX + c
				//c = Y - mX
				dC1 = B1.y - (dSlope1*B1.x);
				dTemp = A1.y - (dSlope1*A1.x);
				assert( dC1<dTemp+ROUND_ERROR &&
						dC1>dTemp-ROUND_ERROR );

				ptCentre.x = A2.x;
				//substitute back into Y = m2X + c2
				ptCentre.y = (dSlope1*ptCentre.x) + dC1;
			}
			else
			{
				dSlope1 = ((B1.y-A1.y)/(B1.x-A1.x));
				dSlope2 = ((B2.y-A2.y)/(B2.x-A2.x));
				assert( dSlope1>dSlope2+ROUND_ERROR_ANGLE ||
						dSlope1<dSlope2-ROUND_ERROR_ANGLE );
				
				//Calculate the intercepts
				//Y = mX + c
				//c = Y - mX
				dC1 = B1.y - (dSlope1*B1.x);
				dTemp = A1.y - (dSlope1*A1.x);
				assert( dC1<dTemp+ROUND_ERROR &&
						dC1>dTemp-ROUND_ERROR );

				dC2 = B2.y - (dSlope2*B2.x);
				dTemp = A2.y - (dSlope2*A2.x);
				assert( dC1<dTemp+ROUND_ERROR &&
						dC1>dTemp-ROUND_ERROR );

				//since				Y1 = m1X1 + c1
				// and				Y2 = m2X2 + c2
				//when				Y1==Y2 and X1==X2 
				//thus		 m1X1 + c1 = m2X2 + c2
				//substitute	     X = X1 = X2
				//we get	  m1X + c1 = m2X + c2
				//so		 m1X - m2X = c2 - c1
				//			  X(m1-m2) = c2 - c1
				//					 X = (c2 - c1)/(m1-m2)
				ptCentre.x = (dC2-dC1)/(dSlope1-dSlope2);
				//substitute back into Y = m1X + c1
				ptCentre.y = (dSlope1*ptCentre.x) + dC1;
				dTemp = (dSlope2*ptCentre.x) + dC2;
				assert( ptCentre.y<dTemp+ROUND_ERROR &&
						ptCentre.y>dTemp-ROUND_ERROR );
			}
		}
	}
	else
	{
		assert( A2.y<=A1.y-ROUND_ERROR || A2.y>=A1.y+ROUND_ERROR );
		assert( A1.x<=A2.x-ROUND_ERROR || A1.x>=A2.x+ROUND_ERROR );
		assert( B1.x<=B2.x-ROUND_ERROR || B1.x>=B2.x+ROUND_ERROR );

		//Equation of perpendicular1 line
		//ptCentre.y = ((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		//Equation of perpendicular2 line
		//ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
		//combining
		//((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) + C1.y - ((A1.x-A2.x)/(A2.y-A1.y))*C1.x = ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x
		//((A1.x-A2.x)/(A2.y-A1.y)ptCentre.x) - ((B1.x-B2.x)/(B2.y-B1.y)ptCentre.x) = C2.y - C1.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		//ptCentre.x ((A1.x-A2.x)/(A2.y-A1.y) - (B1.x-B2.x)/(B2.y-B1.y)) = C2.y - C1.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x
		//ptCentre.x = (C2.y - C1.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x) / ((A1.x-A2.x)/(A2.y-A1.y) - (B1.x-B2.x)/(B2.y-B1.y))
		ptCentre.x = (C2.y - C1.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x + ((A1.x-A2.x)/(A2.y-A1.y))*C1.x) / ((A1.x-A2.x)/(A2.y-A1.y) - (B1.x-B2.x)/(B2.y-B1.y));
		ptCentre.y = ((B1.x-B2.x)/(B2.y-B1.y)*ptCentre.x) + C2.y - ((B1.x-B2.x)/(B2.y-B1.y))*C2.x;
	}

	return ptCentre;
}

void AsdkMeccanoEditorReactor::PrintXData(AcDbEntity *pEnt)
{
#ifdef _DEBUG
	resbuf	*pRes, *pNext;

	Acad::ErrorStatus	es;
	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	if( es==Acad::eOk || es==Acad::eWasOpenForRead || es==Acad::eWasOpenForWrite )
	{
		pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
		if( pRes==NULL )
			pRes = pEnt->xData(XDATA_LABEL_BAY);

		pNext = pRes;
		while( pNext!=NULL )
		{
//			acutPrintf( "\nType:%i", pNext->restype );
			switch( pNext->restype )
			{
			////////////////////////////////////////////////////////////
			case( 0 ):
			case( AcDb::kDxfXdReal ):
			case( AcDb::kDxfXdDist ):
			case( AcDb::kDxfXdScale ):
        acutPrintf( _T("\trreal:%0.2f"), pNext->resval.rreal );
				break;
			////////////////////////////////////////////////////////////
			case( 1 ):
			case( AcDb::kDxfXdXCoord ):
			case( AcDb::kDxfXdWorldXCoord   ):
			case( AcDb::kDxfXdWorldXDisp   ):
			case( AcDb::kDxfXdWorldXDir   ):
        acutPrintf( _T("\trpoint:[%0.2f"), pNext->resval.rpoint[0] );
			case( AcDb::kDxfXdYCoord ):
			case( AcDb::kDxfXdWorldYCoord   ):
			case( AcDb::kDxfXdWorldYDisp   ):
			case( AcDb::kDxfXdWorldYDir   ):
				acutPrintf( _T(",%0.2f"), pNext->resval.rpoint[1]);
			case( AcDb::kDxfXdZCoord ):
			case( AcDb::kDxfXdWorldZCoord   ):
			case( AcDb::kDxfXdWorldZDisp   ):
			case( AcDb::kDxfXdWorldZDir   ):
				acutPrintf( _T(",%0.2f]"), pNext->resval.rpoint[2] );
				break;
			////////////////////////////////////////////////////////////
			case( 2 ):
			case( AcDb::kDxfXdInteger16 ):
        acutPrintf( _T(" rint:%i"), pNext->resval.rint );
				break;
			////////////////////////////////////////////////////////////
			case( 3 ):
			case( AcDb::kDxfXdAsciiString ):
			case( AcDb::kDxfRegAppName ):
			case( AcDb::kDxfXdControlString ):
			case( AcDb::kDxfXdLayerName ):
        acutPrintf( _T(" rstring:%s"), pNext->resval.rstring );
				break;
			////////////////////////////////////////////////////////////
			case( 4 ):
        acutPrintf( _T(" rlname:%l,%l"), pNext->resval.rlname[0], pNext->resval.rlname[0] );
				break;
			////////////////////////////////////////////////////////////
			case( 5 ):
			case( AcDb::kDxfXdInteger32 ):
        acutPrintf( _T(" rlong:%lx"), pNext->resval.rlong );
				break;
			////////////////////////////////////////////////////////////
			case( 6 ):
			case( AcDb::kDxfXdHandle ):
			case( AcDb::kDxfXdBinaryChunk ):
        acutPrintf( _T(" rbinary:%s"), pNext->resval.rbinary );
				break;
			////////////////////////////////////////////////////////////
			case( 7 ):
        acutPrintf( _T(" ihandle:[%c,%c,%c,%c,%c,%c,%c,%c]"), pNext->resval.ihandle[0], pNext->resval.ihandle[1],
							pNext->resval.ihandle[2], pNext->resval.ihandle[3], pNext->resval.ihandle[4],
							pNext->resval.ihandle[5], pNext->resval.ihandle[6], pNext->resval.ihandle[7] );
				break;
			////////////////////////////////////////////////////////////
			default:
				assert( false );
				break;
			}
			pNext = pNext->rbnext;
		}
		acutRelRb(pRes);
		if( es==Acad::eOk )
			pEnt->close();
	}
#endif	//_DEBUG
}

Component * AsdkMeccanoEditorReactor::GetComponent(AcDbEntity *pEnt)
{
	resbuf				*pRes;
	Component			*pComponent;
	Acad::ErrorStatus	es;

	if( pEnt==NULL )
		return NULL;

  /*
	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	if( !(es==Acad::eOk || es==Acad::eWasOpenForRead || es==Acad::eWasOpenForWrite) )
	{
		return NULL;
	}
  */

	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes==NULL )
	{
		//if( es==Acad::eOk )
		//	pEnt->close();
		return NULL;
	}

	//pRes = XDSC_APPNAME		- XData name
	pRes = pRes->rbnext;		//pRes = XDSC_COMPONENT_POINTER		- Pointer to the component
	if(pRes->restype!=AcDb::kDxfXdInteger32 )
	{
		//if( es==Acad::eOk )
		//	pEnt->close();
		return NULL;
	}

	pComponent = (Component*)pRes->resval.rlong;

    acutRelRb(pRes);
	
  //if( es==Acad::eOk )
  //	pEnt->close();

	//There is still a small chance that this is not a true kwikscaf component.
	if( pComponent==NULL || pComponent->m_iKwikScafComponent!=KWIKSCAF_COMPONENT_ID )
		return NULL;

	return pComponent;
}

Matrix3D AsdkMeccanoEditorReactor::GetMovement(AcDbEntity *pEnt, Vector3D Original)
{
	resbuf				*pRes;
	Point3D				pt;
	Vector3D			Vector;
	Matrix3D			Trans;
	Acad::ErrorStatus	_topen;

	Trans.setToIdentity();
	_topen = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	assert( _topen==Acad::eOk || _topen==Acad::eWasOpenForWrite || _topen==Acad::eWasOpenForRead ); 

	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes!=NULL )
	{
		//pRes = XDSC_APPNAME		- XData name
		pRes = pRes->rbnext;		//pRes = XDSC_COMPONENT_POINTER	- Pointer to the component
		pRes = pRes->rbnext;		//pRes = XDSC_POSITION				- 3D Position
		if(pRes->restype==AcDb::kDxfXdWorldXCoord )
		{
			Vector.x = pRes->resval.rpoint[0];
			Vector.y = pRes->resval.rpoint[1];
			Vector.z = pRes->resval.rpoint[2];
			Vector = Vector-Original;
			Trans.setToTranslation( Vector );
		}

		acutRelRb(pRes);
	}
	else
	{
		pRes = pEnt->xData(XDATA_LABEL_BAY);
		if( pRes!=NULL )
		{
			//pRes = XDSB_APPNAME - XData name,
			pRes = pRes->rbnext;	//pRes = XDSB_BAY_POINTER	- Bay Pointer
			pRes = pRes->rbnext;	//pRes = XDSB_DESCRIPTION	- Description
			pRes = pRes->rbnext;	//pRes = XDSB_POSITION		- 3D Position
			if(pRes->restype==AcDb::kDxfXdWorldXCoord )
			{
				Vector.x = pRes->resval.rpoint[0];
				Vector.y = pRes->resval.rpoint[1];
				Vector.z = pRes->resval.rpoint[2];
				Vector = Vector-Original;
				Trans.setToTranslation( Vector );
			}

			acutRelRb(pRes);
		}
	}
	if( _topen==Acad::eOk )
		pEnt->close();

	return Trans;
}

Matrix3D AsdkMeccanoEditorReactor::GetRotation(AcDbEntity *pEnt, Vector3D From1, Vector3D From2)
{
	resbuf				*pRes;
	double				dAngle1, dAngle2;
	Point3D				pt, ptFrom, ptTo;
	Vector3D			To1, To2, Axis, vPerpen;
	Matrix3D			Trans, Temp;
	AcGePlane			plane1, plane2, plane3;
	AcGeLine3d			line, lPerpen;
	Acad::ErrorStatus	es;

	Trans.setToIdentity();
	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	assert( es==Acad::eOk ); 

	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes!=NULL )
	{
		//pRes = XDSC_APPNAME		- XData name
		pRes = pRes->rbnext;		//pRes = XDSC_COMPONENT_POINTER	- Pointer to the component
		pRes = pRes->rbnext;		//pRes = XDSC_POSITION			- 3D Position
		pRes = pRes->rbnext;		//pRes = XDSC_DISPLACEMENT		- To1 Direction of 1st arm
		if( pRes->restype==AcDb::kDxfXdWorldXDisp )
		{
			To1.x = pRes->resval.rpoint[0];
			To1.y = pRes->resval.rpoint[1];
			To1.z = pRes->resval.rpoint[2];

			pRes = pRes->rbnext;	//pRes = XDSC_DIRECTION		- To1 Direction of 2nd arm
			assert( pRes->restype==AcDb::kDxfXdWorldXDir );
			To2.x = pRes->resval.rpoint[0];
			To2.y = pRes->resval.rpoint[1];
			To2.z = pRes->resval.rpoint[2];

			//////////////////////////////////////////////////////////
			//We now have 2 starting vectors and 2 finishing vectors

			//The two starting vectors should be perpendicular, as
			//	should the ending vectors
			assert( To1.isPerpendicularTo(To2) );
			assert( From1.isPerpendicularTo(From2) );
		
			//We now need to find the angle of rotation from the start to the end
			//	this rotation must be done around a vector which is perpendicular
			//	to both vectors lets find the planes which are perpendicular to
			//	vectors.
			pt = AcGePoint3d::kOrigin;

			//calculate the angle of rotation
			if( From1==To1 )
			{
				dAngle1 = 0.00;
				vPerpen = From1;
			}
			else if( From1.isParallelTo( To1 ) )
			{
				if( From2==To2 )
				{
					//This has been rotate around the From2
					vPerpen = From2;
					dAngle1 = pi;
				}
				else
				{
					//This has been rotate 180deg around some other line, lets
					//	use the other vector perpendicular to From1 and From2
					plane1.set( pt, From1 );
					plane2.set( pt, From2 );
					
					//find the line describing the intersection of the planes
					if( plane1.intersectWith( plane2, lPerpen ) )
					{
						vPerpen = lPerpen.direction();
					}
					else
					{
						//These planes should not be parrallel
						assert( false );
						vPerpen = From1.perpVector();
					}

					dAngle1 = pi;
				}
			}
			else
			{
				plane1.set( pt, From1 );
				plane2.set( pt, To1 );
				
				//find the line describing the 
				if( plane1.intersectWith( plane2, lPerpen ) )
				{
					vPerpen = lPerpen.direction();
				}
				else
				{
					//These planes should not be parrallel
					assert( false );
					vPerpen = From1.perpVector();
				}
				//find the angle of rotation around vPerpen
				dAngle1 = From1.angleTo( To1, vPerpen );
			}

			//rotate both from vector by the angle we just found
			From1.rotateBy( dAngle1, vPerpen );
			From2.rotateBy( dAngle1, vPerpen );
			Trans.setToRotation( dAngle1, vPerpen, pt );

			//Now both From1 and To1 shoud be the same
			assert( From1==To1 );

			//We now have to rotate around the vector To1 from the
			//	vector From2 to To2
			if( From2==To2 )
			{
				dAngle2 = 0;
			}
			else if( From2.isParallelTo( To2 ) )
			{
				dAngle2 = pi;
			}
			else
			{
				//find the angle of rotation around To1
				//	we need to project From2 and To2 onto the plane
				//	which is perpendicular to To1 and then work out
				//	the angle of rotation around
				plane3.set( pt, To1 );

				//From
				ptFrom.set( From2.x, From2.y, From2.z );
				ptFrom = plane3.closestPointTo(ptFrom);
				From2 = ptFrom.asVector();
				From2.normalize();

				//To
				ptTo.set( To2.x, To2.y, To2.z );
				ptTo = plane3.closestPointTo(ptTo);
				To2 = ptTo.asVector();
				To2.normalize();
				
				dAngle2 =  From2.angleTo( To2, To1 );
			}
			From2.rotateBy( dAngle2, To1 );
			Temp.setToRotation( dAngle2, To1, pt );

			Trans = Temp * Trans;
		}

		acutRelRb(pRes);
	}
	else
	{
		pRes = pEnt->xData(XDATA_LABEL_BAY);
		if( pRes!=NULL )
		{
/*			//pRes = XDSB_APPNAME - XData name,
			pRes = pRes->rbnext;	//pRes = XDSB_BAY_POINTER	- Bay Pointer
			pRes = pRes->rbnext;	//pRes = XDSB_DESCRIPTION	- Description
			pRes = pRes->rbnext;	//pRes = XDSB_POSITION		- 3D Position
			pRes = pRes->rbnext;	//pRes = XDSB_DISPLACEMENT	- To1 Direction of 1st arm
			pRes = pRes->rbnext;	//pRes = XDSB_DIRECTION		- To1 Direction of 2nd arm

*/
			//JSB Todo 20000925 - complete this function
			assert( false );
			acutRelRb(pRes);
		}
	}
	pEnt->close();

	return Trans;
}

void AsdkMeccanoEditorReactor::ObjectAligned(AcDbEntity *pEnt)
{
	EntityTypeEnum eType;

	assert( GetController()!=NULL );

	//This event may cause a cascade, so we need this flag to prevent this
	if( !GetController()->IsIgnorErase() )
	{
		//The user has erased an entity, identify which one it is and call
		//	the appropriate erase and/or draw procedure
		eType = GetController()->GetErasureEntityType();

		switch( eType )
		{
		case( ET_3D ):
			ObjectAligned3D(pEnt);
			break;
		case( ET_BAY ):
		case( ET_LAPBOARD ):
		case( ET_MATRIX ):
		case( ET_LEVEL ):
		case( ET_INVALID ):	//fallthrough
		case( ET_NONE ):	//fallthrough
		case( ET_OTHER ):
		default:
			assert( false );
			break;
		}
	}
	GetController()->ClearAllEntityTypes();
}

#include "geassign.h"

Component *AsdkMeccanoEditorReactor::CopyComponent(Component *pComponent, Matrix3D Move )
{
	Matrix3D	Trans;
	Component	*pNewComp;

	//Don't copy text!
	if( pComponent->GetType()==CT_TEXT )
		return NULL;

	pNewComp = new Component( pComponent->GetLengthCommon(), pComponent->GetType(), pComponent->GetMaterialType(), pComponent->GetSystem() );
	gpController->GetVisualComponents()->AddComponent( pNewComp );
	Trans = pComponent->GetTransform();
	Trans = Move*Trans;
	if( pComponent->GetLiftPointer()!=NULL )
	{
		Trans = pComponent->GetLiftPointer()->GetTransform()*Trans;

		assert( pComponent->GetLiftPointer()->GetBayPointer()!=NULL );
		Trans = pComponent->GetLiftPointer()->GetBayPointer()->GetTransform()*Trans;

		assert( pComponent->GetLiftPointer()->GetBayPointer()->GetRunPointer()!=NULL );
		Trans = pComponent->GetLiftPointer()->GetBayPointer()->GetRunPointer()->GetTransform()*Trans;

		Trans = GetController()->Get3DCrosshairTransform()*Trans;
	}
	else if( pComponent->GetBayPointer()!=NULL )
	{
		Trans = pComponent->GetBayPointer()->GetTransform()*Trans;

		assert( pComponent->GetBayPointer()->GetRunPointer()!=NULL );
		Trans = pComponent->GetBayPointer()->GetRunPointer()->GetTransform()*Trans;

		Trans = GetController()->Get3DCrosshairTransform()*Trans;
	}
/*
	if( pNewComp->GetType()==CT_TEXT )
	{
		CString sText;
		sText = pComponent->GetTextSmart();
		pNewComp->SetText( sText );
	}
*/
	pNewComp->SetStockDetailsPointerDumb( pComponent->GetStockDetails() );
	pNewComp->Move( Trans, true );
	pNewComp->Delete3DView();
	pNewComp->Create3DView();

	return pNewComp;
}

double AsdkMeccanoEditorReactor::GetRotationAngle(Vector3D vFrom, Vector3D vTo, AxisEnum eAxis)
{
	if( ( vFrom.x<ROUND_ERROR_ANGLE && vFrom.x>-1.00*ROUND_ERROR_ANGLE ) &&
		( vFrom.y<ROUND_ERROR_ANGLE && vFrom.y>-1.00*ROUND_ERROR_ANGLE ) &&
		( vFrom.z<ROUND_ERROR_ANGLE && vFrom.z>-1.00*ROUND_ERROR_ANGLE ) )
	{
		assert( false );
		return LARGE_NEGATIVE_NUMBER;
	}
	if( ( vTo.x<ROUND_ERROR_ANGLE && vTo.x>-1.00*ROUND_ERROR_ANGLE ) &&
		( vTo.y<ROUND_ERROR_ANGLE && vTo.y>-1.00*ROUND_ERROR_ANGLE ) &&
		( vTo.z<ROUND_ERROR_ANGLE && vTo.z>-1.00*ROUND_ERROR_ANGLE ) )
	{
		assert( false );
		return LARGE_NEGATIVE_NUMBER;
	}

	AcGeVector2d	vFrom2D, vTo2D;

	//Project to the plane at the origin of the axis we are interest in
	//	see page 180 in Foley & Van Dam
	switch( eAxis )
	{
	case( X_AXIS ):
		vFrom2D.x	= vFrom.y;
		vFrom2D.y	= vFrom.z;
		vTo2D.x		= vTo.y;
		vTo2D.y		= vTo.z;
		break;
	case( Y_AXIS ):
		vFrom2D.x	= vFrom.z;
		vFrom2D.y	= vFrom.x;
		vTo2D.x		= vTo.z;
		vTo2D.y		= vTo.x;
		break;
	case( Z_AXIS ):
		vFrom2D.x	= vFrom.x;
		vFrom2D.y	= vFrom.y;
		vTo2D.x		= vTo.x;
		vTo2D.y		= vTo.y;
		break;
	default:
		assert( false );
		return LARGE_NEGATIVE_NUMBER;
	}

	vFrom2D.normalize();
	vTo2D.normalize();

	//The length should now be 1, unless the
	//	vector was 0,0,0 or close to it!
	if( vFrom2D.length()<ROUND_ERROR_ANGLE ||
		vTo2D.length()<ROUND_ERROR_ANGLE )
	{
		//one of the vectors was parrallel to
		//	the axis that we are rotating around
		return 0.00;
	}

	//the functions angle and angleTo return a value from 0 to pi
	//	they should return a value from 0 to 2pi or -pi to pi!
	double dAngle, dAngleTo, dAngleFrom;
	dAngleTo	 = vTo2D.angle();
	dAngleFrom	 = vFrom2D.angle();
	dAngle = dAngleTo-dAngleFrom;

	//return a value from -pi to pi
	while( dAngle>pi )
		dAngle-= pi*2.00;
	while( dAngle<=-1.00*pi )
		dAngle+= pi*2.00;

	return dAngle;
}

Bay *AsdkMeccanoEditorReactor::GetBay(AcDbEntity *pEnt)
{
	resbuf				*pRes;
	Bay					*pBay;
	Acad::ErrorStatus	es;

	if( pEnt==NULL )
		return NULL;

	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	if( !(es==Acad::eOk || es==Acad::eWasOpenForRead || es==Acad::eWasOpenForWrite) )
	{
		return NULL;
	}

	pRes = pEnt->xData(XDATA_LABEL_BAY);
	if( pRes==NULL )
	{
		if( es==Acad::eOk )
			pEnt->close();
		return NULL;
	}

	//pRes = XDSC_APPNAME		- XData name
	pRes = pRes->rbnext;		//pRes = XDSB_BAY_POINTER		- Pointer to the bay
	if(pRes->restype!=AcDb::kDxfXdInteger32 )
	{
		if( es==Acad::eOk )
			pEnt->close();
		return NULL;
	}

	pBay = (Bay*)pRes->resval.rlong;


    acutRelRb(pRes);

	if( es==Acad::eOk )
		pEnt->close();

	return pBay;
}

void AsdkMeccanoEditorReactor::ClearXData(AcDbEntity *pEnt)
{
	resbuf				*pRes;
	Acad::ErrorStatus	es;

	if( pEnt==NULL )
		return;

	pEnt->close();
	es = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForWrite );
	if( !(es==Acad::eOk || es==Acad::eWasOpenForWrite) )
	{
		//not able to write to this entity
		assert( false );
		return;
	}

	pRes = pEnt->xData(XDATA_LABEL_BAY);
	if( pRes!=NULL )
	{
		//I should probably step through the existing data and delete it
		pRes->rbnext = NULL;
		pEnt->setXData(pRes);
	}
    acutRelRb(pRes);

	pRes = pEnt->xData(XDATA_LABEL_COMPONENT);
	if( pRes!=NULL )
	{
		//I should probably step through the existing data and delete it
		pRes->rbnext = NULL;
		pEnt->setXData(pRes);
	}
    acutRelRb(pRes);

	pRes = pEnt->xData(XDATA_LABEL_KUT_COMPONENT);
	if( pRes!=NULL )
	{
		//I should probably step through the existing data and delete it
		pRes->rbnext = NULL;
		pEnt->setXData(pRes);
	}
    acutRelRb(pRes);

	if( es==Acad::eOk )
		pEnt->close();
}


// Insert Events.
//
/////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::beginInsert(AcDbDatabase* pTo, const TCHAR* pBlockName, AcDbDatabase* pFrom)
{
	gpController->SetInsertingBlock(true, true);
	Entity	Ent;
	Ent.MarkAllComponentForPreserve(true);
	gpController->SetBlockInserting( gpScaffoldArchive->LoadArchive(pFrom) );
	gpController->SetInsertingBlock(false);
}

/////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::beginInsert(AcDbDatabase* pTo, const AcGeMatrix3d& xform, AcDbDatabase* pFrom)
{

}

/////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::otherInsert(AcDbDatabase* pTo, AcDbIdMapping& idMap, AcDbDatabase* pFrom)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::abortInsert(AcDbDatabase* pTo)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::endInsert  (AcDbDatabase* pTo)
{
}

////////////////////////////////////////////////////////////////////////////

// Wblock Events.
//
void AsdkMeccanoEditorReactor::wblockNotice(AcDbDatabase* pDb)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom, const AcGePoint3d*& insertionPoint)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom, AcDbObjectId blockId)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::otherWblock(AcDbDatabase* pTo, AcDbIdMapping&, AcDbDatabase* pFrom)
{

}

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::abortWblock(AcDbDatabase* pTo)
{

}

#include "dbMain.h"
#include "migrtion.h"

////////////////////////////////////////////////////////////////////////////
void AsdkMeccanoEditorReactor::endWblock  (AcDbDatabase* pTo)
{
	if (gpScaffoldArchive)
	{
		Acad::ErrorStatus	es;

		//////////////////////////////////////////////////////////////////
		//Get the current selection and store it in m_objIDSaveSelection
		es = acdbGetCurrentSelectionSet( gpController->m_objIDSaveSelection );
		assert( es==Acad::eOk );

		/////////////////////////////////////////////////////////////////////
		//store any selected components in the database
		gpController->SetSaveSelected( true );

		Bay	*pBay;
		Run	*pRun;
		int i, iNumberOfBays, iBayNumber;
		for( i=0; i<gpController->GetNumberOfRuns(); i++ )
		{
			iNumberOfBays = 0;
			pRun = gpController->GetRun(i);
			for( iBayNumber=0; iBayNumber<pRun->GetNumberOfBays(); iBayNumber++ )
			{
				pBay = pRun->GetBay(iBayNumber);
				if( pBay->IsBaySelected() )
					iNumberOfBays++;
			}
			if( iNumberOfBays>0 && iNumberOfBays!=pRun->GetNumberOfBays() )
			{
				//They have not selected all the bays in a run!
				CString	sMsg;
				sMsg.Format( _T("You have only selected %i bays out of the %i bays in Run%i!\n"), 
						iNumberOfBays, pRun->GetNumberOfBays(), (pRun->GetRunID()+1) );
				sMsg+= _T("This will produce some inconsistancies within your WBlock drawing,\n");
				sMsg+= _T("such as missing standards, and transoms or missing handrails.\n");
				sMsg+= _T("You should only select entire runs when saving with WBlock.  If this\n");
				sMsg+= _T("was not done intentionally then you should recreate this WBlock.");
				MessageBox( NULL, sMsg, _T("Partial Run Warning"), MB_OK );

				//Don't warn them again
				break;
			}
		}

		gpScaffoldArchive->SaveArchive( pTo );
		gpController->SetSaveSelected( false );
	}
}


bool AsdkMeccanoEditorReactor::GetBayPoints(AcDbEntity *pEnt, Point3D &pt1, Point3D &pt2)
{
	bool				bReturn;
	resbuf				*pRes;
	Point3D				pt;
	Vector3D			Vector;
	Matrix3D			Trans;
	Acad::ErrorStatus	_topen;

	bReturn = false;
	Trans.setToIdentity();
	_topen = acdbOpenAcDbEntity( pEnt, pEnt->objectId(), AcDb::kForRead );
	assert( _topen==Acad::eOk || _topen==Acad::eWasOpenForWrite || _topen==Acad::eWasOpenForRead ); 

	pRes = pEnt->xData(XDATA_LABEL_BAY);
	if( pRes!=NULL )
	{
		//pRes = XDSB_APPNAME - XData name,
		pRes = pRes->rbnext;	//pRes = XDSB_BAY_POINTER	- Bay Pointer
		pRes = pRes->rbnext;	//pRes = XDSB_DESCRIPTION	- Description
		pRes = pRes->rbnext;	//pRes = XDSB_POSITION		- 3D Position
		if(pRes->restype==AcDb::kDxfXdWorldXCoord )
		{
			pt1.x = pRes->resval.rpoint[0];
			pt1.y = pRes->resval.rpoint[1];
			pt1.z = pRes->resval.rpoint[2];

			pt2 = pt1;
			pRes = pRes->rbnext;	//pRes = XDSB_DISPLACEMENT		- Vector Direction of 1st arm
			if(pRes->restype==AcDb::kDxfXdWorldXDisp )
			{
				pt2.x+= pRes->resval.rpoint[0];
				pt2.y+= pRes->resval.rpoint[1];
				pt2.z+= pRes->resval.rpoint[2];
				bReturn = true;
			}
		}

		acutRelRb(pRes);
	}
	if( _topen==Acad::eOk )
		pEnt->close();

	return bReturn;
}
