#include <windows.h>
#include "conf.h"
#include <krb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
///#include <direct.h>
#include <winkrbid.h>
#include <assert.h>

#define ASSERT assert

// Note: This must match the DEF file entries
#define KRB4_MODULE_NAME "KRBV4W32"

#define CHECK_BUFFER(buffer, size, need) \
    ((buffer) && (((size) >= (need)) || ((size) == (size_t)-1)))

__inline
static BOOL
get_sub_reg_config(
    char* buffer,
    size_t size,
    size_t* plen,
    char* default_config_file,
    HKEY hBaseKey,
    char* reg_key,
    char* reg_value,
    char* reg_value_dir
    )
{
    HKEY hKey;
    LONG ret;

    ASSERT(plen);

    if (RegOpenKeyEx(hBaseKey,
                     reg_key, 0, KEY_QUERY_VALUE, &hKey)
        == ERROR_SUCCESS)
    {
        ret = RegQueryValueEx(hKey, reg_value, 0, 0, buffer, plen);
        if (!ret || (ret == ERROR_MORE_DATA))
        {
            RegCloseKey(hKey);
            return TRUE;
        }
        // We did not find the value, so try other dir one.
        *plen = size;
        ret = RegQueryValueEx(hKey, reg_value_dir, 0, 0, buffer, plen);
        RegCloseKey(hKey);
        // do we have an unknown error?
        if (!ret || (ret == ERROR_MORE_DATA))
        {
            // note: plen already includes a byte for NULL, we make it a 
            // backslash:
            *plen += 1 + strlen(default_config_file);
            if (!ret && CHECK_BUFFER(buffer, size, *plen))
            {
                strcat(buffer, "\\");
                strcat(buffer, default_config_file);
            }
            return TRUE;
        }
    }
    return FALSE;
}

__inline
static BOOL
get_reg_config(
    char* buffer,
    size_t size,
    size_t* plen,
    char* default_config_file,
    char* reg_key,
    char* reg_value,
    char* reg_value_dir
    )
{
    return (get_sub_reg_config(buffer, size, plen, default_config_file, 
                               HKEY_CURRENT_USER, 
                               reg_key, reg_value, reg_value_dir)
            ||
            get_sub_reg_config(buffer, size, plen, default_config_file, 
                               HKEY_LOCAL_MACHINE, 
                               reg_key, reg_value, reg_value_dir)
            );
}

