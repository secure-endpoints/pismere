// Module name: krb5routines.c

#include <windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <ntsecapi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/* Private Include files */
#include "leashdll.h"
#include <leashwin.h>
#include "leash-int.h"

#define KRB5_DEFAULT_LIFE            60*60*10 /* 10 hours */

int Leash_krb5_error(krb5_error_code rc, LPCSTR FailedFunctionName, 
                     int FreeContextFlag, krb5_context *ctx,
                     krb5_ccache *cache);
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

long
Leash_convert524(
     krb5_context alt_ctx
     )
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_context ctx = 0;
    krb5_error_code code = 0;
    int icode = 0;
    krb5_principal me = 0;
    krb5_principal server = 0;
    krb5_creds *v5creds = 0;
    krb5_creds increds;
    krb5_ccache cc = 0;
    CREDENTIALS * v4creds = NULL;
    static int init_ets = 1;

    if (!pkrb5_init_context ||
        !pkrb_in_tkt ||
		!pkrb524_init_ets ||
		!pkrb524_convert_creds_kdc)
        return 0;

	v4creds = (CREDENTIALS *) malloc(sizeof(CREDENTIALS));
	memset((char *) v4creds, 0, sizeof(CREDENTIALS));

    memset((char *) &increds, 0, sizeof(increds));
    /*
      From this point on, we can goto cleanup because increds is
      initialized.
    */

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

    if ( init_ets ) {
        pkrb524_init_ets(ctx);
        init_ets = 0;
    }

    if (code = pkrb5_cc_get_principal(ctx, cc, &me))
        goto cleanup;

    if ((code = pkrb5_build_principal(ctx,
                                     &server,
                                     krb5_princ_realm(ctx, me)->length,
                                     krb5_princ_realm(ctx, me)->data,
                                     "krbtgt",
                                     krb5_princ_realm(ctx, me)->data,
                                     NULL))) {
        goto cleanup;
    }

    increds.client = me;
    increds.server = server;
    increds.times.endtime = 0;
    increds.keyblock.enctype = ENCTYPE_DES_CBC_CRC;
    if ((code = pkrb5_get_credentials(ctx, 0,
                                     cc,
                                     &increds,
                                     &v5creds))) {
        goto cleanup;
    }

    if ((icode = pkrb524_convert_creds_kdc(ctx,
                                          v5creds,
                                          v4creds))) {
        goto cleanup;
    }

    /* initialize ticket cache */
    if ((icode = pkrb_in_tkt(v4creds->pname, v4creds->pinst, v4creds->realm)
         != KSUCCESS)) {
        goto cleanup;
    }
    /* stash ticket, session key, etc. for future use */
    if ((icode = pkrb_save_credentials(v4creds->service,
                                      v4creds->instance,
                                      v4creds->realm,
                                      v4creds->session,
                                      v4creds->lifetime,
                                      v4creds->kvno,
                                      &(v4creds->ticket_st),
                                      v4creds->issue_date))) {
        goto cleanup;
    }

 cleanup:
    memset(v4creds, 0, sizeof(v4creds));
	free(v4creds);

    if (v5creds) {
		pkrb5_free_creds(ctx, v5creds);
	}
	if (increds.client == me)
		me = 0;
	if (increds.server == server)
		server = 0;
    pkrb5_free_cred_contents(ctx, &increds);
    if (server) {
        pkrb5_free_principal(ctx, server);
	}
    if (me) {
        pkrb5_free_principal(ctx, me);
	}
	pkrb5_cc_close(ctx, cc);

    if (ctx && (ctx != alt_ctx)) {
        pkrb5_free_context(ctx);
	}
    return !(code || icode);
#endif /* NO_KRB5 */    
}

#ifndef ENCTYPE_LOCAL_RC4_MD4
#define ENCTYPE_LOCAL_RC4_MD4    0xFFFFFF80
#endif

static char *
etype_string(krb5_enctype enctype)
{
    static char buf[12];

    switch (enctype) {
    case ENCTYPE_NULL:
        return "NULL";
    case ENCTYPE_DES_CBC_CRC:
        return "DES-CBC-CRC";
    case ENCTYPE_DES_CBC_MD4:
        return "DES-CBC-MD4";
    case ENCTYPE_DES_CBC_MD5:
        return "DES-CBC-MD5";
    case ENCTYPE_DES_CBC_RAW:
        return "DES-CBC-RAW";
    case ENCTYPE_DES3_CBC_SHA:
        return "DES3-CBC-SHA";
    case ENCTYPE_DES3_CBC_RAW:
        return "DES3-CBC-RAW";
    case ENCTYPE_DES_HMAC_SHA1:
        return "DES-HMAC-SHA1";
    case ENCTYPE_DES3_CBC_SHA1:
        return "DES3-CBC-SHA1";
    case ENCTYPE_AES128_CTS_HMAC_SHA1_96:
        return "AES128_CTS-HMAC-SHA1_96";
    case ENCTYPE_AES256_CTS_HMAC_SHA1_96:
        return "AES256_CTS-HMAC-SHA1_96";
    case ENCTYPE_ARCFOUR_HMAC:
        return "RC4-HMAC-NT";
    case ENCTYPE_ARCFOUR_HMAC_EXP:
        return "RC4-HMAC-NT-EXP";
    case ENCTYPE_UNKNOWN:
        return "UNKNOWN";
    case ENCTYPE_LOCAL_DES3_HMAC_SHA1:
        return "LOCAL-DES3-HMAC-SHA1";
    case ENCTYPE_LOCAL_RC4_MD4:
        return "LOCAL-RC4-MD4";
    default:
        wsprintf(buf, "#%d", enctype);
        return buf;
    }
}

char * 
one_addr(krb5_address *a)
{
    static char retstr[256];
    struct hostent *h;
    int no_resolve = 1;

    retstr[0] = '\0';

    if ((a->addrtype == ADDRTYPE_INET && a->length == 4)
#ifdef AF_INET6
        || (a->addrtype == ADDRTYPE_INET6 && a->length == 16)
#endif
        ) {
        int af = AF_INET;
#ifdef AF_INET6
        if (a->addrtype == ADDRTYPE_INET6)
            af = AF_INET6;
#endif
        if (!no_resolve) {
#ifdef HAVE_GETIPNODEBYADDR
            int err;
            h = getipnodebyaddr(a->contents, a->length, af, &err);
            if (h) {
                wsprintf(retstr, "%s", h->h_name);
                freehostent(h);
            }
#else
            h = gethostbyaddr(a->contents, a->length, af);
            if (h) {
                wsprintf(retstr,"%s", h->h_name);
            }
#endif
            if (h)
                return(retstr);
        }
        if (no_resolve || !h) {
#ifdef HAVE_INET_NTOP
            char buf[46];
            const char *name = inet_ntop(a->addrtype, a->contents, buf, sizeof(buf));
            if (name) {
                wsprintf(retstr,"%s", name);
                return;
            }
#else
            if (a->addrtype == ADDRTYPE_INET) {
                wsprintf(retstr,"%d.%d.%d.%d", a->contents[0], a->contents[1],
                       a->contents[2], a->contents[3]);
                return(retstr);
            }
#endif
        }
    }
    wsprintf(retstr,"unknown addr type %d", a->addrtype);
    return(retstr);
}

