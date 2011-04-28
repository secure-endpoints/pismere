/*
 * key_sched.c
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * This routine computes the DES key schedule given a key.  The
 * permutations and shifts have been done at compile time, resulting
 * in a direct one-step mapping from the input key to the key
 * schedule.
 *
 * Also checks parity and weak keys.
 *
 * Watch out for the subscripts -- most effectively start at 1 instead
 * of at zero.  Maybe some bugs in that area.
 *
 * In case the user wants to cache the computed key schedule, it is
 * passed as an arg.  Also implies that caller has explicit control
 * over zeroing both the key schedule and the key.
 *
 * Originally written 6/85 by Steve Miller, MIT Project Athena.
 */

#include "mit_copy.h"
#ifdef OS2
#include "des_inte.h"
#else
//#include "des_internal.h"
#include "des_inte.h"
#endif
#include <stdio.h>

#include "des.h"

int DES_CALLCONV_C
des_key_sched(k,schedule)
    des_cblock k;
    des_key_schedule schedule;
{
    make_key_sched(k,schedule);

    if (!des_check_key_parity(k))	/* bad parity --> return -1 */
	return(-1);

    /* check against weak keys */
    if (des_is_weak_key(k))
	return(-2);

    /* if key was good, return 0 */
    return 0;
}
