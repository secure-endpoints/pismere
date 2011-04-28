
/*
this is part of Project Mandarin, Inc.'s 
"Kerberos Lite" software.  it parallels our
"Kerberos Client for Windows" software, but
it uses a different kind of security.

Copyright © 1996 by Project Mandarin, Inc.

Project type: Windows dynamic-link library
Memory model: small
Segment setup: SS!=DS  DS NOT loaded on function entry
Link with kclient.lib, winsock.lib 

Notes:
Incoming msg buffer is static (for now).

To Do:
Do I want to pass on Service I received from GetTicketForService()?

(Tabs set to 4 in this source file)
*/

#include <windows.h>
#include <winsock.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "klite.h"
#include "ahssp.h"
#include "klite_rc.h"

#define MAXBUFF 256 

/* ********************** Global Variables** **************** */
HANDLE hInst ;
TC	tc ;					// Transaction Context
LPTC	lpTc ;			// lp to TC

WSADATA wskData ;				// Windows socket fun
LPHOSTENT lph ;				// lp to hostent
LPSERVENT lps ;				// lp to servent
SOCKADDR l_assoc ;			// stores connection info
LPSOCKADDR_IN lpl_assoc ;	// alleviates some casting

int iLastRC ;						// last return code received from ahSSP server
LPSTR lpszRecvList[10] ;		// array of LPSTRs for recv()ing parameters
LPSTR FAR * lpFakeParams ;		// fake parameter pointer
WORD	iNoop ;						// number of incoming parameters
LPWORD lpiNoop ;

FAKETICKET		ft ;				// fake ticket
LPFAKETICKET	lpFt ;		

char szRecv[2048] ;		// receive buffer for incoming
char szSend[1024] ;		// send() buffer for outgoing
char message[MAXMSG] ;	// ahssp compose buffer
char incoming[MAXMSG] ;	// ahssp decompose buffer
char szTrace[MAXBUFF] ;		// Trace messages stored here before displaying
char szTemp[MAXBUFF] ;		// LPSTR to PSTR conversion purposes
char szErrorText[MAXBUFF] ;	// error text

char 	szCmd[MAXBUFF] ;		// ahSSP command
char 	szHost[MAXBUFF] ;		// KLite "server"
WORD  	iVer ;				// ahSSP version
int	iPort ;				// test port

LPSTR lpszCmd  ;			// ahSSP command
LPSTR lpszHost ;			// test host
LPSTR	lpszTrace ;			// alleviates some casting
LPSTR lpSend ;	
LPSTR lpRecv ;	
LPSTR	lpszTemp ;
LPSTR	lpMessage ;	
LPSTR lpIncoming ;
LPSTR	lpszErrorText ;
LPSTR lpszDLL = "klite.dll ->" ; // for easy trace reading

BOOL bTraceL1 ;					// Socket trace option
BOOL bTraceL2 ;					// ahssp trace option
BOOL bTraceIncoming  ;			// Incoming temp file trace options
BOOL bTraceToWindow ;			// Trace to window option
HWND hWndTrace = 0x00 ;			// hWnd to send traces to

UINT iCnt ;   				// number of bytes received from incoming/outgoing
int iWrite ;				// number of bytes written to temp file

fd_set	set ;					// used with select()
TIMEVAL tv ;					// used with select()
BOOL	bReturn ;				// all purpose return value

UINT iEoc = 0xefff ;			// EOchunk  - already in network byte order
UINT iEom = 0xedff ;			// EOmessge - already in network byte order
UINT iBb  = 0xbbff ;       // begin binary - already in network byte order
UINT iEb  = 0xebff ;			// end binary - already in network byte order

WSADATA wskData ;

/* ********************** Function Prototypes **************** */
#if !defined(_WIN32)
int KL_CALLTYPE KL_EXPORT WEP (int) ;
#else
BOOL   WINAPI   DllMain (HANDLE hInst, ULONG ul_reason_for_call,
						 LPVOID lpReserved);
#endif

