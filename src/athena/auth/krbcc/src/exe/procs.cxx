#include <stdlib.h>
#include <stdio.h>
#include "ccapi.h"
#include <cacheapi.h>
#include "handlemap.hxx"
#include "debug.h"
#include "debug.hxx"
#include "marshall.hxx"
#include "autolock.hxx"

static CcOsLock api_lock;

#define SERVER_CONNECT  "SERVER CONNECT: "
#define SERVER_SHUTDOWN "SERVER SHUTDOWN: "
#define ENTER "ENTER: "
#define LEAVE "LEAVE: "
#define NOTSUPPORTED "NOT SUPPORTED: "

CC_UINT32
Connect(
    CC_CHAR* name
    )
{
    HANDLE hMap = 0;
    PDWORD pvalue = 0;
    CC_UINT32 result = 0;

    DEBUG_PRINT((SERVER_CONNECT "Opening map: %s\n", name));

    hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPSTR)name);
    if (!hMap)
        goto cleanup;

    pvalue = (PDWORD)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
    if (!pvalue)
        goto cleanup;

    *pvalue += 1;
    result = *pvalue;

 cleanup:
    if (pvalue) {
        BOOL ok = UnmapViewOfFile(pvalue);
        DEBUG_ASSERT(ok);
    }
    if (hMap)
        CloseHandle(hMap);
    return result;
}

void Shutdown(void)
{
    RPC_STATUS status;

    DEBUG_PRINT((SERVER_SHUTDOWN "Calling RpcMgmtStopServerListening\n"));
    status = RpcMgmtStopServerListening(NULL);
    DEBUG_PRINT((SERVER_SHUTDOWN "RpcMgmtStopServerListening returned: %u\n",
                 status));
    if (status) {
       exit(status);
    }

    DEBUG_PRINT((SERVER_SHUTDOWN "Calling RpcServerUnregisterIf\n"));
    status = RpcServerUnregisterIf(NULL, NULL, FALSE);
    DEBUG_PRINT((SERVER_SHUTDOWN "RpcServerUnregisterIf returned %u\n",
                 status));
    if (status) {
       exit(status);
    }
}

//////////////////////////////////////////////////////////////////////////////

static HandleMap::HandleMap<HCTX, apiCB*> hmContext;
static HandleMap::HandleMap<HCACHE, ccache_p*> hmCache;
static HandleMap::HandleMap<HCACHE_ITER, ccache_cit*> hmCacheIter;
static HandleMap::HandleMap<HCRED_ITER, ccache_cit*> hmCredIter;

CC_INT32
he_to_ccerror(
    HandleMap::Error_t e
    )
{
    using namespace HandleMap;

    switch(e) {
    case HE_NOERROR:
        return CC_NOERROR;
    case HE_NOT_FOUND:
        return CC_BAD_PARM;
    case HE_NO_MEM:
    case HE_MAX_SIZE:
        return CC_NOMEM;
    default:
        // If we get here, we have a bug...
        DEBUG_PRINT((D_BUG "Unrecognized HandleMap::Exception type: %u\n", e));
        DEBUG_ASSERT(false);
        return CC_NOT_SUPP;
    }
}

//////////////////////////////////////////////////////////////////////////////

CC_INT32
rcc_initialize(
    HCTX* pctx
    )
{
    DEBUG_PRINT((ENTER "rcc_initialize:\n"));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    apiCB* real_ctx = 0;
    if (!pctx) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *pctx = 0;
    try {
        hmContext.create(*pctx);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    retval = cc_initialize(&real_ctx, CC_API_VER_2, 0, 0);
    DEBUG_ASSERT((retval && !real_ctx) || (!retval && real_ctx));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmContext.close(*pctx);
    } else {
        hmContext[*pctx] = real_ctx;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_initialize: ctx = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pctx),
                 retval));
    return retval;
}

CC_INT32
rcc_shutdown(
    HCTX* pctx
    )
{
    DEBUG_PRINT((ENTER "rcc_shutdown: ctx = %s\n", PTR_TO_STR(pctx)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!pctx) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    try {
        cc_int32 retval = cc_shutdown(&hmContext[*pctx]);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    hmContext.close(*pctx);
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_shutdown: ctx = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pctx),
                 retval));
    return retval;
}

CC_INT32
rcc_get_change_time(
    HCTX ctx,
    CC_TIME_T* time
    )
{
    DEBUG_PRINT((ENTER "rcc_get_change_time: ctx = 0x%08x\n", ctx));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!time) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *time = 0;
    try {
        retval = cc_get_change_time(hmContext[ctx], time);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_get_change_time: time = %s\n"
                 "\tretval = %d\n",
                 TIME_TO_STR(time),
                 retval));
    return retval;
}

