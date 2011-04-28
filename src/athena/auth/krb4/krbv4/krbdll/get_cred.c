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

#include <leashwin.h>
static HINSTANCE  m_hLeashDLL = 0;
#define  LEASHDLL "leashw32.dll"

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
    int kinited = 0;
    char env[16];
    BOOL prompt;

    GetEnvironmentVariable("KERBEROSLOGIN_NEVER_PROMPT",env, sizeof(env));
    prompt = (GetLastError() == ERROR_ENVVAR_NOT_FOUND);

	c->pname[0] = c->pinst[0] = '\0';

check_cache:

    gettimeofday(&local_time, 0);
        
    /* Open ticket file and lock it for shared reading */
    if ((tf_status = tf_init(TKT_FILE, R_TKT_FIL)) != KSUCCESS) {
		goto cache_checked;
    }
    /* Copy principal's name and instance into the CREDENTIALS struc c */
    if (((tf_status = tf_get_pname(c->pname)) != KSUCCESS) ||
        ((tf_status = tf_get_pinst(c->pinst)) != KSUCCESS))
    {
    	goto cache_checked;
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

cache_checked:
    tf_close();

    // If we are requesting a tgt, prompt for it
	if (tf_status != KSUCCESS && !kinited && 
        strncmp(service, "krbtgt", ANAME_SZ) == 0 && 
        prompt) 
    {
        static int (*pLeash_kinit_dlg_ex)(HWND hParent, LPLSH_DLGINFO_EX lpdlginfoex) = 0;
        static DWORD (*pLeash_get_default_use_krb4)() = 0;

        kinited = 1;

        if ( !m_hLeashDLL ) {
            m_hLeashDLL = LoadLibrary(LEASHDLL);
            if ( m_hLeashDLL ) {
                (FARPROC)pLeash_kinit_dlg_ex=GetProcAddress(m_hLeashDLL,"Leash_kinit_dlg_ex");
                (FARPROC)pLeash_get_default_use_krb4=GetProcAddress(m_hLeashDLL,"Leash_get_default_use_krb4");
            }
        }

        if ( pLeash_get_default_use_krb4 &&
             pLeash_get_default_use_krb4() &&
             pLeash_kinit_dlg_ex ) {
            LSH_DLGINFO_EX dlginfo;
            int success;

            // copy in the existing username and realm
            char * pTmp = calloc(1, strlen(c->pname) + strlen(c->pinst) + 2);
            strcpy(pTmp, c->pname);
            if (c->pname[0] != 0 && c->pinst[0] != 0)
            {
                strcat(pTmp, "/");
                strcat(pTmp, c->pinst);
            }

            memset(&dlginfo, 0, sizeof(LSH_DLGINFO_EX));
            dlginfo.size = sizeof(LSH_DLGINFO_EX);
            dlginfo.dlgtype = DLGTYPE_PASSWD;
            dlginfo.title = "Kerberos 4 - Obtain Tickets";
            dlginfo.username = pTmp;
            dlginfo.realm = realm;
            dlginfo.use_defaults = 1;

            success = pLeash_kinit_dlg_ex(GetDesktopWindow(), &dlginfo);
            free(pTmp);
            if (success)
                goto check_cache;
        }
    }

    if (tf_status == EOF)
        return (GC_NOTKT);

    return(tf_status);
}
