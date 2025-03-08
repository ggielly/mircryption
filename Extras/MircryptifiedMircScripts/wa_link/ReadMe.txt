
                                                                                   
    The WA Link v1.2 mIRC Addon is a bASE crew product (03.27.2002)                
    Offical Site - http://www.base.is/thecrew/ (New versions come here first)      
                                                                                   


1. What is WA Link?
2. What does WA Link do?
3. How does WA Link work?
4. Why is it better then other Winamp Stats DLL's?
5. How to install?
6. How to say my winamp status on a channel or in a query?
7. How to control winamp from mIRC?
8. I want to display a different theme.
9. I want to create my own theme!
10. I want help with my theme and/or WA_Link!
11. I want to report a bug, add to the mIRC scripting or DLL part or come with ideas.
12. I'm paranoid for backdoors, are there any in WA_Link?
13. What is bASE and the bASE crew?
14. Version history.

Credits, special thanks and copyright, see bottom of this file

If you need help with something not explained in this file or in the mIRC help
contact us at: base@base.is or come to our channel. (See bottom of this file)



1. What is WA Link?

   This is a mIRC-Addon to ease communication with Winamp through mIRC.

2. What does WA Link do?

   This Addon let's you show your current Winamp status and track info on a channel 
   or in a query in any format you see fit, plus it can control Winamp from mIRC. 
   Also with another addon script called WA_Remote you can control Winamp through mIRC 
   remotely. 

3. How does WA Link work?

   This Addon uses a mIRC DLL. DLL's are enhancements to mIRC, they add new features 
   to mIRC, this Addon uses "WA_Link v1.2 DLL" made by the bASE crew.

4. Why is it better then other Winamp Stats DLL's?

   WA_Link reads both ID3v1 and ID3v2 mp3 tags it also reads title and artists from
   all other formats which winamp can play. Plus you can in a simple way 
   change the format which the current track info is displayed or just use some 
   of the default format themes which are included. WA_Link can also control all 
   basic winamp functions such as "Stop" and "Play" track. WA_Link doesn't have
   to be installed as Winamp plugin like many other amp stats. This and many other 
   things make WA Link one of the best Winamp stats/controller available for mIRC.

   "This DLL is very good, download it today if you prefer Winamp over mIRC's 
    icky /splay (Like me)" - Rated 10/10 by GrimZ @ scriptaz.com

