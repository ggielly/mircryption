; ------------------------------------------------------------------------
; MircryptionSuite - MircryptionPgp - v1.09.04 - mircryption pgp addon script
; 2001-2003, Dark Raichu
;-------------------------------------------------------------------------
;  pgp dlls are by phil zimmerman and network associates et al (www.pgp.com).
;  pgp wrappers for delphi are by Michael in der Wiesche <idw.doc@t-online.de>
;-------------------------------------------------------------------------
; See the mpgp help file for complete installation and usage info,
;  as well as a complete release history.
; ------------------------------------------------------------------------




; ------------------------------------------------------------------------
; INSTALLER, CALLED ONCE ON FIRST INSTALLATION
; just checks for the proper version of mirc
on 1:LOAD: {
  if ( $version < 5.9 ) {
    echo 7 -s $nopath($script) cannot be installed - it requires mIRC version 5.9 or later!
    echo
    mpgp_uninstall silent
    halt
  }

  echo 7 -si2 MircryptionSuite - MircryptionPgp script (MPGP) is now installed.  Use right-click pop-ups to access MPGP menu.
  echo 7 -si2 You can uninstall this script from the main menubar: Commands->MPGP->Uninstall.
}

alias mpgp_about {
  %mpgp_scriptversion = 1.09.03

  if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 MircryptionPgp ver %mpgp_scriptversion loaded - dll will be started when needed. $+ $crlf
  else echo 7 -s MircryptionSuite - MircryptionPgp ver %mpgp_scriptversion loaded - dll will be started when needed.
}
; ------------------------------------------------------------------------



; ------------------------------------------------------------------------
; INITIALIZATION STARTUP EACH TIME YOU LAUNCH MIRC
on 1:START: {
  ; Initialize mpgp for use

  ; script version
  %mpgp_scriptdll =  " $+ $scriptdir $+ mpgp.dll $+ "
  ; set variables to defaults if they are not already set
  .setmpgpvariables

  ; we use mpgp_dllversion to tell us if dll is installed and available
  %mpgp_dllrunning = no
  %mpgp_dllversion = n/a
  %mpgp_connected = $true

  .mpgp_about

  ; load the dll and get dll version - if it fails, we wont activate it in future
  if (!$isfile(" $+ $scriptdir $+ mpgp.dll")) {
    ; the mpgp file was not found, so we will not load it
    echo 4 -------------------------------
    echo 4 *** MIRCRYPTION MPGP ERROR: You have loaded the micryption pgp addone but the accompanying dll file $scriptdir $+ mpgp.dll was not found.  Please copy the dll file into the proper place, or uninstall the mircryption pgp addon, then restart mirc.
    echo 4 -------------------------------
    %mpgp_disable = yes
    .timer 1 3 unload -rs $script
    halt
  }
}

; simple shortcut function that other scripts can use to check if mpgp is running.
; to check you would use this in your script:  if ($gotmpgp) ...
alias gotmpgp { return $true }

; signal used to tell all mircryption modules to show their version information
on *:SIGNAL:MircryptionInternalSignal_About: {
  .mpgp_about
}
; ------------------------------------------------------------------------



; ------------------------------------------------------------------------
; HELP / ABOUT
alias mpgp {
  ; launch the help file if it is in mirc directory
  if ( $exists(mpgp.chm) ) .run mpgp.chm
  else if ( $exists(mpgp.hlp) ) .run mpgp.hlp
  ; now show commandline help in status window
  echo 7 -si4 - 
  echo 7 -si4 . MircryptionSuite - MircryptionPgp ver %mpgp_scriptversion - Dark Raichu - 7/30/02
  echo 7 -si4 . 
  echo 7 -si2 . pgp dlls are by phil zimmerman and network associates et al.
  echo 7 -si2 . pgp vcl wrappers are by Michael in der Wiesche.
  echo 7 -si4 . 
  echo 7 -si2 . Use right-click popup menus on ->NICKS<- to launch MPGP dialog.
  echo 7 -si4 . 
  echo 7 -si4 . NOTE: For detailed info see help file (Help->Help Files->MircryptionSuite).
  echo 7 -si2 . Uninstall script from the menu: Commands->MPGP->Uninstall.
  echo 7 -si2 - 
  %mpgp_dllversion = $dll(%mpgp_scriptdll , version, dummy )
  if (%mpgp_dllversion != $null) echo 7 -si2 . MircryptionSuite - MircryptionPgp script v. %mpgp_scriptversion (dll v $+ %mpgp_dllversion $+ ) started.
}
; ------------------------------------------------------------------------



