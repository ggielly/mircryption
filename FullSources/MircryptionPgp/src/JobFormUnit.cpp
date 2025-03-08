//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JobFormUnit.h"
#include "JobManager.h"
#include "LogFormUnit.h"
#include "OptionsFormUnit.h"
#include "PgpDialogUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TJobForm *JobForm;
//---------------------------------------------------------------------------
__fastcall TJobForm::TJobForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TJobForm::SuspendItemClick(TObject *Sender)
{
	jobmanager->SuspendJob(JobsListBox->ItemIndex);
	// now update the jobs listbox
	jobmanager->FillJobListBox(JobsListBox);
}


void __fastcall TJobForm::ResumeItemClick(TObject *Sender)
{
	jobmanager->ResumeJob(JobsListBox->ItemIndex);
	// now update the jobs listbox
	jobmanager->FillJobListBox(JobsListBox);
}


void __fastcall TJobForm::CancelItemClick(TObject *Sender)
{
	jobmanager->CancelJob(JobsListBox->ItemIndex,"canceled");
	// now update the jobs listbox
	jobmanager->FillJobListBox(JobsListBox);
}


void __fastcall TJobForm::PurgeAllItemClick(TObject *Sender)
{
	jobmanager->PurgeAllFinishedJobs();
	// now update the jobs listbox
	jobmanager->FillJobListBox(JobsListBox);
}


void __fastcall TJobForm::StopAllItemClick(TObject *Sender)
{
	jobmanager->CancelAllJobs();
	jobmanager->PurgeAllFinishedJobs();
	// now update the jobs listbox
	jobmanager->FillJobListBox(JobsListBox);
}

void __fastcall TJobForm::SuspendAllItemClick(TObject *Sender)
{
	jobmanager->SuspendAllJobs();
}


//---------------------------------------------------------------------------
void __fastcall TJobForm::JobsListBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	// mouse down selects and adjusts context menu
	TPoint pos;
	int index;
	MPGPJob *jobp;

	SuspendItem->Enabled=false;
	ResumeItem->Enabled=false;
	CancelItem->Enabled=false;

	if (JobsListBox->Items->Count==0)
		{
		PurgeAllItem->Enabled=false;
		StopAllItem->Enabled=false;
		SuspendAllItem->Enabled=false;
		return;
		}
	else
		{
		PurgeAllItem->Enabled=true;
		StopAllItem->Enabled=true;
		}

	pos.x=X;
	pos.y=Y;
	index=JobsListBox->ItemAtPos(pos, true);
	if (index<0)
		return;

	// select it
	JobsListBox->ItemIndex=index;
	// adjust context menu
	SuspendItem->Enabled=false;
	ResumeItem->Enabled=false;
	CancelItem->Enabled=false;

	if (jobmanager->IsJobSuspended(index))
		{
		ResumeItem->Enabled=true;
		CancelItem->Enabled=true;
		}
	else if ( (!jobmanager->IsJobConfirmed(index) && !jobmanager->IsJobCanceled(index)) || jobmanager->IsJobOutputting(index))
		{
		SuspendItem->Enabled=true;
		CancelItem->Enabled=true;
		}
}

//---------------------------------------------------------------------------

void __fastcall TJobForm::PauseButtonClick(TObject *Sender)
{
	bool bretv;
	if (jobmanager->get_Paused())
		{
		jobmanager->set_Paused(false);
		PauseButton->Caption="Pause Jobs";
		}
	else
		{
		jobmanager->set_Paused(true);
		PauseButton->Caption="UnPause Jobs";
		}
}


//---------------------------------------------------------------------------
void __fastcall TJobForm::FormHide(TObject *Sender)
{
	// when we hide or close, if options say, then we purge all finished jobs
	if (nowquitting)
		return;

	if (jobmanager!=NULL && OptionsForm!=NULL)
		{
		if (OptionsForm->AutoClearJobsCheckBox->Checked)
			{
			jobmanager->PurgeAllFinishedJobs();
			jobmanager->FillJobListBox(JobsListBox);
			}
		}
}
//---------------------------------------------------------------------------
void __fastcall TJobForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// when we hide or close, if options say, then we purge all finished jobs
	if (nowquitting)
		return;

	if (jobmanager!=NULL && OptionsForm!=NULL)
		{
		if (OptionsForm->AutoClearJobsCheckBox->Checked)
			{
			jobmanager->PurgeAllFinishedJobs();
			jobmanager->FillJobListBox(JobsListBox);
			}
		}
}
//---------------------------------------------------------------------------
void __fastcall TJobForm::ClearFinishedButtonClick(TObject *Sender)
{
	// clear all finished jobs
	jobmanager->PurgeAllFinishedJobs();
	jobmanager->FillJobListBox(JobsListBox);
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::LogButtonClick(TObject *Sender)
{
	// show log window from job list
	if (LogForm->Visible==false)
		LogForm->Present();
	else
		LogForm->Hide();	
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::FormShow(TObject *Sender)
{
	if (nowquitting)
		return;
	if (!jobmanager)
		return;
	if (jobmanager->get_Paused())
		PauseButton->Caption="UnPause Jobs";
	else
		PauseButton->Caption="Pause Jobs";
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::OptionsButtonClick(TObject *Sender)
{
	// show options dialog
	if (OptionsForm->Visible==false)
		OptionsForm->Present();
	else
		OptionsForm->Hide();	
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::MainDialogButtonClick(TObject *Sender)
{
	// sohw the main dialog
	PgpDialogForm->Show();	
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::NewAnonymousKeyButtonClick(TObject *Sender)
{
	// make new anonymous key
	jobmanager->MakeNewAnonymousKey();
}
//---------------------------------------------------------------------------

void __fastcall TJobForm::KeyManagerButtonClick(TObject *Sender)
{
	// show key manager
	jobmanager->LaunchKeyManager();
}

void __fastcall TJobForm::HelpButtonClick(TObject *Sender)
{
	AnsiString execfilename=jobmanager->get_basedir()+"MPGP.chm";
	ShellExecute(NULL, "open", execfilename.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TJobForm::ShowHint(TObject *Sender)
{
	// show the hint on the status bar
	if (Application->Hint != lasthint && lasthint!="_NONHINT_")
		StatusShow(Application->Hint,true);
	else if (lasthint=="_NONHINT_")
		lasthint="";
}

void TJobForm::StatusShow(const AnsiString textstr,bool ishint)
{
	if (StatusBar->SimpleText!=textstr)
		StatusBar->SimpleText=textstr;
	if (ishint==true)
		lasthint=textstr;
	else
		lasthint="_NONHINT_";
}
//---------------------------------------------------------------------------

