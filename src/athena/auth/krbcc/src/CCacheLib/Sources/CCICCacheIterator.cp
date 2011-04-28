#include "CCache.config.h"

#include "CCICCacheIterator.h"
#include "CCache.debug.h"
#include "CCIPointer.h"

#include "CCIContext.h"

const	cc_ccache_iterator_f	CCICCacheIterator::sFunctionTable = {
	CCECCacheIterator::Release,
	CCECCacheIterator::Next
};

cc_int32 CCECCacheIterator::Release (
	cc_ccache_iterator_t		inIterator) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCCacheIterator	iterator (inIterator);
		
		delete iterator.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidCCacheIterator));
	
	return result;
}

cc_int32 CCECCacheIterator::Next (
	cc_ccache_iterator_t		inIterator,
	cc_ccache_t*				outCCache) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StCCacheIterator			iterator (inIterator);
		
		if (iterator -> HasMore ()) {
			StPointer <cc_ccache_t>		newCCache (outCCache);
			StCCache					ccache =
				new CCICCache (iterator -> Next ());
			newCCache = ccache;
		} else {
			result = ccIteratorEnd;
		}
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrNoMem)
	            || (result == ccIteratorEnd)
	            || (result == ccErrInvalidCCacheIterator));
	
	return result;
}

CCICCacheIterator::CCICCacheIterator (
	const CCIContext&			inContext):
	mContext (inContext.GetContext ()) {
	CCIContextDataCallStub	context (mContext);
	try {
		mContextLock = context.Lock ();
		context.GetCCacheIDs (mIterationSet);
		mIterator = mIterationSet.begin ();
	} catch (...) {
		context.Unlock (mContextLock);
	}
#if CCache_v2_compat
	CompatResetRepeatCount ();
#endif
}

CCICCacheIterator::~CCICCacheIterator () {
	try {
		CCIContextDataCallStub	context (mContext);
		context.Unlock (mContextLock);
	} catch (...) {
	}
}

bool
CCICCacheIterator::HasMore () const {
	return mIterator < mIterationSet.end ();
}

CCICCacheData::UniqueID
CCICCacheIterator::Next () {
	if (mIterator == mIterationSet.end ()) {
		throw CCIException (ccIteratorEnd);
	}
	
	// Return current item and advance the iterator
	return *mIterator++;
}

CCICCacheData::UniqueID
CCICCacheIterator::Current () {
	if (mIterator == mIterationSet.end ()) {
		throw CCIException (ccIteratorEnd);
	}
	
	// Return current item
	return *mIterator;
}

void CCICCacheIterator::Validate () {
	CCIMagic <CCICCacheIterator>::Validate ();

	CCIAssert_ (CCIUniqueInProcess <CCICCacheIterator>::Valid ());

	CCIAssert_ ((CCIInternal <CCICCacheIterator, cc_ccache_iterator_d>::Valid ()));
 }

