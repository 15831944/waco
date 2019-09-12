#pragma once
class CKwikScafLicensing : CCmdTarget
{
public:
	CKwikScafLicensing(void);
	~CKwikScafLicensing(void);
	DWORD ExecuteConverterApp(TCHAR* strFunct);
	bool TriggerExe();
};

