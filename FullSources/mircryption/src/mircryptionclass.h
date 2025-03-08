//---------------------------------------------------------------------------
// MircryptionClass.h
//
// New C++ class version for mircryption
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// To avoid multiple header instantiations
#ifndef _mircryptionclassh
#define _mircryptionclassh
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Application includes
#include "mc_blowfish.h"
#include "md5class.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Tag marks
#define MCPS2_STARTTAG		"\xABm\xAB"
#define MCPS2_ENDTAG		"\xBBm\xBB"
#define MCPS2_STARTTAGb		"\xC2\xABm\xC2\xAB"
#define MCPS2_ENDTAGb		"\xC2\xBBm\xC2\xBB"
#define MCPS2_CRC			"@@"
//
#define MCP_DefaultFilename	"MircryptionKeys.txt"
//
#define MAXKEYFILENAMELEN	255
#define MAXKEYSIZE			512
#define MAXSAFEKEYSIZE		200
#define MAXCHANNELNAMESIZE	80
//
#define MAXLINELEN			1000
#define MAXLINELENE			2000
#define MAXRETURNSTRINGLEN	2000
#define CRASHLINELEN		650
//
// most functions fill a return string - your passed return string should be able to hold this number of bytes
//
#define MAGICID				"MeowMeowMeowskido"
#define MIRCRYPTIONERROR	"Mircryption_Error"
#define MIRCRYPTIONCLASSVER	"v1.15.01"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Stores keys
class MircryptionKeypair
{
friend class MircryptionClass;
protected:
	char channelname[MAXCHANNELNAMESIZE];
	char key[MAXKEYSIZE];
	MircryptionKeypair *nextp;
public:
	MircryptionKeypair() {strcpy(channelname,""); strcpy(key,""); nextp=NULL;};
	MircryptionKeypair(char *ic,char *ik) {strcpy(channelname,ic); strcpy(key,ik); nextp=NULL;};
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Main mircryption class
class MircryptionClass
{
protected:
	char keyfilename[MAXKEYFILENAMELEN];
	char masterpassphrase[MAXKEYSIZE];
	bool passphrasesunlockedflag;
	MircryptionKeypair *keypairs;
	int keys;
	bool keyfilebackedup;
	int badtries;
	bool foundencryptedchannels;
	bool gavewarning_lockedkeys;
public:
	MircryptionClass();
	~MircryptionClass();
public:
	// Publically exposed functions
	bool mc_encrypt (char *channelname,char *text, char *returndata);
	bool mc_forceencrypt (char *channelname,char *text, char *returndata);
	bool mc_decrypt (char *channelname,char *text, char *returndata);
	//
	bool mc_encrypt2 (char *channelname,char *text, char *returndata);
	bool mc_encrypt2key (char *key, char *text, char *returndata);
	bool mc_decrypt2 (char *channelname,char *text, char *returndata);
	bool mc_decrypt2key (char *key,char *text, char *returndata);
	//
	bool mc_setkey (char *channelname, char *text, char *returndata);
	bool mc_delkey (char *channelname, char *returndata);
	bool mc_disablekey (char *channelname, char *returndata);
	bool mc_enablekey (char *channelname, char *returndata);
	//
	bool mc_displaykey (char *channelname, char *returndata);
	bool mc_listkeys (char *returndata);
	bool mc_setunlockpassphrase (char *passphrase, char *returndata);
	bool mc_setkeyfilename (char *filename, char *returndata);
	//
	bool mc_iskeyfileunlocked (char *returndata);
	bool mc_isencrypting (char *channelname,char *returndata);
	bool mc_isdecrypting (char *channelname,char *returndata);
	//
	bool mc_md5 (char *text, char *returndata);
public:
	// these are mostly internal commands, but they are defined public for ease of use by others
	// key file management
	bool setunlockpassphrase(char *passphrase);
	bool getunlockpassphrase(char *passphrase) {strcpy(passphrase,masterpassphrase); return true;};
	bool unlock_keys();
	bool verify_keysunlocked();
	bool load_keys_ifnotalready();
	bool load_keys();
	void unloadkeys();
	bool backupkeys();
	bool save_keys();
	// channel key routines
	char *lookup_channelkey(char *channelname,char *key,bool wantencrypted);
	// substring decrypt
	void decrypt2_substring(char *insubstring,char *outstring, char *key);
	char *search_decrypt_string(char *inkey, char *instring);
public:
	int countkeys() {return keys;};
public:
	static void repwhitespaces(char *str);
	static void unrepwhitespaces(char *str);
	static char* mystrstr(char *startp,char *substring);
	//static void CleanMultiservChannelName(char *channelname);
	static void stripnicks(char *str);
	static void mylowercasify(char *str);
protected:
	// IMPORTANT - THESE NEED PLATFORM-DEPENDENT IMPLEMENTATION
	virtual char *get_classversionstring();
	virtual bool present_messagebox(char *messagetext,char *windowtitle);
	virtual bool request_unlockpassphrase();
	virtual bool send_irccommand(char *irccommand,char *text);
	virtual bool display_statustext(char *messagetext);
public:
	void bleachmemory();
	void bleachdelete(char *p);
public:
	static int checkpasswordquality(char *key,char *errorstring);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 01/27/06 to prevent evil attempts to crash by sending nick+chan to big
void mcensuresafebuflen(char *buf, int maxlen);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

