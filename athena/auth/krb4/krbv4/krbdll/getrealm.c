/*
 * Copyright 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * routine to convert hostname into realm name.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <krb.h>
#include <stdlib.h>

#ifdef USE_DNS
#ifdef WSHELPER 
#include <wshelper.h>
#else /* WSHELPER */
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#endif /* WSHELPER */
#endif /* USE_DNS */

/* for Ultrix and friends ... */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#define MAX_DNS_NAMELEN (15*(MAXHOSTNAMELEN + 1)+1)

int
two_fields_q(
    FILE* file_desc,
    char* buffer1,
    int   buffer1_size,
    char* buffer2,
    int   buffer2_size
    )
{
    char buf[MAX_DNS_NAMELEN + 80 + REALM_SZ + 1];
    char *tmpbuf;
    int count = 0;

    memset(buf, '\0', sizeof(buf));
    fgets(buf, sizeof(buf) - 1, file_desc);
    if(buf[0] == 0){
	return(EOF);
    }
    tmpbuf = strtok(buf, " \t\n");
    if(buffer1 == NULL || tmpbuf == NULL){
	return(count);
    }
    strncpy(buffer1, tmpbuf, buffer1_size);
    count++;
    tmpbuf = strtok(NULL, " \t\n");
    if(tmpbuf){
	count++;
	strncpy(buffer2, tmpbuf, buffer2_size);
    }
    buffer1[buffer1_size - 1] = 0;
    buffer2[buffer2_size - 1] = 0;
    return(count);
}

#ifdef USE_DNS

/*
 * krb_try_realm_txt_rr
 *
 * Try to look up a TXT record pointing to a Kerberos realm
 *
 * NOTE: This is internal to the krb4 implementation.
 *
 * XXX: Be careful not to leak memory.
 *       
 */

int
krb_try_realm_txt_rr(
    const char *prefix,
    const char *name,
    char **realm
    )
{
    unsigned char answer[2048], *p;
    char host[MAX_DNS_NAMELEN];
    int size;
    HEADER *hdr;
    int type, class, numanswers, numqueries, rdlen, len;

    /*
     * Form our query, and send it via DNS
     */

    if (name == NULL || name[0] == '\0') {
        strcpy(host,prefix);
    } else {
        if ( strlen(prefix) + strlen(name) + 2 > MAX_DNS_NAMELEN )
            return(KFAILURE);
        sprintf(host,"%s.%s", prefix, name);
    }

    size = res_search(host, C_IN, T_TXT, answer, sizeof(answer));

    if (size < 0)
	return(KFAILURE);

    p = answer;
    hdr = (HEADER *) answer;

    numqueries = ntohs(hdr->qdcount);
    numanswers = ntohs(hdr->ancount);

    p += sizeof(HEADER);

    /*
     * We need to skip over the questions before we can get to the answers,
     * which means we have to iterate over every query record.  We use
     * dn_expand to tell us how long each compressed name is.
     */

#define INCR_CHECK(x, y) x += y; if (x > size + answer) \
					return(KFAILURE)
#define CHECK(x, y)  if ((x + y) > (size + answer)) return(KFAILURE)
#define NTOHSP(x,y) x[0] << 8 | x[1]; x += y

    while (numqueries--) {
	len = dn_expand(answer, answer + size, p, host, sizeof(host));
	if (len < 0)
	    return(KFAILURE);
	INCR_CHECK(p, len + 4);		/* Name plus type plus class */
    }

    /*
     * We're now pointing at the answer records.  Process the first
     * TXT record we find.
     */

    while (numanswers--) {
	
	/* First the name; use dn_expand to get the compressed size */
	len = dn_expand(answer, answer + size, p, host, sizeof(host));
	if (len < 0)
	    return(KFAILURE);
	INCR_CHECK(p, len);

	/* Next is the query type */
        CHECK(p,2);
	type = NTOHSP(p,2);

	/* Next is the query class; also skip over 4 byte TTL */
        CHECK(p,6);
	class = NTOHSP(p,6);

	/* Record data length - make sure we aren't truncated */

        CHECK(p,2);
	rdlen = NTOHSP(p,2);

	if (p + rdlen > answer + size)
	    return(KFAILURE);

	/*
	 * If this is a TXT record, return the string.  Note that the
	 * string has a 1-byte length in the front
	 */

	if (class == C_IN && type == T_TXT) {
	    len = *p++;
	    if (p + len > answer + size)
		return(KFAILURE);
	    *realm = malloc(len + 1);
	    if (*realm == NULL)
		return(KFAILURE);
	    strncpy(*realm, (char *) p, len);
	    (*realm)[len] = '\0';
            /* Avoid a common error. */
            if ( (*realm)[len-1] == '.' )
                (*realm)[len-1] = '\0';
	    return(KSUCCESS);
	}
    }

    return(KFAILURE);
}
#endif /* USE_DNS */


