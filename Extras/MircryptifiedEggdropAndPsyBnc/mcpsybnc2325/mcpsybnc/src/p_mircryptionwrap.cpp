//---------------------------------------------------------------------------
// p_mircryptionwrap.c
// tiny wrappers around mircryption blowfish functins
// mouser 1/15/05
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// application includes
// #include "p_mircryptionwrap.h" <--- DONT INCLUDE THIS ITS ONLY FOR OTHERS
#include "mircryption/mc_blowfish.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// for built in blowfish for default encryption
// note we must make clear these are extern c linkage
extern "C" char * BLOW_stringencrypt(unsigned char *input, unsigned char *key);
extern "C" char * BLOW_stringdecrypt(unsigned char *input, unsigned char *key);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// note this is a c++ file so we explicitly mark these functions as c linkage so the psybnc can call them

extern "C" char *MCPS_stringencrypt(unsigned char *input, unsigned char *key, char *outprefix)
{
	char *resultp;

	// check prefix character which skips encryption
	if (input[0]=='`')
		{
		// skip encryption
		// allocate new buffer with malloc
		char* resultp2 = (char*)(malloc(strlen((char*)input)+1));
		if (resultp2==NULL)
			return resultp2;
		// copy in result
		strcpy(resultp2,(char*)(input+1));
		return resultp2;
		}

	// decide whether to use mircryption alg or built in psy alg. based on key prefix
	//printf("IN MCPS_stringencrypt key='%s' and input = '%s'\n",key,input);
	if (strstr((const char*)key,"mcps:")==(const char*)key  || strstr((const char*)key,"MCPS:")==(const char*)key || strstr((const char*)key,"mcps;")==(const char*)key || strstr((const char*)key,"MCPS;")==(const char*)key || strstr((const char*)key,"mcps-")==(const char*)key || strstr((const char*)key,"MCPS-")==(const char*)key)
		{
		// use new mircryptions algorithms (note mircryption alg will remove the prefix for us
		resultp=encrypt_string((char*)(key), (char*)input);
		strcpy(outprefix,"+OK ");
		}
	else if (strstr((const char*)key,"cbc:")==(const char*)key || strstr((const char*)key,"CBC:")==(const char*)key || strstr((const char*)key,"cbc;")==(const char*)key || strstr((const char*)key,"CBC;")==(const char*)key || strstr((const char*)key,"cbc-")==(const char*)key || strstr((const char*)key,"CBC-")==(const char*)key)
		{
		// use new mircryptions algorithms (note mircryption alg will remove the prefix for us
		resultp=encrypt_string((char*)(key), (char*)input);
		strcpy(outprefix,"+OK ");
		}
	else
		{
		// use default algorithms (we have to use [B] so people can see it)
		resultp=BLOW_stringencrypt(input,key);
		strcpy(outprefix,"[B]");
		}

	// There is one strange detail - mircryption uses new and delete to allocate char* buffers,
	//  but PsyBnc expects to receive a buffer allocated with alloc/free.
	// In testing, PsyBnc doesnt seem to mind deleting a new allocated buffer, leading me to believe that
	//  cygwin, which i tested it under, uses same memory allocation routines, BUT we can't be sure of this
	// So, to be safe, we will now delete[] the buffer created by mircryption routine and return result
	//  in a buffer allocated by alloc.
	//
	// allocate new buffer with malloc
	char* resultp2 = (char*)(malloc(strlen(resultp)+1));
	if (resultp2==NULL)
		return resultp2;
	// copy in result
	strcpy(resultp2,resultp);
	// delete original result
	delete [] resultp;

	// return malloc'd result
	return resultp2;
}


extern "C" char *MCPS_stringdecrypt(unsigned char *input, unsigned char *key, char *inprefix)
{
	char *resultp;
	//printf("IN MCPS_stringdecrypt key='%s' and input = '%s'\n",key,input);
	// decide whether to use mircryption alg or built in psy alg. based on key prefix
	if (strstr((const char*)key,"mcps:")==(const char*)key  || strstr((const char*)key,"MCPS:")==(const char*)key || strstr((const char*)key,"mcps;")==(const char*)key || strstr((const char*)key,"MCPS;")==(const char*)key  || strstr((const char*)key,"mcps-")==(const char*)key || strstr((const char*)key,"MCPS-")==(const char*)key)
		{
		// use new mircryptions algorithms (note mircryption alg will remove the prefix for us
		resultp=decrypt_string((char*)(key), (char*)input);
		strcpy(inprefix,"[ECB] ");
		}
	else if (strstr((const char*)key,"cbc:")==(const char*)key || strstr((const char*)key,"CBC:")==(const char*)key || strstr((const char*)key,"cbc;")==(const char*)key || strstr((const char*)key,"CBC;")==(const char*)key || strstr((const char*)key,"cbc-")==(const char*)key || strstr((const char*)key,"CBC-")==(const char*)key)
		{
		// use new mircryptions algorithms (note mircryption alg will remove the prefix for us
		resultp=decrypt_string((char*)(key), (char*)input);
		strcpy(inprefix,"[CBC] ");
		}
	else
		{
		// use default algorithms (we use [P] here to show its blowfish decryption)
		resultp=BLOW_stringdecrypt(input,key);
		// strcpy(inprefix,"[B] ");
		strcpy(inprefix,"[P] ");
		}

	// There is one strange detail - mircryption uses new and delete to allocate char* buffers,
	//  but PsyBnc expects to receive a buffer allocated with alloc/free.
	// In testing, PsyBnc doesnt seem to mind deleting a new allocated buffer, leading me to believe that
	//  cygwin, which i tested it under, uses same memory allocation routines, BUT we can't be sure of this
	// So, to be safe, we will now delete[] the buffer created by mircryption routine and return result
	//  in a buffer allocated by alloc.
	//
	// allocate new buffer with malloc
	char* resultp2 = (char*)(malloc(strlen(resultp)+1));
	if (resultp2==NULL)
		return resultp2;
	// copy in result
	strcpy(resultp2,resultp);
	// delete original result
	delete [] resultp;

	// return malloc'd result
	return resultp2;
}
//---------------------------------------------------------------------------
