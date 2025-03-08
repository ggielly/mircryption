//---------------------------------------------------------------------------

#ifndef ESaveFormUnitH
#define ESaveFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TESaveForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *Button_ok;
	TBitBtn *Button_cancel;
	TEdit *Edit_Entire;
	TEdit *Edit_Substring;
	TRadioButton *RadioButton_savetypenews;
	TRadioButton *RadioButton_savetypelog;
	void __fastcall RadioButton_savetypelogClick(TObject *Sender);
	void __fastcall RadioButton_savetypenewsClick(TObject *Sender);
	void __fastcall Button_okClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TESaveForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TESaveForm *ESaveForm;
//---------------------------------------------------------------------------
#endif
