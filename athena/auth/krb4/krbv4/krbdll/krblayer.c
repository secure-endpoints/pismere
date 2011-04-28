#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <sys/types.h>
#include <winsock.h>
#include <krb.h>
#include <prot.h>
#include <time.h>

#include <winver.h>

#include <com_err.h>
#include "leasherr.h"

#ifdef OS2
#include "krb_err.h"
#else
#include "krberr.h"
#endif

#include <winkrbid.h>

#include <leashwin.h>

// #include <krblayer.h>

static char FAR *err_context;  


#define LEN     64               /* Maximum Hostname Length */

#define LIFE    DEFAULT_TKT_LIFE /* lifetime of ticket in 5-minute units */
#define KRB_FILE_NAME "KRBV4WIN.DLL"

// local macros  stolen from leash.h
#ifndef MAKEWORD
#define MAKEWORD(low, high) \
    ((WORD)(((BYTE)(low)) | (((UINT)((BYTE)(high))) << 8)))
#endif /*MAKEWORD*/

#if !defined(BSDSOCK)
/* Functions just like Unix gethostname... */
int FAR PASCAL gethostname(LPSTR name, int size)
{
    LONG ip_addr;
    HANDLE res;
    LPSTR lps;
    // struct in_addr addr;

    res = NULL;
    if (size <= 0) {
	return (-1);
    }

    if (-1 == (ip_addr = getmyipaddr())) {
	return (-1);
    }

#ifdef MEWEL
    lstrcpy(name, (LPSTR)raddr(ip_addr));
    /* XXX incomplete due to laziness */
#else
    res = raddr(ip_addr);
    if (res == NULL) {
	inet_ntoa( (DWORD)ip_addr, lps );
    } else if (!(lps = GlobalLock(res))) {
	GlobalFree(res);
	res = NULL;
	/* return (-1); */
    }

    if (lstrlen(lps) > size) {
	*(lps+size-1) = '\0';
    }
    lstrcpy(name, lps);
    if( res != NULL ){
	GlobalUnlock(res);
	GlobalFree(res);
	res = NULL;
    }
#endif
    return (0);
}
#endif /*!WINSOCK*/

#ifdef NOLEASHCOMPAT
long FAR PASCAL lsh_kinit(char *principal, char *password, int lifetime)

{
    char    aname[ANAME_SZ];
    char    inst[INST_SZ];
    char    realm[REALM_SZ];
    char    buf[LEN];
    long    k_errno; 
    
    err_context = "parsing principal";
    inst[0] = '\0';
    realm[0] = '\0';
    
    if (principal &&
	(k_errno = kname_parse(aname, inst, realm, principal))
	!= KSUCCESS) {
	return KRBERR(k_errno);
    }
    
    err_context = "getting hostname";
    if (k_errno = k_gethostname((LPSTR)buf, LEN)) {
#ifdef WINSOCK
	if (k_errno) k_errno=255;
#endif
	return KRBERR(k_errno);
    }
    
	if (lifetime < 5)
		lifetime = 1;
	else
		lifetime /= 5;
    
	/* This should be changed if the maximum ticket lifetime */
	/* changes */
    
	if (lifetime > 255)
		lifetime = 255;
    
	err_context = "getting realm";
	if (!*realm && krb_get_lrealm(realm, 1)) {
	    return LSH_FAILEDREALM;
	}
    
	err_context = "checking principal";
	if ((!*aname) || (!k_isname(aname)))
	    return LSH_INVPRINCIPAL;
    
	/* optional instance */
	if (!k_isinst(inst))
	    return LSH_INVINSTANCE;
    
	if (!k_isrealm(realm))
	    return LSH_INVREALM;
    
	err_context = "fetching ticket";
	k_errno = krb_get_pw_in_tkt((LPSTR)aname, (LPSTR)inst, (LPSTR)realm,
				    (LPSTR)"krbtgt", (LPSTR)realm, lifetime, (LPSTR)password);
	if (k_errno)
	    return KRBERR(k_errno);
	return 0L;
}


long FAR PASCAL lsh_checkpwd(char *principal, char *password)
{
#if !defined(OS2) && !defined(WIN32) && defined(TKT_KERBMEM)
    HANDLE old_ticket = NULL;
    long k_errno;
    
    k_errno = kadm_copy_tickets(principal, &old_ticket);
    if( k_errno != AD_OK ){
        if( old_ticket != NULL ){
	    GlobalFree(old_ticket);
        }
        old_ticket = NULL;
    }
    k_errno = lsh_kinit(principal, password, 0);
    kadm_copy_back_tickets(old_ticket, principal);
    return k_errno;
#else
    return lsh_kinit(principal, password, 0);
#endif
}
#endif

extern long kadm_change_your_password(LPSTR principal, LPSTR old_password, 
				      LPSTR new_password, 
				      void FAR * info_desc);

#ifdef NOLEASHCOMPAT

long FAR PASCAL lsh_changepwd(char *principal, char *password, 
			      char *newpassword, LPSTR return_handle)
{
    long k_errno;
    
    k_errno = kadm_change_your_password((LPSTR) principal, 
					(LPSTR) password, 
					(LPSTR) newpassword,
					(void FAR *) return_handle);
    return k_errno;
}

