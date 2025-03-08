;---------------------------------------------------------------------------
; super slapper script, v1.0.13
; 11/19/03
;
; superslapper is a mirc dialog-based addon that lets you apply random
;  and custom slaps to people (extends mirc's built in command that
;  generates the line "/me slaps xxx around a bit with a large trout").
;
; to install:
;  just copy the entire SuperSlapper directory to your mirc directory, and
;   from inside mirc type /load -rs SuperSlapper\superslapper.mrc
;
; To slap someone, right click on their nick, and choose to superslap them.
;
; Or you can use the following manual commands:
;   /slap NICK to do a random slap, presenting dialog for confirmation.
;   /slapc NICK to do a random slap with clipboard object, presenting dialog for confirmation.
;   /slaps NICK to do a random slap without confirmation dialog.
;   /slapall NICK to do a random slap of all users in chan.
;   /slapallops NICK to do a random slap of all ops in chan.
;   /slapallnonops NICK to do a random slap of all non-ops in chan.
;
; about slap data files:
;   you can edit superslapper_slaplines.txt to add your own slap insults.
;    optionally use _nick and _me and _obj1 and _obj2 etc. to define words to replace.
;   you can edit superslapper_objects.txt to define new objects which can replace _obj?
;   you can include blank lines and comments (start comments with //)
;
; you can also have multiple slaplines and object files, and you can switch
;  between them in the dialog, useful if you want to have a file of nice
;  things to say to your sweetheart :)
; just make sure the files are named *lines*.txt and *objects*.txt,
;  and put them somewhere in the same dir or a subdir of the script.
;
; you can launch your default editor for the currently selected slapfiles
;  using the Edit button in the super slap dialog; after editing a file,
;  click refresh to reload the data from a modified file.
;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
; history
; 11/19/03 - v1.0.13
;   added line to show the script is loaded when it loads.
; 09/19/03 - v1.0.12
;   changed | to ; for separators of multiple statements, to avoid nick character |
; 09/18/03 - v1.0.11
;   added support for using . instead of / for arbitrary command executiion
; 09/14/03 - v1.0.10
;   added new menu option to slap the target of the clipboard.
; 09/12/03 - v1.0.9
;   added new slap+close button
; 09/09/03 - v1.0.8
;  fixed bug in win98 with combo boxes
;  added ability to specify arbitrary mirc statements to perform on a line (instead of just an action)
;   you can separate multiple statements with |
;   you can refer to channel with _chan
;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
on *:start:{
  ; on startup, create default variables if they dont exist
  if (%superslapper_slaplinefile == $null) %superslapper_slaplinefile = slap_lines.txt
  if (%superslapper_objectfile == $null) %superslapper_objectfile = slap_objects.txt

  ; let user know it's running
  echo 13 -si2 SuperSlapper script v1.00.13 loaded and ready.
}
;---------------------------------------------------------------------------




;---------------------------------------------------------------------------
; menu
menu nicklist {
  -
  SuperSlapper - slap with random object ( $+ $$1 $+ ):/ssp_slap $$1 $false $null
  SuperSlapper - slap with clipboard object ( $+ $$1 $+ ):/ssp_slap $$1 $false $cb
  SuperSlapper - slap using clipboard object as target:/ssp_slap clipboardcontents $false $null
  -
}

;; extra functions available through command menu
;menu menubar,status {
;  &SuperSlapper
;  .Edit superslapper_slaplines.txt:/run $scriptdir $+ $char(92) $+ %superslapper_slaplinefile
;  .Edit superslapper_objects.txt:/run $scriptdir $+ $char(92) $+ %superslapper_object
;}

; extra functions available through chan
menu channel {
  &SuperSlapper
  ;  .Edit superslapper_slaplines.txt:/run $scriptdir $+ $char(92) $+ superslapper_slaplines.txt
  ;  .Edit superslapper_objects.txt:/run $scriptdir $+ $char(92) $+ superslapper_objects.txt
  ;  .-
  .Slap all nicks in the channel:/slapall
  .Slap all ops in the channel:/slapallops
  .Slap all non-ops in the channel:/slapallnonops
  .slap using clipboard object as target:/ssp_slap clipboardcontents $false $null
}
;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
; short alias
alias slap {
  ; slap
  if ($1 == $null) return
  /ssp_slap $1 $false $null
}

alias slapc {
  ; slap with clipboard
  if ($1 == $null) return
  /ssp_slap $1 $false $cb
}

