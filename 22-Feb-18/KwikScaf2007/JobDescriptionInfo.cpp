/////////////////////////////////////////////
// ObjectARX defined commands
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

#include <dbdict.h>
#include <summinfo.h>
#include "StdAfx.h"
#include "MeccanoDefinitions.h"

#include "JobDescriptionInfo.h"
#include "VersionDefinitions.h"

// Custom Key definitions
#define JOB_TITLE		_T("Job Title")			// m_sJobTitle
#define JOB_DESCRIPT	_T("Job Description")	// m_sJobDescription;
#define JOB_BRANCH		_T("Job Branch")		// m_sJobBranch;
#define DRAFTSMAN		_T("Draftsman Name")	// m_sDraftsmanName;

#define REF_NUMBER		_T("Job Ref. Number")	// m_sRefNumber;
#define MARKET_ENQUIRE	_T("Marketing Enquiry Number") // m_sMarketingEnquiryNumber;

#define PROJECT_DURATION	_T("Project Duration(days)") // m_sProjectDuration;
#define PROJECT_START_DATE	_T("Project Start Date")	 // m_ctStartDate;


#define CLIENT_NAME		_T("Client Name")		// m_sClientName;
#define CLIENT_COMPANY	_T("Client Company")	// m_sClientAddress;
#define CLIENT_ADDRESS	_T("Client Address")	// m_sClientAddress;
//#define CLIENT_CITY		"Client City"		// m_sClientCity;
#define CLIENT_ZIP		_T("Client Zip Code")	// m_sClientZip;

//#define PHONE_COUNTRY_CODE	"Client Phone Country Code"	// m_sPhoneCountryCode;
//#define PHONE_STATE_CODE	"Client Phone State Code"	// m_sPhoneStateCode;
#define PHONE_NUMBER		_T("Client Phone Number")		// m_sPhoneNumber;

//#define FAX_COUNTRY_CODE	"Client Fax Country Code"	// m_sFaxCountryCode;
//#define FAX_STATE_CODE		"Client Fax State Code"		// m_sFaxStateCode;
#define FAX_NUMBER			_T("Client Fax Number")			// m_sFaxNumber;

CString BRANCH_DETAILS_LABEL[BDI_SIZE];


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

JobDescriptionInfo::JobDescriptionInfo()
{
	BRANCH_DETAILS_LABEL[BDI_COUNTRY]	= _T("Branch Country");
	BRANCH_DETAILS_LABEL[BDI_STATE]		= _T("Branch State");
	BRANCH_DETAILS_LABEL[BDI_NUMBER]	= _T("Branch Number");
	BRANCH_DETAILS_LABEL[BDI_NAME]		= _T("Branch Name");
	BRANCH_DETAILS_LABEL[BDI_EMAIL]		= _T("Branch Email");
	BRANCH_DETAILS_LABEL[BDI_ADDRESS1]	= _T("Branch Address1");
	BRANCH_DETAILS_LABEL[BDI_ADDRESS2]	= _T("Branch Address2");
	BRANCH_DETAILS_LABEL[BDI_ADDRESS3]	= _T("Branch Address3");
	BRANCH_DETAILS_LABEL[BDI_POSTAL1]	= _T("Branch Postal1");
	BRANCH_DETAILS_LABEL[BDI_POSTAL2]	= _T("Branch Postal2");
	BRANCH_DETAILS_LABEL[BDI_POSTAL3]	= _T("Branch Postal3");
	BRANCH_DETAILS_LABEL[BDI_PHONE]		= _T("Branch Phone");
	BRANCH_DETAILS_LABEL[BDI_FAX]		= _T("Branch Fax");
	BRANCH_DETAILS_LABEL[BDI_MANAGER]	= _T("Branch Manager");
	BRANCH_DETAILS_LABEL[BDI_EXTRA1]	= _T("Branch Extra1");
	BRANCH_DETAILS_LABEL[BDI_EXTRA2]	= _T("Branch Extra2");
}

