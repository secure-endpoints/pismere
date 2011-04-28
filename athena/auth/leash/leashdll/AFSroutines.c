//* Module name: AFSroutines.c

#include <windows.h>
#include <stdio.h>
#include <time.h>

/* Private Include files */
#include <conf.h>
#include <leasherr.h>
#include <krb.h>
#include "leashdll.h"

#ifndef NO_AFS
#include "rxkad.h"
#include "afs_tokens.h"
#endif

#define DLL
#include <decldll.h>
#undef DLL

#include "leash-int.h"

#define MAXCELLCHARS   64
#define MAXHOSTCHARS   64
#define MAXHOSTSPERCELL 8
#define TRANSARCAFSDAEMON "TransarcAFSDaemon"
typedef struct {
    char name[MAXCELLCHARS];
    short numServers;
    short flags;
    struct sockaddr_in hostAddr[MAXHOSTSPERCELL];
    char hostName[MAXHOSTSPERCELL][MAXHOSTCHARS];
    char *linkedCell;
} afsconf_cell;

DWORD   AfsOnLine = 1;

EXPORT32 int not_an_API_LeashAFSGetToken(TicketList** ticketList);
DWORD GetServiceStatus(LPSTR lpszMachineName, LPSTR lpszServiceName, DWORD *lpdwCurrentState);
BOOL SetAfsStatus(DWORD AfsStatus);
BOOL GetAfsStatus(DWORD *AfsStatus);
void Leash_afs_error(LONG rc, LPCSTR FailedFunctionName);

static char *afs_realm_of_cell(afsconf_cell *);
static long get_cellconfig_callback(void *, struct sockaddr_in *, char *);
static int get_cellconfig(char *, afsconf_cell *, char *);

/**************************************/
/* LeashAFSdestroyToken():            */
/**************************************/
int
Leash_afs_unlog(
    void
    )
{
#ifdef NO_AFS
    return(0);
#else
    long	rc;
    char    HostName[64];
    DWORD   CurrentState;

    if ((pktc_ListTokens == NULL) ||
        (pktc_GetToken == NULL) ||
        (pktc_SetToken == NULL) ||
        (pcm_GetRootCellName == NULL) ||
        (pcm_SearchCellFile == NULL))
    {
        return(0);
    }

    if (GetAfsStatus(&AfsOnLine) && !AfsOnLine)
        return(0);

    CurrentState = 0;
    memset(HostName, '\0', sizeof(HostName));
    gethostname(HostName, sizeof(HostName));
    if (GetServiceStatus(HostName, TRANSARCAFSDAEMON, &CurrentState) != NOERROR)
        return(0);
    if (CurrentState != SERVICE_RUNNING)
        return(0);

    rc = (*pktc_ForgetAllTokens)();

    return(0);
#endif
}


