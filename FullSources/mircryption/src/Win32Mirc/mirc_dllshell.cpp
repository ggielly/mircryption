//---------------------------------------------------------------------------
// mirc_dllshell.cpp
// Mirc Dll Support Code
// ATTN: i need to add information here about orgincal author of
//   mirc_dllshell, which i have only slightly modified.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "mirc_dllshell.h"
#include <stdio.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// LINKER NOTE
//  This project is compiled with MS Visual C++ 6
//  The use of streaming file io funcitons like fopen requires linking to
//   a library.  I have found 2 options that work.  First, linking with
//   MSVCRT.LIB, and second, linking with default libraries.  These options
//   are set in Project->Settings->Link->General.
// 
// If you link with standard libraries, the .dll will be 30k bigger, but it
//  will no longer have an import dependency on the MVCRT.DLL library.
// If you choose to go this route (i dont know when to prefer one way over
//  another), you need a dummy declaration for main in order to avoid an
//  error. If you link with MSVCRT.LIB you dont need this and you can
//  comment it out.
//
// void main(int argc,void **argv)  { ;};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Global variables
HINSTANCE DllWindow_hinstance;
LPVOID mircdllheap=NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Windows DLL Main()
//extern "C" BOOL WINAPI _DllMainCRTStartup(HINSTANCE windowhinstance,DWORD,LPVOID)
extern "C" BOOL WINAPI DllMain(HINSTANCE windowhinstance,DWORD,LPVOID)
{
	DllWindow_hinstance=windowhinstance;
	if (mircdllheap==NULL)
		{
		mircdllheap = GetProcessHeap();
		do_DllStart();
		}
	return TRUE;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// replacement new and delete, so avoid having to call c lang default lib
LPVOID __cdecl operator new(size_t n) { return HeapAlloc(mircdllheap,HEAP_ZERO_MEMORY,n); }
void __cdecl operator delete(LPVOID p) { if (p) HeapFree(mircdllheap,0,p); }
int __cdecl _purecall() {return 0;}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
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
MircFunc DLLInfo (FUNCPARMS) { dllret(get_mircDllInfo()); }
MircFunc version (FUNCPARMS) { dllret(get_mircDllVersion()); }

DllExport void __stdcall LoadDll(LOADINFO* loadinfo)
{
	// This function is called when the dll is first accessed - here you reserve any global stuff.
	// Return loadinfo->Keep==true if you want to stay loaded on return
	//  or loadinfo->Keep==false is you can be unloaded immediately.
	loadinfo->mKeep = do_mircDllLoad(loadinfo);
}

DllExport int __stdcall UnloadDll(int m)
{
	// This function is called when mirc requests that the dll unload.
	// However, sometimes it is ok to turn down this request.
	// if m==1 then the request is a polite, perfunctary request after 10 minutes of inactivity
	//    which you may want to turn down if you need to preserve memory objects.
	// if m!=1 then this is a forcefull request to terminate, and you should terminate.
	// RETURN 0 if you refuse to terminate, or 1 if you have freed up your memory and are willing to unload
	return do_mircDllUnLoad(m);
}
//---------------------------------------------------------------------------