long lsh_getprincipal(char *principal)
{
    return 0;
} 


long FAR PASCAL lsh_kdestroy(void)
{
    int k_errno;
    
    k_errno = dest_tkt();
    
    if (k_errno)
	return KRBERR(k_errno);
    
    return 0;
}

#endif

char *short_date(long *dp)
{
    register char *cp;
    /*
      #ifndef ctime
      extern char *ctime();
      #endif
    */
    _tzset();
    cp = ctime(dp) + 4;
    cp[15] = '\0';
    return (cp);
}

int com_addr(CREDENTIALS *c)
{
    char messageBuf[56]; 
    long ipAddr;
    char loc_addr[ADDR_SZ];                                
    char service[40];
    char instance[40];
    char addr[40];
    char realm[40];
    int tf_status;
    struct in_addr LocAddr;
    
    while(1) {
	ipAddr = LocalHostAddr();
	if( ipAddr = NULL ){
	    WORD wVersionRequested;
	    WSADATA wsaData;
	    int err;
			
	    wVersionRequested = MAKEWORD( 1,1);
	    err = WSAStartup( wVersionRequested, &wsaData );
	    if( err != 0 ){
				
		return (KFAILURE);
				
	    }
	}
		
	ipAddr = LocalHostAddr();
	if( ipAddr == 0 || ipAddr == -1 ){
	    return(-1);
	}
	LocAddr.s_addr = ipAddr;    
	strcpy(loc_addr,inet_ntoa(LocAddr));
	if ( strcmp(c->address,loc_addr) != 0) {
	    lsh_kdestroy ();
	    return(-1); // they they are not the same IP addresses
	    break;
	} 
	break;
    } //while()
    
    return(0);
    
}


#ifdef NOLEASHCOMPAT
long FAR PASCAL lsh_klist(HWND hlist, TICKETINFO FAR *ticketinfo)
{
    char    pname[ANAME_SZ];
    char    pinst[INST_SZ];
    char    prealm[REALM_SZ];
    char    buf[MAX_K_NAME_SZ+40];
    LPSTR   cp;
    long    expdate;
    int     k_errno;
    CREDENTIALS c;
    int newtickets;
    int open = 0;
    int lsh_errno;
    
    /*
     * Since krb_get_tf_realm will return a ticket_file error,
     * we will call tf_init and tf_close first to filter out
     * things like no ticket file.  Otherwise, the error that
     * the user would see would be
     * klist: can't find realm of ticket file: No ticket file (tf_util)
     * instead of
     * klist: No ticket file (tf_util)
     */
    
    SendMessage(hlist, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hlist, LB_RESETCONTENT, 0, 0L);
    //if(com_addr()){
    // we're not able to get a local IP address
    
    //}
    newtickets = NO_TICKETS;
    
    err_context = (LPSTR)"tktf1";
    /* Open ticket file */
    if (k_errno = tf_init(TKT_FILE, R_TKT_FIL))
	goto cleanup;
    
    /* Close ticket file */
    (void) tf_close();
    /*
     * We must find the realm of the ticket file here before calling
     * tf_init because since the realm of the ticket file is not
     * really stored in the principal section of the file, the
     * routine we use must itself call tf_init and tf_close.
     */
    err_context = "tf realm";
    if ((k_errno = krb_get_tf_realm(TKT_FILE, prealm)) != KSUCCESS)  
	goto cleanup;
	
    /* Open ticket file */
    err_context = "tf init";
    if (k_errno = tf_init(TKT_FILE, R_TKT_FIL))
	goto cleanup;
    
    open = 1;
    
    err_context = "tf pname";
    /* Get principal name and instance */
    if ((k_errno = tf_get_pname(pname)) ||
	(k_errno = tf_get_pinst(pinst)))
	goto cleanup;
    
    /*
     * You may think that this is the obvious place to get the
     * realm of the ticket file, but it can't be done here as the
     * routine to do this must open the ticket file.  This is why
     * it was done before tf_init.
     */
    
    wsprintf((LPSTR)ticketinfo->principal,"%s%s%s%s%s", (LPSTR)pname,
	     (LPSTR)(pinst[0] ? "." : ""), (LPSTR)pinst,
	     (LPSTR)(prealm[0] ? "@" : ""), (LPSTR)prealm);
    newtickets = GOOD_TICKETS;
    
    err_context = "tf cred";
    while ((k_errno = tf_get_cred(&c)) == KSUCCESS) {
		
	expdate = c.issue_date + c.lifetime * 5L * 60L;
		
	if (!lstrcmp((LPSTR)c.service, (LPSTR)TICKET_GRANTING_TICKET) &&
	    !lstrcmp((LPSTR)c.instance, (LPSTR)prealm)) { 
			
	    ticketinfo->issue_date = c.issue_date;
	    ticketinfo->lifetime = c.lifetime * 5L * 60L;
	}
		
	cp = (LPSTR)buf;
	lstrcpy(cp, (LPSTR)short_date(&c.issue_date));
	cp += lstrlen(cp);
	wsprintf(cp,"\t%s\t%s%s%s%s%s",
		 (LPSTR)short_date(&expdate), (LPSTR)c.service,
		 (LPSTR)(c.instance[0] ? "." : ""),
		 (LPSTR)c.instance, (LPSTR)(c.realm[0] ? "@" : ""),
		 (LPSTR) c.realm);
	SendMessage(hlist, LB_ADDSTRING, 0, (LONG)(LPSTR)buf);
    } /* WHILE */
    
 cleanup:
    
    if (open)
	tf_close(); /* close ticket file */
    
    SendMessage(hlist, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hlist, NULL, TRUE);
    UpdateWindow(hlist);
    
    if (k_errno == EOF)
	k_errno = 0;
    
    ticketinfo->btickets = newtickets;
    
    if (k_errno != 0)
	return KRBERR(k_errno);
    return 0;
}
    
