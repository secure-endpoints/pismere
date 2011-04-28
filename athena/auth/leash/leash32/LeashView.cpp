//*****************************************************************************
// File:	LeashView.cpp
// By:		Arthur David Leather
// Created:	12/02/98
// Copyright	@1998 Massachusetts Institute of Technology - All rights reserved.
// Description:	CPP file for LeashView.h. Contains variables and functions
//		for the Leash FormView
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 12/02/98	ADL		Original
//*****************************************************************************

#include "stdafx.h"
#include <afxpriv.h>
#include "Leash.h"
#include "LeashDoc.h"
#include "LeashView.h"
#include "MainFrm.h"
#include "reminder.h"
#include "lglobals.h"
#include "LeashDebugWindow.h"
#include "LeashMessageBox.h"
#include "LeashAboutBox.h"
#include "Krb4Properties.h"
#include "Krb5Properties.h"	
#include "LeashProperties.h"
#include "KrbProperties.h"
#include "AfsProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeashView

IMPLEMENT_DYNCREATE(CLeashView, CFormView)

BEGIN_MESSAGE_MAP(CLeashView, CFormView)
	ON_MESSAGE(WM_GOODBYE, OnGoodbye)
	//{{AFX_MSG_MAP(CLeashView)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREEVIEW, OnItemexpandedTreeview)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_RENEW_TICKET, OnRenewTicket)
	ON_COMMAND(ID_DESTROY_TICKET, OnDestroyTicket)
	ON_COMMAND(ID_CHANGE_PASSWORD, OnChangePassword)
	ON_COMMAND(ID_UPDATE_DISPLAY, OnUpdateDisplay)
	ON_COMMAND(ID_SYN_TIME, OnSynTime)
	ON_COMMAND(ID_DEBUG_MODE, OnDebugMode)
	ON_COMMAND(ID_LARGE_ICONS, OnLargeIcons)
	ON_COMMAND(ID_UPPERCASE_REALM, OnUppercaseRealm)
	ON_COMMAND(ID_KILL_TIX_ONEXIT, OnKillTixOnExit)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_DESTROY_TICKET, OnUpdateDestroyTicket)
	ON_UPDATE_COMMAND_UI(ID_RENEW_TICKET, OnUpdateRenewTicket)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_AFS_CONTROL_PANEL, OnAfsControlPanel)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_MODE, OnUpdateDebugMode)
	ON_UPDATE_COMMAND_UI(ID_CFG_FILES, OnUpdateCfgFiles)
	ON_COMMAND(ID_KRB4_PROPERTIES, OnKrb4Properties)
	ON_COMMAND(ID_KRB5_PROPERTIES, OnKrb5Properties)
	ON_COMMAND(ID_LEASH_PROPERTIES, OnLeashProperties)
	ON_COMMAND(ID_LOW_TICKET_ALARM, OnLowTicketAlarm)
	ON_UPDATE_COMMAND_UI(ID_KRB4_PROPERTIES, OnUpdateKrb4Properties)
	ON_UPDATE_COMMAND_UI(ID_KRB5_PROPERTIES, OnUpdateKrb5Properties)
	ON_UPDATE_COMMAND_UI(ID_AFS_CONTROL_PANEL, OnUpdateAfsControlPanel)
	ON_COMMAND(ID_PROPERTIES, OnKrbProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_HELP_KERBEROS_, OnHelpKerberos)
	ON_COMMAND(ID_HELP_LEASH32, OnHelpLeash32)
	ON_COMMAND(ID_HELP_WHYUSELEASH32, OnHelpWhyuseleash32)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
        ON_WM_SYSCOLORCHANGE()

END_MESSAGE_MAP()


LONG CLeashView::m_ticketTimeLeft = 0;  // # of seconds left before tickets expire
INT  CLeashView::m_forwardableTicket = 0;
INT  CLeashView::m_proxiableTicket = 0;
INT  CLeashView::m_ticketStatusKrb4 = 0; // Defense Condition: are we low on tickets?
INT  CLeashView::m_ticketStatusKrb5 = 0; // Defense Condition: are we low on tickets?
INT  CLeashView::m_warningOfTicketTimeLeftKrb4 = 0; // Prevents warning box from coming up repeatively
INT  CLeashView::m_warningOfTicketTimeLeftKrb5 = 0; // Prevents warning box from coming up repeatively
INT  CLeashView::m_warningOfTicketTimeLeftLockKrb4 = 0;
INT  CLeashView::m_warningOfTicketTimeLeftLockKrb5 = 0;
INT  CLeashView::m_updateDisplayCount;
INT  CLeashView::m_alreadyPlayedDisplayCount;
BOOL CLeashView::m_lowTicketAlarmSound;
BOOL CLeashView::m_gotKrb4Tickets;
BOOL CLeashView::m_gotKrb5Tickets;
BOOL CLeashView::m_gotAfsTokens;

bool change_icon_size = true;


/////////////////////////////////////////////////////////////////////////////
// CLeashView construction/destruction

CLeashView::CLeashView():
CFormView(CLeashView::IDD)
{
    m_afsNoTokens = TRUE;
    m_listKrb4 = NULL;
    m_listKrb5 = NULL;
    m_listAFS = NULL;
    m_startup = TRUE;
    m_warningOfTicketTimeLeftKrb4 = 0;
    m_warningOfTicketTimeLeftKrb5 = 0;
    m_warningOfTicketTimeLeftLockKrb4 = 0;
    m_warningOfTicketTimeLeftLockKrb5 = 0;
    m_largeIcons = 0;
    m_destroyTicketsOnExit = 0;
    m_debugWindow = 0;
    m_upperCaseRealm = 0;
    m_lowTicketAlarm = 0;
    
    m_pDebugWindow = NULL;
    m_pDebugWindow = new CLeashDebugWindow(this);
    if (!m_pDebugWindow)
    {
        MessageBox("There is a problem with the Leash32 Debug Window!", 
                   "Error", MB_OK);
    }
    
    m_debugStartUp = TRUE;
    m_isMinimum = FALSE;
    m_lowTicketAlarmSound = FALSE;
    m_alreadyPlayed = FALSE;
    m_gotKrb4Tickets = FALSE;
    m_gotKrb5Tickets = FALSE;
    ResetTreeNodes();
    m_pTree = NULL;
    m_hMenu = NULL;
    m_pApp = NULL;
    m_pImageList = NULL;
    m_forwardableTicket = 0;
    m_proxiableTicket = 0;
}


CLeashView::~CLeashView()
{
    // destroys window if not already destroyed
    if (m_pDebugWindow)
        delete m_pDebugWindow; 
}

BOOL CLeashView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeashView diagnostics

#ifdef _DEBUG
VOID CLeashView::AssertValid() const
{
    CFormView::AssertValid();
}

VOID CLeashView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

