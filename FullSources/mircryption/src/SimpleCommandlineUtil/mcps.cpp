//---------------------------------------------------------------------------
// mcps.cpp - simple commandline tool utility that can be used to encrypt or decrypt a string with a given key
//
//	mcps -e/d [KEY] [TEXT]
//	 -e for encode
//	 -d for decode
//
// Done by Ourem 14.05.2004.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// This code is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Blowfish algorythms: Bruce Schneier and Jim Conger
//   Bruce Schneier, 1996, Applied Cryptography, 2nd ed., John Wiley & Sons
// Blowfish Eggdrop algorythms:  Robey Pointer
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <iostream>
#include <string>
using namespace std;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// include for forward declarations
#include "mc_blowfish.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// for base64 test
//#include "b64stuff.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Just this part is coded by me (Ourem)
int main(int argc, char** argv) 
{
	char *resultstr;
	
	// base64 test
	/*
	unsigned char dest[257];
	int count;
	int len=4;
	for (count=0;count<len;++count)
	    dest[count]=(unsigned char)count;
	char * dest2=(char*)(spc_base64_encode((unsigned char*)dest,len,0));
	cout << dest2;
	cout << std::endl;
	*/

	if ( argv[1] )
	{
		if( strcmp( argv[1], "-d" ) == 0  && argv[2] && argv[3] ) 
		{
			resultstr = decrypt_string(argv[2], argv[3]);
			cout << resultstr;
			free ( resultstr );
			exit ( 1 );
		}
		if( strcmp( argv[1], "-e" ) == 0  && argv[2] && argv[3] ) 
		{
			int lengh = 0;
			int i;
			char* cryptedtext;
			
			for( i = 3; i < argc; i++ )
			lengh += strlen( argv[i] );
			
			cryptedtext = ( char* )malloc( lengh + argc );
			strcpy( cryptedtext, argv[3] );
			if( argc > 4 )
			{
				for(i = 4; i < argc; i++ )
				{
					strcat( cryptedtext, " " );
					strcat( cryptedtext, argv[i] );
				}
			}
			resultstr = encrypt_string(argv[2], cryptedtext);
			cout << resultstr;
			free( cryptedtext );
			free ( resultstr );
			exit ( 1 );
		}
	}
	cout << "mcps -e/d [KEY] [TEXT]\n";
	cout << "-e for encode\n";
	cout << "-d for decode\n";
	exit ( 0 );
	return ( false );
}
//---------------------------------------------------------------------------
