//	**************************************************************************************
//	File:			LeashProperties.cpp
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:	CPP file for LeashProperties.h. Contains variables and functions 
//					for the Leash Properties Dialog Box
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	12/02/98	ADL		Original
//	**************************************************************************************

#include "stdafx.h"
#include "leash.h"
#include "LeashProperties.h"
#include "LeashMessageBox.h" 
#include <leashinfo.h>
#include "lglobals.h"
#include "reminder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeashProperties dialog

char CLeashProperties::timeServer[255] = {NULL};

CLeashProperties::CLeashProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CLeashProperties::IDD, pParent)
{
	m_noFileError = FALSE;

	//{{AFX_DATA_INIT(CLeashProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLeashProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLeashProperties)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLeashProperties, CDialog)
	//{{AFX_MSG_MAP(CLeashProperties)
	ON_BN_CLICKED(IDC_BUTTON_LEASHINI_HELP2, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeashProperties message handlers

BOOL CLeashProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    pLeashGetTimeServerName(timeServer, TIMEHOST); 
    SetDlgItemText(IDC_EDIT_TIME_SERVER, timeServer);

   	if (getenv(TIMEHOST))
        GetDlgItem(IDC_EDIT_TIME_SERVER)->EnableWindow(FALSE);
    else
        GetDlgItem(IDC_STATIC_TIMEHOST)->ShowWindow(FALSE);

    return TRUE;  
}

void CLeashProperties::OnOK() 
{
	CString timeServer_;
	GetDlgItemText(IDC_EDIT_TIME_SERVER, timeServer_);

	if (getenv(TIMEHOST))
    {   
        // Check system for TIMEHOST, just in case it gets set (somehow)
        MessageBox("Can't change the time host unless you remove it from the environment!", 
                   "Error", MB_OK);
        return;
    }
    
    if (SetRegistryVariable(TIMEHOST, timeServer_))
	{
		MessageBox("There was an error putting your entry into the Registry!", 
                   "Error", MB_OK);
    }
    

	CDialog::OnOK();
}

BOOL CLeashProperties::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (m_noFileError)
	{
		LeashErrorBox("Can't locate INI File", sysDir);		
		m_noFileError = FALSE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CLeashProperties::OnHelp() 
{
    AfxGetApp()->WinHelp(HID_LEASH_PROPERTIES_COMMAND); 	
}
