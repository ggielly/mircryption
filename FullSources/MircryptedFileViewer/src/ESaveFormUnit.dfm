object ESaveForm: TESaveForm
  Left = 368
  Top = 554
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Re-encrypt File with New Key'
  ClientHeight = 139
  ClientWidth = 454
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Button_ok: TBitBtn
    Left = 288
    Top = 110
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Save'
    TabOrder = 2
    OnClick = Button_okClick
    Kind = bkOK
  end
  object Button_cancel: TBitBtn
    Left = 371
    Top = 110
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    TabOrder = 3
    Kind = bkCancel
  end
  object Edit_Entire: TEdit
    Left = 3
    Top = 30
    Width = 445
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object Edit_Substring: TEdit
    Left = 3
    Top = 79
    Width = 445
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    Enabled = False
    TabOrder = 1
  end
  object RadioButton_savetypenews: TRadioButton
    Left = 3
    Top = 63
    Width = 446
    Height = 17
    Caption = 
      'Re-encrypt previously-encrypted substrings with the following ke' +
      'y (use on news .cfg files):'
    TabOrder = 4
    OnClick = RadioButton_savetypenewsClick
  end
  object RadioButton_savetypelog: TRadioButton
    Left = 3
    Top = 14
    Width = 337
    Height = 17
    Caption = 
      'Re-encrypt entire file with the following key (use on log files)' +
      ':'
    Checked = True
    TabOrder = 5
    TabStop = True
    OnClick = RadioButton_savetypelogClick
  end
end
