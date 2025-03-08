//---------------------------------------------------------------------------

#ifndef MainFormUnitH
#define MainFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "AdvEdBtn.hpp"
#include "AdvEdit.hpp"
#include "AdvFileNameEdit.hpp"
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include "Dropper.hpp"
#include <ComCtrls.hpp>
#include "Advlistv.hpp"
#include "AdvListV.hpp"
#include "AdvDirectoryEdit.hpp"
#include "Plusmemo.hpp"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "RecursiveFileSizer.h"
#include "ExtHilit.hpp"
#include "HtmlHighlight.hpp"
#include "PMSupport.hpp"
#include "URLHighlight.hpp"
#include "PlusMemo.hpp"
#include "UrlHighlight.hpp"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
protected:
	bool initialized;
	bool highlightsearch;
	int gooddecryptedlines;
	int baddecryptedlines;
	int hitcount;
	AnsiString searchstring;
	double lastfilesize;
	AnsiString lastfiledatestring;
	bool EuroStyleDate;
protected:
	DirFinder myfinder;
__published:	// IDE-managed Components
	TPanel *Panel_File;
	TPanel *Panel_Top;
	TEdit *Edit_EncryptionKey;
	TLabel *Label_Keyphrase;
	TSaveDialog *SaveDialog;
	TFileDropper *FileDropper1;
	TFileDropper *FileDropper2;
	TStatusBar *StatusBar;
	TLabel *Label_Filename;
	TAdvFileNameEdit *AdvFileNameEdit_File;
	TSpeedButton *SpeedButton_Save;
	TPanel *Panel_Browser;
	TSplitter *Splitter1;
	TAdvListView *AdvListView_Browser;
	TLabel *Label_HighlightingString;
	TEdit *Edit_HighlightString;
	TLabel *Label_LogDirectory;
	TAdvDirectoryEdit *AdvDirectoryEdit;
	TLabel *Label1;
	TEdit *Edit_FileMask;
	TBitBtn *Button_Search;
	TCheckBox *CheckBox_RecurseDirs;
	TTimer *FixTimer;
	TPlusMemo *FileMemo;
	TFileDropper *FileDropper3;
	TFileDropper *FileDropper4;
	TSpeedButton *SpeedButton_eSave;
        TURLHighlighter *URLHighlighter1;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall SpeedButton_SaveClick(TObject *Sender);
	void __fastcall FileDropperDrop(TObject *Sender, AnsiString Filename);
	void __fastcall AdvFileNameEdit_FileChange(TObject *Sender);
	void __fastcall Edit_EncryptionKeyKeyPress(TObject *Sender, char &Key);
	void __fastcall AdvFileNameEdit_FileKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Edit_HighlightStringKeyPress(TObject *Sender, char &Key);
	void __fastcall Edit_FileMaskKeyPress(TObject *Sender, char &Key);
	void __fastcall AdvDirectoryEditKeyPress(TObject *Sender, char &Key);
	void __fastcall AdvDirectoryEditChange(TObject *Sender);
	void __fastcall CheckBox_RecurseDirsClick(TObject *Sender);
	void __fastcall Button_SearchClick(TObject *Sender);
	void __fastcall AdvListView_BrowserDblClick(TObject *Sender);
	void __fastcall FixTimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FileMemoKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Edit_HighlightStringChange(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall AdvListView_BrowserGetFormat(TObject *Sender, int ACol,
          TSortStyle &AStyle, AnsiString &aPrefix, AnsiString &aSuffix);
	void __fastcall SpeedButton_eSaveClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
public:
	bool LoadSettings();
	bool SaveSettings();
public:
	void Search(bool highlight);
	void FoundAFile(AnsiString file);
	void DoneSearch();
public:
	void UncryptFile(AnsiString filename);
	void CryptSearchFile(AnsiString filename,bool fillmemo);
	void HighlightEditor();
	void FindNextSearchString();
public:
	bool mc_decrypt2 (char *key, char *text, char *returndata);
	void decrypt2_substring(char *insubstring,char *outstring, char *key);
	char* search_decrypt_string(char *inkey, char *instring);
	void repwhitespaces(char *str);
	void unrepwhitespaces(char *str);
	bool mc_encrypt2 (char *key,char *text, char *returndata);
	bool mc_redecrypt2 (char *key, char *newkey, char *text, char *returndata);
public:
	void set_lastfilesize(double val) {lastfilesize=val;};
	void set_lastfiledatestring(AnsiString val) {lastfiledatestring=val;};
public:
	bool ReencryptFile(AnsiString sourcefile, AnsiString targetfile, AnsiString decryptpass, AnsiString fullpass, AnsiString subpass);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
