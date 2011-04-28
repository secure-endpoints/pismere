/* Taken from the macintosh src file.
 */

#include <conf.h>

#if !defined(PC)
#include <sys/param.h>
#include <pwd.h>
#include <krb_err.h>
#include <kadm_err.h>
#include <kadm.h>
#else
#ifdef WINDOWS
#include <windows.h>
#endif
#include <com_err.h>
#include "krberr.h"
#include <krb.h>
#include <memory.h>
#include "leasherr.h"
/* #include "krblayer.h" */
/* #include <kadmerr.h> */
#include <kadm.h>

#ifndef KRBERR
#define KRBERR(code) (code + krb_err_base)
#endif

#include <time.h>

extern LPSTR FAR PASCAL get_krb_err_txt_entry(int i);

#ifdef DEBUG01 /* in the prototype file now. */
extern int kadm_check_pw(des_cblock newkey, char *pwstring,
                         u_char **retstring);
extern int kadm_change_pw2(des_cblock newkey, char *pwstring,
                           u_char **retstring);

#endif
#endif /*MSDOS*/

#include "kadmprot.h" /* prototypes */

#define WINDOWS_NEEDS_THIS_STUFF 0 /* Windows doesn't need this stuff */

#define BAD_PW 1
#define GOOD_PW 0
#define FUDGE_VALUE 15          /* for ticket expiration time */
#define PE_NO 0
#define PE_YES 1
#define PE_UNSURE 2

#define LEN 64

/* for get_password, whether it should do the swapping...necessary for
   using vals structure, unnecessary for change_pw requests */
#define DONTSWAP 0
#define SWAP 1


#ifdef DEBUG01 /* in prototype file */
extern long /* int */ kadm_init_link();
#endif

extern void krb_set_tkt_string();

static int multiple = 0;        /* Allow multiple requests per ticket */

#ifdef KADMC1
extern char default_realm[];
extern char current_realm[];
#else
char default_realm[REALM_SZ];
char current_realm[REALM_SZ];
#endif

void do_kadmin_init();
#define TOO_BIG -1

/* moved here from kadmc1.c */
long get_pw_in_tkt(username, password)
    char *username, *password;
{
    long status;                        /* return code */
    char name[ANAME_SZ+1];
    char instance[INST_SZ+1];
    char realm[REALM_SZ+1];
    char emsg[256];

    status = 0; /* just to be sure */

    do_kadmin_init();

    *name = '\0';
    *instance = '\0';
    *realm = '\0';

    if (status = kname_parse(name, instance, realm, username)) {
#ifdef DEBUG01
        wsprintf(emsg, "kerberos error: %s\n", get_krb_err_txt_entry(status));
        KRBERR(status);
        /* do_error(emsg); */
#endif
        return status;
    }

    if (!realm[0]) strcpy(realm, current_realm);
    if (!realm[0]) strcpy(realm, default_realm);

    status = krb_get_pw_in_tkt(name, instance, realm, PWSERV_NAME,
                               KADM_SINST, 2, password);
    return(status);
}

