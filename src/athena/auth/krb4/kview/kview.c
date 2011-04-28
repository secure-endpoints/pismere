/*
Details on building kview.exe (16-bits) or kview32.exe
-------------------------------
Project type: Windows application (exe)
Segment setup: SS==DS * (in 16-bit version)
Linker input libraries:  winsock

Files in project:
	kview.c
	kview.def or kview32.def
	kview.rc
	kclient.lib or kclien32.lib
	
Tab stops are set to 3 spaces for EZ reading.

Copyright © 1996 Project Mandarin, Inc.
*/

#include <windows.h>
#include <winsock.h>
#include <dde.h>

#include "kclient.h"
#include "reskview.h"

#include <string.h>
#include <stdlib.h>

#include <krb.h>

#if defined(_WIN32)
#ifndef _fstrcpy
#include <pcrt32.h>
#endif
#define KCLINAME "kclnt32"
#else
#define KCLINAME "kclient"
#endif

/* ------------------------------ Symbolic constants -------------------- */
#define MAXLIFETIME 24
#define MINLIFETIME 1

/* ------------------------------ Function Prototypes -------------------- */

int	WINAPI WinMain (HANDLE, HANDLE, LPSTR, int) ;
BOOL	InitApplication (HANDLE) ;
BOOL	InitInstance (HANDLE, int) ;

LRESULT CALLBACK LshKinitWndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK Settings(HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK About(HWND, UINT, WPARAM, LPARAM) ;
void GetSettings (RECT *) ;	
void WriteSettings (void) ;	
void autosync(int bWhetherAsk);
void UpdateUserid(void) ;
void ShowUserid(LPSTR theString);

extern void timesync(void) ;

#ifndef itoa
#define itoa _itoa
#endif

/* ----------------------------- Global Variables ------------------------ */
HANDLE hInst ;			// all purpose instance
HWND	gWnd ;			// main window
HWND	hListBox ;		// listbox to display tickets
HWND	hPreviousWnd ;		// window that was most recently active before our launch
HICON   hRegularIcon ;		// normal icon
HICON   hKeyIcon ;		// icon when tickets exist
HICON   hTheIcon ;		// which of those two to use at present

// KLite test info
char szKLiteTicket[256] ;
char szKLiteTest[256] ;
DWORD dwLen ;
LPSTR lpszService = "KLiteFakeService" ;
char titleUserid[256] ;			// the userid currently being shown

#if defined(_WIN32)
LPSTR lpszApp = "KView32" ;		// application name
#else
LPSTR lpszApp = "KView" ;		// application name
#endif

LPSTR lpszSettings = "Settings" ;	// heading used in .ini file
LPSTR lpszClass = "MandKViewClass" ;	// Windows class for this app
BOOL bWeveBeenWinExecEd = FALSE ;	// did we get started from kclient.dl?
LPSTR lpszTitle = "Hi There!" ;		// Title for Leash dialog

int  iLifeTime ;	// Ticket life time setting
UINT iTimeOut ;		// Auto-unload counter
UINT iAutoUnload ;	// number of 5 second units to wait before auto-unload
BOOL bMultiUse ;	// are multiple apps using kclient.dll?
BOOL bAutoLoad ;	// auto-load kview when kclient.dll is in use?
BOOL bDoClear ;		// command line requested clearing tickets?  "/clear"
BOOL bDoSync ;		// command line requested syncing time with Kerberos? "/sync"
BOOL bDoHide ;          // command line requested remaining invisible? "/hide"
BOOL bDoQuit ;          // command line requested immediate exit?  "/quit"
BOOL bDoAsk ;		// command line: is this the kind of sync where we might need to ask first?
BOOL bAskBeforeSync ;	// this is from INI settings.  do we ask before an autosync?
// (we never ask before a menu-selected sync)
BOOL bRefuseActiveOnce ; // refuse activation the first time only (under certain circumstances)
BOOL bFirstDdeInitiate ; // still waiting for DdeInitiate?

UINT msgAutoSync ;	// registered message number for autosyncing to Kerberos time
UINT    hTimer ;	// handle of timer to check for existence of net ID

char 	szTimeServer[128] ;	// hostname of time server
char 	szNetID[64] ;		// Net ID of user
WSADATA wskData;		// for cheap TGT test.


/* ----------------------------- The usual ----------------------------- */

BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wc ;
	
    wc.style = 0;
    wc.lpfnWndProc = LshKinitWndProc ;
    wc.cbClsExtra = 0 ;
    wc.cbWndExtra = 0 ;
    wc.hInstance = hInstance ;
    wc.hIcon = hRegularIcon ;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW) ;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH) ;
    wc.lpszMenuName =  MAKEINTRESOURCE(IDR_KVMENU) ;
    wc.lpszClassName = lpszClass ;

    return(RegisterClass(&wc)) ;
}

BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
    RECT r ;			// for window positioning 
//  UINT tFlags;		// flags for CheckMenuItem
    int whetherToShow;		// the flag for ShowWindow
    HMENU hMenu;		// for setting the check on Ask Before Autosync
    HMENU hSysMenu;		// for adding the "Network Logout" menu item
#define STGTSIZE 20
    char tgtQTime[STGTSIZE] ;	// to load the timer value string
    UINT tgtMs ;		// millisecond interval for TGTExist queries
#define SNETLOGSIZE 100
    char sNetLog[SNETLOGSIZE] ;	// to load name of "Network Logout" menu item
	
    hInst = hInstance ;
    hTimer = 0 ;
    titleUserid[0] = '\0' ;

    GetSettings (&r) ;		// get initial settings
    gWnd = CreateWindow(lpszClass, lpszApp, WS_OVERLAPPEDWINDOW, 
                        r.left, r.top, r.right-r.left, r.bottom-r.top,
                        NULL, NULL, hInstance, NULL) ;

    if (!gWnd)
        return (FALSE) ;

    UpdateUserid();

    if (0 < LoadString(hInst,IDS_TGTQUERY_TIMER,tgtQTime,STGTSIZE))
    {
        if (0 < (tgtMs = (UINT)atoi(tgtQTime)))
        {
            hTimer = SetTimer(gWnd,TGT_TIMER,tgtMs,NULL);
        }
    }

    // did kclient.dll start us?
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINNOACTIVE)
    {
        bWeveBeenWinExecEd = TRUE ;		// yes
	// use reference count timer for auto-exit
        SetTimer (gWnd, RC_TIMER, 1000, NULL) ;
        if (nCmdShow == SW_SHOWMINNOACTIVE)
        {
            bRefuseActiveOnce = TRUE ;
#if defined(_DEBUG)
            OutputDebugString("benice");
#endif			
        }
    }
		
    msgAutoSync = RegisterWindowMessage("KViewAutoSync");
	
    hMenu = GetMenu (gWnd) ;
/*	if (bAskBeforeSync)
        tFlags = MF_BYCOMMAND | MF_CHECKED;
	else
        tFlags = MF_BYCOMMAND | MF_UNCHECKED;
	CheckMenuItem (hMenu, IDM_ASKAUTOSYNC, tFlags) ;
*/
    hSysMenu = GetSystemMenu (gWnd, FALSE) ;
    if (0 == LoadString(hInst,IDM_DEST_TKTS,sNetLog,SNETLOGSIZE))
    	_fstrcpy(sNetLog,"Network &Logout");
     
    AppendMenu (hSysMenu, MF_SEPARATOR, 0,          NULL) ;
    AppendMenu (hSysMenu, MF_STRING,    IDM_DEST_TKTS,  sNetLog) ;

    whetherToShow = nCmdShow;
    if (bDoHide)
        whetherToShow = SW_HIDE;
    ShowWindow(gWnd, whetherToShow) ;
    if (!bDoHide)
        UpdateWindow(gWnd) ;

    return (TRUE);
}



/* -------------------------------------------------------------------
	GetParams -- see which parameters have been specified on command
	line.  set them in booleans.  if any problem, display message and
	return FALSE.  otherwise return TRUE.
	if the command line contains "/clear" or "-clear" or "\clear",
	then remove that token and return TRUE; otherwise return FALSE
	------------------------------------------------------------------- */
