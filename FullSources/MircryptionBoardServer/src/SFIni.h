// Hierarchical Configuration File Manager
// SFini.h
// 8/18/02
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//#include <stdio>
#include <string>
using namespace std;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// To prevent recursive defines
#ifndef SFInih
#define SFInih
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Forward Declarations
class SFIniManager;
class SFIniEntry;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Some string defines
#define SYM_STARTNODE	"{"
#define SYM_ENDNODE		"}"
#define SYM_ASSIGN		" = "
#define SYM_COMMENT		"//"
#define MAXLINELEN		2000
#define MAXIRCLINELEN	400
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// windows
#ifdef WIN32
 #define PATHSEPCHAR	'\\'
 #define PATHSEPSTR		"\\"
#endif

// *nix
#ifndef PATHSEPCHAR
 #define PATHSEPCHAR	'/'
 #define PATHSEPSTR		"/"
#endif

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define String string
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class SFIniManager
{
protected:
	String rootprefix;
	String rootvalue;
	String lastfilename;
protected:
	SFIniEntry *rootp;
public:
	SFIniManager();
	~SFIniManager();
public:
	SFIniEntry *get_rootp() {return rootp;};
public:
	void ClearAndInitialize(String prefix,String value);
	void ClearAndInitialize() {ClearAndInitialize(rootprefix,rootvalue);};
	void DeleteTree();
public:
	bool ReadFile(String filename);
	bool WriteFile(String filename);
	bool WriteFile() {return WriteFile(lastfilename);};
public:
	static void TrimWhiteSpace(char *line);
	static bool SplitLineToStrings(char *line,char *sep,String &prefix,String &value);
	static bool SplitLineToChars(char *line,char *sep,char *&prefix,char *&value);
	static bool SplitStringToStrings(const String &instr,const String &sepstring,String &prefix,String &value);
	static String ExtractDirPath(String fullfilename);
	//	bool RightStringSplit(String &mainstr,String &rightword, const String &splitstr);
//	bool LeftStringSplit(String &mainstr,String &leftword, const String &splitstr);
public:
	static SFIniEntry *GetNextFlattenedNode(SFIniEntry *curp, int &depth, bool explorechildren);
public:
	bool MakeTestTree();
public:
	static String DateNumToString(long dateval);
	static void StringTrim(String &str);
public:
	String get_filename() {return lastfilename;};
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class SFIniEntry
{
	// SFIniEntry contains one entry from the ini file, either a section heading [] or an actual entry
protected:
	String prefix;
	String value;
protected:
	SFIniEntry *prevp;
	SFIniEntry *nextp;
	SFIniEntry *childp;
	SFIniEntry *parentp;
public:
	SFIniEntry(String inprefix,String invalue);
	~SFIniEntry();
public:
	SFIniEntry *LookupChildren(const String &prefixmask,const String &valuemask,int index=1,bool recursive=false);
	String get_ChildValue(const String &prefixname);
	void set_ChildValue(const String &prefixname,const String &val);
	void add_ChildValue(const String &prefixname,const String &val);
	int CountChildren(const String &prefixmask,const String &valuemask,bool recursive);
	int FindChildIndex(const String &prefixmask,const String &valuemask);
	bool MaskMatch(String prefixmask,String valuemask);
public:
	SFIniEntry *InsertAfter(SFIniEntry *entrytoinsert);
	SFIniEntry *InsertBefore(SFIniEntry *entrytoinsert);
	SFIniEntry *InsertChildAtBegining(SFIniEntry *entrytoinsert);
	SFIniEntry *InsertChildAtEnd(SFIniEntry *entrytoinsert);
	SFIniEntry *InsertChildAlphabetically(SFIniEntry *entrytoinsert, bool tieabove=false);
	bool SpliceOut();
public:
//	LookupPath(String nodepath);
public:
	String get_prefix() {return prefix;};
	String get_value() {return value;};
	SFIniEntry *get_prevp() {return prevp;};
	SFIniEntry *get_nextp() {return nextp;};
	SFIniEntry *get_childp() {return childp;};
	SFIniEntry *get_parentp() {return parentp;};
	void set(const String &inprefix,const String &invalue) {prefix=inprefix;value=invalue;};
	void set_value(const String &invalue) {value=invalue;};
public:
	bool get_valueint(int &intval);
	bool get_valuefloat(float &floatval);
	bool isnewer(long lastcheckdate,String userident);
public:
	bool GetUpwardValue(const String &varname,String &retstr);
public:
	String GetAuthorString();
	String GetAuthorName();
	String GetAuthorDate();
public:
	void StringToLower(String &instr);
};
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
