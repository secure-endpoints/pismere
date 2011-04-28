/*
  Details on building kclient.dll or kclnt32.dll
  -------------------------------
  Project type: Windows dynamic-link library
  Memory model: small (if 16-bit)
  Segment setup: SS!=DS  DS NOT loaded on function entry

  Preprocessor, Symbols and macros to define: WINDOWS, IBMPC, WINSOCK,
  (KLITE if using klite.dll instead of krbv4win.dll)

  Explicity link with krbv4win.lib in project (unless compiling for KLite)

  Files in project:
    kclient.c    (shared between both versions)
    kclient.def  (or kclien32.def)
    kclient.rc   (or kclien32.rc)
    krbv4win.lib (or krbv4w32.lib)

  Please don't use TABs.  TABs are at 8 spaces.  Indenting is set to 4.

  Copyright © 1996 by Project Mandarin, Inc.

  Note: MIT's lifetime "units" are in 5 minute intervals.  That is,
  a lifetime of 96 would be 480 minutes (8 hours).
*/

//#define FANCY_CRITICAL 1

#include <windows.h>
#include <limits.h>

#ifdef WINSOCK
#include <winsock.h> // added by MIT for SendTicketToServer
#endif


#include "resource.h"


#if defined(KLITE)
#include "klite.h"
#include "klite_v4.h"
#else // !KLITE

#include <krb.h>
// borrowed from kadm_err.h 
// (because there's an extern in there I don't need):
#define ERROR_TABLE_BASE_kadm (-1783126272L)
// borrowed from krberr.h ditto
#define ERROR_TABLE_BASE_krb (39525376L)
// borrowed from leasherr.h ditto
#define ERROR_TABLE_BASE_lsh (40591872L)
// #include <krblayer.h>
#include "sselayer.h"
typedef CREDENTIALS FAR * LPCREDENTIALS;

#endif // !KLITE

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

/* if you have to change these to lowercase, see EnumToFindKview */
#define KVIEW16NAME "KVIEW.EXE"
#define KVIEW32NAME "KVIEW32.EXE"

#if defined(_WIN32)
#ifndef _fmemcpy
#include <pcrt32.h>
#endif
#include <process.h>
#define KVIEWNAME KVIEW32NAME
#define KVIEWOTHERNAME KVIEW16NAME

//#define KVIEW_AUTOLOAD //uncomment this define to enable KVIEW autoload feature

HINSTANCE  m_hLeashDLL = 0;
HINSTANCE  m_hKrb5DLL = 0;

#include <loadfuncs-leash.h>

DECL_FUNC_PTR(Leash_kinit) = 0;
DECL_FUNC_PTR(Leash_get_default_lifetime) = 0;
DECL_FUNC_PTR(Leash_changepwd_dlg_ex) = 0;
DECL_FUNC_PTR(Leash_kinit_dlg_ex) = 0;

FUNC_INFO leash_fi[] = {
    MAKE_FUNC_INFO(Leash_kinit),
    MAKE_FUNC_INFO(Leash_get_default_lifetime),
    MAKE_FUNC_INFO(Leash_changepwd_dlg_ex),
    MAKE_FUNC_INFO(Leash_kinit_dlg_ex),
    END_FUNC_INFO
};

#define LEASHDLL "leashw32.dll"
#define KRB5_DLL "krb5_32.dll"

#else // !_WIN32
#if defined(FANCY_CRITICAL)
#include "wownt16.h"
#endif // FANCY_CRITICAL
  
// for _FP_OFF and _FP_SEG
#include <dos.h>
  
#define KVIEWNAME KVIEW16NAME
#define KVIEWOTHERNAME KVIEW32NAME
// These are defined in winuser.h if (WINVER >= 0x0400)
// 16 bit api says to look for 0, 1, or 2 ... 
#define BST_UNCHECKED 0x0000
#define BST_CHECKED 	0x0001
#endif // !_WIN32

/* ??? debugging strings */
static char sLoadit[] = "loadit";
static char sNoload[] = "noload";

// these constants are lifted verbatim from the win32 headers
#if !defined(VER_PLATFORM_WIN32s)
# define VER_PLATFORM_WIN32s 0
#endif

#if !defined(VER_PLATFORM_WIN32_WINDOWS)
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#if !defined(VER_PLATFORM_WIN32_NT)
# define VER_PLATFORM_WIN32_NT 2
#endif

//these constants are made up
# define VER_PLATFORM_WIN16 0xffffffff
# define VER_PLATFORM_UNKNOWN 0xfffffffe

/* ------------------------------ Symbolic Constants -------------------- */
#define	KRB_SENDAUTH_VERS "AUTHV0.1" /* MUST be KRB_SENDAUTH_VLEN chars */
#define KRB_SENDAUTH_VLEN  8				
#define	MAXLIFETIME 24
#define	MINLIFETIME 1
#define MINPASSLENGTH 6
#define	MAXPASSERROR 256 

/* ------------------------------ Function Prototypes -------------------- */
int CALLBACK LibMain (HANDLE, WORD, WORD, LPSTR) ;
int CALLBACK WEP (int) ;

// here are the forward references; this also includes kcmacerr.h
#include "kclient.h"

void ShutMeDown(void) ;

#if defined(_WIN32)
BOOL CALLBACK EnumToFindKView(HWND hwnd,LPARAM lParam);
void LoadKViewThread(void * arglist);
extern void LoadKView(void);
#endif // _WIN32

BOOL CALLBACK UserInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChangePasswordDialog(HWND, UINT, WPARAM, LPARAM) ;

BOOL GTGTPrivate(char *);
BOOL DASPrivate(void);
BOOL CopyTicket(LPSTR, KTEXT, LPDWORD, LPSTR, BOOL);
BOOL ParseName (LPSTR, LPSTR, LPSTR, LPSTR);
BOOL IsCredExpired (CREDENTIALS *);
BOOL PassError (int iError);
void KurtsSplashScreen (HANDLE);

BOOL TryToResynchronize(void);

void DeterminePlatform(void);

void ErrorMsg (long);
// int x_htons (int iVal) ;

/* ----------------------------- Global Variables ------------------------ */
#if defined(_WIN32)
OSVERSIONINFO myInfo;
#else // !_WIN32
struct {
    DWORD dwPlatformId;
} myInfo;
#endif // !_WIN32

HANDLE hInst;           // all purpose instance
BOOL   bReturn;         // all purpose return value
BOOL   bForceUCRealm;   // global switch inidicates whether to force realm 
                        // name to UC or not.
                        // this may be set in the ini file, if not found 
                        // there use IDS_FORCE_UCREALM value.

KTEXT_ST auth; 	        // Ticket 
CREDENTIALS c;          // ticket info and more 
LPCREDENTIALS lpc;
CREDENTIALS xc;         // ticket info and more for TgtExist
LPCREDENTIALS lpxc;

char szNetID[MAX_K_NAME_SZ]; // Principal, can be NetID or name.instance@realm
char szPass[255];            // password
char szNewPass[255];         // new password
char szConfirmPass[255];     // confirmed new password

char szPassErrorText[MAXPASSERROR]; // Change password errors go here

char szRealm[REALM_SZ]; // default realm 
int iLife = 120;        // lifetime of credential (10 hours)

HWND hSinkerWindow;     // used by the timesync routine (to find KView)
#define CLASSNAMEBUFSIZE 260
char szClassName[CLASSNAMEBUFSIZE]; // class name of window being inspected 
                                    // by above

long lKerror;    // all purpose kerberos call return value
OSErr sOSErr;    // all purpose kclient call return value
char szUserName[ANAME_SZ];   // user name
char szUserInst[INST_SZ];    // "user" instance (where kerberos server lives)
char szUserRealm[REALM_SZ];  // realm of kerberos server
char szxUserInst[INST_SZ];   // same for TgtExist
char szSrvcName[SNAME_SZ];   // name of service being requested
char szSrvcInst[INST_SZ];    // instance of service being requested
char szSrvcRealm[REALM_SZ];  // realm of requested service?? 
                             // (not sure about this realm stuff)
long lChecksum; // checksum (for mutual authentication)
time_t tLocal;  // local time 
TICKETINFO ti;  // ticket information

UINT msgAutoSync; // registered message to send to KView to get it to autosync

char szTemp[384];
BOOL bExpired;
BOOL bReturn;

// do we use the Kurt "I want to be a Zamoboni Driver" bitmap option?
BOOL bKurt;

BOOL bAskBeforeSync;
BOOL bDoSync;
PSTR p1; // ick!

RECT r_desk; // used for centering password dialog (desktop window)
RECT r_dlg;  // used for centering password dialog (password dialog)


/* --------------------- Critical-Sectioning Stuff ----------------------- */

#if !defined(KLITE)
// this table maps kerberos error number bases to 
// bases in the resources of this dll.
static long errorTableMap [4] [2] = 
{
    0L, COM_ERR_RESOURCE_BASE,
    ERROR_TABLE_BASE_kadm, KADM_ERR_RESOURCE_BASE,
    ERROR_TABLE_BASE_krb,  KRB_ERR_RESOURCE_BASE,
    ERROR_TABLE_BASE_lsh,  LSH_ERR_RESOURCE_BASE
};
#endif


enum { KCLIENTCRITICAL = 1, KERB4CRITICAL };

typedef struct {
    int whichOne;
#if defined(_WIN32)
    HANDLE hMutex;
#else
    DWORD hMutex;
#endif
} ACritical;

#if !defined(KLITE)
static LPSTR kMutexName = "Kerb4Mutex";
ACritical Kerb4Critical;
#endif

#if defined(_WIN32)
HANDLE hKViewLauncherThread;
#define KCLIENT_IN_USE_COUNTER_MAPPING "KClient DLL In-Use Counter"
HANDLE hMapping;
DWORD *inUseCounter = 0;
#endif

static LPSTR kClientMutexName = "KClientMutex" ;
ACritical KClientCritical;

void myGrabMutex(ACritical *theCritical);
void myLetGoMutex(ACritical *theCritical);

#if defined(FANCY_CRITICAL)

#if !defined(_WIN32)
DWORD ghKern32Lib;	/* handle to the 32-bit kernel dll */
HINSTANCE ghWow32Lib;	/* handle to the WOW functions */

