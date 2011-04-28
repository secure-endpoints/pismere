// wshtevw.cpp : implementation of the CWshtestView class
//

#include "stdafx.h"
#include "wshtest.h"

#include <string.h>
#include <wshelper.h>   // includes winsock.h for me

#include "wshtedoc.h"
#include "wshtevw.h"
#include "respdlg.h"
#include "hespdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWshtestView

IMPLEMENT_DYNCREATE(CWshtestView, CView)

BEGIN_MESSAGE_MAP(CWshtestView, CView)
	//{{AFX_MSG_MAP(CWshtestView)
	ON_COMMAND(ID_FILE_NewQuery, OnFILENewQuery)
	ON_COMMAND(ID_FILE_HesiodQuery, OnFILEHesiodQuery)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWshtestView construction/destruction

CWshtestView::CWshtestView()
{
	// TODO: add construction code here
}

CWshtestView::~CWshtestView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWshtestView drawing

void CWshtestView::OnDraw(CDC* pDC)
{
	CWshtestDoc* pDoc = GetDocument();

	// TODO: add draw code here
}



/////////////////////////////////////////////////////////////////////////////
// CWshtestView diagnostics

#ifdef _DEBUG
void CWshtestView::AssertValid() const
{
	CView::AssertValid();
}

void CWshtestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWshtestDoc* CWshtestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWshtestDoc)));
	return (CWshtestDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWshtestView message handlers

void CWshtestView::OnFILENewQuery()
{
	CRespDlg rdlg;
	
	int ret = rdlg.DoModal();
	
}


void CWshtestView::OnFILEHesiodQuery()
{
	CHespDlg hpdlg;
	int ret = hpdlg.DoModal();
	
}