int GetParams(LPSTR lpCmdLine)
{
#define HOWMANYPARMS 5
    static char SomeParms[HOWMANYPARMS][6] = {"clear","sync","hide","quit","ask"};
    static int SomeParmsLen[HOWMANYPARMS] = {5,4,4,4,3};
#define ISCLEAR 0
#define ISSYNC 1
#define ISHIDE 2
#define ISQUIT 3
#define ISASK 4
    int thePos,iterate,tempLen,temperLen,bFound;
#define GETPARAMSMSGBUFLEN 384
    char theMsg[GETPARAMSMSGBUFLEN];
	
    char far * theParm;
	
    theParm = lpCmdLine;
	
    while (TRUE)
    {
        thePos = _fstrcspn(theParm,"-\\/");
        theParm = theParm+thePos;
        if (theParm[0] == '\0')
            return TRUE;
			
        bFound = FALSE;
			
        for (iterate = 0; iterate < HOWMANYPARMS; iterate++)
        {
            if (0 == _fstrnicmp(&theParm[1], SomeParms[iterate], 
                                SomeParmsLen[iterate]) )
            {
                bFound = TRUE;
                switch (iterate)
                {
                case ISCLEAR:
                    bDoClear = TRUE;
                    break;
                case ISSYNC:
                    bDoSync = TRUE;
                    break;
                case ISHIDE:
                    bDoHide = TRUE;
                    break;
                case ISQUIT:
                    bDoQuit = TRUE;
                    break;
                case ISASK:
                    bDoAsk = TRUE;
                    break;
                }
			
                tempLen = _fstrlen(theParm);
                temperLen = SomeParmsLen[iterate]+1;
                if (tempLen <= temperLen)
                    theParm[0] = '\0';
                else
                    theParm += temperLen;
                break;
            }
        }

        if (!bFound)
        {
            theMsg[0] = '\0';
            LoadString(hInst,IDS_BADPARAM,theMsg,GETPARAMSMSGBUFLEN-1);
            tempLen = _fstrlen(theMsg);
            temperLen = GETPARAMSMSGBUFLEN-1 - tempLen;
            _fstrncat(theMsg,theParm,temperLen);
            MessageBox(NULL,theMsg,lpszApp,MB_OK | MB_ICONSTOP);
            return FALSE;
        }
    }
}



/* ------------------------------- WinMain ----------------------------- */

int
WINAPI
WinMain(
    HANDLE hInstance, 
    HANDLE hPrevInstance, 
    LPSTR lpCmdLine, 
    int nCmdShow
    )
{
    MSG msg;
    RECT r;
	
    bDoClear = FALSE;
    bDoSync = FALSE;
    bDoHide = FALSE;
    bDoQuit = FALSE;
    bDoAsk = FALSE;
	
    gWnd = NULL;
	
    hInst = hInstance;
	
    if (!GetParams(lpCmdLine))
        return 0;
		
    /* if we're going to do something, get the settings now */
    if (bDoClear || bDoSync)
        GetSettings(&r);
		
    /* if the command line contains "/clear", then clear the password */
    if (bDoClear)
        DeleteAllSessions();

    /* if the command line contains "/sync", 
       then sync time with kerberos server */
    if (bDoSync)
        autosync(bDoAsk);
	
    if (bDoQuit)
        return 0;

    hPreviousWnd = GetActiveWindow();
    // bRefuseActiveOnce might be set TRUE in InitInstance
    bRefuseActiveOnce = FALSE;
    // haven't seen a WM_DDE_INITIALIZE yet
    bFirstDdeInitiate = TRUE;
	
    hRegularIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KVICON)) ;
    hKeyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZKEY)) ;
    hTheIcon = hRegularIcon ;

    if (!hPrevInstance)
    {
        if (!InitApplication(hInstance))
            return 0;
    }
    else
        return 0 ; 	// only allow one instance

    if (!InitInstance(hInstance, nCmdShow))
        return 0 ;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
			
    return(msg.wParam);
}

/* -------------------------------------------------------------------
	LshKinitWndProc - Message handler for main window.
	------------------------------------------------------------------- */
LRESULT CALLBACK LshKinitWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL bReturn ;
    RECT r ;			// list box positioning
    HMENU hMenu ;		// to check and uncheck menu items
    PAINTSTRUCT ps;
    HANDLE hMod ;		// track how many apps are using kclient.dll
    int	iCount ;		// track how many apps are using kclient.dll
    MSG peekMsg ;
