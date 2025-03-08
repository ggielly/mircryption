//---------------------------------------------------------------------------

#ifndef JobFormUnitH
#define JobFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TJobForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *JobsGroupBox;
	TListBox *JobsListBox;
	TPopupMenu *JobsPopupMenu;
	TMenuItem *SuspendItem;
	TMenuItem *ResumeItem;
	TMenuItem *Spacer1;
	TMenuItem *CancelItem;
	TMenuItem *Spacer2;
	TMenuItem *SuspendAllItem;
	TMenuItem *StopAllItem;
	TMenuItem *Spacer3;
	TMenuItem *PurgeAllItem;
	TStatusBar *StatusBar;
	TBitBtn *PauseButton;
	TBitBtn *ClearFinishedButton;
	TSpeedButton *LogButton;
	TSpeedButton *OptionsButton;
	TSpeedButton *NewAnonymousKeyButton;
	TSpeedButton *KeyManagerButton;
	TSpeedButton *HelpButton;
//
	void __fastcall SuspendItemClick(TObject *Sender);
	void __fastcall ResumeItemClick(TObject *Sender);
	void __fastcall CancelItemClick(TObject *Sender);
	void __fastcall PurgeAllItemClick(TObject *Sender);
	void __fastcall StopAllItemClick(TObject *Sender);
	void __fastcall SuspendAllItemClick(TObject *Sender);
	void __fastcall JobsListBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall PauseButtonClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ClearFinishedButtonClick(TObject *Sender);
	void __fastcall LogButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OptionsButtonClick(TObject *Sender);
	void __fastcall MainDialogButtonClick(TObject *Sender);
	void __fastcall NewAnonymousKeyButtonClick(TObject *Sender);
	void __fastcall KeyManagerButtonClick(TObject *Sender);
	void __fastcall ShowHint(TObject *Sender);
	void __fastcall HelpButtonClick(TObject *Sender);
public:
	void StatusShow(const AnsiString textstr,bool ishint);
private:	// User declarations
	AnsiString lasthint;
public:		// User declarations
	__fastcall TJobForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TJobForm *JobForm;
//---------------------------------------------------------------------------
#endif