CC_INT32
rcc_create(
    HCTX ctx,
    const CC_CHAR* name,
    const CC_CHAR* principal,
    CC_INT32 vers,
    CC_UINT32 flags,
    HCACHE* pcache
    )
{
    DEBUG_PRINT((ENTER "rcc_create: ctx = 0x%08X, name = %s, princ = %s,\n"
                 "\tvers = %d, flags = %u\n", ctx, name, principal, vers, 
                 flags));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    ccache_p* ccache = 0;
    if (!pcache) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *pcache = 0;
    try {
        hmCache.create(*pcache);
        retval = cc_create(hmContext[ctx], (char*)name, (char*)principal,
                           vers, flags, &ccache);
    } catch (HandleMap::Exception& e) {
        // if *pcache, then the exception happened on hmContext[ctx],
        // so we need to cleanup...
        if (*pcache)
            hmCache.close(*pcache);
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    DEBUG_ASSERT((retval && !ccache) || (!retval && ccache));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmCache.close(*pcache);
    } else {
        hmCache[*pcache] = ccache;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_create: cache = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pcache),
                 retval));
    return retval;
}

CC_INT32
rcc_open(
    HCTX ctx,
    const CC_CHAR* name,
    CC_INT32 vers,
    CC_UINT32 flags,
    HCACHE* pcache
    )
{
    DEBUG_PRINT((ENTER "rcc_open: ctx = 0x%08X, name = %s,\n"
                 "\tvers = %d, flags = %u\n", ctx, name, vers, flags));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    ccache_p* cache = 0;
    if (!pcache) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *pcache = 0;
    try {
        hmCache.create(*pcache);
        retval = cc_open(hmContext[ctx], (char*)name, vers, flags, &cache);
    } catch (HandleMap::Exception& e) {
        // if *pcache, then the exception happened on hmContext[ctx],
        // so we need to cleanup...
        if (*pcache)
            hmCache.close(*pcache);
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    DEBUG_ASSERT((retval && !cache) || (!retval && cache));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmCache.close(*pcache);
    } else {
        hmCache[*pcache] = cache;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_open: cache = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pcache),
                 retval));
    return retval;
}

CC_INT32
rcc_close(
    HCACHE* pcache
    )
{
    DEBUG_PRINT((ENTER "rcc_close: cache = %s\n", PTR_TO_STR(pcache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!pcache) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    try {
        cc_int32 retval = cc_close(0 /* XXX */, &hmCache[*pcache]);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    hmCache.close(*pcache);
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_close: cache = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pcache),
                 retval));
    return retval;
}

