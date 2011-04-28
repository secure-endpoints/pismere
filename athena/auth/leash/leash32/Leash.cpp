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

#ifndef NO_AFS
#include "afscompat.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" int VScheckVersion(HWND hWnd, HANDLE hThisInstance);

HWND CLeashApp::m_hProgram = 0;
HINSTANCE CLeashApp::m_hLeashDLL; 
HINSTANCE CLeashApp::m_hKrb4DLL; 
HINSTANCE CLeashApp::m_hKrb5DLL; 
HINSTANCE CLeashApp::m_hKrb5ProfileDLL;
HINSTANCE CLeashApp::m_hAfsDLL;
HINSTANCE CLeashApp::m_hPsapi; 
HINSTANCE CLeashApp::m_hToolHelp32; 
krb5_context CLeashApp::m_krbv5_context;
profile_t CLeashApp::m_krbv5_profile;
BOOL CLeashApp::GetProfileFile(LPSTR confname, CONST INT szConfname);

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
                TicketList* listKrb4 = NULL;
                pLeashKRB4GetTickets(&ticketinfoKrb4, &listKrb4); 
                
                LSH_DLGINFO ldi;
                ldi.principal = ticketinfoKrb4.principal;
                ldi.dlgtype = DLGTYPE_PASSWD;
                ldi.title = "Initialize Ticket";
                
                HWND hApp = GetForegroundWindow();
                if (!pLeash_kinit_dlg(hApp, &ldi))
                {
                    MessageBox(hMsg, "There was an error getting tickets!", 
                               "Error", MB_OK);
                    return FALSE;
                }

                return TRUE;
            }
            else 
            {	
                MessageBox(hMsg, 
                           "'-kinit' or '-i' (Login Only) are the only valid options", 
                           "Leash Error", MB_OK);
                return FALSE;
            }
        }
        else 
        {	
            MessageBox(hMsg, 
                       "'-kinit' or '-i' (Login Only) are the only valid option", 
                       "Leash Error", MB_OK);
            return FALSE;
        }
    }

    // Insure only one instance of Leash
    if (!FirstInstance())
        return FALSE;

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

    ::SetWindowText(m_pMainWnd->m_hWnd, "Leash32");
	
    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

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
DECL_FUNC_PTR(Leash_kinit_dlg);
DECL_FUNC_PTR(Leash_timesync);
DECL_FUNC_PTR(Leash_get_default_lifetime);
DECL_FUNC_PTR(Leash_set_default_lifetime);

FUNC_INFO leash_fi[] = {
    MAKE_FUNC_INFO(not_an_API_LeashKRB4GetTickets),
    MAKE_FUNC_INFO(not_an_API_LeashKRB5GetTickets), 
    MAKE_FUNC_INFO(not_an_API_LeashAFSGetToken),
    MAKE_FUNC_INFO(not_an_API_LeashFreeTicketList),
    MAKE_FUNC_INFO(not_an_API_LeashGetTimeServerName),
    MAKE_FUNC_INFO(Leash_kdestroy),
    MAKE_FUNC_INFO(Leash_changepwd_dlg),
    MAKE_FUNC_INFO(Leash_kinit_dlg),
    MAKE_FUNC_INFO(Leash_timesync),
    MAKE_FUNC_INFO(Leash_get_default_lifetime),
    MAKE_FUNC_INFO(Leash_set_default_lifetime),
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
DECL_FUNC_PTR(krb5_get_host_realm);

FUNC_INFO krb5_fi[] = {
    MAKE_FUNC_INFO(krb5_cc_default_name),
    MAKE_FUNC_INFO(krb5_get_default_config_files),
    MAKE_FUNC_INFO(krb5_free_config_files),
    MAKE_FUNC_INFO(krb5_free_context),
    MAKE_FUNC_INFO(krb5_get_default_realm),
    MAKE_FUNC_INFO(krb5_get_host_realm),
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


BOOL 
CLeashApp::GetProfileFile(
    LPSTR confname, 
    CONST INT szConfname
    )
{
    char **configFile = NULL;
    if (pkrb5_get_default_config_files(&configFile)) 
    {
        // Error
        return TRUE;
    }
    
    *confname = 0;
    
    if (configFile)
    {
        strncpy(confname, *configFile, szConfname);
        pkrb5_free_config_files(configFile); 
    }
    else
    {
        // Error
        pkrb5_free_config_files(configFile); 
        return TRUE;
    }
    
    if (!*confname)
    {
        // Error
        return TRUE;
    }
    
    return FALSE;
}