long 
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
    krb5_ticket    *tkt=NULL;
    int				StartMonth;
    int				EndMonth;
    int             RenewMonth;
    int				StartDay;
    int				EndDay;
    int             RenewDay;
    int             freeContextFlag;
    char			StartTimeString[256];
    char			EndTimeString[256];
    char            RenewTimeString[256];
    char			fill;
    char			*ClientName;
    char			*PrincipalName; 
    char			*sServerName;
    char			Buffer[256];
    char			Months[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"};
    char			StartTime[16];
    char			EndTime[16];
    char            RenewTime[16];
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

    if ((code = pkrb5_cc_set_flags(ctx, cache, flags)))
    {
        if (code != KRB5_FCC_NOFILE)
            Leash_krb5_error(code, "krb5_cc_set_flags()", 0, &ctx, 
                                  &cache);        
        else if (code != KRB5_FCC_NOFILE && ctx != NULL)
        {
            if (cache != NULL)
                pkrb5_cc_close(ctx, cache);
        }

        return code;
    }
	
    if (code = pkrb5_cc_get_principal(ctx, cache, 
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
                pkrb5_cc_close(ctx, cache);
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
                pkrb5_cc_close(ctx, cache);
        }
		
        return(code);
    }

    if ( strcmp(ticketinfo->principal, PrincipalName) )
        wsprintf(ticketinfo->principal, "%s", PrincipalName);

    (*pkrb5_free_principal)(ctx, KRBv5Principal);
    if ((code = pkrb5_cc_start_seq_get(ctx, cache, &KRBv5Cursor))) 
    {
        functionName = "krb5_cc_start_seq_get()";
        freeContextFlag = 1;
        goto on_error; 
    }
	
    memset(&KRBv5Credentials, '\0', sizeof(KRBv5Credentials));

    while (!(code = pkrb5_cc_next_cred(ctx, cache, &KRBv5Cursor, &KRBv5Credentials))) 
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
            Leash_krb5_error(code, "krb5_free_cred_contents()", 0, &ctx, &cache);
			
            if (ClientName != NULL)
                (*pkrb5_free_unparsed_name)(ctx, ClientName);
			
            ClientName = NULL;
            sServerName = NULL;
            continue;
        }

        if ((*pkrb5_unparse_name)(ctx, KRBv5Credentials.server, &sServerName))
        {
            (*pkrb5_free_cred_contents)(ctx, &KRBv5Credentials);
            Leash_krb5_error(code, "krb5_free_cred_contents()", 0, &ctx, &cache);
			
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
        memset(RenewTimeString, '\0', sizeof(RenewTimeString));
        (*pkrb5_timestamp_to_sfstring)((krb5_timestamp)KRBv5Credentials.times.starttime, StartTimeString, 17, &fill);
        (*pkrb5_timestamp_to_sfstring)((krb5_timestamp)KRBv5Credentials.times.endtime, EndTimeString, 17, &fill);
        (*pkrb5_timestamp_to_sfstring)((krb5_timestamp)KRBv5Credentials.times.renew_till, RenewTimeString, 17, &fill);
        memset(temp, '\0', sizeof(temp));
        memcpy(temp, StartTimeString, 2);
        StartDay = atoi(temp);
        memset(temp, (int)'\0', (size_t)sizeof(temp));
        memcpy(temp, EndTimeString, 2);
        EndDay = atoi(temp);
        memset(temp, (int)'\0', (size_t)sizeof(temp));
        memcpy(temp, RenewTimeString, 2);
        RenewDay = atoi(temp);

        memset(temp, '\0', sizeof(temp));
        memcpy(temp, &StartTimeString[3], 2);
        StartMonth = atoi(temp);
        memset(temp, '\0', sizeof(temp));
        memcpy(temp, &EndTimeString[3], 2);
        EndMonth = atoi(temp);
        memset(temp, '\0', sizeof(temp));
        memcpy(temp, &RenewTimeString[3], 2);
        RenewMonth = atoi(temp);

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

        while (1)
        {
            if ((sPtr = strrchr(RenewTimeString, ' ')) == NULL)
                break;
			
            if (strlen(sPtr) != 1)
                break;
			
            (*sPtr) = 0;
        }

        memset(StartTime, '\0', sizeof(StartTime));
        memcpy(StartTime, &StartTimeString[strlen(StartTimeString) - 5], 5);
        memset(EndTime, '\0', sizeof(EndTime));
        memcpy(EndTime, &EndTimeString[strlen(EndTimeString) - 5], 5);
        memset(RenewTime, '\0', sizeof(RenewTime));
        memcpy(RenewTime, &RenewTimeString[strlen(RenewTimeString) - 5], 5);

        memset(temp, '\0', sizeof(temp));
        strcpy(temp, ClientName);
		
        if (!strcmp(ClientName, PrincipalName)) 
            memset(temp, '\0', sizeof(temp));

        memset(Buffer, '\0', sizeof(Buffer));

        ticketFlag = GetTicketFlag(&KRBv5Credentials);    
        
        if (KRBv5Credentials.ticket_flags & TKT_FLG_RENEWABLE)
            wsprintf(Buffer,"%s %02d %s     %s %02d %s     [%s %02d %s]     %s %s       %s",
                      Months[StartMonth - 1], StartDay, StartTime,
                      Months[EndMonth - 1], EndDay, EndTime,
                      Months[RenewMonth - 1], RenewDay, RenewTime,
                      sServerName,
                      temp, ticketFlag);
        else
            wsprintf(Buffer,"%s %02d %s     %s %02d %s     %s %s       %s",
                 Months[StartMonth - 1], StartDay, StartTime,
                 Months[EndMonth - 1], EndDay, EndTime,
                 sServerName,
                 temp, ticketFlag);
		
        list->theTicket = (char*) calloc(1, strlen(Buffer)+1);
        if (!list->theTicket)
        {
            MessageBox(NULL, "Memory Error", "Error", MB_OK);
            return ENOMEM;            
        }       
        strcpy(list->theTicket, Buffer);

        pkrb5_decode_ticket(&KRBv5Credentials.ticket, &tkt);
        if ( tkt ) {
            wsprintf(Buffer, "Ticket Encryption Type: %s", etype_string(tkt->enc_part.enctype));
            list->tktEncType = (char*) calloc(1, strlen(Buffer)+1);
            if (!list->tktEncType)
            {
                MessageBox(NULL, "Memory Error", "Error", MB_OK);
                return ENOMEM;            
            }       
			strcpy(list->tktEncType, Buffer);
            wsprintf(Buffer, "Session Key Type: %s", etype_string(KRBv5Credentials.keyblock.enctype));
            list->keyEncType = (char*) calloc(1, strlen(Buffer)+1);
            if (!list->keyEncType)
            {
                MessageBox(NULL, "Memory Error", "Error", MB_OK);
                return ENOMEM;            
            }       
			strcpy(list->keyEncType, Buffer);
            pkrb5_free_ticket(ctx, tkt);
            tkt = NULL;
        } else {
            list->tktEncType = NULL;
            list->keyEncType = NULL;
        }

        if ( KRBv5Credentials.addresses && KRBv5Credentials.addresses[0] ) {
            int n = 0;
            while ( KRBv5Credentials.addresses[n] )
				n++;
            list->addrList = calloc(1, n * sizeof(char *));
            if (!list->addrList) {
                MessageBox(NULL, "Memory Error", "Error", MB_OK);
                return ENOMEM;            
            }
            list->addrCount = n;
            for ( n=0; n<list->addrCount; n++ ) {
                wsprintf(Buffer, "Address: %s", one_addr(KRBv5Credentials.addresses[n]));
                list->addrList[n] = (char*) calloc(1, strlen(Buffer)+1);
                if (!list->addrList[n])
                {
                    MessageBox(NULL, "Memory Error", "Error", MB_OK);
                    return ENOMEM;            
                }       
                strcpy(list->addrList[n], Buffer);
            }   
        }
        	
        ticketinfo->btickets = GOOD_TICKETS;
        ticketinfo->issue_date = KRBv5Credentials.times.starttime;
        ticketinfo->lifetime = KRBv5Credentials.times.endtime - KRBv5Credentials.times.starttime;
        ticketinfo->renew_till = KRBv5Credentials.ticket_flags & TKT_FLG_RENEWABLE ?
            KRBv5Credentials.times.renew_till : 0;

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
        if ((code = pkrb5_cc_end_seq_get(ctx, cache, &KRBv5Cursor))) 
        {
            functionName = "krb5_cc_end_seq_get()";
            freeContextFlag = 1;
            goto on_error;
        }

        flags = KRB5_TC_OPENCLOSE;
        if ((code = pkrb5_cc_set_flags(ctx, cache, flags))) 
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
            pkrb5_cc_close(ctx, cache);
    }
	
    return(code);

 on_error:
    
    Leash_krb5_error(code, functionName, freeContextFlag, &ctx, &cache);
    return(code);
