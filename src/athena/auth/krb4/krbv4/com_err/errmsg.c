/*
 * $Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/com_err/errmsg.c,v 1.1 1999/03/12 23:05:12 dalmeida Exp $
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/com_err/errmsg.c,v $
 * $Locker:  $
 *
 * Copyright 1987 by the Student Information Processing Board
 * of the Massachusetts Institute of Technology
 *
 * For copyright info, see "mit-sipb-copyright.h".
 */

/* tweaked slightly for MS DOS / MEWEL / MS Windows by jms 1992-1993 */

#include <stdio.h>
#ifndef WINDOWS /* jms 6/30/93 */
#include <string.h>
#else
  #include <windows.h>
  #define strcpy lstrcpy
  #define strlen lstrlen
  #define strcat lstrcat
#endif

/* Support for ECONNREFUSED and friends, TCP/IP-specific error codes */
//#ifdef LWP
//  #include <sys/socket.h>
//#endif /* LWP */
#ifdef WINSOCK 
#include <winsock.h>
#include <errno.h>

/* These defines are here because in Windows NT, winsock.h has these
   commented out "to avoid conflicts with errno.h". */
#ifndef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#endif
#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET 
#endif
#endif

#include "internal.h"
#include "errtbl.h"
#include "mitsipbc.h"

#undef _et_list

static char buffer[25];

#ifdef WINDOWS
HANDLE _et_list = NULL;
HANDLE *__et_list = &_et_list;
#else
struct et_list * _et_list = (struct et_list *) NULL;
struct et_list **__et_list = &_et_list;
#endif

#ifdef WIN16
//LPSTR COMEXP error_message_export (long code)
LPSTR error_message_export (long code)
#else
LPSTR error_message (long code)
#endif
{
    int offset; /* jms 6/30/93 */
#ifdef WINDOWS
    HANDLE het,hetnext;
#endif
    struct et_list *et;
    long table_num;
    int started = 0;
    LPSTR cp,rv;

    offset = code & ((1L<<ERRCODE_RANGE)-1);
    table_num = code - offset;
    if (!table_num) {
#ifdef OS2
      return strerror(offset);
    }
#else
	if (offset < sys_nerr)
	    return(sys_errlist[offset]);
	else
	  switch(offset)
	    {
#if defined(LWP) || defined(WINSOCK)
	      case ECONNREFUSED:
		return "Connection refused";
	      case ETIMEDOUT:
		return "Connection timed out";
	      case ECONNRESET:
		return "Connection reset by peer";
#endif
	      default:
		goto oops;
	    }
      }
#endif /* OS2 */
#ifdef WINDOWS
    het=_et_list;
    while (het) {
	et=GlobalLock(het);
	if (!et) 
	    break;
	if (et->table->base == table_num) {
	    /* This is the right table */
	    if (et->table->n_msgs <= offset) {
		GlobalUnlock(het);
		goto oops;
	    }
/* jms 6/30/93 -- radical new change :o] */
	    if (et->table->msgs == NULL)
		{
			if (et->table->func != NULL) {
			  rv=(*(et->table->func))(offset, code);
			  GlobalUnlock(het);
			  return rv;
			}
			GlobalUnlock(het);
			break; /* right table, nothing there. */
		}
	    else {
		rv=((LPSTR)et->table->msgs[offset]);
		GlobalUnlock(het);
		return rv;
	    }
	}
	hetnext=et->next;
	GlobalUnlock(het);
	het=hetnext;
    }
#else
    for (et = _et_list; et; et = et->next) {
	if (et->table->base == table_num) {
	    /* This is the right table */
	    if (et->table->n_msgs <= offset)
		goto oops;
/* jms 6/30/93 -- radical new change :o] */
	    if (et->table->msgs == NULL)
		{
			if (et->table->func != NULL)
			  return (*(et->table->func))(offset, code);
			break; /* right table, nothing there. */
		}
	    else
	    return((LPSTR)et->table->msgs[offset]);
/*                                       */
	}
    }
#endif
oops:
    strcpy (buffer, "Unknown code ");
    if (table_num) {
	strcat (buffer, error_table_name (table_num));
	strcat (buffer, " ");
    }
    for (cp = buffer; *cp; cp++)
	;
    if (offset >= 100) {
	*cp++ = '0' + offset / 100;
	offset %= 100L;
	started++;
    }
    if (started || offset >= 10) {
	*cp++ = '0' + offset / 10;
	offset %= 10L;
    }
    *cp++ = '0' + offset;
    *cp = '\0';
    return(buffer);
}
