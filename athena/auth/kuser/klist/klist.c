/*
 * clients/klist/klist.c
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
 * List out the contents of your credential cache or keytab.
 */

#include <krb5.h>
#include <krb.h>
#include <com_err.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <kuser.h>

#define com_err pcom_err
#define krb_get_err_text pget_krb_err_txt_entry

krb5_error_code decode_krb5_ticket
	KRB5_PROTOTYPE((const krb5_data *code, krb5_ticket **rep));

int use_k4_only = 0, use_k5_only = 0;
int show_flags = 0, show_time = 0, status_only = 0, show_keys = 0;
int show_etype = 0, show_addresses = 0, no_resolve = 0;
char *defname;
krb5_int32 now;
int timestamp_width;

krb5_context kcontext;

char * etype_string KRB5_PROTOTYPE((krb5_enctype ));
void show_credential KRB5_PROTOTYPE((char *,
				krb5_context,
				krb5_creds *));
	
void do_ccache KRB5_PROTOTYPE((char *));
void do_keytab KRB5_PROTOTYPE((char *));
void printtime KRB5_PROTOTYPE((time_t));
void one_addr KRB5_PROTOTYPE((krb5_address *));
void fillit KRB5_PROTOTYPE((FILE *, int, int));
void do_v4_ccache KRB5_PROTOTYPE((char *));
	
#define DEFAULT 0
#define CCACHE 1
#define KEYTAB 2

void usage()
{
     fprintf(stderr, "Usage: %s [[-4] [-5]] [[-c] [-f] [-e] [-s] [-a] [-n]] [-k [-t] [-K]] [name]\n",
	     progname); 
     fprintf(stderr, "\t-4 Kerberos 4 only, -5 Kerberos 5 only, default is both\n");
     fprintf(stderr, "\t-c specifies credentials cache, -k specifies keytab");
     fprintf(stderr, ", -c is default\n");
     fprintf(stderr, "\toptions for credential caches:\n");
     fprintf(stderr, "\t\t-f shows credentials flags\n");
     fprintf(stderr, "\t\t-e shows the encryption type\n");
     fprintf(stderr, "\t\t-s sets exit status based on valid tgt existence\n");
     fprintf(stderr, "\t\t-a displays the address list\n");
     fprintf(stderr, "\t\t\t-n do not reverse-resolve\n");
     fprintf(stderr, "\toptions for keytabs:\n");
     fprintf(stderr, "\t\t-t shows keytab entry timestamps\n");
     fprintf(stderr, "\t\t-K shows keytab entry DES keys\n");
     exit(1);
}

static int got_k4 = 0;
static int got_k5 = 0; 

void
main(argc, argv)
    int argc;
    char **argv;
{
    krb5_error_code retval;
    char *name;
    int mode;

    fix_progname(argv);
    now = time(0);

    if (!LoadFuncs(COMERR_DLL, ce_fi, 0, 0, 1, 0, 1))
        pcom_err = fake_com_err;
    got_k5 = LoadFuncs(KRB5_DLL, k5_fi, 0, 0, 1, 0, 1);
    got_k4 = LoadFuncs(KRB4_DLL, k4_fi, 0, 0, 1, 0, 1);

    if (got_k5)
    {
        retval = pkrb5_init_context(&kcontext);
        if (retval) {
	    com_err(progname, retval, "while initializing krb5");
	    exit(1);
        }
    }

    argv++;
    name = NULL;
    mode = DEFAULT;
    while (*argv) {
	if ((*argv)[0] != '-') {
	    if (name) usage();
	    name = *argv;
	} else switch ((*argv)[1]) {
	case 'f':
	    show_flags = 1;
	    break;
	case 'e':
	    show_etype = 1;
	    break;
	case 't':
	    show_time = 1;
	    break;
	case 'K':
	    show_keys = 1;
	    break;
	case 's':
	    status_only = 1;
	    break;
	case 'n':
	    no_resolve = 1;
	    break;
	case 'a':
	    show_addresses = 1;
	    break;
	case 'c':
	    if (mode != DEFAULT) usage();
	    mode = CCACHE;
	    break;
	case 'k':
	    if (mode != DEFAULT) usage();
	    mode = KEYTAB;
	    break;
        case '4':
            use_k4_only = 1;
            break;
        case '5':
            use_k5_only = 1;
            break;
	default:
	    usage();
	    break;
	}
	argv++;
    }

    if (mode == DEFAULT || mode == CCACHE) {
	 if (show_time || show_keys)
	      usage();
    } else {
	 if (show_flags || status_only)
	      usage();
    }

    if (use_k4_only && use_k5_only)
    {
        fprintf(stderr, "Only one of -4 and -5 allowed\n");
        usage();
    }
    if (use_k4_only)
        got_k5 = 0;
    if (use_k5_only)
        got_k4 = 0;

    {
	char tmp[BUFSIZ];

	if (!k_timestamp_to_sfstring(now, tmp, 20, (char *) NULL) ||
	    !k_timestamp_to_sfstring(now, tmp, sizeof(tmp), (char *) NULL))
	    timestamp_width = (int) strlen(tmp);
	else
	    timestamp_width = 15;
    }

    if (got_k5)
    {
        if (mode == DEFAULT || mode == CCACHE)
            do_ccache(name);
        else
            do_keytab(name);
    }
    else
    {
        if (mode == DEFAULT || mode == CCACHE)
            do_v4_ccache(name);
        else
        {
            // XXX - we may want to add v4 keytab/srvtab support
            fprintf(stderr, "%s: We do not currently support srvtabs with "
                    "Kerberos 4\n", progname);
            exit(1);
        }
    }

    exit(0);
}    

