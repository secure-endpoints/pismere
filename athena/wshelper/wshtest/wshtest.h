// wshtest.h : main header file for the WSHTEST application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWshtestApp:
// See wshtest.cpp for the implementation of this class
//

class CWshtestApp : public CWinApp
{
public:
	CWshtestApp();

// Overrides
	virtual BOOL InitInstance();

// Implementation

	//{{AFX_MSG(CWshtestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
