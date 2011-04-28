/*
Details on building kclear.exe
-------------------------------
Project type: Windows application (exe)
Memory model: small
Segment setup: SS==DS *
Linker input libraries:  winsock

Files in project:
	kclear.c
	kclear.def
	kclear.rc
	kclient.lib
	
Tab stops are set to 3 spaces for EZ reading.

Copyright 1995 by Cornell University

*/

#include <windows.h>
#include "winsock.h"

#include "kclient.h"
#include "res_kc.h"

#include <string.h>
#include <stdlib.h>

/* ------------------------------ Symbolic constants -------------------- */

/* ------------------------------ Function Prototypes -------------------- */

int	PASCAL WinMain (HANDLE, HANDLE, LPSTR, int) ;
long	FAR PASCAL ClearingDlgProc (HWND, UINT, WPARAM, LPARAM) ;
/* ----------------------------- Global Variables ------------------------ */
HANDLE hInst ;			// all purpose instance
HWND hWnd;				// window handle for popup message window
LPSTR lpszApp = "KClear" ;				// applicaton name


/* -------------------------------------------------------------------
	SeeIfClearPassword -- should we just clear password and return?
	if the command line contains "/clear" or "-clear" or "\clear",
	then remove that token and return TRUE; otherwise return FALSE
	------------------------------------------------------------------- */
int SeeIfClearPassword(LPSTR lpCmdLine)
{
	char far * theParm;
	
	theParm = lpCmdLine;
	
	while (TRUE)
	{
	//	int thePos = _fstrcspn(theParm,"-\\/");
		int thePos = strcspn(theParm,"-\\/");
		if (theParm[thePos] == '\0')
			return FALSE;
//		if (0 == _fstrnicmp(&theParm[1],"clear",5) )
		if (0 == strnicmp(&theParm[1],"clear",5) )
		{
//			int j = _fstrlen(theParm);
			int j = strlen(theParm);
			if (j <= 6)
				theParm[0] = '\0';
			else
//				_fmemmove(theParm,&theParm[6],j+1-6); // [sic]
				memmove(theParm,&theParm[6],j+1-6); // [sic]
			return TRUE;
		}
		else
			theParm = &theParm[thePos+1];
	}
}

/* ------------------------------- WinMain ----------------------------- */

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
	MSG msg;
	static FARPROC lpfnAboutDlgProc;                                        
	char messageBuf[80];

	hInst = hInstance;

   lpfnAboutDlgProc = MakeProcInstance ((FARPROC) ClearingDlgProc, hInst );
   hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_KEYC), NULL, lpfnAboutDlgProc) ;
 	
	DeleteAllSessions();
   
   if (hWnd == NULL)
   	return FALSE;
   	
	ShowWindow(hWnd,SW_SHOWNORMAL);
	UpdateWindow(hWnd);                       
		                                                        
	if (!SetTimer(hWnd,1,2000,NULL))                
	{
		DestroyWindow(hWnd);
		return(FALSE);
	} 
		
	while (GetMessage(&msg, NULL, NULL, NULL))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	return(msg.wParam);
	}
                                                                          

/* -------------------------------------------------------------------
	ClearingDlgProc -- all melts, thaws, and resolves into a dew.
	------------------------------------------------------------------- */
long FAR PASCAL ClearingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RECT rMine,rDesktop;
		WORD wWidth,wHeight,wLeft,wTop;
	                                                                   
	switch (message)
		{
		case WM_INITDIALOG:
			GetWindowRect(hDlg,&rMine);
			GetWindowRect(GetDesktopWindow(),&rDesktop);
			wWidth = rMine.right - rMine.left;
			wHeight = rMine.bottom - rMine.top;
			wLeft = (rDesktop.right - wWidth) /2 ;
			wTop = (rDesktop.bottom - wHeight) / 2;
			MoveWindow(hDlg,wLeft,wTop,wWidth,wHeight,NULL);
			return TRUE;
   		break;
 
		// If we were started by kclient.dll, we should exit when
		// all other apps are done with kclient.dll.
		case WM_TIMER:	
         DestroyWindow(hDlg);
       //  PostAppMessage(GetCurrentTask(),WM_QUIT,0,0);
       PostAppMessage(GetCurrentProcess(),WM_QUIT,0,0);
			return TRUE;

	    default:
			return FALSE;
	    }
    return (NULL);
    }
