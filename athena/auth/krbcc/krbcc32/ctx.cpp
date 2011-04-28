/*
** $Id: ctx.cpp,v 1.12 1999/08/02 21:56:13 dalmeida Exp $
**
** Copyright 1997 by the Regents of the University of Michigan
**
** For copying and distribution information, please see the file
** <UMCopyRt.h>.
**
*/
/*
** Ctx.cpp
**
*/
#include "UmCopyRt.h"
#include <crtdbg.h>
#include "creds.h"
#include <time.h>

#include "UmCopyRt.h"
#include "creds.h"
#include <crtdbg.h>

#ifdef MYAPPHASFLEAS

#include <regstr.h>

#endif

/* _open_cache - An internal routine to open the the ticket cache.
 *
 * lpHand - A pointer to the file handle.
 * lpvPtr - A file pointer returned.
 */
static int _open_cache(LPHANDLE lpHand, LPVOID FAR *lpvPtr)
{
	
    if ((*lpvPtr = MapViewOfFile(*lpHand,(FILE_MAP_READ | FILE_MAP_WRITE),0,0,0)) == (LPVOID)0)
    {
        CloseHandle(*lpHand);
        *lpHand = (HANDLE)0;
        return(CC_NOMEM);
    }
    return(CC_NOERROR);
}

static int OpenFileMap(LPHANDLE lpHand)
{
    if ((*lpHand = OpenFileMappingA((FILE_MAP_READ | FILE_MAP_WRITE),FALSE,KRB_TICKET_FILE)) == (HANDLE)0)
        return(CC_NOTFOUND);

    return(CC_NOERROR);
}

static int OpenViewOfMappedFile(HANDLE Hand, LPHANDLE lpvPtr)
{
    if ((*lpvPtr = MapViewOfFile(Hand,(FILE_MAP_READ | FILE_MAP_WRITE),0,0,0)) == (LPVOID)0)
        return(CC_NOMEM);
    return(CC_NOERROR);
}

static int CloseViewOfMappedFile(LPVOID FAR *lpvPtr)
{
    UnmapViewOfFile(*lpvPtr);
    *lpvPtr = (LPVOID)0;
    return(CC_NOERROR);
}

static int CloseFileMap(LPHANDLE lpHand)
{
    CloseHandle(*lpHand);
    *lpHand = (HANDLE)0;
    return(CC_NOERROR);
}

/* _close_cache - (internal) This will close the ticket cache.
 *
 * lpHand - The cache handle.
 * lpvPtr - The file pointer.
 */

static void _close_cache(LPHANDLE lpHand, LPVOID FAR *lpvPtr)
{

    UnmapViewOfFile(*lpvPtr);
    *lpvPtr = (LPVOID)0;
}


/* KrbCreateCache - (internal) 
 * This will create the Primary ticket cache which contains Named Caches, which contain credentials
 *
 * dwNumTickets - The maximum number of tickets the cache will hold.
 * lpTktHandle  - The resulting ticket cache handle.
 */

