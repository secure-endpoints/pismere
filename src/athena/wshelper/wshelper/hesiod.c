/*
  @doc HESIOD
 
  @module hesiod.c |

  This module contains the defintions for the exported functions:
 		hes_to_bind
 		hes_resolve
 		hes_error
  as well as the internal function hes_init. The hes_init function
  is the one that determines what the Hesiod servers are for your
  site and will parse the configuration files, if any are
  present.
 
  WSHelper DNS/Hesiod Library for WINSOCK

*/

/* This file is part of the Hesiod library.
 * 
 * The BIND 4.8.1 implementation of T_TXT is incorrect; BIND 4.8.1 declares
 * it as a NULL terminated string.  The RFC defines T_TXT to be a length
 * byte followed by arbitrary changes.
 * 
 * Because of this incorrect declaration in BIND 4.8.1, when this bug is fixed,
 * T_TXT requests between machines running different versions of BIND will
 * not be compatible (nor is there any way of adding compatibility).
 * 
 * Copyright 1988 by the Massachusetts Institute of Technology.  See the
 * file <mit-copyright.h> for copying and distribution information.
 */

#define index(str, c) strchr(str,c)
#include <stdio.h>
#include <errno.h>

#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "u-compat.h"

#include <sys/types.h>
#include <arpa/nameser.h>
#include <hesiod.h>
#include <resolv.h>
#include "resscan.h"

#include "resource.h"

extern nsmsg_p far _resolve(LPSTR name, int class, int type, retransXretry_t patience );

#ifndef _WIN32
#define strncpy _fstrncpy
#define strcmp _fstrcmp
#define strlen _fstrlen
#endif

#define USE_HS_QUERY	/* undefine this if your higher-level name servers */
			/* don't know class HS */

char HesConfigFile[_MAX_PATH];
static char Hes_LHS[256];
static char Hes_RHS[256];
static int Hes_Errno = HES_ER_UNINIT;

retransXretry_t NoRetryTime = { 0, 0};


