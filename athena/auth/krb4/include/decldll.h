/*
 * DECLDLL.H
 */

#if !defined (__DECLDLL__H)
#define __DECLDLL__H

#if !defined (_WIN32)
#   define EXPORT __export
#   define EXPORT32
#else
#   define EXPORT
#   if defined (DLL) || defined (_DLL) || defined (_WINDLL) || defined (_MT)
#      define EXPORT32 __declspec(dllexport)
#   else
#      define EXPORT32 __declspec(dllimport)
#   endif
#endif

#endif // __DECLDLL__H
