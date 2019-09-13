// ScaffoldArchive.h: interface for the ScaffoldArchive class.
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
#include <afx.h>
#include <dbmain.h>

#if !defined(AFX_SCAFFOLDARCHIVE_H__9EF36786_6011_11D3_9EA3_00508B043A6C__INCLUDED_)
#define AFX_SCAFFOLDARCHIVE_H__9EF36786_6011_11D3_9EA3_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MEMFILE_GROW_INCREMENT 10240  // 10K bytes inc.

//////////////////////////////////////////////////////////////////////
//	Forward Class Declaration
class Controller;

//////////////////////////////////////////////////////////////////////
//	AcMemFile Class
//////////////////////////////////////////////////////////////////////
class AcMemFile : public AcDbObject
{
public:
	AcMemFile( DWORD dwBufferLength = 0, BYTE *pBuffer = NULL);
	virtual ~AcMemFile();

	ACRX_DECLARE_MEMBERS(AcMemFile);
    virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;

	BYTE *GetBufferPointer();
	DWORD GetBufferLength();

protected:
	DWORD m_dwBufferLength;
	BYTE *m_pBuffer;

};


//////////////////////////////////////////////////////////////////////
//	ScaffoldArchive Class
//////////////////////////////////////////////////////////////////////
class ScaffoldArchive  
{
public:
	ScaffoldArchive( Controller	*pController );
	virtual ~ScaffoldArchive();

	bool LoadArchive(AcDbDatabase *pDB=NULL);
	bool SaveArchive(AcDbDatabase* pDB=NULL);

protected:
	Controller	*m_pController;
//	CMemFile	*m_pMemFile;
//	CArchive	*m_pArchive;
//	AcMemFile	*m_pAcMemFile;

	CMemFile	*ReadNamedObjectDictionaryForCMemFile(AcDbDatabase *pDB=NULL);
	bool		 ReadNamedObjectDictionary(AcMemFile *pAcMemFile);
	bool		 WriteNamedObjectDictionary(AcMemFile *pAcMemFile, AcDbDatabase* pDB=NULL );

};

#endif // !defined(AFX_SCAFFOLDARCHIVE_H__9EF36786_6011_11D3_9EA3_00508B043A6C__INCLUDED_)
