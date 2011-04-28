#ifndef __DLL_CLIENT_H__
#define __DLL_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

DWORD
disconnect_client(
    );

DWORD
connect_client(
    char* ep OPTIONAL
    );

DWORD
reconnect_client(
    char* ep OPTIONAL
    );

#ifdef __cplusplus
#include "autolock.hxx"
extern CcOsLock gClientLock;
#endif

#if 0
{
#endif
#ifdef __cplusplus
}
#endif

#endif
