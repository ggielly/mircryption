; ------------------------------------------------------------------------
; MircryptionSuite - mcboardbot - v1.08.2 - messageboard/news bot *SERVER BOT*
;  normally you would use the tcl/eggdrop bot on a shell, but this is
;  for if you want to just run the newsbot as a user locally in mirc.
; 05/30/04, Dark Raichu
; 07/01/05 - adding auto irc topic feature
;-------------------------------------------------------------------------
; See the mcboardbot help file for complete installation and usage info,
;  as well as a complete release history.
; ------------------------------------------------------------------------




; ------------------------------------------------------------------------
; INSTALLER, CALLED ONCE ON FIRST INSTALLATION
; just checks for the proper version of mirc
on 1:LOAD: {
  if ( $version < 5.9 ) {
    echo 7 -s $nopath($script) cannot be installed - it requires mIRC version 5.9 or later!
    echo
    mcboardbot_uninstall silent
    halt
  }
  echo 7 -si2 MircryptionSuite - MCBoardBot server script is now installed.  Use right-click pop-ups to access mcboard menus.
  echo 7 -si2 You can uninstall this script from the main menubar: Commands->mcboardbot->Uninstall.
}
; ------------------------------------------------------------------------



; ------------------------------------------------------------------------
; INITIALIZATION STARTUP EACH TIME YOU LAUNCH MIRC
on 1:START: {
  ; Initialize mcboardbot for use

  ; script version
  %mcbb_scriptversion = 1.08.03
  %mcbb_scriptdll = " $+ $scriptdir $+ mircryption"
  
  ; set variables to defaults if they are not already set
  .setmcboardbotvariables

  echo 7 -s MircryptionSuite - MCBoardBot ver %mcb_scriptversion loaded and ready.
}
; ------------------------------------------------------------------------



; ------------------------------------------------------------------------
; HELP / ABOUT
alias mcboardbot {
  ; launch the help file if it is in mirc directory
  if ( $exists(mcboard.chm) ) .run mcboard.chm
  else if ( $exists(mcboard.hlp) ) .run mcboard.hlp
  ; now show commandline help in status window
  echo 7 -si4 - 
  echo 7 -si4 . MircryptionSuite - MCBoardBot ver %mcbb_scriptversion - Dark Raichu - 8/22/02
  echo 7 -si4 . 
  echo 7 -si2 . Local News Bot for use with mcboard script
  echo 7 -si4 . 
  echo 7 -si2 . Use right-click popups to access mcboard menus.
  echo 7 -si4 . 
  echo 7 -si4 . NOTE: For detailed info see help file (Help->Help Files->MircryptionSuite).
  echo 7 -si2 . Uninstall script from the menu: Commands->mcboardbot->Uninstall.
  echo 7 -si2 - 
}
; ------------------------------------------------------------------------



; ---------------------------------------------------------------------------
; MENUS

menu menubar,status {
  &MCBoardBot
  .-
  .Uninstall the MCBoardBot script now:.mcboardbot_uninstall
  .-
  .help/about:.mcboardbot
}


; ---------------------------------------------------------------------------




; ---------------------------------------------------------------------------
; intercept outgoing text to news bot and process it
on ^*:text:!mcb *:#: {
  .mcboardbot_command $2-
  haltdef
}
on ^*:text:!mcb *:?: {
  .mcboardbot_command $2-
  haltdef
}
on ^*:notice:!mcb *:*: {
  .mcboardbot_command $2-
  haltdef
}

on ^*:SECTION:*: {
  ; there is an option to auto add all section changes
  .mcboardbot_command autoircsection $1-
}


on ^*:TOPIC:*: {
; let script auto add topicslol
.mcboardbot_command autoirctopic $1-
}


alias mcboardbot_command {
  var %id
  var %cname = $chan
  if ($nick == $null) {
    %id = $mcboardbot_identnick($me)
    } else {
    %id = $mcboardbot_identnick($nick)
    }

  if (%cname == $null) %cname = __NOCHAN__
  var %userclass = NOTOP

  ; /echo 2 -s BOARD BOT RECEIVES $1- from %id on %cname
  ; invoke newsboard.exe FILENAME CHAN NICK "IDENT" USERCLASS "COMMAND STRING"
  ;/echo 2 -s INVOKING: newsboard.exe " $+ $scriptdir $+ %mcbb_newsfilename $+ " %cname $nick " $+ %id $+ " %userclass " $+ $1- $+ "
  /stdrun " $+ $scriptdir $+ newsboard.exe $+ " -m " $+ $scriptdir $+ %mcbb_newsfilename $+ " %cname $nick " $+ %id $+ " %userclass " $+ $1- $+ "
}

alias exeoutputline {
  ; process an output line from the executable
  ; /echo 1 bot sends output: $1-
  [ $1- ]
}
; ---------------------------------------------------------------------------












;--------------------------------------------------------------------------- 
;STARTUP AND HELPER FUNCTIONS

