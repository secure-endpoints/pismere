//****************************************************************************
// File:	Krb5Properties.cpp 
// By:		Arthur David Leather
// Created:	12/02/98
// Copyright:	1998 Massachusetts Institute of Technology - All rights 
//		reserved.
// Description:	CPP file for Krb5Properties.h. Contains variables and functions
//		for Kerberos Five Properties
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 12/02/98	ADL	Original
//*****************************************************************************

#include "stdafx.h"
#include "leash.h"
#include "LeashFileDialog.h" 
#include "Krb5Properties.h"
#include "win-mac.h"
//#include <krb5.h>
//#include "k5-int.h"
#include "lglobals.h"
#include "LeashView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CKrb5ConfigFileLocation dialog

IMPLEMENT_DYNCREATE(CKrb5ConfigFileLocation, CPropertyPage)

CKrb5ConfigFileLocation::CKrb5ConfigFileLocation()
    : CPropertyPage(CKrb5ConfigFileLocation::IDD)
{
    m_initConfigFile = _T("");  
    m_initTicketFile = _T(""); 
    m_newConfigFile = _T("");
    m_newTicketFile = _T("");
    m_startupPage1 = TRUE;
	
    //{{AFX_DATA_INIT(CKrb5ConfigFileLocation)
    //}}AFX_DATA_INIT
}

void CKrb5ConfigFileLocation::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CKrb5ConfigFileLocation)
    DDX_Control(pDX, IDC_EDIT_KRB5_TXT_FILE, m_ticketEditBox);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKrb5ConfigFileLocation, CDialog)
    //{{AFX_MSG_MAP(CKrb5ConfigFileLocation)
    ON_BN_CLICKED(IDC_BUTTON_KRB5INI_BROWSE, OnButtonKrb5iniBrowse)
    ON_BN_CLICKED(IDC_BUTTON_KRB5_TICKETFILE_BROWSE, OnButtonKrb5TicketfileBrowse)
    ON_EN_CHANGE(IDC_EDIT_KRB5_TXT_FILE, OnChangeEditKrb5TxtFile)
    ON_EN_CHANGE(IDC_EDIT_KRB5INI_LOCATION, OnChangeEditKrb5iniLocation)
    ON_WM_SHOWWINDOW()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CKrb5ConfigFileLocation::OnApply()
{
    BOOL tooManySlashes = FALSE; 
    BOOL foundError = FALSE;

    // KRB5.INI file 
    if (0 != m_newConfigFile.CompareNoCase(m_initConfigFile))
    { // Different path for Krb5.ini

        if (IsDlgButtonChecked(IDC_CHECK_CONFIRM_KRB5_EXISTS))
        {
            // Check for extra slashes at end of path
            LPSTR pSlash = strrchr(m_newConfigFile, '\\');
            if (pSlash && *(pSlash - 1) == '\\')
            { // don't commit changes 
                tooManySlashes = TRUE;
            }
            else if (pSlash && *(pSlash + 1) == '\0')
            { // commit changes, but take out slash at the end of path
                *pSlash = 0;
            }
		    
            // Check for invalid path
            Directory directory(m_newConfigFile);
            if (tooManySlashes || !directory.IsValidFile()) 
            { // don't commit changes
                foundError = TRUE;
                
                if (tooManySlashes)
                    LeashErrorBox("OnApply::Too Many Slashes At End of "
                                  "Selected Directory", 
                                  m_newConfigFile); 
                else					        
                    LeashErrorBox("OnApply::Selected file doesn't exist", 
                                  m_newConfigFile); 

                SetDlgItemText(IDC_EDIT_KRB5INI_LOCATION, m_initConfigFile);
            }
            else
            {   
                // more error checking
                CHAR confname[MAX_PATH];

                const char *filenames[2];
                filenames[0] = m_newConfigFile;
                filenames[1] = NULL;

                pprofile_init(filenames, &CLeashApp::m_krbv5_profile);

                const char*  rootSection[] = {"realms", NULL};
                const char** rootsec = rootSection;	
                char **sections = NULL; 

                long retval = pprofile_get_subsection_names(
                    CLeashApp::m_krbv5_profile, 
                    rootsec, &sections
                    );
                if (retval || !*sections )
                {
                    foundError = TRUE;
                    MessageBox("Your file selection is either corrupt or not a Kerberos Five Config. file", 
                               "Leash32", MB_OK);

                    // Restore old 'valid' config. file
                    if (CLeashApp::GetProfileFile(confname, sizeof(confname)))
                    {
                        foundError = TRUE;
                        MessageBox("Can't locate Kerberos Five Config. file!",
                                   "Error", MB_OK);
                        return TRUE;
                    }
	        
                    filenames[0] = confname;
                    filenames[1] = NULL;

                    pprofile_init(filenames, &CLeashApp::m_krbv5_profile);

                    pprofile_free_list(sections);
                    retval = pprofile_get_subsection_names(
                        CLeashApp::m_krbv5_profile, 
                        rootsec, &sections);

                    if (retval || !*sections)
                    {  
                        foundError = TRUE;
                        MessageBox("OnApply::There is a problem with your "
                                   "Kerberos Five Config. file!\n"
                                   "Contact your Administrator.", 
                                   "Leash32", MB_OK);
                    }
        
                    pprofile_free_list(sections);
                    SetDlgItemText(IDC_EDIT_KRB5INI_LOCATION, m_initConfigFile);
                 
                    return TRUE;
                }
                
                pprofile_free_list(sections);
    	    }
        }

        // Commit changes
        if (!foundError)
        {
            if (SetRegistryVariable("config", m_newConfigFile, 
                                    "Software\\MIT\\Kerberos5"))
            {
                MessageBox("Failed to set \"Krb.conf\"!", "Error", MB_OK);
            }
	        
            m_initConfigFile = m_newConfigFile;
            SetModified(TRUE);
        }    
    }	

    // Credential cache (ticket) file 
    // Ticket file
    if (0 != m_initTicketFile.CompareNoCase(m_newTicketFile))	  
    {
        if (getenv("KRB5_CONFIG"))
        {
            // Just in case they set (somehow) KRBTKFILE while this box is up
            MessageBox("OnApply::Ticket file is set in your System's"
                       "Environment!\nYou must first remove it.", 
                       "Error", MB_OK);

            return TRUE;
        }

        // Commit changes
        if (SetRegistryVariable("ticketfile", m_newTicketFile, 
                                "Software\\MIT\\Kerberos5"))
        {
            MessageBox("Failed to set \"ticketfile\"!", "Error", MB_OK);
        }

        m_initTicketFile = m_newTicketFile;
    }

    return TRUE;
}


