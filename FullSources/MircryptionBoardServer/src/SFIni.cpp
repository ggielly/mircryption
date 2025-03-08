// Hierarchical Configuration File Manager
// SFini.cpp
// 8/18/02
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "stdafx.h"
#include "SFIni.h"
#include <time.h>
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
SFIniManager::SFIniManager()
{
	// constructor
	rootp=NULL;
	rootprefix="ROOT";
	rootvalue="";
	lastfilename="";
}


SFIniManager::~SFIniManager()
{
	// a delete of rootp should recursively delete entire tree, but we walk in case of multiple roots
	DeleteTree();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void SFIniManager::DeleteTree()
{
	// a delete of rootp should recursively delete entire tree, but we walk in case of multiple roots
	SFIniEntry *nextp;
	while (rootp!=NULL)
		{
		nextp=rootp->get_nextp();
		delete rootp;
		rootp=nextp;
		}
}


void SFIniManager::ClearAndInitialize(String inrootprefix,String inrootvalue)
{
	// erase any existing tree
	DeleteTree();

	// create the root node
	rootprefix=inrootprefix;
	rootvalue=inrootvalue;
	rootp=new SFIniEntry(rootprefix,rootvalue);
}


bool SFIniManager::ReadFile(String filename)
{
	// Read a text file into the structure
	// NOTE:  this will ADD the file as a brother to the current root! You should delete the tree prior to calling this if you want the root from file

	int depth=0;
	FILE *filep;
	SFIniEntry *curp;
	SFIniEntry *parentp;
	SFIniEntry *newp;
	String prefix;
	String value;
	char line[MAXLINELEN];
	char *retp;

	// open the file
	filep=fopen(filename.c_str(),"rb");
	if (!filep)
		return false;

	// ok walk through the file
	curp=rootp;
	parentp=NULL;
	for (;;)
		{
		retp=fgets(line,MAXLINELEN-1,filep);
		if (retp==NULL)
			break;
		line[MAXLINELEN-1]='\0';
		// trim leading whitespace
		TrimWhiteSpace(line);
		// is it a comment
		if (strlen(line)==0)
			continue;
		if (strncmp(line,SYM_COMMENT,strlen(SYM_COMMENT))==0)
			continue;
		if (strcmp(line,SYM_STARTNODE)==0)
			{
			++depth;
			// now record the parent
			parentp=curp;
			curp=NULL;
			continue;
			}
		if (strcmp(line,SYM_ENDNODE)==0)
			{
			--depth;
			// now pop up from the parent
			if (curp==NULL && parentp==NULL)
				{
				// error
				break;
				}
			if (curp==NULL && parentp!=NULL)
				{
				// pop up and what was our parent is now our brother
				curp=parentp;
				continue;
				}
			// we are in a chain, so first walk back to first child in chain
			while (curp->get_prevp()!=NULL)
				curp=curp->get_prevp();
			// then pop up to parent
			curp=curp->get_parentp();
			continue;
			}
		// now it it an entry, so split off prefix and value
		SplitLineToStrings(line,SYM_ASSIGN,prefix,value);
		if (prefix=="" && value=="")
			continue;
		// create the entry
		newp=new SFIniEntry(prefix,value);
//		printf("adding node %s,%s\n",prefix.c_str(),value.c_str());
		if (curp!=NULL)
			{
			curp->InsertAfter(newp);
			}
		else if (parentp!=NULL)
			{
			parentp->InsertChildAtEnd(newp);
			}
		else if (rootp==NULL)
			{
			// it's the root of a new tree
			rootp=newp;
			}
		else
			{
			// brother of root
			rootp->InsertAfter(newp);
			}
		// now record as brother
		curp=newp;
		}
		
	// close file
	fclose(filep);

	// error if not balanced tree
//	if (depth!=0)
//		return false;

	lastfilename=filename;
	return true;
}


void SFIniManager::TrimWhiteSpace(char *line)
{
	// remove any trailing and leading spaces from a char array
	int count;
	int index;
	char c;

	// first from begining
	for (count=0;;++count)
		{
		c=line[count];
		if (c!=32 && c!='\t' && c!=13 && c!=10)
			break;
		}
	if (count>0)
		{
		index=0;
		for (;;++count)
			{
			c=line[count];
			line[index]=c;
			++index;
			if (c=='\0')
				break;
			}
		}
	// from end
	index=strlen(line)-1;
	while(index>=0)
		{
		c=line[index];
		if (c!=32 && c!='\t' && c!=13 && c!=10)
			break;
		--index;
		}
	line[index+1]='\0';
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool SFIniManager::SplitLineToStrings(char *line,char *sep,String &prefix,String &value)
{
	char *pos;

	if (line[0]=='\0')
		return false;
		
	pos=strstr(line,sep);
	if (pos==NULL)
		{
		prefix=String(line);
		value="";
		return true;
		}
	*pos='\0';
	prefix=String(line);
	pos+=strlen(sep);
	value=String(pos);
	return true;
}

bool SFIniManager::SplitLineToChars(char *line,char *sep,char *&prefix,char *&value)
{
	char *pos;

	if (line[0]=='\0')
		{
		prefix=line;
		value=&line[0];
		return false;
		}
		
	pos=strstr(line,sep);
	if (pos==NULL)
		{
		prefix=line;
		value=&line[strlen(line)];
		return true;
		}
	*pos='\0';
	prefix=line;
	value=pos+1;
	return true;
}

bool SFIniManager::SplitStringToStrings(const String &instr,const String &sepstring,String &prefix,String &value)
{
	char *pos;
	char *line;
	bool bretv;

	line=new char[instr.length()+1];
	strcpy(line,instr.c_str());

	if (line[0]=='\0')
		bretv=false;
	else
		{
		pos=strstr(line,sepstring.c_str());
		if (pos==NULL)
			{
			prefix=String(line);
			value="";
			bretv=true;
			}
		else
			{
			*pos='\0';
			prefix=String(line);
			pos+=strlen(sepstring.c_str());
			value=String(pos);
			bretv=true;
			}
		}

	delete [] line;
	return bretv;
}

String SFIniManager::ExtractDirPath(String fullfilename)
{
	// extract the left path of a file
	const char *p;
	int len=0;
	String retstr;
	unsigned char c;
	int index;

	p=fullfilename.c_str();
	index=0;;
	for (;;)
		{
		c=*p;
		if (c=='\0')
			break;
		if (c==PATHSEPCHAR)
			len=index;
		// ATTN: test to avoid issues with platforms, just assume last \\ or / is end of dir
		if (c=='\\' || c=='/')
			len=index;
		++p;
		++index;
		}
	if (len>0)
		retstr=fullfilename.substr(0,len);
	else
		retstr="";
	return retstr;
}


/*
bool SFIniManager::RightStringSplit(String &mainstr,String &rightword, const String &splitstr)
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

// string splitters - picks off left or right word of string
bool SFIniManager::LeftStringSplit(String &mainstr,String &leftword, const String &splitstr)
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
*/

//---------------------------------------------------------------------------



bool SFIniManager::WriteFile(String filename)
{
	char tabstring[100];
	int count;
	int depth,lastdepth;
	FILE *filep;
	SFIniEntry *curp;
	String prefix;
	String value;
	String bracketstr;
	
	// open the file
	filep=fopen(filename.c_str(),"wb");
	if (!filep)
		return false;
	
	// init the tab string
	for (count=0;count<100;++count)
		tabstring[count]='\t';


	// walk through tree and write the nodes
	// we could do this recursively pretty cleanly, but lets see if we can do it without recursion
	depth=0;
	curp=rootp;
	while (curp!=NULL)
		{
		// write this node
		if (depth>=0 && depth<100)
			{
			tabstring[depth]='\0';
			fprintf(filep,"%s",tabstring);
			tabstring[depth]='\t';	
			}
		prefix=curp->get_prefix();
		value=curp->get_value();
		if (value!="")
			fprintf(filep,"%s%s%s\n",prefix.c_str(),SYM_ASSIGN,value.c_str());
		else
			fprintf(filep,"%s\n",prefix.c_str());

		// now can we go down inside this tree?
		lastdepth=depth;
		curp=GetNextFlattenedNode(curp,depth,true);
		
		// now we do a little tricky and print brackets depending on where the next element is...
		if (depth>lastdepth)
			{
			bracketstr=SYM_STARTNODE;
			}
		else
			{
			bracketstr=SYM_ENDNODE;
			}
		while (lastdepth!=depth)
			{
			if (depth>lastdepth)
				lastdepth+=1;
			if (lastdepth>=0 && lastdepth<100)
			{
				tabstring[lastdepth]='\0';
				fprintf(filep,"%s",tabstring);
				tabstring[lastdepth]='\t';	
				}
			fprintf(filep,"%s\n",bracketstr.c_str());
			if (depth<lastdepth)
				lastdepth-=1;
			}
		}
	
	// close the file
	fclose(filep);
	
	lastfilename=filename;
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool SFIniManager::MakeTestTree()
{
	// make a test tree
	SFIniEntry *curp,*curp1,*curp2,*curp3;
	
	// clear tree
	ClearAndInitialize();

	curp=new SFIniEntry("section","Section 1");
	rootp->InsertChildAtEnd(curp);
	curp1=new SFIniEntry("section","Section 2");
	rootp->InsertChildAtEnd(curp1);
	curp2=new SFIniEntry("section","Section 2.1");
	curp1->InsertChildAtEnd(curp2);
	curp3=new SFIniEntry("section","Section 2.1.1");
	curp2->InsertChildAtEnd(curp3);
	curp3=new SFIniEntry("section","Section 2.1.2");
	curp2->InsertChildAtEnd(curp3);
	curp1=new SFIniEntry("section","Section 3");
	rootp->InsertChildAtEnd(curp1);
	curp2=new SFIniEntry("sectionA","");
	curp1->InsertChildAlphabetically(curp2);
	curp2=new SFIniEntry("sectionD","");
	curp1->InsertChildAlphabetically(curp2);
	curp2=new SFIniEntry("sectionC","");
	curp1->InsertChildAlphabetically(curp2);
	curp2=new SFIniEntry("sectionB","");
	curp1->InsertChildAlphabetically(curp2);

	return true;
}
//---------------------------------------------------------------------------

	



//---------------------------------------------------------------------------
SFIniEntry *SFIniManager::GetNextFlattenedNode(SFIniEntry *curp, int &depth, bool explorechildren)
	{
	// this function will walk in a depth first, left-to-right linear function, to the next node in the tree
	// adjusting depth along the way
	if (curp==NULL)
		return NULL;

	// childen to explore?	
	if (explorechildren && curp->get_childp()!=NULL)
		{
		++depth;
		curp=curp->get_childp();
		return curp;
		}
		
	// we cannot, can we go to next brother?
	if (curp->get_nextp()!=NULL)
		{
		curp=curp->get_nextp();
		return curp;
		}
		
	if (!explorechildren)
		{
		curp=NULL;
		return curp;
		}
		
	// if we cannot, then we need to pop back up to the parents brother - this is the tricky part
	for (;;)
		{
		// first back up to first child
		while (curp->get_prevp()!=NULL)
			curp=curp->get_prevp();
		// now pop back up to parent
		curp=curp->get_parentp();
		if (curp==NULL)
			{
			// we are at top so we are done
			break;
			}
		// ok we've popped back up ONE parent level, if it has a brother, we go to it
		--depth;
		if (curp->get_nextp()!=NULL)
			{
			// ok parent has a brother we have not iterated yet, so we work on it
			curp=curp->get_nextp();
			break;
			}
		// otherwise we need to pop back up another level
		}
	return curp;
	}



//---------------------------------------------------------------------------
String SFIniManager::DateNumToString(long dateval)
	{
	String datestring;

	time_t curtime;

//	char dstring[80];
//	tm curtimetm;
//	tm *cp;
//	cp=(localtime(&curtime));
//	curtimetm=*cp;

	curtime=dateval;
	datestring=String(ctime(&curtime));

	//strftime(dstring,80,"%a %b %d %Y, %I:%M%p",&curtimetm);
	StringTrim(datestring);
	return datestring;
	}

void SFIniManager::StringTrim(String &str)
	{
	int slen=str.length();
	int nlen;
	const char *sp=str.c_str();
	char c;
	int start,end;

	for (start=0;;++start)
		{
		c=sp[start];
		if (c!=10 && c!=13 && c!=32 && c!=9)
			break;
		}
	for (end=slen-1;end>=0;--end)
		{
		c=sp[end];
		if (c!=10 && c!=13 && c!=32 && c!=9)
			break;
		}

	nlen=(end-start)+1;
	if (nlen<=0)
		str="";
	else
		str=str.substr(start,nlen);
	}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
SFIniEntry::SFIniEntry(String inprefix,String invalue)
{
	// set values
	prefix=inprefix;
	value=invalue;
	// initialize pointers
	prevp=NULL;
	nextp=NULL;
	parentp=NULL;
	childp=NULL;
}


SFIniEntry::~SFIniEntry()
{
	// recursively delete children then delete ourselves
	
	// delete child tree
	if (childp!=NULL)
		{
		delete childp;
		childp=NULL;
		}
	
	// splice it out, if it is not already spliced out
	SpliceOut();
}


SFIniEntry *SFIniEntry::LookupChildren(const String &prefixmask,const String &valuemask,int index,bool recursive)
{
	// return the child pointer indexed by index(starting at 0) which matches the masks
	SFIniEntry *np=NULL;
	SFIniEntry *curp;
	int depth=0;
	int count=0;

	curp=childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixmask,valuemask))
			{
			if (++count==index)
				{
				np=curp;
				break;
				}
			}
		curp=SFIniManager::GetNextFlattenedNode(curp,depth,recursive);
		if (depth<0)
			{
			// we've popped above parent, so stop
			break;
			}
		}

	return np;
}


