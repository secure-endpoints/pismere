#include "kuser.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

char *progname;

void
fix_progname(char **argv)
{
    char *p = argv[0] + strlen(argv[0]);
    while ((p > argv[0]) && (*p != '\\') && (*p != '/'))
        p--;
    if ((*p == '\\') || (*p == '/'))
        progname = argv[0] = p + 1;
    else
        progname = argv[0];
}

struct deltat_match_entry {
    const char *	dt_scan_format;		/* sscanf format	*/
    int			dt_nmatch;		/* Number to match	*/
    int			dt_dindex;		/* Day index		*/
    int			dt_hindex;		/* Hour index		*/
    int			dt_mindex;		/* Minute index		*/
    int			dt_sindex;		/* Second index		*/
};

/* Absolute time strings */
static const char atime_full_digits[]	= "%y%m%d%H%M%S";
static const char atime_full_digits_d[]	= "%y.%m.%d.%H.%M.%S";
static const char atime_full_digits_Y[]	= "%Y%m%d%H%M%S";
static const char atime_full_digits_Yd[]= "%Y.%m.%d.%H.%M.%S";
static const char atime_nsec_digits[]	= "%y%m%d%H%M";
static const char atime_rel_hms[]	= "%H%M%S";
static const char atime_rel_hm[]	= "%H%M";
static const char atime_rel_col_hms[]	= "%T";
static const char atime_rel_col_hm[]	= "%R";
static const char atime_ldep_sfmt[]	= "%x:%X";
static const char atime_full_text[]	= "%d-%b-%Y:%T";
static const char atime_full_text_nos[]	= "%d-%b-%Y:%R";

static const char ascan_full_digits[]	= "%02d%02d%02d%02d%02d%02d";
static const char ascan_full_digits_d[]	= "%02d.%02d.%02d.%02d.%02d.%02d";
static const char ascan_full_digits_Y[]	= "%4d%02d%02d%02d%02d%02d";
static const char ascan_full_digits_Yd[]= "%4d.%02d.%02d.%02d.%02d.%02d";
static const char ascan_nsec_digits[]	= "%02d%02d%02d%02d%02d";
static const char ascan_rel_hms[]	= "%02d%02d%02d";
static const char ascan_rel_hm[]	= "%02d%02d";
static const char ascan_rel_col_hms[]	= "%02d:%02d:%02d";
static const char ascan_rel_col_hm[]	= "%02d:%02d";

static const char sftime_default_fmt[]	= "%02d/%02d/%4d %02d:%02d";
static const size_t sftime_default_len	= 2+1+2+1+4+1+2+1+2+1;

/* Delta time strings */
static const char dtscan_dhms_notext[]	= "%d-%02d:%02d:%02d";
static const char dtscan_dhms_stext[]	= "%dd%dh%dm%ds";
static const char dtscan_hms_notext[]	= "%d:%02d:%02d";
static const char dtscan_hms_stext[]	= "%dh%dm%ds";
static const char dtscan_hm_notext[] 	= "%d:%02d";
static const char dtscan_hm_stext[]	= "%dh%dm";
static const char dtscan_days[]		= "%d%[d]";
static const char dtscan_hours[]	= "%d%[h]";
static const char dtscan_minutes[]	= "%d%[m]";
static const char dtscan_seconds[]	= "%d%[s]";
static const char dt_day_singular[]	= "day";
static const char dt_day_plural[]	= "days";
static const char dt_output_donly[]	= "%d %s";
static const char dt_output_dhms[]	= "%d %s %02d:%02d:%02d";
static const char dt_output_hms[]	= "%d:%02d:%02d";


