#include "conf.h"

#include <windows.h>
#include <winkrbid.h>
#include <string.h>

#ifdef WINDOWS
extern char FAR key_string[];
#endif


BOOL FAR PASCAL CheckKerbPassword(LPSTR password)
{
	HANDLE hWnd;
	HANDLE hInst;

	hWnd = GetActiveWindow();
#ifdef WIN32
	hInst = GetWindowLong( hWnd, GWL_HINSTANCE );
#else
	hInst = GetWindowWord( hWnd, GWW_HINSTANCE );
#endif
	return(TRUE);
}




int FAR PASCAL PasswordCB(HWND dialog, unsigned message, WORD wparam, LONG lparam)
{
	FARPROC lpCheckPassword;
	FARPROC lpGetUserName;
	BOOL check;
	HANDLE hInst;
	char passwd[255] = { 0 };




	/* pbh 4-6-92 */
	/* BOOLFARPROC lpCheckPassword( LPSTR uname, LPSTR passwd, LPSTR usrfile, HWND hWnd ); */

	switch(message) {
	case WM_INITDIALOG:
		return(TRUE);
	case WM_COMMAND:
		if(wparam == IDOK) {
			/* OK button pressed, call GetMail's CheckPassword function */
			hInst = hQKrbInst();
			/* lpCheckPassword = (FARPROC) GetProcAddress(hInst, (LPSTR) "CheckKerbPassword"); */

			// if( lpCheckPassword == NULL ) {
			//     /* Couldn't use CheckPassword (no DLL Loaded?) */
			//     /* pbh 4-7-92 */
			//     DialogBox(hInst, "KERB_PASSWDBOX", dialog, lpCheckPassword);

			//     MessageBox(dialog, (LPSTR) "Unable to use CheckKerbPassword", NULL, MB_OK);
			//     EndDialog(dialog, FALSE);
			//     return (TRUE);
			// }

			/* Provide user feedback of what is happening and Disable OK button*/
			/* In future versions, there should an abort option */
			// SetDlgItemText(dialog, IDD_STATUSTEXT,
			//	(LPSTR) "Checking Password...");
			check = GetDlgItemText(dialog, DES_PASSWD_IDC,
			     (LPSTR) passwd, DEFTEXTSIZE);


			EnableWindow(GetDlgItem(dialog, IDOK), FALSE);
			if( check != 0 ){
#ifdef WIN32
			  strcpy(key_string,passwd);
#else
			  _fstrcpy( (LPSTR) key_string, (LPSTR) passwd );
#endif
			}
			EndDialog(dialog, TRUE);
			return (TRUE);
		}
		break;
	}

	return (FALSE);

}