/* if 16-bit running under 32-bit, we will STILL need to be 
   able to call: GetLastError(), CreateMutex(), 
   WaitForSingleObject(), ReleaseMutex(), CloseHandle() */
typedef DWORD (WINAPI * tpGetLastError)(void);
typedef HANDLE (WINAPI * tpCreateMutex)(DWORD,BOOL,LPCSTR);
typedef BOOL (WINAPI * tpCloseHandle)(HANDLE);
typedef DWORD (WINAPI * tpWaitForSingleObject)(HANDLE,DWORD);
typedef BOOL (WINAPI * tpReleaseMutex)(HANDLE);

/* the mutex things are all 32-bit stuff! */
HANDLE WINAPI CreateMutex(
    DWORD dwDesiredAccess,	// access flag 
    BOOL bInheritHandle,	// inherit flag 
    LPCSTR lpName 	// pointer to mutex-object name 
    );
   
BOOL WINAPI CloseHandle(
    HANDLE hObject 	// handle to object to close  
    );
   
DWORD WINAPI GetLastError(VOID);

DWORD WINAPI WaitForSingleObject(
    HANDLE hHandle,	// handle of object to wait for 
    DWORD dwMilliseconds 	// time-out interval in milliseconds  
    );
   
BOOL WINAPI ReleaseMutex(
    HANDLE hMutex 	// handle of mutex object  
    );
#endif // !_WIN32

#endif // FANCY_CRITICAL


#if !defined(KLITE)
#define GRAB_KERBEROS()    myGrabMutex(&Kerb4Critical)
#define RELEASE_KERBEROS() myLetGoMutex(&Kerb4Critical)
#else // KLITE
#define GRAB_KERBEROS()
#define RELEASE_KERBEROS()
#endif // KLITE


#if !defined(_WIN32)
volatile BOOL far bInUse; // used to prevent multiple entries

// 16-bit Windows has non-preemptive multitasking, and no 
// critical section mechanism.  at least we can declare
// bInUse to be volatile.
#define LOCK_REENTRY \
{ \
    lKerror = 0; \
    sOSErr = 0; \
    if (bInUse) \
        return; \
    bInUse = TRUE; \
}
#define LOCK_REENTRY_RETVAL \
{ \
    lKerror = 0; sOSErr = 0; \
    if (bInUse) \
    { \
        sOSErr = cKrbDriverInUse; \
        return FALSE; \
    } \
    bInUse = TRUE; \
}
#define UNLOCK_REENTRY { bInUse = FALSE; }

#else // _WIN32

#define LOCK_REENTRY \
{ \
    lKerror = 0; \
    sOSErr = 0; \
    myGrabMutex(&KClientCritical); \
}
#define LOCK_REENTRY_RETVAL LOCK_REENTRY
#define UNLOCK_REENTRY myLetGoMutex(&KClientCritical)

#endif // _WIN32


LPSTR lpszSettings = "Settings" ;

#ifdef KLITE
LPSTR lpszMsgBoxTitle = "KLite Error" ;
#else // !KLITE
LPSTR lpszMsgBoxTitle = "Kerberos Error" ;
#endif // !KLITE


/* when MIT Kerberos returns a socket error, this table is used to  */
/* map some of those numbers into MacTCP error numbers, so that     */
/* this can be returned by way of KClientError().  for things which */ 
/* are not defined as Berkeley sockets errors, or which have no     */
/* equivalent in MacTCP error numbers, they come through here as    */
/* cKrbKerberosErrBlock, which indicates that the user must use     */
/* KClientKerberosError() instead. */

signed short SockErrToMacTCP[100] = {
/* offset 0 to 9 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 10 to 19 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 20 to 29 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 30 to 39 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,connectionDoesntExist,
    ipBadAddr,
/* offset 40 to 49 */
    invalidLength,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 50 to 59 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    connectionTerminated,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 60 to 69 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 70 to 79 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 80 to 89 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,
/* offset 90 to 99 */
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock,cKrbKerberosErrBlock,cKrbKerberosErrBlock,
    cKrbKerberosErrBlock
};


#ifdef _WIN32
#ifdef SHARED_MUTEX
// sets the DACL for the given security descriptor such that everyone
// has full access to it
BOOL set_sd_all_access(PSECURITY_DESCRIPTOR pNewSD)
{
    if (!InitializeSecurityDescriptor(pNewSD, 
                                      SECURITY_DESCRIPTOR_REVISION)) {
        return(FALSE);
    }
    if (!SetSecurityDescriptorDacl(pNewSD, TRUE, NULL, FALSE)) {
        return(FALSE);
    }
    return TRUE;
}
#endif // SHARED_MUTEX
#endif // _WIN32

/* -------------------------------------------------------------------
   LibMain or DllMain
   ------------------------------------------------------------------- */
#if !defined(_WIN32)
int FAR PASCAL
LibMain(
    HANDLE hInstance,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine
    )
#else
BOOL WINAPI DllMain(
    HINSTANCE  hinstDLL,	// handle to DLL module 
    DWORD   fdwReason,		// reason for calling function 
    LPVOID  lpvReserved 	// reserved
    )
#endif
{
    char szDef[24]; // only needs to hold a boolean stored as a string.

#if defined(_WIN32)
    SECURITY_ATTRIBUTES *secptr;
#ifdef SHARED_MUTEX
    char newSD[SECURITY_DESCRIPTOR_MIN_LENGTH];
    PSECURITY_DESCRIPTOR pNewSD = (PSECURITY_DESCRIPTOR)newSD;
    SECURITY_ATTRIBUTES security_attributes;
    OSVERSIONINFO osinfo;
#endif
#endif

#if defined(FANCY_CRITICAL) & !defined(_WIN32)
    DWORD pGetLastError;
    DWORD pCreateMutex;
#endif

#if defined(_WIN32)
    int iDemiseAttempts;
    DWORD dExitCode;
    HANDLE tLoaderMutex;
    BOOL bGotIt;
    DWORD dWaitRet;
#endif

#if defined(_WIN32)
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        LoadFuncs(LEASHDLL, leash_fi, &m_hLeashDLL, 0, 0, 1, 1);
        m_hKrb5DLL = LoadLibrary(KRB5_DLL);
#else
        bInUse = 0 ;
#endif

#if !defined(_WIN32)
        hInst = hInstance ;		// save instance 
        if (wHeapSize > 0)
            UnlockData (0) ;
#else
#if defined(_DEBUG)
        OutputDebugString("32 bit\n");
#endif
        hInst = hinstDLL ;		// save instance
        hKViewLauncherThread = NULL;
#endif
        lpc = (LPCREDENTIALS)&c ;
        lpxc = (LPCREDENTIALS)&xc ;

        // Get NetID from kview settings file
        GetPrivateProfileString (lpszSettings, "NetID", "",
                                 szNetID, sizeof(szNetID), "kview.ini") ;

        // Ticket lifetime
        if (pLeash_get_default_lifetime &&
            (iLife = pLeash_get_default_lifetime()))
        {
            // convert to minutes
            iLife = iLife / 60;
        } else {
            GetPrivateProfileString (lpszSettings, "LifeTime", "10", 
                                     szTemp, sizeof(szTemp), "kview.ini") ;
            iLife = atoi(szTemp);
        }
	// clip max/min range
        iLife = min(iLife, MAXLIFETIME);
        iLife = max(iLife, MINLIFETIME);
        // convert to MIT Time Units
        iLife = iLife * 12;

        // do we use the Kurt "I want to be a Zamboni Driver" Richardson 
        // bitmap option?
        // see KurtsSplashScreen()
        LoadString (hInst, 900, (LPSTR)szTemp, sizeof(szTemp)) ;
        if (strcmp(szTemp, "LoadBitmap") == 0)
            bKurt = TRUE ;
        else
            bKurt = FALSE ;
        DeterminePlatform();

        // set UCRealm switch, use INI setting with string table setting as 
        // backup user can override via checkbox in authentication dialog
        bForceUCRealm = FALSE;
        LoadString(hInst, IDS_FORCE_UCREALM, szDef, sizeof(szDef)) ; 
        GetPrivateProfileString(lpszSettings, "ForceUppercaseRealm", 
                                szDef, szTemp, sizeof(szTemp), "kview.ini") ;
        if (_fstrlen(szTemp) == 3 && 0 == _fstricmp(szTemp,"Yes") )
            // we got the "Yes" string
            bForceUCRealm = TRUE ;
        GetPrivateProfileString(lpszSettings, "AutoLoad", "No", szTemp, 
                                sizeof(szTemp), "kview.ini") ;
        if (_fstrlen(szTemp) == 3 && 0 == _fstricmp(szTemp, "Yes") )
            // we got the "Yes" string
        {
            lKerror = 0 ;
            sOSErr = 0 ;
            KClientCritical.whichOne = KCLIENTCRITICAL;
            KClientCritical.hMutex = 0;
#if !defined(KLITE)
            Kerb4Critical.whichOne = KERB4CRITICAL;
            Kerb4Critical.hMutex = 0;
#endif

#if defined(_WIN32)
				
#if defined(_DEBUG)
            OutputDebugString("threading\n");
#endif
            /*
              can't use GetModuleHandle (see below), so call a function that
              will do all the rigamarole for either Win95 or WinNT.  because
              it has to do a lot of crunching, especially in NT, make it a
              separate thread. */
#ifdef KVIEW_AUTOLOAD
            hKViewLauncherThread = (HANDLE) _beginthread(LoadKViewThread,0,NULL);
            if ((unsigned long)hKViewLauncherThread == -1)
            {
                hKViewLauncherThread = NULL;
                /* probably being thunked from 16-bit app */
                LoadKViewThread(NULL); 
            }
#endif // end KVIEW_AUTOLOAD
#else // !_WIN32

/* in full 32-bit Windows, GetModuleHandle would notice the presence
   of a module ONLY IF it is loaded into the current process'
   address space!  so, we can only use it in 16 bits.  on the other 
   hand, it doesn't need toolhelp.dll, so we can use it freely. */
            if ((GetModuleHandle(KVIEWNAME) == 0) && 
                (GetModuleHandle(KVIEWOTHERNAME) == 0))
            {
                WinExec (KVIEWNAME,  SW_MINIMIZE) ;
            }
#endif // !_WIN32
        }

        msgAutoSync = RegisterWindowMessage("KViewAutoSync");

#if defined(_WIN32)
#ifdef SHARED_MUTEX
        osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        // get information on windows version
        if (GetVersionEx(&osinfo) == FALSE) 
            return(FALSE);
        if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
            if (set_sd_all_access(pNewSD) == FALSE)
                return(FALSE);
            security_attributes.lpSecurityDescriptor = pNewSD;
            security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
            security_attributes.bInheritHandle = TRUE;
            secptr = &security_attributes;
        }
        else
            secptr = NULL;  // if we're not running NT, just use a NULL SD
