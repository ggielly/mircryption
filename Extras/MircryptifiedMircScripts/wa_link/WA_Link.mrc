;
;                                                              
;               WA_Link v1.2 by the bASE crew                  
;                                                              
;         Offical site: http://www.base.is/thecrew             
;                                                              
;          mIRC Script by Bergþór Olivert Thorstensen          
;                                                              
;

on *:LOAD: {
  echo -a 3WA_Link v1.2 has now been loaded.
  echo -a 4Recommended to use with the latest mIRC and WinAmp (not Beta 3) versions.
  echo -a 4Your WinAmp Status and controls can be used from the popup menu's or command line.
  echo -a 4For a complete command list for WA Link type: /wa help
  echo -a 
  echo -a 4The bASE crew hangs in #base.is on gamers-net.com
  echo -a 4USA Server: us1.gamers-net.com channel: #base.is
  echo -a 4Europe Server: dk1.gamers-net.com channel: #base.is

  if (%WA.Theme == $null) set %WA.Theme $shortfn($mircdirwa_link\themes\Default.ini)
  if (%WA.Theme.ShortName == $null) set %WA.Theme.ShortName Default
  if (%WA.Theme.Random == $null) set %WA.Theme.Random $false
  if (%WA.Stats.Command == $null) set %WA.Stats.Command /Say

  echo -a 4Bringing readme file up for reading....
  wa README
}

on *:START: {
  if (%WA.Theme == $null) set %WA.Theme $shortfn($mircdirwa_link\themes\Default.ini)
  if (%WA.Theme.ShortName == $null) set %WA.Theme.ShortName Default
  if (%WA.Theme.Random == $null) set %WA.Theme.Random $false
  if (%WA.Stats.Command == $null) set %WA.Stats.Command /Say

  if $lock(dll) == $true {
    echo -a 4You have DLL locked, you can not use WA Link unless you unlock it.
    echo -a 4Options - General - Lock uncheck DLL locking
  }

  if $lock(run) == $true {
    echo -a 4You have RUN locked, you can not use certain features unless you unlock it.
    echo -a 4Options - General - Lock uncheck RUN locking
  }
}

;Dialogs -----------------------------------------------
;WA Themes -------------------------------------------------
dialog WA_Themes {
  title "WA Link 1.2 Theme Dialog"
  size -1 -1 149 72
  option dbu
  box "Select / Edit Theme", 5, 2 1 146 56
  icon 7, 6 22 138 10
  combo 9, 25 9 76 88, sort size drop
  check "Random Themes", 2, 94 44 51 10
  text "Theme:", 3, 4 9 20 8
  text "Action:", 4, 6 45 18 8
  button "Set Theme", 8, 72 59 37 12, ok
  button "Cancel", 11, 111 59 37 12, cancel
  button "Edit", 12, 103 7 20 12
  edit "", 13, 24 44 50 10
  text "Themes found: 0", 1, 2 62 62 8
  scroll "", 10, 6 34 139 8, range 100 horizontal bottom
  button "New", 6, 125 7 20 12
}

;Dialog is shown
on *:DIALOG:WA_Themes:init:0: {
  did -o WA_Themes 1 1 Themes found: $findfile($mircdirWA_Link\Themes\,*.INI,0,didtok WA_Themes 9 46 $remove($1-,$mircdirWA_Link\Themes\,.INI))

  if %WA.Theme.ShortName == $NULL {
    did -c WA_Themes 9 1
  }
  else {
    did -c WA_Themes 9 $didwm(WA_Themes, 9, %WA.Theme.ShortName)
  }

  set %WA.Preview.FileName %WA.Theme

  if %WA.Stats.Command != $null {
    did -a WA_Themes 13 %WA.Stats.Command
  }
  else {
    did -a WA_Themes 13 /say
  }

  if %WA.Theme.Random == $true {
    did -c WA_Themes 2
  }
  else {
    did -u WA_Themes 2
  }

  WA_PreviewTheme
}

;Set theme button clicked
on *:DIALOG:WA_Themes:sclick:8: {
  set %WA.Theme $shortfn($mircdirWA_Link\Themes\) $+ $did(WA_Themes,9) $+ .INI
  set %WA.Theme.ShortName $did(WA_Themes,9)

  if $exists(%WA.Theme) == $false {
    echo 4 -a %WA.Theme does not exist.
  }

  if $did(WA_Themes,2).state == 1 {
    set %WA.Theme.Random $true
  }
  else {
    set %WA.Theme.Random $false
  }

  if $did(WA_Themes,13) != $null {
    set %WA.Stats.Command $did(WA_Themes,13)
  }
  else {
    set %WA.Stats.Command /Say
  }
}

;Edit button clicked
on *:DIALOG:WA_THEMES:sclick:12:{
  WA_Theme_Edit $shortfn($mircdirWA_Link\Themes\) $+ $did(WA_Themes,9) $+ .INI $did(WA_Themes,9)
}

;Combobox value changed
ON *:DIALOG:WA_THEMES:SCLICK:9:{
  set %WA.Preview.FileName $shortfn($mircdirWA_Link\Themes\) $+ $did(WA_Themes,9) $+ .INI
  WA_PreviewTheme
}

;Scrollbar was changed
ON *:DIALOG:WA_Themes:SCROLL:10:{
  var %WA.Temp.Preview.Location
  var %WA.Temp.Preview.Text
  var %WA.Temp.Preview.Text.Width

  set %WA.Temp.Preview.Text $remove(%WA.Preview,*)
  set %WA.Temp.Preview.Text.Width $width(%WA.Temp.Preview.Text,FixedSys,12,0,1)

  set %WA.Temp.Preview.Location $calc($calc($calc(%WA.Temp.Preview.Text.Width * $did(WA_Themes,10).sel) / 100) * -1)

  window -c @WA.Link.Theme.Preview.Window
  window -ph @WA.Link.Theme.Preview.Window 10 10 285 45

  drawtext -ph @WA.Link.Theme.Preview.Window 1 FixedSys 12 %WA.Temp.Preview.Location 1 $remove(%WA.Preview,*)

  drawsave @WA.Link.Theme.Preview.Window WA_Link\WA_Link_Preview.bmp

  did -gv WA_Themes 7 WA_Link\WA_Link_Preview.bmp
  .remove WA_Link\WA_Link_Preview.bmp
}