#endif //!NO_KER5
}


int
LeashKRB5_renew(void)
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_error_code		        code = 0;
    krb5_context		        ctx = 0;
    krb5_ccache			        cc = 0;
    krb5_principal		        me = 0;
    krb5_principal              server = 0;
    krb5_creds			        my_creds;
    krb5_data                   *realm = 0;

	memset(&my_creds, 0, sizeof(krb5_creds));

    code = pkrb5_init_context(&ctx);
    if (code) goto cleanup;

    code = pkrb5_cc_default(ctx, &cc);
    if (code) goto cleanup;

    code = pkrb5_cc_get_principal(ctx, cc, &me);
    if (code) goto cleanup;

    realm = krb5_princ_realm(ctx, me);

    code = pkrb5_build_principal_ext(ctx, &server,
                                    realm->length,realm->data,
                                    KRB5_TGS_NAME_SIZE, KRB5_TGS_NAME,
                                    realm->length,realm->data,
                                    0);
    if ( code ) goto cleanup;

    my_creds.client = me;
    my_creds.server = server;

    code = pkrb5_get_renewed_creds(ctx, &my_creds, me, cc, NULL);
    if (code)
        Leash_krb5_error(code, "krb5_get_renewed_creds()", 0, &ctx, &cc);

    code = pkrb5_cc_initialize(ctx, cc, me);
    if (code) goto cleanup;

    code = pkrb5_cc_store_cred(ctx, cc, &my_creds);
    if (code) goto cleanup;

  cleanup:
    if (my_creds.client == me)
        my_creds.client = 0;
    if (my_creds.server == server)
        my_creds.server = 0;
    pkrb5_free_cred_contents(ctx, &my_creds);
    if (me)
        pkrb5_free_principal(ctx, me);
    if (server)
        pkrb5_free_principal(ctx, server);
    if (cc)
        pkrb5_cc_close(ctx, cc);
    if (ctx)
        pkrb5_free_context(ctx);
    return(code);
#endif /* NO_KRB5 */
}

#ifndef NO_KRB5
static krb5_error_code KRB5_CALLCONV
leash_krb5_prompter( krb5_context context,
					 void *data,
					 const char *name,
					 const char *banner,
					 int num_prompts,
					 krb5_prompt prompts[]);
#endif /* NO_KRB5 */

int
Leash_krb5_kinit(
krb5_context alt_ctx,
HWND hParent,
char *principal_name,
char *password,
krb5_deltat lifetime,
DWORD                       forwardable,
DWORD                       proxiable,
krb5_deltat                 renew_life,
DWORD                       addressless,
DWORD                       publicIP
)
{
#ifdef NO_KRB5
    return(0);
#else
    krb5_error_code		        code = 0;
    krb5_context		        ctx = 0;
    krb5_ccache			        cc = 0;
    krb5_principal		        me = 0;
    char*                       name = 0;
    krb5_creds			        my_creds;
    krb5_get_init_creds_opt     options;
    krb5_address **             addrs = NULL;
    int                         i = 0, addr_count = 0;

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
        lifetime = Leash_get_default_lifetime();
    else
        lifetime *= 5*60;

	if (renew_life > 0)
		renew_life *= 5*60;

    if (lifetime)
        pkrb5_get_init_creds_opt_set_tkt_life(&options, lifetime);
	pkrb5_get_init_creds_opt_set_forwardable(&options,
                                                 forwardable ? 1 : 0);
	pkrb5_get_init_creds_opt_set_proxiable(&options,
                                               proxiable ? 1 : 0);
	pkrb5_get_init_creds_opt_set_renew_life(&options,
                                               renew_life);
    if (addressless)
        pkrb5_get_init_creds_opt_set_address_list(&options,NULL);
    else {
		if (publicIP)
        {
            // we are going to add the public IP address specified by the user
            // to the list provided by the operating system
            krb5_address ** local_addrs=NULL;
            DWORD           netIPAddr;

            pkrb5_os_localaddr(ctx, &local_addrs);
            while ( local_addrs[i++] );
            addr_count = i + 1;

            addrs = (krb5_address **) malloc((addr_count+1) * sizeof(krb5_address *));
            if ( !addrs ) {
                pkrb5_free_addresses(ctx, local_addrs);
                assert(0);
            }
            memset(addrs, 0, sizeof(krb5_address *) * (addr_count+1));
            i = 0;
            while ( local_addrs[i] ) {
                addrs[i] = (krb5_address *)malloc(sizeof(krb5_address));
                if (addrs[i] == NULL) {
                    pkrb5_free_addresses(ctx, local_addrs);
                    assert(0);
                }

                addrs[i]->magic = local_addrs[i]->magic;
                addrs[i]->addrtype = local_addrs[i]->addrtype;
                addrs[i]->length = local_addrs[i]->length;
                addrs[i]->contents = (unsigned char *)malloc(addrs[i]->length);
                if (!addrs[i]->contents) {
                    pkrb5_free_addresses(ctx, local_addrs);
                    assert(0);
                }

                memcpy(addrs[i]->contents,local_addrs[i]->contents,
                        local_addrs[i]->length);        /* safe */
                i++;
            }
            pkrb5_free_addresses(ctx, local_addrs);

            addrs[i] = (krb5_address *)malloc(sizeof(krb5_address));
            if (addrs[i] == NULL)
                assert(0);

            addrs[i]->magic = KV5M_ADDRESS;
            addrs[i]->addrtype = AF_INET;
            addrs[i]->length = 4;
            addrs[i]->contents = (unsigned char *)malloc(addrs[i]->length);
            if (!addrs[i]->contents)
                assert(0);

            netIPAddr = htonl(publicIP);
            memcpy(addrs[i]->contents,&netIPAddr,4);
        
            pkrb5_get_init_creds_opt_set_address_list(&options,addrs);

        }
    }

    code = pkrb5_get_init_creds_password(ctx, 
                                       &my_creds, 
                                       me,
                                       password, // password
                                       leash_krb5_prompter, // prompter
                                       hParent, // prompter data
                                       0, // start time
                                       0, // service name
                                       &options);
    if (code) goto cleanup;

    code = pkrb5_cc_initialize(ctx, cc, me);
    if (code) goto cleanup;

    code = pkrb5_cc_store_cred(ctx, cc, &my_creds);
    if (code) goto cleanup;

 cleanup:
    if ( addrs ) {
        for ( i=0;i<addr_count;i++ ) {
            if ( addrs[i] ) {
                if ( addrs[i]->contents )
                    free(addrs[i]->contents);
                free(addrs[i]);
            }
        }
    }
    if (my_creds.client == me)
	my_creds.client = 0;
    pkrb5_free_cred_contents(ctx, &my_creds);
    if (name)
	pkrb5_free_unparsed_name(ctx, name);
    if (me)
	pkrb5_free_principal(ctx, me);
    if (cc)
	pkrb5_cc_close(ctx, cc);
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
	
    rc = pkrb5_cc_destroy(ctx, cache);

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

    return Leash_krb5_error(rc, functionName, freeContextFlag, ctx, cache);

#endif //!NO_KRB5
}


