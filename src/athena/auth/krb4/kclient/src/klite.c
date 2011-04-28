/*

a version of Project Mandarin's Kerberos Client for Windows
that actually does *not* use Kerberos tickets.  instead, it
uses cleartext.

Copyright © 1996 by Project Mandarin, Inc.

*/

#include <windows.h>
#include <winsock.h>

#include <string.h>
#include <assert.h>
#include <time.h>

#include "klite.h"
#include "klite_rc.h"

extern HANDLE hInst ;
extern FAKETICKET		ft ;			// fake ticket
extern LPFAKETICKET	lpFt ;		
extern char szTemp[] ;			// LPSTR to PSTR conversion purposes
extern LPSTR lpszTemp ;				// alleviates some casting
extern LPSTR lpszCmd  ;				// ahSSP command
extern WORD	iVer  ;					// ahSSP version
extern BOOL bTraceL1 ;				// Socket trace option
extern BOOL bTraceL2 ;				// ahssp trace option
extern char szErrorText[] ;	// error text
extern LPSTR lpszErrorText ;

/* ********************** Function Prototypes **************** */

int	KL_CALLTYPE KL_EXPORT krb_get_pw_in_tkt (LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, int, LPSTR) ;
int	KL_CALLTYPE KL_EXPORT krb_get_cred (LPSTR, LPSTR, LPSTR, LPCREDENTIALS) ;
int	KL_CALLTYPE KL_EXPORT krb_get_lrealm (LPSTR, int) ;
BOOL	KL_CALLTYPE KL_EXPORT k_isname (LPSTR) ;
BOOL	KL_CALLTYPE KL_EXPORT k_isrealm (LPSTR) ;
LPSTR KL_CALLTYPE KL_EXPORT get_krb_err_txt_entry (int) ;
LONG	KL_CALLTYPE KL_EXPORT lsh_klist (HANDLE, TICKETINFO FAR *) ;
int	KL_CALLTYPE KL_EXPORT krb_mk_req (KTEXT, LPSTR, LPSTR, LPSTR, long) ;
void	KL_CALLTYPE KL_EXPORT set_krb_debug (BOOL)  ;
void	KL_CALLTYPE KL_EXPORT set_krb_ap_req_debug (BOOL)  ;
int	KL_CALLTYPE KL_EXPORT kname_parse (LPSTR, LPSTR, LPSTR, LPSTR) ;
int	KL_CALLTYPE KL_EXPORT dest_tkt (void) ;
int	KL_CALLTYPE KL_EXPORT krb_check_serv(char *);
int	KL_CALLTYPE KL_EXPORT krb_sendauth(long,
					   int,
					   LPSTR,
					   LPSTR,
					   LPSTR,
					   LPSTR,
					   unsigned long,
					   LPSTR ,
					   LPSTR,
					   LPSTR,
					   struct sockaddr_in *,
					   struct sockaddr_in *,
					   LPSTR);





// NOTE: the following function IS NOT IMPLEMENTED in KLite.  It always
// returns an error code.  However, it is needed so that KClient can call it.
LONG KL_CALLTYPE KL_EXPORT lsh_changepwd (LPSTR, LPSTR, LPSTR, LPSTR) ;

extern BOOL KL_CALLTYPE BounceTicketOffServer (void) ;

/* -------------------------------------------------------------------
	krb_mk_req - 
	------------------------------------------------------------------- */
int KL_CALLTYPE KL_EXPORT krb_mk_req (KTEXT pKtext, LPSTR lpszServName, 
											LPSTR lpszServInst, LPSTR lpszServRealm, long lChecksum) 
	{
	assert (pKtext != NULL) ;
	
	if (_fstrlen(lpFt->szTicket) == 0)
		return (1) ;
		
	_fstrcpy (pKtext->dat, lpFt->szTicket) ;
	pKtext->length = _fstrlen (lpFt->szTicket) ;
	return (0) ;
	}
	

/* -------------------------------------------------------------------
	krb_get_pw_in_tkt - 
	------------------------------------------------------------------- */
int KL_CALLTYPE KL_EXPORT krb_get_pw_in_tkt (LPSTR lpszUserName, LPSTR lpszUserInst, LPSTR lpszUserRealm, 
							LPSTR lpszService, LPSTR lpszUserRealmX, int iLife, LPSTR lpszPass) 
	{	
	assert (lpszUserName != NULL) ;
	assert (lpszPass != NULL) ;
	
	// Store fake ticket
	wsprintf (lpFt->szTicket, "%s,%s", lpszUserName, lpszPass) ;	
	// save NetID for display purposes
	_fstrcpy (lpFt->szNetID, lpszUserName) ;	
	
	if (BounceTicketOffServer () == FALSE)
		return (IDS_ERR_AUTH_SERVER) ;
	
	// save time ticket was acqired
	time (&ft.lTimeAcquired) ;		// time() can't take a FAR pointer, so use the struct "directly"
	// calculate expiration time		
	lpFt->lTimeExpires = lpFt->lTimeAcquired + (5*60*iLife) ;
	lpFt->iLifeTime = iLife	;			// save life time
	return (0) ;
	}

/* -------------------------------------------------------------------
	krb_get_cred - 
	------------------------------------------------------------------- */
