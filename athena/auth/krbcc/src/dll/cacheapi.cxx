#include <stdlib.h>
#include <stdio.h>
#include "ccapi.h"
#include "cacheapi.h"
#include "debug.h"
#include "debug.hxx"
#include "marshall.hxx"
#include "secure.hxx"
#include "client.h"
#include "autolock.hxx"

#define TRY "TRY: "

#define MAKE_RPC_CALL(rc, x) \
do { \
    SecureClient* s = 0; \
    SecureClient::Start(s); \
    CcAutoLock* a = 0; \
    CcAutoLock::Start(a, gClientLock); \
    RpcTryExcept { \
        DEBUG_PRINT((TRY #x "\n")); \
        x; \
    } \
    RpcExcept(1) { \
        rc = handle_exception(RpcExceptionCode()); \
    } \
    RpcEndExcept; \
    CcAutoLock::Stop(a); \
    SecureClient::Stop(s); \
} while (0)

static
DWORD
handle_exception(DWORD code)
{
    DEBUG_PRINT((D_EXCEPTION "Runtime reported exception %u\n", code));
    if (code == RPC_S_SERVER_UNAVAILABLE) {
        reconnect_client(0);
    }
    return CC_IO;
}

//////////////////////////////////////////////////////////////////////////////

static const char VENDOR[] = "ACME, Inc.";

CCACHE_API
cc_initialize(
    apiCB** cc_ctx,
    cc_int32 api_version,
    cc_int32* api_supported,
    const char** vendor
    )
{
    CC_INT32 rc = CC_NOERROR;
    if (!cc_ctx)
        return CC_BAD_PARM;
    *cc_ctx = 0;
    if (api_supported)
        *api_supported = CC_API_VER_2;
    if (vendor)
        *vendor = VENDOR;
    if (api_version != CC_API_VER_2)
        return CC_BAD_API_VERSION;
    MAKE_RPC_CALL(rc, rc = rcc_initialize((HCTX*)cc_ctx));
    return rc;
}

CCACHE_API
cc_shutdown(
    apiCB** cc_ctx
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_shutdown((HCTX*)cc_ctx));
    return rc;
}

 
CCACHE_API
cc_get_change_time(
    apiCB* cc_ctx,       // >  DLL's primary control structure
    cc_time_t* time      // <  time of last change to main cache
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_get_change_time((HCTX)cc_ctx, time));
    return rc;
}

CCACHE_API
cc_create(
    apiCB* cc_ctx,          // >  DLL's primary control structure
    const char* name,       // >  name of cache to be [destroyed if exists, then] created
    const char* principal,
    cc_int32 vers,          // >  ticket version (CC_CRED_V4 or CC_CRED_V5)
    cc_uint32 cc_flags,     // >  options
    ccache_p** ccache_ptr   // <  NC control structure
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_create((HCTX)cc_ctx, (const CC_CHAR*)name, 
                                      (const CC_CHAR*)principal, 
                                      vers, cc_flags, (HCACHE*)ccache_ptr));
    return rc;
}

CCACHE_API
cc_open(
    apiCB* cc_ctx,          // >  DLL's primary control structure
    const char* name,       // >  name of pre-created cache
    cc_int32 vers,          // >  ticket version (CC_CRED_V4 or CC_CRED_V5)
    cc_uint32 cc_flags,     // >  options
    ccache_p** ccache_ptr   // <  NC control structure
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_open((HCTX)cc_ctx, (const CC_CHAR*)name, 
                                    vers, cc_flags, (HCACHE*)ccache_ptr));
    return rc;
}

CCACHE_API
cc_close(
    apiCB* cc_ctx,         // >  DLL's primary control structure
    ccache_p** ccache_ptr  // <> NC control structure. NULL after call.
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_close((HCACHE*)ccache_ptr));
    return rc;
}

CCACHE_API
cc_destroy(
    apiCB* cc_ctx,         // >  DLL's primary control structure
    ccache_p** ccache_ptr  // <> NC control structure. NULL after call.
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_destroy((HCACHE*)ccache_ptr));
    return rc;
}

CCACHE_API
cc_seq_fetch_NCs_begin(
    apiCB* cc_ctx, 
    ccache_cit** itNCs
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_NCs_begin((HCTX)cc_ctx, 
                                                   (HCACHE_ITER*)itNCs));
    return rc;
}

CCACHE_API
cc_seq_fetch_NCs_end(
    apiCB* cc_ctx, 
    ccache_cit** itNCs
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_NCs_end((HCACHE_ITER*)itNCs));
    return rc;
}

CCACHE_API
cc_seq_fetch_NCs_next(
    apiCB* cc_ctx,
    ccache_p** ccache_ptr,
    ccache_cit* itNCs
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_NCs_next((HCACHE_ITER)itNCs,
                                                  (HCACHE*)ccache_ptr));
    return rc;
}

CCACHE_API
cc_get_NC_info(
    apiCB* cc_ctx,          // >  DLL's primary control structure
    infoNC*** ppNCi // <  (NULL before call) null terminated, 
                            //    list of a structs (free via cc_free_infoNC())
    )
{
    CC_INT32 rc = 0;
    if (!ppNCi) return CC_BAD_PARM;
    NC_INFO_LIST* info = 0;
    MAKE_RPC_CALL(rc, rc = rcc_get_NC_info((HCTX)cc_ctx, &info));
    if (!rc)
        rc = Marshall::safe_convert(*ppNCi, *info);
    return rc;
}