static const char * const atime_format_table[] = {
atime_full_digits_Y,	/* yyyymmddhhmmss		*/
atime_full_digits_Yd,	/* yyyy.mm.dd.hh.mm.ss		*/
atime_full_digits,	/* yymmddhhmmss			*/
atime_full_digits_d,	/* yy.mm.dd.hh.mm.ss		*/
atime_nsec_digits,	/* yymmddhhmm			*/
atime_rel_hms,		/* hhmmss			*/
atime_rel_hm,		/* hhmm				*/
atime_rel_col_hms,	/* hh:mm:ss			*/
atime_rel_col_hm,	/* hh:mm			*/
/* The following not really supported unless native strptime present */
atime_ldep_sfmt,	/*locale-dependent short format	*/
atime_full_text,	/* dd-month-yyyy:hh:mm:ss	*/
atime_full_text_nos	/* dd-month-yyyy:hh:mm		*/
};
static const int atime_format_table_nents = sizeof(atime_format_table)/
					    sizeof(atime_format_table[0]);

static const struct deltat_match_entry deltat_table[] = {
/* scan format		nmatch	daypos	hourpos	minpos	secpos	*/
/*---------------------	-------	-------	-------	-------	--------*/
{ dtscan_dhms_notext,	4,	0,	1,	2,	3	},
{ dtscan_dhms_stext,	4,	0,	1,	2,	3	},
{ dtscan_hms_notext,	3,	-1,	0,	1,	2	},
{ dtscan_hms_stext,	3,	-1,	0,	1,	2	},
{ dtscan_hm_notext,	2,	-1,	-1,	0,	1	},
{ dtscan_hm_stext,	2,	-1,	-1,	0,	1	},
{ dtscan_days,		2,	0,	-1,	-1,	-1	},
{ dtscan_hours,		2,	-1,	0,	-1,	-1	},
{ dtscan_minutes,	2,	-1,	-1,	0,	-1	},
{ dtscan_seconds,	2,	-1,	-1,	-1,	0	}
};
static const int deltat_table_nents = sizeof(deltat_table)/
				      sizeof(deltat_table[0]);

static char *
strptime(buf, format, tm)
    char *buf;
    const char *format;
    struct tm *tm;
{
    int year, month, day, hour, minute, second;
    char *bp;
    time_t now;
    
    /*
     * We only understand the following fixed formats:
     *    %Y%m%d%H%M%S
     *    %Y.%m.%d.%H.%M.%S
     *    %y%m%d%H%M%S
     *    %y.%m.%d.%H.%M.%S
     *    %y%m%d%H%M
     *    %H%M%S
     *    %H%M
     *    %T
     *    %R
     */
    bp = (char *) NULL;
    if (!strcmp(format, atime_full_digits_Y) &&
	(sscanf(buf, ascan_full_digits_Y,
		&year, &month, &day, &hour, &minute, &second) == 6)) {
	if (year < 1900)
		return NULL;
	tm->tm_year = year-1900;
	tm->tm_mon = month - 1;
	tm->tm_mday = day;
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_full_digits)];
    }
    else if (!strcmp(format,atime_full_digits_Yd) &&
	     (sscanf(buf, ascan_full_digits_Yd,
		     &year, &month, &day, &hour, &minute, &second) == 6)) {
	if (year < 1900)
		return NULL;
	tm->tm_year = year-1900;
	tm->tm_mon = month - 1;
	tm->tm_mday = day;
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_full_digits_d)];
    }
    else if (!strcmp(format, atime_full_digits) &&
	(sscanf(buf, ascan_full_digits,
		&year, &month, &day, &hour, &minute, &second) == 6)) {
	if (year <= 68)
		year += 100;
	tm->tm_year = year;
	tm->tm_mon = month - 1;
	tm->tm_mday = day;
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_full_digits)];
    }
    else if (!strcmp(format,atime_full_digits_d) &&
	     (sscanf(buf, ascan_full_digits_d,
		     &year, &month, &day, &hour, &minute, &second) == 6)) {
	if (year <= 68)
		year += 100;
	tm->tm_year = year;
	tm->tm_mon = month - 1;
	tm->tm_mday = day;
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_full_digits_d)];
    }
    else if (!strcmp(format, atime_nsec_digits) &&
	     (sscanf(buf, ascan_nsec_digits,
		&year, &month, &day, &hour, &minute) == 5)) {
	if (year <= 68)
		year += 100;
	tm->tm_year = year;
	tm->tm_mon = month - 1;
	tm->tm_mday = day;
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = 0;
	bp = &buf[strlen(atime_nsec_digits)];
    }
    else if (!strcmp(format, atime_rel_hms) &&
	     (sscanf(buf, ascan_rel_hms, &hour, &minute, &second) == 3)) {
	now = time((time_t *) NULL);
	memcpy(tm, localtime(&now), sizeof(struct tm));
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_rel_hms)];
    }
    else if (!strcmp(format, atime_rel_hm) &&
	     (sscanf(buf, ascan_rel_hm, &hour, &minute) == 2)) {
	now = time((time_t *) NULL);
	memcpy(tm, localtime(&now), sizeof(struct tm));
	tm->tm_hour = hour;
	tm->tm_min = minute;
	bp = &buf[strlen(atime_rel_hm)];
    }
    else if (!strcmp(format, atime_rel_col_hms) &&
	     (sscanf(buf, ascan_rel_col_hms, &hour, &minute, &second) == 3)) {
	now = time((time_t *) NULL);
	memcpy(tm, localtime(&now), sizeof(struct tm));
	tm->tm_hour = hour;
	tm->tm_min = minute;
	tm->tm_sec = second;
	bp = &buf[strlen(atime_rel_col_hms)];
    }
    else if (!strcmp(format, atime_rel_col_hm) &&
	     (sscanf(buf, ascan_rel_col_hm, &hour, &minute) == 2)) {
	now = time((time_t *) NULL);
	memcpy(tm, localtime(&now), sizeof(struct tm));
	tm->tm_hour = hour;
	tm->tm_min = minute;
	bp = &buf[strlen(atime_rel_col_hm)];
    } else
	    return NULL;
    return bp;
}

