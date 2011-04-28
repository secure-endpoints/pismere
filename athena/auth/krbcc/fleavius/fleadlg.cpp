// fleadlg.cpp : implementation file
//

#include "stdafx.h"
#include "fleavius.h"
#include "fleadlg.h"
#include "..\krbcc32\creds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFleaDlg dialog

CFleaDlg::CFleaDlg(bool debug, CWnd* pParent /*=NULL*/)
	: CDialog(CFleaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFleaDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDebug = debug;
}

CFleaDlg::CFleaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFleaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFleaDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDebug = true;
}

void CFleaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFleaDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFleaDlg, CDialog)
	//{{AFX_MSG_MAP(CFleaDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnLetGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFleaDlg message handlers

BOOL CFleaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	cc_int32 api_vers = CC_API_VER_1;
	m_api_CB = NULL;
	cc_initialize(&m_api_CB, api_vers, NULL, NULL);
	ASSERT(NULL != m_api_CB);
#ifdef MYAPPHASFLEAS
	if (reinterpret_cast<CCache_ctx*>(m_api_CB)->SetFleaviusProcessID(GetCurrentProcessId()) != CC_NOERROR)
		exit(0);
	HANDLE StartupHandle = OpenEvent(EVENT_ALL_ACCESS, TRUE, "FleaviusStartUp");
	if (StartupHandle != NULL)
		{
		SetEvent(StartupHandle);
		CloseHandle(StartupHandle);
		}
#endif

	// make this app come to life in an iconic state
	ShowWindow(SW_MINIMIZE);
	if (!m_bDebug) PostMessage(WM_PAINT);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFleaDlg::OnPaint() 
{
	if (!m_bDebug) ShowWindow(SW_HIDE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFleaDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFleaDlg::OnLetGo() 
{
#ifdef MYAPPHASFLEAS
	reinterpret_cast<CCache_ctx*>(m_api_CB)->ClearFleaviusProcessID();
#endif
	cc_shutdown(&m_api_CB);
	CDialog::OnOK();
}

void CFleaDlg::OnCancel() 
{
	// stay alive, but iconify
	ShowWindow(SW_MINIMIZE);
}