/**************************************/
/* Leash_krb5_error():           */
/**************************************/
int 
Leash_krb5_error(krb5_error_code rc, LPCSTR FailedFunctionName, 
                 int FreeContextFlag, krb5_context * ctx, 
                 krb5_ccache * cache)
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
        if (*ctx != NULL)
        {
            if (*cache != NULL) {
                pkrb5_cc_close(*ctx, *cache);
				*cache = NULL;
			}
	
            pkrb5_free_context(*ctx);
			*ctx = NULL;
        }
    }

    return rc;

#endif //!NO_KRB5
}


#ifndef NO_KRB5
/***************************************

Leash_ms2mit() routine and support functions are derived from work 
which is Copyright 2000 by Carnegie Mellon University.

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Carnegie Mellon
University not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

static VOID
ShowWinError(
    LPSTR szAPI,
    DWORD dwError
    )
{
#define MAX_MSG_SIZE 256

    // TODO - Write errors to event log so that scripts that don't
    // check for errors will still get something in the event log

    WCHAR szMsgBuf[MAX_MSG_SIZE];
    DWORD dwRes;

    printf("Error calling function %s: %lu\n", szAPI, dwError);

    dwRes = FormatMessage (
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwError,
        MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (char *)szMsgBuf,
        MAX_MSG_SIZE,
        NULL);
    if (0 == dwRes) {
        printf("FormatMessage failed with %d\n", GetLastError());
        ExitProcess(EXIT_FAILURE);
    }

    printf("%S",szMsgBuf);
}

static VOID
ShowLsaError(
    LPSTR szAPI,
    NTSTATUS Status
    )
{
    //
    // Convert the NTSTATUS to Winerror. Then call ShowWinError().
    //
    ShowWinError(szAPI, pLsaNtStatusToWinError(Status));
}



static BOOL
WINAPI
UnicodeToANSI(
    LPTSTR lpInputString,
    LPSTR lpszOutputString,
    int nOutStringLen
    )
{
#ifndef WIN32S
    CPINFO CodePageInfo;

    GetCPInfo(CP_ACP, &CodePageInfo);

    if (CodePageInfo.MaxCharSize > 1)
        // Only supporting non-Unicode strings
        return FALSE;
    else if (((LPBYTE) lpInputString)[1] == '\0')
    {
        // Looks like unicode, better translate it
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) lpInputString, -1,
                            lpszOutputString, nOutStringLen, NULL, NULL);
    }
    else
        lstrcpyA(lpszOutputString, (LPSTR) lpInputString);
#else
    lstrcpy(lpszOutputString, (LPSTR) lpInputString);
#endif
    return TRUE;
}  // UnicodeToANSI

static VOID
WINAPI
ANSIToUnicode(
    LPSTR  lpInputString,
    LPTSTR lpszOutputString,
    int nOutStringLen
    )
{

#ifndef WIN32S
    CPINFO CodePageInfo;

    lstrcpy(lpszOutputString, (LPTSTR) lpInputString);

    GetCPInfo(CP_ACP, &CodePageInfo);

    if (CodePageInfo.MaxCharSize > 1)
        // It must already be a Unicode string
        return;
    else if (((LPBYTE) lpInputString)[1] != '\0')
    {
        // Looks like ANSI, better translate it
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR) lpInputString, -1,
                            (LPWSTR) lpszOutputString, nOutStringLen);
    }
    else
        lstrcpy(lpszOutputString, (LPTSTR) lpInputString);
#endif
}  // ANSIToUnicode


static void
MSPrincToMITPrinc(
    KERB_EXTERNAL_NAME *msprinc,
    WCHAR *realm,
    krb5_context context,
    krb5_principal *principal
    )
{
    WCHAR princbuf[512],tmpbuf[128];
    char aname[512];
    USHORT i;
    princbuf[0]=0;
    for (i=0;i<msprinc->NameCount;i++) {
        wcsncpy(tmpbuf, msprinc->Names[i].Buffer,
                msprinc->Names[i].Length/sizeof(WCHAR));
        tmpbuf[msprinc->Names[i].Length/sizeof(WCHAR)]=0;
        if (princbuf[0])
            wcscat(princbuf, L"/");
        wcscat(princbuf, tmpbuf);
    }
    wcscat(princbuf, L"@");
    wcscat(princbuf, realm);
    UnicodeToANSI(princbuf, aname, sizeof(aname));
    pkrb5_parse_name(context, (char *)aname, principal);
}


static time_t
FileTimeToUnixTime(
    LARGE_INTEGER *ltime
    )
{
    FILETIME filetime, localfiletime;
    SYSTEMTIME systime;
    struct tm utime;
    filetime.dwLowDateTime=ltime->LowPart;
    filetime.dwHighDateTime=ltime->HighPart;
    FileTimeToLocalFileTime(&filetime, &localfiletime);
    FileTimeToSystemTime(&localfiletime, &systime);
    utime.tm_sec=systime.wSecond;
    utime.tm_min=systime.wMinute;
    utime.tm_hour=systime.wHour;
    utime.tm_mday=systime.wDay;
    utime.tm_mon=systime.wMonth-1;
    utime.tm_year=systime.wYear-1900;
    utime.tm_isdst=-1;
    return(mktime(&utime));
}

static void
MSSessionKeyToMITKeyblock(
    KERB_CRYPTO_KEY *mskey,
    krb5_context context,
    krb5_keyblock *keyblock
    )
{
    krb5_keyblock tmpblock;
    tmpblock.magic=KV5M_KEYBLOCK;
    tmpblock.enctype=mskey->KeyType;
    tmpblock.length=mskey->Length;
    tmpblock.contents=mskey->Value;
    pkrb5_copy_keyblock_contents(context, &tmpblock, keyblock);
}


static void
MSFlagsToMITFlags(
    ULONG msflags,
    ULONG *mitflags
    )
{
    *mitflags=msflags;
}

static void
MSTicketToMITTicket(
    KERB_EXTERNAL_TICKET *msticket,
    krb5_context context,
    krb5_data *ticket
    )
{
    krb5_data tmpdata, *newdata;
    tmpdata.magic=KV5M_DATA;
    tmpdata.length=msticket->EncodedTicketSize;
    tmpdata.data=msticket->EncodedTicket;
    // todo: fix this up a little. this is ugly and will break krb_free_data()
    pkrb5_copy_data(context, &tmpdata, &newdata);
    memcpy(ticket, newdata, sizeof(krb5_data));
}

static void
MSCredToMITCred(
    KERB_EXTERNAL_TICKET *msticket,
    krb5_context context,
    krb5_creds *creds
    )
{
    WCHAR wtmp[128];
    ZeroMemory(creds, sizeof(krb5_creds));
    creds->magic=KV5M_CREDS;
    wcsncpy(wtmp, msticket->TargetDomainName.Buffer,
            msticket->TargetDomainName.Length/sizeof(WCHAR));
    wtmp[msticket->TargetDomainName.Length/sizeof(WCHAR)]=0;
    MSPrincToMITPrinc(msticket->ClientName, wtmp, context, &creds->client);
    wcsncpy(wtmp, msticket->DomainName.Buffer,
            msticket->DomainName.Length/sizeof(WCHAR));
    wtmp[msticket->DomainName.Length/sizeof(WCHAR)]=0;
    MSPrincToMITPrinc(msticket->ServiceName, wtmp, context, &creds->server);
    MSSessionKeyToMITKeyblock(&msticket->SessionKey, context, 
                              &creds->keyblock);
    MSFlagsToMITFlags(msticket->TicketFlags, &creds->ticket_flags);
    creds->times.starttime=FileTimeToUnixTime(&msticket->StartTime);
    creds->times.endtime=FileTimeToUnixTime(&msticket->EndTime);
    creds->times.renew_till=FileTimeToUnixTime(&msticket->RenewUntil);

    /* MS Tickets are addressless.  MIT requires an empty address 
     * not a NULL list of addresses.
     */
    creds->addresses = (krb5_address **)malloc(sizeof(krb5_address *));
    memset(creds->addresses, 0, sizeof(krb5_address *));

    MSTicketToMITTicket(msticket, context, &creds->ticket);
}

