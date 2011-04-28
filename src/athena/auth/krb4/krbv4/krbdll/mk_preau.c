/* mk_preauth.c */
/* part of Cygnus Network Security */
/* Copyright 1994 Cygnus Support */
/*
 * Permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Cygnus Support makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 */


#ifdef WINDOWS
#include <windows.h>
#endif

#include <krb.h>
#include <string.h>
#include <stdlib.h>
#ifndef	KEY_PROC_TYPE_DEFINED
typedef int (*key_proc_type) PROTOTYPE ((char *, char *, char *,
					     char *, C_Block));
#endif
#ifndef	DECRYPT_TKT_TYPE_DEFINED
typedef int (*decrypt_tkt_type) PROTOTYPE ((char *, char *, char *, char *,
				     key_proc_type, KTEXT *));
#endif



int
krb_mk_preauth(preauth_p, preauth_len,
	       key_proc,aname,inst,realm,password,key)
    char **preauth_p;
    int  *preauth_len;
    key_proc_type key_proc;
    char *aname;
    char *inst;
    char *realm;
    char *password;
    C_Block key;
{
#ifdef NOENCRYPTION
    *preauth_len = strlen(aname)+1;	/* include the trailing 0 */
    *preauth_p = malloc(*preauth_len);
    strcpy(*preauth_p, aname);		/* this will copy the trailing 0 */
#else
    des_key_schedule key_s;
    int sl = strlen(aname);
#endif

    (*key_proc)(aname,inst,realm,password,key);

#ifndef NOENCRYPTION
    /* 
     * preauth_len is set to a length greater than sl + 1 
     * and a multpile of 8
     */
    *preauth_len = (((sl + 1) / 8) + 1) * 8;
    /* allocate memory for preauth_p and fill it with 0 */
    *preauth_p = (char*) malloc(*preauth_len);
    if ( *preauth_p ) {
        *preauth_len = 0;
        return(1);
    }

    /* create the key schedule */
    if (des_key_sched(key, key_s)) {
      return 1;
    }
    
    /*
     * encrypt aname using key_s as the key schedule and key as the
     * initialization vector.
     */
    des_pcbc_encrypt((des_cblock *) aname, (des_cblock *) *preauth_p,
		     (long) (sl + 1), key_s, (des_cblock *) key, DES_ENCRYPT);
#endif
    return 0;
}

void
krb_free_preauth(preauth_p, preauth_len)
     char *preauth_p;
     int preauth_len;
{
  free(preauth_p);
  return;
}
