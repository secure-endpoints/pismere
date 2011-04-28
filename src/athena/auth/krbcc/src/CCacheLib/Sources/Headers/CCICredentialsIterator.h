/*
 * CCICredentialsIterator.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICredentialsIterator.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */
 
#pragma once

#include "CCache.h"
#include "CCIUniqueInProcess.h"
#include "CCIMagic.h"
#include "CCIleaks.h"
#include "CCIInternal.h"
#include "CCIInternalize.h"
#include "CCICCacheData.h"
#include "CCache.debug.h"

class CCECredentialsIterator {
	public:
		static cc_int32 Release (
			cc_credentials_iterator_t	inCCache);
			
		static cc_int32 Next (
			cc_credentials_iterator_t	inIterator,
			cc_credentials_t*			outCredentials);
			
	private:
		// Disallowed
		CCECredentialsIterator ();
		CCECredentialsIterator (const CCECredentialsIterator&);
		CCECredentialsIterator& operator = (const CCECredentialsIterator&);
};

class CCICCache;

class CCICredentialsIterator:
	public CCIUniqueInProcess <CCICredentialsIterator>,
	public CCIMagic <CCICredentialsIterator>,
	public CCILeaks <CCICredentialsIterator>,
	public CCIInternal <CCICredentialsIterator, cc_credentials_iterator_d> {

	public:
		CCICredentialsIterator (
			const	CCICCache&		inCCache);
			
		~CCICredentialsIterator ();
			
		bool
			HasMore () const;
			
		CCICredentialsData::UniqueID
			Next ();
			
#if CCache_v2_compat
		CCIUInt32
			CompatGetVersion () { return mVersion; }
#endif
			
			
		enum {
			class_ID = FOUR_CHAR_CODE ('CrIt'),
			invalidObject = ccErrInvalidCredentialsIterator
		};
		
	private:
		std::vector <CCICredentialsData::UniqueID>				mIterationSet;
		std::vector <CCICredentialsData::UniqueID>::iterator	mIterator;
		CCICCacheData::UniqueID									mCCache;
		StReadLock												mCCacheLock;
#if CCache_v2_compat
		CCIUInt32					mVersion;
#endif
		
		void		Validate ();

		static const	cc_credentials_iterator_f	sFunctionTable;

		friend class StInternalize <CCICredentialsIterator, cc_credentials_iterator_d>;
		friend class CCIInternal <CCICredentialsIterator, cc_credentials_iterator_d>;

		// Disallowed
		CCICredentialsIterator (const CCICredentialsIterator&);
		CCICredentialsIterator& operator = (const CCICredentialsIterator&);
};

typedef StInternalize <CCICredentialsIterator, cc_credentials_iterator_d>	StCredentialsIterator;

