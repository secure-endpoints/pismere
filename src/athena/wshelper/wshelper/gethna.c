/*
*	@doc RESOLVE
*
*	@module gethna.c  |
*
*	This file contains the function definitions for:
*		rgethostbyname,
*		rgethostbyaddr,
*		rdn_expand,
*		gethinfobyname,
*		getmxbyname,
*		getrecordbyname,
*		rrhost,
*		rgetservbyname,
*		WSHGetHostID
*	and some other internal functions called by these functions.
*
*	
*	WSHelper DNS/Hesiod Library for WINSOCK
*	
*/

/*
 * Copyright (c) 1985, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)gethostnamadr.c	6.48 (Berkeley) 1/10/93";
#endif /* LIBC_SCCS and not lint */

#include <windows.h>
#include <winsock.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "u-compat.h"

#ifdef _WIN32
#include <mitwhich.h>
#endif

#define MAXALIASES      35
#define MAXADDRS        35

static char *h_addr_ptrs[MAXADDRS + 1];

static struct hostent host;
static char *host_aliases[MAXALIASES];
static char hostbuf[BUFSIZ+1];
static struct in_addr host_addr;
static FILE *hostf = NULL;
static char hostaddr[MAXADDRS];
static char *host_addrs[2];
static int stayopen = 0;
static char *mxhostname[MAXMXRECS];

static struct mxent mx;	/* static return structure for MX records, c.f. resolv.h */
#if !defined (_WINDLL) && !defined (_WIN32)
char *strpbrk();
#endif

#ifdef _WINDLL
unsigned long WINAPI inet_aton(register const char *, struct in_addr *);
#endif

#if PACKETSZ > 1024
#define MAXPACKET       PACKETSZ
#else
#define MAXPACKET       1024
#endif

typedef union {
    HEADER hdr;
    u_char buf[MAXPACKET];
} querybuf;

typedef union {
    long al;
    char ac;
} align;

#if !defined (_WINDLL) && !defined (_WIN32)
int h_errno;
#endif

#ifdef _WINDLL
/* UNIXisms */
#define strcasecmp      stricmp
#define bcmp            memcmp
#endif

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifndef _PATH_HOSTS
#ifdef _WINDLL
#define _PATH_HOSTS "C:\\NET\\TCP\\HOSTS"
#else
#define _PATH_HOSTS "/etc/hosts"
#endif
#endif

extern int WINAPI hes_error( void );

#if (defined (_WINDLL) || defined (_WIN32) )&& defined(DEBUG)
/* For debugging output */
char debstr[80];
#endif

