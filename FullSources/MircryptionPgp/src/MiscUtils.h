// MiscUtils.h - Some misc. utils

//---------------------------------------------------------------------------
#ifndef MiscUtilsH
#define MiscUtilsH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>



// string splitters - picks off left or right word of string
bool JrRightStringSplit(AnsiString &mainstr,AnsiString &rightword, const AnsiString &splitstr);
bool JrLeftStringSplit(AnsiString &mainstr,AnsiString &leftword, const AnsiString &splitstr);

// split off the extension and any leading dir info
AnsiString JrExtractPlainFilename(AnsiString filename);
//void JrExtractFilenamePieces(const AnsiString &fullfilename,AnsiString &dirpath,AnsiString &name,AnsiString &extension);
void JrExtractFilenamePieces(const AnsiString &fullfilename,AnsiString &dirpath,AnsiString &name,AnsiString &extension,AnsiString &argstring);


// remove all spaces in string, and return true if there were any to remove
bool JrNoSpaces(AnsiString &str);

// simple wildcard compare (not regex)
bool wildcmp(char *wild, char *string);


// lookup a system path, manually copied defines from <ShlObj.h> since it was erroring when i tried to include it
#define myCSIDL_STARTUP                   0x0007        // Start Menu\Programs\Startup
#define myCSIDL_PROGRAMS                  0x0002        // Start Menu\Programs
#define myCSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
#define myCSIDL_STARTMENU                 0x000b        // <user name>\Start Menu
#define myCSIDL_RECENT                    0x0008        // <user name>\Recent
#define myCSIDL_PERSONAL                  0x0005        // My Documents
#define myCSIDL_DESKTOP                   0x0000	// my desktop
#define myCSIDL_FAVORITES                 0x0006        // <user name>\Favorites
#define myCSIDL_PROGRAMS                  0x0002        // Start Menu\Programs
#define myCSIDL_COMMON_STARTMENU          0x0016        // All Users\Start Menu
#define myCSIDL_COMMON_PROGRAMS           0X0017        // All Users\Programs
#define myCSIDL_COMMON_DESKTOPDIRECTORY   0x0019        // All Users\Desktop
#define myCSIDL_COMMON_DOCUMENTS          0x002e        // All Users\Documents
AnsiString GetSystemPath(int foldernum);

// create a shortcut
void CreateShortCut(const AnsiString &file, const AnsiString &workingdir, const AnsiString &arguments, const AnsiString &shortcutfile, const AnsiString &description);

// read a shortcut
bool ReadShortCut(const AnsiString &shortcutfile, AnsiString *file, AnsiString *workingdir, AnsiString *arguments, AnsiString *description);

// get lowercase 8.3 version of file name
AnsiString JrExtractShortPathName(const AnsiString &str);

// convert string to int
bool JRStringToInt(AnsiString string,int &intval);

// replace instances of from with to
void JRStringReplace(AnsiString &string,AnsiString fromword,AnsiString toword);

// file length
int JRFileLength(AnsiString filename);

// find a unique version of the filename
AnsiString JRUniquifyFilename(AnsiString filename);

// if file doesnt exist, make it
int JRMakeSureFileExists(AnsiString filename);

// overwrite the file with data then delete
bool JRSafeDeleteFile(AnsiString filename);

// return size in bytes, k,mb,gb, whatever most appropriate
AnsiString JRPrettySizeString(int size);

// set base (current) directory
void JrSetCurrentBaseDir(AnsiString &dir);
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
