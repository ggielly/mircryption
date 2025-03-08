///******************
// *** FiSH v1.25 ***
///******************

#define WIN32_LEAN_AND_MEAN
#pragma comment(linker,"/FILEALIGN:0x200 /MERGE:.data=.text /MERGE:.rdata=.text /SECTION:.text,EWR /IGNORE:4078")

#include "FiSH.h"

#include <windows.h>
#include <stdlib.h>

unsigned char iniKey[]="blowinikey\0ADDITIONAL SPACE FOR CUSTOM BLOW.INI PASSWORD";
unsigned char *recv_buf_rest, *recv_buf_copy;
unsigned int buf_size=4096;

//FILE *debugF;


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			PRNGInit();
			if(GetModuleFileName(hModule, iniPath, sizeof(iniPath)) == 0) return FALSE;
			strcpy(randomPath, iniPath);
			strcpy(mIRCiniPath, iniPath);
			strcpy(strrchr(randomPath, 0x5C), "\\random.ECL");
			strcpy(strrchr(mIRCiniPath, 0x5C), "\\mIRC.ini");
			PRNGLoad(randomPath);
			strcpy(strrchr(iniPath, 0x5C), "\\blow.ini");
			recv_buf_rest=malloc(buf_size);
			recv_buf_copy=malloc(buf_size);
			recv_buf_rest[0]=0;
		    mip=mirsys(500, 16);
			if(mip==NULL || recv_buf_copy==NULL || recv_buf_rest==NULL) return FALSE;
			initb64();
			PRNGAddEvent();
			break;
		case DLL_PROCESS_DETACH:
			free(recv_buf_rest);
			free(recv_buf_copy);
			PRNGSave(randomPath);
			PRNGCleanup();
			if(mip) mirexit();
			break;
    }
    return TRUE;
}



