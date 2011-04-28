/*
 * CCICCacheData.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICCacheData.cp,v 1.5 2000/06/18 06:18:08 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCache.h"
#include "CCICCacheData.h"
#include "CCIException.h"
#include "CCIContextData.h"
#include "CCacheUtil.h"

#include <algorithm>
#include <functional>

CCICCacheData::CCICCacheData () {
}

CCICCacheData::CCICCacheData (
			CCIContextData*		inContext,
	const	std::string&		inName,
	CCIInt32					inVersion,
	const	std::string&		inPrincipal):
	
	mContext (inContext),
	mName (inName.c_str ()),
	mHasBeenDefault (false),
	mHavePrincipalV4 (false),
	mHavePrincipalV5 (false) {
	
	if (inVersion == cc_credentials_v4) {
		mHavePrincipalV4 = true;
		mPrincipalV4 = inPrincipal.c_str ();
	} else if (inVersion == cc_credentials_v5) {
		mHavePrincipalV5 = true;
		mPrincipalV5 = inPrincipal.c_str ();
	} else {
		throw CCIException (ccErrBadCredentialsVersion);
	}
	
	Changed ();
}

CCICCacheData::~CCICCacheData () {
	mContext -> RemoveCCache (*this);
}

void
CCICCacheData::Changed () {
	StWriteLock	lock;
	
	mChangeTime = CCICurrentTime ();
	mContext -> Changed ();
}

void
CCICCacheData::Destroy () {
	delete this;
}

void
CCICCacheData::SetDefault () {
	mContext -> SetDefault (*this);
}

CCIUInt32
CCICCacheData::GetCredentialsVersion () const {
	StReadLock	lock;
	
	CCIUInt32	result = 0;
	if (mHavePrincipalV4)
		result |= cc_credentials_v4;
	if (mHavePrincipalV5)
		result |= cc_credentials_v5;
	return result;
}

std::string
CCICCacheData::GetName () const {
	StReadLock	lock;
	
	return mName.c_str ();
}

std::string
CCICCacheData::GetPrincipal (
	CCIInt32		inVersion) const {
	StReadLock	lock;
	
	if (inVersion == cc_credentials_v4) {
		if (mHavePrincipalV4)
			return mPrincipalV4.c_str ();
	} else if (inVersion == cc_credentials_v5) {
		if (mHavePrincipalV5)
			return mPrincipalV5.c_str ();
	}

	throw CCIException (ccErrBadCredentialsVersion);
}


CCITime
CCICCacheData::GetLastDefaultTime () const {
	StReadLock	lock;
	
	if (mHasBeenDefault)
		return mLastDefaultTime;
	
	throw CCIException (ccErrNeverDefault);
}

CCITime
CCICCacheData::GetChangeTime () const {
	StReadLock	lock;
	
	return mChangeTime;
}

void
CCICCacheData::StoreCredentials (
	const	cc_credentials_union*	inCredentials) {
	
	StWriteLock	lock;
	
	mCredentials.push_back (new CCICredentialsData (inCredentials));
	
	Changed ();
}

#if CCache_v2_compat
void
CCICCacheData::CompatStoreCredentials (
	const	cred_union&				inCredentials) {
	
	StWriteLock	lock;
	
	mCredentials.push_back (new CCICredentialsData (inCredentials));
	
	Changed ();
}
#endif

void
CCICCacheData::SetPrincipal (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
			
	StWriteLock	lock;
	
	if (inVersion == cc_credentials_v4) {
		mPrincipalV4 = inPrincipal.c_str ();
		mHavePrincipalV4 = true;
	} else if (inVersion == cc_credentials_v5) {
		mPrincipalV5 = inPrincipal.c_str ();
		mHavePrincipalV5 = true;
	} else {
		throw CCIException (ccErrBadCredentialsVersion);
	}

	
	CCICredentialsData::VersionMatch 	match (inVersion);
	
	SharedCredentialsVector::iterator	iterator = mCredentials.begin ();
	for (; iterator != mCredentials.end (); iterator++) {
		if (match (*iterator)) {
			delete (*iterator);
			*iterator = nil;
		}
	}

	// Based on eliminate_duplicates() on p.534 of The C++ Programming Language, 3rd ed:
	SharedCredentialsVector::iterator	newEnd = 
		std::remove_if (mCredentials.begin (), mCredentials.end (), std::bind2nd(std::equal_to<CCICredentialsData*>(), nil));
	mCredentials.erase(newEnd, mCredentials.end());

	Changed ();
}

#if CCache_v2_compat
void
CCICCacheData::CompatSetPrincipal (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
			
	StWriteLock	lock;
	
	if (inVersion == cc_credentials_v4) {
		mPrincipalV4 = inPrincipal.c_str ();
		mHavePrincipalV4 = true;
	} else if (inVersion == cc_credentials_v5) {
		mPrincipalV5 = inPrincipal.c_str ();
		mHavePrincipalV5 = true;
	} else {
		throw CCIException (ccErrBadCredentialsVersion);
	}
	
	Changed ();
}
#endif

void
CCICCacheData::RemoveCredentials (
	const CCICredentialsData::UniqueID&		inCredentials) {
	
	StWriteLock	lock;
	
	CCICredentialsData::UniqueIDMatch 	match (inCredentials);
	
	SharedCredentialsVector::iterator	iterator = mCredentials.begin ();
	
	for (; iterator != mCredentials.end (); iterator++) {
		if (match (*iterator)) {
			delete (*iterator);
			*iterator = nil;
			mCredentials.erase (iterator);
			break;
		}
	}
	
	Changed ();
}

void
CCICCacheData::Move (
	const UniqueID&		inDestination) {
	
	CCICCacheData*	destination = CCICCacheData::Resolve (inDestination);
	
	destination -> mContext = mContext;
	std::swap (mHavePrincipalV4, destination -> mHavePrincipalV4);
	mPrincipalV4.swap (destination -> mPrincipalV4);	
	std::swap (mHavePrincipalV5, destination -> mHavePrincipalV5);
	mPrincipalV5.swap (destination -> mPrincipalV5);	
	mCredentials.swap (destination -> mCredentials);
	
	destination -> Changed ();
	
	Destroy ();
}

CCILockID 
CCICCacheData::Lock () {
#pragma message (CCIMessage_Warning_ "unimplemented")
	return 0;
}

void
CCICCacheData::Unlock (
	CCILockID			inLock) {
#pragma message (CCIMessage_Warning_ "unimplemented")
}

void			
CCICCacheData::GetCredentialsIDs (
	std::vector <CCICredentialsData::UniqueID>&	outCredentialsIDs) const {
	
	StReadLock		lock;
	
	outCredentialsIDs.reserve (mCredentials.size ());
	outCredentialsIDs.clear ();
	SharedCredentialsVector::const_iterator		iterator = mCredentials.begin ();
	for (; iterator != mCredentials.end (); iterator++) {
		outCredentialsIDs.push_back ((*iterator) -> GetGloballyUniqueID ());
	}
}