;New theme button clicked
ON *:DIALOG:WA_Themes:SCLICK:6:{
  set %WA.Theme.ShortName $$?="Enter name of new theme (Not .ini)"
  set %WA.Theme $shortfn($mircdirwa_link\themes\) $+ %WA.Theme.ShortName $+ .INI

  did -a WA_Themes 9 %WA.Theme.ShortName
  did -c WA_Themes 9 $didwm(WA_Themes, 9, %WA.Theme.ShortName)

  write -l1 %WA.Theme Playing
  write -l2 %WA.Theme Stopped
  write -l3 %WA.Theme Paused
  write -l4 %WA.Theme Not running
  write -l5 %WA.Theme Title
  write -l6 %WA.Theme Artist
  write -l7 %WA.Theme Album
  write -l8 %WA.Theme Year
  write -l9 %WA.Theme Genre
  write -l10 %WA.Theme Comment
  write -l11 %WA.Theme Lengthline
  write -l12 %WA.Theme Bardone
  write -l13 %WA.Theme Barleft
  write -l14 %WA.Theme S
  write -l15 %WA.Theme Error
  write -l16 %WA.Theme Error

  write -l17 %WA.Theme 
  write -l18 %WA.Theme WA Link v1.2 - Theme file
  write -l19 %WA.Theme WA Link just reads the first 16 lines
  write -l20 %WA.Theme %WA.Theme.ShortName Theme by $me
  write -l21 %WA.Theme 

  set %WA.Preview.FileName %WA.Theme

  WA_PreviewTheme

  WA_Theme_Edit %WA.Theme %WA.Theme.ShortName
}

;Edit theme ----------------------------------------------------------
dialog EditTheme {
  title "WA Link 1.2 Theme Edit"
  size -1 -1 189 59
  option dbu
  button "OK", 1, 112 46 37 12, ok
  button "Cancel", 2, 151 46 37 12, cancel
  text "Text Label", 3, 4 8 181 8
  edit "", 4, 4 17 181 10, autohs
  box "Edit Line", 5, 1 0 187 44
  button "Add Var", 6, 112 29 37 12
  combo 7, 43 29 67 52, size drop
  text "Available Vars:", 8, 5 31 37 8
}

on *:DIALOG:EditTheme:INIT:0:{
  did -r EditTheme 3

  did -b EditTheme 7
  did -b EditTheme 8
  did -b EditTheme 6

  if $sline(%WA.Temp.Preview.Window,1).ln == 1 {
    did -a EditTheme 3 Playing
    did -a EditTheme 7 Version
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 2 {
    did -a EditTheme 3 Stopped
    did -a EditTheme 7 Version
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 3 {
    did -a EditTheme 3 Paused
    did -a EditTheme 7 Version
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 4 {
    did -a EditTheme 3 Not running
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 5 {
    did -a EditTheme 3 Title
    did -a EditTheme 7 Title
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 6 {
    did -a EditTheme 3 Artist
    did -a EditTheme 7 Artist
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 7 {
    did -a EditTheme 3 Album
    did -a EditTheme 7 Album
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 8 {
    did -a EditTheme 3 Year
    did -a EditTheme 7 Year
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 9 {
    did -a EditTheme 3 Genre
    did -a EditTheme 7 Genre
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 10 {
    did -a EditTheme 3 Comment
    did -a EditTheme 7 Comment
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 11 {
    did -a EditTheme 3 Length, done and bar
    did -a EditTheme 7 Percent played
    did -a EditTheme 7 Minutes played
    did -a EditTheme 7 Seconds played
    did -a EditTheme 7 Length minutes
    did -a EditTheme 7 Length seconds
    did -a EditTheme 7 Bar
    did -a EditTheme 7 Filesize
    did -a EditTheme 7 Bitrate
    did -a EditTheme 7 Samplerate
    did -a EditTheme 7 Channels (Mono, stereo)
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 12 {
    did -a EditTheme 3 Done bar
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 13 {
    did -a EditTheme 3 Remaining bar
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 14 {
    did -a EditTheme 3 States shown
    did -a EditTheme 7 Show state
    did -a EditTheme 7 Show title
    did -a EditTheme 7 Show artist
    did -a EditTheme 7 Show album
    did -a EditTheme 7 Show year
    did -a EditTheme 7 Show genre
    did -a EditTheme 7 Show comment
    did -a EditTheme 7 Show lengthline
    did -e EditTheme 7
    did -e EditTheme 8
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 15 {
    did -a EditTheme 3 Default playlist not found
  }
  if $sline(%WA.Temp.Preview.Window,1).ln == 16 {
    did -a EditTheme 3 Winamp doesn't return playstate
  }
  did -a EditTheme 4 $read( %WA.Temp.Preview.File ,n ,$sline(%WA.Temp.Preview.Window,1).ln)

  did -f EditTheme 4
}

;OK button clicked
on *:DIALOG:EditTheme:SCLICK:1:{
  var %WA.Temp.Line = %WA.Temp.Line $did(EditTheme,4)

  set %WA.Temp.Line $replace(%WA.Temp.Line,$chr(32),$chr(160))

  if $right($did(EditTheme,4),1) == $chr(32) {
    set %WA.Temp.Line %WA.Temp.Line $+ $chr(160)
  }

  write -l $+ $sline(%WA.Temp.Preview.Window,1).ln %WA.Temp.Preview.File %WA.Temp.Line

  ;Refresh info
  dline %WA.Temp.Preview.Window 1-18

  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,1) 	- Playing
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,2) 	- Stopped
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,3) 	- Paused
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,4) 	- Not running
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,5) 	- Title
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,6) 	- Artist
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,7) 	- Album
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,8) 	- Year
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,9) 	- Genre
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,10) 	- Comment
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,11) 	- Lengthline
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,12) 	- Done bar
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,13) 	- Remaining bar
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,14) 	- States shown
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,15) 	- Default playlist not found
  aline %WA.Temp.Preview.Window $read( %WA.Temp.Preview.File ,n,16) 	- Winamp doesn't return playstate
  aline %WA.Temp.Preview.Window 4---------------------------------------------------------------------------------------------------------
  aline %WA.Temp.Preview.Window 4Double click a line to edit the line

  if $dialog(WA_Themes) != $null {
    WA_PreviewTheme
  }
}

