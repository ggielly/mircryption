;---------------------------------------------------------------------------
; mircryption updater v1.09.37
; some of this code is based on the lean and mean "Downloader v1.0 by |nsane" (www.mircscripts.org)
;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
; note:  If you use a web proxy, set the Alt-R -> Variables:
; %mcu_proxyip
; %mcu_proxyport
;---------------------------------------------------------------------------





;---------------------------------------------------------------------------
on *:load:{
  echo 7 -si2 MircryptionSuite - Mircryption Updater script is now installed (some code based on "Downloader v1.0 by |nsane"); use mircryption menu to check for updates.
}

alias mcu_about {
  %mcu_scriptversion = 1.09.37
  if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 MircryptionUpdater ver. %mcu_scriptversion loaded and ready. $+ $crlf
  else echo 7 -s MircryptionSuite - MircryptionUpdater ver. %mcu_scriptversion loaded and ready.
}
;---------------------------------------------------------------------------





;---------------------------------------------------------------------------
on 1:START: {
  ; let user know we are started
  .mcu_about

  ; set variables to defaults if they are not already set
  .setmcuvariables
}

; simple shortcut function that other scripts can use to check if mircryption is running.
; to check you would use this in your script:  if ($gotmircryption) ...
alias gotmircryptionupdater { return $true }

; signal used to tell all mircryption modules to show their version information
on *:SIGNAL:MircryptionInternalSignal_About: {
  .mcu_about
}
;---------------------------------------------------------------------------








;---------------------------------------------------------------------------
; Customizable parts - if you want to modify this script to work with some
;  other mirc project besides mircryption, you might have to modify some of
;  thse, but you shouldnt need to modify the rest of the script for common stuff.

alias McuUpdaterTitle {
  ; title for dialog
  return Mircryption Updater - for lazy mices
}

alias McuUpdaterDefaultFile {
  ; default update file
  return mircryption.update
}

alias McuUpdaterDefaultUrlBase {
  ; all downloads *MUST* be relative to this base - this is just for safety
  return http://mircryption.sourceforge.net/
}

alias McuFileUpdatesSignal {
  ; this is called every time a file ($1) has been sucessfully updated
}
alias McuUpdateVersionSignal {
  ; Send a signal for all files that could be updates to show their versions (usually in dialog)
  ; AND reupdate their script version info - this is very important, as a reload will
  ;  not re-run the :START: commands, so in order for a script to know it has been updated
  ;  it should, on this signal, reset its current version.
  ; This is only called once at end of all updates.
  .signal MircryptionInternalSignal_About
}
alias McuDllUpdatePreUnloadSignal {
  ; this is called before a dll ($1) is forced to unload in preparation for replacement
}
alias McuDllUpdatePostReplaceSignal {
  ; this is called after a dll ($1) has been replaced
  .setmcvariables 
}

alias McuTouchFile {
  ; touch the file to bring its creation date to specific value - we need to use a dll for this
  var %dllfilename
  if (%mc_scriptdll != $null) %dllfilename = %mc_scriptdll
  else if ($exists(mircryption.dll)) %dllfilename = mircryption
  else %dllfilename = mircryption/mircryption

  ;echo 4 -s DEBUGTEST: setting date of file $2- to $1 using dll %dllfilename
  .dll %dllfilename mc_touchfile $1 $2-
}

alias McuFiledates {
  ; the maintainer can use this to get file $ctime values to insert in .update file
  echo 5 -s Common File Dates
  echo 5 -s mircryption.dll -> $file(mircryption/mircryption.dll).ctime
  echo 5 -s MPgp.dll -> $file(mircryption/MPgp.dll).ctime
  echo 5 -s MircryptedFileViewer.exe -> $file(mircryption/MircryptedFileViewer.exe).ctime
  echo 5 -s MircryptionSuite.chm -> $file(MircryptionSuite.chm).ctime
  if ($1 != $null) echo 5 -s $1 -> $file($1).ctime
}
;---------------------------------------------------------------------------















;---------------------------------------------------------------------------
; Main update dialog
dialog mcupdater_down {
  title $McuUpdaterTitle
  ; "Mircryption Updater - for lazy mices"
  size -1 -1 300 142
  option dbu
  edit "speed", 1, 2 14 248 11, read autohs
  text "Progress:", 2, 2 28 247 9, center
  edit "Got size of size", 4, 2 2 248 11, read autohs
  button "Cancel", 3, 252 26 46 11, default 
  button "View File", 5, 252 14 46 11, disable
  button "Install File(s)", 6, 252 2 46 11, disable
  button "dummy", 9, 999 999 46 11, disable ok cancel
  edit "", 7, 2 40 296 100, read multi vsbar
}

on 1:dialog:mcupdater_down:close:*: {
  .mcupdater_dostopdownload
  .mcupdater_doclearvars
}
;---------------------------------------------------------------------------


















