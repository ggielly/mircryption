//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "LogFormUnit.h"
#include "PgpDialogUnit.h"

#include "JobManager.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLogForm *LogForm;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
__fastcall TLogForm::TLogForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------



void TLogForm::Present()
{
	// show form and align it
	Show();
}

void __fastcall TLogForm::LogEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	// manually add some text
	if (Key==13)
		{
		// For testing, we feed this into the input stream
		jobmanager->QueueInputDebugString(LogEdit->Text);
		LogEdit->Text="";
		}
}
//---------------------------------------------------------------------------
void __fastcall TLogForm::FormResize(TObject *Sender)
{
	if (nowquitting)
		return;
	BottomPanel->Height=LogForm->Height/2;
	InPanel->Width=LogForm->Width/2;
}
//---------------------------------------------------------------------------

void __fastcall TLogForm::ClearButtonClick(TObject *Sender)
{
	LogForm->LogMemo->Clear();
	LogForm->InMemo->Clear();
	LogForm->OutMemo->Clear();

}
//---------------------------------------------------------------------------

