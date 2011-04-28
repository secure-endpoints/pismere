/*
** $Id: namedcache.cpp,v 1.3 1999/06/08 09:56:08 dalmeida Exp $
**
** Copyright 1997 by the Regents of the University of Michigan
**
** For copying and distribution information, please see the file
** <UMCopyRt.h>.
**
*/
/*
** NamedCache.cpp: implementation of the CNamedCache class
** which is used as an interface to a NamedCache pointed to by m_pNC.
** We use a ptr so the data can reside in shared memory (a memory mapped file).
**
*/
//////////////////////////////////////////////////////////////////////

#include "UmCopyRt.h"
#include "Creds.h"
#include <crtdbg.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Initialize new NC
CNamedCache::CNamedCache(CCache_ctx* pCtx, _NC& pNC, const char* name, const char* principal, cc_int32 vers, cc_nc_flags flags)
	{
	m_pCtx = pCtx;
	m_pNC = &pNC;
	strncpy(m_pNC->CacheName, name, MAX_NC_NAME_SZ);
        m_pNC->Principal[0] = 0;
	strncpy(m_pNC->Principal, principal, MAX_PRINCIPAL_SZ);
        m_pNC->Principal[MAX_PRINCIPAL_SZ - 1] = 0;
	m_pNC->iFlags = flags;
	m_pNC->credVersion = vers;
	m_pNC->iCredsInNC = 0;
	m_pNC->iMaxCredsInNC = MAX_CREDS_IN_NC;
	m_pNC->bInUse = TRUE;
	}

// Attach to existing NC
CNamedCache::CNamedCache(CCache_ctx* pCtx, _NC& pNC)
	{
	_ASSERT(_CrtIsValidPointer(&pNC, sizeof(_NC), TRUE));
	m_pNC = &pNC;

	m_pCtx = pCtx;
	}

CNamedCache::~CNamedCache()
	{
	if (m_pNC)
		m_pNC = NULL;
	}

/*
** Reinitialize : Sets NC back to "like new" condition
** called by RemoveNamedCache
*/
int CNamedCache::Reinitialize()
	{
	int	rc = 0;
	int	i = 0;
		
	if ((rc = Validate()) != CC_NOERROR)
		return(rc);

	_ASSERTE(m_pNC);

	m_pNC->bInUse = FALSE;

	memset(m_pNC->CacheName, NULL, sizeof(m_pNC->CacheName));
	memset(m_pNC->Principal, NULL, sizeof(m_pNC->Principal));
	m_pNC->iCredsInNC = 0;
	m_pNC->iFlags = 0;

	for (i = 0; i < MAX_CREDS_IN_NC; i++)
		{
		if (m_pNC->CredList[i].bInUse != FALSE)
			{
			if (m_pNC->credVersion == CC_CRED_V5)
				{
				m_pNC->CredList[i].bInUse = FALSE;		//DO FIRST so reader doesn't see partial item
				memset(&m_pNC->CredList[i].tcu, NULL, sizeof(NC_Tkt));
				}
			}
		memset(&m_pNC->CredList[i], 0, sizeof(NC_Tkt));
		}
	m_pNC->credVersion = CC_CRED_VUNKNOWN;

	if ((rc = Validate()) != CC_NOERROR)
		return(rc);

	return(CC_NOERROR);
	}


/*
** Validate
*/
#ifndef _DEBUG
inline int CNamedCache::Validate() { return(CC_NOERROR);};
#else
int CNamedCache::Validate()
	{
	//TODO: Verify a NamedCache
	_ASSERT(m_pNC->iCredsInNC <= MAX_CREDS_IN_NC);
	_ASSERT(m_pNC->iCredsInNC >= 0);
	_ASSERT(m_pNC->credVersion >= CC_CRED_VUNKNOWN);
	_ASSERT(m_pNC->credVersion < CC_CRED_VMAX);
	_ASSERT(m_pNC->bInUse == 0 || m_pNC->bInUse == 1);

	return(CC_NOERROR);
	}
#endif


