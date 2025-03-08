; 06/12/04
; mircryptified by koncool (http://anti-me.org/koncool)
; for mircryption (http://mircryption.sourceforge.net)
; thanks to great author of paste enhancements for 
;  super clean code which makes mircryptifying trivial.
;
; IMPORTANT -> YOU MUST MOVE THIS SCRIPT TO TOP (ABOVE MIRCRYPTION), OR SOME OF YOUR POSTS WILL BE DOUBLED
;

{ -------------------------------------------- {
 
    Please download  this script  only  from
    http://www.mircscripts.org
 
    There are a number  of sites around that
    take code without permission and host it
    for download.  If any of my scripts  are
    on one  of these other  sites,  I do not
    have  control  over the  code  nor  do I
    update it,  and the code  could possibly
    be modified or dangerous. 
 
    If you got this script from a site other
    than mircscripts.org, please re-download
    the most recent version from this url:
 
    http://www.mircscripts.org/users/myndzi/
 
} -------------------------------------------- }
 
; myndzi / #scripts-are-us @ DALnet
; Paste Enhancements v2.22
;
; "Scripting: pe"@ravaged.org || mrc-pe@ravaged.org
 
;thanks to:
; thecatisold@Dalnet/feedback+testing (i will spit acid in your eyes and blind you!)
; jINx@Dalnet/feedback
; Razer@Dalnet/testing
; dohcan/dialog studio (even if i don't quite like it yet :>)
; blue-elf/dialog tutorial
; Nfs/bug hunting
; qwerty/COM sendkeys
 
;no thanks to:
; ^Andy/whore!
 
 
; --event section-- ;
 
on *:load:{
  if ($version < 6.01) && (!$input(Warning! Paste Enhancements was designed for mIRC versions 6.01 and up, though it should also work on 5.91. You are using version $version $+ . The script may not function properly $+ $chr(44) or at all on this version. Do you wish to load it anyway?,264,Load Paste Enhancements...)) { if ($input(Would you like to visit the mIRC download site?,136,Update your mIRC?)) run http://www.mirc.com/get.html
    .unload -rs " $+ $script $+ "
    return
  }
  pe_readme
  .enable #pe_closeconfig
}
 
alias pemsg { 
  if ($gotmircryption != $true) { msg $1- }
  else { emsg $1- }
}
 
on *:unload:{
  .timer 1 0 if ($isfile( $scriptdirpe_reg.dat )) && ($input(You are unloading Paste Enhancements -- would you like to remove the settings file as well?,136,Unload Paste Enhancements...)) .remove " $+ $scriptdirpe_reg.dat $+ "
  if ($hget(pe_reg)) hfree pe_reg
  while ($window(@pe?_*,1)) { window -c $window(@pe?_*,1) }
}
 
on *:start:{
  if ($hget(pe_reg,force)) && ($script(1) != $script) .reload -rs1 " $+ $script $+ "
  if ($isfile($scriptdirpe_reg.dat)) { if (!$hget(pe_reg)) hmake pe_reg 10 | hload pe_reg " $+ $scriptdirpe_reg.dat $+ " }
  else pe_initreg
  hdel -w pe_reg !* | hdel -w pe_reg .*
  while ($hfind(pe_reg,@*,1,w)) hdel pe_reg $ifmatch
  .enable #pe1
  .disable #pe2
  if ($timer(oc) == $null) oc_chk
}
 
#pe_closeconfig off
on *:close:@pe:{
  .disable #pe_closeconfig
  .timer 1 0 pe_config
}
#pe_closeconfig end
 
on me:*:part:#:pe_pk
on *:kick:#:if ($knick == $me) pe_pk
 
on me:*:quit:pe_qd
on *:disconnect:pe_qd
 
on me:*:join:#:{
  var %w = @pec_ $+ $cid $+ , $+ $chan
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  if ($pe_inpaste($chan,$cid)) {
    if ($pe_prof(%p).resume) .timer 1 $pe_prof(%p).resumen pe_send %w
    else window -c %w
  }
}
 
raw 401:*:{
  var %w = @peq_ $+ $cid $+ , $+ $2
  if ($window(%w)) || ($hget(pe_reg,%w)) {
    .timer $+ %w off
    pe_clean %w
  }
}
 
#pe1 on
on *:input:#:{
  if ($timer(oc) == $null) oc_chk
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  if (%p != Disabled) && ($pe1($1-).c) halt
}
on *:input:?:{
  if ($timer(oc) == $null) oc_chk
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  if (%p != Disabled) && ($pe_prof(%p).query) && ($pe1($1-).q) halt
}
#pe1 end
 
#pe2 off
on *:input:*:if ($timer(oc) == $null) oc_chk | halt
#pe2 end
 
on *:input:*:if ($timer(oc) == $null) oc_chk
 
 
on *:signal:edit_change:if ($1 == StatusWindow) return | hadd -m pe_reg . $+ $1 $replace($editbox($1),$chr(32),$lf)
on *:signal:edit_clear:if ($1 == StatusWindow) return | if ($hget(pe_reg)) hdel pe_reg . $+ $1
on *:active:*:var %w = $remove($active,$chr(32)) | if ($hget(pe_reg,. $+ %w) != $editbox($active)) hdel pe_reg . $+ %w
 
on ^*:hotlink:*:*:{
  var %re = /80,69\d{1,2}\d{1,2}\d{1,2}(.*)/
  if (!$regex($hotline,%re)) halt
}
on *:hotlink:*:*:{
  window -k @Paste
  var %re = /(80,69\d{1,2}\d{1,2}\d{1,2})(.*)/, %nul = $regex($hotline,%re)
  %re = / $+ $regml(1) $+ (.*)/
  filter -cgww $target @Paste %re
  var %i = $line(@Paste,0)
  while (%i) {
    %nul = $regex($line(@Paste,%i),%re)
    rline @Paste %i $regml(1)
    dec %i
  }
}
 
; --main routines-- ;
 
alias -l pe1 {
  var %w = @pe $+ $prop $+ _ $+ $cid $+ , $+ $target
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  if ($inpaste) {
    .disable #pe1
    .enable #pe2
    set %pe_firstpaste $1-
    .timer 1 0 .disable #pe2 $chr(124) .enable #pe1 $chr(124) pe_check $target
    if ($hget(pe_reg,. $+ $active)) hadd -m pe_reg ! $+ $active $ifmatch
    return $true
  }
  elseif ($window(%w)) && ($istok(2.3,$pe_prof(%p).typed,46)) {
    if ($pe_prof(%p).typed == 2) iline %w 1 $1-
    if ($pe_prof(%p).typed == 3) aline %w $1-
    ;if ($hget(pe_reg,. $+ $active)) hadd -m pe_reg ! $+ $active $ifmatch
    return $true
  }
}
 
alias -l pe_check {
  var %ic = $iif($1 ischan,c,q), %w = @pe $+ %ic $+ _ $+ $cid $+ , $+ $1, %i = 1, %j = 1, %k = 0, %l, %t, %y, %z, %v, %w2 = $iif($window(%w),$false,$true), %c = $1
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default))), %meh = $2, %rc = 80,69 $+ $r(0,99) $+  $+ $r(0,99) $+  $+ $r(0,99) $+ 
 
  tokenize 32 $cb(1)
  if (%pe_firstpaste != $null) && ($right(%pe_firstpaste,$len($1-)) != $1-) {
    pemsg %c %pe_firstpaste
    return
  }
  unset %pe_firstpaste
 
  if ($pe_prof(%p).single) {
    while (%j <= $cb(0)) && (%k < 2) {
      if ($remove($cb(%j),$chr(9),$chr(32),$cr,$lf) != $null) {
        inc %k
        %l = $cb(%j)
      }
      inc %j
    }
    if (%k == 1) {
      if ($hget(pe_reg,! $+ $active)) { %l = $+($replace($ifmatch,$lf,$chr(32)),%l) | hdel pe_reg ! $+ $active }
      editbox -a %l
      return
    }
  }
  hdel pe_reg ! $+ $active
  if ($pe_prof(%p).ask) && (!$pe_prof(%p).askover) || (($pe_prof(%p).askover) && ($cb(0) > $pe_prof(%p).askovern)) {
    if (%ic == q) && ($pe_prof(%p).p2dcc) && ( (!$pe_prof(%p).p2dccif) || ($cb(0) > $pe_prof(%p).p2dccifn) ) goto skipask
    if ($pe_prof(%p).asktype == 1) { if (!$input(You are about to paste $cb(0) lines of text into %c $+ ... are you sure?,136,Paste Check...)) return }
    elseif ($pe_prof(%p).asktype == 2) && (%meh != $true) { .timer 1 0 pe_clip | return }
  }
  :skipask
  if ($window(%w) == $null) window -h %w
  while (%i <= $cb(0)) {
    %t = $cb(%i)
    if ($remove(%t,$chr(9),$chr(32),$cr,$lf) != $null) {
      if ($pe_prof(%p).spaces) && (($str($lf,2) isin $replace(%t,$chr(32),$lf)) || ($asc($left(%t,1)) == 32)) {
        %t = $replace(%t,$chr(32),$lf)
        %y = $pos(%t,$lf,0)
        while (%y) {
          %z = $mid(%t,$calc($pos(%t,$lf,%y) - 1),3)
          %v = $pos(%t,$lf,%y)
          if ($str($lf,2) $+ ? iswm %z) && ($right(%z,1) != $lf) {
            %t = $left(%t,%v) $+ $pe_codesat(%t,%v) $+ $mid(%t,$calc(%v + 1))
          }
          dec %y
        }
        while ($str($lf,2) isin %t) { %t = $replace(%t,$str($lf,2),$str( $+ $lf,2)) }
        if ($lf $+ * iswm %t) %t =  $mid(%t,2)
        %t = $replace(%t,$lf,$chr(32))
      }
 
      if ($pe_prof(%p).strip) {
        var %%s = $iif($pe_prof(%p).stripk,c) $+ $iif($pe_prof(%p).stripb,b) $+ $iif($pe_prof(%p).stripu,u) $+ $iif($pe_prof(%p).stripr,r)
 
 
        if ($pe_prof(%p).stripif) {
          var %%t = $pe_prof(%p).stripifn, %%i = $len(%%t)
          while (%%i) {
            if ($mid($pe_prof(%p).stripifn,%%i,1) isincs $chan(%c).mode) {
              %rc = $strip(%rc,%%s)
              %t = $strip(%t,%%s)
              break
            }
            dec %%i
          }
        }
        else %t = $strip(%t,%%s)
      }
 
      if ($pe_prof(%p).break) {
        %y = $pe_prof(%p).breakn
        while ($len(%t) > %y) {
          %z = $deltok($left(%t,%y),-1,32)
          if (%z == $null) { %z = $left(%t,%y) | %t = $mid(%t,$calc(%y + 1)) }
          else %t = $deltok(%t,1- $+ $numtok(%z,32),32)
          aline %w %rc $+ %z
        }
      }
      ;aline %w %t
    }
    elseif ($pe_prof(%p).blanks == 3) %t = 
    elseif ($pe_prof(%p).blanks == 2) %t = $lf
    else %t = ""
    if ($pe_prof(%p).blanks != 1) || (%t != $null) aline %w %rc $+ %t
    inc %i
  }
  hadd -m pe_reg %w $iif($hget(pe_reg,%w),$ifmatch,1)
  if (%ic == q) && ($pe_prof(%p).p2dcc) && ( (!$pe_prof(%p).p2dccif) || ($cb(0) > $pe_prof(%p).p2dccifn) ) pe_p2dcc %w
  elseif (%w2) pe_send %w
}
 
