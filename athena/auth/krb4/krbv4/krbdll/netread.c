/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/netread.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <conf.h>

#include <mit_copy.h>
#ifdef WINDOWS
#include <windows.h>
#endif
#ifdef WINSOCK
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

/*
 * krb_net_read() reads from the file descriptor "fd" to the buffer
 * "buf", until either 1) "len" bytes have been read or 2) cannot
 * read anymore from "fd".  It returns the number of bytes read
 * or a read() error.  (The calling interface is identical to
 * read(2).)
 *
 * XXX must not use non-blocking I/O
 */

int
krb_net_read(fd, buf, len)
int fd;
register char *buf;
register int len;
{
    int cc;
    u_long onOff;
    int ret = 0;
    int len2 = 0;

    do {
#ifdef PC
    #if defined(BSDSOCK)
	/* make the socket non-blocking */
	onOff = 0;
	ret = ioctlsocket(fd,FIONBIO,(u_long FAR*)&onOff);
	if (ret) {
	    if (WSAIsBlocking()) {
	       WSACancelBlockingCall();
	    }
	    if (closesocket(fd) == SOCKET_ERROR) {
		 ret = WSAGetLastError();
	    }
	    return(ret);
	}

        cc = recv(fd, buf, len, 0);
    #else
        cc = soread(fd,buf,len);
    #endif /*WINSOCK*/
#else
        cc = read(fd, buf, len);
#endif
        if (cc < 0)
            return(cc);          /* errno is already set */
        else if (cc == 0) {
            return(len2);
        } else {
            buf += cc;
            len2 += cc;
            len -= cc;
        }
    } while (len > 0);
    return(len2);
}
