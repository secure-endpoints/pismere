#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ccapi.h"
#include "util.h"
#include "debug.h"
#include "opts.hxx"
#include "testapi.h"
#include "init.hxx"
#include "name.h"

#define STARTUP "SERVER STARTUP: "

RPC_STATUS
GetPeerName(
    RPC_BINDING_HANDLE hClient,
    LPTSTR pszClientName,
    int iMaxLen
    )
{
    RPC_STATUS Status		= RPC_S_OK;
    RPC_BINDING_HANDLE hServer	= NULL;
    PTBYTE pszStringBinding	= NULL;
    PTBYTE pszClientNetAddr	= NULL;
    PTBYTE pszProtSequence	= NULL;

    memset(pszClientName, 0, iMaxLen * sizeof(TCHAR));

    __try {
        // Create a partially bound server handle from the client handle.
        Status = RpcBindingServerFromClient (hClient, &hServer);
        if (Status != RPC_S_OK) __leave;

        // Get the partially bound server string binding and parse it.
        Status = RpcBindingToStringBinding (hServer,
                                            &pszStringBinding);
        if (Status != RPC_S_OK) __leave;

        // String binding only contains protocol sequence and client
        // address, and is not currently implemented for named pipes.
        Status = RpcStringBindingParse (pszStringBinding, NULL,
                                        &pszProtSequence, &pszClientNetAddr, 
                                        NULL, NULL);
        if (Status != RPC_S_OK)
            __leave;
        int iLen = lstrlen(pszClientName) + 1;
        if (iMaxLen < iLen)
            Status = RPC_S_BUFFER_TOO_SMALL;
        lstrcpyn(pszClientName, (LPCTSTR)pszClientNetAddr, iMaxLen);
    }
    __finally {
        if (pszProtSequence)
            RpcStringFree (&pszProtSequence);
        
        if (pszClientNetAddr)
            RpcStringFree (&pszClientNetAddr);
        
        if (pszStringBinding)
            RpcStringFree (&pszStringBinding);
        
        if (hServer)
            RpcBindingFree (&hServer);
    }
    return Status;
}

struct client_auth_info {
    RPC_AUTHZ_HANDLE authz_handle;
    unsigned char* server_principal; // need to RpcFreeString this
    ULONG authn_level;
    ULONG authn_svc;
    ULONG authz_svc;
};

RPC_STATUS
GetClientId(
    RPC_BINDING_HANDLE hClient,
    char* client_id,
    int max_len,
    client_auth_info* info
    )
{
    RPC_AUTHZ_HANDLE authz_handle = 0;
    unsigned char* server_principal = 0;
    ULONG authn_level = 0;
    ULONG authn_svc = 0;
    ULONG authz_svc = 0;
    RPC_STATUS status = 0;

    memset(client_id, 0, max_len);

    if (info) {
        memset(info, 0, sizeof(client_auth_info));
    }

    status = RpcBindingInqAuthClient(hClient, &authz_handle, 
                                     info ? &server_principal : 0, 
                                     &authn_level, &authn_svc, &authz_svc);
    if (status == RPC_S_OK)
    {
        if (info) {
            info->server_principal = server_principal;
            info->authz_handle = authz_handle;
            info->authn_level = authn_level;
            info->authn_svc = authn_svc;
            info->authz_svc = authz_svc;
        }

        if (authn_svc == RPC_C_AUTHN_WINNT) {
            WCHAR* username = (WCHAR*)authz_handle;
            int len = lstrlenW(username) + 1;
            if (max_len < len)
                status = RPC_S_BUFFER_TOO_SMALL;
            _snprintf(client_id, max_len, "%S", username);
        } else {
            status = RPC_S_UNKNOWN_AUTHN_SERVICE;
        }
    }
    return status;
}

char*
rpc_error_to_string(
    RPC_STATUS status
    )
{
    switch(status) {
    case RPC_S_OK:
        return "OK";
    case RPC_S_INVALID_BINDING:
        return "Invalid binding";
    case RPC_S_WRONG_KIND_OF_BINDING:
        return "Wrong binding";
    case RPC_S_BINDING_HAS_NO_AUTH:
        RpcRaiseException(RPC_S_BINDING_HAS_NO_AUTH);
        return "Binding has no auth";
    default:
        return "BUG: I am confused";
    }
}

