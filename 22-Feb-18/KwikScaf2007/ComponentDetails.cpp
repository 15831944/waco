// ComponentDetailsElement.cpp: implementation of the ComponentDetailsElement class.
//
//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means(graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "ComponentDetails.h"
#include "Controller.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern Controller *gpController;

extern bool gbOpenFileDialogActive;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              ComponentDetails Class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 20/12/00 3:19p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/COMPONENTDETAILS.CPP $
//	Version			: $Revision: 82 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

//This is here because these number will be changed regularly, and if they were
//	in Meccano definitions then we would be doing a full compile each time we
//	changed one of these numbers
const	int VERSION_MAJOR_0		= 0;
const	int VERSION_MAJOR_1		= 1;

const	int VERSION_MINOR_9		= 9;
const	int VERSION_MINOR_3		= 3;
const	int VERSION_MINOR_4		= 4;
const	int VERSION_MINOR_5		= 5;

const	int VERSION_BUILD_0		= 0;
const	int VERSION_BUILD_1		= 1;
const	int VERSION_BUILD_2		= 2;
const	int VERSION_BUILD_3		= 3;
const	int VERSION_BUILD_4		= 4;
const	int VERSION_BUILD_5		= 5;
const	int VERSION_BUILD_6		= 6;
const	int VERSION_BUILD_7		= 7;
const	int VERSION_BUILD_8		= 8;
const	int VERSION_BUILD_9		= 9;
const	int VERSION_BUILD_10	= 10;
const	int VERSION_BUILD_11	= 11;
const	int VERSION_BUILD_12	= 12;
const	int VERSION_BUILD_13	= 13;
const	int VERSION_BUILD_14	= 14;
const	int VERSION_BUILD_15	= 15;

const int VERSION_CURRENT_MAJOR = VERSION_MAJOR_1;
const int VERSION_CURRENT_MINOR = VERSION_MINOR_5;
const int VERSION_CURRENT_BUILD = VERSION_BUILD_4;

const CString COMPONENT_DETAILS_FILE_LABEL = _T("~Components Details File");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ComponentDetailsElement::ComponentDetailsElement(ComponentDetailsArray *pArrayPointer)
{
	m_eCompType		= CT_UNDEFINED;
	m_dCommonLength	= 0.00;
	m_eMaterialType	= MT_OTHER;

	////////////////////////////////////////////////////
	//description
	m_sMillsPartNumber.Empty();
	m_sKwikstagePartNumber.Empty();
	m_sOtherPartNumber.Empty();
}

ComponentDetailsElement::~ComponentDetailsElement()
{
}

ComponentDetailsArray::ComponentDetailsArray()
{
	SetSize(0);
}

ComponentDetailsArray::~ComponentDetailsArray()
{
	RemoveAll();
}

