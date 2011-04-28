#ifndef _BUILD_INC_VER_INC_
#include <ver.def.inc>
#ifdef BUILD_OFFICIAL
#if defined(BUILD_KFW)
#include <ver/ver.kfw.inc>
#elif defined(BUILD_WINZEPHYR)
#include <ver/ver.winzephyr.inc>
#endif
#endif
#include <ver.rc.inc>
#endif
