// MiscUtils.cpp - Some misc. utils

#include "MiscUtils.h"
#include <ShlObj.h>
#include <io.h>
#include <stdio>


bool JrRightStringSplit(AnsiString &mainstr,AnsiString &rightword, const AnsiString &splitstr)
{
	int pos;
	if (mainstr=="")
		{
		rightword="";
		return false;
		}

	pos=mainstr.LastDelimiter(splitstr);
	if (pos==0)
		{
//		rightword=mainstr.Trim();
		rightword=mainstr;
		mainstr="";
		return true;
		}

	if (pos==mainstr.Length())
		rightword="";
	else
		rightword=mainstr.SubString(pos+1,mainstr.Length()-pos);
	if (pos==1)
		mainstr="";
	else
		mainstr=mainstr.SubString(1,pos-1);

//	rightword=rightword.Trim();
	return true;
}


//---------------------------------------------------------------------------
// string splitters - picks off left or right word of string
bool JrLeftStringSplit(AnsiString &mainstr,AnsiString &leftword, const AnsiString &splitstr)
{
	int pos;
	if (mainstr=="")
		{
		leftword="";
		return false;
		}

	pos=mainstr.Pos(splitstr);
	if (pos==0)
		{
//		leftword=mainstr.Trim();
		leftword=mainstr;
		mainstr="";
		return true;
		}
	if (pos==1)
		leftword="";
	else
		leftword=mainstr.SubString(1,pos-1);
	if (pos==mainstr.Length())
		mainstr="";
	else
		mainstr=mainstr.SubString(pos+1,mainstr.Length()-pos);

//	leftword=leftword.Trim();
//	mainstr=mainstr.Trim();
	return true;
}



AnsiString JrExtractPlainFilename(AnsiString filename)
{
	// split off the extension and any leading dir info
	AnsiString tempstr;
	// remove dir info
	filename=ExtractFileName(filename);
	// remove extension
	JrRightStringSplit(filename,tempstr,".");
	if (filename=="")
		filename=tempstr;
	return filename;
}


void JrExtractFilenamePieces(const AnsiString &fullfilename,AnsiString &dirpath,AnsiString &name,AnsiString &extension)
{
	// extract the pieces of the file name
	// remove dir
	dirpath=fullfilename;
	// split into path and filename
	JrRightStringSplit(dirpath,name,"\\");
	// now split off extension from filename
	JrRightStringSplit(name,extension,".");
	if (name=="")
		{
		// when there is no extension we have to fix this up
		name=extension;
		extension="";
		}
}


void JrExtractFilenamePieces(const AnsiString &fullfilename,AnsiString &dirpath,AnsiString &name,AnsiString &extension,AnsiString &argstring)
{
	// extract the pieces of the file name
	// remove dir
        AnsiString remstring;
	AnsiString purefile;
        AnsiString tempstring;
	int len;
	argstring="";

	// keep walking backwards stripping off args until we have removed commandline args
	purefile=fullfilename;
	while (!FileExists(purefile))
		{
		if (!JrRightStringSplit(purefile,tempstring," "))
			break;
		}

	len=purefile.Length();
	if (len==0)
		{
		// we couldnt find any real file
		argstring="";
		purefile=fullfilename;
		}
	else if (len<fullfilename.Length())
		{
		// ok we found some commandline args
		argstring=fullfilename.SubString(len+2,fullfilename.Length()-(len+1));
		}

	// split into path and filename
	dirpath=purefile;
	JrRightStringSplit(dirpath,name,"\\");
	// now split off extension from filename
	JrRightStringSplit(name,extension,".");
	if (name=="")
		{
		// when there is no extension we have to fix this up
		name=extension;
		extension="";
		}
}



bool JrNoSpaces(AnsiString &str)
{
	// remove all spaces in string, and return true if there were any to remove
	int count;
	int len=str.Length();
	int c;
	int targetpos=1;

	for (count=1;count<=len;++count)
		{
		c=str[count];
		if (c==32)
			continue;
		str[targetpos++]=c;
		}
	if (targetpos<=len)
		{
                // truncate it
		str.SetLength(targetpos-1);
		return true;
		}
	return false;
}


