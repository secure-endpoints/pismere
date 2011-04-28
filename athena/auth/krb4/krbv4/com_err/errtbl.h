/*
 * Copyright 1988 by the Student Information Processing Board of the
 * Massachusetts Institute of Technology.
 *
 * For copyright info, see mit-sipb-copyright.h.
 */

#ifndef _ET_H
/* Are we using ANSI C? */

/* added by JS and LB 9/4/92 */
#ifdef PC
#include <errno.h>
#include "com_err.h"
#else
extern int errno;
extern char const * const sys_errlist[];
extern const int sys_nerr;
#endif
/* end of insertion */

//typedef LPSTR (*err_func)(int, long);
typedef LPSTR (*err_func)(int, long);


struct error_table {
    char const FAR * const FAR * msgs;   
    err_func func; /* pointer to function taking an integer offset,
		       and a long code,
		       expected to return a string (jms 6/30/93) */
    long base;
    int n_msgs;
};

#ifdef WINDOWS
struct et_list {
    HANDLE next;
    const struct error_table *table;
};
#else
struct et_list {
    struct et_list *next;
    const struct error_table *table;
};
#endif

#ifdef WINDOWS
extern HANDLE _et_list;
#else
extern struct et_list * _et_list;
#endif

#define ERRCODE_RANGE   8       /* # of bits to shift table number */
#define BITS_PER_CHAR   6       /* # bits to shift per character in name */

#ifdef WIN16
//extern LPSTR COMEXP error_table_name_export(long);
extern LPSTR error_table_name_export(long);
extern LPSTR (*error_table_name)(long);
#else
extern LPSTR error_table_name(long);
#endif
#define _ET_H
#endif
