#pragma once

#include <windows.h>
#include "autolock.hxx"

class SecureClient
{
public:
    static DWORD Attach();
    static DWORD Detach();
    static DWORD Token(HANDLE& hToken);
    static void Start(SecureClient*& s);
    static void Stop(SecureClient*& s);

#if 0
    static DWORD CheckImpersonation();
    static bool IsImp();
    static DWORD DuplicateImpAsPrimary(HANDLE& hPrimary);
#endif

    SecureClient();
    ~SecureClient();
    DWORD Error();

private:
    static CcOsLock s_lock;
    static DWORD s_refcount;
    static DWORD s_error;
    static HANDLE s_hToken;

    DWORD m_Error;
    HANDLE m_hToken;
    bool m_NeedRestore;
};
