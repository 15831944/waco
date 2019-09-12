// StockListElement.cpp: implementation of the StockListElement class.
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
#include "StockListElement.h"
#include "meccanoDefinitions.h" 
#include "Entity.h" 
#include "dbidmap.h"
#include "dbsymtb.h"
#include "acestext.h"
#include <dbents.h>
#include "KwikScafComponentEditor.h"
#include "ComponentDetails.h"
#include "controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern bool gbOpenFileDialogActive;
extern Controller			*gpController;

const CString EXPORT_STOCK_PREFIX_TITLE			= _T("!KwikScaf Stock Element");
const CString EXPORT_STOCK_PREFIX_VERSION		= _T("!Version,");
const CString EXPORT_STOCK_PREFIX_EDITED_SSC	= _T("Edited SSC,");
const CString EXPORT_STOCK_PREFIX_VSL_MAJOR		= _T("VSL Major,");
const CString EXPORT_STOCK_PREFIX_VSL_MINOR		= _T("VSL Minor,");
const CString EXPORT_STOCK_PREFIX_VSL_BUILD		= _T("VSL Build,");
const CString EXPORT_STOCK_PREFIX_USE_SALE		= _T("Use Sale Price,");
const CString EXPORT_STOCK_PREFIX_USER_CREATED	= _T("User Created,");
const CString EXPORT_STOCK_PREFIX_PART_NUMBER	= _T("Part Number,");
const CString EXPORT_STOCK_PREFIX_DESCRIPTION	= _T("Description,");
const CString EXPORT_STOCK_PREFIX_HIRE			= _T("Hire($),");
const CString EXPORT_STOCK_PREFIX_SALE			= _T("Sale($),");
const CString EXPORT_STOCK_PREFIX_WEIGHT		= _T("Weight(kg),");
const CString EXPORT_STOCK_PREFIX_LENGTH		= _T("Length(mm),");
const CString EXPORT_STOCK_PREFIX_WIDTH			= _T("Width,");
const CString EXPORT_STOCK_PREFIX_HEIGHT		= _T("Height,");
const CString EXPORT_STOCK_PREFIX_FILENAME_2DEO	= _T("Filename 2D end on,");
const CString EXPORT_STOCK_PREFIX_FILENAME_2D	= _T("Filename 2D,");
const CString EXPORT_STOCK_PREFIX_FILENAME_3D	= _T("Filename 3D,");

//This is here because these number will be changed regularly, and if they were
//	in Meccano definitions then we would be doing a full compile each time we
//	changed one of these numbers
const	int VERSION_MAJOR_1	= 1;

const	int VERSION_MINOR_3	= 3;
const	int VERSION_MINOR_5	= 5;

const	int VERSION_BUILD_0	= 0;
const	int VERSION_BUILD_1	= 1;
const	int VERSION_BUILD_2	= 2;
const	int VERSION_BUILD_3	= 3;
const	int VERSION_BUILD_4	= 4;
const	int VERSION_BUILD_5	= 5;
const	int VERSION_BUILD_6	= 6;
const	int VERSION_BUILD_7	= 7;
const	int VERSION_BUILD_8	= 8;

const int VERSION_STOCK_CURRENT_MAJOR = VERSION_MAJOR_1;
const int VERSION_STOCK_CURRENT_MINOR = VERSION_MINOR_5;
const int VERSION_STOCK_CURRENT_BUILD = VERSION_BUILD_1;

const int VERSION_DRAWING_DETAILS_CURRENT_MAJOR = VERSION_MAJOR_1;
const int VERSION_DRAWING_DETAILS_CURRENT_MINOR = VERSION_MINOR_5;
const int VERSION_DRAWING_DETAILS_CURRENT_BUILD = VERSION_BUILD_2;

const CString STOCK_CODE_FILE_LABEL = _T("~Stock Code File");
const CString DRAWING_DEFINIATION_FILE_LABEL = _T("~Drawing Details File");

const double INVALID_DIMENSION = -1.00;

#define	USE_INSERT_METHOD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction
StockListElement::StockListElement()
{
	CString sEmpty;
	sEmpty.Empty();
	SetPartNumber(	sEmpty );
	SetDescription(	sEmpty );
	SetFilename3D(	sEmpty );
	SetFilename2D(	sEmpty );
	SetFilename2DEO(sEmpty );
	SetHire(	INVALID_DIMENSION);
	SetSale(	INVALID_DIMENSION );
	SetWeight(	INVALID_DIMENSION );
	SetLength(	INVALID_DIMENSION );
	SetWidth(	INVALID_DIMENSION );
	SetHeight(	INVALID_DIMENSION );
	SetEntity( NULL, VT_3D );
	SetEntity( NULL, VT_2D );
	SetEntity( NULL, VT_2DEO );
	SetUseSalePrice( false );
	m_bUserCreatedElement = false; 
	SetSSCEdited( false );
}

