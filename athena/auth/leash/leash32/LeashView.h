//	**************************************************************************************
//	File:			LeashView.h 
//	By:				Arthur David Leather
//	Created:		12/02/98
//	Copyright		@1998 Massachusetts Institute of Technology - All rights reserved.
//	Description:	H file for LeashView.cpp. Contains variables and functions 
//					for the Leash FormView
//
//	History:
//
//	MM/DD/YY	Inits	Description of Change
//	12/02/98	ADL		Original
//	**************************************************************************************


#if !defined(AFX_LeashVIEW_H__6F45AD99_561B_11D0_8FCF_00C04FC2A0C2__INCLUDED_)
#define AFX_LeashVIEW_H__6F45AD99_561B_11D0_8FCF_00C04FC2A0C2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define GOOD_TICKETS	1  // Don't change this value
#define EXPIRED_TICKETS 2  // Don't change this value
#define TICKETS_LOW		3
#define ONE_SECOND		1000
#define SMALL_ICONS     16
#define LARGE_ICONS     32

#define UPDATE_DISPLAY_TIME 60  //seconds

#define ACTIVE_TICKET  3
#define LOW_TICKET     5
#define EXPIRED_TICKET 7
#define ACTIVE_CLOCK   4
#define EXPIRED_CLOCK  0
#define PARENT_NODE    1 
#define TICKET_NOT_INSTALLED 6
#define LOW_CLOCK      8

#define NODE_IS_EXPANDED 2

#define CX_BORDER   1
#define CY_BORDER   1

#ifdef NO_TICKETS
#undef NO_TICKETS // XXX - this is evil but necessary thanks to silliness...
#endif

enum ticketTimeLeft{NO_TICKETS, ZERO_MINUTES_LEFT, FIVE_MINUTES_LEFT, TEN_MINUTES_LEFT, 
					FIFTEEN_MINUTES_LEFT}; // Don't change 'NO_TICKET's' value

class CLeashDebugWindow;

class CLeashView : public CFormView
{
private:
    TicketList*         m_listKrb4; 
    TicketList*         m_listKrb5; 
    TicketList*         m_listAFS;
    CLeashDebugWindow*	m_pDebugWindow;
	CImageList			m_imageList;
	CImageList			*m_pImageList; 
    CTreeCtrl*			m_pTree; 
	CWinApp*			m_pApp; 
	HTREEITEM			m_hPrincipal;
	HTREEITEM			m_hKerb4;
	HTREEITEM			m_hKerb5;
	HTREEITEM			m_hAFS; 
	TV_INSERTSTRUCT		m_tvinsert;
	HMENU				m_hMenu; 
    BOOL                m_afsNoTokens; 
    BOOL				m_startup;
	BOOL				m_isMinimum;
	BOOL				m_debugStartUp;	
	BOOL				m_alreadyPlayed;
    INT					m_upperCaseRealm;
	INT					m_destroyTicketsOnExit;
	INT					m_debugWindow;
	INT					m_largeIcons; 
	INT					m_lowTicketAlarm; 
	INT					m_hPrincipalState;
	INT					m_hKerb4State;
	INT					m_hKerb5State;
	INT					m_hAFSState; 
    
    static INT          m_ticketStatusKrb4; 
    static INT          m_ticketStatusKrb5; 
	static INT			m_warningOfTicketTimeLeftKrb4;
	static INT			m_warningOfTicketTimeLeftKrb5;
    static INT			m_warningOfTicketTimeLeftLockKrb4;
    static INT			m_warningOfTicketTimeLeftLockKrb5;
    static INT			m_updateDisplayCount;
    static INT	        m_alreadyPlayedDisplayCount; 
    static LONG			m_ticketTimeLeft;    
	static BOOL			m_lowTicketAlarmSound;
    static BOOL         m_gotKrb4Tickets;
    static BOOL         m_gotKrb5Tickets;
    static BOOL         m_gotAfsTokens;


	VOID ResetTreeNodes();
    VOID ApplicationInfoMissingMsg();
    VOID GetScrollBarState(CSize sizeClient, CSize& needSb,
	                       CSize& sizeRange, CPoint& ptMove, 
                           BOOL bInsideClient);
    VOID UpdateBars();
    VOID GetScrollBarSizes(CSize& sizeSb);
    BOOL GetTrueClientSize(CSize& size, CSize& sizeSb);
    
    //void   GetRowWidthHeight(CDC* pDC, LPCSTR theString, int& nRowWidth,
    //                         int& nRowHeight, int& nCharWidth);
    static VOID	AlarmBeep(); 
	static VOID	CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, 
                                          DWORD dwTime);
	static VOID	UpdateTicketTime(TICKETINFO& ticketinfo);
	static INT	GetLowTicketStatus();
	static LONG	LeashTime();

protected: // create from serialization only
	DECLARE_DYNCREATE(CLeashView)

// Attributes
public:
	static INT m_forwardableTicket; 
	static INT m_proxiableTicket; 
    
    CLeashView();
	//LeashDoc* GetDocument();

	//{{AFX_DATA(CLeashView)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeashView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual VOID OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLeashView();

#ifdef _DEBUG
	virtual VOID AssertValid() const;
	virtual VOID Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLeashView)
        afx_msg VOID OnItemexpandedTreeview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg VOID OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg VOID OnRenewTicket();
	afx_msg VOID OnDestroyTicket();
	afx_msg VOID OnChangePassword();
	afx_msg VOID OnUpdateDisplay();
	afx_msg VOID OnSynTime();
	afx_msg VOID OnDebugMode();
	afx_msg VOID OnLargeIcons();
	afx_msg VOID OnUppercaseRealm();
	afx_msg VOID OnKillTixOnExit();
	afx_msg VOID OnDestroy();
	afx_msg VOID OnUpdateDestroyTicket(CCmdUI* pCmdUI);
	afx_msg VOID OnUpdateRenewTicket(CCmdUI* pCmdUI);
	afx_msg VOID OnAppAbout();
	afx_msg VOID OnAfsControlPanel();
	afx_msg VOID OnUpdateDebugMode(CCmdUI* pCmdUI);
	afx_msg VOID OnUpdateCfgFiles(CCmdUI* pCmdUI);
	afx_msg VOID OnKrb4Properties();
	afx_msg VOID OnKrb5Properties();
	afx_msg void OnLeashProperties();
	afx_msg void OnLowTicketAlarm();
	afx_msg void OnUpdateKrb4Properties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKrb5Properties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAfsControlPanel(CCmdUI* pCmdUI);
        afx_msg void OnKrbProperties();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnHelpKerberos();
	afx_msg void OnHelpLeash32();
	afx_msg void OnHelpWhyuseleash32();
        afx_msg void OnSysColorChange();

	//}}AFX_MSG
	afx_msg LRESULT OnGoodbye(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/*
#ifndef _DEBUG  // debug version in CLeashView.cpp
inline LeashDoc* CLeashView::GetDocument()
   { return (LeashDoc*)m_pDocument; }
#endif
*/

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LeashVIEW_H__6F45AD99_561B_11D0_8FCF_00C04FC2A0C2__INCLUDED_)
