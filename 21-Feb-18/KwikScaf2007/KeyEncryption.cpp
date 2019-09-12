#include "StdAfx.h"
#include "KeyEncryption.h"


CKeyEncryption::CKeyEncryption(void)
{
}


CKeyEncryption::~CKeyEncryption(void)
{
}

char * encrypt(char *PASSWORD, int *randNO)        //encrypt function body
{
     int i=0, j=0, k=0;
     int count=0;
     char alphanum[] ="0123456789""abcdefghijklmnopqrstuvwxyz""ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      
     count=strlen(PASSWORD);
      
     for(j=0;j<count;j++)
     randNO[j]=10+rand()%(10000-10+1);
 
 
     printf("Your password is : ");
     puts(PASSWORD);
     printf("Encryption in progress...\n");
      
     for(i=0;i<count;i++)
     {
        PASSWORD[i] = alphanum[randNO[i]%(sizeof(alphanum) - 1)];    //encryptin
	 }
     PASSWORD[count]='\0';
     puts(PASSWORD);        //encrypted value will be printed here
      
     return PASSWORD;
}

char * decrypt(char *PASSWORD, int *randNO)        //decrypt function body
{
     int i=0, j=0, k=0;
     int count=0;
     char alphanum[] ="0123456789""abcdefghijklmnopqrstuvwxyz""ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      
     count=strlen(PASSWORD);
     printf("Your encrypted password is : ");
     puts(PASSWORD);
      
     printf("Decryption in progress...\n");
      
     for(i=0;i<count;i++)
     {
        PASSWORD[i] = alphanum[randNO[i]%(sizeof(alphanum) + 1)];
         
       
     }
     PASSWORD[count]='\0';
     puts(PASSWORD);
      
     return PASSWORD;
}