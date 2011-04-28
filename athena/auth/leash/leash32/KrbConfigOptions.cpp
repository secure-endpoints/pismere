//	**************************************************************************************
//	File:			KrbConfigOptions.cpp
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:	CPP file for KrbProperties.h. Contains variables and functions 
//					for Kerberos Four and Five Properties
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	2/01/98	ADL		Original
//	**************************************************************************************


#include "stdafx.h"
#include "Leash.h"
#include "KrbProperties.h"
#include "KrbConfigOptions.h" 
#include "LeashFileDialog.h"
#include "LeashMessageBox.h"
#include "wshelper.h"
//#include "krb.h"  <<-- compiler doesn't like this header file 
//#include "k5-int.h"
#include "lglobals.h"
#include <io.h>
#include <direct.h>
#include "reminder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////
// CKrbConfigOptions property page

CString CKrbConfigOptions::m_newDefaultRealm;
CString CKrbConfigOptions::m_hostServer;
CComboBox CKrbConfigOptions::m_krbRealmEditbox;
void CKrbConfigOptions::ResetDefaultRealmComboBox();
BOOL CKrbConfigOptions::m_profileError;
BOOL CKrbConfigOptions::m_dupEntiesError; 

IMPLEMENT_DYNCREATE(CKrbConfigOptions, CPropertyPage)

CKrbConfigOptions::CKrbConfigOptions() : CPropertyPage(CKrbConfigOptions::IDD)
{
	m_initDefaultRealm = _T("");
	m_newDefaultRealm = _T("");
	m_startupPage2 = TRUE;
	m_noKrbFileError = FALSE;
	m_noKrbhostWarning = FALSE;
	m_dupEntiesError = FALSE;
	m_profileError	= FALSE;
	m_noRealm = FALSE;

	//{{AFX_DATA_INIT(CKrbConfigOptions)
	//}}AFX_DATA_INIT
}

CKrbConfigOptions::~CKrbConfigOptions()
{
}

VOID CKrbConfigOptions::DoDataExchange(CDataExchange* pDX)
{
	TRACE("Entering CKrbConfigOptions::DoDataExchange -- %d\n",
	      pDX->m_bSaveAndValidate);
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKrbConfigOptions)
	DDX_Control(pDX, IDC_EDIT_DEFAULT_REALM, m_krbRealmEditbox);
	//}}AFX_DATA_MAP
}

static char far * near parse_str(char far*buffer,char far*result)
{
        while (isspace(*buffer))
                buffer++;
        while (!isspace(*buffer))
                *result++=*buffer++;
        *result='\0';
        return buffer;
}

krb_get_krbhst(char* h, char* r, int n)
{
	char lbstorage[BUFSIZ];
    char tr[REALM_SZ];
    static FILE *cnffile; /*XXX pbh added static because of MS bug in fgets() */
    static char FAR *linebuf; /*XXX pbh added static because of MS bug in fgets() */
    int i;
    char *p;

    //static char buffer[80];
    //krb_get_krbconf(buffer);
    memset(lbstorage, '\0', BUFSIZ ); /* 4-22-94 */
    linebuf = &lbstorage[0];

    if ((cnffile = fopen(CKrbProperties::m_krbPath,"r")) == NULL) {
        if (n==1) {
            (void) strcpy(h,KRB_HOST);  
            return(KSUCCESS);
        } else {
            return(KFAILURE);
        }
    }
    /* linebuf=(char FAR *)malloc(BUFSIZ); */ /*4-22-94*/
    if (fgets(linebuf,BUFSIZ,cnffile)==NULL) {
            /* free(linebuf); */ /* 4-22-94 */ 

            return(KFAILURE);
    }
    /* bzero( tr, sizeof(tr) ); */   /* pbh 2-24-93 */
    memset(tr, '\0', sizeof(tr) );
    parse_str(linebuf,tr);
    if (*tr=='\0') {   
            return (KFAILURE);  
    }        
    /* run through the file, looking for the nth server for this realm */
    for (i = 1; i <= n;) {
        if (fgets(linebuf, BUFSIZ, cnffile) == NULL) {
            /* free(linebuf); */ /*4-22-94*/  
            (void) fclose(cnffile);
            return(KFAILURE);
        }
        /* bzero( tr, sizeof(tr) ); */   /* pbh 2-24-93 */
        memset(tr, '\0', sizeof(tr) );
        p=parse_str(linebuf,tr);
        if (*tr=='\0')
                continue;
        memset(h, '\0', lstrlen(h) );
        parse_str(p,h);
        if (*tr=='\0')
                continue;
        if (!lstrcmp(tr,r))
                i++;
    }
    /* free(linebuf); */ /*4-22-94*/     
    (void) fclose(cnffile);
    return(KSUCCESS);
}

