/*
 * @doc WSHLOAD
 *
 * @module wshload.c |
 *
 * Provide a static library to which DLLs which can be linked which will
 * attempt to manually load the WSHELP32.DLL when needed.  If the DLL
 * is not available then an error value will be returned from each 
 * function call.
 *
 */

#include <windows.h>
#include <wshelper.h>


/* From wshelper.h */
static struct hostent * (__stdcall * p_rgethostbyname)(char *name)=NULL;
static struct hostent * (__stdcall * p_rgethostbyaddr)(char *addr, 
                                              int len, int type)=NULL;
static struct servent * (__stdcall * p_rgetservbyname)(LPSTR name, 
                                              LPSTR proto)=NULL; 
static LPSTR (__stdcall * p_gethinfobyname)(LPSTR name)=NULL;
static LPSTR (__stdcall * p_getmxbyname)(LPSTR name)=NULL;
static LPSTR (__stdcall * p_getrecordbyname)(LPSTR name, int rectype)=NULL;
static DWORD (__stdcall * p_rrhost)( LPSTR lpHost )=NULL;
static unsigned long (__stdcall * p_inet_aton)(register const char *cp, 
                                      struct in_addr *addr)=NULL;
static DWORD (* p_WhichOS)( DWORD *check)=NULL;
static int (__stdcall * p_wsh_gethostname)(char* name, int size)=NULL;
static int (__stdcall * p_wsh_getdomainname)(char* name, int size)=NULL;
static LONG (* p_WSHGetHostID)()=NULL;

/* From resolv.h */

static int  (__stdcall * p_res_init)()=NULL;
static void (__stdcall * p_res_setopts)(long opts)=NULL;
static long (__stdcall * p_res_getopts)(void)=NULL;
static int  (__stdcall * p_res_mkquery)(int op, const char  *dname, 
                                        int qclass, int type, 
                                        const char  *data, int datalen, 
                                        const struct rrec  *newrr,
                                        char  *buf, int buflen)=NULL;
static int  (__stdcall * p_res_send)(const char  *msg, int msglen, 
                                     char  *answer, int anslen)=NULL;
static int  (__stdcall * p_res_querydomain)(const char  *name, 
                                            const char  *domain, 
                                            int qclass, int type, 
                                            u_char  *answer, int anslen)=NULL;
static int  (__stdcall * p_res_search)(const char  *name, 
                                       int qclass, int type, 
                                       u_char  *answer, int anslen)=NULL;

static int  (__stdcall * p_dn_comp)(const u_char  *exp_dn, 
                                    u_char  *comp_dn, 
                                    int length, u_char  *  *dnptrs, 
                                    u_char  *  *lastdnptr)=NULL;
static int  (__stdcall * p_rdn_expand)(const u_char  *msg, 
                                      const u_char  *eomorig, 
                                      const u_char  *comp_dn, 
                                      u_char  *exp_dn, 
                                      int length)=NULL;

/* From heisod.h */

static LPSTR (__stdcall * p_hes_to_bind)(LPSTR HesiodName, LPSTR HesiodNameType)=NULL;

static LPSTR * (__stdcall * p_hes_resolve)(LPSTR HesiodName,LPSTR HesiodNameType)=NULL;

static int (__stdcall * p_hes_error)(void)=NULL;
static struct hes_postoffice  * (__stdcall * p_hes_getmailhost)(LPSTR user)=NULL;
static struct servent  * (__stdcall * p_hes_getservbyname)(LPSTR name, 
                                                              LPSTR proto)=NULL;
static struct passwd  * (__stdcall * p_hes_getpwnam)(LPSTR nam)=NULL;
static struct passwd  * (__stdcall * p_hes_getpwuid)(int uid)=NULL;

static HINSTANCE hWSHELP32 = NULL;

