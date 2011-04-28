/*
 * CCICCacheData.cp
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICredentialsData.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCICredentialsData.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $
 */

#include "CCache.config.h"

#include "CCICredentialsData.h"
#include "CCache.debug.h"

#include <string.h>

CCICredentialsData::CCICredentialsData (
	const cc_credentials_union*		inCredentials):
	CCIUniqueGlobally <CCICredentialsData> (),
	mCredentialsV4 (nil),
	mCredentialsV5 (nil) {
	
	CCIAssert_ ((inCredentials -> version == cc_credentials_v4) ||
		(inCredentials -> version == cc_credentials_v5));
	
	if (inCredentials -> version == cc_credentials_v4) {
		mCredentialsV4 = new CCICredentialsV4Data (inCredentials -> credentials.credentials_v4);
	} else {
		mCredentialsV5 = new CCICredentialsV5Data (inCredentials -> credentials.credentials_v5);
	}
}

CCICredentialsData::~CCICredentialsData () {
	if (mCredentialsV4)
		delete mCredentialsV4;
	if (mCredentialsV5)
		delete mCredentialsV5;
}

#if CCache_v2_compat
CCICredentialsData::CCICredentialsData (
	const cred_union&				inCredentials):
	CCIUniqueGlobally <CCICredentialsData> (),
	mCredentialsV4 (nil),
	mCredentialsV5 (nil) {
	
	CCIAssert_ ((inCredentials.cred_type == CC_CRED_V4) ||
		(inCredentials.cred_type == CC_CRED_V5));
	
	if (inCredentials.cred_type == CC_CRED_V4) {
		mCredentialsV4 = new CCICredentialsV4Data (inCredentials.cred.pV4Cred);
	} else {
		mCredentialsV5 = new CCICredentialsV5Data (inCredentials.cred.pV5Cred);
	}
}
#endif

CCIUInt32
CCICredentialsData::GetVersion () const {
	CCIAssert_ (((mCredentialsV4 == nil) && (mCredentialsV5 != nil)) || 
		(mCredentialsV4 != nil) && (mCredentialsV5 == nil));
	
	if (mCredentialsV4 != nil) {
		return cc_credentials_v4;
	} else {
		return cc_credentials_v5;
	}
}

void
CCICredentialsData::CopyV4Credentials (
	cc_credentials_v4_t&		outCredentials) const {
	CCIAssert_ ((mCredentialsV4 != nil) && (mCredentialsV5 == nil));
	
	mCredentialsV4 -> CopyCredentials (outCredentials);
}

void
CCICredentialsData::CopyV5Credentials (
	cc_credentials_v5_t&		outCredentials) const {
	CCIAssert_ ((mCredentialsV5 != nil) && (mCredentialsV4 == nil));
	
	mCredentialsV5 -> CopyCredentials (outCredentials);
}

#if CCache_v2_compat
void
CCICredentialsData::CompatCopyV4Credentials (
	cc_credentials_v4_compat&		outCredentials) const {
	CCIAssert_ ((mCredentialsV4 != nil) && (mCredentialsV5 == nil));
	
	mCredentialsV4 -> CompatCopyCredentials (outCredentials);
}

void
CCICredentialsData::CompatCopyV5Credentials (
	cc_credentials_v5_compat&		outCredentials) const {
	CCIAssert_ ((mCredentialsV5 != nil) && (mCredentialsV4 == nil));
	
	mCredentialsV5 -> CompatCopyCredentials (outCredentials);
}
#endif

#if PRAGMA_MARK
#pragma mark -
#endif

CCICredentialsData::CCICredentialsV4Data::CCICredentialsV4Data (
	const cc_credentials_v4_t*		inCredentials):
	
	mCredentials (*inCredentials) {
}

