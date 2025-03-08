Instructions for calling dll functions from your own code

//---------------------------------------------------------------------------
The mircryption.dll now (as of 10/9/04) exports some functions that let you call the dll from your own program to perform simple encryption and decryption.
The following functions are exported from the dll:
	DllExport char* __stdcall DecryptString(char *password,char *cryptedtext,char *decryptedtext,int maxresultlen);
	DllExport char* __stdcall EncryptString(char *password,char *plaintext,char *encryptedtext,int maxresultlen);
	DllExport void __stdcall FreeResultString(char *text);
You must allocate space to hold the result of the encryption or decryption and pass it in decryptedtext or encryptedtext.  The resulting string will be 0-terminated.  Any password will be truncated at 54 bytes (the max password length used by blowfish 448).
To protect against buffer overruns, you must specify the maxresultlen that your buffer can hold (in decryptedtext or encryptedtext).
Alternatively, you can pass a NULL pointer for decryptedtext or encryptedtext, in which case the memory for that char* will be allocated for you, and you must free it after processing by calling FreeResultString().
//---------------------------------------------------------------------------