__declspec(dllexport) int incoming(void)
{
	unsigned char *recvPtr, *recv_buf, *recv_buf_end, *forPtr, *msgPtr, *fromPtr, *endMsgPtr, *restPtr;
	unsigned char hostmask[150], contactName[100], theKey[300], bfKey[300], bf_dest[1500];
	unsigned char ini_incoming[20], myMark[20], mark_pos[20];
	unsigned int recv_buf_len, msg_len, i, psylog_found, buf_copied, cmd_type, crypt_prefix_len, only1line;

	__asm
	{
		mov recv_buf, ecx
		mov recv_buf_len, esi
		mov byte ptr [ecx+esi], 0
		mov only1line, ebx	// mIRC v6.14 feature (ebx = 0xAFFE0614)
	}

	if(recv_buf_rest[0]==0 && strstr(recv_buf, "+OK ")==0 && strstr(recv_buf, "mcps ")==0) return recv_buf_len;	// don't process, blowcrypt-prefix not found


	GetPrivateProfileString("FiSH", "process_incoming", "1", ini_incoming, sizeof(ini_incoming), iniPath);
	if(ini_incoming[0]=='0' || ini_incoming[0]=='n' || ini_incoming[0]=='N') return recv_buf_len;


	if(recv_buf_rest[0] != 0)
	{	// append current buffer to incomplete message from previous session
		strcpy(recv_buf_copy, recv_buf_rest);
		strcat(recv_buf_copy, recv_buf);
		recv_buf_len=strlen(recv_buf_copy);
		recv_buf_rest[0] = 0;
		recvPtr=recv_buf_copy;
		buf_copied=1;
	}
	else
	{
		recvPtr=recv_buf;
		buf_copied=0;
	}


	// mIRC v6.14 hands over only one line per call (without 0x0A and 0x0D)
	if(only1line != 0xAFFE0614)
	{
		// only needed for mIRC 5.91 -> 6.12 (cutted multiline messages)
		if(recvPtr[recv_buf_len-1] != 0x0A)
		{	// incomplete message found
			restPtr=strrchr(recvPtr, 0x0A);
			if(restPtr != 0)
			{	// backup incomplete message
				strcpy(recv_buf_rest, restPtr+1);	// copy the incomplete message for use in next session
				recvPtr[restPtr-recvPtr+1] = 0;		// cut the incomplete message from current recv buffer
				recv_buf_len=strlen(recvPtr);
			}
		}
	}


	recv_buf_end = recv_buf_len + recvPtr;


/*	debugF=fopen("c:\\debug_I.txt", "a");
	fprintf(debugF, "\n%s\n", "***ORIGINAL******:");
	fwrite(recvPtr, recv_buf_len, 1, debugF);
	fprintf(debugF, "\n%s\n", "***ORIGINALEND***:");
*/

	while(recvPtr < recv_buf_end)
	{
		fromPtr=recvPtr;
		if(fromPtr[0] != ':') goto find_next_msg;

		recvPtr=strchr(recvPtr, ' ');

		if (strnicmp(recvPtr, " PRIVMSG ", 9)==0) cmd_type=9;
		else if (strnicmp(recvPtr, " NOTICE ", 8)==0) cmd_type=8;
		else cmd_type=0;

		if(cmd_type)
		{
			if(strncmp(fromPtr, ":-psyBNC!psyBNC@lam3rz.de PRIVMSG ", 34)==0)
			{	// psyBNC log message found
				i=0;
				while(fromPtr[i] != 0x0A && fromPtr[i] != 0x0D && fromPtr[i] != 0) i++;	// psyBNC msg length
				i+=(unsigned int)fromPtr;
				fromPtr=strstr(fromPtr, " :(")+2;
				if(((unsigned int)fromPtr >= i) || ((unsigned int)fromPtr == 2)) goto find_next_msg;
				recvPtr=fromPtr;
				forPtr=fromPtr;		// irrelevant
				psylog_found=1;
			}
			else
			{
				recvPtr+=cmd_type;
				forPtr=recvPtr;
				psylog_found=0;
			}
			recvPtr=strchr(recvPtr, ' ');

			recvPtr+=2-psylog_found;

			if(strncmp(recvPtr, "+OK ", 4)==0) crypt_prefix_len=4;
			else if(strncmp(recvPtr, "mcps ", 5)==0) crypt_prefix_len=5;
			else crypt_prefix_len=0;

			if(crypt_prefix_len != 0)
			{	// blowcrypt-prefix found
				msgPtr=recvPtr + crypt_prefix_len;
				if(forPtr[0] == '#')
				{	// channel message
					i=1;
					while(forPtr[i] != ' ') i++;
					strncpy(contactName, forPtr, i);
					contactName[i]=0;
				}
				else
				{	// private message
					fromPtr++;
					ExtractRnick(contactName, fromPtr);
					ExtractRhost(hostmask, fromPtr);
				}

				FixContactName(contactName);

				GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
				if(theKey[0] == 0) goto find_next_msg;	// don't process, key not found in ini
				if(strncmp(theKey, "+OK ", 4)==0)
				{	// key is encrypted, lets decrypt
					decrypt_string(iniKey, theKey+4, bfKey, strlen(theKey+4));
					if(bfKey[0] == 0) goto find_next_msg;	// don't process, decrypted key is bad
				}
				else strcpy(bfKey, theKey);

				ZeroMemory(theKey, sizeof(theKey));

				msg_len=0;
				while(	(msgPtr[msg_len] >= 0x30 && msgPtr[msg_len] <= 0x39) ||
						(msgPtr[msg_len] >= 0x41 && msgPtr[msg_len] <= 0x5A) ||
						(msgPtr[msg_len] >= 0x61 && msgPtr[msg_len] <= 0x7A) ||
						 msgPtr[msg_len] == '.'  || msgPtr[msg_len] == '/'
						) msg_len++;	// count characters from base64 alphabet only

				if(msg_len == 0) goto find_next_msg;

				decrypt_string(bfKey, msgPtr, bf_dest, msg_len);
				ZeroMemory(bfKey, sizeof(bfKey));
				if(bf_dest[0] == 0) goto find_next_msg;	// don't process, decrypted msg is bad

				i=0;
				while(bf_dest[i] != 0x0A && bf_dest[i] != 0x0D && bf_dest[i] != 0x00) i++;
				bf_dest[i]=0x00;	// in case of wrong key, decrypted message might have control characters -> cut message

				// append crypt-mark?
				GetPrivateProfileString(contactName, "mark_encrypted", "1", myMark, sizeof(myMark), iniPath);
				if(myMark[0]!='0' && myMark[0]!='n' && myMark[0]!='N' && strncmp(bf_dest, "\001ACTION ", 8)!=0)
				{
					GetPrivateProfileString("FiSH", "mark_encrypted", "", myMark, sizeof(myMark), iniPath);	// global setting
					if(myMark[0] != 0)
					{
						GetPrivateProfileString("FiSH", "mark_position", "1", mark_pos, sizeof(mark_pos), iniPath);
						if(mark_pos[0] == '0') _asm nop;	// crypt-mark disabled
						else if(mark_pos[0] == '2')
						{	// prefix crypt-mark
							i=strlen(myMark);
							memmove(bf_dest+i, bf_dest, strlen(bf_dest)+1);
							strncpy(bf_dest, myMark, i);
						}
						else strcat(bf_dest, myMark);		//append crypt-mark at the end
					}
				}

				endMsgPtr = msgPtr + msg_len;
				strcat(bf_dest, endMsgPtr);	// append the rest of the recv buffer to decrypted msg
				strcpy(msgPtr-crypt_prefix_len, bf_dest);	// replace encrypted msg with plain text
				ZeroMemory(bf_dest, sizeof(bf_dest));
			}

			find_next_msg:
			while(recvPtr[0] != 0x0A && recvPtr[0] != 0) recvPtr++;	// search for end of line (or message)
			if(recvPtr[0] == 0 || recvPtr[1] == 0) break;
			recvPtr++;		// point to beginning of new line
			if((strstr(recvPtr, "+OK ")==0) && (strstr(recvPtr, "mcps ")==0)) break;
		}
	}


	// only needed for mIRC 5.91 -> 6.12 (cutted multiline messages)
	if(buf_copied)
	{
		if(strlen(recv_buf_copy) < 949) strcpy(recv_buf, recv_buf_copy);
		else
		{	// final string is too large for mIRC's receive buffer...
			strncpy(recv_buf, recv_buf_copy, 948);
			recv_buf[948]=0;	// pass only 948 bytes to mIRC, put the rest onto stack for next session
			if(recv_buf_rest[0] == 0) strcpy(recv_buf_rest, recv_buf_copy+948);
			else
			{
				i=strlen(recv_buf_copy+948);
				memmove(recv_buf_rest+i, recv_buf_rest, strlen(recv_buf_rest)+1);
				strncpy(recv_buf_rest, recv_buf_copy+948, i);
			}
			if(strlen(recv_buf_rest) > buf_size) MessageBox(NULL, "OH NO! FiSH ERROR! recv_buf_rest buffer overflow!\nPlease contact the author.", "FiSH ERROR MESSAGE!", MB_OK);
		}
		ZeroMemory(recv_buf_copy, buf_size);
	}

/*	fprintf(debugF, "\n%s\n", "***ALTERED******:");
	fwrite(recv_buf, strlen(recv_buf), 1, debugF);
	fprintf(debugF, "\n%s\n", "***ALTEREDEND***:");
	fclose(debugF);*/

	return strlen(recv_buf);	// return new length
}




