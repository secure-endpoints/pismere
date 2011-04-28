/*
 * Copyright 1987 - 1999 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <windows.h>
#include <sys\types.h>

#include <krb.h>
#include <prot.h>

#include <stdio.h>
#include <errno.h>

#include <winsock.h>

#ifdef WSHELPER
#include <wshelper.h>
#define GETHOSTBYNAME rgethostbyname
#else
#define GETHOSTBYNAME gethostbyname
#endif

#include <string.h> 
#include <stdlib.h>

#define S_AD_SZ sizeof(struct sockaddr_in)

extern int krb_debug;                         

static int krb_udp_port_conf;
#ifdef USE_DNS
extern int krb_udp_port_dns;
#endif /* USE_DNS */

//----------------------------------------------------

/* CLIENT_KRB_TIMEOUT indicates the time to wait before
 * retrying a server.  It's defined in "krb.h".
 */
static struct timeval timeout = { CLIENT_KRB_TIMEOUT, 0};
static char *prog = "send_to_kdc";

static
send_recv(KTEXT pkt, KTEXT rpkt, SOCKET f,
          struct sockaddr_in *_to, struct hostent *addrs);

/*
 * This file contains two routines, send_to_kdc() and send_recv().
 * send_recv() is a static routine used by send_to_kdc().
 */
        
/*
 * send_to_kdc() sends a message to the Kerberos authentication
 * server(s) in the given realm and returns the reply message.
 * The "pkt" argument points to the message to be sent to Kerberos;
 * the "rpkt" argument will be filled in with Kerberos' reply.
 * The "realm" argument indicates the realm of the Kerberos server(s)
 * to transact with.  If the realm is null, the local realm is used.
 *
 * If more than one Kerberos server is known for a given realm,
 * different servers will be queried until one of them replies.
 * Several attempts (retries) are made for each server before
 * giving up entirely.
 *
 * If an answer was received from a Kerberos host, KSUCCESS is
 * returned.  The following errors can be returned:
 *
 * SKDC_CANT    - can't get local realm
 *              - can't find "kerberos" in /etc/services database
 *              - can't open socket
 *              - can't bind socket
 *              - all ports in use
 *              - couldn't find any Kerberos host
 *
 * SKDC_RETRY   - couldn't get an answer from any Kerberos server,
 *                after several retries
 */
