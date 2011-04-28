/* 
** $Id: creds.h,v 1.10 1999/06/23 17:08:18 dtanner Exp $
**
** Copyright 1997 by the Regents of the University of Michigan
**
** For copying and distribution information, please see the file
** <UMCopyRt.h>.
**
*/
/*
**
** creds.h:
**	This file should be INVISIBLE to external users.  This is
**	how we implement what they see, but should be hidden from them
*/
//////////////////////////////////////////////////////////////////////

#include "UmCopyRt.h"
#include <afxmt.h>

#if !defined(AFX_CREDS_H__CB6823E4_E6A0_11D0_A7B3_444553540000__INCLUDED_)
#define AFX_CREDS_H__CB6823E4_E6A0_11D0_A7B3_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// enable compulsive malloc debugging
#define _CRTDBG_MAP_ALLOC

// MYAPPHASFLEAS creates a bool in the memory mapped file to be used
// by fleavius.exe to indicate that it's running (and doesn't need to
// be started).  Fleavius.exe is an auxillary app whose sole purpose
// is to prevent the MemMapped file from being unloaded if all calling
// apps exit.
#define MYAPPHASFLEAS


#include "cacheapi.h"

typedef struct {
    int        itCreds;
    ccache_p   *ccache_ptr;
} ITERATOR;

#define KRB5_CLIENT_SZ	256
#define KRB5_SERVER_SZ	256
#define KRB5_DATA_SZ	1024
#define KRB5_DATA_CNT	20
#define MAX_THREADS     512

typedef struct _cc_data1
{
    cc_uint32       type;		// should be one of _cc_data_type
    cc_uint32       length;
    unsigned char   data[KRB5_DATA_SZ];	// the proverbial bag-o-bits
} cc_data1;

typedef struct _cc_cache_creds {
    char            client[KRB5_CLIENT_SZ];
    char            server[KRB5_SERVER_SZ];
    cc_data1        keyblock;
    cc_time_t       authtime;
    cc_time_t       starttime;
    cc_time_t       endtime;
    cc_time_t       renew_till;
    cc_int32        is_skey;
    cc_int32        ticket_flags;
    cc_data1        addresses[KRB5_DATA_CNT];
    cc_data1        ticket;
    cc_data1        second_ticket;
    cc_data1        authdata[KRB5_DATA_CNT];
} cc_cache_creds;


typedef struct _ctx FAR *LPCTX;
struct _NC;						/* defined below */


// In various internal places we use a simple array index for what the
// user calls a "ccache_ptr" or "cursor" or "itCreds".  Caller sets
// these to NULL before first call, and we return NULL to indicate end
// of sequence. Zero is a reasonable index, so we need a couple
// routines to convert between external ccache_ptr's and internal
// indexes.

// convert external to internal
static inline int MakeIndex(const int i) { return i - 1; }
// convert internal to external
static inline int MakeCCachePtr(const int i) { return i + 1; }

class CCache_ctx;
#ifdef MYAPPHASFLEAS
class CFleaDlg;					/* our friend fleavius */
#endif /* MYAPPHASFLEAS */


class CNamedCacheList
{
public:
    CNamedCacheList();
    ~CNamedCacheList();
    int Validate();

//private:
    struct _NCL* m_pNCL; // must be pointer to work with memory_mapped file
};

class CNamedCache
{
    // Within the primary cache (ctx) are named caches (NCs) for which
    // you get ccache_ptrs
public:
    // functions:
    int FreeCredential(cc_creds *pCred);
    int StoreCred(const cred_union cred);
    int RemoveCred(const cred_union cred);
    int SeqFetchCreds(cred_union** cred, int* itCreds);

    CNamedCache(CCache_ctx* pCtx, _NC& pNC);	// attach to existing NC
    CNamedCache(CCache_ctx* pCtx, _NC& pNC, const char* name, 
		const char* principal, cc_int32 vers, 
		cc_nc_flags flags);	// initialize new NC
    ~CNamedCache();
    int Reinitialize();	// called by Remove
    int Validate();     // public so callable from CCache_ctx

private:
    // functions
    int	CopyCredentialToCache(const cc_creds *src, cc_cache_creds* dest);
    int	CopyCredentialFromCache(const cc_cache_creds *src, cc_creds* dest);
    int CopyV5Cred(const int idx, cc_creds** pCred);
    int CopyV4Cred(const int idx, V4Cred_type** pCred);
    int RemoveV5Cred(const cc_creds* pCred);
    int RemoveV4Cred(const V4Cred_type* pCred);
    int StoreV5Cred(const int idx, const _cc_creds* pCred);
    int StoreV4Cred(const int idx, const V4Cred_type* pCred);

private:
    // data
    CCache_ctx* m_pCtx; // Pointer to mother ship, so we can use mutex fcns
    struct _NC* m_pNC;  // must be pointer to work with memory_mapped file
};