alias setmcboardbotvariables {
  ; default persistant variables - if they are blank they will be set here. after that you can set
  ;  them programmaticaly in a script OR in the alt-R variables section, and they will preserver
  ;  even if you install a new version.
  if (%mcbb_newsfilename == $null) %mcbb_newsfilename = mcb_options.cfg
}

; uninstall helper
alias mcboardbot_uninstall {
  ; uninstall script
  ; erase all permanent variables related to script

  if ($1 != silent) {
    ; verify they really want to install it
    var %confirmer
    %confirmer = $input(Type the word 'uninstall' to confirm uninstallation:,1,Uninstall MCBoardBot)
    if (%confirmer != uninstall) {
      echo 7 -s Uninstallation of MCBoardBot canceled.
      return
    }
  }
  unset %mcbb_*
  echo 7 -s MCBoardBot script $nopath($script) has been uninstalled.
  echo 7 -s To reinstall, type /load -rs mcboardbot.mrc
  .unload -rs mcboardbot.mrc
  .unload -rs $script
  halt
}
;--------------------------------------------------------------------------- 







;--------------------------------------------------------------------------- 
; OTHER HELPERS

alias mcboardbot_identnick {
  ; find ident associated with nick
  ; this just does not work reliably, espcially with bncs, even if we force /who, etc.
  ; for now, we are justgoing to return the nick as the ident
  var %retv
  %retv = $address($1,1)
  return %retv
}

alias mcboardbot_testident {
  ; find ident associated with nick
  var %retv
  %retv = $address($1,1)
  echo Address %retv
  /userhost $1
  %retv = $chan $+ : $+ $1
  echo -s ANSER: %retv
  return
}

//---------------------------------------------------------------------------












;--------------------------------------------------------------------------- 
;--------------------------------------------------------------------------- 
; from dragonzap's stdio.dll mrc example script

alias -l udll return $scriptdir $+ stdio.dll
alias -l stdio return $dll($udll,$1,$2-)

alias stdrun {
  var %proc = $newprog
  var %run = $stdio(RunConsole, [ %proc ] [ $1- ] )
  if ( $gettok(%run,1,32) == OK ) {
    ;    window -c %proc
    ;    window -ek %proc
    ;    titlebar %proc : $1-
    .timerReadT $+ %proc -m 0 1 readl %proc
  }
  else echo 4 -st Could not create process: %run
}

alias -l readl {
  var %out
  var %err
  while ( $gettok( %err ,1,32) != ERROR ) {
    set %err $stdio(ReadText, [ $1 ] err)
    if ( %err == OK ) set %err OK   
    if ( $gettok(%err,1,32) != ERROR ) echo 4 -s mcboardbot: $ansi2mirc($gettok(%err,2-,32))
  }

  ; repeat several times for faster response
  while ( $gettok( %out ,1,32) != ERROR ) {
    set %out $stdio(ReadText, [ $1 ] out)
    if ( %out == OK ) set %out OK   
    if ( $gettok(%out,1,32) != ERROR ) exeoutputline $ansi2mirc($gettok(%out,2-,32))
  }
  while ( $gettok( %out ,1,32) != ERROR ) {
    set %out $stdio(ReadText, [ $1 ] out)
    if ( %out == OK ) set %out OK   
    if ( $gettok(%out,1,32) != ERROR ) exeoutputline $ansi2mirc($gettok(%out,2-,32))
  }
  while ( $gettok( %out ,1,32) != ERROR ) {
    set %out $stdio(ReadText, [ $1 ] out)
    if ( %out == OK ) set %out OK   
    if ( $gettok(%out,1,32) != ERROR ) exeoutputline $ansi2mirc($gettok(%out,2-,32))
  }
  while ( $gettok( %out ,1,32) != ERROR ) {
    set %out $stdio(ReadText, [ $1 ] out)
    if ( %out == OK ) set %out OK   
    if ( $gettok(%out,1,32) != ERROR ) exeoutputline $ansi2mirc($gettok(%out,2-,32))
  }
  while ( $gettok( %out ,1,32) != ERROR ) {
    set %out $stdio(ReadText, [ $1 ] out)
    if ( %out == OK ) set %out OK   
    if ( $gettok(%out,1,32) != ERROR ) exeoutputline $ansi2mirc($gettok(%out,2-,32))
  }

  if (( $gettok(%out,1-2,32) == ERROR STDE6 ) && ( $gettok(%err,1-2,32) == ERROR STDE6 )) {
    .timerReadT $+ $1 off
    ;    echo 3 -t $1 Process exited, code $stdio(GetExitCode,$1)
  }
}

alias -l newprog {
  var %prognum = 1
  var %progpref = @Console.
  while ( $gettok($stdio(GetProcess, [ [ %progpref ] $+ [ %prognum ] ] ) ,1,32) == OK) inc %prognum
  return %progpref $+ %prognum
}
;--------------------------------------------------------------------------- 
;--------------------------------------------------------------------------- 
