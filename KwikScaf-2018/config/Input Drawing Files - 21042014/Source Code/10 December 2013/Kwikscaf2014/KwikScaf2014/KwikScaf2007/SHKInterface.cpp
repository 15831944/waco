/*
Product Details
Sentinel Keys Version     : 1.0.0  
Developer Details
Developer ID              :  0x99260268

License Details
License Name              :  KwikScaf 2007
License ID                :  0x1D74
Last generated on         :  08/24/2007
Compiler Name             :  ANSI C
Number of Features        :  1

Distributor File Encryption Key : DAFD7993-D5D62E99-6953BE46-89E62EAE
*/

#include "Stdafx.h"

/* use these classes for this License */
#include    "SentinelKeys.h"            /*  Header file for the Sentinel Keys client library  */
#include    "SentinelKeysLicense.h"     /*  Header file for this License  */

bool CheckForSentinelKey()
{
  /* Variables for the Sentinel Keys API */
  SP_DWORD     status;                 /* The Sentinel Keys API return codes */

  /* Sentinel Keys Key Information */
  SP_DWORD    DeveloperID;            /* Developer ID of Sentinel Keys key */
  SP_DWORD    flags;                  /* flags for License  */

  SP_HANDLE   licHandle;              /* Handle for License  */

  /* Variables for the AES Feature */
  SPP_BYTE     plainBuffer;           /* A Pointer for simple data */
  SPP_BYTE     cipherBuffer;          /* A pointer for encrypted data */   
  SP_DWORD     flag_AES;	            /* To set the counter flag on/off */   
  SP_DWORD     querylength_AES;       /* The length of query buffer */
  SPP_BYTE     queryValue_AES;        /* A pointer for query buffer */
  SP_DWORD     responseLength_AES;    /* The length of query buffer */  
  SPP_BYTE     response_AES;          /* A pointer for the response buffer */


  /* Acquire a license from the Sentinel Keys key using the SFNTGetLicense API function. It ensures
  the license is acquired from a key containing the requested license ID by validating the License value
  in the key. Refer to the License class file generated for this License. */

  /* The SentinelKey.h header file provides the following macros to set the Communcation flag:
  #define SP_TCP_PROTOCOL         1
  #define SP_IPX_PROTOCOL         2
  #define SP_NETBEUI_PROTOCOL     4
  #define SP_STANDALONE_MODE      32
  #define SP_SERVER_MODE          64
  #define SP_SHARE_ON             128
  #define SP_LEN_OF_QR            112 */

  DeveloperID = 0x99260268;
  flags = SP_STANDALONE_MODE;

  status = SFNTGetLicense(DeveloperID, SOFTWARE_KEY, LICENSEID, flags, &licHandle);
  if (status != SP_SUCCESS)
  {
    /*
    If a key with the requested developer ID and license ID is not found or a valid license is not
    available, then SFNTGetLicense will return an error.
    */
    CString csError;
    csError.Format(_T("Security Initialization Error %ld!\nPlease check if the hardware lock is plugged into the USB port."), status);
    ::MessageBox(::GetActiveWindow(), csError, _T("KwikScaf 2007"), MB_ICONSTOP | MB_OK);
    return false;
  }

  ///* Release the acquired license using the SFNTReleaseLicense API. */
  status = SFNTReleaseLicense(licHandle);
  if (status != SP_SUCCESS)
  {
    /*
    If the license is already released , then this API will return error. 
    */
    CString csError;
    csError.Format(_T("Security Release Error %ld!\nPlease check if the hardware lock is plugged into the USB port."), status);
    ::MessageBox(::GetActiveWindow(), csError, _T("KwikScaf 2007"), MB_ICONSTOP | MB_OK);
    return false;
  }

  // Success
  return true;
}
