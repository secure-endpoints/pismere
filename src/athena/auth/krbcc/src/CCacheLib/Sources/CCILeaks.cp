#include "CCache.config.h"

#include "CCILeaks.h"

template <class T>
CCIUInt32	CCILeaks <T>::sObjectCounter = 0;

#include "CCIContext.h"
#include "CCICCache.h"
#include "CCICCacheIterator.h"
#include "CCICredentialsIterator.h"
#include "CCIString.h"

CCIUInt32 CCILeaks <CCIContext>::sObjectCounter;
CCIUInt32 CCILeaks <CCICCache>::sObjectCounter;
CCIUInt32 CCILeaks <CCICredentials>::sObjectCounter;
CCIUInt32 CCILeaks <CCICompatCredentials>::sObjectCounter;
CCIUInt32 CCILeaks <CCICCacheIterator>::sObjectCounter;
CCIUInt32 CCILeaks <CCICredentialsIterator>::sObjectCounter;
CCIUInt32 CCILeaks <CCIString>::sObjectCounter;
