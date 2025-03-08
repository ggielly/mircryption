; ---------------------------------------------------------------------------
; MircryptionSuite - McpsFishDH addon v. 1.00.14 - dh key exchange
;
; see installation instructions at end of this readme.
;
; 06/01/03, mirc script by Dark Raichu, http://mircryption.sourceforge.net
; 12/05/03, Updated to use the new fish.dll which does 1024 bit keys and is *reportedly* safer.
; 12/20/03, Fixing some bugs that were preventing script from working if not in main mirc directory.
; 12/29/03, Added better error reporting.
; 02/02/04, updated to point to official fish site - thanks to author of fish
;           for informing me of its location and for making the fish source code publically available.
; 04/23/04, updated with new 1080 version of fish.dll
;           changed name of script.
; 05/24/04, message about old version was causing harmless annoying mirc error
; 01/18/05, adding dh support for cbc keys
; 01/22/05, added proper version reporting
; 01/24/05, added menu+switch for old style key exchange (may be useful for talking with dhfish eggdrops)
; 02/15/05, cleaned up a temp var, fixed rand chan key
; 11/04/05, fixed bug with multiserv nicks reporting no keyexchange in progress
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; IMPORTANT:
; This addon uses the fish.dll which was written by [anonymous]/
;  it comes with version 1.25 of fish, but you should go get the LATEST
;  fish.dll from the original fish author here:
;  ----> http://fish.sekure.us
; Grab the standalone fish dh download and copy the fish.dll into the McpsFishDH directory.
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; this mirc script (not the dll) is based on mirc code originally started by Sinner
;  and then continued by [anonymous]. the original credits section of blow.mrc:
;     "Big thanks go out to nutty for all his help
;      and to Joffi for his additional ideas
;      And even more thanks go out to mpyx, who helped me alot implementing
;      the secure keyXchange feature.
;      _SiNnEr_ 04-10-2003"
;
; Most of the work is done by the Fish.dll, which was written by [anonymous]
;  and uses code from the MIRACL C library.  thanks for making fish available to all of us.
; ---------------------------------------------------------------------------

;;---------------------------------------------------------------------------
;; IMPORTANT: [anonymous] has released the source code for fish(!) this is
;;  great news and means that the code can be validated if you have any
;;  question about it's integrity.  Still, USE THIS AT YOUR OWN RISK(!)
;;  ----> http://fish.sekure.us
;;---------------------------------------------------------------------------


;;---------------------------------------------------------------------------
;This addon is being provided to add compatibility between mircryption and
; blow, and to allow easier key exchange than mpgp.  Version 2 of mircryption
; will add it's own builtin secure key exchange that operates similarly.
;;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
Installation:
0. Unload and delete any old version of mircryption_SinnerFishDH.mrc, this is
an old version and will interfere with the new version if you unload it
/unload -rs mircryption_SinnerFishDH.mrc
1. Copy the McpsFishDH directory to your mirc directory
2. From inside mirc do /load -rs McpsFishDH/mcpsFishDH.mrc
3. See the new fish menu available when right clicking in a query window.
;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
Usage:
open a query window with someone, see new item in right click to exchange a key.
;---------------------------------------------------------------------------













;---------------------------------------------------------------------------
on *:START: {

  ; let user know the script is loaded  
  .mcdh_about
  %mc_sinnerscriptdll = " $+ $scriptdir $+ FiSH.DLL $+ "
}
;---------------------------------------------------------------------------


; about function tells version
alias mcdh_about {
  ; script version
  %mc_sinnerdhversion = 1.00.11
  if ($dialog(mcupdater_down).hwnd != $null) did -a mcupdater_down 7 McpsFishDH addon v. %mc_sinnerdhversion loaded and ready. $+ $crlf
  echo 7 -s MircryptionSuite - McpsFishDH addon v. %mc_sinnerdhversion loaded and ready.
}


;---------------------------------------------------------------------------
menu query {
  .Mircryption McpsFishDH 1080bit keyXchange ..... [/mckeyx $1] : mcsinnerdh.dhgetpubkey $1
  .Mircryption McpsFishDH 1080bit keyXchange (legacy ecb) ..... [/mckeyx $1 ecb] : mcsinnerdh.dhgetpubkey $1 ecb
}

menu nicklist {
  .Mircryption McpsFishDH 1080bit keyXchange ..... [/mckeyx $1] : mcsinnerdh.dhgetpubkey $1
  .Mircryption McpsFishDH 1080bit keyXchange (legacy ecb) ..... [/mckeyx $1] : mcsinnerdh.dhgetpubkey $1 ecb
}

