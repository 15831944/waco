// ScaffoldArchive.cpp: implementation of the ScaffoldArchive class.
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
#include "meccano.h"
#include "ScaffoldArchive.h"
#include "MeccanoDefinitions.h"
#include <dbdict.h>
#include "Controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MECCANO_DICTIONARY_KEY _T("MECCANO_DICT")


int GetCheckSum(BYTE *pBuffer, DWORD dwBufferLength) //debug code 
{
	int checksum;
	DWORD index;
	index = 0;
	checksum = 0;
	while (index < dwBufferLength)
	{
		checksum += pBuffer[index];
		++index;
	}
	return checksum;
}

//////////////////////////////////////////////////////////////////////
//	AcMemFile Class **************************************************
//////////////////////////////////////////////////////////////////////
ACRX_DXF_DEFINE_MEMBERS(AcMemFile, AcDbObject,
AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
0,ACMEMFILE,KWIKSCAF);

// ************************************************************
// Constructor
// *************
AcMemFile::AcMemFile( DWORD dwBufferLength, BYTE *pBuffer )
{
	m_dwBufferLength = dwBufferLength;
	m_pBuffer = pBuffer;
}

// ************************************************************
// Destructor
// *************
AcMemFile::~AcMemFile()
{
	DELETE_PTR(m_pBuffer);
}

// ************************************************************
// dwgInFields
// *************
Acad::ErrorStatus AcMemFile::dwgInFields(AcDbDwgFiler* pFiler)
{
    ;//assertWriteEnabled();
    AcDbObject::dwgInFields(pFiler);

	Adesk::UInt32 version;
	Adesk::UInt32 bufferLength;

    pFiler->readItem(&version);

	switch (version)
	{
		case AC_MEM_FILE_VERSION_1_0_0 :
		{
			 pFiler->readItem(&bufferLength);
			 m_dwBufferLength = bufferLength;
			 m_pBuffer = new BYTE[bufferLength];
			 pFiler->readBytes(m_pBuffer, bufferLength);
			 break;
		}
		default : ;	// unknown version
	};

    return pFiler->filerStatus();
}

// ************************************************************
// dwgOutFields
// *************
Acad::ErrorStatus AcMemFile::dwgOutFields(AcDbDwgFiler* pFiler) const
{
    ;//assertReadEnabled();
    AcDbObject::dwgOutFields(pFiler);

	Adesk::UInt32 version;
	Adesk::UInt32 bufferLength;

	version = AC_MEM_FILE_VERSION_LATEST;
    pFiler->writeItem(version);

	bufferLength = m_dwBufferLength;

    pFiler->writeItem(bufferLength);
    pFiler->writeBytes(m_pBuffer, bufferLength);

    return pFiler->filerStatus();
}


// ************************************************************
// GetBufferLength
// *************
DWORD AcMemFile::GetBufferLength()
{
	return m_dwBufferLength;
}


// ************************************************************
// GetBufferPointer
// *************
BYTE *AcMemFile::GetBufferPointer()
{
	return m_pBuffer;
}



//////////////////////////////////////////////////////////////////////
//	ScaffoldArchive Class ********************************************
//////////////////////////////////////////////////////////////////////

// ************************************************************
// Constructor
// *************
ScaffoldArchive::ScaffoldArchive( Controller *pController )
{
	m_pController	= pController;
}

// ************************************************************
// Desctructor
// *************
ScaffoldArchive::~ScaffoldArchive()
{
}

// ************************************************************
// LoadArchive
// *************
bool ScaffoldArchive::LoadArchive(AcDbDatabase *pDB/*=NULL*/)
{
	bool		bReturn;
	CMemFile	*pMemFile;
	CArchive	*pArchive;

	bReturn = false;
	pMemFile = ReadNamedObjectDictionaryForCMemFile( pDB );
	if (pMemFile)
	{
		pArchive = new CArchive(pMemFile, CArchive::load, 100000);

		// *********** Start Serialize here
		try
		{
			m_pController->Serialize(*pArchive);		    // Serialize Controller out m_pArchive

			//Did we inserted some kwikScaf components during the insert
			//m_pController->BOMExtraSinceInsert()>0 ||
			if( pDB!=NULL &&
				( m_pController->VisualCompsSinceInsert()>0 ||
				  m_pController->RunsSinceInsert()>0 ||
				  m_pController->BaysSinceInsert()>0 ||
				  m_pController->LapsSinceInsert()>0 ) )
			{
				//we have inserted some KwikScaf components into the drawing
				bReturn = true;
			}
		}
		catch(...)
		{
			CString sMsg;
			sMsg = _T("There was an error reading the file.\nLoad operation has been stopped!");
			MessageBox( NULL, sMsg, _T("Error Reading file"), MB_OK|MB_ICONSTOP );
			pArchive->Abort();
		}
//		CString testStr;							// test code
//		*pArchive >> testStr;						// test code
//		acutPrintf( "Test string :%s ", testStr );  // test code
		// *********** End serialize here

		pArchive->Close();
		pMemFile->Close();

		DELETE_PTR(pArchive);
		DELETE_PTR(pMemFile);
	}
	return bReturn;
}

