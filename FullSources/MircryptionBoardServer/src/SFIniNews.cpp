//---------------------------------------------------------------------------
// SFIniNews.cpp
// Derived class for managing news
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// MCBOARD VARIABLES
// ------------------
// ROOT(BOARD) Vars:
// 	AutoLogFilename		- name of file to write log to (no log if blank)
//	Paranoia		- if 'true' then no chanel names/plaintext nicks stored for autotopics, and logfile writing is forced off
//  	DetailedNewTracking	- if 'true' (default) then each post keeps track of who has read it, for more accurate new news checking
//	AutoIrcTopic		- name of the section to automtically store irc section changes inside (if blank, not done)
//	AutoTrimDays		- trim all items in all sections older than this many days
//	AutoTrimCount		- trim all sections so they have at most this number of most recent sections
//
// SECTION Vars:
//	...			- Permission strings include chars from "rads", which indicate permission to r)ead, a)add, d)el, s)et_admin_vars
//	...			- permissions are checked hierarchically from current section, and checked for following categories
//	Perm_Default		- default permission string
//	Perm_Class:XXX		- additional permission string for user depending on whether they are OP,VOICE
//	Perm_Chan:XXX		- additional permission string based on channel message received in
//	Perm_Pass:XXX		- additional permission string based on specified admin pass
//	AutoTrimDays		- trim all items older than this many days
//	AutoTrimCount		- trim all but this number of most recent sections
//	NoDefaultShow		- if 'true' then the section items will not be shown by default on a command to show all items
//  AllowPoints			- if 'true' then addpoints will work in this section
//	Announce		- if 'false' then adding/replacing items is not announced in a channel (good for test sections)
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "SFIniNews.h"

// for hash in getuserid
#include "md5class.h"
//---------------------------------------------------------------------------

#include <time.h>


//---------------------------------------------------------------------------
SFIniNewsManager::SFIniNewsManager()
{
	// constructor
	logfile=NULL;
	paranoidmode=false;
	detailednewtracking=true;
	outputmode=OUTMODE_MIRC;
	directchan=false;
	publicspam=false;
	MakeDefaultNews();
}

SFIniNewsManager::~SFIniNewsManager()
{
	// destructor
	if (logfile!=NULL)
		fclose(logfile);
}

void SFIniNewsManager::MakeDefaultNews()
{
	SFIniEntry *entryp;

	// first make root
	ClearAndInitialize("ROOT","");

	// default permissions for the root of the board
	// people with password can add and delete anything, others by default can only read
	rootp->InsertChildAtEnd(new SFIniEntry("Perm_Pass:TEST","r"));
	rootp->InsertChildAtEnd(new SFIniEntry("Perm_Class:op","r"));	
	rootp->InsertChildAtEnd(new SFIniEntry("Perm_Default",""));
	
	// add a default section called News
	entryp=rootp->InsertChildAtEnd(MakeNewsSection("NEWS",""));
	// and let ops add or delete stuff; let anyone read it
	entryp->InsertChildAtEnd(new SFIniEntry("Perm_Class:op","rad"));	
	entryp->InsertChildAtEnd(new SFIniEntry("Perm_Default","r"));
	// add a default news entry
	entryp->InsertChildAtEnd(MakeNewsItem("testchan","admin","","","","","welcome to the news board"));
	
	// add an OPS only section
	//	entryp=rootp->InsertChildAtEnd(MakeNewsSection("OPS","Operators Only"));
	//	// and let ops add or delete stuff; let default users not even see it
	//	entryp->InsertChildAtEnd(new SFIniEntry("Perm_Class:op","rad"));
	//	entryp->InsertChildAtEnd(new SFIniEntry("Perm_Default",""));
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
SFIniEntry * SFIniNewsManager::MakeNewsItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemtext)
{
	// make a news item with children
	SFIniEntry *entryp;
	String datestring="";

	// set current date

	entryp=new SFIniEntry("ITEM",itemtext);
	if (enick!="")
		entryp->InsertChildAtEnd(new SFIniEntry("NICK",enick));
	else
		{
		// ATTN: 5/12/04 - no point in hiding nick/chan info in encrypted items
		if (paranoidmode && false)
			{
			entryp->InsertChildAtEnd(new SFIniEntry("NICK","anonymous"));
			entryp->InsertChildAtEnd(new SFIniEntry("CHAN","anonymous"));
			}
		else
			{
			entryp->InsertChildAtEnd(new SFIniEntry("NICK",nick));
			entryp->InsertChildAtEnd(new SFIniEntry("CHAN",chan));
			}
		}
	entryp->InsertChildAtEnd(new SFIniEntry("ADDDATE",GetCurDate()));
	return entryp;
}