menu channel {
  &Mircryption
  .More commands..
  ..McpsFishDH - Set Random Channel Key : rndkey $chan
}
;---------------------------------------------------------------------------








;---------------------------------------------------------------------------
alias mcsinnerdh.dhgetpubkey {
  mcsinnerdh.DH1080_INIT $1 $2-
  ; ATTN: halt not needed
  ;halt
}

alias mcsinnerdh.DH1080_INIT {
  if ( %mc_sinnerdh_disautokey == 1 ) { halt }
  set %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($1) ] 1
  var %tempkey $dll(%mc_sinnerscriptdll,DH1080_gen,NOT_USED)
  %mc_sinnerdh_prv_key = $gettok(%tempkey, 1, 32)
  %mc_sinnerdh_pub_key = $gettok(%tempkey, 2, 32)

  unset %tempkey
  if ( ($gotmircryptioncbc) && ($2 != ecb) ) {
    .notice $1 DH1080_INIT_cbc %mc_sinnerdh_pub_key
  }
  else {
    .notice $1 DH1080_INIT %mc_sinnerdh_pub_key
  }
  echo $color(Mode text) -tm $nick *** Sent my DH1080 public key to $1 $+ , waiting for reply ...
}

alias mckeyx {
  if ($1 == $null) {
    if ($$query($active)) { 
      mcsinnerdh.dhgetpubkey $active
    }
  }
  else {
    mcsinnerdh.dhgetpubkey $1
  }
}
;---------------------------------------------------------------------------








;---------------------------------------------------------------------------
; NEW FISH DH Exchange Routines (1080)

on ^1:NOTICE:DH1080_INIT*:?:{
  if ( %mc_sinnerdh_disautokey == 1 ) { halt }
  if ($len($2) == 180 || $len($2) == 181) {
    set %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ] 1
    echo $color(Mode text) -tm $nick *** Received DH1080 public key from $nick $+ ...
    var %tempkey $dll(%mc_sinnerscriptdll,DH1080_gen,NOT_USED)
    %mc_sinnerdh_prv_key = $gettok(%tempkey, 1, 32)
    %mc_sinnerdh_pub_key = $gettok(%tempkey, 2, 32)
    unset %tempkey

    ;/echo HEYDEBUG DH1080_INIT first word is $1

    var %secret = $dll(%mc_sinnerscriptdll,DH1080_comp, %mc_sinnerdh_prv_key $2)

    ; ATTN: set key for %nick to $secret (blow.setkey $nick %secret)
    var %retv
    if ( ( _cbc isin $1 ) && ($gotmircryptioncbc) ) {
      %secret = cbc: $+ %secret
      %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)
    }
    else {
      %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)
    }

    if (%retv != $null) {
      ; echo 4 -s %retv
      if ( ( _cbc isin $1 ) && ($gotmircryptioncbc) ) {
        echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange cbc complete: %retv
        .notice $nick DH1080_FINISH_cbc %mc_sinnerdh_pub_key
      }
      else {
        echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange complete: %retv
        .notice $nick DH1080_FINISH %mc_sinnerdh_pub_key
      }
    }
    else {
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange failed to set mircryption key.
      .notice $nick DH1080_FINISH ERROR
    }

    unset %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ]
    unset %mc_sinnerdh_prv_key
    unset %mc_sinnerdh_pub_key
    unset %secret
  }
  else {
    echo $color(Mode text) -tm $nick *** Improper DH1080 init from $nick, aborting exchange.
  }
  halt
}

on ^1:NOTICE:DH1080_FINISH*:?:{
  var %retv
  if ( %mc_sinnerdh_dh [ $+ [ $mcdh_cleanmultiserve($nick) ] ] != 1 ) {
    echo $color(Mode text) -tm $nick *** Received DH1080 finalization from $nick but no keyXchange is in progress - noaction taken.
    halt
  }

  ;/echo HEYDEBUG DH1080_FINISH first word is $1

  if ($len($2) == 180 || $len($2) == 181) {
    var %secret = $dll(%mc_sinnerscriptdll,DH1080_comp, %mc_sinnerdh_prv_key $2)

    ; ATTN: set key for %nick to $secret (blow.setkey $nick %secret)

    if ( ( _cbc isin $1 ) && ($gotmircryptioncbc) ) {
      %secret = cbc: $+ %secret
      %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)
    }
    else {
      %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)
    }

    if (%retv != $null) {
      if ( ( _cbc isin $1 ) && ($gotmircryptioncbc) ) {
        echo $color(Mode text) -tm $nick *** Mircryption Fish cbc keyXchange complete: %retv
      }
      else {
        echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange complete: %retv
      }
    }
    else {
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange failed to set mircryption key.
    }

    unset %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ]
    unset %mc_sinnerdh_prv_key
    unset %mc_sinnerdh_pub_key
    unset %secret
  }
  else {
    echo $color(Mode text) -tm $nick *** Improper DH1080 reply from $nick, aborting exchange.
    ;echo $color(Mode text) -tm one is $1 and 2 is $2 and len is $len($2)
  }
  halt
}
;---------------------------------------------------------------------------













