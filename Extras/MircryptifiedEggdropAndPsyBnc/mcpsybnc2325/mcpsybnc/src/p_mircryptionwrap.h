//---------------------------------------------------------------------------
// p_mircryptionwrap.h
// tiny wrappers around mircryption blowfish functins
// mouser 1/15/05
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// header guards
#ifndef pmircryptionwraph
#define pmircryptionwraph
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// forward declaration for exposed functions

// note that we expect the encrypt to use standard psybnc blowfish encryption if key dont start with 'mcps:' or 'cbc:'
//  and in that case just defer to normal psybnc routines and return outprefix set to '[B]'
//  (unlike returned char*, outprefix is preallocated and just needs a strcpy).
extern char *MCPS_stringencrypt(unsigned char *input, unsigned char *key, char *outprefix);
extern char *MCPS_stringdecrypt(unsigned char *input, unsigned char *key, char *inprefix);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// header guard
#endif
//---------------------------------------------------------------------------
