#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cacheapi.h>

char* datestring (char* datestring, long date);

cc_int32 dump(apiCB* context)
{
    // XXX (3) - infoNC not properly filled out when returned.
    infoNC**		allCacheInfo = 0; // XXX (2) - bug in krbcc32
    infoNC*		cacheInfo;
    ccache_p*		theCache = 0;
    ccache_cit*		iterator = 0;
    cred_union*		theCreds = 0;
    cc_uint32		err;
    unsigned long	index;

    err = cc_get_NC_info (context, &allCacheInfo); // XXX (2) - another bug
    if (err != CC_NOERROR) {
        printf("Error %d getting named caches\n", err);
        return err;
    }

    for (index = 0; allCacheInfo [index] != 0; index++) {
        cacheInfo = allCacheInfo [index];
        err = cc_open (context, cacheInfo->name, cacheInfo->vers, 0L, 
                       &theCache);
        if (err != CC_NOERROR) {
            printf("Error %d opening cache: %s\n", err, cacheInfo->name);
            return err;
        }

        err = cc_seq_fetch_creds_begin (context, theCache, &iterator);
        if (err != CC_NOERROR) {
            printf("Error %d while fetching creds from cache \"%s\"\n",
                   err, cacheInfo->name);
            return err;
        }

        for (;;) {
            err = cc_seq_fetch_creds_next (context,
                                           &theCreds, iterator);
            if (err == CC_END)
                break;

            if (err != CC_NOERROR) {
                printf("Error %d while fetching creds from cache \"%s\"\n",
                       err, cacheInfo->name);
                return err;
            }
            cc_free_creds (context, &theCreds);
        }

        cc_seq_fetch_creds_end (context, &iterator);
        cc_close (context, &theCache);
    }

    cc_free_NC_info (context, &allCacheInfo);
    return 0;
}

void main(
    int argc,
    char* argv[]
    )
{
    apiCB*		context = 0; // XXX (1) - bug in krbcc32
    cc_int32		version;
    int i;
    cc_int32 err;
    DWORD t1;
    DWORD t2;
    int max;

    if (argc < 2) {
        printf("Usage: cctime loops\n");
        exit(1);
    }

    max = atoi(argv[1]);

    err = cc_initialize (&context, CC_API_VER_2, &version, NULL); // XXX (1)
    if (err != CC_NOERROR) {
        printf ("Error %d initializing the API\n", err);
        return;
    }

    t1 = GetTickCount();
    for (i = 0; !err && (i < max); i++) {
        err = dump(context);
    }
    t2 = GetTickCount();

    cc_shutdown (&context);
    if (err) {
        printf("Error %d during the test\n", err);
    } else {
        printf("Time elapesed is %u msec\n", t2 - t1);
    }
}