/*
LeashDoc* CLeashView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(LeashDoc)));
    return (LeashDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeashView message handlers

BOOL CLeashView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                        DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
                        UINT nID, CCreateContext* pContext)
{
    return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect,
                             pParentWnd, nID, pContext);
}

INT CLeashView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFormView::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

LONG CLeashView::LeashTime()
{
    _tzset();
    return time(0);
}

INT CLeashView::GetLowTicketStatus()
{
    if (!ticketinfoKrb4.btickets && !ticketinfoKrb5.btickets)
        return NO_TICKETS;

    if (m_ticketTimeLeft <= 0L)
        return ZERO_MINUTES_LEFT;

    if (m_ticketTimeLeft <= 15 * 60)
        return (INT)(m_ticketTimeLeft / 5 / 60) + 2 -
            (m_ticketTimeLeft % (5 * 60) == 0 ? 1 : 0);

    return FIFTEEN_MINUTES_LEFT + 1;
}

VOID CLeashView::UpdateTicketTime(TICKETINFO& ticketinfo)
{
    if (!ticketinfo.btickets)
    {
        m_ticketTimeLeft = 0L;
        return;
    }

    m_ticketTimeLeft = ticketinfo.issue_date + ticketinfo.lifetime -
        LeashTime();

    if (m_ticketTimeLeft <= 0L)
        ticketinfo.btickets = EXPIRED_TICKETS;
}


VOID CALLBACK EXPORT CLeashView::TimerProc(HWND hWnd, UINT nMsg,
                                           UINT nIDEvent, DWORD dwTime)
{
    CString ticketStatusKrb4;
    CString ticketStatusKrb5;
    CString strTimeDate;

    if (CLeashApp::m_hKrb4DLL)
    {
        // KRB4
        UpdateTicketTime(ticketinfoKrb4);

        if (!ticketinfoKrb4.btickets)
        {
            m_gotKrb4Tickets = FALSE;

            if (!CLeashApp::m_hKrb4DLL && !CLeashApp::m_hKrb5DLL && !CLeashApp::m_hAfsDLL)
                ticketStatusKrb4 = "Kerb-4: Not Available";
            else if (!CLeashApp::m_hAfsDLL)
                ticketStatusKrb4 = "Kerb-4: No Tickets";
            else
                ticketStatusKrb4 = "Kerb-4/AFS: No Tickets";
        }
        else if (EXPIRED_TICKETS == ticketinfoKrb4.btickets)
        {
            m_gotKrb4Tickets = FALSE;

            if (!CLeashApp::m_hAfsDLL)
                ticketStatusKrb4 = "Kerb-4: Expired Tickets";
            else
                ticketStatusKrb4 = "Kerb-4/AFS: Expired Tickets";

            m_ticketTimeLeft = 0;
        }
        else
        {
    	    m_gotKrb4Tickets = TRUE;

            CString lowTicketWarning;
            m_ticketStatusKrb4 = GetLowTicketStatus();
            switch (m_ticketStatusKrb4)
            {
            case FIFTEEN_MINUTES_LEFT:
                ticketinfoKrb4.btickets = TICKETS_LOW;

                if (!m_gotAfsTokens || !CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
                    lowTicketWarning = "Less then 15 minutes left on your Kerberos Four ticket(s)";
                else
                    lowTicketWarning = "Less then 15 minutes left on your Kerberos Four ticket(s)/token(s)";

                break;
            case TEN_MINUTES_LEFT:
                ticketinfoKrb4.btickets = TICKETS_LOW;

                if (!m_gotAfsTokens || !CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
                    lowTicketWarning = "Less then 10 minutes left on your Kerberos Four ticket(s)";
                else
                    lowTicketWarning = "Less then 10 minutes left on your Kerberos Four ticket(s)/token(s)";

                if (!m_warningOfTicketTimeLeftLockKrb4)
                    m_warningOfTicketTimeLeftKrb4 = 0;

                m_warningOfTicketTimeLeftLockKrb4 = TEN_MINUTES_LEFT;
                break;
            case FIVE_MINUTES_LEFT:
                ticketinfoKrb4.btickets = TICKETS_LOW;
                if (m_warningOfTicketTimeLeftLockKrb4 == TEN_MINUTES_LEFT)
                    m_warningOfTicketTimeLeftKrb4 = 0;

                m_warningOfTicketTimeLeftLockKrb4 = FIVE_MINUTES_LEFT;

                if (!m_gotAfsTokens || !CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
                    lowTicketWarning = "Less then 5 minutes left on your Kerberos Four ticket(s)";
                else
                    lowTicketWarning = "Less then 5 minutes left on your Kerberos Four ticket(s)/token(s)";

                break;
            default:
                m_ticketStatusKrb4 = 0;
                break;
            }

            // Play warning message only once per each case statement above
            if (m_ticketStatusKrb4 && !m_warningOfTicketTimeLeftKrb4)
            {
                m_warningOfTicketTimeLeftKrb4 = ON;
                AlarmBeep();
                CLeashMessageBox leashMessageBox(NULL, lowTicketWarning, 100000);
                leashMessageBox.DoModal();
            }
        }

        if (CMainFrame::m_isMinimum)
        {
            // minimized dispay
            ticketStatusKrb4.Format("Kerb-4: %02d:%02d Left",
                                    (m_ticketTimeLeft / 60L / 60L),
                                    (m_ticketTimeLeft / 60L % 60L));
        }
        else
        {
            // normal display
            if (GOOD_TICKETS == ticketinfoKrb4.btickets ||
                TICKETS_LOW == ticketinfoKrb4.btickets)
            {
                if (m_gotAfsTokens)
                    ticketStatusKrb4.Format("Kerb-4 Ticket/Token Life: %02d:%02d",
                                            (m_ticketTimeLeft / 60L / 60L),
                                            (m_ticketTimeLeft / 60L % 60L));
                else
                    ticketStatusKrb4.Format("Kerb-4 Ticket Life: %02d:%02d",
                                            (m_ticketTimeLeft / 60L / 60L),
                                            (m_ticketTimeLeft / 60L % 60L));
            }

            if (CMainFrame::m_wndStatusBar)
            {
                CMainFrame::m_wndStatusBar.SetPaneInfo(1, 111111, SBPS_NORMAL, 180);
                CMainFrame::m_wndStatusBar.SetPaneText(1, ticketStatusKrb4, TRUE);
            }
        }
    }
    else
    {
        // not installed
        ticketStatusKrb4.Format("Kerb-4: Not Available");

        if (CMainFrame::m_wndStatusBar)
        {
            CMainFrame::m_wndStatusBar.SetPaneInfo(1, 111111, SBPS_NORMAL, 180);
            CMainFrame::m_wndStatusBar.SetPaneText(1, ticketStatusKrb4, TRUE);
        }
    }
    // KRB4

    if (CLeashApp::m_hKrb5DLL)
    {
        // KRB5
        UpdateTicketTime(ticketinfoKrb5);

        if (!ticketinfoKrb5.btickets)
        {
            m_gotKrb5Tickets = FALSE;
            ticketStatusKrb5 = "Kerb-5: No Tickets";
        }
        else if (EXPIRED_TICKETS == ticketinfoKrb5.btickets)
        {
            ticketStatusKrb5 = "Kerb-5: Expired Ticket(s)";
            m_gotKrb5Tickets = FALSE;
            m_ticketTimeLeft = 0;
        }
        else
        {
            m_gotKrb5Tickets = TRUE;

            CString lowTicketWarning;
            m_ticketStatusKrb5 = GetLowTicketStatus();
            switch (m_ticketStatusKrb5)
            {
            case FIFTEEN_MINUTES_LEFT:
                ticketinfoKrb5.btickets = TICKETS_LOW;
                lowTicketWarning = "Less then 15 minutes left on your Kerberos Five ticket(s)";
                break;
            case TEN_MINUTES_LEFT:
                ticketinfoKrb5.btickets = TICKETS_LOW;
                lowTicketWarning = "Less then 10 minutes left on your Kerberos Five ticket(s)";
                if (!m_warningOfTicketTimeLeftLockKrb5)
                    m_warningOfTicketTimeLeftKrb5 = 0;
                m_warningOfTicketTimeLeftLockKrb5 = TEN_MINUTES_LEFT;
                break;
            case FIVE_MINUTES_LEFT:
                ticketinfoKrb5.btickets = TICKETS_LOW;
                if (m_warningOfTicketTimeLeftLockKrb5 == TEN_MINUTES_LEFT)
                    m_warningOfTicketTimeLeftKrb5 = 0;
                m_warningOfTicketTimeLeftLockKrb5 = FIVE_MINUTES_LEFT;
                lowTicketWarning = "Less then 5 minutes left on your Kerberos Five ticket(s)";
                break;
            default:
                m_ticketStatusKrb5 = 0;
                break;
            }

            // Play warning message only once per each case statement above
            if (m_ticketStatusKrb5 && !m_warningOfTicketTimeLeftKrb5)
            {
                m_warningOfTicketTimeLeftKrb5 = ON;
                AlarmBeep();
                CLeashMessageBox leashMessageBox(NULL, lowTicketWarning, 100000);
                leashMessageBox.DoModal();
            }
        }

        if (CMainFrame::m_isMinimum)
        {
            // minimized dispay
            ticketStatusKrb5.Format("Kerb-5: %02d:%02d Left",
                                    (m_ticketTimeLeft / 60L / 60L),
                                    (m_ticketTimeLeft / 60L % 60L));
        }
        else
        {
            // normal display
            if (GOOD_TICKETS == ticketinfoKrb5.btickets || TICKETS_LOW == ticketinfoKrb5.btickets)
            {
                ticketStatusKrb5.Format("Kerb-5 Ticket Life: %02d:%02d",
                                        (m_ticketTimeLeft / 60L / 60L),
                                        (m_ticketTimeLeft / 60L % 60L));
            }

            if (CMainFrame::m_wndStatusBar)
            {
                CMainFrame::m_wndStatusBar.SetPaneInfo(2, 111111, SBPS_NORMAL, 180);
                CMainFrame::m_wndStatusBar.SetPaneText(2, ticketStatusKrb5, TRUE);
            }
        }
    }
    else
    {
        // not installed
        ticketStatusKrb5.Format("Kerb-5: Not Available");

        if (CMainFrame::m_wndStatusBar)
        {
            CMainFrame::m_wndStatusBar.SetPaneInfo(2, 111111, SBPS_NORMAL, 180);
            CMainFrame::m_wndStatusBar.SetPaneText(2, ticketStatusKrb5, TRUE);
        }
    }
    //KRB5

    CTime tTimeDate = CTime::GetCurrentTime();

    if (CMainFrame::m_isMinimum)
    {
        strTimeDate = ("Leash32 - [" + ticketStatusKrb4 + "] - " + "[" +
                       ticketStatusKrb5 + "] - " +
                       "[" + ticketinfoKrb5.principal + "]" + " - " +
                       tTimeDate.Format("%A, %B %d, %Y  %H:%M ")
            );
    }
    else
    {
        strTimeDate = ("Leash32 - " +
                       tTimeDate.Format("%A, %B %d, %Y  %H:%M ")
                       //timeDate.Format("%d %b %y %H:%M:%S - ")
            );
    }

    ::SetWindowText(CLeashApp::m_hProgram, strTimeDate);
    m_updateDisplayCount++;
    m_alreadyPlayedDisplayCount++;
}

VOID  CLeashView::ApplicationInfoMissingMsg()
{ 
    MessageBox("There is a problem finding Leash32 application information!", 
               "Error", MB_OK);
}

VOID CLeashView::OnShowWindow(BOOL bShow, UINT nStatus)
{
    // Get State of Icons Size
    m_pApp = AfxGetApp();
    if (!m_pApp)
    {
        ApplicationInfoMissingMsg();
    }
    else
    {
        m_largeIcons = m_pApp->GetProfileInt("Settings", "LargeIcons", TRUE_FLAG);

        // Forwardable flag
        m_forwardableTicket = m_pApp->GetProfileInt("Settings", "ForwardableTicket", 
                                                    TRUE_FLAG);

        // Proxiable flag
        m_proxiableTicket = m_pApp->GetProfileInt("Settings", "ProxiableTicket", 
                                                  TRUE_FLAG);

        // Get State of Destroy Tickets On Exit
        m_destroyTicketsOnExit = m_pApp->GetProfileInt("Settings", "DestroyTicketsOnExit",
                                                       FALSE_FLAG);

        // Get State of Low Ticket Alarm
        m_lowTicketAlarm = m_pApp->GetProfileInt("Settings", "LowTicketAlarm", ON);

        // Get State of Upper Case Realm
        m_upperCaseRealm = m_pApp->GetProfileInt("Settings", "UpperCaseRealm", ON);

        OnLargeIcons();
    }

    SetTimer(1, ONE_SECOND, TimerProc);

    if (!CLeashApp::m_hKrb4DLL && !CLeashApp::m_hKrb5DLL && !CLeashApp::m_hAfsDLL)
    {
        MessageBox("Neither Kerberos Four, Kerberos Five nor AFS is loaded!!!"
                   "\r\nYou will not be able to retrieve tickets and/or "
                   "tokens.",
                   "Warning", MB_OK);
    }

    if (!CLeashApp::m_hAfsDLL)
    {
        // No AFS installed
        SetDlgItemText(IDC_LABEL_KERB_TICKETS, 
                       "Your Kerberos Tickets (Issued/Expires/Principal)");
    }

    CLeashApp::m_krbv5_context = NULL;
}

VOID CLeashView::OnRenewTicket()
{
    LSH_DLGINFO ldi;

    ldi.principal = ticketinfoKrb4.principal;
    if (!*ldi.principal)
        ldi.principal = ticketinfoKrb5.principal;

    ldi.dlgtype = DLGTYPE_PASSWD;
    ldi.title = "Initialize Ticket";

	if (!m_hWnd)
    {
        MessageBox("There is a problem finding the Leash32 Window!", 
                   "Error", MB_OK);
        return;
    }

    int result = pLeash_kinit_dlg(m_hWnd, &ldi);
    if (-1 == result)
    {
        MessageBox("There is a problem getting tickets!", 
                   "Error", MB_OK);
    }
    else
    {
		ticketinfoKrb4.btickets = GOOD_TICKETS;
		m_warningOfTicketTimeLeftKrb4 = 0;
        m_warningOfTicketTimeLeftKrb5 = 0;
        OnUpdateDisplay();
    }
}

VOID CLeashView::OnDestroyTicket()
{
    if (m_gotKrb4Tickets || m_gotKrb5Tickets || !m_afsNoTokens) 
    {
        INT whatToDo;
		
        if (!CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
            whatToDo = AfxMessageBox("You are about to destroy your ticket(s)!", 
                                     MB_OKCANCEL, 0);
        else	
            whatToDo = AfxMessageBox("You are about to destroy your ticket(s)/token(s)!",
                                     MB_OKCANCEL, 0);
		
        if (whatToDo == IDOK)
        {
            pLeash_kdestroy();
            ResetTreeNodes();
            OnUpdateDisplay();
        }
    }
}

VOID CLeashView::OnChangePassword() 
{
    if (!m_hWnd)
    {
        MessageBox("There is a problem finding the Leash32 Window!", 
                   "Error", MB_OK);
        return;
    }
    
    LSH_DLGINFO ldi;

    ldi.principal = ticketinfoKrb4.principal;
    if (!*ldi.principal)
        ldi.principal = ticketinfoKrb5.principal;

    ldi.dlgtype = DLGTYPE_CHPASSWD;
    ldi.title = "Change password";

    int result = pLeash_changepwd_dlg(m_hWnd, &ldi);
    if (-1 == result)
    {
        MessageBox("There is a problem changing password!", 
                   "Error", MB_OK);
    }
}

VOID CLeashView::OnUpdateDisplay()
{
    m_pTree = (CTreeCtrl*) GetDlgItem(IDC_TREEVIEW);
    if (!m_pTree)
    {
        MessageBox("There is a problem finding the Ticket Tree!", 
                    "Error", MB_OK);
        return;
    }

    m_pImageList = &m_imageList;
    if (!m_pImageList)
    {
        MessageBox("There is a problem finding images for the Ticket Tree!", 
                   "Error", MB_OK);
        return;
    }

    m_pTree->SetImageList(&m_imageList, TVSIL_NORMAL);

    TV_INSERTSTRUCT m_tvinsert;

    INT ticketIconStatusKrb4;
    INT ticketIconStatus_SelectedKrb4;
    INT iconStatusKrb4;

    INT ticketIconStatusKrb5;
    INT ticketIconStatus_SelectedKrb5;
    INT iconStatusKrb5;

    LONG krb4Error;
    LONG krb5Error;
    //LONG afsError;

    // Get Kerb 4 tickets in list
    krb4Error = pLeashKRB4GetTickets(&ticketinfoKrb4, &m_listKrb4);

    // Get Kerb 5 tickets in list
    if (CLeashApp::m_krbv5_context)
    { // need 'm_krbv5_context' for the profile data it contains
        pkrb5_free_context(CLeashApp::m_krbv5_context);
        CLeashApp::m_krbv5_context = NULL;
    }

  
    krb5Error = pLeashKRB5GetTickets(&ticketinfoKrb5, &m_listKrb5,
                                     &CLeashApp::m_krbv5_context);
    if (!krb5Error || krb5Error == KRB5_FCC_NOFILE)
    {
        if (CLeashApp::m_hKrb5DLL && !CLeashApp::m_krbv5_profile)
        {
            CHAR confname[MAX_PATH];
            if (CLeashApp::GetProfileFile(confname, sizeof(confname)))
            {
                MessageBox("Can't locate Kerberos Five Config. file!", 
                           "Error", MB_OK);
            }

            const char *filenames[2];
            filenames[0] = confname;
            filenames[1] = NULL;
            pprofile_init(filenames, &CLeashApp::m_krbv5_profile);
        }
    }

    // Get AFS Tokens in list
    if (CLeashApp::m_hAfsDLL)
        pLeashAFSGetToken(&m_listAFS);
    
    if (!m_listAFS)
        m_afsNoTokens = TRUE;
    else
        m_afsNoTokens = FALSE;
    
    // Tree Structure common values
    m_pTree->DeleteAllItems();

    m_tvinsert.hParent = NULL;
    m_tvinsert.hInsertAfter = TVI_LAST;
    m_tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE
        | TVIF_TEXT;
    m_tvinsert.item.hItem = NULL;
    m_tvinsert.item.state = 0;
    m_tvinsert.item.stateMask = 0; //TVIS_EXPANDED;
    m_tvinsert.item.cchTextMax = 6;
    m_tvinsert.item.iSelectedImage = 1;
    m_tvinsert.item.cChildren = 0;
    m_tvinsert.item.lParam = 0;


    // Krb4
    m_tvinsert.hParent = NULL;
    m_tvinsert.item.iImage = 1;

	m_tvinsert.item.pszText = ticketinfoKrb4.principal;
	
    m_hPrincipal = m_pTree->InsertItem(&m_tvinsert);

    UpdateTicketTime(ticketinfoKrb4);
    if (!m_listKrb4 || EXPIRED_TICKETS == ticketinfoKrb4.btickets)
    {
        ticketIconStatusKrb4 = EXPIRED_CLOCK;
        ticketIconStatus_SelectedKrb4 = EXPIRED_CLOCK;
        iconStatusKrb4 = EXPIRED_TICKET;
    }
    else if (TICKETS_LOW == ticketinfoKrb4.btickets ||
             m_ticketStatusKrb4 == FIVE_MINUTES_LEFT ||
             m_ticketStatusKrb4 == TEN_MINUTES_LEFT ||
             m_ticketStatusKrb4 == FIFTEEN_MINUTES_LEFT)
    {
        ticketIconStatusKrb4 = LOW_CLOCK;
        ticketIconStatus_SelectedKrb4 = LOW_CLOCK;
        iconStatusKrb4 = LOW_TICKET;
    }
    else
    {
        ticketIconStatusKrb4 = ACTIVE_CLOCK;
        ticketIconStatus_SelectedKrb4 = ACTIVE_CLOCK;
        iconStatusKrb4 = ACTIVE_TICKET;
    }

    m_tvinsert.hParent = m_hPrincipal;

    if (CLeashApp::m_hKrb4DLL)
    {
        m_tvinsert.item.pszText = "Kerberos Four Tickets";
        m_tvinsert.item.iImage = iconStatusKrb4;
        m_tvinsert.item.iSelectedImage = iconStatusKrb4;
    }
    else
    {
        ticketinfoKrb4.btickets = NO_TICKETS;
        m_tvinsert.item.pszText = "Kerberos Four Tickets (Not Available)";
        m_tvinsert.item.iImage = TICKET_NOT_INSTALLED;
        m_tvinsert.item.iSelectedImage = TICKET_NOT_INSTALLED;
    }

    m_hKerb4 = m_pTree ->InsertItem(&m_tvinsert);

    if (m_hPrincipalState == NODE_IS_EXPANDED)
        m_pTree->Expand(m_hPrincipal, TVE_EXPAND);

    m_tvinsert.hParent = m_hKerb4;
    m_tvinsert.item.iImage = ticketIconStatusKrb4;
    m_tvinsert.item.iSelectedImage = ticketIconStatus_SelectedKrb4;

    TicketList* tempList = m_listKrb4, *killList;
    while (tempList)
    {
        m_tvinsert.item.pszText = tempList->theTicket;
        ::SendMessage(m_pTree->m_hWnd, TVM_INSERTITEM , 0,
                      (LONG) (LPTVINSERTSTRUCT) &m_tvinsert);

        tempList = tempList->next;
    }

    pLeashFreeTicketList(&m_listKrb4);

    if (m_hKerb4State == NODE_IS_EXPANDED)
        m_pTree->Expand(m_hKerb4, TVE_EXPAND);


    // Krb5
    UpdateTicketTime(ticketinfoKrb5);
    m_ticketStatusKrb5 = GetLowTicketStatus();

    if (!m_listKrb5 || EXPIRED_TICKETS == ticketinfoKrb5.btickets) //|| EXPIRED_CLOCK == ticketinfo.btickets)
    {
        ticketIconStatusKrb5 = EXPIRED_CLOCK;
        ticketIconStatus_SelectedKrb5 = EXPIRED_CLOCK;
        iconStatusKrb5 = EXPIRED_TICKET;
    }
    else if (TICKETS_LOW == ticketinfoKrb5.btickets ||
             m_ticketStatusKrb5 == FIVE_MINUTES_LEFT ||
             m_ticketStatusKrb5 == TEN_MINUTES_LEFT ||
             m_ticketStatusKrb5 == FIFTEEN_MINUTES_LEFT)
    {
        ticketIconStatusKrb5 = LOW_CLOCK;
        ticketIconStatus_SelectedKrb5 = LOW_CLOCK;
        iconStatusKrb5 = LOW_TICKET;
    }
    else
    {
        ticketIconStatusKrb5 = ACTIVE_CLOCK;
        ticketIconStatus_SelectedKrb5 = ACTIVE_CLOCK;
        iconStatusKrb5 = ACTIVE_TICKET;
    }

    m_tvinsert.hParent = m_hPrincipal;

    if (CLeashApp::m_hKrb5DLL)
    {
        // kerb5 installed
        m_tvinsert.item.pszText = "Kerberos Five Tickets";
        m_tvinsert.item.iImage = iconStatusKrb5;
        m_tvinsert.item.iSelectedImage = iconStatusKrb5;
    }
    else
    {
        // kerb5 not installed
        ticketinfoKrb5.btickets = NO_TICKETS;
        m_tvinsert.item.pszText = "Kerberos Five Tickets (Not Available)";
        m_tvinsert.item.iImage = TICKET_NOT_INSTALLED;
        m_tvinsert.item.iSelectedImage = TICKET_NOT_INSTALLED;
    }

    m_hKerb5 = m_pTree->InsertItem(&m_tvinsert);


    m_tvinsert.hParent = m_hKerb5;
    m_tvinsert.item.iImage = ticketIconStatusKrb5;
    m_tvinsert.item.iSelectedImage = ticketIconStatus_SelectedKrb5;

    tempList = m_listKrb5, *killList;
    while (tempList)
    {

        m_tvinsert.item.pszText = tempList->theTicket;
        ::SendMessage(m_pTree->m_hWnd, TVM_INSERTITEM , 0,
                      (LONG) (LPTVINSERTSTRUCT) &m_tvinsert);

        tempList = tempList->next;
    }

    pLeashFreeTicketList(&m_listKrb5);

    if (m_hKerb5State == NODE_IS_EXPANDED)
        m_pTree->Expand(m_hKerb5, TVE_EXPAND);


    // AFS
    m_tvinsert.hParent = m_hPrincipal;

    /*
    if (!krb4Error && CLeashApp::m_hAfsDLL && CLeashApp::m_hKrb4DLL &&
        m_tvinsert.item.pszText && m_afsNoTokens)
    { // AFS service not started or just no tickets
        m_gotAfsTokens = FALSE;
        m_tvinsert.item.pszText = "AFS Tokens (Not Available)";
        m_tvinsert.item.iImage = TICKET_NOT_INSTALLED;
        m_tvinsert.item.iSelectedImage = TICKET_NOT_INSTALLED;
    }
    */
    
    if (!krb4Error && CLeashApp::m_hAfsDLL && CLeashApp::m_hKrb4DLL &&
        m_tvinsert.item.pszText)
    { // AFS installed

        if (!m_afsNoTokens)
        {
            m_gotAfsTokens = TRUE;
            m_tvinsert.item.pszText = "AFS Tokens";
            m_tvinsert.item.iImage = iconStatusKrb4;
            m_tvinsert.item.iSelectedImage = iconStatusKrb4;
        }
    	else
        {
            m_gotAfsTokens = FALSE;
            m_tvinsert.item.pszText = "AFS Tokens (Not Available)";
            m_tvinsert.item.iImage = TICKET_NOT_INSTALLED;
            m_tvinsert.item.iSelectedImage = TICKET_NOT_INSTALLED;
        }

        m_hAFS = m_pTree->InsertItem(&m_tvinsert);

        m_tvinsert.hParent = m_hAFS;
        m_tvinsert.item.iImage = ticketIconStatusKrb4;
        m_tvinsert.item.iSelectedImage = ticketIconStatus_SelectedKrb4;

        tempList = m_listAFS, *killList;
        while (tempList)
        {
            m_tvinsert.item.pszText = tempList->theTicket;
            ::SendMessage(m_pTree->m_hWnd, TVM_INSERTITEM , 0,
                          (LONG) (LPTVINSERTSTRUCT) &m_tvinsert);

            tempList = tempList->next;
        }

        pLeashFreeTicketList(&m_listAFS);

        if (m_hAFSState == NODE_IS_EXPANDED)
            m_pTree->Expand(m_hAFS, TVE_EXPAND);
    }
    else if (!krb4Error && CLeashApp::m_hAfsDLL && CLeashApp::m_hKrb4DLL &&
             !m_tvinsert.item.pszText)
    {
        m_gotAfsTokens = FALSE;
        m_tvinsert.item.pszText = "AFS Tokens";
        m_tvinsert.item.iImage = EXPIRED_TICKET;;
        m_tvinsert.item.iSelectedImage = EXPIRED_TICKET;
    }

    if (m_startup)
    {
        //m_startup = FALSE;
        UpdateTicketTime(ticketinfoKrb4);
    }

    CString sPrincipal = ticketinfoKrb5.principal;
    if (sPrincipal.IsEmpty())
        sPrincipal = ticketinfoKrb4.principal;

	// if no tickets
	if (!ticketinfoKrb4.btickets && !ticketinfoKrb5.btickets)
		sPrincipal = " No Tickets ";
    

	// if no tickets and tokens
    if (!ticketinfoKrb4.btickets && !ticketinfoKrb5.btickets && m_afsNoTokens) //&& sPrincipal.IsEmpty())
    {
        // No tickets
        m_pTree->DeleteAllItems();

        m_tvinsert.hParent = NULL;
        m_tvinsert.item.iImage = 1;
        m_tvinsert.item.pszText = " No Tickets/Tokens ";
        m_tvinsert.item.iSelectedImage = 1;
        m_hPrincipal = m_pTree->InsertItem(&m_tvinsert);

        if (CMainFrame::m_wndToolBar)
        {
            CToolBarCtrl *_toolBar = NULL;
            CToolBarCtrl& toolBar = CMainFrame::m_wndToolBar.GetToolBarCtrl();
            _toolBar = &toolBar; 
            if (_toolBar)
            {
                //toolBar.EnableButton(ID_DESTROY_TICKET, FALSE);
                toolBar.SetState(ID_DESTROY_TICKET, TBSTATE_INDETERMINATE);
                //TBSTATE_HIDDEN
            }
            else
            {
                MessageBox("There is a problem with the Leash32 Toolbar!", "Error", 
                           MB_OK);
            }
        }
    }
    else
    {
        // We have some tickets
        m_pTree->SetItemText(m_hPrincipal, sPrincipal);
        //m_pTree->Expand(m_hPrincipal, TVE_EXPAND);

        if (CMainFrame::m_wndToolBar)
        {
            CToolBarCtrl *_toolBar = NULL;
            CToolBarCtrl& toolBar = CMainFrame::m_wndToolBar.GetToolBarCtrl();
            _toolBar = &toolBar; 
            if (_toolBar)
            {
                //toolBar.EnableButton(ID_DESTROY_TICKET, TRUE);
                toolBar.SetState(ID_DESTROY_TICKET, TBSTATE_ENABLED);
            }
            else
            {
                MessageBox("There is a problem with the Leash32 Toolbar!", "Error", MB_OK);
            }
        }
    }
}

