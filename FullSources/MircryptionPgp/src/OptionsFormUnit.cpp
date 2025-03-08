//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OptionsFormUnit.h"
#include "PgpDialogUnit.h"
#include "LogFormUnit.h"
#include "JobFormUnit.h"
#include "MiscUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOptionsForm *OptionsForm;
//---------------------------------------------------------------------------
__fastcall TOptionsForm::TOptionsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------



void TOptionsForm::Present()
{
	// present the form
	Show();
}



void __fastcall TOptionsForm::ResetOptionsButtonClick(TObject *Sender)
{
	//
	KeysizeRadioGroup->ItemIndex=1;
	RandKeyRadioGroup->ItemIndex=1;
	ArmouringRadioGroup->ItemIndex=1;
	KeyringDirectoryEdit->Text="PgpKeys";
	KeyManagerEdit->Text=jobmanager->get_DefaultKeyToolsExe();
	TimeoutEdit->Text="180";
	QuestionTimeoutEdit->Text="180";
	MessageTimeoutEdit->Text="120";
	OptionsForm->SpeedEdit->Text="300";
	BurstEdit->Text="1000";
	TimerEdit->Text="500";
	IncomingAnonymousRequests->ItemIndex=3;
}
//---------------------------------------------------------------------------

void __fastcall TOptionsForm::OkButtonClick(TObject *Sender)
{
	Hide();

	// and any settings applied after OK
	UpdateOptions();
}

void TOptionsForm::UpdateOptions()
{
	// timer
	int timerinterval=1000;
	JRStringToInt(TimerEdit->Text,timerinterval);
	if (timerinterval<100)
		timerinterval=100;
	if (timerinterval>3000)
		timerinterval=3000;
	PgpDialogForm->OutputTimer->Interval=timerinterval;	
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TOptionsForm::NewAnonymousKeyButtonClick(TObject *Sender)
{
	jobmanager->MakeNewAnonymousKey();
}


void __fastcall TOptionsForm::KeyManagerButtonClick(TObject *Sender)
{
	jobmanager->LaunchKeyManager();
}



void __fastcall TOptionsForm::LogButtonClick(TObject *Sender)
{
	// toiggle display of log window
	if (LogForm->Visible==false)
		LogForm->Present();
	else
		LogForm->Hide();
}


void __fastcall TOptionsForm::JoblistButtonClick(TObject *Sender)
{
	// toggle visibility of joblist
	if (JobForm->Visible==false)
		JobForm->Show();
	else
		JobForm->Hide();
}


void __fastcall TOptionsForm::HelpButtonClick(TObject *Sender)
{
	// Show Help
	AnsiString execfilename=jobmanager->get_basedir()+"MPGP.chm";
	ShellExecute(NULL, "open", execfilename.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TOptionsForm::FormHide(TObject *Sender)
{
	if (nowquitting)
		return;
	// give opportunity to sleep if there are no jobs
	if (PgpDialogForm->Visible==false && jobmanager!=NULL)
		jobmanager->SleepIfNoJobs();
}
//---------------------------------------------------------------------------