;Combocox was changed
ON *:DIALOG:EditTheme:SCLICK:7:{
  did -e EditTheme 6
}

;Add var button clicked
ON *:DIALOG:EditTheme:SCLICK:6:{
  if $did(EditTheme,7) == Version {
    did -a EditTheme 4 % $+ VERSION
  }
  elseif $did(EditTheme,7) == Title {
    did -a EditTheme 4 % $+ TITLE
  }
  elseif $did(EditTheme,7) == Artist {
    did -a EditTheme 4 % $+ ARTIST
  }
  elseif $did(EditTheme,7) == Album {
    did -a EditTheme 4 % $+ ALBUM
  }
  elseif $did(EditTheme,7) == Year {
    did -a EditTheme 4 % $+ YEAR
  }
  elseif $did(EditTheme,7) == Genre {
    did -a EditTheme 4 % $+ GENRE
  }
  elseif $did(EditTheme,7) == Comment {
    did -a EditTheme 4 % $+ COMMENT
  }
  elseif $did(EditTheme,7) == Percent played {
    did -a EditTheme 4 % $+ PERCENT
  }
  elseif $did(EditTheme,7) == Minutes played {
    did -a EditTheme 4 % $+ MINDONE
  }
  elseif $did(EditTheme,7) == Seconds played {
    did -a EditTheme 4 % $+ SECDONE
  }
  elseif $did(EditTheme,7) == Length minutes {
    did -a EditTheme 4 % $+ MINLEN
  }
  elseif $did(EditTheme,7) == Length seconds {
    did -a EditTheme 4 % $+ SECLEN
  }
  elseif $did(EditTheme,7) == Bar {
    did -a EditTheme 4 % $+ BAR
  }
  elseif $did(EditTheme,7) == Filesize {
    did -a EditTheme 4 % $+ SIZE
  }
  elseif $did(EditTheme,7) == Bitrate {
    did -a EditTheme 4 % $+ BITRATE
  }
  elseif $did(EditTheme,7) == Samplerate {
    did -a EditTheme 4 % $+ SAMPLERATE
  }
  elseif $did(EditTheme,7) == Channels (Mono, stereo) {
    did -a EditTheme 4 % $+ CHANNELS
  }
  elseif $did(EditTheme,7) == Show state {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 S
    }
    else {
      did -a EditTheme 4 +S
    }
  }
  elseif $did(EditTheme,7) == Show title {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 T
    }
    else {
      did -a EditTheme 4 +T
    }
  }
  elseif $did(EditTheme,7) == Show artist {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 A
    }
    else {
      did -a EditTheme 4 +A
    }
  }
  elseif $did(EditTheme,7) == Show album {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 B
    }
    else {
      did -a EditTheme 4 +B
    }
  }
  elseif $did(EditTheme,7) == Show year {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 Y
    }
    else {
      did -a EditTheme 4 +Y
    }
  }
  elseif $did(EditTheme,7) == Show genre {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 G
    }
    else {
      did -a EditTheme 4 +G
    }
  }
  elseif $did(EditTheme,7) == Show comment {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 C
    }
    else {
      did -a EditTheme 4 +C
    }
  }
  elseif $did(EditTheme,7) == Show lengthline {
    if $did(EditTheme,4) == $null {
      did -a EditTheme 4 L
    }
    else {
      did -a EditTheme 4 +L
    }
  }
}

;Pop-Ups -----------------------------------------------------
menu channel,query,nicklist,menubar {
  -
  WA Link
  .WA Link Stats: wa STATS
  .-
  .WA Tracks
  ..Find Track: wa find $$?="Enter keyword to search for in playlist"
  ..Jump to Track: wa jump $$?="Enter number of track to jump to in playlist"
  ..View Playlist: wa ViewPl
  ..-
  ..Next: wa NEXT
  ..Previous: wa PREV
  ..Pause: wa PAUSE
  ..Play: wa PLAY
  ..Stop: wa STOP
  ..Fade 'n' Stop: wa FADESTOP
  ..-
  ..First Track: wa BOFPL
  ..Last Track: wa EOFPL
  ..-
  ..Add Tracks
  ...Add Track: {
    wa pladdf
    if $window(@WA.Playlist).state != $null {
      .timer 1 1 ViewPl
    }
  }
  ...Add Folder: {
    wa pladdd
    if $window(@WA.Playlist).state != $null {
      .timer 1 1 ViewPl
    }
  }
  ...Add URL: {
    wa pladd
    if $window(@WA.Playlist).state != $null {
      .timer 1 1 ViewPl
    }
  }
  ..Open File(s): wa OPEN
  ..-
  ..Seek
  ...Seek (custom): wa SEEK $$?="Enter seek in percentage (0-100)"
  ...-
  ...FFW 5 Sec: wa FFW5
  ...REW 5 Sec: wa REW5
  ...-
  ...Seek 10%: wa SEEK 10
  ...Seek 20%: wa SEEK 20
  ...Seek 30%: wa SEEK 30
  ...Seek 40%: wa SEEK 40
  ...Seek 50%: wa SEEK 50
  ...Seek 60%: wa SEEK 60
  ...Seek 70%: wa SEEK 70
  ...Seek 80%: wa SEEK 80
  ...Seek 90%: wa SEEK 90
  .-
  .WA Volume
  ..Volume (custom): wa VOL $$?="Enter volume in percentage (0-100)" 
  ..-
  ..Volume Up: wa VOL UP
  ..Volume Down: wa VOL DOWN
  ..-
  ..Volume 0%: wa VOL 0
  ..Volume 25%: wa VOL 25
  ..Volume 50%: wa VOL 50
  ..Volume 75%: wa VOL 75
  ..Volume 100%: wa VOL 100
  ..-
  ..WA Panning
  ...Pan (custom): wa PAN $$?="Enter panning value (0=Center; -100=Left; 100=Right)"
  ...-
  ...Pan Left 100%: wa PAN -100
  ...Pan Left 50%: wa PAN -50
  ...Pan Middle: wa PAN 0
  ...Pan Right 50%: wa PAN 50
  ...Pan Right 100%: wa PAN 100
  .-
  .WA Controls
  ..Toggle Shuffle: wa SHUFFLE
  ..Toggle Repeat: wa REPEAT
  ..-
  ..WA Window
  ...Minimize: wa MINIMIZE
  ...Restore: wa RESTORE
  ...-
  ...Hide: wa HIDE
  ...Show: wa SHOW
  ...Always on top: wa ONTOP
  ...-
  ...Toggle playlist: wa playlist
  ...Toggle EQ window: wa eq
  ...Winamp Shade: wa SHADE
  ...Playlist Shade: wa PLSHADE
  ..-
  ..Exec VIS: wa VISPLUG
  ..Close Winamp: wa CLOSE
  ..Start Winamp: wa START
  .-
  .WA Link Themes: .dialog -m WA_Themes WA_Themes
  .-
  .WA Link ReadMe: wa README
  .WA Link About: wa ABOUT
  .-
  .WA Link Contacts
  ..E-mail the bASE Crew: run mailto:base@base.is
  ..Visit the bASE Crew website: run http://www.base.is/thecrew/
  ..Chat with the bASE Crew
  ...Denmark (DK1.Gamers-Net.com): run irc://dk1.gamers-net.com/base.is
  ...USA (US1.Gamers-Net.com): run irc://us1.gamers-net.com/base.is
  ...UK (UK1.Gamers-Net.com): run irc://uk1.gamers-net.com/base.is
  ...Netherlands (NL1.Gamers-Net.com): run irc://nl1.gamers-net.com/base.is

}

