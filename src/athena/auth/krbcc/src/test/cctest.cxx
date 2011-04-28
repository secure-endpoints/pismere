#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "cacheapi.h"
#include "testapi.h"
#include "debug.h"
#include "opts.hxx"

void _CRTAPI1 main(int argc, char **argv)
{
    DWORD status = 0;

    SetDebugMode(DM_STDOUT);

    ParseOpts::Opts opts = { 0 };
    ParseOpts PO("e");
    PO.Parse(opts, argc, argv);

    CCTEST_functions f = { 0 };
    f.size = sizeof(CCTEST_functions);
    status = CCTEST(&f);
    if (status)
        printf("Could not grab CCTEST interface (%d)\n", status);
    CLEANUP_ON_STATUS(status);

    printf("Handling API reconnect issues...\n");

    if (opts.pszEndpoint) {
        f.shutdown();
        status = f.reconnect(opts.pszEndpoint);
        if (status)
            printf("Could not reconnect (%d)\n", status);
        CLEANUP_ON_STATUS(status);
    }

    printf("Calling API...\n");

    {
        apiCB* ctx = 0;
        cc_int32 ver = 0;
        const char* vendor = 0;
        cc_int32 rc = cc_initialize(&ctx, 2, &ver, &vendor);
        printf("rc = %d, ctx = 0x%08X, ver = %d, vendor = %s\n",
               rc, ctx, ver, vendor);
        rc = cc_shutdown(&ctx);
        printf("rc = %d, ctx = 0x%08X\n", rc, ctx);
        printf("Press a key...");
        getchar();
        printf("Shutting down\n");
        f.shutdown();  // shut down the server side
        status = rc;
    }

 cleanup:
    exit(status);
}
