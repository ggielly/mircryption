object SearchForm: TSearchForm
  Left = 366
  Top = 107
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Search For'
  ClientHeight = 96
  ClientWidth = 385
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
  object Label1: TLabel
    Left = 3
    Top = 13
    Width = 252
    Height = 13
    Caption = 'Type in strings to search for, separated by semicolons'
  end
  object Edit_SearchString: TEdit
    Left = 3
    Top = 32
    Width = 374
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object Button_cancel: TBitBtn
    Left = 301
    Top = 61
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    TabOrder = 1
    OnClick = Button_cancelClick
    Kind = bkCancel
  end
  object Button_ok: TBitBtn
    Left = 218
    Top = 61
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    TabOrder = 2
    OnClick = Button_okClick
    Kind = bkOK
  end
end