BOOL CKrb5ConfigFileLocation::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CHAR confname[MAX_PATH];
    CHAR ticketName[MAX_PATH];

    CheckDlgButton(IDC_CHECK_CONFIRM_KRB5_EXISTS, TRUE);
    
    // Config. file (Krb5.ini)
    if (CLeashApp::GetProfileFile(confname, sizeof(confname)))
    {
        MessageBox("Can't locate Kerberos Five config. file!", "Error", MB_OK);
        return TRUE;     
    }

    m_initConfigFile = m_newConfigFile = confname;
    SetDlgItemText(IDC_EDIT_KRB5INI_LOCATION, m_initConfigFile);

    if (getenv("KRB5_CONFIG"))
    {
        GetDlgItem(IDC_EDIT_KRB5INI_LOCATION)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_KRB5INI_BROWSE)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_CONFIRM_KRB5_EXISTS)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_STATIC_INIFILES)->ShowWindow(FALSE);
    }


    // Set TICKET.KRB file Editbox
    *ticketName = NULL;
    if (CLeashApp::m_krbv5_context)
    {
        const char *pticketName = pkrb5_cc_default_name(CLeashApp::m_krbv5_context);
        
        if (pticketName)
            strcpy(ticketName, pticketName); 
    }
    
    if (!*ticketName)
    {
        MessageBox("OnInitDialog::Can't locate Kerberos Five ticket file!", 
                   "Error", MB_OK);
        return TRUE;
    }
    else
    { 
        m_initTicketFile = m_newTicketFile = ticketName;
        SetDlgItemText(IDC_EDIT_KRB5_TXT_FILE, m_initTicketFile);
    }

    if (getenv("KRB5CCNAME"))
        GetDlgItem(IDC_EDIT_KRB5_TXT_FILE)->EnableWindow(FALSE);
    else
        GetDlgItem(IDC_STATIC_TICKETFILE)->ShowWindow(FALSE);

    return TRUE;  
}

void CKrb5ConfigFileLocation::OnButtonKrb5iniBrowse() 
{
    CLeashFileDialog dlgFile(TRUE, NULL, "*.*", 
                             "Kerbereos Five Config. File (.ini)");
    dlgFile.m_ofn.lpstrTitle = "Select the Kerberos Five Config. File";
    while (TRUE)
    {
        if (IDOK == dlgFile.DoModal())
        {
            m_newConfigFile = dlgFile.GetPathName();
            SetDlgItemText(IDC_EDIT_KRB5INI_LOCATION, m_newConfigFile);
            break;
        }
        else
            break;
    }
}

void CKrb5ConfigFileLocation::OnButtonKrb5TicketfileBrowse() 
{
    CString ticket_path = "*.*";
    CLeashFileDialog dlgFile(TRUE, NULL, ticket_path, 
                             "Kerbereos Five Ticket File (Krb5cc)");
    dlgFile.m_ofn.lpstrTitle = "Select Credential Cache (Ticket) File";

    if (IDOK == dlgFile.DoModal()) 	
    {
        m_newTicketFile = dlgFile.GetPathName();
        SetDlgItemText(IDC_EDIT_KRB5_TXT_FILE, m_newTicketFile);
    }
}

