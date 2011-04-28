#ifndef __LOADFUNCS_LEASH_H__
#define __LOADFUNCS_LEASH_H__

#include "loadfuncs.h"
#include <leashwin.h>

#define LEASH_DLL      "leashw32.dll"

#define CALLCONV_C

TYPEDEF_FUNC(
    int,
    CALLCONV_C,
    Leash_kinit_dlg,
    (HWND, LPLSH_DLGINFO)
    );
TYPEDEF_FUNC(
    int,
    CALLCONV_C,
    Leash_changepwd_dlg,
    (HWND, LPLSH_DLGINFO)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_checkpwd,
    (char *, char *)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_changepwd,
    (char *, char *, char*, char*)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_kinit,
    (char *, char *, int)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_klist,
    (HWND, TICKETINFO*)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_kdestroy,
    (void)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_get_lsh_errno,
    (LONG *)
    );
TYPEDEF_FUNC(
    BOOL,
    CALLCONV_C,
    Leash_set_help_file,
    (char *)
    );
TYPEDEF_FUNC(
    char *,
    CALLCONV_C,
    Leash_get_help_file,
    (void)
    );
TYPEDEF_FUNC(
    long,
    CALLCONV_C,
    Leash_timesync,
    (int)
    );
TYPEDEF_FUNC(
    DWORD,
    CALLCONV_C,
    Leash_get_default_lifetime,
    (void)
    );
TYPEDEF_FUNC(
    DWORD,
    CALLCONV_C,
    Leash_set_default_lifetime,
    (DWORD)
    );

/* They are not yet all here... */

#endif /* __LOADFUNCS_LEASH_H__ */
