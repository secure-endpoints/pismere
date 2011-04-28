/*
 *	@doc HESIOD
 *
 * @module hesmailh.c |
 *
 * This file contains hes_postoffice, which retrieves post-office information
 * for a user.
 *
 *  For copying and distribution information, see the file 
 *  <lt> mit-copyright.h <gt>
 *
 *  Original version by Steve Dyer, IBM/Project Athena.
 *
 *	WSHelper DNS/Hesiod Library for WINSOCK
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h> /*s*/

#include <hesiod.h>
#include "u-compat.h"

#define LINESIZE 80


/*

@func struct hes_postoffice FAR * WINAPI | hes_getmailhost | This will
a pointer to a hes_postoffice structure. This call is used to obtain a
user's type of mail account and the location of that account. E.g. POP
PO10.MIT.EDU or IMAP IMAP-TEST.MIT.EDU

 @parm LPSTR | user | The username to be used when querying for the
 Hesiod Name Type POBOX.

@rdesc NULL if there was an error or if there was no entry for the
username. Otherwise a pointer to a hes_postoffice structure is
returned.

*/
struct hes_postoffice FAR *
#if defined (_WINDLL) || defined (_WIN32)
WINAPI
#endif
hes_getmailhost(LPSTR user)
{
    static struct hes_postoffice ret;
    static char linebuf[LINESIZE];
    char *p;
    char **cp;

    cp = hes_resolve(user, "pobox");
    if (cp == NULL) return(NULL);
    strcpy(linebuf, *cp);
    ret.po_type = linebuf;
    p = linebuf;
    while(!isspace(*p)) p++;
    *p++ = '\0';
    ret.po_host = p;
    while(!isspace(*p)) p++;
    *p++ = '\0';
    ret.po_name = p;
    return(&ret);
}