//#if defined(_DEBUG)
//	char tWicket[4096];	// buffer for TicketForService test
//	long tSize;
//#endif

    if (msgAutoSync != 0 && message == msgAutoSync)
    {
        autosync(TRUE);
        return 0L;
    }

    switch (message)
    {
    case WM_DDE_INITIATE:
        // when Launch Pad (or whoever) is starting up, Kview gets
        // bombarded with these, and needs to ignore them quickly
        // instead of passing them back to the system.
        if (bFirstDdeInitiate)
        {
            bFirstDdeInitiate = FALSE;
            // yield once without reading a message immediately
            PeekMessage(&peekMsg,hWnd,0,0,PM_NOREMOVE);
        }
        break;

    case WM_TIMER:
        if (wParam == RC_TIMER)
        {
            // If we were started by kclient.dll, we should exit when
            // all other apps are done with kclient.dll.
            hMod = GetModuleHandle (KCLINAME) ;
            // XXX - criteria for autoclosing under win32 are not yet decided.
#if !defined(_WIN32)
            if (hMod > 0)
                iCount = GetModuleUsage (hMod) ;
            else
#endif
                iCount = 0 ;
            // yes, check auto-unload value
            if (iCount == 1)
            {
                iTimeOut ++ ;	// increment time out count
                if (iTimeOut >= iAutoUnload)  // if we hit setting, exit
                    PostMessage (hWnd, WM_CLOSE, 0, 0L) ;
            }
            else 
                iTimeOut = 0 ;	// reset time out value
        }
        else if (wParam == TGT_TIMER)
        {
            UpdateUserid();
        }
        break ;

#if 0			
    case WM_ACTIVATE:
        if (bRefuseActiveOnce)
        {
            bRefuseActiveOnce = FALSE;
            SetActiveWindow(hPreviousWnd);
            return 0;
        }
        else
            return (DefWindowProc(hWnd, message, wParam, lParam));
#endif /* 0 */
			
    case WM_PAINT:
        if (IsIconic(hWnd))
        {
            BeginPaint(hWnd,&ps);
            DrawIcon(ps.hdc,0,0,hTheIcon);
            EndPaint(hWnd,&ps);
            return 0;
        }
        else
            return (DefWindowProc(hWnd, message, wParam, lParam));

    case WM_QUERYDRAGICON:
        return (long)hTheIcon;

    case WM_KILLFOCUS:
    case WM_SETFOCUS:
        // yet another cheap way to stay visually up-to-date
	// show what we've got
        ListTickets (hListBox) ;
        UpdateUserid();
        break ;

    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case IDM_DEST_TKTS:
            if (DeleteAllSessions() == TRUE)
            {
                // stay visually up2date
                ListTickets (hListBox) ;
                ShowUserid("");
            }
            return 0;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
        }
			
        break;

    case WM_CREATE:
        GetClientRect (hWnd, &r) ;

        // listbox in which to display tickets
        hListBox = CreateWindow("listbox", NULL, 	
                                WS_CHILD | WS_VISIBLE | 
                                LBS_STANDARD | LBS_USETABSTOPS,
                                0, 0, r.right, r.bottom, hWnd, 
                                (HMENU)IDLB_1, hInst, NULL) ;

	// just in case they already have some
        ListTickets(hListBox) ;

        break ;

    case WM_SIZE:
        // size listbox to fit main window
	// make sure it has been created
        if (hListBox != 0x00)
            MoveWindow (hListBox, 0, 0, LOWORD (lParam), 
                        HIWORD (lParam), TRUE) ;
#if defined(_DEBUG)
        OutputDebugString(" KVS");
