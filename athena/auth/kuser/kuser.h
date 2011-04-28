#ifndef __KUSER_H__
#define __KUSER_H__

#include <krb5.h>

extern char *progname;

void
fix_progname(char **argv);

krb5_error_code KRB5_CALLCONV
k_string_to_timestamp(
    char		* string,
    krb5_timestamp	* timestampp
    );

krb5_error_code KRB5_CALLCONV
k_string_to_deltat(
    char        * string,
    krb5_deltat * deltatp
    );

krb5_error_code KRB5_CALLCONV
k_timestamp_to_string(
    krb5_timestamp	timestamp,
    char		FAR * buffer,
    size_t		buflen
    );


krb5_error_code KRB5_CALLCONV
k_timestamp_to_sfstring(
    krb5_timestamp	timestamp,
    char		FAR * buffer,
    size_t		buflen,
    char		FAR * pad
    );

void KRB5_CALLCONV_C
fake_com_err(
    const char FAR *whoami,
    errcode_t code,
    const char FAR *fmt, 
    ...
    );

krb5_error_code
KRB5_CALLCONV
k_read_password(
    krb5_context        ctx,
    const char		* prompt,
    const char		* prompt2,
    char		* password,
    unsigned int	* pwsize
    );

/* ------------------------------------------------------------------------- */

void
dynamic_load(
    int * pgot_k4,
    int * pgot_k5
    );

#define COMERR_DLL    "comerr32.dll"
#define KRB5_DLL      "krb5_32.dll"
#define KRB4_DLL      "krbv4w32.dll"

#include <loadfuncs.h>

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV_C,
    com_err,
    (const char FAR *, errcode_t, const char FAR *, ...)
    );
TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_deltat,
    (char *, krb5_deltat *)
    );
TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_timestamp,
    (char *, krb5_timestamp *)
    );
TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_timestamp_to_sfstring,
    (krb5_timestamp, char *, size_t, char *)
    );
TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_read_password,
    (krb5_context,
     const char *,
     const char *,
     char *,
     unsigned int * )
    );

extern DECL_FUNC_PTR(krb5_string_to_deltat);
extern DECL_FUNC_PTR(krb5_string_to_timestamp);
extern DECL_FUNC_PTR(krb5_read_password);
extern DECL_FUNC_PTR(krb5_timestamp_to_sfstring);
extern DECL_FUNC_PTR(com_err);

#endif /* __KUSER_H__ */
