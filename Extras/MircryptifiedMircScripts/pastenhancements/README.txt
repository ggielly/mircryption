; mircryptified by koncool (http://anti-me.org/koncool)
; for mircryption (http://mircryption.sourceforge.net)
; thanks to great author of paste enhancements for 
;  super clean code which makes mircryptifying trivial.
;
; IMPORTANT -> YOU MUST MOVE THIS SCRIPT TO TOP (ABOVE MIRCRYPTION),
;  OR SOME OF YOUR POSTS WILL BE DOUBLED
;
;
;
; BELOW FOLLOWS ORIGINAL README BY AUTHOR OF PASTE ENHANCEMENTS:
; ---------------------------------------------------------------------








         -- Paste Enhancements v2.22 --

This is a relatively simple addon that was designed
to be straightforward and easy to use. It should be
working "out of the box", so to speak, and yet pro-
vide a comfortable amount of configurability, along
with a solid set of features.


Installation instructions::

* Decompress the script:
  - WinZip (www.winzip.com) will do the trick.
  - Place the files in a folder called 'pe' in your
    mIRC directory.

* Load the script:
  - Launch mIRC
  - Type in: /load -rs pe\pe.mrc
  - Click 'Yes' if prompted about initialization

* You're set!

  - To configure Paste Enhancements, you may:
    + Type in: /pe_config from within mIRC
    + Under the "Commands" (or similar) menu choose
      "Paste Enhancements" then "Config"

  - To unload:
    + Type in: /unload -rs pe.mrc
    + Under the "Commands" (or similar) menu choose
      "Paste Enhancements" then "Unload"

  - To view this document again:
    + Under the "Commands" (or similar) menu choose
      "Paste Enhancements" then "Readme/Help"

* Upgrading from 2.0:
  - Unzip into a separate directory
  - Copy pe.mrc and readme.txt into the existing PE
    directory.
  - Click yes when asked to overwrite.
  - Type in: /reload -rs pe\pe.mrc


Explanation of features::

The settings dialog has been completely re-designed
in this version. Most of the old features are still
around, though some have been renamed, but a number
of things have been added too. The most significant
change is probably the settings dialog.

The settings dialog has three parts. The left side,
'Targets', allows you to add networks/channels that
you can associate a profile with. A profile is just
a saved group of settings. The top right contains a
list of profiles. Both the targets and the profiles
lists have buttons to add and remove items. You are
not allowed to remove the Default target or profile
or the Disabled profile.

The main dialog is given over to Paste Enhancements
features. The tabs separate them into two sections.
Clicking Apply or OK will save the current settings
under the selected profile. Changing the profile or
target will discard settings changes (doing so will
reload the settings from the chosen profile). Also,
clicking cancel will discard changes.

Changing the Profile combo box will set the profile
used with the selected target. This takes effect at
once and will not be undone by clicking Cancel.

The Copy button will place a line in your clipboard
that adds a profile using the selected settings. It
can be used to easily share settings with others.

The 'Force load as first script' option is there to
help prevent script conflicts. For it to work best,
any scripts that use the on input event need to use
the & prefix (on &*:input:...) so the event doesn't
trigger when PE has halted it. PE will use the /msg
command to send its text to the destination.

A note on profiles. Channel targets are specific to
a network. If you paste into a channel that doesn't 
have a matching target, PE will use the profile for
the network you're on. If there is also no matching
target for the network, it will use the profile for
the Default target. When you paste into a query, if
the profile assigned to the current network has the
'Enable in query windows' option checked, then that
network's profile settings are used.

Now that you know how to make use of the new dialog
to match your PE settings to your chatting environ-
ment, I'll move on to the feature breakdown.


Basic Settings

"[ ] Enable in query windows (...)"
  - Enables features for pastes into query windows. 
    No effect when pasting into a channel.

"[ ] Paste to a DCC Chat"
  - DCC Chats the user you are querying in order to
    paste without delay.

"[ ] If over [X] lines"
  - Used in conjunction with the previous checkbox.
    Asks before pasting ONLY if the lines you would
    paste exceed the value set here.

"[ ] Paste one line every [X] ms..."
  - One of the main features. When enabled, PE will
    paste the lines one at a time, delaying X ms.

"[ ] Ask before pasting"
  - Displays a Yes/No dialog whenever you are about
    to paste text into a channel or query, based on
    what you selected in the first combo box.

"[ ] [Display warning dialog] when pasting"
  - Displays a Yes/No dialog whenever you are about
    to paste text into a channel or query, based on
    what you selected in the first combo box.

"[ ] [Open Clipboard Editor] when pasting"
  - In lieu of displaying the Yes/No dialog, PE can
    open the Clipboard Editor, allowing you to look
    over and/or modify what you are about to paste.

"[ ] Over [N] lines"
  - Used in conjunction with the previous checkbox.
    Asks before pasting ONLY if the lines you would
    paste exceed the value set here.

"[ ] Slow down pastes larger than [X] ... to [Y]ms"
  - When enabled, increases the delay between lines 
    to Y ms, after having already pasted X lines.

"[ ] Break up lines longer than [X] characters"
  - When pasting a line that contains more than the
    specified X characters, this will make PE break
    the line into two or more lines. Each line will
    be divided after X characters or the first word
    break before that boundary.


Extra Settings

"Blank lines in clipboard:"
  - Determines how you want to treat the pasting of
    blank lines in the clipboard.

"Text typed while pasting:"
  - Determines what to do when you type a line into
    a window that is currently being pasted to.

"[ ] Resume pasting [X] seconds after... rejoin"
  - If a channel you are pasting to becomes invalid
    during the paste, PE will abort the action.
    Check this box if you want PE to resume pasting
    what it was doing upon rejoining a channel.

"[ ] Paste single line to editbox"
  - Sometimes, you end up with a single line in the
    clipboard, and when you go to paste it into the
    edit box, it ends up getting sent to the active
    window instead. Check this box to avoid this!

"[ ] Keep spaces in pastes"
  - When enabled, pasting multiple lines containing
    multiple consecutive spaces will be fixed using
    CTRL+O characters so that the alignment is kept
    in tact.

"[ ] Strip codes from pastes:"
  - Check any of the BURK checkboxes to strip those
    characters from pasted text when this option is
    enabled.

"[ ] If one of these channel modes is set:"
  - Applies the above setting only when the channel
    mode contains one or more of the listed modes.

"[ ] No delay if one of these umodes is set:"
  - Overrides delay settings if your umode contains
    one or more of the listed modes.


"[ ] Force load as first script"
  - Some scripts, especially ones with themes, will
    interfere with the ON INPUT event. If you check
    this box, PE will check (in the ON START event)
    to make sure it is the first loaded script, and
    if it isn't, will reload itself to the #1 slot.


Version 2.0 and above::

Last, some non-settings features have been changed,
added, or removed. I added a clipboard editor which
contains buttons for fixing spaces, removing tokens
and pasting to the active window. I removed channel
popup items for these features. mIRC has a popup in
the editbox that contains a Paste option, so I also
took out 'Paste clipboard to active window'.

By the way, the clipboard editor is not supposed to
be a full-fledged text editor, so use your favorite
word processor for that. It's just a preview window
with some useful-to-irc features. You can get at it
via the channel/query popups, or by typing /pe_clip
in the editbox.


Notes::

1) *Fixed*
   Used some code from iinc to apply a kludge I had
   worked out before but didn't want to implement.

   When pasting a single <CRLF> terminated line, if
   the "Paste single line to editbox" option is on,
   the pasted line will replace anything that might
   have been in the active editbox. There is no way
   for me to correct this in the script.

2) If your pasted text is being sent to the channel
   or query twice per line, try enabling the "Force
   load as first script" option. This won't help in
   all cases; using on &*:input:... in scripts will
   help to make them compatible with PE. The & pre-
   fix will cause mIRC not to execute that event if
   it was previously halted.

