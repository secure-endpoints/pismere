//*****************************************************************************
// File:	KrbMiscConfigOpt.cpp
// By:		Paul B. Hill
// Created:	08/12/1999
// Copyright:	@1999 Massachusetts Institute of Technology - All rights 
//		reserved.
// Description: CPP file for KrbMiscConfigOpt.cpp.  Contains variables
//		and functions for Kerberos Properties.
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 08/12/99	PBH	Original
//*****************************************************************************

#include "stdafx.h"
#include "Leash.h"
#include "KrbProperties.h"
#include "KrbMiscConfigOpt.h" 
#include "LeashFileDialog.h"
#include "LeashMessageBox.h"
#include "lglobals.h"
#include <direct.h>
#include "reminder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////
// CKrbMiscConfigOpt property page

CString CKrbMiscConfigOpt::m_DefaultLifeTime;
CString CKrbMiscConfigOpt::m_initDefaultLifeTime;
CString CKrbMiscConfigOpt::m_newDefaultLifeTime;
void CKrbMiscConfigOpt::ResetDefaultLifeTimeEditBox();
CEdit CKrbMiscConfigOpt::m_krbLifeTimeEditbox;

IMPLEMENT_DYNCREATE(CKrbMiscConfigOpt, CPropertyPage)

CKrbMiscConfigOpt::CKrbMiscConfigOpt() : CPropertyPage(CKrbMiscConfigOpt::IDD)
{
    m_DefaultLifeTime = _T("");
    m_noLifeTime = FALSE;

	//{{AFX_DATA_INIT(CKrbConfigOptions)
	//}}AFX_DATA_INIT
}

CKrbMiscConfigOpt::~CKrbMiscConfigOpt()
{
}

VOID CKrbMiscConfigOpt::DoDataExchange(CDataExchange* pDX)
{
    TRACE("Entering CKrbMiscConfigOpt::DoDataExchange -- %d\n",
          pDX->m_bSaveAndValidate);
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CKrbMscConfigOpt)
    DDX_Control(pDX, IDC_EDIT_DEFAULT_LIFETIME, m_krbLifeTimeEditbox);
    //}}AFX_DATA_MAP
}


BOOL CKrbMiscConfigOpt::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();

    LPTSTR buf = m_DefaultLifeTime.GetBuffer(80);
    DWORD lifetime = pLeash_get_default_lifetime();

    _itoa(lifetime, buf, 10);
    m_DefaultLifeTime.ReleaseBuffer();
    m_initDefaultLifeTime = m_DefaultLifeTime;

    if (lifetime)
        m_noLifeTime = FALSE; // We now have the value.
    else
        m_noLifeTime = TRUE;

    return(TRUE);
}

BOOL CKrbMiscConfigOpt::OnApply()
{
    // If no changes were made, quit this function
    if (0 == m_initDefaultLifeTime.CompareNoCase(m_DefaultLifeTime))
        return TRUE;
	
    m_DefaultLifeTime.TrimLeft();
    m_DefaultLifeTime.TrimRight();
    if(m_DefaultLifeTime.IsEmpty())
    {
        MessageBox("The ticket lifetime must be filled in.",
                    "Leash32", MB_OK);
        m_DefaultLifeTime = m_initDefaultLifeTime;
        SetDlgItemText(IDC_EDIT_DEFAULT_LIFETIME, m_DefaultLifeTime );
        return FALSE;
    }

    // If we're using an environment variable tell the user that we
    // can't use Leash to modify the value.

    if( getenv("LIFETIME") !=  NULL)
    {
        MessageBox("The ticket lifetime is being controlled by the environment"
                   "variable LIFETIME instead of the regsitry. Leash32 cannot modify"
                   "the environment. Use the System control panel instead.", 
                    "Leash32", MB_OK);
        return(FALSE);
    }

    if (!atoi(m_DefaultLifeTime))
    {
        MessageBox("A lifetime setting of 0 is special in that it means that "
                   "the application is free to pick whatever default it deems "
                   "appropriate",
                   "Leash32", MB_OK);
    }

    // Make this the active ticket lifetime and save it to the regsitry
    if(pLeash_set_default_lifetime(atoi(m_DefaultLifeTime)))
    {
        MessageBox("Unable to save the ticket lifetime to the regsitry."
                   "Leash32", MB_OK);
        return(FALSE);
    }
    m_initDefaultLifeTime = m_DefaultLifeTime;
    return TRUE;
}

void CKrbMiscConfigOpt::OnSelchangeEditDefaultLifeTime() 
{
    if (!m_startupPage2)
    {
        char selLifetime[5];
        strcpy( selLifetime, m_newDefaultLifeTime);

        GetDlgItemText(IDC_EDIT_DEFAULT_LIFETIME, m_DefaultLifeTime);
        SetModified(TRUE);
    }
}

void CKrbMiscConfigOpt::OnEditchangeEditDefaultLifeTime() 
{
    if (!m_startupPage2)
    {
        GetDlgItemText(IDC_EDIT_DEFAULT_LIFETIME, m_DefaultLifeTime);
        SetModified(TRUE);
    }
}

void CKrbMiscConfigOpt::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CPropertyPage::OnShowWindow(bShow, nStatus);

    if (CLeashApp::m_hKrb5DLL)
        ResetDefaultLifeTimeEditBox();

    SetDlgItemText(IDC_EDIT_DEFAULT_LIFETIME, m_DefaultLifeTime);
}

void CKrbMiscConfigOpt::ResetDefaultLifeTimeEditBox()
{ 
    // Reset Config Tab's Default LifeTime Editbox

    LPTSTR buf = m_DefaultLifeTime.GetBuffer(80);
    DWORD lifetime = pLeash_get_default_lifetime();

    if(lifetime)
    {
        _itoa(lifetime, buf, 10);
    }
    m_DefaultLifeTime.ReleaseBuffer();

    ::SetDlgItemText(::GetForegroundWindow(), IDC_EDIT_DEFAULT_LIFETIME, 
                     m_DefaultLifeTime);
}

BOOL CKrbMiscConfigOpt::PreTranslateMessage(MSG* pMsg) 
{
    if (!m_startupPage2)
    {
        if (m_noLifeTime)
        {
            MessageBox("A lifetime setting of 0 is special in that it means that "
                       "the application is free to pick whatever default it deems "
                       "appropriate",
                       "Leash32", MB_OK);
            m_noLifeTime = FALSE;
        }
    }

    m_startupPage2 = FALSE;	
    return CPropertyPage::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CKrbMiscConfigOpt, CPropertyPage)
	//{{AFX_MSG_MAP(CKrbConfigOptions)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_DEFAULT_LIFETIME, OnEditchangeEditDefaultLifeTime)
	ON_CBN_SELCHANGE(IDC_EDIT_DEFAULT_LIFETIME, OnSelchangeEditDefaultLifeTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
