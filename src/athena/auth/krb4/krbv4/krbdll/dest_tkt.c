/*
* Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
* of Technology.
*
* For copying and distribution information, please see the file
* <mit-copyright.h>.
*/

#include <conf.h>
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <krb.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <errno.h>
#include <fcntl.h>	  /* MS-DOS */

extern int krb_debug;

#ifdef MM_CACHE
#include <cacheapi.h>
extern apiCB* g_cc_ctx;
#endif

/*
* dest_tkt() is used to destroy the ticket store upon logout.
* If the ticket file does not exist, dest_tkt() returns RET_TKFIL.
* Otherwise the function returns RET_OK on success, KFAILURE on
* failure.
*
* The ticket file (TKT_FILE) is defined in "krb.h".
*/

int FAR dest_tkt(void)
{
    ccache_p *ccache_ptr;
    char     *tf_name = TKT_FILE;
    int ret = RET_OK;

    if (!g_cc_ctx)
        return KFAILURE;

    if (cc_open(g_cc_ctx, tf_name, CC_CRED_V4, 0, &ccache_ptr) == CC_NOERROR)
    {
	if (cc_destroy(g_cc_ctx, &ccache_ptr) != CC_NOERROR)
            return KFAILURE;
        else
            return RET_OK;
    } else {
        return RET_TKFIL;
    }
}
