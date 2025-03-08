//---------------------------------------------------------------------------
// xchat mircryption.h
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// To avoid multiple header instantiations
#ifndef _mircryptionh
#define _mircryptionh
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "../mircryptionclass.h"
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
class MircryptionClass_xchat : public MircryptionClass
{
public:
	MircryptionClass_xchat() : MircryptionClass() {;};
	virtual ~MircryptionClass_xchat() {;};
protected:
	// Mirc specific overrides
	virtual char *get_classversionstring();
	virtual bool present_messsagebox(char *messagetext,char *windowtitle);
	virtual bool request_unlockpassphrase();
	virtual bool send_irccommand(char *irccommand,char *text);
	virtual bool display_statustext(char *messagetext);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// new extra safe buffer overflow checks
// thanks to www.rainbowcrack-online.com and ircfuz for finding this dangeroud possibility
//  more serious buffer overflow checks are also now in the dll as well
void mcnicksafe_strcpy(char *dest,const char* src);
void mclinesafe_strcpy(char *dest,const char* src);
void mckeysafe_strcpy(char *dest,const char* src);
void mcsafe_strcpy(char *dest,const char* src,int maxlen);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