void KL_CALLTYPE KL_EXPORT Abort (void) ;   			
BOOL KL_CALLTYPE KL_EXPORT TraceToWindow (BOOL, HWND) ;

BOOL KL_CALLTYPE	Init (LPTC) ;		
BOOL KL_CALLTYPE	SendMsg (LPTC, LPSTR, int) ;
UINT KL_CALLTYPE	RecvMsg (LPTC) ;
UINT KL_CALLTYPE  ComposeMsg (LPSTR, LPSTR, int, int, LPSTR FAR *lpParams) ;
BOOL KL_CALLTYPE  DeComposeMsg (LPWORD, LPSTR FAR *lpParams, UINT) ;
BOOL KL_CALLTYPE	IsValid (UINT);
UINT KL_CALLTYPE	CopyInAuthenticator (LPBYTE) ;
BOOL KL_CALLTYPE 	BounceTicketOffServer (void) ;
void KL_CALLTYPE 	GetSettings (void) ;
BOOL Trace (LPSTR, ...) ;



/* *********************************************************** 
	LibMain() or DllMain() as appropriate, followed by WEP (if 16 bit)
	*********************************************************** */
#if defined(_WIN32)
BOOL WINAPI DllMain (HANDLE hInstance, ULONG ul_reason_for_call, LPVOID lpReserved)
#else
int KL_CALLTYPE LibMain (HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine)
#endif

{

#if !defined(_WIN32)
	if (wHeapSize > 0)
		UnlockData (0) ;
#endif

#if defined(_WIN32)
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
#endif

	hInst = hInstance ;
	
	lpTc = (LPTC)&tc ;
	lpszTrace = (LPSTR)szTrace ;
	lpszTemp = (LPSTR)szTemp ;			
	lpSend = (LPSTR)szSend ;			
	lpRecv = (LPSTR)szRecv ;			
	lpMessage = (LPSTR)message ;		
	lpIncoming = (LPSTR)incoming ;	
	lpiNoop = &iNoop ;
	lpFt = (LPFAKETICKET)&ft ;
	lpFakeParams = (LPSTR FAR *)lpszRecvList ;
	lpszCmd = (LPSTR)szCmd ;
	lpszHost = (LPSTR)szHost ;
	lpszErrorText = (LPSTR)szErrorText ;
	
//	if (WSAStartup (0x0101, &wskData))
//		MessageBox (NULL, "Could not start Windows Sockets stack!", NULL, MB_OK) ;

	GetSettings () ;
	return (1) ;

#if defined(_WIN32)
		case DLL_PROCESS_DETACH:
#else
	}

/* *********************************************************** 
	WEP
	*********************************************************** */
int KL_CALLTYPE KL_EXPORT WEP (int nParam)
{
#endif
	WSACleanup();		// shutdown WinSock
	return (1) ;
#if defined(_WIN32)
	} /*  end of switch statement (32-bit only) */
#endif
}


/* *********************************************************** 
	SendMsg () - send composed msg.
	*********************************************************** */