/*
** StoreCred
*/
int CNamedCache::StoreCred(const cred_union cred)
	{
	int Rc = CC_NOERROR;
	int idx = 0;

	Validate();

	if (m_pNC->iCredsInNC >= m_pNC->iMaxCredsInNC)
		return(CC_ERR_CACHE_FULL);
	
	// we can enforce the one cred type per NC rule
	switch (cred.cred_type)
		{
		case CC_CRED_V4:
			if (CC_CRED_V4 != m_pNC->credVersion)
				return(CC_ERR_CRED_VERSION);
			break;
		case CC_CRED_V5:
			if (CC_CRED_V5 != m_pNC->credVersion)
				return(CC_ERR_CRED_VERSION);
			break;
		default:
			return(CC_ERR_CRED_VERSION);
			break;
		}

	while (idx < m_pNC->iMaxCredsInNC && m_pNC->CredList[idx].bInUse)
		{
		idx++;
		}

	_ASSERT(idx < m_pNC->iMaxCredsInNC);
	_ASSERT(!m_pNC->CredList[idx].bInUse);

	if (CC_CRED_V4 == cred.cred_type)
		Rc = StoreV4Cred(idx, cred.cred.pV4Cred);
	else if (CC_CRED_V5 == cred.cred_type)
		Rc = StoreV5Cred(idx, cred.cred.pV5Cred);
	else
		Rc = CC_ERR_CRED_VERSION;

	Validate();

	if (CC_NOERROR == Rc)
		{
		if (idx >= m_pNC->iCredsInNC)
			m_pNC->iCredsInNC++;
		}
	return(Rc);
	}

/*
** StoreV4Cred
*/
int CNamedCache::StoreV4Cred(const int idx, const V4Cred_type* pCred)
	{

	memset(&m_pNC->CredList[idx], NULL, sizeof(NC_Tkt));
	memcpy(&m_pNC->CredList[idx].tcu, pCred, sizeof(V4Cred_type));
	m_pNC->CredList[idx].bInUse = TRUE;
	
	return(CC_NOERROR);
	}

/*
** StoreV5Cred
*/
int CNamedCache::StoreV5Cred(const int idx, const _cc_creds* cred)
	{

	_ASSERTE(m_pCtx);

	m_pNC->CredList[idx].bInUse = TRUE;
	CopyCredentialToCache(cred, &m_pNC->CredList[idx].tcu.V5Cred);

	return(CC_NOERROR);
	}

