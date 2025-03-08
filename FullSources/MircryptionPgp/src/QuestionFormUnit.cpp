//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "QuestionFormUnit.h"

#include "JobManager.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TQuestionForm *QuestionForm;
//---------------------------------------------------------------------------
__fastcall TQuestionForm::TQuestionForm(TComponent* Owner)
	: TForm(Owner)
{
	wantsquit=false;
}
//---------------------------------------------------------------------------




int TQuestionForm::PresentYesNo(AnsiString titlebar,AnsiString text,int indefault,int intimeout,AnsiString itimeoutmesg)
{
	// present the question, and timeout to default answer after timeout seconds
	// returns 1 on YES
	// returns 0 on NO
	// returns -1 if no default and we timeout

	// block to prevent async mirc call
	jobmanager->set_BlockedForInput(true);

	timeout=intimeout;
	answer=indefault;
	timeoutmesg=itimeoutmesg;

	// adjust display
	if (titlebar=="")
		Caption="MPGP Question?";
	else
		Caption=titlebar;
	YesButton->Visible=true;
	NoButton->Visible=true;
	YesButton->Caption="&Yes";
	WaitButton->Visible=false;

	QuestionMemo->Text=text;
	seconds=timeout;
	wantsquit=false;
	TimeoutLabel->Caption="";
	Timer1->Enabled=true;

	// loop until they answer or we timeout
	Show();
	while (!wantsquit)
		Application->HandleMessage();

	// shutdown
	Hide();
	Timer1->Enabled=false;
	wantsquit=false;
	TimeoutLabel->Caption="";

	// return the answer
	jobmanager->set_BlockedForInput(false);
	return answer;
}


int TQuestionForm::PresentOk(AnsiString titlebar,AnsiString text,int intimeout,AnsiString itimeoutmesg)
{
	// present the question, and timeout to default answer after timeout seconds
	// returns -1

	// block to prevent async mirc call
	jobmanager->set_BlockedForInput(true);

	timeout=intimeout;
	answer=-1;
	timeoutmesg=itimeoutmesg;

	// adjust display
	if (titlebar=="")
		Caption="MPGP Message";
	else
		Caption=titlebar;
	YesButton->Visible=true;
	NoButton->Visible=false;
	YesButton->Caption="&Ok";
	WaitButton->Visible=false;

	QuestionMemo->Text=text;
	seconds=timeout;
	wantsquit=false;
	TimeoutLabel->Caption="";
	Timer1->Enabled=true;

	// loop until they answer or we timeout
	Show();
	while (!wantsquit)
		Application->HandleMessage();

	// shutdown
	Hide();
	Timer1->Enabled=false;
	wantsquit=false;
	TimeoutLabel->Caption="";

	// return the answer
	jobmanager->set_BlockedForInput(false);
	return answer;
}


void __fastcall TQuestionForm::Timer1Timer(TObject *Sender)
{
        if (jobmanager==NULL)
                return;
                
	if (seconds<=0)
		return;
	if (--seconds<=0)
		{
		wantsquit=true;
		// show mirc message
		if (timeoutmesg!="")
			jobmanager->MircMessage("",timeoutmesg);
		}
	else if (seconds<timeout-(timeout/3))
		{
		// make wait button visible
		if (NoButton->Visible)
			WaitButton->Left=NoButton->Left-(YesButton->Left-NoButton->Left);
		else
			WaitButton->Left=NoButton->Left;
		WaitButton->Visible=true;
		if (answer==-1)
			TimeoutLabel->Caption=AnsiString(seconds)+" seconds before closing.";
		else if (answer==0)
			TimeoutLabel->Caption=AnsiString(seconds)+" seconds before deafulting to NO.";
		else if (answer==1)
			TimeoutLabel->Caption=AnsiString(seconds)+" seconds before deafulting to YES.";
		}
}
//---------------------------------------------------------------------------

void __fastcall TQuestionForm::YesButtonClick(TObject *Sender)
{
	answer=1;
	wantsquit=true;
}
//---------------------------------------------------------------------------

void __fastcall TQuestionForm::NoButtonClick(TObject *Sender)
{
	answer=0;
	wantsquit=true;
}
//---------------------------------------------------------------------------

void __fastcall TQuestionForm::FormClick(TObject *Sender)
{
	// seconds==-1 will cancel timeout
	seconds=-1;
	TimeoutLabel->Caption="";
	WaitButton->Visible=false;
}

void __fastcall TQuestionForm::WaitButtonClick(TObject *Sender)
{
	FormClick(NULL);
}

//---------------------------------------------------------------------------

void __fastcall TQuestionForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action=caNone;
	// assume answer is NO if they try to close form manually
	answer=0;
	wantsquit=true;
}
//---------------------------------------------------------------------------


