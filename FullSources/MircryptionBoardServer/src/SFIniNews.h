//---------------------------------------------------------------------------
// SFIniNews.h
// Derived class for managing news
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// To prevent recursive defines
#ifndef SFIniNewsh
#define SFIniNewsh
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "SFIni.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define OUTMODE_EGGDROP		1
#define OUTMODE_MIRC		2

#define MAX_POINTSDESCLEN	340
#define PROGRAMVERSION		"1.08.06"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class SFIniNewsManager : public SFIniManager
{
protected:
	FILE *logfile;
	bool paranoidmode;
	bool detailednewtracking;
	int outputmode;
	bool directchan;
	bool publicspam;
public:
	SFIniNewsManager();
	~SFIniNewsManager();
public:
	void MakeDefaultNews();
	SFIniEntry *MakeNewsItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemtext);
	SFIniEntry *MakeNewsSection(const String &section,const String &label);
public:
	bool Parse(int arg_outputmode,const String &enick,const String &password,const String &chan,const String &nick,const String &ident,const String &userclass,const String &newcomstring);
public:
	bool AddSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &label);
	bool DelSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section);
	bool DelSectionNum(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const int secnum);
	bool MovSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &toabovesection);
	bool RepSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &newsection,const String &newlabel);
	bool ClearSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section);
	bool ClearSectionByDays(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &section, int daycount,bool announce);
	bool ClearSectionByCount(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &section, int keepcount,bool announce);
	bool ClearByDays(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,int daycount,bool announce);
	bool ClearByCount(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,int keepcount,bool announce);
	//
	SFIniEntry *AddItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemtext,bool announce, bool checkperms);
	bool DelItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index);
	bool MovItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index,const String &newsection);
	bool RepItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index,const String &newitemtext);
	//
	bool SetSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname,const String &varval);
	bool GetSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname);
	bool AddSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname);
	bool DelSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname);
	bool SetBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname,const String &varval);
	bool GetBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname);
	bool AddBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname);
	bool DelBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname);
	bool ListVars(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password);
	//
public:
	bool AddPoints(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemname, String reasonstr, int points);
	void ParseScoreEntry(String str,String &iname,String &idesc,int &iscore);
	SFIniEntry *MakeScoreEntry(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemname,const String &reasonstr,const int points);
	//
	bool AddActivity(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemname, String reasonstr);
	void ParseActivityEntry(String str,String &iname,String &idesc);
	SFIniEntry *MakeActivityEntry(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemname,const String &reasonstr);
public:
	void AutoCleanupBoard(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password);
	void AutoCleanupSection(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,SFIniEntry *sectionp);
	bool AutoIrcTopic(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &ircsection);
	void UpdateLogFile(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &commandstr);
	void SetModes();
	void set_directchan(bool val) {directchan=val;};
	void set_publicspam(bool val) {publicspam=val;};
public:
	SFIniEntry *LookupSection(const String &chan, const String &nick, const String &section,bool errornotfound);
	SFIniEntry *LookupSectionNum(const String &chan, const String &nick, int secnum,bool errornotfound);
	SFIniEntry *LookupItem(const String &section,int index);
public:
	bool Show(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &sectionmask,int countmask,int daysmask,bool showchildren,bool onlyshownew, bool silentempty);
	bool ListSections(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password);
public:
	long LookupCutoffDate(const String &userid);
	bool UpdateCutoffDate (const String &userid);
	bool UpdateItemCutoffDate(SFIniEntry *entryp,const String &userid);
	long GetDateMinusDays(int daycount);
	String GetCurDate();
	String UserId(const String &chan,const String &nick,const String &ident);
public:
	String GetPermissionStr(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,SFIniEntry *entryp);
	bool HasPermission(const String &chan,const String &nick,const String &permstring,char *pc,bool errorifnot=true);
	void UpdatePerms(const String &pstring,bool &perm_read,bool &perm_add,bool &perm_del,bool &perm_set);
public:
	static String MyIntToStr(int val);
	static String MyLongToStr(long val);
public:
	bool SendNickNoticeOutput(const String &chan,const String &nick,const String &outstr);
	bool SendChanOutput(const String &chan,const String &nick,const String &outstr);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
