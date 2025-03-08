// DerivedJobs.cp
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#include <io.h>
#include <stdio>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
#include "Jobs.h"
#include "DerivedJobs.h"
#include "JobManager.h"
#include "MiscUtils.h"

#include "PgpDialogUnit.h"
#include "JobFormUnit.h"
#include "QuestionFormUnit.h"
#include "MPgpMain.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
// Outgoing requests
//---------------------------------------------------------------------------
JobORQ_ANONHEXID::JobORQ_ANONHEXID(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid)
	:MPGPJob(jobmanager->CreateUniqueJobId(),RQ_ANONHEXID,"request anonymous hex id of "+tn,tc,tn,ti,fid,tid,"",JOBDIR_OUTGOING)
{
	// flowchart for this job
	// 1a. ask recipient for their hexid
	// 1b - receive their hexid
	// IF their hexid is not on the keyring
	//	{
	//	2a request their full anonymous pubkey
	//	2b receive anonymous pubkey
	//	}
	sentrequest=false;
}

bool JobORQ_ANONHEXID::DerivedIterate()
{
	// what stage are we in?
	if (!sentrequest)
		{
		// send the request - which has no contents besides the basic header
		SetOutputString(RQ_ANONHEXID,".");
		set_stagestring("requesting hexid");
		sentrequest=true;
		return true;
		}
	if (ToId!="")
		{
		// we have gotten a valid hexid for a key in our ring (from an adopted reply child), so we are done
		BecomeConfirmed("");
		// give our found pgp hexid to our parent
		parentp->set_ToId(ToId);
		set_stagestring("received");
		return true;
		}
	// still waiting for the reply of a valid id
	set_stagestring("waiting for reply");
	return true;
}

bool JobORQ_ANONHEXID::DerivedAdopt(MPGPJob *injobp)
{
	// look for an anonymous hexid reply
	if (injobp->DoesJobMatch(RP_ANONHEXID,jobid))
		{
		// adopt the child
		AddChild(injobp);
		return true;
		}
	// look for an anonymous pubkey reply
	if (injobp->DoesJobMatch(RP_ANONPUBKEY,jobid))
		{
		// adopt the child
		AddChild(injobp);
		return true;
		}

	return false;
}

void JobORQ_ANONHEXID::NeedsFullPublicKey()
{
	// after a request for a user's anon hexid, we have determined we dont have the key, so now we need to request
	// a full anon pubkey from the user
	set_descstring("request anonymous pubkey from "+tonick);
	SetOutputString(RQ_ANONPUBKEY,".");
	set_stagestring("requesting pubkey");
}

//---------------------------------------------------------------------------

JobORQ_ANONPUBKEY::JobORQ_ANONPUBKEY(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid)
	:MPGPJob(jobmanager->CreateUniqueJobId(),RQ_ANONPUBKEY,"request anonymous pubkey of "+tn,tc,tn,ti,fid,tid,"",JOBDIR_OUTGOING)
{
}

//---------------------------------------------------------------------------
JobORQ_PROOFRAND::JobORQ_PROOFRAND(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid)
	:MPGPJob(jobmanager->CreateUniqueJobId(),RQ_PROOFRAND,"request random proof string from "+tn,tc,tn,ti,fid,tid,"",JOBDIR_OUTGOING)
{
	sentrequest=false;
}
bool JobORQ_PROOFRAND::DerivedIterate()
{
	if (!sentrequest)
		{
		// send the request - as contents we neec to provide the job id of the parent proof request
		// so that the recipient knows which job will eventually arrive 
		AnsiString parentjobid=parentp->get_jobid();
		SetOutputString(RQ_PROOFRAND,parentjobid);
		set_stagestring("requested");
		sentrequest=true;
		return true;
		}
	return true;
}
bool JobORQ_PROOFRAND::DerivedAdopt(MPGPJob *injobp)
{
	// look for an rand string reply
	if (injobp->DoesJobMatch(RP_PROOFRAND,jobid))
		{
		// adopt the child
		AddChild(injobp);
		return true;
		}
	return false;
}


//---------------------------------------------------------------------------
JobORQ_FILEPERM::JobORQ_FILEPERM(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString filestring)
	:MPGPJob(jobmanager->CreateUniqueJobId(),RQ_FILEPERM,"request permission to send file to "+tn,tc,tn,ti,fid,tid,filestring,JOBDIR_OUTGOING)
{
	// file request
	sentrequest=false;

	// get count and size of file(s) to be sent
	filecount=0;
	filesize=0;
	AnsiString filename;
	AnsiString errorlist;

	filelist=filestring.Trim();
	while (JrLeftStringSplit(filelist,filename,"\n"))
		{
		// got a filename
		++filecount;
		if (FileExists(filename))
			{
			// it exists and so we can send it
			filesize+=JRFileLength(filename);
			}
		else
			{
			if (errorlist.Length()>0)
				errorlist+="\n";
			errorlist+=filename;
			}
		}

	if (filecount==0)
		{
		// no files to send
		QuestionForm->PresentOk("","ERROR: No files to send",jobmanager->MessageTimeout(),"");
		ErrorCancel("no file to send");
		return;
		}
	else if (filecount>1)
		{
		// no files to send
		QuestionForm->PresentOk("","ERROR: You can only specify one file to send at a time",jobmanager->MessageTimeout(),"");
		ErrorCancel("no file to send");
		return;
		}
	else if (errorlist.Length()>0)
		{
		// one or more of the files could not be found, so we cancel this job
		QuestionForm->PresentOk("","The following file could not be located to send:\r\n\r\n"+errorlist,jobmanager->MessageTimeout(),"");
		ErrorCancel("file could not be found");
		return;
		}
	else
		{
		// form output string
		filelist=filestring.Trim();
		contents=filelist;
		// set our description
		if (filecount>1)
			set_descstring("request permission to send "+tonick+" "+filecount+" files totaling "+JRPrettySizeString(filesize));
		else
			set_descstring("request permission to send "+tonick+" a "+JRPrettySizeString(filesize)+" file");
		}
}
bool JobORQ_FILEPERM::DerivedIterate()
{
	// just wait for our children to finish and deliver us a randstring
	if (ToId==AnonymousMircryptionKeyId)
		{
		// we are still waiting for a child to retrieve a proper target key id
		return true;
		}

	if (!sentrequest)
		{
		// form the request to transmit files
		bool bretv;
		AnsiString outstring;
		AnsiString parentjobid;
		AnsiString filename;
		// now encrypt and sign it
		contents=filelist;
		JrRightStringSplit(contents,filename,"\\");
		parentjobid=parentp->get_jobid();
		contents=AnsiString(filecount)+" "+AnsiString(filesize)+" "+parentjobid+" "+filename;
		bretv=jobmanager->EncryptString(FromId,ToId,contents,outstring,"Encrypt request to "+PrettyId(tonick,ToId,false));
		if (!bretv)
			{
			// error signing it
			ErrorCancel("could not sign/encrypt request");
			return false;
			}
		SetOutputString(RQ_FILEPERM,outstring);
		set_stagestring("requested");
		sentrequest=true;
		return true;
		}
	return true;
}
bool JobORQ_FILEPERM::DerivedAdopt(MPGPJob *injobp)
{
	// we will adopt the reply
	if (injobp->DoesJobMatch(RP_FILEPERM,jobid))
		{
		// ok so now we have received a reply to our request (either yes or no)
		AddChild(injobp);
		return true;
		}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// Incoming requests
//---------------------------------------------------------------------------
JobIRQ_ANONHEXID::JobIRQ_ANONHEXID(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RQ_ANONHEXID,"provide anonymous hex id to "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	sentreply=false;
}
bool JobIRQ_ANONHEXID::DerivedIterate()
{
	// send our hexid and then mark us as completed (though not confirmed)
	bool bretv;
	int retv;

	if (!sentreply)
		{
		AnsiString sender=jobmanager->PrettySigNick(tonick,true);
		// check users options how they want to deal with this key of request
		int treatanon=OptionsForm->IncomingAnonymousRequests->ItemIndex;
		if (treatanon==TREATANON_SILENTREJECT)
			{
			// we are set to always deny requests for anonymous hex ids silently (ie no reply at all)
			ErrorCancel("options say to always reject silently");
			return false;
			}
		else if (treatanon==TREATANON_REJECT)
			{
			// reject and send a cancel
			jobmanager->SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"anonymous messages disallowed");
			ErrorCancel("options say to always reject");
			return false;
			}
		else if (treatanon==TREATANON_HEXASK)
			{
			// ask the user before providing our anonymous hexid
			retv=QuestionForm->PresentYesNo("","In preparation to sending you something, "+sender+" is requesting your anonymous hexid.\r\n\r\nThis is considered a completely harmless request - providing your anonymous hexid to strangers is not a security risk, and requires minimal bandwidth, but your options specify to ask you before responding to this request.\r\n\r\nProvide your anonymous hex id to "+sender+"?",0,jobmanager->QuestionTimeout(),"incoming request for hexid from "+tonick+" was denied due to question timeout");
			if (retv!=1)
				{
				jobmanager->SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"anonymous message disallowed");
				ErrorCancel("rejected");
				return false;
				}
			}

		// first make sure we have an anonymous key
		bretv=jobmanager->EnsureAnonymousKeyExists();
		if (!bretv)
			{
			ErrorCancel("anonymous pubkey does not exist and could not be made");
			return false;
			}
		// get the hex id of our anonymous key
		AnsiString hexid=jobmanager->GetPgpOurAnonHexId();
		if (hexid=="")
			{
			ErrorCancel("anonymous pubkey not found on keyring");
			return false;
			}
		// send the reply
		SetOutputString(RP_ANONHEXID,hexid);
		// mark it as sent
		sentreply=true;
		// and we are done
		BecomeConfirmed("");
		}
	return true;
}