long kadm_change_your_password(
    char *principal, char *old_password,
    char *new_password, 
#ifdef WINDOWS
    HANDLE* pinfo_desc
#else
    char** pinfo_desc
#endif
    )
{
    des_cblock new_key;
    char *ret_st;

    char    aname[ANAME_SZ];
    char    inst[INST_SZ];
    char    realm[REALM_SZ];
    char    buf[LEN];
    long    k_errno;
#ifdef WINDOWS
    HANDLE old_ticket = 0;
#else
    char *old_ticket = 0;
#endif
    char* info_desc = 0;

    inst[0] = '\0';
    realm[0] = '\0';

    if (principal &&
        (k_errno = kname_parse(aname, inst, realm, principal))
        != KSUCCESS) {
        return KRBERR(k_errno);
    }

    if (k_gethostname(buf, LEN)) {
        return KRBERR(k_errno);
    }
    if (!*realm && krb_get_lrealm(realm, 1)) {
        return LSH_FAILEDREALM;
    }

    if ((!*aname) || (!k_isname(aname)))
        return LSH_INVPRINCIPAL;

    /* optional instance */
    if (!k_isinst(inst))
        return LSH_INVINSTANCE;

    if (!k_isrealm(realm))
        return LSH_INVREALM;

    do_kadmin_init();

#if !defined(OS2) && !defined(WIN32) && defined(TKT_KERBMEM)    /* XXX */
    k_errno = kadm_copy_tickets(principal, &old_ticket);
#endif
    if( k_errno != AD_OK ){
        free(old_ticket);
        old_ticket = NULL;
    }

    k_errno = get_pw_in_tkt(principal, old_password);
    if (k_errno != 0) goto out;

    if ((k_errno = kadm_init_link(PWSERV_NAME, KADM_SINST, realm))
        != KADM_SUCCESS)
        goto out;

#ifdef NOENCRYPTION
    bzero((char *) new_key, sizeof(des_cblock));
    new_key[0] = (unsigned char) 1;
#else
#ifdef AFS
    {
        extern int remember_the_function;
        extern int (*next_string_to_key[])();
        extern int krb_debug;

        if (krb_debug)
            kdebug("Encoding new password with "
                   "next_string_to_key[%d]\n",
                   remember_the_function);
        (*next_string_to_key[ remember_the_function ])
            ( new_password, new_key, realm);
    }
#else /*the MIT way*/
    (void) des_string_to_key(new_password, new_key);
#endif /* AFS */
#endif /* NOENCRYPTION */

    /*  wade - this will not work *ret_st = NULL;
        ret_st = NULL;
    */
#ifdef CHECK_ONLY
    k_errno = kadm_check_pw(new_key, new_password, &ret_st);
    if( k_errno != 0 ){
        if( ret_st != NULL ){
            char tmp_buf[1024];
            wsprintf( (LPSTR) tmp_buf, (LPSTR) ret_st );
            MessageBox( NULL, (LPSTR) tmp_buf, (LPSTR) "Weak Password", MB_OK | MB_ICONSTOP );
        } else {
            MessageBox( NULL, (LPSTR) "You selected a weak password, please "
                        "select a better one.", (LPSTR) "Weak Password",
                        MB_OK | MB_ICONSTOP );
        }
    }

#else /* ! CHECK_ONLY */
    k_errno = kadm_change_pw2(new_key, new_password, &ret_st);
    if (k_errno != 0) {
#ifdef DEBUG01
        if (ret_st != NULL) {
            char tmp_buf[1024];
            wsprintf( (LPSTR) tmp_buf, (LPSTR) ret_st );
            MessageBox(NULL, tmp_buf, "Weak Password",
                       MB_OK | MB_ICONSTOP );
        } else {
            MessageBox(NULL,
                       "You selected a weak password, "
                       "please select a better one.",
                       "Weak Password",
                       MB_OK | MB_ICONSTOP);
        }
#else /* ! DEBUG01 */
        /* This is the "normal" code path */
        if (ret_st && pinfo_desc) {
#ifdef WINDOWS
            info_desc = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                    lstrlen(ret_st));
#else
            info_desc = malloc(lstrlen(ret_st));
#endif
            if (info_desc)
                lstrcpyn(info_desc, ret_st, lstrlen(ret_st));
            *pinfo_desc = info_desc;
        }
#endif /* ! DEBUG01 */
    }
#endif /* ! CHECK_ONLY */

 out:
#if !defined(OS2) && !defined(WIN32) && defined(TKT_KERBMEM) /* XXX */
    kadm_copy_back_tickets( old_ticket, principal);
#endif
    return(k_errno);
}

cv_nl_cr(c)
    char *c;
{
    while (*c) {
        if (*c == 7) *c = ' '; /* bell */
        if (*c == '\n') *c = '\r';
        *c++;
    }
}

/********************************************************
 *                                                      *
 * change_password                                      *
 *                                                      *
 ********************************************************/

