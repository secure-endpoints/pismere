
/* This is ripped off from MIT's Leash source.  I've replaced the
	com_err lines with MessageBox()es, removed the Leash dependencies 
	as well as the mysterious setjmp	references.  
	
	-- SSE:  5/23/95.
*/

/* WARNING: in Win32, this code calls gmtime, which has an internal
   static variable.  use of the multi-threaded run-time libraries is
   strongly urged. */

/* under Windows NT, only users who have the appropriate user rights
   can change the system time.  see the Policies menu of the User
   Manager.  this is relevant only on a multi-user NT workstation,
   as a sole user is highly likely to be an Administrator and 
   therefore has this right by default.  I don't call
   AdjustTokenPrivileges because that requires a more powerful
   right than merely setting the system time. */

/* timesync stuff for leash - 7/28/94 - evanr */

#include <windows.h>
#if !defined(_WINSOCKAPI_)
#include <winsock.h>
#endif
#include <time.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <pcrt32.h>
#endif

// sse: #include <mit/com_err.h>
// sse: #include <setjmp.h>
// #include "leasherr.h"
// #include "leashids.h"
// extern HANDLE hThisInstance;

#define TM_OFFSET 2208988800

struct timezone 	/* timezone.h has a winsock.h conflict */
	{		
	int     tz_minuteswest;
	int     tz_dsttime;
	};

// #define DEFAULT_TIME_SERVER	   "time"
// char DEFAULT_TIME_SERVER[255] = { NULL };
char DEFAULT_TIME_SERVER[255] ;

// sse: some needed KView info
extern LPSTR lpszApp ;				// applicaton name
extern HWND	gWnd ;					// main window
extern char	szTimeServer[128] ;	// hostname of time server


int settimeofday (struct timeval *tv,struct timezone *tz) 
	{
#if defined(_WIN32)
   SYSTEMTIME bigTime;
   struct tm* myCUT;
//   OSVERSIONINFO myVInfo;

   /* in this case, we want to STAY in universal time anyhow */
   myCUT = gmtime(&(tv->tv_sec));
   if (myCUT == NULL)
	   return 0;

   bigTime.wYear = 1900+myCUT->tm_year;
   bigTime.wMonth = 1+myCUT->tm_mon;
   /* wDayOfWeek is supposedly ignored for SetSystemTime, but we have it.. */
   bigTime.wDayOfWeek = myCUT->tm_wday;
   bigTime.wDay = myCUT->tm_mday;
   bigTime.wHour = myCUT->tm_hour;
   bigTime.wMinute = myCUT->tm_min;
   bigTime.wSecond = myCUT->tm_sec;
   bigTime.wMilliseconds = 0;
   /* under NT, SetSystemTime requires privileges */
   /* myVINfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&myVInfo);
   if (myVInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) */
   if (!SetSystemTime(&bigTime))
	   return 0;
#else
   struct _dostime_t dostime;      
   struct _dosdate_t dosdate;
   struct tm *newtime;

   newtime = localtime((time_t *)&(tv->tv_sec));
   dostime.hour = newtime->tm_hour;
   dostime.minute = newtime->tm_min;   
   dostime.second = newtime->tm_sec;
   dostime.hsecond = 0;
   dosdate.day = newtime->tm_mday;
   dosdate.month = 1+newtime->tm_mon; 		/* burp! Microsft really sucks */
   dosdate.year = 1900+newtime->tm_year;	/* burp! Microsoft sucks again... */
   dosdate.dayofweek = newtime->tm_wday;
	
   if (_dos_setdate(&dosdate) == 0 && _dos_settime(&dostime) == 0)
   	return 0;
#endif
   	
	return 1;    
	}       

int gettimeofday (struct timeval *tv,struct timezone *tz) 
	{
	time_t long_time;

	if (getenv ("TZ") == NULL)  /* Default Cambridge time [forget Seattle!!] */
		{
		_timezone = 18000;
		_daylight = 1;
		_tzname[0] = "EST";
		_tzname[1] = "EDT";
		}
	else
		_tzset();
		
	tz->tz_minuteswest = (int)_timezone;
	tz->tz_dsttime = _daylight;
	time(&long_time);
	tv->tv_sec = long_time;
	tv->tv_usec = 0;
	return 0;       
	}       