/*

  @func int | hes_init |

  This function is not exported.  It takes no arguments. However it is
  important to understand how this works. It sets the global variables
  Hes_LHS and Hes_RHS which are used to form the Hesiod
  queries. Understanding how this works and setting up the correct
  configuration will determine if the Hesiod queries will work at your
  site. Settings can be configured by makgin source code changes and
  rebuilding the DLL, editing resources in the DLL, using a
  configuration file, or setting an environment variable.

  The function first tries to open the HesConfigFile and set the
  Hes_RHS and Hes_LHS variables from this. If there is no config file
  then the function tries to load a string resource from the DLL to
  set the LHS and RHS. If the string resources cannot be loaded then
  the LHS and RHS will be set by the values of DEF_LHS and DEF_RHS,
  these are defined in hesiod.h. Note that the string resources are by
  default set to these same values since the RC files include hesiod.h

  Finally if the user sets the environment variable HES_DOMAIN the RHS
  will be overridden by the value of the HES_DOMAIN value.

  Note that LoadString requires us to first find the module handle of
  the DLL. We have to use the internal module name as defined in the
  DEF file. If you change the library name within the DEF file you
  also need to change the appropriate string in hesiod.c

*/
int hes_init( void )
{
    register FILE *fp;
    register char FAR *key;
    register char FAR *cp;
    // register char FAR **cpp;
    // int len;
    char buf[MAXDNAME+7];
    HMODULE hModWSHelp;
#if !defined( MSDOS ) && !defined( WIN32 ) /* il 8/17/95 -- NT is not DOS */
    char *calloc(), *getenv();
#endif

    Hes_Errno = HES_ER_UNINIT;
    Hes_LHS[0] = '\0';
    Hes_RHS[0] = '\0';

    // Note: these must match the DEF file entries
#if defined (_WIN32)
    hModWSHelp = GetModuleHandle( "WSHELP32" );
#else
    hModWSHelp = GetModuleHandle( "WSHELPER" );
#endif

    if(!LoadString( hModWSHelp, IDS_DEF_HES_CONFIG_FILE, 
                    HesConfigFile, sizeof(HesConfigFile) )){
        strcpy( HesConfigFile, HESIOD_CONF);
    }

    if ((fp = fopen(HesConfigFile, "r")) == NULL) {
        /* use defaults compiled in */
        /* no file or no access uses defaults */
        /* but poorly formed file returns error */
		
        if(!LoadString( hModWSHelp, IDS_DEF_HES_RHS, Hes_RHS, sizeof(Hes_RHS) )){
            strcpy( Hes_RHS, DEF_RHS);
        }

        if(!LoadString( hModWSHelp, IDS_DEF_HES_LHS, Hes_LHS, sizeof(Hes_LHS) )){
            strcpy( Hes_LHS, DEF_LHS);
        }

    } else {
        while(fgets((LPSTR) buf, MAXDNAME+7, fp) != NULL) {
            cp = (LPSTR) buf;
            if (*cp == '#' || *cp == '\n'){
                continue;
            }
            while(*cp == ' ' || *cp == '\t'){
                cp++;
            }
            key = cp;
            while(*cp != ' ' && *cp != '\t' && *cp != '='){
                cp++;
            }
            *cp++ = '\0';
            if (strcmp(key, "lhs") == 0){
                strncpy(&Hes_LHS[0], cp, (strlen(cp)-1));
            } else if (strcmp(key, "rhs") == 0){
                strncpy(&Hes_RHS[0], cp, (strlen(cp)-1));
            } else {
                continue;
            }
            while(*cp == ' ' || *cp == '\t' || *cp == '='){
                cp++;
            }
            if (*cp != '.') {
                Hes_Errno = HES_ER_CONFIG;
                fclose(fp);
                return(Hes_Errno);
            }
            // len = strlen(cp);
            // *cpp = calloc((unsigned int) len, sizeof(char));
            // (void) strncpy(*cpp, cp, len-1);
        }
        fclose(fp);
    }
    /* see if the RHS is overridden by environment variable */
    if ((cp = getenv("HES_DOMAIN")) != NULL){
        // Hes_RHS = strcpy(malloc(strlen(cp)+1),cp);
        strcpy(Hes_RHS,cp);
    }
    /* the LHS may be null, the RHS must not be null */
    if (Hes_RHS == NULL)
        Hes_Errno = HES_ER_CONFIG;
    else
        Hes_Errno = HES_ER_OK;	
    return(Hes_Errno);
}


/* 

  @func char * WINAPI  | hes_to_bind |

  The hes_to_bind function use the LHS and RHS values and 
  binds them with the parameters so that a well formed DNS query may
  be performed.

  @parm LPSTR | HesiodName     | The Hesiod name such as a username or 
                                 service name
  @parm LPSTR | HesiodNameType | The Hesiod name type such as pobox, passwd, 
                                 or sloc

  @rdesc Returns NULL if there was an error. Otherwise the pointer to a 
         string containing a valid query is returned.

*/
char *
#if defined  (_WINDLL) || defined (_DLL) || defined(_MT)
WINAPI 
#endif
hes_to_bind(LPSTR HesiodName,			
            LPSTR HesiodNameType)		
{
    register char *cp, **cpp;
    static char bindname[MAXDNAME];
    char *RHS;

    if (Hes_Errno == HES_ER_UNINIT || Hes_Errno == HES_ER_CONFIG)
        (void) hes_init();
    if (Hes_Errno == HES_ER_CONFIG) return(NULL);
    if (cp = index(HesiodName,'@')) {
        if (index(++cp,'.'))
            RHS = cp;
        else
            if (cpp = hes_resolve(cp, "rhs-extension"))
                RHS = *cpp;
            else {
                Hes_Errno = HES_ER_NOTFOUND;
                return(NULL);
            }
        (void) strcpy(bindname,HesiodName);
        (*index(bindname,'@')) = '\0';
    } else {
        RHS = Hes_RHS;
        (void) strcpy(bindname, HesiodName);
    }
    (void) strcat(bindname, ".");
    (void) strcat(bindname, HesiodNameType);
    if (Hes_LHS) {
        if (Hes_LHS[0] != '.')
            (void) strcat(bindname,".");
        (void) strcat(bindname, Hes_LHS);
    }
    if (RHS[0] != '.')
        (void) strcat(bindname,".");
    (void) strcat(bindname, RHS);
/*
    if(cpp != NULL ){	
        free( *cpp );
        *cpp = NULL;
    }
*/
    return(bindname);
}


