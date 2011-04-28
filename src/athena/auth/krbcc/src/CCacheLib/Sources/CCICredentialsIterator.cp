#include "CCache.config.h"

#include "CCICredentialsIterator.h"
#include "CCache.debug.h"
#include "CCIPointer.h"
#include "CCICredentials.h"
#include "CCICCache.h"

#include "CCIContext.h"

const	cc_credentials_iterator_f	CCICredentialsIterator::sFunctionTable = {
	CCECredentialsIterator::Release,
	CCECredentialsIterator::Next
};

cc_int32 CCECredentialsIterator::Release (
	cc_credentials_iterator_t		inIterator) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCredentialsIterator	iterator (inIterator);
		
		delete iterator.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCredentialsIterator));
	
	return result;
}

cc_int32 CCECredentialsIterator::Next (
	cc_credentials_iterator_t		inIterator,
	cc_credentials_t*				outCredentials) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCredentialsIterator			iterator (inIterator);
		
		if (iterator -> HasMore ()) {
			StPointer <cc_credentials_t>		newCredentials (outCredentials);
			StCredentials					credentials =
				new CCICredentials (iterator -> Next ());
			newCredentials = credentials;
		} else {
			result = ccIteratorEnd;
		}
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrNoMem)
	            || (result == ccIteratorEnd)
	            || (result == ccErrInvalidCredentialsIterator));
	
	return result;
}

CCICredentialsIterator::CCICredentialsIterator (
	const CCICCache&			inCCache):
	mCCache (inCCache.GetCCache ()) {
	CCICCacheDataCallStub	ccache (mCCache);
	try {
		mCCacheLock = ccache.Lock ();
		ccache.GetCredentialsIDs (mIterationSet);
		mIterator = mIterationSet.begin ();
#if CCache_v2_compat
		mVersion = inCCache.CompatGetVersion ();
#endif
	} catch (...) {
		ccache.Unlock (mCCacheLock);
	}
}

CCICredentialsIterator::~CCICredentialsIterator () {
	try {
		CCICCacheDataCallStub	ccache (mCCache);
		ccache.Unlock (mCCacheLock);
	} catch (...) {
	}
}

bool
CCICredentialsIterator::HasMore () const {
	return mIterator < mIterationSet.end ();
}

CCICredentialsData::UniqueID
CCICredentialsIterator::Next () {
	if (mIterator == mIterationSet.end ()) {
		throw CCIException (ccIteratorEnd);
	}
	return *mIterator++;
}

void CCICredentialsIterator::Validate () {
	CCIMagic <CCICredentialsIterator>::Validate ();

	CCIAssert_ (CCIUniqueInProcess <CCICredentialsIterator>::Valid ());

	CCIAssert_ ((CCIInternal <CCICredentialsIterator, cc_credentials_iterator_d>::Valid ()));
 }

