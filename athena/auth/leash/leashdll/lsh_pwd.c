/* LSH_PWD.C

   Jason Hunter
   8/2/94
   DCNS/IS MIT


   Contains the callback functions for the EnterPassword an
   ChangePassword dialog boxes and well as the API function
   calls:

   Lsh_Enter_Password_Dialog
   Lsh_Change_Password_Dialog

   for calling the dialogs.

   Also contains the callback for the MITPasswordControl.

*/

/* Standard Include files */
#include <stdio.h>
#include <string.h>

/* Private Inlclude files */
#include "leashdll.h"
#include <conf.h>
#include <leashwin.h>
#include "leash-int.h"
#include "leashids.h"
#include <leasherr.h>
#include <krb.h>

/* Global Variables. */
static long lsh_errno;
static char *err_context;       /* error context */
extern HINSTANCE hLeashInst;
extern int (*Lcom_err)(LPSTR,long,LPSTR,...);
extern LPSTR (*Lerror_message)(long);
extern LPSTR (*Lerror_table_name)(long);


INT_PTR
CALLBACK
PasswordProc(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    );


long Leash_get_lsh_errno(LONG *err_val)
{
    return lsh_errno;
}

/*/////// ******** API Calls follow here.   ******** /////////*/

int Leash_kinit_dlg(HWND hParent, LPLSH_DLGINFO lpdlginfo)
{
    lpdlginfo->dlgtype = DLGTYPE_PASSWD;

    /* set the help file */
    Leash_set_help_file(NULL);

    /* Call the Dialog box with the DLL's Password Callback and the
       DLL's instance handle. */
    return DialogBoxParam(hLeashInst, "EnterPasswordDlg", hParent,
                          PasswordProc, (LPARAM)lpdlginfo);
}


int Leash_changepwd_dlg(HWND hParent, LPLSH_DLGINFO lpdlginfo)
{

    lpdlginfo->dlgtype = DLGTYPE_CHPASSWD;

    /* Call the Dialog box with the DLL's Password Callback and the
       DLL's instance handle. */
    return DialogBoxParam(hLeashInst, "CHANGEPASSWORDDLG", hParent,
                          PasswordProc, (LPARAM)lpdlginfo);
}


/*  These little utils are taken from lshutil.c
    they are added here for the Call back funtion.
****** beginning of added utils from lshutil.c  ******/

BOOL IsDlgItem(HWND hWnd, WORD id)
{
    HWND hChild;

    hChild = GetDlgItem(hWnd, id);
    return hChild ? IsWindow(hChild) : 0;
}

int lsh_getkeystate(WORD keyid)
{
    static BYTE keys[256];

    GetKeyboardState((LPBYTE) &keys);
    return (int) keys[keyid];
}

LPSTR krb_err_func(int offset, long code)
{
    return pget_krb_err_txt_entry(offset);
}



/****** End of Added utils from leash.c  ******/


int PaintLogoBitmap( HANDLE hPicFrame )
{
    HBITMAP hBitmap;
    HBITMAP hOldBitmap;
    BITMAP Bitmap;
    HDC hdc, hdcMem;
    RECT rect;

    /* Invalidate the drawing space of the picframe. */
    InvalidateRect( hPicFrame, NULL, TRUE);
    UpdateWindow( hPicFrame );

    hdc = GetDC(hPicFrame);
    hdcMem = CreateCompatibleDC(hdc);
    GetClientRect(hPicFrame, &rect);
    hBitmap = LoadBitmap(hLeashInst, "LOGOBITMAP");
    hOldBitmap = SelectObject(hdcMem, hBitmap);
    GetObject(hBitmap, sizeof(Bitmap), (LPSTR) &Bitmap);
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0,
               Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);

    SelectObject(hdcMem, hOldBitmap); /* pbh 8-15-94 */
    ReleaseDC(hPicFrame, hdc);
    DeleteObject( hBitmap );  /* pbh 8-15-94 */
    DeleteDC( hdcMem );       /* pbh 8-15-94 */

    return 0;
}


