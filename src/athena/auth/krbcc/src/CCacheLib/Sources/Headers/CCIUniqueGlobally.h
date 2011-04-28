#pragma once

#include <map>

#include "CCIException.h"
#include "CCISharedStaticData.h"
#include "CCache.internal.h"
#include "CCIAllocators.h"
#include "CCIImplementations.h"

// CCIGloballyUnique is a template that is used to assign unique
// IDs among processes to objects of type T. The IDs are not going
// to be unique among different types, but they will be unique
// among different processes.
// The unique IDs have to be mapped back to an object,
// so we keep a global array of pointers to all objects of a type,
// and use that to map a unique ID to an object. 


template <class T>
class CCIUniqueGlobally:
	public Implementations::SharedData {
	public:
	
		typedef typename	CCIUInt32							ProcessID;
		typedef typename	CCIUInt32							ObjectID;
		typedef typename	std::pair <ProcessID, ObjectID>		UniqueID;
			
		CCIUniqueGlobally ():
			mGlobalsProxy (sGlobals) {
			mUniqueID = UniqueID (mGlobalsProxy.Get () -> sProcessID, 
				mGlobalsProxy.Get () -> sNextObjectID++);
			mGlobalsProxy.Get () -> sObjects.insert (
				ObjectMap::value_type (mUniqueID, static_cast <T*> (this)));
		}

		~CCIUniqueGlobally () {
			mGlobalsProxy.Get () -> sObjects.erase (mUniqueID);
		}
		
		const UniqueID& GetGloballyUniqueID () const { return mUniqueID; }
		
		bool operator == (
			const CCIUniqueGlobally&		inRhs) const {
			return inRhs.mUniqueID == mUniqueID;
		}
		
		bool Valid () const;
		
		static T*	Resolve (
			const UniqueID&	inID);
		
	private:
		UniqueID		mUniqueID;

		// I would have preferred to use hash_map, but CWP4 doesn't have
		// a standards compliant hash_map (no allocator template argument),
		// so I can't (the allocator is needed for sysheap storage when we don't
		// use AEs).
		typedef	typename	Implementations::Map <UniqueID, T*>::Shared		ObjectMap;

		struct Globals: CCISharedData {
			Globals () { sNextObjectID = 0; sProcessID = 0; }
			ObjectID			sNextObjectID;
			ProcessID			sProcessID;
			ObjectMap			sObjects;
		};
		
		static CCISharedStaticData <Globals>		sGlobals;
		CCISharedStaticDataProxy <Globals>			mGlobalsProxy;
		static CCISharedStaticDataProxy <Globals>	sGlobalsProxy;

		// Disallowed
		CCIUniqueGlobally (const CCIUniqueGlobally&);
		CCIUniqueGlobally& operator = (const CCIUniqueGlobally&);

};

template <class T>
T*
CCIUniqueGlobally <T>::Resolve (
	const typename CCIUniqueGlobally <T>::UniqueID&	inID) {
	
	StReadLock	lock;
	CCIUniqueGlobally::ObjectMap::iterator	location =
		sGlobalsProxy.Get () -> sObjects.find (inID);
		
	if (location == sGlobalsProxy.Get () -> sObjects.end ()) {
		throw CCIException (T::objectNotFound);
	}
	
	return location -> second;
}
