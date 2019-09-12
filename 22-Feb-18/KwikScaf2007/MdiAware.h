//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(MDIAWARE_H)
#define MDIAWARE_H

#include <dbmain.h>
#include <acdocman.h>
#include <dbapserv.h>
#include <aced.h>
#include "EntityDefinitions.h"
#include "GraphicTypes.h"	// Added by ClassView
#include "MeccanoDefinitions.h"

///////////////////////////////////////////////////////////////////////////////
//forward declaration
class AsdkAppDocGlobals;
class AsdkDbReactor;
class Controller;
class Component;
class Bay;
class ScaffoldArchive;

// ****************************************************************
//  GLOBAL VARIABLES!!!  *******************************************
//  gpAsdkAppDocGlobals *******************************************
// ****************************************************************
extern AsdkAppDocGlobals	*gpAsdkAppDocGlobals; 
extern Controller			*gpController;
extern ScaffoldArchive		*gpScaffoldArchive;
extern bool					 gbOpenFileDialogActive;

// ****************************************************************
// class AsdkMeccanoEditorReactor *********************************
// ****************************************************************
class AsdkMeccanoEditorReactor: public AcEditorReactor
{ 
public: 

    // Insert Events.
    //
    virtual void beginInsert(AcDbDatabase* pTo, const TCHAR* pBlockName,
			     AcDbDatabase* pFrom);
    virtual void beginInsert(AcDbDatabase* pTo, const AcGeMatrix3d& xform,
			     AcDbDatabase* pFrom);
    virtual void otherInsert(AcDbDatabase* pTo, AcDbIdMapping& idMap,
			     AcDbDatabase* pFrom);
    virtual void abortInsert(AcDbDatabase* pTo);
    virtual void endInsert  (AcDbDatabase* pTo);

    // Wblock Events.
    //
    virtual void wblockNotice(AcDbDatabase* pDb);
    virtual void beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom,
                             const AcGePoint3d*& insertionPoint);
    virtual void beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom,
                             AcDbObjectId blockId);
    virtual void beginWblock(AcDbDatabase* pTo, AcDbDatabase* pFrom); 
    virtual void otherWblock(AcDbDatabase* pTo, AcDbIdMapping&,
                             AcDbDatabase* pFrom);
    virtual void abortWblock(AcDbDatabase* pTo);
    virtual void endWblock  (AcDbDatabase* pTo);

	AcDbObjectIdArray m_MovedIds;
	double GetRotationAngle( Vector3D vFrom, Vector3D vTo, AxisEnum eAxis );
	Component *GetComponent( AcDbEntity *pEnt );
	void PrintXData( AcDbEntity *pEnt );
	Point3D FindCentreOfRotation( Point3D A1, Point3D A2, Point3D B1, Point3D B2 );

	Controller * GetController();
	AsdkMeccanoEditorReactor();
	~AsdkMeccanoEditorReactor();
	///////////////////////////////////////////////////////
	//command ended stuff
	virtual void commandEnded( const TCHAR *cmdStr );
	virtual void commandWillStart( const TCHAR *cmdStr );

	//other trapable operations
	virtual void modelessOperationEnded(const TCHAR* contextStr);
	virtual void beginSave( AcDbDatabase *pDwg, const TCHAR *pIntendedName );
    virtual void databaseConstructed( AcDbDatabase *pDwg ); 
	virtual void initialDwgFileOpenComplete( AcDbDatabase* pDwg );
	virtual void databaseToBeDestroyed(AcDbDatabase* pDwg);