BOOL CKrbConfigOptions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (CLeashApp::m_hKrb4DLL && !CLeashApp::m_hKrb5DLL)
	{  // Krb4 NOT krb5	
		// Fill in all edit boxes
		char krbRealm[REALM_SZ + 1];
		char krbhst[MAX_HSTNM + 1];
		CStdioFile krbCon;
		if (!krbCon.Open(CKrbProperties::m_krbPath, CFile::modeRead)) 
		{	
			SetDlgItemText(IDC_EDIT_DEFAULT_REALM, KRB_REALM);
			SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, KRB_MASTER);
			//CheckRadioButton(IDC_RADIO_ADMIN_SERVER, IDC_RADIO_NO_ADMIN_SERVER, IDC_RADIO_NO_ADMIN_SERVER);
			m_initDefaultRealm = m_newDefaultRealm = KRB_REALM;
		}
		else
		{ // place krbRealm in Edit box
			memset(krbRealm, '\0', sizeof(krbRealm));
			if (!krbCon.ReadString(krbRealm, sizeof(krbRealm)) || '\n' == *krbRealm  || 
				'\n' == *krbRealm)
			{
				//MessageBox("OnInitDialog::Can't Locate Your Kerberos Realm!", 
				//	       "Error", MB_OK);

				SetDlgItemText(IDC_EDIT_DEFAULT_REALM, KRB_REALM);
				SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, KRB_MASTER);
				//CheckRadioButton(IDC_RADIO_ADMIN_SERVER, IDC_RADIO_NO_ADMIN_SERVER, IDC_RADIO_NO_ADMIN_SERVER);
				m_initDefaultRealm = m_newDefaultRealm = KRB_REALM;
			}
			else
			{ // check for Host
				// don't use - would have to re-logon, on file location change, to use this function
				//if (KFAILURE == pkrb_get_lrealm(krbRealm, 0))
				//{
				//	MessageBox("OnInitDialog::Can't locate Default Local Realm!", 
				//		       "Error", MB_OK);
				//}
				//	
		 
				*(krbRealm + strlen(krbRealm) - 1) = 0;  
				LPSTR pSpace = strchr(krbRealm, ' ');
				if (pSpace)
				  *pSpace = 0;
				
				//GetDlgItem(IDC_EDIT_DEFAULT_REALM)->SetWindowText(krbRealm);
				m_initDefaultRealm = m_newDefaultRealm = krbRealm;
					
				memset(krbhst, '\0', sizeof(krbhst));
				krbCon.Close();
			
				// Check for Host
				// don't use KRB4 - krb_get_krbhst - would have to re-logon, on file location 
				// change, to use this function
				if (KFAILURE == pkrb_get_krbhst(krbhst, krbRealm, 1))
				{
					m_noKrbhostWarning = TRUE;
				}
				else
				{ // place hostname in Edit Box
					//SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, krbhst);	
					
					m_hostServer = krbhst;

					// New suff to put realms in Combo Box
					CStdioFile krbCon;
					if (!krbCon.Open(CKrbProperties::m_krbPath, CFile::modeRead)) 
					{	
						m_noKrbFileError = TRUE;
						return FALSE;	
					}
					
					LPSTR space = NULL;	
					CHAR lineBuf[REALM_SZ + MAX_HSTNM + 20];
					CHAR localRealm[REALM_SZ + MAX_HSTNM + 20];				
					memset(lineBuf, '\0', sizeof(lineBuf));
					memset(localRealm, '\0', sizeof(localRealm));
					
					if (krbCon.ReadString(localRealm, sizeof(localRealm)))
					  *(localRealm + strlen(localRealm) - 1) = 0;					
					else
					  return FALSE;

					space = strchr(localRealm, ' '); 
					if (space)
					  *space = 0;
					
					while (TRUE)
					{
						if (!krbCon.ReadString(lineBuf, sizeof(lineBuf)))
						  break;

						*(lineBuf + strlen(lineBuf) - 1) = 0;
						
						space = strchr(lineBuf, ' ');
						if (space)
						  *space = 0;
						else
						  ASSERT(0);

						if (CB_ERR == m_krbRealmEditbox.FindStringExact(-1, lineBuf))
						{ // no dups
							if (LB_ERR == m_krbRealmEditbox.AddString(lineBuf))
							{
								MessageBox("OnInitDialog::Can't add to Kerberos Realm Combobox", 
										   "Leash32", MB_OK);
								return FALSE;
							}
						}	
					}
					  
					
					m_krbRealmEditbox.SelectString(-1, krbRealm);

				} // end of 'place hostname in Edit Box' else statement
			} // end of 'Check for Host' else statement
		} // end of 'place krbRealm in Edit box' else
	}
	else if (CLeashApp::m_hKrb5DLL)
	{ // Krb5 OR krb5 AND krb4	
		char *realm = NULL; 
		pkrb5_get_default_realm(CLeashApp::m_krbv5_context, &realm); 
		
		if (!realm)
		{
			m_noRealm = TRUE;
			return TRUE;
		}

		m_initDefaultRealm = m_newDefaultRealm = realm; 
	}	

	// Set host and domain names in their Edit Boxes, respectively.
	char hostName[80];
	char domainName[80];
	int ckHost = wsh_gethostname(hostName, sizeof(hostName));	
	int ckdomain = wsh_getdomainname(domainName, sizeof(domainName));	
	CString dot_DomainName = "."; 
	dot_DomainName += domainName;
		
	SetDlgItemText(IDC_EDIT_HOSTNAME, hostName);  
	SetDlgItemText(IDC_EDIT_DOMAINNAME, dot_DomainName);  
	
	return FALSE;  
}