; ---------------------------------------------------------------------------
; MENUS

menu menubar {
  &MPGP
  .Show Pending Joblist:/mpgp_showjobs
  .Show Options Dialog:/mpgp_showoptions
  .-
  .Unload the MPGP dll from memory:/mpgp_forceunload
  .-
  .Uninstall the MPGP script now:/mpgp_uninstall
  .-
  .help/about:/mpgp
}

menu channel,status {
  &MPGP
  .Show Pending Joblist:/mpgp_showjobs
  .Show Options Dialog:/mpgp_showoptions
  .-
  .Unload the MPGP dll from memory:/mpgp_forceunload
  .-
  .help/about:/mpgp
}


menu nicklist {
  ;  .$submenu($mpgpnickmenu($1, [ $chan ] , [ $$1 ] ))
  MPGP Crypto - Send Something to [ $$1 ] :/mpgp_presentform default [ $chan ] [ $$1 ]
  MPGP Crypto - Secure Private Query Window w/ [ $$1 ] : /mpgp_performquery [ $$1 ]
}

menu query {
  &MPGP
  .MPGP Crypto Operation - $$1:/mpgp_presentform default $$1 $$1
  .MPGP Randomize Query Key and Exchange It - $$1:/mpgp_performquery $$1
  ;  .MPGP Crypto Operation:/mpgp_presentform
  ;  .MPGP Randomize Query Key and Exchange It:/mpgp_performquery
  .-
  .Show Pending Joblist:/mpgp_showjobs
  .Show Options Dialog:/mpgp_showoptions
  .-
  .Unload the MPGP dll from memory:/mpgp_forceunload
  .-
  .help/about:/mpgp
}

menu chat {
  &MPGP
  .MPGP Crypto Operation - $$1:/mpgp_presentform default $$1 $$1
  .MPGP Randomize Chat Key and Exchange It - $$1:/mpgp_performchat $$1
  ;  .MPGP Crypto Operation:/mpgp_presentform
  ;  .MPGP Randomize Chat Key and Exchange It:/mpgp_performchat
  .-
  .Show Pending Joblist:/mpgp_showjobs
  .Show Options Dialog:/mpgp_showoptions
  .-
  .Unload the MPGP dll from memory:/mpgp_forceunload
  .-
  .help/about:/mpgp
}

alias mpgpnickmenu {
  ;  if ($3 == $null) return
  ;  if ($2 == $null) return
  if ($1 == begin) return -
  if ($1 == 1) return MPGP Crypto - Send Something to [ $3 ] :/mpgp_presentform default [ $2 ] [ $3 ]
  if ($1 == 2) return MPGP Crypto - Open Private Query Window with Random Keyphrase for [ $3 ] : /mpgp_performquery [ $3 ]
  if ($1 == end) return -
}
; ---------------------------------------------------------------------------










; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------




alias mpgpform {
  ; easy shortcut
  var %uname | %uname = $1
  if (%uname == $null) %uname = $input(Nick name of the person you want to open up dialog for:,1,Open MPGP Trasnmission Dialog Form,%uname)
  if (%uname == $null) return
  .mpgp_presentform default %uname %uname
}





; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------









; ---------------------------------------------------------------------------
; PRESENT FORMS ON MENU REQUEST

