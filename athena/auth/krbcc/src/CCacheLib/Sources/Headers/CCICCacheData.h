#pragma once

#include <list>
#include <string>

#include "CCache.internal.h"

#include "CCIAllocators.h"
#include "CCIUniqueGlobally.h"
#include "CCIImplementations.h"

#include "CCICredentialsData.h"

#if CCache_v2_compat
#include "CCache2.h"
#endif

class CCIContextData;
class CCICCacheData;

class CCICCacheData:
	public CCIUniqueGlobally <CCICCacheData> {
	public:
	
		enum {
			objectNotFound = ccErrCCacheNotFound
		};
		
		typedef	Implementations::List <CCICCacheData>::Shared		SharedCCacheDataList;
			
	
		CCICCacheData ();
		CCICCacheData (
					CCIContextData*		inContext,
			const	std::string&		inName,
			CCIInt32					inVersion,
			const	std::string&		inPrincipal);
		~CCICCacheData ();
		
		void		Destroy ();

		void		SetDefault ();

		CCIUInt32	GetCredentialsVersion () const;
		
		std::string	GetPrincipal (
			CCIInt32				inVersion) const;
		
		std::string	GetName () const;
		
		void		SetPrincipal (
			CCIInt32				inVersion,
			const std::string&		inPrincipal);
		
#if CCache_v2_compat		
		void		CompatSetPrincipal (
			CCIInt32				inVersion,
			const std::string&		inPrincipal);
#endif
		
		void		StoreCredentials (
			const cc_credentials_union*				inCredentials);

#if CCache_v2_compat		
		void		CompatStoreCredentials (
			const cred_union&						inCredentials);
#endif
		
		void		RemoveCredentials (
			const CCICredentialsData::UniqueID&		inCredentials);
			
		CCITime		GetLastDefaultTime () const;

		CCITime		GetChangeTime () const;
		
		void		Move (
			const UniqueID&			inDestination);
			
		CCILockID	Lock ();

		void		Unlock (
			CCILockID			inLock);
			
		void			GetCredentialsIDs (
			std::vector <CCICredentialsData::UniqueID>&	outCredentialsIDs) const;
			
	private:
	
		typedef	Implementations::String::Shared							SharedString;
		typedef	Implementations::Vector <CCICredentialsData*>::Shared	SharedCredentialsVector;
	
		CCIContextData*							mContext;
		CCITime									mChangeTime;
		bool									mHasBeenDefault;
		CCITime									mLastDefaultTime;
		SharedString							mName;
		bool									mHavePrincipalV4;
		SharedString							mPrincipalV4;
		bool									mHavePrincipalV5;
		SharedString							mPrincipalV5;
		SharedCredentialsVector					mCredentials;
		
		void Changed ();

		// Disallowed
		CCICCacheData (const CCICCacheData&);
		CCICCacheData& operator = (const CCICCacheData&);
};

