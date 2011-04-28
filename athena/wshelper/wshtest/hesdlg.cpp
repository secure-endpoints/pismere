// hesdlg.cpp : implementation file
//

#include "stdafx.h"
#include "wshtest.h"
#include "hesdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHesDlg dialog

CHesDlg::CHesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHesDlg)
	m_HesIdent = "";
	m_HesType = "";
	//}}AFX_DATA_INIT
}

void CHesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHesDlg)
	DDX_Text(pDX, IDC_HesIdent, m_HesIdent);
	DDX_Text(pDX, IDC_HesType, m_HesType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHesDlg, CDialog)
	//{{AFX_MSG_MAP(CHesDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHesDlg message handlers

int CHesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
