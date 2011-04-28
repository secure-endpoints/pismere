/*
 * CCIContextData.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCIContextData.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCIContextData.h"
#include "CCacheUtil.h"
#include "CCIException.h"

#include <algorithm>
#include <strstream>

const char	CCIContextData::sInitialDefaultCCacheName [] = "Initial default ccache";

CCIContextData::CCIContextData ():
	mCCaches () {
	
	Changed ();
}

CCIContextData::~CCIContextData () {
}

const CCICCacheData::UniqueID&
CCIContextData::GetCCacheID (
	const std::string&		inName) const {
	
	StReadLock lock;
	CCICCacheData*	ccache;

	if (FindCCache (inName, ccache)) {
		return ccache -> GetGloballyUniqueID ();
	}
	
	throw CCIException (ccErrCCacheNotFound);
}

const CCICCacheData::UniqueID&
CCIContextData::GetDefaultCCacheID () const {
	StReadLock lock;
	if (mCCaches.size () == 0) {
		throw CCIException (ccErrCCacheNotFound);
	}
	
	return mCCaches [0];
}

std::string
CCIContextData::GetDefaultCCacheName () const {
	StReadLock lock;
	if (mCCaches.size () == 0) {
		return sInitialDefaultCCacheName;
	} else {
		return CCICCacheData::Resolve (mCCaches [0]) -> GetName ();
	}
}

const CCICCacheData::UniqueID&
CCIContextData::CreateCCache (
	const std::string&		inName,
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	StWriteLock lock;
	
	CCICCacheData*		ccache;
	
	if (FindCCache (inName, ccache)) {
		ccache -> SetPrincipal (inVersion, inPrincipal);
		Changed ();
		return ccache -> GetGloballyUniqueID ();
	}
	
	CCICCacheData*	newCCache = new CCICCacheData (this, inName, inVersion, inPrincipal);

	// Note that the default ccache is the first ccache in the list, so
	// if it doesn't exist, this sets a new default, but if a default
	// exists, it's unchanged
	mCCaches.push_back (newCCache -> GetGloballyUniqueID ());
	
	Changed ();
	
	return newCCache -> GetGloballyUniqueID ();
}

const CCICCacheData::UniqueID&
CCIContextData::CreateDefaultCCache (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	StWriteLock lock;
	
	return CreateCCache (GetDefaultCCacheName (), inVersion, inPrincipal);
}

const CCICCacheData::UniqueID&
CCIContextData::CreateNewCCache (
	CCIInt32				inVersion,
	const std::string&		inPrincipal) {
	
	StWriteLock lock;
	
	if (mCCaches.size () == 0) {
		return CreateDefaultCCache (inVersion, inPrincipal);
	} else {
		for (CCIUInt32 index = 0; ; index++) {
			char	newName [64];
			std::strstream	newNameStream (newName, 64);
			newNameStream << index << std::ends;

			CCICCacheData*	ccache;

			if (!FindCCache (newName, ccache)) {
				return CreateCCache (newName, inVersion, inPrincipal);
			}
		}
	}
}

CCITime
CCIContextData::GetChangeTime () const {
	StReadLock lock;
	return mChangeTime;
}

void			
CCIContextData::GetCCacheIDs (
	std::vector <CCICCacheData::UniqueID>&	outCCacheIDs) const {
	
	StReadLock		lock;
	
	outCCacheIDs.reserve (mCCaches.size ());
	outCCacheIDs.clear ();
	SharedCCacheCollection::const_iterator		iterator = mCCaches.begin ();
	for (; iterator != mCCaches.end (); iterator++) {
		outCCacheIDs.push_back (*iterator);
	}
}

CCILockID
CCIContextData::Lock () const {
#pragma message (CCIMessage_Warning_ "unimplemented")
	return 0;
}

void
CCIContextData::Unlock (
	const CCILockID&		inLock) {
#pragma message (CCIMessage_Warning_ "unimplemented")
}

void
CCIContextData::Changed () {
	StWriteLock lock;
	mChangeTime = CCICurrentTime ();
}

void
CCIContextData::RemoveCCache (
	const CCICCacheData&	inCCache) {
	
	StWriteLock	lock;
	mCCaches.erase (std::find (mCCaches.begin (), mCCaches.end (), inCCache.GetGloballyUniqueID ()));
	Changed ();
}

void
CCIContextData::SetDefault (
	const CCICCacheData&	inCCache) {
	
	StWriteLock lock;
	RemoveCCache (inCCache);
	mCCaches.push_front (inCCache.GetGloballyUniqueID ());
	Changed ();
}

bool
CCIContextData::FindCCache (
	const std::string&	inName,
	CCICCacheData*&		outCCache) const {
	
	StReadLock	lock;
	
	SharedCCacheCollection::const_iterator	iter;
	
	for (iter = mCCaches.begin (); iter < mCCaches.end (); iter++) {
		CCICCacheData*	ccache = CCICCacheData::Resolve (*iter);
		if (ccache -> GetName () == inName) {
			outCCache = ccache;
			return true;
		}
	}
	
	return false;
}