void do_keytab(name)
   char *name;
{
    krb5_keytab kt;
    krb5_keytab_entry entry;
    krb5_kt_cursor cursor;
    char buf[BUFSIZ]; /* hopefully large enough for any type */
    char *pname;
    int code;
     
    if (name == NULL) {
        if ((code = pkrb5_kt_default(kcontext, &kt))) {
            com_err(progname, code, "while getting default keytab");
            exit(1);
        }
    } else {
        if ((code = pkrb5_kt_resolve(kcontext, name, &kt))) {
            com_err(progname, code, "while resolving keytab %s",
                    name);
            exit(1);
        }
    }

    if ((code = krb5_kt_get_name(kcontext, kt, buf, BUFSIZ))) {
        com_err(progname, code, "while getting keytab name");
        exit(1);
    }

    printf("Keytab name: %s\n", buf);
     
    if ((code = krb5_kt_start_seq_get(kcontext, kt, &cursor))) {
        com_err(progname, code, "while starting keytab scan");
        exit(1);
    }

    if (show_time) {
        printf("KVNO Timestamp");
        fillit(stdout, timestamp_width - sizeof("Timestamp") + 2, (int) ' ');
        printf("Principal\n");
        printf("---- ");
        fillit(stdout, timestamp_width, (int) '-');
        printf(" ");
        fillit(stdout, 78 - timestamp_width - sizeof("KVNO"), (int) '-');
        printf("\n");
    } else {
        printf("KVNO Principal\n");
        printf("---- --------------------------------------------------------------------------\n");
    }
     
    while ((code = krb5_kt_next_entry(kcontext, kt, &entry, &cursor)) == 0) {
        if ((code = pkrb5_unparse_name(kcontext, entry.principal, &pname))) {
            com_err(progname, code, "while unparsing principal name");
            exit(1);
        }
        printf("%4d ", entry.vno);
        if (show_time) {
            printtime(entry.timestamp);
            printf(" ");
        }
        printf("%s", pname);
        if (show_etype)
            printf(" (%s) " , etype_string(entry.key.enctype));
        if (show_keys) {
            printf(" (0x");
            {
                int i;
                for (i = 0; i < entry.key.length; i++)
                    printf("%02x", entry.key.contents[i]);
            }
            printf(")");
        }
        printf("\n");
        pkrb5_free_unparsed_name(kcontext, pname);
    }
    if (code && code != KRB5_KT_END) {
        com_err(progname, code, "while scanning keytab");
        exit(1);
    }
    if ((code = krb5_kt_end_seq_get(kcontext, kt, &cursor))) {
        com_err(progname, code, "while ending keytab scan");
        exit(1);
    }
    exit(0);
}
void do_ccache(name)
   char *name;
{
    krb5_ccache cache = NULL;
    krb5_cc_cursor cur;
    krb5_creds creds;
    krb5_principal princ;
    krb5_flags flags;
    krb5_error_code code;
    int	exit_status = 0;

    if (status_only)
	/* exit_status is set back to 0 if a valid tgt is found */
	exit_status = 1;

    if (name == NULL) {
	if ((code = pkrb5_cc_default(kcontext, &cache))) {
	    if (!status_only)
		com_err(progname, code, "while getting default ccache");
	    exit(1);
	    }
    } else {
	if ((code = pkrb5_cc_resolve(kcontext, name, &cache))) {
	    if (!status_only)
		com_err(progname, code, "while resolving ccache %s",
			name);
	    exit(1);
	}
    }
 
    flags = 0;				/* turns off OPENCLOSE mode */
    if ((code = krb5_cc_set_flags(kcontext, cache, flags))) {
	if (code == KRB5_FCC_NOFILE) {
	    if (!status_only) {
		com_err(progname, code, "(ticket cache %s:%s)",
                        krb5_cc_get_type(kcontext, cache),
			krb5_cc_get_name(kcontext, cache));
                if (name == NULL)
                    do_v4_ccache(0);
            }
	} else {
	    if (!status_only)
		com_err(progname, code,
			"while setting cache flags (ticket cache %s:%s)",
                        krb5_cc_get_type(kcontext, cache),
			krb5_cc_get_name(kcontext, cache));
	}
	exit(1);
    }
    if ((code = krb5_cc_get_principal(kcontext, cache, &princ))) {
	if (!status_only)
	    com_err(progname, code, "while retrieving principal name");
	exit(1);
    }
    if ((code = pkrb5_unparse_name(kcontext, princ, &defname))) {
	if (!status_only)
	    com_err(progname, code, "while unparsing principal name");
	exit(1);
    }
    if (!status_only) {
	printf("Ticket cache: %s:%s\nDefault principal: %s\n\n",
               krb5_cc_get_type(kcontext, cache),
	       krb5_cc_get_name(kcontext, cache), defname);
	fputs("Valid starting", stdout);
	fillit(stdout, timestamp_width - sizeof("Valid starting") + 3,
	       (int) ' ');
	fputs("Expires", stdout);
	fillit(stdout, timestamp_width - sizeof("Expires") + 3,
	       (int) ' ');
	fputs("Service principal\n", stdout);
    }
    if ((code = krb5_cc_start_seq_get(kcontext, cache, &cur))) {
	if (!status_only)
	    com_err(progname, code, "while starting to retrieve tickets");
	exit(1);
    }
    while (!(code = krb5_cc_next_cred(kcontext, cache, &cur, &creds))) {
	if (status_only) {
	    if (exit_status && creds.server->length == 2 &&
		strcmp(creds.server->realm.data, princ->realm.data) == 0 &&
		strcmp((char *)creds.server->data[0].data, "krbtgt") == 0 &&
		strcmp((char *)creds.server->data[1].data,
		       princ->realm.data) == 0 && 
		creds.times.endtime > now)
		exit_status = 0;
	} else {
	    show_credential(progname, kcontext, &creds);
	}
	pkrb5_free_cred_contents(kcontext, &creds);
    }
    if (code == KRB5_CC_END) {
	if ((code = krb5_cc_end_seq_get(kcontext, cache, &cur))) {
	    if (!status_only)
		com_err(progname, code, "while finishing ticket retrieval");
	    exit(1);
	}
	flags = KRB5_TC_OPENCLOSE;	/* turns on OPENCLOSE mode */
	if ((code = krb5_cc_set_flags(kcontext, cache, flags))) {
	    if (!status_only)
		com_err(progname, code, "while closing ccache");
	    exit(1);
	}
        if (name == NULL && !status_only)
            do_v4_ccache(0);
	exit(exit_status);
    } else {
	if (!status_only)
	    com_err(progname, code, "while retrieving a ticket");
	exit(1);
    }	
}

