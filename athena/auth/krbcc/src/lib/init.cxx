#include <windows.h>
#include "init.hxx"
#include "debug.h"
#include "secure.hxx"

CcOsLock Init::s_lock;
DWORD Init::s_refcount = 0;
DWORD Init::s_error = ERROR_INVALID_HANDLE;
bool Init::s_init = false;
Init::InitInfo Init::s_info = { 0 };
HINSTANCE Init::s_hRpcDll = 0;

#define INIT "INIT: "

static
void
ShowInfo(
    Init::InitInfo& info
    );

DWORD
Init::Info(
    InitInfo& info
    )
{
    // This funciton will not do automatic initialization.
    CcAutoLock AL(s_lock);
    if (!s_init) {
        memset(&info, 0, sizeof(info));
        return s_error ? s_error : ERROR_INVALID_HANDLE;
    } else {
        info = s_info;
        return 0;
    }
}

DWORD
Init::Attach(
    )
{
    CcAutoLock AL(s_lock);
    if (s_init) {
        s_refcount++;
        return 0;
    }
    DWORD status = 0;
    OSVERSIONINFO osvi;
    BOOL isSupportedVersion = FALSE;
    memset(&s_info, 0, sizeof(s_info));
    memset(&osvi, 0, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (!GetVersionEx(&osvi))
        CLEANUP_STATUS(GetLastError());

    switch(osvi.dwPlatformId) {
    case VER_PLATFORM_WIN32_WINDOWS:
        s_info.isNT = FALSE;
        isSupportedVersion = TRUE;
        break;
    case VER_PLATFORM_WIN32_NT:
        s_info.isNT = TRUE;
        isSupportedVersion = TRUE;
        break;
    case VER_PLATFORM_WIN32s:
    default:
        s_info.isNT = FALSE;
        break;
    }

    if (!isSupportedVersion) {
        DEBUG_PRINT((INIT "Trying to run on an unsupported version of "
                     "Windows\n"));
        CLEANUP_STATUS(ERROR_NOT_SUPPORTED);
    }

    if (!s_info.isNT)
        goto cleanup;

    s_hRpcDll = LoadLibrary(TEXT("rpcrt4.dll"));
    if (!s_hRpcDll)
        CLEANUP_STATUS(GetLastError());

    s_info.fRpcBindingSetAuthInfoEx = (FP_RpcBindingSetAuthInfoEx)
        GetProcAddress(s_hRpcDll, TEXT(FN_RpcBindingSetAuthInfoEx));
    if (!s_info.fRpcBindingSetAuthInfoEx) {
        DEBUG_PRINT((INIT "Running on NT but could not find "
                     "RpcBindinSetAuthInfoEx\n"));
        CLEANUP_STATUS(GetLastError());
    }
    
    s_info.fRpcServerRegisterIfEx = (FP_RpcServerRegisterIfEx)
        GetProcAddress(s_hRpcDll, TEXT(FN_RpcServerRegisterIfEx));
    if (!s_info.fRpcServerRegisterIfEx) {
        DEBUG_PRINT((INIT "Running on NT but could not find "
                     "RpcServerRegisterIfEx\n"));
        CLEANUP_STATUS(GetLastError());
    }

    status = SecureClient::Attach();
    if (status) {
        DEBUG_PRINT((INIT "SecureClient::Attach() failed (%u)\n", status));
    }

 cleanup:
    if (status) {
        memset(&s_info, 0, sizeof(s_info));
        if (s_hRpcDll) {
            FreeLibrary(s_hRpcDll);
            s_hRpcDll = 0;
        }
        DEBUG_PRINT((INIT "Init::Attach() failed (%u)\n", status));
    } else {
        s_refcount++;
        s_init = true;
        ShowInfo(s_info);
    }
    s_error = status;
    return status;
}

DWORD
Init::Detach(
    )
{
    CcAutoLock AL(s_lock);
    s_refcount--;
    if (s_refcount) return 0;
    if (!s_init) return 0;
    DWORD error = 0;
    if (s_hRpcDll) {
        FreeLibrary(s_hRpcDll);
        s_hRpcDll = 0;
    }
    error = SecureClient::Detach();
    memset(&s_info, 0, sizeof(s_info));
    s_init = false;
    s_error = 0;
    DEBUG_PRINT((INIT "Init::Detach() had an error (%u)\n", error));
    return error;
}

static
void
ShowInfo(
    Init::InitInfo& info
    )
{
    if (info.isNT) {
        DEBUG_PRINT((INIT "Running on Windows NT using secure mode\n"));
    } else {
        DEBUG_PRINT((INIT "Running insecurely on non-NT Windows\n"));
    }
    return;
}