String SFIniEntry::get_ChildValue(const String &prefixname)
{
	// lookup the child which matches and return its value
	SFIniEntry *curp;

	curp=childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixname,"*"))
			return curp->get_value();
		curp=curp->nextp;
		}
	return "";
}

void SFIniEntry::set_ChildValue(const String &prefixname,const String &val)
{
	SFIniEntry *curp;

	curp=childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixname,"*"))
			{
			curp->set_value(val);
			break;
			}
		curp=curp->nextp;
		}
}



void SFIniEntry::add_ChildValue(const String &prefixname,const String &val)
{
	// see if a child already exists with this exact prefix and value and if not add it
	// note this will NOT replace a child with same prefix but dif value
	SFIniEntry *curp;

	curp=childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixname,val))
			return;
		curp=curp->nextp;
		}
	// doesnt exist, so add it
	InsertChildAtEnd(new SFIniEntry(prefixname,val));
}

int SFIniEntry::CountChildren(const String &prefixmask,const String &valuemask,bool recursive)
{
	// count the number of children which match the masks
	SFIniEntry *curp;
	int depth=0;
	int count=0;

	curp=childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixmask,valuemask))
			++count;
		curp=SFIniManager::GetNextFlattenedNode(curp,depth,recursive);
		if (depth<0)
			{
			// we've popped above parent, so stop
			break;
			}
		}

	return count;
}


