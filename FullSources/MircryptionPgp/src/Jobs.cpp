// Jobs.cpp

//---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#include <io.h>
#include <stdio>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
#include "Jobs.h"
#include "JobManager.h"
#include "MiscUtils.h"
#include "DerivedJobs.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// MessageQueue
MessageQueue::MessageQueue(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString instring)
{
	tochan=itochan;
	tonick=itonick;
	toident=itoident;
	string=instring;
	priorp=NULL;
	nextp=NULL;
	trycount=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//MPGPJob

MPGPJob::MPGPJob(AnsiString jid,AnsiString jt,AnsiString desc, AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts, int jobdir)
{
	// constructor

	// identification
	jobid=jid;
	description=desc;
	jobtype=jt;
	tochan=tc;
	tonick=tn;
	toident=ti;
	FromId=fid;
	ToId=tid;

	// contents
	contents=conts;

	// stage
	stagestring="";
	stagenum=0;

	// state
	suspended=false;
	canceled=false;
	accomplished=false;
	assembling=false;

	// aux state
	confirmed=false;
	expired=false;
	errored=false;

	// pointers
	priorp=NULL;
	nextp=NULL;
	childp=NULL;
	parentp=NULL;
	dependee=NULL;

	// incoming buffer
	storeinfile=false;
	maxfilesize=0;
	bufferfilename="";
	inbufferstring="";
	bufferp=NULL;
	totalsize=-1;
	receivedsize=0;
	incurpos=0;
	rewindpos=-1;

	// ATTN: TEST - store all contents in files
	//	maxfilesize=MaxSafeJobFileDataSize;
	//	storeinfile=true;

	// outgoing buffer position
	writefromfile=false;
	OutputFilename="";
	OutputString="";
	OutputJobtype="";
	curpos=0;
	outputlen=0;

	// direction - is it an incoming or outgoing job
	direction=jobdir;

	// update timestamp
	UpdateTimestamp();
}


MPGPJob::~MPGPJob()
{
	// delete the job

	// splice it out
	if (priorp!=NULL)
		priorp->nextp=nextp;
	if (nextp!=NULL)
		nextp->priorp=priorp;

	// splice out children?

	// adjust first and last
	if (jobmanager->get_Jfirstp()==this)
		{
		jobmanager->set_Jfirstp(nextp);
		}
	if (jobmanager->get_Jlastp()==this)
		{
		jobmanager->set_Jfirstp(priorp);
		}

	// clear pointers
	priorp=NULL;
	nextp=NULL;
	childp=NULL;
	parentp=NULL;

	// release buffer?
	if (bufferp!=NULL)
		delete [] bufferp;
	bufferp=NULL;
	totalsize=-1;
	receivedsize=0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void MPGPJob::AddChild(MPGPJob *cp)
{
	// add a child
	MPGPJob *p;

	// fist set the child's parent to us
	cp->parentp=this;

	// if we have no child, then just point to this child
	if (childp==NULL)
		childp=cp;
	else
		{
		// we need to add it to end of chain of children
		p=childp;
		while (p->nextp!=NULL)
			p=p->nextp;
		// add it to last one
		p->nextp=cp;
		cp->priorp=p;
		}

	// tell the jobmanager that jobs have changed
	jobmanager->SetJobsChanged();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MPGPJob::Cancel(AnsiString cancelreasonstr)
{
	MPGPJob *nchildp;
	AnsiString ccancelreasonstr;

	// is it already done?
	if (IsConfirmed() || IsCanceled())
		return true;

	// cancel current job
	if (cancelreasonstr!="")
		stagestring=cancelreasonstr;
	if (cancelreasonstr!="" && cancelreasonstr!="cancel" && cancelreasonstr!="canceled")
		jobmanager->LogMessage("Canceling job "+GetPJobid()+": "+cancelreasonstr+".");
	else
		jobmanager->LogMessage("Canceling job "+GetPJobid()+".");
	canceled=true;

	// derived calls
	DerivedStopped();
	DerivedCancel();

	// if there is a file buffer, we kill it
	if (storeinfile && bufferfilename!="" && totalsize>0)
		{
		// detelee the file
		JRSafeDeleteFile(bufferfilename);
		}

	// clear other states
	if (suspended)
		suspended=false;
	if (assembling)
		assembling=false;

	// now if it has children, cancel all it's children
	nchildp=childp;
	while (nchildp!=NULL)
		{
		if (cancelreasonstr!="expired")
			ccancelreasonstr="parent canceled";
		else
			ccancelreasonstr=cancelreasonstr;
		nchildp->Cancel(ccancelreasonstr);
		nchildp=nchildp->nextp;
		}

	// now if it has a parent, cancel its parent
	if (parentp!=NULL)
		{
		if (cancelreasonstr!="expired")
			ccancelreasonstr="child canceled";
		else
			ccancelreasonstr=cancelreasonstr;
		parentp->Cancel(ccancelreasonstr);
		}

	// tell the jobmanager that jobs have changed
	jobmanager->SetJobsChanged();

	return true;
}

bool MPGPJob::Suspend()
{
	// suspend job
	MPGPJob *nchildp;

	if (IsStopped() && !IsOutputting())
		return false;

	// suspend this job
	if (!suspended)
		suspended=true;
	DerivedSuspend();
	jobmanager->LogMessage("job "+GetPJobid()+" has suspended.");

	// suspend any children
	nchildp=childp;
	while (nchildp!=NULL)
		{
		nchildp->Suspend();
		nchildp=nchildp->nextp;
		}

	// tell the jobmanager that jobs have changed
	jobmanager->SetJobsChanged();

	return true;
}

bool MPGPJob::Resume()
{
	// resume job
	MPGPJob *nchildp;
	
	// resume this job
	if (suspended)
		suspended=false;
	DerivedResume();
	jobmanager->LogMessage("job "+GetPJobid()+" has resumed.");

	// resume any children
	nchildp=childp;
	while (nchildp!=NULL)
		{
		nchildp->Resume();
		nchildp=nchildp->nextp;
		}

	// tell the jobmanager that jobs have changed
	jobmanager->SetJobsChanged();

	// update timestamp
	UpdateTimestamp();

	return true;
}

void MPGPJob::BecomeAssembled()
{
	jobmanager->LogMessage("job "+GetPJobid()+" has finished assembling.");
	assembling=false;
}

void MPGPJob::BecomeAccomplished()
{
	if (accomplished)
		return;
	jobmanager->LogMessage("job "+GetPJobid()+" has been accomplished.");
	accomplished=true;
	// derived call
	DerivedStopped();
	DerivedAccomplished();
}

void MPGPJob::BecomeConfirmed(AnsiString confirmparam)
{
	if (confirmed)
		return;
	jobmanager->LogMessage("job "+GetPJobid()+" has been confirmed.");
	// derived call
	DerivedConfirmed(confirmparam);
	if (!accomplished)
		{
		DerivedStopped();
		DerivedAccomplished();
		}
	accomplished=true;
	confirmed=true;
}

void MPGPJob::BecomeExpired()
{
	if (confirmed || expired || canceled)
		return;
	jobmanager->LogMessage("job "+GetPJobid()+" is expiring..");
	if (!accomplished)
		{
		// if are accomplished when we expire, then we dont cancel we just expire
		Cancel("expired");
		}
	expired=true;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MPGPJob::IsSuspended()
{
	// return true if it is suspended (manually suspended)
	if (suspended)
		return true;
	return false;
}

bool MPGPJob::IsAssembling()
{
	// return true if it is still being partially assembled from incoming data
	if (assembling)
		return true;
	return false;
}


bool MPGPJob::IsAccomplished()
{
	// return true if it has completed successfully
	if (accomplished)
		return true;
	return false;
}


bool MPGPJob::IsConfirmed()
{
	// return true if it has completed successfully and confirmed
	if (confirmed)
		return true;
	return false;
}

bool MPGPJob::IsOutputting()
{
	// return true if it is still not fully outputted (can occur even when "done")
	if (canceled)
		return false;
	if (curpos<outputlen)
		return true;
	return false;
}

bool MPGPJob::IsExpired()
{
	// return true if it is expired (note it could be accomplished or canceled)
	if (expired)
		return true;
	return false;
}

bool MPGPJob::IsDependent()
{
	// return true if we are dependent on a non-completed job
	if (dependee!=NULL)
		{
		// we are dependent on another job to finish successfully before we can iterate
		if (!dependee->IsAccomplished())
			return true;
		}
	return false;
}

bool MPGPJob::IsReadyToIterate()
{
	// return true if we are not stopped or blocked or assembling, and are ready to iterate
	if (IsStopped())
		return false;
	if (IsSuspended())
		return false;
	if (IsDependent())
		return false;
	if (IsAssembling())
		return false;
	return true;
}

bool MPGPJob::IsStopped()
{
	// return true if we are done, succesfully or canceled
	if (accomplished || canceled)
		return true;
	return false;
}

bool MPGPJob::IsBlocked()
{
	// return true if we are (not canceled) and (not ready to execute), ie suspended or blocked or dependent
	if (IsReadyToIterate())
		return false;
	if (IsStopped())
		return false;
	return true;
}

bool MPGPJob::IsCanceled()
{
	// return true if we are canceled (ie error of some sort or manually canceled)
	if (canceled)
		return true;
	return false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MPGPJob::UpdateTimestamp()
{
	// update timestamp to now
	updatetime=Now();
}

void MPGPJob::set_stagestring(AnsiString str)
{
	if (stagestring!=str)
		{
		// tell the jobmanager that jobs have changed
		jobmanager->SetJobsChanged();
		if (str!="")
			jobmanager->LogMessage("job "+GetPJobid()+" - "+str+".");
		}
	// set the current stage description for the job
	if (str!=".")
		stagestring=str;

	// update timestamp
	UpdateTimestamp();
}

void MPGPJob::set_descstring(AnsiString str)
{
	if (description!=str)
		{
		// tell the jobmanager that jobs have changed
		jobmanager->SetJobsChanged();
		}
	// set the current description for the job
	description=str;
	UpdateTimestamp();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
AnsiString MPGPJob::GetPJobid()
{
	// job id (add an i for incoming jobs)
	if (direction==JOBDIR_INCOMING)
		return (jobid+"i");
	else
		return (jobid);
}


AnsiString MPGPJob::GetPJobid(AnsiString jobstr,int direction)
{
	// job id (add an i for incoming jobs)
	if (direction==JOBDIR_INCOMING)
		return (jobstr+"i");
	else
		return (jobstr);
}


AnsiString MPGPJob::GetJobDesc()
{
	// return a string descriptor of the job
	AnsiString str="";
	AnsiString desc="";

	// do we want to indent? to aid display reading?
	/*
	MPGPJob *p;
	int depth=0;
	p=parentp;
	while (p!=NULL)
		{
		++depth;
		p=p->parentp;
		// str+="+";
		}
	*/

	// id and description
	str+=GetPJobid()+" - "+description;

	// dynamically created desc component here
	if (desc!="")
		str+=" - "+desc;

	// stage suffic
	if (stagestring!="")
		str+=" - "+stagestring;

	// success suffixs
	if (IsConfirmed() || IsAccomplished())
		{
		if (curpos<outputlen)
			{
			float perc=float(curpos);
			perc/=float(outputlen);
			perc*=100.0;
			str+=" ("+AnsiString(int(perc))+"%)";
			}
		else if (IsConfirmed())
			str+=" (done)";
		else if (IsAccomplished())
			{
			if (IsExpired())
				str+=" (unconfirmed)";
			else
				str+=" (waiting for confirmation)";
			}
		}

	// failure suffixes
	if (IsCanceled() && stagestring!="canceled" && stagestring!="parent canceled" && stagestring!="child canceled" && stagestring!="expired")
		{
			str+=" (canceled)";
		}
	// suspended
	if (IsSuspended())
		str+=" (suspended)";

	// end with a period
	str+=".";

	return str;
}

AnsiString MPGPJob::PrettyId(AnsiString nick,AnsiString keydesc,bool showanon)
{
	// return a string like:  "nick (pgpkey@email.com)"
	// if keydesc==anonymous and showanon==true:  "nick (anonymous)"
	// if keydesc==anonymous and showanon==false:  "nick"
	AnsiString str="";
	// nick
	if (nick!="")
		str+=nick;
	// key details
	if (jobmanager->IsPgpKeyAnonymous(keydesc))
		{
		// key is anonymous
		if (showanon)
			str+=" (anonymous)";
		}
	else
		str+=" ("+StripKeymail(keydesc)+")";

	return str;
}


AnsiString MPGPJob::PrettySig(AnsiString nick,AnsiString keydesc,bool showanon)
{
	// return a string like:  "nick (signed by pgpkey@email.com)"
	// if keydesc==anonymous and showanon==true:  "nick (not signed)"
	// if keydesc==anonymous and showanon==false:  "nick"
	AnsiString str="";
	// nick
	if (nick!="")
		str+=nick+" (";
	// key details
	if (jobmanager->IsPgpKeyAnonymous(keydesc))
		{
		// key is anonymous
		if (showanon)
			{
			if (nick=="")
				str+=" ";
			str+="not signed";
			}
		}
	else
		{
		if (nick=="")
			str+=" ";
		str+="signed by "+StripKeymail(keydesc);
		}
	if (nick!="")
		str+=")";
	return str;
}


AnsiString MPGPJob::PrettyShort(AnsiString instr,int maxlen)
{
	// return str if it is < maxlen bytes, otherwise shorten it and add "..."
	AnsiString str="";

	if (instr.Length()>maxlen)
		str="\""+instr.SubString(1,maxlen-4)+"..\"";
	else
		str=instr;

	// change all newlines to commas?
	/*
	int len=str.Length();
	int c;
	for (int count=1;count<len;++count)
		{
		c=str[count];
		if (c==10 || c==13)
			str[count]=',';
		}
	*/

	return str;
}

AnsiString MPGPJob::StripKeymail(AnsiString keydesc)
{
	// return the key description after removing any optional email
	AnsiString str="";
	int pos=keydesc.Pos("<");
	if (pos>2)
		str=keydesc.SubString(1,pos-2);
	else
		str=keydesc;
	return str;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MPGPJob::Iterate()
{
	// iterate a job and its children
	// return true if anything was done by us or children
	MPGPJob *nchildp,*nextjobp;
	bool bretv1=false;
	bool bretv2=false;
	bool bretv3=false;
	bool cretv;

	// iterate any children
	nchildp=childp;
	while (nchildp!=NULL)
		{
		nextjobp=nchildp->nextp;
		cretv=nchildp->Iterate();
		nchildp=nextjobp;
		if (cretv)
			bretv3=true;
		}

	// now iterate ourselves
	if (IsReadyToIterate())
		{
		// iterate the job
		bretv1=DerivedIterate();
		}

	// process any buffer output, regadless if we are ready to iterate or note, as long as we are not suspended
	if (!IsSuspended() && !IsCanceled())
		bretv2=ProcessOutput();

	if (bretv1 || bretv2 || bretv3)
		return true;

	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MPGPJob::SetOutputString(const AnsiString &injobtype,const AnsiString &instring)
{
	// set the output string for a job.
	// not all jobs have outputs, but those that do should only call this once(?)
	// idea is that a job sets this, which we then encode for transfer,
	// and iteratively deliver it in small chunks

	// ste output string
	OutputString=instring;
	OutputJobtype=injobtype;
	EncodeStringForOutput(OutputString);
	//	jobmanager->LogMessage("ENCODE FOR OUTPUT: "+OutputString);
	// reset current position flag and length
	curpos=0;
	outputlen=OutputString.Length();
	return true;
}

bool MPGPJob::SetOutputFile(const AnsiString &injobtype,const AnsiString &infilename)
{
	// set the output file for a job.
	// not all jobs have outputs, but those that do should only call this once(?)
	// idea is that a job sets this, which we then encode for transfer,
	// and iteratively deliver it in small chunks
	// NOTE: we expect the infilename to ALREADY be processed by EncodeFileForOutput

	// ste output string
	writefromfile=true;
	OutputFilename=infilename;
	OutputJobtype=injobtype;
	// reset current position flag and length
	curpos=0;
	outputlen=JRFileLength(infilename);
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MPGPJob::AddAssembleData(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int &startpos,int &thislen,int &totallength,AnsiString &contents)
{
	// try to add some data to an assembling job
	// returns true on success
	// returns false on failire
	FILE *filep;
	int contentslen=contents.Length();
	char blankstring[100];
	unsigned int filebytes;
	int thiswrite,thisread;
	int count;
	bool bretv;

	// make sure the job is still running
	if (IsStopped())
		{
		// we give a warning, but dont cancel job since it is already stopped
		jobmanager->LogWarning("Job "+GetPJobid()+" ignoring data received after having stopped.");
		return false;
		}

	// more data when we have got it all
	if (!IsAssembling())
		{
		// the job has already finished assembling; we give a warning, but dont cancel it
		jobmanager->LogWarning("Job "+GetPJobid()+" ignoring data received after having received it all.");
		return false;
		}

	// make sure size is not TOO BIG for security reasons
	if (!storeinfile && totallength>MaxSafeJobMemoryDataSize)
		{
		// error because it is too big for safe memory storage
		ErrorCancel("job data too big for safe memory storage");
		return false;
		}
	else if (storeinfile && maxfilesize==0)
		{
		// error because it is too big for safe file storage
		ErrorCancel("rejected unsolicited file job");
		return false;
		}
	else if (storeinfile && totallength>maxfilesize)
		{
		// error because it is too big for safe file storage
		ErrorCancel("job data too big for file storage");
		return false;
		}

	// first makse sure incoming data is not illegal
	if (startpos<0 || thislen<0 || totallength<0)
		{
		// illegal parameter
		ErrorCancel("job data parameter too small");
		return false;
		}
	if (startpos+thislen>totallength)
		{
		// this data is too big
		ErrorCancel("job data too big");
		return false;
		}
	if (totalsize!=-1 && totallength!=totalsize)
		{
		// contradictory total size
		ErrorCancel("contradictory size of job data");
		return false;
		}
	if (thislen<contents.Length())
		{
		// more data than we were told to expect
		ErrorCancel("incoming job data is bigger than asserted:"+AnsiString(contents.Length())+">"+AnsiString(thislen));
		return false;
		}
	if (thislen>contents.Length())
		{
		// less data than we were told to expect
		ErrorCancel("incoming job data is smaller than asserted: "+AnsiString(contents.Length())+"<"+AnsiString(thislen));
		return false;
		}

	// update timestamp
	UpdateTimestamp();

	// if this is first data coming in, then allocate it
	if (totalsize==-1)
		{
		// this is first time we are getting data, so allocate it and set it
		totalsize=totallength;
		if (totalsize>0)
			{
			// allocated it
			if (!storeinfile)
				{
				bufferp=new char [totalsize+1];
				if (bufferp==NULL)
					{
					// error allocating it
					ErrorCancel("error allocating memory for job data");
					return false;
					}
				// fill buffer with 0s
				memset(bufferp,0,totalsize+1);
				}
			else
				{
				// create a file and prewrite the blank space
				if (bufferfilename=="")
					{
					// create a new unique file
					bufferfilename=jobmanager->get_filesdir()+"\\"+GetPJobid()+"_tempreceivedfile.pgp";
					bufferfilename=JRUniquifyFilename(bufferfilename);
					if (bufferfilename=="")
						{
						ErrorCancel("error finding a temp file name to hold job data");
						return false;
						}
					}
				// create and fill in the file data with 0s
				for (count=0;count<100;++count)
					blankstring[count]='\0';
				filep=fopen(bufferfilename.c_str(),"wb");
				if (filep==NULL)
					{
					ErrorCancel("could not create temp file "+bufferfilename+" to hold job data");
					return false;
					}
				filebytes=totalsize;
				while (filebytes>0)
					{
					if (filebytes<100)
						thiswrite=filebytes;
					else
						thiswrite=100;
					fwrite(blankstring,thiswrite,1,filep);
					filebytes-=thiswrite;
					}
				fclose(filep);
				}
			}
		// set current incoming pos = 0
		incurpos=0;
		}

	// New - we check incoming curpos with current incurpos and if we are missing some data we ask for rewind
	if (incurpos<startpos)
		{
		// we are missing some data - so send a rewind
		jobmanager->LogMessage("asking to rewind to position "+AnsiString(incurpos)+" because incurpos = "+AnsiString(incurpos)+" and startpos = "+AnsiString(startpos));
		Rewind(incurpos);
		return true;
		}

	// write in the data into the buffer
	if (contentslen>0)
		{
		if (!storeinfile)
			{
			// store in memory
			memcpy(&(bufferp[startpos]),contents.c_str(),thislen);
			// recalculate receivedsize, by counting the non-zero bytes
			receivedsize=0;
			incurpos=0;
			for (count=0;count<totalsize;++count)
				{
				if (bufferp[count]!='\0')
					++receivedsize;
				else if (incurpos==0)
					incurpos=count;
				}
			}
		else
			{
			// store in file
			filep=fopen(bufferfilename.c_str(),"r+b");
			if (filep==NULL)
				{
				ErrorCancel("couldnt open file to update incoming job data");
				return false;
				}
			// make sure file is expected length
			int flen=filelength(fileno(filep));
			if (flen!=totalsize)
				{
				fclose(filep);
				ErrorCancel("file holding incoming job data has changed size");
				return false;
				}
			// set position
			fseek(filep,startpos,SEEK_SET);
			// write data
			fwrite(contents.c_str(),thislen,1,filep);
			// recalculate receivedsize, by counting the non-zero bytes
			receivedsize=0;
			fseek(filep,0,SEEK_SET);
			filebytes=totalsize;
			incurpos=0;
			int tcount=0;
			while (filebytes>0)
				{
				if (filebytes<100)
					thisread=filebytes;
				else
					thisread=100;
				fread(blankstring,thisread,1,filep);
				for (count=0;count<thisread;++count)
					{
					if (blankstring[count]!='\0')
						++receivedsize;
					else if (incurpos==0)
						incurpos=tcount+count;
					}
				filebytes-=thisread;
				tcount+=thisread;
				}
			fclose(filep);
			}
		}

	// check for odd error if overlapping incoming data
	if (receivedsize>totalsize)
		{
		ErrorCancel("received too much incoming job data");
		return false;
		}

	// update status
	if (totalsize>0)
		{
		if (startpos==0 && totalsize==thislen && stagestring=="")
			{
			// we got the whole job right away - no point in updating status.
			}
		else if (receivedsize==totalsize)
			set_stagestring("finished receiving "+AnsiString(totalsize)+" bytes");
		else
			set_stagestring("received "+AnsiString(receivedsize)+" of "+AnsiString(totalsize)+" bytes");
		}

	// do some brief error checking
	bretv=IncomingProgress();
	if (bretv==false)
		{
		// we have received an error about the (possibly partial) incoming data for this job
		// this can happen if our incoming job is supposed to be very short, etc.
		// we rely on the IncomingProgress() call to have triggered ErrorCancel() on this job
		return false;
		}

	// set assembling=true if all data is not in yet, or false if it is
	if (receivedsize==totalsize)
		{
		// this was the last piece of data we needed to fully assemble
		BecomeAssembled();
		}

	return true;
}

void MPGPJob::Rewind(int inrewindpos)
{
	// send a rewind request
	if (rewindpos==inrewindpos)
		{
		// we already rewound to here, so we dont want to send another request
		return;
		}
	rewindpos=inrewindpos;
	MPGPJob *jobp=new JobOJC_REWIND(tochan,tonick,toident,jobid,jobtype,rewindpos,JOBDIR_OUTGOING);
	if (jobp!=NULL)
		jobmanager->AddJobToEnd(jobp);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// input output
bool MPGPJob::RecursivelyAdopt(MPGPJob *injobp)
{
	// see if we or any children want to adopt this job (ie is it responding to us)
	// returns true if we or any of our children adopt it
	// returns false if we reject it
	MPGPJob *nchildp,*nextjobp;
	bool bretv=false;
	bool bretv2;

	// process for any children
	nchildp=childp;
	while (nchildp!=NULL)
		{
		nextjobp=nchildp->nextp;
		bretv2=nchildp->RecursivelyAdopt(injobp);
		if (bretv2)
			{
			bretv=true;
			break;
			}
		nchildp=nextjobp;
		}

	if (!bretv)
		{
		// children didnt adopt, now check this job iteself
		bretv2=Adopt(injobp);
		if (bretv2)
			bretv=true;
		}

	return bretv;
}

bool MPGPJob::DoesJobMatch(AnsiString jtype,AnsiString jid)
{
	if (jobtype==jtype && jobid==jid)
		return true;
	return false;
}

bool MPGPJob::Adopt(MPGPJob *injobp)
{
	// generic process
	bool bretv;
	// see if the dervied class wants to adopt it
	bretv=DerivedAdopt(injobp);
	return bretv;
}

bool MPGPJob::IncomingProgress()
{
	// we are an incoming job, which has just received some data
	// the only thing we do here is check for some error condition
	// returns false on error
	bool bretv=DerivedIncomingProgress();
	return bretv;
}

bool MPGPJob::IncomingCompletes()
{
	// we are an incoming job, which has just now received all of our incoming data
	// if we are a child of an existing job, we probably wouldnt do anything here
	// but if we are a standalone job, like a channel key, we would take action now
	bool bretv;

	// first thing, decode the received buffer into the instring. This could be done more efficiently and we could free bufferp afterwards
	inbufferstring=AnsiString(bufferp);
	//	jobmanager->LogMessage("BEFORE DECODE FROM STRING: "+inbufferstring);
	DecodeStringFromInput(inbufferstring);
	if (storeinfile)
		{
		bretv=DecodeFileFromInput(bufferfilename);
		if (!bretv)
			return false;
		}
	bretv=DerivedIncomingCompletes();
	return bretv;
}

bool MPGPJob::ProcessOutput()
{
	// generic process
	bool bretv;
	int thiswritelen;
	AnsiString outstr;
	AnsiString formattedoutstr;
	int flen;
	FILE *filep;

	// ATTN: test
	//	jobmanager->LogMessage("Job "+GetPJobid()+" takes time to output.");

	// is there buffered output waiting?
	if (outputlen-curpos>0)
		{
		// yes there is stuff to output - figure out how much
		if (outputlen-curpos>jobmanager->MaxOutLineLength)
			thiswritelen=jobmanager->MaxOutLineLength;
		else
			thiswritelen=outputlen-curpos;

		if (!writefromfile)
			{
			// get partial output section from a decoded string
			outstr=OutputString.SubString(curpos+1,thiswritelen);
			}
		else
			{
			// get a section from middle of file
			// allocate temp memory
			char *readbuf=new char[thiswritelen+1];
			if (readbuf==NULL)
				{
				ErrorCancel("failed to allocate memory for partial file write");
				return false;
				}
			filep=fopen(OutputFilename.c_str(),"rb");
			if (filep==NULL)
				{
				delete [] readbuf;
				ErrorCancel("failed to read temp file for sending");
				return false;
				}
			// get file length
			flen=filelength(fileno(filep));
			if (curpos+thiswritelen>flen || flen!=outputlen)
				{
				delete [] readbuf;
				ErrorCancel("file size mismatch in writing to output");
				return false;
				}
			// seek to start pos
			if (curpos>0)
				fseek(filep,curpos,SEEK_SET);
			// read the data
			fread(readbuf,1,thiswritelen,filep);
			readbuf[thiswritelen]='\0';
			// now convert it to string and encode it for transfer
			outstr=AnsiString(readbuf);
			// close the file
			fclose(filep);
			// free temp memory
			delete [] readbuf;
			}

		// format the output data to indicate which part of data we are sending
		formattedoutstr=AnsiString(JOBPREFIX)+" "+jobid+" "+OutputJobtype+" "+AnsiString(curpos)+" "+AnsiString(thiswritelen)+" "+AnsiString(outputlen)+" "+outstr;
		bretv=jobmanager->QueueOutputString(tochan,tonick,toident,formattedoutstr);
		if (!bretv)
			{
			// error writing output - should we cancel the job or suspend it?
			ErrorCancel("failed to queue output of job");
			return false;
			}
		// advance pointer
		curpos+=thiswritelen;
		// we can either force update ALWAYS, or just when finished
		if (curpos>=outputlen || true)
			jobmanager->SetJobsChanged();
		}

	// dervied process
	bretv=DerivedProcessOutput();
	return bretv;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MPGPJob::EncodeStringForOutput(AnsiString &str)
{
	// Encode output for transmission - there are two methods we could use - one
	//  preserves size but is only usable on ascii armoured text.
	// the other can be used on binary data but can increase size (double chars used in place of illegal chars)
	int count;
	bool asciiarm=jobmanager->get_PgpAsciiArmour() && false;

	if (asciiarm)
		{
		// convert whitespaces (32,8,9,10,13,14) to chars not fucked with by mirc, and not used by pgp
		int count;
		int len=str.Length();
		unsigned char c;
		for (count=1;count<=len;++count)
			{
			c=str[count];
			if (c==32)
				c=160;
			else if (c==8)
				c=161;
			else if (c==9)
				c=162;
			else if (c==10)
				c=163;
			else if (c==13)
				c=164;
			else if (c==14)
				c=165;
			else if (c==126)
				c=166;
			else
				continue;
			str[count]=c;
			}
		}
	else
		{
		// binary convert
		// init special binary conversions
		unsigned char binspecial[255];
		unsigned char rbinspecial[40];
		memset(binspecial,0,255);
		binspecial[0]=1; rbinspecial[1]=0;
		binspecial[2]=2; rbinspecial[2]=2;
		binspecial[3]=3; rbinspecial[3]=3;
		binspecial[15]=4; rbinspecial[4]=15;
		binspecial[22]=5; rbinspecial[5]=22;
		binspecial[31]=6; rbinspecial[6]=31;
		binspecial[32]=7; rbinspecial[7]=32;
		binspecial[33]=8; rbinspecial[8]=33;
		binspecial[36]=9; rbinspecial[9]=36;
		binspecial[127]=10; rbinspecial[10]=127;
		binspecial[27]=11; rbinspecial[11]=27;
		binspecial[40]=12; rbinspecial[12]=40;
		binspecial[41]=13; rbinspecial[13]=41;
		binspecial[44]=14; rbinspecial[14]=44;
		binspecial[34]=15; rbinspecial[15]=34;
		binspecial[47]=16; rbinspecial[16]=47;
		binspecial[37]=17; rbinspecial[17]=37;
		binspecial[48]=18; rbinspecial[18]=48;
		binspecial[13]=19; rbinspecial[19]=13;
		binspecial[10]=20; rbinspecial[20]=10;
		binspecial[9]=21; rbinspecial[21]=9;
		binspecial[12]=22; rbinspecial[22]=12;
		binspecial[245]=23; rbinspecial[23]=245;
		binspecial[246]=24; rbinspecial[24]=246;
		binspecial[247]=25; rbinspecial[25]=247;
		binspecial[248]=26; rbinspecial[26]=248;
		binspecial[249]=27; rbinspecial[27]=249;
		binspecial[250]=28; rbinspecial[28]=250;
		binspecial[251]=29; rbinspecial[29]=251;
		binspecial[252]=30; rbinspecial[30]=252;
		binspecial[253]=31; rbinspecial[31]=253;
		binspecial[254]=32; rbinspecial[32]=254;
		binspecial[255]=33; rbinspecial[33]=255;

		int len = str.Length();
		int index=0;
		unsigned char c,sc;
		unsigned char *charp = new char [(len*2)+1];
		for (count=1;count<=len;++count)
			{
			c=str[count];
			sc=binspecial[c];
			if (sc!=0)
				{
				charp[index++]=48;
				charp[index++]=64+sc;
				}
			else
				charp[index++]=c;
			}
		charp[index]='\0';
		str=AnsiString((char *)charp,index);
		delete [] charp;
		}
}

void MPGPJob::DecodeStringFromInput(AnsiString &str)
{
	// uncovert special characters
	int count;
	bool asciiarm=jobmanager->get_PgpAsciiArmour() && false;

	if (asciiarm)
		{
		int len=str.Length();
		unsigned char c;
		for (count=1;count<=len;++count)
			{
			c=str[count];
			if (c==160)
				c=32;
			else if (c==161)
				c=8;
			else if (c==162)
				c=9;
			else if (c==163)
				c=10;
			else if (c==164)
				c=13;
			else if (c==165)
				c=14;
			else if (c==166)
				c=126;
			else
				continue;
			str[count]=c;
			}
		}
	else
		{
		// binary convert
		// init special binary conversions
		unsigned char binspecial[255];
		unsigned char rbinspecial[40];
		memset(binspecial,0,255);
		binspecial[0]=1; rbinspecial[1]=0;
		binspecial[2]=2; rbinspecial[2]=2;
		binspecial[3]=3; rbinspecial[3]=3;
		binspecial[15]=4; rbinspecial[4]=15;
		binspecial[22]=5; rbinspecial[5]=22;
		binspecial[31]=6; rbinspecial[6]=31;
		binspecial[32]=7; rbinspecial[7]=32;
		binspecial[33]=8; rbinspecial[8]=33;
		binspecial[36]=9; rbinspecial[9]=36;
		binspecial[127]=10; rbinspecial[10]=127;
		binspecial[27]=11; rbinspecial[11]=27;
		binspecial[40]=12; rbinspecial[12]=40;
		binspecial[41]=13; rbinspecial[13]=41;
		binspecial[44]=14; rbinspecial[14]=44;
		binspecial[34]=15; rbinspecial[15]=34;
		binspecial[47]=16; rbinspecial[16]=47;
		binspecial[37]=17; rbinspecial[17]=37;
		binspecial[48]=18; rbinspecial[18]=48;
		binspecial[13]=19; rbinspecial[19]=13;
		binspecial[10]=20; rbinspecial[20]=10;
		binspecial[9]=21; rbinspecial[21]=9;
		binspecial[12]=22; rbinspecial[22]=12;
		binspecial[245]=23; rbinspecial[23]=245;
		binspecial[246]=24; rbinspecial[24]=246;
		binspecial[247]=25; rbinspecial[25]=247;
		binspecial[248]=26; rbinspecial[26]=248;
		binspecial[249]=27; rbinspecial[27]=249;
		binspecial[250]=28; rbinspecial[28]=250;
		binspecial[251]=29; rbinspecial[29]=251;
		binspecial[252]=30; rbinspecial[30]=252;
		binspecial[253]=31; rbinspecial[31]=253;
		binspecial[254]=32; rbinspecial[32]=254;
		binspecial[255]=33; rbinspecial[33]=255;

		int len = str.Length();
		int index=0;
		unsigned char c;
		unsigned char rc;
		unsigned char *charp = new unsigned char [len+1];
		for (count=1;count<=len;++count)
			{
			c=str[count];
			if (c==48)
				{
				c=str[++count];
				rc=c-64;
				if (rc<40)
					rc=rbinspecial[rc];
				charp[index++]=rc;
				}
			else
				charp[index++]=c;
			}
		charp[index]='\0';
		str=AnsiString((char *)charp,index);
		delete [] charp;
		}
}

bool MPGPJob::EncodeFileForOutput(AnsiString &filename)
{
	// convert whitespaces (32,9,13,10) to chars not fucked with by mirc, and not used by pgp
	// delete old file and and change str to new filename
	// ONLY CALL THIS ON TEMP FILES - IT WILL PERMANENTLY ERASE THE ORIGINAL!
	// this is pretty inefficient, but wtf, we dont use this very often, and only for small files, and speed is not critical
	FILE *filep,*fileoutp;
	int flen;
	int count;
	unsigned char c,sc;
	bool asciiarm=jobmanager->get_PgpAsciiArmour() && false;
	AnsiString outfilename;
	char cc[2];

	// init special binary conversions
		unsigned char binspecial[255];
		unsigned char rbinspecial[40];
		memset(binspecial,0,255);
		binspecial[0]=1; rbinspecial[1]=0;
		binspecial[2]=2; rbinspecial[2]=2;
		binspecial[3]=3; rbinspecial[3]=3;
		binspecial[15]=4; rbinspecial[4]=15;
		binspecial[22]=5; rbinspecial[5]=22;
		binspecial[31]=6; rbinspecial[6]=31;
		binspecial[32]=7; rbinspecial[7]=32;
		binspecial[33]=8; rbinspecial[8]=33;
		binspecial[36]=9; rbinspecial[9]=36;
		binspecial[127]=10; rbinspecial[10]=127;
		binspecial[27]=11; rbinspecial[11]=27;
		binspecial[40]=12; rbinspecial[12]=40;
		binspecial[41]=13; rbinspecial[13]=41;
		binspecial[44]=14; rbinspecial[14]=44;
		binspecial[34]=15; rbinspecial[15]=34;
		binspecial[47]=16; rbinspecial[16]=47;
		binspecial[37]=17; rbinspecial[17]=37;
		binspecial[48]=18; rbinspecial[18]=48;
		binspecial[13]=19; rbinspecial[19]=13;
		binspecial[10]=20; rbinspecial[20]=10;
		binspecial[9]=21; rbinspecial[21]=9;
		binspecial[12]=22; rbinspecial[22]=12;
		binspecial[245]=23; rbinspecial[23]=245;
		binspecial[246]=24; rbinspecial[24]=246;
		binspecial[247]=25; rbinspecial[25]=247;
		binspecial[248]=26; rbinspecial[26]=248;
		binspecial[249]=27; rbinspecial[27]=249;
		binspecial[250]=28; rbinspecial[28]=250;
		binspecial[251]=29; rbinspecial[29]=251;
		binspecial[252]=30; rbinspecial[30]=252;
		binspecial[253]=31; rbinspecial[31]=253;
		binspecial[254]=32; rbinspecial[32]=254;
		binspecial[255]=33; rbinspecial[33]=255;

	// init - control character for double length chars
	cc[0]=48;

	// open files
	filep=fopen(filename.c_str(),"rb");
	if (filep==NULL)
		return false;
	outfilename=JRUniquifyFilename(filename);
	fileoutp=fopen(outfilename.c_str(),"wb");
	if (fileoutp==NULL)
		{
		fclose(filep);
		return false;
		}
	flen=filelength(fileno(filep));
	// write from input to output
	for (count=0;count<flen;++count)
		{
		fread(&c,1,1,filep);
		if (asciiarm)
			{
			// ascii mode
			if (c==32)
				c=160;
			else if (c==8)
				c=161;
			else if (c==9)
				c=162;
			else if (c==10)
				c=163;
			else if (c==13)
				c=164;
			else if (c==14)
				c=165;
			else if (c==126)
				c=166;
			fwrite(&c,1,1,fileoutp);
			}
		else
			{
			// binary
			sc=binspecial[c];
			if (sc!=0)
				{
				cc[1]=64+sc;
				fwrite(&cc,2,1,fileoutp);
				}
			else
				fwrite(&c,1,1,fileoutp);
			}
		}
	// close files
	fclose(filep);
	fclose(fileoutp);

	// now delete original and rename new to old
	JRSafeDeleteFile(filename);
	// rename it
	//	RenameFile(outfilename,filename);
	// or alternatively, dont rename, but change filename instead
	filename=outfilename;

	return true;
}

bool MPGPJob::DecodeFileFromInput(AnsiString &filename)
{
	// uncovert special characters
	// delete old file and and change str to new filename
	// ONLY CALL THIS ON TEMP FILES - IT WILL PERMANENTLY ERASE THE ORIGINAL!
	// this is pretty inefficient, but wtf, we dont use this very often, and only for small files, and speed is not critical
	FILE *filep,*fileoutp;
	int flen;
	int count;
	unsigned char c,rc;
	bool asciiarm=jobmanager->get_PgpAsciiArmour() && false;
	AnsiString outfilename;

	// init special binary conversions
		unsigned char binspecial[255];
		unsigned char rbinspecial[40];
		memset(binspecial,0,255);
		binspecial[0]=1; rbinspecial[1]=0;
		binspecial[2]=2; rbinspecial[2]=2;
		binspecial[3]=3; rbinspecial[3]=3;
		binspecial[15]=4; rbinspecial[4]=15;
		binspecial[22]=5; rbinspecial[5]=22;
		binspecial[31]=6; rbinspecial[6]=31;
		binspecial[32]=7; rbinspecial[7]=32;
		binspecial[33]=8; rbinspecial[8]=33;
		binspecial[36]=9; rbinspecial[9]=36;
		binspecial[127]=10; rbinspecial[10]=127;
		binspecial[27]=11; rbinspecial[11]=27;
		binspecial[40]=12; rbinspecial[12]=40;
		binspecial[41]=13; rbinspecial[13]=41;
		binspecial[44]=14; rbinspecial[14]=44;
		binspecial[34]=15; rbinspecial[15]=34;
		binspecial[47]=16; rbinspecial[16]=47;
		binspecial[37]=17; rbinspecial[17]=37;
		binspecial[48]=18; rbinspecial[18]=48;
		binspecial[13]=19; rbinspecial[19]=13;
		binspecial[10]=20; rbinspecial[20]=10;
		binspecial[9]=21; rbinspecial[21]=9;
		binspecial[12]=22; rbinspecial[22]=12;
		binspecial[245]=23; rbinspecial[23]=245;
		binspecial[246]=24; rbinspecial[24]=246;
		binspecial[247]=25; rbinspecial[25]=247;
		binspecial[248]=26; rbinspecial[26]=248;
		binspecial[249]=27; rbinspecial[27]=249;
		binspecial[250]=28; rbinspecial[28]=250;
		binspecial[251]=29; rbinspecial[29]=251;
		binspecial[252]=30; rbinspecial[30]=252;
		binspecial[253]=31; rbinspecial[31]=253;
		binspecial[254]=32; rbinspecial[32]=254;
		binspecial[255]=33; rbinspecial[33]=255;

	// open files
	filep=fopen(filename.c_str(),"rb");
	if (filep==NULL)
		return false;
	outfilename=JRUniquifyFilename(filename);
	fileoutp=fopen(outfilename.c_str(),"wb");
	if (fileoutp==NULL)
		{
		fclose(filep);
		return false;
		}
	flen=filelength(fileno(filep));
	// write from input to output
	for (count=0;count<flen;++count)
		{
		// ascii armoured
		fread(&c,1,1,filep);
		if (asciiarm)
			{
			// ascii mode
			if (c==160)
				c=32;
			else if (c==161)
				c=8;
			else if (c==162)
				c=9;
			else if (c==163)
				c=10;
			else if (c==164)
				c=13;
			else if (c==165)
				c=14;
			else if (c==166)
				c=126;
			fwrite(&c,1,1,fileoutp);
			}
		else
			{
			// binary
			if (c==48)
				{
				fread(&c,1,1,filep);
				rc=c-64;
				if (c>=64 && rc<40)
					c=rbinspecial[rc];
				++count;
				}
			fwrite(&c,1,1,fileoutp);
			}
		}
	// close files
	fclose(filep);
	fclose(fileoutp);

	// now delete original and rename new to old
	JRSafeDeleteFile(filename);
	// rename it
	//	RenameFile(outfilename,filename);
	// or alternatively, dont rename, but change filename instead
	filename=outfilename;

	return true;
}
//---------------------------------------------------------------------------



void MPGPJob::SendAnAck(const AnsiString &creason,int outdirection,AnsiString ackjobtype)
{
	// create and add to job queue a message to send to tonick to ascknowledge a job of theirs
	MPGPJob *jobp;

        // ATTN: need to make this a proper cancel job
	// make the cancel request
	jobp=new JobOJC_ACK(tochan,tonick,toident,jobid,ackjobtype,outdirection,creason);
	if (outdirection==JOBDIR_INCOMING)
		jobmanager->LogMessage("Sending "+tonick+" an acknowledgement for job "+jobid+"i: "+creason);
	else
		jobmanager->LogMessage("Sending "+tonick+" an acknowledgement for job "+jobid+": "+creason);
	// queue it up
	jobmanager->AddJobToEnd(jobp);
}