alias -l pe_send {
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  var %w = $1, %t = $mid(%w,4,1)
  if ($window(%w) == $null) { pe_clean %w | return }
  tokenize 44 $right(%w,-5)
  if ($server == $null) { if (!$pe_prof(%p).resume) pe_clean %w }
  elseif ($me ison $2) || (%t = q) {
    if ($line(%w,1) != $lf) pemsg $2 $line(%w,1)
    if ($line(%w,0) > 1) {
      hadd -m pe_reg %w $calc($iif($hget(pe_reg,%w),$ifmatch,1) + 1)
 
      if ($pe_prof(%p).spam) {
        var %%t = $pe_prof(%p).spamn, %%i = $len(%%t)
        while (%%i) {
          if ($mid(%%t,%%i,1) isincs $usermode) { .timer $+ %w -mo 1 0 pe_send %w | goto yuck }
          dec %%i
        }
      }
      if ($pe_prof(%p).slow) && ($hget(pe_reg,%w) > $pe_prof(%p).slown) .timer $+ %w -mo 1 $pe_prof(%p).slowto pe_send %w
      elseif ($pe_prof(%p).delay) .timer $+ %w -mo 1 $pe_prof(%p).delayn pe_send %w
      else .timer $+ %w -mo 1 0 pe_send %w
 
      :yuck
      dline %w 1
    }
    else pe_clean %w
  }
  elseif (%t = c) && (!$pe_prof(%p).resume) pe_clean %w
}
 