__declspec(dllexport) int outgoing(void)
{
	unsigned char *send_buf, *sendPtr, *forPtr, *msgPtr;
	unsigned char contactName[100], theKey[300], bfKey[300], bf_dest[2000];
	unsigned char ini_tmp_buf[10], plain_prefix[10];
	unsigned int send_buf_len, i, cmd_type;
	BOOL encrypt_notice, encrypt_action;


	_asm mov send_buf, esi;
	sendPtr=send_buf;

	send_buf_len=strlen(sendPtr);


	GetPrivateProfileString("FiSH", "process_outgoing", "1", ini_tmp_buf, sizeof(ini_tmp_buf), iniPath);
	if(ini_tmp_buf[0]=='0' || ini_tmp_buf[0]=='n' || ini_tmp_buf[0]=='N') return send_buf_len;

	GetPrivateProfileString("FiSH", "encrypt_notice", "0", ini_tmp_buf, sizeof(ini_tmp_buf), iniPath);
	if(ini_tmp_buf[0]=='1' || ini_tmp_buf[0]=='Y' || ini_tmp_buf[0]=='y') encrypt_notice=TRUE;
	else encrypt_notice=FALSE;

	GetPrivateProfileString("FiSH", "encrypt_action", "0", ini_tmp_buf, sizeof(ini_tmp_buf), iniPath);
	if(ini_tmp_buf[0]=='1' || ini_tmp_buf[0]=='Y' || ini_tmp_buf[0]=='y') encrypt_action=TRUE;
	else encrypt_action=FALSE;


/*
	debugF=fopen("c:\\debug_O.txt", "a");
	fprintf(debugF, "\n%s\n", "***ORIGINAL***:");
	fwrite((char *)recv_buf, recv_buf_len, 1, debugF);
*/

	if (strnicmp(sendPtr, "PRIVMSG ", 8)==0) cmd_type=1;
	else if (encrypt_notice && strnicmp(sendPtr, "NOTICE ", 7)==0) cmd_type=2;
	else cmd_type=0;

	if(cmd_type)
	{
		sendPtr=strchr(sendPtr, ' ');
		forPtr=sendPtr+1;

		i=1;
		while(sendPtr[i] != ' ') i++;
		sendPtr += i;
		strncpy(contactName, forPtr, i);
		contactName[i]=0;

		sendPtr=strchr(sendPtr, ':');
		msgPtr=sendPtr+1;

		if(cmd_type==2 && strncmp(msgPtr, "DH1080_", 7)==0) return send_buf_len;	// don't encrypt DH1080 key-exchange

		if(msgPtr[0] == 0x01)
		{
			if(encrypt_action==0 || strncmp(msgPtr, "\001ACTION ", 8)!=0)	// encrypt ACTION?
				return send_buf_len;
		}

		FixContactName(contactName);

		GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
		if(theKey[0] == 0) return send_buf_len;	// don't process, key not found in ini

		// plain-prefix in msg found?
		GetPrivateProfileString("FiSH", "plain_prefix", "", plain_prefix, sizeof(plain_prefix), iniPath);
		if(plain_prefix[0] != 0)
		{
			i=strlen(plain_prefix);
			if(strnicmp(msgPtr, plain_prefix, i)==0)
			{
				ZeroMemory(theKey, sizeof(theKey));
				strcpy(bf_dest, msgPtr+i);
				strcpy(msgPtr, bf_dest);

				return strlen(send_buf);
			}
		}

		if(strncmp(theKey, "+OK ", 4)==0)
		{	// key is encrypted, lets decrypt
			decrypt_string(iniKey, theKey+4, bfKey, strlen(theKey+4));
			if(bfKey[0] == 0) return send_buf_len;	// don't process, decrypted key is bad
		}
		else strcpy(bfKey, theKey);

		ZeroMemory(theKey, sizeof(theKey));

		encrypt_string(bfKey, msgPtr, bf_dest, strlen(msgPtr)-1);	// -1 because we should not encrypt 0x0A

		ZeroMemory(bfKey, sizeof(bfKey));

		bf_dest[512]=0;

		strcpy(msgPtr, "+OK ");
		strcat(msgPtr, bf_dest);
		strcat(msgPtr, "\n");

		send_buf_len=strlen(send_buf);
	}


/*	fprintf(debugF, "\n%s\n", "***ALTERED***:");
	fwrite((char *)recv_buf, strlen((char *)recv_buf), 1, debugF);
	fclose(debugF);*/

	return send_buf_len;
}



