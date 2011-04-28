//*****************************************************************************
// File:	lgobals.h
// By:		Arthur David Leather
// Created:	12/02/98
// Copyright:	@1998 Massachusetts Institute of Technology - All rights 
//              reserved.
// Description:	H file for lgobals.cpp. Contains global variables and helper 
//		functions
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 02/02/98	ADL	Original
//*****************************************************************************

#if !defined LEASHGLOBALS_H
#define LEASHGLOBALS_H

#include "tlhelp32.h"

#include <loadfuncs-krb5.h>
#include <loadfuncs-krb.h>
#include <loadfuncs-profile.h>
#include <loadfuncs-leash.h>

struct TicketList
{
    char* theTicket;
    TicketList* next;
};

// toolhelp functions
TYPEDEF_FUNC(
    HANDLE,
    WINAPI,
    CreateToolhelp32Snapshot,
    (DWORD, DWORD)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    Module32First,
    (HANDLE, LPMODULEENTRY32)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    Module32Next,
    (HANDLE, LPMODULEENTRY32)
    );

// psapi functions
TYPEDEF_FUNC(
    DWORD,
    WINAPI,
    GetModuleFileNameExA,
    (HANDLE, HMODULE, LPSTR, DWORD)
    );
TYPEDEF_FUNC(
    BOOL,
    WINAPI,
    EnumProcessModules,
    (HANDLE, HMODULE*, DWORD, LPDWORD)
    );

#define pGetModuleFileNameEx pGetModuleFileNameExA

// leash functions
TYPEDEF_FUNC(
    long,
    WINAPIV,
    not_an_API_LeashKRB4GetTickets,
    (TICKETINFO *, TicketList **)
    );
TYPEDEF_FUNC(
    long,
    WINAPIV,
    not_an_API_LeashKRB5GetTickets,
    (TICKETINFO *, TicketList **, krb5_context *)
    );
TYPEDEF_FUNC(
    long,
    WINAPIV,
    not_an_API_LeashAFSGetToken,
    (TicketList **)
    );
TYPEDEF_FUNC(
    long,
    WINAPIV,
    not_an_API_LeashFreeTicketList,
    (TicketList**)
    );
TYPEDEF_FUNC(
    long,
    WINAPIV,
    not_an_API_LeashGetTimeServerName,
    (char *, const char*)
    );

extern DECL_FUNC_PTR(not_an_API_LeashKRB4GetTickets);
extern DECL_FUNC_PTR(not_an_API_LeashKRB5GetTickets); 
extern DECL_FUNC_PTR(not_an_API_LeashAFSGetToken);
extern DECL_FUNC_PTR(not_an_API_LeashFreeTicketList);
extern DECL_FUNC_PTR(not_an_API_LeashGetTimeServerName);
extern DECL_FUNC_PTR(Leash_kdestroy);
extern DECL_FUNC_PTR(Leash_changepwd_dlg);
extern DECL_FUNC_PTR(Leash_kinit_dlg);
extern DECL_FUNC_PTR(Leash_timesync);
extern DECL_FUNC_PTR(Leash_get_default_lifetime);
extern DECL_FUNC_PTR(Leash_set_default_lifetime);

#define pLeashKRB4GetTickets     pnot_an_API_LeashKRB4GetTickets
#define pLeashKRB5GetTickets     pnot_an_API_LeashKRB5GetTickets
#define pLeashAFSGetToken        pnot_an_API_LeashAFSGetToken
#define pLeashFreeTicketList     pnot_an_API_LeashFreeTicketList
#define pLeashGetTimeServerName  pnot_an_API_LeashGetTimeServerName

// krb4 functions
extern DECL_FUNC_PTR(set_krb_debug);
extern DECL_FUNC_PTR(set_krb_ap_req_debug);
extern DECL_FUNC_PTR(krb_get_krbconf2); 
extern DECL_FUNC_PTR(krb_get_krbrealm2); 
extern DECL_FUNC_PTR(tkt_string); 
extern DECL_FUNC_PTR(krb_set_tkt_string); 
extern DECL_FUNC_PTR(krb_realmofhost);
extern DECL_FUNC_PTR(krb_get_lrealm);
extern DECL_FUNC_PTR(krb_get_krbhst);

// psapi functions
extern DECL_FUNC_PTR(GetModuleFileNameExA);
extern DECL_FUNC_PTR(EnumProcessModules);

// toolhelp functions
extern DECL_FUNC_PTR(CreateToolhelp32Snapshot);
extern DECL_FUNC_PTR(Module32First);
extern DECL_FUNC_PTR(Module32Next);

// krb5 functions
extern DECL_FUNC_PTR(krb5_cc_default_name);
extern DECL_FUNC_PTR(krb5_get_default_config_files);
extern DECL_FUNC_PTR(krb5_free_config_files);
extern DECL_FUNC_PTR(krb5_free_context);
extern DECL_FUNC_PTR(krb5_get_default_realm);
extern DECL_FUNC_PTR(krb5_get_host_realm);

// profile functions
extern DECL_FUNC_PTR(profile_release);
extern DECL_FUNC_PTR(profile_init);
extern DECL_FUNC_PTR(profile_flush);
extern DECL_FUNC_PTR(profile_rename_section);
extern DECL_FUNC_PTR(profile_update_relation);
extern DECL_FUNC_PTR(profile_clear_relation);
extern DECL_FUNC_PTR(profile_add_relation);
extern DECL_FUNC_PTR(profile_get_relation_names);
extern DECL_FUNC_PTR(profile_get_subsection_names);
extern DECL_FUNC_PTR(profile_get_values);
extern DECL_FUNC_PTR(profile_free_list);
extern DECL_FUNC_PTR(profile_abandon);

#define LEFT_SIDE     1
#define RIGHT_SIDE    2
#define TOP_SIDE      3
#define BOTTOM_SIDE   6

#define ADMIN_SERVER "admin_server"

extern TICKETINFO ticketinfo;
extern TICKETINFO ticketinfoKrb4;
extern TICKETINFO ticketinfoKrb5;


#define ON  1
#define OFF 0
#define TRUE_FLAG		1 
#define FALSE_FLAG		0
#define LEASHDLL "leashw32.dll"
#define KERB4DLL "krbv4w32.dll"
#define KERB5DLL "krb5_32.dll"
#define KERB5_PPROFILE_DLL "xpprof32.dll"

extern BOOL SetRegistryVariable(const CString& regVariable, 
                                const CString& regValue,
                                const char* regSubKey = "Software\\MIT\\Leash32\\Settings");
extern VOID LeashErrorBox(LPCSTR errorMsg, LPCSTR insertedString, 
                          LPCSTR errorFlag = "Error");


class Directory
{
    CHAR m_savCurPath[MAX_PATH];
    CString m_pathToValidate;

public:
    Directory(LPCSTR pathToValidate);
    virtual ~Directory();

    BOOL IsValidDirectory();
    BOOL IsValidFile();
};

#endif 
