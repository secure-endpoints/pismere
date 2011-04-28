/*
   WSHelper DNS/Hesiod Library for WINSOCK
   wshelper.h
*/

#ifndef _WSHELPER_
#define _WSHELPER_

#include <winsock.h>
#include <decldll.h>
#include <mitwhich.h>

#include <resolv.h>
#include <hesiod.h>

#ifdef __cplusplus
extern "C" {
#endif

EXPORT32 struct hostent FAR* EXPORT WINAPI rgethostbyname(char FAR *name);
EXPORT32 struct hostent FAR* EXPORT WINAPI rgethostbyaddr(char FAR *addr, 
                                                          int len, int type);
EXPORT32 struct servent FAR* EXPORT WINAPI rgetservbyname(LPSTR name, 
                                                          LPSTR proto); 

EXPORT32 LPSTR EXPORT WINAPI gethinfobyname(LPSTR name);
EXPORT32 LPSTR EXPORT WINAPI getmxbyname(LPSTR name);
EXPORT32 LPSTR EXPORT WINAPI getrecordbyname(LPSTR name, int rectype);
EXPORT32 DWORD EXPORT WINAPI rrhost( LPSTR lpHost );

EXPORT32 unsigned long EXPORT WINAPI inet_aton(register const char *cp, 
                                               struct in_addr *addr);

EXPORT32 DWORD EXPORT WhichOS( DWORD *check);

#ifdef _WIN32
EXPORT32 int EXPORT WINAPI wsh_gethostname(char* name, int size);
EXPORT32 int EXPORT WINAPI wsh_getdomainname(char* name, int size);
EXPORT32 LONG FAR EXPORT WSHGetHostID();
#endif

/* some definitions to determine which OS were using and which subsystem */
                              
#if !defined( STACK_UNKNOWN )                              
#define STACK_UNKNOWN    -1
#define MS_NT_32          1
#define MS_NT_16          2
#define MS_95_32          3
#define MS_95_16          4
#define NOVELL_LWP_16     5
#endif /* STACK_UNKNOWN */


#ifdef __cplusplus
}
#endif

#endif  /* _WSHELPER_ */

