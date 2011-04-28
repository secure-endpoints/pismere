/*
 * CCISharedStaticData.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCISharedStaticData.h,v 1.6 2000/06/21 20:10:01 dalmeida Exp $
 */
 
/*
 * This class is used to initialize globally shared data
 *
 * When we are using data-only shared libraries and the system heap,
 * we can't get a constructor called when the data library is loaded,
 * because the data library can't contain any code.
 *
 * So, any class that has globally shared static data has to inherit
 * from CCISharedStaticData and provide InitializeStaticData to initialize
 * the static data
 */
 
#pragma once

#include "CCache.debug.h"

template <class Data> class CCISharedStaticDataProxy;

template <class Data>
class CCISharedStaticData {
	public:
		CCISharedStaticData (): mRefCount (0), mData (nil) {}
		~CCISharedStaticData () {}
		
	private:
		CCIUInt32		mRefCount;
		Data*			mData;
		
		friend class CCISharedStaticDataProxy <Data>;
};

template <class Data> 
class CCISharedStaticDataProxy {
	public:
		CCISharedStaticDataProxy (
			CCISharedStaticData <Data>&	inData):
			mData (inData),
			mInitialized (false) {}

		Data* Get () {
			if (!mInitialized) {
				if (mData.mRefCount == 0) {
					mData.mData = new Data ();
				}
				mData.mRefCount++;
				mInitialized = true;
			}

			return mData.mData;
		}
		
		~CCISharedStaticDataProxy () {
			if (mInitialized) {
				mInitialized = false;
				mData.mRefCount--;
				if (mData.mRefCount == 0) {
					delete mData.mData;
				}
			}
		}
	
	private:
		CCISharedStaticData <Data>&		mData;
		bool							mInitialized;
};