;Aliases --------------------------------------------------------------------------------
alias WA {
  var %WA.vol
  var %WA.pan
  var %WA.SearchString
  var %WA.seek
  var %WA.result
  var %WA.Stats
  var %WA.Files.Add
  var %WA.Files.Add.Dir
  var %WA.Files.Add.Counter

  if $1 != $null {
    if $$1 isin EQ WRITEPL PLADDD PLADDF PLADD CMD VIEWPL SAVEPL RAW PLAYLIST SHADE PLSHADE OPEN MINIMIZE RESTORE SHOW HIDE ONTOP JUMP FIND EOFPL BOFPL THEME README STATS NEXT PREV PAUSE PLAY STOP FADESTOP FFW5 REW5 SEEK VOL PAN SHUFFLE REPEAT VISPLUG CLOSE START ABOUT {
      if $$1 == EQ {
        set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,EQ)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == WRITEPL {
        set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,WRITEPL)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start      
      }

      elseif $$1 == PLADDD {
        if $lock(run) == $false {
          .set %WA.Files.Add

          .set %WA.Files.Add.Dir $sdir(c:\,Select dir)

          .set %WA.Files.Add.Counter 1
          window -lh @WA.FilesAdd

          echo -s 4WA Link (*.mp3) Files found: $findfile(%WA.Files.Add.Dir,*.mp3,@WA.FilesAdd,0,@WA.FilesAdd)
          echo -s 4WA Link (*.wma) Files found: $findfile(%WA.Files.Add.Dir,*.wma,@WA.FilesAdd,0,@WA.FilesAdd)
          echo -s 4WA Link (*.wav) Files found: $findfile(%WA.Files.Add.Dir,*.wav,@WA.FilesAdd,0,@WA.FilesAdd)
          echo -s 4WA Link (*.cda) Files found: $findfile(%WA.Files.Add.Dir,*.cda,@WA.FilesAdd,0,@WA.FilesAdd)
          echo -s 4WA Link (*.mid) Files found: $findfile(%WA.Files.Add.Dir,*.mid,@WA.FilesAdd,0,@WA.FilesAdd)

          while %WA.Files.Add.Counter <= $line(@WA.FilesAdd,0,1) {
            run $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,WADIR) $+ winamp.exe /add " $+ $line(@WA.FilesAdd,%WA.Files.Add.Counter,1) $+ "

            inc %WA.Files.Add.Counter
          }
          window -c @WA.FilesAdd
        }
      }

      elseif $$1 == PLADDF {
        if $lock(run) == $false {
          run $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,WADIR) $+ winamp.exe /add " $+ $sfile(c:\,Select track) $+ "
        }
      }

      elseif $$1 == PLADD {
        if $lock(run) == $false {
          if $2 == $null {
            run $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,WADIR) $+ winamp.exe /add $$?="Full path or URL. Enclose multiple tracks with double quotes (ASCII #34)"
          }
          else {
            run $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,WADIR) $+ winamp.exe /add $2-
          }
        }
      }

      elseif $$1 == CMD {
        if $2 == $null {
          echo -a 4Current stats command: %WA.Stats.Command
        }
        else {
          set %WA.Stats.Command $2-
          echo -a 4Stats command set to: %WA.Stats.Command
        }
      }

      elseif $$1 == VIEWPL {
        ViewPl
      }

      elseif $$1 == SAVEPL {
        SavePl
      }

      elseif $$1 == RAW {
        if $2 != $null {
          if $$2 isin SAMPLERATE CHANNELS TRACKSIZE BITRATE GETSHUFFLE GETREPEAT WADIR VERSION STATE TRACKSEC POSSEC TRACKNO TRACKFILENAME FILETYPE TITLE ARTIST ALBUM YEAR COMMENT GENRE {
            echo 4 -a $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,$$2)
          }
          elseif $$2 == HELP {
            echo 4 -s ----------------------------------
            echo 4 -s RAW Commands are NOT case sensitive.
            echo 4 -s RAW Stats are echoed back for demonstration purpose.
            echo 4 -s Check Developers.txt for more information.
            echo -s 4"/wa RAW <command>4"
            echo 4 -s ----------------------------------
            echo 4 -s Available RAW commands for WA_Link v1.2:
            echo 4 -s WADIR ......... Path to winamp
            echo 4 -s VERSION ....... Winamp version
            echo 4 -s STATE ......... Playstate returns:
            echo 4 -s                 $ $+ WA_PAUSED - Paused
            echo 4 -s                 $ $+ WA_STOPPED - Stopped
            echo 4 -s                 $ $+ WA_PLAYING - Playing
            echo 4 -s TRACKSEC ...... Track length in seconds
            echo 4 -s POSSEC ........ Seconds played
            echo 4 -s TRACKNO ....... Track number in playlist
            echo 4 -s TRACKFILENAME . Track filename, fullpath
            echo 4 -s FILETYPE ...... Extension or type of track
            echo 4 -s TITLE ......... Title of current track
            echo 4 -s ARTIST ........ Artist of current track
            echo 4 -s ALBUM ......... Album of current track
            echo 4 -s YEAR .......... Year of current track
            echo 4 -s COMMENT ....... Comment of current track
            echo 4 -s GENRE ......... Genre of current track
            echo 4 -s GETSHUFFLE .... Returns: $ $+ SHUFFLE_ON or $ $+ SHUFFLE_OFF
            echo 4 -s GETREPEAT ..... Returns: $ $+ REPEAT_ON or $ $+ REPEAT_OFF
            echo 4 -s BITRATE ....... Returns the bitrate of current track
            echo 4 -s SAMPLERATE .... Returns the samplerate of current track
            echo 4 -s CHANNELS ...... Returns number of channels (1=mono, 2=stereo)
            echo 4 -s TRACKSIZE ..... Returns the tracksize in bytes
            echo 4 -s ----------------------------------
            echo 4 -s Examples:
            echo -s /wa raw trackfilename
            echo -s /wa RAW wadir
            echo 4 -s ----------------------------------
          }
          else {
            echo -a 4Unknown WA RAW command: $2-
            echo -a 4For a complete RAW command list use: /wa raw help
          }
        }
        else {
          echo -a 4Insufficient parameters for WA RAW command
          echo -a 4For a complete RAW command list use: /wa raw help
        }
      }

      elseif $$1 == PLAYLIST {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,PLAYLIST)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == SHADE {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,SHADE)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == PLSHADE {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,PLSHADE)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == OPEN {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,OPEN)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == MINIMIZE {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,MINIMIZE)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == RESTORE {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,RESTORE)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == SHOW {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,SHOW)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == HIDE {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,HIDE)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == ONTOP {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,ONTOP)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == JUMP {
        set %WA.result $$2
        if %WA.result < 1 .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,JUMP 1)
        else .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,JUMP %WA.result)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == FIND {
        unset %WA.result
        ;If called from WA_Remote uses /wa find remote <search>
        if $$2 == REMOTE {
          .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,FIND $$3-)
          if (%WA.Result != -1) && (%WA.Result != -2) {
            .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,JUMP %WA.result)
          }
        }
        else {
          .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,FIND $$2-)
          if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
          elseif %WA.result == -2 echo 4 -a Your search for ( $+ $$2- $+ ) in the playlist did not return any results. Try different or more general keywords.
          else .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,JUMP %WA.result)
        }
      }

      elseif $$1 == EOFPL {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,EOFPL)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == BOFPL {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,BOFPL)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == THEME {
        if $$2 == LIST {
          echo 4 -s Themes found: $findfile($mircdirWA_Link\Themes\,*.INI,0,echo 4 -s $remove($1-,$mircdirWA_Link\Themes\))
          echo 4 -s Current theme: %WA.Theme.ShortName $+ .INI
          echo 4 -s ----------------------------------
          echo -s 4To set theme use: /wa theme set <ThemeName.INI> 4(Not directory)
          echo -s 4To set random theme feature: /wa theme set random 0/1 4(0=off, 1=on)
          echo -s 4To see random theme setting: /wa theme set random 4(Returns ON/OFF)
          echo 4 -s ----------------------------------
        }

        elseif $$2 == SET {
          if $$3 == RANDOM {
            if $4 == $null {
              if %WA.Theme.Random == $false {
                echo 4 -a Random theme feature: OFF
              }
              elseif %WA.Theme.Random == $true {
                echo 4 -a Random theme feature: ON
              }
            }
            elseif $4 == 0 {
              set %WA.Theme.Random $false
              echo 4 -a WA Link random theme feature: OFF
            }
            elseif $4 == 1 {
              set %WA.Theme.Random $true
              echo 4 -a WA Link random theme feature: ON
            }
          }
          elseif $exists($shortfn($mircdirWA_Link\Themes\) $+ $$3) == $false {
            echo 4 -a $$3 does not exist.
          }
          else {
            set %WA.Theme $shortfn($mircdirWA_Link\Themes\) $+ $$3

            echo 4 -a Theme set to: %WA.Theme
            set %WA.Theme.ShortName $remove($$3,.INI)
          }
        }
      }

      elseif $$1 == README {
        window -k0 @WA_Link.ReadMe
        filter -fw wa_link\readme.txt @WA_Link.ReadMe *
        sline @WA_Link.ReadMe 27
      }

      elseif $$1 == ABOUT {
        dll $shortfn($mircdirWA_Link\WA_Link.dll) WA_Link_About
      }

      elseif $$1 == STATS {
        if $2 == $null {
          set %WA.Stats $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Stats,%WA.Theme)

          ;Replace all fakespaces with realspaces
          set %WA.Stats $replace(%WA.Stats,$chr(160),$chr(32))

          set %WA.Stats $replace(%WA.Stats,$chr(44),$chr(160))

          set %WA.Stats $eval(%WA.Stats,2)

          set %WA.Stats $replace(%WA.Stats,$chr(160),$chr(44))

          %WA.Stats.Command %WA.Stats

          ;$eval(%WA.Stats.Command %WA.Stats,2)

          if %WA.Theme.Random == $true {
            RandomTheme
          }
        }
      }

      elseif $$1 == VOL {
        set %WA.vol $$2
        if (%WA.vol != UP) && (%WA.vol != DOWN) {
          if %WA.vol < 0 {
            set %WA.vol 0
          }
          if %WA.vol > 100 {
            set %WA.vol 100
          }
        }
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,VOL %WA.vol)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start 
      }

      elseif $$1 == PAN {
        set %WA.pan $$2
        if %WA.pan < -100 {
          set %WA.pan -100
        }
        if %WA.pan > 100 {
          set %WA.pan 100
        }
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,PAN %WA.pan)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }

      elseif $$1 == SEEK {
        set %WA.seek $$2
        if %WA.seek < 0 {
          set %WA.seek 0
        }
        if %WA.seek > 100 {
          set %WA.seek 100
        }
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,SEEK %WA.seek)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }
      else {
        unset %WA.result
        .set %WA.result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,$$1)
        if %WA.result == -1 echo -a 4ERROR: WinAMP is not running. First try: /wa start
      }
    }

    elseif $$1 == HELP {
      echo 4 -s ------------------------------------------------------------------------
      echo 4 -s Commands are NOT case sensitive.
      echo -s 4"/wa <command> [value]4" (Only a few commands need a value) to use it.
      echo 4 -s ------------------------------------------------------------------------
      echo 4 -s Available commands for WA_Link v1.2:
      echo 4 -s STATS ................ Shows info about current playing track
      echo 4 -s FIND X ............... X is a keyword to search for in playlist, if found WA_Link starts playing it
      echo 4 -s JUMP X................ X is a number in the playlist and WA_Link will jump to that track number and start playing
      echo 4 -s NEXT ................. Plays next track in playlist
      echo 4 -s PREV ................. Plays previous track in playlist
      echo 4 -s PAUSE ................ Toggles pause/play
      echo 4 -s PLAY ................. Starts or resumes playing
      echo 4 -s STOP ................. Stops the playing
      echo 4 -s BOFPL ................ Plays the first track in the playlist
      echo 4 -s EOFPL ................ Plays the last track in the playlist
      echo 4 -s FADESTOP ............. Fades to a stop
      echo 4 -s FFW5 ................. Fast-forwards 5 seconds
      echo 4 -s REW5 ................. Rewinds 5 seconds
      echo 4 -s SEEK X ............... X is a seek value in the range of 0 to 100 (%)
      echo 4 -s VOL X ................ X is a volume value in the range of 0 to 100 (%)
      echo 4 -s PAN X ................ X is a value ranging from -100 to 100.  -100 is left, 0 is center, 100 is right
      echo 4 -s SHUFFLE .............. Toggles shuffle/random play on/off
      echo 4 -s REPEAT ............... Toggles repeat play on/off
      echo 4 -s MINIMIZE ............. Minimizes Winamp (NOT the same as HIDE)
      echo 4 -s RESTORE .............. Restores a minimized Winamp (NOT the same as SHOW)
      echo 4 -s HIDE ................. Hides a shown Winamp (NOT the same as minimize.  Does NOT hide the playlist)
      echo 4 -s SHOW ................. Shows a hidden Winamp (NOT the same as restore. Used after HIDE)
      echo 4 -s ONTOP ................ Toggles Stay On Top on/off
      echo 4 -s PLAYLIST ............. Toggles the playlist show on/off
      echo 4 -s SHADE ................ Toggles the winamp shade mode on/off
      echo 4 -s PLSHADE .............. Toggles the playlist shade mode on/off
      echo 4 -s EQ ................... Toggles the equalizer window on/off
      echo 4 -s OPEN ................. Opens the Winamp open file(s) dialog
      echo 4 -s VISPLUG .............. Toggles visualisation plug-in on/off
      echo 4 -s CLOSE ................ Closes WinAMP
      echo 4 -s START ................ Starts WinAMP
      echo 4 -s THEME ................ Needs additional parameter (LIST or SET)
      echo 4 -s THEME SET X .......... X is the name of the theme you want (with extension, but not directory)
      echo 4 -s THEME SET RANDOM ..... Returns if the random theme feature is on or off
      echo 4 -s THEME SET RANDOM 0/1 . Turns the random theme feature on or off (0=off, 1=on)
      echo 4 -s THEME LIST ........... Lists all themes available in the themes directory (Themes\*.INI)
      echo 4 -s SAVEPL ............... Saves the default playlist in $mircdirWA_Link\Playlist.txt
      echo 4 -s VIEWPL ............... Views the default playlist in a window, double click to jump to that track
      echo 4 -s PLADD ................ Adds a track to playlist (Full path required, multiple tracks enclosed in " )
      echo 4 -s PLADDF ............... Adds a single track to playlist
      echo 4 -s PLADDD ............... Adds a folder to playlist (Subfolders not added and only most common extensions)
      echo -s 4CMD [X] .............. If X is not specified, views current /wa stats4 command, else it sets a new one
      echo 4 -s ABOUT ................ Shows about info
      echo 4 -s HELP ................. Shows this command list
      echo 4 -s README ............... Shows the readme file
      echo -s 4RAW X ................ Echoes to active window raw winamp information (use: /wa raw help 4for list)
      echo 4 -s WRITEPL .............. Makes winamp write the default playlist (wadir\winamp.m3u)
      echo 4 -s ------------------------------------------------------------------------
      echo 4 -s Examples:
      echo -s /wa Vol 75
      echo -s /wa pause
      echo -s /wa Stats
      echo -s /wa Visplug
      echo -s /wa seek 75
      echo -s /wa pladd "c:\music\winamp\demo.mp3" "c:\music\winamp\songs\britney spears - one more time.mp3"
      echo 4 -s ------------------------------------------------------------------------
    }
    else {
      echo -a 4Unknown WA command: $1-
      echo -a 4For a complete WA Link command list use: /wa help
    }
  }
  ;end if $1 != $null
  else {
    echo -a 4Insufficient parameters for WA command
    echo -a 4For a complete WA Link command list use: /wa help
  }
}