SFIniEntry *SFIniNewsManager::MakeNewsSection(const String &section,const String &label)
{
	SFIniEntry *entryp;

	entryp=new SFIniEntry("SECTION",section);
	entryp->InsertChildAtEnd(new SFIniEntry("Label",label));
	return entryp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool SFIniNewsManager::Parse(int arg_outputmode,const String &enick,const String &password,const String &chan,const String &nick,const String &ident,const String &userclass,const String &newcomstring)
{
	// parse a full command line
	char *comword,*arg1,*arg2,*arg3;
	bool bretv;
	int index=0;

	char comstringdata[2000];
	char *comstring=comstringdata;
	if (newcomstring.length()>=2000)
		{
		strncpy(comstring,newcomstring.c_str(),1998);
		comstring[1998]='\0';
		}
	else
		strcpy(comstring,newcomstring.c_str());

	SetModes();
	outputmode=arg_outputmode;
	
	UpdateLogFile(chan,nick,enick,ident,userclass,password,comstring);

	bretv=SplitLineToChars(comstring," ",comword,comstring);
	if (strncmp(comword,"read",4)==0)
		strncpy(comword,"show",4);

		
	if (strcmp(comword,"about")==0)
		{
		// just show them the version of this program
		SendNickNoticeOutput(chan,nick,String("mcboard server version ")+String(PROGRAMVERSION));
		bretv=true;
		}
	else if (strcmp(comword,"help")==0)
		{
		// just show them the version of this program
		SendNickNoticeOutput(chan,nick,String("RTFM"));
		bretv=true;
		}
	else if ((strcmp(comword,"show")==0 || strcmp(comword,"pubshow")==0)&& strcmp(comstring,"new")!=0)
		{
		if (strcmp(comword,"pubshow")==0)
			{
			// spam reply to channel
			publicspam=true;
			}
		if (strcmp(comstring,"")==0 || strcmp(comstring,"all")==0)
			{
			// show all items
			bretv=Show(chan,nick,ident,userclass,password,"*",-1,-1,true,false,false);
			// update date of user
			UpdateCutoffDate(UserId(chan,nick,ident));
			}
		else
			{
			// show items matching a section mask string
			bretv=Show(chan,nick,ident,userclass,password,comstring,-1,-1,true,false,false);
			}
		}
	else if (strcmp(comword,"silentnew")==0)
		{
		bretv=Show(chan,nick,ident,userclass,password,"*",-1,-1,true,true,true);
		// update date of user
		UpdateCutoffDate(UserId(chan,nick,ident));
		}
	else if (strcmp(comword,"")==0 || strcmp(comword,"new")==0 || strcmp(comword,"news")==0 || (strcmp(comword,"show")==0 && strcmp(comstring,"news")==0))
		{
		bretv=Show(chan,nick,ident,userclass,password,"*",-1,-1,true,true,false);
		// update date of user
		UpdateCutoffDate(UserId(chan,nick,ident));
		}
	else if (strcmp(comword,"showdays")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg2,"")!=0)
			{
			int daysmask=atoi(arg2);
			bretv=Show(chan,nick,ident,userclass,password,arg1,-1,daysmask,true,false,false);
			}
		else
			{
			int daysmask=atoi(arg1);
			bretv=Show(chan,nick,ident,userclass,password,"*",-1,daysmask,true,false,false);
			// update date of user
			UpdateCutoffDate(UserId(chan,nick,ident));
			}
		}
	else if (strcmp(comword,"showcount")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg2,"")!=0)
			{
			int countmask=atoi(arg2);
			bretv=Show(chan,nick,ident,userclass,password,arg1,countmask,-1,true,false,false);
			}
		else
			{
			int countmask=atoi(arg1);
			bretv=Show(chan,nick,ident,userclass,password,"*",countmask,-1,true,false,false);
			// update date of user
			UpdateCutoffDate(UserId(chan,nick,ident));
			}
		}
	else if (strcmp(comword,"sections")==0)
		{
			bretv=ListSections(chan,nick,ident,userclass,password);
		}
	else if (strcmp(comword,"list")==0)
		{
		if (strcmp(comstring,"")==0)
			bretv=Show(chan,nick,ident,userclass,password,"*",-1,-1,false,false,false);
		else
			bretv=Show(chan,nick,ident,userclass,password,comstring,-1,-1,false,false,false);
		}
	else if (strcmp(comword,"addsection")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=AddSection(chan,nick,enick,ident,userclass,password,String(arg1),String(comstring));
		else
			SendNickNoticeOutput(chan,nick,"you need to specify the name and description of the section to add");
		}
	else if (strcmp(comword,"delsection")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=DelSection(chan,nick,enick,ident,userclass,password,String(arg1));
		else
			SendNickNoticeOutput(chan,nick,"you need to specify the name of the section to delete");
		}
	else if (strcmp(comword,"delsectionnum")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			{
			int secnum=atoi(arg1);
			if (secnum!=0)
				bretv=DelSectionNum(chan,nick,enick,ident,userclass,password,secnum);
			else
				SendNickNoticeOutput(chan,nick,"error specifying section number to delete (index starts at 1)");
			}
		else
			SendNickNoticeOutput(chan,nick,"you need to specify the number of the section to delete");
		}
	else if (strcmp(comword,"movsection")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0 )
			bretv=MovSection(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2));
		else
			SendNickNoticeOutput(chan,nick,"error specifying movsection parameters");
		}
	else if (strcmp(comword,"repsection")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			bretv=RepSection(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2),String(comstring));
		else
			SendNickNoticeOutput(chan,nick,"error specifying repsection parameters");
		}
	else if (strcmp(comword,"clearsection")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=ClearSection(chan,nick,enick,ident,userclass,password,String(arg1));
		else
			SendNickNoticeOutput(chan,nick,"you need to specify section name to clear");
		}
	else if (strcmp(comword,"trimsectiondays")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			{
			index=atoi(arg2);
			if (index!=0)
				bretv=ClearSectionByDays(chan,nick,ident,userclass,password,String(arg1),index,true);
			else
				SendNickNoticeOutput(chan,nick,"days to trim was not parsed as a good integer");
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"trimsectioncount")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			{
			index=atoi(arg2);
			if (index!=0)
				bretv=ClearSectionByCount(chan,nick,ident,userclass,password,String(arg1),index,true);
			else
				SendNickNoticeOutput(chan,nick,"count to trim was not parsed as a good integer");
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"trimdays")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			{
			index=atoi(arg1);
			if (index!=0)
				bretv=ClearByDays(chan,nick,ident,userclass,password,index,true);
			else
				SendNickNoticeOutput(chan,nick,"days to trim was not parsed as a good integer");
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"trimcount")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			{
			index=atoi(arg1);
			if (index!=0)
				bretv=ClearByCount(chan,nick,ident,userclass,password,index,true);
			else
				SendNickNoticeOutput(chan,nick,"trimcount was not parsed as a good integer");
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"add")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(comstring,"")!=0)
			{
			SFIniEntry *iretvp;
			iretvp=AddItem(chan,nick,enick,ident,userclass,password,String(arg1),String(comstring),true,true);
			bretv=(iretvp!=NULL);
			}
		}
	else if (strcmp(comword,"del")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			{
			index=atoi(arg2);
			bretv=DelItem(chan,nick,enick,ident,userclass,password,String(arg1),index);
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"mov")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		bretv=SplitLineToChars(comstring," ",arg3,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0 && strcmp(arg3,"")!=0)
			{
			index=atoi(arg2);
			bretv=MovItem(chan,nick,enick,ident,userclass,password,String(arg1),index,String(arg3));
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"rep")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0 && strcmp(comstring,"")!=0)
			{
			index=atoi(arg2);
			bretv=RepItem(chan,nick,enick,ident,userclass,password,String(arg1),index,String(comstring));
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"setsectionvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		bretv=SplitLineToChars(comstring," ",arg3,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0 && strcmp(arg3,"")!=0)
			bretv=SetSectionVar(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2),String(arg3));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"getsectionvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			bretv=GetSectionVar(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"addsectionvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			bretv=AddSectionVar(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"delsectionvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			bretv=DelSectionVar(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"setboardvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			bretv=SetBoardVar(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"getboardvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=GetBoardVar(chan,nick,enick,ident,userclass,password,String(arg1));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"addboardvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=AddBoardVar(chan,nick,enick,ident,userclass,password,String(arg1));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"delboardvar")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		if (strcmp(arg1,"")!=0)
			bretv=DelBoardVar(chan,nick,enick,ident,userclass,password,String(arg1));
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"listvars")==0)
		{
		bretv=ListVars(chan,nick,enick,ident,userclass,password);
		}
	else if (strcmp(comword,"autoirctopic")==0)
		{
		if (strcmp(comstring,"")!=0)
			bretv=AutoIrcTopic(chan,nick,enick,ident,userclass,password,comstring);
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"addpoints")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		bretv=SplitLineToChars(comstring," ",arg3,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0 && strcmp(arg3,"")!=0)
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			{
			index=atoi(arg3);
			if (index==0)
				{
				SendNickNoticeOutput(chan,nick,"error: bad addpoints command.");
				bretv=false;
				}
			else
				bretv=AddPoints(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2),String(comstring),index);
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else if (strcmp(comword,"activity")==0)
		{
		bretv=SplitLineToChars(comstring," ",arg1,comstring);
		bretv=SplitLineToChars(comstring," ",arg2,comstring);
		if (strcmp(arg1,"")!=0 && strcmp(arg2,"")!=0)
			{
			bretv=AddActivity(chan,nick,enick,ident,userclass,password,String(arg1),String(arg2),String(comstring));
			}
		else
			SendNickNoticeOutput(chan,nick,"error specifying parameters");
		}
	else
		{
		SendNickNoticeOutput(chan,nick,String("mcboard bot command \"")+comword+","+comstring+"\" not understood.");
		bretv=false;
		}
	return bretv;
}

//---------------------------------------------------------------------------
	




//---------------------------------------------------------------------------
bool SFIniNewsManager::AddSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &label)
{
	// add a new section
	SFIniEntry *entryp;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"a"))
		return false;

	entryp=rootp->InsertChildAtEnd(new SFIniEntry("SECTION",section));
	entryp->InsertChildAtEnd(new SFIniEntry("Label",label));
	SendNickNoticeOutput(chan,nick,"Added section "+section+".");
	return true;
}

