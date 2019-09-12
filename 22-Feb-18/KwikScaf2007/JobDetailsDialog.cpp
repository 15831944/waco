// JobDetailsDialog.cpp : implementation file
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

#include "stdafx.h"
#include "meccano.h"
#include "JobDescriptionInfo.h"
#include "JobDetailsDialog.h"
#include "VersionDefinitions.h"
#include "MeccanoDefinitions.h"
#include "MdiAware.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const	int VERSION_MAJOR_1		= 1;

const	int VERSION_MINOR_4		= 4;
const	int VERSION_MINOR_5		= 5;

const	int VERSION_BUILD_0		= 0;
const	int VERSION_BUILD_1		= 1;
const	int VERSION_BUILD_2		= 2;
const	int VERSION_BUILD_3		= 3;
const	int VERSION_BUILD_4		= 4;
const	int VERSION_BUILD_5		= 5;
const	int VERSION_BUILD_6		= 6;

const int VERSION_CURRENT_MAJOR = VERSION_MAJOR_1;
const int VERSION_CURRENT_MINOR = VERSION_MINOR_5;
const int VERSION_CURRENT_BUILD = VERSION_BUILD_0;

const CString AUST_KBN_FILE_LABEL = _T("~Branch Details File");

/////////////////////////////////////////////////////////////////////////////
// JobDetailsDialogdialog


JobDetailsDialog::JobDetailsDialog(CWnd* pParent /*=NULL*/, JobDescriptionInfo *pJobDescriptionInfo /*=NULL*/)
	: KwikscafDialog(JobDetailsDialog::IDD, pParent)
{
	m_pJobDescriptionInfo = pJobDescriptionInfo;
	m_pJobDescriptionInfo->LoadSummInfo();

		//{{AFX_DATA_INIT(JobDetailsDialog)
	m_sJobBranch = _T("");
	m_sClientAddress = _T("");
	m_sClientCity = _T("");
	m_sClientName = _T("");
	m_sClientZip = _T("");
	m_sDraftsmanName = _T("");
	m_sFaxCountryCode = _T("");
	m_sFaxNumber = _T("");
	m_sFaxStateCode = _T("");
	m_sMarketingEnquiryNumber = _T("");
	m_sPhoneCountryCode = _T("");
	m_sPhoneNumber = _T("");
	m_sPhoneStateCode = _T("");
	m_sProjectDuration = _T("");
	m_sRefNumber = _T("");
	m_sJobTitle = _T("");
	m_ctStartDate = COleDateTime::GetCurrentTime();
	m_sClientCompany = _T("");
	m_sJobDescription = _T("");
	//}}AFX_DATA_INIT

	m_sJobTitle					= m_pJobDescriptionInfo->m_sJobTitle;
	m_sJobBranch				= m_pJobDescriptionInfo->m_sJobBranch;
	m_sClientAddress			= m_pJobDescriptionInfo->m_sClientAddress;
	m_sClientCity				= m_pJobDescriptionInfo->m_sClientCity;
	m_sClientName				= m_pJobDescriptionInfo->m_sClientName;
	m_sClientCompany			= m_pJobDescriptionInfo->m_sClientCompany;
	m_sClientZip				= m_pJobDescriptionInfo->m_sClientZip;
	m_sJobDescription			= m_pJobDescriptionInfo->m_sJobDescription;
	m_sDraftsmanName			= m_pJobDescriptionInfo->m_sDraftsmanName;
	m_sFaxCountryCode			= m_pJobDescriptionInfo->m_sFaxCountryCode;
	m_sFaxNumber				= m_pJobDescriptionInfo->m_sFaxNumber;
	m_sFaxStateCode				= m_pJobDescriptionInfo->m_sFaxStateCode;
	m_sMarketingEnquiryNumber	= m_pJobDescriptionInfo->m_sMarketingEnquiryNumber;
	m_sPhoneCountryCode			= m_pJobDescriptionInfo->m_sPhoneCountryCode;
	m_sPhoneNumber				= m_pJobDescriptionInfo->m_sPhoneNumber;
	m_sPhoneStateCode			= m_pJobDescriptionInfo->m_sPhoneStateCode;
	m_sProjectDuration			= m_pJobDescriptionInfo->m_sProjectDuration;
	m_sRefNumber				= m_pJobDescriptionInfo->m_sRefNumber;
	m_ctStartDate.ParseDateTime(m_pJobDescriptionInfo->m_sStartDate, VAR_DATEVALUEONLY ); // convert date only
}