static int
load_wshelp32(void)
{
    static int load_attempted = 0;  /* have we attempted to load WSHELP32.DLL? */

    if ( load_attempted ) {
        if ( hWSHELP32 != NULL )
            return 1;
        else
            return 0;
    }
     
    load_attempted = 1;
    hWSHELP32 = LoadLibrary("WSHELP32");
    if ( hWSHELP32 == NULL )
        return 0;

    (FARPROC) p_rgethostbyname= GetProcAddress( hWSHELP32, "rgethostbyname" );
    (FARPROC) p_rgethostbyaddr= GetProcAddress( hWSHELP32, "rgethostbyaddr" );
    (FARPROC) p_rgetservbyname= GetProcAddress( hWSHELP32, "rgetservbyname" );
    (FARPROC) p_gethinfobyname= GetProcAddress( hWSHELP32, "gethinfobyname" );
    (FARPROC) p_getmxbyname= GetProcAddress( hWSHELP32, "getmxbyname" );
    (FARPROC) p_getrecordbyname= GetProcAddress( hWSHELP32, "getrecordbyname" );
    (FARPROC) p_rrhost= GetProcAddress( hWSHELP32, "rrhost" );
    (FARPROC) p_inet_aton= GetProcAddress( hWSHELP32, "inet_aton" );
    (FARPROC) p_WhichOS= GetProcAddress( hWSHELP32, "WhichOS" );
    (FARPROC) p_wsh_gethostname= GetProcAddress( hWSHELP32, "wsh_gethostname" );
    (FARPROC) p_wsh_getdomainname= GetProcAddress( hWSHELP32, "wsh_getdomainname" );
    (FARPROC) p_WSHGetHostID= GetProcAddress( hWSHELP32, "WSHGetHostID" );
    (FARPROC) p_res_init= GetProcAddress( hWSHELP32, "res_init" );
    (FARPROC) p_res_setopts= GetProcAddress( hWSHELP32, "res_setopts" );
    (FARPROC) p_res_getopts= GetProcAddress( hWSHELP32, "res_getopts" );
    (FARPROC) p_res_mkquery= GetProcAddress( hWSHELP32, "res_mkquery" );
    (FARPROC) p_res_send= GetProcAddress( hWSHELP32, "res_send" );
    (FARPROC) p_res_querydomain= GetProcAddress( hWSHELP32, "res_querydomain" );
    (FARPROC) p_res_search= GetProcAddress( hWSHELP32, "res_search" );
    (FARPROC) p_dn_comp= GetProcAddress( hWSHELP32, "dn_comp" );
    (FARPROC) p_rdn_expand= GetProcAddress( hWSHELP32, "rdn_expand" );
    (FARPROC) p_hes_to_bind= GetProcAddress( hWSHELP32, "hes_to_bind" );
    (FARPROC) p_hes_resolve= GetProcAddress( hWSHELP32, "hes_resolve" );
    (FARPROC) p_hes_error= GetProcAddress( hWSHELP32, "hes_error" );
    (FARPROC) p_hes_getmailhost= GetProcAddress( hWSHELP32, "hes_getmailhost" );
    (FARPROC) p_hes_getservbyname= GetProcAddress( hWSHELP32, "hes_getservbyname" );
    (FARPROC) p_hes_getpwnam= GetProcAddress( hWSHELP32, "hes_getpwnam" );
    (FARPROC) p_hes_getpwuid= GetProcAddress( hWSHELP32, "hes_getpwuid" );

    return(1);
}


struct hostent * __stdcall 
rgethostbyname(char *name)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_rgethostbyname )
        return p_rgethostbyname(name);
    return NULL;
}

struct hostent * __stdcall 
rgethostbyaddr(char *addr, int len, int type)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_rgethostbyaddr )
        return p_rgethostbyaddr(addr,len,type);
    return NULL;
}

struct servent * __stdcall 
rgetservbyname(LPSTR name, LPSTR proto)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_rgetservbyname )
        return p_rgetservbyname(name,proto);
    return NULL;
}

LPSTR __stdcall 
gethinfobyname(LPSTR name)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_gethinfobyname )
        return p_gethinfobyname(name);
    return NULL;
        
}

LPSTR __stdcall 
getmxbyname(LPSTR name)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_getmxbyname )
        return p_getmxbyname(name);
    return NULL;
}

LPSTR __stdcall 
getrecordbyname(LPSTR name, int rectype)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_getrecordbyname )
        return p_getrecordbyname(name,rectype);
    return NULL;
        
}

DWORD __stdcall 
rrhost( LPSTR lpHost )
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_rrhost )
        return p_rrhost(lpHost);
    return 0;
        
}
unsigned long __stdcall 
inet_aton(register const char *cp,struct in_addr *addr)
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_inet_aton )
        return p_inet_aton(cp,addr);
    return 0;
}