/* Callback function for the Password Dialog box that initilializes and
   renews tickets. */

INT_PTR
CALLBACK
PasswordProc(
    HWND hDialog,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    static POINT Position = { -1, -1 };
    static short state;
    int lifetime;
#define ISCHPASSWD (lpdi->dlgtype == DLGTYPE_CHPASSWD)
#define STATE_INIT     0
#define STATE_PRINCIPAL 1
#define STATE_OLDPWD   2
#define STATE_NEWPWD1  3
#define STATE_NEWPWD2  4
#define STATE_CLOSED   5
#define NEXTSTATE(newstate) SendMessage(hDialog, WM_COMMAND, ID_NEXTSTATE, newstate)
    static int ids[STATE_NEWPWD2 + 1] = {
        0,
        ID_PRINCIPAL, ID_OLDPASSWORD, ID_CONFIRMPASSWORD1,
        ID_CONFIRMPASSWORD2};
    static char principal[255], oldpassword[255], newpassword[255],
        newpassword2[255];
    static char *strings[STATE_NEWPWD2 + 1] = {
        NULL, principal, oldpassword, newpassword, newpassword2};
    static LPLSH_DLGINFO lpdi;
    char gbuf[200];                 /* global buffer for random stuff. */


#define checkfirst(id, stuff) IsDlgItem(hDialog, id) ? stuff : 0
#define CGetDlgItemText(hDlg, id, cp, len) checkfirst(id, GetDlgItemText(hDlg, id, cp, len))
#define CSetDlgItemText(hDlg, id, cp) checkfirst(id, SetDlgItemText(hDlg, id, cp))
#define CSetDlgItemInt(hDlg, id, i, b) checkfirst(id, SetDlgItemInt(hDlg, id, i, b))
#define CSendDlgItemMessage(hDlg, id, m, w, l) checkfirst(id, SendDlgItemMessage(hDlg, id, m, w, l))
#define CSendMessage(hwnd, m, w, l) IsWindow(hwnd) ? SendMessage(hwnd, m, w, l) : 0
#define CShowWindow(hwnd, state) IsWindow(hwnd) ? ShowWindow(hwnd, state) : 0

#define GETITEMTEXT(id, cp, maxlen) \
  GetDlgItemText(hDialog, id, (LPSTR)(cp), maxlen)
#define CloseMe(x) SendMessage(hDialog, WM_COMMAND, ID_CLOSEME, x)


#define EDITFRAMEIDOFFSET               500

    switch (message) {

    case WM_INITDIALOG:

        *( (LPLSH_DLGINFO far *)(&lpdi) ) = (LPLSH_DLGINFO)(LPSTR)lParam;
        lpdi->dlgstatemax = ISCHPASSWD ? STATE_NEWPWD2
            : STATE_OLDPWD;
        SetWindowText(hDialog, lpdi->title);
        /* stop at old password for normal password dlg */

        SetProp(hDialog, "HANDLES_HELP", (HANDLE)1);

        if (lpdi->principal)
            lstrcpy(principal, lpdi->principal);
        else
    	{
            principal[0] = '\0';
            /* is there a principal already being used? if so, use it. */
	    }

        CSetDlgItemText(hDialog, ID_PRINCIPAL, principal);

        lifetime = Leash_get_default_lifetime();
        if (lifetime <= 0)
            lifetime = 600; /* 10 hours */

        CSetDlgItemInt(hDialog, ID_DURATION, lifetime, FALSE);

        /* setup text of stuff. */

        if (Position.x > 0 && Position.y > 0 &&
            Position.x < GetSystemMetrics(SM_CXSCREEN) &&
            Position.y < GetSystemMetrics(SM_CYSCREEN))
            SetWindowPos(hDialog, 0, Position.x, Position.y, 0, 0, 
                         SWP_NOSIZE | SWP_NOZORDER);

        /* set window pos to last saved window pos */


        /* replace standard edit control with our own password edit
           control for password entry. */
        {
            RECT r;
            POINT pxy, psz;
            HWND hwnd;
            int i;

            for (i = ID_OLDPASSWORD; i <= ids[lpdi->dlgstatemax]; i++)
            {
                hwnd = GetDlgItem(hDialog, i);
                GetWindowRect(hwnd, &r);
                psz.x = r.right - r.left;
                psz.y = r.bottom - r.top;

                pxy.x = r.left; pxy.y = r.top;
                ScreenToClient(hDialog, &pxy);

                /* create a substitute window: */

                DestroyWindow(hwnd);
                /* kill off old edit window. */

                CreateWindow(MIT_PWD_DLL_CLASS,	/* our password window :o] */
                             "",		/* no text */
                             WS_CHILD | WS_VISIBLE | WS_TABSTOP, /* child window, visible,tabstop */
                             pxy.x, pxy.y,	/* x, y coords */
                             psz.x, psz.y,	/* width, height */
                             hDialog,		/* the parent */
                             (HMENU)i,		/* same id *//* id offset for the frames */
                             (HANDLE)hLeashInst,/* instance handles */
                             NULL);		/* createstruct */
            }
        }

        state = STATE_INIT;
        NEXTSTATE(STATE_PRINCIPAL);
        break;

    case WM_PAINT:
        PaintLogoBitmap( GetDlgItem(hDialog, ID_PICFRAME) );
        break;

    case WM_COMMAND:
        switch (wParam) {
        case ID_HELP:
	{
            WinHelp(GetWindow(hDialog,GW_OWNER), KRB_HelpFile, HELP_CONTEXT,
                    ISCHPASSWD ? ID_CHANGEPASSWORD : ID_INITTICKETS);
	}
	break;
        case ID_CLOSEME:
	{
            int i;

            for (i = STATE_PRINCIPAL; i <= lpdi->dlgstatemax; i++)
	    {
                memset(strings[i], '\0', 255);
                SetDlgItemText(hDialog, ids[i], "");
	    }
            /* I claim these passwords in the name
               of planet '\0'... */

            RemoveProp(hDialog, "HANDLES_HELP");
            state = STATE_CLOSED;
            EndDialog(hDialog, (int)lParam);
	}
	break;
        case ID_DURATION:
            break;
        case ID_PRINCIPAL:
        case ID_OLDPASSWORD:
        case ID_CONFIRMPASSWORD1:
        case ID_CONFIRMPASSWORD2:
            if (HIWORD(lParam) == EN_SETFOCUS)
            {
                /* nothing, for now. */
            }
            break;
        case ID_NEXTSTATE:
	{
            RECT rbtn, redit;
            POINT p;
            int idfocus, i, s;
            HWND hfocus, hbtn;
            int oldstate = state;

            state = (int)lParam;
            idfocus = ids[state];

#ifdef ONE_NEWPWDBOX
            if (state == STATE_NEWPWD2)
                SendDlgItemMessage(hDialog, ID_CONFIRMPASSWORD1, WM_SETTEXT,
                                   0, (LONG)(LPSTR)"");
#endif

            for (s = STATE_PRINCIPAL; s <= lpdi->dlgstatemax; s++)
	    {
                i = ids[s];

                if (s > state)
                    SendDlgItemMessage(hDialog, i, WM_SETTEXT, 0,
                                       (LONG)(LPSTR)"");
                EnableWindow(GetDlgItem(hDialog, i), i == idfocus);
                ShowWindow(GetDlgItem(hDialog, i),
                           (i <= idfocus ? SW_SHOW : SW_HIDE));
                /* ShowWindow(GetDlgItem(hDialog, i + CAPTION_OFFSET),
                   (i <= idfocus ? SW_SHOW : SW_HIDE));*/
                /* show caption? */
	    }
#ifdef ONE_NEWPWDBOX
            CSetDlgItemText(hDialog, ID_CONFIRMCAPTION1,
                            state < STATE_NEWPWD2 ?
                            "Enter new password:" :
                            "Enter new password again:");
            if (state == STATE_NEWPWD2)
	    {
                HWND htext;
                htext = GetDlgItem(hDialog, ID_CONFIRMCAPTION1);
                FlashAnyWindow(htext);
                WinSleep(50);
                FlashAnyWindow(htext);
	    }
#endif

            hfocus = GetDlgItem(hDialog, idfocus);
            if ( hfocus != (HWND)NULL ){
                SetFocus(hfocus); /* switch focus */
                if (idfocus >= ID_OLDPASSWORD)
                    SendMessage(hfocus, WM_SETTEXT, 0, (LPARAM) (LPSTR) "");
                else
                {
                    SendMessage(hfocus, EM_SETSEL, 0, MAKELONG(0, -1));
                }
                GetWindowRect(hfocus, &redit);
            }

            hbtn   = GetDlgItem(hDialog, IDOK);
            if( IsWindow(hbtn) ){
                GetWindowRect(hbtn, &rbtn);
                p.x = rbtn.left; p.y = redit.top;
                ScreenToClient(hDialog, &p);

                SetWindowPos(hbtn, 0, p.x, p.y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER);
            }
	}
	break;
        case IDOK:
	{
	    char* p_Principal;
	    LONG lResult;
            DWORD value = 0;

	    GETITEMTEXT(ids[state], (LPSTR)strings[state], 255);

            switch(state)
            {
            case STATE_PRINCIPAL:
            {
                if (!principal[0])
                {
                    MessageBox(hDialog, "You are not allowed to enter a "
                               "blank principal.",
                               "Invalid Principal",
                               MB_OK | MB_ICONSTOP);
		    NEXTSTATE(STATE_PRINCIPAL);
		    return TRUE;
                }

	        // Change 'principal' to upper case after checking
	        // "UpperCase" value in the Registry
                p_Principal = strchr(principal, '@');

                if (p_Principal) {
                    // Get the registry value for 'UpperCase'
                    // If value is TRUE, than convert realm to upper case
                    lResult = read_registry_setting_user(LEASH_REG_SETTING_UPPERCASEREALM,
                                                         &value,
                                                         sizeof(value));
                    if (lResult == ERROR_SUCCESS && value)
                    {
                        // found
                        strupr(p_Principal);
                    }
                }
                break;
            }
	    case STATE_OLDPWD:
            {
		int duration;

		if (!ISCHPASSWD)
                    duration = GetDlgItemInt(hDialog, ID_DURATION, 0, FALSE);
                if (!oldpassword[0])
                {
                    MessageBox(hDialog, "You are not allowed to enter a "
                               "blank password.",
                               "Invalid Password",
                               MB_OK | MB_ICONSTOP);
		    NEXTSTATE(STATE_OLDPWD);
		    return TRUE;
                }
                if (lpdi->dlgtype == DLGTYPE_CHPASSWD)
                    lsh_errno = Leash_checkpwd(principal, oldpassword);
                else
                {
                    lsh_errno = Leash_kinit(principal, oldpassword, duration);
                }
		if (lsh_errno != 0)
                {
		    int next_state = state;
		    int capslock;
		    LONG check_time;
		    char *cp;

		    err_context = "";

		    switch(lsh_errno)
                    {
                    case LSH_INVPRINCIPAL:
                    case LSH_INVINSTANCE:
                    case LSH_INVREALM:
                    case KRBERR(KDC_PR_UNKNOWN):
			next_state = STATE_PRINCIPAL;
			break;
                    case KRBERR(RD_AP_TIME):
                    case KRBERR(KDC_SERVICE_EXP):
                        check_time = Leash_timesync(1);
                        if( check_time == 0 ){
                            next_state = STATE_PRINCIPAL;
                            SendMessage(hDialog, WM_COMMAND, IDOK, state);
                            return(TRUE);
                        } else {
                            next_state = STATE_PRINCIPAL;
                            lsh_errno = check_time;
                            return(TRUE);
                        }
                        break;
                    }
		    capslock = lsh_getkeystate(VK_CAPITAL);
                    /* low-order bit means caps lock is
                       toggled; if so, warn user since there's
                       been an error. */
		    if (capslock & 1)
                    {
			lstrcpy((LPSTR)gbuf, (LPSTR)err_context);
			cp = gbuf + lstrlen((LPSTR)gbuf);
			if (cp != gbuf)
                            *cp++ = ' ';
			lstrcpy(cp, "(This may be because your CAPS LOCK key is down.)");
			err_context = gbuf;
                    }

// XXX		    DoNiftyErrorReport(lsh_errno, ISCHPASSWD ? ""
// XXX				       : "Ticket initialization failed.");
		    NEXTSTATE(next_state);
		    return TRUE;
                }
		if (ISCHPASSWD)
                    break;
		CloseMe(TRUE); /* success */
            }
            break;
	    case STATE_NEWPWD1:
            {
		int i = 0;
		for( i = 0; i < 255; i++ ){
                    if( newpassword[i] == '\0' ){
                        i = 255;
                        break;
                    } else if( !isprint(newpassword[i]) ){
                        memset(newpassword, '\0', 255);
                        /* I claim these passwords in the name of planet '\0'... */
                        Lcom_err("Leash", LSH_BADCHARS, "");
                        NEXTSTATE(STATE_NEWPWD1);
                        return TRUE;
                    }
		}
            }
            break;
	    case STATE_NEWPWD2:
                if (lstrcmp(newpassword, newpassword2))
		{
                    NEXTSTATE(STATE_NEWPWD1);
                    Lcom_err("Leash", LSH_NOMATCH, "");
                    return TRUE;
		}
                else
		{
                    /* make them type both pwds again if error */
                    int next_state = STATE_NEWPWD1;
                    int capslock;
                    char *cp;

                    capslock = lsh_getkeystate(VK_CAPITAL);
                    /* low-order bit means caps lock is
                       toggled; if so, warn user since there's
                       been an error. */
                    if (capslock & 1)
		    {
                        lstrcpy((LPSTR)gbuf, (LPSTR)err_context);
                        cp = gbuf + lstrlen((LPSTR)gbuf);
                        if (cp != gbuf)
                            *cp++ = ' ';
                        lstrcpy(cp, "(This may be because your CAPS LOCK key is down.)");
                        err_context = gbuf;
		    }

                    if ((lsh_errno =
                         Leash_changepwd(principal, oldpassword,
                                         newpassword, 0))
                        == 0){
                        CloseMe(TRUE);
                    }
                    else {
                        // XXX - DoNiftyErrorReport(lsh_errno, "Error while changing password.");
                        NEXTSTATE(next_state);
                        return TRUE;

                    }
		}
                break;
	    }
            /* increment state, but send the old state as a
               parameter */
            SendMessage(hDialog, WM_COMMAND, ID_NEXTSTATE, state + 1);
	}
	break;
        case IDCANCEL:
            CloseMe(FALSE);
            break;
        case ID_RESTART:
	{
            int i;

            for (i = ID_OLDPASSWORD; i <= ids[lpdi->dlgstatemax]; i++)
                SetDlgItemText(hDialog, i, "");
            SendMessage(hDialog, WM_COMMAND, ID_NEXTSTATE,
                        STATE_PRINCIPAL);
	}
	break;
        }
        break;

    case WM_MOVE:
        if (state != STATE_CLOSED)
#ifdef _WIN32
#define LONG2POINT(l,pt) ((pt).x=(SHORT)LOWORD(l),  \
			  (pt).y=(SHORT)HIWORD(l))
            LONG2POINT(lParam,Position);
#else
        Position = MAKEPOINT(lParam);
#endif
        break;
    }
    return FALSE;
}
