#pragma once
//class CIniReader
//{
//public:
//	CIniReader(void);
//	~CIniReader(void);
//};

#ifndef INIREADER_H
#define INIREADER_H
class CIniReader
{
public:
 CIniReader(char* szFileName); 
 
 std::string  ReadString(char* szSection, char* szKey, const char* szDefaultValue);
private:
  char m_szFileName[255];
};
#endif//INIREADER_H