krb5_error_code KRB5_CALLCONV
k_string_to_deltat(string, deltatp)
    char	FAR * string;
    krb5_deltat	FAR * deltatp;
{
    int i;
    int found;
    int svalues[4];
    int days, hours, minutes, seconds;
    krb5_deltat	dt;

    found = 0;
    days = hours = minutes = seconds = 0;
    for (i=0; i<deltat_table_nents; i++) {
	if (sscanf(string, deltat_table[i].dt_scan_format,
		   &svalues[0], &svalues[1], &svalues[2], &svalues[3]) ==
	    deltat_table[i].dt_nmatch) {
	    if (deltat_table[i].dt_dindex >= 0)
		days = svalues[deltat_table[i].dt_dindex];
	    if (deltat_table[i].dt_hindex >= 0)
		hours = svalues[deltat_table[i].dt_hindex];
	    if (deltat_table[i].dt_mindex >= 0)
		minutes = svalues[deltat_table[i].dt_mindex];
	    if (deltat_table[i].dt_sindex >= 0)
		seconds = svalues[deltat_table[i].dt_sindex];
	    found = 1;
	    break;
	}
    }
    if (found) {
	dt = days;
	dt *= 24;
	dt += hours;
	dt *= 60;
	dt += minutes;
	dt *= 60;
	dt += seconds;
	*deltatp = dt;
    }
    return((found) ? 0 : EINVAL);
}

krb5_error_code KRB5_CALLCONV
k_string_to_timestamp(string, timestampp)
    char		FAR * string;
    krb5_timestamp	FAR * timestampp;
{
    int i;
    struct tm timebuf;
    time_t now, ret_time;
    char *s;

    now = time((time_t *) NULL);
    for (i=0; i<atime_format_table_nents; i++) {
        /* We reset every time throughout the loop as the manual page
	 * indicated that no guarantees are made as to preserving timebuf
	 * when parsing fails
	 */
	memcpy(&timebuf, localtime(&now), sizeof(timebuf));
	if ((s = strptime(string, atime_format_table[i], &timebuf))
	    && (s != string)) {
	    if (timebuf.tm_year <= 0)
		continue;	/* clearly confused */
	    ret_time = mktime(&timebuf);
	    if (ret_time == (time_t) -1)
		continue;	/* clearly confused */
	    *timestampp = (krb5_timestamp) ret_time;
	    return 0;
	}
    }
    return(EINVAL);
}