///////////////////////////////////////////////////////////////////////////////
//Serialize storage/retrieval function
///////////////////////////////////////////////////////////////////////////////
void JobDescriptionInfo::Serialize(CArchive &ar)
{
	ar;
/*	if (ar.IsStoring())    // Store Object?
	{
		ar << JOB_DESCRIPTION_VERSION_1_0_0;
		
		ar << m_sJobBranch;
		ar << m_sClientAddress;
		ar << m_sClientCity;
		ar << m_sClientName;
		ar << m_sClientZip;
		ar << m_sJobDescription;
		ar << m_sDraftsmanName;
		ar << m_sFaxCountryCode;
		ar << m_sFaxNumber;
		ar << m_sFaxStateCode;
		ar << m_sMarketingEnquiryNumber;
		ar << m_sPhoneCountryCode;
		ar << m_sPhoneNumber;
		ar << m_sPhoneStateCode;
		ar << m_sProjectDuration;
		ar << m_sRefNumber;
		ar << m_ctStartDate;
	}
	else					// or Load Object?
	{
		VersionNumber uiVersion;
		ar >> uiVersion;
		switch (uiVersion)
		{
			case JOB_DESCRIPTION_VERSION_1_0_0 :
				ar >> m_sJobBranch;
				ar >> m_sClientAddress;
				ar >> m_sClientCity;
				ar >> m_sClientName;
				ar >> m_sClientZip;
				ar >> m_sJobDescription;
				ar >> m_sDraftsmanName;
				ar >> m_sFaxCountryCode;
				ar >> m_sFaxNumber;
				ar >> m_sFaxStateCode;
				ar >> m_sMarketingEnquiryNumber;
				ar >> m_sPhoneCountryCode;
				ar >> m_sPhoneNumber;
				ar >> m_sPhoneStateCode;
				ar >> m_sProjectDuration;
				ar >> m_sRefNumber;
				ar >> m_ctStartDate;
				break;
			default:
				;//assert( false );
		};
	}
*/
}


void JobDescriptionInfo::LoadSummInfo()
{
	Acad::ErrorStatus es;
	AcDbDatabaseSummaryInfo *pInfo;
	AcDbDatabase *pCurDb = NULL;
	AcDbDictionary* pDict;
	TCHAR* info;
//	char* key;
//	char* value;
//	int customQty;
//	int index;


  /////////////////////////////////////////////////////////////////////////////////////////
  // This is no longer required in AutoCAD 2007/2008
  // Code commented by ~SJ~, 30.07.2007
  //
	//You need to make sure that 'acsiobj.arx' is loaded
	//if (!acrxDynamicLinker->loadModule(_T("acsiobj.arx"), 0))
	//{
  //      acutPrintf(_T("\nJob detail error - Failed to load the required SummaryInfo object.\n"));
  //      return;
  //}
  // End code commented by ~SJ~, 30.07.2007
  /////////////////////////////////////////////////////////////////////////////////////////

	pCurDb = acdbHostApplicationServices()->workingDatabase();

	// Get the Named Objects Dictionary
	es = pCurDb->getNamedObjectsDictionary(pDict, AcDb::kForRead);
	;//assert( es==Acad::eOk );

	// Is there a SUMMARY_INFO_DICTIONARY_NAME key
	if(!pDict->has(SUMMARY_INFO_DICTIONARY_NAME))
	{
//		acutPrintf("\nThis drawing does not contain Summary Information");
		es = pDict->close(); 
		;//assert( es==Acad::eOk );
		return;
	}

	es = pDict->close();
	;//assert( es==Acad::eOk );

	// Get a pointer to the workingDatabase() summary information
	es = acdbGetSummaryInfo(pCurDb, pInfo);
	;//assert( es==Acad::eOk );

	// read data in from summary tab fields
	pInfo->getTitle(info);
	m_sJobTitle = CString(info);

	pInfo->getComments(info);
	m_sJobDescription = CString(info);

	pInfo->getAuthor(info);
	m_sDraftsmanName = CString(info);

	// read data in from custom fields
	GetCustomInfo( pInfo, JOB_BRANCH, m_sJobBranch);
//	GetCustomInfo( pInfo, REF_NUMBER, m_sRefNumber);
	GetCustomInfo( pInfo, MARKET_ENQUIRE, m_sMarketingEnquiryNumber);

	GetCustomInfo( pInfo, PROJECT_DURATION, m_sProjectDuration);
	GetCustomInfo( pInfo, PROJECT_START_DATE, m_sStartDate);

	GetCustomInfo( pInfo, CLIENT_NAME, m_sClientName);
	GetCustomInfo( pInfo, CLIENT_COMPANY, m_sClientCompany);
	GetCustomInfo( pInfo, CLIENT_ADDRESS, m_sClientAddress);
	GetCustomInfo( pInfo, CLIENT_ZIP, m_sClientZip);

	GetCustomInfo( pInfo, PHONE_NUMBER, m_sPhoneNumber);

	GetCustomInfo( pInfo, FAX_NUMBER, m_sFaxNumber);

	int  i, j;
	TCHAR caBranchlabel[31];
	for( i=0; i<BDI_SIZE; i++ )
	{
		for( j=0; j<30 && j<BRANCH_DETAILS_LABEL[i].GetLength(); j++ )
		{
			caBranchlabel[j] = BRANCH_DETAILS_LABEL[i].GetAt(j);
		}
		caBranchlabel[j] = _T('\0');
		GetCustomInfo( pInfo, caBranchlabel, m_sBranchDetails[i] );
	}
}

