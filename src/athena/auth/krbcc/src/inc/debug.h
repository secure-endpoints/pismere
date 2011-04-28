#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define CLEANUP_STATUS(x) do { status = x; goto cleanup; } while(0)
#define CLEANUP_ON_STATUS(x) do { if (x) { status = x; goto cleanup; } } while(0)

enum DebugMode_t {
    DM_NONE,
    DM_DEBUGGER,
    DM_STDOUT,
    DM_STDERR,
    DM_MAX_SENTINEL
};

void
SetDebugMode(
    DebugMode_t
    );

void
DEBUG_PRINT_EX(
    char* format, 
    ...
    );

#define D_MACRO_STRING(x)        #x
#define D_MACRO_APPLY(macro, x)  macro(x)
#define D_LINE                   D_MACRO_APPLY(D_MACRO_STRING, __LINE__)

#define DEBUG_PRINT_ALWAYS(x) \
    DEBUG_PRINT_EX x
#define DEBUG_PRINT_ASSERT_MSG(x) \
    DEBUG_PRINT_ALWAYS(("ASSERT FAILED: " x \
                        "\n\tLine: " D_LINE "\n\tFile: " __FILE__ "\n"))
#define DEBUG_ASSERT_ALWAYS(x) \
    do { if (!(x)) DEBUG_PRINT_ASSERT_MSG(#x); assert(x); } while (0)
#define DEBUG_ASSERT_PRINT_ONLY(x) \
    do { if (!(x)) DEBUG_PRINT_ASSERT_MSG(#x); } while (0)

#ifdef NDEBUG
#define DEBUG_PRINT(x)
#define DEBUG_ASSERT(x) DEBUG_ASSERT_PRINT_ONLY(x)
#else
#define DEBUG_PRINT(x)  DEBUG_PRINT_ALWAYS(x)
#define DEBUG_ASSERT(x) DEBUG_ASSERT_ALWAYS(x)
#endif

#define D_BUG       "BUG: "
#define D_EXCEPTION "EXCEPTION: "
#define D_CLEANUP   "CLEANUP: "
#define D_MEM       "MEM: "

#if 0
{
#endif
#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H__ */
