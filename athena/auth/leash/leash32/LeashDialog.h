#ifndef Leash
#define Leash

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// LeashDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLeashDialog dialog

class CLeashDialog : public CDialog
{
// Construction
public:
	int m_nTrackbar2;
	int m_nTrackbar1;
	int m_nProgress;
	CLeashDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLeashDialog)
	enum { IDD = IDD_DIALOG1 };
	double	m_dSpin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeashDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLeashDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemchangedListview1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeview1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	static double dValue[];
	CImageList m_imageList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // Leash