static struct hostent *
getanswer(querybuf *answer, int anslen, int iquery)
{
    register HEADER *hp;
    register u_char *cp;
    register int n;
    u_char *eom;
    char *bp, **ap;
    int type, qclass, buflen, ancount, qdcount;
    int haveanswer, getclass = C_ANY;
    char **hap;

#if defined (_WINDLL) || defined (_WIN32)
    int __dn_skipname(const u_char *, const u_char *);
#endif  

    eom = answer->buf + anslen;
    /*
     * find first satisfactory answer
     */
    hp = &answer->hdr;
    ancount = ntohs(hp->ancount);
    qdcount = ntohs(hp->qdcount);
    bp = hostbuf;
    buflen = sizeof(hostbuf);
    cp = answer->buf + sizeof(HEADER);
    if (qdcount) {
        if (iquery) {
            if ((n = rdn_expand((u_char *)answer->buf,
                                (u_char *)eom, (u_char *)cp, (u_char *)bp,
                                buflen)) < 0) {

#if !defined (_WINDLL) && !defined (_WIN32)
                h_errno = NO_RECOVERY;
#else
                WSASetLastError(WSANO_RECOVERY);
#endif
                return ((struct hostent *) NULL);
            }
            cp += n + QFIXEDSZ;
            host.h_name = bp;
            n = strlen(bp) + 1;
            bp += n;
            buflen -= n;
        } else
            cp += __dn_skipname(cp, eom) + QFIXEDSZ;
        while (--qdcount > 0)
            cp += __dn_skipname(cp, eom) + QFIXEDSZ;
    } else if (iquery) {
        if (hp->aa)
#if !defined (_WINDLL) && !defined (_WIN32)
            h_errno = HOST_NOT_FOUND;
#else
        WSASetLastError(WSAHOST_NOT_FOUND);
#endif
        else
#if !defined (_WINDLL) && !defined (_WIN32)
            h_errno = TRY_AGAIN;
#else
        WSASetLastError(WSATRY_AGAIN);
#endif
        return ((struct hostent *) NULL);
    }
    ap = host_aliases;
    *ap = NULL;
    host.h_aliases = host_aliases;
    hap = h_addr_ptrs;
    *hap = NULL;
#if BSD >= 43 || defined(h_addr)        /* new-style hostent structure */
    host.h_addr_list = h_addr_ptrs;
#endif
    haveanswer = 0;
    while (--ancount >= 0 && cp < eom) {
        if ((n = rdn_expand((u_char *)answer->buf, (u_char *)eom,
                            (u_char *)cp, (u_char *)bp, buflen)) < 0)
            break;
        cp += n;
        type = _getshort(cp);
        cp += sizeof(u_short);
        qclass = _getshort(cp);
        cp += sizeof(u_short) + sizeof(u_long);
        n = _getshort(cp);
        cp += sizeof(u_short);
        if (type == T_CNAME) {
            cp += n;
            if (ap >= &host_aliases[MAXALIASES-1])
                continue;
            *ap++ = bp;
            n = strlen(bp) + 1;
            bp += n;
            buflen -= n;
            continue;
        }
        if (iquery && type == T_PTR) {
            if ((n = rdn_expand((u_char *)answer->buf,
                                (u_char *)eom, (u_char *)cp, (u_char *)bp,
                                buflen)) < 0)
                break;
            cp += n;
            host.h_name = bp;
            return(&host);
        }
        if (iquery || type != T_A)  {
#ifdef DEBUG
            if (_res.options & RES_DEBUG)
#if !defined (_WINDLL) && !defined (_WIN32)
                printf("unexpected answer type %d, size %d\n",
                       type, n);
#else
            {
                wsprintf(debstr, "unexpected answer type %d, size %d\n",
                         type, n);
                OutputDebugString(debstr);
            }
#endif
#endif
            cp += n;
            continue;
        }
        if (haveanswer) {
            if (n != host.h_length) {
                cp += n;
                continue;
            }
            if (qclass != getclass) {
                cp += n;
                continue;
            }
        } else {
            host.h_length = n;
            getclass = qclass;
            host.h_addrtype = (qclass == C_IN) ? AF_INET : AF_UNSPEC;
            if (!iquery) {
                host.h_name = bp;
                bp += strlen(bp) + 1;
            }
        }

        bp += sizeof(align) - ((u_long)bp % sizeof(align));

        if (bp + n >= &hostbuf[sizeof(hostbuf)]) {
#ifdef DEBUG
            if (_res.options & RES_DEBUG)
#if !defined (_WINDLL) && !defined (_WIN32)
                printf("size (%d) too big\n", n);
#else                   
            {
                wsprintf(debstr, "size (%d) too big\n", n);
                OutputDebugString(debstr);
            }
#endif
#endif
            break;
        }
#if !defined (_WINDLL) && !defined (_WIN32)
        bcopy(cp, *hap++ = bp, n);
#else
        memcpy(*hap++ = bp, cp, n);
#endif
        bp +=n;
        cp += n;
        haveanswer++;
    }
    if (haveanswer) {
        *ap = NULL;
#if BSD >= 43 || defined(h_addr)        /* new-style hostent structure */
        *hap = NULL;
#else
        host.h_addr = h_addr_ptrs[0];
#endif
        return (&host);
    } else {
#if !defined (_WINDLL) && !defined (_WIN32)
        h_errno = TRY_AGAIN;
#define
        WSASetLastError(WSATRY_AGAIN);
#endif
        return ((struct hostent *) NULL);
    }
}


