#ifndef __CHECK_DELAY_LOAD_DLLS__

#define __CHECK_DELAY_LOAD_DLLS__

#ifdef __cplusplus
extern "C" {
#endif

/* To balance {}s For Emacs and other editors */
#if 0
}
#endif

struct _DelayLoadDlls_handle_t;
typedef struct _DelayLoadDlls_handle_t* DelayLoadDlls_handle_t;

typedef struct {
    PCSTR pszDllName;
    DWORD dwError;
} DelayLoadDlls_dll_error_t;

typedef struct {
    PCSTR pszDllName;
    BOOL IsOrdinal;
    WORD dwOrdinal;
    PCSTR pszFuncName;
    DWORD dwError;
} DelayLoadDlls_func_error_t;


DelayLoadDlls_handle_t
DelayLoadDllsLoad(
    OUT PDWORD pdwStatus
    );

BOOL
DelayLoadDllsFree(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    );

DelayLoadDlls_dll_error_t*
DelayLoadDllsGetDllErrors(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    );

DelayLoadDlls_func_error_t*
DelayLoadDllsGetFuncErrors(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    OUT PDWORD pdwStatus
    );

BOOL
DelayLoadDllsLoadedDll(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    IN  PCSTR pszDllName,
    OUT PDWORD pdwStatus
    );

BOOL
DelayLoadDllsLoadedDllAll(
    IN  DelayLoadDlls_handle_t hDelayLoadDlls,
    IN  PCSTR pszDllName,
    OUT PDWORD pdwStatus
    );

/* To balance {}s For Emacs and other editors */
#if 0
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CHECK_DELAY_LOAD_DLLS__ */
