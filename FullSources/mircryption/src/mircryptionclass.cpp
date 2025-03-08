//---------------------------------------------------------------------------
// MircryptionClass.cpp
// New C++ classes for mircryption
//
// http://mircryption.sourceforge.net
// By Dark Raichu, 12/01 - 07/03
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Application includes
#include "mircryptionclass.h"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 01/27/06 to prevent evil attempts to crash by sending nick+chan to big
// thanks to www.rainbowcrack-online.com and ircfuz for finding this dangerous possibility
void mcensuresafebuflen(char *buf, int maxlen) {if (strlen(buf)>=maxlen-10) buf[maxlen-10]='\0';};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
MircryptionClass::MircryptionClass()
{
	// constructor

	// set default keyfile name
	strcpy(keyfilename,MCP_DefaultFilename);
	// mark keyfile as being locked (ie not decrypted)
	passphrasesunlockedflag=false;
	// initiailize keypair linked list
	keypairs=NULL;
	keys=0;
	// remember that we have not backed up keyfile yet
	keyfilebackedup=false;
	// reset count of number of failed tries to unlock keyfile
	badtries=0;
	// when we read password file, we want to know if the channel names were encrypted
	foundencryptedchannels=false;
	// have we warned user yet about locked keys?
	gavewarning_lockedkeys=false;
}

MircryptionClass::~MircryptionClass()
{
	// destructor

	// unload keys if they are loaded - perfunctary as all changes are saved after any change
	unloadkeys();

	// bleach key memory
	bleachmemory();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// PUBLICALLY EXPOSED FUNCTIONS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
bool MircryptionClass::mc_encrypt (char *channelname,char *text, char *returndata)
{
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	lookup_channelkey(channelname,key,false);
	if (key[0]!='\0')
		{
		if (strlen(text)>CRASHLINELEN)
			{
			// text[CRASHLINELEN]='\0';
			sprintf(returndata,"%s. Line not sent. Encryption engine cannot encrypt lines more than %d characters.",MIRCRYPTIONERROR,CRASHLINELEN);
			memset(key,0,MAXKEYSIZE);
			return false;
			}

		// ok, we got a key for this channel, so encrypt it
		if (verify_keysunlocked())
			{
			if (strcmp(key,"_ENCRYPTED_")==0)
				lookup_channelkey(channelname,key,false);	// relook up a prev. encrypted key
			char* p = encrypt_string(key,text);
			sprintf(returndata,"%s",p);
			bleachdelete(p);
			}
		else
			{
			sprintf(returndata,"%s master keyfile is not unlocked.",MIRCRYPTIONERROR);
			memset(key,0,MAXKEYSIZE);
			return false;
			}
		}
	else
		{
		// no key found for this channel, returnvalue=""
		sprintf(returndata,"");
		// should we return true or false?
		memset(key,0,MAXKEYSIZE);
		return false;
		}

	memset(key,0,MAXKEYSIZE);
	return true;
}


bool MircryptionClass::mc_forceencrypt (char *channelname,char *text, char *returndata)
{
	// same as mc_encrypt, but we encrypt even for disabled channels
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	lookup_channelkey(channelname,key,false);
	if (key[0]=='\0')
		{
		// no key for this channel, but check if there is one for the disabled version of this channel
		char tempchannel[MAXCHANNELNAMESIZE+1];
		sprintf(tempchannel,"-%s",channelname);
		// now set key to value of key for '-#channelname'
		lookup_channelkey(tempchannel,key,false);
		}

	if (key[0]!='\0')
		{
		if (strlen(text)>CRASHLINELEN)
			{
			sprintf(returndata,"%s. Line not sent. Encryption engine cannot encrypt lines more than %d characters.",MIRCRYPTIONERROR,CRASHLINELEN);
			memset(key,0,MAXKEYSIZE);
			return false;
			}

		// ok, we got a key for this channel, so encrypt it
		if (verify_keysunlocked())
			{
			if (strcmp(key,"_ENCRYPTED_")==0)
				lookup_channelkey(channelname,key,false);	// relook up a prev. encrypted key
			char* p = encrypt_string(key,text);
			sprintf(returndata,"%s",p);
			bleachdelete(p);
			}
		else
			{
			sprintf(returndata,"%s master keyfile is not unlocked.",MIRCRYPTIONERROR);
			memset(key,0,MAXKEYSIZE);
			return false;
			}
		}
	else
		{
		// no key found for this channel, returnvalue=""
		sprintf(returndata,"");
		// should we return true or false?
		return false;
		}

	memset(key,0,MAXKEYSIZE);
	return true;
}


bool MircryptionClass::mc_decrypt (char *channelname,char *text, char *returndata)
{
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);
	
	lookup_channelkey(channelname,key,false);
	if (key[0]=='\0')
		{
		// no key for this channel, but check if there is one for the disabled version of this channel
		char tempchannel[MAXCHANNELNAMESIZE+1];
		sprintf(tempchannel,"-%s",channelname);
		lookup_channelkey(tempchannel,key,false);
		}

	if (key[0]!='\0')
		{
		// ok, we got a key for this channel, so encrypt it
		if (verify_keysunlocked())
			{
			if (strcmp(key,"_ENCRYPTED_")==0)
				lookup_channelkey(channelname,key,false);	// relook up a prev. encrypted key
			char* p = decrypt_string(key,text);
			sprintf(returndata,"%s",p);
			bleachdelete(p);
			}
		else
			{
			strcpy(returndata,"key found for channel but text could not be decrypted - make sure master passphrase is set.");
			memset(key,0,MAXKEYSIZE);
			return false;
			}
		}
	else
		{
		// no key found for this channel, returnvalue=""
		sprintf(returndata,"");
		// should we return true or false?
		return false;
		}		

	memset(key,0,MAXKEYSIZE);
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// These are new versions of mc_encrypt and mc_decrypt - they work differently than the old ones
// 1. they wrap the text in start_encryption and end_encryption tags
// 2. they add a couple chars before encryption (and remove after) that allows them to confirm succesfull decoding
// 3. decoding will also operate on embeddded *substrings* of encoded text, where only parts of a string are encrypted
// 4. no whitespaces or comas are present in substring-crypted text, which makes it easier to identify crypted substrings
// 5. during decoding, if channel name key fails to decrypt the text, system will try *ALL* stored keys for a successful decode
// All of these features are meant to allow mircryption to better decode text with substrings of encrypted text which can be useful when using mircryption with existing scripts

bool MircryptionClass::mc_encrypt2 (char *channelname,char *text, char *returndata)
{
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);
	
	lookup_channelkey(channelname,key,false);
	if (key[0]!='\0')
		{
/*
// we dont need to worry about this anymore, mirc script will take care of it
		if (strlen(text)>CRASHLINELEN)
			{
			// text[CRASHLINELEN]='\0';
			sprintf(returndata,"%s. Line not sent. Encryption engine cannot encrypt lines more than %d characters.",MIRCRYPTIONERROR,CRASHLINELEN);
			return false;
			}
*/

		// ok, we got a key for this channel, so encrypt it
		if (verify_keysunlocked())
			{
			if (strcmp(key,"_ENCRYPTED_")==0)
				lookup_channelkey(channelname,key,false);	// relook up a prev. encrypted key
			mc_encrypt2key(key,text,returndata);
			}
		else
			{
			sprintf(returndata,"%s master keyfile is not unlocked.",MIRCRYPTIONERROR);
			memset(key,0,MAXKEYSIZE);
			return false;
			}
		}
	else
		{
		// no encryption for this channel, BUT we still return the original text
		sprintf(returndata,"%s",text);
		// should we return true or false? for now we drop down and return true
		}

	memset(key,0,MAXKEYSIZE);
	return true;
}


