//---------------------------------------------------------------------------
// newsboard.cpp
// v 1.03
// 05/21/04
// this is a commandline main() class for invoking the news board as a standalone tool
// call newsboard like this:  newsboard [-eNICK] [-pPASS] [-bBOARDNAME] FILENAME CHAN NICK "IDENT" USERCLASS "COMMAND STRING"
//  output will be sent to stdout.
//
// v1.03 - 5/21/04 - fixed dirctory parsing which was failing for cygwin windrops
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "stdafx.h"
#include "SFIniNews.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// forward declarations
void ShowUsage();
bool SendNickNoticeOutput(const String &nick,const String &outstr, int outputmode);
//---------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	bool bretv;
	char blank[2];
	int argindex;
	String filename;
	FILE *tempfilep;
	String filedir;
	int expectedargs;
	int outputmode=OUTMODE_MIRC;
	bool directchan=false;

	
	// for parsing args
	argindex=1;
	expectedargs=7;
	
	// peek at first arg and see if it tells us the kind of called mode
	if (argc>1)
		{
		if (strcmp(argv[1],"-e")==0)
			{
			outputmode=OUTMODE_EGGDROP;
			++argindex;
			++expectedargs;
			}
		else if (strcmp(argv[1],"-m")==0)
			{
			outputmode=OUTMODE_MIRC;
			++argindex;
			++expectedargs;
			}
		}
	
	// make sure enough argumnts
	if (argc!=expectedargs || strcmp(argv[1],"-help")==0 || strcmp(argv[1],"--help")==0)
		{
		// argument error
		ShowUsage();
		}


	char *enick,*epass,*fname,*cname,*nname,*iname,*classname,*comstring;
	char *boardname;

	// point to blanks
	strcpy(blank,"");
	enick=blank;
	epass=blank;
	fname=blank;
	cname=blank;
	nname=blank;
	iname=blank;
	classname=blank;
	boardname=blank;
	comstring=blank;
	fname=argv[argindex++];
	cname=argv[argindex++];
	nname=argv[argindex++];
	iname=argv[argindex++];
	classname=argv[argindex++];
	comstring=argv[argindex++];

	// ATTN: test
	// printf("DEBUG fname is '%s'\n",fname);


	// new method to try to find a bug?