char *
etype_string(enctype)
    krb5_enctype enctype;
{
    static char buf[100];
    krb5_error_code retval;
    
    if ((retval = pkrb5_enctype_to_string(enctype, buf, sizeof(buf)))) {
	/* XXX if there's an error != EINVAL, I should probably report it */
	sprintf(buf, "etype %d", enctype);
    }

    return buf;
}

char *
flags_string(cred)
    register krb5_creds *cred;
{
    static char buf[32];
    int i = 0;
	
    if (cred->ticket_flags & TKT_FLG_FORWARDABLE)
        buf[i++] = 'F';
    if (cred->ticket_flags & TKT_FLG_FORWARDED)
        buf[i++] = 'f';
    if (cred->ticket_flags & TKT_FLG_PROXIABLE)
        buf[i++] = 'P';
    if (cred->ticket_flags & TKT_FLG_PROXY)
        buf[i++] = 'p';
    if (cred->ticket_flags & TKT_FLG_MAY_POSTDATE)
        buf[i++] = 'D';
    if (cred->ticket_flags & TKT_FLG_POSTDATED)
        buf[i++] = 'd';
    if (cred->ticket_flags & TKT_FLG_INVALID)
        buf[i++] = 'i';
    if (cred->ticket_flags & TKT_FLG_RENEWABLE)
        buf[i++] = 'R';
    if (cred->ticket_flags & TKT_FLG_INITIAL)
        buf[i++] = 'I';
    if (cred->ticket_flags & TKT_FLG_HW_AUTH)
        buf[i++] = 'H';
    if (cred->ticket_flags & TKT_FLG_PRE_AUTH)
        buf[i++] = 'A';
    buf[i] = '\0';
    return(buf);
}