static BOOL
PackageConnectLookup(
    HANDLE *pLogonHandle,
    ULONG *pPackageId
    )
{
    LSA_STRING Name;
    NTSTATUS Status;

    if ( pLsaConnectUntrusted == NULL || 
         pLsaLookupAuthenticationPackage == NULL ||
         pLsaCallAuthenticationPackage == NULL ||
         pLsaFreeReturnBuffer == NULL)
        return(FALSE);;

    Status = pLsaConnectUntrusted(
        pLogonHandle
        );

    if (FAILED(Status))
    {
        ShowLsaError("LsaConnectUntrusted", Status);
        return FALSE;
    }

    Name.Buffer = MICROSOFT_KERBEROS_NAME_A;
    Name.Length = strlen(Name.Buffer);
    Name.MaximumLength = Name.Length + 1;

    Status = pLsaLookupAuthenticationPackage(
        *pLogonHandle,
        &Name,
        pPackageId
        );

    if (FAILED(Status))
    {
        ShowLsaError("LsaLookupAuthenticationPackage", Status);
        return FALSE;
    }

    return TRUE;
}

static DWORD
ConcatenateUnicodeStrings(
    UNICODE_STRING *pTarget,
    UNICODE_STRING Source1,
    UNICODE_STRING Source2
    )
{
    //
    // The buffers for Source1 and Source2 cannot overlap pTarget's
    // buffer.  Source1.Length + Source2.Length must be <= 0xFFFF,
    // otherwise we overflow...
    //

    USHORT TotalSize = Source1.Length + Source2.Length;
    PBYTE buffer = (PBYTE) pTarget->Buffer;

    if (TotalSize > pTarget->MaximumLength)
        return ERROR_INSUFFICIENT_BUFFER;

    pTarget->Length = TotalSize;
    memcpy(buffer, Source1.Buffer, Source1.Length);
    memcpy(buffer + Source1.Length, Source2.Buffer, Source2.Length);
    return ERROR_SUCCESS;
}


static BOOL
GetSecurityLogonSessionData(PSECURITY_LOGON_SESSION_DATA * ppSessionData)
{
    NTSTATUS Status = 0;
    HANDLE  TokenHandle;
    TOKEN_STATISTICS Stats;
    DWORD   ReqLen;
    BOOL    Success;

    if (!ppSessionData)
        return FALSE;
    *ppSessionData = NULL;

    Success = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &TokenHandle );
    if ( !Success )
        return FALSE;

    Success = GetTokenInformation( TokenHandle, TokenStatistics, &Stats, sizeof(TOKEN_STATISTICS), &ReqLen );
    CloseHandle( TokenHandle );
    if ( !Success )
        return FALSE;

    Status = pLsaGetLogonSessionData( &Stats.AuthenticationId, ppSessionData );
    if ( FAILED(Status) || !ppSessionData )
        return FALSE;

    return TRUE;
}

//
// IsKerberosLogon() does not validate whether or not there are valid tickets in the 
// cache.  It validates whether or not it is reasonable to assume that if we 
// attempted to retrieve valid tickets we could do so.  Microsoft does not 
// automatically renew expired tickets.  Therefore, the cache could contain
// expired or invalid tickets.  Microsoft also caches the user's password 
// and will use it to retrieve new TGTs if the cache is empty and tickets
// are requested.

static BOOL
IsKerberosLogon(VOID)
{
    PSECURITY_LOGON_SESSION_DATA pSessionData = NULL;
    BOOL    Success = FALSE;

    if ( GetSecurityLogonSessionData(&pSessionData) ) {
        if ( pSessionData->AuthenticationPackage.Buffer ) {
            WCHAR buffer[256];
            WCHAR *usBuffer;
            int usLength;

            Success = FALSE;
            usBuffer = (pSessionData->AuthenticationPackage).Buffer;
            usLength = (pSessionData->AuthenticationPackage).Length;
            if (usLength < 256)
            {
                lstrcpyn (buffer, usBuffer, usLength);
                lstrcat (buffer,L"");
                if ( !lstrcmp(L"Kerberos",buffer) )
                    Success = TRUE;
            }
        }
        pLsaFreeReturnBuffer(pSessionData);
    }
    return Success;
}

static NTSTATUS
ConstructTicketRequest(UNICODE_STRING DomainName, PKERB_RETRIEVE_TKT_REQUEST * outRequest,
                       ULONG * outSize)
{
    NTSTATUS Status;
    UNICODE_STRING TargetPrefix;
    USHORT TargetSize;
    ULONG RequestSize;
    PKERB_RETRIEVE_TKT_REQUEST pTicketRequest = NULL;

    *outRequest = NULL;
    *outSize = 0;

    //
    // Set up the "krbtgt/" target prefix into a UNICODE_STRING so we
    // can easily concatenate it later.
    //

    TargetPrefix.Buffer = L"krbtgt/";
    TargetPrefix.Length = wcslen(TargetPrefix.Buffer) * sizeof(WCHAR);
    TargetPrefix.MaximumLength = TargetPrefix.Length;

    //
    // We will need to concatenate the "krbtgt/" prefix and the 
    // Logon Session's DnsDomainName into our request's target name.
    //
    // Therefore, first compute the necessary buffer size for that.
    //
    // Note that we might theoretically have integer overflow.
    //

    TargetSize = TargetPrefix.Length + DomainName.Length;

    //
    // The ticket request buffer needs to be a single buffer.  That buffer
    // needs to include the buffer for the target name.
    //

    RequestSize = sizeof(*pTicketRequest) + TargetSize;

    //
    // Allocate the request buffer and make sure it's zero-filled.
    //

    pTicketRequest = (PKERB_RETRIEVE_TKT_REQUEST) LocalAlloc(LMEM_ZEROINIT, RequestSize);
    if (!pTicketRequest)
        return GetLastError();

    //
    // Concatenate the target prefix with the previous reponse's
    // target domain.
    //

    pTicketRequest->TargetName.Length = 0;
    pTicketRequest->TargetName.MaximumLength = TargetSize;
    pTicketRequest->TargetName.Buffer = (PWSTR) (pTicketRequest + 1);
    Status = ConcatenateUnicodeStrings(&(pTicketRequest->TargetName),
                                        TargetPrefix,
                                        DomainName);
    assert(SUCCEEDED(Status));
    *outRequest = pTicketRequest;
    *outSize    = RequestSize;
    return Status;
}