// someone!ident@host.net PRIVMSG leetguy :Some Text -> Result: Rnick="someone"
int ExtractRnick (char *Rnick, char *incoming_msg)		// needs direct pointer to "nick@host" not ":nick@host" etc...
{
	int i=0, k=0;

	while(incoming_msg[i] != '!') {
		Rnick[k]=incoming_msg[i];
		k++;
		i++;
	}
	Rnick[k]=0;

	if (Rnick[0] != 0) return TRUE;
	else return FALSE;
}


// :someone!ident@host.net PRIVMSG #ECLiPSE :Some Text -> Result: Rhost="ident@host.net"
int ExtractRhost (char *Rhost, char *incoming_msg)
{
	int i=2, k=0;

	while(incoming_msg[i] != '!') i++;
	i++;

	while(incoming_msg[i]!=')' && incoming_msg[i]!=' ') {
		Rhost[k]=incoming_msg[i];
		k++;
		i++;
	}
	Rhost[k]=0;

	if (strlen(Rhost) > 4) return TRUE;
	else return FALSE;
}


// replace '[' and ']' from nick/channel with '~' (else problems with .ini files)
void FixContactName(char *contactName)
{
	int i=0;

	while(contactName[i] != 0)
	{
		if((contactName[i] == '[') || (contactName[i] == ']')) contactName[i]='~';
		i++;
	}
}