// ************************************************************
// SaveArchive
// *************
bool ScaffoldArchive::SaveArchive( AcDbDatabase* pDB/*=NULL*/ )
{
	DWORD		 dwBufferLength;
	BYTE		*pBuffer;
	AcMemFile	*pAcMemFile;

	CMemFile	*pMemFile;
	CArchive	*pArchive;

	pMemFile	= new CMemFile( MEMFILE_GROW_INCREMENT );
	pArchive	= new CArchive( pMemFile, CArchive::store);

	// ************** Start Serialize here
	m_pController->Serialize(*pArchive); 	// Serialize Controller into m_pArchive
//	*pArchive << "check this out";			// test code 
	// ************** End Serialize here

	pArchive->Close();

	dwBufferLength	= pMemFile->GetLength();
	pBuffer			= pMemFile->Detach();    // this closes the pMemFile [CMemfile class]
	
//	int checksum = GetCheckSum(pBuffer, dwBufferLength); //test code 
//	acutPrintf( "Write Checksum :%d ", checksum );  // test code

	// AcMemFile owns the pBuffer once passed over
	pAcMemFile = new AcMemFile( dwBufferLength, pBuffer );

	if( !WriteNamedObjectDictionary(pAcMemFile, pDB) )
		return false;
	// WriteNamedObjectDictionary closes the pAcMemFile

	DELETE_PTR(pArchive);
	DELETE_PTR(pMemFile);

	// save job description
	m_pController->SaveJobDescription();

	return true;
}


// ************************************************************
// ReadNamedObjectDictionaryForCMemFile
// *************************************
// Opens the dictionary associated with the key MECCANO_DICTIONARY_KEY
// and iterates through all its entries
CMemFile *ScaffoldArchive::ReadNamedObjectDictionaryForCMemFile(AcDbDatabase *pDB/*=NULL*/)
{
    AcDbDictionary			*pNamedObjectDictionary;
    AcDbDictionary			*pMeccanoDictionary;
    AcDbDictionaryIterator	*pDictIter;
	AcMemFile				*pAcMemFile;
	CMemFile				*pCMemFile;
	BYTE					*pBuffer;
	DWORD					 dwBufferLength;
	Acad::ErrorStatus		es;

	pCMemFile = NULL;
	pAcMemFile = NULL;
	pMeccanoDictionary = NULL;
	pNamedObjectDictionary = NULL;

	// get pointer to Named Object Dictionary
	if( pDB==NULL )
		pDB = acdbHostApplicationServices()->workingDatabase();

	es = pDB->getNamedObjectsDictionary(pNamedObjectDictionary, AcDb::kForRead);

	if( es!=Acad::eOk )
	{
		return NULL;
	}

    // get a pointer to the MECCANO_DICTIONARY_KEY dictionary.
	if( pNamedObjectDictionary!=NULL )
	{
		es = pNamedObjectDictionary->getAt(MECCANO_DICTIONARY_KEY, (AcDbObject*&)pMeccanoDictionary, AcDb::kForRead);
		if( es==Acad::eOk )
		{
			//Fine
			1;
		}
		else if( es==Acad::eKeyNotFound )
		{
			pMeccanoDictionary = NULL;
		}
		else
		{
			;//assert( false );
		}
		if( pNamedObjectDictionary!=NULL )
			pNamedObjectDictionary->close();
	}
	else
	{
		CString sErrMsg;
		sErrMsg = _T("Unable to open Named Object Dictionary!");
		MessageBeep(MB_ICONSTOP);
		MessageBox( NULL, sErrMsg, _T("Database Error"), MB_OK|MB_ICONSTOP );
	}

	if (pMeccanoDictionary)
	{
		// get an iterator for the MECCANO_DICTIONARY_KEY dictionary.
		pDictIter = pMeccanoDictionary->newIterator();

	    for (; !pDictIter->done(); pDictIter->next()) 
		{
			// Get the current record, open it for read
			es = pDictIter->getObject((AcDbObject*&)pAcMemFile, AcDb::kForRead);
			;//assert( es==Acad::eOk );
			dwBufferLength = pAcMemFile->GetBufferLength();
			pBuffer = new BYTE[dwBufferLength];
			memcpy(pBuffer, pAcMemFile->GetBufferPointer(), dwBufferLength);

			// DEBUG CODE START
//			int checksum = GetCheckSum(pBuffer, dwBufferLength); //test code 
//			acutPrintf( "Read Checksum :%d ", checksum );  // test code
			// DEBUG CODE END

			DELETE_PTR(pCMemFile);
			pCMemFile = new CMemFile(pBuffer, dwBufferLength);
			pAcMemFile->close();
		}

		delete pDictIter;
		//delete pDictIter;
		pMeccanoDictionary->close();
	}
	return pCMemFile;
}