class CCache_ctx
{
    // This is THE Primary Cache Control Structure (which cc_ctx points to)
    // allocated by cc_initialize, deleted by cc_shutdown
    // passed as first parm on ALL calls.
public:
    // Cache operations
    cc_time_t GetCacheChangeTime();
    int CreateNamedCache(const char* name, const char* principal, 
			 cc_int32 vers, cc_nc_flags flags, int* ccache_ptr);
    int OpenNamedCache(const char* name, const cc_int32 vers, 
		       const cc_nc_flags cc_flags, int* ccache_ptr);
    int CloseNamedCache(int* ccache_ptr);
    int RemoveNamedCache(int* ccache_ptr);
    int SeqFetchNCs(int* ccache_ptr, int* itNCs);
    int GetNCInfo(struct _infoNC*** ppNCi);
    int GetName(const int ccache_ptr, char** name);
    int GetPrincipal(const int ccache_ptr, char** principal);
    int GetCredVersion(const int ccache_ptr, cc_int32* vers);

    int EndExclusiveUse();
    int BeginExclusiveUse();

    CCache_ctx();
    ~CCache_ctx();

public:
    // Credential operations (performed within an NC)
    int SetPrincipal(const int ccache_ptr, const char* principal);
    int StoreCredential(const int ccache_ptr, const cred_union creds);
    int RemoveCredential(const int ccache_ptr, const cred_union creds);
    int SeqFetchCreds(const int ccache_ptr, cred_union** creds, int* itCreds);
    int SeqFetchCredsBegin();
    int SeqFetchCredsEnd();

private:
    // functions
    int PunchTimeClock(LPCTX);
    int Validate(LPCTX);
    int AttachCache(LPVOID *);	// get the memory mapped file
    int AttachCache(LPCTX * ptr) {
        return AttachCache((LPVOID*)ptr);
    };
    inline void DetachCache(LPVOID *);	// release the memory mapped file
    inline void DetachCache(LPCTX * ptr) {
        DetachCache((LPVOID*)ptr);
    }
    int FindNamedCache(const char* name, const cc_int32 vers, const cc_nc_flags flags, int* ccache_ptr, LPCTX);

#ifdef MYAPPHASFLEAS

private:
    // functions called by our friend fleavius.exe
    void WakeUpFleavius(LPCTX);

    friend CFleaDlg;
    __declspec(dllexport) int SetFleaviusProcessID(const DWORD);
    __declspec(dllexport) int ClearFleaviusProcessID();

#endif /* MYAPPHASFLEAS */

private:
    // data
    HANDLE m_hFileMapping;
    // Implemented as a pointer to a struct so the struct can
    // easily live in a memory mapped file
//    LPCTX m_pCtx;
    // ptr to THE primary data structure, aka the memory mapped file
    HANDLE m_hTktFile;
    // handle (via KrbCreateCache) to "ticket file"...  don't think of
    // it as a file though!
//    LPVOID m_lpFilePtr;
    // ptr to memory (as in memory mapped) where our shared data
    // structure resides content same as m_pCtx, but types differ
    CMutex* m_pCacheMutex;     // Mutex object to avoid simultaneous writes
    CMutex* m_pExclusiveMutex;     // Mutex object to avoid simultaneous writes
    CCriticalSection* m_pCacheCriticalSection;
    CSingleLock* m_pCacheLock; // Tool to use with mutex

};


// Must map our data structures onto THE POINTER one uses with a
// "memory mapped file" "Memory mapped file" is kind of a misnomer,
// you don't do I/O just map your dsect into the shared memory managed
// by this scheme and make assignments ...

//
// Tune these to control size/capacity of "the cache"
//
#define MAX_NCS		6		// max # of NCs in "the cache"
#define MAX_NC_NAME_SZ	40		// max length of an NCs name
#define MAX_CREDS_IN_NC	20		// max creds in an NC
#define MAX_PRINCIPAL_SZ 1024           // max princ. name we can handle
#define MEM_MAPPED_FILE_SIZE sizeof(_ctx)

#define KRB_TICKET_FILE  "krbcc32.Memory"
#define MUTEX_NAME       "krbcc32.Mutex"
#define EX_MUTEX_NAME    "EXkrbcc32.Mutex"

//
// The data structures
//
typedef union _ticket_cred_union {
    V4Cred_type V4Cred;
    cc_cache_creds V5Cred;
} ticket_cred_union;

typedef struct _NC_Ticket {
    BOOL bInUse;
    ticket_cred_union tcu;
} NC_Tkt;

struct _NC {
    BOOL bInUse;
    CHAR CacheName[MAX_NC_NAME_SZ];
    CHAR Principal[MAX_PRINCIPAL_SZ];
    cc_nc_flags iFlags;
    cc_int32 credVersion;
    int iCredsInNC;
    int iMaxCredsInNC;
    NC_Tkt CredList[MAX_CREDS_IN_NC];
};

#define MIN_KOSHER_HANDLE	0
#define MAX_KOSHER_HANDLE	MAX_NCS

struct _NCL {
    int iNamedCaches;
    int iMaxNamedCaches;
    struct _NC NC[MAX_NCS];
};

struct _cache_head {
    DWORD CacheSize;     /* Offset to EOC, */
    LONG CacheTimeStamp; /* Last change time for entire cache */
};

struct _ctx {
    _cache_head Hd;
    _NCL NCList;
#ifdef MYAPPHASFLEAS
    // Set & cleared by Fleavius.exe which holds cache in mem:
    DWORD  FleaviusProcessID;
#endif /* MYAPPHASFLEAS */
    BYTE MineField[16];	// set to NULL by KrbCreateCache
};

#endif // !defined(AFX_CREDS_H__CB6823E4_E6A0_11D0_A7B3_444553540000__INCLUDED_)
