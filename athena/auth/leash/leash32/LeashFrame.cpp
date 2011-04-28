//	**************************************************************************************
//	File:			LeashFrame.cpp 
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:	CPP file for LeashFrame.h. Contains variables and functions 
//					for Leash 
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	12/02/98	ADL		Original
//	**************************************************************************************


#include "stdafx.h"
#include "LeashFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////////////////////////////
// CLeashFrame

const CRect CLeashFrame::s_rectDefault(179, 61, 806, 522);  // static
const char CLeashFrame::s_profileHeading[] = "Window size";
const char CLeashFrame::s_profileRect[] = "Rect";
const char CLeashFrame::s_profileIcon[] = "icon";
const char CLeashFrame::s_profileMax[] = "max";
const char CLeashFrame::s_profileTool[] = "tool";
const char CLeashFrame::s_profileStatus[] = "status";

IMPLEMENT_DYNAMIC(CLeashFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CLeashFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CLeashFrame)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
CLeashFrame::CLeashFrame()
{
    m_bFirstTime = TRUE;
}

///////////////////////////////////////////////////////////////
CLeashFrame::~CLeashFrame()
{
}

///////////////////////////////////////////////////////////////
void CLeashFrame::OnDestroy()
{
	CString strText;
	BOOL bIconic, bMaximized;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	// gets current window position and
	//  iconized/maximized status
	BOOL bRet = GetWindowPlacement(&wndpl);
	if (wndpl.showCmd == SW_SHOWNORMAL) 
	{
		bIconic = FALSE;
		bMaximized = FALSE;
	}
	else if (wndpl.showCmd == SW_SHOWMAXIMIZED) 
	{
		bIconic = FALSE;
		bMaximized = TRUE;
	} 
	else if (wndpl.showCmd == SW_SHOWMINIMIZED) 
	{
		bIconic = TRUE;
		if (wndpl.flags) 
		{
			bMaximized = TRUE;
		}
		else 
		{
			bMaximized = FALSE;
		}
	}
	
	strText.Format("%04d %04d %04d %04d",
	               wndpl.rcNormalPosition.left,
	               wndpl.rcNormalPosition.top,
	               wndpl.rcNormalPosition.right,
	               wndpl.rcNormalPosition.bottom);
	
	AfxGetApp()->WriteProfileString(s_profileHeading,
	                                s_profileRect, strText);
	
	AfxGetApp()->WriteProfileInt(s_profileHeading,
	                             s_profileIcon, bIconic);
	
	AfxGetApp()->WriteProfileInt(s_profileHeading,
	                             s_profileMax, bMaximized);
	
	SaveBarState(AfxGetApp()->m_pszProfileName);
	
	CFrameWnd::OnDestroy();
}

///////////////////////////////////////////////////////////////
void CLeashFrame::ActivateFrame(int nCmdShow)
{
	CString strText;
	BOOL bIconic, bMaximized;
	UINT flags;
	WINDOWPLACEMENT wndpl;
	CRect rect;

    ShowWindow(SW_HIDE);
    
    if (m_bFirstTime) 
	{
		m_bFirstTime = FALSE;
		strText = AfxGetApp()->GetProfileString(s_profileHeading,
		                                        s_profileRect);
		if (!strText.IsEmpty()) 
		{
			rect.left = atoi((const char*) strText);
			rect.top = atoi((const char*) strText + 5);
			rect.right = atoi((const char*) strText + 10);
			rect.bottom = atoi((const char*) strText + 15);
		}
		else 
		{
			rect = s_rectDefault;
		}

		bIconic = AfxGetApp()->GetProfileInt(s_profileHeading,
		                                     s_profileIcon, 0);
		
		bMaximized = AfxGetApp()->GetProfileInt(s_profileHeading,
		                                        s_profileMax, 0);   
		if (bIconic)
		{
			nCmdShow = SW_SHOWMINNOACTIVE;
			if (bMaximized) 
			{
				flags = WPF_RESTORETOMAXIMIZED;
			}
			else 
			{
				flags = WPF_SETMINPOSITION;
			}
		}
		else
		{
			if (bMaximized) 
			{
				nCmdShow = SW_SHOWMAXIMIZED;
				flags = WPF_RESTORETOMAXIMIZED;
			}
			else 
			{
				nCmdShow = SW_NORMAL;
				flags = WPF_SETMINPOSITION;
			}
		}
		
		wndpl.length = sizeof(WINDOWPLACEMENT);
		wndpl.showCmd = nCmdShow;
		wndpl.flags = flags;
		wndpl.ptMinPosition = CPoint(0, 0);
		wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER),
									 -::GetSystemMetrics(SM_CYBORDER));
		wndpl.rcNormalPosition = rect;
		LoadBarState(AfxGetApp()->m_pszProfileName);
		
		BOOL bRet = SetWindowPlacement(&wndpl);
	}

	CFrameWnd::ActivateFrame(nCmdShow);
}




