/*
** Copyright 1997 by the Regents of the University of Michigan
**
** For copying and distribution information, please see the file
** <UMCopyRt.h>.
*/

/*
**
** cacheapi.cpp
**	
** All calls pass through here.  This layer does as much parameter checking as
** it can and does all the type casting.
*/

/*
** Bugs/Weaknesses/PossibleToDos
**
** - should make copious use of try/catch or try/finally or catch
** exceptions somehow
**/

#include "UmCopyRt.h"
#include "creds.h"
#include <crtdbg.h>

extern apiCB *gControlBlock;
extern DWORD gControlBlockCount;
extern CRITICAL_SECTION gCS;

/* ----------------------------------------------------------------------------
**	constants, inline casts
** ----------------------------------------------------------------------------
*/

static const char* VENDOR = "MIT + UMichigan shared, memory based credentials cache project - 1997";

/*
CacheAPI is the "top level" of all this. Every call comes through CacheAPI
first.  CacheAPI mostly does parameter checking (addressability, limit checks)
and type casting.  cc_ctx is a pointer to a CCache_ctx class object which is 
implemented in ctx.cpp.  
*/


/* ----------------------------------------------------------------------------
**	main cache : initialize, shutdown, get_cache_names, get_change_time,
**				 & get/set both principal & instance
** ----------------------------------------------------------------------------
*/
/*
** cc_initialize
** (apiCB** cc_ctx, const int api_version, PINT api_supported, 
**  const char** vendor)
**
** Description:
**   Allocates storage DLL needs to provide cache services to caller.
**
**   apiCB** cc_ctx        : address of storage DLL can store ptr to its 
**                           opaque data structure in
**   const int api_version : version of API caller can handle
**   PINT api_supported    : NULL or address of storage DLL should put API 
**                           supported in
**   const char** vendor   : NULL or address of storage DLL should put ptr 
**                           to read only Vendor string in
**
** Return Value:
**   returns CC_NOERROR with cc_ctx pointing to a CCache_ctx class object 
**   or one of the error codes below
**
** Error Codes: 
**   CC_BAD_PARM         : cc_ctx NULL, *cc_ctx not NULL, or *vendor not NULL
**   CC_NO_MEM           : insufficient memory to procede
**   CC_BAD_API_VERSION  : api_version != CC_API_VER_1
**
** Asserts (in Debug mode)
**   cc_ctx readable mem, *cc_ctx is writeable mem
**   api_supported is readable mem, [if !NULL, *api_supported is writable mem]
**   vendor is readable mem, [if !NULL, *vendor is writable mem]
*/
extern "C"
cc_int32 cc_initialize(apiCB** cc_ctx, cc_int32 api_version, cc_int32* api_supported, const char** vendor)
{
    if (NULL == cc_ctx)
	return(CC_BAD_PARM);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(apiCB*), TRUE));
	
    if (NULL != *cc_ctx)
	return(CC_BAD_PARM);

    if (api_version > CC_API_VER_2 || api_version < 0)
	return(CC_BAD_API_VERSION);
	
    if (api_supported)
    {
	_ASSERTE(_CrtIsValidPointer(api_supported, sizeof(int), TRUE));
	*api_supported = CC_API_VER_2;
    }
	
    if (vendor)
    {
	_ASSERTE(_CrtIsValidPointer(vendor, sizeof(char**), TRUE));
	if (NULL == *vendor)
	    *vendor = VENDOR;
	else
	    return(CC_BAD_PARM);
    }

    _ASSERTE(!((bool)gControlBlockCount ^ (bool)gControlBlock));

    EnterCriticalSection(&gCS);
    if (!gControlBlockCount)
    {
	*cc_ctx = reinterpret_cast<apiCB*>(new CCache_ctx());
	gControlBlock = *cc_ctx;
    }
    else
	*cc_ctx = gControlBlock;

    cc_int32 rc = CC_NOERROR;

    if (NULL == *cc_ctx)
	rc = CC_NOMEM;
    else
	gControlBlockCount++;

    LeaveCriticalSection(&gCS);
    return(rc);
};

