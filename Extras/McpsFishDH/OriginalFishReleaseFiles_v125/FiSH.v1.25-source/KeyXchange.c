#include "FiSH.h"
#include <time.h>


// ### new sophie-germain 1080bit prime number ###
char *prime1080="++ECLiPSE+is+proud+to+present+latest+FiSH+release+featuring+even+more+security+for+you+++shouts+go+out+to+TMG+for+helping+to+generate+this+cool+sophie+germain+prime+number++++/C32L";
// Base16: FBE1022E23D213E8ACFA9AE8B9DFADA3EA6B7AC7A7B7E95AB5EB2DF858921FEADE95E6AC7BE7DE6ADBAB8A783E7AF7A7FA6A2B7BEB1E72EAE2B72F9FA2BFB2A2EFBEFAC868BADB3E828FA8BADFADA3E4CC1BE7E8AFE85E9698A783EB68FA07A77AB6AD7BEB618ACF9CA2897EB28A6189EFA07AB99A8A7FA9AE299EFA7BA66DEAFEFBEFBF0B7D8B
// Base10: 12745216229761186769575009943944198619149164746831579719941140425076456621824834322853258804883232842877311723249782818608677050956745409379781245497526069657222703636504651898833151008222772087491045206203033063108075098874712912417029101508315117935752962862335062591404043092163187352352197487303798807791605274487594646923


