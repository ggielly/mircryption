#include "ECL-PRNG\prng_clib.h"
#include "MIRACL.H"


miracl *mip;
unsigned char iniPath[255], randomPath[255], mIRCiniPath[255];

#define mIRC_export extern __declspec(dllexport) int __stdcall

int decrypt_string(char *key, char *str, char *dest, int len);
int encrypt_string(char *key, char *str, char *dest, int len);

void initb64();
extern unsigned char B64ABC[];
int b64toh(char *b, char *d);
int htob64(char *h, char *d, unsigned int l);

void SHA256_memory(unsigned char *buf, int len, unsigned char *hash);
int sha_file(unsigned char *filename, unsigned char *hash);

int ExtractRhost (char *Rhost, char *incoming_msg);
int ExtractRnick (char *Rnick, char *incoming_msg);
void FixContactName(char *contactName);		// replace '[' and ']' in nick/channel with '~'

void memXOR(char *s1, const char *s2, int n);