#else
        secptr = NULL;
#endif

#if !defined(KLITE)
        Kerb4Critical.hMutex = CreateMutex(NULL, FALSE, kMutexName) ; 
#endif
        KClientCritical.hMutex = CreateMutex(NULL, FALSE, kClientMutexName) ; 
#else // !_WIN32

#if defined(FANCY_CRITICAL)
#if defined(_DEBUG)
        OutputDebugString("a\n");
#endif
        if (myInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || 
            myInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
        {
#if defined(_DEBUG)
            OutputDebugString("b\n");
#endif
            /* connect to the WOW software */
            // ghWow32Lib = LoadLibrary("wow32.dll");
            // if (ghWow32Lib >= HINSTANCE_ERROR)
            // {
            /* prepare to call CreateMutex */
#if defined(_DEBUG)
            OutputDebugString("c\n");
#endif
            ghKern32Lib = LoadLibraryEx32W( "kernel32.dll", (DWORD)NULL, 0 );
            if (ghKern32Lib >= HINSTANCE_ERROR)
            {
#if defined(_DEBUG)
                OutputDebugString("d\n");
#endif
                pCreateMutex = GetProcAddress32W(ghKern32Lib, "CreateMutex");
                pGetLastError = GetProcAddress32W(ghKern32Lib, "GetLastError");
            }
            // } else {
            //     MessageBox("Attempt to load WOW library in KClient "
            //                "unsuccessful.", "KClient", MB_OK | MB_ICONSTOP);
            //     return 0;
            // }
            if (
                // ghWow32Lib < HINSTANCE_ERROR || 
                ghKern32Lib < HINSTANCE_ERROR || 
                0 == pCreateMutex || 
                0 == pGetLastError)
            {
                MessageBox(NULL,
                           "The Kerberos Client cannot access the 32-bit "
                           "functions it needs to proceed.",
                           "KClient", MB_OK | MB_ICONSTOP);
                return 0;
            }
            /* okay, now call CreateMutex for both things */
#if !defined(KLITE)
#if defined(_DEBUG)
            OutputDebugString("e\n");
#endif
            Kerb4Critical.hMutex = CallProcEx32W(3 | CPEX_DEST_STDCALL, 
                                                 0x00000005,
                                                 pCreateMutex, NULL, FALSE, 
                                                 kMutexName);
#endif
            KClientCritical.hMutex = CallProcEx32W(3 | CPEX_DEST_STDCALL, 
                                                   0x00000005, 
                                                   pCreateMutex, NULL, FALSE, 
                                                   kClientMutexName);
        }
        /* no prep needed here on actual 16-bit platform */
#endif
#endif

#if defined(_WIN32) || defined(FANCY_CRITICAL)
        if (KClientCritical.hMutex == 0
#if !defined(KLITE)
            || Kerb4Critical.hMutex == 0
#endif // !KLITE
            )
        {
            MessageBox(NULL, "The Kerberos Client finds itself unable "
                       "to guard against simultaneous use.",
                       "KClient", MB_OK | MB_ICONSTOP);
            return 0;
        }
#endif // _WIN32 || FANCY_CRITICAL

#if defined(_WIN32)
        // now we need to set up the shared memory for storing the
        // kclient dll in-use counter
        // XXX - how do we handle an error?
        GRAB_KERBEROS();
        hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
                                     secptr,
                                     PAGE_READWRITE,
                                     0,
                                     sizeof(*inUseCounter),
                                     KCLIENT_IN_USE_COUNTER_MAPPING);
        if(hMapping) {
            inUseCounter = (DWORD*)MapViewOfFile(hMapping,
                                                 FILE_MAP_ALL_ACCESS, 
                                                 0, 0, 0);
            if (inUseCounter)
            {
                if(GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    // we're the only copy of the dll running right now
                    *inUseCounter = 1;
                } else {
                    *inUseCounter++;
                }
            }
        }
        RELEASE_KERBEROS();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        // more to do later, perhaps
        break;

    case DLL_PROCESS_DETACH:
        if (m_hLeashDLL != NULL)
            FreeLibrary(m_hLeashDLL);
        if (m_hKrb5DLL != NULL)
            FreeLibrary(m_hKrb5DLL);
        pLeash_kinit = NULL;
        m_hLeashDLL = NULL;
        m_hKrb5DLL = NULL;
        if (hKViewLauncherThread != NULL)
        {
            dExitCode = 0;
            GetExitCodeThread(hKViewLauncherThread, &dExitCode);
            if (STILL_ACTIVE == dExitCode)
            {
                tLoaderMutex = CreateMutex(NULL, FALSE, "KClientLoadsKView");
                if (tLoaderMutex != NULL)
                {
                    iDemiseAttempts = 0;
                    do {
                        bGotIt = FALSE;
                        dWaitRet = WaitForSingleObject(tLoaderMutex, 500);
                        if (dWaitRet == WAIT_ABANDONED || 
                            dWaitRet == WAIT_OBJECT_0)
                            ReleaseMutex(tLoaderMutex);
                        GetExitCodeThread(hKViewLauncherThread, &dExitCode);
                    } while(STILL_ACTIVE == dExitCode && 
                            iDemiseAttempts++ < 4);
                    CloseHandle(tLoaderMutex);
                }
            }
        }

        // decrement the dll in-use counter and clean up
        GRAB_KERBEROS();
        if (inUseCounter)
        {
            *inUseCounter--;
            UnmapViewOfFile((void *)inUseCounter);
        }
        if (hMapping)
            CloseHandle(hMapping);
        RELEASE_KERBEROS();

        ShutMeDown();
        break;

    };
#endif // _WIN32
    return(TRUE);
}

/* -------------------------------------------------------------------
   WEP
   ------------------------------------------------------------------- */
#if !defined(_WIN32)
int FAR PASCAL WEP (int nParam) 
{
    ShutMeDown();
    return(1);
}
#endif



/* -------------------------------------------------------------------
   ShutMeDown
	------------------------------------------------------------------- */
void ShutMeDown(void)
{
#if defined(FANCY_CRITICAL) & !defined(_WIN32)
    DWORD pCloseHandle;
    DWORD pGetLastError;
#endif

#if defined(_WIN32)
#if !defined(KLITE)
    if (Kerb4Critical.hMutex != NULL)
        CloseHandle(Kerb4Critical.hMutex);
#endif // !KLITE
    if (KClientCritical.hMutex != NULL)
        CloseHandle(KClientCritical.hMutex);
#else // !_WIN32
#if defined(FANCY_CRITICAL)
    if (myInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || 
        myInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        pCloseHandle = GetProcAddress32W(ghKern32Lib, "CloseHandle");
        pGetLastError = GetProcAddress32W(ghKern32Lib, "GetLastError");
        if (KClientCritical.hMutex != 0)
            CallProcEx32W(1 | CPEX_DEST_STDCALL, 0, pCloseHandle, 
                          NULL, FALSE, KClientCritical.hMutex);
#if !defined(KLITE)
        if (Kerb4Critical.hMutex != 0)
            CallProcEx32W(1 | CPEX_DEST_STDCALL, 0, pCloseHandle, 
                          NULL, FALSE, Kerb4Critical.hMutex);
#endif // !KLITE
        FreeLibrary32W( ghKern32Lib );
        FreeLibrary( ghWow32Lib );
    }
#endif // FANCY_CRITICAL
#endif // !_WIN32
}


#if defined(_WIN32)

/* -------------------------------------------------------------------
   LoadKViewThread
   ------------------------------------------------------------------- */

/* this function is run as a separate thread.  it determines whether */
/* KView is already running; if not, it starts it. */

void LoadKViewThread(void * arglist)
{
    LoadKView();
}


/* -------------------------------------------------------------------
   EnumToFindKView
   ------------------------------------------------------------------- */

#define BUFFSIZE 261
/* look for either Kview or Kview32.  if either one is found,    */
/* set the BOOL pointed to by lParam to FALSE, and quit looking. */
/* needed only for win 95 or NT. */
BOOL CALLBACK EnumToFindKView(HWND hwnd,LPARAM lParam)
{
    HINSTANCE tInstance;
    char sBuffer[BUFFSIZE];
    DWORD tLen;
    WORD iterate;
    BOOL * pBool;

    tInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
    if (0 != (tLen = GetModuleFileName(tInstance,sBuffer,BUFFSIZE-1)) )
    {
#if defined(_DEBUG)
        OutputDebugString("kclient a\n");
#endif
        for (iterate = 0; iterate < tLen; iterate++)
            sBuffer[iterate] = toupper(sBuffer[iterate]);
        if ( NULL != strstr(sBuffer,KVIEWNAME) ||
             NULL != strstr(sBuffer,KVIEWOTHERNAME) )
        {
            /* FOUND! */
            pBool = (BOOL *)lParam;
            *pBool = FALSE;
            return FALSE;
        }
    }
    return TRUE;
}

#endif // _WIN32



/* -------------------------------------------------------------------
   GetTicketForService() 

   pszService - service name string
   pTicket - address of ticket buffer
   lpdwLen - address of ticket length

   Copy ticket for service specified in pszService into pTicket, and copy 
   length of ticket into lpdwLen.  Get the ticket (and a TGT) if needed.

   Returns:  TRUE on success.  FALSE otherwise
   ------------------------------------------------------------------- */