/*
** RemoveCred : Remove cred/ticket from Named Cache
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CNamedCache::RemoveCred(const cred_union cred)
	{
	int Rc = CC_NOERROR;

	Validate();

	if (m_pNC->iCredsInNC == 0)
		return(CC_NOERROR);
	
	if (cred.cred_type <= CC_CRED_VUNKNOWN || cred.cred_type >= CC_CRED_VMAX)
		return(CC_ERR_CRED_VERSION);

	if (CC_CRED_V4 == m_pNC->credVersion)
		Rc = RemoveV4Cred(cred.cred.pV4Cred);
	else if (CC_CRED_V5 == m_pNC->credVersion)
		Rc = RemoveV5Cred(cred.cred.pV5Cred);
	else
		Rc = CC_ERR_CRED_VERSION;

	if (CC_NOERROR == Rc)
		m_pNC->iCredsInNC--;
	
	Validate();
	return(Rc);
	}

int CNamedCache::RemoveV4Cred(const V4Cred_type* pCred)
	{
	int	rc = 0;

	_ASSERTE(m_pCtx);
	for (int i = 0; i < m_pNC->iMaxCredsInNC; i++)
		{
		if (0 == memcmp(&m_pNC->CredList[i].tcu.V4Cred, pCred, sizeof(V4Cred_type))) 
			{
			if (rc != CC_NOERROR)
				return(rc);
			m_pNC->CredList[i].bInUse = FALSE;
			memset(&m_pNC->CredList[i].tcu, NULL, sizeof(NC_Tkt));
			return(CC_NOERROR);
			}
		}
	return(CC_NOTFOUND);
	}

int CNamedCache::RemoveV5Cred(const cc_creds * pCred)
	{
	int		rc;
	int		i;

	_ASSERTE(m_pCtx);

	rc = CC_NOTFOUND;
	for (i = 0; i < m_pNC->iMaxCredsInNC; i++)
		{
		if (m_pNC->CredList[i].bInUse == TRUE)
			{
			if (!memcmp(m_pNC->CredList[i].tcu.V5Cred.server, pCred->server, strlen(pCred->server)))
				{
				m_pNC->CredList[i].bInUse = FALSE;		//DO FIRST so reader doesn't see partial item
//				FreeCredential(&m_pNC->CredList[i].tcu.V5Cred);
				memset(&m_pNC->CredList[i].tcu, NULL, sizeof(NC_Tkt));
				rc = CC_NOERROR;
				}
			}
		}
	return(rc);
	}

/*
** SeqFetchCreds : walk through the list of tickets/creds stored in a named cache
**
** passed:
**		itCreds	set to NULL before first call to start at the beginning
**		cred is expected to be NULL (every time) to make sure we aren't liberating malloced memory
**
** returns 
**		cred that must be freed via cc_free_cred()
**		itCreds, a local "where was I" index held in callers storage, NULL when EOList returned
**
** Revisions:
**		980126 : sgr : modified so CC_END is NOT returned with a valid cred.  This allows callers
**		to loop while ( seq_fetch_creds(...) == CC_NOERROR ) more easily.
*/
// Note: RemoveCredentials may be called between calls to SeqFetchCreds.
//		 This should not affect the behaviour of SeqFetchCreds
int CNamedCache::SeqFetchCreds(cred_union** pCred, int* itCreds)
	{
	Validate();
	int Rc = CC_NOERROR;

	if (m_pNC->iCredsInNC == 0)
		return(CC_END);

	if (*itCreds < 0 || *itCreds >= m_pNC->iMaxCredsInNC)
		return(CC_BAD_PARM);
	
	// skip over empty slots in our discontiguous list
	while (*itCreds < m_pNC->iMaxCredsInNC 
		&& !m_pNC->CredList[*itCreds].bInUse) 
		{
		(*itCreds)++;
		}
	
	if (*itCreds >= m_pNC->iMaxCredsInNC) 
		{
		*itCreds = 0;
		return(CC_END);
		}
	
	_ASSERT(m_pNC->CredList[*itCreds].bInUse);

	*pCred = (cred_union*) malloc(sizeof (cred_union));

	(*pCred)->cred_type = m_pNC->credVersion;

	if (CC_CRED_V4 == m_pNC->credVersion) 
		{
		(**pCred).cred.pV4Cred = NULL;
		Rc = CopyV4Cred(*itCreds, &((**pCred).cred.pV4Cred));
		} 
	else if (CC_CRED_V5 == m_pNC->credVersion) 
		{
		(**pCred).cred.pV5Cred = NULL;
		(*pCred)->cred_type = CC_CRED_V5;
		Rc = CopyV5Cred(*itCreds, &((**pCred).cred.pV5Cred));
		} 
	else 
		{
		Rc = CC_ERR_CRED_VERSION;
		}

	(*itCreds)++;
	Validate();
	return(Rc);
	}

int CNamedCache::CopyV4Cred(const int idx, V4Cred_type** pCred)
	{

	if (NULL != *pCred)
		return(CC_BAD_PARM);

	if ((*pCred = (V4Cred_type*) calloc(1, sizeof(V4Cred_type))) == NULL)
		return(CC_NOMEM);

	memcpy(*pCred, &(m_pNC->CredList[idx].tcu.V4Cred), sizeof(V4Cred_type));
	
	return(CC_NOERROR);
	}

int CNamedCache::CopyV5Cred(const int idx, cc_creds** pCred)
	{

	if (NULL != *pCred)
		return(CC_BAD_PARM);

	if ((*pCred = (cc_creds*) calloc(1, sizeof(cc_creds))) == NULL)
		return(CC_NOMEM);
	if (m_pNC->CredList[idx].bInUse == TRUE)
		CopyCredentialFromCache((const _cc_cache_creds*)&m_pNC->CredList[idx].tcu.V5Cred, *pCred);

	return(CC_NOERROR);
	}