void JobDetailsDialog::DoDataExchange(CDataExchange* pDX)
{
	KwikscafDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JobDetailsDialog)
	DDX_Control(pDX, IDC_JOB_BRANCH_COMBO, m_cmboBrachName);
	DDX_Text(pDX, IDC_JOB_BRANCH_COMBO, m_sJobBranch);
	DDX_Text(pDX, IDC_JOB_CLIENT_ADDR_EDIT, m_sClientAddress);
	DDX_Text(pDX, IDC_JOB_CLIENT_EDIT, m_sClientName);
	DDX_Text(pDX, IDC_JOB_CLIENT_ZIP_EDIT, m_sClientZip);
	DDX_Text(pDX, IDC_JOB_DRAFTSMAN_EDIT, m_sDraftsmanName);
	DDX_Text(pDX, IDC_JOB_FAX_NUMBER_EDIT, m_sFaxNumber);
	DDX_Text(pDX, IDC_JOB_MARKETING_NUM_EDIT, m_sMarketingEnquiryNumber);
	DDX_Text(pDX, IDC_JOB_PHONE_EDIT, m_sPhoneNumber);
	DDX_Text(pDX, IDC_JOB_PROJECT_DURATION_EDIT, m_sProjectDuration);
	DDX_Text(pDX, IDC_JOB_REF_NUMBER_EDIT, m_sRefNumber);
	DDX_Text(pDX, IDC_JOB_TITLE_EDIT, m_sJobTitle);
	DDX_DateTimeCtrl(pDX, IDC_JOB_START_DATETIMEPICKER, m_ctStartDate);
	DDX_Text(pDX, IDC_JOB_CLIENT_COMPANY_EDIT, m_sClientCompany);
	DDX_Text(pDX, IDC_JOB_DESCRIPTION_EDIT, m_sJobDescription);
	//}}AFX_DATA_MAP

	m_pJobDescriptionInfo->m_sJobTitle					= m_sJobTitle;
	m_pJobDescriptionInfo->m_sJobBranch					= m_sJobBranch;
	m_pJobDescriptionInfo->m_sClientAddress				= m_sClientAddress;
	m_pJobDescriptionInfo->m_sClientCity				= m_sClientCity;
	m_pJobDescriptionInfo->m_sClientName				= m_sClientName;
	m_pJobDescriptionInfo->m_sClientCompany				= m_sClientCompany;
	m_pJobDescriptionInfo->m_sClientZip					= m_sClientZip;
	m_pJobDescriptionInfo->m_sJobDescription			= m_sJobDescription;
	m_pJobDescriptionInfo->m_sDraftsmanName				= m_sDraftsmanName;
	m_pJobDescriptionInfo->m_sFaxCountryCode			= m_sFaxCountryCode;
	m_pJobDescriptionInfo->m_sFaxNumber					= m_sFaxNumber;
	m_pJobDescriptionInfo->m_sFaxStateCode				= m_sFaxStateCode;
	m_pJobDescriptionInfo->m_sMarketingEnquiryNumber	= m_sMarketingEnquiryNumber;
	m_pJobDescriptionInfo->m_sPhoneCountryCode			= m_sPhoneCountryCode;
	m_pJobDescriptionInfo->m_sPhoneNumber				= m_sPhoneNumber;
	m_pJobDescriptionInfo->m_sPhoneStateCode			= m_sPhoneStateCode;
	m_pJobDescriptionInfo->m_sProjectDuration			= m_sProjectDuration;
	m_pJobDescriptionInfo->m_sRefNumber					= m_sRefNumber;
	m_pJobDescriptionInfo->m_sStartDate					= m_ctStartDate.Format(VAR_DATEVALUEONLY);

	int i, iIndex;

	for( iIndex=0; iIndex<m_saBranchDetails[BDI_NAME].GetSize(); iIndex++ )
	{
		if( m_saBranchDetails[BDI_NAME].GetAt(iIndex)==m_sJobBranch )
			break;
	}
	if( iIndex<m_saBranchDetails[BDI_NAME].GetSize() )
	{
		for( i=0; i<BDI_SIZE; i++ )
		{
			m_pJobDescriptionInfo->m_sBranchDetails[i] = m_saBranchDetails[i].GetAt(iIndex);
		}
	}

	m_pJobDescriptionInfo->StoreSummaryInfo();
}


BEGIN_MESSAGE_MAP(JobDetailsDialog, KwikscafDialog)
	//{{AFX_MSG_MAP(JobDetailsDialog)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// JobDetailsDialog message handlers

void JobDetailsDialog::OnOK() 
{
	KwikscafDialog::OnOK();
}

void JobDetailsDialog::OnMove(int x, int y) 
{
	KwikscafDialog::OnMove(x, y);
	StoreWindowPositionInReg( _T("JobDetailsDialog") );
}