BOOL KL_CALLTYPE SendMsg (LPTC lpTc, LPSTR lpSend, int iLength) 
	{
	LPSTR lp1 ;
	
	assert (lpSend != NULL) ;
	assert (lpTc != NULL) ;	
	
	if (iLength > MAXMSG) 
		return (FALSE) ;
	
	// Do we have a valid socket yet?
	if (lpTc->socket == INVALID_SOCKET)	
		{
		if (Init(lpTc) == FALSE)			// try to initialize and connect
			{
			Trace ((LPSTR)"%s ERROR! Unable to initialize connection (%d)", lpszDLL, WSAGetLastError()) ;			
			return (FALSE) ;
			}
		}
	else		// see if the connection timed out, if so, try to reconnect
		{	
		FD_ZERO(&set) ;					// set select values
		FD_SET(lpTc->socket, &set) ;
		tv.tv_sec = 0 ;					
		tv.tv_usec = 0 ;
	
		// can we write on this socket?
		if (select(0, NULL, &set, NULL, &tv) == 0)	// no
			{
			FD_CLR (lpTc->socket, &set) ;		// clear select values
			if (bTraceL1 == TRUE)
				Trace ((LPSTR)"%s Connection timed out - reconnecting...", lpszDLL) ;

			if (Init(lpTc) == FALSE)			// try to initialize and connect
				{
				if (bTraceL1 == TRUE)
					Trace ((LPSTR)"%s ERROR! Unable to initialize connection (%d)", lpszDLL, WSAGetLastError()) ;
					
				return (FALSE) ;
				}	
			}
		}
	
	lp1 = lpSend ;							// line up at head of msg
	while (iLength > MAXSEND)        // if greater than MAXSEND, send next MAXSEND bytes
		{
		if(send(lpTc->socket, lp1, MAXSEND, 0) == SOCKET_ERROR)
			{
			if (bTraceL1 == TRUE)		
				Trace ((LPSTR)"%s ERROR sending on socket %d (%d)", 
						lpszDLL, lpTc->socket, WSAGetLastError()) ;
					
			return (FALSE) ;			
			}
			
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s Sent %d bytes on socket %d", lpszDLL, iLength, lpTc->socket) ;
		
		lp1 += MAXSEND	 ;				// move past section we just sent
		iLength -= MAXSEND ;			// track length counter
		}
		
	// we have less than MAXSEND bytes left, send them all
	if(send(lpTc->socket, lp1, iLength, 0) == SOCKET_ERROR)
		{		
		if (bTraceL1 == TRUE)		
			Trace ((LPSTR)"%s ERROR sending on socket %d (%d)", 
					lpszDLL, lpTc->socket, WSAGetLastError()) ;

		return (FALSE) ;			
		}
		

	return (TRUE) ;	
	}
	


/* *********************************************************** 
	RecvMsg () - recv a msg.  If ahssp drops the connection when done,
	then I don't need select(). Incoming msg gets placed in
	global buffer lpIncoming.  Returns: number of bytes recv()ed,
	0 if error.
	*********************************************************** */
UINT KL_CALLTYPE RecvMsg (LPTC lpTc)
	{
	LPSTR lp1 ;
	
	assert (lpTc != NULL) ;
	
	// initialize a few things
	iCnt = 0 ;											// bytes received
	lpTc->dwBytes = 0L ;								// size of incoming
	_fmemset (lpIncoming, '\0', MAXMSG) ;		// initialize incoming msg buffer

	// select values 
	FD_ZERO(&set) ;					// set select values
	FD_SET(lpTc->socket, &set) ;
	tv.tv_sec = 0 ;					
	tv.tv_usec = 0 ;
	
	// Make sure we are connected  
	if (lpTc->socket == INVALID_SOCKET)
		return (0) ;

	// if all goes well, we'll loop until we recv 0 bytes 
	lp1 = lpIncoming ;
	while (1)
		{
		// actually receive the incoming
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s Blocking to recv()...",	lpszDLL) ;
			
		iCnt = recv (lpTc->socket, szRecv, 512, 0) ;
		if (iCnt == 0)					// no more data
			break ;
						
		if (iCnt == SOCKET_ERROR)				// recv() error
			{
			if (bTraceL1 == TRUE)		
				Trace ((LPSTR)"%s ERROR reading data on socket %d (%d)", lpszDLL, lpTc->socket, WSAGetLastError()) ;
					
			// in case we got a partial msg
			return ((UINT)lpTc->dwBytes) ;
			}
			
		if (bTraceIncoming == TRUE)			// trace option
			Trace ((LPSTR)"%s Recv()ed %d bytes on socket %d.\n", lpszDLL, iCnt, lpTc->socket) ;
			
		_fmemmove (lp1, lpRecv, iCnt) ;		// copy in packet
		lp1 += iCnt ;								// advance incoming msg buffer pointer
		
		lpTc->dwBytes += (DWORD)iCnt ;				// track progress		
		
		// cheap bounds check
		if (lpTc->dwBytes > MAXMSG - 512)
			break ;

		if (select(0, &set, NULL, NULL, &tv) == 0)
			break ;						
			
      }

	// clear select values
	FD_CLR (lpTc->socket, &set) ;

	if (bTraceIncoming == TRUE)
		Trace ((LPSTR)"%s Total bytes received: %lu.\n", lpszDLL, lpTc->dwBytes) ;

	return ((UINT)lpTc->dwBytes) ;
	}


