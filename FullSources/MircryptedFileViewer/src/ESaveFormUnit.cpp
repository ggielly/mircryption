//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ESaveFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TESaveForm *ESaveForm;
//---------------------------------------------------------------------------
__fastcall TESaveForm::TESaveForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TESaveForm::RadioButton_savetypelogClick(TObject *Sender)
{
	Edit_Substring->Enabled=false;
	Edit_Entire->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TESaveForm::RadioButton_savetypenewsClick(TObject *Sender)
{
	Edit_Substring->Enabled=true;
	Edit_Entire->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TESaveForm::Button_okClick(TObject *Sender)
{
	// re-encrypt and save	
}
//---------------------------------------------------------------------------
