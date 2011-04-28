// @doc HESIOD
// @module herror.c | The definition of herror is in this module.

	

/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)herror.c	6.1 (Berkeley) 12/4/87";
#endif

#include <windows.h>
#include <winsock.h>

#include <sys/types.h>

char	*h_errlist[] = {
	"Error 0",
	"Unknown host",				/* 1 HOST_NOT_FOUND */
	"Host name lookup failure",		/* 2 TRY_AGAIN */
	"Unknown server error",			/* 3 NO_RECOVERY */
	"No address associated with name",	/* 4 NO_ADDRESS */
};
int	h_nerr = { sizeof(h_errlist)/sizeof(h_errlist[0]) };

/*
extern int	h_errno;
*/

/*
 * @func void | herror |
 *	print the error indicated by the h_errno value in a MessageBox.
 *
 * @parm char * | s | OK so we don't actually use this parameter. This
 * is left over UNIX code where it was used to get the error
 * message. We just make a call to WSAGetLastError under Windows and
 * then play with some offsets.
 *
 */
void herror(char *s)	/* DEFINITELY not Windows material */
{
/*
	struct iovec iov[4];
	register struct iovec *v = iov;

	if (s && *s) {
		v->iov_base = s;
		v->iov_len = strlen(s);
		v++;
		v->iov_base = ": ";
		v->iov_len = 2;
		v++;
	}
	v->iov_base = h_errno < h_nerr ? h_errlist[h_errno] : "Unknown error";
	v->iov_len = strlen(v->iov_base);
	v++;
	v->iov_base = "\n";
	v->iov_len = 1;
	writev(2, iov, (v - iov) + 1);
*/
    MessageBox(NULL, ((WSAGetLastError() - 11000) < h_nerr) ?
               h_errlist[WSAGetLastError() - 11000] :
               "Unknown Error", "WSHelper Resolver Error",
               MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL); 
}