5. How to install?

   We recomend newest version of mIRC and Winamp (Not Beta 3). 
   Although it might work on older versions. 
   The latest mIRC as of this writing: 6.01 (Does work on 5.91)
   The latest non-beta WinAMP as of this writing: 2.78 (Must be installed properly)

   NOTE: WA_LINK DOES NOT WORK FULLY WITH WINAMP 3 BETA!!! 

   For new users (Users who don't have WA Link 1.0 or 1.1)
   - Unzip the zipfile to your mIRC dir, a subfolder called "\WA_LINK\" should be 
   - automatically created within your mIRC dir while unzipping. If not create the 
   - folder and place the appropriate files there. In the "\WA_Link\Themes" folder
   - are several INI files, these are the themes.

   - After the files have been unzipped in the folder WA_Link under your mIRC 
   - folder, you fire up mIRC and type: "/load -rs WA_Link\WA_Link.mrc" This 
   - will load the WA_Link mIRC script into mIRC. Afterward you may delete 
   - WA_Link.zip from your mIRC dir.

   For WA Link 1.0 or 1.1 users (Users who already have 1.0 or 1.1 and would like to upgrade to 1.2)
   - Close mIRC! Delete \WA_Link\ in your mIRC folder (including \WA_Link\Themes\, backup your custom themes first).
   - Then unzip the new WA Link zipfile to your mIRC dir, a subfolder called "\WA_LINK\" 
   - should be automatically created within your mIRC dir while unzipping. If not create the 
   - folder and place the appropriate files there. In the "\WA_Link\Themes\" folder
   - are several INI files, these are the themes. 
   - Then run mIRC and enjoy the new version of WA_Link. (No need to reload the script)

   - NOTE: If you don't want to close mIRC while updating WA Link you can also just 
   - reload the script ("/reload -rs WA_Link\WA_Link.mrc") after copying the files.

   NOTE: DLL locking must be disabled for this addon to work.
   Go to the options dialog, open the general->lock tab and uncheck the /dll 
   disable checkbox. /run command must be enabled as well. (Needed to add mp3 
   in Winamp playlist from mIRC)
   
   NOTE: Also we recommend you set you Winamp on "Read titles on Load
   Go to the Winamp Preference dialog, open up Options and check on Load. If you
   do not that, WA Find and Playlist functions might not work 100% since the 
   winamp default playlist might not be fully loaded.

   NOTE: Themes from WA Link 1.0 or 1.1 do not work unmodified with WA Link 1.2
   check section "9. I want to create my own theme!" for 1.2 theme format

   Files in the WA_Link directory after installation of WA_Link 1.2 should be:
     ReadMe.txt     - This file
     WA_Link.dll    - The dynamic link library, containing all the juice
     WA_Link.mrc    - The mIRC script
     Developers.txt - Documentation for those who want to modify the WA_Link mIRC script or DLL
     /Themes        - Subfolder containing various themes (*.ini)
                    - 13 themes are included by default (For more themes check our 
                    - website http://www.base.is/thecrew/)
     /DLL_Source    - The DLL source (Zipped) 
                             
6. How to say my Winamp status on a channel or in a query?

   After the script has been loaded, a "WA_Link" option is added to query and channel
   popup menuses, from there you can use this addon, right click, choose 
   WA Link -> WA Link Stats.  
   Note. You can also use the "/wa stats" alias.

7. How to control winamp from mIRC?

   After the script has been installed, a "WA Link" option is added to query and channel 
   popup menuses, from there you can use this addon. Right click and choose 
   WA Link -> WA Tracks, WA Volume or WA Controls and under that choose your command.
   You can also type "/wa <command> [value]" (Only a few commands need a value) to use it.

   To get the command list in mIRC, type "/wa help" in mIRC and the command list will 
   appear in then status window.

   NOTE: Commands are NOT case sensitive
   
   Available commands for WA_Link v1.2:
     STATS ................ Shows info about current playing track
     FIND X ............... X is a keyword to search for in playlist, if found WA_Link starts playing it
     JUMP X................ X is a number in the playlist and WA_Link will jump to that track number and start playing
     NEXT ................. Plays next track in playlist
     PREV ................. Plays previous track in playlist
     PAUSE ................ Toggles pause/play
     PLAY ................. Starts or resumes playing
     STOP ................. Stops the playing
     BOFPL ................ Plays the first track in the playlist
     EOFPL ................ Plays the last track in the playlist
     FADESTOP ............. Fades to a stop
     FFW5 ................. Fast-forwards 5 seconds
     REW5 ................. Rewinds 5 seconds
     SEEK X ............... X is a seek value in the range of 0 to 100 (%)
     VOL X ................ X is a volume value in the range of 0 to 100 (%)
     PAN X ................ X is a value ranging from -100 to 100. -100 is left, 0 is center, 100 is right
     SHUFFLE .............. Toggles shuffle/random play on/off
     REPEAT ............... Toggles repeat play on/off
     MINIMIZE ............. Minimizes Winamp (NOT the same as HIDE)
     RESTORE .............. Restores a minimized Winamp (NOT the same as SHOW)
     HIDE ................. Hides a shown Winamp (NOT the same as minimize. Does NOT hide the playlist)
     SHOW ................. Shows a hidden Winamp (NOT the same as restore. Used after HIDE)
     ONTOP ................ Toggles Stay On Top on/off
     PLAYLIST ............. Toggles the playlist show on/off
     SHADE ................ Toggles the winamp shade mode on/off
     PLSHADE .............. Toggles the playlist shade mode on/off
     EQ ................... Toggles the equalizer window on/off
     OPEN ................. Opens the Winamp open file(s) dialog
     VISPLUG .............. Toggles visualisation plug-in on/off
     CLOSE ................ Closes WinAMP
     START ................ Starts WinAMP
     THEME ................ Needs additional parameter (LIST or SET)
     THEME SET X .......... X is the name of the theme you want (with extension, but not directory)
     THEME SET RANDOM ..... Returns if the random theme feature is on or off
     THEME SET RANDOM 0/1 . Turns the random theme feature on or off (0=off, 1=on)
     THEME LIST ........... Lists all themes available in the themes directory (Themes\*.INI)
     SAVEPL ............... Saves the default playlist in WA_Link\Playlist.txt
     VIEWPL ............... Views the default playlist in a window, double click to jump to that track or right click for
                            several other commands
     PLADD ................ Adds a track to playlist (Full path required, multiple tracks enclosed in "<trackname>" )
     PLADDF ............... Adds a single track to playlist
     PLADDD ............... Adds a folder to playlist (Subfolders not added and only most common extensionsare added)
     CMD [X] .............. If X is not specified, views current /wa stats command, else it sets a new one
     ABOUT ................ Shows DLL about info
     HELP ................. Shows this command list
     README ............... Shows the readme file
     RAW X ................ Echoes to active window raw winamp information (use: /wa raw help for list)
     WRITEPL .............. Makes winamp write the default playlist (winamp_dir\winamp.m3u)

   Examples:
     /wa Vol 75
     /wa pause
     /wa Stats
     /wa Visplug
     /wa seek 75
     /wa pladd "c:\music\winamp\demo.mp3" "c:\music\winamp\songs\britney spears - one more time.mp3"   

8. I want to display a different theme.

   Either use the WA Link theme dialog (right click choose WA Link -> WA Link Themes)
   or the "/wa theme list" and then "/wa theme set <themename.ini>" aliases. 

   New themes can be downloaded from our site: http://www.base.is/thecrew 
   when made available. 

9. I want to create my own theme.

   First of all. WA Link theme files are just regular text files with the .ini 
   extention and are located in the /theme folder under the /wa_link folder

   You can either create a new theme with the included theme editor or do it in a 
   text editor such as notepad.

   It might be helpful to check out some of the themes which are included. It
   is better if you just want to make little changes to simply edit the theme
   you want to change. WA Link popup menu -> WA Link Themes -> Select theme and press
   "Edit". (To make a new theme press "New" and enter the name of the theme)

   The theme file format is listed below:

     1st line is only showed if Winamp is playing. The parameter %VERSION displays 
         the Winamp version
     2nd line is only showed if Winamp is stopped. The parameter %VERSION displays 
         the Winamp version
     3rd line is only showed if Winamp is paused. The parameter %VERSION displays 
         the Winamp version
     4th line is only showed if Winamp is not running. Takes no parameter
     5th line contains the way the title info is shown. The parameter %TITLE 
         displays the title
     6th line contains the way the artist info is shown. The parameter %ARTIST 
         displays the artist name
     7th line contains the way the album info is shown. The parameter %ALBUM 
         displays the album
     8th line contains the way the year info is shown. The parameter %YEAR 
         displays the year
     9th line contains the way the genre info is shown. The parameter %GENRE 
         displays the genre
    10th line contains the way the comment is shown. The parameter %COMMENT 
         displays the comment
    11th line contains the way the length, percentage done and bar is shown. 
         The parameter %MINLEN displays the minutes part of the current track
         The parameter %SECLEN displays the seconds part of the current track
         The parameter %MINDONE displays the minutes part played of the current track
         The parameter %SECDONE displays the seconds part played of the current track
         The parameter %PERCENT displays the percentage done
         The parameter %BAR displays the playbar
         The parameter %SIZE displays the size of the current track in Megabytes (X.XX)
         The parameter %BITRATE displays the bitrate of the current track in Kbps (XX)
         The parametar %SAMPLERATE displays the samplerate of the current track in KHz (XXX)
         The parameter %CHANNELS displays the channels of the current track (Mono/Stereo)
    12th line contains the way the done part of the playbar is shown
    13th line contains the way the remaining part of the playbar is shown
    14th line contains the way the info is shown, and what info is shown and in 
         what order. Plus symble is the seperator.
         S = Display status of Winamp. Displays either line 1,2,3 or 4 depending on 
             winamp playstate. Always shown if specified.
         T = Display Title info. Line 5. Only showed if there is Title info.
         A = Display Artist info. Line 6. Only showed if there is Artist info.
         B = Display Album info. Line 7. Only showed if there is Album info.
         Y = Display Year info. Line 8. Only showed if there is Year info.
         G = Display Genre info. Line 9. Only showed if there is Genre info.
         C = Display Comment info. Line 10. Only showed if there is Comment info.
         L = Display length info. Line 11. Always shown if specified and winamp is playing.
    15th line contains the error info if Winamp cannot find the default playlist, this should never happen,
         unless Winamp install info cannot be found in the registry, due to lack of installation. Takes no parameter.
    16th line contains the error info if Winamp does not return a playstate. Could happen if you have a very old 
         version of winamp. Takes no parameter.
    15th and 16th lines are error messages and take no parameter.

   If you're not using the Theme Editor you must save the textfile as *.ini and 
   place it in the \themes\ folder undir WA_Link   

   WA_Link ignores all lines below the 15th line. So there you can put info about your
   theme and/or credits. Also it is good to say what version of WA Link your theme is for. 
   If you want to share your new cool theme with others, E-mail it to base@base.is and 
   we'll put it up on the official WA Link page.

10. I want help with my theme and/or WA_Link.

   Send us your theme file. Also it will be helpful if you told us your 
   Winamp version, mIRC version and WA_Link version.
   We will try to assist you the best way we can. E-mail: base@base.is

11. I want to report a bug, add to the mIRC scripting or DLL part or come with ideas.

   Contact us at base@base.is and earn your place in special thanks 
   section in future versions. Please comment what parts you own in the script or DLL
   
   TIP! It might be useful to take a look at developers.txt located in the WA_Link 
   folder if you're planning to modify the WA Link mIRC Script or DLL   

12. I'm paranoid about backdoors, are there any in WA_Link?
   
   WA Link DLL and mIRC script are both open source so you can always check for backdoors.

   We can guarantee that there are no backdoors in the mIRC script part of WA_Link 
   or in the WA_Link DLL which comes directly from us. Although it is possible 
   you have a 3rd party modified version of WA_Link. If that is the case we can't 
   guarantee anything since everybody are free to modify WA Link and distribute it at will.
   (Offical WA_Link Site - http://www.base.is/thecrew)  

13. What is bASE and the bASE crew?
   
   bASE is a software company which specialize in database programming and hardware
   solutions which include automation systems and programmable logic controllers.
   bASE has also done solutions in the field of Graphic and Internet programming.
   WA Link was made by a few bASE employees and others (who call them self the 
   bASE crew) in their spare time. WA Link is not an official bASE product.

   If you want to join the crew check out out website http://www.base.is/thecrew
   or visit our channel. See bottom of this file.

14. Version history

   WA_Link 1.0 (01.19.2002)                                                                
   - Initial version                                                           
   WA_Link 1.1 (02.01.2002)                                   
   - /say is now included in the themes, not hardcoded in the DLL anymore
   - Added alias "/wa jump <playlist_nr>" (also added in WA Link popup)
   - Added alias "/wa find <string>"  (also added in WA Link popup)
   - Added alias "/wa hide"           (also added in WA Link popup)
   - Added alias "/wa show"           (also added in WA Link popup)
   - Added alias "/wa restore"        (also added in WA Link popup)
   - Added alias "/wa minimized"      (also added in WA Link popup)
   - Added alias "/wa shade"          (also added in WA Link popup)
   - Added alias "/wa plshade"        (also added in WA Link popup)
   - Added alias "/wa playlist"       (also added in WA Link popup)
   - Added alias "/wa open"           (also added in WA Link popup)
   - Added in popup. Custom Volume, Seek and panning.
   - Theme file format changed. Simplyfied and enhanced.
   - Readme file updated
   - Command line help improved
   - Error messsages added
   - General debugging
   WA_Link 1.2 (03.27.2002)
   - Edit and Preview options added in Theme dialog
   - Random theme feature added
   - WA_Link.ini in the /WA_Link dir is no longer needed
   - RAW commands added in the DLL
   - Enhanced alias "/wa theme list"
   - Added alias "/wa theme set random" 
   - Added alias "/wa raw help"
   - Added alias "/wa raw wadir"
   - Added alias "/wa raw version"
   - Added alias "/wa raw state"
   - Added alias "/wa raw tracksec"
   - Added alias "/wa raw possec"
   - Added alias "/wa raw trackno"
   - Added alias "/wa raw trackfilename"
   - Added alias "/wa raw filetype"
   - Added alias "/wa raw title"
   - Added alias "/wa raw artist"
   - Added alias "/wa raw album"
   - Added alias "/wa raw year"
   - Added alias "/wa raw comment"
   - Added alias "/wa raw genre"
   - Added alias "/wa raw getshuffle"
   - Added alias "/wa raw getrepeat"
   - Added alias "/wa raw bitrate"
   - Added alias "/wa raw samplerate"
   - Added alias "/wa raw channels"
   - Added alias "/wa raw tracksize"
   - Added alias "/wa cmd" 
   - Added alias "/wa EQ"  (also added in WA Link popup)
   - Added alias "/wa pladd"  (also added in WA Link popup)
   - Added alias "/wa pladdd" (also added in WA Link popup)
   - Added alias "/wa pladdf" (also added in WA Link popup)
   - Added alias "/wa savepl"
   - Added alias "/wa viewpl" (also added in WA Link popup)
   - Pop-up menus are updated with new aliases
   - Readme file updated
   - Developers file added
   - General debugging



WA Link is a freeware and everyone are free to distribute it and to use WA_Link 
(.dll or .mrc) as they see fit, without having to include any of the original
files or credits. But if you decide to distribute a modified WA_Link (.dll or .mrc)
an E-mail would be very much appreciated (base@base.is).

Even though this addon should not be able to damage anything in your or other computers,
we cannot be held responsible if so unlikely this would happen. This addon is provided 
"as is" and is to be used on the users own responsibilty. 

Credits:
WA_Link v1.2 was made by the bASE Crew
DLL programming : Sveinn Steinarsson
mIRC scripting  : Bergþór Olivert Thorstensen
Documentation   : Bergþór Olivert Thorstensen and Sveinn Steinarsson

Official IRC channel #base.is on gamers-net.com
USA Server    : us1.gamers-net.com channel: #base.is
Europe Server : dk1.gamers-net.com channel: #base.is

Offical Site http://www.base.is/thecrew/

Special thanks : MrBucket                    : Testing WA Link and coming up with ideas for it
                 DelphiGiz                   : Advertising WA Link and supporting the WA Link project
                 Barnuts                     : Coming up with ideas for WA Link and advertising it
                 _]kAtO[_                    : Translating the default WA Link theme to Italian
                 Svolfluga                   : Translating the default WA link theme to French
                 Punchbag                    : Testing WA Link
                 René Rosenlund              : Coming up with ideas for WA Link and advertisting it
                 Lasse Ruud                  : Bug reporting
                 Phish                       : Coming up with ideas for WA Link
                 Karma                       : Advertisting and testing WA Link
                 DiGiT                       : Testing


Winamp is a trademark of the Nullsoft, Inc.
mIRC is a trademark of the mIRC co. Ltd.
WA_Link is not a trademark. If it were, it would be a trademark of the bASE crew.