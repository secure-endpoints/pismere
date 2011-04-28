/*
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
*/

#include "conf.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <winbase.h>
#include <fcntl.h>
#include <string.h>
#include <io.h>
#include <krb.h>
#include <kadm.h>
#include <winsock.h>

extern int krb_debug;

#define MAXHOSTNAME  64

#ifdef MM_CACHE
#include <cacheapi.h>
#include <krbintrn.h>

/* need one global cache control block */
apiCB* g_cc_ctx = NULL;
extern DWORD TlsIndex;

extern int remember_the_function;   /* record of which STK algo used */

static
int
cc2mit(
    cred_union* ccC,
    CREDENTIALS* mitC
    );

/*
 * GetThreadData
 * 
 *   Gets the TLS pointer for this thread.  If the pointer is 0, we
 *   allocate the necessary storage, initialize it, and stuff the
 *   pointer back into the TLS slot for this thread.  If an error
 *   happens, return 0.  Otherwise, we return the pointer to the
 *   storage.
 */
THREAD_DATA *
GetThreadData(
    )
{
    THREAD_DATA *ThreadData;

    ThreadData = TlsGetValue(TlsIndex);
    if (!ThreadData && !GetLastError())
    {
        ThreadData = (THREAD_DATA*)LocalAlloc(LPTR, sizeof(THREAD_DATA));
        if (!ThreadData)
        {
            return 0;
        }
        ThreadData->itCreds = 0;
        ThreadData->g_cache_ptr = 0;
        if (!TlsSetValue(TlsIndex, ThreadData))
        {
            LocalFree(ThreadData);
            return 0;
        }
    }
    return ThreadData;
}

#endif // MM_CACHE

/*
 * This file contains routines for manipulating the ticket cache file.
 *
 * The ticket file is in the following format:
 *
 *      principal's name        (null-terminated string)
 *      principal's instance    (null-terminated string)
 *      CREDENTIAL_1
 *      CREDENTIAL_2
 *      ...
 *      CREDENTIAL_n
 *      EOF
 *
 *      Where "CREDENTIAL_x" consists of the following fixed-length
 *      fields from the CREDENTIALS structure (see "krb.h"):
 *
 *              char            service[ANAME_SZ]
 *              char            instance[INST_SZ]
 *              char            realm[REALM_SZ]
 *              C_Block         session
 *              int             lifetime
 *              int             kvno
 *              KTEXT_ST        ticket_st
 *              long            issue_date
 *
 * Short description of routines:
 *
 * tf_init() opens the ticket file and locks it.
 *
 * tf_get_pname() returns the principal's name.
 *
 * tf_get_pinst() returns the principal's instance (may be null).
 *
 * tf_get_cred() returns the next CREDENTIALS record.
 *
 * tf_save_cred() appends a new CREDENTIAL record to the ticket file.
 *
 * tf_close() closes the ticket file and releases the lock.
 *
 * tf_gets() returns the next null-terminated string.  It's an internal
 * routine used by tf_get_pname(), tf_get_pinst(), and tf_get_cred().
 *
 * tf_read() reads a given number of bytes.  It's an internal routine
 * used by tf_get_cred().
*/

/*
* tf_init() should be called before the other ticket file routines.
* It takes the name of the ticket file to use, "tf_name", and a
* read/write flag "rw" as arguments.
*
* It tries to open the ticket file, checks the mode, and if everything
* is okay, locks the file.  If it's opened for reading, the lock is
* shared.  If it's opened for writing, the lock is exclusive.
*
* Returns KSUCCESS if all went well, otherwise one of the following:
*
* NO_TKT_FIL   - file wasn't there
* TKT_FIL_ACC  - file was in wrong mode, etc.
* TKT_FIL_LCK  - couldn't lock the file, even after a retry
*/
int FAR tf_init(tf_name, rw)
    char   *tf_name;
{
    int ret;
    THREAD_DATA *ThreadData;

    if (!g_cc_ctx)
        return NO_TKT_FIL;

    ThreadData = GetThreadData();
    if (!ThreadData)
        return KFAILURE;

    tf_close();
    if (cc_open(g_cc_ctx, tf_name, CC_CRED_V4, 0, &ThreadData->g_cache_ptr) != CC_NOERROR)
    {
        ret = NO_TKT_FIL;
    } else {
        ret = KSUCCESS;
    }
    return ret;
}