//////////////////////////////////////////////////////////////////////
// Destruction
StockListElement::~StockListElement()
{

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction
StockListArray::StockListArray()
{
	m_sLastPartNumber.Empty();
	SetSize(0);
}

//////////////////////////////////////////////////////////////////////
// Destruction
StockListArray::~StockListArray()
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// GetFile
bool StockListArray::GetFile(CStdioFile &File, bool bStockFile )
{
	UINT					uiMode;
	CString					sFilename;
	CFileDialog				*pDialog;

	uiMode = CFile::modeRead|CFile::typeText;

	//read the filename from the registry
	sFilename = GetFilenameInRegistry(bStockFile);

	//try the default filename
	if( sFilename.IsEmpty() || !File.Open( sFilename, uiMode ) )
	{
		///////////////////////////////////////////////////
		//Get the file name to use from the user
		if( bStockFile )
		{
			pDialog = new CFileDialog(TRUE, EXTENTION_LOAD_STOCK_LIST_FILE, sFilename,
							OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_LOAD_STOCK_LIST_FILE );
			pDialog->m_ofn.lpstrTitle = TITLE_LOAD_STOCK_LIST_FILE;
		}
		else
		{
			pDialog = new CFileDialog(TRUE, EXTENTION_LOAD_DRAWING_DETAILS_FILE, sFilename,
							OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_LOAD_DRAWING_DETAILS_FILE );
			pDialog->m_ofn.lpstrTitle = TITLE_LOAD_DRAWING_DETAILS_FILE;
		}

		gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
		if (pDialog->DoModal()!=IDOK)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			gbOpenFileDialogActive = false;            
			return false;
		}
		gbOpenFileDialogActive = false;            
		sFilename = pDialog->GetPathName();

		//open it this time
		CFileException Error;
		if( !File.Open( sFilename, uiMode, &Error ) )
		{
			TCHAR   szCause[255];
			CString strFormatted;
			CString sMessage;

			MessageBeep(MB_ICONEXCLAMATION);
			Error.GetErrorMessage(szCause, 255);
			sMessage = ERROR_MSG_NOT_OPEN_FILE_READ;
			sMessage+= szCause;
			MessageBox( NULL, sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
			ClearFilenameInRegistry( bStockFile );
			return false;
		}

		//store the filename in the registry
		SetFilenameInRegistry( sFilename, bStockFile );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// SetFilenameInRegistry
// Changed LPCTSTR to CString, ~BSK, 21.02.2018
void StockListArray::SetFilenameInRegistry(CString strFileName, bool bStockFile )
{
	if( bStockFile )
		::SetStringInRegistry( PROJECT_STOCK_LIST_FILENAME_SECTION, strFileName);
	else
		::SetStringInRegistry( PROJECT_DRAWING_DETAILS_FILENAME_SECTION, strFileName);
}

//////////////////////////////////////////////////////////////////////
// GetFilenameInRegistry
// Changed LPCTSTR to CString, ~SJ, 03.09.2007
CString StockListArray::GetFilenameInRegistry(bool bStockFile)
{
	if( bStockFile )
		return ::GetStringInRegistry(PROJECT_STOCK_LIST_FILENAME_SECTION);
	else
		return ::GetStringInRegistry(PROJECT_DRAWING_DETAILS_FILENAME_SECTION);
}

//////////////////////////////////////////////////////////////////////
// 
void StockListElement::SetEntPathInRegistry(LPCTSTR strFileName, PathEnum eP)
{
	//Store the filename in the registry
	CWinApp	*pApp;
	pApp = AfxGetApp();
	switch( eP )
	{
	case(P_STOCK):		//stock.csv
		SetStringInRegistry( PROJECT_STOCK_LIST_ENT_PATH_SECTION, strFileName);
		break;
	case(P_DRAWING):	//DrawingDetails.csv
		SetStringInRegistry( PROJECT_DRAWING_DETAILS_ENT_PATH_SECTION, strFileName);
		break;
	case(P_ENTITY):	//Path for entity files
		SetStringInRegistry( PROJECT_COMPONENT_DETAILS_ENT_PATH_SECTION, strFileName);
		break;
	default:
		;//assert(false);
	}
}

//////////////////////////////////////////////////////////////////////
// 
// Changed LPCTSTR to CString, ~SJ, 27.06.2008
CString StockListElement::GetEntPathInRegistry(PathEnum eP)
{
	//Get the filename from the registry
	switch( eP )
	{
	case(P_STOCK):		//stock.csv
		return GetStringInRegistry(PROJECT_STOCK_LIST_ENT_PATH_SECTION);
	case(P_DRAWING):	//DrawingDetails.csv
		return GetStringInRegistry(PROJECT_DRAWING_DETAILS_ENT_PATH_SECTION);
	case(P_ENTITY):	//Path for entity files
		return GetStringInRegistry(PROJECT_COMPONENT_DETAILS_ENT_PATH_SECTION);
	default:
		;//assert(false);
	}

	return "";
}


//////////////////////////////////////////////////////////////////////
// GetNextDoubleAndMoveOn
double StockListArray::GetNextDoubleAndMoveOn(CString &sLine)
{
	int		iComma;
	CString	sText;

	iComma = sLine.Find( COMMA );
	if( iComma<1 )
	{
		if (sLine.GetLength()>0)
		{
			double	dValue;
			dValue = _tstof(sLine);
			sLine.Empty();
			return dValue;
		}
	}
//		return SLT_NOT_FOUND;	//no comma

	sText = sLine.Left( iComma );
	sLine = sLine.Right( sLine.GetLength()-(iComma+1) );

//	if (sText[0] == '$')       // get rid of any dollar signs
	sText.TrimLeft(_T('$'));

	if( _istdigit( sText[0] ) || sText[0]==_T('-') || sText[0]==_T('.') )
		return _tstof( sText );

	return SLT_NOT_FOUND;
}


//////////////////////////////////////////////////////////////////////
//LoadList
bool StockListArray::LoadList()
{
	int					iComma;
	double				dHire, dSale, dWeight;
	CString				sLine, sPartNumber, sErrMsg, sDescription, sText,
						sVersionMajor, sVersionMinor, sVersionBuild;
	CStdioFile			File;
	StockListElement	*pElement;

	///////////////////////////////////////////////////
	//clear the length table
	RemoveAll();

	///////////////////////////////////////////////////
	//open the file
	if( !GetFile( File, true ) )
		return false;

	///////////////////////////////////////////////////
	//read the file into the table
	while( File.ReadString( sLine ) )
	{
		///////////////////////////////////////////////////////////////
		//Is this a comment?
		if( sLine[0]==_T('!') )	continue;

		///////////////////////////////////////////////////////////////
		//Is this a Version?
		if( sLine[0]==_T('~') )
		{
			////////////////////////////////////////////////////////////
			//!File Type - ignor
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sText = sLine.Left( iComma );
			if( sText!=STOCK_CODE_FILE_LABEL )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sErrMsg.Format( _T("The file, %s, is not a Stock Code File"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
				return false;
			}
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );

			////////////////////////////////////////////////////////////
			//Version Number Major
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sVersionMajor	= sLine.Left( iComma );
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( !_istdigit( sVersionMajor[0] ) )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Major version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Version Number Minor
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sVersionMinor	= sLine.Left( iComma );
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( !_istdigit( sVersionMinor[0] ) )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Minor version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Version Number Build	
			sVersionBuild = sLine;
			if( !_istdigit( sVersionBuild[0] ) )
			{
				;//assert( false );
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Build version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}
				
			////////////////////////////////////////////////////////////
			//Get the version numbers
			m_iVSLMajor	= (ComponentTypeEnum)_ttoi( sVersionMajor );
			m_iVSLMinor	= (ComponentTypeEnum)_ttoi( sVersionMinor );
			m_iVSLBuild	= (ComponentTypeEnum)_ttoi( sVersionBuild );

			////////////////////////////////////////////////////////////
			//Is this the correct version?
			if( m_iVSLMajor	!=VERSION_STOCK_CURRENT_MAJOR || 
				m_iVSLMinor	!=VERSION_STOCK_CURRENT_MINOR ||
				m_iVSLBuild	!=VERSION_STOCK_CURRENT_BUILD )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Incorrect version number for the file: %s\nThis file is marked as version : %i.%i.%i\nHowever, this ARX requires version: %i.%i.%i"),
								File.GetFileName(), m_iVSLMajor, m_iVSLMinor, m_iVSLBuild,
								VERSION_STOCK_CURRENT_MAJOR, VERSION_STOCK_CURRENT_MINOR, VERSION_STOCK_CURRENT_BUILD );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Fine - correct version
			continue;
		}

		///////////////////////////////////////////////////////////////
		//find the partNumber type
		iComma			= sLine.Find( COMMA );
		if( iComma<1 )	continue;	//no comma

		sPartNumber		= sLine.Left( iComma );
		sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );

		///////////////////////////////////////////////////////////////
		//find the Description
		iComma			= sLine.Find( COMMA );
		if( iComma>=0 )
		{
			sDescription	= sLine.Left( iComma );
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
		}

		///////////////////////////////////////////////////////////////
		//find the Hire Rate
		dHire = GetNextDoubleAndMoveOn( sLine );
		if( dHire==SLT_NOT_FOUND )
			dHire = INVALID_DIMENSION;

		///////////////////////////////////////////////////////////////
		//find the Sale Rate
		dSale = GetNextDoubleAndMoveOn( sLine );
		if( dSale==SLT_NOT_FOUND )
			dSale = INVALID_DIMENSION;

		///////////////////////////////////////////////////////////////
		//find the Weight Rate
		dWeight = GetNextDoubleAndMoveOn( sLine );
		if( dWeight==SLT_NOT_FOUND )
			dWeight = INVALID_DIMENSION;

		// add element
		pElement = new StockListElement;
		pElement->SetPartNumber( sPartNumber );
		pElement->SetDescription( sDescription );
		pElement->SetHire( dHire );
		pElement->SetSale( dSale );
		pElement->SetWeight( dWeight );
		Add( pElement );
	}
	//did we find anything?
	if( GetSize()<=0 )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		sErrMsg.Format( _T("The file, %s, is not a Stock Code File"),
						File.GetFileName() );
		MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return false;
	}
	File.Close();

	///////////////////////////////////////////////////
	//open the file
	if( !GetFile( File, false ) )
		return false;

	double	dLength, dWidth, dHeight;
	CString	sFilename3D, sFilename2D, sFilename2DEO;
	CStringArray	saDuplicates;

	///////////////////////////////////////////////////
	//read the file into the table
	saDuplicates.RemoveAll();
	while( File.ReadString( sLine ) )
	{
		dLength	= INVALID_DIMENSION;
		dWidth	= INVALID_DIMENSION;
		dHeight	= INVALID_DIMENSION;
		sFilename3D.Empty();
		sFilename2D.Empty();
		sFilename2DEO.Empty();

		///////////////////////////////////////////////////////////////
		//Is this a comment?
		if( sLine[0]==_T('!') )	continue;

		///////////////////////////////////////////////////////////////
		//Is this a Version?
		if( sLine[0]==_T('~') )
		{
			////////////////////////////////////////////////////////////
			//!File Type - ignor
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sText = sLine.Left( iComma );
			if( sText!=DRAWING_DEFINIATION_FILE_LABEL )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sErrMsg.Format( _T("The file, %s, is not a Drawing Details File"), File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
				return false;
			}
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );

			////////////////////////////////////////////////////////////
			//Version Number Major
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sVersionMajor	= sLine.Left( iComma );
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( !_istdigit( sVersionMajor[0] ) )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Major version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Version Number Minor
			iComma			= sLine.Find( COMMA );
			if( iComma<1 )	continue;	//no comma
			sVersionMinor	= sLine.Left( iComma );
			sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( !_istdigit( sVersionMinor[0] ) )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Minor version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Version Number Build	
			sVersionBuild = sLine;
			if( !_istdigit( sVersionBuild[0] ) )
			{
				;//assert( false );
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Cannot read the Build version from the file: %s"),
								File.GetFileName() );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}
				
			////////////////////////////////////////////////////////////
			//Get the version numbers
			m_iVVCMajor	= (ComponentTypeEnum)_ttoi( sVersionMajor );
			m_iVVCMinor	= (ComponentTypeEnum)_ttoi( sVersionMinor );
			m_iVVCBuild	= (ComponentTypeEnum)_ttoi( sVersionBuild );

			////////////////////////////////////////////////////////////
			//Is this the correct version?
			if( m_iVVCMajor	!=VERSION_DRAWING_DETAILS_CURRENT_MAJOR || 
				m_iVVCMinor	!=VERSION_DRAWING_DETAILS_CURRENT_MINOR ||
				m_iVVCBuild	!=VERSION_DRAWING_DETAILS_CURRENT_BUILD )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Incorrect version number for the file: %s\nThis file is marked as version : %i.%i.%i\nHowever, this ARX requires version: %i.%i.%i"),
								File.GetFileName(), m_iVVCMajor, m_iVVCMinor, m_iVVCBuild,
								VERSION_DRAWING_DETAILS_CURRENT_MAJOR, VERSION_DRAWING_DETAILS_CURRENT_MINOR, VERSION_DRAWING_DETAILS_CURRENT_BUILD );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Fine - correct version
			continue;
		}

		///////////////////////////////////////////////////////////////
		//find the partNumber type
		iComma			= sLine.Find( COMMA );
		if( iComma<1 )	continue;	//no comma

		sPartNumber		= sLine.Left( iComma );
		sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );

		///////////////////////////////////////////////////////////////
		//find the Length
		dLength = GetNextDoubleAndMoveOn( sLine );
		if( dLength==SLT_NOT_FOUND )
			dLength = INVALID_DIMENSION;

		///////////////////////////////////////////////////////////////
		//find the Width
		dWidth = GetNextDoubleAndMoveOn( sLine );
		if( dWidth==SLT_NOT_FOUND )
			dWidth = INVALID_DIMENSION;

		///////////////////////////////////////////////////////////////
		//find the Height
		dHeight = GetNextDoubleAndMoveOn( sLine );
		if( dHeight==SLT_NOT_FOUND )
			dHeight = INVALID_DIMENSION;

		///////////////////////////////////////////////////////////////
		//find the 3D Filename
		iComma			= sLine.Find( COMMA );
		if( iComma>=0 )
		{
			sFilename3D	= sLine.Left( iComma );
			sLine	= sLine.Right( sLine.GetLength()-(iComma+1) );

			///////////////////////////////////////////////////////////////
			//find the 2D Filename
			iComma			= sLine.Find( COMMA );
			if( iComma>=0 )
			{
				sFilename2D	= sLine.Left( iComma );
				sLine	= sLine.Right( sLine.GetLength()-(iComma+1) );

				///////////////////////////////////////////////////////////////
				//find the 2D Filename End On
				iComma			= sLine.Find( COMMA );
				if( iComma>=0 )
				{
					sFilename2DEO	= sLine.Left( iComma );
					sLine	= sLine.Right( sLine.GetLength()-(iComma+1) );
				}
				else
				{
					if( !sLine.IsEmpty() )
						sFilename2DEO = sLine;
				}
			}
			else
			{
				if( !sLine.IsEmpty() )
					sFilename2D = sLine;
			}
		}
		else
		{
			if( !sLine.IsEmpty() )
				sFilename3D = sLine;
		}

		///////////////////////////////////////////////////////////////
		//Add details about this element!
		pElement = NULL;
		pElement = GetMatchingComponent( sPartNumber );
		if( pElement!=NULL )
		{
			//The length must always be valid
			if( pElement->GetLength()>INVALID_DIMENSION+ROUND_ERROR ||
				pElement->GetLength()<INVALID_DIMENSION-ROUND_ERROR )
			{
				//We already have data for this part number!
				saDuplicates.Add( sPartNumber );
			}
			else
			{
				pElement->SetLength( dLength );
				pElement->SetWidth( dWidth );
				pElement->SetHeight( dHeight );
				pElement->SetFilename3D( sFilename3D );
				pElement->SetFilename2D( sFilename2D );
				pElement->SetFilename2DEO( sFilename2DEO );
			}
		}
		else
		{
/*
			pElement = new StockListElement;
			pElement->SetPartNumber( sPartNumber );
			pElement->SetDescription( sDescription );
			pElement->SetHire( dHire );
			pElement->SetSale( dSale );
			pElement->SetWeight( dWeight );
			Add( pElement );
			CT_VISUAL_COMPONENT
*/
			//The component does not exist in the stock file!
			CString sMsg;
      sMsg.Format( _T("Part number: %s in the Drawing Details file: %s\n"),
						sPartNumber, File.GetFileName() );
			sMsg+= _T("does not have a matching part number in the Stock file.\n\n");
			sMsg+= _T("This component cannot be drawn!");
			MessageBox( NULL, sMsg, _T("Drawing Details not in Stock file"), MB_OK );
		}
	}

	if( saDuplicates.GetSize()>0 )
	{
		CString sMsg;
		if( saDuplicates.GetSize()==1 )
		{
      sMsg.Format( _T("Part number: %s in the Drawing Details file: %s\n"),
						saDuplicates.GetAt(0), File.GetFileName() );
			sMsg+= _T("Has duplicate entries.  ");
		}
		else
		{
      int i=0;
      sMsg.Format( _T("The Drawing Details file: %s\nContains the following duplicated components:\n"),
								File.GetFileName() );
			for( i=0; i<saDuplicates.GetSize()-1; i++ )
			{
				sMsg+= saDuplicates.GetAt(i);
				if( i%5==4 )
					sMsg+= _T(",\n");
				else
					sMsg+= _T(",\t");
			}
			sMsg+= saDuplicates.GetAt(i);
			sMsg+= _T(".\n");
		}
		sMsg+= _T("Please edit the file and remove one of the definitions.");
		MessageBox( NULL, sMsg, _T("Drawing Details already exist"), MB_OK );
	}

	File.Close();

	return true;
}


