//---------------------------------------------------------------------------
The files inside the src directory can be compiled for local use, with
 the included visual c++ project files, or for use on an eggdrop, by
 copying the src directory to your shell and running "make"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
From mircryption help file:

Mircryptionboard consists of a client and a server.  All people on the
channel that want to be able to read and add news will need to install the
client mirc script.  Only one person on the channel needs to run the news bot
server, which can be run from a local mirc, or, preferably, from an eggdrop
shell.

See the MircryptionSuite general instructions for how to install the
MircryptionBoard client script.

Installing the Remote Eggdrop Mircryption Board Newsbot Server:

1.The preffered way to run the mircryptionboard news bot server is by
 using an eggdrop robot on a shell account.  Information on setting up a shell
 accont and an eggdrop robot are beyond the scope of this helpfile.  Once you
 have setup an eggdrop and it is working properly, you may follow these steps.

2.Eggdrops use tcl scripts to provide additional functionality.  Upload
 the entire MircryptionBoardServer directory to your eggdrop's
 scripts directory on your shell account, so that you have a directory
 structure like eggdrop/scripts/MircryptionBoardServer/.

3.Log into your shell account and change to the eggdrop's
 scripts/MircryptionBoardServer/src subdirectory, and type: make
 to compile the c++ code for mcboardbot.exe which runs the news server.
 After compilation you should see a mcboardbot.exe in the parent directory.
 If you see any errors during compilation, you will have to edit the
 Makefile to resolve the problems.  Ask for help on #mircryption on efnet.

4.If compilation is successful, the last step is to modify your eggdrop
 configuration file to tell it to load the mircryptionboard tcl script on
 startup.  Add the following line to the end of your eggdrop's .conf file: 
 source scripts/MircryptionBoardServer/mcboardbot.tcl

5.Now kill your eggdrop if it is currently running and restart it, or use
 the .rehash command. If the tcl script loads successfully you will see a
 line saying so when you start your eggdrop.

6.See the section on mircryptionboard configuration files for
 information on how to properly configure a news board.

//---------------------------------------------------------------------------

*ALTERNATIVE* METHOD: Installing the Local Mirc Mircryption Board News Server Script

1.The best way to run Mircryptionboard is by using an eggdrop robot
 running on a shell account, because this allows the news board to run as a
 dedicated news server which does not require a specific person to be
 connected to irc for it to work.  However, files are provided to help one
 person on a channel run the mircryptionboard server from within their mirc. 
 This can also be useful for testing mircryption board.

2.To install the local mircryptionboard, first copy the files from the
 .\Extras\LocalMirc_MircryptionBoardServer\ subdirectory into your mircryption
 subdirectory of mirc.

3.Then, start mirc and type:		/load -rs
 mircryption\mcboardbot.mrc

4.This script instructs your local mirc to respond to news requests by
 other users; see the section on mircryptionboard configuration files for
 information on how to properly configure a news board.

5.When instructions in the remainder of this help file refer to files
 on the eggdrop server, you will be working with these same files inside the
 mircryption subdirectory of your local mirc.
//---------------------------------------------------------------------------
