#include "ccapi.h"
#include "debug.h"
#include "client.h"
#include "testapi.h"
#include "secure.hxx"

static CCTEST_functions Funcs = {
    sizeof(CCTEST_functions),
    CCTEST_reconnect,
    CCTEST_shutdown
};

DWORD
CCTEST(CCTEST_functions* funcs)
{
    CLIENT_INIT_EX(0, 0);

    if (!funcs || (funcs->size != sizeof(Funcs)))
        return ERROR_INVALID_PARAMETER;
    *funcs = Funcs;
    return 0;
}

DWORD
CCTEST_reconnect(
    char* endpoint OPTIONAL
    )
{
    CcAutoLock AL(Client::sLock);
    SecureClient s;
    return Client::Reconnect(endpoint);
}

DWORD
CCTEST_shutdown(
    void
    )
{
    DWORD status = 0;

    SecureClient* s = 0;
    SecureClient::Start(s);
    CcAutoLock* a = 0;
    CcAutoLock::Start(a, Client::sLock);

    RpcTryExcept {
        Shutdown();
    }
    RpcExcept(1) {
        status = RpcExceptionCode();
        DEBUG_PRINT((D_EXCEPTION "Runtime reported exception %u\n", status));
    }
    RpcEndExcept;

    CcAutoLock::Stop(a);
    SecureClient::Stop(s);

    return status;
}