//
// #define ENABLE_PURGING
// to allow the purging of expired tickets from LSA cache.  This is necessary
// to force the retrieval of new TGTs.  Microsoft does not appear to retrieve
// new tickets when they expire.  Instead they continue to accept the expired
// tickets.  I do not want to enable purging of the LSA cache without testing
// the side effects in a Windows domain with a machine which has been suspended,
// removed from the network, and resumed after ticket expiration.
//
static BOOL
GetMSTGT(
    HANDLE LogonHandle,
    ULONG PackageId,
    KERB_EXTERNAL_TICKET **ticket
    )
{
    //
    // INVARIANTS:
    //
    //   (FAILED(Status) || FAILED(SubStatus)) ==> error
    //   bIsLsaError ==> LsaCallAuthenticationPackage() error
    //

    BOOL bIsLsaError = FALSE;
    NTSTATUS Status = 0;
    NTSTATUS SubStatus = 0;

    KERB_QUERY_TKT_CACHE_REQUEST CacheRequest;
    PKERB_RETRIEVE_TKT_REQUEST pTicketRequest;
    static PKERB_RETRIEVE_TKT_RESPONSE pTicketResponse = NULL;
    ULONG RequestSize;
    ULONG ResponseSize;
#ifdef ENABLE_PURGING
    KERB_PURGE_TKT_CACHE_REQUEST PurgeRequest;
    int    purge_cache = 0;
#endif /* ENABLE_PURGING */
    int    ignore_cache = 0;

    if ( pTicketResponse ) {
        memset(pTicketResponse,0,sizeof(KERB_RETRIEVE_TKT_RESPONSE));
        pLsaFreeReturnBuffer(pTicketResponse);
        pTicketResponse = NULL;
    }

    CacheRequest.MessageType = KerbRetrieveTicketMessage;
    CacheRequest.LogonId.LowPart = 0;
    CacheRequest.LogonId.HighPart = 0;

    Status = pLsaCallAuthenticationPackage(
        LogonHandle,
        PackageId,
        &CacheRequest,
        sizeof(CacheRequest),
        &pTicketResponse,
        &ResponseSize,
        &SubStatus
        );

    if (FAILED(Status))
    {
        // if the call to LsaCallAuthenticationPackage failed we cannot
        // perform any queries most likely because the Kerberos package 
        // is not available or we do not have access
        bIsLsaError = TRUE;
        goto cleanup;
    }

    if (FAILED(SubStatus)) {
        PSECURITY_LOGON_SESSION_DATA pSessionData = NULL;
        BOOL    Success = FALSE;
        OSVERSIONINFOEX verinfo;
        int supported = 0;

        // SubStatus 0x8009030E is not documented.  However, it appears
        // to mean there is no TGT
        if (SubStatus != 0x8009030E) {
            bIsLsaError = TRUE;
            goto cleanup;
        }

        verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO *)&verinfo);
        supported = (verinfo.dwMajorVersion > 5) || 
            (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion >= 1);

        // If we could not get a TGT from the cache we won't know what the
        // Kerberos Domain should have been.  On Windows XP and 2003 Server
        // we can extract it from the Security Logon Session Data.  However,
        // the required fields are not supported on Windows 2000.  :(
        if ( supported && GetSecurityLogonSessionData(&pSessionData) ) {
            if ( pSessionData->DnsDomainName.Buffer ) {
                Status = ConstructTicketRequest(pSessionData->DnsDomainName,
                                                &pTicketRequest, &RequestSize);
                if ( FAILED(Status) ) {
                    goto cleanup;
                }
            } else {
                bIsLsaError = TRUE;
                goto cleanup;
            }
            pLsaFreeReturnBuffer(pSessionData);
        } else {
            CHAR  UserDnsDomain[256];
            WCHAR UnicodeUserDnsDomain[256];
            UNICODE_STRING wrapper;
            if ( !get_STRING_from_registry(HKEY_CURRENT_USER,
                                          "Volatile Environment",
                                          "USERDNSDOMAIN",
                                           UserDnsDomain,
                                           sizeof(UserDnsDomain)
                                           ) )
            {
                goto cleanup;
            }

            ANSIToUnicode(UserDnsDomain,UnicodeUserDnsDomain,256);
            wrapper.Buffer = UnicodeUserDnsDomain;
            wrapper.Length = wcslen(UnicodeUserDnsDomain) * sizeof(WCHAR);
            wrapper.MaximumLength = 256;

            Status = ConstructTicketRequest(wrapper,
                                             &pTicketRequest, &RequestSize);
            if ( FAILED(Status) ) {
                goto cleanup;
            }
        }
    } else {
#ifdef PURGE_ALL
        purge_cache = 1;
#else
        switch (pTicketResponse->Ticket.SessionKey.KeyType) {
        case KERB_ETYPE_DES_CBC_CRC:
        case KERB_ETYPE_DES_CBC_MD4:
        case KERB_ETYPE_DES_CBC_MD5:
        case KERB_ETYPE_NULL:
        case KERB_ETYPE_RC4_HMAC_NT: {
            FILETIME Now, EndTime, LocalEndTime;
            GetSystemTimeAsFileTime(&Now);
            EndTime.dwLowDateTime=pTicketResponse->Ticket.EndTime.LowPart;
            EndTime.dwHighDateTime=pTicketResponse->Ticket.EndTime.HighPart;
            FileTimeToLocalFileTime(&EndTime, &LocalEndTime);
            if (CompareFileTime(&Now, &LocalEndTime) >= 0) {
#ifdef ENABLE_PURGING
                purge_cache = 1;
#else
                ignore_cache = 1;
#endif /* ENABLE_PURGING */
                break;
            }
            if (pTicketResponse->Ticket.TicketFlags & KERB_TICKET_FLAGS_invalid) {
                ignore_cache = 1;
                break;      // invalid, need to attempt a TGT request
            }
            goto cleanup;   // all done
        }
        case KERB_ETYPE_RC4_MD4:
        default:
            // not supported
            ignore_cache = 1;
            break;
        }
#endif /* PURGE_ALL */

        Status = ConstructTicketRequest(pTicketResponse->Ticket.TargetDomainName,
                                        &pTicketRequest, &RequestSize);
        if ( FAILED(Status) ) {
            goto cleanup;
        }

        //
        // Free the previous response buffer so we can get the new response.
        //

        if ( pTicketResponse ) {
            memset(pTicketResponse,0,sizeof(KERB_RETRIEVE_TKT_RESPONSE));
            pLsaFreeReturnBuffer(pTicketResponse);
            pTicketResponse = NULL;
        }

#ifdef ENABLE_PURGING
        if ( purge_cache ) {
            //
            // Purge the existing tickets which we cannot use so new ones can 
            // be requested.  It is not possible to purge just the TGT.  All
            // service tickets must be purged.
            //
            PurgeRequest.MessageType = KerbPurgeTicketCacheMessage;
            PurgeRequest.LogonId.LowPart = 0;
            PurgeRequest.LogonId.HighPart = 0;
            PurgeRequest.ServerName.Buffer = L"";
            PurgeRequest.ServerName.Length = 0;
            PurgeRequest.ServerName.MaximumLength = 0;
            PurgeRequest.RealmName.Buffer = L"";
            PurgeRequest.RealmName.Length = 0;
            PurgeRequest.RealmName.MaximumLength = 0;
            Status = pLsaCallAuthenticationPackage(LogonHandle,
                                                    PackageId,
                                                    &PurgeRequest,
                                                    sizeof(PurgeRequest),
                                                    NULL,
                                                    NULL,
                                                    &SubStatus
                                                    );
        }
#endif /* ENABLE_PURGING */
    }
    
    //
    // Intialize the request of the request.
    //

    pTicketRequest->MessageType = KerbRetrieveEncodedTicketMessage;
    pTicketRequest->LogonId.LowPart = 0;
    pTicketRequest->LogonId.HighPart = 0;
    // Note: pTicketRequest->TargetName set up above
#ifdef ENABLE_PURGING
    pTicketRequest->CacheOptions = ((ignore_cache || !purge_cache) ? 
                                     KERB_RETRIEVE_TICKET_DONT_USE_CACHE : 0L);
#else
    pTicketRequest->CacheOptions = (ignore_cache ? KERB_RETRIEVE_TICKET_DONT_USE_CACHE : 0L);
