#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "wsh-int.h"

BOOL
WINAPI
DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved   // reserved
)
{
    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        res_init_startup();
        break;
    case DLL_PROCESS_DETACH:
        res_init_cleanup();
        break;
    }
    return TRUE;
}