/*
** cc_shutdown
**
** Purpose:
**   Frees all storage allocated by DLL in cc_init()
**
** Parameters:
**   apiCB** cc_ctx : address of storage DLL should free (filled in cc_init)
**                  : will be returned NULL
**
** Possible Error Codes: 
**   CC_NO_EXIST    : *cc_ctx already NULL
**		
** Asserts (in Debug mode)
**   *cc_ctx is writeable mem
*/
extern "C"
cc_int32 cc_shutdown(apiCB** cc_ctx)
{
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(apiCB*), TRUE));
    _ASSERTE(!((bool)gControlBlockCount ^ (bool)gControlBlock));

    if (NULL == *cc_ctx)
	return(CC_NO_EXIST);

    if (*cc_ctx != gControlBlock)
	return CC_NO_EXIST;

    if (!gControlBlockCount)
	return CC_NO_EXIST;

    _ASSERTE(_CrtIsValidPointer(*cc_ctx, sizeof(CCache_ctx), TRUE));

    EnterCriticalSection(&gCS);

    gControlBlockCount--;
    if (!gControlBlockCount)
    {
	CCache_ctx* pC = reinterpret_cast<CCache_ctx*>(*cc_ctx);
	delete pC;
	gControlBlock = NULL;
    }

    LeaveCriticalSection(&gCS);

    *cc_ctx = NULL;
    return(CC_NOERROR);
};


/*
**	cc_get_change_time
*/
extern "C"
cc_int32 cc_get_change_time(apiCB* cc_ctx, cc_time_t* time)
{
    if (NULL == cc_ctx) 
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));
    _ASSERTE(_CrtIsValidPointer(time, sizeof(cc_time_t), TRUE));

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    if (time)
    {
	*time = reinterpret_cast<CCache_ctx*>(cc_ctx)->GetCacheChangeTime();
	reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
	return(CC_NOERROR);
    }
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(CC_NOMEM);
};

/*
**	cc_get_NC_info
*/
extern "C"
cc_int32 cc_get_NC_info(apiCB* cc_ctx, struct _infoNC*** ppNCi)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));
    _ASSERTE(_CrtIsValidPointer(ppNCi, sizeof(struct _infoNC**), TRUE));

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->GetNCInfo(ppNCi);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
** cc_free_NC_info
**
** Frees all storage returned by cc_get_NC_info.
** cc_get_NC_info calls this during bailout due to malloc failure
** So this routine has to handle partially built data structures.
*/

extern "C"
cc_int32 cc_free_NC_info(apiCB* cc_ctx, struct _infoNC*** ppNCi)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(ppNCi, sizeof(struct _infoNC**), TRUE));
    _ASSERTE(_CrtIsValidPointer(*ppNCi, sizeof(struct _infoNC*), TRUE));
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    if (*ppNCi)
    {
	struct _infoNC** list_pNCI = *ppNCi;
	while (*list_pNCI)
	{
	    struct _infoNC* pNCI = *list_pNCI;
	    if (pNCI)
	    {
		if (pNCI->name)
		{
		    free(pNCI->name);
		    pNCI->name = NULL;
		}
		free(pNCI); pNCI = NULL;
	    }
	    list_pNCI++;
	}
	free(*ppNCi); *ppNCi = NULL;	
    }
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(CC_NOERROR);
};

