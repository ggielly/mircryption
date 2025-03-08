object QuestionForm: TQuestionForm
  Left = 710
  Top = 110
  Width = 486
  Height = 227
  Caption = 'MPGP Question?'
  Color = clBtnFace
  Constraints.MinHeight = 200
  Constraints.MinWidth = 460
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClick = FormClick
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object TimeoutLabel: TLabel
    Left = 5
    Top = 178
    Width = 221
    Height = 13
    Anchors = [akLeft, akRight, akBottom]
    AutoSize = False
    Caption = 'TimeoutLabel'
    OnClick = FormClick
  end
  object YesButton: TBitBtn
    Left = 399
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Yes'
    ModalResult = 6
    TabOrder = 0
    TabStop = False
    OnClick = YesButtonClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object NoButton: TBitBtn
    Left = 319
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'No'
    TabOrder = 1
    TabStop = False
    OnClick = NoButtonClick
    Kind = bkNo
  end
  object WaitButton: TBitBtn
    Left = 239
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Wait..'
    ModalResult = 7
    TabOrder = 2
    TabStop = False
    OnClick = WaitButtonClick
    NumGlyphs = 2
  end
  object Panel1: TPanel
    Left = 3
    Top = 4
    Width = 472
    Height = 161
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 3
    object QuestionMemo: TMemo
      Left = 8
      Top = 8
      Width = 457
      Height = 145
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clActiveBorder
      Lines.Strings = (
        'QuestionMemo')
      ReadOnly = True
      TabOrder = 0
    end
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 8
    Top = 13
  end
end
