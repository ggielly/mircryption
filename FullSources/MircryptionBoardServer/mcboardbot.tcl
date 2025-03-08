# ------------------------------------------------------------------------------------
# mcboardbot.tcl, v1.08.07, 2/14/05
# Tcl based interface to the mcboardbot.exe executable news board
#
# History
#  v1.08.03 changed the directories where it will run files
#  v1.08.07 Changed script load announce to be cleaner
#
# ToDo List
#  bypass mceggdrop if it is installed
# ------------------------------------------------------------------------------------


# ------------------------------------------------------------------------------------
# announce to shell when eggrop is started
putlog "McBoardBot TCL for Mircryption News Board v1.08.07 is loaded."
# ------------------------------------------------------------------------------------



# ------------------------------------------------------------------------------------
# register bot to get notified about topic changes
bind topc - * topic_changes

#register bot to get notified when user types !mcb to issue a news command
bind msg - !mcb msg_news_command

#register bot to get notified when user types !mcb to issue a news command
bind notc - !mcb msg_news_command

#register bot to get notified when user types !mcb to issue a news command
bind pub - !mcb pub_news_command
# ------------------------------------------------------------------------------------




# ------------------------------------------------------------------------------------
#arguments - fixed
set exename  "scripts/MircryptionBoardServer/mcboardbot.exe"
set optionsfilename  "scripts/MircryptionBoardServer/mcb_options.cfg"
# ------------------------------------------------------------------------------------
#arguments - based on message being processed
set argchan  ""
set argnick  ""
set argident ""
set argopstring "def"
# ------------------------------------------------------------------------------------
#argument - text message received (absent the initial !mcb)
set comstring "" 
# ------------------------------------------------------------------------------------



# ------------------------------------------------------------------------------------
# helper funcs from http://stage.caldera.com/Technology/tcl/Fragments.html
# convert integer to ascii char
proc asc i { 
    if { $i<0 || $i>255 } { error "asc:Integer out of range 0-255" } 
    return [format %c $i ] 
}

# or for asci \0 handling wher available 
if {[info tclversion]>=8.0} {
  proc chr {c} {
     binary scan $c c1 v
     return $v
  }
} else {
  proc chr {c} {
    if {[scan $c "%c" v]==1} { 
       return $v 
    } else {
       return 0 
    }
  }
}
# ------------------------------------------------------------------------------------


# ------------------------------------------------------------------------------------
# Shell the news command and pipe output to server
proc ShellNewsCommand {} {
    # allow access to global vars
    global exename
    global optionsfilename
    global argchan
    global argnick
    global argident
    global argopstring
    global comstring

    #ATTN:  2/12/03 -> test to make sure user is not passing us any characters
    # that could be used to confuse the exec command and make it overight files on
    # the server.  The details of this potential exploit is described here
    # http://forum.egghelp.org/viewtopic.php?t=2273, and below in proc mk_inputcheck
  
    #Note: i tried to see if the old version (without this test) was
    # vulnerable to this exploit and found that it was not - the > in strings were
    # not being passed to exec in a way that they were parsed by the OS.
    # You might want to disable this check if you use it on a channel which
    # contains characters like <,>,|, if you are sure that your tcl is not
    # vulnerable to this exploir.
    #if [mk_inputcheck "$exename $optionsfilename $argchan $argnick $argident $argopstring $comstring" ] {
    #      puthelp "notice $argnick :  Your command to the news bot could not be processed, because it contains one or more characters from the following set:  '|', '<', '>', '&'.  These characters present a security risk to the exec command and are not permitted." 
    #      return
    #      }

    # fix up evil ascii switch characters for bugfix - some shells/eggdrops/tcl seem to treat high ascii as negatives and then mess up the shell
   set len [string length $comstring]
   set index 0
   set newcomstring ""
   while {$index < $len} {
     set character [ string index $comstring $index ]
     set charnum [ chr $character ]
     # putlog "char $index is $charnum"
     if {$charnum == -69} {
          set character [ format %c 187 ];
          }
     if {$charnum == -85} {
          set character [ format %c 171 ];
          }
    append newcomstring "$character"
    incr index
    }
    set comstring $newcomstring


    #uncomment to send debug ingo to partyline for debuggin
    # putlog "newsboard -> exec $exename -e $optionsfilename $argchan $argnick $argident $argopstring $comstring"

    #shell the command
    set shellresult [ exec $exename -e $optionsfilename $argchan $argnick $argident $argopstring $comstring ]

    #split it into newline separated string list
    set shellresultLineList [split $shellresult "\n"]

    #uncomment to so we can tell if we got a response on partyline
    #putlog "received a response from newsboard."

    set linecount 0

    #show the list
    foreach i $shellresultLineList {

            if ($linecount==0) {
              # first line we send quickly so that multiple requests will see that their job is coming
              putquick "$i"
              #un comment below putlog to telnet into your bot and watch it reply
              #putlog "newsboard response line one -> $i"
            } else {
              #send the output directly into irc (dif. methods call dif. eggdrop output routines, for dif. speed but eggdrop is brainf'ckd so this dont help)
              # we use puthelp which is polite, and will wait behind putquick
              puthelp "$i"
              #putserv "$i"
              #putquick "$i"
            }
        }
}
# ---------------------------------------------------------------------------


