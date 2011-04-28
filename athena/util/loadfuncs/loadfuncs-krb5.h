#ifndef __LOADFUNCS_KRB5_H__
#define __LOADFUNCS_KRB5_H__

#include "loadfuncs.h"
#include <krb5.h>

#define KRB5_DLL      "krb5_32.dll"

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_principal,
    KRB5_PROTOTYPE((krb5_context, krb5_principal))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_authenticator,
    KRB5_PROTOTYPE((krb5_context, krb5_authenticator FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_authenticator_contents,
    KRB5_PROTOTYPE((krb5_context, krb5_authenticator FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_addresses,
    KRB5_PROTOTYPE((krb5_context, krb5_address FAR * FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_address,
    KRB5_PROTOTYPE((krb5_context, krb5_address FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_authdata,
    KRB5_PROTOTYPE((krb5_context, krb5_authdata FAR * FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_enc_tkt_part,
    KRB5_PROTOTYPE((krb5_context, krb5_enc_tkt_part FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_ticket,
    KRB5_PROTOTYPE((krb5_context, krb5_ticket FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_tickets,
    KRB5_PROTOTYPE((krb5_context, krb5_ticket FAR * FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_kdc_req,
    KRB5_PROTOTYPE((krb5_context, krb5_kdc_req FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_kdc_rep,
    KRB5_PROTOTYPE((krb5_context, krb5_kdc_rep FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_last_req,
    KRB5_PROTOTYPE((krb5_context, krb5_last_req_entry FAR * FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_enc_kdc_rep_part,
    KRB5_PROTOTYPE((krb5_context, krb5_enc_kdc_rep_part FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_error,
    KRB5_PROTOTYPE((krb5_context, krb5_error FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_ap_req,
    KRB5_PROTOTYPE((krb5_context, krb5_ap_req FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_ap_rep,
    KRB5_PROTOTYPE((krb5_context, krb5_ap_rep FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_safe,
    KRB5_PROTOTYPE((krb5_context, krb5_safe FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_priv,
    KRB5_PROTOTYPE((krb5_context, krb5_priv FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_priv_enc_part,
    KRB5_PROTOTYPE((krb5_context, krb5_priv_enc_part FAR * ))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_cred,
    KRB5_PROTOTYPE((krb5_context, krb5_cred FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_creds,
    KRB5_PROTOTYPE((krb5_context, krb5_creds FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_cred_contents,
    KRB5_PROTOTYPE((krb5_context, krb5_creds FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_cred_enc_part,
    KRB5_PROTOTYPE((krb5_context, krb5_cred_enc_part FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_checksum,
    KRB5_PROTOTYPE((krb5_context, krb5_checksum FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_checksum_contents,
    KRB5_PROTOTYPE((krb5_context, krb5_checksum FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_keyblock,
    KRB5_PROTOTYPE((krb5_context, krb5_keyblock FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_keyblock_contents,
    KRB5_PROTOTYPE((krb5_context, krb5_keyblock FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_pa_data,
    KRB5_PROTOTYPE((krb5_context, krb5_pa_data FAR * FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_ap_rep_enc_part,
    KRB5_PROTOTYPE((krb5_context, krb5_ap_rep_enc_part FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_tkt_authent,
    KRB5_PROTOTYPE((krb5_context, krb5_tkt_authent FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_pwd_data,
    KRB5_PROTOTYPE((krb5_context, krb5_pwd_data FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_pwd_sequences,
    KRB5_PROTOTYPE((krb5_context, passwd_phrase_element FAR * FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_data,
    KRB5_PROTOTYPE((krb5_context, krb5_data FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_data_contents,
    KRB5_PROTOTYPE((krb5_context, krb5_data FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_unparsed_name,
    KRB5_PROTOTYPE((krb5_context, char FAR *))
    );
TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_cksumtypes,
    KRB5_PROTOTYPE((krb5_context, krb5_cksumtype FAR *))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_encrypt,
    KRB5_PROTOTYPE((krb5_context context, krb5_const krb5_keyblock *key,
		    krb5_keyusage usage, krb5_const krb5_data *ivec,
		    krb5_const krb5_data *input, krb5_enc_data *output))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_decrypt,
    KRB5_PROTOTYPE((krb5_context context, krb5_const krb5_keyblock *key,
		    krb5_keyusage usage, krb5_const krb5_data *ivec,
		    krb5_const krb5_enc_data *input, krb5_data *output))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_encrypt_length,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype enctype,
		    size_t inputlen, size_t *length))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_block_size,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype enctype,
		    size_t *blocksize))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_make_random_key,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype enctype,
		    krb5_keyblock *random_key))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_random_make_octets,
    KRB5_PROTOTYPE((krb5_context context, krb5_data *data))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_random_seed,
    KRB5_PROTOTYPE((krb5_context context, krb5_data *data))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_string_to_key,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype enctype,
		    krb5_const krb5_data *string, krb5_const krb5_data *salt,
		    krb5_keyblock *key))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_enctype_compare,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype e1, krb5_enctype e2,
		    krb5_boolean *similar))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_make_checksum,
    KRB5_PROTOTYPE((krb5_context context, krb5_cksumtype cksumtype,
		    krb5_const krb5_keyblock *key, krb5_keyusage usage,
		    krb5_const krb5_data *input, krb5_checksum *cksum))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_verify_checksum,
    KRB5_PROTOTYPE((krb5_context context, 
		    krb5_const krb5_keyblock *key, krb5_keyusage usage,
		    krb5_const krb5_data *data,
		    krb5_const krb5_checksum *cksum,
		    krb5_boolean *valid))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_checksum_length,
    KRB5_PROTOTYPE((krb5_context context, krb5_cksumtype cksumtype,
		    size_t *length))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_c_keyed_checksum_types,
    KRB5_PROTOTYPE((krb5_context context, krb5_enctype enctype, 
		    unsigned int *count, krb5_cksumtype **cksumtypes))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_boolean,
    KRB5_CALLCONV,
    valid_enctype,
    KRB5_PROTOTYPE((krb5_const krb5_enctype ktype))
    );

TYPEDEF_FUNC(
    krb5_boolean,
    KRB5_CALLCONV,
    valid_cksumtype,
    KRB5_PROTOTYPE((krb5_const krb5_cksumtype ctype))
    );

TYPEDEF_FUNC(
    krb5_boolean,
    KRB5_CALLCONV,
    is_coll_proof_cksum,
    KRB5_PROTOTYPE((krb5_const krb5_cksumtype ctype))
    );

TYPEDEF_FUNC(
    krb5_boolean,
    KRB5_CALLCONV,
    is_keyed_cksum,
    KRB5_PROTOTYPE((krb5_const krb5_cksumtype ctype))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_init_context,
    KRB5_PROTOTYPE((krb5_context FAR *))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_context,
    KRB5_PROTOTYPE((krb5_context))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_decrypt_tkt_part,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_keyblock FAR *,
                    krb5_ticket FAR * ))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_tgt_creds,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_creds FAR * FAR* ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_credentials,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_creds FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_credentials_validate,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_creds FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_credentials_renew,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_creds FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_req,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *,
                    krb5_const krb5_flags,
                    char FAR *,
                    char FAR *,
                    krb5_data FAR *,
                    krb5_ccache,
                    krb5_data FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_req_extended,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *,
                    krb5_const krb5_flags,
                    krb5_data FAR *,
                    krb5_creds FAR *,
                    krb5_data FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_rep,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_rep,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_const krb5_data FAR *,
                    krb5_ap_rep_enc_part FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_error,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_error FAR *,
                    krb5_data FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_error,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_data FAR *,
                    krb5_error FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_safe,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_const krb5_data FAR *,
                    krb5_data FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_priv,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_const krb5_data FAR *,
                    krb5_data FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_parse_name,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const char FAR *,
                    krb5_principal FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_unparse_name,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const_principal,
                    char FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_unparse_name_ext,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const_principal,
                    char FAR * FAR *,
                    int FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_set_principal_realm,
    KRB5_PROTOTYPE((krb5_context, krb5_principal, const char FAR *))
    );

TYPEDEF_FUNC(
    krb5_boolean,
    KRB5_CALLCONV,
    krb5_principal_compare,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const_principal,
                    krb5_const_principal))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_keyblock,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_keyblock FAR *,
                    krb5_keyblock FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_keyblock_contents,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_keyblock FAR *,
                    krb5_keyblock FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_creds,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_creds FAR *,
                    krb5_creds FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_data,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_data FAR *,
                    krb5_data FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_principal,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const_principal,
                    krb5_principal FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_addr,
    KRB5_PROTOTYPE((krb5_context,
                    const krb5_address FAR *,
                    krb5_address FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_addresses,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_address FAR * krb5_const FAR *,
                    krb5_address FAR * FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_ticket,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_ticket FAR *,
                    krb5_ticket FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_authdata,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_authdata FAR * krb5_const FAR *,
                    krb5_authdata FAR * FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_authenticator,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_authenticator FAR *,
                    krb5_authenticator FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_copy_checksum,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_checksum FAR *,
                    krb5_checksum FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_server_rcache,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_data *, krb5_rcache *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV_C,
    krb5_build_principal_ext,
    KRB5_STDARG_P((krb5_context, krb5_principal FAR *, int, krb5_const char FAR *, ...))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV_C,
    krb5_build_principal,
    KRB5_STDARG_P((krb5_context, krb5_principal FAR *, int, krb5_const char FAR *, ...))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_425_conv_principal,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const char FAR *name,
                    krb5_const char FAR *instance, krb5_const char FAR *realm,
                    krb5_principal FAR *princ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_524_conv_principal,
    KRB5_PROTOTYPE((krb5_context context, krb5_const krb5_principal princ, 
                    char FAR *name, char FAR *inst, char FAR *realm))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_chpw_req,
    KRB5_PROTOTYPE((krb5_context context, krb5_auth_context auth_context,
                    krb5_data *ap_req, char *passwd, krb5_data *packet))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_chpw_rep,
    KRB5_PROTOTYPE((krb5_context context, krb5_auth_context auth_context,
                    krb5_data *packet, int *result_code,
                    krb5_data *result_data))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_chpw_result_code_string,
    KRB5_PROTOTYPE((krb5_context context, int result_code,
                    char **result_codestr))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_register,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_kt_ops FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_resolve,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const char FAR *,
                    krb5_keytab FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_default_name,
    KRB5_PROTOTYPE((krb5_context,
                    char FAR *,
                    int ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_default,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_keytab FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_free_entry,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_keytab_entry FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_remove_entry,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_keytab,
                    krb5_keytab_entry FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_add_entry,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_keytab,
                    krb5_keytab_entry FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cc_resolve,
    KRB5_PROTOTYPE((krb5_context,
                    const char FAR *,
                    krb5_ccache FAR * ))
    );

TYPEDEF_FUNC(
    const char FAR*,
    KRB5_CALLCONV,
    krb5_cc_default_name,
    KRB5_PROTOTYPE((krb5_context))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cc_set_default_name,
    KRB5_PROTOTYPE((krb5_context, const char *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cc_default,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_ccache FAR *))
    );

TYPEDEF_FUNC(
    unsigned int,
    KRB5_CALLCONV,
    krb5_get_notification_message,
    KRB5_PROTOTYPE((void))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cc_copy_creds,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_ccache incc,
                    krb5_ccache outcc))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_us_timeofday,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_int32 FAR *,
                    krb5_int32 FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_timeofday,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_int32 FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_os_localaddr,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_address FAR * FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_default_realm,
    KRB5_PROTOTYPE((krb5_context,
                    char FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_set_default_realm,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const char FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_sname_to_principal,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const char FAR *,
                    krb5_const char FAR *,
                    krb5_int32,
                    krb5_principal FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_change_password,
    KRB5_PROTOTYPE((krb5_context context, krb5_creds *creds, char *newpw,
                    int *result_code, krb5_data *result_code_string,
                    krb5_data *result_string))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_default_config_files,
    KRB5_PROTOTYPE((char ***filenames))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_config_files,
    KRB5_PROTOTYPE((char **filenames))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_in_tkt,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_address FAR * krb5_const FAR *,
                    krb5_enctype FAR *,
                    krb5_preauthtype FAR *,
                    krb5_error_code ( FAR * )(krb5_context,
                                              krb5_const krb5_enctype,
                                              krb5_data FAR *,
                                              krb5_const_pointer,
                                              krb5_keyblock FAR * FAR *),
                    krb5_const_pointer,
                    krb5_error_code ( FAR * )(krb5_context,
                                              krb5_const krb5_keyblock FAR *,
                                              krb5_const_pointer,
                                              krb5_kdc_rep FAR * ),
                    krb5_const_pointer,
                    krb5_creds FAR *,
                    krb5_ccache,
                    krb5_kdc_rep FAR * FAR * ))
    );


TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_in_tkt_with_password,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_address FAR * krb5_const FAR *,
                    krb5_enctype FAR *,
                    krb5_preauthtype FAR *,
                    krb5_const char FAR *,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_kdc_rep FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_in_tkt_with_skey,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_address FAR * krb5_const FAR *,
                    krb5_enctype FAR *,
                    krb5_preauthtype FAR *,
                    krb5_const krb5_keyblock FAR *,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_kdc_rep FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_in_tkt_with_keytab,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_const krb5_flags,
                    krb5_address FAR * krb5_const FAR *,
                    krb5_enctype FAR *,
                    krb5_preauthtype FAR *,
                    krb5_const krb5_keytab,
                    krb5_ccache,
                    krb5_creds FAR *,
                    krb5_kdc_rep FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_req,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *,
                    krb5_const krb5_data FAR *,
                    krb5_const_principal,
                    krb5_keytab,
                    krb5_flags FAR *,
                    krb5_ticket FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_kt_read_service_key,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_pointer,
                    krb5_principal,
                    krb5_kvno,
                    krb5_enctype,
                    krb5_keyblock FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_safe,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_const krb5_data FAR *,
                    krb5_data FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_priv,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_const krb5_data FAR *,
                    krb5_data FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cc_register,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_cc_ops FAR *,
                    krb5_boolean ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_sendauth,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *,
                    krb5_pointer,
                    char FAR *,
                    krb5_principal,
                    krb5_principal,
                    krb5_flags,
                    krb5_data FAR *,
                    krb5_creds FAR *,
                    krb5_ccache,
                    krb5_error FAR * FAR *,
                    krb5_ap_rep_enc_part FAR * FAR *,
                    krb5_creds FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_recvauth,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *,
                    krb5_pointer,
                    char FAR *,
                    krb5_principal,
                    krb5_int32, 
                    krb5_keytab,
                    krb5_ticket FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_ncred,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_creds FAR * FAR *,
                    krb5_data FAR * FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_mk_1cred,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_creds FAR *,
                    krb5_data FAR * FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_rd_cred,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_data FAR *,
                    krb5_creds FAR * FAR * FAR *,
                    krb5_replay_data FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_fwd_tgt_creds,
    KRB5_PROTOTYPE((krb5_context, 
                    krb5_auth_context,
                    char FAR *,
                    krb5_principal, 
                    krb5_principal, 
                    krb5_ccache,
                    int forwardable,
                    krb5_data FAR *))
    );
	
TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_init,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_free,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_setflags,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_int32))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getflags,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_int32 FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_setuseruserkey,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_keyblock FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getkey,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_keyblock **))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getlocalsubkey,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_keyblock FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_set_req_cksumtype,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_cksumtype))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getlocalseqnumber,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_int32 FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getremoteseqnumber,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_int32 FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_setrcache,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_rcache))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getauthenticator,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_authenticator FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_getremotesubkey,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    krb5_keyblock FAR * FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_read_password,
    KRB5_PROTOTYPE((krb5_context,
                    const char FAR *,
                    const char FAR *,
                    char FAR *,
                    int FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_host_realm,
    KRB5_PROTOTYPE((krb5_context,
                    const char FAR *,
                    char FAR * FAR * FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_free_host_realm,
    KRB5_PROTOTYPE((krb5_context,
                    char FAR * const FAR * ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_realm_domain,
    KRB5_PROTOTYPE((krb5_context,
                    const char *,
                    char ** ))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_auth_con_genaddrs,
    KRB5_PROTOTYPE((krb5_context,
                    krb5_auth_context,
                    int, int))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_enctype,
    KRB5_PROTOTYPE((char FAR *, krb5_enctype FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_salttype,
    KRB5_PROTOTYPE((char FAR *, krb5_int32 FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_cksumtype,
    KRB5_PROTOTYPE((char FAR *, krb5_cksumtype FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_timestamp,
    KRB5_PROTOTYPE((char FAR *, krb5_timestamp FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_string_to_deltat,
    KRB5_PROTOTYPE((char FAR *, krb5_deltat FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_enctype_to_string,
    KRB5_PROTOTYPE((krb5_enctype, char FAR *, size_t))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_salttype_to_string,
    KRB5_PROTOTYPE((krb5_int32, char FAR *, size_t))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_cksumtype_to_string,
    KRB5_PROTOTYPE((krb5_cksumtype, char FAR *, size_t))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_timestamp_to_string,
    KRB5_PROTOTYPE((krb5_timestamp, char FAR *, size_t))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_timestamp_to_sfstring,
    KRB5_PROTOTYPE((krb5_timestamp, char FAR *, size_t, char FAR *))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_deltat_to_string,
    KRB5_PROTOTYPE((krb5_deltat, char FAR *, size_t))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_prompter_posix,
    KRB5_PROTOTYPE((krb5_context context,
                    void *data,
                    const char *name,
                    const char *banner,
                    int num_prompts,
                    krb5_prompt prompts[]))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_init,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_tkt_life,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_deltat tkt_life))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_renew_life,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_deltat renew_life))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_forwardable,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    int forwardable))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_proxiable,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    int proxiable))
    );


TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_etype_list,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_enctype *etype_list,
                    int etype_list_length))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_address_list,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_address **addresses))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_preauth_list,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_preauthtype *preauth_list,
                    int preauth_list_length))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_get_init_creds_opt_set_salt,
    KRB5_PROTOTYPE((krb5_get_init_creds_opt *opt,
                    krb5_data *salt))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_init_creds_password,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_creds *creds,
                    krb5_principal client,
                    char *password,
                    krb5_prompter_fct prompter,
                    void *data,
                    krb5_deltat start_time,
                    char *in_tkt_service,
                    krb5_get_init_creds_opt *options))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_init_creds_keytab,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_creds *creds,
                    krb5_principal client,
                    krb5_keytab arg_keytab,
                    krb5_deltat start_time,
                    char *in_tkt_service,
                    krb5_get_init_creds_opt *options))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_verify_init_creds_opt_init,
    KRB5_PROTOTYPE((krb5_verify_init_creds_opt *options))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_verify_init_creds_opt_set_ap_req_nofail,
    KRB5_PROTOTYPE((krb5_verify_init_creds_opt *options,
                    int ap_req_nofail))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_verify_init_creds,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_creds *creds,
                    krb5_principal ap_req_server,
                    krb5_keytab ap_req_keytab,
                    krb5_ccache *ccache,
                    krb5_verify_init_creds_opt *options))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_validated_creds,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_creds *creds,
                    krb5_principal client,
                    krb5_ccache ccache,
                    char *in_tkt_service))
    );


TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_get_renewed_creds,
    KRB5_PROTOTYPE((krb5_context context,
                    krb5_creds *creds,
                    krb5_principal client,
                    krb5_ccache ccache,
                    char *in_tkt_service))
    );

/* ------------------------------------------------------------------------- */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_realm_iterator_create,
    KRB5_PROTOTYPE((krb5_context context, void **iter_p))
    );

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_realm_iterator,
    KRB5_PROTOTYPE((krb5_context context, void **iter_p, char **ret_realm))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_realm_iterator_free,
    KRB5_PROTOTYPE((krb5_context context, void **iter_p))
    );

TYPEDEF_FUNC(
    void,
    KRB5_CALLCONV,
    krb5_free_realm_string,
    KRB5_PROTOTYPE((krb5_context context, char *str))
    );

TYPEDEF_FUNC(
    krb5_prompt_type*,
    KRB5_CALLCONV,
    krb5_get_prompt_types,
    KRB5_PROTOTYPE((krb5_context context))
    );

/* NOT IN krb5.h HEADER: */

TYPEDEF_FUNC(
    krb5_error_code,
    KRB5_CALLCONV,
    krb5_decode_ticket,
    KRB5_PROTOTYPE((const krb5_data *code, krb5_ticket **rep))
    );

#endif /* __LOADFUNCS_KRB5_H__ */