bool MircryptionClass::mc_encrypt2key (char *key, char *text, char *returndata)
{
	// do new encryption given a key

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	// first encrypt the (crc+text)
	char plaintext[MAXLINELEN];
	strcpy(plaintext,MCPS2_CRC);
	strcat(plaintext,text);
	char* p = encrypt_string(key,plaintext);
	// now write it to data var, with start and end tages
	sprintf(returndata,"%s%s%s",MCPS2_STARTTAG,p,MCPS2_ENDTAG);
	// now convert whitespaces to replacements
	repwhitespaces(returndata);
	// delete encrypted pointer
	bleachdelete(p);
	return true;
}


bool MircryptionClass::mc_decrypt2 (char *channelname,char *text, char *returndata)
{
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (!verify_keysunlocked())
		{
		// cant open decrypted keys, SO warn them first, then return undecrypted text on other runs
		if (gavewarning_lockedkeys)
			sprintf(returndata,"%s",text);	
		else
			{
			sprintf(returndata,"WARNING: Your mircryption keys are not unlocked - no encyrption/decryption will occur until you set your master password.");
			gavewarning_lockedkeys=true;
			}
		memset(key,0,MAXKEYSIZE);
		return false;
		}
		
	lookup_channelkey(channelname,key,false);
	if (key[0]=='\0')
		{
		// we can still decrypt using a disabled ecnryption channel
		char tempchannel[MAXCHANNELNAMESIZE+1];
		sprintf(tempchannel,"-%s",channelname);
		lookup_channelkey(tempchannel,key,false);
		}
	if (key[0]!='\0')
		{
		// ok, we got a key for this channel, so decrypt it
		if (strcmp(key,"_ENCRYPTED_")==0)
			lookup_channelkey(channelname,key,false);	// relook up a prev. encrypted key
		}

	// legacy decrypt of style 1
	if (strncmp(text,"mcps ",5)==0)
		{
		char* p = decrypt_string(key,text+5);
		sprintf(returndata,"%s",p);
		bleachdelete(p);
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		memset(key,0,MAXKEYSIZE);
		return true;
		}

	// should we also decrypt the "OK+" format of blowcrypt or bnc blowfish?
	// THIS WAS CHANGED ON 8/1/03 - it *should* work fine
	//if (false && strncmp(text,"+OK ",4)==0)
	if (strncmp(text,"+OK ",4)==0)
		{
		char* p = decrypt_string(key,text+4);
		sprintf(returndata,"%s",p);
		bleachdelete(p);
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		memset(key,0,MAXKEYSIZE);
		return true;
		}

	// now loop through replacing substrings of encoded text (which may be nonexistent)
	mc_decrypt2key(key,text,returndata);
	memset(key,0,MAXKEYSIZE);
	return true;
}




