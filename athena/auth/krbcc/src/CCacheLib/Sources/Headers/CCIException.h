/*
 * CCIException.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCIException.h,v 1.4 2000/06/16 21:44:07 dalmeida Exp $
 */
 
#include "CCache.internal.h"

#pragma once

class CCIException {
	public:
		CCIException	(CCIResult	inError):
			mError (inError) {}
		
		CCIResult	Error () const { return mError; }
	
	private:
		CCIResult		mError;
};