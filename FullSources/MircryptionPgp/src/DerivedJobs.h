// DerivedJobs.h
// classes derived from MPGPJob
//---------------------------------------------------------------------------
#ifndef MPGPDerivedJobsH
#define MPGPDerivedJobsH
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// forware declarations
class JOBIRP_ANONHEXID;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Outgoing requests
//---------------------------------------------------------------------------
class JobORQ_ANONPUBKEY : public MPGPJob
{
public:
	JobORQ_ANONPUBKEY(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid);
	~JobORQ_ANONPUBKEY() {;};
public:
	virtual bool DerivedIterate() {return true;};
};

class JobORQ_ANONHEXID : public MPGPJob
{
public:
	JobORQ_ANONHEXID(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid);
	~JobORQ_ANONHEXID() {;};
protected:
	bool sentrequest;
public:
	virtual bool DerivedIterate();
	virtual bool DerivedAdopt(MPGPJob *injobp);
public:
	void NeedsFullPublicKey();
};

class JobORQ_PROOFRAND : public MPGPJob
{
public:
	JobORQ_PROOFRAND(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid);
	~JobORQ_PROOFRAND() {;};
protected:
	bool sentrequest;
public:
	virtual bool DerivedIterate();
	virtual bool DerivedAdopt(MPGPJob *injobp);
};

