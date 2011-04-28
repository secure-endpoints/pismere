#include "ccapi.h"
#include "debug.h"

#define MAKE_RPC_RUNDOWN(x) \
RpcTryExcept { \
    x; \
} \
RpcExcept(1) { \
    DWORD code = RpcExceptionCode(); \
    DEBUG_PRINT((D_EXCEPTION "Runtime reported exception %u\n", code)); \
} \
RpcEndExcept

void __RPC_USER HCTX_rundown(HCTX ctx)
{
    DEBUG_PRINT((D_CLEANUP "Running down CTX: 0x%08X\n", ctx));
    MAKE_RPC_RUNDOWN(rcc_shutdown(&ctx));
}

void __RPC_USER HCACHE_rundown(HCACHE cache)
{
    DEBUG_PRINT((D_CLEANUP "Running down CACHE: 0x%08X\n", cache));
    MAKE_RPC_RUNDOWN(rcc_close(&cache));
}

void __RPC_USER HCACHE_ITER_rundown(HCACHE_ITER iter)
{
    DEBUG_PRINT((D_CLEANUP "Running down CACHE_ITER: 0x%08X\n", iter));
    MAKE_RPC_RUNDOWN(rcc_seq_fetch_NCs_end(&iter));
}

void __RPC_USER HCRED_ITER_rundown(HCRED_ITER iter)
{
    DEBUG_PRINT((D_CLEANUP "Running down CRED_ITER: 0x%08X\n", iter));
    MAKE_RPC_RUNDOWN(rcc_seq_fetch_creds_end(&iter));
}
