//---------------------------------------------------------------------------
// mc_blowfish.cpp
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// include header
#include "newblowfish.h"
#include "oldblowfish1.h"
#include "mc_blowfish.h"
#include <string.h>
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Proxies for deciding which algorithm to use based on a key prefix

// encrypt and decrypt
char *encrypt_string(char *key, char *str)
{
	// Note: Returned string must be freed when done with it!
	if (key!=0 && (strncmp(key,"cbc:",4)==0 || strncmp(key,"CBC:",4)==0))
		{
		// new method
		return encrypt_string_new(key+4,str);
		}

	// invoke old ecb method
	return encrypt_string_oldecb(key,str);
}

char *decrypt_string(char *key, char *str)
{
	// Note: Returned string must be freed when done with it!
	if (key!=NULL && (strncmp(key,"cbc:",4)==0 || strncmp(key,"CBC:",4)==0))
		{
		// new method
		if (str[0]=='*')
			return decrypt_string_new(key+4,str+1);
		// it wasnt in cbc as expected, so use old method and warn user
		char *cp=decrypt_string_oldecb(key,str);
		char *cp2 = new char[strlen(cp)+15];
		strcpy(cp2,"ERROR_NONCBC:");
		strcat(cp2,cp);
		delete cp;
		return cp2;
		}
	// invoke old ecb method
	return decrypt_string_oldecb(key,str);
}
//---------------------------------------------------------------------------
