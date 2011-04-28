/*
 * CCICredentialsDataCallStub.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICredentialsDataCallStubs.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */

#pragma once

#include "CCICredentialsData.h"

#if CCache_v2_compat
#include "CCache2.h"
#endif

class CCICredentialsDataCallStub {
	public:
		CCICredentialsDataCallStub (
			CCICredentialsData::UniqueID		inCredentials);
		~CCICredentialsDataCallStub ();
		
		CCIInt32	GetCredentialsVersion ();
		
		const CCICredentialsData::UniqueID&	GetCredentials () const { return mCredentials; }
		
		void	CopyV4Credentials (
			cc_credentials_v4_t&		outCredentials) const;
		
		void	CopyV5Credentials (
			cc_credentials_v5_t&		outCredentials) const;
		
#if CCache_v2_compat
		void	CompatCopyV4Credentials (
			cc_credentials_v4_compat&	outCredentials) const;
		
		void	CompatCopyV5Credentials (
			cc_credentials_v5_compat&	outCredentials) const;
#endif
		
	private:
		CCICredentialsData::UniqueID		mCredentials;

		// Disallowed
		CCICredentialsDataCallStub ();
		CCICredentialsDataCallStub (const CCICredentialsDataCallStub&);
		CCICredentialsDataCallStub& operator = (const CCICredentialsDataCallStub&);
};

class CCICredentialsDataCallInterface {
	public:
		CCICredentialsDataCallInterface (
			const CCICredentialsData::UniqueID&		inCredentials);
			
		CCICredentialsData* operator -> () { return mCredentials; }
		
	private:
		CCICredentialsData*	mCredentials;
};

namespace CallImplementations {
	typedef	CCICredentialsDataCallStub	CredentialsDataStub;
}