VOID CLeashView::OnSynTime()
{
    LONG returnValue;
    returnValue = pLeash_timesync(1);
}

VOID CLeashView::OnActivateView(BOOL bActivate, CView* pActivateView,
                                CView* pDeactiveView)
{
    UINT check = NULL;

    if (m_alreadyPlayed)
    {
        CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
        return;
    }
	
    // The following code has put here because at the time
    // 'checking and unchecking' a menuitem with the
    // 'OnUpdate.....(CCmdUI* pCmdUI) functions' were unreliable
    // in CLeashView -->> Better done in CMainFrame
    if( CLeashApp::m_hProgram != 0 )
    {
        m_hMenu = ::GetMenu(CLeashApp::m_hProgram); 
    } else {
        return;
    }
    if( m_hMenu == NULL )
    {
        return;
    }
    if (!m_largeIcons)
        check = CheckMenuItem(m_hMenu, ID_LARGE_ICONS, MF_CHECKED); 
    else
        check = CheckMenuItem(m_hMenu, ID_LARGE_ICONS, MF_UNCHECKED); 
		
    if( check != MF_CHECKED || check != MF_UNCHECKED )
    {
        m_debugStartUp = 1;
    }

    if (!m_destroyTicketsOnExit)
        check = CheckMenuItem(m_hMenu, ID_KILL_TIX_ONEXIT, MF_UNCHECKED); 
    else
        check = CheckMenuItem(m_hMenu, ID_KILL_TIX_ONEXIT, MF_CHECKED); 
	
    if (!m_upperCaseRealm)
        check = CheckMenuItem(m_hMenu, ID_UPPERCASE_REALM, MF_UNCHECKED); 
    else
        check = CheckMenuItem(m_hMenu, ID_UPPERCASE_REALM, MF_CHECKED); 
	
    if (!m_lowTicketAlarm)
	m_hMenu = ::GetMenu(CLeashApp::m_hProgram); 
    if (!m_hMenu)
    {
        MessageBox("There is a problem finding the Leash32 main menu!", 
                   "Error", MB_OK);
    }
    
    if (!m_lowTicketAlarm)
    {
        m_lowTicketAlarmSound = FALSE;
		
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_LOW_TICKET_ALARM, MF_UNCHECKED); 
    }
    else
    {
        m_lowTicketAlarmSound = TRUE;
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_LOW_TICKET_ALARM, MF_CHECKED); 
    }

    m_debugWindow = m_pApp->GetProfileInt("Settings", "DebugWindow", 0);
    if (!m_debugWindow)
        check = CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_UNCHECKED); 	
    else
        check = CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_CHECKED); 

    m_alreadyPlayed = TRUE;
    if (m_pApp)
    {
        m_debugWindow = m_pApp->GetProfileInt("Settings", "DebugWindow", 0);
	    
        if (m_hMenu)
        {
            if (!m_debugWindow)
            {
                CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_UNCHECKED); 	
            }
            else
            {
                CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_CHECKED); 
            }
        }
    }
    else
    {
        ApplicationInfoMissingMsg();
    }
    
    m_alreadyPlayed = TRUE;

    if (!CKrbProperties::KrbPropertiesOn)
        OnUpdateDisplay(); 

    if (m_debugStartUp)
    {
        OnDebugMode(); 
    }

    m_debugStartUp = FALSE; 

    CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

