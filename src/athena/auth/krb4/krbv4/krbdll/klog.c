/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <windows.h>
#include <time.h>
#include <stdio.h>

#include <krb.h>
#include <klog.h>

static char *log_name = KRBLOG;
static int is_open;

char *month_sname();

/*
 * This file contains two logging routines: kset_logfile()
 * to determine the file to which log entries should be written;
 * and klog() to write log entries to the file.
 */

/*
 * klog() is used to add entries to the logfile (see kset_logfile()
 * below).  Note that it is probably not portable since it makes
 * assumptions about what the compiler will do when it is called
 * with less than the correct number of arguments which is the
 * way it is usually called.
 *
 * The log entry consists of a timestamp and the given arguments
 * printed according to the given "format" string.
 *
 * The log file is opened and closed for each log entry.
 *
 * If the given log type "type" is unknown, or if the log file
 * cannot be opened, no entry is made to the log file.
 */

void
klog(
    char *format,
    ...
    )
{
    FILE *logfile;
    long now;
    struct tm *tm;
    char *month_sname();
    va_list ap;

    va_start(ap, format);
    if ((logfile = fopen(log_name,"a")) == NULL)
        return;

    time(&now);
    tm = localtime(&now);

    fprintf(logfile,"%2d-%s-%02d %02d:%02d:%02d ",tm->tm_mday,
            month_sname(tm->tm_mon + 1),tm->tm_year,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    vfprintf(logfile, format, ap);
    fprintf(logfile, "\n");
    fclose(logfile);
    return;
}

/*
 * kset_logfile() changes the name of the file to which
 * messages are logged.  If kset_logfile() is not called,
 * the logfile defaults to KRBLOG, defined in "krb.h".
 */

kset_logfile(filename)
    char *filename;
{
    log_name = filename;
    is_open = 0;
}