// simple wildcard compare, from web: http://www.codeproject.com/useritems/wildcmp.asp
// wildcard string compare (globbing)
// By Jack Handy
// matches a string against a wildcard string such as "*.*" or "bl?h.*" etc. This is good for file globbing or to match hostmasks.
bool wildcmp(char *wild, char *string)
	{
	char *cp, *mp;

	while ((*string) && (*wild != '*'))
		{
		if ((*wild != *string) && (*wild != '?'))
			return false;
		wild++;
		string++;
		}

	while (*string)
		{
		if (*wild == '*')
			{
			if (!*++wild)
				return true;
			mp = wild;
			cp = string+1;
			}
		else if ((*wild == *string) || (*wild == '?'))
			{
			wild++;
			string++;
			}
		else
			{
			wild = mp;
			string = cp++;
			}
		}

	while (*wild == '*')
		wild++;

	if (*wild=='\0')
		return true;
	return false;
}



// lookup a system path, from http://www.bcbdev.com/faqs/faq80.htm
//	CSIDL_DESKTOP for WINDOWS\Desktop
//	CSIDL_DESKTOPDIRECTORY for WINDOWS\Desktop
//	CSIDL_FONTS for WINDOWS\FONTS
//	CSIDL_NETHOOD for WINDOWS\NetHood
//	CSIDL_PERSONAL for X:\My Documents
//	CSIDL_PROGRAMS for WINDOWS\StartMenu\Programs
//	CSIDL_RECENT for WINDOWS\Recent
//	CSIDL_SENDTO for WINDOWS\SendTo
//	CSIDL_STARTMENU for WINDOWS\Start Menu
//	CSIDL_STARTUP for WINDOWS\Start Menu\Programs\StartUp
//	CSIDL_TEMPLATES for WINDOWS\ShellNew
// #define CSIDL_STARTUP                   0x0007        // Start Menu\Programs\Startup
// #define CSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
AnsiString GetSystemPath(int foldernum)
{
    LPITEMIDLIST  pidl;
    LPMALLOC      pShellMalloc;
    char          szDir[MAX_PATH];
    AnsiString retstring="";

    // SHGetSpecialFolderLocation generates a PIDL. The memory for the PIDL
    // is allocated by the shell, and should be freed using the shell
    // mallocator COM object. Use SHGetMalloc to retrieve the malloc object
    if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
    {
        // if we were able to get the shell malloc object, then
        // proceed by fetching the pidl for the windows desktop
        if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,foldernum, &pidl)))
        {
            // return is true if success
            if(SHGetPathFromIDList(pidl, szDir))
            {
                // set one caption to the directory path
                retstring= szDir;
            }
            pShellMalloc->Free(pidl);
        }

        pShellMalloc->Release();
    }
    return retstring;
}




// create shortcuts, borrowed mostly from http://www.codeguru.com/mfc/comments/673.shtml
// Firo, Software Engineer, AutoCAD Engineering, Autodesk, Inc.
//CreateShortcut("explorer.exe", "c:\\temp", "Open The Temp Directory");
//	    * Creates a shortcut on the desktop that runs "explorer.exe c:\temp"
//CreateShortcut("", "", "Desktop Folder", TRUE);
//	    * Creates a 'Desktop Folder' folder on the desktop
//CreateShortcut("notepad.exe", "c:\\temp\\file.txt", "Desktop Folder\\Edit the File");
//	    * Creates a shortcut IN 'Desktop Folder' on the desktop that runs "notepad.exe c:\temp\file.txt"

/*
bool CreateShortcut(const AnsiString Target, const AnsiString Arguments, const AnsiString LinkFileName, bool IsFolder)
{
  HRESULT hres;
  AnsiString Link=LinkFileName;

  if (!IsFolder)
  {
	IShellLink* psl;
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);

	if (SUCCEEDED(hres))
	{
	  IPersistFile* ppf;
	  psl->SetPath(Target.c_str());
		return true;
	  psl->SetArguments(Arguments.c_str());
	  hres = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);

		if (SUCCEEDED(hres) && false)
		 {
		   AnsiString Temp = Link.LowerCase();;
		   if (Temp.Pos(".lnk")==-1)
			Temp=Temp+AnsiString(".lnk");
		   //WORD wsz[MAX_PATH];
		   wchar_t wsz[MAX_PATH];
		   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Link.c_str(), -1, wsz, MAX_PATH);
		   hres = ppf->Save(wsz, TRUE);
		   ppf->Release();
	           hres=true;
		 }
	  psl->Release();
	}
//	else
//	exit(1);
  }
  else
	hres = CreateDir(Link);	   //Create the folder on the desktop

  return hres;
}
*/


