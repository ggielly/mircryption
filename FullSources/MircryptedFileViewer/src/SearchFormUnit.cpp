//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SearchFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSearchForm *SearchForm;
//---------------------------------------------------------------------------
__fastcall TSearchForm::TSearchForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::Button_okClick(TObject *Sender)
{
	ModalResult =  mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::Button_cancelClick(TObject *Sender)
{
	ModalResult =  mrCancel;	
}
//---------------------------------------------------------------------------
