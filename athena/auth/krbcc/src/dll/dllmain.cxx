#include <windows.h>
#include "client.h"
#include "util.h"
#include "init.hxx"
#include "secure.hxx"

BOOL
WINAPI
DllMain(
    HINSTANCE hinstDLL, // handle to DLL module
    DWORD fdwReason, // reason for calling function
    LPVOID lpvReserved // reserved
    )
{
    DWORD status = 0;
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        SecureClient s;
        status = Init::Attach();
        if (status) break;
        status = connect_client(0);
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        SecureClient s;
        status = disconnect_client();
        Init::Detach();
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return status ? FALSE : TRUE;
}
