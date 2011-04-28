/*
 * CCICredentials.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCICredentials.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */
 
#include "CCache.h"
#include "CCache.internal.h"
#include "CCacheUtil.h"

#if CCache_v2_compat
#include "CCache2.h"
#endif

#pragma once

#include "CCIInternalize.h"
#include "CCIInternal.h"
#include "CCIMagic.h"
#include "CCILeaks.h"
#include "CCICredentialsDataStubs.h"
#include "CCIUniqueInProcess.h"

class CCECredentials {
	public:
		static cc_int32		Release (
			cc_credentials_t			inCredentials);
			
	private:
		// Disallowed
		CCECredentials ();
		CCECredentials (const CCECredentials&);
		CCECredentials& operator = (const CCECredentials&);
};

class CCICredentials:
	public CCIUniqueInProcess <CCICredentials>,
	public CCIMagic <CCICredentials>,
	public CCILeaks <CCICredentials>,
	public CCIInternal <CCICredentials, cc_credentials_d>,
	public Implementations::CredentialsDataStub {
	public:
		enum {
			class_ID = FOUR_CHAR_CODE ('Cred'),
			invalidObject = ccErrInvalidCredentials
		};
		
		CCICredentials (
			const CCICredentialsData::UniqueID&		inCredentials);
		
		~CCICredentials ();
		
	private:
		cc_credentials_union		mCredentials;
	
		void Validate ();
		
	const	static	cc_credentials_f	sFunctionTable;

	friend class StInternalize <CCICredentials, cc_credentials_d>;
	friend class CCIInternal <CCICredentials, cc_credentials_d>;
};

typedef StInternalize <CCICredentials, cc_credentials_d>		StCredentials;

class CCICredentialsV4:
	public cc_credentials_v4_t {
	
	public:
		CCICredentialsV4 (
			const CCICredentialsData::UniqueID&		inCredentials);
			
		~CCICredentialsV4 ();
			
};

class CCICredentialsV5:
	public cc_credentials_v5_t {
	
	public:
		CCICredentialsV5 (
			const CCICredentialsData::UniqueID&		inCredentials);
		
		~CCICredentialsV5 ();
		
	private:
		void Cleanup ();
};

#if CCache_v2_compat
class CCICompatCredentials:
	public CCIUniqueInProcess <CCICompatCredentials>,
	public CCIMagic <CCICompatCredentials>,
	public CCILeaks <CCICompatCredentials>,
	public CCIInternalWorkaround <CCICompatCredentials, cred_union>,
	public Implementations::CredentialsDataStub {
	public:
		enum {
			class_ID = FOUR_CHAR_CODE ('Crd2'),
			invalidObject = ccErrInvalidCredentials
		};
		
		CCICompatCredentials (
			const CCICredentialsData::UniqueID&		inCredentials);
		
		~CCICompatCredentials ();

	private:
		void Validate ();
		
	friend class StInternalize <CCICompatCredentials, cred_union>;
	friend class CCIInternalWorkaround <CCICompatCredentials, cred_union>;
};

typedef StInternalize <CCICompatCredentials, cred_union>		StCompatCredentials;

class CCICompatCredentialsV4:
	public cc_credentials_v4_compat {
	
	public:
		CCICompatCredentialsV4 (
			const CCICredentialsData::UniqueID&		inCredentials);
			
		~CCICompatCredentialsV4 ();
			
};

class CCICompatCredentialsV5:
	public cc_credentials_v5_compat {
	
	public:
		CCICompatCredentialsV5 (
			const CCICredentialsData::UniqueID&		inCredentials);
		
		~CCICompatCredentialsV5 ();
		
	private:
		void Cleanup ();
};
#endif