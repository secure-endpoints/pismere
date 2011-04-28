#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cacheapi.h>

char* datestring (char* datestring, long date);

void main(void)
{
    // XXX (3) - infoNC not properly filled out when returned.
    infoNC**		allCacheInfo = 0; // XXX (2) - bug in krbcc32
    infoNC*		cacheInfo;
    ccache_p*		theCache = 0;
    ccache_cit*		iterator = 0;
    cred_union*		theCreds = 0;
    cc_uint32		err;
    unsigned long	index;
    apiCB*		context = 0; // XXX (1) - bug in krbcc32
    cc_int32		version;

    printf ("Dumping all credential caches... (times are in GMT)\n");

    err = cc_initialize (&context, CC_API_VER_2, &version, NULL); // XXX (1)
    if (err != CC_NOERROR) {
        printf ("*** cc_initialize returned %d ***\n", err);
        return;
    }

    err = cc_get_NC_info (context, &allCacheInfo); // XXX (2) - another bug
    if (err != CC_NOERROR) {
        printf ("*** cc_get_NC_info returned %d ***\n", err);
        return;
    }

    for (index = 0; allCacheInfo [index] != 0; index++) {
        cacheInfo = allCacheInfo [index];

        switch (cacheInfo->vers) {
        case CC_CRED_V4:
            printf ("\tv4 credentials\n");
            break;

        case CC_CRED_V5:
            printf ("\tv5 credentials\n");
            break;

        default:
            printf ("\t*** bogus credentials type %d***\n", cacheInfo->vers);
            continue;
        }

        printf ("\tfor %s\n\tin %s\n", cacheInfo->principal, cacheInfo->name);

        err = cc_open (context, cacheInfo->name, cacheInfo->vers, 0L, 
                       &theCache);
        if (err != CC_NOERROR) {
            printf ("\t*** cc_open returned %d ***\n", err);
            continue;
        }

        err = cc_seq_fetch_creds_begin (context, theCache, &iterator);
        if (err != CC_NOERROR) {
            printf ("\t*** cc_seq_fetch_creds_begin returned %d ***\n", err);
            continue;
        }

        for (;;) {
            err = cc_seq_fetch_creds_next (context,
                                           &theCreds, iterator);
            if (err == CC_END)
                break;

            if (err != CC_NOERROR) {
                printf ("\t\t*** seq_fetch_creds returned %d ***\n", err);
                continue;
            }

            switch (theCreds->cred_type) {
            case CC_CRED_V4: {
                char start [26];
                char end [26];
                printf ("\t\t%s - %s: %s.%s@%s\n",
                        datestring (start, 
                                    theCreds->cred.pV4Cred->issue_date),
                        datestring (end, 
                                    theCreds->cred.pV4Cred->issue_date + 
                                    theCreds->cred.pV4Cred->lifetime * 5 * 60),
                        theCreds->cred.pV4Cred->service,
                        theCreds->cred.pV4Cred->service_instance,
                        theCreds->cred.pV4Cred->realm);
                break;
            }

            case CC_CRED_V5: {
                char start [26];
                char end [26];
                printf ("\t\t%s - %s: %s\n",
                        datestring (start, theCreds->cred.pV5Cred->starttime),
                        datestring (end, theCreds->cred.pV5Cred->endtime),
                        theCreds->cred.pV5Cred->server);
                break;
            }
            }
            cc_free_creds (context, &theCreds);
        }

        cc_seq_fetch_creds_end (context, &iterator);
        cc_close (context, &theCache);

        printf ("\n");
    }

    cc_free_NC_info (context, &allCacheInfo);

    cc_shutdown (&context);
}

/* Lame date formatting, like ctime but with no \n */

static const char *day_name[]
= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
   "Friday", "Saturday"};

static const char *month_name[]
= {"January", "February", "March","April","May","June",
   "July", "August",  "September", "October", "November","December"};

char* datestring (char* datestring, long date)
{
    time_t timer = (time_t) date;
    struct tm tm;

    tm = *localtime (&timer);

    sprintf(datestring, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
            day_name[tm.tm_wday],
            month_name[tm.tm_mon],
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            tm.tm_year + 1970);

    return datestring;
}
