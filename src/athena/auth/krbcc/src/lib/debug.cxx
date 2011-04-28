#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

static
char*
DEBUG_PRINT_HELP(
    char* buffer,
    size_t& size,
    char* format,
    ...
    )
{
    if (!size)
        return buffer;
    char* result = 0;
    va_list ap;
    va_start(ap, format);
    int count = _vsnprintf(buffer, size, format, ap);
    va_end(ap);
    if (count < 0) {
        result = buffer + size;
        size = 0;
    } else {
        DEBUG_ASSERT(size <= (size_t)count);
        size -= count;
        result = buffer + count;
    }
    return result;
}

static DebugMode_t DEBUG_MODE = DM_DEBUGGER;

void
SetDebugMode(
    DebugMode_t mode
    )
{
    if (mode >= DM_NONE && mode < DM_MAX_SENTINEL)
        DEBUG_MODE = mode;
}

void
DEBUG_PRINT_EX(
    char* format, 
    ...
    )
{
    char buffer[1024];
    char *p = buffer;
    size_t size = sizeof(buffer);
    int count = 0;
    int res = 0;

#if 0
    if (module)
        p = DEBUG_PRINT_HELP(p, size, "[%s]: ", module);
    if (function)
        p = DEBUG_PRINT_HELP(p, size, "%s: ", function);
    if (file && line)
        p = DEBUG_PRINT_HELP(p, size, "{%s(%d)}", file, line);
    else if (file)
        p = DEBUG_PRINT_HELP(p, size, "{%s}: ", file);
    else if (line)
        p = DEBUG_PRINT_HELP(p, size, "{(%d)}: ", line);
#endif

    if (size > 0) {
        va_list ap;
        va_start(ap, format);
        _vsnprintf(p, size, format, ap);
        va_end(ap);
    }

    buffer[size-1] = 0;
    switch(DEBUG_MODE) {
    case DM_NONE:
        break;
    case DM_DEBUGGER:
        OutputDebugString(buffer);
        break;
    case DM_STDOUT:
        printf(buffer);
        break;
    case DM_STDERR:
        fprintf(stderr, buffer);
        break;
    }
}
