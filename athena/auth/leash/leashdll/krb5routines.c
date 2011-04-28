// Module name: krb5routines.c

#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Private Inlclude files */
#include "leashdll.h"
#include <krb.h>
#include <decldll.h>
#include <leashwin.h>

#define KRB5_DEFAULT_LIFE            60*60*10 /* 10 hours */
#define KRB5_DEFAULT_OPTIONS 0

const int LeashKrb5ErrorMessage(krb5_error_code rc, LPCSTR FailedFunctionName, 
                                int FreeContextFlag, krb5_context KRBv5Context,
                                krb5_ccache KRBv5Cache);
int LeashKRB5initialize(krb5_context *, krb5_ccache *);
int LeashKRB5destroyTicket(void);
int LeashKRB5processTicket(char *, char *, char *, char *, char *, 
                           krb5_deltat, int, krb5_context);

char *GetTicketFlag(krb5_creds *cred) 
{
   static char buf[32];
   int i = 0;

   buf[i++] = ' ';
   buf[i++] = '(';
   
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

   buf[i++] = ')';
   buf[i] = '\0';
   
   if (i <= 3)
     buf[0] = '\0';
   
   return buf;
}


EXPORT32 long 
not_an_API_LeashKRB5GetTickets(
    TICKETINFO * ticketinfo, 
    TicketList** ticketList,
    krb5_context *krbv5Context
    )
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_context	KRBv5Context;
    krb5_ccache		KRBv5Cache;
    krb5_error_code	code;
    krb5_principal	KRBv5Principal;
    krb5_flags		flags;
    krb5_cc_cursor	KRBv5Cursor;
    krb5_creds		KRBv5Credentials;
    int				StartMonth;
    int				EndMonth;
    int				StartDay;
    int				EndDay;
    int             freeContextFlag;
    char			StartTimeString[256];
    char			EndTimeString[256];
    char			fill;
    char			*ClientName;
    char			*PrincipalName; 
    char			*sServerName;
    char			Buffer[256];
    char			Months[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"};
    char			StartTime[16];
    char			EndTime[16];
    char			temp[128];
    char			*sPtr;
    char            *ticketFlag; 
    LPCSTR          functionName;

    TicketList* list = NULL; 
    
    KRBv5Context = NULL;
    KRBv5Cache = NULL;
    ticketinfo->btickets = NO_TICKETS; 
    
    if ((code = LeashKRB5initialize(&(*krbv5Context), &KRBv5Cache)))
        return(code);
	
    KRBv5Context = (*krbv5Context);
	
    flags = 0;

    if ((code = krb5_cc_set_flags(KRBv5Context, KRBv5Cache, flags)))
    {
        if (code != KRB5_FCC_NOFILE)
            LeashKrb5ErrorMessage(code, "krb5_cc_set_flags()", 0, KRBv5Context, 
                                  KRBv5Cache);        
        else if (code != KRB5_FCC_NOFILE && KRBv5Context != NULL)
        {
            if (KRBv5Cache != NULL)
                krb5_cc_close(KRBv5Context, KRBv5Cache);
        }

        return code;
    }
	
    if (code = krb5_cc_get_principal(KRBv5Context, KRBv5Cache, 
                                     &KRBv5Principal))
    {
        functionName = "krb5_cc_get_principal()";
        freeContextFlag = 1;
        goto on_error;
    }
	
    PrincipalName = NULL;
    ClientName = NULL;
    sServerName = NULL;
    if ((code = (*pkrb5_unparse_name)(KRBv5Context, KRBv5Principal, 
                                      (char **)&PrincipalName))) 
    {
        if (PrincipalName != NULL)
            (*pkrb5_free_unparsed_name)(KRBv5Context, PrincipalName);
		
        (*pkrb5_free_principal)(KRBv5Context, KRBv5Principal);
        if (KRBv5Context != NULL)
        {
            if (KRBv5Cache != NULL)
                krb5_cc_close(KRBv5Context, KRBv5Cache);
        }
	
        return(code);
    }

    if (!strcspn(PrincipalName, "@" ))
    {
        if (PrincipalName != NULL)
            (*pkrb5_free_unparsed_name)(KRBv5Context, PrincipalName);
	
        (*pkrb5_free_principal)(KRBv5Context, KRBv5Principal);
        if (KRBv5Context != NULL)
        {
            if (KRBv5Cache != NULL)
                krb5_cc_close(KRBv5Context, KRBv5Cache);
        }
		
        return(code);
    }

    if (!strcmp(ticketinfo->principal, PrincipalName))
    {
        if (strcmp(PrincipalName, ticketinfo->principal))
        {
            if (PrincipalName != NULL)
                (*pkrb5_free_unparsed_name)(KRBv5Context, PrincipalName);

            (*pkrb5_free_principal)(KRBv5Context, KRBv5Principal);
            if (KRBv5Context != NULL)
            {
                if (KRBv5Cache != NULL)
                    krb5_cc_close(KRBv5Context, KRBv5Cache);
            }
			
            return(code);
        }
    }

    wsprintf(ticketinfo->principal, "%s", PrincipalName);

    (*pkrb5_free_principal)(KRBv5Context, KRBv5Principal);
    if ((code = krb5_cc_start_seq_get(KRBv5Context, KRBv5Cache, &KRBv5Cursor))) 
    {
        functionName = "krb5_cc_start_seq_get()";
        freeContextFlag = 1;
        goto on_error; 
    }
	
    memset(&KRBv5Credentials, '\0', sizeof(KRBv5Credentials));

    while (!(code = krb5_cc_next_cred(KRBv5Context, KRBv5Cache, &KRBv5Cursor, &KRBv5Credentials))) 
    {
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

        if ((*pkrb5_unparse_name)(KRBv5Context, KRBv5Credentials.client, &ClientName))
        {
            (*pkrb5_free_cred_contents)(KRBv5Context, &KRBv5Credentials);
            LeashKrb5ErrorMessage(code, "krb5_free_cred_contents()", 0, KRBv5Context, KRBv5Cache);
			
            if (ClientName != NULL)
                (*pkrb5_free_unparsed_name)(KRBv5Context, ClientName);
			
            ClientName = NULL;
            sServerName = NULL;
            continue;
        }

        if ((*pkrb5_unparse_name)(KRBv5Context, KRBv5Credentials.server, &sServerName))
        {
            (*pkrb5_free_cred_contents)(KRBv5Context, &KRBv5Credentials);
            LeashKrb5ErrorMessage(code, "krb5_free_cred_contents()", 0, KRBv5Context, KRBv5Cache);
			
            if (ClientName != NULL)
                (*pkrb5_free_unparsed_name)(KRBv5Context, ClientName);
			
            ClientName = NULL;
            sServerName = NULL;
            continue;
        }
		
        if (!KRBv5Credentials.times.starttime)
            KRBv5Credentials.times.starttime = KRBv5Credentials.times.authtime;

        fill = ' ';
        memset(StartTimeString, '\0', sizeof(StartTimeString));
        memset(EndTimeString, '\0', sizeof(EndTimeString));
        (*pkrb5_timestamp_to_sfstring)((krb5_timestamp)KRBv5Credentials.times.starttime, StartTimeString, 17, &fill);
        (*pkrb5_timestamp_to_sfstring)((krb5_timestamp)KRBv5Credentials.times.endtime, EndTimeString, 17, &fill);
        memset(temp, '\0', sizeof(temp));
        memcpy(temp, StartTimeString, 2);
        StartDay = atoi(temp);
        memset(temp, (int)'\0', (size_t)sizeof(temp));
        memcpy(temp, EndTimeString, 2);
        EndDay = atoi(temp);

        memset(temp, '\0', sizeof(temp));
        memcpy(temp, &StartTimeString[3], 2);
        StartMonth = atoi(temp);
        memset(temp, '\0', sizeof(temp));
        memcpy(temp, &EndTimeString[3], 2);
        EndMonth = atoi(temp);

        while (1)
        {
            if ((sPtr = strrchr(StartTimeString, ' ')) == NULL)
                break;
			
            if (strlen(sPtr) != 1)
                break;
			
            (*sPtr) = 0;
        }

        while (1)
        {
            if ((sPtr = strrchr(EndTimeString, ' ')) == NULL)
                break;
			
            if (strlen(sPtr) != 1)
                break;
			
            (*sPtr) = 0;
        }

        memset(StartTime, '\0', sizeof(StartTime));
        memcpy(StartTime, &StartTimeString[strlen(StartTimeString) - 5], 5);
        memset(EndTime, '\0', sizeof(EndTime));
        memcpy(EndTime, &EndTimeString[strlen(EndTimeString) - 5], 5);

        memset(temp, '\0', sizeof(temp));
        strcpy(temp, ClientName);
		
        if (!strcmp(ClientName, PrincipalName)) 
            memset(temp, '\0', sizeof(temp));

        memset(Buffer, '\0', sizeof(Buffer));

        ticketFlag = GetTicketFlag(&KRBv5Credentials);    
        
        wsprintf(Buffer,"%s %02d %s     %s %02d %s     %s %s       %s",
                 Months[StartMonth - 1], StartDay, StartTime,
                 Months[EndMonth - 1], EndDay, EndTime,
                 sServerName,
                 temp, ticketFlag);
		
        list->theTicket = (char*) calloc(1, sizeof(Buffer));
        
        if (!list->theTicket)
        {
            MessageBox(NULL, "Memory Error", "Error", MB_OK);
            return ENOMEM;            
        }       
        
        strcpy(list->theTicket, Buffer);
        	
        ticketinfo->btickets = GOOD_TICKETS;
        ticketinfo->issue_date = KRBv5Credentials.times.starttime;
        ticketinfo->lifetime = KRBv5Credentials.times.endtime - KRBv5Credentials.times.starttime;

    	if (ClientName != NULL)
            (*pkrb5_free_unparsed_name)(KRBv5Context, ClientName);
		
        if (sServerName != NULL)
            (*pkrb5_free_unparsed_name)(KRBv5Context, sServerName);
		
        ClientName = NULL;
        sServerName = NULL;
        (*pkrb5_free_cred_contents)(KRBv5Context, &KRBv5Credentials);
    }

    if (PrincipalName != NULL)
        (*pkrb5_free_unparsed_name)(KRBv5Context, PrincipalName);
	
    if (ClientName != NULL)
        (*pkrb5_free_unparsed_name)(KRBv5Context, ClientName);
	
    if (sServerName != NULL)
        (*pkrb5_free_unparsed_name)(KRBv5Context, sServerName);

    if ((code == KRB5_CC_END) || (code == KRB5_CC_NOTFOUND))
    {
        if ((code = krb5_cc_end_seq_get(KRBv5Context, KRBv5Cache, &KRBv5Cursor))) 
        {
            functionName = "krb5_cc_end_seq_get()";
            freeContextFlag = 1;
            goto on_error;
        }

        flags = KRB5_TC_OPENCLOSE;
        if ((code = krb5_cc_set_flags(KRBv5Context, KRBv5Cache, flags))) 
        {
            functionName = "krb5_cc_set_flags()";
            freeContextFlag = 1;
            goto on_error;
        }
    }
    else 
    {
        functionName = "krb5_cc_next_cred()";
        freeContextFlag = 1;
        goto on_error;
    }

    if (KRBv5Context != NULL)
    {
        if (KRBv5Cache != NULL)
            krb5_cc_close(KRBv5Context, KRBv5Cache);
    }
	
    return(code);

 on_error:
    
    LeashKrb5ErrorMessage(code, functionName, freeContextFlag, KRBv5Context, KRBv5Cache);
    return(code);
#endif //!NO_KER5
}