//	char enick[255],epass[255],fname[255],cname[255],nname[255],iname[255],classname[255],comstring[1000],boardname[255];
//	strcpy(fname,argv[argindex++]);
//	strcpy(cname,argv[argindex++]);
//	strcpy(nname,argv[argindex++]);
//	strcpy(iname,argv[argindex++]);
//	strcpy(classname,argv[argindex++]);
//	strcpy(comstring,argv[argindex++]);

	// SendNickNoticeOutput(nname,"DEBUG -> comstring is "+string(comstring),outputmode);

	if (strncmp(comstring,"-c",2)==0)
		{
		// they have specified an optional explicit channel to override default (this is useful when being submitted as a query/notice message)
		cname=comstring+2;
		comstring=strchr(comstring,32);
		if (comstring==NULL)
			{
			// argument error
			ShowUsage();
			}
		*comstring='\0';
		comstring+=1;
		// set the flag to say that a direct channel was specified, which will cause output to be directed at that
		directchan=true;
		// SendNickNoticeOutput(nname,"DEBUG -> channel name is "+string(cname),outputmode);
		}
	if (strncmp(comstring,"-e",2)==0)
		{
		// they have specified an optional encrypted nick to use for storing author info
		enick=comstring+2;
		comstring=strchr(comstring,32);
		if (comstring==NULL)
			{
			// argument error
			ShowUsage();
			}
		*comstring='\0';
		comstring+=1;
		}
	if (strncmp(comstring,"-p",2)==0)
		{
		// they have specified an optional password which can give better permissions
		epass=comstring+2;
		comstring=strchr(comstring,32);
		if (comstring==NULL)
			{
			// argument error
			ShowUsage();
			}
		*comstring='\0';
		comstring+=1;
		}
	if (strncmp(comstring,"-b",2)==0)
		{
		// they have specified an optional board name which allows us to run multiple news files from asme bot
		boardname=comstring+2;
		comstring=strchr(comstring,32);
		if (comstring==NULL)
			{
			// argument error
			ShowUsage();
			}
		*comstring='\0';
		comstring+=1;
		// SendNickNoticeOutput(nname,"DEBUG -> board name is "+string(boardname)+".",outputmode);
		}
	if (strncmp(comstring,"-i",2)==0)
		{
		// they have specified an optional ident name which overrides the irc ident when seeing when they last read the news, which allows them to check news from various idents
		iname=comstring+2;
		comstring=strchr(comstring,32);
		if (comstring==NULL)
			{
			// argument error
			ShowUsage();
			}
		*comstring='\0';
		comstring+=1;
		}
	
	// the user can specify the name of the board they are trying to interact with
	filename="";
	if (boardname!=blank)
		{
		// okay they have specified a non-default board name (file name)
		// make sure no \\ or // for safety reasons
		if (strchr(boardname,'/')==NULL && strchr(boardname,'\\')==NULL && strchr(boardname,'~')==NULL)
			{
			filename=String(boardname);
			// force an extension on it, for safety
			filename="mcb_news_"+filename+".cfg";

			// take path from default fname
			filedir=SFIniManager::ExtractDirPath(fname);
			if (filedir!="")
				filename=filedir+PATHSEPSTR+filename;
			}
		}

	// use default file name if none specified
	if (filename=="")
		{
		// we now use the default file name to specify a SEPARATE configuration file which tells us which board to use for a given channel
		// which is important for automatic irc sections.
		String cfgfilename;
		SFIniManager sficonfig;

		// read the config file name (specified by arguement fname)
		cfgfilename=String(fname);
		bretv=sficonfig.ReadFile(cfgfilename);
		if (!bretv)
			{
			SendNickNoticeOutput(nname,"Error: Improper news configuration file - inform news bot owner.\n",outputmode);
			return 0;
			}

		// now decide on the news file to use based on looking up caller channel/name in the config file
		SFIniEntry *entryp=NULL;
		SFIniEntry *rootp=sficonfig.get_rootp();
		if (rootp!=NULL)
			{
			if (cname!="")
				entryp=rootp->LookupChildren("CHANNEL",cname);
			else
				entryp=rootp->LookupChildren("CHANNEL","default");
			}
		if (entryp!=NULL)
			{
			// get the file name corresponding to this channel
			filename=entryp->get_ChildValue("filename");

			// now, is it ok to use this filename even if user did not specify it explicitly?
			// for automatic section changes, we need to say yes, but for requests from users, sometimes we dont want to allow any idiot
			// in channel to access this without knowing the news board name.
			if (strncmp(comstring,"autoirctopic",12)!=0)
				{
				if (entryp->get_ChildValue("isprivate")=="true")
					{
					SendNickNoticeOutput(nname,"Error: The news board for this channel is private - contact news bot owner or reconfigure the bot nick.\n",outputmode);
					return 0;
					}
				}
			else
				{
				// this is an auto topic, first check if the config file tells us tat board disallows it
				if (entryp->get_ChildValue("AutoIrcTopic")=="false")
					{
					// dont store topics here
					return 0;
					}
				}

			if (SFIniManager::ExtractDirPath(filename)=="")
				{
				// take path from default fname
				filedir=SFIniManager::ExtractDirPath(fname);
				if (filedir!="")
					filename=filedir+PATHSEPSTR+filename;
				}
			}
		}

	// make sure the news file exists
	if (filename=="" || (tempfilep=fopen(filename.c_str(),"rb"))==NULL)
		{
		if (strncmp(comstring,"autoirctopic",12)==0)
			{
			// no warning if cant find file to add auto irc sections too
			}
		else
			{
			SendNickNoticeOutput(nname,"Error: News file does not exist (or is not readable) - contact news bot owner or reconfigure the bot nick (board name '"+string(boardname)+").\n",outputmode);
//			SendNickNoticeOutput(nname,"Error: News file '"+filename+"' (board name '"+string(boardname)+"') does not exist or is not readable - contact news bot owner or reconfigure the bot nick.\n",outputmode);
			}
		return 0;
		}
	else
		fclose(tempfilep);

	// create the news board class
	SFIniNewsManager sinip;

	// set directchan
	sinip.set_directchan(directchan);

	// load the news board file
	sinip.DeleteTree();
	bretv=sinip.ReadFile(filename);
	if (!bretv)
		{
		// sinip.MakeDefaultNews();
		//	printf(".notice %s Created news file '%s'\n",nname,filename.c_str());
		SendNickNoticeOutput(nname,"Error: News file is not properly formatted - inform news bot owner.\n",outputmode);
		return 0;
		}

	// parse the command
	unsigned char *comstring2=(unsigned char *)comstring;
	string newcomstring((char*)comstring2);

	// fix up evil ascii switch characters for bugfix
	int len=newcomstring.length();
	for (int count=0;count<len;++count)
		{
		if (newcomstring[count]==30)
			newcomstring[count]=171;
		if (newcomstring[count]==31)
			newcomstring[count]=187;
		}

	// debug
	// SendNickNoticeOutput(nname,newcomstring,outputmode);

	sinip.Parse(outputmode,enick,epass,cname,nname,iname,classname,newcomstring);

	// now save the file
	bretv=sinip.WriteFile(filename);

	// and return success
	return 0;
}


void ShowUsage()
{
	// usage
	printf("echo -s ERROR in call to news bot. Call newsboard like this:  newsboard FILENAME CHAN NICK \"IDENT\" USERCLASS \"[-cCHAN] [-eNICK] [-pADMINPASS] [-bBOARDNAME] [-iIDENT] COMMAND STRING\"\n\n");
	exit(0);
}


bool SendNickNoticeOutput(const String &nick,const String &outstr, int outputmode)
{
	if (outputmode==OUTMODE_EGGDROP)
		{
		// eggdrop output
		printf("NOTICE %s :%s\n",nick.c_str(),outstr.c_str()); 
		}
	else
		{
		// mirc output
		printf(".notice %s %s\n",nick.c_str(),outstr.c_str()); 
		}
	return true;
}

