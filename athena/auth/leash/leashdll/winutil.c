#include <stdio.h>

#include "leashdll.h"
#include <krb.h>
#include <leashwin.h>

#ifndef HFILE
typedef int HFILE;      /* Windows 3.1 syntax complied under C6, Win 3.0 */
#endif

#include <string.h>

#define DLGHT(ht) (HIWORD(GetDialogBaseUnits())*(ht)/8)
#define DLGWD(wd) (LOWORD(GetDialogBaseUnits())*(wd)/4)

#if (_MSC_VER < 700)
#define _close(x)   close(x)
#endif /* MSC_VER  */

extern BOOL fWin31;

int FAR PASCAL my_atoi(LPSTR buf)
{
	int i=0,j,k;

	while(((j = *(buf+i)) < 48 || j > 57) && j != 0)
		i++;

	j -= 48;

	while((k = *(buf+(++i))) > 47 && j < 58 && j != 0)
		j = j * 10 + (k-48);

	return (j);
}

void mit_yield(void)
{
  MSG msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	switch (msg.message) {
	    case WM_KEYDOWN:
	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
	    case WM_NCLBUTTONDOWN:
	    case WM_NCMBUTTONDOWN:
	    case WM_NCRBUTTONDOWN:
		;
	    default:
		DispatchMessage(&msg);
	}
}

/* jms 1/14/93 */
void UpdateListItem(HWND hList, int index, BOOL update)
{
  static RECT r;

  if (SendMessage(hList, LB_GETITEMRECT, index, (LONG)(LPRECT)&r) != LB_ERR)
    {
      InvalidateRect(hList, &r, TRUE);
      if (update)
	UpdateWindow(hList);
    }
}

/* Callback for the MITPasswordControl
This is a replacement for the normal edit control.  It does not show the 
annoying password char in the edit box so that the number of chars in the 
password are not known.
*/

LONG FAR PASCAL _export MITPwdWinProcDLL(HWND hWnd, WORD message, WORD wParam, LONG lParam)
{
  #ifdef WIN32
  static SIZE pwdcharsz;
  #else
  static LONG pwdcharsz;
  #endif
  BOOL pass_the_buck = FALSE;
  
  if (message > WM_USER && message < 0x7FFF)
    pass_the_buck = TRUE;
  
  switch(message)
    {
    case WM_GETTEXT:
    case WM_GETTEXTLENGTH:
    case WM_SETTEXT:
      pass_the_buck = TRUE;
      break;
    case WM_PAINT:
      {
        HDC hdc;
        PAINTSTRUCT ps;
        RECT r;
        
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &r);
        Rectangle(hdc, 0, 0, r.right, r.bottom);
        EndPaint(hWnd, &ps);
      }
      break;
    case WM_SIZE:
      {
	#ifdef WIN32
        MoveWindow(GetDlgItem(hWnd, 1), DLGWD(2), DLGHT(2),
		   pwdcharsz.cx / 2, pwdcharsz.cy, TRUE);
	#else
        MoveWindow(GetDlgItem(hWnd, 1), DLGWD(2), DLGHT(2),
		   LOWORD(pwdcharsz) / 2, HIWORD(pwdcharsz), TRUE);
	#endif
      }
      break;
    case WM_LBUTTONDOWN:
    case WM_SETFOCUS:
      {
        SetFocus(GetDlgItem(hWnd, 1));
      }
      break;
    case WM_CREATE:
      {
        HWND heditchild;
        char pwdchar = PASSWORDCHAR;
        HDC hdc;
        /* Create a child window of this control for default processing. */
          hdc = GetDC(hWnd);
#ifdef WIN32
        GetTextExtentPoint32(hdc, &pwdchar, 1, &pwdcharsz);
#else
        pwdcharsz = GetTextExtent(hdc, &pwdchar, 1);
#endif
        ReleaseDC(hWnd, hdc);
        
        heditchild =
          CreateWindow("edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL |
                       ES_LEFT | ES_PASSWORD | WS_TABSTOP,
                       0, 0, 0, 0,
                       hWnd,
                       (HMENU)1,
                       ((LPCREATESTRUCT)lParam)->hInstance,
                       NULL);
        SendMessage(heditchild, EM_SETPASSWORDCHAR, PASSWORDCHAR, 0L);
      }
      break;
    }
  
  if (pass_the_buck)
    return SendMessage(GetDlgItem(hWnd, 1), message, wParam, lParam);
  return DefWindowProc(hWnd, message, wParam, lParam);
}
