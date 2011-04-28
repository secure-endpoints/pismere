/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <windows.h>
#include <stdio.h>
#include <krb.h>
#include <string.h>
#include <ctype.h>

#ifdef USE_DNS
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#define MAX_DNS_NAMELEN (15*(MAXHOSTNAMELEN + 1)+1)
#endif /* USE_DNS */

/*
 * krb_get_lrealm takes a pointer to a string, and a number, n.  It fills
 * in the string, r, with the name of the nth realm specified on the
 * first line of the kerberos config file (KRB_CONF, defined in "krb.h").
 * It returns 0 (KSUCCESS) on success, and KFAILURE on failure.  If the
 * config file does not exist, and if n=1, a successful return will occur
 * with r = KRB_REALM (also defined in "krb.h").
 *
 * NOTE: for archaic & compatibility reasons, this routine will only return
 * valid results when n = 1.
 *
 * For the format of the KRB_CONF file, see comments describing the routine
 * krb_get_krbhst().
 */

/*
 * XXX - Note: we are not doing any buffer size checking.  According
 * to the krb4 docs, r should be at least REALM_SZ long.
 */

int
krb_get_lrealm(
    char *r,
    int n
    )
{
    FILE *cnffile;
    char buffer[_MAX_PATH];
    char *p;
    char *q;
    char *conf_fn = 0;
    size_t conf_sz = 0;
    // We assume failure and explicitly set success:
    int rc = KFAILURE;

    // XXX - Should the check below be n != 1?
    if (n > 1)
	return(KFAILURE);  /* Temporary restriction */

    krb_get_krbconf2(conf_fn, &conf_sz);
    conf_fn = malloc(conf_sz);
    if (!conf_fn || !krb_get_krbconf2(conf_fn, &conf_sz))
        goto dns;

    if ((cnffile = fopen(conf_fn, "r")) == NULL) {
#ifdef USE_DNS
        krb_set_use_dns(2);
#endif /* USE_DNS */
        goto dns;
    }
#ifdef USE_DNS
    /* if we are using DNS because we couldn't find KRB.CON */
    if (krb_use_dns() == 2)
        krb_set_use_dns(-1);
#endif /* USE_DNS */

    fgets(buffer, sizeof(buffer), cnffile);
    for (p = buffer; isspace(*p); p++);
    for (q = r; isgraph(*p); *q++ = *p++);
    *q = '\0';

    fclose(cnffile);

    if (q != r) {
        rc = KSUCCESS;
        goto cleanup;
    }

 dns:
#ifdef USE_DNS
    if ( krb_use_dns() ) {
        /*
         * Since this didn't appear in our config file, try looking
         * it up via DNS.  Look for a TXT records of the form:
         *
         * _kerberos.<localhost>
         * _kerberos.<domainname>
         * _kerberos.<searchlist>
         *
         */
        char *dns_realm = 0;
        char localhost[MAX_DNS_NAMELEN+1];
        char * p;
        int retval;

        localhost[0] = '\0';
        gethostname(localhost,MAX_DNS_NAMELEN);

        if ( localhost[0] ) {
            p = localhost;
            do {
                retval = krb_try_realm_txt_rr("_kerberos", p, &dns_realm);
                p = strchr(p,'.');
                if (p)
                    p++;
            } while (retval && p && p[0]);

            if (retval)
                retval = krb_try_realm_txt_rr("_kerberos", "", &dns_realm);
        } else {
            krb_try_realm_txt_rr("_kerberos", "", &dns_realm);
        }

        if (dns_realm) {
            lstrcpy(r, dns_realm);
            free(dns_realm);
            rc = KSUCCESS;
        }
    }
#endif /* USE_DNS */

 cleanup:
    if (conf_fn)
        free(conf_fn);
    if (rc != KSUCCESS)
    {
        if (n == 1)
        {
            lstrcpy(r, KRB_REALM);
            return KSUCCESS;
        } else {
            return KFAILURE;
        }
    } else {
        return KSUCCESS;
    }
}
