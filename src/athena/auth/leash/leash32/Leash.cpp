//**************************************************************************
// File:	Leash.cpp 
// By:		Arthur David Leather
// Created:	12/02/98
// Copyright:	1998 Massachusetts Institute of Technology - All rights 
//		reserved.
//
// Description:	CPP file for Leash.h. Contains variables and functions 
//		for Leash 
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 12/02/98	ADL	Original
//**************************************************************************

#include "stdafx.h"
#include "Leash.h"

#include "MainFrm.h"
#include "LeashDoc.h"
#include "LeashView.h"
#include "LeashAboutBox.h" 

#include "reminder.h"
#include "mitwhich.h"
#include <leasherr.h>
#include "lglobals.h"
#include <com_err.h>

#include <wshelper.h>

#ifndef NO_AFS
#include "afscompat.h"
#endif

#include <errno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" int VScheckVersion(HWND hWnd, HANDLE hThisInstance);

HWND CLeashApp::m_hProgram = 0;
HINSTANCE CLeashApp::m_hLeashDLL = 0; 
HINSTANCE CLeashApp::m_hKrb4DLL = 0; 
HINSTANCE CLeashApp::m_hKrb5DLL = 0; 
HINSTANCE CLeashApp::m_hKrb5ProfileDLL= 0;
HINSTANCE CLeashApp::m_hAfsDLL = 0;
HINSTANCE CLeashApp::m_hPsapi = 0; 
HINSTANCE CLeashApp::m_hToolHelp32 = 0; 
krb5_context CLeashApp::m_krbv5_context = 0;
profile_t CLeashApp::m_krbv5_profile = 0;
HINSTANCE CLeashApp::m_hKrbLSA = 0;

/////////////////////////////////////////////////////////////////////////////
// CLeashApp

BEGIN_MESSAGE_MAP(CLeashApp, CWinApp)
	//{{AFX_MSG_MAP(CLeashApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeashApp construction
