object LogForm: TLogForm
  Left = 408
  Top = 347
  Width = 597
  Height = 429
  Caption = 'Debug Log Window'
  Color = clBtnFace
  Constraints.MinHeight = 300
  Constraints.MinWidth = 440
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object BottomPanel: TPanel
    Left = 0
    Top = 58
    Width = 589
    Height = 344
    Align = alBottom
    BevelOuter = bvNone
    Constraints.MinHeight = 50
    Constraints.MinWidth = 300
    TabOrder = 0
    object InPanel: TPanel
      Left = 0
      Top = 0
      Width = 347
      Height = 344
      Align = alLeft
      BevelOuter = bvNone
      Constraints.MinHeight = 40
      Constraints.MinWidth = 160
      TabOrder = 1
      object Label1: TLabel
        Left = 3
        Top = 7
        Width = 92
        Height = 13
        Caption = 'Debug Force Input:'
      end
      object InMemo: TMemo
        Left = 0
        Top = 50
        Width = 347
        Height = 293
        Anchors = [akLeft, akTop, akRight, akBottom]
        Color = clInfoBk
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
      object LogEdit: TEdit
        Left = 108
        Top = 2
        Width = 236
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 1
        OnKeyDown = LogEditKeyDown
      end
      object FeedbackCheckBox: TCheckBox
        Left = 1
        Top = 28
        Width = 120
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Feedback Loop Test'
        TabOrder = 2
      end
      object BitBtn1: TBitBtn
        Left = 262
        Top = 25
        Width = 83
        Height = 24
        Anchors = [akTop, akRight]
        Caption = 'Clear Logs'
        TabOrder = 3
        OnClick = ClearButtonClick
        Glyph.Data = {
          4E010000424D4E01000000000000760000002800000012000000120000000100
          040000000000D800000000000000000000001000000010000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF006FFFFFFFFFFF
          FFFFFF000000000FFFFFFFF00F0F00000000FFFFFFFFFFFFFFFFFF00000000F0
          0FFFFFFFF000F0000000FFFFFFFFF00FFFFFFF0000000F0F00FF07700FFF0F00
          0000FFFFFFF07777700FFF00000000F00F07777777700F000000FFFFF8887777
          77777000000000F0F8FF8877777777000000FFFFFF8FFF88777777000000FFFF
          FF8FFFFF88777700000000000008FFFFFF887700000066666668FFFFFFFF8800
          0000666666668FFFFFFFFF000000666666668FFFFFFFFF00000066666666688F
          FFFFFF0000006666666666688FFFFF000000}
      end
    end
    object OutPanel: TPanel
      Left = 347
      Top = 0
      Width = 242
      Height = 344
      Align = alClient
      BevelOuter = bvNone
      Constraints.MinHeight = 40
      Constraints.MinWidth = 40
      TabOrder = 0
      object OutMemo: TMemo
        Left = 0
        Top = 0
        Width = 242
        Height = 344
        Align = alClient
        Color = clInfoBk
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
  end
  object TopPanel: TPanel
    Left = 0
    Top = 0
    Width = 589
    Height = 58
    Align = alClient
    BevelOuter = bvNone
    Constraints.MinHeight = 40
    Constraints.MinWidth = 100
    TabOrder = 1
    object LogMemo: TMemo
      Left = 0
      Top = 0
      Width = 589
      Height = 58
      Align = alClient
      Color = clInfoBk
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      ReadOnly = True
      ScrollBars = ssBoth
      TabOrder = 0
      WordWrap = False
    end
  end
end