/* getting local host IP address */
long
LocalHostAddr()
{ 
    char szLclHost[MAXHOSTNAME];
    LPHOSTENT lpstHostent;
    SOCKADDR_IN stLclAddr;
    SOCKADDR_IN stRmtAddr;
    int nAddrSize = sizeof(SOCKADDR);
    SOCKET hSock;
    int nRet;  

    stLclAddr.sin_addr.s_addr = INADDR_ANY;
		
    nRet = gethostname(szLclHost, MAXHOSTNAME);
    if (nRet != SOCKET_ERROR)
    {
	lpstHostent = gethostbyname((LPSTR) szLclHost);
	if (lpstHostent)
	    stLclAddr.sin_addr.s_addr = *((u_long FAR*)(lpstHostent->h_addr));
    }
    else
    {
	nRet = WSAGetLastError();
	if (nRet == WSANOTINITIALISED)
	    return(-1);
	else
	    return(0);
    }
		
    /* if still not resolved, then try second strategy */
    if (stLclAddr.sin_addr.s_addr == INADDR_ANY)
    {
	/* get a UDP socket */
	hSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (hSock != INVALID_SOCKET)
	{
	    /* connect to arbirary port and address (NOT loopback) */
	    stRmtAddr.sin_family = AF_INET;
	    stRmtAddr.sin_port = htons(IPPORT_ECHO);
	    stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1");
	    nRet = connect(hSock, (LPSOCKADDR) &stRmtAddr, sizeof(SOCKADDR));
	    if (nRet != SOCKET_ERROR)
	    {
		/* gte local address */
		getsockname(hSock, (LPSOCKADDR) &stLclAddr, 
			    (int FAR*)&nAddrSize);
	    }
	    closesocket(hSock);
	}
	else
	{
	    nRet = WSAGetLastError();
	    if (nRet == WSANOTINITIALISED)
		return(-1);
	    else
		return(0);
	}
    }
    return(stLclAddr.sin_addr.s_addr);
}   
    
/*
 * tf_get_pname() reads the principal's name from the ticket file. It
 * should only be called after tf_init() has been called.  The
 * principal's name is filled into the "p" parameter.  If all goes well,
 * KSUCCESS is returned.  If tf_init() wasn't called, TKT_FIL_INI is
 * returned.  If the name was null, or EOF was encountered, or the name
 * was longer than ANAME_SZ, TKT_FIL_FMT is returned.
 */
    
int
tf_get_pname(
    char   *p
    )
{   
    char        *principal = NULL; 
    int	        rc;
    char        principal_name[ANAME_SZ];
    char        instance[INST_SZ];
    char        realm[REALM_SZ];
    char        first_part[256];
    char        second_part[256];
    char        temp[1024];
    THREAD_DATA *ThreadData;

    ThreadData = GetThreadData();
    if (!ThreadData)
        return KFAILURE;

    if (!g_cc_ctx || !ThreadData->g_cache_ptr)
    {
	if (krb_debug)
	    kdebug_err("tf_get_pname called before tf_init.\n");
        return TKT_FIL_INI;
    }

    if ((rc = cc_get_principal(g_cc_ctx, ThreadData->g_cache_ptr, &principal)) != 0)
    {
    	if ((principal) != NULL)
            cc_free_principal(g_cc_ctx, &principal);
        return(rc);
    }

    memset(temp, '\0', sizeof(temp));
    memset(principal_name, '\0', sizeof(principal_name));
    memset(instance, '\0', sizeof(instance));
    memset(realm, '\0', sizeof(realm));
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));

    sscanf(principal, "%[/0-9a-zA-Z._-]@%[/0-9a-zA-Z._-]", first_part, 
           second_part);
    strcpy(temp, first_part);
    strcpy(realm, second_part);
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));
    sscanf(temp, "%[@0-9a-zA-Z._-]/%[@0-9a-zA-Z._-]", first_part, second_part);
    strcpy(principal_name, first_part);
    strcpy(instance, second_part);

    strcpy(p, principal_name);
    cc_free_principal(g_cc_ctx, &principal);
    return(KSUCCESS);
}
	
/*
 * tf_get_pinst() reads the principal's instance from a ticket file.
 * It should only be called after tf_init() and tf_get_pname() have been
 * called.  The instance is filled into the "inst" parameter.  If all
 * goes well, KSUCCESS is returned.  If tf_init() wasn't called,
 * TKT_FIL_INI is returned.  If EOF was encountered, or the instance
 * was longer than ANAME_SZ, TKT_FIL_FMT is returned.  Note that the
 * instance may be null.
 */
