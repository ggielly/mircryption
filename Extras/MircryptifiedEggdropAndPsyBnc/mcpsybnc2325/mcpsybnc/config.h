/*
 * Configuration file for psyBNC, created by menuconf
 */
// conditionally-compiled mircryption code added 1/15/05 by mouser

/* Encryption */

#define CRYPT

/* Encryption Type*/

#define BLOWFISH

/* Allow Translation */

#define TRANSLATE

/* Allow internal network */

#define INTNET

/* Allow traffic logging */

#define TRAFFICLOG

/* Allow linkage of bouncers */

#define LINKAGE

/* Allow the dcc File-Functions */

#define DCCFILES

/* Pipe dcc Chats */

#define DCCCHAT

/* Allow to add more users */

#define MULTIUSER

/* Number of max. Users */

#define MAXUSER 50

/* Number of max. Connections per User */

#define MAXCONN 99

/* Allow the usage of scripts */

#define SCRIPTING

/* Allow multiple irc connections per user */

#define NETWORK

/* Allow Proxy Support */

#define PROXYS

/* The logging level */

#define LOGLEVEL 0

/* SSL-Security */

#define SSLSEC 2

/* Blocking DNS is preferred. Non Blocking DNS is experimental */

#ifndef BLOCKDNS
#define BLOCKDNS
#endif


// 1/16/05, mouser, enable mircryption code compiling
#define COMPILEMIRCRYPTION

// 1/16/05, mouser, let user disable ssl support if they want
//#define NOSSL

// 1/15/05, mouser, i dont think normal users should be able to run bwho (see p_client.c for code)
//#define ONLYADMINSBWHO


