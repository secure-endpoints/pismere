/*
 * key_test.c
 *
 * Copyright 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * exit status:          0 ==> success
 *                      -1 ==> error
 */

#include "mit-copyright.h"
#include <stdio.h>
#include <errno.h>
#include "des.h"

#define MIN_ARGC        0       /* min # args, not incl flags */
#define MAX_ARGC        99      /* max # args, not incl flags */

extern int des_key_sched();
char *progname;
int sflag;
int vflag;
int kflag;
int mflag;
int pid;
extern int des_debug;

unsigned long dummy[2];
unsigned char dum_c[8] = { 0x80,1,1,1,1,1,1,1 };
des_key_schedule KS;
des_cblock kk;

main(argc,argv)
    int argc;
    char *argv[];
{
    /*  Local Declarations */

    int i;
    progname=argv[0];           /* salt away invoking program */

    /* Assume a long is four bytes */
    if (sizeof(long) != 4) {
	fprintf(stderr,"\nERROR,  size of long is %d",sizeof(long));
	exit(-1);
    }

    while (--argc > 0 && (*++argv)[0] == '-')
	for (i=1; argv[0][i] != '\0'; i++) {
	    switch (argv[0][i]) {

		/* debug flag */
	    case 'd':
		des_debug=1;
		continue;

		/* keys flag */
	    case 'k':
		kflag = 1;
		continue;

		/* test ANSI msb only key */
	    case 'm':
		mflag = 1;
		continue;

	    default:
		printf("%s: illegal flag \"%c\" ",
		       progname,argv[0][i]);
		exit(1);
	    }
	};

    if (argc < MIN_ARGC || argc >MAX_ARGC) {
	printf("Usage: xxx [-xxx]  xxx xxx\n");
	exit(1);
    }

    /*  argv[0] now points to first non-option arg, if any */


    if (des_debug) {
	if (mflag) {
	    fprintf(stderr,"\nChecking a key 0x 80 01 01 01 01 01 01 01 ");
	    fprintf(stderr,"\nKey = ");
	    des_key_sched(dum_c,KS);
	    des_cblock_print (dum_c);
	    exit(0);
	}

	if (kflag) {
	    printf("\nChecking a weak key...");
	    dummy[0] =  0x01fe01fe;
	    dummy[1] =  0x01fe01fe;
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);

	    dummy[0] = 0x01010101;
	    dummy[1] = 0x01010101;
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKS= %lx",* (long *)KS);
	    dummy[0] = 0x01010101;
	    dummy[1] = 0x01010101;
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKS= %lx", *(long *)KS);

	    dummy[0] = 0x80808080;
	    dummy[1] = 0x80808080;
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKS[0]= %lx",* (long * ) KS);

	    printf("\nstring to key 'a'");
	    des_string_to_key("a",dummy);
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKS= %lx",* (long *) KS);

	    printf("\nstring to key 'c'");
	    des_string_to_key("c",dummy);
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	    des_key_sched(dummy,KS);
	    fprintf(stderr,"\nKS= %lx", * (long * ) KS);
	}

	printf("\nstring to key 'e'");
	des_string_to_key("e",dummy);
	    fprintf(stderr,"\nKey[0] = %lx Key[1] = %lx",
			(long)dummy[0], (long)dummy[1]);
	des_key_sched(dummy,KS);
	fprintf(stderr,"\nKS= %lx", ((long*)KS)[0]);

	printf("\ndes_string_to_key '%s'",argv[0]);
	des_string_to_key(argv[0],dummy);
#ifdef notdef
	des_string_to_key(argv[0],dummy);

	for (i =0; i<1; i++)
	    des_key_sched(dummy,KS);
    }
    else
    {
	for (i =0; i<1000; i++) {
	    des_string_to_key(argv[0],kk);
	    des_key_sched(kk,KS);
	}
#endif
    }
    exit(0);
}