#if WINDOWS_NEEDS_THIS_STUFF
int
kadm_change_password(admin_name, username, new_password)
    char *admin_name, *username, *new_password;
{
    Kadm_vals old, new;
    int k_errno;

    do_kadmin_init();

    k_errno = setvals(&old, username);
    if (k_errno != KADM_SUCCESS)        return(k_errno);

    new = old;

    SET_FIELD(KADM_DESKEY,new.fields);

    if (princ_exists(old.name, old.instance, current_realm) != PE_NO) {
        /* get the admin's password */
        if (get_admin_ticket(admin_name) != GOOD_PW)
            return(-1);

        if (get_key(&new.key_low, &new.key_high, SWAP, new_password) == GOOD_PW) {
            k_errno = kadm_mod(&old, &new);
            return(k_errno);
        }
        else return(-2);
        bzero((char *)&new, sizeof(new));
#ifndef NO_MULTIPLE
        if (!multiple)
            //clean_up();
#endif
            }
    else return(KDC_PR_UNKNOWN);

    return(0);
}

/********************************************************
 *                                                      *
 * add_new_key                                          *
 *                                                      *
 ********************************************************/

int
kadm_add_new_key(admin_name, target_name, target_password)
    char *admin_name, *target_name, *target_password;
{
    Kadm_vals new;
    int k_errno;

    do_kadmin_init();

    k_errno = setvals(&new, target_name);
    if (k_errno != KADM_SUCCESS)
        return(k_errno);

    SET_FIELD(KADM_DESKEY,new.fields);

    if (princ_exists(new.name, new.instance, current_realm) != PE_YES) {
        /* get the admin's password */
        if (get_admin_ticket(admin_name) !=
#ifdef DEBUG01
            GOOD_PW
#else
            GT_PW_OK
#endif
            ) return(-1);
        if (get_key(&new.key_low, &new.key_high, SWAP, target_password) == GOOD_PW) {
            k_errno = kadm_add(&new);
            bzero((char *)&new, sizeof(new));
#ifndef NO_MULTIPLE
            if (!multiple)
                dest_tkt();
#endif
            return(k_errno);
        }
    }
    else return(-1); /* Principal already exists ... need a com_err code for this */
}

#endif /* end of Windows-possibly-not-needed stuff */

static int inited = 0;

/************************************************************
 *                                                          *
 * utility routines:                                        *
 *         do_kadmin_init                                   *
 *                setup ticket cache, get local realm       *
 *         get_admin_ticket                                 *
 *                get admin ticket if needed                *
 *                                                          *
 *         int get_password(low, high, byteswap, password)  *
 *                                                          *
 *         int setvals(vals, string)                        *
 *                                                          *
 *         int princ_exists(name, instance, realm)          *
 *                                                          *
 ************************************************************/

void do_kadmin_init()
{
#ifdef SEPARATE_TICKET_FILE
    (void) sprintf(tktstring, "/tmp/tkt_adm_%d",getpid());
    krb_set_tkt_string(tktstring);
#endif

    if (!inited) {
        *default_realm = '\0';
        *current_realm = '\0';

        /*
         * This is only as a default/initial realm; we don't care
         * about failure.
         */
        if (krb_get_lrealm(default_realm, 1) != KSUCCESS)
            strcpy(default_realm, KRB_REALM);

        /*
         * If we can reach the local realm, initialize to it.  Otherwise,
         * don't initialize.
         */
        if (kadm_init_link(PWSERV_NAME, KADM_SINST,
                           default_realm) != KADM_SUCCESS)
            bzero(current_realm, sizeof(current_realm));
        else
            strcpy(current_realm, default_realm);

        inited = 1;
    }
}