alias slaps {
  ; slap and dont show dialog, just send right away
  if ($1 == $null) return
  /ssp_slap $1 $true $null
}

alias slapall {
  ; slap all users in chan without dialog
  var %nickname
  var %count = 1
  while ($nick($chan($active),%count,a) != $null) {
    %nickname = $nick($chan($active),%count,a)
    if (%nickname != $me) /ssp_slap %nickname $true $null
    %count = %count + 1
  }
}

alias slapallops {
  ; slap all ops in chan without dialog
  var %nickname
  var %count = 1
  while ($nick($chan($active),%count,o) != $null) {
    %nickname = $nick($chan($active),%count,o)
    if (%nickname != $me) /ssp_slap %nickname $true $null
    %count = %count + 1
  }
}

alias slapallnonops {
  ; slap all nonops in chan without dialog
  var %nickname
  var %count = 1
  while ($nick($chan($active),%count,a,o) != $null) {
    %nickname = $nick($chan($active),%count,a,o)
    if (%nickname != $me) /ssp_slap %nickname $true $null
    %count = %count + 1
  }
}
;---------------------------------------------------------------------------



;---------------------------------------------------------------------------
; start slap
alias ssp_slap {
  ; save vars
  %tmp_slapchan = $chan
  %tmp_slapnick = $1
  %tmp_slapautosend = $2
  %tmp_slapcb = $3-
  %tmp_slapreshow = $false
  var %slapreshow

  ; clipboard contents
  if (%tmp_slapnick == clipboardcontents) %tmp_slapnick = $cb

  ; show dialog
  var %retv = $dialog(slapdialog,slapdialog,-2)
  if (%retv != $null) {
    ; say slap
    /ssp_sayslap %retv
  }

  ; set non global var
  %slapreshow = %tmp_slapreshow

  ; unset vars
  /unset %tmp_slapchan 
  /unset %tmp_slapnick 
  /unset %tmp_slapcb
  /unset %tmp_slapautosend
  /unset %tmp_slapreshow

  ; relaunch slap dialog for another?
  if (%slapreshow == $false) return
  if ($2 == $true) return
  if (%retv != $null) .timerSlapRepeat 1 0 /ssp_slap $1 $false $3
}
;---------------------------------------------------------------------------



;---------------------------------------------------------------------------
; choose a random slap line from list
alias ssp_chooserandomslapline {
  var %slapindex2

  var %slapobject = $did(slapdialog,5).seltext
  if ( (%slapobject != $cb) || ($cb == $null) ) {
    %slapindex2 = $ssp_pickgooddialoglistitem(4,$null)
  }
  else {
    %slapindex2 = $ssp_pickgooddialoglistitem(4,_obj)
  }

  if (%slapindex2 == 0) return
  /did -c slapdialog 4 %slapindex2
}


; choose a random slap object from list
alias ssp_chooserandomslapobject {
  var %slapindex2
  %slapindex2 = $ssp_pickgooddialoglistitem(5,$null)
  if (%slapindex2 == 0) return
  /did -c slapdialog 5 %slapindex2
}


; pick a random slap object from file
alias ssp_pickrandomslapobject {
  var %slapindex2
  %slapindex2 = $ssp_pickgooddialoglistitem(5,$null)
  if (%slapindex2 == 0) return an empty file
  return $did(slapdialog,5,%slapindex2).text 
}


