// wshtedoc.cpp : implementation of the CWshtestDoc class
//

#include "stdafx.h"
#include "wshtest.h"

#include "wshtedoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWshtestDoc

IMPLEMENT_DYNCREATE(CWshtestDoc, CDocument)

BEGIN_MESSAGE_MAP(CWshtestDoc, CDocument)
	//{{AFX_MSG_MAP(CWshtestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWshtestDoc construction/destruction

CWshtestDoc::CWshtestDoc()
{
	// TODO: add one-time construction code here
}

CWshtestDoc::~CWshtestDoc()
{
}

BOOL CWshtestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWshtestDoc serialization

void CWshtestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWshtestDoc diagnostics

#ifdef _DEBUG
void CWshtestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWshtestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWshtestDoc commands
