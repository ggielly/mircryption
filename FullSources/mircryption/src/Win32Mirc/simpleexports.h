// simpleexports.h
// some very simple functions that dont use key file and can be called from
//  other exes (not mirc)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// To avoid multiple header instantiations
#ifndef SIMPLEEXPORTSh
#define SIMPLEEXPORTSh
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "mirc_dllshell.h"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// decrypt and encrypt
// you can EITHER pass a (char*) to hold the result along with a maxresultlen which indicated maximum size of result
//  OR you can pass NULL as the (char*) for result, and delete the returned char* using free() or by calling FreeResultString.
DllExport char* __stdcall DecryptString(char *password,char *cryptedtext,char *decryptedtext,int maxresultlen);
DllExport char* __stdcall EncryptString(char *password,char *plaintext,char *encryptedtext,int maxresultlen);
DllExport void __stdcall FreeResultString(char *text);
//
DllExport char* __stdcall GetVersionString();
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

