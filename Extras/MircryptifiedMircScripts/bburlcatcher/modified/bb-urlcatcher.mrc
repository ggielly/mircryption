//---------------------------------------------------------------------------
; MY CUSTOMIZATIONS
alias bb-url-catcher-urlopen {
  if ( %bb-url-catcher-urlopenrun != $null ) && ( $event != $null ) {
    if ( $input(Do you really want to open $iif($len($1) > 40,$left($1,40) $+ ...,$1) via /run? This will execute the url on system!,ywsa,@BB-URL-Catcher,Really open URL via /run?) == $true ) run $1
  }
  else {
    ; ATTN call this replacement url function so we can redirect it
    bbMyCustom_url $+($iif(%bb-url-catcher-urlopennew != $null || %bb-url-catcher-urlopenactivate != $null,-),$iif(%bb-url-catcher-urlopennew != $null,n),$iif(%bb-url-catcher-urlopenactivate != $null,a)) $1-
  }
  :end
}

alias bbMyCustom_url {
  ; just invoke the /url command by default, or a browserswitch version if the /http version is available
  if ($isalias(http)) {
; use the http command from browserswitch
    .http $1-
  }
  else {
; normal url launch
    .url $1-
  }
}

on *:SIGNAL:MircryptionSignal: {
  ; catch mircryption decrypted incoming text and check it for urls
  ; $1 = event type (text,notice,action,join,topic,part,kick,quit,nick,mode)
  ; $2 = target ($chan)
  ; $3 = speaker ($nick)
  ; $4- = decrypted text
  bb-url-catch $4-
}
//---------------------------------------------------------------------------




; BrotBueXes URL-Catcher (March 2003)
;
; v0.01 - Initial Testing Release :)
;
; v0.02 - Added Navigation in Detaildialog
;         Added URL-Icons
;         Added Search-Dialog (not working yet :/ )
;
; v0.03 - Fixed sort of urllist
;         Search-Dialog first working version
;
; v0.04 - Search-Next added
;
; v0.05 - removed the crap hashtable-shit, use now a tab-delimeted file
;         can convert the old hashtable via menuitem, or via alias bb-url-catcher-convert
;
; v0.06 - catch own urls
;
; v0.07 - add configure dialog
;         can now make favorites for internet explorer
;         types of urls (wildcards) now configurable
;         different actions on catch selectable
;         you can exclude chans, nicks and msgs (wildcards allowed)
;         popups to quick exclude chans/nicks
;         can remove excludes later from history-file
;         dupedetection can be disabled
;
; v0.08 - bugfixes
;           remove with rightclick doesnt work
;           autoadd standardwildcardstrings on load was too late :)
;
; v0.09 - Multiserver send-to-popupmenu in BB-URL-Catcher-Window (Chans, Querys, Chats)
;         Unload-Messages added
;
; v0.10 - Changed the default wildcardstrings (replace ? to :, ? was a workaround from a very old version)
;         HTTP/FTP/ED2K buttons only add wildcardstrings that not in list
;         Now you can add a url manually via dialog
;         Config-Dialog has now right tab-order
;         Use now $input for question-dialogs
;         Different small modifications (see options)
;         Added more options (window-behavior, urlopen-behavior, etc.)
;         Can now create nick/chan subfolders on favorite-dir
;         Url can now open via /run (no autoopen) instead /url (dangerous!, but its give a warning)
;         fixed bug, no catch in query/chat windows on input
;         very nice html-export (use layout-settings from mIRC :)
;
; v0.11 - Changed ftp.*.* to ftp.*.??* and www.*.* to www.*.??*
;         bugfix in export ( wrote $$ instead of && :)
;
; v0.12 - the td1/td2 colors in html export was not dimmed correctly
;
; v0.13 - bugfix, dupecheck dont work
;
; v0.14 - changed config dialog to 2 tabs
;
; v0.15 - now use highlightcolor for mark new urls
;
; v0.16 - fixed bug ('/msg bla http://www.test.de' in a chan-window put this url as sayed in this chan)
;
; v0.17 - fixed bug with spaces in paths
;
; ToDo:
;         select programs to urls
;         catch email-adresses (ugly wildcardstyle / like a hostmask)
;         hashtable for settings
;         select found option in searchdialog, select all option in popupmenu
;         html-export: sortable via javascript *g*, calculate also link-colors, cut too long urls
;         fix wildcardmatching (check if * exists in %match?)
;         check changed items in config dialog (reopen url-window)
;         open dialogs on desktop when bb-url-catcher-window also on desktop
;         use regional settings for date/time
;         automake htmlfile (autosend to website via ftp?)
;         add editfield for filename on html-export-question
;
; Known Bugs/Limitations:
;         every time a dupereplace of a new-marked url is done, 1 more old item is marked :/ i'll try to fix this
;         www.*.* removes last wildcard matched www.*.* url :/ (workaround added)
;

