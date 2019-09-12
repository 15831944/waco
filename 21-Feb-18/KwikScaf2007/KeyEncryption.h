#pragma once
#include <string>
#include <stdexcept>
class CKeyEncryption
{
public:
	CKeyEncryption(void);
	~CKeyEncryption(void);
	char * encrypt(char *PASSWORD, int *randNO);
	char * Decrypt(char *Key, int *randNO);
	/*public:
		static XmlDocument *getXmlData(const std::wstring &path);

		static XmlDocument *EncryptXmlData(const std::wstring &path);

	private:
		static void Decrypt(XmlDocument *doc);

	public:
		static void Encrypt(XmlDocument *doc);*/
};

//#pragma once



// ***********************************************************************
// Assembly         : Encryption
// Author           : Rajendra
// Created          : 05-23-2013
//
// Last Modified By : Rajendra
// Last Modified On : 05-23-2013
// ***********************************************************************
// <copyright file="ClsExtention.cs" company="Microsoft">
//     Microsoft. All rights reserved.
// </copyright>
// <summary></summary>
// ***********************************************************************

//namespace Encryption
//{
	/*class ClsExtention final
	{
	public:
		static XmlDocument *getXmlData(const std::wstring &path);

		static XmlDocument *EncryptXmlData(const std::wstring &path);

	private:
		static void Decrypt(XmlDocument *doc);

	public:
		static void Encrypt(XmlDocument *doc);

	};*/
//}