//////////////////////////////////////////////////////////////////////
// GetMatchingComponent
StockListElement *StockListArray::GetMatchingComponent(CString sPartNumber, bool bShowWarning/*=true*/ )
{
	int					i;
	CString				sPN;
	StockListElement	*pElement;
//	CString sMsg, sText;

	sPartNumber.MakeUpper();

	i=0;
	while( i<GetSize() )
	{
		pElement = GetAt(i);
		;//assert( pElement!=NULL );
		if( pElement!=NULL && pElement->GetPartNumber()==sPartNumber )
		{
			return pElement;
		}
		i++;
	}
/*
	if( bShowWarning )
	{
		//We could not locate the component in the component details file,
		//	so give an error message
		sMsg = "The Components Details file does not contain valid\n";
		sMsg+= "information for the following component:\n";
		sText.Format( "Component Type: %s\nCommon Length: %0.2f\nMaterial Type: %s\n",
						GetComponentDescStr(eComponentType),
						dCommonLength,
						GetComponentMaterialStr( eMaterial ) );
		sMsg+= sText;
		sMsg+= "Please restart autocad to locate the valid Component Details File!";
		MessageBox( NULL, sMsg, "Error in Component Details", MB_OK );
		ClearFilenameInRegistry(true);
	}
*/
	return NULL;
}

void StockListArray::ClearFilenameInRegistry( bool bStockFile )
{
	SetFilenameInRegistry( _T(""), bStockFile? TRUE: FALSE );
}

void StockListArray::RemoveAll()
{
	StockListElement	   *pEle;
	
	while( GetSize()>0 )
	{
		pEle = NULL;
		pEle = GetAt(0);
		if( pEle!=NULL )
		{
			delete pEle;
		}
		else
		{
			;//assert( false );
		}

		RemoveAt(0);
	}
}

//GetPartNumber
CString StockListElement::GetPartNumber()
{
	return m_sPartNumber;
}//GetPartNumber

//GetDescription
CString StockListElement::GetDescription()
{
	return m_sDescription;
}//GetDescription

//GetHire
double	StockListElement::GetHire()
{
	return m_dHire;
}//GetHire

//GetSale
double  StockListElement::GetSale()
{
	return m_dSale;
}//GetSale

//GetWeight
double  StockListElement::GetWeight()
{
	return m_dWeight;
}//GetWeight

//GetLength
double	StockListElement::GetLength()
{
	return m_dX;
}//GetLength

//GetWidth
double	StockListElement::GetWidth()
{
	return m_dY;
}//GetWidth

//GetHeight
double	StockListElement::GetHeight()
{
	return m_dZ;
}//GetHeight

//GetFilename2DEO
CString StockListElement::GetFilename2DEO()
{
	return m_sFilename2DEO;
}//GetFilename2DEO

//GetFilename2D
CString StockListElement::GetFilename2D()
{
	return m_sFilename2D;
}//GetFilename2D

//GetFilename3D
CString	StockListElement::GetFilename3D()
{
	return m_sFilename3D;
}//GetFilename3D


//SetPartNumber
void StockListElement::SetPartNumber( CString sPartNumber )
{
	sPartNumber.MakeUpper();
	m_sPartNumber = sPartNumber;
}

//SetDescription
void StockListElement::SetDescription( CString sDesc )
{
	m_sDescription = sDesc;
}

//SetHire
void StockListElement::SetHire(	double dHire )
{
	m_dHire = dHire;
}


//SetSale
void StockListElement::SetSale(	double  dSale )
{
	m_dSale = dSale;
}


//SetWeight
void StockListElement::SetWeight( double dWeight )
{
	m_dWeight = dWeight;
}


//SetLength
void StockListElement::SetLength( double dLength )
{
	m_dX = dLength;
}


//SetWidth
void StockListElement::SetWidth( double dWidth )
{
	m_dY = dWidth;
}


//SetHeight
void StockListElement::SetHeight( double dHeight )
{
	m_dZ = dHeight;
}

//SetFilename2DEO
void StockListElement::SetFilename2DEO(	CString sFilename )
{
	m_sFilename2DEO = sFilename;
}

//SetFilename2D
void StockListElement::SetFilename2D( CString sFilename )
{
	m_sFilename2D = sFilename;
}

//SetFilename3D
void StockListElement::SetFilename3D( CString sFilename )
{
	m_sFilename3D = sFilename;
}

//GetDescription
bool StockListArray::GetDescription(CString sPartNumber, CString &sDesc)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	sDesc = pEle->GetDescription();
	return true;
}

//GetHire
bool StockListArray::GetHire(	CString sPartNumber, double &dHire)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dHire = pEle->GetHire();
	return true;
}	//GetHire