VOID CLeashView::OnDebugMode() 
{
    if (!pset_krb_debug) 
        return;	
	
    if (!m_pDebugWindow)
    {
        MessageBox("There is a problem with the Leash32 Debug Window!", 
                   "Error", MB_OK);
        return;
    }
  
        
    // Check all possible 'KRB' system varables, then reset (delete) debug file
    CHAR*  Env[] = {"TEMP", "TMP", "HOME", NULL};
    CHAR** pEnv = Env;
    CHAR debugFilePath[MAX_PATH];
    *debugFilePath = 0; 
	
    while (*pEnv)
    {
        CHAR* ptestenv = getenv(*pEnv);
        if (ptestenv)
        {
            // reset debug file
            strcpy(debugFilePath, ptestenv);
            strcat(debugFilePath, "\\LshDebug.log"); 
            remove(debugFilePath);
            break;			
        }

        pEnv++;
    }
	
    if (!m_debugStartUp)
    {
        if (m_debugWindow%2 == 0)
            m_debugWindow = ON;
        else
            m_debugWindow = OFF;
    }

    if (!m_pApp)
    {
        ApplicationInfoMissingMsg();
    }
    else if (!m_debugWindow) 
    {
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_UNCHECKED); 
		
        m_pApp->WriteProfileInt("Settings", "DebugWindow", FALSE_FLAG);			
        m_pDebugWindow->DestroyWindow();
        pset_krb_debug(OFF);
        pset_krb_ap_req_debug(OFF);
        return;
    }
    else
    {
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_DEBUG_MODE, MF_CHECKED); 
	
        m_pApp->WriteProfileInt("Settings", "DebugWindow", TRUE_FLAG);			
    }
	
    // Creates the Debug dialog if not created already
    if (m_pDebugWindow->GetSafeHwnd() == 0) 
    { // displays the Debug Window
        m_pDebugWindow->Create(debugFilePath); 
    }
}