// ************************************************************
// ReadNamedObjectDictionary
// *************************
//
// !!!!! This Function needs work !!!!! - It has a number of problems !!!!!!
// !!! do not use !!!
// Opens the dictionary associated with the key MECCANO_DICTIONARY_KEY
// and iterates through all its entries
bool ScaffoldArchive::ReadNamedObjectDictionary(AcMemFile *pAcMemFile)
{
    AcDbDictionary			*pNamedObjectDictionary;
    AcDbDictionary			*pMeccanoDictionary;
    AcDbDictionaryIterator	*pDictIter;

	pMeccanoDictionary = NULL;

	// get pointer to Named Object Dictionary
    acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObjectDictionary, AcDb::kForRead);

    // get a pointer to the MECCANO_DICTIONARY_KEY dictionary.
    pNamedObjectDictionary->getAt(MECCANO_DICTIONARY_KEY, (AcDbObject*&)pMeccanoDictionary, AcDb::kForRead);
    pNamedObjectDictionary->close();

	if (pMeccanoDictionary)
	{
		// get an iterator for the MECCANO_DICTIONARY_KEY dictionary.
		pDictIter= pMeccanoDictionary->newIterator();

	    for (; !pDictIter->done(); pDictIter->next()) 
		{
			// Get the current record, open it for read
			pDictIter->getObject((AcDbObject*&)pAcMemFile, AcDb::kForRead);
			// serialize read here
			//*********
//			pAcMemFile->close();
		}
		delete pDictIter;

		pMeccanoDictionary->close();
		return true;
	}
	else
		return false;
}


// ************************************************************
// WriteNamedObjectDictionary
// *************************
// them to the dictionary associated with the key MECCANO_DICTIONARY_KEY.  
// If this dictionary doesn't exist, it is created and added
// to the named object dictionary.
bool ScaffoldArchive::WriteNamedObjectDictionary(AcMemFile *pAcMemFile, AcDbDatabase* pDB/*=NULL*/ )
{
    AcDbDictionary			*pNamedObjectDictionary;
    AcDbDictionary			*pMeccanoDictionary;
    AcDbObjectId			 MeccanoDictionaryId;
    AcDbObjectId			 AcMemFileId;
	Acad::ErrorStatus		es;

    if( pDB==NULL )
		pDB = acdbHostApplicationServices()->workingDatabase();

	;//assert( pDB!=NULL );
	es = pDB->getNamedObjectsDictionary(pNamedObjectDictionary, AcDb::kForWrite);

	if( es!=Acad::eOk )
	{
		;//assert( false );
		return false;
	}

    // Check to see if the dictionary we want to create is already present. If not, 
	// create it and add it to the named object dictionary.
    if (pNamedObjectDictionary->getAt(MECCANO_DICTIONARY_KEY, (AcDbObject*&)pMeccanoDictionary, AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pMeccanoDictionary = new AcDbDictionary;
        pNamedObjectDictionary->setAt(MECCANO_DICTIONARY_KEY, pMeccanoDictionary, MeccanoDictionaryId);
    }
    pNamedObjectDictionary->close();
    if (pMeccanoDictionary) 
	{
        // add object to the new dictionary, then close it.
        pMeccanoDictionary->setAt(_T("AcMemFile"), pAcMemFile, AcMemFileId);
        pAcMemFile->close();
        pMeccanoDictionary->close();
    }
	return true;
}