alias mpgp_presentform {
  ; present the main form

  ; get parameters
  var %startform | %startform = $1
  var %cname | %cname = $2
  var %nname | %nname = $3

  if (%cname == $null) {
    ; if not passed any parameters, set them smartly
    var %tname
    %tname = $nick
    if (%tname == $null) %tname = $chan
    if (%tname == $null) return
    %cname = %tname
    %nname = %tname
    %startform = default
  }

  %cname = $mpgp_chatchan(%cname)

  var %toident | %toident = $mpgp_identnick(%nname,%cname)
  var %fromident | %fromident = $mpgp_identnick($me,%cname)
  var %chankey

  ; channel key set to random if we are forcing a channelkey
  if (%startform == chankey) %chankey = .
  else %chankey = $mpgp_currentchannelkey(%cname)

  ; echo 7 -si4 Present %cname , %nname , %toident , %fromident , %chankey

  ; make sure valid values for al
  if (%chankey == $null) %chankey = .
  if (%cname == $null) {
    echo -si4 MPGP Script Error: can't lookup channel name.
    return
  }
  if (%nname == $null) {
    echo -si4 MPGP Script Error: can't lookup target nick name.
    return
  }
  if (%toident == $null) {
    echo -si4 MPGP Script Error: can't lookup target nick ident.
    return
  }
  if (%fromident == $null) {
    echo -si4 MPGP Script Error: can't lookup our ident.
    return
  }

  ; call it
  var %mpgpretv
  var %mpgp_data
  %mpgp_data = %cname %nname %toident %fromident %startform %chankey
  %mpgpretv = $dll(%mpgp_scriptdll , dllPresent, %mpgp_data )
}


alias mpgp_showjobs {
  var %retv
  %retv = $dll(%mpgp_scriptdll , dllShowJobs, dummyval )
}

alias mpgp_showoptions {
  ; show the options dialog
  var %retv
  %retv = $dll(%mpgp_scriptdll , dllShowOptions, dummyval )
}

alias mpgp_performquery {
  var %tname | %tname = $1
  if (%tname == $null) %tname = $nick
  if (%tname == $null) %tname = $chan
  if (%tname == $null) return

  .mpgp_presentform chankey %tname %tname
  /QUERY $1
  echo 7 $1 Send MPGP channel key to $1 and wait for confirmation before begining proceeding.
}

alias mpgp_performchat {
  var %tname | %tname = $1
  if (%tname == $null) %tname = $nick
  if (%tname == $null) %tname = $chan
  if (%tname == $null) return

  .mpgp_presentform chankey %tname %tname
  echo 7 $1 Send MPGP channel key to $1 and wait for confirmation before begining proceeding.
}
;--------------------------------------------------------------------------- 





; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------





;--------------------------------------------------------------------------- 
; process an output line - called from dll or from a manual iteration call above
; return 1 on success, 0 on failure
alias mpgp_DllOutputs {
  ; okay now split the line into <nick text...>

  ;  echo 7 si START OUTLINE

  var %tonick
  var %textmsg
  %tonick = $gettok($1,1,32)
  %textmsg = $deltok($1,1,32)

  ; echo 7 -s mcps1 TO %tonick AND MESG = %textmsg

  if (%tonick == $null) return
  if (%textmsg == $null) return

  ; okay, send it to them!
  ; echo 7 -s mpgp sending: .notice %tonick %mpgp_tag %textmsg

  ; return false if the nick is not visible
  if ($mpgp_nickonline(%tonick) == $false) {
    echo 7 -si badnick in mpgp_dlloutputs ( %tonick )
    return 0
  }

  .notice %tonick %mpgp_tag %textmsg

  ;  echo 7 si END OUTLINE

  return 1
}
;--------------------------------------------------------------------------- 




alias mpgp_TestOut {
  ;
  /echo hello and received $1
}


alias mpgp_echo {
  ; echo some stuff to a particular channel
  var %cname | %cname = $null

  if ( $chan($1) != $null) %cname = $1
  if ( $query($1) != $null ) %cname = $1
  if ( $chat($1) != $null ) %cname = $1

  if (%cname == $null) {
    %cname = $1
    /QUERY [ %cname ]
    .timer_mpgp_tempecho 1 1 echo 7 [ %cname ] $2-
  }
  else echo 7 [ %cname ] $2-
}

alias mpgp_ensurequery {
  ; if query for $1 is not open, open it
  if ( $query($1) != $null ) return
  if ( $chat($1) != $null ) return
  /QUERY [ $1 ]
}









; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
















;--------------------------------------------------------------------------- 
; PROCESS INPUT LINE PARSED FROM A NOTICE EVENT BY MIRCRYPTION

