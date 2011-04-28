#ifndef __DLL_CLIENT_H__
#define __DLL_CLIENT_H__

#include <autolock.hxx>
#include <init.hxx>

class Client
{
public:
    static DWORD Initialize();
    static DWORD Cleanup();
    static DWORD Reconnect(char* ep OPTIONAL);

    static bool Initialized() { return s_init; }

    static CcOsLock sLock;

private:
    static bool s_init;

    static DWORD Disconnect();
    static DWORD Connect(char* ep OPTIONAL);
};

#define CLIENT_INIT_EX(trap, error) \
do \
{ \
    INIT_INIT_EX(trap, error); \
    if (!Client::Initialized()) \
    { \
        DWORD status = Client::Initialize(); \
        if (status) return (trap) ? (error) : status; \
    } \
} while(0)

#endif