//GetSale
bool StockListArray::GetSale(	CString sPartNumber, double &dSale)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dSale = pEle->GetSale();
	return true;
}	//GetSale

//GetWeight
bool StockListArray::GetWeight(	CString sPartNumber, double &dWeight)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dWeight = pEle->GetWeight();
	return true;
}	//GetWeight

//GetLength
bool StockListArray::GetLength(	CString sPartNumber, double &dLenght)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dLenght = pEle->GetLength();
	return true;
}	//GetLength

//GetWidth
bool StockListArray::GetWidth(	CString sPartNumber, double &dWidth)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dWidth = pEle->GetWidth();
	return true;
}	//GetWidth

//GetHeight
bool StockListArray::GetHeight(	CString sPartNumber, double &dHeight)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	dHeight = pEle->GetHeight();
	return true;
}	//GetHeight

//GetFilename3D
bool StockListArray::GetFilename3D(	CString sPartNumber, CString &sFilename)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	sFilename = pEle->GetFilename3D();
	return true;
}	//GetFilename3D

//GetFilename2D
bool StockListArray::GetFilename2D(	CString sPartNumber, CString &sFilename)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	sFilename = pEle->GetFilename2D();
	return true;
}	//GetFilename2D

//GetFilename2DEO
bool StockListArray::GetFilename2DEO(CString sPartNumber, CString &sFilename)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	sFilename = pEle->GetFilename2DEO();
	return true;
}	//GetFilename2DEO


bool StockListArray::GetUseSalePrice(CString sPartNumber, bool &bUseSale)
{
	StockListElement *pEle;
	pEle = GetMatchingComponent( sPartNumber );
	if( pEle==NULL )
		return false;
	bUseSale = pEle->UseSalePrice();
	if( bUseSale )
	{
		;//assert( pEle->GetHire()>ROUND_ERROR );
		;//assert( pEle->GetSale()>ROUND_ERROR );
	}
	return true;
}


Entity * StockListElement::GetClonedEntity(ViewTypeEnum eVT)
{
	Entity					*pEntity;
	AcDbIdPair				idPair;
    AcDbObjectId			modelSpaceId, Id;
    AcDbIdMapping			idMap;
    AcDbBlockTable			*pBlockTable;
	Acad::ErrorStatus		es;
    AcDbObjectIdArray		objList;
	const AcDb::OpenMode	modeWrite = AcDb::kForWrite;

	pEntity = NULL;
    
	//Try to load the entity
	if( GetEntity(eVT)==NULL )
	{
		//It wouldn't load so we need to load it if possible

		//////////////////////////////////////////////////////////
		//find the path for the files

		CString sPath = GetEntPathInRegistry(P_ENTITY);
		if( sPath.IsEmpty() )
		{
			sPath = MOST_LIKELY_PATH;
		}

		if( sPath[sPath.GetLength()-1]!=_T('\\') )
		{
			//path must end in a '\'
			sPath+= _T("\\");
		}

		if( !LoadEntity( sPath, eVT ) )
			return NULL;

		//We have successfully loaded the entity!
		SetEntPathInRegistry(sPath, P_ENTITY);
	}

	if( GetEntity(eVT)!=NULL )
	{
		;//assert( objList.length()==0 );
		Id = GetEntity(eVT)->objectId();
		objList.append( Id );

		es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
		if( es==Acad::eOk )
		{
			es = pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
			if( es==Acad::eOk )
			{
				es = GetEntity(eVT)->close();
				if( es==Acad::eOk )
				{
					es = acdbHostApplicationServices()->workingDatabase()
						->deepCloneObjects(objList, modelSpaceId, idMap);
					if( es==Acad::eOk )
					{
						AcDbIdMappingIter iter(idMap);
						for( iter.start(); !iter.done(); iter.next() )
						{
							if( !iter.getMap(idPair) )
								continue;

							if(!idPair.isCloned())
								continue;
						}
						es = acdbOpenAcDbEntity((AcDbEntity*&)pEntity, idPair.value(), modeWrite );
						if( es!=Acad::eOk )
						{
							;//assert( false );
							return NULL;
						}

						pEntity->setVisibility( AcDb::kVisible );

						es = pEntity->close();
						if( es!=Acad::eOk )
						{
							;//assert( false );
							return NULL;
						}
					}
				}
			}
		}
		es = pBlockTable->close();
		if( es!=Acad::eOk )
		{
			;//assert( false );
			return NULL;
		}

		es = GetEntity(eVT)->close();
		if( es!=Acad::eOk )
		{
			;//assert( false );
			return NULL;
		}
	}
	return pEntity;
}

Entity * StockListElement::GetEntity(ViewTypeEnum eVT)
{
	switch( eVT )
	{
	case( VT_3D ):
		return m_pEntity3D;
	case( VT_2D ):
		return m_pEntity2D;
	case( VT_2DEO ):
		return m_pEntity2DEO;
	default:
		;//assert( false );
	}
	return NULL;
}

bool StockListElement::LoadEntity(CString &sPath, ViewTypeEnum eVT)
{
	int								i, iSize, iPos;
	TCHAR							strFileName[255];
	bool							bFound, bUsePath;
	Entity							*pEntity;
	CString							sFilename, sMessage;
	Matrix3D						Trans;
	AcDbDatabase					*pFileDb, *pWorkingDb;
	AcDbObjectId					entityId;
	AcDbBlockTable					*pBlockTable;
	Acad::ErrorStatus				es;
	AcDbBlockTableRecord			*pSpaceRecordModel, *pSpaceRecordPaper;
	const AcDb::OpenMode			modeWrite = AcDb::kForWrite,
									modeRead = AcDb::kForRead;
	AcDbBlockTableRecordIterator	*pIterator;

	/////////////////////////////////////////////////////////////////
	//we are about to create a new entity so any existing one should be
	//	removed by now
	if( GetEntity( eVT )!=NULL )
	{
		//This has already been loaded
		return true;
	}

	/////////////////////////////////////////////////////////////////
	// Set constructor parameter to kFalse so that the
	// database will be constructed empty.  This way only
	// what is read in will be in the database.
	pFileDb = new AcDbDatabase(Adesk::kFalse);
	pWorkingDb = acdbHostApplicationServices()->workingDatabase();

	/////////////////////////////////////////////////////////////////
	// The AcDbDatabase::readDwgFile() function
	// automatically appends a DWG extension if it is not
	// specified in the filename parameter.
	// lets just test this with Solid01.dwg
	switch( eVT )
	{
	case( VT_3D ):
		sFilename = m_sFilename3D;
		break;
	case( VT_2D ):
		sFilename = m_sFilename2D;
		break;
	case( VT_2DEO ):
		sFilename = m_sFilename2DEO;
		break;
	default:
		;//assert( false );
	}

	if( sFilename.GetLength()<=0 )
	{
		delete pFileDb;
		return true;
	}

	bUsePath = false;
	if( sFilename.Find( _T("\\") )<0 )
	{
		//Filename does not include a path,
		//	so add the path to the name
		sFilename = sPath + sFilename;
		bUsePath = true;
	}
	else
	{
		//The filename does include a path,
		//	so include it
	}

	
	iSize = sFilename.GetLength();

	if( iSize<=0 )
	{
		delete pFileDb;
		return false;
	}

	for( i=0; i<iSize; i++ )
	{
		strFileName[i] = sFilename[i];
	}
	strFileName[i] = _T('\0');

	es = pFileDb->readDwgFile( strFileName );
	if( es!=Acad::eOk )
	{
		if( es==Acad::eFileNotFound )
		{
			CFileDialog dbox(TRUE, EXTENTION_LOAD_3D_ENTITY_FILE, sFilename,
				OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_LOAD_3D_ENTITY_FILE );
			dbox.m_ofn.lpstrTitle = TITLE_LOAD_3D_COMPONENT_ENTITY_FILE;
			dbox.m_ofn.Flags = OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
			if(dbox.DoModal()!=IDOK)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				delete pFileDb;
				return false;
			}

			sFilename = dbox.GetPathName();

			if( bUsePath )
			{
				///////////////////////////////////////////////////
				//find the last '\'
				bFound=false;
				iPos = sFilename.GetLength()-1;
				while( iPos>=0 )
				{
					if( sFilename[iPos]==_T('\\') )
					{
						bFound = true;
						break;
					}
					iPos--;
				}
				if( !bFound )
				{
					MessageBeep(MB_ICONEXCLAMATION);
					sMessage = _T("Invalid Path and Filename");
					MessageBox( NULL, sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
					delete pFileDb;
					return true;
				}

				//seperate the path and the filename
				sPath = sFilename.Left( iPos+1 );

				switch( eVT )
				{
				case( VT_3D ):
					m_sFilename3D = sFilename.Right( sFilename.GetLength()-(iPos+1) );
					break;
				case( VT_2D ):
					m_sFilename2D = sFilename.Right( sFilename.GetLength()-(iPos+1) );
					break;
				case( VT_2DEO ):
					m_sFilename2DEO = sFilename.Right( sFilename.GetLength()-(iPos+1) );
					break;
				default:
					;//assert( false );
				}
			}
			else
			{
				switch( eVT )
				{
				case( VT_3D ):
					m_sFilename3D = sFilename;
					break;
				case( VT_2D ):
					m_sFilename2D = sFilename;
					break;
				case( VT_2DEO ):
					m_sFilename2DEO = sFilename;
					break;
				default:
					;//assert( false );
				}
			}

			//////////////////////////////////////////////////////
			//try again using this new filename
			iSize = sFilename.GetLength();
			if( iSize<=0 )
			{
				acutPrintf(_T("\nFilename \"%s\" is invalid!"), sFilename );
				delete pFileDb;
				return true;
			}
			for( i=0; i<iSize; i++ )
			{
				strFileName[i] = sFilename[i];
			}
			strFileName[i] = _T('\0');
			es = pFileDb->readDwgFile( strFileName );
			if( es!=Acad::eOk )
			{
        acutPrintf(_T("\nCould not open Entity file: \"%s\" reason: %s"),
							sFilename, acadErrorStatusText( es ) );
				delete pFileDb;
				return true;
			}
		}
		else
		{
      acutPrintf(_T("\nCould not open Entity file: \"%s\" reason: %s"),
						sFilename, acadErrorStatusText( es ) );
			delete pFileDb;
			return true;
		}
	}

	//lets find out how many entities we have in the model space
	int iNumModelSpace, iNumPaperSpace;
	iNumModelSpace=iNumPaperSpace=0;

	if((es = pWorkingDb->getBlockTable(pBlockTable, modeRead))==Acad::eOk)
	{
		es = pBlockTable->getAt(ACDB_PAPER_SPACE, pSpaceRecordPaper, modeRead );
		if(es==Acad::eOk)
		{
			es = pSpaceRecordPaper->newIterator( pIterator );
			if( es==Acad::eOk )
			{
				while( !pIterator->done() )
				{
					iNumPaperSpace++;
					pIterator->step();
				}
				delete pIterator;
			}
		}
		es = pSpaceRecordPaper->close();
		;//assert( es==Acad::eOk );
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecordModel, modeRead );
		if(es==Acad::eOk)
		{
			es = pSpaceRecordModel->newIterator( pIterator );
			if( es==Acad::eOk )
			{
				while( !pIterator->done() )
				{
					iNumModelSpace++;
					pIterator->step();
				}
				delete pIterator;
			}
		}
		es = pSpaceRecordModel->close();
		;//assert( es==Acad::eOk );
	}
	//Close the block table
	es = pBlockTable->close();
	;//assert( es==Acad::eOk );

