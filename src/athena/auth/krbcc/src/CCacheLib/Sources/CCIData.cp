#include "CCache.config.h"

#include "CCIData.h"
#include "CCIAllocators.h"

CCISharedCCData::CCISharedCCData (
	const cc_data&		inData):
	mType (inData.type),
	mSize (inData.length),
	mData (mSize) {
	
	for (CCIUInt32	index = 0; index < mSize; index++) {
		mData [index] = static_cast <unsigned char*> (inData.data) [index];
	}
}

CCISharedCCDataArray::CCISharedCCDataArray (
	const cc_data* const*		inData) {

	if (inData != nil) {
		for (const cc_data* const*	item = inData; *item != NULL; item++) {
			mItems.push_back (new CCISharedCCData (**item));
		}
	}
};

namespace CallImplementations {
	typedef ::CCISharedCCData CCISharedCCData;
	typedef ::CCISharedCCDataArray CCISharedCCDataArray;
}
