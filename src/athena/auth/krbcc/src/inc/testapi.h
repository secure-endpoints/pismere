#ifndef __TESTAPI_H__
#define __TESTAPI_H__

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#ifdef NO_TYPEDEFS
#define CCTEST_MAKE_FUNC(ret, name, args) \
ret\
name args
#else
#define CCTEST_MAKE_FUNC(ret, name, args) \
typedef ret (*FP_##name) args; \
ret name args
#endif

CCTEST_MAKE_FUNC(
    DWORD,
    CCTEST_reconnect, (
        char* endpoint OPTIONAL
        )
    );

CCTEST_MAKE_FUNC(
    DWORD,
    CCTEST_shutdown, (
        )
    );

#ifndef NO_TYPEDEFS
typedef struct {
    DWORD size;
    FP_CCTEST_reconnect reconnect;
    FP_CCTEST_shutdown  shutdown;
} CCTEST_functions;

DWORD
CCTEST(
    CCTEST_functions* funcs
    );
#endif

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* __TESTAPI_H__ */
