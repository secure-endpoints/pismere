#include <windows.h>
#include <stdio.h> // for _snprintf
#include "util.h"
#include "debug.h"
#include "secure.hxx"

void*
malloc_alloc_p(
    size_t size
    )
{
    return user_allocate(size);
}

void
free_alloc_p(
    void *pptr
    )
{
    void **real_pptr = (void**)pptr;
    DEBUG_ASSERT(real_pptr);
    if (*real_pptr) {
        user_free(*real_pptr);
        *real_pptr = 0;
    }
}

DWORD
alloc_textual_sid(
    PSID pSid,          // binary Sid
    LPSTR *pTextualSid  // buffer for Textual representaion of Sid
    )
{
    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSubAuthorities;
    DWORD dwSidRev = SID_REVISION;
    DWORD dwCounter;
    DWORD dwSidSize;

    *pTextualSid = 0;

    //
    // test if Sid passed in is valid
    //
    if(!IsValidSid(pSid)) return ERROR_INVALID_PARAMETER;

    // obtain SidIdentifierAuthority
    psia = GetSidIdentifierAuthority(pSid);
 
    // obtain sidsubauthority count
    dwSubAuthorities =* GetSidSubAuthorityCount(pSid);
 
    //
    // compute buffer length
    // S-SID_REVISION- + identifierauthority- + subauthorities- + NULL
    //
    dwSidSize = (15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);
    *pTextualSid = (LPSTR)malloc_alloc_p(dwSidSize);
    if (!*pTextualSid)
        return GetLastError();

    LPSTR TextualSid = *pTextualSid;

    //
    // prepare S-SID_REVISION-
    //
    wsprintf(TextualSid, TEXT("S-%lu-"), dwSidRev );
 
    //
    // prepare SidIdentifierAuthority
    //
    if ( (psia->Value[0] != 0) || (psia->Value[1] != 0) )
    {
        wsprintf(TextualSid + lstrlen(TextualSid),
                 TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
                 (USHORT)psia->Value[0],
                 (USHORT)psia->Value[1],
                 (USHORT)psia->Value[2],
                 (USHORT)psia->Value[3],
                 (USHORT)psia->Value[4],
                 (USHORT)psia->Value[5]);
    }
    else
    {
        wsprintf(TextualSid + lstrlen(TextualSid), TEXT("%lu"),
                 (ULONG)(psia->Value[5]      )   +
                 (ULONG)(psia->Value[4] <<  8)   +
                 (ULONG)(psia->Value[3] << 16)   +
                 (ULONG)(psia->Value[2] << 24)   );
    }
 
    //
    // loop through SidSubAuthorities
    //
    for (dwCounter=0 ; dwCounter < dwSubAuthorities ; dwCounter++)
    {
        wsprintf(TextualSid + lstrlen(TextualSid), TEXT("-%lu"),
                 *GetSidSubAuthority(pSid, dwCounter) );
    }
    return 0;
}

DWORD
alloc_token_user(
    HANDLE hToken,
    PTOKEN_USER *pptu
    )
{
    DWORD status = 0;
    DWORD size = 0;
    *pptu = 0;

    GetTokenInformation(hToken, TokenUser, *pptu, 0, &size);
    if (size == 0)
        CLEANUP_STATUS(GetLastError());

    if (!(*pptu = (PTOKEN_USER)malloc_alloc_p(size)))
        CLEANUP_STATUS(GetLastError());

    if (!GetTokenInformation(hToken, TokenUser, *pptu, size, &size))
        CLEANUP_STATUS(GetLastError());

 cleanup:
    if (status && *pptu) {
        free_alloc_p(pptu);
    }
    return status;
}

DWORD
alloc_username(
    PSID Sid,
    LPSTR* pname,
    LPSTR* pdomain = 0
    )
{
    DWORD status = 0;
    DWORD name_len = 0;
    DWORD domain_len = 0;
    SID_NAME_USE snu;
    LPSTR name = 0;
    LPSTR domain = 0;

    *pname = 0;
    if (pdomain) *pdomain = 0;

    LookupAccountSidA(NULL, Sid, 0, &name_len, 0, &domain_len, &snu);
    if ((name_len == 0) || (domain_len == 0))
        CLEANUP_STATUS(GetLastError());

    if (!(name = (LPSTR)malloc_alloc_p(name_len)))
        CLEANUP_STATUS(GetLastError());

    if (!(domain = (LPSTR)malloc_alloc_p(domain_len)))
        CLEANUP_STATUS(GetLastError());

    if (!LookupAccountSidA(NULL, Sid, name, &name_len, domain, &domain_len, 
                           &snu))
        CLEANUP_STATUS(GetLastError());

 cleanup:
    if (status) {
        if (name)
            free_alloc_p(&name);
        if (domain)
            free_alloc_p(&domain);
    } else {
        if (pdomain) {
            *pname = name;
            *pdomain = domain;
        } else {
            DWORD size = name_len + domain_len + 1;
            *pname = (LPSTR)malloc_alloc_p(size);
            if (!*pname)
                status = GetLastError();
            else
                _snprintf(*pname, size, "%s\\%s", name, domain);
        }
    }
    return status;
}

