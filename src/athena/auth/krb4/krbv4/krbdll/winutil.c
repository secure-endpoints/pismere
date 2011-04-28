
#include "conf.h"
#include <windows.h>
#include <winsock.h>

#include <lsh_pwd.h>
//#include <leashids.h>
#include <com_err.h>
#include <leasherr.h>
#include <krberr.h>
#include <krb.h>
#include <winkrbid.h>
/* #include <krblayer.h> */

/* Global Variables. */
static long lsh_errno;
static char *err_context;       /* error context */
static char *kadm_info; /* to get info from the kadm* files */

/* Callback functions */

#ifdef NOLEASHCOMPAT
LONG PASCAL
_export
lsh_get_lsh_errno(
    LONG FAR *err_val
    )
{
    *err_val = lsh_errno;
    return(lsh_errno);
}
#endif
