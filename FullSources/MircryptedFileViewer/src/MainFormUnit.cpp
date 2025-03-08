//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <stdio.h>
#include "MainFormUnit.h"
#include "SearchFormUnit.h"
#include "ESaveFormUnit.h"
#include "mc_blowfish.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AdvFileNameEdit"
#pragma link "Dropper"
#pragma link "AdvListV"
#pragma link "AdvListV"
#pragma link "AdvDirectoryEdit"
#pragma link "Plusmemo"
#pragma link "ExtHilit"
#pragma link "HtmlHighlight"
#pragma link "PMSupport"
#pragma link "URLHighlight"
#pragma link "PlusMemo"
#pragma link "UrlHighlight"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TMainForm *MainForm;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define MAXLINELEN		3000
#define MAXKEYSIZE		128
#define MCPS2_STARTTAG	"\xABm\xAB"
#define MCPS2_ENDTAG	"\xBBm\xBB"
#define MCPS2_CRC		"@@"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	initialized=false;
	// constructor
	highlightsearch=false;
	searchstring="";
	// load any settings
	LoadSettings();
	initialized=true;
        // check localization
        char *mpos,*dpos;
        char shortdatestr[255];
        strcpy(shortdatestr,ShortDateFormat.c_str());
        mpos=strchr(shortdatestr,'m');
        if (mpos==NULL)
        	mpos=strchr(shortdatestr,'M');
        dpos=strchr(shortdatestr,'d');
        if (dpos==NULL)
        	dpos=strchr(shortdatestr,'D');
        if (mpos>dpos)
	        EuroStyleDate=true;
        else
        	EuroStyleDate=false;
}


void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// save settings
	myfinder.set_wantsquit(true);
	SaveSettings();
}