#endif /* ENABLE_PURGING */
    pTicketRequest->TicketFlags = 0L;
    pTicketRequest->EncryptionType = 0L;

    Status = pLsaCallAuthenticationPackage(
        LogonHandle,
        PackageId,
        pTicketRequest,
        RequestSize,
        &pTicketResponse,
        &ResponseSize,
        &SubStatus
        );

    if (FAILED(Status) || FAILED(SubStatus))
    {
        bIsLsaError = TRUE;
        goto cleanup;
    }

    //
    // Check to make sure the new tickets we received are of a type we support
    //

    switch (pTicketResponse->Ticket.SessionKey.KeyType) {
    case KERB_ETYPE_DES_CBC_CRC:
    case KERB_ETYPE_DES_CBC_MD4:
    case KERB_ETYPE_DES_CBC_MD5:
    case KERB_ETYPE_NULL:
    case KERB_ETYPE_RC4_HMAC_NT:
        goto cleanup;   // all done
    case KERB_ETYPE_RC4_MD4:
    default:
        // not supported
        break;
    }


    //
    // Try once more but this time specify the Encryption Type
    // (This will not store the retrieved tickets in the LSA cache)
    //
    pTicketRequest->EncryptionType = ENCTYPE_DES_CBC_CRC;
    pTicketRequest->CacheOptions = KERB_RETRIEVE_TICKET_DONT_USE_CACHE;

    if ( pTicketResponse ) {
        memset(pTicketResponse,0,sizeof(KERB_RETRIEVE_TKT_RESPONSE));
        pLsaFreeReturnBuffer(pTicketResponse);
        pTicketResponse = NULL;
    }

    Status = pLsaCallAuthenticationPackage(
        LogonHandle,
        PackageId,
        pTicketRequest,
        RequestSize,
        &pTicketResponse,
        &ResponseSize,
        &SubStatus
        );

    if (FAILED(Status) || FAILED(SubStatus))
    {
        bIsLsaError = TRUE;
        goto cleanup;
    }

  cleanup:
    if ( pTicketRequest )
        pLsaFreeReturnBuffer(pTicketRequest);

    if (FAILED(Status) || FAILED(SubStatus))
    {
        if (bIsLsaError)
        {
            // XXX - Will be fixed later
            if (FAILED(Status))
                ShowLsaError("LsaCallAuthenticationPackage", Status);
            if (FAILED(SubStatus))
                ShowLsaError("LsaCallAuthenticationPackage", SubStatus);
        }
        else
        {
            ShowWinError("GetMSTGT", Status);
        }

        if (pTicketResponse) {
            memset(pTicketResponse,0,sizeof(KERB_RETRIEVE_TKT_RESPONSE));
            pLsaFreeReturnBuffer(pTicketResponse);
            pTicketResponse = NULL;
        }
        return(FALSE);
    }

    *ticket = &(pTicketResponse->Ticket);
    return(TRUE);
}
#endif /* NO_KRB5 */

BOOL
Leash_ms2mit(BOOL save_creds)
{
#ifdef NO_KRB5
    return(FALSE);
#else /* NO_KRB5 */    

    if ( save_creds ) {
        krb5_context ctx;
        krb5_error_code code;
        krb5_creds creds;
        krb5_ccache cache=NULL;
        krb5_get_init_creds_opt opts;
        HANDLE LogonHandle=NULL;
        ULONG PackageId;
        KERB_EXTERNAL_TICKET *msticket;
        BOOL  rc = FALSE;

        if(!PackageConnectLookup(&LogonHandle, &PackageId))
            return(FALSE);

        if (!GetMSTGT(LogonHandle, PackageId, &msticket)) {
            CloseHandle(LogonHandle);
            return(FALSE);
        }
        CloseHandle(LogonHandle);

        code = Leash_krb5_initialize(&ctx, &cache);
        if (code)
            return(FALSE);

        MSCredToMITCred(msticket, ctx, &creds);
        pkrb5_get_init_creds_opt_init(&opts);
        code = pkrb5_cc_initialize(ctx, cache, creds.client);
        if (code) goto cleanup;

        code = pkrb5_cc_store_cred(ctx, cache, &creds);
        if (code) goto cleanup;
        rc = TRUE;

      cleanup:
        pkrb5_cc_close(ctx, cache);
        if (ctx != NULL)
            pkrb5_free_context(ctx);
        return(rc);
    } else {
        return IsKerberosLogon();
    }
#endif /* NO_KRB5 */
}


#ifndef NO_KRB5
/* User Query data structures and functions */

struct textField {
    char * buf;                       /* Destination buffer address */
    int    len;                       /* Destination buffer length */
    char * label;                     /* Label for this field */
    char * def;                       /* Default response for this field */
    int    echo;                      /* 0 = no, 1 = yes, 2 = asterisks */
};

static int                mid_cnt = 0;
static struct textField * mid_tb = NULL;

#define ID_TEXT       150
#define ID_MID_TEXT 300

static BOOL CALLBACK 
MultiInputDialogProc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i;

    switch ( message ) {
    case WM_INITDIALOG:
        if ( GetDlgCtrlID((HWND) wParam) != ID_MID_TEXT )
        {
            SetFocus(GetDlgItem( hDialog, ID_MID_TEXT));
            return FALSE;
        }
		for ( i=0; i < mid_cnt ; i++ ) {
			if (mid_tb[i].echo == 0)
				SendDlgItemMessage(hDialog, ID_MID_TEXT+i, EM_SETPASSWORDCHAR, 32, 0);
		    else if (mid_tb[i].echo == 2) 
				SendDlgItemMessage(hDialog, ID_MID_TEXT+i, EM_SETPASSWORDCHAR, '*', 0);
		}
        return TRUE;

    case WM_COMMAND:
        switch ( LOWORD(wParam) ) {
        case IDOK:
            for ( i=0; i < mid_cnt ; i++ ) {
                if ( !GetDlgItemText(hDialog, ID_MID_TEXT+i, mid_tb[i].buf, mid_tb[i].len) )
                    *mid_tb[i].buf = '\0';
            }
            /* fallthrough */
        case IDCANCEL:
            EndDialog(hDialog, LOWORD(wParam));
            return TRUE;
        }
    }
    return FALSE;
}

static LPWORD 
lpwAlign( LPWORD lpIn )
{
    ULONG ul;

    ul = (ULONG) lpIn;
    ul += 3;
    ul >>=2;
    ul <<=2;
    return (LPWORD) ul;;
}

/*
 * dialog widths are measured in 1/4 character widths
 * dialog height are measured in 1/8 character heights
 */

