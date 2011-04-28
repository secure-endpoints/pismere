#pragma once

#include "CCache.internal.h"

// 
// CCILeaks
//
// Template to track allocations and deallocations of classes exported
// from the API via opaque pointers. Note that if you inherit from CCILeaks
// in a new class, you have to instantiate the class in CCILeaks.cp
//

template <class T>
class CCILeaks {
	public:
		CCILeaks () {
#if CCI_DEBUG
			sObjectCounter++;
#endif
		}

		~CCILeaks () {
#if CCI_DEBUG
			sObjectCounter--;
#endif
		}
	private:
		static CCIUInt32	sObjectCounter;

		// Disallowed
		CCILeaks (const CCILeaks&);
		CCILeaks& operator = (const CCILeaks&);
};