class JobORQ_FILEPERM : public MPGPJob
{
public:
	JobORQ_FILEPERM(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString filestring);
	~JobORQ_FILEPERM() {;};
protected:
	bool sentrequest;
	int filecount;
	int filesize;
	AnsiString filelist;
public:
	virtual bool DerivedIterate();
	virtual bool DerivedAdopt(MPGPJob *injobp);
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// Incoming requests
//---------------------------------------------------------------------------
class JobIRQ_ANONPUBKEY : public MPGPJob
{
public:
	JobIRQ_ANONPUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRQ_ANONPUBKEY() {;};
public:
	bool sentreply;
public:
	virtual bool DerivedIterate();
};

class JobIRQ_ANONHEXID : public MPGPJob
{
public:
	JobIRQ_ANONHEXID(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRQ_ANONHEXID() {;};
public:
	bool sentreply;
public:
	virtual bool DerivedIterate();
};

class JobIRQ_PROOFRAND : public MPGPJob
{
public:
	JobIRQ_PROOFRAND(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRQ_PROOFRAND() {;};
public:
	bool sentreply;
	AnsiString randstring;
	AnsiString proofjobid;
public:
	AnsiString get_randstring() {return randstring;};
public:
	virtual bool DerivedIncomingCompletes();
	virtual bool DerivedIterate();
	virtual bool DerivedAdopt(MPGPJob *injobp);
};

class JobIRQ_FILEPERM : public MPGPJob
{
public:
	JobIRQ_FILEPERM(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRQ_FILEPERM() {;};
protected:
	int filecount,filesize;
	bool willacceptfiles;
	AnsiString filename;
	AnsiString filesendjobid;
public:
	virtual bool DerivedIncomingCompletes();
	virtual bool DerivedIterate() {return true;};
	virtual bool DerivedAdopt(MPGPJob *injobp);
};
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
// Outgoing main jobs
class JobOSE_PROOF : public MPGPJob
{
public:
	JobOSE_PROOF(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts);
	~JobOSE_PROOF() {;};
protected:
	JobORQ_ANONHEXID *job_getanonhexid;
	JobORQ_PROOFRAND *job_getproofrand;
	AnsiString randproofstring;
public:
	void ReceiveRandString(AnsiString str) {randproofstring=str;};
public:
	virtual bool DerivedIterate();
};

class JobOSE_CHAN : public MPGPJob
{
public:
	JobOSE_CHAN(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts);
	~JobOSE_CHAN() {;};
protected:
	JobORQ_ANONHEXID *job_getanonhexid;
	AnsiString echochan;
	AnsiString sendtochan;
public:
	virtual bool DerivedIterate();
	virtual void DerivedConfirmed(AnsiString confirmparam);
};

class JobOSE_PUBKEY : public MPGPJob
{
public:
	JobOSE_PUBKEY(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts);
	~JobOSE_PUBKEY() {;};
protected:
	JobORQ_ANONHEXID *job_getanonhexid;
	bool initiatedsend;
public:
	virtual bool DerivedIterate();
};

class JobOSE_MESG : public MPGPJob
{
public:
	JobOSE_MESG(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts);
	~JobOSE_MESG() {;};
protected:
	JobORQ_ANONHEXID *job_getanonhexid;
public:
	virtual bool DerivedIterate();
};

class JobOSE_FILE : public MPGPJob
{
public:
	JobOSE_FILE(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts);
	~JobOSE_FILE() {;};
protected:
	JobORQ_ANONHEXID *job_getanonhexid;
	JobORQ_FILEPERM *job_getfileperm;
	bool initiatedsend;
	bool willacceptfiles;
	AnsiString tempfilename;
public:
	virtual bool DerivedIterate();
	virtual void DerivedCancel();
	virtual void DerivedConfirmed(AnsiString replyparam);
public:
	void ReceivedPermission() {willacceptfiles=true;};
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// Incoming main jobs
class JobISE_PROOF : public MPGPJob
{
public:
	JobISE_PROOF(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobISE_PROOF() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobISE_CHAN : public MPGPJob
{
public:
	JobISE_CHAN(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobISE_CHAN() {;};
protected:
	AnsiString targetchan;
	AnsiString sendtochan;
	AnsiString echochan;
public:
	virtual bool DerivedIncomingCompletes();
	virtual void DerivedConfirmed(AnsiString replyparam);
};

class JobISE_PUBKEY : public MPGPJob
{
public:
	JobISE_PUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobISE_PUBKEY() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobISE_MESG : public MPGPJob
{
public:
	JobISE_MESG(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobISE_MESG() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobISE_FILE : public MPGPJob
{
public:
	JobISE_FILE(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobISE_FILE() {;};
public:
	virtual bool DerivedIncomingCompletes();
	virtual void DerivedCancel();
};
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
// Incoming replies
//---------------------------------------------------------------------------
class JobIRP_ANONHEXID : public MPGPJob
{
public:
	JobIRP_ANONHEXID(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRP_ANONHEXID() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobIRP_ANONPUBKEY : public MPGPJob
{
public:
	JobIRP_ANONPUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRP_ANONPUBKEY() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobIRP_PROOFRAND : public MPGPJob
{
public:
	JobIRP_PROOFRAND(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRP_PROOFRAND() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobIRP_FILEPERM : public MPGPJob
{
public:
	JobIRP_FILEPERM(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIRP_FILEPERM() {;};
protected:
	bool willacceptfiles;
public:
	virtual bool DerivedIncomingCompletes();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// Outgoing replies
//---------------------------------------------------------------------------






















//---------------------------------------------------------------------------
// Incoming control signals
//---------------------------------------------------------------------------

class JobIJC_KILL : public MPGPJob
{
public:
	JobIJC_KILL(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIJC_KILL() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobIJC_ACK : public MPGPJob
{
public:
	JobIJC_ACK(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIJC_ACK() {;};
public:
	virtual bool DerivedIncomingCompletes();
};

class JobIJC_REWIND : public MPGPJob
{
public:
	JobIJC_REWIND(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti);
	~JobIJC_REWIND() {;};
public:
	virtual bool DerivedIncomingCompletes();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// Outgoing control signals
//---------------------------------------------------------------------------
class JobOJC_KILL : public MPGPJob
{
public:
	JobOJC_KILL(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int direction);
	~JobOJC_KILL() {;};
public:
	virtual bool DerivedIterate();
};

class JobOJC_ACK : public MPGPJob
{
public:
	JobOJC_ACK(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int direction,AnsiString replyparam);
	~JobOJC_ACK() {;};
public:
	virtual bool DerivedIterate();
};

class JobOJC_REWIND : public MPGPJob
{
public:
	JobOJC_REWIND(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int pos,int direction);
	~JobOJC_REWIND() {;};
public:
	virtual bool DerivedIterate();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
