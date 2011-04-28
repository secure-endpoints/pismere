/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/one.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef	lint
static char rcsid_one_c[] =
"$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/one.c,v 1.1 1999/03/12 23:06:06 dalmeida Exp $";
#endif	lint

#include <mit_copy.h>

/*
 * definition of variable set to 1.
 * used in krb_conf.h to determine host byte order.
 */

int krbONE = 1;
