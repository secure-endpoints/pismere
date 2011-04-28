/* Platform and runtime macros. Derived from Mac OS Universal Headers 3.2

/****************************************************************************************************

	TARGET_CPU_*
	These conditionals specify which microprocessor instruction set is being
	generated.  At most one of these is true, the rest are false.

		TARGET_CPU_PPC			- Compiler is generating PowerPC instructions
		TARGET_CPU_68K			- Compiler is generating 680x0 instructions
		TARGET_CPU_X86			- Compiler is generating x86 instructions
		TARGET_CPU_MIPS			- Compiler is generating MIPS instructions
		TARGET_CPU_SPARC		- Compiler is generating Sparc instructions
		TARGET_CPU_ALPHA		- Compiler is generating Dec Alpha instructions


	TARGET_OS_*
	These conditionals specify in which Operating System the generated code will
	run. At most one of the these is true, the rest are false.

		TARGET_OS_MAC			- Generate code will run under Mac OS
		TARGET_OS_WIN32			- Generate code will run under 32-bit Windows
		TARGET_OS_UNIX			- Generate code will run under some unix 


	TARGET_RT_*	
	These conditionals specify in which runtime the generated code will
	run. This is needed when the OS and CPU support more than one runtime
	(e.g. MacOS on 68K supports CFM68K and Classic 68k).

		TARGET_RT_LITTLE_ENDIAN	- Generated code uses little endian format for integers
		TARGET_RT_BIG_ENDIAN	- Generated code uses big endian format for integers 	
		TARGET_RT_MAC_CFM		- TARGET_OS_MAC is true and CFM68K or PowerPC CFM being used	
		TARGET_RT_MAC_68881		- TARGET_OS_MAC is true and 68881 floating point instructions used	


	PRAGMA_*
	These conditionals specify whether the compiler supports particular #pragma's
	
		PRAGMA_IMPORT 			- Compiler supports: #pragma import on/off/reset
		PRAGMA_ONCE  			- Compiler supports: #pragma once
		PRAGMA_STRUCT_ALIGN  	- Compiler supports: #pragma options align=mac68k/power/reset
		PRAGMA_STRUCT_PACK		- Compiler supports: #pragma pack(n)
		PRAGMA_STRUCT_PACKPUSH	- Compiler supports: #pragma pack(push, n)/pack(pop)
		PRAGMA_ENUM_PACK 		- Compiler supports: #pragma options(!pack_enums)
		PRAGMA_ENUM_ALWAYSINT 	- Compiler supports: #pragma enumsalwaysint on/off/reset
		PRAGMA_ENUM_OPTIONS		- Compiler supports: #pragma options enum=int/small/reset
		PRAGMA_MARK				- Compiler supports: #pragma mark

	FOUR_CHAR_CODE
	This conditional does the proper byte swapping to assue that a four character code (e.g. 'TEXT')
	is compiled down to the correct value on all compilers.

		FOUR_CHAR_CODE('abcd')	- Convert a four-char-code to the correct 32-bit value

	TYPE_*
	These conditionals specify whether the compiler supports particular types.

		TYPE_BOOL				- Compiler supports "bool"

****************************************************************************************************/


/****************************************************************************************************

	EXTERN_API*
	These conditionals are used to specify the calling convention of a function.
	Mac classic 68K and Win32 each have two possible calling conventions:

		EXTERN_API				- Classic 68K pascal, Win32 __cdecl
		EXTERN_API_C			- Classic 68K C,      Win32 __cdecl
		EXTERN_API_STDCALL		- Classic 68K pascal, Win32 __stdcall
		EXTERN_API_C_STDCALL	- Classic 68K C,      Win32 __stdcall


	CALLBACK_API*
	These conditionals are used to specify the calling convention of a function pointer.
	Mac classic 68K and Win32 each have two possible calling conventions:

		CALLBACK_API			- Classic 68K pascal, Win32 __cdecl
		CALLBACK_API_C			- Classic 68K C,      Win32 __cdecl
		CALLBACK_API_STDCALL	- Classic 68K pascal, Win32 __stdcall
		CALLBACK_API_C_STDCALL	- Classic 68K C,      Win32 __stdcall

****************************************************************************************************/

/* On Mac, we get all of those except for PRAGMA_MARK for free */
#include <ConditionalMacros.h>

#if defined(__MWERKS__)
	#define PRAGMA_MARK		1
#else
	#define PRAGMA_MARK		0
#endif

/****************************************************************************************************

	Debugging macros
	
	CCIDEBUG_ASSERT				- assertion macro (test a condition, display a message, and abort)
	CCIDEBUG_SIGNAL				- signal macro (display a message and abort)
	CCIDEBUG_VALIDPOINTER		- pointer validation function
	CCIDEBUG					- 1 if debugging inmformation should be compiled in

****************************************************************************************************/

#define	CCIDEBUG_ASSERT				Assert_
#define CCIDEBUG_SIGNAL				SignalCStr_
#define CCIDEBUG_VALIDPOINTER		ValidAddress

#if	MACDEV_DEBUG
#define CCI_DEBUG			1
#else
#define	CCI_DEBUG			0
#endif
