/*
 * CCICCacheDataCallStubs.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICCacheDataCallStubs.cp,v 1.4 2000/06/16 21:44:04 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCICCacheDataCallStubs.h"

CCICCacheDataCallStub::CCICCacheDataCallStub (
	CCICCacheData::UniqueID	inCCache):
	mCCache (inCCache) {
}

CCICCacheDataCallStub::~CCICCacheDataCallStub () {
}

void
CCICCacheDataCallStub::Destroy () {
	CCICCacheDataCallInterface (mCCache) -> Destroy ();
}

void
CCICCacheDataCallStub::SetDefault () {
	CCICCacheDataCallInterface (mCCache) -> SetDefault ();
}

CCIUInt32
CCICCacheDataCallStub::GetCredentialsVersion () {
	return CCICCacheDataCallInterface (mCCache) -> GetCredentialsVersion ();
}

std::string
CCICCacheDataCallStub::GetPrincipal (
	CCIUInt32				inVersion) {
	
	return CCICCacheDataCallInterface (mCCache) -> GetPrincipal (inVersion);
}
	
std::string
CCICCacheDataCallStub::GetName () {
	
	return CCICCacheDataCallInterface (mCCache) -> GetName ();
}
	
void
CCICCacheDataCallStub::SetPrincipal (
	CCIUInt32				inVersion,
	const std::string&		inPrincipal) {
	
	CCICCacheDataCallInterface (mCCache) -> SetPrincipal (inVersion, inPrincipal);
}
	
#if CCache_v2_compat
void
CCICCacheDataCallStub::CompatSetPrincipal (
	CCIUInt32				inVersion,
	const std::string&		inPrincipal) {
	
	CCICCacheDataCallInterface (mCCache) -> CompatSetPrincipal (inVersion, inPrincipal);
}
#endif	

void
CCICCacheDataCallStub::StoreCredentials (
	const cc_credentials_union*		inCredentials) {
	
	CCICCacheDataCallInterface (mCCache) -> StoreCredentials (inCredentials);
}

#if CCache_v2_compat
void
CCICCacheDataCallStub::CompatStoreCredentials (
	const cred_union&		inCredentials) {
	
	CCICCacheDataCallInterface (mCCache) -> CompatStoreCredentials (inCredentials);
}

#endif
	
void		
CCICCacheDataCallStub::RemoveCredentials (
	const CCICredentialsDataCallStub&	inCredentials) {
	
	CCICCacheDataCallInterface (mCCache) -> RemoveCredentials (inCredentials.GetCredentials ());
}
	
CCITime		
CCICCacheDataCallStub::GetLastDefaultTime () {
	
	return CCICCacheDataCallInterface (mCCache) -> GetLastDefaultTime ();
}

CCITime		
CCICCacheDataCallStub::GetChangeTime () {
	
	return CCICCacheDataCallInterface (mCCache) -> GetChangeTime ();
}

void		
CCICCacheDataCallStub::Move (
	CCICCacheDataCallStub&		inCCache) {
	
	CCICCacheDataCallInterface (mCCache) -> Move (inCCache.mCCache);
}

CCILockID		
CCICCacheDataCallStub::Lock () {
	
	return CCICCacheDataCallInterface (mCCache) -> Lock ();
}

void		
CCICCacheDataCallStub::Unlock (
	CCILockID					inLock) {
	
	CCICCacheDataCallInterface (mCCache) -> Unlock (inLock);
}

void		
CCICCacheDataCallStub::GetCredentialsIDs (
	std::vector <CCICredentialsData::UniqueID>&	outCredenitalsIDs) const {
	
	CCICCacheDataCallInterface (mCCache) -> GetCredentialsIDs (outCredenitalsIDs);
}

CCICCacheDataCallInterface::CCICCacheDataCallInterface (
	const CCICCacheData::UniqueID&		inCCache) {

	mCCache = CCICCacheData::Resolve (inCCache);
}