BOOL CKrbConfigOptions::OnApply()
{
	// If no changes were made, quit this function
	if (0 == m_initDefaultRealm.CompareNoCase(m_newDefaultRealm))
	  return TRUE;
	
	m_newDefaultRealm.TrimLeft();
	m_newDefaultRealm.TrimRight();
	
	if (m_newDefaultRealm.IsEmpty())
	{
		MessageBox("OnApply::Your Kerberos Realm field must be filled in!", 
                    "Leash32", MB_OK);
		m_newDefaultRealm = m_initDefaultRealm;
		SetDlgItemText(IDC_EDIT_DEFAULT_REALM, m_newDefaultRealm);
		return TRUE;
	}

	CStdioFile krbCon;
	if (!krbCon.Open(CKrbProperties::m_krbPath, CFile::modeCreate |
												CFile::modeNoTruncate |
												CFile::modeRead)) 
	{	
		LeashErrorBox("OnApply::Can't open configuration file", 
					  CKrbProperties::m_krbPath);
		return TRUE;
	}

	CStdioFile krbCon2;
	CString krbCon2File = CKrbProperties::m_krbPath;
	krbCon2File += "___";
	if (!krbCon2.Open(krbCon2File, CFile::modeCreate | CFile::modeWrite)) 
	{	
		LeashErrorBox("OnApply:: Can't open configuration file", 
					  CKrbProperties::m_krbPath);
		return TRUE;
	}

	CString readWrite;
	krbCon.ReadString(readWrite);
	krbCon2.WriteString(m_newDefaultRealm);
	krbCon2.WriteString("\n");
	while (krbCon.ReadString(readWrite))
	{
		krbCon2.WriteString(readWrite);
		krbCon2.WriteString("\n");
	}
	
	krbCon.Close();
	krbCon2.Close();
	krbCon2.Remove(CKrbProperties::m_krbPath);
	krbCon2.Rename(krbCon2File, CKrbProperties::m_krbPath);

	if (CLeashApp::m_hKrb5DLL)
	{ // Krb5 OR krb5 AND krb4	

		const char*  Names[] = {"libdefaults", "default_realm", NULL}; 
		const char** names = Names;	
		
		CHAR orgRealm[REALM_SZ];
		CHAR selRealm[REALM_SZ];
		GetDlgItemText(IDC_EDIT_DEFAULT_REALM, selRealm, sizeof(selRealm));
		strcpy(orgRealm, m_initDefaultRealm);
		
		long retval = pprofile_update_relation(CLeashApp::m_krbv5_profile, 
									            names, orgRealm, selRealm);
	
		if (retval) 
		{
			MessageBox("OnApply::There is on error, profile will not be saved!!!\
                        \nIf this error persist, contact your administrator.", 
					   "Leash32", MB_OK);
			return TRUE;
		}
		
		// Save to Kerberos Five config. file "Krb5.ini"
	    retval = pprofile_flush(CLeashApp::m_krbv5_profile);
	}
	
	m_initDefaultRealm = m_newDefaultRealm;	  
	return TRUE;
}

