/*
 * $Source: /cvs/pismere/pismere/athena/auth/krb4/krbv4/include/krb_conf.h,v $
 * $Author: dalmeida $
 * $Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/include/krb_conf.h,v 1.1 1999/03/12 23:05:23 dalmeida Exp $ 
 *
 * Copyright 1988, 89, 90, 91, 92, 93, 94, 95, 96, 1997 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * This file contains configuration information for the Kerberos library
 * which is machine specific; currently, this file contains
 * configuration information for the vax, the "ibm032" (RT), and the
 * "PC8086" (IBM PC). 
 *
 * Note:  cross-compiled targets must appear BEFORE their corresponding
 * cross-compiler host.  Otherwise, both will be defined when running
 * the native compiler on the programs that construct cross-compiled
 * sources. 
 */

#ifndef KRB_CONF_DEFS
#define KRB_CONF_DEFS

#include <mit_copy.h>

/* Byte ordering */
extern int krbONE;
#define		HOST_BYTE_ORDER	(* (char *) &krbONE)
#define		MSB_FIRST		0	/* 68000, IBM RT/PC */
#define		LSB_FIRST		1	/* Vax, PC8086 */

#endif /* KRB_CONF_DEFS */
