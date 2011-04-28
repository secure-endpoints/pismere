#ifndef __LEASHWIN__
#define __LEASHWIN__

#include <krb.h>

typedef struct {
    int dlgtype;
#define DLGTYPE_PASSWD   0
#define DLGTYPE_CHPASSWD 1
    // Tells whether dialog box is in change pwd more or init ticket mode???
    // (verify this):
    int dlgstatemax; // What is this???
    // The title on the Dialog box - for Renewing or Initializing:
    LPSTR title;
    LPSTR principal;
} LSH_DLGINFO, FAR *LPLSH_DLGINFO;

typedef struct {
	DWORD size;
    int dlgtype;
#define DLGTYPE_PASSWD   0
#define DLGTYPE_CHPASSWD 1
    // Tells whether dialog box is in change pwd more or init ticket mode???
    // (verify this):
    LPSTR title;
    LPSTR username;
	LPSTR realm;
	int   use_defaults;
	int   forwardable;
	int   noaddresses;
	int   lifetime;
	int   renew_till;
	int   proxiable;
	int   publicip;
} LSH_DLGINFO_EX, FAR *LPLSH_DLGINFO_EX;

typedef struct {                                                
    char    principal[MAX_K_NAME_SZ]; /* Principal name/instance/realm */
    int     btickets;                 /* Do we have tickets? */
    long    lifetime;                 /* Lifetime -- needs to have
                                         room for 255 5-minute
                                         periods * 5 * 60 */
    long    issue_date;               /* The issue time */
    long    renew_till;               /* The Renew time (k5 only) */
} TICKETINFO;

int FAR Leash_kinit_dlg(HWND hParent, LPLSH_DLGINFO lpdlginfo);
int FAR Leash_kinit_dlg_ex(HWND hParent, LPLSH_DLGINFO_EX lpdlginfoex);
int FAR Leash_changepwd_dlg(HWND hParent, LPLSH_DLGINFO lpdlginfo);
int FAR Leash_changepwd_dlg_ex(HWND hParent, LPLSH_DLGINFO_EX lpdlginfo);

long FAR Leash_checkpwd(char *principal, char *password);
long FAR Leash_changepwd(char *principal, char *password, char *newpassword, char** result_string);
long FAR Leash_kinit(char *principal, char *password, int lifetime);
long FAR Leash_kinit_ex(char * principal, char * password, int lifetime,
						int forwardable, int proxiable, int renew_life,
						int addressless, unsigned long publicIP);

long FAR Leash_klist(HWND hlist, TICKETINFO FAR *ticketinfo);
long FAR Leash_kdestroy(void);
long FAR Leash_get_lsh_errno( LONG FAR *err_val);

BOOL Leash_set_help_file( char FAR *szHelpFile );
LPSTR Leash_get_help_file(void);

void Leash_reset_defaults(void);

#include <com_err.h>

/*
 * This is a hack needed because the real com_err.h does
 * not define err_func.  We need it in the case where
 * we pull in the real com_err instead of the krb4 
 * impostor.
 */
#ifndef _DCNS_MIT_COM_ERR_H
typedef LPSTR (*err_func)(int, long);
#endif

#include <krberr.h>
extern void Leash_initialize_krb_error_func(err_func func,struct et_list **);
#undef init_krb_err_func
#define init_krb_err_func(erf) Leash_initialize_krb_error_func(erf,&_et_list)

#include <kadm_err.h>

extern void Leash_initialize_kadm_error_table(struct et_list **);
#undef init_kadm_err_tbl
#define init_kadm_err_tbl() Leash_initialize_kadm_error_table(&_et_list)
#define kadm_err_base ERROR_TABLE_BASE_kadm

#define krb_err_func Leash_krb_err_func

#include <stdarg.h>
int lsh_com_err_proc (LPSTR whoami, long code,
		      LPSTR fmt, va_list args);
void FAR Leash_load_com_err_callback(FARPROC,FARPROC,FARPROC);

#define NO_TICKETS 0
#define EXPD_TICKETS 2
#define GOOD_TICKETS 1

#ifndef KRBERR
#define KRBERR(code) (code + krb_err_base)
#endif

/* Leash Configuration functions - alters Current User Registry */
DWORD Leash_get_default_lifetime();
DWORD Leash_set_default_lifetime(DWORD minutes);
DWORD Leash_reset_default_lifetime();
DWORD Leash_get_default_renew_till();
DWORD Leash_set_default_renew_till(DWORD minutes);
DWORD Leash_reset_default_renew_till();
DWORD Leash_get_default_renewable();
DWORD Leash_set_default_renewable(DWORD onoff);
DWORD Leash_reset_default_renewable();
DWORD Leash_get_default_forwardable();
DWORD Leash_set_default_forwardable(DWORD onoff);
DWORD Leash_reset_default_forwardable();
DWORD Leash_get_default_noaddresses();
DWORD Leash_set_default_noaddresses(DWORD onoff);
DWORD Leash_reset_default_noaddresses();
DWORD Leash_get_default_proxiable();
DWORD Leash_set_default_proxiable(DWORD onoff);
DWORD Leash_reset_default_proxiable();
DWORD Leash_get_default_publicip();
DWORD Leash_set_default_publicip(DWORD ipv4addr);
DWORD Leash_reset_default_publicip();
DWORD Leash_get_default_use_krb4();
DWORD Leash_set_default_use_krb4(DWORD onoff);
DWORD Leash_reset_default_use_krb4();
DWORD Leash_get_hide_kinit_options();
DWORD Leash_set_hide_kinit_options(DWORD onoff);
DWORD Leash_reset_hide_kinit_options();
DWORD Leash_get_default_life_min();
DWORD Leash_set_default_life_min(DWORD minutes);
DWORD Leash_reset_default_life_min();
DWORD Leash_get_default_life_max();
DWORD Leash_set_default_life_max(DWORD minutes);
DWORD Leash_reset_default_life_max();
DWORD Leash_get_default_renew_min();
DWORD Leash_set_default_renew_min(DWORD minutes);
DWORD Leash_reset_default_renew_min();
DWORD Leash_get_default_renew_max();
DWORD Leash_set_default_renew_max(DWORD minutes);
DWORD Leash_reset_default_renew_max();
DWORD Leash_get_lock_file_locations();
DWORD Leash_set_lock_file_locations(DWORD onoff);
DWORD Leash_reset_lock_file_locations();
DWORD Leash_get_default_uppercaserealm();
DWORD Leash_set_default_uppercaserealm(DWORD onoff);
DWORD Leash_reset_default_uppercaserealm();
#endif /* LEASHWIN */