bool MircryptionClass::mc_decrypt2key (char *key,char *text, char *returndata)
{
	char decodedtext[MAXLINELENE];
	char decodedsubstring[MAXLINELENE];
	char *p,*p2;

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	strcpy(decodedtext,text);
	p=decodedtext;
	for(;;)
		{
		// find leftmost substring
		p=strstr(p,MCPS2_STARTTAG);
		if (p==NULL)
			break;
		p2=strstr(p,MCPS2_ENDTAG);
		if (p2!=NULL)
			{
			// cap substring
			*p2='\0';
			}
		else
			{
			// truncated string
			p2=p+strlen(p);
			}

		// decode substring
		decrypt2_substring(p+strlen(MCPS2_STARTTAG),decodedsubstring,key);
		// add right half of string to decoded substring
		strcat(decodedsubstring,p2+strlen(MCPS2_ENDTAG));
		// cap main string before first tag
		*p='\0';
		// now add left half to right half
		strcat(decodedtext,decodedsubstring);
		// advance p to find next encrypted substring
		// p=decodedtext+strlen(decodedsubstring);
		p=decodedtext+1;
		}

	// kludge fix for xchat, which expands our tags wierdly
	p=decodedtext;
	for(;;)
		{
		// find leftmost substring
		p=strstr(p,MCPS2_STARTTAGb);
		if (p==NULL)
			break;
		p2=strstr(p,MCPS2_ENDTAGb);
		if (p2!=NULL)
			{
			// cap substring
			*p2='\0';
			}
		else
			{
			// truncated string
			p2=p+strlen(p);
			}

		// decode substring
		decrypt2_substring(p+strlen(MCPS2_STARTTAGb),decodedsubstring,key);
		// add right half of string to decoded substring
		strcat(decodedsubstring,p2+strlen(MCPS2_ENDTAGb));
		// cap main string before first tag
		*p='\0';
		// now add left half to right half
		strcat(decodedtext,decodedsubstring);
		// advance p to find next encrypted substring
		//p=decodedtext+strlen(decodedsubstring);
		p=decodedtext+1;
		}

	strcpy(returndata,decodedtext);
	// bleach
	memset(decodedtext,0,MAXLINELENE);
	memset(decodedsubstring,0,MAXLINELENE);
	return true;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MircryptionClass::mc_setkey (char *channelname,char *key,char *returndata)
{
	// either change the value for a key, or add a new keypair
	char dchannelname[MAXCHANNELNAMESIZE];
	MircryptionKeypair *keyp=keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (!verify_keysunlocked())
		{
		strcpy(returndata,"mircryption keys cannot be set until you set the master passphrase.");
		return false;
		}

	// search for an existing key (use dchannelname to search for 'disabled' keys)
	sprintf(dchannelname,"-%s",channelname);
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,channelname)==0 || strcmp(keyp->channelname,dchannelname)==0)
			{
			if (strcmp(keyp->key,key)!=0)
				{
				strcpy(keyp->key,key);
				sprintf(returndata,"mircryption key for %s has been changed to %s.",channelname,key);
				}
			else
				{
				// key was already set to this, so there is nothing for us to do
				sprintf(returndata,"mircryption key for %s was already set to %s.",channelname,key);
				// force it to enable if it is disabled
				strcpy(keyp->channelname,channelname);
				}
			break;
			}
		keyp=keyp->nextp;
		}

	// if no existing key, make a new one
	if (keyp==NULL)
		{
		// add the new key to top of the keypairs linked list
		if (keypairs==NULL)
			{
			// this happens when we are creating our FIRST key of all, which means we didnt read a magicID yet, so our keyscount
			// is 0 when it should be 1 (including magicid key), so we do a little kludge here just for printing.
			keys=1;
			}
		keyp=keypairs;
		keypairs=new MircryptionKeypair(channelname,key);
		keypairs->nextp=keyp;
		sprintf(returndata,"mircryption key '%s' added for channel %s.",key,channelname);
		++keys;
		}

	// save changed keys immediately
	save_keys();
	return true;
}


bool MircryptionClass::mc_delkey (char *channelname,char *returndata)
{
	char dchannelname[MAXCHANNELNAMESIZE];
	MircryptionKeypair *keyp=keypairs;
	MircryptionKeypair **priorp=&keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);


	// search for an existing key (use dchannelname to search for 'disabled' keys)
	sprintf(dchannelname,"-%s",channelname);
	// walk through the linked list and delete the key if we find it
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,channelname)==0 || strcmp(keyp->channelname,dchannelname)==0)
			{
			sprintf(returndata,"mircryption key for %s has been deleted (was %s).",channelname,keyp->key);
			*priorp=keyp->nextp;
			// bleach it before deletion
			memset(keyp->key,0,strlen(keyp->key));
			memset(keyp->channelname,0,strlen(keyp->channelname));
			delete keyp;
			--keys;
			// save changed keys and break
			save_keys();
			break;
			}
		priorp=&(keyp->nextp);
		keyp=keyp->nextp;
		}

	if (keyp==NULL)
		{
		sprintf(returndata,"mircryption key for %s was not found.",channelname);
		return false;
		}

	return true;
}


bool MircryptionClass::mc_disablekey (char *channelname,char *returndata)
{
	char dchannelname[MAXCHANNELNAMESIZE];
	MircryptionKeypair *keyp=keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	// walk through the linked list and disable the key if we find it
	sprintf(dchannelname,"-%s",channelname);
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,channelname)==0)
			{
			sprintf(keyp->channelname,"-%s",channelname);
			sprintf(returndata,"mircryption for %s has been temporarily disabled. type /enablekey to re-enable it.",channelname);
			save_keys();
			break;
			}
		else if (strcmp(keyp->channelname,dchannelname)==0)
			{
			sprintf(returndata,"mircryption for %s is already disabled.",channelname);
			return false;
			}
		keyp=keyp->nextp;
		}

	if (keyp==NULL)
		{
		sprintf(returndata,"mircryption key for %s was not found.",channelname);
		return false;
		}

	return true;
}


