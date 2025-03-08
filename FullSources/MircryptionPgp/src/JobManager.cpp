// JobManager.cpp

//---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
//#include <SysUtils.hpp>
//#include <systdate.h>
#include <io.h>
#include <stdio>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
#include "PgpDialogUnit.h"
#include "JobFormUnit.h"
#include "JobManager.h"
#include "Jobs.h"
#include "DerivedJobs.h"
#include "OptionsFormUnit.h"
#include "LogFormUnit.h"
#include "MiscUtils.h"
#include "QuestionFormUnit.h"
#include "MPgpMain.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Global jobmanager
MPGPJobManager *jobmanager;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// A Thread to let the dialog run asynchonously

SelfThreader::SelfThreader()
	:TThread(false)
{
	// constructor
	FreeOnTerminate=true;
	Priority = tpNormal;
}
void __fastcall SelfThreader::Execute()
{
	// This rather complicated ordeal is nesc. to allow normal VCL stuff like application hints to work
	//  in an independent thread (asynchronously as a dll)
	while (!Terminated)
		{
		// make a safe call to iterate the application vcl function
		Synchronize(SelfRunThreadIterates);
		Sleep(50);
		}
}
void __fastcall SelfThreader::SelfRunThreadIterates(void)
{
	if (jobmanager->get_ScheduledJobIterate())
		{
		// reset flag
		jobmanager->set_ScheduledJobIterate(false);
		// this is a new technique so that job scheduling is asynchronous
		jobmanager->IterateJobs();
		// now call iterateoutput again (though since we already wrote some output before, we may not have any output to generate)
		jobmanager->IterateOutput();
		}
	Application->HandleMessage();
	// ProcessMessages() is faster but doesnt support Application Hints?
	// Application->ProcessMessages();
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
MPGPJobManager::MPGPJobManager()
{
	// initialize queues and jobs
	QIfirstp=NULL;
	QIlastp=NULL;
	QOfirstp=NULL;
	QOlastp=NULL;
	Jfirstp=NULL;
	Jlastp=NULL;

	// pointer to thread that will keep us running
	selfthreaderp=NULL;

	// init
	JobCounter=0;
	JobsHaveChanged=true;
	set_AllowedOutBytes(200);
	PgpAsciiArmour=DEFPgpAsciiArmour;
	HasWorkToDo=false;
	wantsquit=false;
	ScheduledJobIterate=false;
	BlockedForInput=false;
	DidInitialEnsureAnonymous=false;
	LeftOverWriteBytes=0;
}


MPGPJobManager::~MPGPJobManager()
{
	// destructor

	// deinitialize
	DeInitialize();
}


bool MPGPJobManager::Initialize()
{
	// call this AFTER all forms are constructed, but BEFORE any other processing
	Paused=false;

	// set base dll directory
	char DllFileName[500];
	GetModuleFileName( HInstance,DllFileName, sizeof (DllFileName) );
	basedir=ExtractFilePath(DllFileName);

	// initialize yarrow prng
	PgpDialogForm->JYarrow1->Initialize();

	// other initialization
	filesdir=basedir+"SentAndReceivedFiles";
	configfilename=basedir+"MPGP.ini";
	pgp_KeyringsNeedRestore=false;

	// initialize main form
	PgpDialogForm->Initialize();

	// Initialize pgp
	InitPgpTools();
	// pgp init error?
	if (Pgpbase::PGPInitErrorCode!=ieNone)
		return false;

	// Load options
	LoadIniSettings();

	// init alternate pgp keyring locations (possibly based on ini options)
	SetAltPgpKeyrings();

	// Options Form display
	if (pgp_InstallVersion!="")
		OptionsForm->PgpDirectoriesGroupbox->Caption="PGP Directories (pgp "+pgp_InstallVersion+" found)";
	else
		OptionsForm->PgpDirectoriesGroupbox->Caption="PGP Directories";
	OptionsForm->UpdateOptions();

	// return success
	initialized=true;

	// start the self threader
	SelfRunThreadStart();

	return true;
}


void MPGPJobManager::DeInitialize()
{
	// if already uninitialized, exit
	if (!initialized)
		return;

	// stopt the self threader
	SelfRunThreadEnd();

	// save ini settings
	if (initialized)
		{
		if (Pgpbase::PGPInitErrorCode==ieNone)
			{
			// This will cause crash in mirc due to blocking
			//			EnsureAnonymousKeyExists();
			if (pgp_KeyringsNeedRestore)
				RestorePgpKeyringLocs();
			SaveIniSettings();
			}
		}

	// kill all jobs
	CancelAllJobs();
	PurgeAllFinishedJobs();

	// kill input queue
	ClearInputQueue();
	ClearOutputQueue();

	// force main form to close
	wantsquit=true;

	// clear initialized flag
	initialized=false;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool MPGPJobManager::LoadIniSettings()
{
	// load settings to ini file
        TIniFile *ini;
	AnsiString fileversion;

	// open ini file
        ini = new TIniFile(configfilename);
        if (!ini)
        	{
		JMShowMessage("Error: could not open configuration file '"+configfilename+"' for loading options.");
                return false;
                }

	// load settings
	try
		{
		fileversion=ini->ReadString("Info","File Format Version","");
		// window pos+size
		PgpDialogForm->Left=ini->ReadInteger("Windows","MainLeft",PgpDialogForm->Left);
		PgpDialogForm->Top=ini->ReadInteger("Windows","MainTop",PgpDialogForm->Top);
		PgpDialogForm->Width=ini->ReadInteger("Windows","MainWidth",PgpDialogForm->Width);
		PgpDialogForm->Height=ini->ReadInteger("Windows","MainHeight",PgpDialogForm->Height);
		// splitters
		PgpDialogForm->PanelFrom->Height=ini->ReadInteger("Windows","PanelFromHeight",PgpDialogForm->PanelFrom->Height);
		// options
		OptionsForm->Left=ini->ReadInteger("Windows","OptionsLeft",OptionsForm->Left);
		OptionsForm->Top=ini->ReadInteger("Windows","OptionsTop",OptionsForm->Top);
		OptionsForm->Width=ini->ReadInteger("Windows","OptionsWidth",OptionsForm->Width);
		OptionsForm->Height=ini->ReadInteger("Windows","OptionsHeight",OptionsForm->Height);
		// log form
		LogForm->Left=ini->ReadInteger("Windows","LogLeft",LogForm->Left);
		LogForm->Top=ini->ReadInteger("Windows","LogTop",LogForm->Top);
		LogForm->Width=ini->ReadInteger("Windows","LogWidth",LogForm->Width);
		LogForm->Height=ini->ReadInteger("Windows","LogHeight",LogForm->Height);
		// job form
		JobForm->Left=ini->ReadInteger("Windows","JobLeft",JobForm->Left);
		JobForm->Top=ini->ReadInteger("Windows","JobTop",JobForm->Top);
		JobForm->Width=ini->ReadInteger("Windows","JobWidth",JobForm->Width);
		JobForm->Height=ini->ReadInteger("Windows","JobHeight",JobForm->Height);

		// job type
		int jt=ini->ReadInteger("Options","JobType",PgpDialogForm->TypeRadioGroup->ItemIndex);
		if (jt==1)
			{
			// wait a tiny bit before generating a rand chankey, to allow entropy gatherer to get some system entropy
			Sleep(500);
			}
		PgpDialogForm->TypeRadioGroup->ItemIndex=jt;
		// other options from options form
		OptionsForm->KeysizeRadioGroup->ItemIndex=ini->ReadInteger("Options","Keysize",OptionsForm->KeysizeRadioGroup->ItemIndex);
		OptionsForm->RandKeyRadioGroup->ItemIndex=ini->ReadInteger("Options","RandChanKeyLetters",OptionsForm->RandKeyRadioGroup->ItemIndex);
		OptionsForm->ArmouringRadioGroup->ItemIndex=ini->ReadInteger("Options","Armouring",OptionsForm->ArmouringRadioGroup->ItemIndex);
		OptionsForm->IncomingAnonymousRequests->ItemIndex=ini->ReadInteger("Options","IncomingAnonymous",OptionsForm->IncomingAnonymousRequests->ItemIndex);
		OptionsForm->KeyManagerEdit->Text=ini->ReadString("Options","KeyManagerTool",OptionsForm->KeyManagerEdit->Text);
		if (OptionsForm->KeyManagerEdit->Text=="")
			OptionsForm->KeyManagerEdit->Text=pgp_KeyManagerExefile;
		else if (!FileExists(OptionsForm->KeyManagerEdit->Text))
			OptionsForm->KeyManagerEdit->Text=pgp_KeyManagerExefile;
		OptionsForm->KeyringDirectoryEdit->Text=ini->ReadString("Options","KeyringDirectory",OptionsForm->KeyringDirectoryEdit->Text);
		OptionsForm->SpeedEdit->Text=ini->ReadString("Options","Speed",OptionsForm->SpeedEdit->Text);
		OptionsForm->TimerEdit->Text=ini->ReadString("Options","TimerInterval",OptionsForm->TimerEdit->Text);
		OptionsForm->BurstEdit->Text=ini->ReadString("Options","BurstBytes",OptionsForm->BurstEdit->Text);
		OptionsForm->TimeoutEdit->Text=ini->ReadString("Options","Timeout",OptionsForm->TimeoutEdit->Text);
		OptionsForm->AutoShowJobsCheckBox->Checked=ini->ReadBool("Options","AutoShowJobs",OptionsForm->AutoShowJobsCheckBox->Checked);
		OptionsForm->AutoHideJobsCheckBox->Checked=ini->ReadBool("Options","AutoHideJobs",OptionsForm->AutoHideJobsCheckBox->Checked);
		OptionsForm->AutoClearJobsCheckBox->Checked=ini->ReadBool("Options","AutoClearJobs",OptionsForm->AutoClearJobsCheckBox->Checked);

		//other values
		JobCounter=ini->ReadInteger("Values","JobCounter",JobCounter);
		// questoinform
		QuestionForm->Width=ini->ReadInteger("Windows","QuestionWidth",QuestionForm->Width);
		QuestionForm->Height=ini->ReadInteger("Windows","QuestionHeight",QuestionForm->Height);

		// pgpkeyring locations and emergency restore
		pgp_OrigPubring=ini->ReadString("Options","PgpOrigPubRing",pgp_OrigPubring);
		pgp_OrigSecring=ini->ReadString("Options","PgpOrigPubRing",pgp_OrigSecring);
		pgp_KeyringsNeedRestore=ini->ReadBool("Options","PgpKeyringsNeedResotre",pgp_KeyringsNeedRestore);
		}
	catch (...)
		{
		JMShowMessage("Error: write error while saving configuration file '"+configfilename+"'.");
                return false;
		}

	// we use a trick to repair pgp keyring locations if we crashed before restoring them
	if (pgp_KeyringsNeedRestore==true)
		{
		// this can only happen if we crashed before we restored the orig keyring locations
		RestorePgpKeyringLocs();
		}

	delete ini;
	return true;
}


bool MPGPJobManager::SaveIniSettings()
{
	// save settings to ini file
        TIniFile *ini;

	// open ini file
        ini = new TIniFile(configfilename);
        if (!ini)
        	{
		JMShowMessage("Error: could not open configuration file '"+configfilename+"' for saving options.");
                return false;
                }

	// save settings
	try
		{
		// save info
		ini->WriteString("Info","File Format Version","MPGP 2.0");
		// window pos+size
		ini->WriteInteger("Windows","MainLeft",PgpDialogForm->Left);
		ini->WriteInteger("Windows","MainTop",PgpDialogForm->Top);
		ini->WriteInteger("Windows","MainWidth",PgpDialogForm->Width);
		ini->WriteInteger("Windows","MainHeight",PgpDialogForm->Height);
		// splitters
		ini->WriteInteger("Windows","PanelFromHeight",PgpDialogForm->PanelFrom->Height);
		// options form
		ini->WriteInteger("Windows","OptionsLeft",OptionsForm->Left);
		ini->WriteInteger("Windows","OptionsTop",OptionsForm->Top);
		ini->WriteInteger("Windows","OptionsWidth",OptionsForm->Width);
		ini->WriteInteger("Windows","OptionsHeight",OptionsForm->Height);
		// log form
		ini->WriteInteger("Windows","LogLeft",LogForm->Left);
		ini->WriteInteger("Windows","LogTop",LogForm->Top);
		ini->WriteInteger("Windows","LogWidth",LogForm->Width);
		ini->WriteInteger("Windows","LogHeight",LogForm->Height);
		// job form
		ini->WriteInteger("Windows","JobLeft",JobForm->Left);
		ini->WriteInteger("Windows","JobTop",JobForm->Top);
		ini->WriteInteger("Windows","JobWidth",JobForm->Width);
		ini->WriteInteger("Windows","JobHeight",JobForm->Height);
		// job type
		ini->WriteInteger("Options","JobType",PgpDialogForm->TypeRadioGroup->ItemIndex);
		// other options from options form
		ini->WriteInteger("Options","Keysize",OptionsForm->KeysizeRadioGroup->ItemIndex);
		ini->WriteInteger("Options","RandChanKeyLetters",OptionsForm->RandKeyRadioGroup->ItemIndex);
		ini->WriteInteger("Options","Armouring",OptionsForm->ArmouringRadioGroup->ItemIndex);
		ini->WriteInteger("Options","IncomingAnonymous",OptionsForm->IncomingAnonymousRequests->ItemIndex);
		if (OptionsForm->KeyManagerEdit->Text != pgp_KeyManagerExefile)
			ini->WriteString("Options","KeyManagerTool",OptionsForm->KeyManagerEdit->Text);
		else
			ini->WriteString("Options","KeyManagerTool","");
		ini->WriteString("Options","KeyringDirectory",OptionsForm->KeyringDirectoryEdit->Text);
		ini->WriteString("Options","Speed",OptionsForm->SpeedEdit->Text);
		ini->WriteString("Options","TimerInterval",OptionsForm->TimerEdit->Text);
		ini->WriteString("Options","BurstBytes",OptionsForm->BurstEdit->Text);
		ini->WriteString("Options","Timeout",OptionsForm->TimeoutEdit->Text);
		ini->WriteBool("Options","AutoShowJobs",OptionsForm->AutoShowJobsCheckBox->Checked);
		ini->WriteBool("Options","AutoHideJobs",OptionsForm->AutoHideJobsCheckBox->Checked);
		ini->WriteBool("Options","AutoClearJobs",OptionsForm->AutoClearJobsCheckBox->Checked);
		//other values
		ini->WriteInteger("Values","JobCounter",JobCounter);
		// questoinform
		ini->WriteInteger("Windows","QuestionWidth",QuestionForm->Width);
		ini->WriteInteger("Windows","QuestionHeight",QuestionForm->Height);
		// pgpkeyring locations and emergency restore
		ini->WriteString("Options","PgpOrigPubRing",pgp_OrigPubring);
		ini->WriteString("Options","PgpOrigPubRing",pgp_OrigSecring);
		ini->WriteBool("Options","PgpKeyringsNeedResotre",pgp_KeyringsNeedRestore);
		}
	catch (...)
		{
		JMShowMessage("Error: write error while saving configuration file '"+configfilename+"'.");
                return false;
		}

	delete ini;
	return true;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void MPGPJobManager::PresentForm(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString ichankeyphrase,AnsiString ifromident)
{
	// present the main dialog

	// save caller presentation info
	Global_tochan=itochan;
	Global_tonick=itonick;
	Global_toident=itoident;
	ChannelKeyphrase=ichankeyphrase;
	Global_fromident=ifromident;

	// fix blank channel key as "."
	if (ChannelKeyphrase==".")
		ChannelKeyphrase="";

	// update main form with tochan info
	AnsiString tolabel=Global_tonick;
	if (Global_tochan!="")
		tolabel=Global_tochan+":"+tolabel;
	if (Global_toident!="")
		tolabel=tolabel + " aka "+Global_toident;
		PgpDialogForm->ToGroupBox->Caption = "To ("+ tolabel+ ")";

	// initialize the current job type
	PgpDialogForm->TypeRadioGroupClick(NULL);

	// show the main form
	PgpDialogForm->Show();

	// dont work - need to make form stayontop
	Application->BringToFront();

	// start the self threader
//	SelfRunThreadStart();
}


void MPGPJobManager::RunTest()
{
	// loop
	Application->Run();
}

void MPGPJobManager::SelfRunThreadStart()
{
	// start a self running thread to process dialog
	if (selfthreaderp==NULL)
		selfthreaderp=new SelfThreader();
}

void MPGPJobManager::SelfRunThreadEnd()
{
	// kill the self running thread
	if (selfthreaderp!=NULL)
		{
                //		selfthreaderp->Suspend();
		selfthreaderp->Terminate();
                // it will self delete
                //		delete selfthreaderp;
		}
	selfthreaderp=NULL;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// SIMPLE PGP KEY MANAGEMENT ROUTINES

bool MPGPJobManager::InitPgpTools()
{
	// initialize pgp and retrieve some flie locations, etc.
	TRegistry *regkey;
	AnsiString seckeyname="SOFTWARE\\Network Associates\\PGP";
	bool keyexists;
	bool bretv=false;

	try
		{
	        regkey= new TRegistry;
	        regkey->RootKey=HKEY_LOCAL_MACHINE;
		keyexists=regkey->OpenKey(seckeyname,false);
		if (keyexists)
			{
			if (regkey->ValueExists("InstallPath"))
				{
				pgp_InstallPath=regkey->ReadString("InstallPath");
				pgp_KeyManagerExefile=pgp_InstallPath+"PGPKeys.exe";
				bretv=true;
				}
			if (regkey->ValueExists("Version"))
				pgp_InstallVersion=regkey->ReadString("Version");
			}
		}
	catch(Exception &e)
		{
		// exceptions are thrown trying to read non-existent keys
		}
	delete regkey;

	if (bretv==false)
		{
		// pgp not installed but local copy exists, and has not had chance to run yet?
		pgp_KeyManagerExefile="PGPKeys.exe";
		if (FileExists(pgp_KeyManagerExefile))
			{
			ShellExecute(NULL, "open", pgp_KeyManagerExefile.c_str(), NULL, NULL, SW_SHOWNORMAL);
			JMShowMessage("An exisitng installation of PGP was not found on this computer.  Trying to initialize local copy...\nIf the PGP key manager has successfully launched, configure your options and close it.\nOtherwise, please install PGP 6.5x+ or reinstall the mircryption PGP Addon.");
			}
		}

	// get keyring locations
	PgpDialogForm->PGPPreferences1->PGPPrefs.Clear();
//	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_DefaultKeyID;
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PublicKeyring;
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PrivateKeyring;
	PgpDialogForm->PGPPreferences1->DoGetPreferences();
	pgp_OrigPubring=PgpDialogForm->PGPPreferences1->PublicKeyring;
	pgp_OrigSecring=PgpDialogForm->PGPPreferences1->PrivateKeyring;

	return bretv;
}


void MPGPJobManager::SetAltPgpKeyrings()
{
	// set alternative temporary pgp settings (alternative keyrings, etc.)
	AnsiString pubring,secring;
	int retv1,retv2;
	bool bretv;
	AnsiString keyloc;

	PgpDialogForm->PGPPreferences1->AltPrivKeyring="";
	PgpDialogForm->PGPPreferences1->AltPubKeyring="";

	keyloc=OptionsForm->KeyringDirectoryEdit->Text;
	if (keyloc!="")
		{
		// set keyrings manually
		if (keyloc.Pos(":")<=0)
			{
			// add current directory if it is not explicit
			keyloc=basedir+keyloc;
			}

		pubring=keyloc+"\\pubring.pkr";
		secring=keyloc+"\\secring.skr";
		// remove double \\
		JRStringReplace(pubring,"\\\\","\\");
		JRStringReplace(secring,"\\\\","\\");
		retv1=JRMakeSureFileExists(pubring);
		retv2=JRMakeSureFileExists(secring);
		if (retv1!=-1 && retv2!=-1)
			{
			PgpDialogForm->PGPPreferences1->AltPubKeyring=pubring;
			PgpDialogForm->PGPPreferences1->AltPrivKeyring=secring;
			// in pgp, if we just created the keyrings, we need to force a reset
			PgpDialogForm->ListBoxFrom->Clear();
			PgpDialogForm->ListBoxTo->Clear();
			PgpDialogForm->PGPUpdateKeyrings();
			}
		else
			{
			PgpDialogForm->PGPPreferences1->AltPubKeyring="";
			PgpDialogForm->PGPPreferences1->AltPrivKeyring="";
			}
		}

	// will cause crash in mirc due to blocking
	//	Initial_EnsureAnonymousKeyExists();

	// force update of listboxes
	PgpDialogForm->PGPUpdateKeyrings();
}

void MPGPJobManager::Initial_EnsureAnonymousKeyExists()
{
	EnsureAnonymousKeyExists();
	// force update of listboxes
	PgpDialogForm->PGPUpdateKeyrings();
	DidInitialEnsureAnonymous=true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// SIMPLE PGP KEY MANAGEMENT ROUTINES

void MPGPJobManager::MakeRandomChanKey(bool forcenew)
{
	int retv;
	if (ChannelKeyphrase=="" || ChannelKeyphrase.Pos("[RAND]")==1 || forcenew)
		{
		// generate a new key channel
		if (OptionsForm->RandKeyRadioGroup->ItemIndex==0)
			retv=PgpDialogForm->JYarrow1->MakeRandPrettyString(RandomKeyphraseLength);
		else
			retv=PgpDialogForm->JYarrow1->MakeIrcSafeRandString(RandomKeyphraseLength);
		if (retv==0)
			ChannelKeyphrase="[RAND]"+(PgpDialogForm->JYarrow1->ResultStr);
		else
			ChannelKeyphrase="[ERROR]"+(PgpDialogForm->JYarrow1->ResultStr);
		}
}


AnsiString MPGPJobManager::MakeRandomProofString()
{
	// make a random proofstring, using a random number plus current time/date
	AnsiString str;
	int retv;

	// date
	str=DateTimeToStr(TDateTime::CurrentDateTime());

	// rand component
	retv=PgpDialogForm->JYarrow1->MakeRandPrettyString(RandomProofStringLength);
	if (retv==0)
		{
		// got a rand string
		str+=" ("+PgpDialogForm->JYarrow1->ResultStr+")";
		}
	else
		{
		// random string generator failed
		// its really not a big deal if we dont have a ranom number, the date should be enough to prevent spoofing signatures
		}
	return str;
}


void MPGPJobManager::MakeNewAnonymousKey()
{
	int retv;
	AnsiString anonkeyid=GetPgpOurAnonHexId();
	PgpDialogForm->PGPKeysGenerate->UserName=AnonymousMircryptionKeyId;

	// delete any existing key
	PgpDialogForm->PGPGetKeyPropsPublic->KeyID=anonkeyid;
	retv=PgpDialogForm->PGPGetKeyPropsPublic->KeyIsOnRing();
	if (retv >=0 )
		{
		// anon key is on ring, so now delete
		PgpDialogForm->PGPSetKeyProps->KeyID=anonkeyid;
		PgpDialogForm->PGPSetKeyProps->DoKeyRemove();
		// update key listboxes
		PgpDialogForm->PGPUpdateKeyrings();
		}

	// set pgp key generation options
	PgpDialogForm->PGPKeysGenerate->EmailAddress="";
	switch(OptionsForm->KeysizeRadioGroup->ItemIndex)
		{
		case 0:
		defalut:
			// 2048bits
			// version 323 of pgpdc
			//		PgpDialogForm->PGPKeysGenerate->SubKeySize=SubKeySize_2048;
			// version 330 of pgpdc
			PgpDialogForm->PGPKeysGenerate->SubKeySize=2048;
			break;
		case 1:
			// 4096 bits
			// version 323 of pgpdc
			//		PgpDialogForm->PGPKeysGenerate->SubKeySize=SubKeySize_4096;
			// version 330 of pgpdc
			PgpDialogForm->PGPKeysGenerate->SubKeySize=4096;
			break;
		}

	set_BlockedForInput(true);
	PgpDialogForm->PGPKeysGenerate->DHDSSKeyGenerate();
	set_BlockedForInput(false);
}


void MPGPJobManager::LaunchKeyManager()
{
	// Here we just want to launch the key manager tool
	TempChangePgpKeyringLocs();
	AnsiString keytoolexe=OptionsForm->KeyManagerEdit->Text;
	ShellExecute(NULL, "open", keytoolexe.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void MPGPJobManager::TempChangePgpKeyringLocs()
{
	// temporarily change permanent locations of pgp keyrings
	PgpDialogForm->PGPPreferences1->PGPPrefs.Clear();
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PublicKeyring;
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PrivateKeyring;

	if (PgpDialogForm->PGPPreferences1->AltPubKeyring=="" || PgpDialogForm->PGPPreferences1->AltPrivKeyring=="")
		return;
	if (PgpDialogForm->PGPPreferences1->AltPubKeyring!="")
		PgpDialogForm->PGPPreferences1->PublicKeyring=PgpDialogForm->PGPPreferences1->AltPubKeyring;
	if (PgpDialogForm->PGPPreferences1->AltPrivKeyring!="")
		PgpDialogForm->PGPPreferences1->PrivateKeyring=PgpDialogForm->PGPPreferences1->AltPrivKeyring;
	PgpDialogForm->PGPPreferences1->DoSetPreferences();
	pgp_KeyringsNeedRestore=true;

	// now save ini settings in case of crash
	SaveIniSettings();
}


void MPGPJobManager::RestorePgpKeyringLocs()
{
	// restore orig locations of pgp keyrings
	PgpDialogForm->PGPPreferences1->PGPPrefs.Clear();
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PublicKeyring;
	PgpDialogForm->PGPPreferences1->PGPPrefs << Pref_PrivateKeyring;

	if (PgpDialogForm->PGPPreferences1->AltPubKeyring=="" || PgpDialogForm->PGPPreferences1->AltPrivKeyring=="")
		return;
	PgpDialogForm->PGPPreferences1->PublicKeyring=pgp_OrigPubring;
	PgpDialogForm->PGPPreferences1->PrivateKeyring=pgp_OrigSecring;
	PgpDialogForm->PGPPreferences1->DoSetPreferences();
	pgp_KeyringsNeedRestore=false;

	// now save ini settings in case of crash
	SaveIniSettings();
}


bool MPGPJobManager::EnsureAnonymousKeyExists()
{
	// if anonymous key does not exist to sign, then make user create it, or return false on cancel
	int retv;
	AnsiString anonkeyid;

	// does anonymous key exist?
	anonkeyid=GetPgpOurAnonHexId();
	if (anonkeyid!="")
		return true;
//	PgpDialogForm->PGPGetKeyPropsPublic->KeyID=anonkeyid;
//	retv=PgpDialogForm->PGPGetKeyPropsPublic->KeyIsOnRing();
//	if (retv >=0)
//		return true;


	// make it
	JMShowMessage("The Mircryption PGP Addon needs you to generate an anonymous pgp keypair so that you can receive encrypted messages.\nClick OK to generate an anonymous keypair now.");
	jobmanager->MakeNewAnonymousKey();

	// does anonymous key exist?
	Sleep(500);
	anonkeyid=GetPgpOurAnonHexId();
//	PgpDialogForm->PGPGetKeyPropsPublic->KeyID=anonkeyid;
//	retv=PgpDialogForm->PGPGetKeyPropsPublic->KeyIsOnRing();
	if (anonkeyid!="")
		{
		JMShowMessage("Your anonymous keypair was successfully generated.\nYou can replace this keypair whenever you want by clicking the New Anonymous Keypair button.\nYou can create non-anonymous keys and import other keys using the key manager tool.");
		if (PgpDialogForm->PGPPreferences1->AltPubKeyring!="" && PgpDialogForm->PGPPreferences1->AltPrivKeyring!="")
			JMShowMessage("NOTE: Your current options specify the use of alternate (non-default) keyfiles which were just now created.\nTo import pgp keys from your normal keyfiles, click the Key Manager button and use the Keys->Import option.");
		return true;
		}
	JMShowMessage("NOTE: An anonymous keypair was *not* generated - you will not be able to receive encrypted messages until you generate one.");
	if (PgpDialogForm->PGPPreferences1->AltPubKeyring!="" && PgpDialogForm->PGPPreferences1->AltPrivKeyring!="")
		{
		// special message when they try to use alternative keys
		if (PgpDialogForm->ListBoxFrom->Items->Count>1 && PgpDialogForm->ListBoxTo->Items->Count>1)
			JMShowMessage("NOTE: Your current options specify the use of alternate (non-default) keyfiles which were just now created.\nBecause generation of your anonymous keypair was aborted, your existing normal PGP keys will be displayed until you make your first mircryption keypair in the new keyfiles.");
		}

	return false;
}


AnsiString MPGPJobManager::GetPgpHexId(AnsiString keyname)
{
	// get hex id of keyname
	bool bretv;
	int retv;
	int index;

	if (PgpDialogForm->PGPGetKeyPropsPrivate->KeyRingCount()<=0)
		{
		// ShowMessage("err1: no keyringcount");
		return "";
		}

	// we used to look up the key by calling: index=PgpDialogForm->ListBoxPrivInternal->Items->IndexOf(keyname);
	// BUT it seems the new pgpdc v330 has changed the names of keys in our internal listbox, so we need to be smarter and iterate through ourselves
	index=PgpDialogForm->ListBoxPrivInternal->Items->IndexOf(keyname);
	if (index<0)
		{
		// more sophisticated search
		int listcount=PgpDialogForm->ListBoxPrivInternal->Items->Count;
		for (index=0;index<listcount;++index)
			{
			if (PgpDialogForm->ListBoxPrivInternal->Items->Strings[index].Pos(keyname+" (")==1)
				break;
			}
		if (index>=listcount)
			index=-1;
		}

	if (index<0)
		{
		// ShowMessage("err2: no index for "+keyname);
		return "";
		}

	PgpDialogForm->PGPGetKeyPropsPrivate->KeyID=keyname;
	PgpDialogForm->PGPGetKeyPropsPrivate->Update();

	// long hex id - why does borland c++ builder give an erroneous error about kpl not being used?
	TKeyPropsList *kpl=PgpDialogForm->PGPGetKeyPropsPrivate->RingPropsList;
	GlobalHexId=kpl->Strings[index];
	GlobalKeynames=keyname;

	// ShowMessage("global hexid long="+GlobalHexId);

	// short hex id
	/*
	TKeyPropsRec kpr;
	bretv=kpl->GetKeyPropsRec(kpr,index);
	if (bretv==true)
		GlobalHexId=kpr.kHexID;
	else
		GlobalHexId="";
	ShowMessage("global hexid short="+GlobalHexId);
	*/

	return GlobalHexId;
}

AnsiString MPGPJobManager::GetPgpOurAnonHexId()
{
	// return the hexid for our anonymous key
	AnsiString str;

	str=GetPgpHexId(AnonymousMircryptionKeyId);
	return str;
}

AnsiString MPGPJobManager::GetPgpOurAnonPubKey()
{
	// return the full exported keydata for our anonymous key
	AnsiString str;
// old using userid for anon
//	str=GetPgpKeyData(AnonymousMircryptionKeyId);
//	LogMessage("$ IN MPGPJobManager::GetPgpOurAnonPubKey");
	str=GetPgpKeyData(GetPgpOurAnonHexId());
	return str;
}

AnsiString MPGPJobManager::GetPgpKeyData(AnsiString keylist)
{
	// return the full exported keydata for our anonymous key
	int retv;
	AnsiString str;
	AnsiString keydesc;

//	jobmanager->LogMessage("starting getpgpkeydata.");

	keylist=keylist.Trim();
	PgpDialogForm->PGPKeyExport->KeyIDs->Clear();
	while (JrLeftStringSplit(keylist,keydesc,"\n"))
		{
		keydesc=keydesc.Trim();
		if (keydesc!="")
			{
			PgpDialogForm->PGPKeyExport->KeyIDs->Add(keydesc);
			}
		}

	PgpDialogForm->PGPKeyExport->KeyData="";
	PgpDialogForm->PGPKeyExport->KeyDlgPrompt="Select the keys you want to export";
	set_BlockedForInput(true);
	retv=PgpDialogForm->PGPKeyExport->DoKeyExport();
	set_BlockedForInput(false);
	if (retv==0)
		str=PgpDialogForm->PGPKeyExport->KeyData;
	else
		str="";
	return str;
}

bool MPGPJobManager::ImportKeyData(AnsiString keydata,AnsiString &outstring,AnsiString dialogstr)
{
	// import key data into public ring
	int retv;
	set_GlobalHexId("");
	set_GlobalKeynames("");
	PgpDialogForm->PGPKeyImport->KeyData=keydata;
	PgpDialogForm->PGPKeyImport->KeyDlgPrompt=dialogstr;
	set_BlockedForInput(true);
	retv=PgpDialogForm->PGPKeyImport->DoKeyImport();
	set_BlockedForInput(false);
	if (retv==0)
		outstring=jobmanager->get_GlobalKeynames();
	else
		outstring="";
	return true;
}

bool MPGPJobManager::ImportAnonKeyData(AnsiString keydata,AnsiString &outhexid)
{
	// import key data into public ring
	// ATTN: unfinished - we need to make sure we do NOT impor any non-anonymous keys
	int retv;

	set_GlobalHexId("");
	set_GlobalKeynames("");
	PgpDialogForm->PGPKeyImport->KeyData=keydata;
	PgpDialogForm->PGPKeyImport->KeyDlgPrompt="";
	set_BlockedForInput(true);
	retv=PgpDialogForm->PGPKeyImport->DoKeyImport();
	set_BlockedForInput(false);
	if(retv==0)
		outhexid=jobmanager->get_GlobalHexId();
	else
		outhexid="";

	// verify integrity of anonymous keys
	// a user has sent us their anonymous public key.
	// HOWEVER, it is possible they may try to trick us by sending us keys that are not named anonymous
	// and thus could be used by us by accident.  So we verify here that they are named appropriately
	AnsiString strk,strks;
	strks=GlobalKeynames;
	bool badkeys=false;
	AnsiString akeyname=AnsiString(AnonymousMircryptionKeyId);

	while (JrLeftStringSplit(strks,strk,","))
		{
		if (strk!=akeyname)
			{
			// got an illegal publickey name
			QuestionForm->PresentOk("","ERROR: While attempting import of anonymous key, encountered an ILLEGAL KEY.  Recently imported key is being eliminated.\r\n\r\nPlease check your public keyring anyway.",jobmanager->MessageTimeout(),"");
			badkeys=true;
			break;
			}
		}
	if (badkeys)
		{
		// delete all recently imported key
		strks=GlobalHexIds;
		while (JrLeftStringSplit(strks,strk,","))
			{
			// delete key with hexid strk
			PgpDialogForm->PGPSetKeyProps->KeyID=strk;
			PgpDialogForm->PGPSetKeyProps->DoKeyRemove();
			}
		// update key listboxes
		PgpDialogForm->PGPUpdateKeyrings();
		return false;
		}

	return true;
}

bool MPGPJobManager::IsPgpKeyAnonymous(AnsiString keydesc)
	{
	// ATTN: unfinished - we need to lookup hexids and see if their corresponding username are _mcps_anon
	// be smart about figuring out if the keydesc (either hexid or user name) is anonymous or not
	if (keydesc.Pos(AnonymousMircryptionKeyId)==1 || keydesc.Pos(AnonymousMircryptionKeyIdPrefix)==1)
		return true;
	return false;
	}


bool MPGPJobManager::PgpGoodEnoughValidity(AnsiString validstr)
{
	// return true if the validity level string is sufficient (see GetValidityString())
	if (validstr=="valid" || validstr=="marginally valid")
		return true;
	return false;
}


AnsiString MPGPJobManager::PrettySigNick(AnsiString nick, bool showanon)
{
	// this is aclled after a decryption routines
	// it used the global signatures recovered during decryption to make a string showing verified signatures
	AnsiString str=nick;

	if (GlobalSig!="")
		str+=" (signed as "+GlobalSig+", "+GlobalSigValidity+")";
	else
		{
		if (showanon)
			str+=" (anonymous)";
		}

	return str;
}

void MPGPJobManager::SetPgpWindowHandle(THandle parenthandle)
{
	// set the parent handle for modal display of pgp components over mirc window
	PgpDialogForm->PGPKeysGenerate->ParentHandle=parenthandle;
	PgpDialogForm->PGPKeyImport->ParentHandle=parenthandle;
//	PgpDialogForm->PGPKeyExport->ParentHandle=parenthandle;
	PgpDialogForm->PGPDecode->ParentHandle=parenthandle;
	PgpDialogForm->PGPEncode->ParentHandle=parenthandle;
}

void MPGPJobManager::PgpFileOperationError()
{
	QuestionForm->PresentOk("PGP ERROR","Attention: Operation was NOT completed.\r\n\r\nThe PGP DLL was not able to access the keyfile to perform the command.  This is a bug we know about but are still actively trying to track down and remove.  I'm afraid the only thing you can do to fix it is close this window and then tell mirc to unload the Mpgp DLL from memory (via right click in any window), and then try again.  We apologize and are working to solve this problem.",jobmanager->MessageTimeout(),"");
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
bool MPGPJobManager::EncryptString(AnsiString FromId,AnsiString ToId,AnsiString instring,AnsiString &outstring,AnsiString PassPrompt)
{
	// encrypt string
	// return true on success

	// set encrypting component values
	PgpDialogForm->PGPEncode->FileOutput=false;
	PgpDialogForm->PGPEncode->EncryptKeyIDs->SetText(ToId.c_str());

	if (FromId!=AnonymousMircryptionKeyId)
		PgpDialogForm->PGPEncode->SignKeyID=GetPgpHexId(FromId);
	else
		PgpDialogForm->PGPEncode->SignKeyID="";

	// other options
	PgpDialogForm->PGPEncode->PassDlgPrompt=PassPrompt;
	PgpDialogForm->PGPEncode->FormatOptions.Clear();
	if (OptionsForm->ArmouringRadioGroup->ItemIndex==0)
		PgpAsciiArmour=true;
	else
		PgpAsciiArmour=false;
	if (PgpAsciiArmour)
		{
		// set ascii armour mode
		PgpDialogForm->PGPEncode->FormatOptions << Format_Armor;
		//	PgpDialogForm->PGPEncode->FormatOptions << Format_Textmode;
		}

	// ecnrypt it
	GlobalCodedOutput="";
	set_BlockedForInput(true);
	if (PgpDialogForm->PGPEncode->SignKeyID!="")
		PgpDialogForm->PGPEncode->KeyEncryptBuffer(instring,true);
	else
		PgpDialogForm->PGPEncode->KeyEncryptBuffer(instring,false);
	set_BlockedForInput(false);

	// result is returned in this global var
	if (GlobalCodedOutput=="")
		return false;
	outstring=GlobalCodedOutput;
	return true;
}


bool MPGPJobManager::EncryptFile(AnsiString FromId,AnsiString ToId,AnsiString infilename,AnsiString &outfilename,AnsiString PassPrompt)
{
	// encrypt file
	// return true on success
	int retv;

	// set encrypting component values
	PgpDialogForm->PGPEncode->FileOutput=true;
	PgpDialogForm->PGPEncode->OutputFileName=outfilename;
	PgpDialogForm->PGPEncode->EncryptKeyIDs->SetText(ToId.c_str());
	if (FromId!=AnonymousMircryptionKeyId)
		PgpDialogForm->PGPEncode->SignKeyID=GetPgpHexId(FromId);
	else
		PgpDialogForm->PGPEncode->SignKeyID="";
	PgpDialogForm->PGPEncode->PassDlgPrompt=PassPrompt;
	PgpDialogForm->PGPEncode->FormatOptions.Clear();
	if (OptionsForm->ArmouringRadioGroup->ItemIndex==0)
		PgpAsciiArmour=true;
	else
		PgpAsciiArmour=false;
	if (PgpAsciiArmour)
		{
		PgpDialogForm->PGPEncode->FormatOptions << Format_Armor;
		//	PgpDialogForm->PGPEncode->FormatOptions << Format_Textmode;
		}

	set_BlockedForInput(true);
	if (PgpDialogForm->PGPEncode->SignKeyID!="")
		retv=PgpDialogForm->PGPEncode->KeyEncryptFile(infilename,true);
	else
		retv=PgpDialogForm->PGPEncode->KeyEncryptFile(infilename,false);
	set_BlockedForInput(false);

	// result is returned in this global var
	if (retv!=0)
		return false;
	return true;
}

bool MPGPJobManager::DecryptString(AnsiString FromId,AnsiString ToId,AnsiString instring,AnsiString &outstring,AnsiString passprompt,bool addkeys)
{
	// descrypt a string.  instring is encrypted. outstring returns with decrypted result
	int retv;

	PgpDialogForm->PGPDecode->FileOutput=false;
	PgpDialogForm->PGPDecode->OutputFileName="";
	PgpDialogForm->PGPDecode->PassDlgPrompt=passprompt;
	PgpDialogForm->PGPDecode->KeyDlgPrompt=passprompt;
	PgpDialogForm->PGPDecode->QueryAddKeys=addkeys;
	GlobalFilename="";
	GlobalSig="";
	GlobalCodedOutput="";
	GlobalKeynames="";

	set_BlockedForInput(true);
	retv=PgpDialogForm->PGPDecode->DecodeBuffer(instring);
	set_BlockedForInput(false);

	if (retv!=0 || GlobalCodedOutput=="")
		{
		// failed to decode
//		JMShowMessage("decodebuffer failed with retv=="+AnsiString(retv)+" and str="+GlobalCodedOutput);
		outstring="";
		return false;
		}
	// succeeded in decoding, set outstring to decrypted result
	outstring=GlobalCodedOutput;
	return true;
}

bool MPGPJobManager::DecryptFile(AnsiString FromId,AnsiString ToId,AnsiString infilename,AnsiString &outfilename,AnsiString passprompt,bool addkeys)
{
	// decrypt a file.  instring comes in as crypted filename, goes out as extracted filename
	int retv;
	PgpDialogForm->PGPDecode->FileOutput=true;
	PgpDialogForm->PGPDecode->OutputFileName="";
	PgpDialogForm->PGPDecode->PassDlgPrompt=passprompt;
	PgpDialogForm->PGPDecode->QueryAddKeys=addkeys;
	GlobalFilename="";
	GlobalSig="";
	GlobalCodedOutput="";

	set_BlockedForInput(true);
	retv=PgpDialogForm->PGPDecode->DecodeFile(infilename);
	set_BlockedForInput(false);

	if (retv!=0 || PgpDialogForm->PGPDecode->OutputFileName=="")
		{
		// failed to decode
		return false;
		}
	// succeeded in decoding, set outstring to decrypted destination filename
	outfilename=PgpDialogForm->PGPDecode->OutputFileName;
	return true;
}
//---------------------------------------------------------------------------

























//---------------------------------------------------------------------------
// Job Management
void MPGPJobManager::SuspendJob(MPGPJob *jobp)
{
	if (jobp==NULL)
		return;
	jobp->Suspend();
}

void MPGPJobManager::ResumeJob(MPGPJob *jobp)
{
	if (jobp==NULL)
		return;
	jobp->Resume();
}

void MPGPJobManager::CancelJob(MPGPJob *jobp,AnsiString cancelreasonstr)
{
	if (jobp==NULL)
		return;
	jobp->Cancel(cancelreasonstr);
}

bool MPGPJobManager::IsJobSuspended(MPGPJob *jobp)
{
	// returns true if job index is suspended and can be resumed
	if (jobp==NULL)
		return false;
	return jobp->IsSuspended();
}

bool MPGPJobManager::IsJobStopped(MPGPJob *jobp)
{
	// returns true if job index is suspended and can be resumed
	if (jobp==NULL)
		return false;
	return jobp->IsStopped();
}

bool MPGPJobManager::IsJobOutputting(MPGPJob *jobp)
{
	// returns true if job index is suspended and can be resumed
	if (jobp==NULL)
		return false;
	return jobp->IsOutputting();
}

bool MPGPJobManager::IsJobConfirmed(MPGPJob *jobp)
{
	// returns true if job index is suspended and can be resumed
	if (jobp==NULL)
		return false;
	return jobp->IsConfirmed();
}

bool MPGPJobManager::IsJobCanceled(MPGPJob *jobp)
{
	// returns true if job index is suspended and can be resumed
	if (jobp==NULL)
		return false;
	return jobp->IsCanceled();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void MPGPJobManager::CancelAllJobs()
{
	// cancel all running jobs
	// walk through job list and cancel all of them
	MPGPJob *jobp=Jfirstp;
	while (jobp!=NULL)
		{
		CancelJob(jobp,"cancel");
		jobp=jobp->nextp;
		}
}

void MPGPJobManager::SuspendAllJobs()
{
	// cancel all running jobs
	// walk through job list and cancel all of them
	MPGPJob *jobp=Jfirstp;
	while (jobp!=NULL)
		{
		SuspendJob(jobp);
		jobp=jobp->nextp;
		}
}

void MPGPJobManager::PurgeAllFinishedJobs()
{
	// remove all finished (whether by success or error) jobs
	// NOTE: we only do this for toplevel jobs.  Finished children of unfinished jobs are not purged
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;

	while (jobp!=NULL)
		{
		nextjobp=jobp->nextp;
		if ((jobp->IsConfirmed() && !IsJobOutputting(jobp)) || jobp->IsCanceled() || jobp->IsExpired())
			RecursivelyDeleteJob(jobp);
		jobp=nextjobp;
		}
	SleepIfNoJobs();
}

void MPGPJobManager::SleepIfNoJobs()
{
	if (wantsquit)
		return;
	if (Jfirstp==NULL)
		{
		// all jobs are gone - inform the dll that it can sleep us if it wants
		if (OptionsForm->AutoHideJobsCheckBox->Checked==true && JobForm->Visible==true)
			JobForm->Hide();
		DLLCanSleep();
		}
}

void MPGPJobManager::RecursivelyDeleteJob(MPGPJob *jobp)
{
	// delete the job and all children
	// be careful about updating lastp pointer
	MPGPJob *childp;
	MPGPJob *nextjobp;
	
	// first children
	childp=jobp->childp;
	while (childp!=NULL)
		{
		nextjobp=childp->nextp;
		RecursivelyDeleteJob(childp);
		childp=nextjobp;
		}

	// if we are about to delete the last one, reassign Jlastp
	if (Jfirstp==jobp)
		Jfirstp=jobp->nextp;
	if (Jlastp==jobp)
		Jlastp=jobp->priorp;

	// adjust pointers to splice out current job
	if (jobp->priorp!=NULL)
		(jobp->priorp)->nextp=jobp->nextp;
	if (jobp->nextp!=NULL)
		(jobp->nextp)->priorp=jobp->priorp;

	// now the job itself
	JobsHaveChanged=true;
	delete jobp;

	if (Jfirstp==NULL && OptionsForm->AutoHideJobsCheckBox->Checked==true && JobForm->Visible==true)
		JobForm->Hide();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
MPGPJob *MPGPJobManager::LookupJob(int index)
{
	// lookup a job, as indexed in the job listbox
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextp;
	int count=-1;

	while (++count<index && jobp!=NULL)
		{
		// next in line is a child
		nextp=jobp->childp;
		// no child, then the next in line (brother)
		if (nextp==NULL)
			nextp=jobp->nextp;
		// no child or brother, then pop back up to parent's brother
		if (nextp==NULL)
			{
			if (jobp->parentp!=NULL)
				{
				do
					{
					jobp=jobp->parentp;
					nextp=jobp->nextp;
					} while (jobp!=NULL && nextp==NULL);
				}
			}
		jobp=nextp;
		}
	return jobp;
}


MPGPJob *MPGPJobManager::LookUpMatchingJob(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype, int jobdir)
{
	// lookup a job, as indexed in the job listbox
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextp;

	while (jobp!=NULL)
		{
		// is this our job? it must match id and type
		if (jobp->jobid==jobid && jobp->jobtype==jobtype)
			{
			// must match tonick and ident
			if (jobp->tonick==tonick && jobp->toident==toident)
				{
				// must match the direction sought
				if (jobp->direction==jobdir || jobdir==JOBDIR_UNDEFINED)
					return jobp;
				}
			}

		// next in line is a child
		nextp=jobp->childp;
		// no child, then the next in line (brother)
		if (nextp==NULL)
			nextp=jobp->nextp;
		// no child or brother, then pop back up to parent's brother
		if (nextp==NULL)
			{
			if (jobp->parentp!=NULL)
				{
				do
					{
					jobp=jobp->parentp;
					nextp=jobp->nextp;
					} while (jobp!=NULL && nextp==NULL && jobp->parentp!=NULL);
				}
			}
		jobp=nextp;
		}
	return jobp;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MPGPJobManager::ClearInputQueue()
{
	// delete all input queue items
	MessageQueue *qp;

	while (QIfirstp!=NULL)
		{
		qp=QIfirstp->nextp;
		delete QIfirstp;
		QIfirstp=qp;
		}
	QIlastp=NULL;
	return;
}


void MPGPJobManager::DeleteInputQueue(MessageQueue *qp)
{
	// Delete an input queue entry
	// splice it out
	if (qp->nextp!=NULL)
		(qp->nextp)->priorp=qp->priorp;
	if (qp->priorp!=NULL)
		(qp->priorp)->nextp=qp->nextp;
	// adjust first and last
	if (QIfirstp==qp)
		QIfirstp=qp->nextp;
	if (QIlastp==qp)
		{
		// we are at end
		QIlastp=qp->priorp;
		}
	// now delete and advance
	delete qp;
}


void MPGPJobManager::AddInputQueueToEnd(MessageQueue *qp)
{
	// add the job to the end and update first/lasp pointers
	if (QIfirstp==NULL)
		{
		// first in list
		QIfirstp=qp;
		QIlastp=qp;
		}
	else
		{
		// add to end
		qp->priorp=QIlastp;
		QIlastp->nextp=qp;
		QIlastp=qp;
		}
}


bool MPGPJobManager::IterateInputQueue()
{
	// iterate input queue
	// returns true if there are still input jobs left
	bool bretv=false;
	MessageQueue *qp=QIfirstp;
	MessageQueue *nextqp;

	while (qp!=NULL)
		{
		// process it
		ProcessInputString(qp->tochan,qp->tonick,qp->toident,qp->string);
		nextqp=qp->nextp;
		DeleteInputQueue(qp);
		qp=nextqp;
		bretv=true;
		}

	return bretv;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void MPGPJobManager::ClearOutputQueue()
{
	// delete all input queue items
	MessageQueue *qp;

	while (QOfirstp!=NULL)
		{
		qp=QOfirstp->nextp;
		delete QOfirstp;
		QOfirstp=qp;
		}
	QOlastp=NULL;
	return;
}


void MPGPJobManager::DeleteOutputQueue(MessageQueue *qp)
{
	// Delete an input queue entry
	// splice it out
	if (qp->nextp!=NULL)
		(qp->nextp)->priorp=qp->priorp;
	if (qp->priorp!=NULL)
		(qp->priorp)->nextp=qp->nextp;
	// adjust first and last
	if (QOfirstp==qp)
		QOfirstp=qp->nextp;
	if (QOlastp==qp)
		{
		// we are at end
		QOlastp=qp->priorp;
		}
	// now delete and advance
	delete qp;
}


void MPGPJobManager::AddOutputQueueToEnd(MessageQueue *qp)
{
	// add the job to the end and update first/lasp pointers
	if (QOfirstp==NULL)
		{
		// first in list
		QOfirstp=qp;
		QOlastp=qp;
		}
	else
		{
		// add to end
		qp->priorp=QOlastp;
		QOlastp->nextp=qp;
		QOlastp=qp;
		}
}


bool MPGPJobManager::IterateOutputQueue()
{
	// iterate input queue
	// returns true if there are still input jobs left
	MessageQueue *qp=QOfirstp;
	MessageQueue *nextqp;
	int qlen;
	int count=0;
	int count2;
	AnsiString stext;
	bool bretv;
	static AnsiString laststext;
	bool pauseafter=false;

	while (qp!=NULL)
		{
		// process it
		nextqp=qp->nextp;
		qlen=qp->string.Length();
		count+=qlen;
		if (qlen<AllowedOutBytes || (AllowedOutBytes==TotalAllowedOutBytes))
			{
			// ok, so we are allowed enough bytes, OR (we arent allowed more bytes, but because of a change to maxsize, we have lines longer than allowed max)
			// output it
			bretv=ProcessOutputString(qp->tochan,qp->tonick,qp->toident,qp->string);
			if (bretv)
				{
				AllowedOutBytes-=qlen;
				// since we've output it, we can now delete it
				DeleteOutputQueue(qp);
				}
			else
				{
				if (++(qp->trycount)>MaxOutputLineTries)
					{
					// this output line has failed too many times, kill it
					DeleteOutputQueue(qp);
					LogWarning("Deleting line from output queue due to too many errors: "+qp->tochan+","+qp->tonick+","+qp->toident+","+qp->string);
					}
				else
					pauseafter=true;
				}
			}
		qp=nextqp;
		}

	if (pauseafter==true)
		{
		// pausing output jobs since we had an error
		set_Paused(true);
		JobForm->Hide();
		JobForm->Show();
		QuestionForm->PresentOk("Output to Mirc Error","Error writing one ore more output lines; jobs paused.",jobmanager->MessageTimeout(),"");
		}

	// now add bytes from jobs that have not yet added their stuff to output queue
	count2=CountJobRemainingBytes();
	count+=count2;

	// joblist status bar bytes pending
	if (count>0)
		stext=AnsiString(count)+" bytes pending in output queue.";
	else
		stext="Output queue is empty.";

	// because of hints, sometimes we dont want to overwrite the statusbar
	if (stext!=laststext || PgpDialogForm->Visible==true)
		JobForm->StatusShow(stext,false);
	laststext=stext;

	if (QOfirstp!=NULL)
		return true;
	return false;
}

int MPGPJobManager::CountJobsPending()
{
	// walk through job queue and count the number of non-stopped (TOPLEVEL) jobs
	int count=0;
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;

	while (jobp!=NULL)
		{
		nextjobp=jobp->nextp;
		if (!jobp->IsCanceled())
			++count;
		jobp=nextjobp;
		}

	// kludge test - also say there are jobs if we have an output queue waiting
	if (QOfirstp!=NULL)
		++count;
	if (QIfirstp!=NULL)
		++count;

	return count;
}

int MPGPJobManager::CountJobRemainingBytes()
{
	// walk through job queue and count the number of non-stopped (TOPLEVEL) jobs
	int tbytes=0;
	int rembytes;
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;

	while (jobp!=NULL)
		{
		nextjobp=jobp->nextp;
		if (!jobp->IsCanceled())
			{
			rembytes=(jobp->outputlen)-(jobp->curpos);
			if (rembytes>0)
				tbytes+=rembytes;
			}
		jobp=nextjobp;
		}

	return tbytes;
}


void MPGPJobManager::set_BlockedForInput(bool bval)
{
	// this is called before popping up a pgp form
	// we do a trick here and set the parent window for modal display of the pgp form to make sure it stays visible
	BlockedForInput=bval;
	if (bval==true)
		{
		//
		if (PgpDialogForm->Visible)
			jobmanager->SetPgpWindowHandle(THandle(PgpDialogForm->Handle));
		else if (JobForm->Visible)
			jobmanager->SetPgpWindowHandle(THandle(JobForm->Handle));
		else if (OptionsForm->Visible)
			jobmanager->SetPgpWindowHandle(THandle(OptionsForm->Handle));
		else
			jobmanager->SetPgpWindowHandle(THandle(Application->Handle));
		}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void MPGPJobManager::FillJobListBox(TListBox *boxp, bool forceupdate)
{
	// fill listbox with job descriptions
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextp;
	AnsiString prefix;
	AnsiString description;
	int count;
	int depth=0;
	int oldindex;

	// only display if jobs have changed
	if (!JobsHaveChanged || forceupdate)
		return;

	// store current position
	oldindex=boxp->ItemIndex;
	if (oldindex!=-1 && oldindex==boxp->Items->Count-1)
		oldindex=99999;

	// clear the listbox first
	boxp->Clear();

	while (jobp!=NULL)
		{
		// show it
		description=jobp->GetJobDesc();
		prefix="";
		for (count=0;count<depth;++count)
			prefix+="+";
		// add it into listbox
		boxp->Items->Add(prefix+description);

		// next in line is a child
		if (jobp->childp!=NULL)
			{
			++depth;
			nextp=jobp->childp;
			}
		else
			nextp=NULL;
		// no child, then the next in line (brother)
		if (nextp==NULL)
			nextp=jobp->nextp;
		// no child or brother, then pop back up to parent's brother
		if (nextp==NULL)
			{
			if (jobp->parentp!=NULL)
				{
				do
					{
					--depth;
					jobp=jobp->parentp;
					nextp=jobp->nextp;
					} while (jobp!=NULL && nextp==NULL && jobp->parentp!=NULL);
				}
			}
		jobp=nextp;
		}

	// restore old index
	if (boxp->Items->Count>0)
		boxp->ItemIndex=boxp->Items->Count-1;
	if (oldindex<boxp->Items->Count)
		boxp->ItemIndex=oldindex;

	// record that we have updated the list
	JobsHaveChanged=false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MPGPJobManager::MakeAndQueueJobFromDialog()
{
	// make a job from the PGPDialogForm and queue it up
	// returns true on success
	MPGPJob *jobp;
	AnsiString fromkeyid,tokeyid;
	int jobradio;
	AnsiString contents;
	int sindex;

	// get parameters from dialog
	sindex=PgpDialogForm->ListBoxFrom->ItemIndex;
	if (sindex==-1)
		fromkeyid=AnonymousMircryptionKeyId;
	else
		fromkeyid=PgpDialogForm->ListBoxFrom->Items->Strings[sindex];
	sindex=PgpDialogForm->ListBoxTo->ItemIndex;
	if (sindex==-1)
		tokeyid=AnonymousMircryptionKeyId;
	else
		tokeyid=PgpDialogForm->ListBoxTo->Items->Strings[sindex];
	jobradio=PgpDialogForm->TypeRadioGroup->ItemIndex;
	contents=PgpDialogForm->ContentsMemo->Lines->Text;


	// Make sure they have provided sufficient info to make a job
	// if they trying to prove their identity but fromkeyid==anonymous, then complain
	if (fromkeyid==AnonymousMircryptionKeyId && jobradio==JOBRADIO_PROOF)
		{
		JMShowMessage("To prove your identity, you must select a non-anonymous pgp key in the FROM field.");\
		return false;
		}

	// WE NO LONGER DO THIS - instead a blank contents will bring up a dialog from pgp
	// blank pub key send means send fromkey (instead of signing with it)
	//	if (jobradio==JOBRADIO_PUBLICKEY && contents=="" && fromkeyid!=AnonymousMircryptionKeyId)
	//		contents=fromkeyid;
	// if they are sending one public key, and its the same one as they would sign by, then we dont sign
	//	if (jobradio==JOBRADIO_PUBLICKEY && contents==fromkeyid)
	//		fromkeyid=AnonymousMircryptionKeyId;
	// must provide pubkeys to send
	// if blank they will br prompted.
	//	if (jobradio==JOBRADIO_PUBLICKEY && contents=="")
	//		{
	//		JMShowMessage("To send public keys, you must provide their ids, or select a non-anonymous pgp key in the FROM field.");
	//		return false;
	//		}

	// if they are sending a message, make sure its not blank
	if (jobradio==JOBRADIO_MESSAGE && contents=="")
		{
		JMShowMessage("You need to fill in a message to send first.");
		return false;
		}
	// if they are sending files, make sure they have indicated some to send
	if (jobradio==JOBRADIO_FILE && contents=="")
		{
		JMShowMessage("You need to specify what file(s) to send; drag and drop them from windows explorer.");
		return false;
		}

	// make the new job
	jobp=NULL;
	switch(jobradio)
		{
		case JOBRADIO_PROOF:
			jobp=new JobOSE_PROOF(Global_tochan,Global_tonick,Global_toident,fromkeyid,tokeyid,contents);
			break;
		case JOBRADIO_CHANKEY:
			jobp=new JobOSE_CHAN(Global_tochan,Global_tonick,Global_toident,fromkeyid,tokeyid,contents);
			break;
		case JOBRADIO_PUBLICKEY:
			jobp=new JobOSE_PUBKEY(Global_tochan,Global_tonick,Global_toident,fromkeyid,tokeyid,contents);
			break;
		case JOBRADIO_MESSAGE:
			jobp=new JobOSE_MESG(Global_tochan,Global_tonick,Global_toident,fromkeyid,tokeyid,contents);
			break;
		case JOBRADIO_FILE:
			jobp=new JobOSE_FILE(Global_tochan,Global_tonick,Global_toident,fromkeyid,tokeyid,contents);
			break;
		default:
			LogError("Unknown job radio choice ("+AnsiString(jobradio)+").");
			break;
		}

	// if a job was made add it to the end of the job list and return success
	if (jobp!=NULL)
		{
		AddJobToEnd(jobp);
		return true;
		}
	return false;
}


void MPGPJobManager::AddJobToEnd(MPGPJob *jobp)
{
	// add the job to the end and update first/lasp pointers
	if (Jfirstp==NULL)
		{
		// first in list
		Jfirstp=jobp;
		Jlastp=jobp;
		}
	else
		{
		// add to end
		Jlastp->nextp=jobp;
		jobp->priorp=Jlastp;
		Jlastp=jobp;
		}

	// mark jobs as having been changed
	JobsHaveChanged=true;
	// let the dll (dll owner) know that a new job has been added - in case it has hibernated from iterating us
	DLLShouldWake();
	// autoshow job list?
	if (OptionsForm->AutoShowJobsCheckBox->Checked==true && JobForm->Visible==false)
		JobForm->Show();
}

AnsiString MPGPJobManager::CreateUniqueJobId()
{
	// return a unique job id
	AnsiString jobidstr;
	++JobCounter;
	jobidstr=AnsiString(JobCounter);
	return jobidstr;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

bool MPGPJobManager::TimerTicks(int interval)
{
	// the auto timer ticks
	if (BlockedForInput)
		return true;
	if (dllMircMode!=MircModeTimer)
		return true;
	if (get_Paused())
		return true;

	if (!DidInitialEnsureAnonymous)
		Initial_EnsureAnonymousKeyExists();
	float timere=PgpDialogForm->OutputTimer->Interval;

	AnsiString speedstr=OptionsForm->SpeedEdit->Text;
	// LogMessage("speededit = '"+speedstr+"' int="+AnsiString(maxbytes));
	int burstbytes;
	int maxbytes=100;
	JRStringToInt(speedstr,maxbytes);
	maxbytes/=(1000.0/timere);
	burstbytes=maxbytes;
	JRStringToInt(OptionsForm->BurstEdit->Text,burstbytes);
	int omaxbytes=maxbytes;
	maxbytes+=LeftOverWriteBytes;

	if (maxbytes>MinGoodMircLineLength || maxbytes>burstbytes+omaxbytes)
		{
		if (maxbytes<100)
			maxbytes=100;

		// Iterate and generate some output
		Iterate(maxbytes);
		}
	else
		AllowedOutBytes=maxbytes;

	// now add up our leftover bytes - for bursting
	LeftOverWriteBytes=AllowedOutBytes;
	if (LeftOverWriteBytes>burstbytes)
		LeftOverWriteBytes=burstbytes;
	if (LeftOverWriteBytes<0)
		LeftOverWriteBytes=0;

	//	LogMessage("leftoverbytes = "+AnsiString(LeftOverWriteBytes) + "    maxbytes = "+AnsiString(maxbytes)+"    allowedout = "+AnsiString(AllowedOutBytes));

	return true;
}


bool MPGPJobManager::Iterate(int maxoutbytes)
{
	// process any input (feed it to waiting jobs
	// returns true if there is any input, output, or jobs left to iterate
	bool bretv1;
	bool bretv2=false;
	bool bretv3=false;

	// set global var of num bytes we can write on this call
	// ATTN: IMPORTANT this needs to sometimes be bigger than MaxOutLineLength(+headersize), otherwise we could get lines in output buffer
	// that are too big to be outputed.
	// This can be tricky if a subsequent call to Iterate reduces the maxoutbytes size AFTER we have already queued up some longer lines
	if (maxoutbytes>0)
		set_AllowedOutBytes(maxoutbytes);

	// process any pending input
	do
		{
		bretv1=IterateInput();
		} while (bretv1);

	// only if we are unpaused, we can iterate and send output
	if (!get_Paused())
		{
		// NOW, we have to do something clever here.
		// We are only allowed to output a certain number of bytes per second, or we will get kicked for flooding.
		// This can be tricky since, we can have big jobs that want to output a lot of data, and small jobs
		//  which we dont want to block while waiting for the bigger jobs.
		// Plus, because of overhead of headers, we want to write long lines where possible.
		// To help us, we maintain an output queue of (MaxOutLineLength) lines that have been buffered for output.
		// Still, we must ask, how should we process this output queue, and how should we iterate jobs to avoid
		//  this output queue getting huge if one of our jobs is trying to send a huge file.
		// We may also want to demonstrate a bias for small strings.?

		// A simple solution might work like this:
		// On each call we are allocated a max number of bytes to output
		// First we check output buffer - if it is non-empty, process as much of it as we can
		// Then, ONLY we the output buffer is empty to we iterate jobs and let them make new output
		// If we iterate jobs then we can check output buffer again before we return.
		// This method insures that the output queue never builds up more than 1 output line per job

		// process any output that is already buffered
		bretv2=IterateOutput();
		if (QOfirstp==NULL)
			{
			// no output in buffer, so iterate jobs
			// OUR new technique is to avoid blocking in Iterate(), so we make sure we return always right away here
			if (dllMircMode==MircModeOnRequest)
				{
				// in this mode, a call to iterate() just schedules a single async iterate
				set_ScheduledJobIterate(true);
				}
			else
				{
				// in this mode, this func is called by an async timer already, no need to schedule
				bretv3=IterateJobs();
				// now call iterateoutput again (though since we already wrote some output before, we may not have any output to generate)
				bretv2=IterateOutput();
				}
			}
		}

	// now update pending job list
	FillJobListBox(JobForm->JobsListBox);

	// returns true if there is any input, output, or jobs left to iterate
	if (bretv1 || bretv2 || bretv3)
		{
		HasWorkToDo=true;
		return true;
		}
	HasWorkToDo=false;
	return false;
}


bool MPGPJobManager::IterateInput()
{
	// iterate input queue
	bool bretv;
	bretv=IterateInputQueue();
	return bretv;
}

bool MPGPJobManager::IterateOutput()
{
	// iterate output queue
	bool bretv;
	bretv=IterateOutputQueue();
	return bretv;
}

bool MPGPJobManager::IterateJobs()
{
	// iterate jobs, outputing maxbytes
	// returns true if there are still jobs to run
	bool bretv=false;
	bool bretv2;
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;
	static int expirecount=0;

	if (Jfirstp==NULL)
		return false;

	while (jobp!=NULL)
		{
		bretv2=jobp->Iterate();
		if (bretv2)
			bretv=true;
		jobp=jobp->nextp;
		}

	// every once and a while we check for expired jobs
	if (++expirecount>10)
		{
		CheckForExpiredJobs();
		expirecount=0;
		}

	return bretv;
}

void MPGPJobManager::CheckForExpiredJobs()
{
	// expire any jobs that have gone too long
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;
	TDateTime exptime=Now();
	double fracdays;
	int expsecs=60;

	if (Jfirstp==NULL)
		return;

	// set expire time as ucrrent time + some # of fractional days (86400 secs per day)
	JRStringToInt(OptionsForm->TimeoutEdit->Text,expsecs);
	if (expsecs==0)
		return;
	fracdays=(double)expsecs/86400.0;
	exptime-=fracdays;

	while (jobp!=NULL)
		{
		// has jobp expired?
		if (!jobp->IsCanceled() && !jobp->IsConfirmed() && !jobp->IsSuspended())
			{
			if (jobp->updatetime<exptime)
				jobp->BecomeExpired();
			}
		jobp=jobp->nextp;
		}
}

void MPGPJobManager::set_AllowedOutBytes(int maxoutbytes)
{
	if (maxoutbytes<100)
		maxoutbytes=100;
	TotalAllowedOutBytes=maxoutbytes;
	AllowedOutBytes=TotalAllowedOutBytes;
	MaxOutLineLength=maxoutbytes-50;
	if (MaxOutLineLength>MaxMircLineLength)
		MaxOutLineLength=MaxMircLineLength;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
int MPGPJobManager::QuestionTimeout()
{
	int retv=60;
	JRStringToInt(OptionsForm->MessageTimeoutEdit->Text,retv);
	return retv;
}

int MPGPJobManager::MessageTimeout()
{
	int retv=60;
	JRStringToInt(OptionsForm->QuestionTimeoutEdit->Text,retv);
	return retv;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// HELPERS
AnsiString MPGPJobManager::GetValidityString(int validlevel)
{
	AnsiString sretv;
	switch(validlevel)
		{
		case Validity_Unknown:
			sretv="unknown signer";
			break;
		case Validity_Invalid:
			sretv="not yet signed as a valid key";
			break;
		case Validity_Marginal:
			sretv="marginally valid";
			break;
		case Validity_Complete:
			sretv="valid";
			break;
		default:
			sretv="INVALID";
			break;
		}
	return sretv;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// DEBUG HELPERS
void MPGPJobManager::LogMessage(AnsiString str)
{
	if (LogForm->Visible)
		LogForm->LogMemo->Lines->Add(str);
}

void MPGPJobManager::LogWarning(AnsiString str)
{
	if (LogForm->Visible)
		LogForm->LogMemo->Lines->Add(str);
}

void MPGPJobManager::LogError(AnsiString str)
{
	LogForm->LogMemo->Lines->Add(str);
}

void MPGPJobManager::LogDebug(AnsiString str)
{
	if (LogForm->Visible)
		LogForm->LogMemo->Lines->Add(str);
}

void MPGPJobManager::LogInput(AnsiString str)
{
	if (LogForm->Visible)
		LogForm->InMemo->Lines->Add(str);
}

void MPGPJobManager::LogOutput(AnsiString str)
{
	if (LogForm->Visible)
		LogForm->OutMemo->Lines->Add(str);
}

//---------------------------------------------------------------------------
bool MPGPJobManager::MircMessage(AnsiString chan,AnsiString str)
{
	// display a message in mirc status window
	bool bretv;
	// need to avoid newlines
	JRStringReplace(str,"\r\n"," | ");
	if (chan=="")
		bretv=DLLDirectSendMircOutput("/statusmsg "+str);
	else
		bretv=DLLDirectSendMircOutput("/mpgp_echo "+chan+" "+str);
	return bretv;
}

bool MPGPJobManager::SetMircChannelkey(AnsiString chan,AnsiString key)
{
	// set the channel key in mirc (if we have change it)
	bool bretv=DLLDirectSendMircOutput("/mpgp_setkey "+chan+" "+key);
	return bretv;
}

void MPGPJobManager::JMShowMessage(AnsiString mesg)
{
	// blocking message display
	set_BlockedForInput(true);
	ShowMessage(mesg);
	set_BlockedForInput(false);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// PROCESS INPUT AND OUTPUT

bool MPGPJobManager::QueueOutputString(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString instring)
{
	// queue an input line for later processing (useful for debugging a feedback loop)
	MessageQueue *qp=new MessageQueue(itochan,itonick,itoident,instring);
	AddOutputQueueToEnd(qp);
	return true;
}

bool MPGPJobManager::QueueInputString(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString instring)
{
	// queue an input line for later processing (useful for debugging a feedback loop)
	MessageQueue *qp=new MessageQueue(itochan,itonick,itoident,instring);
	AddInputQueueToEnd(qp);
	return true;
}


bool MPGPJobManager::QueueInputDebugString(AnsiString instring)
{
	// queue an input line using current global tochan,tonick, for debugging tests
	bool bretv;
	bretv=QueueInputString(Global_tochan,Global_tonick,Global_toident,instring);
	return bretv;
}


bool MPGPJobManager::ProcessInputString(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString str)
{
	// process input from mirc
	// returns true on success
	bool bretv;
	AnsiString jobkeyword;
	AnsiString jobid;
	AnsiString jobtype;
	AnsiString contents;
	AnsiString ostr=str;
	int startpos,totallength,thislen;
	AnsiString num;

	// decode input parameters
	bretv=JrLeftStringSplit(str,jobkeyword," ");
	if (bretv==false)
		{
		IncomingJobParseError("Empty incoming data: "+ostr);
		return false;
		}
	if (jobkeyword!=AnsiString(JOBPREFIX))
		{
		IncomingJobParseError("Missing JOB prefix keyword: "+ostr);
		return false;
		}
	bretv=JrLeftStringSplit(str,jobid," ");
	if (bretv==false)
		{
		IncomingJobParseError("No job id found: "+ostr);
		return false;
		}
	bretv=JrLeftStringSplit(str,jobtype," ");
	if (bretv==false)
		{
		IncomingJobParseError("No job type found: "+ostr);
		return false;
		}
	bretv=JrLeftStringSplit(str,num," ");
	if (bretv==false || !JRStringToInt(num,startpos))
		{
		IncomingJobParseError("No startpos found: "+ostr);
		return false;
		}
	bretv=JrLeftStringSplit(str,num," ");
	if (bretv==false || !JRStringToInt(num,thislen))
		{
		IncomingJobParseError("thislen not found: "+ostr);
		return false;
		}
	bretv=JrLeftStringSplit(str,num," ");
	if (bretv==false || !JRStringToInt(num,totallength))
		{
		IncomingJobParseError("totallength not found: "+ostr);
		return false;
		}
	// remainder of string is the contents
	contents=str;

	// display in log window
	LogInput(ostr);

	// process it
	bretv=ProcessInputJobData(jobkeyword,itochan,itonick,itoident,jobid,jobtype,startpos,thislen,totallength,contents);

	// return true if we have accepted it
	return bretv;
}


void MPGPJobManager::IncomingJobParseError(AnsiString mesg)
{
	// error parsing
	LogError("MPGP Parse Error - "+mesg);
}


bool MPGPJobManager::ProcessOutputString(AnsiString itochan,AnsiString itonick,AnsiString itoident,AnsiString str)
{
	// send output to mirc
	// returns true on success
	bool bretv;

	// send output to mirc?
	bretv=DLLSendMircOutput(itochan,itonick,itoident,str);
	if (bretv)
		{
		// display in log window
		LogOutput(str);
		// for DEBUGGING FEEDBACK LOOP
		if (LogForm->Visible==true && LogForm->FeedbackCheckBox->Checked==true)
			QueueInputString(itochan,itonick,itoident,str);
		}

	return bretv;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
bool MPGPJobManager::ProcessInputJobData(AnsiString jobkeyword,AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int &startpos,int &thislen,int &totallength,AnsiString &contents)
{
	// process parsed input data by offering it to each job
	// returns true if any job accepts it
	bool bretv;
	MPGPJob *jobp;
	bool justborn;
	bool alreadystopped=false;

	// first, see if there is an existing job we need to add this data to
	jobp=LookUpMatchingJob(tochan,tonick,toident,jobid,jobtype,JOBDIR_INCOMING);

	// if there is no existing job, so make a TEMPORARY job for it - note that this step may occur even if
	// we later decide not to "accept" this incoming job, it basically just builds a wrapper for the (possibly partial) input
	if (jobp==NULL)
		{
		// ok try to make a new job
		jobp=CreateNewIncomingJob(tochan,tonick,toident,jobid,jobtype);
		// initialize the job to make sure it knows it is just begining to be assembled
		if (jobp==NULL)
			{
			// we refuse to accept it
			SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"not an allowed incoming job");
			return false;
			}
		jobp->assembling=true;
		justborn=true;
		}
	else
		{
		justborn=false;
		alreadystopped=jobp->IsStopped() && !(jobp->IsOutputting() && !(jobp->IsAssembling()));
		}

	// adoption
	if (justborn)
		{
		// The new job has its first chunk of data, now decide whther to accept it
		bretv=AdoptNewJob(jobp);
		if (!bretv)
			{
			// the job has been rejected, so cancel it, reject it, and delete it
			// this happens for unsolicited jobs
			if (!alreadystopped)
				SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"job could not be adopted");
			delete jobp;
			jobp=NULL;
			return false;
			}
		}

	// now add the incoming (possibly partial) data to the job
	bretv=jobp->AddAssembleData(tochan,tonick,toident,jobid,jobtype,startpos,thislen,totallength,contents);
	if (!bretv)
		{
		// there was an error adding the data, so send a cancel to the sender and return false
		// note that the newobp will already have been ceaceled from within AddAssembleData
		if (!alreadystopped)
			SendACancel(tochan,tonick,toident,jobid,jobtype,JOBDIR_OUTGOING,"cannot add new data to this job");
		// if it was just created and not adopted yet, we need to delete it
		if (justborn)
			delete jobp;
		jobp=NULL;
		return false;
		}

	// is this current job still being built? if so, just return saying the job is ok
	if (jobp->IsAssembling())
		{
		// still being assembled
		}
	else
		{
		// the job has just now got all of its input data and is fully assembled
		jobp->IncomingCompletes();
		}

	return true;
}



bool MPGPJobManager::AdoptNewJob(MPGPJob *newjobp)
{
	// a new incoming job has been born; only the first bit of data is in it at this point
	// returns false if it is an unwanted incoming job
	// returns true if another job will accept it as a child, OR if we add it to jobqueue as an inpdepdent job

	// see of any current jobs want to adopt it
	MPGPJob *jobp=Jfirstp;
	MPGPJob *nextjobp;
	bool acceptit=false;
	bool bretv=false;
	bool bretv2;

	// first see if anyone wants to adopt this new job
	while (jobp!=NULL)
		{
		if (!jobp->IsStopped())
			{
			// note even suspended jobs are allowed to process incoming jobs (just not send output)
			bretv2=jobp->RecursivelyAdopt(newjobp);
			if (bretv2)
				{
				// someone adopted it, so break;
				bretv=true;
				break;
				}
			}
		jobp=jobp->nextp;
		}
	if (bretv==true)
		return true;

	// no existing job wanted to adopt it.  Is it a job that can start on its own?
	if (newjobp->jobtype==RQ_ANONHEXID)
		acceptit=true;
	else if (newjobp->jobtype==RQ_ANONPUBKEY)
		acceptit=true;
	else if (newjobp->jobtype==RQ_PROOFRAND)
		acceptit=true;
	else if (newjobp->jobtype==RQ_FILEPERM)
		acceptit=true;
	else if (newjobp->jobtype==SE_CHAN)
		acceptit=true;
	else if (newjobp->jobtype==SE_PUBKEY)
		acceptit=true;
	else if (newjobp->jobtype==SE_MESG)
		acceptit=true;
	else if (newjobp->jobtype==JC_KILL)
		acceptit=true;
	else if (newjobp->jobtype==JC_ACK)
		acceptit=true;
	else if (newjobp->jobtype==JC_REWIND)
		acceptit=true;

	if (acceptit)
		{
		// ok we add it to our queue and let it live as a standalone job
		AddJobToEnd(newjobp);
		}
	else
		{
		// job was rejected - should we output a message to log about this?
		jobmanager->LogMessage("Incoming job "+newjobp->GetPJobid()+" ("+newjobp->jobtype+") from "+newjobp->tonick+" was rejected.");
		}

	return acceptit;
}


void MPGPJobManager::SendACancel(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype,int direction,const AnsiString &creason)
{
	// create and add to job queue a message to send to tonick to cancel a job of theirs
	MPGPJob *jobp;

        // ATTN: need to make this a proper cancel job
	// make the cancel request
	AnsiString contents=AnsiString(jobid)+" "+AnsiString(jobtype);
	jobp=new JobOJC_KILL(tochan,tonick,toident,jobid,jobtype,direction);
	if (direction==JOBDIR_INCOMING)
		jobmanager->LogMessage("Sending "+tonick+" a request to cancel job "+jobid+"i: "+creason);
	else
		jobmanager->LogMessage("Sending "+tonick+" a request to cancel job "+jobid+": "+creason);
	// queue it up
	AddJobToEnd(jobp);
}


MPGPJob *MPGPJobManager::CreateNewIncomingJob(AnsiString &tochan,AnsiString &tonick,AnsiString &toident,AnsiString &jobid,AnsiString &jobtype)
{
	// create a new job from this incoming data IF it is allowed, otherwise return NULL
	MPGPJob *jobp=NULL;

	if (jobtype==RQ_ANONHEXID)
		jobp=new JobIRQ_ANONHEXID(jobid,tochan,tonick,toident);
	else if (jobtype==RQ_ANONPUBKEY)
		jobp=new JobIRQ_ANONPUBKEY(jobid,tochan,tonick,toident);
	else if (jobtype==RQ_PROOFRAND)
		jobp=new JobIRQ_PROOFRAND(jobid,tochan,tonick,toident);
	else if (jobtype==RQ_FILEPERM)
		jobp=new JobIRQ_FILEPERM(jobid,tochan,tonick,toident);
	else if (jobtype==SE_PROOF)
		jobp=new JobISE_PROOF(jobid,tochan,tonick,toident);
	else if (jobtype==SE_CHAN)
		jobp=new JobISE_CHAN(jobid,tochan,tonick,toident);
	else if (jobtype==SE_PUBKEY)
		jobp=new JobISE_PUBKEY(jobid,tochan,tonick,toident);
	else if (jobtype==SE_MESG)
		jobp=new JobISE_MESG(jobid,tochan,tonick,toident);
	else if (jobtype==SE_FILE)
		jobp=new JobISE_FILE(jobid,tochan,tonick,toident);
	else if (jobtype==RP_ANONHEXID)
		jobp=new JobIRP_ANONHEXID(jobid,tochan,tonick,toident);
	else if (jobtype==RP_ANONPUBKEY)
		jobp=new JobIRP_ANONPUBKEY(jobid,tochan,tonick,toident);
	else if (jobtype==RP_PROOFRAND)
		jobp=new JobIRP_PROOFRAND(jobid,tochan,tonick,toident);
	else if (jobtype==RP_FILEPERM)
		jobp=new JobIRP_FILEPERM(jobid,tochan,tonick,toident);
	else if (jobtype==JC_KILL)
		jobp=new JobIJC_KILL(jobid,tochan,tonick,toident);
	else if (jobtype==JC_ACK)
		jobp=new JobIJC_ACK(jobid,tochan,tonick,toident);
	else if (jobtype==JC_REWIND)
		jobp=new JobIJC_REWIND(jobid,tochan,tonick,toident);
	return jobp;
}
//---------------------------------------------------------------------------


