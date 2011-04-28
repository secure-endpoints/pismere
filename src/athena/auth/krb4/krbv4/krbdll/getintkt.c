/*
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <stdio.h>
#include <krb.h>
#include <prot.h>

#ifndef NULL
#define NULL 0
#endif

#ifndef	KEY_PROC_TYPE_DEFINED
typedef int (*key_proc_type) PROTOTYPE ((char *, char *, char *,
					     char *, C_Block));
#endif
#ifndef	DECRYPT_TKT_TYPE_DEFINED
typedef int (*decrypt_tkt_type) PROTOTYPE ((char *, char *, char *, char *,
				     key_proc_type, KTEXT *));
#endif


/*
 * This file contains two routines: passwd_to_key() converts
 * a password into a DES key (prompting for the password if
 * not supplied), and krb_get_pw_in_tkt() gets an initial ticket for
 * a user.
 */

/*
 * passwd_to_key(): given a password, return a DES key.
 * There are extra arguments here which (used to be?)
 * used by srvtab_to_key().
 *
 * If the "passwd" argument is not null, generate a DES
 * key from it, using string_to_key().
 *
 * If the "passwd" argument is null, call des_read_password()
 * to prompt for a password and then convert it into a DES key.
 *
 * In either case, the resulting key is put in the "key" argument,
 * and 0 is returned.
 */

#if !defined(OLDSTRTOKEY) && !defined(NOENCRYPTION)

#include <stdio.h>
#include <string.h>

/*
 *  Alternative string to key function table
 *
 *  If in_tkt_decrypt_again is > 0 on return from the decrypt procedure in
 *  krb_get_in_tkt(), it will continue to call the decrypt procedure until
 *  either this count decreases to 0 or the decryption produces a meaningful
 *  TGT.  Each time the decrypt procedure is invoked, it calls us back here to
 *  obtain the appropriate decryption key.  We use in_tkt_decrypt_again to
 *  iterate (backwards) through this table to apply each known string to key
 *  function in turn.  
 */
int in_tkt_decrypt_again = 0;
int remember_the_function = 0;
 
extern int string_to_key();
#ifdef AFS
extern int afs_string_to_key();
#endif /* AFS */
extern int krb_debug;

int (*next_string_to_key[])() = {
#ifdef AFS
    afs_string_to_key,
#endif /* AFS */
    string_to_key               /* applied from last to first */
};
#define NS2K    (sizeof(next_string_to_key)/sizeof(next_string_to_key[0]))

#endif /* !OLDSTRTOKEY && !NOENCRYPTION */


/*ARGSUSED */
static int
passwd_to_key(
    char *user,
    char *instance,
    char *realm,
    char *passwd,
    C_Block key
    )
{
#ifdef NOENCRYPTION
    if (!passwd)
        placebo_read_password(key, "Password: ", 0);
#else /* Do encyryption */
  
#ifndef OLDSTRTOKEY
    static char key_string[BUFSIZ];
    static int have_key_string = {0};
    int ok = 0;
  
    /*
     *  We are called this way by krb_get_in_tkt() when the decryption has
     *  succeeded to finally zero the stored password string which we had to
     *  keep hold of in order to apply alternate string to key functions on
     *  each iteration.  This call also resets the iteration count so that
     *  krb_get_in_tkt() is ready for its next use on return.
     */
    if (user == 0 && instance == 0 && realm == 0) {
        bzero(key_string, sizeof (key_string));
        have_key_string = 0;
        in_tkt_decrypt_again = 0;
        return (0);
    }
  
    /*
     *  The iteration count starts off at 0.  The first call here from
     *  krb_get_in_tkt() initializes it to the number of string to key
     *  algorithms defined in the table above.  It decrements to 0 on each
     *  successive call back or is set to 0 on the special call back issued to
     *  clear the stored password when the decryption has succeeded.  In either
     *  case, by the time krb_get_in_tkt() is done with us the count is always
     *  back at 0 so that subsequent calls with password functions that do not
     *  iterate will continue to operate without incident.
     */
    if (in_tkt_decrypt_again <= 0)
        in_tkt_decrypt_again = NS2K;
  
    if (!passwd) {
        passwd = key_string;
        /*
         *  Prompt for the password the first time.
         */
        if (!have_key_string) {
            ok = des_read_pw_string(key_string, sizeof(key_string), 
                                    "Password: ", 0);
            have_key_string = 1;
        }
    }
    if (ok == 0) 
    {
        (*next_string_to_key[--in_tkt_decrypt_again])(passwd, key, realm);
        if (krb_debug)
            kdebug("Trying next_string_to_key[%d]\n",in_tkt_decrypt_again);
    } else
        --in_tkt_decrypt_again;

    /* 
     *  If the principal is known to be in the local realm, don't even bother
     *  to attempt decryptions with alternative string to key algorithms (i.e.
     *  we presumably aren't schizophrenic with respect to our own string to
     *  key function).  This is simply an optimization to avoid extra useless
     *  work when a bad password has been supplied in the common case.
     */
    /* THIS IS WRONG!!!!!
     * if (strcmp(realm, KRB_REALM) == 0)
     *   in_tkt_decrypt_again = 0; 
     */
    /*
     *  If there are no more string to key algorithms to try, zero the password
     *  string now so that krb_get_in_tkt() doesn't have to call us back since
     *  it can't distinguish this case from one when the password function
     *  doesn't support iteration at all.
     */
    if (in_tkt_decrypt_again == 0) {
        bzero(key_string, sizeof (key_string));
        have_key_string = 0;
    }
#else /* OLDSTRTOKEY */
    // back to the original MIT code
    if (passwd)
        string_to_key(passwd, key);
    else {
        des_read_password(key, "Password: ", 0);
    }
#endif /* OLDSTRTOKEY */
  
#endif /* !NOENCRYPTION */
    return (0);
}