/*
  @func char ** WINAPI | hes_resolve |

  This is the primary interface to the Hesiod name server.  It takes
  two arguments, a name to be resolved and a string, known as a
  HesiodNameType. Remarks If the environment variable HES_DOMAIN is
  set, this domain will override what is in /etc/athena/hesiod.conf.
  FILES: hesiod.h, hesiod.conf

  @parm LPSTR | HesiodName | The name to be resovled.
  @parm LPSTR | HesiodNameType | A valid Hesiod name type.

  @rdesc It returns a NULL terminated vector of strings (a la argv),
  one for each resource record containing Hesiod data, or NULL if
  there is any error. If there is an error call hes_error() to get
  further information.

*/
char **
#if defined (_WINDLL) || defined (_WIN32)
WINAPI
#endif
hes_resolve(LPSTR HesiodName, LPSTR HesiodNameType)
{
    register char FAR *cp;
    static char FAR *retvec[100];
    char FAR *ocp;
    char FAR *dst;
#if !defined( MSDOS ) && !defined( WIN32 ) /* il 8/17/95 -- NT is not DOS */
    char *calloc();
#endif
    char *freeme;
    int i, j, n;
#ifndef WINDOWS
    struct nsmsg *ns, *_resolve();
    rr_t *rp;
#else
    struct nsmsg FAR *ns;
    struct nsmsg FAR *_resolve();
    rr_t FAR *rp;
#endif
#if !defined( MSDOS ) && !defined( WIN32 ) /* il 8/17/95 -- NT is not DOS */
    extern int errno;
#endif

    cp = hes_to_bind(HesiodName, HesiodNameType);
    if (cp == NULL) return(NULL);
    errno = 0;
    ns = _resolve(cp, C_IN, T_TXT, NoRetryTime);
    if (errno == ETIMEDOUT || errno == ECONNREFUSED) {
        Hes_Errno = HES_ER_NET;
        return(NULL);
    }
    if (ns == NULL || ns->ns_off <= 0) {
        Hes_Errno = HES_ER_NOTFOUND;
        return(NULL);
    }
    for(i = j = 0, rp = &ns->rr; i < ns->ns_off; rp++, i++) {
        if (
            rp->class == C_IN &&
            rp->type == T_TXT) { /* skip CNAME records */
            retvec[j] = calloc(rp->dlen + 1, sizeof(char));
            freeme = retvec[j]; /* pbh 8-1-93 */
            dst = retvec[j];
            ocp = cp = rp->data;
            while (cp < ocp + rp->dlen) {
                n = (unsigned char) *cp++;
#ifndef _WINDLL
                (void) bcopy(cp, dst, n);
#else
                (void) memcpy(dst, cp, n);
#endif
                cp += n;
                dst += n;
            }
            *dst = 0;
            j++;
        }
    }
    retvec[j] = 0;
    // free( freeme ); /* pbh 9/96 */
    return(retvec);
}


/* 
@func int WINAPI | hes_error |

The  function  hes_error may be called to determine the 
source of the error.  It will return one of the HES_ER_* codes defined 
in hesiod.h. It does not take an argument.



*/
int 
#if defined (_WINDLL) || (_WIN32)
WINAPI
#endif
hes_error(void)
{
    return(Hes_Errno);
}