CLeashApp::CLeashApp()
{
	m_krbv5_context = NULL;
    m_krbv5_profile = NULL;
    // TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CLeashApp::~CLeashApp()
{
 	AfxFreeLibrary(m_hLeashDLL);
	AfxFreeLibrary(m_hKrb4DLL); 
	AfxFreeLibrary(m_hKrb5DLL);  
	AfxFreeLibrary(m_hKrb5ProfileDLL);  
	AfxFreeLibrary(m_hAfsDLL); 
	AfxFreeLibrary(m_hPsapi);  	
    AfxFreeLibrary(m_hToolHelp32);
    AfxFreeLibrary(m_hKrbLSA);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLeashApp object

CLeashApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLeashApp initialization

void CLeashApp::ParseParam (LPCTSTR lpszParam,BOOL bFlag,BOOL bLast)
{
	//CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast) ;
}

BOOL CLeashApp::InitInstance()
{
    // NOTE: Not used at this time
    /// Set LEASH_DLL to the path where the Leash.exe is
    ///char dllFile[MAX_PATH];
    ///ASSERT(GetModuleFileName(AfxGetInstanceHandle(), dllFile, MAX_PATH));

    ///char* pPath = dllFile + strlen(dllFile) - 1;
    ///while (*pPath != '\\')
    ///{
    ///	*pPath = 0;
    ///	pPath--;   
    ///}
	
    ///strcat(dllFile, LEASH_DLL);  
    ///m_leashDLL = dllFile;
		
    BOOL autoInit = FALSE;
    HWND hMsg = GetForegroundWindow();
    if (!InitDLLs())
        return FALSE; //exit program, can't load LEASHDLL

	// Check for args (switches)
    LPCTSTR exeFile		= __targv[0];
    LPCTSTR optionParam =  __targv[1];
	
    if (optionParam)
    {	
        if (*optionParam  == '-' || *optionParam  == '/')
        {
            if (0 == stricmp(optionParam+1, "kinit") || 
                0 == stricmp(optionParam+1, "i"))
            {
                TicketList* ticketList = NULL;
                pLeashKRB5GetTickets(&ticketinfoKrb5, &ticketList, 
                                      &CLeashApp::m_krbv5_context);
                pLeashFreeTicketList(&ticketList);
                pLeashKRB4GetTickets(&ticketinfoKrb4, &ticketList);
                pLeashFreeTicketList(&ticketList);
                
#ifdef OLD_DLG
                LSH_DLGINFO ldi;
                ldi.principal = ticketinfoKrb4.principal;
                ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.title = "Initialize Ticket";
                
                if (!pLeash_kinit_dlg(hMsg, &ldi))
                {
                    MessageBox(hMsg, "There was an error getting tickets!", 
                               "Error", MB_OK);
                    return FALSE;
                }
#else
                LSH_DLGINFO_EX ldi;
				char username[64]="";
				char realm[192]="";
				int i=0, j=0;
                if ( ticketinfoKrb5.btickets && ticketinfoKrb5.principal[0] ) {
                    for (; ticketinfoKrb5.principal[i] && ticketinfoKrb5.principal[i] != '@'; i++)
                    {
                        username[i] = ticketinfoKrb5.principal[i];
                    }
                    username[i] = '\0';
                    if (ticketinfoKrb5.principal[i]) {
                        for (i++ ; ticketinfoKrb5.principal[i] ; i++, j++)
                        {
                            realm[j] = ticketinfoKrb5.principal[i];
                        }
                    }
                    realm[j] = '\0';
                } else if ( ticketinfoKrb4.btickets && ticketinfoKrb4.principal[0] ) {
                    for (; ticketinfoKrb4.principal[i] && ticketinfoKrb4.principal[i] != '@'; i++)
                    {
                        username[i] = ticketinfoKrb4.principal[i];
                    }
                    username[i] = '\0';
                    if (ticketinfoKrb4.principal[i]) {
                        for (i++ ; ticketinfoKrb4.principal[i] ; i++, j++)
                        {
                            realm[j] = ticketinfoKrb4.principal[i];
                        }
                    }
                    realm[j] = '\0';
                }
				ldi.size = sizeof(ldi);
				ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.title = "Initialize Ticket";
                ldi.username = username;
				ldi.realm = realm;
                ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.use_defaults = 1;

                if (!pLeash_kinit_dlg_ex(hMsg, &ldi))
                {
                    MessageBox(hMsg, "There was an error getting tickets!", 
                               "Error", MB_OK);
                    return FALSE;
                }
#endif /* OLD_DLG */
                return TRUE;
            }
            else if (0 == stricmp(optionParam+1, "ms2mit") || 
                     0 == stricmp(optionParam+1, "import") || 
                     0 == stricmp(optionParam+1, "m"))
            {
                if (!pLeash_import())
                {
                    MessageBox(hMsg, 
                               "There was an error importing tickets from the Microsoft Logon Session!",
                               "Error", MB_OK);
                    return FALSE;
                }
                return TRUE;
            }
            else if (0 == stricmp(optionParam+1, "destroy") || 
                     0 == stricmp(optionParam+1, "d"))
            {
                if (!pLeash_kdestroy())
                {
                    MessageBox(hMsg, 
                               "There was an error destroying tickets!",
                               "Error", MB_OK);
                    return FALSE;
                }
                return TRUE;
            }
            else if (0 == stricmp(optionParam+1, "renew") || 
                     0 == stricmp(optionParam+1, "r"))
            {
                if (!pLeash_renew())
                {
                    MessageBox(hMsg, 
                               "There was an error renewing tickets!",
                               "Error", MB_OK);
                    return FALSE;
                }
                return TRUE;
            }
            else if (0 == stricmp(optionParam+1, "autoinit") || 
                     0 == stricmp(optionParam+1, "a"))
            {
                autoInit = TRUE;
            }
            else
            {	
                MessageBox(hMsg, 
                           "'-kinit' or '-i' to perform ticket initialization (and exit)\n"
                            "'-renew' or '-r' to perform ticket renewal (and exit)\n"
                            "'-destroy' or '-d' to perform ticket destruction (and exit)\n"
                            "'-autoinit' or '-a' to perform automatic ticket initialization\n"
                            "'-ms2mit' or '-import' or '-m' to perform ticket importation (and exit)",
                           "Leash Error", MB_OK);
                return FALSE;
            }
        }
        else 
        {	
            MessageBox(hMsg, 
                        "'-kinit' or '-i' to perform ticket initialization (and exit)\n"
                        "'-renew' or '-r' to perform ticket renewal (and exit)\n"
                        "'-destroy' or '-d' to perform ticket destruction (and exit)\n"
                        "'-autoinit' or '-a' to perform automatic ticket initialization\n"
                        "'-ms2mit' or '-import' or '-m' to perform ticket importation (and exit)",
                       "Leash Error", MB_OK);
            return FALSE;
        }
    }

    // Insure only one instance of Leash
    if (!FirstInstance())
        return FALSE;

    // Check to see if there are any tickets in the cache
    // If not and the Windows Logon Session is Kerberos authenticated attempt an import
    {
        TicketList* ticketList = NULL;
        pLeashKRB5GetTickets(&ticketinfoKrb5, &ticketList, &CLeashApp::m_krbv5_context);
        pLeashFreeTicketList(&ticketList);
        pLeashKRB4GetTickets(&ticketinfoKrb4, &ticketList);
        pLeashFreeTicketList(&ticketList);

        if ( !ticketinfoKrb4.btickets && !ticketinfoKrb5.btickets ) {
            if ( pLeash_importable() ) {
                pLeash_import();
            } 
            else if (autoInit) {
                LSH_DLGINFO_EX ldi;
				ldi.size = sizeof(ldi);
				ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.title = "Initialize Ticket";
                ldi.username = NULL;
				ldi.realm = NULL;
                ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.use_defaults = 1;

                pLeash_kinit_dlg_ex(hMsg, &ldi);
            }
        }
    }

	//register our unique wnd class name to find it later
    WNDCLASS wndcls;
    memset(&wndcls, 0, sizeof(WNDCLASS));
    wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = AfxGetInstanceHandle();
    wndcls.hIcon = LoadIcon(IDR_MAINFRAME);
    wndcls.hCursor = LoadCursor(IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndcls.lpszMenuName = NULL;
    //now the wnd class name to find it
    wndcls.lpszClassName = _T("LEASH.0WNDCLASS");
	
    //register the new class
    if(!AfxRegisterClass(&wndcls))
    {
        TRACE("Class registration failed\n");
        return FALSE;
    }

    AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    // Registry key under which our settings are stored.
	if (m_pszAppName)
		free((void*)m_pszAppName);
	m_pszAppName = _tcsdup("Leash32");    
    SetRegistryKey(_T("MIT"));

    LoadStdProfileSettings(); // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(LeashDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CLeashView));
    AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    VScheckVersion(m_pMainWnd->m_hWnd, AfxGetInstanceHandle());

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->SetWindowText("Leash");
    m_pMainWnd->UpdateWindow();
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->SetForegroundWindow();

    ValidateConfigFiles();

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLeashApp commands

// leash functions
DECL_FUNC_PTR(not_an_API_LeashKRB4GetTickets);
DECL_FUNC_PTR(not_an_API_LeashKRB5GetTickets); 
DECL_FUNC_PTR(not_an_API_LeashAFSGetToken);
DECL_FUNC_PTR(not_an_API_LeashFreeTicketList);
DECL_FUNC_PTR(not_an_API_LeashGetTimeServerName);
DECL_FUNC_PTR(Leash_kdestroy);
DECL_FUNC_PTR(Leash_changepwd_dlg);
DECL_FUNC_PTR(Leash_changepwd_dlg_ex);
DECL_FUNC_PTR(Leash_kinit_dlg);
DECL_FUNC_PTR(Leash_kinit_dlg_ex);
DECL_FUNC_PTR(Leash_timesync);
DECL_FUNC_PTR(Leash_get_default_lifetime);
DECL_FUNC_PTR(Leash_set_default_lifetime);
DECL_FUNC_PTR(Leash_get_default_forwardable);
DECL_FUNC_PTR(Leash_set_default_forwardable);
DECL_FUNC_PTR(Leash_get_default_renew_till);
DECL_FUNC_PTR(Leash_set_default_renew_till);
DECL_FUNC_PTR(Leash_get_default_noaddresses);
DECL_FUNC_PTR(Leash_set_default_noaddresses);
DECL_FUNC_PTR(Leash_get_default_proxiable);
DECL_FUNC_PTR(Leash_set_default_proxiable);
DECL_FUNC_PTR(Leash_get_default_publicip);
DECL_FUNC_PTR(Leash_set_default_publicip);
DECL_FUNC_PTR(Leash_get_default_use_krb4);
DECL_FUNC_PTR(Leash_set_default_use_krb4);
DECL_FUNC_PTR(Leash_get_default_life_min);
DECL_FUNC_PTR(Leash_set_default_life_min);
DECL_FUNC_PTR(Leash_get_default_life_max);
DECL_FUNC_PTR(Leash_set_default_life_max);
DECL_FUNC_PTR(Leash_get_default_renew_min);
DECL_FUNC_PTR(Leash_set_default_renew_min);
DECL_FUNC_PTR(Leash_get_default_renew_max);
DECL_FUNC_PTR(Leash_set_default_renew_max);
DECL_FUNC_PTR(Leash_get_default_renewable);
DECL_FUNC_PTR(Leash_set_default_renewable);
DECL_FUNC_PTR(Leash_get_lock_file_locations);
DECL_FUNC_PTR(Leash_set_lock_file_locations);
DECL_FUNC_PTR(Leash_get_default_uppercaserealm);
DECL_FUNC_PTR(Leash_set_default_uppercaserealm);
DECL_FUNC_PTR(Leash_import);
DECL_FUNC_PTR(Leash_importable);
DECL_FUNC_PTR(Leash_renew);
DECL_FUNC_PTR(Leash_reset_defaults);

FUNC_INFO leash_fi[] = {
    MAKE_FUNC_INFO(not_an_API_LeashKRB4GetTickets),
    MAKE_FUNC_INFO(not_an_API_LeashKRB5GetTickets), 
    MAKE_FUNC_INFO(not_an_API_LeashAFSGetToken),
    MAKE_FUNC_INFO(not_an_API_LeashFreeTicketList),
    MAKE_FUNC_INFO(not_an_API_LeashGetTimeServerName),
    MAKE_FUNC_INFO(Leash_kdestroy),
    MAKE_FUNC_INFO(Leash_changepwd_dlg),
    MAKE_FUNC_INFO(Leash_changepwd_dlg_ex),
    MAKE_FUNC_INFO(Leash_kinit_dlg),
	MAKE_FUNC_INFO(Leash_kinit_dlg_ex),
    MAKE_FUNC_INFO(Leash_timesync),
    MAKE_FUNC_INFO(Leash_get_default_lifetime),
    MAKE_FUNC_INFO(Leash_set_default_lifetime),
    MAKE_FUNC_INFO(Leash_get_default_renew_till),
    MAKE_FUNC_INFO(Leash_set_default_renew_till),
    MAKE_FUNC_INFO(Leash_get_default_forwardable),
    MAKE_FUNC_INFO(Leash_set_default_forwardable),
    MAKE_FUNC_INFO(Leash_get_default_noaddresses),
    MAKE_FUNC_INFO(Leash_set_default_noaddresses),
    MAKE_FUNC_INFO(Leash_get_default_proxiable),
    MAKE_FUNC_INFO(Leash_set_default_proxiable),
    MAKE_FUNC_INFO(Leash_get_default_publicip),
    MAKE_FUNC_INFO(Leash_set_default_publicip),
    MAKE_FUNC_INFO(Leash_get_default_use_krb4),
    MAKE_FUNC_INFO(Leash_set_default_use_krb4),
    MAKE_FUNC_INFO(Leash_get_default_life_min),
    MAKE_FUNC_INFO(Leash_set_default_life_min),
    MAKE_FUNC_INFO(Leash_get_default_life_max),
    MAKE_FUNC_INFO(Leash_set_default_life_max),
    MAKE_FUNC_INFO(Leash_get_default_renew_min),
    MAKE_FUNC_INFO(Leash_set_default_renew_min),
    MAKE_FUNC_INFO(Leash_get_default_renew_max),
    MAKE_FUNC_INFO(Leash_set_default_renew_max),
    MAKE_FUNC_INFO(Leash_get_default_renewable),
    MAKE_FUNC_INFO(Leash_set_default_renewable),
    MAKE_FUNC_INFO(Leash_get_lock_file_locations),
    MAKE_FUNC_INFO(Leash_set_lock_file_locations),
    MAKE_FUNC_INFO(Leash_get_default_uppercaserealm),
    MAKE_FUNC_INFO(Leash_set_default_uppercaserealm),
    MAKE_FUNC_INFO(Leash_import),
    MAKE_FUNC_INFO(Leash_importable),
    MAKE_FUNC_INFO(Leash_renew),
    MAKE_FUNC_INFO(Leash_reset_defaults),
    END_FUNC_INFO
};

// krb4 functions
DECL_FUNC_PTR(set_krb_debug);
DECL_FUNC_PTR(set_krb_ap_req_debug);
DECL_FUNC_PTR(krb_get_krbconf2); 
DECL_FUNC_PTR(krb_get_krbrealm2); 
DECL_FUNC_PTR(tkt_string); 
DECL_FUNC_PTR(krb_set_tkt_string); 
DECL_FUNC_PTR(krb_realmofhost);
DECL_FUNC_PTR(krb_get_lrealm);
DECL_FUNC_PTR(krb_get_krbhst);
DECL_FUNC_PTR(tf_init);
DECL_FUNC_PTR(tf_close);
DECL_FUNC_PTR(krb_get_tf_realm);

FUNC_INFO krb4_fi[] = {
    MAKE_FUNC_INFO(set_krb_debug),
    MAKE_FUNC_INFO(set_krb_ap_req_debug),
    MAKE_FUNC_INFO(krb_get_krbconf2), 
    MAKE_FUNC_INFO(krb_get_krbrealm2), 
    MAKE_FUNC_INFO(tkt_string), 
    MAKE_FUNC_INFO(krb_set_tkt_string), 
    MAKE_FUNC_INFO(krb_realmofhost),
    MAKE_FUNC_INFO(krb_get_lrealm),
    MAKE_FUNC_INFO(krb_get_krbhst),
    MAKE_FUNC_INFO(tf_init),
    MAKE_FUNC_INFO(tf_close),
    MAKE_FUNC_INFO(krb_get_tf_realm),
    END_FUNC_INFO
};


// psapi functions
DECL_FUNC_PTR(GetModuleFileNameExA);
DECL_FUNC_PTR(EnumProcessModules);

FUNC_INFO psapi_fi[] = {
    MAKE_FUNC_INFO(GetModuleFileNameExA),
    MAKE_FUNC_INFO(EnumProcessModules),
    END_FUNC_INFO
};

// toolhelp functions
DECL_FUNC_PTR(CreateToolhelp32Snapshot);
DECL_FUNC_PTR(Module32First);
DECL_FUNC_PTR(Module32Next);

FUNC_INFO toolhelp_fi[] = {
    MAKE_FUNC_INFO(CreateToolhelp32Snapshot),
    MAKE_FUNC_INFO(Module32First),
    MAKE_FUNC_INFO(Module32Next),
    END_FUNC_INFO
};

// krb5 functions
DECL_FUNC_PTR(krb5_cc_default_name);
DECL_FUNC_PTR(krb5_get_default_config_files);
DECL_FUNC_PTR(krb5_free_config_files);
DECL_FUNC_PTR(krb5_free_context);
DECL_FUNC_PTR(krb5_get_default_realm);
DECL_FUNC_PTR(krb5_init_context);
DECL_FUNC_PTR(krb5_cc_default);
DECL_FUNC_PTR(krb5_parse_name);
DECL_FUNC_PTR(krb5_free_principal);
DECL_FUNC_PTR(krb5_cc_close);

FUNC_INFO krb5_fi[] = {
    MAKE_FUNC_INFO(krb5_cc_default_name),
    MAKE_FUNC_INFO(krb5_get_default_config_files),
    MAKE_FUNC_INFO(krb5_free_config_files),
    MAKE_FUNC_INFO(krb5_free_context),
    MAKE_FUNC_INFO(krb5_get_default_realm),
    MAKE_FUNC_INFO(krb5_init_context),
    MAKE_FUNC_INFO(krb5_cc_default),
    MAKE_FUNC_INFO(krb5_parse_name),
    MAKE_FUNC_INFO(krb5_free_principal),
    MAKE_FUNC_INFO(krb5_cc_close),
    END_FUNC_INFO
};

// profile functions
DECL_FUNC_PTR(profile_release);
DECL_FUNC_PTR(profile_init);
DECL_FUNC_PTR(profile_flush);
DECL_FUNC_PTR(profile_rename_section);
DECL_FUNC_PTR(profile_update_relation);
DECL_FUNC_PTR(profile_clear_relation);
DECL_FUNC_PTR(profile_add_relation);
DECL_FUNC_PTR(profile_get_relation_names);
DECL_FUNC_PTR(profile_get_subsection_names);
DECL_FUNC_PTR(profile_get_values);
DECL_FUNC_PTR(profile_free_list);
DECL_FUNC_PTR(profile_abandon);
DECL_FUNC_PTR(profile_get_string);
DECL_FUNC_PTR(profile_release_string);

FUNC_INFO profile_fi[] = {
    MAKE_FUNC_INFO(profile_release),
    MAKE_FUNC_INFO(profile_init),
    MAKE_FUNC_INFO(profile_flush),
    MAKE_FUNC_INFO(profile_rename_section),
    MAKE_FUNC_INFO(profile_update_relation),
    MAKE_FUNC_INFO(profile_clear_relation),
    MAKE_FUNC_INFO(profile_add_relation),
    MAKE_FUNC_INFO(profile_get_relation_names),
    MAKE_FUNC_INFO(profile_get_subsection_names),
    MAKE_FUNC_INFO(profile_get_values),
    MAKE_FUNC_INFO(profile_free_list),
    MAKE_FUNC_INFO(profile_abandon),
    MAKE_FUNC_INFO(profile_get_string),
    MAKE_FUNC_INFO(profile_release_string),
    END_FUNC_INFO
};

// Tries to load the .DLL files.  If it works, we get some functions from them
// and return a TRUE.  If it doesn't work, we return a FALSE.
BOOL CLeashApp::InitDLLs()
{
    m_hLeashDLL = AfxLoadLibrary(LEASHDLL); 
    m_hKrb4DLL = AfxLoadLibrary(KERB4DLL); 
    m_hKrb5DLL = AfxLoadLibrary(KERB5DLL); 
    m_hKrb5ProfileDLL = AfxLoadLibrary(KERB5_PPROFILE_DLL);

#ifndef NO_AFS
    afscompat_init();
    m_hAfsDLL = AfxLoadLibrary(AFSAuthentDLL());
#endif

#define PSAPIDLL "psapi.dll"
#define TOOLHELPDLL "kernel32.dll"

    m_hPsapi = AfxLoadLibrary(PSAPIDLL);
    m_hToolHelp32 = AfxLoadLibrary(TOOLHELPDLL);

    HWND hwnd = GetForegroundWindow();
    if (!m_hLeashDLL) 
    {
        // We couldn't load the m_hLeashDLL.
        m_msgError = "Couldn't load the Leash DLL or one of its dependents.";
        MessageBox(hwnd, m_msgError, "Error", MB_OK);	
        return FALSE;
    }

    if (!LoadFuncs(LEASHDLL, leash_fi, 0, 0, 1, 0, 0))
    {
        MessageBox(hwnd, 
                   "Functions within the Leash DLL didn't load properly!", 
                   "Error", MB_OK);		
        return FALSE;
    }

    if (m_hKrb4DLL)
    {
        if (!LoadFuncs(KERB4DLL, krb4_fi, 0, 0, 1, 0, 0))
        {
            MessageBox(hwnd,
                       "Unexpected error while loading " KERB4DLL ".\n"
                       "Kerberos 4 functionality will be disabled.\n",
                       "Error", MB_OK);
        }
    }

    if (m_hKrb5DLL)
    {
        if (!LoadFuncs(KERB5DLL, krb5_fi, 0, 0, 1, 0, 0))
        {
            MessageBox(hwnd,
                       "Unexpected error while loading " KERB5DLL ".\n"
                       "Kerberos 5 functionality will be disabled.\n",
                       "Error", MB_OK);
            AfxFreeLibrary(m_hKrb5DLL);
            m_hKrb5DLL = 0;
        }
        else if (!m_hKrb5ProfileDLL || 
                 !LoadFuncs(KERB5_PPROFILE_DLL, profile_fi, 0, 0, 1, 0, 0))
        {
            MessageBox(hwnd,
                       "Unexpected error while loading "KERB5_PPROFILE_DLL".\n"
                       "Kerberos 5 functionality will be disabled.\n",
                       "Error", MB_OK);
            AfxFreeLibrary(m_hKrb5ProfileDLL);
            m_hKrb5ProfileDLL = 0;
            // Use m_hKrb5DLL to undo LoadLibrary in loadfuncs...
            UnloadFuncs(krb5_fi, m_hKrb5DLL);
            AfxFreeLibrary(m_hKrb5DLL);
            m_hKrb5DLL = 0;
        }

    }

    OSVERSIONINFO osvi;
    memset(&osvi, 0, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    // XXX: We should really use feature testing, first
    // checking for CreateToolhelp32Snapshot.  If that's
    // not around, we try the psapi stuff.
    //
    // Only load LSA functions if on NT/2000/XP
    if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        // Windows 9x
        AfxFreeLibrary(m_hPsapi);
        m_hPsapi = NULL;
        if (!m_hToolHelp32 || 
            !LoadFuncs(TOOLHELPDLL, toolhelp_fi, 0, 0, 1, 0, 0))
        {
            MessageBox(hwnd, "Could not load " TOOLHELPDLL "!", "Error", 
                       MB_OK);
            return FALSE;
        }
    }             
    else if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        // Windows NT
        AfxFreeLibrary(m_hToolHelp32);
        m_hToolHelp32 = NULL;
        if (!m_hPsapi ||
            !LoadFuncs(PSAPIDLL, psapi_fi, 0, 0, 1, 0, 0))
        {
            MessageBox(hwnd, "Could not load " PSAPIDLL "!", "Error", MB_OK);
            return FALSE;
        }

		m_hKrbLSA  = AfxLoadLibrary(SECUR32DLL);
    }
    else
    {
        MessageBox(hwnd, 
                   "Unrecognized Operating System!", 
                   "Error", MB_OK);
        return FALSE;
    }
           
    return TRUE;
}


BOOL CLeashApp::FirstInstance()
{
    CWnd* pWndprev;
    CWnd* pWndchild;
	
    //find if it exists
    pWndprev = CWnd::FindWindow(_T("LEASH.0WNDCLASS"), NULL);	
    if (pWndprev)
    {
        //if it has popups
        pWndchild = pWndprev->GetLastActivePopup();
        //if iconic restore
        if (pWndprev->IsIconic())
            pWndprev->ShowWindow(SW_RESTORE);
			
        //bring the wnd to foreground
        pWndchild->SetForegroundWindow();
		
        return FALSE;
    }
    //we could not find prev instance
    else
        return TRUE;
}

void
CLeashApp::ValidateConfigFiles()
{
    CStdioFile krbCon;
    char confname[257];
    char realm[256]="";
    
    CWinApp * pApp = AfxGetApp();
    if (pApp)
        if (!pApp->GetProfileInt("Settings", "CreateMissingConfig", FALSE_FLAG))
            return;

    if ( m_hKrb5DLL ) {
        int krb_con_open = 0;

        // Create the empty KRB5.INI file
        if (!GetProfileFile(confname,sizeof(confname))) {
            const char *filenames[2];
		    filenames[0] = confname;
		    filenames[1] = NULL;
		    long retval = pprofile_init(filenames, &m_krbv5_profile);
			if (!retval) 
				return;
			else if (retval == ENOENT) {
				FILE * f = fopen(confname,"w");
				if (f != NULL) {
					fclose(f);
					retval = pprofile_init(filenames, &m_krbv5_profile);
				}
			}


            if ( !GetKrb4ConFile(confname,sizeof(confname)) ) {
                if (!krbCon.Open(confname, CFile::modeNoTruncate | CFile::modeRead)) 
                {	
                    if (krbCon.Open(confname, CFile::modeCreate | CFile::modeWrite)) 
                    {	
                        krb_con_open = 1;
                    }
                }
            }

            const char*  lookupKdc[] = {"libdefaults", "dns_lookup_kdc", NULL}; 
            const char*  lookupRealm[] = {"libdefaults", "dns_lookup_realm", NULL}; 
            const char*  defRealm[] = {"libdefaults", "default_realm", NULL};
            const char*  noAddresses[] = {"libdefaults", "noaddresses", NULL}; 

            // activate DNS KDC Lookups
            const char** names = lookupKdc;	
            retval = pprofile_add_relation(m_krbv5_profile, 
                                           names, 
                                           "true");

            // activate No Addresses
            names = noAddresses;	
            retval = pprofile_add_relation(m_krbv5_profile, 
                                           names, 
                                           "true");

            // Get Windows 2000/XP/2003 Kerberos config
            if ( m_hKrbLSA )
            {
                char domain[256]="";
                HKEY hk=0;
                DWORD dwType, dwSize, dwIndex;

                if ( !RegOpenKeyEx(HKEY_CURRENT_USER,
                                    "Volatile Environment", 0,
                                    KEY_READ, &hk) ) 
                { 
                    dwSize = sizeof(domain);
                    RegQueryValueEx(hk, "USERDNSDOMAIN", 0, 0, (LPBYTE)domain, &dwSize);
                    RegCloseKey(hk);
                }
                else if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                             "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
                             0, KEY_READ, &hk))
                {
                    
                    dwSize = sizeof(domain);
                    RegQueryValueEx( hk, "DefaultDomainName",
                                     NULL, &dwType, (unsigned char *)&domain, &dwSize);
                    RegCloseKey(hk);
                } 

                char realmkey[256]="SYSTEM\\CurrentControlSet\\Control\\Lsa\\Kerberos\\Domains\\";
                int  keylen = strlen(realmkey)-1;

                if ( domain[0] ) {
                    strncpy(realm,domain,256);
                    if ( krb_con_open ) {
                        krbCon.WriteString(realm);
                        krbCon.WriteString("\n");
                    }
                    strncat(realmkey,domain,256);
                }

                if ( domain[0] &&
                     !RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                   realmkey,
                                   0, 
                                   KEY_READ,
                                   &hk)
                     )
                {
                    RegCloseKey(hk);

                    realmkey[keylen] = '\0';
                    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                 realmkey,
                                 0,
                                 KEY_READ|KEY_ENUMERATE_SUB_KEYS,
                                 &hk);

                    dwIndex = 0;
                    unsigned char subkey[256];
                    FILETIME ft;
                    dwSize = 256;
                    while ( ERROR_SUCCESS == RegEnumKeyEx(hk,dwIndex++,
                                                          (char *)subkey,
                                                          &dwSize,
                                                          0,
                                                          0,
                                                          0,
                                                          &ft) )
                    {
                        HKEY hksub;

                        if ( !RegOpenKeyEx(hk,
                                   (char *)subkey,
                                   0, 
                                   KEY_READ,
                                   &hksub) )
                        {
                            unsigned char * lpszValue = NULL, *p;
                            dwSize = 0;
							dwType = 0;
                            RegQueryValueEx( hksub, "KdcNames",
                                             NULL, &dwType, lpszValue, &dwSize);
                            if ( dwSize > 0 ) {
                                lpszValue = (unsigned char *)malloc(dwSize+1);
                                dwSize += 1;
                                RegQueryValueEx( hksub, "KdcNames",
                                                 NULL, &dwType, lpszValue, &dwSize);

                                p = lpszValue;
                                while ( *p ) {
                                    const char*  realmKdc[] = {"realms", (const char *)subkey, "kdc", NULL};
                                    names = realmKdc;
                                    retval = pprofile_add_relation(m_krbv5_profile, 
                                                                    names, 
                                                                    (const char *)p);

                                    if ( krb_con_open ) {
                                        krbCon.WriteString((const char *)subkey);
                                        krbCon.WriteString("\t");
                                        krbCon.WriteString((const char *)p);
                                        krbCon.WriteString("\n");
                                    }

                                    p += strlen((char*)p) + 1;
                                }
                                free(lpszValue);
                            }
                            RegCloseKey(hksub);
                        }
                    }
                    RegCloseKey(hk);
                }
            } else {
                // activate DNS Realm Lookups (temporarily)
                names = lookupRealm;	
                retval = pprofile_add_relation(m_krbv5_profile, 
                                                names, 
                                                "true");
            }

            // Save to Kerberos Five config. file "Krb5.ini"
            retval = pprofile_flush(m_krbv5_profile);


            // Use DNS to retrieve the realm (if possible)
            if (!realm[0]) {
                krb5_context ctx = 0;
                krb5_principal me = 0;
                krb5_error_code code = 0;

                code = pkrb5_init_context(&ctx);
                if (code) goto no_k5_realm;

                code = pkrb5_parse_name(ctx, "foo", &me);
                if (code) goto no_k5_realm;

                if ( krb5_princ_realm(ctx,me)->length < sizeof(realm) - 1) {
                    memcpy(realm, krb5_princ_realm(ctx,me)->data,
                            krb5_princ_realm(ctx,me)->length);
                    realm[krb5_princ_realm(ctx,me)->length] = '\0';
                }

                if ( krb_con_open ) {
                    krbCon.WriteString(realm);
                    krbCon.WriteString("\n");
                }

              no_k5_realm:
                if ( me )
                    pkrb5_free_principal(ctx,me);
                if ( ctx )
                    pkrb5_free_context(ctx);
            }

            // disable DNS Realm Lookups
            retval = pprofile_update_relation(m_krbv5_profile, 
                                             names,
                                             "true", "false");

            // save the default realm if it was discovered
            if ( realm[0] ) {
                names = defRealm;
                retval = pprofile_add_relation(m_krbv5_profile, 
                                               names, 
                                               realm);

                // It would be nice to be able to generate a list of KDCs
                // but to do so based upon the contents of DNS would be 
                // wrong for several reasons:
                // . it would make static the values inserted into DNS SRV
                //   records 
                // . DNS cannot necessarily be trusted
            }
            
            // Save to Kerberos Five config. file "Krb5.ini"
            retval = pprofile_flush(m_krbv5_profile);

            pprofile_release(m_krbv5_profile);
            m_krbv5_profile = NULL;

            // Close KRB.CON file
            if ( krb_con_open ) {
                krbCon.WriteString(".KERBEROS.OPTION. dns\n");
                krbCon.Close();
            }

            // Create the empty KRBREALM.CON file
            if ( !GetKrb4RealmFile(confname,sizeof(confname)) ) {
                if (!krbCon.Open(confname, CFile::modeNoTruncate | CFile::modeRead)) 
                {	
                    if (krbCon.Open(confname, CFile::modeCreate | CFile::modeWrite)) 
                    {	
                        krbCon.Close();
                    }
                } else
                    krbCon.Close();
            }
        
        }
    } else if ( m_hKrb4DLL ) {
        if ( !realm[0] ) {
            /* Open ticket file */
            char * file = ptkt_string();
            int k_errno;

            if (file != NULL && file[0]) {
                if ((k_errno = ptf_init(file, R_TKT_FIL)) == KSUCCESS) {
                    /* Close ticket file */
                    (void) ptf_close();

                    k_errno = pkrb_get_tf_realm(file, realm);
                }
                if (k_errno != KSUCCESS) {
                    k_errno = pkrb_get_lrealm(realm, 1);
                }
            }
        }

        if ( !GetKrb4ConFile(confname,sizeof(confname)) ) {
            if (!krbCon.Open(confname, CFile::modeNoTruncate | CFile::modeRead)) 
            {	
                if (krbCon.Open(confname, CFile::modeCreate | CFile::modeWrite)) 
                {	
                    if ( realm[0] )
                        krbCon.WriteString(realm);
                    krbCon.WriteString("\n.KERBEROS.OPTION. dns\n");
                    krbCon.Close();
                }
            } else
				krbCon.Close();
        }

        if ( !GetKrb4RealmFile(confname,sizeof(confname)) ) {
            if (!krbCon.Open(confname, CFile::modeNoTruncate | CFile::modeRead)) 
            {	
                if (krbCon.Open(confname, CFile::modeCreate | CFile::modeWrite)) 
                {	
                    krbCon.Close();
                }
            } else
				krbCon.Close();
        }
    }
}

