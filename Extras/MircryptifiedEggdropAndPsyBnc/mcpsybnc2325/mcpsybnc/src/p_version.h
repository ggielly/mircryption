#define APPNAME "psyBNC"
#define APPVER "2.3.2-5"

#ifdef P_MAIN

static char ver[70];

char *buildversion()
{
    strcpy(ver,APPNAME);
    strcat(ver,APPVER);
    strcat(ver,"-");
#ifdef CRYPT
    strcat(ver,"c");
#endif
#ifdef BLOWFISH
    strcat(ver,"B");
#endif
#ifdef IDEA
    strcat(ver,"I");
#endif
#ifdef TRANSLATE
    strcat(ver,"t");
#endif
#ifdef INTNET
    strcat(ver,"I");
#endif
#ifdef PARTYCHAN
    strcat(ver,"P");
#endif
#ifdef TRAFFICLOG
    strcat(ver,"T");
#endif
#ifdef LINKAGE
    strcat(ver,"L");
#endif
#ifdef DCCFILES
    strcat(ver,"d");
#endif
#ifdef DCCCHAT
    strcat(ver,"D");
#endif
#ifdef MULTIUSER
    strcat(ver,"M");
#endif
#ifdef SCRIPTING
    strcat(ver,"S");
#endif
#ifdef OIDENTD
    strcat(ver,"o");
#endif
#ifdef SHAREBANS
    strcat(ver,"s");
#endif
#ifdef NETWORK
    strcat(ver,"N");
#endif
#ifdef PROXYS
    strcat(ver,"p");
#endif
#ifdef ANONYMOUS
    strcat(ver,"A");
#endif
#ifdef DYNAMIC
    strcat(ver,"D");
#endif
#ifdef HAVE_SSL
    strcat(ver,"E");
#endif
#ifndef BLOCKDNS
    strcat(ver,"r");
#endif

// mouser 1/15/05, when compiled for mircryption add a [mc] prefix to version string
#ifdef COMPILEMIRCRYPTION
    strcat(ver,"[mc]");
#endif

    return ver;
}

#endif
