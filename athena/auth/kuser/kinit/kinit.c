/*
 * clients/kinit/kinit.c
 *
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 *
 * Initialize a credentials cache.
 */

#include <krb5.h>
#include <krb.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <com_err.h>
#include <getopt.h>

#include <kuser.h>
#include <loadfuncs-leash.h>

static char* progname_v5 = 0;
static char* progname_v4 = 0;

static int got_k4 = 0;
static int got_k5 = 0;

static int authed_k5 = 0;
static int authed_k4 = 0;

#define KRB5_DEFAULT_OPTIONS (KDC_OPT_PROXIABLE|KDC_OPT_FORWARDABLE)
#define KRB5_DEFAULT_LIFE 60*60*10 /* 10 hours */

static krb5_timestamp now;

static krb5_error_code
read_console_password(
    const char		* prompt,
    const char		* prompt2,
    char		* password,
    int			* pwsize
    )
{
    HANDLE		handle;
    DWORD		old_mode, new_mode;
    char		*tmpstr = 0;
    char		*ptr;
    int			scratchchar;
    krb5_error_code	errcode = 0;

    handle = GetStdHandle(STD_INPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE)
	return ENOTTY;
    if (!GetConsoleMode(handle, &old_mode))
	return ENOTTY;

    new_mode = old_mode;
    new_mode |=  ( ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT );
    new_mode &= ~( ENABLE_ECHO_INPUT );

    if (!SetConsoleMode(handle, new_mode))
	return ENOTTY;

    (void) fputs(prompt, stdout);
    (void) fflush(stdout);
    (void) memset(password, 0, *pwsize);

    if (fgets(password, *pwsize, stdin) == NULL) {
	(void) putchar('\n');
	errcode = KRB5_LIBOS_CANTREADPWD;
	goto cleanup;
    }
    (void) putchar('\n');

    if ((ptr = strchr(password, '\n')))
	*ptr = '\0';
    else /* need to flush */
	do {
	    scratchchar = getchar();
	} while (scratchchar != EOF && scratchchar != '\n');

    if (prompt2) {
	if (! (tmpstr = (char *)malloc(*pwsize))) {
	    errcode = ENOMEM;
	    goto cleanup;
	}
	(void) fputs(prompt2, stdout);
	(void) fflush(stdout);
	if (fgets(tmpstr, *pwsize, stdin) == NULL) {
	    (void) putchar('\n');
	    errcode = KRB5_LIBOS_CANTREADPWD;
	    goto cleanup;
	}
	(void) putchar('\n');

	if ((ptr = strchr(tmpstr, '\n')))
	    *ptr = '\0';
	else /* need to flush */
	    do {
		scratchchar = getchar();
	    } while (scratchchar != EOF && scratchchar != '\n');

	if (strncmp(password, tmpstr, *pwsize)) {
	    errcode = KRB5_LIBOS_BADPWDMATCH;
	    goto cleanup;
	}
    }

cleanup:
    (void) SetConsoleMode(handle, old_mode);
    if (tmpstr) {
	(void) memset(tmpstr, 0, *pwsize);
	(void) free(tmpstr);
    }
    if (errcode)
	(void) memset(password, 0, *pwsize);
    else
	*pwsize = strlen(password);
    return errcode;
}

typedef enum { INIT_PW, INIT_KT, RENEW, VALIDATE } action_type;

struct k_opts
{
    /* in seconds */
    krb5_deltat starttime;
    krb5_deltat lifetime;
    krb5_deltat rlife;

    int forwardable;
    int proxiable;

    int not_forwardable;
    int not_proxiable;

    int verbose;

    char* principal_name;
    char* service_name;
    char* keytab_name;
    char* cache_name;

    action_type action;
};

struct k5_data
{
    krb5_context ctx;
    krb5_ccache cc;
    krb5_principal me;
    char* name;
};

struct k4_data
{
    char aname[ANAME_SZ + 1];
    char inst[INST_SZ + 1];
    char realm[REALM_SZ + 1];
    char* name;
};