//---------------------------------------------------------------------------

JobIRQ_ANONPUBKEY::JobIRQ_ANONPUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RQ_ANONPUBKEY,"send anonymous pubkey to "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	sentreply=false;
}
bool JobIRQ_ANONPUBKEY::DerivedIterate()
{
	// send our hexid and then mark us as completed (though not confirmed)
	bool bretv;
	int retv;

	if (!sentreply)
		{
		// first make sure we have an anonymous key
		AnsiString sender=jobmanager->PrettySigNick(tonick,true);
		// check users options how they want to deal with this key of request
		int treatanon=OptionsForm->IncomingAnonymousRequests->ItemIndex;
		if (treatanon==TREATANON_SILENTREJECT)
			{
			// we are set to always deny requests for anonymous hex ids silently (ie no reply at all)
			ErrorCancel("options say to always reject silently");
			return false;
			}
		else if (treatanon==TREATANON_REJECT)
			{
			// reject and send a cancel
			jobmanager->SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"anonymous messages disallowed");
			ErrorCancel("options say to always reject");
			return false;
			}
		else if (treatanon==TREATANON_HEXASK || treatanon==TREATANON_PUBASK)
			{
			// ask the user before providing our anonymous hexid
			retv=QuestionForm->PresentYesNo("","In preparation to sending you something, "+sender+" is requesting your anonymous public key.\r\n\r\nThis is considered a harmless request - providing your anonymous public key to strangers is not a security risk.  However, it will result in you transmitting about 2k of data.\r\n\r\nProvide your anonymous public key to "+sender+"?\r\n\r\nNote: You can disable this pop-up from the Options Screen.",0,jobmanager->QuestionTimeout(),"incoming request for public key from "+tonick+" was denied due to question timeout");
			if (retv!=1)
				{
				jobmanager->SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"anonymous message disallowed");
				ErrorCancel("rejected");
				return false;
				}
			}

		bretv=jobmanager->EnsureAnonymousKeyExists();
		if (!bretv)
			{
			ErrorCancel("anonymous pubkey does not exist and could not be made");
			return false;
			}
		// get the full data for our anonymous public key
		AnsiString pubkey=jobmanager->GetPgpOurAnonPubKey();
		if (pubkey=="")
			{
			ErrorCancel("anonymous pubkey not found on keyring");
			return false;
			}
		// send the reply
		SetOutputString(RP_ANONPUBKEY,pubkey);
		// mark it as sent
		sentreply=true;
		// and we are done
		BecomeAccomplished();
		}
	return true;
}
//---------------------------------------------------------------------------
JobIRQ_PROOFRAND::JobIRQ_PROOFRAND(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RQ_PROOFRAND,"authenticating part 1 - by request, provide random proof string to "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	// we are passed the jobid of the eventual proof message that will be sent to us
	randstring="";
	proofjobid="";
	sentreply=false;
}
bool JobIRQ_PROOFRAND::DerivedIncomingCompletes()
{
	// the proof request has completed
	// now we can get the jobid of the eventual proof message that we need to look for in order to match it up with this initial request
	proofjobid=get_inbufferstring();
	return true;
}
bool JobIRQ_PROOFRAND::DerivedIterate()
{
	if (!sentreply)
		{
		// we need to create a random string, store it for later checking, and sent it to requester
		randstring=jobmanager->MakeRandomProofString();
		// send the reply
		SetOutputString(RP_PROOFRAND,randstring);
		set_stagestring("sent and awaiting proof reply");
		// mark it as sent
		sentreply=true;
		// now, we dont want to se ourselves to be complete yet, as we will need to stay active to adopt the proof reply
		}
	return true;
}
bool JobIRQ_PROOFRAND::DerivedAdopt(MPGPJob *injobp)
{
	// we will adopt the proof reply encoded with out randstring
	if (injobp->DoesJobMatch(SE_PROOF,proofjobid))
		{
		// OK, SO now the user has replied, by signing our rand string.
		// just adopt it here, until the reply is fully parsed, after which we will verify it
		AddChild(injobp);
		return true;
		}
	return false;
}

