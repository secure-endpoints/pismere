/*
 * string_to_key.c
 *
 * Copyright 1985, 1986, 1987, 1988, 1989 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file <mit-copyright.h>.
 *
 * These routines perform encryption and decryption using the DES  
 * private key algorithm, or else a subset of it-- fewer inner loops.
 * (AUTH_DES_ITER defaults to 16, may be less.)  
 *
 * Under U.S. law, this software may not be exported outside the US  
 * without license from the U.S. Commerce department.
 *
 * The key schedule is passed as an arg, as well as the cleartext or  
 * ciphertext.  The cleartext and ciphertext should be in host order.
 *
 * These routines form the library interface to the DES facilities.
 *
 *      spm     8/85    MIT project athena  */

#include "mit_copy.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "des.h"
#ifdef OS2
#include "des_inte.h"
#else
//#include "des_internal.h"
#include "des_inte.h"
#endif

#ifdef DEBUG
extern int des_debug;
extern int des_debug_print();
#endif

#ifdef AFS

/* IFS version */

static char des_the_realm[256] = { NULL };  /* IFS version special global */



/* Permits external functions to access special global.
*/
void set_the_realm(char *realm)
{
  strcpy(des_the_realm, realm);
}


#endif /*AFS */

/*
 * convert an arbitrary length string to a DES key  */

int DES_CALLCONV_C
des_string_to_key(str,key)
    char *str;
    des_cblock key;
{
    char *in_str;
    unsigned temp,i;
    int j;
    long length;
    unsigned char *k_p;
    int forward;
    char *p_char;
    char k_char[64];
    des_key_schedule key_sked;

    in_str = str;
    forward = 1;
    p_char = k_char;
    length = strlen(str);

    /* init key array for bits */
    memset(k_char, 0,sizeof(k_char));

#ifdef DEBUG
    if (des_debug)
	fprintf(stdout,
		"\n\ninput str length = %d  string = %s\nstring = 0x ",
		length,str);
#endif

    /* get next 8 bytes, strip parity, xor */
    for (i = 1; i <= length; i++) {
	/* get next input key byte */
	temp = (unsigned int) *str++;
#ifdef DEBUG
	if (des_debug)
	    fprintf(stdout,"%02x ",temp & 0xff);
#endif
	/* loop through bits within byte, ignore parity */
	for (j = 0; j <= 6; j++) {
	    if (forward)
		*p_char++ ^= (int) temp & 01;
	    else
		*--p_char ^= (int) temp & 01;
	    temp = temp >> 1;
	} while (--j > 0);

	/* check and flip direction */
	if ((i%8) == 0)
	    forward = !forward;
    }

    /* now stuff into the key des_cblock, and force odd parity */
    p_char = k_char;
    k_p = (unsigned char *) key;

    for (i = 0; i <= 7; i++) {
	temp = 0;
	for (j = 0; j <= 6; j++)
	    temp |= *p_char++ << (1+j);
	*k_p++ = (unsigned char) temp;
    }

    /* fix key parity */
    des_fixup_key_parity(key);

    /* Now one-way encrypt it with the folded key */
    (void) des_key_sched(key,key_sked);
    (void) des_cbc_cksum((des_cblock *)in_str,key,length,key_sked,key);
    /* erase key_sked */
    memset((char *)key_sked, 0,sizeof(key_sked));

    /* now fix up key parity again */
    des_fixup_key_parity(key);

#ifdef DEBUG
    if (des_debug)
	fprintf(stdout,
		"\nResulting string_to_key = 0x%x 0x%x\n",
		*((unsigned long *) key) & 0xffffffff,
		*((unsigned long *) key+1) & 0xffffffff); 
#endif

    return 0;
}

#ifdef	AFS

extern char *crypt( char *, char *);

#endif /* AFS */

#ifdef AFS
#include <ctype.h>

int
afs_string_to_key(char *str, des_cblock key, char *realm) {
    char password[BUFSIZ];
    long length;
    long passlen;
    int i;
    char *crypt_return;

    memset( key, 0, sizeof(des_cblock));
    length = strlen(str);
    if( length <=8 ){

		memset( (char *)password, 0, sizeof(password));
		strncpy(password, realm, 8);
		for (i=0; i<8; i++)
		   if (isupper(password[i]))
			password[i] = tolower(password[i]);
		for (i=0; i<length; i++)
		    password[i] ^= str[i];
		for (i=0; i<8; i++){
		    if (password[i] == '\0'){
			password[i] = 'X';
		    }
		}

	    strncpy(key, (crypt_return = (crypt(password, "#~"))) + 2, sizeof(des_cblock));

	/* original code
		crypt_return = crypt(password, "#~");
		(void)strncpy(key, (crypt_return + 2), 8);
	*/
		for (i=0; i<8; i++)
		    key[i] <<= 1;
		/* now fix up key parity again */
		des_fixup_key_parity(key);
    } else {
		char ikey[8];
		char tkey[8];
		static des_key_schedule key_sked;

		strncpy (password, str, sizeof(password));
		for (i=length; i<sizeof(password) && *realm; i++) {
		    password[i] = *realm++;
		    if (isupper(password[i]))
			password[i] = tolower(password[i]);
		}

		memcpy( ikey, "kerberos", sizeof(ikey));
		memcpy( tkey, ikey, sizeof(tkey));
		des_fixup_key_parity (tkey);
		(void) des_key_sched (tkey, key_sked);
		(void) des_cbc_cksum (password, tkey, i, key_sked, ikey);

		memcpy( ikey, tkey, sizeof(ikey));
		des_fixup_key_parity (tkey);
		(void) des_key_sched (tkey, key_sked);
		(void) des_cbc_cksum (password, key, i, key_sked, ikey);

		/* erase key_sked */
		memset( (char *)key_sked, 0, sizeof(key_sked));

		/* now fix up key parity again */
		des_fixup_key_parity(key);
    }
    return(0);
}

#endif /* AFS */
 
