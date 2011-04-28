/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit_copy.h>
#include <stdio.h>
#include <sys\types.h>
#include <krb.h>
#include <string.h>
#include <stdlib.h>

/*
 * This routine is used to generate the name of the file that holds
 * the user's cache of server tickets and associated session keys.
 *
 * If it is set, krb_ticket_string contains the ticket file name.
 * Otherwise, the filename is constructed as follows:
 *
 * If it is set, the environment variable "KRBTKFILE" will be used as
 * the ticket file name.  Otherwise TKT_ROOT (defined in "krb.h") and
 * the user's uid are concatenated to produce the ticket file name
 * (e.g., "/tmp/tkt123").  A pointer to the string containing the ticket
 * file name is returned.
 */

// XXX - We should put a critical section around here...
static char krb_ticket_string[MAXPATHLEN] = "";

static
int
try_registry(
    HKEY  hBaseKey
    )
{
    HKEY hKey;
    LONG err;
    DWORD size;

    err = RegOpenKeyEx(hBaseKey,
                       "Software\\MIT\\Kerberos4",
                       0,
                       KEY_QUERY_VALUE,
                       &hKey);
    if (err)
        return 0;
    size = sizeof(krb_ticket_string);
    err = RegQueryValueEx(hKey, "ticketfile", 0, 0, krb_ticket_string, &size);
    RegCloseKey(hKey);
    krb_ticket_string[sizeof(krb_ticket_string)-1] = 0;
    return !err;
}

static
int
is_drive_letter(
    char letter
    )
{
    // We use this function instead of isalpha because we do not want to
    // be locale-specific (in case that matters for some locales).
    return ((('A' <= letter) && (letter <= 'Z')) ||
            (('a' <= letter) && (letter <= 'z')));
}

static
void
tkt_string_fixup(
    )
{
    /* Note: We assume prefix_len is sane compared to
       sizeof(krb_ticket_string).  This is ok because we control what
       gets passed in for the prefix to this routine. */
    char prefix[] = "API:";
    int prefix_len = strlen(prefix);
    char *rest = krb_ticket_string;

    while (*rest && (*rest != '\\') && (*rest != '/') && (*rest != ':'))
        rest++;

    if (*rest && (*rest == ':') && 
        (((rest - krb_ticket_string) != 1) ||
         (((rest - krb_ticket_string) == 1) && !is_drive_letter(*(rest - 1)))))
        rest++;
    else
        rest = krb_ticket_string;

    if (strncmp(prefix, krb_ticket_string, prefix_len))
    {
        int rest_len = strlen(rest);
        int overflow = prefix_len + rest_len + 1 - sizeof(krb_ticket_string);
        if (overflow < 0) overflow = 0;
        memmove(krb_ticket_string + prefix_len, 
                rest, 
                rest_len + 1 - overflow);
        memcpy(krb_ticket_string, prefix, prefix_len);
        krb_ticket_string[sizeof(krb_ticket_string)-1] = 0;
    }
}


char *
tkt_string()
{
    char *env;

    if (*krb_ticket_string) return krb_ticket_string;

    if (env = getenv("KRBTKFILE"))
    {
        strncpy(krb_ticket_string, env, sizeof(krb_ticket_string)-1);
    }
    else if (try_registry(HKEY_CURRENT_USER) ||
             try_registry(HKEY_LOCAL_MACHINE))
    {
        // We don't need to do anything...we have the value...
    }
    else
    {
        strncpy(krb_ticket_string, "API:krb4cc", sizeof(krb_ticket_string));
    }
    krb_ticket_string[sizeof(krb_ticket_string)-1] = '\0';
    // FIXUP w/API: prefix if not already there.
    tkt_string_fixup();
    return(krb_ticket_string);
}

/*
 * This routine is used to set the name of the file that holds the user's
 * cache of server tickets and associated session keys.
 *
 * The value passed in is copied into local storage.
 *
 * NOTE:  This routine should be called during initialization, before other
 * Kerberos routines are called; otherwise tkt_string() above may be called
 * and return an undesired ticket file name until this routine is called.
 */

void
krb_set_tkt_string(
    char *val
    )
{
    *krb_ticket_string = 0;
    if (val && *val)
    {
        strncpy(krb_ticket_string, val, sizeof(krb_ticket_string)-1);
        krb_ticket_string[sizeof(krb_ticket_string)-1] = '\0';
        // XXX - only while no FILE: support...
        tkt_string_fixup();
    }
}
