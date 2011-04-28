/*
 * CCICredentials.cp
 *
 * Implementation of credentials cache structures for CCache API.
 *
 * Internally, ccaches are kept in a linked list of cci_ccache_data
 * structures. 
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICredentials.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 */
 
#include "CCache.config.h"

#include "CCICredentials.h"
#include "CCache.debug.h"

const	cc_credentials_f	CCICredentials::sFunctionTable = {
	CCECredentials::Release
};

cc_int32 CCECredentials::Release (
	cc_credentials_t			inCredentials) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCredentials			credentials (inCredentials);
		
		delete credentials.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCredentials));
	
	return result;
}

typedef Implementations::CredentialsDataStub CredentialsDataStubBase; // MS VC++ 6.0 workaround

CCICredentials::CCICredentials (
	const CCICredentialsData::UniqueID&	inCredentials):
	CredentialsDataStubBase (inCredentials) {
	
	GetPublicData ().data = &mCredentials;
	CCIAssert_ ((GetCredentialsVersion () == cc_credentials_v4) ||
		(GetCredentialsVersion () == cc_credentials_v5));
	if (GetCredentialsVersion () == cc_credentials_v4) {
		mCredentials.version = cc_credentials_v4;
		mCredentials.credentials.credentials_v4 =
			new CCICredentialsV4 (inCredentials);
	} else {
		mCredentials.version = cc_credentials_v5;
		mCredentials.credentials.credentials_v5 = 
			new CCICredentialsV5 (inCredentials);
	}
}

CCICredentials::~CCICredentials () {
	if (mCredentials.version == cc_credentials_v4) {
		delete static_cast <CCICredentialsV4*> (mCredentials.credentials.credentials_v4);
	}
	if (mCredentials.version == cc_credentials_v5) {
		delete static_cast <CCICredentialsV5*> (mCredentials.credentials.credentials_v5);
	}
}

void CCICredentials::Validate () {

	CCIMagic <CCICredentials>::Validate ();
	CCIAssert_ (CCIUniqueInProcess <CCICredentials>::Valid ());
	
	CCIAssert_ ((CCIInternal <CCICredentials, cc_credentials_d>::Valid ()));
}

CCICredentialsV4::CCICredentialsV4 (
	const CCICredentialsData::UniqueID&		inCredentials) {
	
	CCICredentialsDataCallStub			credentials (inCredentials);
	
	CCIAssert_ (credentials.GetCredentialsVersion () == cc_credentials_v4);
	
	credentials.CopyV4Credentials (*this);
}

CCICredentialsV4::~CCICredentialsV4 () {
}

CCICredentialsV5::CCICredentialsV5 (
	const CCICredentialsData::UniqueID&		inCredentials) {

	client = nil;
	server = nil;
	keyblock.data = nil;
	ticket.data = nil;
	second_ticket.data = nil;
	addresses = nil;
	authdata = nil;

	try {
		CCICredentialsDataCallStub			credentials (inCredentials);
		
		CCIAssert_ (credentials.GetCredentialsVersion () == cc_credentials_v5);
		
		credentials.CopyV5Credentials (*this);
	} catch (...) {
		Cleanup ();
		throw;
	}
}

CCICredentialsV5::~CCICredentialsV5 () {
	Cleanup ();
}

void
CCICredentialsV5::Cleanup () {
	delete [] client;
	delete [] server;
	delete [] keyblock.data;
	delete [] ticket.data;
	delete [] second_ticket.data;
	if (addresses != nil) {
		for (cc_data**	walker = addresses; *walker != nil; walker++) {
			delete [] (*walker) -> data;
			delete (*walker);
		}
	}
	delete [] addresses;
	if (authdata != nil) {
		for (cc_data**	walker = authdata; *walker != nil; walker++) {
			delete [] (*walker) -> data;
			delete (*walker);
		}
	}
	delete [] authdata;
}
	
typedef Implementations::CredentialsDataStub CredentialsDataStubBase;

#if CCache_v2_compat
CCICompatCredentials::CCICompatCredentials (
	const CCICredentialsData::UniqueID&	inCredentials):
	CredentialsDataStubBase (inCredentials) {
	
	CCIAssert_ ((GetCredentialsVersion () == cc_credentials_v4) ||
		(GetCredentialsVersion () == cc_credentials_v5));
	if (GetCredentialsVersion () == cc_credentials_v4) {
		GetPublicData ().cred_type = CC_CRED_V4;
		GetPublicData ().cred.pV4Cred =
			new CCICompatCredentialsV4 (inCredentials);
	} else {
		GetPublicData ().cred_type = CC_CRED_V5;
		GetPublicData ().cred.pV5Cred = 
			new CCICompatCredentialsV5 (inCredentials);
	}
}

CCICompatCredentials::~CCICompatCredentials () {
	if (GetPublicData ().cred_type == CC_CRED_V4) {
		delete static_cast <CCICompatCredentialsV4*> (GetPublicData ().cred.pV4Cred);
	}
	if (GetPublicData ().cred_type == CC_CRED_V5) {
		delete static_cast <CCICompatCredentialsV5*> (GetPublicData ().cred.pV5Cred);
	}
}

void CCICompatCredentials::Validate () {

	CCIMagic <CCICompatCredentials>::Validate ();
	CCIAssert_ (CCIUniqueInProcess <CCICompatCredentials>::Valid ());
}

CCICompatCredentialsV4::CCICompatCredentialsV4 (
	const CCICredentialsData::UniqueID&		inCredentials) {
	
	CCICredentialsDataCallStub			credentials (inCredentials);
	
	CCIAssert_ (credentials.GetCredentialsVersion () == cc_credentials_v4);
	
	credentials.CompatCopyV4Credentials (*this);
}

CCICompatCredentialsV4::~CCICompatCredentialsV4 () {
}

CCICompatCredentialsV5::CCICompatCredentialsV5 (
	const CCICredentialsData::UniqueID&		inCredentials) {
	
	client = nil;
	server = nil;
	keyblock.data = nil;
	ticket.data = nil;
	second_ticket.data = nil;
	addresses = nil;
	authdata = nil;

	CCICredentialsDataCallStub			credentials (inCredentials);
	
	CCIAssert_ (credentials.GetCredentialsVersion () == cc_credentials_v5);
	
	credentials.CompatCopyV5Credentials (*this);
}

CCICompatCredentialsV5::~CCICompatCredentialsV5 () {
	Cleanup ();
}

void
CCICompatCredentialsV5::Cleanup () {
	delete [] client;
	delete [] server;
	delete [] keyblock.data;
	delete [] ticket.data;
	delete [] second_ticket.data;
	if (addresses != nil) {
		for (cc_data**	walker = addresses; *walker != nil; walker++) {
			delete [] (*walker) -> data;
			delete (*walker);
		}
	}
	delete [] addresses;
	if (authdata != nil) {
		for (cc_data**	walker = authdata; *walker != nil; walker++) {
			delete [] (*walker) -> data;
			delete (*walker);
		}
	}
	delete [] authdata;
}
	
#endif