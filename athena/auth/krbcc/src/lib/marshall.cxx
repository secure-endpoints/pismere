#include <ccapi.h>
#include <cacheapi.h>
#include <stdio.h>
#include <marshall.hxx>
#include <stddef.h>
#include "debug.h"
#include <new>

namespace Marshall {
    static void* Alloc(size_t size);
    static void* SafeAlloc(size_t size);
    static void Free(void* ptr);
}

static
void*
Marshall::Alloc(size_t size)
{
    void* ptr = midl_user_allocate(size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

static
void*
Marshall::SafeAlloc(size_t size)
{
    void* ptr = Marshall::Alloc(size);
    memset(ptr, 0, size);
    return ptr;
}

static
void
Marshall::Free(void* ptr)
{
    midl_user_free(ptr);
}

///////////////////////////////////////////////////////////////////////////////
#if 0
template<>
void
Marshall::convertfunc<CC_CHAR*, const char*>(
    CC_CHAR*& to,
    const char*& from
    )
{
}
template<>
void
Marshall::freefunc<CC_CHAR*>(
    CC_CHAR*& s
    )
{
    if (s) {
        Marshall::Free(s);
        s = 0;
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////
#if 0
#define Marshall_DEFINE_convert(a, b) void Marshall::convert(a& to, b& from)

Marshall_DEFINE_convert(
    CC_CHAR*,
    const char*
    )
{
    copy_string(to, from);
}
#endif
///////////////////////////////////////////////////////////////////////////////

Marshall::Exception::CcError(
    )
{
    switch(type) {
    case ME_NOERROR:
        return CC_NOERROR;
    case ME_INVALID:
        return CC_BAD_PARM;
    case ME_NO_MEM:
        return CC_NOMEM;
    default:
        // If we get here, we have a bug...
        DEBUG_PRINT((D_BUG "Unrecognized Marshall::Exception: %u\n", type));
        DEBUG_ASSERT(false && "Unrecognized Marshall::Exception");
        return CC_NOT_SUPP;
    }
}

///////////////////////////////////////////////////////////////////////////////

template <typename T1, typename T2>
static
void
copy_fixed_string(
    T1& to, 
    T2& from
    )
{
    size_t const N(sizeof(to) / sizeof(to[0]));
    strncpy((char*)to, (char*)from, N);
    to[N-1] = 0;
}

template <typename T1, typename T2>
static
void
copy_string(
    T1& to, 
    T2 from
    )
{
    if (from) {
        size_t const N(strlen((const char*)from) + 1);
        to = (T1)Marshall::Alloc(N);
        memcpy(to, from, N);
    } else {
        to = 0;
    }
}

template <typename T>
static
void
free_string(
    T*& data
    )
{
    if (data) {
        Marshall::Free(data);
        data = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////

void
Marshall::convert(
    CC_CHAR*& to,
    const char*& from
    )
{
    copy_string(to, from);
}

void
Marshall::free_convert(
    CC_CHAR*& data
    )
{
    free_string(data);
}

void
Marshall::convert(
    char*& to,
    const CC_CHAR*& from
    )
{
    copy_string(to, from);
}

void
Marshall::free_convert(
    char*& data
    )
{
    free_string(data);
}

///////////////////////////////////////////////////////////////////////////////

template <typename T1, typename T2>
static
void
copy_data(
    T1& to,
    T2& from
    )
{
    to.type = from.type;
    to.length = from.length;
    to.data = 0;
    if (to.length) {
        to.data = (unsigned char*)Marshall::Alloc(to.length);
        memcpy(to.data, from.data, to.length);
    }
}

template <typename T>
static
void
free_data(
    T& to
    )
{
    if (to.data) Marshall::Free(to.data);
    to.data = 0;
    to.length = 0;
}

template <typename T1, typename T2>
static
void
copy_v4(
    T1 to,
    T2 from
    )
{
    to->kversion = from->kversion;
    copy_fixed_string(to->principal, from->principal);
    copy_fixed_string(to->principal_instance, from->principal_instance);
    copy_fixed_string(to->service, from->service);
    copy_fixed_string(to->service_instance, from->service_instance);
    copy_fixed_string(to->realm, from->realm);
    memcpy(to->session_key, from->session_key, sizeof(to->session_key));
    to->kvno = from->kvno;
    to->str_to_key = from->str_to_key;
    to->issue_date = from->issue_date;
    to->lifetime = from->lifetime;
    to->address = from->address;
    to->ticket_sz = from->ticket_sz;
    memcpy(to->ticket, from->ticket, min(sizeof(to->ticket), 
                                         to->ticket_sz));
}

static
void
copy_data_list(
    CC_DATA_LIST& to,
    cc_data** from
    )
{
    to.count = 0;
    to.data = 0;
    if (from) {
        while(from[to.count])
            to.count++;
        if (to.count) {
            to.data = (CC_DATA*) Marshall::SafeAlloc(sizeof(CC_DATA) * 
                                                     to.count);
            for (unsigned int i = 0; i < to.count; i++)
                copy_data(to.data[i], *from[i]);
        }
    }
}

static
void
free_data_list(
    CC_DATA_LIST& list
    )
{
    for (unsigned int i = 0; i < list.count; i++)
        free_data(list.data[i]);
    Marshall::Free(list.data);
    list.data = 0;
    list.count = 0;
}

static
void
copy_data_list(
    cc_data**& to,
    const CC_DATA_LIST& from
    )
{
    to = 0;
    if (from.count) {
        to = (cc_data**) Marshall::SafeAlloc(sizeof(cc_data*) * 
                                             (from.count + 1));
        to[from.count] = 0;
        for (unsigned int i = 0; i < from.count; i++) {
            to[i] = (cc_data*) Marshall::SafeAlloc(sizeof(cc_data));
            copy_data(*to[i], from.data[i]);
        }
    }
}

static
void
free_data_list(
    cc_data**& list
    )
{
    if (list) {
        unsigned int i = 0;
        while (list[i]) {
            free_data(*list[i]);
            Marshall::Free(list[i]);
            list[i] = 0;
            i++;
        }
        Marshall::Free(list[i]);
        Marshall::Free(list);
        list = 0;
    }
    
}

template <typename T, typename F, typename TV4, typename TV5, 
    typename FV4, typename FV5>
static
void
copy_creds(
    T*& to,
    const F& from
    )
{
    to = 0;
    try {
        switch(from.cred_type) {
        case CC_CRED_V4:
        {
            to = (T*)Marshall::SafeAlloc(sizeof(T));
            to->cred.pV4Cred = (TV4*)Marshall::SafeAlloc(sizeof(TV4));
            to->cred_type = from.cred_type;
            TV4* to2 = to->cred.pV4Cred;
            FV4* from2 = from.cred.pV4Cred;
            copy_v4(to2, from2);
            return;
        }
        case CC_CRED_V5:
        {
            to = (T*)Marshall::SafeAlloc(sizeof(T));
            to->cred.pV5Cred = (TV5*)Marshall::SafeAlloc(sizeof(TV5));
            to->cred_type = from.cred_type;
            TV5* to2 = to->cred.pV5Cred;
            FV5* from2 = from.cred.pV5Cred;
            copy_string(to2->client, from2->client);
            copy_string(to2->server, from2->server);
            copy_data(to2->keyblock, from2->keyblock);
            to2->authtime     = from2->authtime;
            to2->starttime    = from2->starttime;
            to2->endtime      = from2->endtime;
            to2->renew_till   = from2->renew_till;
            to2->is_skey      = from2->is_skey;
            to2->ticket_flags = from2->ticket_flags;
            copy_data_list(to2->addresses, from2->addresses);
            copy_data(to2->ticket, from2->ticket);
            copy_data(to2->second_ticket, from2->second_ticket);
            copy_data_list(to2->authdata, from2->authdata);
            return;
        }
        default:
            throw Marshall::Exception(Marshall::ME_INVALID);
        }
        throw Marshall::Exception(Marshall::ME_INVALID);
    } catch (std::bad_alloc&) {
        if (to) Marshall::Free(to);
        throw Marshall::Exception(Marshall::ME_NO_MEM);
    }
}

template <typename T, typename V4, typename V5>
static
void
free_creds(
    T*& creds
    )
{
    if (!creds) return;

    switch(creds->cred_type) {
    case CC_CRED_V4:
    {
        V4* to = creds->cred.pV4Cred;
        if (to) Marshall::Free(to);
        break;
    }
    case CC_CRED_V5:
    {
        V5* to = creds->cred.pV5Cred;
        if (to) {
            free_string(to->client);
            free_string(to->server);
            free_data(to->keyblock);
            free_data_list(to->addresses);
            free_data(to->ticket);
            free_data(to->second_ticket);
            free_data_list(to->authdata);
            Marshall::Free(to);
        }
        break;
    }
    default:
        throw Marshall::Exception(Marshall::ME_INVALID);
    }
    Marshall::Free(creds);
    creds = 0;
    return;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
namespace Magic {
    enum magic_t {
        min = 4321,
        marshall_creds = min,
        unmarshall_creds,
        marshall_nc_info,
        unmarshall_nc_info,
        max = unmarshall_nc_info
    };
}

template <typename T>
class Data {
public:
    Magic::magic_t magic;
    T data;
    Data(Magic::magic_t m):magic(m) { Validate(); }
    static Data<T>* Internalize(void* opaque) {
        Data<T>* d = reinterpret_cast<Data<T>*> (
                reinterpret_cast<char*>(opaque) - offsetof(Data<T>, data));
        d->Validate();
        return d;
    }
private:
    void Validate() {
        if (magic < Magic::min || magic > Magic::max)
            throw Marshall::Exception(Marshall::ME_INVALID);
    }
};

void
Marshall::free_ex(
    void* data
    )
{
    Data<void*>* D = Data<void*>::Internalize(data);
    switch(D->magic) {
    case Magic::marshall_creds:
    {
        typedef CRED_UNION type_t;
        Data<type_t>* d = Data<type_t>::Internalize(data);
        type_t& creds = d->data;
        switch(creds.cred_type) {
        case CC_CRED_V4:
        {
            Marshall::Free(creds.cred.pV4Cred);
            break;
        }
        case CC_CRED_V5:
        {
            break;
        }
        default:
            throw Marshall::Exception(Marshall::ME_INVALID);
        }
        delete d;
        return;
    }
    case Magic::unmarshall_creds:
    {
        typedef cred_union type_t;
        Data<type_t>* d = Data<type_t>::Internalize(data);
        type_t& creds = d->data;
        switch(creds.cred_type) {
        case CC_CRED_V4:
        {
            Marshall::Free(creds.cred.pV4Cred);
            break;
        }
        case CC_CRED_V5:
        {
            break;
        }
        default:
            throw Marshall::Exception(Marshall::ME_INVALID);
        }
        delete d;
        return;
    }
    case Magic::marshall_nc_info:
    case Magic::unmarshall_nc_info:
    default:
        throw Marshall::Exception(Marshall::ME_INVALID);
    }
    throw Marshall::Exception(Marshall::ME_INVALID);
}

#endif

///////////////////////////////////////////////////////////////////////////////

void
Marshall::convert(
    CRED_UNION*& rcreds,
    const cred_union& creds
    )
{
    copy_creds<CRED_UNION, cred_union, V4_CRED, V5_CRED, V4Cred_type, cc_creds>(rcreds, creds);
}

void
Marshall::convert(
    cred_union*& creds,
    const CRED_UNION& rcreds
    )
{
    copy_creds<cred_union, CRED_UNION, V4Cred_type, cc_creds, V4_CRED, V5_CRED>(creds, rcreds);
}

void
Marshall::free_convert(
    CRED_UNION*& creds
    )
{
    free_creds<CRED_UNION, V4_CRED, V5_CRED>(creds);
}

void
Marshall::free_convert(
    cred_union*& creds
    )
{
    free_creds<cred_union, V4Cred_type, cc_creds>(creds);
}

///////////////////////////////////////////////////////////////////////////////

void
Marshall::convert(
    NC_INFO_LIST*& rinfo,
    const infoNC* const *& info
    )
{
    rinfo = 0;

    try {
        CC_UINT32 n = 0;
        while(info[n])
            n++;

        rinfo = (NC_INFO_LIST*) Marshall::Alloc(sizeof(NC_INFO_LIST));
        rinfo->length = n;
        rinfo->info = 0;

        if (rinfo->length) {
            rinfo->info = (NC_INFO*) Marshall::SafeAlloc(rinfo->length * 
                                                         sizeof(NC_INFO));
            for (n = 0; n < rinfo->length; n++)
            {
                copy_string(rinfo->info[n].name, info[n]->name);
                copy_string(rinfo->info[n].principal, info[n]->principal);
                rinfo->info[n].vers = info[n]->vers;
            }
        }
    } catch (std::bad_alloc&) {
        if (rinfo) Marshall::free_convert(rinfo);
        throw Marshall::Exception(Marshall::ME_NO_MEM);
    }
}

void
Marshall::free_convert(
    NC_INFO_LIST*& rinfo
    )
{
    if (!rinfo) return;
    CC_UINT32 n;
    for(n = 0; n < rinfo->length; n++) {
        free_string(rinfo->info[n].name);
        free_string(rinfo->info[n].principal);
        Marshall::Free(rinfo->info);
    }
    Marshall::Free(rinfo);
    rinfo = 0;
}

void
Marshall::convert(
    infoNC**& info,
    const NC_INFO_LIST& rinfo
    )
{
    info = 0;
    try {
        info = (infoNC**) Marshall::SafeAlloc((rinfo.length+1) * 
                                              sizeof(infoNC*));
        CC_UINT32 n;
        for (n = 0; n < rinfo.length; n++) {
            info[n] = (infoNC*) Marshall::SafeAlloc(sizeof(infoNC));
            copy_string(info[n]->name, rinfo.info[n].name);
            copy_string(info[n]->principal, rinfo.info[n].principal);
            info[n]->vers = rinfo.info[n].vers;
        }
        info[n] = 0;
    } catch (std::bad_alloc&) {
        if (info) Marshall::free_convert(info);
        throw Marshall::Exception(Marshall::ME_NO_MEM);
    }
}

void
Marshall::free_convert(
    infoNC**& info
    )
{
    if (!info) return;
    CC_UINT32 n;
    for(n = 0; info[n]; n++) {
        infoNC* i = info[n];
        free_string(i->name);
        free_string(i->principal);
        Marshall::Free(i);
    }
    Marshall::Free(info);
    info = 0;
}