BOOL JobDetailsDialog::OnInitDialog() 
{
	KwikscafDialog::OnInitDialog();
	GetWindowPositionInReg(_T("JobDetailsDialog"));

	LoadBranchNames();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void JobDetailsDialog::LoadBranchNames()
{
	UINT		uiMode;
	CString		sFilename;
	CStdioFile	File;

	uiMode = CFile::modeRead|CFile::typeText;

	//read the filename from the registry
	sFilename = ::GetStringInRegistry(PROJECT_BRANCH_NAME_FILENAME_SECTION);

	//try the default filename
	if( sFilename.IsEmpty() || !File.Open( sFilename, uiMode ) )
	{
		///////////////////////////////////////////////////
		//Get the file name to use from the user
		CFileDialog dbox(TRUE, EXTENTION_BRANCH_NAME_FILE, sFilename,
					OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, FILTER_BRANCH_NAME_FILE ); //, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, acedGetAcadFrame() /*CWnd* pParentWnd = NULL*/ );
		dbox.m_ofn.lpstrTitle = TITLE_BRANCH_NAME_FILE;
		gbOpenFileDialogActive = true;             // this is to prevent infinte looping in 95/98 see Bug 301
		if (dbox.DoModal()!=IDOK)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			gbOpenFileDialogActive = false;
			return;
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
			MessageBox( sMessage, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );
			::SetStringInRegistry( PROJECT_BRANCH_NAME_FILENAME_SECTION, _T("") );
			return;
		}

		//store the filename in the registry
		::SetStringInRegistry( PROJECT_BRANCH_NAME_FILENAME_SECTION, sFilename );
	}

	int		i;
	CString sLine, sBranchDetails[BDI_SIZE], sName,
			sFileType, sMajor, sMinor, sBuild, sErrMsg;

	for( i=0; i<BDI_SIZE; i++ )
	{
		m_saBranchDetails[i].RemoveAll();
	}

	while( File.ReadString( sLine ) )
	{
		//Ignor comments
		if( sLine[0]==_T('!') )
			continue;

		if( sLine[0]==_T('~') )
		{
			int iMajor, iMinor, iBuild;
			ExtractCSVField( sLine, sFileType );
			ExtractCSVField( sLine, sMajor );
			ExtractCSVField( sLine, sMinor );
			ExtractCSVField( sLine, sBuild );

			if( sFileType!=AUST_KBN_FILE_LABEL ||
				!_istdigit(sMajor[0]) || !_istdigit(sMinor[0]) || !_istdigit(sBuild[0]) )
			{
				;//assert( false );

				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
				sErrMsg.Format( _T("The file %s does not appear to be an Austrailan Branch Details File,\n"), sFilename );
				sErrMsg+= _T("\nPlease locate the correct file");
				MessageBox( sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				::SetStringInRegistry( PROJECT_BRANCH_NAME_FILENAME_SECTION, _T("") );
				LoadBranchNames();
				return;
			}

			iMajor = _ttoi( sMajor );
			iMinor = _ttoi( sMinor );
			iBuild = _ttoi( sBuild );
			if( iMajor!=VERSION_CURRENT_MAJOR ||
				iMinor!=VERSION_CURRENT_MINOR ||
				iBuild!=VERSION_CURRENT_BUILD )
			{
				;//assert( false );

				//display error message
				MessageBeep(MB_ICONEXCLAMATION);
        sErrMsg.Format( _T("Incorrect version number for the file: %s\nThis file is marked as version : %i.%i.%i\nHowever, this ARX requires version: %i.%i.%i"),
								sFilename, iMajor, iMinor, iBuild,
								VERSION_CURRENT_MAJOR, VERSION_CURRENT_MINOR, VERSION_CURRENT_BUILD );
				sErrMsg+= _T("\nPlease locate the correct file.");
				MessageBox( sErrMsg, ERROR_MSG_TITLE_NOT_OPEN_FILE_WRITE, MB_OK );

				::SetStringInRegistry( PROJECT_BRANCH_NAME_FILENAME_SECTION, _T("") );
				LoadBranchNames();
				return;
			}
			continue;
		}

		//Read the data from this line of the csv file
		//	we expect to find BDI_SIZE number of feilds on this line
		for( i=0; i<BDI_SIZE; i++ )
		{
			sBranchDetails[i].Empty();
			//extract one field from the csv line
			ExtractCSVField( sLine, sBranchDetails[i] );
			//trim excess spaces
			sBranchDetails[i].TrimLeft();
			sBranchDetails[i].TrimRight();
			//we must also add empty string, otherwise the index will be wrong
			m_saBranchDetails[i].Add(sBranchDetails[i]);
		}
	}

	m_cmboBrachName.ResetContent();
	int iIndex, iCurSel;
	iCurSel = -1;
	for( i=0; i<m_saBranchDetails[BDI_NAME].GetSize(); i++ )
	{
		sName = m_saBranchDetails[BDI_NAME].GetAt(i);
		iIndex = m_cmboBrachName.AddString( sName );
		m_cmboBrachName.SetItemData( iIndex, (DWORD)i );
		
		if( sName==m_sJobBranch )
			iCurSel = iIndex;
	}
	if( iCurSel>=0 )
		m_cmboBrachName.SetCurSel( iCurSel );
}

bool JobDetailsDialog::ExtractCSVField(CString &sCSVData, CString &sField)
{
	sField.Empty();
	if( sCSVData.IsEmpty() )
		return false;

	int iComma	= sCSVData.Find( COMMA );
	if( iComma<0 )
	{
		sField = sCSVData;
		sCSVData.Empty();
		return true;
	}

	sField		= sCSVData.Left( iComma );
	sCSVData	= sCSVData.Right( sCSVData.GetLength()-(iComma+1) );

	return true;
}