void JobDescriptionInfo::StoreSummaryInfo()
{
	Acad::ErrorStatus es;
	AcDbDatabaseSummaryInfo *pInfo;
	AcDbDatabase *pCurDb = NULL;
	AcDbDictionary* pDict;
	AcDbDictionary* pSummaryDict;
    AcDbObjectId SummaryDictId;
    AcDbObjectId SumInfoId;
//	char* info;
//	char* key;
//	char* value;
//	int customQty;
//	int index;

  /////////////////////////////////////////////////////////////////////////////////////////
  // This is no longer required in AutoCAD 2007/2008
  // Code commented by ~SJ~, 30.07.2007
  //
	//You need to make sure that 'acsiobj.arx' is loaded
	//if (!acrxDynamicLinker->loadModule(_T("acsiobj.arx"), 0))
	//{
  //      acutPrintf(_T("\nFailed to load the required SummaryInfo object.\n"));
  //      return;
  //}
  // End code commented by ~SJ~, 30.07.2007
  /////////////////////////////////////////////////////////////////////////////////////////

	pCurDb = acdbHostApplicationServices()->workingDatabase();

	// Get the Named Objects Dictionary
	es = pCurDb->getNamedObjectsDictionary(pDict, AcDb::kForWrite);
	;//assert( es==Acad::eOk );
	// Is there a SUMMARY_INFO_DICTIONARY_NAME key
	if(!pDict->has( SUMMARY_INFO_DICTIONARY_NAME )) // if not create it
	{
		pSummaryDict = new AcDbDictionary;

        es=pDict->setAt(SUMMARY_INFO_DICTIONARY_NAME, pSummaryDict, SummaryDictId);
		;//assert( es==Acad::eOk );

		es=pSummaryDict->close();
		;//assert( es==Acad::eOk );
	}
	es=pDict->close();
	;//assert( es==Acad::eOk );

	// Get a pointer to the workingDatabase() summary information
	es = acdbGetSummaryInfo(pCurDb, pInfo);
	;//assert( es==Acad::eOk );

	// write data to summary tab fields
	pInfo->setTitle(m_sJobTitle);
	pInfo->setComments(m_sJobDescription);
	pInfo->setAuthor(m_sDraftsmanName);

	// write date to custom fields
	AddCustomInfo( pInfo, JOB_BRANCH, &m_sJobBranch);
//	AddCustomInfo( pInfo, REF_NUMBER, &m_sRefNumber);
	AddCustomInfo( pInfo, MARKET_ENQUIRE, &m_sMarketingEnquiryNumber);

	AddCustomInfo( pInfo, PROJECT_DURATION, &m_sProjectDuration);
	AddCustomInfo( pInfo, PROJECT_START_DATE, &m_sStartDate);

	AddCustomInfo( pInfo, CLIENT_NAME, &m_sClientName);
	AddCustomInfo( pInfo, CLIENT_COMPANY, &m_sClientCompany);
	AddCustomInfo( pInfo, CLIENT_ADDRESS, &m_sClientAddress);
	AddCustomInfo( pInfo, CLIENT_ZIP, &m_sClientZip);

	AddCustomInfo( pInfo, PHONE_NUMBER, &m_sPhoneNumber);

	AddCustomInfo( pInfo, FAX_NUMBER, &m_sFaxNumber);

	int  i, j;
	TCHAR caBranchlabel[30];
	for( i=0; i<BDI_SIZE; i++ )
	{
		for( j=0; j<30 && j<BRANCH_DETAILS_LABEL[i].GetLength(); j++ )
		{
			caBranchlabel[j] = BRANCH_DETAILS_LABEL[i].GetAt(j);
		}
		caBranchlabel[j] = _T('\0');
		AddCustomInfo( pInfo, caBranchlabel, &(m_sBranchDetails[i]) );
	}

	es = acdbPutSummaryInfo(pInfo);
	;//assert( es==Acad::eOk );
	
//	acdbFree(info);  // todo > check this DAR 991004**
} 

void JobDescriptionInfo::AddCustomInfo(AcDbDatabaseSummaryInfo *pInfo, TCHAR *pKey, CString *pValue)
{
	TCHAR *pTempValue;

	if (pInfo->getCustomSummaryInfo( pKey, pTempValue ) == Acad::eOk)
		pInfo->setCustomSummaryInfo( pKey, *pValue);
	else
		pInfo->addCustomSummaryInfo( pKey, *pValue );

//	acdbFree(pTempValue);
}

void JobDescriptionInfo::GetCustomInfo(AcDbDatabaseSummaryInfo *pInfo, TCHAR *pKey, CString &Value)
{
	TCHAR *pValue;
	
	pValue = NULL;

	if (pInfo->getCustomSummaryInfo( pKey, pValue) == Acad::eOk )
		Value = CString(pValue);

//	acdbFree(pValue);
}