void
print_client_info(
    RPC_STATUS peer_status,
    const char* peer_name,
    RPC_STATUS client_status,
    const char* client_id,
    client_auth_info* info
    )
{
    if (peer_status == RPC_S_OK || peer_status == RPC_S_BUFFER_TOO_SMALL) {
        DEBUG_PRINT((STARTUP "Peer Name is \"%s\"\n",
                     peer_name));
    } else {
        DEBUG_PRINT((STARTUP "Error %u getting Peer Name (%s)\n",
                     peer_status, rpc_error_to_string(peer_status)));
    }

    if (client_status == RPC_S_OK || client_status == RPC_S_BUFFER_TOO_SMALL) {
        if (info) {
            DEBUG_PRINT((STARTUP "Client Auth Info\n"
                         "\tServer Principal:       %s\n"
                         "\tAuthentication Level:   %d\n"
                         "\tAuthentication Service: %d\n"
                         "\tAuthorization Service:  %d\n",
                         info->server_principal,
                         info->authn_level,
                         info->authn_svc,
                         info->authz_svc));
        }
        DEBUG_PRINT((STARTUP "Client ID is \"%s\"\n",
                     client_id));
    } else {
        DEBUG_PRINT((STARTUP "Error getting Client Info (%u = %s)\n", 
                     client_status, rpc_error_to_string(client_status)));
    }
}

DWORD
sid_check()
{
    DWORD status = 0;
    HANDLE hToken_c = 0;
    HANDLE hToken_s = 0;
    PTOKEN_USER ptu_c = 0;
    PTOKEN_USER ptu_s = 0;
    DWORD len = 0;
    BOOL bImpersonate = FALSE;

    // Note GetUserName will fail while impersonating at identify
    // level.  The workaround is to impersonate, OpenThreadToken,
    // revert, call GetTokenInformation, and finally, call
    // LookupAccountSid.

    // XXX - Note: This workaround does not appear to work.
    // OpenThreadToken fails with error 1346: "Either a requid
    // impersonation level was not provided or the provided
    // impersonation level is invalid".

    status = RpcImpersonateClient(0);
    if (status)
        CLEANUP_STATUS(status);

    bImpersonate = TRUE;

    if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &hToken_c))
        CLEANUP_STATUS(GetLastError());

    status = RpcRevertToSelf();
    if (status)
        CLEANUP_STATUS(status);
    bImpersonate = FALSE;

    len = 0;
    GetTokenInformation(hToken_c, TokenUser, ptu_c, 0, &len);
    if (len == 0)
        CLEANUP_STATUS(GetLastError());
    if (!(ptu_c = (PTOKEN_USER)LocalAlloc(0, len)))
        CLEANUP_STATUS(GetLastError());
    if (!GetTokenInformation(hToken_c, TokenUser, ptu_c, len, &len))
        CLEANUP_STATUS(GetLastError());

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken_s))
        CLEANUP_STATUS(GetLastError());

    len = 0;
    GetTokenInformation(hToken_s, TokenUser, ptu_s, 0, &len);
    if (len == 0)
        CLEANUP_STATUS(GetLastError());
    if (!(ptu_s = (PTOKEN_USER)LocalAlloc(0, len)))
        CLEANUP_STATUS(GetLastError());

    if (!GetTokenInformation(hToken_s, TokenUser, ptu_s, len, &len))
        CLEANUP_STATUS(GetLastError());

    if (!EqualSid(ptu_s->User.Sid, ptu_c->User.Sid))
        status = RPC_S_ACCESS_DENIED;

 cleanup:
    if (!hToken_c && !bImpersonate)
        DEBUG_PRINT((STARTUP "Cannot impersonate (%u)\n", status));
    else if (!hToken_c)
        DEBUG_PRINT((STARTUP "Failed to open client token (%u)\n", status));
    else if (bImpersonate)
        DEBUG_PRINT((STARTUP "Failed to revert (%u)\n", status));
    else if (!ptu_c)
        DEBUG_PRINT((STARTUP "Failed to get client token user info (%u)\n",
                     status));
    else if (!hToken_s)
        DEBUG_PRINT((STARTUP "Failed to open server token (%u)\n", status));
    else if (!ptu_s)
        DEBUG_PRINT((STARTUP "Failed to get server token user info (%u)\n",
                     status));
    else if (status == RPC_S_ACCESS_DENIED)
        DEBUG_PRINT((STARTUP "SID **does not** match!\n"));
    else if (status == RPC_S_OK)
        DEBUG_PRINT((STARTUP "SID matches!\n"));
    else
        assert(status == RPC_S_OK);

    if (bImpersonate)
        RpcRevertToSelf();
    if (hToken_c && hToken_c != INVALID_HANDLE_VALUE)
        CloseHandle(hToken_c);
    if (ptu_c)
        LocalFree(ptu_c);
    if (hToken_s && hToken_s != INVALID_HANDLE_VALUE)
        CloseHandle(hToken_s);
    if (ptu_s)
        LocalFree(ptu_s);
    if (status) {
        DEBUG_PRINT((STARTUP "sid_check returning %u\n", status));
    }
    return status;
}