int KL_CALLTYPE KL_EXPORT krb_get_cred (LPSTR lpszService, LPSTR lpszUserInst, LPSTR lpszUserRealm, LPCREDENTIALS lpc) 
	{
	if (_fstrlen(lpFt->szTicket) > 0)
		{
		lpc->issue_date = lpFt->lTimeAcquired ;
		lpc->lifetime = lpFt->iLifeTime ;
		return (0) ;
		}
	else
		return (IDS_ERR_GET_CRED) ;
	}
	

/* -------------------------------------------------------------------
	krb_get_lrealm - Not used
	------------------------------------------------------------------- */
int KL_CALLTYPE KL_EXPORT krb_get_lrealm (LPSTR lpszRealm, int iValue) 
	{
	_fstrcpy (lpszRealm, (LPSTR)"TomServo") ;
	return (0) ;
	}
	
/* -------------------------------------------------------------------
	k_isname - Barely used
	------------------------------------------------------------------- */
BOOL KL_CALLTYPE KL_EXPORT k_isname (LPSTR lpszName)
	{
	if (lpszName != NULL && lpszName[0] != '\0')
		return (0) ;
	else
		return (IDS_ERR_NETID) ;
	}

/* -------------------------------------------------------------------
	k_isrealm - Not used
	------------------------------------------------------------------- */
BOOL KL_CALLTYPE KL_EXPORT k_isrealm (LPSTR lpszRealm)
	{
	return (0) ;
	}
	

/* -------------------------------------------------------------------
	get_krb_err_txt_entry (iKerror) -
	------------------------------------------------------------------- */
LPSTR KL_CALLTYPE KL_EXPORT get_krb_err_txt_entry (int iKerror)
	{	
	LoadString (hInst, iKerror, lpszErrorText, 128) ;	
	return (lpszErrorText) ;
	}
	


/* -------------------------------------------------------------------
	lsh_klist - 
	------------------------------------------------------------------- */
LONG KL_CALLTYPE KL_EXPORT lsh_klist (HANDLE hListBox, TICKETINFO FAR * lpti) 
	{	
	LPSTR lp1 ;
	
	// clear out list box
	SendMessage (hListBox, LB_RESETCONTENT, 0, 0L) ;
	
	// only list if we have a NetID
	if (lpFt->bAuthenticated == FALSE)
		return (0) ;
	
	// note: ctime() can't take a FAR pointer, so use the struct "directly"
	// append expired time, due to funky way ctime() works.  Unfortunately, I can't
	// put this in 1 wsprintf() statement.  Dooh!
	wsprintf (lpszTemp, "%s\t", (LPSTR)ctime(&ft.lTimeAcquired)) ;
	_fstrcat (lpszTemp, (LPSTR)ctime(&ft.lTimeExpires)) ;
	_fstrcat (lpszTemp, (LPSTR)"\t") ;
	_fstrcat (lpszTemp, lpFt->szNetID) ;
	
	// remove '\n' from ctime output
	if ((lp1 = _fstrchr(lpszTemp, '\n')) != NULL)
		{
		*lp1 = ' ' ;
		lp1++ ;
		if ((lp1 = _fstrchr(lpszTemp, '\n')) != NULL)			
			*lp1 = ' ' ;
		}
		
	SendMessage (hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)lpszTemp) ;	
	return (0) ;
	}


/* *********************************************************** 
	set_krb_debug () - TRUE enable trace messages socket level detail.
	FALSE disables tracing.
	*********************************************************** */
void KL_CALLTYPE KL_EXPORT set_krb_debug (BOOL bState)  
	{
	bTraceL1 = bState;
	}

/* *********************************************************** 
	set_krb_ap_req_debug) - TRUE enable trace messages Ahssp.
	FALSE disables tracing.
	*********************************************************** */
void KL_CALLTYPE KL_EXPORT set_krb_ap_req_debug (BOOL bState)  
	{
	bTraceL2 = bState;
	}

/* *********************************************************** 
	kname_parse - 
	*********************************************************** */
int KL_CALLTYPE KL_EXPORT kname_parse (LPSTR lpszName, LPSTR lpszInst, LPSTR lpszRealm, LPSTR lpszService) 
	{
	_fstrcpy (lpszName, lpszService) ;
	return (0) ;
	}

/* *********************************************************** 
	dest_tkt - womp the fake ticket
	*********************************************************** */
int KL_CALLTYPE KL_EXPORT dest_tkt (void) 
	{
	_fmemset (lpFt, '\0', sizeof(FAKETICKET)) ;
	return (0) ;	
	}


/* *********************************************************** 
	lsh_changepwd - (theoretically) change the user's password
	*********************************************************** */


// NOTE: the following function IS NOT IMPLEMENTED in KLite.  It always
// returns an error code.  However, it is needed so that KClient can call it.
LONG KL_CALLTYPE KL_EXPORT lsh_changepwd (LPSTR lpszName, LPSTR lpszInst, LPSTR lpszRealm, LPSTR junk)
	{
		return (IDS_ERR_NOT_IMPLEMENTED);
	}



int	KL_CALLTYPE KL_EXPORT krb_check_serv(char *service){

	return(0);
}


int	KL_CALLTYPE KL_EXPORT krb_sendauth(long options,
					   int fd,
					   LPSTR ticket,
					   LPSTR service,
					   LPSTR inst,
					   LPSTR realm,
					   unsigned long checksum,
					   LPSTR msg_data,
					   LPSTR cred,
					   LPSTR schedule,
					   struct sockaddr_in *laddr,
					   struct sockaddr_in *faddr,
					   LPSTR version){

	return(0);
	/* this needs a lot more work but we hope no one will ever call it
	   from klite.
	 */

}
