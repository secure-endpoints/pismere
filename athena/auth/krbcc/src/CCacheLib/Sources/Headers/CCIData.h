#pragma once

#include "CCache.h"
#include "CCache.internal.h"
#include "CCIAllocators.h"
#include "CCIImplementations.h"

class CCISharedCCData:
	public Implementations::SharedData {
	public:
		CCISharedCCData (
			const cc_data&		inData);
			
		typedef Implementations::Vector <unsigned char>::Shared::iterator	iterator;
		typedef Implementations::Vector <unsigned char>::Shared::const_iterator	const_iterator;
			
		CCIUInt32 GetType () const { return mType; }
		CCIUInt32 GetSize () const { return mSize; }
		
		iterator begin () { return mData.begin (); }
		iterator end () { return mData.end (); }
		const_iterator begin () const { return mData.begin (); }
		const_iterator end () const { return mData.end (); }
			
	private:
		CCIUInt32											mType;
		CCIUInt32											mSize;
		Implementations::Vector <unsigned char>::Shared		mData;
};

class CCISharedCCDataArray:
	public Implementations::SharedData {
	public:
		CCISharedCCDataArray (
			const cc_data* const*		inData);
			
		typedef Implementations::Vector <CCISharedCCData*>::Shared::iterator	iterator;
		typedef Implementations::Vector <CCISharedCCData*>::Shared::const_iterator	const_iterator;
			
		CCIUInt32 GetSize () const { return mItems.size (); }
			
		iterator begin () { return mItems.begin (); }
		iterator end () { return mItems.end (); }
			
		const_iterator begin () const { return mItems.begin (); }
		const_iterator end () const { return mItems.end (); }

	private:
		Implementations::Vector <CCISharedCCData*>::Shared	mItems;
};

namespace Implementations {
	typedef ::CCISharedCCData CCISharedCCData;
	typedef ::CCISharedCCDataArray CCISharedCCDataArray;
}
