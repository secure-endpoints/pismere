#pragma once

#include "CCache.internal.h"

//
// CCIUniqueInProcess
//
// This is a template that is used to assign unique
// IDs within a process to obejcts of type T. The IDs are not
// going to be unique among different classes of objects, or among
// processes.
//

template <class T>
class CCIUniqueInProcess {
	public:
	
		typedef CCIUInt32	UniqueID;
			
		CCIUniqueInProcess ():
			mUniqueID (sNextUniqueID++) {}

		~CCIUniqueInProcess () {}
		
		UniqueID GetUniqueIDInProcess () const { return mUniqueID; }
		
		bool operator == (
			const CCIUniqueInProcess&		inRhs) const {
			return inRhs.mUniqueID == mUniqueID;
		}
		
		bool Valid () const { return mUniqueID < sNextUniqueID; }
	
	private:
		UniqueID		mUniqueID;
		
		static	UniqueID		sNextUniqueID;

		// Disallowed
		CCIUniqueInProcess (const CCIUniqueInProcess&);
		CCIUniqueInProcess& operator = (const CCIUniqueInProcess&);
};