mIRC_export DH1080_gen(HWND mWnd, HWND aWnd, char *priv_and_pub_key, char *params, BOOL show, BOOL nopause)
{
	unsigned char raw_buf[160], iniHash[33];
    unsigned long seed, VolSer, len;

	POINT curPoint;
	HANDLE hINI;

    big b_privkey, b_prime, b_pubkey;
	csprng myRNG;

	PRNGAddEvent();

    b_privkey=mirvar(0);
    b_pubkey=mirvar(0);
    b_prime=mirvar(0);

	len=b64toh(prime1080, raw_buf);
	bytes_to_big(len, raw_buf, b_prime);

/*
b_2=mirvar(0);
b_SGp=mirvar(0);
if(isprime(b_prime))
{
	lgconv(2, b_2);
	divide(b_prime, b_2, b_SGp);
	if(isprime(b_SGp)) MessageBox(NULL, "YES, thats a sophie-germain prime!", "OMG!", MB_OK);
	bytes_to_big(len, raw_buf, b_prime);
}*/

	// #*#*#*#*#* RNG START #*#*#*#*#*
	time((time_t *)&seed);
	seed ^= GetTickCount();
	__asm {
		rdtsc
		xor seed, eax
	}
	PRNGAddEvent();
	GetVolumeInformation("C:\\", 0, 0, &VolSer, 0, 0, 0, 0);
	seed ^= VolSer;

	GetCursorPos(&curPoint);
	seed ^= curPoint.x;

	hINI = CreateFile(iniPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	seed += GetFileSize(hINI, NULL) * (curPoint.y + 1);
	seed ^= (unsigned int)hINI;
	CloseHandle(hINI);

	PRNGAddEvent();
	sha_file(iniPath, iniHash);
	PRNGAddEvent();
	PRNGOutputRandomBytes(raw_buf, sizeof(raw_buf));
	memXOR(raw_buf+128, iniHash, 32);
	sha_file(mIRCiniPath, iniHash);
	memXOR(raw_buf+128, iniHash, 32);
	ZeroMemory(iniHash, sizeof(iniHash));
	// first 128 byte in raw_buf: output from ECL_PRNG.lib
	// last 32 byte in raw_buf: SHA-256 digest from blow.ini and mirc.ini

	seed += GetTickCount();
	strong_init(&myRNG, sizeof(raw_buf), raw_buf, seed);
	strong_rng(&myRNG);
	strong_bigdig(&myRNG, 1080, 2, b_privkey);
	strong_kill(&myRNG);
	PRNGAddEvent();
	seed=0;
	// #*#*#*#*#* RNG END #*#*#*#*#*

    powltr(2, b_privkey, b_prime, b_pubkey);

	len=big_to_bytes(sizeof(raw_buf), b_privkey, raw_buf, FALSE);
	mirkill(b_privkey);
	htob64(raw_buf, priv_and_pub_key, len);

	strcat(priv_and_pub_key, " ");

	len=big_to_bytes(sizeof(raw_buf), b_pubkey, raw_buf, FALSE);
	htob64(raw_buf, priv_and_pub_key+strlen(priv_and_pub_key), len);
	ZeroMemory(raw_buf, sizeof(raw_buf));

	mirkill(b_pubkey);
	mirkill(b_prime);

	PRNGAddEvent();

    return 3;
}



mIRC_export DH1080_comp(HWND mWnd, HWND aWnd, char *pubkeys, char *params, BOOL show, BOOL nopause)
{
    int i=0, len;
	unsigned char SHA256digest[35], base64_tmp[160];
	big b_myPrivkey, b_HisPubkey, b_prime, b_theKey;

	PRNGAddEvent();

    b_myPrivkey=mirvar(0);
    b_HisPubkey=mirvar(0);
    b_theKey=mirvar(0);
    b_prime=mirvar(0);

	len=b64toh(prime1080, base64_tmp);
	bytes_to_big(len, base64_tmp, b_prime);

/*
b_2=mirvar(0);
b_SGp=mirvar(0);
if(isprime(b_prime))
{
	lgconv(2, b_2);
	divide(b_prime, b_2, b_SGp);
	if(isprime(b_SGp)) MessageBox(NULL, "YES, thats a sophie-germain prime!", "OMG!", MB_OK);
	bytes_to_big(len, base64_tmp, b_prime);
}*/

	while((pubkeys[i] != ' ') && (pubkeys[i] != 0x00)) i++;
	pubkeys[i]=0;

	// Verify base64 strings
	if((strspn(pubkeys, B64ABC) != strlen(pubkeys)) || (strspn(pubkeys+i+1, B64ABC) != strlen(pubkeys+i+1)))
	{
		memset(pubkeys, 0x20, strlen(pubkeys));
		return 0;
	}

	len=b64toh(pubkeys, base64_tmp);
	memset(pubkeys, 0x20, strlen(pubkeys));
	bytes_to_big(len, base64_tmp, b_myPrivkey);

	len=b64toh(pubkeys+i+1, base64_tmp);
	bytes_to_big(len, base64_tmp, b_HisPubkey);

	powmod(b_HisPubkey, b_myPrivkey, b_prime, b_theKey);
	mirkill(b_myPrivkey);

	len=big_to_bytes(sizeof(base64_tmp), b_theKey, base64_tmp, FALSE);
	mirkill(b_theKey);
	SHA256_memory(base64_tmp, len, SHA256digest);
	htob64(SHA256digest, pubkeys, 32);
	ZeroMemory(base64_tmp, sizeof(base64_tmp));
	ZeroMemory(SHA256digest, sizeof(SHA256digest));

	mirkill(b_HisPubkey);
	mirkill(b_prime);

	PRNGAddEvent();

	return 3;
}



// ### old DH1024 (should not be used anymore for security reasons) ###
//char *prime1024="CEF5048B11264893472E19AC7F95DFEDF20A539C7A39E801BDE0A459C874BA7E2A2CDFF0C8E11B323135315DC5FD26126D79F8EAC0CA5F05EADDE616096D9D66D3A96C783BCAA8E1E4FA0B81F802FBED66223FE08BB1736F54350F922575215C48BDFBE542EFE17487BA9DD230F95323972542602FACBC15FD733B795AB67F53";
char *prime1024="zvUEixEmSJNHLhmsf5Xf7fIKU5x6OegBveCkWch0un4qLN/wyOEbMjE1MV3F/SYSbXn46sDKXwXq3eYWCW2dZtOpbHg7yqjh5PoLgfgC++1mIj/gi7Fzb1Q1D5IldSFcSL375ULv4XSHup3SMPlTI5clQmAvrLwV/XM7eVq2f1M";

mIRC_export DH1024_gen(HWND mWnd, HWND aWnd, char *priv_and_pub_key, char *params, BOOL show, BOOL nopause)
{
	unsigned char raw_buf[160], iniHash[33];
    unsigned long seed, VolSer;

	POINT curPoint;
	HANDLE hINI;

    big b_privkey, b_prime, b_pubkey;
	csprng myRNG;

	PRNGAddEvent();

    b_privkey=mirvar(0);
    b_pubkey=mirvar(0);
    b_prime=mirvar(0);

	mip->IOBASE=64;
	cinstr(b_prime, prime1024);		// that is the main implementation flaw
									// using MIRACL base64 conversion, while prime1024 uses an incompatible base64 format

	// #*#*#*#*#* RNG START #*#*#*#*#*
	time((time_t *)&seed);
	seed ^= GetTickCount();
	__asm {
		rdtsc
		xor seed, eax
	}
	PRNGAddEvent();
	GetVolumeInformation("C:\\", 0, 0, &VolSer, 0, 0, 0, 0);
	seed ^= VolSer;

	GetCursorPos(&curPoint);
	seed ^= curPoint.x;

	hINI = CreateFile(iniPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	seed += GetFileSize(hINI, NULL) * (curPoint.y + 1);
	seed ^= (unsigned int)hINI;
	CloseHandle(hINI);

	PRNGAddEvent();
	sha_file(iniPath, iniHash);
	PRNGAddEvent();
	PRNGOutputRandomBytes(raw_buf, sizeof(raw_buf));
	memXOR(raw_buf+128, iniHash, 32);
	sha_file(mIRCiniPath, iniHash);
	memXOR(raw_buf+128, iniHash, 32);
	ZeroMemory(iniHash, sizeof(iniHash));
	// first 128 byte in raw_buf: output from ECL_PRNG.lib
	// last 32 byte in raw_buf: SHA-256 digest from blow.ini and mirc.ini

	seed += GetTickCount();
	strong_init(&myRNG, sizeof(raw_buf), raw_buf, seed);
	strong_rng(&myRNG);
	strong_bigdig(&myRNG, 1024, 2, b_privkey);
	strong_kill(&myRNG);
	PRNGAddEvent();
	ZeroMemory(raw_buf, sizeof(raw_buf));
	seed=0;
	// #*#*#*#*#* RNG END #*#*#*#*#*

    powltr(2, b_privkey, b_prime, b_pubkey);

	cotstr(b_privkey, priv_and_pub_key);
	mirkill(b_privkey);

	strcat(priv_and_pub_key, " ");

	cotstr(b_pubkey, priv_and_pub_key+strlen(priv_and_pub_key));

	mirkill(b_pubkey);
	mirkill(b_prime);

	PRNGAddEvent();

    return 3;
}



mIRC_export DH1024_comp(HWND mWnd, HWND aWnd, char *pubkeys, char *params, BOOL show, BOOL nopause)
{
    int i=0;
	unsigned char SHA256digest[35];
	big b_myPrivkey, b_HisPubkey, b_prime, b_theKey;

	PRNGAddEvent();

    b_myPrivkey=mirvar(0);
    b_HisPubkey=mirvar(0);
    b_theKey=mirvar(0);
    b_prime=mirvar(0);

	mip->IOBASE=64;
	cinstr(b_prime, prime1024);

	while((pubkeys[i] != ' ') && (pubkeys[i] != 0x00)) i++;
	pubkeys[i]=0;

	// Verify base64 strings
	if((strcspn(pubkeys, B64ABC) > 0) || (strcspn(pubkeys+i+1, B64ABC) > 0))
	{
		ZeroMemory(pubkeys, strlen(pubkeys));
		return 0;
	}

	cinstr(b_myPrivkey, pubkeys);
	cinstr(b_HisPubkey, pubkeys+i+1);

	powmod(b_HisPubkey, b_myPrivkey, b_prime, b_theKey);
	mirkill(b_myPrivkey);

	cotstr(b_theKey, pubkeys);
	mirkill(b_theKey);
	i=strlen(pubkeys);
	SHA256_memory(pubkeys, i, SHA256digest);
	htob64(SHA256digest, pubkeys, 32);
	ZeroMemory(SHA256digest, sizeof(SHA256digest));

	mirkill(b_HisPubkey);
	mirkill(b_prime);

	PRNGAddEvent();

	return 3;
}



void memXOR(char *s1, const char *s2, int n)
{
	while(n--) *s1++ ^= *s2++;
}