BOOL
CLeashApp::GetKrb4ConFile(
    LPSTR confname, 
    UINT szConfname
    )
{
    if (m_hKrb5DLL && !m_hKrb4DLL)
	{ // hold krb.con where krb5.ini is located
		CHAR krbConFile[MAX_PATH]="";
	    //strcpy(krbConFile, CLeashApp::m_krbv5_profile->first_file->filename);
        if (GetProfileFile(krbConFile, sizeof(krbConFile)))	
        {
		    GetWindowsDirectory(krbConFile,sizeof(krbConFile));
			strncat(krbConFile,"\\KRB5.INI",sizeof(krbConFile));
        }

		LPSTR pFind = strrchr(krbConFile, '\\');
		if (pFind)
		{
			*pFind = 0;
			strcat(krbConFile, "\\");
			strcat(krbConFile, KRB_FILE);
		}
		else
		  ASSERT(0);
		
		strncpy(confname, krbConFile, szConfname);
        confname[szConfname-1] = '\0';
	}
	else if (m_hKrb4DLL)
	{ 
        unsigned int size = szConfname;
        memset(confname, '\0', szConfname);
 		if (!pkrb_get_krbconf2(confname, &size))
		{ // Error has happened
		    GetWindowsDirectory(confname,szConfname);
			strncat(confname, "\\",szConfname);
			strncat(confname,KRB_FILE,szConfname);
		}
	}
    return FALSE;
}

