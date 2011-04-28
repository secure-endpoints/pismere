/*
 * CCICCacheDataStubs.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICCacheDataCallStubs.h,v 1.4 2000/06/16 21:44:06 dalmeida Exp $
 */

#pragma once

#include <string>

#include "CCache.internal.h"
#include "CCICCacheData.h"
#include "CCICredentialsDataCallStubs.h"

#if CCache_v2_compat
#include "CCache2.h"
#endif

class CCICCacheDataCallStub {
	public:
		CCICCacheDataCallStub (
			CCICCacheData::UniqueID		inCCache);
		~CCICCacheDataCallStub ();
		
		void Destroy ();
		
		void SetDefault ();
		
		CCIUInt32	GetCredentialsVersion ();
		
		std::string	GetPrincipal (
			CCIUInt32				inVersion);
			
		std::string	GetName ();
			
		void		SetPrincipal (
			CCIUInt32				inVersion,
			const std::string&		inPrincipal);
			
#if CCache_v2_compat
		void		CompatSetPrincipal (
			CCIUInt32				inVersion,
			const std::string&		inPrincipal);
#endif
			
		void		StoreCredentials (
			const cc_credentials_union*			inCredentials);
			
#if CCache_v2_compat
		void		CompatStoreCredentials (
			const cred_union&			inCredentials);
#endif			

		void		RemoveCredentials (
			const CCICredentialsDataCallStub&	inCredentials);
			
		CCITime		GetLastDefaultTime ();
		
		CCITime		GetChangeTime ();
		
		void		Move (
			CCICCacheDataCallStub&		inCCache);
			
		CCILockID	Lock ();
		
		void		Unlock (
			CCILockID					inLock);
			
		void		GetCredentialsIDs (
				std::vector <CCICredentialsData::UniqueID>&	outCredenitalsIDs) const;
			
		const CCICCacheData::UniqueID&	GetCCache () const { return mCCache; }
		
	private:
		CCICCacheData::UniqueID		mCCache;

		// Disallowed
		CCICCacheDataCallStub (const CCICCacheDataCallStub&);
		CCICCacheDataCallStub& operator = (const CCICCacheDataCallStub&);
};

class CCICCacheDataCallInterface {
	public:
		CCICCacheDataCallInterface (
			const CCICCacheData::UniqueID&		inCCache);
			
		CCICCacheData* operator -> () { return mCCache; }
		
	private:
		CCICCacheData*	mCCache;

		// Disallowed
		CCICCacheDataCallInterface (const CCICCacheDataCallInterface&);
		CCICCacheDataCallInterface& operator = (const CCICCacheDataCallInterface&);
};

namespace CallImplementations {
	typedef	CCICCacheDataCallStub	CCacheDataStub;
}