VOID CLeashView::OnLargeIcons()
{
    INT x, y;

    if (change_icon_size)
    {
        if (m_largeIcons%2 == 0)
            m_largeIcons = ON;
        else
            m_largeIcons = OFF;
    }
    else
    {
        if (m_largeIcons%2 == 0)
            m_largeIcons = OFF;
        else
            m_largeIcons = ON;
    }
	
    x = y = SMALL_ICONS;
    
    if (!m_pApp)
        ApplicationInfoMissingMsg();
    else   
    {
        if (!m_largeIcons)
	{
            if (m_hMenu)
                CheckMenuItem(m_hMenu, ID_LARGE_ICONS, MF_CHECKED);
		    
            x = y = LARGE_ICONS;
		    
	    if (!m_startup)
	    {
                m_pApp->WriteProfileInt("Settings", "LargeIcons", TRUE_FLAG);
	    }
	}
	else
	{
	    if (m_hMenu)
                CheckMenuItem(m_hMenu, ID_LARGE_ICONS, MF_UNCHECKED); 

            x = y = SMALL_ICONS;
    	
            if (!m_startup)
            {
                m_pApp->WriteProfileInt("Settings", "LargeIcons", FALSE_FLAG);
	    }
	}	
    }	    
	
    HICON hIcon[9];
    hIcon[0] = NULL;
    hIcon[1] = NULL;
    hIcon[2] = NULL;
    hIcon[3] = NULL;
    hIcon[4] = NULL;
    hIcon[5] = NULL;
    hIcon[6] = NULL;
    hIcon[7] = NULL;
    hIcon[8] = NULL;

    m_imageList.DeleteImageList( );
    m_imageList.Create(x, y, 0, 8, 8);
    m_imageList.SetBkColor(GetSysColor(COLOR_WINDOW));

    hIcon[0] = AfxGetApp()->LoadIcon(IDI_TICKETEXPIRED);
    hIcon[1] = AfxGetApp()->LoadIcon(IDI_LEASHSELECT); 
    hIcon[2] = AfxGetApp()->LoadIcon(IDI_LEASH); 
    hIcon[3] = AfxGetApp()->LoadIcon(IDI_TICKETTYPE);
    hIcon[4] = AfxGetApp()->LoadIcon(IDI_TICKETACTIVE);
    hIcon[5] = AfxGetApp()->LoadIcon(IDI_TICKETLOW);
    hIcon[6] = AfxGetApp()->LoadIcon(IDI_TICKET_NOTINSTALLED); 
    hIcon[7] = AfxGetApp()->LoadIcon(IDI_TICKETTYPE_EXPIRED);
    hIcon[8] = AfxGetApp()->LoadIcon(IDI_TICKETTYPE_LOW);
	
	
    if (!hIcon[0] || !hIcon[1] || !hIcon[2] || !hIcon[3] || !hIcon[4] || 
        !hIcon[5] || !hIcon[6] || !hIcon[7] || !hIcon[8])
    {
        MessageBox("Can't find one or more images in the Leash32 Ticket Tree!",
                   "Error", MB_OK);
        return;
    }
    
    for (INT n = 0; n < 9; n++)
    {
	m_imageList.Add(hIcon[n]);
    }
	
    if (!m_startup)
        OnUpdateDisplay();
}