/*
**	cc_set_principal
*/
extern "C"
cc_int32 cc_set_principal(apiCB* cc_ctx, const ccache_p* ccache_pointer, const cc_int32 vers, const char* principal)
{
    cc_int32 rc;
    if (NULL == cc_ctx)
	return(CC_NO_EXIST);
	
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));
    _ASSERTE(_CrtIsValidPointer(principal, sizeof(char*), FALSE));

    if ((ccache_pointer == NULL) || (principal == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->SetPrincipal(reinterpret_cast<const int>(ccache_pointer), principal);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
**	cc_get_principal
*/
extern "C"
cc_int32 cc_get_principal(apiCB* cc_ctx, const ccache_p* ccache_pointer, char** principal)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);
	
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if ((ccache_pointer == NULL) || (principal == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->GetPrincipal(reinterpret_cast<const int>(ccache_pointer), principal);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/* ----------------------------------------------------------------------------
**  named caches (NCs) : create/destroy, open/close, 
**                 get_principal, cc_get_cred_version, & lock_request
** ----------------------------------------------------------------------------
*/
/*
**	cc_open
*/
extern "C"
cc_int32 cc_open(apiCB* cc_ctx, const char* name, cc_int32 vers, cc_uint32 cc_flags, ccache_p** ccache_ptr)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    if ((ccache_ptr == NULL) || (name == NULL))
	return(CC_BAD_PARM);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));
    _ASSERTE(_CrtIsValidPointer(name, sizeof(char*), FALSE));
    _ASSERTE(_CrtIsValidPointer(ccache_ptr, sizeof(apiCB*), TRUE));

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->OpenNamedCache(name, vers, (const cc_nc_flags)cc_flags, reinterpret_cast<int*>(ccache_ptr));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
**	cc_close
*/
extern "C"
cc_int32 cc_close(apiCB* cc_ctx, ccache_p** ccache_ptr)
{
    int rc = 0;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (ccache_ptr == NULL)
	return(CC_BAD_PARM);

    if (*ccache_ptr == NULL)
        return CC_NO_EXIST;

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->CloseNamedCache(reinterpret_cast<int*>(ccache_ptr));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/* ----------------------------------------------------------------------------
**  named caches (NCs) : create/destroy, open/close, 
**                 get_principal, cc_get_cred_version, & lock_request
** ----------------------------------------------------------------------------
*/
/*
**	cc_create
*/
extern "C"
cc_int32 cc_create(apiCB* cc_ctx, const char* name, const char* principal, cc_int32 vers, cc_uint32 cc_flags, ccache_p** ccache_ptr)
{
    int rc = 0;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));
    _ASSERTE(_CrtIsValidPointer(ccache_ptr, sizeof(ccache_p*), TRUE));

    *ccache_ptr = NULL; // in case we leave early

    if (!_CrtIsValidPointer(name, sizeof(char*), FALSE))
	return(CC_BAD_PARM);

    if (!_CrtIsValidPointer(principal, sizeof(char*), FALSE))
	return(CC_BAD_PARM);

    // duplicates detected by CreateNamedCache
    if ((ccache_ptr == NULL) || (name == NULL) || (principal == NULL))
	return(CC_BAD_PARM);

    if ((rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->CreateNamedCache(name, principal, vers, cc_flags, 
								      reinterpret_cast<int*>(ccache_ptr))) != CC_NOERROR)
	return(rc);

    _ASSERTE(*ccache_ptr);
    return(CC_NOERROR);
};

/*
**	cc_destroy
*/
extern "C"
cc_int32 cc_destroy(apiCB* cc_ctx, ccache_p** ccache_ptr)
{
    cc_int32 rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (ccache_ptr == NULL)
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->RemoveNamedCache(reinterpret_cast<int*>(ccache_ptr));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/* ----------------------------------------------------------------------------
**  named caches (NCs) : create/destroy, open/close, 
**                 get_principal, cc_get_cred_version, & lock_request
** ----------------------------------------------------------------------------
*/
/*
**	cc_lock_request
*/
extern "C"
cc_int32 cc_lock_request(apiCB* cc_ctx, const ccache_p* ccache_ptr, const cc_int32 lock_type)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (lock_type == 1)
    {
	reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    }
    else if ((lock_type == 2) || (lock_type == 3))
    {
	reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    }

    //TODO: implement lock_request
    return(CC_NOERROR);
};