RPC_STATUS
RPC_ENTRY
sec_callback(
    IN RPC_IF_ID *Interface,
    IN void *Context
    )
{
    char peer_name[1024];
    char client_name[1024];
    RPC_STATUS peer_status;
    RPC_STATUS client_status;

    DEBUG_PRINT((STARTUP "Entering sec_callback\n"));
    peer_status = GetPeerName(Context, peer_name, sizeof(peer_name));
    client_status = GetClientId(Context, client_name, sizeof(client_name), 0);
    print_client_info(peer_status, peer_name, client_status, client_name, 0);
    DWORD sid_status = sid_check();
    DEBUG_PRINT((STARTUP "Exiting sec_callback (%u)\n", sid_status));
    return sid_status;
}

DWORD
startup_server(
    ParseOpts::Opts& opts
    )
{
    LPSTR endpoint = 0;
    LPSTR event_name = 0;
    DWORD status = 0;
    PSECURITY_DESCRIPTOR psd = 0;
    HANDLE hEvent = 0;
    Init::InitInfo info;

    status = Init::Info(info);
    CLEANUP_ON_STATUS(status);

    if (!opts.pszEndpoint) {
        status = alloc_name(&endpoint, "ep", info.isNT);
        CLEANUP_ON_STATUS(status);
        status = alloc_name(&event_name, "startup", info.isNT);
        CLEANUP_ON_STATUS(status);
        hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, event_name);
        // We ignore any error opening the even because we do not know
        // who started us.

        // XXX - Maybe there sould be a command-line parameter to
        // specify whether we error if we cannot open the event, but
        // that seems unncessary since the event signaling is really
        // just an optimization.
    } else {
        endpoint = opts.pszEndpoint;
    }

    DEBUG_PRINT((STARTUP "Registering endpoint %s\n", endpoint));

    if (info.isNT) {
        status = alloc_own_security_descriptor_NT(&psd);
        DEBUG_PRINT((STARTUP
                     "alloc_own_security_descriptor_NT returned %u\n",
                     status));
        CLEANUP_ON_STATUS(status);
    }

    status = RpcServerUseProtseqEp((unsigned char *)"ncalrpc",
                                   opts.cMaxCalls,
                                   (unsigned char *)endpoint,
                                   opts.bDontProtect ? 0 : psd);  // SD
    DEBUG_PRINT((STARTUP "RpcServerUseProtseqEp returned %u\n", status));
    CLEANUP_ON_STATUS(status);

    status = RpcServerRegisterAuthInfo(0, // server pricipal
                                       RPC_C_AUTHN_WINNT,
                                       0,
                                       0);
    DEBUG_PRINT((STARTUP "RpcServerRegisterAuthInfo returned %u\n", status));
    CLEANUP_ON_STATUS(status);

    if (!info.isNT) {
        status = RpcServerRegisterIf(ccapi_ServerIfHandle,  // interface 
                                     NULL,   // MgrTypeUuid
                                     NULL);  // MgrEpv; null means use default
    } else {
        status = info.fRpcServerRegisterIfEx(ccapi_ServerIfHandle, // interface
                                             NULL, // MgrTypeUuid
                                             NULL, // MgrEpv; 0 means default
                                             RPC_IF_ALLOW_SECURE_ONLY,
                                             opts.cMaxCalls,
                                             opts.bSecCallback ? 
                                             (RPC_IF_CALLBACK_FN*)
                                             sec_callback : 0
            );
    }

    DEBUG_PRINT((STARTUP "RpcServerRegisterIf%s returned %u\n", 
                 info.isNT ? "Ex" : "", status));
    CLEANUP_ON_STATUS(status);

    DEBUG_PRINT((STARTUP "Calling RpcServerListen\n"));
    status = RpcServerListen(opts.cMinCalls,
                             opts.cMaxCalls,
                             opts.fDontWait);
    DEBUG_PRINT((STARTUP "RpcServerListen returned: %u\n", status));
    CLEANUP_ON_STATUS(status);

    if (opts.fDontWait) {
        // Ignore error on SetEvent since it's just an optimization.
        if (hEvent)
            SetEvent(hEvent);
        DEBUG_PRINT((STARTUP "Calling RpcMgmtWaitServerListen\n"));
        status = RpcMgmtWaitServerListen();  // wait operation
        DEBUG_PRINT((STARTUP "RpcMgmtWaitServerListen returned: %u\n",
                     status));
        CLEANUP_ON_STATUS(status);
    }
 cleanup:
    if (hEvent)
        CloseHandle(hEvent);
    free_alloc_p(&event_name);
    free_alloc_p(&psd);
    if (endpoint && (endpoint != opts.pszEndpoint))
        free_alloc_p(&endpoint);
    return status;

}

