//	File:			KrbProperties.cpp
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:	CPP file for KrbProperties.h. Contains variables and functions 
//					for Kerberos Four Properties
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	02/01/98	ADL		Original
//	**************************************************************************************


#include "stdafx.h"
//#include "k5-int.h"
#include "KrbProperties.h"
#include "Krb4Properties.h"

#include "Leash.h"
#include "wshelper.h"
//#include "krb.h"  <<-- compiler doesn't like this header file 
#include "lglobals.h"
//#include "LeashMessageBox.h" 
#include "reminder.h"

CHAR CKrbProperties::m_krbPath[MAX_PATH];
CHAR CKrbProperties::m_krbrealmPath[MAX_PATH];
BOOL CKrbProperties::KrbPropertiesOn;

///////////////////////////////////////////////////////////////////////
// CKrbProperties

IMPLEMENT_DYNAMIC(CKrbProperties, CPropertySheet)
CKrbProperties::CKrbProperties(UINT nIDCaption, CWnd* pParentWnd,
                               UINT iSelectPage)
:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CKrbProperties::CKrbProperties(LPCTSTR pszCaption, CWnd* pParentWnd,
							   UINT iSelectPage)
:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	KrbPropertiesOn = FALSE;
	
	if (CLeashApp::m_hKrb5DLL)
	{
		char *realm = NULL; 
		pkrb5_get_default_realm(CLeashApp::m_krbv5_context, &realm); 
		
		if (!realm)
		{
			MessageBox("CKrbProperties::There maybe a problem with your Kerberos Config. file.\
                        \n Consult your Administrator!", 
					   "Error", MB_OK);

			return;
		}
	}
	
    if (CLeashApp::m_hKrb5DLL && !CLeashApp::m_hKrb4DLL)
	{ // hold krb.con where krb5.ini is located
		CHAR krbConFile[MAX_PATH];
	    //strcpy(krbConFile, CLeashApp::m_krbv5_profile->first_file->filename);
        if (CLeashApp::GetProfileFile(krbConFile, sizeof(krbConFile)))	
        {
            MessageBox("Can't locate Kerberos Five Config. file!", "Error", MB_OK);
            return;
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
		
		strcpy(m_krbPath, krbConFile);
	}
	else if (CLeashApp::m_hKrb4DLL)
	{ 
		unsigned int krbPathSize = sizeof(m_krbPath);
        memset(m_krbPath, '\0', sizeof(m_krbPath));
 		if (!pkrb_get_krbconf2(m_krbPath, &krbPathSize))
		{ // Error has happened
			; //m_noKrbFileStartupWarning = TRUE;
		}
	}

    if (CLeashApp::m_hKrb5DLL && !CLeashApp::m_hKrb4DLL)
	{ // hold krb.con where krb5.ini is located
		CHAR krbRealmConFile[MAX_PATH];
		//strcpy(krbRealmConFile, CLeashApp::m_krbv5_profile->first_file->filename);
		if (CLeashApp::GetProfileFile(krbRealmConFile, sizeof(krbRealmConFile)))	
        {
            MessageBox("Can't locate Kerberos Five Config. file!", "Error", MB_OK);
            return;
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
		
		strcpy(m_krbrealmPath, krbRealmConFile);
	}
	else if (CLeashApp::m_hKrb4DLL)
	{ 
		unsigned int krbrealmPathSize = sizeof(m_krbrealmPath);
        memset(m_krbrealmPath, '\0', sizeof(m_krbrealmPath));
        if (!pkrb_get_krbrealm2(m_krbrealmPath, &krbrealmPathSize))
		{ // Error has happened
			; //m_noKrbrealmFileStartupWarning = TRUE;
		}
	}	

	AddPage(&m_configOptions);
	AddPage(&m_miscConfigOpt);
	
	if (CLeashApp::m_hKrb4DLL && !CLeashApp::m_hKrb5DLL)
	{
		AddPage(&m_krb4RealmHostMaintenance);
		AddPage(&m_krb4DomainRealmMaintenance);
	}
	else if (CLeashApp::m_hKrb5DLL)
	{
		AddPage(&m_realmHostMaintenance); 
		AddPage(&m_domainRealmMaintenance);
	}

	KrbPropertiesOn = TRUE;
}

CKrbProperties::~CKrbProperties()
{
	KrbPropertiesOn = FALSE;
}

void CKrbProperties::OnHelp()
{
    AfxGetApp()->WinHelp(HID_KERBEROS_PROPERTIES_COMMAND); 	
}


BEGIN_MESSAGE_MAP(CKrbProperties, CPropertySheet)
	//{{AFX_MSG_MAP(CKrbProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
    ON_COMMAND(ID_HELP, OnHelp)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////
// CKrbProperties message handlers