void
usage()
{
    fprintf(stderr, "Usage: %s [-l lifetime] [-r renewable_life] [-f] [-F] [-p] [-P] [-4] [-5] [-V] [-s start_time] [-S target_service] [-k [-t keytab_file]] [-R] [-v] [-c cachename] [principal]\n", progname);
    fprintf(stderr,
            "\t-4 Kerberos 4 only, -5 Kerberos 5 only, default is both\n"
            "\toptions applicable to Kerberos 5 only:\n"
            "\t\t-v validate\n"
            "\t\t-c cache name\n"
            "\t\t-f forwardable\n"
            "\t\t-F not forwardable\n"
            "\t\t-p proxiable\n"
            "\t\t-P not proxiable\n"
            "\t\t-r renewable lifetime\n"
            "\t\t-s start time\n"
            "\t\tdefault is %sforwardable and %sproxiable\n"
            "\toptions applicable to both:\n"
            "\t\t-R renew (will only work if also using Kerberos 5)\n"
            "\t\t-l lifetime\n"
            "\t\t-S service\n"
            "\t\t-k use keytab/srvtab\n"
            "\t\t-t filename of keytab/srvtab to use\n"
            "\t\t-V verbose\n",
            (KRB5_DEFAULT_OPTIONS & KDC_OPT_FORWARDABLE)?"":"not",
            (KRB5_DEFAULT_OPTIONS & KDC_OPT_PROXIABLE)?"":"not"
        );
    exit(2);
}

char *
parse_options(
    int argc,
    char **argv,
    struct k_opts* opts
    )
{
    krb5_error_code code;
    int errflg = 0;
    int use_k4_only = 0;
    int use_k5_only = 0;
    int i;

    while ((i = getopt(argc, argv, "r:fpFP45Vl:s:c:kt:RS:v")) != -1)
    {
	switch (i) {
        case 'V':
            opts->verbose = 1;
            break;
        case 'l':
            /* Lifetime */
            code = k_string_to_deltat(optarg, &opts->lifetime);
            if (code != 0 || opts->lifetime == 0) {
                fprintf(stderr, "Bad lifetime value %s\n", optarg);
                errflg++;
            }
            break;
	case 'r':
            /* Renewable Time */
            code = k_string_to_deltat(optarg, &opts->rlife);
            if (code != 0 || opts->rlife == 0) {
                fprintf(stderr, "Bad lifetime value %s\n", optarg);
                errflg++;
            }
            break;
	case 'f':
            opts->forwardable = 1;
            break;
        case 'F':
            opts->not_forwardable = 1;
            break;
	case 'p':
            opts->proxiable = 1;
            break;
	case 'P':
            opts->not_proxiable = 1;
            break;
       	case 's':
	    code = k_string_to_timestamp(optarg, &opts->starttime);
	    if (code != 0 || opts->starttime == 0) {
                krb5_deltat dt;
                code = k_string_to_deltat(optarg, &dt);
                if (code != 0 || dt == 0) {
                    fprintf(stderr, "Bad postdate start time value %s\n", 
                            optarg);
                    errflg++;
                } else {
                    opts->starttime = now + dt;
                }
            }
	    break;
        case 'S':
            opts->service_name = optarg;
	    break;
        case 'k':
            opts->action = INIT_KT;
	    break;
        case 't':
            if (opts->keytab_name)
            {
                fprintf(stderr, "Only one -t option allowed.\n");
                errflg++;
            } else {
                opts->keytab_name = optarg;
            }
            break;
	case 'R':
	    opts->action = RENEW;
	    break;
	case 'v':
	    opts->action = VALIDATE;
	    break;
       	case 'c':
            if (opts->cache_name)
            {
                fprintf(stderr, "Only one -c option allowed\n");
                errflg++;
            } else {
                opts->cache_name = optarg;
            }
	    break;
        case '4':
            if (!got_k4)
            {
                fprintf(stderr, "Kerberos 4 support could not be loaded\n");
                exit(3);
            }
            use_k4_only = 1;
            break;
        case '5':
            if (!got_k5)
            {
                fprintf(stderr, "Kerberos 5 support could not be loaded\n");
                exit(3);
            }
            use_k5_only = 1;
            break;
	default:
	    errflg++;
	    break;
	}
    }

    if (use_k5_only && use_k4_only)
    {
        fprintf(stderr, "Only one of -4 and -5 allowed\n");
        errflg++;
    }
    if (opts->forwardable && opts->not_forwardable)
    {
        fprintf(stderr, "Only one of -f and -F allowed\n");
        errflg++;
    }
    if (opts->proxiable && opts->not_proxiable)
    {
        fprintf(stderr, "Only one of -p and -P allowed\n");
        errflg++;
    }

    if (argc - optind > 1) {
	fprintf(stderr, "Extra arguments (starting with \"%s\").\n",
		argv[optind+1]);
	errflg++;
    }

    if (errflg) {
        usage();
    }

    /* At this point, we know we only have one option selection */
    if (use_k4_only)
        got_k5 = 0;
    if (use_k5_only)
        got_k4 = 0;

    opts->principal_name = (optind == argc-1) ? argv[optind] : 0;
    return opts->principal_name;
}

