//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#ifndef JOB_DESCRIPTION_INFO
#define JOB_DESCRIPTION_INFO

#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afx.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif

enum BranchDetailsIndexEnum
{
	BDI_COUNTRY,
	BDI_STATE,
	BDI_NUMBER,
	BDI_NAME,
	BDI_EMAIL,
	BDI_ADDRESS1,
	BDI_ADDRESS2,
	BDI_ADDRESS3,
	BDI_POSTAL1,
	BDI_POSTAL2,
	BDI_POSTAL3,
	BDI_PHONE,
	BDI_FAX,
	BDI_MANAGER,
	BDI_EXTRA1,
	BDI_EXTRA2,
	BDI_SIZE
};


class CArchive;
class AcDbDatabaseSummaryInfo;

class JobDescriptionInfo 
{
public:
	void StoreSummaryInfo();
	void LoadSummInfo();
	JobDescriptionInfo();
	void Serialize(CArchive &ar);

	CString	m_sJobTitle;
	CString	m_sJobBranch;
	CString	m_sClientAddress;
	CString	m_sClientCity;
	CString	m_sClientName;
	CString	m_sClientCompany;
	CString	m_sClientZip;
	CString	m_sJobDescription;
	CString	m_sDraftsmanName;
	CString	m_sFaxCountryCode;
	CString	m_sFaxNumber;
	CString	m_sFaxStateCode;
	CString	m_sMarketingEnquiryNumber;
	CString	m_sPhoneCountryCode;
	CString	m_sPhoneNumber;
	CString	m_sPhoneStateCode;
	CString	m_sProjectDuration;
	CString	m_sRefNumber;
	CString m_sStartDate;

	CString m_sDeliveryInstructions;
	CString m_sDeliveryTime;
	CString	m_bDrawingSent;
	CString m_sBranchDetails[BDI_SIZE];

protected:
	void AddCustomInfo(AcDbDatabaseSummaryInfo *pInfo, TCHAR *pKey, CString *pValue);
	void GetCustomInfo(AcDbDatabaseSummaryInfo *pInfo, TCHAR *pKey, CString &Value);

};

#endif // JOB_DESCRIPTION_INFO