/*
 * read_password.c
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * This routine prints the supplied string to standard
 * output as a prompt, and reads a password string without
 * echoing.
 */

#include "mit_copy.h"
#include "des.h"
#include "conf.h"

#include <stdio.h>
#ifdef BSDUNIX
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <setjmp.h>
#else
#ifdef PC
#include <string.h>
#include <conio.h>
#else
char     *strcpy();
int      strcmp();
#endif
#endif

#ifdef	POSIX
#include <termios.h>
#endif

#ifdef	BSDUNIX
static jmp_buf env;
#endif

#ifdef BSDUNIX
static sigtype sig_restore();
static push_signals(), pop_signals();
int des_read_pw_string();
#endif

#ifdef GUIONLY
#include "wpasswd.h"
HANDLE  FAR PASCAL hQKrbInst( void );
#endif

/*** Routines ****************************************************** */
int
des_read_password(k,prompt,verify)
    des_cblock *k;
    char *prompt;
    int	verify;
{
    int ok;
    char key_string[BUFSIZ];

#ifdef BSDUNIX
    if (setjmp(env)) {
	ok = -1;
	goto lose;
    }
#endif

    ok = des_read_pw_string(key_string, BUFSIZ, prompt, verify);
    if (ok == 0)
	des_string_to_key(key_string, k);

lose:
    memset(key_string, 0, sizeof (key_string));
    return ok;
}


#ifdef GUIONLY
char FAR *key_string;
#endif

#ifdef PC
static void gets_no_echo(char *s,int size)
{
#ifndef GUIONLY
        char *end=s+size-1,*p=s;
        while (s<end && (*s=getch()) != '\r')
                s++;
        *s='\0';
        putch('\r');
        putch('\n');
#else
        HWND hActive;
        HWND hInst;
        FARPROC lpDlgProc;
        int try = 0;

	key_string=s;

        hActive = GetActiveWindow();
        hInst = hQKrbInst();

        lpDlgProc = MakeProcInstance( PasswordCB, hInst );
        DialogBox(  hInst, "KERB_PASSWDBOX", hActive, lpDlgProc );

        FreeProcInstance( lpDlgProc );
#endif
}
#endif

/*
 * This version just returns the string, doesn't map to key.
 *
 * Returns 0 on success, non-zero on failure.
 */

int
des_read_pw_string(s,max,prompt,verify)
    char *s;
    int	max;
    char *prompt;
    int	verify;
{
    int ok = 0;
    char *ptr;

#ifdef POSIX
    struct termios old_tty_state;
    struct termios tty_state;
#endif
#ifdef BSDUNIX
    jmp_buf old_env;
#ifndef POSIX
    struct sgttyb old_tty_state;
    struct sgttyb tty_state;
#endif
#endif
    char key_string[BUFSIZ];

    if (max > BUFSIZ) {
	return -1;
    }

#ifdef	BSDUNIX
#ifdef POSIX
    /* save terminal state */
    if (tcgetattr(0, &tty_state) == -1)
	return -1;
    if (tcgetattr(0, &old_tty_state) == -1)
	return -1;
    /* Turn off echo */
    tty_state.c_lflag &= ~ECHO;
    if (tcsetattr(0, TCSANOW, &tty_state) == -1)
	return -1;
#else
    /* save terminal state*/
    if (ioctl(0,TIOCGETP,(char *)&tty_state) == -1) 
	return -1;
    if (ioctl(0,TIOCGETP,(char *)&old_tty_state) == -1) 
	return -1;
    /* Turn off echo */
    tty_state.sg_flags &= ~ECHO;
    if (ioctl(0,TIOCSETP,(char *)&tty_state) == -1)
	return -1;
#endif
    /* XXX assume jmp_buf is typedef'ed to an array */
    memcpy((char *)env, (char *)old_env, sizeof(env));
    if (setjmp(env))
	goto lose;
    push_signals();
#endif

    while (!ok) {
	(void) printf(prompt);
	(void) fflush(stdout);
#ifdef	CROSSMSDOS
	h19line(s,sizeof(s),0);
	if (!strlen(s))
	    continue;
#else
#ifdef PC
	gets_no_echo(s, max);
	if (!strlen(s))
	  continue;
#else
	if (!fgets(s, max, stdin)) {
	    clearerr(stdin);
	    continue;
	}
#endif
	if ((ptr = strchr(s, '\n')))
	    *ptr = '\0';
#endif
	if (verify) {
	    printf("\nVerifying, please re-enter %s",prompt);
	    (void) fflush(stdout);
#ifdef CROSSMSDOS
	    h19line(key_string,sizeof(key_string),0);
	    if (!strlen(key_string))
		continue;
#else
#ifdef PC
	    gets_no_echo(key_string,sizeof(key_string));
	    if (!strlen(key_string))
	      continue;
#else
	    if (!fgets(key_string, sizeof(key_string), stdin)) {
		clearerr(stdin);
		continue;
	    }
#endif
            if ((ptr = strchr(key_string, '\n')))
	    *ptr = '\0';
#endif
	    if (strcmp(s,key_string)) {
		printf("\n\07\07Mismatch - try again\n");
		(void) fflush(stdout);
		continue;
	    }
	}
	ok = 1;
    }

lose:
    if (!ok)
	memset(s, 0, max);
    printf("\n");
#ifdef	BSDUNIX
#ifdef POSIX
    if (tcsetattr(0, TCSANOW, &old_tty_state))
	ok = 0;
#else
    if (ioctl(0,TIOCSETP,(char *)&old_tty_state))
	ok = 0;
#endif
    pop_signals();
    memcpy((char *)old_env, (char *)env, sizeof(env));
#endif
    if (verify)
	memset(key_string, 0, sizeof (key_string));
    s[max-1] = 0;		/* force termination */
    return !ok;			/* return nonzero if not okay */
}

#ifdef	BSDUNIX
/*
 * this can be static since we should never have more than
 * one set saved....
 */
static sigtype (*old_sigfunc[NSIG])();

static push_signals()
{
    register int i;
    for (i = 0; i < NSIG; i++)
	old_sigfunc[i] = signal(i,sig_restore);
}

static pop_signals()
{
    register int i;
    for (i = 0; i < NSIG; i++)
	(void) signal(i,old_sigfunc[i]);
}

static sigtype
sig_restore()
{
    longjmp(env,1);
}
#endif