static int KrbCreateCache(DWORD dwNumTickets, LPHANDLE lpTktHandle)
{
    HANDLE					hTktFile = NULL;
    LPVOID					lpTktFilePtr = NULL;
    LPCTX					lpCtx = NULL;
    DWORD					dwParaSZ = 0, dwTmp = 0;
    HKEY					keyKrb = NULL;
    SECURITY_ATTRIBUTES SA;

    *lpTktHandle = 0;

    SA.nLength = sizeof(SA);
    SA.lpSecurityDescriptor = 0;
    SA.bInheritHandle = TRUE;

    if ((hTktFile = CreateFileMappingA(INVALID_HANDLE_VALUE, &SA, 
                                       PAGE_READWRITE, 0, 
                                       MEM_MAPPED_FILE_SIZE,
                                       KRB_TICKET_FILE)) == (HANDLE)0)
        return(CC_NOMEM);

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        //_KRB_ERROR_FLAG = KRB_ERR_NONE;  /* This is really not an error. */
        /*TODO: This is an error here, Doc says we'll destroy previous and start fresh */
        CloseHandle(hTktFile);
        return(CC_NOERROR);
    }
	
    if ((lpTktFilePtr = MapViewOfFile(hTktFile, 
                                      (FILE_MAP_READ | FILE_MAP_WRITE),
                                      0, 0, 0)) == (LPVOID)0)
    {
        CloseHandle(hTktFile);
        return(CC_NOMEM);
    }
	
    lpCtx = (LPCTX) lpTktFilePtr;
    memset(lpCtx, NULL, sizeof(_ctx));
    lpCtx->Hd.CacheSize = sizeof(struct _ctx);
    lpCtx->Hd.CacheTimeStamp  = (LONG) time((time_t FAR *)0);
 
    lpCtx->NCList.iNamedCaches = 0;
    lpCtx->NCList.iMaxNamedCaches = MAX_NCS;

    for (int i = 0; i < MAX_NCS; i++)
        lpCtx->NCList.NC[i].iMaxCredsInNC = MAX_CREDS_IN_NC;

    UnmapViewOfFile(lpTktFilePtr);
	
    *lpTktHandle = hTktFile;
    return(CC_NOERROR);
};


#ifdef OPEN_ATTACH_ORIGINAL
/* KrbReleaseCache - This will close the passed handle to the ticket cache.
 *  (internal)       when the last open handle is closed the cache should
 *                   dissapear.
 *
 * lpTktHandle - The ticket cache handle.
 */

static int KrbReleaseCache(LPHANDLE lpTktHandle, CMutex* pMutex)
{
    if (*lpTktHandle != NULL)
        CloseHandle(*lpTktHandle)
	
            *lpTktHandle = (HANDLE)0;
    return(CC_NOERROR);
}
#endif // OPEN_ATTACH_ORIGINAL
/*
** This is THE Primary Class (pointed to by cc_ctx)
** which contains a pointer to THE Cache Control Structure
** allocated by cc_initialize, deleted by cc_shutdown
** One per customer (calling App).
*/
/*
** constructor & destructor
*/
CCache_ctx::CCache_ctx()
{
    int		rc = CC_NOERROR;
    LPCTX	ptr;

    ptr = NULL; //MineField
    m_hTktFile = NULL;
//    m_lpFilePtr = NULL;
    m_hFileMapping = NULL;
    HANDLE hViewOfMappedFile = NULL;
    m_pCacheMutex = NULL;
    m_pExclusiveMutex = NULL;
    m_pCacheLock = NULL;

    m_pCacheMutex = new CMutex(FALSE, MUTEX_NAME, NULL);
    _ASSERTE(m_pCacheMutex);
    m_pExclusiveMutex = new CMutex(FALSE, EX_MUTEX_NAME, NULL);
    _ASSERTE(m_pExclusiveMutex);

    m_pCacheCriticalSection = new CCriticalSection();
    _ASSERTE(m_pCacheCriticalSection);

    m_pCacheLock = new CSingleLock(m_pCacheMutex, FALSE);
    _ASSERTE(m_pCacheLock);

    // If cache exists, we can attach, if not, create it
    // Must Keep it open one way or the other, 
    // else it will disappear out from under us when the first task exits
    if (CC_NOERROR == OpenFileMap(&m_hFileMapping))
    {
        if (CC_NOERROR == OpenViewOfMappedFile(m_hFileMapping, 
                                               &hViewOfMappedFile))
        {
            ptr = static_cast<struct _ctx*>(hViewOfMappedFile);
            Validate(ptr);
            ptr = NULL;
            CloseViewOfMappedFile(&hViewOfMappedFile);
            m_hTktFile = m_hFileMapping;
        }
        else
        {
            CloseFileMap(&m_hFileMapping);
        }
    }
    else
    {
		
        KrbCreateCache(0, &m_hFileMapping);
        m_hTktFile = m_hFileMapping;
    }
};

