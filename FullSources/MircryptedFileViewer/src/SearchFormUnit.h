//---------------------------------------------------------------------------

#ifndef SearchFormUnitH
#define SearchFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TSearchForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *Edit_SearchString;
	TBitBtn *Button_cancel;
	TBitBtn *Button_ok;
	TLabel *Label1;
	void __fastcall Button_okClick(TObject *Sender);
	void __fastcall Button_cancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSearchForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSearchForm *SearchForm;
//---------------------------------------------------------------------------
#endif
