#include "conf.h"

#include <stdio.h>
#include <windows.h>
#include <krb.h>
#include <winsock.h>

LPSTR krb_get_phost(LPSTR);

#define KRB_SENDAUTH_VERS "AUTHV0.1" /* MUST be KRB_SENDAUTH_VLEN chars */
#define HEY_PAUL_WHAT_ERROR KFAILURE

int krb_sendauth_udp(long options, int fd, KTEXT_FP ticket,
	const LPSTR service, const LPSTR inst, LPSTR realm, LONG checksum,
	MSG_DAT *msg_data, CREDENTIALS *cred, Key_schedule *schedule,
	struct sockaddr_in *laddr, struct sockaddr_in *faddr, LPSTR version,
	LPSTR buffer, int sz)
{
	int rem, i, cc;
	char srv_inst[INST_SZ];
	char krb_realm[REALM_SZ];
	char buf[BUFSIZ];
	long tkt_len;
	u_char priv_buf[1024];
	u_long cksum;
	
	rem = KSUCCESS;
	
	/* get current realm if not passed in */
	if (!realm) {
		rem = krb_get_lrealm(krb_realm,1);
		if (rem != KSUCCESS)
			return (rem);
		realm = krb_realm;
	}
	
	/* Copy instance into local storage, canonicalizing if desired */
	if (options & KOPT_DONT_CANON)
		(void) strncpy(srv_inst, inst, INST_SZ);
	else
		(void) strncpy((LPSTR)srv_inst, (LPSTR)krb_get_phost(inst), INST_SZ);
		
	/* Get the ticket if desired */
	if (!(options & KOPT_DONT_MK_REQ)) {
		rem = krb_mk_req(ticket, service, srv_inst, realm, checksum);
		if (rem != KSUCCESS)
			return (rem);
	}
	
#ifdef ATHENA_COMPAT
	/* this is only for compatibility with old servers */
	if (options & KOPT_DO_OLDSTYLE) {
		(void) sprintf(buf, "%d ", ticket->length);
		(void) write(fd, buf, strlen(buf));
		(void) write(fd, (char *) ticket->dat, ticket->length);
		return (rem);
	}
#endif /* ATHENA_COMPAT */
	/* if the mutual auth, get credentials so we have service session
	 * keys for decryption below
	 */
	if (options & KOPT_DO_MUTUAL)
	{
		/*if (cc == 1)
		 *{
		 *      krb_get_cred(service, srv_inst, realm, cred);
		 *      return (cc);
		 *}
		 */
		krb_get_cred(service, srv_inst, realm, cred);
	}
			
	/* Zero the buffer */
	(void) bzero(buf, BUFSIZ);
	
	/* insert version strings */
	(void) strncpy(buf, KRB_SENDAUTH_VERS, KRB_SENDAUTH_VLEN);
	
	/* increment past version strings */
	i = 2*KRB_SENDAUTH_VLEN;
	
	/* put ticket length into buffer */
	tkt_len = htonl((unsigned long) ticket->length);
	(void) bcopy((char *) &tkt_len, buf+i, ticket->length);
	i += ticket->length;
	
	/* Normally, we write the request to the server
	** For udp, esp. TechNotify type protocols, we
	** just return the buffer.  
	*/
	if (sz < i)
		return HEY_PAUL_WHAT_ERROR;     /* Buffer not big enough */
	bcopy(buf, buffer, i);
	return (KSUCCESS);
}

int krb_sendauth_udpchk(long options, LONG checksum, MSG_DAT *msg_data, 
	CREDENTIALS* cred, Key_schedule *schedule, struct sockaddr_in *laddr,
	struct sockaddr_in *faddr, LPSTR packet)
{                   
	int cc;
	long tkt_len;      
	u_long cksum;
	u_char priv_buf[1024];
	
	/* mutual authentication, if desired */
	if (options & KOPT_DO_MUTUAL) {
		/* get the length of the reply */
		bcopy(packet,(char*) &tkt_len, sizeof(tkt_len));
		tkt_len = ntohl((unsigned long) tkt_len);
		
		/* if the length is negative, the server failed to recognize us
		 */
		if ((tkt_len < 0) || (tkt_len > sizeof(priv_buf)))
			return (KFAILURE);      /* XXX */
		/* read the reply... */
		bcopy(packet+sizeof(tkt_len), (char *)priv_buf, 
			(int) tkt_len);
			
		/* ...and decrypt it */
#ifndef NOENCRYPTION
		key_sched(cred->session, *schedule);
#endif /* !NOENCRYPTION */
		if (cc = krb_rd_priv(priv_buf, (unsigned long) tkt_len, *schedule,
				cred->session, faddr, laddr, msg_data))
			return (cc);
				
		/* fetch the (modified) checksum */
		(void) bcopy((char *)msg_data->app_data, (char *)&cksum,
			sizeof(cksum));
		cksum = ntohl(cksum);
		
		/* if it doesn't match, fail */
		if (cksum != checksum + 1)
			return (KFAILURE);      /* XXX */
	}
	return (KSUCCESS);
}
