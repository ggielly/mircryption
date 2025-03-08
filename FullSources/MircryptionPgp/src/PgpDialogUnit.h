//---------------------------------------------------------------------------
#ifndef PgpDialogUnitH
#define PgpDialogUnitH
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>

#include "OptionsFormUnit.h"

#include "JobManager.h"

#include "Dropper.hpp"
#include "PGP2Comp.hpp"
#include "PGPKeyGenerate.hpp"
#include "PGPDecode.hpp"
#include "PGPEncode.hpp"
#include "TJYarrow.h"
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
class TPgpDialogForm : public TForm
{
private:
	AnsiString ProgressString;
	AnsiString lasthint;
	bool showlog_waiting;
	AnsiString channelkeyphrase;
	AnsiString tochan,tonick,toident;
__published:
	// IDE-managed Components
	TSplitter *SplitterFromTo;
	TPanel *PanelTo;
	TRadioGroup *TypeRadioGroup;
	TGroupBox *ContentsGroupBox;
	TMemo *ContentsMemo;
	TBitBtn *CancelButton;
	TBitBtn *SendButton;
	TGroupBox *ToGroupBox;
	TListBox *ListBoxTo;
	TPanel *PanelFrom;
	TGroupBox *FromGroupBox;
	TListBox *ListBoxFrom;
	TPGPPreferences *PGPPreferences1;
	TPGPGetKeyProps *PGPGetKeyPropsPrivate;
	TPGPSetKeyProps *PGPSetKeyProps;
	TPGPKeysGenerate *PGPKeysGenerate;
	TPGPGetKeyProps *PGPGetKeyPropsPublic;
	TJYarrow *JYarrow1;
	TPGPEncode *PGPEncode;
	TPGPDecode *PGPDecode;
	TFileDropper *FileDropper;
	TPopupMenu *ContentsPopupMenu;
	TMenuItem *RandomizeChannelKeyItem;
	TListBox *ListBoxPrivInternal;
	TListBox *ListBoxPublicInternal;
	TPGPKeyImport *PGPKeyImport;
	TPGPKeyExport *PGPKeyExport;
	TSpeedButton *OptionsButton;
	TSpeedButton *HelpButton;
	TSpeedButton *NewAnonymousKeyButton;
	TSpeedButton *KeyManagerButton;
	TSpeedButton *LogButton;
	TStatusBar *StatusBar;
	TSpeedButton *JoblistButton;
	TTimer *OutputTimer;
	//
	void __fastcall TypeRadioGroupClick(TObject *Sender);
	void __fastcall FileDropperDrop(TObject *Sender, AnsiString Filename);
	void __fastcall AppOnMessage(tagMSG &Msg, bool &Handled);
	void __fastcall PGPFailure(int ErrorCode,  const AnsiString ErrorMsg);
	void __fastcall OptionsButtonClick(TObject *Sender);
	void __fastcall NewAnonymousKeyButtonClick(TObject *Sender);
	void __fastcall KeyManagerButtonClick(TObject *Sender);
	void __fastcall LogButtonClick(TObject *Sender);
	void __fastcall JoblistButtonClick(TObject *Sender);
	void __fastcall HelpButtonClick(TObject *Sender);
	void __fastcall PGPKeysGenerateShowState(char State, LongBool &Cancel);
	void __fastcall PGPKeysGenerateGetPassphrase(const PChar Passphrase, const TKeyPropsRec &MasterKeyProps, TMinPassLen MinPassLen, TMinPassQual MinPassQual, LongBool &Cancel);
	void __fastcall PGPKeysGenerateKeyGeneration(const AnsiString NewUserID, const AnsiString NewHexID, LongBool Aborted);
	void __fastcall SendButtonClick(TObject *Sender);
	void __fastcall ListBoxFromClick(TObject *Sender);
	void __fastcall ListBoxToClick(TObject *Sender);
	void __fastcall RandomizeChannelKeyItemClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall SplitterMoved(TObject *Sender);
	void __fastcall JYarrow1Seed(TObject *Sender);
	void __fastcall FixResizeKludge(TObject *Sender);
	void __fastcall IterateJobsButtonClick(TObject *Sender);
	void __fastcall PGPEncodeShowProgress(int BytesProcessed, int BytesTotal);
	void __fastcall PGPEncodeEncoded(const AnsiString BufferOut, const AnsiString FileOut);
	void __fastcall PGPDecodeGetOutputFileName(AnsiString &SuggestedName);
	void __fastcall PGPDecodeGetSignature(const TSigPropsRec &SigProps);
	void __fastcall PGPKeyImportKeyImported(const TKeyPropsList *KeyPropsList, int KeysImported);
	void __fastcall PGPDecodeDecoded(const AnsiString BufferOut, const AnsiString FileOut, const TSigPropsRec &SigProps, const TKeyPropsList *KeyPropsList);
	void __fastcall ContentsMemoExit(TObject *Sender);
	void __fastcall PGPDecodeAddKeys(LongBool &CancelAdding);
	void __fastcall PGPKeyImportGetFileIn(AnsiString &FileIn);
	void __fastcall OutputTimerTimer(TObject *Sender);
	void __fastcall PGPKeyExportKeyExported(const TKeyPropsList *KeyPropsList,
          const AnsiString KeyData, const AnsiString FileOut);
	void __fastcall PGPKeysGenerateKeyGeneration2(const AnsiString NewHexID,
          TKeyPropsRec &MasterKeyProps, LongBool Aborted);
public:
	__fastcall TPgpDialogForm(TComponent* Owner);
	__fastcall ~TPgpDialogForm();
public:
	void Initialize();
	void StatusShow(const AnsiString textstr,bool ishint);
	void PGPUpdateKeyrings();
	void ForceRadioGroup(int index);
private:
	void __fastcall ShowHint(TObject *Sender);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
extern PACKAGE TPgpDialogForm *PgpDialogForm;
//---------------------------------------------------------------------------
#endif