;----------------- Theme editor -----------------------------
alias WA_Theme_Edit {
  if $window(@Edit: $+ $$2).state != $null {
    dline @Edit: $+ $$2 1-18
  }

  window -k0l -t72 @Edit: $+ $$2

  set %WA.Temp.Preview.Window @Edit: $+ $$2
  set %WA.Temp.Preview.File $$1
  set %WA.Temp.Preview.ShortName $$2

  aline @Edit: $+ $$2 $read( $$1 ,n ,1) 	- Playing
  aline @Edit: $+ $$2 $read( $$1 ,n ,2) 	- Stopped
  aline @Edit: $+ $$2 $read( $$1 ,n ,3) 	- Paused
  aline @Edit: $+ $$2 $read( $$1 ,n ,4) 	- Not running
  aline @Edit: $+ $$2 $read( $$1 ,n ,5) 	- Title
  aline @Edit: $+ $$2 $read( $$1 ,n ,6) 	- Artist
  aline @Edit: $+ $$2 $read( $$1 ,n ,7) 	- Album
  aline @Edit: $+ $$2 $read( $$1 ,n ,8) 	- Year
  aline @Edit: $+ $$2 $read( $$1 ,n ,9) 	- Genre
  aline @Edit: $+ $$2 $read( $$1 ,n ,10) 	- Comment
  aline @Edit: $+ $$2 $read( $$1 ,n ,11) 	- Lengthline
  aline @Edit: $+ $$2 $read( $$1 ,n ,12) 	- Done bar
  aline @Edit: $+ $$2 $read( $$1 ,n ,13) 	- Remaining bar
  aline @Edit: $+ $$2 $read( $$1 ,n ,14) 	- States shown
  aline @Edit: $+ $$2 $read( $$1 ,n ,15) 	- Default playlist not found
  aline @Edit: $+ $$2 $read( $$1 ,n ,16) 	- Winamp doesn't return playstate
  aline @Edit: $+ $$2 4---------------------------------------------------------------------------------------------------------
  aline @Edit: $+ $$2 4Double click a line to edit the line
}

