/*
 * CCICache.c
 *
 * Implementation of credentials cache structures for CCache API.
 *
 * Internally, ccaches are kept in a linked list of cci_ccache_data
 * structures. 
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICCache.cp,v 1.5 2000/06/16 21:44:04 dalmeida Exp $
 */
 
#include "CCache.config.h"

#include "CCache.h"
#include "CCICCache.h"
#include "CCIException.h"
#include "CCIString.h"
#include "CCICredentialsIterator.h"
 
#include "CCIPointer.h"

const	cc_ccache_f	CCICCache::sFunctionTable = {
	CCECCache::Release,
	CCECCache::Destroy,
	CCECCache::SetDefault,
	CCECCache::GetCredentialsVersion,
	CCECCache::GetName,
	CCECCache::GetPrincipal,
	CCECCache::SetPrincipal,
	CCECCache::StoreCredentials,
	CCECCache::RemoveCredentials,
	CCECCache::NewCredentialsIterator,
	CCECCache::Move,
	CCECCache::Lock,
	CCECCache::GetLastDefaultTime,
	CCECCache::GetChangeTime
};

cc_int32 CCECCache::Release (
	cc_ccache_t			inCCache) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCCache				ccache (inCCache);
		
		delete ccache.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCCache));
	
	return result;
}

cc_int32 CCECCache::Destroy (
	cc_ccache_t			inCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		ccache -> Destroy ();
		delete ccache.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCCache));

	return result;
}

cc_int32 CCECCache::SetDefault (
	cc_ccache_t			inCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		ccache -> SetDefault ();
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
	            || (result == ccErrInvalidCCache));

	return result;
}

cc_int32 CCECCache::GetCredentialsVersion (
	cc_ccache_t			inCCache,
	cc_int32*			outVersion) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		StPointer <cc_int32>		version (outVersion);
		version = ccache -> GetCredentialsVersion ();
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrBadParam)
	            || (result == ccErrInvalidCCache));

	return result;
}
	
cc_int32 CCECCache::GetPrincipal (
	cc_ccache_t			inCCache,
	cc_uint32			inVersion,
	cc_string_t*		outPrincipal) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		StPointer <cc_string_t>		newPrincipal (outPrincipal);
		StString					principal =
			new CCIString (ccache -> GetPrincipal (inVersion));
		
		newPrincipal = principal;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrNoMem)
				|| (result == ccErrBadParam)
				|| (result == ccErrBadCredentialsVersion));

	return result;
}

cc_int32 CCECCache::SetPrincipal (
	cc_ccache_t			inCCache,
	cc_uint32			inVersion,
	const char*			inPrincipal) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		ccache -> SetPrincipal (inVersion, inPrincipal);

	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrNoMem)
				|| (result == ccErrBadParam));

	return result;
}

cc_int32 CCECCache::GetName (
	cc_ccache_t			inCCache,
	cc_string_t*		outName) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		StPointer <cc_string_t>		newName (outName);
		StString					name =
			new CCIString (ccache -> GetName ());
		
		newName = name;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrInvalidCCache)
				|| (result == ccErrNoMem)
				|| (result == ccErrBadParam)
				|| (result == ccErrBadCredentialsVersion));

	return result;
}

cc_int32 CCECCache::StoreCredentials (
	cc_ccache_t					inCCache,
	const cc_credentials_union*	inCreds) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		ccache -> StoreCredentials (inCreds);
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrInvalidCCache)
				|| (result == ccErrNoMem)
				|| (result == ccErrBadParam)
				|| (result == ccErrBadCredentialsVersion));

	return result;
}

cc_int32 CCECCache::RemoveCredentials (
	cc_ccache_t					inCCache,
	cc_credentials_t			inCreds) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache					ccache (inCCache);
		StCredentials				credentials (inCreds);
		ccache -> RemoveCredentials (*(credentials.Get ()));
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrInvalidCCache)
				|| (result == ccErrInvalidCredentials)
				|| (result == ccErrCredentialsNotFound)
				|| (result == ccErrBadParam));

	return result;
}

cc_int32 CCECCache::NewCredentialsIterator (
	cc_ccache_t					inCCache,
	cc_credentials_iterator_t*	outIterator) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache								ccache (inCCache);
		StPointer <cc_credentials_iterator_t>	newIterator (outIterator);
		StCredentialsIterator					iterator =
			new CCICredentialsIterator (*ccache.Get ());
			
		newIterator = iterator;
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCCache)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCECCache::GetLastDefaultTime (
	cc_ccache_t				inCCache,
	cc_time_t*				outTime) {

	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache				ccache (inCCache);
		StPointer <cc_time_t>	time (outTime);
		
		time = ccache -> GetLastDefaultTime ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
				|| (result == ccErrNeverDefault)
	            || (result == ccErrInvalidCCache)
	            || (result == ccErrBadParam));

	return result;
}
	
cc_int32 CCECCache::GetChangeTime (
	cc_ccache_t				inCCache,
	cc_time_t*				outTime) {

	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache				ccache (inCCache);
		StPointer <cc_time_t>	time (outTime);
		
		time = ccache -> GetChangeTime ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
	            || (result == ccErrInvalidCCache)
	            || (result == ccErrBadParam));

	return result;
}
	
cc_int32 CCECCache::Lock (
	cc_ccache_t			inContext,
	cc_uint32			inLockType,
	cc_lock_t*			outLock) {
#pragma message (CCIMessage_Warning_ "Not implemented")
	return ccErrBadParam;
}

cc_int32 CCECCache::Move (
	cc_ccache_t				inSource,
	cc_ccache_t				inDestination) {

	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StCCache				source (inSource);
		StCCache				destination (inDestination);
		
		source -> Move (*(destination.Get ()));
		delete source.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
				|| (result == ccErrCCacheNotFound)
	            || (result == ccErrInvalidCCache));

	return result;
}

typedef Implementations::CCacheDataStub		CCacheDataStubBase;		// VC++ 6.0 workaround

CCICCache::CCICCache (
	CCICCacheData::UniqueID		inCCache):
#if CCache_v2_compat
	mVersion (cc_credentials_v4_v5), // By default, both v4 and v5
#endif
	CCacheDataStubBase (inCCache) {
}

#if CCache_v2_compat
void
CCICCache::CompatSetVersion (
	CCIUInt32					inVersion) {
	
	if ((inVersion != cc_credentials_v4) && (inVersion != cc_credentials_v5)) {
		throw CCIException (ccErrBadCredentialsVersion);
	}
	
	mVersion = inVersion;
}
#endif

void CCICCache::Validate () {

	CCIMagic <CCICCache>::Validate ();
	CCIAssert_ (CCIUniqueInProcess <CCICCache>::Valid ());
	
	CCIAssert_ ((CCIInternal <CCICCache, cc_ccache_d>::Valid ()));
}

