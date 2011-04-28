/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <krb.h>
#include <string.h>

#include <ctype.h>

#ifdef USE_DNS
#ifdef WSHELPER
#include <wshelper.h>
#else  /* WSHELPER */
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#endif /* WSHELPER */
#ifndef T_SRV
#define T_SRV 33
#endif /* T_SRV */

/* for Ultrix and friends ... */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#define MAX_DNS_NAMELEN (15*(MAXHOSTNAMELEN + 1)+1)
#endif /* USE_DNS */

/*
 * Given a Kerberos realm, find a host on which the Kerberos authenti-
 * cation server can be found.
 *
 * krb_get_krbhst takes a pointer to be filled in, a pointer to the name
 * of the realm for which a server is desired, and an integer, n, and
 * returns (in h) the nth entry from the configuration file (KRB_CONF,
 * defined in "krb.h") associated with the specified realm.
 *
 * On end-of-file, krb_get_krbhst returns KFAILURE.  If n=1 and the
 * configuration file does not exist, krb_get_krbhst will return KRB_HOST
 * (also defined in "krb.h").  If all goes well, the routine returnes
 * KSUCCESS.
 *
 * The KRB_CONF file contains the name of the local realm in the first
 * line (not used by this routine), followed by lines indicating realm/host
 * entries.  The words "admin server" following the hostname indicate that
 * the host provides an administrative database server.
 *
 * For example:
 *
 *      ATHENA.MIT.EDU
 *      ATHENA.MIT.EDU kerberos-1.mit.edu admin server
 *      ATHENA.MIT.EDU kerberos-2.mit.edu
 *      LCS.MIT.EDU kerberos.lcs.mit.edu admin server
 *
 * This is a temporary hack to allow us to find the nearest system running
 * kerberos.  In the long run, this functionality will be provided by a
 * nameserver.
 */

static char *
parse_str(
    char *buffer,
    char *result
    )
{
    if (!buffer)
        goto cleanup;

    while (*buffer && isspace(*buffer))
        buffer++;
    while (*buffer && !isspace(*buffer))
        *result++=*buffer++;

    cleanup:
    *result='\0';
    return buffer;
}

static int
krb_get_krbhst_conf(
    char *h,
    char *r,
    int n,
    int *m
    )
{
    char tr[REALM_SZ];
    FILE *cnffile = 0;
    char linebuf[BUFSIZ];
    int i;
    char *p;
    char *conf_fn = 0;
    size_t conf_sz = 0;
    // We assume failure and explicitly set success:
    int rc = KFAILURE;

    *m = 0;

    krb_get_krbconf2(conf_fn, &conf_sz);
    conf_fn = malloc(conf_sz);
    if (!conf_fn || !krb_get_krbconf2(conf_fn, &conf_sz))
        goto cleanup;

    memset(linebuf, 0, sizeof(linebuf));
    if ((cnffile = fopen(conf_fn, "r")) == NULL) {
#ifdef USE_DNS
        krb_set_use_dns(2);
#endif /* USE_DNS */
        goto cleanup;
    }
#ifdef USE_DNS
    /* if we are using DNS because we couldn't find KRB.CON */
    if (krb_use_dns() == 2)
        krb_set_use_dns(-1);
#endif /* USE_DNS */
    if (fgets(linebuf, sizeof(linebuf), cnffile) == NULL) {
        goto cleanup;
    }
#ifdef USE_DNS
    /* If we are using DNS do not consider a blank default realm an error */
    /* as that indicates we should determine the default realm using DNS  */
    if (!krb_use_dns()) {
#endif /* USE_DNS */
        memset(tr, 0, sizeof(tr) );
        parse_str(linebuf, tr);
        if (*tr == '\0') {
            goto cleanup;
        }        
#ifdef USE_DNS
    }
#endif /* USE_DNS */
    /* run through the file, looking for the nth server for this realm */
    for (i = 0; i < n;) {
        if (fgets(linebuf, sizeof(linebuf), cnffile) == NULL) {
            *m = i;
            goto cleanup;
        }
        memset(tr, 0, sizeof(tr));
        p = parse_str(linebuf, tr);
        if (*tr == '\0')
            continue;
        memset(h, 0, lstrlen(h));
        parse_str(p,h);
        if (*tr == '\0')
            continue;
        if (!lstrcmp(tr,r))
            i++;
    }
    *m = n;
    rc = KSUCCESS;
 cleanup:
    if (cnffile)
        fclose(cnffile);
    if (conf_fn)
        free(conf_fn);
    return rc;
}


#ifdef USE_DNS
/*
 * Lookup a KDC via DNS SRV records
 */