/*
 * krb_get_pw_in_tkt() takes the name of the server for which the initial
 * ticket is to be obtained, the name of the principal the ticket is
 * for, the desired lifetime of the ticket, and the user's password.
 * It passes its arguments on to krb_get_in_tkt(), which contacts
 * Kerberos to get the ticket, decrypts it using the password provided,
 * and stores it away for future use.
 *
 * krb_get_pw_in_tkt() passes two additional arguments to krb_get_in_tkt():
 * the name of a routine (passwd_to_key()) to be used to get the
 * password in case the "password" argument is null and NULL for the
 * decryption procedure indicating that krb_get_in_tkt should use the
 * default method of decrypting the response from the KDC.
 *
 * The result of the call to krb_get_in_tkt() is returned.
 */

int
krb_get_pw_in_tkt(
    LPSTR user,
    LPSTR instance,
    LPSTR realm,
    LPSTR service,
    LPSTR sinstance, 
    int life, 
    LPSTR password
    )
{
    /* In spite of the comments above, we don't allow that path here,
       to simplify coding the non-UNIX clients. The only code that now
       depends on this behavior is the preauth support, which has a
       seperate function without this trap. Strictly speaking, this 
       is an API change. 
     
       Imported from krb5 tree for kfw 2.6.
     */

    if (password == 0)
    	return INTK_PW_NULL;

    return(krb_get_in_tkt(user,instance,realm,service,sinstance,life,
                          passwd_to_key, (int(*))NULL, password));
}


/*
 * krb_get_pw_in_tkt_preauth() gets handed the password or key explicitly,
 * since the whole point of "pre" authentication is to prove that we've
 * already got the key, and the only way to do that is to ask the user
 * for it. Clearly we shouldn't ask twice.
 */
 
static C_Block old_key;

static int stub_key(user,instance,realm,passwd,key)
    char *user, *instance, *realm, *passwd;
    C_Block key;
{
    /*
     *  We are called this way by krb_get_in_tkt() when the decryption has
     *  succeeded to finally zero the stored password string which we had to
     *  keep hold of in order to apply alternate string to key functions on
     *  each iteration.  This call also resets the iteration count so that
     *  krb_get_in_tkt() is ready for its next use on return.
     */
    if (user == 0 && instance == 0 && realm == 0) {
        in_tkt_decrypt_again = 0;
        return (0);
    }
  
   if (key)
       (void) memcpy((char *) key, (char *) old_key, sizeof(old_key));
    --in_tkt_decrypt_again;     /* use next string to key */
   return 0;
}

int
krb_get_pw_in_tkt_preauth(user,instance,realm,service,sinstance,life,password)
    char FAR *user, FAR *instance, FAR *realm, FAR *service, FAR *sinstance;
    int life;
    char FAR *password;
{
   char *preauth_p=NULL;
   int   preauth_len=0;
   int   ret_st=0;

    /* On non-Unix systems, we can't handle a null password, because
       passwd_to_key can't handle prompting for the password.  
     
       Imported from krb5 tree for kfw 2.6.
     */
    if (password == 0)
        return INTK_PW_NULL;


   krb_mk_preauth(&preauth_p, &preauth_len, (key_proc_type)passwd_to_key,
		  user, instance, realm, password, old_key);
   ret_st = krb_get_in_tkt_preauth(user,instance,realm,service,sinstance,life,
				   (key_proc_type) stub_key,
				   (decrypt_tkt_type) NULL, password,
				   preauth_p, preauth_len);

   krb_free_preauth(preauth_p, preauth_len);
   return ret_st;
}

#ifdef NOENCRYPTION
/*
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * This routine prints the supplied string to standard
 * output as a prompt, and reads a password string without
 * echoing.
 */

#include <des.h>

#include <stdio.h>
#include <string.h>

/*** Routines ****************************************************** */
int
placebo_read_password(
    des_cblock *k,
    char *prompt,
    int verify
    )
{
    int ok;
    char key_string[BUFSIZ];

    ok = placebo_read_pw_string(key_string, BUFSIZ, prompt, verify);
    if (ok == 0)
	bzero(k, sizeof(C_Block));

    bzero(key_string, sizeof (key_string));
    return ok;
}

/*
 * This version just returns the string, doesn't map to key.
 *
 * Returns 0 on success, non-zero on failure.
 */

int
placebo_read_pw_string(
    char *s,
    int max,
    char *prompt,
    int verify
    )
{
    int ok = 0;
    char *ptr;

    char key_string[BUFSIZ];

    if (max > BUFSIZ) {
	return -1;
    }

    while (!ok) {
	printf(prompt);
	fflush(stdout);
	if (!fgets(s, max, stdin)) {
	    clearerr(stdin);
	    continue;
	}
	if ((ptr = index(s, '\n')))
	    *ptr = '\0';
	if (verify) {
	    printf("\nVerifying, please re-enter %s",prompt);
	    fflush(stdout);
	    if (!fgets(key_string, sizeof(key_string), stdin)) {
		clearerr(stdin);
		continue;
	    }
	    if ((ptr = index(key_string, '\n')))
	    *ptr = '\0';
	    if (strcmp(s,key_string)) {
		printf("\n\07\07Mismatch - try again\n");
		fflush(stdout);
		continue;
	    }
	}
	ok = 1;
    }

    if (verify)
	bzero(key_string, sizeof (key_string));
    s[max-1] = 0;               /* force termination */
    return !ok;                 /* return nonzero if not okay */
}

#endif /* NOENCRYPTION */
