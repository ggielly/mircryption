//---------------------------------------------------------------------------
// Mircryption Dll wrapper for Win32
// 
// http://mircryption.sourceforge.net
// By Dark Raichu, 12/01 - 6/03
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "mircryption.h"
#include "mircryption_resource.h"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Global variables
HWND MircHwnd;
// for sending irc window text
HWND ircMapWindow;
HANDLE ircMapHandle;
LPSTR ircMapData;
char *ircMapTempTextp;
char ircMapChannelname[255];
// for dialogs
char dialogmasterpassphrase[255];
bool dialogup=false;
bool abortsend=false;
bool sending=false;
bool sendingpause=false;
// annoying flag for keeping track whether we are asking for simple plass to unlock, or double pass to change
bool doublepass=false;
//
int loadcalls=0;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// pointer to global mircryptor which does the bulk of the work
MircryptionClass_Mirc *mircryptor=NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// inline simples
const char *get_mircDllInfo() {return MIRCDLL_INFO;};
const char *get_mircDllVersion() {return MIRCDLL_VERSION;};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__inline bool IsDownControl() {return (GetKeyState(VK_CONTROL)&0x8000)==0x8000;};
__inline bool IsDownShift() {return (GetKeyState(VK_SHIFT)&0x8000)==0x8000;};
__inline bool IsDownAlt() {return (GetKeyState(VK_MENU)&0x8000)==0x8000;};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// DLL initialization
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void do_DllStart()
{
	// when dll loads for first time - called ONLY ONCE
	// initialize

	// init dialog stuff
	strcpy(dialogmasterpassphrase,"");
	dialogup=false;

	// init map for talking to mirc
	ircMapWindow=0;
	ircMapHandle=0;
	ircMapData=NULL;
	ircMapTempTextp=NULL;

	// initialize stuff - duplicate from global initializations
	dialogup=false;
	abortsend=false;
	sending=false;
	sendingpause=false;
	doublepass=false;
	loadcalls=0;

	// create our mircryptor class which does the bulk of the work
	mircryptor=new MircryptionClass_Mirc;
	// if keyfile is not yet unlocked, then unlock them
	mircryptor->load_keys();
}


bool do_mircDllLoad(LOADINFO* loadinfo)
{
	// load the dll
	// This function is called when the dll is first accessed - here you reserve any global stuff.
	// Return true if you want to stay loaded on return or false is you can be unloaded immediately.


	// there may be a bug, or its a multithreading thing, that can cause 2 calls to load. we ignore all but first
	++loadcalls;
	if (loadcalls>1)
		{
		// we are already loaded, so nothing to do
		return true;
		}

	// call one-time init procedure
	MircHwnd=loadinfo->mHwnd;
	do_DllStart();

	// we want to stay loaded so we return true
	return true;
}


int do_mircDllUnLoad(int mode)
	{
	// unload the dll
	// This function is called when mirc requests that the dll unload.
	// However, sometimes it is ok to turn down this request.
	// if m==1 then the request is a polite, perfunctary request after 10 minutes of inactivity
	//    which you may want to turn down if you need to preserve memory objects.
	// if m!=1 then this is a forcefull request to terminate, and you should terminate.
	// RETURN 0 if you refuse to terminate, or 1 if you have freed up your memory and are willing to unload

	if (mode==1)
		{
		// this is a polite 10min inactive call to unload.
		// we return 0 to ask not to be unloaded - so that user does not have to enter master passphrase repeatedly
		return 0;
		}


	if (loadcalls>1)
		{
		// when multiple loads, we let ourselve exit
		// only free mem on the last call
		--loadcalls;
		return 1;
		}


	// When we get to here we are unloading from memory and so want to free everything up
	// MessageBox(0, "Sole DLL unloading!", "Mircryption Message", 0);

	// free mircryptor object
	if (mircryptor!=NULL)
		delete mircryptor;
	mircryptor=NULL;

	// close the map (used for sending info to mirc)
	ircMapClose();

	// bleachmemory to overwrite any passwords in memory
	bleachmemory();

	// unload
	--loadcalls;
	return 1;
}


