/* $Copyright:
 *
 * Copyright 1998-2000 by the Massachusetts Institute of Technology.
 * 
 * All rights reserved.
 * 
 * Export of this software from the United States of America may require a
 * specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and distribute
 * this software and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice appear in all
 * copies and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of M.I.T. not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Furthermore if you
 * modify this software you must label your software as modified software
 * and not distribute it in such a fashion that it might be confused with
 * the original MIT software. M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Individual source code files are copyright MIT, Cygnus Support,
 * OpenVision, Oracle, Sun Soft, FundsXpress, and others.
 * 
 * Project Athena, Athena, Athena MUSE, Discuss, Hesiod, Kerberos, Moira,
 * and Zephyr are trademarks of the Massachusetts Institute of Technology
 * (MIT).  No commercial use of these trademarks may be made without prior
 * written permission of MIT.
 * 
 * "Commercial use" means use of a name in a product or other for-profit
 * manner.  It does NOT prevent a commercial firm from referring to the MIT
 * trademarks in order to convey information (although in doing so,
 * recognition of their trademark status should be given).
 * $
 */

/* $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/CCache.CFM.cp,v 1.4 2000/06/16 21:44:05 dalmeida Exp $ */

/*************************************************************
 *
 *		  Implementation of Credential Cache API for MacOS
 *		
 *		 -= MacOS Code Fragment Manager Specific Routines =- 
 *
 *	Revision 1: Frank Dabek 6/4/98
 *				-Initial Revision
 *	
 *
 *************************************************************/

#include <TicketKeeperLib.h>

#include <Notification.h>
#include <Errors.h>

#include <CFM.InitTerm.h>

#include "CCache.config.h"

#include "CCache.h"
#include "CCache.debug.h"
#include "CCacheUtil.h"

// -- Declarations ------

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
#	pragma import on
#endif

pascal OSErr __initialize_CCacheLib (
	CFragInitBlockPtr inInitBlock);
pascal void __terminate_CCacheLib (void);

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
#	pragma import reset
#endif

void NotifyUser (
	CFragInitBlockPtr	initBlock);

Str255	kNotificationString = "\pMIT Ticket Keeper is not running. This may cause problems with the MIT Kerberos Library. Please open the Kerberos Manager control panel immediately to fix this.";
OSType	kKTMSignature = 'KrbM';

NMRec	gNotification;
Boolean	gHaveNotification = false;

Boolean CurrentProcessIsKTM (void);

// -- Routines ------

pascal OSErr __initialize_CCacheLib (
	CFragInitBlockPtr	initBlock) {

	OSErr err = noErr;
	Boolean tkRunning = false;
	Boolean inTicketManager = false;
	
	// Call MW init routine
	err = __initialize(initBlock);

	if (err != noErr)
		return err;
	
	// Make sure Ticket Keeper is running. Post notification if not, except when we are 
	// in ticket manager
	
	tkRunning = IsTicketKeeperRunning (nil);
	inTicketManager = CurrentProcessIsKTM ();
	
	if (!tkRunning && !inTicketManager) {
		NotifyUser (initBlock);
	}

	return noErr;
}

pascal void __terminate_CCacheLib (void) {

	if (gHaveNotification) {
		NMRemove (&gNotification);
	}

	__terminate();
}

/*
 * Post a notification about the error
 */

void NotifyUser (
	CFragInitBlockPtr	initBlock)
{
#pragma unused (initBlock) 		// in case we do use resources in the future

	/* 
	 * The notification string should be moved into a resource... but I don't feel
	 * like dealing with shlib resources just for that
	 */
	 
	gNotification.qType = nmType;
	gNotification.nmMark = 0;
	gNotification.nmIcon = nil;
	gNotification.nmSound = nil;
	gNotification.nmStr = kNotificationString;
	
	gNotification.nmResp = (NMUPP) -1;
	
	NMInstall (&gNotification);
	gHaveNotification = true;
}

Boolean CurrentProcessIsKTM (void)
{
	ProcessSerialNumber	currentProcess;
	ProcessInfoRec		processInfo;
	
	OSErr	err = GetCurrentProcess (&currentProcess);
	
	if (err != noErr)
		return false;
		
	processInfo.processInfoLength = sizeof (processInfo);
	processInfo.processName = nil;
	processInfo.processAppSpec = nil;
	
	err = GetProcessInformation (&currentProcess, &processInfo);
	if (err != noErr)
		return false;
	
	if (processInfo.processSignature == kKTMSignature)
		return true;
	
	return false;
}