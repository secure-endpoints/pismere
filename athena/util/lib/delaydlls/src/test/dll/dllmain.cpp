#include <windows.h>
#include <stdio.h>
#include <testdll.h>

extern "C"
BOOL
WINAPI
DllMain(
    HINSTANCE hinstDll,
    DWORD fdwReason,
    LPVOID lpvReserved
    )
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        printf("DLL_PROCESS_ATTACH\n");
        return TRUE;
    case DLL_PROCESS_DETACH:
        printf("DLL_PROCESS_DETACH\n");
        return TRUE;
    case DLL_THREAD_ATTACH:
        printf("DLL_THREAD_ATTACH\n");
        return TRUE;
    case DLL_THREAD_DETACH:
        printf("DLL_THREAD_DETACH\n");
        return TRUE;
    default:
        printf("--- !!! ---\n");
        return FALSE;
    }
}

void testdll()
{
    printf("testdll()\n");
}
