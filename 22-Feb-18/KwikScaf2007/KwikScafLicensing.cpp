#include "StdAfx.h"
#include <afxwin.h>
#include <afxwin2.inl>
#include <OAIdl.h>
#include "KwikScafLicensing.h"


CKwikScafLicensing::CKwikScafLicensing(void)
{
}


CKwikScafLicensing::~CKwikScafLicensing(void)
{
}

//---------------------------------------------------------
DWORD CKwikScafLicensing::ExecuteConverterApp(TCHAR* strFunct)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	ULONG rc;
	
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_FORCEONFEEDBACK;
	StartupInfo.wShowWindow = SW_HIDE;

	if (!CreateProcess( NULL, strFunct, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, 
		NULL, 
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		return GetLastError();		
	}

	DWORD temp = GetLastError();	
	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
	rc = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);
	return rc;
}

bool CKwikScafLicensing::TriggerExe()
{
	//E:\Kalai_Sample_Docs\modeless\Debug
   
	CString strExePath = _T("D:\2013\Projects\GISCAD\GisCAD- DeskTop\Choku-GISCAD\GIS_CAD 2010 - Internal\debug_1\GISCADConverterD.exe");
	//CString strExePath = _T("E:\2014\Kwikscaf License\Debug\Kwikscaf License.exe");
	//strExePath.get
	//CCmdTarget objCmdTarget;
	//obj
	//Execute the converter utility
	//TCHAR* pszCmd = strExePath.GetBuffer(0);
	//BeginWaitCursor();
	//int nRetVal = (int)ExecuteConverterApp(pszCmd);
	//int nRetVal = (int)ExecuteConverterApp(strExePath.GetBuffer(0));

   //Invalidate();
	//RestoreWaitCursor();
	//EndWaitCursor();
	return true;
}