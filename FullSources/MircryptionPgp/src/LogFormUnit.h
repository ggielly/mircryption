//---------------------------------------------------------------------------

#ifndef LogFormUnitH
#define LogFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TLogForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *BottomPanel;
	TPanel *InPanel;
	TPanel *OutPanel;
	TMemo *OutMemo;
	TMemo *InMemo;
	TEdit *LogEdit;
	TPanel *TopPanel;
	TMemo *LogMemo;
	TCheckBox *FeedbackCheckBox;
	TLabel *Label1;
	TBitBtn *BitBtn1;
	void __fastcall LogEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TLogForm(TComponent* Owner);
public:
	void Present();
};
//---------------------------------------------------------------------------
extern PACKAGE TLogForm *LogForm;
//---------------------------------------------------------------------------
#endif