alias mpgp_processInputLine {
  ; process a line of input from someone
  ; get parameters

  ;  echo 7 si START PROCESSINLINE

  if (%mpgp_disable == yes) return

  ; echo 7 -si got job input, chan = $1  nick = $2  text = $3-

  var %cname | %cname = $mpgp_chatchan($1) 
  var %nname | %nname = $2
  var %toident | %toident = $mpgp_identnick(%nname,%cname)
  var %fromident | %fromident = $mpgp_identnick($me,%cname)

  if (%cname == $null) {
    echo -si4 MPGP Script Error: can't lookup channel name.
    return
  }
  if (%nname == $null) {
    echo -si4 MPGP Script Error: can't lookup target nick name.
    return
  }
  if (%toident == $null) {
    echo -si4 MPGP Script Error: can't lookup target nick ident.
    return
  }
  if (%fromident == $null) {
    echo -si4 MPGP Script Error: can't lookup our ident.
    return
  }

  ; call it
  var %mpgpretv
  var %mpgp_data
  %mpgp_data = %cname %nname %toident %fromident $3-
  %mpgpretv = $dll(%mpgp_scriptdll , dllReceiveInput, %mpgp_data )
  %mpgpretv = $null

  ;  echo 7 si END PROCESSINLINE
  return
}
;--------------------------------------------------------------------------- 











; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------











;--------------------------------------------------------------------------- 
;STARTUP AND HELPER FUNCTIONS

alias setmpgpvariables {
  ; default persistant variables - if they are blank they will be set here. after that you can set
  ;  them programmaticaly in a script OR in the alt-R variables section, and they will preserver
  ;  even if you install a new version.
  if (%mpgp_disable == $null) %mpgp_disable = no
  if (%mpgp_loadtime == $null) %mpgp_loadtime = delayed
  if (%mpgp_output_interval == $null) %mpgp_output_interval = 2
  if (%mpgp_outbytes_periterate == $null) %mpgp_outbytes_periterate = 200
  if (%mpgp_fastunload == $null) %mpgp_fastunload = sleep
  if (%mpgp_tag == $null) %mpgp_tag = mcps
  if (%mpgp_dllrunning == $null) %mpgp_dllrunning = yes
}

; uninstall helper
alias mpgp_uninstall {
  ; uninstall script
  ; erase all permanent variables related to script

  if ($1 != silent) {
    ; verify they really want to install it
    var %confirmer
    %confirmer = $input(Type the word 'uninstall' to confirm uninstallation:,1,Uninstall MPGP)
    if (%confirmer != uninstall) {
      echo 7 -s Uninstallation of MPGP canceled.
      return
    }
  }
  unset %mpgp_*
  echo 7 -s MPGP script $nopath($script) has been uninstalled.
  echo 7 -s To reinstall, type /load -rs mircryption\mpgp.mrc
  .unload -rs mpgp.mrc
  .unload -rs mircryption\mpgp.mrc
  .unload -rs $script
  halt
}

alias mpgp_forceunload {
  ; force unloading of mpgp dlls
  %mpgp_dllrunning = no
  .dll -u %mpgp_scriptdll
}

alias mpgp_forcegentlesleep {
  ; force sleep of dll
  ; echo 5 asking for gentle unload
  return $dll(%mpgp_scriptdll , dllGentleSleep , dummy )
}

;--------------------------------------------------------------------------- 





;--------------------------------------------------------------------------- 
; OTHER HELPERS

alias mpgp_currentchannelkey {
  ; lookup mircryption current channel key
  return $dll(%mc_scriptdll , mc_displaykey , $1)
}

