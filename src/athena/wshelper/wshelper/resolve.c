/*
 *	@doc RESOLVE
 *
 *	
 * @module resolve.c | Contains the implementation of resolve and some
 * internally defined functions.
 *
 * WSHelper DNS/Hesiod Library for WINSOCK
 */

#define _RESOLVE_C_
#include <stdio.h>

#include <windows.h>
#include <string.h>
#include <errno.h>
#include "u-compat.h"

#include <sys/types.h>
#include <winsock.h>

#include <arpa/nameser.h>
#include <resolv.h>
#include "resscan.h"


unsigned short getshort(LPSTR msgp);
#define _getshort getshort


#define USE_PACKETSZ_HACK
/* this deals with evil nameservers... */
#ifdef  USE_PACKETSZ_HACK
#define PACKETSZ_HACK 1500
#else
#define PACKETSZ_HACK PACKETSZ
#endif

#ifndef _CADDR_T_DEF /* pbh 12-3-91 */
#ifndef caddr_t
#ifndef WINDOWS
typedef char * caddr_t;
#else
typedef char far * caddr_t;
#endif
#define _CADDR_T_DEF
#endif
#endif

#if defined( MSDOS ) || defined( WIN32 )
#ifndef u_short
#define u_short unsigned short
#endif
#ifndef u_long
#define u_long unsigned long
#endif
#ifndef u_char
#define u_char unsigned char
#endif
#endif

#define DEF_RETRANS 4
#define DEF_RETRY 3
#if !defined( MSDOS ) && !defined( WIN32 )
extern int errno;
#endif

int dn_skip (LPSTR comp_dn);
#ifndef WINDOWS
nsmsg_p _resolve(char *name, int class, int type, retransXretry_t patience);
#else
nsmsg_p FAR _resolve(LPSTR name, int class, int type, retransXretry_t patience);
#define strncpy _fstrncpy
#endif

static caddr_t
rr_scan(char FAR *cp, rr_t FAR *rr)
{
    register int n;

    if ((n = dn_skip(cp)) < 0) {
        errno = EINVAL;
        return((char *)NULL);
    }

    cp += n;
    rr->type = _getshort(cp);
    cp += sizeof(u_short/*type*/);

    rr->class = _getshort(cp);
    cp += sizeof(u_short/*class*/) + sizeof(u_long/*ttl*/);

    rr->dlen = (int) _getshort(cp);
    rr->data = cp + sizeof(u_short/*dlen*/);

    return(rr->data + rr->dlen);
}

nsmsg_p
res_scan(char FAR *msg)
{
    static char FAR bigmess[sizeof(nsmsg_t) + sizeof(rr_t)*((PACKETSZ_HACK-sizeof(HEADER))/RRFIXEDSZ)];
    static char FAR datmess[PACKETSZ_HACK-sizeof(HEADER)];
    register char FAR *cp;
    register rr_t FAR *rp;
    register HEADER FAR *hp;
    register char FAR *data = datmess;
    register int n, n_an, n_ns, n_ar, nrec;
    register nsmsg_t FAR *mess = (nsmsg_t FAR *)bigmess;

    hp = (HEADER FAR *)msg;
    cp = msg + sizeof(HEADER);
    n_an = ntohs(hp->ancount);
    n_ns = ntohs(hp->nscount);
    n_ar = ntohs(hp->arcount);
    nrec = n_an + n_ns + n_ar;

    mess->len = 0;
    mess->hd = hp;
    mess->ns_off = n_an;
    mess->ar_off = n_an + n_ns;
    mess->count = nrec;
    rp = &mess->rr;

    /* skip over questions */
    if (n = ntohs(hp->qdcount)) {
        while (--n >= 0) {
            register int i;
	    if ((i = dn_skip(cp)) < 0)
                return((nsmsg_t *)NULL);
            cp += i + (sizeof(u_short/*type*/) + sizeof(u_short/*class*/));
        }
    }

    /* scan answers */
    if (n = n_an) {
        while (--n >= 0) {
            if ((cp = rr_scan(cp, rp)) == NULL)
                return((nsmsg_t *)NULL);
            (void) strncpy(data, rp->data, rp->dlen);
            rp->data = data;
            data += rp->dlen;
            *data++ = '\0';
            rp++;
        }
    }

    /* scan name servers */
    if (n = n_ns) {
        while (--n >= 0) {
            if ((cp = rr_scan(cp, rp)) == NULL)
                return((nsmsg_t *)NULL);
            (void) strncpy(data, rp->data, rp->dlen);
            rp->data = data;
            data += rp->dlen;
            *data++ = '\0';
            rp++;
        }
    }

    /* scan additional records */
    if (n = n_ar) {
        while (--n >= 0) {
            if ((cp = rr_scan(cp, rp)) == NULL)
                return((nsmsg_t *)NULL);
            (void) strncpy(data, rp->data, rp->dlen);
            rp->data = data;
            data += rp->dlen;
            *data++ = '\0';
            rp++;
        }
    }
    /* !!!! dangerous under MSDOS */
#ifndef _WINDLL
    mess->len = (int)cp - (int)msg;
#else
    mess->len = (int) ((long)cp - (long)msg);
#endif

    return(mess);
}

