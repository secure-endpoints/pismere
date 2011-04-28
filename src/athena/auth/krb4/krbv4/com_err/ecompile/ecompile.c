/*
 *
 * Adapted from the compile_et program (Copyright 1986, 1987, 1988
 * by MIT Student Information Processing Board)
 *  by Jason M. Sachs
 *  Feb. 1993
 *
 */

#include <stdio.h>

static const char char_set[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

static char buf[6];

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int char2no(char c)
{
  if (c >= 'A' && c <= 'Z')
    return (c - 'A') + 1;
  if (c >= 'a' && c <= 'z')
    return (c - 'a') + 27;
  if (c >= '0' && c <= '9')
    return (c - '0') + 53;
  if (c >= '_')
    return 63;
  return 0;
}

int valid_id_string(char *s)
{
  while (*s)
    if (char2no(*s++) == 0)
      return FALSE;
  return TRUE;
}

long error_table_base(char *name)
{
    long etb;
    char *cp;
    int charno;

    for (etb = 0, cp = name; *cp && (cp - name) < 4; cp++)
      {
	etb <<= 6;
	charno = char2no(*cp);

	if (charno == 0)
	  return -1;

	etb += charno;
      }

    return (etb << 8);
}

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXPATHLEN 120 /* hmm...? */

int strcasecmp(char *s1, char *s2)
{
  while (*s1 && toupper(*s1) == toupper(*s2))
    s1++, s2++;
  return (*s1); /* if they're the same, *s1 should be '\0',
	           and thus strcasecmp will be 0. */
}

int main (int argc, char **argv)
{
    char *p, *filename, *ename;
    char c_file[MAXPATHLEN], h_file[MAXPATHLEN];
    int len;
    FILE *infile, *cfile, *hfile;

    char etablename[5];
#define IDSZ 32
#define BUFSZ 256
    char buf[BUFSZ + 1], *bufp;
    char c, *cp;

#define WTFLEN 20

#define declare_str(name) \
  static char str_##name[] = #name

declare_str(error_table);
declare_str(error_code);
declare_str(end);

#define S_ERRORTABLE		1
#define S_ERRORTABLENAME	2
#define S_ERRORCODE		3
#define S_ERRORCODEID		4
#define S_ERRORCODESTRING	5
#define S_END			6
    int mainstate = S_ERRORTABLE;
    int spacestate = TRUE;
    int quotestate = FALSE;

    int id_number = 0, id_number_max = 0;
    long etablebase;
    int line;

   /* argument parsing, (c) jms 6/30/93 */

    int i, functionize = 0;

    for (i = 0; i < argc; i++)
	{
	  if (argv[i][0] == '-')
	    switch(argv[i][1])
		{
		  case '?':
			printf("usage: ecompile [-f] filename\n");
			exit(0);
		  case 'f':
			functionize = 1;
			break;
		}
	  else
	    filename = argv[i];
	}

    p = malloc (strlen (filename) + 5);
    strcpy (p, filename);
    filename = p;

    ename = p;
    len = strlen (ename);
    p += len - 3;
    if (strcmp (p, ".et"))
	p += 3;
    *p++ = '.';
    /* now p points to where "et" suffix should start */
    /* generate new filenames */
    strcpy (p, "c");
    strcpy (c_file, ename);
    *p = 'h';
    strcpy (h_file, ename);
    strcpy (p, "et");

    infile = fopen(filename, "r");
    if (!infile) {
	perror(filename);
	exit(1);
    }

    hfile = fopen(h_file, "w");
    if (hfile == (FILE *)NULL) {
    perror(h_file);
	exit(1);
    }

    cfile = fopen(c_file, "w");
    if (cfile == (FILE *)NULL) {
	perror(c_file);
	exit(1);
    }

    fprintf(cfile, "/*\n"
		   " * %s\n"
		   " * This file is the C file for %s.\n"
		   " * Please do not edit it as it is automatically generated.\n"
		   " */\n"
	           "\n"
		   "#ifndef WINDOWS\n"
		   "#define FAR\n"
		   "#else\n"
	           "#include <windows.h>\n"
		   "#endif\n"
                   "#define LPSTR char FAR *\n", c_file, ename);
/* changes for Windows -- jms 6/30/93 */

    fprintf(hfile, "/*\n"
		   " * %s\n"
		   " * This file is the #include file for %s.\n"
		   " * Please do not edit it as it is automatically generated.\n"
		   " */\n"
	           "\n", h_file, ename);

    if (functionize) /* all this stuff is new -- jms 6/30/93 */
	/* functional form of an error table entry */
	{
	/* different from the normal error table file... hmm...
	   sorry... */

	  bufp = buf;

/* finite state machine time! */
	  mainstate = S_ERRORTABLENAME;
	  while(mainstate != S_END)
		{
		  c = fgetc(infile);
		  if (c == '#')
		    while ((c != '\r') && (c != '\n') && (c != EOF))
		      c = fgetc(infile);

		  if (c != EOF)
		    *bufp++ = c;
		  else
		    mainstate = S_END;

		  if (bufp - buf >= BUFSZ)
		    {
		      fprintf(stderr, "token too long on line %d.\n", line);
		      exit(1);
		    }

		  if (!spacestate)
			{
			  if (c != EOF && !isspace(c))
			    continue;
			  spacestate = TRUE;
	    	/* something wonderful is about to happen... */
		/* (i.e. a token is stopping) */

			  *--bufp = '\0';
		/* capture the token in bufp. */
			  if (mainstate == S_ERRORTABLENAME)
			    {
				if (strlen(buf) > 4
			    	|| (etablebase = error_table_base(buf)) == -1)
			    	  {
			    	  	fprintf(stderr, "invalid error table name: %s\n", buf);
			    	  	exit(1);
			    	  }
			    	strcpy(etablename, buf);
			    	mainstate = S_ERRORCODE;
			    }
			  else /* mainstate == S_ERRORCODE or S_END */
			    {
				if (buf[0] == '!')
				{
					id_number = atoi(buf + 1);
					continue;
				}
			    	if (strlen(buf) > IDSZ || !valid_id_string(buf))
			    	{
			    		fprintf(stderr, "invalid ID: %s\n", buf);
			    		exit(1);
			    	}
			    	fprintf(hfile, "#define %-40s (%ldL)\n", buf,
			    	        etablebase + id_number);
				id_number++;
			    	if (id_number > id_number_max)
			    	  id_number_max = id_number;
			    }
			}
		  else
			{
				if (isspace(c))
				  continue;

				spacestate = FALSE;
		/* something wonderful is about to happen... */
		/* (i.e. a token is starting) */

				bufp = buf;
		/* mark beginning of token. */
				*bufp++ = c;
		/* the first character needs to be copied here. */
			}
		}

		if (id_number > id_number_max)
		  id_number_max = id_number;

	}
    else /* normal error table now */
    {

    fprintf(cfile, "static const char FAR * const text[] = {\n");

     bufp = buf;

/* finite state machine time! */
     while((c = fgetc(infile)) != EOF && mainstate != S_END)
	{
	  if (c == '#')
	  while ((c != '\r') && (c != '\n') && (c != EOF))
            c = fgetc(infile);
	  if (c == EOF)
	    break;

	  *bufp++ = c;
	  if (c == '\r' || c == '\n') /* jms 6/30/93 */
		line++;

	  if (quotestate)
	    {
		/* inside quotes. */
		if (c == '\n' || c == '\r')
	    	  {
		    fprintf(stderr, "invalid string on line %d.\n", line);
		    exit(1);
		  }
		if (c == '\"')
		  quotestate = FALSE;
           	continue;
	    }
	  if (spacestate)
	    {
		/* in white space. */
		if (isspace(c))
		  continue;

		spacestate = FALSE;
		/* something wonderful is about to happen... */
		/* (i.e. a token is starting) */

		bufp = buf;
		/* mark beginning of token. */
		*bufp++ = c;
		/* the first character needs to be copied here. */

		switch(mainstate)
		  {
		    case S_ERRORCODESTRING:
			if (c == '\"')
			  {
			    quotestate = TRUE;
			  }
			else
			  {
				fprintf(stderr,
			  	  "expecting a string on line %d.\n",
			          line);
				exit(1);
			  }
		  }
	    }
	  else if (isspace(c))
	    {
	    	/* something wonderful is about to happen... */
		/* (i.e. a token is stopping) */

	      spacestate = TRUE;
	      *--bufp = '\0';
		/* capture the token in bufp. */

		switch(mainstate)
		  {
		    case S_ERRORTABLE:
		    case S_ERRORCODE:
		      {
		      	char *expecting;

			if (mainstate == S_ERRORCODE && !strcasecmp(buf, "end"))
			  {
			    mainstate = S_END;
			    continue;
			  }

			expecting = mainstate == S_ERRORTABLE ?
			            str_error_table : str_error_code;

			if (strcasecmp(buf, expecting) &&
		            !(mainstate == S_ERRORCODE && !strcasecmp(buf, "ec")))
			  {
    			    fprintf(stderr, "unknown token %s. expecting \"%s\" on line %d.\n",
				  buf, expecting, line);
			    exit(1);
			  }
			/* proceed to next state! */
			mainstate++;
		      }
		      break;
		    case S_ERRORTABLENAME:
		      if (strlen(buf) > 4
		  	|| (etablebase = error_table_base(buf)) == -1)
			{
			  fprintf(stderr, "invalid error table name: %s\n", buf);
			  exit(1);
			}
		      strcpy(etablename, buf);
		      mainstate = S_ERRORCODE;
		      break;
		    case S_ERRORCODEID:
		      bufp--;
		      if (*bufp != ',')
			{
			  fprintf(stderr, "ID should be followed by a comma on line %d.\n",
			          line);
			  exit(1);
			}
		      *bufp = '\0';
		      if (strlen(buf) > IDSZ || !valid_id_string(buf))
			{
			  fprintf(stderr, "invalid ID: %s\n", buf);
			  exit(1);
			}
/* output a line like this to the .h file:

	#define KRB_ERROR			 (33303031L)

 */

		      fprintf(hfile, "#define %-40s (%ldL)\n", buf,
		              etablebase + id_number);
		      mainstate = S_ERRORCODESTRING;
		      break;
		    case S_ERRORCODESTRING:
		      if (quotestate)
			{
			  fprintf(stderr, "string not closed on line %d.\n",
			          line);
			  exit(1);
			}
		      fprintf(cfile, "\t%s,\n", buf);
		      id_number++;
		      id_number_max++;
		      mainstate = S_ERRORCODE;
		      break;
		  }
            }
	  else
	    {
		/* the continuing saga of the token */
		if (bufp - buf >= BUFSZ)
		  {
		    fprintf(stderr, "token too long on line %d.\n", line);
		    exit(1);
		  }

		switch(mainstate)
		  {
		    case S_ERRORTABLE:
		    case S_ERRORCODE:
		      if (bufp - buf > WTFLEN)
				/* both tokens are shorter than WTFLEN. */
			{
			  fprintf(stderr, "unknown token. expecting \"%s\"\n on line %d.",
			          mainstate == S_ERRORTABLE ? str_error_table
			             : str_error_code, line);
			  exit(1);
			}
		      break;
		    case S_ERRORCODESTRING:
		      if (quotestate == FALSE)
			{
			  fprintf(stderr, "invalid string on line %d.\n", line);
			  exit(1);
			}
		      break;
		  }
	    }
	}

	/* we should have hit an "end" by now. */
    if (mainstate != S_END)
	{
	  fprintf(stderr, "expecting \"end\".\n");
	  exit(1);
	}

    fprintf(cfile, "    0\n};\n\n");

    }

    /* now take care of the C file */

    fprintf(cfile, "typedef LPSTR (*err_func)(int, long);\n");

    fprintf(cfile, "struct error_table {\n"
		   "    char const FAR * const FAR * msgs;\n"
		   "    err_func func;\n"
		   "	long base;\n"
		   "	int n_msgs;\n"
		   "};\n"
	           "struct et_list {\n"
	           "	struct et_list *next;\n"
	           "	const struct error_table * table;\n"
	           "};\n");

    fprintf(cfile, /* "extern struct et_list *_et_list;\n\n" */ /* CRS 10/6/95 */
	           "static %s struct error_table et = "
	           "{ %s, (err_func)0, %ldL, %d };\n\n",
			functionize ? "" : "const",
			functionize ? "0" : "text",
	                etablebase, id_number_max);
/* changes made here -- jms 6/30/93 */

    fprintf(cfile, "static struct et_list link = { 0, 0 };\n\n"
		   "void initialize_%s_error_%s(%s) {\n", etablename,
			functionize ? "func" : "table",                                                         
		        functionize?"err_func func, struct et_list **__et_list"
		        : "struct et_list **__et_list");

    if (functionize)
    fprintf(cfile, "    et.func = func;\n");

    fprintf(cfile, "    if (!link.table) {\n"
		   "        link.next = *__et_list;\n"
		   "        link.table = &et;\n"
		   "        *__et_list = &link;\n"
		   "    }\n"
		   "}\n");
    fclose(cfile);

/* now do the .h file */

    fprintf(hfile, "extern void initialize_%s_error_%s(%s);\n"
		   "#define ERROR_TABLE_BASE_%s (%ldL)\n\n",
		etablename,
		functionize ? "func" : "table",
		functionize ? "err_func func, struct et_list **" : "struct et_list **",
	        etablename, etablebase);

    fprintf(hfile, "/* for compatibility with older versions... */\n"
		   "#define init_%s_err_%s initialize_%s_error_%s\n"
		   "#define %s_err_base ERROR_TABLE_BASE_%s\n",
		etablename, functionize ? "func(erf)" : "tbl()",
	        etablename, functionize ? "func(erf,&_et_list)" : "table(&_et_list)",
	        etablename, etablename);
    fclose(hfile);

    fclose(infile);

    return 0;
}