long /* was an int -- jms 8/23/93 */
get_admin_ticket(admin_name, admin_password)
    char *admin_name, *admin_password;
{
    char name[ANAME_SZ+1];
    char instance[INST_SZ+1];
    char realm[REALM_SZ+1];
#ifndef NO_MULTIPLE
    CREDENTIALS c;
#endif
    char msg[256];

    int k_errno;
    int ticket_life = 1;  /* minimum ticket lifetime */

    *name = '\0';
    *instance = '\0';
    *realm = '\0';

    if (k_errno = kname_parse(name, instance, realm, admin_name)) {
#ifdef DEBUG01
        wsprintf(msg, "kerberos error: %s\n", (LPSTR) get_krb_err_txt_entry(k_errno));
        /*   do_error(msg); */
#endif
        return KRBERR(k_errno);
        /* return(k_errno); */
    }

    if (!realm[0]) strcpy(realm, current_realm);
    if (!realm[0]) strcpy(realm, default_realm);

#ifndef NO_MULTIPLE
    if (multiple) {
        /* If admin tickets exist and are valid, just exit. */
        bzero(&c, sizeof(c));
        if (krb_get_cred(PWSERV_NAME, KADM_SINST, current_realm, &c) == KSUCCESS)
            /*
             * If time is less than lifetime - FUDGE_VALUE after issue date,
             * tickets will probably last long enough for the next
             * transaction.
             */
            if (time(0) < (c.issue_date + (5 * 60 * c.lifetime) - FUDGE_VALUE))
                return(KADM_SUCCESS);
        ticket_life = DEFAULT_TKT_LIFE;
    }
#endif

    k_errno = krb_get_pw_in_tkt(name, instance, current_realm, PWSERV_NAME,
                                KADM_SINST, ticket_life, admin_password);

#ifdef DEBUG01
    switch(k_errno) {
    case GT_PW_OK:
        return(GOOD_PW);
    case KDC_PR_UNKNOWN:
        wsprintf(msg, "Principal %s does not exist.\n", admin_name);
        KRBERR(k_errno);
        // do_error(msg);
        return(BAD_PW);
    case GT_PW_BADPW:
        // do_error("Incorrect admin password.\n");
        KRBERR(k_errno);
        return(BAD_PW);
    default:
        com_err("kadmin", k_errno+krb_err_base, "while getting password tickets");
        return(BAD_PW);
    }
#else
    return KRBERR(k_errno);
#endif
}

int
princ_exists(name, instance, realm)
    char *name;
char *instance;
char *realm;
{
    int k_errno;

    k_errno = krb_get_pw_in_tkt(name, instance, realm, "krbtgt", realm, 1, "");

    if ((k_errno == KSUCCESS) || (k_errno == INTK_BADPW))
        return(PE_YES);
    else if (k_errno == KDC_PR_UNKNOWN)
        return(PE_NO);
    else
        return(PE_UNSURE);
}

int
get_key(low, high, byteswap, password)
    unsigned long *low, *high;
int byteswap;
char *password;
{
    des_cblock newkey;

#ifdef NOENCRYPTION
    bzero((char *) newkey, sizeof(newkey));
#else
    des_string_to_key(password, newkey);
#endif

    bcopy((char *) newkey,(char *)low,4);
    bcopy((char *)(((long *) newkey) + 1), (char *)high,4);

    bzero((char *) newkey, sizeof(newkey));

#ifdef NOENCRYPTION
    *low = 1;
#endif

    if (byteswap != DONTSWAP) {
        *low = htonl(*low);
        *high = htonl(*high);
    }
    return(GOOD_PW);
}

#if WINDOWS_NEEDS_THIS_STUFF
int
setvals(vals, string)
    Kadm_vals *vals;
char *string;
{
    char realm[REALM_SZ];
    int k_errno = KADM_SUCCESS;
    char msg[256];

    bzero(vals, sizeof(*vals));
    bzero(realm, sizeof(realm));

    SET_FIELD(KADM_NAME,vals->fields);
    SET_FIELD(KADM_INST,vals->fields);
    if (k_errno = kname_parse(vals->name, vals->instance, realm, string)) {
        wsprintf(msg, "kerberos error: %s\n", get_krb_err_txt_entry(k_errno));
        KRBERR(k_errno);
        // do_error(msg);
        return k_errno;
    }
    if (!realm[0])
        strcpy(realm, default_realm);
    if (strcmp(realm, current_realm)) {
        strcpy(current_realm, realm);
        if ((k_errno = kadm_init_link(PWSERV_NAME, KADM_SINST, current_realm))
            != KADM_SUCCESS) {
            wsprintf(msg, "kadm error for realm %s: %s\n",
                     current_realm, error_message(k_errno));
            KRBERR(k_errno);
            // do_error(msg);
        }
    }
    if (k_errno)
        return 1;
    else
        return KADM_SUCCESS;
}

#endif  /* WINDOWS_NEEDS_THIS_STUFF */
