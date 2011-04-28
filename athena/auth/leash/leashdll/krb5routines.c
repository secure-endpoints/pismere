// Module name: krb5routines.c

#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Private Inlclude files */
#include "leashdll.h"
#include <krb.h>
#include <decldll.h>
#include <leashwin.h>
#include "leash-int.h"

#define KRB5_DEFAULT_LIFE            60*60*10 /* 10 hours */

int Leash_krb5_error(krb5_error_code rc, LPCSTR FailedFunctionName, 
                     int FreeContextFlag, krb5_context ctx,
                     krb5_ccache cache);
int Leash_krb5_initialize(krb5_context *, krb5_ccache *);

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
    krb5_context	ctx;
    krb5_ccache		cache;
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
    
    ctx = NULL;
    cache = NULL;
    ticketinfo->btickets = NO_TICKETS; 
    
    if ((code = Leash_krb5_initialize(&(*krbv5Context), &cache)))
        return(code);
	
    ctx = (*krbv5Context);
	
    flags = 0;

    if ((code = krb5_cc_set_flags(ctx, cache, flags)))
    {
        if (code != KRB5_FCC_NOFILE)
            Leash_krb5_error(code, "krb5_cc_set_flags()", 0, ctx, 
                                  cache);        
        else if (code != KRB5_FCC_NOFILE && ctx != NULL)
        {
            if (cache != NULL)
                krb5_cc_close(ctx, cache);
        }

        return code;
    }
	
    if (code = krb5_cc_get_principal(ctx, cache, 
                                     &KRBv5Principal))
    {
        functionName = "krb5_cc_get_principal()";
        freeContextFlag = 1;
        goto on_error;
    }
	
    PrincipalName = NULL;
    ClientName = NULL;
    sServerName = NULL;
    if ((code = (*pkrb5_unparse_name)(ctx, KRBv5Principal, 
                                      (char **)&PrincipalName))) 
    {
        if (PrincipalName != NULL)
            (*pkrb5_free_unparsed_name)(ctx, PrincipalName);
		
        (*pkrb5_free_principal)(ctx, KRBv5Principal);
        if (ctx != NULL)
        {
            if (cache != NULL)
                krb5_cc_close(ctx, cache);
        }
	
        return(code);
    }

    if (!strcspn(PrincipalName, "@" ))
    {
        if (PrincipalName != NULL)
            (*pkrb5_free_unparsed_name)(ctx, PrincipalName);
	
        (*pkrb5_free_principal)(ctx, KRBv5Principal);
        if (ctx != NULL)
        {
            if (cache != NULL)
                krb5_cc_close(ctx, cache);
        }
		
        return(code);
    }

    if (!strcmp(ticketinfo->principal, PrincipalName))
    {
        if (strcmp(PrincipalName, ticketinfo->principal))
        {
            if (PrincipalName != NULL)
                (*pkrb5_free_unparsed_name)(ctx, PrincipalName);

            (*pkrb5_free_principal)(ctx, KRBv5Principal);
            if (ctx != NULL)
            {
                if (cache != NULL)
                    krb5_cc_close(ctx, cache);
            }
			
            return(code);
        }
    }

    wsprintf(ticketinfo->principal, "%s", PrincipalName);

    (*pkrb5_free_principal)(ctx, KRBv5Principal);
    if ((code = krb5_cc_start_seq_get(ctx, cache, &KRBv5Cursor))) 
    {
        functionName = "krb5_cc_start_seq_get()";
        freeContextFlag = 1;
        goto on_error; 
    }
	
    memset(&KRBv5Credentials, '\0', sizeof(KRBv5Credentials));

    while (!(code = krb5_cc_next_cred(ctx, cache, &KRBv5Cursor, &KRBv5Credentials))) 
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

        if ((*pkrb5_unparse_name)(ctx, KRBv5Credentials.client, &ClientName))
        {
            (*pkrb5_free_cred_contents)(ctx, &KRBv5Credentials);
            Leash_krb5_error(code, "krb5_free_cred_contents()", 0, ctx, cache);
			
            if (ClientName != NULL)
                (*pkrb5_free_unparsed_name)(ctx, ClientName);
			
            ClientName = NULL;
            sServerName = NULL;
            continue;
        }

        if ((*pkrb5_unparse_name)(ctx, KRBv5Credentials.server, &sServerName))
        {
            (*pkrb5_free_cred_contents)(ctx, &KRBv5Credentials);
            Leash_krb5_error(code, "krb5_free_cred_contents()", 0, ctx, cache);
			
            if (ClientName != NULL)
                (*pkrb5_free_unparsed_name)(ctx, ClientName);
			
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
            (*pkrb5_free_unparsed_name)(ctx, ClientName);
		
        if (sServerName != NULL)
            (*pkrb5_free_unparsed_name)(ctx, sServerName);
		
        ClientName = NULL;
        sServerName = NULL;
        (*pkrb5_free_cred_contents)(ctx, &KRBv5Credentials);
    }

    if (PrincipalName != NULL)
        (*pkrb5_free_unparsed_name)(ctx, PrincipalName);
	
    if (ClientName != NULL)
        (*pkrb5_free_unparsed_name)(ctx, ClientName);
	
    if (sServerName != NULL)
        (*pkrb5_free_unparsed_name)(ctx, sServerName);

    if ((code == KRB5_CC_END) || (code == KRB5_CC_NOTFOUND))
    {
        if ((code = krb5_cc_end_seq_get(ctx, cache, &KRBv5Cursor))) 
        {
            functionName = "krb5_cc_end_seq_get()";
            freeContextFlag = 1;
            goto on_error;
        }

        flags = KRB5_TC_OPENCLOSE;
        if ((code = krb5_cc_set_flags(ctx, cache, flags))) 
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

    if (ctx != NULL)
    {
        if (cache != NULL)
            krb5_cc_close(ctx, cache);
    }
	
    return(code);

 on_error:
    
    Leash_krb5_error(code, functionName, freeContextFlag, ctx, cache);
    return(code);
#endif //!NO_KER5
}


