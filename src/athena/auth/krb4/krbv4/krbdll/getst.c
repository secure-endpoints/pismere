/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/getst.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef lint
static char rcsid_getst_c[] =
"$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/getst.c,v 1.1 1999/03/12 23:05:43 dalmeida Exp $";
#endif /* lint */

#include <conf.h>

#include <mit_copy.h>
#include <io.h>

/*
 * getst() takes a file descriptor, a string and a count.  It reads
 * from the file until either it has read "count" characters, or until
 * it reads a null byte.  When finished, what has been read exists in
 * the given string "s".  If "count" characters were actually read, the
 * last is changed to a null, so the returned string is always null-
 * terminated.  getst() returns the number of characters read, including
 * the null terminator.
 */

getst(fd, s, n)
    int fd;
    register char *s;
{
    register count = n;
    while (read(fd, s, 1) > 0 && --count)
        if (*s++ == '\0')
            return (n - count);
    *s = '\0';
    return (n - count);
}