DWORD
get_authentication_id(
    HANDLE hToken,
    LUID* pAuthId
    )
{
    TOKEN_STATISTICS ts;
    DWORD len;

    if (!GetTokenInformation(hToken, TokenStatistics, &ts, sizeof(ts), &len))
        return GetLastError();
    *pAuthId = ts.AuthenticationId;
    return 0;
}

DWORD
alloc_name_9x(
    LPSTR* pname,
    LPSTR postfix
    )
{
    char prefix[] = "krbcc";
    DWORD len = (sizeof(prefix) - 1) + 1 + strlen(postfix) + 1;

    *pname = (LPSTR)malloc_alloc_p(len);
    if (!*pname) return GetLastError();
    _snprintf(*pname, len, "%s.%s", prefix, postfix);
    return 0;
}

DWORD
alloc_name_NT(
    LPSTR* pname,
    LPSTR postfix
    )
{
    DWORD status = 0;
    HANDLE hToken = 0;
    PTOKEN_USER ptu = 0;
    LUID auth_id;
    DWORD len = 0;
    LPSTR name = 0;
    LPSTR domain = 0;
    LPSTR sid = 0;
    char prefix[] = "krbcc";
    char lid_buffer[80];

    *pname = 0;

    status = SecureClient::Token(hToken);
    CLEANUP_ON_STATUS(status);
    status = get_authentication_id(hToken, &auth_id);
    CLEANUP_ON_STATUS(status);
    status = alloc_token_user(hToken, &ptu);
    CLEANUP_ON_STATUS(status);
    status = alloc_username(ptu->User.Sid, &name, &domain);
    CLEANUP_ON_STATUS(status);
    status = alloc_textual_sid(ptu->User.Sid, &sid);
    CLEANUP_ON_STATUS(status);

    _snprintf(lid_buffer, sizeof(lid_buffer), "%I64u", auth_id);

    len = (sizeof(prefix) - 1) + 1 + strlen(domain) + 1 + strlen(name) + 1 + 
        strlen(sid) + 1 + strlen(lid_buffer) + 1 + strlen(postfix) + 1;
    *pname = (LPSTR)malloc_alloc_p(len);
    if (!*pname)
        CLEANUP_STATUS(GetLastError());

    _snprintf(*pname, len, "%s.%s.%s.%s.%s.%s", prefix, domain, name, sid, 
              lid_buffer, postfix);

 cleanup:
    if (sid)
        free_alloc_p(&sid);
    if (name)
        free_alloc_p(&name);
    if (domain)
        free_alloc_p(&domain);
    if (ptu)
        free_alloc_p(&ptu);
    if (hToken && hToken != INVALID_HANDLE_VALUE)
        CloseHandle(hToken);
    if (status && *pname)
        free_alloc_p(pname);
    return status;
}

DWORD
alloc_name(
    LPSTR* pname,
    LPSTR postfix,
    BOOL isNT
    )
{
    return isNT ? alloc_name_NT(pname, postfix) : 
        alloc_name_9x(pname, postfix);
}