/*
 * krb_realmofhost.
 * Given a fully-qualified domain-style primary host name,
 * return the name of the Kerberos realm for the host.
 * If the hostname contains no discernable domain, or an error occurs,
 * return the local realm name, as supplied by get_krbrlm().
 * If the hostname contains a domain, but no translation is found,
 * the hostname's domain is converted to upper-case and returned.
 *
 * The format of each line of the translation file is:
 * domain_name kerberos_realm
 * -or-
 * host_name kerberos_realm
 *
 * domain_name should be of the form .XXX.YYY (e.g. .LCS.MIT.EDU)
 * host names should be in the usual form (e.g. FOO.BAR.BAZ)
 */

static char ret_realm[REALM_SZ+1];              // XXX - Thread-safety?

#ifdef USE_DNS
char *
krb_realmofhost_dns(host)
char FAR *host;
{
    char *dns_realm=NULL;
    char *domain = host;
    
    ret_realm[0] = '\0';

    do {
        if ( krb_try_realm_txt_rr("_kerberos", domain, &dns_realm) == KSUCCESS)
            break;
        domain = (char *)index(domain, '.');
        if ( domain )
            domain += 1;
    } while (domain);

    if (!dns_realm)
        krb_try_realm_txt_rr("_kerberos", "", &dns_realm);

    if (dns_realm) {
        strncpy(ret_realm,dns_realm,REALM_SZ);
        ret_realm[REALM_SZ] = '\0';
        free(dns_realm);
        return(ret_realm);
    }
    return(NULL);
}
#endif /* USE_DNS */

char *
PASCAL
krb_realmofhost(
    char *host
    )
{
    char *domain;
    char trans_host[MAX_DNS_NAMELEN + 1];
    char trans_realm[REALM_SZ+1];
    int retval;
    FILE *trans_file = 0;
    char *trans_fn = 0;
    size_t trans_sz = 0;

    ret_realm[0] = 0;
    domain = (char *)index(host, '.');

    krb_get_krbrealm2(trans_fn, &trans_sz);
    trans_fn = malloc(trans_sz);
    if (!trans_fn || !krb_get_krbrealm2(trans_fn, &trans_sz))
        goto cleanup;

    if ((trans_file = fopen(trans_fn, "r")) == 0) {
        /* krb_errno = KRB_NO_TRANS */
        goto cleanup;
    }
    while (1) {
        if ((retval = two_fields_q(trans_file,
                                   trans_host, sizeof(trans_host),
                                   trans_realm, sizeof(trans_realm))) != 2) {
            if (retval == EOF) {
                goto cleanup;
            }
            continue;       /* ignore broken lines */
        }
        if (!stricmp(trans_host, host)) {
            /* exact match of hostname, so return the realm */
            strncpy(ret_realm, trans_realm, REALM_SZ);
            goto cleanup;
        }
        if ((trans_host[0] == '.') && domain) {
            /* this is a domain match */
            if (!stricmp(trans_host, domain)) {
                /* domain match, save for later */
                strncpy(ret_realm, trans_realm, REALM_SZ);
                continue;
            }
        }
    }

 cleanup:
    if (trans_file)
        fclose(trans_file);
    if (trans_fn)
        free(trans_fn);

    /* Did we find anything? */
#ifdef USE_DNS
    /* krb_realmofhost_dns() stores its result if any in ret_realm[] */
    if (!ret_realm[0] && krb_use_dns())
        krb_realmofhost_dns(host);
#endif /* USE_DNS */
    if (!ret_realm[0])
    {
        if (domain) {
            char *cp;
            strncpy(ret_realm, domain + 1, REALM_SZ);
            ret_realm[REALM_SZ] = '\0';
            /* Upper-case realm */
            for (cp = ret_realm; *cp; cp++)
                if (islower(*cp))
                    *cp = toupper(*cp);
        } else {
            if (krb_get_lrealm(ret_realm, 1) != KSUCCESS)
                ret_realm[0] = 0;
        }
    }
    ret_realm[REALM_SZ] = 0;
    return ret_realm;
}