int
krb_locate_srv_dns(
    const char *realm,
    const char *service,
    const char *protocol,
    char ** hosts,
    int   * ports,
    int     hostsz,
    int  *  nhosts
    )
{
    int i, count, out;
    unsigned char answer[2048], *p=NULL;
    char host[MAX_DNS_NAMELEN+1];
    struct sockaddr *addr = NULL;
    struct sockaddr_in *sin = NULL;
    struct hostent *hp = NULL;
    int type, class;
    int priority, weight, size, len, numanswers, numqueries, rdlen;
    unsigned short port;
    HEADER *hdr = NULL;
    const int hdrsize = sizeof(HEADER);
    struct srv_dns_entry {
	struct srv_dns_entry *next;
	int priority;
	int weight;
	unsigned short port;
	char *host;
    };

    struct srv_dns_entry *head = NULL;
    struct srv_dns_entry *srv=NULL, *entry=NULL;

    addr = (struct sockaddr *) malloc(sizeof(struct sockaddr));
    if (addr == NULL)
	return(KFAILURE);

    count = 1;
    out = 0;

    /*
     * First off, build a query of the form:
     *
     * service.protocol.realm
     *
     * which will most likely be something like:
     *
     * _kerberos._udp.REALM
     *
     */

    if ( strlen(service) + strlen(protocol) + strlen(realm) + 4 
         > MAX_DNS_NAMELEN )
        goto out;
    sprintf(host, "%s.%s.%s", service, protocol, realm);

    size = res_search(host, C_IN, T_SRV, answer, sizeof(answer));

    if (size < hdrsize)
	goto out;

    /*
     * We got an answer!  First off, parse the header and figure out how
     * many answers we got back.
     */

    p = answer;
    hdr = (HEADER *) answer;

    numqueries = ntohs(hdr->qdcount);
    numanswers = ntohs(hdr->ancount);

    p += sizeof(HEADER);

    /*
     * We need to skip over all of the questions, so we have to iterate
     * over every query record.  dn_expand() is able to tell us the size
     * of compress DNS names, so we use it.
     */

#define CHECK(x,y) if (((x) + (y)) > (size + answer)) goto out
#define INCR_CHECK(x,y) x += (y); if ((x) > size + answer) goto out
#define NTOHSP(x,y) x[0] << 8 | x[1]; x += y

    while (numqueries--) {
	len = dn_expand(answer, answer + size, p, host, sizeof(host));
	if (len < 0)
	    goto out;
	INCR_CHECK(p, len + 4);
    }

    /*
     * We're now pointing at the answer records.  Only process them if
     * they're actually T_SRV records (they might be CNAME records,
     * for instance).
     *
     * But in a DNS reply, if you get a CNAME you always get the associated
     * "real" RR for that CNAME.  RFC 1034, 3.6.2:
     *
     * CNAME RRs cause special action in DNS software.  When a name server
     * fails to find a desired RR in the resource set associated with the
     * domain name, it checks to see if the resource set consists of a CNAME
     * record with a matching class.  If so, the name server includes the CNAME
     * record in the response and restarts the query at the domain name
     * specified in the data field of the CNAME record.  The one exception to
     * this rule is that queries which match the CNAME type are not restarted.
     *
     * In other words, CNAMEs do not need to be expanded by the client.
     */

    while (numanswers--) {

	/* First is the name; use dn_expand to get the compressed size */
	len = dn_expand(answer, answer + size, p, host, sizeof(host));
	if (len < 0)
	    goto out;
	INCR_CHECK(p, len);

	/* Next is the query type */

        CHECK(p,2);
	type = NTOHSP(p,2);

	/* Next is the query class; also skip over 4 byte TTL */
        CHECK(p, 6);
	class = NTOHSP(p,6);

	/* Record data length */

        CHECK(p, 2);
	rdlen = NTOHSP(p,2);

	/*
	 * If this is an SRV record, process it.  Record format is:
	 *
	 * Priority
	 * Weight
	 * Port
	 * Server name
	 */

	if (class == C_IN && type == T_SRV) {
	    CHECK(p,2);
	    priority = NTOHSP(p,2);
            CHECK(p,2);
	    weight = NTOHSP(p,2);
	    CHECK(p, 2);
	    port = NTOHSP(p,2);
	    len = dn_expand(answer, answer + size, p, host, sizeof(host));
	    if (len < 0)
		goto out;
	    INCR_CHECK(p, len);

	    /*
	     * We got everything!  Insert it into our list, but make sure
	     * it's in the right order.  Right now we don't do anything
	     * with the weight field
	     */

	    srv = (struct srv_dns_entry *) malloc(sizeof(struct srv_dns_entry));
	    if (srv == NULL)
		goto out;
	
	    srv->priority = priority;
	    srv->weight = weight;
	    srv->port = port;
	    srv->host = strdup(host);

	    if (head == NULL || head->priority > srv->priority) {
		srv->next = head;
		head = srv;
	    } else
		/*
		 * This is confusing.  Only insert an entry into this
		 * spot if:
		 * The next person has a higher priority (lower priorities
		 * are preferred).
		 * Or
		 * There is no next entry (we're at the end)
		 */
		for (entry = head; entry != NULL; entry = entry->next)
		    if ((entry->next &&
			 entry->next->priority > srv->priority) ||
			entry->next == NULL) {
			srv->next = entry->next;
			entry->next = srv;
			break;
		    }
	} else
	    INCR_CHECK(p, rdlen);
    }
	
    /*
     * Okay!  Now we've got a linked list of entries sorted by
     * priority.  
     */

    if (head == NULL)
	goto out;

    /* Move host names to hosts array */
    for ( entry = head, i=0; 
          entry != NULL && i<hostsz; 
          entry = entry->next, i++ ) {
        hosts[i] = entry->host;
        ports[i] = ntohs(entry->port);
        entry->host = NULL;
    }
    out = i;

    for (entry = head; entry != NULL; ) {
        if ( entry->host ) {
            free(entry->host);
            entry->host = NULL;
        }
	srv = entry;
	entry = entry->next;
	free(srv);
        srv = NULL;
    }

  out: 
    if (addr)
        free(addr);
    if (srv)
        free(srv);

    *nhosts = out;
    if ( out == 0 )
        return(KFAILURE);
    return(KSUCCESS);
}