/* *********************************************************** 
	ComposeMsg - composed msg gets put into global buffer message
	Returns: number of bytes in msg - 0 on error.
	
   <eom> = 0xFFED          -- "end of message"
   <eoc> = 0xFFEF          -- "end of chunk"
   <bb> = 0xFFBB           -- "begin binary"
   <eb> = 0xFFEB           -- "end binary"
   <KTEXT> = kerberos authenticator

   example:  WSSEM<eoc>1<eoc><bb><KTEXT><eb><eoc><eom>
	*********************************************************** */
UINT KL_CALLTYPE  ComposeMsg (LPSTR lpszAlias, LPSTR lpszCommand, int iVersion, int iNoop, LPSTR FAR *lpParams)
	{
	BYTE FAR * lpB1 ;
	LPSTR 	lpS1 ;
	UINT		iMsgLength ;
	UINT		iAuthenticatorLength ;
	int		i ;
	
	assert (lpszAlias != NULL) ;
	assert (lpszCommand != NULL) ;
	assert (lpParams != NULL) ;
	
	// bounds check
   if (_fstrlen(lpszAlias) > MAXBUFF)
   	return (0) ;

   if (_fstrlen(lpszCommand) > MAXBUFF)
   	return (0) ;
   
   if (iNoop < 0)    		// bogus Noop
   	return (0) ;
	
	// initialize	
	lpS1 = lpMessage ;
	lpB1 = (BYTE FAR *)lpMessage ;
	iMsgLength = 0 ;
	iAuthenticatorLength = 0 ;
	_fmemset (lpMessage, '\0', MAXMSG) ;		// compose msg buffer
	
	// copy in command
	_fstrcpy (lpS1, lpszCommand) ;
	iMsgLength += _fstrlen(lpszCommand) ;
	lpS1 += _fstrlen(lpszCommand) ;
	lpB1 += _fstrlen(lpszCommand) ;
	
	// Eoc
	_fmemcpy (lpB1, &iEoc, sizeof(UINT)) ;
	iMsgLength += sizeof(UINT) ;
	lpS1 += sizeof(UINT) ;
	lpB1 += sizeof(UINT) ;
	
	// copy in version
	// convert version to string, I'd use itoa(), but it only does _near pointers	
	wsprintf (lpszTemp, "%d", iVersion) ;
	_fstrcpy (lpS1, lpszTemp) ;
	iMsgLength += _fstrlen(lpszTemp) ;
	lpS1 += _fstrlen(lpszTemp) ;
	lpB1 += _fstrlen(lpszTemp) ;
	
	// Eoc
	_fmemcpy (lpB1, &iEoc, sizeof(UINT)) ;
	iMsgLength += sizeof(UINT) ;
	lpS1 += sizeof(UINT) ;
	lpB1 += sizeof(UINT) ;
	
	// copy in authenticator	
	if (_fstrlen (lpTc->szService) > 0)				// skip if no service
		{
		iAuthenticatorLength = CopyInAuthenticator(lpS1) ;
		iMsgLength += iAuthenticatorLength ;
		lpS1 += iAuthenticatorLength ;
		lpB1 += iAuthenticatorLength ;
		}
	
	// Eoc
	_fmemcpy (lpB1, &iEoc, sizeof(UINT)) ;
	iMsgLength += sizeof(UINT) ;
	lpS1 += sizeof(UINT) ;
	lpB1 += sizeof(UINT) ;
	
	// loop through and get params
	for (i=0; i<iNoop; i++)
		{
		// copy in command
		_fstrcpy (lpS1, lpParams[i]) ;
		iMsgLength += _fstrlen(lpParams[i]) ;
		lpS1 += _fstrlen(lpParams[i]) ;
		lpB1 += _fstrlen(lpParams[i]) ;
	
		// Eoc
		_fmemcpy (lpB1, &iEoc, sizeof(UINT)) ;
		iMsgLength += sizeof(UINT) ;
		lpS1 += sizeof(UINT) ;
		lpB1 += sizeof(UINT) ;
		
		// cheap bounds check on overall msg size
		// if we get within 512 bytes of the end, bail out
		if (iMsgLength > (MAXMSG - 512))
			return (FALSE) ;
			
   	}
   	
	// Eom
	_fmemcpy (lpB1, &iEom, sizeof(UINT)) ;
	iMsgLength += sizeof(UINT) ;
	lpS1 += sizeof(UINT) ;
	lpB1 += sizeof(UINT) ;

	return (iMsgLength) ;	
	}