#endif
        break ;

    case WM_MOVE:
        // save current window position
        WriteSettings () ;
        break ;
         
    case WM_COMMAND:	
        switch (wParam)
        {
        case IDM_LSH_KINIT:
            // get a tgt, just to see if kerberos works		
            DeleteAllSessions() ;		// DeAuth their ass
            ShowUserid("");			// which means no userid
            // start the Windows Sockets stack 
            WSAStartup(0x0101, &wskData) ;

            bReturn = GetTicketGrantingTicket() ;	// try to get a TGT
#if 0
            // I needed to test the case when the user had a valid TGT
            // but an expired service ticket repeatedly.
            {
                DWORD dwLen ;
                KTEXT_ST tWicket;
                LPSTR service = "pop.postoffice5@CIT.CORNELL.EDU"
                LPSTR service = "pop.po10@ATHENA.MIT.EDU"

                SetTicketLifeTime(1);
                bReturn = GetTicketForService(service, &tWicket, &dwLen);
            }
#endif /* 0 */

            if (bReturn == TRUE)
            {
		// stay visually up2date
                ListTickets (hListBox) ;
                MessageBox (NULL, "Test successful.  "
                            "KClient appears to be working.", 
                            lpszApp, MB_OK | MB_ICONINFORMATION) ;
#if 1
                // I put this in so that I could toggle it off 
                // when doing some other debugging.
                DeleteAllSessions() ;		// delete tgt for good measure
#endif
                ListTickets (hListBox) ;	// stay visually up2date
            }
            else
                MessageBox (NULL, "Test not completed.  Ticket not acquired.", 
                            lpszApp, MB_OK | MB_ICONINFORMATION) ;
						
            WSACleanup() ;	// clean up WinSock stack after test
            break ;

        case IDM_DEST_TKTS:
            // delete all tickets
            if (DeleteAllSessions() == TRUE)
            {
		// stay visually up2date
                ListTickets (hListBox) ;
                ShowUserid("");
            }
            break ;

        case ID_FILE_LISTTICKETS:
            // List tickets
            // show'em what we've got
            ListTickets (hListBox) ;
            UpdateUserid();
            break ;

        case IDM_CHANGEPASSWORD:
            // Change password
            // start the Windows Sockets stack
            WSAStartup(0x0101, &wskData) ;
            ChangePassword () ;
            // clean up WinSock stack after test
            WSACleanup() ;
            // stay visually up2date
            ListTickets (hListBox) ;
            UpdateUserid();
            break ;

        case IDM_TIMESYNC:
            // Synchronize time
            timesync () ;
            break ;

#if 0
        case IDM_ASKAUTOSYNC:
            hMenu = GetMenu (hWnd) ;
            if ((GetMenuState (hMenu, IDM_ASKAUTOSYNC, MF_BYCOMMAND)) 
                == MF_CHECKED)
            {
                CheckMenuItem (hMenu, IDM_ASKAUTOSYNC, 
                               MF_BYCOMMAND | MF_UNCHECKED) ;
                bAskBeforeSync = FALSE;
            }
            else 
            {
                CheckMenuItem (hMenu, IDM_ASKAUTOSYNC, 
                               MF_BYCOMMAND | MF_CHECKED) ;
                bAskBeforeSync = TRUE;
            }
            WriteSettings();
            break ;
#endif /* 0 */

        case IDM_KRBDLL_DEBUG:
            // Debug mode
            // toggle debug mode on krbv4win.dll
            hMenu = GetMenu (hWnd) ;
            if ((GetMenuState (hMenu, IDM_KRBDLL_DEBUG, MF_BYCOMMAND)) 
                == MF_CHECKED)
            {
                CheckMenuItem (hMenu, IDM_KRBDLL_DEBUG,
                               MF_BYCOMMAND | MF_UNCHECKED) ;
                SetKrbdllMode (FALSE) ;
            }
            else 
            {
                CheckMenuItem (hMenu, IDM_KRBDLL_DEBUG, 
                               MF_BYCOMMAND | MF_CHECKED) ;
                SetKrbdllMode (TRUE) ;
            }
            break ;

        case IDM_PREFERENCES:	
            // settings
            DialogBox(hInst, MAKEINTRESOURCE(IDD_PREFERENCES), hWnd, Settings);
            break ;

        case IDM_FILE_EXIT:
            // Exit
            SendMessage(hWnd, WM_CLOSE, 0, 0L) ;	
            break ;

        case ID_HELP_ABOUT:
            // About Box
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, About) ;
            break ;
						
        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
        }
        break;
		
    case WM_RBUTTONDOWN:		
        // "Undocumented" test for KLite operation.
        // If right button double click and shift key is down, 
        // do a GetTicketForServiceTest on KLite
        if (wParam == (MK_SHIFT | MK_RBUTTON))
        {
            // only test if klite is loaded, not krbv4win.dll
            if (GetModuleHandle("klite") != 0)
            {
                // zero out to get an sz for sure
                memset (szKLiteTicket, '\0', sizeof(szKLiteTicket)) ;
		// start stack 
                WSAStartup(0x0101, &wskData) ;
                bReturn = GetTicketForService (lpszService, 
                                               (LPSTR)szKLiteTicket, &dwLen) ;
                if (bReturn == TRUE)
                {
                    // safety check, force an sz at szKLiteTicket[128]
                    szKLiteTicket[128] = '\0' ;
                    wsprintf ((LPSTR)szKLiteTest, 
                              "Stealth GetTicketForService() test passed.  "
                              "Current Ticket is: %s",
                              (LPSTR)szKLiteTicket) ;
                }
                else
                    wsprintf ((LPSTR)szKLiteTest, 
                              "Stealth GetTicketForService() test failed.") ;
					
                ListTickets (hListBox) ;	// stay visually up2date
                MessageBox (NULL, (LPSTR)szKLiteTest, lpszApp, 
                            MB_OK | MB_ICONINFORMATION) ;
                DeleteAllSessions() ;   	// delete tgt for good measure
                ShowUserid("");
                ListTickets (hListBox) ;	// stay visually up2date
                WSACleanup() ;
            }
        }
        break ;

