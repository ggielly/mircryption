//---------------------------------------------------------------------------

#ifndef QuestionFormUnitH
#define QuestionFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TQuestionForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *YesButton;
	TBitBtn *NoButton;
	TLabel *TimeoutLabel;
	TTimer *Timer1;
	TBitBtn *WaitButton;
	TPanel *Panel1;
	TMemo *QuestionMemo;
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall YesButtonClick(TObject *Sender);
	void __fastcall NoButtonClick(TObject *Sender);
	void __fastcall FormClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall WaitButtonClick(TObject *Sender);
private:	// User declarations
	int seconds;
	int answer;
	bool wantsquit;
	int timeout;
	AnsiString timeoutmesg;
public:		// User declarations
	__fastcall TQuestionForm(TComponent* Owner);
public:
	int PresentYesNo(AnsiString titlebar,AnsiString text,int indefault,int intimeout,AnsiString itimeoutmesg);
	int PresentOk(AnsiString titlebar,AnsiString text,int intimeout,AnsiString itimeoutmesg);
};
//---------------------------------------------------------------------------
extern PACKAGE TQuestionForm *QuestionForm;
//---------------------------------------------------------------------------
#endif
