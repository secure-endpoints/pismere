// fleadlg.h : header file
//

#if !defined(AFX_FLEADLG_H__FB594691_1A38_11D1_BA65_00C04FD49F29__INCLUDED_)
#define AFX_FLEADLG_H__FB594691_1A38_11D1_BA65_00C04FD49F29__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <cacheapi.h>

/////////////////////////////////////////////////////////////////////////////
// CFleaDlg dialog

class CFleaDlg : public CDialog
{
// Construction
public:
	CFleaDlg(CWnd* pParent = NULL);	// standard constructor
	CFleaDlg(bool debug, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CFleaDlg)
	enum { IDD = IDD_FLEAVIUS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFleaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	bool m_bDebug;

	// Generated message map functions
	//{{AFX_MSG(CFleaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLetGo();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	apiCB* m_api_CB;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLEADLG_H__FB594691_1A38_11D1_BA65_00C04FD49F29__INCLUDED_)
