// hespdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHespDlg dialog

class CHespDlg : public CDialog
{
// Construction
public:
	CHespDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHespDlg)
	enum { IDD = IDD_HespDlg };
	CEdit	m_HespText;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CHespDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
