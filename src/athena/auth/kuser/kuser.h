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
    int * pgot_k5,
    int * pgot_cc
    );

#ifdef _WIN64
#define COMERR_DLL    "comerr64.dll"
#define KRB5_DLL      "krb5_64.dll"
#define CCAPI_DLL     "krbcc64.dll"
#else
#define COMERR_DLL    "comerr32.dll"
#define KRB5_DLL      "krb5_32.dll"
#define KRB4_DLL      "krbv4w32.dll"
#define CCAPI_DLL     "krbcc32.dll"
#endif

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

/* In order to avoid including the private CCAPI headers */
typedef int cc_int32;

#define CC_API_VER_1 1
#define CC_API_VER_2 2

#define CCACHE_API cc_int32

/*
** The Official Error Codes
*/
#define CC_NOERROR           0
#define CC_BADNAME           1
#define CC_NOTFOUND          2
#define CC_END               3
#define CC_IO                4
#define CC_WRITE             5
#define CC_NOMEM             6
#define CC_FORMAT            7
#define CC_LOCKED            8
#define CC_BAD_API_VERSION   9
#define CC_NO_EXIST          10
#define CC_NOT_SUPP          11
#define CC_BAD_PARM          12
#define CC_ERR_CACHE_ATTACH  13
#define CC_ERR_CACHE_RELEASE 14
#define CC_ERR_CACHE_FULL    15
#define CC_ERR_CRED_VERSION  16

enum {
    CC_CRED_VUNKNOWN = 0,       // For validation
    CC_CRED_V4 = 1,
    CC_CRED_V5 = 2,
    CC_CRED_VMAX = 3            // For validation
};

typedef struct opaque_dll_control_block_type* apiCB;
typedef struct _infoNC {
    char*     name;
    char*     principal;
    cc_int32  vers;
} infoNC;

TYPEDEF_FUNC(
CCACHE_API,
KRB5_CALLCONV_C,
cc_initialize,
    (
    apiCB** cc_ctx,           // <  DLL's primary control structure.
                              //    returned here, passed everywhere else
    cc_int32 api_version,     // >  ver supported by caller (use CC_API_VER_1)
    cc_int32*  api_supported, // <  if ~NULL, max ver supported by DLL
    const char** vendor       // <  if ~NULL, vendor name in read only C string
    )
);

TYPEDEF_FUNC(
CCACHE_API,
KRB5_CALLCONV_C,
cc_shutdown,
    (
    apiCB** cc_ctx            // <> DLL's primary control structure. NULL after
    )
);

TYPEDEF_FUNC(
CCACHE_API,
KRB5_CALLCONV_C,
cc_get_NC_info,
    (
    apiCB* cc_ctx,          // >  DLL's primary control structure
    struct _infoNC*** ppNCi // <  (NULL before call) null terminated,
                            //    list of a structs (free via cc_free_infoNC())
    )
);

TYPEDEF_FUNC(
CCACHE_API,
KRB5_CALLCONV_C,
cc_free_NC_info,
    (
    apiCB* cc_ctx,
    struct _infoNC*** ppNCi // <  free list of structs returned by
                            //    cc_get_cache_names().  set to NULL on return
    )
);
#endif /* __KUSER_H__ */