int CNamedCache::CopyCredentialToCache(const cc_creds *src, cc_cache_creds* dest)
	{
	cc_data	**dataPtr;
	int		counter;
	int		i;

	strcpy(dest->client, src->client);
	strcpy(dest->server, src->server);
	dest->authtime = src->authtime;
	dest->starttime = src->starttime;
	dest->endtime = src->endtime;
	dest->renew_till = src->renew_till;
	dest->is_skey = src->is_skey;
	dest->ticket_flags = src->ticket_flags;

	dataPtr = (cc_data **)src->addresses;
	counter = 0;
	while (*dataPtr != 0)
		{
		++counter;
		++dataPtr;
		}
	++counter;
	dataPtr = src->addresses;
	for (i = 0; i < counter; i++)
		{
		if (*dataPtr == NULL)
			{
			break;
			}
		dest->addresses[i].type = (*dataPtr)->type;
		dest->addresses[i].length = (*dataPtr)->length;
		memcpy((dest)->addresses[i].data, (*dataPtr)->data, (*dataPtr)->length);
		++dataPtr;
		}

	memset(dest->ticket.data, '\0', sizeof(dest->ticket.data));
	dest->ticket.type = src->ticket.type;
	dest->ticket.length = src->ticket.length;
	memcpy(dest->ticket.data, src->ticket.data, src->ticket.length);

	memset(dest->keyblock.data, '\0', sizeof(dest->keyblock.data));
	dest->keyblock.type = src->keyblock.type;
	dest->keyblock.length = src->keyblock.length;
	if (src->keyblock.length != 0)
		{
		memcpy(dest->keyblock.data, src->keyblock.data, src->keyblock.length);
		}

	memset(dest->second_ticket.data, '\0', sizeof(dest->second_ticket.data));
	dest->second_ticket.type = src->second_ticket.type;
	dest->second_ticket.length = src->second_ticket.length;
	if (src->second_ticket.length != 0)
		{
		memcpy(dest->second_ticket.data, src->second_ticket.data, src->second_ticket.length);
		}

	if (src->authdata != NULL)
		{
		dataPtr = (cc_data **)src->authdata;
		counter = 0;
		while (*dataPtr != 0)
			{
			++counter;
			++dataPtr;
			}
		++counter;
		dataPtr = src->authdata;
		for (i = 0; i < counter; i++)
			{
			if (*dataPtr == NULL)
				{
				break;
				}
			dest->authdata[i].type = (*dataPtr)->type;
			dest->authdata[i].length = (*dataPtr)->length;
			memcpy(dest->authdata[i].data, (*dataPtr)->data, (*dataPtr)->length);
			++dataPtr;
			}
		}

	return(CC_NOERROR);
	}

