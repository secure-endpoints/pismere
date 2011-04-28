/************************************************
 timeoday.c

 gettimeofday replacement
 Note that it ignores the timezone parameter

 by Peter Tan
************************************************/
#include <windows.h>
#include <timeval.h>
#include <sys\timeb.h>
#include <time.h>
    
/* timezone.h has a winsock.h conflict */
struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

int gettimeofday (struct timeval *tv, struct timezone *tz)
{
    struct _timeb tb;
    _tzset();
    _ftime(&tb);
    if (tv) {
	tv->tv_sec = tb.time;
	tv->tv_usec = tb.millitm * 1000;
    }
    if (tz) {
	tz->tz_minuteswest = tb.timezone;
	tz->tz_dsttime = tb.dstflag;
    }
    return 0;
}