; pick a random non-comment line from a dialog list
alias ssp_pickgooddialoglistitem {
  var %dialognum = $1
  var %needstr = $2
  var %slapobject
  var %slapindex
  var %slapindex2
  var %tempcounter = 0

  %slapindex = $did(slapdialog,%dialognum).lines
  if (%slapindex == 0) return 0

  while ($true) {
    %slapindex2 = $rand(1,%slapindex)
    %slapobject = $did(slapdialog,%dialognum,%slapindex2).text 
    if (($pos(%slapobject,//,1) != 1) && ($len(%slapobject)>1)) {
      if ((%needstr == $null) || ($pos(%slapobject,%needstr,1)!=$null) ) {
        return %slapindex2
      }
    }
    %tempcounter = %tempcounter +1
    if (%tempcounter == 50) return %slapindex2
  }
}
;---------------------------------------------------------------------------





;---------------------------------------------------------------------------
; fill in any missing objects
alias ssp_fillslapline {
  var %slapline = $1
  var %slapnick = $2
  var %slapobject = $3
  var %tempobj

  ; replace target nick
  %slapline = $replace(%slapline,_nick,%slapnick)

  ; replace misc. stuff (like $me)
  %slapline = $replace(%slapline,_me,$me)
  %slapline = $replace(%slapline,_chan,%tmp_slapchan)

  ; replace objects
  if ($pos(%slapline,_obj1,0)>0) {
    %tempobj = %slapobject
    if (%tempobj == $null) %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj1,%tempobj)
  }
  if ($pos(%slapline,_obj2,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj2,%tempobj)
  }
  if ($pos(%slapline,_obj3,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj3,%tempobj)
  }
  if ($pos(%slapline,_obj4,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj4,%tempobj)
  }
  if ($pos(%slapline,_obj5,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj5,%tempobj)
  }
  if ($pos(%slapline,_obj6,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj6,%tempobj)
  }
  if ($pos(%slapline,_obj7,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj7,%tempobj)
  }
  if ($pos(%slapline,_obj8,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj8,%tempobj)
  }
  if ($pos(%slapline,_obj9,0)>0) {
    %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj9,%tempobj)
  }
  if ($pos(%slapline,_obj,0)>0) {
    %tempobj = %slapobject
    if (%tempobj == $null) %tempobj = $ssp_pickrandomslapobject()
    %slapline = $replace(%slapline,_obj,%tempobj)
  }

  return %slapline
}
;---------------------------------------------------------------------------




;---------------------------------------------------------------------------
; say the slapline
alias ssp_sayslap {
  var %slapline = $1-

  ; new ability to have lines that perform actions
  if ( ( $left(%slapline , 1 ) == $chr(47) ) || ( $left(%slapline,1) == $chr(46) ) ) {
    ; execute an the line as if it were raw mirc command
    if ($mid(%slapline,2,1) == $chr(47) ) return
    ; evaluate all | separated things 
    var %eline
    var %count = 1
    while ($true) {
      ;      %eline = $gettok(%slapline , %count , 124)
      %eline = $gettok(%slapline , %count , 59)
      if (%eline == $null) break
      $eval( [ %eline ] , 1 )
      %count = %count + 1
    }
    return
  }

  if ($gotmircryption) {
    /eaction %slapline
  }
  else {
    /action %slapline
  }
}
;---------------------------------------------------------------------------





;---------------------------------------------------------------------------
; dialog for showing user their slap about to be applied
dialog -l slapdialog {
  title "Mouser's SuperSlapper.."
  size -1 -1 373 241
  option dbu
  edit "", 2, 2 211 369 17, multi result
  text "", 3, 3 202 200 8
  list 4, 2 15 283 190, vsbar
  list 5, 288 15 83 190, vsbar
  combo 6, 3 3 282 80, drop
  combo 7, 288 3 83 80, drop
  combo 8, 288 200 83 80, drop edit
  button "&Random", 200, 243 230 28 10
  button "&Send", 195, 273 230 28 10, ok
  button "&Send+Close", 190, 303 230 38 10, ok
  button "&Close", 210, 343 230 28 10, cancel
  button "Edit Current Slaplines File", 220, 2 230 75 10
  button "Edit Current Objects File", 230, 79 230 75 10
  button "Refresh Lists", 240, 156 230 45 10
}


on 1:dialog:slapdialog:init:*: {
  ; fill the file combos and select current files
  /slap_fillfilecombos

  ; fill channel combobox
  /slap_fillnickcombo

  ; fill buffeers with file contents
  /slap_filldialog

  ; select send button by default
  /did -f slapdialog 190

  ; autosend?
  if (%tmp_slapautosend == $true) {
    /dialog -k slapdialog
    /dialog -c slapdialog
  }
}