/*

  @func struct hostent * WINAPI | rgethostbyname |
  
Searches for information for a host with the
hostname specified by the character string parameter name.


  @parm const char *| name | A pointer to the hostname.


  @rdesc      The functions rgethostbyname(),  rgethostbyname_r(),  
  each return a pointer to a struct hostent if they  successfully  
  locate  the  requested entry; otherwise they return NULL.

*/
struct hostent *
#ifdef _WINDLL
WINAPI
#endif
#if !defined (_WINDLL) && !defined (_WIN32)
gethostbyname(const char *name)
#else
rgethostbyname(const char *name)
#endif
{
    querybuf buf;
    register const char *cp;
    int n;
    extern struct hostent *_gethtbyname();

#ifdef _WINDLL
    unsigned long WINAPI inet_aton(register const char *, struct in_addr *);
#endif
#ifdef _WIN32
    DWORD version;

    if (WhichOS(&version))
    {
        switch ( HIWORD(version) ) {
        case MS_OS_2000:
        case MS_OS_XP:
        case MS_OS_2003:
        case MS_OS_NT_UNKNOWN:
            /* use the Windows version of gethostbyname() */
            return gethostbyname(name);
        default:
            break;
        }
    }
#endif

    /*
     * disallow names consisting only of digits/dots, unless
     * they end in a dot.
     */
    if (isdigit(name[0]))
        for (cp = name;; ++cp) {
            if (!*cp) {
                if (*--cp == '.')
                    break;
                /*
                 * All-numeric, no dot at the end.
                 * Fake up a hostent as if we'd actually
                 * done a lookup.
                 */
                if (!inet_aton(name, &host_addr)) {
#if !defined (_WINDLL) && !defined (_WIN32)
                    h_errno = HOST_NOT_FOUND;
#else
                    WSASetLastError(WSAHOST_NOT_FOUND);
#endif
                    return((struct hostent *) NULL);
                }
                host.h_name = (char *)name;
                host.h_aliases = host_aliases;
                host_aliases[0] = NULL;
                host.h_addrtype = AF_INET;
                host.h_length = sizeof(u_long);
                h_addr_ptrs[0] = (char *)&host_addr;
                h_addr_ptrs[1] = (char *)0;
#if BSD >= 43 || defined(h_addr)        /* new-style hostent structure */
                host.h_addr_list = h_addr_ptrs;
#else
                host.h_addr = h_addr_ptrs[0];
#endif
                return (&host);
            }
            if (!isdigit(*cp) && *cp != '.') 
                break;
        }

    if ((n = res_search(name, C_IN, T_A, buf.buf, sizeof(buf))) < 0) {
#ifdef DEBUG
        if (_res.options & RES_DEBUG)
#if !defined (_WINDLL) && !defined (_WIN32)
            printf("res_search failed\n");
#else
        {
            wsprintf(debstr, "res_search failed\n");
            OutputDebugString(debstr);
        }
#endif
#endif

#if !defined (_WINDLL) && !defined (_WIN32)
        if (errno == ECONNREFUSED)
#else
            if (WSAGetLastError() == WSAECONNREFUSED)
#endif
                return (_gethtbyname(name));
            else
                return ((struct hostent *) NULL);
    }
    return (getanswer(&buf, n, 0));
}