krb5_error_code KRB5_CALLCONV
k_timestamp_to_string(timestamp, buffer, buflen)
    krb5_timestamp	timestamp;
    char		FAR * buffer;
    size_t		buflen;
{
    char *cp;
    time_t t = timestamp;
    
    cp = ctime(&t);
    if (strlen(cp) >= buflen)
	return ENOMEM;
    strcpy(buffer, cp);
    /* ctime returns <datestring>\n\0 */
    buffer[strlen(buffer)-1] = '\0';
    return(0);
}

krb5_error_code KRB5_CALLCONV
k_timestamp_to_sfstring(timestamp, buffer, buflen, pad)
    krb5_timestamp	timestamp;
    char		FAR * buffer;
    size_t		buflen;
    char		FAR * pad;
{
    struct tm	*tmp;
    size_t i;
    size_t	ndone;

    tmp = localtime((time_t *) &timestamp);
    ndone = 0;
    if (!ndone) {
	if (buflen >= sftime_default_len) {
	    sprintf(buffer, sftime_default_fmt,
		    tmp->tm_mday, tmp->tm_mon+1, 1900+tmp->tm_year,
		    tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	    ndone = strlen(buffer);
	}
    }
    if (ndone && pad) {
	for (i=ndone; i<buflen-1; i++)
	    buffer[i] = *pad;
	buffer[buflen-1] = '\0';
    }
    return((ndone) ? 0 : ENOMEM);
}

void KRB5_CALLCONV_C
fake_com_err(
    const char FAR *whoami,
    errcode_t code,
    const char FAR *fmt, 
    ...
    )
{
    va_list ap;
    va_start(ap, fmt);

    if (whoami) {
        fputs(whoami, stderr);
        fputs(": ", stderr);
    }
    if (code) {
        fprintf(stderr, "(error code: 0x%08X) ", code);
    }
    if (fmt) {
        vfprintf(stderr, fmt, ap);
    }
    va_end(ap);
}

DECL_FUNC_PTR(get_krb_err_txt_entry);
DECL_FUNC_PTR(krb_realmofhost);
DECL_FUNC_PTR(tkt_string);
DECL_FUNC_PTR(k_isname);
DECL_FUNC_PTR(k_isinst);
DECL_FUNC_PTR(k_isrealm);
DECL_FUNC_PTR(tf_close);
DECL_FUNC_PTR(tf_init);
DECL_FUNC_PTR(tf_get_pinst);
DECL_FUNC_PTR(tf_get_pname);
DECL_FUNC_PTR(tf_get_cred);
DECL_FUNC_PTR(kname_parse);
DECL_FUNC_PTR(krb_get_pw_in_tkt);
DECL_FUNC_PTR(k_gethostname);
DECL_FUNC_PTR(krb_get_lrealm);
DECL_FUNC_PTR(krb_get_tf_realm);
DECL_FUNC_PTR(krb_get_cred);   
DECL_FUNC_PTR(krb_mk_req);
DECL_FUNC_PTR(krb_get_krbhst);
DECL_FUNC_PTR(krb_get_tf_fullname);
DECL_FUNC_PTR(krb_check_serv);
DECL_FUNC_PTR(dest_tkt);

DECL_FUNC_PTR(krb5_free_unparsed_name);
DECL_FUNC_PTR(krb5_free_principal);
DECL_FUNC_PTR(krb5_free_context);
DECL_FUNC_PTR(krb5_free_cred_contents);
DECL_FUNC_PTR(krb5_free_ticket);
DECL_FUNC_PTR(krb5_unparse_name);
DECL_FUNC_PTR(krb5_timestamp_to_sfstring);
DECL_FUNC_PTR(krb5_init_context);
DECL_FUNC_PTR(krb5_cc_default);
DECL_FUNC_PTR(krb5_cc_resolve);
DECL_FUNC_PTR(krb5_timeofday);
DECL_FUNC_PTR(krb5_parse_name);
DECL_FUNC_PTR(krb5_build_principal_ext);
DECL_FUNC_PTR(krb5_get_in_tkt_with_password);
DECL_FUNC_PTR(krb5_get_in_tkt_with_keytab);
DECL_FUNC_PTR(krb5_get_validated_creds);
DECL_FUNC_PTR(krb5_get_renewed_creds);
DECL_FUNC_PTR(krb5_kt_default);
DECL_FUNC_PTR(krb5_kt_resolve);
DECL_FUNC_PTR(krb5_sname_to_principal);
DECL_FUNC_PTR(decode_krb5_ticket);
DECL_FUNC_PTR(krb5_enctype_to_string);

DECL_FUNC_PTR(com_err);

FUNC_INFO k4_fi[] = {
    MAKE_FUNC_INFO(get_krb_err_txt_entry),
    MAKE_FUNC_INFO(krb_realmofhost),
    MAKE_FUNC_INFO(tkt_string),
    MAKE_FUNC_INFO(k_isname),
    MAKE_FUNC_INFO(k_isinst),
    MAKE_FUNC_INFO(k_isrealm),
    MAKE_FUNC_INFO(tf_close),
    MAKE_FUNC_INFO(tf_init),
    MAKE_FUNC_INFO(tf_get_pinst),
    MAKE_FUNC_INFO(tf_get_pname),
    MAKE_FUNC_INFO(tf_get_cred),
    MAKE_FUNC_INFO(kname_parse),
    MAKE_FUNC_INFO(krb_get_pw_in_tkt),
    MAKE_FUNC_INFO(k_gethostname),
    MAKE_FUNC_INFO(krb_get_lrealm),
    MAKE_FUNC_INFO(krb_get_tf_realm),
    MAKE_FUNC_INFO(krb_get_cred),   
    MAKE_FUNC_INFO(krb_mk_req),
    MAKE_FUNC_INFO(krb_get_krbhst),
    MAKE_FUNC_INFO(krb_get_tf_fullname),
    MAKE_FUNC_INFO(krb_check_serv),
    MAKE_FUNC_INFO(dest_tkt),
    END_FUNC_INFO
};

FUNC_INFO k5_fi[] = {
    MAKE_FUNC_INFO(krb5_free_unparsed_name),
    MAKE_FUNC_INFO(krb5_free_principal),
    MAKE_FUNC_INFO(krb5_free_context),
    MAKE_FUNC_INFO(krb5_free_cred_contents),
    MAKE_FUNC_INFO(krb5_free_ticket),
    MAKE_FUNC_INFO(krb5_unparse_name),
    MAKE_FUNC_INFO(krb5_timestamp_to_sfstring),
    MAKE_FUNC_INFO(krb5_init_context),
    MAKE_FUNC_INFO(krb5_cc_default),
    MAKE_FUNC_INFO(krb5_cc_resolve),
    MAKE_FUNC_INFO(krb5_timeofday),
    MAKE_FUNC_INFO(krb5_parse_name),
    MAKE_FUNC_INFO(krb5_build_principal_ext),
    MAKE_FUNC_INFO(krb5_get_in_tkt_with_password),
    MAKE_FUNC_INFO(krb5_get_in_tkt_with_keytab),
    MAKE_FUNC_INFO(krb5_get_validated_creds),
    MAKE_FUNC_INFO(krb5_get_renewed_creds),
    MAKE_FUNC_INFO(krb5_kt_default),
    MAKE_FUNC_INFO(krb5_kt_resolve),
    MAKE_FUNC_INFO(krb5_sname_to_principal),
    MAKE_FUNC_INFO(decode_krb5_ticket),
    MAKE_FUNC_INFO(krb5_enctype_to_string),
    END_FUNC_INFO
};

FUNC_INFO ce_fi[] = {
    MAKE_FUNC_INFO(com_err),
    END_FUNC_INFO
};