private:
	bool GetBayPoints( AcDbEntity *pEnt, Point3D &pt1, Point3D &pt2 );
	void ClearXData(AcDbEntity *pEnt);
	Bay	*GetBay( AcDbEntity *pEnt );
	Component *CopyComponent( Component *pComponent, Matrix3D Move );
	void ObjectAligned(AcDbEntity *pEnt);
	void ObjectAligned3D(AcDbEntity *pEnt);
	void ObjectRotated3D(AcDbEntity *pEnt);
	Matrix3D GetRotation(AcDbEntity *pEnt, Vector3D Original, Vector3D OriginalAlt);
	Matrix3D GetMovement( AcDbEntity *pEnt, Vector3D Original );
	//Utilities
	AcDbEntity * EnsureEntityWriteable();

	//object erased
	void ObjectErased();
	void ObjectErasedMatrix();
	void ObjectErased3D();
	void ObjectErasedBay();
	void ObjectErasedLapboard();
	void ObjectErasedLevel();

	//Object moved
	void ObjectMovedMatrix(AcDbEntity *pEnt);
	void ObjectMoved3D(AcDbEntity *pEnt);
	void ObjectMovedBay(AcDbEntity *pEnt);
	void ObjectMovedLapboard(AcDbEntity *pEnt);
	void ObjectMovedLevel(AcDbEntity *pEnt);

	//Object Rotated
	void ObjectRotatedLapboard(AcDbEntity *pEnt);
	void ObjectRotatedBay(AcDbEntity *pEnt);
	void ObjectRotated(AcDbEntity *pEnt);

protected:
	bool m_bKwikScafBlockInserted;
	AcDbObjectIdArray m_CurrentIDs;
	int m_iAlignCounter;
	bool m_bMove;
}; 


// ****************************************************************
// class AsdkDocReactor *******************************************
// ****************************************************************
class AsdkDocReactor: public AcApDocManagerReactor 
{ 
public: 
    virtual void documentToBeActivated(AcApDocument* pDoc); 
    virtual void documentCreated(AcApDocument* pDoc); 
    virtual void documentToBeDestroyed(AcApDocument* pDoc); 
    virtual void documentCreateStarted(AcApDocument* pDoc); 
}; 

// ****************************************************************
// class AsdkPerDocData *******************************************
// ****************************************************************
class AsdkPerDocData  
{ 
    friend class AsdkAppDocGlobals; 
public: 
    AsdkPerDocData(AcApDocument* pDoc); 
	~AsdkPerDocData();

	Controller		*GetController();
	ScaffoldArchive *GetScaffoldArchive();
private: 
    AcApDocument   *m_pDoc; 
    AsdkPerDocData *m_pNext; 
    long			m_EntAcc;			        // Entity count 
    AsdkDbReactor  *m_pDbr;						// Pointer to database reactor 
	Controller	   *m_pController;
	ScaffoldArchive *m_pScaffoldArchive;
}; 

// ****************************************************************
// class AsdkAppDocGlobals ****************************************
// ****************************************************************
class AsdkAppDocGlobals  
{ 
public: 
    AsdkAppDocGlobals(AcApDocument* pDoc); 
    bool setGlobals(AcApDocument* pDoc); 
    void removeDocGlobals(AcApDocument *pDoc); 
    void removeAllDocGlobals(AsdkPerDocData* pTarget); 
    void unload(); 
    long &entityCount(); 
    void incrementEntityCount(); 
    void decrementEntityCount(); 
    AsdkDbReactor *dbReactor(); 

    void setDbReactor(AsdkDbReactor *pDb); 
private: 
    AsdkPerDocData *m_pHead; 
    AsdkPerDocData *m_pData; 
    AsdkDocReactor *m_pDocReactor; 

	bool bGetGlobalsLoopProtection;
}; 

// ****************************************************************
// class AsdkDbReactor ********************************************
// ****************************************************************
// Custom AcDbDatabaseReactor class for Database
// event notification.
//

class AsdkDbReactor : public AcDbDatabaseReactor
{
public:
	ACRX_DECLARE_MEMBERS(AsdkDbReactor);

    virtual void objectAppended(const AcDbDatabase* dwg,
        const AcDbObject* dbObj);

    virtual void objectModified(const AcDbDatabase* dwg,
        const AcDbObject* dbObj);

    virtual void objectErased(const AcDbDatabase* dwg,
        const AcDbObject* dbObj, Adesk::Boolean pErased);
};

#endif // !defined(MDIAWARE_H)
