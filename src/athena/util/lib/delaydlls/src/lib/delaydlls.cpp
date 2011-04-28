#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <delayimp.h>
#include <delaydlls.h>
#include <assert.h>

/*
 Create a linked list of dll_name, function pairs that failed...

 Instead of linked list, we can just count up the functions and
 allocate an array right away...  Then we just use enough locations
 for what fails.
*/





// NOTE: RVA = relative virtual address

template <class T> static T PointerFromRVA(PBYTE pBase, DWORD dwRVA)
{
    return (T)(pBase + dwRVA);
}

static
PBYTE
GetCurrentProcessModuleAddress()
{
    HMODULE h = GetModuleHandle(0);
    return (PBYTE) h;
}

static
void
UnloadModuleAddress(
    IN  PBYTE pAddress
    )
{
    FreeLibrary((HMODULE) pAddress);
}

static
PIMAGE_NT_HEADERS
PEHeaderFromAddress(
    IN  PBYTE pAddress
    )
{
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) pAddress;
    PIMAGE_NT_HEADERS pPEHeader = 0;

    __try
    {
        if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            __leave;
        }

        pPEHeader = (PIMAGE_NT_HEADERS) (pAddress + pDOSHeader->e_lfanew);
        if (pPEHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            pPEHeader = 0;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return pPEHeader;
}

// GetProcAddress(h, MAKEINTRESOURCE(OrdinalValue))

enum walker_func_action_e {
    WALKER_DLL_START,
    WALKER_DLL_END,
    WALKER_FUNCTION
};

typedef BOOL (* walker_func_t)(
    PVOID pContext, walker_func_action_e Action, PCSTR pszDllName,
    BOOL IsOrdinal, WORD dwOrdinal, PCSTR pszFuncName
    );