//---------------------------------------------------------------------------
JobIRQ_FILEPERM::JobIRQ_FILEPERM(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RQ_FILEPERM,"receive request from "+tn+" to send you a file",tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	// receive an incoming file send request.  we memorize filecount and filesize
	filecount=0;
	filesize=0;
	filesendjobid="";
	willacceptfiles=false;
}
bool JobIRQ_FILEPERM::DerivedIncomingCompletes()
{
	// we are receiving a request to receive files
	bool bretv;
	int retv;
	AnsiString numstr;
	AnsiString contents=get_inbufferstring();
	AnsiString sender;
	AnsiString decodedstr;

	// decrypt the incoming channel key
	contents=get_inbufferstring();
	bretv=jobmanager->DecryptString(FromId,ToId,contents,decodedstr,"Decrypt filerequest from "+tonick,false);
	if (!bretv)
		{
		ErrorCancel("decrypt failed");
		return false;
		}
	// get sender pretty info
	sender=jobmanager->PrettySigNick(tonick,true);

	// parse the filecount and filesize
	bretv=JrLeftStringSplit(decodedstr,numstr," ");
	if (bretv==false || !JRStringToInt(numstr,filecount))
		{
		ErrorCancel("filecount couldn't be parsed");
		return false;
		}
	bretv=JrLeftStringSplit(decodedstr,numstr," ");
	if (bretv==false || !JRStringToInt(numstr,filesize))
		{
		ErrorCancel("filesize couldn't be parsed");
		return false;
		}
	bretv=JrLeftStringSplit(decodedstr,filesendjobid," ");
	if (bretv==false)
		{
		ErrorCancel("original sender jobid couldn't be parsed");
		return false;
		}

	// rest of string is the filename
	filename=decodedstr;

	// ok, the request is received, now we want to ask user if they want to accept the files
	retv=QuestionForm->PresentYesNo("",sender+" wants to send you a "+JRPrettySizeString(filesize)+" file: "+filename+".\r\n\r\nAccept it?\r\n\r\nSecurity Note: This file will be stored in an isolated folder and not executed; if sender tries to send a significantly larger file it will be rejected.",0,jobmanager->QuestionTimeout(),"incoming request from "+tonick+" to send you files was denied due to question timeout");
	if (retv!=1)
		{
		// set flag to say we will NOT accept file
		willacceptfiles=false;
		// now we want to send a reply saying YES we will accept the file
		SetOutputString(RP_FILEPERM,"no");
		set_stagestring("permission denied");
		BecomeConfirmed("");
		}
	else
		{
		// set flag to say we will accept file
		willacceptfiles=true;
		// now we want to send a reply saying YES we will accept the file
		SetOutputString(RP_FILEPERM,"yes");
		set_stagestring("permission granted");
		}
	return true;
}

