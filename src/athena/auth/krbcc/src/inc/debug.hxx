#pragma once

#include <time.h>

class PtrStr
{
private:
    char str[80]; //11
public:
    PtrStr(void** ptr) {
        if (ptr)
            _snprintf(str, sizeof(str), "0x%08p", *ptr);
        else
            _snprintf(str, sizeof(str), "(null)");
    }
    PtrStr(char** ptr) {
        if (ptr)
            _snprintf(str, sizeof(str), "%s", *ptr);
        else
            _snprintf(str, sizeof(str), "(null)");
    }
    PtrStr(time_t* ptr) {
        if (ptr)
            _snprintf(str, sizeof(str), "%s", ctime(ptr));
        else
            _snprintf(str, sizeof(str), "(null)");
    }
    PtrStr(int* ptr) {
        if (ptr)
            _snprintf(str, sizeof(str), "%d", *ptr);
        else
            _snprintf(str, sizeof(str), "(null)");
    }
    ~PtrStr() {
        // Empty destructor for source-level debugging purposes.
        ;
    }
    operator char*() {
        return str;
    }
};

#define INT_TO_STR(x) ((char*)PtrStr((int*)x))
#define STR_TO_STR(x) ((char*)PtrStr((char**)x))
#define PTR_TO_STR(x) ((char*)PtrStr((void**)x))
#define TIME_TO_STR(x) ((char*)PtrStr((time_t*)x))