bool ComponentDetailsArray::Load( Controller* pController )
{
	int						iComma, iVMajor, iVMinor, iVBuild;
	bool					bAdd;
	double					dCommonLength;
	CStdioFile				File;
	CString					sLine, sCompType, sCommonLength, sMaterial, sMsg,
							sMillsPartNumber, sKwikstagePartNumber, sOtherPartNumber, sText,
							sVersionMajor, sVersionMinor, sVersionBuild, sErrMsg;
	MaterialTypeEnum		eMaterial;
	ComponentTypeEnum		eCompType;
	ComponentDetailsElement	*pElement;

	///////////////////////////////////////////////////
	//clear the length table
	RemoveAll();

	if( pController!=NULL )
		gpController = pController;

	///////////////////////////////////////////////////
	//open the file
	if( !GetFile( File ) )
		return false;

	gbOpenFileDialogActive = true;
	;//assert( pController!=NULL );
	;//assert( pController->IsStockFileLoaded() );

/*	///////////////////////////////////////////////////
	//loading progress bar
	iProgRet = acedSetStatusBarProgressMeter( "Loading Component Details", 0, 100 );
	;//assert( iProgRet==0 );
	//do this calc here so it is not done 100's of times in the loop
	dProgSize = 100.00/(double)File.GetLength();
*/
	///////////////////////////////////////////////////
	//read the file into the table
	while( File.ReadString( sLine ) )
	{
/*		///////////////////////////////////////////////////
		//update progress bar
		iProgPos =(int)((double)File.GetPosition()*dProgSize);
		iProgRet = acedSetStatusBarProgressMeterPos( iProgPos );
*/
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
			if( sText!=COMPONENT_DETAILS_FILE_LABEL )
			{
				MessageBeep(MB_ICONEXCLAMATION);
				sErrMsg.Format( _T("The file, %s, is not a Components Details File"),
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
			iVMajor	=(ComponentTypeEnum)_ttoi( sVersionMajor );
			iVMinor	=(ComponentTypeEnum)_ttoi( sVersionMinor );
			iVBuild	=(ComponentTypeEnum)_ttoi( sVersionBuild );

			////////////////////////////////////////////////////////////
			//Is this the correct version?
			if( iVMajor	!=VERSION_CURRENT_MAJOR || 
				iVMinor	!=VERSION_CURRENT_MINOR ||
				iVBuild	!=VERSION_CURRENT_BUILD )
			{
				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Incorrect version number for the file: %s\nThis file is marked as version : %i.%i.%i\nHowever, this ARX requires version: %i.%i.%i"),
								File.GetFileName(), iVMajor, iVMinor, iVBuild,
								VERSION_CURRENT_MAJOR, VERSION_CURRENT_MINOR, VERSION_CURRENT_BUILD );
				MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				return false;
			}

			////////////////////////////////////////////////////////////
			//Fine - correct version
			continue;
		}


		///////////////////////////////////////////////////////////////
		//find the component type
		iComma			= sLine.Find( COMMA );
		if( iComma<1 )	continue;	//no comma
		sCompType		= sLine.Left( iComma );
		sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
		if( !_istdigit( sCompType[0] ) )
		{
			if(      sCompType == _T("DECKING PLANK") )			eCompType = CT_DECKING_PLANK;
			else if( sCompType == _T("STAGE BOARD") )			eCompType = CT_STAGE_BOARD;
			else if( sCompType == _T("LAPBOARD") )				eCompType = CT_LAPBOARD;
			else if( sCompType == _T("LEDGER") )				eCompType = CT_LEDGER;
			else if( sCompType == _T("TRANSOM") )				eCompType = CT_TRANSOM;
			else if( sCompType == _T("MESH GUARD") )			eCompType = CT_MESH_GUARD;
			else if( sCompType == _T("RAIL") )					eCompType = CT_RAIL;
			else if( sCompType == _T("MID RAIL") )				eCompType = CT_MID_RAIL;
			else if( sCompType == _T("TOE BOARD") )				eCompType = CT_TOE_BOARD;
			else if( sCompType == _T("CHAIN LINK") )			eCompType = CT_CHAIN_LINK;
			else if( sCompType == _T("SHADE CLOTH") )			eCompType = CT_SHADE_CLOTH;
			else if( sCompType == _T("TIE TUBE") )				eCompType = CT_TIE_TUBE;
			else if( sCompType == _T("TIE CLAMP COLUMN") )		eCompType = CT_TIE_CLAMP_COLUMN;
			else if( sCompType == _T("TIE CLAMP MASONARY") )	eCompType = CT_TIE_CLAMP_MASONARY;
			else if( sCompType == _T("TIE CLAMP YOKE") )		eCompType = CT_TIE_CLAMP_YOKE;
			else if( sCompType == _T("TIE CLAMP 90DEGREE") )	eCompType = CT_TIE_CLAMP_90DEGREE;
			else if( sCompType == _T("BRACING") )				eCompType = CT_BRACING;
			else if( sCompType == _T("STAIR") )					eCompType = CT_STAIR;
			else if( sCompType == _T("LADDER") )				eCompType = CT_LADDER;
			else if( sCompType == _T("STANDARD") )				eCompType = CT_STANDARD;
			else if( sCompType == _T("STANDARD OPENEND") )		eCompType = CT_STANDARD_OPENEND;
			else if( sCompType == _T("JACK") )					eCompType = CT_JACK;
			else if( sCompType == _T("SOLEBOARD") )				eCompType = CT_SOLEBOARD;
			else if( sCompType == _T("HOPUP BRACKET") )			eCompType = CT_HOPUP_BRACKET;
			else if( sCompType == _T("CORNER STAGE BOARD") )	eCompType = CT_CORNER_STAGE_BOARD;
			else if( sCompType == _T("TIE BAR") )				eCompType = CT_TIE_BAR;
			else if( sCompType == _T("STANDARD CONNECTOR") )	eCompType = CT_STANDARD_CONNECTOR;
			else if( sCompType == _T("LADDER PUTLOG") )			eCompType = CT_LADDER_PUTLOG;
			else if( sCompType == _T("STAIR RAIL") )			eCompType = CT_STAIR_RAIL;
			else if( sCompType == _T("STAIR RAIL STOPEND") )	eCompType = CT_STAIR_RAIL_STOPEND;
			else if( sCompType == _T("TOE BOARD CLIP") )		eCompType = CT_TOE_BOARD_CLIP;
			else if( sCompType == _T("PUTLOG CLIP") )			eCompType = CT_PUTLOG_CLIP;
			else if( sCompType == _T("TEXT") )					eCompType = CT_TEXT;
			else
				continue;
		}
		else
		{
			eCompType	=(ComponentTypeEnum)_ttoi( sCompType );
		}

		///////////////////////////////////////////////////////////////
		//find the component type
		iComma			= sLine.Find( COMMA );
		if( iComma<1 )	continue;	//no comma
		sMaterial		= sLine.Left( iComma );
		sLine			= sLine.Right( sLine.GetLength()-(iComma+1) );
		if( !_istdigit( sMaterial[0] ) )
		{
			if(      sMaterial == _T("STEEL") )		eMaterial = MT_STEEL;
			else if( sMaterial == _T("TIMBER") )	eMaterial = MT_TIMBER;
			else if( sMaterial == _T("OTHER") )		eMaterial = MT_OTHER;
			else
				continue;
		}
		else
		{
			eMaterial	=(MaterialTypeEnum)_ttoi( sMaterial );
		}

		///////////////////////////////////////////////////////////////
		//find the common length
		dCommonLength = GetNextDoubleAndMoveOn( sLine );
		if( dCommonLength==CDT_NOT_FOUND ) continue;

		///////////////////////////////////////////////////////////////
		//find the mills Part Number
		iComma			= sLine.Find( COMMA );
		if( iComma>=0 )
		{
			sMillsPartNumber	= sLine.Left( iComma );
			sLine				= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( _istdigit( sCompType[0] ) )
			{
				continue;
			}
		}

		///////////////////////////////////////////////////////////////
		//find the Kwikstage Part Number
		iComma			= sLine.Find( COMMA );
		if( iComma>=0 )
		{
			sKwikstagePartNumber	= sLine.Left( iComma );
			sLine					= sLine.Right( sLine.GetLength()-(iComma+1) );
			if( _istdigit( sCompType[0] ) )
			{
				continue;
			}
			else
			{
				///////////////////////////////////////////////////////////////
				//find the Other Part Number
				if( sLine.GetLength()>0 )
				{
					iComma			= sLine.Find( COMMA );
					if( iComma>=0 )
					{
						sOtherPartNumber		= sLine.Left( iComma );
						sLine					= sLine.Right( sLine.GetLength()-(iComma+1) );
						if( _istdigit( sCompType[0] ) )
						{
							continue;
						}
					}
					else
					{
						sOtherPartNumber = sLine;
					}
				}

			}
		}
		else
		{
			sKwikstagePartNumber = sLine;
		}


		///////////////////////////////////////////////////////////////
		//ok we have the values so lets add them to the table
		pElement = new ComponentDetailsElement(this);
		pElement->m_eCompType					= eCompType;
		pElement->m_eMaterialType				= eMaterial;
		pElement->m_dCommonLength				= dCommonLength;
		pElement->m_sMillsPartNumber			= sMillsPartNumber;
		pElement->m_sKwikstagePartNumber		= sKwikstagePartNumber;
		pElement->m_sOtherPartNumber			= sOtherPartNumber;

		bAdd = true;
		//Kwikstage stock element
		if( !sMillsPartNumber.IsEmpty() )
		{
			pElement->m_pStockDetailsMills = pController->GetStockList()->GetMatchingComponent( sMillsPartNumber, false );
			if( pElement->m_pStockDetailsMills==NULL )
			{
				sMsg.Format( _T("The component details file, %s,\ncontains Mills system part number, %s,\n"),
							File.GetFileName(), sMillsPartNumber );
				bAdd = false;
			}
		}

		//Kwikstage stock element
		if( !sKwikstagePartNumber.IsEmpty() )
		{
			pElement->m_pStockDetailsKwikScaf = pController->GetStockList()->GetMatchingComponent( sKwikstagePartNumber, false );
			if( pElement->m_pStockDetailsKwikScaf==NULL )
			{
				sMsg.Format( _T("The component details file, %s,\ncontains Kwikstage system part number, %s,\n"),
							File.GetFileName(), sKwikstagePartNumber );
				bAdd = false;
			}
		}

		//Other stock element
		if( !sOtherPartNumber.IsEmpty() )
		{
			pElement->m_pStockDetailsOther = pController->GetStockList()->GetMatchingComponent( sOtherPartNumber, false );
			if( pElement->m_pStockDetailsOther==NULL )
			{
				sMsg.Format( _T("The component details file, %s,\ncontains part number, %s,\n"),
							File.GetFileName(), sOtherPartNumber );
				bAdd = false;
			}
		}

		if( bAdd )
		{
			Add( pElement );
		}
		else
		{
			CString sTemp;
			sTemp.Format( _T("( %s, %s, %0.0fmm )\n"), GetComponentDescStr(eCompType), GetComponentMaterialStr(eMaterial), dCommonLength );
			sMsg+= sTemp;
			sMsg+= _T("that does not exist in the stock file!  I cannot\n");
			sMsg+= _T("use this component until this error has been\ncorrected.");
			MessageBox( NULL, sMsg, _T("Part number not found"), MB_OK );
			delete pElement;
		}
	}
	File.Close();
	gbOpenFileDialogActive = false;
	//did we find anything?
	if( GetSize()<=0 )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		sErrMsg.Format( _T("The file, %s, is not a Components Details File"),
						File.GetFileName() );
		MessageBox( NULL, sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
		return false;
	}
	else
	{
		int		i, iVirtualNumber = 1;
		double	dLength;
		CString	sPartNumber;

		for( i=0; i<pController->GetStockList()->GetSize(); i++ )
		{
			StockListElement *pEle;
			pEle = pController->GetStockList()->GetAt(i);
			sPartNumber = pEle->GetPartNumber();
			dLength		= pEle->GetLength();
			if( dLength>ROUND_ERROR && GetMatchingComponent( sPartNumber )==NULL )
			{
				//We have a component in the stock list that has a length
				//	(thus drawing info) but no matching data in component
				//	details file
				pElement = new ComponentDetailsElement(this);
				pElement->m_eCompType					= CT_VISUAL_COMPONENT;
				pElement->m_eMaterialType				= MT_OTHER;
				pElement->m_dCommonLength				= iVirtualNumber++;
				pElement->m_sMillsPartNumber			.Empty();
				pElement->m_sKwikstagePartNumber		.Empty();
				pElement->m_sOtherPartNumber			= sPartNumber;
				pElement->m_pStockDetailsMills			= NULL;
				pElement->m_pStockDetailsKwikScaf		= NULL;
				pElement->m_pStockDetailsOther			= pEle;
				Add( pElement );
			}
		}
	}

	return true;
}

