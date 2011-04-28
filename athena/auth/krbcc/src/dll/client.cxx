#include "ccapi.h"
#include "util.h"
#include "debug.h"
#include "client.h"
#include "init.hxx"
#include "name.h"
#include "secure.hxx"

CcOsLock gClientLock;

#define SECONDS_TO_WAIT 10

#define STARTUP "CLIENT STARTUP: "
#define DISCONNECT "CLIENT DISCONNECT: "

DWORD
bind_client(
    char* ep OPTIONAL,
    Init::InitInfo& info
    )
{
    LPSTR endpoint = 0;
    DWORD status = 0;
    unsigned char * pszStringBinding = NULL;

    if (!ep) {
        status = alloc_name(&endpoint, "ep", info.isNT);
        CLEANUP_ON_STATUS(status);
    } else {
        endpoint = ep;
    }

    /* Use a convenience function to concatenate the elements of */
    /* the string binding into the proper sequence.              */
    status = RpcStringBindingCompose(0,            // uuid
                                     (unsigned char*)"ncalrpc",    // protseq
                                     0,            // address
                                     (unsigned char*)endpoint,     // endpoint
                                     0,            // options
                                     &pszStringBinding);
    DEBUG_PRINT((STARTUP "RpcStringBindingCompose returned 0x%x (%u)\n", 
                 status, status));
    DEBUG_PRINT((STARTUP "pszStringBinding = %s\n", pszStringBinding));
    CLEANUP_ON_STATUS(status);

    /* Set the binding handle that will be used to bind to the server. */
    status = RpcBindingFromStringBinding(pszStringBinding,
                                         &ccapi_IfHandle);
    DEBUG_PRINT((STARTUP "RpcBindingFromStringBinding returned 0x%x (%u)\n", 
                 status, status));
    CLEANUP_ON_STATUS(status);

    // Win9x might call RpcBindingSetAuthInfo (not Ex), but it does not
    // quite work on Win9x...
    if (info.isNT) {
        RPC_SECURITY_QOS qos;
        qos.Version = RPC_C_SECURITY_QOS_VERSION;
        qos.Capabilities = RPC_C_QOS_CAPABILITIES_DEFAULT;
        qos.IdentityTracking = RPC_C_QOS_IDENTITY_STATIC;
        qos.ImpersonationType = RPC_C_IMP_LEVEL_IDENTIFY;

        status = info.fRpcBindingSetAuthInfoEx(ccapi_IfHandle,
                                               0, // principal
                                               RPC_C_AUTHN_LEVEL_CONNECT,
                                               RPC_C_AUTHN_WINNT,
                                               0, // current address space
                                               RPC_C_AUTHZ_NAME,
                                               &qos);
        DEBUG_PRINT((STARTUP "RpcBindingSetAuthInfo%s returned 0x%x (%u)\n", 
                     info.isNT ? "Ex" : "", status, status));
        CLEANUP_ON_STATUS(status);
    }

 cleanup:
    if (endpoint && (endpoint != ep))
        free_alloc_p(&endpoint);
    if (pszStringBinding) {
        DWORD status = RpcStringFree(&pszStringBinding); 
        DEBUG_PRINT((STARTUP "RpcStringFree returned 0x%x (%u)\n", 
                     status, status));
    }
    DEBUG_PRINT((STARTUP "bind_client() returning %u\n", status));
    return status;
}