BOOL KC_CALLTYPE KC_EXPORT
GetTicketForService(
    LPSTR pszService,
    LPSTR pTicket,
    LPDWORD lpdwLen
    )
{
    LOCK_REENTRY_RETVAL;

    // First, check out input params
    if (pszService == NULL || pszService[0] == 0 || pTicket == NULL)
    {
        UNLOCK_REENTRY ;
        return (FALSE) ;
    }

    // Get the service realm, which we use as the user realm
    ParseName (szSrvcName, szSrvcInst, szSrvcRealm, pszService) ;

    if (!GTGTPrivate(szSrvcRealm))
    {
        UNLOCK_REENTRY ;
        return (FALSE) ;
    }

    // get specified service ticket
    GRAB_KERBEROS();
    lKerror = krb_mk_req(&auth, szSrvcName, szSrvcInst, 
                         szSrvcRealm, lChecksum) ;
    RELEASE_KERBEROS();

#if !defined(KLITE)
    if (lKerror == ERROR_TABLE_BASE_krb + RD_AP_TIME) // sync error, try again
    {
        if (TryToResynchronize())
        {
            GRAB_KERBEROS();
            lKerror = krb_mk_req(&auth, szSrvcName, szSrvcInst, 
                                 szSrvcRealm, lChecksum) ;
            RELEASE_KERBEROS();
        }
    }
#endif // !KLITE

    if (lKerror)
    {
        ErrorMsg (lKerror);
        UNLOCK_REENTRY;
        return (FALSE); // could not get specified service ticket
    }

    CopyTicket(pTicket, &auth, lpdwLen, "", FALSE);
    UNLOCK_REENTRY;
    return (TRUE);
}		





/* ------------------------------------------------------------------------
   GTGTPrivate() - the guts of GetTicketGrantingTicket.
   The real one has the reentrancy code around it so it can be
   called by external programs.  This part is here so it can
   be called directly by GetTicketForService without messing
   up the reentrancy stuff. 

   Returns:  TRUE on success.  FALSE otherwise
   ------------------------------------------------------------------------ */

BOOL
GTGTPrivate(
    char * realm
    )
{
    int     iTmp = 0;
    char    *pTmp;

    if (realm) 
    {
        strcpy(szUserRealm, realm);
        strcpy(szRealm,realm);
    }
    else
    {
        // set realm from ticket file or local default
        GRAB_KERBEROS();
        lKerror = krb_get_tf_realm(tkt_string(), (LPSTR)szRealm);
        if ( lKerror )
            lKerror = krb_get_lrealm((LPSTR)szRealm, 1);
        RELEASE_KERBEROS();
        if (lKerror)
            ErrorMsg(lKerror);
        else
            _fstrcpy(szUserRealm, szRealm);
    }

    // these lines commented out per Doug Hornig (after he left CU);
    // also second parm of krb_get_cred call changed to same as third.
    // 16 May 1996 otherwise if userid has non-null instance, the 
    // dialog box ALWAYS comes up.
    if (szUserInst[0] == 0)
    {
        // use default realm as default instance
        _fstrcpy (szUserInst, szRealm);
    }

    GRAB_KERBEROS();
    lKerror = krb_get_cred((LPSTR)"krbtgt", (LPSTR)szUserInst, 
                           (LPSTR)szUserRealm, lpc);
    RELEASE_KERBEROS();

#if !defined(KLITE)
    if (lKerror == ERROR_TABLE_BASE_krb + RD_AP_TIME) // sync error, try again
    {
        if (TryToResynchronize())
        {
            GRAB_KERBEROS();
            lKerror = krb_get_cred((LPSTR)"krbtgt", (LPSTR)szUserInst, 
                                   (LPSTR)szUserRealm, lpc) ;
            RELEASE_KERBEROS();
        }
    }
#endif

    if (!lKerror)
    {
        bExpired = IsCredExpired(&c); // if so, have credentials expired?
    }
	
    if ((lKerror) || (bExpired == TRUE)) 
    {
        char env[16];
        BOOL prompt;
        
        GetEnvironmentVariable("KERBEROSLOGIN_NEVER_PROMPT",env, sizeof(env));
        prompt = (GetLastError() == ERROR_ENVVAR_NOT_FOUND);

        if ( pLeash_kinit_dlg_ex && prompt ) {
            LSH_DLGINFO_EX dlginfo;

            GRAB_KERBEROS();

            // copy in the existing username and realm
            pTmp = calloc(1, strlen(szUserName) + strlen(szUserInst) + 2);
            strcpy(pTmp, szUserName);
            if (szUserName[0] != 0 && szUserInst[0] != 0)
            {
                strcat(pTmp, "/");
                strcat(pTmp, szUserInst);
            }

            memset(&dlginfo, 0, sizeof(LSH_DLGINFO_EX));
            dlginfo.size = sizeof(LSH_DLGINFO_EX);
            dlginfo.dlgtype = DLGTYPE_PASSWD;
            dlginfo.title = "KClient - Obtain Tickets";
            dlginfo.username = pTmp;
            dlginfo.realm = szRealm;
            dlginfo.use_defaults = 1;

            if (pLeash_kinit_dlg_ex(GetDesktopWindow(), &dlginfo)) {
                // copy out the new username and realm
                char * p = strchr(dlginfo.out.username, '/');
                if ( p ) {
                    *p++ = 0;
                    strncpy(szUserInst, p, INST_SZ);
                    szUserInst[INST_SZ-1] = 0;
                }
                strncpy(szUserName, dlginfo.out.username, ANAME_SZ);
                szUserName[ANAME_SZ-1] = 0;
                strncpy(szRealm, dlginfo.out.realm, REALM_SZ);
                szRealm[REALM_SZ-1] = 0;
            }

            free(pTmp);
            RELEASE_KERBEROS();
        } else 
        // no credentials, or they have expired
        // loop if any error getting tgt (this may change)
        // but user can still cancel out
        while (1)
        {
            // pop the user info dialog box
            bReturn = DialogBox(hInst, MAKEINTRESOURCE(IDD_ENTERPASS), 
                                NULL, UserInfo);
            if (bReturn == FALSE)
            {
                // they pressed cancel, let's bail
                sOSErr = cKrbUserCancelled;
                return (FALSE);
            }

            szUserInst[0] = '\0';
            // get tgt
            ParseName(szUserName, szUserInst, szUserRealm, szNetID);

            // if requested, force Realm to uppercase
            if ( bForceUCRealm )
            {
                _fstrupr(szUserRealm);
                // Replace realm with UCRealm in szNetID (if present)
                // so change password works too this also has the side
                // effect of changing what's in kview.ini.

                // Change case of realm only if present, otherwise,
                // don't tamper
                if (_fstrlen(szNetID) >= (_fstrlen(szUserName) + 
                                          _fstrlen(szUserRealm) + 1)
                    && 0 != strchr( szNetID, '@')
                    && 0 == stricmp(strchr(szNetID,'@')+1, szUserRealm))
                {
                    _fstrcpy(szNetID + 
                             (_fstrlen(szNetID) - _fstrlen(szUserRealm)), 
                             szUserRealm);
                }
            }

            // instance must be NULL for krb_get_pw_in_tkt(), not sure why			
            GRAB_KERBEROS();
            if (pLeash_kinit != NULL)
                {
                pTmp = calloc(1, strlen(szUserName) + strlen(szUserInst) +
                              strlen(szUserRealm) + 3);
                strcpy(pTmp, szUserName);
                if (szUserInst[0] != 0)
                    {
                    if (m_hKrb5DLL != NULL)
                        strcat(pTmp, "/");
                    else
                        strcat(pTmp, ".");
                    strcat(pTmp, szUserInst);
                    }
                 if (szUserRealm[0] != 0)
                    {
                    strcat(pTmp, "@");
                    strcat(pTmp, szUserRealm);
                    }

                lKerror = (*pLeash_kinit)(pTmp, szPass, iLife * 5);
                free(pTmp);
                }
            else
                lKerror = krb_get_pw_in_tkt((LPSTR)szUserName, 
                                            (LPSTR)szUserInst, 
                                            (LPSTR)szUserRealm,
                                            (LPSTR)"krbtgt", 
                                            (LPSTR)szUserRealm, 
                                            iLife, (LPSTR)szPass);
            RELEASE_KERBEROS();

#if !defined(KLITE)
            if (lKerror == RD_AP_TIME || KDC_SERVICE_EXP == lKerror)
            {
                // sync error, try again
                if (TryToResynchronize())
                {
                    GRAB_KERBEROS();
                    if (pLeash_kinit != NULL)
                        lKerror = (*pLeash_kinit)(szUserName, szPass, 
                                                  iLife * 5);
                    else
                        lKerror = krb_get_pw_in_tkt((LPSTR)szUserName, 
                                                    (LPSTR)szUserInst, 
                                                    (LPSTR)szUserRealm, 
                                                    (LPSTR)"krbtgt", 
                                                    (LPSTR)szUserRealm, 
                                                    iLife, (LPSTR)szPass) ;
                    RELEASE_KERBEROS();
                }
            }
#endif // !KLITE
            if (lKerror)
                ErrorMsg (lKerror) ;	
            else
                break; // No error, break out of while() loop.
        }
    }

    return (TRUE);
}


/* ------------------------------------------------------------------------
   GetTicketGrantingTicket() - just like it sounds.	

   Returns:  TRUE on success.  FALSE otherwise
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
GetTicketGrantingTicket(
    void
    )
{
    int tRetCode ;

    LOCK_REENTRY_RETVAL ;
    tRetCode = GTGTPrivate(0) ;
    UNLOCK_REENTRY ;
    return tRetCode ;
}



/* ------------------------------------------------------------------------
   DASPrivate() - destroy all tickets in "ticket store"
   The real one has the reentrancy code around it so it can be
   called by external programs.  This part is here so it can
   be called directly by various other functions here without messing
   up the reentrancy stuff. 

   Returns:  TRUE on success.  FALSE otherwise
   ------------------------------------------------------------------------ */
BOOL
DASPrivate(
    void
    )
{	
    GRAB_KERBEROS();
    lKerror = dest_tkt();     // trash the ticket store
    RELEASE_KERBEROS();
    if (lKerror == 0)         // all went well
        return (TRUE);

    if (lKerror == RET_TKFIL) // no tickets to destroy
        return (TRUE);
    else
    {
        ErrorMsg (lKerror);
        return (FALSE);       // error if we made it this far
    }
}