int
Leash_krb5_kinit(
    char *principal_name,
    char *password,
    krb5_deltat lifetime,
    krb5_context alt_ctx
    )
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_error_code		code = 0;
    krb5_context		ctx = 0;
    krb5_ccache			cc = 0;
    krb5_principal		me = 0;
    char*                       name = 0;
    krb5_creds			my_creds;
    krb5_get_init_creds_opt     options;
    DWORD                       forwardable = 0;
    DWORD                       proxiable = 0;

    if (!pkrb5_init_context)
        return 0;

    pkrb5_get_init_creds_opt_init(&options);
    memset(&my_creds, 0, sizeof(my_creds));

    if (alt_ctx)
    {
        ctx = alt_ctx;
    }
    else
    {
        code = pkrb5_init_context(&ctx);
        if (code) goto cleanup;
    }

    code = pkrb5_cc_default(ctx, &cc);
    if (code) goto cleanup;

    code = pkrb5_parse_name(ctx, principal_name, &me);
    if (code) goto cleanup;

    code = pkrb5_unparse_name(ctx, me, &name);
    if (code) goto cleanup;

    if (lifetime == 0)
        lifetime = KRB5_DEFAULT_LIFE;
    else
        lifetime *= 5*60;

    if (lifetime)
        pkrb5_get_init_creds_opt_set_tkt_life(&options, lifetime);
    if (!read_registry_setting(LEASH_REG_SETTING_KRB5_FORWARDABLE,
                               &forwardable, sizeof(forwardable)))
	pkrb5_get_init_creds_opt_set_forwardable(&options,
                                                 forwardable ? 1 : 0);
    if (!read_registry_setting(LEASH_REG_SETTING_KRB5_PROXIABLE,
                               &proxiable, sizeof(proxiable)))
	pkrb5_get_init_creds_opt_set_proxiable(&options,
                                               proxiable ? 1 : 0);

    code = pkrb5_get_init_creds_password(ctx, 
                                       &my_creds, 
                                       me,
                                       password, // password
                                       0, // prompter
                                       0, // prompter data
                                       0, // start time
                                       0, // service name
                                       &options);
    if (code) goto cleanup;

    code = krb5_cc_initialize(ctx, cc, me);
    if (code) goto cleanup;

    code = krb5_cc_store_cred(ctx, cc, &my_creds);
    if (code) goto cleanup;

 cleanup:
    if (my_creds.client == me)
	my_creds.client = 0;
    pkrb5_free_cred_contents(ctx, &my_creds);
    if (name)
	pkrb5_free_unparsed_name(ctx, name);
    if (me)
	pkrb5_free_principal(ctx, me);
    if (cc)
	krb5_cc_close(ctx, cc);
    if (ctx && (ctx != alt_ctx))
	pkrb5_free_context(ctx);
    return(code);
#endif //!NO_KRB5
}


/**************************************/
/* LeashKRB5destroyTicket():          */
/**************************************/
int
Leash_krb5_kdestroy(
    void
    )
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_context		ctx;
    krb5_ccache			cache;
    krb5_error_code		rc;

    ctx = NULL;
    cache = NULL;
    if (rc = Leash_krb5_initialize(&ctx, &cache))
        return(rc);
	
    rc = krb5_cc_destroy(ctx, cache);

    if (ctx != NULL)
        pkrb5_free_context(ctx);

    return(rc);

#endif //!NO_KRB5
}

/**************************************/
/* Leash_krb5_initialize():             */
/**************************************/
int Leash_krb5_initialize(krb5_context *ctx, krb5_ccache *cache)
{
#ifdef NO_KRB5
    return(0);
#else

    LPCSTR          functionName;
    int             freeContextFlag;
    krb5_error_code	rc;

    if (pkrb5_init_context == NULL)
        return 1;

    if ((rc = (*pkrb5_init_context)(ctx)))
    {
        functionName = "krb5_init_context()";
        freeContextFlag = 0;
        goto on_error;
    }

    if ((rc = pkrb5_cc_default(*ctx, cache)))
    {
        functionName = "krb5_cc_default()";
        freeContextFlag = 1;
        goto on_error;
    }

    return(rc);

 on_error:

    return Leash_krb5_error(rc, functionName, freeContextFlag, *ctx, *cache);

#endif //!NO_KRB5
}


/**************************************/
/* Leash_krb5_error():           */
/**************************************/
int 
Leash_krb5_error(krb5_error_code rc, LPCSTR FailedFunctionName, 
                 int FreeContextFlag, krb5_context ctx, 
                 krb5_ccache cache)
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
        if (ctx != NULL)
        {
            if (cache != NULL)
                krb5_cc_close(ctx, cache);
	
            pkrb5_free_context(ctx);
        }
    }

    return rc;

#endif //!NO_KRB5
}
