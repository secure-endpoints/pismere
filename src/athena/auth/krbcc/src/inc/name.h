#pragma once

#ifdef _WIN64
#define CCAPI_MODULE "krbcc64"
#else
#define CCAPI_MODULE "krbcc32"
#endif
#define CCAPI_DLL CCAPI_MODULE ".dll"
#define CCAPI_EXE CCAPI_MODULE "s.exe"