#if 0
// Leave this out for now, allowing KView to be closed at any time
    case WM_CLOSE:
        hMod = GetModuleHandle (KCLINAME) ;
        iCount = GetModuleUsage (hMod) ;			
        if (iCount > 1)	// yes, check auto-unload value
        {
            if (MessageBox (NULL, "Other applications are using KClient.  "
                            "If you exit now, the system may become unstable.",
                            lpszApp, MB_OKCANCEL | MB_ICONSTOP) == IDOK) 
            {
                DeleteAllSessions() ;	// DeAuth them.
                // decrement ref count to 1
                for (iCount; iCount>1; iCount--)
                    FreeLibrary (hMod) ;
						
                DestroyWindow (hWnd) ;	// aloha
            }
        }
        else
        {
            DeleteAllSessions() ;	// DeAuth them.
            DestroyWindow (hWnd) ;	// bye bye
        }
				
        break ;
#endif			

    case WM_DESTROY:
        // time for the big dirt nap
	// get current user name from kclient.dll
#if defined(_WIN32)
        KCGetUserName((LPSTR)szNetID) ;
#else
        GetUserName((LPSTR)szNetID) ;
#endif
        WriteSettings () ;			// save settings
        if (bWeveBeenWinExecEd == TRUE)		// if the timer is live...
            KillTimer (hWnd, RC_TIMER) ;	// ...hose it.
					
        PostQuitMessage(0) ;				
        break;

    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    return (0L);
}


/* ------------------------------------------------------------------------
	GetSettings - get initial settings from kview.ini.
	
	rc - pointer to RECT struct which will be filled in with KView's
	most recently used window coordinates.
 ------------------------------------------------------------------------ */
void GetSettings (RECT *rc)
{	
    char szTemp[40] ;

    // NetID
    GetPrivateProfileString ("Settings", "NetID", "",
                             szNetID, sizeof(szNetID), "kview.ini") ;
    // Auto-load kview?
    GetPrivateProfileString ("Settings", "AutoLoad", "No", szTemp, 
                             sizeof(szTemp), "kview.ini") ;
    if (strlen(szTemp) > 2)  	// we got the "Yes" string
        bAutoLoad = TRUE ;	
    else
        bAutoLoad = FALSE ;

    // ask before autosync?
    GetPrivateProfileString ("Settings", "AskBeforeSync", "No", szTemp, 
                             sizeof(szTemp), "kview.ini") ;
    if ( 0 == _fstricmp(szTemp,"Yes"))
        bAskBeforeSync = TRUE ;	
    else
        bAskBeforeSync = FALSE ;
									
    // Ticket life time
    GetPrivateProfileString (lpszSettings, "LifeTime", "10", 
                             szTemp, sizeof(szTemp), "kview.ini") ;
    iLifeTime = atoi (szTemp) ;
	
    // Time Server
    GetPrivateProfileString (lpszSettings, "TimeServer", "ntp0.cornell.edu",
                             szTimeServer, sizeof(szTimeServer), "kview.ini") ;

    // window placement
    GetPrivateProfileString (lpszSettings, "Left", "100", 
                             szTemp, sizeof(szTemp), "kview.ini") ;
    rc->left = atoi (szTemp) ;	
    GetPrivateProfileString (lpszSettings, "Top", "100",
                             szTemp, sizeof(szTemp), "kview.ini") ;
    rc->top = atoi (szTemp) ;	
    GetPrivateProfileString (lpszSettings, "Right", "350",
                             szTemp, sizeof(szTemp), "kview.ini") ;
    rc->right = atoi (szTemp) ;	
	
    GetPrivateProfileString (lpszSettings, "Bottom", "250",
                             szTemp, sizeof(szTemp), "kview.ini") ;
    rc->bottom = atoi (szTemp) ;	
	
}

