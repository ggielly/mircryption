// JobManager.h
//---------------------------------------------------------------------------
#ifndef MPGPJobManagerH
#define MPGPJobManagerH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>

#include "Jobs.h"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#define AnonymousMircryptionKeyId		"Anonymous (mircryption)"
#define AnonymousMircryptionKeyIdPrefix		"_mcps_"
#define RandomKeyphraseLength			56
#define mcpsPrefix				"mcps"
#define RandomProofStringLength			25
#define MinChanKeyLengthWarning			15
#define DEFPgpAsciiArmour			true
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//#define QuestionFormTimeoutYesNo		45
//#define QuestionFormTimeoutOk			30
#define AlwaysMircResult			true
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define MinGoodMircLineLength  			250
#define MaxMircLineLength     			350
#define MaxSafeJobMemoryDataSize		10000
#define MaxSafeJobFileDataSize			10000000
#define MaxOutputLineTries			1
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// From radio box on main dialog
#define JOBRADIO_PROOF		0
#define JOBRADIO_CHANKEY	1
#define JOBRADIO_PUBLICKEY	2
#define JOBRADIO_MESSAGE	3
#define JOBRADIO_FILE	 	4
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// How to treat anonymouse request options (from optionsform itemindex
#define TREATANON_SILENTREJECT	0
#define TREATANON_REJECT	1
#define TREATANON_HEXASK	2
#define TREATANON_PUBASK	3
#define TREATANON_ALWAYS	4
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// JOB IDS
// main jobs (correspond to the kinds of jobs you can create)
#define SE_PROOF		"SE.PROOF"
#define SE_CHAN			"SE.CHAN"
#define SE_PUBKEY		"SE.PUBK"
#define SE_MESG			"SE.MESG"
#define SE_FILE			"SE.FILE"
// requests, send as part of larger job
#define RQ_ANONHEXID		"RQ.ANONHEXID"
#define RQ_ANONPUBKEY		"RQ.ANONPUBKEY"
#define RQ_PROOFRAND		"RQ.PROOFRAND"
#define RQ_FILEPERM		"RQ.FILEPERM"
// replies created in respoonse to a request
#define RP_ANONHEXID		"RP.ANONHEXID"
#define RP_ANONPUBKEY		"RP.ANONPUBKEY"
#define RP_PROOFRAND		"RP.PROOFRAND"
#define RP_FILEPERM		"RP.FILEPERM"
// control signals
#define JC_KILL			"JC.KILL"
#define JC_ACK			"JC.ACK"
#define JC_REWIND		"JC.REWIND"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Job direction (incoming or outgoing)
#define JOBDIR_INCOMING		1
#define JOBDIR_OUTGOING		2
#define JOBDIR_UNDEFINED	3
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Prefixes
#define JOBPREFIX		"J1"
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
class SelfThreader : public TThread
{
	public:
		SelfThreader();
	public:
		void __fastcall Execute();
		void __fastcall SelfRunThreadIterates(void);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
class MPGPJobManager
{
friend class MPGPJob;
private:
	bool Paused;
private:
	MessageQueue *QIfirstp,*QIlastp;
	MessageQueue *QOfirstp,*QOlastp;
	MPGPJob *Jfirstp,*Jlastp;
private:
	bool PgpAsciiArmour;
	AnsiString GlobalCodedOutput;
	AnsiString GlobalHexId;
	AnsiString GlobalHexIds;
	AnsiString GlobalKeynames;
	AnsiString GlobalFilename;
	AnsiString GlobalSig;
	AnsiString GlobalSigValidity;
	//
	AnsiString ChannelKeyphrase;
	AnsiString Global_tochan,Global_tonick,Global_toident;
	AnsiString Global_fromident;
	//
	AnsiString pgp_InstallPath;
	AnsiString pgp_InstallVersion;
	AnsiString pgp_KeyManagerExefile;
	bool pgp_KeyringsNeedRestore;
	AnsiString pgp_OrigPubring,pgp_OrigSecring;
	//
	bool initialized;
	bool JobsHaveChanged;
	AnsiString filesdir;
	AnsiString configfilename;
	int JobCounter;
	//
	int MaxOutLineLength;
	int TotalAllowedOutBytes;
	int AllowedOutBytes;
	int LeftOverWriteBytes;
	//
	bool HasWorkToDo;
	bool wantsquit;
	//
	SelfThreader *selfthreaderp;
	AnsiString basedir;
	bool ScheduledJobIterate;
	bool BlockedForInput;
	bool DidInitialEnsureAnonymous;
public:
	MPGPJobManager();
	~MPGPJobManager();
	bool Initialize();
	void DeInitialize();
	void PresentForm(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString ichankeyphrase,AnsiString ifromident);
	void RunTest();
	void SelfRunThreadStart();
	void SelfRunThreadEnd();
public:
	bool LoadIniSettings();
	bool SaveIniSettings();
public:
	// PGP interface
	bool InitPgpTools();
	void SetAltPgpKeyrings();
	//
	void MakeRandomChanKey(bool forcenew);
	AnsiString MakeRandomProofString();
	void MakeNewAnonymousKey();
	void LaunchKeyManager();
	void TempChangePgpKeyringLocs();
	void RestorePgpKeyringLocs();
	bool EnsureAnonymousKeyExists();
	void Initial_EnsureAnonymousKeyExists();
	//
	AnsiString GetPgpHexId(AnsiString keyname);
	AnsiString GetPgpOurAnonHexId();
	AnsiString GetPgpOurAnonPubKey();
	AnsiString GetPgpKeyData(AnsiString keydesc);
	bool ImportKeyData(AnsiString keydata,AnsiString &outstring,AnsiString dialogstr);
	bool ImportAnonKeyData(AnsiString keydata,AnsiString &hexid);
	bool IsPgpKeyAnonymous(AnsiString keydesc);
	bool PgpGoodEnoughValidity(AnsiString validstr);
	AnsiString PrettySigNick(AnsiString nick, bool showanon);
	void SetPgpWindowHandle(THandle parenthandle);
	void PgpFileOperationError();
public:
	bool EncryptString(AnsiString FromId,AnsiString ToId,AnsiString instring,AnsiString &outstring,AnsiString passprompt);
	bool EncryptFile(AnsiString FromId,AnsiString ToId,AnsiString infilename,AnsiString &outfilename,AnsiString passprompt);
	bool DecryptString(AnsiString FromId,AnsiString ToId,AnsiString instring,AnsiString &outstring,AnsiString passprompt,bool addkeys);
	bool DecryptFile(AnsiString FromId,AnsiString ToId,AnsiString infilename,AnsiString &outfilename,AnsiString passprompt,bool addkeys);
public:
	// job management1a
	void SuspendJob(int index) {SuspendJob(LookupJob(index));};
	void ResumeJob(int index) {ResumeJob(LookupJob(index));};
	void CancelJob(int index,AnsiString cancelreasonstr) {CancelJob(LookupJob(index),cancelreasonstr);};
	bool IsJobSuspended(int index) {return IsJobSuspended(LookupJob(index));};
	bool IsJobStopped(int index) {return IsJobStopped(LookupJob(index));};
	bool IsJobOutputting(int index) {return IsJobOutputting(LookupJob(index));};
	bool IsJobCanceled(int index) {return IsJobCanceled(LookupJob(index));};
	bool IsJobConfirmed(int index) {return IsJobConfirmed(LookupJob(index));};
	void SetJobsChanged() {JobsHaveChanged=true;};
	// job management1b
	void SuspendJob(MPGPJob *jobp);
	void ResumeJob(MPGPJob *jobp);
	void CancelJob(MPGPJob *jobp,AnsiString cancelreasonstr);
	bool IsJobSuspended(MPGPJob *jobp);
	bool IsJobStopped(MPGPJob *jobp);
	bool IsJobOutputting(MPGPJob *jobp);
	bool IsJobConfirmed(MPGPJob *jobp);
	bool IsJobCanceled(MPGPJob *jobp);
	// job management helpers
	MPGPJob *LookupJob(int index);
	MPGPJob *LookUpMatchingJob(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype, int jobdir);
	// job management2
	void CancelAllJobs();
	void SuspendAllJobs();
	void PurgeAllFinishedJobs();
	void RecursivelyDeleteJob(MPGPJob *jobp);
	void SleepIfNoJobs();
	// job managerment3
	void FillJobListBox(TListBox *boxp, bool forceupdate=false);
public:
	// making jobs
	bool MakeAndQueueJobFromDialog();
	void AddJobToEnd(MPGPJob *jobp);
	AnsiString CreateUniqueJobId();
public:
	// input queue
	void DeleteInputQueue(MessageQueue *qp);
	void ClearInputQueue();
	void AddInputQueueToEnd(MessageQueue *qp);
	bool IterateInputQueue();
	// output queue
	void DeleteOutputQueue(MessageQueue *qp);
	void ClearOutputQueue();
	void AddOutputQueueToEnd(MessageQueue *qp);
	bool IterateOutputQueue();
public:
	// processing
	bool TimerTicks(int interval);
	bool Iterate(int maxoutbytes);
	bool IterateInput();
	bool IterateJobs();
	void CheckForExpiredJobs();
	bool IterateOutput();
	bool DoesHaveWorkToDo() {return HasWorkToDo;};
	int CountJobsPending();
	int CountJobRemainingBytes();
public:
	// simple accessors
	void set_wantsquit() {wantsquit=true;};
	bool get_ScheduledJobIterate() {return ScheduledJobIterate;}
	void set_ScheduledJobIterate(bool bval) {ScheduledJobIterate=bval;}
	bool get_BlockedForInput() {return BlockedForInput;}
	void set_BlockedForInput(bool bval);
	AnsiString get_basedir() {return basedir;};
	bool get_wantsquit() {return wantsquit;};
	void set_AllowedOutBytes(int val);
	bool get_Paused() {return Paused;};
	void set_Paused(bool bval) {Paused=bval;};
	AnsiString GetCurrentChannelKeyphrase() {return ChannelKeyphrase;};
	void set_GlobalFilename(AnsiString str) {GlobalFilename=str;};
	void set_GlobalCodedOutput(AnsiString str) {GlobalCodedOutput=str;};
	void set_GlobalHexId(const AnsiString &str) {GlobalHexId=str;};
	void set_GlobalHexIds(const AnsiString &str) {GlobalHexIds=str;};
	void set_GlobalKeynames(const AnsiString &str) {GlobalKeynames=str;};
	void set_GlobalSig(AnsiString str) {GlobalSig=str;};
	void set_GlobalSigValidity(int num) {GlobalSigValidity=GetValidityString(num);};
	bool get_PgpAsciiArmour() {return PgpAsciiArmour;};
	AnsiString get_DefaultKeyToolsExe() {return pgp_KeyManagerExefile;};
	AnsiString get_GlobalHexId() {return GlobalHexId;};
	AnsiString get_GlobalKeynames() {return GlobalKeynames;};
	AnsiString get_GlobalSig() {return GlobalSig;};
	AnsiString get_GlobalSigValidity() {return GlobalSigValidity;};
	AnsiString get_filesdir() {return filesdir;};
	void set_fromident(AnsiString fromident) {Global_fromident=fromident;};
	AnsiString get_fromident() {return Global_fromident;};
	AnsiString get_ChannelKeyphrase() {return ChannelKeyphrase;};
	//
	int QuestionTimeout();
	int MessageTimeout(); 
public:
	// helpers
	AnsiString GetValidityString(int validlevel);
	// debug helpers
	void LogWarning(AnsiString str);
	void LogMessage(AnsiString str);
	void LogError(AnsiString str);
	void LogDebug(AnsiString str);
	void LogInput(AnsiString str);
	void LogOutput(AnsiString str);
	//
	bool MircMessage(AnsiString chan,AnsiString str);
	bool SetMircChannelkey(AnsiString chan,AnsiString key);
	void JMShowMessage(AnsiString mesg);
public:
	// input and output
	bool QueueOutputString(AnsiString tochan,AnsiString tonick,AnsiString toident,AnsiString instring);
	bool QueueInputString(AnsiString tochan,AnsiString tonick,AnsiString toident,AnsiString instring);
	bool QueueInputDebugString(AnsiString instring);
	bool ProcessInputString(AnsiString tochan,AnsiString tonick,AnsiString toident,AnsiString str);
	bool ProcessOutputString(AnsiString tochan,AnsiString tonick,AnsiString toident,AnsiString str);
public:
	// heavy input parsing
	void IncomingJobParseError(AnsiString mesg);
	bool ProcessInputJobData(AnsiString jobkeyword,AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int &startpos,int &thislen,int &totallength,AnsiString &contents);
public:
	bool AdoptNewJob(MPGPJob *newjobp);
	void SendACancel(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int direction,const AnsiString &creason);
	MPGPJob *CreateNewIncomingJob(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype);
public:
	MPGPJob *get_Jfirstp() {return Jfirstp;};
	MPGPJob *get_Jlastp() {return Jlastp;};
	void set_Jfirstp(MPGPJob *val) {Jfirstp=val;};
	void set_Jlastp(MPGPJob *val) {Jlastp=val;};
};



//---------------------------------------------------------------------------
// Global jobmanager
extern MPGPJobManager *jobmanager;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
