/*
 * CCIContextDataCallStubs.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCIContextDataCallStubs.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCIContextDataCallStubs.h"

#if CCache_ContainsSharedStaticData
CCISharedStaticData <CCIContextData> CCIContextDataCallInterface::sGlobalContext;
#endif

CCISharedStaticDataProxy	<CCIContextData>
					CCIContextDataCallInterface::sGlobalContextProxy = sGlobalContext;


CCIContextDataCallStub::CCIContextDataCallStub ():
	mContext (CCIContextDataCallInterface::GetGlobalContextID ()),
	mGlobalContextProxy (CCIContextDataCallInterface::sGlobalContext) {

	mGlobalContextProxy.Get ();
}

CCIContextDataCallStub::CCIContextDataCallStub (
	const CCIContextData::UniqueID&		inContext):
	mContext (inContext),
	mGlobalContextProxy (CCIContextDataCallInterface::sGlobalContext) {
	
	mGlobalContextProxy.Get ();
}

CCIContextDataCallStub::~CCIContextDataCallStub () {
}

CCITime
CCIContextDataCallStub::GetChangeTime () {

	return CCIContextDataCallInterface (mContext) -> GetChangeTime ();
}


const CCICCacheData::UniqueID&
CCIContextDataCallStub::OpenCCache (
	const std::string&		inCCacheName) {
	
	return CCIContextDataCallInterface (mContext) -> GetCCacheID (inCCacheName);
}

const CCICCacheData::UniqueID&
CCIContextDataCallStub::OpenDefaultCCache () {
	
	return CCIContextDataCallInterface (mContext) -> GetDefaultCCacheID ();
}

std::string
CCIContextDataCallStub::GetDefaultCCacheName () {
	
	return CCIContextDataCallInterface (mContext) -> GetDefaultCCacheName ();
}

const CCICCacheData::UniqueID&
CCIContextDataCallStub::CreateCCache (
	const std::string&		inName,
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	return CCIContextDataCallInterface (mContext) -> CreateCCache (inName, inVersion, inPrincipal);
}

const CCICCacheData::UniqueID&
CCIContextDataCallStub::CreateDefaultCCache (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	return CCIContextDataCallInterface (mContext) -> CreateDefaultCCache (inVersion, inPrincipal);
}

const CCICCacheData::UniqueID&
CCIContextDataCallStub::CreateNewCCache (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	return CCIContextDataCallInterface (mContext) -> CreateNewCCache (inVersion, inPrincipal);
}

void CCIContextDataCallStub::GetCCacheIDs (
		std::vector <CCICCacheData::UniqueID>&		outCCacheIDs) const {

	CCIContextDataCallInterface (mContext) -> GetCCacheIDs (outCCacheIDs);
}

CCILockID
CCIContextDataCallStub::Lock () {
	return CCIContextDataCallInterface (mContext) -> Lock ();
}

void
CCIContextDataCallStub::Unlock (
	CCILockID		inLock) {
	CCIContextDataCallInterface (mContext) -> Unlock (inLock);
}

CCIContextDataCallInterface::CCIContextDataCallInterface (
	const CCIContextData::UniqueID&		inContext):
	mGlobalContextProxy (sGlobalContext) {
	
	mGlobalContextProxy.Get (); // Increment the refcount
	mContext = CCIContextData::Resolve (inContext);
}

const CCIContextData::UniqueID&
CCIContextDataCallInterface::GetGlobalContextID () {
	return sGlobalContextProxy.Get () -> GetGloballyUniqueID ();
}