CCache_ctx::~CCache_ctx()
{

    _ASSERTE(m_pCacheLock);
    if (m_pCacheLock)
    {
        delete m_pCacheLock; // calls unlock
        m_pCacheLock = NULL;
    }

    _ASSERTE(m_pCacheMutex);
    if (m_pCacheMutex)
    {
        delete m_pCacheMutex;
        m_pCacheMutex = NULL;
    }

    _ASSERTE(m_pExclusiveMutex);
    if (m_pExclusiveMutex)
    {
        delete m_pExclusiveMutex;
        m_pExclusiveMutex = NULL;
    }

    if (m_pCacheCriticalSection)
    {
        delete m_pCacheCriticalSection;
        m_pCacheCriticalSection = NULL;
    }

    if (m_hFileMapping) 
        CloseFileMap(&m_hFileMapping);
    _ASSERTE(NULL == m_hFileMapping);
}


/*
** Cache Mutex and Lock operations
*/

#define LOCK_TIMEOUT INFINITE
// XXX - We assume we did not timeout.

int CCache_ctx::BeginExclusiveUse()
{
    BOOL    rc;

    _ASSERTE(m_pCacheMutex);
    _ASSERTE(m_pCacheLock);

    rc = m_pCacheCriticalSection->Lock(LOCK_TIMEOUT);
    rc = m_pCacheLock->Lock(LOCK_TIMEOUT);
    return(CC_NOERROR);

}

int CCache_ctx::EndExclusiveUse()
{
    BOOL    rc;

    _ASSERTE(m_pCacheLock);
    _ASSERTE(m_pCacheMutex);

    rc = m_pCacheLock->Unlock();
    rc = m_pCacheCriticalSection->Unlock();
    return(CC_NOERROR);
}

/*
** utility routines : Validate, PunchTimeClock, GetCacheChangeTime
*/
#ifndef _DEBUG
inline int CCache_ctx::Validate(LPCTX) { return(CC_NOERROR); };	// only costs in debug build
#else
/*
** Validate : checks data structures and their values for trouble
*/
int CCache_ctx::Validate(LPCTX ptr)
{
    BOOL	rc;
    int		i;

    rc = m_pCacheLock->IsLocked();

    _ASSERTE(_CrtIsValidPointer(ptr, sizeof(_ctx), TRUE));

    _ASSERTE(m_pCacheMutex);
    _ASSERTE(m_pCacheLock);
	
    // verify that MineField is still clear
    if (ptr)
    {
        for (int i = 0; i < sizeof(ptr->MineField); i++) 
        {
            if (NULL != ptr->MineField[i])
                MessageBox(NULL, "Someone's been walking in the mine field!", "Validate _ctx", MB_ICONSTOP);
        }
    }

    CNamedCache* p_NC = NULL;
    for (i = 0; i < ptr->NCList.iMaxNamedCaches; i++)
    {
        p_NC = new CNamedCache(this, ptr->NCList.NC[i]); // attach to existing NC
        p_NC->Validate();
        delete p_NC; p_NC = NULL;
    }
    return(CC_NOERROR);
}
#endif

/*
** VerifyFlags : enforce reserved bits
*/
static int VerifyFlags(const cc_nc_flags flags)
{
    int i = 0;

    // Enforce reservation of remaining bits
    if ((i = flags & CC_FLAGS_RESERVED) != 0)
        return(CC_BAD_PARM);

    return(CC_NOERROR);
}

/*
** PunchTimeClock() : reset timestamp on cache (one timestamp for entire cache)
**                    Assumes caller has attached cache.
*/
int CCache_ctx::PunchTimeClock(LPCTX ptr)
{
    int rc = 0;

    if (ptr)
    {
        if (rc != CC_NOERROR)
            return(rc);

        ptr->Hd.CacheTimeStamp = (LONG) time((time_t FAR *)0);
        Validate(ptr);
    }
    return(CC_NOERROR);
}

/*
** GetCacheChangeTime
*/
cc_time_t CCache_ctx::GetCacheChangeTime()
{
    int			rc = 0; 
    cc_time_t	t = NULL;
    LPVOID		ptr;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (ptr)
        t = ((LPCTX)ptr)->Hd.CacheTimeStamp;

    DetachCache(&ptr);
    return(t);
}