void 
printtime(tv)
    time_t tv;
{
    char timestring[BUFSIZ];
    char fill;

    fill = ' ';
    if (!k_timestamp_to_sfstring((krb5_timestamp) tv,
                                 timestring,
                                 timestamp_width+1,
                                 &fill)) {
	printf(timestring);
    }
}

void
show_credential(progname, kcontext, cred)
    char 		* progname;
    krb5_context  	  kcontext;
    register krb5_creds * cred;
{
    krb5_error_code retval;
    krb5_ticket *tkt;
    char *name, *sname, *flags;
    int	extra_field = 0;

    retval = pkrb5_unparse_name(kcontext, cred->client, &name);
    if (retval) {
	com_err(progname, retval, "while unparsing client name");
	return;
    }
    retval = pkrb5_unparse_name(kcontext, cred->server, &sname);
    if (retval) {
	com_err(progname, retval, "while unparsing server name");
	pkrb5_free_unparsed_name(kcontext, name);
	return;
    }
    if (!cred->times.starttime)
	cred->times.starttime = cred->times.authtime;

    printtime(cred->times.starttime);
    putchar(' '); putchar(' ');
    printtime(cred->times.endtime);
    putchar(' '); putchar(' ');

    printf("%s\n", sname);

    if (strcmp(name, defname)) {
	    printf("\tfor client %s", name);
	    extra_field++;
    }
    
    if (cred->times.renew_till) {
	if (!extra_field)
		fputs("\t",stdout);
	else
		fputs(", ",stdout);
	fputs("renew until ", stdout);
        printtime(cred->times.renew_till);
	extra_field += 2;
    }

    if (extra_field > 3) {
	fputs("\n", stdout);
	extra_field = 0;
    }

    if (show_flags) {
	flags = flags_string(cred);
	if (flags && *flags) {
	    if (!extra_field)
		fputs("\t",stdout);
	    else
		fputs(", ",stdout);
	    printf("Flags: %s", flags);
	    extra_field++;
        }
    }

    if (extra_field > 2) {
	fputs("\n", stdout);
	extra_field = 0;
    }

    if (show_etype) {
	retval = pdecode_krb5_ticket(&cred->ticket, &tkt);
	if (!extra_field)
	    fputs("\t",stdout);
	else
	    fputs(", ",stdout);
	printf("Etype (skey, tkt): %s, ",
	       etype_string(cred->keyblock.enctype));
	printf("%s ",
	       etype_string(tkt->enc_part.enctype));
	pkrb5_free_ticket(kcontext, tkt);
	extra_field++;
    }

    /* if any additional info was printed, extra_field is non-zero */
    if (extra_field)
	putchar('\n');


    if (show_addresses) {
	if (!cred->addresses || !cred->addresses[0]) {
	    printf("\tAddresses: (none)\n");
	} else {
	    int i;

	    printf("\tAddresses: ");
	    one_addr(cred->addresses[0]);

	    for (i=1; cred->addresses[i]; i++) {
		printf(", ");
		one_addr(cred->addresses[i]);
	    }

	    printf("\n");
	}
    }

    pkrb5_free_unparsed_name(kcontext, name);
    pkrb5_free_unparsed_name(kcontext, sname);
}

