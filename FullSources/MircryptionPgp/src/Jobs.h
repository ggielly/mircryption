// Jobs.h
//---------------------------------------------------------------------------
#ifndef MPGPJobsH
#define MPGPJobsH
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Disable useless message about compiler refusing to inline certain functions
#pragma warn -8026
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern bool nowquitting;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// This class is for queuing up incoming messages
//  Exclusively used for debugging via self-feedback, which makes it look
//  like what you output comes back to you.
class MessageQueue
{
friend class MPGPJobManager;
protected:
	AnsiString tochan,tonick,toident;
	AnsiString string;
	int trycount;
protected:
	MessageQueue *priorp,*nextp;
public:
	MessageQueue(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString instring);
	~MessageQueue() {;};
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// for managing/scheduling jobs
class MPGPJob
{
friend class MPGPJobManager;
protected:
	MPGPJob *priorp;
	MPGPJob *nextp;
	MPGPJob *childp;
	MPGPJob *parentp;
	MPGPJob *dependee;
protected:
	// identification
	AnsiString jobid;
	AnsiString tochan,tonick,toident;
	AnsiString FromId,ToId;
	AnsiString description;
	AnsiString jobtype;
	TDateTime updatetime;
protected:
	AnsiString contents;
protected:
	// main states
	bool suspended;
	bool canceled;
	bool accomplished;
	bool assembling;
	// auxiliary additional states
	bool expired;
	bool errored;
	bool confirmed;
	// stage
	AnsiString stagestring;
	int stagenum;
	// direction - is it an incoming or outgoing job
	int direction;
protected:
	// input and output buffers
	bool storeinfile;
	int maxfilesize;
	AnsiString bufferfilename;
	AnsiString inbufferstring;
	char *bufferp;
	int totalsize;
	int receivedsize;
	int incurpos;
	int rewindpos;
protected:
	bool writefromfile;
	AnsiString OutputFilename;
	AnsiString OutputString;
	AnsiString OutputJobtype;
	int curpos;
	int outputlen;
public:
	MPGPJob(AnsiString jid,AnsiString jt,AnsiString desc, AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts, int jobdir);
	// imp:
	virtual ~MPGPJob();
public:
	void AddChild(MPGPJob *cp);
public:
	// state setters
	bool ErrorCancel(AnsiString cancelreasonstr) {errored=true;return Cancel(cancelreasonstr);};
	bool Cancel(AnsiString cancelreasonstr);
	bool Suspend();
	bool Resume();
	void BecomeAssembled();
	void BecomeAccomplished();
	void BecomeConfirmed(AnsiString confirmparam);
	void BecomeExpired();
	// primitive state getters
	bool IsSuspended();
	bool IsDependent();
	bool IsAssembling();
	bool IsAccomplished();
	bool IsConfirmed();
	bool IsOutputting();
	bool IsExpired();
	// higher level state getters
	bool IsReadyToIterate();
	bool IsStopped();
	bool IsBlocked();
	bool IsCanceled();
public:
	void UpdateTimestamp();
	MPGPJob *get_parentp() {return parentp;};
	AnsiString get_jobid() {return jobid;};
	AnsiString get_jobtype() {return jobtype;};
	int get_direction() {return direction;};
	AnsiString get_stagestring() {return stagestring;};
	void set_ToId(AnsiString str) {ToId=str;};
	void set_maxfilesize(int size) {maxfilesize=size;};
	int get_outputlen() {return outputlen;};
	void set_curpos(int val) {curpos=val;};
	//
	AnsiString get_inbufferstring() {return inbufferstring;};
	void set_stagestring(AnsiString str);
	void set_descstring(AnsiString str);
	AnsiString GetPJobid();
	static AnsiString GetPJobid(AnsiString jobstr,int direction);
	AnsiString GetJobDesc();
public:
	static AnsiString PrettyId(AnsiString nick,AnsiString keydesc,bool showanon);
	static AnsiString PrettySig(AnsiString nick,AnsiString keydesc,bool showanon);
	static AnsiString PrettyShort(AnsiString instr,int maxlen);
	static AnsiString StripKeymail(AnsiString keydesc);
public:
	bool SetOutputString(const AnsiString &injobtype,const AnsiString &instring);
	bool SetOutputFile(const AnsiString &injobtype,const AnsiString &infilename);
public:
	bool AddAssembleData(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int &startpos,int &thislen,int &totallength,AnsiString &contents);
	void Rewind(int inrewindpos);
public:
	bool Iterate();
public:
	// input output
	bool RecursivelyAdopt(MPGPJob *injobp);
	bool Adopt(MPGPJob *injobp);
	bool DoesJobMatch(AnsiString jtype,AnsiString jid);
	//
	bool IncomingProgress();
	bool IncomingCompletes();
	bool ProcessOutput();
public:
	static void EncodeStringForOutput(AnsiString &str);
	static void DecodeStringFromInput(AnsiString &str);
	bool EncodeFileForOutput(AnsiString &str);
	bool DecodeFileFromInput(AnsiString &str);
public:
	void SendAnAck(const AnsiString &creason,int outdirection,AnsiString ackjobtype);
protected:
	// virtual classes
	virtual void DerivedCancel() {;};
	virtual void DerivedSuspend() {;};
	virtual void DerivedResume() {;};
	virtual void DerivedStopped() {;};
	virtual void DerivedAccomplished() {;};
	virtual void DerivedConfirmed(AnsiString confirmparam) {;};
	virtual bool DerivedIterate() {return true;};
	virtual bool DerivedAdopt(MPGPJob *injobp) {return false;};
	virtual bool DerivedIncomingProgress() {return true;};
	virtual bool DerivedIncomingCompletes() {return true;};
	virtual bool DerivedProcessOutput() {return true;};
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