void bleachmemory()
{
	// bleachmemory to overwrite any passwords in memory
	// bleach dialog text - hmm.. can we be sure windows wont page this to swap file? ie free and 
	GetDlgItemText(MircHwnd,IDC_PASSWORDEDIT,dialogmasterpassphrase,254);
	memset(dialogmasterpassphrase,0,strlen(dialogmasterpassphrase));
	SetDlgItemText(MircHwnd,IDC_PASSWORDEDIT,dialogmasterpassphrase);
	GetDlgItemText(MircHwnd,IDC_PASSWORDEDIT2,dialogmasterpassphrase,254);
	memset(dialogmasterpassphrase,0,strlen(dialogmasterpassphrase));
	SetDlgItemText(MircHwnd,IDC_PASSWORDEDIT2,dialogmasterpassphrase);
	// bleach master pass
	memset(dialogmasterpassphrase,1,253);
	dialogmasterpassphrase[254]='\0';
	SetDlgItemText(MircHwnd,IDC_PASSWORDEDIT,dialogmasterpassphrase);
	SetDlgItemText(MircHwnd,IDC_PASSWORDEDIT2,dialogmasterpassphrase);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Publically exposed mirc dll functions - simple direct translations
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// $dll(mircryption,mc_setunlockpassphrase,$active)
MircFunc mc_setunlockpassphrase (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	mcensuresafebuflen(data,MAXSAFEKEYSIZE);
	strcpy(dialogmasterpassphrase,data);
	mircryptor->mc_setunlockpassphrase(data,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_setkeyfile,$fullkeyfilename)
MircFunc mc_setkeyfile (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	mcensuresafebuflen(data,MAXKEYFILENAMELEN);
	mircryptor->mc_setkeyfilename(data,returndata);
	strcpy(data,returndata);
	return 1;
}

// $dll(mircryption,mc_iskeyunlocked,$chan) - returns "yes" if the master keyfile is unlocked
MircFunc mc_iskeyunlocked (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	if (mircryptor->mc_iskeyfileunlocked(returndata))
		lstrcpy(data,"yes");
	else
		lstrcpy(data,"no");
	return 3;
}

// %m=$dll(mircryption,mc_encrypt,$chan %om)
MircFunc mc_encrypt (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char *text=tmp;
	mircryptor->mc_encrypt(channelname,text,returndata);
	strcpy(data,returndata);
	return 3;
}

//  %m=$dll(mircryption,mc_decrypt,$nick %om)
MircFunc mc_decrypt (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char *text=tmp;
	mircryptor->mc_decrypt(channelname,text,returndata);
	strcpy(data,returndata);
	return 3;
}

// %m=$dll(mircryption,mc_forceencrypt,$chan %om)
MircFunc mc_forceencrypt (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	// same as mc_encrypt, but we encrypt even for disabled channels
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char *text=tmp;
	mircryptor->mc_forceencrypt(channelname,text,returndata);
	strcpy(data,returndata);
	return 3;
}

// %m=$dll(mircryption,mc_encrypt2,$chan %om)
MircFunc mc_encrypt2 (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char *text=tmp;
	mircryptor->mc_encrypt2(channelname,text,returndata);	
	strcpy(data,returndata);
	return 3;
}

//  %m=$dll(mircryption,mc_decrypt2,$nick %om)
MircFunc mc_decrypt2 (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char *text=tmp;
	mircryptor->mc_decrypt2(channelname,text,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_setkey,$active %fargs)
MircFunc mc_setkey (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	char* key = tmp;
	mircryptor->mc_setkey(channelname,key,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_delkey,$active %fargs)
MircFunc mc_delkey (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	mircryptor->mc_delkey(channelname,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_disablekey,$active %fargs)
MircFunc mc_disablekey (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	mircryptor->mc_disablekey(channelname,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_enablekey,$active %fargs)
MircFunc mc_enablekey (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	mircryptor->mc_enablekey(channelname,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_displaykey,$active %fargs)
MircFunc mc_displaykey (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	mircryptor->mc_displaykey(channelname,returndata);
	strcpy(data,returndata);
	return 3;
}

// $dll(mircryption,mc_listkeys,$active)
MircFunc mc_listkeys (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	mircryptor->mc_listkeys(returndata);
	strcpy(data,returndata);
	return 1;
}

// $dll(mircryption,mc_isdecrypting,$chan) - returns "yes" if the channel is set to encrypt text (if disabnle say no)
MircFunc mc_isencrypting (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	if (mircryptor->mc_isencrypting(channelname,returndata))
		lstrcpy(data,"yes");
	else
		lstrcpy(data,"no");
	return 3;
}

// $dll(mircryption,mc_isdecrypting,$chan) - returns "yes" if a key exists to decrypt text
MircFunc mc_isdecrypting (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	char* tmp = data;
	char* channelname = _getlcword(tmp);
	if (mircryptor->mc_isdecrypting(channelname,returndata))
		lstrcpy(data,"yes");
	else
		lstrcpy(data,"no");
	return 3;
}

// $dll(mircryption,mc_md5 text...)
MircFunc mc_md5 (FUNCPARMS)
{
	char returndata[MAXRETURNSTRINGLEN];
	// New - simple md5 of a string
	char* tmp = data;
	char* text = tmp;
	mircryptor->mc_md5(text,returndata);
	strcpy(data,returndata);
	return 3;
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Sending stuff to mirc via mirc map
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
// Procedures for sending commands to mirc

bool SendMircText (char *preface, char *p, HWND targetwin, int maxlinelen, int speedmod)
{
	// send text to an irc window, breaking it up into line-sized chunks
	char *startp=p;
	int len=0;
	char c;
	int retv;
	char forcefix=0;
	int sleeptime=MINTEXTSLEEPTIME;
	int textoutcount=0;
	bool fixconsecspaces=true;
	unsigned char spacefixer=160;
	bool lastwasspace=true;
	MSG msg;

	if (sending)
		return false;

	// set global flags
	abortsend=false;
	sending=true;

	// make sure line length is reasonable
	if (maxlinelen>CRASHLINELEN-1)
		maxlinelen=CRASHLINELEN-1;
	// default to server-safe line length)
	if (maxlinelen<=1)
		maxlinelen=MAXIRCSAFELINELEN;

	// make sure the irc file map is open
	ircMapOpen();

	for (;;++p)
		{
		c=*p;
		if (c==13 || c==10 || (c==0 && len>0))
			{
			*p='\0';
			if (startp==p)
				wsprintf(ircMapData,"%s .",preface);
			else
				wsprintf(ircMapData,"%s %s",preface,startp);

			// now send the text from global file map
			// MessageBox(0, ircMapData, "About to sendmessage", 0);
			retv=SendMessage(targetwin, WM_USER + 200,0,0);

			if (retv==0)
				{
				MessageBox(0, "Error received while trying to send the text to Mirc via SendMessage() api call.", "Mircryption Error", 0);
				break;
				}
			if (abortsend)
				{
				break;
				}

			textoutcount+=strlen(startp);

			// now sleep for a bit to avoid flood, and inc. sleep time as heuristic to aviod flood detect
			if (speedmod>0 || sendingpause)
				{
				// we used to sleep here, now we try to be more clever and allow ourselves to be interrupted
				//			Sleep(sleeptime*strlen(startp)*speedmod);
				DWORD sleeptill=GetTickCount()+sleeptime*strlen(startp)*speedmod;
				while((GetTickCount()<sleeptill) || sendingpause)
					{
					while (::PeekMessage( &msg, NULL, 0,0, PM_NOREMOVE) && !abortsend )
						{
						if (msg.message != WM_QUIT)
							{
							::GetMessage(&msg,NULL,0,0);
							::TranslateMessage(&msg);
							::DispatchMessage(&msg);
							if (msg.message ==WM_COMMAND && LOWORD(msg.wParam)==IDCANCEL)
								{
								abortsend=true;
								break;
								}
							}
				       else
							{
							abortsend=true;
							break;
							}
						}
					if (abortsend)
						break;
					if (sendingpause)
						{
						// reset slowdown
						textoutcount=0;
						sleeptime=MINTEXTSLEEPTIME;
						}
					}

				if (abortsend)
					break;

				// update
				if (textoutcount>TEXTSWITCHPOINT)
					{
					if (sleeptime==MINTEXTSLEEPTIME)
						sleeptime=MAXTEXTSLEEPTIME;
					}
				}


			if (forcefix!=0)
				{
				// this happens when we have a line that is too big, so we need to back up and fix last char and go from there
				*p=forcefix;
				forcefix=0;
				--p;
				}
			if (*(p+1)==10)
				++p;
			lastwasspace=true;
			startp=p+1;
			len=0;

			if (c==0)
				break;
			}
		else if (c==0)
			break;
		else if ((c==32 || c==9) && lastwasspace && fixconsecspaces)
			{
			// mirc fucks up multiple consecutive spaces (and has for a long time and wont fix it >:)
			// leading spaces convert to some non-space character
			*p=spacefixer;
			++len;
			lastwasspace=true;
			}
		else
			{
			if (c==32 || c==9)
				lastwasspace=true;
			else
				lastwasspace=false;
			++len;
			}
		if (len>maxlinelen)
			{
			// well we have a problem, we have a line that is too big.
			// so what we want to do now is back up last whitspace and force a linebreak there.
			// failing that (if just pure characters, we will break in the middle
			--len; --p;
			for (;len>0;--len,--p)
				{
				c=*p;
				if (c==32 || c==9)
					{
					// okay we found a whitespace, so we force in a ret, and move pointer back so normal routine will see it on next iteration
					*p=13;
					--p;
					--len;
					lastwasspace=true;
					break;
					}
				}
			if (len<=0)
				{
				// no whitespace was found, so we are back at the start.  just print out a max length line
				len=maxlinelen-1;
				p=p+len;
				forcefix=*p;
				*p=13;
				--p;
				lastwasspace=true;
				}
			}
		}

	sendingpause=false;
	sending=false;
	return true;
}


void ircMapOpen()
{
	// open the file map to send messages to irc if its not open alread
	if (ircMapData==NULL)
		{
		ircMapHandle=CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,MAXIRCMAPSIZE,"mIRC");
		if (!ircMapHandle) MessageBox(NULL,(char *)"Can't create a file mapping object.",(char *)"Error...",MB_OK);
		ircMapData=(LPSTR)MapViewOfFile(ircMapHandle, FILE_MAP_ALL_ACCESS,0,0,0);
		if (!ircMapData) MessageBox(NULL,(char *)"Can't map view of the file.",(char *)"Error...",MB_OK);
		ircMapTempTextp=new char[MAXTEXTBOXLEN];
	}
}


void ircMapClose()
{
	// close the file map to irc if it is open
	if (ircMapData!=NULL)
		{
		UnmapViewOfFile(ircMapData);
		CloseHandle(ircMapHandle);
		ircMapWindow=0;
		ircMapHandle=0;
		ircMapData=NULL;
		delete ircMapTempTextp;
		ircMapTempTextp=NULL;
	}
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Windows-Specific GUI functions
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
// Exposed dll commands which involve gui

// this is like mc_pastepad, but is for when user already has a long line of text, and just wants to make sure it is of safe size
// $dll(mircryption,mc_splitsay channelname linelen text...)
MircFunc mc_splitsay (FUNCPARMS)
{
	char* tmp = data;
	char* cname = _getword(tmp);
	int linelen=_atolp(_getword(tmp));
	bool bretv;
	char preface[255];

	mcensuresafebuflen(cname,200);
	wsprintf(preface,"/forceemsg %s",cname);
	bretv=SendMircText(preface, tmp, aWnd, linelen,1);
	return 1;
}



// bring up a big copy&paste edit box and let them type as much text as they want, then we feed ito to mirc 1 safe short line at a time
// $dll(mircryption,mc_pastepad)
MircFunc mc_pastepad (FUNCPARMS)
{
	// return true if user gives us a valid passphrase, and unlock the keys, false if not
	bool bretv=false;
	int dialogretv;
	char* tmp = data;
	char* cname = _getword(tmp);

	// remember the target window which has called you , which is one of the FUNPARMS
	ircMapWindow=aWnd;
	strcpy(ircMapChannelname,cname);

	// allow user to paste text into the dialogbox
	dialogretv=DialogBox(DllWindow_hinstance, MAKEINTRESOURCE(IDD_BIGPAD), NULL, (DLGPROC)EditPadDialogProc);
	return 1;
}


// force the window titlebar of a channel window
// $dll(mircryption,mc_forcetopic,$window($chan).hwnd,custom topic text)
MircFunc mc_forcetopic (FUNCPARMS)
{
	// this function is used to force the topic text of a channel to a hardcoded value, used for encrypted topics
	char* tmp = data;
	char topic[MAXLINELEN];
	char *p;
	int count;

	// the handle of the window to change is passed as an ascii number
	HWND window = (HWND)_atolp(_getword(tmp));
	char* preface = _getword(tmp);
	// make sure its a valid handle
	if (IsWindow(window))
		{
		// ok, first get current title of window
		GetWindowText(window,topic,MAXLINELEN-1);
		if (lstrcmp(preface,"yes")==0)
			{
			// add (e) if it dont exit
			if (topic[0]!='(' || topic[1]!='e' && topic[2]!=')' || topic [3]!=' ')
				{
				int len=strlen(topic);
				topic[len+4+1]='\0';
				for (count=len+4;count>=4;--count)
					topic[count]=topic[count-4];
				topic[0]='(';
				topic[1]='e';
				topic[2]=')';
				topic[3]=' ';
				}
			}
		else
			{
			// remove (e) if it exits
			if (topic[0]=='(' && topic[1]=='e' && topic[2]==')' && topic [3]==' ')
				{
				int len=strlen(topic);
				for (count=0;count<=len-4;++count)
					topic[count]=topic[count+4];
				topic[count]=0;
				}
			}

		// now find the first ':' character (where custom topic starts) or eol, and end line there
		p=_mystrchr(topic,':');
		if (p!=NULL)
			{
			*(p+1)='\0';
			// now we ad the custom topic we were passed to it.
			lstrcat(topic," ");
			lstrcat(topic,tmp);
			}
		SetWindowText(window,topic);
		lstrcpy(data,"ok.");
		}
	else
		lstrcpy(data,"error.");
	return 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Callback procedure to receive callbacks from the master key input dialog
BOOL CALLBACK MyDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char editstring[MAXKEYSIZE],editstring2[MAXKEYSIZE];
  switch (msg)
  {
    case WM_INITDIALOG:
	// det the default text
	SetDlgItemText(hDlg,IDC_PASSWORDEDIT,dialogmasterpassphrase);
	if (doublepass)
		{
		// double passphrase change dialog
		SetDlgItemText(hDlg,IDC_PASSWORDEDIT2,dialogmasterpassphrase);
		}
	return TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDOK:
		GetDlgItemText(hDlg,IDC_PASSWORDEDIT,editstring,254);
		if (doublepass)
			GetDlgItemText(hDlg,IDC_PASSWORDEDIT2,editstring2,254);
		else
			strcpy(editstring2,editstring);
		if (strcmp(editstring,editstring2)==0)
			{
			if (lstrcmp(editstring,"")!=0)
				{
				// accept the new password?
				char errorstring[255];
				if (doublepass && MircryptionClass::checkpasswordquality(editstring,errorstring)<0)
					{
					MessageBox(hDlg, errorstring, "Error - Passphrase is not sufficiently secure", 0);
					}
				else
					{
					lstrcpy(dialogmasterpassphrase,editstring);
					EndDialog(hDlg, IDOK);
					}
				}
			else
				{
				// empty pass is cancel
				EndDialog(hDlg, IDOK);
				}
			}
		else if (strcmp(editstring,"")!=0)
			{
			//  give them error message
			MessageBox(hDlg, "Type the same passphrase twice to confirm it.", "Error - Passphrases do not match", 0);
			}
		else
			EndDialog(hDlg, IDOK);
		// bleach editstring
		memset(editstring,1,MAXKEYSIZE);
		memset(editstring2,1,MAXKEYSIZE);
		return TRUE;
	case IDCANCEL:
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
      }
      break;
  }
  return FALSE;
}


// Callback procedure to receive callbacks from the master key input dialog
BOOL CALLBACK EditPadDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int bretv;
	char preface[255];

	switch (msg)
		{
		case WM_INITDIALOG:
			// enable send button
			SetWindowText(GetDlgItem(hDlg,IDOK), "SUBMIT");
			// EnableWindow(GetDlgItem(hDlg,IDOK),TRUE);
			return TRUE;
		case WM_COMMAND:
		switch (LOWORD(wParam))
			{
			case IDOK:
				// make sure the irc file map is open
				if (sending)
					{
					sendingpause=!sendingpause;
					if (sendingpause)
						SetWindowText(GetDlgItem(hDlg,IDOK), "RESUME");
					else
						SetWindowText(GetDlgItem(hDlg,IDOK), "PAUSE");
					}
				else
					{
					ircMapOpen();
					GetDlgItemText(hDlg,IDC_RICHEDITTEXT,ircMapTempTextp,MAXTEXTBOXLEN-1);
					wsprintf(preface,"/emsg %s",ircMapChannelname);
					// disable send button?
					SetWindowText(GetDlgItem(hDlg,IDOK), "PAUSE");
					bretv=SendMircText (preface,ircMapTempTextp, ircMapWindow, MAXIRCSAFELINELEN,1);
					sending=false;
					sendingpause=false;
					EndDialog(hDlg, IDOK);
					}
				return TRUE;
			case IDCANCEL:
				abortsend=true;
				sendingpause=false;
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			case IDC_STRIPNICKBUTTON:
				ircMapOpen();
				// the strip nicks button is pushed - so strip nicks from textpad and reset contents
				GetDlgItemText(hDlg,IDC_RICHEDITTEXT,ircMapTempTextp,MAXTEXTBOXLEN-1);
				MircryptionClass::stripnicks(ircMapTempTextp);
				SetDlgItemText(hDlg,IDC_RICHEDITTEXT,ircMapTempTextp);
				break;
			default:
				break;
			}
			break;
		case WM_KEYDOWN:
			// MICROSOFT SHOULD BURN IN HELL for making this so impossible to get working
			//if (wParam == VK_ESCAPE)
			//	{
			//	if (IsDownAlt())
			//		::PostMessage(hDlg, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			//	}
			break;
	}
	return FALSE;
}










//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Virtual functions for derived MircryptionClass_Mirc, required by the mirc specific mircryptionclass
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
char *MircryptionClass_Mirc::get_classversionstring()
{
	// return some version information - you dont have to override this one
	return "Mircryption Class for Mirc";
}


bool MircryptionClass_Mirc::present_messagebox(char *messagetext,char *windowtitle)
{
	// present some info to the user, preferably in a pop-up modal dialog
	// returns true on success

	MessageBox(0, messagetext, windowtitle, 0);
	return true;
}


// we need the user to give us a valid master passphrase
bool MircryptionClass_Mirc::request_unlockpassphrase()
{
	// return true if user gives us a valid passphrase, and unlock the keys, false if not
	bool bretv=false;
	int dialogretv;
	int dialognum;

	// multithread sync - mirc can call this multiple times, and there is no good way to avoid either displaying dialog twice or hanging forever
	// so we will try to avoid the situation of getting here twice from scripts, but if we do, we return from 2nd call as if user hit cancel.
	if (dialogup==true)
		return mircryptor->mc_iskeyfileunlocked(NULL);

	// here we want to ask the user to type in a passphrase in a dialog box
	for (;;)
		{
		dialogup=true;
		// now we use 2 dif. dialogs, depending on whether they are just unlocking it vs. or changing it or initially setting it
		if (mircryptor->mc_iskeyfileunlocked(NULL) || mircryptor->countkeys()==0)
			{
			mircryptor->getunlockpassphrase(dialogmasterpassphrase);
			dialognum=IDD_PASSWORDDIALOG_CHANGE;
			doublepass=true;
			}
		else
			{
			dialognum=IDD_PASSWORDDIALOG;
			doublepass=false;
			}
		dialogretv=DialogBox(DllWindow_hinstance, MAKEINTRESOURCE(dialognum), NULL, (DLGPROC)MyDialogProc);
		if (dialogretv==IDOK && lstrcmp(dialogmasterpassphrase,"")!=0)
			{
			// MessageBox(0, dialogmasterpassphrase, "passphrase Try", 0);
			// now set master passphrase if appropriate
			bretv=mircryptor->setunlockpassphrase(dialogmasterpassphrase);
			if (bretv)
				break;
			else
				{
				// passphrase not accepted
				}
			}
		else
			{
			bretv=false;
			break;
			}
		}
	dialogup=false;
	return bretv;
}


bool MircryptionClass_Mirc::send_irccommand(char *irccommand,char *text)
{
	// send an irc command to server
	// returns true on success
	SendMircText (irccommand, text, MircHwnd, 200,0);
	return false;
}


bool MircryptionClass_Mirc::display_statustext(char *messagetext)
{
	// display some information for the user in some default text area (like status window in mirc)
	// returns true on success
	SendMircText ("/statusmsg", messagetext, MircHwnd, 200,0);
	return false;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------


// Matches the command line arguments/options
#define OPT_MODIFY_ATIME       (1 << 0)
#define OPT_MODIFY_MTIME       (1 << 1)
#define OPT_MODIFY_CTIME       (1 << 2)
#define OPT_NO_CREATE	       (1 << 3)
#define OPT_USE_TEMPLATE       (1 << 4)
#define OPT_USER_TIME          (1 << 5)

// The magic function which does all the touching
static DWORD mytouch(LPCTSTR lpszFile, FILETIME* atime, FILETIME* mtime, FILETIME* ctime, WORD wOpts)
{
	SetLastError(ERROR_SUCCESS);

	HANDLE hFile = CreateFile(
		lpszFile, 
		GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		(wOpts & OPT_NO_CREATE) ? OPEN_EXISTING : OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		0);

	DWORD dwRetVal = GetLastError();

	// Check for CreateFile() special cases
	if(dwRetVal != ERROR_SUCCESS) {
		if((wOpts & OPT_NO_CREATE) && dwRetVal == ERROR_FILE_NOT_FOUND)
			return ERROR_SUCCESS; // not an error
		else if(hFile != INVALID_HANDLE_VALUE && dwRetVal == ERROR_ALREADY_EXISTS)
			dwRetVal = ERROR_SUCCESS; // not an error according to MSDN docs
	}

	if(dwRetVal != ERROR_SUCCESS)
		return dwRetVal;

	// Is there any template timestamp?  
	if(atime || mtime || ctime) {
		SetLastError(ERROR_SUCCESS);
		SetFileTime(
			hFile, 
			(wOpts & OPT_MODIFY_CTIME) ? ctime : NULL,
			(wOpts & OPT_MODIFY_ATIME) ? atime : NULL,
			(wOpts & OPT_MODIFY_MTIME) ? mtime : NULL);
		dwRetVal = GetLastError();
	}

	CloseHandle(hFile);

	return dwRetVal;
}

void myTimetToFileTime( time_t t, LPFILETIME pft )
{
	// wierd function found on web to convert time formats - it yields a type conversion warning on compilation but seems to work.
	// though this may fail on non i386 platforms, but it is only used by the optional online updater tool, so *nix users wont use it anyway.

	#if _MSC_VER == 1200
		LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000L;
   	#else
	    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000LL;
	#endif
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}



// $dll(mircryption,mc_touchfile ($time $filename)
MircFunc mc_touchfile (FUNCPARMS)
{
	char* tmp = data;
	char* datestring = _getlcword(tmp);
	char *retcp;
	double datenum;
	
	FILETIME newfiletime;
	time_t atimet;
	datenum=strtod(datestring,&retcp);
	atimet=(time_t)datenum;
	//newfiletime=datestring;
	myTimetToFileTime(atimet,&newfiletime);

	mytouch(tmp,&newfiletime,&newfiletime,&newfiletime,OPT_MODIFY_ATIME | OPT_MODIFY_MTIME | OPT_MODIFY_CTIME | OPT_NO_CREATE);
	return 1;
}
//---------------------------------------------------------------------------


