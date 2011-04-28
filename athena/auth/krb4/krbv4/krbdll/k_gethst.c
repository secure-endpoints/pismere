/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/k_gethst.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef lint
static char rcsid_k_gethostname_c[] =
"$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/k_gethst.c,v 1.1 1999/03/12 23:05:46 dalmeida Exp $";
#endif /* lint */

#ifdef WINDOWS
#include <windows.h>
#endif
#include <mit_copy.h>
#include <string.h>
#ifdef OS2
#include <netdb.h>
#endif
#ifdef WINSOCK
#include <winsock.h>
#endif
#ifdef LWP
#include <sys/socket.h>
#endif

#ifndef PC
#ifndef BSD42
#ifndef OS2
#ifndef WINSOCK
#error teach me how to k_gethostname for your system here
#endif
#endif
#endif
#endif

/*
 * Return the local host's name in "name", up to "namelen" characters.
 * "name" will be null-terminated if "namelen" is big enough.
 * The return code is 0 on success, -1 on failure.  (The calling
 * interface is identical to gethostname(2).)
 *
 * Currently defined for BSD 4.2 and PC.  The BSD version just calls
 * gethostname(); the PC code was taken from "kinit.c", and may or may
 * not work.
 */

int k_gethostname(name, namelen)
    char *name;
{
#ifdef LWP
    struct in_addr ip_addr;
    char *lps,*p;

    if (namelen <= 0) {
       return (-1);
    }

    if (-1 == (ip_addr.s_addr = getmyipaddr())) {
       return (-1);
    }

    lps=raddr(ip_addr.s_addr);
    if (!lps)
       lps = inet_ntoa(ip_addr);

    p=lps;
    do {
       *p=tolower(*p);
       if (*p=='.') {
	  *p='\0';
	  break;
       }
    } while (*p++);
    strcpy(name, lps);

    return (0);

#else
#if defined(PC) && !defined(BSDSOCK)
	char *p="localhost";
	if (rhost(&p)==-1)
		return -1;
	strncpy(name,p,namelen);
	return 0;
#else
#if defined(BSD42) || defined(BSDSOCK)
    int rt;
    rt=gethostname(name, namelen);
#ifdef OS2
    if (rt) psock_errno(NULL);
#else
	if (rt) rt=WSAGetLastError();
#endif
    return rt;
#endif /* BSD42 */
#endif /* PC */
#endif /* LWP */
}