int
k5_begin(
    struct k_opts* opts,
    struct k5_data* k5,
    struct k4_data* k4
    )
{
    char* progname = progname_v5;
    krb5_error_code code = 0;

    if (!got_k5)
        return 0;

    if (code = pkrb5_init_context(&k5->ctx)) {
        pcom_err(progname, code, "while initializing Kerberos 5 library");
        return 0;
    }
    if (opts->cache_name)
    {
        code = pkrb5_cc_resolve(k5->ctx, opts->cache_name, &k5->cc);
        if (code != 0) {
            pcom_err(progname, code, "resolving ccache %s",
                     opts->cache_name);
            return 0;
        }
    } 
    else
    {
        if ((code = pkrb5_cc_default(k5->ctx, &k5->cc))) {
            pcom_err(progname, code, "while getting default ccache");
            return 0;
        }
    }

    if (opts->principal_name)
    {
        /* Use specified name */
        if ((code = pkrb5_parse_name(k5->ctx, opts->principal_name, 
                                     &k5->me))) {
            pcom_err(progname, code, "when parsing name %s", 
                     opts->principal_name);
            return 0;
        }
    }
    else
    {
        /* No principal name specified */
        if (opts->action == INIT_KT) {
            /* Use the default host/service name */
            if (code = pkrb5_sname_to_principal(k5->ctx, NULL, NULL,
                                                KRB5_NT_SRV_HST, 
                                                &k5->me)) {
                pcom_err(progname, code,
                         "when creating default server principal name");
                return 0;
            }
        } else {
            /* Get default principal from cache if one exists */
            if (code = krb5_cc_get_principal(k5->ctx, k5->cc, 
                                             &k5->me))
            {
                char name[1024];
                DWORD name_size = sizeof(name);
                if (!GetUserName(name, &name_size))
                {
                    fprintf(stderr, "Unable to identify user\n");
                    return 0;
                }
                if ((code = pkrb5_parse_name(k5->ctx, name, 
                                             &k5->me)))
                {
                    pcom_err(progname, code, "when parsing name %s", 
                             name);
                    return 0;
                }
            }
        }
    }
    if (code = pkrb5_unparse_name(k5->ctx, k5->me, 
                                  &k5->name)) {
        pcom_err(progname, code, "when unparsing name");
        return 0;
    }
    opts->principal_name = k5->name;

    if (got_k4)
    {
        krb5_data* c;
        c = krb5_princ_component(k5->ctx, k5->me, 0);
        memcpy(k4->aname, c->data, min(c->length, sizeof(k4->aname)));
        k4->aname[sizeof(k4->aname)] = 0;
        if (krb5_princ_size(k5->ctx, k5->me) > 1) {
            c = krb5_princ_component(k5->ctx, k5->me, 1);
            memcpy(k4->inst, c->data, min(c->length, sizeof(k4->inst)));
            k4->inst[sizeof(k4->inst)] = 0;
        } else {
            k4->inst[0] = 0;
        }
        c = krb5_princ_realm(k5->ctx, k5->me);
        memcpy(k4->realm, c->data, min(c->length, sizeof(k4->realm)));
        k4->realm[sizeof(k4->realm)] = 0;
    }
    return 1;
}