3) The "force load as first script" option may have
   problems if another script that attempts to keep
   itself as the first loaded script is loaded, and
   this other script uses an ON INPUT event in such
   a way that it causes problems with PE's ON INPUT
   event (such as that mentioned in #2).

4) *Removed*

5) If you enable the "Keep spaces in pastes" option
   background colors will not be kept in strings of
   spaces, but will be picked up on the other side.
   I haven't decided if it would be worth the added
   length to the text to correct this.

6) *Removed*
7) There is a 30k size limit on dialog editboxes in
   mIRC; this affects the clipboard editor.

Versions::

*** ?.?.04 - v2.22 ***
1) Made spaces-in-paste options work again :\
2) Remembered to update.

*** 3.24.04 - v2.21 ***
1) Fixed bug with pasting text with "strip if mode"
   enabled; a mIRC nested while one-liner thing was
   causing it. Expanded the loop and it worked.
2) Fixed bug where spaces weren't kept in pastes if
   stripping was enabled.
3) Fixed a stupid bug in "text typed while pasting"
   options. Changed $2- to $1- but I can't remember
   why it was $2- in the first place.
4) Fixed item #3 from version 2.1; pasting multiple
   lines while there was text in the editbox caused
   it to not clear the temporary editbox info.
5) Corrected a minor issue where temporary data got
   saved along with settings. PE now clears any old
   temporary entries from the hash table on start.
