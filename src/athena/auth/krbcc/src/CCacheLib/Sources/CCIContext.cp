/*
 * CCIContext.c
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCIContext.cp,v 1.4 2000/06/16 21:44:04 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCIContext.h"
#include "CCIException.h"
#include "CCIString.h"
#include "CCICCache.h"
#include "CCIPointer.h"
#include "CCICCacheIterator.h"

#include "CCache.debug.h"
#include "CCacheUtil.h"

static const char	ccapi_vendor[] = "MIT I/S MacDev";
static const char	gInitialDefaultCCacheName[] = "Initial default ccache";

const	cc_context_f	CCIContext::sFunctionTable = {
	CCEContext::Release,
	CCEContext::GetVersion,
	CCEContext::Clone,
	CCEContext::GetChangeTime,
	CCEContext::GetDefaultCCacheName,
	CCEContext::OpenCCache,
	CCEContext::OpenDefaultCCache,
	CCEContext::CreateCCache,
	CCEContext::CreateDefaultCCache,
	CCEContext::CreateNewCCache,
	CCEContext::NewCCacheIterator,
	CCEContext::Lock
};
	
cc_int32 cc_initialize (
	cc_context_t*		outContext,
	cc_int32			inVersion,
	cc_int32*			outSupportedVersion,
	char const**		outVendor) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		if (outVendor != nil) {
			*outVendor = ccapi_vendor;
		}

	
		if ((inVersion != ccapi_version_2) &&
		    (inVersion != ccapi_version_3)) {

			if (outSupportedVersion != nil) {
				*outSupportedVersion = ccapi_version_3;
			}

			throw CCIException (ccErrBadAPIVersion);
		}
		
		if (!CCIValidPointer (outContext))
			throw CCIException (ccErrBadParam);
		
		StContext					context = new CCIContext ();
		StPointer <cc_context_t>	newContext (outContext);
		
		newContext = context;
	} CCIEndSafeTry_ (result, ccErrBadParam)
		
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadAPIVersion)
	            || (result == ccErrBadParam));
	            
	return result;
}

#if PRAGMA_MARK
#pragma mark -
#endif

cc_int32 CCEContext::Clone (
	cc_context_t		inContext,
	cc_context_t*		outContext,
	cc_int32			inVersion,
	cc_int32*			outSupportedVersion,
	char const**		outVendor) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		if (outVendor != nil) {
			StPointer <const char*>	vendor (outVendor);
			vendor = ccapi_vendor;
		}
		
		if ((inVersion != ccapi_version_2) &&
		    (inVersion != ccapi_version_3)) {

			if (outSupportedVersion != nil) {
				*outSupportedVersion = ccapi_version_3;
			}

			throw CCIException (ccErrBadAPIVersion);
		}
		
		StContext					oldContext (inContext);
		StPointer <cc_context_t> 	newContext (outContext);
		StContext 					context = new CCIContext ();
		
		newContext = context;
	} CCIEndSafeTry_ (result, ccErrBadParam)
		
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadAPIVersion)
	            || (result == ccErrBadParam));
	            
	return result;
}	

cc_int32 CCEContext::GetVersion (
	cc_context_t		inContext,
	cc_int32*			outVersion) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StContext				context (inContext);
		StPointer <cc_int32>	version (outVersion);
		
		version = context -> GetAPIVersion ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrBadParam));
	
	return result;
}

cc_int32 CCEContext::Release (
	cc_context_t		inContext) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StContext				context (inContext);
		
		delete context.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext));
	
	return result;
}

cc_int32 CCEContext::GetChangeTime (
	cc_context_t		inContext,
	cc_time_t*			outTime) {

	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext				context (inContext);
		StPointer <cc_time_t>	time (outTime);
		
		time = context -> GetChangeTime ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::GetDefaultCCacheName (
	cc_context_t		inContext,
	cc_string_t*		outName) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_string_t>		newName (outName);
		StString					name =
			new CCIString (context -> GetDefaultCCacheName ());
		
		newName = name;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrNoMem));

	return result;
}

cc_int32 CCEContext::OpenCCache (
	cc_context_t		inContext,
	const char*			inName,
	cc_ccache_t*		outCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_ccache_t>		newCCache (outCCache);
		StCCache					ccache =
			new CCICCache (context -> OpenCCache (inName));
		
		newCCache = ccache;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrBadName)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrCCacheNotFound)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::OpenDefaultCCache (
	cc_context_t		inContext,
	cc_ccache_t*		outCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_ccache_t>		newCCache (outCCache);
		StCCache					ccache =
			new CCICCache (context -> OpenDefaultCCache ());
		
		newCCache = ccache;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrBadName)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrCCacheNotFound)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::CreateCCache (
	cc_context_t		inContext,
	const char*			inName,
	cc_int32			inVersion,
	const char*			inPrincipal,
	cc_ccache_t*		outCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_ccache_t>		newCCache (outCCache);
		StCCache					ccache =
			new CCICCache (context -> CreateCCache (inName, inVersion, inPrincipal));
		
		newCCache = ccache;
	} CCIEndSafeTry_ (result, ccErrBadParam) 
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrBadName)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrBadCredentialsVersion)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::CreateDefaultCCache (
	cc_context_t		inContext,
	cc_int32			inVersion,
	const char*			inPrincipal,
	cc_ccache_t*		outCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_ccache_t>		newCCache (outCCache);
		StCCache					ccache = 
			new CCICCache (context -> CreateDefaultCCache (inVersion, inPrincipal));
		
		newCCache = ccache;
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrBadCredentialsVersion)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::CreateNewCCache (
	cc_context_t		inContext,
	cc_int32			inVersion,
	const char*			inPrincipal,
	cc_ccache_t*		outCCache) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext					context (inContext);
		StPointer <cc_ccache_t>		newCCache (outCCache);
		StCCache					ccache = 
			new CCICCache (context -> CreateNewCCache (inVersion, inPrincipal));
			
		newCCache = ccache;
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrBadCredentialsVersion)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

cc_int32 CCEContext::Lock (
	cc_context_t		inContext,
	cc_uint32			inLockType,
	cc_lock_t*			outLock) {
#pragma message (CCIMessage_Warning_ "Not implemented")
	return ccErrBadParam;
}

cc_int32 CCEContext::NewCCacheIterator (
	cc_context_t			inContext,
	cc_ccache_iterator_t*	outIterator) {
	
	CCIResult	result = ccNoError;

	CCIBeginSafeTry_ {
		StContext								context (inContext);
		StPointer <cc_ccache_iterator_t>		newIterator (outIterator);
		StCCacheIterator						iterator =
			new CCICCacheIterator (*context.Get ());
			
		newIterator = iterator;
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidContext)
	            || (result == ccErrNoMem)
	            || (result == ccErrBadParam));

	return result;
}

#if PRAGMA_MARK
#pragma mark -
#endif

typedef Implementations::ContextDataStub ContextDataStubBase; // MS VC++ 6.0 workaround

CCIContext::CCIContext ():
	ContextDataStubBase (),
	mAPIVersion (ccapi_version_3) {
}

void CCIContext::Validate () {
	CCIMagic <CCIContext>::Validate ();

	CCIAssert_ (CCIUniqueInProcess <CCIContext>::Valid ());
	CCIAssert_ ((CCIInternal <CCIContext, cc_context_d>::Valid ()));
	CCIAssert_ (mAPIVersion == ccapi_version_3);
}

