// fleavius.h : main header file for the FLEAMFCAPP1 application
//

#if !defined(AFX_FLEAVIUS_H__FB59468F_1A38_11D1_BA65_00C04FD49F29__INCLUDED_)
#define AFX_FLEAVIUS_H__FB59468F_1A38_11D1_BA65_00C04FD49F29__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFleaApp:
// See fleavius.cpp for the implementation of this class
//

class CFleaApp : public CWinApp
{
public:
	CFleaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFleaApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFleaApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLEAVIUS_H__FB59468F_1A38_11D1_BA65_00C04FD49F29__INCLUDED_)
