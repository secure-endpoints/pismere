/*
 * Copyright 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * Revision 4.9.2.1  91/11/08  00:35:14  mja
 *      Update krb_get_in_tkt() to optionally loop through multiple
 *      decryption TGT reply decryption attempts so that we can handle
 *      taking to servers (like those in AFS) with non-standard string
 *      to key functions.
 *      [91/11/07  00:51:45  mja]
 * 
 */

#include "conf.h"

#include <windows.h>
#include <time.h>
#include <stdlib.h>

#include <errno.h>

#include <krb.h>
#include <des.h>
#include <prot.h>

#include <stdio.h>
#include <string.h>

/* use the bsd time.h struct defs for PC too! */
#include <sys\types.h>

#include <winsock.h>

int swap_bytes;
extern int krb_debug;

/*
 * decrypt_tkt(): Given user, instance, realm, passwd, key_proc
 * and the cipher text sent from the KDC, decrypt the cipher text
 * using the key returned by key_proc.
 */

static int
decrypt_tkt(
    char *user,
    char *instance,
    char *realm,
    char *arg,
    int (*key_proc)(),
    KTEXT *cipp
    )
{
    KTEXT cip = *cipp;
    C_Block key;                /* Key for decrypting cipher */
    Key_schedule key_s;                       
  
#ifndef NOENCRYPTION
    /* Attempt to decrypt it */
#endif

    /* generate a key */
    {
	register int rc;
	rc = (*key_proc)(user, instance, realm, arg, key);
	if (rc)
	    return(rc);
    }

#ifndef NOENCRYPTION  
    key_sched(key, key_s);
    pcbc_encrypt((C_Block *)cip->dat, (C_Block *)cip->dat,
		 (long) cip->length, key_s, key, 0);
#endif /* !NOENCRYPTION */

    /* Get rid of all traces of key */
    bzero((char *)key,sizeof(key));
    bzero((char *)key_s,sizeof(key_s));                
      
    return(0);
}

/*
 * krb_get_in_tkt() gets a ticket for a given principal to use a given
 * service and stores the returned ticket and session key for future
 * use.
 *
 * krb_get_in_tkt_preauth() does the same but appends preauthentication
 * data to the end of the request.
 *
 * The "user", "instance", and "realm" arguments give the identity of
 * the client who will use the ticket.  The "service" and "sinstance"
 * arguments give the identity of the server that the client wishes
 * to use.  (The realm of the server is the same as the Kerberos server
 * to whom the request is sent.)  The "life" argument indicates the
 * desired lifetime of the ticket; the "key_proc" argument is a pointer
 * to the routine used for getting the client's private key to decrypt
 * the reply from Kerberos.  The "decrypt_proc" argument is a pointer
 * to the routine used to decrypt the reply from Kerberos; and "arg"
 * is an argument to be passed on to the "key_proc" routine.
 *
 * If all goes well, krb_get_in_tkt() returns INTK_OK, otherwise it
 * returns an error code:  If an AUTH_MSG_ERR_REPLY packet is returned
 * by Kerberos, then the error code it contains is returned.  Other
 * error codes returned by this routine include INTK_PROT to indicate
 * wrong protocol version, INTK_BADPW to indicate bad password (if
 * decrypted ticket didn't make sense), INTK_ERR if the ticket was for
 * the wrong server or the ticket store couldn't be initialized.
 *
 * The format of the message sent to Kerberos is as follows:
 *
 * Size                 Variable                Field
 * ----                 --------                -----
 *
 * 1 byte               KRB_PROT_VERSION        protocol version number
 * 1 byte               AUTH_MSG_KDC_REQUEST |  message type
 *                      HOST_BYTE_ORDER         local byte order in lsb
 * string               user                    client's name
 * string               instance                client's instance
 * string               realm                   client's realm
 * 4 bytes              tlocal.tv_sec           timestamp in seconds
 * 1 byte               life                    desired lifetime
 * string               service                 service's name
 * string               sinstance               service's instance
 */