int SFIniEntry::FindChildIndex(const String &prefixmask,const String &valuemask)
{
	// return the index of us in our parent's list of matching children
	SFIniEntry *p,*curp;
	int depth=0;
	int count=-1;

	// get parent
	curp=this;
	while (curp->prevp!=NULL)
		curp=curp->prevp;
	p=curp->parentp;
	if (p==NULL)
		return -1;

	curp=p->childp;
	while (curp!=NULL)
		{
		if (curp->MaskMatch(prefixmask,valuemask))
			++count;
		if (curp==this)
			break;
		curp=curp->nextp;
		}

	// hard to see how this could happen
	if (curp==NULL)
		return -1;

	return count;
}



bool SFIniEntry::MaskMatch(String prefixmask,String valuemask)
{
	// return true if the mask matches the prefix and value
	// for now we just use very restrictive  mask
	bool pm=false;
	bool vm=false;
	String lcprefix;
	String lcvalue;


	lcprefix=prefix;
	lcvalue=value;
	StringToLower(lcprefix);
	StringToLower(lcvalue);
	StringToLower(prefixmask);
	StringToLower(valuemask);

	if (prefixmask==lcprefix)
		pm=true;
	else if (prefixmask=="*")
		pm=true;
	if (valuemask==lcvalue)
		vm=true;
	else if (valuemask=="*")
		vm=true;
	if (pm && vm)
		return true;
	return false;
}

