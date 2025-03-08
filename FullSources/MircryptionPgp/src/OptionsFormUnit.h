//---------------------------------------------------------------------------

#ifndef OptionsFormUnitH
#define OptionsFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TOptionsForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *KeysizeRadioGroup;
	TRadioGroup *RandKeyRadioGroup;
	TGroupBox *PgpDirectoriesGroupbox;
	TEdit *KeyManagerEdit;
	TEdit *KeyringDirectoryEdit;
	TLabel *Label1;
	TLabel *Label2;
	TButton *ResetOptionsButton;
	TGroupBox *SpeedGroupBox;
	TLabel *Label3;
	TMaskEdit *SpeedEdit;
	TBitBtn *OkButton;
	TGroupBox *GroupBox2;
	TLabel *Label6;
	TLabel *Label7;
	TMaskEdit *QuestionTimeoutEdit;
	TMaskEdit *MessageTimeoutEdit;
	TGroupBox *GroupBox3;
	TCheckBox *AutoShowJobsCheckBox;
	TCheckBox *AutoHideJobsCheckBox;
	TCheckBox *AutoClearJobsCheckBox;
	TRadioGroup *ArmouringRadioGroup;
	TLabel *Label8;
	TMaskEdit *BurstEdit;
	TLabel *Label9;
	TMaskEdit *TimerEdit;
	TRadioGroup *IncomingAnonymousRequests;
	TLabel *Label4;
	TMaskEdit *TimeoutEdit;
	TSpeedButton *LogButton;
	TSpeedButton *NewAnonymousKeyButton;
	TSpeedButton *KeyManagerButton;
	TSpeedButton *JoblistButton;
	TSpeedButton *HelpButton;
	void __fastcall ResetOptionsButtonClick(TObject *Sender);
	void __fastcall OkButtonClick(TObject *Sender);
	//
	void __fastcall NewAnonymousKeyButtonClick(TObject *Sender);
	void __fastcall KeyManagerButtonClick(TObject *Sender);
	void __fastcall LogButtonClick(TObject *Sender);
	void __fastcall JoblistButtonClick(TObject *Sender);
	void __fastcall HelpButtonClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TOptionsForm(TComponent* Owner);
	void UpdateOptions();
public:
	void Present();
};
//---------------------------------------------------------------------------
extern PACKAGE TOptionsForm *OptionsForm;
//---------------------------------------------------------------------------
#endif
