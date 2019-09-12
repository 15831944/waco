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
#include <windows.h>
//#include <QSettings>
#include "IniReader.h"
//#using <mscorlib.dll>
//#include "Encryption.h"
//System.Reflection::Assembly
#include <iostream>
//#include <winsock2.h>
//#include <iphlpapi.h>
//#include <WinBase.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
//#include "SimpleIni\SimpleIni.h"
//#include "SimpleIni.h"
#include <stdlib.h>
//#include <rpc.h>
//#include Rpcdce.h
//#include <stat>
//#include <process>
//#include <types.h>
//io.h
//types.h
//fcntl.h
//process.h

#include <rpc.h>
#include <iostream>
#include <Windows.h>
#include <Winsvc.h>
#include "string.h"
#include "stdafx.h"
#include "cderr.h" 
//#ifdef VALIDATOR
#include "shellapi.h"
#include    "Psapi.h"

//#include <iphlpapi.h>
//#pragma comment(lib, "IPHLPAPI.lib")

//#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
//#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

//#include <unistd.h>  
//using std :: cout;
//using std :: endl;
/* use these classes for this License */
#include    "SentinelKeys.h"            /*  Header file for the Sentinel Keys client library  */
#include    "SentinelKeysLicense.h"     /*  Header file for this License  */
#include    "Strsafe.h"

//#include <CkService.h>
//#include <CkServiceW.h>
#include "KwikScafLicensing.h"
#include <process.h>
#include <windows.h>

#include <tlhelp32.h>
#include <vector>
#include <iostream>

using namespace std;

#include <tchar.h>
//#import "System.Security.dll"
//using namespace system.security;
//#define MACADDR_SIZE (6*3)
//#pragma comment(lib, "advapi32.lib")
//#using <System.Security.dll>
//#using <System.Xml.dll>

// Import the type library.
//#import "E:\2014\sKeyEncryption\sKeyEncryption\bin\Debug\MsKeyEncryption.tlb" raw_interfaces_only
//using namespace std;
//UNICODE;_UNICODE
//_WIN32_WCE=$(CEVER);UNDER_CE;$(PLATFORMDEFINES);WINCE;$(ARCHFAM);$(_ARCHFAM_)

