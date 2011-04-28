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

/* ------------------------------------------------------------------------- */

#include "loadfuncs-krb5.h"

extern FUNC_INFO k5_fi[];

extern DECL_FUNC_PTR(krb5_free_unparsed_name);
extern DECL_FUNC_PTR(krb5_free_principal);
extern DECL_FUNC_PTR(krb5_free_context);
extern DECL_FUNC_PTR(krb5_free_cred_contents);
extern DECL_FUNC_PTR(krb5_free_ticket);
extern DECL_FUNC_PTR(krb5_unparse_name);
extern DECL_FUNC_PTR(krb5_timestamp_to_sfstring);
extern DECL_FUNC_PTR(krb5_init_context);
extern DECL_FUNC_PTR(krb5_cc_default);
extern DECL_FUNC_PTR(krb5_cc_resolve);
extern DECL_FUNC_PTR(krb5_timeofday);
extern DECL_FUNC_PTR(krb5_parse_name);
extern DECL_FUNC_PTR(krb5_build_principal_ext);
extern DECL_FUNC_PTR(krb5_get_in_tkt_with_password);
extern DECL_FUNC_PTR(krb5_get_in_tkt_with_keytab);
extern DECL_FUNC_PTR(krb5_get_validated_creds);
extern DECL_FUNC_PTR(krb5_get_renewed_creds);
extern DECL_FUNC_PTR(krb5_kt_default);
extern DECL_FUNC_PTR(krb5_kt_resolve);
extern DECL_FUNC_PTR(krb5_sname_to_principal);
extern DECL_FUNC_PTR(decode_krb5_ticket);
extern DECL_FUNC_PTR(krb5_enctype_to_string);

/* ------------------------------------------------------------------------- */

#include "loadfuncs-krb.h"

extern FUNC_INFO k4_fi[];

extern DECL_FUNC_PTR(get_krb_err_txt_entry);
extern DECL_FUNC_PTR(krb_realmofhost);
extern DECL_FUNC_PTR(tkt_string);
extern DECL_FUNC_PTR(k_isname);
extern DECL_FUNC_PTR(k_isinst);
extern DECL_FUNC_PTR(k_isrealm);
extern DECL_FUNC_PTR(tf_close);
extern DECL_FUNC_PTR(tf_init);
extern DECL_FUNC_PTR(tf_get_pinst);
extern DECL_FUNC_PTR(tf_get_pname);
extern DECL_FUNC_PTR(tf_get_cred);
extern DECL_FUNC_PTR(kname_parse);
extern DECL_FUNC_PTR(krb_get_pw_in_tkt);
extern DECL_FUNC_PTR(k_gethostname);
extern DECL_FUNC_PTR(krb_get_lrealm);
extern DECL_FUNC_PTR(krb_get_tf_realm);
extern DECL_FUNC_PTR(krb_get_cred);   
extern DECL_FUNC_PTR(krb_mk_req);
extern DECL_FUNC_PTR(krb_get_krbhst);
extern DECL_FUNC_PTR(krb_get_tf_fullname);
extern DECL_FUNC_PTR(krb_check_serv);
extern DECL_FUNC_PTR(dest_tkt);

/* ------------------------------------------------------------------------- */

#include "loadfuncs-com_err.h"

extern FUNC_INFO ce_fi[];

extern DECL_FUNC_PTR(com_err);

#endif /* __KUSER_H__ */