#ifndef	USE_INSERT_METHOD
	es = pFileDb->getBlockTable(pBlockTable, AcDb::kForRead);
	;//assert( es==Acad::eOk );
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite);
	;//assert( es==Acad::eOk );
	es = pBlockTable->close();
	;//assert( es==Acad::eOk );
	blockId = pSpaceRecord->objectId();
	es = pSpaceRecord->close();
	;//assert( es==Acad::eOk );
#endif	//#ifndef	USE_INSERT_METHOD

	AcDbObjectId blockId;
	AcGePoint3d basePoint;
	basePoint.set( 0.00, 0.00, 0.00 );

	Trans.setToIdentity();

	es = pWorkingDb->insert( Trans, pFileDb, Adesk::kFalse );
	;//assert( es==Acad::eOk );

#ifndef	USE_INSERT_METHOD
	blockId = addBlockWithAttributes( blockId, basePoint );
#endif	//#ifndef	USE_INSERT_METHOD

	/////////////////////////////////////////////////////////////////
	//Get a pointer to the current drawing
	//and get the drawing's block table.  Open it for read.
	if((es = pWorkingDb->getBlockTable(pBlockTable, modeRead))==Acad::eOk)
	{
		/////////////////////////////////////////////////////////////////
		//Then get the Model Space record and open it for write.  
		//This will be the owner of the new line.
		es = pBlockTable->getAt(ACDB_PAPER_SPACE, pSpaceRecordPaper, modeWrite);
		if(es==Acad::eOk)
		{
			/////////////////////////////////////////////////////////////////
			//Append to Model Space, then close it and the Model Space record.
			es = pSpaceRecordPaper->newIterator( pIterator );
			if( es==Acad::eOk )
			{
				/////////////////////////////////////////////////////////////////
				//find the last entity in the Database
				while( !pIterator->done() )
				{
					iNumPaperSpace--;
					pIterator->step();
				}
				//too far, go back one!
				pIterator->step( false );

				if( iNumPaperSpace<0 )
				{
					/////////////////////////////////////////////////////////////////
					//open the entity and store the pointer
					es = pIterator->getEntityId( entityId );
					if( es==Acad::eOk )
					{
						es=acdbOpenAcDbEntity((AcDbEntity*&)pEntity, entityId, modeWrite );
						if( es==Acad::eOk )
						{
							//;//assert( blockId == pEntity->objectId() );

							//Store the pointer
							SetEntity( pEntity, eVT );

							//we don't want to see the component details entities
							pEntity->setVisibility( AcDb::kInvisible );
						}
						es = pEntity->close();
						;//assert( es==Acad::eOk );
					}
				}

				delete pIterator;
			}
		}
		es = pSpaceRecordPaper->close();
		;//assert( es==Acad::eOk );

		/////////////////////////////////////////////////////////////////
		//Then get the Model Space record and open it for write.  
		//This will be the owner of the new line.
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecordModel, modeWrite);
		if(es==Acad::eOk)
		{
			/////////////////////////////////////////////////////////////////
			//Append to Model Space, then close it and the Model Space record.
			es = pSpaceRecordModel->newIterator( pIterator );
			if( es==Acad::eOk )
			{
				/////////////////////////////////////////////////////////////////
				//find the last entity in the Database
				while( !pIterator->done() )
				{
					iNumModelSpace--;
					pIterator->step();
				}
				//too far, go back one!
				pIterator->step( false );

				if( iNumModelSpace<0 )
				{
					/////////////////////////////////////////////////////////////////
					//open the entity and store the pointer
					es = pIterator->getEntityId( entityId );
					if( es==Acad::eOk )
					{
						es=acdbOpenAcDbEntity((AcDbEntity*&)pEntity, entityId, modeWrite );
						if( es==Acad::eOk )
						{
							//;//assert( blockId == pEntity->objectId() );

							//Store the pointer
							SetEntity( pEntity, eVT );

							//we don't want to see the component details entities
							pEntity->setVisibility( AcDb::kInvisible );
						}
						es = pEntity->close();
						;//assert( es==Acad::eOk );
					}
				}

				delete pIterator;
			}
		}
		es = pSpaceRecordModel->close();
		;//assert( es==Acad::eOk );
	}
	//Close the block table
	es = pBlockTable->close();
	;//assert( es==Acad::eOk );

	;//assert( pEntity->objectId()==entityId );

	delete pFileDb;

	return(GetEntity(eVT)!=NULL);
}

void StockListElement::SetEntity(Entity *pEntity, ViewTypeEnum eVT)
{
	switch( eVT )
	{
	case( VT_3D ):
		m_pEntity3D = pEntity;
		break;
	case( VT_2D ):
		m_pEntity2D = pEntity;
		break;
	case( VT_2DEO ):
		m_pEntity2DEO = pEntity;
		break;
	default:
		;//assert( false );
	}
}

void StockListElement::ClearActualComponents()
{
	DeleteEntity( VT_3D );
	DeleteEntity( VT_2D );
	DeleteEntity( VT_2DEO );
}

void StockListElement::DeleteEntity(ViewTypeEnum eVT)
{
	Acad::ErrorStatus	es;

	switch( eVT )
	{
	case( VT_3D ):
		if( m_pEntity3D!=NULL )
		{
			//Delete the entity
			es = m_pEntity3D->open();
			if( es==Acad::eOk )
			{
				es = m_pEntity3D->erase();
				;//assert( es==Acad::eOk );
			}
			else if( es==Acad::eWasErased )
			{
				//fine
				1;
			}
			else
			{
				;//assert( false );
			}
			es = m_pEntity3D->close();
			;//assert( es==Acad::eOk );
		}
		break;

	/////////////////////////////////////////////////////////////
	case( VT_2D ):
		if( m_pEntity2D!=NULL )
		{
			//Delete the entity
			es = m_pEntity2D->open();
			if( es==Acad::eOk )
			{
				es = m_pEntity2D->erase();
				;//assert( es==Acad::eOk );
			}
			else if( es==Acad::eWasErased )
			{
				//fine
				1;
			}
			else
			{
				;//assert( false );
			}
			es = m_pEntity2D->close();
			;//assert( es==Acad::eOk );
		}
		break;

	/////////////////////////////////////////////////////////////
	case( VT_2DEO ):
		if( m_pEntity2DEO!=NULL )
		{
			//Delete the entity
			es = m_pEntity2DEO->open();
			if( es==Acad::eOk )
			{
				es = m_pEntity2DEO->erase();
				;//assert( es==Acad::eOk );
			}
			else if( es==Acad::eWasErased )
			{
				//fine
				1;
			}
			else
			{
				;//assert( false );
			}
			es = m_pEntity2DEO->close();
			;//assert( es==Acad::eOk );
		}
		break;
	default:
		;//assert( false );
	}

	SetEntity( NULL, eVT );
}