int
send_to_kdc(
    KTEXT pkt,
    KTEXT rpkt,
    char *realm
    )
{
    int i;
    SOCKET f = INVALID_SOCKET;
    int no_host; /* was a kerberos host found? */
    int retry;
    int n_hosts;
    int retval;
    struct sockaddr_in to;
    struct hostent *host, *hostlist, *fphost, *temp_hostlist;
    int *portlist, *temp_portlist;
    char *cp;
    char krbhst[MAX_HSTNM];
    char lrealm[REALM_SZ];

    hostlist = 0;
    portlist = 0;

    /*
     * If "realm" is non-null, use that, otherwise get the
     * local realm.
     */                      

    if (realm && realm[0]) {
	strncpy(lrealm, realm, REALM_SZ);
        lrealm[REALM_SZ] = 0;
    }
    else
	if (krb_get_lrealm(lrealm, 1)) {
	    if (krb_debug)
		kdebug("%s: can't get local realm\n", prog);
	    return(SKDC_CANT);
	}
    if (krb_debug)
	kdebug("lrealm is %s\n", lrealm);

    if (krb_udp_port_conf == 0) {
	register struct servent FAR *sp;
        if (sp = getservbyname("kerberos", "udp")) {
            krb_udp_port_conf = sp->s_port;
        } else if (KRB_PORT) {
            if (krb_debug)
                kdebug("%s: Can't get kerberos/udp service -- "
                       "using default port\n", prog);
            krb_udp_port_conf = htons(KRB_PORT);
        } else {
            if (krb_debug)
                kdebug("%s: Can't get kerberos/udp service at all\n", prog);
            return(SKDC_CANT);
        }
        if (krb_debug)
            kdebug("krb_udp_port_conf is %d\n", ntohs(krb_udp_port_conf));
    }

    /* from now on, exit through rtn label for cleanup */

    memset((char *)&to, 0, S_AD_SZ);
    hostlist = (struct hostent *) malloc(sizeof(struct hostent));
    if (!hostlist) {
        retval = SKDC_CANT;
        goto rtn;
    }
    memset(hostlist, 0, sizeof(struct hostent));

    f = socket(AF_INET, SOCK_DGRAM, 0);
    if (f == INVALID_SOCKET) {
        if (krb_debug)
            kdebug("%s: Can't open socket (error %d)\n", prog, 
                   WSAGetLastError());
        retval = SKDC_CANT;
        goto rtn;
    }

    /* make the socket non-blocking */
    {
        u_long onOff = TRUE;
        if (ioctlsocket(f, FIONBIO, (u_long FAR*)&onOff))
        {
            if (krb_debug)
                kdebug("%s: Can't make socket non-blocking (error %d)\n", 
                       prog, WSAGetLastError());
            retval = SKDC_CANT;
            goto rtn;
        }
    }

    /*
    ** FTP Software's WINSOCK implmentation insists that
    ** a socket be bound before it can receive datagrams.
    ** This is outside specs.  Since it shouldn't hurt any
    ** other implementations we'll go ahead and do it for
    ** now.
    */
    {
	struct sockaddr_in from;
	memset ((char *)&from, 0, S_AD_SZ);
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = INADDR_ANY;
	if ( bind(f, (struct sockaddr *)&from, S_AD_SZ) == SOCKET_ERROR ) {
	    if (krb_debug)
                kdebug("%s : Can't bind\n", prog);
	    retval = SKDC_CANT;
	    goto rtn;
	}
    }
    /* End of kludge for FTP Software WinSock stack.  */

    no_host = 1;
    /* get an initial allocation */
    n_hosts = 0;
    for (i = 1; krb_get_krbhst(krbhst, lrealm, i) == KSUCCESS; ++i) {
#ifdef USE_DNS
        int krb_udp_port = krb_udp_port_dns?krb_udp_port_dns:krb_udp_port_conf;
        if (krb_debug) {
            kdebug("krb_udp_port is %d\n", ntohs(krb_udp_port));
        }
#else /* !USE_DNS */
        int krb_udp_port = krb_udp_port_conf;
#endif /* !USE_DNS */
        if (krb_debug) {
            kdebug("Getting host entry for %s...", krbhst);
        }
        fphost = /* host = */ GETHOSTBYNAME(krbhst);
        if (krb_debug) {
            kdebug("%s.\n",
                   fphost ? "Got it" : "Didn't get it");
        }
        if (!fphost){
            continue;
        }    
        no_host = 0;    /* found at least one */
        n_hosts++;
        /* preserve host network address to check later
         * (would be better to preserve *all* addresses,
         * take care of that later)
         */
        temp_portlist = portlist;
        portlist = (int *) realloc(portlist, n_hosts*sizeof(int));
        if (!portlist) {
            portlist = temp_portlist;
            retval = SKDC_CANT;
            goto rtn;
        }
        portlist[n_hosts-1] = krb_udp_port;

        temp_hostlist = hostlist;
        hostlist = (struct hostent *)
            realloc((char *)hostlist,
                    (unsigned)
                    sizeof(struct hostent)*(n_hosts+1));
        if (!hostlist){
            hostlist = temp_hostlist;
            retval = SKDC_CANT;
            goto rtn;
        }
        hostlist[n_hosts-1] = *fphost;
        memset(&hostlist[n_hosts], 0, sizeof(struct hostent));
        host = &hostlist[n_hosts-1];
        cp = (char*)malloc((unsigned)host->h_length);
        if (!cp) {
            retval = SKDC_CANT;
            goto rtn;
        }
        memcpy(cp, host->h_addr, host->h_length);
        host->h_addr_list = (char **)malloc(sizeof(char *));
        if (!host->h_addr_list) {
            retval = SKDC_CANT;
            goto rtn;
        }
        host->h_addr = cp;
        to.sin_family = host->h_addrtype;
        memcpy((char *)&to.sin_addr, host->h_addr, 
	       host->h_length);
        to.sin_port = krb_udp_port;
        if (send_recv(pkt, rpkt, f, &to, hostlist)) {
            retval = KSUCCESS;
            goto rtn;
        }
        if (krb_debug) {
            kdebug("Timeout, error, or wrong descriptor\n");
        }
    }
    if (no_host) {
        if (krb_debug)
            kdebug("%s: can't find any Kerberos host.\n", prog);
        retval = SKDC_CANT;
        goto rtn;
    }
    /* retry each host in sequence */
    for (retry = 0; retry < CLIENT_KRB_RETRY; ++retry) {
        i = 0;
        for (host = hostlist; host->h_name != 0; host++) {
            to.sin_family = host->h_addrtype;
            to.sin_port = portlist[i++];
            memcpy((char *)&to.sin_addr, host->h_addr, 
		   host->h_length);
            if (send_recv(pkt, rpkt, f, &to, hostlist)) {
                retval = KSUCCESS;
                goto rtn;
            }
        }
    }
    retval = SKDC_RETRY;

 rtn:
    if (f != INVALID_SOCKET)
        if (closesocket(f) == SOCKET_ERROR)
            if (krb_debug)
                kdebug("%s: Could not close socket (error %d)\n",
                       prog, WSAGetLastError());
    if (hostlist) {
        register struct hostent *hp;
        for (hp = hostlist; hp->h_name; hp++)
            if (hp->h_addr_list) {
                if (hp->h_addr)
                    free(hp->h_addr);
                free(hp->h_addr_list);
            }
        free(hostlist);
    }
    if (portlist)
        free(portlist);
    return(retval);
}

