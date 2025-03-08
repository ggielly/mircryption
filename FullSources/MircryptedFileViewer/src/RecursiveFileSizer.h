//---------------------------------------------------------------------------
// Recursive Directory Size Comparer
// 12/16/02
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
using namespace std;

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef RecursiveFileSizer_H
#define RecursiveFileSizer_H
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Forward declarations
class DirFinder
{
	bool wantsquit;
public:
	double RecursiveFileSize(string directory, const string &filefilterstring, bool recursive,long &filecount, long &foldercount, long &smallestfile, long &biggestfile);
	string get_bytestring(double bytes_searched, bool parenshow);
	string get_commabytestring(double bytecount);
public:
	void set_wantsquit(bool val) {wantsquit=val;};
protected:
	bool RightStringSplit(string &mainstr,string &rightword,  char splitchar);
public:
	void ProcessFile(const char *dir,const char *filename);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#endif
