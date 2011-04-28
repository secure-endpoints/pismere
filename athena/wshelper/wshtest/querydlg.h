// querydlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg dialog

class CQueryDlg : public CDialog
{
// Construction
public:
	CQueryDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryDlg)
	enum { IDD = IDD_ResOptions };
	BOOL	m_Auth;
	BOOL	m_DebugMsg;
	BOOL	m_DefName;
	CString	m_Host;
	BOOL	m_StayOpen;
	BOOL	m_UseTCP;
	BOOL	m_Recurse;
	int		m_ByHow;
	BOOL	m_DnSrch;
	BOOL	m_Primary;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CQueryDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
