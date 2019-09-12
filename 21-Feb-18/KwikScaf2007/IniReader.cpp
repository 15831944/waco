#include "StdAfx.h"
//#include "IniReader.h"

//
//CIniReader::CIniReader(void)
//{
//}
//
//
//CIniReader::~CIniReader(void)
//{
//}


#include "IniReader.h"
#include <iostream>
#include <Windows.h>

CIniReader::CIniReader(char* szFileName)
{
 memset(m_szFileName, 0x00, 255);
 memcpy(m_szFileName, szFileName, strlen(szFileName));
}
//int CIniReader::ReadInteger(char* szSection, char* szKey, int iDefaultValue)
//{
// int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
// return iResult;
//}
//float CIniReader::ReadFloat(char* szSection, char* szKey, float fltDefaultValue)
//{
// char szResult[255];
// char szDefault[255];
// float fltResult;
// sprintf(szDefault, "%f",fltDefaultValue);
// GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
// fltResult =  atof(szResult);
// return fltResult;
//}
//bool CIniReader::ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue)
//{
// char szResult[255];
// char szDefault[255];
// bool bolResult;
// sprintf(szDefault, "%s", bolDefaultValue? "True" : "False");
 /*GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
 bolResult =  (strcmp(szResult, "True") == 0 || 
		strcmp(szResult, "true") == 0) ? true : false;
 return bolResult;
}*/
std::string CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
{
std::string szResult;
 //memset(szResult, 0x00, 255);
GetPrivateProfileString((LPCWSTR)szSection, (LPCWSTR)szKey, (LPCWSTR)szDefaultValue,
	(LPWSTR)szResult.c_str(), 255, (LPCWSTR)m_szFileName); 
	return szResult;
}

//std::string CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
//{
//	//char* szResult = new char[255];
//	std::string szResult;	
//	GetPrivateProfileString((LPCSTR)szSection, (LPCSTR)szKey, (LPCSTR)szDefaultValue, (LPSTR)szResult.c_str(), 255, (LPCSTR)m_szFileName); 
//	return szResult;
//}