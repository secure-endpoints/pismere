/*
 * random_key.c
 *
 * Copyright 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * Under U.S. law, this software may have export restrictions.
 * Check with a lawyer.
 *
 * create a random des key; should force odd parity per byte;
 * parity is bits 8,16,...64 in des order, implies 0, 8, 16, ...
 * vax order
 *
 * spm	8/85	MIT project athena
 */

#include "mit_copy.h"
#include <stdio.h>

#include "des.h"
#ifdef OS2
#include "des_inte.h"
#else
//#include "des_internal.h"
#include "des_inte.h"
#endif

int
des_random_key(key)
    des_cblock *key;
{
    /*
     * create a random des key; should force odd parity per byte;
     * parity is bits 8,16,...64 in des order, implies 0, 8, 16, ...
     * vax order
     */

    register unsigned int temp;
    register int odd; 
     static long n;
    int i,j; 
    unsigned KRB_INT32 buf;
    register unsigned char *c = (unsigned char *) key;
    unsigned KRB_INT32 *k = (unsigned KRB_INT32 *) key;
 
   
    unsigned KRB_INT32 time_secs, time_usecs;

    time_secs = TIME_GMT_UNIXSEC_US (&time_usecs);
    /* randomize start */
    srandom((long) (time_usecs ^ time_secs ^
		    RANDOM_KRB_INT32_1 ^ RANDOM_KRB_INT32_2 ^ n++) );

    buf = random();
    memcpy(k, &buf, sizeof buf);
    ++k;
    buf = random();
    memcpy(k, &buf, sizeof buf);

    /* make each byte parity odd */
    for (i = 0; i <= 7; i++) {
	odd = 0;
	temp = (unsigned int) *c;
	/* ignore bit 0, lsb,  it will be parity (on vax) */
	/* should do this with a table lookup */
	for (j = 0; j <= 6; j++) {
	    temp = temp >> 1;
	    odd ^= temp & 01;
	}
	/* set odd parity in lsb */
	if (!odd)
	    *c |= 1;
	else
	    *c &= ~1;
	c++;
    }

    return 0;
}