on *:LOAD: {
  echo -ta Welcome to BB's URL-Catcher v0.17 (Dec 2002)
  echo -ta It will catch all urls (if its possible), store them in a historyfile and show it in a better way as the internal catcher.
  echo -ta Open the window with the menupoint "BB-URL-Catcher" from menubar or channelmenu ...
  set %bb-url-catcher-new 0
  if ( $isfile(" $+ $scriptdir $+ bb-urlcatcher.hst $+ ") ) bb-url-catcher-convert
  if ( %bb-url-catcher-wildcards == $null ) set %bb-url-catcher-wildcards $+(ed2k://|file|*|*|*|,$chr(9),ed2k://|file|*|*|*|/,$chr(9),ftp.*.??*,$chr(9),ftp://*.*,$chr(9),http://*.*,$chr(9),www.*.??*)
  dialog -m bb-url-catcher-config config
}

on *:START: {
  set %bb-url-catcher-new 0
  if ( %bb-url-catcher-new-value > 0 ) { set %bb-url-catcher-count $calc(%bb-url-catcher-count + %bb-url-catcher-new-value) | unset %bb-url-catcher-new-value }
  if ( %bb-url-catcher-openstart != $null ) bb-url-catcher-window
}

on *:UNLOAD: {
  unset %bb-url-catcher*
  echo -ta All Variables from BB's URL-Catcher was removed...
  if ( $isfile(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") ) echo -ta To remove completly remove the History: $scriptdir $+ bb-urlcatcher.hist
}

on *:TEXT:*:*: { bb-url-catch $1- }
on *:NOTICE:*:*: { bb-url-catch $1- }
on *:ACTION:*:*: { bb-url-catch $1- }
on *:CHAT:*: { bb-url-catch $1- }
on *:INPUT:*: { if ( $left($1,1) != / ) bb-url-catch $1- }

alias bb-url-catch {
  if ( %bb-url-catcher-off != $null ) || ( %bb-url-catcher-wildcards == $null ) goto end
  if ( ( $chan != $null ) && ( $wildtok(%bb-url-catcher-excl-chans,$chan,1,9) == $chan ) ) || ( $wildtok(%bb-url-catcher-excl-nicks,$nick,1,9) == $nick ) goto end
  var %h = $numtok(%bb-url-catcher-excl-msgs,9)
  while ( %h > 0 ) {
    if ( $gettok(%bb-url-catcher-excl-msgs,%h,9) iswm $1- ) goto end
    dec %h
  }
  var %i = $numtok(%bb-url-catcher-wildcards,9)
  while ( %i > 0 ) {
    if ( $wildtok($strip($1-),$gettok(%bb-url-catcher-wildcards,%i,9),0,32) != 0 ) var %j = $ifmatch
    else { dec %i | continue }
    unset %match
    while ( %j > 0 ) {
      var %match = $wildtok($strip($1-),$gettok(%bb-url-catcher-wildcards,%i,9),%j,32)
      ; Dupecheck and Replace
      if ( %bb-url-catcher-dupeoff == $null ) && ( $read(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ",w,* $+ $chr(9) $+ %match $+ $chr(9) $+ *,0) != $null ) {
        ; temp. workaround, do not replace urls when catch wildcards (see listed bugs)
        if ( %match == $gettok($read(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ",$readn),4,9) ) {
          write -dl $readn " $+ $scriptdir $+ bb-urlcatcher.hist $+ "
          if ( $window(@BB-URL-Catcher,0) != 0 ) bb-url-catcher-refreshwindow
        }
      }
      ; add to histfile
      set %bb-url-catcher-count $calc( %bb-url-catcher-count + 1 )
      set %bb-url-catcher-new $calc( %bb-url-catcher-new + 1 )
      write -il1 " $+ $scriptdir $+ bb-urlcatcher.hist $+ " $+($asctime(dd.mm. HH:nn:ss),$chr(9),$iif($chan != $null,$chan,private),$chr(9),$nick,$chr(9),%match,$chr(9),$1-)
      ; check maxvalue an cut if its exceeded
      if ( %bb-url-catcher-max != $null ) && ( $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") > %bb-url-catcher-max ) {
        var %k = %bb-url-catcher-max
        while ( %k < $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") ) {
          write -d " $+ $scriptdir $+ bb-urlcatcher.hist $+ "
        }
      }
      ; insert in window when opened
      if ( $window(@BB-URL-Catcher,0) != 0 ) {
        iline $iif(%bb-url-catcher-marknew != $null,$color(high)) @BB-URL-Catcher 2 $+($asctime(dd.mm. HH:nn:ss),$chr(9),$iif($chan != $null,$chan,private),$chr(9),$nick,$chr(9),%match,$chr(9),$1-)
        renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new - Overall: %bb-url-catcher-count
      }
      if ( %bb-url-catcher-opencatch != $null ) && ( $window(@BB-URL-Catcher,0) == 0 ) bb-url-catcher-window
      if ( %bb-url-catcher-switchbar != $null ) && ( $window(@BB-URL-Catcher,0) != 0 ) window -g2 @BB-URL-Catcher
      if ( %bb-url-catcher-sound != $null ) beep
      if ( %bb-url-catcher-urlopen != $null ) bb-url-catcher-urlopen %match
      dec %j
    }
    dec %i
  }
  :end
}

alias bb-url-catcher-window {
  ; open window / and load or reload histfile
  if ( $window(@BB-URL-Catcher,0) == 0 ) {
    window $+(-,$iif(%bb-url-catcher-opendesktop ,d),$iif(%bb-url-catcher-openontop ,o),blk0z) $+(-t,$iif(%bb-url-catcher-tabs != $null,$ifmatch,$+(16,$chr(44),30,$chr(44),40,$chr(44),300))) @BB-URL-Catcher $mircdir $+ mirc.exe 12
    renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new - Overall: %bb-url-catcher-count
  }
  else window -a @BB-URL-Catcher
  bb-url-catcher-refreshwindow
}

alias bb-url-catcher-refreshwindow {
  if ( $window(@BB-URL-Catcher,0) != 0 ) {
    ; reload the histfile in window
    clear @BB-URL-Catcher
    aline @BB-URL-Catcher $+(,Date,$chr(9),Channel,$chr(9),Nickname,$chr(9),URL,$chr(9),Message)
    renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new - Overall: %bb-url-catcher-count
    if ( $isfile(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") == $true ) filter -fw " $+ $scriptdir $+ bb-urlcatcher.hist $+ " @BB-URL-Catcher

    if ( %bb-url-catcher-marknew != $null ) && ( %bb-url-catcher-new > 0 ) {
      var %z = %bb-url-catcher-new
      while ( %z > 0 ) {
        cline $color(high) @BB-URL-Catcher $calc(%z + 1)
        dec %z
      }
    }
  }
}

menu @BB-URL-Catcher {
  dclick: if ( $sline(@BB-URL-Catcher,1).ln > 1 ) bb-url-catcher-urlopen $gettok($sline(@BB-URL-Catcher,1),4,9)
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) &Open URL(s): {
    var %i = $sline(@BB-URL-Catcher,0)
    while ( %i > 0 ) {
      bb-url-catcher-urlopen $gettok($sline(@BB-URL-Catcher,%i),4,9)
      dec %i
    }
  }
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) &Copy URL: clipboard $gettok($sline(@BB-URL-Catcher,1),4,9)
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) Copy &Entry: clipboard Date: $gettok($sline(@BB-URL-Catcher,1),1,9) Chan/Nick: $gettok($sline(@BB-URL-Catcher,1),2,9) $+ / $+ $gettok($sline(@BB-URL-Catcher,1),3,9) URL: $gettok($sline(@BB-URL-Catcher,1),4,9)
  $iif( ( $sline(@BB-URL-Catcher,1).ln == 1 ) || ( !$isdir(" $+ %bb-url-catcher-favorites $+ ") ) ,$style(2)) &Make IE-Favorite: if ( $sline(@BB-URL-Catcher,1).ln > 1 ) { bb-url-catcher-create-fav }
  $iif( ( $sline(@BB-URL-Catcher,1).ln == 1 ) || ( !$isdir(" $+ %bb-url-catcher-htmls $+ ") ) ,$style(2)) E&xport URL(s) to HTML...: if ( $sline(@BB-URL-Catcher,1).ln > 1 ) { bb-url-catcher-exporthtml }
  -
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) &Send to
  .$submenu($bb-url-sendto-submenu($1))
  -
  &Add Manual...: dialog -m bb-url-catcher-newentry newentry
  &Find...: dialog -m bb-url-catcher-search search
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) &Details...: dialog -m bb-url-catcher-details details
  &Unmark New-Items: { set %bb-url-catcher-new 0 | bb-url-catcher-refreshwindow }
  $iif( $sline(@BB-URL-Catcher,1).ln == 1 ,$style(2)) &Remove URL(s)...: {
    if ( $input(Do you want to remove $sline(@BB-URL-Catcher,0) Item(s)?,yqsa,@BB-URL-Catcher,Remove $sline(@BB-URL-Catcher,0) URL(s)?) == $true ) {
      var %i = $sline(@BB-URL-Catcher,0)
      while ( %i > 0 ) {
        write -dl $+ $calc($sline(@BB-URL-Catcher,%i).ln - 1) " $+ $scriptdir $+ bb-urlcatcher.hist $+ "
        dline @BB-URL-Catcher $sline(@BB-URL-Catcher,%i).ln
        renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new  - Overall: %bb-url-catcher-count
        dec %i
      }
    }
  }
  Remove Excludes...: bb-url-catcher-remove-excludes
  -
  $iif( %bb-url-catcher-off == $null ,$style(1)) &Enabled: { if ( %bb-url-catcher-off != $null ) unset %bb-url-catcher-off | else set %bb-url-catcher-off 1 }
  Co&nfigure...: dialog -m bb-url-catcher-config config
  $iif( !$isfile(" $+ $scriptdir $+ bb-urlcatcher.hst $+ ") ,$style(2)) Convert Hashtable: bb-url-catcher-convert
  -
  Close &Window: window -c @BB-URL-Catcher
}

menu query,channel,menubar,chat,status,nicklist {
  BB-URL-Catcher
  .&Open Window: bb-url-catcher-window
  .&Add Manual...: dialog -m bb-url-catcher-newentry newentry
  .-
  .$iif( %bb-url-catcher-off == $null ,$style(1)) &Enabled: { if ( %bb-url-catcher-off != $null ) unset %bb-url-catcher-off | else set %bb-url-catcher-off 1 }
  .&Configure...: dialog -m bb-url-catcher-config config
}

menu channel {
  BB-URL-Catcher
  .-
  .$iif( $findtok(%bb-url-catcher-excl-chans,$chan,1,9) > 0 ,$style(1)) &Exclude Chan: { if ( $findtok(%bb-url-catcher-excl-chans,$chan,0,9) > 0 ) set %bb-url-catcher-excl-chans $remtok(%bb-url-catcher-excl-chans,$chan,1,9) | else set %bb-url-catcher-excl-chans $addtok(%bb-url-catcher-excl-chans,$chan,9) }
}

menu query,nicklist {
  BB-URL-Catcher
  .-
  .$iif( $findtok(%bb-url-catcher-excl-nicks,$1,1,9) > 0 ,$style(1)) &Exclude Nick: { if ( $findtok(%bb-url-catcher-excl-nicks,$1,0,9) > 0 ) set %bb-url-catcher-excl-nicks $remtok(%bb-url-catcher-excl-nicks,$1,1,9) | else set %bb-url-catcher-excl-nicks $addtok(%bb-url-catcher-excl-nicks,$1,9) }
}

dialog -l details {
  title "BB-URL-Catcher - Details"
  size -1 -1 300 105
  option dbu
  icon $mircdir $+ mirc.exe, 12
  text "Date:", 100, 0 7 30 11, right
  text "Channel:", 101, 86 7 25 11, right
  text "Nick:", 102, 195 7 15 11, right
  text "URL:", 103, 0 23 30 11, right
  text "Message:", 104, 0 39 30 11, right
  button "&Close", 10, 260 89 35 11, default cancel
  edit "", 200, 35 5 50 11
  edit "", 201, 115 5 80 11
  edit "", 202, 215 5 80 11
  edit "", 203, 35 21 260 11
  edit "", 204, 35 37 260 33, multi
  button "&Search...", 15, 100 74 35 11
  button "&Open", 11, 140 74 35 11
  button "&Copy URL", 12, 180 74 35 11
  button "Copy &Entry", 13, 220 74 35 11
  button "&Remove", 14, 220 89 35 11
  button "&First", 20, 5 90 35 11
  button "&Previous", 21, 5 75 35 11
  button "&Next", 22, 45 75 35 11
  button "&Last", 23, 45 90 35 11
  button "&Favorite", 16, 260 74 35 11
}

on *:dialog:bb-url-catcher-details:init:0: {
  bb-url-catcher-details-reload
}

on *:dialog:bb-url-catcher-details:sclick:11: {
  if ( $sline(@BB-URL-Catcher,1).ln > 1 ) bb-url-catcher-urlopen $gettok($sline(@BB-URL-Catcher,1),4,9)
}

on *:dialog:bb-url-catcher-details:sclick:12: {
  if ( $sline(@BB-URL-Catcher,1).ln > 1 ) clipboard $gettok($sline(@BB-URL-Catcher,1),4,9)
}

on *:dialog:bb-url-catcher-details:sclick:13: {
  if ( $sline(@BB-URL-Catcher,1).ln > 1 ) clipboard Date: $gettok($sline(@BB-URL-Catcher,1),1,9) Chan/Nick: $gettok($sline(@BB-URL-Catcher,1),2,9) $+ / $+ $gettok($sline(@BB-URL-Catcher,1),3,9) URL: $gettok($sline(@BB-URL-Catcher,1),4,9)
}

on *:dialog:bb-url-catcher-details:sclick:14: {
  if ( $sline(@BB-URL-Catcher,1).ln > 1 ) { if ( $input(Do you want to remove this Item?,yqs,@BB-URL-Catcher,Remove URL?) == $true ) { write -dl $+ $calc($sline(@BB-URL-Catcher,1) - 1) " $+ $scriptdir $+ bb-urlcatcher.hist $+ " | dline @BB-URL-Catcher $sline(@BB-URL-Catcher,1).ln | renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new  - Overall: %bb-url-catcher-count } }
}

on *:dialog:bb-url-catcher-details:sclick:15: {
  dialog -m bb-url-catcher-search search
}

on *:dialog:bb-url-catcher-details:sclick:16: {
  if ( $sline(@BB-URL-Catcher,1).ln > 1 ) bb-url-catcher-create-fav $gettok($sline(@BB-URL-Catcher,1),4,9)
}

on *:dialog:bb-url-catcher-details:sclick:20: {
  sline @BB-URL-Catcher 2
  bb-url-catcher-details-reload
}

on *:dialog:bb-url-catcher-details:sclick:21: {
  if ( $sline(@BB-URL-Catcher,1).ln > 2 ) sline @BB-URL-Catcher $calc( $sline(@BB-URL-Catcher,1).ln - 1 )
  bb-url-catcher-details-reload
}

on *:dialog:bb-url-catcher-details:sclick:22: {
  if ( $sline(@BB-URL-Catcher,1).ln < $line(@BB-URL-Catcher,0) ) sline @BB-URL-Catcher $calc( $sline(@BB-URL-Catcher,1).ln + 1 )
  bb-url-catcher-details-reload
}

on *:dialog:bb-url-catcher-details:sclick:23: {
  sline @BB-URL-Catcher $line(@BB-URL-Catcher,0)
  bb-url-catcher-details-reload
}

alias bb-url-catcher-details-reload {
  did -r bb-url-catcher-details 200,201,202,203,204
  did -am bb-url-catcher-details 200 $gettok($sline(@BB-URL-Catcher,1),1,9)
  did -am bb-url-catcher-details 201 $gettok($sline(@BB-URL-Catcher,1),2,9)
  did -am bb-url-catcher-details 202 $gettok($sline(@BB-URL-Catcher,1),3,9)
  did -am bb-url-catcher-details 203 $gettok($sline(@BB-URL-Catcher,1),4,9)
  did -am bb-url-catcher-details 204 $gettok($sline(@BB-URL-Catcher,1),-1,9)
  if ( !$isdir(%bb-url-catcher-favorites) ) did -b bb-url-catcher-details 16
}

dialog -l search {
  title "BB-URL-Catcher - Search"
  size -1 -1 190 63
  option dbu
  icon $mircdir $+ mirc.exe, 12

  text "Search:", 100, 5 7 30 11
  edit "", 200, 35 5 150 11

  box "Search in:" , 101, 5 21 180 21
  radio "&Date", 201, 10 29 30 11
  radio "C&hannel", 202, 45 29 30 11
  radio "&Nick", 203, 80 29 30 11
  radio "&URL", 204, 115 29 30 11
  radio "&All", 205, 150 29 30 11

  text "", 300, 5 49 60 11
  button "&Search", 90, 85 47 30 11, default
  button "&Next", 92, 120 47 30 11
  button "&Cancel", 91, 155 47 30 11, cancel
}

on *:dialog:bb-url-catcher-search:init:0: {
  did -b bb-url-catcher-search 92
  did -c bb-url-catcher-search 204
}

on *:dialog:bb-url-catcher-search:sclick:90: {
  did -r bb-url-catcher-search 300
  if ( $did(bb-url-catcher-search,100) == $null ) halt
  if ( $did(bb-url-catcher-search,201).state == 1 ) var %search = $+($did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,202).state == 1 ) var %search = $+(*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,203).state == 1 ) var %search = $+(*,$chr(9),*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,204).state == 1 ) var %search = $+(*,$chr(9),*,$chr(9),*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,205).state == 1 ) var %search = $+(*,$did(bb-url-catcher-search,200),*)

  if ( $fline(@BB-URL-Catcher,%search,0) == 0 ) {
    did -a bb-url-catcher-search 300 Nothing found :/
    halt
  }

  if ( $fline(@BB-URL-Catcher,%search,1) != 0 ) {
    did -a bb-url-catcher-search 300 Show 1 of $fline(@BB-URL-Catcher,%search,0)
    sline @BB-URL-Catcher $ifmatch
    if ( $dialog(bb-url-catcher-details) != $null ) bb-url-catcher-details-reload
    if ( $fline(@BB-URL-Catcher,%search,0) > 1 ) did -e bb-url-catcher-search 92
  }
}