DWORD 
WhichOS( DWORD *check)
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_WhichOS )
        return p_WhichOS(check);
    return 0;
}
int __stdcall 
wsh_gethostname(char* name, int size)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_wsh_gethostname )
        return p_wsh_gethostname(name,size);

    return -1;
}

int __stdcall 
wsh_getdomainname(char* name, int size)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_wsh_getdomainname )
        return p_wsh_getdomainname(name,size);
    return -1;
        
}
LONG WSHGetHostID()
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_WSHGetHostID )
        return p_WSHGetHostID();
    return 0;

}

/* From resolv.h */

int  __stdcall 
res_init()
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_res_init )
        return p_res_init();
    return 0;
}

void __stdcall 
res_setopts(long opts)
{
    if ( !load_wshelp32() )
        return;

    if ( p_res_setopts )
        p_res_setopts(opts);
    return;
}

long __stdcall 
res_getopts(void)
{
    if ( !load_wshelp32() )
        return 0;

    if ( p_res_getopts )
        return p_res_getopts();
    return 0;
}

int  __stdcall 
res_mkquery(int op, const char  *dname,
             int qclass, int type, 
             const char  *data, int datalen, 
             const struct rrec  *newrr,
             char  *buf, int buflen)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_res_mkquery )
        return p_res_mkquery(op,dname,qclass,type,data,datalen,
                              newrr,buf,buflen);
    return -1;
}

int  __stdcall 
res_send(const char  *msg, int msglen,
          char  *answer, int anslen)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_res_send )
        return p_res_send(msg,msglen,answer,anslen);
    return -1;
}

int  __stdcall 
res_querydomain(const char  *name,
                 const char  *domain, 
                 int qclass, int type, 
                 u_char  *answer, int anslen)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_res_querydomain )
        return p_res_querydomain(name,domain,qclass,type,answer,anslen);
    return -1;
}

int  __stdcall 
res_search(const char  *name,
            int qclass, int type, 
            u_char  *answer, int anslen)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_res_search ) {
        int rc = p_res_search(name,qclass,type,answer,anslen);
        return rc;
    }
    return -1;
}

int  __stdcall 
dn_comp(const u_char  *exp_dn,
         u_char  *comp_dn, 
         int length, u_char  *  *dnptrs, 
         u_char  *  *lastdnptr)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_dn_comp )
        return p_dn_comp(exp_dn,comp_dn,length,dnptrs,lastdnptr);
    return -1;
}

int  __stdcall 
rdn_expand(const u_char  *msg,
            const u_char  *eomorig, 
            const u_char  *comp_dn, 
            u_char  *exp_dn, 
            int length)
{
    if ( !load_wshelp32() )
        return -1;

    if ( p_rdn_expand )
        return p_rdn_expand(msg,eomorig,comp_dn,exp_dn,length);
    return -1;
        
}

/* From heisod.h */

LPSTR __stdcall 
hes_to_bind(LPSTR HesiodName, LPSTR HesiodNameType)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_to_bind )
        return p_hes_to_bind(HesiodName,HesiodNameType);
    return NULL;
}

LPSTR * __stdcall 
hes_resolve(LPSTR HesiodName,LPSTR HesiodNameType)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_resolve )
        return p_hes_resolve(HesiodName,HesiodNameType);
    return NULL;
}

int __stdcall 
hes_error(void)
{
    if ( !load_wshelp32() )
        return HES_ER_UNINIT;

    if ( p_hes_error )
        return p_hes_error();
    return HES_ER_UNINIT;
}

struct hes_postoffice  * __stdcall 
hes_getmailhost(LPSTR user)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_getmailhost )
        return p_hes_getmailhost(user);
    return NULL;
}

struct servent  * __stdcall 
hes_getservbyname(LPSTR name, LPSTR proto)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_getservbyname )
        return p_hes_getservbyname(name,proto);
    return NULL;
}

struct passwd  * __stdcall 
hes_getpwnam(LPSTR nam)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_getpwnam )
        return p_hes_getpwnam(nam);
    return NULL;
}

struct passwd  * __stdcall 
hes_getpwuid(int uid)
{
    if ( !load_wshelp32() )
        return NULL;

    if ( p_hes_getpwuid )
        return p_hes_getpwuid(uid);
    return NULL;
}