bool ComponentDetailsArray::GetFile(CStdioFile &File)
{
	UINT					uiMode;
	CString					sFilename;

	uiMode = CFile::modeRead|CFile::typeText;

	//read the filename from the registry
	sFilename = GetFilenameInRegistry();

  //try the default filename
	if( sFilename.IsEmpty() || !File.Open( sFilename, uiMode ) )
	{
		///////////////////////////////////////////////////
		//Get the file name to use from the user
		CFileDialog dbox(TRUE, EXTENTION_LOAD_COMPONENT_DETAILS_FILE, sFilename,
			OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_LOAD_COMPONENT_DETAILS_FILE ); //, acedGetAcadFrame() /*CWnd* pParentWnd = NULL*/ );
		dbox.m_ofn.lpstrTitle = TITLE_LOAD_COMPONENT_DETAILS_FILE;
		gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
		if(dbox.DoModal()!=IDOK)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			gbOpenFileDialogActive = false;
			return false;
		}
		gbOpenFileDialogActive = false; 

		sFilename = dbox.GetPathName();

		//open it this time
		CFileException Error;
		if( !File.Open( sFilename, uiMode, &Error ) )
		{
			TCHAR   szCause[255];
			CString strFormatted;
			CString sMessage;

			MessageBeep(MB_ICONEXCLAMATION);
			Error.GetErrorMessage(szCause, 255);
			sMessage = ERROR_MSG_NOT_OPEN_FILE_WRITE;
			sMessage+= szCause;
			MessageBox( NULL, sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
			SetFilenameInRegistry( _T("") );
			return false;
		}

		//store the filename in the registry
		SetFilenameInRegistry( sFilename );
	}
	return true;
}