#if CCache_v2_compat
CCICredentialsData::CCICredentialsV4Data::CCICredentialsV4Data (
	const cc_credentials_v4_compat*		inCredentials) {
	
	mCredentials.version = 4;
	strcpy (mCredentials.principal, inCredentials -> principal);
	strcpy (mCredentials.principal_instance, inCredentials -> principal_instance);
	strcpy (mCredentials.service, inCredentials -> service);
	strcpy (mCredentials.service_instance, inCredentials -> service_instance);
	strcpy (mCredentials.realm, inCredentials -> realm);
	memcpy (mCredentials.session_key, inCredentials -> session_key, sizeof (inCredentials -> session_key));
	mCredentials.kvno = inCredentials -> kvno;
	mCredentials.string_to_key_type = inCredentials -> str_to_key;
	mCredentials.issue_date = inCredentials -> issue_date;
	mCredentials.lifetime = inCredentials -> lifetime;
	mCredentials.address = inCredentials -> address;
	mCredentials.ticket_size = inCredentials -> ticket_sz;
	CCIAssert_ (inCredentials -> ticket_sz <= sizeof(mCredentials.ticket));
#pragma message (CCIMessage_Warning_ "This should be a bad param error instead of an assertion")
	memcpy (mCredentials.ticket, inCredentials -> ticket, inCredentials -> ticket_sz);
}
#endif

void
CCICredentialsData::CCICredentialsV4Data::CopyCredentials (
	cc_credentials_v4_t&		outCredentials) const {
	outCredentials = mCredentials;
}

#if CCache_v2_compat
void
CCICredentialsData::CCICredentialsV4Data::CompatCopyCredentials (
	cc_credentials_v4_compat&		outCredentials) const {
	outCredentials.kversion = 4;
	strcpy (outCredentials.principal, mCredentials.principal);
	strcpy (outCredentials.principal_instance, mCredentials.principal_instance);
	strcpy (outCredentials.service, mCredentials.service);
	strcpy (outCredentials.service_instance, mCredentials.service_instance);
	strcpy (outCredentials.realm, mCredentials.realm);
	memcpy (outCredentials.session_key, mCredentials.session_key, sizeof (mCredentials.session_key));
	outCredentials.kvno = mCredentials.kvno;
	outCredentials.str_to_key = mCredentials.string_to_key_type;
	outCredentials.issue_date = mCredentials.issue_date;
	outCredentials.lifetime = mCredentials.lifetime;
	outCredentials.address = mCredentials.address;
	outCredentials.ticket_sz = mCredentials.ticket_size;
	memcpy (outCredentials.ticket, mCredentials.ticket, mCredentials.ticket_size);
	outCredentials.oops = 0xDEADBEEF;
}
#endif

#if PRAGMA_MARK
#pragma mark -
#endif

CCICredentialsData::CCICredentialsV5Data::CCICredentialsV5Data (
	const cc_credentials_v5_t*		inCredentials):
	
	mClient (inCredentials -> client),
	mServer (inCredentials -> server),
	mKeyblock (inCredentials -> keyblock),
	mAuthTime (inCredentials -> authtime),
	mStartTime (inCredentials -> starttime),
	mEndTime (inCredentials -> endtime),
	mRenewTill (inCredentials -> renew_till),
	mIsSKey (inCredentials -> is_skey),
	mTicketFlags (inCredentials -> ticket_flags),
	mAddresses (inCredentials -> addresses),
	mTicket (inCredentials -> ticket),
	mSecondTicket (inCredentials -> second_ticket),
	mAuthData (inCredentials -> authdata) {
}

#if CCache_v2_compat
CCICredentialsData::CCICredentialsV5Data::CCICredentialsV5Data (
	const cc_credentials_v5_compat*		inCredentials):
	
	mClient (inCredentials -> client),
	mServer (inCredentials -> server),
	mKeyblock (inCredentials -> keyblock),
	mAuthTime (inCredentials -> authtime),
	mStartTime (inCredentials -> starttime),
	mEndTime (inCredentials -> endtime),
	mRenewTill (inCredentials -> renew_till),
	mIsSKey (inCredentials -> is_skey),
	mTicketFlags (inCredentials -> ticket_flags),
	mAddresses (inCredentials -> addresses),
	mTicket (inCredentials -> ticket),
	mSecondTicket (inCredentials -> second_ticket),
	mAuthData (inCredentials -> authdata) {
}
#endif

