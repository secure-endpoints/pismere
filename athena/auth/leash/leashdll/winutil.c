#include <stdio.h>

#include "leashdll.h"
#include <krb.h>
#include <leashwin.h>

#include <string.h>

#define DLGHT(ht) (HIWORD(GetDialogBaseUnits())*(ht)/8)
#define DLGWD(wd) (LOWORD(GetDialogBaseUnits())*(wd)/4)

/* Callback for the MITPasswordControl
This is a replacement for the normal edit control.  It does not show the 
annoying password char in the edit box so that the number of chars in the 
password are not known.
*/

LRESULT CALLBACK
MITPwdWinProcDLL(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    static SIZE pwdcharsz;
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
        MoveWindow(GetDlgItem(hWnd, 1), DLGWD(2), DLGHT(2),
		   pwdcharsz.cx / 2, pwdcharsz.cy, TRUE);
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
        GetTextExtentPoint32(hdc, &pwdchar, 1, &pwdcharsz);
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