on *:dialog:bb-url-catcher-search:sclick:201: {
  did -b bb-url-catcher-search 92
}
on *:dialog:bb-url-catcher-search:sclick:202: {
  did -b bb-url-catcher-search 92
}
on *:dialog:bb-url-catcher-search:sclick:203: {
  did -b bb-url-catcher-search 92
}
on *:dialog:bb-url-catcher-search:sclick:204: {
  did -b bb-url-catcher-search 92
}
on *:dialog:bb-url-catcher-search:sclick:205: {
  did -b bb-url-catcher-search 92
}
on *:dialog:bb-url-catcher-search:edit:200: {
  did -b bb-url-catcher-search 92
}

on *:dialog:bb-url-catcher-search:sclick:92: {
  if ( $did(bb-url-catcher-search,201).state == 1 ) var %search = $+($did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,202).state == 1 ) var %search = $+(*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,203).state == 1 ) var %search = $+(*,$chr(9),*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,204).state == 1 ) var %search = $+(*,$chr(9),*,$chr(9),*,$chr(9),*,$did(bb-url-catcher-search,200),*,$chr(9),*)
  elseif ( $did(bb-url-catcher-search,205).state == 1 ) var %search = $+(*,$did(bb-url-catcher-search,200),*)

  if ( $fline(@BB-URL-Catcher,%search,$calc( $gettok($did(bb-url-catcher-search,300),2,32) + 1 )) != 0 ) {
    var %dummy $calc( $gettok($did(bb-url-catcher-search,300),2,32) + 1 )
    did -r bb-url-catcher-search 300
    did -a bb-url-catcher-search 300 Show %dummy of $fline(@BB-URL-Catcher,%search,0)
    sline @BB-URL-Catcher $ifmatch
    if ( $dialog(bb-url-catcher-details) != $null ) bb-url-catcher-details-reload
  }
  if ( $gettok($did(bb-url-catcher-search,300),2,32) == $gettok($did(bb-url-catcher-search,300),4,32) ) { did -b bb-url-catcher-search 92 | halt }
}