VOID CLeashView::OnKillTixOnExit()
{
    if (m_destroyTicketsOnExit%2 == 0)
        m_destroyTicketsOnExit = ON;
    else
        m_destroyTicketsOnExit = OFF;
	
    if (m_pApp)
    {
        if (!m_destroyTicketsOnExit)
        {
            if (m_hMenu)
                CheckMenuItem(m_hMenu, ID_KILL_TIX_ONEXIT, MF_UNCHECKED); 
		    
            MessageBox("Tickets will be not be destroyed upon exiting Leash32!!!", 
                       "Warning", MB_OK);
            m_pApp->WriteProfileInt("Settings", "DestroyTicketsOnExit", 
                                    FALSE_FLAG);			
        }
        else
        {
            if (m_hMenu)
                CheckMenuItem(m_hMenu, ID_KILL_TIX_ONEXIT, MF_CHECKED); 
		    
            MessageBox("All tickets/tokens will be destroyed upon exiting Leash32!!!", 
                       "Warning", MB_OK);
            m_pApp->WriteProfileInt("Settings", "DestroyTicketsOnExit", TRUE_FLAG);
        }
    }
    else
    {
        ApplicationInfoMissingMsg();
    }
}

VOID CLeashView::OnUppercaseRealm()
{
    if (m_upperCaseRealm%2 == 0)
        m_upperCaseRealm = ON;
    else
        m_upperCaseRealm = OFF;
	
    if (!m_pApp)
    {
        ApplicationInfoMissingMsg();
    }
    else if (!m_upperCaseRealm)
    {  
        m_pApp->WriteProfileInt("Settings", "UpperCaseRealm", FALSE_FLAG);			
		
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_UPPERCASE_REALM, MF_UNCHECKED); 
    }
    else
    {
        m_pApp->WriteProfileInt("Settings", "UpperCaseRealm", TRUE_FLAG);			
		
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_UPPERCASE_REALM, MF_CHECKED); 
    }
}