6) PE now tags pastes with some color codes. If you
   double-click the contents of some text pasted by
   another PE user, it will display the whole paste
   in a separate window, with an option to copy the
   text to your own clipboard.

*** 3.3.04 - v2.2 ***
1) Added an option to paste via nomorepasting.com's
   mIRC scripts section.
2) Modified options and clipboard editor dialogs to
   support #1.
3) Clipboard Editor now displays consecutive spaces
   properly. It still won't paste them right unless
   you use the 'fix spaces' command.
4) The Apply button copies spaces properly as well.

*** 1.20.04 - v2.11, v2.22 ***
1) Fixed a stupid bug in the Clipboard Editor where
   clicking Paste doesn't work.
2) Don't drink and code.

*** 1.20.04 - v2.1 ***
1) Fixed a silly bug but didn't write it down.
2) Fixed compatibility with programs that send text
   to mIRC as a paste (even though that text is not
   in the clipboard) i.e. NaturallySpeaking
3) There seems to be a bug where PE does not always
   clear the text it stores from the editbox to use
   in the editbox hack mentioned above. If you have
   figured out how to reliably reproduce this error
   please contact me.
   The bug manifests itself by prepending some text
   that you previously had in the editbox to pasted
   text that you sent when the editbox was empty.
4) Improved the clipboard editor. Added strip codes
   options and moved some buttons to a menu.
5) Modified ask-on-paste options to allow having PE
   open the Clipboard Editor when pasting.

*** 8.10.03 - v2.0 ***
1) Fixed a major bug in text-while-pasting behavior
   (it simply didn't work. Don't know how that went
   unnoticed by me or anyone for so long...)

2) Didn't want to update for one silly bugfix, so..
   welcome to version 2 :)

3) Redesigned settings dialog

4) Added profiles

5) Added features:
   - Clipboard editor
   - Break up lines over N characters
   - No delay if umodes set

6) Expanded 'Strip ctrl+k from Dalnet +c channels'
   to strip any codes from channels with any modes.

7) Kludged 'Paste single line to editbox' to paste
   properly to the end of text already in editbox.


*** 10.30.02 - v1.2 ***
Minor bug fixes and changes to the readme.

1) Pasting more text while in a previous paste will 
   no longer cause the next line to be sent early.

2) "Keep spaces in pastes" option now handles lines
   prefixed with a single space.

3) Removed code in "Strip clipboard" popup that was
   performing unnecessary conversions.

4) "chose" -> "choose" - I can't believe I did that
   one, heh. (Readme)

5) Note 2 expanded to offer some help to users with
   double-pasting problems. (Readme)

6) Removed Note 4: I had misunderstood how /halt is
   intended to work. (Readme)


*** 2.29.02 - v1.01 ***

1) Added input box to warn on old mIRC version

2) Added input box to optionally take you to mIRC's
   download site if you have an old version

3) Added input box to clean up variables on unload

4) Took ctrl-o characters out of readme.txt
   (Apparently, mIRC actually preserves spaces when
    using /loadbuf ;)

5) Changed command menu item to submenu

6) Added options under command submenu:

   * Config
   * Unload
   * Readme/Help

   * Added instructions to get at Unload and Readme

   * Updated readme instructions to reflect this

7) Added $isfile check for readme.txt to pe_readme
   * Added appropriate error box if not found

8) Improved "Paste single line to editbox" option:

   * Strips ASCII characters 9, 10, 13, and 32 when
     checking for blank line
   * Now ignores blank clipboard lines during check
     for single pasteable line

9) Strips ASCII characters 9, 10, 13, and 32 before
   checking for blank clipboard lines as it buffers
   the clipboard to the hidden @window

10)Added features, and channel/query popup submenus
   to access them:

   * Stop pasting
   * Paste clipboard to active window
   * Strip clipboard

     "Strip clipboard" is a submenu holding up to 3
     items. These items will remove 1-4 tokens from
     every clipboard item. (Timestamps, nicks, etc)

11)Added $pe_inpaste(window,cid), will return $true 
   during PE pastes, $false otherwise.

12)Tidied up, organized, and expanded all code.


*** 2.27.02 - v1.0 ***
First released version.


Other::

Questions, comments, or solicitations for sex:
                        "Scripting: PE"@ravaged.org
                                 mrc-pe@ravaged.org

Contact me on IRC:
               myndzi on DALNet, Efnet, or Undernet
       #boom, #scripting, #scripts-are-us on DALNet
                       #mircscripts.org on Undernet
                                #pollution on Efnet

                     -- end --
