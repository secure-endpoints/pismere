#ifndef __LEASH_INT_H__
#define __LEASH_INT_H__

#ifdef _WIN32
#define EXPORT
#else
#define EXPORT _export
#endif

#include <stdio.h>
#include <stdlib.h>

// Some defines swiped from leash.h
//  These are necessary but they must be kept sync'ed with leash.h
#define HELPFILE "kerberos.hlp"
extern char KRB_HelpFile[_MAX_PATH];

// Function Prototypes.
int lsh_com_err_proc (LPSTR whoami, long code, LPSTR fmt, va_list args);
int EXPORT DoNiftyErrorReport(long errnum, LPSTR what);
LONG FAR PASCAL EXPORT MITPwdWinProcDLL(HWND hWnd, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL EXPORT PasswordProcDLL(HWND hDialog, WORD message, WORD wParam, LONG lParam);
LONG FAR PASCAL EXPORT lsh_get_lsh_errno( LONG FAR *err_val);
LONG Leash_timesync(int);

// Crap...
#include <krb5.h>

long
Leash_kinit_ex(
    char * principal, 
    char * password, 
    int lifetime,
    krb5_context ctx
    );

int
Leash_krb5_kdestroy(
    void
    );

int
Leash_krb5_kinit(
    char * principal_name, 
    char * password,
    krb5_deltat lifetime,
    krb5_context
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


LPSTR err_describe(LPSTR buf, long code);

#define LEASH_REG_SETTING_AFS_STATUS       "AfsStatus"
#define LEASH_REG_SETTING_KRB5_FORWARDABLE "ForwardableTicket"
#define LEASH_REG_SETTING_KRB5_PROXIABLE   "ProxiableTicket"
#define LEASH_REG_SETTING_UPPERCASEREALM   "UpperCaseRealm"

#endif /* __LEASH_INT_H__ */
