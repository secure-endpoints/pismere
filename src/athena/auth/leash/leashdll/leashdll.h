#ifndef _LEASHDLL_H_
#define _LEASHDLL_H_

/* Internal Stuff */

#include <windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <ntsecapi.h>

extern HINSTANCE hKrb4;
extern HINSTANCE hKrb5;
extern HINSTANCE hProfile;

#define TIMEHOST "TIMEHOST"

#define LEASH_DEBUG_CLASS_GENERIC   0
#define LEASH_DEBUG_CLASS_KRB4      1
#define LEASH_DEBUG_CLASS_KRB4_APP  2

#define LEASH_PRIORITY_LOW  0
#define LEASH_PRIORITY_HIGH 1

typedef struct TicketList
{
    char* theTicket;
    struct TicketList* next;
    char* tktEncType;
    char* keyEncType;
    int   addrCount;
    char ** addrList;
} TicketList;

///////////////////////////////////////////////////////////////////////////////

#define LEASH_DLL     "leashw32.dll"
#define KRBCC32_DLL   "krbcc32.dll"
#define SERVICE_DLL   "advapi32.dll"
#define SECUR32_DLL   "secur32.dll"
#define PROFILE_DLL   "xpprof32.dll"

//////////////////////////////////////////////////////////////////////////////

#include <loadfuncs-com_err.h>
#include <loadfuncs-krb5.h>
#include <loadfuncs-profile.h>
#include <loadfuncs-krb.h>
#include <loadfuncs-krb524.h>
#include <loadfuncs-lsa.h>

#include <errno.h>

#ifndef NO_AFS
#include "afscompat.h"
#endif

// service definitions
typedef SC_HANDLE (WINAPI *FP_OpenSCManagerA)(char *, char *, DWORD);
typedef SC_HANDLE (WINAPI *FP_OpenServiceA)(SC_HANDLE, char *, DWORD);
typedef BOOL (WINAPI *FP_QueryServiceStatus)(SC_HANDLE, LPSERVICE_STATUS);
typedef BOOL (WINAPI *FP_CloseServiceHandle)(SC_HANDLE);

//////////////////////////////////////////////////////////////////////////////

// krb4 functions
extern DECL_FUNC_PTR(get_krb_err_txt_entry);
extern DECL_FUNC_PTR(k_isinst);
extern DECL_FUNC_PTR(k_isname);
extern DECL_FUNC_PTR(k_isrealm);
extern DECL_FUNC_PTR(kadm_change_your_password);
extern DECL_FUNC_PTR(kname_parse);
extern DECL_FUNC_PTR(krb_get_cred);
extern DECL_FUNC_PTR(krb_get_krbhst);
extern DECL_FUNC_PTR(krb_get_lrealm);
extern DECL_FUNC_PTR(krb_get_pw_in_tkt);
extern DECL_FUNC_PTR(krb_get_tf_realm);
extern DECL_FUNC_PTR(krb_mk_req);
extern DECL_FUNC_PTR(krb_realmofhost);
extern DECL_FUNC_PTR(tf_init);
extern DECL_FUNC_PTR(tf_close);
extern DECL_FUNC_PTR(tf_get_cred);
extern DECL_FUNC_PTR(tf_get_pname);
extern DECL_FUNC_PTR(tf_get_pinst);
extern DECL_FUNC_PTR(LocalHostAddr);
extern DECL_FUNC_PTR(tkt_string);
extern DECL_FUNC_PTR(krb_set_tkt_string);
extern DECL_FUNC_PTR(initialize_krb_error_func);
extern DECL_FUNC_PTR(initialize_kadm_error_table);
extern DECL_FUNC_PTR(dest_tkt);
extern DECL_FUNC_PTR(lsh_LoadKrb4LeashErrorTables); // XXX
extern DECL_FUNC_PTR(krb_in_tkt);
extern DECL_FUNC_PTR(krb_save_credentials);
extern DECL_FUNC_PTR(krb_get_krbconf2);
extern DECL_FUNC_PTR(krb_get_krbrealm2);

