#include "CCache.config.h"

#include "CCIUniqueGlobally.h"

#if CCache_ContainsSharedStaticData
template <class T>
CCISharedStaticData <typename CCIUniqueGlobally <T>::Globals>	CCIUniqueGlobally <T>::sGlobals;
#endif

template <class T>
CCISharedStaticDataProxy <typename CCIUniqueGlobally <T>::Globals>
		CCIUniqueGlobally <T>::sGlobalsProxy = CCIUniqueGlobally <T>::sGlobals;

#include "CCICCacheData.h"
#include "CCIContextData.h"
#include "CCICredentialsData.h"

#if CCache_ContainsSharedStaticData
CCISharedStaticData <CCIUniqueGlobally <CCICCacheData>::Globals>	CCIUniqueGlobally <CCICCacheData>::sGlobals;
CCISharedStaticData <CCIUniqueGlobally <CCIContextData>::Globals>	CCIUniqueGlobally <CCIContextData>::sGlobals;
CCISharedStaticData <CCIUniqueGlobally <CCICredentialsData>::Globals>	CCIUniqueGlobally <CCICredentialsData>::sGlobals;
#endif

CCISharedStaticDataProxy <CCIUniqueGlobally <CCICCacheData>::Globals> CCIUniqueGlobally <CCICCacheData>::sGlobalsProxy = CCIUniqueGlobally <CCICCacheData>::sGlobals;
CCISharedStaticDataProxy <CCIUniqueGlobally <CCIContextData>::Globals> CCIUniqueGlobally <CCIContextData>::sGlobalsProxy = CCIUniqueGlobally <CCIContextData>::sGlobals;
CCISharedStaticDataProxy <CCIUniqueGlobally <CCICredentialsData>::Globals> CCIUniqueGlobally <CCICredentialsData>::sGlobalsProxy = CCIUniqueGlobally <CCICredentialsData>::sGlobals;
