#include "CCache.config.h"

#include "CCIString.h"
#include "CCache.debug.h"
#include "CCIPointer.h"
#include "CCIException.h"

const	cc_string_f	CCIString::sFunctionTable = {
	CCEString::Release
};

cc_int32 CCEString::Release (
	cc_string_t		inString) {
	
	CCIResult	result = ccNoError;
	
	CCIBeginSafeTry_ {
		StString	string (inString);
		
		delete string.Get ();
	} CCIEndSafeTry_ (result, ccErrBadParam)
	
	CCIAssert_ ((result == ccNoError)
	            || (result == ccErrInvalidString));
	
	return result;
}

CCIString::CCIString (
	const std::string&			inString) {
	mString = inString;
	GetPublicData ().data = mString.c_str ();
}

CCIString::~CCIString () {
}

void CCIString::Validate () {

	CCIMagic <CCIString>::Validate ();
	CCIAssert_ (CCIUniqueInProcess <CCIString>::Valid ());
	
	CCIAssert_ ((CCIInternal <CCIString, cc_string_d>::Valid ()));
}

