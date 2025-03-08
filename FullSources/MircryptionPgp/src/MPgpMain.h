//---------------------------------------------------------------------------
// Mircryption PGP Addon
// MPgp.h
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef MPGPaddonH
#define MPGPaddonH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// #define WIN32_LEAN_AND_MEAN
#include <vcl.h>
#include <windows.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define MIRCDLL_VERSION "1.2"
#define MIRCDLL_INFO "Mircryption PGP Addon DLL"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// mirc sadly has linelength limit of about 900.  worse yet, servers often have even smaller limits like 450, so 400 is safe.
// BUT recently we've discovered mirc can choke on words>400 chars, which can result from strings of about 350 chars after blowfishing
#define MAXIRCSAFELINELEN 250
#define MAXLINELEN 1024
#define CRASHLINELEN 650
#define MAXTEXTBOXLEN 63000
#define MAXIRCMAPSIZE 4096
//#define MAXIRCMAPSIZE 40960
// sleep a few ms between line posts to make sure you dont get flood kicked (works:2,20,1000)
#define MINTEXTSLEEPTIME 2
#define MAXTEXTSLEEPTIME 20
#define TEXTSWITCHPOINT 1000
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Ways this dll can be setup to interact with mirc
// nomirc = similar to onrequest, except we throw away any auxiliary mirc commands (like to change channel)
// disabled = dont try to send anything to mirc - just throw it all away
// onrequest = only iterate when asked to, buffer all output in string returned on interate call
// timeron = use an internal timer to iterate jobs and send output directly to mirc via filamap - timer enabled
// timeroff = use an internal timer to iterate jobs and send output directly to mirc via filamap - timer disabled
#define MircModeDisabled	1
#define MircModeOnRequest	2
#define MircModeTimer		3
//---------------------------------------------------------------------------
extern bool mircsending;
extern int dllMircMode;
extern AnsiString MircOutputString;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Global typedef
typedef struct {
	DWORD mVersion;
	HWND mHwnd;
	BOOL mKeep;
} LOADINFO;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define FUNCPARMS    HWND mWnd, HWND aWnd, char* data, char* parms, BOOL show, BOOL nopause
#define DllExport    extern "C" __declspec(dllexport)
#define MircFunc     extern "C" __declspec(dllexport) int __stdcall
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define dllret(parms) { wsprintf(data,"%s",parms); return 3; }
#define dllret2(params1,params2) { wsprintf(data,"%s %s",params1,params2); return 3; }
#define mylowercase(p); { if (*p>'A' && *p<'Z') *p=*p+32; }
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Forward declarations for helper functions
int _atolp(char* p);
char* _getword(char*& p);
char* _getlcword(char*& p);
char *_mystrchr(char *p,char c);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// EXPORTED DLL FUNCTIONS THAT CAN BE CALLED

// Initial loading and unloading
DllExport void __stdcall LoadDll(LOADINFO* loadinfo);
DllExport int __stdcall UnloadDll(int m);

// set flags saying not to communicate with mirc (for standalone testing/debugging)
DllExport void __stdcall NoMirc();

// Strings returned with info about the dll
MircFunc DLLInfo (FUNCPARMS);
MircFunc version (FUNCPARMS);

// dllPauseOutput - pause outputting
MircFunc dllPauseOutput (FUNCPARMS);

// dllResumeOutput - resume outputing
MircFunc dllResumeOutput (FUNCPARMS);

// dllPresent - present the main form to user and let them create pgp messages to send
MircFunc dllPresent (FUNCPARMS);

// dllShowJobs - show the job list dialog
MircFunc dllShowJobs (FUNCPARMS);

// dllShowOptions - show the options dialog
MircFunc dllShowOptions (FUNCPARMS);

// dllGentleSleep - shutdown stuff not needed
MircFunc dllGentleSleep (FUNCPARMS);

// dllManualIterate() - iterate all pending jobs
MircFunc dllSetMircMode (FUNCPARMS);

// dllManualIterate() - iterate all pending jobs
MircFunc dllManualIterate (FUNCPARMS);

// dllManualPeek() - peeek for new output
MircFunc dllManualIPeek (FUNCPARMS);

// dllReceiveInput() - accept incoming job date
MircFunc dllReceiveInput (FUNCPARMS);

// dllPendingJobs() - returns the number of jobs still pending
// if ==0 then no jobs, no main form showing; if ==-1 then no jobs, BUT main form is still showing
MircFunc dllCountJobsPending (FUNCPARMS);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// other forward declarations
bool DLLSendMircOutput(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString str);
bool DLLDirectSendMircOutput(AnsiString str);
void DLLCanSleep();
void DLLShouldWake();
void mpgpInitializeDll();
void mpgpUnInitializeDll();
bool ircMapOpen();
void ircMapClose();
void AnnounceLoad();
bool SendMircText(AnsiString str);
bool SendMircTextRet (AnsiString str,AnsiString &sretv);
void GentleIntoSleep();
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