/* ------------------------------------------------------------------------
   DeleteAllSessions() - destroy all tickets in "ticket store"
   Returns:  TRUE on success.  FALSE otherwise
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
DeleteAllSessions(
    void
    )
{	
    int tRetCode ;

    LOCK_REENTRY_RETVAL ;
    tRetCode = DASPrivate() ;
    UNLOCK_REENTRY ;
    return tRetCode ;
}

/* ------------------------------------------------------------------------
   SetUserName() - destroy all tickets and set user name to specified string.
   Set global DLL variable szNetID.
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
SetUserName(
    LPSTR lpszName
    )
{
    LOCK_REENTRY_RETVAL;
    DASPrivate(); // make sure we get rid all ticket first
	
    if ((_fstrcmp(lpszName, szNetID)) != 0)
        _fstrcpy(szNetID, lpszName) ;

    UNLOCK_REENTRY;
	
    return (TRUE);
}

/* ------------------------------------------------------------------------
   [KC]GetUserName() - used by KView to find out the current NetID, which is 
   saved in a settings file.

   Returns:  TRUE on success.  FALSE if no user name. 	
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT 
#if defined(_WIN32)
KCGetUserName(LPSTR lpszName)
#else
GetUserName(LPSTR lpszName)
#endif
{
    LOCK_REENTRY_RETVAL ;

    lpszName[0] = 0;
    // XXX - we still need to do the right thing for
    //       the case where the app has not filled in szNetID yet
    //       by looking in the credendials cache.
    if (szNetID[0] == 0)
    {
        // no user name yet
        sOSErr = cKrbNotLoggedIn;
        UNLOCK_REENTRY ;
        return (FALSE) ;
    }

    // return current user name
    _fstrcpy (lpszName, (LPSTR)szNetID);

    UNLOCK_REENTRY ;
    return (TRUE) ;
}

	
/* ------------------------------------------------------------------------
   SetKrbdllMode() - enable or disable debug modes on krbv4win.dll
   ------------------------------------------------------------------------ */
void KC_CALLTYPE KC_EXPORT
SetKrbdllMode(
    BOOL bMode
    )
{
    LOCK_REENTRY ;
    GRAB_KERBEROS();

    if (bMode == TRUE)
    {
        set_krb_debug(1);				
        set_krb_ap_req_debug(1);	
    }
    else
    {
        set_krb_debug(0);				
        set_krb_ap_req_debug(0);	
    }

    RELEASE_KERBEROS();
    UNLOCK_REENTRY ;
}
	
/* -------------------------------------------------------------------
   CopyTicket() - 

   pDest - address of buffer into which ticket will be copied
   pKtext - address of KTEXT
   lpdwBytes - address into which ticket length will be copied
   lpszVersion - can be NULL for Mandarin stuff
   bVersion - can be FALSE for Mandarin stuff

   According to PCB, Mandarin servers don't use "SendAuth()" 
   style "messages", only using actual KTEXT.  So pszVersion = NULL, and 
   bVersion = FALSE.  pszVersion and bVersion only exist for
   SendAuth() style support.
   ------------------------------------------------------------------- */
BOOL
CopyTicket(
    LPSTR pDest,
    KTEXT pKtext,
    LPDWORD lpdwBytes,
    LPSTR pszVersion,
    BOOL bVersion)
{
    // DWORD dwLen ;
    DWORD nbytes = 0 ;
    WORD iTest  = 0 ;
	
#ifdef KLITE
    _fmemcpy (pDest, pKtext->dat, pKtext->length) ;
    *lpdwBytes = (DWORD)pKtext->length ;
    return (TRUE) ;
#else // !KLITE
    // first put version info into the buffer 
    if (bVersion == TRUE) 
    {
        _fstrncpy(pDest, KRB_SENDAUTH_VERS, KRB_SENDAUTH_VLEN) ;
        _fstrncpy(pDest+KRB_SENDAUTH_VLEN, pszVersion, KRB_SENDAUTH_VLEN) ;
        nbytes = 2*KRB_SENDAUTH_VLEN ;
    }

    /* Okay, I'm going out of my way _not_ to use the WinSock f(x)
       htonl(), which means this DLL does not need to call the WinSock
       stack, meaning the DLL should still work with Kerberos over LWP
       (or something else).  KClient says:
	
   		dwLen = htonl((unsigned long) pKtext->length) ;
			_fmemcpy(pDest+nbytes, &dwLen, sizeof(dwLen)) ;
			nbytes += sizeof(dwLen) ;

       The first line promotes pKtext->length (an int) to a DWORD and
       changes the byte order, then copies this new "expanded/rotated"
       DWORD value into the ticket buffer.
	
       Instead, I'll achieve the same result by:

       _not_ protmoting pKtext->length to a DWORD, and leaving it an int.
       Copying in an int value of 0x00 (to simulate a DWORD cast)
       Rotating pKtext->length with x_htons(), contained in this DLL
       Well that means this code will only work on Intel hardware so
       instead we'll try using the real htons so that we can support native
       NT binaries.
    */

    iTest = 0x00 ;
    _fmemcpy(pDest+nbytes, &iTest, sizeof(iTest)) ;
    nbytes += sizeof(iTest) ;

    iTest = pKtext->length ;
    // iTest = x_htons (iTest) ;
    iTest = htons (iTest) ;
    _fmemcpy(pDest+nbytes, &iTest, sizeof(iTest)) ;
    nbytes += sizeof(iTest) ;

    // put ticket into buffer 
    _fmemcpy(pDest+nbytes, pKtext->dat, pKtext->length) ;
    nbytes += pKtext->length ;
    
    *lpdwBytes = nbytes ;
    return (TRUE) ;
#endif // !KLITE
}
	
/* -------------------------------------------------------------------
   ParseName() - this will provide some error checking on username
   and service name parsing.  k_isinst() find an instance invalid if it 
   contains the following characters ".,\".  For tgt, our instance is
   CIT.CORNELL.EDU, which will fail; so k_isinst() is not used.  

   If no realm is parsed from pszService, the default realm	is used. 
   ------------------------------------------------------------------- */
BOOL
ParseName(
    LPSTR pszName,
    LPSTR pszInst,
    LPSTR pszRealm,
    LPSTR pszService)
{
    pszRealm[0] = '\0';	// 3 lines, MDO, July 28, 1995
    pszInst[0] = '\0';
    pszName[0] = '\0';

    GRAB_KERBEROS();	
    lKerror = kname_parse(pszName, pszInst, pszRealm, pszService);
    if (lKerror)
        ErrorMsg (lKerror);

    if ((!*pszName) || (!k_isname(pszName)))
    {
        RELEASE_KERBEROS();
        return (FALSE) ;
    }
		
    // cheap realm syntax check
    if (!k_isrealm(pszRealm)) 
        _fstrcpy(pszRealm, szRealm); // give 'em the default realm

    RELEASE_KERBEROS();
    return (TRUE);
}


/* -------------------------------------------------------------------
   SetOSErrFromKError() - set the KClient error code based on the
   MIT Kerberos error code.
		
   this code is replicated from ErrorMsg.  EEEEUUUUUWWWW!
   why?  i wanted to carve out that code as a subroutine so that
   other functions could call it separately, but a) I can't use
   call by reference because this is a C program not a C++ program.
   b) I can't pass in the addresses of the variables because this
   is a DLL, and DS != SS so you can't take addresses of things on
   the stack and expect it to work.  hence this considered-harmful
   thing.
   ------------------------------------------------------------------- */
void
SetOSErrFromKError(
    long lKerror
    )
{	
#if defined(KLITE)
    sOSErr = (OSErr)lKerror;
#else // !KLITE
    // base of the given table in my resources.
    int resourceBase = INT_MAX;
    BOOL bUnknown = FALSE;  // whether error is an 'unknown' code
    int whichTable;         // which of the four tables to use
    // split out the two parts of the error code.
    long tableBase;
    int code;

    // if kerberos forgot to "base" the error, assume a krb error.
    if (0L <= lKerror && lKerror < 256L)
        lKerror |= ERROR_TABLE_BASE_krb;

    if (lKerror >= 10000L && lKerror <= 11004)
    {
        tableBase = 0;
        code = (int)(lKerror - 10000L);
    }
    else
    {
        tableBase = lKerror & 0xffffff00lu;
        code = (int)(lKerror & 0x000000fflu);
    }

    // find out what kind of error we have.		
    for (whichTable = 0; whichTable <= 3; whichTable++)
    {
        if (tableBase == errorTableMap[whichTable][0])
        {
            resourceBase = (int)(errorTableMap[whichTable][1]);
            break;
        }
    }

    if (resourceBase == KRB_ERR_RESOURCE_BASE)
    {
        sOSErr = cKrbKerberosErrBlock - code ;
    }
    else if (resourceBase != INT_MAX)
        sOSErr = cKrbKerberosErrBlock ;
#endif // !KLITE
}


/* -------------------------------------------------------------------
   ErrorMsg () - MessageBox with Kerberos error text.
	
   lKerror - krbv4win.dll error number
   such errors are four bytes long.  the top three bytes is 
   a value which identifies *what kind of* error we're dealing
   with.  although it looks arbitrary, the value is really 
   the name of the table encoded as six-bit characters
   (see error_table_name() in the kerberos source).  the
   low-order byte is the error number.  our problem is to get
   at the appropriate error string, and since the version of
   kerberos code i have doesn't expose the string tables hard-
   coded into the dll, i moved the strings into my resources.
   sigh.
   ------------------------------------------------------------------- */