on 1:dialog:slapdialog:close:*: {
  ; free vars
  /unset %tmp_slapchan
  /unset %tmp_slapnick
}
;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
on 1:dialog:slapdialog:sclick:4: {
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:5: {
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:195: {
  ; send and close
  ; this is same as send+close, but we set flag saying reshow dialog
  %tmp_slapreshow = $true
  /dialog -k slapdialog
}

on 1:dialog:slapdialog:sclick:200: {
  ; re-randomize
  /ssp_chooserandomslapline
  var %slapobject = $did(slapdialog,5).seltext
  if ( (%slapobject != $cb) || ($cb == $null) ) {
    /ssp_chooserandomslapobject
  }
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:6: {
  ; user selects new slapline file
  %superslapper_slaplinefile = $did(slapdialog,6).seltext
  ; now update lists
  /slap_filldialog_lines
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:7: {
  ; user selects new slapobjects file
  %superslapper_objectfile = $did(slapdialog,7).seltext
  ; now update lists
  /slap_filldialog_objects
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:8: {
  ; change target nick
  %tmp_slapnick = $did(slapdialog,8).text
  ; now update
  /slapdialog_update
}

on 1:dialog:slapdialog:edit:8: {
  ; change target nick
  %tmp_slapnick = $did(slapdialog,8).text
  ; now update
  /slapdialog_update
}

on 1:dialog:slapdialog:sclick:220: {
  ; edit slaplines file - user will have to save and change slaplines object
  /run $scriptdir $+ $char(92) $+ %superslapper_slaplinefile

}

on 1:dialog:slapdialog:sclick:230: {
  ; edit slaplines file - user will have to save and change slaplines object
  /run $scriptdir $+ $char(92) $+ %superslapper_objectfile
}

on 1:dialog:slapdialog:sclick:240: {
  ; refresh lists from file
  /slap_filldialog
}
;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
alias slap_fillfilecombos {
  ; fill file combos
  var %count = 1
  var %dirlen = $len($scriptdir)
  var %rightlen
  var %filename
  /did -r slapdialog 6
  /did -r slapdialog 7

  ; add slaplines and select one
  %count = 1
  while ($true) {
    %filename = $findfile($scriptdir,*lines*.txt,%count)
    if (%filename == $null) break
    %rightlen = $len(%filename) - %dirlen
    %filename = $right(%filename,%rightlen)
    /did -a slapdialog 6 %filename
    if ((%count == 1) || (%filename == %superslapper_slaplinefile)) /did -c slapdialog 6 %count
    %count = %count + 1
  }

  ; add slaplines and select one
  %count = 1
  while ($true) {
    %filename = $findfile($scriptdir,*objects*.txt,%count)
    if (%filename == $null) break
    %rightlen = $len(%filename) - %dirlen
    %filename = $right(%filename,%rightlen)
    /did -a slapdialog 7 %filename
    if ((%count == 1) || (%filename == %superslapper_objectfile)) /did -c slapdialog 7 %count
    %count = %count + 1
  }

  ; now set current files
  %superslapper_slaplinefile = $did(slapdialog,6).seltext
  %superslapper_objectfile = $did(slapdialog,7).seltext
}

alias slap_filldialog {
  /slap_filldialog_lines
  /slap_filldialog_objects
  /slapdialog_update
}

alias slap_filldialog_lines {
  /loadbuf -ro slapdialog 4 " $+ $scriptdir $+ $char(92) $+ %superslapper_slaplinefile $+ "
  /did -f slapdialog 190

  ; re-randomize
  /ssp_chooserandomslapline
}

alias slap_filldialog_objects {
  /loadbuf -ro slapdialog 5 " $+ $scriptdir $+ $char(92) $+ %superslapper_objectfile $+ "

  if (%tmp_slapcb != $null) {
    /did -a slapdialog 5 %tmp_slapcb
    var %slapindex = $did(slapdialog,5).lines
    /did -c slapdialog 5 %slapindex
  }

  ; re-randomize
  var %slapobject = $did(slapdialog,5).seltext
  if ( (%slapobject != $cb) || ($cb == $null) ) {
    /ssp_chooserandomslapobject
  }
}


alias slap_fillnickcombo {
  ; fill nick combo
  /did -r slapdialog 8
  var %cname = $chan($active)
  var %count = 1
  var %thenick
  var %foundournick = $false
  while ($true) {
    %thenick = $nick(%cname,%count)
    if (%thenick == $null) break
    /did -a slapdialog 8 %thenick
    if ((%thenick == %tmp_slapnick) ) {
      %foundournick = $true
      /did -c slapdialog 8 %count
    }
    %count = %count + 1
  }
  if (%foundournick == $false) {
    ; our target nick is not found in channel, so create it in the list
    /did -a slapdialog 8 %tmp_slapnick
    /did -c slapdialog 8 %count    
  }
}


alias slapdialog_update {
  ; update the slapline display based on current selections
  var %slapline = $did(slapdialog,4).seltext
  var %slapobject = $did(slapdialog,5).seltext
  %slapline = $ssp_fillslapline(%slapline,%tmp_slapnick,%slapobject)
  /did -ra slapdialog 2 %slapline
  /did -ra slapdialog 3 In channel %tmp_slapchan $+ , $me ...
}
;---------------------------------------------------------------------------
