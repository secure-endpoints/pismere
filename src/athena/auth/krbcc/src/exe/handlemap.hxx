#pragma once

#include <autolock.hxx>
#include <new>

namespace HandleMap {
#if 0
}
#endif

const size_t MIN_HANDLE = 1;
const size_t MAX_HANDLE = (size_t)(-2); // cast
const size_t MAX_SIZE = MAX_HANDLE - MIN_HANDLE;

enum Error_t {
    HE_NOERROR = 0,
    HE_NOT_FOUND,
    HE_NO_MEM,
    HE_MAX_SIZE
};

class Exception {
public:
    Error_t type;
    Exception(Error_t t):type(t) {};
};

template <typename H, typename T>
class HandleMap {
private:
    class Data {
    public:
        bool used;
        T data;
        Data():used(false) {}
    };

    bool init;
    Data* handles;
    size_t last;
    size_t used;
    size_t size;
    const size_t base_size;
    const size_t grow_size;

    CcOsLock lock;

    void initialize()
        {
            grow(base_size);
            init = true;
        };

    void grow(size_t els = 0)
        {
            try {
                if (!els)
                    els = grow_size;
                if ((els + size) > MAX_SIZE)
                    throw Exception(HE_MAX_SIZE);
                if (!handles) {
                    handles = new Data[els];
                    if (!handles)
                        throw Exception(HE_NO_MEM);
                    last = 0;
                    size = els;
                } else {
                    Data* nhandles = new Data[size + els];
                    if (!nhandles)
                        throw Exception(HE_NO_MEM);
                    for (size_t i = 0; i < size; i++)
                        nhandles[i] = handles[i];
                    size += els;
                    delete [] handles;
                    handles = nhandles;
                }
            } catch (std::bad_alloc&) {
                throw Exception(HE_NO_MEM);
            }
        };

public:
    void create(H& handle)
        {
            CcAutoLock AL(lock);
            if (!init) initialize();
            if (used >= size) grow();
            if (used >= size) throw Exception(HE_NO_MEM);
            while (handles[last % size].used)
                last++;
            last %= size;
            handles[last].used = true;
            used++;
            handle = (H)(last + MIN_HANDLE); // cast
        };

    void close(H& handle)
        {
            CcAutoLock AL(lock);
            if (!init) initialize();
            size_t i = (size_t)(handle) - MIN_HANDLE; // cast
            if ((i > (size-1)) || !handles[i].used)
                throw Exception(HE_NOT_FOUND);
            handles[i].used = false;
            used--;
            handle = 0;
        }

    T& operator[] (H handle)
        {
            CcAutoLock AL(lock);
            if (!init) initialize();
            size_t i = (size_t)(handle) - MIN_HANDLE; // csat
            if ((i > (size-1)) || !handles[i].used)
                throw Exception(HE_NOT_FOUND);
            return handles[i].data;
        }

    HandleMap(size_t s = 1000, size_t g = 100):
        base_size(s),
        grow_size(g),
        init(false),
        handles(0),
        last(0),
        used(0),
        size(0)
        {
        };

    ~HandleMap()
        {
            CcAutoLock AL(lock);
            delete [] handles;
        };
};

#if 0
{
#endif
}
