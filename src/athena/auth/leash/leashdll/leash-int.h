#ifndef __LEASH_INT_H__
#define __LEASH_INT_H__

#include <stdio.h>
#include <stdlib.h>

#include "leashdll.h"
#include <leashwin.h>

#include "tlhelp32.h"

#define MIT_PWD_DLL_CLASS "MITPasswordWndDLL"

BOOL
Register_MITPasswordEditControl(
    HINSTANCE hInst
    );

BOOL
Unregister_MITPasswordEditControl(
    HINSTANCE hInst
    );

// Some defines swiped from leash.h
//  These are necessary but they must be kept sync'ed with leash.h
#define HELPFILE "leash32.hlp"
extern char KRB_HelpFile[_MAX_PATH];

// Function Prototypes.
int lsh_com_err_proc (LPSTR whoami, long code, LPSTR fmt, va_list args);
int DoNiftyErrorReport(long errnum, LPSTR what);
LONG Leash_timesync(int);
BOOL Leash_ms2mit(BOOL);

#ifndef NO_AFS
int      not_an_API_LeashAFSGetToken(TICKETINFO * ticketinfo, TicketList** ticketList, char * kprinc);
long FAR not_an_API_LeashFreeTicketList(TicketList** ticketList) ;
#endif

// Crap...
#include <krb5.h>

long
Leash_int_kinit_ex(
    krb5_context ctx,
    HWND hParent,
    char * principal, 
    char * password, 
    int lifetime,
    int forwardable,
    int proxiable,
    int renew_life,
    int addressless,
    unsigned long publicIP,
    int displayErrors
    );

long
Leash_int_checkpwd(
    char * principal,
    char * password,
    int    displayErrors
    );

long
Leash_int_changepwd(
    char * principal, 
    char * password, 
    char * newpassword,
    char** result_string,
    int    displayErrors
    );

int
Leash_krb5_kdestroy(
    void
    );

int
Leash_krb5_kinit(
    krb5_context,
    HWND hParent,
    char * principal_name, 
    char * password,
    krb5_deltat lifetime,
    DWORD       forwardable,
    DWORD       proxiable,
    krb5_deltat renew_life,
    DWORD       addressless,
    DWORD       publicIP
    );

long
Leash_convert524(
    krb5_context ctx
    );
    
int
Leash_afs_unlog(
    void
    );

int
Leash_afs_klog(
    char *, 
    char *, 
    char *, 
    int
    );

int 
LeashKRB5_renew(void);

LONG
write_registry_setting(
    char* setting,
    DWORD type,
    void* buffer,
    size_t size
    );

LONG
read_registry_setting_user(
    char* setting,
    void* buffer,
    size_t size
    );

LONG
read_registry_setting(
    char* setting,
    void* buffer,
    size_t size
    );

BOOL
get_STRING_from_registry(
    HKEY hBaseKey,
    char * key,
    char * value,
    char * outbuf,
    DWORD  outlen
    );

BOOL
get_DWORD_from_registry(
    HKEY hBaseKey,
    char * key,
    char * value,
    DWORD * result
    );

int
config_boolean_to_int(
    const char *s
    );

#ifndef NO_KRB5
int Leash_krb5_error(krb5_error_code rc, LPCSTR FailedFunctionName, 
                     int FreeContextFlag, krb5_context *ctx,
                     krb5_ccache *cache);
int Leash_krb5_initialize(krb5_context *, krb5_ccache *);
#endif /* NO_KRB5 */

LPSTR err_describe(LPSTR buf, long code);

// toolhelp functions
TYPEDEF_FUNC(
    HANDLE,
    WINAPI,
    CreateToolhelp32Snapshot,
    (DWORD, DWORD)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    Module32First,
    (HANDLE, LPMODULEENTRY32)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    Module32Next,
    (HANDLE, LPMODULEENTRY32)
    );

// psapi functions
TYPEDEF_FUNC(
    DWORD,
    WINAPI,
    GetModuleFileNameExA,
    (HANDLE, HMODULE, LPSTR, DWORD)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    EnumProcessModules,
    (HANDLE, HMODULE*, DWORD, LPDWORD)
    );

#define pGetModuleFileNameEx pGetModuleFileNameExA
#define TOOLHELPDLL "kernel32.dll"
#define PSAPIDLL "psapi.dll"

