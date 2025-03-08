//---------------------------------------------------------------------------
#include "MPgpMain.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
USEFORM("PgpDialogUnit.cpp", PgpDialogForm);
USEFORM("OptionsFormUnit.cpp", OptionsForm);
USEFORM("LogFormUnit.cpp", LogForm);
USEFORM("JobFormUnit.cpp", JobForm);
USEUNIT("MiscUtils.cpp");
USEFORM("QuestionFormUnit.cpp", QuestionForm);
USEUNIT("JobManager.cpp");
USEUNIT("Jobs.cpp");
USEUNIT("DerivedJobs.cpp");
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "PgpDialogUnit.h"
#include "JobFormUnit.h"
#include "LogFormUnit.h"
#include "QuestionFormUnit.h"
#include "JobManager.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// remembering mirc/dll stuff
HINSTANCE DllWindow_hinstance;
LPVOID mircdllheap = NULL;
HWND GLOB_aWnd=0;
HWND oldapphandle=0;
// for sending irc window text
HWND ircMapWindow=0;
HANDLE ircMapHandle=0;
LPSTR ircMapData=NULL;
char *ircMapTempTextp=NULL;
char ircMapChannelname[255];
bool ircmap_showederror=false;
// to keep track of initialization
bool DidInitialize=false;
int loadcalls=0;
bool AnnouncedLoad=false;
bool nowquitting=false;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NEW - set to false to prevent 10min mirc unload
bool politeunloadonrequest=false;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// dllMircMode = determines mode of conversation with mirc (see .h file)
bool mircsending=true;
int dllMircMode=MircModeTimer;
AnsiString MircOutputString="";
//---------------------------------------------------------------------------





























//---------------------------------------------------------------------------
/*
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void *lpReserved)
{
	if (reason==DLL_PROCESS_DETACH) && (ThisAPP))
		Application = ThisApp;
	return 1;
}
*/
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//just return a string of info about the dll
const char *get_mircDllInfo() {return MIRCDLL_INFO;}
const char *get_mircDllVersion() {return MIRCDLL_VERSION;}
MircFunc DLLInfo (FUNCPARMS) { dllret(get_mircDllInfo()); }
MircFunc version (FUNCPARMS) { dllret(get_mircDllVersion()); }
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Main load of dll
#pragma argsused
extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	DllWindow_hinstance=hinstDLL;
	if (mircdllheap==NULL)
		{
		// get heap for use if we want to use caller memory allocation
		mircdllheap = GetProcessHeap();
		}
	return TRUE;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
DllExport void __stdcall LoadDll(LOADINFO* loadinfo)
{
	// This function is called when the dll is first accessed - here you reserve any global stuff.
	// Return loadinfo->Keep==true if you want to stay loaded on return
	//  or loadinfo->Keep==false is you can be unloaded immediately.

	// there may be a bug, or its a multithreading thing, that can cause 2 calls to load. we ignore all but first
	++loadcalls;
	if (loadcalls>1)
		{
		// spurious load call?
		ShowMessage("MULTIPLE LOADCALLS: "+AnsiString(loadcalls));
		loadinfo->mKeep = false;
		// loadinfo->mKeep = true;
		return;
		}

	// Initialize
	mpgpInitializeDll();

	// we want to stay loaded so we return true
	loadinfo->mKeep = true;
}