int
krb_get_in_tkt_preauth(
    char *user,
    char *instance,
    char *realm,
    char *service,
    char *sinstance,
    int life,
    int (*key_proc)(),
    int (*decrypt_proc)(),
    char *arg,
    char * preauth_p,
    int    preauth_len
    )
{
    KTEXT_ST pkt_st;
    KTEXT pkt = &pkt_st;        /* Packet to KDC */
    KTEXT_ST rpkt_st;
    KTEXT rpkt = &rpkt_st;      /* Returned packet */
    KTEXT_ST cip_st;
    KTEXT cip = &cip_st;        /* Returned Ciphertext */
    KTEXT_ST tkt_st;
    KTEXT tkt = &tkt_st;        /* Current ticket */
    C_Block ses;                /* Session key for tkt */
    int kvno;                   /* Kvno for session key */
    unsigned char *v = pkt->dat; /* Prot vers no */
    unsigned char *t = (pkt->dat+1); /* Prot msg type */
  
    char s_name[MAX_KTXT_LEN];
    char s_instance[MAX_KTXT_LEN];
    char rlm[MAX_KTXT_LEN];
    int lifetime;
    int msg_byte_order;
    int kerror;
    unsigned long exp_date;
    char *ptr; 
  
    struct timeval t_local;                          
  
    unsigned long rep_err_code;
  
    unsigned long kdc_time;   /* KDC time */ 
  
    /* BUILD REQUEST PACKET */

    /* Set up the fixed part of the packet */
    *v = (unsigned char) KRB_PROT_VERSION;
    *t = (unsigned char) AUTH_MSG_KDC_REQUEST;
    *t |= HOST_BYTE_ORDER;
  
    /* Now for the variable info */
    (void) strcpy((char *)(pkt->dat+2),user); /* aname */
    pkt->length = 3 + strlen(user);
    (void) strcpy((char *)(pkt->dat+pkt->length),
                  instance);    /* instance */
    pkt->length += 1 + strlen(instance);
    (void) strcpy((char *)(pkt->dat+pkt->length),realm); /* realm */
    pkt->length += 1 + strlen(realm);
  
    (void) gettimeofday(&t_local,(struct timezone *) 0);  
    /* timestamp */
    bcopy(&(t_local.tv_sec), (pkt->dat + pkt->length), 4);
    pkt->length += 4;
  
    *(pkt->dat+(pkt->length)++) = (char) life;
    (void) strcpy((char *)(pkt->dat+pkt->length),service);
    pkt->length += 1 + strlen(service);
    (void) strcpy((char *)(pkt->dat+pkt->length),sinstance);
    pkt->length += 1 + strlen(sinstance);

    if (preauth_len) {
	memcpy((char *)(pkt->dat+pkt->length), preauth_p, preauth_len);
        pkt->length += preauth_len;
    }

    rpkt->length = 0;
  
    /* SEND THE REQUEST AND RECEIVE THE RETURN PACKET */
  
    if (kerror = send_to_kdc(pkt, rpkt, realm) ){
        int sErr;

        sErr = WSAGetLastError();
        if( sErr == WSANOTINITIALISED ){
	    WORD wVersionRequested;
	    WSADATA wsaData;
	    int err;

	    wVersionRequested = MAKEWORD(1, 1);
	    err = WSAStartup( wVersionRequested, &wsaData );
	    if( err != 0 ){

		return (kerror);

	    } else {
		if(kerror = send_to_kdc(pkt, rpkt, realm)) return(kerror);
	    }
        } else {
            return(kerror);
        }

    }

    /* check packet version of the returned packet */
    if (pkt_version(rpkt) != KRB_PROT_VERSION)
        return(INTK_PROT);
  
    /* Check byte order */
    msg_byte_order = pkt_msg_type(rpkt) & 1;
    swap_bytes = 0;
    if (msg_byte_order != HOST_BYTE_ORDER) {
        swap_bytes++;
    }
    switch (pkt_msg_type(rpkt) & ~1) {                
    case AUTH_MSG_KDC_REPLY:
        break;
    case AUTH_MSG_ERR_REPLY:
        bcopy(pkt_err_code(rpkt), &rep_err_code, 4);
        if (swap_bytes) swap_u_long(rep_err_code);
        return((int)rep_err_code);
    default:
        return(INTK_PROT);
    }

    /* EXTRACT INFORMATION FROM RETURN PACKET */

    /* get the principal's expiration date */
    bcopy(pkt_x_date(rpkt), &exp_date, sizeof(exp_date));  
    if (swap_bytes) swap_u_long(exp_date);
    /* Extract the ciphertext */
    cip->length = pkt_clen(rpkt);       /* let clen do the swap */
  
    if ((cip->length < 0) || (cip->length > sizeof(cip->dat)))
        return(INTK_ERR);               /* no appropriate error code
                                           currently defined for INTK_ */

    {

#ifndef OLDSTRTOKEY
        extern int in_tkt_decrypt_again;
        extern int remember_the_function;

        /*
         *  We use this macro below in cases where a decryption has
         *  resulted in what appears to be a bad password error.
         *  Instead of returning the error immediately, it instead
         *  allows the code to iterate through successive decryption
         *  passes (i.e. trying alternative string to key functions)
         *  as long as the decrypt function has requested another
         *  iteration by setting in_tkt_decrypt_again before returning
         *  to us.  In the only current implementation within
         *  krb_get_pw_in_tkt() the iteration requests are actually
         *  made by the password function called by the decrypt
         *  function.
         */
#define DECRYPT_AGAIN(code) { kerror=code; continue; }
#define DECRYPT_NOT_DONE   ((in_tkt_decrypt_again > 0) && (kerror != KSUCCESS))
#else /* OLDSTRTOKEY */
        /*
         *  ... or the original code.
         */
#define DECRYPT_AGAIN(code) return(code)
#define DECRYPT_NOT_DONE    0
#endif /* OLDSTRTOKEY */

        do
        {
            kerror =  KSUCCESS;

            /* copy information from return packet into "cip" */
            bcopy((LPSTR) pkt_cipher(rpkt),(LPSTR)(cip->dat),cip->length); 

            /* Attempt to decrypt the reply. */
            if (decrypt_proc == NULL)
                decrypt_proc = decrypt_tkt;
            (*decrypt_proc)(user, instance, realm, arg, key_proc, &cip);

            ptr = (char *) cip->dat;
            /* extract session key */
            bcopy(ptr, ses, 8);
            ptr += 8;

            if ((strlen(ptr) + (ptr - (char *) cip->dat)) > cip->length || 
                strlen(ptr) >= SNAME_SZ)
            {
                DECRYPT_AGAIN(INTK_BADPW);
                /* return(INTK_BADPW); */
            }
            /* extract server's name */
            (void) strcpy(s_name,ptr);  
            ptr += strlen(s_name) + 1;                        

            if ((strlen(ptr) + (ptr - (char *) cip->dat)) > cip->length || 
                strlen(ptr) >= INST_SZ)
            {
                DECRYPT_AGAIN(INTK_BADPW);
                /* return(INTK_BADPW); */
            }

            /* extract server's instance */
            (void) strcpy(s_instance,ptr);
            ptr += strlen(s_instance) + 1;

            if ((strlen(ptr) + (ptr - (char *) cip->dat)) > cip->length || 
                strlen(ptr) >= REALM_SZ)
            {
                DECRYPT_AGAIN(INTK_BADPW);
                /* return(INTK_BADPW); */
            }

            /* extract server's realm */
            (void) strcpy(rlm,ptr);   
            ptr += strlen(rlm) + 1;

            /* extract ticket lifetime, server key version, ticket length */
            /* be sure to avoid sign extension on lifetime! */
            lifetime = (unsigned char) ptr[0];
            kvno = (unsigned char) ptr[1];
            tkt->length = (unsigned char) ptr[2];
            ptr += 3;

            if ((tkt->length < 0) ||
                ((tkt->length + (ptr - (char *) cip->dat)) > cip->length))
            {
		DECRYPT_AGAIN(INTK_BADPW);
		/* return(INTK_BADPW); */
            }
        } while (DECRYPT_NOT_DONE);
#ifndef OLDSTRTOKEY
        /*
         *  Continue to try alternate decryptions as long as
         *  requested or until the current decryption attempt has
         *  succeeded.
         */
        if (kerror == KSUCCESS)
        {
            /* we want that for changing the password, pbh 8-14-94 */
            remember_the_function = in_tkt_decrypt_again;
            if (krb_debug) kdebug("Remembering %d\n", 
                                  in_tkt_decrypt_again);
        }
        /*
         *  Call the password function back to zap any stashed key
         *  string if we terminate before exhausting the list of
         *  alternate decryptions.  Password functions which support
         *  iteration are required to zap this string themselves when
         *  returning for the last time since we can't distinguish
         *  this case from a password function which doesn't support
         *  iteration at all.
         */
        if (in_tkt_decrypt_again >= 0){
            (*key_proc)((char *)0,(char *)0,(char *)0,arg,(C_Block *)0);
        }
        /*
         *  None of the alternatives succeeded in decrypting the reply.  It
         *  must have really been a bad password after all.
         */
        if (kerror != KSUCCESS)
            return(kerror);
#endif /* !OLDSTRTOKEY */
    }

    /* extract ticket itself */
    bcopy(ptr, (tkt->dat), tkt->length);
    ptr += tkt->length;
    if (strcmp(s_name, service) || strcmp(s_instance, sinstance) ||
        strcmp(rlm, realm))     /* not what we asked for */
        return(INTK_ERR);       /* we need a better code here XXX */
  
    /* check KDC time stamp */
    bcopy(ptr, &kdc_time, 4); /* Time (coarse) */
    if (swap_bytes) swap_u_long(kdc_time);

    ptr += 4;
    (void) gettimeofday(&t_local,(struct timezone *) 0);
    if (krb_debug)
    {
        kdebug("local %ld, server %ld\n", t_local.tv_sec, kdc_time);
        kdebug("local time: %s\nserver time: %s\n",
               ctime(&t_local.tv_sec),
               ctime(&kdc_time));
    }
    if (abs((int)(t_local.tv_sec - kdc_time)) > CLOCK_SKEW) {
        return(RD_AP_TIME);             /* XXX should probably be better
                                           code */
    }
    /* initialize ticket cache */
    if (in_tkt(user, instance, rlm) != KSUCCESS)
        return(INTK_ERR);
    /* stash ticket, session key, etc. for future use */
    if (kerror = save_credentials(s_name, s_instance, rlm, ses,
                                  lifetime, kvno, tkt, t_local.tv_sec))
        return(kerror);
    return(INTK_OK);
}

int
krb_get_in_tkt(
    char *user,
    char *instance,
    char *realm,
    char *service,
    char *sinstance,
    int life,
    int (*key_proc)(),
    int (*decrypt_proc)(),
    char *arg)
{
    int kerror;
    kerror = krb_get_in_tkt_preauth(user, instance, realm,
				  service, sinstance, life,
			   	  key_proc, decrypt_proc, arg, (char *)0, 0);
    return(kerror);
}