int FAR tf_get_pinst(inst)
    char *inst;
{
    char        *principal = NULL; 
    int	        rc;
    char        principal_name[ANAME_SZ];
    char        instance[INST_SZ];
    char        realm[REALM_SZ];
    char        first_part[256];
    char        second_part[256];
    char        temp[1024];
    THREAD_DATA *ThreadData;

    ThreadData = GetThreadData();
    if (!ThreadData)
        return KFAILURE;

    if (!g_cc_ctx || !ThreadData->g_cache_ptr)
    {
	if (krb_debug)
	    kdebug_err("tf_get_pinst called before tf_init.\n");
        return TKT_FIL_INI;
    }

    if ((rc = cc_get_principal(g_cc_ctx, ThreadData->g_cache_ptr,
                               &principal)) != 0)
    {
    	if ((principal) != NULL)
            cc_free_principal(g_cc_ctx, &principal);
        return(rc);
    }

    memset(temp, '\0', sizeof(temp));
    memset(principal_name, '\0', sizeof(principal_name));
    memset(instance, '\0', sizeof(instance));
    memset(realm, '\0', sizeof(realm));
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));

    sscanf(principal, "%[/0-9a-zA-Z._-]@%[/0-9a-zA-Z._-]", first_part, 
           second_part);
    strcpy(temp, first_part);
    strcpy(realm, second_part);
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));
    sscanf(temp, "%[@0-9a-zA-Z._-]/%[@0-9a-zA-Z._-]", first_part, second_part);
    strcpy(principal_name, first_part);
    strcpy(instance, second_part);

    strcpy(inst, instance);
    cc_free_principal(g_cc_ctx, &principal);
    return(KSUCCESS);
}                               
    
/******************************************/
/* tf_get_cred_begin(): internal          */
/******************************************/
int
tf_get_cred_begin(
    THREAD_DATA *ThreadData
    )
{
    /* Invariant: TheadData != NULL
     *
     * XXX - should we toss in an ASSERT?  Maybe, but instead, we toss
     * up a KFAILURE since this function can return an error anyway.
     * Arguably, we should try to dereference the pointer and crash so
     * that someone fixes the broken invariant.
     *
     * As far as freeing the iterator, there really is nothing useful
     * a caller can do with an error from here.  Maybe logging it
     * somewhere would be useful.  So we ignore that.
     *
     * Note that a return value of 0 is ok and non-zero will be 
     * a krb4 error code of some sort.
     */
    int ret;
    if (!ThreadData)
        return KFAILURE;

    if (ThreadData->itCreds) {
        cc_seq_fetch_creds_end(g_cc_ctx, &ThreadData->itCreds);
        ThreadData->itCreds = NULL;
    }
    ret = cc_seq_fetch_creds_begin(g_cc_ctx, ThreadData->g_cache_ptr, 
                                   &ThreadData->itCreds);
    if (!ret)
        return KSUCCESS;
    if (ret == CC_NO_EXIST)
        return RET_NOTKT;
    return KFAILURE;
}

/******************************************/
/* tf_get_cred_end(): internal            */
/******************************************/
void
tf_get_cred_end(
    THREAD_DATA *ThreadData
    )
{
    /* Invariant: TheadData != NULL
     *
     * XXX - should we toss in an ASSERT?  Progably, but instead, we
     * simply ignore it (by returning).  Arguably, we should try to
     * dereference the pointer and crash so that someone fixes the
     * broken invariant.
     *
     * As far as freeing the iterator, there really is nothing useful
     * a caller can do with an error from here.  Maybe logging it
     * somewhere would be useful.  So we ignore that.
     */
    if (!ThreadData)
        return;
    if (ThreadData->itCreds != NULL)
        cc_seq_fetch_creds_end(g_cc_ctx, &ThreadData->itCreds);
    ThreadData->itCreds = NULL;
}

/*
 * tf_get_cred() reads a CREDENTIALS record from a ticket file and fills
 * in the given structure "c".  It should only be called after tf_init(),
 * tf_get_pname(), and tf_get_pinst() have been called. If all goes well,
 * KSUCCESS is returned.  Possible error codes are:
 *
 * TKT_FIL_INI  - tf_init wasn't called first
 * TKT_FIL_FMT  - bad format
 * EOF          - end of file encountered
 */
int
tf_get_cred(
    CREDENTIALS *c
    )
{

    THREAD_DATA *ThreadData;
    cred_union *cpu = NULL;
    int ret;

    ThreadData = GetThreadData();
    if (!ThreadData)
        return KFAILURE;

    if (!g_cc_ctx || !ThreadData->g_cache_ptr)
    {
	if (krb_debug)
	    kdebug_err("tf_get_cred called before tf_init.\n");
        return TKT_FIL_INI;
    }

    if (ThreadData->itCreds == NULL)
    {
	if (ret = tf_get_cred_begin(ThreadData))
            return ret;
    }

    ret = cc_seq_fetch_creds_next(g_cc_ctx, &cpu, ThreadData->itCreds);

    if (ret == CC_END)
    {
	tf_get_cred_end(ThreadData);
	return(EOF);
    }
    if (ret == CC_NOTFOUND)
    {
	tf_get_cred_end(ThreadData);
	return(RET_NOTKT);
    }
    else if (ret != CC_NOERROR)
    {
	tf_get_cred_end(ThreadData);
	return(TKT_FIL_FMT);
    }
		
    if (cc2mit(cpu, c) != KSUCCESS)
	return(TKT_FIL_FMT);
		
    return(KSUCCESS);
}
    
