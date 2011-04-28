// hesdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHesDlg dialog

class CHesDlg : public CDialog
{
// Construction
public:
	CHesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHesDlg)
	enum { IDD = IDD_HesOptions };
	CString	m_HesIdent;
	CString	m_HesType;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CHesDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