CCACHE_API
cc_free_NC_info(
    apiCB* cc_ctx,
    infoNC*** ppNCi // <  free list of structs returned by 
                            //    cc_get_cache_names().  set to NULL on return
    )
{
    if (!ppNCi) return CC_BAD_PARM;
    return Marshall::safe_free(*ppNCi);
}

CCACHE_API
cc_get_name(
    apiCB* cc_ctx,              // > DLL's primary control structure
    const ccache_p* ccache_ptr, // > NC control structure
    char** name                 // < name of NC associated with ccache_ptr 
                                //   (free via cc_free_name())
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_get_name((HCACHE)ccache_ptr, (CC_CHAR**)name));
    return rc;
}

CCACHE_API
cc_set_principal(
    apiCB* cc_ctx,                  // > DLL's primary control structure
    const ccache_p* ccache_ptr, // > NC control structure
    const cc_int32 vers,
    const char* principal           // > name of principal associated with NC
                                    //   Free via cc_free_principal()
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_set_principal((HCACHE)ccache_ptr, vers,
                                             (const CC_CHAR*)principal));
    return rc;
}

CCACHE_API
cc_get_principal(
    apiCB* cc_ctx,                  // > DLL's primary control structure
    const ccache_p* ccache_ptr, // > NC control structure
    char** principal                // < name of principal associated with NC
                                    //   Free via cc_free_principal()
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_get_principal((HCACHE)ccache_ptr,
                                             (CC_CHAR**)principal));
    return rc;
}

CCACHE_API
cc_get_cred_version(
    apiCB* cc_ctx,              // > DLL's primary control structure
    const ccache_p* ccache_ptr, // > NC control structure
    cc_int32* vers              // < ticket version associated with NC
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_get_cred_version((HCACHE)ccache_ptr, vers));
    return rc;
}

CCACHE_API
cc_lock_request(
    apiCB* cc_ctx,     	        // > DLL's primary control structure
    const ccache_p* ccache_ptr, // > NC control structure
    const cc_int32 lock_type    // > one (or combination) of above defined 
                                //   lock types
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_lock_request((HCACHE)ccache_ptr, lock_type));
    return rc;
}

CCACHE_API
cc_store(
    apiCB* cc_ctx,               // > DLL's primary control structure
    ccache_p* ccache_ptr,        // > NC control structure
    const cred_union creds       // > credentials to be copied into NC
    )
{
    CC_INT32 rc = 0;
    CRED_UNION* rcreds = 0;
    rc = Marshall::safe_convert(rcreds, creds);
    if (rc) return rc;
    MAKE_RPC_CALL(rc, rc = rcc_store((HCACHE)ccache_ptr, *rcreds));
    CC_INT32 rc2 = Marshall::safe_free(rcreds);
    DEBUG_ASSERT(!rc2);
    return rc;
}

CCACHE_API
cc_remove_cred(
    apiCB* cc_ctx,            // > DLL's primary control structure
    ccache_p* ccache_ptr,     // > NC control structure
    const cred_union creds    // > credentials to remove from NC
    )
{
    CC_INT32 rc = 0;
    CRED_UNION* rcreds = 0;
    rc = Marshall::safe_convert(rcreds, creds);
    if (rc) return rc;
    MAKE_RPC_CALL(rc, rc = rcc_remove_cred((HCACHE)ccache_ptr, *rcreds));
    CC_INT32 rc2 = Marshall::safe_free(rcreds);
    DEBUG_ASSERT(!rc2);
    return rc;
}

CCACHE_API
cc_seq_fetch_creds_begin(
    apiCB* cc_ctx, 
    const ccache_p* ccache_ptr, 
    ccache_cit** itCreds
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_creds_begin((HCACHE)ccache_ptr, 
                                                     (HCRED_ITER*)itCreds));
    return rc;
}

CCACHE_API
cc_seq_fetch_creds_end(
    apiCB* cc_ctx, 
    ccache_cit** itCreds
    )
{
    CC_INT32 rc = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_creds_end((HCRED_ITER*)itCreds));
    return rc;
}

CCACHE_API
cc_seq_fetch_creds_next(
    apiCB* cc_ctx, 
    cred_union** creds, 
    ccache_cit* itCreds
    )
{
    CC_INT32 rc = 0;
    if (!creds) return CC_BAD_PARM;
    CRED_UNION* rcreds = 0;
    MAKE_RPC_CALL(rc, rc = rcc_seq_fetch_creds_next((HCRED_ITER)itCreds,
                                                    &rcreds));
    if (!rc) {
        rc = Marshall::safe_convert(*creds, *rcreds);
        CC_INT32 rc2 = Marshall::safe_free(rcreds);
        DEBUG_ASSERT(!rc2);
    }
    return rc;
}

CCACHE_API
cc_free_principal(
    apiCB* cc_ctx,   // >  DLL's primary control structure
    char** principal // <> ptr to principal to be freed, returned as NULL
                     //    (from cc_get_principal())
    )
{
    if (!principal) return CC_BAD_PARM;
    return Marshall::safe_free(*principal);
}

CCACHE_API
cc_free_name(
    apiCB* cc_ctx,   // >  DLL's primary control structure
    char** name      // <> ptr to name to be freed, returned as NULL
                     //    (from cc_get_name())
    )
{
    if (!name) return CC_BAD_PARM;
    return Marshall::safe_free(*name);
}

CCACHE_API
cc_free_creds(
    apiCB* cc_ctx,     // > DLL's primary control structure
    cred_union** pCred // <> cred (from cc_seq_fetch_creds()) to be freed
                       //    Returned as NULL.
    )
{
    if (!pCred) return CC_BAD_PARM;
    return Marshall::safe_free(*pCred);
}
