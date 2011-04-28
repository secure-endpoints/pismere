/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/gettfrlm.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#ifndef lint
static char rcsid_get_tf_realm_c[] =
"$Id: gettfrlm.c,v 1.1 1999/03/12 23:05:44 dalmeida Exp $";
#endif /* lint */

#ifdef WINDOWS
#include <windows.h>
#endif
#include <mit_copy.h>
#include <krb.h>
#include <string.h>

/*
 * This file contains a routine to extract the realm of a kerberos
 * ticket file.
 */

/*
 * krb_get_tf_realm() takes two arguments: the name of a ticket
 * and a variable to store the name of the realm in.
 *
 */

int FAR
krb_get_tf_realm(ticket_file, realm)
  char *ticket_file;
  char *realm;
{
    return(krb_get_tf_fullname(ticket_file, (char*)0,(char*)0, realm));
}