DWORD
shutdown_server(
    char* ep OPTIONAL
    )
{
    DWORD status = 0;
    CCTEST_MAKE_FUNC(DWORD, CCTEST, (CCTEST_functions*));
    FP_CCTEST pCCTEST = 0;
    CCTEST_functions f = { 0 };
    f.size = sizeof(CCTEST_functions);
    HMODULE module = LoadLibrary(CCAPI_DLL);
    if (!module)
        CLEANUP_ON_STATUS(GetLastError());
    pCCTEST = (FP_CCTEST)GetProcAddress(module, "CCTEST");
    if (!pCCTEST)
        CLEANUP_ON_STATUS(GetLastError());
    status = pCCTEST(&f);
    CLEANUP_ON_STATUS(status);

    if (ep) {
        status = f.shutdown();
        CLEANUP_ON_STATUS(status);
        status = f.reconnect(ep);
        CLEANUP_ON_STATUS(status);
    }
    status = f.shutdown();  // shut down the server side

 cleanup:
    if (status) {
    }
    if (module)
        FreeLibrary(module);
    return status;
}

/* main:  register the interface, start listening for clients */
void main(int argc, char * argv[])
{
    RPC_STATUS status;

    ParseOpts::Opts opts = { 0 };

    opts.cMinCalls           = 1;
    opts.cMaxCalls           = 20;
    opts.fDontWait           = TRUE;

    ParseOpts PO;
#ifdef CCAPI_TEST_OPTIONS
    PO.SetValidOpts("kemnfubc");
#else
    PO.SetValidOpts("kc");
#endif

    PO.Parse(opts, argc, argv);

    if (opts.bConsole)
        SetDebugMode(DM_STDOUT);

    BOOL bAdjustedShutdown = FALSE;
    HMODULE hKernel32 = GetModuleHandle("kernel32");
    if (hKernel32) {
        typedef BOOL (WINAPI *FP_SetProcessShutdownParameters)(DWORD, DWORD);
        FP_SetProcessShutdownParameters pSetProcessShutdownParameters =
            (FP_SetProcessShutdownParameters)
            GetProcAddress(hKernel32, "SetProcessShutdownParameters");
        if (pSetProcessShutdownParameters) {
            bAdjustedShutdown = pSetProcessShutdownParameters(100, 0);
        }
    }
    DEBUG_PRINT(("%s Shutdown Parameters\n", bAdjustedShutdown ? "Adjusted" :
                 "Did not adjust"));

    status = Init::Initialize();
    CLEANUP_ON_STATUS(status);

    if (opts.bShutdown) {
        status = shutdown_server(opts.pszEndpoint);
        CLEANUP_ON_STATUS(status);
    } else {
        status = startup_server(opts);
        CLEANUP_ON_STATUS(status);
    }

 cleanup:
    Init::Cleanup();
    if (status) {
        fprintf(stderr, "An error occured while %s the server (%u)\n", 
                opts.bShutdown ? "shutting down" : "starting/running",
                status);
        exit(status);
    }
}