static
int 
krb_get_krbhst_dns(
    char *h,
    int  *p,
    char *r,
    int n
    )
{
    char * hosts[64];
    int    ports[64];
    int    i;
    int    nhosts = 0;
	int    rc;

    h[0] = '\0';

	rc = krb_locate_srv_dns(r, "_kerberos-iv", "_udp", hosts, ports, 64, &nhosts);
	if (rc == KFAILURE)
		rc = krb_locate_srv_dns(r, "_kerberos", "_udp", hosts, ports, 64, &nhosts);
    if (rc == KSUCCESS) 
    {
        if (n <= 0 || n > nhosts || (nhosts == 1 && !strcmp(hosts[0],"."))) {
            for (i = 0; i < nhosts; i++)
                free(hosts[i]);
            return(KFAILURE);
        }

        strcpy(h, hosts[n-1]);
        *p = ports[n-1];
        for (i = 0; i < nhosts; i++)
            free(hosts[i]);
        return(KSUCCESS);
    } else {
        return(KFAILURE);
    }
}

int krb_udp_port_dns; /* Has krb_udp_port been set by DNS SRV? */

#endif /* USE_DNS */

#ifdef USE_KRBHST_DEFAULT
/* 
 * KfW 2.5 / Kerberos V5 1.3
 * 
 * It has been decided that we will no longer provide a default of 
 * kerberos.REALM as a response if there is a negative response
 * from the krb.conf and DNS SRV lookups.  This is to prevent 
 * undesireable Kerberos IV attempts to Kerberos V only KDCs
 */

static int
get_krbhst_default(h, r, n)
    char *h;
    char *r;
    int n;
{
    if (n != 1)
        return KFAILURE;
    if (strlen(KRB_HOST) + 1 + strlen(r) >= MAXHOSTNAMELEN)
        return KFAILURE;
    /* KRB_HOST.REALM (ie. kerberos.CYGNUS.COM) */
    strcpy(h, KRB_HOST);
    strcat(h, ".");
    strcat(h, r);
    return KSUCCESS;
}
#endif /* USE_KRBHST_DEFAULT */

int
krb_get_krbhst(
    char *h,
    char *r,
    int n
    )
{
    int rc = KFAILURE;
    int m = 0;

    rc = krb_get_krbhst_conf(h, r, n, &m);
#ifdef USE_DNS
    krb_udp_port_dns = 0;
    /* only use DNS if there were no entries in the conf file */
    if (rc != KSUCCESS && m == 0 && krb_use_dns()) {
        rc = krb_get_krbhst_dns(h, &krb_udp_port_dns, r, n);
        if (rc != KSUCCESS)
            krb_udp_port_dns = 0;
    }
#endif /* USE_DNS */
    if (rc != KSUCCESS) {
#ifdef USE_KRBHST_DEFAULT
        rc = get_krbhst_default(h, r, n);
#endif /* USE_KRBHST_DEFAULT */
        return(rc);
    } else {
        return(KSUCCESS);
    }
}