int FAR not_an_API_LeashAFSGetToken(TicketList** ticketList)
{
#ifdef NO_AFS
    return(0);
#else
    struct ktc_principal    aserver;
    struct ktc_principal    aclient;
    struct ktc_token        atoken;
    int                     EndMonth;
    int                     EndDay;
    int                     cellNum;
    int                     BreakAtEnd;
    char                    UserName[64];
    char                    CellName[64];
    char                    ServiceName[64];
    char                    InstanceName[64];
    char                    EndTime[16];
    char                    Buffer[256];
    char                    Months[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"};
    char                    TokenStatus[16];
    time_t                  CurrentTime;
    struct tm               *newtime;
    DWORD                   CurrentState;
    DWORD                   rc;
    char                    HostName[64];


    TicketList* list = NULL; 
	
    if ((pktc_ListTokens == NULL) ||
        (pktc_GetToken == NULL) ||
        (pktc_SetToken == NULL) ||
        (pcm_GetRootCellName == NULL) ||
        (pcm_SearchCellFile == NULL))
    {
        return(0);
    }

    if (GetAfsStatus(&AfsOnLine) && !AfsOnLine)
        return(0);

    CurrentState = 0;
    memset(HostName, '\0', sizeof(HostName));
    gethostname(HostName, sizeof(HostName));
    if (GetServiceStatus(HostName, TRANSARCAFSDAEMON, &CurrentState) != NOERROR)
        return(0);
    if (CurrentState != SERVICE_RUNNING)
        return(0);

    BreakAtEnd = 0;
    cellNum = 0;
    while (1) 
    {
        if ((rc = (*pktc_ListTokens)(cellNum, &cellNum, &aserver)))
        {
            if (rc != KTC_NOENT)
            {
                AfsOnLine = 0;
                SetAfsStatus(AfsOnLine);
                break;
            }
            if (BreakAtEnd == 1)
                break;
        }
        BreakAtEnd = 1;
        memset(&atoken, '\0', sizeof(atoken));
        if ((rc = (*pktc_GetToken)(&aserver, &atoken, sizeof(atoken), &aclient)))
        {
            if (rc == KTC_ERROR)
            {
                AfsOnLine = 0;
                SetAfsStatus(AfsOnLine);
                break;
            }
            continue;
        }

        if (!list)
        {
            list = (TicketList*) calloc(1, sizeof(TicketList));
            (*ticketList) = list; 
        }    
        else 
        {
            list->next = (struct TicketList*) calloc(1, sizeof(TicketList));
            list = (TicketList*) list->next;
        }
        
        CurrentTime = time(NULL);

        newtime = localtime(&atoken.endTime);

        memset(UserName, '\0', sizeof(UserName));
        strcpy(UserName, aclient.name);

        memset(CellName, '\0', sizeof(CellName));
        strcpy(CellName, aclient.cell);
	
        memset(ServiceName, '\0', sizeof(ServiceName));
        strcpy(ServiceName, aserver.name);
	
        memset(InstanceName, '\0', sizeof(InstanceName));
        strcpy(InstanceName, aserver.instance);
	
        memset(TokenStatus, '\0', sizeof(TokenStatus));

        EndDay = newtime->tm_mday;

        EndMonth = newtime->tm_mon + 1;;

        sprintf(EndTime, "%02d:%02d:%02d", newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

        sprintf(Buffer,"                               %s %02d %s      %s%s%s@%s  %s",		 
                Months[EndMonth - 1], EndDay, EndTime,
                ServiceName,
                InstanceName[0] ? "." : "",
                InstanceName,
                CellName,
                TokenStatus);
			
         
        list->theTicket = (char*) calloc(1, sizeof(Buffer));
        if (!list->theTicket)
        {
            MessageBox(NULL, "Memory Error", "Error", MB_OK);
            return ENOMEM;            
        }       
        
        strcpy(list->theTicket, Buffer);
    }
	
    return(0);
#endif
}


int
Leash_afs_klog(
    char *service,
    char *cell,
    char *realm,
    int LifeTime
    )
{
#ifdef NO_AFS
    return(0);
#else
    long	rc;
    CREDENTIALS	creds;
    KTEXT_ST	ticket;
    struct ktc_principal	aserver;
    struct ktc_principal	aclient;
    char	username[BUFSIZ];	/* To hold client username structure */
    char	realm_of_user[REALM_SZ]; /* Kerberos realm of user */
    char	realm_of_cell[REALM_SZ]; /* Kerberos realm of cell */
    char	local_cell[MAXCELLCHARS+1];
    char	Dmycell[MAXCELLCHARS+1];
    struct ktc_token	atoken;
    struct ktc_token	btoken;
    afsconf_cell	ak_cellconfig; /* General information about the cell */
    char	RealmName[128];
    char	CellName[128];
    char	ServiceName[128];
    DWORD       CurrentState;
    char        HostName[64];

    if ((pktc_ListTokens == NULL) ||
        (pktc_GetToken == NULL) ||
        (pktc_SetToken == NULL) ||
        (pcm_GetRootCellName == NULL) ||
        (pcm_SearchCellFile == NULL))
    {
        return(0);
    }

    if (GetAfsStatus(&AfsOnLine) && !AfsOnLine)
        return(0);

    CurrentState = 0;
    memset(HostName, '\0', sizeof(HostName));
    gethostname(HostName, sizeof(HostName));
    if (GetServiceStatus(HostName, TRANSARCAFSDAEMON, &CurrentState) != NOERROR)
        return(0);
    if (CurrentState != SERVICE_RUNNING)
        return(0);

    memset(RealmName, '\0', sizeof(RealmName));
    memset(CellName, '\0', sizeof(CellName));
    memset(ServiceName, '\0', sizeof(ServiceName));
    memset(realm_of_user, '\0', sizeof(realm_of_user));
    memset(realm_of_cell, '\0', sizeof(realm_of_cell));
    memset(Dmycell, '\0', sizeof(Dmycell));

    // NULL or empty cell returns information on local cell
    if (rc = get_cellconfig(Dmycell, &ak_cellconfig, local_cell))
    {
        Leash_afs_error(rc, "get_cellconfig()");
        AfsOnLine = 0;
        SetAfsStatus(AfsOnLine);
        return(rc);
    }

    if ((rc = (*pkrb_get_tf_realm)((*ptkt_string)(), realm_of_user)) != KSUCCESS)
        {
        AfsOnLine = 0;
        SetAfsStatus(AfsOnLine);
        return(rc);
        }

    strcpy(realm_of_cell, afs_realm_of_cell(&ak_cellconfig));

    if (strlen(service) == 0)
        strcpy(ServiceName, "afs");
    else
        strcpy(ServiceName, service);

    if (strlen(cell) == 0)
        strcpy(CellName, local_cell);
    else
        strcpy(CellName, cell);

    if (strlen(realm) == 0)
        strcpy(RealmName, realm_of_cell);
    else
        strcpy(RealmName, realm);

    memset(&creds, '\0', sizeof(creds));
    rc = (*pkrb_get_cred)(ServiceName, CellName, RealmName, &creds);
    if (rc != KSUCCESS)
    {
        if ((rc = (*pkrb_mk_req)(&ticket, ServiceName, CellName, RealmName, 0)) == KSUCCESS)
        {
            if ((rc = (*pkrb_get_cred)(ServiceName, CellName, RealmName, &creds)) != KSUCCESS)
            {
                Leash_afs_error(rc, "krb_get_cred()");
                AfsOnLine = 0;
                SetAfsStatus(AfsOnLine);
                return(rc);
            }
        }
        else
        {
            AfsOnLine = 0;
            SetAfsStatus(AfsOnLine);
            return(rc);
        }
    }

    memset(&aserver, '\0', sizeof(aserver));
    strncpy(aserver.name, creds.service, strlen(creds.service));
    strncpy(aserver.cell, creds.instance, strlen(creds.instance));

    strcpy(username, creds.pname);
    if (creds.pinst[0]) 
    {
        strcat(username, ".");
        strcat(username, creds.pinst);
    }

    memset(&atoken, '\0', sizeof(atoken));
    atoken.kvno = creds.kvno;
    atoken.startTime = creds.issue_date;
    atoken.endTime = creds.issue_date + (LifeTime * 300);
    memcpy(&atoken.sessionKey, creds.session, 8);
    atoken.ticketLen = creds.ticket_st.length;
    memcpy(atoken.ticket, creds.ticket_st.dat, atoken.ticketLen);

    if (!(rc = (*pktc_GetToken)(&aserver, &btoken, sizeof(btoken), &aclient)) &&
        atoken.kvno == btoken.kvno &&
        atoken.ticketLen == btoken.ticketLen &&
        !memcmp(&atoken.sessionKey, &btoken.sessionKey, sizeof(atoken.sessionKey)) &&
        !memcmp(atoken.ticket, btoken.ticket, atoken.ticketLen)) 
    {
        AfsOnLine = 0;
        SetAfsStatus(AfsOnLine);
        return(0);
    }

    // * Reset the "aclient" structure before we call ktc_SetToken.
    // * This structure was first set by the ktc_GetToken call when
    // * we were comparing whether identical tokens already existed.

    strncpy(aclient.name, username, MAXKTCNAMELEN - 1);
    strcpy(aclient.instance, "");
    strncpy(aclient.cell, creds.realm, MAXKTCREALMLEN - 1);

    // * NOTE: On WIN32, the order of SetToken params changed...
    // * to   ktc_SetToken(&aserver, &aclient, &atoken, 0)
    // * from ktc_SetToken(&aserver, &atoken, &aclient, 0) on Unix...

    if (rc = (*pktc_SetToken)(&aserver, &aclient, &atoken, 0))
    {
        Leash_afs_error(rc, "ktc_SetToken()");
        AfsOnLine = 0;
        SetAfsStatus(AfsOnLine);
        return(rc);
    }

    return(0);
#endif
}

/**************************************/
/* afs_realm_of_cell():               */
/**************************************/
static char *afs_realm_of_cell(afsconf_cell *cellconfig)
{
#ifdef NO_AFS
    return(0);
#else
    char krbhst[MAX_HSTNM];
    static char krbrlm[REALM_SZ+1];

    if (!cellconfig)
        return 0;

    strcpy(krbrlm, (char *)(*pkrb_realmofhost)(cellconfig->hostName[0]));
    if ((*pkrb_get_krbhst)(krbhst, krbrlm, 1) != KSUCCESS)
    {
        char *s = krbrlm;
        char *t = cellconfig->name;
        int c;

        while (c = *t++)
        {
            if (islower(c)) c=toupper(c);
            *s++ = c;
        }
        *s++ = 0;
    }
    return(krbrlm);
#endif
}

/**************************************/
/* get_cellconfig():                  */
/**************************************/
static int get_cellconfig(char *cell, afsconf_cell *cellconfig, char *local_cell)
{
#ifdef NO_AFS
    return(0);
#else
    int	rc;

    local_cell[0] = (char)0;
    memset(cellconfig, 0, sizeof(*cellconfig));

    /* WIN32: cm_GetRootCellName(local_cell) - NOTE: no way to get max chars */
    if (rc = (*pcm_GetRootCellName)(local_cell)) 
    {
        return(rc);
    }

    if (strlen(cell) == 0)
        strcpy(cell, local_cell);

    /* WIN32: cm_SearchCellFile(cell, pcallback, pdata) */
    strcpy(cellconfig->name, cell);
    return((*pcm_SearchCellFile)(cell, get_cellconfig_callback, (void *)cellconfig)); 
#endif
}

/**************************************/
/* get_cellconfig_callback():          */
/**************************************/
static long get_cellconfig_callback(void *cellconfig, struct sockaddr_in *addrp, char *namep)
{
#ifdef NO_AFS
    return(0);
#else
    afsconf_cell *cc = (afsconf_cell *)cellconfig;

    cc->hostAddr[cc->numServers] = *addrp;
    strcpy(cc->hostName[cc->numServers], namep);
    cc->numServers++;
    return(0);
#endif
}


/**************************************/
/* Leash_afs_error():           */
/**************************************/
void
Leash_afs_error(LONG rc, LPCSTR FailedFunctionName)
{
#ifdef NO_AFS
    return;
#else
    char message[256];
    const char *errText; 

    // Using AFS defines as error messages for now, until Transarc 
    // gets back to me with "string" translations of each of these 
    // const. defines. 
    switch (rc)
    {
    case KTC_ERROR:		
        errText = "KTC_ERROR";
        break;
    case KTC_TOOBIG:		
        errText = "KTC_TOOBIG";
    case KTC_INVAL:		
        errText = "KTC_INVAL";
        break;
    case KTC_NOENT:		
        errText = "KTC_NOENT";
        break;
    case KTC_PIOCTLFAIL:		
        errText = "KTC_PIOCTLFAIL";
        break;
    case KTC_NOPIOCTL:		
        errText = "KTC_NOPIOCTL";
        break;   
    case KTC_NOCELL:		
        errText = "KTC_NOCELL";
        break;    
    case KTC_NOCM:		
        errText = "KTC_NOCM: The service, Transarc AFS Daemon, most likely is not started!";
        break;
    default:
        errText = "Unknown error!";
        break;
    };

    sprintf(message, "%s\n(%s failed)", errText, FailedFunctionName);
    MessageBox(NULL, message, "AFS", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
    return;

#endif 
}

DWORD GetServiceStatus( 
    LPSTR lpszMachineName, 
    LPSTR lpszServiceName,
    DWORD *lpdwCurrentState) 
{ 
#ifdef NO_AFS
    return(NOERROR);
#else
    DWORD           hr               = NOERROR; 
    SC_HANDLE       schSCManager     = NULL; 
    SC_HANDLE       schService       = NULL; 
    DWORD           fdwDesiredAccess = 0; 
    SERVICE_STATUS  ssServiceStatus  = {0}; 
    BOOL            fRet             = FALSE; 

    if ((pOpenSCManagerA == NULL) ||
        (pOpenServiceA == NULL) ||
        (pQueryServiceStatus == NULL) ||
        (pCloseServiceHandle == NULL))
        {
        *lpdwCurrentState = SERVICE_RUNNING;
        return(NOERROR);
        }

    *lpdwCurrentState = 0; 
 
    fdwDesiredAccess = GENERIC_READ; 
 
    schSCManager = (*pOpenSCManagerA)(lpszMachineName,  
                                 NULL,
                                 fdwDesiredAccess); 
 
    if(schSCManager == NULL) 
    { 
        hr = GetLastError();
        goto cleanup; 
    } 
 
    schService = (*pOpenServiceA)(schSCManager,
                             lpszServiceName,
                             fdwDesiredAccess); 
 
    if(schService == NULL) 
    { 
        hr = GetLastError();
        goto cleanup; 
    } 
 
    fRet = (*pQueryServiceStatus)(schService,
                              &ssServiceStatus); 
 
    if(fRet == FALSE) 
    { 
        hr = GetLastError(); 
        goto cleanup; 
    } 
 
    *lpdwCurrentState = ssServiceStatus.dwCurrentState; 
 
cleanup: 
 
    (*pCloseServiceHandle)(schService); 
    (*pCloseServiceHandle)(schSCManager); 
 
    return(hr); 
#endif 
} 

BOOL
SetAfsStatus(
    DWORD AfsStatus
    )
{
#ifdef NO_AFS
    return(TRUE);
#else
    return write_registry_setting(LEASH_REG_SETTING_AFS_STATUS, 
                                  REG_DWORD, &AfsStatus, 
                                  sizeof(AfsStatus)) ? FALSE : TRUE;
#endif
}

BOOL
GetAfsStatus(
    DWORD *AfsStatus
    )
{
#ifdef NO_AFS
    return(TRUE);
#else
    return read_registry_setting(LEASH_REG_SETTING_AFS_STATUS, 
                                 AfsStatus, sizeof(DWORD)) ? FALSE : TRUE;
#endif 
}