/**************************************/
/* LeashKRB5processTicket():          */
/**************************************/
int
LeashKRB5processTicket(
    char *PrincipalName, 
    char *PassWord, 
    char *service, 
    char *cell, 
    char *Realm, 
    krb5_deltat LifeTime, 
    int TicketType,
    krb5_context alt_ctx
    )
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_timestamp		StartTime;
    krb5_timestamp		CurrentTime;
    int					Options;
    krb5_error_code		rc = 0;
    krb5_context		KRBv5Context = 0;
    krb5_ccache			KRBv5Cache = 0;
    krb5_principal		KRBv5Principal = 0;
    krb5_principal		TmpKRBv5Principal = 0;
    krb5_principal		server = 0;
    krb5_preauthtype	*PreAuth = NULL;
    krb5_creds			KRBv5Creds;
//    krb5_data			pwchname = {0, 8, "changepw"};
    krb5_address		**Addrs;
    char				*ClientName = 0;
    char				*pName = 0;
    char				RealmName[128];
    char				CellName[128];
    char				ServiceName[128];
    krb5_auth_context	AuthContext;
    krb5_data			InData;
    krb5_data			OutData;
    LPCSTR              functionName;
    HKEY hKey;
		
    
    if (!pkrb5_init_context)
        return 0;

    memset((char *)&KRBv5Creds, '\0', sizeof(KRBv5Creds));

    if (alt_ctx)
    {
        KRBv5Context = alt_ctx;
    }
    else
    {
        if (rc = pkrb5_init_context(&KRBv5Context))
            goto cleanup;
    }

    if (rc = pkrb5_cc_default(KRBv5Context, &KRBv5Cache))
        goto cleanup;

    if (TicketType == 1)
    {
        memset(&AuthContext, '\0', sizeof(AuthContext));
        memset(&InData, '\0', sizeof(InData));
        memset(&OutData, '\0', sizeof(OutData));

        rc = pkrb5_mk_req(KRBv5Context, &AuthContext, 
                          AP_OPTS_USE_SESSION_KEY, service, cell, 
                          &InData, KRBv5Cache, &OutData);

        if (!rc)
            pkrb5_free_data_contents(KRBv5Context, &OutData);

        goto cleanup;
    }

    LifeTime *= 5;
    LifeTime *= 60;
	
    if (LifeTime == 0)
        LifeTime = KRB5_DEFAULT_LIFE;

    StartTime = 0;
    Options = KRB5_DEFAULT_OPTIONS;
    
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, 
                                       "Software\\MIT\\Leash32\\Settings", 
                                       0, KEY_QUERY_VALUE, &hKey))
    {
        long lResult; 
        DWORD dwType; 
        DWORD dwCount;
        int value;
        // Get the registry value for 'Forwardable tickets'
        value = 0;
        dwCount = sizeof(value);
        lResult = RegQueryValueEx(hKey, "ForwardableTicket", 
                                  NULL, &dwType,
                                  (LPBYTE)&value, &dwCount);
        if (lResult == ERROR_SUCCESS && value)
        {
            if (value)
                Options |= KDC_OPT_FORWARDABLE;
        }
        // Get the registry value for 'Proxiable tickets'
        value = 0;
        dwCount = sizeof(value);
        lResult = RegQueryValueEx(hKey, "ProxiableTicket", 
                                  NULL, &dwType,
                                  (LPBYTE)&value, &dwCount);
        if (lResult == ERROR_SUCCESS && value)
        {
            if (value)
                Options |= KDC_OPT_PROXIABLE;
        }
        RegCloseKey(hKey);
    }
  
    Addrs = (krb5_address **)0;

    rc = pkrb5_timeofday(KRBv5Context, &CurrentTime); 
	
    if (rc = pkrb5_parse_name(KRBv5Context, PrincipalName, &KRBv5Principal))
    {
        functionName = "krb5_parse_name()";
        goto cleanup;
    }

    ClientName = NULL;
    rc = pkrb5_unparse_name(KRBv5Context, KRBv5Principal, &ClientName);

    memset(ServiceName, '\0', sizeof(ServiceName));
    if (strlen(service) == 0)
        strcpy(ServiceName, KRB5_TGS_NAME);
    else
        strcpy(ServiceName, service);

    memset(CellName, '\0', sizeof(CellName));
    if (strlen(cell) == 0)
        memcpy(CellName, krb5_princ_realm(KRBv5Context, KRBv5Principal)->data, krb5_princ_realm(KRBv5Context, KRBv5Principal)->length);
    else
        strcpy(CellName, cell);

    memset(RealmName, '\0', sizeof(RealmName));
    if (strlen(cell) == 0)
        memcpy(RealmName, krb5_princ_realm(KRBv5Context, KRBv5Principal)->data, krb5_princ_realm(KRBv5Context, KRBv5Principal)->length);
    else
        strcpy(RealmName, Realm);

    KRBv5Creds.client = KRBv5Principal;

    rc = pkrb5_build_principal_ext(KRBv5Context,
                                   &server,
                                   strlen(RealmName),
                                   RealmName,
                                   strlen(ServiceName),
                                   ServiceName,
                                   strlen(CellName),
                                   CellName,
                                   0);

    KRBv5Creds.server = server;
    server = 0;
    KRBv5Creds.times.starttime = 0;
    KRBv5Creds.times.endtime = CurrentTime + LifeTime;
    KRBv5Creds.times.renew_till = 0;

    if ((rc = pkrb5_get_in_tkt_with_password(KRBv5Context, Options, Addrs, 
                                             NULL, PreAuth, PassWord, 0, 
                                             &KRBv5Creds, 0)))
    {
        functionName = "krb5_get_in_tkt_with_password()";
        goto cleanup;
    }

    if (rc = krb5_cc_get_principal(KRBv5Context, KRBv5Cache, 
                                   &TmpKRBv5Principal))
    {
        if ((rc = krb5_cc_initialize(KRBv5Context, KRBv5Cache, 
                                     KRBv5Principal)) != 0)
        {
            functionName = "krb5_cc_initialize()";
            goto cleanup;
    	}
    }
    else
    {
        if ((rc = pkrb5_unparse_name(KRBv5Context, TmpKRBv5Principal, &pName)))
        {
            if ((rc = krb5_cc_initialize(KRBv5Context, KRBv5Cache, 
                                         KRBv5Principal)) != 0)
            {
                goto cleanup;
            }
        }
        else
        {
            if ((!strcmp(pName, ClientName)) && 
                (KRBv5Cache->ops->remove_cred != NULL))
            {
                krb5_cc_remove_cred(KRBv5Context, KRBv5Cache, 0, &KRBv5Creds);
            }
            else
            {
                if ((rc = krb5_cc_initialize(KRBv5Context, KRBv5Cache, 
                                             KRBv5Principal)) != 0)
                {
                    functionName = "krb5_cc_initialize()";
                    goto cleanup;
                }
            }
        }
    }

    if ((rc = krb5_cc_store_cred(KRBv5Context, KRBv5Cache, &KRBv5Creds)) != 0)
    {
        functionName = "krb5_cc_store_cred()";
        goto cleanup;
    }

 cleanup:
    if (KRBv5Creds.client)
        KRBv5Principal = 0;
    pkrb5_free_cred_contents(KRBv5Context, &KRBv5Creds);
    if (server)
        pkrb5_free_principal(KRBv5Context, server);
    if (KRBv5Principal)
        pkrb5_free_principal(KRBv5Context, KRBv5Principal);
    if (TmpKRBv5Principal)
        pkrb5_free_principal(KRBv5Context, TmpKRBv5Principal);
    if (ClientName)
        pkrb5_free_unparsed_name(KRBv5Context, ClientName);
    if (pName)
        pkrb5_free_unparsed_name(KRBv5Context, pName);

    if (KRBv5Cache)
        krb5_cc_close(KRBv5Context, KRBv5Cache);
    if (KRBv5Context && (KRBv5Context != alt_ctx))
        pkrb5_free_context(KRBv5Context);
    return(rc);