CC_INT32
rcc_destroy(
    HCACHE* pcache
    )
{
    DEBUG_PRINT((ENTER "rcc_destroy: cache = %s\n", PTR_TO_STR(pcache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!pcache) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    try {
        cc_int32 retval = cc_destroy(0 /* XXX */, &hmCache[*pcache]);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    hmCache.close(*pcache);
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_destroy: cache = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pcache),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_NCs_begin(
    HCTX ctx,
    HCACHE_ITER* piter
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_NCs_begin: ctx = %s\n", 
                 PTR_TO_STR(&ctx)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    ccache_cit* iter = 0;
    if (!piter) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *piter = 0;
    try {
        hmCacheIter.create(*piter);
        retval = cc_seq_fetch_NCs_begin(hmContext[ctx], &iter);
    } catch (HandleMap::Exception& e) {
        // if *piter, then the exception happened on hmContext[ctx],
        // so we need to cleanup...
        if (*piter)
            hmCacheIter.close(*piter);
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    DEBUG_ASSERT((retval && !iter) || (!retval && iter));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmCacheIter.close(*piter);
    } else {
        hmCacheIter[*piter] = iter;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_NCs_begin: iter = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(piter),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_NCs_end(
    HCACHE_ITER* piter
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_NCs_end: iter = %s\n", 
                 PTR_TO_STR(piter)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!piter) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    try {
        cc_int32 retval = cc_seq_fetch_NCs_end(0 /* XXX */, 
                                               &hmCacheIter[*piter]);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    hmCacheIter.close(*piter);
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_NCs_end: iter = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(piter),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_NCs_next(
    HCACHE_ITER iter,
    HCACHE* pcache
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_NCs_next: iter = %s\n", 
                 PTR_TO_STR(&iter)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    ccache_p* cache = 0;
    if (!pcache) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *pcache = 0;
    try {
        hmCache.create(*pcache);
        retval = cc_seq_fetch_NCs_next(0 /* XXX */, &cache, hmCacheIter[iter]);
    } catch (HandleMap::Exception& e) {
        // if *pcache, then the exception happened on hmContext[ctx],
        // so we need to cleanup...
        if (*pcache)
            hmCache.close(*pcache);
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    DEBUG_ASSERT((retval && !cache) || (!retval && cache));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmCache.close(*pcache);
    } else {
        hmCache[*pcache] = cache;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_NCs_next: cache = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(pcache),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_NCs(
    HCTX ctx,
    HCACHE_ITER* piter,
    HCACHE* pcache
    )
{
    DEBUG_PRINT((NOTSUPPORTED "rcc_seq_fetch_NCs: ctx = %s, iter = %s\n", 
                 PTR_TO_STR(&ctx), PTR_TO_STR(piter)));
    return CC_NOT_SUPP;
}

CC_INT32
rcc_get_NC_info(
    HCTX ctx,
    NC_INFO_LIST** info_list
    )
{
    DEBUG_PRINT((ENTER "rcc_get_NC_info: ctx = %s\n", PTR_TO_STR(&ctx)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    infoNC** ret_info = 0;
    if (!info_list) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *info_list = 0;
    try {
        retval = cc_get_NC_info(hmContext[ctx], &ret_info);
        if (!retval) {
            DEBUG_ASSERT(ret_info);
            retval = Marshall::safe_convert(*info_list, ret_info);
        }
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    if (ret_info) {
        cc_int32 temp = cc_free_NC_info(0 /* XXX */, &ret_info);
        DEBUG_ASSERT(!temp);
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_get_NC_info: info_list = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(info_list),
                 retval));
    return retval;
}

CC_INT32
rcc_get_name(
    HCACHE cache,
    CC_CHAR** name
    )
{
    DEBUG_PRINT((ENTER "rcc_get_name: cache = %s\n", PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    char* ret_name = 0;
    if (!name) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *name = 0;
    try {
        retval = cc_get_name(0 /* XXX */, hmCache[cache], &ret_name);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    if (!retval) {
        DEBUG_ASSERT(ret_name);
        retval = Marshall::safe_convert(*name, ret_name);
        cc_int32 temp = cc_free_name(0 /* XXX */, &ret_name);
        DEBUG_ASSERT(!temp);
    };
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_get_name: name = %s\n"
                 "\tretval = %d\n",
                 STR_TO_STR(name),
                 retval));
    return retval;
}

CC_INT32
rcc_set_principal(
    HCACHE cache,
    CC_INT32 vers,
    const CC_CHAR* principal
    )
{
    DEBUG_PRINT((ENTER "rcc_set_principal: cache = %s, vers = %d, "
                 "principal = %s\n", 
                 PTR_TO_STR(&cache), vers, principal));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    try {
        retval = cc_set_principal(0 /* XXX */, hmCache[cache], 
                                  vers, (char*) principal);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    DEBUG_PRINT((LEAVE "rcc_set_principal:\n"
                 "\tretval = %d\n",
                 retval));
    return retval;
}

CC_INT32
rcc_get_principal(
    HCACHE cache,
    CC_CHAR** principal
    )
{
    DEBUG_PRINT((ENTER "rcc_get_principal: cache = %s\n", 
                 PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    char* ret_principal = 0;
    if (!principal) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *principal = 0;
    try {
        retval = cc_get_principal(0 /* XXX */, hmCache[cache], &ret_principal);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    if (!retval) {
        DEBUG_ASSERT(ret_principal);
        retval = Marshall::safe_convert(*principal, ret_principal);
        cc_int32 temp = cc_free_principal(0 /* XXX */, &ret_principal);
        DEBUG_ASSERT(!temp);
    };
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_get_principal: principal = %s\n"
                 "\tretval = %d\n",
                 STR_TO_STR(principal),
                 retval));
    return retval;
}

CC_INT32
rcc_get_cred_version(
    HCACHE cache,
    CC_INT32* vers
    )
{
    DEBUG_PRINT((ENTER "rcc_get_cred_version: cache = %s\n", 
                 PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!vers) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *vers = 0;
    try {
        retval = cc_get_cred_version(0 /* XXX */, hmCache[cache], vers);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_get_cred_version: vers = %s\n"
                 "\tretval = %d\n",
                 INT_TO_STR(vers),
                 retval));
    return retval;
}

CC_INT32
rcc_lock_request(
    HCACHE cache,
    CC_INT32 lock_type
    )
{
    DEBUG_PRINT((NOTSUPPORTED "rcc_lock_request: cache = %s, "
                 "lock_type = %d\n", 
                 PTR_TO_STR(&cache), lock_type));
    return CC_NOT_SUPP;
}

CC_INT32
rcc_store(
    HCACHE cache,
    CRED_UNION cred
    )
{
    DEBUG_PRINT((ENTER "rcc_store: cache = %s, cred = ?\n", 
                 PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    cred_union* pcred = 0;
    try {
        Marshall::convert(pcred, cred);
        retval = cc_store(0 /* XXX */, hmCache[cache], *pcred);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    } catch (Marshall::Exception& e) {
        retval = e.CcError();
    }
    if (pcred) {
        cc_int32 temp = Marshall::safe_free(pcred);
        DEBUG_ASSERT(!temp);
    }
    DEBUG_PRINT((LEAVE "rcc_store:\n"
                 "\tretval = %d\n",
                 retval));
    return retval;
}

CC_INT32
rcc_remove_cred(
    HCACHE cache,
    CRED_UNION cred
    )
{
    DEBUG_PRINT((ENTER "rcc_remove_cred: cache = %s, cred = ?\n", 
                 PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    cred_union* pcred = 0;
    try {
        Marshall::convert(pcred, cred);
        retval = cc_remove_cred(0 /* XXX */, hmCache[cache], *pcred);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    } catch (Marshall::Exception& e) {
        retval = e.CcError();
    }
    if (pcred) {
        cc_int32 temp = Marshall::safe_free(pcred);
        DEBUG_ASSERT(!temp);
    }
    DEBUG_PRINT((LEAVE "rcc_remove_cred:\n"
                 "\tretval = %d\n",
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_creds(
    HCACHE cache,
    HCRED_ITER* piter,
    CRED_UNION** cred
    )
{
    DEBUG_PRINT((NOTSUPPORTED "rcc_seq_fetch_creds: cache = %s\n"));
    return CC_NOT_SUPP;
}

CC_INT32
rcc_seq_fetch_creds_begin(
    HCACHE cache,
    HCRED_ITER* piter
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_creds_begin: cache = %s\n",
                 PTR_TO_STR(&cache)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    ccache_cit* iter = 0;
    if (!piter) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *piter = 0;
    try {
        hmCredIter.create(*piter);
        retval = cc_seq_fetch_creds_begin(0 /* XXX */, hmCache[cache], &iter);
    } catch (HandleMap::Exception& e) {
        // if *piter, then the exception happened on hmContext[ctx],
        // so we need to cleanup...
        if (*piter)
            hmCredIter.close(*piter);
        retval = he_to_ccerror(e.type);
        goto cleanup;
    }
    DEBUG_ASSERT((retval && !iter) || (!retval && iter));
    // if these throw, it's equivalent to an assertion failure...
    if (retval) {
        hmCredIter.close(*piter);
    } else {
        hmCredIter[*piter] = iter;
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_creds_begin: iter = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(piter),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_creds_end(
    HCRED_ITER* piter
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_creds_end: iter = %s\n",
                 PTR_TO_STR(piter)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    if (!piter) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    try {
        cc_int32 retval = cc_seq_fetch_creds_end(0 /* XXX */, 
                                                 &hmCredIter[*piter]);
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    hmCredIter.close(*piter);
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_creds_end: iter = %s\n"
                 "\tretval = %d\n",
                 PTR_TO_STR(piter),
                 retval));
    return retval;
}

CC_INT32
rcc_seq_fetch_creds_next(
    HCRED_ITER iter,
    CRED_UNION** cred
    )
{
    DEBUG_PRINT((ENTER "rcc_seq_fetch_creds_next: iter = %s\n",
                 PTR_TO_STR(&iter)));
    CcAutoLock AL(api_lock);
    cc_int32 retval = 0;
    cred_union* pcred = 0;
    if (!cred) {
        retval = CC_BAD_PARM;
        goto cleanup;
    }
    *cred = 0;
    try {
        retval = cc_seq_fetch_creds_next(0 /* XXX */, &pcred, 
                                         hmCredIter[iter]);
        if (!retval) {
            DEBUG_ASSERT(pcred);
            retval = Marshall::safe_convert(*cred, *pcred);
        }
    } catch (HandleMap::Exception& e) {
        retval = he_to_ccerror(e.type);
    }
    if (pcred) {
        cc_int32 temp = cc_free_creds(0 /* XXX */, &pcred);
        DEBUG_ASSERT(!temp);
    }
 cleanup:
    DEBUG_PRINT((LEAVE "rcc_seq_fetch_creds_next: creds = ?\n"
                 "\tretval = %d\n",
                 retval));
    return retval;
}