__inline
static BOOL
get_env_config(
    char* buffer,
    size_t size,
    size_t* plen,
    char* default_config_file,
    char* env_var,
    char* env_var_dir
    )
{
    char* cp;

    ASSERT(plen);

    if (cp = getenv(env_var))
    {
        *plen = strlen(cp) + 1;
        if (CHECK_BUFFER(buffer, size, *plen))
            strcpy(buffer, cp);
        return TRUE;
    }
    else if (cp = getenv(env_var_dir))
    {
        *plen = strlen(cp) + 1 + strlen(default_config_file) + 1;
        if (CHECK_BUFFER(buffer, size, *plen)) {
            strcpy(buffer, cp);
            strcat(buffer, "\\");
            strcat(buffer, default_config_file);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

__inline
static BOOL
krb_try(
    char* buffer,
    size_t size,
    size_t* plen,
    const char* env_str,
    const char* mid_str,
    const char* def_str
    )
{
    FILE* fp;
    char* cp;
    char* buf = 0;
    BOOL found = FALSE;

    ASSERT(plen);

    if ((cp = getenv(env_str)) &&
        (*plen = (strlen(cp) + strlen(mid_str) + 
                  strlen(def_str) + 1)) &&
        (buf = CHECK_BUFFER(buffer, size, *plen)?buffer:malloc(*plen)) &&
        strcpy(buf, cp) &&
        strcat(buf, mid_str) &&
        strcat(buf, def_str) &&
        (fp = fopen(buf, "r")))
    {
        fclose(fp);
        found = TRUE;
    }
    if (buf && (buf != buffer))
        free(buf);
    return found;
}

__inline
static BOOL
krb_search(
    char* config_path,
    size_t size,
    size_t* plen,
    char* filename
    )
{
    HFILE    hf;
    OFSTRUCT ofs;

    ASSERT(plen);

    if ((hf = OpenFile(filename, &ofs, OF_EXIST)) != HFILE_ERROR)
    {
        // NOTE: We must not _lclose(hf) becasue OF_EXIST automagically
        // closes the file for us!
        *plen = strlen(ofs.szPathName) + 1;
        if (CHECK_BUFFER(config_path, size, *plen)) {
            strcpy(config_path, ofs.szPathName);
        }
        return TRUE;
    }
    return FALSE;
}

__inline
static char *
old_config(
    LPSTR config_path,
    size_t size,
    size_t* plen,
    char* default_config_file,
    UINT resource_id
    )
{
    char     def_config_file[_MAX_PATH];
    HMODULE  hModKRB;

    ASSERT(plen);

    hModKRB = GetModuleHandle(KRB4_MODULE_NAME);
    if(!LoadString(hModKRB, resource_id,
                   def_config_file, sizeof(def_config_file)))
    {
	strncpy(def_config_file, default_config_file, _MAX_PATH);
        def_config_file[_MAX_PATH - 1] = 0;
    }

    /* try to guess the path */
    if (krb_try(config_path, size, plen, "NDIR", "\\kerb\\", 
                def_config_file) ||
        krb_search(config_path, size, plen, def_config_file) ||
        krb_try(config_path, size, plen, "NDIR", "\\", def_config_file) ||
        krb_try(config_path, size, plen, "ETC", "\\", def_config_file))
    {
        return CHECK_BUFFER(config_path, size, *plen)?config_path:0;
    }
    else
    {
        // We didn't find it
        *plen = 0;
#ifdef USE_DNS 
        krb_set_use_dns(2);
#endif /* USE_DNS */
	return 0;
    }
}

#define KRB_REG_KEY "Software\\MIT\\Kerberos4"
#define KRB_CONFIG_ENV "KRB4_CONFIG"
#define KRB_CONFIG_REG_VALUE "config"

#define KRB_CONF_ENV "KRB4_KRB.CONF"
#define KRB_CONF_REG_VALUE "krb.conf"
#define KRB_REALMS_ENV "KRB4_KRB.REALMS"
#define KRB_REALMS_REG_VALUE "krb.realms"

static char *
get_config(
    char* buffer,
    size_t* plen,
    char* def_fname,
    char* env_var,
    char* reg_value,
    UINT resource_id
    )
{
    size_t size;
    ASSERT(plen);
    size = *plen;
    if(get_env_config(buffer, size, plen, def_fname, env_var, 
                      KRB_CONFIG_ENV) ||
       get_reg_config(buffer, size, plen, def_fname, KRB_REG_KEY, reg_value, 
                      KRB_CONFIG_REG_VALUE))
    {
        return CHECK_BUFFER(buffer, size, *plen)?buffer:0;
    } else {
        return old_config(buffer, size, plen, def_fname, resource_id);
    }
}

char *
krb_get_krbconf2(
    LPSTR config_path,
    size_t* plen
    )
{
    return get_config(config_path, plen, 
                      KRB_CONF,
                      KRB_CONF_ENV,
                      KRB_CONF_REG_VALUE,
                      IDS_KRB_CONF);
}
 
char *
krb_get_krbrealm2(
    LPSTR config_path,
    size_t* plen
    )
{
    return get_config(config_path, plen, 
                      KRB_RLM_TRANS,
                      KRB_REALMS_ENV,
                      KRB_REALMS_REG_VALUE,
                      IDS_KRB_RLM_TRANS);
}

char *
krb_get_krbconf(
    LPSTR config_path
    )
{
    size_t size = _MAX_PATH;
    return krb_get_krbconf2(config_path, &size);
}
 
char *
krb_get_krbrealm(
    LPSTR config_path
    )
{
    size_t size = _MAX_PATH;
    return krb_get_krbrealm2(config_path, &size);
}

#ifdef USE_DNS
/* The following functions are used to determine whether DNS lookups */
/* should be used or not.   use_dns is a flag that is set to one of  */
/* the following values:                                             */
/*   -1 = not yet initialized                                        */
/*    0 = do not use DNS                                             */
/*    1 = use DNS as per KRB.CON setting                             */
/*    2 = use DNS since KRB.CON could not be found                   */

static char *
parse_str(
    char *buffer,
    char *result
    )
{
    if (!buffer)
        goto cleanup;

    while (*buffer && isspace(*buffer))
        buffer++;
    while (*buffer && !isspace(*buffer))
        *result++=*buffer++;

    cleanup:
    *result='\0';
    return buffer;
}

static int use_dns = -1;

int
krb_set_use_dns(int x) {
    use_dns = x;
    return(1);
}

int 
krb_use_dns(void) {
    char tr[REALM_SZ];
    char th[MAX_HSTNM];
    FILE *cnffile = 0;
    char linebuf[BUFSIZ];
    char *p;
    char *conf_fn = 0;
    size_t conf_sz = 0;

    /* if we know to use or not use DNS return immediately */
    if ( use_dns >= 0 && use_dns <= 2 )
        return use_dns;

    use_dns = -1;

    krb_get_krbconf2(conf_fn, &conf_sz);
    conf_fn = malloc(conf_sz);
    if (!conf_fn || !krb_get_krbconf2(conf_fn, &conf_sz))
        goto cleanup;

    memset(linebuf, 0, sizeof(linebuf));
    if ((cnffile = fopen(conf_fn, "r")) == NULL) {
        use_dns = 2;
        goto cleanup;
    }
    if (fgets(linebuf, sizeof(linebuf), cnffile) == NULL) {
        goto cleanup;
    }
    /* Ignore the first line as it contains the default realm (if any) */
    /* run through the file, looking for the realm ".KERBEROS.OPTION." */
    /* with a host of "dns"                                            */
    for (;;) {
        if (fgets(linebuf, sizeof(linebuf), cnffile) == NULL) {
            goto cleanup;
        }
        memset(tr, 0, sizeof(tr));
        p = parse_str(linebuf, tr);
        if (*tr == '\0')
            continue;
        memset(th, 0, lstrlen(th));
        parse_str(p,th);
        if (*tr == '\0')
            continue;
        if (!lstrcmp(tr,".KERBEROS.OPTION.") && !lstrcmp(th,"dns"))
            use_dns = 1;
    }

  cleanup:
    if ( use_dns < 0 )
        use_dns = 0;

    if (cnffile)
        fclose(cnffile);
    if (conf_fn)
        free(conf_fn);
    return use_dns;
}
#endif /* USE_DNS */