alias -l pe_p2dcc {
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  var %w = $1, %t = $mid(%w,4,1), %n = $gettok(%w,2,44), %to = $gettok($readini($mircini,options,n2),11,44)
  if ($chat(%n).status != active) {
    .timer 1 0 dcc chat %n
    hadd -mu $+ %to pe_reg p2dcc_ $+ %n %w
    .timer $+ %w -o 1 %to pe_clean %w
  }
  else {
    hdel pe_reg p2dcc_ $+ %n
    while ($line(%w,0)) {
      pemsg = $+ %n $line(%w,1)
      dline %w 1
    }
    .timer $+ %w off
    pe_clean %w
 
  }
}
on *:open:=:{
  var %w = $hget(pe_reg,p2dcc_ $+ $nick)
  if (%w) pe_p2dcc %w
}
 
 
alias -l pe_pk {
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  var %w = @pec_ $+ $cid $+ , $+ $chan
  if ($pe_prof(%p).resume) .timer $+ %w off
  elseif ($window(%w)) || ($hget(pe_reg,%w)) { .timer $+ %w off | pe_clean %w }
}
 
alias -l pe_qd {
  var %p = $iif($pe_chan($network,$chan),$ifmatch,$iif($pe_net($network),$ifmatch,$pe_net(Default)))
  if ($pe_prof(%p).resume) return
  var %w = @pe?_ $+ $cid $+ ,*
  var %i = $window(%w,0)
  while (%i) {
    pe_clean $window(%w,%i)
    dec %i
  }
}
 
alias -l pe_clean {
  window -c $1
  if ($hget(pe_reg,$1)) hdel pe_reg $1
}
 
;control codes at position N in text
alias -l pe_codesat {
  var %t = $1, %n = $2, %r, %u, %c1, %c2, %re = ^(\d{1,2})(,(\d{1,2}))?.*
  %t = $gettok($left(%t,%n),-1,15)
  if (%t == $null) return
  if ($pe_isodd($pos(%t,$chr(2),0))) %r = %r $+ $chr(2)
  if ($pe_isodd($pos(%t,$chr(22),0))) %r = %r $+ $chr(22)
  if ($pe_isodd($pos(%t,$chr(31),0))) %r = %r $+ $chr(31)
  if ($chr(3) isin %t) {
    %u = $replace($gettok(%t,-1,3),$chr(32),s)
    if ($regex(%u,%re)) { %c1 = $base($regml(1),10,10,2) | if ($regml(3)) %c2 = $base($ifmatch,10,10,2) }
    %r = %r $+ $chr(3) $+ $iif(%c1 != $null,$base(%c1,10,10,2)) $+ $iif(%c2 != $null,$chr(44) $+ $base(%c2,10,10,2))
  }
  return %r
}
 
alias -l pe_isodd if ($calc($1 % 2)) return $true | return $false
 
;$pe_net(N/name)
;/pe_net net [profile]
alias -l pe_net {
  if ($isid) { if ($1 isnum) return $gettok($hfind(pe_reg,(^Targets\\[^\\]*$),$1,r),-1,92) | return $hget(pe_reg,Targets\ $+ $1) }
  if ($0 < 1) return
  elseif ($0 < 2) { if ($hget(pe_reg)) hdel pe_reg Targets\ $+ $1 }
  else hadd -m pe_reg Targets\ $+ $1 $iif($pe_prof($2),$2,Default)
}
 
