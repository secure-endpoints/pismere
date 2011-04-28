/*
 * Copyright 1987, 1988 by MIT Student Information Processing Board.
 *
 * For copyright info, see mit-sipb-copyright.h.
 */

/*
 * This version is totally hacked because this is DOS, not UNIX.
 */

#include <stdio.h>
#include "mitsipbc.h"
#include <stdarg.h>
#if defined(WINDOWS)
#include <windows.h>
#elif defined(MEWEL)
#include <window.h>
#endif
#include "errtbl.h"
#include "internal.h"
#include "com_err.h"

#ifndef OLD
int com_err_va (LPSTR whoami, long code, LPSTR fmt, va_list args);
#endif

static int default_com_err_proc (LPSTR whoami, long code,
				 LPSTR fmt, va_list args)
{
#if defined(WINDOWS) || defined(MEWEL)
    int retval;
    HWND hOldFocus;
    char buf[200], *cp;
    WORD mbformat = MB_OK | MB_ICONINFORMATION;

    cp = buf;
    cp[0] = '\0';

    if (code)
      {
	lstrcpy(buf, error_message(code));
	while (*cp)
	  cp++;
      }

    if (fmt)
      {
	if (fmt[0] == '%' && fmt[1] == 'b')
	  {
	    fmt += 2;
	    mbformat = va_arg(args, WORD);
		/* if the first arg is a %b, we use it for the message
		   box MB_??? flags. */
	  }
	if (code)
	  {
	    *cp++ = '\n';
	    *cp++ = '\n';
	  }
	wvsprintf((LPSTR)cp, fmt, args);
      }
    hOldFocus = GetFocus();
    retval = MessageBox((HWND)NULL, buf, whoami, mbformat | MB_TASKMODAL);
    SetFocus(hOldFocus);
    return retval;
#else
    if (whoami) {
	fputs(whoami, stderr);
	fputs(": ", stderr);
    }
    if (code) {
	fputs(error_message(code), stderr);
	fputs(" ", stderr);
    }
    if (fmt) {
	vfprintf (stderr, fmt, args);
    }
    putc('\n', stderr);
    /* should do this only on a tty in raw mode */
    putc('\r', stderr);
    fflush(stderr);
    return 0;
#endif
}

typedef int (FAR *errf) ( LPSTR, long,  LPSTR, va_list);

errf com_err_hook = default_com_err_proc;

int com_err_va (LPSTR whoami, long code, LPSTR fmt, va_list args)
{
    return (*com_err_hook) (whoami, code, fmt, args);
}

#ifdef WIN16
int COMEXP com_err_export (LPSTR whoami, long code, LPSTR fmt, ...)
#else
int com_err (LPSTR whoami, long code, LPSTR fmt, ...)
#endif
{
    va_list pvar;
    int retval;

    if (!com_err_hook)
	com_err_hook = default_com_err_proc;

    va_start(pvar, fmt);
    retval = com_err_va (whoami, code, fmt, pvar);
    va_end(pvar);
    return retval;
}

int mbprintf (LPSTR whoami, LPSTR fmt, ...)
{
  va_list pvar;
  int retval;

  va_start(pvar, fmt);
  retval = default_com_err_proc(whoami, 0, fmt, pvar);
  va_end(pvar);
  return retval;
}

errf set_com_err_hook (new_proc)
    errf new_proc;
{
    errf x = com_err_hook;

    if (new_proc)
	com_err_hook = new_proc;
    else
	com_err_hook = default_com_err_proc;

    return x;
}

errf reset_com_err_hook () {
    errf x = com_err_hook;
    com_err_hook = default_com_err_proc;
    return x;
}