bool SFIniNewsManager::DelSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section)
{
	// delete a section
	SFIniEntry *entryp;

	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"d"))
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;

	delete entryp;
	SendNickNoticeOutput(chan,nick,"Deleted section "+section+".");
	return true;
}

bool SFIniNewsManager::DelSectionNum(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,int secnum)
{
	// delete a section
	SFIniEntry *entryp;
	String section;

	if (secnum<=0)
		return false;

	// find the section with index secnum, where first section is #1
	entryp=LookupSectionNum(chan,nick,secnum,true);
	if (entryp==NULL)
		return false;

	section=entryp->get_value();

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"d"))
		return false;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;

	SendNickNoticeOutput(chan,nick,"Deleted section #"+MyIntToStr(secnum)+" ("+section+").");
	delete entryp;
	return true;
}

bool SFIniNewsManager::MovSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &toabovesection)
{
	// move a section to below another section
	SFIniEntry *entryp,*sectionentryp;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"ad"))
		return false;
	
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;
	entryp->SpliceOut();
	sectionentryp=LookupSection(chan,nick,toabovesection,true);
	if (sectionentryp==NULL)
		return false;
	sectionentryp->InsertBefore(entryp);
	SendNickNoticeOutput(chan,nick,"Moved section "+section+".");
	return true;
}

bool SFIniNewsManager::RepSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &newsection,const String &newlabel)
{
	// replace a section name and label
	SFIniEntry *entryp;
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"ad"))
		return false;

	entryp->set_value(newsection);
	entryp=entryp->LookupChildren("Label","*");
	if (entryp==NULL)
		return false;
	entryp->set_value(newlabel);
	SendNickNoticeOutput(chan,nick,"Replaced section "+section+".");
	return true;
}

bool SFIniNewsManager::ClearSection(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section)
{
	// delete all items in a section, but leave section alone
	SFIniEntry *entryp,*nextp;

	// lookup parent section
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;

	// now walk through and delete all children
	entryp=entryp->get_childp();
	while (entryp!=NULL)
		{
		nextp=entryp->get_nextp();
		if (entryp->get_prefix()=="ITEM")
			delete entryp;
		entryp=nextp;
		}

	SendNickNoticeOutput(chan,nick,"Cleared all items from section "+section+".");
	return true;
}

bool SFIniNewsManager::ClearSectionByDays(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &section, int daycount,bool announce)
{
	// delete all items in a section, but leave section alone
	SFIniEntry *entryp,*nextp;
	long cutoffdate;
	
	// calculate cutoff date
	cutoffdate=GetDateMinusDays(daycount);
	
	// lookup parent section
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;

	// now walk through and delete all children
	entryp=entryp->get_childp();
	while (entryp!=NULL)
		{
		nextp=entryp->get_nextp();
		if (entryp->get_prefix()=="ITEM")
			{
			// is it too old?
			if (!(entryp->isnewer(cutoffdate,"")))
				delete entryp;
			}
		entryp=nextp;
		}
	if (announce)
		SendNickNoticeOutput(chan,nick,"Cleared all items more than "+MyIntToStr(daycount)+" days old from section "+section+".");
	return true;
}


bool SFIniNewsManager::ClearSectionByCount(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &section, int keepcount,bool announce)
{
	// delete all items in a section, but leave section alone
	SFIniEntry *entryp,*nextp;
	int itemcount;
	
	// lookup parent section
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;
	
	// now walk through and delete all children
	itemcount=entryp->CountChildren("ITEM","*",false);
	if (itemcount>keepcount)
		{
		entryp=entryp->get_childp();
		while (entryp!=NULL)
			{
			nextp=entryp->get_nextp();
			if (entryp->get_prefix()=="ITEM")
				{
				// is it too old?
				if (itemcount>keepcount)
					{
					delete entryp;
					--itemcount;
					}
				else
					break;
				}
			entryp=nextp;
			}
		}
	if (announce)
		SendNickNoticeOutput(chan,nick,"Cleared all but most recent "+MyIntToStr(keepcount)+" items from section "+section+".");
	return true;
}

bool SFIniNewsManager::ClearByDays(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,int daycount,bool announce)
{
	// delete all old items in all sections
	SFIniEntry *sectionp,*entryp,*nextp;
	long cutoffdate;
	
	// calculate cutoff date
	cutoffdate=GetDateMinusDays(daycount);
	
	// lookup parent section
	sectionp=rootp->get_childp();
	while (sectionp!=NULL)
		{
		if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"d",false))
			{
			sectionp=sectionp->get_nextp();
			continue;
			}

		// now walk through and delete all children
		entryp=sectionp->get_childp();
		while (entryp!=NULL)
			{
			nextp=entryp->get_nextp();
			if (entryp->get_prefix()=="ITEM")
				{
				// is it too old?
				if (!(entryp->isnewer(cutoffdate,"")))
					delete entryp;
				}
			entryp=nextp;
			}
		sectionp=sectionp->get_nextp();
		}
	if (announce)
		SendNickNoticeOutput(chan,nick,"Cleared all items more than "+MyIntToStr(daycount)+" days old.");
	return true;
}


bool SFIniNewsManager::ClearByCount(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,int keepcount,bool announce)
{
	// delete all old items in all section
	SFIniEntry *sectionp,*entryp,*nextp;
	int itemcount;
	
	// lookup parent section
	// lookup parent section
	sectionp=rootp->get_childp();
	while (sectionp!=NULL)
		{
		if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"d",false))
			{
			sectionp=sectionp->get_nextp();
			continue;
			}

		// now walk through and delete all children
		itemcount=sectionp->CountChildren("ITEM","*",false);
		if (itemcount>keepcount)
			{
			entryp=sectionp->get_childp();
			while (entryp!=NULL)
				{
				nextp=entryp->get_nextp();
				if (entryp->get_prefix()=="ITEM")
					{
					// is it too old?
					if (itemcount>keepcount)
						{
						delete entryp;
						--itemcount;
						}
					else
						break;
					}
				entryp=nextp;
				}
			}
		sectionp=sectionp->get_nextp();
		}
	if (announce)
		SendNickNoticeOutput(chan,nick,"Cleared all but most recent "+MyIntToStr(keepcount)+" items from each section.");
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
SFIniEntry * SFIniNewsManager::AddItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemtext,bool announce, bool checkperms)
{
	// add a new news item
	SFIniEntry *entryp,*sectionentryp;
	int index;

	sectionentryp=LookupSection(chan,nick,section,true);
	if (sectionentryp==NULL)
		return NULL;

	if (checkperms)
		{
		if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionentryp),"a"))
			return NULL;
		}

	entryp=MakeNewsItem(chan,nick,enick,ident,userclass,password,itemtext);
	sectionentryp->InsertChildAtEnd(entryp);
	index=entryp->FindChildIndex("ITEM","*")+1;

	if (announce)
		{
		if (chan!="" && chan!="__NOCHAN__" && HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,"","",entryp),"r",false) && sectionentryp->get_ChildValue("Announce")!="false")
			{
			// notify channel of what we've done?
			SendChanOutput(chan,nick,nick+" added \""+itemtext+"\" to "+section+" ("+MyIntToStr(index)+").");
			}
		else
			SendNickNoticeOutput(chan,nick,"Added \""+itemtext+"\" to "+section+" ("+MyIntToStr(index)+").");
		}
	
	// mark this item as specifically read by this user
	String identuserid=UserId(chan,nick,ident);
	UpdateItemCutoffDate(entryp,identuserid);
	return entryp;
}