;---------------------------------------------------------------------------
; OLD FISH DH Exchange Routines

on ^1:NOTICE:DH1024_INIT*:?:{
  var %retv
  if ( %mc_sinnerdh_disautokey == 1 ) { halt }
  if ($len($2) <= 172 && $len($2) >= 170) {
    set %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ] 1
    echo $color(Mode text) -tm $nick *** Received old DH1024 public key from $nick $+ ...
    .notice $nick Received old DH1024 public key from you! Please update to latest Fish version: http://fish.sekure.us , http://mircryption.sourceforge.net
    var %tempkey $dll(%mc_sinnerscriptdll,DH1024_gen,NOT_USED)
    %mc_sinnerdh_prv_key = $gettok(%tempkey, 1, 32)
    %mc_sinnerdh_pub_key = $gettok(%tempkey, 2, 32)
    unset %tempkey

    var %secret = $dll(%mc_sinnerscriptdll,DH1024_comp, %mc_sinnerdh_prv_key $2)

    ; ATTN: set key for %nick to $secret (blow.setkey $nick %secret)

    %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)
    if (%retv != $null) {
      ; echo 4 -s %retv
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange complete: %retv
      .notice $nick DH1024_FINISH %mc_sinnerdh_pub_key
    }
    else {
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange failed to set mircryption key.
      .notice $nick DH1024_FINISH ERROR
    }

    unset %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ]
    unset %mc_sinnerdh_prv_key
    unset %mc_sinnerdh_pub_key
    unset %secret
  }
  else {
    echo $color(Mode text) -tm $nick *** Improper DH1024 init from $nick, aborting exchange.
  }
  halt
}


on ^1:NOTICE:DH1024_FINISH*:?:{
  if ( %mc_sinnerdh_dh [ $+ [ $mcdh_cleanmultiserve($nick) ] ] != 1 ) {
    echo $color(Mode text) -tm $nick *** Received old DH1024 finalization from $nick but no keyXchange is in progress - noaction taken.
    halt
  }
  if ($len($2) <= 172 && $len($2) >= 170) {
    var %secret = $dll(%mc_sinnerscriptdll,DH1024_comp, %mc_sinnerdh_prv_key $2)

    ; ATTN: set key for %nick to $secret (blow.setkey $nick %secret)
    var %retv
    %retv = $dll( %mc_scriptdll  , mc_setkey , $mcdh_cleanmultiserve($nick) %secret)

    if (%retv != $null) {
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange complete: %retv
    }
    else {
      echo $color(Mode text) -tm $nick *** Mircryption Fish keyXchange failed to set mircryption key.
    }

    unset %mc_sinnerdh_dh $+ [ $mcdh_cleanmultiserve($nick) ]
    unset %mc_sinnerdh_prv_key
    unset %mc_sinnerdh_pub_key
    unset %secret
  }
  else {
    echo $color(Mode text) -tm $nick *** Improper DH1024 reply from $nick, aborting exchange.
  }
  halt
}
;---------------------------------------------------------------------------





;---------------------------------------------------------------------------
alias rndkey {
  var %cname = $1
  if (%cname == $null) %cname = $active
  %cname = $chatchan(%cname)
  var %tempkey $dll(%mc_sinnerscriptdll,DH1080_gen,NOT_USED)
  %mc_sinnerdh_prv_key = $gettok(%tempkey, 1, 32)
  %mc_sinnerdh_pub_key = $gettok(%tempkey, 2, 32)
  var %tempkey2 $dll(%mc_sinnerscriptdll,DH1080_gen,NOT_USED)
  %mc_sinnerdh_prv_key2 = $gettok(%tempkey2, 1, 32)
  %mc_sinnerdh_pub_key2 = $gettok(%tempkey2, 2, 32)
  var %secret = $dll(%mc_sinnerscriptdll,DH1080_comp, %mc_sinnerdh_prv_key2 %mc_sinnerdh_pub_key)
  setkey %cname cbc: $+ %secret
  /echo 4 %cname key for %cname randomized.
  return %secret
}
;---------------------------------------------------------------------------



;---------------------------------------------------------------------------
; just do a multiserv
alias mcdh_cleanmultiserve {
  return $cleanmultiserve($1)
}
;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
;end of file