AcDbObjectId StockListElement::addBlockWithAttributes(AcDbObjectId blockId, AcGePoint3d basePoint)
{
	AcDbObjectId	retId;
	Acad::ErrorStatus es;

	/////////////////////////////////////////////
	//Step 1: Allocate a block reference object
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;

	/////////////////////////////////////////////
	//Step 2: Set up the block reference to the newly
	//created block definition
	es = pBlkRef->setBlockTableRecord(blockId);
	;//assert( es==Acad::eOk );

	//Give it the current UCS normal.
	es = pBlkRef->setPosition(basePoint);
	;//assert( es==Acad::eOk );
	es = pBlkRef->setRotation(0.0);
	;//assert( es==Acad::eOk );
	es = pBlkRef->setNormal(AcGeVector3d(0.0, 0.0, 1.0));
	;//assert( es==Acad::eOk );

	/////////////////////////////////////////////
	//Step 3: Open current database's Model Space
	//blockTableRecord
	AcDbBlockTable			*pBlockTable;
	AcDbBlockTableRecord	*pBlockTableRecord;
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	;//assert( es==Acad::eOk );
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	;//assert( es==Acad::eOk );
	es = pBlockTable->close();
	;//assert( es==Acad::eOk );

	//Append the block reference to the model space
	//block table record
	AcDbObjectId newEntId;
	es = pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
	;//assert( es==Acad::eOk );
	es = pBlockTableRecord->close();
	;//assert( es==Acad::eOk );

	/////////////////////////////////////////////
	//Step 4: Open the block definition for read
	AcDbEntity						*pEnt;
	AcDbBlockTableRecord			*pBlockDef;
	AcDbAttributeDefinition			*pAttdef;
	AcDbBlockTableRecordIterator	*pIterator;

	es = acdbOpenObject(pBlockDef, blockId, AcDb::kForRead);
	;//assert( es==Acad::eOk );
	es = pBlockDef->newIterator(pIterator);
	;//assert( es==Acad::eOk );
	retId.setNull();
	for( pIterator->start(); !pIterator->done(); pIterator->step() )
	{
		//Get the next entity
		es = pIterator->getEntity(pEnt, AcDb::kForRead);
		;//assert( es==Acad::eOk );

		retId = pEnt->objectId();

		//Make sure the entity is an attribute definition
		//and not a constant
		pAttdef =AcDbAttributeDefinition::cast(pEnt);
		if( pAttdef!=NULL && !pAttdef->isConstant() )
		{
			//We have a non-constant attribute definition
			//so build an attribute entity
			AcDbAttribute *pAtt = new AcDbAttribute;
			es = pAtt->setPropertiesFrom(pAttdef);
			;//assert( es==Acad::eOk );
			es = pAtt->setInvisible(pAttdef->isInvisible());
			;//assert( es==Acad::eOk );

			//Translate attribute by block reference.
			//To be really correct, entire block
			//reference transform should be applied here.
			basePoint =pAttdef->position();
			basePoint +=pBlkRef->position().asVector();
			es = pAtt->setPosition(basePoint);
			;//assert( es==Acad::eOk );
			es = pAtt->setHeight(pAttdef->height());
			;//assert( es==Acad::eOk );
			es = pAtt->setRotation(pAttdef->rotation());
			;//assert( es==Acad::eOk );
			es = pAtt->setTag(_T("Tag"));
			;//assert( es==Acad::eOk );
			es = pAtt->setFieldLength(25);
			;//assert( es==Acad::eOk );
			TCHAR *pStr =pAttdef->tag();
			es = pAtt->setTag(pStr);
			;//assert( es==Acad::eOk );
			delete pStr;
			es = pAtt->setFieldLength(pAttdef->fieldLength());
			;//assert( es==Acad::eOk );

			//Database Column value should be displayed
			//INSERT would prompt for this...
			es = pAtt->setTextString(_T("XXX"));
			;//assert( es==Acad::eOk );

			//Set Alignments
			es = pAtt->setHorizontalMode( pAttdef->horizontalMode());
			;//assert( es==Acad::eOk );
			es = pAtt->setVerticalMode(pAttdef->verticalMode());
			;//assert( es==Acad::eOk );
			es = pAtt->setAlignmentPoint( pAttdef->alignmentPoint()
								+ pBlkRef->position().asVector() );
			;//assert( es==Acad::eOk );

			//Insert the attribute in the DWG
			AcDbObjectId attId;
			es = pBlkRef->appendAttribute(attId, pAtt);
			;//assert( es==Acad::eOk );
			es = pAtt->close();
			;//assert( es==Acad::eOk );
		}

		//Use pEnt... pAttdef might be NULL
		es = pEnt->close();
		;//assert( es==Acad::eOk );
	}
	delete pIterator;
	es = pBlockDef->close();
	;//assert( es==Acad::eOk );
	es = pBlkRef->close();
	;//assert( es==Acad::eOk );

	return retId;
}

void StockListArray::TraceContents( bool bShowList )
{
	int			i;
	StockListElement	*pEle;

	if( bShowList )
	{
		acutPrintf(_T("\nvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv") );
		acutPrintf(_T("\nStock List Array contents") );

    acutPrintf( _T("\nElement#, Part#, Desc, Hire, Sale, Weight, Length, Width, Height, 3D, 2D, 2D end on") );
		for( i=0; i<GetSize(); i++ )
		{
			acutPrintf( _T("\n%i, "), i+1 );

			pEle = GetAt(i);
			pEle->TraceContents(NULL);
		}
		acutPrintf(_T("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") );
	}
	else
	{
		UINT		uiMode;
		CString		sText, sFilename;
		CStdioFile	File;
		CFileDialog	*pDialog;

		uiMode = CFile::modeWrite|CFile::modeCreate|CFile::typeText;

		//try the default filename
		sFilename.Empty();
		while( !File.Open( sFilename, uiMode ) )
		{
			///////////////////////////////////////////////////
			//Get the file name to use from the user
			pDialog = new CFileDialog(TRUE, _T(".csv"), sFilename,
							OFN_HIDEREADONLY, _T("Comma Seperated File(*.csv)|*.csv|All Files(*.*)|*.*||") );
			pDialog->m_ofn.lpstrTitle = _T("Stock List Data");
			//int iRet;
			INT_PTR iRet;
			gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
			iRet = pDialog->DoModal();
			gbOpenFileDialogActive = false;            
			if (iRet==IDCANCEL)
			{
				return;
			}
			else if( iRet!=IDOK )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sFilename.Empty();
			}
			else
			{
				sFilename = pDialog->GetPathName();
			}
		}
		sText = _T("\nStock List Array contents\n");
		File.WriteString( sText );

    sText = _T("\nElement#, Part#, Desc, Hire, Sale, Weight, Length, Width, Height, 3D, 2D, 2D end on");
		File.WriteString( sText );
		for( i=0; i<GetSize(); i++ )
		{
			sText.Format( _T("\n%i, "), i+1 );
			File.WriteString( sText );

			sText.Empty();
			pEle = GetAt(i);
			pEle->TraceContents( &sText );
			File.WriteString( sText );
		}
		File.Close();
	}
}

void StockListElement::TraceContents(CString *pText/*=NULL*/)
{
	if( pText==NULL )
	{
		acutPrintf( _T("%s, %s, $%0.2f, $%0.2f, %0.1fkg, %0.0fmm, %0.0fmm, %0.0fmm, %s, %s, %s"),
					GetPartNumber(), GetDescription(), GetHire(), GetSale(),
					GetWeight(), GetLength(), GetWidth(), GetHeight(),
					GetFilename2DEO(), GetFilename2D(), GetFilename3D() );
	}
	else
	{
		pText->Format( _T("%s, %s, $%0.2f, $%0.2f, %0.1fkg, %0.0fmm, %0.0fmm, %0.0fmm, %s, %s, %s"),
					GetPartNumber(), GetDescription(), GetHire(), GetSale(),
					GetWeight(), GetLength(), GetWidth(), GetHeight(),
					GetFilename2DEO(), GetFilename2D(), GetFilename3D() );
	}
}

int StockListArray::EditStockElement()
{
	//int						iReturn;
	INT_PTR						iReturn;
	KwikScafComponentEditor	*pEditor;

	pEditor = new KwikScafComponentEditor( this, m_sLastPartNumber );

	iReturn = pEditor->DoModal();
	if( iReturn==IDOK && pEditor->GetCurrentElement()!=NULL )
	{
		m_sLastPartNumber = pEditor->GetCurrentElement()->GetPartNumber();
	}

	return iReturn;
}

void StockListElement::SetUseSalePrice(bool bUseSale)
{
	m_bUseSalePrice = bUseSale;
}

bool StockListElement::UseSalePrice()
{
	return m_bUseSalePrice;
}

