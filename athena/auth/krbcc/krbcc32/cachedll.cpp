#include "UmCopyRt.h"
#include "creds.h"
#include <crtdbg.h>
#include <cacheapi.h>

apiCB* gControlBlock = NULL;
DWORD gControlBlockCount = 0;
CRITICAL_SECTION gCS;


BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

    switch (fdwReason) 
    {
    case DLL_PROCESS_ATTACH:
	InitializeCriticalSection(&gCS);
	break;
    case DLL_PROCESS_DETACH:
	_ASSERTE(!((bool)gControlBlockCount ^ (bool)gControlBlock));

	while(gControlBlockCount)
	{
	    apiCB* ctx = gControlBlock;
	    cc_shutdown(&ctx);
	}
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	break;
    }
    return(TRUE);
}