#if 1
/* *********************************************************** 
	DeComposeMsg() - break up the incoming ahssp msg, which should
	be waiting for us in lpIncoming.  Right now this can not
	decompose msgs that contain binary.  Note the number of parameters
	actually in the msg (including return code and text, is 1 less
	than the index.
	*********************************************************** */
BOOL KL_CALLTYPE DeComposeMsg (LPWORD lpiNoop, LPSTR FAR *lpParams, UINT iMsgLength) 
   {
   WORD	iIndex ;		// index into array of parameter strings
	LPBYTE lp1 ;
	LPBYTE lp2 ;
	LPSTR  lpNextString ;		// start of next sz parameter
	BOOL	bFoundAChunk ; 		// just left an EOC sequence
	LPSTR lpszReturnCode ;		// return code in ahssp msg
	LPSTR lpszText ;				// text of return code
	int x ;
		
	assert (lpiNoop != NULL) ;
	assert (lpParams != NULL) ;
	
	// initialize, just in case
	lpszReturnCode = (LPSTR)lpIncoming ;	// just so it is not dangling
	lpszText = (LPSTR)lpIncoming ;			// just so it is not dangling
	assert (lpszReturnCode != NULL) ;
	assert (lpszText != NULL) ;

	// other initializations	
	bFoundAChunk = FALSE ;		// first char should be a string
	iIndex = 0 ;
	x = 0 ;
	
	// test for ahssp msg validity
	if (IsValid (iMsgLength) == FALSE)
		return (FALSE) ;
	
	lp1 = (LPBYTE)lpIncoming ;		// start at head of msg
	lpNextString = (LPSTR)lp1 ;	// start at head of msg
	while (iMsgLength)
		{
		if (*lp1 == 0xff)			// found an IAC
			{
			lp2 = lp1 + 1 ;		// check next byte
			switch (*lp2)
				{
				case 0xEF:			// Eoc found
					x++ ;				// increment chunk counter
					*lp1 = '\0'	;	// replace IAC with NULL to make an sz
					if (x == 1)
						lpszReturnCode = lpNextString ;	// return code is 1st chunk
					else						
						if (x == 2)
							{
							lpszText	= lpNextString ;					// text is 2nd chunk
							_fstrcpy (lpszErrorText, lpszText) ;	// save error text
							}
						else													// the rest should be parameters
							{						
							lpParams[iIndex] = lpNextString ;	// store this parameter
							iIndex++ ;				
							}

					bFoundAChunk = TRUE ;
					if (bTraceL2 == TRUE)
						{
						Trace ((LPSTR)"%s End Of Chunk found: %s", lpszDLL) ;
						Trace ((LPSTR)"%s Chunk: %s", lpszDLL, lpNextString) ;
						}						
					break ;
					
				case 0xED:			// Eom found					
					// last minute sanity check
					if (x < 2)					// did not even get rc and text
						return (FALSE) ;				
						
					*lpiNoop = x - 2 ;		// 2 less than the chunks we found					
					
					// save return code
					_fstrcpy (lpszTemp, lpszReturnCode) ;										
					iLastRC = atoi (szTemp) ;
					
					if (bTraceL2 == TRUE)
						Trace ((LPSTR)"%s Return Code: %s. Text: %s", lpszDLL, lpszReturnCode, lpszText) ;
						
					if (bTraceL2 == TRUE)
						{
						Trace ((LPSTR)"%s End Of Message found: %s", lpszDLL) ;
						Trace ((LPSTR)"%s Number of parameters received: %d", lpszDLL, *lpiNoop) ;
						}
						
					return (TRUE) ;				
				}			
			}
			
		if (bFoundAChunk == TRUE)
			{
			lpNextString = lp1+2 ;			// we are defintely starting the next string
			bFoundAChunk = FALSE ;
			}

		lp1++ ;						// advance to next byte
		iMsgLength-- ;
		}
				
	// no Eom found if we made it this far.
	return (FALSE) ;	
						
   }
   
