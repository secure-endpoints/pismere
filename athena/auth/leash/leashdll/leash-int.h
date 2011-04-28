#ifndef __LEASH_INT_H__
#define __LEASH_INT_H__

#include <stdio.h>
#include <stdlib.h>

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