void StockListArray::Serialize(CArchive &ar)
{
	int					i, iSize;
	CString				sMsg, sTemp;
	VersionNumber		uiVersion;
	StockListElement	*pElement, *pSLE;	

	if (ar.IsStoring())    // Store Object?
	{
		ar << STOCK_ARRAY_VERSION_LATEST;

		//STOCK_ARRAY_VERSION_1_0_1
		iSize = GetNumberOfEditedSSC();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pElement = GetUserEditedSSC(i);
			pElement->Serialize(ar);
		}

		//STOCK_ARRAY_VERSION_1_0_0
		ar << m_sLastPartNumber;

		iSize = GetNumberOfUserCreatedElements();
		ar << iSize;
		for( i=0; i<iSize; i++ )
		{
			pElement = GetUserCreatedElement(i);
			pElement->Serialize(ar);
		}
	}
	else
	{
		ar >> uiVersion;
		switch (uiVersion)
		{
		case STOCK_ARRAY_VERSION_1_0_1 :

			//Load the edited sole, shade, chain components
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				pElement = new StockListElement();
				pElement->Serialize(ar);
				pSLE = GetMatchingComponent( pElement->GetPartNumber() );
				if( pSLE!=NULL )
				{
					//This element is an edited Soleboard, ShadeCloth or ChainMesh

					if( pSLE->GetSale()>pElement->GetSale()+ROUND_ERROR ||
						pSLE->GetSale()<pElement->GetSale()-ROUND_ERROR ||
						pSLE->UseSalePrice()!=pElement->UseSalePrice() )
					{
						pSLE->SetSSCEdited( true );
					}
					else
					{
						//same as the existing one, so it is as good as never
						//	have been edited
						pSLE->SetSSCEdited( false );
					}
					//adjust the existing element's sale price
					pSLE->SetSale( pElement->GetSale() );
					pSLE->SetUseSalePrice( pElement->UseSalePrice() );

					//and discard the element we just loaded
					delete pElement;
					pElement = NULL;
				}
				else
				{
					//What is this component, why did we store it?
					;//assert( false );
					Add( pElement );
				}
			}

		case STOCK_ARRAY_VERSION_1_0_0 :
			ar >> m_sLastPartNumber;
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				pElement = new StockListElement();
				pElement->Serialize(ar);
				Add( pElement );
			}
			break;
		default:
			;//assert( false );
			if( uiVersion>STOCK_ARRAY_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: StockListArray.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), STOCK_ARRAY_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

void StockListElement::Serialize(CArchive &ar)
{
	bool					bDummy;
	BOOL					BTemp;
	double					dDummy;
	CString					sDummy, sMsg, sTemp;
	VersionNumber			uiVersion;

	if (ar.IsStoring())    // Store Object?
	{
		ar << STOCK_ELEMENT_VERSION_LATEST;

		//STOCK_ELEMENT_VERSION_1_0_1
		bDummy = IsEditedSSC();
		STORE_bool_IN_AR(bDummy);
		if( bDummy )
		{
			//store the version that this is overwriting
			ar << gpController->GetStockList()->m_iVSLMajor;
			ar << gpController->GetStockList()->m_iVSLMinor;
			ar << gpController->GetStockList()->m_iVSLBuild;
		}

		//STOCK_ELEMENT_VERSION_1_0_0
		bDummy = UseSalePrice();
		STORE_bool_IN_AR(bDummy);
		bDummy = GetUserCreatedElement();
		STORE_bool_IN_AR(bDummy);
		sDummy = GetPartNumber();
		ar << sDummy;
		sDummy = GetDescription();
		ar << sDummy;
		dDummy = GetHire();
		ar << dDummy;
		dDummy = GetSale();
		ar << dDummy;
		dDummy = GetWeight();
		ar << dDummy;
		dDummy = GetLength();
		ar << dDummy;
		dDummy = GetWidth();
		ar << dDummy;
		dDummy = GetHeight();
		ar << dDummy;
		sDummy = GetFilename2DEO();
		ar << sDummy;
		sDummy = GetFilename2D();
		ar << sDummy;
		sDummy = GetFilename3D();
		ar << sDummy;
	}
	else
	{
		int						iVMajor, iVMinor, iVBuild;
		CString					sMsg;
		StockListElement		*pElement;	
		StockListArray			*pSLA;
		pSLA = gpController->GetStockList();

		ar >> uiVersion;
		switch (uiVersion)
		{
		case STOCK_ELEMENT_VERSION_1_0_1 :
			LOAD_bool_IN_AR(bDummy);
			if( bDummy )
			{
				//store the version that this is overwriting
				ar >> iVMajor;
				ar >> iVMinor;
				ar >> iVBuild;
			}
			//fallthrough

		case STOCK_ELEMENT_VERSION_1_0_0 :
			LOAD_bool_IN_AR(bDummy);
			SetUseSalePrice(bDummy);
			LOAD_bool_IN_AR(bDummy);
			SetUserCreatedElement(bDummy);
			ar >> sDummy;
			SetPartNumber(sDummy);
			ar >> sDummy;
			SetDescription(sDummy);
			ar >> dDummy;
			SetHire(dDummy);
			ar >> dDummy;
			SetSale(dDummy);
			ar >> dDummy;
			SetWeight(dDummy);
			ar >> dDummy;
			SetLength(dDummy);
			ar >> dDummy;
			SetWidth(dDummy);
			ar >> dDummy;
			SetHeight(dDummy);
			ar >> sDummy;
			SetFilename2DEO(sDummy);
			ar >> sDummy;
			SetFilename2D(sDummy);
			ar >> sDummy;
			SetFilename3D(sDummy);

			if( pSLA->m_iVSLMajor>iVMajor || (pSLA->m_iVSLMinor>iVMinor && pSLA->m_iVSLMajor==iVMajor) ||
				( pSLA->m_iVSLBuild>iVBuild && pSLA->m_iVSLMinor==iVMinor && pSLA->m_iVSLMajor==iVMajor))
			{
				//The version number of the stock.csv file has increased since this was stored
				pElement = gpController->GetStockList()->GetMatchingComponent( GetPartNumber() );
				if( IsEditedSSC() && pElement!=NULL && pElement->GetSale()!=GetSale() )
				{
					//This product code already exists in the stock file, and
					//	in the Component Details file(because of IsEditedSSC),
					//	we also know that it is Chain Shade or soleboard, and
					//	that the sale price has changed during the Stock.csv
					//	file update, we should ask if they want to keep their
					//	old price.
					sMsg = _T("Since this file was last saved, the Stock.csv file has\n");
					sTemp.Format( _T("been updated from version %i.%i.%i to %i.%i.%i and the sale price\n"),
							iVMajor, iVMinor, iVBuild, pSLA->m_iVSLMajor, pSLA->m_iVSLMinor, pSLA->m_iVSLBuild );
					sMsg+= sTemp;
					sTemp.Format( _T("of Product Code %s (%s) has changed from $%1.02f to $%1.02f."),
								GetPartNumber(), GetDescription(), GetSale(), pElement->GetSale() );
					sMsg+= sTemp;
					sTemp.Format( _T("\n\nWould you like to keep the edited price ($%1.02f)?"), GetSale() );
					sMsg+= sTemp;

					if( MessageBox( NULL, sMsg, _T("Stock File updated"), MB_YESNO )==IDNO )
					{
						SetUseSalePrice( pElement->UseSalePrice() );
						SetSale( pElement->GetSale() );
					}
				}
			}
			break;
		default:
			;//assert( false );
			if( uiVersion>STOCK_ELEMENT_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: StockListElement.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), STOCK_ELEMENT_VERSION_LATEST, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

bool StockListElement::Export( CStdioFile &file )
{
	CString sLine;

	sLine = EXPORT_STOCK_PREFIX_TITLE;
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_VERSION, STOCK_ELEMENT_VERSION_LATEST );
	file.WriteString( sLine+_T("\n") );

	//STOCK_ELEMENT_VERSION_1_0_1
	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_EDITED_SSC, (int)IsEditedSSC() );
	file.WriteString( sLine+_T("\n") );

	//store the version that this is overwriting
	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_VSL_MAJOR, gpController->GetStockList()->m_iVSLMajor );
	file.WriteString( sLine+_T("\n") );
	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_VSL_MINOR, gpController->GetStockList()->m_iVSLMinor );
	file.WriteString( sLine+_T("\n") );
	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_VSL_BUILD, gpController->GetStockList()->m_iVSLBuild );
	file.WriteString( sLine+_T("\n") );

	//STOCK_ELEMENT_VERSION_1_0_0
	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_USE_SALE, (int)UseSalePrice() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%i"), EXPORT_STOCK_PREFIX_USER_CREATED, (int)GetUserCreatedElement() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%s"), EXPORT_STOCK_PREFIX_PART_NUMBER, GetPartNumber() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%s"), EXPORT_STOCK_PREFIX_DESCRIPTION, GetDescription() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%0.2f"), EXPORT_STOCK_PREFIX_HIRE, GetHire() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%0.2f"), EXPORT_STOCK_PREFIX_SALE, GetSale() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%0.1f"), EXPORT_STOCK_PREFIX_WEIGHT, GetWeight() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%f"), EXPORT_STOCK_PREFIX_LENGTH, GetLength() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%f"), EXPORT_STOCK_PREFIX_WIDTH, GetWidth() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%f"), EXPORT_STOCK_PREFIX_HEIGHT, GetHeight() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%s"), EXPORT_STOCK_PREFIX_FILENAME_2DEO, GetFilename2DEO() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%s"), EXPORT_STOCK_PREFIX_FILENAME_2D, GetFilename2D() );
	file.WriteString( sLine+_T("\n") );

	sLine.Format( _T("%s%s"), EXPORT_STOCK_PREFIX_FILENAME_3D, GetFilename3D() );
	file.WriteString( sLine+_T("\n") );

	if( !GetFilename2DEO().IsEmpty() ||
		!GetFilename2D().IsEmpty() ||
		!GetFilename3D().IsEmpty() )
	{
		CString sMsg, sTemp;

		sMsg.Format( _T("The folowing files are used by this component (%s)\n"), GetPartNumber() );
		sMsg+=       _T("to draw itself when using actual components.  If the Actual compoents\n");
		sMsg+=       _T("are ESSENTIAL then you will need to ensure these files are available.\n");
		sMsg+=       _T("If these files are not available for any reason then KwikScaf will\n");
		sMsg+=       _T("default to the wireframe model.\n\n");

		if( !GetFilename3D().IsEmpty() )
		{
      sTemp.Format( _T("3D Filename: %s"), GetFilename3D() );
			sMsg+= sTemp;
		}
		
		if( !GetFilename2D().IsEmpty() )
		{
      sTemp.Format( _T("2D Filename: %s"), GetFilename2D() );
			sMsg+= sTemp;
		}

		if( !GetFilename2DEO().IsEmpty() )
		{
      sTemp.Format( _T("2D End on Filename: %s"), GetFilename2DEO() );
			sMsg+= sTemp;
		}

		if( MessageBox( NULL, sMsg, _T("Warning Actual Components used"), MB_OKCANCEL )==IDCANCEL )
		{
			return false;
		}
	}

	return true;
}

