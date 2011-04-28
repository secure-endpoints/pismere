/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/debug.c,v $
 * $Author: dalmeida $
 *
 * Copyright 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit_copy.h>

#include "conf.h"

#ifdef WINDOWS
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#endif
/* Declare global debugging variables. */
#include <krb.h>

#ifdef WIN32
int krb_ap_req_debug = 1;
int krb_debug = 1;
#else
int krb_ap_req_debug = 0;
int krb_debug = 0;
#endif

static int appl_krb_ap_req_debug = 0;
static int appl_krb_debug = 0;

int set_krb_debug(int i)
{
    appl_krb_debug = i;
    krb_debug = i;
    return krb_debug;
}

int set_krb_ap_req_debug(int i)
{
    appl_krb_ap_req_debug = i;
    krb_ap_req_debug = i;
    return krb_ap_req_debug;
}

#ifdef GUIONLY

int
vkdebug(
    char *fmt,
    va_list ap
    )
{
    char buffer[256];
    char debugFilePath[MAX_PATH]; 
    char savchr;
    int value, bufsz;
    FILE* dstream;
    CHAR*  Env[] = {"TEMP", "TMP", "HOME", NULL};
    CHAR** pEnv = Env;
    CHAR* ptestenv = getenv(*pEnv);
    HWND hDebugWindow = NULL; 
			
    value = wvsprintf(buffer, fmt, ap);

    hDebugWindow = FindWindow(NULL, "Leash Debug Window");
    if (!hDebugWindow)
    {
        if ((appl_krb_debug && krb_debug) || 
            (appl_krb_ap_req_debug && krb_ap_req_debug))
        {
            // old debug message box
            MessageBox(NULL,(LPSTR)buffer,(LPSTR)"stdout",MB_OK);
        }
        return value;
    }
    hDebugWindow = GetTopWindow(hDebugWindow);
    hDebugWindow  = GetNextWindow(hDebugWindow, GW_HWNDNEXT);	

    // Check all possible 'KRB' system varables to place the debug log file in
    while (*pEnv)
    {
        if (ptestenv)
        { // reset debug file
            strcpy(debugFilePath, ptestenv);
            strcat(debugFilePath, "\\LshDebug.log");
            break;			
        }
        pEnv++;
    }

    // Continue with debug stuff
    if (ptestenv && (dstream = fopen(debugFilePath, "a+")) != NULL)   
    {
        fputs(buffer, dstream);
        fclose(dstream);   
    }
    else
    {
        if (ptestenv)
        {
            SendMessage(hDebugWindow, LB_ADDSTRING, 0, 
                        (LPARAM)(LPCTSTR)
                        "Error::Writing to Leash Debug Log File failed!");
        }
    }

    bufsz = strlen(buffer) -1;
    savchr = *(buffer + bufsz); 
    *(buffer + bufsz) = 0; 
    SendMessage(hDebugWindow, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR) &buffer); 
    *(buffer + bufsz) = savchr; 
    return value;
}

#else

int
vkdebug(
    char *fmt,
    va_list ap
    )
{
    return vprintf(fmt, ap);
}

#endif // GUIONLY

int
kdebug(
    char *fmt,
    ...
    )
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vkdebug(fmt, ap);
    va_end(ap);
    return ret;
}

int
kdebug_ap(
    char *fmt,
    ...
    )
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vkdebug(fmt, ap);
    va_end(ap);
    return ret;
}

int
kdebug_err(
    char *fmt,
    ...
    )
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}