#endif

/* *********************************************************** 
	IsValid() - checks the reply in lpIncoming to see if it is 
	a valid ahssp msg.
	*********************************************************** */
BOOL KL_CALLTYPE IsValid(UINT iMsgLength)
	{
	LPBYTE lp1 ;
	LPBYTE lp2 ;
	
	lp1 = (LPBYTE)lpIncoming ;	// start at head of msg
	while (iMsgLength) 
		{
		if (*lp1 == 0xff)			// found an IAC
			{
			lp2 = lp1 + 1 ;		// check next byte
			if (*lp2 == 0xef)		// Eom found
				return (TRUE) ;
				
			}
		lp1++ ;						// advance to next byte
		iMsgLength-- ;				
		}

	// no Eom found if we made it this far.
	return (FALSE) ;	
	}

/* *********************************************************** 
	CopyInAuthenticator() - copy in Kerberos ticket, being sure
	to double all IACs.  Current service should be in the TC.
	lpB1 is the pointer to the actual ahssp msg being composed.
	Authenticator includes begin binary and end binary IAC sequences,
	but does not include end of chunk.
	
	Returns: number of bytes copyied into msg - 0 on error.
	*********************************************************** */
UINT KL_CALLTYPE CopyInAuthenticator (LPBYTE lpB1)
	{
	// copy in Fake Ticket
	_fstrcpy (lpB1, lpFt->szTicket) ;
	return (_fstrlen (lpFt->szTicket)) ;
	}

/* ***********************************************************
	Init ()-  Create socket and a remote association.
	
	Returns: TRUE if Tc is initialized okay. 
				FALSE if error.
	*********************************************************** */
