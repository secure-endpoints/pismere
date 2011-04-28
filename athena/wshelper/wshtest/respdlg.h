// respdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRespDlg dialog

class CRespDlg : public CDialog
{
// Construction
public:
	CRespDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRespDlg)
	enum { IDD = IDD_RespDlg };
	CEdit	m_OHost;
	CListBox	m_AliasList;
	CListBox	m_AddrList;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CRespDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
