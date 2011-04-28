#ifndef __UTIL_H__
#define __UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

void*
user_allocate(
    size_t size
    );

void
user_free(
    void* ptr
    );

void
free_alloc_p(
    void* pptr
    );

DWORD
alloc_name(
    LPSTR* pname,
    LPSTR postfix,
    BOOL isNT
    );

DWORD
alloc_own_security_descriptor_NT(
    PSECURITY_DESCRIPTOR* ppsd
    );

DWORD
alloc_module_dir_name(
    char* module,
    char** pname
    );

DWORD
alloc_module_dir_name_with_file(
    char* module,
    char* file,
    char** pname
    );

#ifdef __cplusplus
}
#endif

#endif /* __UTIL_H__ */