void CreateShortCut(const AnsiString &file, const AnsiString &workingdir, const AnsiString &arguments, const AnsiString &shortcutfile, const AnsiString &description)
{
    IShellLink* pLink;
    IPersistFile* pPersistFile;

    if(SUCCEEDED(CoInitialize(NULL)))
    {
        if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IShellLink, (void **) &pLink)))
        {
            pLink->SetPath(file.c_str());
            pLink->SetDescription(description.c_str());
            pLink->SetWorkingDirectory(workingdir.c_str());
            pLink->SetArguments(arguments.c_str());
            pLink->SetShowCmd(SW_SHOW);

            if(SUCCEEDED(pLink->QueryInterface(IID_IPersistFile,
                                               (void **)&pPersistFile)))
            {

                WideString strShortCutLocation(shortcutfile.c_str());
                pPersistFile->Save(strShortCutLocation.c_bstr(), TRUE);
                pPersistFile->Release();
            }
            pLink->Release();
        }

        CoUninitialize();
    }
}





bool ReadShortCut(const AnsiString &shortcutfile, AnsiString *file, AnsiString *workingdir, AnsiString *arguments, AnsiString *description)
{
	// read the properties of a shortcut
	IShellLink* pLink;
	IPersistFile* pPersistFile;
	WIN32_FIND_DATA wfd;

	//  is hwnd important?
	//	HWND hWnd=NULL;

	// we realy only need to reuse one of these, but who cares
	char lpszTempString[MAX_PATH];

	// results
	HRESULT hres;

	// clear by default
	if (file!=NULL)
		*file="";
	if (description!=NULL)
		*description="";
	if (workingdir!=NULL)
		*workingdir="";
	if (arguments!=NULL)
		*arguments="";

	hres=CoInitialize(NULL);
	if (SUCCEEDED(hres))
		{
		hres=CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **) &pLink);
		if (SUCCEEDED(hres))
			{
			hres=pLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
			if (SUCCEEDED(hres))
				{
				WideString strShortCutLocation(shortcutfile.c_str());
				hres=pPersistFile->Load(strShortCutLocation.c_bstr(), STGM_READ);
				if (SUCCEEDED(hres))
					{
					// it will hang here for a long time if link points to non-existent file
					// what does this do?
					// hres=pLink->Resolve(hWnd, SLR_ANY_MATCH | SLR_NO_UI);
					}
				if (SUCCEEDED(hres) && (file!=NULL))
					{
					// Get the path of the file the link points to.
					hres=pLink->GetPath(lpszTempString, MAX_PATH, &wfd, SLGP_SHORTPATH);
					*file=AnsiString(lpszTempString);
					}
				if (SUCCEEDED(hres) && (description!=NULL))
					{
					// get the description of the shortcut
					hres = pLink->GetDescription(lpszTempString, MAX_PATH);
					*description=AnsiString(lpszTempString);
					}
				if (SUCCEEDED(hres) && (workingdir!=NULL))
					{
					// get the description of the shortcut
					hres = pLink->GetArguments(lpszTempString, MAX_PATH);
					*arguments=AnsiString(lpszTempString);
					}
				if (SUCCEEDED(hres) && (arguments!=NULL))
					{
					// get the description of the shortcut
					hres = pLink->GetWorkingDirectory(lpszTempString, MAX_PATH);
					*workingdir=AnsiString(lpszTempString);
					}
				pPersistFile->Release();
				}
			pLink->Release();
			}
		CoUninitialize();
		}
	if (SUCCEEDED(hres))
		return true;
	return false;
}




AnsiString JrExtractShortPathName(const AnsiString &str)
{
	AnsiString lcs=ExtractShortPathName(str).LowerCase();
	return lcs;
}



// convert string to int
bool JRStringToInt(AnsiString string,int &intval)
{
	string=string.Trim();
	try
		{
		intval=string.ToInt();
		}
	catch (...)
		{
		return false;
		}
	return true;
}


