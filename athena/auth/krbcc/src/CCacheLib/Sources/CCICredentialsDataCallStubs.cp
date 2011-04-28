/*
 * CCICredentialsDataCallStubs.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICredentialsDataCallStubs.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCICredentialsDataCallStubs.h"

CCICredentialsDataCallStub::CCICredentialsDataCallStub (
	CCICredentialsData::UniqueID	inCredentials):
	mCredentials (inCredentials) {
}

CCICredentialsDataCallStub::~CCICredentialsDataCallStub () {
}

CCIInt32
CCICredentialsDataCallStub::GetCredentialsVersion () {
	return CCICredentialsDataCallInterface (mCredentials) -> GetVersion ();
}

void
CCICredentialsDataCallStub::CopyV4Credentials (
	cc_credentials_v4_t&		outCredentials) const {
	CCICredentialsDataCallInterface (mCredentials) -> CopyV4Credentials (outCredentials);
}

void
CCICredentialsDataCallStub::CopyV5Credentials (
	cc_credentials_v5_t&		outCredentials) const {
	CCICredentialsDataCallInterface (mCredentials) -> CopyV5Credentials (outCredentials);
}

#if CCache_v2_compat
void
CCICredentialsDataCallStub::CompatCopyV4Credentials (
	cc_credentials_v4_compat&		outCredentials) const {
	CCICredentialsDataCallInterface (mCredentials) -> CompatCopyV4Credentials (outCredentials);
}

void
CCICredentialsDataCallStub::CompatCopyV5Credentials (
	cc_credentials_v5_compat&		outCredentials) const {
	CCICredentialsDataCallInterface (mCredentials) -> CompatCopyV5Credentials (outCredentials);
}
#endif

CCICredentialsDataCallInterface::CCICredentialsDataCallInterface (
	const CCICredentialsData::UniqueID&		inCredentials) {

	mCredentials = CCICredentialsData::Resolve (inCredentials);
}
			