BOOL KL_CALLTYPE Init (LPTC lpTc)
	{
	assert (lpTc != NULL) ;

	lpTc->r_asso.sin_family = PF_INET ;			// the usual
	
	if (bTraceL1 == TRUE)						// trace option
		Trace ((LPSTR)"%s Resolving host name: %s", lpszDLL, lpTc->szHost) ;
		
	// assume machine name is an address
	lpTc->r_asso.sin_addr.s_addr = inet_addr (lpTc->szHost) ;	
	if (lpTc->r_asso.sin_addr.s_addr == INADDR_NONE)				
	   {		// If ascii addr conversion fails, it's probably a host name
		if((lph = gethostbyname(lpTc->szHost)) == NULL)
			{
			if (bTraceL1 == TRUE)
				Trace ((LPSTR)"%s ERROR! Unknown host: %s", lpszDLL, lpTc->szHost) ;

			return (FALSE) ;
			}		
		_fmemcpy((char FAR *) &lpTc->r_asso.sin_addr.s_addr, lph->h_addr, lph->h_length) ;
		}

	// port
	lpTc->r_asso.sin_port = htons (lpTc->iPort) ;
	if (bTraceL1 == TRUE)					// trace option
		Trace ((LPSTR)"%s Port: %d.", lpszDLL, lpTc->iPort) ;
		
	// actually make the socket
	if((lpTc->socket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s ERROR! Unable to create a socket", lpszDLL) ;

		return (FALSE) ;
		}
	if (bTraceL1 == TRUE)
		Trace ((LPSTR)"%s Created socket: %d.", lpszDLL, lpTc->socket) ;

	// connect to remote association		
	if (bTraceL1 == TRUE)
		Trace ((LPSTR)"%s Trying to connect to host %s on socket %d ", lpszDLL, lpTc->szHost, lpTc->socket) ;
		
	if(connect(lpTc->socket, (LPSOCKADDR)&lpTc->r_asso, sizeof(SOCKADDR_IN)))
		{
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s ERROR! Unable connect to host %s on socket %d ",
					lpszDLL, lpTc->szHost, lpTc->socket) ;
	
		closesocket(lpTc->socket) ;         // free up socket
		lpTc->socket = INVALID_SOCKET ;      
		lpTc->connect = FALSE ;					// not connected
		return (FALSE) ;
		}		
		
	lpTc->connect = TRUE ;						// successfully connected	
	if (bTraceL1 == TRUE)
		Trace ((LPSTR)"%s OK! Connected to host %s on socket: %d", lpszDLL, lpTc->szHost, lpTc->socket) ;
	
	return (TRUE) ;
	}

/* *********************************************************** 
	Abort () - 
	*********************************************************** */
void KL_CALLTYPE KL_EXPORT Abort (void)
	{
	assert (lpTc != NULL) ;
	
	if (bTraceL1 == TRUE)
		Trace ((LPSTR)"%s Closing active transaction: %s", lpszDLL, lpTc->szAlias) ;

	if (WSAIsBlocking () == TRUE)			// Are we currently blocking?
		WSACancelBlockingCall () ;			// Not any more :)

	if (lpTc->socket != INVALID_SOCKET) 
		{
		shutdown (lpTc->socket, 3) ;		
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s Shutdown on socket: %d", lpszDLL, lpTc->socket) ;
		}
	
	if (lpTc->socket != INVALID_SOCKET) 
		{
		closesocket(lpTc->socket) ;			// close socket
		if (bTraceL1 == TRUE)
			Trace ((LPSTR)"%s Close socket: %d", lpszDLL, lpTc->socket) ;
		}
		
	lpTc->socket = INVALID_SOCKET ;			// back to invalidity
	lpTc->connect = FALSE ;						// not connected
	
	}

/* *********************************************************** 
	TraceToWindow () - TRUE: use SendMessage() to direct all trace 
	messages to the window specified by hWndCello.  FALSE disables 
	this type of tracing.  The state is tracked by global variable
	bTraceToWindow.  Tracing to _a window_ from each DLL will
	be globally enabled/disabled from here.
	*********************************************************** */
BOOL KL_CALLTYPE KL_EXPORT TraceToWindow (BOOL bState, HWND hWndCello)
	{
	assert (hWndCello != 0x00) ;		// make sure we have a Window handle
	
	if (bState == TRUE)
		{
		bTraceToWindow = TRUE ;
		hWndTrace = hWndCello ;
		return (TRUE) ;
		}
	else
		{
		bTraceToWindow = FALSE ;
		hWndTrace = 0x00 ;			// zero this for assert()tion protection
		return (FALSE) ;
		}
	}

/* *********************************************************** 
	Trace () - used as a "front end" to Trace(), allowing a
	variable number of parameters to be passed and "prinf()" style 
	substitued into a single sz.  I can't do this directly with
	Trace, because it is KL_EXPORTed, and needs to be KL_CALLTYPE, and
	only cdecl calling conventions can accept variable number of 
	arguments. 
	*********************************************************** */
