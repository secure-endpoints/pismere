#ifndef __KRBINTRN_H__
#define __KRBINTRN_H__

#include <cacheapi.h>

typedef struct {
    ccache_cit  *itCreds;
    ccache_p    *g_cache_ptr;
} THREAD_DATA;

#endif
