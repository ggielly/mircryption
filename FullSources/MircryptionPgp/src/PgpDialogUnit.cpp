//---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#include <io.h>
#include <stdio>
#pragma hdrstop
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "PgpDialogUnit.h"
#include "JobFormUnit.h"
#include "OptionsFormUnit.h"
#include "LogFormUnit.h"
#include "MiscUtils.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Dropper"

#pragma link "PGP2Comp"
#pragma link "PGPKeyGenerate"
#pragma link "PGPDecode"
#pragma link "PGPEncode"
#pragma link "PGPDecode"
#pragma link "PGPEncode"
#pragma link "TJYarrow"

#pragma resource "*.dfm"
TPgpDialogForm *PgpDialogForm;
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
__fastcall TPgpDialogForm::TPgpDialogForm(TComponent* Owner)
	: TForm(Owner)
{
	// For intercepting pgp errors
	Application->OnMessage=AppOnMessage;

	// update key listboxes
	PGPUpdateKeyrings();

	// indicate proper transmission type
	// This is actually bad to execute here, since it forces a call to jobmanager which may not be active yet
	// better to force this from jobmanager itself
	//	TypeRadioGroupClick(NULL);

	// for hints
	lasthint="";
	Application->OnHint=ShowHint;

	// adjust constraints
	SplitterMoved(NULL);
}


_fastcall TPgpDialogForm::~TPgpDialogForm()
{
	// nothing to do here.
}


void TPgpDialogForm::Initialize()
{
	// call this AFTER all forms are constructed, but BEFORE any other processing
}


void __fastcall TPgpDialogForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (nowquitting)
		return;
	// user wants to close form
	if (jobmanager==NULL)
		{
		// this can happen if we are quiting app, so we close ourselves
		return;
		}

	if (jobmanager->get_wantsquit())
		{
		// we really want to quit;
		jobmanager->SelfRunThreadEnd();
		return;
		}

	// but we will not really close it - just hide it
	Action=caNone;
	Hide();
	// give opportunity to sleep if there are no jobs
	jobmanager->SleepIfNoJobs();
}
//---------------------------------------------------------------------------




























