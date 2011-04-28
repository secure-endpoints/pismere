/* il 7/24/95 -- adding nt */

#ifndef _nt_patch_h_
#define _nt_patch_h_

#if defined (WIN32) || defined (_WIN32)

/* the #undef instructions are not needed, but left to avoid possible
 * conflicts with windows.h's similar definitions */

// #undef FAR
// #undef far
// #undef _far
// #undef __far
//
// #define FAR
// #define far
// #define _far
// #define __far

// #undef __pascal
// #undef _pascal
// #undef PASCAL
//
// #define __stdcall
// #define _pascal __pascal
// #define PASCAL __pascal

#undef _osmajor
#undef _osmanor
#define _osmajor _winmajor
#define _osminor _winminor

#undef _fstrlen
#undef _fstrcat
#undef _fstrncpy
#undef _fmemcpy
#undef _fstrcmp
#define _fstrlen strlen
#define _fstrcat strcat
#define _fstrncpy strncpy
#define _fmemcpy memcpy
#define _fstrcmp strcmp

#undef WINDOWS
#define WINDOWS

#if defined( _DLL ) || defined (_MT)
#undef _WINDLL
#define _WINDLL
#endif

#endif // _WIN32

#endif // _nt_patch_h_