void one_addr(a)
    krb5_address *a;
{
    struct hostent *h;

    if ((a->addrtype == ADDRTYPE_INET) &&
	(a->length == 4)) {
	if (!no_resolve) {
	    h = gethostbyaddr(a->contents, 4, AF_INET);
	    if (h) {
		printf("%s", h->h_name);
	    }
	}
	if (no_resolve || !h) {
	    printf("%d.%d.%d.%d", a->contents[0], a->contents[1],
		   a->contents[2], a->contents[3]);
	}
    } else {
	printf("unknown addr type %d", a->addrtype);
    }
}

void
fillit(f, num, c)
    FILE	*f;
    int		num;
    int		c;
{
    int i;

    for (i=0; i<num; i++)
	fputc(c, f);
}

void
do_v4_ccache(
    char* name
    )
{
    char    pname[ANAME_SZ];
    char    pinst[INST_SZ];
    char    prealm[REALM_SZ];
    char    *file;
    int     k_errno;
    CREDENTIALS c;
    int     header = 1;

    if (!got_k4)
        return;

    file = name?name:ptkt_string();

    if (status_only)
        exit(pkrb_check_serv("krbtgt")?1:0);

    if (got_k5)
        printf("\n\n");

    printf("Kerberos 4 ticket cache: %s\n", file);

    /* 
     * Since krb_get_tf_realm will return a ticket_file error, 
     * we will call tf_init and tf_close first to filter out
     * things like no ticket file.  Otherwise, the error that 
     * the user would see would be 
     * klist: can't find realm of ticket file: No ticket file (tf_util)
     * instead of
     * klist: No ticket file (tf_util)
     */

    /* Open ticket file */
    if (k_errno = ptf_init(file, R_TKT_FIL)) {
	fprintf(stderr, "%s: %s\n", progname, krb_get_err_text (k_errno));
	exit(1);
    }
    /* Close ticket file */
    (void) ptf_close();

    /* 
     * We must find the realm of the ticket file here before calling
     * tf_init because since the realm of the ticket file is not
     * really stored in the principal section of the file, the
     * routine we use must itself call tf_init and tf_close.
     */
    if ((k_errno = pkrb_get_tf_realm(file, prealm)) != KSUCCESS) {
	fprintf(stderr, "%s: can't find realm of ticket file: %s\n",
		progname, krb_get_err_text (k_errno));
	exit(1);
    }

    /* Open ticket file */
    if (k_errno = ptf_init(file, R_TKT_FIL)) {
	fprintf(stderr, "%s: %s\n", progname, krb_get_err_text (k_errno));
	exit(1);
    }
    /* Get principal name and instance */
    if ((k_errno = ptf_get_pname(pname)) ||
	(k_errno = ptf_get_pinst(pinst))) {
	fprintf(stderr, "%s: %s\n", progname, krb_get_err_text (k_errno));
	exit(1);
    }

    /* 
     * You may think that this is the obvious place to get the
     * realm of the ticket file, but it can't be done here as the
     * routine to do this must open the ticket file.  This is why 
     * it was done before tf_init.
     */
       
    printf("Principal: %s%s%s%s%s\n\n", pname,
	   (pinst[0] ? "." : ""), pinst,
	   (prealm[0] ? "@" : ""), prealm);
    while ((k_errno = ptf_get_cred(&c)) == KSUCCESS) {
	if (header) {
	    printf("%-18s  %-18s  %s\n",
		   "  Issued", "  Expires", "  Principal");
	    header = 0;
	}
	printtime(c.issue_date);
	fputs("  ", stdout);
	printtime(c.issue_date + ((unsigned char) c.lifetime) * 5 * 60);
	printf("  %s%s%s%s%s\n",
	       c.service, (c.instance[0] ? "." : ""), c.instance,
	       (c.realm[0] ? "@" : ""), c.realm);
    }
    if (header && k_errno == EOF) {
	printf("No tickets in file.\n");
    }
}
