#pragma once
#include <iostream>
using std :: cout;
using std :: endl;

class CEncryption
{
public:
	CEncryption(void);
	~CEncryption(void);
	void encrypt( char [ ] ); // prototypes of functions used in the code
    void decrypt( char * ePtr );
};

