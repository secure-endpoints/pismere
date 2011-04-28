// respdlg.cpp : implementation file
//

#include "stdafx.h"
#include "wshtest.h"
#include "respdlg.h"
#include "querydlg.h"

#include <ctype.h>
#include <wshelper.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRespDlg dialog

CRespDlg::CRespDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRespDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRespDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CRespDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRespDlg)
	DDX_Control(pDX, IDC_OHost, m_OHost);
	DDX_Control(pDX, IDC_AliasList, m_AliasList);
	DDX_Control(pDX, IDC_AddrList, m_AddrList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRespDlg, CDialog)
	//{{AFX_MSG_MAP(CRespDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRespDlg message handlers

BOOL CRespDlg::OnInitDialog()
{
	static CQueryDlg qdlg;

	struct hostent *host = NULL;
	struct mxent *mymx = NULL;
	LPSTR cp, randinfo = NULL;
	struct in_addr taddr;
	int i;
	long topts = 0;
	char tmpbuf[256];
	
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
	
	int ret=qdlg.DoModal();

	if (qdlg.m_Auth) topts |= RES_AAONLY;
	if (qdlg.m_DebugMsg) topts |= RES_DEBUG;
	if (qdlg.m_DefName) topts |= RES_DEFNAMES;
	if (qdlg.m_StayOpen) topts |= RES_STAYOPEN;
	if (qdlg.m_UseTCP) topts |= RES_USEVC;
	if (qdlg.m_Recurse) topts |= RES_RECURSE;
	if (qdlg.m_DnSrch) topts |= RES_DNSRCH;
	if (qdlg.m_Primary) topts |= RES_PRIMARY;

	res_setopts(topts);
	
	if (ret == IDCANCEL)
	{
		m_OHost.SetWindowText("Query cancelled");
	    WSACleanup();
	    return TRUE;
	}
	else
		m_OHost.SetWindowText("Not found");
	
	unsigned long tiaddr;
	
	switch (qdlg.m_ByHow)
	{
		case 0:
			host = rgethostbyname(qdlg.m_Host.GetBuffer(0));
			break;
		case 1:
			tiaddr = inet_addr(qdlg.m_Host.GetBuffer(0));
			host = rgethostbyaddr((char *) &tiaddr, 4, PF_INET);
			break;
		case 2:
		    mymx = (struct mxent FAR *) getmxbyname(qdlg.m_Host.GetBuffer(0));
		    break;
		case 3:
			randinfo = gethinfobyname(qdlg.m_Host.GetBuffer(0));
		    break;
	}
	
	
	if ((qdlg.m_ByHow == 0 || qdlg.m_ByHow == 1) && host != NULL)
	{                 
		m_OHost.SetWindowText(host->h_name);

		for (i = 0; host->h_aliases[i]; i++)
			m_AliasList.InsertString(-1, host->h_aliases[i]);
		for (i = 0; host->h_addr_list[i]; i++)
		{
			memcpy(&taddr, host->h_addr_list[i], host->h_length); 
			m_AddrList.InsertString(-1, inet_ntoa(taddr));
		}
	}

	if (qdlg.m_ByHow == 2 && mymx != NULL)
	{
		m_OHost.SetWindowText("(MX Query)");
		
		for (i = 0; i < mymx->numrecs; i++)
		{
        	wsprintf(tmpbuf, "%s - Pref: %u", mymx->hostname[i],
        				mymx->pref[i]);
        	m_AddrList.InsertString(-1, tmpbuf);
		}
	}	
	
	if (qdlg.m_ByHow == 3 && randinfo != NULL)
    {
    	
    	cp = randinfo;
    	while (*cp)
		{	
    		if (!isprint(*cp) || (*cp == '\n'))
    		{
//    			*cp = '\0';
    			*cp = 0x20;
//    			m_AddrList.InsertString(-1, randinfo);
//                randinfo = cp + 1;
            }
            ++cp;
        }
		m_AddrList.InsertString(-1, randinfo);
		m_OHost.SetWindowText("(Info Query)");

    }
	
	WSACleanup();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CRespDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
