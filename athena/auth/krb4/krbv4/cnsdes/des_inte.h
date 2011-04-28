/*
 * des_internal.h
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * Private include file for the Data Encryption Standard library.
 * This used to have more than one thing in it, but now that we're
 * using Fergusen's DES, that's all that is needed.
 */

/*
 * number of iterations of the inner
 * loop of the DES algorithm.  The
 * standard is 16, but in case that is
 * too slow, we might do less.  Of
 * course, less also means less
 * security.
 */
#define	 AUTH_DES_ITER   16

/*
 * This is needed now that we are including the DES routines into the
 * interface of an MS-Windows DLL.  The calling sequences of the functions
 * get changed based on their declarations, and we must make sure that
 * the declaration and definition match, by having the declaration reach
 * the definition. 
 */
#include "krb.h"