VOID CLeashView::ResetTreeNodes()
{
    m_hPrincipalState = 0;
    m_hKerb4State = 0;
    m_hKerb5State = 0;
    m_hAFSState = 0;
}

VOID CLeashView::OnDestroy()
{
    CFormView::OnDestroy();
    if (m_destroyTicketsOnExit && (ticketinfoKrb4.btickets || ticketinfoKrb5.btickets))
    {
        if (pLeash_kdestroy())
        {
            MessageBox("There is a problem destroying tickets!", 
                       "Error", MB_OK);
        }
    }
}

VOID CLeashView::OnUpdateDestroyTicket(CCmdUI* pCmdUI)
{
    if (!CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
        pCmdUI->SetText("&Destroy Ticket(s)\tCtrl+D");
    else
        pCmdUI->SetText("&Destroy Ticket(s)/Token(s)\tCtrl+D");

    if (!m_gotKrb4Tickets && !m_gotKrb5Tickets && m_afsNoTokens)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

VOID CLeashView::OnUpdateRenewTicket(CCmdUI* pCmdUI)
{
    if (!ticketinfoKrb4.btickets && !ticketinfoKrb5.btickets)
    {
        if (!CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
            pCmdUI->SetText("&Get Ticket(s)\tCtrl+T");
        else
            pCmdUI->SetText("&Get Ticket(s)/Token(s)\tCtrl+T");
    }
    else
    {
        if (!CLeashApp::m_hAfsDLL || !CLeashApp::m_hKrb4DLL)
            pCmdUI->SetText("&Renew Ticket(s)\tCtrl+T");
        else
            pCmdUI->SetText("&Renew Ticket(s)/Token(s)\tCtrl+T");
    }

    if (!CLeashApp::m_hKrb4DLL && !CLeashApp::m_hKrb5DLL &&
        !CLeashApp::m_hAfsDLL)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

LRESULT CLeashView::OnGoodbye(WPARAM wParam, LPARAM lParam)
{
    m_pDebugWindow->DestroyWindow();
    return 0L;
}

VOID CLeashView::OnAppAbout()
{
    CLeashAboutBox leashAboutBox;
    leashAboutBox.DoModal();
}


VOID CLeashView::OnAfsControlPanel()
{
    CAfsProperties afsProperties;
    afsProperties.DoModal();
}

VOID CLeashView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    CLeashApp::m_hProgram = ::FindWindow(_T("LEASH.0WNDCLASS"), NULL);
}

VOID CLeashView::OnItemexpandedTreeview(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    if (m_hPrincipal == pNMTreeView->itemNew.hItem)
        m_hPrincipalState = pNMTreeView->action;
    else if (m_hKerb4 == pNMTreeView->itemNew.hItem)
        m_hKerb4State = pNMTreeView->action;
    else if (m_hKerb5 == pNMTreeView->itemNew.hItem)
        m_hKerb5State = pNMTreeView->action;
    else if (m_hAFS ==  pNMTreeView->itemNew.hItem)
        m_hAFSState =  pNMTreeView->action;

    CMainFrame::m_isBeingResized = TRUE;
    *pResult = 0;
}

VOID CLeashView::OnUpdateDebugMode(CCmdUI* pCmdUI)
{
    if (!pset_krb_debug)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

VOID CLeashView::OnUpdateCfgFiles(CCmdUI* pCmdUI)
{
    if (!pkrb_get_krbconf2)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

VOID CLeashView::OnLeashProperties()
{
    CLeashProperties leashProperties;
    leashProperties.DoModal();
}

VOID CLeashView::OnKrbProperties()
{
    CKrbProperties krbProperties("Kerberos Properties");
    krbProperties.DoModal();
}

VOID CLeashView::OnKrb4Properties()
{
    CKrb4Properties krb4Properties("Kerberos Four Properties");
    krb4Properties.DoModal();
}

VOID CLeashView::OnKrb5Properties()
{
    CKrb5Properties krb5Properties("Kerberos Five Properties");
    krb5Properties.DoModal();
}

/*
void CLeashView::GetRowWidthHeight(CDC* pDC, LPCSTR theString, int& nRowWidth,
                                   int& nRowHeight, int& nCharWidth)
{
    TEXTMETRIC tm;

    //CEx29aDoc* pDoc = GetDocument();
	pDC->GetTextMetrics(&tm);
    nCharWidth = tm.tmAveCharWidth + 1;
    nRowWidth = strlen(theString);

    //int nFields = theString.GetLength();

    //for(int i = 0; i < nFields; i++)
    //{
	//    nRowWidth += nCharWidth;
	//}

    nRowWidth *= nCharWidth;
    nRowHeight = tm.tmHeight;
}
*/

BOOL CLeashView::PreTranslateMessage(MSG* pMsg)
{
    if (UPDATE_DISPLAY_TIME == m_updateDisplayCount)
    {
        m_updateDisplayCount = 0;
        OnUpdateDisplay();
    }

    if (2 < m_alreadyPlayedDisplayCount)
    {
        m_alreadyPlayedDisplayCount = 0;
        m_alreadyPlayed = FALSE;
    }

    if (CMainFrame::m_isBeingResized)
    {
        WINDOWPLACEMENT headingWndpl;
        headingWndpl.length = sizeof(WINDOWPLACEMENT);

        CWnd *heading = GetDlgItem(IDC_LABEL_KERB_TICKETS);
        if (!heading->GetWindowPlacement(&headingWndpl))
        {
            MessageBox("There is a problem getting Leash32 Heading size!", 
                       "Error", MB_OK);
            return CFormView::PreTranslateMessage(pMsg);;
         }

        m_pTree = (CTreeCtrl*) GetDlgItem(IDC_TREEVIEW);
        VERIFY(m_pTree);
        if (!m_pTree)
        {
            MessageBox("There is a problem finding the Ticket Tree!", 
                       "Errror", MB_OK);
            return CFormView::PreTranslateMessage(pMsg);
        }

        CRect rect;
        GetClientRect(&rect);

        WINDOWPLACEMENT wndpl;
        wndpl.length = sizeof(WINDOWPLACEMENT);
        
        if (!GetWindowPlacement(&wndpl))
        {
            MessageBox("There is a problem getting Leash32 Window size!", 
                       "Error", MB_OK);
            return CFormView::PreTranslateMessage(pMsg);
        }
	

        wndpl.rcNormalPosition.top = rect.top + headingWndpl.rcNormalPosition.bottom;
        wndpl.rcNormalPosition.right = rect.right;
        wndpl.rcNormalPosition.bottom = rect.bottom;

        m_startup = FALSE;

        if (!m_pTree->SetWindowPlacement(&wndpl))
        {
            MessageBox("There is a problem setting Leash32 ticket Tree size!", 
                       "Error", MB_OK);
        }
	
               
        UpdateWindow();

        /* The follow code creates a cool scroll bar on the MainFrame
           m_pTree = (CTreeCtrl*) GetDlgItem(IDC_TREEVIEW);
           CWnd *pLabel = GetDlgItem(IDC_LABEL_KERB_TICKETS);

           VERIFY(m_pTree);

           // Sync Tree Frame with Main Frame
           WINDOWPLACEMENT wndpl;
           WINDOWPLACEMENT wndplTree;
           WINDOWPLACEMENT wndplLabel;
           wndpl.length = sizeof(WINDOWPLACEMENT);
           wndplTree.length = sizeof(WINDOWPLACEMENT);
           wndplLabel.length = sizeof(WINDOWPLACEMENT);
           GetWindowPlacement(&wndpl);
           m_pTree->GetWindowPlacement(&wndplTree);
           pLabel->GetWindowPlacement(&wndplLabel);

           if (!m_startup)
           {
           if (ticketinfoKrb4.btickets || ticketinfoKrb5.btickets)
           { // control scroll bars to TreeView
           #define TICKET_LABEL_TOP 8
           #define TICKET_LABEL_BOTTOM 28
           #define TICKET_LABEL_RIGHT 398
           #define RIGHT_FRAME_ADJUSTMENT 13
           #define BOTTOM_FRAME_ADJUSTMENT 72
           #define STRETCH_FACTOR 3

           char theText[MAX_K_NAME_SZ+40];
           int longestLine = 0;
           int theHeight = 0;
           BOOL disableScrollHorz = FALSE;
           BOOL disableScrollVert = FALSE;
           RECT rect;

           HTREEITEM  xTree;
           TV_ITEM item;
           item.mask = TVIF_HANDLE | TVIF_TEXT;
           item.cchTextMax = sizeof(theText);

           xTree = m_hKerb4;
           do
           {
           item.hItem = xTree;
           item.pszText = theText;
           VERIFY(m_pTree->GetItem(&item));

           UINT offSet = m_pTree->GetIndent();
           if (!m_pTree->GetItemRect(xTree, &rect, TRUE))
           {
           longestLine = 0;
           theHeight = 0;
           break;
           }

           if (rect.right > longestLine)
           longestLine = rect.right + RIGHT_FRAME_ADJUSTMENT;

           theHeight = rect.bottom + BOTTOM_FRAME_ADJUSTMENT;
           }
           while ((xTree = m_pTree->GetNextItem(xTree, TVGN_NEXTVISIBLE)));


           // Horz
           if (longestLine < wndpl.rcNormalPosition.right)
           { // disable scroll
           disableScrollHorz = TRUE;
           SetScrollPos(SB_HORZ, 0, TRUE);
           EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
           }
           else
           { // enable scroll
           EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
           SetScrollRange(SB_HORZ, 0, longestLine , TRUE);
           }

           // Vert
           if (theHeight < wndpl.rcNormalPosition.bottom)
           { // disable scroll
           disableScrollVert = TRUE;
           SetScrollPos(SB_VERT, 0, TRUE);
           EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
           }
           else
           { // enable scroll
           EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
           SetScrollRange(SB_VERT, 0, theHeight, TRUE);
           }


           if (!disableScrollHorz)
           {
           wndpl.rcNormalPosition.left =
           wndplTree.rcNormalPosition.left;

           wndplLabel.rcNormalPosition.left =
           wndpl.rcNormalPosition.left + 8;
           }

           if (!disableScrollVert)
           {
           wndpl.rcNormalPosition.top =
           wndplTree.rcNormalPosition.top;
           }
           else
           {
           wndplLabel.rcNormalPosition.left =
           wndpl.rcNormalPosition.left + 8;

           wndplLabel.rcNormalPosition.top = TICKET_LABEL_TOP;
           wndplLabel.rcNormalPosition.bottom = TICKET_LABEL_BOTTOM;
           wndplLabel.rcNormalPosition.right = TICKET_LABEL_RIGHT;
           }

           wndpl.rcNormalPosition.right *= STRETCH_FACTOR;
           wndpl.rcNormalPosition.bottom *= STRETCH_FACTOR;
           }
           }

           m_startup = FALSE;

           m_pTree->SetWindowPlacement(&wndpl);
           pLabel->SetWindowPlacement(&wndplLabel);
        */

        CMainFrame::m_isBeingResized = FALSE;
    }

    return CFormView::PreTranslateMessage(pMsg);
}

VOID CLeashView::OnLowTicketAlarm()
{
    if (m_lowTicketAlarm%2 == 0)
        m_lowTicketAlarm = ON;
    else
        m_lowTicketAlarm = OFF;
	
	
    if (!m_pApp)
    {
        ApplicationInfoMissingMsg();
    }
    else if (!m_lowTicketAlarm)
    {
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_LOW_TICKET_ALARM, MF_UNCHECKED); 
		
        m_pApp->WriteProfileInt("Settings", "LowTicketAlarm", FALSE_FLAG);			
        m_lowTicketAlarmSound = FALSE;
    }
    else
    {
        if (m_hMenu)
            CheckMenuItem(m_hMenu, ID_LOW_TICKET_ALARM, MF_CHECKED); 
		
        m_pApp->WriteProfileInt("Settings", "LowTicketAlarm", TRUE_FLAG);						
        m_lowTicketAlarmSound = TRUE;
    }	
}

VOID CLeashView::AlarmBeep()
{
	if (m_lowTicketAlarmSound) 
	{
		::Beep(2000, 200); 
		::Beep(200, 200); 
		::Beep(700, 200);
	}
}

VOID CLeashView::OnUpdateProperties(CCmdUI* pCmdUI)
{
    if (CLeashApp::m_hKrb5DLL || CLeashApp::m_hKrb4DLL)
        pCmdUI->Enable();
    else
        pCmdUI->Enable(FALSE);
}

VOID CLeashView::OnUpdateKrb4Properties(CCmdUI* pCmdUI)
{
    if (CLeashApp::m_hKrb4DLL)
        pCmdUI->Enable();
    else
        pCmdUI->Enable(FALSE);
}

VOID CLeashView::OnUpdateKrb5Properties(CCmdUI* pCmdUI)
{
    if (CLeashApp::m_hKrb5DLL)
        pCmdUI->Enable();
    else
        pCmdUI->Enable(FALSE);
}

VOID CLeashView::OnUpdateAfsControlPanel(CCmdUI* pCmdUI)
{
    // need Krb 4 to get AFS tokens
    if (CLeashApp::m_hAfsDLL && CLeashApp::m_hKrb4DLL)
        pCmdUI->Enable();
    else
        pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
}

void CLeashView::OnHelpLeash32()
{
    AfxGetApp()->WinHelp(HID_LEASH_PROGRAM);
}

void CLeashView::OnHelpKerberos()
{
    AfxGetApp()->WinHelp(HID_ABOUT_KERBEROS);
}

void CLeashView::OnHelpWhyuseleash32()
{
    AfxGetApp()->WinHelp(HID_WHY_USE_LEASH32);
}

void CLeashView::OnSysColorChange()
{
    change_icon_size = FALSE;
    CWnd::OnSysColorChange();
    OnLargeIcons();
    /* m_imageList.SetBkColor(GetSysColor(COLOR_WINDOW)); */
    change_icon_size = TRUE;
}