void __fastcall TPgpDialogForm::SendButtonClick(TObject *Sender)
{
	// make and queue the job
	bool bretv;
	bretv=jobmanager->MakeAndQueueJobFromDialog();

	if (bretv)
		{
		// now update the jobs listbox
		jobmanager->FillJobListBox(JobForm->JobsListBox);
		// Hide the dialog after they send
		Hide();
		}

	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TPgpDialogForm::TypeRadioGroupClick(TObject *Sender)
{
	// user changes radiobox option
	switch (TypeRadioGroup->ItemIndex)
        {
	case JOBRADIO_PROOF:
		TypeRadioGroup->Hint="Send a proof that you are who you say you are.";
		ContentsMemo->Hint="Any text you want to send along with your authentification.";
		ContentsMemo->Visible=true;
		ContentsGroupBox->Caption="Additional Greetings to Accompany Authentification";
		ContentsMemo->Text="";
		RandomizeChannelKeyItem->Visible=false;
		break;
	case JOBRADIO_CHANKEY:
		TypeRadioGroup->Hint="Send the current channel keyphrase (or create a new random one)";
		ContentsMemo->Hint="Channel keyphrase to send (and set in mircryption).";
		ContentsMemo->Visible=true;
		ContentsGroupBox->Caption="Channel Keyphrase";
		// now generate random key IF it dont exist
		jobmanager->MakeRandomChanKey(false);
		ContentsMemo->Text=jobmanager->GetCurrentChannelKeyphrase();
		RandomizeChannelKeyItem->Visible=true;
		break;
	case JOBRADIO_PUBLICKEY:
		TypeRadioGroup->Hint="Send a public key from your keyring to the receiver.";
		ContentsMemo->Hint="Identifiers of public key to send - leave blank for selection dialog";
		ContentsMemo->Visible=true;
		ContentsGroupBox->Caption="Public Key(s) to send";
		ContentsMemo->Text="";
		RandomizeChannelKeyItem->Visible=false;
		break;
	case JOBRADIO_MESSAGE:
		TypeRadioGroup->Hint="Send a short message.";
		ContentsMemo->Hint="Text of message to send";
		ContentsMemo->Visible=true;
		ContentsGroupBox->Caption="Message to Send";
		ContentsMemo->Text="";
		RandomizeChannelKeyItem->Visible=false;
		break;
	case JOBRADIO_FILE:
		TypeRadioGroup->Hint="Send a *small* file through normal irc (ie over bnc/firewall).";
		ContentsMemo->Hint="File to send - this is *EXTREMELY SLOW* so only send tiny files.";
		ContentsMemo->Visible=true;
		ContentsGroupBox->Caption="File List (drag and drop file here)";
		ContentsMemo->Text="";
		RandomizeChannelKeyItem->Visible=false;
		break;
	}
}
//---------------------------------------------------------------------------




void __fastcall TPgpDialogForm::OptionsButtonClick(TObject *Sender)
{
	if (OptionsForm->Visible==false)
		OptionsForm->Present();
	else
		OptionsForm->Hide();
}


void __fastcall TPgpDialogForm::NewAnonymousKeyButtonClick(TObject *Sender)
{
	jobmanager->MakeNewAnonymousKey();
}


void __fastcall TPgpDialogForm::KeyManagerButtonClick(TObject *Sender)
{
	jobmanager->LaunchKeyManager();
}



void __fastcall TPgpDialogForm::LogButtonClick(TObject *Sender)
{
	// toiggle display of log window
	if (LogForm->Visible==false)
		LogForm->Present();
	else
		LogForm->Hide();
}


void __fastcall TPgpDialogForm::JoblistButtonClick(TObject *Sender)
{
	// toggle visibility of joblist
	if (JobForm->Visible==false)
		JobForm->Show();
	else
		JobForm->Hide();
}


void __fastcall TPgpDialogForm::HelpButtonClick(TObject *Sender)
{
	// Show Help
	AnsiString execfilename=jobmanager->get_basedir()+"MPGP.chm";
	ShellExecute(NULL, "open", execfilename.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void __fastcall TPgpDialogForm::CancelButtonClick(TObject *Sender)
{
	// user cancels
	Close();
}


void __fastcall TPgpDialogForm::ListBoxFromClick(TObject *Sender)
{
	// modify hint based on anonymous or no
	if (ListBoxFrom->Items->Strings[ListBoxFrom->ItemIndex].Pos(AnonymousMircryptionKeyId)==1)
		ListBoxFrom->Hint="Anonymous tranmissions are encrypted but not signed.";
	else
		ListBoxFrom->Hint="Your message will be signed with this key which the recipient MUST already have.";
}


void __fastcall TPgpDialogForm::ListBoxToClick(TObject *Sender)
{
	// modify hint based on anonymous or no
	if (ListBoxTo->Items->Strings[ListBoxTo->ItemIndex].Pos(AnonymousMircryptionKeyId)==1)
		ListBoxTo->Hint="Your message will be encrypted to the recipient's anonymous key.";
	else
		ListBoxTo->Hint="Your message will be encrypted to this key, which the recipient must have.";
}

void __fastcall TPgpDialogForm::RandomizeChannelKeyItemClick(TObject *Sender)
{
	// randomize channel key
	jobmanager->MakeRandomChanKey(true);
	TypeRadioGroupClick(Sender);
}







void __fastcall TPgpDialogForm::IterateJobsButtonClick(TObject *Sender)
{
	// iterate the jobs, as a test
	jobmanager->Iterate(0);
}

void __fastcall TPgpDialogForm::ContentsMemoExit(TObject *Sender)
{
	// contents change, so we update the internal variables, depending on what job is current
	switch (TypeRadioGroup->ItemIndex)
        {
	case JOBRADIO_PROOF:
		break;
	case JOBRADIO_CHANKEY:
		break;
	case JOBRADIO_PUBLICKEY:
		break;
	case JOBRADIO_MESSAGE:
		break;
	case JOBRADIO_FILE:
		break;
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void __fastcall TPgpDialogForm::AppOnMessage(tagMSG &Msg, bool &Handled)
{
	if (  Msg.message==WM_PGP_ReloadPrefs || Msg.message==WM_PGP_ReloadKeyring)
		PGPUpdateKeyrings();
}


void __fastcall TPgpDialogForm::ShowHint(TObject *Sender)
{
	// show the hint on the status bar
	if (Application->Hint != lasthint && lasthint!="_NONHINT_")
		StatusShow(Application->Hint,true);
	else if (lasthint=="_NONHINT_")
		lasthint="";
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void __fastcall TPgpDialogForm::SplitterMoved(TObject *Sender)
{
	// adjust constraints
	FixResizeKludge(NULL);
}


void __fastcall TPgpDialogForm::FixResizeKludge(TObject *Sender)
{
	if (nowquitting)
		return;

	int mh;
	int pmh;

	int minbotv=(Constraints->MinHeight)-120;
	PanelFrom->Constraints->MaxHeight=Height-minbotv;

//	pmh=PanelFrom->Height+(Constraints->MinHeight)+3;
//	mh=pmh+(PanelBottom->Constraints->MinHeight)+81;

//	PanelTop->Constraints->MinHeight=pmh;

	// change min height ONLY if it is smaller than current height
//	if (mh<Height)
//		Constraints->MinHeight=mh;
//	PanelFrom->Constraints->MaxHeight=(Height-(PanelTo->Constraints->MinHeight+PanelBottom->Constraints->MinHeight))-83;

	// kludge to prevent wierd effect
//	PanelFrom->Constraints->MaxHeight=(Height-(PanelTo->Constraints->MinHeight+PanelBottom->Constraints->MinHeight))-83;
}


void __fastcall TPgpDialogForm::FileDropperDrop(TObject *Sender, AnsiString Filename)
{
	// user has dropped a file
	if (TypeRadioGroup->ItemIndex!=4)
		{
		// auto switching dont seem to work
		//		TypeRadioGroup->ItemIndex=4;
		ShowMessage("You need to switch to the File transmission type before dragging files.");
		return;
		}
	ContentsMemo->Lines->Text=Filename;
//	ContentsMemo->Lines->Text=(ContentsMemo->Lines->Text).Trim();
}


void __fastcall TPgpDialogForm::JYarrow1Seed(TObject *Sender)
{
	// this allows us to provide a custom seed procedure, in addition to default entropy from timers and clocks and mouse?
}
















//---------------------------------------------------------------------------
void __fastcall TPgpDialogForm::PGPDecodeGetOutputFileName(AnsiString &SuggestedName)
{
	// if file exists, change it
	// make it safe in case they are doing something naughty
	if (SuggestedName=="")
		SuggestedName="NoNameProvided";
	SuggestedName=ExtractFileName(SuggestedName);
	// now make sure it is unique
	SuggestedName=JRUniquifyFilename(SuggestedName);
	jobmanager->set_GlobalFilename(SuggestedName);
}

void __fastcall TPgpDialogForm::PGPKeysGenerateShowState(char State, LongBool &Cancel)
{
	// Key generatoin progress
	ProgressString=ProgressString + ".";
	StatusShow(ProgressString,false);
}

void __fastcall TPgpDialogForm::PGPKeysGenerateGetPassphrase(const PChar Passphrase, const TKeyPropsRec &MasterKeyProps,TMinPassLen MinPassLen, TMinPassQual MinPassQual, LongBool &Cancel)
{
	// auto password for anonymous keys - this used to be locked to OnGetPassphrase with pgpdc version 323, now its onEnterPassphrase with v 330

	strcpy(Passphrase,"");
	PGPKeysGenerate->MinPassLen=0;
	PGPKeysGenerate->MinPassQual=0;
	ProgressString="Generating key...";
}


void __fastcall TPgpDialogForm::PGPKeysGenerateKeyGeneration(const AnsiString NewUserID, const AnsiString NewHexID, LongBool Aborted)
{
	// OLD Function prototype for pgpdc v323 - no longer used! 
	if (!Aborted)
		{
		ProgressString="New key: " + NewUserID + " [" + NewHexID + "]";
		StatusShow(ProgressString,false);
		}
	else
		{
		ProgressString="Key generation aborted.";
		StatusShow(ProgressString,false);
		}
	PGPUpdateKeyrings();
}

void __fastcall TPgpDialogForm::PGPKeysGenerateKeyGeneration2(const AnsiString NewHexID, TKeyPropsRec &MasterKeyProps,LongBool Aborted)
{
	// new function prototype for pgpdc 330
	// on key generation
	if (!Aborted)
		{
		ProgressString="New key: " + NewHexID + "]";
		StatusShow(ProgressString,false);
		}
	else
		{
		ProgressString="Key generation aborted.";
		StatusShow(ProgressString,false);
		}
	PGPUpdateKeyrings();
}


void __fastcall TPgpDialogForm::PGPEncodeShowProgress(int BytesProcessed, int BytesTotal)
{
	// progress of encryptor
	// int dots=(int)((30.0*BytesProcess)/BytesTotal);
}


void __fastcall TPgpDialogForm::PGPEncodeEncoded(const AnsiString BufferOut, const AnsiString FileOut)
{
	// called after encoding a buffer
	jobmanager->set_GlobalCodedOutput(BufferOut);
}

void __fastcall TPgpDialogForm::PGPDecodeDecoded(const AnsiString BufferOut, const AnsiString FileOut, const TSigPropsRec &SigProps, const TKeyPropsList *KeyPropsList)
{
	// called after encoding a buffer
//	ShowMessage("In TPgpDialogForm::PGPDecodeDecoded with "+BufferOut+","+FileOut);
	jobmanager->set_GlobalCodedOutput(BufferOut);
}

void __fastcall TPgpDialogForm::PGPFailure(int ErrorCode,  const AnsiString ErrorMsg)
{
	// Generic pgp component error handler
	// This isnt strictly on error, but on some commands return codes (ie key not found, etc)
	// we show it in log, just for debugging purposes
	jobmanager->LogDebug("PGP Driver: "+ErrorMsg+" ("+AnsiString(ErrorCode)+").");

	// special bad pgp bug bug error - keyfiles get locked and cant import or generate new keys?
	if (ErrorMsg=="file operation error")
		jobmanager->PgpFileOperationError();
}


void __fastcall TPgpDialogForm::PGPDecodeGetSignature(const TSigPropsRec &SigProps)
{
	jobmanager->set_GlobalSig(SigProps.sUserID);
	jobmanager->set_GlobalSigValidity(SigProps.sValidity);
}


void __fastcall TPgpDialogForm::PGPKeyImportKeyImported(const TKeyPropsList *KeyPropsList, int KeysImported)
{
	// a key is imported, so now we want to retrieve it's hexid
	TKeyPropsRec kpr;
	bool bretv;

	if (KeysImported>0)
		{
		AnsiString str=KeyPropsList->Strings[0];
		jobmanager->set_GlobalHexId(str);
		// get keynames
		AnsiString keyname,keynames,hexids;
		keynames="";
		hexids="";
		for (int count=0;count<KeysImported;++count)
			{
			bretv=KeyPropsList->GetKeyPropsRec(kpr,count);
			if (!bretv)
				continue;
			keyname=kpr.kUserID;
			if (keynames!="")
				keynames+=",";
			keynames+=keyname;
			str=KeyPropsList->Strings[count];
			if (hexids!="")
				hexids+=",";
			hexids+=str;
			}
		jobmanager->set_GlobalKeynames(keynames);
		jobmanager->set_GlobalHexIds(hexids);
//		jobmanager->JMShowMessage("Keys imported: "+keynames);
		}
}



//---------------------------------------------------------------------------
void TPgpDialogForm::StatusShow(const AnsiString textstr,bool ishint)
{
	if (Visible==false && JobForm->Visible==true)
		{
		// forward the hint to the jobform since it is visible and we are not
		JobForm->StatusShow(textstr,ishint);
		}
	else
		{
		if (StatusBar->SimpleText!=textstr)
			StatusBar->SimpleText=textstr;
		if (ishint==true)
			lasthint=textstr;
		else
			lasthint="_NONHINT_";
		}
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
void TPgpDialogForm::PGPUpdateKeyrings()
	{
	// keyrings may have changed, so we need to update our listboxes
	AnsiString fromkeyid,tokeyid,keyname;
	int sindex;
	int count;
	int boxid;

	// update interal key listboxes
	PGPGetKeyPropsPrivate->Update();
	PGPGetKeyPropsPublic->Update();

	// now update visibles

	// remember currently selected keys
	sindex=ListBoxFrom->ItemIndex;
	if (sindex==-1)
		fromkeyid=AnonymousMircryptionKeyId;
	else
		fromkeyid=ListBoxFrom->Items->Strings[sindex];
	sindex=ListBoxTo->ItemIndex;
	if (sindex==-1)
		tokeyid=AnonymousMircryptionKeyId;
	else
		tokeyid=ListBoxTo->Items->Strings[sindex];

	// now transfer visible keys

	// from box
	ListBoxFrom->Items->Clear();
	ListBoxFrom->Items->Add(AnonymousMircryptionKeyId);
	boxid=0;
	for (count=0;count<ListBoxPrivInternal->Items->Count;++count)
		{
		keyname=ListBoxPrivInternal->Items->Strings[count];
		if (keyname.Pos(AnsiString(AnonymousMircryptionKeyId))==1)
			continue;
		if (keyname.Pos(AnonymousMircryptionKeyIdPrefix)>0)
			continue;
		ListBoxFrom->Items->Add(keyname);
		++boxid;
		if (keyname==fromkeyid)
			ListBoxFrom->ItemIndex=boxid;
		}
	if (ListBoxFrom->ItemIndex==-1)
		ListBoxFrom->ItemIndex=0;

	// to box
	ListBoxTo->Items->Clear();
	ListBoxTo->Items->Add(AnonymousMircryptionKeyId);
	boxid=0;
	for (count=0;count<ListBoxPublicInternal->Items->Count;++count)
		{
		keyname=ListBoxPublicInternal->Items->Strings[count];
		if (keyname==AnsiString(AnonymousMircryptionKeyId))
			continue;
		if (keyname.Pos(AnsiString(AnonymousMircryptionKeyId))==1)
			continue;
		if (keyname.Pos(AnonymousMircryptionKeyIdPrefix)>0)
			continue;
		ListBoxTo->Items->Add(keyname);
		++boxid;
		if (keyname==tokeyid)
			ListBoxTo->ItemIndex=boxid;
		}
	if (ListBoxTo->ItemIndex==-1)
		ListBoxTo->ItemIndex=0;

	// update hints
	ListBoxFromClick(NULL);
	ListBoxToClick(NULL);
	}
//---------------------------------------------------------------------------


void __fastcall TPgpDialogForm::PGPDecodeAddKeys(LongBool &CancelAdding)
{
//	jobmanager->JMShowMessage("In decode addkeys.");
}
//---------------------------------------------------------------------------

void __fastcall TPgpDialogForm::PGPKeyImportGetFileIn(AnsiString &FileIn)
{
//	jobmanager->JMShowMessage("PGPKeyImportGetFileIn: "+FileIn+".");
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void __fastcall TPgpDialogForm::OutputTimerTimer(TObject *Sender)
{
//	Timer ticks
	static bool simplemutex=false;

    if (jobmanager==NULL)
         return;

	if (!simplemutex)
		{
		simplemutex=true;
		if (!jobmanager->get_BlockedForInput())
			jobmanager->TimerTicks(OutputTimer->Interval);
		simplemutex=false;
		}
}
//---------------------------------------------------------------------------



void TPgpDialogForm::ForceRadioGroup(int index)
{
	// kludge to fix bug that resets this?
	TypeRadioGroup->OnClick=NULL;
	TypeRadioGroup->ItemIndex=index;
	TypeRadioGroup->OnClick=TypeRadioGroupClick;
	TypeRadioGroup->ItemIndex=index;
	TypeRadioGroupClick(NULL);
}
void __fastcall TPgpDialogForm::PGPKeyExportKeyExported(const TKeyPropsList *KeyPropsList, const AnsiString KeyData, const AnsiString FileOut)
{
	// anything to do here?
}
//---------------------------------------------------------------------------