/*
 * tf_close() closes the ticket file and sets "fd" to -1. If "fd" is
 * not a valid file descriptor, it just returns.  It also clears the
 * buffer used to read tickets.
 *
 * The return value is not defined.
 */
    
void
tf_close()
{
    THREAD_DATA *ThreadData;

    ThreadData = GetThreadData();
    if (!ThreadData)
        return;

    if (ThreadData->g_cache_ptr != NULL)
    {
        tf_get_cred_end(ThreadData);
        cc_close(g_cc_ctx, &ThreadData->g_cache_ptr);
    }
    return;
}
    
/*
 * tf_save_cred() appends an incoming ticket to the end of the ticket
 * file.  You must call tf_init() before calling tf_save_cred().
 *
 * The "service", "instance", and "realm" arguments specify the
 * server's name; "session" contains the session key to be used with
 * the ticket; "kvno" is the server key version number in which the
 * ticket is encrypted, "ticket" contains the actual ticket, and
 * "issue_date" is the time the ticket was requested (local host's time).
 *
 * Returns KSUCCESS if all goes well, TKT_FIL_INI if tf_init() wasn't
 * called previously, and KFAILURE for anything else that went wrong.
 */
	
int
tf_save_cred(
    char   *service,            /* Service name */
    char   *instance,           /* Instance */
    char   *realm,              /* Auth domain */
    C_Block session,            /* Session key */
    int     lifetime,           /* Lifetime */
    int     kvno,               /* Key version number */
    KTEXT   ticket,             /* The ticket itself */
    long    issue_date          /* The issue time */
    )
{
    long                ipAddr;
    char                loc_addr[ADDR_SZ];
    struct in_addr      LocAddr;
    int                 ret;
    V4Cred_type         c;
    cred_union          cpu;
    char                *name = NULL;
    char                *principal = NULL;
    THREAD_DATA         *ThreadData;
    char                *env_ticket = NULL;
    cred_union	        *seq_cred_ptr = NULL;
    char                principal_name[ANAME_SZ];
    char                principal_instance[INST_SZ];
    char                principal_realm[REALM_SZ];
    char                first_part[256];
    char                second_part[256];
    char                temp[1024];

    ThreadData = GetThreadData();
    if (!ThreadData)
        return KFAILURE;

    if (!g_cc_ctx || !ThreadData->g_cache_ptr)
        return TKT_FIL_INI;

    ipAddr = LocalHostAddr();
    if (ipAddr == -1)
    {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1,1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
	    return(KFAILURE);
	}
    }

    ipAddr = LocalHostAddr();
    LocAddr.s_addr = ipAddr;
    strcpy(loc_addr,inet_ntoa(LocAddr));

    ret = cc_get_principal(g_cc_ctx, ThreadData->g_cache_ptr, &principal);
    if (CC_NOERROR != ret)
    {
        return(ret);                         //sgr wrong return code
    }

    memset(temp, '\0', sizeof(temp));
    memset(principal_name, '\0', sizeof(principal_name));
    memset(principal_instance, '\0', sizeof(principal_instance));
    memset(principal_realm, '\0', sizeof(principal_realm));
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));

    sscanf(principal, "%[/0-9a-zA-Z._-]@%[/0-9a-zA-Z._-]", first_part, 
           second_part);
    strcpy(temp, first_part);
    strcpy(principal_realm, second_part);
    memset(first_part, '\0', sizeof(first_part));
    memset(second_part, '\0', sizeof(second_part));
    sscanf(temp, "%[@0-9a-zA-Z._-]/%[@0-9a-zA-Z._-]", first_part, second_part);
    strcpy(principal_name, first_part);
    strcpy(principal_instance, second_part);

    cpu.cred_type = CC_CRED_V4;

    memset(&c, '\0', sizeof(c));
    memcpy(c.principal, principal_name, (strlen(principal_name)+1));
    memcpy(c.principal_instance, principal_instance, 
           (strlen(principal_instance)+1));
    memcpy(c.service, service, (strlen(service)+1));
    memcpy(c.service_instance, instance, (strlen(instance)+1));
    memcpy(c.realm, principal_realm, (strlen(principal_realm)+1));
    memcpy(c.session_key, session, 9);
    memcpy(&c.address, (unsigned char *)&LocAddr, 4);

    c.kvno = kvno;
    c.kversion = CC_CRED_V4;
    c.str_to_key = remember_the_function;
    c.issue_date = issue_date;
    c.lifetime = lifetime;
    c.ticket_sz = ticket->length;

    memcpy(c.ticket,(char *)ticket->dat,ticket->length);

    cpu.cred.pV4Cred = &c;

    if (tf_get_cred_begin(ThreadData))
        return KFAILURE;

    if (strcmp(service, "krbtgt")==0)
    {	
        while ((ret = cc_seq_fetch_creds_next(g_cc_ctx, &seq_cred_ptr, 
                                              ThreadData->itCreds)) != CC_END)
        {
            if (ret == CC_NOERROR)
            {
                if (seq_cred_ptr->cred_type == CC_CRED_V4)
                    cc_remove_cred(g_cc_ctx, ThreadData->g_cache_ptr, *seq_cred_ptr);
                cc_free_creds(g_cc_ctx, &seq_cred_ptr);	
            }
            if (ret == CC_NOTFOUND)
            {
                ret = CC_NOERROR;
                break;
            }
            if (ret != CC_NOERROR)
            {
                tf_get_cred_end(ThreadData);
                cc_free_principal(g_cc_ctx, &principal);
                return(KFAILURE);
            }
        }
    }
    tf_get_cred_end(ThreadData);
    ret = cc_store(g_cc_ctx, ThreadData->g_cache_ptr, cpu);
    if (ret != CC_NOERROR)
    {
        cc_free_principal(g_cc_ctx, &principal);
        tf_close(); // XXX - is this really correct?  If so, we should doc it.
        return(KFAILURE);
    }
    cc_free_principal(g_cc_ctx, &principal);
    return(KSUCCESS);
}
    