void CKrbConfigOptions::OnSelchangeEditDefaultRealm() 
{
	if (!m_startupPage2)
	{
		GetDlgItemText(IDC_EDIT_DEFAULT_REALM, m_newDefaultRealm);
		SetModified(TRUE);

		if (CLeashApp::m_hKrb5DLL)
		{
			CHAR selRealm[REALM_SZ];
			strcpy(selRealm, m_newDefaultRealm);
			const char*  Section[] = {"realms", selRealm, "kdc", NULL}; 
			const char** section = Section;	
			char **values = NULL; 
		
			long retval = pprofile_get_values(CLeashApp::m_krbv5_profile, 
											   section, &values);
	
			if (retval)
			{
				MessageBox("OnInitDialog::There is on error, profile will not be saved!!!\
                            \nIf this error persist, contact your administrator.", 
						   "Leash32", MB_OK);
				return;
			}

			if (values)
			  SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, *values);
		}
		else
		{
			CHAR krbhst[MAX_HSTNM + 1];
			CHAR krbrlm[REALM_SZ + 1];
				
			strcpy(krbrlm, CKrbConfigOptions::m_newDefaultRealm);
			memset(krbhst, '\0', sizeof(krbhst));

			// Check for Host
			// don't use KRB4 - krb_get_krbhst - would have to re-logon, on file location 
			// change, to use this function
			if (KFAILURE == pkrb_get_krbhst(krbhst, krbrlm, 1))
			{
				MessageBox("We can't find the Host Server for your Default Realm!!!\
                            \n 'Apply' your changes and try again.", 
					        "Leash32", MB_OK);
			    SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, "");
				return;
			}

			m_hostServer = krbhst;
			if (strlen(krbhst))
			  SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, m_hostServer);
		}
	}
}

void CKrbConfigOptions::OnEditchangeEditDefaultRealm() 
{
	if (!m_startupPage2)
	{
		GetDlgItemText(IDC_EDIT_DEFAULT_REALM, m_newDefaultRealm);
		SetModified(TRUE);
	}
}

void CKrbConfigOptions::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	if (CLeashApp::m_hKrb5DLL)
	  ResetDefaultRealmComboBox();

	SetDlgItemText(IDC_EDIT_REALM_HOSTNAME, m_hostServer);
}