/* @func struct hostent * WINAPI | rgethostbyaddr |

Our replacement for gethostbyaddr, called rgethostbyaddr These
functions are used to obtain entries describing hosts. Gethostbyaddr()
searches for information for a host with a given host address. The
parameter type specifies the family of the address. This should be one
of the address families defined in <lt> sys/socket.h <gt>. The
parameter addr must be a pointer to a buffer containing the
address. The address is given in a form specific to the address
family. See the NOTES section below for more information.  The
parameter len specifies the length of the buffer indicated by addr.

     
 @parm const char *| addr |The parameter addr must be a pointer to a
 buffer containing the address.  The address is given in a form
 specific to the address family.

 @parm int | len | The length of the address

 @parm int | type | The parameter type specifies the family of the address. 

*/
struct hostent *
#ifdef _WINDLL
WINAPI
#endif
#if !defined (_WINDLL) && !defined (_WIN32)
gethostbyaddr(const char *addr, int len, int type)
#else
rgethostbyaddr(const char *addr, int len, int type)		 
#endif
{
    int n;
    querybuf buf;
    register struct hostent *hp;
    char qbuf[MAXDNAME];
    extern struct hostent *_gethtbyaddr();
	
#ifdef _WINDLL
    int WINAPI res_query(char *, int, int, u_char *, int);
#endif
#ifdef _WIN32
    DWORD version;

    if (WhichOS(&version))
    {
        switch ( HIWORD(version) ) {
        case MS_OS_2000:
        case MS_OS_XP:
        case MS_OS_2003:
        case MS_OS_NT_UNKNOWN:
            /* use the Windows version of gethostbyaddr() */
            return gethostbyaddr(addr,len,type);
        default:
            break;
        }
    }
#endif

    if (type != AF_INET)
        return ((struct hostent *) NULL);
#if !defined (_WINDLL) && !defined (_WIN32)
    (void)sprintf
#else
        (void)wsprintf
#endif
        (qbuf, "%u.%u.%u.%u.in-addr.arpa",
         ((unsigned)addr[3] & 0xff),
         ((unsigned)addr[2] & 0xff),
         ((unsigned)addr[1] & 0xff),
         ((unsigned)addr[0] & 0xff));
    n = res_query(qbuf, C_IN, T_PTR, (char *)&buf, sizeof(buf));
    if (n < 0) {
#ifdef DEBUG
        if (_res.options & RES_DEBUG)
#if !defined (_WINDLL) && !defined (_WIN32)
            printf("res_query failed\n");
#else
        {
            wsprintf(debstr, "res_query failed\n");
            OutputDebugString(debstr);
        }
#endif
#endif

#if !defined (_WINDLL) && !defined (_WIN32)
        if (errno == ECONNREFUSED)
#else
            if (WSAGetLastError() == WSAECONNREFUSED)
#endif
                return (_gethtbyaddr(addr, len, type));
        return ((struct hostent *) NULL);
    }
    hp = getanswer(&buf, n, 1);
    if (hp == NULL)
        return ((struct hostent *) NULL);
    hp->h_addrtype = type;
    hp->h_length = len;
    h_addr_ptrs[0] = (char *)&host_addr;
    h_addr_ptrs[1] = (char *)0;
    host_addr = *(struct in_addr *)addr;
#if BSD < 43 && !defined(h_addr)        /* new-style hostent structure */
    hp->h_addr = h_addr_ptrs[0];
#endif
    return(hp);
}

short getshortint(LPSTR lpbuf)
{
    return ((short) (((short) *lpbuf * 256) + (short) *(lpbuf + 1)));
}

BOOL
dbgetanswer(querybuf *answer, int anslen, int iquery, LPSTR FAR *info)
{
    HEADER *hp;
    LPSTR cp;
    int n, haveanswer = 0;
    LPSTR eom;
    LPSTR bp; 
    int type, qclass, buflen, ancount, qdcount;
    int getclass = C_ANY;
#ifndef WINDOWS
    LPSTR FAR *hap;
#endif
    int h_length = -1;
    int mxcount = 0;    

    eom = answer->buf + anslen;
    /*
     * find first satisfactory answer
     */
    hp = &answer->hdr;
    ancount = ntohs(hp->ancount);
    qdcount = ntohs(hp->qdcount);
    bp = hostbuf;
    buflen = sizeof(hostbuf);
    cp = answer->buf + sizeof(HEADER);
    //time to deal with qdcount!!!!!
    if (qdcount) {
        if (iquery) {
            if ((n = rdn_expand (answer->buf, eom, cp, bp,
                                 buflen)) < 0)
                return FALSE;
            cp += n + QFIXEDSZ;
            n = lstrlen (bp) +1;
            bp += n;
            buflen -=n;
        }
        else
            cp += rdn_expand (answer ->buf, eom, cp, bp,
                              buflen) + QFIXEDSZ;
        while (--qdcount > 0)
            cp += rdn_expand (answer -> buf, eom, cp, bp,
                              buflen) + QFIXEDSZ;
    }
    else if (iquery)
        return FALSE;
    while (--ancount >= 0 && cp < eom) {
        if ((n = rdn_expand(answer->buf, eom, cp, bp, buflen)) < 0)
            break;
        cp += n;
        type = getshortint	(cp);
	    cp += sizeof(unsigned short);
	    qclass = getshortint  ( (LPSTR) cp);
	    cp += sizeof(unsigned short) + sizeof(unsigned long);
	    n = getshortint  ( (LPSTR) cp);
	    cp += sizeof(unsigned short);
	    if (type == T_CNAME) {
		cp += n;
		n = lstrlen (bp) + 1;
		bp += n;
		buflen -= n;
		continue;
		}

	    if (type == T_HINFO) {
		*info = (LPSTR) cp;
		cp += n;
		*cp = '\0';
		buflen -= n;
		haveanswer ++;
		continue;
	    }

	    if (type == T_MX) {
		if ((n = rdn_expand((LPSTR)answer->buf, eom,
			     cp+sizeof(short), bp, buflen)) < 0)
			{
			cp += n;       
			continue;
			}                  
			mx.pref[mxcount] = _getshort((LPSTR) cp);
		    mx.hostname = mxhostname;
		    mx.hostname[mxcount] = bp;
		    cp += (sizeof(short) + n);
		    n = lstrlen(bp) + 1;
		    bp += n;
		    buflen -= n;
		    mx.numrecs = ++mxcount;
		    *info = (LPSTR) &mx;
		    haveanswer++;
		    continue;
	    }
	    if (iquery || type != T_A) {
		cp += n;
		continue;
		}
	    if (haveanswer) {
		if (n!= h_length) {
		    cp += n;
		    continue;
		    }
		if (qclass != getclass) {
		    cp += n;
		    continue;
		    }
		}
	    else {
		h_length = n;
		getclass = qclass;
		if (!iquery) {
		    //
		    bp += lstrlen (bp) + 1;
		    }
		}

	    bp += sizeof (align) - ((unsigned long) bp % sizeof (align));

	    if (bp + n >= &hostbuf [sizeof(hostbuf)])
		break;
	    bp += n;
	    cp += n;
	    haveanswer++;
	}

    return (haveanswer);
}