/* ------------------------------------------------------------------------
	WriteSettings - write settings to kview.ini.
 ------------------------------------------------------------------------ */
void WriteSettings (void)
{
    char szTemp[8] ;
    RECT rc ;

    // NetID
    WritePrivateProfileString (lpszSettings, "NetID", szNetID, "kview.ini") ;
	
    // Auto-load kview?
    if (bAutoLoad == TRUE)
        strcpy (szTemp, "Yes") ;
    else
        strcpy (szTemp, "No") ;
    WritePrivateProfileString (lpszSettings, "AutoLoad", szTemp, "kview.ini") ;
	
    // Ask before autosync?
    if (bAskBeforeSync == TRUE)
        strcpy (szTemp, "Yes") ;
    else
        strcpy (szTemp, "No") ;
    WritePrivateProfileString (lpszSettings, "AskBeforeSync", szTemp, 
                               "kview.ini") ;
	   
    // Ticket life time	
    itoa (iLifeTime, szTemp, 10) ;
    WritePrivateProfileString (lpszSettings, "LifeTime", szTemp, "kview.ini") ;

    // time server
    WritePrivateProfileString (lpszSettings, "TimeServer", szTimeServer, 
                               "kview.ini") ;

    // window placement
    // don't save window position if iconisized.
    if (!IsIconic(gWnd))
    {
        // get our current window placement
        GetWindowRect (gWnd, &rc) ;
        itoa (rc.left, szTemp, 10) ;
        WritePrivateProfileString(lpszSettings, "Left", szTemp, "kview.ini");
        itoa (rc.top, szTemp, 10) ;
        WritePrivateProfileString(lpszSettings, "Top", szTemp, "kview.ini");
        itoa (rc.right, szTemp, 10) ;
        WritePrivateProfileString(lpszSettings, "Right", szTemp, "kview.ini");
        itoa (rc.bottom, szTemp, 10) ;
        WritePrivateProfileString(lpszSettings, "Bottom", szTemp, "kview.ini");
    }
}

/* ------------------------------------------------------------------------
	Settings - dialog for settings
 ------------------------------------------------------------------------ */
BOOL CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szTemp[64] ;

    switch (message)
    {
    case WM_INITDIALOG:
        // NetId
        SendDlgItemMessage (hDlg, IDC_EDIT1, EM_LIMITTEXT, 40, 0) ;
        SendDlgItemMessage (hDlg, IDC_EDIT1, WM_SETTEXT, 0, 
                            (LPARAM)(LPCSTR)szNetID) ;

        // Ticket lifetime (in hours)
        SendDlgItemMessage (hDlg, IDC_EDIT2, EM_LIMITTEXT, 2, 0) ;
        wsprintf ((LPSTR)szTemp, "%d", iLifeTime) ;
        SendDlgItemMessage (hDlg, IDC_EDIT2, WM_SETTEXT, 0, 
                            (LPARAM)(LPCSTR)szTemp) ;
				
        // Auto-load kview?
        if (bAutoLoad == TRUE)
            CheckDlgButton (hDlg, IDC_CHECK1, 1) ;
			
        // Time Server
        SendDlgItemMessage (hDlg, IDC_EDIT3, EM_LIMITTEXT, 80, 0) ;
        SendDlgItemMessage (hDlg, IDC_EDIT3, WM_SETTEXT, 0, 
                            (LPARAM)(LPCSTR)szTimeServer) ;
			
        return (TRUE) ;
					
    case WM_COMMAND:
        if (wParam == IDOK) 
        {
            // NetID
            SendDlgItemMessage (hDlg, IDC_EDIT1, WM_GETTEXT, 40, 
                                (LPARAM)(LPCSTR)szNetID) ;

            // Ticket lifetime
            SendDlgItemMessage (hDlg, IDC_EDIT2, WM_GETTEXT, 2, 
                                (LPARAM)(LPCSTR)szTemp) ;
            iLifeTime = atoi (szTemp) ;
            iLifeTime = min (iLifeTime, MAXLIFETIME) ;	// clip max/min range
            iLifeTime = max (iLifeTime, MINLIFETIME) ;
            // this must be "MIT time uints!
            SetTicketLifeTime (iLifeTime * 12) ;
			
            // Time Server
            SendDlgItemMessage (hDlg, IDC_EDIT3, WM_GETTEXT, 80, 
                                (LPARAM)(LPCSTR)szTimeServer) ;

            // Auto-load kview?
            if (IsDlgButtonChecked (hDlg, IDC_CHECK1) == 1)
                bAutoLoad = TRUE ;
            else
                bAutoLoad = FALSE ;
						
            SetUserName((LPSTR)szNetID) ;	// Set user name in kclient.dll
            WriteSettings () ;			// save any new settings
				
            EndDialog(hDlg, TRUE);
            return (TRUE);
        }
									
        if (wParam == IDCANCEL)
        {
            EndDialog(hDlg, FALSE);
            return (TRUE);
        }				
        break;
    }
    return (FALSE);
}