bool MircryptionClass::mc_enablekey (char *channelname,char *returndata)
{
	char dchannelname[MAXCHANNELNAMESIZE];
	MircryptionKeypair *keyp=keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	// walk through the linked list and enable the key if we find it
	sprintf(dchannelname,"-%s",channelname);
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,dchannelname)==0)
			{
			sprintf(returndata,"mircryption for %s has been re-enabled.",channelname);
			strcpy(keyp->channelname,channelname);
			save_keys();
			break;
			}
		else if (strcmp(keyp->channelname,channelname)==0)
			{
			sprintf(returndata,"mircryption for %s is already enabled.",channelname);
			return false;
			}
		keyp=keyp->nextp;
		}

	if (keyp==NULL)
		{
		sprintf(returndata,"mircryption key for %s was not found.",channelname);
		return false;
		}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MircryptionClass::mc_displaykey (char *channelname,char *returndata)
{
	char dchannelname[MAXCHANNELNAMESIZE];
	MircryptionKeypair *keyp=keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (!verify_keysunlocked())
		{
		strcpy(returndata,"not displayable until you set the master passphrase.");
		return false;
		}

	// walk through the linked list and disaply the key if we find it
	sprintf(dchannelname,"-%s",channelname);
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,channelname)==0 || strcmp(keyp->channelname,dchannelname)==0)
			{
			sprintf(returndata,"%s",keyp->key);
			break;
			}
		keyp=keyp->nextp;
		}

	if (keyp==NULL)
		{
		strcpy(returndata,"");
		return false;
		}

	return true;
}


