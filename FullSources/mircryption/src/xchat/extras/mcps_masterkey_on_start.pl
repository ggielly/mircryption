#!/usr/bin/perl
#
#    file: mcps_masterkey_on_start.pl
#  author: Gregor Jehle <gjehle@gmail.com>
# created: 24th NOV 2003
#
# This mini script will have mircryption for xchat enter your master passphrase
#  automatically at startup. Warning - this makes your master passphrase
#  accessible to anyone who can view the files on your computer.
#
# Installation Instructions:
#  1.  Replace your master key below
#  2a. In linux to have it autoload on startup, place this file in ~/.xchat2/
#  2b. Or on windows place it in  %USER_PROFILE%\Application Data\xchat
#  3.  You may have to load the script manually once before it will autoload.
#
#
# set the mcps masterkey at xchat start
# (3 seconds after that to be precise..
#  let the other shit load first)
#
# NOTE
# very basic example
# this can basicaly used to execute whatever you like
# on scriptload (usualy xchat start)
#
# use at own risk


$mcps_masterkey = 'replace_this_with_your_master_key';

IRC::register("on start","0.1","","");
IRC::add_timeout_handler(3000,'on_start');
IRC::print("on start loaded. timeout set.");

sub on_start
{
	IRC::command("/masterkey $mcps_masterkey");
	return 0;
}
