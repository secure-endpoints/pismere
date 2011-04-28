/*
 * CCICache.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICCacheIterator.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */
 
#pragma once

#include <vector>

#include "CCache.h"

#include "CCIUniqueInProcess.h"
#include "CCIMagic.h"
#include "CCILeaks.h"
#include "CCIInternal.h"
#include "CCIInternalize.h"
#include "CCICCacheData.h"
#include "CCICCache.h"

class CCECCacheIterator {
	public:
		static cc_int32 Release (
			cc_ccache_iterator_t	inCCache);
			
		static cc_int32 Next (
			cc_ccache_iterator_t	inIterator,
			cc_ccache_t*			outCCache);
			
	private:
		// Disallowed
		CCECCacheIterator ();
		CCECCacheIterator (const CCECCacheIterator&);
		CCECCacheIterator& operator = (const CCECCacheIterator&);
};

class CCIContext;

class CCICCacheIterator:
	public CCIUniqueInProcess <CCICCacheIterator>,
	public CCIMagic <CCICCacheIterator>,
	public CCILeaks <CCICCacheIterator>,
	public CCIInternal <CCICCacheIterator, cc_ccache_iterator_d> {

	public:
		CCICCacheIterator (
			const	CCIContext&		inContext);
			
		~CCICCacheIterator ();
			
		bool
			HasMore () const;

		CCICCacheData::UniqueID
			Next ();
			
		CCICCacheData::UniqueID
			Current ();
			
#if CCache_v2_compat
		void		CompatResetRepeatCount () { mRepeatCount = 0; }
		void		CompatIncrementRepeatCount () { mRepeatCount ++; }
		CCIUInt32	CompatGetRepeatCount () { return mRepeatCount; }
#endif
			
		enum {
			class_ID = FOUR_CHAR_CODE ('CCIt'),
			invalidObject = ccErrInvalidCCacheIterator
		};
		
	private:
		std::vector <CCICCacheData::UniqueID>			mIterationSet;
		std::vector <CCICCacheData::UniqueID>::iterator	mIterator;
		CCIContextData::UniqueID						mContext;
		StReadLock										mContextLock;
#if CCache_v2_compat
		CCIUInt32										mRepeatCount;
#endif
		
		void		Validate ();

		static const	cc_ccache_iterator_f	sFunctionTable;

		friend class StInternalize <CCICCacheIterator, cc_ccache_iterator_d>;
		friend class CCIInternal <CCICCacheIterator, cc_ccache_iterator_d>;

		// Disallowed
		CCICCacheIterator (const CCICCacheIterator&);
		CCICCacheIterator& operator = (const CCICCacheIterator&);
};

typedef StInternalize <CCICCacheIterator, cc_ccache_iterator_d>	StCCacheIterator;