dialog -l config {
  title "BB-URL-Catcher - Configure"
  size -1 -1 232 231
  option dbu
  icon $mircdir $+ mirc.exe, 12

  tab "General", 15, 3 1 224 207
  box "General:", 3, 10 18 212 24, tab 15
  check "Detect && Replace Dupes", 46, 65 26 80 11, tab 15
  check "Catch URLs", 200, 15 26 45 11, tab 15
  text "max URLs:", 5, 145 28 30 8, tab 15 right
  edit "", 4, 181 26 35 11, tab 15 right

  box "Export:", 28, 10 48 212 59, tab 15
  text "IE-Favorites-Folder:", 101, 15 58 55 8, tab 15
  check "Make Nick/Chan Subfolders", 41, 95 56 80 10, tab 15 left
  edit "", 201, 15 66 161 11, tab 15
  button "&Browse...", 202, 180 66 37 12, tab 15
  text "HTML-Export-Folder:", 39, 15 83 55 8, tab 15
  edit "", 40, 15 91 161 11, tab 15
  button "Bro&wse...", 31, 180 91 37 12, tab 15

  box "Window Settings:", 17, 10 114 112 35, tab 15
  check "Open on Start", 7, 15 122 50 11, tab 15
  check "Mark new URLs", 29, 70 123 50 10, tab 15
  check "Desktop-Window", 27, 15 133 50 10, tab 15
  check "Place On Top", 26, 70 133 50 10, tab 15

  box "On Catch:", 10, 125 114 97 35, tab 15
  check "Open Window", 6, 130 123 60 10, tab 15
  check "Play Sound", 9, 180 123 40 10, tab 15
  check "Highl. Switchbar", 8, 130 133 50 10, tab 15
  check "Open URL", 11, 180 133 40 10, tab 15

  box "Column positions:", 25, 10 156 112 45, tab 15
  text "Channel:", 21, 15 169 25 8, tab 15
  edit "", 32, 41 167 20 11, tab 15 right
  text "Nickname:", 22, 65 169 25 8, tab 15
  edit "", 33, 96 167 20 11, tab 15 right
  text "URL:", 23, 15 184 25 8, tab 15
  edit "", 34, 41 182 20 11, tab 15 right
  text "Message:", 24, 65 184 25 8, tab 15
  edit "", 35, 96 182 20 11, tab 15 right

  box "Open URLs:", 42, 125 156 97 45, tab 15
  check "Open a new Browser-Window", 43, 130 166 85 10, tab 15
  check "Activate Browser-Window", 44, 130 176 80 10, tab 15
  check "Open via /run instead of /url", 48, 130 186 85 10, tab 15

  tab "Wildcards", 36
  box "URL-Types / Wildcardstrings:", 37, 7 19 106 120, tab 36
  combo 18, 12 28 65 107, tab 36 sort size edit
  button "Add", 19, 82 28 27 12, tab 36
  button "Remove", 20, 82 43 27 12, tab 36
  button "HTTP", 13, 82 92 27 12, tab 36
  button "FTP", 14, 82 107 27 12, tab 36
  button "ED2K", 16, 82 122 27 12, tab 36

  box "Excludes / Wildcardstrings:", 12, 117 19 106 175, tab 36
  text "Nicknames:", 38, 123 27 40 8, tab 36
  combo 50, 122 93 65 42, tab 36 sort size edit
  button "Add", 51, 191 93 27 12, tab 36
  button "Remove", 52, 191 108 27 12, tab 36
  text "Channels:", 45, 123 82 40 8, tab 36
  combo 60, 122 38 65 42, tab 36 sort size edit
  button "Add", 61, 191 38 27 12, tab 36
  button "Remove", 62, 191 53 27 12, tab 36
  text "Messages:", 49, 123 137 40 8, tab 36
  combo 70, 122 148 65 42, tab 36 sort size edit
  button "Add", 71, 191 148 27 12, tab 36
  button "Remove", 72, 191 163 27 12, tab 36

  button "&Reset", 30, 5 215 37 12
  button "Remove excluded Items", 47, 45 215 67 12
  button "&OK", 1, 150 215 37 12, default ok
  button "&Cancel", 2, 190 215 37 12, cancel
}


