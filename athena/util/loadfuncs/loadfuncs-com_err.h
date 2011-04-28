#ifndef __LOADFUNCS_COM_ERR_H__
#define __LOADFUNCS_COM_ERR_H__

#include "loadfuncs.h"
#include <com_err.h>

#define COMERR_DLL      "comerr32.dll"

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV_C,
    com_err,
    ET_STDARG_P((const char FAR *, errcode_t, const char FAR *, ...))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    com_err_va,
    ET_P((const char FAR *whoami, errcode_t code, const char FAR *fmt,
          va_list ap))
    );
TYPEDEF_FUNC(
    const char FAR *,
    KRB5_CALLCONV,
    error_message,
    ET_P((errcode_t))
    );
TYPEDEF_FUNC(
    errcode_t,
    KRB5_CALLCONV,
    add_error_table,
    ET_P((const struct error_table FAR *))
    );
TYPEDEF_FUNC(
    errcode_t,
    KRB5_CALLCONV,
    remove_error_table,
    ET_P((const struct error_table FAR *))
    );

#endif /* __LOADFUNCS_COM_ERR_H__ */