# ---------------------------------------------------------------------------
# notes from string check prog. by Peter Postma <mail@ask.me> 
# 
# This function checks the whole string for false chars. 
# We need to do this when we're passing arguments from IRC to the TCL 
# exec command. Without checking which characters in the string are, 
# we create a huge security hole which can be easily exploited. 
# I'll explain this later. 
# 
# I found this hole in some of my own code. I audited all my code and also 
# the well known Qstat script, there are now alot versions available of 
# Qstat4eggdrop and all which I've seen are exploitable. 
# 
# Exploit: redirect the output with the '>' char to a file which can be 
# called whatever you want. It's possible to create and overwrite files. 
# Like Qstat4eggdrop's "!hl" command can be exploited with: 
#      !hl >test : it will create the file "test" with the output from !hl 
# 
# This is how it looks like in IRC: 
#   10:41 <Me> !hl localhost 
#   10:41 <Bot> Some server (localhost) 0/16 de_dust 10ms cstrike 
#   10:41 <Me> !hl >server.qstat 
#   10:41 <Me> !hl localhost 
#   (no response) 
#   10:42 <Me> !hl localhost 
#   (still nothing offcourse) 
# 
# The bot's config file got screwed up, cause of my 'corrupt' command. 
# Now that anyone is able to overwrite your config file, they also can 
# overwrite your documents, public html files or anything else. 
# 
# The redirection in the other direction "<" (read), seems to be safe, also 
# the pipe "|" character, but it always better to NOT allow these characters 
# to prevent possible errors/bugs or maybe exploits. 
# 
# Solution: Use the string_check function below. This function checks the 
# whole string for false characters, the chars which will be 
# checked are:  >  <  | 
# 
# The function returns a 1 when no false chars are found and a 0 when a false 
# character is found. Use the function like this: 
#   if {![string_check $text]} { return 0 } 
# 
# Tip: Be very carefull when calling the TCL exec command and passing 
# arguments to it directly from IRC. Without checking the argument will 
# create a easily exploited security hole. I've found this multiple times 
# in my scripts. A good idea is to use the function below which checks if 
# the string contains bad characters. 

proc mk_inputcheck {txt} { 
  foreach char {">" "<" "|" "&"} { 
    if [string match "*$char*" $txt] {return 1} 
  } 
  return 0 
}
# ------------------------------------------------------------------------------------





# ------------------------------------------------------------------------------------
proc topic_changes {tpnick uhost hand channel topic} {
    # allow access to global vars
    global exename
    global optionsfilename
    global argchan
    global argnick
    global argident
    global argopstring
    global comstring
    
    set argnick $tpnick
    set argchan $channel
    set argident $uhost
    set argopstring "d"
    set comstring "autoirctopic $topic"
    
    ShellNewsCommand
    return 0
}
# ------------------------------------------------------------------------------------


# ------------------------------------------------------------------------------------
proc msg_news_command {nick uhost hand args} {
    # allow access to global vars
    global exename
    global optionsfilename
    global argchan
    global argnick
    global argident
    global argopstring
    global comstring
        
    set argnick $nick
    set argchan "_NOCHAN_"
    set argident $uhost
    set argopstring "d"
    set comstring [join $args]
    
    ShellNewsCommand
    return 0
}
# ------------------------------------------------------------------------------------


# ------------------------------------------------------------------------------------
proc pub_news_command {nick uhost hand channel args} {
    # allow access to global vars
    global exename
    global optionsfilename
    global argchan
    global argnick
    global argident
    global argopstring
    global comstring
    
    set argnick $nick
    set argchan $channel
    set argident $uhost
    set argopstring "d"
    set comstring [join $args]

    ShellNewsCommand
    return 0
}
# ------------------------------------------------------------------------------------