#ifdef MYAPPHASFLEAS
/*
** SetFleaviusFlag
*/
__declspec(dllexport) int CCache_ctx::SetFleaviusProcessID(const DWORD FleaviusProcessID)
{
    int		rc = 0;
    LPVOID	ptr;
    HANDLE	fHandle;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (ptr)
    {
        if (((LPCTX)ptr)->FleaviusProcessID != NULL)
        {
            fHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ((LPCTX)ptr)->FleaviusProcessID);
            if (fHandle != NULL)
            {
                CloseHandle(fHandle);
                return(CC_ERR_CACHE_ATTACH);
            }
        }

        ((LPCTX)ptr)->FleaviusProcessID = FleaviusProcessID;
        Validate((LPCTX)ptr);
    }
	
    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** ClearFleaviusFlag
*/
__declspec(dllexport) int CCache_ctx::ClearFleaviusProcessID()
{
    int	rc;

    rc = SetFleaviusProcessID(0);

    return(rc);
}


/*
** WakeUpFleavius : Fleavius sets/clears bFleaviusIsRunning
**			        Called from CreateNamedCache & OpenNamedCache.
**                  assumes caller has already attached cache
**					This is optional, failure is ignored.
*/
void CCache_ctx::WakeUpFleavius(LPCTX ptr)
{
    DWORD						rc;
    HANDLE						FleaviusHandle;
    HANDLE						StartupHandle;
    static STARTUPINFO			startit;
    static PROCESS_INFORMATION	processinfo;

    unsigned long ulValueType = NULL;
    char szBuf[REGSTR_MAX_VALUE_LENGTH+1]; // 256 in REGSTR.H at time of writing
    unsigned long nBytes = sizeof(szBuf);

    if (ptr->FleaviusProcessID != NULL)
    {
        FleaviusHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ptr->FleaviusProcessID);
        if (FleaviusHandle != NULL)
        {
            CloseHandle(FleaviusHandle);
            return;
        }
    }
//	if (ptr->bFleaviusIsRunning)
//		return;

    // Get path for fleavius from the registry so it can live anywhere user wants
    // assuming it was installed with an installer that sets this key as is recommended
    // in the Logo Handbook V3.0
    {
	CString RPU(REGSTR_PATH_UNINSTALL "\\Fleavius");

	HKEY Hkey = NULL;
	strcpy(szBuf, "Fleavius.exe");
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, LPCTSTR(RPU), (DWORD)0, KEY_READ, &Hkey))
        {
            rc = RegQueryValueEx(Hkey, "InstallLocation", NULL, &ulValueType, reinterpret_cast<unsigned char*>(szBuf), &nBytes);
            RegCloseKey(Hkey);
            Hkey = NULL;
            if (rc == ERROR_SUCCESS)
            {
                if (REG_EXPAND_SZ == ulValueType)
                {
                    CString oldBuf(szBuf);
                    if (!ExpandEnvironmentStrings(LPCSTR(oldBuf), static_cast<char*>(szBuf), nBytes))
                    {
                        strcpy(szBuf, "Fleavius.exe");
                    }
                    else
                    {
                        strcat(szBuf, "\\");
                        strcat(szBuf, "Fleavius.exe");
                    }
                } 
                else if (REG_SZ != ulValueType)
                {
                    strcpy(szBuf, "Fleavius.exe");
                }
                else if (0 == strlen(static_cast<const char*>(szBuf)))
                {
                    strcpy(szBuf, "fleavius.exe");
                }
            }
            else
                strcpy(szBuf, "fleavius.exe");
        }
    }

    StartupHandle = CreateEvent(NULL, FALSE, FALSE, "FleaviusStartUp");

    GetStartupInfo(&startit);
    if (!_stricmp(szBuf, "fleavius.exe"))
        rc = CreateProcess(NULL, szBuf, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startit, &processinfo);
    else
        rc = CreateProcess(szBuf, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startit, &processinfo);
    if (rc)
    {
        CloseHandle(processinfo.hThread);
        CloseHandle(processinfo.hProcess);
        if (StartupHandle != NULL)
        {
            rc = WaitForSingleObject(StartupHandle, 1500);
            CloseHandle(StartupHandle);
        }
    }
    return;
}
#endif // MYAPPHASFLEAS