bool MircryptionClass::mc_listkeys (char *returndata)
{
	// we use the virtual function display_statustext() rather than return text answer to display many lines

	// unlock keys if not
	if (!verify_keysunlocked())
		{
		present_messagebox("You must set the master passphrase before keys can be accessed (type /mircryption for help).","Attention");
		strcpy(returndata,"");
		return true;
		}

	// intro message
	if (keys==0)
		sprintf(returndata,"no mircryption keys found for any channels.");
	else if (strcmp(masterpassphrase,"")==0)
		sprintf(returndata,"mircryption passphrase is not yet set.  %d Keys:   ",keys-1);
	else
		sprintf(returndata,"mircryption passphrase is '%s'.  %d Keys:   ",masterpassphrase,keys-1);
	display_statustext(returndata);

	// walk through the linked list and print all keys
	MircryptionKeypair *keyp=keypairs;
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,"MAGICID")!=0)
			{
			sprintf(returndata,"  %s -> '%s'",keyp->channelname,keyp->key);
			display_statustext(returndata);
			}
		keyp=keyp->nextp;
		}

	strcpy(returndata,"");
	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MircryptionClass::mc_setunlockpassphrase (char *passphrase, char *returndata)
{
	// set master keyfile password
	bool bretv;

	mcensuresafebuflen(passphrase,MAXSAFEKEYSIZE);

	strcpy(returndata,"");
	// set the master passphrase and try to unlock the global file	
	if (strcmp(passphrase,"?")==0)
		{
		// if we are passed a master passphrase of '?' it means to use function request_unlockpassphrase() to request it interactively
		bretv=request_unlockpassphrase();
		if (!bretv)
			{
			if (passphrasesunlockedflag || keypairs==NULL)
				sprintf(returndata,"Change of mircryption master passphrase canceled.");
			else
				sprintf(returndata,"Mircryption master passphrase not set.");
			return false;
			}
		}
	else
		bretv=setunlockpassphrase(passphrase);

	if (bretv)
		{
		if (passphrasesunlockedflag)
			{
			sprintf(returndata,"MircryptionSuite - Mircryption master passphrase accepted.");
			if (keys>0)
				save_keys();
			}
		}
	else
		{
		sprintf(returndata,"mircryption master passphrase rejected.");
		return false;
		}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MircryptionClass::mc_setkeyfilename (char *filename,char *returndata)
{
	// change the name of the keyfile (unloading and reloading if one is already loaded)
	if (returndata!=NULL)
		strcpy(returndata,"");
	if (strcmp(keyfilename,filename)==0)
		{
		// keyfile name has not changed, so just ignore this
		load_keys_ifnotalready();
		return true;
		}
	strcpy(keyfilename,filename);
	load_keys();
	return true;
}

bool MircryptionClass::mc_iskeyfileunlocked (char *returndata)
{
	// has master keyfile been unlocked?
	if (returndata!=NULL)
		strcpy(returndata,"");
	if (!passphrasesunlockedflag)
		return false;
	return true;
}


bool MircryptionClass::mc_isencrypting (char *channelname,char *returndata)
{
	// is current channel have a key and is encrypting?
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (returndata!=NULL)
		strcpy(returndata,"");
	lookup_channelkey(channelname,key,false);
	if (key[0]=='\0')
		return false;
	memset(key,0,MAXKEYSIZE);
	return true;
}


bool MircryptionClass::mc_isdecrypting (char *channelname,char *returndata)
{
	// is current channel have a key? (ie is decryptable)	
	char key[MAXKEYSIZE];

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (returndata!=NULL)
		strcpy(returndata,"");

	lookup_channelkey(channelname,key,false);
	if (key[0]=='\0')
		{
		// we can still decrypt using a disable ecnryption channel
		char tempchannel[MAXCHANNELNAMESIZE];
		sprintf(tempchannel,"-%s",channelname);
		lookup_channelkey(tempchannel,key,false);
		}

	if (key[0]=='\0')
		return false;
	memset(key,0,MAXKEYSIZE);
	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MircryptionClass::mc_md5 (char *text, char *returndata)
{
	// simple md5 hash of a string
	CMD5 md5;
	md5.setPlainText(text);
	sprintf(returndata,"%s",md5.getMD5Digest());
	return true;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// INTERNAL METHODS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
bool MircryptionClass::setunlockpassphrase(char *passphrase)
	{
	// Set the global master passphrase to enencrypt the key file, and unlock keyfile
	// returns true on successful unlocking
	bool bretv;

	mcensuresafebuflen(passphrase,MAXSAFEKEYSIZE);

	// record master keyfile password
	strcpy(masterpassphrase,passphrase);
	// try to unlock it and load keys
	bretv=unlock_keys();

	if (!bretv)
		{
		// keep track of how many bad tries to open it, so we can warn user
		++badtries;
		if (badtries==4)
			{
			present_messagebox("You poor bastard.  Have you forgotten your passphrase?  I warned you about this, didn't I?\r\n  I'm going to stop asking you for it.  You can delete the MircryptionKeys.txt file to reset all keys and set a new passphrase.","Attention");
			// ATTN: we return true to make the program stop asking for master keyfile - BUT is this a bad idea? willit cause keys to be overwritten?
			bretv=true;
			}
		else if (badtries>4)
			{
			// on subsequent call after failing, we reset the cycle
			badtries=1;
			}
		else
			{
			present_messagebox("Master passphrase does not match that stored in MircryptionKeys.txt file.\r\n  Try again or delete keyfile to erase all keys.","Attention");
			}
		}
	return bretv;
	}


bool MircryptionClass::unlock_keys()
{
	// unlock(uncrypt) keys which are already loaded
	// returns true if keys are unlocked with passphrase
	char key[MAXKEYSIZE];
	char tempstring[MAXLINELENE+1];
	char* p;

	if (foundencryptedchannels)
		{
		// this flag is set when we have just read encrypted channel names.  we need to disable it or our call to lookup_chankey below will recursively call us and fail.
		foundencryptedchannels=false;
		}

	// if already unlocked, then return
	if (passphrasesunlockedflag==true)
		return true;
	// if no master pass set, return false
	if (masterpassphrase[0]=='\0')
		return false;

	if (keys>0)
		{ 
		// if we have loaded keys which have not been decrypted, we need to test unlock key by checking the magic id
		lookup_channelkey("MAGICID",key,true);
		if (strcmp(key,"")==0)
			{
			// no magic identifier means this is not a proper keyfile
			return false;
			}
		// decrypt the magic id
		p = decrypt_string(masterpassphrase,key);
		strcpy(key,p);
		// returned char * from decrypt_string must be deleted
		bleachdelete(p);
		if (strcmp(key,MAGICID)!=0)
			{
			// it does not match, so this is bad master password
			memset(key,0,MAXKEYSIZE);
			return false;
			}

		// master pass is good, so cycle through all keys and unencrypt them
		MircryptionKeypair *keyp=keypairs;
		MircryptionKeypair *keyp2;
		MircryptionKeypair *priorkeyp=NULL;
		while (keyp!=NULL)
			{
			// decrypt it and store the decrypted value
			p = decrypt_string(masterpassphrase,keyp->key);
			strcpy(keyp->key,p);
			bleachdelete(p);
			// is the channel name itself encryped?  if so decrypt it
			mc_decrypt2key(masterpassphrase,keyp->channelname,keyp->channelname);
			strcpy(tempstring,keyp->channelname);
			// force it to lowercase
			if (strcmp(keyp->channelname,"magicid")==0)
				{
				// bug created when we accidentally lowercased magicid, so we want to kill this key and splice it out
				keyp2=keyp->nextp;
				if (priorkeyp!=NULL)
					priorkeyp->nextp=keyp2;
				else
					keypairs=keyp2;
				delete keyp;
				keyp=keyp2;
				--keys;
				continue;
				}
			else if (strcmp(keyp->channelname,"MAGICID")!=0)
				{
				mylowercasify(keyp->channelname);
				// check to see if channel name was not lowercase, which can happen with older keyfiles
				if (strcmp(keyp->channelname,tempstring)!=0)
					{
					// it wasnt lowercase, so this was old channel name, which could have a duplicate
					keyp2=keypairs;
					while (keyp2!=NULL && keyp2!=keyp)
						{
						if (strcmp(keyp->channelname,keyp2->channelname)==0)
							{
							// ok we got a duplicate key, SO we dont want to add both
							strcat(keyp->channelname,"_errorduplicate");
							break;
							}
						keyp2=keyp2->nextp;
						}
					}
				}
			// now loop to next key
			priorkeyp=keyp;
			keyp=keyp->nextp;
			}
		}
	
	// set flag to show the passphrases have been unlocked
	passphrasesunlockedflag=true;
	memset(key,0,MAXKEYSIZE);
	return true;
}


bool MircryptionClass::verify_keysunlocked()
{
	// verify keys are loaded and unlocked.  if keys are locked, try to get user to give passphrase to unlock
	// returns true on success
	bool bretv;

	// check if keys are already unlocked
	if (passphrasesunlockedflag)
		return true;
	if (badtries==4)
		{
		// after 4 bad tries at passphrase, we stop asking; this will reset if user explicitly tries to set passphrase
		return false;
		}

	// try to unlock them
	for (;;)
		{
		// now unlock them if need be
		bretv=unlock_keys();
		if (bretv)
			{
			// we successfully unlocked them
			break;
			}
		if (passphrasesunlockedflag)
			{
			// this can happen if we give up(?)
			break;;
			}
		// ask user for master pass
		bretv=request_unlockpassphrase();
		if (!bretv)
			break;
		// we give up after many bad attempts
		if (badtries==4)
			break;
		}

	// return flag saying whether keys are unlocked
	return passphrasesunlockedflag;
}


bool MircryptionClass::load_keys_ifnotalready()
{
	// load keys if they are not already loaded
	if (!passphrasesunlockedflag)
		load_keys();
	return true;
}


bool MircryptionClass::load_keys()
{
	// load encrypted keys from file
	// returns true on succesfull loading of keys, or if file does not exist
	FILE *fp;
	MircryptionKeypair *keyp;
	char tempstring[MAXLINELENE+1];
	char *p;
	int len;
	int c;
   	MircryptionKeypair *lastkeyp;
	char key[MAXKEYSIZE];

	// unload any that are already loaded
	unloadkeys();
	keypairs=lastkeyp=NULL;
	foundencryptedchannels=false;
	
	// open the file
	if ((fp=fopen(keyfilename,"r"))==(FILE *)NULL)
		{
		// this should only happen if file does not exist
		return true;
		}
	
	// read keys from file
	while (!feof(fp))
		{
		// read a line
		fgets(tempstring,MAXLINELENE,fp);
		tempstring[MAXLINELENE]='\0';
		len=strlen(tempstring)-1;

		// remove cr/lf at end
		while (tempstring[len]==13 || tempstring[len]==10)
			{
			tempstring[len]='\0';
			if (--len<0)
				break;
			}

		// skip comments and empty lines
		if ((tempstring[0]=='/' && tempstring[1]=='/') || tempstring[0]=='\0')
			{
			if (strcmp(tempstring,"// Using Encrypted Channel Names")==0)
				{
				// directive about encrypted channels
				// we dont actually have to detect this, we will pick it up when parsing channel keys
				// however, we do need to remember the channel names are encrypted, so we decrypt them before looking up channels
				foundencryptedchannels=true;
				}
			continue;
			}

		// pick off channel name
		p=tempstring;
		for(;;)
			{
			c=*p;
			if (c==' ' || c=='\0') break;
			++p;
			}
		// providing there is a key for this channel, add the keypair
		if (*p!='\0')
			{
			*p='\0';
			++p;
			if (strlen(p)>MAXKEYSIZE)
				{
				// do not allow keys bigger than this size (5/17/05)
				continue;
				}

			if (!foundencryptedchannels)
				{
				// all keyfiles are encrypted now, this is just for legacy keyfiles
				// legacy keys can have multicase channel names, which can cause troubles.
				// check to see if channel name was not lowercase
				if (strcmp(tempstring,"MAGICID")!=0)
					mylowercasify(tempstring);
				// check to see if it is a duplicate
				lookup_channelkey(tempstring,key,true);
				if (key[0]!='\0')
					{
					// ok we got a duplicate key, SO we dont want to add both
					strcat(tempstring,"_errorduplicate");
					}
				}
			// create a key and add it to the linked list
			keyp=new MircryptionKeypair(tempstring,p);
			keyp->nextp=NULL;
			if (lastkeyp!=NULL)
				lastkeyp->nextp=keyp;
			else
				keypairs=keyp;
			lastkeyp=keyp;
			++keys;
			}
		}

	// we have loaded the keyfile, but remember that it is not uncrypted(unlocked) yet
	passphrasesunlockedflag=false;
	fclose(fp);
	return true;
}


void MircryptionClass::unloadkeys()
{
	// force the keys out of memory, clear the master passphrase, and free keypair list
	MircryptionKeypair *nextp;

	masterpassphrase[0]='\0';
	passphrasesunlockedflag=false;

	// walk through the linked list and delete all keys
	while (keypairs!=NULL)
		{
		// bleach it before deletion
		memset(keypairs->key,0,strlen(keypairs->key));
		memset(keypairs->channelname,0,strlen(keypairs->channelname));
		// delete key
		nextp=keypairs->nextp;
		delete keypairs;
		keypairs=nextp;
		}
	keys=0;
	keypairs=NULL;
}


bool MircryptionClass::backupkeys()
{
	// backup key file
	FILE *fp,*fp2;
	char tempstring[MAXLINELENE+1];

	/*
	// double backup .bak to .bk2 ?
	sprintf(tempstring,"%s.bak",keyfilename);
	fp=fopen(tempstring,"r");
	sprintf(tempstring,"%s.bk2",keyfilename);
	fp2=fopen(tempstring,"w");
	if (fp!=(FILE *)NULL && fp2!=(FILE *)NULL)
		{
		while (!feof(fp))
			{
			fgets(tempstring,MAXLINELENE,fp);
			tempstring[MAXLINELENE]='\0';
			fputs(tempstring,fp2);
			}
		fclose(fp);
		fclose(fp2);
		}
	*/

	// create a bak file ending in .bak
	fp=fopen(keyfilename,"r");
	sprintf(tempstring,"%s.bak",keyfilename);
	fp2=fopen(tempstring,"w");
	if (fp!=(FILE *)NULL && fp2!=(FILE *)NULL)
		{
		// copy the file line by line
		while (!feof(fp))
			{
			fgets(tempstring,MAXLINELENE,fp);
			tempstring[MAXLINELENE]='\0';
			fputs(tempstring,fp2);
			}
		fclose(fp);
		fclose(fp2);
		// record flag saying we have backed up the file
		keyfilebackedup=true;
		}
	return keyfilebackedup;
}



bool MircryptionClass::save_keys()
{
	// save encrypted keys to file
	// returns true on succesful saving of keys
	FILE *fp;
   	char* p;
	char tempstring[MAXLINELENE+1];
	MircryptionKeypair *keyp=keypairs;
	bool encryptchannelnames=true;

	// keys not unlocked, so we refuse to save them
	if (masterpassphrase[0]=='\0' || keys==0 || passphrasesunlockedflag==false)
		return false;

	// before we save any changes, we backup the key file (but only if keys have changed since last backup)
	if (!keyfilebackedup)
		backupkeys();

	// open the file to write
	if ((fp=fopen(keyfilename,"w"))==(FILE *)NULL)
		return false;
	
	// save a header
	fprintf(fp,"// Mircryption Channel Keys - %s - stored in encrypted form (do not forget your master passphrase)\n",MIRCRYPTIONCLASSVER);
	if (encryptchannelnames==true)
		fprintf(fp,"// Using Encrypted Channel Names\n");
	fprintf(fp,"//\n");

	// save the magic id in encrypted form to help verify proper decrypting on next load
	p=encrypt_string(masterpassphrase,MAGICID);
	fprintf(fp,"MAGICID %s\n",p);
	bleachdelete(p);

	// cycle through all keys and save them in encrypted form
	// ATTN: we should check return codes from the fprintf below to make sure write succeeds
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,"MAGICID")!=0)
			    {
				if (encryptchannelnames)
					{
					// encrypt channel name and key
					mc_encrypt2key(masterpassphrase, keyp->channelname, tempstring);
					p=encrypt_string(masterpassphrase,keyp->key);
					fprintf(fp,"%s %s\n",tempstring,p);
					bleachdelete(p);
					}
				else
					{
					// dont encrypt channel name, just key
					p=encrypt_string(masterpassphrase,keyp->key);
					fprintf(fp,"%s %s\n",keyp->channelname,p);
					bleachdelete(p);
					}
			    }
		keyp=keyp->nextp;
		}

	fclose(fp);
	// bleach tempstrings
	memset(tempstring,1,MAXLINELENE);
	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
char* MircryptionClass::lookup_channelkey(char *channelname,char *key,bool wantencrypted)
{
	// internal func called to return the key associated with a channel
	// lookup the keypass for a channel or return "" if it doesnt exist
	// parameter wantencrypted tells us if it wants us to return undecrypted values (if keys are not yet unlocked), OR the keyword "_ENCRYPTED_" for keys which are not unlocked
	MircryptionKeypair *keyp=keypairs;

	mcensuresafebuflen(channelname,MAXCHANNELNAMESIZE);

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	// new force lowercase, to be safe
//	mylowercasify(channelname);

	if (foundencryptedchannels)
		{
		// if the keyfile has encrypted channels, then we need to make sure it is unlocked before we can lookup a channel key
		// kind of a kludge, but we need to reset foundencryptedchannels to avoid a recursive call back to us during decyrption
		foundencryptedchannels=false;
		verify_keysunlocked();
		}

	// loop through keys
	while (keyp!=NULL)
		{
		if (strcmp(keyp->channelname,channelname)==0)
			{
			// found a match, so return it
			strcpy(key,keyp->key);
			break;
			}
		keyp=keyp->nextp;
		}

	// if keys are not unlocked, modify returned value depending on wantencrypted parameter
	if (keyp==NULL)
		strcpy(key,"");
	else if (key[0]!='\0' && passphrasesunlockedflag==false && !wantencrypted)
		strcpy(key,"_ENCRYPTED_");
	return key;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MircryptionClass::decrypt2_substring(char *insubstring,char *outstring, char *key)
{
	// given a substring which has start and end tags removed, decrypt it and store result in outstring

	// undo whitespace conversion created during encryption
	unrepwhitespaces(insubstring);

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	// decrypt it using provided key
	char* p = search_decrypt_string(key,insubstring);
	// copy to output
	if (p!=NULL)
		{
		// copy the result, ABSENT the initial crc tag
		strcpy(outstring,p+strlen(MCPS2_CRC));
		bleachdelete(p);
		}
	else
		strcpy(outstring,"");
}


char* MircryptionClass::search_decrypt_string(char *inkey, char *instring)
{
	// this is a wrapper for the blowfish command decrypt_string(char *key,char *instring) which can search through all decryption keys if the suggested one doesnt work
	// this is useful if we have some text and dont know which key to apply, though can be a bit slow if you have a lot of keys.
	char* p;
	char key[MAXKEYSIZE];
	char mcps2crcstr[10];
	int mcps2crclen;
	MircryptionKeypair *keyp=NULL;

	// crc tag
	strcpy(mcps2crcstr,MCPS2_CRC);
	mcps2crclen=strlen(mcps2crcstr);
	
	// protect against too long keys
	if (strlen(inkey)>MAXSAFEKEYSIZE)
		inkey[MAXSAFEKEYSIZE]='\0';

	// start with suggested key
	strcpy(key,inkey);
	for (;;)
		{
		// decrypt it
		p = decrypt_string(key,instring);
		if (p==NULL)
			break;
		// here we check if decryption is valid by checking a tag which will be a certain value if decryption is successful
		if (strncmp(p,mcps2crcstr,mcps2crclen)==0)
			{
			// ok, successfully decrypted, return pointer (caller should delete p when we return!)
			break;
			}
		// key did not work, do try another
		bleachdelete(p);
		p=NULL;
		// get next key to try
		if (keyp==NULL)
			keyp=keypairs;
		else
			keyp=keyp->nextp;
		if (keyp==NULL)
			break;
		strcpy(key,keyp->key);
		}
	memset(key,0,MAXKEYSIZE);
	return p;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void MircryptionClass::repwhitespaces(char *str)
{
	// given a blowfish encoded string str, convert all whitespaces to non-64bit (blowfish used) replacement characters
	char *p=str;
	char c;
	for (;;)
		{
		c=*p;
		if (c=='\0')
			break;
		if (c==9)
			*p=(char)162;
		else if (c==10)
			*p=(char)163;
		else if (c==13)
			*p=(char)164;
		else if (c==32)
			*p=(char)165;
		++p;
		}
}


void MircryptionClass::unrepwhitespaces(char *str)
{
	// unconvert all whitespace replaced characters above to original whitespaced
	char *p=str;
	char c;
	for (;;)
		{
		c=*p;
		if (c=='\0')
			break;
		if (c==(char)162)
			*p=(char)9;
		else if (c==(char)163)
			*p=(char)10;
		else if (c==(char)164)
			*p=(char)13;
		else if (c==(char)165)
			*p=(char)32;
		++p;
		}
}


char* MircryptionClass::mystrstr(char *startp,char *substring)
{
	// inefficient implementation of strstr - since im not sure builtin one is working
	int substrlen=strlen(substring);
	while (*startp!='\0')
		{
		if (strncmp(startp,substring,substrlen)==0)
			return startp;
		++startp;
		}
	return (char *)NULL;
}


void MircryptionClass::stripnicks(char *str)
{
	// strip any leading <nick> or [nick] or 'timestamp [nick]' from lines
	// the tricky part is the potential confusion from timestamps or alternative ways of specifying nick...
	char c;
	bool atlinefront=true;
	bool foundnick=false;
	char *cp=str;
	int anglecount=0;
	int bracketcount=0;
	char *nickstart=str;
	bool foundanglebracket=false;
	bool skipline=false;
	// if alwayskillprefix is true, then we kill everything to the left of the first space found (not inside a bracket/angle)
	// if alwayskillprefix is false, then we only kill prefixes if they were in a breacket/angle container
	bool alwayskillprefix=true;

	while ((c=*cp)!='\0')
		{
		
		// prepare for next char
		if (c==13 || c==10)
			{
			// on a linebreak we reset so we look for the next nick prefix
			atlinefront=true;
			foundnick=false;
			foundanglebracket=false;
			skipline=false;
			anglecount=0;
			bracketcount=0;
			nickstart=cp+1;
			}
		else
			{
			if (c=='<')
				{
				++anglecount;
				foundanglebracket=true;
				}
			else if (c=='>')
				--anglecount;
			else if (c=='[')
				{
				++bracketcount;
				foundanglebracket=true;
				}
			else if (c==']')
				--bracketcount;
			else if (foundnick==false && !atlinefront && !skipline)
				{
				// havent found the nick yet, could this be a good place for us? we are looking for a space after any brackets+angles
				if (c==32)
					{
					// we found a leading space, should we consider it the start of the real sentence after the nick
					if (alwayskillprefix==false && foundanglebracket==false)
						{
						// space without brackets in this alwayskillprefix==false mode tells us to skip this line
						skipline=true;
						}
					else if (anglecount==0 && bracketcount==0)
						{
						// we are not inside any angels or brackets so we consider this a separator for the nick
						foundnick=true;
						// now remove it
						strcpy(nickstart,cp+1);
						// and adjust cp
						cp=nickstart;
						}
					}
				}
			atlinefront=false;
			}
		++cp;
		}
}

void MircryptionClass::mylowercasify(char *str)
{
	if (str==NULL)
		return;

	int len=strlen(str);
	char c;

	for (int count=0;count<len;++count)
		{
		c= str[count];
		if (c>='A' && c<='Z')
			str[count]=c+32;
		}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MircryptionClass::bleachmemory()
{
	// overwrite all passwords before freeing memory
	MircryptionKeypair *keyp=keypairs;

	// bleach master pass
	memset(masterpassphrase,0,MAXKEYSIZE-1);
	passphrasesunlockedflag=false;

	// loop through keys and bleach them
	while (keyp!=NULL)
		{
		memset(keyp->key,0,strlen(keyp->key));
		memset(keyp->channelname,0,strlen(keyp->channelname));
		keyp=keyp->nextp;
		}

}

void MircryptionClass::bleachdelete(char *p)
{
	// bleach the string and delete it
	memset(p,0,strlen(p));
	delete p;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
int MircryptionClass::checkpasswordquality(char *key,char *errorstring)
{
	// static function which evaluates quality of key
	// returns <0 for keys considered unacceptable
	if (strlen(key)<14)
		{
		strcpy(errorstring,"Please do not use passwords smaller than 14 characters (20-50 characters recommended, mix case and digits, do not use dictionary words).");
		return -1;
		}

	// protect against too long keys
	if (strlen(key)>MAXSAFEKEYSIZE)
		key[MAXSAFEKEYSIZE]='\0';

	return 1;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// PLATFORM DEPENDENT FUNCTIONS - OVERRIDE THESE FOR YOUR CLIENT
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// IMPORTANT: THESE ARE VIRTUAL FUNCTIONS, THEY NEED TO BE OVERRIDEN BY
//   PLATFORM-DEPENDENT IMPLEMENTATION, depending on your irc client.

char *MircryptionClass::get_classversionstring()
{
	// return some version information
	// by default return defined macro - you dont have to override this one
	return MIRCRYPTIONCLASSVER;
}

bool MircryptionClass::present_messagebox(char *messagetext,char *windowtitle)
{
	// present some info to the user, preferably in a pop-up modal dialog
	// returns true on success

	// by default just display it in status window - you dont have to override this one
	display_statustext(messagetext);

	return false;
}

bool MircryptionClass::request_unlockpassphrase()
{
	// we need the user to give us a valid master passphrase
	// returns true if user gives us a valid passphrase, and unlock the keys, false if not
	return false;
}


bool MircryptionClass::send_irccommand(char *irccommand,char *text)
{
	// send an irc command to server
	// returns true on success
	return false;
}

bool MircryptionClass::display_statustext(char *messagetext)
{
	// display some information for the user in some default text area (like status window in mirc)
	// returns true on success
	return false;
}
//---------------------------------------------------------------------------