void __fastcall TMainForm::FormResize(TObject *Sender)
{
	// main form gets resized
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool TMainForm::LoadSettings()
{
	// open ini file
	AnsiString filename="MircryptedFileViewer.ini";
	bool bretv;

	TIniFile *ini = new TIniFile(ExtractFilePath(Application->ExeName)+filename);
	if (!ini)
		{
        	return false;
	        }

	// load settings
	try
		{
		// load info
		AnsiString version;
		version=ini->ReadString("Information","File Identification","");
		if (version!="Mircrypted File Viewer")
			{
			delete ini;
			return false;
			}
		Left=ini->ReadInteger("Windows","MainLeft",Left);
		Top=ini->ReadInteger("Windows","MainTop",Top);
		Width=ini->ReadInteger("Windows","MainWidth",Width);
		Height=ini->ReadInteger("Windows","MainHeight",Height);
		Panel_Browser->Height=ini->ReadInteger("Windows","PanelBrowserHeight",Panel_Browser->Height);
		AdvDirectoryEdit->Text=ini->ReadString("Options","Directory",AdvDirectoryEdit->Text);
		Edit_FileMask->Text=ini->ReadString("Options","FileMask",Edit_FileMask->Text);
		Edit_HighlightString->Text=ini->ReadString("Options","HighlightStrings",Edit_HighlightString->Text);
		CheckBox_RecurseDirs->Checked=ini->ReadBool("Options","RecurseDirs",CheckBox_RecurseDirs->Checked);
		}
	catch (...)
		{
		delete ini;
		return false;
		}

	delete ini;
	return true;
}

bool TMainForm::SaveSettings()
{
    TIniFile *ini;
	bool bretv;
	AnsiString filename="MircryptedFileViewer.ini";

	// open ini file
	ini = new TIniFile(ExtractFilePath(Application->ExeName)+filename);
    if (!ini)
       	{
        return false;
        }

	// save settings
	try
		{
		// save info
		ini->WriteString("Information","File Identification","Mircrypted File Viewer");
		ini->WriteInteger("Windows","MainLeft",Left);
		ini->WriteInteger("Windows","MainTop",Top);
		ini->WriteInteger("Windows","MainWidth",Width);
		ini->WriteInteger("Windows","MainHeight",Height);
		ini->WriteInteger("Windows","PanelBrowserHeight",Panel_Browser->Height);
		ini->WriteString("Options","Directory",AdvDirectoryEdit->Text);
		ini->WriteString("Options","FileMask",Edit_FileMask->Text);
		ini->WriteString("Options","HighlightStrings",Edit_HighlightString->Text);
		ini->WriteBool("Options","RecurseDirs",CheckBox_RecurseDirs->Checked);
		}
	catch (...)
		{
		delete ini;
        return false;
		}

	delete ini;
	return true;

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvFileNameEdit_FileChange(TObject *Sender)
{
	UncryptFile(AdvFileNameEdit_File->Text);
}

void __fastcall TMainForm::FileDropperDrop(TObject *Sender, AnsiString Filename)
{
	AdvFileNameEdit_File->Text=Filename;
}

void __fastcall TMainForm::Edit_EncryptionKeyKeyPress(TObject *Sender, char &Key)
{
	if (Key==13)
		{
		Search(false);
		UncryptFile(AdvFileNameEdit_File->Text);
		}
}

void __fastcall TMainForm::AdvFileNameEdit_FileKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (Key==13)
		UncryptFile(AdvFileNameEdit_File->Text);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TMainForm::SpeedButton_SaveClick(TObject *Sender)
{
	SaveDialog->Title="Save decrypted file as..";
	SaveDialog->FileName=AdvFileNameEdit_File->Text+".txt";
	if (SaveDialog->Execute())
		{
		// save the file
		TFileStream *filestreamp;
		filestreamp=new TFileStream(SaveDialog->FileName,fmCreate);
		FileMemo->SaveToStream(filestreamp);
		delete filestreamp;
		}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TMainForm::Edit_HighlightStringKeyPress(TObject *Sender, char &Key)
{
	searchstring=Edit_HighlightString->Text;
	if (Key==13)
		{
		HighlightEditor();
		}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Edit_FileMaskKeyPress(TObject *Sender, char &Key)
{
	if (Key==13)
		Search(false);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvDirectoryEditKeyPress(TObject *Sender, char &Key)
{
	if (Key==13)
		Search(false);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvDirectoryEditChange(TObject *Sender)
{
	// we need to disable this if they are typing
	TWinControl *Temp = dynamic_cast<TWinControl *>(AdvDirectoryEdit);
	if (!Temp->Focused())
		Search(false);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckBox_RecurseDirsClick(TObject *Sender)
{
	Search(false);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button_SearchClick(TObject *Sender)
{
	if (Button_Search->Caption=="&Search Files")
		Search(true);
	else
		myfinder.set_wantsquit(true);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void TMainForm::UncryptFile(AnsiString filename)
{
	// uncrypt the file
	CryptSearchFile(filename,true);
	FileMemo->SelStart=0;
	FileMemo->SelLength=0;
	FileMemo->ScrollInView();
	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvListView_BrowserDblClick(TObject *Sender)
{
	// double click on a filename should load it in viewer
	if (AdvListView_Browser->Selected!=NULL)
		AdvFileNameEdit_File->Text=AdvListView_Browser->Selected->Caption;
}
//---------------------------------------------------------------------------





void TMainForm::HighlightEditor()
{
	// highlight words in editor
	AnsiString keywordstr;
	int count;

	keywordstr=Edit_HighlightString->Text;
	if (keywordstr=="")
		{
		FileMemo->Keywords->Clear();
		FileMemo->ReApplyKeywords();
		return;
		}

	TStringList *highstringlist;
	highstringlist=new TStringList;
	for (count=1;count<=keywordstr.Length();++count)
		{
		if (keywordstr[count]==';')
			keywordstr[count]=',';
		}
	highstringlist->CommaText=keywordstr.LowerCase();

	FileMemo->Keywords->Clear();

	//
	TWordOptions keywordoptions;
	TExtFontStyles fontstyles;
	TCursor cursortype;
	TColor backgrndc=clWhite;
	TColor foregrndc=clRed;
	int contextnum=1;
//	fontstyles << TExtFontStyle(fsHighlight);

	for (count=0;count<highstringlist->Count;++count)
		{
		FileMemo->Keywords->AddKeyWord(highstringlist->Strings[count],keywordoptions,fontstyles,contextnum,cursortype,backgrndc,foregrndc);
		}

	FileMemo->ReApplyKeywords();

	delete highstringlist;
}








//---------------------------------------------------------------------------
void TMainForm::Search(bool highlight)
{
	// find all matching files, and add them to browse list.
	// search contents for highlight strings ig highlight==true
	AnsiString filemasklist;

	StatusBar->SimpleText="";

	if (Button_Search->Caption!="&Search Files")
		return;
	if (!initialized)
		return;

	highlightsearch=highlight;

	// clear listview
	AdvListView_Browser->Items->Clear();

	if (AdvDirectoryEdit->Text=="")
		return;

	Button_Search->Caption="CANCEL Search";

	// fast searcher
	myfinder.set_wantsquit(false);
	long filecount=0;
	long foldercount=0;
	long smallestfile=-1;
	long biggestfile=-1;
	double allsize;
	allsize=myfinder.RecursiveFileSize(string(AdvDirectoryEdit->Text.c_str()), string(Edit_FileMask->Text.c_str()), CheckBox_RecurseDirs->Checked, filecount, foldercount, smallestfile, biggestfile);
	StatusBar->SimpleText="Scanned "+AnsiString(foldercount)+" directories.  Found "+AnsiString(filecount)+" files totaling "+AnsiString((myfinder.get_bytestring(allsize,false)).c_str())+".  Smallest file was "+AnsiString((myfinder.get_bytestring(smallestfile,false)).c_str())+".  Biggest file was "+AnsiString((myfinder.get_bytestring(biggestfile,false)).c_str())+".";
	DoneSearch();
}


void TMainForm::DoneSearch()
{
	// resize to force scrollbar listview kludge fix _ GOD DAMN FUCKING THING
	FixTimer->Enabled=true;
//	AdvFileNameEdit_FileChange(this);
	Button_Search->Caption="&Search Files";
}

void TMainForm::FoundAFile(AnsiString file)
{
    TListItem *t1;
	t1=AdvListView_Browser->Items->Add();
	t1->Caption=file;

	lastfiledatestring=FileDateToDateTime(FileAge(file)).DateString();

	if (highlightsearch)
		{
		// load the file and examine it
		CryptSearchFile(file,false);
		char sizestr[40];
		sprintf(sizestr,"%12.1fkb",lastfilesize/1024);
		AnsiString filesizestr=AnsiString(sizestr);
//		AnsiString filesizestr=AnsiString((myfinder.get_bytestring(lastfilesize,false)).c_str());

		t1->SubItems->Add(lastfiledatestring);
		t1->SubItems->Add(filesizestr);
		if (baddecryptedlines>0 && gooddecryptedlines>0)
			t1->SubItems->Add("partial");
		else if (gooddecryptedlines>0)
			t1->SubItems->Add("decoded");
		else if (baddecryptedlines>0)
			t1->SubItems->Add("crypted");
		else if (Edit_EncryptionKey->Text!="")
			t1->SubItems->Add("no");
		else
			t1->SubItems->Add("");
		// seach for text
		t1->SubItems->Add(AnsiString(hitcount));
		}
	else
		{
		char sizestr[40];
		sprintf(sizestr,"%12.1fkb",lastfilesize/1024);
		AnsiString filesizestr=AnsiString(sizestr);
//		AnsiString filesizestr=AnsiString((myfinder.get_bytestring(lastfilesize,false)).c_str());
		t1->SubItems->Add(lastfiledatestring);
		t1->SubItems->Add(filesizestr);
		t1->SubItems->Add("");
		t1->SubItems->Add("");
		}
}

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void __fastcall TMainForm::FixTimerTimer(TObject *Sender)
{
	// fix timer
//	AdvListView_Browser->Columns->Items[0]->AutoSize=true;
	Width=Width-1;
	Width=Width+1;
	FixTimer->Enabled=false;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void TMainForm::CryptSearchFile(AnsiString filename,bool fillmemo)
{
	// uncrypt the file
	AnsiString keystring;
	AnsiString encryptedstring;
	AnsiString decryptedstring;
	char *key;
	int linecount=0;
	int count;
	char returnstr[MAXLINELEN+200];
	char encrypline[MAXLINELEN];
	bool bretv;
	FILE *filep;
	int len;
	int lsize;
	bool stopdecrypting=false;

	if (fillmemo)
		FileMemo->Clear();

        if (fillmemo)
                StatusBar->SimpleText="Loading file "+filename+" ...";

	gooddecryptedlines=0;
	baddecryptedlines=0;
	hitcount=0;

	if (filename=="")
		return;
	if (!FileExists(filename))
		return;

	// stringlist of keywords
	TStringList *highstringlist;
	AnsiString keywordstr;
	highstringlist=new TStringList;
	keywordstr=Edit_HighlightString->Text;
	if (keywordstr!="")
		{
		for (count=1;count<=keywordstr.Length();++count)
			{
			if (keywordstr[count]==';')
				keywordstr[count]=',';
			}
		highstringlist->CommaText=keywordstr.LowerCase();
		}
	lsize=highstringlist->Count;

	// init key
	keystring=Edit_EncryptionKey->Text;
	key=keystring.c_str();

	// load file and decrypt through it
	filep=fopen(filename.c_str(),"rb");

	while (!feof(filep))
		{
		fgets(encrypline,MAXLINELEN,filep);
		if feof(filep)
			break;
		++linecount;
		if (encrypline[0]!='\0')
			{
			len=strlen(encrypline)-1;
			if (encrypline[len]==13)
				--len;
			if (encrypline[len]==10)
				--len;
			encrypline[len]='\0';
			}

		if (encrypline[0]=='\0')
			{
			if (fillmemo)
				FileMemo->Lines->Append("");
			continue;
			}

		if (key[0]!='\0' && !stopdecrypting)
			bretv=mc_decrypt2(key,encrypline,returnstr);
		else
			{
			strcpy(returnstr,encrypline);
			bretv=true;
			}

		if (bretv==false)
			{
			if (fillmemo)
				FileMemo->Lines->Append(encrypline);
			++baddecryptedlines;
			}
		else if (returnstr[0]=='\0')
			{
			if (fillmemo)
				FileMemo->Lines->Append(encrypline);
			++baddecryptedlines;
			}
		else if (strcmp(returnstr,encrypline)!=0)
			{
			if (fillmemo)
				FileMemo->Lines->Append(returnstr);
			++gooddecryptedlines;
			}
		else
			{
			if (fillmemo)
				FileMemo->Lines->Append(returnstr);
			}

		// now search in returnstr for highlightable keywords
		len=strlen(returnstr);
		for (count=0;count<len;++count)
			returnstr[count]=tolower(returnstr[count]);
		for (count=0;count<lsize;++count)
			{
            if (strstr(returnstr,(highstringlist->Strings[count]).c_str())!=NULL)
				{
				// we count number of hit lines, not total hit count
				++hitcount;
				break;
				}
			}

		if (baddecryptedlines>1 && !fillmemo)
			{
			stopdecrypting=true;
			if (lsize==0)
				{
				// no point continuing
				break;
				}
			}
		}

	lastfilesize=ftell(filep);
	fclose(filep);
	delete highstringlist;

	if (fillmemo)
		{
		HighlightEditor();
		AnsiString hitstring="";
		if (hitcount>0)
			hitstring=AnsiString("  ")+AnsiString(hitcount)+" hits.";
		AnsiString filesizestr=AnsiString((myfinder.get_bytestring(lastfilesize,false)).c_str());
		if (gooddecryptedlines>0)
			StatusBar->SimpleText="DECRYPTED FILE "+filename+" - "+AnsiString(linecount)+" total lines ("+filesizestr+")."+hitstring;
		else
			StatusBar->SimpleText="File "+filename+" - "+AnsiString(linecount)+" total lines ("+filesizestr+")."+hitstring;
		lastfiledatestring=FileDateToDateTime(FileAge(filename)).DateString();
		}
}
//---------------------------------------------------------------------------
























//---------------------------------------------------------------------------
bool TMainForm::mc_decrypt2 (char *key, char *text, char *returndata)
{
	char decodedtext[MAXLINELEN];
	char decodedsubstring[MAXLINELEN];
	char *p,*p2;
	unsigned char fc2;
	bool bretv=true;

	// legacy decrypt of style 1

	if (strncmp(text,"mcps ",5)==0)
		{
		char* p = decrypt_string(key,text+5);
		strcpy(returndata,p);
		delete p;
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		return bretv;
		}

	// should we also decrypt the "OK+" format of blowcrypt or bnc blowfish?
	if (false && strncmp(text,"+OK ",4)==0)
		{
		char* p = decrypt_string(key,text+4);
		strcpy(returndata,p);
		delete p;
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		return bretv;
		}

	// now loop through replacing substrings of encoded text (which may be nonexistent)
	strcpy(decodedtext,text);
	p=decodedtext;
	for(;;)
		{
		// find leftmost substring
		p=strstr(p,MCPS2_STARTTAG);
		if (p==NULL)
			break;
		p2=strstr(p,MCPS2_ENDTAG);
		if (p2!=NULL)
			{
			// cap substring
			*p2='\0';
			}
		else
			{
			// truncated string
			p2=p+strlen(p);
			}

		// decode substring
		decrypt2_substring(p+strlen(MCPS2_STARTTAG),decodedsubstring,key);
		if (strlen(decodedsubstring)==0)
			bretv=false;
		// add right half of string to decoded substring
		strcat(decodedsubstring,p2+strlen(MCPS2_ENDTAG));
		// cap main string before first tag
		*p='\0';
		// now add left half to right half
		strcat(decodedtext,decodedsubstring);
		// advance p to find next encrypted substring
		p=decodedtext+strlen(decodedsubstring);
		}

	//sprintf(returndata,"'%s' [%s] '%s' firstchar=%d",MCPS2_STARTTAG,decodedtext,MCPS2_ENDTAG,fc);
	strcpy(returndata,decodedtext);
	return bretv;
}


void TMainForm::decrypt2_substring(char *insubstring,char *outstring, char *key)
{
	// given a substring which has start and end tags removed, decrypt it and store result in outstring

	// undo whitespace conversion created during encryption
	unrepwhitespaces(insubstring);

	// decrypt it using provided key
	char* p = search_decrypt_string(key,insubstring);
	// copy to output
	if (p!=NULL)
		{
		// copy the result, ABSENT the initial crc tag
		strcpy(outstring,p+strlen(MCPS2_CRC));
		delete p;
		}
	else
		strcpy(outstring,"");
}


char* TMainForm::search_decrypt_string(char *key, char *instring)
{
	// this is a wrapper for the blowfish command decrypt_string(char *key,char *instring) which can search through all decryption keys if the suggested one doesnt work
	// this is useful if we have some text and dont know which key to apply, though can be a bit slow if you have a lot of keys.
	char* p;
	char mcps2crcstr[10];
	int mcps2crclen;
	
	// crc tag
	strcpy(mcps2crcstr,MCPS2_CRC);
	mcps2crclen=strlen(mcps2crcstr);
	
		// decrypt it
		p = decrypt_string(key,instring);
		if (p==NULL)
			{
			// did not work
			}
		else if (strncmp(p,mcps2crcstr,mcps2crclen)==0)
			{
			// here we check if decryption is valid by checking a tag which will be a certain value if decryption is successful
			// ok, successfully decrypted, return pointer (caller should delete p when we return!)
			}
		else
			{
			// key did not work, do try another
			// ATTN: test - i would like to see decrypted no matter if fail
			// delete p;
			// p=NULL;
			}

	return p;
}


void TMainForm::unrepwhitespaces(char *str)
{
	// unconvert all whitespace replaced characters above to original whitespaced
	char *p=str;
	char c;
	for (;;)
		{
		c=*p;
		if (c=='\0')
			break;
		if (c==(char)162)
			*p=(char)9;
		else if (c==(char)163)
			*p=(char)10;
		else if (c==(char)164)
			*p=(char)13;
		else if (c==(char)165)
			*p=(char)32;
		++p;
		}
}

void TMainForm::repwhitespaces(char *str)
{
	// given a blowfish encoded string str, convert all whitespaces to non-64bit (blowfish used) replacement characters
	char *p=str;
	char c;
	for (;;)
		{
		c=*p;
		if (c=='\0')
			break;
		if (c==9)
			*p=(char)162;
		else if (c==10)
			*p=(char)163;
		else if (c==13)
			*p=(char)164;
		else if (c==32)
			*p=(char)165;
		++p;
		}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



void __fastcall TMainForm::FileMemoKeyDown(TObject *Sender, WORD &Key,  TShiftState Shift)
{
//	ShowMessage("Key pressed = "+AnsiString(Key));

	if (Shift.Contains(ssCtrl) && Key==70)
		{
		// Ctrl+F
		// ask user for new search string?
		SearchForm->Edit_SearchString->Text=searchstring;
		SearchForm->Edit_SearchString->SelectAll();
		int retv=SearchForm->ShowModal();
		if (retv==mrOk)
			{
			searchstring=SearchForm->Edit_SearchString->Text;
			FindNextSearchString();
			}
		}

	if (Key==114)
		{
		// find next search string occurence
		FindNextSearchString();
		}
}


void TMainForm::FindNextSearchString()
{
	AnsiString keywordstr;
	int count;

	keywordstr=searchstring;
	if (keywordstr=="")
		{
		return;
		}

	TStringList *highstringlist;
	highstringlist=new TStringList;
	for (count=1;count<=keywordstr.Length();++count)
		{
		if (keywordstr[count]==';')
			keywordstr[count]=',';
		}
	highstringlist->CommaText=keywordstr.LowerCase();

	// find nearest of any keyword
	int curpos=FileMemo->SelStart;
	int oldlen=FileMemo->SelLength;
	int fpos;
	int smallestpos=-1;
	int len;

	if (curpos<=0)
		curpos=1;

	FileMemo->SelLength=0;
	FileMemo->SelStart=curpos+1;

	for (count=0;count<highstringlist->Count;++count)
		{
		FileMemo->FindTxt(highstringlist->Strings[count],true,false,false,false);
		fpos=FileMemo->SelStart;
		if ((fpos<smallestpos || smallestpos==-1) && (fpos>curpos+1))
			{
			smallestpos=fpos;
			len=highstringlist->Strings[count].Length();
			}
		FileMemo->SelLength=0;
		FileMemo->SelStart=curpos+1;
		}

	if (smallestpos!=-1)
		{
		// we found a position
		FileMemo->SelStart=smallestpos-len;
		FileMemo->SelLength=len;
		}
	else
		{
		// none found, so we start from top?
		FileMemo->SelStart=curpos;
		FileMemo->SelLength=oldlen;
		}

	FileMemo->ScrollInView();
	delete highstringlist;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TMainForm::Edit_HighlightStringChange(TObject *Sender)
{
	searchstring=Edit_HighlightString->Text;	
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::FormPaint(TObject *Sender)
{
	static bool firstdraw=true;

	if (firstdraw)
		{
		firstdraw=false;
		Search(false);
		}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvListView_BrowserGetFormat(TObject *Sender,
      int ACol, TSortStyle &AStyle, AnsiString &aPrefix,
      AnsiString &aSuffix)
{
	if (ACol==-1)
		AStyle=ssAlphabetic;
	else if (ACol==0)
                {
                if (EuroStyleDate)
         		AStyle=ssShortDateEU;
                else
                        AStyle=ssShortDateUS;
                }
	else if (ACol==1)
 		AStyle=ssAlphabetic;
	else
 		AStyle=ssNumeric;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SpeedButton_eSaveClick(TObject *Sender)
{
	// re-encrypt the file
	int retv;
	bool bretv=false;
	SaveDialog->Title="Save re-encrypted file as.. (you will be prompted for new passphrase)";
	SaveDialog->FileName=AdvFileNameEdit_File->Text+".crypted";
	if (SaveDialog->Execute())
		{
		if (AdvFileNameEdit_File->Text != SaveDialog->FileName)
			{
			retv=ESaveForm->ShowModal();
			if (retv=mrOk)
				{
				if (ESaveForm->RadioButton_savetypelog->Checked && ESaveForm->Edit_Entire->Text!="")
					bretv=ReencryptFile(AdvFileNameEdit_File->Text,SaveDialog->FileName,Edit_EncryptionKey->Text,ESaveForm->Edit_Entire->Text,"");
				else if (ESaveForm->Edit_Substring->Text!="")
					bretv=ReencryptFile(AdvFileNameEdit_File->Text,SaveDialog->FileName,Edit_EncryptionKey->Text,"",ESaveForm->Edit_Substring->Text);
				if (!bretv)
					ShowMessage("Error saving file.  File was not saved.");
				}
			}
		}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool TMainForm::ReencryptFile(AnsiString sourcefile, AnsiString targetfile, AnsiString decryptpass, AnsiString fullpass, AnsiString subpass)
{
	FILE *filep,*ofilep;
	char returnstr[MAXLINELEN+200];
	char encrypline[MAXLINELEN+200];
	bool fullemode;
	char *key;
	char *newkey;
	int len;
	bool bretv;

	// encrypt mode
	if (fullpass!="")
		{
		fullemode=true;
		newkey=fullpass.c_str();
		}
	else
		{
		fullemode=false;
		newkey=subpass.c_str();
		}

	// decrypt pass
	key=decryptpass.c_str();

	// load file and decrypt through it
	filep=fopen(sourcefile.c_str(),"rb");
	if (filep==NULL)
		return false;
	ofilep=fopen(targetfile.c_str(),"wb");
	if (ofilep==NULL)
		{
		fclose(filep);
		return false;
		}

	while (!feof(filep))
		{
		fgets(encrypline,MAXLINELEN,filep);
		if feof(filep)
			break;

		if (encrypline[0]!='\0')
			{
			len=strlen(encrypline)-1;
			if (encrypline[len]==13)
				--len;
			if (encrypline[len]==10)
				--len;
			encrypline[len]='\0';
			}

		if (fullemode)
			{
			// decrypt it
			if (key[0]!='\0')
				{
				bretv=mc_decrypt2(key,encrypline,returnstr);
				if (bretv)
					strcpy(encrypline,returnstr);
				}
			// re-encrypt
			bretv=mc_encrypt2(newkey,encrypline,returnstr);
			}
		else
			{
			// replace key
			bretv=mc_redecrypt2(key,newkey,encrypline,returnstr);
			}

		// now write the line
		fputs(returnstr,ofilep);
		fputs("\r\n",ofilep);
		}

	lastfilesize=ftell(filep);
	fclose(filep);
	fclose(ofilep);
	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool TMainForm::mc_redecrypt2 (char *key, char *newkey, char *text, char *returndata)
{
	char decodedtext[MAXLINELEN];
	char decodedsubstring[MAXLINELEN];
	char *p,*p2;
	unsigned char fc2;
	bool bretv=true;
	char oldc;

	// legacy decrypt of style 1

	if (strncmp(text,"mcps ",5)==0)
		{
		char* p = decrypt_string(key,text+5);
		strcpy(returndata,p);
		delete p;
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		return bretv;
		}

	// should we also decrypt the "OK+" format of blowcrypt or bnc blowfish?
	if (false && strncmp(text,"+OK ",4)==0)
		{
		char* p = decrypt_string(key,text+4);
		strcpy(returndata,p);
		delete p;
		// ATTN: should we verify from decrypt_string whether decrypt was successful or no?
		return bretv;
		}

	// now loop through replacing substrings of encoded text (which may be nonexistent)
	strcpy(decodedtext,text);
	p=decodedtext;
	for(;;)
		{
		// find leftmost substring
		p=strstr(p,MCPS2_STARTTAG);
		if (p==NULL)
			break;
		p2=strstr(p,MCPS2_ENDTAG);
		if (p2!=NULL)
			{
			oldc=*p2;
			*p2='\0';
			// cap substring
			}
		else
			{
			// truncated string
			bretv=false;
			break;
			}

		// decode substring
		decrypt2_substring(p+strlen(MCPS2_STARTTAG),decodedsubstring,key);
		if (strlen(decodedsubstring)==0)
			{
			*p2=oldc;
			bretv=false;
			break;
			}

		// now re-encrypt it
		mc_encrypt2(newkey,decodedsubstring,returndata);
		strcpy(decodedsubstring,returndata);
		// add right half of string to decoded substring
		strcat(decodedsubstring,p2+strlen(MCPS2_ENDTAG));
		// cap main string before first tag
		*(p)='\0';
		// now add left half to right half
		strcat(decodedtext,decodedsubstring);
		p=decodedtext+strlen(decodedsubstring);
		}

	//sprintf(returndata,"'%s' [%s] '%s' firstchar=%d",MCPS2_STARTTAG,decodedtext,MCPS2_ENDTAG,fc);
	strcpy(returndata,decodedtext);
	return bretv;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool TMainForm::mc_encrypt2 (char *key, char *text, char *returndata)
{
	if (key[0]!='\0')
		{
		char plaintext[MAXLINELEN];
		// first encrypt the (crc+text)
		strcpy(plaintext,MCPS2_CRC);
		strcat(plaintext,text);
		char* p = encrypt_string(key,plaintext);
		// now write it to data var, with start and end tages
		sprintf(returndata,"%s%s%s",MCPS2_STARTTAG,p,MCPS2_ENDTAG);
		// now convert whitespaces to replacements
		repwhitespaces(returndata);
		// delete encrypted pointer
		delete p;
		}
	else
		strcpy(returndata,text);

	return true;
}
//---------------------------------------------------------------------------