;$pe_chan(net,N/name)
;/pe_chan net chan [profile]
alias -l pe_chan {
  if ($isid) { if ($2 isnum) return $gettok($hfind(pe_reg,(^Targets\\ $+ $1\\# $+ [^\\]*$),$2,r),-1,92) | return $hget(pe_reg,Targets\ $+ $1\ $+ $2) }
  if (!$pe_net($1)) pe_net $1
  if ($0 < 2) return
  elseif ($0 < 3) { if ($hget(pe_reg)) hdel pe_reg Targets\ $+ $1\ $+ $2 }
  else hadd -m pe_reg Targets\ $+ $1\ $+ $2 $iif($pe_prof($3),$3,Default)
}
 
;$pe_prof(N/profile)[.prop]
;/pe_prof profile property [value]
alias -l pe_prof {
  if ($isid) { if ($1 isnum) return $gettok($hfind(pe_reg,(^Profiles\\[^\\]*$),$1,r),-1,92) | return $hget(pe_reg,Profiles\ $+ $1 $+ $iif($prop,\ $+ $prop)) }
  if ($0 < 1) return
  elseif ($0 == 1) {
    if (!$hget(pe_reg)) return
    while ($hfind(pe_reg,Profiles\ $+ $1\ $+ *,1,w)) hdel pe_reg $ifmatch
    hdel pe_reg Profiles\ $+ $1
  }
  else {
    if (!$hget(pe_reg,Profiles\ $+ $1)) hadd -m pe_reg Profiles\ $+ $1 $1
    hadd -m pe_reg Profiles\ $+ $1\ $+ $2 $3
  }
}
 
alias -l pe_netlist {
  var %i = 1, %j = 1, %t
  window -sh @pe_netsort
  clear @pe_netsort
  while ($pe_net(%i)) { if ($ifmatch != Default) aline @pe_netsort $ifmatch | inc %i }
 
  did -r $dname 121
 
  did -a $dname 121 Default
  did -a $dname 121
 
  %i = 1
  while ($line(@pe_netsort,%i)) {
    %t = $ifmatch
    did -a $dname 121 %t
 
    while ($pe_chan(%t,%j)) {
      did -a $dname 121 - $ifmatch
      inc %j
    }
 
    did -a $dname 121
    %j = 1
    inc %i
  }
  window -c @pe_netsort
 
  if ($hget(pe_reg,lasttarg)) did -c $dname 121 $didwm($dname,121,$ifmatch)
  else did -c $dname 121 1
}
 
alias -l pe_proflist {
  var %i = 1, %t
  %i = 1
  window -sh @pe_profsort
  clear @pe_profsort
  while ($pe_prof(%i)) { if ($ifmatch != Default) && ($ifmatch != Disabled) aline @pe_profsort $ifmatch | inc %i }
 
  did -r $dname 131
 
  did -a $dname 131 Default
 
  %i = 1
  while ($line(@pe_profsort,%i)) {
    did -a $dname 131 $ifmatch
    inc %i
  }
 
  did -a $dname 131 Disabled
  window -c @pe_profsort
}
 
alias -l pe_buttons {
  if ($did($dname,121).sel == 1) { if ($did($dname,123).enabled) did -b $dname 123 } | elseif (!$did($dname,123).enabled) did -e $dname 123
  if ($did($dname,131).sel == 1) || ($did($dname,131).sel == $did($dname,131).lines) { if ($did($dname,133).enabled) did -b $dname 133 } | elseif (!$did($dname,133).enabled) did -e $dname 133
  if ($hget(pe_reg,dirty)) { if (!$did($dname,202).enabled) did -e $dname 202 } | elseif ($did($dname,202).enabled) did -b $dname 202
}
 
alias -l pe_setprof {
  var %t = $did($dname,121).seltext, %i
  if (- * iswm %t) {
    %i = $did($dname,121).sel - 1
    while (- * iswm $did($dname,121,%i).text) dec %i
    %t = $pe_chan($did($dname,121,%i).text,$gettok(%t,2,32))
    did -c $dname 131 $didwm($dname,131,%t)
  }
  else did -c $dname 131 $didwm($dname,131,$pe_net(%t))
 
  pe_loadprof $did($dname,131).seltext
}
 
alias -l pe_loadprof {
  hadd -m pe_reg dirty $false
  if ($pe_prof($1).query) did -c $dname 10 | else did -u $dname 10
  if ($pe_prof($1).p2dcc) did -c $dname 15 | else did -u $dname 15
  if ($pe_prof($1).p2dccif) did -c $dname 16 | else did -u $dname 16
  if ($pe_prof($1).p2dccifn != $null) did -o $dname 17 1 $ifmatch | else did -r $dname 17
  if ($pe_prof($1).delay) did -c $dname 20 | else did -u $dname 20
  if ($pe_prof($1).delayn != $null) did -o $dname 21 1 $ifmatch | else did -r $dname 21
  if ($pe_prof($1).ask) did -c $dname 30 | else did -u $dname 30
  if ($pe_prof($1).asktype) did -c $dname 31 $ifmatch | else did -c $dname 31 1
  if ($pe_prof($1).askover) did -c $dname 33 | else did -u $dname 33
  if ($pe_prof($1).askovern != $null) did -o $dname 34 1 $ifmatch | else did -r $dname 34
  if ($pe_prof($1).slow) did -c $dname 40 | else did -u $dname 40
  if ($pe_prof($1).slown != $null) did -o $dname 41 1 $ifmatch | else did -r $dname 41
  if ($pe_prof($1).slowto != $null) did -o $dname 43 1 $ifmatch | else did -r $dname 43
  if ($pe_prof($1).break) did -c $dname 46 | else did -u $dname 46
  if ($pe_prof($1).breakn != $null) did -o $dname 47 1 $ifmatch | else did -r $dname 47
  if ($pe_prof($1).blanks) did -c $dname 51 $ifmatch | else did -c $dname 51 1
  if ($pe_prof($1).typed) did -c $dname 61 $ifmatch | else did -c $dname 61 1
  if ($pe_prof($1).resume) did -c $dname 70 | else did -u $dname 70
  if ($pe_prof($1).resumen != $null) did -o $dname 71 1 $ifmatch | else did -r $dname 71
  if ($pe_prof($1).single) did -c $dname 80 | else did -u $dname 80
  if ($pe_prof($1).spaces) did -c $dname 90 | else did -u $dname 90
  if ($pe_prof($1).strip) did -c $dname 100 | else did -u $dname 100
  if ($pe_prof($1).stripb) did -c $dname 101 | else did -u $dname 101
  if ($pe_prof($1).stripu) did -c $dname 102 | else did -u $dname 102
  if ($pe_prof($1).stripr) did -c $dname 103 | else did -u $dname 103
  if ($pe_prof($1).stripk) did -c $dname 104 | else did -u $dname 104
  if ($pe_prof($1).stripif) did -c $dname 105 | else did -u $dname 105
  if ($pe_prof($1).stripifn != $null) did -o $dname 106 1 $ifmatch | else did -r $dname 106
  if ($pe_prof($1).spam) did -c $dname 110 | else did -u $dname 110
  if ($pe_prof($1).spamn != $null) did -o $dname 111 1 $ifmatch | else did -r $dname 111
 
  pe_buttons
}
 
alias -l pe_profstring {
  if (!$pe_prof($1)) return
  pe_save $1
  clipboard /pe_addprofile $!$?="Profile name?" $pe_prof($1).query $pe_prof($1).delay " $+ $pe_prof($1).delayn $+ " $pe_prof($1).ask $pe_prof($1).askover " $+ $pe_prof($1).askovern $+ " $pe_prof($1).slow " $+ $pe_prof($1).slown $+ " " $+ $pe_prof($1).slowto $+ " $pe_prof($1).break " $+ $pe_prof($1).breakn $+ " $pe_prof($1).blanks $pe_prof($1).typed $pe_prof($1).resume " $+ $pe_prof($1).resumen $+ " $pe_prof($1).single $pe_prof($1).spaces $pe_prof($1).strip $pe_prof($1).stripb $pe_prof($1).stripu $pe_prof($1).stripr $pe_prof($1).stripk $pe_prof($1).stripif " $+ $pe_prof($1).stripifn $+ " $pe_prof($1).spam " $+ $pe_prof($1).spamn $+ " $pe_prof($1).asktype $pe_prof($1).p2dcc $pe_prof($1).p2dccif " $+ $pe_prof($1).p2dccifn $+ "
}
 
alias -l pe_save {
  pe_prof $1 query $did($dname,10).state
  pe_prof $1 p2dcc $did($dname,15).state
  pe_prof $1 p2dccif $did($dname,16).state
  pe_prof $1 p2dccifn $did($dname,17).text
  pe_prof $1 delay $did($dname,20).state
  pe_prof $1 delayn $did($dname,21).text
  pe_prof $1 ask $did($dname,30).state
  pe_prof $1 asktype $did($dname,31).sel
  pe_prof $1 askover $did($dname,33).state
  pe_prof $1 askovern $did($dname,34).text
  pe_prof $1 slow $did($dname,40).state
  pe_prof $1 slown $did($dname,41).text
  pe_prof $1 slowto $did($dname,43).text
  pe_prof $1 break $did($dname,46).state
  pe_prof $1 breakn $did($dname,47).text
  pe_prof $1 blanks $did($dname,51).sel
  pe_prof $1 typed $did($dname,61).sel
  pe_prof $1 resume $did($dname,70).state
  pe_prof $1 resumen $did($dname,71).text
  pe_prof $1 single $did($dname,80).state
  pe_prof $1 spaces $did($dname,90).state
  pe_prof $1 strip $did($dname,100).state
  pe_prof $1 stripb $did($dname,101).state
  pe_prof $1 stripu $did($dname,102).state
  pe_prof $1 stripr $did($dname,103).state
  pe_prof $1 stripk $did($dname,104).state
  pe_prof $1 stripif $did($dname,105).state
  pe_prof $1 stripifn $did($dname,106).text
  pe_prof $1 spam $did($dname,110).state
  pe_prof $1 spamn $did($dname,111).text
}
 
alias -l pe_initreg {
  .pe_addprofile Default 1 0 "2500" 1 1 "15" 1 "5" "3000" 1 "400" 1 1 0 "3" 1 1 1 0 0 0 1 1 "c" 1 "o" 1 1 1 "5"
  .pe_addprofile Strict 1 1 "1500" 1 1 "3" 1 "5" "5000" 1 "400" 1 1 0 "3" 1 1 1 1 1 1 1 0 "c" 0 "o" 1 1 1 "5"
  .pe_addprofile !mp3 1 1 "9999" 1 1 "3" 0 "5" "5000" 1 "400" 1 1 1 "3" 1 0 1 1 1 1 1 0 "c" 0 "o" 1 1 1 "5"
  .pe_addprofile Scripting 1 0 "1500" 1 1 "3" 1 "5" "5000" 0 "400" 3 3 0 "3" 1 1 1 0 0 0 1 1 "c" 1 "o" 2 1 1 "5"
  .pe_addprofile Casual 1 0 "1500" 1 1 "15" 1 "5" "3000" 1 "400" 3 1 1 "3" 1 1 0 0 0 0 0 0 "c" 1 "o" 1 1 1 "5"
  pe_net Default Default
  hsave -o pe_reg " $+ $scriptdirpe_reg.dat $+ "
}
 
 
; --public alias section-- ;
 
alias pe_config dialog -hm pe_settings pe_settings
alias pe_clip dialog -hm pe_clip pe_clip
 
alias pe_readme {
  if (!$isfile($scriptdirreadme.txt)) return $input(Readme.txt was not found in script path: $+ $cr $+ $scriptdir,260,Error)
  window -kf +betl @PE -1 -1 376 392 Lucida Console 12
  titlebar @PE - readme
  loadbuf @PE " $+ $scriptdirreadme.txt $+ "
  sline @PE 30
}
 
alias pe_inpaste {
  var %w = @pe $+ $iif($1 ischan,c,q) $+ _ $+ $2 $+ , $+ $1
  if ($window(%w)) return $true
  return $false
}
 
alias pe_addprofile {
  if ($show) && ($pe_prof($1)) && (!$input(The profile ' $+ $1 $+ ' already exists $+ $chr(44) overwrite?,yq,Confirmation)) return
 
  pe_prof $1 query $2
  pe_prof $1 delay $3
  pe_prof $1 delayn $remove($4,")
  pe_prof $1 ask $5
  pe_prof $1 askover $6
  pe_prof $1 askovern $remove($7,")
  pe_prof $1 slow $8
  pe_prof $1 slown $remove($9,")
  pe_prof $1 slowto $remove($10,")
  pe_prof $1 break $11
  pe_prof $1 breakn $remove($12,")
  pe_prof $1 blanks $13
  pe_prof $1 typed $14
  pe_prof $1 resume $15
  pe_prof $1 resumen $remove($16,")
  pe_prof $1 single $17
  pe_prof $1 spaces $18
  pe_prof $1 strip $19
  pe_prof $1 stripb $20
  pe_prof $1 stripu $21
  pe_prof $1 stripr $22
  pe_prof $1 stripk $23
  pe_prof $1 stripif $24
  pe_prof $1 stripifn $remove($25,")
  pe_prof $1 spam $26
  pe_prof $1 spamn $remove($27,")
  pe_prof $1 asktype $28
  pe_prof $1 p2dcc $29
  pe_prof $1 p2dccif $30
  pe_prof $1 p2dccifn $remove($31,")
}
 
; --nomorepasting.com-- ;
 
; myndzi 3/3/2004
alias nmp_paste {
  if ($window($active).type !isin channel.query) return
  if ($sock(nmp)) {
    echo -a * NMP Paste already in progress!
    return
  }
  window -hn @nmp
  clear @nmp
  sockopen nmp www.nomorepasting.com 80
  sockmark nmp $iif($1,$1,$active)
}
on *:sockopen:nmp:{
  if ($sockerr) return $nmp_err
  var %s = sockwrite -n nmp, %t, %u, %l = 0, %i = 1
  %t = action=pastecode&Category=8&Subject=Clipboard+Data&Paste=;+Paste+Enhancements+by+myndzi%3A+http%3A%2F%2Fwww%2Emircscripts%2Eorg%2Fcomments%2Ephp%3Fid%3D1295 $+ $str($crlf,2)
 
  while (%i <= $cb(0)) {
    %t = %t $+ $urlencode($replace($cb(%i),$chr(32),$lf) $+ $crlf).lfs
    while ($len(%t) > 450) {
      %u = $left(%t,450)
      aline @nmp %u
      inc %l $len(%u)
      %t = $mid(%t,451)
    }
    inc %i
  }
  aline @nmp %t
  inc %l $len(%t)
 
  %s POST /paste.php HTTP/1.1
  %s Host: www.nomorepasting.com
  %s Content-Type: application/x-www-form-urlencoded
  %s Content-Length: %l
  %s Connection: Close
  %s
}
on *:sockwrite:nmp:{
  if ($sockerr) return $nmp_err
  if ($sock(nmp).sq) || (!$window(@nmp)) return
  sockwrite -t nmp $line(@nmp,1)
  dline @nmp 1
  if ($line(@nmp,0) == 0) window -c @nmp
}
on *:sockread:nmp:{
  if ($sockerr) return $nmp_err
  var %t, %re = /<!--<p>Paste ID: (\d+)</p>-->/, %p
  :l
  sockread %t
  if ($sockbr) { 
    if ($regex(%t,%re)) pemsg $gettok($sock(nmp).mark,1,32) http://www.nomorepasting.com/paste.php?pasteID= $+ $regml(1)
    goto l
  }
}
on *:sockclose:nmp:if ($sockerr) return $nmp_err
alias -l nmp_err echo -ti2 $sock(nmp).mark * /nmp_paste: Error $sock(nmp).wserr on $event ( $+ $sock(nmp).wsmsg $+ )
 
alias -l urlencode {
  var %r, %i = $len($1), %t
  while (%i) {
    %t = $asc($mid($1,%i,1))
    %r = $iif($regex($chr(%t), /([0-9a-zA-Z])/), $regml(1), $iif(%t == 32, +, $+($chr(37), $base(%t, 10, 16, 2)))) $+ %r
    dec %i
  }
  if ($prop == lfs) %r = $replace(%r,% $+ 0D%0A,$lf,% $+ 0A,+,$lf,% $+ 0D%0A)
  return %r
}
 
; --menu section-- ;
 
menu menubar {
  Paste Enhancements
  .Config:pe_config
  .Unload:unload -rs " $+ $script $+ "
  .Readme/Help:pe_readme
}
 
menu channel,query {
  -
  $iif((!$pe_inpaste($active,$cid)),$style(2)) $+ Stop pasting:pe_clean @pe $+ $iif($active ischan,c,q) $+ _ $+ $cid $+ , $+ $active 
  Edit clipboard:pe_clip
  -
}
 
menu @Paste {
  Close:window -c $active
  Copy to clipboard:{
    clipboard $line(@paste,1) $+ $crlf
    dline @paste 1
    while ($line(@paste,1) != $null) {
      clipboard -a $ifmatch $+ $crlf
      dline @paste 1
    }
    window -c $active
  }
}
 
 
; --dialog section-- ;
; despite it all, i ended up doing this all myself.
; did i mention i despise writing dialogs?
 
dialog pe_settings {
  title "Settings - Paste Enhancements"
  size -1 -1 259 158
  option dbu
 
 
  tab "Basic Settings", 1, 95 17 160 122
 
  check "&Enable in query windows", 10, 100 33 151 11, tab 1
  check "Paste to a &DCC Chat", 15, 108 45 61 11, tab 1
  check "&If over", 16, 116 57 26 11, tab 1
  edit "", 17, 144 57 19 10, tab 1
  text "lines", 18, 165 59 14 8, tab 1
 
  check "&Paste one line every", 20, 100 69 61 11, tab 1
  edit "", 21, 160 69 19 10, tab 1
  text "ms (else paste immediately)", 22, 181 71 69 8, tab 1
 
  check "&when pasting", 30, 100 81 9 11, tab 1
  combo 31, 109 81 70 49, tab 1 size drop
  text "when pasting", 32, 181 83 37 8, tab 1
  check "&Over", 33, 108 93 22 11, tab 1
  edit "", 34, 132 93 19 10, tab 1
  text "lines", 35, 153 95 14 8, tab 1
 
  check "&Slow down pastes larger than", 40, 100 105 83 11, tab 1
  edit "", 41, 183 105 19 10, tab 1
  text "lines to", 42, 205 107 18 8, tab 1
  edit "", 43, 224 105 19 10, tab 1
  text "ms", 44, 245 107 7 8, tab 1
 
  check "&Break up lines longer than", 46, 100 117 73 11, tab 1
  edit "", 47, 174 117 19 10, tab 1
  text "characters", 48, 195 119 29 8, tab 1
 
 
  tab "Extra Settings", 2
 
  text "Blank lines in clipboard:", 50, 99 36 62 8, tab 2
  combo 51, 165 34 86 34, tab 2 size drop
 
  text "Text typed while pasting:", 60, 99 48 62 8, tab 2
  combo 61, 165 46 86 34, tab 2 size drop
 
  check "&Resume pasting", 70, 100 65 51 11, tab 2
  edit "", 71, 150 65 19 10, tab 2
  text "seconds after reconnect/rejoin", 72, 172 67 77 8, tab 2
 
  check "P&aste single line to editbox", 80, 100 77 128 11, tab 2
 
  check "Keep &spaces in pastes", 90, 100 89 128 11, tab 2
 
  check "Strip codes from pastes:", 100, 100 101 72 11, tab 2
  check "B", 101, 193 101 15 11, tab 2
  check "U", 102, 208 101 15 11, tab 2
  check "R", 103, 223 101 15 11, tab 2
  check "K", 104, 238 101 15 11, tab 2
  check "If one of these channel modes is set:", 105, 108 113 105 11, tab 2
  edit "", 106, 213 113 32 10, tab 2
 
  check "No delay if one of these umodes is set:", 110, 100 125 105 11, tab 2
  edit "", 111, 213 125 32 10, tab 2
 
 
  text "Targets:", 120, 4 6 23 8
  list 121, 3 15 88 125, size
  button "Add", 122, 48 4 20 10
  button "Del", 123, 70 4 20 10
 
  text "Profile:", 130, 96 6 21 8
  combo 131, 120 4 90 44, size drop
  button "Add", 132, 213 4 20 10
  button "Del", 133, 235 4 20 10
 
  check "Force load as first script", 140, 3 145 70 11
 
  button "OK", 200, 95 143 37 12, default ok
  button "Cancel", 201, 136 143 37 12, cancel
  button "Apply", 202, 218 143 37 12
  button "Copy", 203, 177 143 37 12
}
 
dialog pe_newtarg {
  title "New Target - Paste Enhancements"
  size -1 -1 128 59
  option dbu
 
  text "Create new:", 10, 4 4 31 8, right
  radio "Network", 11, 39 2 32 11
  radio "Channel", 12, 39 12 31 11
 
  combo 13, 74 12 51 38, size drop
 
  text "Name:", 20, 4 29 31 8, right
  edit "", 21, 38 27 87 11
 
  button "OK", 100, 26 44 37 12, default ok
  button "Cancel", 101, 66 44 37 12, cancel
}
 
dialog pe_clip {
  title "Clipboard - Paste Enhancements"
  size -1 -1 196 147
  option dbu
 
  menu "&Adjustments", 1
  menu "&Strip", 2, 1
  item "&All", 3, 2
  item break, 4, 2
  item "&Color", 5, 2
  item "&Bold", 6, 2
  item "&Underline", 7, 2
  item "Re&verse", 8, 2
  item "&Reset", 9, 2
  item "&Fix spaces", 10, 1
  item "Strip &tokens...", 11, 1
 
  edit "", 12, 0 1 196 131, multi return autohs autovs hsbar vsbar
 
  button "&Paste", 14, 1 134 37 12
  button "&Apply", 15, 40 134 37 12
  button "&NMP.COM", 16, 79 134 37 12
  combo 17, 118 135 37 130, size drop
  button "&Close", 18, 157 134 37 12, default cancel
}
 
on *:dialog:pe_clip:init:*:{
  did -a $dname 17 *.NET
  did -a $dname 17 ASP
  did -a $dname 17 Assembler
  did -a $dname 17 C/C++
  did -a $dname 17 ColdFusion
  did -a $dname 17 Delphi
  did -a $dname 17 Java
  did -a $dname 17 mIRC Script
  did -a $dname 17 Other
  did -a $dname 17 Perl
  did -a $dname 17 PHP
  did -a $dname 17 SQL
  did -a $dname 17 Visual Basic
  did -a $dname 17 Python
  did -a $dname 17 TCL
  did -a $dname 17 Javascript
 
  did -c $dname 17 $iif(%nmp_type,%nmp_type,9)
 
  sendkeys ^v
}
on *:dialog:pe_clip:edit:12:did -e $dname 15
on *:dialog:pe_clip:menu:11:{
  var %i = 1, %x = $did($dname,12).lines, %n = $input(Strip how many tokens from each line?,e,Input request,1), %u
  dialog -v $dname
  if (%n !isnum) return | inc %n
  while (%i <= %x) {
    %u = $did($dname,12,%i).text
    if (%i == %x) && (%u == $null) break
    %u = $gettok(%u,%n $+ -,32)
    did -o $dname 12 %i %u
    inc %i
  }
  did -e $dname 15
}
 
on *:dialog:pe_clip:menu:3:pe_clipstrip burco
on *:dialog:pe_clip:menu:5:pe_clipstrip c
on *:dialog:pe_clip:menu:6:pe_clipstrip b
on *:dialog:pe_clip:menu:7:pe_clipstrip u
on *:dialog:pe_clip:menu:8:pe_clipstrip r
on *:dialog:pe_clip:menu:9:pe_clipstrip o
 
alias -l pe_clipstrip {
  var %i = 1, %x = $did($dname,12).lines, %t = $remove($1,o), %u
  dialog -v $dname
  if (%n !isnum) return | inc %n
  while (%i <= %x) {
    %u = $did($dname,12,%i).text
    if (%i == %x) && (%u == $null) break
    if (%t) %u = $strip(%u,%t)
    if (o isin $1) %u = $remove(%u,)
    did -o $dname 12 %i %u
    inc %i
  }
  did -e $dname 15
}
 
on *:dialog:pe_clip:menu:10:{
  var %t, %y, %z, %v, %i = 1
  did -r $dname 12
  while (%i <= $cb(0)) {
    %t = $replace($cb(%i),$chr(32),$lf)
    %y = $pos(%t,$lf,0)
    while (%y) {
      %z = $mid(%t,$calc($pos(%t,$lf,%y) - 1),3)
      %v = $pos(%t,$lf,%y)
      if ($str($lf,2) $+ ? iswm %z) && ($right(%z,1) != $lf) {
        %t = $left(%t,%v) $+ $pe_codesat(%t,%v) $+ $mid(%t,$calc(%v + 1))
      }
      dec %y
    }
    while ($str($lf,2) isin %t) { %t = $replace(%t,$str($lf,2),$str( $+ $lf,2)) }
    if ($lf $+ * iswm %t) %t =  $mid(%t,2)
    %t = $replace(%t,$lf,$chr(32))
    did -a $dname 12 %t $+ $crlf
    inc %i
  }
  did -e $dname 15
}
on *:dialog:pe_clip:sclick:14:{
  did -f $dname 12
  sendkeys ^{HOME}+^{END}^c
  .timer 1 0 dialog -c $dname $chr(124) pe_check $active $true
}
on *:dialog:pe_clip:sclick:15:{
  var %s = $did($dname,12).selstart, %e = $did($dname,12).selend
  did -f $dname 12
  sendkeys ^{HOME}+^{END}^c^{END}
  .timer 1 0 did -c $dname 12 1 %s %e
}
on *:dialog:pe_clip:sclick:16:nmp_paste $active | %nmp_type = $did($dname,17).sel | dialog -c $dname
 
on *:dialog:pe_settings:init:*:{
  if (!$hget(pe_reg)) pe_initreg
 
  var %i = 1, %j = 1, %t
 
  pe_netlist
  pe_proflist
 
  did -a $dname 31 Display warning dialog
  did -a $dname 31 Open Clipboard Editor
 
  did -a $dname 51 Discard, move on to next line
  did -a $dname 51 Pause, don't send blank line
  did -a $dname 51 Send CTRL+O
 
  did -a $dname 61 Send immediately
  did -a $dname 61 Send as next line in paste
  did -a $dname 61 Send after paste is complete
 
  if ($hget(pe_reg,force)) did -c $dname 140 | else did -u $dname 140
  if ($hget(pe_reg,force)) && ($script(1) != $script) .reload -rs1 " $+ $script $+ "
 
  pe_setprof
 
  .timer 1 0 did -f $dname $iif($hget(pe_reg,lasttab),$ifmatch,1)
}
on *:dialog:pe_settings:sclick:121:{
  var %t = $did($dname,121).sel
  while ($did($dname,121,%t).text == $null) dec %t
  if (%t != $did($dname,121).sel) did -c $dname 121 %t
  hadd -m pe_reg lasttarg $did($dname,121).seltext
  pe_setprof
  pe_buttons
}
on *:dialog:pe_settings:sclick:131:{
  pe_loadprof $did($dname,131).seltext
 
  if ($did($dname,131).seltext == Disabled) did -b $dname 10,20,21,30,31,32,40,41,43,46,47,51,61,70,71,80,90,100,101,102,103,104,105,106,110,111
  elseif (!$did($dname,1).enabled) did -e $dname 10,20,21,30,31,32,40,41,43,46,47,51,61,70,71,80,90,100,101,102,103,104,105,106,110,111
 
  var %t = $did($dname,121).seltext, %u, %i
  if (- * iswm %t) {
    %u = $gettok(%t,2,32)
    %i = $did($dname,121).sel - 1
    while (- * iswm $did($dname,121,%i).text) dec %i
    pe_chan $did($dname,121,%i).text %u $did($dname,131).seltext
  }
  else pe_net %t $did($dname,131).seltext
}
 
on *:dialog:pe_settings:sclick:10,15,16,20,30,31,33,40,46,51,61,70,80,90,100,101,102,103,104,105,110:hadd -m pe_reg dirty $true | pe_buttons
on *:dialog:pe_settings:edit:17,21,34,41,43,47,71,106,111:hadd -m pe_reg dirty $true | pe_buttons
 
on *:dialog:pe_settings:sclick:1,2:hadd -m pe_reg lasttab $did
 
on *:dialog:pe_settings:sclick:200,202:{
  hadd -m pe_reg force $did($dname,140).state
  if ($hget(pe_reg,dirty)) pe_save $did($dname,131).seltext
  if ($did == 202) { hadd -m pe_reg dirty $false | pe_buttons }
 
  hsave -o pe_reg " $+ $scriptdirpe_reg.dat $+ "
}
 
on *:dialog:pe_settings:sclick:203:pe_profstring $did($dname,131).seltext
 
on *:dialog:pe_settings:sclick:122:var %t = $dialog(pe_newtarg,pe_newtarg) | pe_netlist
on *:dialog:pe_settings:sclick:123:{
  if (- * iswm $did($dname,121).seltext) {
    if (!$input(Are you sure you want to delete the channel ' $+ $gettok($did($dname,121).seltext,2,32) $+ '?,yq,Confirmation)) return
    var %i = $did($dname,121).sel - 1
    while (- * iswm $did($dname,121,%i).text) dec %i
    pe_chan $did($dname,121,%i).text $gettok($did($dname,121).seltext,2,32)
  }
  else {
    if (!$input(Are you sure you want to delete the network ' $+ $did($dname,121).seltext $+ ' and all channels under it?,yq,Confirmation)) return
    var %t = $did($dname,121).seltext
    while ($pe_chan(%t,1)) pe_chan %t $ifmatch
    pe_net %t
  }
  hadd -m pe_reg lasttarg Default
  pe_netlist
}
 
on *:dialog:pe_settings:sclick:132:{
  var %t = $strip($remove($$input(Enter a name for the new profile:,e),\,$chr(32))), %u, %v, %i
  if ($pe_prof(%t)) return $input(Sorry $+ $chr(44) that name is already in use!,ow,D'oh!)
  pe_save %t
  pe_proflist
 
  %u = $did($dname,121).seltext
  if (- * iswm %u) {
    %v = $did($dname,121).sel - 1
    while (- * iswm $did($dname,121,%v).text) dec %v
    pe_chan %v $gettok(%u,2,32) %t
  }
  else pe_net %u %t
  did -c $dname 131 $didwm($dname,131,%t)
 
  pe_buttons
}
on *:dialog:pe_settings:sclick:133:{
  var %i = 0, %n = 1, %c = 1, %t = $did($dname,131).seltext, %u, %v
  if (!$input(Are you sure you want to delete the profile ' $+ %t $+ '?,yq,Confirmation)) return
  while ($pe_net(%n)) {
    %u = $ifmatch
    if ($pe_net(%u) == %t) { inc %i | pe_net %u Default }
    while ($pe_chan(%u,%c)) {
      if ($pe_chan(%u,$ifmatch) == %t) { inc %i | pe_chan %u $pe_chan(%u,%c) Default }
      inc %c
    }
    %c = 1
    inc %n
  }
  pe_prof %t
  pe_proflist
 
  %u = $did($dname,121).seltext
  if (- * iswm %u) {
    %v = $did($dname,121).sel - 1
    while (- * iswm $did($dname,121,%v).text) dec %v
    did -c $dname 131 $didwm($dname,131,$pe_chan($did($dname,121,%v).text,$gettok(%u,2,32)))
  }
  else did -c $dname 131 $didwm($dname,131,$pe_net(%u))
  pe_buttons
 
  return $input(Profile deleted. %i targets were reset to the Default profile.,o)
}
 
on *:dialog:pe_newtarg:init:*:{
  var %t, %u = $did(pe_settings,121).sel
 
  while (- * iswm $did(pe_settings,121,%u).text) dec %u
  %u = $did(pe_settings,121,%u)
 
  did -c $dname 11
 
  var %i = $did(pe_settings,121).lines
  while (%i) {
    %t = $did(pe_settings,121,%i).text
    if (- * !iswm %t) && (%t != $null) && (%t != Default) did -i $dname 13 1 %t
    dec %i
  }
 
  if ($didwm($dname,13,$network) == 0) { did -o $dname 21 1 $network | did -c $dname 13 1 | did -b $dname 13 }
  elseif ($active ischan) && ($didwm(pe_settings,121,- $active) == 0) { did -o $dname 21 1 $active | did -c $dname 13 $didwm($dname,13,$network) | did -c $dname 12 | did -u $dname 11 }
  else { did -c $dname 13 $didwm($dname,13,%u) | did -b $dname 13 }
}
 
on *:dialog:pe_newtarg:sclick:11,12:{
  if ($did($dname,12).state) did -e $dname 13
  else did -b $dname 13
}
on *:dialog:pe_newtarg:sclick:100:{
  if ($did($dname,11).state) pe_net $strip($remove($did($dname,21).text,\,$chr(32))) $did(pe_settings,131).seltext
  else pe_chan $did($dname,13).seltext $strip($remove($did($dname,21).text,\,$chr(32))) $did(pe_settings,131).seltext
}
 
 
; compatibility
alias -l cid if ($isid) return pre6
 
 
alias oc_chk {
  .timeroc -oim 1 10 if ($isalias(oc_chk)) oc_chk
  var %e = $editbox($active), %a = $remove($active,$chr(32)), %w = $window($active).wid
  if ($hash($editbox($active),32) != $hget(oc,%w)) {
    if (%e == $null) {
      hdel oc $window($active).wid
      .signal edit_clear %a
    }
    else {
      hadd -m oc %w $hash($editbox($active),32)
      .signal edit_change %a
    }
  }
}
 
; courtesy of qwerty, the COM guy!
alias sendkeys {
  var %a = sendkeys $+ $ticks
  .comopen %a WScript.Shell
  if !$comerr {
    var %b = $com(%a,SendKeys,3,bstr,$1-)
    .comclose %a
    return %b
  }
  return 0
}