void timesync()
	{
	char buffer[512];
	struct timeval tv;
	struct timezone tz;
//	jmp_buf top_level;
	struct sockaddr_in sin;
	struct servent FAR * sp;
	struct hostent FAR *host;
	register int s;
	long hosttime;
	register long *nettime; /* used to be an int */
	char tmpstr[200], hostname[64];
	int attempts = 0, cc, host_retry;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	//	int check;
	static BOOL bDoneOnce = FALSE;

	if (bDoneOnce)
		return;

	wVersionRequested = 0x0101;
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
		{
		wsprintf(tmpstr, "Couldn't initialize WinSock to synchronize time\n\rError Number: %d", err);
		// com_err("Leash", LSH_BADWINSOCK, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}
		
	/* OK, now try the resource -- to be added */
	sp = getservbyname("time", "udp");
	if (sp == 0)
		sin.sin_port = htons(IPPORT_TIMESERVER);
	else
		sin.sin_port = sp->s_port;
		
	host_retry = 0;
	//	check = LoadString( hThisInstance , LSH_TIME_HOST, &DEFAULT_TIME_SERVER[0], sizeof(DEFAULT_TIME_SERVER) );
	strcpy (hostname, szTimeServer);		// sse:

//	if( check != 0 )
//		{
//	    strcpy (hostname, DEFAULT_TIME_SERVER);
//		} 
//	else 
//		{
//	    strcpy (hostname, (LPSTR) "time");
//		}

	while (host_retry < 5) 
		{
		host = gethostbyname(hostname); /* Ok for DOS */
		if ((host != NULL) && WSAGetLastError() != WSATRY_AGAIN) 
			{
	   	host_retry = 5;
			}
			
		host_retry++;
		}
		
	if (host == NULL) 
		{
		wsprintf (tmpstr, "The timeserver host %s is unknown\n", (LPSTR)hostname);
		//	com_err("Leash", LSH_BADTIMESERV, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}
		
	sin.sin_family = host->h_addrtype;
	_fmemcpy ((struct sockaddr *)&sin.sin_addr, host->h_addr, host->h_length);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) 
		{
		wsprintf(tmpstr, "Couldn't open socket - Error: %d", WSAGetLastError());
		//	com_err("Leash", LSH_NOSOCKET, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ; 	// sse
		WSACleanup();
		return;
		}

	if (connect (s, (struct sockaddr *)&sin, sizeof (sin)) < 0) 
		{
		wsprintf(tmpstr, "Couldn't connect to timeserver - Error: %d", WSAGetLastError());
		// com_err("Leash", LSH_NOCONNECT, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}
		
//	setjmp(top_level);
	if (attempts++ > 5) 
		{
		closesocket (s);
		wsprintf (tmpstr, "Failed to get time from %s - Error: %d", (LPSTR)hostname, WSAGetLastError());
		// com_err("Leash", LSH_TIMEFAILED, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}        
		
	send (s, buffer, 40, 0); /* Send an empty packet */
	if (gettimeofday (&tv, &tz) < 0) 
		{
		//	com_err("Leash", LSH_GETTIMEOFDAY, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}

	cc = recv (s, buffer, 512, 0);
	if (cc < 0) 
		{
		wsprintf(tmpstr, "Error receiving time from %s - Error: %d", (LPSTR)hostname, WSAGetLastError());
		// com_err("Leash", LSH_RECVTIME, tmpstr);
		closesocket(s);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}

	if (cc != 4) 
		{
		closesocket(s);
		wsprintf(tmpstr, "Protocol error -- received %d bytes; expected 4", 	cc);
		// com_err("Leash", LSH_RECVBYTES, tmpstr);
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}
		
	nettime = (long *)buffer;
	hosttime = (long) ntohl (*nettime) - TM_OFFSET;
	(&tv)->tv_sec = hosttime;
	if (settimeofday (&tv, &tz) < 0) 
		{
		// com_err("Leash", LSH_SETTIMEOFDAY, "Couldn't set local time of day.");
		MessageBox (gWnd, (LPSTR)tmpstr, lpszApp, MB_OK) ;		// sse
		WSACleanup();
		return;
		}

	bDoneOnce = TRUE;

	strcpy (tmpstr, "To be able to use the Kerberos server, it was necessary to set the system time to:  ") ;		// sse
	strcat(tmpstr, ctime((time_t *)&hosttime));				// sse
	
//	strcpy(tmpstr, ctime((time_t *)&hosttime));
	tmpstr[strlen(tmpstr)-1] = '\0';
//	MessageBox(NULL, tmpstr, "Server Time", MB_APPLMODAL | MB_ICONINFORMATION | MB_OK);
	MessageBox(NULL, tmpstr, "Kerberos Client", MB_APPLMODAL | MB_OK);
	closesocket(s);
	WSACleanup();
	return;
	}