/* ------------------------------------------------------------------------
	About - dialog for settings
 ------------------------------------------------------------------------ */
BOOL CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE ;
			
    case WM_COMMAND:
        if (wParam == IDOK) 
        {
            EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}



/* ------------------------------------------------------------------------
	Autosync time with kerberos server.  parm is whether to ask first.
	Note: this does NOT check whether the time difference is bad enough to
	need syncing.
 ------------------------------------------------------------------------ */


void autosync(int bWhetherAsk)
{
    int bDoIt,tLength;
#define ASMSGBUFSIZE 384
    char theMsg[ASMSGBUFSIZE];
	
    bDoIt = TRUE;
	
    if (bWhetherAsk && bAskBeforeSync)
    {  
        theMsg[0] = '\0';
        LoadString(hInst,IDS_ASKSYNC1,theMsg,ASMSGBUFSIZE-1);
        tLength = _fstrlen(theMsg);
        if (tLength > 0)
        {
            LoadString(hInst, IDS_ASKSYNC2, &theMsg[tLength], 
                       (ASMSGBUFSIZE-1)-tLength);
            if (IDCANCEL == MessageBox(gWnd, theMsg, lpszApp, 
                                       MB_OKCANCEL | MB_ICONQUESTION))
                bDoIt = FALSE;
        }
    }

    if (bDoIt)	
        timesync();
}



void UpdateUserid()
{
    char tempUserName[256];

    if (TgtExist())
    {
#if defined(_WIN32)
        KCGetUserName(tempUserName);
#else
        GetUserName(tempUserName);
#endif
        ShowUserid(tempUserName);
    }
    else
        ShowUserid("");
}


/* ------------------------------------------------------------------------
    Display the indicated string as the beginning of the title of this app.
 ------------------------------------------------------------------------ */

void ShowUserid(LPSTR theString)
{
    char theTitle[300];
    RECT tRect;

    if (0 == _fstrcmp(theString,titleUserid))
        return;	// already shown

    _fstrcpy(theTitle,theString);
    if (theTitle[0] != '\0')
        _fstrcat(theTitle," -- ");

    _fstrcat(theTitle,lpszApp);
	
    if (gWnd != 0)
    {
        SetWindowText(gWnd,theTitle);
        _fstrcpy(titleUserid,theString);
 		
        if (theString == NULL || theString[0] == '\0')
            hTheIcon = hRegularIcon;
        else
            hTheIcon = hKeyIcon;
        if (IsIconic(gWnd))
        {
            tRect.left = 0;
            tRect.top = 0;
            tRect.right = GetSystemMetrics(SM_CXICON) - 1;
            tRect.bottom = GetSystemMetrics(SM_CYICON) - 1;
            InvalidateRect(gWnd,&tRect,TRUE);
            UpdateWindow(gWnd);
        }
    }
}