alias mpgp_identnick {
  ; $1 = nick, $2 = chan/query
  ; find ident associated with nick
  ; this just does not work reliably, espcially with bncs, even if we force /who, etc.
  ; for now, we are justgoing to return the nick as the ident
  var %retv
  ; %retv = $address($1,1)
  ; %retv = $2 $+ : $+ $1 $+ : $+ $network

  ; we USED to return $1:$network , but now we return $1:$chan where $chan is fixed to lowercase and multiserve net removed
  var %retv = $2
  ; NEW fixes for different server/client behaviors:  lowercase it and strip multiserv header
  var %multiservepos = $pos(%retv,~#,1 )
  if ( %multiservepos > 0) {
    %multiservepos = $len(%retv) - %multiservepos
    %retv = $right( %retv , %multiservepos )
  }

  ; it seems the identnick info is unreliable, since pgp requests can come in from queries (ie not same channel)
  ; so for now we just use the nick as the indent info.  this is ok because the nature of the proof string makes it unhackable anyway.
  ; %retv = $1 $+ : $+ %retv
  %retv = $1
  %retv = $lower(%retv)

  return %retv
}

alias mpgp_chatchan {
  ; chat channels have '=' in front of their names, we remove that
  var %cname = $1
  if ($left(%cname , 1) == $chr(61)) {
    ; chat channel names have = in front, we remove that
    var %clen = $len(%cname)
    dec %clen
    %cname = $right(%cname , %clen)
  }
  else if ( [ $left(%cname,4) ] == chat && $mid(%cname,5,1) == $chr(32)) {
    var %clen = $len(%cname)
    %clen = %clen - 5
    %cname = $right(%cname , %clen)
  }

  return %cname
}

alias mpgp_testident {
  ; find ident associated with nick
  var %retv
  %retv = $address($1,1)
  echo Address %retv
  /userhost $1
  %retv = $chan $+ : $+ $1
  echo -s ANSER: %retv
  return
}

alias mpgp_setkey {
  ; just pass on the channel key to mircryption
  .setkey $1 $2-
}

alias mpgp_nickonline {
  ; return true if we are connected and can see user $1

  ;  echo 7 -si mpgpconnected = %mpgp_connected  and dollar 1 = $1
  ;  echo 7 -si comchan = $comchan($1,0)
  ;  echo 7 -si query = $query($1)
  ;  echo 7 -si chat = $chat($1)

  if ( $mpgp_isconnected( dummy ) == $false) return $false
  if ( $comchan($1,0) > 0 ) return $true
  if ( $query($1) != $null ) return $true
  if ( $chat($1) != $null ) return $true
  return $false
}

alias mpgp_isconnected {
  ; fucking god damn connect/disconnect event do not work properly
  if ($server != $null) return $true
  if ($status == connected) return $true
  if ( %mpgp_connected == $true) return $true
  return $false
}
;--------------------------------------------------------------------------- 






;--------------------------------------------------------------------------- 
;--------------------------------------------------------------------------- 


alias mpgp_connectest {
  /echo 7 -si server = $server
  /echo 7 -si server0 = $server(0)
  /echo 7 -si status = $status
  /echo 7 -si mpgpconnected = %mpgp_connected
}





;--------------------------------------------------------------------------- 
; EVENT HANDLERS
on *:CONNECT: {
  %mpgp_connected = $true
  if (%mpgp_dllrunning == yes) {
    ; tell it to resume outputting
    .mpgp_ResumeOutputting
  }
}


on *:DISCONNECT: {
  %mpgp_connected = $false
  if (%mpgp_dllrunning == yes) {
    ; tell it to resume outputting
    .mpgp_PauseOutputting
  }
}
;--------------------------------------------------------------------------- 







;--------------------------------------------------------------------------- 
;--------------------------------------------------------------------------- 






;--------------------------------------------------------------------------- 
alias mpgp_ResumeOutputting {
  ; send a command to the dll to tell it we are re-connected and so are able to receive output
}

alias mpgp_PauseOutputting {
  ; send a command to the dll to tell it we are disconnected and so we dont want output
}

alias mpgp_IsUnloading {
  ; called by the dll to tell us that is is no longer accessible
  %mpgp_dllrunning = no
  echo 7 -si IsUnLoading
}

alias mpgp_IsLoading {
  ; called by the dll to tell us that is is now accessible
  %mpgp_dllrunning = yes
  %mpgp_dllversion = $1
  echo 7 -si IsLoading with $mpgp_dllversion
}

alias mpgp_CanSleep {
  ; called by the dll to tell us that there are no jobs pending, so we can sleep if we want to
  if (%mpgp_fastunload == yes) {
    ; unload it
    .timer_mpgp_tempunload 1 2 .mpgp_forceunload
  }
  else if (%mpgp_fastunload == sleep) {
    ; unload it
    .timer_mpgp_tempunload 1 2 .mpgp_forcegentlesleep
  }
}
;--------------------------------------------------------------------------- 


;---------------------------------------------------------------------------
alias mpgpstresstest {
  ; load mpgp
  /mpgp_showoptions
  .timer_mpgptest 1 3 .mpgpstresstest_stage2
}

alias mpgpstresstest_stage2 {
  ; load mpgp
  /mpgp_forceunload
  .timer_mpgptest 1 3 .mpgpstresstest
}

alias killmpgpstresstest {
  .timer_mpgptest off
}
;---------------------------------------------------------------------------
