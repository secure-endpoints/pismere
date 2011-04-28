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
#include <timeval.h>

/*
 * krb_get_cred takes a service name, instance, and realm, and a
 * structure of type CREDENTIALS to be filled in with ticket
 * information.  It then searches the ticket file for the appropriate
 * ticket and fills in the structure with the corresponding
 * information from the file.  If successful, it returns KSUCCESS.
 * On failure it returns a Kerberos error code.
 */

int
krb_get_cred(
    char *service,              /* Service name */
    char *instance,             /* Instance */
    char *realm,                /* Auth domain */
    CREDENTIALS *c              /* Credentials struct */
    )
{
    int tf_status;              /* return value of tf function calls */
    struct timeval local_time;

    gettimeofday(&local_time, 0);
        
    /* Open ticket file and lock it for shared reading */
    if ((tf_status = tf_init(TKT_FILE, R_TKT_FIL)) != KSUCCESS) {
    	tf_close();
    	return(tf_status);
    }
    /* Copy principal's name and instance into the CREDENTIALS struc c */
    if (((tf_status = tf_get_pname(c->pname)) != KSUCCESS) ||
        ((tf_status = tf_get_pinst(c->pinst)) != KSUCCESS))
    {
    	tf_close();
    	return (tf_status);
    }
    /* Search for requested service credentials and copy into c */ 
    while ((tf_status = tf_get_cred(c)) == KSUCCESS) {
        /* Is this the right ticket? */
        if (!service || (strcmp(c->service,service) == 0) &&
            (!instance || strcmp(c->instance,instance) == 0) &&
            (!realm || strcmp(c->realm,realm) == 0))
        {  
            // Hey, is this a valid ticket? Let's check before we return.
            if (((long)((c->issue_date + c->lifetime * 5L * 60L) -
                        (long)local_time.tv_sec) >= 0)
                &&
                ( 0 == com_addr(c)))
            {
                break; // we're OK
            }
        }
    }

    tf_close();
    if (tf_status == EOF)
   	return (GC_NOTKT);

    return(tf_status);
}
