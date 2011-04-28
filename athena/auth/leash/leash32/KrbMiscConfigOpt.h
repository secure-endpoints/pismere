//*****************************************************************************
// File:	KrbMiscConfigOpt.h
// By:		Paul B. Hill
// Created:	08/12/1999
// Copyright:	@1999 Massachusetts Institute of Technology - All rights 
//		reserved.
// Description: H file for KrbMiscConfigOpt.cpp.  Contains variables
//		and functions for Kerberos Properties.
//
// History:
//
// MM/DD/YY	Inits	Description of Change
// 08/12/99	PBH	Original
//*****************************************************************************


#if !defined(AFX_MISCCONFIGOPT_H__CD702F99_7495_11D0_8FDC_00C04FC2A0C2__INCLUDED_)
#define AFX_MISCONFIGOPT_H__CD702F99_7495_11D0_8FDC_00C04FC2A0C2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif 


#include "resource.h"


///////////////////////////////////////////////////////////////////////
// CKrbMiscConfigOptions dialog

class CKrbMiscConfigOpt : public CPropertyPage
{
// Construction
private:
	DECLARE_DYNCREATE(CKrbMiscConfigOpt)
	BOOL m_startupPage2;
	static CString m_DefaultLifeTime;
	static CString m_initDefaultLifeTime;
	static CString m_newDefaultLifeTime;
    BOOL m_noLifeTime;

    static void ResetDefaultLifeTimeEditBox();


public:
	CKrbMiscConfigOpt();
	~CKrbMiscConfigOpt();

// Dialog Data
	//{{AFX_DATA(CKrbMiscConfigOpt)
	enum { IDD = IDD_KRB_PROP_MISC };
	static CEdit m_krbLifeTimeEditbox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CKrbConfigOptions)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual BOOL OnApply();
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CKrbMiscConfigOpt)
	virtual BOOL OnInitDialog();	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEditchangeEditDefaultLifeTime();
	afx_msg void OnResetDefaultLifeTimeEditBox();
	afx_msg void OnSelchangeEditDefaultLifeTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISCONFIGOPT_H__CD702F99_7495_11D0_8FDC_00C04FC2A0C2__INCLUDED_)