void
k5_end(
    struct k5_data* k5
    )
{
    if (k5->name)
        pkrb5_free_unparsed_name(k5->ctx, k5->name);
    if (k5->me)
        pkrb5_free_principal(k5->ctx, k5->me);
    if (k5->cc)
        krb5_cc_close(k5->ctx, k5->cc);
    if (k5->ctx)
        pkrb5_free_context(k5->ctx);
    memset(k5, 0, sizeof(*k5));
}

int
k4_begin(
    struct k_opts* opts,
    struct k4_data* k4
    )
{
    char* progname = progname_v4;
    int k_errno = 0;

    if (!got_k4)
        return 0;

    if (k4->aname[0])
        goto skip;

    if (opts->principal_name)
    {
        /* Use specified name */
        if (k_errno = pkname_parse(k4->aname, k4->inst, k4->realm, 
                                   opts->principal_name))
        {
            fprintf(stderr, "%s: %s\n", progname, 
                    pget_krb_err_txt_entry(k_errno));
            return 0;
        }
    } else {
        /* No principal name specified */
        if (opts->action == INIT_KT) {
            /* Use the default host/service name */
            /* XXX - need to add this functionality */
            fprintf(stderr, "%s: Kerberos 4 srvtab support is not "
                    "implemented\n", progname);
            return 0;
        } else {
            /* Get default principal from cache if one exists */
            if (k_errno = pkrb_get_tf_fullname(ptkt_string(), k4->aname, 
                                               k4->inst, k4->realm))
            {
                char name[1024];
                DWORD name_size = sizeof(name);
                if (!GetUserName(name, &name_size))
                {
                    fprintf(stderr, "Unable to identify user\n");
                    return 0;
                }
                if (k_errno = pkname_parse(k4->aname, k4->inst, k4->realm,
                                           name))
                {
                    fprintf(stderr, "%s: %s\n", progname, 
                            pget_krb_err_txt_entry(k_errno));
                    return 0;
                }
            }
        }
    }

    if (!k4->realm[0])
        pkrb_get_lrealm(k4->realm, 1);

    k4->name = malloc(strlen(k4->aname) + 1 + strlen(k4->inst) + 1 +
                      strlen(k4->realm) + 1);
    if (!k4->name)
        return 1; // XXX - pretend we're ok.
    if (k4->inst[0])
        sprintf(k4->name, "%s.%s@%s", k4->aname, k4->inst, k4->realm);
    else
        sprintf(k4->name, "%s@%s", k4->aname, k4->realm);
    opts->principal_name = k4->name;

 skip:
    if (k4->aname[0] && !pk_isname(k4->aname))
    {
	fprintf(stderr, "%s: bad Kerberos 4 name format\n", progname);
        return 0;
    }

    if (k4->inst[0] && !pk_isinst(k4->inst))
    {
        fprintf(stderr, "%s: bad Kerberos 4 instance format\n", progname);
        return 0;
    }

    if (k4->realm[0] && !pk_isrealm(k4->realm))
    {
        fprintf(stderr, "%s: bad Kerberos 4 realm format\n", progname);
        return 0;
    }
    return 1;
}

void
k4_end(
    struct k4_data* k4
    )
{
    if (k4->name)
        free(k4->name);
    memset(k4, 0, sizeof(*k4));
}