/* ----------------------------------------------------------------------------
**  named caches (NCs) : iteration and information functions
**                seq_fetch_NCs, get_name, get_principal, & cc_get_cred_version
** ----------------------------------------------------------------------------
*/
/*
** cc_seq_fetch_NCs
**
** effectively does sequential cc_opens on one NC after another
** caller must call cc_close() for each ccache_ptr returned.
** (as with seq_fetch_creds & free_creds)
*/
extern "C"
cc_int32 cc_seq_fetch_NCs_begin(apiCB* cc_ctx, ccache_cit** itNCs)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (itNCs == NULL)
	return(CC_BAD_PARM);

    (*itNCs) = 0;
    return(CC_NOERROR);
};

extern "C"
cc_int32 cc_seq_fetch_NCs_end(apiCB* cc_ctx, ccache_cit** itNCs)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (itNCs == NULL)
	return(CC_BAD_PARM);

    (*itNCs) = 0;
    return(CC_NOERROR);
};

extern "C"
cc_int32 cc_seq_fetch_NCs_next(apiCB* cc_ctx, ccache_p** ccache_ptr, ccache_cit* itNCs)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if ((ccache_ptr == NULL) || (itNCs == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->SeqFetchNCs(reinterpret_cast<int*>(ccache_ptr), reinterpret_cast<int*>(&itNCs));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

extern "C"
cc_int32 cc_seq_fetch_NCs(apiCB* cc_ctx, ccache_p** ccache_ptr, ccache_cit** itNCs)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if ((ccache_ptr == NULL) || (itNCs == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->SeqFetchNCs(reinterpret_cast<int*>(ccache_ptr), reinterpret_cast<int*>(itNCs));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
** cc_get_name : return name of NC whose ccache_ptr you passed
*/
extern "C"
cc_int32 cc_get_name(apiCB* cc_ctx, const ccache_p* ccache_ptr, char** name)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if ((ccache_ptr == NULL) || (name == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->GetName(reinterpret_cast<const int>(ccache_ptr), name);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
}

/*
**	cc_get_cred_version
*/
extern "C"
cc_int32 cc_get_cred_version(apiCB* cc_ctx, const ccache_p* ccache_ptr, cc_int32* vers)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);
	
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (ccache_ptr == NULL)
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->GetCredVersion(reinterpret_cast<const int>(ccache_ptr), vers);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
}
 

/* ----------------------------------------------------------------------------
**	credentials : store, remove, & seq_fetch_creds
** ----------------------------------------------------------------------------
*/
/*
**	cc_store
*/
extern "C"
cc_int32 cc_store(apiCB* cc_ctx, ccache_p* ccache_ptr, const cred_union cred)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (ccache_ptr == NULL)
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->StoreCredential(reinterpret_cast<const int>(ccache_ptr), cred);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
**	cc_remove_cred
*/
extern "C" 
cc_int32 cc_remove_cred(apiCB* cc_ctx, ccache_p* ccache_ptr, const cred_union cred)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (ccache_ptr == NULL)
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->RemoveCredential(reinterpret_cast<const int>(ccache_ptr), cred);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/*
**	cc_seq_fetch_creds_begin
*/
extern "C"
cc_int32 cc_seq_fetch_creds_begin(apiCB* cc_ctx, const ccache_p* ccache_ptr, ccache_cit** itCreds)
{
    ITERATOR	*iterator;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    if ((ccache_ptr == NULL) || (itCreds == NULL))
	return(CC_BAD_PARM);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    iterator = (ITERATOR *)calloc(1, sizeof(ITERATOR));
    iterator->itCreds = 0;
    iterator->ccache_ptr = (ccache_p *)ccache_ptr;
    (*itCreds) = (ccache_cit *)iterator;

    return(CC_NOERROR);
};

/*
**	cc_seq_fetch_creds_end
*/
extern "C"
cc_int32 cc_seq_fetch_creds_end(apiCB* cc_ctx, ccache_cit** itCreds)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);
    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (itCreds == NULL)
	return(CC_BAD_PARM);

    if (*itCreds != NULL)
	free(*itCreds);
    *itCreds = NULL;

    return(CC_NOERROR);
};

/*
**	cc_seq_fetch_creds_next
*/
extern "C"
cc_int32 cc_seq_fetch_creds_next(apiCB* cc_ctx, cred_union** cred, ccache_cit* itCreds)
{
    ITERATOR	*iterator;
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    iterator = (ITERATOR *)itCreds;

    if ((itCreds == NULL) || (cred == NULL))
	return(CC_BAD_PARM);

    if (iterator->ccache_ptr == NULL)
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->SeqFetchCreds(reinterpret_cast<const int>(iterator->ccache_ptr), cred, (int *)&iterator->itCreds);
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
}

/*
**	cc_seq_fetch_creds
*/
extern "C"
cc_int32 cc_seq_fetch_creds(apiCB* cc_ctx, const ccache_p* ccache_ptr, cred_union** cred, ccache_cit** itCreds)
{
    cc_int32	rc;

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if ((ccache_ptr == NULL) || (itCreds == NULL) || (cred == NULL))
	return(CC_BAD_PARM);

    reinterpret_cast<CCache_ctx*>(cc_ctx)->BeginExclusiveUse();
    rc = reinterpret_cast<CCache_ctx*>(cc_ctx)->SeqFetchCreds(reinterpret_cast<const int>(ccache_ptr), cred, reinterpret_cast<int*>(itCreds));
    reinterpret_cast<CCache_ctx*>(cc_ctx)->EndExclusiveUse();
    return(rc);
};

/* ----------------------------------------------------------------------------
**	Liberation Routines : free stuff (and null what pointed to it)
** ----------------------------------------------------------------------------
*/
/*
**	cc_free_principal (aquired via cc_get_principal())
*/
extern "C"
cc_int32 cc_free_principal(apiCB* cc_ctx, char** principal)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (principal != NULL)
    {
	if (*principal)			// make freeing multiple times safe
	    free(*principal);
	*principal = NULL;
    }
    return(CC_NOERROR);
};