/*
** GetNCInfo
**
** This is a higher level utility routine that returns a null terminated list of pointers to
** _infoNC structs.  This provides the caller with the distinguishing information for all NCs
** in one data structure.  You can get the same information via seq_fetch_NCs and get_name, get_principal,
** & get_cred_version.
*/
int CCache_ctx::GetNCInfo(struct _infoNC*** p_list_pNCI)
{
    int		rc = 0; 
    int		nItems = 0;
    LPVOID	ptr;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    _ASSERTE(NULL == *p_list_pNCI);

    // allocate enough room for max + 1, to avoid running off the end if someone adds an NC
    // after we allocate space and before we finish filling it
    if ((*p_list_pNCI = (struct _infoNC**)calloc(((LPCTX)(ptr))->NCList.iMaxNamedCaches + 1, sizeof(struct _infoNC*))) == NULL)
    {
        DetachCache(&ptr);
        return(CC_NOMEM);
    }

    struct _infoNC** list_pNCI = *p_list_pNCI;

    for (int i = 0; i < ((LPCTX)(ptr))->NCList.iMaxNamedCaches; i++)
    {
        if (!((LPCTX)(ptr))->NCList.NC[i].bInUse) 
            continue;

        *list_pNCI = (struct _infoNC*)calloc(1, sizeof(struct _infoNC));
        struct _infoNC*  pNCI = *list_pNCI;

        if (pNCI == NULL)
        {
            cc_free_NC_info(reinterpret_cast<apiCB*>(this), p_list_pNCI);
            DetachCache(&ptr);
            return(CC_NOMEM);
        }

        pNCI->name = (char*)malloc(strlen(((LPCTX)(ptr))->NCList.NC[i].CacheName) + 1);
        if (NULL == pNCI->name)
        {
            cc_free_NC_info(reinterpret_cast<apiCB*>(this), p_list_pNCI);
            DetachCache(&ptr);
            return(CC_NOMEM);
        }
        strcpy(pNCI->name, ((LPCTX)(ptr))->NCList.NC[i].CacheName);

        pNCI->vers = ((LPCTX)(ptr))->NCList.NC[i].credVersion;

        list_pNCI++;	// ready for next, if there is one
        nItems++;		// keep count
    }

    // If our list is empty, free allocated mem and return CC_NO_EXIST
    if (0 == nItems)
    {
        cc_free_NC_info(reinterpret_cast<apiCB*>(this), p_list_pNCI);
        DetachCache(&ptr);
        return(CC_NO_EXIST);
    }

    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** wrappers for _open_cache & _close_cache
*/
/*
** AttachCache
*/
int CCache_ctx::AttachCache(LPVOID *ptr)
{
    int rc = 0;

    if ((rc = _open_cache(&m_hTktFile, ptr)) != CC_NOERROR)
        return(rc);

    Validate((LPCTX)*ptr); // only costly if _DEBUG
    return(CC_NOERROR);
}

/*
** DetachCache
*/
void CCache_ctx::DetachCache(LPVOID *ptr)
{
    if (NULL != m_hTktFile || NULL != ptr)
    {
        Validate((LPCTX)*ptr);
        _close_cache(&m_hTktFile,ptr);
    }
    (*ptr) = NULL;
}

/*
**	CreateNamedCache:
**
**	Validate flags here because constructor can't return an error.
*/
int CCache_ctx::CreateNamedCache(const char* name, const char* principal, cc_int32 vers, cc_nc_flags flags, int* ccache_ptr)
{	
    int		rc = 0; 
    LPVOID	ptr;

    *ccache_ptr = NULL;	// paranoia strikes deep ... 

    if ((rc = VerifyFlags(flags)) != CC_NOERROR)
        return(rc);

    // Fetch me the cache please
    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

#ifdef MYAPPHASFLEAS
    WakeUpFleavius((LPCTX)ptr);
#endif
	
    // Room for one more named cache?
    if (((LPCTX)(ptr))->NCList.iMaxNamedCaches <= ((LPCTX)(ptr))->NCList.iNamedCaches)
    {
        DetachCache(&ptr);
        return(CC_ERR_CACHE_FULL);
    }

    { // Does named cache already exist?
	int tmp = 0;
	if (CC_NOERROR == FindNamedCache(name, vers, flags, &tmp, (LPCTX)ptr))
        {
            DetachCache(&ptr);
            return(CC_BADNAME);
            //TODO: Ted's doc says I'm supposed to zap the old and start fresh if same name exists
            // seems like either 1) there ought to be a boolean bKillOK pass here, or
            // 2) there's a remove call, let them use it ...
        }
    }

    // find an NC slot that's not in use

    int idx = 0;
    while (((LPCTX)(ptr))->NCList.NC[idx].bInUse && idx < ((LPCTX)(ptr))->NCList.iMaxNamedCaches)
        idx++;

    _ASSERTE(idx < ((LPCTX)(ptr))->NCList.iMaxNamedCaches);

    if (rc != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    CNamedCache _nc(this, ((LPCTX)(ptr))->NCList.NC[idx], name, principal, vers, flags); // initialize via constructor, destroyed on scope exit

    rc = PunchTimeClock((LPCTX)ptr);

    _ASSERTE(rc == CC_NOERROR);
	
    *ccache_ptr = MakeCCachePtr(idx);	// keep an accurate count
    ((LPCTX)(ptr))->NCList.iNamedCaches++;	// keep an accurate count
	
    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** FindNamedCache (private)
** Assumes there won't be multiple named caches with same name (i.e. stops at first match found)
*/
int CCache_ctx::FindNamedCache(const char* name, const cc_int32 vers, const cc_nc_flags flags, int* ccache_ptr, LPCTX ptr)
{
    int rc = 0; 

    _ASSERTE(ptr);

    if ((rc = VerifyFlags(flags)) != CC_NOERROR)
        return(rc);
	
    for (int i = 0; i < (ptr)->NCList.iMaxNamedCaches; i++)
    {
        if (0 == strcmp(name, ptr->NCList.NC[i].CacheName))
        {
            // Cache Names match, verify version
            if (vers == ptr->NCList.NC[i].credVersion)
            {
                *ccache_ptr = i;
                return(CC_NOERROR);
            }
        }
    }
	
    return(CC_NOTFOUND);
}

/*
** OpenNamedCache
*/
int CCache_ctx::OpenNamedCache(const char* name, const cc_int32 vers, const cc_nc_flags cc_flags, int* ccache_ptr)
{
    int		rc = 0; 
    int		tptr;
    LPVOID	ptr;

    tptr = 0;

//	*ccache_ptr = NULL;	// paranoia strikes deep ...

    if ((rc = VerifyFlags(cc_flags)) != CC_NOERROR)
        return(rc);

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

#ifdef MYAPPHASFLEAS
    WakeUpFleavius((LPCTX)ptr);
#endif
	
    // search through named caches looking for <name>
    if ((rc = FindNamedCache(name, vers, cc_flags, &tptr, (LPCTX)ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }
	
    // if found, give caller a ccache_ptr to use
    tptr = MakeCCachePtr(tptr);
    DetachCache(&ptr);
    *ccache_ptr = tptr;
    return(CC_NOERROR);
}

/*
** RemoveNamedCache
*/
int CCache_ctx::RemoveNamedCache(int* ccache_ptr)
{
    int		rc = 0; 
    LPVOID	ptr;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (NULL == *ccache_ptr
        || (rc = MakeIndex(*ccache_ptr)) > ((LPCTX)(ptr))->NCList.iMaxNamedCaches 
        || (rc = MakeIndex(*ccache_ptr)) < 0)
    {
        DetachCache(&ptr);
        return(CC_NO_EXIST);
    }

    CNamedCache _nc(this, ((LPCTX)(ptr))->NCList.NC[MakeIndex(*ccache_ptr)]);
    rc = _nc.Reinitialize();
    if (rc != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    PunchTimeClock((LPCTX)ptr);
    ((LPCTX)(ptr))->NCList.iNamedCaches--;
    DetachCache(&ptr);	
    return(CC_NOERROR);
}

/*
** SeqFetchNCs
**
** returns ccache_ptr to next NC which must be freed via cc_close()
** caller should set itNCs to NULL before first call
** at EOSequence ccache_ptr and itNCs will both be NULL and CC_END will be returned
** 
*/
int CCache_ctx::SeqFetchNCs(int* ccache_ptr, int* itNCs)
{
    int		rc = 0;
    LPVOID	ptr;

    *ccache_ptr = NULL;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    // Find next slot in use in our possibly discontiguous list
    while (*itNCs < ((LPCTX)(ptr))->NCList.iMaxNamedCaches 
           && !((LPCTX)(ptr))->NCList.NC[*itNCs].bInUse)
        (*itNCs)++;
	
    // Did we walk the plank?
    if (*itNCs == ((LPCTX)(ptr))->NCList.iMaxNamedCaches)
    {
        DetachCache(&ptr);
        *itNCs = 0;
        return(CC_END);			// no more NCs found
    }
		
    _ASSERT(((LPCTX)(ptr))->NCList.NC[*itNCs].bInUse);

    // Looks simple, but this is the point of this function
    // Pass user a non-zero indicator of next cache in use, 
    *ccache_ptr = MakeCCachePtr(*itNCs);	
    (*itNCs)++;

    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** GetName : returns name of NC associated with ccache_ptr
*/
int CCache_ctx::GetName(const int ccache_ptr, char** name)
{
    int		rc = 0; 
    LPVOID	ptr;

    _ASSERT(name);

    if (NULL == ccache_ptr)
        return(CC_NO_EXIST);

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    _ASSERT(((LPCTX)(ptr)));
    _ASSERT(((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)].bInUse);
	
    *name = (char*)malloc(strlen(((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)].CacheName) + 1);
    if (NULL == *name)
    {
        DetachCache(&ptr);
        return(CC_NOMEM);
    }
	
    strcpy(*name, ((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)].CacheName);
    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** SetPrincipal
**
** set NC's principal
*/
int CCache_ctx::SetPrincipal(const int ccache_ptr, const char * principal)
{
    int rc = CC_NOERROR;
    LPCTX ptr = 0;
    char* prince;

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
        goto cleanup;

    _ASSERTE(ptr);
    
    if (MakeIndex(ccache_ptr) < MIN_KOSHER_HANDLE || 
        MakeIndex(ccache_ptr) > MAX_KOSHER_HANDLE || 
        !ptr->NCList.NC[MakeIndex(ccache_ptr)].bInUse)
    {
        rc = CC_NO_EXIST;
        goto cleanup;
    }
    prince = ptr->NCList.NC[MakeIndex(ccache_ptr)].Principal;
    prince[0] = 0;
    strncpy(prince, principal, MAX_PRINCIPAL_SZ);
    prince[MAX_PRINCIPAL_SZ - 1] = 0;
    rc = CC_NOERROR;
 cleanup:
    if (ptr)
        DetachCache(&ptr);
    return(rc);
}

/*
** GetPrincipal
**
** returns char* principal (read only!) which must be freed via cc_free_principal
*/
int CCache_ctx::GetPrincipal(const int ccache_ptr, char** principal)
{
    int rc = CC_NOERROR;
    LPCTX ptr = 0;
    char* prince;

    _ASSERT(principal);
    _ASSERT(NULL == *principal); // XXX: why???!!!  We don't care!

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
        goto cleanup;
	
    _ASSERT(ptr);

    if (MakeIndex(ccache_ptr) < MIN_KOSHER_HANDLE || 
        MakeIndex(ccache_ptr) > MAX_KOSHER_HANDLE || 
        !ptr->NCList.NC[MakeIndex(ccache_ptr)].bInUse)
    {
        rc = CC_NO_EXIST;
        goto cleanup;
    }
    prince = ptr->NCList.NC[MakeIndex(ccache_ptr)].Principal;
    *principal = (char*)calloc(1, strlen(prince) + 1);
    if (NULL == *principal)
    {
        rc = CC_NOMEM;
        goto cleanup;
    }
    strcpy(*principal, prince);
    rc = CC_NOERROR;
 cleanup:
    if (ptr)
        DetachCache(&ptr);
    return(rc);
}

/*
** GetCredVersion
**
*/
int CCache_ctx::GetCredVersion(const int ccache_ptr, cc_int32* vers)
{
    int		rc = 0; 
    LPVOID	ptr;

    if (NULL == ccache_ptr)
        return(CC_NO_EXIST);

    if ((rc = AttachCache(&ptr)) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }
	
    _ASSERT(((LPCTX)(ptr)));
    *vers = ((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)].credVersion;
	
    DetachCache(&ptr);
    return(CC_NOERROR);
}

/*
** CloseNamedCache
**
*/
int CCache_ctx::CloseNamedCache(int* ccache_ptr)
{
//	int	rc;

    // Not sure what needs to be done here
    // I've already closed & detached the cache 
    // all the user has is and "index" into the list of Named Caches
    // I can Null it for them :-)

    if (NULL == *ccache_ptr)
        return(CC_NO_EXIST);

    *ccache_ptr = NULL;
    return(CC_NOERROR);
}


/*
** StoreCredential
*/
int CCache_ctx::StoreCredential(const int ccache_ptr, const cred_union cred)
{
    cc_int32	rc = 0;
    LPVOID		ptr;

    if (NULL == ccache_ptr)
        return(CC_NO_EXIST);

    if (rc = AttachCache(&ptr) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (MakeIndex(ccache_ptr) < MIN_KOSHER_HANDLE || MakeIndex(ccache_ptr) > MAX_KOSHER_HANDLE)
    {
        DetachCache(&ptr);
        return(CC_NO_EXIST);
    }

    CNamedCache NC(this, ((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)]);
    rc = NC.StoreCred(cred);
	
    if (CC_NOERROR == rc)
        PunchTimeClock((LPCTX)ptr);

    DetachCache(&ptr);
    return(rc);
}

/*
** RemoveCredential
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CCache_ctx::RemoveCredential(const int ccache_ptr, const cred_union cred)
{
    int		rc = 0;
    LPVOID	ptr;

    if (NULL == ccache_ptr)
        return(CC_NO_EXIST);

    if (rc = AttachCache(&ptr) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (MakeIndex(ccache_ptr) < MIN_KOSHER_HANDLE || MakeIndex(ccache_ptr) > MAX_KOSHER_HANDLE)
    {
        DetachCache(&ptr);
        return(CC_NO_EXIST);
    }

    CNamedCache NC(this, ((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)]);
    rc = NC.RemoveCred(cred);

    if (CC_NOERROR == rc)
        PunchTimeClock((LPCTX)ptr);

    DetachCache(&ptr);
    return(rc);
}

/*
** SeqFetchCreds
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CCache_ctx::SeqFetchCredsBegin()
{

    return(CC_NOERROR);
}

/*
** SeqFetchCreds
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CCache_ctx::SeqFetchCredsEnd()
{

    return(CC_NOERROR);
}

/*
** SeqFetchCreds
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CCache_ctx::SeqFetchCreds(const int ccache_ptr, cred_union** creds, int* itCreds)
{
    int		rc = 0;
    LPVOID	ptr;

    if (NULL == ccache_ptr)
        return(CC_NO_EXIST);

    if (rc = AttachCache(&ptr) != CC_NOERROR)
    {
        DetachCache(&ptr);
        return(rc);
    }

    if (MakeIndex(ccache_ptr) < MIN_KOSHER_HANDLE || MakeIndex(ccache_ptr) > MAX_KOSHER_HANDLE)
    {
        DetachCache(&ptr);
        return(CC_NO_EXIST);
    }

    CNamedCache NC(this, ((LPCTX)(ptr))->NCList.NC[MakeIndex(ccache_ptr)]);
    rc = NC.SeqFetchCreds(creds, itCreds);

    DetachCache(&ptr);
    return(rc);
}
