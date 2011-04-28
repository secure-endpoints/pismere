#include <windows.h>
#include <new>
#include <malloc.h>
#include "ccapi.h"
#include "util.h"
#include "debug.h"
#include "autolock.hxx"

/*********************************************************************/
/*                MIDL allocate and free                             */
/*********************************************************************/

#ifdef DEBUG_MEMORY

class MemInfo {
public:
    struct MemInfoPair {
        size_t size;
        size_t blocks;
    };
private:
    CcOsLock lock;
    MemInfoPair data;
public:
    MemInfoPair add(size_t s) {
        CcAutoLock AL(lock);
        data.size += s;
        data.blocks++;
        return data;
    };
    MemInfoPair del(size_t s) {
        CcAutoLock AL(lock);
        data.size -= s;
        data.blocks--;
        return data;
    };
public:
    MemInfo() { data.size = 0; data.blocks = 0; };
    ~MemInfo() {};
};

static MemInfo mem;

void* user_allocate(size_t size)
{
    void * ptr = malloc(size + sizeof(size_t));
    if (ptr) {
        size_t* s = (size_t*)ptr;
        *s = size;
        char* temp = (char*)ptr;
        temp += sizeof(size_t);
        ptr = temp;
        MemInfo::MemInfoPair total = mem.add(size);
        DEBUG_PRINT((D_MEM "Alloc: 0x%08X (size = %u) really at 0x%08X\n"
                     "\tTotal: %u blocks for %u bytes\n",
                     ptr, size, s, total.blocks, total.size));
    } else {
        DEBUG_PRINT((D_MEM "Alloc: (size = %d) -- FAILED\n", size));
    }
    return(ptr);
}

void user_free(void* ptr)
{
    if (!ptr) return;
    char* temp = (char*)ptr;
    temp -= sizeof(size_t);
    size_t* s = (size_t*)temp;
    MemInfo::MemInfoPair total = mem.del(*s);
    DEBUG_PRINT((D_MEM "Free: 0x%08X (size = %u) really at 0x%08X\n"
                 "\tTotal: %u blocks for %u bytes\n",
                 ptr, *s, s, total.blocks, total.size));
    free(s);
}

#else // !DEBUG_MEMORY

// NOTE: For some unfathomable reason, using LocalAlloc/LoalFree instead
// of malloc/free below causes a crash...

void* user_allocate(size_t size)
{
    return malloc(size);
}

void user_free(void* ptr)
{
    if (ptr) free(ptr);
}

#endif // !DEBUG_MEMORY

void* operator new (size_t size) throw (std::bad_alloc) {
    void* buffer = user_allocate(size);
    if (!buffer) throw std::bad_alloc();
    return buffer;
}

void operator delete (void* ptr) throw () {
    user_free(ptr);
}

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t size)
{
    return user_allocate(size);
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    user_free(ptr);
}
