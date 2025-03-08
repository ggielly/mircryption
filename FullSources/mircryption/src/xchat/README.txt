//---------------------------------------------------------------------------
// INTRODUCTION
//---------------------------------------------------------------------------
This is a port of mircryption for the xchat irc client (www.xchat.org) v1.9x+
For more information about mircryption, see the MS Windows help file or web
 pages at http://mircryption.sourceforge.net.  This file is just an
 addendum for the preliminary xchat port.  Most mircryption development is
 done on windows with the Mirc irc client.  The xchat port is just for the
 basic functionality.  Improvements to this script are welcomed.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// INSTALATION:
//---------------------------------------------------------------------------
This is an addon for the xchat irc client (www.xchat.org).
It uses the "new" plugin system of xchat, which is only available for xchat
 clients of version 1.9x or greater, and will NOT work on versions of xchat
 prior to 1.9x.
It is coded in C++, and you will need to recompile it on your computer before
 you can load it into xchat (unless we have provided a binary precompiled
 library for your platform).
To compile this code, simply change to the src/xchat subdirectory and type:
 make
This will present you with a list of known targets.  Try the one that seems
 best to you, if it doesn't work, try another :)
Hopefully this will build the mircryption.so shared library.  If not, you
 may have to modify the Makefile to suit your system.  Please let us know
 if you find any bugs.  After the mircryption.so library is built, you might
 want to copy it to a permanent location.
Now you must load it into xchat.  To do this manually, start xchat, confirm
 you are using 1.9x or greater,  then from Xchat menu, select Load Plugin,
 and browse to the directory where mircryption.so is and load it
To have mircryption addon load each time you start xchat, copy the file
 mircryption.so file into the directory $(libdir)/xchat/plugins/ which
 usually translates to /usr/lib/xchat/plugins/ (make the dir if nonexistent).
You shoud be able to copy the .so to ${HOME}/.xchat2/ instead if you prefer.
If the addon loads properly, you should see the mircryption version
 displayed and you can now type /help to see a list of mircryption commands.
NOTE:
Before you can begin encrypting, you need to set the master password for the
 key file using the command "/masterkey ..." where ... is replaced by the
 passphrase you want to use to encrypt all other channel keys.  Use a phrase
 which cannot be guess and is between 30-48 characters.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// USAGE NOTE:
//---------------------------------------------------------------------------
After each time you run xchat with the mircryption addon, you *MUST* type:
	/masterkey passphrase
 where passphrase is you passprhase for the key file.  Until you do, you
 will not be able to use your keys. This prevents someone else from learning
 your channel keys even if they get access to your files.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// FOR USE WITH BLOWFISH +OK
//---------------------------------------------------------------------------
You can edit srx\xchat\mircryption.cpp and change the default encryption
 prefix to +OK so that people with the blow addon can read you.  see
 the file for more instructions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// LIMITATIONS AND KNOWN ISSUES:
//---------------------------------------------------------------------------
Only the basic mircryption functions are supported in this xchat port:
 Channel/query encryption and encrypted topics.
 The xchat port can use your existing MircryptionKeys.txt keyfiles from mirc.
 You must manually set the master keyfile password on startup with /masterkey.
Just type 'make' to get a list of make targets, try the alternate targets
 if your compiler complains.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
NOTE ABOUT KEYFILE LOCATION:
//---------------------------------------------------------------------------
Mircryption for xchat now defaults to using a keyfile called
 .MircryptionKeys.txt in the user's home directory (~/.MircryptionKeys.txt).
SO if you are upgrading you need to move your MircryptionKeys.txt file into
 your home dir and rename it to .MircryptionKeys.txt which will make it
 inivisible unless you do ls -a.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// NEWS:
//---------------------------------------------------------------------------
03/03/03 Thanks to a pro xchat scripter, xro, encrypted nicks are now
          aligned properly, encrypted topics are displayed perfectly, and actions are
          encrypted when appropriate.
04/27/03 xro fixed lines with / not being encrypted
04/28/03 xro added \017 characters to format, which allows nicks to be clicked.
05/09/03 changed default keyfile location to ~/.xchat/MircryptionKeys.txt
05/13/03 added windows compatibility (tested with xygwin and xchat2 for win32)
09/08/03 added support for ` prefix toggle
12/27/03 added color stripping [Gregor Jehle <gjehle@gmail.com>]
03/24/04 added instructions for using +OK blowfish, modified makefile
03/30/04 changed default outgoing tag to +OK for better default compatibility with other scripts
05/18/04 added meow support
01/09/05 major feature: added support for CBC mode (prefix key with 'cbc:')
         note this NEEDS to be tested for endianness compatibility (!)
//---------------------------------------------------------------------------