void SFIniEntry::StringToLower(String &s)
{
	// hey its not like anyone would ever need to convert a string to lowercase right? fucking assholes.
	return;
	
	//	char *p;
	//	p= new char[s.length()+1];
	//	strcpy(p,s.c_str());
	//	strlwr(p);
	//	s=String(p);
	//	delete p;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
SFIniEntry *SFIniEntry::InsertBefore(SFIniEntry *entrytoinsert)
{
	// insert item before us
	// IMPORTANT: the entry to insert should not have nextp or prevp entries! (it can have children)
	entrytoinsert->prevp=prevp;
	if (prevp!=NULL)
			prevp->nextp=entrytoinsert;
	prevp=entrytoinsert;
	entrytoinsert->nextp=this;
	if (parentp!=NULL)
		{
		entrytoinsert->parentp=parentp;
		parentp->childp=entrytoinsert;
		parentp=NULL;
		}
	return entrytoinsert;
}

SFIniEntry *SFIniEntry::InsertAfter(SFIniEntry *entrytoinsert)
{
	// insert item after us
	// IMPORTANT: the entry to insert should not have nextp or prevp entries! (it can have children)
	entrytoinsert->nextp=nextp;
	nextp=entrytoinsert;
	entrytoinsert->prevp=this;
	return entrytoinsert;
}
	
SFIniEntry *SFIniEntry::InsertChildAtBegining(SFIniEntry *entrytoinsert)
{
	// fixup entry
	entrytoinsert->parentp=this;
	
	// fixup parent
	if (childp==NULL)
		childp=entrytoinsert;
	else
		{
		// fixup the old first child
		childp->prevp=entrytoinsert;
		childp->parentp=NULL;
		// make the new entry the new first child
		entrytoinsert->nextp=childp;
		childp=entrytoinsert;
		}
	return entrytoinsert;
}

SFIniEntry *SFIniEntry::InsertChildAtEnd(SFIniEntry *entrytoinsert)
{
	SFIniEntry *curp;
	if (childp==NULL)
		{
		// if there are no children yet then use the insertatend procedure
		return InsertChildAtBegining(entrytoinsert);
		}
	// find last child
	curp=childp;
	while (curp->nextp!=NULL)
		curp=curp->nextp;
	// ok now add to end
	curp->InsertAfter(entrytoinsert);
	return entrytoinsert;
}


SFIniEntry *SFIniEntry::InsertChildAlphabetically(SFIniEntry *entrytoinsert, bool tieabove)
{
	// insert a child at the alphatetical order
	SFIniEntry *curp;
	int compareval;
	
	curp=childp;
	while (curp!=NULL)
		{
		// do we belong before curp?
		compareval=strcmp((entrytoinsert->get_prefix()).c_str(),(curp->get_prefix()).c_str());
		if (compareval==0)
			compareval=strcmp((entrytoinsert->get_value()).c_str(),(curp->get_value()).c_str());
		if ((compareval==0 && tieabove) || compareval<0)
			{
			// ok we are earlier in alphabetical order than the current one
			break;
			}
		curp=curp->nextp;
		}
		
	if (curp!=NULL)
		{
		// insert before this child
		curp->InsertBefore(entrytoinsert);
		}
	else
		{
		// insert at end
		InsertChildAtEnd(entrytoinsert);
		}
	return entrytoinsert;
}


bool SFIniEntry::SpliceOut()
{
	// splice out a node by patching up its neighbors
	if (prevp!=NULL)
		prevp->nextp=nextp;
	if (nextp!=NULL)
		nextp->prevp=prevp;
	if (parentp!=NULL)
		{
		parentp->childp=nextp;
		if (nextp!=NULL)
			nextp->parentp=parentp;
		}
	
	// clear our pointers
	prevp=NULL;
	nextp=NULL;
	parentp=NULL;
	return true;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/*
SFIniEntry *SFIniEntry::LookupPath(String nodepath)
{
	// look up a dot separated path of children relative to us
	String nodename;
	bool bretv;
	SFIniEntry *curp=this;
	
	whlie (curp!=NULL)
		{
		// split of a word from left, and find it as a child of curp
		bretv=SFIniManager::LeftStringSplit(nodepath,nodename, ".");
		if (!bretv)
			break;
		// find nodename as a child of curp;
		curp=curp->childp;
		while (curp!=NULL && !match
		// ATTN: unfinished
		}

	return curp;
}
*/

//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
bool SFIniEntry::get_valueint(int &intval)
{
	// convert value to int and return it

	intval=atoi(value.c_str());
	return true;
}


bool SFIniEntry::get_valuefloat(float &floatval)
{
	// convert value to float and return it
	floatval=atof(value.c_str());

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool SFIniEntry::isnewer(long lastcheckdate,String userident)
	{
	SFIniEntry *datep;
	SFIniEntry *readmark;
	long itemdate;
	String datestring;
	
	// there are 2 ways an entry can be considered NOT new
	// either it was added before the last check date, OR it has a special entry saying the userident has read it
	if (userident!="")
		{
		// see if the user has explicitly read it
		readmark=LookupChildren("READMARK",userident);
		if (readmark!=NULL)
				return false;
		}
	datep=LookupChildren("ADDDATE","*");
	if (datep!=NULL)
		{
		datestring=datep->get_value();
		itemdate=atol(datestring.c_str());
		}
	if (itemdate>lastcheckdate)
		return true;
	return false;
	}


String SFIniEntry::GetAuthorString()
	{
	// form a string saying who added this
	SFIniEntry *datep;
	int itemdate;
	String authorstring;
	String datestring;
	String retstring="";

			
	datep=LookupChildren("ADDDATE","*");
	if (datep!=NULL)
		{
		datep->get_valueint(itemdate);
		datestring=SFIniManager::DateNumToString(itemdate);
		}
	else
		datestring="";

	authorstring=get_ChildValue("NICK");

	if (datestring!="")
		authorstring+=", "+datestring;
	return authorstring;
	}

String SFIniEntry::GetAuthorName()
	{
	// form a string saying who added this
	String authorstring;
	authorstring=get_ChildValue("NICK");
	return authorstring;
	}

String SFIniEntry::GetAuthorDate()
	{
	// form a string saying who added this
	SFIniEntry *datep;
	int itemdate;
	String datestring;
			
	datep=LookupChildren("ADDDATE","*");
	if (datep!=NULL)
		{
		datep->get_valueint(itemdate);
		datestring=SFIniManager::DateNumToString(itemdate);
		}
	else
		datestring="";

	return datestring;
	}

//---------------------------------------------------------------------------



bool SFIniEntry::GetUpwardValue(const String &varname,String &retstr)
{
	// proceed up the object parent tree till we find a parent which has a child variable varname, then return it
	// this is useful for getting the most appropriate permission
	SFIniEntry *parentp=this;
	SFIniEntry *entryp;
	while (parentp!=NULL)
		{
		entryp=parentp->LookupChildren(varname,"*");
		if (entryp!=NULL)
			{
			retstr=entryp->get_value();
			return true;
			}
		// climb up to parent
		while (parentp->get_prevp()!=NULL)
				parentp=parentp->get_prevp();
		parentp=parentp->get_parentp();
		}
	retstr="";
	return false;
}