static
DWORD
walk_functions(
    IN  PIMAGE_THUNK_DATA pINT,
    IN  walker_func_t pfWalker,
    IN  PVOID pContext,
    IN  PCSTR pszDllName,
	IN  PBYTE pAddress
    )
{
    DWORD dwCount = 0;

    while (pINT->u1.AddressOfData || pINT->u1.Ordinal)
    {
        if (pfWalker)
        {
            if (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
            {
                pfWalker(pContext, WALKER_FUNCTION, pszDllName, TRUE,
                         (WORD)IMAGE_ORDINAL(pINT->u1.Ordinal), NULL);
            }
            else
            {
                PIMAGE_IMPORT_BY_NAME pImportByName =
#if defined(_MSC_VER) && (_MSC_VER < 1300 )
                    (PIMAGE_IMPORT_BY_NAME)pINT->u1.AddressOfData;
#else
                    (PIMAGE_IMPORT_BY_NAME)PointerFromRVA<PIMAGE_IMPORT_BY_NAME>(pAddress, pINT->u1.AddressOfData);
#endif
				pfWalker(pContext, WALKER_FUNCTION, pszDllName, FALSE,
                         0, (PCSTR)pImportByName->Name);
            }
        }
        pINT++;
        dwCount++;
    }

    return dwCount;
}

static
DWORD
walk_dlls(
    IN  PBYTE pAddress,
    IN  walker_func_t pfWalker,
    IN  PVOID pContext
    )
{
    PIMAGE_NT_HEADERS pPEHeader = 0;
    DWORD dwImportRVA = 0;
    PImgDelayDescr pImportDesc = 0;
    PCSTR pszDLL = 0;
    PIMAGE_THUNK_DATA pINT = 0;
    HMODULE hDLL = 0;

    __try
    {
        pPEHeader = PEHeaderFromAddress(pAddress);
        if (!pPEHeader)
        {
            return ERROR_INVALID_EXE_SIGNATURE;
        }
    
        dwImportRVA = 
            pPEHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
        if (!dwImportRVA)
        {
            return ERROR_SUCCESS;
        }

        pImportDesc = PointerFromRVA<PImgDelayDescr>(pAddress, dwImportRVA);
        if (!pImportDesc)
        {
            /* This should never happen */
            assert(pImportDesc);
            return ERROR_BAD_EXE_FORMAT;
        }
#if defined(_MSC_VER) && (_MSC_VER < 1300 )
        while (pImportDesc->szName)
        {
            pszDLL = pImportDesc->szName;
            if (pfWalker)
                pfWalker(pContext, WALKER_DLL_START, pszDLL, FALSE, 0, NULL);
            pINT = (PIMAGE_THUNK_DATA)pImportDesc->pINT;
            walk_functions(pINT, pfWalker, pContext, pszDLL, pAddress);
            if (pfWalker)
                pfWalker(pContext, WALKER_DLL_END, pszDLL, FALSE, 0, NULL);
            pImportDesc++;
        }

#else
        while (pImportDesc->rvaDLLName)
        {
            pszDLL = PointerFromRVA<LPCSTR>(pAddress, pImportDesc->rvaDLLName);
            if (pfWalker)
                pfWalker(pContext, WALKER_DLL_START, pszDLL, FALSE, 0, NULL);
            pINT = (PIMAGE_THUNK_DATA)PointerFromRVA<PCImgThunkData>(pAddress, pImportDesc->rvaINT);
            walk_functions(pINT, pfWalker, pContext, pszDLL, pAddress);
            if (pfWalker)
                pfWalker(pContext, WALKER_DLL_END, pszDLL, FALSE, 0, NULL);
            pImportDesc++;
        }

#endif

        return ERROR_SUCCESS;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        printf("Exception in display_imports(0x%08X) while ", pAddress);
        if (!pPEHeader)
        {
            return ERROR_INVALID_EXE_SIGNATURE;
        }
        else if (!dwImportRVA)
        {
            /* This should never happen.  If we get to this exception
               handler, it means that we failed while trying to get
               the RVA for the delay import descriptor, which means
               tthat we bombed out on a memory access while poking the
               EXE.  This should never happen... */
            assert(dwImportRVA);
            return ERROR_BAD_EXE_FORMAT;
        }
        else if (!pImportDesc)
        {
            /* This should never happen.  If we get to this exception
               handler, it means that we failed while trying to get
               the import delay descriptor, which means tthat we
               bombed out on a memory access while poking the EXE.
               This should never happen... */
            assert(pImportDesc);
            return ERROR_BAD_EXE_FORMAT;
        }
        else if (!pszDLL)
        {
            
            /* This should never happen.  If we get to this exception
               handler, it means that we failed while trying to get
               the delay DLL name, which means tthat we bombed out on
               a memory access while poking the EXE.  This should
               never happen... */
            assert(pszDLL);
            return ERROR_BAD_EXE_FORMAT;
        }
        else if (!pINT)
        {
            /* XXX */
        }
        else
        {
            printf("???");
        }
        printf("\n");
        return false;
    }
    return true;
}

template <class T>
static
T*
new_zero_array(
    size_t count
    )
{
    T* ptr = new T[count];
    if (ptr)
        memset(ptr, 0, sizeof(T) * count);
    return ptr;
}

template <class T>
static
void
safe_delete_array(
    T*& ptr
    )
{
    if (ptr)
    {
        delete [] ptr;
        ptr = 0;
    }
}

template <class T>
static
void
safe_delete(
    T*& ptr
    )
{
    if (ptr)
    {
        delete ptr;
        ptr = 0;
    }
}

class dll_handle_t {
private:
    bool m_bInitialized;

    struct dll_t {
        PCSTR pszDllName;
        HMODULE hDll;
    };

    DWORD m_cDll;
    DWORD m_cFunc;

    DWORD m_iDll;
    DWORD m_iDllError;
    DWORD m_iFuncError;

    dll_t* m_aDll;
    DelayLoadDlls_dll_error_t*  m_aDllError;
    DelayLoadDlls_func_error_t* m_aFuncError;

private:
    void FreeLists() {
        if (m_aDll)
        {
            for (DWORD i = 0; i < m_cDll; i++)
            {
                HMODULE& h = m_aDll[i].hDll;
                PCSTR& name = m_aDll[i].pszDllName;
                if (h)
                {
                    FreeLibrary(h);
                    h = 0;
                }
                if (name)
                {
#if defined(_MSC_VER) && (_MSC_VER < 1300 )
                    __FUnloadDelayLoadedDLL(name);
#else
                    __FUnloadDelayLoadedDLL2(name);
#endif
					name = 0;
                }
            }
            
        }
        safe_delete_array(m_aDll);
        safe_delete_array(m_aDllError);
        safe_delete_array(m_aFuncError);
    }

    void AddDllError(PCSTR pszDllName, DWORD dwError) {

        assert(m_iDllError < m_cDll);
        assert(m_aDllError);

        DelayLoadDlls_dll_error_t& e = m_aDllError[m_iDllError];
        m_iDllError++;

        e.pszDllName = pszDllName;
        e.dwError = dwError;
    };

    void AddFuncError(PCSTR pszDllName, BOOL IsOrdinal, WORD dwOrdinal,
                      PCSTR pszFuncName, DWORD dwError) {

        assert(m_iFuncError < m_cFunc);
        assert(m_aFuncError);

        DelayLoadDlls_func_error_t& e = m_aFuncError[m_iFuncError];
        m_iFuncError++;

        e.pszDllName = pszDllName;
        e.IsOrdinal = IsOrdinal;
        e.dwOrdinal = dwOrdinal;
        e.pszFuncName = pszFuncName;
        e.dwError = dwError;
    };

    bool HaveDllErrors() {
        return m_iDllError != 0;
    }

    bool HaveFuncErrors() {
        return m_iFuncError != 0;
    }

public:

    void IncDll()  { m_cDll++;  };
    void IncFunc() { m_cFunc++; };

    void AddDll(PCSTR pszDllName) {

        assert(m_iDll < m_cDll);
        assert(m_aDll);

        m_aDll[m_iDll].pszDllName = pszDllName;
        HMODULE& h = m_aDll[m_iDll].hDll;

        m_iDll++;

        // Set error mode to disable system error pop-ups (for LoadLibrary)
        DWORD em = SetErrorMode(SEM_FAILCRITICALERRORS);

        h = LoadLibrary(pszDllName);

        // Restore previous error mode
        SetErrorMode(em);

        if (!h)
        {
            DWORD dwError = GetLastError();
            AddDllError(pszDllName, dwError);
        }
    };

    void AddFunc(PCSTR pszDllName, BOOL IsOrdinal, WORD dwOrdinal,
                 PCSTR pszFuncName) {

        assert((m_iDll - 1) < m_cDll);
        assert(m_aDll);
        assert(pszFuncName || IsOrdinal);

        HMODULE& h = m_aDll[m_iDll-1].hDll;
        if (!h) return;

        LPCSTR lpProcName = (IsOrdinal ? MAKEINTRESOURCE(dwOrdinal) :
                             pszFuncName);

        assert(lpProcName);

        FARPROC pf = GetProcAddress(h, lpProcName);
        if (!pf)
        {
            DWORD dwError = GetLastError();
            AddFuncError(pszDllName, IsOrdinal, dwOrdinal, pszFuncName,
                         dwError);
        }
    };

    bool LoadedDll(PCSTR pszDllName) {
        assert(m_aDll);

        for (DWORD i = 0; i < m_cDll; i++)
            if (!stricmp(pszDllName, m_aDll[i].pszDllName))
                return m_aDll[i].hDll != NULL;
        return false;
    }

    bool LoadedDllAll(PCSTR pszDllName) {
        assert(m_aFuncError);

        if (!LoadedDll(pszDllName))
            return false;

        for (DWORD i = 0; m_aFuncError[i].pszDllName; i++)
            if (!stricmp(pszDllName, m_aFuncError[i].pszDllName))
                return false;
        return true;
    }

    DelayLoadDlls_dll_error_t* GetDllErrors() {
        return HaveDllErrors() ? m_aDllError : NULL;
    }

    DelayLoadDlls_func_error_t* GetFuncErrors() {
        return HaveFuncErrors() ? m_aFuncError : NULL;
    }

    bool IsEmpty() {
        return (!m_cDll || !m_cFunc);
    };

    bool Init() {
        if (m_bInitialized) return true;
        if (IsEmpty()) return true;

        assert(!m_aDll && !m_aDllError && !m_aFuncError);

        m_aDll = new_zero_array<dll_t>(m_cDll);
        // Allocate one extra so that the array is zero-terminated
        m_aDllError = new_zero_array<DelayLoadDlls_dll_error_t>(m_cDll + 1);
        m_aFuncError = new_zero_array<DelayLoadDlls_func_error_t>(m_cFunc + 1);

        if (!m_aDll || !m_aDllError || !m_aFuncError)
        {
            FreeLists();
            return false;
        }

        m_bInitialized = true;
        return true;
    }

    dll_handle_t():m_bInitialized(false),m_cDll(0),m_cFunc(0),m_iDll(0),m_iDllError(0),m_iFuncError(0),m_aDll(0),m_aDllError(0),m_aFuncError(0) {
    };
    ~dll_handle_t() {
        FreeLists();
    }
};

static
BOOL
CountWalker(
    PVOID pContext, walker_func_action_e Action, PCSTR pszDllName,
    BOOL IsOrdinal, WORD dwOrdinal, PCSTR pszFuncName
    )
{
    dll_handle_t* pHandle = (dll_handle_t*)pContext;

    assert(pHandle);

    switch (Action)
    {
    case WALKER_DLL_START:
        pHandle->IncDll();
        break;
    case WALKER_DLL_END:
        break;
    case WALKER_FUNCTION:
        pHandle->IncFunc();
        break;
    default:
        assert(0);
        return FALSE;
    }
    return TRUE;
}

static
BOOL
LoadWalker(
    PVOID pContext, walker_func_action_e Action, PCSTR pszDllName,
    BOOL IsOrdinal, WORD dwOrdinal, PCSTR pszFuncName
    )
{
    dll_handle_t* pHandle = (dll_handle_t*)pContext;

    assert(pHandle);

    switch (Action)
    {
    case WALKER_DLL_START:
        pHandle->AddDll(pszDllName);
        break;
    case WALKER_DLL_END:
        break;
    case WALKER_FUNCTION:
        pHandle->AddFunc(pszDllName, IsOrdinal, dwOrdinal, pszFuncName);
        break;
    default:
        assert(0);
        return FALSE;
    }
    return TRUE;
}


DelayLoadDlls_handle_t
DelayLoadDllsLoad(
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pContext = 0;
    DWORD status = 0;
    PBYTE pAddress = 0;

    dll_handle_t* pHandle = new dll_handle_t;
    if (!pHandle)
    {
        status = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }

    pAddress = GetCurrentProcessModuleAddress();

    walk_dlls(pAddress, CountWalker, pHandle);

    if (pHandle->IsEmpty())
    {
        safe_delete(pHandle);
        status = ERROR_SUCCESS;
        goto cleanup;
    }

    if (!pHandle->Init())
    {
        status = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }

    walk_dlls(pAddress, LoadWalker, pHandle);

 cleanup:
    if (status && pHandle)
        safe_delete(pHandle);
    if (pdwStatus) *pdwStatus = status;
    return (DelayLoadDlls_handle_t) pHandle;
}

DelayLoadDlls_dll_error_t*
DelayLoadDllsGetDllErrors(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pHandle = (dll_handle_t*) hDelayLoadDlls;

    if (!pHandle)
    {
        if (pdwStatus) *pdwStatus = ERROR_INVALID_PARAMETER;
        return NULL;
    }

    if (pdwStatus) *pdwStatus = ERROR_SUCCESS;
    return pHandle->GetDllErrors();
}

DelayLoadDlls_func_error_t*
DelayLoadDllsGetFuncErrors(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pHandle = (dll_handle_t*) hDelayLoadDlls;

    if (!pHandle)
    {
        if (pdwStatus) *pdwStatus = ERROR_INVALID_PARAMETER;
        return NULL;
    }

    if (pdwStatus) *pdwStatus = ERROR_SUCCESS;
    return pHandle->GetFuncErrors();
}

BOOL
DelayLoadDllsFree(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pHandle = (dll_handle_t*) hDelayLoadDlls;

    if (!pHandle)
    {
        if (pdwStatus) *pdwStatus = ERROR_INVALID_PARAMETER;
        return FALSE;
    }

    safe_delete(pHandle);

    if (pdwStatus) *pdwStatus = ERROR_SUCCESS;
    return TRUE;
}

BOOL
DelayLoadDllsLoadedDll(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    IN  PCSTR pszDllName,
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pHandle = (dll_handle_t*) hDelayLoadDlls;

    if (!pHandle)
    {
        if (pdwStatus) *pdwStatus = ERROR_INVALID_PARAMETER;
        return NULL;
    }

    if (pdwStatus) *pdwStatus = ERROR_SUCCESS;
    return pHandle->LoadedDll(pszDllName);
}

BOOL
DelayLoadDllsLoadedDllAll(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    IN  PCSTR pszDllName,
    OUT PDWORD pdwStatus
    )
{
    dll_handle_t* pHandle = (dll_handle_t*) hDelayLoadDlls;

    if (!pHandle)
    {
        if (pdwStatus) *pdwStatus = ERROR_INVALID_PARAMETER;
        return NULL;
    }

    if (pdwStatus) *pdwStatus = ERROR_SUCCESS;
    return pHandle->LoadedDllAll(pszDllName);
}