bool SFIniNewsManager::DelItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index)
{
	// delete an item
	SFIniEntry *entryp;
	entryp=LookupItem(section,index);
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section or item not found.");
		return false;
		}

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"d"))
		return false;

	delete entryp;
	SendNickNoticeOutput(chan,nick,"Deleted item "+MyIntToStr(index)+" from "+section+", and renumbered subsequent items.");
	return true;
}

bool SFIniNewsManager::MovItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index,const String &newsection)
{
	// move an item under a different section
	SFIniEntry *entryp,*sectionentryp;
	entryp=LookupItem(section,index);
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section or item not found.");
		return false;
		}
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"ad"))
		return false;

	entryp->SpliceOut();
	sectionentryp=LookupSection(chan,nick,newsection,true);
	if (sectionentryp==NULL)
		return false;
	sectionentryp->InsertChildAtEnd(entryp);
	SendNickNoticeOutput(chan,nick,"Moved item from "+section+" to "+newsection+".");
	return true;
}

bool SFIniNewsManager::RepItem(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,int index,const String &newitemtext)
{
	// replace the text of an item
	SFIniEntry *entryp,*sectionentryp;

	sectionentryp=LookupSection(chan,nick,section,true);
	entryp=LookupItem(section,index);
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section or item not found.");
		return false;
		}
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"ad"))
		return false;
	entryp->set_value(newitemtext);
	// update the date of it
	entryp->set_ChildValue("ADDDATE",GetCurDate());

	// tell them about what we did

	if (chan!="" && chan!="__NOCHAN__" && HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,"","",entryp),"r",false) && sectionentryp->get_ChildValue("Announce")!="false")
		{
		// notify channel of what we've done?
		// only if the section is set to be default readable by all
		SendChanOutput(chan,nick,nick+" replaced "+section+"("+MyIntToStr(index)+") with \""+newitemtext+"\".");
		}
	else
		SendNickNoticeOutput(chan,nick,"Replaced "+section+"("+MyIntToStr(index)+") with \""+newitemtext+"\".");

	// mark this item as specifically read by this user
	String identuserid=UserId(chan,nick,ident);
	UpdateItemCutoffDate(entryp,identuserid);
	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool SFIniNewsManager::SetSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname,const String &varval)
{
	SFIniEntry *entryp;
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"s"))
		return false;

	entryp=entryp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	entryp->set_value(varval);
	SendNickNoticeOutput(chan,nick,"Set "+section+"."+varname+" to "+varval+".");
	return true;
}

bool SFIniNewsManager::GetSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname)
{
	SFIniEntry *entryp;
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"s"))
		return false;

	entryp=entryp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	String varval=entryp->get_value();
	SendNickNoticeOutput(chan,nick,"Value of "+section+"."+varname+" is "+varval+".");	
	return true;
}

bool SFIniNewsManager::AddSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname)
{
	SFIniEntry *sectionp,*entryp,*entryp2;
	sectionp=LookupSection(chan,nick,section,true);
	if (sectionp==NULL)
		return false;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"s"))
		return false;

	entryp=sectionp->LookupChildren(varname,"*");
	if (entryp!=NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section variable "+varname+" already exists.");
		return false;
		}

	// create it with blank value
	entryp=new SFIniEntry(varname,"");
	entryp2=sectionp->InsertChildAtBegining(entryp);
	if (entryp2==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Could not add section variable "+varname+".");
		return false;
		}
	SendNickNoticeOutput(chan,nick,"Added section variable "+varname+".");
	return true;
}

bool SFIniNewsManager::DelSectionVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section, const String &varname)
{
	SFIniEntry *entryp;
	entryp=LookupSection(chan,nick,section,true);
	if (entryp==NULL)
		return false;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,entryp),"s"))
		return false;

	entryp=entryp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Section variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	
	// delete it
	delete entryp;
	SendNickNoticeOutput(chan,nick,"Deleted variable "+varname+" from section "+section+".");
	return true;
}


bool SFIniNewsManager::SetBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname,const String &varval)
{
	SFIniEntry *entryp;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"s"))
		return false;
	entryp=rootp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Board variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	entryp->set_value(varval);
	SendNickNoticeOutput(chan,nick,"Set board variable "+varname+" to "+varval+".");
	return true;
}

bool SFIniNewsManager::GetBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname)
{
	SFIniEntry *entryp;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"s"))
		return false;
	entryp=rootp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Board variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	String varval=entryp->get_value();
	SendNickNoticeOutput(chan,nick,"Value of board variable "+varname+" is "+varval+".");	
	return true;
}

bool SFIniNewsManager::AddBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname)
{
	SFIniEntry *entryp,*entryp2;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"s"))
		return false;
	entryp=rootp->LookupChildren(varname,"*");
	if (entryp!=NULL)
		{
		SendNickNoticeOutput(chan,nick,"Board variable "+varname+" already exists.");
		return false;
		}

	// create it with blank value
	entryp=new SFIniEntry(varname,"");
	entryp2=rootp->InsertChildAtBegining(entryp);
	if (entryp2==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Could not add board variable "+varname+".");
		return false;
		}
	SendNickNoticeOutput(chan,nick,"Added board variable "+varname+".");
	return true;
	return true;
}

bool SFIniNewsManager::DelBoardVar(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &varname)
{
	SFIniEntry *entryp;
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"s"))
		return false;
	entryp=rootp->LookupChildren(varname,"*");
	if (entryp==NULL)
		{
		SendNickNoticeOutput(chan,nick,"Board variable "+varname+" is not known (note that variable names are case sensitive).");
		return false;
		}
	// ATTN: del it
	delete entryp;
	SendNickNoticeOutput(chan,nick,"Deleted variable "+varname+" from board.");
	return true;
}