int CNamedCache::CopyCredentialFromCache(const cc_cache_creds *src, cc_creds* dest)
	{
	cc_data	**dataPtr;
	int		counter;
	int		i;

	dest->client = (char *)calloc(1, strlen(src->client) + 1);
	strcpy(dest->client, src->client);
	dest->server = (char *)calloc(1, strlen(src->server) + 1);
	strcpy(dest->server, src->server);
	dest->authtime = src->authtime;
	dest->starttime = src->starttime;
	dest->endtime = src->endtime;
	dest->renew_till = src->renew_till;
	dest->is_skey = src->is_skey;
	dest->ticket_flags = src->ticket_flags;

	counter = 0;
	for (i = 0; i < KRB5_DATA_CNT; i++)
		{
		if (src->addresses[i].length == 0)
			break;
		++counter;
		}
	++counter;
	dest->addresses = (cc_data **)calloc(1, sizeof(dataPtr) * counter);

	dataPtr = dest->addresses;
	for (i = 0; i < counter; i++)
		{
		if (src->addresses[i].length == 0)
			{
			(*dataPtr) = NULL;
			break;
			}
		(*dataPtr) = (cc_data *)calloc(1, sizeof(cc_data));
		(*dataPtr)->type = src->addresses[i].type;
		(*dataPtr)->length = src->addresses[i].length;
		(*dataPtr)->data = (unsigned char *)calloc(1, src->addresses[i].length);
		memcpy((*dataPtr)->data, src->addresses[i].data, src->addresses[i].length);
		++dataPtr;
		}

	dest->ticket.type = src->ticket.type;
	dest->ticket.length = src->ticket.length;
	dest->ticket.data = (unsigned char *)calloc(1, src->ticket.length);
	memcpy(dest->ticket.data, src->ticket.data, src->ticket.length);

	dest->keyblock.type = src->keyblock.type;
	dest->keyblock.length = src->keyblock.length;
	if (src->keyblock.data == NULL)
		dest->keyblock.data = NULL;
	else
		{
		dest->keyblock.data = (unsigned char *)calloc(1, src->keyblock.length);
		memcpy(dest->keyblock.data, src->keyblock.data, src->keyblock.length);
		}

	dest->second_ticket.type = src->second_ticket.type;
	dest->second_ticket.length = src->second_ticket.length;
	if (src->second_ticket.length == 0)
		dest->second_ticket.data = NULL;
	else
		{
		dest->second_ticket.data = (unsigned char *)calloc(1, src->second_ticket.length);
		memcpy(dest->second_ticket.data, src->second_ticket.data, src->second_ticket.length);
		}

	if (src->authdata != NULL)
		{
		counter = 0;
		for (i = 0; i < KRB5_DATA_CNT; i++)
			{
			if (src->authdata[i].length == 0)
				break;
			++counter;
			}
		if (counter == 0)
			return(CC_NOERROR);
		++counter;
		dest->authdata = (cc_data **)calloc(1, sizeof(dataPtr) * counter);
		dataPtr = dest->authdata;
		for (i = 0; i < counter; i++)
			{
			if (src->authdata[i].length == 0)
				{
				(*dataPtr) = NULL;
				break;
				}
			(*dataPtr) = (cc_data *)calloc(1, sizeof(cc_data));
			(*dataPtr)->type = src->authdata[i].type;
			(*dataPtr)->length = src->authdata[i].length;
			(*dataPtr)->data = (unsigned char *)calloc(1, src->authdata[i].length);
			memcpy((*dataPtr)->data, src->addresses[i].data, src->authdata[i].length);
			++dataPtr;
			}
		}
	return(CC_NOERROR);
	}

int CNamedCache::FreeCredential(cc_creds *cred)
	{
	cc_data	**dataPtr;

	if (cred->client != NULL)
		{
		free(cred->client);
		cred->client = NULL;
		}
	if (cred->server != NULL)
		{
		free(cred->server);
		cred->server = NULL;
		}
	if (cred->keyblock.data != NULL)
		{
		free(cred->keyblock.data);
		cred->keyblock.data = NULL;
		}
	if (cred->ticket.data != NULL)
		{
		free(cred->ticket.data);
		cred->ticket.data = NULL;
		}
	if (cred->second_ticket.data != NULL)
		{
		free(cred->second_ticket.data);
		cred->second_ticket.data = NULL;
		}

	if (cred->addresses != NULL)
		{
		dataPtr = (cc_data **)cred->addresses;
		while (*dataPtr != NULL)
			{
			if ((*dataPtr)->data != NULL)
				{
				free((*dataPtr)->data);
				(*dataPtr)->data = NULL;
				}
			free(*dataPtr);
			*dataPtr = NULL;
			++dataPtr;
			}
		free(cred->addresses);
		cred->addresses = NULL;
		}
	if (cred->authdata != NULL)
		{
		dataPtr = (cc_data **)cred->authdata;
		while (*dataPtr != 0)
			{
			if ((*dataPtr)->data != NULL)
				{
				free((*dataPtr)->data);
				(*dataPtr)->data = NULL;
				}
			free(*dataPtr);
			*dataPtr = NULL;
			++dataPtr;
			}
		free(cred->authdata);
		cred->authdata = NULL;
		}
	return(CC_NOERROR);
	}
