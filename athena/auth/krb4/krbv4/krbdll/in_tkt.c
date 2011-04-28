/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifdef WINDOWS
#include <windows.h>
#endif
#include <conf.h>
#include <mit_copy.h>
#include <stdio.h>
#include <krb.h>
#ifdef PC
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int krb_debug;

#ifdef MM_CACHE
#include <cacheapi.h>
extern apiCB* g_cc_ctx;
#endif

/*
 * in_tkt() is used to initialize the ticket store.  It creates the
 * file to contain the tickets and writes the given user's name "pname"
 * and instance "pinst" in the file.  in_tkt() returns KSUCCESS on
 * success, or KFAILURE if something goes wrong.
 */

in_tkt(pname, pinst,prealm)
    char *pname;
    char *pinst;
    char *prealm;
{
#ifdef MM_CACHE
    char temp[2048];
    char lrealm[REALM_SZ]=""; 
    ccache_p* ccache_ptr;
    char* tf_name = TKT_FILE;
    int ret = 0;

    if ( prealm == NULL || prealm[0] == '\0' )
        krb_get_lrealm(lrealm, 1);

    memset(temp, '\0', sizeof(temp));
    if (strlen(pname) != 0)
    {
        strcpy(temp, pname);
        if (strlen(pinst) != 0)
        {
            strcat(temp, "/");
            strcat(temp, pinst);
        }
        if (prealm && prealm[0]) {
            strcat(temp, "@");
            strcat(temp, prealm);
        } 
        else if (lrealm[0])
        {
            strcat(temp, "@");
            strcat(temp, lrealm);
        }
    }

    if (cc_open(g_cc_ctx, tf_name, CC_CRED_V4, 0, &ccache_ptr) == CC_NOERROR)
    {
        // It exists, so let's blow it away.
        if ((ret = cc_destroy(g_cc_ctx, &ccache_ptr)) != CC_NOERROR) {
            if (krb_debug)
                kdebug_err("Error destroying while initializing %s (%d)", 
                           tf_name, ret);
            return KFAILURE;
        }
    }
    if ((ret = cc_create(g_cc_ctx, tf_name, temp, CC_CRED_V4, 0, &ccache_ptr))
        != CC_NOERROR)
    {
	if (krb_debug)
	    kdebug_err("Error creating while initializing %s (%d)", 
                       tf_name, ret);
        return KFAILURE;
    } else {
        cc_close(g_cc_ctx, &ccache_ptr);
        return KSUCCESS;
    }
#else /* !MM_CACHE */
    int tktfile;
    struct stat buf;
    int count;
    char *file = TKT_FILE;
    int fd;
    register int i;
    char charbuf[BUFSIZ];

    if (stat(file,&buf) == 0) {
	/* file already exists, and permissions appear ok, so nuke it */
	if ((fd = open(file, O_RDWR, 0)) < 0)
	    goto out; /* can't zero it, but we can still try truncating it */
	memset(charbuf,0, sizeof(charbuf));

	for (i = 0; i < buf.st_size; i += sizeof(charbuf))
	    if (write(fd, charbuf, sizeof(charbuf)) != sizeof(charbuf)) {
		(void) close(fd);
		goto out;
	    }

	(void) close(fd);
    }
 out:

    if ((tktfile = creat(file,S_IREAD|S_IWRITE)) < 0) {
	if (krb_debug)
	    kdebug_err("Error initializing %s", TKT_FILE);
	return(KFAILURE);
    }

    if (stat(file,&buf) < 0) {
	if (krb_debug)
	    kdebug_err("Error initializing %s", TKT_FILE);
	return(KFAILURE);
    }

    count = strlen(pname)+1;
    if (write(tktfile, pname, count) != count) {
	(void) close(tktfile);
	return(KFAILURE);
    }

    count = strlen(pinst)+1;
    if (write(tktfile,pinst,count) != count) {
	(void) close(tktfile);
	return(KFAILURE);
    }
    (void) close(tktfile);
    return(KSUCCESS);
#endif /* MM_CACHE */
}