bool SFIniNewsManager::ListVars(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password)
{
	// walk through board and display variabls, (ONLY IF the person has permissions to see them!)
	// ATTN: not implemented yet
	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"s"))
		return false;
	return true;
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool SFIniNewsManager::AddPoints(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemname, String reasonstr, int points)
{
	// add some points to an item
	SFIniEntry *entryp,*sectionp,*nextp;
	String iname,idesc;
	int iscore;
	bool bretv;

	// first find the score section
	sectionp=LookupSection(chan,nick,section,false);
	if (sectionp==NULL)
		{
		// no scores section exists... we used to just exit here but now we are nice and create the section for them if we can
		if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"a"))
			return false;
		SendNickNoticeOutput(chan,nick,"Creating section "+section+" to hold scores...");
		entryp=rootp->InsertChildAtEnd(new SFIniEntry("SECTION",section));
		entryp->InsertChildAtEnd(new SFIniEntry("Label","PointTotals"));
		entryp->InsertChildAtEnd(new SFIniEntry("AllowPoints","true"));
		sectionp=entryp;
		// now we drop down to continue process
		}

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"a"))
		return false;

	// to keep out trouble makers, we only allow section names with "SCORE" in it
	if (sectionp->get_ChildValue("AllowPoints")!="true")
		{
		SendNickNoticeOutput(chan,nick,"Scores cannot be added to section "+section+".");
		return false;
		}

	// now lets see if there is an existing entry for this item
	entryp=sectionp->get_childp();
	while (entryp!=NULL)
		{
		nextp=entryp->get_nextp();
		if (entryp->get_prefix()=="ITEM")
			{
			// parse the score item to separate item name from score and description
			ParseScoreEntry(entryp->get_value(),iname,idesc,iscore);
			if (iname==itemname)
				{
				// found a match
				break;
				}
			}
		entryp=nextp;
		}

	if (reasonstr!="")
		reasonstr=MyIntToStr(points)+" points for "+reasonstr;
	else
		reasonstr=MyIntToStr(points)+" points";


	if (entryp!=NULL)
		{
		// we update our score and delete it (will be replaced below)
		iname=itemname;
		iscore+=points;
		if (idesc!="")
			{
			// strip off parentheses
			idesc=idesc.substr(1,idesc.length()-2);
			String prefix,suffix;
			while (reasonstr.length()+idesc.length()>MAX_POINTSDESCLEN)
				{
				// trim off last reason since it is too long
				bretv=SplitStringToStrings(idesc,"; ",prefix,suffix);
				if (!bretv)
					break;
				if (suffix=="")
					{
					idesc="";
					break;
					}
				idesc=prefix;
				}
			if (idesc!="")
				idesc=reasonstr+"; "+idesc;
			else
				idesc=reasonstr;
		}
		else
			idesc=reasonstr;
		delete entryp;
		entryp=NULL;
		}
	else
		{
		iname=itemname;
		iscore=points;
		idesc=reasonstr;
		}

	// create the entry
	entryp=MakeScoreEntry(chan,nick,enick,ident,userclass,password,iname,idesc,iscore);
	// add it to section
	sectionp->InsertChildAtEnd(entryp);
	// announce it

	if (chan!="" && chan!="__NOCHAN__")
		{
		// notify channel of what we've done?
		// only if the section is set to be default readable by all
		if (HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,"","",entryp),"r",false))
			SendChanOutput(chan,nick,nick+" gives "+iname+" "+reasonstr+". Total = "+MyIntToStr(iscore)+".");
		}
	else
		SendNickNoticeOutput(chan,nick,"Gave "+iname+" "+reasonstr+". Total = "+MyIntToStr(iscore)+".");

	// mark this item as specifically read by this user
	String identuserid=UserId(chan,nick,ident);
	UpdateItemCutoffDate(entryp,identuserid);
	return true;
}

void SFIniNewsManager::ParseScoreEntry(String str,String &iname,String &idesc,int &iscore)
{
	// break of str into components with score (name,score,description)
	String scorestring;
	bool bretv;

	// separator indicator
	unsigned char sepstr[4];
	String sepstring;
	sepstr[0]=160;
	sepstr[1]='\0';
	sepstring=String((char *)sepstr);

	bretv=SplitStringToStrings(str,sepstring,iname,str);
	bretv=SplitStringToStrings(str,sepstring,scorestring,str);
	iscore=0;
	if (scorestring.length()>0)
		iscore=atoi(scorestring.c_str());
	SplitStringToStrings(str,sepstring,idesc,str);
}

SFIniEntry *SFIniNewsManager::MakeScoreEntry(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemname,const String &reasonstr,const int points)
{
	// makea new entry with a properly formatted string
	SFIniEntry *entryp;
	String itemtext;

	// separator indicator
	unsigned char sepstr[4];
	String sepstring;
	sepstr[0]=160;
	sepstr[1]='\0';
	sepstring=String((char *)sepstr);

	// form itemtext
	itemtext=itemname+sepstring+MyIntToStr(points);
	if (reasonstr!="")
		itemtext+=sepstring+"("+reasonstr+")";

	entryp=MakeNewsItem(chan,nick,enick,ident,userclass,password,itemtext);
	return entryp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool SFIniNewsManager::AddActivity(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &section,const String &itemname, String reasonstr)
{
	// add activity to users entry
	SFIniEntry *entryp,*sectionp,*nextp;
	String iname,idesc;

	// first find the score section
	sectionp=LookupSection(chan,nick,section,false);
	if (sectionp==NULL)
		{
		// no scores section exists... we used to just exit here but now we are nice and create the section for them if we can
		if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,rootp),"a"))
			return false;
		SendNickNoticeOutput(chan,nick,"Creating section "+section+" to hold current user activities...");
		entryp=rootp->InsertChildAtEnd(new SFIniEntry("SECTION",section));
		entryp->InsertChildAtEnd(new SFIniEntry("Label","What Users are Currently Doing"));
		entryp->InsertChildAtEnd(new SFIniEntry("AllowActivities","true"));
		sectionp=entryp;
		// now we drop down to continue process
		}

	if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"a"))
		return false;

	// to keep out trouble makers, we only allow section names with "SCORE" in it
	if (sectionp->get_ChildValue("AllowActivities")!="true")
		{
		SendNickNoticeOutput(chan,nick,"Activities cannot be added to section "+section+".");
		return false;
		}

	// now lets see if there is an existing entry for this item
	entryp=sectionp->get_childp();
	while (entryp!=NULL)
		{
		nextp=entryp->get_nextp();
		if (entryp->get_prefix()=="ITEM")
			{
			// parse the score item to separate item name from description
			ParseActivityEntry(entryp->get_value(),iname,idesc);
			if (iname==itemname)
				{
				// found a match
				break;
				}
			}
		entryp=nextp;
		}

	// semicolon means blank
	if (reasonstr==";")
		reasonstr="";

	if (entryp!=NULL)
		{
		// we update our activity and delete it (will be replaced below)
		iname=itemname;
		idesc=reasonstr;
		delete entryp;
		entryp=NULL;
		}
	else
		{
		iname=itemname;
		idesc=reasonstr;
		}

	if (idesc!="")
		{
		// create the entry
		entryp=MakeActivityEntry(chan,nick,enick,ident,userclass,password,iname,idesc);
		// add it to section
		sectionp->InsertChildAtEnd(entryp);
		// mark this item as specifically read by this user
		String identuserid=UserId(chan,nick,ident);
		UpdateItemCutoffDate(entryp,identuserid);
		}

	// announce it
	if (chan!="" && chan!="__NOCHAN__" && reasonstr!="")
		{
		// notify channel of what we've done?
		// only if the section is set to be default readable by all
		if (HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,"","",entryp),"r",false))
			SendChanOutput(chan,nick,iname+" is now "+reasonstr+".");
		}
	else
		{
		if (reasonstr=="")
			reasonstr="not currently engaged in any activity";
		SendNickNoticeOutput(chan,nick,"board updated to show that "+iname+" is "+reasonstr+".");
		}

	return true;
}

void SFIniNewsManager::ParseActivityEntry(String str,String &iname,String &idesc)
{
	// break of str into components with activity (name,description)
	String scorestring;

	// separator indicator
	unsigned char sepstr[4];
	String sepstring;
	sepstr[0]=160;
	sepstr[1]='\0';
	sepstring=String((char *)sepstr);

	SplitStringToStrings(str,sepstring,iname,str);
	SplitStringToStrings(str,sepstring,idesc,str);
}