ON *:CLOSE:@Edit*:{
  unset %WA.Temp.Preview.Window
  unset %WA.Temp.Preview.File
  unset %WA.Temp.Preview.ShortName
}

menu @Edit* {
  dclick: {
    if $sline(%WA.Temp.Preview.Window,1).ln < 17 {
      .dialog -m EditTheme EditTheme
    }
  }
}

;----------------- Theme preview thingie -----------------------------
alias WA_PreviewTheme {
  var %WA.Temp.Preview.Counter
  var %WA.Temp.Preview.Lines
  var %WA.Temp.Preview.Bar.Done
  var %WA.Temp.Preview.Bar.Rem
  var %WA.Temp.Preview.Bar
  var %WA.Temp.Preview.Location
  var %WA.Temp.Preview.Text
  var %WA.Temp.Preview.Text.Width

  window -c @WA.Link.Theme.Preview.Window
  window -ph @WA.Link.Theme.Preview.Window 10 10 285 45

  set %WA.Temp.Preview.Counter 1
  set %WA.Preview

  ;Line 14 is what to display and in what order
  set %WA.Temp.Preview.Lines $read(%WA.Preview.FileName, 14)

  ;While there are lines to display, display them
  while (%WA.Temp.Preview.Counter <= $len(%WA.Temp.Preview.Lines)) {
    ;It's nothing...
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == +) {
      inc %WA.Temp.Preview.Counter
    }
    ;Show status
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == S) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 1)
    }
    ;Show title
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == T) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 5)
    }
    ;Show artist
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == A) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 6)
    }
    ;Show album
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == B) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 7)
    }
    ;Show year
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == Y) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 8)
    }
    ;Show genre
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == G) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 9)
    }
    ;Show comment
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == C) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName, n, 10)
    }
    ;Show length, donepercent and bar
    if ($right($left(%WA.Temp.Preview.Lines,%WA.Temp.Preview.Counter),1) == L) {
      set %WA.Preview %WA.Preview $+ $read(%WA.Preview.FileName ,n ,11)
      set %WA.Temp.Preview.Bar.Done $read(%WA.Preview.FileName, 12) $+ $read(%WA.Preview.FileName, 12) $+ $read(%WA.Preview.FileName, 12) $+ $read(%WA.Preview.FileName, 12) $+ $read(%WA.Preview.FileName, 12)
      set %WA.Temp.Preview.Bar.Rem $read(%WA.Preview.FileName, 13) $+ $read(%WA.Preview.FileName, 13) $+ $read(%WA.Preview.FileName, 13) $+ $read(%WA.Preview.FileName, 13) $+ $read(%WA.Preview.FileName, 13)
      set %WA.Temp.Preview.Bar %WA.Temp.Preview.Bar.Done $+ %WA.Temp.Preview.Bar.Rem
      set %WA.Preview $replace(%WA.Preview,% $+ BAR,%WA.Temp.Preview.Bar)
    }
    inc %WA.Temp.Preview.Counter
  }

  set %WA.Temp.Preview.Text $remove(%WA.Preview,*)
  set %WA.Temp.Preview.Text.Width $width(%WA.Temp.Preview.Text,FixedSys,12,0,1)

  set %WA.Temp.Preview.Location $calc($calc($calc(%WA.Temp.Preview.Text.Width * $did(WA_Themes,10).sel) / 100) * -1)

  drawtext -ph @WA.Link.Theme.Preview.Window 1 FixedSys 12 %WA.Temp.Preview.Location 1 %WA.Temp.Preview.Text
  drawsave @WA.Link.Theme.Preview.Window WA_Link\WA_Link_Preview.bmp

  did -gv WA_Themes 7 WA_Link\WA_Link_Preview.bmp
  .remove WA_Link\WA_Link_Preview.bmp
}

