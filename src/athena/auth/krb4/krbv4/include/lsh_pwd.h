/* LSH_PWD.H this is the include file for the LSH_PWD.C  */

/* Included from krb.h - CRS 940805 */

#ifndef __LSH_PWD__
#define __LSH_PWD__

// Definition of the info structure that is passed to tell the dialog box what state it
//  should be in.

#ifdef _WIN32
#define EXPORT
#else
#define EXPORT _export
#endif

#include <stdio.h>
#include <stdlib.h>

#include <leashwin.h>

// Some defines swiped from leash.h
//  These are necessary but they must be kept sync'ed with leash.h
#define HELPFILE "kerberos.hlp"

#define DLGHT(ht) (HIWORD(GetDialogBaseUnits())*(ht)/8)
#define DLGWD(wd) (LOWORD(GetDialogBaseUnits())*(wd)/4)

// external variables
#ifdef PDLL
long lsh_errno;
char *err_context;       /* error context */
char FAR *kadm_info; /* to get info from the kadm* files */
long dlgu;                              /* dialog units  */
#ifdef WINSOCK
HINSTANCE hinstWinSock = NULL;
#endif // WINSOCK
#endif // PDLL

extern char KRB_HelpFile[_MAX_PATH];

// local macros  stolen from leash.h
#ifndef MAKEWORD
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((UINT)((BYTE)(high))) << 8)))
#endif /*MAKEWORD*/


// Function Prototypes.
int FAR PASCAL EXPORT Lsh_Enter_Password_Dialog(HWND hParent, LPLSH_DLGINFO lpdlginfo);
int FAR PASCAL EXPORT Lsh_Change_Password_Dialog(HWND hParent, LPLSH_DLGINFO lpdlginfo);
int lsh_com_err_proc (LPSTR whoami, long code, LPSTR fmt, va_list args);
int EXPORT DoNiftyErrorReport(long errnum, LPSTR what);
LONG FAR PASCAL EXPORT MITPwdWinProcDLL(HWND hWnd, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL EXPORT PasswordProcDLL(HWND hDialog, WORD message, WORD wParam, LONG lParam);
LONG FAR PASCAL EXPORT lsh_get_lsh_errno( LONG FAR *err_val);
LONG Leash_timesync(int);
#endif /* __LSH_PWD__ */