#endif //!NO_KRB5
}


/**************************************/
/* LeashKRB5destroyTicket():          */
/**************************************/
int LeashKRB5destroyTicket(void)
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_context		KRBv5Context;
    krb5_ccache			KRBv5Cache;
    krb5_error_code		rc;

    KRBv5Context = NULL;
    KRBv5Cache = NULL;
    if (rc = LeashKRB5initialize(&KRBv5Context, &KRBv5Cache))
        return(rc);
	
    rc = krb5_cc_destroy(KRBv5Context, KRBv5Cache);

    if (KRBv5Context != NULL)
        (*pkrb5_free_context)(KRBv5Context);

    return(rc);

#endif //!NO_KRB5
}

/**************************************/
/* LeashKRB5initialize():             */
/**************************************/
int LeashKRB5initialize(krb5_context *KRBv5Context, krb5_ccache *KRBv5Cache)
{
#ifdef NO_KRB5
    return(0);
#else

    LPCSTR          functionName;
    int             freeContextFlag;
    krb5_error_code	rc;

    if (pkrb5_init_context == NULL)
        return 1;

    if ((rc = (*pkrb5_init_context)(KRBv5Context)))
    {
        functionName = "krb5_init_context()";
        freeContextFlag = 0;
        goto on_error;
    }

    if ((rc = (*pkrb5_cc_default)(*KRBv5Context, KRBv5Cache)))
    {
        functionName = "krb5_cc_default()";
        freeContextFlag = 1;
        goto on_error;
    }

    return(rc);

 on_error:

    return LeashKrb5ErrorMessage(rc, functionName, freeContextFlag, *KRBv5Context, *KRBv5Cache);

#endif //!NO_KRB5
}


