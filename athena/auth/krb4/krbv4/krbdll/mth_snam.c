/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/mth_snam.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef lint
static char *rcsid_month_sname_c =
"$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/mth_snam.c,v 1.1 1999/03/12 23:06:04 dalmeida Exp $";
#endif /* lint */

#include <mit_copy.h>

/*
 * Given an integer 1-12, month_sname() returns a string
 * containing the first three letters of the corresponding
 * month.  Returns 0 if the argument is out of range.
 */

char *month_sname(n)
    int n;
{
    static char *name[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    return((n < 1 || n > 12) ? 0 : name [n-1]);
}