void
ErrorMsg(
    long tError
    )
{	
    // wsprintf (szTemp, (LPSTR)get_krb_err_txt_entry(lKerror)) ;	
    // MessageBox (NULL, (LPSTR)szTemp, lpszMsgBoxTitle, MB_OK) ;

    // base of the given table in my resources.
    int resourceBase = INT_MAX;
    BOOL bUnknown = FALSE; // whether error is an 'unknown' code
    int whichTable;        // which of the four tables to use
    int iResourceLength;   // how big the loaded string is.
    int theID;             // a string id for one of the loads
    char * tBuffer;        // pointer into szTemp so can LoadString.
    // split out the two parts of the error code.
    long tableBase;
    int code,showCode;
#if defined(_WIN32)
    long lMyRet;
#endif


#if !defined(KLITE)
    // this table maps kerberos error number bases to 
    // bases in the resources of this dll.
    static long errorTableMap [4] [2] = 
    {
        0L, COM_ERR_RESOURCE_BASE,
        ERROR_TABLE_BASE_kadm, KADM_ERR_RESOURCE_BASE,
        ERROR_TABLE_BASE_krb,  KRB_ERR_RESOURCE_BASE,
        ERROR_TABLE_BASE_lsh,  LSH_ERR_RESOURCE_BASE
    };
#endif // !KLITE

    resourceBase = INT_MAX;
    bUnknown = FALSE;

#if !defined(KLITE)
    // if kerberos forgot to "base" the error, assume a krb error.
    if (0L <= lKerror && lKerror < 256L)
        lKerror |= ERROR_TABLE_BASE_krb;

    if (lKerror >= 10000L && lKerror <= 11004)
    {
        tableBase = 0;
        code = (int)(lKerror - 10000L);
        showCode = (int)lKerror;
    }
    else
#endif // !KLITE
    {
        tableBase = lKerror & 0xffffff00lu;
        code = (int)(lKerror & 0x000000fflu);
        showCode = code;
    }

#if !defined(KLITE)
    // find out what kind of error we have.		
    for (whichTable = 0; whichTable <= 3; whichTable++)
    {
        if (tableBase == errorTableMap[whichTable][0])
        {
            resourceBase = (int)(errorTableMap[whichTable][1]);
            break;
        }
    }
#endif // !KLITE
		
    if (resourceBase == INT_MAX)
        bUnknown = TRUE;
    else if (resourceBase == KRB_ERR_RESOURCE_BASE)
    {
        sOSErr = cKrbKerberosErrBlock - code ;
    }
    else
        sOSErr = cKrbKerberosErrBlock ;

    // load either the table name or "Unknown"			
    theID = bUnknown ? IDS_UNKNOWN : TABLE_NAME_BASE + whichTable;

    iResourceLength = LoadString(hInst,theID,szTemp,255);
#if defined(_WIN32)
    if (iResourceLength == 0)
        lMyRet = GetLastError();
#endif
    tBuffer = szTemp + iResourceLength;

    if (bUnknown)
    {
        // add the hex value of tableBase, plus a close paren
        _ltoa(tableBase,tBuffer,16);
        _fstrcat(tBuffer,")");
        tBuffer += _fstrlen(tBuffer);
    }

    // say " error "		                            
    iResourceLength = LoadString(hInst,IDS_SAYERROR,tBuffer,255);
    tBuffer += iResourceLength;

    // and the individual error number
    _itoa(showCode,tBuffer,10);
    _fstrcat(tBuffer,": ");
    tBuffer += _fstrlen(tBuffer);

    // load the actual error text. if the number is unknown,
    // this one might not work, even if no mem shortage etc.

    if (!bUnknown)
    {
        iResourceLength = LoadString(hInst, resourceBase+code, tBuffer, 255);
        // there wasn't any, so add standard disclaimer
        if (iResourceLength == 0)
            LoadString(hInst, IDS_NOMESSAGE, tBuffer, 255);
    }

    // now put it out on the screen		
    MessageBox (NULL, (LPSTR)szTemp, lpszMsgBoxTitle,
                MB_OK | MB_ICONEXCLAMATION);
}

/* ------------------------------------------------------------------------
   UserInfo - dialog to get ID and Password.

   DialogBox() was called for this dialog with the parent hWnd = NULL,
   because I did not want to require the calling application to pass
   its hWnd to GetTicketForService().  A parent hWnd of NULL will
   cause the dialog to appear in the upper left corner of the desktop
   window.  I can get around this by "manually" centering the dialog
   (using MoveWindow() on the dialog window).
   ------------------------------------------------------------------------ */
BOOL CALLBACK KC_EXPORT
UserInfo(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    HWND hEdit, hPrev, hStatic;
    PAINTSTRUCT	ps;

    switch (message)
    {
    case WM_INITDIALOG:
        // center the dialog on the desktop window
        GetWindowRect(GetDesktopWindow(), &r_desk) ;
        GetWindowRect(hDlg, &r_dlg) ;
        MoveWindow(hDlg, 
                   ((r_desk.right-r_desk.left)/2)-((r_dlg.right-r_dlg.left)/2),
                   ((r_desk.bottom-r_desk.top)/2)-((r_dlg.bottom-r_dlg.top)/2),
                   (r_dlg.right-r_dlg.left), 
                   (r_dlg.bottom-r_dlg.top),
                   TRUE);

        // Make sure there is a fully qualified szNetID, if possible
        if (szNetID[0]) {
            // Use whatever we have and try to fully quality it
            ParseName(szUserName, szUserInst, szUserRealm, szNetID);
        } else {
            // Use the ticket file if we can
            GRAB_KERBEROS();
            lKerror = krb_get_tf_fullname(tkt_string(), szUserName, szUserInst,
                                          szUserRealm);
            RELEASE_KERBEROS();
            if (lKerror) {
                szUserName[0] = 0;
                szUserInst[0] = 0;
                szUserRealm[0] = 0;
            }
        }
        if (szUserName[0]) {
            strcpy(szNetID, szUserName);
            if (szUserInst[0]) {
                strcat(szNetID, ".");
                strcat(szNetID, szUserInst);
            }
            if (szUserRealm[0])  {
                strcat(szNetID, "@");
                strcat(szNetID, szUserRealm);
            } else {
                // set realm from ticket file or local default
                GRAB_KERBEROS();
                lKerror = krb_get_tf_realm(tkt_string(), (LPSTR)szRealm);
                if (lKerror)
                    lKerror = krb_get_lrealm((LPSTR)szRealm, 1);
                RELEASE_KERBEROS();
                if (!lKerror) {
                    strcat(szNetID, "@");
                    strcat(szNetID, szRealm);
                }
            }
        }

        // NetID
        SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, sizeof(szNetID), 0);
        SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETTEXT, 0, 
                           (LPARAM)(LPCSTR)szNetID) ;

        // UCRealm checkbox
        SendDlgItemMessage(hDlg, IDC_REALM_CHECK, BM_SETCHECK, 
                           bForceUCRealm ? BST_CHECKED : BST_UNCHECKED , 0);

        // password
        SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, sizeof(szPass), 0);

        if (szNetID[0]) // is there a NetID?
        {
            hEdit = GetDlgItem (hDlg, IDC_EDIT2);
            hPrev = SetFocus (hEdit);
            return (FALSE);
            // Note: must return FALSE, or Windows will override, 
        }
        // and set focus to first dialog control in tab order.
        return (TRUE);

    case WM_COMMAND:
        if (wParam == IDOK) 
        {
            // NetID
            SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, sizeof(szNetID),
                               (LPARAM)(LPCSTR)szNetID);
            // Password
            SendDlgItemMessage(hDlg, IDC_EDIT2, WM_GETTEXT, sizeof(szPass),
                               (LPARAM)(LPCSTR)szPass);
            // UCRealm checkbox
            bForceUCRealm = BST_CHECKED == 
                SendDlgItemMessage(hDlg, IDC_REALM_CHECK, BM_GETCHECK, 0, 0);

            if (!szNetID[0]) {
                MessageBox(hDlg,
                           "You are not allowd to enter a blank network ID.",
                           "Invalid Network ID",
                           MB_OK | MB_ICONSTOP);
            } else if (!szPass[0]) {
                MessageBox(hDlg, 
                           "You are not allowed to enter a blank password.",
                           "Invalid Password",
                           MB_OK | MB_ICONSTOP);
            } else {
                EndDialog(hDlg, TRUE);
            }
            return (TRUE);
        }
        if (wParam == IDCANCEL)
        {
            EndDialog(hDlg, FALSE);
            return (TRUE);
        }
        break;

    case WM_PAINT:
        BeginPaint(hDlg, &ps); // this validates the entire About box
        EndPaint (hDlg, &ps);
        if (bKurt == TRUE) // it's a long story (see KurtsSplashScree())
        {
            hStatic = GetDlgItem (hDlg, 901);
            if (hStatic != NULL)
                KurtsSplashScreen(hStatic); // this puts up our credit bitmap
        }
        return (TRUE);
    }

    return (FALSE);
}

/* ------------------------------------------------------------------------
   IsCredExpired - see if credentials have expired.

   pc - pointer to credentials

   Return TRUE if expired.  FALSE if _not_ expired.  

   A krbdll.dll problem:  gettimeofday() is declared as near, but 
   lives in krbdll.dll (and needs to be FAR).  So, I'm getting local 
   time by using C runtime f(x) time().  Note:  I'm leaving out KClient Mac's 
   CLOCK_SKEW (here's their explanation):

   // Note: twice CLOCK_SKEW was added to age of ticket so that we could 
   // be more sure that the ticket was good.
   if ((tp.tv_sec - c->issue_date + (2*CLOCK_SKEW)) > (5 * 60 * c->lifetime))
   ------------------------------------------------------------------------ */
BOOL
IsCredExpired(
    CREDENTIALS *pc
    )
{
    if(krb_check_serv(pc->service)){
        return(TRUE); /* this ticket is no good */
    }
    return(FALSE); /* no we have not expired */
}

/* ------------------------------------------------------------------------
   x_htons() - cheap WinSock htons() relacement.  This keeps our DLL from 
   needing to link to winsock.lib, just to call one WinSock function 
   I really ought to do a 	x_htonl() as well.  
   ------------------------------------------------------------------------ */

/*
int x_htons(int iVal)
{
    WORD iReturn ;
    WORD tVal ;
    tVal = iVal;
    _asm
    {
        mov  ax,tVal
        xchg al,ah
        mov  iReturn, ax
    }
    return ((int)iReturn);
}
*/

/* ------------------------------------------------------------------------
   ListTickets () - a cheap call to lsh_klist(), allowing the calling app
   to avoid using kerberos header files and TICKETINFO structs, making for
   somewhat easier kerberization.
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
ListTickets(
    HWND hListBox
    )
{
    LOCK_REENTRY_RETVAL;
    GRAB_KERBEROS();

    lsh_klist (hListBox, (TICKETINFO FAR *)&ti) ;

    RELEASE_KERBEROS();

    UNLOCK_REENTRY ;	
    return (TRUE) ;
}

/* ------------------------------------------------------------------------
   SetTicketLifetime () - set global variable that stores life time.  

   iNewLifeTime - MIT lifetime units

   Note:  MIT's lifetime "units" are in 5 minute intervals.  That is,
   a life time	of 96 would be 480 minutes (8 hours).
   ------------------------------------------------------------------------ */
void KC_CALLTYPE KC_EXPORT
SetTicketLifeTime(
    int iNewLifeTime
    )
{
    LOCK_REENTRY;

    iLife = iNewLifeTime ;
    krb_set_lifetime(iNewLifeTime); // Do we want to do this? pbh

    UNLOCK_REENTRY ;
}