// Changed LPCTSTR to CString, ~BSK, 21.02.2018
void ComponentDetailsArray::SetFilenameInRegistry(CString strFileName)
{
	::SetStringInRegistry( PROJECT_COMPONENT_DETAILS_FILENAME_SECTION, strFileName );
}

// Changed LPCTSTR to CString, ~SJ, 03.09.2007
CString ComponentDetailsArray::GetFilenameInRegistry()
{
	return ::GetStringInRegistry( PROJECT_COMPONENT_DETAILS_FILENAME_SECTION );
}

void ComponentDetailsArray::SetEntPathInRegistry(LPCTSTR strFileName)
{
	//Store the filename in the registry
	SetStringInRegistry( PROJECT_COMPONENT_DETAILS_ENT_PATH_SECTION, strFileName);
}

LPCTSTR ComponentDetailsArray::GetEntPathInRegistry()
{
	//Get the filename from the registry
	return GetStringInRegistry( PROJECT_COMPONENT_DETAILS_ENT_PATH_SECTION );
}

SystemEnum ComponentDetailsArray::GetSystem() const
{
	;//assert( gpController!=NULL );
	return gpController->GetSystem();
}

double ComponentDetailsArray::GetNextDoubleAndMoveOn(CString &sLine)
{
	int		iComma;
	CString	sText;

	iComma = sLine.Find( COMMA );
	if( iComma<1 )
		return CDT_NOT_FOUND;	//no comma

	sText = sLine.Left( iComma );
	sLine = sLine.Right( sLine.GetLength()-(iComma+1) );

	if( _istdigit( sText[0] ) || sText[0]==_T('-') || sText[0]==_T('.') )
		return _tstof( sText );

	return CDT_NOT_FOUND;
}

