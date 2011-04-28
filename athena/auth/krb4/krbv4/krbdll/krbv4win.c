/*
  
  PROGRAM: krbdll.c
  
  PURPOSE: Stub for Kerberos dll library
  
*/

#include "conf.h"
#include <stdlib.h>

#include "lsh_pwd.h"        
#include <winkrbid.h>
#include <windows.h>
#include <winbase.h>               
#include <string.h>
#include <com_err.h>
#include <krberr.h>            
#include <leasherr.h>

#include <krb.h>

#include <winsock.h>
#include <krbintrn.h>

#ifdef MM_CACHE

#include <cacheapi.h>
extern apiCB* g_cc_ctx;
DWORD TlsIndex = 0xFFFFFFFF;

void
FreeThreadData(
    )
{
    THREAD_DATA *ThreadData;
    if (TlsIndex != 0xFFFFFFFF)
    {
        if (ThreadData = TlsGetValue(TlsIndex))
        {
            if (ThreadData->itCreds)
                cc_seq_fetch_creds_end(g_cc_ctx, &ThreadData->itCreds);
            if (ThreadData->g_cache_ptr)
                cc_close(g_cc_ctx, &ThreadData->g_cache_ptr);
            LocalFree(ThreadData);
        }
    }
}
#endif /* MM_CACHE */

static HANDLE hKrbInst;
HINSTANCE hinstWinSock = NULL;
    
void UnloadLeashDll();
    
void WSLoad()
{
    if( hinstWinSock == NULL ){
	hinstWinSock = LoadLibrary("wsock32.dll");
	if( (UINT) hinstWinSock > HINSTANCE_ERROR ){
	    WORD wVersionRequested;
	    WSADATA wsaData;
    
	    wVersionRequested=MAKEWORD(1,1);
	    WSAStartup(wVersionRequested, &wsaData);
	}
    }
}

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
#ifdef INCL_LEASH
    WNDCLASS wndclass;
#endif // INCL_LEASH

    switch (fdwReason) 
    {
    case DLL_PROCESS_ATTACH:
	hKrbInst = hinstDLL;
#ifdef MM_CACHE
	TlsIndex = TlsAlloc();
        if (TlsIndex == 0xFFFFFFFF)
            return FALSE;
	if (cc_initialize(&g_cc_ctx, CC_API_VER_2, NULL, NULL) != CC_NOERROR)
            return FALSE;
#endif // MM_CACHE

#ifdef INCL_LEASH
	/* Register wcom_err for the leash and krbdll err_tables. */
	set_com_err_hook(lsh_com_err_proc);
	init_lsh_err_tbl();
    
	/* Register window class for the MITPasswordControl that   *
	 * replaces normal edit controls for password input.       *
	 * zero any fields we don't explicitly set */
	memset((void *) &wndclass,0x00,sizeof(WNDCLASS));
    
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = MITPwdWinProcDLL;
	wndclass.cbClsExtra = sizeof(HWND);
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hKrbInst;
	wndclass.hbrBackground = COLOR_WINDOW + 1;
	wndclass.lpszClassName = MIT_PWD_DLL_CLASS;
	wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_IBEAM);
	
	RegisterClass(&wndclass);
#endif // INCL_LEASH
    
	WSLoad();
	return TRUE;
    case DLL_PROCESS_DETACH:

#ifdef MM_CACHE
        FreeThreadData();
        TlsFree(TlsIndex);
#endif // MM_CACHE

	if(hinstWinSock != NULL)
        {
	    FreeLibrary(hinstWinSock);
	}
	UnloadLeashDll();  /* from lshcmpat.c */

#ifdef MM_CACHE
	/* unload cache related storage to avoid leaks */
	cc_shutdown(&g_cc_ctx);
#endif /* MM_CACHE */
	return TRUE;
    case DLL_THREAD_ATTACH:
	return TRUE;
    case DLL_THREAD_DETACH:
#ifdef MM_CACHE
        FreeThreadData();
#endif /* MM_CACHE */
	return TRUE;
    default:
	return TRUE;
    }
}
    
// It looks like this is not actually used anywhere
HANDLE FAR PASCAL hGetTimeZone(void)
{
    HANDLE h;
    char *s=getenv("TZ"),*t;
    
    h=GlobalAlloc(GHND,strlen(s)+1);
    t=GlobalLock(h);
    strcpy(t,s);
    GlobalUnlock(h);
    return h;
}

HANDLE FAR PASCAL hQKrbInst(void)
{
    return(hKrbInst);
}