DWORD
find_server(
    Init::InitInfo& info
    )
{
    DWORD status = 0;
    LPSTR event_name = 0;
    HANDLE hEvent = 0;
    SECURITY_ATTRIBUTES sa = { 0 };
    PSECURITY_ATTRIBUTES psa = 0;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    char* szExe = 0;
    char* szDir = 0;
    BOOL bRes = FALSE;
#if 0
    HANDLE hToken = 0;
#endif

    psa = info.isNT ? &sa : 0;

    DEBUG_PRINT((STARTUP "Looking for server...\n"));
    status = RpcMgmtIsServerListening(ccapi_IfHandle);
    if (status == RPC_S_NOT_LISTENING)
    {
        DEBUG_PRINT((STARTUP "Server *NOT* found!\n"));
        si.cb = sizeof(si);

        DEBUG_PRINT((STARTUP "Calling alloc_module_dir_name()...\n"));
        status = alloc_module_dir_name(CCAPI_DLL, &szDir);
        CLEANUP_ON_STATUS(status);

        DEBUG_PRINT((STARTUP "Calling alloc_module_dir_name_with_file()...\n"));
        status = alloc_module_dir_name_with_file(CCAPI_DLL, CCAPI_EXE,
                                                 &szExe);
        CLEANUP_ON_STATUS(status);

        DEBUG_PRINT((STARTUP "Calling alloc_name()...\n"));
        status = alloc_name(&event_name, "startup", info.isNT);
        CLEANUP_ON_STATUS(status);

        if (info.isNT) {
            DEBUG_PRINT((STARTUP
                         "Calling alloc_own_security_descriptor_NT()...\n"));
            sa.nLength = sizeof(sa);
            status = alloc_own_security_descriptor_NT(&sa.lpSecurityDescriptor);
            CLEANUP_ON_STATUS(status);
        }

        DEBUG_PRINT((STARTUP "Calling CreateEvent()...\n"));
        hEvent = CreateEvent(psa, FALSE, FALSE, event_name);
        if (!hEvent) {
            status = GetLastError();
            CLEANUP_STATUS(status);
        }

#if 0
        if (SecureClient::IsImp()) {
            DEBUG_PRINT((STARTUP "Token is impersonation token\n"));
            SecureClient::DuplicateImpAsPrimary(hToken);
        } else {
            DEBUG_PRINT((STARTUP "Token is NOT impersonation token\n"));
        }
#endif

        DEBUG_PRINT((STARTUP "Trying to create process...\n"));
#if 0
        if (hToken)
            bRes = CreateProcessAsUser(hToken,
                                       szExe, // app name
                                       NULL, // cmd line
                                       psa, // SA
                                       psa, // SA
                                       FALSE, 
                                       CREATE_NEW_PROCESS_GROUP | 
                                       //CREATE_NEW_CONSOLE |
                                       NORMAL_PRIORITY_CLASS |
                                       // CREATE_NO_WINDOW |
                                       DETACHED_PROCESS |
                                       0
                                       ,
                                       NULL, // environment
                                       szDir, // current dir
                                       &si,
                                       &pi);
        else
#endif
            bRes = CreateProcess(      szExe, // app name
                                       NULL, // cmd line
                                       psa, // SA
                                       psa, // SA
                                       FALSE, 
                                       CREATE_NEW_PROCESS_GROUP | 
                                       //CREATE_NEW_CONSOLE |
                                       NORMAL_PRIORITY_CLASS |
                                       // CREATE_NO_WINDOW |
                                       DETACHED_PROCESS |
                                       0
                                       ,
                                       NULL, // environment
                                       szDir, // current dir
                                       &si,
                                       &pi);
        if (!bRes) {
            CLEANUP_STATUS(GetLastError());
        }
        DEBUG_PRINT((STARTUP "Waiting...\n"));

        status = WaitForSingleObject(hEvent, (SECONDS_TO_WAIT)*1000);
        status = RpcMgmtIsServerListening(ccapi_IfHandle);
        DEBUG_PRINT(((!status) ? (STARTUP "Server FOUND!\n") : 
                     (STARTUP "Server *NOT* found!\n")));
    } else if (status) {
        DEBUG_PRINT((STARTUP "unexpected error while looking for server... "
                     "(%u)\n", status));
    } else {
        DEBUG_PRINT((STARTUP "Server FOUND!\n"));
    }

 cleanup:
#if 0
    if (hToken)
        CloseHandle(hToken);
#endif
    if (szDir)
        free_alloc_p(&szDir);
    if (szExe)
        free_alloc_p(&szExe);
    if (hEvent)
        CloseHandle(hEvent);
    if (pi.hThread)
        CloseHandle(pi.hThread);
    if (pi.hProcess)
        CloseHandle(pi.hProcess);
    if (sa.lpSecurityDescriptor)
        free_alloc_p(&sa.lpSecurityDescriptor);
    DEBUG_PRINT((STARTUP "find_server() returning %u\n", status));
    return status;
}