// krb5 functions
extern DECL_FUNC_PTR(krb5_change_password);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_init);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_tkt_life);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_renew_life);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_forwardable);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_proxiable);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_renew_life);
extern DECL_FUNC_PTR(krb5_get_init_creds_opt_set_address_list);
extern DECL_FUNC_PTR(krb5_get_init_creds_password);
extern DECL_FUNC_PTR(krb5_build_principal_ext);
extern DECL_FUNC_PTR(krb5_cc_resolve);
extern DECL_FUNC_PTR(krb5_cc_default);
extern DECL_FUNC_PTR(krb5_cc_default_name);
extern DECL_FUNC_PTR(krb5_cc_set_default_name);
extern DECL_FUNC_PTR(krb5_cc_initialize);
extern DECL_FUNC_PTR(krb5_cc_destroy);
extern DECL_FUNC_PTR(krb5_cc_close);
extern DECL_FUNC_PTR(krb5_cc_store_cred);
// extern DECL_FUNC_PTR(krb5_cc_retrieve_cred);
extern DECL_FUNC_PTR(krb5_cc_get_principal);
extern DECL_FUNC_PTR(krb5_cc_start_seq_get);
extern DECL_FUNC_PTR(krb5_cc_next_cred);
extern DECL_FUNC_PTR(krb5_cc_end_seq_get);
// extern DECL_FUNC_PTR(krb5_cc_remove_cred);
extern DECL_FUNC_PTR(krb5_cc_set_flags);
// extern DECL_FUNC_PTR(krb5_cc_get_type);
extern DECL_FUNC_PTR(krb5_free_context);
extern DECL_FUNC_PTR(krb5_free_cred_contents);
extern DECL_FUNC_PTR(krb5_free_principal);
extern DECL_FUNC_PTR(krb5_get_in_tkt_with_password);
extern DECL_FUNC_PTR(krb5_init_context);
extern DECL_FUNC_PTR(krb5_parse_name);
extern DECL_FUNC_PTR(krb5_timeofday);
extern DECL_FUNC_PTR(krb5_timestamp_to_sfstring);
extern DECL_FUNC_PTR(krb5_unparse_name);
extern DECL_FUNC_PTR(krb5_get_credentials);
extern DECL_FUNC_PTR(krb5_mk_req);
extern DECL_FUNC_PTR(krb5_sname_to_principal);
extern DECL_FUNC_PTR(krb5_get_credentials_renew);
extern DECL_FUNC_PTR(krb5_free_data);
extern DECL_FUNC_PTR(krb5_free_data_contents);
// extern DECL_FUNC_PTR(krb5_get_realm_domain);
extern DECL_FUNC_PTR(krb5_free_unparsed_name);
extern DECL_FUNC_PTR(krb5_os_localaddr);
extern DECL_FUNC_PTR(krb5_copy_keyblock_contents);
extern DECL_FUNC_PTR(krb5_copy_data);
extern DECL_FUNC_PTR(krb5_free_creds);
extern DECL_FUNC_PTR(krb5_build_principal);
extern DECL_FUNC_PTR(krb5_get_renewed_creds);
extern DECL_FUNC_PTR(krb5_free_addresses);
extern DECL_FUNC_PTR(krb5_get_default_config_files);
extern DECL_FUNC_PTR(krb5_free_config_files);
extern DECL_FUNC_PTR(krb5_get_default_realm);
extern DECL_FUNC_PTR(krb5_free_ticket);
extern DECL_FUNC_PTR(krb5_decode_ticket);

// Krb524 functions
extern DECL_FUNC_PTR(krb524_init_ets);
extern DECL_FUNC_PTR(krb524_convert_creds_kdc);

// ComErr functions
extern DECL_FUNC_PTR(com_err);
extern DECL_FUNC_PTR(error_message);

// Profile functions
extern DECL_FUNC_PTR(profile_init);
extern DECL_FUNC_PTR(profile_release);
extern DECL_FUNC_PTR(profile_get_subsection_names);
extern DECL_FUNC_PTR(profile_free_list);
extern DECL_FUNC_PTR(profile_get_string);
extern DECL_FUNC_PTR(profile_release_string);

// Service functions

extern DECL_FUNC_PTR(OpenSCManagerA);
extern DECL_FUNC_PTR(OpenServiceA);
extern DECL_FUNC_PTR(QueryServiceStatus);
extern DECL_FUNC_PTR(CloseServiceHandle);
extern DECL_FUNC_PTR(LsaNtStatusToWinError);

// LSA Functions

extern DECL_FUNC_PTR(LsaConnectUntrusted);
extern DECL_FUNC_PTR(LsaLookupAuthenticationPackage);
extern DECL_FUNC_PTR(LsaCallAuthenticationPackage);
extern DECL_FUNC_PTR(LsaFreeReturnBuffer);
extern DECL_FUNC_PTR(LsaGetLogonSessionData);

#endif /* _LEASHDLL_H_ */