double ComponentDetailsArray::GetActualLength( SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning )
{
	return GetStockDetails( eCompType, dCommonLength, eMaterial, eSystem )->GetLength();
}

double ComponentDetailsArray::GetCommonLength( SystemEnum eSystem, ComponentTypeEnum eCompType, double dActualLength, MaterialTypeEnum eMaterial, bool bShowWarning )
{
	int i;
	double	dLength;

	i=0;
	while( i<GetSize() )
	{
		if( GetAt(i)->m_eCompType==eCompType &&
			GetAt(i)->m_eMaterialType==eMaterial )
		{
			dLength = GetAt(i)->GetStockDetails(eSystem)->GetLength();

			if( dLength>=dActualLength-ROUND_ERROR && 
				 dLength<=dActualLength+ROUND_ERROR )
			{
				 return GetAt(i)->m_dCommonLength;
			}
		}
		i++;
	}
	return CDT_NOT_FOUND;
}

double ComponentDetailsArray::GetActualWeight( SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning )
{
	return GetStockDetails( eCompType, dCommonLength, eMaterial, eSystem )->GetWeight();
}

double ComponentDetailsArray::GetActualWidth( SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning )
{
	return GetStockDetails( eCompType, dCommonLength, eMaterial, eSystem )->GetWidth();
}

double ComponentDetailsArray::GetActualHeight( SystemEnum eSystem, ComponentTypeEnum eCompType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning)
{
	return GetStockDetails( eCompType, dCommonLength, eMaterial, eSystem )->GetHeight();
}

LPCTSTR ComponentDetailsArray::GetComponentDescriptionStr( SystemEnum eSystem, ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial  )
{
	return GetStockDetails( eComponentType, dCommonLength, eMaterial, eSystem )->GetDescription();
}

LPCTSTR ComponentDetailsArray::GetComponentPartNumberStr( SystemEnum eSystem, ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial)
{
	ComponentDetailsElement	*pElement;

	pElement = GetMatchingComponent( eComponentType, dCommonLength, eMaterial, false );

	if( pElement!=NULL )
	{
		switch( eSystem )
		{
		case( S_MILLS ):
			return pElement->m_sMillsPartNumber;
		case( S_KWIKSTAGE ):
			return pElement->m_sKwikstagePartNumber;
		case( S_OTHER ):
			return pElement->m_sOtherPartNumber;
		default:
			;//assert( false );
		}
	}
	return UNKNOWN_COMP_TYPE;
}


ComponentDetailsElement *ComponentDetailsArray::GetMatchingComponent(ComponentTypeEnum eComponentType, double dCommonLength, MaterialTypeEnum eMaterial, bool bShowWarning )
{
	int		iCounter;
	double	dLength;

	iCounter=0;
	while( iCounter<GetSize() )
	{
		if( GetAt(iCounter)->m_eCompType==eComponentType )
		{
			//if the common length if 0.00 then we are just after the first one found
			if((dCommonLength==0.00) ||(GetAt(iCounter)->m_eMaterialType==eMaterial) )
			{
				//if the common length if 0.00 then we are just after the first one found
				dLength = GetAt(iCounter)->m_dCommonLength;
				if((dCommonLength==0.00) ||(dLength>=dCommonLength-0.01 &&
											   dLength<=dCommonLength+0.01) )
				{
					return GetAt(iCounter);
				}
			}
		}
		iCounter++;
	}

	//We could not locate the component in the component details file, so give an error message
	if( bShowWarning )
	{
		if( eComponentType==CT_BOM_EXTRA ||
			eComponentType==CT_VISUAL_COMPONENT )
		{
			//This is a visual component so it is not supprising that
			//	we couldn't find it in the component details file
			return NULL;
		}
		CString sMsg, sMat, sFile, sType;
		sType	= GetComponentDescStr(eComponentType);
		sMat	= GetComponentMaterialStr( eMaterial );
		sFile	= GetFilenameInRegistry();
    sMsg.Format( _T("Component: %s, Common Length: %0.2f, Material: %s\nComponent Filename: "),
						sType, dCommonLength, sMat );
		sMsg += sFile;
		sMsg +=( _T("\n------------------------------------------------------------\n"));
		acutPrintf( sMsg );
	}

	return NULL;
}

ComponentDetailsElement *ComponentDetailsArray::GetMatchingComponent( CString sPartNumber )
{
	int						iCounter;
	ComponentDetailsElement	*pEle;

	iCounter=0;
	while( iCounter<GetSize() )
	{
		pEle = GetAt(iCounter);
		if( pEle->m_sKwikstagePartNumber==sPartNumber || 
			pEle->m_sMillsPartNumber==sPartNumber ||
			pEle->m_sOtherPartNumber==sPartNumber )
		{
			return pEle;
		}
		iCounter++;
	}

	return NULL;
}