DWORD
authenticate_server(
    Init::InitInfo& info
    )
{
    DWORD status = 0;
    LPSTR mem_name = 0;
    SECURITY_ATTRIBUTES sa = { 0 };
    PDWORD pvalue = 0;
    HANDLE hMap = 0;
    DWORD challenge = 17; // XXX - maybe use random number
    DWORD desired_response = challenge + 1;
    CC_UINT32 response = 0;

    DEBUG_PRINT((STARTUP "Trying to authenticate RPC Server\n"));

    status = alloc_name(&mem_name, "auth", info.isNT);
    CLEANUP_ON_STATUS(status);

    if (info.isNT) {
        sa.nLength = sizeof(sa);
        status = alloc_own_security_descriptor_NT(&sa.lpSecurityDescriptor);
        CLEANUP_ON_STATUS(status);
    }

    hMap = CreateFileMapping(INVALID_HANDLE_VALUE, info.isNT ? &sa : 0, 
                             PAGE_READWRITE, 0, sizeof(DWORD), mem_name);
    if (!hMap)
        CLEANUP_STATUS(GetLastError());

    pvalue = (PDWORD)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!pvalue)
        CLEANUP_STATUS(GetLastError());

    *pvalue = challenge;

    RpcTryExcept {
        response = Connect((CC_CHAR*)mem_name);
    }
    RpcExcept(1) {
        status = RpcExceptionCode();
        DEBUG_PRINT((STARTUP "Runtime reported exception %u\n", status));
        CLEANUP_STATUS(status);
    }
    RpcEndExcept;

    // Check response
    if ((response != desired_response) && (*pvalue != desired_response)) {
        DEBUG_PRINT((STARTUP "Could not authenticate server.\n"));
        CLEANUP_STATUS(ERROR_ACCESS_DENIED); // XXX - CO_E_NOMATCHINGSIDFOUND?
    } else {
        DEBUG_PRINT((STARTUP "server authenticated!\n"));
    }

 cleanup:
    free_alloc_p(&mem_name);
    free_alloc_p(&sa.lpSecurityDescriptor);
    if (pvalue) {
        BOOL ok = UnmapViewOfFile(pvalue);
        DEBUG_ASSERT(ok);
    }
    if (hMap)
        CloseHandle(hMap);
    return status;
}

DWORD
disconnect_client(
    )
{
    DWORD status = 0;
    if (ccapi_IfHandle) {
        /*  The calls to the remote procedures are complete. */
        /*  Free the binding handle           */
        status = RpcBindingFree(&ccapi_IfHandle);
        DEBUG_PRINT((DISCONNECT "RpcBindingFree returned %u [0x%08x]\n", 
                     status, ccapi_IfHandle));
    }
    return status;
}

DWORD
connect_client(
    char* ep OPTIONAL
    )
{
    DWORD status = 0;
    if (!ccapi_IfHandle)
    {
        Init::InitInfo info;

        status = Init::Info(info);
        CLEANUP_ON_STATUS(status);

        status = bind_client(ep, info);
        CLEANUP_ON_STATUS(status);

        status = find_server(info);
        CLEANUP_ON_STATUS(status);

        status = authenticate_server(info);
        CLEANUP_ON_STATUS(status);
    }
 cleanup:
    if (status)
        disconnect_client();
    return status;
}

DWORD
reconnect_client(
    char* ep OPTIONAL
    )
{
    CcAutoLock AL(gClientLock);
    SecureClient s;
    DWORD status;
    status = disconnect_client();
    CLEANUP_ON_STATUS(status);
    status = connect_client(ep);
 cleanup:
    return status;
}