on *:dialog:bb-url-catcher-config:init:0: {
  did -f bb-url-catcher-config 1
  if ( %bb-url-catcher-off == $null ) did -c bb-url-catcher-config 200
  if ( %bb-url-catcher-dupeoff == $null ) did -c bb-url-catcher-config 46
  if ( %bb-url-catcher-max != $null ) did -a bb-url-catcher-config 4 $ifmatch

  if ( %bb-url-catcher-wildcards != $null ) didtok bb-url-catcher-config 18 9 $ifmatch

  if ( %bb-url-catcher-excl-nicks != $null ) didtok bb-url-catcher-config 50 9 $ifmatch
  if ( %bb-url-catcher-excl-chans != $null ) didtok bb-url-catcher-config 60 9 $ifmatch
  if ( %bb-url-catcher-excl-msgs != $null ) didtok bb-url-catcher-config 70 9 $ifmatch

  if ( %bb-url-catcher-favorites != $null ) did -a bb-url-catcher-config 201 $ifmatch
  else did -a bb-url-catcher-config 201 $bb-url-catcher-get-fav-folder
  if ( %bb-url-catcher-favsub != $null ) did -c bb-url-catcher-config 41
  if ( %bb-url-catcher-htmls != $null ) did -a bb-url-catcher-config 40 $ifmatch
  else did -a bb-url-catcher-config 40 c:\

  if ( %bb-url-catcher-tabs != $null ) {
    did -a bb-url-catcher-config 32 $gettok(%bb-url-catcher-tabs,1,44)
    did -a bb-url-catcher-config 33 $gettok(%bb-url-catcher-tabs,2,44)
    did -a bb-url-catcher-config 34 $gettok(%bb-url-catcher-tabs,3,44)
    did -a bb-url-catcher-config 35 $gettok(%bb-url-catcher-tabs,4,44)
  }
  else {
    did -a bb-url-catcher-config 32 16
    did -a bb-url-catcher-config 33 30
    did -a bb-url-catcher-config 34 40
    did -a bb-url-catcher-config 35 300
  }

  if ( %bb-url-catcher-openstart != $null ) did -c bb-url-catcher-config 7
  if ( %bb-url-catcher-marknew != $null ) did -c bb-url-catcher-config 29
  if ( %bb-url-catcher-opendesktop != $null ) did -c bb-url-catcher-config 27
  if ( %bb-url-catcher-openontop != $null ) {
    did -c bb-url-catcher-config 26,27
    did -b bb-url-catcher-config 27
  }

  if ( %bb-url-catcher-opencatch != $null ) did -c bb-url-catcher-config 6
  if ( %bb-url-catcher-switchbar != $null ) did -c bb-url-catcher-config 8
  if ( %bb-url-catcher-sound != $null ) did -c bb-url-catcher-config 9
  if ( %bb-url-catcher-urlopen != $null ) did -c bb-url-catcher-config 11

  if ( %bb-url-catcher-urlopennew != $null ) did -c bb-url-catcher-config 43
  if ( %bb-url-catcher-urlopenactivate != $null ) did -c bb-url-catcher-config 44
  if ( %bb-url-catcher-urlopenrun != $null ) {
    did -c bb-url-catcher-config 48
    did -bu bb-url-catcher-config 43,44,11
  }
}

on *:dialog:bb-url-catcher-config:sclick:13: {
  if ( $findtok($didtok(bb-url-catcher-config,18,9),http://*.*,9) == $null ) did -a bb-url-catcher-config 18 http://*.*
  if ( $findtok($didtok(bb-url-catcher-config,18,9),www.*.??*,9) == $null ) did -a bb-url-catcher-config 18 www.*.??*
}

on *:dialog:bb-url-catcher-config:sclick:14: {
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ftp://*.*,9) == $null ) did -a bb-url-catcher-config 18 ftp://*.*
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ftp.*.??*,9) == $null ) did -a bb-url-catcher-config 18 ftp.*.??*
}

