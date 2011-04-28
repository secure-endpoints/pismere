/*
 * CCICache.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICCache.h,v 1.4 2000/06/16 21:44:06 dalmeida Exp $
 */
 
#include "CCache.h"

#include "CCache.Debug.h"

#include "CCICCacheDataStubs.h"
#include "CCIContextDataStubs.h"
#include "CCICredentials.h"

#include "CCIInternalize.h"
#include "CCIInternal.h"
#include "CCIUniqueInProcess.h"
#include "CCIMagic.h"
#include "CCILeaks.h"
#include "CCIImplementations.h"

#pragma once

class CCECCache {
	public:
		static cc_int32 Release (
			cc_ccache_t				inCCache);
			
		static cc_int32 SetDefault (
			cc_ccache_t				inCCache);
			
		static cc_int32 GetCredentialsVersion (
			cc_ccache_t				inCCache,
			cc_int32*				outVersion);
			
		static cc_int32 GetName (
			cc_ccache_t				inCCache,
			cc_string_t*			outName);
			
		static cc_int32 GetPrincipal (
			cc_ccache_t				inCCache,
			cc_uint32				inVersion,
			cc_string_t*			outPrincipal);
			
		static cc_int32 SetPrincipal (
			cc_ccache_t				inCCache,
			cc_uint32				inVersion,
			const char*				inPrincipal);
			
		static cc_int32 StoreCredentials (
			cc_ccache_t					inCCache,
			const cc_credentials_union*	inCreds);
			
		static cc_int32 RemoveCredentials (
			cc_ccache_t					inCCache,
			cc_credentials_t			inCreds);

		static cc_int32 Destroy (
			cc_ccache_t				inCCache);
			
		static cc_int32 NewCredentialsIterator (
			cc_ccache_t					inCCache,
			cc_credentials_iterator_t*	outIter);

		static cc_int32 GetLastDefaultTime (
			cc_ccache_t				inCCache,
			cc_time_t*				outTime);
			
		static cc_int32 GetChangeTime (
			cc_ccache_t				inCCache,
			cc_time_t*				outTime);
			
		static cc_int32 Lock (
			cc_ccache_t				inCCache,
			cc_uint32				inLockType,
			cc_lock_t*				outLock);
			
		static cc_int32 Move (
			cc_ccache_t				inSource,
			cc_ccache_t				inDestination);
			
	private:
		// Disallowed
		CCECCache ();
		CCECCache (const CCECCache&);
		CCECCache& operator = (const CCECCache&);
};
		

class CCICCache:
	public CCIUniqueInProcess <CCICCache>,
	public CCIMagic <CCICCache>,
	public CCILeaks <CCICCache>,
	public CCIInternal <CCICCache, cc_ccache_d>,
	public Implementations::CCacheDataStub {

	public:
		CCICCache (
			CCICCacheData::UniqueID		inCCache);

#if CCache_v2_compat
		void
			CompatSetVersion (
				CCIUInt32				inVersion);

		CCIUInt32
			CompatGetVersion () const { return mVersion; }
#endif
			
		enum {
			class_ID = FOUR_CHAR_CODE ('CChe'),
			invalidObject = ccErrInvalidCCache
		};
		
	private:
#if CCache_v2_compat
		CCIUInt32					mVersion;
#endif
		
		void		Validate ();

		static const	cc_ccache_f	sFunctionTable;

		friend class StInternalize <CCICCache, cc_ccache_d>;
		friend class CCIInternal <CCICCache, cc_ccache_d>;

		// Disallowed
		CCICCache (const CCICCache&);
		CCICCache& operator = (const CCICCache&);
};

typedef StInternalize <CCICCache, cc_ccache_d>	StCCache;