void JRStringReplace(AnsiString &string,AnsiString fromword,AnsiString toword)
{
	// replace instances of from with to
	int stringlen;
	int startpos;
	AnsiString firsthalf,secondhalf;
	int fromwordlen;
	int cpos;

	if (fromword=="" || string=="" || toword.Pos(fromword)>0)
		return;

	stringlen=string.Length();
	fromwordlen=fromword.Length();

//	DebugLog->Display("replace in="+AnsiString(string));

	while ((startpos=string.Pos(fromword))>0)
		{
		if (startpos-1>0)
			firsthalf=string.SubString(1,startpos-1);
		else
			firsthalf="";
		cpos=startpos+(fromwordlen-1);
		if (stringlen-cpos>0)
			secondhalf=string.SubString(cpos+1,stringlen-cpos);
		else
			secondhalf="";
		string=firsthalf+toword+secondhalf;
		stringlen=string.Length();
		}
//	DebugLog->Display("replace out="+AnsiString(string));
}



// file length
int JRFileLength(AnsiString filename)
{
	FILE *filep;
	int flen;

	// open file
	filep=fopen(filename.c_str(),"rb");
	if (filep==NULL)
		return 0;

	// get file length
	flen=filelength(fileno(filep));
	fclose(filep);

	return flen;
}


// find a unique version of the filename
AnsiString JRUniquifyFilename(AnsiString filename)
{
	int count;
	AnsiString numstr;
	AnsiString dirpath,name,extension,prefix,suffix;
	AnsiString tryfilename;

	if (!FileExists(filename))
		return filename;

	JrExtractFilenamePieces(filename,dirpath,name,extension);
	prefix=dirpath;
	if (prefix!="")
		prefix+="\\";
	prefix+=name+"_";
	if (extension!="")
		suffix="."+extension;
	else
		suffix="";

	for (count=2;count<9999;++count)
		{
		numstr=AnsiString(count);
		tryfilename=prefix+numstr+suffix;
		if (!FileExists(tryfilename))
			return tryfilename;
		}
	return filename;
}


int JRMakeSureFileExists(AnsiString filename)
{
	// if file doesnt exist, make it
	// returns 0 on file already exists
	// returns 1 if we made it
	// returns -1 if we failed to make it

	if (FileExists(filename))
		return 0;
	int fhandle=FileCreate(filename);
	if (fhandle>=0)
		{
		FileClose(fhandle);
		return 1;
		}
	return -1;
}


bool JRSafeDeleteFile(AnsiString filename)
{
	// overwrite then delete - note that file NAME could still be recovered

	if (!FileExists(filename))
		return false;

	char blankstring[]="ERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASEDERASED";
	int blanklen=strlen(blankstring);
	FILE *filep;
	filep=fopen(filename.c_str(),"r+b");
	if (filep==NULL)
		return false;
	fseek(filep,0,SEEK_SET);
	unsigned int filebytes=filelength(fileno(filep));
	int thiswrite;
	while (filebytes>0)
		{
		if (filebytes<strlen(blankstring))
			thiswrite=filebytes;
		else
			thiswrite=blanklen;
		fwrite(blankstring,thiswrite,1,filep);
		filebytes-=thiswrite;
		}
	fclose(filep);
	DeleteFile(filename);
	return true;
}


AnsiString JRPrettySizeString(int size)
{
	float gigs=1000.0*1000.0*100024.0;
	float megs=1000.0*1000.0;
//	float kilos=1024.0;
	float kilos=1000.0;
	char nstring[255];
	float rempart;

	if (size>=gigs)
		{
		rempart=float(size/gigs)-int(size/gigs);
		if (rempart>=0.1)
			sprintf(nstring,"%.1f",(size/gigs));
		else
			sprintf(nstring,"%d",int(size/gigs));
		return AnsiString(nstring)+"gb";
		}
	if (size>=megs)
		{
		rempart=float(size/megs)-int(size/megs);
		if (rempart>=0.1)
			sprintf(nstring,"%.1f",(size/megs));
		else
			sprintf(nstring,"%d",int(size/megs));
		return AnsiString(nstring)+"mb";
		}
	if (size>=kilos)
		{
		rempart=float(size/kilos)-int(size/kilos);
		if (rempart>0.1)
			sprintf(nstring,"%.1f",(size/kilos));
		else
			sprintf(nstring,"%d",int(size/kilos));
		return AnsiString(nstring)+"kb";
		}
	return AnsiString(size)+" bytes";
}


void JrSetCurrentBaseDir(AnsiString &dir)
{
	// set new base (current) directory
	//	chdir(dir.c_str());
	SetCurrentDir(dir);
}
