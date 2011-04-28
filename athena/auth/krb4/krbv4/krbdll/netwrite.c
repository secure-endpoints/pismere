/*
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit_copy.h>
#include <conf.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>

#ifdef WINSOCK
char *WSErrorString(UINT err);

typedef struct{
    UINT err;
    char *sztext;
} ERRENTRY;

ERRENTRY WSErrors[]=
{
    WSAVERNOTSUPPORTED, "This version of Windows sockets  is not supported",
    WSASYSNOTREADY,     "Winsock support not present or is not responding",
};
#endif /*WINSOCK*/

/*
 * krb_net_write() writes "len" bytes from "buf" to the file
 * descriptor "fd".  It returns the number of bytes written or
 * a write() error.  (The calling interface is identical to
 * write(2).)
 *
 * XXX must not use non-blocking I/O
 */

int
krb_net_write(SOCKET fd, char *buf, int len)
{
    int cc;
    int wrlen = len;
    do {
        cc = send(fd, buf, wrlen, 0);
        if( cc == SOCKET_ERROR ){
            //MessageBox(NULL, WSErrorString(WSAGetLastError()), NULL, MB_OK);
            return(-1);
        }
        if (cc < 0) {
	  return(cc);
	} else {
            buf += cc;
            wrlen -= cc;
        }
    } while (wrlen > 0);
    return(len);
}

#ifdef WINSOCK
char *WSErrorString(UINT err){
    int i;
    static char szerr[80];

    for(i = 0; i < (sizeof(WSErrors) / sizeof(WSErrors[0])); i++ ){
        if( err == WSErrors[i].err ){
            return(WSErrors[i].sztext);
        }
    }
    sprintf(szerr, "Windows Sockets reports error %06d", err );
    return(szerr);
}
#endif /*WINSOCK*/