/* ------------------------------------------------------------------------
   TgtExist () - got any TGTs? Return TRUE if yes, FALSE otherwise.
	
   this function deliberately avoids displaying Kerberos errors, because
   given what it does it is likely to be called very often (for example,
   Launch Pad calls it every 5 seconds) and the errors would show repeatedly.
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
TgtExist(
    void
    )
{               
    LOCK_REENTRY_RETVAL;
		
    // see if there is already a realm
    if (szUserRealm[0] == 0) // if not, get default realm
    {
        GRAB_KERBEROS();	
        lKerror = krb_get_tf_realm(tkt_string(), (LPSTR)szRealm);
        if ( lKerror )
            lKerror = krb_get_lrealm((LPSTR)szRealm, 1);
        RELEASE_KERBEROS();	

        if (lKerror) // error getting local realm
        {
            SetOSErrFromKError(lKerror) ;
            UNLOCK_REENTRY ;
            return (FALSE) ;
        }
        else
            _fstrcpy (szUserRealm, szRealm) ;
    }

    // don't use same Inst buffer -- somebody else might be using it!
    if (szUserInst[0] == 0)
        _fstrcpy (szxUserInst, szRealm); // use default realm as default instance
    else
        _fstrcpy (szxUserInst, szUserInst);
	
    // already have a tgt?
    GRAB_KERBEROS();
    lKerror = krb_get_cred((LPSTR)"krbtgt", (LPSTR)szxUserInst, 
                           (LPSTR)szUserRealm, lpxc);
    RELEASE_KERBEROS();
    if (!lKerror)
    {
        if (IsCredExpired(&xc) == FALSE) // if so, have credentials expired?
        {
            UNLOCK_REENTRY;
            return (TRUE); // we got a tgt and its live
        }
        else
        {
            UNLOCK_REENTRY;
            return (FALSE); // got a tgt, but its too old
        }
    }
    else
    {
        SetOSErrFromKError(lKerror);
        UNLOCK_REENTRY;
        return (FALSE);     // no tgt or error	
    }
}
	
	
// the following two functions will exist ONLY until the other functions
// are revised to return OSErr themselves.  this minimizes the API
// change for this release.

/* ------------------------------------------------------------------------
   KClientKerberosErrno () - returns most recent Kerberos error code
   ------------------------------------------------------------------------ */
signed long KC_CALLTYPE KC_EXPORT
KClientKerberosErrno(
    void
    )
{
    return lKerror ;
}

/* ------------------------------------------------------------------------
   KClientErrno () - returns most recent KClient error code
   ------------------------------------------------------------------------ */
OSErr KC_CALLTYPE KC_EXPORT
KClientErrno(
    void
    )
{
    return sOSErr ;
}

/* ------------------------------------------------------------------------
   ChangePassword - just like it sounds.  There are a few lines 
   which are needed due to the way Leash and lsh_changepwd() were
   made at MIT. 
   ------------------------------------------------------------------------ */
BOOL KC_CALLTYPE KC_EXPORT
ChangePassword(
    void
    )
{	
    HANDLE hKadm_info; // lsh_changepwd() needs this

    LOCK_REENTRY_RETVAL;

    hKadm_info = 0; // Leash specific error handling weirdism!!!
    DASPrivate();   // hose out their tickets

    if (GTGTPrivate(0) == TRUE) // get a TGT
    {

        if ( pLeash_changepwd_dlg_ex ) {
            LSH_DLGINFO_EX dlginfo;
            char    *pTmp;

            GRAB_KERBEROS();

            // copy in the existing username and realm
            pTmp = calloc(1, strlen(szUserName) + strlen(szUserInst) + 2);
            strcpy(pTmp, szUserName);
            if (szUserInst[0] != 0)
            {
                strcat(pTmp, "/");
                strcat(pTmp, szUserInst);
            }

            memset(&dlginfo, 0, sizeof(LSH_DLGINFO_EX));
            dlginfo.size = sizeof(LSH_DLGINFO_EX);
            dlginfo.dlgtype = DLGTYPE_CHPASSWD;
            dlginfo.title = "KClient - Change Password";
            dlginfo.username = pTmp;
            dlginfo.realm = szRealm;
            dlginfo.use_defaults = 1;

            if (pLeash_changepwd_dlg_ex(NULL, &dlginfo)) {
                RELEASE_KERBEROS();
                free(pTmp);
                // success!!!
                DASPrivate();
                // hose all tickets
                UNLOCK_REENTRY;
                return (TRUE) ;
            }
            free(pTmp);
            RELEASE_KERBEROS();
        } else {
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_CHANGEPASS), 
                           NULL, ChangePasswordDialog) == TRUE)
            {
                // actually change the kerberos password
                GRAB_KERBEROS();	
                lKerror = lsh_changepwd((LPSTR)szNetID, (LPSTR)szPass, 
                                         (LPSTR)szNewPass, (LPSTR)&hKadm_info);
                RELEASE_KERBEROS();	
                if (hKadm_info != 0)
                {
                    // It's a long story.
                    // we have to do this due to the way Leash is made.
                    GlobalUnlock (hKadm_info);
                    GlobalFree (hKadm_info) ;
                }
                if (lKerror)
                {
                    // in case it bombed in krbv4win.dll
                    // XXX - what error is this???
                    if (lKerror == 11004)
                    {
                        SetOSErrFromKError(lKerror);
                        LoadString(hInst, IDS_PASSDOMAIN, szTemp, 255);
                        MessageBox(NULL, (LPSTR)szTemp, (LPSTR)lpszMsgBoxTitle,
                                    MB_OK | MB_ICONEXCLAMATION);
                    }
                    else
                        ErrorMsg (lKerror) ;
                    PassError (IDS_PASSNOTCHANGED);
                    UNLOCK_REENTRY;
                    return (FALSE);
                }
                // success!!!
                DASPrivate();
                // hose all tickets
                MessageBox(NULL, (LPSTR)"Your password has been changed.", 
                            (LPSTR)"Password", MB_OK | MB_ICONINFORMATION);
                // erase passwords
                strset(szPass, '\0');
                strset(szNewPass, '\0');
                strset(szConfirmPass, '\0');
                UNLOCK_REENTRY;
                return (TRUE) ;
            }
        }
    }

    // error if we made it this far		
    // Gets rid of TGT if they bail
    DASPrivate();
    // out of the change password dialog
    PassError(IDS_PASSNOTCHANGED);
    UNLOCK_REENTRY ;
    return (FALSE) ;
}

/* ------------------------------------------------------------------------
   Dialog procedure for Change Password dialog.  
   ------------------------------------------------------------------------ */
BOOL CALLBACK KC_EXPORT
ChangePasswordDialog(
    HWND hDlg, 
    UINT message, 
    WPARAM wParam,
    LPARAM lParam
    )
{
    HWND hPass, hConfirmPass ;
    BOOL bHasDigit, bHasLetter ;

    switch (message)
    {
    case WM_INITDIALOG:
	// center the dialog on the desktop window
	GetWindowRect (GetDesktopWindow(), &r_desk) ;
	GetWindowRect (hDlg, &r_dlg) ;
	MoveWindow(hDlg, 
		   ((r_desk.right-r_desk.left)/2)-((r_dlg.right-r_dlg.left)/2),
		   ((r_desk.bottom-r_desk.top)/2)-((r_dlg.bottom-r_dlg.top)/2),
		   (r_dlg.right-r_dlg.left), 
		   (r_dlg.bottom-r_dlg.top),
		   TRUE);

	// set max. length of new and confirmed passwords
	SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 
			   sizeof(szNewPass), 0);
	SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, 
			   sizeof(szConfirmPass), 0);

	// zap change password buffers
	*szNewPass = '\0';
	*szConfirmPass = '\0';

	return (TRUE);

    case WM_COMMAND:
	if (wParam == IDOK) 
	{
	    // New and confirmed passwords
	    SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, 
			       sizeof(szNewPass), (LPARAM)(LPCSTR)szNewPass);
	    SendDlgItemMessage(hDlg, IDC_EDIT2, WM_GETTEXT, 
			       sizeof(szConfirmPass), 
			       (LPARAM)(LPCSTR)szConfirmPass);

	    // we will need these in case there is an error, to set the focus
	    hPass = GetDlgItem(hDlg, IDC_EDIT1);
	    hConfirmPass = GetDlgItem(hDlg, IDC_EDIT2);

	    // make sure they confirmed the new password
	    if (szConfirmPass[0] == 0)
	    {
		PassError(IDS_PASSNOTCONFIRMED);
		SetFocus(hConfirmPass);
		return (TRUE);
	    }

	    // validate length
	    if (strlen(szNewPass) < MINPASSLENGTH)
	    {
		PassError(IDS_PASSTOOSHORT);
		SetFocus (hPass) ;
		// select contents of new password edit control
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, 
				   MAKELONG(0, -1));
		return (TRUE);
	    }

	    // loop and check for at least 1 digit and 1 letter
	    bHasDigit = FALSE;   // initialize
	    bHasLetter = FALSE;
	    p1 = szNewPass;      // p1 is global (no near local variables 
	                         // in DLLs; SS!=DS)
	    while (*p1 != '\0')
	    {
		if (isdigit(*p1))
		    bHasDigit = TRUE ;

		if (isalpha(*p1))
		    bHasLetter = TRUE ;

		p1++ ;
	    }

	    // no digit
	    if (bHasDigit == FALSE)
	    {
		PassError(IDS_PASSNODIGIT);
		SetFocus(hPass);
		// select contents of new password edit control
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, 
				   MAKELONG(0, -1));
		return (TRUE);
	    }

	    // no letter
	    if (bHasLetter == FALSE)
	    {
		PassError (IDS_PASSNOLETTER) ;
		SetFocus (hPass) ;
		// select contents of new password edit control
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, 
				   MAKELONG(0, -1));
		return (TRUE);
	    }

	    // compare new and confirmed password
	    if (strcmp(szNewPass, szConfirmPass)!= 0)
	    {
		PassError(IDS_PASSNOMATCH);
		SetFocus(hConfirmPass);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_SETSEL, 0, 
				   MAKELONG(0, -1)) ;
		return (TRUE);
	    }

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
   So I can use a string table, and bypass Leash error messages
   if needed.
   ------------------------------------------------------------------------ */