int
k5_kinit(
    struct k_opts* opts,
    struct k5_data* k5,
    char* password
    )
{
    char* progname = progname_v5;
    int notix = 1;
    krb5_keytab keytab = 0;
    krb5_creds my_creds;
    krb5_error_code code = 0;
    krb5_flags options = KRB5_DEFAULT_OPTIONS;

    if (!got_k5)
        goto cleanup;

    memset(&my_creds, 0, sizeof(my_creds));

    if (opts->rlife)
    {
        options |= KDC_OPT_RENEWABLE;
        my_creds.times.renew_till = now + opts->rlife;
    }

    if (opts->forwardable)
    {
        options |= KDC_OPT_FORWARDABLE;
    }
    if (opts->not_forwardable)
    {
        options &= ~KDC_OPT_FORWARDABLE;
    }
    if (opts->proxiable)
    {
        options |= KDC_OPT_PROXIABLE;
    }
    if (opts->not_proxiable)
    {
        options &= ~KDC_OPT_PROXIABLE;
    }
    if (opts->starttime)
    {
        options |= KDC_OPT_POSTDATED;
        my_creds.times.starttime = opts->starttime;
        my_creds.times.endtime = opts->starttime + opts->lifetime;
    } else {
        my_creds.times.endtime = now + opts->lifetime;
    }

    if ((opts->action == INIT_KT) && opts->keytab_name)
    {
        code = pkrb5_kt_resolve(k5->ctx, opts->keytab_name, &keytab);
        if (code != 0) {
            pcom_err(progname, code, "resolving keytab %s", 
                     opts->keytab_name);
            goto cleanup;
        }
    }

    switch (opts->action)
    {
    case INIT_PW:
    case INIT_KT:
        my_creds.client = k5->me;
        if (!opts->service_name) {
            if (code = pkrb5_build_principal_ext(k5->ctx, &my_creds.server,
                                                 krb5_princ_realm(k5->ctx, k5->me)->length,
                                                 krb5_princ_realm(k5->ctx, k5->me)->data,
                                                 KRB5_TGS_NAME_SIZE, 
                                                 KRB5_TGS_NAME,
                                                 krb5_princ_realm(k5->ctx, k5->me)->length,
                                                 krb5_princ_realm(k5->ctx, k5->me)->data,
                                                 0))
            {
                pcom_err(progname, code, "while building server name");
                goto cleanup;
            }
        } else {
            if (code = pkrb5_parse_name(k5->ctx, opts->service_name, 
                                        &my_creds.server))
            {
                pcom_err(progname, code, "while parsing service name %s",
                         opts->service_name);
                goto cleanup;
            }
        }
    }

    switch (opts->action) {
    case INIT_PW:
	code = pkrb5_get_in_tkt_with_password(k5->ctx, options, 0, 0, 0,
                                              password, 0, &my_creds, 0);
	break;
    case INIT_KT:
	code = pkrb5_get_in_tkt_with_keytab(k5->ctx, options, 0, 0, 0,
                                            keytab, 0, &my_creds, 0);
	break;
    case VALIDATE:
	code = pkrb5_get_validated_creds(k5->ctx, &my_creds, k5->me, k5->cc,
                                         opts->service_name);
	break;
    case RENEW:
	code = pkrb5_get_renewed_creds(k5->ctx, &my_creds, k5->me, k5->cc,
                                       opts->service_name);
	break;
    }

    if (code) {
        char *doing = 0;
        switch (opts->action) {
        case INIT_PW:
        case INIT_KT:
            doing = "getting initial credentials";
            break;
        case VALIDATE:
            doing = "validating credentials";
            break;
        case RENEW:
            doing = "renewing credentials";
            break;
        }

	// If got code == KRB5_AP_ERR_V4_REPLY && got_k4, we should
        // let the user know that maybe he/she wants -4.
        if (code == KRB5KRB_AP_ERR_V4_REPLY && got_k4)
            pcom_err(progname, code, "while %s\n"
                     "The KDC doesn't support v5.  "
                     "You may want the -4 option in the future",
                     doing);
        else if (code == KRB5KRB_AP_ERR_BAD_INTEGRITY)
            fprintf(stderr, "%s: Password incorrect while %s\n", progname,
                    doing);
	else
            pcom_err(progname, code, "while %s", doing);
        goto cleanup;
    }

    if (code = krb5_cc_initialize(k5->ctx, k5->cc, k5->me)) {
        pcom_err(progname, code, "when initializing cache %s",
                 opts->cache_name?opts->cache_name:"");
        goto cleanup;
    }

    if (code = krb5_cc_store_cred(k5->ctx, k5->cc, &my_creds)) {
        pcom_err(progname, code, "while storing credentials");
        goto cleanup;
    }

    notix = 0;

 cleanup:
    if (keytab)
	krb5_kt_close(k5->ctx, keytab);

    if (my_creds.client == k5->me) {
        my_creds.client = 0;
    }
    // XXX - free my_creds...
    return notix?0:1;
}

