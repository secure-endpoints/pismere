#include <windows.h>
#include "secure.hxx"
#include "debug.h"

CcOsLock SecureClient::s_lock;
DWORD SecureClient::s_refcount = 0;
DWORD SecureClient::s_error = 0;
HANDLE SecureClient::s_hToken = 0;

#include "util.h"

#define SC "SecureClient::"

DWORD
SecureClient::Attach(
    )
{
    CcAutoLock AL(s_lock);
    if (s_hToken) {
        s_refcount++;
        return 0;
    }
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, 
                         &s_hToken)) {
        s_refcount++;
        s_error = 0;
    } else {
        s_hToken = 0;
        s_error = GetLastError();
    }
    return s_error;
}

DWORD
SecureClient::Detach(
    )
{
    CcAutoLock AL(s_lock);
    s_refcount--;
    if (s_refcount) return 0;
    if (!s_hToken) return 0;
    DWORD error = 0;
    if (!CloseHandle(s_hToken))
        error = GetLastError();
    s_hToken = 0;
    s_error = 0;
    return error;
}

DWORD
SecureClient::Token(HANDLE& hToken)
{
    // This funciton will not do automatic initialization.
    CcAutoLock AL(s_lock);
    hToken = 0;
    if (!s_hToken) {
        DEBUG_PRINT((SC "Token: "
                     "no process token initialized (%u)\n", s_error));
        return s_error ? s_error : ERROR_INVALID_HANDLE;
    } else {
        DWORD status = 0;
        if (!DuplicateHandle(GetCurrentProcess(), s_hToken, 
                             GetCurrentProcess(), &hToken, 0, FALSE, 
                             DUPLICATE_SAME_ACCESS))
        {
            status = GetLastError();
            DEBUG_PRINT((SC "Token: "
                         "Could not duplicate handle (%u)\n", status));
        }
        return status;
    }
}

void
SecureClient::Start(SecureClient*& s) {
    s = new SecureClient;
}

void
SecureClient::Stop(SecureClient*& s) {
    delete s;
    s = 0;
}

///////////////////////////////////////////////////////////////////////////////

SecureClient::SecureClient():
    m_Error(0),
    m_hToken(0),
    m_NeedRestore(false)
{
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE,
                         &m_hToken)) {
        m_Error = GetLastError();
        DEBUG_PRINT((SC "SecureClient: "
                     "Cannot open thread token (%u)\n", m_Error));
        return;
    }
    HANDLE hThread = GetCurrentThread();
    if (SetThreadToken(&hThread, NULL)) {
        m_NeedRestore = true;
    } else {
        m_Error = GetLastError();
        DEBUG_PRINT((SC "SecureClient: "
                     "Cannot remove thread token (%u)\n", m_Error));
    }
}

SecureClient::~SecureClient()
{
    if (m_NeedRestore) {
        HANDLE hThread = GetCurrentThread();
        if (!SetThreadToken(&hThread, m_hToken)) {
            m_Error = GetLastError();
            DEBUG_PRINT((SC "~SecureClient: "
                         "Cannot restore thread token (%u)\n", m_Error));
        }
    }
    if (m_hToken) {
        if (!CloseHandle(m_hToken)) {
            m_Error = GetLastError();
            DEBUG_PRINT((SC "~SecureClient: "
                         "Error closing token handle (%u)\n", m_Error));
        }
    }
}

DWORD
SecureClient::Error()
{
    return m_Error;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
DWORD
SecureClient::CheckImpersonation(
    )
{
    DWORD dwImp = 0;
    HANDLE hThreadTok = 0;
    DWORD dwBytesReturned;
    DWORD status = 0;

    if (!OpenThreadToken(GetCurrentThread(),
                         TOKEN_QUERY,
                         TRUE,
                         &hThreadTok))
    {
        status = GetLastError();
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "Unable to read thread token (%u)\n", status));
        CLEANUP_STATUS(status);
    }

    if (!GetTokenInformation(hThreadTok,
                             TokenImpersonationLevel, 
                             &dwImp,
                             sizeof(DWORD),
                             &dwBytesReturned))
    {
        status = GetLastError();
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "Unable to read impersonation level (%u)\n", status));
        CLEANUP_STATUS(status);
    }

    switch (dwImp)
    {
    case SecurityAnonymous:
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "SecurityAnonymous\n"));
        break;
    case SecurityIdentification:
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "SecurityIdentification\n"));
        break;
    case SecurityImpersonation:
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "SecurityImpersonation\n"));
        break;
    case SecurityDelegation:
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "SecurityDelegation\n"));
        break;
    default:
        DEBUG_PRINT((SC "CheckImpersonation: "
                     "Unable to determine impersonation level (%u)\n", 
                     dwImp));
        break;
    }
 cleanup:
    if (hThreadTok)
        CloseHandle(hThreadTok);
    return status;
}

bool
SecureClient::IsImp(
    )
{
    DWORD status = 0;
    HANDLE hToken = 0;

    return false;

    if (!OpenThreadToken(GetCurrentThread(), 
                         TOKEN_ALL_ACCESS, 
                         TRUE,
                         &hToken))
    {
        status = GetLastError();
        DEBUG_PRINT((SC "IsImp: "
                     "Cannot open thread token (%u)\n", status));
    }
    if (hToken)
        CloseHandle(hToken);
    return status ? false : true;
}

DWORD
SecureClient::DuplicateImpAsPrimary(
    HANDLE& hPrimary
    )
{
    DWORD status = 0;
    HANDLE hToken = 0;
    hPrimary = 0;

    if (!OpenThreadToken(GetCurrentThread(), 
                         TOKEN_ALL_ACCESS, 
                         TRUE,
                         &hToken))
    {
        status = GetLastError();
        DEBUG_PRINT((SC "DuplicateImpAsPrimary: "
                     "Cannot open thread token (%u)\n", status));
        CLEANUP_STATUS(status);
    }

    if (!DuplicateTokenEx(hToken,
                          TOKEN_QUERY | TOKEN_DUPLICATE | 
                          TOKEN_ASSIGN_PRIMARY,
                          0, 
                          SecurityImpersonation,
                          TokenPrimary,
                          &hPrimary))
    {
        status = GetLastError();
        DEBUG_PRINT((SC "DuplicateImpAsPrimary: "
                     "Cannot duplicate thread token (%u)\n", status));
        CLEANUP_STATUS(status);
    }

 cleanup:
    if (hToken)
        CloseHandle(hToken);
    return status;
}
#endif