/*  

  @doc MISC

  @func LPSTR WINAPI | gethinfobyname | Given the name
  of a host query the nameservers for the T_HINFO information
  associated with the host.
  
  @parm LPSTR | name | pointer to the name of the host that the query is about.

  @rdesc NULL or a pointer to the T_HINFO.

*/

LPSTR
#ifdef _WINDLL
WINAPI
#endif
gethinfobyname(LPSTR name)
{
    querybuf buf;
    LPSTR cp;
    int n;
    LPSTR info;

    /*
     * disallow names consisting only of digits/dots, unless
     * they end in a dot.
     */
    if (isdigit(name[0]))
        for (cp = name;; ++cp) {
            if (!*cp) {
                if (*--cp == '.')
                    break;
                return (NULL);
            }
            if (!isdigit(*cp) && *cp != '.') 
                break;
        }

    n = res_search(name, C_IN, T_HINFO, buf.buf, sizeof(querybuf));
    if (n < 0) {
        return (NULL);
    }

    if(dbgetanswer(&buf, n, 0, &info))
        return(info);
    else
        return NULL;
}


/*

  @func struct mxent FAR * WINAPI | getmxbyname | This
  function will query the nameservers for the MX records associated
  with the given hostname. Note that the return is a pointer to the
  mxent structure so an application making this call can iterate
  through the different records returned and can also reference the
  preference information associated with each hostname returned.
  
  @parm LPSTR | name | The name of the host for which we want MX records.

  @rdesc NULL or a pointer to a mxent structure.

 */

struct mxent FAR *
#ifdef _WINDLL
WINAPI
#endif
getmxbyname(LPSTR name)
{
    querybuf buf;
    LPSTR cp;
    int n;
    struct mxent FAR *info;

    /*
     * disallow names consisting only of digits/dots, unless
     * they end in a dot.
     */
    if (isdigit(name[0]))
        for (cp = name;; ++cp) {
            if (!*cp) {
                if (*--cp == '.')
                    break;
                return (NULL);
            }
            if (!isdigit(*cp) && *cp != '.') 
                break;
        }

    n = res_search(name, C_IN, T_MX, buf.buf, sizeof(querybuf));
    if (n < 0) {
        return (NULL);
    }

    if(dbgetanswer(&buf, n, 0, (LPSTR FAR *) &info))
        return(info);
    else
        return NULL;
}


