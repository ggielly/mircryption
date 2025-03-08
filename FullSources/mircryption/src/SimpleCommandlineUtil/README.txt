//---------------------------------------------------------------------------
mcps.cpp - simple commandline tool utility that can be used to encrypt or decrypt a string with a given key
Done by Ourem 14.05.2004.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
This is a simple commandline tool that can be used to encrypt or decrypt
 blowfish strings using mircryption/fish/blow/eggdrop/psybnc version of
 blowfish (unfortunately these tools use a nonstandard base64 encoding so
 some other standard blowfish algorithms dont yield identical results).
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
NOTE: mircryption.dll now exports encrypt and decrypt functions, so you
can use the dll from your own programs for faster processing if you know
how to use dlls.
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
USAGE:

	mcps -e/d [KEY] [TEXT]
	 -e for encode
	 -d for decode

 note that this tool is not used for normal mircryption operation; it is an
 optional utility for those who need it.
 
Ourem gives this example of calling this utility from a php script:
	<?php
	$key = "testkey";
	$text = "baFRX1VJxW5.Cpe9V.FgiH00";
	$fp = popen("/bin/mcps -d $key $text", 'r');
	$read = fread($fp, 1024);
	pclose($fp);
	echo $read;
	?>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
COMPILATION INSTRUCTIONS:
Compile using dev-c++ provided project file, or using the Makefile in *nix.
Note that the makefile uses files in the parent src/ directory.
//---------------------------------------------------------------------------