//using namespace sKeyEncryption;
//using System.Globalization;
//#ifdef _MSC_VER
//using namespace std;
//#endif
//[DllImport("C:\\Program Files (x86)\\Reference Assemblies\\Microsoft\\Framework\\.NETFramework\\v4.0\\System.Security.dll")]
//Assembly^ SampleAssembly;
bool CheckForSentinelKey()
{
//	LPCSTR ZXml_Path = "C:\\Program Files\\Autodesk\\License.xml";
//	LPCSTR ZExe_Path = "C:\\Program Files\\Autodesk\\KwikscafApplication.exe";
//  //DWORD ftyp = GetFileAttributesA(ZdirName_in.c_str());
//	DWORD ftyp_xml = GetFileAttributesA(ZXml_Path);
//	DWORD ftyp_exe = GetFileAttributesA(ZExe_Path);
//	if (ftyp_xml == INVALID_FILE_ATTRIBUTES || ftyp_exe == INVALID_FILE_ATTRIBUTES)
//	{
//		MessageBoxA(NULL, ("License.xml or KwikscafApplication.exe file Not Found!..Please Contact Administrator."), ("File Not Found Error!"),MB_OK);
//		return false;  //something is wrong with your path!
//	}
//
//	HWND hwnd;
//	/*ShellExecute(hwnd, NULL, _T("E:\2014\Kwikscaf License\Debug\\Kwikscaf License.exe"), NULL, NULL, SW_HIDE );
//	CloseWindow(hwnd);*/
//	HINSTANCE hInt =  ShellExecute(hwnd, NULL, _T("C:\\Program Files\\Autodesk\\KwikscafApplication.exe"), NULL, NULL, SW_HIDE );
//	CloseWindow(hwnd);
//	
//	int retval = ::_tsystem( _T("taskkill /F /T /IM KwikscafApplication.exe") );
//	//intptr_t nRetVal = execl("C:\\Windows\\SysWOW64\\taskkill.exe", "taskkill", "/F", "/IM", "C:\\Program Files\\Autodesk\\KwikscafApplication.exe", 0);
//
//	/*void killProcessByName(const char *filename)
//{*/
// //   char *filename = "KwikscafApplication.exe";
//	////WCHAR* fileName = _T("KwikscafApplication.exe");
//	//HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
// //   PROCESSENTRY32 pEntry;
// //   pEntry.dwSize = sizeof (pEntry);
// //   BOOL hRes = Process32First(hSnapShot, &pEntry);
//	////pEntry.szExeFile
// //   while (hRes)
// //   {
//	//	//WCHAR* zExeFile;
//	//	//mbstowcs(zExeFile, pEntry.szExeFile, 256);
// //       if (strcmp(pEntry.szExeFile, filename) == 0)
// //       {
// //           HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
// //                                         (DWORD) pEntry.th32ProcessID);
// //           if (hProcess != NULL)
// //           {
// //               TerminateProcess(hProcess, 9);
// //               CloseHandle(hProcess);
// //           }
// //       }
// //       hRes = Process32Next(hSnapShot, &pEntry);
// //   }
// //   CloseHandle(hSnapShot);
////}
//	
//	//TerminateProcess(HINSTANCE);
//	//terminate();
//	//int retval = ::_tsystem( _T("taskkill /F /T /IM C:\\Program Files\\Autodesk\\KwikscafApplication.exe") );
//	/*HANDLE explorer;
//	explorer = OpenProcess(PROCESS_ALL_ACCESS,false,2120);
//	TerminateProcess(explorer,1);*/
//	/*TerminateProcess(
//  _In_  HANDLE hProcess,
//  _In_  UINT uExitCode
//);*/
//	//system("C:\\Program Files\\Autodesk\\KwikscafApplication.exe");
//	//DWORD GetProcessByFileName(char* name){
//
//	//char* name = "C:\\Program Files\\Autodesk\\KwikscafApplication.exe";
//	//
//	////std::vector<DWORD> SetOfPID;
//
//	////GetProcessID("KwikscafApplication.exe",SetOfPID);   // get all process id's of example.exe
// //  const int maxProcIds = 1024;
// //  DWORD procList[maxProcIds];
// //  DWORD procCount;
// //  char* exeName = "C:\\Program Files\\Autodesk\\KwikscafApplication.exe";
// //  char processName[MAX_PATH];
//
// //  // get the process by name
// //  if (!EnumProcesses(procList, sizeof(procList), &procCount))
// //     MessageBoxA(NULL, ("Can't get the current Process."), ("Error!"),MB_OK);
//
// //  // convert from bytes to processes
// //  procCount = procCount / sizeof(DWORD);
//
// //  // loop through all processes
// //  for (DWORD procIdx=0; procIdx<procCount; procIdx++)
// //  {
// //     // get a handle to the process
// //     HANDLE procHandle = OpenProcess(PROCESS_TERMINATE, FALSE, procList[procIdx]);
//	// /* *if(procHandle != NULL)
//	//  {*/
//	//	  // get the process name
//	//	  GetProcessImageFileName(procHandle, LPWSTR(processName), sizeof(processName));
//	//	  // terminate all pocesses that contain the name
//	//	  if (strstr(processName, exeName))
//	//		 TerminateProcess(procHandle, 0);
//	//	  CloseHandle(procHandle); 
//	//  //}
// //  }
//
//   //Process^ myProcess = gcnew Process;
//	//std::vector<DWORD> SetOfPID;
//	//HANDLE ProcHandle;
//	//GetProcessIdOfThread(ProcHandle);
//	////SetOfPID = GetProcessId(ProcHandle);
//	////SetOfPID
//	//if (SetOfPID.empty())   // Process is not running
//	//{
//	//	MessageBoxA(NULL, ("Process is not running"), ("Error!"),MB_OK);
//	//	//printf("Process is not running\n");   // Or do nothing as you wish.
//	//}
//	//else    // Process is running
//	//{
//	//	for (int i=0;i < SetOfPID.size(); i++)
//	//	{
//	//		printf("Process ID is %d\n", SetOfPID[i]);
//	//		HANDLE hProcess = OpenProcess(
//	//			 PROCESS_ALL_ACCESS,FALSE,SetOfPID[i]);  // get Handles for every found process.
//	//		TerminateProcess (hProcess, 0);     // Should kill the Process with exitcode 0
//	//		CloseHandle(hProcess);
//	//	}
//	//}
//
//	//CString exeName = "C:\\Program Files\\Autodesk\\KwikscafApplication.exe";
// //	exeName.GetLength();
//	//
//	//CString strimage_name;
// //  	DWORD process_id_array[1024];
// //   DWORD bytes_returned;
// //   DWORD num_processes;
// //   HANDLE hProcess;
// //   char image_name[256];
//	//char* exe_name;
//	//exe_name = (char*) (LPCTSTR)exeName;
//
// //   char buffer[256];
// //   int i;
// //   DWORD exitcode;
// //   EnumProcesses(process_id_array, 256*sizeof(DWORD), &bytes_returned);
// //   num_processes = (bytes_returned/sizeof(DWORD));
//
//	////CString szName;
// //   for (i = 0; i < num_processes; i++)
//	//{
// //        //hProcess=OpenProcess(PROCESS_ALL_ACCESS,TRUE,process_id_array[i]);
//	//	hProcess=OpenProcess(PROCESS_TERMINATE,FALSE,process_id_array[i]);
//	//	if(hProcess == NULL)
//	//	{
//	//		DWORD WINAPI GetLastError(void);
//	//	}
//	//	else
//	//	{
//	//		if(GetModuleBaseName(hProcess,0,LPWSTR(image_name),256))
//	//		{
//	//			szName = CString(image_name);
//	//			if(szName.CompareNoCase(exeName) == 0)
//	//			{
//	//			/*if(!stricmp(image_name,name))
//	//			{*/
//	//				TerminateProcess(hProcess, 0);
//	//				//CloseHandle(hProcess);
//	//				//return process_id_array[i];
//	//			//}
//	//			}
//	//		}
//	//		CloseHandle(hProcess);
//	//	}
// //  }
//
//	//TerminateProcess(hProcess,0);
// //   CloseHandle(hProcess); //Close Handle*/
//
//    char* StringName;
//	DWORD dwType = REG_SZ;
//	HKEY hKey = 0;
//	char value[1024];
//	DWORD value_length = 1024;
//	char buf[255] = {0};
//	DWORD dwBufSize = sizeof(buf);
//
//   CString strStatus, strMessage;
//   CRegKey regKey;
//   regKey.Open( HKEY_CURRENT_USER, _T("Software\\Kwikscaf\\Protection") );
//   ULONG len = 255;
//   ULONG len2 = 255;
//   regKey.QueryStringValue( TEXT("status"), strStatus.GetBufferSetLength( len ),  &len );
//
//   strStatus.ReleaseBuffer();
//   if(strStatus.CompareNoCase(_T("Access")) == 0)
//   {
//	   return true;
//   }
//   else
//   {
//	   regKey.QueryStringValue( TEXT("Message"), strMessage.GetBufferSetLength( len2 ),  &len2 );
//       strMessage.ReleaseBuffer();
//	   MessageBox(NULL,strMessage, _T("Security Alert!"), MB_OK);
//	   return false;
//   }
    
  return true;
}
