/*
 * CCIContext.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCIContext.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */
 
#include "CCache.h"
#include "CCache.internal.h"

#include "CCIContextDataStubs.h"

#include "CCIInternalize.h"
#include "CCIInternal.h"
#include "CCIUniqueInProcess.h"
#include "CCIMagic.h"
#include "CCILeaks.h"
#include "CCIImplementations.h"

#pragma once

#if PRAGMA_MARK
#pragma mark CCEContext
#endif

//
// CCEContext
//
// Implements static exported functions -- outermost layer of the API
//
 
class CCEContext {
public:
		static cc_int32 Clone (
			cc_context_t		inContext,
			cc_context_t*		outContext,
			cc_int32			inVersion,
			cc_int32*			outSupportedVersion,
			char const**		outVendor);

		static cc_int32 GetVersion (
			cc_context_t		inContext,
			cc_int32*			outVersion);
			
		static cc_int32 GetChangeTime (
			cc_context_t		inContext,
			cc_time_t*			outTime);

		static cc_int32 Release (
			cc_context_t		inContext);

		static cc_int32 GetDefaultCCacheName (
			cc_context_t		inContext,
			cc_string_t*		outName);

		static cc_int32 OpenCCache (
			cc_context_t		inContext,
			const char*			inName,
			cc_ccache_t*		outCCache);

		static cc_int32 OpenDefaultCCache (
			cc_context_t		inContext,
			cc_ccache_t*		outCCache);
			
		static cc_int32 CreateCCache (
			cc_context_t		inContext,
			const char*			inName,
			cc_int32			inVersion,
			const char*			inPrincipal,
			cc_ccache_t*		outCCache);
			
		static cc_int32 CreateDefaultCCache (
			cc_context_t		inContext,
			cc_int32			inVersion,
			const char*			inPrincipal,
			cc_ccache_t*		outCCache);
			
		static cc_int32 CreateNewCCache (
			cc_context_t		inContext,
			cc_int32			inVersion,
			const char*			inPrincipal,
			cc_ccache_t*		outCCache);
			
		static cc_int32 NewCCacheIterator (
			cc_context_t			inContext,
			cc_ccache_iterator_t*	outIter);
			
		static cc_int32 Lock (
			cc_context_t			inContext,
			cc_uint32				inLockType,
			cc_lock_t*				outLock);
			
	private:
		// Disallowed
		CCEContext ();
		CCEContext (const CCEContext&);
		CCEContext& operator = (const CCEContext&);
};

#if PRAGMA_MARK
#pragma mark CCIContext
#endif

//
// CCIContext
//
// Object which implements a reference to a ccache collection
//

class CCIContext:
	public CCIUniqueInProcess <CCIContext>,
	public CCIMagic <CCIContext>,
	public CCILeaks <CCIContext>,
	public CCIInternal <CCIContext, cc_context_d>,
	public Implementations::ContextDataStub {

	public:
		CCIContext ();
		
		CCIInt32	GetAPIVersion () const { return mAPIVersion; }

		enum {
			class_ID = FOUR_CHAR_CODE ('CCTX'),
			invalidObject = ccErrInvalidContext
		};
	
	private:
	
		CCIInt32				mAPIVersion;

		void Validate ();

	 	friend cc_int32 cc_initialize (
			cc_context_t*		outContext,
			cc_int32			inVersion,
			cc_int32*			outSupportedVersion,
			char const**		outVendor);
			
		static const cc_context_f sFunctionTable;
			
		friend class StInternalize <CCIContext, cc_context_d>;
		friend class CCIInternal <CCIContext, cc_context_d>;

		// Disallowed
		CCIContext (const CCIContext&);
		CCIContext& operator = (const CCIContext&);
};

typedef StInternalize <CCIContext, cc_context_d>	StContext;
