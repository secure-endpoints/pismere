/*
 * internal include file for com_err package
 */
#include "mitsipbc.h"
#ifndef __STDC__
#undef const
#define const
#endif

/* added by JMS and LB 9/4/92 */
#ifdef PC

#include <errno.h>
#include <stdlib.h>
 #if (_MSC_VER >= 700) /* C7 needs this. grr. how idiotic. */
 #define sys_nerr _sys_nerr
 #define sys_errlist _sys_errlist
 #endif

#ifndef WINDOWS /* jms 6/30/93 */
#define FAR
#define LPSTR char *
#endif


#else

extern int errno;
//extern char const * const sys_errlist[];
//extern const int sys_nerr;
#endif
/* end of insertion */

/* AIX and Ultrix (and MSDOS) have standard conforming header files. */
#if !defined(ULTRIX_40) && !defined(_AIX) && !defined(PC)
#ifdef __STDC__
void perror (const char *);
#else
//int perror ();
#endif
#endif