BOOL Trace (LPSTR lpszFmt, ...)
	{
	LPSTR lpArg ;
	LPSTR lp1 ;

	assert (lpszFmt != NULL) ;
	if (_fstrlen(lpszFmt) > MAXTRACELEN - 4)		// trace string too long? (bounds overrun?)
		return (FALSE) ;

	lpArg = (LPSTR)&lpszFmt ;		// get address of format string
	lpArg += 4 ;						// walk past it
	wvsprintf (lpszTrace, lpszFmt, lpArg) ;	// wsprintf()-style to buffer

	if (_fstrlen(lpszTrace) > (MAXTRACELEN - 4))	// trace string too long? (bounds overrun?)
		{
		lp1 = lpszTrace +  MAXTRACELEN - 34 ;		// make room the truncation "tag" (...)
		*lp1 = '\0' ;	                              // cut it short
		_fstrcat (lpszTrace, "(...)") ;				// show that we chopped it
		}

	// remove all (embedded) newline characters for easy trace reading 
	lp1 = lpszTrace ;
	while (lp1 != NULL)
		{
		if ((lp1 = _fstrchr (lpszTrace, '\n')) != NULL)
			*lp1 = ' ' ;
		}

	_fstrcat (lpszTrace, "\n") ;		// add newline only to end 
	OutputDebugString (lpszTrace) ;	// Send to debug window	

	// send to application trace window, if enabled
	if (bTraceToWindow == TRUE)
		{	
		// now take '\n' back out for hWnd trace (TextOut())
		if ((lp1 = _fstrchr (lpszTrace, '\n')) != NULL)
			*lp1 = ' ' ;
		
		if (hWndTrace == 0x00)		// make sure we have a Window handle
			return (FALSE) ;
		// post it to the application's trace window
		PostMessage (hWndTrace, YYZ_TRACE, 0, (LPARAM)(LPSTR)lpszTrace) ;
		}

	return (TRUE) ;
	}
	
/* -------------------------------------------------------------------
	BounceTicketOffServer - 
	------------------------------------------------------------------- */
BOOL KL_CALLTYPE BounceTicketOffServer (void)
	{
	UINT	iMsgLength ;
	BOOL 	bReturn ;
	
	_fstrcpy (lpTc->szService, (LPSTR)"FakeService") ;
	_fstrcpy (lpTc->szHost, lpszHost) ;
	lpTc->iPort = iPort ;
	lpTc->socket = INVALID_SOCKET ;	

	bReturn = FALSE ;
	if(WSAStartup(0x0101, &wskData) != 0)
		return (FALSE) ;

	iMsgLength = ComposeMsg ((LPSTR)"FakeAlias", lpszCmd, iVer, 0, lpFakeParams) ;
	if (iMsgLength > 0) 
		if (Init(lpTc))
			if (SendMsg (lpTc, lpMessage, iMsgLength))
				{
				iMsgLength = RecvMsg (lpTc) ;
				if (iMsgLength > 0)
					if (DeComposeMsg (lpiNoop, lpFakeParams, iMsgLength))						
						if (iLastRC == 0)
							{
							lpFt->bAuthenticated = TRUE ;
							bReturn = TRUE ; 
							}
						else		// got messaage back from server, but there is error text
							{
							wsprintf (lpszTemp, "%s (%d).", lpszErrorText, iLastRC) ;
							MessageBox (NULL, lpszTemp, NULL, MB_OK) ;
							}
				}
	
	Abort () ;		// the short cut to cleaning up
	return (bReturn) ;		
	}

/* -------------------------------------------------------------------
	GetSettings ()
	------------------------------------------------------------------- */
void KL_CALLTYPE GetSettings (void)
	{
	LoadString (hInst, IDS_HOST, lpszHost, MAXBUFF-1) ;
	LoadString (hInst, IDS_PORT, lpszTemp, MAXBUFF-1) ;
	iPort = atoi (szTemp) ;
	
	LoadString (hInst, IDS_COMMAND, lpszCmd, MAXBUFF-1) ;
	LoadString (hInst, IDS_VERSION, lpszTemp, MAXBUFF-1) ;	
	iVer = atoi (szTemp) ;
	}
	