BOOL
PassError(
    int iError
    )
{
    LoadString(hInst, iError, (LPSTR)szPassErrorText, MAXPASSERROR - 1);
    MessageBox(NULL, (LPSTR)szPassErrorText, (LPSTR)"Password",
	       MB_OK | MB_ICONINFORMATION
#ifdef _WIN32  // pbh, I forgot this was win32 specific :(
	       | MB_TOPMOST | MB_SETFOREGROUND
#endif // _WIN32
	);
    return (TRUE) ;
}
	
/* ------------------------------------------------------------------------
   If the clock is not synchronized when I try to get a service ticket, 
   find KView and tell it to resynchronize.
   (hModules is a single array because it has to be passed to the
   enum proc; use: 0) handle of kview if out there  1) handle of kview32
   if out there  2) module handle of current window in enum  3) handle
   of "found" window if any.  )
   ------------------------------------------------------------------------ */
BOOL CALLBACK
FindResyncher(
    HWND hWnd,
    LPARAM lParam
    )
{
    GetClassName(hWnd,szClassName,CLASSNAMEBUFSIZE);

    if (0 == _fstricmp("MandKViewClass",szClassName))
    {
	*((HWND*)lParam) = hWnd;
	return 0;
    }

    return 1;
}


BOOL 
TryToResynchronize(
    )
{
    hSinkerWindow = 0;

#if defined(_WIN32)		
    EnumWindows(MakeProcInstance(FindResyncher, hInst), 
		(LPARAM)&hSinkerWindow);
#else
    EnumWindows(MakeProcInstance(FindResyncher, hInst),
		(LPARAM)(WORD)&hSinkerWindow);
#endif

    if (hSinkerWindow != 0)
    {
	// that is now a handle to the window we seek
	// remember, zero from WM_COMMAND means "I handled this message"
	// which is what I want.  The '2' way out there is so it can tell
	// that the message wasn't straight from the menu, so maybe it
	// should ask the user before synching.
	if (!SendMessage((HWND)hSinkerWindow, msgAutoSync, 0, 0))
	    return TRUE;
    }
    else
    {
	strcpy(szTemp, KVIEWNAME);
	strcat(szTemp, " /sync /quit");		
	WinExec(szTemp, SW_HIDE);
    }
    return FALSE;
}



void 
myGrabMutex(
    ACritical *theCritical
    )
{

#if defined(_WIN32)
    WaitForSingleObject(theCritical->hMutex, INFINITE);
#else // !_WIN32
    BOOL far * pByte;
    WORD mySeg;
    WORD myOff;

    /* $$$ okay, for Win16, we have a single set of global variables, 
       so just set it raw in assembler */

#if defined(_DEBUG)
    if (theCritical->whichOne == KERB4CRITICAL)
	MessageBox(NULL, "Kerb4Critical", "KClient", 
		   MB_OK | MB_ICONINFORMATION);
#endif

    if (theCritical->whichOne == KCLIENTCRITICAL)
    {
#if defined(_DEBUG)
	MessageBox(NULL, "KClientCritical", "KClient",
		   MB_OK | MB_ICONINFORMATION);
#endif
	// the code below and in cussp_end_critical 
	// is from Microsoft Knowledge Base Q84053
	// 'How a TSR Can Serialize Access to Its Data'
	// in this code it will only be used when compiled for 16-bit windows
	pByte = &bInUse;
	mySeg = _FP_SEG(pByte);
	myOff = _FP_OFF(pByte);
	__asm
        {
	    mov  cx,es
	    mov  es,[mySeg]
	    mov  bx,[myOff]
	  Wait_On_Sem:
	    mov  al,1
	    xchg al,BYTE PTR es:[bx]
	    test al,0ffh
	    je   short Got_Access
	    mov  ax,1680h
	    int  2fh
	    jmp short Wait_On_Sem
	  Got_Access:
            mov  es,cx
        }
    }
#endif // !_WIN32
}


void
myLetGoMutex(
    ACritical *theCritical
    )
{

#if defined(_WIN32)
    ReleaseMutex(theCritical->hMutex);
#else // !_WIN32
    BOOL far * pByte;
    WORD mySeg;
    WORD myOff;

    /* $$$ okay, for Win16, we have a single set of global variables, 
       so just set it raw in assembler */

    if (theCritical->whichOne == KCLIENTCRITICAL)
    {
        pByte = &bInUse;
        mySeg = _FP_SEG(pByte);
        myOff = _FP_OFF(pByte);
        __asm
        {
            mov  cx,es
            mov  es,[mySeg]
            mov  bx,[myOff]
            xor al,al
            xchg al,BYTE PTR es:[bx]
            mov  es,cx
        }
    }
#endif // !_WIN32
}


// figure out which platform we're running under.
void
DeterminePlatform(
    void
    )
{
#ifndef WF_WINNT
#define WF_WINNT 0x4000
#endif

#if defined(_WIN32)
    myInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&myInfo);
#else // !_WIN32
    WORD oldVersion = (WORD)(((GetVersion() << 8) & 0x0000ff00) | 
                             ((GetVersion() >> 8) & 0x000000ff));

    if (0 != (GetWinFlags() & WF_WINNT))
        myInfo.dwPlatformId = VER_PLATFORM_WIN32_NT;
    else if (oldVersion >  0x030A)
        myInfo.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS; // Windows 95
    else
    {
        /* for now, treat Win32s and Win16 as the same */
        myInfo.dwPlatformId = VER_PLATFORM_WIN16;

        /* UINT iEM = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
           HINSTANCE hWin32sys = ::LoadLibrary("W32SYS.DLL");
           ::SetErrorMode(iEM);
           if (hWin32sys > HINSTANCE_ERROR)
           {
           _fstrcpy(theString,"w32s");
           ::FreeLibrary(hWin32sys);
           }
           else
           _fstrcpy(theString,"w16"); */
    }
#endif // !_WIN32
}


#if 1
/* ------------------------------------------------------------------------
   By special request, UMich needs the ability to add a bitmap to the 
   password dialog box.  CU doesn't need a bitmap, so rather than
   always BitBlt() a .bmp where we don't need one, what follows is a
   way for someone to "ResEdit" one in, without rebuilding the project.

   This routine is just for them, and only get's called if the global 
   varaiable bKurt is TRUE (see the WM_PAINT case in UserInfo()).  
   bKurt is TRUE if the string resource ID 900 is set to "LoadBitmap" 
   (no quotes used in the actual string table entry, and this is 
   case sensitive), otherwise bKurt is FALSE.  See the LoadResource() 
   line in LibMain() which does this.  

   This function insists that a frame be added to the password dialog 
   (via App Studio or equivalent) with an ID of 901.  The actual bitmap
   being displayed must have an ID of 902.  

   In summary, the steps to make a bitmap appear in the password dialog are:
   1) Open App Studio and directly edit kclient.dll
   2) Add a frame with an ID of 901	to dialog 102 (the password dialog)
   3) Add your bitmap and give it an ID of 902
   4) Edit string table entry 900 to read "LoadBitmap"

   Note: this only works with the Password dialog, _not_ the 
   Change Password dialog.
   ------------------------------------------------------------------------ */
void
KurtsSplashScreen(
    HANDLE hCtrl
    )
{
    HBITMAP hBm;
    PAINTSTRUCT	ps;
    BITMAP bm;
    HDC hdc, hdcMem;

    InvalidateRect(hCtrl, NULL, TRUE); // invalidate the static control
    // load the bitmap as a resource
    hBm = LoadBitmap (hInst, MAKEINTRESOURCE(902));
    hdc = BeginPaint(hCtrl, &ps);
    // make a compatible DC
    hdcMem = CreateCompatibleDC(hdc);
    // select in the bitmap
    SelectObject (hdcMem, hBm);
    // get bitmap stats
    GetObject (hBm, sizeof(BITMAP), (LPSTR)&bm);
    // Blit it
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
    // clean up
    DeleteDC(hdcMem);
    DeleteObject (hBm) ;
    EndPaint (hCtrl, &ps) ;
}
#endif // 1


#ifdef WINSOCK

BOOL KC_CALLTYPE KC_EXPORT
SendTicketForService(
    LPSTR service, 
    LPSTR version,
    int fd)
{
    BOOL res;
    char srv[ANAME_SZ], inst[INST_SZ], rlm[REALM_SZ];
    char cRet[4];
    KTEXT_ST ticket;
    int len = sizeof(cRet);

    res = 0;

    // added 12-16-97, supplied by Peter Tirrell of Brown
    if (!TgtExist()){
        if (!GetTicketForService(service, (LPSTR) &ticket.dat,
                                 (LPDWORD) &ticket.length))
        {
            return (res);
        }
    }

    GRAB_KERBEROS();

    if(SOCKET_ERROR == getsockopt(fd, SOL_SOCKET, SO_ERROR, cRet, &len))
    {
        // good chance it is 10038, not a socket
        lKerror = WSAGetLastError();
		if( lKerror != 10038 )
		{
            ErrorMsg(lKerror);
		}
        RELEASE_KERBEROS();
        return (res);
    }

    if (kname_parse(srv, inst, rlm, service))
    {
        RELEASE_KERBEROS();
        return (res);
    }

    lKerror = krb_sendauth(0L,
                           fd,
                           &ticket,
                           srv,
                           inst,
                           rlm,
                           0L,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           version);

    if(lKerror){
        ErrorMsg(lKerror);
    } else{
        res = 1;
    }

    RELEASE_KERBEROS();
    return (res);
}

#endif // WINSOCK


#ifdef _WIN32
// _KCGetNumInUse:
//
// - returns the number of instances of the dll currently in use
//
// - _KCGetNumInUse is used at CMU.  It was their invention.  Their
//   current Kstatus.exe uses it to keep Kstatus from exiting while any
//   application that loads Kclnt32.dll is active.

DWORD KC_CALLTYPE KC_EXPORT _KCGetNumInUse (void)
{	
    DWORD ret;
	
    LOCK_REENTRY_RETVAL ;
	
    GRAB_KERBEROS();
    ret = inUseCounter?*inUseCounter:0;
    RELEASE_KERBEROS();
	
    UNLOCK_REENTRY ;
    return(ret);
}
#endif
