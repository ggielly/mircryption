//---------------------------------------------------------------------------
// Recursive Directory Size Comparer
// 12/16/02
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "RecursiveFileSizer.h"
#include <stdio.h>
//#include <stdafx.h>
#include <string>
//#include <windows.h>
//using namespace std;

#include "MainFormUnit.h"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
using namespace std;

double DirFinder::RecursiveFileSize(string directory, const string &filefilterstring, bool recursive, long &filecount, long &foldercount, long &smallestfile, long &biggestfile)
{
	// This method recursively searches the directory specified by the parameter and all
	// of its subdirectories (if recursive set).
	// The directory name is assumed to end with a backslash.
	string fullfilename;
	double recursivesize=0L;
	double dirsize=0;
	double fsize;
	string fullfilter=filefilterstring;
	string curfilter;

	if (wantsquit)
		return recursivesize;

	if (directory.length()>0)
		{
		// trim final slash
		if (directory[directory.length()-1]=='\\')
			directory=directory.substr(0,directory.length()-1);
		}

	WIN32_FIND_DATA filedata;  // Structure for file data
	HANDLE filehandle;         // Handle for searching

	// Search files
	while (RightStringSplit(fullfilter,curfilter,';'))
		{
		filehandle = FindFirstFile ((directory+ "\\"+curfilter).c_str (), &filedata) ;
		if (filehandle != INVALID_HANDLE_VALUE)
			{
			do
				{
				if ((filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
					// Directory
					continue;
					}
				else
					{
					// File
					// add the size of the file
					fsize=((double)MAXDWORD * filedata.nFileSizeHigh) + filedata.nFileSizeLow;
					recursivesize+=fsize;
					++filecount;
					if (fsize<smallestfile || smallestfile==-1)
							smallestfile=fsize;
					if (fsize>biggestfile || smallestfile==-1)
							biggestfile=fsize;
					MainForm->set_lastfilesize(fsize);
//					MainForm->set_lastfiledatestring( TDateTime(((ULARGE_INTEGER)(filedata.ftLastWriteTime) / 8.64E11) - 109205.0).DateString());
					ProcessFile(directory.c_str(),filedata.cFileName);
					}
				if (wantsquit)
					break;
				} while (FindNextFile (filehandle, &filedata));
			FindClose (filehandle);
			}
		MainForm->StatusBar->SimpleText="Scanning "+AnsiString(directory.c_str());
		Application->ProcessMessages();
		}


	// Search subdirectories
	filehandle = FindFirstFile ((directory+ "\\*.*").c_str (), &filedata) ;
	if (filehandle != INVALID_HANDLE_VALUE && !wantsquit)
		{
		do
			{
			if ((filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
				// Directory
				if (recursive && strcmp(filedata.cFileName,".")!=0 && strcmp(filedata.cFileName,"..")!=0)
					{
					// Recursive call here
					dirsize=RecursiveFileSize(directory+"\\"+filedata.cFileName,filefilterstring,true,filecount,foldercount,smallestfile,biggestfile);
					recursivesize+=dirsize;
					++foldercount;
					}
				}
			if (wantsquit)
				break;
			} while (FindNextFile (filehandle, &filedata));
		FindClose (filehandle);
		}


  	// done searching
  	return recursivesize;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
string DirFinder::get_bytestring(double bytes_searched, bool parenshow)
	{
	// display nice user readable count of disk space searched
	double gigs=1024.0*1024.0*1024.0;
	double megs=1024.0*1024.0;
	double kilos=1024.0;
	char nstring[255];

	if (bytes_searched>=gigs)
		{
		if (parenshow)
			{
			sprintf(nstring," (%.2f",(bytes_searched/gigs));
			return string(nstring)+"gb)";
			}
		else
			{
			sprintf(nstring,"%.2f",(bytes_searched/gigs));
			return string(nstring)+"gb";
			}
		}
	if (bytes_searched>=megs)
		{
		if (parenshow)
			{
			sprintf(nstring," (%.2f",(bytes_searched/megs));
			return string(nstring)+"mb)";
			}
		else
			{
			sprintf(nstring,"%.2f",(bytes_searched/megs));
			return string(nstring)+"mb";
			}
		}
	if (bytes_searched>=kilos)
		{
		if (parenshow)
			{
			sprintf(nstring," (%.2f",(bytes_searched/kilos));
			return string(nstring)+"kb)";
			}
		else
			{
			sprintf(nstring,"%.2f",(bytes_searched/kilos));
			return string(nstring)+"kb";
			}			
		}
	if (parenshow)
		{
		return string("");
		//sprintf(nstring," (%.0f",bytes_searched);
		//return string(nstring)+"b)";
		}
	else
		{
		sprintf(nstring,"%.0f",bytes_searched);
		return string(nstring)+"b";
		}
	//return string("0 bytes");
	}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
string DirFinder::get_commabytestring(double bytecount)
{
	string retstring="";
	char nstring[255];
	sprintf(nstring,"%.0f",bytecount);
	int len;

	// now add commas
	for (;;)
		{
		len=strlen(nstring);
		if (len<=3)
			break;
		retstring=string(",")+string(&nstring[len-3])+retstring;
		nstring[len-3]='\0';
		}
	if (len>0)
		retstring=string(nstring)+retstring;

	retstring+=" bytes";
	return retstring;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool DirFinder::RightStringSplit(string &mainstr,string &rightword, char splitchar)
{
	int pos;
	if (mainstr=="")
		return false;

	pos=mainstr.length()-1;
	while (pos>=0 && mainstr[pos]!=splitchar)
		--pos;
	if (pos<0)
		{
		rightword=mainstr;
		mainstr="";
		return true;
		}

	if (pos==mainstr.length()-1)
		rightword="";
	else
		rightword=mainstr.substr(pos+1,mainstr.length()-(pos+1));
	if (pos==0)
		mainstr="";
	else
		mainstr=mainstr.substr(0,pos);

	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void DirFinder::ProcessFile(const char *dirname,const char *filename)
{
	// process a file
	MainForm->FoundAFile(AnsiString(dirname)+"\\"+filename);
}
//---------------------------------------------------------------------------