/*
**	cc_free_name (aquired vie cc_get_name())
*/
extern "C"
cc_int32 cc_free_name(apiCB* cc_ctx, char** name)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

//	_ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (name != NULL)
    {
	if (*name)			// make freeing multiple times safe
	    free(*name);
	*name = NULL;
    }
    return(CC_NOERROR);
};

/*
**	cc_free_creds
*/
extern "C"
cc_int32 cc_free_creds(apiCB* cc_ctx, cred_union **pCred)
{

    if (NULL == cc_ctx)
	return(CC_NO_EXIST);

    _ASSERTE(_CrtIsValidPointer(cc_ctx, sizeof(CCache_ctx), TRUE));

    if (pCred == NULL)
	return(CC_NOERROR);

    if ((*pCred)->cred_type == CC_CRED_V5)
    {
	reinterpret_cast<CNamedCache*>(cc_ctx)->FreeCredential((*pCred)->cred.pV5Cred);
	if ((*pCred)->cred.pV5Cred)
	    free((*pCred)->cred.pV5Cred);
	(*pCred)->cred.pV5Cred = NULL;
	if (*pCred)
	    free(*pCred);
	(*pCred) = NULL;
    }
    else if ((*pCred)->cred_type == CC_CRED_V4)
    {
	if ((*pCred)->cred.pV4Cred)
	    free((*pCred)->cred.pV4Cred);
	(*pCred)->cred.pV4Cred = NULL;
	free(*pCred);
    }
    return(CC_NOERROR);
};
