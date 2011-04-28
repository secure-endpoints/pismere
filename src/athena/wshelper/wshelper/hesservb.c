/*
 *	@doc HESIOD
 *
 * @module hesservb.c |
 *	
 *
 *	  Contains the definition for hes_getservbyname,
 *
 *	  WSHelper DNS/Hesiod Library for WINSOCK
 *	
 */

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getservbyname.c	5.3 (Berkeley) 5/19/86";
#endif /* LIBC_SCCS and not lint */

#include <hesiod.h>
#include <windows.h>
#include <winsock.h>
//#define register

#include "u-compat.h"


#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>

#define cistrcmp stricmp

#define LISTSIZE 15


/*
@func struct servent * WINAPI | hes_getservbyname |
This function will query a Hesiod server for a servent structure given
a service name and protocol. This is a replacement for the Winsock
getservbyname function which normally just uses a local services
file. This allows a site to use a centralized database for adding new
services.

@parm char * | name | pointer to the official name of the service, eg "POP3".
@parm char * | proto | pointer to the protocol to use when contacting the 
                       service, e.g. "TCP"

  @rdesc NULL or a pointer to a servent structure.

*/

struct servent *
#ifdef _WINDLL
WINAPI
#endif
hes_getservbyname(char *name, char *proto)
{
	register struct servent *p;
	register char *l, **cp;
	register int i = 0;
	static char *aliases[LISTSIZE];

	cp = hes_resolve(name, "service");
	if (cp == NULL) return(NULL);
	p = (struct servent *) malloc(sizeof(struct servent));
	while (l = *cp++) {
		register char *servicename, *protoname, *port;

		while(*l && (*l == ' ' || *l == '\t')) l++;
		servicename = l;
		while(*l && *l != ' ' && *l != '\t' && *l != ';') l++;
		if (*l == '\0') continue; /* malformed entry */
		*l++ = '\0';
		while(*l && (*l == ' ' || *l == '\t')) l++;
		protoname = l;
		while(*l && *l != ' ' && *l != ';') l++;
		if (*l == '\0') continue; /* malformed entry */
		*l++ = '\0';
		if (cistrcmp(proto, protoname)) continue; /* wrong port */
		while(*l && (*l == ' ' || *l == '\t' || *l == ';')) l++;
		if (*l == '\0') continue; /* malformed entry */
		port = l;
		while(*l && (*l != ' ' && *l != '\t' && *l != ';')) l++;
		if (*l) *l++ = '\0';
		if (*l != '\0') {
			do {
				aliases[i++] = l;
				while(*l && !isspace(*l)) l++;
				if (*l) *l++ = 0;
			} while(*l);
		}
		aliases[i] = NULL;
		p->s_name = servicename;
		p->s_port = htons((u_short)atoi(port));
		p->s_proto = protoname;
		p->s_aliases = aliases;
		return (p);
	}
	return(NULL);
}
