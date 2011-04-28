/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/stime.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef lint
static char *rcsid_stime_c =
"$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/stime.c,v 1.1 1999/03/12 23:06:14 dalmeida Exp $";
#endif /* lint */

#include <mit_copy.h>
#include <time.h>
#include <stdio.h>                      /* for sprintf() */

char *month_sname();

/*
 * Given a pointer to a long containing the number of seconds
 * since the beginning of time (midnight 1 Jan 1970 GMT), return
 * a string containing the local time in the form:
 *
 * "25-Jan-88 10:17:56"
 */

char *stime(t)
    long *t;
{
    static char st_data[40];
    static char *st = st_data;
    struct tm *tm;

    tm = localtime(t);
    (void) sprintf(st,"%2d-%s-%02d %02d:%02d:%02d",tm->tm_mday,
                   month_sname(tm->tm_mon + 1),tm->tm_year,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
    return st;
}