/*
 * Resolve name into data records
 */
#ifndef WINDOWS
nsmsg_p
#else
nsmsg_p FAR
#endif
_resolve(LPSTR name, int class, int type, retransXretry_t patience)
{
    static char FAR qbuf[PACKETSZ];
    static char FAR abuf[PACKETSZ_HACK];
    register int n;
#ifndef WINDOWS
    register long res_options = _res.options;
    register int res_retrans = _res.retrans;
    register int res_retry = _res.retry;
#else
#ifdef LWP
    DWORD res_options;
    DWORD old_res_options;
#endif
#endif /* WINDOWS */

#ifdef WINDOWS
    /************
    res_options = res_opts(RES_STATUS, 0L);
    old_res_options = res_options;
    *************/
#endif

#ifdef DEBUG
    if (res_options & RES_DEBUG)
#ifndef _WINDLL
	printf("_resolve: class = %d, type = %d\n", class, type);
#else
    {
        wsprintf(debstr, "_resolve: class = %d, type = %d\n", class, type);
        OutputDebugString(debstr);
    }
#endif
#endif

    if (class < 0 || type < 0) {
        errno = EINVAL;
        return((nsmsg_t *)NULL);
    }

#ifndef WINDOWS
    _res.options |= RES_IGNTC;
#else
    /**************
    res_options  |= RES_IGNTC;
    res_options = res_opts( RES_SETSTATE, res_options );
    ****************/
#endif /* WINDOWS */

    n = res_mkquery(QUERY, name, class, type, (char *)0, 0, NULL, qbuf, PACKETSZ);
    if (n < 0) {
        errno = EMSGSIZE;
        return((nsmsg_t *)NULL);
    }

#ifndef WINDOWS
    _res.retrans = (patience.retrans ? patience.retrans : DEF_RETRANS);
    _res.retry = (patience.retry ? patience.retry : DEF_RETRY);
#endif /* WINDOWS */

//#ifndef WINDOWS
    n = res_send(qbuf, n, abuf, sizeof(abuf));
//#else
//    n = res_send((LPSTR) qbuf, n, (LPSTR) abuf, sizeof(abuf));
//#endif

#ifndef WINDOWS
    _res.options = res_options;
    _res.retrans = res_retrans;
    _res.retry = res_retry;
#else
    /* res_options = res_opts( RES_SETSTATE, old_res_options ); */
#endif /* WINDOWS */


    if (n < 0) {
        errno = ECONNREFUSED;
        return((nsmsg_t *)NULL);
    }

    return(res_scan(abuf));
}


/*
 * Skip over a compressed domain name. Return the size or -1.
 */
static
int
dn_skip(LPSTR comp_dn)
{
#ifndef WINDOWS
    register char *cp;
#else
    LPSTR cp;
#endif
    register int n;

    cp = comp_dn;
    while (n = *cp++) {
        /*
         * check for indirection
         */
        switch (n & INDIR_MASK) {
        case 0:		/* normal case, n == len */
            cp += n;
            continue;
        case INDIR_MASK:	/* indirection */
            cp++;
            break;
        default:	/* illegal type */
            return (-1);
        }
        break;
    }
    return (cp - comp_dn);
}

#ifdef WINDOWS
unsigned short getshort(LPSTR msgp)
{
    register unsigned char FAR *p = (unsigned char FAR *) msgp;
    register unsigned short u;

    u = *p++ << 8;
    return ((unsigned short) (u | *p));
}
#endif