on *:dialog:bb-url-catcher-config:sclick:16: {
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ed2k://|file|*|*|*|/,9) == $null ) did -a bb-url-catcher-config 18 ed2k://|file|*|*|*|/
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ed2k://|file|*|*|*|,9) == $null ) did -a bb-url-catcher-config 18 ed2k://|file|*|*|*|
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ed2k://|server|*.*.*.*|*|,9) == $null ) did -a bb-url-catcher-config 18 ed2k://|server|*.*.*.*|*|
  if ( $findtok($didtok(bb-url-catcher-config,18,9),ed2k://|server|*.*.*.*|*|/,9) == $null ) did -a bb-url-catcher-config 18 ed2k://|server|*.*.*.*|*|/
}

on *:dialog:bb-url-catcher-config:sclick:19: {
  if ( $did(bb-url-catcher-config,18) != $null ) {
    if ( $findtok($didtok(bb-url-catcher-config,18,9),$did(bb-url-catcher-config,18),0,9) > 0 ) halt
    else did -a bb-url-catcher-config 18 $did(bb-url-catcher-config,18)
  }
}

on *:dialog:bb-url-catcher-config:sclick:20: {
  if ( $did(bb-url-catcher-config,18).sel != $null ) {
    did -d bb-url-catcher-config 18 $ifmatch
    did -d bb-url-catcher-config 18 0
  }
}

on *:dialog:bb-url-catcher-config:sclick:51: {
  if ( $did(bb-url-catcher-config,50) != $null ) {
    if ( $findtok($didtok(bb-url-catcher-config,50,9),$did(bb-url-catcher-config,50),0,9) > 0 ) halt
    else did -a bb-url-catcher-config 50 $did(bb-url-catcher-config,50)
  }
}

on *:dialog:bb-url-catcher-config:sclick:52: {
  if ( $did(bb-url-catcher-config,50).sel != $null ) {
    did -d bb-url-catcher-config 50 $ifmatch
    did -d bb-url-catcher-config 50 0
  }
}

on *:dialog:bb-url-catcher-config:sclick:61: {
  if ( $did(bb-url-catcher-config,60) != $null ) {
    if ( $findtok($didtok(bb-url-catcher-config,60,9),$did(bb-url-catcher-config,60),0,9) > 0 ) halt
    else did -a bb-url-catcher-config 60 $did(bb-url-catcher-config,60)
  }
}

on *:dialog:bb-url-catcher-config:sclick:62: {
  if ( $did(bb-url-catcher-config,60).sel != $null ) {
    did -d bb-url-catcher-config 60 $ifmatch
    did -d bb-url-catcher-config 60 0
  }
}

on *:dialog:bb-url-catcher-config:sclick:71: {
  if ( $did(bb-url-catcher-config,70) != $null ) {
    if ( $findtok($didtok(bb-url-catcher-config,70,9),$did(bb-url-catcher-config,70),0,9) > 0 ) halt
    else did -a bb-url-catcher-config 70 $did(bb-url-catcher-config,70)
  }
}

on *:dialog:bb-url-catcher-config:sclick:72: {
  if ( $did(bb-url-catcher-config,70).sel != $null ) {
    did -d bb-url-catcher-config 70 $ifmatch
    did -d bb-url-catcher-config 70 0
  }
}

on *:dialog:bb-url-catcher-config:sclick:26: {
  if ( $did(bb-url-catcher-config,26).state == 1 ) did -bc bb-url-catcher-config 27
  else did -e bb-url-catcher-config 27
}

on *:dialog:bb-url-catcher-config:sclick:48: {
  if ( $did(bb-url-catcher-config,48).state == 1 ) did -bu bb-url-catcher-config 43,44,11
  else did -e bb-url-catcher-config 43,44,11
}

on *:dialog:bb-url-catcher-config:sclick:202: {
  if ( $sdir($iif($isdir($did(bb-url-catcher-config,201)),$did(bb-url-catcher-config,201),c:\),Please select the Favorites Directory) != $null ) did -ra bb-url-catcher-config 201 $left($ifmatch,$calc($len($ifmatch) - 1))
}

on *:dialog:bb-url-catcher-config:sclick:31: {
  if ( $sdir($iif($isdir($did(bb-url-catcher-config,40)),$did(bb-url-catcher-config,40),c:\),Please select a Directory) != $null ) did -ra bb-url-catcher-config 40 $left($ifmatch,$calc($len($ifmatch) - 1))
}

on *:dialog:bb-url-catcher-config:sclick:30: {
  if ( $input(Do you want to reset all settings?,yq,Reset settings?) == $true ) {
    did -e bb-url-catcher-config 43,44,11,27
    did -c bb-url-catcher-config 200,46,29
    did -u bb-url-catcher-config 6,7,8,9,11,41,26,27,43,44,48
    did -r bb-url-catcher-config 4,18,50,60,70
    did -ra bb-url-catcher-config 32 16
    did -ra bb-url-catcher-config 33 30
    did -ra bb-url-catcher-config 34 40
    did -ra bb-url-catcher-config 35 400
    var %dummy = $bb-url-catcher-get-fav-folder
    if ( $isdir(%dummy) ) did -ra bb-url-catcher-config 201 %dummy
    else did -r bb-url-catcher-config 40
    did -ra bb-url-catcher-config 40 c:\
  }
}

on *:dialog:bb-url-catcher-config:sclick:47: {
  bb-url-catcher-remove-excludes
}

on *:dialog:bb-url-catcher-config:sclick:1: {

  if ( $did(bb-url-catcher-config,200).state == 0 ) set %bb-url-catcher-off 1
  else unset %bb-url-catcher-off
  if ( $did(bb-url-catcher-config,46).state == 0 ) set %bb-url-catcher-dupeoff 1
  else unset %bb-url-catcher-dupeoff
  if ( $did(bb-url-catcher-config,4) != $null ) {
    if ( $ifmatch isnum ) set %bb-url-catcher-max $did(bb-url-catcher-config,4)
    else {
      did -fr bb-url-catcher-config 4
      halt
    }
  }
  else unset %bb-url-catcher-max

  if ( $did(bb-url-catcher-config,18).lines != 0 ) set %bb-url-catcher-wildcards $didtok(bb-url-catcher-config,18,9)
  else unset %bb-url-catcher-wildcards

  if ( $did(bb-url-catcher-config,50).lines != 0 ) set %bb-url-catcher-excl-nicks $didtok(bb-url-catcher-config,50,9)
  else unset %bb-url-catcher-excl-nicks
  if ( $did(bb-url-catcher-config,60).lines != 0 ) set %bb-url-catcher-excl-chans $didtok(bb-url-catcher-config,60,9)
  else unset %bb-url-catcher-excl-chans
  if ( $did(bb-url-catcher-config,70).lines != 0 ) set %bb-url-catcher-excl-msgs $didtok(bb-url-catcher-config,70,9)
  else unset %bb-url-catcher-excl-msgs

  if ( $did(bb-url-catcher-config,201) != $null ) {
    if ( $isdir(" $+ $ifmatch $+ ") == $true ) set %bb-url-catcher-favorites $did(bb-url-catcher-config,201)
    else {
      did -fr bb-url-catcher-config 201
      halt
    }
  }
  else unset %bb-url-catcher-favorites
  if ( $did(bb-url-catcher-config,41).state != 0 ) set %bb-url-catcher-favsub 1
  else unset %bb-url-catcher-favsub
  if ( $did(bb-url-catcher-config,40) != $null ) {
    if ( $isdir(" $+ $ifmatch $+ ") == $true ) set %bb-url-catcher-htmls $did(bb-url-catcher-config,40)
    else {
      did -fr bb-url-catcher-config 40
      halt
    }
  }
  else unset %bb-url-catcher-htmls

  if ( $did(bb-url-catcher-config,32) != $null ) && ( $did(bb-url-catcher-config,33) != $null ) && ( $did(bb-url-catcher-config,34) != $null ) && ( $did(bb-url-catcher-config,35) != $null ) {
    if ( $+($did(bb-url-catcher-config,32),$did(bb-url-catcher-config,33),$did(bb-url-catcher-config,34),$did(bb-url-catcher-config,35)) isnum ) {
      set %bb-url-catcher-tabs $+($did(bb-url-catcher-config,32),$chr(44),$did(bb-url-catcher-config,33),$chr(44),$did(bb-url-catcher-config,34),$chr(44),$did(bb-url-catcher-config,35))
    }
    elseif ( !$did(bb-url-catcher-config,32) isnum ) {
      did -fr bb-url-catcher-config 32
      halt
    }
    elseif ( !$did(bb-url-catcher-config,33) isnum ) {
      did -fr bb-url-catcher-config 33
      halt
    }
    elseif ( !$did(bb-url-catcher-config,34) isnum ) {
      did -fr bb-url-catcher-config 34
      halt
    }
    elseif ( !$did(bb-url-catcher-config,35) isnum ) {
      did -fr bb-url-catcher-config 35
      halt
    }
  }
  else unset %bb-url-catcher-tabs

  if ( $did(bb-url-catcher-config,7).state == 1 ) set %bb-url-catcher-openstart 1
  else unset %bb-url-catcher-openstart
  if ( $did(bb-url-catcher-config,27).state == 1 ) set %bb-url-catcher-opendesktop 1
  else unset %bb-url-catcher-opendesktop
  if ( $did(bb-url-catcher-config,26).state == 1 ) set %bb-url-catcher-openontop 1
  else unset %bb-url-catcher-openontop
  if ( $did(bb-url-catcher-config,29).state == 1 ) set %bb-url-catcher-marknew 1
  else unset %bb-url-catcher-marknew

  if ( $did(bb-url-catcher-config,6).state == 1 ) set %bb-url-catcher-opencatch 1
  else unset %bb-url-catcher-opencatch
  if ( $did(bb-url-catcher-config,8).state == 1 ) set %bb-url-catcher-switchbar 1
  else unset %bb-url-catcher-switchbar
  if ( $did(bb-url-catcher-config,9).state == 1 ) set %bb-url-catcher-sound 1
  else unset %bb-url-catcher-sound
  if ( $did(bb-url-catcher-config,11).state == 1 ) set %bb-url-catcher-urlopen 1
  else unset %bb-url-catcher-urlopen

  if ( $did(bb-url-catcher-config,43).state == 1 ) set %bb-url-catcher-urlopennew 1
  else unset %bb-url-catcher-urlopennew
  if ( $did(bb-url-catcher-config,44).state == 1 ) set %bb-url-catcher-urlopenactivate 1
  else unset %bb-url-catcher-urlopenactivate
  if ( $did(bb-url-catcher-config,48).state == 1 ) set %bb-url-catcher-urlopenrun 1
  else unset %bb-url-catcher-urlopenrun
}

alias bb-url-catcher-convert {
  filter -ffte 1 9 " $+ $scriptdir $+ bb-urlcatcher.hst $+ " " $+ $scriptdir $+ bb-urlcatcher.hist $+ " *:*:*
  .remove " $+ $scriptdir $+ bb-urlcatcher.hst $+ "
  echo -ts URL-History converted...
}

alias bb-url-catcher-get-fav-folder {
  .comopen spe WScript.Shell
  var %r = $com(spe,SpecialFolders,3,bstr,Favorites)
  if (%r) %r = $com(spe).result
  .comclose spe
  return %r
}

alias bb-url-catcher-create-fav {
  if ( $isdir(" $+ %bb-url-catcher-favorites $+ ") == $true ) && ( $gettok($sline(@BB-URL-Catcher,1),4,9) != $null ) {
    var %dir %bb-url-catcher-favorites $+ $iif(%bb-url-catcher-favsub != $null,\ $+ $replace($iif($gettok($sline(@BB-URL-Catcher,1),2,9) isin private manual,$gettok($sline(@BB-URL-Catcher,1),3,9),$gettok($sline(@BB-URL-Catcher,1),2,9)),<,_,>,_,:,_,/,_,\,_,$chr(34),_,*,_,$chr(124),_,?,_))
    var %url $replace($gettok($sline(@BB-URL-Catcher,1),4,9),<,_,>,_,:,_,/,_,\,_,$chr(34),_,*,_,$chr(124),_,?,_) $+ .url
    if ( !$isdir(" $+ %dir $+ ") ) mkdir " $+ %dir $+ "
    if ( !$isdir(" $+ %dir $+ ") ) { echo -s Error on creacting Favorite-Folder, cannot make %dir $+. | halt }
    if ( $isfile(" $+ %dir $+ \ $+ %url $+ ") != $true ) {
      write " $+ %dir $+ \ $+ %url $+ " [DEFAULT]
      write " $+ %dir $+ \ $+ %url $+ " BASEURL= $+ $gettok($sline(@BB-URL-Catcher,1),4,9)
      write " $+ %dir $+ \ $+ %url $+ " [InternetShortcut]
      write " $+ %dir $+ \ $+ %url $+ " URL= $+ $gettok($sline(@BB-URL-Catcher,1),4,9)
      echo -ts Favorite for $gettok($sline(@BB-URL-Catcher,1),4,9) has been written.
    }
  }
  else echo -ts Error on creating Favorite, %bb-url-catcher-favorites doesnt exists!
}

alias bb-url-sendto-submenu {
  if ( $1 == begin ) {
    set %bb-url-sendto-submenu-servers $scon(0)
    unset %bb-url-sendto-submenu-chans %bb-url-sendto-submenu-query %bb-url-sendto-submenu-chats
  }
  if ( $1 == end ) {
    unset %bb-url-sendto-submenu-*
    return
  }
  if ( %bb-url-sendto-submenu-servers == 0 ) && ( %bb-url-sendto-submenu-chans == $null ) && ( %bb-url-sendto-submenu-querys == $null ) && ( %bb-url-sendto-submenu-chats == $null ) return
  if ( %bb-url-sendto-submenu-servers != 0 ) && ( %bb-url-sendto-submenu-chans == $null ) && ( %bb-url-sendto-submenu-querys == $null ) && ( %bb-url-sendto-submenu-chats == $null ) {
    scon %bb-url-sendto-submenu-servers
    dec %bb-url-sendto-submenu-servers
    set %bb-url-sendto-submenu-chans $chan(0)
    set %bb-url-sendto-submenu-querys $query(0)
    set %bb-url-sendto-submenu-chats $chat(0)
    return $iif($network,$ifmatch,$server) - $me $+ :bb-url-sendto $calc(%bb-url-sendto-submenu-servers + 1) server
  }
  if ( %bb-url-sendto-submenu-chans != 0 ) {
    scon $calc(%bb-url-sendto-submenu-servers + 1)
    dec %bb-url-sendto-submenu-chans
    return $chan($calc(%bb-url-sendto-submenu-chans + 1)) $+ :bb-url-sendto $calc(%bb-url-sendto-submenu-servers + 1) $chan($calc(%bb-url-sendto-submenu-chans + 1))
  }
  if ( %bb-url-sendto-submenu-querys != 0 ) {
    scon $calc(%bb-url-sendto-submenu-servers + 1)
    dec %bb-url-sendto-submenu-querys
    return $query($calc(%bb-url-sendto-submenu-querys + 1)) $+ :bb-url-sendto $calc(%bb-url-sendto-submenu-servers + 1) $query($calc(%bb-url-sendto-submenu-querys + 1))
  }
  if ( %bb-url-sendto-submenu-chats != 0 ) {
    scon $calc(%bb-url-sendto-submenu-servers + 1)
    dec %bb-url-sendto-submenu-chats
    return Chat $chat($calc(%bb-url-sendto-submenu-chats + 1)) $+ :bb-url-sendto $calc(%bb-url-sendto-submenu-servers + 1) = $+ $chat($calc(%bb-url-sendto-submenu-chats + 1))
  }
  if ( %bb-url-sendto-submenu-chans == 0 ) && ( %bb-url-sendto-submenu-querys == 0 ) && ( %bb-url-sendto-submenu-chats == 0 ) { unset %bb-url-sendto-submenu-chans %bb-url-sendto-submenu-querys %bb-url-sendto-submenu-chats | return - }
}

alias bb-url-sendto {
  if ( $1 isnum ) && ( $2- != $null ) {
    scon $1
    if ( $2 == server ) echo -s URL: $gettok($sline(@BB-URL-Catcher,1),4,9)
    else msg $2- $gettok($sline(@BB-URL-Catcher,1),4,9)
  }
}

alias bb-url-catcher-remove-excludes {
  if ( $isfile(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") == $true ) && ( $input(Do you want to remove excluded Item(s)?,yqsa,@BB-URL-Catcher,Remove URL(s)?) == $true ) {
    var %h = $numtok(%bb-url-catcher-excl-chans,9)
    while ( %h > 0 ) {
      filter -cxff " $+ $scriptdir $+ bb-urlcatcher.hist $+ " " $+ $scriptdir $+ bb-urlcatcher.hist $+ " $+(*,$chr(9),*,$gettok(%bb-url-catcher-excl-chans,%h,9),*,$chr(9),*,$chr(9),*,$chr(9),*)
      dec %h
    }
    var %h = $numtok(%bb-url-catcher-excl-nicks,9)
    while ( %h > 0 ) {
      filter -cxff " $+ $scriptdir $+ bb-urlcatcher.hist $+ " " $+ $scriptdir $+ bb-urlcatcher.hist $+ " $+(*,$chr(9),*,$chr(9),*,$gettok(%bb-url-catcher-excl-nicks,%h,9),*,$chr(9),*,$chr(9),*)
      dec %h
    }
    var %h = $numtok(%bb-url-catcher-excl-msgs,9)
    while ( %h > 0 ) {
      filter -cxff " $+ $scriptdir $+ bb-urlcatcher.hist $+ " " $+ $scriptdir $+ bb-urlcatcher.hist $+ " $+(*,$chr(9),*,$chr(9),*,$chr(9),*,$chr(9),*,$gettok(%bb-url-catcher-excl-msgs,%h,9),*)
      dec %h
    }
    if ( $window(@BB-URL-Catcher,0) != 0 ) bb-url-catcher-refreshwindow
  }
}

dialog -l newentry {
  title "BB-URL-Catcher - New Entry"
  size -1 -1 300 90
  option dbu
  icon $mircdir $+ mirc.exe, 12
  text "Date:", 100, 0 7 30 11, right
  edit "", 200, 35 5 50 11, read
  text "Channel:", 101, 86 7 25 11, right
  edit "", 201, 115 5 80 11, read
  text "Nick:", 102, 195 7 15 11, right
  edit "", 202, 215 5 80 11, read
  text "&URL:", 103, 0 23 30 11, right
  edit "", 203, 35 21 260 11
  text "&Message:", 104, 0 39 30 11, right
  edit "", 204, 35 37 260 33, multi
  button "&Cancel", 11, 260 74 35 11, cancel
  button "&Add", 10, 220 74 35 11, default ok
}

on *:dialog:bb-url-catcher-newentry:init:0: {
  did -a bb-url-catcher-newentry 200 $asctime(dd.mm. HH:nn:ss)
  did -a bb-url-catcher-newentry 201 manual
  did -a bb-url-catcher-newentry 202 $me
  did -a bb-url-catcher-newentry 204 Manual Entry
  did -f bb-url-catcher-newentry 203
}

on *:dialog:bb-url-catcher-newentry:sclick:10: {
  if ( $did(bb-url-catcher-newentry,203) == $null ) {
    did -f bb-url-catcher-newentry 203
    halt
  }
  ; Dupecheck and Replace
  if ( %bb-url-catcher-dupeoff == $null ) && ( $read(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ",w,* $+ $chr(9) $+ $did(bb-url-catcher-newentry,203) $+ $chr(9) $+ *,0) != $null ) {
    write -dl $readn " $+ $scriptdir $+ bb-urlcatcher.hist $+ "
    if ( $window(@BB-URL-Catcher,0) != 0 ) bb-url-catcher-refreshwindow
  }
  ; add to histfile
  set %bb-url-catcher-count $calc( %bb-url-catcher-count + 1 )
  set %bb-url-catcher-new $calc( %bb-url-catcher-new + 1 )
  write -il1 " $+ $scriptdir $+ bb-urlcatcher.hist $+ " $+($did(bb-url-catcher-newentry,200),$chr(9),$did(bb-url-catcher-newentry,201),$chr(9),$did(bb-url-catcher-newentry,202),$chr(9),$did(bb-url-catcher-newentry,203),$chr(9),$did(bb-url-catcher-newentry,204))
  ; check maxvalue an cut if its exceeded
  if ( %bb-url-catcher-max != $null ) && ( $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") > %bb-url-catcher-max ) {
    var %k = %bb-url-catcher-max
    while ( %k < $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") ) {
      write -d " $+ $scriptdir $+ bb-urlcatcher.hist $+ "
    }
  }
  ; insert in window when opened
  if ( $window(@BB-URL-Catcher,0) != 0 ) {
    iline $iif(%bb-url-catcher-marknew != $null,$color(high)) @BB-URL-Catcher 2 $+($did(bb-url-catcher-newentry,200),$chr(9),$did(bb-url-catcher-newentry,201),$chr(9),$did(bb-url-catcher-newentry,202),$chr(9),$did(bb-url-catcher-newentry,203),$chr(9),$did(bb-url-catcher-newentry,204))
    renwin @BB-URL-Catcher @BB-URL-Catcher - Entrys: $lines(" $+ $scriptdir $+ bb-urlcatcher.hist $+ ") - New: %bb-url-catcher-new - Overall: %bb-url-catcher-count
  }
  if ( %bb-url-catcher-opencatch != $null ) && ( $window(@BB-URL-Catcher,0) == 0 ) bb-url-catcher-window
  if ( %bb-url-catcher-switchbar != $null ) && ( $window(@BB-URL-Catcher,0) != 0 ) window -g2 @BB-URL-Catcher
  if ( %bb-url-catcher-sound != $null ) beep
  if ( %bb-url-catcher-urlopen != $null ) bb-url-catcher-urlopen $did(bb-url-catcher-newentry,203)
}



alias bb-url-catcher-exporthtml {
  if ( $isdir(" $+ %bb-url-catcher-htmls $+ ") ) && ( $input(Do you want to export $sline(@BB-URL-Catcher,0) Items to %bb-url-catcher-htmls $+ bb-url-catcher.html? This can take a while!,yqsa,@BB-URL-Catcher,Export?) == $true ) {
    write -c " $+ %bb-url-catcher-htmls  $+ bb-url-catcher.html $+ " <html><title>BB-URL-Catcher-History - $asctime(dd.mm.yy HH:nn:ss) - $sline(@BB-URL-Catcher,0) Items</title>
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1">
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <style type="text/css">
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <!--
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " body, blockquote, center, div, p, table, td, tr, h1, h2, h3, h4, h5, h6, li, ol, ul, form, option, select, br $+ $chr(123) $+  background-color:# $+ $gethexcolor($color($color(back))) $+ ;font-family: $+ $window(@BB-URL-Catcher).font $+ ;font-size: $+ $window(@BB-URL-Catcher).fontsize $+ px;color:# $+ $gethexcolor($color($color(listbox text))) $+ $iif($window(@BB-URL-Catcher).fontbold == $true,;font-weight:bold;) $+ $chr(125)
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " td $+ $chr(123) $+ vertical-align:top;padding-left:2px;padding-right:2px $+ $chr(125)
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " .tableheader $+($chr(123),color:#,$gethexcolor($color($color(back))),;background-color:#,$gethexcolor($color($color(highlight text))),$chr(125))
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " .td1 $+($chr(123),background-color:#,$gethexcolor($color($color(back))),$chr(125))
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " .td2 $+($chr(123),background-color:#,$gethexcolor($iif($color($color(back)) < 1052688,$calc($color($color(back)) + 1052688),$calc($color($color(back)) - 1052688))),$chr(125))
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " .small $+ $chr(123) $+ font-size: $+ $calc($window(@BB-URL-Catcher).fontsize - 2) $+ px $+ $chr(125)
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " -->
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " </style>
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <body>
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <table size="100%">
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),<tr class="tableheader">)
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),$chr(9),<td class="tableheader" nowrap><b>Date:</b></td><td class="tableheader" nowrap><b>Chan:</b></td><td class="tableheader" nowrap><b>Nick:</b></td><td class="tableheader" nowrap><b>URL:</b></td><td class="tableheader"><b>MSG:</b></td>)
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),</tr>)
    var %x = 1
    while ( %x <= $sline(@BB-URL-Catcher,0) ) {
      write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),<tr>)
      write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),$chr(9),<td class=",$iif($calc( %x % 2 ) == 0,td2,td1)," nowrap>,$gettok($sline(@BB-URL-Catcher,%x),1,9),</td><td class=" $+ $iif($calc( %x % 2 ) == 0,td2,td1)," nowrap>,$gettok($sline(@BB-URL-Catcher,%x),2,9),</td><td class=" $+ $iif($calc( %x % 2 ) == 0,td2,td1)," nowrap>,$gettok($sline(@BB-URL-Catcher,%x),3,9),</td>)
      write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+(<td class=" $+ $iif($calc( %x % 2 ) == 0,td2,td1),"><a href=",$gettok($sline(@BB-URL-Catcher,%x),4,9),">,$iif( $len($gettok($sline(@BB-URL-Catcher,%x),4,9)) > 45 ,$left($gettok($sline(@BB-URL-Catcher,%x),4,9),45) $+ ...,$gettok($sline(@BB-URL-Catcher,%x),4,9)),</a></td>)
      write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+(<td class=" $+ $iif($calc( %x % 2 ) == 0,td2,td1),">,$strip($gettok($sline(@BB-URL-Catcher,%x),5,9)),</td>)
      write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " $+($chr(9),</tr>)
      inc %x
    }
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " <tr><td colspan="5" align="right" class="small">Created by BB-URL-Catcher - $asctime(dd.mm.yy HH:nn:ss) $+ </td></tr>
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " </table>
    write " $+ %bb-url-catcher-htmls $+ bb-url-catcher.html $+ " </body></html>
  }
}

alias gethexcolor {
  return $base($and($1,255),10,16,2) $+ $base($int($calc(($1 - ( $int($calc($1 / (256 * 256))) *256*256))/256)),10,16,2) $+ $base($int($calc($1 / (256 * 256))),10,16,2)
}
