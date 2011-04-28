#pragma once

#include <list>

#include "CCache.h"
#include "CCIUniqueGlobally.h"
#include "CCIImplementations.h"
#include "CCIAllocators.h"
#include "CCIData.h"

#if CCache_v2_compat
#include "CCache2.h"
#endif

class CCICredentialsData:
	public CCIUniqueGlobally <CCICredentialsData> {
	public:
	
		enum {
			objectNotFound = ccErrCredentialsNotFound
		};
		
		CCICredentialsData (
			const cc_credentials_union*		inCredentials);
#if CCache_v2_compat
		CCICredentialsData (
			const cred_union&				inCredentials);
#endif
		~CCICredentialsData ();
		
		CCIUInt32	GetVersion () const;
		
		void		CopyV4Credentials (
			cc_credentials_v4_t&			outCredentials) const;
		
		void		CopyV5Credentials (
			cc_credentials_v5_t&			outCredentials) const;
		
#if CCache_v2_compat
		void		CompatCopyV4Credentials (
			cc_credentials_v4_compat&		outCredentials) const;
		
		void		CompatCopyV5Credentials (
			cc_credentials_v5_compat&		outCredentials) const;
#endif
		
		class VersionMatch {
			public:
				VersionMatch (
					CCIUInt32		inVersion):
					mVersion (inVersion) {}

				bool operator () (
					const CCICredentialsData*	inCompare) {
					return (inCompare -> GetVersion () == mVersion);
				}
			
			private:
				CCIUInt32	mVersion;
		};

		class UniqueIDMatch {
			public:
				UniqueIDMatch (
					const UniqueID&		inUniqueID):
					mUniqueID (inUniqueID) {}

				bool operator () (
					const CCICredentialsData*	inCompare) {
					return (inCompare -> GetGloballyUniqueID () == mUniqueID);
				}
			
			private:
				UniqueID	mUniqueID;
		};
		
	private:
		// Disallowed
		CCICredentialsData ();
		CCICredentialsData (
			const CCICredentialsData&);
		CCICredentialsData&
		operator = (
			const CCICredentialsData&);

		class CCICredentialsV4Data:
			public Implementations::SharedData {
			public:
				CCICredentialsV4Data (
					const cc_credentials_v4_t*	inCredentials);
#if CCache_v2_compat
				CCICredentialsV4Data (
					const cc_credentials_v4_compat*	inCredentials);
#endif
				
				void	CopyCredentials (
					cc_credentials_v4_t&		outCredentials) const;
#if CCache_v2_compat
				void	CompatCopyCredentials (
					cc_credentials_v4_compat&		outCredentials) const;
#endif
				
			private:
				cc_credentials_v4_t		mCredentials;
		};
		
		class CCICredentialsV5Data:
			public Implementations::SharedData {
			public:
				CCICredentialsV5Data (
					const cc_credentials_v5_t*	inCredentials);
#if CCache_v2_compat
				CCICredentialsV5Data (
					const cc_credentials_v5_compat*	inCredentials);
#endif
			
				void	CopyCredentials (
					cc_credentials_v5_t&		outCredentials) const;

#if CCache_v2_compat
				void	CompatCopyCredentials (
					cc_credentials_v5_compat&		outCredentials) const;
#endif

			private:
				typedef	Implementations::String::Shared			SharedString;
				typedef Implementations::CCISharedCCData		SharedCCData;
				typedef Implementations::CCISharedCCDataArray	SharedCCDataArray;

				SharedString		mClient;
				SharedString		mServer;
				SharedCCData		mKeyblock;
				CCITime				mAuthTime;
				CCITime				mStartTime;
				CCITime				mEndTime;
				CCITime				mRenewTill;
				CCIUInt32			mIsSKey;
				CCIUInt32			mTicketFlags;
				SharedCCDataArray	mAddresses;
				SharedCCData		mTicket;
				SharedCCData		mSecondTicket;
				SharedCCDataArray	mAuthData;
				
				void	CopyString (const SharedString&	inSource, char*&	ioDestination) const;
				void	CopyCCData (const SharedCCData& inSource, cc_data&	ioDestination) const;
				void	CopyCCDataArray (const SharedCCDataArray& inSource, cc_data**&	ioDestination) const;
				
				void	DeleteString (char*&	inString) const;
				void	DeleteCCData (cc_data&	inData) const;
				void	DeleteCCDataArray (cc_data**&	inDataArray) const;
		};
		
		CCICredentialsV5Data*		mCredentialsV5;
		CCICredentialsV4Data*		mCredentialsV4;
};