BOOL
CLeashApp::GetKrb4RealmFile(
    LPSTR confname, 
    UINT szConfname
    )
{
    if (m_hKrb5DLL && !m_hKrb4DLL)
	{ // hold krb.con where krb5.ini is located
		CHAR krbRealmConFile[MAX_PATH];
		//strcpy(krbRealmConFile, CLeashApp::m_krbv5_profile->first_file->filename);
		if (GetProfileFile(krbRealmConFile, sizeof(krbRealmConFile)))	
        {
		    GetWindowsDirectory(krbRealmConFile,sizeof(krbRealmConFile));
			strncat(krbRealmConFile,"\\KRB5.INI",sizeof(krbRealmConFile));
        }

		LPSTR pFind = strrchr(krbRealmConFile, '\\');
		if (pFind)
		{
			*pFind = 0;
			strcat(krbRealmConFile, "\\");
			strcat(krbRealmConFile, KRBREALM_FILE);
		}
		else
		  ASSERT(0);
		
		strncpy(confname, krbRealmConFile, szConfname);
        confname[szConfname-1] = '\0';
	}
	else if (m_hKrb4DLL)
	{ 
        unsigned int size = szConfname;
        memset(confname, '\0', szConfname);
        if (!pkrb_get_krbrealm2(confname, &size))
		{ 
		    GetWindowsDirectory(confname,szConfname);
			strncat(confname, "\\",szConfname);
			strncat(confname,KRBREALM_FILE,szConfname);
            return TRUE;
		}
	}	
    return FALSE;
}

BOOL 
CLeashApp::GetProfileFile(
    LPSTR confname, 
    UINT szConfname
    )
{
    char **configFile = NULL;
    if (pkrb5_get_default_config_files(&configFile)) 
    {
        GetWindowsDirectory(confname,szConfname);
        strncat(confname,"\\KRB5.INI",szConfname);
        return FALSE;
    }
    
    *confname = 0;
    
    if (configFile)
    {
        strncpy(confname, *configFile, szConfname);
        pkrb5_free_config_files(configFile); 
    }
    
    if (!*confname)
    {
        GetWindowsDirectory(confname,szConfname);
        strncat(confname,"\\KRB5.INI",szConfname);
    }
    
    return FALSE;
}