/*

  @func LPSTR WINAPI | getrecordbyname | This function
  will query the nameservers about the given hostname for and DNS
  record type that the application wishes to query.
  
  @parm LPSTR | name | a pointer to the hostname

  @parm int | rectype | a DNS record type, e.g. T_MX, T_HINFO, ...

  @rdesc The return is NULL or a pointer to a string containing the
  data returned. It is up to the calling application to parse the
  string appropriately for the rectype queried.

*/

LPSTR
#ifdef _WINDLL
WINAPI
#endif
getrecordbyname(LPSTR name, int rectype)
{
    querybuf buf;
    LPSTR cp;
    int n;
    LPSTR info;
    BOOL ans;

#ifdef _WINDLL
    int WINAPI res_query(char *, int, int, u_char *, int);
#endif

    /*
     * disallow names consisting only of digits/dots, unless
     * they end in a dot.
     */
    if (isdigit(name[0]))
        for (cp = name;; ++cp) {
            if (!*cp) {
                if (*--cp == '.')
                    break;
                return (NULL);
            }
            if (!isdigit(*cp) && *cp != '.') 
                break;
        }

    n = res_query(name, C_IN, rectype, buf.buf, MAXPACKET);
    if (n < 0) {
        return (NULL);
    }

    ans = dbgetanswer (&buf, n, 0, &info);
    if(ans)
    {
        return(info);
    }
    else
        return NULL;
}


#ifdef _WINDLL
void
#endif
_sethtent(int f)
{
    if (hostf == NULL)
        hostf = fopen(_PATH_HOSTS, "r" );
    else
        rewind(hostf);
    stayopen |= f;
}

#ifdef _WINDLL
void
#endif
_endhtent()
{
    if (hostf && !stayopen) {
        (void) fclose(hostf);
        hostf = NULL;
    }
}

struct hostent *
_gethtent()
{
    char *p;
    register char *cp, **q;

    if (hostf == NULL && (hostf = fopen(_PATH_HOSTS, "r" )) == NULL)
        return (NULL);
again:
    if ((p = fgets(hostbuf, BUFSIZ, hostf)) == NULL)
        return (NULL);
    if (*p == '#')
        goto again;
    cp = strpbrk(p, "#\n");
    if (cp == NULL)
        goto again;
    *cp = '\0';
    cp = strpbrk(p, " \t");
    if (cp == NULL)
        goto again;
    *cp++ = '\0';
    /* THIS STUFF IS INTERNET SPECIFIC */
#if BSD >= 43 || defined(h_addr)        /* new-style hostent structure */
    host.h_addr_list = host_addrs;
#endif
    host.h_addr = hostaddr;
    *((u_long *)host.h_addr) = inet_addr(p);
    host.h_length = sizeof (u_long);
    host.h_addrtype = AF_INET;
    while (*cp == ' ' || *cp == '\t')
        cp++;
    host.h_name = cp;
    q = host.h_aliases = host_aliases;
    cp = strpbrk(cp, " \t");
    if (cp != NULL) 
        *cp++ = '\0';
    while (cp && *cp) {
        if (*cp == ' ' || *cp == '\t') {
            cp++;
            continue;
        }
        if (q < &host_aliases[MAXALIASES - 1])
            *q++ = cp;
        cp = strpbrk(cp, " \t");
        if (cp != NULL)
            *cp++ = '\0';
    }
    *q = NULL;
    return (&host);
}

struct hostent *
_gethtbyname(char *name)
{
    register struct hostent *p;
    register char **cp;
	
    _sethtent(0);
    while (p = _gethtent()) {
        if (strcasecmp(p->h_name, name) == 0)
            break;
        for (cp = p->h_aliases; *cp != 0; cp++)
            if (strcasecmp(*cp, name) == 0)
                goto found;
    }
found:
    _endhtent();
    return (p);
}

struct hostent *
_gethtbyaddr(const char *addr, int len, int type)
{
    register struct hostent *p;

    _sethtent(0);
    while (p = _gethtent())
        if (p->h_addrtype == type && !bcmp(p->h_addr, addr, len))
            break;
    _endhtent();
    return (p);
}


/*

  @func DWORD WINAPI | rrhost | This function emulates the
  rhost function that was part of Excelan / Novell's LAN WorkPlace TCP/IP API.
  Given a pointer to an IP hostname it will return the IP address as a 32 bit
  integer.

  
  @parm LPSTR | lpHost | a pointer to the hostname.

  @rdesc 0 or the IP address as a 32 bit integer.
  
*/

