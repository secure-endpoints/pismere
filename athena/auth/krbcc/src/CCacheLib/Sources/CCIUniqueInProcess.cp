#include "CCache.config.h"

#include "CCIUniqueInProcess.h"

template <class T>
typename CCIUniqueInProcess <T>::UniqueID	CCIUniqueInProcess <T>::sNextUniqueID		= 0;

#include "CCIContext.h"
#include "CCICCache.h"
#include "CCICCacheIterator.h"
#include "CCICredentialsIterator.h"
#include "CCIString.h"

CCIUniqueInProcess<CCIContext>::UniqueID CCIUniqueInProcess<CCIContext>::sNextUniqueID;
CCIUniqueInProcess<CCICCache>::UniqueID CCIUniqueInProcess<CCICCache>::sNextUniqueID;
CCIUniqueInProcess<CCICCacheIterator>::UniqueID CCIUniqueInProcess<CCICCacheIterator>::sNextUniqueID;
CCIUniqueInProcess<CCICredentials>::UniqueID CCIUniqueInProcess<CCICredentials>::sNextUniqueID;
CCIUniqueInProcess<CCICredentialsIterator>::UniqueID CCIUniqueInProcess<CCICredentialsIterator>::sNextUniqueID;
CCIUniqueInProcess<CCIString>::UniqueID CCIUniqueInProcess<CCIString>::sNextUniqueID;

#if CCache_v2_compat
CCIUniqueInProcess<CCICompatCredentials>::UniqueID CCIUniqueInProcess<CCICompatCredentials>::sNextUniqueID;
#endif