bool JobIRQ_FILEPERM::DerivedAdopt(MPGPJob *injobp)
{
	// we will adopt the reply
	if (injobp->DoesJobMatch(SE_FILE,filesendjobid))
		{
		// ok so now we have received a reply to our request (either yes or no)
		AddChild(injobp);
		// we need to now set the expected max filesize for the incoming job
		// we were promissed a size, but that size may be the size of the unencryptes/signed file, so we need to allow some buffer room
		injobp->set_maxfilesize(filesize*3+3000);
		BecomeConfirmed("");
		return true;
		}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
// Outgoing main jobs
JobOSE_PROOF::JobOSE_PROOF(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts)
	:MPGPJob(jobmanager->CreateUniqueJobId(),SE_PROOF,"prove to "+PrettyId(tn,tid,false)+" that you are "+fid,tc,tn,ti,fid,tid,conts,JOBDIR_OUTGOING)
{
	// flowchart for this job
	// IF sending to anonymous
	//	{
	//	1a. ask recipient for their hexid
	//	1b - receive their hexid
	//	IF their hexid is not on the keyring
	//		{
	//		2a request their full anonymous pubkey
	//		2b receive anonymous pubkey
	//		}
	//	}
	// 3a - request a random string from recipient
	// 3b - receive a random string from them
	// 4a - encrypt and sign the random string
	// 4b - send the recipient the signed&encrypted random string
	// 5 - wait for confirmation that the proof was received

	// get recipient key
	if (tid==AnonymousMircryptionKeyId)
		{
		// we are sending to anonymous, so we need to request their anon hexid
		job_getanonhexid=new JobORQ_ANONHEXID(tc,tn,ti,fid,"");
		AddChild(job_getanonhexid);
		}
	else
		job_getanonhexid=NULL;

	// get a random proofstring
	randproofstring="";
	job_getproofrand=new JobORQ_PROOFRAND(tc,tn,ti,fid,tid);
	AddChild(job_getproofrand);
}

bool JobOSE_PROOF::DerivedIterate()
{
	// just wait for our children to finish and deliver us a randstring
	if (randproofstring=="")
		return true;

	// ok, we are ready now to sign the randstring
	AnsiString signstring;
	AnsiString outstring;
	bool bretv;

	// for extra security we add the fromident and toident, so it will be impossible for anyone to spoof a proof signature
	signstring=randproofstring+":::"+toident+":::"+jobmanager->get_fromident();
	if (contents!="")
		signstring+="\n"+contents;
	// now encrypt and sign it
	bretv=jobmanager->EncryptString(FromId,ToId,signstring,outstring,"Sign proof to "+PrettyId(tonick,ToId,false));
	if (!bretv)
		{
		// error signing it
		ErrorCancel("could not sign proofstring");
		return false;
		}
	// send the signed proof
	SetOutputString(SE_PROOF,outstring);
	// we are done
	set_stagestring("signed and sent proofstring");
	BecomeAccomplished();
	return true;
}


JobOSE_CHAN::JobOSE_CHAN(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts)
	:MPGPJob(jobmanager->CreateUniqueJobId(),SE_CHAN,"send channel key for "+tc+" to "+PrettyId(tn,tid,false)+PrettySig("",fid,false),tc,tn,ti,fid,tid,conts,JOBDIR_OUTGOING)
{
	// flowchart for this job
	// IF sending to anonymous
	//	{
	//	1a. ask recipient for their hexid
	//	1b - receive their hexid
	//	IF their hexid is not on the keyring
	//		{
	//		2a request their full anonymous pubkey
	//		2b receive anonymous pubkey
	//		}
	// 	}
	// 3a - encrypt and sign the channel key
	// 4 - wait for confirmation
	int retv;

	// first make sure we have a valid key
	contents=contents.Trim();
	if (contents=="")
		{
		// its empty, so make a random one
		jobmanager->MakeRandomChanKey(true);
		contents=jobmanager->get_ChannelKeyphrase();
		PgpDialogForm->ContentsMemo->Text=contents;
		}
	// adjust contents
	if (contents.Pos("[RAND]")==1)
		contents=contents.SubString(7,contents.Length()-6);
	else if (contents.Pos("[ERROR]")==1)
		{
		// error in channel generation
		ErrorCancel("error in channel key");
		return;
		}

	// warn them before sending a short channel key
	if (contents.Length()<MinChanKeyLengthWarning)
		{
		retv=QuestionForm->PresentYesNo("","You are about to send (and switch to) a channel key for "+tochan+" which is shorter than the recommended minimum length of "+AnsiString(MinChanKeyLengthWarning)+" characters, and so might be considered insecure.\r\n\r\nYou should always use longer keys that are not made up of dictionary words.\r\n\r\nAre you sure you want to send and switch to this channel key?",0,jobmanager->QuestionTimeout(),"");
		if (retv!=1)
			{
			// abort
			ErrorCancel("canceled");
			return;
			}
		}

	// message to user
	jobmanager->MircMessage(tochan,"Channel change request has been sent to "+tonick+"; please wait for reply before typing anything.");

	// ok, go ahead with the job
	// get recipient key
	if (tid==AnonymousMircryptionKeyId)
		{
		// we are sending to anonymous, so we need to request their anon hexid
		job_getanonhexid=new JobORQ_ANONHEXID(tc,tn,ti,fid,"");
		AddChild(job_getanonhexid);
		}
	else
		job_getanonhexid=NULL;
}

bool JobOSE_CHAN::DerivedIterate()
{
	// send channel key iterate
	AnsiString signstring;
	AnsiString outstring;
	AnsiString cstring;
	int retv;
	bool bretv;

	// just wait for our children to finish and deliver us a randstring
	if (ToId==AnonymousMircryptionKeyId)
		{
		// we are still waiting for a child to retrieve a proper target key id
		return true;
		}

	// signstring is channelname [space] channelkey

	// decide what channel to send them
	if (tochan==tonick)
		{
		// if tochan==tonick then we really want them to set the key for a query/chat which will have our nick as the name
		sendtochan=".";
		echochan=AnsiString("=")+tochan;
		}
	else
		{
		// we are setting a key for a normal mutual channel
		sendtochan=tochan;
		echochan=tochan;
		}

	// string to send
	signstring=sendtochan+" "+contents;

	// ok, we are ready now to encrypt and sign the channel key text
	bretv=jobmanager->EncryptString(FromId,ToId,signstring,outstring,"Sign channel key for "+tochan+" to "+PrettyId(tonick,ToId,false));
	if (!bretv)
		{
		// error signing it
		ErrorCancel("could not encrypt or sign channel key");
		return false;
		}

	// ATTN: TEST
//	AnsiString decodedstr;
//	AnsiString outstring2=outstring;
//	EncodeStringForOutput(outstring2);
//	DecodeStringFromInput(outstring2);
//	if (outstring2!=outstring)
//		jobmanager->JMShowMessage("DECODING ERROR!");
//	bretv=jobmanager->DecryptString(FromId,ToId,outstring2,decodedstr,"Decrypt test from "+tonick,false);
//	jobmanager->JMShowMessage(decodedstr);


	// now we should switch mircryption keys?
	// ATTN: Or should we wait until we get confirmed, or both

	// send the signed proof
	SetOutputString(SE_CHAN,outstring);

	// we are done
	set_stagestring("send channel key proposal");
	BecomeAccomplished();
	return true;
}

void JobOSE_CHAN::DerivedConfirmed(AnsiString confirmparam)
{
	// the other person has responded to our request for key change
	bool bretv;
	if (confirmparam=="yes")
		{
		bretv=jobmanager->SetMircChannelkey(tochan,contents);
		if (!bretv)
			{
			// error changing key
			ErrorCancel("could not set channel key in mirc.");
			jobmanager->MircMessage(tochan,"ERROR: "+tonick+" accepted the new key for "+tochan+" and changed to it, BUT the key could not be set here.");
			SendAnAck("error",JOBDIR_INCOMING,jobtype);
			return;
			}
		// let user know that the key has been changed
		jobmanager->MircMessage(tochan,tonick+" has accepted the new key for "+tochan+" - you may now talk safely.");
		SendAnAck("yes",JOBDIR_INCOMING,jobtype);
		}
	else
		{
		// let user know that the key was NOT changed
		jobmanager->MircMessage(tochan,tonick+" rejected the key change - new key for "+tochan+" was NOT set.");
		}
}



JobOSE_PUBKEY::JobOSE_PUBKEY(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts)
	:MPGPJob(jobmanager->CreateUniqueJobId(),SE_PUBKEY,"send public key for "+PrettyShort(conts,30)+" to "+PrettyId(tn,tid,false)+PrettySig("",fid,false),tc,tn,ti,fid,tid,conts,JOBDIR_OUTGOING)
{
	// flowchart for this job
	// IF sending to anonymous
	//	{
	//	1a. ask recipient for their hexid
	//	1b - receive their hexid
	//	IF their hexid is not on the keyring
	//		{
	//		2a request their full anonymous pubkey
	//		2b receive anonymous pubkey
	//		}
	// 	}
	// 3a - encrypt and sign the non-anonymous public key
	// 4 - wait for confirmation

	// get recipient key
	if (tid==AnonymousMircryptionKeyId)
		{
		// we are sending to anonymous, so we need to request their anon hexid
		job_getanonhexid=new JobORQ_ANONHEXID(tc,tn,ti,fid,"");
		AddChild(job_getanonhexid);
		}
	else
		job_getanonhexid=NULL;
	initiatedsend=false;
}
bool JobOSE_PUBKEY::DerivedIterate()
{
	// send a text message
	AnsiString signstring;
	AnsiString outstring;
	AnsiString cstring;
	int retv;
	bool bretv;
	AnsiString keyname;
	AnsiString keydata;

	// just wait for our children to finish and deliver us a target id
	if (ToId==AnonymousMircryptionKeyId)
		{
		// we are still waiting for a child to retrieve a proper target key id
		return true;
		}

	if (initiatedsend)
		return true;
	// set flag to say we have sent key
	initiatedsend=true;

	// data to send - export the chosen public key
	jobmanager->LogMessage("in JobOSE_PUBKEY::DerivedIterate");
	contents=contents.Trim();
	if (contents=="" || true)
		{
		signstring=jobmanager->GetPgpKeyData(contents);
		}
/*
	else
		{
		// pick off list of explicit key names
		signstring="";
		while (JrLeftStringSplit(contents,keyname,"\n"))
			{
			keyname=keyname.Trim();
			if (keyname=="")
				continue;
			if (jobmanager->GetPgpHexId(keyname)=="")
				{
				// the key could not be found, so launch prompt dialog instead of using explicit list
				contents="";
				signstring=jobmanager->GetPgpKeyData(contents);
				break;
				}
			keydata=jobmanager->GetPgpKeyData(keyname);
			signstring+=keydata;
			}
		}
*/

	if (signstring=="")
		{
		// error getting the public key
		ErrorCancel("could not export public key(s)");
		}

	// ok, we are ready now to encrypt and sign the public key(s)s text
	bretv=jobmanager->EncryptString(FromId,ToId,signstring,outstring,"Sign pubkey for "+PrettyId(tonick,ToId,false));
	if (!bretv)
		{
		// error signing it
		ErrorCancel("could not encrypt or sign message");
		return false;
		}
	// send the signed proof
	SetOutputString(SE_PUBKEY,outstring);
	// we are done
	set_stagestring("sent key");
	BecomeAccomplished();
	
	return true;
}



JobOSE_MESG::JobOSE_MESG(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts)
	:MPGPJob(jobmanager->CreateUniqueJobId(),SE_MESG,"send message ("+PrettyShort(conts,30)+") to "+PrettyId(tn,tid,false)+PrettySig("",fid,false),tc,tn,ti,fid,tid,conts,JOBDIR_OUTGOING)
{
	// flowchart for this job
	// IF sending to anonymous
	//	{
	//	1a. ask recipient for their hexid
	//	1b - receive their hexid
	//	IF their hexid is not on the keyring
	//		{
	//		2a request their full anonymous pubkey
	//		2b receive anonymous pubkey
	//		}
	// 	}
	// 3a - encrypt and sign the message
	// 4 - wait for confirmation

	// get recipient key
	if (tid==AnonymousMircryptionKeyId)
		{
		// we are sending to anonymous, so we need to request their anon hexid
		job_getanonhexid=new JobORQ_ANONHEXID(tc,tn,ti,fid,"");
		AddChild(job_getanonhexid);
		}
	else
		job_getanonhexid=NULL;
}
bool JobOSE_MESG::DerivedIterate()
{
	// send a text message
	AnsiString signstring;
	AnsiString outstring;
	AnsiString cstring;
	int retv;
	bool bretv;

	// just wait for our children to finish and deliver us a target id
	if (ToId==AnonymousMircryptionKeyId)
		{
		// we are still waiting for a child to retrieve a proper target key id
		return true;
		}

	// channel key to send
	signstring=contents;

	// ok, we are ready now to encrypt and sign the channel key text
	bretv=jobmanager->EncryptString(FromId,ToId,signstring,outstring,"Sign message to "+PrettyId(tonick,ToId,false));
	if (!bretv)
		{
		// error signing it
		ErrorCancel("could not encrypt or sign message");
		return false;
		}
	// send the signed proof
	SetOutputString(SE_MESG,outstring);

	// we are done
	set_stagestring("sent message");
	BecomeAccomplished();

	return true;
}



JobOSE_FILE::JobOSE_FILE(AnsiString tc,AnsiString tn,AnsiString ti, AnsiString fid, AnsiString tid, AnsiString conts)
	:MPGPJob(jobmanager->CreateUniqueJobId(),SE_FILE,"send file ("+PrettyShort(conts,30)+") to "+PrettyId(tn,tid,false)+PrettySig("",fid,false),tc,tn,ti,fid,tid,conts,JOBDIR_OUTGOING)
{
	// flowchart for this job
	// IF sending to anonymous
	//	{
	//	1a. ask recipient for their hexid
	//	1b - receive their hexid
	//	IF their hexid is not on the keyring
	//		{
	//		2a request their full anonymous pubkey
	//		2b receive anonymous pubkey
	//		}
	//	}
	// 3a - request permission to send files (provide approx filesize)
	// 3b - receive permission answer
	// If permission was granted
	//	{
	//	4 - encrypt and sign files
	//	5 - send files
	//	6 - wait for confirmation that the proof was received
	//	}

	// get recipient key
	if (tid==AnonymousMircryptionKeyId)
		{
		// we are sending to anonymous, so we need to request their anon hexid
		job_getanonhexid=new JobORQ_ANONHEXID(tc,tn,ti,fid,"");
		AddChild(job_getanonhexid);
		}
	else
		job_getanonhexid=NULL;
	//
	job_getfileperm=NULL;
	initiatedsend=false;
	tempfilename="";
}

bool JobOSE_FILE::DerivedIterate()
{
	bool bretv;
	// we need to first get the recipient id
	if (ToId==AnonymousMircryptionKeyId)
		{
		// we are still waiting for a recipient
		return true;
		}
	else if (job_getfileperm==NULL)
		{
		// get permission to send file(s)
		job_getfileperm=new JobORQ_FILEPERM(tochan,tonick,toident,FromId,ToId,contents);
		AddChild(job_getfileperm);
		}
	else if (willacceptfiles==true && initiatedsend==false)
		{
		// ok, we just got permission to send the file, so initiated file transfer
		initiatedsend=true;
		// ok so first thing we need to do is encrypt the file to send
		AnsiString filename=contents.Trim();

		tempfilename=jobmanager->get_filesdir()+"\\"+jobid+"_tempsentfile.pgp";
		tempfilename=JRUniquifyFilename(tempfilename);
		bretv=jobmanager->EncryptFile(FromId,ToId,filename,tempfilename,"Sign file for transmission");
		if (!bretv)
			{
			ErrorCancel("could not encrypt file to send");
			return false;
			}
		// encode the file for safe transmition over irc
		bretv=EncodeFileForOutput(tempfilename);
		if (!bretv)
			{
			ErrorCancel("could not enccode file to send");
			return false;
			}
		// now trigger the sending of the file
		SetOutputFile(SE_FILE,tempfilename);
		}
	else if (initiatedsend==true)
		{
		// report sending progress
		if (curpos==outputlen)
			{
			// we have finished sending the file
			BecomeAccomplished();
			}
		}
	return true;
}

void JobOSE_FILE::DerivedCancel()
{
	// the job is finished (success or cancel), so erased any tempfile
	jobmanager->LogMessage("Deleting temp file");
	if (tempfilename!="")
		JRSafeDeleteFile(tempfilename);
}

void JobOSE_FILE::DerivedConfirmed(AnsiString replyparam)
{
	// the job is finished (success or cancel), so erased any tempfile
	jobmanager->LogMessage("Deleting temp file");
	if (tempfilename!="")
		JRSafeDeleteFile(tempfilename);
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
// Incoming main jobs
JobISE_PROOF::JobISE_PROOF(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,SE_PROOF,"authenticating part 2 - receive proof signature from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobISE_PROOF::DerivedIncomingCompletes()
{
	// OK, so we have received a proof submission.
	// At this point we have received the complete proof string and our adoptive parent has the PROPER randstring
	// that the signed proof message should contain.
	int retv;
	bool bretv;

	// adjust parent stagestring
	parentp->set_stagestring("authenticating");

	// get randstring form our adoptive parent
	JobIRQ_PROOFRAND *irp=(JobIRQ_PROOFRAND *)parentp;
	AnsiString desired_signstring=irp->get_randstring();
	desired_signstring+=":::"+jobmanager->get_fromident()+":::"+toident;

	// decrypt the proof they sent us
	AnsiString str;
	AnsiString received_signstring=get_inbufferstring();
	AnsiString decryptedstr="";
	AnsiString received_signstringdec;
	bretv=jobmanager->DecryptString(FromId,ToId,received_signstring,decryptedstr,"Decrypt proofstring from "+tonick,false);
	if (!bretv)
		{
		parentp->set_stagestring("authentification failed");
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		ErrorCancel("could not decrypt incoming proof submission");
		return false;
		}
	// separate the proper sig from any additional message
	JrLeftStringSplit(decryptedstr,received_signstringdec,"\n");

	// signify that we and out parent are done
	BecomeConfirmed("");
	parentp->BecomeConfirmed("");

	// finally
	if (received_signstringdec=="")
		{
		// proof not signed
		SendAnAck("no",JOBDIR_OUTGOING,jobtype);
		parentp->set_stagestring("");
		set_stagestring("INVALID PROOF (empty)");
		}
	else if (jobmanager->get_GlobalSig()=="")
		{
		// proof not signed
		SendAnAck("no",JOBDIR_OUTGOING,jobtype);
		parentp->set_stagestring("");
		set_stagestring("INVALID PROOF (signer key not found)");
		}
	else if (received_signstringdec!=desired_signstring)
		{
		// proof does not match
		SendAnAck("no",JOBDIR_OUTGOING,jobtype);
		str=tonick+" has been returned an invalid proof reply.\r\n\r\nExpected:\r\n'"+desired_signstring+"'\r\n\r\nReceived:\r\n'"+received_signstringdec+"'";
		QuestionForm->PresentOk("INVALID Authentification Submission",str,jobmanager->MessageTimeout(),tonick+" sent an invalid proof submission");
		parentp->set_stagestring("");
		set_stagestring("INVALID PROOF (spoof attempt?)");
		}
	else
		{
		// proof does match
		parentp->set_stagestring("");
		SendAnAck("yes",JOBDIR_OUTGOING,jobtype);
		set_descstring("authenticating part 2");

		if (!jobmanager->PgpGoodEnoughValidity(jobmanager->get_GlobalSigValidity()))
			{
			// sig used is not marked as trusted
			set_stagestring("Authenticated "+tonick+" as "+jobmanager->get_GlobalSig()+" ("+jobmanager->get_GlobalSigValidity()+").");
			}
		else
			{
			// sig is a trusted on
			set_stagestring("AUTHENTICATED "+tonick+" as "+jobmanager->get_GlobalSig()+" with trusted key ("+jobmanager->get_GlobalSigValidity()+")");
			}

		// force list update before we popup box
		jobmanager->FillJobListBox(JobForm->JobsListBox);
		// now pop up a window to show them the sucessful authentification
		str=tonick+" has been securely authenticated as "+jobmanager->get_GlobalSig()+" ("+jobmanager->get_GlobalSigValidity()+")";
		if (decryptedstr!="")
			{
			// add any sender comments
			str+="\r\n\r\nMessage from "+jobmanager->get_GlobalSig()+": \""+decryptedstr+"\"";
			}
		retv=QuestionForm->PresentOk("Authentification Received",str,jobmanager->MessageTimeout(),"");
		if (retv==-1 || AlwaysMircResult)
			{
			// it timed out, so show it in mirc status
			if (!jobmanager->PgpGoodEnoughValidity(jobmanager->get_GlobalSigValidity()))
				str=tonick+" has been authenticated as "+jobmanager->get_GlobalSig()+" ("+jobmanager->get_GlobalSigValidity()+")";
			else
				str=tonick+" has been securely authenticated as "+jobmanager->get_GlobalSig()+" with trusted key ("+jobmanager->get_GlobalSigValidity()+")";
			if (decryptedstr!="")
				str+=". Message from "+jobmanager->get_GlobalSig()+": \""+decryptedstr+"\".";
			else
				str+=".";
			jobmanager->MircMessage("",str);
			}
		}
	return true;
}

JobISE_CHAN::JobISE_CHAN(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,SE_CHAN,"receive proposed channel key from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobISE_CHAN::DerivedIncomingCompletes()
{
	// first make sure we have a valid key
	int retv;
	bool bretv;
	AnsiString mesg;
	AnsiString receivechan;
	AnsiString sender;

	// decrypt the incoming channel key
	contents=get_inbufferstring();
	bretv=jobmanager->DecryptString(FromId,ToId,contents,receivechan,"Decrypt proposed chan key for "+tochan+" from "+tonick,false);
	if (!bretv)
		{
		ErrorCancel("decrypt failed");
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		return false;
		}

	// get sender pretty info
	sender=jobmanager->PrettySigNick(tonick,true);

	// get target channel name
	bretv=JrLeftStringSplit(receivechan,targetchan," ");
	if (bretv==false || receivechan=="")
		{
		ErrorCancel("couldnt find channel name");
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		return false;
		}
	// see if it is special channel name for queryies/chats
	if (targetchan==".")
		{
		sendtochan=tonick;
		mesg="You have received a proposed keyphrase for query/chat with "+sendtochan+" from "+sender+": \""+receivechan+"\".";
		echochan=AnsiString("=")+tonick;
		}
	else
		{
		sendtochan=targetchan;
		mesg="You have received a proposed channel keyphrase for "+sendtochan+" from "+sender+": \""+receivechan+"\".";
		echochan=targetchan;
		}

	// warn them before sending a short channel key
	if (receivechan.Length()<MinChanKeyLengthWarning)
		mesg+="\r\n\r\nIMPORTANT: This keyphrase is shorter than the recommended length ("+AnsiString(MinChanKeyLengthWarning)+" characters), and so might be considered insecure; you should always use longer keys that are not made up of dictionary words.";
        mesg+="\r\n\r\nDo you want to accept this keyphrase for "+sendtochan+"?";

	retv=QuestionForm->PresentYesNo("",mesg,0,jobmanager->QuestionTimeout(),tonick+"'s proposed keyphrase for channel "+sendtochan+" was rejected due to question timeout");
	if (retv!=1)
		{
		// abort
		set_stagestring("rejected");
		SendAnAck("no",JOBDIR_OUTGOING,jobtype);
		BecomeConfirmed("");
		return true;
		}
	// accept the new channel
	bretv=jobmanager->SetMircChannelkey(sendtochan,receivechan);
	if (!bretv)
		{
		// error writing it
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		ErrorCancel("could not set channel key in mirc.");
		return false;
		}

	SendAnAck("yes",JOBDIR_OUTGOING,jobtype);
	BecomeAccomplished();

	// create a query for targetchan if appropriate?
	if (targetchan==".")
		DLLDirectSendMircOutput("/mpgp_ensurequery "+sendtochan);

	jobmanager->MircMessage(sendtochan,"Your key for "+sendtochan+" has been changed; please wait for "+tonick+" to acknowledge the change before typing anything...");
	return true;
}
void JobISE_CHAN::DerivedConfirmed(AnsiString replyparam)
{
	if (replyparam=="")
		return;
	if (replyparam=="yes")
		jobmanager->MircMessage(sendtochan,tonick+" has acknowledged the key change; you may safely use this channel now.");
	else
		jobmanager->MircMessage(sendtochan,"ERROR:  "+tonick+" has failed to perform the key change.");
}

JobISE_PUBKEY::JobISE_PUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,SE_PUBKEY,"receive public key from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobISE_PUBKEY::DerivedIncomingCompletes()
{
	// first make sure we have a valid key
	int retv;
	bool bretv;
	AnsiString mesg;
	AnsiString receivedstr;
	AnsiString sender;
	AnsiString outstring;

	// decrypt the incoming pubkey
	contents=get_inbufferstring();

	// get sender pretty info
	sender=jobmanager->PrettySigNick(tonick,true);

	// now show the message
	mesg="You have received a new, non-anonymous, public PGP key from "+sender+".    Note that this is NOT a channel key - it is a public pgp key that you can use to encrypt messages to a specific known user.\r\n";
	mesg+="\r\nDo you want to launch the key import dialog?\r\n\r\n";
	mesg+="VERY IMPORTANT: Examine closely the the keys you import to make sure that you are not overwriting an existing key, and that you trust the sender to vouch for these keys.";

	retv=QuestionForm->PresentYesNo("Public Key Receipt",mesg,0,jobmanager->QuestionTimeout(),"non-anonymous public key from "+tonick+" was rejected due to question timeout");
	// show it in mirc status window
	if (retv==1)
		{
		// public key accepted (at least they will try)
		bretv=jobmanager->DecryptString(FromId,ToId,contents,receivedstr,"Decrypt pubkey from "+tonick,false);
		if (!bretv)
			{
			SendAnAck("error",JOBDIR_OUTGOING,jobtype);
			ErrorCancel("decrypt failed");
			return false;
			}

//		if (bretv)
//			bretv=jobmanager->DecryptString(FromId,ToId,contents,receivedstr,"Import pubkey from "+tonick,true);
//		if (!bretv)
//			{
//			ErrorCancel("import failed");
//			return false;
//			}

		// try to import the data
		bretv=jobmanager->ImportKeyData(receivedstr,outstring,"Import key from "+sender+"?");
		if (!bretv)
			{
			SendAnAck("no",JOBDIR_OUTGOING,jobtype);
			ErrorCancel("not imported");
			return false;
			}
		outstring=jobmanager->get_GlobalKeynames();
		if (outstring!="")
			{
			// key successfully imported
			SendAnAck("yes",JOBDIR_OUTGOING,jobtype);
			mesg="The following public key(s) from "+sender+" have been imported: \""+outstring+"\".\r\n\r\n";
			mesg+="You can examine, delete, or change the trust of these keys using the key manager tool - in fact, newly imported keys are marked by default as INVALID, and you will have to sign new keys from the pgp key tool in order to mark it as valid.\r\n\r\n";
			mesg+="Please remember, that there is no inherent guarantee that these pgp keys belong to the user/email associated with them."; 
			retv=QuestionForm->PresentOk("Public Key Receipt",mesg,jobmanager->MessageTimeout(),"");
			if (retv==-1 || AlwaysMircResult)
				{
				jobmanager->MircMessage("","Imported public key(s) from "+sender+": \""+outstring+"\".");
				}
			}
		else
			{
			// keys not imported
			SendAnAck("error",JOBDIR_OUTGOING,jobtype);
			ErrorCancel("not imported");
			QuestionForm->PresentOk("Public Key Receipt","Error: No keys were imported.",jobmanager->MessageTimeout(),"");
			}
		}
	else
		{
		SendAnAck("no",JOBDIR_OUTGOING,jobtype);
		set_stagestring("rejected incoming key");
		}
	BecomeConfirmed("");
	return true;
}

JobISE_MESG::JobISE_MESG(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,SE_MESG,"receive message from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobISE_MESG::DerivedIncomingCompletes()
{
	// first make sure we have a valid key
	int retv;
	bool bretv;
	AnsiString mesg;
	AnsiString receivedstr;
	AnsiString sender;

	// decrypt the incoming channel key
	contents=get_inbufferstring();
	bretv=jobmanager->DecryptString(FromId,ToId,contents,receivedstr,"Decrypt message from "+tonick,false);
	if (!bretv)
		{
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		ErrorCancel("decrypt failed");
		return false;
		}

	// get sender pretty info
	sender=jobmanager->PrettySigNick(tonick,true);

	// now show the message
	mesg="You have received a secure text message from "+sender+":\r\n\r\n";
	mesg+="\""+receivedstr+"\".";

	retv=QuestionForm->PresentOk("Secure Text Message",mesg,jobmanager->MessageTimeout(),"");
	// show it in mirc status window
	if (retv==-1 || AlwaysMircResult)
		{
		// message timed out, so we show it in mirc
		jobmanager->MircMessage("","Received secure text message from "+sender+": \""+receivedstr+"\".");
		}
	SendAnAck("yes",JOBDIR_OUTGOING,jobtype);
	BecomeConfirmed("");
	return true;
}

JobISE_FILE::JobISE_FILE(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,SE_FILE,"receive file from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	// for an incoming file, if it is accepted, we want to mark it as a file that needs to be received directly into a file
	storeinfile=true;
}
bool JobISE_FILE::DerivedIncomingCompletes()
{
 	// we have finished receiving the file - nothing else to do but decode it
	bool bretv;
	AnsiString outfilename;
	AnsiString mesg;
	AnsiString sender;
	int retv;

	// get sender pretty info
	sender=jobmanager->PrettySigNick(tonick,true);

	if (bufferfilename=="")
		return true;

	bretv=jobmanager->DecryptFile(FromId,ToId,bufferfilename,outfilename,"decrypt file received from "+tonick,false);
	if (!bretv)
		{
		// error decrypting file
		SendAnAck("error",JOBDIR_OUTGOING,jobtype);
		ErrorCancel("could not decrypt file");
		}
	else
		{
		// successfully downloaded and decyrpted
		set_stagestring("received and decrypted");
	 	BecomeConfirmed("");
		// now put up a message telling them so
		mesg="Secure file received from "+sender+":\r\n\r\n";
		mesg+="\""+outfilename+"\".";
		retv=QuestionForm->PresentOk("Secure File Transfer",mesg,jobmanager->MessageTimeout(),"");
		// show it in mirc status window
		if (retv==-1 || AlwaysMircResult)
			{
			// message timed out, so we show it in mirc
			jobmanager->MircMessage("","Secure file received from "+sender+": \""+outfilename+"\".");
			}
		}
	// delete the originally received file
	if (bufferfilename!="")
		{
		JRSafeDeleteFile(bufferfilename);
		bufferfilename="";
		}
	// send an ackknowledgement to sender
	SendAnAck("yes",JOBDIR_OUTGOING,jobtype);
	return true;
}

void JobISE_FILE::DerivedCancel()
{
	// delete the (partially) received file
	if (bufferfilename!="")
		{
		JRSafeDeleteFile(bufferfilename);
		bufferfilename="";
		}
}

//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
// Incoming replies
//---------------------------------------------------------------------------

JobIRP_ANONHEXID::JobIRP_ANONHEXID(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RP_ANONHEXID,"receive anonymous hexid from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}

bool JobIRP_ANONHEXID::DerivedIncomingCompletes()
{
	// the anon hexid has been fully received
	MPGPJob *requestor;
	int retv;

	// get the full reply text
	contents=get_inbufferstring();
	// update stage
	set_stagestring(contents);

	// is the key on our keyring?  if so, just sent the parent ORQ_ANONHEXID
	PgpDialogForm->PGPGetKeyPropsPublic->KeyID=contents;
	retv=PgpDialogForm->PGPGetKeyPropsPublic->KeyIsOnRing();
	// ATTN: test make it always act like key not yet on ring
	if (retv>=0)
		{
		// yes we already have it! so we just give the hexid to our parent
		set_stagestring("found on ring");
		parentp->set_ToId(contents);
		}
	else
		{
		// if not, we will need to cause the creation of a request for the recipient's anon pubkey (which is actually handled by parent)
		set_stagestring("not yet on ring");
		JobORQ_ANONHEXID *orp;
		// cast the parent to the outgoing anonhexid request
		orp=(JobORQ_ANONHEXID *)parentp;
		// now tell orq_the anonhexid to put in a request a full public key
		orp->NeedsFullPublicKey();
		}
	// all done
	BecomeConfirmed("");
	return true;
}

JobIRP_ANONPUBKEY::JobIRP_ANONPUBKEY(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RP_ANONPUBKEY,"receive anonymous pubkey from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}

bool JobIRP_ANONPUBKEY::DerivedIncomingCompletes()
{
	// the anon pubkey has been fully received
	AnsiString hexid;
	bool bretv;

	// get key data
	contents=get_inbufferstring();

	// now add it to our public keyring
	// ATTN: UNFINISHED - we need to ensure that this key is clearly marked as ANONYMOUS -
	// IMPORTANT: if the user tries to give us a non-anonymous looking key, it could confuse us later
	bretv=jobmanager->ImportAnonKeyData(contents,hexid);
	if (bretv==true && hexid!="")
		{
		// and now set the parent ORQ_ANONHEXID
		parentp->set_ToId(hexid);
		// mark this job (the request for anon pubkey) as finished
		set_stagestring("received anonymous pubkey");
		BecomeConfirmed("");
		SendAnAck("yes",JOBDIR_INCOMING,RQ_ANONPUBKEY);
		}
	else
		{
		// pub key could not be imported
		SendAnAck("error",JOBDIR_INCOMING,RQ_ANONPUBKEY);
		jobmanager->MircMessage("","ERROR: could not import anonymous key from "+tonick);
		ErrorCancel("failed to import");
		}
	return true;
}

JobIRP_PROOFRAND::JobIRP_PROOFRAND(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RP_PROOFRAND,"receive random proofstring from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobIRP_PROOFRAND::DerivedIncomingCompletes()
{
	// the reply to our request for a proofstring has come in
	// we just pass the reply up to our parent proof job
	contents=get_inbufferstring();
	set_stagestring("received");
	JobOSE_PROOF *ose;
	// cast the parent to the parent outgoing proofrand request (which is an outgoing proof command)
	ose=(JobOSE_PROOF *)(parentp->get_parentp());
	ose->ReceiveRandString(contents);
	// and confirm the parent request
	parentp->BecomeConfirmed("");
	BecomeConfirmed("");
	return true;
}


JobIRP_FILEPERM::JobIRP_FILEPERM(AnsiString jid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jid,RP_FILEPERM,"receive file permission answer from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
	willacceptfiles=false;
}
bool JobIRP_FILEPERM::DerivedIncomingCompletes()
{
	contents=get_inbufferstring();
	if (contents=="yes")
		{
		willacceptfiles=true;
		set_stagestring("permission granted");
		// ok so now we want to tell our original Outgoing send file job to go ahead and send it
		// our parent is a file request, and its parent is a OSE_FILE
		JobOSE_FILE *sef=(JobOSE_FILE *) (parentp->get_parentp());
		sef->ReceivedPermission();
		// tell our parent that it is accomplished
		parentp->BecomeConfirmed("");
		}
	else
		{
		willacceptfiles=false;
		ErrorCancel("permission denied");
		}
	BecomeConfirmed("");
	return true;
}

//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// Incoming control signals
//---------------------------------------------------------------------------
JobIJC_KILL::JobIJC_KILL(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_KILL,"receive cancel request from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobIJC_KILL::DerivedIncomingCompletes()
{
	// retrieve target job id and type
	AnsiString targetjobid,targetjobtype,targetjobdirstr;
	int targetjobdir;
	MPGPJob *jobp;
	bool bretv;
	// parse parameters
	contents=get_inbufferstring();
	bretv=JrLeftStringSplit(contents,targetjobid," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobtype," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobdirstr," ");
	if (bretv) bretv=JRStringToInt(targetjobdirstr,targetjobdir);
	if (!bretv)
		{
		ErrorCancel("badly formed request");
		return true;
		}
	// lookup job and cancel it
	jobp=jobmanager->LookUpMatchingJob(tochan,tonick,toident,targetjobid,targetjobtype,targetjobdir);
	if (jobp==NULL)
		{
		ErrorCancel("target job ("+targetjobid+","+targetjobtype+","+AnsiString(targetjobdir)+") not found");
		return true;
		}
	jobp->Cancel("canceled by "+tonick);
	// done
	set_stagestring("processed");
	BecomeConfirmed("");
	return true;
}

JobIJC_ACK::JobIJC_ACK(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_ACK,"receive job ack from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobIJC_ACK::DerivedIncomingCompletes()
{
	// retrieve target job id and type
	AnsiString targetjobid,targetjobtype,targetjobdirstr;
	int targetjobdir;
	MPGPJob *jobp;
	bool bretv;
	// parse parameters
	contents=get_inbufferstring();
	bretv=JrLeftStringSplit(contents,targetjobid," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobtype," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobdirstr," ");
	if (bretv) bretv=JRStringToInt(targetjobdirstr,targetjobdir);
	if (!bretv)
		{
		ErrorCancel("badly formed request");
		return true;
		}
	// lookup job and confirm it
	jobp=jobmanager->LookUpMatchingJob(tochan,tonick,toident,targetjobid,targetjobtype,targetjobdir);
	if (jobp==NULL)
		{
		ErrorCancel("target job ("+targetjobid+","+targetjobtype+","+AnsiString(targetjobdir)+") not found");
		return true;
		}
	// remainder of contens is ack param
	jobp->BecomeConfirmed(contents);
	// done
	set_stagestring("processed");
	BecomeConfirmed("");
	return true;
}

JobIJC_REWIND::JobIJC_REWIND(AnsiString ijobid,AnsiString tc,AnsiString tn,AnsiString ti)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_REWIND,"receive rewind request from "+tn,tc,tn,ti,"","","",JOBDIR_INCOMING)
{
}
bool JobIJC_REWIND::DerivedIncomingCompletes()
{
	// retrieve target job id and type
	AnsiString targetjobid,targetjobtype,targetposstr,targetjobdirstr;
	int targetjobdir,targetpos;
	MPGPJob *jobp;
	bool bretv;
	// parse parameters
	contents=get_inbufferstring();
	bretv=JrLeftStringSplit(contents,targetjobid," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobtype," ");
	if (bretv) bretv=JrLeftStringSplit(contents,targetjobdirstr," ");
	if (bretv) bretv=JRStringToInt(targetjobdirstr,targetjobdir);
	if (bretv) bretv=JrLeftStringSplit(contents,targetposstr," ");
	if (bretv) bretv=JRStringToInt(targetposstr,targetpos);
	if (!bretv)
		{
		ErrorCancel("badly formed request");
		return true;
		}
	// lookup job and rewind it
	jobp=jobmanager->LookUpMatchingJob(tochan,tonick,toident,targetjobid,targetjobtype,targetjobdir);
	if (jobp==NULL)
		{
		ErrorCancel("target job ("+targetjobid+","+targetjobtype+","+AnsiString(targetjobdir)+") not found");
		return true;
		}
	if (jobp->IsConfirmed() || jobp->IsCanceled())
		{
		ErrorCancel("can't rewind - job already finished");
		return true;
		}
	if (targetpos>=jobp->get_outputlen() || targetpos<0)
		{
		ErrorCancel("illegal rewind position for job "+targetjobid);
		return true;
		}
	jobp->set_curpos(targetpos);
	jobp->set_stagestring("rewound by "+tonick);
	// done
	set_stagestring("processed");
	BecomeConfirmed("");
	return true;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// Outgoing control signals
//---------------------------------------------------------------------------
JobOJC_KILL::JobOJC_KILL(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int direction)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_KILL,"cancel "+tn+"'s job "+MPGPJob::GetPJobid(targetjobid,direction),tc,tn,ti,"","",targetjobid+" "+targetjobtype+" "+AnsiString(direction),JOBDIR_OUTGOING)
{
}
bool JobOJC_KILL::DerivedIterate()
{
	// send the signed proof
	SetOutputString(JC_KILL,contents);
	// we are done
	set_stagestring("sent kill signal");
	BecomeConfirmed("");
	return true;
}


JobOJC_ACK::JobOJC_ACK(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int direction,AnsiString replyparam)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_ACK,"confirm "+tn+"'s job "+MPGPJob::GetPJobid(targetjobid,direction),tc,tn,ti,"","",targetjobid+" "+targetjobtype+" "+AnsiString(direction)+" "+replyparam,JOBDIR_OUTGOING)
{
}
bool JobOJC_ACK::DerivedIterate()
{
	// send the signed proof
	SetOutputString(JC_ACK,contents);
	// we are done
	set_stagestring("sent");
	BecomeConfirmed("");
	return true;
}

JobOJC_REWIND::JobOJC_REWIND(AnsiString tc,AnsiString tn,AnsiString ti,AnsiString targetjobid,AnsiString targetjobtype,int pos,int direction)
	:MPGPJob(jobmanager->CreateUniqueJobId(),JC_REWIND,"rewind "+tn+"'s job "+MPGPJob::GetPJobid(targetjobid,direction),tc,tn,ti,"","",targetjobid+" "+targetjobtype+" "+AnsiString(direction)+" "+AnsiString(pos),JOBDIR_OUTGOING)
{
}
bool JobOJC_REWIND::DerivedIterate()
{
	// send the signed proof
	SetOutputString(JC_REWIND,contents);
	// we are done
	set_stagestring("sent rewind signal");
	BecomeConfirmed("");
	return true;
}
//---------------------------------------------------------------------------