static LRESULT
MultiInputDialog( HINSTANCE hinst, HWND hwndOwner, 
                  char * ptext[], int numlines, int width, 
                  int tb_cnt, struct textField * tb)
{
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    LRESULT ret;
    int nchar, i, pwid;

    hgbl = GlobalAlloc(GMEM_ZEROINIT, 4096);
    if (!hgbl)
        return -1;
 
    mid_cnt = tb_cnt;
    mid_tb = tb;

    lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);
 
    // Define a dialog box.
 
    lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU
                   | DS_MODALFRAME | WS_CAPTION | DS_CENTER 
                   | DS_SETFOREGROUND | DS_3DLOOK
                   | DS_SHELLFONT | DS_NOFAILCREATE;
    lpdt->cdit = numlines + (2 * tb_cnt) + 2;  // number of controls
    lpdt->x  = 10;  
    lpdt->y  = 10;
    lpdt->cx = 20 + width * 4; 
    lpdt->cy = 20 + (numlines + tb_cnt + 4) * 14;

    lpw = (LPWORD) (lpdt + 1);
    *lpw++ = 0;   // no menu
    *lpw++ = 0;   // predefined dialog box class (by default)

    lpwsz = (LPWSTR) lpw;
    nchar = MultiByteToWideChar (CP_ACP, 0, "", -1, lpwsz, 128);
    lpw   += nchar;
    *lpw++ = 8;                        // font size (points)
    lpwsz = (LPWSTR) lpw;
    nchar = MultiByteToWideChar (CP_ACP, 0, "MS Shell Dlg", 
                                    -1, lpwsz, 128);
    lpw   += nchar;

    //-----------------------
    // Define an OK button.
    //-----------------------
    lpw = lpwAlign (lpw); // align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE) lpw;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_BORDER;
    lpdit->dwExtendedStyle = 0;
    lpdit->x  = (lpdt->cx - 14)/4 - 20; 
    lpdit->y  = 10 + (numlines + tb_cnt + 2) * 14;
    lpdit->cx = 40; 
    lpdit->cy = 14;
    lpdit->id = IDOK;  // OK button identifier

    lpw = (LPWORD) (lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;    // button class

    lpwsz = (LPWSTR) lpw;
    nchar = MultiByteToWideChar (CP_ACP, 0, "OK", -1, lpwsz, 50);
    lpw   += nchar;
    *lpw++ = 0;           // no creation data

    //-----------------------
    // Define an Cancel button.
    //-----------------------
    lpw = lpwAlign (lpw); // align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE) lpw;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | WS_BORDER;
    lpdit->dwExtendedStyle = 0;
    lpdit->x  = (lpdt->cx - 14)*3/4 - 20; 
    lpdit->y  = 10 + (numlines + tb_cnt + 2) * 14;
    lpdit->cx = 40; 
    lpdit->cy = 14;
    lpdit->id = IDCANCEL;  // CANCEL button identifier

    lpw = (LPWORD) (lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;    // button class

    lpwsz = (LPWSTR) lpw;
    nchar = MultiByteToWideChar (CP_ACP, 0, "Cancel", -1, lpwsz, 50);
    lpw   += nchar;
    *lpw++ = 0;           // no creation data

    /* Add controls for preface data */
    for ( i=0; i<numlines; i++) {
        /*-----------------------
         * Define a static text control.
         *-----------------------*/
        lpw = lpwAlign (lpw); /* align DLGITEMTEMPLATE on DWORD boundary */
        lpdit = (LPDLGITEMTEMPLATE) lpw;
        lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
        lpdit->dwExtendedStyle = 0;
        lpdit->x  = 10; 
        lpdit->y  = 10 + i * 14;
        lpdit->cx = strlen(ptext[i]) * 4 + 10; 
        lpdit->cy = 14;
        lpdit->id = ID_TEXT + i;  // text identifier

        lpw = (LPWORD) (lpdit + 1);
        *lpw++ = 0xFFFF;
        *lpw++ = 0x0082;                         // static class

        lpwsz = (LPWSTR) lpw;
        nchar = MultiByteToWideChar (CP_ACP, 0, ptext[i], 
                                         -1, lpwsz, 2*width);
        lpw   += nchar;
        *lpw++ = 0;           // no creation data
    }
    
    for ( i=0, pwid = 0; i<tb_cnt; i++) {
        if ( pwid < strlen(tb[i].label) )
            pwid = strlen(tb[i].label);
    }

    for ( i=0; i<tb_cnt; i++) {
        /* Prompt */
        /*-----------------------
         * Define a static text control.
         *-----------------------*/
        lpw = lpwAlign (lpw); /* align DLGITEMTEMPLATE on DWORD boundary */
        lpdit = (LPDLGITEMTEMPLATE) lpw;
        lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
        lpdit->dwExtendedStyle = 0;
        lpdit->x  = 10; 
        lpdit->y  = 10 + (numlines + i + 1) * 14;
        lpdit->cx = pwid * 4; 
        lpdit->cy = 14;
        lpdit->id = ID_TEXT + numlines + i;  // text identifier

        lpw = (LPWORD) (lpdit + 1);
        *lpw++ = 0xFFFF;
        *lpw++ = 0x0082;                         // static class

        lpwsz = (LPWSTR) lpw;
        nchar = MultiByteToWideChar (CP_ACP, 0, tb[i].label ? tb[i].label : "", 
                                     -1, lpwsz, 128);
        lpw   += nchar;
        *lpw++ = 0;           // no creation data

        /*-----------------------
         * Define an edit control.
         *-----------------------*/
        lpw = lpwAlign (lpw); /* align DLGITEMTEMPLATE on DWORD boundary */
        lpdit = (LPDLGITEMTEMPLATE) lpw;
        lpdit->style = WS_CHILD | WS_VISIBLE | ES_LEFT | WS_TABSTOP | WS_BORDER | (tb[i].echo == 1 ? 0L : ES_PASSWORD);
        lpdit->dwExtendedStyle = 0;
        lpdit->x  = 10 + (pwid + 1) * 4; 
        lpdit->y  = 10 + (numlines + i + 1) * 14;
        lpdit->cx = (width - (pwid + 1)) * 4; 
        lpdit->cy = 14;
        lpdit->id = ID_MID_TEXT + i;             // identifier

        lpw = (LPWORD) (lpdit + 1);
        *lpw++ = 0xFFFF;
        *lpw++ = 0x0081;                         // edit class

        lpwsz = (LPWSTR) lpw;
        nchar = MultiByteToWideChar (CP_ACP, 0, tb[i].def ? tb[i].def : "", 
                                     -1, lpwsz, 128);
        lpw   += nchar;
        *lpw++ = 0;           // no creation data
    }

    GlobalUnlock(hgbl); 
    ret = DialogBoxIndirect(hinst, (LPDLGTEMPLATE) hgbl, 
							hwndOwner, (DLGPROC) MultiInputDialogProc); 
    GlobalFree(hgbl); 

    switch ( ret ) {
    case 0:     /* Timeout */
        return -1;
    case IDOK:
        return 1;
    case IDCANCEL:
        return 0;
    default: {
        char buf[256];
        sprintf(buf,"DialogBoxIndirect() failed: %d",GetLastError());
        MessageBox(hwndOwner,
                    buf,
                    "GetLastError()",
                    MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
        return -1;
    }
    }
}

static int
multi_field_dialog(HWND hParent, char * preface, int n, struct textField tb[])
{
	extern HINSTANCE hLeashInst;
    int maxwidth = 0;
    int numlines = 0;
    int len;
    char * plines[16], *p = preface ? preface : "";
    int i;

    for ( i=0; i<16; i++ ) 
        plines[i] = NULL;

    while (*p && numlines < 16) {
        plines[numlines++] = p;
        for ( ;*p && *p != '\r' && *p != '\n'; p++ );
        if ( *p == '\r' && *(p+1) == '\n' ) {
            *p++ = '\0';
            p++;
        } else if ( *p == '\n' ) {
            *p++ = '\0';
        } 
        if ( strlen(plines[numlines-1]) > maxwidth )
            maxwidth = strlen(plines[numlines-1]);
    }

    for ( i=0;i<n;i++ ) {
        len = strlen(tb[i].label) + 1 + (tb[i].len > 40 ? 40 : tb[i].len);
        if ( maxwidth < len )
            maxwidth = len;
    }

    return(MultiInputDialog(hLeashInst, hParent, plines, numlines, maxwidth, n, tb));
}

static krb5_error_code KRB5_CALLCONV
leash_krb5_prompter( krb5_context context,
					 void *data,
					 const char *name,
					 const char *banner,
					 int num_prompts,
					 krb5_prompt prompts[])
{
    krb5_error_code     errcode = 0;
    int                 i;
    struct textField * tb = NULL;
    int    len = 0, blen=0, nlen=0;
	HWND hParent = (HWND)data;

    if (name)
        nlen = strlen(name)+2;

    if (banner)
        blen = strlen(banner)+2;

    tb = (struct textField *) malloc(sizeof(struct textField) * num_prompts);
    if ( tb != NULL ) {
        int ok;
        memset(tb,0,sizeof(struct textField) * num_prompts);
        for ( i=0; i < num_prompts; i++ ) {
            tb[i].buf = prompts[i].reply->data;
            tb[i].len = prompts[i].reply->length;
            tb[i].label = prompts[i].prompt;
            tb[i].def = NULL;
            tb[i].echo = (prompts[i].hidden ? 2 : 1);
        }   

        ok = multi_field_dialog(hParent,(char *)banner,num_prompts,tb);
        if ( ok ) {
            for ( i=0; i < num_prompts; i++ )
                prompts[i].reply->length = strlen(prompts[i].reply->data);
        } else
            errcode = -2;
    }

    if ( tb )
        free(tb);
    if (errcode) {
        for (i = 0; i < num_prompts; i++) {
            memset(prompts[i].reply->data, 0, prompts[i].reply->length);
        }
    }
    return errcode;
}
#endif /* NO_KRB5 */