SFIniEntry *SFIniNewsManager::MakeActivityEntry(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &itemname,const String &reasonstr)
{
	// makea new entry with a properly formatted string
	SFIniEntry *entryp;
	String itemtext;

	// separator indicator
	unsigned char sepstr[4];
	String sepstring;
	sepstr[0]=160;
	sepstr[1]='\0';
	sepstring=String((char *)sepstr);

	// form itemtext
	itemtext=itemname+sepstring+"is"+sepstring+reasonstr+".";

	entryp=MakeNewsItem(chan,nick,enick,ident,userclass,password,itemtext);
	return entryp;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
SFIniEntry *SFIniNewsManager::LookupSection(const String &chan, const String &nick, const String &section,bool errornotfound)
{
	// look up a specified section
	SFIniEntry *entryp;
	entryp=rootp->LookupChildren("SECTION",section);
	if (entryp==NULL && errornotfound)
		SendNickNoticeOutput(chan,nick,"Error: section \""+section+"\" was not found.");
	return entryp;
}

SFIniEntry *SFIniNewsManager::LookupSectionNum(const String &chan, const String &nick, int secnum,bool errornotfound)
{
	// look up a specified section
	SFIniEntry *entryp;
	entryp=rootp->LookupChildren("SECTION","*",secnum);
	if (entryp==NULL && errornotfound)
		SendNickNoticeOutput(chan,nick,"Error: section #"+MyIntToStr(secnum)+" was not found.");
	return entryp;
}

SFIniEntry *SFIniNewsManager::LookupItem(const String &section,int index)
{
	// look up entry #index under the specified section
	// first lookup section
	SFIniEntry *entryp;
	entryp=rootp->LookupChildren("SECTION",section);
	if (entryp==NULL)
			return entryp;
	// now lookup item inside section
	entryp=entryp->LookupChildren("ITEM","*",index);
	return entryp;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool SFIniNewsManager::Show(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,const String &sectionmask,int countmask,int daysmask,bool showchildren,bool onlyshownew, bool silentempty)
{
	// show the specified sections
	SFIniEntry *sectionp,*entryp;
	int totalitems,newitems,itemcount;
	int matchingsections=0;
	long lastcheckdate;
	String sectionname,itemtext;
	String authorstring;
	String modstring;
	String label;
	int allitemcount=0;
	unsigned char spacestring[2];
	bool showthissection,showthisitem;
	String retstr;
	bool showborder=false;
	int sectionitemcount;
	bool thisshowchildren;
	long maskdate;
	String identuserid;

	// space string	
	spacestring[0]=160;
	spacestring[1]='\0';
	
	// get last date user has checked
	identuserid=UserId(chan,nick,ident);
	lastcheckdate=LookupCutoffDate(identuserid);
	if (daysmask>-1)
		maskdate=GetDateMinusDays(daysmask);
	
	// walk through all sections that match mask
	AutoCleanupBoard(chan,nick,ident,userclass,password);
	sectionp=rootp->get_childp();
	while (sectionp!=NULL)
		{
		showthissection=true;
		thisshowchildren=showchildren;
		if (!(sectionp->MaskMatch("SECTION",sectionmask)))
			showthissection=false;
		else if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"r",false))
			showthissection=false;
		else
			{
			if (sectionmask=="*" && showchildren==true)
				{
				// certain sections can be marked as not to be displayed in the LIST ALL ITEMS command
				sectionp->GetUpwardValue("NoDefaultShow",retstr);
				if (retstr=="true")
					{
					// dont show this section by default, but warn them
					thisshowchildren=false;
					if (onlyshownew)
							showthissection=false;
					if (onlyshownew == false && false)
						{
						sectionname=sectionp->get_value();
						label=sectionp->get_ChildValue("Label");
						sectionname=String("[")+sectionname+String("]");
						if (label!="")
							sectionname+=" - "+label;
						if (!showborder)
							{
							showborder=true;
							SendNickNoticeOutput(chan,nick,"-----------------------------------");
							}
						SendNickNoticeOutput(chan,nick,sectionname+" - not shown");
						}
					}
				}
			}
		if (showthissection)
			AutoCleanupSection(chan,nick,ident,userclass,password,sectionp);
		if (showthissection)
			{
			++matchingsections;
			// got a matching section
			// count items
			totalitems=0;
			newitems=0;
			entryp=sectionp->get_childp();
			while (entryp!=NULL)
				{
				if (entryp->MaskMatch("ITEM","*"))
					{
					++totalitems;
					if (entryp->isnewer(lastcheckdate,identuserid))
						{
						// new item
						++newitems;
						}
					}
				entryp=entryp->get_nextp();
				}
			// now show the header for this section
			sectionname=sectionp->get_value();	
			label=sectionp->get_ChildValue("Label");
			sectionname=String("[")+sectionname+String("]");
			if (label!="")
				sectionname+=" - "+label;
			if (totalitems==0 && onlyshownew==false)
				{
				if (!showborder)
					{
					showborder=true;
					SendNickNoticeOutput(chan,nick,"-----------------------------------");
					}
				SendNickNoticeOutput(chan,nick,sectionname+" - empty");
				}
			itemcount=0;
			if (totalitems>0 && (newitems>0 || onlyshownew==false))
				{
				if (!showborder)
					{
					showborder=true;
					SendNickNoticeOutput(chan,nick,"-----------------------------------");
					}
				SendNickNoticeOutput(chan,nick,sectionname+String(" - ")+MyIntToStr(totalitems)+" total, "+MyIntToStr(newitems)+" new");
				if (thisshowchildren)
					{
					// Show children
					entryp=sectionp->get_childp();
					sectionitemcount=0;
					if (onlyshownew)
						totalitems=newitems;
					while (entryp!=NULL)
						{
						if (entryp->MaskMatch("ITEM","*"))
							{
							++sectionitemcount;
							// decide whether to show it
							showthisitem=true;
							if (onlyshownew && !(entryp->isnewer(lastcheckdate,identuserid)))
								showthisitem=false;
							if (countmask>0 && (totalitems-sectionitemcount)>=countmask)
								showthisitem=false;
							if (daysmask>0 && !(entryp->isnewer(maskdate,"")))
								showthisitem=false;
							if (showthisitem)
								{
								// show it
								++itemcount;
								++allitemcount;
								modstring="";
								if (totalitems<=9)
									modstring+=MyIntToStr(sectionitemcount);
								else
									{
									if (itemcount<=9)
										modstring+=String("0")+MyIntToStr(sectionitemcount);
									else
										modstring+=MyIntToStr(sectionitemcount);
									}
								if (entryp->isnewer(lastcheckdate,identuserid))
									{
									// new item
									modstring+="* ";
									}
								else
									{
									//modstring+=String((char *)spacestring);
									modstring+=". ";
									}
								itemtext=entryp->get_value();
								authorstring=entryp->GetAuthorString();
								SendNickNoticeOutput(chan,nick,modstring+"\""+itemtext+"\" - "+authorstring);
								// mark this item as read by this user
								if (detailednewtracking)
									UpdateItemCutoffDate(entryp,identuserid);
								}
							}
						entryp=entryp->get_nextp();
						}
					}
				}
			}
		sectionp=sectionp->get_nextp();
		}
	if (matchingsections==0)
		{
		if (sectionmask!="*")
			SendNickNoticeOutput(chan,nick,"no matching sections on messageboard.");
		else
			SendNickNoticeOutput(chan,nick,"nothing to read.");
		}
	else if (allitemcount==0 && showchildren && onlyshownew==true && showborder==false && silentempty==false)
		SendNickNoticeOutput(chan,nick,"nothing new on message board.");

	// end border
	if (showborder)
		{
		// eggdrop queue is FUCKED, and will refuse an end border = start border
		SendNickNoticeOutput(chan,nick,"-----------------------------------\xA0");
		}

	return true;
}


