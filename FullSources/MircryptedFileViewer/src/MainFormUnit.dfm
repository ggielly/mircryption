object MainForm: TMainForm
  Left = 289
  Top = 112
  Width = 464
  Height = 431
  Caption = 'Mircrypted File Viewer - v1.11.02'
  Color = clBtnFace
  Constraints.MinHeight = 312
  Constraints.MinWidth = 312
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001001010100001000400280100001600000028000000100000002000
    0000010004000000000000000000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    000000000000078888888888880007FFFFFFFFFFF80007FF0000000FF80007FF
    3FBBBB0FF80007FF3FB0BB0FF80007FF3FB0FB0FF80007FF3F000B0FF80007FF
    3FFFFF0FF80007FF3333333FF80007FFF3FFF3FFF80007FFF3FFF3FFF80007FF
    F33F33F0000007FFF83338F7F70007FFFFFFFFF7700007777777777700008001
    0000800100008001000080010000800100008001000080010000800100008001
    0000800100008001000080010000800100008003000080070000800F0000}
  OldCreateOrder = False
  OnClose = FormClose
  OnPaint = FormPaint
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 208
    Width = 456
    Height = 3
    Cursor = crVSplit
    Align = alTop
  end
  object Panel_File: TPanel
    Left = 0
    Top = 211
    Width = 456
    Height = 174
    Align = alClient
    BorderStyle = bsSingle
    Constraints.MinHeight = 100
    TabOrder = 0
    DesignSize = (
      452
      170)
    object Label_Filename: TLabel
      Left = 6
      Top = 9
      Width = 101
      Height = 13
      Caption = 'Encrypted File Name:'
    end
    object SpeedButton_Save: TSpeedButton
      Left = 400
      Top = 6
      Width = 21
      Height = 21
      Hint = 'Save decrypted text to a new file'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        04000000000080000000CE0E0000C40E00001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00222222222222
        2222220000000000000220330000008803022033000000880302203300000088
        0302203300000000030220333333333333022033000000003302203088888888
        0302203088888888030220308888888803022030888888880302203088888888
        0002203088888888080220000000000000022222222222222222}
      Margin = 0
      OnClick = SpeedButton_SaveClick
    end
    object SpeedButton_eSave: TSpeedButton
      Left = 425
      Top = 6
      Width = 21
      Height = 21
      Hint = 'Save file re-encrypted with a different passphrase'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFF000000000
        0FFFFFF080BBBBBB80FFFFF080BBBBBB80FFFFF080BBBBBB80FFFFF080BBBBBB
        80FFFFF0803BBBBB80FFFF088803BBBB80FFFF0808803BBB80FFFF0888803737
        30FFFFF00003737370FFFFFF0B0FFF0B0FFFFFFF0B0FFF0B0FFFFFFF0B0FFF0B
        0FFFFFFFF0B0000B0FFFFFFFFF0BBBB0FFFFFFFFFFF0000FFFFF}
      Margin = 0
      OnClick = SpeedButton_eSaveClick
    end
    object AdvFileNameEdit_File: TAdvFileNameEdit
      Left = 112
      Top = 6
      Width = 284
      Height = 21
      AutoFocus = False
      ErrorMarkerPos = 0
      ErrorMarkerLen = 0
      ErrorColor = clRed
      ErrorFontColor = clWhite
      Flat = False
      FlatLineColor = clBlack
      FlatParentColor = True
      FocusAlign = eaDefault
      FocusBorder = False
      FocusColor = clWindow
      FocusFontColor = clWindowText
      FocusLabel = False
      FocusWidthInc = 0
      ModifiedColor = clHighlight
      DisabledColor = clSilver
      ReturnIsTab = False
      LengthLimit = 0
      TabOnFullLength = False
      LabelPosition = lpLeftTop
      LabelMargin = 4
      LabelTransparent = False
      LabelAlwaysEnabled = False
      LabelFont.Charset = DEFAULT_CHARSET
      LabelFont.Color = clWindowText
      LabelFont.Height = -11
      LabelFont.Name = 'MS Sans Serif'
      LabelFont.Style = []
      Persistence.Enable = False
      Persistence.Section = 'RightFolder'
      Persistence.Location = plInifile
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Color = clWindow
      Enabled = True
      HintShowLargeText = False
      OleDropTarget = False
      OleDropSource = False
      TabOrder = 0
      Transparent = False
      Visible = True
      OnChange = AdvFileNameEdit_FileChange
      OnKeyDown = AdvFileNameEdit_FileKeyDown
      ButtonWidth = 18
      Etched = False
      Glyph.Data = {
        CE000000424DCE0000000000000076000000280000000C0000000B0000000100
        0400000000005800000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00D00000000DDD
        00000077777770DD00000F077777770D00000FF07777777000000FFF00000000
        00000FFFFFFF0DDD00000FFF00000DDD0000D000DDDDD0000000DDDDDDDDDD00
        0000DDDDD0DDD0D00000DDDDDD000DDD0000}
      FilterIndex = 0
      DialogOptions = []
      DialogTitle = 'Select encrypted file..'
      DialogKind = fdOpen
    end
    object FileMemo: TPlusMemo
      Left = 1
      Top = 31
      Width = 450
      Height = 138
      Cursor = crIBeam
      Alignment = taLeftJustify
      Lines.Strings = (
        
          'Decrypted file contents will be shown here once the proper keyph' +
          'rase is provided.'
        ''
        
          'You can drag files into this window or use the browse-for-file b' +
          'utton above.')
      ScrollBars = ssVertical
      AltFont.Charset = DEFAULT_CHARSET
      AltFont.Color = clWindowText
      AltFont.Height = -11
      AltFont.Name = 'MS Sans Serif'
      AltFont.Style = []
      CaretWidth = 1
      ColumnWrap = 0
      DisplayOnly = False
      EndOfTextMark.Color = clRed
      EndOfTextMark.Style = psClear
      Highlighter = URLHighlighter1
      Justified = False
      LineHeight = 0
      Options = [pmoWrapCaret, pmoInsertKeyActive, pmoWideOverwriteCaret, pmoAutoScrollBars, pmoNoDragnDrop]
      Overwrite = False
      ReadOnly = True
      RightLinePen.Color = clGray
      RightLinePen.Style = psDot
      ScrollTime = 0
      SelBackColor = clHighlight
      SelTextColor = clHighlightText
      Separators = #9#10#13' $%&,./:;<=>'
      SpecUnderline.Color = clRed
      Version = 'v6.2e Unregistered ed.'
      Align = alBottom
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      TabStop = True
      OnKeyDown = FileMemoKeyDown
    end
  end
  object Panel_Top: TPanel
    Left = 0
    Top = 0
    Width = 456
    Height = 37
    Align = alTop
    BorderStyle = bsSingle
    TabOrder = 1
    DesignSize = (
      452
      33)
    object Label_Keyphrase: TLabel
      Left = 6
      Top = 10
      Width = 106
      Height = 13
      Caption = 'Encryption Keyphrase:'
    end
    object Edit_EncryptionKey: TEdit
      Left = 118
      Top = 6
      Width = 328
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnKeyPress = Edit_EncryptionKeyKeyPress
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 385
    Width = 456
    Height = 19
    Panels = <>
    SimplePanel = True
    SimpleText = 
      'Type encryption phrase into the editbox / Drag an log files into' +
      ' the window'
  end
  object Panel_Browser: TPanel
    Left = 0
    Top = 37
    Width = 456
    Height = 171
    Align = alTop
    BorderStyle = bsSingle
    Constraints.MinHeight = 100
    TabOrder = 3
    DesignSize = (
      452
      167)
    object Label_HighlightingString: TLabel
      Left = 8
      Top = 57
      Width = 93
      Height = 13
      Caption = 'Highlighting Strings:'
    end
    object Label_LogDirectory: TLabel
      Left = 8
      Top = 10
      Width = 85
      Height = 13
      Caption = 'Log File Directory:'
    end
    object Label1: TLabel
      Left = 9
      Top = 33
      Width = 95
      Height = 13
      Caption = 'Log Filename Mask:'
    end
    object AdvListView_Browser: TAdvListView
      Left = 1
      Top = 105
      Width = 450
      Height = 61
      Align = alBottom
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <
        item
          AutoSize = True
          Caption = 'Filename'
          MinWidth = 60
        end
        item
          Caption = 'File Date'
          MaxWidth = 80
          MinWidth = 80
          Width = 80
        end
        item
          Alignment = taRightJustify
          Caption = 'Size'
          MaxWidth = 80
          MinWidth = 80
          Width = 80
        end
        item
          Alignment = taRightJustify
          Caption = 'Encryption'
          MaxWidth = 85
          MinWidth = 85
          Width = 85
        end
        item
          Alignment = taRightJustify
          Caption = 'Hits'
          MaxWidth = 52
          MinWidth = 52
          Width = 52
        end>
      HoverTime = -1
      ReadOnly = True
      RowSelect = True
      SortType = stBoth
      TabOrder = 5
      ViewStyle = vsReport
      OnDblClick = AdvListView_BrowserDblClick
      AutoHint = False
      ClipboardEnable = False
      ColumnSize.Save = False
      ColumnSize.Stretch = False
      ColumnSize.Storage = stInifile
      FilterTimeOut = 500
      PrintSettings.FooterSize = 0
      PrintSettings.HeaderSize = 0
      PrintSettings.Time = ppNone
      PrintSettings.Date = ppNone
      PrintSettings.DateFormat = 'dd/mm/yyyy'
      PrintSettings.PageNr = ppNone
      PrintSettings.Title = ppNone
      PrintSettings.Font.Charset = DEFAULT_CHARSET
      PrintSettings.Font.Color = clWindowText
      PrintSettings.Font.Height = -11
      PrintSettings.Font.Name = 'MS Sans Serif'
      PrintSettings.Font.Style = []
      PrintSettings.HeaderFont.Charset = DEFAULT_CHARSET
      PrintSettings.HeaderFont.Color = clWindowText
      PrintSettings.HeaderFont.Height = -11
      PrintSettings.HeaderFont.Name = 'MS Sans Serif'
      PrintSettings.HeaderFont.Style = []
      PrintSettings.FooterFont.Charset = DEFAULT_CHARSET
      PrintSettings.FooterFont.Color = clWindowText
      PrintSettings.FooterFont.Height = -11
      PrintSettings.FooterFont.Name = 'MS Sans Serif'
      PrintSettings.FooterFont.Style = []
      PrintSettings.Borders = pbNoborder
      PrintSettings.BorderStyle = psSolid
      PrintSettings.Centered = False
      PrintSettings.RepeatHeaders = False
      PrintSettings.LeftSize = 0
      PrintSettings.RightSize = 0
      PrintSettings.ColumnSpacing = 0
      PrintSettings.RowSpacing = 0
      PrintSettings.Orientation = poPortrait
      PrintSettings.FixedWidth = 0
      PrintSettings.FixedHeight = 0
      PrintSettings.UseFixedHeight = False
      PrintSettings.UseFixedWidth = False
      PrintSettings.FitToPage = fpNever
      PrintSettings.PageNumSep = '/'
      HTMLHint = False
      HTMLSettings.Width = 100
      HeaderHotTrack = False
      HeaderDragDrop = False
      HeaderFlatStyle = False
      HeaderOwnerDraw = False
      HeaderHeight = 13
      HeaderFont.Charset = DEFAULT_CHARSET
      HeaderFont.Color = clWindowText
      HeaderFont.Height = -11
      HeaderFont.Name = 'MS Sans Serif'
      HeaderFont.Style = []
      ProgressSettings.ColorFrom = clSilver
      ProgressSettings.FontColorFrom = clBlack
      ProgressSettings.ColorTo = clWhite
      ProgressSettings.FontColorTo = clGray
      SelectionRTFKeep = False
      ScrollHint = False
      SelectionColor = clHighlight
      SelectionTextColor = clHighlightText
      SizeWithForm = False
      SortDirection = sdAscending
      SortShow = True
      SortIndicator = siRight
      SortUpGlyph.Data = {
        56010000424D560100000000000036000000280000000A000000090000000100
        18000000000020010000130B0000130B00000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000CED7D6000000
        CED7D6CED7D6CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D60000CED7D6000000
        CED7D6CED7D6CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D60000CED7D6CED7D6
        000000CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D6CED7D60000CED7D6CED7D6
        000000CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D6CED7D60000CED7D6CED7D6
        CED7D6000000CED7D6CED7D6FFFFFFCED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6000000CED7D6CED7D6FFFFFFCED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6CED7D6000000FFFFFFCED7D6CED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6CED7D6000000CED7D6CED7D6CED7D6CED7D6CED7D60000}
      SortDownGlyph.Data = {
        56010000424D560100000000000036000000280000000A000000090000000100
        18000000000020010000130B0000130B00000000000000000000CED7D6CED7D6
        CED7D6CED7D6000000CED7D6CED7D6CED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6CED7D6000000FFFFFFCED7D6CED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6000000CED7D6CED7D6FFFFFFCED7D6CED7D6CED7D60000CED7D6CED7D6
        CED7D6000000CED7D6CED7D6FFFFFFCED7D6CED7D6CED7D60000CED7D6CED7D6
        000000CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D6CED7D60000CED7D6CED7D6
        000000CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D6CED7D60000CED7D6000000
        CED7D6CED7D6CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D60000CED7D6000000
        CED7D6CED7D6CED7D6CED7D6CED7D6CED7D6FFFFFFCED7D60000000000000000
        0000000000000000000000000000000000000000000000000000}
      StretchColumn = False
      SubImages = False
      SubItemEdit = False
      SubItemSelect = False
      VAlignment = vtaCenter
      ItemHeight = 13
      SaveHeader = False
      LoadHeader = False
      ReArrangeItems = False
      DetailView.Visible = False
      DetailView.Column = 0
      DetailView.Font.Charset = DEFAULT_CHARSET
      DetailView.Font.Color = clBlue
      DetailView.Font.Height = -11
      DetailView.Font.Name = 'MS Sans Serif'
      DetailView.Font.Style = []
      DetailView.Height = 16
      DetailView.Indent = 0
      DetailView.SplitLine = False
      OnGetFormat = AdvListView_BrowserGetFormat
    end
    object Edit_HighlightString: TEdit
      Left = 107
      Top = 53
      Width = 216
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
      OnChange = Edit_HighlightStringChange
      OnKeyPress = Edit_HighlightStringKeyPress
    end
    object AdvDirectoryEdit: TAdvDirectoryEdit
      Left = 107
      Top = 5
      Width = 338
      Height = 21
      AutoFocus = False
      ErrorMarkerPos = 0
      ErrorMarkerLen = 0
      ErrorColor = clRed
      ErrorFontColor = clWhite
      Flat = False
      FlatLineColor = clBlack
      FlatParentColor = True
      FocusAlign = eaDefault
      FocusBorder = False
      FocusColor = clWindow
      FocusFontColor = clWindowText
      FocusLabel = False
      FocusWidthInc = 0
      ModifiedColor = clHighlight
      DisabledColor = clSilver
      ReturnIsTab = False
      LengthLimit = 0
      TabOnFullLength = False
      LabelPosition = lpLeftTop
      LabelMargin = 4
      LabelTransparent = False
      LabelAlwaysEnabled = False
      LabelFont.Charset = DEFAULT_CHARSET
      LabelFont.Color = clWindowText
      LabelFont.Height = -11
      LabelFont.Name = 'MS Sans Serif'
      LabelFont.Style = []
      Persistence.Enable = False
      Persistence.Section = 'LogDirectory'
      Persistence.Location = plInifile
      Anchors = [akLeft, akTop, akRight]
      Color = clWindow
      Enabled = True
      HintShowLargeText = False
      OleDropTarget = False
      OleDropSource = False
      TabOrder = 0
      Transparent = False
      Visible = True
      OnChange = AdvDirectoryEditChange
      OnKeyPress = AdvDirectoryEditKeyPress
      ButtonWidth = 18
      Etched = False
      Glyph.Data = {
        CE000000424DCE0000000000000076000000280000000C0000000B0000000100
        0400000000005800000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00F00000000FFF
        00000088888880FF00000B088888880F00000BB08888888000000BBB00000000
        00000BBBBBBB0B0F00000BBB00000B0F0000F000BBBBBB0F0000FF0BBBBBBB0F
        0000FF0BBB00000F0000FFF000FFFFFF0000}
      BrowseDialogText = 'Select Directory'
    end
    object Edit_FileMask: TEdit
      Left = 107
      Top = 29
      Width = 216
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
      Text = '*.log'
      OnKeyPress = Edit_FileMaskKeyPress
    end
    object Button_Search: TBitBtn
      Left = 328
      Top = 53
      Width = 119
      Height = 22
      Anchors = [akTop, akRight]
      Caption = '&Search Files'
      TabOrder = 4
      OnClick = Button_SearchClick
      Glyph.Data = {
        4E010000424D4E01000000000000760000002800000012000000120000000100
        040000000000D800000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00666666666666
        6666660000006660000000000006660000006668777777777706660000006668
        FFFFFFFFF70666000000668008FFFFF80006660000006008800FFF0088006600
        000080777800F007778086000000077F77800077F7780600000007FFF780007F
        FF780600000080FF7700000FF770860000006007700000007700660000006680
        0000C0000006660000006668FF0000077F06660000006668FFFFFFFFFF066600
        00006668FFFFFFF00006660000006668FFFFFFF7F866660000006668FFFFFFF7
        866666000000666888888888666666000000}
    end
    object CheckBox_RecurseDirs: TCheckBox
      Left = 329
      Top = 31
      Width = 114
      Height = 17
      Alignment = taLeftJustify
      Anchors = [akTop, akRight]
      Caption = 'Recurse Directories'
      Checked = True
      State = cbChecked
      TabOrder = 2
      OnClick = CheckBox_RecurseDirsClick
    end
  end
  object SaveDialog: TSaveDialog
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Title = 'Save decrypted file as..'
    Left = 190
    Top = 319
  end
  object FileDropper1: TFileDropper
    DropSite = AdvFileNameEdit_File
    OnDrop = FileDropperDrop
    AllowDir = False
    RecurseAllSubFolders = False
    RecurseOneSubFolder = False
    OnlyDirs = False
    OnlyLeafs = False
    MultiDropIndex = 0
    MultiDropTotal = 0
    Left = 19
    Top = 335
  end
  object FileDropper2: TFileDropper
    DropSite = AdvListView_Browser
    OnDrop = FileDropperDrop
    AllowDir = False
    RecurseAllSubFolders = False
    RecurseOneSubFolder = False
    OnlyDirs = False
    OnlyLeafs = False
    MultiDropIndex = 0
    MultiDropTotal = 0
    Left = 51
    Top = 335
  end
  object FixTimer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = FixTimerTimer
    Left = 74
    Top = 151
  end
  object FileDropper3: TFileDropper
    DropSite = FileMemo
    OnDrop = FileDropperDrop
    AllowDir = False
    RecurseAllSubFolders = False
    RecurseOneSubFolder = False
    OnlyDirs = False
    OnlyLeafs = False
    MultiDropIndex = 0
    MultiDropTotal = 0
    Left = 154
    Top = 336
  end
  object FileDropper4: TFileDropper
    DropSite = Panel_Browser
    OnDrop = FileDropperDrop
    AllowDir = False
    RecurseAllSubFolders = False
    RecurseOneSubFolder = False
    OnlyDirs = False
    OnlyLeafs = False
    MultiDropIndex = 0
    MultiDropTotal = 0
    Left = 114
    Top = 336
  end
  object URLHighlighter1: TURLHighlighter
    BaseContext = 1024
    WebURL.AltFont = False
    WebURL.Style = [fsUnderline]
    WebURL.Background = -1
    WebURL.Foreground = clBlue
    WebURL.Cursor = crHandPoint
    EmailURL.AltFont = False
    EmailURL.Style = [fsUnderline]
    EmailURL.Background = -1
    EmailURL.Foreground = clGreen
    EmailURL.Cursor = crHandPoint
    NNTP_URL.AltFont = False
    NNTP_URL.Style = [fsUnderline]
    NNTP_URL.Background = -1
    NNTP_URL.Foreground = clRed
    NNTP_URL.Cursor = crHandPoint
    FileURL.AltFont = False
    FileURL.Style = [fsUnderline]
    FileURL.Background = -1
    FileURL.Foreground = clFuchsia
    FileURL.Cursor = crHandPoint
    OtherURL.AltFont = False
    OtherURL.Style = [fsItalic]
    OtherURL.Background = clInfoBk
    OtherURL.Foreground = clSilver
    OtherURL.Cursor = crHandPoint
    LaunchURLs = True
    DblClickLaunch = True
    Scope = 0
    Priority = False
    Left = 280
    Top = 280
  end
end
