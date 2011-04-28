//	**************************************************************************************
//	File:			MainFrm.cpp
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:    CPP file for MainFrm.h. Contains variables and functions 
//					for Leash 
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	12/02/98	ADL		Original
//	**************************************************************************************


#include "stdafx.h"
#include "Leash.h"
#include "MainFrm.h"
#include "lglobals.h"
//#include "KrbRealmHostMaintenance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

//CStatusBar  m_wndStatusBar;
//CToolBar    m_wndToolBar;

#define SKIP_MINSIZE  0 
#define RESET_MINSIZE 4 
#define MIN_LEFT      179
#define MIN_TOP		  61
#define MIN_RIGHT	  530 //500
#define MIN_BOTTOM	  280 //300	

CStatusBar CMainFrame::m_wndStatusBar;
CToolBar   CMainFrame::m_wndToolBar;
BOOL	   CMainFrame::m_isMinimum;
BOOL       CMainFrame::m_isBeingResized; 
int        CMainFrame::m_whatSide;

IMPLEMENT_DYNCREATE(CMainFrame, CLeashFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CLeashFrame)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_RESET_WINDOW_SIZE, OnResetWindowSize)
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_LEASH_, CMainFrame::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMainFrame::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMainFrame::OnContextHelp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_winRectLeft = 0;
	m_winRectTop = 0;
	m_winRectRight = 0;
	m_winRectBottom = 0;
	m_whatSide = RESET_MINSIZE;
	m_isMinimum = FALSE;
    m_isBeingResized = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CLeashFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    ShowWindow(SW_HIDE);
        
/* NT4 and NT5 aren't shipped with a version of MFC that supports 
// 'CreateEx()' as of 2/1/99
#if _MFC_VER > 0x0421 

	if (!m_wndToolBar.CreateEx(this) || 
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

#else

	if (!m_wndToolBar.Create(this) || 
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

#endif 	
*/

	if (!m_wndToolBar.Create(this) || 
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		MessageBox("There is problem creating the Leash32 Toolbar!", 
                   "Error", MB_OK);
        TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		MessageBox("There is problem creating the Leash32 Status Bar!", 
                   "Error", MB_OK);
        TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		                     CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//CLeashFrame::m_bAutoMenuEnable = FALSE;	
	
	// Use the specific class name we established earlier

    cs.dwExStyle |= WS_EX_CLIENTEDGE; 
	cs.lpszClass = _T("LEASH.0WNDCLASS");
    cs.cx = 0;
    cs.cy = 0;
    cs.y = -150;
    cs.x = -150;

    // Change the following line to call
	// CLeashFrame::PreCreateWindow(cs) if this is an SDI application.
	return CLeashFrame::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CLeashFrame::AssertValid();
}
	
void CMainFrame::Dump(CDumpContext& dc) const
{
	CLeashFrame::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnResetWindowSize() 
{
	
    WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	
    if (!GetWindowPlacement(&wndpl))
    {
        MessageBox("There is a problem getting Leash32 Window size!", 
                   "Error", MB_OK);
        return;
    }

	wndpl.rcNormalPosition.left = 0;
	wndpl.rcNormalPosition.top = 24;
	wndpl.rcNormalPosition.right = 627;
	wndpl.rcNormalPosition.bottom = 494; //486;

	m_whatSide = SKIP_MINSIZE;

    if (!SetWindowPlacement(&wndpl))
    { 
        MessageBox("There is a problem setting Leash32 Window size!", 
                   "Error", MB_OK);
    }

	m_whatSide = RESET_MINSIZE;
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{ // Keeps track of Leash window size for function CMainFrame::RecalcLayout
	m_winRectLeft = pRect->left;
	m_winRectTop = pRect->top;
	m_winRectRight = pRect->right;
	m_winRectBottom = pRect->bottom;

	if (m_whatSide)
	  m_whatSide = fwSide;
	
	CLeashFrame::OnSizing(fwSide, pRect);
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{ // MINSIZE - Insurance that we have a minimum Leash window size 
	int width = MIN_RIGHT - MIN_LEFT; 
	int height = MIN_BOTTOM - MIN_TOP;	

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	    
    if (!GetWindowPlacement(&wndpl))
    {
        MessageBox("There is a problem getting Leash32 Window size!", 
                   "Error", MB_OK);
        return;
    }
	
	if (m_whatSide)
	{
		if ((m_winRectRight - m_winRectLeft) < width)
		{
			if (m_whatSide == LEFT_SIDE)
			  wndpl.rcNormalPosition.left = wndpl.rcNormalPosition.right - width;
			else if (m_whatSide == RIGHT_SIDE)
			  wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + width;
		}
		else if ((m_winRectBottom - m_winRectTop) < height)
		{
			if (m_whatSide == TOP_SIDE)
			  wndpl.rcNormalPosition.top = wndpl.rcNormalPosition.bottom - height;
			else if (m_whatSide == BOTTOM_SIDE)
			  wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top + height;		
		}
	}
	
	if (!SetWindowPlacement(&wndpl))
    {
        MessageBox("There is a problem setting Leash32 Window size!", 
                   "Error", MB_OK);
    }
		
    m_isBeingResized = TRUE;
        
    CLeashFrame::RecalcLayout(bNotify);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CLeashFrame::OnGetMinMaxInfo(lpMMI);
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
 	switch(message)
	{
		case WM_SYSCOMMAND: 
			if (SC_MINIMIZE == (wParam & 0xfff0))
			  m_isMinimum = TRUE;	
			else if (SC_MAXIMIZE == (wParam & 0xfff0)) 
 			  m_isMinimum = FALSE;	
			else if (SC_RESTORE == (wParam & 0xfff0)) 
 			  m_isMinimum = FALSE;	
		
			break;
	}
	
	return CLeashFrame::WindowProc(message, wParam, lParam);
}

/*
void CMainFrame::OnHelp()
{

}
*/

/*
void CMainFrame::OnContextHelp()
{

}
*/