#endif
    
DWORD FAR PASCAL GetKRBDLLVersionFlags()
{
    char szFilename[255], szVerQ[90], *cp;
    VS_FIXEDFILEINFO FAR *lpffinfo;
    LPSTR lpAppVersion, lpAppName;
    LONG FAR *lpLangInfo;
    DWORD hVersionInfoID, size;
    GLOBALHANDLE hVersionInfo;
    LPSTR lpVersionInfo;
    int dumint, retval;

    /* GetModuleFileName(hThisInstance, (LPSTR)szFilename, 255); */
    size = GetFileVersionInfoSize((LPSTR) KRB_FILE_NAME, &hVersionInfoID);
    if( size == 0 ){
	return(-1);
    }
    hVersionInfo = GlobalAlloc(GHND, size);
    if( hVersionInfo == NULL ){
	return(-1);
    }
    lpVersionInfo = GlobalLock(hVersionInfo);
    if( lpVersionInfo == NULL ){
	GlobalFree(hVersionInfo);
	return(-1);
    }
    retval = GetFileVersionInfo(KRB_FILE_NAME, hVersionInfoID, size, lpVersionInfo);
    if( retval == 0 ){
	GlobalUnlock(hVersionInfo);
	GlobalFree(hVersionInfo);
	return(-1);
    }

    retval = VerQueryValue(lpVersionInfo, "\\", (LPSTR FAR *)&lpffinfo, &dumint);
    retval = lpffinfo->dwFileFlags;
		
    GlobalUnlock(hVersionInfo);
    GlobalFree(hVersionInfo);
    return retval;
}
    
    
//int FAR krb_get_cred(service,instance,realm,c)
    
int FAR get_cred(TICKETINFO FAR *ticketinfo, CREDENTIALS *cr)
{      
    char    pname[ANAME_SZ];
    char    pinst[INST_SZ];
    char    prealm[REALM_SZ];
    char    buf[MAX_K_NAME_SZ+40];
    LPSTR   cp;
    long    expdate;
    int     k_errno; 
    int newtickets;
    int open = 0;
		
    /*
     * Since krb_get_tf_realm will return a ticket_file error,
     * we will call tf_init and tf_close first to filter out
     * things like no ticket file.  Otherwise, the error that
     * the user would see would be
     * klist: can't find realm of ticket file: No ticket file (tf_util)
     * instead of
     * klist: No ticket file (tf_util)
     */
		
    err_context = (LPSTR)"tktf1";
    /* Open ticket file */
    if (k_errno = tf_init(TKT_FILE, R_TKT_FIL)) {    
	return k_errno;                          
    }
		
    /* Close ticket file */
    (void) tf_close();

    /*
     * We must find the realm of the ticket file here before calling
     * tf_init because since the realm of the ticket file is not
     * really stored in the principal section of the file, the
     * routine we use must itself call tf_init and tf_close.
     */
    err_context = "tf realm";
    if ((k_errno = krb_get_tf_realm(TKT_FILE, prealm)) != KSUCCESS)
	return k_errno;
		
    /* Open ticket file */
    err_context = "tf init";
    if (k_errno = tf_init(TKT_FILE, R_TKT_FIL))
	return k_errno;

    open = 1;
		
    err_context = "tf pname";
    /* Get principal name and instance */
    if ((k_errno = tf_get_pname(pname)) ||
	(k_errno = tf_get_pinst(pinst)))
	return k_errno;
    /*
     * You may think that this is the obvious place to get the
     * realm of the ticket file, but it can't be done here as the
     * routine to do this must open the ticket file.  This is why
     * it was done before tf_init.
     */

    wsprintf((LPSTR)ticketinfo->principal,"%s%s%s%s%s", (LPSTR)pname,
	     (LPSTR)(pinst[0] ? "." : ""), (LPSTR)pinst,
	     (LPSTR)(prealm[0] ? "@" : ""), (LPSTR)prealm);
    newtickets = GOOD_TICKETS; 
    if ((k_errno = tf_get_cred(cr)) != KSUCCESS) {
	tf_close();
	return k_errno;
    }
    tf_close();
    return KSUCCESS; 
}
