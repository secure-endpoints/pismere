/* ------------------------------------------------------------------------- */
/* Kerberos 5: */

#define krb5_free_unparsed_name pkrb5_free_unparsed_name
#define krb5_free_principal pkrb5_free_principal
#define krb5_free_context pkrb5_free_context
#define krb5_free_cred_contents pkrb5_free_cred_contents
#define krb5_free_ticket pkrb5_free_ticket
#define krb5_unparse_name pkrb5_unparse_name
#define krb5_timestamp_to_sfstring pkrb5_timestamp_to_sfstring
#define krb5_init_context pkrb5_init_context
#define krb5_cc_default pkrb5_cc_default
#define krb5_cc_resolve pkrb5_cc_resolve
#define krb5_timeofday pkrb5_timeofday
#define krb5_parse_name pkrb5_parse_name
#define krb5_build_principal_ext pkrb5_build_principal_ext
#define krb5_get_in_tkt_with_password pkrb5_get_in_tkt_with_password
#define krb5_get_in_tkt_with_keytab pkrb5_get_in_tkt_with_keytab
#define krb5_get_validated_creds pkrb5_get_validated_creds
#define krb5_get_renewed_creds pkrb5_get_renewed_creds
#define krb5_kt_default pkrb5_kt_default
#define krb5_kt_resolve pkrb5_kt_resolve
#define krb5_sname_to_principal pkrb5_sname_to_principal
#define krb5_decode_ticket pkrb5_decode_ticket
#define krb5_enctype_to_string pkrb5_enctype_to_string
#define krb5_timestamp_to_sfstring pkrb5_timestamp_to_sfstring
//
#define krb5_string_to_deltat pkrb5_string_to_deltat
#define krb5_string_to_timestamp pkrb5_string_to_timestamp
#define krb5_524_conv_principal pkrb5_524_conv_principal
#define krb5_get_prompt_types pkrb5_get_prompt_types
#define krb5_prompter_posix pkrb5_prompter_posix
#define krb5_get_init_creds_keytab pkrb5_get_init_creds_keytab
#define krb5_get_init_creds_password pkrb5_get_init_creds_password
#define krb5_free_addresses pkrb5_free_addresses
#define krb5_get_init_creds_opt_set_address_list pkrb5_get_init_creds_opt_set_address_list
#define krb5_os_localaddr pkrb5_os_localaddr
#define krb5_get_init_creds_opt_set_proxiable pkrb5_get_init_creds_opt_set_proxiable
#define krb5_get_init_creds_opt_set_forwardable pkrb5_get_init_creds_opt_set_forwardable
#define krb5_get_init_creds_opt_set_renew_life pkrb5_get_init_creds_opt_set_renew_life
#define krb5_get_init_creds_opt_set_tkt_life pkrb5_get_init_creds_opt_set_tkt_life
#define krb5_get_init_creds_opt_init pkrb5_get_init_creds_opt_init
#define krb5_read_password pkrb5_read_password

/* ------------------------------------------------------------------------- */
/* Kerberos 4: */

#define get_krb_err_txt_entry pget_krb_err_txt_entry
#define krb_realmofhost pkrb_realmofhost
#define tkt_string ptkt_string
#define k_isname pk_isname
#define k_isinst pk_isinst
#define k_isrealm pk_isrealm
#define tf_close ptf_close
#define tf_init ptf_init
#define tf_get_pinst ptf_get_pinst
#define tf_get_pname ptf_get_pname
#define tf_get_cred ptf_get_cred
#define kname_parse pkname_parse
#define krb_get_pw_in_tkt pkrb_get_pw_in_tkt
#define k_gethostname pk_gethostname
#define krb_get_lrealm pkrb_get_lrealm
#define krb_get_tf_realm pkrb_get_tf_realm
#define krb_get_cred pkrb_get_cred   
#define krb_mk_req pkrb_mk_req
#define krb_get_krbhst pkrb_get_krbhst
#define krb_get_tf_fullname pkrb_get_tf_fullname
#define krb_check_serv pkrb_check_serv
#define dest_tkt pdest_tkt

/* ------------------------------------------------------------------------- */
/* comerr: */
#define com_err pcom_err

/* ------------------------------------------------------------------------- */
#undef krb_get_err_text
#define krb_get_err_text pget_krb_err_txt_entry