#ifdef MM_CACHE
/*
    ** Given a common cache cred, make an MIT cred
    ** calloc mem for mit's cred, so mitC should be NULL on call
    ** free cc_cred, ccC should be returned NULL
    */
int
cc2mit(
    cred_union* ccC,
    CREDENTIALS* mitC
    )
{
    struct in_addr LocAddr;       

    if (0 == (*ccC).cred.pV4Cred)
    {
	return(KFAILURE);
    }
		
    /* if called with non-null ptr to mitC, not fatal, but pray
     */
    if (0 == mitC)
    {
	if ((mitC = calloc(1, sizeof(CREDENTIALS))) == 0)
	{
	    return(KFAILURE);
	}
    } /* else, pray they allocated enough space */
		
    mitC->ticket_st.mbz = 0;   /* should still be 0 after all the memcpys */

    memcpy(mitC->service, (*ccC).cred.pV4Cred->service,
	   (strlen((*ccC).cred.pV4Cred->service)+1));
    memcpy(mitC->instance, (*ccC).cred.pV4Cred->service_instance,
	   (strlen((*ccC).cred.pV4Cred->service_instance)+1));
    memcpy(mitC->realm, (*ccC).cred.pV4Cred->realm,
	   (strlen((*ccC).cred.pV4Cred->realm)+1));
    memcpy(mitC->session, (*ccC).cred.pV4Cred->session_key, 9);
    mitC->lifetime = (*ccC).cred.pV4Cred->lifetime;
    mitC->kvno = (*ccC).cred.pV4Cred->kvno;
    mitC->ticket_st.length = (*ccC).cred.pV4Cred->ticket_sz;
    memcpy(mitC->ticket_st.dat, (*ccC).cred.pV4Cred->ticket,
	   (*ccC).cred.pV4Cred->ticket_sz);
    mitC->issue_date = (*ccC).cred.pV4Cred->issue_date;
    memcpy(mitC->pname, (*ccC).cred.pV4Cred->principal,
	   (strlen((*ccC).cred.pV4Cred->principal)+1));
    memcpy(mitC->pinst, (*ccC).cred.pV4Cred->principal_instance,
	   (strlen((*ccC).cred.pV4Cred->principal_instance)+1));
    memcpy(&LocAddr, &(*ccC).cred.pV4Cred->address, 4);
    strcpy(mitC->address, inet_ntoa(LocAddr));

    if (0 != mitC->ticket_st.mbz)
    {
	return(KFAILURE);
    }

    cc_free_creds(g_cc_ctx, &ccC);
    return(KSUCCESS);       /* with mitCredentials */
}
#endif /* MM_CACHE */
