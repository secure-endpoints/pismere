/*
 *	@doc HESIOD
 *
 * @module hespwnam.c |
 *	
 * This file contains hes_getpwnam, for retrieving passwd information about 
 * a user.
 *
 * For copying and distribution information, see the file 
 * <lt> mit-copyright.h <gt>
 *
 * Original version by Steve Dyer, IBM/Project Athena.
 *  
 *	  WSHelper DNS/Hesiod Library for WINSOCK
 *    
 *    
 */

/* This file contains hes_getpwnam, for retrieving passwd information about
 * a user.
 *
 * For copying and distribution information, see the file <mit-copyright.h>
 *
 * Original version by Steve Dyer, IBM/Project Athena.
 *
 */

#include <stdio.h>
#include <string.h> /*s*/

#include <stdlib.h>

#include <windows.h>
#include <decldll.h>

#include "pwd.h"
#include "u-compat.h"

static struct passwd pw_entry;
static char buf[256];


/* 

@func EXPORT32 struct passwd * EXPORT WINAPI | hes_getpwuid |
Given a UID this function will return the pwd information, eg username, uid,
gid, fullname, office location, phone number, home directory, and default shell

@parm  int | uid | The user ID

@rdesc NULL or a pointer to the passwd structure.


*/
EXPORT32 struct passwd *
#ifdef _WINDLL
EXPORT WINAPI
#endif
hes_getpwuid(int uid)
{
    register char *p, **pp; char *_NextPWField( char *),
#ifndef _WINDLL 
    **hes_resolve();
#else
	** WINAPI hes_resolve(char *, char *);
#endif

    char nam[8];
#ifndef _WINDLL
    sprintf(nam, "%d", uid);
#else
    wsprintf(nam, "%d", uid);
#endif  
    pp = hes_resolve(nam, "uid");
    if (pp == NULL || *pp == NULL)
        return(NULL);
    /* choose only the first response (only 1 expected) */
    (void) strcpy(buf, pp[0]);
    p = buf;
    pw_entry.pw_name = p;
    p = _NextPWField(p);
    pw_entry.pw_passwd = p;
    p = _NextPWField(p);
    pw_entry.pw_uid = atoi(p);
    p = _NextPWField(p);
    pw_entry.pw_gid = atoi(p);
#if !defined(_AIX) || (AIXV < 31)
    pw_entry.pw_quota = 0;
#if defined(_AIX) && (AIXV < 31)
    pw_entry.pw_age =
#endif
    pw_entry.pw_comment = "";
#endif
    p = _NextPWField(p);
    pw_entry.pw_gecos = p;
    p = _NextPWField(p);
    pw_entry.pw_dir = p;
    p = _NextPWField(p);
    pw_entry.pw_shell = p;
    while (*p && *p != '\n')
        p++;
    *p = '\0';
    return(&pw_entry);
}


/*

  @func EXPORT32 struct passwd * EXPORT WINAPI | hes_getpwnam |

Given a username this function will return the pwd information, eg
username, uid, gid, fullname, office location, phone number, home
directory, and default shell

  @parm char *| nam | a pointer to the username

  @rdesc NULL or a pointer to the passwd structure.

*/
EXPORT32
struct passwd *
#ifdef _WINDLL
EXPORT WINAPI
#endif
hes_getpwnam(char *nam)
{
    register char *p, **pp; char *_NextPWField( char *), 
#ifndef _WINDLL 
    **hes_resolve();
#else
    ** WINAPI hes_resolve(char *, char *);
#endif

    pp = hes_resolve(nam, "passwd");
    if (pp == NULL || *pp == NULL)
        return(NULL);
    /* choose only the first response (only 1 expected) */
    (void) strcpy(buf, pp[0]);
    p = buf;
    pw_entry.pw_name = p;
    p = _NextPWField(p);
    pw_entry.pw_passwd = p;
    p = _NextPWField(p);
    pw_entry.pw_uid = atoi(p);
    p = _NextPWField(p);
    pw_entry.pw_gid = atoi(p);
#if !defined(_AIX) || (AIXV < 31)
    pw_entry.pw_quota = 0;
#if defined(_AIX) && (AIXV < 31)
    pw_entry.pw_age =
#endif
    pw_entry.pw_comment = "";
#endif
    p = _NextPWField(p);
    pw_entry.pw_gecos = p;
    p = _NextPWField(p);
    pw_entry.pw_dir = p;
    p = _NextPWField(p);
    pw_entry.pw_shell = p;
    while (*p && *p != '\n')
        p++;
    *p = '\0';
    return(&pw_entry);
}


/* Move the pointer forward to the next colon-separated field in the
 * password entry.
 */

static char *
_NextPWField(char *ptr)
{
    while (*ptr && *ptr != '\n' && *ptr != ':')
        ptr++;
    if (*ptr)
        *ptr++ = '\0';
    return(ptr);
}