DWORD
alloc_own_security_descriptor_NT(
    PSECURITY_DESCRIPTOR* ppsd
    )
{
    DWORD status = 0;
    HANDLE hToken = 0;
    PTOKEN_USER ptu = 0;
    PSID pSid = 0;
    PACL pAcl = 0;
    DWORD size = 0;
    SECURITY_DESCRIPTOR sd;

    *ppsd = 0;

    status = SecureClient::Token(hToken);
    CLEANUP_ON_STATUS(status);

    // Get SID:
    status = alloc_token_user(hToken, &ptu);
    CLEANUP_ON_STATUS(status);

    size = GetLengthSid(ptu->User.Sid);
    pSid = (PSID) malloc_alloc_p(size);
    if (!pSid)
        CLEANUP_STATUS(GetLastError());
    if (!CopySid(size, pSid, ptu->User.Sid))
        CLEANUP_STATUS(GetLastError());

    // Prepare ACL:
    size = sizeof(ACL);
    // Add an ACE:
    size += sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
        + GetLengthSid(pSid);
    pAcl = (PACL) malloc_alloc_p(size);
    if (!pAcl)
        CLEANUP_STATUS(GetLastError());

    if (!InitializeAcl(pAcl, size, ACL_REVISION))
        CLEANUP_STATUS(GetLastError());
    if (!AddAccessAllowedAce(pAcl, ACL_REVISION, GENERIC_ALL, pSid))
        CLEANUP_STATUS(GetLastError());

    // Prepare SD itself:
    if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
        CLEANUP_STATUS(GetLastError());

    if (!SetSecurityDescriptorDacl(&sd, TRUE, pAcl, FALSE))
        CLEANUP_STATUS(GetLastError());

    if (!SetSecurityDescriptorOwner(&sd, pSid, FALSE))
        CLEANUP_STATUS(GetLastError());

    if (!IsValidSecurityDescriptor(&sd))
        CLEANUP_STATUS(ERROR_INVALID_PARAMETER);

    // We now have a SD.  Let's copy it.
    {
        // This should not succeed.  Instead it should give us the size.
        BOOL ok = MakeSelfRelativeSD(&sd, 0, &size);
        DEBUG_ASSERT(!ok);
    }
    if (size == 0)
        CLEANUP_STATUS(GetLastError());
    *ppsd = (PSECURITY_DESCRIPTOR) malloc_alloc_p(size);
    if (!*ppsd)
        CLEANUP_STATUS(GetLastError());
    if (!MakeSelfRelativeSD(&sd, *ppsd, &size))
        CLEANUP_STATUS(GetLastError());

 cleanup:
    if (ptu)
        free_alloc_p(&ptu);
    if (pSid)
        free_alloc_p(&pSid);
    if (pAcl)
        free_alloc_p(&pAcl);
    if (hToken && hToken != INVALID_HANDLE_VALUE)
        CloseHandle(hToken);
    if (status && *ppsd)
        free_alloc_p(ppsd);
    return status;
}

DWORD
alloc_module_file_name(
    char* module,
    char** pname
    )
{
    const DWORD max = 8192;
    DWORD status = 0;
    DWORD got = 0;
    DWORD size = 512; // use low number to test...
    HMODULE h = 0;
    BOOL ok = FALSE;
    char* name = 0;

    if (!pname)
        return ERROR_INVALID_PARAMETER;
    *pname = 0;

    h = GetModuleHandle(module);

    if (!h)
        return GetLastError();

    // We assume size < max and size > 0
    while (!status && !ok) {
        if (size > max) {
            // XXX - Assert?
            status = ERROR_INVALID_DATA;
            continue;
        }
        if (name) free_alloc_p(&name);
        name = (char*)malloc_alloc_p(size + 1);
        if (!name) {
            status = ERROR_NOT_ENOUGH_MEMORY;
            continue;
        }
        name[size] = 0;
        got = GetModuleFileName(h, name, size);
        if (!got) {
            status = GetLastError();
            // sanity check:
            if (!status) {
                // XXX - print nasty message...assert?
                status = ERROR_INVALID_DATA;
            }
            continue;
        }
        // To know we're ok, we need to verify that what we got
        // was bigger than GetModuleSize thought it got.
        ok = got && (got < size) && !name[got];
        size *= 2;
    }
    if (status && name)
        free_alloc_p(&name);
    else
        *pname = name;
    return status;
}

DWORD
alloc_module_dir_name(
    char* module,
    char** pname
    )
{
    DWORD status = alloc_module_file_name(module, pname);
    if (!status) {
        char* name = *pname;
        char* p = name + strlen(name);
        while ((p >= name) && (*p != '\\') && (*p != '/')) p--;
        if (p < name) {
            free_alloc_p(pname);
            status = ERROR_INVALID_DATA;
        } else {
            *p = 0;
        }
    }
    return status;
}

DWORD
alloc_module_dir_name_with_file(
    char* module,
    char* file,
    char** pname
    )
{
    DWORD status = alloc_module_dir_name(module, pname);
    if (!status) {
        char* name = *pname;
        size_t name_size = strlen(name);
        size_t size = name_size + 1 + strlen(file) + 1;
        char* result = (char*)malloc_alloc_p(size);
        if (!result) {
            status = ERROR_NOT_ENOUGH_MEMORY;
            free_alloc_p(pname);
        } else {
            strcpy(result, name);
            result[name_size] = '\\';
            strcpy(result + name_size + 1, file);
            free_alloc_p(pname);
            *pname = result;
        }
    }
    return status;
}