void
CCICredentialsData::CCICredentialsV5Data::CopyCredentials (
	cc_credentials_v5_t&		outCredentials) const {
	
	CopyString (mClient, outCredentials.client);
	CopyString (mServer, outCredentials.server);
	CopyCCData (mKeyblock, outCredentials.keyblock);
	CopyCCDataArray (mAddresses, outCredentials.addresses);
	CopyCCData (mTicket, outCredentials.ticket);
	CopyCCData (mSecondTicket, outCredentials.second_ticket);
	CopyCCDataArray (mAuthData, outCredentials.authdata);
	
	outCredentials.authtime = mAuthTime;
	outCredentials.starttime = mStartTime;
	outCredentials.endtime = mEndTime;
	outCredentials.renew_till = mRenewTill;
	outCredentials.is_skey = mIsSKey;
	outCredentials.ticket_flags = mTicketFlags;
}

#if CCache_v2_compat
void
CCICredentialsData::CCICredentialsV5Data::CompatCopyCredentials (
	cc_credentials_v5_compat&		outCredentials) const {
	
	CopyString (mClient, outCredentials.client);
	CopyString (mServer, outCredentials.server);
	CopyCCData (mKeyblock, outCredentials.keyblock);
	CopyCCDataArray (mAddresses, outCredentials.addresses);
	CopyCCData (mTicket, outCredentials.ticket);
	CopyCCData (mSecondTicket, outCredentials.second_ticket);
	CopyCCDataArray (mAuthData, outCredentials.authdata);
	
	outCredentials.authtime = mAuthTime;
	outCredentials.starttime = mStartTime;
	outCredentials.endtime = mEndTime;
	outCredentials.renew_till = mRenewTill;
	outCredentials.is_skey = mIsSKey;
	outCredentials.ticket_flags = mTicketFlags;
}
#endif

void
CCICredentialsData::CCICredentialsV5Data::CopyString (
	const SharedString&			inSource,
	char*&						outDestination) const {
	
	outDestination = new char [inSource.length () + 1];
	inSource.copy (outDestination, inSource.length ());
	outDestination [inSource.length ()] = '\0';
}

void
CCICredentialsData::CCICredentialsV5Data::CopyCCData (
	const SharedCCData&			inSource,
	cc_data&					outDestination) const {
	
	outDestination.type = inSource.GetType ();
	outDestination.length = inSource.GetSize ();
	unsigned char* data = new unsigned char [inSource.GetSize ()];
	SharedCCData::const_iterator	iterator = inSource.begin ();
	for (; iterator < inSource.end (); iterator++) {
		data [iterator - inSource.begin ()] = *iterator;
	}
	outDestination.data = data;
}

void
CCICredentialsData::CCICredentialsV5Data::CopyCCDataArray (
	const SharedCCDataArray&	inSource,
	cc_data**&					outDestination) const {
	
	// Special case 0-length, because that's how it is in Kerberos...
	if (inSource.GetSize () == 0) {
		outDestination = nil;
		return;
	}
	
	outDestination = new cc_data* [inSource.GetSize () + 1];
	// Nil out the entire array, so that we can bail safely
	for (CCIUInt32 i = 0; i < inSource.GetSize () + 1; i++) {
		outDestination [i] = nil;
	}

	SharedCCDataArray::const_iterator		iterator = inSource.begin ();
	for (; iterator < inSource.end (); iterator++) {
		outDestination [iterator - inSource.begin ()] =
			new cc_data ();
		outDestination [iterator - inSource.begin ()] -> data = nil;
		CopyCCData (*(*iterator), *outDestination [iterator - inSource.begin ()]);
	}
}