/*******************************************************************************
*	History Records
********************************************************************************
* $History: ComponentDetails.cpp $
 * 
 * *****************  Version 82  *****************
 * User: Jsb          Date: 20/12/00   Time: 3:19p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed the Corner hopup problem, this should hopefully be the final 1.2
 * release
 * 
 * *****************  Version 81  *****************
 * User: Jsb          Date: 14/12/00   Time: 9:52a
 * Updated in $/Meccano/Stage 1/Code
 * 500 and 800mm standards seem to be working
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:42p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 24/10/00   Time: 4:09p
 * Updated in $/Meccano/Stage 1/Code
 * About to release 1.1h for testing
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 5/10/00    Time: 8:14a
 * Updated in $/Meccano/Stage 1/Code
 * just finished building version 1.5.1.3 (R1.1c)
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 3/10/00    Time: 4:38p
 * Updated in $/Meccano/Stage 1/Code
 * Just finished preliminary ability to be able to use different systems
 * within the same drawing
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 30/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Customize Colour complete and hidden - 
 * Shade and Chain mesh prices fixed
 * Bill Cross stage 1 fully complete
 * 
 * *****************  Version 73  *****************
 * User: Jsb          Date: 28/08/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * Merger completed, now include stage 1 of Bill cross stuff & latest
 * build.  This is the new way point
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 21/08/00   Time: 3:50p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 9/08/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 8/08/00    Time: 4:16p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 7/08/00    Time: 3:21p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8w
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 25/07/00   Time: 5:07p
 * Updated in $/Meccano/Stage 1/Code
 * Column Ties are not oriented correctly
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 7/07/00    Time: 7:50a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 6/07/00    Time: 12:29p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 28/06/00   Time: 1:27p
 * Updated in $/Meccano/Stage 1/Code
 * About to try to create a seperate Toolbar project
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 23/06/00   Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 20/06/00   Time: 12:28p
 * Updated in $/Meccano/Stage 1/Code
 * About to change the way moving decks and roof protect lapboards look
 * for  lapboards 
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 14/06/00   Time: 4:51p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 25/05/00   Time: 4:48p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 54  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:00p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 11/05/00   Time: 2:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 9/05/00    Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 8/05/00    Time: 4:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 7/04/00    Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5(RC1.4.4fe)
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 2/03/00    Time: 4:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 28/02/00   Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 24/02/00   Time: 4:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 14/02/00   Time: 4:02p
 * Updated in $/Meccano/Stage 1/Code
 * currently debugging the ladder bays
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 9/02/00    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * About to try bug 754 - BOMs Standards/Transoms Selected Bays
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 7/02/00    Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 4/02/00    Time: 4:26p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 3/02/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 31/01/00   Time: 11:22a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 30/01/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Need to test the Ladder bays code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 29/01/00   Time: 2:18p
 * Updated in $/Meccano/Stage 1/Code
 * Completed 680, 631, 722, 723, 724, 725, 726, 727, 729 & 730
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 28/01/00   Time: 2:56p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.13
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 27/01/00   Time: 4:36p
 * Updated in $/Meccano/Stage 1/Code
 * currently working on the end on components
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 25/01/00   Time: 4:46p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on 704
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 19/01/00   Time: 4:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 31  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 30  *****************
 * User: Jsb          Date: 5/01/00    Time: 12:10p
 * Updated in $/Meccano/Stage 1/Code
 * about to create release 1.3.12(Beta13)
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 4/01/00    Time: 12:12p
 * Updated in $/Meccano/Stage 1/Code
 * About to create 1.3.11(Beta12)
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 6/12/99    Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * Got everything working as well as I had them at home
 * 
 * *****************  Version 27  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Jsb          Date: 8/11/99    Time: 3:57p
 * Updated in $/Meccano/Stage 1/Code
 * Currently working on bug #447
 * 
 * *****************  Version 23  *****************
 * User: Jsb          Date: 5/11/99    Time: 10:24a
 * Updated in $/Meccano/Stage 1/Code
 * Building Beta7(1.3.6)
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 21  *****************
 * User: Dar          Date: 26/10/99   Time: 12:54p
 * Updated in $/Meccano/Stage 1/Code
 * fixed bug 301 - requires testing
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 19  *****************
 * User: Jsb          Date: 6/10/99    Time: 11:50a
 * Updated in $/Meccano/Stage 1/Code
 * Have updated Reader for ComponentDetails.csv since addition of Mills
 * and  Kwikscaff Part numbers
 * 
 * *****************  Version 18  *****************
 * User: Jsb          Date: 5/10/99    Time: 10:53a
 * Updated in $/Meccano/Stage 1/Code
 * Added the PartNumber to the ComponentDetails
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 23/09/99   Time: 11:32a
 * Updated in $/Meccano/Stage 1/Code
 * All 3D Component Entity drawing now use the same path, and it will
 * prompt them for that path if it can't find it
 * 
 * *****************  Version 16  *****************
 * User: Jsb          Date: 22/09/99   Time: 3:09p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 21/09/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Insert bay nearly working
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 17/09/99   Time: 12:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 17/09/99   Time: 11:58a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 17/09/99   Time: 10:07a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 15/09/99   Time: 8:40a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Jsb          Date: 8/31/99    Time: 3:05p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Schematic Text now correctly positioned
 * 2) Currently working on positioning and removing schematic
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/27/99    Time: 1:01p
 * Updated in $/Meccano/Stage 1/Code
 * Corner boards now working
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 8/27/99    Time: 10:35a
 * Updated in $/Meccano/Stage 1/Code
 * Stage boards are now operating correctly - almost
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 8/19/99    Time: 11:48a
 * Updated in $/Meccano/Stage 1/Code
*
*******************************************************************************/

