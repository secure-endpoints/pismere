// wshtevw.h : interface of the CWshtestView class
//
/////////////////////////////////////////////////////////////////////////////

class CWshtestView : public CView
{
protected: // create from serialization only
	CWshtestView();
	DECLARE_DYNCREATE(CWshtestView)

// Attributes
public:
	CWshtestDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CWshtestView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CWshtestView)
	afx_msg void OnFILENewQuery();
	afx_msg void OnFILEHesiodQuery();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in wshtevw.cpp
inline CWshtestDoc* CWshtestView::GetDocument()
   { return (CWshtestDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
