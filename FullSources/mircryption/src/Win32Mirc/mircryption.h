//---------------------------------------------------------------------------
// mircryption.h
// 
// http://mircryption.sourceforge.net
// By Dark Raichu
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// To avoid multiple header instantiations
#ifndef _mircryptionh
#define _mircryptionh
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "mirc_dllshell.h"
#include "../mircryptionclass.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// mircDll support functions, called from mirc_dllshell.cpp
// just standard ways to provide some info
#define MIRCDLL_VERSION "1.15.01"
#define MIRCDLL_INFO "Mircryption DLL"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// see above
const char *get_mircDllInfo();
const char *get_mircDllVersion();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Defines for sending text to mirc
#define MAXTEXTBOXLEN 63000
#define MAXIRCMAPSIZE 4096

// mirc sadly has linelength limit of about 900.  worse yet, servers often have even smaller limits like 450, so 400 is safe.
// BUT recently we've discovered mirc can choke on words>400 chars, which can result from strings of about 350 chars after blowfishing
#define MAXIRCSAFELINELEN 250

// sleep a few ms between line posts to make sure you dont get flood kicked
//#define MINTEXTSLEEPTIME 2
//#define MAXTEXTSLEEPTIME 20
//#define TEXTSWITCHPOINT 1000
#define MINTEXTSLEEPTIME 2
#define MAXTEXTSLEEPTIME 20
#define TEXTSWITCHPOINT 700
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
class MircryptionClass_Mirc : public MircryptionClass
{
public:
	MircryptionClass_Mirc() : MircryptionClass() {;};
	virtual ~MircryptionClass_Mirc() {;};
protected:
	// Mirc specific overrides
	virtual char *get_classversionstring();
	virtual bool present_messagebox(char *messagetext,char *windowtitle);
	virtual bool request_unlockpassphrase();
	virtual bool send_irccommand(char *irccommand,char *text);
	virtual bool display_statustext(char *messagetext);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Forward declarations of functions
void do_DllStart();
bool do_mircDllLoad(LOADINFO* loadinfo);
int do_mircDllUnLoad(int mode);
void bleachmemory();
//
MircFunc mc_setunlockpassphrase (FUNCPARMS);
MircFunc mc_setkeyfile (FUNCPARMS);
MircFunc mc_iskeyunlocked (FUNCPARMS);
MircFunc mc_encrypt (FUNCPARMS);
MircFunc mc_decrypt (FUNCPARMS);
MircFunc mc_forceencrypt (FUNCPARMS);
MircFunc mc_encrypt2 (FUNCPARMS);
MircFunc mc_decrypt2 (FUNCPARMS);
MircFunc mc_setkey (FUNCPARMS);
MircFunc mc_delkey (FUNCPARMS);
MircFunc mc_disablekey (FUNCPARMS);
MircFunc mc_enablekey (FUNCPARMS);
MircFunc mc_displaykey (FUNCPARMS);
MircFunc mc_listkeys (FUNCPARMS);
MircFunc mc_isencrypting (FUNCPARMS);
MircFunc mc_isdecrypting (FUNCPARMS);
MircFunc mc_md5 (FUNCPARMS);
MircFunc mc_touchfile (FUNCPARMS);
//
bool SendMircText (char *preface, char *p, HWND targetwin, int maxlinelen, int speedmod, bool stripnicks);
void ircMapOpen();
void ircMapClose();
//
MircFunc mc_splitsay (FUNCPARMS);
MircFunc mc_pastepad (FUNCPARMS);
MircFunc mc_forcetopic (FUNCPARMS);
//
BOOL CALLBACK MyDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EditPadDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MenuDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