DllExport int __stdcall UnloadDll(int m)
{
	// This function is called when mirc requests that the dll unload.
	// However, sometimes it is ok to turn down this request.
	// if m==1 then the request is a polite, perfunctary request after 10 minutes of inactivity
	//    which you may want to turn down if you need to preserve memory objects.
	// if m!=1 then this is a forcefull request to terminate, and you should terminate.
	// RETURN 0 if you refuse to terminate, or 1 if you have freed up your memory and are willing to unload
	int reply;

	// NOW, for the mpgp dll, we want to politely unload IFF there are no pending outgoing jobs
	if (m==1)
		{
		// polite request to terminate. any outgoing jobs still pending?
		if (jobmanager==NULL)
			reply=1;
		else if (jobmanager->CountJobsPending()>0 || jobmanager->get_BlockedForInput())
			{
			// there are still outgoing jobs pending, so we will deny the polite request to terminate
			reply=0;
			}
		else
			{
			// no outgoing jobs, so we can terminate and we will get triggered automatically when new incoming data comes in
			// send a message to mirc to let it know we are loaded
			// causes crash:
			//			DLLDirectSendMircOutput("/mpgp_IsUnLoading");
			if (politeunloadonrequest)
				reply=1;
			else
				{
				// we deny request to unloac
				reply=0;
				// but we could shut down stuff we dont need
				GentleIntoSleep();
				}
			}
		}
	else
		{
		// we are being FORCED to terminate
		reply=1;
		}

	if (reply==1)
		{
		// unintialize the dll
		dllMircMode=MircModeDisabled;
		mpgpUnInitializeDll();
		}

	return reply;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void mpgpInitializeDll()
{
	// Initalize the dll - call before first run
	if (!DidInitialize)
		{
		Application->CreateForm(__classid(TPgpDialogForm), &PgpDialogForm);
		Application->CreateForm(__classid(TJobForm), &JobForm);
		Application->CreateForm(__classid(TOptionsForm), &OptionsForm);
		Application->CreateForm(__classid(TLogForm), &LogForm);
		Application->CreateForm(__classid(TQuestionForm), &QuestionForm);
		jobmanager=new MPGPJobManager();
		jobmanager->Initialize();
		DidInitialize=true;
		}
	if (GLOB_aWnd!=0)
		{
		// set the parent window for pgp operations

		// TEST:
		oldapphandle=Application->Handle;
//		Application->Handle=GLOB_aWnd;
//		jobmanager->SetPgpWindowHandle(THandle(Application->Handle));

		//
		// OptionsForm->Handle=THandle(Application->Handle);

		//		jobmanager->SetPgpWindowHandle(GLOB_aWnd);
		//		jobmanager->SetPgpWindowHandle(THandle(JobForm->Handle));
		}
}


void mpgpUnInitializeDll()
{
	// Uninitialize the dll
	int stage=0;
	int count;
	static bool firsttime=true;
	double start;

	if (firsttime==false)
		{
		ShowMessage("Warning: for some reason uninit is being called twice!");
		return;
		}
	firsttime=false;

	PgpDialogForm->OutputTimer->Enabled=False;
	QuestionForm->Timer1->Enabled=False;

	/*
	// super wierd mousedown bug fix
	char keyarray[256];
	bool forcewait;
	int trycount=0;
	do
		{
		forcewait=false;

		Application->ProcessMessages();
		GetKeyboardState(keyarray);
		for (count=0;count<255;++count)
			{
			if (keyarray[count]<0)
				{
				forcewait=true;
				keyarray[count]=0;
				}
			}

		if (forcewait==true)
			{
			++trycount;
			if (trycount>20)
				{
				break;
				}
			//			OptionsForm->Caption=AnsiString(trycount);
			start=GetTickCount();
			do
				Application->ProcessMessages();
			while (GetTickCount()<(start+500));
			}
		} while (forcewait);

	nowquitting=true;
	*/



	try
		{
		stage=0;
		//	ircMapClose();

		stage=1;
		/*
		PgpDialogForm->Hide();
		QuestionForm->Hide();
		JobForm->Hide();
		LogForm->Hide();
		OptionsForm->Hide();
		*/

		// global nowquitting flag
		start=GetTickCount();
		do
			Application->ProcessMessages();
		while (GetTickCount()<(start+500));

		stage=2;
		jobmanager->set_wantsquit();

		stage=3;
		Application->ProcessMessages();
		PgpDialogForm->OutputTimer->Enabled=False;
		Application->ProcessMessages();
		QuestionForm->Timer1->Enabled=False;
		Application->ProcessMessages();

		stage=4;
		do
			Application->ProcessMessages();
		while (GetTickCount()<(start+500));

		stage=5;
		if (DidInitialize && jobmanager!=NULL)
			{
			jobmanager->DeInitialize();
			delete jobmanager;
			jobmanager=NULL;
				Application->ProcessMessages();
			DidInitialize=false;
			}

		// new try
		QuestionForm->Close();
		OptionsForm->Close();
		JobForm->Close();
		LogForm->Close();
		PgpDialogForm->Close();
		do
			Application->ProcessMessages();
		while (GetTickCount()<(start+500));

		stage=6;
		Application->ProcessMessages();
		delete QuestionForm;
		QuestionForm=NULL;
		Application->ProcessMessages();
		delete OptionsForm;
		OptionsForm=NULL;
		Application->ProcessMessages();
		delete JobForm;
		JobForm=NULL;
		Application->ProcessMessages();
		delete LogForm;
		LogForm=NULL;
		Application->ProcessMessages();
		delete PgpDialogForm;
		PgpDialogForm=NULL;
		Application->ProcessMessages();

		stage=7;
		do
			Application->ProcessMessages();
		while (GetTickCount()<(start+500));
		return;
		}
	catch (Exception &exception)
		{
		ShowMessage("An mpgp error that we have been trying to eliminate has unfortunately occurred.  One known cause is having mouse button down in mpgp form during unload of mpgp.  Please report this message (and the subsequent exception) to mouser in #mircryption: MPGP.DLL CRASH AT STAGE #"+AnsiString(stage));
		Application->ShowException(&exception);
		}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void GentleIntoSleep()
{
	PgpDialogForm->JYarrow1->ShutdownFrontend();
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
//---------------------------------------------------------------------------























//---------------------------------------------------------------------------
DllExport void __stdcall NoMirc()
{
	// set the mirc mode saying not to try to talk to mirc (for standalone testing/debugging)
	mircsending=false;
}

MircFunc dllPauseOutput (FUNCPARMS)
{
	// dllPauseOutput - pause outputting
	jobmanager->set_Paused(true);
	return 3;
}

MircFunc dllResumeOutput (FUNCPARMS)
{
	// dllResumeOutput - resume outputing
	jobmanager->set_Paused(false);
	return 3;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Present the main pgp addon form
MircFunc dllPresent (FUNCPARMS)
{
	// parse parameters
	char *chan,*tonick,*toident,*fromident,*chankey,*startform;
	char* tmp = data;
	chan = _getlcword(tmp);
	tonick = _getlcword(tmp);
	toident = _getword(tmp);
	fromident = _getword(tmp);
	startform = _getlcword(tmp);
	chankey = tmp;

	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	// invoke the mpgp form
	if (!(jobmanager->get_BlockedForInput()))
		{
		jobmanager->PresentForm(chan,tonick,toident,chankey,fromident);
		if (strcmp(startform,"chankey")==0)
			{
			// default to chat chan
			PgpDialogForm->ForceRadioGroup(1);
			}
		PgpDialogForm->ListBoxTo->ItemIndex=0;
		PgpDialogForm->ListBoxFrom->ItemIndex=0;
		}

	return 3;
}


// Present the jobs dialog
MircFunc dllShowJobs (FUNCPARMS)
{
	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	// show job dialog
	JobForm->Show();

	return 3;
}


// Present the jobs dialog
MircFunc dllShowOptions (FUNCPARMS)
{
	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	// show job dialog
	OptionsForm->Show();

	return 3;
}


// Set the mirc interface mode - which determines whether we do our own asynchronus iterating, or not
MircFunc dllSetMircMode (FUNCPARMS)
{
	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	// get parameters
	char *numstr;
	char* tmp = data;
	numstr = _getlcword(tmp);
	int modenum=_atolp(numstr);

	// set the mode
	dllMircMode=modenum;

	return 3;
}


// shut down stuff not needed
MircFunc dllGentleSleep (FUNCPARMS)
{
	GentleIntoSleep();
	return 3;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
MircFunc dllManualIterate (FUNCPARMS)
{
	// parse parameters
	int maxbytes;
	char *maxbytestr;
	char* tmp = data;
	maxbytestr = _getword(tmp);
	maxbytes=atoi(maxbytestr);

	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	if (jobmanager->get_BlockedForInput())
		{
		// not allowed to iterate when we are presenting user with a question dialog
		jobmanager->LogMessage("blocked.");
		jobmanager->IterateOutput();
		}
	else
		{
		// Timed iterate
		// iterate for a certain number of bytes of output
		jobmanager->Iterate(maxbytes);
		}

	if (MircOutputString=="")
		wsprintf(data,"");
	else
		{
		// now return the output in data (better be maxbytes alloocated to receive it!)
		wsprintf(data,"%s",MircOutputString.c_str());
		// jobmanager->LogMessage("MPGP Outputing: "+MircOutputString);
		// reset global output string flag
		MircOutputString="";
		}

	// return
	return 3;
}

MircFunc dllManualPeek (FUNCPARMS)
{
	// JUST CHECK FOR NEW OUTPUT - DONT ITERATE JOBS

	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	jobmanager->IterateOutput();

	if (MircOutputString=="")
		wsprintf(data,"");
	else
		{
		// now return the output in data (better be maxbytes alloocated to receive it!)
		wsprintf(data,"%s",MircOutputString.c_str());
		// jobmanager->LogMessage("MPGP Outputing: "+MircOutputString);
		// reset global output string flag
		MircOutputString="";
		}

	// return
	return 3;
}


MircFunc dllReceiveInput (FUNCPARMS)
{
	// Receive some input from mirc
	// parse parameters
	char *chan,*tonick,*toident,*fromident,*instring;
	char* tmp = data;

	chan = _getlcword(tmp);
	tonick = _getlcword(tmp);
	toident = _getword(tmp);
	fromident = _getword(tmp);
	instring = tmp;

	//	jobmanager->LogMessage("Received: "+AnsiString(chan)+","+AnsiString(tonick)+","+AnsiString(toident)+","+AnsiString(instring));

	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	AnnounceLoad();

	// save from ident - this is a kind of kludge
	jobmanager->set_fromident(fromident);

	// Queue up the input string for processing
	jobmanager->QueueInputString(chan,tonick,toident,instring);

	// Let it process pending inputs
	DLLShouldWake();
//	if (!(jobmanager->get_BlockedForInput()))
//		jobmanager->IterateInput();

	return 3;
}

MircFunc dllCountJobsPending (FUNCPARMS)
{
	// Return the number of jobs still pending
	// if ==0 then no jobs, no main form showing; if ==-1 then no jobs, BUT main form is still showing

	// save aWnd param from incoming funcparams
	GLOB_aWnd=aWnd;
	// make sure it's initialized
	mpgpInitializeDll();

	// get a count of number of pending jobs and return it
	int jobspending=jobmanager->CountJobsPending();
	if (jobspending==0 && (PgpDialogForm->Visible==true || jobmanager->get_BlockedForInput()))
		{
		// a slight kludge: if main form is visible, we return -1, so caller will no not to unload us
		jobspending=-1;
		}
	wsprintf(data,"%d",jobspending);
	return 3;
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
bool DLLSendMircOutput(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString str)
{
	// This is the function that is called when the addon wants to output a line to mirc
	bool bretv;
	AnsiString sretv;


	if (dllMircMode==MircModeDisabled)
		{
		// Just throw away all output that would normally go to mirc
		bretv=true;
		}
	else if (dllMircMode==MircModeOnRequest || mircsending==false)
		{
		// We add this output to a global string (queue it) for returning during explcit calls to dllManualIterare()
		if (MircOutputString!="")
			{
			// convert \r\n to something - otherwise mirc treats as separate commands
			char thirteenstr[2];
			thirteenstr[0]=13;
			thirteenstr[1]='\0';
			MircOutputString+=AnsiString(thirteenstr);
			}
		MircOutputString+=itonick+","+str;
		bretv=true;
		}
	else
		{
		// For other modes, MircModeTimer, we send the output immediately to mirc
		str="$mpgp_DllOutputs("+itonick+" "+str+")";
		//		jobmanager->LogMessage("MIRCOUT: "+str);

		bretv=SendMircTextRet(str,sretv);
		if (sretv=="0")
			{
			// the data was sent to mirc, but our returning function said it could not be executed
			jobmanager->LogMessage("out to mirc failed.");
			bretv=false;
			}
		}

	return bretv;
}

bool DLLDirectSendMircOutput(AnsiString str)
{
	// This is the function that is called when the addon wants to output a line to mirc
	// we just send this directly to mirc right now, as if the user typed it.
        bool bretv;
	AnsiString sretv;

	if (dllMircMode==MircModeDisabled || mircsending==false)
		{
		// dont send
		return true;
		}

	jobmanager->LogMessage("DirectSend to mirc: "+str);

	bretv=SendMircText(str);

	//	bretv=SendMircTextRet(str,sretv);
	//	if (sretv=="0")
	//		{
	//		// the data was sent to mirc, but our returning function said it could not be executed
	//		bretv=false;
	//		}

	return bretv;
}

void DLLCanSleep()
{
	// this is called by the addon when the joblist is clear - and we want
	// to inform mirc that is can unload us if it really wants to
	DLLDirectSendMircOutput("/mpgp_CanSleep");
	// then we can put our timer to sleep if we want (optional)
//	if (PgpDialogForm->OutputTimer->Enabled)
//		PgpDialogForm->OutputTimer->Enabled=false;
}

void DLLShouldWake()
{
	// this is called by the addon when a job is added so the timer iteration must wake
	// then we should wake our timer
	if (!PgpDialogForm->OutputTimer->Enabled)
		PgpDialogForm->OutputTimer->Enabled=true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool ircMapOpen()
{
	// open the file map to send messages to irc if its not open alread
	if (ircMapData==NULL)
		{
		ircMapHandle=CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,MAXIRCMAPSIZE,"mIRC");
		if (!ircMapHandle)
			{
			if (!ircmap_showederror)
				jobmanager->JMShowMessage("Can't create a file mapping object for the mirc interface.");
			ircmap_showederror=true;
			return false;
			}
		ircMapData=(LPSTR)MapViewOfFile(ircMapHandle, FILE_MAP_ALL_ACCESS,0,0,0);
		if (!ircMapData)
			{
			if (!ircmap_showederror)
				jobmanager->JMShowMessage("Can't map view of the mirc interface file.");
			ircMapHandle=NULL;
			ircmap_showederror=true;
			return false;
			}
		ircMapTempTextp=new char[MAXTEXTBOXLEN];
		}
	return true;
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
bool SendMircText(AnsiString str)
{
	bool bretv;
	int retv;

	bretv=ircMapOpen();
	if (!bretv)
		return false;
	AnnounceLoad();

//	jobmanager->LogMessage("Send to mirc: "+str);

//	wsprintf(ircMapData,"%s",str.c_str());
	lstrcpy(ircMapData,str.c_str());
	retv=SendMessage(GLOB_aWnd, WM_USER + 200,0,0);
	if (retv==0)
		{
		if (!ircmap_showederror)
			jobmanager->JMShowMessage("Error received while trying to send the text to Mirc via SendMessage() api call.");
		ircmap_showederror=true;
		return false;
		}
	return true;
}


bool SendMircTextRet(AnsiString str,AnsiString &sretv)
{
	bool bretv;
	int retv;

	bretv=ircMapOpen();
	if (!bretv)
		return false;
	AnnounceLoad();

//	jobmanager->LogMessage("Sendret to mirc: "+str);

	wsprintf(ircMapData,"%s",str.c_str());
	retv=SendMessage(GLOB_aWnd, WM_USER + 201,0,0);
	if (retv==0)
		{
		if (!ircmap_showederror)
			jobmanager->JMShowMessage("Error received while trying to send the text to Mirc via SendMessage() api call.");
		ircmap_showederror=true;
		sretv="";
		return false;
		}
	// get return value
	sretv=AnsiString(ircMapData);

//	jobmanager->LogMessage("Ret of Sendret to mirc: "+sretv);

	return true;
}                        
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// PARAMETER PARSING HELPER FUNCTIONS
// replacement new and delete, so avoid having to call c lang default lib
//LPVOID __cdecl operator new(size_t n) { return HeapAlloc(mircdllheap,HEAP_ZERO_MEMORY,n); }
//void __cdecl operator delete(LPVOID p) { if (p) HeapFree(mircdllheap,0,p); }
//int __cdecl _purecall() {return 0;}

// Replacement functions so we can avoid linking std c library (makes dll smaller)
int _atolp(char* p)
{
	// string to int
	register int n = 0;
	while (*p >= '0' && *p <= '9') {
		n *= 10;
		n += (*p++ - '0');
	}
	return n;
}

char* _getword(char*& p)
{
	if (!p) { return ""; }
	while (*p == ' ') { p++; }
	char* r = p;
	while (*p && (*p != ' ')) { p++; }
	if (*p) { *p++ = 0; }
	return r;
}
char* _getlcword(char*& p)
{
	// get word and lowercase it in the process
	if (!p) { return ""; }
	while (*p == ' ') { p++; }
	char* r = p;
	while (*p && (*p != ' ')) { mylowercase(p); p++; }
	if (*p) { *p++ = 0; }
	return r;
}
char *_mystrchr(char *p,char c)
{
	// strchr(string,char) return a pointer to first character c in string, or NULL if not found
	while (*p!=c && *p!='\0')
		++p;
	if (*p=='\0')
		return NULL;
	return p;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void AnnounceLoad()
	{
	// causes crash!
	/*
	if (!AnnouncedLoad)
		{
		// send a message to mirc to let it know we are loaded
		DLLDirectSendMircOutput("/mpgp_IsLoading "+AnsiString(MIRCDLL_VERSION)+"");
		AnnouncedLoad=true;
		}
	*/
	}
//---------------------------------------------------------------------------
