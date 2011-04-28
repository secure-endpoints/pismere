/*
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <stdio.h>
#include <krb.h>
#include <string.h>

#ifdef USE_DNS
#ifdef WSHELPER
#include <wshelper.h>
#else /* WSHELPER */
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#endif /* WSHELPER */
#ifndef T_SRV
#define T_SRV 33
#endif /* T_SRV */

extern int krb_locate_srv_dns(const char *, const char *, const char *,
                              char **, int *, int, int *);
#endif /* USE_DNS */

/*
 * Given a Kerberos realm, find a host on which the Kerberos database
 * administration server can be found.
 *
 * krb_get_admhst takes a pointer to be filled in, a pointer to the name
 * of the realm for which a server is desired, and an integer n, and
 * returns (in h) the nth administrative host entry from the configuration
 * file (KRB_CONF, defined in "krb.h") associated with the specified realm.
 *
 * On error, get_admhst returns KFAILURE. If all goes well, the routine
 * returns KSUCCESS.
 *
 * For the format of the KRB_CONF file, see comments describing the routine
 * krb_get_krbhst().
 *
 * This is a temporary hack to allow us to find the nearest system running
 * a Kerberos admin server.  In the long run, this functionality will be
 * provided by a nameserver.
 */

int
krb_get_admhst_conf(
    char *h,
    char *r,
    int n,
    int *m
    )
{
    FILE *cnffile = 0;
    char tr[REALM_SZ];
    char linebuf[BUFSIZ];
    register int i;
    char *tmp;
    char *conf_fn = 0;
    size_t conf_sz = 0;
    // We assume failure and explicitly set success:
    int rc = KFAILURE;

    *m = 0;

    krb_get_krbconf2(conf_fn, &conf_sz);
    conf_fn = malloc(conf_sz);
    if (!conf_fn || !krb_get_krbconf2(conf_fn, &conf_sz))
        goto cleanup;

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

    if (fgets(linebuf, BUFSIZ, cnffile) == NULL) {
        /* error reading */
        goto cleanup;
    }
    if (!index(linebuf, '\n')) {
        /* didn't all fit into buffer, punt */ 
        goto cleanup;
    }
    for (i = 0; i < n; ) {
        /* run through the file, looking for admin host */
        if (fgets(linebuf, BUFSIZ, cnffile) == NULL) { 
            *m = i;
            goto cleanup;
        }
        // XXX - We scan for something of the format "realm host something"
        //       instead of "realm host admin server".  Interestingly,
        //       the Unix code looks for "realm host admin something".
        tmp = NULL;
        tmp = strtok( linebuf, (LPSTR) " \t");
        if( tmp == NULL ){
            continue;
        }
        strcpy( (LPSTR) tr, (LPSTR) tmp );
        tmp = NULL;
        tmp = strtok( NULL, (LPSTR) " \t");
        if( tmp == NULL ){
            continue;
        }
        strcpy( (LPSTR) h, (LPSTR) tmp );
        tmp = NULL;
        tmp = strtok( NULL, (LPSTR) " \t");
        if( tmp == NULL ){
            continue;
        }
        if (!strcmp(tr,r))
            i++;
    }
    *m = n;
    rc = KSUCCESS;
 cleanup:
    if (cnffile)
        fclose(cnffile);
    if (conf_fn)
        free(conf_fn);
    return(rc);
}

#ifdef USE_DNS
static
int 
krb_get_admhst_dns(
    char *h,
    char *r,
    int n
    )
{
    char * hosts[64];
    int    ports[64];
    int    i;
    int    nhosts = 0;

    h[0] = '\0';

    if (krb_locate_srv_dns(r, "_kadmin", "_tcp", hosts, ports, 64, &nhosts) 
         == KSUCCESS ) 
    {
        if (n <= 0 || n > nhosts) {
            for ( i=0;i<nhosts;i++ )
                free(hosts[i]);
            return(KFAILURE);
        }

        strcpy(h, hosts[n-1]);
        for (i = 0; i < nhosts; i++)
            free(hosts[i]);
        return(KSUCCESS);
    } else {
        return(KFAILURE);
    }
}
#endif /* USE_DNS */

int
krb_get_admhst(h,r,n)
    char *h;
    char *r;
    int n;
{
    int rc = KFAILURE;
    int m = 0;

    rc = krb_get_admhst_conf(h, r, n, &m);
#ifdef USE_DNS
    /* only use DNS if conf file contains no entries */
    if (rc != KSUCCESS && m == 0 && krb_use_dns()) {
        rc = krb_get_admhst_dns(h, r, n);
    }
#endif /* USE_DNS */
    if (rc != KSUCCESS) {
        return(KFAILURE);
    } else {
        return(KSUCCESS);
    }
}
