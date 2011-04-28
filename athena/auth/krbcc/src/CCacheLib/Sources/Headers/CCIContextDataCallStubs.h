/*
 * CCIContextDataStubs.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCIContextDataCallStubs.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */

#pragma once

#include <string>

#include "CCICCacheDataCallStubs.h"
#include "CCIContextData.h"
#include "CCache.internal.h"

#include "CCIContextDataStubsCommon.h"
#include "CCISharedStaticData.h"

class CCIContextDataCallStub {
	public:
		CCIContextDataCallStub ();

		CCIContextDataCallStub (
			const CCIContextData::UniqueID&	inContext);

		~CCIContextDataCallStub ();
		
		CCITime			GetChangeTime ();
		
		const CCICCacheData::UniqueID&
			OpenCCache (
				const std::string&		inCCacheName);

		const CCICCacheData::UniqueID&
			OpenDefaultCCache ();
	
		std::string
			GetDefaultCCacheName ();

		const CCICCacheData::UniqueID&
			CreateCCache (
				const std::string&		inName,
				CCIInt32				inVersion,
				const std::string&		inPrincipal);

		const CCICCacheData::UniqueID&
			CreateDefaultCCache (
				CCIInt32				inVersion,
				const std::string&		inPrincipal);

		const CCICCacheData::UniqueID&
			CreateNewCCache (
				CCIInt32				inVersion,
				const std::string&		inPrincipal);
				
		void GetCCacheIDs (
				std::vector <CCICCacheData::UniqueID>&		outCCacheIDs) const;
				
		CCILockID
			Lock ();
		
		void
			Unlock (
				CCILockID				inLock);
				
		const CCIContextData::UniqueID&
			GetContext () const {
			return mContext;
		}
			
	private:
		CCIContextData::UniqueID	mContext;

		// We need to hold the global context available for as long as
		// there are cc_context_t's referring to it (this is different from
		// ccaches and credentials which can disappear while someone has 
		// reference to them). 
		CCISharedStaticDataProxy	<CCIContextData> 			mGlobalContextProxy;

		// Disallowed
		CCIContextDataCallStub (const CCIContextDataCallStub&);
		CCIContextDataCallStub& operator = (const CCIContextDataCallStub&);
};

class CCIContextDataCallInterface {
	public:
		CCIContextDataCallInterface (
			const CCIContextData::UniqueID&		inContext);
			
		CCIContextData* operator -> () { return mContext; }
			
		static const CCIContextData::UniqueID&	GetGlobalContextID ();
		
	private:
		CCIContextData*		mContext;

		// Let the stub get at the global context
		friend class CCIContextDataCallStub;

		static	CCISharedStaticData 		<CCIContextData>	sGlobalContext;
		CCISharedStaticDataProxy	<CCIContextData> 			mGlobalContextProxy;
		static	CCISharedStaticDataProxy	<CCIContextData> 	sGlobalContextProxy;

		// Disallowed
		CCIContextDataCallInterface (const CCIContextDataCallInterface&);
		CCIContextDataCallInterface& operator = (const CCIContextDataCallInterface&);
};

namespace CallImplementations {
	typedef	CCIContextDataCallStub	ContextDataStub;
}

