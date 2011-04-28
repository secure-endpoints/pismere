/* 
 * $Id: aklog.h,v 1.2 2001/01/12 21:52:05 darkwing Exp $
 *
 * Copyright 1990,1991 by the Massachusetts Institute of Technology
 * For distribution and copying rights, see the file "mit-copyright.h"
 */

#ifndef __AKLOG_H__
#define __AKLOG_H__

#if !defined(lint) && !defined(SABER)
static char *rcsid_aklog_h = "$Id: aklog.h,v 1.2 2001/01/12 21:52:05 darkwing Exp $";
#endif /* lint || SABER */

#ifndef WIN32
#include <afs/param.h>
#endif

#if !defined(vax)
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <limits.h>
#endif

#ifndef WIN32
#include <sys/types.h>
#endif
#include <krb.h>
#include "linked_list.h"

#ifdef __STDC__
#define ARGS(x) x
#else
#define ARGS(x) ()
#endif /* __STDC__ */

#include <afscompat.h>

typedef struct {
    int (*readlink)ARGS((char *, char *, int));
    int (*isdir)ARGS((char *, unsigned char *));
    char *(*getcwd)ARGS((char *, size_t));
    int (*get_cred)ARGS((char *, char *, char *, CREDENTIALS *));
    int (*get_user_realm)ARGS((char *));
    void (*pstderr)ARGS((char *));
    void (*pstdout)ARGS((char *));
    void (*exitprog)ARGS((char));
} aklog_params;

void aklog ARGS((int, char *[], aklog_params *));
void aklog_init_params ARGS((aklog_params *));

#endif /* __AKLOG_H__ */