/**************************************/
/* LeashKrb5ErrorMessage():           */
/**************************************/
const int 
LeashKrb5ErrorMessage(krb5_error_code rc, LPCSTR FailedFunctionName, 
                      int FreeContextFlag, krb5_context KRBv5Context, 
                      krb5_ccache KRBv5Cache)
{
#ifdef NO_KRB5
    return 0;
#else
    char message[256];
    const char *errText;
    int krb5Error = ((int)(rc & 255));  
    
    /*
    switch (krb5Error)
    {
        // Wrong password
        case 31:
        case 8:
            return;
    }
    */
        
    errText = perror_message(rc);   
    _snprintf(message, sizeof(message), 
              "%s\n(Kerberos error %ld)\n\n%s failed", 
              errText, 
              krb5Error, 
              FailedFunctionName);
		
    MessageBox(NULL, message, "Kerberos Five", MB_OK | MB_ICONERROR | 
               MB_TASKMODAL | 
               MB_SETFOREGROUND);
    if (FreeContextFlag == 1)
    {
        if (KRBv5Context != NULL)
        {
            if (KRBv5Cache != NULL)
                krb5_cc_close(KRBv5Context, KRBv5Cache);
	
            (*pkrb5_free_context)(KRBv5Context);
        }
    }

    return rc;

#endif //!NO_KRB5
}
