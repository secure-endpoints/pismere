#pragma once

namespace Marshall {
#if 0
}
#endif

enum Error_t {
    ME_NOERROR = 0,
    ME_INVALID,
    ME_NO_MEM
};

class Exception {
public:
    Error_t type;
    Exception(Error_t t):type(t) {};
    CC_INT32 CcError();
};

// NOTE: Due to template weirdness in VC++ 6, we can't do the template
// below.
#if 0
template<typename T1, typename T2>
void
convertfunc(
    T1& to,
    T2& from
    );
template<typename T>
void
freefunc(
    T&
    );
#endif

#define Marshall_DECLARE_PAIR(a, b) \
    void convert(a&, b&); \
    void free_convert(a&)

Marshall_DECLARE_PAIR(CC_CHAR*,  char*);
Marshall_DECLARE_PAIR(char*,  CC_CHAR*);
Marshall_DECLARE_PAIR(infoNC**,  NC_INFO_LIST);
Marshall_DECLARE_PAIR(NC_INFO_LIST*,  infoNC* *);
Marshall_DECLARE_PAIR(cred_union*,  CRED_UNION);
Marshall_DECLARE_PAIR(CRED_UNION*,  cred_union);

Marshall_DECLARE_PAIR(CC_CHAR*,  const char*);
Marshall_DECLARE_PAIR(char*,  const CC_CHAR*);
Marshall_DECLARE_PAIR(infoNC**,  const NC_INFO_LIST);
Marshall_DECLARE_PAIR(NC_INFO_LIST*,  const infoNC* *);
Marshall_DECLARE_PAIR(cred_union*,  const CRED_UNION);
Marshall_DECLARE_PAIR(CRED_UNION*,  const cred_union);

template <typename T1, typename T2>
int
safe_convert(T1& to, T2& from)
{
    try {
        Marshall::convert(to, from);
    } catch (Marshall::Exception& e) {
        return e.CcError();
    }
    return 0;
}

template <typename T>
int
safe_free(T& data)
{
    try {
        Marshall::free_convert(data);
    } catch (Marshall::Exception& e) {
        return e.CcError();
    }
    return 0;
}

#if 0
{
#endif
}