;---------------------------------------------------------------------------
alias mcupdate {
  var %filetoupdate = $1
  if (%filetoupdate == $null) %filetoupdate = $McuUpdaterDefaultFile

  if (%filetoupdate == $null) { echo -ta * Error: Usage /mcupdate filename (filename is always relative to $McuUpdaterDefaultUrlBase $+ ) | halt }

  ; parameter 2 is either confirm | $null | silent
  ; if set to confirm then the user is asked to click a button before downloading starts
  if ($2 == confirm) %mcu_temp_confirmdownload = $true
  else %mcu_temp_confirmdownload = $false

  ; we FORCE fileparam to be relative to $McuUpdaterDefaultUrlBase to prevent malicious use of this function
  %mcu_temp_fileparam = $McuUpdaterDefaultUrlBase $+ %filetoupdate

  set %mcu_temp_d.path $remove(%mcu_temp_fileparam,%mcu_temp_d.host,http://,ftp://))

  %mcu_temp_fullfilename = $scriptdir $+ updates\
  ; make update di if it dont exist
  if (!$exists(%mcu_temp_fullfilename)) .mkdir " $+ %mcu_temp_fullfilename $+ "
  %mcu_temp_fullfilename = %mcu_temp_fullfilename $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
  %mcu_temp_fullfilename = " $+ %mcu_temp_fullfilename $+ "

  if ( $exists( %mcu_temp_fullfilename) ) {
    var %hmm = $true
    ;set %hmm $input(File $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) already exists $+ $chr(44) $crlf $+ Do you want to overwrite it?,y)
    if (%hmm == $true) {
      ; erase existing file
      .remove -b %mcu_temp_fullfilename
      goto start
    }
    else { mcupdater_dostopdownload | halt }
  }

  ; set proxy info
  %mcu_temp_proxyinfo = $null
  if (%mcu_proxyip != $null) {
    %mcu_temp_proxyinfo = (using proxy %mcu_proxyip
    if (%mcu_proxyport != $null) %mcu_temp_proxyinfo = %mcu_temp_proxyinfo $+ : $+ %mcu_proxyport
    %mcu_temp_proxyinfo = %mcu_temp_proxyinfo $+ )
  }

  :start

  ; determine file type based on file name (either news | update | OTHER)
  %mcu_temp_filetype = $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,46),46)

  ; prepare variables
  set %mcu_temp_time.total $ctime
  set %mcu_temp_time $ctime
  if (http:// isin %mcu_temp_fileparam) { set %mcu_temp_d.host $gettok(%mcu_temp_fileparam,2,47) }
  else { set %mcu_temp_d.host $gettok(%mcu_temp_fileparam , 1, 47) } 
  set %mcu_temp_d.path $remove(%mcu_temp_fileparam,%mcu_temp_d.host,http://,ftp://))

  ; show initial dialog
  if ($dialog(mcupdater_down).hwnd == $null) {
    ; create the dialog
    ;dialog -m mcupdater_down mcupdater_down
    dialog -m mcupdater_down mcupdater_down
  }

  ; set fields
  did -ra mcupdater_down 5 View File
  did -ra mcupdater_down 4 %mcu_temp_fileparam
  did -ra mcupdater_down 1 Ready to download - click start to begin.
  did -b mcupdater_down 5
  did -b mcupdater_down 6
  did -ra mcupdater_down 2 $eval([,0) $str(:,100) $eval(],0)
  did -f mcupdater_down 3 

  ; show proxy info in title
  .dialog -t mcupdater_down $McuUpdaterTitle %mcu_temp_proxyinfo

  ; clear textbox area?
  if ( (%mcu_temp_filetype == news) || (%mcu_temp_filetype == update) ) {
    did -r mcupdater_down 7
  }

  ; are we on confirm mode
  if (%mcu_temp_confirmdownload) {
    ; dont start download right away
    did -e mcupdater_down 6
    did -fra mcupdater_down 6 Start Download
    did -ra mcupdater_down 3 Skip Download
  }
  else {
    ; start download right away
    .mcu_startdownload
  }
}

alias mcupdated {
  ; same as mcupdate but first set a flag that turns on some debugging info
  %mcu_temp_debugmode = $true
  .mcupdate $1 $2 $3 $4
}

alias mcu_startdownload {
  ; actually start the download

  ; update dialog
  %mcu_temp_confirmdownload = $false
  did -b mcupdater_down 6
  did -ra mcupdater_down 6 Install File(s)
  did -ra mcupdater_down 3 Don't Install

  var %sockreq = $remove(%mcu_temp_d.host,http://,ftp://)
  did -ra mcupdater_down 4 Please wait...
  if (%mcu_proxyip != $null) {
    if (%mcu_proxyport == $null) sockopen rep %mcu_proxyip 80
    else sockopen rep %mcu_proxyip %mcu_proxyport
  }
  else {
    ; normal open
    sockopen rep $remove(%mcu_temp_d.host,http://,ftp://) 80
  }

  ; update dialog
  did -fra mcupdater_down 3 Cancel
  did -ra mcupdater_down 1 Speed: calculating...

  ; start download timer
  .timer.mcuspeed 0 1 mcu_speedupdate
}
;---------------------------------------------------------------------------






;---------------------------------------------------------------------------
alias mcu_speedupdate {
  var %timeval1 = $ctime - %mcu_temp_time
  if (%timeval1 <= 0) return
  %timeval1 = %mcu_temp_rt / %timeval1
  %timeval1 = %timeval1 / 1024
  if (%timeval1 == $null) %timeval1 = nulltime with ctime = $ctime
  else %timeval1 = $round(%timeval1,1)
  did -ra mcupdater_down 1 Speed: %timeval1 kb/s
  %mcu_temp_rt = 0
  %mcu_temp_time = $ctime
}

alias mcu_sizeupdate {
  ;/echo readsize = %mcu_temp_read and fsize = %mcu_temp_fsize and rt = %mcu_temp_rt
  ; if we read everyhing, close socket which will trigger file finished
  if (%mcu_temp_rt >= %mcu_temp_fsize) {
    .sockclose rep
  }
}

alias mcu_filefinished {
  ;  if (%mcu_temp_read >= %mcu_temp_fsize) {
  ; file download complete
  ; echo IN FILEFINISH
  .timer.mcud off
  .timer.mcuspeed off
  did -ra mcupdater_down 2 $eval([,0) $str(|,100) $eval(],0)

  ; now fix up dialog based on the kind of file we just downloaded
  if (%mcu_temp_filetype == news) {
    did -ra mcupdater_down 3 Done
    ; automatically 'install' the news file into the script dir, so we know the user has seen it
    .mcu_doinstallfile
    ; now show it in the edit box
    .mcu_fileintoedit %mcu_temp_fullfilename
    ;set any variables after install
    .mcu_dosetmircvarval
  }
  else if (%mcu_temp_filetype == update) {
    .mcu_parseinstallscript %mcu_temp_fullfilename
    if (%mcu_temp_updatequeue_pending == $null) {
      ; no files to update
      did -fra mcupdater_down 3 Done
    }
    else {
      ; update dialog
      did -e mcupdater_down 6
      did -fra mcupdater_down 6 Begin Update
      did -ra mcupdater_down 3 Cancel Update
    }
  }
  else {
    did -e mcupdater_down 5
    did -fe mcupdater_down 6
    did -ra mcupdater_down 3 Don't Install
    did -a mcupdater_down 1 .  --> [Push button to install]
  }
}
;---------------------------------------------------------------------------











;---------------------------------------------------------------------------
on *:SOCKOPEN:rep:{
  ; socket open event
  set %mcu_temp_d.check 0
  unset %mcu_temp_read

  if ($sock($sockname).wserr != 0) {
    ; there was an error
    did -ra mcupdater_down 1 $sock($sockname).wsmsg
    did -ra mcupdater_down 4 Error (if you use a web proxy, set % $+ mcu_proxyip and % $+ mcu_proxyport in your alt+R variables).
    ; should we stop download or let it keep trying? - we need to stop it if we dont want the speed timers to overwrite our error message
    .mcupdater_dostopdownload
    return
  }

  if (%mcu_proxyip != $null) {
    sockwrite -n $sockname GET %mcu_temp_fileparam HTTP/1.1
    sockwrite -n $sockname Connection: keep-alive
    sockwrite -n $sockname Host: %mcu_temp_d.host
    sockwrite $sockname $crlf
  }
  else {
    sockwrite -n $sockname GET %mcu_temp_d.path HTTP/1.1
    sockwrite -n $sockname Connection: keep-alive
    sockwrite -n $sockname Host: %mcu_temp_d.host
    sockwrite $sockname $crlf
  }
}


on *:SOCKREAD:rep: { 
  ; socket read event
  if (!$dialog(mcupdater_down)) { dialog -m mcupdater_down mcupdater_down }
  if (%mcu_temp_d.check == 0) {
    sockread %mcu_temp_dat
    if (*HTTP/* 404 Not Found* iswm %mcu_temp_dat) {
      %mcu_temp_rt = 0
      sockclose rep
      did -ra mcupdater_down 1 File not found!
      did -ra mcupdater_down 4 Error
      .mcupdater_dostopdownload
    }
    if (content-length isin %mcu_temp_dat) {
      set %mcu_temp_fsize $remove($gettok(%mcu_temp_dat,2,58),$chr(32))
      ;echo DEBUG read filesize as %mcu_temp_fsize
      ;echo TEMP SIZE is %mcu_temp_fsize
    }
    if ( ($mid(%mcu_temp_dat,1,1) == $chr(47)) && ($mid(%mcu_temp_dat,2,1) == $chr(47)) ) {
      set %mcu_temp_d.check 1
      ; the first line should be a //
      %mcu_temp_fsize = %mcu_temp_fsize - $len(%mcu_temp_dat)
      %mcu_temp_fsize = %mcu_temp_fsize - 2
      ;echo TEMP SIZE is %mcu_temp_fsize
    }
    if ($len(%mcu_temp_dat) < 4) {
      set %mcu_temp_d.check 1
    }
  }
  else if (%mcu_temp_d.check == 1) {
    .timer.mcud -m 0 200 mcu_sizeupdate
    ; dialog -t mcupdater_down Downloaded $round($calc(%mcu_temp_read / %mcu_temp_fsize * 100),-1) $+ % of " $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    sockread &tmp

    var %socklen = $bvar(&tmp,0)
    ;/echo read a socked size of $sockbr and binlen = %socklen
    if ($sockbr == $null) return

    if (%mcu_temp_read == $null) %mcu_temp_read = 0
    if (%mcu_temp_rt == $null) %mcu_temp_rt = 0
    %mcu_temp_read = %mcu_temp_read + $sockbr
    %mcu_temp_rt = %mcu_temp_rt + $sockbr

    ;/echo setting mcu_temp_rt to %mcu_temp_rt

    did -ra mcupdater_down 4 $round($calc(%mcu_temp_read / 1024),-1) kb / $round($CALC(%mcu_temp_fsize / 1024),-1) kb -- %mcu_temp_fileparam
    set %mcu_temp_percents $int($calc(%mcu_temp_read / %mcu_temp_fsize * 100))

    ; Progress bar
    ;    did -ra mcupdater_down 2 $eval([,0) $str(|,$int($calc(%mcu_temp_percents / 2.5 ))) $+ $str($chr(58),$calc(40 - $int($calc(%mcu_temp_percents / 2.5)))) $eval(],0)
    did -ra mcupdater_down 2 $eval([,0) $str(|,$int($calc(%mcu_temp_percents / 1 ))) $+ $str($chr(58),$calc(100 - $int($calc(%mcu_temp_percents / 1)))) $eval(],0)

    ; write some data
    bwrite %mcu_temp_fullfilename -1 &tmp

    if (%mcu_temp_rt >= %mcu_temp_fsize) {
      ;    if (%mcu_temp_read >= %mcu_temp_fsize) {
      ; file has completed download
      ; dialog -t mcupdater_down Downloaded " $+ %mcu_temp_fileparam $+ "
      did -ra mcupdater_down 4 $round($CALC(%mcu_temp_fsize / 1024),-1) kb -- %mcu_temp_fileparam
      did -ra mcupdater_down 1 Download Complete.  Average Speed: $round($calc($round($calc(%mcu_temp_fsize / ($ctime - %mcu_temp_time.total ))) / 1024),1) KB/s.  Time: $duration($calc($ctime - %mcu_temp_time.total))
      .timer.mcuspeed off
    }
  }
}


on *:SOCKCLOSE:rep:{
  ; socket close
  ;echo DEBUG: got a socket close from remote
  did -ra mcupdater_down 4 $round($CALC(%mcu_temp_fsize / 1024),-1) kb -- %mcu_temp_fileparam
  did -ra mcupdater_down 1 Download Complete.  Average Speed: $round($calc($round($calc(%mcu_temp_fsize / ($ctime - %mcu_temp_time.total ))) / 1024),1) KB/s.  Time: $duration($calc($ctime - %mcu_temp_time.total))
  .timer.mcuspeed off

  ;echo total size read is %mcu_temp_rt

  if ($sock($sockname).wserr != 0) {
    ; there was an error
    did -ra mcupdater_down 1 $sock($sockname).wsmsg
    did -ra mcupdater_down 4 Error (if you use a web proxy, set % $+ mcu_proxyip and % $+ mcu_proxyport in your alt+R variables).
    ; should we stop download or let it keep trying? - we need to stop it if we dont want the speed timers to overwrite our error message
    .mcupdater_dostopdownload
    return
  }

  if (%mcu_temp_rt == $null) return
  if (%mcu_temp_rt == 0) return

  ; file finished properly
  .mcu_filefinished
}

;---------------------------------------------------------------------------








;---------------------------------------------------------------------------
on *:dialog:mcupdater_down:sclick:3:{
  ; click on Cancel

  ; now process next in queue if one is waiting
  .mcupdater_doclose force
}


on *:dialog:mcupdater_down:sclick:5:{
  ; click on View File
  var %fileextension = $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,46),46)
  if ((%fileextension == txt) || (%fileextension == mrc) ) {
    ; find a good viewer by checking some known locations
    var %viewerapp
    %viewerapp = "C:\Program Files\UltraEdit\UEDIT32.EXE"
    if (!$exists(%viewerapp)) %viewerapp = "C:\Program Files\Windows NT\Accessories\wordpad.exe"
    if (!$exists(%viewerapp)) %viewerapp = "C:\Program Files\accessories\wordpad.exe"
    ; launch viewer
    if ($exists(%viewerapp)) run %viewerapp %mcu_temp_fullfilename
    else run %mcu_temp_fullfilename
  }
  else {
    run %mcu_temp_fullfilename
  }
}


on *:dialog:mcupdater_down:sclick:6:{
  ; click on Install or "start download"

  if (%mcu_temp_confirmdownload) {
    ; this is the start download trigger so just start download - its not install button
    .mcu_startdownload
    return
  }

  ; now fix up dialog based on the kind of file we just downloaded
  if (%mcu_temp_filetype == news) {
    ; we shouldnt get here
  }
  else if (%mcu_temp_filetype == update) {
    ; installing a script requires special work, done on a timer
    .mcu_doinstallscript %mcu_temp_fullfilename
    ;set any variables after install
    .mcu_dosetmircvarval
  }
  else {
    ; install the file
    .mcu_doinstallfile
  }

  ; now process next in queue if one is waiting
  .mcupdater_doclose
}
;---------------------------------------------------------------------------








;---------------------------------------------------------------------------
alias mcu_parseinstallscript {
  ; install a script

  ; reset %mcu_temp_updatequeue to files to update
  %mcu_temp_updatequeue = $null
  %mcu_temp_updatequeue_pending = $null

  var %field_filename = $null
  var %field_label = $null
  var %field_exclusive = $null
  var %field_mircversionvar = $null
  var %field_setmircvar = $null
  var %field_requiredfile = $null
  var %field_installnoversion = $null
  var %field_installversion = $null
  var %field_filetodate = $null
  var %field_installnofile = $null
  var %field_notes_install = $null
  var %field_notes_noinstall = $null

  var %emptyeditbox = $true
  var %dontlineeditbox = $true
  var %fieldname = $null
  var %fieldval = $null
  var %skipentry = $false
  var %installthisentry = $false
  var %newavailable = $false
  var %fversion = $null
  var %fileupdatecount = 0
  var %iline
  var %count = 1
  var %totalcount = $lines($1-)

  while (%count <= %totalcount) {
    %iline = $read($1-,n,%count)

    if (($left(%iline,2) != //) && (%iline != $crlf) && (%iline != $null) ) {
      ; process this non comment
      if (%iline == $chr(64) $+ ENDENTRY) {
        ; end of a main entry, so reset our skipentry flag
        ;did -a mcupdater_down 7 DEBUG PROCESSED: %field_filename
        %skipentry = $false
        ; show any install notes
        if ((%field_notes_install != $null) && (%installthisentry == $true) ) did -a mcupdater_down 7 %field_notes_install $crlf
        if ((%field_notes_noinstall != $null) && (%installthisentry != $true) ) {
          if (!%emptyeditbox) did -a mcupdater_down 7 ------------------------------------------------ $+ $crlf
          did -a mcupdater_down 7 %field_notes_noinstall $crlf
          %emptyeditbox = $false
        }

        ; update news variable immediately after showing it
        if ((%field_filename == info) && (%field_setmircvar != $null) && (%installthisentry == $true)) {
          ;for news, we set any setmircvar immediately after showing it
          .set [ [ %field_setmircvar ] ] %field_installversion
          %field_setmircvar = $null
        }

        ;exclusive install?
        if ((%field_exclusive == $true) && (%installthisentry == $true) ) {
          ; since we are installing an exclusive update, we now stop.
          .break
        }

        %count = %count + 1
        .continue
      }
      else if (%iline == $chr(64) $+ END) {
        ; stop parsing file
        %skipentry = $false
        .break
      }
      else if (%skipentry) {
        ; we are skipping this entry
        %count = %count + 1
        .continue
      }

      if ($left(%iline,1) == $chr(64)) {
        ; its a new main entry
        %skipentry = $false
        %installthisentry = $false
        %field_filename = $right(%iline,$calc($len(%ilen) - 1))
        ; clear other fields
        %field_label = $null
        %field_exclusive = $null
        %field_mircversionvar = $null
        %field_setmircvar = $null
        %field_requiredfile = $null
        %field_installnoversion = $null
        %field_installversion = $null
        %field_filetodate = $null
        %field_installnofile = $null
        %field_notes_install = $null
        %field_notes_noinstall = $null
      }
      else if ($left(%iline,1) == $chr(36)) {
        ; field value $
        %fieldname = $right(%iline,$calc($len(%ilen) - 1))
        %fieldval = $gettok(%fieldname,2-,32)
        %fieldname = $deltok(%fieldname,2-,32)
        if (%fieldname == LABEL) {
          %field_label = %fieldval
        }
        else if (%fieldname == MIRCVERSIONVAR) {
          %field_mircversionvar = %fieldval
        }
        else if (%fieldname == EXCLUSIVE) {
          %field_exclusive = $true
        }
        else if (%fieldname == SETMIRCVAR) {
          %field_setmircvar = %fieldval
        }
        else if (%fieldname == FILETODATE) {
          %field_filetodate = %fieldval
        }
        else if (%fieldname == REQUIREDFILE) {
          %field_requiredfile = %fieldval
        }
        else if (%fieldname == INSTALLNOVERSION) {
          %field_installnoversion = $true
        }
        else if (%fieldname == INSTALLNOFILE) {
          %field_installnofile = $true
        }
        else if (%fieldname == NOTESINSTALL) {
          %field_notes_install = %fieldval
        }
        else if (%fieldname == NOTESNOINSTALL) {
          %field_notes_noinstall = %fieldval
        }
        else {
          ; unknown field - ask user to contact author
          did -a mcupdater_down 7 ATTENTION -> The updater script encountered an unknown field in the update file ( $+ %fieldname $+ ).  Please let update maintainer know about this bug. $+ $crlf
        }
      }
      else if ($left(%iline,1) == $chr(35)) {
        ; version #
        ; we set %installthisentry and add the file to install queue if there is a new file version installed
        %fversion = $right(%iline,$calc($len(%ilen) - 1))
        %newavailable = $mcu_compareversion(%field_filename,%fversion,%field_mircversionvar,%field_requiredfile,%field_filetodate)
        if ( (%newavailable == missing) && ((%field_installnoversion == $true) || (%field_installnofile == $true)) ) %newavailable = $true
        if (%newavailable == $true) {
          %dontlineeditbox = $false
          if (!%installthisentry) {
            ; ok set this file up to be downloaded
            %installthisentry = $true
            %field_installversion = %fversion
            if (!%emptyeditbox) did -a mcupdater_down 7 ------------------------------------------------ $+ $crlf

            if (%field_filename == info) {
              if (%field_label != $null) did -a mcupdater_down 7 %field_label $crlf
              else did -a mcupdater_down 7 Information $crlf
            }
            else {
              ; add this file to end of our list of files to install
              %fileupdatecount = %fileupdatecount + 1
              if (%mcu_temp_updatequeue_pending != $null) %mcu_temp_updatequeue_pending = %mcu_temp_updatequeue_pending $+ :
              if (%field_setmircvar != $null) {
                ; slightly tricky -> update file can ask us to specifically set a variable after installing, so we add this (!var = val) to the file, to be parsed laster
                %mcu_temp_updatequeue_pending = %mcu_temp_updatequeue_pending $+ %field_filename $+ $chr(33) $+ %field_setmircvar $+ $chr(61) $+ %field_installversion
              }
              else if (%field_filetodate != $null) {
                ; we are going to have to set the downloaded filetime
                ;  .dll mircryption/mircryption.dll mc_touchfile $ctime mircryption/MircryptedFileViewer.exe
                %mcu_temp_updatequeue_pending = %mcu_temp_updatequeue_pending $+ %field_filename $+ $chr(33) $+ - $+ $chr(61) $+ %field_installversion
              }
              else {
                %mcu_temp_updatequeue_pending = %mcu_temp_updatequeue_pending $+ %field_filename
              }
              if (%field_label != $null) did -a mcupdater_down 7 NEW VERSION AVAILABLE: %field_label
              else did -a mcupdater_down 7 NEW VERSION AVAILABLE: %field_filename
              if (%field_mircversionvar != $null) {
                var %mircversionvarval = [ [ %field_mircversionvar ] ]
                if (%mircversionvarval != $null) did -a mcupdater_down 7 $crlf $+ YOUR CURRENT VERSION: %mircversionvarval
              }
              else {
                ; try to grab current file date
                var %filetodate
                if ($exists($scriptdir $+ %field_filename)) %filetodate = $scriptdir $+ %field_filename
                else if ($exists($mircdir $+ %field_filename)) %filetodate = $mircdir $+ %field_filename
                if (!$exists(%filetodate)) did -a mcupdater_down 7 $crlf $+ YOU DO NOT YET HAVE THIS FILE INSTALLED.
                else {
                  ; be nice and show them their current file date since we cant detect their version
                  var %mircversionvarval = $file(%filetodate).ctime
                  did -a mcupdater_down 7 $crlf $+ YOUR CURRENT VERSION: $asctime(%mircversionvarval)
                }
              }
              did -a mcupdater_down 7 $crlf
            }
          }
          if (%field_filename != info) {
            if ( ($pos(%fversion,.,1) != $null) || (%field_filetodate == $null) ) did -a mcupdater_down 7 => Changes in version %fversion $+ : $+ $crlf
            else did -a mcupdater_down 7 => Changes as of $asctime(%fversion) $+ : $+ $crlf
          }
          ;set flag saying the editbox is not empty
          %emptyeditbox = $false
        }
        else {
          ; skip everything from here down to end of entry, as this is all older stuff
          %skipentry = $true
          if ((%installthisentry != $true) && (%mcu_temp_debugmode == $true)) {
            if ((!%emptyeditbox) && (%dontlineeditbox != $true)) did -a mcupdater_down 7 ------------------------------------------------ $+ $crlf
            did -a mcupdater_down 7 [-] %field_filename - no update needed. $+ $crlf
            %emptyeditbox = $false
            %dontlineeditbox = $true
          }
        }
      }
      else {
        ; should be version description
        did -a mcupdater_down 7 %iline $+ $crlf
      }
    }
    %count = %count + 1
  }

  if (%fileupdatecount == 0) {
    did -a mcupdater_down 7 ================================================ $+ $crlf
    did -a mcupdater_down 7 There are no files that need updating. $+ $crlf
    did -a mcupdater_down 7 ================================================ $+ $crlf
    ; do an about to show current versions
    .McuUpdateVersionSignal
  }
  else {
    did -a mcupdater_down 7 ================================================ $+ $crlf
    did -a mcupdater_down 7 %fileupdatecount file(s) need updating. $+ $crlf
    if ((%field_exclusive == $true) && (%installthisentry == $true) ) {
      did -a mcupdater_down 7 NOTE: Because you are replacing the updater script itself, please re-run the updater after installing this file to check for further updates. $+ $crlf
    }
  }

  ; now go to top
  did -c mcupdater_down 7 1 
}


alias mcu_doinstallscript {
  ; fill the %mcu_temp_updatequeue with the parsed files to update
  %mcu_temp_updatequeue = %mcu_temp_updatequeue_pending
}


alias mcu_dosetmircvarval {
  ; anupdate script can ask us to set some values to be performed after a component install

  if (%mcu_temp_setmircvar != $null) {
    var %varval = $gettok(%mcu_temp_setmircvar,2-,61)
    %mcu_temp_setmircvar = $deltok(%mcu_temp_setmircvar,2-,61)

    if (%mcu_temp_setmircvar == -) {
      if (%mcu_temp_savedfilename != $null) {
        var %nlen = $len(%mcu_temp_savedfilename)
        %nlen = %nlen - 2
        %mcu_temp_savedfilename = $mid(%mcu_temp_savedfilename,2,%nlen);
        ; echo now touch the file to update its date
        McuTouchFile %varval %mcu_temp_savedfilename
      }
    }
    else {
      ;echo 4 -s DEBUGTEST: setting %mcu_temp_setmircvar to %varval
      .set [ [ %mcu_temp_setmircvar ] ] %varval
    }
  }
  %mcu_temp_setmircvar = $null
  %mcu_temp_savedfilename = $null
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
alias mcu_compareversion {
  ; arguments are %fname,%fversion,%field_mircversionvar,%field_requiredfile,%field_filetodate
  ; return true if %fversion is more recent

  ; echo 4 -s in mcu_compareversion $1 $2 $3 $4 $5

  var %fname = $1
  var %fversion = $2
  var %field_mircversionvar = $3
  var %field_requiredfile = $4
  var %field_filetodate = $5

  if (%fversion == xxx) {
    ; this version string means always update
    return $true
  }

  if (%field_requiredfile != $null) {
    ; first step is make sure the specified file exists, if not return $false
    if ( (!$exists(%field_requiredfile)) && (!$exists($scriptdir $+ %field_requiredfile)) && (!$exists($mircdir $+ %field_requiredfile)) ) return $false
  }

  if (%field_mircversionvar != $null) {
    ; check the mirc version var val
    ; ATTN: we might want to add a check here to make sure we are not being passed a malicious string to run, since we are EVALUATIONG it - but this could only happen if an evildoer does something bad
    var %mircversionvarval = [ [ %field_mircversionvar ] ]
    ; Uncomment this if you want to 
    ;echo 4 -s Evaluating %fname current version variable %field_mircversionvar = %mircversionvarval
    ; now what if we dont find a version?? - we return: missing
    if (%mircversionvarval == $null) return missing
    return $mcu_newerversion(%fversion,%mircversionvarval)
  }

  if (%field_filetodate != $null) {
    ; update script wants to check file dates of a specific file
    ; now what if we dont find the file? - we return: missing
    if ($exists($scriptdir $+ %field_filetodate)) %field_filetodate = $scriptdir $+ %field_filetodate
    else if ($exists($mircdir $+ %field_filetodate)) %field_filetodate = $mircdir $+ %field_filetodate
    if (!$exists(%field_filetodate)) return missing
    var %mircversionvarval = $file(%field_filetodate).ctime
    if (%mircversionvarval < %fversion) return $true
    return $false
  }

  return $true
}



alias mcu_newerversion {
  ; args are %webversion,%currentversion
  var %webversion = $1
  var %currentversion = $2
  var %webversion_word = $null
  var %currentversion_word = $null

  ;echo 4 -s comparing webversion %webversion vs current version %currentversion

  ; we expect these version strings to be of form numbers separated by dots, ie #.# or #.#.# or #.#.#.#, and we stop on shorted string
  while ((%webversion != $null) && (%currentversion != $null)) {
    ; pick off leftmost word of both versionstrings
    %webversion_word = $gettok(%webversion,1,46)
    %webversion = $deltok(%webversion,1,46)
    %currentversion_word = $gettok(%currentversion ,1,46)
    %currentversion = $deltok(%currentversion ,1,46)
    ;echo DEBUGTEST compareversion webv = %webversion_word vs. curv = %currentversion_word
    if ( %currentversion_word isnum ) {
      if (%webversion_word < %currentversion_word) return $false
      if (%webversion_word > %currentversion_word) return $true
    }
    else return $true
  }

  if (%webversion != $null) return $true
  return $false
}
;---------------------------------------------------------------------------



;---------------------------------------------------------------------------
alias mcu_doinstallfile {
  ; Install a file - this should be cleaned up and redundant code merged
  var %oldfilename
  var %bakfilename
  var %isnewfile = $false
  var %mrcfilename
  var %basefilename

  var %fileextension = $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,46),46)

  ; NOTE -> we should merge these below

  if (%fileextension == mrc) {
    ; ----------- MRC FILE EXTENSION
    ; IF this file is a .mrc file then do:

    ; here we want to first BACKUP current file
    %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    if (!$exists(%oldfilename)) {
      ; old filename not found in script dir - maybe it's in mirc directory instead (ie help file)
      %oldfilename = " $+ $mircdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "    
    }
    if (!$exists(%oldfilename)) %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    if ($exists(%oldfilename)) {
      %bakfilename = " $+ $scriptdir $+ updates\ $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ .bak $+ "
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Updating %oldfilename (backup placed in updates subdirectory). $+ $crlf
      else echo 4 -s Updating %oldfilename (backup placed in updates subdirectory).
      ; echo backup file is %bakfilename
      .copy -o %oldfilename %bakfilename
    }
    else {
      ; existing file not found - should we consider this an error? no, we dont
      ;if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Update aborted: File %oldfilename not found. $+ $crlf
      ;else echo 4 -s Update aborted: File %oldfilename not found.
      ;halt
      %isnewfile = $true
    }
    ; then replace old one with one we just downloaded
    ; we can EITHER force complete unload then load, or just reload
    if ($exists(%mcu_temp_fullfilename)) {
      ; overwrite it
      .copy -o %mcu_temp_fullfilename %oldfilename
      %mcu_temp_savedfilename = %oldfilename
      ;set any variables after install
      .mcu_dosetmircvarval
      did -a mcupdater_down 7 Update succeeded. $+ $crlf
      ; send signal that file has updated, in case we want to do anything special
      .McuFileUpdatesSignal %oldfilename

      ; do a soft reload
      ; if we are updating ourself we need some special steps
      if ($gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) == $gettok($script,$numtok($script,92),92)) {
        ; we are updating OURSELVES!!!
        ; terminate upgrade and close dialog since we will be unloading ourselves - and do a delayed show of result
        var %sname = $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
        echo 4 -s Upgrade of [ %sname ] complete.
        ; if we only run the mcu_about then it forgets we have updated other files
        ; .timer_delayedabout1 1 0 .reload -rs [ %oldfilename ] | .mcu_about
        .timer_delayedabout1 1 0 .reload -rs [ %oldfilename ] | .McuUpdateVersionSignal
        .dialog -x mcupdater_down
        .mcupdater_doclearvars
        halt
      }
      else {
        if (%isnewfile) .load -rs %oldfilename
        else .reload -rs %oldfilename
        ; we used to do a version signal here, but now we do it on completion
      }
    }
    else {
      ; new file not found - this is error
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Update aborted: Downloaded file %fullfilename not found. $+ $crlf
      else echo 4 -s Update aborted: Downloaded file %fullfilename not found.
      halt
    }
  }
  else if (%fileextension == dll) {
    ; ----------- MRC FILE EXTENSION
    ; IF this file is a .dll file then do:

    ; here we want to first BACKUP current file
    %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    if (!$exists(%oldfilename)) {
      ; old filename not found in script dir - maybe it's in mirc directory instead (ie help file)
      %oldfilename = " $+ $mircdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "    
    }
    if (!$exists(%oldfilename)) %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    if ($exists(%oldfilename)) {
      %bakfilename = " $+ $scriptdir $+ updates\ $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ .bak $+ "
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Updating %oldfilename (backup placed in updates subdirectory). $+ $crlf
      else echo 4 -s Updating %oldfilename (backup placed in updates subdirectory).
      ; echo backup file is %bakfilename
      .copy -o %oldfilename %bakfilename
    }
    else {
      ; existing file not found - should we considered an error? no
      ;if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Update aborted: File %oldfilename not found. $+ $crlf
      ;else echo 4 -s Update aborted: File %oldfilename not found.
      ;halt
      %isnewfile = $true
    }
    ; then replace old one with one we just downloaded
    ; but we made need to unload the mrc before we can replace the dll
    if ($exists(%mcu_temp_fullfilename)) {
      %basefilename = $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
      if (!$exists(%basefilename)) {
        ; old filename not found in script dir - maybe it's in mirc directory instead (ie help file)
        %basefilename = $mircdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
      }

      var %basefilenamelen = $len(%basefilename) - $len(%fileextension)
      %basefilenamelen = %basefilenamelen - 1
      %basefilename = $left(%basefilename ,%basefilenamelen)
      %mrcfilename = " $+ %basefilename $+ .mrc $+ "

      ; always explicitly unload the dll (we try several unloads, no harm) before we try to overwrite it
      var %shortdllname = $gettok(%basefilename,$numtok(%basefilename,92),92)

      ; call any special post-dll replacement code
      .McuDllUpdatePreUnloadSignal %shortdllname

      ;unload the dll
      .dll -u %shortdllname

      ; the above unload should do it, but we try some others also, since there is no harm in it
      .dll -u %basefilename
      .dll -u %mcu_temp_fullfilename
      .dll -u $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
      .dll -u $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)
      .dll -u $mircdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47)

      ;test to list running dlls
      ;echo loaded dll one is $dll(1)
      ;echo loaded dll two is $dll(2)
      ;echo loaded dll three is $dll(3)

      ; we have turned off this function, which forces scripts of same name to unload and reload when replacing a dll
      var %forcescriptunload = $false
      if ((%forcescriptunload) && ($exists(%mrcfilename))) {
        if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 About to unload and then reload a .mrc file in order to install the dll update; if this fails, exit mirc and copy the dll from the updates dir to your mircyrption dir and restart mirc. $+ $crlf
        else echo 4 -s About to unload and then reload a .mrc file in order to install the dll update; if this fails, exit mirc and copy the dll from the updates dir to your mircyrption dir and restart mirc.

        ; unload existing mrc file?
        %mc_silentupdate = $true
        .unload -rs %mrcfilename

        ; overwrite the dll
        .copy -o %mcu_temp_fullfilename %oldfilename
        %mcu_temp_savedfilename = %oldfilename
        ;set any variables after install
        .mcu_dosetmircvarval

        ; load back the script
        .timer.delayeddllload 1 0 .load -rs [ %mrcfilename ] | .unset [ %mc_silentupdate ]
        ;did -a mcupdater_down 7 Updated --> $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ $crlf
        did -a mcupdater_down 7 Update succeeded. $+ $crlf
        ; send signal that file has updated, in case we want to do anything special
        .McuFileUpdatesSignal %oldfilename
      }
      else {
        ; overwrite it
        .copy -o %mcu_temp_fullfilename %oldfilename
        %mcu_temp_savedfilename = %oldfilename
        ;set any variables after install
        .mcu_dosetmircvarval
        did -a mcupdater_down 7 Update succeeded. $+ $crlf
        ; send signal that file has updated, in case we want to do anything special
        .McuFileUpdatesSignal %oldfilename
      }
      ; call any special post-dll replacement code
      .McuDllUpdatePostReplaceSignal %shortdllname
    }
    else {
      ; new file not found - this is error
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Update aborted: Downloaded file %fullfilename not found. $+ $crlf
      else echo 4 -s Update aborted: Downloaded file %fullfilename not found.
      halt
    }
  }
  else {
    ; ----------- OTHER FILE EXTENSIONS
    ; other files just get replaced
    ; here we want to first BACKUP current file
    %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "
    if (!$exists(%oldfilename)) {
      ; old filename not found in script dir - maybe it's in mirc directory instead (ie help file)
      %oldfilename = " $+ $mircdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "    
    }
    ; if its not in mirc dir, go back to script dir
    if (!$exists(%oldfilename)) %oldfilename = " $+ $scriptdir $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ "

    if ((%fileextension == exe) && (!$exists(%oldfilename)) ) {
      ; ELSE if this file is a nonexistent .exe file then execute the file
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Launching update file $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ . $+ $crlf
      else echo 4 -s Launching update file $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ .
      run %mcu_temp_fullfilename
      ;set any variables after install
      .mcu_dosetmircvarval
    }
    else {
      ; just install the file
      if ($exists(%oldfilename)) {
        %bakfilename = " $+ $scriptdir $+ updates\ $+ $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ .bak $+ "
        if ((%mcu_temp_filetype != news) && (%mcu_temp_filetype != update)) {
          if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Updating %oldfilename (backup placed in updates subdirectory). $+ $crlf
          else echo 4 -s Updating %oldfilename (backup placed in updates subdirectory).
        }
        ; echo backup file is %bakfilename
        .copy -o %oldfilename %bakfilename
      }
      ; just install the file
      if ($exists(%mcu_temp_fullfilename)) {
        ; overwrite it
        .copy -o %mcu_temp_fullfilename %oldfilename
        %mcu_temp_savedfilename = %oldfilename
        ;set any variables after install
        .mcu_dosetmircvarval
        if ((%mcu_temp_filetype != news) && (%mcu_temp_filetype != update)) {
          ;did -a mcupdater_down 7 Updated --> $gettok(%mcu_temp_d.path,$numtok(%mcu_temp_d.path,47),47) $+ $crlf
          did -a mcupdater_down 7 Update succeeded. $+ $crlf
          ; send signal that file has updated, in case we want to do anything special
          .McuFileUpdatesSignal %oldfilename
        }
      }
      else {
        ; new file not found - this is error
        if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 Update aborted: Downloaded file %fullfilename not found. $+ $crlf
        else echo 4 -s Update aborted: Downloaded file %fullfilename not found.
      }
    }
  }
}
;---------------------------------------------------------------------------







;---------------------------------------------------------------------------
alias mcu_fileintoedit {
  ; load the $1 file into the edit box
  var %iline
  var %count = 1
  var %totalcount = $lines($1-)

  while (%count <= %totalcount) {
    %iline = $read($1-,n,%count)
    if (%count > 1) did -a mcupdater_down 7 $crlf $+ %iline
    else did -a mcupdater_down 7 %iline
    %count = %count + 1
  }

  ; now go to top
  did -c mcupdater_down 7 1 
}
;---------------------------------------------------------------------------






;---------------------------------------------------------------------------
alias mcupdater_dostopdownload {
  ; and turn off any timers
  %mcu_temp_rt = 0
  %mcu_temp_read = 0
  sockclose rep
  ; stop our times
  .timer.mcuspeed off
  .timer.mcud off
}


alias mcupdater_doclose {
  ; first stop download
  .mcupdater_dostopdownload

  var %filename
  ; pick off first name
  %filename = $gettok(%mcu_temp_updatequeue,1,58)
  %mcu_temp_updatequeue = $deltok(%mcu_temp_updatequeue,1,58)

  ; does filename has a setmircvariable associated with it (separated by !), if so, strip it off and prepare it for running if user installs
  %mcu_temp_setmircvar = $null
  %mcu_temp_setmircvar = $gettok(%filename,2,33)
  %filename = $deltok(%filename,2,33)

  ; and run the next install on queue
  if (%filename != $null) {
    ; process next file in queue
    if ( (%mcu_temp_filetype == update) && (%mcu_temp_updatequeue == $null) ) {
      ; when there is only one file in an update, and they click to install update, then we just run it without asking for confirmation
      .timer.popinstallqueue 1 0 .mcupdate [ %filename ]
    }
    else {
      ; update with confirm 
      .timer.popinstallqueue 1 0 .mcupdate [ %filename ] confirm
    }
  }
  else {
    ; no more files to process
    if ($1 == force) {
      ; close dialog and clean variabls
      if (%mcu_temp_didshowversions != $true) {
        .McuUpdateVersionSignal
      }
      .timer.delayedclose 1 0 .dialog -x mcupdater_down
      .mcupdater_doclearvars
    }
    else {
      ; all done installing, now change button to say to close
      did -e mcupdater_down 3
      did -b mcupdater_down 5
      did -b mcupdater_down 6
      did -ra mcupdater_down 3 Done
      ;
      ; ask all versionable modules to show their version infos
      if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 ================================================ $+ $crlf
      ; make all modules shows their versions
      .McuUpdateVersionSignal
      %mcu_temp_didshowversions = $true
    }
  }
}

alias mcupdater_doclearvars {
  ; remove any variables we dont need anymore
  unset %mcu_temp_*
}
;---------------------------------------------------------------------------






;---------------------------------------------------------------------------
alias setmcuvariables {
  ; default persistant variables - if they are blank they will be set here. after that you can set
  ;  them programmaticaly in a script OR in the alt-R variables section, and they will preserver
  ;  even if you install a new version.
  if (%mcu_proxyip == $null) %mcu_proxyip = $null
  if (%mcu_proxyport == $null) %mcu_proxyport = $null
}
;---------------------------------------------------------------------------
