/*
 * unix_time.c
 * 
 * Glue code for pasting Kerberos into the Unix environment.
 *
 * Originally written by John Gilmore, Cygnus Support, May '94.
 * Public Domain.
 */

#include "krb.h"
#ifdef PC
#include <time.h>
#include <timeval.h>
#else
#include <sys/time.h>
#endif

/* Time handling.  Translate Unix time calls into Kerberos cnternal 
   procedure calls.  See ../../include/cc-unix.h.  */

unsigned KRB_INT32
unix_time_gmt_unixsec (usecptr)
	unsigned KRB_INT32      *usecptr;
{
	struct timeval  now;

	(void) gettimeofday (&now, (struct timezone *)0);
	if (usecptr)
		*usecptr = now.tv_usec;
	return now.tv_sec;
}
