#include <windows.h>
#include "client.h"
#include "util.h"
#include "init.hxx"
#include "secure.hxx"

#define CCAPI_V2_MUTEX_NAME     TEXT("MIT_CCAPI_V2_MUTEX")
HANDLE  hCCAPIv2Mutex = NULL;

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
        hCCAPIv2Mutex = CreateMutex(NULL, FALSE, CCAPI_V2_MUTEX_NAME);
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        // XXX - Process Teardown "Problem"
        //
        // There are two problems that occur during process teardown:
        //
        // 1) Windows (NT/9x/2000) does not keep track of load/unload
        //    ordering dependencies for use in process teardown.
        //
        // 2) The RPC exception handling in the RPC calls do not work
        //    during process shutdown in Win9x.
        //
        // When a process is being torn down in Windows, the krbcc DLL
        // may get a DLL_PROCESS_DETACH before other DLLs are done
        // with it.  Thus, it may disconnect from the RPC server
        // before the last shutdown RPC call.
        //
        // On NT/2000, this is ok because the RPC call will fail and just
        // return an error.
        //
        // On Win9x/Me, the RPC exception will not be caught.
        // However, Win9x ignores exceptions during process shutdown,
        // so the exception will never be seen unless a debugger is
        // attached to the proccess.
        //
        // A good potential woraround would be to have a global
        // variable that denotes whether the DLL is attached to the
        // process.  If it is not, all entrypoints into the DLL should
        // return failure.
        //
        // A not as good workaround is below but ifdefed out.
        //
        // However, we can safely ignore this problem since it can
        // only affects people running debuggers under 9x/Me who are
        // using multiple DLLs that use this DLL.
        //
        WaitForSingleObject( hCCAPIv2Mutex, INFINITE );
#if 0
        bool process_teardown_workaround = false;
        if (lpvReserved) {
            Init::InitInfo info;
            status = Init::Info(info);
            if (status) break;
            if (!info.isNT) process_teardown_workaround = true;
        }
        if (process_teardown_workaround)
            break;
#endif
        // return value is ignored, so we set status for debugging purposes
        status = Client::Cleanup();
        status = Init::Cleanup();
        ReleaseMutex( hCCAPIv2Mutex );
        CloseHandle( hCCAPIv2Mutex );
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return status ? FALSE : TRUE;
}
