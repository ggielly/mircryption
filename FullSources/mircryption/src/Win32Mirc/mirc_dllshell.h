// mirc_dllhell.h
// Mirc Dll Support Code
// ATTN: i need to add information here about orgincal author of
//   mirc_dllshell, which i have only slightly modified.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// To avoid multiple header instantiations
#ifndef _mirc_dllshellh
#define _mirc_dllshellh
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Compiler directive for dlls
#define WIN32_LEAN_AND_MEAN
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// structure passed by mirc on startup
typedef struct {
	DWORD mVersion;
	HWND mHwnd;
	BOOL mKeep;
} LOADINFO;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Global variables
extern HINSTANCE DllWindow_hinstance;
extern LPVOID mircdllheap;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward declarations for functions you must define
const char *get_mircDllInfo();
const char *get_mircDllVersion();
int do_mircDllUnLoad(int mode);
bool do_mircDllLoad(LOADINFO* loadinfo);
void do_DllStart();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Forward declarations for helper functions provided by mirc_sllshell.cpp
int _atolp(char* p);
char* _getword(char*& p);
char* _getlcword(char*& p);
char *_mystrchr(char *p,char c);
#ifndef lopen
 #define lopen _lopen
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// replacement new and delete, so avoid having to call c lang default lib
LPVOID __cdecl operator new(size_t n);
void __cdecl operator delete(LPVOID p);
int __cdecl _purecall();
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// typedefs
#define FUNCPARMS    HWND mWnd, HWND aWnd, char* data, char* parms, BOOL show, BOOL nopause
#define DllExport    extern "C" __declspec(dllexport)
#define MircFunc     extern "C" __declspec(dllexport) int __stdcall
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// simple inline functions
#define dllret(parms) { wsprintf(data,"%s",parms); return 3; }
#define dllret2(params1,params2) { wsprintf(data,"%s %s",params1,params2); return 3; }
#define mylowercase(p); { if (*p>='A' && *p<='Z') *p=*p+32; }
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