;----------------- Playlist thingie -----------------------------
alias SavePl {
  var %WA.Temp.Playlist.File
  var %WA.Temp.Playlist.Line
  var %WA.Temp.Playlist.Line.Length

  var %WA.Temp.Playlist.Track.Number
  var %WA.Temp.Playlist.Track.Length
  var %WA.Temp.Playlist.Track.Length.Min
  var %WA.Temp.Playlist.Track.Length.Sec
  var %WA.Temp.Playlist.Track.Length.Len

  var %WA.Temp.Playlist.Counter

  wa writepl

  set %WA.Temp.Playlist.Track.Number 1

  set %WA.Temp.Playlist.File $shortfn($dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Raw_Stats,WADIR) $+ Winamp.m3u)

  filter -ffc %WA.Temp.Playlist.File WA_Link\Playlist.tmp *EXTINF*
  .remove WA_Link\Playlist.txt

  while ($read(WA_Link\Playlist.tmp,%WA.Temp.Playlist.Track.Number) != $null) {
    ;Read a line (track) from the playlist and process it
    {
      set %WA.Temp.Playlist.Line $read(WA_Link\Playlist.tmp,%WA.Temp.Playlist.Track.Number)
      set %WA.Temp.Playlist.Line.Length $len(%WA.Temp.Playlist.Line)
      set %WA.Temp.Playlist.Line $right(%WA.Temp.Playlist.Line,$calc(%WA.Temp.Playlist.Line.Length - 8))
      set %WA.Temp.Playlist.Line.Length $len(%WA.Temp.Playlist.Line)

      set %WA.Temp.Playlist.Counter 1

      while ($right($left(%WA.Temp.Playlist.Line,%WA.Temp.Playlist.Counter),1) != $chr(44)) {
        inc %WA.Temp.Playlist.Counter
      }

      set %WA.Temp.Playlist.Track.Length $left(%WA.Temp.Playlist.Line,$calc(%WA.Temp.Playlist.Counter - 1))    
      set %WA.Temp.Playlist.Track.Length.Len $len(%WA.Temp.Playlist.Track.Length)
      inc %WA.Temp.Playlist.Track.Length.Len

      while (%WA.Temp.Playlist.Track.Length > 60) {
        dec %WA.Temp.Playlist.Track.Length 60
        inc %WA.Temp.Playlist.Track.Length.Min 1
      }
      set %WA.Temp.Playlist.Track.Length.Sec %WA.Temp.Playlist.Track.Length

      set %WA.Temp.Playlist.Line $right(%WA.Temp.Playlist.Line,$calc(%WA.Temp.Playlist.Line.Length - %WA.Temp.Playlist.Track.Length.Len)))
    }

    ;Write to file
    {
      if (%WA.Temp.Playlist.Track.Length.Sec < 10) {
        set %WA.Temp.Playlist.Track.Length.Sec 0 $+ %WA.Temp.Playlist.Track.Length.Sec
      }
      write WA_Link\Playlist.txt %WA.Temp.Playlist.Track.Number $+ . %WA.Temp.Playlist.Line $chr(91) $+ %WA.Temp.Playlist.Track.Length.Min $+ : $+ %WA.Temp.Playlist.Track.Length.Sec $+ $chr(93)
    }

    ;Increment variable to read next line (track) and reset all track-length-related variables
    set %WA.Temp.Playlist.Track.Length
    set %WA.Temp.Playlist.Track.Length.Min
    set %WA.Temp.Playlist.Track.Length.Sec
    set %WA.Temp.Playlist.Line.Length
    set %WA.Temp.Playlist.Line
    set %WA.Temp.Playlist.Counter

    inc %WA.Temp.Playlist.Track.Number
  }
  .remove WA_Link\Playlist.tmp
}

