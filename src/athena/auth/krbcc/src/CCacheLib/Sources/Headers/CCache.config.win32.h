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

#pragma once

#define CCache_v2_compat		1
#define CCache_ContainsSharedStaticData	1

#pragma warning (disable: 4786)

#define PRAGMA_MSG_STRING(x) #x
#define PRAGMA_MSG_APPLY(m, x) m(x)
#define PRAGMA_MSG_LINE PRAGMA_MSG_APPLY(PRAGMA_MSG_STRING, __LINE__)
#define PRAGMA_MSG_HEADER __FILE__ "(" PRAGMA_MSG_LINE ") : "

#define PRAGMA_MSG_REMINDER PRAGMA_MSG_HEADER "REMINDER: "
#define PRAGMA_MSG_WARNING PRAGMA_MSG_HEADER "WARNING: "

#define CCIMessage_Warning_ PRAGMA_MSG_WARNING

#ifndef NDEBUG
#define CCI_DEBUG 1
#else
#define CCI_DEBUG 0
#endif

/*****************************************************************************/
#include <windows.h>
#include <assert.h>

#define D_MACRO_STRING(x)        #x
#define D_MACRO_APPLY(macro, x)  macro(x)
#define D_LINE                   D_MACRO_APPLY(D_MACRO_STRING, __LINE__)

#define DEBUG_PRINT_ALWAYS(x) \
    OutputDebugString x
#define DEBUG_PRINT_ASSERT_MSG(x) \
    DEBUG_PRINT_ALWAYS(("ASSERT FAILED: " x \
                        "\n\tLine: " D_LINE "\n\tFile: " __FILE__ "\n"))
#define DEBUG_ASSERT_ALWAYS(x) \
    do { if (!(x)) DEBUG_PRINT_ASSERT_MSG(#x); assert(x); } while (0)
#define DEBUG_ASSERT_PRINT_ONLY(x) \
    do { if (!(x)) DEBUG_PRINT_ASSERT_MSG(#x); } while (0)

#ifdef NDEBUG
#define DEBUG_PRINT(x)
#define DEBUG_ASSERT(x) DEBUG_ASSERT_PRINT_ONLY(x)
#else
#define DEBUG_PRINT(x)  DEBUG_PRINT_ALWAYS(x)
#define DEBUG_ASSERT(x) DEBUG_ASSERT_ALWAYS(x)
#endif
/*****************************************************************************/

#define CCIDEBUG_ASSERT DEBUG_ASSERT
#define CCIDEBUG_VALIDPOINTER(x) true
#define CCIDEBUG_SIGNAL(x) assert(0 && x)

 
#include <new> //added by pilawa for .NET build. Fixes error 'redefinition; different linkage'
	namespace std {
		typedef ::size_t size_t;
		typedef void (*new_handler)(void);
		 inline new_handler set_new_handler(new_handler) throw() {
			return 0;
			};  
		
}