bool SFIniNewsManager::ListSections(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password)
{
	// a very compact list of sections, for min bandwidth
	SFIniEntry *sectionp,*entryp;
	bool showthissection;
	String sectionname;
	int matchingsections=0;
	int totalitems,newitems;
	long lastcheckdate;
	String sectionliststring="Sections:";
	String identuserid;

	// get last date user has checked
	identuserid=UserId(chan,nick,ident);
	lastcheckdate=LookupCutoffDate(identuserid);

	// walk through all sections that match mask
	AutoCleanupBoard(chan,nick,ident,userclass,password);
	sectionp=rootp->get_childp();
	while (sectionp!=NULL)
		{
		showthissection=true;
		if (!(sectionp->MaskMatch("SECTION","*")))
			showthissection=false;
		else if (!HasPermission(chan,nick,GetPermissionStr(chan,nick,ident,userclass,password,sectionp),"r",false))
			showthissection=false;
		if (showthissection)
			AutoCleanupSection(chan,nick,ident,userclass,password,sectionp);
		if (showthissection)
			{
			++matchingsections;
			// got a matching section
			// count items
			totalitems=0;
			newitems=0;
			entryp=sectionp->get_childp();
			while (entryp!=NULL)
				{
				if (entryp->MaskMatch("ITEM","*"))
					{
					++totalitems;
					if (entryp->isnewer(lastcheckdate,identuserid))
						{
						// new item
						++newitems;
						}
					}
				entryp=entryp->get_nextp();
				}
			// now show the header for this section
			sectionname=sectionp->get_value();	
			sectionname=sectionname;
			if (totalitems==0)
				sectionname+=String("[0]");
			else
				sectionname+=String("[")+MyIntToStr(totalitems)+String(":")+MyIntToStr(newitems)+String("]");
			sectionliststring+=" "+sectionname;
			if (sectionliststring.length()>MAXIRCLINELEN)
				{
				SendNickNoticeOutput(chan,nick,sectionliststring);
				// its too long for comfort (irc server may truncate it)
				sectionliststring="Sections:";
				}
			}
		sectionp=sectionp->get_nextp();
		}

	if (matchingsections==0)
			SendNickNoticeOutput(chan,nick,"board is empty.");
	else if (sectionliststring.length()>strlen("Sections:"))
			SendNickNoticeOutput(chan,nick,sectionliststring);
	return true;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
long SFIniNewsManager::LookupCutoffDate(const String &userid)
{
	// look up the lastnewscheckdate for a user
	SFIniEntry *entryp;
	String datestring;
	long dateval;

	// first lookup options	
	entryp=rootp->LookupChildren("DATA","CHECKDATES");
	if (entryp==NULL)
		return 0;
	entryp=entryp->LookupChildren(userid,"*");
	if (entryp==NULL)
		return 0;
	datestring=entryp->get_value();
	dateval=atol(datestring.c_str());
	return dateval;
		
}

bool SFIniNewsManager::UpdateCutoffDate (const String &userid)
{
	// update the lastnewscheckdate for a user
	SFIniEntry *entryp,*optionp;
	
	// first lookup options	
	optionp=rootp->LookupChildren("DATA","CHECKDATES");
	if (optionp==NULL)
		{
		// doesnt exist yet, so make it
		optionp=rootp->InsertChildAtEnd(new SFIniEntry("DATA","CHECKDATES"));
		if (optionp==NULL)
			return false;
		}
	// look up user
	entryp=optionp->LookupChildren(userid,"*");
	if (entryp==NULL)
		{
		entryp=optionp->InsertChildAtEnd(new SFIniEntry(userid,GetCurDate()));
		if (entryp==NULL)
			return false;
		}
	else
		entryp->set_value(GetCurDate());
	return true;
}


bool SFIniNewsManager::UpdateItemCutoffDate(SFIniEntry *entryp,const String &userid)
{
	// add a mark to an entry saying this user has read this entry
	if (userid!="")
		entryp->add_ChildValue("READMARK",userid);
	return true;
}


String SFIniNewsManager::GetCurDate()
{
	long dateval;

	time_t curr=time(NULL);
	dateval=curr;
	return MyLongToStr(dateval);
}

long int SFIniNewsManager::GetDateMinusDays(int daycount)
{
	long dateval;

	time_t curr=time(NULL);
	dateval=curr;
	dateval-=(daycount*60*60*24);
	return dateval;
}



String SFIniNewsManager::UserId(const String &chan,const String &nick,const String &ident)
{
	// return a string identifying the user
	// Here we use a hash, so as not to leave any user's real info in the config file - my god we are paranoid ;)
	String retstr;
	CMD5 md5;
	md5.setPlainText(ident.c_str());
	retstr=String(md5.getMD5Digest());
	return retstr;
}

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
String SFIniNewsManager::GetPermissionStr(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,SFIniEntry *entryp)
{
	// climb the hierarchy and get the relevant permissions
	// Idea is that we can have a hierarchy of permission levels for different conditions, plus the ability to set passwords at any level of the hierarchy
	String pstring="";
	String sstring;
	String retstring;
	bool bretv;
	
	// init
	bool perm_read=false;
	bool perm_add=false;
	bool perm_del=false;
	bool perm_set=false;

	// default
	bretv=entryp->GetUpwardValue("Perm_Default",pstring);
	UpdatePerms(pstring,perm_read,perm_add,perm_del,perm_set);

	// userclass related
	if (userclass!="")
		{
		sstring="Perm_Class:"+userclass;
		bretv=entryp->GetUpwardValue(sstring,pstring);
		UpdatePerms(pstring,perm_read,perm_add,perm_del,perm_set);
		}

	// channel related
	if (chan!="")
		{
		sstring="Perm_Chan:"+chan;
		bretv=entryp->GetUpwardValue(sstring,pstring);
		UpdatePerms(pstring,perm_read,perm_add,perm_del,perm_set);
		}
		
	// password related
	if (password!="")
		{
		sstring="Perm_Pass:"+password;
		bretv=entryp->GetUpwardValue(sstring,pstring);
		UpdatePerms(pstring,perm_read,perm_add,perm_del,perm_set);
		}

	// ok now form the current permission string
	retstring="";
	if (perm_read)
		retstring+="r";
	if (perm_add)
		retstring+="a";
	if (perm_del)
		retstring+="d";
	if (perm_set)
		retstring+="s";
	return retstring;
}

void SFIniNewsManager::UpdatePerms(const String &pstring,bool &perm_read,bool &perm_add,bool &perm_del,bool &perm_set)
{
	char *p;
	const char *sp=pstring.c_str();
	
	if ((p=strchr(sp,'r'))!=NULL)
		{
		if (p>sp && *(p-1)=='-')
			perm_read=false;
		else
			perm_read=true;
		}
	if ((p=strchr(sp,'a'))!=NULL)
		{
		if (p>sp && *(p-1)=='-')
			perm_add=false;
		else
			perm_add=true;
		}
	if ((p=strchr(sp,'d'))!=NULL)
		{
		if (p>sp && *(p-1)=='-')
			perm_del=false;
		else
			perm_del=true;
		}
	if ((p=strchr(sp,'s'))!=NULL)
		{

		if (p>sp && *(p-1)=='-')
			perm_set=false;
		else
			perm_set=true;	
		}
}

bool SFIniNewsManager::HasPermission(const String &chan,const String &nick,const String &permstring,char *pc, bool errorifnot)
{
	// return false if any chars in pc are not in permissions

	// ATTN: test
//	return true;

	int slen=strlen(pc);
	char c;
	for (int count=0;count<slen;++count)
		{
		c=pc[count];
		if (strchr(permstring.c_str(),c)==NULL)
			{
			if (errorifnot)
				SendNickNoticeOutput(chan,nick,"You dont have sufficient permission to do that.");
			return false;
			}
		}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void SFIniNewsManager::AutoCleanupBoard(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password)
{
	// board can have variable in it which indicates that we should trim old items, etc
	String retstr;
	int val;
	
	retstr=rootp->get_ChildValue("AutoTrimDays");
	if (retstr!="")
		{
		val=atoi(retstr.c_str());
		ClearByDays(chan,nick,ident,userclass,password,val,false);
		}
	retstr=rootp->get_ChildValue("AutoTrimCount");	
	if (retstr!="")
		{
		val=atoi(retstr.c_str());
		ClearByCount(chan,nick,ident,userclass,password,val,false);
		}	
}
void SFIniNewsManager::AutoCleanupSection(const String &chan,const String &nick,const String &ident,const String &userclass,const String &password,SFIniEntry *sectionp)
{
	// sections can have variables in them which indicate that we should trim old items, etc
	String retstr;
	String sectionname;
	int val;

	sectionname=sectionp->get_value();
	retstr=sectionp->get_ChildValue("AutoTrimDays");
	if (retstr!="")
		{
		val=atoi(retstr.c_str());
		ClearSectionByDays(chan,nick,ident,userclass,password,sectionname,val,false);
		}
	retstr=sectionp->get_ChildValue("AutoTrimCount");	
	if (retstr!="")
		{
		val=atoi(retstr.c_str());
		ClearSectionByCount(chan,nick,ident,userclass,password,sectionname,val,false);
		}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool SFIniNewsManager::AutoIrcTopic(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &ircsection)
{
	// there is an option that allows us to have a special board section which records all irc section changes
	SFIniEntry *sectionp,*entryp,*curp;
	String sectionname;
	String retstr,entryval;

	retstr=rootp->get_ChildValue("AutoIrcTopic");
	if (retstr!="")
		{
		sectionname=retstr;
		// yes, retstr tells us which board section to store it under
		// if the irc section doesnt exist yet then we create it

		sectionp=LookupSection(chan, nick,sectionname,false);
		if (!sectionp)
			AddSection(chan,nick,enick,ident,userclass,password,sectionname,"Irc Topics");
		sectionp=LookupSection(chan,nick,sectionname,true);
		if (sectionp!=NULL)
			{
			if (paranoidmode)
				{
				// in paranoidmode we dont allow saving of regular nick or channel name
				entryp=AddItem(chan,nick,enick,ident,userclass,password,sectionname,ircsection,false,false);
				}
			else
				entryp=AddItem(chan,nick,enick,ident,userclass,password,sectionname,chan+" - "+ircsection,false,false);
			// before we finish we want to make sure that we are adding an item which is a duplicate, which can happen when we rejoin channels, etc
			// this is further complicated by the fact that one forum may host multiple channels, so we have to look back a bit
			entryval=entryp->get_value();
			curp=entryp->get_prevp();
			while (curp!=NULL)
				{
				if ( entryval == curp->get_value())
					{
					// it is a duplicate
					delete curp;
					break;
					}
				curp=curp->get_prevp();
				}
			}
		}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void SFIniNewsManager::SetModes()
{
	// set the paranoid mode if variable is set
	// paranoid mode if set in the news configuration file prevents user info from being saved
	// paranoia mode prevents the news bot from saving the names of nicks who change sections, which are by nature unencrypted
	// paranois mode also prevents the log file from saving nicks and idents
	String modestr;
	modestr=rootp->get_ChildValue("Paranoia");
	if (modestr=="true")
		paranoidmode=true;
	else
		paranoidmode=false;
	// detailed new tracking (for each post we keep track of who has read it)
	modestr=rootp->get_ChildValue("DetailedNewTracking");
	if (modestr=="false")
		detailednewtracking=false;
	else
		detailednewtracking=true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void SFIniNewsManager::UpdateLogFile(const String &chan,const String &nick,const String &enick,const String &ident,const String &userclass,const String &password,const String &commandstr)
{
	// we can optionally log all commands sent to the bot
	String logfilename;
	String dirpath;;

	logfilename=rootp->get_ChildValue("AutoLogFilename");
	if (logfilename!="")
		{
		// open the file if it is not already open
		if (logfile==NULL)
			{
			// get the dir of last open file as our dir if we do not specify one
			dirpath=ExtractDirPath(logfilename);
			if (dirpath=="")
				{
				dirpath=ExtractDirPath(get_filename());
				if (dirpath!="")
						logfilename=dirpath+PATHSEPSTR+logfilename;
				}
			logfile=fopen(logfilename.c_str(),"a+");
			if (logfile==NULL)
				return;
			}
		fprintf(logfile,"COMMAND: %s\n",commandstr.c_str());
		if (!paranoidmode)
			{
			fprintf(logfile,"CHAN: %s\n",chan.c_str());
			fprintf(logfile,"NICK: %s\n",nick.c_str());
			fprintf(logfile,"IDENT: %s\n",ident.c_str());
			fprintf(logfile,"ADMINPASS: %s\n",password.c_str());
			}
		else
			{
			if (enick!="" && enick!=nick)
				fprintf(logfile,"ENICK: %s\n",enick.c_str());
			}
		fprintf(logfile,"USERCLASS: %s\n",userclass.c_str());
		fprintf(logfile,"\n");
		}
}
//---------------------------------------------------------------------------


String SFIniNewsManager::MyIntToStr(int val)
{
	return MyLongToStr((long)val);
}

String SFIniNewsManager::MyLongToStr(long val)
{
	char cnum[80];
	String retstr;
	
	sprintf(cnum,"%ld",val);
	retstr=String(cnum);
	return retstr;
}





//---------------------------------------------------------------------------

bool SFIniNewsManager::SendNickNoticeOutput(const String &chan,const String &nick,const String &outstr)
{
	if (outputmode==OUTMODE_EGGDROP)
		{
		// eggdrop output
		if (publicspam)
			printf("privmsg %s :%s\n",chan.c_str(),outstr.c_str()); 
		else if (directchan)
			printf("NOTICE %s :%c%c%s %s\n",nick.c_str(),150,150,chan.c_str(),outstr.c_str()); 
		else
			printf("NOTICE %s :%s\n",nick.c_str(),outstr.c_str()); 
		}
	else
		{
		// mirc output
		if (publicspam)
			printf(".privmsg :%s %s\n",chan.c_str(),outstr.c_str()); 
		else if (directchan)
			printf(".notice %s %c%c%s %s\n",nick.c_str(),150,150,chan.c_str(),outstr.c_str()); 
		else
			printf(".notice %s %s\n",nick.c_str(),outstr.c_str()); 
		}
	return true;
}

bool SFIniNewsManager::SendChanOutput(const String &chan,const String &nick,const String &outstr)
{
	if (outputmode==OUTMODE_EGGDROP)
		{
		// eggdrop output
		if (directchan)
			printf("NOTICE %s :%c%c%s %s\n",chan.c_str(),150,150,chan.c_str(),outstr.c_str()); 
		else
			printf("NOTICE %s :%s\n",chan.c_str(),outstr.c_str()); 
		}
	else
		{
		// mirc output
		if (directchan)
			printf(".notice %s %c%c%s %s\n",chan.c_str(),150,150,chan.c_str(),outstr.c_str()); 
		else
			printf(".notice %s %s\n",chan.c_str(),outstr.c_str()); 
		}
	return true;
}
//---------------------------------------------------------------------------