alias ViewPl {
  var %WA.Playlist.Counter = 1

  if $window(@WA.Playlist).state != $null {
    dline @WA.Playlist 1- $+ $line(@WA.Playlist,0,1)
  }

  SavePl

  window -k0l @WA.Playlist

  aline @WA.Playlist 4Double click to jump to track, or right click to refresh, find or add tracks
  aline @WA.Playlist 4-----------------------------

  while $read(WA_Link\Playlist.txt,%WA.Playlist.Counter) != $null {
    aline @WA.Playlist $read(WA_Link\Playlist.txt,%WA.Playlist.Counter)

    inc %WA.Playlist.Counter
  }
}

menu @WA.Playlist {
  dclick: {
    if $sline(@WA.Playlist,1).ln > 2 {
      wa jump $calc($sline(@WA.Playlist,1).ln - 2)
    }
  }
}

menu @WA.Playlist {
  ;Right click popup menu
  Find Track:{
    var %WA.Temp.Playlist.Result
    var %WA.Temp.Playlist.Search

    .set %WA.Temp.Playlist.Search $$?="Enter keyword to search for in playlist"

    .set %WA.Temp.Playlist.Result $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,FIND %WA.Temp.Playlist.Search)

    if (%WA.Temp.Playlist.Result != -1) && (%WA.Temp.Playlist.Result != -2) {
      $dll($shortfn($mircdirWA_Link\WA_Link.dll),WA_Link_Command,JUMP %WA.Temp.Playlist.Result)
      sline @WA.Playlist $calc(%WA.Temp.Playlist.Result + 2)
    }
  }
  -
  Add Track: wa pladdf | .timer 1 1 ViewPl
  Add Folder: wa pladdd | .timer 1 1 ViewPl
  Add URL: wa pladd | .timer 1 1 ViewPl
  -
  Refresh: wa viewpl
}

;----------------- Random theme thingie -----------------------------
alias RandomTheme {
  var %WA.Temp.Themes

  window -lh @RandomTheme

  .set %WA.Temp.Themes $findfile($mircdirWA_Link\Themes\,*.INI,0,aline @RandomTheme $1-)

  set %WA.Theme $line(@RandomTheme,$rand(1,%WA.Temp.Themes),1)

  set %WA.Theme.ShortName $remove(%WA.Theme,$mircdirWA_Link\Themes\,.INI)
  set %WA.Theme $shortfn(%WA.Theme)

  window -c @RandomTheme
}