doubleArray ComponentDetailsArray::GetCommonLengthsForComponent( ComponentTypeEnum eCompType, MaterialTypeEnum eMaterial )
{
	int			i, j;
	bool		bInserted;
	double		dLength, dTest;
	CString		sDescription, sMsg, sText;
	doubleArray	daLengths;

	daLengths.RemoveAll();

	i=0;
	while( i<GetSize() )
	{
		if( GetAt(i)->m_eCompType==eCompType )
		{
			//if the common length if 0.00 then we are just after the first one found
			if( GetAt(i)->m_eMaterialType==eMaterial )
			{
				dLength = GetAt(i)->m_dCommonLength;

				//if it is not already there, then insert the length
				//	into the list using insertion sort!
				bInserted = false;
				for( j=0; j<daLengths.GetSize(); j++ )
				{
					dTest = daLengths.GetAt(j);
					if( dTest==dLength )
					{
						//already in the list
						bInserted = true;
						break;
					}

					if( dTest>dLength )
					{
						daLengths.InsertAt( j, dLength );
						bInserted = true;
						break;
					}
				}
				if( !bInserted )
				{
					//this is the longest length so far, so add it to the back!
					daLengths.Add( dLength );
				}
			}
		}
		i++;
	}

	return daLengths;
}

void ComponentDetailsArray::RemoveAll()
{
	ComponentDetailsElement	*pEle;

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

double ComponentDetailsArray::GetAvailableLength(ComponentTypeEnum eType, MaterialTypeEnum eMaterail, double dLength, RoundingTypeEnum eRound, bool bNotFoundReturnNearest/*=false*/)
{
	int			i;
	double		dAvailableLength, dNearest, dNearestLength;
	doubleArray	daLengths;

	daLengths = GetCommonLengthsForComponent( eType, eMaterail );
	if( daLengths.GetSize()<=0 )
		return -2.00;

	if( eRound==RT_ROUND_OFF )
	{
		double		dAvailableLengthAlt;
		daLengths.SelectionSort( true );

		//Check if the length is < the first available
		dAvailableLength	= daLengths.GetAt(0);
		if( dLength<dAvailableLength+ROUND_ERROR )
			return dAvailableLength;

		//Check if the length is > the last available
		dAvailableLength	= daLengths.GetAt(daLengths.GetSize()-1);
		if( dLength>dAvailableLength-ROUND_ERROR )
			return dAvailableLength;

		//it must be in between two available lengths
		for( i=0; i<daLengths.GetSize()-1; i++ )
		{
			dAvailableLength	= daLengths.GetAt(i);
			dAvailableLengthAlt	= daLengths.GetAt(i+1);
			if( dLength>dAvailableLength-ROUND_ERROR &&
				dLength<dAvailableLengthAlt+ROUND_ERROR )
			{
				if((dLength-dAvailableLength)<(dAvailableLengthAlt-dLength) )
				{
					return dAvailableLength;
				}
				else
				{
					return dAvailableLengthAlt;
				}

			}
		}

		//How did we get here?  we should have found one by now!
		;//assert( false );
	}
	else
	{
		//sort the list of available lengths
		if( eRound==RT_ROUND_DOWN )
			daLengths.SelectionSort( false );
		else if( eRound==RT_ROUND_UP )
			daLengths.SelectionSort( true );

		;//assert( daLengths.GetSize()>0 );
		dNearestLength = daLengths.GetAt(0);
		dNearest = fabs( dNearestLength-dLength );

		for( i=0; i<daLengths.GetSize(); i++ )
		{
			dAvailableLength = daLengths.GetAt(i);

			if( fabs(dAvailableLength-dLength)<dNearest )
			{
				dNearest = fabs(dAvailableLength-dLength);
				dNearestLength = dAvailableLength;
			}

			switch( eRound )
			{
			case( RT_ROUND_DOWN ):
				if( dAvailableLength<dLength )
					return dAvailableLength;

				break;
			case( RT_ROUND_UP ):
				if( dAvailableLength>dLength )
					return dAvailableLength;

				break;
			default:
			case( RT_INVALID ):
				;//assert( false );
				break;
			}
		}
	}

	if( bNotFoundReturnNearest )
		return dNearestLength;
	//not found!
	return -1.00;
}

/*
void ComponentDetailsArray::switchToModelSpace()
{
	HMODULE hAcad =::GetModuleHandle(_T("acad.exe"));
	AcDbMspacePtr pAcDbMspacePtr =(AcDbMspacePtr)::GetProcAddress(hAcad,
						_T("?acedMspace@@YA?AW4ErrorStatus@Acad@@PAVAcDbDatabase@@@Z"));
	if(pAcDbMspacePtr != NULL)
		(acdbHostApplicationServices()->workingDatabase());
}

void ComponentDetailsArray::switchToPaperSpace()
{
	HMODULE hAcad =::GetModuleHandle(_T("acad.exe"));
	AcDbPspacePtr pAcDbPspacePtr =(AcDbPspacePtr)::GetProcAddress(hAcad,
						_T("?acedPspace@@YA?AW4ErrorStatus@Acad@@PAVAcDbDatabase@@@Z"));
	if(pAcDbPspacePtr != NULL)
		(acdbHostApplicationServices()->workingDatabase());
}
*/

double ComponentDetailsArray::GetBayLengthActual(double dCommonLength, SystemEnum eSystem )
{
	return GetActualLength( eSystem, CT_LEDGER, dCommonLength, MT_STEEL, false ) +
			GetActualWidth( eSystem, CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
}

double ComponentDetailsArray::GetBayWidthActual( double dCommonLength, SystemEnum eSystem )
{
	return GetActualLength( eSystem, CT_TRANSOM, dCommonLength, MT_STEEL, false ) +
			GetActualWidth( eSystem, CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
}

double ComponentDetailsArray::GetBayLengthCommon( double dActualLength, SystemEnum eSystem )
{
	dActualLength-= GetActualWidth( eSystem, CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
	return GetCommonLength( eSystem, CT_LEDGER, dActualLength, MT_STEEL, false );
}

double ComponentDetailsArray::GetBayWidthCommon( double dActualLength, SystemEnum eSystem )
{
	dActualLength-= GetActualWidth( eSystem, CT_STANDARD, COMPONENT_LENGTH_2000, MT_STEEL, false );
	return GetCommonLength( eSystem, CT_LEDGER, dActualLength, MT_STEEL, false );
}


StockListElement *ComponentDetailsArray::GetStockDetails(ComponentTypeEnum eType, double dCommonLength, MaterialTypeEnum eMaterial, SystemEnum eSystem)
{
	ComponentDetailsElement *pEle;

	pEle = GetMatchingComponent( eType, dCommonLength, eMaterial, true );
	if( pEle==NULL )
	{
		if( eType!=CT_BOM_EXTRA && eType!=CT_VISUAL_COMPONENT && eType!=CT_UNDEFINED )
		{
			;//assert( false );
		}
		return NULL;
	}
	return pEle->GetStockDetails( eSystem );
}

StockListElement * ComponentDetailsElement::GetStockDetails(SystemEnum eSystem)
{
	switch( eSystem )
	{
	case( S_MILLS ):
		if( m_pStockDetailsMills!=NULL )
		{
			return m_pStockDetailsMills;
		}

		;//assert( gpController!=NULL );
		m_pStockDetailsMills = gpController->GetStockList()->GetMatchingComponent( m_sMillsPartNumber, false );
		;//assert( m_pStockDetailsMills!=NULL );
		return m_pStockDetailsMills;

	case( S_KWIKSTAGE ):
		if( m_pStockDetailsKwikScaf!=NULL )
		{
			return m_pStockDetailsKwikScaf;
		}

		;//assert( gpController!=NULL );
		m_pStockDetailsKwikScaf = gpController->GetStockList()->GetMatchingComponent( m_sKwikstagePartNumber, false );
		;//assert( m_pStockDetailsKwikScaf!=NULL );
		return m_pStockDetailsKwikScaf;

	case( S_OTHER ):
		if( m_pStockDetailsOther!=NULL )
		{
			return m_pStockDetailsOther;
		}

		;//assert( gpController!=NULL );
		m_pStockDetailsOther = gpController->GetStockList()->GetMatchingComponent( m_sOtherPartNumber, false );
		;//assert( m_pStockDetailsOther!=NULL );
		return m_pStockDetailsOther;

	default:
		;//assert( false );
	}

	return NULL;
}