// psapi functions
extern DECL_FUNC_PTR(GetModuleFileNameExA);
extern DECL_FUNC_PTR(EnumProcessModules);

// toolhelp functions
extern DECL_FUNC_PTR(CreateToolhelp32Snapshot);
extern DECL_FUNC_PTR(Module32First);
extern DECL_FUNC_PTR(Module32Next);

/* The following definitions are summarized from KRB4, KRB5, Leash32, and 
 * Leashw32 modules.  They are current as of KfW 2.5 Beta 4.  There is no
 * guarrantee that changes to other modules will be updated in this list.
 */

/* Must match the values used in Leash32.exe */
#define LEASH_SETTINGS_REGISTRY_KEY_NAME "Software\\MIT\\Leash32\\Settings"
#define LEASH_SETTINGS_REGISTRY_VALUE_AFS_STATUS       "AfsStatus"
#define LEASH_SETTINGS_REGISTRY_VALUE_DEBUG_WINDOW     "DebugWindow"
#define LEASH_SETTINGS_REGISTRY_VALUE_LARGE_ICONS      "LargeIcons"
#define LEASH_SETTINGS_REGISTRY_VALUE_DESTROY_TKTS     "DestroyTickets"
#define LEASH_SETTINGS_REGISTRY_VALUE_LOW_TKT_ALARM    "LowTicketAlarm"
#define LEASH_SETTINGS_REGISTRY_VALUE_AUTO_RENEW_TKTS  "AutoRenewTickets"
#define LEASH_SETTINGS_REGISTRY_VALUE_UPPERCASEREALM   "UpperCaseRealm"
#define LEASH_SETTINGS_REGISTRY_VALUE_TIMEHOST         "TIMEHOST"
#define LEASH_SETTINGS_REGISTRY_VALUE_CREATE_MISSING_CFG "CreateMissingConfig"

/* These values are defined and used within Leashw32.dll */
#define LEASH_REGISTRY_KEY_NAME "Software\\MIT\\Leash"
#define LEASH_REGISTRY_VALUE_LIFETIME "lifetime"
#define LEASH_REGISTRY_VALUE_RENEW_TILL "renew_till"
#define LEASH_REGISTRY_VALUE_RENEWABLE "renewable"
#define LEASH_REGISTRY_VALUE_FORWARDABLE "forwardable"
#define LEASH_REGISTRY_VALUE_NOADDRESSES "noaddresses"
#define LEASH_REGISTRY_VALUE_PROXIABLE "proxiable"
#define LEASH_REGISTRY_VALUE_PUBLICIP "publicip"
#define LEASH_REGISTRY_VALUE_USEKRB4 "usekrb4"
#define LEASH_REGISTRY_VALUE_KINIT_OPT "hide_kinit_options"
#define LEASH_REGISTRY_VALUE_LIFE_MIN "life_min"
#define LEASH_REGISTRY_VALUE_LIFE_MAX "life_max"
#define LEASH_REGISTRY_VALUE_RENEW_MIN "renew_min"
#define LEASH_REGISTRY_VALUE_RENEW_MAX "renew_max"
#define LEASH_REGISTRY_VALUE_LOCK_LOCATION "lock_file_locations"

/* must match values used within krbv4w32.dll */
#define KRB4_REGISTRY_KEY_NAME "Software\\MIT\\Kerberos4"
#define KRB4_REGISTRY_VALUE_CONFIGFILE  "config"
#define KRB4_REGISTRY_VALUE_KRB_CONF    "krb.conf"
#define KRB4_REGISTRY_VALUE_KRB_REALMS  "krb.realms"
#define KRB4_REGISTRY_VALUE_TICKETFILE  "ticketfile"

/* must match values used within krb5_32.dll */
#define KRB5_REGISTRY_KEY_NAME "Software\\MIT\\Kerberos5"
#define KRB5_REGISTRY_VALUE_CCNAME      "ccname"
#define KRB5_REGISTRY_VALUE_CONFIGFILE  "config"

/* must match values used within wshelper.dll */
#define WSHELP_REGISTRY_KEY_NAME  "Software\\MIT\\WsHelper"
#define WSHELP_REGISTRY_VALUE_DEBUG   "DebugOn"

#endif /* __LEASH_INT_H__ */