DWORD WINAPI rrhost( LPSTR lpHost )
{
    struct hostent FAR *host_entry;
    static DWORD addr;

    host_entry = rgethostbyname( lpHost );      
    if( host_entry == NULL ){
	return((DWORD) 0);
    }
    _fmemcpy((LPSTR) &addr, (LPSTR) host_entry->h_addr,
	   host_entry->h_length);
    return( addr );
}


/*

  @doc RESOLVE

  @func struct servent FAR * WINAPI | rgetservbyname |
  This is equivalent to getservbyname but instead of just looking up
  the service entry in the local host file it will first query the
  Hesiod server(s) for the information. This allows a site to add new
  services in a centrally managed fashion instead of relying on
  propagation of a new services file to all machines on the network.

  If the Hesiod query fails the function will fall back to the Winsock
  version of getservbyname which is usually implementated as a simple
  lookup in a local host file.
  
  @parm LPCSTR | name | a pointer to the service name to look up.
  
  @parm LPCSTR | proto | which protcol do you want for the service
  (UDP or TCP)?

  @rdesc It will a pointer the servent structure filled with data
  received from the Hesiod server or it will return the result of the
  Winsock getservbyname function.

*/

struct servent FAR * WINAPI rgetservbyname(LPCSTR name, LPCSTR proto)
{
    struct servent FAR * WINAPI hes_getservbyname(LPCSTR name, LPCSTR proto);
    struct servent FAR *tmpent;

    tmpent = hes_getservbyname(name, proto);
    return (!hes_error()) ? tmpent : getservbyname(name, proto);
}



/* getting local host IP address 

@doc MISC

@func LONG FAR | WSHGetHostID | Get the local IP address using
the following algorithm:<NL>

	- get local host name with gethostname()<nl>
	- attempt to resolve local host name with gethostbyname()<nl>
If it fails:<nl>
	- get a UDP socket<nl>
	- connect UDP socket to aribitrary address and port<nl>
	- use getsockname() to get local address<nl>

@comm This function code was borrowed from the book, "Windows Sockets
Network Programming" by Bob Quinn and Dave Shute, Copyright 1996
Addision-Welsley Publishing Company, Inc., ISBN 0-201-63372-8

*/
LONG FAR WSHGetHostID()
{
    char szLclHost [MAXHOSTNAME];
    LPHOSTENT lpstHostent;
    SOCKADDR_IN stLclAddr;
    SOCKADDR_IN stRmtAddr;
    int nAddrSize = sizeof( SOCKADDR);
    SOCKET hSock;
    int nRet;

    /* init local address (to zero) */
    stLclAddr.sin_addr.s_addr = INADDR_ANY;

    /* get local host name */
    nRet = gethostname(szLclHost, MAXHOSTNAME);
    if( nRet != SOCKET_ERROR){
        /* resolve host name for local address */
        lpstHostent = gethostbyname((LPSTR) szLclHost);
        if( lpstHostent ){
            stLclAddr.sin_addr.s_addr = *((u_long FAR*) (lpstHostent->h_addr));
        }
    }

    /* if still not resolved, then try second strategy */
    if( stLclAddr.sin_addr.s_addr == INADDR_ANY){
        /* get a UDP socket */
        hSock = socket(AF_INET, SOCK_DGRAM, 0);
        if( hSock != INVALID_SOCKET ){
            /* connect to arbirary port and address (NOT loopback) */
            stRmtAddr.sin_family = AF_INET;
            stRmtAddr.sin_port = htons( IPPORT_ECHO );
            stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1");
            nRet = connect( hSock,
                            (LPSOCKADDR) &stRmtAddr,
                            sizeof(SOCKADDR));
            if( nRet != SOCKET_ERROR ){
				/* gte local address */
                getsockname( hSock,
                             (LPSOCKADDR) &stLclAddr,
                             (int FAR*)&nAddrSize);
            }
            closesocket(hSock); /* we're done with the socket */
        }
    }
    return( stLclAddr.sin_addr.s_addr);
} /* WSHGetHostID() */

