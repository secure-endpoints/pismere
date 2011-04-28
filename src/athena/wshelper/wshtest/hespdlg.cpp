// hespdlg.cpp : implementation file
//

#include "stdafx.h"
#include "wshtest.h"
#include "hespdlg.h"
#include "hesdlg.h"

#include <wshelper.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHespDlg dialog

CHespDlg::CHespDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHespDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHespDlg)
	//}}AFX_DATA_INIT
}

void CHespDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHespDlg)
	DDX_Control(pDX, IDC_HespText, m_HespText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHespDlg, CDialog)
	//{{AFX_MSG_MAP(CHespDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHespDlg message handlers

BOOL CHespDlg::OnInitDialog()
{
	CHesDlg hdlg;
  
    char **cpp;
	char hespbuf[1024];
    
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = 0x0101;

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
	     HIBYTE( wsaData.wVersion ) != 1 ) {
	WSACleanup( );
	return 0;   

	}

	CDialog::OnInitDialog();
	
	int ret=hdlg.DoModal();

	if (ret == IDCANCEL)
	{
		m_HespText.SetWindowText("Query cancelled");
	    WSACleanup();
	    return TRUE;
	}

	cpp = hes_resolve(hdlg.m_HesIdent.GetBuffer(0),
					  hdlg.m_HesType.GetBuffer(0));
	if (cpp == NULL) { 
		switch(hes_error()) {
		case 0:
			break;
		case HES_ER_NOTFOUND:
			m_HespText.SetWindowText("Hesiod name not found");
			break;
		case HES_ER_CONFIG:
			m_HespText.SetWindowText("Hesiod configuration error");
			break;
		default:
			m_HespText.SetWindowText("Unknown Hesiod error");
			break;
		}
	} else {
		while(*cpp) wsprintf(hespbuf, "%s\n", *cpp++);
  		hespbuf[strlen(hespbuf)-1] = '\0';
		m_HespText.SetWindowText(hespbuf);
	}

	WSACleanup();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CHespDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}
