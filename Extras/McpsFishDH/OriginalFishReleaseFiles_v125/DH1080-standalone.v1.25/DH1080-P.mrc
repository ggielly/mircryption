;*******************
;* DH-1080 v1.25-P *
;*******************
; This is the standalone DH-1080 keyXchange
; script for very OLD blowcrypt scripts, which
; store the keys plain-text in the blow.ini!
;
; The DH routines are now located in FiSH.DLL
; and have been rewritten using MIRACL.




on 1:OPEN:?:{
  if (%autokeyx == [On]) {
    var %tmp1 = $readini blow.ini $nick key
    if (($len(%tmp1) > 0) || (+OK isin $1)) {
      FiSH.DH1080_INIT $nick
    }
  }
}



on ^1:NOTICE:DH1080_INIT*:?:{
  if ($len($2) == 180 || $len($2) == 181) {
    query $nick
    echo $color(Mode text) -tm $nick *** Received DH1080 public key from $nick $+ , sending mine...
    var %tempkey $dll(FiSH.DLL,DH1080_gen,NOT_USED)
    %FiSH.prv_key = $gettok(%tempkey, 1, 32)
    %FiSH.pub_key = $gettok(%tempkey, 2, 32)
    unset %tempkey
    var %secret = $dll(FiSH.DLL,DH1080_comp, %FiSH.prv_key $2)
    .notice $nick DH1080_FINISH %FiSH.pub_key
    FiSH.setkey $nick %secret
    unset %FiSH.prv_key
    unset %FiSH.pub_key
    unset %secret
  }
  halt
}


on ^1:NOTICE:DH1080_FINISH*:?:{
  if ( %FiSH.dh [ $+ [ $nick ] ] != 1 ) {
    echo "No keyXchange in progress!"
    halt
  }
  if ($len($2) == 180 || $len($2) == 181) {
    if ($len(%FiSH.prv_key) == 180 || $len(%FiSH.prv_key) == 181) {
      var %secret = $dll(FiSH.DLL,DH1080_comp, %FiSH.prv_key $2)
      FiSH.setkey $nick %secret
      unset %FiSH.dh $+ [ $nick ]
      unset %FiSH.prv_key
      unset %FiSH.pub_key
      unset %secret
    }
  }
  halt
}


; *** Old DH1024 detected ***
on ^1:NOTICE:DH1024_INIT*:?:{
  if ($len($2) <= 172 && $len($2) >= 170) {
    query $nick
    echo $color(Mode text) -tm $nick *** Received old DH1024 public key from $nick $+ , sending mine (and telling him to update)...
    .notice $nick Received old DH1024 public key from you! Please update to latest version: http://fish.sekure.us
    var %tempkey $dll(FiSH.DLL,DH1024_gen,NOT_USED)
    %FiSH.prv_key = $gettok(%tempkey, 1, 32)
    %FiSH.pub_key = $gettok(%tempkey, 2, 32)
    unset %tempkey
    var %secret = $dll(FiSH.DLL,DH1024_comp, %FiSH.prv_key $2)
    .notice $nick DH1024_FINISH %FiSH.pub_key
    FiSH.setkey $nick %secret
    unset %FiSH.prv_key
    unset %FiSH.pub_key
    unset %secret
  }
  halt
}



alias FiSH.setkey {
  if ($1 == /query) var %cur_contact = $active
  else var %cur_contact = $1
  if ($2- == $null) {
    FiSH.removekey %cur_contact
    halt
  }
  var %ky = $2-
  writeini -n blow.ini %cur_contact key %ky
  writeini -n blow.ini %cur_contact date $date
  unset %ky
  flushini mirc.ini
  if ( $window(%cur_contact) == $NULL) { 
    echo $color(Mode text) -at *** Key for %cur_contact set to *censored*
  }
  else {
    echo $color(Mode text) -tm %cur_contact *** Key for %cur_contact set to *censored*
  }
}


alias FiSH.showkey {
  if ($1 == /query) var %cur_contact = $active
  else var %cur_contact = $1
  flushini mirc.ini
  var %ky = $readini blow.ini %cur_contact key
  if ($len(%ky) > 4) {
    window -dCo +l @Blowcrypt-Key -1 -1 500 80
    aline @Blowcrypt-Key Key for %cur_contact :
    aline -p @Blowcrypt-Key %ky
    unset %ky
  }
  else {
    echo $color(Mode text) -at *** No valid key for %cur_contact found
  }
}


alias FiSH.removekey {
  if ($1 == /query) var %cur_contact = $active
  else var %cur_contact = $1
  remini "blow.ini" %cur_contact
  flushini mirc.ini
  echo $color(Mode text) -at *** Key for %cur_contact has been removed
}


alias FiSH.DH1080_INIT {
  if ($1 == /query) var %cur_contact = $active
  else var %cur_contact = $1
  set %FiSH.dh $+ [ %cur_contact ] 1
  var %tempkey $dll(FiSH.DLL,DH1080_gen,NOT_USED)
  %FiSH.prv_key = $gettok(%tempkey, 1, 32)
  %FiSH.pub_key = $gettok(%tempkey, 2, 32)
  unset %tempkey
  .NOTICE %cur_contact DH1080_INIT %FiSH.pub_key
  echo $color(Mode text) -tm $nick *** Sent my DH1080 public key to %cur_contact $+ , waiting for reply ...
}



menu query,nicklist {
  -
  DH-1080-P
  .keyXchange: FiSH.DH1080_INIT $1
  .Show key :FiSH.showkey $1
  .Remove key :FiSH.removekey $1
  .-
  .Auto-KeyXchange $+ $chr(32) $+ %autokeyx
  ..Enable :/set %autokeyx [On]
  ..Disable :/set %autokeyx [Off]
}
