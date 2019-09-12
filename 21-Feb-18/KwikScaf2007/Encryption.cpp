#include "StdAfx.h"
#include "Encryption.h"


CEncryption::CEncryption(void)
{
}


CEncryption::~CEncryption(void)
{
}

//encrypt data
void CEncryption::encrypt (char e[] )
{
for( int i=0; e[i] != '\0'; ++i ) ++e[i];
} // encrypt

//decrypt data
void CEncryption::decrypt( char * ePtr ) {
for( ; * ePtr != '\0'; ++ ePtr ) --(* ePtr);
}