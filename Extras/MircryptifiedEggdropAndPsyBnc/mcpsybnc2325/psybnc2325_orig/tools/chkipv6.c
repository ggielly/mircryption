/************************************************************************
 *   psybnc2.2.2, tools/chkipv6.c
 *   Copyright (C) 2001 the most psychoid  and
 *                      the cool lam3rz IRC Group, IRCnet
 *			http://www.psychoid.lam3rz.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* this program gets compiled, if IPv6 is supported. */

#ifndef lint
static char rcsid[] = "@(#)$Id: chkipv6.c,v 1.1.1.1 2005/02/06 12:56:20 hisi Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>

#ifdef SUNOS
int error_num;
#endif

int main()
{
    int lsock;
    struct sockaddr_in6 sin;
    struct hostent *he;

    lsock = socket (AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if(lsock==-1)
	printf("No Interface\n");

    exit(0x0);
#ifdef SUNOS
    he = getipnodebyname("www.freenet6.org", AF_INET6,AI_DEFAULT,&error_num);
#else
    he = gethostbyname2("www.freenet6.org", AF_INET6);
#endif

    exit(0x0);
}