bool StockListElement::Import( CStdioFile &file )
{
	int					iValue, iLength, iMajor, iMinor, iBuild;
	bool				bMore;
	CString				sLine, sValue, sMsg, sTemp;
	StockListElement	sleTemp;

	do
	{
		bMore = file.ReadString( sLine )? true: false;
	}
	while( bMore && sLine!=EXPORT_STOCK_PREFIX_TITLE );

	if( !bMore )
	{
		//we are at the end of the file
		return false;
	}

	if( !file.ReadString( sLine ) )		return false;
	iLength = EXPORT_STOCK_PREFIX_VERSION.GetLength();
	if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_VERSION )	return false;
	sValue = sLine.Right( sLine.GetLength()-iLength );
	iValue = _ttoi( sValue );

	switch( iValue )
	{
	case( STOCK_ELEMENT_VERSION_1_0_1 ):
		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_EDITED_SSC.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_EDITED_SSC )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetSSCEdited( _ttoi(sValue)==0? false: true );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_VSL_MAJOR.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_VSL_MAJOR )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		iMajor = _ttoi(sValue);

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_VSL_MINOR.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_VSL_MINOR )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		iMinor = _ttoi(sValue);

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_VSL_BUILD.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_VSL_BUILD )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		iBuild = _ttoi(sValue);

		//STOCK_ELEMENT_VERSION_1_0_0
		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_USE_SALE.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_USE_SALE )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetUseSalePrice( _ttoi(sValue)==0? false: true );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_USER_CREATED.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_USER_CREATED )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetUserCreatedElement( _ttoi(sValue)==0? false: true );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_PART_NUMBER.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_PART_NUMBER )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetPartNumber( sValue );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_DESCRIPTION.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_DESCRIPTION )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetDescription( sValue );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_HIRE.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_HIRE )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetHire( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_SALE.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_SALE )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetSale( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_WEIGHT.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_WEIGHT )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetWeight( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_LENGTH.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_LENGTH )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetLength( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_WIDTH.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_WIDTH )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetWidth( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_HEIGHT.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_HEIGHT )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetHeight( _tstof(sValue) );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_FILENAME_2DEO.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_FILENAME_2DEO )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetFilename2DEO( sValue );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_FILENAME_2D.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_FILENAME_2D )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetFilename2D( sValue );

		if( !file.ReadString( sLine ) )	return false;
		iLength = EXPORT_STOCK_PREFIX_FILENAME_3D.GetLength();
		if( sLine.Left(iLength)!=EXPORT_STOCK_PREFIX_FILENAME_3D )	return false;
		sValue = sLine.Right( sLine.GetLength()-iLength );
		sleTemp.SetFilename3D( sValue );

		break;

	case( STOCK_ELEMENT_VERSION_1_0_0 ):

		//This was only started in STOCK_ELEMENT_VERSION_1_0_1, so this is not possible
		//fallthrough
	default:
		;//assert( false );
		if( iValue>STOCK_ELEMENT_VERSION_LATEST )
		{
			sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
			sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
			sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
		}
		else
		{
			sMsg = _T("An unidentified error has occured during loading of this file.\n");
			sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
		}
		sMsg+= _T("Details of error -\n");
    sMsg+= _T("Class: StockListElement Import.\n");
    sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), STOCK_ELEMENT_VERSION_LATEST, iValue );
		sMsg+= sTemp;
		MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
		return false;
	}

	//Great, the read worked, copy the data from the temp
	*this = sleTemp;
	return true;
}

bool StockListElement::GetUserCreatedElement()
{
	return m_bUserCreatedElement;
}

void StockListElement::SetUserCreatedElement(bool bUse)
{
	m_bUserCreatedElement = bUse;
}

int StockListArray::GetNumberOfUserCreatedElements()
{
	int					i, iSize, iCount;
	StockListElement	*pElement;	

	iCount = 0;
	iSize = GetSize();
	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		if( pElement->GetUserCreatedElement() )
			iCount++;
	}
	return iCount;
}

StockListElement *StockListArray::GetUserCreatedElement(int iIndex)
{
	int					i, iSize, iCount;
	StockListElement	*pElement;	

	iCount = 0;
	iSize = GetSize();

	if( iIndex<0 || iIndex>=iSize )
	{
		;//assert( false );
		return NULL;
	}

	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		if( pElement->GetUserCreatedElement() )
		{
			if( iCount==iIndex )
			{
				return pElement;
			}
			iCount++;
		}
	}
	return NULL;
}

int StockListArray::GetNumberOfEditedSSC()
{
	int						i, iSize, iCount;
	CString					sPartNumber;
	StockListElement		*pElement;	

	iCount = 0;
	iSize = GetSize();

	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		if( pElement->IsEditedSSC() )
		{
			iCount++;
		}
	}
	return iCount;
}

StockListElement * StockListArray::GetUserEditedSSC(int iIndex)
{
	int						i, iSize, iCount;
	CString					sPartNumber;
	StockListElement		*pElement;	

	iCount = 0;
	iSize = GetSize();

	if( iIndex<0 || iIndex>=iSize )
	{
		;//assert( false );
		return NULL;
	}

	for( i=0; i<iSize; i++ )
	{
		pElement = GetAt(i);
		if( pElement->IsEditedSSC() )
		{
			if( iCount==iIndex )
			{
				//Found it!
				return pElement;
			}
			//wrong one, try again
			iCount++;
		}
	}

	//Didn't find it!
	return NULL;
}

void StockListElement::SetSSCEdited( bool bEdited )
{
	m_bSSCEdited = bEdited;
}

bool StockListElement::IsEditedSSC()
{
	return m_bSSCEdited;
/*
	bool					bEditedSSC;
	CString					sPartNumber;
	ComponentDetailsElement *pCompEle;

	bEditedSSC = false;
	if( GetSale()>0.00 || UseSalePrice() )
	{
		//We have a sale price and use sale is ticked

		sPartNumber = GetPartNumber();
		pCompEle = gpController->GetCompDetails()->GetMatchingComponent( sPartNumber );
		if( pCompEle!=NULL )
		{
			//This element is not user created
			;//assert( !GetUserCreatedElement() );

			if( pCompEle->m_eCompType==CT_CHAIN_LINK ||
				pCompEle->m_eCompType==CT_SHADE_CLOTH ||
				pCompEle->m_eCompType==CT_SOLEBOARD )
			{
				bEditedSSC = true;
			}
		}
	}
	return bEditedSSC;
*/
}


StockListElement & StockListElement::operator =(const StockListElement &Original)
{
	m_bSSCEdited			= Original.m_bSSCEdited;
	m_bUserCreatedElement	= Original.m_bUserCreatedElement;
	m_bUseSalePrice			= Original.m_bUseSalePrice;
	m_sPartNumber			= Original.m_sPartNumber;
	m_sDescription			= Original.m_sDescription;
	m_dHire					= Original.m_dHire;
	m_dSale					= Original.m_dSale;
	m_dWeight				= Original.m_dWeight;
	m_dX					= Original.m_dX;
	m_dY					= Original.m_dY;
	m_dZ					= Original.m_dZ;
	m_sFilename2DEO			= Original.m_sFilename2DEO;
	m_sFilename2D			= Original.m_sFilename2D;
	m_sFilename3D			= Original.m_sFilename3D;

	m_pEntity3D		= Original.m_pEntity3D;
	m_pEntity2D		= Original.m_pEntity2D;
	m_pEntity2DEO	= Original.m_pEntity2DEO;

	return *this;
}

void StockListArray::ClearActualComponents()
{
	StockListElement	*pEle;

	int i;
	for( i=0; i<GetSize(); i++ )
	{
		pEle = GetAt(i);
		pEle->ClearActualComponents();
	}
}

