//---------------------------------------------------------------------------
// simpleexports.cpp
// some very simple functions that dont use key file and can be called from
//  other exes (not mirc)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "simpleexports.h"
#include "..\oldblowfish1.h"
#include "malloc.h"
#include "mircryption.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// external functions from mc_blowfish.cpp
extern char *encrypt_string(char *key, char *str);
extern char *decrypt_string(char *key, char *str);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
DllExport char* __stdcall DecryptString(char *password,char *cryptedtext,char *decryptedtext,int maxresultlen)
{
	// decrypt the string
	// for safety, we will not fill decryptedtext with more characters than specified in maxresultlen
	// if decryptedtext=NULL then we will allocate the space and caller should release
	// note you should NOT pass the +OK or mcps prefix
	// returns pointer to result

	int length;
	char* resultstr;

	// truncated password at MAXKEYBYTES_COMPATMODE bytes
	if (strlen(password)>MAXKEYBYTES_COMPATMODE)
		password[MAXKEYBYTES_COMPATMODE]='\0';

	// make sure we have enough space (we decrease by one since we add a '\0')
	if (maxresultlen==0)
		{
		// encryptedtext better be NULL or else this function will have no effect
		}
	else
		--maxresultlen;

	// decrypt (allocates space)
	resultstr = decrypt_string(password, cryptedtext);

	// length of result
	length=strlen(resultstr);

	// copy to user's result
	if (decryptedtext==NULL)
		{
		// just return the allocated space
		decryptedtext=resultstr;
		}
	else
		{
		// make sure we dont try to copy too much
		if (length>maxresultlen)
			{
			strncpy(decryptedtext,resultstr,maxresultlen);
			decryptedtext[maxresultlen]='\0';
			}
		else
			strcpy(decryptedtext,resultstr);			
		// now free the memory allocated in the encrypt call
		delete [] resultstr;
		}
		
	// return pointer to result
	return decryptedtext;
}



DllExport char* __stdcall EncryptString(char *password,char *plaintext,char *encryptedtext,int maxresultlen)
{
	// decrypt the string
	// for safety, we will not fill encryptedtext with more characters than specified in maxresultlen
	// if decryptedtext=NULL then we will allocate the space and caller should release
	// note this function will NOT add +OK or mcps as a prefix
	// returns pointer to result
	int length;
	char* resultstr;

	// truncated password at MAXKEYBYTES_COMPATMODE bytes
	if (strlen(password)>MAXKEYBYTES_COMPATMODE)
		password[MAXKEYBYTES_COMPATMODE]='\0';

	// make sure we have enough space (we decrease by one since we add a '\0')
	if (maxresultlen==0)
		{
		// encryptedtext better be NULL or else this function will have no effect
		}
	else
		--maxresultlen;

	// encrypt (allocates space)
	resultstr = encrypt_string(password, plaintext);
	
	// length of result
	length=strlen(resultstr);
	
	// copy to user's result
	if (encryptedtext==NULL)
		{
		// just return the allocated space
		encryptedtext=resultstr;
		}
	else
		{
		// make sure we dont try to copy too much
		if (length>maxresultlen)
			{
			strncpy(encryptedtext,resultstr,maxresultlen);
			encryptedtext[maxresultlen]='\0';
			}
		else
			strcpy(encryptedtext,resultstr);			
		// now free the memory allocated in the encrypt call
		delete [] resultstr;
		}
		
	// return pointer to result
	return encryptedtext;
}


DllExport void __stdcall FreeResultString(char *text)
{
	// free some memory allocated by us (for example if you call EncryptString or DecryptString with NULL result buffers
	delete [] text;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
DllExport char* __stdcall GetVersionString()
{
	// return char* to version of dll
	return (MIRCDLL_VERSION);
}
//---------------------------------------------------------------------------