/*
 * try to send out and receive message.
 * return 1 on success, 0 on failure
 */
static
send_recv(KTEXT pkt, KTEXT rpkt, SOCKET f,
          struct sockaddr_in *_to, struct hostent *addrs)
{
    fd_set readfds;
    register struct hostent *hp;
    struct sockaddr_in from;
    int sin_size;
    int numsent;
    int ret = SOCKET_ERROR;

    if (krb_debug) {
        if (_to->sin_family == AF_INET)
            kdebug("Sending message to %s...", inet_ntoa(_to->sin_addr));
        else
            kdebug("Sending message...");
    }

    if ((numsent = sendto(f,(char *)(pkt->dat), pkt->length, 0,
                          (struct sockaddr *)_to, S_AD_SZ)) != pkt->length) {
        if (krb_debug)
            kdebug("sent only %d/%d\n", numsent, pkt->length);
        return 0;
    }

    if (krb_debug) {
        kdebug("Sent\nWaiting for reply...");
    }
    FD_ZERO(&readfds);
    FD_SET(f, &readfds);
    WSASetLastError(0);
    /* select - either recv is ready, or timeout */
    /* see if timeout or error or wrong descriptor */
    ret = select(0, &readfds, (fd_set *)0, (fd_set *)0, &timeout);
    if (ret != 1 || !FD_ISSET(f, &readfds)) {
        /* I don't think the WinSock blocking stuff is needed... */
        if (WSAIsBlocking()) {
            WSACancelBlockingCall();
        }
        if (krb_debug) {
            kdebug("select failed: ret=%d, readfds=%x, errno=%d",
                   ret, readfds, WSAGetLastError());
        }
        return 0;
    }

    sin_size = sizeof(from);
    if ((numsent = (recvfrom(f, (char *)(rpkt->dat), sizeof(rpkt->dat), 0,
                             (struct sockaddr *)&from, &sin_size)))
        != rpkt->length) {
        if (numsent < 0) {
            if (krb_debug)
                kdebug("recvfrom : Error %d\n", WSAGetLastError());
            return 0;
        }
    }
    if (krb_debug) {
        kdebug("received packet from %s\n", inet_ntoa(from.sin_addr));
    }

    for (hp = addrs; hp->h_name != 0; hp++) {
        if (!memcmp(hp->h_addr, &from.sin_addr.s_addr,
                    hp->h_length))
        {
            if (krb_debug) {
                kdebug("Received it\n");
            }
            return 1;
        }
        if (krb_debug)
            kdebug("packet not from %x\n", hp->h_addr);
    }

    if (krb_debug)
        kdebug("%s: received packet from wrong host! (%x)\n",
               "send_to_kdc(send_rcv)", from.sin_addr.s_addr);
    return 0;
}
