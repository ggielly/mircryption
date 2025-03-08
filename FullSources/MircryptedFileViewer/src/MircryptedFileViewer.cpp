//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("MainFormUnit.cpp", MainForm);
USEFORM("SearchFormUnit.cpp", SearchForm);
USEFORM("ESaveFormUnit.cpp", ESaveForm);
//---------------------------------------------------------------------------
#include "MainFormUnit.h"
#include "ESaveFormUnit.h"

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TSearchForm), &SearchForm);
		Application->CreateForm(__classid(TESaveForm), &ESaveForm);
		int pcount=ParamCount();
		if (pcount>=1)
			{
			if (pcount>=2)
				MainForm->Edit_EncryptionKey->Text=ParamStr(2);
			if (pcount>=3)
				MainForm->SaveDialog->FileName=ParamStr(3);
			if (pcount>=4)
				ESaveForm->Edit_Entire->Text=ParamStr(4);
			if (ParamStr(1) != ".")
				MainForm->AdvFileNameEdit_File->Text=ParamStr(1);
			Application->ProcessMessages();
			if (pcount>=3)
				{
				MainForm->ReencryptFile(MainForm->AdvFileNameEdit_File->Text,MainForm->SaveDialog->FileName,MainForm->Edit_EncryptionKey->Text,ESaveForm->Edit_Entire->Text,"");
				return (0);
				}
			}

		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