void CKrbConfigOptions::ResetDefaultRealmComboBox()
{ // Krb5 is loaded
	// Reset Config Tab's Default Realm Combo Editbox
	const char*  rootSection[] = {"realms", NULL};
	const char** rootsec = rootSection;	
	char **sections = NULL, 
		 **cpp = NULL;
			
	long retval = pprofile_get_subsection_names(CLeashApp::m_krbv5_profile, 
												 rootsec , &sections);
	
	if (retval)
	{
		m_profileError = TRUE;
		return;
	}
	
	m_krbRealmEditbox.ResetContent();

	for (cpp = sections; *cpp; cpp++) 
	{
		if (CB_ERR == m_krbRealmEditbox.FindStringExact(-1, *cpp))
		{ // no dups
			if (CB_ERR == m_krbRealmEditbox.AddString(*cpp))
			{
				::MessageBox(NULL, "OnInitDialog::Can't add to Kerberos Realm Combobox", 
							 "Leash32", MB_OK);
				return;
			}
		}
		else 
		  m_dupEntiesError = TRUE;
	}

	m_krbRealmEditbox.SelectString(-1, m_newDefaultRealm);
	
	const char*  Section[] = {"realms", m_newDefaultRealm, "kdc", NULL}; //theSection
	const char** section = Section;	
	char **values = NULL; 
	
	retval = pprofile_get_values(CLeashApp::m_krbv5_profile, 
								  section, &values);

	if (retval) 
	{
		m_profileError = TRUE;
		m_hostServer = _T("");
		return;
	}
	
	if (values)
	{
		m_hostServer = *values;
		::SetDlgItemText(::GetForegroundWindow(), IDC_EDIT_REALM_HOSTNAME, m_hostServer);
	}
}

BOOL CKrbConfigOptions::PreTranslateMessage(MSG* pMsg) 
{
	if (!m_startupPage2)
	{
		if (m_noKrbFileError)
		{
			LeashErrorBox("OnInitDialog::Can't open configuration file", 
				!strlen(CKrbProperties::m_krbPath) ? KRB_FILE :
				CKrbProperties::m_krbPath);	
			m_noKrbFileError = FALSE;
		}

		if (m_noKrbhostWarning)
		{
			MessageBox("OnInitDialog::Can't locate Your Kerberos Host for Your Kerberos Realm!", 
					   "Leash32", MB_OK);
			m_noKrbhostWarning = FALSE;
		}
	
		if (m_dupEntiesError)
		{
			MessageBox("Found an error (duplicate items) in your Kerberos Five Config. File!!!\
                        \nPlease contract your Administrator.",
					   "Leash32", MB_OK);

			m_dupEntiesError = FALSE;
		}
	
		if (m_profileError)
		{
			MessageBox("OnInitDialog::There is on error, profile will not be saved!!!\
                        \nIf this error persist, contact your administrator.", 
				       "Leash32", MB_OK);

			m_profileError	= FALSE;
		}

		if (m_noRealm)
		{
			MessageBox("OnInitDialog::There is a problem with your Kerberos Config. file (No Default Realm).\
                        \n Consult your Administrator!", 
					   "Leash32", MB_OK);

			m_noRealm = FALSE;
		}
	}

	m_startupPage2 = FALSE;	
	return CPropertyPage::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CKrbConfigOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CKrbConfigOptions)
	ON_WM_SHOWWINDOW()
	ON_CBN_EDITCHANGE(IDC_EDIT_DEFAULT_REALM, OnEditchangeEditDefaultRealm)
	ON_CBN_SELCHANGE(IDC_EDIT_DEFAULT_REALM, OnSelchangeEditDefaultRealm)
	ON_BN_CLICKED(IDC_BUTTON_KRB_HELP, OnButtonKrbHelp)
	ON_BN_CLICKED(IDC_BUTTON_KRBREALM_HELP, OnButtonKrbrealmHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CKrbConfigOptions::OnButtonKrbHelp() 
{
	MessageBox("No Help Available!", "Leash32", MB_OK);	
}

void CKrbConfigOptions::OnButtonKrbrealmHelp() 
{
	MessageBox("No Help Available!", "Leash32", MB_OK);	
}
