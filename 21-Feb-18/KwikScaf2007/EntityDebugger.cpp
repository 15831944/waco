// EntityDebugger.cpp: implementation of the EntityDebugger class.
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
#include "EntityDebugger.h"
#include "Controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EntityDebuggerElement::EntityDebuggerElement()
{
	m_iDeleteCount = 0;
	m_iCreateCount = 0;
}

EntityDebuggerElement::~EntityDebuggerElement()
{

}

EntityDebugger::EntityDebugger()
{
	RemoveAll();
}

EntityDebugger::~EntityDebugger()
{
	RemoveAll();
}

bool EntityDebugger::AddEntityInfo( void *pPointer, CString sDesc )
{
	int						i, iSize;
	AcDbObjectId			id;
	EntityDebuggerElement	*pNew;

	CString					sNumber;
	sNumber.Format( _T("(%i)"), ++giDebugNumber );

	id.setNull();
	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		if( pPointer==GetAt(i)->m_pPointer )
		{
			if( GetAt(i)->m_iDeleteCount!=GetAt(i)->m_iCreateCount)
			{
				//we already have this one in the array
				PrintRemaining();
				;//assert( false );
				return false;
			}
			else
			{
				GetAt(i)->m_iCreateCount++;
        GetAt(i)->m_sDesc+= _T("{");
				GetAt(i)->m_sDesc+= sDesc;
				GetAt(i)->m_sDesc+= sNumber;
        GetAt(i)->m_sDesc+= _T("}");
				return true;
			}
		}
	}

	pNew = new EntityDebuggerElement();

	pNew->m_ID		= id;
	pNew->m_iCreateCount++;
	pNew->m_pPointer = pPointer;
  pNew->m_sDesc	= _T("{");
	pNew->m_sDesc	+= sDesc;
	pNew->m_sDesc	+= sNumber;
  pNew->m_sDesc	+= _T("}");
	i=giDebugNumber;
	pNew->m_iNumber	= i;
	Add( pNew );

	return true;
}

bool EntityDebugger::AddEntityInfo( AcDbObjectId id, CString sDesc )
{
	int						i, iSize;
	AcDbObjectId			idTemp;
	EntityDebuggerElement	*pNew;

	CString					sNumber;
	sNumber.Format( _T("(%i)"), ++giDebugNumber );

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		idTemp = GetAt(i)->m_ID;
		if( idTemp==id )
		{
			if( GetAt(i)->m_iDeleteCount!=GetAt(i)->m_iCreateCount )
			{
				//we already have this one in the array
				PrintRemaining();
				;//assert( false );
				return false;
			}
			else
			{
				GetAt(i)->m_iCreateCount++;
        GetAt(i)->m_sDesc+= _T("{");
				GetAt(i)->m_sDesc+= sDesc;
				GetAt(i)->m_sDesc+= sNumber;
        GetAt(i)->m_sDesc+= _T("}");
				return true;
			}
		}
	}

	pNew = new EntityDebuggerElement();

	pNew->m_ID		= id;
	pNew->m_iCreateCount++;
	pNew->m_pPointer = NULL;
  pNew->m_sDesc	= _T("{");
	pNew->m_sDesc	+= sDesc;
	pNew->m_sDesc+= sNumber;
  pNew->m_sDesc	+= _T("}");
	i=giDebugNumber;
	pNew->m_iNumber	= i;
	Add( pNew );

	return true;
}

bool EntityDebugger::DeleteEntityInfo( AcDbObjectId id )
{
	int						i, iSize;
	EntityDebuggerElement	*pEnt;

	CString					sNumber;
	sNumber.Format( _T("(%i)"), ++giDebugNumber );

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pEnt = GetAt(i);
		if( id == pEnt->m_ID )
		{
			pEnt->m_iDeleteCount++;
			pEnt->m_sDesc+= sNumber;

			if( pEnt->m_iDeleteCount!=pEnt->m_iCreateCount )
			{
				PrintRemaining();
				;//assert( false );
				return false;
			}
			return true;
		}
	}

	//we are deleting something we didn't create!
	PrintRemaining();
	//;//assert( false );
	return false;
}

bool EntityDebugger::DeleteEntityInfo( void *pPointer )
{
	int						i, iSize;
	EntityDebuggerElement	*pEnt;

	CString					sNumber;
	sNumber.Format( _T("(%i)"), ++giDebugNumber );

	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pEnt = GetAt(i);
		if( pPointer==pEnt->m_pPointer )
		{
			pEnt->m_iDeleteCount++;
			pEnt->m_sDesc+= sNumber;
			if( pEnt->m_iDeleteCount!=pEnt->m_iCreateCount )
			{
				PrintRemaining();
				;//assert( false );
				return false;
			}

			return true;
		}
	}

	//we are deleting something we didn't create!
	PrintRemaining();
	return false;
}

void EntityDebugger::PrintRemaining()
{
#ifdef _DEBUG
	int			i, iSize;
	bool		bFound;
	CString		sText;
	CStdioFile	File;
	UINT		uiMode;
	CString		sFilename;

	uiMode = CFile::modeWrite|CFile::modeCreate|CFile::typeText;

	//read the filename from the registry
  sFilename = _T("c:\\EntityDegbugger.txt");

	iSize = GetSize();
	bFound = false;
	acutPrintf(_T("\nEntities remaining") );
	if( File.Open( sFilename, uiMode ) )
	{
		sText.Empty();
		for( i=0; i<iSize; i++ )
		{
			if( GetAt(i)->m_iCreateCount!=GetAt(i)->m_iDeleteCount )
			{
				sText.Format(_T("\n%lx - [%i,%i] - %d(%i) - %s"), GetAt(i)->m_pPointer,
								GetAt(i)->m_iCreateCount,
								GetAt(i)->m_iDeleteCount, GetAt(i)->m_ID,
								GetAt(i)->m_iNumber, GetAt(i)->m_sDesc );
				File.WriteString( sText );
			}
		}
	}
#endif	//#ifdef _DEBUG
}

void EntityDebugger::RemoveAll()
{
	EntityDebuggerElement *pEnt;
	while( GetSize()>0 )
	{
		pEnt = GetAt(0);
		if( pEnt!=NULL )
		{
			delete pEnt;
		}
		else
		{
			;//assert( false );
		}
		RemoveAt(0);
	}
}
