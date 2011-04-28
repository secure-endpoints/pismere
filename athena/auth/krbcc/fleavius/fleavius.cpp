// fleavius.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "fleavius.h"
#include "fleadlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFleaApp

BEGIN_MESSAGE_MAP(CFleaApp, CWinApp)
	//{{AFX_MSG_MAP(CFleaApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFleaApp construction

CFleaApp::CFleaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFleaApp object

CFleaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFleaApp initialization

BOOL CFleaApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Figure out whether to run debug or not...
	bool b_debug = false;
	LPWSTR lpwCmdLine = GetCommandLineW();
	int argc = 0;
	LPWSTR* argvw = CommandLineToArgvW(lpwCmdLine, &argc);
	if (argvw && argc) {
	    for (int i = 1; i < argc; i++) {
		LPWSTR argw = argvw[i];
		if (!lstrcmpiW(argw, L"-debug") ||
		    !lstrcmpiW(argw, L"--debug") ||
		    !lstrcmpiW(argw, L"-d") ||
		    !lstrcmpiW(argw, L"/d") ||
		    !lstrcmpiW(argw, L"/debug")) {
		    b_debug = true;
		} else {
		    MessageBox(0,
			       "Invalid command-line option.\n"
			       "--debug is the only supported option.",
			       "Fleavius", MB_OK | MB_ICONERROR);
		    return FALSE;
		}
	    }
	}

	CFleaDlg dlg(b_debug);
	m_pMainWnd = &dlg;

	int nResponse = dlg.DoModal();
	/*if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		// Do nothing, handled via OnLetGo in FleaDlg.cpp
		TRACE0("OK hit\n");
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
		TRACE0("CANCEL hit\n");
	}
	else
	{
		TRACE0("something ELSE hit\n");
	} */

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

