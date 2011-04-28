//
// tktv4.cpp: implementation of the V4 Ticket interface.
//
//////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include "cacheapi.h"
#include "tktv4.h"
#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// Create & Destroy
//////////////////////////////////////////////////////////////////////

V4Cred_type* v4CreateTkt()
{
    return (V4Cred_type*)(calloc(1, sizeof(V4Cred_type)));
}
link();
int v4DestroyTkt(V4Cred_type* pTkt)
{
    free(pTkt); pTkt = NULL;
    return CC_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Set functions 
//////////////////////////////////////////////////////////////////////


int v4Set_kversion(V4Cred_type* pTkt, BYTE kversion)
{
    pTkt->kversion = kversion;
    return CC_NOERROR;
}

int v4Set_principal(V4Cred_type* pTkt, const char* principal)
{
    strncpy(pTkt->principal, principal, KRB_NAME_SZ);
    return CC_NOERROR;
}

int v4Set_principal_instance(V4Cred_type* pTkt, const char* principal_instance)
{
    strncpy(pTkt->principal_instance, principal_instance, KRB_INSTANCE_SZ);
    return CC_NOERROR;
}

int v4Set_service(V4Cred_type* pTkt, const char* service)
{
    strncpy(pTkt->service, service, KRB_NAME_SZ);
    return CC_NOERROR;
}

int v4Set_service_instance(V4Cred_type* pTkt, const char* service_instance)
{
    strncpy(pTkt->service_instance, service_instance, KRB_INSTANCE_SZ);
    return CC_NOERROR;
}

int v4Set_realm(V4Cred_type* pTkt, const char* realm)
{
    strncpy(pTkt->realm, realm, KRB_REALM_SZ);
    return CC_NOERROR;
}

int v4Set_session_key(V4Cred_type* pTkt, const unsigned char* session_key)
{
    strncpy((char*)pTkt->session_key, (char*)session_key, 8);
    return CC_NOERROR;
}

int v4Set_kvno(V4Cred_type* pTkt, const cc_int32 kvno)
{
    pTkt->kvno = kvno;
    return CC_NOERROR;
}

int v4Set_str_to_key(V4Cred_type* pTkt, const cc_int32 str_to_key)
{
    pTkt->str_to_key = str_to_key;
    return CC_NOERROR;
}

int v4Set_issue_date(V4Cred_type* pTkt, const long issue_date)
{
    pTkt->issue_date = issue_date;
    return CC_NOERROR;
}

int v4Set_lifetime(V4Cred_type* pTkt, const cc_int32 lifetime)
{
    pTkt->lifetime = lifetime;
    return CC_NOERROR;
}

int v4Set_address(V4Cred_type* pTkt, const char* address)
{
    struct in_addr LocAddr;

    memset(&LocAddr, '\0', sizeof(LocAddr));
    LocAddr.s_addr = inet_addr(address);
    memcpy(&pTkt->address,(unsigned char *)&LocAddr, 4);
    return CC_NOERROR;
}

int v4Set_ticket(V4Cred_type* pTkt, const unsigned char* ticket)
{
    memcpy(pTkt->ticket, ticket, sizeof(ticket));
    return CC_NOERROR;
}



//////////////////////////////////////////////////////////////////////
// Get functions
//////////////////////////////////////////////////////////////////////

unsigned char* v4Get_ticket(V4Cred_type* pTkt) 
{
    return (unsigned char*)(pTkt->ticket);
}

char* v4Get_address(V4Cred_type* pTkt)
{
    struct in_addr LocAddr;

    memset(&LocAddr, '\0', sizeof(LocAddr));
    memcpy(&LocAddr, &pTkt->address, 4);
    return(inet_ntoa(LocAddr));
}

cc_int32 v4Get_lifetime(V4Cred_type* pTkt)
{
    return pTkt->lifetime;
}

long v4Get_issue_date(V4Cred_type* pTkt)
{
    return pTkt->issue_date;
}

cc_int32 v4Get_str_to_key(V4Cred_type* pTkt)
{
    return pTkt->str_to_key;
}

cc_int32 v4Get_kvno(V4Cred_type* pTkt)
{
    return pTkt->kvno;
}

unsigned char* v4Get_session_key(V4Cred_type* pTkt)
{
    return (unsigned char*)(pTkt->session_key);
}

char* v4Get_realm(V4Cred_type* pTkt)
{
    return (char*)(pTkt->realm);
}

char* v4Get_service_instance(V4Cred_type* pTkt)
{
    return (char*)(pTkt->service_instance);
}

char* v4Get_service(V4Cred_type* pTkt)
{
    return (char*)(pTkt->service);
}

char* v4Get_principal_instance(V4Cred_type* pTkt)
{
    return (char*)(pTkt->principal_instance);
}

char* v4Get_principal(V4Cred_type* pTkt)
{
    return (char*)(pTkt->principal);
}

BYTE v4Get_kversion(V4Cred_type* pTkt)
{
    return pTkt->kversion;
}

