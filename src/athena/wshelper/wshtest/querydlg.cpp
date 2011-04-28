// querydlg.cpp : implementation file
//

#include "stdafx.h"
#include "wshtest.h"
#include "querydlg.h"

#ifdef cork
#include <shmo.h>
#endif

#include <wshelper.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg dialog

CQueryDlg::CQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryDlg::IDD, pParent)
{
	long curopts;
	
	curopts = res_getopts();
	
	//{{AFX_DATA_INIT(CQueryDlg)
	m_Auth = (curopts & RES_AAONLY) != 0;
	m_DebugMsg = (curopts & RES_DEBUG) != 0;
	m_DefName = (curopts & RES_DEFNAMES) != 0;
	m_Host = "winftp.cica.indiana.edu";
	m_StayOpen = (curopts & RES_STAYOPEN) != 0;
	m_UseTCP = (curopts & RES_USEVC) != 0;
	m_Recurse = (curopts & RES_RECURSE) != 0;
	m_ByHow = 0;
	m_DnSrch = (curopts & RES_DNSRCH) != 0;
	m_Primary = (curopts & RES_PRIMARY) != 0;
	//}}AFX_DATA_INIT
}

void CQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryDlg)
	DDX_Check(pDX, IDC_Authoritative, m_Auth);
	DDX_Check(pDX, IDC_DebugMessages, m_DebugMsg);
	DDX_Check(pDX, IDC_DefName, m_DefName);
	DDX_Text(pDX, IDC_Host, m_Host);
	DDX_Check(pDX, IDC_StayOpen, m_StayOpen);
	DDX_Check(pDX, IDC_UseTCP, m_UseTCP);
	DDX_Check(pDX, IDC_Recurse, m_Recurse);
	DDX_Radio(pDX, IDC_ByName, m_ByHow);
	DDX_Check(pDX, IDC_DnSrch, m_DnSrch);
	DDX_Check(pDX, IDC_Primary, m_Primary);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQueryDlg, CDialog)
	//{{AFX_MSG_MAP(CQueryDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg message handlers

int CQueryDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CWnd *momWnd;
	RECT myRect, momRect;
	
	momWnd = GetParent();
	momWnd->GetClientRect(&momRect);
	GetClientRect(&myRect);
	SetWindowPos(NULL, (momRect.right-myRect.right)/2,
					(momRect.bottom-myRect.bottom)/2,
					0, 0, SWP_NOSIZE);
	
	return 0;
}