int
k4_kinit(
    struct k_opts* opts,
    struct k4_data* k4,
    char* password
    )
{
    char* progname = progname_v4;
    int k_errno = 0;

    if (!got_k4)
        return 0;

    switch (opts->action)
    {
    case INIT_PW:
        k_errno = pkrb_get_pw_in_tkt(k4->aname, k4->inst, k4->realm, "krbtgt", 
                                     k4->realm, opts->lifetime / (5 * 60), 
                                     password);

        if (k_errno) {
            fprintf(stderr, "%s: %s\n", progname, 
                    pget_krb_err_txt_entry(k_errno));
            if (authed_k5)
                fprintf(stderr, "Maybe your KDC does not support v4.  "
                        "Try the -5 option next time.\n");
            return 0;
        }
        return 1;
    case INIT_KT:
        fprintf(stderr, "%s: srvtabs are not yet supported\n", progname);
        return 0;
    case  RENEW:
        fprintf(stderr, "%s: renewal of krb4 tickets are not yet supported\n", progname);
        return 0;
    default:
        return 0;
    }
}

char*
getvprogname(
    char v
    )
{
    int len = strlen(progname) + 5;
    char *ret = malloc(len);
    if (ret)
        sprintf(ret, "%s(v%c)", progname, v);
    else
        ret = progname;
    return ret;
}

void
main(argc, argv)
    int argc;
    char **argv;
{
    struct k_opts opts;
    struct k5_data k5;
    struct k4_data k4;
    char password[255];

    fix_progname(argv);
    progname_v5 = getvprogname('5');
    progname_v4 = getvprogname('4');
    now = time(0);

    /* Ensure we can be driven from a pipe */
    if(!isatty(fileno(stdin)))
        setvbuf(stdin, 0, _IONBF, 0);
    if(!isatty(fileno(stdout)))
        setvbuf(stdout, 0, _IONBF, 0);
    if(!isatty(fileno(stderr)))
        setvbuf(stderr, 0, _IONBF, 0);

    if (!LoadFuncs(COMERR_DLL, ce_fi, 0, 0, 1, 0, 1))
        pcom_err = fake_com_err;
    got_k5 = LoadFuncs(KRB5_DLL, k5_fi, 0, 0, 1, 0, 1);
    got_k4 = LoadFuncs(KRB4_DLL, k4_fi, 0, 0, 1, 0, 1);

    memset(&opts, 0, sizeof(opts));
    opts.lifetime = KRB5_DEFAULT_LIFE;
    opts.action = INIT_PW;

    {
        DECL_FUNC_PTR(Leash_get_default_lifetime);
        FUNC_INFO leash_fi[] = {
            MAKE_FUNC_INFO(Leash_get_default_lifetime),
            END_FUNC_INFO
        };
        DWORD lifetime;
        if (LoadFuncs(LEASH_DLL, leash_fi, 0, 0, 1, 0, 1) &&
            (lifetime = pLeash_get_default_lifetime()))
            opts.lifetime = lifetime * 60;
    }

    memset(&k5, 0, sizeof(k5));
    memset(&k4, 0, sizeof(k4));

    parse_options(argc, argv, &opts);

    got_k5 = k5_begin(&opts, &k5, &k4);
    got_k4 = k4_begin(&opts, &k4);

    if (opts.action == INIT_PW)
    {
        char prompt[255];
        int pwsize = sizeof(password);
        krb5_error_code code;

        sprintf(prompt, "Password for %s: ", opts.principal_name);
        password[0] = 0;
        code = read_console_password(prompt, 0, password, &pwsize);
        if (code || pwsize == 0)
        {
            fprintf(stderr, "Error while reading password for '%s'\n",
                    opts.principal_name);
            memset(password, 0, sizeof(password));
            exit(1);
        }
    }

    authed_k5 = k5_kinit(&opts, &k5, password);
    authed_k4 = k4_kinit(&opts, &k4, password);

    memset(password, 0, sizeof(password));

    if (authed_k5 && opts.verbose)
        fprintf(stderr, "Authenticated to Kerberos v5\n");
    if (authed_k4 && opts.verbose)
        fprintf(stderr, "Authenticated to Kerberos v4\n");

    k5_end(&k5);
    k4_end(&k4);

    if ((got_k5 && !authed_k5) || (got_k4 && !authed_k4))
        exit(1);
    exit(0);
}
