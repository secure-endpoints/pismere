#include <windows.h>
#include <stdio.h>
#include <delaydlls.h>
#include <testdll.h>

int
main(
    int argc,
    char* argv[]
    )
{
    DelayLoadDlls_handle_t hDlls = 0;
    DelayLoadDlls_dll_error_t* pDllError = 0;
    DelayLoadDlls_func_error_t* pFuncError = 0;

    DWORD status = 0;

    hDlls = DelayLoadDllsLoad(&status);
    printf("%u\n", status);
    if (!hDlls)
        return 1;
    pDllError = DelayLoadDllsGetDllErrors(hDlls, NULL);
    if (pDllError)
    {
        printf("DLL ERROR\n");
    }
    pFuncError = DelayLoadDllsGetFuncErrors(hDlls, NULL);
    if (pFuncError)
    {
        printf("FUNC ERROR\n");
    }
    testdll();
    DelayLoadDllsFree(hDlls, &status);
    printf("%u\n", status);
    return 0;
}
