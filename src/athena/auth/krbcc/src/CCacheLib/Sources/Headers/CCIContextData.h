#pragma once

#include <string>
#include <list>

#include "CCICCacheData.h"
#include "CCIUniqueGlobally.h"

class CCIContextData:
	public CCIUniqueGlobally <CCIContextData> {
	public:
		enum {
			objectNotFound = ccErrContextNotFound
		};
		
		CCIContextData ();
		~CCIContextData ();
		
		const CCICCacheData::UniqueID&	GetCCacheID (
			const std::string&		inName) const;
			
		const CCICCacheData::UniqueID& GetDefaultCCacheID () const;
		
		std::string		GetDefaultCCacheName () const;
		
		const CCICCacheData::UniqueID&	CreateCCache (
			const std::string&		inName,
			CCIInt32				inVersion,
			const std::string&		inPrincipal);

		const CCICCacheData::UniqueID&	CreateDefaultCCache (
			CCIInt32				inVersion,
			const std::string&		inPrincipal);

		const CCICCacheData::UniqueID&	CreateNewCCache (
			CCIInt32				inVersion,
			const std::string&		inPrincipal);
			
		CCITime			GetChangeTime () const;
		
		void			GetCCacheIDs (
			std::vector <CCICCacheData::UniqueID>&	outCCacheIDs) const;
			
		CCILockID		Lock () const;
		void			Unlock (
			const CCILockID&		inLock);

	private:
		// IDs for the ccaches are kept in defaultness order in the list
		// so that the first ccache is always the default one etc
		typedef	Implementations::Deque <CCICCacheData::UniqueID>::Shared		SharedCCacheCollection;
		SharedCCacheCollection						mCCaches;
		CCITime										mChangeTime;
	
		void 			Changed ();
		bool			FindCCache (
			const std::string&		inName,
			CCICCacheData*&			outCCache) const;
		void			RemoveCCache (
			const CCICCacheData&	inCCache);
		void			SetDefault (
			const CCICCacheData&	inCCache);
			
		friend class CCICCacheData;

		static const char sInitialDefaultCCacheName[];
		
		// Disallowed
		CCIContextData (const CCIContextData&);
		CCIContextData& operator = (const CCIContextData&);
};