/*
 * Copyright 1987 by MIT Student Information Processing Board
 *
 * For copyright info, see mit-sipb-copyright.h.
 */

#ifdef WINDOWS
#include <windows.h>
#endif

#include "internal.h"
#include "errtbl.h"
#include "mitsipbc.h"

#ifndef lint
static const char copyright[] =
    "Copyright 1987,1988 by Student Information Processing Board, Massachusetts Institute of Technology";
static const char rcsid_et_name_c[] =
    "$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/com_err/et_name.c,v 1.1 1999/03/12 23:05:13 dalmeida Exp $";
#endif

static const char char_set[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

static char buf[6];

#ifdef WIN16
//LPSTR COMEXP error_table_name_export(long num)
LPSTR error_table_name_export(long num)
#else
LPSTR error_table_name(long num)
#endif
{
    int ch;
    int i;
    char *p;

    /* num = aa aaa abb bbb bcc ccc cdd ddd d?? ??? ??? */
    p = buf;
    num >>= ERRCODE_RANGE;
    /* num = ?? ??? ??? aaa aaa bbb bbb ccc ccc ddd ddd */
    num &= 077777777L;
    /* num = 00 000 000 aaa aaa bbb bbb ccc ccc ddd ddd */
    for (i = 4; i >= 0; i--) {
	ch = (num >> BITS_PER_CHAR * i) & ((1L << BITS_PER_CHAR) - 1);
	if (ch != 0)
	    *p++ = char_set[ch-1];
    }
    *p = '\0';
    return(buf);
}