void CKrb5ConfigFileLocation::OnChangeEditKrb5iniLocation() 
{
    if (!m_startupPage1)
    {
        GetDlgItemText(IDC_EDIT_KRB5INI_LOCATION, m_newConfigFile);
        SetModified(TRUE);
    }
}

void CKrb5ConfigFileLocation::OnChangeEditKrb5TxtFile() 
{
    if (!m_startupPage1)
    {
        GetDlgItemText(IDC_EDIT_KRB5_TXT_FILE, m_newTicketFile);
        SetModified(TRUE);
    }
}

void CKrb5ConfigFileLocation::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CDialog::OnShowWindow(bShow, nStatus);
    m_startupPage1 = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CKrb5ConfigOptions dialog

IMPLEMENT_DYNCREATE(CKrb5ConfigOptions, CPropertyPage)

CKrb5ConfigOptions::CKrb5ConfigOptions()
	: CPropertyPage(CKrb5ConfigOptions::IDD)
{
    m_initForwardable = 0;
    m_newForwardable = 0;
    m_initProxiable = 0;
    m_newProxiable = 0;
	
    //{{AFX_DATA_INIT(CKrb5ConfigOptions)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CKrb5ConfigOptions::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
	
    //{{AFX_DATA_MAP(CKrb5ConfigOptions)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKrb5ConfigOptions, CDialog)
    //{{AFX_MSG_MAP(CKrb5ConfigOptions)
    ON_BN_CLICKED(IDC_CHECK_FORWARDABLE, OnCheckForwardable)
    ON_BN_CLICKED(IDC_CHECK_PROXIABLE, OnCheckProxiable)
    ON_WM_HELPINFO()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CKrb5ConfigOptions::OnApply()
{
    if ((m_initForwardable == m_newForwardable) && 
        (m_initProxiable == m_newProxiable))
        return TRUE;	

    CWinApp *pApp = NULL; 
    pApp = AfxGetApp();
    if (!pApp)
    {
        MessageBox("There is a problem finding Leash32 application "
                   "information!", 
                   "Error", MB_OK);
    }
    else
    {
        pApp->WriteProfileInt("Settings", "ForwardableTicket", 
                              m_newForwardable);
        pApp->WriteProfileInt("Settings", "ProxiableTicket", 
                              m_newProxiable);
    }
    CLeashView::m_forwardableTicket = m_initForwardable = m_newForwardable;
    CLeashView::m_proxiableTicket = m_initProxiable = m_newProxiable;
    return TRUE;
}

BOOL CKrb5ConfigOptions::OnInitDialog()
{
    CDialog::OnInitDialog();

    CWinApp *pApp = NULL;
    pApp = AfxGetApp();
    if (!pApp)
    {
        MessageBox("There is a problem finding Leash32 application "
                   "information!", 
                   "Error", MB_OK);
    }
    else
    {
        m_initForwardable = pApp->GetProfileInt("Settings", 
                                                "ForwardableTicket", 0);
        m_initProxiable = pApp->GetProfileInt("Settings", 
                                              "ProxiableTicket", 0);
    }

    CheckDlgButton(IDC_CHECK_FORWARDABLE, m_initForwardable);
    m_newForwardable = m_initForwardable;

    CheckDlgButton(IDC_CHECK_PROXIABLE, m_initProxiable);
    m_newProxiable = m_initProxiable;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CKrb5ConfigOptions::OnCheckForwardable() 
{
    m_newForwardable = (BOOL)IsDlgButtonChecked(IDC_CHECK_FORWARDABLE);
    SetModified(TRUE);
}

void CKrb5ConfigOptions::OnCheckProxiable() 
{
    m_newProxiable = (BOOL)IsDlgButtonChecked(IDC_CHECK_PROXIABLE);
    SetModified(TRUE);
}

///////////////////////////////////////////////////////////////////////
// CKrb5Properties

IMPLEMENT_DYNAMIC(CKrb5Properties, CPropertySheet)

CKrb5Properties::CKrb5Properties(UINT nIDCaption, CWnd* pParentWnd,
                                 UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CKrb5Properties::CKrb5Properties(LPCTSTR pszCaption, CWnd* pParentWnd,
                                 UINT iSelectPage)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_fileLocation);
    AddPage(&m_configOptions);
}

CKrb5Properties::~CKrb5Properties()
{
}

void CKrb5Properties::OnHelp()
{
    AfxGetApp()->WinHelp(HID_KRB5_PROPERTIES_COMMAND); 	
}



BEGIN_MESSAGE_MAP(CKrb5Properties, CPropertySheet)
    //{{AFX_MSG_MAP(CKrb5Properties)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_COMMAND(ID_HELP, OnHelp)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
